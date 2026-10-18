"""Record the extension layout and real lifecycle references before migrating storage."""
import csv
import hashlib
import json
from pathlib import Path

import capstone
from inspect_release import Image,value,inherited_value
from export_abi import type_of,type_name


def main():
    root=Path(__file__).resolve().parents[2];cache=root/'.recovery'
    debug_data=Path('G:/Manalink/ManalinkEh_10_22_Hotfix.dbg').read_bytes();debug=Image(debug_data)
    runtime=Image((cache/'2022-ManalinkEh.dll').read_bytes())
    result=dict(debug_sha256=hashlib.sha256(debug_data).hexdigest(),types={},globals={},exports=[],references=[])
    names={'card_ext','disp_card_ext','card_ext_buffer1','card_ext_buffer2','card_ext_buffer3',
           'card_ext_template','backup_card_ext','backup0_card_ext'}
    def type_size(typ):
        while typ.tag in {'DW_TAG_typedef','DW_TAG_const_type','DW_TAG_volatile_type'}:typ=type_of(typ)
        if typ.tag=='DW_TAG_array_type':
            size=type_size(type_of(typ))
            for dimension in typ.iter_children():
                if dimension.tag=='DW_TAG_subrange_type':
                    size*=value(dimension,'DW_AT_upper_bound')+1-value(dimension,'DW_AT_lower_bound',0)
            return size
        size=value(typ,'DW_AT_byte_size');assert isinstance(size,int) and size>0
        return size
    for cu in debug.dwarf().iter_CUs():
        source=value(cu.get_top_DIE(),'DW_AT_name')
        if source not in {'functions/deck.c','functions/ai.c'}:continue
        for die in cu.get_top_DIE().iter_children():
            name=inherited_value(die,'DW_AT_name')
            if die.tag=='DW_TAG_typedef' and name in {'card_ext_t','inst_info_t','inst_stored_iid_t'}:
                structure=type_of(die)
                record=dict(bytes=value(structure,'DW_AT_byte_size'),members=[dict(name=value(c,'DW_AT_name'),
                    offset=value(c,'DW_AT_data_member_location'),type=type_name(type_of(c)),bytes=type_size(type_of(c)))
                    for c in structure.iter_children() if c.tag=='DW_TAG_member'])
                assert name not in result['types'] or result['types'][name]==record
                result['types'][name]=record
            if die.tag=='DW_TAG_variable' and name in names and value(die,'DW_AT_location'):
                result['globals'][name]=dict(type=type_name(type_of(die)),dwarf_die=hex(die.offset),source=source)
        cu._dielist.clear();cu._diemap.clear();type_name.cache_clear()
    assert {n:t['bytes'] for n,t in result['types'].items()}==dict(card_ext_t=192,inst_info_t=8,inst_stored_iid_t=4)
    assert set(result['globals'])==names
    for name,entry in result['globals'].items():
        symbols=[s for s in debug.symbols if s[0]=='_'+name];assert len(symbols)==1
        n,offset,section,kind,storage=symbols[0];segment=debug.pe.sections[section-1]
        address=debug.pe.OPTIONAL_HEADER.ImageBase+segment.VirtualAddress+offset
        entry.update(address=f'{address:x}',coff_storage_class=storage,section=segment.Name.rstrip(b'\0').decode())
        if name=='card_ext_template':
            assert not segment.Characteristics&0x80000000
            entry['bytes']=runtime.pe.get_data(address-runtime.pe.OPTIONAL_HEADER.ImageBase,192).hex()
    for row in csv.DictReader((cache/'decompiled-extension-core/export.tsv').open(),delimiter='\t'):
        assert row['status']=='ok'
        raw=(cache/'decompiled-extension-core'/(row['address']+'.c')).read_text()
        result['exports'].append(dict(row,raw_sha256=hashlib.sha256(raw.encode()).hexdigest()))
    assert len(result['exports'])==7
    decoder=capstone.Cs(capstone.CS_ARCH_X86,capstone.CS_MODE_32);decoder.detail=True
    globals_by_address={int(v['address'],16):n for n,v in result['globals'].items()}
    rows=list(csv.DictReader((cache/'2022-functions.csv').open()));seen=set()
    for row in rows:
        if not row['source'].endswith('.c') or not row['low_pc'] or not row['high_pc']:continue
        start,end=int(row['low_pc'],16),int(row['high_pc'],16)
        if start in seen or not 0x2000000<=start<end<0x2710000:continue
        seen.add(start)
        for ins in decoder.disasm(runtime.pe.get_data(start-runtime.pe.OPTIONAL_HEADER.ImageBase,end-start),start):
            for op in ins.operands:
                address=op.imm if op.type==capstone.x86.X86_OP_IMM else op.mem.disp if op.type==capstone.x86.X86_OP_MEM else None
                if address in globals_by_address:
                    result['references'].append(dict(function=row['name'],source=row['source'],function_address=f'{start:x}',
                        instruction_address=f'{ins.address:x}',bytes=ins.bytes.hex(),instruction=ins.mnemonic+' '+ins.op_str,
                        global_name=globals_by_address[address]))
    result['migration_requirements']=[
        'Recover the complete 192-byte record and its five live/display/scratch storage objects plus template and both AI snapshots.',
        'Migrate card_instance_t offset 0x2c together with creation, copying, displayed data, scratch buffers and native untap accesses; do not overlay a live pointer onto the old eight-byte field without those consumers.',
        'Preserve destination extension identity when copying a 300-byte instance and its 192-byte extension.',
        'Model inst_info as an eight-byte EDX:EAX value before exporting callers, not as a pointer.',
        'Recover initialization/reset, AI backup/restore and native hook sites coherently. Decide save-format integration only with the complete target supplemental record.',
        'Migrate special-flag and keyword readers/writers together rather than keeping incompatible old and new storage pathways.'
    ]
    path=root/'docs/recovery-evidence/extension-foundation.json'
    previous=json.loads(path.read_text()) if path.exists() else {}
    if previous.get('source_recovered'):
        for name,record in previous['types'].items():
            assert record['bytes']==result['types'][name]['bytes']
            assert [{k:v for k,v in m.items() if k!='bytes'} for m in record['members']]==[
                {k:v for k,v in m.items() if k!='bytes'} for m in result['types'][name]['members']]
        result={**previous,**result}
        result['status']=previous['status']
    else:
        result['status']='Investigation evidence only. Extension layout and live storage have not yet been migrated into the game source.'
    path.write_text(json.dumps(result,indent=2))
    print(json.dumps(dict(types=len(result['types']),globals=len(names),exports=len(result['exports']),
        referenced_functions=sorted({r['function'] for r in result['references']})),indent=2))


if __name__=='__main__':main()
