"""Recover the 76-byte token interface and its complete versioned creation/copy core."""
import argparse
import csv
import hashlib
import json
import re
import shutil
import subprocess
from pathlib import Path
from capstone import Cs, CS_ARCH_X86, CS_MODE_32
from inspect_release import Image, value
from export_abi import type_of, type_name
from promote_recovery import clean, strip_comments, UNRESOLVED, LITERAL
from audit_source import function_body

NAMES = {'default_token_definition':'default_token_definition_2022',
         'copy_token_definition':'copy_token_definition_2022', 'generate_token':'generate_token_2022',
         'resolve_iid':'resolve_token_iid_2022', 'set_token':'set_token_2022',
         'get_copyable_special_infos':'get_copyable_special_infos_part_2022',
         'token_characteristic_setting_effects':'token_characteristic_setting_effects_2022',
         'generic_token':'generic_token_2022'}


def rewrite(text):
    names = {**NAMES, 'token_generation_t':'token_generation_2022_t', 'token_action_t':'token_action_2022_t'}
    pattern = re.compile(LITERAL + r'|\b(?:TOKEN_ACTION_\w+|' + '|'.join(names) + r')\b')
    return pattern.sub(lambda m: (m[0].replace('TOKEN_ACTION_', 'TOKEN2022_ACTION_') if m[0].startswith('TOKEN_ACTION_') else names.get(m[0], m[0])), text)


