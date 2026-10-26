"""Recover the native pack chooser/template and Booster Tutor/Stocking Tiger."""
import argparse,csv,json,re,shutil,struct
from pathlib import Path
from capstone import Cs,CS_ARCH_X86,CS_MODE_32
from capstone.x86_const import X86_OP_IMM,X86_OP_MEM
from inspect_release import Image,value,inherited_value
from export_abi import type_of
from promote_recovery import strip_comments,UNRESOLVED
from recover_test_consumers import read
from recover_combat_mask_cards import cleaned
from recover_literals import readonly_string,byte_literal
from recover_spell_copy import compile_sources
from recover_phase_prompt_cards import debug_evidence
from recover_leaves_test_native import native_layouts
from recover_temporary_exile import save_json,sha
from audit_source import function_body,target_validation_input
from pack_chooser_bodies import BODIES

DIALOG='26ecc50';TEMPLATE='26ed780';CHOOSE='26ed790';TUTOR='2305250';TIGER='2411220'
ADDRESSES=[DIALOG,TEMPLATE,CHOOSE,TUTOR,TIGER];PRIVATE={DIALOG};HELPERS={DIALOG,TEMPLATE,CHOOSE}
LABEL='/* October 2022 native pack selection flags and chooser. */'
IMPORT_APPEND='  blt_expansion_symbol\n'
FLAGS='typedef int pack_flags_t;\nenum { PACK_0 = 0, PACK_ALLOW_BOOSTER = 1, PACK_ALLOW_STARTER = 2, PACK_ALLOW_CANCEL = 4 };'
PRIVATE_TYPES='''#include <windows.h>
typedef struct pack_selection_t { EXPANSION_t expansion; int booster; } pack_selection_t;
typedef struct choose_a_pack_args_t {
  int num_packs, allow_boosters, allow_starters, allow_cancel;
  pack_selection_t results[6];
} choose_a_pack_args_t;
#define pack_path_duel_art EXE_STR(0x739e30)
#define pack_str_ok EXE_STR(0x78f0a8)
#define pack_str_cancel EXE_STR(0x7281a4)
#define pack_main_hinstance EXE_TYP(HINSTANCE, 0x78f0c8)
#define pack_hwnd_MainClass EXE_TYP(HWND, 0x739d54)
#define pack_dblbfr_hdc EXE_TYP(HDC, 0x62047c)
#define pack_critical_section_for_painting EXE_TYP_PTR(CRITICAL_SECTION, 0x790500)
#define pack_read_graphics_file EXE_FN(HGDIOBJ, 0x459fa0, const char *)
#define pack_set_palette_and_stretch_mode EXE_FN(void, 0x466050, HDC)
#define pack_blt_pic EXE_FN(void, 0x4662f0, HDC, RECT *, HGDIOBJ)
#define pack_handle_palette_msgs EXE_FN(INT_PTR, 0x467fa0, HWND, UINT, WPARAM, LPARAM)
#define pack_draw_button EXE_FN(void, 0x4673c0, DRAWITEMSTRUCT *, HBRUSH, HPEN, HPEN, COLORREF, int)
__attribute__((dllimport)) void blt_expansion_symbol(HDC hdc, const RECT *rect, int expansion, int rarity, int force_color);
'''
WIN32_NAMES='CreateSolidBrush CreatePen SetDlgItemTextA GetDlgItem ShowWindow SendDlgItemMessageA SetFocus EndDialog DeleteObject GetFocus GetSysColor SetTextColor SetBkColor GetTextMetricsA SendMessageA ExtTextOutA GetSysColorBrush FillRect DrawFocusRect GetClientRect EnterCriticalSection SaveDC GetStockObject RestoreDC BitBlt LeaveCriticalSection DialogBoxIndirectParamA'.split()
WIN32_SIGNATURES={
    'CreateSolidBrush':['HBRUSH','COLORREF'],'CreatePen':['HPEN','int','int','COLORREF'],
    'SetDlgItemTextA':['int','HWND','int','char *'],'GetDlgItem':['HWND','HWND','int'],
    'ShowWindow':['int','HWND','int'],'SendDlgItemMessageA':['LRESULT','HWND','int','UINT','WPARAM','LPARAM'],
    'SetFocus':['HWND','HWND'],'EndDialog':['int','HWND','INT_PTR'],'DeleteObject':['int','HGDIOBJ'],
    'GetFocus':['HWND'],'GetSysColor':['COLORREF','int'],'SetTextColor':['COLORREF','HDC','COLORREF'],
    'SetBkColor':['COLORREF','HDC','COLORREF'],'GetTextMetricsA':['int','HDC','TEXTMETRIC *'],
    'SendMessageA':['LRESULT','HWND','UINT','WPARAM','LPARAM'],
    'ExtTextOutA':['int','HDC','int','int','UINT','RECT *','char *','UINT','int *'],
    'GetSysColorBrush':['HBRUSH','int'],'FillRect':['int','HDC','RECT *','HBRUSH'],
    'DrawFocusRect':['int','HDC','RECT *'],'GetClientRect':['int','HWND','RECT *'],
    'EnterCriticalSection':['void','CRITICAL_SECTION *'],'SaveDC':['int','HDC'],
    'GetStockObject':['HGDIOBJ','int'],'RestoreDC':['int','HDC','int'],
    'BitBlt':['int','HDC','int','int','int','int','HDC','int','int','DWORD'],
    'LeaveCriticalSection':['void','CRITICAL_SECTION *'],
    'DialogBoxIndirectParamA':['INT_PTR','HINSTANCE','DLGTEMPLATE *','HWND','void *','LPARAM']}
