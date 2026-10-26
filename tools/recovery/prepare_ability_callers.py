"""Prepare native ability-loss caller bodies, rejecting unresolved machine state.

Source remains untouched. Concrete native views of the shared info_slot word
are restored as integers when Ghidra selected its callback-pointer union arm.
Real callback comparisons continue to use info_slot_fn.
"""
import inspect,json,re,collections
from pathlib import Path
from canonicalize_helpers import ROOT,rename,sha
from promote_recovery import clean,UNRESOLVED,aggregate_stack_aliases
from recover_spell_copy import format_c
from inspect_release import Image
from recover_literals import recover_strings

PENDING={'leaves_play':'leaves_play_2022','other_leaves_play':'other_leaves_play_2022',
 'get_subtypes_by_csvid':'get_subtypes_by_csvid_2022','has_subtype_by_id':'has_subtype_by_id_2022',
 'reset_subtypes':'reset_subtypes_from_instance_2022','generic_split_card':'generic_split_card_2022',
 'remove_counters':'remove_counters_from_source','remove_all_counters':'remove_all_counters_from_source',
 'copy_counters':'copy_counters_from_source','add_counters_predoubled':'add_counters_predoubled_from_source',
 'move_counters':'move_counters_from_source','add_or_remove_counters_impl':'add_or_remove_counters_from_source_impl'}

def typed_views(body,address):
    changes=[]
    def replace(old,new):
        nonlocal body
        assert old in body,(address,old)
        changes.append(dict(before=old,after=new,count=body.count(old)))
        body=body.replace(old,new)
    # The anonymous native field_21 union explicitly selects ext. The shared
    # card_instance_t already exposes this same, verified pointer field.
    if address in {'26df340','2673410'} or '(instance->field_21).ext' in body:
        if '(instance->field_21).ext' in body:replace('(instance->field_21).ext','instance->ext')
    increment={'224a850':'instance','23e7bc0':'pcVar2','211d1c0':'instance','202ca30':'pcVar3'}
    if address in increment:
        var=increment[address]
        replace(var+'->info_slot_fn = '+var+'->info_slot_fn + 1',var+'->info_slot = '+var+'->info_slot + 1')
    if address=='2110850':
        replace('pcVar4->info_slot_fn = (CardFunction)(iVar2 + 1)','pcVar4->info_slot = iVar2 + 1')
    if address=='255dc30':
        replace('pcVar1->info_slot_fn = (CardFunction)(result + 0x42)','pcVar1->info_slot = result + 0x42')
    if address=='250e6f0':
        replace('CardFunction p_Var4;','int p_Var4;')
        replace('(CardFunction)instance->info_slot','instance->info_slot')
        replace('(CardFunction)0x0','0')
        replace('instance->info_slot_fn','instance->info_slot')
    if address in {'20beb30','23e88f0'}:
        index,instance=('pcVar4','pcVar2') if address=='20beb30' else ('pcVar3','pcVar1')
        replace('code *'+index+';','int '+index+';')
        replace(instance+'->info_slot_fn',instance+'->info_slot')
        replace(index+'[(int)&'+instance+'->targets[2].player]',
                '((uint8_t *)&'+instance+'->targets[2].player)['+index+']')
        replace('(code *)0xffffffff','-1')
    if address in {'2368350','262d160'}:
        replace('code *func_ptr;','CardFunction func_ptr;')
    if address=='2459060':
        declarations=[]
        data=json.loads((ROOT/'.recovery/ability-small-native-data.json').read_text())
        for symbol,entry in zip(['_CSWTCH_414','_CSWTCH_415'],data['tables']):
            pattern=r'\*\(int\s*\*\)\(&'+symbol+r'\s*\+\s*uVar8\s*\*\s*4\)'
            new=entry['name']+'[uVar8]';old=body
            body,n=re.subn(pattern,new,body);assert n,(address,symbol)
            changes.append(dict(table=entry,accesses=n))
            declarations.append('  static const int '+entry['name']+'[3] = {'+', '.join(map(str,entry['values']))+'};')
        body=body.replace('{','{\n'+'\n'.join(declarations),1)
    if address=='20b5c30':
        data=json.loads((ROOT/'.recovery/ability-small-native-data.json').read_text())
        entry=next(e for e in data['enum_members'] if e['name']=='DDBM_MAY_DAMAGE_PLANESWALKER')
        assert entry['value']==64
        body=body.replace('{','{\n  enum { DDBM_MAY_DAMAGE_PLANESWALKER = 64 };',1)
        changes.append(dict(native_enum=entry))
    # Optimized private entries have a different argument order from the public
    # C frontend. The native frontend exports show these exact forwarding rules;
    # the worker's unused incoming registers are not C arguments to preserve.
    if address=='236c900':
        replace('  int in_EAX;\n','')
        replace('chroma(player,3,0,in_EAX)','chroma(player,-1,COLOR_GREEN,0)')
        changes.append(dict(public_frontend='0x2363860',worker='0x2363610',
                            binding='player, -1, clr1, clr2 selects the battlefield worker'))
    if address=='253a320':
        replace('  int in_stack_fffffff0;\n','')
        replace('  int in_stack_fffffff4;\n','')
        replace('affinity(player,0x40,~EVENT_0,in_stack_fffffff0,in_stack_fffffff4)',
                'affinity(player,card,event,TYPE_ARTIFACT,-1)')
        changes.append(dict(public_frontend='0x2537d50',worker='0x2537cf0',
                            binding='EVENT_MODIFY_COST branch; public frontend forwards player/type/subtype'))
    if address=='20e6860':
        replace('  anon_union_4_2_59bd3754_for_card_instance_struct_31 aVar5;\n','')
        replace('      aVar5.eot_toughness._2_2_ = 0;\n      aVar5.eot_toughness._0_2_ = cards_drawn_this_turn[player];\n','')
        replace('instance->field_31 = aVar5','instance->eot_toughness = (uint16_t)cards_drawn_this_turn[player]')
    if address=='24fd880':
        replace('_DAT_004ef1a8','player_bits[1]')
        if '_player_bits' in body:replace('_player_bits','player_bits[0]')
    if address=='2245ad0':
        replace('_DAT_0060a534','EXE_DWORD(0x60a534)')
        replace('_DAT_00736808','EXE_DWORD(0x736808)')
        replace('(unsigned int)pcVar1->field_26','(unsigned int)pcVar1->info_slot')
    return body,changes

