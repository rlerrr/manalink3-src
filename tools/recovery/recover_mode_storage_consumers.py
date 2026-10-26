"""Recover native modal handlers, packed card state and activated-counter helpers."""
import argparse,csv,json,re,shutil,struct
from pathlib import Path
from capstone import Cs,CS_ARCH_X86,CS_MODE_32
from capstone.x86_const import X86_OP_IMM
from inspect_release import Image,value
from export_abi import type_of
from promote_recovery import strip_comments,UNRESOLVED
from recover_test_consumers import read,rename
from recover_combat_mask_cards import cleaned
from recover_literals import recover_strings,readonly_string,byte_literal
from recover_spell_copy import compile_sources
from recover_phase_prompt_cards import debug_evidence
from recover_leaves_test_native import native_layouts
from recover_temporary_exile import save_json,sha
from audit_source import function_body,target_validation_input

VERDANT='2242d30';CRYPT_B='25c5920';CRYPT_E='25c5f90';INSCRIPTION='23095c0';PENANCE='264ee80';LUTRI='228b8f0';VISION='20253c0';FALLEN='2305650';SYMBOL='2305d40';SLY='25c6ed0';CAST='2692e80';VALIDATE='2692ef0';RAW='2692fc0';RESOLVE='2692ff0'
ADDRESSES=[CAST,VALIDATE,RAW,RESOLVE,VERDANT,CRYPT_B,CRYPT_E,INSCRIPTION,PENANCE,LUTRI,VISION,FALLEN,SYMBOL,SLY]
HELPERS={CAST,VALIDATE,RAW,RESOLVE};PRIVATE=set();ALIASES={}
LEGACY=[('functions/functions.c',n) for n in ['cast_counter_activated_ability','validate_counter_activated_ability','raw_counter_activated_ability','resolve_counter_activated_ability']]
LABEL='/* October 2022 native activated-ability counter actions. */'
NAMES={n:n+'_2022' for s,n in LEGACY}
NAMES.update(can_counter_activated_ability='can_counter_activated_ability_2022',phase_out='phase_out_2022',new_effect_coercion='new_effect_coercion_2022')
CHOICES={VERDANT:{'CHOICE_INVALID_TARGET':0,'CHOICE_SQUIRREL':1,'CHOICE_COUNTER_PW_ABIL':2,'CHOICE_EXILE_CARD':3,'CHOICE_3_LIFE':4},CRYPT_B:{'CHOICE_UNTAP':1,'CHOICE_TAP':2,'CHOICE_DISCARD_DRAW':3,'CHOICE_RAISE':4},CRYPT_E:{'CHOICE_COUNTER':1,'CHOICE_BOUNCE':2,'CHOICE_UNTAP':3,'CHOICE_MILL':4}}
CONFIG_IMPORT='__attribute__((dllimport)) void get_config_data(int config, int item, int *result);'
ARRAYS={'basic_land_subtypes':('2723acc',5),'land_subtypes':('2723ae0',13)}

