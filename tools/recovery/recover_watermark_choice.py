"""Recover the private paged chooser and watermark VLA with its missing vararg."""
import argparse
import csv
import hashlib
import json
import subprocess
from pathlib import Path

from inspect_release import Image, value, inherited_value
from export_abi import type_of, type_name
from promote_recovery import clean
from recover_literals import readonly_string


def main():
    parser = argparse.ArgumentParser(__doc__)
    parser.add_argument('--apply', action='store_true')
    args = parser.parse_args()
    root = Path(__file__).resolve().parents[2]; cache = root / '.recovery'
    debug = Image(Path('G:/Manalink/ManalinkEh_10_22_Hotfix.dbg').read_bytes())
    runtime = Image((cache / '2022-ManalinkEh.dll').read_bytes())
    addresses = {'25c4d00': 'do_dialog_lots_of_choices', '25c4fd0': 'choose_a_watermark'}
    evidence = {}
    for cu in debug.dwarf().iter_CUs():
        if value(cu.get_top_DIE(), 'DW_AT_name') != 'cards/unstable.c':
            continue
        for die in cu.get_top_DIE().iter_children():
            low = value(die, 'DW_AT_low_pc')
            if low not in {int(a, 16) for a in addresses}:
                continue
            address = f'{low:x}'; assert inherited_value(die, 'DW_AT_name') == addresses[address]
            evidence[address] = dict(line=str(value(die, 'DW_AT_decl_line')), size=value(die, 'DW_AT_high_pc'),
                locals={inherited_value(c, 'DW_AT_name'): type_name(type_of(c))
                        for c in die.iter_children() if c.tag == 'DW_TAG_variable'})
        break
    assert evidence['25c4fd0']['locals']['idxs'] == 'int[256]'
    assert evidence['25c4fd0']['locals']['choices'].startswith('char *[')
    for address, name in addresses.items():
        assert any(n == '_' + name and storage == 3 and debug.pe.OPTIONAL_HEADER.ImageBase +
            debug.pe.sections[s - 1].VirtualAddress + off == int(address, 16)
            for n, off, s, kind, storage in debug.functions())
    def read(at, size):
        rva = at - runtime.pe.OPTIONAL_HEADER.ImageBase
        section = next(s for s in runtime.pe.sections if s.VirtualAddress <= rva < s.VirtualAddress + s.Misc_VirtualSize)
        offset = section.PointerToRawData + rva - section.VirtualAddress
        return runtime.data[offset:offset + size]
    checks = {
        '25c5006': '80780f00',                  # saturation byte
        '25c5015': '83c01081f9ff000000',        # stride 16, indices < 255
        '25c5036': '8d049500000000',            # choices count * sizeof(char*)
        '25c5043': 'e88871130029c4',            # probe, then VLA allocation
        '25c505d': 'c1e1048b4c0b04890c87',      # choices[i] = watermark[idxs[i]].disp
        '25c5081': 'e87afcffff',                # paged chooser call
        '25c5086': '8b9c85f4fbffff',            # return idxs[choice]
        '25c50ad': 'e80ed10e00',                # recorded_rand(player, count)
        '25c50c2': '89d88b1540417702c1e004',    # selected watermark * 16
        '25c50d3': '01d08b4004',                # selected display text at offset 4
        '25c50d8': 'c7442404ba7c750289442408893c24', # format and missing third arg
        '25c50e7': 'e804cd1300',                # sprintf thunk
        '25c5129': 'ff1578537b02',              # lazy cache import
        '25c513e': 'bbff000000',                # no active watermarks -> NONE
    }
    for at, code in checks.items():
        assert read(int(at, 16), len(bytes.fromhex(code))) == bytes.fromhex(code)
    assert readonly_string(runtime, 0x2757cba) == b' %s\n'
    evidence['25c4fd0']['machine_checks'] = checks
    evidence['25c4fd0']['scope'] = 'Array holds indices 0..254 with name and nonzero saturation; VLA has one display pointer per retained entry. AI/speculation uses recorded_rand. Reconstruct omitted sprintf third argument from explicit [esp+8] store.'
    raw0 = (cache / 'decompiled-abi/25c4d00.c').read_text()
    body0 = 'static ' + clean(raw0)
    assert 'const char *buffer;' in body0 and 'buffer = buf + iVar1 + 7;' in body0
    assert 'buffer = buffer + iVar2;' in body0
    body0 = body0.replace('const char *buffer;', 'char *buffer;')
    evidence['25c4d00']['writable_alias'] = dict(local='buffer', backing_array='char buf[3000]',
        assignments=['buffer = buf + iVar1 + 7;', 'buffer = buffer + iVar2;'],
        scope='Writable pointer into the local character array; preserve standard sprintf destination type.')
    raw1 = (cache / 'decompiled-abi/25c4fd0.c').read_text()
    assert 'sprintf(buf," %s\\n");' in raw1
    body1 = '''static watermark_t choose_a_watermark(int player, int src_player, int src_card)
{
  if (watermarks == NULL)
    watermarks = get_watermarks();
  int idxs[256];
  int num_idxs = 0;
  for (int i = 0; i < 255; ++i)
    if (watermarks[i].name != NULL && watermarks[i].saturation != '\\0')
      idxs[num_idxs++] = i;
  if (num_idxs > 0)
  {
    int choice;
    if (ai_is_speculating == 1 || (player == 1 && ((unsigned char)trace_mode & 2) == 0))
    {
      choice = recorded_rand(player, num_idxs);
      if (ai_is_speculating != 1)
      {
        char buf[300];
        sprintf(buf, " %s\\n", watermarks[idxs[choice]].disp);
        do_dialog(player, src_player, src_card, -1, -1, buf, 0);
      }
    }
    else
    {
      char* choices[num_idxs];
      for (int i = 0; i < num_idxs; ++i)
        choices[i] = watermarks[idxs[i]].disp;
      choice = do_dialog_lots_of_choices(player, src_player, src_card, choices, num_idxs);
    }
    return (watermark_t)idxs[choice];
  }
  return WMARK_NONE;
}
'''
    entries = []; addition = ''
    for (address, name), body, raw in zip(addresses.items(), [body0, body1], [raw0, raw1]):
        evidence[address]['machine_sha256'] = hashlib.sha256(read(int(address, 16), evidence[address]['size'])).hexdigest()
        body = subprocess.run(['C:/Program Files/LLVM/bin/clang-format.exe',
            '-style={BasedOnStyle: LLVM, IndentWidth: 2, BreakBeforeBraces: Allman, ColumnLimit: 100}'],
            input=body.encode(), capture_output=True, check=True).stdout.decode()
        line = evidence[address]['line']
        addition += f'\n\n/* ManalinkEh 2022: 0x{address}; original cards/unstable.c:{line}. */\n' + body
        entries.append(dict(name=name, address=address, source='cards/unstable.c', line=line, static=True,
            prototype=body[:body.index('{')].strip() + ';', raw_sha256=hashlib.sha256(raw.encode()).hexdigest(),
            recovered_literals=[], reconstruction=evidence[address].get('scope', evidence[address].get('writable_alias')),
            interface_evidence=evidence[address]))
    source = root / 'cards/unstable.c'; original = source.read_bytes()
    assert b'int do_dialog_lots_of_choices(' not in original and b'watermark_t choose_a_watermark(' not in original
    candidate = cache / 'watermark-choice-candidate.c'; candidate.write_bytes(original + addition.encode())
    command = ['C:/Program Files/LLVM/bin/clang.exe', '-target', 'i686-pc-windows-msvc', '-std=gnu99', '-fsyntax-only',
        '-Werror=implicit-function-declaration', '-Werror=incompatible-pointer-types', '-Werror=return-type',
        '-Werror=uninitialized', '-Werror=sometimes-uninitialized', '-Werror=format', '-I', str(root), str(candidate)]
    result = subprocess.run(command, capture_output=True, text=True)
    (cache / 'watermark-choice-check.log').write_text(result.stderr); result.check_returncode()
    if args.apply:
        path = root / 'docs/recovery-evidence/recovered-functions.json'; manifest = json.loads(path.read_text())
        assert not {f['address'] for f in manifest['accepted_functions']} & set(addresses)
        source.write_bytes(original.rstrip() + addition.replace('\r\n', '\n').replace('\n', '\r\n').encode())
        manifest['accepted_functions'].extend(entries); manifest['accepted'] = len(manifest['accepted_functions'])
        path.write_text(json.dumps(manifest, indent=2))
        path = root / 'docs/recovery-evidence/interface-recovery.json'; interfaces = json.loads(path.read_text())
        interfaces['watermark_choice_reconstruction'] = evidence; path.write_text(json.dumps(interfaces, indent=2))
    print(json.dumps(dict(applied=args.apply, helpers=list(addresses.values())), indent=2))


if __name__ == '__main__':
    main()
