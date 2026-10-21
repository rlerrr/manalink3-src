"""Bind recovered native callers to verified scalar-only helper replacements.

Aggregate callers and bodies bound by earlier manual reconstructions are left
for their own complete interface/storage migration. No record casts are added.
"""
import argparse,copy,csv,hashlib,json,re,subprocess
from pathlib import Path
from capstone import Cs,CS_ARCH_X86,CS_MODE_32
from capstone.x86_const import X86_OP_IMM
from inspect_release import Image
from audit_source import function_body
from recover_test_consumers import read,rename

def sha(data):return hashlib.sha256(data).hexdigest()

def main():
    ap=argparse.ArgumentParser(__doc__);ap.add_argument('--apply',action='store_true');ap.add_argument('--family',choices=['aura','ability','pump','activated','attack','entry','level_buddy','phyrexian','death_trigger','action','life','cannot_lose','life_loss'],default='aura');args=ap.parse_args()
    root=Path(__file__).resolve().parents[2];cache=root/'.recovery';mp=root/'docs/recovery-evidence/recovered-functions.json';ip=root/'docs/recovery-evidence/interface-recovery.json'
    component={'aura':'native_scalar_callers_2022','ability':'native_ability_callers_2022','pump':'native_pump_callers_2022','activated':'native_activated_callers_2022','attack':'native_attack_callers_2022','entry':'native_entry_callers_2022','level_buddy':'native_level_buddy_callers_2022','phyrexian':'native_phyrexian_callers_2022','death_trigger':'native_death_trigger_callers_2022','action':'native_action_callers_2022','life':'native_life_callers_2022','cannot_lose':'native_cannot_lose_callers_2022','life_loss':'native_life_loss_callers_2022'}[args.family]
    binding={'aura':'native_scalar_callee_binding_2022','ability':'native_ability_callee_binding_2022','pump':'native_pump_callee_binding_2022','activated':'native_activated_callee_binding_2022','attack':'native_attack_callee_binding_2022','entry':'native_entry_callee_binding_2022','level_buddy':'native_level_buddy_callee_binding_2022','phyrexian':'native_phyrexian_callee_binding_2022','death_trigger':'native_death_trigger_callee_binding_2022','action':'native_action_callee_binding_2022','life':'native_life_callee_binding_2022','cannot_lose':'native_cannot_lose_callee_binding_2022','life_loss':'native_life_loss_callee_binding_2022'}[args.family]
    m=json.loads(mp.read_text());interfaces=json.loads(ip.read_text());assert component not in interfaces
    addresses=(set(interfaces['aura_consumers_2022']['functions'])|set(interfaces['split_card_helpers_2022']['functions'])) if args.family=='aura' else set(interfaces[{'ability':'ability_writers_2022','pump':'pump_core_2022','activated':'activated_core_2022','attack':'attacker_core_2022','entry':'entry_triggers_2022','level_buddy':'level_buddy_2022','phyrexian':'phyrexian_costs_2022','death_trigger':'death_trigger_core_2022','action':'action_core_2022','life':'life_core_2022','cannot_lose':'cannot_lose_core_2022','life_loss':'life_loss_2022'}[args.family]]['functions'])
    def eligible(prototype):
        if args.family=='activated':prototype=re.sub(r'\bcard_instance_t\s*\*','',prototype)
        return not re.search(r'\*|\b(?:target|test)_definition',prototype)
    helpers={f['name'].removesuffix('_2022'):f for f in m['accepted_functions'] if f['address'] in addresses and not f['static'] and f['name'].endswith('_2022') and eligible(f['prototype'])}
    targets={int(f['address'],16):name for name,f in helpers.items()}
    metadata={r['low_pc'][2:]:r for r in csv.DictReader((cache/'2022-functions.csv').open()) if r['low_pc'] and r['high_pc']}
    runtime=Image((cache/'2022-ManalinkEh.dll').read_bytes());decoder=Cs(CS_ARCH_X86,CS_MODE_32);decoder.detail=True
    sources={s:read(root/s) for s in m['source_files']};proof=dict(helpers={n:dict(name=f['name'],address=f['address'],prototype=f['prototype']) for n,f in helpers.items()},functions={},deferred={},
        scope='Only scalar-only helper calls in already recovered native functions, with exact native call/jump destinations. Leave all legacy source functions, aggregate callers and earlier manually reconstructed bodies unchanged.')
    if args.family=='activated':
        proof['common_instance_layout']=dict(bytes=300,ability_loss_offset=256)
        proof['scope']='Native scalar and common 300-byte card-instance pointer permission/cost callees with exact native destinations. Leave legacy source functions, test/target aggregate callers and earlier manual reconstructions unchanged.'
    for f in m['accepted_functions']:
        # Pre-filter before repeatedly parsing large source units.
        raw=(cache/'decompiled-abi'/(f['address']+'.c')).read_text()
        if not any(re.search(r'\b'+n+r'\s*\(',raw.split('{',1)[-1]) for n in helpers):continue
        before=function_body(sources[f['source']],f['name']);calls={n:helpers[n]['name'] for n in helpers if re.search(r'\b'+n+r'\s*\(',before.split('{',1)[1])}
        if not calls:continue
        if f.get('reconstruction_body_sha256') or f.get('stack_storage_reconstruction'):
            proof['deferred'][f['address']]='Earlier manual reconstruction body requires a coordinated component migration.';continue
        native=runtime.pe.get_data(int(f['address'],16)-0x2000000,int(metadata[f['address']]['high_pc'],16)-int(f['address'],16));sites=[]
        for i in decoder.disasm(native,int(f['address'],16)):
            if i.mnemonic not in {'call','jmp'} or i.operands[0].type!=X86_OP_IMM or targets.get(i.operands[0].imm) not in calls:continue
            old=targets[i.operands[0].imm];sites.append(dict(address=f'{i.address:x}',bytes=i.bytes.hex(),target=f'{i.operands[0].imm:x}',name=calls[old],original=old))
        if {s['original'] for s in sites}!=set(calls):
            proof['deferred'][f['address']]='A scalar call has no exact direct native destination in this export.';continue
        after,count=rename(before,calls);assert count>0
        evidence=dict(source=f['source'],name=f['name'],previous_record=copy.deepcopy(f),previous_body_sha256=sha(before.encode()),body_sha256=sha(after.encode()),
            machine_bytes=len(native),machine_sha256=sha(native),changes=[],canonical_calls=sites,calls=calls)
        proof['functions'][f['address']]=evidence
        f[binding]={k:v for k,v in evidence.items() if k!='previous_record'}
        f['reconstruction']='Native scalar-only helper callee migration; earlier raw and interface evidence retained.';f['reconstruction_body_sha256']=evidence['body_sha256']
        if args.family=='aura' and f.get('target_interface_2022'):
            f['target_interface_2022']['calls'].update(calls)
        if args.family=='ability':
            interface=f.setdefault('ability_interface_2022',dict(type='pump_ability_2022_t',definition_present=bool(re.search(r'\bpump_ability_2022_t\b',after)),calls={}))
            interface['calls'].update(calls)
        if args.family=='pump':
            f.setdefault('pump_interface_2022',dict(calls={}))['calls'].update(calls)
        if args.family=='activated':
            f.setdefault('activated_interface_2022',dict(calls={}))['calls'].update(calls)
        if args.family=='attack':
            f.setdefault('attack_interface_2022',dict(calls={}))['calls'].update(calls)
        if args.family=='entry':
            f.setdefault('entry_interface_2022',dict(calls={}))['calls'].update(calls)
        if args.family=='level_buddy':
            f.setdefault('level_buddy_interface_2022',dict(calls={}))['calls'].update(calls)
        if args.family=='life_loss':
            f.setdefault('life_loss_interface_2022',dict(calls={}))['calls'].update(calls)
        if args.family=='cannot_lose':
            f.setdefault('cannot_lose_interface_2022',dict(calls={}))['calls'].update(calls)
        if args.family=='life':
            f.setdefault('life_interface_2022',dict(translations={}))['translations'].update(calls)
        if args.family=='action':
            f.setdefault('action_interface_2022',dict(calls={}))['calls'].update(calls)
        if args.family=='death_trigger':
            f.setdefault('death_trigger_interface_2022',dict(calls={}))['calls'].update(calls)
        if args.family=='phyrexian':
            f.setdefault('phyrexian_interface_2022',dict(calls={}))['calls'].update(calls)
        assert sources[f['source']].count(before)==1,(f['address'],'Caller body contains comments or has ambiguous source text; preserve it for a scoped migration.')
        sources[f['source']]=sources[f['source']].replace(before,after)
    changed={d['source'] for d in proof['functions'].values()};scratch=cache/('scalar-'+args.family+'-callers-check');scratch.mkdir(exist_ok=True)
    for source in changed:
        target=scratch/Path(source).name;target.write_bytes(sources[source].encode('latin1'))
        # Check complete units with the same policy as check_source.py. These
        # identifier-only edits do not change format arguments/local storage;
        # pre-existing warnings elsewhere in a unit remain visible in its log.
        result=subprocess.run(['C:/Program Files/LLVM/bin/clang.exe','-target','i686-pc-windows-msvc','-std=gnu99','-fsyntax-only','-Werror=implicit-function-declaration','-Werror=implicit-int','-Werror=incompatible-pointer-types','-Werror=return-type','-I',str(root),str(target)],capture_output=True,text=True)
        (scratch/(Path(source).stem+'.log')).write_text(result.stderr)
        if result.returncode:print(result.stderr)
        result.check_returncode()
    if args.apply:
        for source in changed:(root/source).write_bytes(sources[source].replace('\n','\r\n').encode('latin1'))
        mp.write_text(json.dumps(m,indent=2));interfaces[component]=proof;ip.write_text(json.dumps(interfaces,indent=2))
    print(json.dumps(dict(applied=args.apply,changed_native_callers=len(proof['functions']),source_files=len(changed),deferred=len(proof['deferred']),new_implementations=0)))

if __name__=='__main__':main()