def inline_prompt_copy(raw,address,runtime):
    """Recover two constant-size native prompt copies, including padding bytes."""
    choices={'202c080':(0x2724028,38,'puVar5','pcVar6','iVar4','bVar7',0x28),
             '205ec90':(0x27257e0,26,'puVar3','pcVar4','iVar2','bVar5',0x1c)}
    if address not in choices:return raw,[]
    pointer,width,src,dst,index,direction,padding=choices[address]
    data=runtime.pe.get_data(pointer-runtime.pe.OPTIONAL_HEADER.ImageBase,width)
    assert len(data)==width and data[-1:]==b'\0'
    start=raw.index('msg[0x'+format(width,'x')+']')
    end=raw.index('new_default_test_definition',start)
    old=raw[start:end]
    assert src in old and dst in old and f'msg + 0x{padding:x}' in old
    # Both native paths initialize all 100 bytes: copied bytes, two zeros, and
    # DWORD zero fill. An explicit byte array avoids alignment/aliasing casts.
    init=json.dumps(data[:-1].decode('ascii'))
    raw=raw[:start]+f'memset(msg,0,sizeof(msg));\n    memcpy(msg,{init},{width});\n    '+raw[end:]
    for declaration in [f'  undefined4 *{src};\n',f'  char *{dst};\n',f'  byte {direction};\n',f'  {direction} = 0;\n']:
        assert declaration in raw;raw=raw.replace(declaration,'')
    return raw,[dict(address=hex(pointer),width=width,bytes_hex=data.hex(),
                     bytes_sha256=sha(data),initialization='100 bytes, exact constant copy and zero padding')]

