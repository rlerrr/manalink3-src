"""Prepare remaining native interface callers, with physical callee binding.

Writes candidates and evidence only. Unresolved storage, register values and
ambiguous optimized callees remain review cases; they never receive defaults.
"""
import collections,csv,inspect,json,re
from pathlib import Path
from capstone import Cs,CS_ARCH_X86,CS_MODE_32
from capstone.x86_const import X86_OP_IMM
from elftools.dwarf.locationlists import LocationParser,LocationExpr,BaseAddressEntry
from elftools.dwarf.dwarf_expr import DWARFExprParser
from canonicalize_helpers import ROOT,read,rename,sha,span,mask,source_names as canonical_names
from unify_scalar_helpers import parameters
from inspect_release import Image,value,inherited_value
from export_abi import type_of,type_name
from promote_recovery import clean,UNRESOLVED,aggregate_stack_aliases
from prepare_ability_callers import PENDING,typed_views,inline_prompt_copy
from recover_literals import recover_strings,byte_literal
from recover_spell_copy import format_c
from recover_stack_storage import rewrite,descendants,MEMBERS
from resolve_stack_partials import resolve_partials

INVENTORY=ROOT/'.recovery/native-pending-interface-callers-631.json'
DIRECTORY=ROOT/'.recovery/decompiled-pending-interface-callers'
OUTPUT=ROOT/'.recovery/interface-caller-bodies'
REPORT=ROOT/'.recovery/interface-caller-body-candidates.json'
PUBLIC_ENTRIES={0x26738b0:'functions/has_creature_type.c',0x2681910:'functions/functions.c',
    0x268dc60:'functions/functions.c',0x26d6eb0:'functions/exiledby.c',
    0x22c4ca0:'cards/vanguard.c',0x2691770:'functions/functions.c',
    0x26d4260:'functions/counters.c',0x2693be0:'functions/functions.c'}

def prompt_copy(raw,address,native):
    choices={'2066720':(0x2725c24,24,'puVar5','pcVar6','bVar7','puVar5 ='),
             '26414d0':(0x275c318,34,'puVar2','pcVar3',None,"msg[0x23] =")}
    if address not in choices:return raw,[]
    pointer,width,src,dst,direction,anchor=choices[address]
    data=native.pe.get_data(pointer-0x2000000,width);assert len(data)==width
    start=raw.index(anchor);end=raw.index('new_default_test_definition',start);old=raw[start:end]
    assert f'UNK_{pointer:08x}' in old and 'msg +' in old
    raw=raw[:start]+'memset(msg,0,sizeof(msg));\n    memcpy(msg,'+byte_literal(data)+','+str(width)+');\n    '+raw[end:]
    for declaration in ['  undefined4 *'+src+';\n','  char *'+dst+';\n']:
        assert declaration in raw;raw=raw.replace(declaration,'')
    if direction:
        for declaration in ['  byte '+direction+';\n','  '+direction+' = 0;\n']:
            assert declaration in raw;raw=raw.replace(declaration,'')
    return raw,[dict(address=hex(pointer),width=width,bytes_hex=data.hex(),bytes_sha256=sha(data),initialization='100 bytes; fixed native copy followed by zero padding')]

