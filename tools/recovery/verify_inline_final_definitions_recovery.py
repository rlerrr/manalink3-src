"""Verify final inline definitions and Portal's original chooser identity."""
import json
from pathlib import Path
from capstone import Cs,CS_ARCH_X86,CS_MODE_32
from capstone.x86 import X86_OP_IMM,X86_OP_MEM,X86_OP_REG,X86_REG_ESP
from canonicalize_helpers import ROOT,read,sha,span
from inspect_release import Image
from verify_inline_wrapper_recovery import verify as verify_definitions
from verify_inline_card_mechanics_recovery import function_bounds,symbol_address
from verify_native_absolute_bindings import runtime_sections,stable_sections


def chooser_arguments(image,debug,name,source_leaf):
    lo,hi=function_bounds(debug,name,source_leaf);base=image.pe.OPTIONAL_HEADER.ImageBase
    data=image.pe.get_data(lo-base,hi-lo)
    md=Cs(CS_ARCH_X86,CS_MODE_32);md.detail=True;ins=list(md.disasm(data,lo))
    assert sum(i.size for i in ins)==len(data)
    target=symbol_address(debug,image,'exiledby_choose')
    leaders={lo}
    for i in ins:
        if i.mnemonic.startswith('j'):
            if i.operands[0].type==X86_OP_IMM:leaders.add(i.operands[0].imm)
            leaders.add(i.address+i.size)
    arguments={};registers={};calls=[]
    for i in ins:
        if i.address in leaders:arguments.clear();registers.clear()
        ops=i.operands
        if i.mnemonic=='call':
            if ops[0].type==X86_OP_IMM and ops[0].imm==target:
                assert all(k in arguments for k in [8,12,16,20,24]),(name,hex(i.address),arguments)
                prompt=image.pe.get_data(arguments[20]-base,80).split(b'\0')[0].decode('latin1')
                calls.append(dict(address=hex(i.address),csvid=arguments[8],flags=arguments[12],
                                  testarg=arguments[16],prompt=prompt,last=arguments[24]))
            arguments.clear()
            for reg in ['eax','ecx','edx']:registers.pop(reg,None)
            continue
        if i.mnemonic=='mov' and len(ops)==2:
            val=ops[1].imm if ops[1].type==X86_OP_IMM else registers.get(md.reg_name(ops[1].reg)) if ops[1].type==X86_OP_REG else None
            if ops[0].type==X86_OP_MEM and ops[0].mem.base==X86_REG_ESP and not ops[0].mem.index:
                if val is not None:arguments[ops[0].mem.disp]=val&0xffffffff
                else:arguments.pop(ops[0].mem.disp,None)
            if ops[0].type==X86_OP_REG:
                if val is not None:registers[md.reg_name(ops[0].reg)]=val&0xffffffff
                else:registers.pop(md.reg_name(ops[0].reg),None)
        else:
            for reg in i.regs_access()[1]:registers.pop(md.reg_name(reg),None)
    assert calls,name
    return dict(name=name,address=hex(lo),end=hex(hi),bytes_sha256=sha(data),
                instructions=[[hex(i.address),i.mnemonic,i.op_str] for i in ins],calls=calls)


def portal_analysis(image,debug):
    names={n for n,v,s,k,c in debug.symbols if s>0 and k==32}
    name='prototype_portal_impl' if '_prototype_portal_impl' in names else 'card_prototype_portal'
    result=chooser_arguments(image,debug,name,'scars_of_mirrodin.c')
    expected={(11338,0x140002,0,'',1),(11338,0x100000,0x6d,'',1),(11338,0,0,'Creature',0)}
    actual={(r['csvid'],r['flags'],r['testarg'],r['prompt'],r['last']) for r in result['calls']}
    assert actual==expected,actual
    return result


