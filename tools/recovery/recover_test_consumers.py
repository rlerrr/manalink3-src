"""Recover typed graveyard/zone and counterspell consumers and three compiler parts.

Keeps the legacy test/target interfaces available while completing a native
216-byte test / 84-byte target dependency group. No game tests are run here.
"""
import argparse
import csv
import hashlib
import json
import re
import shutil
import subprocess
from pathlib import Path

from capstone import Cs, CS_ARCH_X86, CS_MODE_32
from capstone.x86_const import X86_OP_IMM
from elftools.dwarf.descriptions import describe_DWARF_expr
from inspect_release import Image, value, inherited_value
from export_abi import type_of, type_name
from promote_recovery import clean, strip_comments, UNRESOLVED, LITERAL
from audit_source import function_body
from recover_literals import recover_strings


CONSUMERS = '2650b80 2650cf0 2650de0 2651140 2651300 26515c0 2651660 2652b10 2652b90 26e62e0 26b9850 26b98e0 26b99a0 2692a40 2692b30 26b6500'.split()
PARTS = {'2060dd0': ('card_whirlpool_rider', ['2069960', '20699b0', '2069a60']),
         '2359940': ('escape', ['235a2e0']),
         '255fd50': ('amaug_leg', ['2560360'])}
PART_NAME = 'select_multiple_cards_from_graveyard_to_array_part_2022'


def sha(data):
    return hashlib.sha256(data).hexdigest()


def format_c(text):
    return subprocess.run(['C:/Program Files/LLVM/bin/clang-format.exe',
        '-style={BasedOnStyle: LLVM, IndentWidth: 2, BreakBeforeBraces: Allman, ColumnLimit: 100}'],
        input=text.encode(), capture_output=True, check=True).stdout.decode().replace('\r\n', '\n')


def read(path):
    return path.read_bytes().decode('latin1').replace('\r\n', '\n')


def rename(text, mapping):
    count = 0
    def replace(match):
        nonlocal count
        token = match[0]
        if token in mapping:
            count += 1
            return mapping[token]
        return token
    text = re.sub(LITERAL+r'|\b(?:'+'|'.join(mapping)+r')\b', replace, text)
    return text, count


