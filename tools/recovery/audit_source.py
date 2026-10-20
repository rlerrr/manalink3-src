"""Audit source provenance and literal bytes against cached recovery inputs."""
import hashlib
import json
import re
import struct
from pathlib import Path

from inspect_release import Image
from promote_recovery import LITERAL, strip_comments, aggregate_stack_aliases, project_extension_keywords, project_replicate_count, project_extension_flags
from recover_literals import recover_strings, readonly_string, byte_literal


def literal_tokens(text):
    text = strip_comments(text)
    values, end = [], -1
    for match in re.finditer(LITERAL, text):
        token = match[0]
        if values and token[0] == '"' and values[-1][0] == '"' and not text[end:match.start()].strip():
            values[-1] = ('"', values[-1][1] + token[1:-1])
        else:
            values.append((token[0], token[1:-1]))
        end = match.end()
    return values


def function_body(source, name):
    source = strip_comments(source)
    masked = re.sub(LITERAL, lambda m: " " * len(m[0]), source)
    match = re.search(r"\b" + re.escape(name) + r"\s*\([^;{}]*\)\s*\{", masked)
    if not match:
        raise ValueError("Missing function definition")
    level = 1
    for at in range(match.end(), len(masked)):
        if masked[at] == "{":
            level += 1
        elif masked[at] == "}":
            level -= 1
            if level == 0:
                return source[match.start():at + 1]
    raise ValueError("Unclosed function body")


