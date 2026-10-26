"""Recover the October 2022 full-card rendering, text helpers and window hooks."""
import argparse,csv,json,re
from pathlib import Path
from capstone import Cs,CS_ARCH_X86,CS_MODE_32
from capstone.x86_const import X86_OP_IMM,X86_OP_MEM,X86_OP_REG
from inspect_release import Image,value,inherited_value
from export_abi import type_of
from pack_native_tables import unqual
from promote_recovery import strip_comments,UNRESOLVED
from recover_test_consumers import read
from recover_combat_mask_cards import cleaned
from recover_literals import readonly_string,byte_literal
from recover_spell_copy import compile_sources
from recover_phase_prompt_cards import debug_evidence
from recover_temporary_exile import save_json,sha
from audit_source import function_body,target_validation_input
from display_hook_bodies import BODIES

COPY='2657690';LEGACY='26590a0';KEYWORDS='265b7c0';WORKER='265bca0';DRAW='265c8b0';LIST='26577b0';FULL='265c8f0'
ADDRESSES=[COPY,LEGACY,KEYWORDS,WORKER,DRAW,LIST,FULL];PRIVATE={WORKER};PUBLIC_HELPERS=[COPY,LEGACY,KEYWORDS];WINDOW_HELPERS=[DRAW,LIST,FULL]
NAMES={COPY:'memcpy_displayed_inst_and_ext',LEGACY:'legacy_name_or_text',KEYWORDS:'keywords_to_str',WORKER:'draw_fullcard_special_effect_card_impl_2022',DRAW:'draw_fullcard_special_effect_card',LIST:'wndproc_ShowListCard_hook',FULL:'wndproc_FullCardClass_hook'}
LABEL='/* October 2022 displayed-instance copy and legacy/keyword text helpers. */'
TYPES='typedef struct inst_and_ext_t { card_instance_t inst; card_ext_t ext; } inst_and_ext_t;'
IMPORT_APPEND='  DrawSmallCardEx\n  DrawFullCard\n  DrawFullCardEx\n'
PRIVATE_TYPES='''#include <ctype.h>
static char *kws_strs[32], *spkws_strs[32];
#define display_dblbfr_hdc_2022 EXE_TYP(HDC, 0x62047c)
#define display_critical_section_for_painting_2022 EXE_TYP_PTR(CRITICAL_SECTION, 0x790500)
#define display_last_hover_hwnd_2022 EXE_TYP(HWND, 0x56bec8)
#define display_tick_adjustment_2022 EXE_DWORD(0x62c370)
#define display_draw_numeral_2022 EXE_FN(void, 0x4d34e0, HDC, RECT *, int)
#define display_get_infoslot_2022 EXE_FN(int, 0x4389a0, int, int)
#define display_get_counters_2022 EXE_FN(void, 0x438e80, int, int, int *, int *)
#define display_get_color_2022 EXE_FN(color_test_t, 0x438dc0, int, int)
#define display_draw_full_activation_2022 EXE_FN(void, 0x4d1950, HDC, RECT *, int, int)
#define display_draw_full_instance_2022 EXE_FN(int, 0x4d1780, HDC, RECT *, const card_ptr_t *, int, int, int, int)
#define display_sleight_text_2022 EXE_FN(void, 0x439010, int, int, char *)
#define display_hack_text_2022 EXE_FN(void, 0x439070, int, int, char *)
#define display_replace_text_2022 EXE_FN(void, 0x466e90, char *, const char *, int, const char *)
DLLIMPORT int DrawSmallCardEx(HDC hdc, const RECT *rect, const card_ptr_t *cp, int version, int mode, int player, int card, int expansion);
DLLIMPORT int DrawFullCard(HDC hdc, const RECT *rect, const card_ptr_t *cp, int version, int style, int expanded_text, const char *illustrator);
DLLIMPORT int DrawFullCardEx(HDC hdc, const RECT *rect, const card_ptr_t *cp, int version, int style, int expanded_text, const char *illustrator, int expansion);
void effect_card_text_replace_pipe_n(char *buf, int pow, int tgh);
'''
KEYWORD_MAP=[10,11,12,13,14,0,2,3,5,7,1,15,16,17,18,19,20,39,None,None,None,21,None,22,None,None,None,None,None,40,6,None]
SPECIAL_MAP=[27,30,37,4,24,23,32,None,25,29,None,36,None,31,34,33,49,48,None,None,None,None,None,38,None,28,None,None,None,66,26,None]
LITERALS={0x275d530:b'functions/windows.c:2241\nevent == EVENT_SET_LEGACY_EFFECT_NAME || event == EVENT_SET_LEGACY_EFFECT_TEXT',
0x275d598:b'',0x275d599:b'%s: %d',0x275d5a0:b' (%s)',0x275d5c4:b'|n',0x275d5f6:b'%d',0x275d624:b'%c%s and %c%s',0x275d639:b'and %c%s',0x275d642:b'Damage',0x275d649:b'Effect',0x275d650:b'Blocker',0x275d658:b'%s\n',0x275d65c:b'|S',0x275d65f:b'|S1',0x275d663:b'|S2',0x275d667:b'|S3',0x275d66b:b'|S4',0x275d66f:b'|H',0x275d672:b'|H1',0x275d676:b'|H2',0x275d67a:b'|H3',0x275d67e:b'|H4',0x275d682:b'|a',0x275d685:b'undying',0x275d68d:b'|e',0x275d690:b'|e0',0x275d694:b'|N',0x275d697:b'|e1',0x275d69b:b'|e2',0x275d69f:b'|e3'}
EXE_TARGETS=[0x4d34e0,0x4389a0,0x438e80,0x438dc0,0x4d1950,0x4d1780,0x439010,0x439070,0x466e90,0x46d9a0,0x4cff20,0x438b00,0x466050]