def delegated_to_foundry(image,debug):
    lo,hi=function_bounds(debug,'card_prototype_portal','scars_of_mirrodin.c')
    target=symbol_address(debug,image,'soul_foundry_impl');md=Cs(CS_ARCH_X86,CS_MODE_32);md.detail=True
    ins=list(md.disasm(image.pe.get_data(lo-image.pe.OPTIONAL_HEADER.ImageBase,hi-lo),lo))
    sites=[hex(i.address) for i in ins if i.mnemonic in ['call','jmp'] and i.operands[0].type==X86_OP_IMM and i.operands[0].imm==target]
    assert len(sites)==1
    return dict(name='soul_foundry_impl',sites=sites)


def verify_runtime(record,before,after,root):
    control=Image((root/record['runtime_control']['path']).read_bytes())
    assert sha(control.data)==record['runtime_control']['dll_sha256']
    assert stable_sections(runtime_sections(before))==stable_sections(runtime_sections(control))
    left,right=record['control_link_inputs'],record['corrected_link_inputs'];assert set(left)==set(right)
    changed=[p for p in left if left[p]!=right[p]]
    assert changed==['.recovery/build-gcc93/obj/cards/scars_of_mirrodin.o']
    for key,expected in [('control_portal_object',left[changed[0]]),('corrected_portal_object',right[changed[0]])]:
        obj=record[key];assert sha((root/obj['path']).read_bytes())==obj['sha256']==expected
    old=record['control_debug'];new=record['corrected_debug']
    old_bytes=(root/old['path']).read_bytes();new_bytes=(root/new['path']).read_bytes()
    assert sha(old_bytes)==old['sha256'] and sha(new_bytes)==new['sha256']
    assert delegated_to_foundry(control,Image(old_bytes))==record['control_foundry_delegation']
    assert portal_analysis(after,Image(new_bytes))==record['compiled_portal']


def verify_superseded_definition(proof,image,root,source,name,old_hash):
    assert (source,name)==('cards/scars_of_mirrodin.c','card_prototype_portal')
    record=json.loads((root/proof['native_inline_final_definitions_recovery']).read_bytes())
    assert sha(image.data)==record['native_dll_sha256']
    row=record['source_correction'];assert row['before_sha256']==old_hash and row['applied']
    t=read(root/source);a,b=span(t,name)
    assert t[a:b]==row['after_definition'] and sha(t[a:b].encode())==row['after_sha256']
    assert row['before_definition'].replace('return soul_foundry_impl(player, card, event);',
        'return prototype_portal_impl(player, card, event);')==row['after_definition']
    assert len(record['native_portal']['calls'])==4


def verify(proof,image,root=ROOT):
    key='native_inline_final_definitions_recovery';path=proof.get(key)
    if not path:return {}
    result=verify_definitions(proof,image,root,record_key=key,expected_count=22,
        edit_key='native_inline_final_definitions_extension',runtime_check=verify_runtime)
    record=json.loads((root/path).read_bytes())
    debug=Image(Path('G:/Manalink/ManalinkEh_10_22_Hotfix.dbg').read_bytes())
    assert portal_analysis(image,debug)==record['native_portal']
    row=record['source_correction'];verify_superseded_definition(proof,image,root,row['source'],row['name'],row['before_sha256'])
    assert any(e.get('native_inline_final_definitions_correction')==path and
               e['before']==row['before_definition'] and e['after']==row['after_definition'] for e in proof['source_edits'])
    current=Image((root/'ManalinkEh.dll').read_bytes());current_debug=Image((root/'ManalinkEh.dbg').read_bytes())
    portal_analysis(current,current_debug)
    assert len(record['unresolved_semantic_limits'])==6
    result.update(control_runtime_sections_unchanged=26,corrected_native_callers=1,
                  only_changed_link_object='cards/scars_of_mirrodin.o',
                  remaining_missing_inline_definitions=0,
                  explicitly_unresolved_return_or_mode_semantics=record['unresolved_semantic_limits'])
    return result


if __name__=='__main__':
    from canonicalize_helpers import load
    print(json.dumps(verify(load(),Image((ROOT/'.recovery/2022-ManalinkEh.dll').read_bytes())),indent=2))