CONTRACTS={'228b8f0': {'228ba32': '8d049d00000000', '228ba39': 'e892074700', '228ba43': '8d542414', '228ba4b': '89d9', '228ba4f': 'c1e902', '228ba52': 'f3ab', '228ba59': '66c7070000', '228ba66': 'c60700', '228ba6c': '8d98d0070000', '228ba8c': '31c9', '228ba8e': '668b4f24', '228ba92': '8d0c8a'}, '20253c0': {'2025690': 'd3e2', '20256e7': 'd3e3', '20256f2': '8b1c9dcc3a7202', '202570b': '8b1485e03a7202'}, '23095c0': {'2309663': 'b915000000', '230966e': 'f3a5', '230967c': 'b915000000', '2309687': 'f3a5', '2309699': 'b915000000', '23096b0': 'f3a5', '2309749': 'b915000000', '2309754': 'f3a5', '2309762': 'b915000000', '230976d': 'f3a5', '230979e': 'f3a5', '2309a1f': 'b915000000', '2309a29': 'f3a5', '2309a37': 'b915000000', '2309a42': 'f3a5', '2309a4d': 'b915000000', '2309a60': 'f3a5'}, '2305d40': {'2305daa': 'c744240401000000', '2305db2': '89442408', '2305db6': 'c70424ffffffff', '2305dbd': '8b3d70537b02', '2305dc3': 'ffd7', '2305dfd': 'c744240402000000', '2305e05': '89442408', '2305e09': 'c7042403000000', '2305e10': 'ffd7', '2305e6f': '8a00', '2305e71': '3cc5', '2305e7a': 'c684052effffff01', '2305e98': '8d852fffffff', '2305e9e': '8d4df4', '2305ef3': '66c78530ffffff0000', '2305efc': 'c68532ffffff00'}, '2692e80': {'2692eb7': '8b0cd5a0965000', '2692ebe': '8b1cd5a4965000', '2692ed9': '42', '2692eda': '83c164', '2692edd': '885036'}, '2692ef0': {'2692f65': '0fbf04852c965000', '2692f7a': '897308'}, '2242d30': {'22430df': '8b8520feffff', '22430e5': '8b9524feffff', '2243105': '8b851cfeffff', '2243119': '8b8518feffff', '2243234': '8d8500feffff', '224323a': '39c6'}, '25c5920': {'25c5c88': '8d859cfdffff', '25c5d58': '8d859cfdffff', '25c5d5e': '897c240c', '25c5d70': 'e89bb90900', '25c5d87': '8d859cfdffff', '25c5d8d': '897c240c', '25c5d9f': 'e86cb90900'}, '25c5f90': {'25c61ba': '399d14feffff', '25c6214': '39bd14feffff', '25c65fd': '39bd14feffff'}}

def native_storage(debug,runtime):
    for sites in CONTRACTS.values():
        for at,data in sites.items():assert runtime.pe.get_data(int(at,16)-0x2000000,len(bytes.fromhex(data)))==bytes.fromhex(data)
    runtime.pe.parse_data_directories()
    imports=[(d.dll.decode(),i.name.decode(),f'{i.address:x}') for d in runtime.pe.DIRECTORY_ENTRY_IMPORT for i in d.imports if i.name and i.name==b'get_config_data']
    assert imports==[('Drawcardlib.dll','get_config_data','27b5370')]
    assert runtime.pe.get_data(0x305e6f,2)==bytes.fromhex('8a00')
    result=dict(native_contract_sites=CONTRACTS,config_import=dict(dll=imports[0][0],name=imports[0][1],iat_address=imports[0][2],prototype=CONFIG_IMPORT),command_enums={},vision_arrays={},rarity_expansion_prefix=dict(offset=0,field_bytes=1,instruction=dict(address='2305e6f',bytes='8a00')))
    for cu in debug.dwarf().iter_CUs():
        source=value(cu.get_top_DIE(),'DW_AT_name')
        if source not in {'cards/modern_horizons_2.c','cards/unstable.c','cards/visions.c','cards/unglued_unhinged.c','cards/ikoria.c'}:continue
        for d in cu.iter_DIEs():
            name=value(d,'DW_AT_name')
            if name not in {'CommandChoices',*ARRAYS,'options'}:continue
            parent=d.get_parent()
            while parent and parent.tag!='DW_TAG_subprogram':parent=parent.get_parent()
            pc=value(parent,'DW_AT_low_pc') if parent else None;a=f'{pc:x}' if pc else ''
            if d.tag=='DW_TAG_typedef' and name=='CommandChoices' and a in CHOICES:
                t=type_of(d);members={value(c,'DW_AT_name'):value(c,'DW_AT_const_value') for c in t.iter_children()}
                assert members==CHOICES[a] and value(t,'DW_AT_byte_size')==4
                result['command_enums'][a]=dict(bytes=4,members=members)
            if d.tag=='DW_TAG_variable' and a==VISION and name in ARRAYS:
                at,count=ARRAYS[name];loc=value(d,'DW_AT_location');assert loc==[3,*bytes.fromhex(at.zfill(8))[::-1]]
                t=type_of(d)
                while t.tag in {'DW_TAG_const_type','DW_TAG_typedef'}:t=type_of(t)
                assert t.tag=='DW_TAG_array_type' and value(next(c for c in t.iter_children() if c.tag=='DW_TAG_subrange_type'),'DW_AT_upper_bound')==count-1
                data=runtime.pe.get_data(int(at,16)-0x2000000,count*4)
                result['vision_arrays'][name]=dict(address=at,count=count,bytes=data.hex(),values=list(struct.unpack('<'+'i'*count,data)))
            if d.tag=='DW_TAG_variable' and a==LUTRI and name=='options':
                t=type_of(d)
                while t.tag in {'DW_TAG_const_type','DW_TAG_typedef'}:t=type_of(t)
                upper=next(c for c in t.iter_children() if c.tag=='DW_TAG_subrange_type').attributes['DW_AT_upper_bound']
                assert upper.form=='DW_FORM_exprloc' and upper.value==[0x73,0x7f]
                result['lutri_options']=dict(element_bytes=value(type_of(t),'DW_AT_byte_size'),dynamic_upper_bound_form=upper.form,dynamic_upper_bound_expression=upper.value)
    assert set(result['command_enums'])==set(CHOICES) and set(result['vision_arrays'])==set(ARRAYS) and result['lutri_options']['element_bytes']==4
    assert 'rarity_expansion_prefix' in result
    return result


