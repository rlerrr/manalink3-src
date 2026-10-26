"""Recover the October 2022 reanimation family without changing legacy APIs."""
import argparse,csv,json,re,shutil
from pathlib import Path
from capstone import Cs,CS_ARCH_X86,CS_MODE_32
from capstone.x86_const import X86_OP_IMM
from inspect_release import Image,value,inherited_value
from export_abi import type_of,type_name
from promote_recovery import REANIMATION_API_NAMES,REANIMATION_ENUM_VALUES,REANIMATION_DOMAIN_NAMES,strip_comments,UNRESOLVED
from recover_combat_mask_cards import cleaned
from recover_phase_prompt_cards import debug_evidence
from recover_test_consumers import read,rename
from recover_spell_copy import compile_sources
from recover_literals import recover_strings
from recover_temporary_exile import save_json,sha
from audit_source import function_body,target_validation_input

ADDRESSES='269ba70 269c810 2695730 269b760 269b8f0 269b970 269b9f0 269c830 2683db0 26a2d40 269a810 2683c00'.split()
PRIVATE={'269ba70','2695730','2683db0'}
NAMES={**REANIMATION_API_NAMES,**{n:n+'_2022' for n in ('new_reanimate_all_impl','reanimate_permanent_impl','haste_and_exile_your_upkeep')}}
SOURCE='functions/functions.c'
HEADER_LABEL='/* October 2022 reanimation APIs and four-argument before-ETB callback. */'
DEFS_LABEL='/* October 2022 signed reanimation action domain. */'
PROLOGUE={'269ba70':'5589e557565350b86c200000e84f07060029c4','269ba86':'8b0404'}


def scoped_changes(text):
    """Flatten the two proven 4000-byte planes; retain every native loop/index."""
    changes=[]
    def replace(old,new,kind):
        nonlocal text
        count=text.count(old);assert count,(old,kind)
        changes.append(dict(original=old,replacement=new,occurrences=count,kind=kind,
            machine_sites=[dict(address=a,bytes=b) for a,b in PROLOGUE.items()]))
        text=text.replace(old,new)
    for old in ['auras[1][0x1f2] = 0x269ba81;','auras[1][499] = player;']:
        replace(old,'','compiler prologue save/probe temporary before array lifetimes')
    for name in ['to_reanimate','auras']:
        replace(f'int {name} [2] [500];',f'int RECOVERY_FLAT_{name}[1000];','4000-byte array plane declaration')
        pattern=r'\b'+name+r'\[([^\[\]\n]+)\](?:\[([^\[\]\n]+)\])?'
        matches=list(re.finditer(pattern,text))
        # Two-index expressions first, then row pointers; each unique spelling once.
        expressions={m[0]:(f'{name}[({m[1]}) * 500 + ({m[2]})]' if m[2] is not None else f'({name} + ({m[1]}) * 500)') for m in matches}
        for old,new in sorted(expressions.items(),key=lambda x:-len(x[0])):
            replace(old,new,'array plane row stride 500 ints')
        replace('RECOVERY_FLAT_'+name,name,'finish flattened array declaration')
    return text,changes


def type_evidence(debug):
    result={}
    for cu in debug.dwarf().iter_CUs():
        if value(cu.get_top_DIE(),'DW_AT_name')!=SOURCE:continue
        for d in cu.get_top_DIE().iter_children():
            if d.tag=='DW_TAG_typedef' and value(d,'DW_AT_name')=='reanimate_mode_t':
                t=type_of(d);result['enum_bytes']=value(t,'DW_AT_byte_size')
                result['enum_values']={value(x,'DW_AT_name'):value(x,'DW_AT_const_value') for x in t.iter_children() if x.tag=='DW_TAG_enumerator'}
            if value(d,'DW_AT_low_pc')==0x2695730:
                for p in d.iter_children():
                    name=inherited_value(p,'DW_AT_name')
                    if p.tag!='DW_TAG_formal_parameter' or name not in {'before_etb','effect_fn'}:continue
                    t=type_of(p)
                    while t is not None and t.tag!='DW_TAG_subroutine_type':t=type_of(t)
                    assert t is not None
                    result[name]=dict(result=type_name(type_of(t)),parameters=[type_name(type_of(c)) for c in t.iter_children() if c.tag=='DW_TAG_formal_parameter'])
    assert result['enum_bytes']==4 and result['enum_values']==REANIMATION_ENUM_VALUES
    assert result['before_etb']==dict(result='void',parameters=['int']*4)
    assert result['effect_fn']==dict(result='int',parameters=['int','int','event_t'])
    return result


