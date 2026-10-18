"""Verify and merge scoped integer info-slot decompilations for attacker-count handlers."""
import csv
import hashlib
import json
import shutil
from pathlib import Path

from capstone import Cs, CS_ARCH_X86, CS_MODE_32
from capstone.x86 import X86_OP_MEM, X86_OP_REG, X86_REG_EBP, X86_REG_ESP
from inspect_release import Image, value, inherited_value
from export_abi import type_of, type_name


def descendants(die):
    for child in die.iter_children():
        yield child
        yield from descendants(child)


def main():
    root = Path(__file__).resolve().parents[2]; cache = root / '.recovery'
    requests = [line.split('\t') for line in (cache / 'export-request-scalar-info-slots.tsv').read_text().splitlines()]
    wanted = {int(r[0], 16): r for r in requests}; assert len(wanted) == 8
    debug = Image(Path('G:/Manalink/ManalinkEh_10_22_Hotfix.dbg').read_bytes())
    runtime = Image((cache / '2022-ManalinkEh.dll').read_bytes())
    evidence = {}; union = None
    for cu in debug.dwarf().iter_CUs():
        source = value(cu.get_top_DIE(), 'DW_AT_name')
        if source not in {r[1] for r in requests}:
            continue
        for die in cu.get_top_DIE().iter_children():
            if die.tag == 'DW_TAG_typedef' and value(die, 'DW_AT_name') == 'card_instance_t':
                record = type_of(die)
                member = next(m for m in record.iter_children() if value(m, 'DW_AT_data_member_location') == 56)
                view = type_of(member)
                assert view.tag == 'DW_TAG_union_type' and value(view, 'DW_AT_byte_size') == 4
                fields = {value(m, 'DW_AT_name'): type_name(type_of(m)) for m in view.iter_children()}
                assert fields['info_slot'] in {'int', 'int32_t'} and 'info_slot_fn' in fields
                scalar = type_of(next(m for m in view.iter_children() if value(m, 'DW_AT_name') == 'info_slot'))
                while scalar.tag == 'DW_TAG_typedef':
                    scalar = type_of(scalar)
                assert scalar.tag == 'DW_TAG_base_type' and value(scalar, 'DW_AT_byte_size') == 4 and value(scalar, 'DW_AT_encoding') == 5
                union = dict(offset=56, bytes=4, members=fields)
            at = value(die, 'DW_AT_low_pc')
            if at not in wanted:
                continue
            row = wanted[at]; assert inherited_value(die, 'DW_AT_name') == row[2]
            locals = [(inherited_value(v, 'DW_AT_name'), type_name(type_of(v))) for v in descendants(die)
                      if v.tag == 'DW_TAG_variable' and inherited_value(v, 'DW_AT_name') in {'amt', 'attackers'}]
            assert ('amt', 'int') in locals
            if at != 0x23ddcf0:
                assert ('attackers', 'unsigned char *') in locals
            evidence[row[0]] = dict(name=row[2], source=source, locals=locals, bytes=value(die, 'DW_AT_high_pc'))
    assert union is not None and len(evidence) == 8
    disassembler = Cs(CS_ARCH_X86, CS_MODE_32); disassembler.detail = True
    for address, item in evidence.items():
        at = int(address, 16); code = runtime.pe.get_data(at - runtime.pe.OPTIONAL_HEADER.ImageBase, item['bytes'])
        loads = []; indices = []
        for ins in disassembler.disasm(code, at):
            if len(ins.operands) == 2 and ins.operands[0].type == X86_OP_REG and ins.operands[1].type == X86_OP_MEM:
                memory = ins.operands[1]
                row = dict(address=f'{ins.address:x}', bytes=ins.bytes.hex(), instruction=ins.mnemonic + ' ' + ins.op_str)
                if memory.size == 4 and memory.mem.disp == 56 and memory.mem.base not in {X86_REG_EBP, X86_REG_ESP}:
                    loads.append(row)
                if memory.size == 1 and memory.mem.disp == 132 and memory.mem.index != 0:
                    indices.append(row)
        assert loads and (indices or address == '23ddcf0')
        item.update(count_loads=loads, attacker_byte_loads=indices, machine_sha256=hashlib.sha256(code).hexdigest())
    log = (cache / 'scalar-info-slots-console.log').read_text()
    assert 'RECOVERY_SCALAR_INFO_SLOTS_COMPLETE count=8' in log
    assert 'RECOVERY_EXPORT_COMPLETE success=8 failed=0' in log and 'ERROR' not in log
    facets = list(csv.DictReader((cache / 'scalar-info-slot-facets.tsv').open(), delimiter='\t'))
    assert {int(r['function'], 16) for r in facets} == set(wanted)
    path = root / 'docs/recovery-evidence/recovered-functions.json'; manifest = json.loads(path.read_text())
    accepted = {f['address']: f for f in manifest['accepted_functions']}
    path = cache / 'decompiled-abi/export.tsv'; rows = list(csv.DictReader(path.open(), delimiter='\t'))
    replacements = {r['address']: r for r in csv.DictReader((cache / 'decompiled-scalar-info-slots/export.tsv').open(), delimiter='\t')}
    assert set(replacements) == set(evidence)
    for address in replacements:
        raw = (cache / 'decompiled-scalar-info-slots' / (address + '.c')).read_text()
        assert 'info_slot_fn' not in raw and 'code *' not in raw
        if address in accepted:
            assert address == '23ddcf0' and accepted[address]['raw_sha256'] == hashlib.sha256(raw.encode()).hexdigest()
        shutil.copyfile(cache / 'decompiled-scalar-info-slots' / (address + '.c'), cache / 'decompiled-abi' / (address + '.c'))
    with path.open('w', newline='') as stream:
        writer = csv.DictWriter(stream, fieldnames=rows[0], delimiter='\t'); writer.writeheader()
        writer.writerows(replacements.get(row['address'], row) for row in rows)
    path = root / 'docs/recovery-evidence/interface-recovery.json'; interfaces = json.loads(path.read_text())
    interfaces['scalar_attacker_info_slots'] = dict(union=union, functions=evidence, facets=facets,
        scope='Per-access union hints for verified integer counts; the global union and real function-pointer uses remain intact. Tenured Inkcaster also receives verified overlapping stack storage.')
    path.write_text(json.dumps(interfaces, indent=2))
    print(json.dumps(dict(merged=len(evidence), facets=len(facets)), indent=2))


if __name__ == '__main__':
    main()
