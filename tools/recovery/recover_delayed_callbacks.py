"""Recover missing delayed token callbacks and native life/ability API consumers."""
import argparse
import csv
import hashlib
import json
import re
import shutil
import subprocess
from pathlib import Path
from capstone import Cs, CS_ARCH_X86, CS_MODE_32
from capstone.x86_const import X86_OP_IMM, X86_OP_MEM
from elftools.dwarf.descriptions import describe_DWARF_expr
from inspect_release import Image, value, inherited_value
from export_abi import type_of, type_name
from promote_recovery import clean, strip_comments, UNRESOLVED
from recover_test_consumers import read, rename, format_c
from recover_literals import recover_strings
from audit_source import function_body

ADDRESSES = '2377110 212c130 218f5b0 2335b80 21b8dc0 24bc200'.split()
PRIVATE = set(ADDRESSES[:4])
VERSIONS = {'last_chance_legacy':'last_chance_legacy_2022', 'effect_nettling_imp':'effect_nettling_imp_2022'}
EXPOSURE = '23eb810'
HEADER_LABEL = '/* October 2022 delayed-effect callbacks and retained cantrip helper. */'
CONTROL = {'2377110': ['237714c','2377190'], '212c130': ['212c16c','212c1b0'],
           '218f5b0': ['218f5ec','218f630'], '2335b80': ['2335bbc','2335c00'],
           '24bc200': ['24bc2bc']}


def sha(data):
    return hashlib.sha256(data).hexdigest()


def audit_component(component, manifest, sources, machine_bytes, root):
    def require(ok, message):
        if not ok:
            raise ValueError(message)
    entries = {f['address']:f for f in manifest['accepted_functions'] if f.get('delayed_callback_evidence')}
    require(set(entries) == set(ADDRESSES) == set(component['functions']), 'Delayed callback membership mismatch')
    require({a for a,f in entries.items() if f['static']} == PRIVATE, 'Delayed callback private/public linkage mismatch')
    for address, entry in entries.items():
        detail = component['functions'][address]
        require(entry['delayed_callback_evidence'] == detail, 'Delayed callback function evidence mismatch')
        params = component['parameters'][address]
        require([p['name'] for p in params] == ['player','card','event'] and
                [p['location'] for p in params] == ['(DW_OP_fbreg: 0)','(DW_OP_fbreg: 4)','(DW_OP_fbreg: 8)'],
                'Delayed callback physical parameters mismatch')
        for change in detail['changes']:
            require(bool(change['machine_sites']), 'Delayed callback operand proof missing')
        if address in PRIVATE:
            require([c['replacement'] for c in detail['changes']] == ['TRIGGER_EOT','STATE_OUBLIETTED'], 'Delayed token event/state reconstruction mismatch')
            body = function_body(sources[entry['source']],entry['name'])
            require('targets[1].player' in body and 'reason_for_trigger_controller' in body and
                    'EVENT_RESOLVE_TRIGGER' in body, 'Delayed token countdown/controller guard missing')
            expected = [4,4] if address == '218f5b0' else [3,4]
            require([s['value'] for s in detail['kill_argument_sites']] == expected, 'Delayed token native kill modes mismatch')
            for site in detail['kill_argument_sites']:
                data = bytes.fromhex(site['bytes'])
                require(machine_bytes(int(site['address'],16),len(data)) == data, 'Delayed token kill-mode instruction mismatch')
            if address == '212c130':
                require('KILL_SACRIFICE' in body, 'Destroy-named callback must preserve native sacrifice mode')
    require(component['enum_values']['TRIGGER_EOT'] == 205 and component['enum_values']['STATE_OUBLIETTED'] == 0x800000 and
            component['enum_values']['STATE_SUMMON_SICK']|component['enum_values']['STATE_ATTACKED'] == 0x20040,
            'Delayed callback native enum mismatch')
    exposure = component['cantrip_exposure']
    require(exposure['storage'] == 2 and exposure['address'] == EXPOSURE and exposure['name'] == 'cantrip_legacy',
            'Cantrip public symbol mismatch')
    require(sha(machine_bytes(int(EXPOSURE,16),exposure['machine_bytes'])) == exposure['machine_sha256'], 'Cantrip native hash mismatch')
    require(sha(function_body(sources[exposure['source']].replace('\r\n','\n'),exposure['name']).encode()) == exposure['body_sha256'],
            'Retained cantrip callback changed')
    require(sha(read(root/'.recovery/decompiled-abi'/f'{EXPOSURE}.c').encode()) == exposure['raw_sha256'], 'Cantrip raw export mismatch')
    require(exposure['new_signature']+';' in read(root/'manalink.h') and exposure['new_signature'] in sources[exposure['source']] and
            exposure['old_signature'] not in sources[exposure['source']], 'Cantrip exposure declarations mismatch')


