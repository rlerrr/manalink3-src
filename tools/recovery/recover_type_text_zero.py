"""Add the named empty type-text flag verified in the callee's DWARF unit."""
import argparse
import hashlib
import json
import re
import subprocess
from pathlib import Path
from inspect_release import Image,value
from export_abi import type_of


def main():
    parser=argparse.ArgumentParser(__doc__);parser.add_argument('--apply',action='store_true');args=parser.parse_args()
    root=Path(__file__).resolve().parents[2];data=Path('G:/Manalink/ManalinkEh_10_22_Hotfix.dbg').read_bytes();image=Image(data)
    units={};wanted={'functions/functions.c'}
    for cu in image.dwarf().iter_CUs():
        source=value(cu.get_top_DIE(),'DW_AT_name')
        if source not in wanted:continue
        for die in cu.get_top_DIE().iter_children():
            if die.tag=='DW_TAG_enumeration_type':
                enum=die
                members={value(c,'DW_AT_name'):value(c,'DW_AT_const_value') for c in enum.iter_children()}
                if 'TYPETEXT_0' not in members:continue
                assert value(enum,'DW_AT_byte_size')==4 and members=={'TYPETEXT_0':0,'TYPETEXT_NO_ABBREV':1,'TYPETEXT_INVERT':2}
                units[source]=dict(die=hex(enum.offset),bytes=4,members=members);break
        cu._dielist.clear();cu._diemap.clear()
    assert set(units)==wanted
    path=root/'defs.h';text=path.read_bytes().decode('latin1').replace('\r\n','\n');assert not re.search(r'\bTYPETEXT_0\s*=',text)
    text,count=re.subn(r'(typedef enum\s*\{)(\s*TYPETEXT_NO_ABBREV\b)',r'\1\n\tTYPETEXT_0 = 0, /* October 2022 name for no type-text flags. */\2',text);assert count==1
    probe=root/'.recovery/type-text-zero-check.c';probe.write_text('#include "manalink.h"\nenum { RECOVERY_TYPETEXT_0 = 0 };\n_Static_assert(sizeof(type_text_t)==4 && TYPETEXT_NO_ABBREV==1 && TYPETEXT_INVERT==2,"compatible type text flags");\n')
    subprocess.run(['C:/Program Files/LLVM/bin/clang.exe','-target','i686-pc-windows-msvc','-std=gnu99','-fsyntax-only','-I',str(root),str(probe)],check=True)
    if args.apply:
        path.write_bytes(text.replace('\n','\r\n').encode('latin1'))
        p=root/'docs/recovery-evidence/interface-recovery.json';interfaces=json.loads(p.read_text());interfaces['type_text_empty_flag']=dict(debug_sha256=hashlib.sha256(data).hexdigest(),units=units,value=0,scope='Add only the empty flag name; retain all original type-text flag values and members.');p.write_text(json.dumps(interfaces,indent=2))
    print(json.dumps(dict(applied=args.apply,value=0,verified_units=len(units))))


if __name__=='__main__':main()