def audit_component(component,manifest,sources,machine_bytes,root):
    assert component['debug_sha256']=='7cdd6bfea74652b900250faae275c754ef1d32966b9f314a729e46b980337b85'
    assert component['runtime_sha256']=='52793f60124a3b83d8b2728d145e918101a395b4e8d1c0af69cddcfc87ef10e5'
    entries={f['address']:f for f in manifest['accepted_functions'] if f.get('reanimation_evidence')}
    assert set(entries)==set(ADDRESSES)==set(component['functions'])
    assert {a for a,f in entries.items() if f['static']}==PRIVATE
    assert component['types']['enum_values']==REANIMATION_ENUM_VALUES and component['types']['enum_bytes']==4
    assert component['types']['before_etb']==dict(result='void',parameters=['int']*4)
    defs=read(root/'defs.h') if not component.get('planning') else None
    if defs is not None:
        if (root/'docs/recovery-evidence/canonical-reanimation.json').exists():
            from canonicalize_reanimation import verify as verify_reanimation
            assert verify_reanimation(root)['values']==REANIMATION_ENUM_VALUES
        else:
            for name,number in REANIMATION_ENUM_VALUES.items():
                assert re.search(r'\b'+REANIMATION_DOMAIN_NAMES[name]+r'\s*=\s*'+str(number)+r'\b',defs),name
        assert 'typedef void (*ReanimationBeforeEtb_2022)(int, int, int, int);' in read(root/'manalink.h')
    params=component['parameters']['269ba70']
    assert [p['name'] for p in params[:5]]==['player','card','targ_player','this_test','action']
    assert [p['location'][0]['expression'] for p in params[:3]]==['(DW_OP_reg0 (r0))','(DW_OP_reg2 (r2))','(DW_OP_reg1 (r1))']
    entry_location=lambda p:p['location'][0]['expression'] if isinstance(p['location'],list) else p['location']
    assert [entry_location(p) for p in params[3:5]]==['(DW_OP_fbreg: 0)','(DW_OP_fbreg: 4)']
    assert len(params)==8 and all('DW_OP_GNU_parameter_ref' in str(p['location']) for p in params[5:])
    single=component['parameters']['2695730'];assert len(single)==8
    assert [entry_location(p) for p in single[3:]]==[f'(DW_OP_fbreg: {i})' for i in [0,4,8,12,16]]
    for name,offset,typ in [('to_reanimate',-8020,'int[2][500]'),('auras',-4020,'int[2][500]'),('trc',-8252,'int[2]'),('ac',-8244,'int[2]'),('this_test2',-8236,'test_definition_t')]:
        assert any(v['name']==name and v['type']==typ and v['location']==f'(DW_OP_fbreg: {offset})' for v in component['locals']['269ba70']),(name,offset)
    for a,data in PROLOGUE.items():assert machine_bytes(int(a,16),len(bytes.fromhex(data))).hex()==data
    assert machine_bytes(0x2677460,3).hex()=='31c0c3'
    for name,digest in component['retained_bodies'].items():
        from unify_scalar_helpers import retained_source_body
        assert sha(retained_source_body(root,SOURCE,name).encode())==digest,name
    decoder=Cs(CS_ARCH_X86,CS_MODE_32);decoder.detail=True
    for a,entry in entries.items():
        detail=component['functions'][a];body=function_body(sources[SOURCE].replace('\r\n','\n'),entry['name'])
        assert entry['reanimation_evidence']==detail and sha(body.encode())==entry['reconstruction_body_sha256']
        assert entry['name']==NAMES[component['native_names'][a]]
        native=machine_bytes(int(a,16),detail['machine_bytes']);assert sha(native)==detail['machine_sha256']
        raw=read(root/'.recovery'/('decompiled-reanimation' if component.get('planning') else 'decompiled-abi')/f'{a}.c')
        assert sha(raw.encode())==entry['raw_sha256'];raw=strip_comments(raw)
        if a=='269ba70':
            expected,changes=scoped_changes(raw);assert changes==detail['changes']
            assert target_validation_input(raw,detail,a,machine_bytes)==expected
            assert all(f'int {n}[1000];' in body for n in ['auras','to_reanimate'])
            assert re.search(r'new_reanimate_all_impl_2022\([^)]*\)',body)[0].count(',')==4
        else:assert detail['changes']==[]
        instructions={f'{i.address:x}':i for i in decoder.disasm(native,int(a,16))}
        for site in detail['canonical_calls']+detail['callback_references']:
            ins=instructions[site['address']];assert ins.bytes.hex()==site['bytes']
            assert any(o.type==X86_OP_IMM and o.imm==int(site['target'],16) for o in ins.operands)
            assert re.search(r'\b'+site['name']+r'\b',body),(a,site)
        if a not in PRIVATE:assert entry['prototype'] in read(root/'manalink.h') or component.get('planning')