EXE_INTERFACES=[
    ['0x459fa0','read_graphics_file','HGDIOBJ','char *'],
    ['0x466050','set_palette_and_stretch_mode','void','HDC'],
    ['0x4662f0','blt_pic','void','HDC','RECT *','HGDIOBJ'],
    ['0x467fa0','handle_palette_msgs','INT_PTR','HWND','UINT','WPARAM','LPARAM'],
    ['0x4673c0','draw_pack_dialog_button','void','DRAWITEMSTRUCT *','HBRUSH','HPEN','HPEN','COLORREF','int']]
LITERALS={0x276c6a5:b'%s\\WINBK_QuestN.pic',0x276c6c8:b'Menus.txt',0x276c6b9:b'EXPANSIONNAMES',0x276c6a0:b'14-',0x276c6a4:b'',0x276c6d2:b'%s (%s%d cards)',0x276c6ed:b'%s %s deck (%d cards)',0x276c703:b'starter',0x276c6e2:b'tournament',0x273f226:b'Booster contents'}


def storage(debug,runtime):
    records={};statics={};local_arrays={};row=None
    for cu in debug.dwarf().iter_CUs():
        source=value(cu.get_top_DIE(),'DW_AT_name')
        if source not in {'functions/packs.c','cards/unglued_unhinged.c','cards/promo.c'}:continue
        for d in cu.iter_DIEs():
            n=value(d,'DW_AT_name')
            if source=='functions/packs.c' and d.tag=='DW_TAG_structure_type' and n in {'choose_a_pack_args_t','tagDRAWITEMSTRUCT','tagTEXTMETRICA','tagRECT'}:
                records[n]=dict(bytes=value(d,'DW_AT_byte_size'),members={value(c,'DW_AT_name'):value(c,'DW_AT_data_member_location') for c in d.iter_children() if c.tag=='DW_TAG_member'})
                if n=='choose_a_pack_args_t':
                    member=next(c for c in d.iter_children() if value(c,'DW_AT_name')=='results');t=type_of(member)
                    assert t.tag=='DW_TAG_array_type' and [value(c,'DW_AT_upper_bound') for c in t.iter_children()]==[5]
                    t=type_of(t);row=dict(bytes=value(t,'DW_AT_byte_size'),members={value(c,'DW_AT_name'):value(c,'DW_AT_data_member_location') for c in t.iter_children() if c.tag=='DW_TAG_member'})
            if d.tag!='DW_TAG_variable' or 'DW_AT_location' not in d.attributes:continue
            parent=d.get_parent()
            while parent is not None and parent.tag!='DW_TAG_subprogram':parent=parent.get_parent()
            owner=value(parent,'DW_AT_name') if parent is not None else None
            if owner not in {'dlgproc_choose_a_pack','choose_packs','generate_choose_a_pack_dlg_template','card_booster_tutor','card_stocking_tiger'}:continue
            loc=value(d,'DW_AT_location');t=type_of(d)
            if isinstance(loc,list) and len(loc)==5 and loc[0]==3:
                address=struct.unpack('<I',bytes(loc[1:]))[0]
                statics[owner+'::'+n]=dict(address=f'{address:x}',location=loc,bytes=value(t,'DW_AT_byte_size'))
            if t.tag=='DW_TAG_array_type':
                local_arrays[owner+'::'+n]=dict(element_bytes=value(type_of(t),'DW_AT_byte_size'),bounds=[value(c,'DW_AT_upper_bound') for c in t.iter_children() if c.tag=='DW_TAG_subrange_type'],location=loc)
    assert records['choose_a_pack_args_t']==dict(bytes=64,members=dict(num_packs=0,allow_boosters=4,allow_starters=8,allow_cancel=12,results=16))
    assert row==dict(bytes=8,members=dict(expansion=0,booster=4))
    assert statics['choose_packs::idx']['address']=='2721e18' and runtime.pe.get_data(0x721e18,4)==b'\xff'*4
    assert statics['generate_choose_a_pack_dlg_template::templ']['address']=='2721e20'
    assert {k:v['address'] for k,v in statics.items() if k.startswith('dlgproc_choose_a_pack::')}=={
        'dlgproc_choose_a_pack::winbk_questn':'27952b4','dlgproc_choose_a_pack::expansion_names':'27952a0',
        'dlgproc_choose_a_pack::brush':'27952b0','dlgproc_choose_a_pack::pen1':'27952ac','dlgproc_choose_a_pack::pen2':'27952a8','dlgproc_choose_a_pack::args':'27952a4'}
    assert local_arrays['dlgproc_choose_a_pack::path']['bounds']==[278] and local_arrays['dlgproc_choose_a_pack::buf']['bounds']==[299]
    return dict(records=records,result_row=row,statics=statics,arrays=local_arrays)