def main():
    inventory=json.loads(INVENTORY.read_text());proof=json.loads((ROOT/'docs/recovery-evidence/helper-canonicalization.json').read_text())
    mapping=canonical_names()
    accepted=json.loads((ROOT/'docs/recovery-evidence/recovered-functions.json').read_text())['accepted_functions']
    accepted={int(f['address'],16):mapping.get(f['name'],f['name']) for f in accepted}
    census={int(r['low_pc'],16):r for r in csv.DictReader((ROOT/'.recovery/2022-functions.csv').open()) if r['low_pc'] and r['high_pc']}
    names=collections.defaultdict(set)
    for at,row in census.items():names[row['name']].add(at)
    native=Image((ROOT/'.recovery/2022-ManalinkEh.dll').read_bytes());assert sha(native.data)==inventory['dll_sha256']
    data=Path('G:/Manalink/ManalinkEh_10_22_Hotfix.dbg').read_bytes();assert sha(data)==inventory['debug_sha256']
    dwarf=Image(data).dwarf();locations=LocationParser(dwarf.location_lists());expressions=DWARFExprParser(dwarf.structs)
    decoder=Cs(CS_ARCH_X86,CS_MODE_32);decoder.detail=True
    flags={n:True for n in inspect.signature(clean).parameters if n.startswith('use_')}
    public_interfaces={}
    for address,source in PUBLIC_ENTRIES.items():
        entry=census[address];die=dwarf.get_DIE_from_refaddr(int(entry['die_offset'],16))
        ps=[c for c in die.iter_children() if c.tag=='DW_TAG_formal_parameter'];name=entry['name']
        text=read(ROOT/source);lo,hi=span(text,name);prototype=text[lo:hi].split('{',1)[0].strip()
        cparams=parameters(mask(prototype)[prototype.index('(')+1:prototype.rindex(')')])
        cnames=[re.findall(r'\b\w+\b',p)[-1] for p in cparams]
        assert cnames==[inherited_value(p,'DW_AT_name') for p in ps],(name,cnames)
        evidence=[]
        for index,param in enumerate(ps):
            loc=locations.parse_from_attribute(param.attributes['DW_AT_location'],die.cu['version'],param)
            expr=loc.loc_expr if isinstance(loc,LocationExpr) else next(e.loc_expr for e in loc if not isinstance(e,BaseAddressEntry))
            ops=expressions.parse_expr(expr)
            assert len(ops)==1 and ops[0].op_name=='DW_OP_fbreg' and ops[0].args==[index*4],(name,index,ops)
            evidence.append(dict(name=cnames[index],type=type_name(type_of(param)),die=hex(param.offset),entry_frame_offset=index*4))
        public_interfaces[address]=dict(name=name,source=source,prototype=prototype,parameters=evidence,
            scope='Public ABI binding only; existing callee body remains separate review work.')
    # MinGW's variadic snprintf wrapper has the usual three fixed cdecl slots.
    address=0x22c3350;die=dwarf.get_DIE_from_refaddr(int(census[address]['die_offset'],16))
    ps=[c for c in die.iter_children() if c.tag=='DW_TAG_formal_parameter']
    assert [inherited_value(p,'DW_AT_name') for p in ps]==['__stream','__n','__format']
    for index,param in enumerate(ps):
        ops=expressions.parse_expr(value(param,'DW_AT_location'))
        assert len(ops)==1 and ops[0].op_name=='DW_OP_fbreg' and ops[0].args==[index*4]
    public_interfaces[address]=dict(name='snprintf',scope='Standard cdecl variadic CRT interface; three fixed stack slots unchanged.')
    record_names={'test_definition_2022_t':'test_definition_t','target_definition_2022_t':'target_definition_t','token_generation_2022_t':'token_generation_t'}
    report=[];OUTPUT.mkdir(exist_ok=True)
    for row in inventory['callers']:
        result=dict(source=row['source'],caller=row['caller'],native_ranges=row['native_ranges'])
        report.append(result)
        if len(row['native_ranges'])!=1:
            result['status']='native range review';continue
        nr=row['native_ranges'][0];at=int(nr['low_pc'],16);end=int(nr['high_pc'],16);address=f'{at:x}'
        result['address']=address
        path=DIRECTORY/(address+'.c');raw=path.read_text();raw2,prompt_literals=inline_prompt_copy(raw,address,native)
        raw2,extra_literals=prompt_copy(raw2,address,native);prompt_literals+=extra_literals
        literals_raw,literals=recover_strings(raw2,native,{})
        body=rename(rename(rename(clean(literals_raw,overlapping_globals=['trigger_condition'],**flags),PENDING),mapping),record_names)
        body,views=typed_views(body,address)
        # An explicitly selected ext member is already the shared pointer field.
        body,n=re.subn(r'\(\((\w+)->field_21\)\.ext\)',r'\1->ext',body)
        if n:views.append(dict(extension_pointer_views=n))
        groups=collections.defaultdict(list);storage=[];stack_issue=None
        if aggregate_stack_aliases(body):
            die=dwarf.get_DIE_from_refaddr(int(nr['die_offset'],16))
            for variable in descendants(die):
                if variable.tag!='DW_TAG_variable' or 'DW_AT_location' not in variable.attributes:continue
                typ=type_name(type_of(variable))
                if typ not in MEMBERS:continue
                loc=locations.parse_from_attribute(variable.attributes['DW_AT_location'],die.cu['version'],variable)
                exprs=[loc.loc_expr] if isinstance(loc,LocationExpr) else [e.loc_expr for e in loc if not isinstance(e,BaseAddressEntry)]
                offsets=set()
                for expr in exprs:
                    ops=expressions.parse_expr(expr)
                    if len(ops)==1 and ops[0].op_name=='DW_OP_fbreg':offsets.add(ops[0].args[0])
                for offset in offsets:groups[offset].append(dict(name=inherited_value(variable,'DW_AT_name'),type=typ,die=hex(variable.offset)))
            try:body,storage=rewrite(body,aggregate_stack_aliases(body),groups,dict(test_definition_t=216,target_definition_t=84,token_generation_t=76,int=4,**{'char[100]':100}))
            except AssertionError as error:stack_issue=str(error)
        if storage:
            layouts=json.loads((ROOT/'docs/recovery-evidence/stack-storage.json').read_text())['target_layouts']
            body,partial,remaining=resolve_partials(body,storage,layouts)
            if partial:views.append(dict(verified_stack_partials=partial))
        # No-argument EXE calls retain their fixed addresses and side effects.
        # These sites have no return-value consumer or incoming argument frame.
        for pointer in [0x477070,0x477a90,0x437e20]:
            old=f'(*(code *)0x{pointer:x})()'
            if old in body:
                body=body.replace(old,f'EXE_FN(void, 0x{pointer:x}, void)()')
                views.append(dict(absolute_call=hex(pointer),parameters=[],result='discarded'))
        if '(*(code *)0x499010)(0)' in body:
            body=body.replace('(*(code *)0x499010)(0)','EXE_FN(int, 0x499010, int)(0)')
            views.append(dict(absolute_call='0x499010',parameters=[0],result='discarded'))
        if address=='253c530':
            old='(unsigned int *)&stack0xfffffff0';assert old in body
            body=body.replace(old,'stolen_subtypes.reported_subtypes + 26')
            anchors=[]
            for pc,assembly in [(0x253c666,'lea'),(0x253c718,'lea')]:
                instruction=next(decoder.disasm(native.pe.get_data(pc-0x2000000,8),pc))
                assert instruction.mnemonic==assembly
                assert instruction.op_str==('esi, [ebp - 0x74]' if pc==0x253c666 else 'eax, [ebp - 0xc]')
                anchors.append(dict(address=hex(pc),bytes=instruction.bytes.hex(),instruction=instruction.op_str))
            assert 0x74-0xc==26*4
            views.append(dict(array_end='stolen_subtypes.reported_subtypes + 26',native_anchors=anchors,bytes=104))
        if address=='26ed920':
            returns=[i for i in decoder.disasm(native.pe.get_data(at-0x2000000,end-at),at) if i.mnemonic=='ret']
            assert returns and all(i.op_str=='8' for i in returns)
            assert 'int is_displayed_as_attached(' in body
            body=body.replace('int is_displayed_as_attached(','int __stdcall is_displayed_as_attached(',1)
            views.append(dict(calling_convention='__stdcall',native_returns=[dict(address=hex(i.address),bytes=i.bytes.hex()) for i in returns]))
        if address=='22b6ca0':
            for old,new in [('CardFunction p_Var4;','int p_Var4;'),
                            ('instance->info_slot_fn','instance->info_slot'),
                            ('(CardFunction)0x1','1'),('(CardFunction)0x0','0'),
                            ('(code *)0x1 < p_Var4 + -2','1U < (unsigned int)p_Var4 - 2U')]:
                assert old in body,(address,old);body=body.replace(old,new)
            body=re.sub(r'\(CardFunction\)\s*(?=dialog_impl(?:_2022)?\s*\()','',body).replace('(CardFunction)0x3','3')
            views.append(dict(integer_info_slot='p_Var4; four-byte state word, unsigned range check'))
        if address=='221cd70':
            assert '(unsigned int)instance->field_26' in body
            body=body.replace('(unsigned int)instance->field_26','(unsigned int)instance->info_slot')
            views.append(dict(integer_info_slot='instance; four-byte flags word'))
        if address=='2120f40':
            assert '_DAT_00736808' in body;body=body.replace('_DAT_00736808','EXE_DWORD(0x736808)')
            views.append(dict(exe_global='0x736808',width=4))
        if address=='26d70d0':
            assert 'code *func_ptr;' in body;body=body.replace('code *func_ptr;','CardFunction func_ptr;')
            assert '_store_counters_legacy' in body;body=body.replace('_store_counters_legacy','store_counters_legacy')
            views.append(dict(callback_views='CardFunction; C store_counters_legacy resolves the native COFF entry'))
        if address=='23347a0':
            assert '(test_definition_t_conflict *)&test' in body
            body=body.replace('(test_definition_t_conflict *)&test','&test')
            views.append(dict(test_pointer_view='existing canonical test local and choose_a_card_test_ct parameter'))
        if address=='26c8340':
            old='  uVar3._0_2_ = cards_data[card].id;\n  uVar3._2_2_ = cards_data[card].reserved2;'
            assert old in body;body=body.replace(old,'  uVar3 = (uint16_t)cards_data[card].id | ((uint32_t)(uint16_t)cards_data[card].reserved2 << 16);')
            body,n=re.subn(r'uVar3\._0_2_ = (uVar[12]);',r'uVar3 = (uVar3 & 0xffff0000U) | (uint16_t)\1;',body);assert n==2
            views.append(dict(partial_integer='uVar3; low and high 16-bit fields of the same four-byte card data load'))
        if address in {'2681660','2663bc0'}:
            enum_name='SF2_0' if address=='2681660' else 'SUBTYPE_NOBLE'
            die=dwarf.get_DIE_from_refaddr(int(nr['die_offset'],16));found=[]
            for enum in die.cu.get_top_DIE().iter_children():
                if enum.tag!='DW_TAG_enumeration_type':continue
                for member in enum.iter_children():
                    if value(member,'DW_AT_name')==enum_name:found.append((value(member,'DW_AT_const_value'),member.offset))
            assert len(found)==1,(address,enum_name,found)
            number,enum_die=found[0]
            if enum_name=='SF2_0':body=rename(body,{enum_name:str(number)})
            else:body=body.replace('{','{\n  enum { '+enum_name+' = '+str(number)+' };',1)
            views.append(dict(native_enum=enum_name,value=number,die=hex(enum_die)))
        # Bind direct native targets to recovered C identities. In particular,
        # do not bind a reordered private worker to a same-named public API.
        bindings=collections.defaultdict(set);ambiguities=[];sites=[]
        for ins in decoder.disasm(native.pe.get_data(at-0x2000000,end-at),at):
            if ins.mnemonic not in {'call','jmp'} or len(ins.operands)!=1 or ins.operands[0].type!=X86_OP_IMM:continue
            target=ins.operands[0].imm;callee=census.get(target)
            if not callee or at<=target<end:continue
            original=callee['name'];expected=accepted.get(target)
            if expected:bindings[original].add(expected)
            elif target in public_interfaces:
                sites.append(dict(name=original,target=hex(target),public_abi=public_interfaces[target],site=hex(ins.address),bytes=ins.bytes.hex()))
            elif len(names[original])>1 and re.search(r'\b'+re.escape(original)+r'\s*\(',body):
                ambiguities.append(dict(name=original,target=hex(target),site=hex(ins.address),bytes=ins.bytes.hex()))
            if expected:sites.append(dict(name=original,target=hex(target),expected=expected,site=hex(ins.address),bytes=ins.bytes.hex()))
        for original,targets in bindings.items():
            if len(targets)!=1:
                if re.search(r'\b'+re.escape(original)+r'\s*\(',body):ambiguities.append(dict(name=original,targets=sorted(targets)))
                continue
            expected=next(iter(targets))
            if original!=expected and re.search(r'\b'+re.escape(original)+r'\s*\(',body):
                head,tail=body.split('{',1);body=head+'{'+rename(tail,{original:expected})
        issues=sorted(set(UNRESOLVED.findall(body)));aliases=aggregate_stack_aliases(body)
        result.update(raw_export=path.relative_to(ROOT).as_posix(),raw_sha256=sha(raw.encode()),
            native_bytes_sha256=sha(native.pe.get_data(at-0x2000000,end-at)),unresolved=issues,stack_aliases=aliases,
            stack_issue=stack_issue,stack_groups=dict(groups),stack_reconstruction=storage,typed_views=views,
            recovered_literals=literals+prompt_literals,callee_bindings=sites,ambiguous_callees=ambiguities)
        result['status']='needs reconstruction' if issues or aliases or stack_issue or ambiguities else 'compile candidate'
        if result['status']=='compile candidate':(OUTPUT/(address+'.c')).write_text(format_c(body))
        # Retain unresolved text for focused reconstruction without changing C.
        else:(OUTPUT/(address+'.review.c')).write_text(body)
    REPORT.write_text(json.dumps(dict(
        inventory_sha256=sha(INVENTORY.read_bytes()),canonical_checkpoint_sha256=sha((ROOT/'docs/recovery-evidence/helper-canonicalization.json').read_bytes()),
        recipe=dict(flags=flags,pending_names=PENDING,record_names=record_names),callers=report),indent=2)+'\n')
    print(dict(collections.Counter(r['status'] for r in report)))
    print('unresolved',dict(collections.Counter(x for r in report for x in r.get('unresolved',[]))))
    print('ambiguous',dict(collections.Counter(x['name'] for r in report for x in r.get('ambiguous_callees',[]))))

if __name__=='__main__':main()