def reconstruct(a,raw,runtime,meta,storage):
    text=strip_comments(raw);changes=[];pairs=[]
    size=int(meta[a]['high_pc'],16)-int(a,16);block=dict(machine_sites=[],machine_range=[a,meta[a]['high_pc'][2:]],machine_bytes=size,machine_sha256=sha(runtime.pe.get_data(int(a,16)-0x2000000,size)))
    def change(old,new,kind='restore native local storage, field/stack view or typed interface'):
        nonlocal text
        count=text.count(old);assert count>0,(a,old)
        changes.append(dict(original=old,replacement=new,occurrences=count,kind=kind,**block));text=text.replace(old,new)
    if a in CHOICES:
        declaration='  typedef enum { '+', '.join(f'{n} = {v}' for n,v in CHOICES[a].items())+' } CommandChoices;\n'
        change('  CommandChoices CVar1;',declaration+'  CommandChoices CVar1;','restore owning DWARF local four-byte modal enum')
    if a==VERDANT:
        pairs=[('pCVar4 == (CommandChoices *)abils','pCVar4 == choices + 2'),('pCStack_220 != (CommandChoices *)abils','pCStack_220 != choices + 2'),('pCStack_220 == (CommandChoices *)abils','pCStack_220 == choices + 2'),('  int iStack_214;','  int iStack_214;\n  int priority3;'),('pCStack_220 = (CommandChoices *)0x5;','priority3 = 5;'),('pCStack_220 = (CommandChoices *)count_graveyard(iVar3);','priority3 = count_graveyard(iVar3);'),('pCStack_220 = (CommandChoices *)priority[3];','priority3 = priority[3];'),('abils[3],pCStack_220,','abils[3],priority3,')]
    if a==CRYPT_B:
        pairs=[('&td_permanent != (target_definition_t *)pCVar8','choices + 2 != pCVar8'),('(target_definition_t *)pCVar7 != &td_permanent','pCVar7 != choices + 2'),('(target_definition_t *)pCVar8 != &td_permanent','pCVar8 != choices + 2')]
    if a==CRYPT_E:
        pairs=[('&td_spell != (target_definition_t *)pCVar7','choices + 2 != pCVar7'),('&td_spell != (target_definition_t *)pCVar6','choices + 2 != pCVar6')]
    if a==INSCRIPTION:
        for source,dest in [('td_creature','td_dont_control'),('td_dont_control','td_control'),('td_control','td_player')]:
            pattern=r'ptVar6 = &'+source+r';\s*ptVar7 = &'+dest+r';\s*for \(iVar4 = 0x15; iVar4 != 0; iVar4 = iVar4 \+ -1\) \{\s*ptVar7->who_chooses = ptVar6->who_chooses;\s*ptVar6 = \(target_definition_t \*\)\(\(int\)ptVar6 \+ \(uint\)bVar9 \* -8 \+ 4\);\s*ptVar7 = \(target_definition_t \*\)\(\(int\)ptVar7 \+ \(uint\)bVar9 \* -8 \+ 4\);\s*\}'
            found=list(re.finditer(pattern,text));assert len(found)==3
            for old in dict.fromkeys(m[0] for m in found):change(old,dest+' = '+source+';','restore full native 21-word target-definition copies')
        pairs=[('  target_definition_t *ptVar6;\n',''),('  target_definition_t *ptVar7;\n',''),('  byte bVar9;\n',''),('  bVar9 = 0;\n','')]
    if a==PENANCE:pairs=[('*(undefined1 *)&pcVar1->field_26','((unsigned char *)&pcVar1->info_slot)[0]')]
    if a==LUTRI:
        start=text.index('  if ((event == EVENT_QUERY)');end=text.index('  check_legend_rule(',start)
        query='''  if ((event == EVENT_QUERY) && (attacking_card == 2)) {
    int options[available_slots];
    /* Native code clears available_slots bytes, not the whole int array. */
    memset(options, 0, available_slots);
    int *deck = deck_ptr[player];
    for (int i = 0; i != 500; ++i) {
      int iid = deck[i];
      if (iid == -1) return 1;
      if ((cards_data[iid].type & TYPE_LAND) == 0) {
        unsigned int csvid = (uint16_t)cards_data[iid].id;
        if (options[csvid] != 0) return 0;
        options[csvid] = 1;
      }
    }
    return 1;
  }
'''
        change(text[start:end],query,'restore dynamic int options and native partial byte clear, unsigned ID and 500-entry deck bound')
        decl=re.search(r'(?m)^  anon_union_\w+ aVar7;\n',text);assert decl
        pairs=[(decl[0],''),('      aVar7.info_slot._1_3_ = 0;\n      aVar7.info_slot._0_1_ = iVar2 != 0;\n      pcVar4->field_26 = aVar7;','      pcVar4->info_slot = (unsigned int)(iVar2 != 0);')]
    if a==VISION:
        declaration='  /* Native mask-valued indices can exceed these table bounds; retained for downstream review. */\n'
        for name,record in storage['vision_arrays'].items():
            declaration+=f'  static const int {name}[{record["count"]}] = {{ '+', '.join(str(v) for v in record['values'])+' };\n'
            pairs.append(('card_vision_charm::lexical_block_1::lexical_block_1_0::'+name,name))
        change('  target_definition_t td;',declaration+'  target_definition_t td;','restore exact local read-only subtype arrays; retain native mask-valued indexing including out-of-bounds cases')
    if a==FALLEN:pairs=[('  undefined *puVar6;','  const char *puVar6;'),('  undefined *puVar8;','  const char *puVar8;')]
    if a==SLY:pairs=[('  undefined *puVar4;','  const char *puVar4;')]
    if a==SYMBOL:pairs=[('  Rarities *pRVar4;','  const Rarities *pRVar4;'),('expansions._0_4_ = expansions._0_4_ & 0xffff;','expansions[2] = 0;\n        expansions[3] = 0;'),('expansions._4_2_ = expansions._4_2_ & 0xff00;','expansions[4] = 0;'),('&stack0xfffffff0','expansions + 198')]
    if a==CAST:pairs=[('*(int32_t *)(stack_size * 8 + 0x5096a4)','stack_cards[stack_size - 2].card'),('*(int32_t *)(stack_size * 8 + 0x5096a0)','stack_cards[stack_size - 2].player')]
    if a==VALIDATE:pairs=[('*(short *)(stack_size * 4 + 0x50962c)','stack_data[stack_size - 1].internal_card_id')]
    for old,new in pairs:change(old,new)
    if a in {FALLEN,SLY}:
        for old in sorted(set(re.findall(r'&UNK_[0-9a-f]{8}',text))):
            at=int(old[5:],16)
            # Dialog controls are recovered by the ordinary argument-literal pass.
            if readonly_string(runtime,at) not in {b'',b's'}:continue
            literal=readonly_string(runtime,at);change(old,byte_literal(literal),'restore exact singular/plural label pointer')
            changes[-1].update(native_literal=dict(address=f'{at:x}',bytes=(literal+b'\0').hex(),copy_bytes=len(literal)+1),literal=literal.decode())
    return text,changes