def template(runtime):
    data=runtime.pe.get_data(0x721e20,294);style,extended,count,x,y,cx,cy=struct.unpack_from('<IIHhhhh',data);at=18
    def field():
        nonlocal at
        first=struct.unpack_from('<H',data,at)[0];at+=2
        if first==0xffff:
            ordinal=struct.unpack_from('<H',data,at)[0];at+=2;return ordinal
        text=[]
        while first:
            text.append(first);first=struct.unpack_from('<H',data,at)[0];at+=2
        return bytes().join(struct.pack('<H',c) for c in text).decode('utf-16le')
    menu=field();window_class=field();title=field();point_size=struct.unpack_from('<H',data,at)[0];at+=2;font=field();controls=[]
    for i in range(count):
        at=(at+3)&~3;s,e,xx,yy,w,h,ident=struct.unpack_from('<IIhhhhH',data,at);at+=18
        cls=field();text=field();extra=struct.unpack_from('<H',data,at)[0];at+=2;extra_data=data[at:at+extra];at+=extra
        controls.append(dict(style=s,extended=e,x=xx,y=yy,width=w,height=h,id=ident,window_class=cls,title=text,extra_bytes=extra_data.hex()))
    assert at==294 and count==8 and [c['id'] for c in controls]==list(range(1101,1107))+[1,2]
    assert font=='MS Sans Serif' and point_size==10 and (x,y,cx,cy)==(0,0,186,116)
    declaration='static unsigned char choose_a_pack_template[294] __attribute__((aligned(4))) = {\n'+''.join('  '+', '.join(f'0x{b:02x}' for b in data[i:i+16])+',\n' for i in range(0,len(data),16))+'};'
    return dict(address='2721e20',bytes=data.hex(),sha256=sha(data),style=style,extended=extended,menu=menu,window_class=window_class,title=title,font=font,point_size=point_size,controls=controls),dict(source='functions/packs.c',name='choose_a_pack_template',declaration=declaration,target_address='2721e20',linkage='private',native_bytes=294)