def main():
    parser = argparse.ArgumentParser(__doc__)
    parser.add_argument('--apply', action='store_true')
    args = parser.parse_args()
    root = Path(__file__).resolve().parents[2]
    cache = root / '.recovery'
    directory = cache / 'decompiled-test-consumers'
    addresses = CONSUMERS + list(PARTS)
    debug_data = Path('G:/Manalink/ManalinkEh_10_22_Hotfix.dbg').read_bytes()
    debug = Image(debug_data)
    dwarf = debug.dwarf()
    runtime = Image((cache / '2022-ManalinkEh.dll').read_bytes())
    decoder = Cs(CS_ARCH_X86, CS_MODE_32)
    decoder.detail = True
    metadata = {r['low_pc'][2:]: r for r in csv.DictReader((cache / '2022-functions.csv').open()) if r['low_pc'] and r['high_pc']}
    manifest_path = root / 'docs/recovery-evidence/recovered-functions.json'
    manifest = json.loads(manifest_path.read_text())
    accepted = {f['address']: f for f in manifest['accepted_functions']}
    assert set(addresses) & set(accepted) == {'2652b90'}
    parameters, locals_by_address, globals_proof = {}, {}, {}
    for cu in dwarf.iter_CUs():
        for die in cu.get_top_DIE().iter_children():
            if (value(cu.get_top_DIE(),'DW_AT_name')=='functions/tutors.c' and
                die.tag=='DW_TAG_variable' and inherited_value(die,'DW_AT_name')=='hack_show_deck_force_expansion'):
                typ=type_of(die)
                while typ.tag in {'DW_TAG_typedef','DW_TAG_const_type'}:
                    typ=type_of(typ)
                assert value(typ,'DW_AT_byte_size')==4
                enum={value(c,'DW_AT_name'):value(c,'DW_AT_const_value') for c in typ.iter_children() if c.tag=='DW_TAG_enumerator'}
                assert enum['EXPANSION_0']==0
                symbols=[dict(name=n,address=f'{debug.pe.OPTIONAL_HEADER.ImageBase+debug.pe.sections[sec-1].VirtualAddress+off:x}',storage=s)
                         for n,off,sec,k,s in debug.symbols if sec>0 and n=='_hack_show_deck_force_expansion']
                assert symbols==[dict(name='_hack_show_deck_force_expansion',address='2774164',storage=2)]
                globals_proof=dict(bytes=4,zero_value=enum['EXPANSION_0'],symbols=symbols,
                    declaration='int32_t hack_show_deck_force_expansion;')
            at = value(die, 'DW_AT_low_pc')
            address = f'{at:x}' if at else ''
            if address not in addresses:
                continue
            ps, ls = [], []
            for child in die.iter_children():
                if child.tag not in {'DW_TAG_formal_parameter', 'DW_TAG_variable'}:
                    continue
                location = child.attributes.get('DW_AT_location')
                item = dict(name=inherited_value(child, 'DW_AT_name'), type=type_name(type_of(child)),
                            location=location.value if location else None, location_form=location.form if location else None)
                if location and location.form == 'DW_FORM_exprloc':
                    item['decoded'] = describe_DWARF_expr(location.value, dwarf.structs)
                if child.tag == 'DW_TAG_formal_parameter':
                    assert location and value(child, 'DW_AT_const_value') is None
                    ps.append(item)
                else:
                    ls.append(item)
            parameters[address], locals_by_address[address] = ps, ls
    assert set(parameters) == set(addresses)
    hand = next(x for x in locals_by_address['2651300'] if x['name'] == 'hand_index')
    assert hand['type'] == 'int[150]' and hand['decoded'] == '(DW_OP_fbreg: -620)'
    names = {a: metadata[a]['name'] + '_2022' for a in CONSUMERS}
    names['2651660'] = PART_NAME
    names.update({a: name + '_part_2022' for a, (name, _) in PARTS.items()})
    names['2060dd0']='whirlpool_rider_part_2022'
    mapping = {metadata[a]['name']: names[a] for a in CONSUMERS if a != '2651660'}
    targets = {int(a, 16): n for a, n in names.items()}
    targets.update({int(f['address'], 16): f['name'] for f in accepted.values()
                    if f['name'].endswith('_2022')})
    proof = dict(debug_sha256=sha(debug_data), parameters=parameters, locals=locals_by_address,
                 functions={}, repairs={}, legacy_compatibility={},
                 scope='Native graveyard/zone selection and counterspell consumers use the 216-byte test and 84-byte target domains. Separate compiler parts repair recursive name bindings in Whirlpool Rider/Drake/Warrior, escape and amaug_leg. The previously accepted to-array wrapper is migrated once and its legacy interface receives an explicitly uncounted compatibility body.',
                 integration_pending='Generic-spell still needs the native sacrifice family; two other compiler-part bindings need their own target/test consumers. No DLL build or game execution performed.')
    texts = {metadata[a]['source']: read(root / metadata[a]['source']) for a in addresses}
    old_texts = dict(texts)
    bodies, entries = {}, []
    for address in addresses:
        row = metadata[address]
        source = row['source']
        private = address in PARTS or address in {'2651660','2650b80'}
        assert any(s == (3 if private else 2) and
            debug.pe.OPTIONAL_HEADER.ImageBase + debug.pe.sections[sec-1].VirtualAddress + off == int(address, 16)
            for n, off, sec, k, s in debug.functions())
        raw_path = (directory if address in CONSUMERS else cache / 'decompiled-abi') / (address + '.c')
        raw = raw_path.read_text()
        text = strip_comments(raw)
        code = runtime.pe.get_data(int(address, 16)-0x2000000, int(row['high_pc'], 16)-int(address, 16))
        instructions = list(decoder.disasm(code, int(address, 16)))
        changes = []

        def replace(old, new, kind, sites=()):
            assert text.count(old) == 1, (address, old)
            change = dict(original=old, replacement=new, occurrences=1, kind=kind,
                          machine_sites=[dict(address=f'{i.address:x}', bytes=i.bytes.hex(), instruction=i.mnemonic+' '+i.op_str)
                                         for i in instructions if f'{i.address:x}' in sites])
            changes.append(change)
            return text.replace(old, new)

        if address == '2650de0':
            text = replace('str_Done', '(const char *)ENGINE_DONE_LABEL', 'live Done label', ['2650e4d'])
            text = replace('EXPANSION_0', '0', 'native zero expansion enumerator', ['2650e05'])
            # ESP subtracts exactly zone_count*4. The fixed outgoing call area
            # occupies 24 bytes below ai_pick. Keep the exported control flow.
            text = replace('  iVar2 = zone_count * -4;\n  piStack_28 = &iStack_34 + -zone_count;',
                           '  int ai_pick[zone_count];\n  piStack_28 = ai_pick;',
                           'native VLA allocation', ['2650ee6','2650ee9','2650eec','2650ef1','2650ef3','2650ef7'])
            # Recover each actual cdecl argument from its outgoing word store.
            blocks = [
                ('*(test_definition_t **)(&stack0xffffffc0 + iVar2) = this_test;\n        *(int *)(&stack0xffffffbc + iVar2) = test_type;\n        *(int *)(&stack0xffffffb8 + iVar2) = s_location[iVar5];\n        *(int *)(&stack0xffffffb4 + iVar2) = targ_player;\n        (&uStackY_50)[-zone_count] = 0x2650f47;\n        iVar1 = new_make_test(*(int *)(&stack0xffffffb4 + iVar2),*(int *)(&stack0xffffffb8 + iVar2),\n                              *(int *)(&stack0xffffffbc + iVar2),\n                              *(test_definition_t **)(&stack0xffffffc0 + iVar2));',
                 'iVar1 = new_make_test(targ_player,s_location[iVar5],test_type,this_test);', '2650f2a', '2650f47'),
                ('*(undefined4 *)(&stack0xffffffb4 + iVar2) = 100;\n        (&uStackY_50)[-zone_count] = 0x265102c;\n        iVar1 = internal_rand(*(int *)(&stack0xffffffb4 + iVar2));',
                 'iVar1 = internal_rand(100);', '2651020', '265102c'),
                ('*(undefined4 *)(&stack0xffffffb4 + iVar2) = 0xffffffff;\n        *(int *)(&stack0xffffffb8 + iVar2) = iVar1;\n        (&uStackY_50)[-zone_count] = 0x265105b;\n        iVar1 = good_to_put_in_grave\n                          (*(int *)(&stack0xffffffb4 + iVar2),*(int *)(&stack0xffffffb8 + iVar2));',
                 'iVar1 = good_to_put_in_grave(-1,iVar1);', '2651046', '265105b'),
                ('*(int *)(&stack0xffffffb4 + iVar2) = s_location[iVar5];\n            (&uStackY_50)[-zone_count] = 0x2651109;\n            iVar1 = get_cmc_by_internal_id(*(int *)(&stack0xffffffb4 + iVar2));\n            *(undefined4 *)(&stack0xffffffb8 + iVar2) = 0;\n            *(int *)(&stack0xffffffbc + iVar2) = iVar1;\n            *(undefined4 *)(&stack0xffffffb4 + iVar2) = 1;\n            (&uStackY_50)[-zone_count] = 0x2651121;\n            iVar3 = has_mana(*(int *)(&stack0xffffffb4 + iVar2),*(int *)(&stack0xffffffb8 + iVar2),\n                             *(int *)(&stack0xffffffbc + iVar2));',
                 'iVar1 = get_cmc_by_internal_id(s_location[iVar5]);\n            iVar3 = has_mana(1,0,iVar1);', '26510fc', '2651121'),
                ('*(int *)(&stack0xffffffb8 + iVar2) = s_location[iVar5];\n          *(undefined4 *)(&stack0xffffffb4 + iVar2) = 0xffffffff;\n          (&uStackY_50)[-zone_count] = 0x26510de;\n          iVar1 = get_base_value(*(int *)(&stack0xffffffb4 + iVar2),\n                                 *(int *)(&stack0xffffffb8 + iVar2));',
                 'iVar1 = get_base_value(-1,s_location[iVar5]);', '26510cb', '26510de'),
                ('*(int *)(&stack0xffffffb4 + iVar2) = s_location[iVar5];\n        (&uStackY_50)[-zone_count] = 0x26510bb;\n        iVar1 = get_cmc_by_internal_id(*(int *)(&stack0xffffffb4 + iVar2));',
                 'iVar1 = get_cmc_by_internal_id(s_location[iVar5]);', '26510ae', '26510bb'),
                ('*(int *)(&stack0xffffffb4 + iVar2) = s_location[iVar5];\n        (&uStackY_50)[-zone_count] = 0x265108a;\n        iVar1 = get_cmc_by_internal_id(*(int *)(&stack0xffffffb4 + iVar2));\n        *(undefined4 *)(&stack0xffffffb8 + iVar2) = 0;\n        *(int *)(&stack0xffffffbc + iVar2) = iVar1;\n        *(undefined4 *)(&stack0xffffffb4 + iVar2) = 1;\n        (&uStackY_50)[-zone_count] = 0x26510a2;\n        iVar3 = has_mana(*(int *)(&stack0xffffffb4 + iVar2),*(int *)(&stack0xffffffb8 + iVar2),\n                         *(int *)(&stack0xffffffbc + iVar2));',
                 'iVar1 = get_cmc_by_internal_id(s_location[iVar5]);\n        iVar3 = has_mana(1,0,iVar1);', '265107d', '26510a2'),
                ('*(int *)(&stack0xffffffb4 + iVar2) = iStack_1c;\n      (&uStackY_50)[-zone_count] = 0x265100d;\n      iVar2 = internal_rand(*(int *)(&stack0xffffffb4 + iVar2));',
                 'iVar2 = internal_rand(iStack_1c);', '2651005', '265100d')]
            for old, new, start, end in blocks:
                sites = [f'{i.address:x}' for i in instructions if int(start,16) <= i.address < int(end,16)]
                text = replace(old, new, 'VLA outgoing arguments', sites)
            # The high three bytes of these spills are never read as part of
            # the boolean; later full-word assignments have separate lifetimes.
            old = 'iStack_2c = CONCAT31(iStack_2c._1_3_,cVar4);'
            assert text.count(old) == 2
            changes.append(dict(original=old, replacement='iStack_2c = cVar4;', occurrences=2,
                                kind='low-byte boolean spill', machine_sites=[]))
            text = text.replace(old, 'iStack_2c = cVar4;')
            # Remove decompiler return-address/outgoing-SP scaffolding only.
            for pattern in [r'  undefined4 uStackY_50;\n', r'  undefined1 \*puStack_30;\n',
                            r'  puStack_30 = &stack0xffffffb4;\n', r'  +uStackY_50 = 0x[0-9a-f]+;\n']:
                for match in list(re.finditer(pattern, text))[::-1]:
                    text = replace(match[0], '', 'compiler call-frame bookkeeping')
            assert 'stack0x' not in text and 'CONCAT' not in text and 'uStackY' not in text
        if address == '2651300':
            text = replace('(int *)&stack0xfffffff0', 'hand_index + 150', 'DWARF hand-array end')
        if address == '2652b10':
            old='backtrace_and_abort("functions/tutors.c:1175\\nmax_targets > 0 && max_targets <= 19");'
            text=replace(old,old+'\n  __builtin_unreachable();','native nonreturning assertion')
        text, literals = recover_strings(text, runtime, {})
        if address == '2651660':
            text = re.sub(r'\bselect_multiple_cards_from_graveyard_to_array\b', PART_NAME, text)
        text, _ = rename(text, mapping)
        if address in ['2652b10', '2652b90']:
            head, tail = text.split('{', 1)
            tail, count = re.subn(r'\bselect_multiple_cards_from_graveyard_to_array_2022\b', PART_NAME, tail)
            assert count == 1
            text = head + '{' + tail
        if address in PARTS:
            original = PARTS[address][0]
            text, count = rename(text, {original:names[address]})
            assert count == 1, (address, original, count, text[:200])
        text = clean(text, use_test_api_2022=True, use_target_api_2022=True,
                     use_keyword_accessors_2022=True, use_extension_flags_2022=True)
        if private:
            text = 'static ' + text
        assert not UNRESOLVED.search(text) and not re.search(r'\._\d|stack0x', text), address
        body = format_c(text)
        bodies[address] = body
        calls = []
        for i in instructions:
            if i.mnemonic in {'call','jmp'} and i.operands[0].type == X86_OP_IMM and i.operands[0].imm in targets:
                destination = targets[i.operands[0].imm]
                assert re.search(r'\b'+destination+r'\s*\(', body), (address, destination)
                calls.append(dict(address=f'{i.address:x}', bytes=i.bytes.hex(), target=f'{i.operands[0].imm:x}', name=destination))
        detail = dict(machine_bytes=len(code), machine_sha256=sha(code), changes=changes, canonical_calls=calls)
        proof['functions'][address] = detail
        entries.append(dict(name=names[address], address=address, source=source, line=row['line'], static=private,
                            prototype=body[:body.index('{')].strip()+';', raw_sha256=sha(raw.encode()),
                            recovered_literals=literals, test_consumer_evidence=detail, reconstruction=proof['scope'],
                            reconstruction_body_sha256=sha(function_body(body,names[address]).encode())))

    # Preserve the legacy 208-byte to-array interface without converting records
    # or imposing the public card-target-array limit of 19. This is compatibility
    # code, not a second recovered native entry. All filtering stays on old APIs.
    source = 'functions/tutors.c'
    original = 'select_multiple_cards_from_graveyard_to_array'
    before = function_body(texts[source], original)
    legacy = bodies['2651660']
    legacy = legacy.replace('static int '+PART_NAME, 'int '+original)
    for old, new in {'test_definition_2022_t':'test_definition_t', 'new_default_test_definition_2022':'new_default_test_definition',
                     'new_select_a_card_2022':'new_select_a_card'}.items():
        legacy = re.sub(r'\b'+old+r'\b', new, legacy)
    brace = legacy.index('{')
    legacy = legacy[:brace+1]+'\n  if (max_targets == 0) return 0;\n'+legacy[brace+1:]
    legacy = format_c(legacy)
    legacy_body = function_body(legacy, original)
    assert 'test_definition_2022_t' not in legacy and original+'(' not in legacy_body[legacy_body.index('{'):]
    marker = '/* ManalinkEh 2022: 0x2652b90; original functions/tutors.c:1196. */'
    # Find the marker by identity: line references can differ by export pass.
    marker_match = re.search(r'/\* ManalinkEh 2022: 0x2652b90;[^*]*\*/', texts[source])
    assert marker_match
    texts[source] = texts[source].replace(marker_match[0], '/* Legacy 208-byte test compatibility for the to-array selector.\n * Native 2022 entry and private part are recovered below. */')
    assert texts[source].count(before) == 1
    texts[source] = texts[source].replace(before, legacy_body)
    proof['legacy_compatibility'] = dict(name=original, source=source, previous_body_sha256=sha(before.encode()),
        body_sha256=sha(legacy_body.encode()), basis_address='2651660', retained_test_bytes=208,
        native_test_bytes=216, counted_implementation=False,
        scope='Same selection/restoration/sort algorithm over legacy tests and existing legacy selection APIs; zero-count guard retained, no 19-target limit added. No cross-layout cast or temporary conversion.')
    previous_wrapper = dict(accepted['2652b90'])
    accepted['2652b90'] = next(e for e in entries if e['address']=='2652b90')
    proof['migrated_native_wrapper'] = dict(address='2652b90', previous_record=previous_wrapper,
                                          name=names['2652b90'])
    assert globals_proof
    proof['show_deck_expansion_global']=globals_proof
    # Bind only calls whose native target is the distinct compiler part.
    for part, (original, wrappers) in PARTS.items():
        source = metadata[part]['source']
        part_entry = next(e for e in entries if e['address']==part)
        at = texts[source].index('/* ManalinkEh 2022: 0x'+wrappers[0]+';')
        texts[source] = texts[source][:at]+part_entry['prototype']+'\n\n'+texts[source][at:]
        for address in wrappers:
            public_name = metadata[address]['name']
            before = function_body(texts[source], public_name)
            head, tail = before.split('{',1)
            tail, count = re.subn(r'\b'+original+r'\b', names[part], tail)
            assert count == 1
            after = head+'{'+tail
            texts[source] = texts[source].replace(before, after)
            code = runtime.pe.get_data(int(address,16)-0x2000000,int(metadata[address]['high_pc'],16)-int(address,16))
            calls = [dict(address=f'{i.address:x}',bytes=i.bytes.hex(),target=part) for i in decoder.disasm(code,int(address,16))
                     if i.mnemonic in {'call','jmp'} and i.operands[0].type==X86_OP_IMM and i.operands[0].imm==int(part,16)]
            assert len(calls)==1
            repair = dict(name=public_name, part=names[part], part_address=part, machine_bytes=len(code), machine_sha256=sha(code),
                          calls=calls, previous_body_sha256=sha(before.encode()),body_sha256=sha(after.encode()))
            proof['repairs'][address]=repair
            accepted[address]['compiler_part_binding_2022']=repair
            accepted[address]['reconstruction_body_sha256']=repair['body_sha256']
    for source in texts:
        texts[source] += '\n\n#ifndef SHANDALAR\n/* Recovered October 2022 typed consumers and compiler parts. */\n'
        if source=='functions/tutors.c':
            texts[source]+=globals_proof['declaration']+'\n'
        for entry in entries:
            if entry['source'] != source:
                continue
            texts[source] += f"\n/* ManalinkEh 2022: 0x{entry['address']}; original {source}:{entry['line']}. */\n"+bodies[entry['address']]+'\n'
        texts[source] += '#endif\n'
    header = read(root/'manalink.h')
    public = [e for e in entries if not e['static']]
    assert 'int hack_allow_sorcery_if_rules_engine_is_only_effect = 0;' in read(root/'functions/rules_engine.c')
    proof['reused_global']=dict(name='hack_allow_sorcery_if_rules_engine_is_only_effect',
        source='functions/rules_engine.c',declaration='int hack_allow_sorcery_if_rules_engine_is_only_effect = 0;',
        target_address='2795100',native_site=dict(address='23599ef',bytes='833d0051790201'))
    block = '/* October 2022 graveyard/zone and counterspell consumers. */\n#ifndef SHANDALAR\nextern int32_t hack_show_deck_force_expansion;\nextern int hack_allow_sorcery_if_rules_engine_is_only_effect;\n'+'\n'.join(e['prototype'] for e in public)+'\n#endif\n\n'
    at=header.rfind('#ifdef __cplusplus')
    header=header[:at]+block+header[at:]
    scratch=cache/'test-consumers-check'
    scratch.mkdir(exist_ok=True)
    (scratch/'manalink.h').write_bytes(header.encode('latin1'))
    # The private part precedes both new public wrappers in the appended group.
    for source, content in texts.items():
        target = scratch/Path(source).name
        target.write_bytes(content.encode('latin1'))
        result=subprocess.run(['C:/Program Files/LLVM/bin/clang.exe','-target','i686-pc-windows-msvc','-std=gnu99',
            '-fsyntax-only','-ferror-limit=0','-Werror=implicit-function-declaration','-Werror=incompatible-pointer-types',
            '-Werror=return-type','-Werror=uninitialized','-Werror=sometimes-uninitialized','-Werror=format',
            '-I',str(scratch),'-I',str(root),str(target)],capture_output=True,text=True)
        (scratch/(Path(source).stem+'.log')).write_text(result.stderr)
        if result.returncode:
            print(result.stderr)
        result.check_returncode()
    export_path=cache/'decompiled-abi/export.tsv'
    rows=list(csv.DictReader(export_path.open(),delimiter='\t'))
    focused={r['address']:r for r in csv.DictReader((directory/'export.tsv').open(),delimiter='\t')}
    existing={r['address'] for r in rows}
    rows=[focused[r['address']] if r['address'] in CONSUMERS else r for r in rows]
    rows.extend(focused[a] for a in CONSUMERS if a not in existing)
    if args.apply:
        # Preserve raw records already cited by earlier implementations/fragments.
        citations=manifest['source_fragments']+[s for f in accepted.values() for s in f.get('supporting_exports',[])]
        for citation in citations:
            a=citation['address']
            if a not in CONSUMERS or citation.get('raw_export'):
                continue
            old=cache/'decompiled-abi'/(a+'.c')
            if sha(old.read_text().encode())==sha((directory/(a+'.c')).read_text().encode()):
                continue
            support=cache/'decompiled-support'/(a+'-'+citation['raw_sha256']+'.c')
            support.parent.mkdir(exist_ok=True)
            shutil.copyfile(old,support)
            citation['raw_export']=support.relative_to(cache).as_posix()
        for source,content in texts.items():
            (root/source).write_bytes(content.replace('\n','\r\n').encode('latin1'))
        (root/'manalink.h').write_bytes(header.replace('\n','\r\n').encode('latin1'))
        new=[e for e in entries if e['address']!='2652b90']
        manifest['accepted_functions']=list(accepted.values())+new
        manifest['accepted']+=len(new)
        manifest['helper_functions']+=len(new)
        manifest['private_functions']+=5
        manifest['private_helpers']+=5
        manifest['shared_helper_interfaces']+=len([e for e in new if not e['static']])
        for entry in new:
            source=entry['source'];manifest['source_files'][source]=manifest['source_files'].get(source,0)+1
        manifest['raw_export_entries']=len(rows)
        manifest['exported_functions']=len({r['name'] for r in rows})
        manifest['source_globals'].append(dict(source='functions/tutors.c',name='hack_show_deck_force_expansion',
            declaration=globals_proof['declaration'],target_address='2774164',zero_initialized=True,linkage='external'))
        manifest_path.write_text(json.dumps(manifest,indent=2))
        path=root/'docs/recovery-evidence/interface-recovery.json'
        interfaces=json.loads(path.read_text());interfaces['test_consumers_2022']=proof;path.write_text(json.dumps(interfaces,indent=2))
        with export_path.open('w',newline='') as stream:
            writer=csv.DictWriter(stream,fieldnames=rows[0],delimiter='\t');writer.writeheader();writer.writerows(rows)
        for a in CONSUMERS:
            shutil.copyfile(directory/(a+'.c'),export_path.parent/(a+'.c'))
    print(json.dumps(dict(applied=args.apply,new_helpers=18,native_to_array_wrapper_migrated=1,
                          repaired_existing_wrappers=5,legacy_to_array_compatibility=1)))


if __name__=='__main__':
    main()