def native_storage(runtime,debug,meta,root):
    arrays=[];types={};statics={};scalars={}
    for cu in debug.dwarf().iter_CUs():
        if value(cu.get_top_DIE(),'DW_AT_name')!='functions/windows.c':continue
        for d in cu.iter_DIEs():
            n=inherited_value(d,'DW_AT_name')
            if d.tag=='DW_TAG_typedef' and n in {'inst_and_ext_t','card_ext_t','card_instance_t','card_ptr_t','card_txt_t','RECT','PAINTSTRUCT','CRITICAL_SECTION'}:
                t=unqual(type_of(d));types[n]=dict(bytes=value(t,'DW_AT_byte_size'),members={value(c,'DW_AT_name'):value(c,'DW_AT_data_member_location') for c in t.iter_children() if c.tag=='DW_TAG_member' and value(c,'DW_AT_name') is not None})
            if d.tag!='DW_TAG_variable' or n is None:continue
            t=unqual(type_of(d));parent=d.get_parent()
            while parent is not None and parent.tag!='DW_TAG_subprogram':parent=parent.get_parent()
            owner=inherited_value(parent,'DW_AT_name') if parent is not None else None
            loc=value(d,'DW_AT_location')
            if n in {'kws_strs','spkws_strs','hack_force_expansion'} and isinstance(loc,list) and len(loc)==5 and loc[0]==3:
                addr=int.from_bytes(bytes(loc[1:]),'little');section=next(s for s in runtime.pe.sections if s.VirtualAddress<=addr-0x2000000<s.VirtualAddress+s.Misc_VirtualSize)
                assert section.Characteristics&0x80
                statics[n]=dict(address=f'{addr:x}',bytes=value(t,'DW_AT_byte_size'),section=section.Name.rstrip(b'\0').decode(),location=loc)
            if parent is None or value(parent,'DW_AT_low_pc') not in {int(a,16) for a in ADDRESSES} or loc is None:continue
            if t.tag=='DW_TAG_array_type':
                arrays.append(dict(owner=owner,name=n,element_bytes=value(unqual(type_of(t)),'DW_AT_byte_size'),bounds=[value(c,'DW_AT_upper_bound') for c in t.iter_children() if c.tag=='DW_TAG_subrange_type'],location=loc))
            elif n in {'emptystr','counter_power','counter_toughness','paint','rect','cp','inst'}:
                scalars[owner+'::'+n]=dict(bytes=value(t,'DW_AT_byte_size'),location=loc)
    shapes={x['owner']+'::'+x['name']:(x['element_bytes'],x['bounds']) for x in arrays}
    assert shapes=={'keywords_to_str::strs':(4,[64]),'legacy_name_or_text::tmp':(1,[1999]),'draw_fullcard_special_effect_card::card_name':(1,[99]),'draw_fullcard_special_effect_card::rules_text':(1,[1999]),'draw_fullcard_special_effect_card::kwstr':(1,[999]),'draw_fullcard_special_effect_card::et':(1,[14]),'draw_fullcard_special_effect_card::pt':(1,[14])},shapes
    assert types['inst_and_ext_t']==dict(bytes=492,members=dict(inst=0,ext=300))
    assert {n:types[n]['bytes'] for n in ['card_ext_t','card_instance_t','card_ptr_t','card_txt_t','RECT','PAINTSTRUCT','CRITICAL_SECTION']}==dict(card_ext_t=192,card_instance_t=300,card_ptr_t=152,card_txt_t=20,RECT=16,PAINTSTRUCT=64,CRITICAL_SECTION=24)
    assert scalars['draw_fullcard_special_effect_card::emptystr']['bytes']==1
    assert {n:x['address'] for n,x in statics.items()}==dict(kws_strs='2774640',spkws_strs='27745c0',hack_force_expansion='2774484')
    get=lambda a,n:runtime.pe.get_data(a-0x2000000,n)
    anchors={
        'extension_copy_count':('26576f4','bac0000000'),'instance_copy_count':('265771e','c74424082c010000'),
        'extension_rebind_after_unlock':('265773d','8b45f089432c'),
        'legacy_flags':('26590d4','8b504c88d385f2'),'legacy_callback':('26591a7','ff5038'),
        'keyword_lowest_bit':('265b7ee','0fbcc2'),'keyword_signed_tolower':('265bc10','0fbe46ff'),
        'worker_zero_record':('265bcba','b92600000031c0'),'worker_instance_copy':('265bcf7','c74424082c010000'),
        'text_e2_gates_e3':('265c0fe','0f84a3000000'),'text_e3_no_null_guard':('265c160','c7442404f6d57502'),
        'text_e_replaces_N':('265c195','c744240494d67502'),'empty_string_write':('265c2df','c6853cf4ffff00'),
        'list_stdcall_return':('2657963','c21000'),'full_stdcall_return':('265c939','c21000'),
        'full_clear_force_bit':('265cb56','81e2ffffffbf'),'full_zero_forward_lparam':('265cb65','c7451400000000')}
    for n,(a,b) in anchors.items():assert get(int(a,16),len(bytes.fromhex(b))).hex()==b,n
    decoder=Cs(CS_ARCH_X86,CS_MODE_32);decoder.detail=True
    matrix={};origin=None
    for i in decoder.disasm(get(0x265b978,0x280),0x265b978):
        if i.mnemonic!='mov' or len(i.operands)!=2:continue
        dst,src=i.operands
        if dst.type==X86_OP_REG and i.reg_name(dst.reg)=='eax':
            origin=(src.mem.disp-0x27746c0)//4 if src.type==X86_OP_MEM and src.mem.base==src.mem.index==0 and 0x27746c0<=src.mem.disp<0x27747f4 else None
        if dst.type==X86_OP_MEM and dst.mem.base==dst.mem.index==0 and 0x27745c0<=dst.mem.disp<0x27746c0:
            if src.type==X86_OP_IMM:assert src.imm==0x275d598;index=None
            else:assert src.type==X86_OP_REG and i.reg_name(src.reg)=='eax' and origin is not None;index=origin
            matrix[dst.mem.disp]=index
    assert len(matrix)==64
    assert [matrix[0x2774640+4*i] for i in range(32)]==KEYWORD_MAP
    assert [matrix[0x27745c0+4*i] for i in range(32)]==SPECIAL_MAP
    for a,s in LITERALS.items():assert readonly_string(runtime,a)==s,(hex(a),readonly_string(runtime,a),s)
    runtime.pe.parse_data_directories();imports={s.address:dict(dll=d.dll.decode(),name=s.name.decode()) for d in runtime.pe.DIRECTORY_ENTRY_IMPORT for s in d.imports if s.name}
    assert {imports[a]['name'] for a in [0x27b5350,0x27b5354,0x27b5358]}=={'DrawFullCard','DrawFullCardEx','DrawSmallCardEx'}
    refs={};symbols={}
    for a in ADDRESSES:
        code=get(int(a,16),int(meta[a]['high_pc'],16)-int(a,16));used=[]
        for i in decoder.disasm(code,int(a,16)):
            for o in i.operands:
                if o.type==X86_OP_MEM and o.mem.base==o.mem.index==0 and o.mem.disp in imports:used.append(dict(address=f'{i.address:x}',bytes=i.bytes.hex(),iat=f'{o.mem.disp:x}',**imports[o.mem.disp]))
        refs[a]=used
        ss=[dict(name=n,storage=st) for n,v,sec,k,st in debug.functions() if sec>0 and k==32 and debug.pe.OPTIONAL_HEADER.ImageBase+debug.pe.sections[sec-1].VirtualAddress+v==int(a,16)]
        assert len(ss)==1 and ss[0]['storage']==(3 if a in PRIVATE else 2);symbols[a]=ss[0]
    exe=Image((root/'.recovery/2022-Magic.exe').read_bytes());assert sha(exe.data)=='0170f02b1c381a20a97ca0c12e99c8d338453ce4edbfc99148e726d565bee0de'
    return dict(arrays=arrays,types=types,statics=statics,scalars=scalars,keyword_mapping=KEYWORD_MAP,special_keyword_mapping=SPECIAL_MAP,imports=refs,native_symbols=symbols,
        exe_sha256=sha(exe.data),exe_boundaries={f'{a:x}':exe.pe.get_data(a-0x400000,48).hex() for a in EXE_TARGETS},
        literals={f'{a:x}':dict(bytes=(s+b'\0').hex(),literal=byte_literal(s)) for a,s in LITERALS.items()},anchors={n:dict(address=a,bytes=b) for n,(a,b) in anchors.items()},
        behavior=['Copy the 192-byte extension before the 300-byte instance under the display lock; rebind the copied pointer after unlocking.',
        'Preserve the low-byte alternate selector, callback flags, 51/999 name/text terminators, both 32-entry lazy keyword maps, signed-byte lowercasing and original separators.',
        'Restore the distinct 100/2000/1000/15/15-byte display arrays and separate one-byte empty string despite shared native stack slots.',
        'Retain native |e substitutions searching |e0/1/2/3 but passing |N to replacement; |e2 gates an unchecked |e3 replacement. Preserve the original order and source-prefix pointers.',
        'Both window procedures use stdcall and forward their four parameters to stdcall EXE procedures. Forced expansion clears the marker bit and forwarded lparam; paint locking, redraw checks, fallback styles, tick adjustment and conditional EndPaint remain native.'])


