"""Record target widths and instruction evidence for scalar-word view adaptations."""
import csv
import hashlib
import json
import re
import subprocess
from pathlib import Path

from capstone import Cs, CS_ARCH_X86, CS_MODE_32, CS_GRP_CALL, CS_GRP_JUMP, CS_GRP_RET
from capstone.x86 import X86_OP_MEM, X86_OP_REG, X86_REG_EBP, X86_REG_ESP
from inspect_release import Image, value
from export_abi import type_of


def main():
    root = Path(__file__).resolve().parents[2]; cache = root / '.recovery'
    debug = Image(Path('G:/Manalink/ManalinkEh_10_22_Hotfix.dbg').read_bytes())
    runtime = Image((cache / '2022-ManalinkEh.dll').read_bytes())
    assert runtime.pe.FILE_HEADER.Machine == 0x14c and runtime.pe.OPTIONAL_HEADER.Magic == 0x10b
    addresses = dict(event_result=0x6205b4, trigger_cause_controller=0x62c17c)
    linker = (root / 'manalink.lds').read_text(); globals = {}
    for name, address in addresses.items():
        assert any(n == '_' + name and at == address and section == -1 for n, at, section, kind, storage in debug.symbols)
        match = re.search(r'_' + name + r'\s*=\s*(0x[0-9a-fA-F]+);', linker)
        assert match and int(match[1], 16) == address
    for cu in debug.dwarf().iter_CUs():
        for die in cu.get_top_DIE().iter_children():
            name = value(die, 'DW_AT_name')
            if die.tag != 'DW_TAG_variable' or name not in addresses or name in globals:
                continue
            typ = type_of(die)
            while typ.tag in {'DW_TAG_typedef', 'DW_TAG_const_type', 'DW_TAG_volatile_type'}:
                typ = type_of(typ)
            assert typ.tag == 'DW_TAG_base_type' and value(typ, 'DW_AT_byte_size') == 4
            globals[name] = dict(address=f'{addresses[name]:x}', bytes=4, die=hex(die.offset),
                                 dwarf_unit=value(cu.get_top_DIE(), 'DW_AT_name'))
        if len(globals) == 2:
            break
    assert len(globals) == 2
    manifest = json.loads((root / 'docs/recovery-evidence/recovered-functions.json').read_text())
    functions = {r['low_pc'].removeprefix('0x'): r for r in csv.DictReader((cache / '2022-functions.csv').open()) if r['low_pc']}
    disassembler = Cs(CS_ARCH_X86, CS_MODE_32); disassembler.detail = True
    copies = {}
    for function in manifest['accepted_functions']:
        if not any('Four-byte info-slot union payloads' in a for a in function.get('interface_adaptations', [])):
            continue
        row = functions[function['address']]; start = int(function['address'], 16)
        code = runtime.pe.get_data(start - runtime.pe.OPTIONAL_HEADER.ImageBase, int(row['high_pc'], 16) - start)
        instructions = list(disassembler.disasm(code, start)); pairs = []
        for index, load in enumerate(instructions):
            if load.mnemonic != 'mov' or len(load.operands) != 2 or load.operands[0].type != X86_OP_REG:
                continue
            memory = load.operands[1]
            if memory.type != X86_OP_MEM or memory.size != 4 or memory.mem.disp != 56 or memory.mem.base in {X86_REG_EBP, X86_REG_ESP}:
                continue
            register = load.operands[0].reg
            for store in instructions[index + 1:index + 7]:
                if any(store.group(g) for g in [CS_GRP_CALL, CS_GRP_JUMP, CS_GRP_RET]):
                    break
                operands = store.operands
                if (store.mnemonic == 'mov' and len(operands) == 2 and operands[0].type == X86_OP_MEM and
                    operands[0].size == 4 and (operands[0].mem.index == 0 or operands[0].mem.scale == 8) and
                    116 <= operands[0].mem.disp <= 264 and (operands[0].mem.disp - 116) % 4 == 0 and
                    operands[1].type == X86_OP_REG and operands[1].reg == register):
                    pairs.append(dict(load_address=f'{load.address:x}', load_bytes=load.bytes.hex(),
                                      store_address=f'{store.address:x}', store_bytes=store.bytes.hex(),
                                      target_member='player' if (operands[0].mem.disp - 116) % 8 == 0 else 'card',
                                      constant_target_index=(operands[0].mem.disp - 116) // 8))
                    break
                if register in store.regs_access()[1]:
                    break
        assert pairs, function['name']
        copies[function['address']] = dict(name=function['name'], source=function['source'], pairs=pairs,
                                          machine_sha256=hashlib.sha256(code).hexdigest())
    probe = cache / 'scalar-word-layout.c'
    probe.write_text('''#include "manalink.h"
_Static_assert(sizeof(event_result) == 4 && sizeof(trigger_cause_controller) == 4, "engine words");
_Static_assert(sizeof(((card_instance_t*)0)->info_slot) == 4, "info word");
_Static_assert(sizeof(target_t) == 8 && __builtin_offsetof(target_t, player) == 0 && __builtin_offsetof(target_t, card) == 4, "target words");
''')
    result = subprocess.run(['C:/Program Files/LLVM/bin/clang.exe', '-target', 'i686-pc-windows-msvc', '-std=gnu99',
        '-fsyntax-only', '-I', str(root), str(probe)], capture_output=True, text=True)
    (cache / 'scalar-word-layout.log').write_text(result.stderr); result.check_returncode()
    path = root / 'docs/recovery-evidence/interface-recovery.json'; interfaces = json.loads(path.read_text())
    interfaces['scalar_word_views'] = dict(globals=globals, copies=copies, little_endian_pe32=True,
        scope='Unsigned byte/halfword extraction from verified 32-bit globals. Info-slot union payloads copy into target words without changing their four-byte representation; no extension storage is projected.')
    path.write_text(json.dumps(interfaces, indent=2))
    print(json.dumps(dict(global_words=len(globals), copied_functions=len(copies)), indent=2))


if __name__ == '__main__':
    main()
