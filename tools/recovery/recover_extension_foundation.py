"""Recover extension storage, lifecycle, copies and packed information helpers together."""
import argparse
import collections
import csv
import hashlib
import json
import re
import shutil
import struct
import subprocess
from pathlib import Path

from inspect_release import Image,value,inherited_value
from export_abi import type_of
from extension_bodies import BODIES
from audit_source import function_body,literal_tokens


def main():
    parser=argparse.ArgumentParser(__doc__);parser.add_argument('--apply',action='store_true');args=parser.parse_args()
    root=Path(__file__).resolve().parents[2];cache=root/'.recovery'
    foundation=json.loads((root/'docs/recovery-evidence/extension-foundation.json').read_text())
    debug_bytes=Path('G:/Manalink/ManalinkEh_10_22_Hotfix.dbg').read_bytes()
    assert hashlib.sha256(debug_bytes).hexdigest()==foundation['debug_sha256']
    debug=Image(debug_bytes);runtime=Image((cache/'2022-ManalinkEh.dll').read_bytes())
    log=(cache/'extension-pair-console.log').read_text();assert 'ERROR' not in log
    assert 'RECOVERY_INST_INFO_RETURN_COMPLETE EDX:4,EAX:4' in log and 'RECOVERY_EXPORT_COMPLETE success=18 failed=0' in log
    exports={r['name']:r for r in csv.DictReader((cache/'decompiled-extension-pair/export.tsv').open(),delimiter='\t')}
    all_functions={r['low_pc'][2:]:r for r in csv.DictReader((cache/'2022-functions.csv').open()) if r['low_pc']}
    manifest_path=root/'docs/recovery-evidence/recovered-functions.json';manifest=json.loads(manifest_path.read_text())
    assert not {exports[n]['address'] for n in BODIES}&{f['address'] for f in manifest['accepted_functions']}
    enums={}
    for cu in debug.dwarf().iter_CUs():
        if value(cu.get_top_DIE(),'DW_AT_name')!='functions/deck.c':continue
        for die in cu.get_top_DIE().iter_children():
            if die.tag=='DW_TAG_typedef' and value(die,'DW_AT_name')=='CardFromWhere':
                typ=type_of(die);assert value(typ,'DW_AT_byte_size')==4 and value(typ,'DW_AT_encoding')==5
                enums={value(c,'DW_AT_name'):value(c,'DW_AT_const_value') for c in typ.iter_children()}
        break
    assert len(enums)==32
    def declaration(typ,name):
        match=re.fullmatch(r'(\w+)((?:\[\d+\])*)',typ);assert match,typ
        return f'  {match[1]} {name}{match[2]};'
    types=foundation['types'];type_block='\n#ifndef SHANDALAR\n/* October 2022 extension records, verified against DWARF. */\n'
    type_block+='typedef enum\n{\n'+',\n'.join(f'  {n} = {v}' for n,v in enums.items())+'\n} CardFromWhere;\n\n'
    for name in ['inst_info_t','inst_stored_iid_t']:
        type_block+='typedef struct\n{\n'+'\n'.join(declaration(m['type'],m['name']) for m in types[name]['members'])+f'\n}} {name};\n\n'
    type_block+='struct card_ext_struct\n{\n'+'\n'.join(declaration(m['type'],m['name']) for m in types['card_ext_t']['members'])+'\n};\n#endif\n\n'
    defs=(root/'defs.h').read_bytes().decode('latin1').replace('\r\n','\n')
    assert 'typedef struct card_ext_struct card_ext_t;' not in defs
    marker='/* Info struct */';assert defs.count(marker)==1
    defs=defs.replace(marker,'#ifndef SHANDALAR\ntypedef struct card_ext_struct card_ext_t;\n#endif\n\n'+marker,1)
    old_line=next(l for l in defs.splitlines(True) if 'uint8_t' in l and 'mana_to_untap[8]' in l)
    new_slot='''  union
  {
    uint8_t extension_init_bytes[8]; /* Byte view for the existing zero initializer. */
    struct
    {
      card_ext_t *ext;               /* October 2022: 0x2c. */
      uint32_t unused30;             /* October 2022: 0x30. */
    };
  };
'''
    defs=defs.replace(old_line,new_slot,1);at=defs.rfind('#endif');defs=defs[:at]+type_block+defs[at:]
    template_bytes=bytes.fromhex(foundation['globals']['card_ext_template']['bytes']);assert len(template_bytes)==192
    base_formats={'uint8_t':('B',1),'int8_t':('b',1),'uint16_t':('H',2),'int16_t':('h',2),
                  'uint32_t':('I',4),'CardFromWhere':('i',4),'special_flags_t':('I',4),
                  'special_flags2_t':('I',4),'special_flags3_t':('I',4)}
    def initializer(name,data):
        result=[]
        for member in types[name]['members']:
            typ=member['type'];offset=member['offset'];match=re.fullmatch(r'(\w+)(?:\[(\d+)\])?',typ);assert match
            base,count=match[1],int(match[2] or 1)
            if base in types:
                values=[initializer(base,data[offset+i*types[base]['bytes']:offset+(i+1)*types[base]['bytes']]) for i in range(count)]
            else:
                code,size=base_formats[base];values=[str(struct.unpack_from('<'+code,data,offset+i*size)[0]) for i in range(count)]
            value_text='{ '+', '.join(values)+' }' if match[2] else values[0]
            result.append('.'+member['name']+' = '+value_text)
        return '{ '+', '.join(result)+' }'
    template_initializer=initializer('card_ext_t',template_bytes)
    declaration_globals=[]
    for name,g in foundation['globals'].items():
        if name=='card_ext_template':decl='static const card_ext_t card_ext_template = '+template_initializer+';'
        else:
            typ=g['type'];m=re.fullmatch(r'(\w+)((?:\[\d+\])*)',typ);assert m
            decl=('static ' if g['coff_storage_class']==3 else '')+m[1]+' '+name+m[2]+';'
        if name=='card_ext_template':
            decl=subprocess.run(['C:/Program Files/LLVM/bin/clang-format.exe',
                '-style={BasedOnStyle: LLVM, IndentWidth: 2, BreakBeforeBraces: Allman, ColumnLimit: 100}'],
                input=decl.encode(),capture_output=True,check=True).stdout.decode().replace('\r\n','\n').strip()
        source='functions/ai.c' if name.startswith('backup') else 'functions/deck.c'
        declaration_globals.append(dict(source=source,name=name,declaration=decl,target_address=g['address'],
            zero_initialized=name!='card_ext_template',linkage='static' if g['coff_storage_class']==3 else 'external'))
    sources={s:(root/s).read_bytes().decode('latin1').replace('\r\n','\n') for s in
        ['functions/deck.c','functions/engine.c','functions/ai.c','functions/windows.c','functions/gain_control_and_legend_rule.c']}
    # windows.c existed before this pass but previously contained no recovered unit.
    window_snapshot=cache/'batch23-before/functions/windows.c'
    if not window_snapshot.exists():
        window_snapshot.parent.mkdir(parents=True,exist_ok=True)
        shutil.copyfile(root/'functions/windows.c',window_snapshot)
    for source in ['functions/deck.c','functions/ai.c']:
        block='\n/* October 2022 extension storage; fixed banks and original template. */\n'
        block+='\n'.join(g['declaration'] for g in declaration_globals if g['source']==source)+'\n'
        assert sources[source].count('#include "manalink.h"\n')==1
        sources[source]=sources[source].replace('#include "manalink.h"\n','#include "manalink.h"\n'+block,1)
    # Preserve the old 300-byte instance initializer with an explicit byte view.
    old_init='{0,0,0,0,0,0,0,0},\t// mana_to_untap[8]\t\t// [7] is left uninitialized'
    assert sources['functions/deck.c'].count(old_init)==1
    sources['functions/deck.c']=sources['functions/deck.c'].replace(old_init,
        '{{0,0,0,0,0,0,0,0}},\t// extension pointer and unused30',1)
    fragments=[];copy_proof={}
    canonical_path=cache/'decompiled-abi/export.tsv';canonical_rows=list(csv.DictReader(canonical_path.open(),delimiter='\t'))
    canonical={r['address']:r for r in canonical_rows};supplemental={}
    def raw_for(row):
        address=row['address']
        if address in canonical:return (cache/'decompiled-abi'/(address+'.c')).read_text()
        supplemental[address]=row
        return (cache/'decompiled-extension-pair'/(address+'.c')).read_text()
    def fragment(source,function,body,replaced=None):
        row=exports.get(function) or canonical['2663990'];address=row['address']
        start=f'/* Recovered 2022 extension fragment: {function} @ 0x{address}. */'
        end=f'/* End recovered 2022 extension fragment: {function}. */'
        entry=dict(source=source,original_function=function,address=address,raw_sha256=hashlib.sha256(raw_for(row).encode()).hexdigest(),
            start_marker=start,end_marker=end,body_sha256=hashlib.sha256(body.strip().encode()).hexdigest(),
            recovery='Only the independently verified extension binding or copy; other existing source logic retained.')
        if replaced:entry['replaced_source']=replaced
        fragments.append(entry)
        return '  '+start+'\n  '+body+'\n  '+end+'\n'
    needle='  memcpy(instance, &card_instance_template, sizeof(card_instance_t));\n'
    assert sources['functions/deck.c'].count(needle)==1
    sources['functions/deck.c']=sources['functions/deck.c'].replace(needle,needle+fragment('functions/deck.c','create_card_instance',
        'instance->ext = &card_ext[player][card];\n  memcpy(instance->ext, &card_ext_template, sizeof(*instance->ext));'),1)
    for function,backup,restoring in [('backup_data_for_ai','backup_card_ext',False),('restore_data_for_ai','backup_card_ext',True),
        ('backup_data_for_ai_0','backup0_card_ext',False),('restore_data_for_ai_0','backup0_card_ext',True)]:
        row=exports[function];start=int(row['address'],16);end=int(all_functions[row['address']]['high_pc'],16)
        code=runtime.pe.get_data(start-runtime.pe.OPTIONAL_HEADER.ImageBase,end-start)
        source,dest=(backup,'card_ext') if restoring else ('card_ext',backup)
        src=int(foundation['globals'][source]['address'],16);dst=int(foundation['globals'][dest]['address'],16)
        patterns=[b'\xc7\x44\x24\x08'+struct.pack('<I',57600),b'\xc7\x44\x24\x04'+struct.pack('<I',src),b'\xc7\x04\x24'+struct.pack('<I',dst)]
        positions=[code.find(p) for p in patterns];assert all(p>=0 for p in positions)
        assert positions==sorted(positions) and positions[-1]-positions[0]<32
        copy_proof[function]=dict(source=f'{src:x}',destination=f'{dst:x}',bytes=57600,
            stores=[dict(address=f'{start+p:x}',bytes=pat.hex()) for p,pat in zip(positions,patterns)])
        begin=sources['functions/ai.c'].index(f'void {function}(void)\n{{\n')
        at=sources['functions/ai.c'].index('#define BACKUP_CMD_A(data)',begin)
        sources['functions/ai.c']=sources['functions/ai.c'][:at]+fragment('functions/ai.c',function,
            f'memcpy({dest}, {source}, sizeof(card_ext));')+sources['functions/ai.c'][at:]
    old_copy='      memcpy(v5, v4, 0x12Cu);\n';assert sources['functions/gain_control_and_legend_rule.c'].count(old_copy)==1
    sources['functions/gain_control_and_legend_rule.c']=sources['functions/gain_control_and_legend_rule.c'].replace(old_copy,
        fragment('functions/gain_control_and_legend_rule.c','gain_control_impl',
            'memcpy_inst_and_ext(v5, (const card_instance_t *)v4);',old_copy),1)
    entries=[]
    for name,body in BODIES.items():
        row=exports[name];address=row['address'];source=row['source'];meta=all_functions[address]
        assert any(n=='_'+name and storage==2 and debug.pe.OPTIONAL_HEADER.ImageBase+debug.pe.sections[s-1].VirtualAddress+off==int(address,16)
            for n,off,s,kind,storage in debug.functions())
        raw=(cache/'decompiled-extension-pair'/(address+'.c')).read_text();assert literal_tokens(raw)==literal_tokens(body)
        body=subprocess.run(['C:/Program Files/LLVM/bin/clang-format.exe',
            '-style={BasedOnStyle: LLVM, IndentWidth: 2, BreakBeforeBraces: Allman, ColumnLimit: 100}'],
            input=body.encode(),capture_output=True,check=True).stdout.decode().replace('\r\n','\n')
        sources[source]+=f'\n\n/* ManalinkEh 2022: 0x{address}; original {source}:{meta["line"]}. */\n'+body
        entries.append(dict(name=name,address=address,source=source,line=meta['line'],static=False,
            prototype=body[:body.index('{')].strip()+';',raw_sha256=hashlib.sha256(raw.encode()).hexdigest(),recovered_literals=[],
            reconstruction='Verified 192-byte extension layout, fixed banks and explicit lifecycle. Packed six-entry information semantics and twelve-entry stored-ID semantics preserve target behavior.',
            reconstruction_body_sha256=hashlib.sha256(function_body(body,name).encode()).hexdigest()))
    header=(root/'manalink.h').read_bytes().decode('latin1').replace('\r\n','\n');at=header.rfind('#ifdef __cplusplus')
    public='#ifndef SHANDALAR\n/* October 2022 extension lifecycle and information interfaces. */\n'
    public+='\n'.join('extern '+g['declaration'] for g in declaration_globals if g['linkage']=='external')+'\n'
    public+='\n'.join(e['prototype'] for e in entries)+'\n#endif\n\n';header=header[:at]+public+header[at:]
    candidate=cache/'extension-foundation-candidate';candidate.mkdir(exist_ok=True)
    (candidate/'manalink.h').write_text(header);(candidate/'defs.h').write_text(defs)
    assertions=['_Static_assert(sizeof(card_instance_t)==300 && offsetof(card_instance_t,ext)==44 && offsetof(card_instance_t,unused30)==48,"instance extension slot");']
    for name,record in types.items():
        assertions.append(f'_Static_assert(sizeof({name})=={record["bytes"]},"extension record size");')
        assertions.extend(f'_Static_assert(offsetof({name},{m["name"]})=={m["offset"]},"extension member");' for m in record['members'])
    assertions.append('_Static_assert(sizeof(card_ext)==57600 && sizeof(disp_card_ext)==57600,"extension banks");')
    probe=candidate/'layout.c';probe.write_text('#include "manalink.h"\n#include <stddef.h>\n'+'\n'.join(assertions)+'\nconst card_ext_t recovery_probe = '+template_initializer+';\n')
    command=['C:/Program Files/LLVM/bin/clang.exe','-target','i686-pc-windows-msvc','-std=gnu99',
        '-Werror=implicit-function-declaration','-Werror=incompatible-pointer-types','-Werror=return-type','-I',str(candidate),'-I',str(root)]
    obj=candidate/'layout.obj';result=subprocess.run(command+['-c',str(probe),'-o',str(obj)],capture_output=True,text=True)
    (candidate/'layout.log').write_text(result.stderr);result.check_returncode()
    obj_bytes=obj.read_bytes();machine,count=struct.unpack_from('<HH',obj_bytes);optional=struct.unpack_from('<H',obj_bytes,16)[0];assert machine==0x14c
    sections=[]
    for i in range(count):
        at=20+optional+40*i;name=obj_bytes[at:at+8].rstrip(b'\0');size,pos=struct.unpack_from('<II',obj_bytes,at+16)
        if name==b'.rdata':sections.append(obj_bytes[pos:pos+size])
    assert sections==[template_bytes],'Compiled template bytes differ from the released 192-byte object'
    # The existing 300-byte template must remain byte-identical after giving
    # its former byte array the pointer/unused-word layout.
    text=sources['functions/deck.c'];begin=text.index('static card_instance_t card_instance_template =')
    begin=text.index('{',begin);end=text.index('\n};',begin)+2
    instance_probe=candidate/'instance-layout.c';instance_probe.write_text('#include "manalink.h"\nconst card_instance_t instance_probe = '+text[begin:end]+';\n')
    instance_obj=candidate/'instance-layout.obj'
    result=subprocess.run(command+['-c',str(instance_probe),'-o',str(instance_obj)],capture_output=True,text=True)
    (candidate/'instance-layout.log').write_text(result.stderr);result.check_returncode()
    instance_bytes=instance_obj.read_bytes();count=struct.unpack_from('<H',instance_bytes,2)[0];optional=struct.unpack_from('<H',instance_bytes,16)[0]
    instance_sections=[]
    for i in range(count):
        at=20+optional+40*i;name=instance_bytes[at:at+8].rstrip(b'\0');size,pos=struct.unpack_from('<II',instance_bytes,at+16)
        if name==b'.rdata':instance_sections.append(instance_bytes[pos:pos+size])
    native_instance=runtime.pe.get_data(0x27650a0-runtime.pe.OPTIONAL_HEADER.ImageBase,300)
    assert instance_sections==[native_instance],'Compiled instance template differs from release'
    for source,text in sources.items():
        path=candidate/Path(source).name;path.write_bytes(text.encode('latin1'))
        result=subprocess.run(command+['-fsyntax-only',str(path)],capture_output=True,text=True)
        (candidate/(Path(source).name+'.log')).write_text(result.stderr);result.check_returncode()
    if args.apply:
        for source,text in sources.items():(root/source).write_bytes(text.replace('\n','\r\n').encode('latin1'))
        (root/'defs.h').write_bytes(defs.replace('\n','\r\n').encode('latin1'));(root/'manalink.h').write_bytes(header.replace('\n','\r\n').encode('latin1'))
        manifest['accepted_functions'].extend(entries);manifest['accepted']=len(manifest['accepted_functions'])
        manifest['source_globals'].extend(declaration_globals);manifest['source_fragments'].extend(fragments)
        manifest['source_files']=dict(collections.Counter(f['source'] for f in manifest['accepted_functions']))
        for f in manifest['source_fragments']:manifest['source_files'].setdefault(f['source'],0)
        manifest_path.write_text(json.dumps(manifest,indent=2))
        for e in entries:
            row=exports[e['name']];shutil.copyfile(cache/'decompiled-extension-pair'/(e['address']+'.c'),cache/'decompiled-abi'/(e['address']+'.c'));canonical[e['address']]=row
        for address,row in supplemental.items():
            shutil.copyfile(cache/'decompiled-extension-pair'/(address+'.c'),cache/'decompiled-abi'/(address+'.c'));canonical[address]=row
        with canonical_path.open('w',newline='') as stream:
            writer=csv.DictWriter(stream,fieldnames=canonical_rows[0],delimiter='\t');writer.writeheader();writer.writerows(canonical.values())
        foundation.update(source_recovered=True,status='C storage, creation binding, AI snapshots, display lifecycle, ownership-copy fragment and packed information APIs recovered. Native hook/jump-table integration, flag-storage migration and complete save-format migration remain.',
            compiled_template_sha256=hashlib.sha256(template_bytes).hexdigest(),card_from_where_enum=enums,ai_copy_proof=copy_proof)
        foundation.update(compiled_instance_template_sha256=hashlib.sha256(native_instance).hexdigest(),instance_template_bytes_checked=300)
        (root/'docs/recovery-evidence/extension-foundation.json').write_text(json.dumps(foundation,indent=2))
    print(json.dumps(dict(applied=args.apply,new_functions=len(entries),globals=len(declaration_globals),source_fragments=len(fragments),compiled_template_bytes=len(template_bytes)),indent=2))


if __name__=='__main__':main()