def audit_component(component,manifest,sources,machine_bytes,root):
    entries={f['address']:f for f in manifest['accepted_functions'] if f.get('mode_storage_consumer_evidence')};assert set(entries)==set(ADDRESSES)==set(component['functions'])
    runtime=Image((root/'.recovery/2022-ManalinkEh.dll').read_bytes());assert component['runtime_sha256']==sha(runtime.data)=='52793f60124a3b83d8b2728d145e918101a395b4e8d1c0af69cddcfc87ef10e5'
    assert component['native_storage']==native_storage(Image(Path('G:/Manalink/ManalinkEh_10_22_Hotfix.dbg').read_bytes()),runtime)
    if not component.get('planning'):
        assert CONFIG_IMPORT in read(root/'manalink.h')
        assert read(root/'Drawcardlib.def').splitlines().count('  get_config_data')==1
        from recover_display_hooks import previous_import_definition
        definition=previous_import_definition(root,read(root/'Drawcardlib.def'))
        chooser=json.loads((root/'docs/recovery-evidence/interface-recovery.json').read_text()).get('pack_chooser_native_2022')
        if chooser:
            assert chooser['import_definition_append']=='  blt_expansion_symbol\n' and definition.endswith(chooser['import_definition_append'])
            definition=definition.removesuffix(chooser['import_definition_append'])
            assert sha(definition.encode())==chooser['previous_import_definition_sha256']
        assert sha(definition.removesuffix('  get_config_data\n').encode())==component['previous_import_definition_sha256']
    from unify_scalar_helpers import retained_source_body
    for old in component['retained_bodies']:assert sha(retained_source_body(root,old['source'],old['name']).encode())==old['body_sha256']
    for layout in component['unit_layouts'].values():assert layout['test_definition_t']['bytes']==216 and layout['target_definition_t']['bytes']==84
    def local(a,n):return next(v for v in component['locals'][a] if v['name']==n)
    for a,end in [(VERDANT,'abils'),(CRYPT_B,'td_permanent'),(CRYPT_E,'td_spell')]:
        start=local(a,'choices');assert start['type']=='CommandChoices[2]'
        off=lambda l:int(re.fullmatch(r'\(DW_OP_fbreg: (-?\d+)\)',l['location'])[1])
        assert off(local(a,end))-off(start)==8
    assert local(SYMBOL,'expansions')['type']=='char[198]'
    overlap=[s.split('\t')[1] for s in (root/'.recovery/absolute-interfaces.tsv').read_text().splitlines() if s.split('\t')[2]=='global'];meta={r['low_pc'][2:]:r for r in csv.DictReader((root/'.recovery/2022-functions.csv').open()) if r['low_pc'] and r['high_pc']};decoder=Cs(CS_ARCH_X86,CS_MODE_32);decoder.detail=True
    for a,entry in entries.items():
        detail=component['functions'][a];assert entry['mode_storage_consumer_evidence']==detail and not entry['static']
        body=function_body(sources[entry['source']].replace('\r\n','\n'),entry['name']);assert sha(body.encode())==entry['reconstruction_body_sha256']
        raw=read(root/'.recovery'/('decompiled-mode-storage' if component.get('planning') else 'decompiled-abi')/f'{a}.c');assert sha(raw.encode())==entry['raw_sha256']
        text,changes=reconstruct(a,raw,runtime,meta,component['native_storage']);assert changes==detail['changes'] and target_validation_input(strip_comments(raw),detail,a,machine_bytes)==text
        text,literals=recover_strings(text,runtime,ALIASES);assert literals==entry['recovered_literals'];text,_=rename(text,NAMES);assert function_body(cleaned(text,overlap),entry['name'])==body
        code=machine_bytes(int(a,16),detail['machine_bytes']);assert sha(code)==detail['machine_sha256'] and detail['native_coff_storage']==2
        insns={f'{i.address:x}':i for i in decoder.disasm(code,int(a,16))}
        for site in detail['canonical_calls']+detail['callback_references']:
            ins=insns[site['address']];assert ins.bytes.hex()==site['bytes'] and any(o.type==X86_OP_IMM and o.imm==int(site['target'],16) for o in ins.operands);assert re.search(r'\b'+site['name']+r'\b',body)
        if a in HELPERS:assert entry['prototype'] in read(root/'manalink.h') or component.get('planning')
    assert len(entries)==14 and sum(e['name'].startswith('card_') for e in entries.values())==10