def native_interfaces(runtime,root):
    runtime.pe.parse_data_directories();imports={s.address:dict(dll=d.dll.decode(),name=s.name.decode()) for d in runtime.pe.DIRECTORY_ENTRY_IMPORT for s in d.imports if s.name}
    names=set(WIN32_NAMES)|{'blt_expansion_symbol'};selected={f'{a:x}':record for a,record in imports.items() if record['name'] in names};assert len(selected)==28
    assert selected['27b5360']==dict(dll='Drawcardlib.dll',name='blt_expansion_symbol')
    get=lambda a,n:runtime.pe.get_data(a-0x2000000,n)
    frames=[]
    for start,end,args in [(0x23052f8,0x230530c,[0,26]),(0x2305370,0x2305384,[1,26]),(0x241130f,0x2411323,[0,26]),(0x241144c,0x2411460,[1,26])]:
        code=get(start,end-start);assert code[:3]==bytes.fromhex('c74424') and struct.unpack_from('<I',code,4)[0]==26
        assert code[8:11]==bytes.fromhex('c70424') and struct.unpack_from('<I',code,11)[0]==args[0] and code[15]==0xe8
        target=end+struct.unpack_from('<i',code,16)[0];assert target==(0x4a6bc0 if args[0]==0 else 0x4a70c0)
        frames.append(dict(address=f'{start:x}',bytes=code.hex(),arguments=args,target=f'{target:x}'))
    exe=Image((root/'.recovery/2022-Magic.exe').read_bytes());assert sha(exe.data)=='0170f02b1c381a20a97ca0c12e99c8d338453ce4edbfc99148e726d565bee0de'
    boundaries={f'{a:x}':exe.pe.get_data(a-0x400000,48).hex() for a in [0x459fa0,0x466050,0x4662f0,0x467fa0,0x4673c0,0x4a6bc0,0x4a70c0]}
    for a,text in LITERALS.items():assert readonly_string(runtime,a)==text,(hex(a),text)
    assert set(WIN32_SIGNATURES)==set(WIN32_NAMES)
    return dict(imports=selected,win32_signatures=WIN32_SIGNATURES,exe_interfaces=EXE_INTERFACES,network_frames=frames,exe_boundaries=boundaries,exe_sha256=sha(exe.data),literals={f'{a:x}':dict(bytes=(s+b'\0').hex(),literal=byte_literal(s)) for a,s in LITERALS.items()},
        vla_allocations={TUTOR:dict(count=2,element_bytes=4,size_callee='26ec8a0',probe_sites=['23053d3','23053e0']),TIGER:dict(count=1,element_bytes=4,size_callee='26ec8a0',probe_sites=['24113a8'])},
        retained_native_behavior=['Booster Tutor display and selection use maximum pack capacity, including any unfilled entries; retries have no cancel fallback.',
        'choose_packs treats every nonzero DialogBox result, including -1, as success.',
        'Stocking Tiger clears its packed expansion marker before generation and consumes the full generated pack on the qualifying combat event.',
        'Dialog expansion-name guard remains num_textlines > expansion + 6 with lookup at expansion + 7.'])


def reconstruct(a,raw,runtime,meta):
    original=strip_comments(raw);body=BODIES[a].strip()+'\n';size=int(meta[a]['high_pc'],16)-int(a,16)
    return body,[dict(original=original,replacement=body,occurrences=1,kind='restore native Win32 typed records/static storage, exact template interface and stack-probed card arrays with explicit network frames',machine_sites=[],machine_range=[a,meta[a]['high_pc'][2:]],machine_bytes=size,machine_sha256=sha(runtime.pe.get_data(int(a,16)-0x2000000,size)))]