def main():
    parser=argparse.ArgumentParser(__doc__);parser.add_argument('--apply',action='store_true');args=parser.parse_args()
    root=Path(__file__).resolve().parents[2];cache=root/'.recovery';directory=cache/'decompiled-reanimation'
    mp=root/'docs/recovery-evidence/recovered-functions.json';ip=root/'docs/recovery-evidence/interface-recovery.json'
    m=json.loads(mp.read_text());interfaces=json.loads(ip.read_text());assert 'reanimation_2022' not in interfaces
    assert not set(ADDRESSES)&{f['address'] for f in m['accepted_functions']}
    meta={r['low_pc'][2:]:r for r in csv.DictReader((cache/'2022-functions.csv').open()) if r['low_pc'] and r['high_pc']}
    data=Path('G:/Manalink/ManalinkEh_10_22_Hotfix.dbg').read_bytes();debug=Image(data)
    proof=dict(debug_sha256=sha(data),**debug_evidence(debug,meta,ADDRESSES),types=type_evidence(debug),functions={},native_names={a:meta[a]['name'] for a in ADDRESSES},
        retained_bodies={n:sha(function_body(read(root/SOURCE),n).encode()) for n in [*NAMES,'empty']},
        scope='Native single/mass reanimation, upkeep and unearth helpers. Preserve native loop/index/owner quirks and returned-index masks. The mass worker physically receives five arguments; three GNU_parameter_ref parameters are source-only. Two prologue temporary saves precede the array lifetimes; flatten each 4000-byte array at a 500-int row stride. Legacy functions remain unchanged.')
    runtime_data=(cache/'2022-ManalinkEh.dll').read_bytes();runtime=Image(runtime_data);proof['runtime_sha256']=sha(runtime_data)
    decoder=Cs(CS_ARCH_X86,CS_MODE_32);decoder.detail=True
    targets={int(f['address'],16):f['name'] for f in m['accepted_functions']};targets.update({int(a,16):NAMES[meta[a]['name']] for a in ADDRESSES})
    overlap=[line.split('\t')[1] for line in (cache/'absolute-interfaces.tsv').read_text().splitlines() if line.split('\t')[2]=='global']
    entries=[];bodies={}
    for a in ADDRESSES:
        raw=read(directory/f'{a}.c');text=strip_comments(raw);changes=[]
        if a=='269ba70':text,changes=scoped_changes(text)
        text,_=rename(text,{**NAMES,**REANIMATION_DOMAIN_NAMES})
        text,literals=recover_strings(text,runtime,{})
        text=re.sub(r'\b_func_void_int_int_int_int\s*\*','ReanimationBeforeEtb_2022 ',text)
        body=cleaned(text,overlap)
        assert not UNRESOLVED.search(body) and not re.search(r'stack0x|\._\d|_conflict',body),a
        storage=[s for n,v,sec,k,s in debug.functions() if sec>0 and debug.pe.OPTIONAL_HEADER.ImageBase+debug.pe.sections[sec-1].VirtualAddress+v==int(a,16)]
        assert storage==([3] if a in PRIVATE else [2]),(a,storage)
        if a in PRIVATE:body='static '+body
        native=runtime.pe.get_data(int(a,16)-0x2000000,int(meta[a]['high_pc'],16)-int(a,16));calls=[];refs=[]
        for ins in decoder.disasm(native,int(a,16)):
            for o in ins.operands:
                if o.type!=X86_OP_IMM or o.imm not in targets:continue
                name=targets[o.imm];assert re.search(r'\b'+name+r'\b',body),(a,name,hex(o.imm))
                site=dict(address=f'{ins.address:x}',bytes=ins.bytes.hex(),target=f'{o.imm:x}',name=name)
                (calls if ins.mnemonic in {'call','jmp'} else refs).append(site)
        detail=dict(machine_bytes=len(native),machine_sha256=sha(native),changes=changes,canonical_calls=calls,callback_references=refs)
        proof['functions'][a]=detail;bodies[a]=body
        entries.append(dict(address=a,name=NAMES[meta[a]['name']],source=SOURCE,line=meta[a]['line'],static=a in PRIVATE,
            prototype=body[:body.index('{')].strip()+';',raw_sha256=sha(raw.encode()),recovered_literals=literals,reanimation_evidence=detail,
            reconstruction=proof['scope'],reconstruction_body_sha256=sha(function_body(body,NAMES[meta[a]['name']]).encode())))
    source=read(root/SOURCE)+'\n\n#ifndef SHANDALAR\n/* Recovered October 2022 reanimation family. */\n'+'\n'.join(f['prototype'] for f in entries)+'\n'
    for entry in entries:source+=f'\n/* ManalinkEh 2022: 0x{entry["address"]}; original {SOURCE}:{entry["line"]}. */\n'+bodies[entry['address']]+'\n'
    source+='#endif\n'
    header=read(root/'manalink.h');at=header.rfind('#ifdef __cplusplus')
    block=HEADER_LABEL+'\n#ifndef SHANDALAR\ntypedef void (*ReanimationBeforeEtb_2022)(int, int, int, int);\n'+'\n'.join(f['prototype'] for f in entries if not f['static'])+'\n#endif\n\n'
    header=header[:at]+block+header[at:]
    defs=read(root/'defs.h');at=defs.rfind('\n#endif')
    block='\n#ifndef SHANDALAR\n'+DEFS_LABEL+'\ntypedef enum\n{\n'+',\n'.join('  '+REANIMATION_DOMAIN_NAMES[n]+' = '+str(v) for n,v in REANIMATION_ENUM_VALUES.items())+'\n} reanimate_mode_2022_t;\n#endif\n'
    defs=defs[:at]+block+defs[at:]
    scratch=cache/'reanimation-check';scratch.mkdir(exist_ok=True);(scratch/'defs.h').write_bytes(defs.encode('latin1'))
    sample='#include "manalink.h"\n_Static_assert(sizeof(reanimate_mode_2022_t)==4 && (reanimate_mode_2022_t)-1 < 0,"signed 32-bit reanimation action");\n'+'\n'.join(f['prototype'] for f in entries)+'\n'+'\n'.join(bodies.values())
    compile_sources(root,scratch,{SOURCE:sample},header)
    proof['planning']=True;audit_component(proof,{**m,'accepted_functions':m['accepted_functions']+entries},{SOURCE:source},lambda a,n:runtime.pe.get_data(a-0x2000000,n),root);del proof['planning']
    if args.apply:
        citations=m['accepted_functions']+m['source_fragments']+[s for f in m['accepted_functions'] for s in f.get('supporting_exports',[])]
        assert not set(ADDRESSES)&{f['address'] for f in citations}
        rows={r['address']:r for r in csv.DictReader((cache/'decompiled-abi/export.tsv').open(),delimiter='\t')}
        for row in csv.DictReader((directory/'export.tsv').open(),delimiter='\t'):
            rows[row['address']]=row;shutil.copyfile(directory/f'{row["address"]}.c',cache/'decompiled-abi'/f'{row["address"]}.c')
        with (cache/'decompiled-abi/export.tsv').open('w',newline='') as stream:
            writer=csv.DictWriter(stream,fieldnames=['address','source','name','status','prototype'],delimiter='\t');writer.writeheader();writer.writerows(rows.values())
        for path,text in [(SOURCE,source),('manalink.h',header),('defs.h',defs)]:
            (root/path).write_bytes(text.replace('\n','\r\n').encode('latin1'))
        m['accepted_functions']+=entries;m['accepted']+=12;m['helper_functions']+=12;m['private_functions']+=3;m['private_helpers']+=3;m['shared_helper_interfaces']+=9;m['source_files'][SOURCE]+=12
        m['raw_export_entries']=len(rows);m['exported_functions']=len({r['name'] for r in rows.values()})
        save_json(mp,m);interfaces['reanimation_2022']=proof;save_json(ip,interfaces)
    save_json(cache/'reanimation-plan.json',proof);print(json.dumps(dict(applied=args.apply,new_helpers=12,private_helpers=3,public_helpers=9)))


if __name__=='__main__':main()