def previous_import_definition(root,definition):
    """Peel this append while preserving the prior import-definition hash chain."""
    component=json.loads((root/'docs/recovery-evidence/interface-recovery.json').read_text()).get('display_hooks_native_2022')
    if component is None:return definition
    assert component['import_definition_append']==IMPORT_APPEND and definition.endswith(IMPORT_APPEND)
    previous=definition.removesuffix(IMPORT_APPEND)
    assert sha(previous.encode())==component['previous_import_definition_sha256']
    return previous


def reconstruct(a,raw,runtime,meta):
    original=strip_comments(raw);body=BODIES[a].strip()+'\n';size=int(meta[a]['high_pc'],16)-int(a,16)
    return body,[dict(original=original,replacement=body,occurrences=1,kind='restore typed card/extension copies, real scratch arrays and legacy/keyword formatting, private renderer part, stdcall window hooks and exact native import/EXE frames',machine_sites=[],machine_range=[a,meta[a]['high_pc'][2:]],machine_bytes=size,machine_sha256=sha(runtime.pe.get_data(int(a,16)-0x2000000,size)))]


def audit_component(component,manifest,sources,machine_bytes,root):
    entries={f['address']:f for f in manifest['accepted_functions'] if f.get('display_hook_evidence')};assert set(entries)==set(ADDRESSES)==set(component['functions'])
    runtime=Image((root/'.recovery/2022-ManalinkEh.dll').read_bytes());assert component['runtime_sha256']==sha(runtime.data)=='52793f60124a3b83d8b2728d145e918101a395b4e8d1c0af69cddcfc87ef10e5'
    debug=Image(Path('G:/Manalink/ManalinkEh_10_22_Hotfix.dbg').read_bytes());assert component['debug_sha256']==sha(debug.data)
    for old in component['retained_helpers']:assert sha(function_body(read(root/old['source']),old['name']).encode())==old['body_sha256']
    meta={r['low_pc'][2:]:r for r in csv.DictReader((root/'.recovery/2022-functions.csv').open()) if r['low_pc'] and r['high_pc']};assert component['native_storage']==native_storage(runtime,debug,meta,root)
    overlap=[s.split('\t')[1] for s in (root/'.recovery/absolute-interfaces.tsv').read_text().splitlines() if s.split('\t')[2]=='global'];decoder=Cs(CS_ARCH_X86,CS_MODE_32);decoder.detail=True
    for a,e in entries.items():
        detail=component['functions'][a];assert e['display_hook_evidence']==detail and e['static']==(a in PRIVATE)
        body=function_body(sources[e['source']].replace('\r\n','\n'),e['name']);assert sha(body.encode())==e['reconstruction_body_sha256']
        raw=read(root/'.recovery/decompiled-abi'/f'{a}.c');assert sha(raw.encode())==e['raw_sha256']
        text,changes=reconstruct(a,raw,runtime,meta);assert changes==detail['changes'] and target_validation_input(strip_comments(raw),detail,a,machine_bytes)==text
        assert function_body(cleaned(text,overlap),e['name'])==body
        code=machine_bytes(int(a,16),detail['machine_bytes']);assert sha(code)==detail['machine_sha256'] and detail['native_coff_storage']==(3 if a in PRIVATE else 2)
        insns={f'{i.address:x}':i for i in decoder.disasm(code,int(a,16))}
        for site in detail['canonical_calls']+detail['callback_references']:
            ins=insns[site['address']];assert ins.bytes.hex()==site['bytes'] and any(o.type==X86_OP_IMM and o.imm==int(site['target'],16) for o in ins.operands);assert re.search(r'\b'+site['name']+r'\b',body)
    source=sources['functions/windows.c'].replace('\r\n','\n');assert PRIVATE_TYPES in source and component['private_declarations']==PRIVATE_TYPES
    header=read(root/'manalink.h') if not component.get('planning') else component['planned_header'];assert TYPES in header
    for a in PUBLIC_HELPERS:assert entries[a]['prototype'] in header
    for a in WINDOW_HELPERS:assert entries[a]['prototype'] in source
    assert component['import_definition_append']==IMPORT_APPEND
    if not component.get('planning'):previous_import_definition(root,read(root/'Drawcardlib.def'))
    assert 'CALLBACK' in entries[LIST]['prototype'] and 'CALLBACK' in entries[FULL]['prototype']