def audit_component(component,manifest,sources,machine_bytes,root):
    entries={f['address']:f for f in manifest['accepted_functions'] if f.get('pack_chooser_evidence')};assert set(entries)==set(ADDRESSES)==set(component['functions'])
    runtime=Image((root/'.recovery/2022-ManalinkEh.dll').read_bytes());assert component['runtime_sha256']==sha(runtime.data)=='52793f60124a3b83d8b2728d145e918101a395b4e8d1c0af69cddcfc87ef10e5'
    debug=Image(Path('G:/Manalink/ManalinkEh_10_22_Hotfix.dbg').read_bytes());assert component['native_storage']==storage(debug,runtime) and component['native_interfaces']==native_interfaces(runtime,root)
    templ,global_=template(runtime);assert component['template']==templ and component['globals']==[global_] and global_ in manifest['source_globals']
    source=sources['functions/packs.c'].replace('\r\n','\n');assert source.count(global_['declaration'])==1 and PRIVATE_TYPES in source and component['private_types']==PRIVATE_TYPES
    for a in LITERALS:
        literal=component['native_interfaces']['literals'][f'{a:x}']['literal']
        assert literal in strip_comments(BODIES[DIALOG]+BODIES[TUTOR]),literal
    meta={r['low_pc'][2:]:r for r in csv.DictReader((root/'.recovery/2022-functions.csv').open()) if r['low_pc'] and r['high_pc']};decoder=Cs(CS_ARCH_X86,CS_MODE_32);decoder.detail=True
    overlap=[s.split('\t')[1] for s in (root/'.recovery/absolute-interfaces.tsv').read_text().splitlines() if s.split('\t')[2]=='global']
    for a,e in entries.items():
        detail=component['functions'][a];assert e['pack_chooser_evidence']==detail and e['static']==(a in PRIVATE)
        body=function_body(sources[e['source']].replace('\r\n','\n'),e['name']);assert sha(body.encode())==e['reconstruction_body_sha256']
        raw=read(root/'.recovery'/('decompiled-pack-chooser' if component.get('planning') else 'decompiled-abi')/f'{a}.c');assert sha(raw.encode())==e['raw_sha256']
        text,changes=reconstruct(a,raw,runtime,meta);assert changes==detail['changes'] and target_validation_input(strip_comments(raw),detail,a,machine_bytes)==text
        assert function_body(cleaned(text,overlap),e['name'])==body
        code=machine_bytes(int(a,16),detail['machine_bytes']);assert sha(code)==detail['machine_sha256']
        insns={f'{i.address:x}':i for i in decoder.disasm(code,int(a,16))}
        for site in detail['canonical_calls']+detail['callback_references']:
            ins=insns[site['address']];assert ins.bytes.hex()==site['bytes'] and any(o.type==X86_OP_IMM and o.imm==int(site['target'],16) for o in ins.operands);assert re.search(r'\b'+site['name']+r'\b',body)
        for site in detail['iat_references']:
            ins=insns[site['address']];assert ins.bytes.hex()==site['bytes'] and any(o.type==X86_OP_MEM and o.mem.disp==int(site['iat'],16) for o in ins.operands)
            assert component['native_interfaces']['imports'][site['iat']]['name']==site['name'] and site['name'] in body
    assert entries[CHOOSE]['prototype'] in read(root/'manalink.h') or component.get('planning')
    assert FLAGS in read(root/'manalink.h') or component.get('planning')
    assert '  blt_expansion_symbol\n' in read(root/'Drawcardlib.def') or component.get('planning')
    assert component['import_definition_append']==IMPORT_APPEND
    definition=read(root/'Drawcardlib.def')
    if not component.get('planning'):
        from recover_display_hooks import previous_import_definition
        definition=previous_import_definition(root,definition)
        assert definition.endswith(IMPORT_APPEND);definition=definition.removesuffix(IMPORT_APPEND)
    assert sha(definition.encode())==component['previous_import_definition_sha256']


