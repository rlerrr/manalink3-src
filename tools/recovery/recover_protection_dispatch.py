"""Recover the 2022 protection mode API and its initial-result event dispatchers."""
import argparse
import csv
import hashlib
import json
import re
import shutil
import subprocess
from pathlib import Path

from capstone import Cs, CS_ARCH_X86, CS_MODE_32
from inspect_release import Image, value, inherited_value
from export_abi import type_of, type_name
from promote_recovery import clean, UNRESOLVED, strip_comments
from audit_source import function_body


def main():
    parser = argparse.ArgumentParser(__doc__); parser.add_argument('--apply', action='store_true'); args = parser.parse_args()
    root = Path(__file__).resolve().parents[2]; cache = root / '.recovery'
    data = Path('G:/Manalink/ManalinkEh_10_22_Hotfix.dbg').read_bytes(); debug = Image(data)
    runtime = Image((cache / '2022-ManalinkEh.dll').read_bytes())
    modes = {'IPF_DAMAGE':1,'IPF_ATTACH_AURA':2,'IPF_ATTACH_EQUIPMENT':3,'IPF_BLOCK':4,'IPF_TARGET':5,'IPF_QUERY':6}
    enum_proof = None; local_proof = None
    for cu in debug.dwarf().iter_CUs():
        if value(cu.get_top_DIE(),'DW_AT_name') != 'functions/targets.c': continue
        def walk(die):
            nonlocal local_proof
            for child in die.iter_children():
                if child.tag == 'DW_TAG_variable' and inherited_value(child,'DW_AT_name') == 'initial_event_result':
                    assert type_name(type_of(child)) == 'int'
                    local_proof = dict(die=hex(child.offset), type='int', function='265d580')
                walk(child)
        for die in cu.get_top_DIE().iter_children():
            if die.tag == 'DW_TAG_typedef' and value(die,'DW_AT_name') == 'is_protected_from_mode_t':
                enum = type_of(die)
                assert value(enum,'DW_AT_byte_size') == 4
                members = {value(c,'DW_AT_name'):value(c,'DW_AT_const_value') for c in enum.iter_children()}
                assert members == modes
                enum_proof = dict(die=hex(enum.offset), bytes=4, members=members)
            if value(die,'DW_AT_low_pc') == 0x265d580: walk(die)
        break
    assert enum_proof and local_proof
    events = (root / 'functions/events.c').read_text()
    for name,address in [('push_affected_card_stack',0x435c80),('pop_affected_card_stack',0x435cd0)]:
        assert re.search(r'#define\s+'+name+r'\s+EXE_FN\(void,\s*0x'+f'{address:x}'+r',\s*void\)',events,re.I)
    assert re.search(r'(?m)^_trigger_condition\s*=\s*0x7a2d78;', (root/'manalink.lds').read_text(),re.I)
    absolute = (cache/'absolute-interfaces.tsv').read_text()
    assert '0x7a2d78\ttrigger_condition\tglobal\ttrigger_t' in absolute
    assert '0x4ef1a4\tplayer_bits\tglobal\tplayer_bits_t[2]' in absolute
    metadata = {r['low_pc'][2:]:r for r in csv.DictReader((cache/'2022-functions.csv').open()) if r['low_pc'] and r['high_pc']}
    addresses = ['26cb5b0','26cbdb0','265d580','265d6c0']
    raw = {a:(cache/('decompiled-protection-wrapper' if a=='265d6c0' else 'decompiled-abi')/(a+'.c')).read_text() for a in addresses}
    proof = dict(debug_sha256=hashlib.sha256(data).hexdigest(), modes=enum_proof, integer_local=local_proof,
        native_bindings={'push_affected_card_stack':'435c80','pop_affected_card_stack':'435cd0','trigger_condition':'7a2d78','player_bits':'4ef1a4'}, functions={},
        scope='Preserve original protection API; recover the six 2022 mode values under distinct names. Initial-result dispatch retains native context push/pop and trigger save/restore.',
        integration_pending='Native dispatch paths and remaining old protection/keyword consumers still require downstream integration.')
    bodies = {}; prototypes = []; decoder = Cs(CS_ARCH_X86,CS_MODE_32)
    for a in addresses:
        start = int(a,16); length = int(metadata[a]['high_pc'],16)-start
        code = runtime.pe.get_data(start-runtime.pe.OPTIONAL_HEADER.ImageBase,length)
        instructions = list(decoder.disasm(code,start))
        sites = []
        for ins in instructions:
            if any(v in ins.op_str for v in ('0x435c80','0x435cd0','0x7a2d78','0x5000000','0x4ef1a7')) or ins.mnemonic=='shl':
                sites.append(dict(address=f'{ins.address:x}', bytes=ins.bytes.hex(), instruction=ins.mnemonic+' '+ins.op_str))
        body = raw[a]
        if a.startswith('26cb'):
            assert body.count('(*(code *)0x435c80)();') == 1 and body.count('(*(code *)0x435cd0)();') == 1
            body = body.replace('(*(code *)0x435c80)();','push_affected_card_stack();').replace('(*(code *)0x435cd0)();','pop_affected_card_stack();')
            body = re.sub(r'\b_trigger_condition\b','trigger_condition',body)
            assert any('0x435c80' in s['instruction'] for s in sites) and any('0x435cd0' in s['instruction'] for s in sites)
        if a=='265d580':
            assert 'undefined *initial_event_result;' in body and body.count('&UNK_05000000') == 1
            assert runtime.pe.get_data(0x265d60a-0x2000000,5).hex() == 'bb00000005'
            assert runtime.pe.get_data(0x265d61b-0x2000000,3).hex() == 'c1e318'
            assert runtime.pe.get_data(0x265d683-0x2000000,3).hex() == 'c1e318'
            assert runtime.pe.get_data(0x265d5c4-0x2000000,8).hex() == 'f604bda7f14e0004'
            body = body.replace('undefined *initial_event_result;','int initial_event_result;')
            body = body.replace('(undefined *)(new_protection_mode << 0x18)','(new_protection_mode << 0x18)')
            body = body.replace('&UNK_05000000','(IPF_TARGET << 24)')
            body = body.replace('*(byte *)(protected_player * 4 + 0x4ef1a7)', '((unsigned int)player_bits[protected_player] >> 24 & 0xff)')
        body = clean(body)
        assert not UNRESOLVED.search(strip_comments(body))
        body = subprocess.run(['C:/Program Files/LLVM/bin/clang-format.exe','-style={BasedOnStyle: LLVM, IndentWidth: 2, BreakBeforeBraces: Allman, ColumnLimit: 100}'],input=body.encode(),capture_output=True,check=True).stdout.decode().replace('\r\n','\n')
        bodies[a] = body; prototypes.append(body[:body.index('{')].strip()+';')
        proof['functions'][a] = dict(machine_sha256=hashlib.sha256(code).hexdigest(), bytes=length, machine_sites=sites)
    enum_block = '\n#ifndef SHANDALAR\n/* October 2022 protection event modes; original modes retained above. */\ntypedef enum\n{\n'+',\n'.join(f'  {n.replace("IPF_","IPF2022_")} = {v}' for n,v in modes.items())+'\n} is_protected_from_mode_2022_t;\n#endif\n\n'
    defs = (root/'defs.h').read_bytes().decode('latin1').replace('\r\n','\n'); at = defs.rfind('#endif')
    assert 'is_protected_from_mode_2022_t;' not in defs
    defs = defs[:at]+enum_block+defs[at:]
    # Check both complete owning units using a private include directory.
    include = cache/'protection-dispatch-include'; include.mkdir(exist_ok=True)
    (include/'defs.h').write_bytes(defs.encode('latin1'))
    header = (root/'manalink.h').read_bytes().decode('latin1').replace('\r\n','\n'); at = header.rfind('#ifdef __cplusplus')
    header = header[:at]+'/* October 2022 protection dispatch interfaces. */\n#ifndef SHANDALAR\n'+'\n'.join(prototypes)+'\n#endif\n\n'+header[at:]
    (include/'manalink.h').write_bytes(header.encode('latin1'))
    candidates = {}
    for a in addresses:
        source = metadata[a]['source']
        candidates.setdefault(source,(root/source).read_bytes().decode('latin1').replace('\r\n','\n'))
        candidates[source] += f"\n\n/* ManalinkEh 2022: 0x{a}; original {source}:{metadata[a]['line']}. */\n"+bodies[a]
    for source,candidate in candidates.items():
        scratch = include/Path(source).name; scratch.write_bytes(candidate.encode('latin1'))
        result = subprocess.run(['C:/Program Files/LLVM/bin/clang.exe','-target','i686-pc-windows-msvc','-std=gnu99','-fsyntax-only','-Werror=implicit-function-declaration','-Werror=incompatible-pointer-types','-Werror=return-type','-Werror=uninitialized','-Werror=sometimes-uninitialized','-Werror=format','-I',str(include),'-I',str(root),str(scratch)],capture_output=True,text=True)
        (cache/('protection-dispatch-'+Path(source).stem+'.log')).write_text(result.stderr); result.check_returncode()
    if args.apply:
        manifest_path = root/'docs/recovery-evidence/recovered-functions.json'; manifest = json.loads(manifest_path.read_text())
        assert not set(addresses) & {f['address'] for f in manifest['accepted_functions']}
        for source,candidate in candidates.items(): (root/source).write_bytes(candidate.replace('\n','\r\n').encode('latin1'))
        (root/'defs.h').write_bytes(defs.replace('\n','\r\n').encode('latin1')); (root/'manalink.h').write_bytes(header.replace('\n','\r\n').encode('latin1'))
        for a,prototype in zip(addresses,prototypes):
            name = metadata[a]['name'] + ('_2022' if a=='265d6c0' else '')
            manifest['accepted_functions'].append(dict(name=name,original_name=metadata[a]['name'],address=a,source=metadata[a]['source'],line=metadata[a]['line'],static=False,prototype=prototype,raw_sha256=hashlib.sha256(raw[a].encode()).hexdigest(),recovered_literals=[],reconstruction=proof['scope'],protection_dispatch_evidence=proof['functions'][a],reconstruction_body_sha256=hashlib.sha256(function_body(bodies[a],name).encode()).hexdigest()))
        exports = cache/'decompiled-abi/export.tsv'; rows = list(csv.DictReader(exports.open(),delimiter='\t'))
        assert '265d6c0' not in {r['address'] for r in rows}
        rows.extend(csv.DictReader((cache/'decompiled-protection-wrapper/export.tsv').open(),delimiter='\t'))
        with exports.open('w',newline='') as stream:
            writer = csv.DictWriter(stream,fieldnames=rows[0],delimiter='\t'); writer.writeheader(); writer.writerows(rows)
        shutil.copyfile(cache/'decompiled-protection-wrapper/265d6c0.c',exports.parent/'265d6c0.c')
        manifest['accepted'] = len(manifest['accepted_functions'])
        manifest_path.write_text(json.dumps(manifest,indent=2))
        path = root/'docs/recovery-evidence/interface-recovery.json'; interfaces = json.loads(path.read_text()); interfaces['protection_dispatch_2022'] = proof; path.write_text(json.dumps(interfaces,indent=2))
    print(json.dumps(dict(applied=args.apply,helpers=len(addresses),enum_members=len(modes))))


if __name__ == '__main__': main()
