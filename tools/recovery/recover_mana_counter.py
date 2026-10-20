"""Recover the VLA mana-count pass and typed views of the native mana lists."""
import argparse
import csv
import hashlib
import json
import re
import shutil
import subprocess
from pathlib import Path
from capstone import Cs,CS_ARCH_X86,CS_MODE_32
from inspect_release import Image,value,inherited_value
from export_abi import type_of,type_name
from audit_source import function_body


BODY=r'''void count_mana_2022(void)
{
  for (int color = 0; color < 8; ++color)
  {
    raw_mana_available[1][color] = 0;
    raw_mana_available[0][color] = 0;
    mana_color_override[1][color] = color;
    mana_color_override[0][color] = color;
    mana_quantity_override[1][color] = -1;
    mana_quantity_override[0][color] = -1;
  }
  engine_raw_mana_available_hex_2022[0][0] = -1;
  engine_mana_may_be_spent_as_color_2022[1][0] = -1;
  engine_mana_may_be_spent_as_color_2022[0][0] = -1;
  engine_raw_mana_available_hex_2022[1][0] = -1;
  mana_colors_added[1] = 0;
  mana_colors_added[0] = 0;
  multi_mana_transmutation[1][1] = 0;
  multi_mana_transmutation[0][1] = 0;
  multi_mana_transmutation[1][0] = 0;
  multi_mana_transmutation[0][0] = 0;

  int capacity = active_cards_count[0];
  if (capacity < active_cards_count[1])
    capacity = active_cards_count[1];
  int mana_sources[2][capacity];
  memset(mana_sources, 0xff, sizeof(mana_sources));
  int source_counts[2] = {0, 0};
  for (int player = 0; player < 2; ++player)
  {
    remove_special_flags2_2022(player, -1, 0x40);
    dispatch_event(player, 0, EVENT_MODIFY_MANA_PROD);
    for (int card = 0; card < active_cards_count[player]; ++card)
    {
      card_instance_t *instance = in_play(player, card);
      if (instance != NULL)
      {
        if ((cards_data[instance->internal_card_id].extra_ability & 0x1000) != 0)
          mana_sources[player][source_counts[player]++] = card;
        uint16_t csvid = cards_data[instance->internal_card_id].id;
        if (csvid == 100 || (csvid == 0xee && (instance->state & 0x10) == 0))
          dispatch_event_to_single_card(player, card, EVENT_COUNT_MANA, -1, -1);
      }
      instance = in_hand(player, card);
      if (instance != NULL)
      {
        uint16_t csvid = cards_data[instance->internal_card_id].id;
        if (csvid == 0x8e3 || csvid == 0x2293)
          dispatch_event_to_single_card(player, card, EVENT_COUNT_MANA, -1, -1);
      }
    }
    if (is_id_in_grave(player, 0x176c) != 0 &&
        ((uint32_t)player_bits[player] & 0x02000000U) == 0)
      mana_colors_added[player] |= 0x28;
  }
  for (int player = 0; player < 2; ++player)
    for (int source = 0; source < source_counts[player]; ++source)
      dispatch_event(player, mana_sources[player][source], EVENT_COUNT_MANA);
}
'''