def main():
    root = Path(__file__).resolve().parents[2]
    analysis = root / ".recovery"
    path = root / "docs/recovery-evidence/recovered-functions.json"
    manifest = json.loads(path.read_text())
    image = Image((analysis / "2022-ManalinkEh.dll").read_bytes())
    sources = {p:(root / p).read_bytes().decode("utf-8", errors="replace") for p in manifest["source_files"]}
    issues = []
    recovered_strings = 0
    literal_pointer_stores = 0
    literal_constants = 0
    def machine_bytes(address, length):
        rva = address - image.pe.OPTIONAL_HEADER.ImageBase
        for section in image.pe.sections:
            if section.VirtualAddress <= rva < section.VirtualAddress + section.Misc_VirtualSize:
                offset = section.PointerToRawData + rva - section.VirtualAddress
                return image.data[offset:offset + length]
        raise ValueError("Machine literal load outside runtime sections")
    for function in manifest["accepted_functions"]:
        try:
            raw = (analysis / "decompiled-abi" / (function["address"] + ".c")).read_text(encoding="utf-8")
            if hashlib.sha256(raw.encode()).hexdigest() != function["raw_sha256"]:
                raise ValueError("Raw export hash mismatch")
            for support in function.get("supporting_exports", []):
                support_raw = (analysis / support.get('raw_export', 'decompiled-abi/' + support['address'] + '.c')).read_text(encoding="utf-8")
                if hashlib.sha256(support_raw.encode()).hexdigest() != support["raw_sha256"]:
                    raise ValueError("Supporting export hash mismatch")
            evidence = function.get("recovered_literals", [])
            aliases = {e["symbol"]:int(e["address"], 16) for e in evidence if e["symbol"] == "_rdata"}
            for entry in evidence:
                if readonly_string(image, int(entry["address"], 16)) != bytes.fromhex(entry["bytes"]):
                    raise ValueError("Read-only string byte mismatch")
            literal_input = strip_comments(raw)
            for entry in function.get("reconstructed_literal_constants", []):
                # Manually reconstructed VLA call frames can lose typed string
                # arguments. The raw pointer spelling, read-only bytes and all
                # recorded immediate materializations must agree. These are
                # explicit per-function reconstructions, never a global cast.
                from capstone import Cs, CS_ARCH_X86, CS_MODE_32
                from capstone.x86_const import X86_OP_IMM
                address = int(entry['address'], 16)
                token = '&' + entry['symbol']
                literal = readonly_string(image, address)
                if (entry['symbol'] not in {f'UNK_{address:08x}', '_rdata'} or
                    (entry['symbol'] == '_rdata' and entry['raw_occurrences'] != 1) or
                    literal != bytes.fromhex(entry['bytes']) or
                    literal_input.count(token) != entry['raw_occurrences'] or
                    not entry['materializations']):
                    raise ValueError('VLA literal pointer provenance mismatch')
                for site in entry['materializations']:
                    at, code = int(site['address'],16), bytes.fromhex(site['bytes'])
                    decoder = Cs(CS_ARCH_X86, CS_MODE_32);decoder.detail = True
                    instructions = list(decoder.disasm(code, at))
                    if (machine_bytes(at,len(code)) != code or len(instructions) != 1 or
                        not int(function['address'],16) <= at < int(entry['function_end'],16)):
                        raise ValueError('VLA literal instruction mismatch')
                    ins = instructions[0]
                    if (ins.mnemonic != 'mov' or ins.operands[1].type != X86_OP_IMM or
                        ins.operands[1].imm != address or ins.operands[0].size != 4):
                        raise ValueError('VLA literal immediate mismatch')
                literal_input = literal_input.replace(token,byte_literal(literal))
                literal_constants += entry['raw_occurrences']
            for entry in function.get("reconstructed_literal_pointers", []):
                # Typed pointer fields may still print as &UNK. Require each
                # materialization to be a four-byte immediate MOV to memory,
                # inside this function, pointing at the exact read-only bytes.
                from capstone import Cs, CS_ARCH_X86, CS_MODE_32
                from capstone.x86_const import X86_OP_MEM, X86_OP_IMM
                address = int(entry["address"], 16)
                literal = readonly_string(image, address)
                token = '&' + entry["symbol"]
                if (entry["symbol"] != f'UNK_{address:08x}' or
                    literal != bytes.fromhex(entry["bytes"]) or
                    literal_input.count(token) != len(entry["materializations"])):
                    raise ValueError("Reconstructed literal pointer provenance mismatch")
                for site in entry["materializations"]:
                    at, code = int(site["address"], 16), bytes.fromhex(site["bytes"])
                    decoded = Cs(CS_ARCH_X86, CS_MODE_32); decoded.detail = True
                    instructions = list(decoded.disasm(code, at))
                    if (machine_bytes(at, len(code)) != code or len(instructions) != 1 or
                        not int(function["address"], 16) <= at < int(entry["function_end"], 16)):
                        raise ValueError("Reconstructed literal pointer machine range mismatch")
                    ins = instructions[0]
                    if (ins.mnemonic != 'mov' or ins.operands[0].type != X86_OP_MEM or
                        ins.operands[0].size != 4 or ins.operands[1].type != X86_OP_IMM or
                        ins.operands[1].imm != address):
                        raise ValueError("Reconstructed literal pointer MOV mismatch")
                literal_input = literal_input.replace(token, byte_literal(literal))
                literal_pointer_stores += len(entry["materializations"])
            for entry in function.get("reconstructed_literal_loads", []):
                # VLA stack state can obscure a direct constant argument. Accept
                # its explicit reconstruction only with the x86 ESP-relative
                # argument store and relative CALL bytes verified independently.
                address = int(entry["address"], 16)
                argument = entry["argument"]
                if not 0 <= argument < 32:
                    raise ValueError("Unsupported reconstructed literal argument")
                load = b'\xc7\x44\x24' + bytes([argument * 4]) + struct.pack('<I', address)
                load_at = int(entry["load_address"], 16)
                call_at = int(entry["call_address"], 16)
                target = int(entry["call_target"], 16)
                call = b'\xe8' + struct.pack('<i', target - call_at - 5)
                if (load.hex() != entry["load_bytes"] or machine_bytes(load_at, len(load)) != load or
                    call.hex() != entry["call_bytes"] or machine_bytes(call_at, len(call)) != call or
                    not load_at + len(load) <= call_at < load_at + 64):
                    raise ValueError("Reconstructed literal machine argument mismatch")
                literal = readonly_string(image, address)
                if literal != bytes.fromhex(entry["bytes"]):
                    raise ValueError("Reconstructed literal byte mismatch")
                token = '&' + entry["symbol"]
                if literal_input.count(token) != 1:
                    raise ValueError("Reconstructed literal raw load is ambiguous")
                literal_input = literal_input.replace(token, byte_literal(literal))
                recovered_strings += 1
            expected, resolved = recover_strings(literal_input, image, aliases)
            if resolved != evidence:
                raise ValueError("Literal argument provenance mismatch")
            recovered_strings += len(evidence)
            source = sources[function["source"]]
            marker = "/* ManalinkEh 2022: 0x" + function["address"] + ";"
            if source.count(marker) != 1:
                raise ValueError("Missing or repeated source provenance marker")
            body = function_body(source[source.index(marker):], function["name"])
            token_core = function.get('token_core_evidence')
            if token_core and hashlib.sha256(machine_bytes(int(function['address'],16), token_core['machine_bytes'])).hexdigest() != token_core['machine_sha256']:
                raise ValueError('Token core machine hash mismatch')
            token_interface = function.get('token_interface_2022')
            if token_interface:
                from promote_recovery import TOKEN_API_NAMES
                if token_interface['type'] != 'token_generation_2022_t' or not re.search(r'\btoken_generation_2022_t\b',body):
                    raise ValueError('Versioned token caller type mismatch')
                for original,replacement in token_interface['calls'].items():
                    if TOKEN_API_NAMES.get(original) != replacement or not re.search(r'\b'+replacement+r'\s*\(',body):
                        raise ValueError('Versioned token caller API mismatch')
                for action in token_interface['actions']:
                    if action not in raw or action.replace('TOKEN_ACTION_','TOKEN2022_ACTION_') not in body:
                        raise ValueError('Versioned token action name mismatch')
            yorion = function.get('yorion_evidence')
            if yorion:
                if hashlib.sha256(machine_bytes(int(function['address'], 16), yorion['machine_bytes'])).hexdigest() != yorion['machine_sha256']:
                    raise ValueError('Yorion/predicate machine hash mismatch')
                zero_fill = yorion.get('zero_fill')
                if zero_fill:
                    block = zero_fill['original']
                    if (zero_fill['bytes'] != 302 or hashlib.sha256(block.encode()).hexdigest() != zero_fill['original_sha256'] or
                        expected.count(block) != 1 or block.count("'\\0'") != 8 or
                        zero_fill['replacement'].strip() != 'memset(flicker_array, 0, sizeof(flicker_array));'):
                        raise ValueError('Yorion zero-fill reconstruction provenance mismatch')
                    expected = expected.replace(block, zero_fill['replacement'])
            native_string = function.get('native_string_view')
            if native_string:
                address = int(native_string['address'],16)
                argument = native_string['argument']
                expected_load = b'\xc7\x44\x24' + bytes([argument * 4]) + struct.pack('<I',address)
                if (native_string['type'] != 'const char *' or
                    expected_load.hex() != native_string['load_bytes'] or
                    machine_bytes(int(native_string['load_address'],16),len(expected_load)) != expected_load):
                    raise ValueError('Native runtime-string argument mismatch')
            masks = function.get('state_mask_evidence')
            if masks:
                if (masks['raw_sha256'] != function['raw_sha256'] or masks['source'] != function['source'] or
                    masks['unit_evidence']['state_member']['offset'] != 8 or
                    masks['unit_evidence']['state_member']['bytes'] != 4):
                    raise ValueError('State-mask type provenance mismatch')
                for replacement in masks['replacements']:
                    match = re.fullmatch(r'\b\w+->state\s*[&|]\s*(0x[0-9a-fA-F]+)', replacement['original'])
                    if (not match or int(match[1],16) != replacement['mask'] or
                        sum(replacement['bits'].values()) != replacement['mask']):
                        raise ValueError('State-mask value mismatch')
                    for name,number in replacement['bits'].items():
                        if masks['unit_evidence']['enumerators'][name]['value'] != number:
                            raise ValueError('State-mask enumerator mismatch')
                    for site in replacement['machine_sites']:
                        code=bytes.fromhex(site['bytes'])
                        if machine_bytes(int(site['address'],16),len(code)) != code:
                            raise ValueError('State-mask instruction mismatch')
            if function.get("reconstruction_body_sha256") and hashlib.sha256(
                    body.replace("\r\n", "\n").encode()).hexdigest() != function["reconstruction_body_sha256"]:
                raise ValueError("Manual reconstruction body hash mismatch")
            keyword_proof = function.get('keyword_interface_evidence') or function.get('keyword_worker_repair')
            if keyword_proof:
                address = int(function['address'], 16)
                if hashlib.sha256(machine_bytes(address, keyword_proof['bytes'])).hexdigest() != keyword_proof['machine_sha256']:
                    raise ValueError('Keyword helper machine hash mismatch')
                for branch in keyword_proof['worker_branches']:
                    code = bytes.fromhex(branch['bytes'])
                    if machine_bytes(int(branch['address'], 16), len(code)) != code:
                        raise ValueError('Keyword worker branch mismatch')
            keyword_masks = function.get('keyword_mask_evidence')
            if keyword_masks:
                from recover_keyword_masks import ARGUMENTS
                if keyword_masks['raw_sha256'] != function['raw_sha256'] or keyword_masks['source'] != function['source']:
                    raise ValueError('Keyword-mask raw provenance mismatch')
                for replacement in keyword_masks['replacements']:
                    if int(replacement['literal'],16) != replacement['mask'] or sum(replacement['bits'].values()) != replacement['mask']:
                        raise ValueError('Keyword-mask numeric value mismatch')
                    if replacement['context'] == 'argument':
                        if ARGUMENTS[replacement['function']] != (replacement['arity'], replacement['argument']):
                            raise ValueError('Keyword-mask argument context mismatch')
                    elif replacement['context'] == 'extension_word':
                        _, projected = project_extension_keywords(strip_comments(raw))
                        if not any(replacement['original'].startswith(p['original']) and p['extension_offset'] == 0 for p in projected):
                            raise ValueError('Keyword-mask extension context mismatch')
                    else:
                        raise ValueError('Unsupported keyword-mask context')
                    for name,number in replacement['bits'].items():
                        if keyword_masks['enumerators'][name]['value'] != number:
                            raise ValueError('Keyword-mask enumerator mismatch')
                    for site in replacement['machine_sites']:
                        code = bytes.fromhex(site['bytes'])
                        if machine_bytes(int(site['address'],16),len(code)) != code:
                            raise ValueError('Keyword-mask machine operand mismatch')
            dispatch = function.get('protection_dispatch_evidence')
            if dispatch:
                address = int(function['address'],16)
                if hashlib.sha256(machine_bytes(address,dispatch['bytes'])).hexdigest() != dispatch['machine_sha256']:
                    raise ValueError('Protection dispatcher machine hash mismatch')
                for site in dispatch['machine_sites']:
                    code = bytes.fromhex(site['bytes'])
                    if machine_bytes(int(site['address'],16),len(code)) != code:
                        raise ValueError('Protection dispatcher instruction mismatch')
            lookup = function.get('keyword_lookup_evidence')
            if lookup and hashlib.sha256(machine_bytes(int(function['address'],16),lookup['bytes'])).hexdigest() != lookup['machine_sha256']:
                raise ValueError('Keyword lookup machine hash mismatch')
            if lookup:
                for table in lookup.get('jump_tables',[]):
                    data=bytes.fromhex(table['bytes'])
                    if machine_bytes(int(table['address'],16),len(data)) != data:
                        raise ValueError('Keyword lookup jump-table mismatch')
            replicate=function.get('replicate_worker_evidence')
            if replicate:
                if hashlib.sha256(machine_bytes(0x252b8d0,0x110)).hexdigest()!=replicate['machine_sha256']:
                    raise ValueError('Replicate worker machine hash mismatch')
                for address,expected_hex in replicate['machine_checks'].items():
                    data=bytes.fromhex(expected_hex)
                    if machine_bytes(int(address,16),len(data))!=data:
                        raise ValueError('Replicate ABI/storage instruction mismatch')
            replicate_projections=function.get('extension_replicate_projections')
            if replicate_projections:
                _,expected_replicate_projections=project_replicate_count(strip_comments(raw))
                foundation=json.loads((root/'docs/recovery-evidence/extension-foundation.json').read_text())
                member=next(m for m in foundation['types']['card_ext_t']['members'] if m['name']=='times_replicate_paid')
                if replicate_projections!=expected_replicate_projections or member['offset']!=159 or member['bytes']!=1:
                    raise ValueError('Replicate count field provenance mismatch')
            packed_views = function.get('packed_information_value_views')
            if packed_views:
                layout=json.loads((root/'docs/recovery-evidence/extension-foundation.json').read_text())['types']['inst_info_t']
                if layout['bytes']!=8 or {m['name']:(m['offset'],m['bytes']) for m in layout['members']}!={'id':(0,2),'data2':(2,2),'data':(4,4)}:
                    raise ValueError('Packed information field layout mismatch')
                for local,count in packed_views.items():
                    if (not re.search(r'\binst_info_t\s+'+re.escape(local)+r'\s*;',raw) or
                        len(re.findall(r'\(ulonglong\)\s*'+re.escape(local)+r'\b',raw)) != count):
                        raise ValueError('Packed information scalar-view provenance mismatch')
            flag_projection=function.get('extension_flag_projections')
            flags=function.get('extension_flags_evidence')
            if flags:
                if hashlib.sha256(machine_bytes(int(function['address'],16),flags['bytes'])).hexdigest()!=flags['machine_sha256']:
                    raise ValueError('Extension flag helper machine hash mismatch')
                for site in flags['member_sites']:
                    data=bytes.fromhex(site['bytes'])
                    if machine_bytes(int(site['address'],16),len(data))!=data:
                        raise ValueError('Extension flag member operand mismatch')
                for complement in flags['complements']:
                    mask=0
                    for number in complement['enumerators'].values():mask|=number&0xffffffff
                    if mask!=0xffffffff or complement['original'] not in raw:
                        raise ValueError('Extension flag complement provenance mismatch')
                flag_projection=flags['projections']
            if flag_projection:
                _,projected=project_extension_flags(strip_comments(raw))
                layout=json.loads((root/'docs/recovery-evidence/extension-foundation.json').read_text())['types']['card_ext_t']
                members={m['name']:(m['offset'],m['bytes']) for m in layout['members']}
                if flag_projection!=projected:
                    raise ValueError('Extension flag field projection mismatch')
                for projection in flag_projection:
                    if members[projection['replacement'].rsplit('->',1)[1]]!=(projection['extension_offset'],4):
                        raise ValueError('Extension flag member layout mismatch')
            migrated_flags=function.get('extension_flag_call_migration')
            if migrated_flags:
                if migrated_flags['body_sha256']!=function['reconstruction_body_sha256']:
                    raise ValueError('Extension flag caller body hash mismatch')
                for callee in migrated_flags['callees']:
                    inline=callee.get('inline_read')
                    if inline:
                        if (inline['raw_expression'] not in raw or inline['instance_offset']!=44 or inline['member_offset']!=188 or
                            inline['test_byte_offset']!=189 or inline['mask']!=512):
                            raise ValueError('Inline extension flag read provenance mismatch')
                        for address,expected_hex in inline['machine_checks'].items():
                            data=bytes.fromhex(expected_hex)
                            if machine_bytes(int(address,16),len(data))!=data:
                                raise ValueError('Inline extension flag instruction mismatch')
                    for call in callee['calls']:
                        address=int(call['address'],16);data=bytes.fromhex(call['bytes'])
                        if (len(data)!=5 or data[0] not in {0xe8,0xe9} or machine_bytes(address,5)!=data or
                            address+5+struct.unpack('<i',data[1:])[0]!=int(callee['target'],16)):
                            raise ValueError('Extension flag callee proof mismatch')
            card_table=function.get('card_table_evidence')
            if card_table:
                data=bytes.fromhex(card_table['bytes'])
                if (machine_bytes(int(card_table['table_address'],16),len(data))!=data or
                    struct.pack('<'+'i'*len(card_table['values']),*card_table['values'])!=data or
                    card_table['declaration'] not in strip_comments(sources[function['source']])):
                    raise ValueError('Copied card-table initializer mismatch')
                if hashlib.sha256(machine_bytes(int(function['address'],16),card_table['machine_bytes'])).hexdigest()!=card_table['machine_sha256']:
                    raise ValueError('Card-table function machine hash mismatch')
                for address,expected_bytes in card_table['machine_checks'].items():
                    data=bytes.fromhex(expected_bytes)
                    if machine_bytes(int(address,16),len(data))!=data:
                        raise ValueError('Card-table copy/access instruction mismatch')
            color_part = function.get('basic_color_part_evidence')
            if color_part:
                if hashlib.sha256(machine_bytes(int(function['address'], 16), color_part['machine_bytes'])).hexdigest() != color_part['machine_sha256']:
                    raise ValueError('Basic-land color part/caller machine hash mismatch')
                if color_part['partial_helper']:
                    if function['name'] != 'basic_subtype_to_color_part_2022' or not function['static']:
                        raise ValueError('Partial basic-land color helper interface mismatch')
                else:
                    if len(re.findall(r'\bbasic_subtype_to_color_part_2022\s*\(', body)) != len(color_part['calls']):
                        raise ValueError('Partial basic-land color caller count mismatch')
                    for call in color_part['calls']:
                        at, code = int(call['address'], 16), bytes.fromhex(call['bytes'])
                        if (len(code) != 5 or code[0] != 0xe8 or machine_bytes(at, 5) != code or
                            at + 5 + struct.unpack('<i', code[1:])[0] != 0x23ed750 or call['target'] != '23ed750'):
                            raise ValueError('Partial basic-land color callee mismatch')
            chooser = function.get('mana_chooser_evidence')
            if chooser:
                if hashlib.sha256(machine_bytes(int(function['address'], 16), chooser['machine_bytes'])).hexdigest() != chooser['machine_sha256']:
                    raise ValueError('Mana chooser machine hash mismatch')
                tail = chooser['tail']
                tail_bytes = bytes.fromhex(tail['bytes'])
                jump_at = int(tail['jump_address'], 16)
                if (machine_bytes(int(tail['address'], 16), len(tail_bytes)) != tail_bytes or
                    tail_bytes[-5] != 0xe9 or jump_at + 5 + struct.unpack('<i', tail_bytes[-4:])[0] != int(tail['target'], 16) or
                    tail['target'] != '4721c0' or 'const char *prompt' not in body):
                    raise ValueError('Mana chooser native tail-call/interface mismatch')
                native = chooser['native_dialog']
                executable = Image((analysis / '2022-Magic.exe').read_bytes())
                native_code = executable.pe.get_data(int(native['address'], 16) - executable.pe.OPTIONAL_HEADER.ImageBase, native['bytes'])
                if (hashlib.sha256(executable.data).hexdigest() != native['executable_sha256'] or
                    hashlib.sha256(native_code).hexdigest() != native['machine_sha256']):
                    raise ValueError('Native mana dialog machine hash mismatch')
            mana_counter=function.get('mana_counter_evidence')
            if mana_counter:
                if hashlib.sha256(machine_bytes(int(function['address'],16),mana_counter['machine_bytes'])).hexdigest()!=mana_counter['machine_sha256']:
                    raise ValueError('Mana count pass machine hash mismatch')
                for address,expected_hex in mana_counter['machine_checks'].items():
                    data=bytes.fromhex(expected_hex)
                    if machine_bytes(int(address,16),len(data))!=data:
                        raise ValueError('Mana count VLA/reset/call-frame instruction mismatch')
                linker=(root/'manalink.lds').read_text()
                for name,alias in mana_counter['native_aliases'].items():
                    if not re.search(r'(?m)^_'+name+r'\s*=\s*0x'+alias['address']+r';',linker,re.I):
                        raise ValueError('Typed native mana-list linker binding mismatch')
            production=function.get('mana_production_evidence')
            if production:
                if hashlib.sha256(machine_bytes(int(function['address'],16),production['machine_bytes'])).hexdigest()!=production['machine_sha256']:
                    raise ValueError('Mana production function machine hash mismatch')
                for site in production['mask_sites']:
                    data=bytes.fromhex(site['bytes'])
                    if machine_bytes(int(site['address'],16),len(data))!=data:
                        raise ValueError('Mana producer typed flag instruction mismatch')
                if production['prompt_read_only'] and not re.search(r'\bconst\s+char\s*\*\s*prompt\b',body):
                    raise ValueError('Mana prompt lost read-only qualifier')
            mana_migration=function.get('mana_call_migration')
            if mana_migration:
                if mana_migration['body_sha256']!=function['reconstruction_body_sha256']:
                    raise ValueError('Mana caller body hash mismatch')
                for callee in mana_migration['callees']:
                    for call in callee['calls']:
                        address=int(call['address'],16);data=bytes.fromhex(call['bytes'])
                        if (len(data)!=5 or data[0] not in {0xe8,0xe9} or machine_bytes(address,5)!=data or
                            address+5+struct.unpack('<i',data[1:])[0]!=int(call['target'],16) or call['target'] not in callee['targets']):
                            raise ValueError('Mana producer/count callee provenance mismatch')
            query_repair=function.get('keyword_id_query_repair')
            if query_repair:
                for change in query_repair['changes']:
                    expected_target={'get_special_abilities':0x26df840,'check_for_special_ability':0x26df890}[change['original']]
                    for call in change['machine_calls']:
                        address=int(call['address'],16);data=bytes.fromhex(call['bytes'])
                        if (len(data)!=5 or data[0]!=0xe8 or machine_bytes(address,5)!=data or
                            address+5+struct.unpack('<i',data[1:])[0]!=expected_target):
                            raise ValueError('Card-ID query callee provenance mismatch')
            projections = function.get('extension_keyword_projections')
            if projections:
                _, expected_projections = project_extension_keywords(strip_comments(raw))
                if projections != expected_projections:
                    raise ValueError('Extension keyword projection provenance mismatch')
                foundation = json.loads((root / 'docs/recovery-evidence/extension-foundation.json').read_text())
                members = {m['name']: m for m in foundation['types']['card_ext_t']['members']}
                if not foundation['source_recovered']:
                    raise ValueError('Extension foundation is not recovered')
                for projection in projections:
                    member = members[projection['replacement'].rsplit('->', 1)[1]]
                    if (projection['instance_offset'] != 44 or projection['bytes'] != 4 or
                        member['offset'] != projection['extension_offset'] or member['bytes'] != 4):
                        raise ValueError('Extension keyword layout mismatch')
            if aggregate_stack_aliases(body):
                raise ValueError("Cross-type stack cast remains without verified storage")
            storage = function.get("stack_storage_reconstruction")
            if storage and hashlib.sha256(body.replace("\r\n", "\n").encode()).hexdigest() != storage["body_sha256"]:
                raise ValueError("Reconstructed stack-storage body hash mismatch")
            if literal_tokens(expected) != literal_tokens(body):
                raise ValueError("Source literal mismatch")
            compact = re.sub(r"\s+", "", body)
            for enum in function.get("local_enum_evidence", []):
                for name, number in enum["enumerators"].items():
                    if not re.search(r"\b" + name + "=" + str(number) + r"\b", compact):
                        raise ValueError("Function-local enum value mismatch")
            boundaries = function.get("array_boundaries", [])
            boundary_proof = function.get('array_boundary_evidence', {})
            if 'machine_checks' in boundary_proof:
                if boundary_proof['raw_sha256'] != function['raw_sha256']:
                    raise ValueError('Array endpoint raw provenance mismatch')
                for address,expected_bytes in boundary_proof['machine_checks'].items():
                    data=bytes.fromhex(expected_bytes)
                    if machine_bytes(int(address,16),len(data))!=data:
                        raise ValueError('Array endpoint instruction mismatch')
            for boundary in boundaries:
                if re.sub(r"\s+", "", boundary["original"]) in compact:
                    raise ValueError("Adjacent-object array endpoint remains")
                count = sum(b["replacement"] == boundary["replacement"] for b in boundaries)
                if compact.count(re.sub(r"\s+", "", boundary["replacement"])) != count:
                    raise ValueError("Recovered array endpoint mismatch")
        except (ValueError, OSError) as error:
            issues.append(dict(source=function["source"], name=function["name"], error=str(error)))
    interfaces = json.loads((root / "docs/recovery-evidence/interface-recovery.json").read_text())
    tokens = interfaces.get('token_core_2022')
    if tokens:
        try:
            if (tokens['layout']['bytes'] != 76 or len(tokens['layout']['members']) != 22 or
                tokens['layout']['members'][-1] != {'name':'special_flags3','type':'int','offset':72} or
                tokens['actions']['bytes'] != 4 or len(tokens['actions']['members']) != 16 or
                tokens['keyword_word']['offset'] != 292 or tokens['keyword_word']['bytes'] != 4 or
                (tokens['quantities_table']['records'], tokens['quantities_table']['words_per_record'], tokens['quantities_table']['bytes']) != (4,2,32)):
                raise ValueError('Token layout/action/keyword-word evidence mismatch')
            for at,expected_hex in tokens['machine_checks'].items():
                data = bytes.fromhex(expected_hex)
                if machine_bytes(int(at,16),len(data)) != data:
                    raise ValueError('Token resolver/copy/keyword instruction mismatch')
            defs = (root/'defs.h').read_text()
            if tokens['keyword_word']['overlay'] not in defs:
                raise ValueError('Token keyword mirror lost the original upkeep byte declarations')
            if not re.search(r'_engine_dynamic_iid_start_2022\s*=\s*0x7a5380;', (root/'manalink.lds').read_text(),re.I):
                raise ValueError('Native dynamic token-type pool binding mismatch')
        except (ValueError,OSError,KeyError) as error:
            issues.append(dict(source='functions/token_generation.c',name='token_core_2022',error=str(error)))
    yorion_master = interfaces.get('yorion_2022')
    if yorion_master:
        try:
            if (yorion_master['shape']['dimensions'] != [2, 151] or yorion_master['shape']['bytes'] != 302 or
                yorion_master['predicate_parameters'] != [['who_chooses','int',[145,0]], ['player','int',[145,4]],
                    ['card','int',[145,8]], ['targeting_player','int',[145,12]], ['targeting_card','int',[145,16]]]):
                raise ValueError('Yorion array/callback layout mismatch')
            for at, expected_hex in yorion_master['machine_checks'].items():
                data = bytes.fromhex(expected_hex)
                if machine_bytes(int(at, 16), len(data)) != data:
                    raise ValueError('Yorion zero-fill/callback/native-mode instruction mismatch')
            mode = yorion_master['singleplayer_interface']
            defs = (root / 'defs.h').read_text()
            header = (root / 'manalink.h').read_text()
            mode_type = mode['moved_original'].split('extern ')[0]
            if (mode_type not in defs or header.count('extern singleplayer_mode_t singleplayer_mode;') != 1 or
                'singleplayer_mode_t;' in sources['functions/rules_engine.c'] or
                mode['enum']['bytes'] != 4 or mode['native_address'] != '4e95a8' or
                not re.search(r'_singleplayer_mode\s*=\s*0x4e95a8;', (root / 'manalink.lds').read_text(), re.I)):
                raise ValueError('Shared singleplayer-mode interface mismatch')
        except (ValueError, OSError, KeyError) as error:
            issues.append(dict(source='cards/ikoria.c', name='yorion_2022', error=str(error)))
    game_mode = interfaces.get('game_mode_2022')
    if game_mode:
        try:
            code = machine_bytes(0x26c7a80, game_mode['pregame_machine_bytes'])
            if hashlib.sha256(code).hexdigest() != game_mode['pregame_machine_sha256']:
                raise ValueError('Game-mode pregame machine hash mismatch')
            for at, expected_hex in game_mode['machine_checks'].items():
                data = bytes.fromhex(expected_hex)
                if machine_bytes(int(at, 16), len(data)) != data:
                    raise ValueError('Game-mode conditional reset/avatar write mismatch')
            if (game_mode['global_state']['address'] != '27b3728' or game_mode['global_state']['bytes'] != 4 or
                game_mode['enum']['bytes'] != 4 or game_mode['enum']['members'] !=
                    {'OGM_NONE':0, 'OGM_EDH':1, 'OGM_PLANECHASE':2, 'OGM_RANDOM_SINGLETON':4, 'OGM_SPAT':8}):
                raise ValueError('Game-mode enum/storage layout mismatch')
        except (ValueError, OSError, KeyError) as error:
            issues.append(dict(source='functions/rules_engine.c', name='game_mode_2022', error=str(error)))
    life_thunks = interfaces.get('life_upkeep_thunks')
    if life_thunks:
        for name, thunk in life_thunks['functions'].items():
            try:
                text = sources[thunk['source']]
                body = function_body(text, name).replace('\r\n', '\n')
                raw = (analysis / 'decompiled-abi' / (thunk['address'] + '.c')).read_text()
                if (hashlib.sha256(body.encode()).hexdigest() != thunk['body_sha256'] or
                    hashlib.sha256(raw.encode()).hexdigest() != thunk['raw_sha256'] or
                    hashlib.sha256(machine_bytes(int(thunk['address'], 16), thunk['machine_bytes'])).hexdigest() != thunk['machine_sha256'] or
                    re.search(r'\bstatic\s+int\s+' + name + r'\s*\(', strip_comments(text)) or
                    thunk['prototype'] not in (root / 'manalink.h').read_text()):
                    raise ValueError('Retained life-upkeep callback interface/body mismatch')
            except (ValueError, OSError) as error:
                issues.append(dict(source=thunk['source'], name=name, error=str(error)))
    player_flags = interfaces.get('player_flag_caches_2022')
    if player_flags:
        try:
            for name, info in {**player_flags['copies'], **player_flags['resets']}.items():
                if hashlib.sha256(machine_bytes(int(info['function_address'], 16), info['machine_bytes'])).hexdigest() != info['machine_sha256']:
                    raise ValueError('Player-flag lifecycle machine hash mismatch: ' + name)
                for site in info['instructions']:
                    site_bytes = bytes.fromhex(site['bytes'])
                    if machine_bytes(int(site['address'], 16), len(site_bytes)) != site_bytes:
                        raise ValueError('Player-flag lifecycle instruction mismatch')
            snapshots = (('backup_data_for_ai', 'backup_', False), ('restore_data_for_ai', 'backup_', True),
                         ('backup_data_for_ai_0', 'backup0_', False), ('restore_data_for_ai_0', 'backup0_', True))
            for name, prefix, restoring in snapshots:
                expected_pairs = []
                for live_name in ('card_flags', 'your_spells_cant_be_countered_by_color'):
                    live = player_flags['globals'][live_name]
                    backup = player_flags['globals'][prefix + live_name]
                    src, dst = (backup, live) if restoring else (live, backup)
                    width = min(live['bytes'], 4)
                    if live['bytes'] != backup['bytes']:
                        raise ValueError('Player-flag backup size mismatch')
                    expected_pairs.extend(dict(destination=f'{int(dst["address"], 16) + off:x}', source=f'{int(src["address"], 16) + off:x}', width=width) for off in range(0, live['bytes'], width))
                expected_pairs.sort(key=lambda p: int(p['destination'], 16))
                if player_flags['copies'][name]['bytes'] != 10 or player_flags['copies'][name]['pairs'] != expected_pairs:
                    raise ValueError('Player-flag complete snapshot-copy mismatch')
            for info in player_flags['resets'].values():
                g = player_flags['globals'][info['global_name']]
                width = min(g['bytes'], 4)
                if ({(int(s['destination'], 16), s['width']) for s in info['instructions']} !=
                    {(int(g['address'], 16) + off, width) for off in range(0, g['bytes'], width)}):
                    raise ValueError('Player-flag complete reset range mismatch')
        except (ValueError, OSError, KeyError) as error:
            issues.append(dict(source='functions/engine.c', name='player_flag_caches_2022', error=str(error)))
    mana_caches = interfaces.get("mana_modifier_caches_2022")
    if mana_caches:
        try:
            live_names = ("mana_colors_added", "mana_color_override", "mana_quantity_override", "multi_mana_transmutation")
            snapshots = (("backup_data_for_ai", "backup_", False),
                         ("restore_data_for_ai", "backup_", True),
                         ("backup_data_for_ai_0", "backup0_", False),
                         ("restore_data_for_ai_0", "backup0_", True))
            for name, prefix, restoring in snapshots:
                copy = mana_caches["copies"][name]
                code = machine_bytes(int(copy["function_address"], 16), copy["machine_bytes"])
                if hashlib.sha256(code).hexdigest() != copy["machine_sha256"]:
                    raise ValueError("Mana snapshot machine hash mismatch: " + name)
                expected_pairs = {}
                for live_name in live_names:
                    live = mana_caches["globals"][live_name]
                    backup = mana_caches["globals"][prefix + live_name]
                    if live["bytes"] != backup["bytes"]:
                        raise ValueError("Mana snapshot storage size mismatch")
                    src, dst = ((backup, live) if restoring else (live, backup))
                    expected_pairs.update({f'{int(dst["address"], 16) + off:x}':
                                           f'{int(src["address"], 16) + off:x}'
                                           for off in range(0, live["bytes"], 4)})
                if (copy["bytes"] != 152 or len(expected_pairs) != 38 or
                    copy["word_pairs"] != expected_pairs or len(copy["blocks"]) != 2):
                    raise ValueError("Mana snapshot complete word-copy proof mismatch")
                for site in copy["instructions"]:
                    site_bytes = bytes.fromhex(site["bytes"])
                    if machine_bytes(int(site["address"], 16), len(site_bytes)) != site_bytes:
                        raise ValueError("Mana snapshot instruction mismatch")
                for block in copy["blocks"]:
                    if block["words"] != 16 or block["bytes"] != "f3a5":
                        raise ValueError("Mana snapshot block-copy size mismatch")
                    for off in range(0, 64, 4):
                        if expected_pairs.get(f'{int(block["destination"], 16) + off:x}') != f'{int(block["source"], 16) + off:x}':
                            raise ValueError("Mana snapshot block-copy range mismatch")
        except (ValueError, OSError, KeyError) as error:
            issues.append(dict(source="functions/ai.c", name="mana_modifier_caches_2022", error=str(error)))
    mana_callers_path = root / "docs/recovery-evidence/mana-callers.json"
    if mana_callers_path.exists():
        try:
            route = json.loads(mana_callers_path.read_text())["native_count_route"]
            executable = Image((analysis / "2022-Magic.exe").read_bytes())
            if hashlib.sha256(executable.data).hexdigest() != route["executable_sha256"]:
                raise ValueError("Native count-mana executable hash mismatch")
            entry, stub = route["entry"], route["stub"]
            entry_at, stub_at = int(entry["address"], 16), int(stub["address"], 16)
            entry_bytes, stub_bytes = bytes.fromhex(entry["bytes"]), bytes.fromhex(stub["bytes"])
            if (len(entry_bytes) != 5 or len(stub_bytes) != 5 or
                entry_bytes[0] != 0xe9 or stub_bytes[0] != 0xe9 or
                executable.pe.get_data(entry_at - executable.pe.OPTIONAL_HEADER.ImageBase, 5) != entry_bytes or
                machine_bytes(stub_at, 5) != stub_bytes or
                entry_at + 5 + struct.unpack('<i', entry_bytes[1:])[0] != stub_at or
                stub_at + 5 + struct.unpack('<i', stub_bytes[1:])[0] != int(route["target"], 16) or
                route["target"] != "26a9b30"):
                raise ValueError("Native count-mana jump route mismatch")
        except (ValueError, OSError, KeyError) as error:
            issues.append(dict(source="functions/engine.c", name="count_mana_2022", error=str(error)))
    for declaration in manifest.get("source_globals", []):
        source = strip_comments(sources[declaration["source"]]).replace('\r\n', '\n')
        if source.count(declaration["declaration"]) != 1:
            issues.append(dict(source=declaration["source"], name=declaration["name"],
                               error="Missing or repeated recovered global declaration"))
    for fragment in manifest.get("source_fragments", []):
        try:
            raw = (analysis / fragment.get('raw_export', 'decompiled-abi/' + fragment['address'] + '.c')).read_text(encoding="utf-8")
            if hashlib.sha256(raw.encode()).hexdigest() != fragment["raw_sha256"]:
                raise ValueError("Fragment raw export hash mismatch")
            source = sources[fragment["source"]]
            start, end = fragment["start_marker"], fragment["end_marker"]
            if source.count(start) != 1 or source.count(end) != 1:
                raise ValueError("Missing or repeated fragment marker")
            body = source.split(start)[1].split(end)[0].strip().replace("\r\n", "\n")
            if hashlib.sha256(body.encode()).hexdigest() != fragment["body_sha256"]:
                raise ValueError("Source fragment hash mismatch")
        except (ValueError, OSError) as error:
            issues.append(dict(source=fragment["source"], name=fragment["original_function"], error=str(error)))
    report = dict(functions=len(manifest["accepted_functions"]), source_fragments=len(manifest.get("source_fragments", [])),
                  stack_storage_functions=sum(bool(f.get("stack_storage_reconstruction")) for f in manifest["accepted_functions"]),
                  recovered_literal_arguments=recovered_strings,
                  reconstructed_literal_pointer_stores=literal_pointer_stores,
                  reconstructed_literal_constants=literal_constants,
                  source_markers="All unique and present." if not issues else "See issues.",
                  raw_export_hashes="All match." if not issues else "See issues.",
                  literal_values="Checked, including byte-exact read-only arguments, verified pointer constants/stores and adjacent C strings.",
                  issues=issues)
    (analysis / "recovery-audit.json").write_text(json.dumps(report, indent=2))
    if not issues:
        manifest["provenance_audit"] = {**report, "issues": 0}
        path.write_text(json.dumps(manifest, indent=2))
    print(json.dumps(report, indent=2))
    raise SystemExit(bool(issues))


if __name__ == "__main__":
    main()