def main():
    parser=argparse.ArgumentParser(__doc__);parser.add_argument('--apply',action='store_true');parser.add_argument('--prepare',action='store_true');args=parser.parse_args()
    root=Path(__file__).resolve().parents[2];cache=root/'.recovery';mp=root/'docs/recovery-evidence/recovered-functions.json';ip=root/'docs/recovery-evidence/interface-recovery.json';m=json.loads(mp.read_text());interfaces=json.loads(ip.read_text());meta={r['low_pc'][2:]:r for r in csv.DictReader((cache/'2022-functions.csv').open()) if r['low_pc'] and r['high_pc']};debug=Image(Path('G:/Manalink/ManalinkEh_10_22_Hotfix.dbg').read_bytes());runtime=Image((cache/'2022-ManalinkEh.dll').read_bytes());get=lambda a,n:runtime.pe.get_data(a-0x2000000,n)
    if args.prepare:
        storage(debug,runtime);template(runtime);native=native_interfaces(runtime,root)
        import_rows={r['name']:[a,r['name'],r['dll']]+WIN32_SIGNATURES[r['name']] for a,r in native['imports'].items() if r['name'] in WIN32_SIGNATURES}
        (cache/'pack-dialog-imports.tsv').write_text(''.join('\t'.join(import_rows[n])+'\n' for n in WIN32_NAMES))
        (cache/'pack-dialog-exe.tsv').write_text(''.join('\t'.join(r)+'\n' for r in EXE_INTERFACES))
        (cache/'export-request-pack-chooser.tsv').write_text(''.join('\t'.join([a,meta[a]['source'],meta[a]['name'],meta[a]['high_pc'][2:]])+'\n' for a in ADDRESSES));print('Prepared pack dialog, template, public chooser and two card consumers.');return
    assert 'pack_chooser_native_2022' not in interfaces and not set(ADDRESSES)&{f['address'] for f in m['accepted_functions']}
    templ,global_=template(runtime);globals_=[global_]
    proof=dict(previous_import_definition_sha256=sha(read(root/'Drawcardlib.def').encode()),import_definition_append=IMPORT_APPEND,runtime_sha256=sha(runtime.data),debug_sha256=sha(debug.data),native_storage=storage(debug,runtime),native_interfaces=native_interfaces(runtime,root),template=templ,globals=globals_,private_types=PRIVATE_TYPES,**debug_evidence(debug,meta,ADDRESSES),functions={},
        scope='Recover the full native pack chooser/dialog, its exact eight-control template, Booster Tutor and Stocking Tiger in their original source files. Retain stdcall callback ABI, six-selector records, native local statics, owner-draw/background/palette/resource lifecycle, clamping and DialogBox result semantics. Restore three four-byte VLAs, 216-byte filter, explicit network packet-26 frames, native capacity-vs-count quirks and packed Stocking Tiger marker. Wish/card-picker stubs remain unchanged; gameplay integration remains downstream.')
    targets={int(f['address'],16):f for f in m['accepted_functions']};targets.update({int(a,16):dict(name=meta[a]['name'],source=meta[a]['source'],static=a in PRIVATE) for a in ADDRESSES});decoder=Cs(CS_ARCH_X86,CS_MODE_32);decoder.detail=True;overlap=[s.split('\t')[1] for s in (cache/'absolute-interfaces.tsv').read_text().splitlines() if s.split('\t')[2]=='global'];sources={'functions/packs.c':read(root/'functions/packs.c')+'\n\n#ifndef SHANDALAR\n'+PRIVATE_TYPES+'\n'+global_['declaration']+'\n#endif\n'};entries=[]
    for a in ADDRESSES:
        row=meta[a];source=row['source'];name=row['name'];raw=read(cache/'decompiled-pack-chooser'/f'{a}.c');text,changes=reconstruct(a,raw,runtime,meta);body=cleaned(text,overlap)
        if a in PRIVATE:body='static '+body
        assert not UNRESOLVED.search(strip_comments(body)) and not re.search(r'stack0x|\._\d|_conflict|::',body),a
        symbols=[(n,s) for n,v,sec,k,s in debug.functions() if sec>0 and debug.pe.OPTIONAL_HEADER.ImageBase+debug.pe.sections[sec-1].VirtualAddress+v==int(a,16)];assert len(symbols)==1 and symbols[0][1]==(3 if a in PRIVATE else 2)
        code=get(int(a,16),int(row['high_pc'],16)-int(a,16));detail=dict(machine_bytes=len(code),machine_sha256=sha(code),changes=changes,canonical_calls=[],callback_references=[],iat_references=[],native_coff_storage=symbols[0][1],native_coff_name=symbols[0][0])
        for ins in decoder.disasm(code,int(a,16)):
            for op in ins.operands:
                if op.type==X86_OP_MEM and f'{op.mem.disp:x}' in proof['native_interfaces']['imports']:
                    imp=proof['native_interfaces']['imports'][f'{op.mem.disp:x}'];assert imp['name'] in body
                    detail['iat_references'].append(dict(address=f'{ins.address:x}',bytes=ins.bytes.hex(),iat=f'{op.mem.disp:x}',**imp))
                if op.type!=X86_OP_IMM or op.imm not in targets:continue
                target=targets[op.imm];assert re.search(r'\b'+target['name']+r'\b',body),(a,target['name']);assert not target['static'] or target['source']==source
                site=dict(address=f'{ins.address:x}',bytes=ins.bytes.hex(),target=f'{op.imm:x}',name=target['name']);(detail['canonical_calls'] if ins.mnemonic in {'call','jmp'} else detail['callback_references']).append(site)
        proof['functions'][a]=detail;entries.append(dict(address=a,name=name,source=source,line=row['line'],static=a in PRIVATE,prototype=body[:body.index('{')].strip()+';',prototype_scope='manalink.h' if a==CHOOSE else source,raw_sha256=sha(raw.encode()),recovered_literals=[],pack_chooser_evidence=detail,reconstruction=proof['scope'],reconstruction_body_sha256=sha(function_body(body,name).encode())));sources[source]=sources.get(source,read(root/source))+f'\n\n#ifndef SHANDALAR\n/* ManalinkEh 2022: 0x{a}; original {source}:{row["line"]}. */\n'+body+'\n#endif\n'
    header=read(root/'manalink.h');at=header.rfind('#ifdef __cplusplus');header=header[:at]+LABEL+'\n#ifndef SHANDALAR\n'+FLAGS+'\n'+next(e['prototype'] for e in entries if e['address']==CHOOSE)+'\n#endif\n\n'+header[at:];compile_sources(root,cache/'pack-chooser-check',sources,header)
    proof['planning']=True;audit_component(proof,{**m,'accepted_functions':m['accepted_functions']+entries,'source_globals':m['source_globals']+globals_},sources,get,root);del proof['planning']
    if args.apply:
        citations=m['accepted_functions']+m['source_fragments']+[s for f in m['accepted_functions'] for s in f.get('supporting_exports',[])];assert not set(ADDRESSES)&{f['address'] for f in citations};rows={r['address']:r for r in csv.DictReader((cache/'decompiled-abi/export.tsv').open(),delimiter='\t')};before=len(rows)
        for row in csv.DictReader((cache/'decompiled-pack-chooser/export.tsv').open(),delimiter='\t'):rows[row['address']]=row;shutil.copyfile(cache/'decompiled-pack-chooser'/f'{row["address"]}.c',cache/'decompiled-abi'/f'{row["address"]}.c')
        proof['new_canonical_raw_entries']=len(rows)-before
        with (cache/'decompiled-abi/export.tsv').open('w',newline='') as stream:
            writer=csv.DictWriter(stream,fieldnames=['address','source','name','status','prototype'],delimiter='\t');writer.writeheader();writer.writerows(rows.values())
        for source,text in sources.items():(root/source).write_bytes(text.replace('\n','\r\n').encode('latin1'))
        (root/'manalink.h').write_bytes(header.replace('\n','\r\n').encode('latin1'))
        imports=read(root/'Drawcardlib.def');assert '  blt_expansion_symbol\n' not in imports;(root/'Drawcardlib.def').write_bytes((imports+'  blt_expansion_symbol\n').replace('\n','\r\n').encode())
        m['accepted_functions']+=entries;m['accepted']+=5;m['card_handler_functions']+=2;m['helper_functions']+=3;m['private_functions']+=1;m['private_helpers']+=1;m['shared_helper_interfaces']+=2;m['source_globals']+=globals_
        for e in entries:m['source_files'][e['source']]=m['source_files'].get(e['source'],0)+1
        m['raw_export_entries']=len(rows);m['exported_functions']=len({r['name'] for r in rows.values()});save_json(mp,m);interfaces['pack_chooser_native_2022']=proof;save_json(ip,interfaces)
    save_json(cache/'pack-chooser-plan.json',proof);print(json.dumps(dict(applied=args.apply,new_cards=2,new_helpers=3,new_template_bytes=294)))


if __name__=='__main__':main()