def main():
    parser=argparse.ArgumentParser(__doc__);parser.add_argument('--apply',action='store_true');args=parser.parse_args()
    root=Path(__file__).resolve().parents[2];cache=root/'.recovery';source='functions/engine.c';a='26a9b30'
    row=next(r for r in csv.DictReader((cache/'2022-functions.csv').open()) if r['low_pc']=='0x'+a and r['name']=='count_mana')
    image=Image((cache/'2022-ManalinkEh.dll').read_bytes());length=int(row['high_pc'],16)-int(a,16);machine=image.pe.get_data(int(a,16)-0x2000000,length)
    checks={'26a9b56':'89048560717902','26a9b5d':'89048540717902','26a9b64':'c70485e0717902ffffffff','26a9b6f':'c70485c0717902ffffffff',
        '26a9b7b':'83f808','26a9b80':'c7051cf24e00ffffffff','26a9b8a':'c705e0a46000ffffffff','26a9b94':'c705b4a46000ffffffff','26a9ba9':'c705e8f24e00ffffffff',
        '26a9bf5':'8d048d00000000','26a9bfc':'c1e103','26a9c04':'e8c7250500','26a9c09':'29c4','26a9c0e':'8d742414','26a9c14':'c1e902','26a9c1c':'f3ab',
        '26a9c37':'c744240840000000','26a9c3f':'c7442404ffffffff','26a9c4a':'e8c105fdff','26a9c4f':'c7442408a6000000','26a9c57':'c744240400000000','26a9c62':'e849bdd8fd',
        '26a9d82':'c74424046c170000','26a9d96':'f604bda7f14e0002','26a9da7':'83cb28'}
    for address,expected in checks.items():assert image.pe.get_data(int(address,16)-0x2000000,len(bytes.fromhex(expected))).hex()==expected
    debug_data=Path('G:/Manalink/ManalinkEh_10_22_Hotfix.dbg').read_bytes();debug=Image(debug_data);vla=None
    for cu in debug.dwarf().iter_CUs():
        if value(cu.get_top_DIE(),'DW_AT_name')!=source:continue
        def walk(die):
            nonlocal vla
            for child in die.iter_children():
                if child.tag=='DW_TAG_variable' and inherited_value(child,'DW_AT_name')=='mana_sources':
                    array=type_of(child);assert array.tag=='DW_TAG_array_type';ranges=[c for c in array.iter_children() if c.tag=='DW_TAG_subrange_type'];assert len(ranges)==2
                    assert value(ranges[0],'DW_AT_upper_bound')==1 and ranges[1].attributes['DW_AT_upper_bound'].form=='DW_FORM_ref4'
                    scalar=type_of(array);assert type_name(scalar)=='int' and value(scalar,'DW_AT_byte_size')==4
                    vla=dict(die=hex(child.offset),outer_count=2,element_bytes=4,dynamic_bound_die=hex(ranges[1].attributes['DW_AT_upper_bound'].value+cu.cu_offset))
                walk(child)
        for die in cu.get_top_DIE().iter_children():
            if value(die,'DW_AT_low_pc')==int(a,16):walk(die)
        break
    assert vla
    aliases={'engine_raw_mana_available_hex_2022':(0x4ef21c,'[2][51]'),'engine_mana_may_be_spent_as_color_2022':(0x60a4b4,'[2][11]')}
    assert '0x4ef21c\traw_mana_available_hex\tglobal\tint[2][51]' in (cache/'absolute-interfaces.tsv').read_text()
    original=(root/source).read_bytes().decode('latin1').replace('\r\n','\n');assert re.search(r'#define raw_mana_may_be_spent_as_color\s+EXE_2D_ARR\(int, 2, 11, 0x60a4b4\)',original)
    linker_path=root/'manalink.lds';linker=linker_path.read_bytes().decode('latin1').replace('\r\n','\n');assert re.search(r'_raw_mana_available_hex\s*=\s*0x4ef21c;',linker,re.I)
    for name in aliases:assert '_'+name not in linker
    linker+='\n/* Typed October 2022 views of existing native mana lists. */\n'+''.join(f'_{name} = 0x{address:X};\n' for name,(address,dims) in aliases.items())
    header=(root/'manalink.h').read_bytes().decode('latin1').replace('\r\n','\n');at=header.rfind('#ifdef __cplusplus')
    header=header[:at]+'/* October 2022 mana count pass and native list views. */\n#ifndef SHANDALAR\n'+''.join(f'extern int {name}{dims};\n' for name,(address,dims) in aliases.items())+'void count_mana_2022(void);\n#endif\n\n'+header[at:]
    body=subprocess.run(['C:/Program Files/LLVM/bin/clang-format.exe','-style={BasedOnStyle: LLVM, IndentWidth: 2, BreakBeforeBraces: Allman, ColumnLimit: 100}'],input=BODY.encode(),capture_output=True,check=True).stdout.decode().replace('\r\n','\n')
    candidate=original+f"\n\n/* ManalinkEh 2022: 0x{a}; original {source}:{row['line']}. */\n"+body
    checkdir=cache/'mana-counter-check';checkdir.mkdir(exist_ok=True);(checkdir/'manalink.h').write_bytes(header.encode('latin1'));scratch=checkdir/'engine.c';scratch.write_bytes(candidate.encode('latin1'))
    result=subprocess.run(['C:/Program Files/LLVM/bin/clang.exe','-target','i686-pc-windows-msvc','-std=gnu99','-fsyntax-only','-Werror=implicit-function-declaration','-Werror=incompatible-pointer-types','-Werror=return-type','-Werror=uninitialized','-Werror=sometimes-uninitialized','-Werror=format','-I',str(checkdir),'-I',str(root),str(scratch)],capture_output=True,text=True)
    (checkdir/'check.log').write_text(result.stderr);result.check_returncode()
    proof=dict(debug_sha256=hashlib.sha256(debug_data).hexdigest(),machine_sha256=hashlib.sha256(machine).hexdigest(),machine_bytes=length,machine_checks=checks,vla=vla,
        calls=[dict(address=f'{i.address:x}',bytes=i.bytes.hex(),target=i.op_str) for i in Cs(CS_ARCH_X86,CS_MODE_32).disasm(machine,int(a,16)) if i.mnemonic=='call'],native_aliases={n:dict(address=f'{v[0]:x}',dimensions=v[1]) for n,v in aliases.items()},
        scope='Typed two-row VLA with native max-active-card stride, exact modifier resets, special hand/battlefield/graveyard paths, then deferred per-source COUNT_MANA dispatch. Original native count_mana import retained.',
        integration_pending='Native hook routing and earlier source callers still require coordinated migration. No game execution performed.')
    if args.apply:
        manifest_path=root/'docs/recovery-evidence/recovered-functions.json';manifest=json.loads(manifest_path.read_text());assert a not in {f['address'] for f in manifest['accepted_functions']}
        backup=cache/'batch28-before/manalink.lds';assert not backup.exists();shutil.copyfile(linker_path,backup)
        (root/source).write_bytes(candidate.replace('\n','\r\n').encode('latin1'));(root/'manalink.h').write_bytes(header.replace('\n','\r\n').encode('latin1'));linker_path.write_bytes(linker.replace('\n','\r\n').encode('latin1'))
        raw=(cache/'decompiled-abi'/(a+'.c')).read_text();manifest['accepted_functions'].append(dict(name='count_mana_2022',original_name='count_mana',address=a,source=source,line=row['line'],static=False,prototype='void count_mana_2022(void);',raw_sha256=hashlib.sha256(raw.encode()).hexdigest(),recovered_literals=[],reconstruction=proof['scope'],mana_counter_evidence=proof,reconstruction_body_sha256=hashlib.sha256(function_body(body,'count_mana_2022').encode()).hexdigest()))
        manifest['accepted']=len(manifest['accepted_functions']);manifest_path.write_text(json.dumps(manifest,indent=2));p=root/'docs/recovery-evidence/interface-recovery.json';interfaces=json.loads(p.read_text());interfaces['mana_counter_2022']=proof;p.write_text(json.dumps(interfaces,indent=2))
    print(json.dumps(dict(applied=args.apply,new_helpers=1,vla_rows=2,native_list_aliases=2)))


if __name__=='__main__':main()