def main():
    parser = argparse.ArgumentParser(__doc__); parser.add_argument('--apply',action='store_true'); args = parser.parse_args()
    root = Path(__file__).resolve().parents[2]; cache = root/'.recovery'; directory = cache/'decompiled-delayed-effects'
    mp = root/'docs/recovery-evidence/recovered-functions.json'; ip = root/'docs/recovery-evidence/interface-recovery.json'
    manifest = json.loads(mp.read_text()); interfaces = json.loads(ip.read_text())
    assert 'delayed_callbacks_2022' not in interfaces
    assert not set(ADDRESSES+[EXPOSURE]) & {f['address'] for f in manifest['accepted_functions']}
    meta = {r['low_pc'][2:]:r for r in csv.DictReader((cache/'2022-functions.csv').open()) if r['low_pc'] and r['high_pc']}
    names = {a:VERSIONS.get(meta[a]['name'],meta[a]['name']) for a in ADDRESSES}
    debug_data = Path('G:/Manalink/ManalinkEh_10_22_Hotfix.dbg').read_bytes(); debug = Image(debug_data); dwarf = debug.dwarf()
    proof = dict(debug_sha256=sha(debug_data), parameters={}, enum_values={}, symbols={}, functions={},
        scope='Four missing private delayed-token callbacks and two public callbacks using native life/ability APIs. Preserve delay counter placement, native kill modes and controller/state guards. Expose the unchanged existing cantrip callback without counting its body again.',
        integration_pending='Earlier manual consumers, native linking and gameplay verification remain pending.')
    wanted = {'TRIGGER_EOT','STATE_OUBLIETTED','STATE_SUMMON_SICK','STATE_ATTACKED','EVENT_TRIGGER','EVENT_RESOLVE_TRIGGER',
              'EVENT_CLEANUP','EVENT_SHOULD_AI_PLAY','KILL_SACRIFICE','KILL_DESTROY','KILL_EXILE'}
    for cu in dwarf.iter_CUs():
        if value(cu.get_top_DIE(),'DW_AT_name') not in {meta[a]['source'] for a in ADDRESSES+[EXPOSURE]}:
            continue
        for die in cu.get_top_DIE().iter_children():
            if die.tag == 'DW_TAG_enumeration_type':
                for child in die.iter_children():
                    name = value(child,'DW_AT_name')
                    if name in wanted:
                        number = value(child,'DW_AT_const_value')
                        assert name not in proof['enum_values'] or proof['enum_values'][name] == number
                        proof['enum_values'][name] = number
            pc = value(die,'DW_AT_low_pc'); address = f'{pc:x}' if pc else ''
            if address in ADDRESSES:
                proof['parameters'][address] = [dict(name=inherited_value(p,'DW_AT_name'),type=type_name(type_of(p)),
                    location=describe_DWARF_expr(p.attributes['DW_AT_location'].value,dwarf.structs))
                    for p in die.iter_children() if p.tag == 'DW_TAG_formal_parameter']
    assert set(proof['enum_values']) == wanted
    runtime = Image((cache/'2022-ManalinkEh.dll').read_bytes()); decoder = Cs(CS_ARCH_X86,CS_MODE_32); decoder.detail = True
    targets = {int(f['address'],16):f['name'] for f in manifest['accepted_functions']}
    targets.update({int(a,16):n for a,n in names.items()})
    overlap = [line.split('\t')[1] for line in (cache/'absolute-interfaces.tsv').read_text().splitlines() if line.split('\t')[2]=='global']
    sources, entries = {}, []
    for address in ADDRESSES+[EXPOSURE]:
        symbols = [dict(name=n,storage=s) for n,off,sec,k,s in debug.functions()
            if debug.pe.OPTIONAL_HEADER.ImageBase+debug.pe.sections[sec-1].VirtualAddress+off == int(address,16)]
        assert len(symbols) == 1 and symbols[0]['storage'] == (3 if address in PRIVATE else 2)
        proof['symbols'][address] = symbols[0]
        row = meta[address]; source = row['source']; raw = (directory/f'{address}.c').read_text()
        native = runtime.pe.get_data(int(address,16)-0x2000000,int(row['high_pc'],16)-int(address,16))
        instructions = list(decoder.disasm(native,int(address,16))); by_site = {f'{i.address:x}':i for i in instructions}
        sources.setdefault(source,read(root/source))
        if address == EXPOSURE:
            old_signature = 'static int cantrip_legacy(int player, int card, event_t event)'
            new_signature = old_signature.removeprefix('static ')
            assert sources[source].count(old_signature) == 1
            proof['cantrip_exposure'] = dict(address=address,name=row['name'],source=source,storage=2,old_signature=old_signature,new_signature=new_signature,
                body_sha256=sha(function_body(sources[source],row['name']).encode()),raw_sha256=sha(raw.encode()),machine_bytes=len(native),machine_sha256=sha(native))
            sources[source] = sources[source].replace(old_signature,new_signature,1)
            continue
        name = names[address]; text = strip_comments(raw); changes = []
        replacements = [('0xcd','TRIGGER_EOT'),('0x800000','STATE_OUBLIETTED')] if address in PRIVATE else []
        if address == '24bc200':
            replacements = [('0x20040','(STATE_SUMMON_SICK | STATE_ATTACKED)')]
        for (original,replacement),site in zip(replacements,CONTROL.get(address,[])):
            assert text.count(original) == 1
            ins = by_site[site]
            assert any(o.type == X86_OP_IMM and o.imm == int(original,16) for o in ins.operands)
            changes.append(dict(original=original,replacement=replacement,occurrences=1,kind='verified delayed callback event/state operand',
                                machine_sites=[dict(address=site,bytes=ins.bytes.hex())]))
            text = text.replace(original,replacement)
        text,_ = rename(text,VERSIONS); text,literals = recover_strings(text,runtime,{})
        body = clean(text,overlap,use_keyword_accessors_2022=True,use_extension_flags_2022=True,use_ability_api_2022=True,
                     use_pump_api_2022=True,use_life_api_2022=True,use_life_loss_api_2022=True)
        if address in PRIVATE:
            body = 'static '+body
        body = format_c(body)
        assert not UNRESOLVED.search(body) and not re.search(r'\._\d|stack0x',body),address
        calls = []
        for ins in instructions:
            if ins.mnemonic in {'call','jmp'} and ins.operands[0].type == X86_OP_IMM and ins.operands[0].imm in targets:
                target = targets[ins.operands[0].imm]
                assert re.search(r'\b'+target+r'\s*\(',body),(address,target)
                calls.append(dict(address=f'{ins.address:x}',bytes=ins.bytes.hex(),target=f'{ins.operands[0].imm:x}',name=target))
        kill_sites = [dict(address=f'{ins.address:x}',bytes=ins.bytes.hex(),value=ins.operands[1].imm) for ins in instructions
            if ins.mnemonic=='mov' and len(ins.operands)==2 and ins.operands[0].type==X86_OP_MEM and
            ins.reg_name(ins.operands[0].mem.base)=='esp' and ins.operands[0].mem.disp==8 and ins.operands[0].size==4 and
            ins.operands[1].type==X86_OP_IMM and ins.operands[1].imm in {3,4}]
        detail = dict(machine_bytes=len(native),machine_sha256=sha(native),changes=changes,canonical_calls=calls,kill_argument_sites=kill_sites)
        proof['functions'][address] = detail
        entries.append(dict(address=address,name=name,source=source,line=row['line'],static=address in PRIVATE,
            prototype=body[:body.index('{')].strip()+';',raw_sha256=sha(raw.encode()),recovered_literals=literals,delayed_callback_evidence=detail,
            reconstruction=proof['scope'],reconstruction_body_sha256=sha(function_body(body,name).encode())))
        sources[source] += f'\n\n#ifndef SHANDALAR\n/* ManalinkEh 2022: 0x{address}; original {source}:{row["line"]}. */\n'+body+'\n#endif\n'
    header = read(root/'manalink.h'); at = header.rfind('#ifdef __cplusplus')
    header = header[:at]+HEADER_LABEL+'\n#ifndef SHANDALAR\n'+'\n'.join(e['prototype'] for e in entries if not e['static'])+'\n'+proof['cantrip_exposure']['new_signature']+';\n#endif\n\n'+header[at:]
    scratch = cache/'delayed-callbacks-check'; scratch.mkdir(exist_ok=True); (scratch/'manalink.h').write_bytes(header.encode('latin1'))
    for source,text in sources.items():
        path = scratch/Path(source).name; path.write_bytes(text.encode('latin1'))
        result = subprocess.run(['C:/Program Files/LLVM/bin/clang.exe','-target','i686-pc-windows-msvc','-std=gnu99','-fsyntax-only','-ferror-limit=0',
            '-Werror=implicit-function-declaration','-Werror=incompatible-pointer-types','-Werror=return-type','-Werror=uninitialized',
            '-Werror=sometimes-uninitialized','-Werror=format','-I',str(scratch),'-I',str(root),str(path)],capture_output=True,text=True)
        path.with_suffix('.log').write_text(result.stderr)
        if result.returncode:
            print(result.stderr[-6000:])
        result.check_returncode()
    # Check the exposure against its focused export before canonical cache merging.
    temporary_raw = proof['cantrip_exposure']['raw_sha256']
    assert sha(read(directory/f'{EXPOSURE}.c').encode()) == temporary_raw
    if args.apply:
        citations = manifest['source_fragments']+manifest['accepted_functions']+[s for f in manifest['accepted_functions'] for s in f.get('supporting_exports',[])]
        assert not any(c['address'] in ADDRESSES+[EXPOSURE] for c in citations)
        for source,text in sources.items():
            (root/source).write_bytes(text.replace('\n','\r\n').encode('latin1'))
        (root/'manalink.h').write_bytes(header.replace('\n','\r\n').encode('latin1'))
        manifest['accepted_functions'] += entries; manifest['accepted'] += 6; manifest['helper_functions'] += 6
        manifest['private_functions'] += 4; manifest['private_helpers'] += 4; manifest['shared_helper_interfaces'] += 2
        for entry in entries:
            manifest['source_files'][entry['source']] = manifest['source_files'].get(entry['source'],0)+1
        mp.write_text(json.dumps(manifest,indent=2)); interfaces['delayed_callbacks_2022'] = proof; ip.write_text(json.dumps(interfaces,indent=2))
        for address in ADDRESSES+[EXPOSURE]:
            shutil.copyfile(directory/f'{address}.c',cache/'decompiled-abi'/f'{address}.c')
    (cache/'delayed-callbacks-plan.json').write_text(json.dumps(proof,indent=2))
    print(json.dumps(dict(applied=args.apply,new_helpers=6,private_helpers=4,public_helpers=2,exposed_existing_helpers=1)))


if __name__ == '__main__':
    main()
