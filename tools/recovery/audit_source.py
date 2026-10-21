"""Audit source provenance and literal bytes against cached recovery inputs."""
import ast
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


def target_validation_input(text, evidence, address, machine_bytes):
    """Replay only the recorded per-function string/storage reconstructions."""
    if hashlib.sha256(machine_bytes(int(address, 16), evidence['machine_bytes'])).hexdigest() != evidence['machine_sha256']:
        raise ValueError('Target validation/availability machine hash mismatch')
    for change in evidence['changes']:
        original = change['original']
        anchor=change.get('after_anchor')
        at=text.index(anchor) if anchor else 0
        if text[at:].count(original) != change['occurrences']:
            raise ValueError('Target validator reconstruction input mismatch')
        text = text[:at]+text[at:].replace(original, change['replacement'])
        for site in change['machine_sites']:
            data = bytes.fromhex(site['bytes'])
            if machine_bytes(int(site['address'], 16), len(data)) != data:
                raise ValueError('Target validator storage/predicate/live-string instruction mismatch')
        if 'machine_range' in change:
            start, end = (int(a, 16) for a in change['machine_range'])
            if (end - start != change['machine_bytes'] or
                hashlib.sha256(machine_bytes(start, end - start)).hexdigest() != change['machine_sha256']):
                raise ValueError('Target validator inline append range mismatch')
        for block in change.get('machine_ranges',[]):
            start,end=int(block['start'],16),int(block['end'],16)
            if end-start!=block['bytes'] or hashlib.sha256(machine_bytes(start,end-start)).hexdigest()!=block['sha256']:
                raise ValueError('Reconstructed storage native range mismatch')
        literal = change.get('native_literal')
        if literal:
            data = bytes.fromhex(literal['bytes'])
            if (not data.endswith(b'\0') or len(data) != literal['copy_bytes'] or
                machine_bytes(int(literal['address'], 16), len(data)) != data):
                raise ValueError('Target validator inline append literal mismatch')
            expected = change.get('literal', ',cannot target self').encode() + b'\0'
            if data != expected:
                raise ValueError('Target validator inline append text mismatch')
    return text


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
            target_validation = (function.get('target_validation_evidence') or function.get('target_selection_evidence') or
                                 function.get('test_core_evidence') or function.get('test_consumer_evidence') or
                                 function.get('sacrifice_core_evidence') or function.get('generic_spell_evidence') or
                                 function.get('split_helper_evidence') or function.get('aura_consumer_evidence') or
                                 function.get('aura_attachment_binding_2022') or function.get('native_scalar_callee_binding_2022') or
                                 function.get('ability_writer_evidence') or function.get('ability_writer_binding_2022') or
                                 function.get('native_ability_callee_binding_2022') or function.get('pump_core_evidence') or
                                 function.get('pump_callee_binding_2022') or function.get('native_pump_callee_binding_2022') or
                                 function.get('activated_core_evidence') or function.get('native_activated_callee_binding_2022') or
                                 function.get('activation_cost_evidence') or function.get('attacker_core_evidence') or
                                 function.get('native_attack_callee_binding_2022') or function.get('entry_trigger_evidence') or
                                 function.get('native_entry_callee_binding_2022') or function.get('level_buddy_evidence') or
                                 function.get('native_level_buddy_callee_binding_2022') or function.get('native_dependency_bindings_2022') or
                                 function.get('ward_guard_evidence') or function.get('phyrexian_cost_evidence') or
                                 function.get('native_phyrexian_callee_binding_2022') or function.get('spell_trigger_evidence') or function.get('dependency_repair_evidence') or function.get('death_trigger_evidence') or function.get('native_death_trigger_callee_binding_2022') or function.get('action_core_evidence') or function.get('native_action_callee_binding_2022') or function.get('global_tutor_evidence'))
            if target_validation:
                literal_input = target_validation_input(literal_input, target_validation, function['address'], machine_bytes)
                for change in target_validation['changes']:
                    if change['kind'] == 'VLA call-frame reconstruction':
                        template = function_body(sources[function['source']], change['source_template_name'])
                        if (hashlib.sha256(template.replace('\r\n', '\n').encode()).hexdigest() != change['source_template_sha256'] or
                            (change['logical_size_addend'], change['native_alignment']) != (50, 4) or
                            literal_tokens(change['replacement']) != [('"', '%s %d %d'), ('"', 'TARGET_COUNT'), ('"', 'FIREBALL')]):
                            raise ValueError('Target picker VLA source-template/layout mismatch')
                        for literal in change['literals']:
                            data = bytes.fromhex(literal['bytes']); instruction = literal['instruction']; code = bytes.fromhex(instruction['bytes'])
                            if (readonly_string(image, int(literal['address'], 16)) != data or
                                machine_bytes(int(instruction['address'], 16), len(code)) != code or
                                not code.endswith(struct.pack('<I', int(literal['address'], 16)))):
                                raise ValueError('Target picker VLA text-resource operand mismatch')
                        for block in change['ranges']:
                            start, end = int(block['start'], 16), int(block['end'], 16)
                            if end - start != block['bytes'] or hashlib.sha256(machine_bytes(start, end - start)).hexdigest() != block['sha256']:
                                raise ValueError('Target picker VLA argument/allocation/cleanup range mismatch')
                        stores = change['format_word_stores']
                        if [s['bytes'] for s in stores] != ['c7459025732025', 'c7459464202564', 'c6459800']:
                            raise ValueError('Target picker VLA AI format stores mismatch')
                        for site in stores:
                            data = bytes.fromhex(site['bytes'])
                            if machine_bytes(int(site['address'], 16), len(data)) != data:
                                raise ValueError('Target picker VLA AI format instruction mismatch')
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
            for label,typename in [('test','test_definition_2022_t'),('target','target_definition_2022_t')]:
                interface=function.get(label+'_interface_2022')
                if interface:
                    from promote_recovery import TEST_API_NAMES,TARGET_API_NAMES
                    names=TEST_API_NAMES if label=='test' else TARGET_API_NAMES
                    if interface['type']!=typename or interface['definition_present']!=bool(re.search(r'\b'+typename+r'\b',body)):
                        raise ValueError('Versioned '+label+' aggregate interface mismatch')
                    for original,replacement in interface['calls'].items():
                        if names.get(original)!=replacement or not re.search(r'\b'+replacement+r'\s*\(',body):
                            raise ValueError('Versioned '+label+' API binding mismatch')
            sacrifice_interface=function.get('sacrifice_interface_2022')
            if sacrifice_interface:
                from promote_recovery import SACRIFICE_API_NAMES,SACRIFICE_FLAGS_2022
                if sacrifice_interface['type']!='sacrifice_2022_t' or sacrifice_interface['definition_present']!=bool(re.search(r'\bsacrifice_2022_t\b',body)):
                    raise ValueError('Versioned sacrifice interface mismatch')
                for original,replacement in sacrifice_interface['calls'].items():
                    if SACRIFICE_API_NAMES.get(original)!=replacement or not re.search(r'\b'+replacement+r'\s*\(',body):
                        raise ValueError('Versioned sacrifice callee mismatch')
                for flag in sacrifice_interface['flags']:
                    if flag not in SACRIFICE_FLAGS_2022 or not re.search(r'\b'+flag.replace('SAC_','SAC2022_')+r'\b',body):
                        raise ValueError('Versioned sacrifice flag mismatch')
            spell_interface=function.get('spell_interface_2022')
            if spell_interface:
                from promote_recovery import SPELL_API_NAMES,SPELL_FLAGS_2022
                if spell_interface['type']!='generic_spell_flags_2022_t' or spell_interface['definition_present']!=bool(re.search(r'\bgeneric_spell_flags_2022_t\b',body)):
                    raise ValueError('Versioned generic-spell interface mismatch')
                for original,replacement in spell_interface['calls'].items():
                    if SPELL_API_NAMES.get(original)!=replacement or not re.search(r'\b'+replacement+r'\s*\(',body):
                        raise ValueError('Versioned generic-spell callee mismatch')
                for flag in spell_interface['flags']:
                    if flag not in SPELL_FLAGS_2022 or not re.search(r'\b'+flag.replace('GS_','GS2022_')+r'\b',body):
                        raise ValueError('Versioned generic-spell flag mismatch')
            ability_interface=function.get('ability_interface_2022')
            if ability_interface:
                from promote_recovery import ABILITY_API_NAMES
                if ability_interface['type']!='pump_ability_2022_t' or ability_interface['definition_present']!=bool(re.search(r'\bpump_ability_2022_t\b',body)):
                    raise ValueError('Versioned ability/pump interface mismatch')
                for original,replacement in ability_interface['calls'].items():
                    if ABILITY_API_NAMES.get(original)!=replacement or not re.search(r'\b'+replacement+r'\s*\(',body):
                        raise ValueError('Versioned ability/pump callee mismatch')
            pump_interface=function.get('pump_interface_2022')
            if pump_interface:
                from promote_recovery import PUMP_API_NAMES
                for original,replacement in pump_interface['calls'].items():
                    if PUMP_API_NAMES.get(original)!=replacement or not re.search(r'\b'+replacement+r'\s*\(',body):
                        raise ValueError('Versioned pump callee mismatch')
            activated_interface=function.get('activated_interface_2022')
            if activated_interface:
                from promote_recovery import ACTIVATED_API_NAMES
                for original,replacement in activated_interface['calls'].items():
                    if ACTIVATED_API_NAMES.get(original)!=replacement or not re.search(r'\b'+replacement+r'\s*\(',body):
                        raise ValueError('Versioned activation callee mismatch')
            attack_interface=function.get('attack_interface_2022')
            if attack_interface:
                from promote_recovery import ATTACK_API_NAMES
                for original,replacement in attack_interface['calls'].items():
                    if ATTACK_API_NAMES.get(original)!=replacement or not re.search(r'\b'+replacement+r'\s*\(',body):
                        raise ValueError('Versioned attacker/keyword helper callee mismatch')
            entry_interface=function.get('entry_interface_2022')
            if entry_interface:
                from promote_recovery import ENTRY_API_NAMES
                for original,replacement in entry_interface['calls'].items():
                    if ENTRY_API_NAMES.get(original)!=replacement or not re.search(r'\b'+replacement+r'\s*\(',body):
                        raise ValueError('Versioned entry-trigger/keyword helper callee mismatch')
            level_buddy_interface=function.get('level_buddy_interface_2022')
            if level_buddy_interface:
                from promote_recovery import LEVEL_BUDDY_API_NAMES
                for original,replacement in level_buddy_interface['calls'].items():
                    if LEVEL_BUDDY_API_NAMES.get(original)!=replacement or not re.search(r'\b'+replacement+r'\s*\(',body):
                        raise ValueError('Versioned Level Up/Buddy callee mismatch')
            phyrexian_interface=function.get('phyrexian_interface_2022')
            if phyrexian_interface:
                from promote_recovery import PHYREXIAN_API_NAMES
                for original,replacement in phyrexian_interface['calls'].items():
                    if PHYREXIAN_API_NAMES.get(original)!=replacement or not re.search(r'\b'+replacement+r'\s*\(',body):
                        raise ValueError('Versioned Phyrexian cost callee mismatch')
            global_tutor_interface=function.get('global_tutor_interface_2022')
            if global_tutor_interface:
                from promote_recovery import GLOBAL_TUTOR_API_NAMES,TUTOR_DESTINATION_NAMES,TUTOR_ORIGIN_NAMES,TUTOR_DOMAIN_NAMES
                mapping={**GLOBAL_TUTOR_API_NAMES,**TUTOR_DESTINATION_NAMES,**TUTOR_ORIGIN_NAMES,**TUTOR_DOMAIN_NAMES}
                for original,replacement in global_tutor_interface['translations'].items():
                    if mapping.get(original)!=replacement or not re.search(r'\b'+replacement+r'\b',body):
                        raise ValueError('Native global-tutor interface/domain mismatch')
            action_interface=function.get('action_interface_2022')
            if action_interface:
                from promote_recovery import ACTION_API_NAMES
                for original,replacement in action_interface['calls'].items():
                    if ACTION_API_NAMES.get(original)!=replacement or not re.search(r'\b'+replacement+r'\s*\(',body):
                        raise ValueError('Native action/copy consumer callee mismatch')
            death_trigger_interface=function.get('death_trigger_interface_2022')
            if death_trigger_interface:
                from promote_recovery import DEATH_TRIGGER_API_NAMES
                for original,replacement in death_trigger_interface['calls'].items():
                    if DEATH_TRIGGER_API_NAMES.get(original)!=replacement or not re.search(r'\b'+replacement+r'\s*\(',body):
                        raise ValueError('Native death-trigger callee mismatch')
            spell_trigger_interface=function.get('spell_trigger_interface_2022')
            if spell_trigger_interface:
                from promote_recovery import SPELL_TRIGGER_API_NAMES
                for original,replacement in spell_trigger_interface['calls'].items():
                    if SPELL_TRIGGER_API_NAMES.get(original)!=replacement or not re.search(r'\b'+replacement+r'\s*\(',body):
                        raise ValueError('Native spell-trigger callee mismatch')
            aura_part=function.get('aura_part_evidence')
            if aura_part and hashlib.sha256(machine_bytes(int(function['address'],16),aura_part['machine_bytes'])).hexdigest()!=aura_part['machine_sha256']:
                raise ValueError('Aura compiler-part machine hash mismatch')
            aura_binding=function.get('aura_part_binding_2022')
            if aura_binding:
                if (hashlib.sha256(machine_bytes(int(function['address'],16),aura_binding['machine_bytes'])).hexdigest()!=aura_binding['machine_sha256'] or
                    body.count(aura_binding['part']+'(')!=1 or re.search(r'\b'+re.escape(function['name'])+r'\s*\(',body[body.index('{'):]) or
                    aura_binding['body_sha256']!=function['reconstruction_body_sha256']):
                    raise ValueError('Aura public wrapper private binding mismatch')
                for call in aura_binding['calls']:
                    at=int(call['address'],16);data=bytes.fromhex(call['bytes'])
                    if (len(data)!=5 or data[0] not in {0xe8,0xe9} or machine_bytes(at,5)!=data or at+5+struct.unpack('<i',data[1:])[0]!=int(call['target'],16)):
                        raise ValueError('Aura compiler-part call/tail target mismatch')
            consumer = (function.get('test_consumer_evidence') or function.get('sacrifice_core_evidence') or
                        function.get('generic_spell_evidence') or function.get('split_helper_evidence') or
                        function.get('aura_consumer_evidence') or function.get('aura_attachment_binding_2022') or
                        function.get('native_scalar_callee_binding_2022') or function.get('ability_writer_evidence') or
                        function.get('ability_writer_binding_2022') or function.get('native_ability_callee_binding_2022') or
                        function.get('pump_core_evidence') or function.get('pump_callee_binding_2022') or
                        function.get('native_pump_callee_binding_2022') or function.get('activated_core_evidence') or
                        function.get('native_activated_callee_binding_2022') or function.get('activation_cost_evidence') or
                        function.get('attacker_core_evidence') or function.get('native_attack_callee_binding_2022') or
                        function.get('entry_trigger_evidence') or function.get('native_entry_callee_binding_2022') or
                        function.get('level_buddy_evidence') or function.get('native_level_buddy_callee_binding_2022') or
                        function.get('native_dependency_bindings_2022') or function.get('ward_guard_evidence') or
                        function.get('phyrexian_cost_evidence') or function.get('native_phyrexian_callee_binding_2022') or function.get('spell_trigger_evidence') or function.get('dependency_repair_evidence') or function.get('death_trigger_evidence') or function.get('native_death_trigger_callee_binding_2022') or function.get('action_core_evidence') or function.get('native_action_callee_binding_2022') or function.get('global_tutor_evidence'))
            if consumer:
                for call in consumer['canonical_calls']:
                    at=int(call['address'],16);data=bytes.fromhex(call['bytes'])
                    if (len(data)!=5 or data[0] not in {0xe8,0xe9} or machine_bytes(at,5)!=data or
                        at+5+struct.unpack('<i',data[1:])[0]!=int(call['target'],16) or
                        not re.search(r'\b'+re.escape(call['name'])+r'\s*\(',body)):
                        raise ValueError('Test consumer native callee binding mismatch')
            dependency_binding=function.get('native_dependency_bindings_2022')
            if dependency_binding:
                from capstone import Cs,CS_ARCH_X86,CS_MODE_32
                from capstone.x86_const import X86_OP_IMM
                from promote_recovery import code_only
                decoder=Cs(CS_ARCH_X86,CS_MODE_32);decoder.detail=True
                for reference in dependency_binding['function_references']:
                    at=int(reference['address'],16);data=bytes.fromhex(reference['bytes'])
                    instructions=list(decoder.disasm(data,at))
                    if (machine_bytes(at,len(data))!=data or len(instructions)!=1 or
                        not any(o.type==X86_OP_IMM and o.imm==int(reference['target'],16) for o in instructions[0].operands) or
                        not re.search(r'\b'+re.escape(reference['name'])+r'\b',code_only(body).split('{',1)[1])):
                        raise ValueError('Native dependency callback/function address mismatch')
            compiler_binding=function.get('compiler_part_binding_2022')
            if compiler_binding:
                if (hashlib.sha256(machine_bytes(int(function['address'],16),compiler_binding['machine_bytes'])).hexdigest()!=compiler_binding['machine_sha256'] or
                    len(re.findall(r'\b'+re.escape(compiler_binding['part'])+r'\s*\(',body))!=1 or
                    compiler_binding['body_sha256']!=function['reconstruction_body_sha256']):
                    raise ValueError('Compiler-part public binding mismatch')
                for call in compiler_binding['calls']:
                    at=int(call['address'],16);data=bytes.fromhex(call['bytes'])
                    if (len(data)!=5 or data[0] not in {0xe8,0xe9} or machine_bytes(at,5)!=data or
                        at+5+struct.unpack('<i',data[1:])[0]!=int(call['target'],16)):
                        raise ValueError('Compiler-part native call target mismatch')
            target_getter_binding = function.get('target_getter_binding_2022')
            if target_getter_binding:
                call = target_getter_binding['call']; at = int(call['address'], 16); data = bytes.fromhex(call['bytes'])
                if (function['address'] != '265d580' or call['target'] != '265d2e0' or len(data) != 5 or data[0] != 0xe8 or
                    machine_bytes(at, 5) != data or at + 5 + struct.unpack('<i', data[1:])[0] != int(call['target'], 16) or
                    body.count(target_getter_binding['replacement'] + '(') != 1 or
                    target_getter_binding['body_sha256'] != function['reconstruction_body_sha256']):
                    raise ValueError('Protection dispatcher private getter binding mismatch')
            if target_validation:
                for call in target_validation['canonical_calls']:
                    at = int(call['address'], 16); data = bytes.fromhex(call['bytes'])
                    if len(data) == 5 and data[0] in {0xe8, 0xe9}:
                        delta = struct.unpack('<i', data[1:])[0]
                    elif len(data) == 2 and data[0] == 0xeb:
                        delta = struct.unpack('<b', data[1:])[0]
                    else:
                        raise ValueError('Unsupported target validation call encoding')
                    if (machine_bytes(at, len(data)) != data or at + len(data) + delta != int(call['target'], 16) or
                        not re.search(r'\b' + re.escape(call['name']) + r'\s*\(', body)):
                        raise ValueError('Target validation versioned callee mismatch')
            target_initializer = function.get('target_initializer_evidence')
            if target_initializer:
                if hashlib.sha256(machine_bytes(int(function['address'], 16), target_initializer['machine_bytes'])).hexdigest() != target_initializer['machine_sha256']:
                    raise ValueError('Target initializer/protection worker machine hash mismatch')
                for call in target_initializer['calls']:
                    data = bytes.fromhex(call['bytes']); at = int(call['address'], 16)
                    if (machine_bytes(at, len(data)) != data or len(data) != 5 or data[0] not in {0xe8, 0xe9} or
                        at + 5 + struct.unpack('<i', data[1:])[0] != int(call['target'], 16) or
                        not re.search(r'\b' + re.escape(call['name']) + r'\s*\(', body)):
                        raise ValueError('Target initializer private/public call binding mismatch')
                store = target_initializer['flags_store']
                if store:
                    data = bytes.fromhex(store['bytes'])
                    if (machine_bytes(int(store['address'], 16), len(data)) != data or
                        data not in {bytes.fromhex('c7405000000000'), bytes.fromhex('c7435000000000')} or
                        (store['offset'], store['value']) != (80, 0) or 'td->flags = 0;' not in body):
                        raise ValueError('Target initializer flags store mismatch')
            subtype_mutator=function.get('extension_subtype_evidence')
            if subtype_mutator:
                if hashlib.sha256(machine_bytes(int(function['address'],16),subtype_mutator['machine_bytes'])).hexdigest()!=subtype_mutator['machine_sha256']:
                    raise ValueError('Extension subtype mutator machine hash mismatch')
                compact_body=re.sub(r'\s+','',body)
                for change in subtype_mutator['adjustments']:
                    count=change.get('occurrences',1)
                    if raw.count(change['original'])!=count or compact_body.count(re.sub(r'\s+','',change['replacement']))!=count:
                        raise ValueError('Extension subtype endpoint/lookahead/range reconstruction mismatch')
                    if change['kind']=='adjacent information ID lookahead' and (change['subtype_count'],change['adjacent_offset'],change['bytes'])!=(12,32,2):
                        raise ValueError('Extension subtype adjacent-word layout mismatch')
            token_stack = function.get('token_stack_evidence')
            if token_stack:
                if hashlib.sha256(machine_bytes(int(function['address'],16),token_stack['machine_bytes'])).hexdigest() != token_stack['machine_sha256']:
                    raise ValueError('Token stack-card/predicate machine hash mismatch')
                word_blocks=[b for b in token_stack['literal_blocks'] if b['kind']=='token word store']
                if len(word_blocks)!=len(token_stack['word_stores']):
                    raise ValueError('Token stack word-store block count mismatch')
                for block,store in zip(word_blocks,token_stack['word_stores']):
                    from capstone import Cs,CS_ARCH_X86,CS_MODE_32
                    from capstone.x86_const import X86_OP_MEM,X86_OP_IMM,X86_REG_EBP
                    chars=re.findall(r"=\s*('(?:\\.|[^'\\])*');",block['original'])
                    if len(chars)!=4 or int.from_bytes(bytes(ord(ast.literal_eval(c)) for c in chars),'little')!=store['value']:
                        raise ValueError('Token stack native byte/word value mismatch')
                    site=store['site'];code=bytes.fromhex(site['bytes']);at=int(site['address'],16)
                    decoder=Cs(CS_ARCH_X86,CS_MODE_32);decoder.detail=True;instructions=list(decoder.disasm(code,at))
                    if machine_bytes(at,len(code))!=code or len(instructions)!=1:
                        raise ValueError('Token stack native word instruction mismatch')
                    ins=instructions[0];ops=ins.operands
                    if (ins.mnemonic!='mov' or len(ops)!=2 or ops[0].type!=X86_OP_MEM or ops[0].size!=4 or
                        ops[0].mem.base!=X86_REG_EBP or ops[0].mem.disp!=store['frame_offset']+8+store['offset'] or
                        ops[1].type!=X86_OP_IMM or ops[1].imm!=store['value']):
                        raise ValueError('Token stack native word destination/value mismatch')
                for block in token_stack['literal_blocks']:
                    original=block['original']
                    if expected.count(original)!=1 or hashlib.sha256(original.encode()).hexdigest()!=block['original_sha256']:
                        raise ValueError('Token stack literal-block provenance mismatch')
                    expected=expected.replace(original,block['replacement'])
            frontend=function.get('token_frontend_evidence')
            if frontend:
                if hashlib.sha256(machine_bytes(int(function['address'],16),frontend['machine_bytes'])).hexdigest()!=frontend['machine_sha256']:
                    raise ValueError('Token frontend machine hash mismatch')
                for call in frontend['canonical_calls']:
                    at=int(call['address'],16);data=bytes.fromhex(call['bytes'])
                    if len(data)==5 and data[0] in {0xe8,0xe9}:delta=struct.unpack('<i',data[1:])[0]
                    elif len(data)==2 and data[0]==0xeb:delta=struct.unpack('<b',data[1:])[0]
                    else:raise ValueError('Unsupported token frontend call/jump encoding')
                    if (machine_bytes(at,len(data))!=data or at+len(data)+delta!=int(call['target'],16) or
                        not re.search(r'\b'+call['replacement']+r'\s*\(',body)):
                        raise ValueError('Token frontend canonical call/tail-jump mismatch')
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
    aura_parts=interfaces.get('aura_parts_2022')
    if aura_parts:
        try:
            by_address={f['address']:f for f in manifest['accepted_functions']}
            if len(aura_parts['functions'])!=3 or len(aura_parts['repairs'])!=3:
                raise ValueError('Aura compiler-part component membership mismatch')
            for address,detail in aura_parts['functions'].items():
                if by_address[address]['aura_part_evidence']!=detail or not by_address[address]['static']:
                    raise ValueError('Aura compiler-part ownership/evidence mismatch')
            for address,detail in aura_parts['repairs'].items():
                if by_address[address]['aura_part_binding_2022']!=detail or len(aura_parts['parameters'][address])!=len(aura_parts['parameters'][detail['part_address']]):
                    raise ValueError('Aura wrapper physical-argument/evidence mismatch')
        except (ValueError,OSError,KeyError) as error:
            issues.append(dict(source='functions/auras.c',name='aura_parts_2022',error=str(error)))
    test_core=interfaces.get('test_core_2022')
    if test_core:
        try:
            record=test_core['layout'];members={m['name']:m for m in record['members']}
            if (record['bytes']!=216 or (members['sp_keyword']['offset'],members['sp_keyword']['bytes'])!=(140,4) or
                (members['sp_keyword_flag']['offset'],members['sp_keyword_flag']['bytes'])!=(144,2) or members['storage']['offset']!=212 or
                test_core['callback']!=dict(return_type='int',arguments=['int']*4)):
                raise ValueError('2022 test record/callback layout mismatch')
            native=json.loads((root/'docs/recovery-evidence/stack-storage.json').read_text())['target_layouts']['test_definition_t']
            if native['members']!={m['name']:m['offset'] for m in record['members']} or native['member_sizes']!={m['name']:m['bytes'] for m in record['members']}:
                raise ValueError('2022 test member registry mismatch')
            entries={f['address']:f for f in manifest['accepted_functions'] if f.get('test_core_evidence')}
            if len(entries)!=8 or set(entries)!=set(test_core['functions']):
                raise ValueError('2022 test core membership mismatch')
            for address,detail in test_core['functions'].items():
                if entries[address]['test_core_evidence']!=detail:
                    raise ValueError('2022 test core evidence mismatch')
                for site in detail['new_field_sites']:
                    data=bytes.fromhex(site['bytes'])
                    if machine_bytes(int(site['address'],16),len(data))!=data:
                        raise ValueError('2022 test added-field native operand mismatch')
        except (ValueError,OSError,KeyError) as error:
            issues.append(dict(source='functions/global_checking.c',name='test_core_2022',error=str(error)))
    spell=interfaces.get('generic_spell_2022')
    if spell:
        try:
            from generic_spell_body import STORAGE
            entries={f['address']:f for f in manifest['accepted_functions'] if f.get('generic_spell_evidence')}
            if set(entries)!={'26a0250','2650c40'} or set(entries)!=set(spell['functions']):
                raise ValueError('Generic-spell component membership mismatch')
            if spell['storage_declarations']!=STORAGE or sources['functions/functions.c'].replace('\r\n','\n').count(STORAGE)!=1:
                raise ValueError('Generic-spell typed storage declaration mismatch')
            if (spell['flags']['bytes']!=4 or len(spell['flags']['members'])!=16 or
                list(spell['flags']['members'].values())!=[1<<n for n in range(16)] or
                spell['retained_flags']!=dict(GS_ONE_OR_MULTIPLE_TARGETS=8192,GS_SAC_PERMANENT_AS_COST=16384)):
                raise ValueError('Generic-spell native/legacy flag-domain mismatch')
            fixed={(x['name'],x['type'],x['location']) for x in spell['locals']['26a0250'] if isinstance(x['location'],str)}
            if not {('test','test_definition_t','(DW_OP_fbreg: -236)'),('illegal_tgt','char[200]','(DW_OP_fbreg: -236)'),('td9','target_definition_t','(DW_OP_fbreg: -236)'),('targeted_player','int[2]','(DW_OP_fbreg: -244)')}<=fixed:
                raise ValueError('Generic-spell native scratch/array locations mismatch')
            if len(spell['parameters']['26a0250'])!=8 or len(spell['parameters']['2650c40'])!=6:
                raise ValueError('Generic-spell physical interface mismatch')
            literal=spell['retarget_prompt'];site=literal['site']
            if readonly_string(image,int(literal['address'],16))!=bytes.fromhex(literal['bytes']) or site['bytes']!='c744240c74447602':
                raise ValueError('Generic-spell target-choice literal mismatch')
            for site in [site,*spell['control_sites']]:
                data=bytes.fromhex(site['bytes'])
                if machine_bytes(int(site['address'],16),len(data))!=data:
                    raise ValueError('Generic-spell VLA/variadic/cleanup native operand mismatch')
            for address,detail in spell['functions'].items():
                if entries[address]['generic_spell_evidence']!=detail:
                    raise ValueError('Generic-spell component evidence mismatch')
        except (ValueError,OSError,KeyError,StopIteration) as error:
            issues.append(dict(source='functions/functions.c',name='generic_spell_2022',error=str(error)))
    split_helpers=interfaces.get('split_card_helpers_2022')
    if split_helpers:
        try:
            entries={f['address']:f for f in manifest['accepted_functions'] if f.get('split_helper_evidence')}
            if set(entries)!={'26c0860','2199560','2569100','25696b0','2569740','21a3da0'} or set(entries)!=set(split_helpers['functions']):
                raise ValueError('Split card helper membership mismatch')
            for address,detail in split_helpers['functions'].items():
                if entries[address]['split_helper_evidence']!=detail:
                    raise ValueError('Split card helper evidence mismatch')
            if split_helpers['events']!=dict(EVENT_CAN_ACTIVATE_FROM_HAND=177,EVENT_ACTIVATE_FROM_HAND=178,EVENT_RESOLVE_ACTIVATION_FROM_HAND=179):
                raise ValueError('Split card helper hand-event range mismatch')
            for address,arity in [('2199560',3),('2569100',11)]:
                ps=split_helpers['parameters'][address]
                if len(ps)!=arity or [p['location'][0]['expression'] for p in ps[:3]]!=['(DW_OP_reg0 (r0))','(DW_OP_reg2 (r2))','(DW_OP_reg1 (r1))']:
                    raise ValueError('Split card helper physical parameter mismatch')
            legacy=split_helpers['legacy_compatibility']
            body=function_body(sources[legacy['source']],legacy['name'])
            if (legacy['counted_implementation'] or (legacy['retained_target_bytes'],legacy['native_target_bytes'])!=(80,84) or
                hashlib.sha256(body.replace('\r\n','\n').encode()).hexdigest()!=legacy['body_sha256'] or
                re.search(r'\b'+legacy['name']+r'\s*\(',body[body.index('{'):])):
                raise ValueError('Split card helper legacy compatibility mismatch')
        except (ValueError,OSError,KeyError) as error:
            issues.append(dict(source='cards/betrayers_of_kamigawa.c',name='split_card_helpers_2022',error=str(error)))
    aura_consumers=interfaces.get('aura_consumers_2022')
    if aura_consumers:
        try:
            from recover_aura_consumers import ADDRESSES,PRIVATE
            entries={f['address']:f for f in manifest['accepted_functions'] if f.get('aura_consumer_evidence')}
            if set(entries)!=set(ADDRESSES) or set(entries)!=set(aura_consumers['functions']) or {a for a,e in entries.items() if e['static']}!=PRIVATE:
                raise ValueError('Aura consumer membership/linkage mismatch')
            for address,detail in aura_consumers['functions'].items():
                if entries[address]['aura_consumer_evidence']!=detail:
                    raise ValueError('Aura consumer evidence mismatch')
            for address,detail in aura_consumers['repairs'].items():
                entry=next(f for f in manifest['accepted_functions'] if f['address']==address)
                recorded={k:v for k,v in detail.items() if k!='previous_record'}
                body=function_body(sources[entry['source']],entry['name'])
                if (entry.get('aura_attachment_binding_2022')!=recorded or detail['body_sha256']!=entry.get('reconstruction_body_sha256') or
                    re.search(r'\battach_aura_to_target\s*\(',body)):
                    raise ValueError('Aura native attachment binding mismatch')
            trigger=aura_consumers['trigger_condition']
            if trigger['address']!='7a2d78' or trigger['existing_linker_symbol']!='_trigger_condition':
                raise ValueError('Aura trigger-condition linker binding mismatch')
            for site in trigger['sites']:
                data=bytes.fromhex(site['bytes'])
                if machine_bytes(int(site['address'],16),len(data))!=data:
                    raise ValueError('Aura trigger-condition native operand mismatch')
        except (ValueError,OSError,KeyError) as error:
            issues.append(dict(source='functions/auras.c',name='aura_consumers_2022',error=str(error)))
    for component,binding in [('native_scalar_callers_2022','native_scalar_callee_binding_2022'),
                              ('native_ability_callers_2022','native_ability_callee_binding_2022'),
                              ('native_pump_callers_2022','native_pump_callee_binding_2022'),
                              ('native_activated_callers_2022','native_activated_callee_binding_2022'),
                              ('native_attack_callers_2022','native_attack_callee_binding_2022'),
                              ('native_entry_callers_2022','native_entry_callee_binding_2022'),
                              ('native_level_buddy_callers_2022','native_level_buddy_callee_binding_2022'),
                              ('native_phyrexian_callers_2022','native_phyrexian_callee_binding_2022'),
                              ('native_death_trigger_callers_2022','native_death_trigger_callee_binding_2022'),
                              ('native_action_callers_2022','native_action_callee_binding_2022')]:
        scalar_callers=interfaces.get(component)
        if not scalar_callers:continue
        try:
            entries={f['address']:f for f in manifest['accepted_functions'] if f.get(binding)}
            if set(entries)!=set(scalar_callers['functions']):
                raise ValueError('Native scalar helper caller membership mismatch')
            for original,helper in scalar_callers['helpers'].items():
                entry=next(f for f in manifest['accepted_functions'] if f['address']==helper['address'])
                prototype=helper['prototype']
                if component=='native_activated_callers_2022':
                    if scalar_callers['common_instance_layout']!=dict(bytes=300,ability_loss_offset=256):
                        raise ValueError('Common activation instance-pointer layout mismatch')
                    prototype=re.sub(r'\bcard_instance_t\s*\*','',prototype)
                if entry['name']!=helper['name'] or entry['prototype']!=helper['prototype'] or re.search(r'\*|\b(?:target|test)_definition',prototype):
                    raise ValueError('Native scalar helper signature mismatch')
            for address,detail in scalar_callers['functions'].items():
                entry=entries[address];recorded={k:v for k,v in detail.items() if k!='previous_record'}
                body=function_body(sources[entry['source']],entry['name'])
                if (entry[binding]!=recorded or detail['body_sha256']!=entry['reconstruction_body_sha256'] or
                    any(re.search(r'\b'+original+r'\s*\(',body[body.index('{'):]) for original in detail['calls'])):
                    raise ValueError('Native scalar helper caller binding mismatch')
                from recover_test_consumers import rename
                restored,_=rename(body.replace('\r\n','\n'),{new:old for old,new in detail['calls'].items()})
                if (hashlib.sha256(restored.encode()).hexdigest()!=detail['previous_body_sha256'] or
                    hashlib.sha256(machine_bytes(int(address,16),detail['machine_bytes'])).hexdigest()!=detail['machine_sha256']):
                    raise ValueError('Native scalar caller changed outside recorded bindings or native body mismatch')
                for call in detail['canonical_calls']:
                    at=int(call['address'],16);data=bytes.fromhex(call['bytes'])
                    helper=scalar_callers['helpers'][call['original']]
                    if (len(data)!=5 or data[0] not in {0xe8,0xe9} or machine_bytes(at,5)!=data or
                        at+5+struct.unpack('<i',data[1:])[0]!=int(helper['address'],16) or
                        call['target']!=helper['address'] or call['name']!=helper['name'] or
                        detail['calls'].get(call['original'])!=call['name']):
                        raise ValueError('Native scalar caller exact callee mismatch')
        except (ValueError,OSError,KeyError,StopIteration) as error:
            issues.append(dict(source='cards/',name=component,error=str(error)))
    ability_writers=interfaces.get('ability_writers_2022')
    if ability_writers:
        try:
            from recover_ability_writers import ADDRESSES,PRIVATE,TWO_PARAMETER_PARTS,PUMP_TYPE
            entries={f['address']:f for f in manifest['accepted_functions'] if f.get('ability_writer_evidence')}
            if set(entries)!=set(ADDRESSES) or set(entries)!=set(ability_writers['functions']) or {a for a,f in entries.items() if f['static']}!=set(PRIVATE):
                raise ValueError('Native ability writer membership/linkage mismatch')
            for address,detail in ability_writers['functions'].items():
                if entries[address]['ability_writer_evidence']!=detail:
                    raise ValueError('Native ability writer evidence mismatch')
            record=ability_writers['pump_layout']
            if (record['bytes']!=48 or ability_writers['retained_pump_bytes']!=36 or
                [x['offset'] for x in record['members']]!=list(range(0,48,4)) or any(x['bytes']!=4 for x in record['members']) or
                [x['name'] for x in record['members'][-3:]]!=['subtype','color','disabling_mode'] or
                ability_writers['pump_type_declaration']!=PUMP_TYPE or (root/'defs.h').read_text().count(PUMP_TYPE)!=1):
                raise ValueError('Native/legacy pump layout mismatch')
            for address in TWO_PARAMETER_PARTS:
                ps=ability_writers['parameters'][address]
                if (len(ps)!=3 or [p['location'][0]['expression'] for p in ps[:2]]!=['(DW_OP_reg0 (r0))','(DW_OP_reg2 (r2))'] or
                    not all('DW_OP_GNU_parameter_ref' in x['expression'] for x in ps[2]['location']) or
                    re.search(r'\bevent\b',entries[address]['prototype'])):
                    raise ValueError('Native keyword helper physical/source parameter mismatch')
            ps=ability_writers['parameters']['26df8e0']
            if len(ps)!=5 or ps[3]['location'][0]['expression']!='(DW_OP_fbreg: 0)' or not all('DW_OP_GNU_parameter_ref' in x['expression'] for x in ps[4]['location']):
                raise ValueError('Native granted-flanking physical/source parameter mismatch')
            for address,detail in ability_writers['repairs'].items():
                entry=next(f for f in manifest['accepted_functions'] if f['address']==address)
                if (entry.get('ability_writer_binding_2022')!={k:v for k,v in detail.items() if k!='previous_record'} or
                    entry.get('reconstruction_body_sha256')!=detail['body_sha256'] or
                    re.search(r'\bspecial_abilities\s*\(',function_body(sources[entry['source']],entry['name']))):
                    raise ValueError('Attached keyword writer native callee mismatch')
        except (ValueError,OSError,KeyError,StopIteration) as error:
            issues.append(dict(source='functions/abilities.c',name='ability_writers_2022',error=str(error)))
    pump_core=interfaces.get('pump_core_2022')
    if pump_core:
        try:
            from recover_pump_core import CORE,REMOVAL,DEPENDENCIES,SUBTYPES,PRIVATE,REPLACED,REPAIRS
            entries={f['address']:f for f in manifest['accepted_functions'] if f.get('pump_core_evidence')}
            if (set(entries)!=set(CORE+REMOVAL+DEPENDENCIES+SUBTYPES) or set(entries)!=set(pump_core['functions']) or
                {a for a,f in entries.items() if f['static']}!=PRIVATE or set(pump_core['previous_records'])!=REPLACED or
                set(pump_core['replacements'])!=REPLACED or set(pump_core['repairs'])!=set(REPAIRS)):
                raise ValueError('Native pump component membership/linkage mismatch')
            for address,detail in pump_core['functions'].items():
                if entries[address]['pump_core_evidence']!=detail:
                    raise ValueError('Native pump component evidence mismatch')
                body=function_body(sources[entries[address]['source']],entries[address]['name'])
                from capstone import Cs,CS_ARCH_X86,CS_MODE_32
                from capstone.x86_const import X86_OP_IMM
                decoder=Cs(CS_ARCH_X86,CS_MODE_32);decoder.detail=True
                for ref in detail['function_references']:
                    data=bytes.fromhex(ref['bytes']);ins=list(decoder.disasm(data,int(ref['address'],16)))
                    if (machine_bytes(int(ref['address'],16),len(data))!=data or len(ins)!=1 or
                        not any(o.type==X86_OP_IMM and o.imm==int(ref['target'],16) for o in ins[0].operands) or
                        not re.search(r'\b'+re.escape(ref['name'])+r'\b',body)):
                        raise ValueError('Native pump callback/reference operand mismatch')
            for address,detail in pump_core['repairs'].items():
                entry=next(f for f in manifest['accepted_functions'] if f['address']==address)
                body=function_body(sources[entry['source']],entry['name'])
                if (entry.get('pump_callee_binding_2022')!={k:v for k,v in detail.items() if k!='previous_record'} or
                    entry.get('reconstruction_body_sha256')!=detail['body_sha256'] or
                    any(re.search(r'\b'+name+r'\s*\(',body) for name in detail['calls'])):
                    raise ValueError('Native subtype frontend binding mismatch')
                target_validation_input('',{**detail,'changes':[]},address,machine_bytes)
                for call in detail['canonical_calls']:
                    at=int(call['address'],16);data=bytes.fromhex(call['bytes'])
                    if (len(data)!=5 or data[0] not in {0xe8,0xe9} or machine_bytes(at,5)!=data or
                        at+5+struct.unpack('<i',data[1:])[0]!=int(call['target'],16) or
                        not re.search(r'\b'+re.escape(call['name'])+r'\s*\(',body)):
                        raise ValueError('Native subtype frontend call/jump mismatch')
            for address,detail in pump_core['replacements'].items():
                if entries[address]['reconstruction_body_sha256']!=detail['body_sha256'] or entries[address]['name']!=pump_core['previous_records'][address]['name']:
                    raise ValueError('Native pump existing scalar entry replacement mismatch')
            ps=pump_core['parameters']['21ecf20']
            if ([p['name'] for p in ps]!=['player','card','who_gains','event'] or
                [p['location'][0]['expression'] for p in ps[:3]]!=['(DW_OP_reg0 (r0))','(DW_OP_reg2 (r2))','(DW_OP_reg1 (r1))'] or
                not all('DW_OP_GNU_parameter_ref' in x['expression'] for x in ps[3]['location'])):
                raise ValueError('Spirit Link physical/source parameter mismatch')
            for site in pump_core['control_sites']:
                data=bytes.fromhex(site['bytes'])
                if machine_bytes(int(site['address'],16),len(data))!=data:
                    raise ValueError('Native pump record-copy/slot operand mismatch')
            sites={s['address']:s['bytes'] for s in pump_core['control_sites']}
            if any(sites.get(a)!=b for a,b in {'26a1551':'b915000000','26a157a':'f3a5','26a162b':'b915000000','26a1635':'f3a5','26a24a1':'8b83b8000000','26a24ab':'8b83b4000000','267ae15':'8b7224','267ae25':'89b0b4000000'}.items()):
                raise ValueError('Native pump field/copy semantics mismatch')
            color=pump_core['native_color_slot'];serialization=pump_core['pump_serialization']
            if ((color['tested_offset'],color['loaded_offset'])!=(184,180) or serialization!=dict(record_bytes=48,subtype_offset=36,color_offset=40,subtype_target_offset=180,color_target_offset=184,unserialized_disabling_offset=44)):
                raise ValueError('Native pump subtype/color serialization mismatch')
        except (ValueError,OSError,KeyError,StopIteration) as error:
            issues.append(dict(source='functions/functions.c',name='pump_core_2022',error=str(error)))
    activated=interfaces.get('activated_core_2022')
    if activated:
        try:
            from recover_activated_core import ADDRESSES
            from activated_core_bodies import STORAGE
            entries={f['address']:f for f in manifest['accepted_functions'] if f.get('activated_core_evidence')}
            if set(entries)!=set(ADDRESSES) or set(entries)!=set(activated['functions']) or any(f['static'] for f in entries.values()):
                raise ValueError('Native activated-ability core membership/linkage mismatch')
            for address,detail in activated['functions'].items():
                if entries[address]['activated_core_evidence']!=detail:
                    raise ValueError('Native activated-ability evidence mismatch')
            fixed={(v['name'],v['type'],v['location']) for v in activated['locals']['269c8c0'] if isinstance(v['location'],str)}
            if not {('td_default','target_definition_t','(DW_OP_fbreg: -236)'),('test','test_definition_t','(DW_OP_fbreg: -236)'),('tgt','target_t','(DW_OP_fbreg: -236)')}<=fixed:
                raise ValueError('Native activated-ability shared storage location mismatch')
            if activated['storage_declaration']!=STORAGE or sources['functions/functions.c'].replace('\r\n','\n').count(STORAGE)!=1:
                raise ValueError('Native activated-ability shared storage declaration mismatch')
            if activated['flags']['bytes']!=4 or activated['zero_flags']!=dict(SF_0=0,SF2_0=0) or activated['added_flag_members']!=dict(GAA_DURING_COMBAT=4096,GAA_MANA_ABILITY=2097152):
                raise ValueError('Native activated-ability enum domain mismatch')
            if activated['mana_payment_array']!=dict(type='int[8]',elements=8,element_bytes=4,bytes=32,symbols=[dict(name='_pay_mana_xbugrwaU',address='4ef400',storage=2)]):
                raise ValueError('Native activation mana-payment array mismatch')
            if not re.search(r'_pay_mana_xbugrwaU\s*=\s*0x4EF400;', (root/'manalink.lds').read_text()):
                raise ValueError('Native activation mana-payment linker binding mismatch')
        except (ValueError,OSError,KeyError) as error:
            issues.append(dict(source='functions/functions.c',name='activated_core_2022',error=str(error)))
    global_tutor=interfaces.get('global_tutor_core_2022')
    if global_tutor:
        try:
            from audit_global_tutor import audit_component
            audit_component(global_tutor,manifest,sources,machine_bytes,root)
        except (ValueError,OSError,KeyError,StopIteration) as error:
            issues.append(dict(source='functions/tutors.c',name='global_tutor_core_2022',error=str(error)))
    action_core=interfaces.get('action_core_2022')
    if action_core:
        try:
            from audit_action_core import audit_component
            audit_component(action_core,manifest,sources,machine_bytes,image,root)
        except (ValueError,OSError,KeyError,StopIteration) as error:
            issues.append(dict(source='functions/manipulate_and_damage_all.c',name='action_core_2022',error=str(error)))
    death_trigger=interfaces.get('death_trigger_core_2022')
    if death_trigger:
        try:
            from audit_death_trigger_core import audit_component
            audit_component(death_trigger,manifest,machine_bytes)
        except (ValueError,OSError,KeyError) as error:
            issues.append(dict(source='functions/functions.c',name='death_trigger_core_2022',error=str(error)))
    dependency_repairs=interfaces.get('dependency_repairs_2022')
    if dependency_repairs:
        try:
            from audit_dependency_repairs import audit_component
            audit_component(dependency_repairs,manifest,machine_bytes,root)
        except (ValueError,OSError,KeyError) as error:
            issues.append(dict(source='functions/',name='dependency_repairs_2022',error=str(error)))
    spell_trigger=interfaces.get('spell_trigger_native_2022')
    if spell_trigger:
        try:
            from audit_spell_trigger_native import audit_component
            audit_component(spell_trigger,manifest,sources,machine_bytes)
        except (ValueError,OSError,KeyError) as error:
            issues.append(dict(source='functions/functions.c',name='spell_trigger_native_2022',error=str(error)))
    phyrexian=interfaces.get('phyrexian_costs_2022')
    if phyrexian:
        try:
            from audit_phyrexian_costs import audit_component
            audit_component(phyrexian,manifest,sources,machine_bytes,image)
        except (ValueError,OSError,KeyError) as error:
            issues.append(dict(source='cards/new_phyrexia.c',name='phyrexian_costs_2022',error=str(error)))
    ward_guard=interfaces.get('ward_recursion_guard_2022')
    if ward_guard:
        try:
            from recover_ward_guard import ADDRESS,GLOBAL,DECLARATION,SOURCE
            entries=[f for f in manifest['accepted_functions'] if f.get('ward_guard_evidence')]
            if len(entries)!=1 or entries[0]['address']!=ADDRESS or entries[0]['static'] or entries[0]['ward_guard_evidence']!=ward_guard['function']:
                raise ValueError('Ward guard function evidence/membership mismatch')
            record=ward_guard['global_record'];declaration=record['declaration'];address=int(declaration['target_address'],16)
            if (record['bytes']!=8 or record['members']!=dict(player=0,card=4) or record['initial_bytes']!='ffffffffffffffff' or
                machine_bytes(address,8)!=bytes.fromhex(record['initial_bytes']) or record['location']!=[3]+list(struct.pack('<I',address)) or
                len(record['symbols'])!=1 or record['symbols'][0]['address']!=declaration['target_address'] or record['symbols'][0]['storage']!=3):
                raise ValueError('Ward guard static pair native layout/initializer mismatch')
            if declaration not in manifest['source_globals'] or declaration['declaration']!=DECLARATION or declaration['source']!=SOURCE or declaration['name']!=GLOBAL:
                raise ValueError('Ward guard static pair source registry mismatch')
            if ward_guard['function_symbols']!=[dict(name='_this_effect_doesnt_remove_this',storage=2)] or ward_guard['target_flags']!=dict(bytes=84,offset=80,mask=3) or ward_guard['regen_context']!=dict(offset=40,mask=0x8000000):
                raise ValueError('Ward guard public linkage/target/context mismatch')
            if len(record['sites'])!=7 or {s['field_offset'] for s in record['sites']}!={0,4}:
                raise ValueError('Ward guard native word reference membership mismatch')
            for site in record['sites']:
                data=bytes.fromhex(site['bytes'])
                if site['field_bytes']!=4 or machine_bytes(int(site['address'],16),len(data))!=data:
                    raise ValueError('Ward guard native word reference mismatch')
        except (ValueError,OSError,KeyError) as error:
            issues.append(dict(source='functions/auras.c',name='ward_recursion_guard_2022',error=str(error)))
    frontier=interfaces.get('dependency_frontier_2022')
    if frontier:
        try:
            entries={f['address']:f for f in manifest['accepted_functions'] if f.get('native_dependency_bindings_2022')}
            targets={f['address']:f for f in manifest['accepted_functions']}
            if set(entries)!=set(frontier['functions']):
                raise ValueError('Dependency frontier evidence membership mismatch')
            for address,detail in frontier['functions'].items():
                entry=entries[address]
                if entry['native_dependency_bindings_2022']!=detail:
                    raise ValueError('Dependency frontier function evidence mismatch')
                for reference in detail['canonical_calls']+detail['function_references']:
                    target=targets[reference['target']]
                    if target['name']!=reference['name'] or (target['static'] and target['source']!=entry['source']):
                        raise ValueError('Dependency frontier private owner/callee identity mismatch')
        except (ValueError,OSError,KeyError) as error:
            issues.append(dict(source='cards/',name='dependency_frontier_2022',error=str(error)))
    level_buddy=interfaces.get('level_buddy_2022')
    if level_buddy:
        try:
            from audit_level_buddy import audit_component
            audit_component(level_buddy,manifest,sources,machine_bytes,image)
        except (ValueError,OSError,KeyError) as error:
            issues.append(dict(source='cards/rise_of_the_eldrazi.c',name='level_buddy_2022',error=str(error)))
    entry_triggers=interfaces.get('entry_triggers_2022')
    if entry_triggers:
        try:
            from audit_entry_triggers import audit_component
            audit_component(entry_triggers,manifest,sources,machine_bytes)
        except (ValueError,OSError,KeyError) as error:
            issues.append(dict(source='functions/functions.c',name='entry_triggers_2022',error=str(error)))
    attacker=interfaces.get('attacker_core_2022')
    if attacker:
        try:
            from audit_attacker_core import audit_component
            audit_component(attacker,manifest,sources,machine_bytes,image)
        except (ValueError,OSError,KeyError) as error:
            issues.append(dict(source='functions/functions.c',name='attacker_core_2022',error=str(error)))
    costs=interfaces.get('activation_costs_2022')
    if costs:
        try:
            from audit_activation_costs import audit_component
            audit_component(costs,manifest,sources,machine_bytes,image)
        except (ValueError,OSError,KeyError) as error:
            issues.append(dict(source='functions/functions.c',name='activation_costs_2022',error=str(error)))
    sacrifice=interfaces.get('sacrifice_core_2022')
    if sacrifice:
        try:
            from sacrifice_core_bodies import STORAGE
            entries={f['address']:f for f in manifest['accepted_functions'] if f.get('sacrifice_core_evidence')}
            if len(entries)!=36 or set(entries)!=set(sacrifice['functions']) or sum(f['static'] for f in entries.values())!=6:
                raise ValueError('Sacrifice component membership mismatch')
            expected=dict(SAC_NO_CANCEL=1,SAC_DONE=2,SAC_JUST_MARK=4,SAC_ALL_OR_NONE=8,SAC_AS_COST=16,SAC_CAUSED=32,SAC_RETURN_CHOICE=64,SAC_OPTIONAL=128,SAC_SET_TARGETS=256)
            if sacrifice['enum']!=dict(bytes=4,members=expected) or sacrifice['retained_set_targets_value']!=128:
                raise ValueError('Sacrifice native/legacy flag-domain mismatch')
            if sacrifice['storage_declarations']!=STORAGE or sources['functions/sacrifices.c'].replace('\r\n','\n').count(STORAGE)!=1:
                raise ValueError('Sacrifice shared storage declaration mismatch')
            global_proof=sacrifice['test_global']
            if (global_proof['address'],global_proof['pointer_bytes'],global_proof['symbols'])!=('279529c',4,[dict(name='_sacrifice_test',address='279529c',storage=3)]):
                raise ValueError('Sacrifice native test-global binding mismatch')
            locals=sacrifice['locals']
            worker={(x['name'],x['type'],x['location']) for x in locals['26e4110'] if isinstance(x['location'],str)}
            if not {('rval','int','(DW_OP_fbreg: -532)'),('tgt','target_t','(DW_OP_fbreg: -532)'),('token','token_generation_t','(DW_OP_fbreg: -532)'),
                    ('marked','char[151]','(DW_OP_fbreg: -371)'),('default_prompt','char[200]','(DW_OP_fbreg: -220)')}<=worker:
                raise ValueError('Sacrifice native shared-storage offsets mismatch')
            picker={(x['name'],x['type'],x['location']) for x in locals['26e4c50'] if isinstance(x['location'],str)}
            if not {('prompt','char[200]','(DW_OP_fbreg: -236)'),('test','test_definition_t','(DW_OP_fbreg: -236)')}<=picker:
                raise ValueError('Sacrifice prompt/test shared storage mismatch')
            cause=sacrifice['parameters']['26e3990'];prompt=sacrifice['parameters']['26e3a30']
            if (len(cause)!=2 or cause[0]['location'][0]['expression']!='(DW_OP_reg0 (r0))' or
                not all('DW_OP_GNU_parameter_ref' in x['expression'] for x in cause[1]['location']) or
                [p['name'] for p in prompt]!=['prompt','type','invert','maxlen'] or prompt[-1]['location'] is not None or prompt[-1]['constant']!=200):
                raise ValueError('Sacrifice physical/abstract parameter mismatch')
            for address,detail in sacrifice['functions'].items():
                if entries[address]['sacrifice_core_evidence']!=detail:
                    raise ValueError('Sacrifice component evidence mismatch')
                for site in detail['typed_storage_sites']:
                    data=bytes.fromhex(site['bytes'])
                    if machine_bytes(int(site['address'],16),len(data))!=data:
                        raise ValueError('Sacrifice typed storage native operand mismatch')
            token_fields={s['address']:s['bytes'] for s in sacrifice['functions']['26e4110']['typed_storage_sites']}
            if any(token_fields.get(a)!=b for a,b in {'26e48ef':'898508feffff','26e48f5':'c6851ffeffff02','26e48fc':'c78524feffff10000000'}.items()):
                raise ValueError('Sacrifice token controller/quantity/action stores mismatch')
        except (ValueError,OSError,KeyError,StopIteration) as error:
            issues.append(dict(source='functions/sacrifices.c',name='sacrifice_core_2022',error=str(error)))
    consumers=interfaces.get('test_consumers_2022')
    if consumers:
        try:
            entries={f['address']:f for f in manifest['accepted_functions'] if f.get('test_consumer_evidence')}
            if len(entries)!=19 or set(entries)!=set(consumers['functions']):
                raise ValueError('Test consumer component membership mismatch')
            for address,detail in consumers['functions'].items():
                if entries[address]['test_consumer_evidence']!=detail:
                    raise ValueError('Test consumer component evidence mismatch')
            hand=next(x for x in consumers['locals']['2651300'] if x['name']=='hand_index')
            if hand['type']!='int[150]' or hand['decoded']!='(DW_OP_fbreg: -620)':
                raise ValueError('Test consumer hand storage evidence mismatch')
            expansion=consumers['show_deck_expansion_global']
            if (expansion['bytes'],expansion['zero_value'],expansion['symbols'])!=(4,0,[dict(name='_hack_show_deck_force_expansion',address='2774164',storage=2)]):
                raise ValueError('Test consumer deck expansion global mismatch')
            legacy=consumers['legacy_compatibility']
            compatibility=function_body(sources[legacy['source']],legacy['name'])
            if (legacy['counted_implementation'] or (legacy['retained_test_bytes'],legacy['native_test_bytes'])!=(208,216) or
                hashlib.sha256(compatibility.replace('\r\n','\n').encode()).hexdigest()!=legacy['body_sha256'] or
                re.search(r'\b'+re.escape(legacy['name'])+r'\s*\(',compatibility[compatibility.index('{'):]) or
                'test_definition_2022_t' in compatibility):
                raise ValueError('Legacy to-array compatibility body mismatch')
            migrated=consumers['migrated_native_wrapper']
            if entries[migrated['address']]['name']!=migrated['name']:
                raise ValueError('Test consumer native wrapper identity mismatch')
            all_entries={f['address']:f for f in manifest['accepted_functions']}
            for address,repair in consumers['repairs'].items():
                if all_entries[address]['compiler_part_binding_2022']!=repair:
                    raise ValueError('Compiler-part repair component mismatch')
            reused=consumers['reused_global'];site=reused['native_site']
            if (reused['declaration'] not in sources[reused['source']] or
                machine_bytes(int(site['address'],16),len(bytes.fromhex(site['bytes']))).hex()!=site['bytes']):
                raise ValueError('Test consumer reused rules-engine global mismatch')
        except (ValueError,OSError,KeyError,StopIteration) as error:
            issues.append(dict(source='functions/tutors.c',name='test_consumers_2022',error=str(error)))
    target_selection = interfaces.get('target_selection_2022')
    if target_selection:
        try:
            from target_selection_bodies import STORAGE
            if target_selection['shared_storage_declaration'] != STORAGE or target_selection['shared_storage_bytes'] != 480:
                raise ValueError('Target selection shared storage declaration mismatch')
            selector_locals = {(v['name'], v['type'], v['frame_offset']) for v in target_selection['locals']['265f3d0']}
            if not {('err_reason', 'char[208]', -500), ('illegal_tgt', 'char[200]', -500), ('candidates', 'target_t[60]', -500),
                    ('illegal_tgt', 'char[200]', -700), ('tmp_tgt', 'target_t', -708), ('unknown_v44', 'int', -712)} <= selector_locals:
                raise ValueError('Target selection native stack aliases mismatch')
            if len(target_selection['parameters']['265f3d0']) != 23 or len(target_selection['parameters']['265fb20']) != 23:
                raise ValueError('Target selection physical parameter count mismatch')
            bounds = target_selection['candidate_bound_instructions']
            if [s['bytes'] for s in bounds[:2]] != ['47', '83ff39'] or bounds[2]['instruction'] != 'jg 0x265f755':
                raise ValueError('Target selection 58-card candidate bound mismatch')
            for site in bounds:
                data = bytes.fromhex(site['bytes'])
                if machine_bytes(int(site['address'], 16), len(data)) != data:
                    raise ValueError('Target selection candidate bound instruction mismatch')
            thunk = target_selection['fscanf']
            if (thunk['thunk'], thunk['bytes'], thunk['iat'], thunk['dll'], thunk['name'], thunk['fixed_arguments'], thunk['variadic']) != ('2701e58', 'ff25b4547b02', '27b54b4', 'msvcrt.dll', 'fscanf', 2, True):
                raise ValueError('Target selection fscanf import interface mismatch')
            if machine_bytes(int(thunk['thunk'], 16), 6).hex() != thunk['bytes']:
                raise ValueError('Target selection fscanf import thunk mismatch')
            image.pe.parse_data_directories()
            imports = [(lib.dll.decode(), imp.name.decode(), imp.address) for lib in image.pe.DIRECTORY_ENTRY_IMPORT
                       for imp in lib.imports if imp.name == b'fscanf']
            if imports != [('msvcrt.dll', 'fscanf', 0x27b54b4)]:
                raise ValueError('Target selection fscanf IAT identity mismatch')
            if sum(c['name'] == 'fscanf' for c in target_selection['functions']['2660930']['canonical_calls']) != 8:
                raise ValueError('Target selection fscanf call count mismatch')
            for detail in target_selection['functions'].values():
                for change in detail['changes']:
                    for operand in change.get('pointer_operands', []):
                        for site in operand['sites']:
                            data = bytes.fromhex(site['bytes'])
                            if machine_bytes(int(site['address'], 16), len(data)) != data:
                                raise ValueError('Target selection live engine operand mismatch')
            entries = {f['address']: f for f in manifest['accepted_functions'] if f.get('target_selection_evidence')}
            if len(entries) != 18 or set(entries) != set(target_selection['functions']):
                raise ValueError('Target selection component membership mismatch')
            for address, detail in target_selection['functions'].items():
                if entries[address]['target_selection_evidence'] != detail:
                    raise ValueError('Target selection component evidence mismatch')
            migration = target_selection['chooser_migration']
            if entries[migration['address']]['reconstruction_body_sha256'] != migration['body_sha256']:
                raise ValueError('Target selection chooser migration hash mismatch')
        except (ValueError, OSError, KeyError) as error:
            issues.append(dict(source='functions/targets.c', name='target_selection_2022', error=str(error)))
    target_validation = interfaces.get('target_validation_2022')
    if target_validation:
        try:
            local = target_validation['validator_locals']
            if (local['error_str']['type'], local['error_str']['frame_offset'],
                local['buffer']['type'], local['buffer']['frame_offset'], target_validation['frame_base_bias_from_ebp']) != ('char[200]', -220, 'char[100]', -320, 8):
                raise ValueError('Target validator native buffer layout mismatch')
            if target_validation['predicate']['return_type'] != 'const char *' or target_validation['predicate']['parameters'] != ['int'] * 5:
                raise ValueError('Target validator predicate interface mismatch')
            physical = target_validation['parameters']
            if (len(physical['265d720']['physical']) != 23 or len(physical['265ef60']['physical']) != 22 or
                len(physical['265f1f0']['physical']) != 22 or
                physical['265f1f0']['abstract_constants'] != [dict(name='test_damage_target_or_source', value=0, physical_argument=False)]):
                raise ValueError('Target validator/availability physical argument mismatch')
            target_entries = {f['address']: f for f in manifest['accepted_functions'] if f.get('target_validation_evidence')}
            if set(target_entries) != set(target_validation['functions']) or len(target_entries) != 14:
                raise ValueError('Target validation component membership mismatch')
            for address, detail in target_validation['functions'].items():
                if target_entries[address]['target_validation_evidence'] != detail:
                    raise ValueError('Target validation component evidence mismatch')
            getter_binding = target_validation['protection_getter_binding']
            getter_entry = next(f for f in manifest['accepted_functions'] if f['address'] == getter_binding['address'])
            if getter_entry['target_getter_binding_2022'] != getter_binding:
                raise ValueError('Target validation protection-getter migration evidence mismatch')
        except (ValueError, OSError, KeyError) as error:
            issues.append(dict(source='functions/targets.c', name='target_validation_2022', error=str(error)))
    target_initializers = interfaces.get('target_initializers_2022')
    if target_initializers:
        try:
            target_layout = target_initializers['layout']
            if (target_layout['bytes'] != 84 or len(target_layout['members']) != 21 or
                any(m['offset'] != i * 4 or m['type'] != 'int32_t' or m['bytes'] != 4
                    for i, m in enumerate(target_layout['members'])) or
                target_layout['members'][-1]['name'] != 'flags'):
                raise ValueError('2022 target definition layout mismatch')
            retained = target_initializers['retained_legacy_wrapper']
            retained_body = function_body(sources[retained['source']], retained['name'])
            if hashlib.sha256(retained_body.replace('\r\n', '\n').encode()).hexdigest() != retained['body_sha256']:
                raise ValueError('Retained legacy target wrapper changed')
            initializer_entries = {f['address']: f for f in manifest['accepted_functions'] if f.get('target_initializer_evidence')}
            if set(initializer_entries) != set(target_initializers['functions']):
                raise ValueError('Target initializer evidence membership mismatch')
            for address, detail in target_initializers['functions'].items():
                if initializer_entries[address]['target_initializer_evidence'] != detail:
                    raise ValueError('Target initializer evidence mismatch')
        except (ValueError, OSError, KeyError) as error:
            issues.append(dict(source='functions/targets.c', name='target_initializers_2022', error=str(error)))
    subtype_mutators=interfaces.get('extension_subtype_mutators_2022')
    if subtype_mutators:
        try:
            types=subtype_mutators['types']
            if (types['card_ext_t']['bytes']!=192 or types['card_ext_t']['members']['subtype']!={'offset':8,'type':'uint16_t[12]'} or
                types['card_ext_t']['members']['inst_info_data']!={'offset':32,'type':'inst_info_t[6]'} or
                types['inst_info_t']['members']['id']!={'offset':0,'type':'uint16_t'}):
                raise ValueError('Extension subtype/info array layout mismatch')
            for at,expected_hex in subtype_mutators['machine_checks'].items():
                data=bytes.fromhex(expected_hex)
                if machine_bytes(int(at,16),len(data))!=data:
                    raise ValueError('Extension subtype bounds/unsigned-range instruction mismatch')
        except (ValueError,OSError,KeyError) as error:
            issues.append(dict(source='functions/has_creature_type.c',name='extension_subtype_mutators_2022',error=str(error)))
    token_stack_master=interfaces.get('token_stack_cards_2022')
    if token_stack_master:
        try:
            for at,expected_hex in token_stack_master['machine_checks'].items():
                data=bytes.fromhex(expected_hex)
                if machine_bytes(int(at,16),len(data))!=data:
                    raise ValueError('Token stack slot/prompt/predicate instruction mismatch')
            prompt=token_stack_master['prompt'];data=bytes.fromhex(prompt['bytes'])
            if (len(data)!=24 or not data.endswith(b'\0') or machine_bytes(int(prompt['address'],16),24)!=data or
                (prompt['copy_bytes'],prompt['zero_bytes'],prompt['total_bytes'])!=(24,76,100)):
                raise ValueError('Varina exact prompt-copy/clear mismatch')
            shared=token_stack_master['existing_shared_interface']
            if (shared['prototype'] not in (root/'manalink.h').read_text() or
                'int has_no_mana_cost(int player, int card){' not in sources[shared['source']] or shared['new_implementations']!=0):
                raise ValueError('Retained no-mana-cost predicate interface mismatch')
        except (ValueError,OSError,KeyError) as error:
            issues.append(dict(source='cards/commander_2018.c',name='token_stack_cards_2022',error=str(error)))
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