def main():
    mapping={e['recovery_name']:e['canonical_name'] for e in json.loads((ROOT/'docs/recovery-evidence/helper-canonicalization.json').read_text())['functions']}
    flags={n:True for n in inspect.signature(clean).parameters if n.startswith('use_')}
    plan=json.loads((ROOT/'.recovery/ability-caller-plan.json').read_text());report=[]
    runtime=Image((ROOT/'.recovery/2022-ManalinkEh.dll').read_bytes())
    candidate=ROOT/'.recovery/ability-caller-bodies';candidate.mkdir(exist_ok=True)
    for row in plan:
        if row['status']!='recover native caller':continue
        assert len(row['native_ranges'])==1,(row['source'],row['caller'])
        address=row['native_ranges'][0]['low_pc'][2:]
        path=ROOT/'.recovery/decompiled-ability-callers'/(address+'.c');raw=path.read_text()
        prompt_raw,prompt_literals=inline_prompt_copy(raw,address,runtime)
        strings,literals=recover_strings(prompt_raw,runtime,{})
        literals+=prompt_literals
        body=rename(rename(clean(strings,overlapping_globals=['trigger_condition'],**flags),PENDING),mapping)
        body,views=typed_views(body,address)
        storage=[]
        if aggregate_stack_aliases(body):
            evidence=json.loads((ROOT/'.recovery/ability-stack-views.json').read_text())
            if address in evidence:
                from recover_stack_storage import rewrite
                body=rename(body,{'test_definition_2022_t':'test_definition_t',
                    'target_definition_2022_t':'target_definition_t','token_generation_2022_t':'token_generation_t'})
                body,storage=rewrite(body,aggregate_stack_aliases(body),evidence[address]['groups'],
                    dict(test_definition_t=216,target_definition_t=84,token_generation_t=76,int=4,**{'char[100]':100}))
                if address=='245da90':
                    from resolve_stack_partials import resolve_partials
                    layouts=json.loads((ROOT/'docs/recovery-evidence/stack-storage.json').read_text())['target_layouts']
                    body,partial,unresolved=resolve_partials(body,storage,layouts)
                    assert not unresolved,unresolved
                    for n in [1,2,3]:body=body.replace('damage_card._'+str(n)+'_1_',
                        '((unsigned int)damage_card >> '+str(8*n)+' & 0xff)')
                    views.append(dict(partial_stack_views=partial,scalar_byte_reads='32-bit damage_card; no write through a byte extraction'))
                    # This phi value only restores a dead stack word. All
                    # paths that use td overwrite it with default_target_definition.
                    for fragment in ['    iVar2 = (uint32_t)(recovered_stack_token.target.extra);\n',
                                     'iVar2 = (uint32_t)(recovered_stack_token.target.extra),',
                                     '      recovered_stack_token.target.extra = (int32_t)(iVar2);\n']:
                        assert fragment in body,fragment
                        body=body.replace(fragment,'')
                    views.append(dict(dead_stack_phi='target.extra; overwritten before every use by default_target_definition'))
        issues=sorted(set(UNRESOLVED.findall(body)));stack=aggregate_stack_aliases(body)
        status='needs reconstruction' if issues or stack else 'compile candidate'
        if status=='compile candidate':(candidate/(address+'.c')).write_text(format_c(body))
        report.append(dict(source=row['source'],caller=row['caller'],address=address,status=status,
            unresolved=issues,stack_aliases=stack,typed_views=views,raw_sha256=sha(raw.encode()),
            recovered_literals=literals,
            stack_reconstruction=storage,
            recipe=dict(clean_flags=flags,pending_names=PENDING,canonical_names=mapping)))
    (ROOT/'.recovery/ability-caller-body-candidates.json').write_text(json.dumps(report,indent=2)+'\n')
    print(dict(collections.Counter(r['status'] for r in report)))

if __name__=='__main__':main()
