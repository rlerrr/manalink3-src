"""Recover Reconstruct History's five-element endpoint from its stack layout."""
import hashlib
import json
from pathlib import Path
from elftools.dwarf.dwarf_expr import DWARFExprParser
from inspect_release import Image,value,inherited_value
from export_abi import type_of,type_name


def main():
    root=Path(__file__).resolve().parents[2];data=Path('G:/Manalink/ManalinkEh_10_22_Hotfix.dbg').read_bytes();debug=Image(data);dwarf=debug.dwarf();parser=DWARFExprParser(dwarf.structs);layout={}
    for cu in dwarf.iter_CUs():
        if value(cu.get_top_DIE(),'DW_AT_name')!='cards/strixhaven.c':continue
        def walk(die):
            for child in die.iter_children():
                name=inherited_value(child,'DW_AT_name')
                if child.tag=='DW_TAG_variable' and name in {'types','msg'}:
                    expression=value(child,'DW_AT_location');ops=parser.parse_expr(expression)
                    assert len(ops)==1 and ops[0].op_name=='DW_OP_fbreg'
                    layout[name]=dict(type=type_name(type_of(child)),frame_offset=ops[0].args[0],die=hex(child.offset),expression=expression)
                walk(child)
        for die in cu.get_top_DIE().iter_children():
            if value(die,'DW_AT_low_pc')==0x23d9080:walk(die)
        break
    assert layout['types']['type']=='int[5]' and layout['msg']['type']=='char[100]'
    assert layout['msg']['frame_offset']-layout['types']['frame_offset']==20
    runtime=Image((root/'.recovery/2022-ManalinkEh.dll').read_bytes())
    checks={'23d90f1':'8d8da4feffff','23d9138':'83c304','23d913b':'8d85b8feffff','23d9141':'39c3','23d9143':'0f84bf010000'}
    for address,expected in checks.items():assert runtime.pe.get_data(int(address,16)-0x2000000,len(bytes.fromhex(expected))).hex()==expected
    assert layout['types']['frame_offset']+8==-0x15c and layout['msg']['frame_offset']+8==-0x148
    raw=(root/'.recovery/decompiled-abi/23d9080.c').read_text();assert raw.count('piVar3 == (int *)msg')==1
    p=root/'docs/recovery-evidence/local-enums.json';saved=json.loads(p.read_text());assert saved['debug_sha256']==hashlib.sha256(data).hexdigest()
    saved['functions']['23d9080']=dict(source='cards/strixhaven.c',name='card_reconstruct_history',types=[],declarations=[],array_boundaries=[dict(original='piVar3 == (int *)msg',replacement='piVar3 == types + 5')],array_boundary_evidence=dict(layout=layout,machine_checks=checks,raw_sha256=hashlib.sha256(raw.encode()).hexdigest(),scope='Five four-byte types end at the adjacent message buffer; express the endpoint as the one-past-array pointer without relying on C local placement.'))
    p.write_text(json.dumps(saved,indent=2));print('Prepared five-element endpoint with DWARF and loop-instruction proof')


if __name__=='__main__':main()