def main():
    parser = argparse.ArgumentParser(__doc__)
    parser.add_argument('--apply', action='store_true')
    args = parser.parse_args()
    root = Path(__file__).resolve().parents[2]
    cache = root / '.recovery'
    source = 'functions/token_generation.c'
    debug_data = Path('G:/Manalink/ManalinkEh_10_22_Hotfix.dbg').read_bytes()
    debug = Image(debug_data)
    runtime = Image((cache / '2022-ManalinkEh.dll').read_bytes())
    layout, actions, callbacks, sf2_zero, keyword_word = None, None, {}, False, None
    for cu in debug.dwarf().iter_CUs():
        if value(cu.get_top_DIE(), 'DW_AT_name') != source:
            continue
        for die in cu.get_top_DIE().iter_children():
            if die.tag == 'DW_TAG_enumeration_type':
                members = {value(c, 'DW_AT_name'): value(c, 'DW_AT_const_value') for c in die.iter_children()}
                if 'SF2_0' in members:
                    assert members['SF2_0'] == 0
                    sf2_zero = True
            if die.tag != 'DW_TAG_typedef':
                continue
            name = value(die, 'DW_AT_name')
            if name == 'card_instance_t':
                record = type_of(die)
                for member in record.iter_children():
                    if member.tag == 'DW_TAG_member' and value(member,'DW_AT_name') == 'unused0x124':
                        assert value(member,'DW_AT_data_member_location') == 292 and type_name(type_of(member)) == 'uint32_t'
                        keyword_word = dict(die=hex(member.offset),offset=292,bytes=4,type='uint32_t')
            if name == 'token_action_t':
                enum = type_of(die)
                assert value(enum, 'DW_AT_byte_size') == 4
                actions = dict(die=hex(enum.offset), bytes=4, members={value(c, 'DW_AT_name'):value(c, 'DW_AT_const_value') for c in enum.iter_children()})
            if name == 'token_generation_t':
                record = type_of(die)
                layout = dict(die=hex(record.offset), bytes=value(record, 'DW_AT_byte_size'), members=[])
                for member in record.iter_children():
                    if member.tag != 'DW_TAG_member':
                        continue
                    member_name = value(member, 'DW_AT_name')
                    typ = type_of(member)
                    layout['members'].append(dict(name=member_name, type=type_name(typ), offset=value(member, 'DW_AT_data_member_location')))
                    if member_name.startswith('special_code_'):
                        signature = type_of(typ)
                        callbacks[member_name] = dict(return_type=type_name(type_of(signature)), arguments=[type_name(type_of(p)) for p in signature.iter_children() if p.tag == 'DW_TAG_formal_parameter'])
        break
    assert layout and layout['bytes'] == 76 and len(layout['members']) == 22 and sf2_zero and keyword_word
    assert actions and len(actions['members']) == 16
    assert actions['members']['TOKEN_ACTION_HASTE'] == 16 and actions['members']['TOKEN_ACTION_CONVERT_INTO_ARTIFACT'] == 64
    assert callbacks == {'special_code_for_legacy':dict(return_type='int',arguments=['int','int','event_t']), 'special_code_on_generation':dict(return_type='void',arguments=['struct_token_generation_t *','int','int'])}
    fields = []
    for member in layout['members']:
        name, typ = member['name'], member['type']
        if name == 'special_code_for_legacy':
            fields.append('  int (*special_code_for_legacy)(int, int, event_t);')
        elif name == 'special_code_on_generation':
            fields.append('  void (*special_code_on_generation)(struct struct_token_generation_2022_t *, int, int);')
        else:
            assert 'UNRESOLVED' not in typ
            fields.append('  ' + ('token_action_2022_t' if typ == 'token_action_t' else typ) + ' ' + name + ';')
    addition = '\n#ifndef SHANDALAR\n/* October 2022 token creation ABI; legacy layout/actions retained above. */\ntypedef enum\n{\n' + ',\n'.join(f'  {name.replace("TOKEN_ACTION_", "TOKEN2022_ACTION_")} = {number}' for name,number in actions['members'].items()) + '\n} token_action_2022_t;\n\ntypedef struct struct_token_generation_2022_t\n{\n' + '\n'.join(fields) + '\n} token_generation_2022_t;\n#endif\n\n'
    defs = (root / 'defs.h').read_bytes().decode('latin1').replace('\r\n', '\n')
    assert 'token_generation_2022_t;' not in defs
    upkeep = re.search(r'(?m)^  uint8_t\tupkeep_colorless;.*?^  uint8_t\tupkeep_green;[^\n]*\n',defs,re.S)
    assert upkeep and len(re.findall(r'\buint8_t\b',upkeep[0])) == 4
    old_upkeep = upkeep[0]
    overlay = '  union\n  {\n    struct\n    {\n' + old_upkeep + '    };\n    uint32_t token_special_keywords_2022; /* Native token keyword mirror at 0x124. */\n  };\n'
    defs = defs[:upkeep.start()] + overlay + defs[upkeep.end():]
    at = defs.rfind('#endif')
    defs = defs[:at] + addition + defs[at:]
    metadata = {r['low_pc'][2:]:r for r in csv.DictReader((cache/'2022-functions.csv').open()) if r['low_pc'] and r['high_pc']}
    directories = {r['address']:cache/'decompiled-token-core' for r in csv.DictReader((cache/'decompiled-token-core/export.tsv').open(), delimiter='\t')}
    directories['2665900'] = cache/'decompiled-token-generic'
    assert len(directories) == 8
    checks = {'2664f7f':'8b1d80537a00', '2664fab':'8b8334707e00',
              '2664edf':'899024010000', '2665877':'8b8324010000',
              '2665501':'c7434800000000', '2665c33':'83f804',
              '2665a37':'894580', '2665a47':'894584', '2665a2a':'c74588ffffffff',
              '2665a40':'c7458c00000000', '2665a4d':'c74590ffffffff',
              '2665a54':'c7459400000000', '2665a62':'c74598ffffffff', '2665a69':'c7459c00000000',
              '26651f2':'b912000000', '266520f':'f3a5'}
    for at, expected_hex in checks.items():
        data = bytes.fromhex(expected_hex)
        assert runtime.pe.get_data(int(at,16)-0x2000000,len(data)) == data
    original_source = (root/source).read_bytes().decode('latin1').replace('\r\n','\n')
    assert 'sizeof(card_data_t) == 72' in defs
    assert 'EXE_DWORD(0x7a5380)' in (root/'functions/ai.c').read_text()
    linker = (root/'manalink.lds').read_bytes().decode('latin1').replace('\r\n','\n')
    assert '_engine_dynamic_iid_start_2022' not in linker
    linker += '\n/* Native dynamic card-type pool index used by the 2022 token resolver. */\n_engine_dynamic_iid_start_2022 = 0x7A5380;\n'
    entries, bodies, prototypes = [], {}, []
    decoder = Cs(CS_ARCH_X86,CS_MODE_32)
    proof = dict(debug_sha256=hashlib.sha256(debug_data).hexdigest(), layout=layout, actions=actions, callbacks=callbacks, keyword_word=dict(**keyword_word,original_declarations=old_upkeep,overlay=overlay), machine_checks=checks, native_dynamic_iid_word='7a5380', functions={}, scope='Separate 76-byte token definition and action enum with full creation/resolution/copy core. Preserve legacy 72-byte layout and action values. Three private helpers retain exact native interfaces; the special-info part helper is called only behind its original guard.', integration_pending='Original source/token consumers, callback producers and native/card dispatch bindings need coordinated downstream migration. No game execution performed.')
    proof['quantities_table'] = dict(records=4,words_per_record=2,bytes=32,scope='Retain the concrete eight-word local recovered from native stack stores and the four-record loop bound; do not invent a huge allocation from the optimized DWARF bound.')
    proof['special_flags3_core_note'] = 'The focused core exports initialize this field but contain no explicit read of it. Their behavior is retained; no additional consumer was inferred.'
    for address, directory in directories.items():
        row = metadata[address]
        assert row['source'] == source and row['name'] in NAMES
        raw = (directory/(address+'.c')).read_text()
        text = strip_comments(raw)
        modifications = []
        if address == '2664f30':
            pair = '      uVar2._0_2_ = cards_data[iVar12].id;\n      uVar2._2_2_ = cards_data[iVar12].reserved2;'
            assert text.count(pair) == 1
            text = text.replace(pair, '      uVar2 = (uint32_t)cards_data[iVar12].id | ((uint32_t)cards_data[iVar12].reserved2 << 16);')
            start = text.index('          pcVar14 = cards_data + int_id;')
            end = text.index('          iVar9 = token->pow;', start)
            original_copy = text[start:end]
            assert 'for (iVar9 = 0x12;' in original_copy
            text = text[:start] + '          cards_data[iVar11] = cards_data[int_id];\n' + text[end:]
            assert text.count('_DAT_007a5380') == 5
            text = text.replace('_DAT_007a5380', 'engine_dynamic_iid_start_2022')
            modifications.append(dict(kind='72-byte card-data copy', original_sha256=hashlib.sha256(original_copy.encode()).hexdigest(), bytes=72))
        text = text.replace('(_func_void_struct_token_generation_t_ptr_int_int *)0x0', 'NULL')
        text = text.replace('->unused0x124','->token_special_keywords_2022')
        text = text.replace('SF2_0 < value', '0 < value')
        text = text.replace('(char *)0x790074', 'EXE_STR(0x790074)')
        text = rewrite(text)
        text = clean(text, use_keyword_accessors_2022=True, use_extension_flags_2022=True, use_mana_api_2022=True)
        private = address in {'2664eb0','2664f30','2665380'}
        if private:
            text = 'static ' + text
        assert not UNRESOLVED.search(strip_comments(text)) and not re.search(r'\._\d|\.field_',text)
        text = subprocess.run(['C:/Program Files/LLVM/bin/clang-format.exe','-style={BasedOnStyle: LLVM, IndentWidth: 2, BreakBeforeBraces: Allman, ColumnLimit: 100}'],input=text.encode(),capture_output=True,check=True).stdout.decode().replace('\r\n','\n')
        code = runtime.pe.get_data(int(address,16)-0x2000000,int(row['high_pc'],16)-int(address,16))
        instructions = list(decoder.disasm(code,int(address,16)))
        assert sum(i.size for i in instructions) == len(code)
        prototype = text[:text.index('{')].strip()+';'
        detail = dict(machine_bytes=len(code),machine_sha256=hashlib.sha256(code).hexdigest(),modifications=modifications)
        proof['functions'][address] = detail
        entries.append(dict(name=NAMES[row['name']],original_name=row['name'],address=address,source=source,line=row['line'],static=private,prototype=prototype,raw_sha256=hashlib.sha256(raw.encode()).hexdigest(),recovered_literals=[],token_core_evidence=detail,reconstruction=proof['scope'],reconstruction_body_sha256=hashlib.sha256(function_body(text,NAMES[row['name']]).encode()).hexdigest()))
        bodies[address] = text
        prototypes.append(prototype)
    candidate = original_source + '\n\n/* October 2022 token creation component. */\n' + '\n'.join(prototypes)+'\n'
    for entry in entries:
        candidate += f"\n/* ManalinkEh 2022: 0x{entry['address']}; original {source}:{entry['line']}. */\n" + bodies[entry['address']]
    header = (root/'manalink.h').read_bytes().decode('latin1').replace('\r\n','\n')
    at = header.rfind('#ifdef __cplusplus')
    header = header[:at] + '/* October 2022 token creation interfaces. */\n#ifndef SHANDALAR\nextern int engine_dynamic_iid_start_2022;\n' + '\n'.join(e['prototype'] for e in entries if not e['static']) + '\n#endif\n\n' + header[at:]
    scratch = cache/'token-core-check'
    scratch.mkdir(exist_ok=True)
    for name,text in [('defs.h',defs),('manalink.h',header),('token_generation.c',candidate)]:
        (scratch/name).write_bytes(text.encode('latin1'))
    probe = '#include "manalink.h"\n#include <stddef.h>\n_Static_assert(sizeof(token_generation_t)==72 && sizeof(token_generation_2022_t)==76 && sizeof(card_instance_t)==300,"token/instance sizes");\n'
    probe += '\n'.join(f'_Static_assert(offsetof(token_generation_2022_t,{m["name"]})=={m["offset"]},"token member {m["name"]}");' for m in layout['members'])
    probe += '\n_Static_assert(offsetof(card_instance_t,token_special_keywords_2022)==292 && sizeof(((card_instance_t*)0)->token_special_keywords_2022)==4 && offsetof(card_instance_t,upkeep_colorless)==292 && offsetof(card_instance_t,upkeep_green)==295,"keyword mirror overlay");\n'
    (scratch/'layout.c').write_text(probe)
    subprocess.run(['C:/Program Files/LLVM/bin/clang.exe','-target','i686-pc-windows-msvc','-std=gnu99','-fsyntax-only','-I',str(scratch),'-I',str(root),str(scratch/'layout.c')],check=True)
    result = subprocess.run(['C:/Program Files/LLVM/bin/clang.exe','-target','i686-pc-windows-msvc','-std=gnu99','-fsyntax-only','-Werror=implicit-function-declaration','-Werror=incompatible-pointer-types','-Werror=return-type','-Werror=uninitialized','-Werror=sometimes-uninitialized','-Werror=format','-I',str(scratch),'-I',str(root),str(scratch/'token_generation.c')],capture_output=True,text=True)
    (scratch/'check.log').write_text(result.stderr)
    result.check_returncode()
    if args.apply:
        p = root/'docs/recovery-evidence/recovered-functions.json'
        manifest = json.loads(p.read_text())
        assert not set(bodies)&{f['address'] for f in manifest['accepted_functions']}
        (root/source).write_bytes(candidate.replace('\n','\r\n').encode('latin1'))
        (root/'defs.h').write_bytes(defs.replace('\n','\r\n').encode('latin1'))
        (root/'manalink.h').write_bytes(header.replace('\n','\r\n').encode('latin1'))
        (root/'manalink.lds').write_bytes(linker.replace('\n','\r\n').encode('latin1'))
        manifest['accepted_functions'].extend(entries)
        manifest['accepted'] = len(manifest['accepted_functions'])
        # Earlier scoped fragments may cite a different exact decompiler export
        # of the same function. Preserve that export before refreshing the cache.
        cited_exports = [*manifest.get('source_fragments', [])]
        cited_exports.extend(support for function in manifest['accepted_functions'] for support in function.get('supporting_exports', []))
        for fragment in cited_exports:
            address = fragment['address']
            if address not in directories:
                continue
            old_path = cache / fragment.get('raw_export', 'decompiled-abi/' + address + '.c')
            old_raw = old_path.read_text()
            assert hashlib.sha256(old_raw.encode()).hexdigest() == fragment['raw_sha256']
            saved = cache / 'decompiled-support' / (address + '-' + fragment['raw_sha256'] + '.c')
            saved.parent.mkdir(exist_ok=True)
            shutil.copyfile(old_path,saved) if old_path != saved else None
            fragment['raw_export'] = saved.relative_to(cache).as_posix()
        p.write_text(json.dumps(manifest,indent=2))
        p = root/'docs/recovery-evidence/interface-recovery.json'
        interfaces = json.loads(p.read_text())
        interfaces['token_core_2022'] = proof
        p.write_text(json.dumps(interfaces,indent=2))
        canonical_path = cache/'decompiled-abi/export.tsv'
        rows = list(csv.DictReader(canonical_path.open(),delimiter='\t'))
        by_address = {r['address']:r for r in rows}
        for address,directory in directories.items():
            row = next(r for r in csv.DictReader((directory/'export.tsv').open(),delimiter='\t') if r['address']==address)
            if address in by_address:
                by_address[address].update(row)
            else:
                rows.append(row)
            shutil.copyfile(directory/(address+'.c'),canonical_path.parent/(address+'.c'))
        with canonical_path.open('w',newline='') as stream:
            writer = csv.DictWriter(stream,fieldnames=rows[0],delimiter='\t')
            writer.writeheader();writer.writerows(rows)
    print(json.dumps(dict(applied=args.apply,implementations=8,token_bytes=76,action_values=16,native_aliases=1)))


if __name__ == '__main__':
    main()