def main():
    parser=argparse.ArgumentParser(__doc__);parser.add_argument('--apply',action='store_true');parser.add_argument('--prepare',action='store_true');args=parser.parse_args()
    root=Path(__file__).resolve().parents[2];cache=root/'.recovery';mp=root/'docs/recovery-evidence/recovered-functions.json';ip=root/'docs/recovery-evidence/interface-recovery.json';m=json.loads(mp.read_text());interfaces=json.loads(ip.read_text());meta={r['low_pc'][2:]:r for r in csv.DictReader((cache/'2022-functions.csv').open()) if r['low_pc'] and r['high_pc']};debug=Image(Path('G:/Manalink/ManalinkEh_10_22_Hotfix.dbg').read_bytes());runtime=Image((cache/'2022-ManalinkEh.dll').read_bytes());get=lambda a,n:runtime.pe.get_data(a-0x2000000,n)
    if args.prepare:
        native_storage(runtime,debug,meta,root);print('Prepared seven display helpers, including one private renderer part and two stdcall window hooks.');return
    assert 'display_hooks_native_2022' not in interfaces and not set(ADDRESSES)&{f['address'] for f in m['accepted_functions']}
    retained=[('functions/windows.c',n) for n in ['initialize_ability_tooltip_names','draw_smallcard_special_effect_card','copy_to_display_start','copy_to_display_end']]+[('functions/functions.c','effect_card_text_replace_pipe_n'),('functions/utility.cpp','sort_ArrayConstCharPtr')]
    proof=dict(previous_import_definition_sha256=sha(read(root/'Drawcardlib.def').encode()),import_definition_append=IMPORT_APPEND,runtime_sha256=sha(runtime.data),debug_sha256=sha(debug.data),native_storage=native_storage(runtime,debug,meta,root),**debug_evidence(debug,meta,ADDRESSES),functions={},private_declarations=PRIVATE_TYPES,
        retained_helpers=[dict(name=n,source=s,body_sha256=sha(function_body(read(root/s),n).encode())) for s,n in retained],
        scope='Recover seven card-display functions in functions/windows.c, including one private renderer compiler part, both stdcall window procedures, instance/extension copy and legacy/keyword formatting. Preserve typed EXE/import boundaries, all seven local arrays, 492-byte composite record, keyword maps and native text/selection/paint quirks. Existing display providers and deferred Wish dialog stubs remain unchanged; downstream dispatch/ABI/gameplay integration is separate work.')
    targets={int(f['address'],16):f for f in m['accepted_functions']};targets.update({int(a,16):dict(name=NAMES[a],source=meta[a]['source'],static=a in PRIVATE) for a in ADDRESSES});decoder=Cs(CS_ARCH_X86,CS_MODE_32);decoder.detail=True;overlap=[s.split('\t')[1] for s in (cache/'absolute-interfaces.tsv').read_text().splitlines() if s.split('\t')[2]=='global'];source=read(root/'functions/windows.c')+'\n\n#ifndef SHANDALAR\n'+PRIVATE_TYPES+'\n#endif\n';entries=[]
    for a in ADDRESSES:
        row=meta[a];name=NAMES[a];raw=read(cache/'decompiled-abi'/f'{a}.c');text,changes=reconstruct(a,raw,runtime,meta);body=cleaned(text,overlap)
        if a in PRIVATE:body='static '+body
        assert not UNRESOLVED.search(strip_comments(body)) and not re.search(r'stack0x|\._\d|_conflict|::',body),a
        code=get(int(a,16),int(row['high_pc'],16)-int(a,16));detail=dict(machine_bytes=len(code),machine_sha256=sha(code),changes=changes,canonical_calls=[],callback_references=[],native_coff_storage=3 if a in PRIVATE else 2)
        for ins in decoder.disasm(code,int(a,16)):
            for op in ins.operands:
                if op.type!=X86_OP_IMM or op.imm not in targets:continue
                target=targets[op.imm];assert re.search(r'\b'+target['name']+r'\b',body),(a,target['name']);assert not target['static'] or target['source']==row['source']
                site=dict(address=f'{ins.address:x}',bytes=ins.bytes.hex(),target=f'{op.imm:x}',name=target['name']);(detail['canonical_calls'] if ins.mnemonic in {'call','jmp'} else detail['callback_references']).append(site)
        proof['functions'][a]=detail;entries.append(dict(address=a,name=name,source=row['source'],line=row['line'],static=a in PRIVATE,prototype=body[:body.index('{')].strip()+';',raw_sha256=sha(raw.encode()),recovered_literals=[],display_hook_evidence=detail,reconstruction=proof['scope'],reconstruction_body_sha256=sha(function_body(body,name).encode())))
        source+=f'\n\n#ifndef SHANDALAR\n/* ManalinkEh 2022: 0x{a}; original {row["source"]}:{row["line"]}. */\n'+body+'\n#endif\n'
    declarations='\n'.join(e['prototype'] for e in entries if e['address'] in WINDOW_HELPERS)+'\n'
    source=source.replace(PRIVATE_TYPES,PRIVATE_TYPES+declarations,1);proof['window_declarations']=declarations
    header=read(root/'manalink.h');at=header.rfind('#ifdef __cplusplus');header=header[:at]+LABEL+'\n#ifndef SHANDALAR\n'+TYPES+'\n'+'\n'.join(e['prototype'] for e in entries if e['address'] in PUBLIC_HELPERS)+'\n#endif\n\n'+header[at:]
    compile_sources(root,cache/'display-hooks-check',{'functions/windows.c':source},header)
    proof['planning']=True;proof['planned_header']=header;audit_component(proof,{**m,'accepted_functions':m['accepted_functions']+entries},{'functions/windows.c':source},get,root);del proof['planning'];del proof['planned_header']
    if args.apply:
        citations=m['accepted_functions']+m['source_fragments']+[s for f in m['accepted_functions'] for s in f.get('supporting_exports',[])];assert not set(ADDRESSES)&{f['address'] for f in citations}
        rows=list(csv.DictReader((cache/'decompiled-abi/export.tsv').open(),delimiter='\t'));assert set(ADDRESSES)<={r['address'] for r in rows};proof['new_canonical_raw_entries']=0
        (root/'functions/windows.c').write_bytes(source.replace('\n','\r\n').encode('latin1'))
        m['accepted_functions']+=entries;m['accepted']+=7;m['helper_functions']+=7;m['shared_helper_interfaces']+=6;m['private_functions']+=1;m['private_helpers']+=1
        for e in entries:m['source_files'][e['source']]=m['source_files'].get(e['source'],0)+1
        save_json(mp,m);(root/'manalink.h').write_bytes(header.replace('\n','\r\n').encode('latin1'));(root/'Drawcardlib.def').write_bytes((read(root/'Drawcardlib.def')+IMPORT_APPEND).replace('\n','\r\n').encode('latin1'));interfaces['display_hooks_native_2022']=proof;save_json(ip,interfaces)
    save_json(cache/'display-hooks-plan.json',proof);print(json.dumps(dict(applied=args.apply,new_cards=0,new_helpers=7)))


if __name__=='__main__':main()