def main():
    parser=argparse.ArgumentParser(__doc__);parser.add_argument('--apply',action='store_true');parser.add_argument('--prepare',action='store_true');args=parser.parse_args()
    root=Path(__file__).resolve().parents[2];cache=root/'.recovery';mp=root/'docs/recovery-evidence/recovered-functions.json';ip=root/'docs/recovery-evidence/interface-recovery.json';m=json.loads(mp.read_text());interfaces=json.loads(ip.read_text());meta={r['low_pc'][2:]:r for r in csv.DictReader((cache/'2022-functions.csv').open()) if r['low_pc'] and r['high_pc']};debug=Image(Path('G:/Manalink/ManalinkEh_10_22_Hotfix.dbg').read_bytes());runtime=Image((cache/'2022-ManalinkEh.dll').read_bytes());get=lambda a,n:runtime.pe.get_data(a-0x2000000,n)
    if args.prepare:
        native_storage(debug,runtime);(cache/'export-request-mode-storage.tsv').write_text(''.join('\t'.join([a,meta[a]['source'],meta[a]['name'],meta[a]['high_pc'][2:]])+'\n' for a in ADDRESSES));print('Prepared fourteen mode/storage consumers and counter helpers.');return
    assert 'mode_storage_consumers_native_2022' not in interfaces and not set(ADDRESSES)&{f['address'] for f in m['accepted_functions']}
    proof=dict(previous_import_definition_sha256=sha(read(root/'Drawcardlib.def').encode()),runtime_sha256=sha(runtime.data),debug_sha256=sha(debug.data),native_storage=native_storage(debug,runtime),**debug_evidence(debug,meta,ADDRESSES),unit_layouts=native_layouts(debug,{meta[a]['source'] for a in ADDRESSES}),functions={},retained_bodies=[dict(source=s,name=n,body_sha256=sha(function_body(read(root/s),n).encode())) for s,n in LEGACY],
        scope='Recover ten modal/packed-state card handlers and four native counter-activated-ability helpers in their original source units. Restore local enum declarations, two-choice endpoints, full 84-byte target copies, typed stack and byte views, local subtype tables, partial-cleared Lutri VLA, expansion bitmap bounds and exact label pointers. Preserve original native edge behavior and legacy helper bodies; earlier callers and gameplay integration remain downstream.')
    targets={int(f['address'],16):f for f in m['accepted_functions']};targets.update({int(a,16):dict(name=NAMES.get(meta[a]['name'],meta[a]['name']),source=meta[a]['source'],static=a in PRIVATE) for a in ADDRESSES});decoder=Cs(CS_ARCH_X86,CS_MODE_32);decoder.detail=True;overlap=[s.split('\t')[1] for s in (cache/'absolute-interfaces.tsv').read_text().splitlines() if s.split('\t')[2]=='global'];sources={};entries=[]
    for a in ADDRESSES:
        row=meta[a];source=row['source'];name=targets[int(a,16)]['name'];raw=read(cache/'decompiled-mode-storage'/f'{a}.c');text,changes=reconstruct(a,raw,runtime,meta,proof['native_storage']);text,literals=recover_strings(text,runtime,ALIASES);text,_=rename(text,NAMES);body=cleaned(text,overlap)
        if a in PRIVATE:body='static '+body
        assert not UNRESOLVED.search(strip_comments(body)) and not re.search(r'stack0x|\._\d|_conflict|::',body),a
        storage=[s for n,v,sec,k,s in debug.functions() if sec>0 and debug.pe.OPTIONAL_HEADER.ImageBase+debug.pe.sections[sec-1].VirtualAddress+v==int(a,16)];assert storage==[3 if a in PRIVATE else 2]
        code=get(int(a,16),int(row['high_pc'],16)-int(a,16));detail=dict(machine_bytes=len(code),machine_sha256=sha(code),changes=changes,canonical_calls=[],callback_references=[],native_coff_storage=storage[0])
        for ins in decoder.disasm(code,int(a,16)):
            for op in ins.operands:
                if op.type!=X86_OP_IMM or op.imm not in targets:continue
                target=targets[op.imm];assert re.search(r'\b'+target['name']+r'\b',body),(a,target['name']);assert not target['static'] or target['source']==source
                site=dict(address=f'{ins.address:x}',bytes=ins.bytes.hex(),target=f'{op.imm:x}',name=target['name']);(detail['canonical_calls'] if ins.mnemonic in {'call','jmp'} else detail['callback_references']).append(site)
        proof['functions'][a]=detail;entries.append(dict(address=a,name=name,source=source,line=row['line'],static=a in PRIVATE,prototype=body[:body.index('{')].strip()+';',raw_sha256=sha(raw.encode()),recovered_literals=literals,mode_storage_consumer_evidence=detail,reconstruction=proof['scope'],reconstruction_body_sha256=sha(function_body(body,name).encode())));sources[source]=sources.get(source,read(root/source))+f'\n\n#ifndef SHANDALAR\n/* ManalinkEh 2022: 0x{a}; original {source}:{row["line"]}. */\n'+body+'\n#endif\n'
    header=read(root/'manalink.h');at=header.rfind('#ifdef __cplusplus');header=header[:at]+LABEL+'\n#ifndef SHANDALAR\n'+'\n'.join(e['prototype'] for e in entries if e['address'] in HELPERS-PRIVATE)+'\n'+CONFIG_IMPORT+'\n#endif\n\n'+header[at:];compile_sources(root,cache/'mode-storage-check',sources,header)
    proof['planning']=True;audit_component(proof,{**m,'accepted_functions':m['accepted_functions']+entries},sources,get,root);del proof['planning']
    if args.apply:
        citations=m['accepted_functions']+m['source_fragments']+[s for f in m['accepted_functions'] for s in f.get('supporting_exports',[])];assert not set(ADDRESSES)&{f['address'] for f in citations};rows={r['address']:r for r in csv.DictReader((cache/'decompiled-abi/export.tsv').open(),delimiter='\t')};before=len(rows)
        for row in csv.DictReader((cache/'decompiled-mode-storage/export.tsv').open(),delimiter='\t'):rows[row['address']]=row;shutil.copyfile(cache/'decompiled-mode-storage'/f'{row["address"]}.c',cache/'decompiled-abi'/f'{row["address"]}.c')
        proof['new_canonical_raw_entries']=len(rows)-before
        with (cache/'decompiled-abi/export.tsv').open('w',newline='') as stream:
            writer=csv.DictWriter(stream,fieldnames=['address','source','name','status','prototype'],delimiter='\t');writer.writeheader();writer.writerows(rows.values())
        for source,text in sources.items():(root/source).write_bytes(text.replace('\n','\r\n').encode('latin1'))
        (root/'Drawcardlib.def').write_bytes((read(root/'Drawcardlib.def')+'  get_config_data\n').replace('\n','\r\n').encode())
        (root/'manalink.h').write_bytes(header.replace('\n','\r\n').encode('latin1'));m['accepted_functions']+=entries;m['accepted']+=14;m['card_handler_functions']+=10;m['helper_functions']+=4;m['shared_helper_interfaces']+=4
        for e in entries:m['source_files'][e['source']]=m['source_files'].get(e['source'],0)+1
        m['raw_export_entries']=len(rows);m['exported_functions']=len({r['name'] for r in rows.values()});save_json(mp,m);interfaces['mode_storage_consumers_native_2022']=proof;save_json(ip,interfaces)
    save_json(cache/'mode-storage-plan.json',proof);print(json.dumps(dict(applied=args.apply,new_cards=10,new_helpers=4)))


if __name__=='__main__':main()
