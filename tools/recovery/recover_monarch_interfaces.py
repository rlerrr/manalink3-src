"""Expose existing Monarch helpers after verifying target public linkage/signatures."""
import json
import re
from pathlib import Path

from inspect_release import Image, inherited_value, value
from export_abi import type_of, type_name


def main():
    root = Path(__file__).resolve().parents[2]
    image = Image(Path('G:/Manalink/ManalinkEh_10_22_Hotfix.dbg').read_bytes())
    expected = {'who_is_the_monarch': ('220d450', 'int', []),
                'you_become_the_monarch': ('220d4f0', 'void', ['int'])}
    signatures, flags = {}, {}
    for cu in image.dwarf().iter_CUs():
        source = value(cu.get_top_DIE(), 'DW_AT_name')
        if source not in {'cards/conspiracy-take-the-crown.c', 'cards/commander_legends.c'}:
            continue
        for die in cu.iter_DIEs():
            name = inherited_value(die, 'DW_AT_name')
            if die.tag == 'DW_TAG_enumerator' and name == 'STATE_OUBLIETTED':
                flags[source] = value(die, 'DW_AT_const_value')
            if die.tag != 'DW_TAG_subprogram' or name not in expected:
                continue
            address, returns, parameters = expected[name]
            if value(die, 'DW_AT_low_pc') != int(address, 16):
                continue
            actual = (type_name(type_of(die)), [type_name(type_of(p)) for p in die.iter_children()
                                               if p.tag == 'DW_TAG_formal_parameter'])
            assert actual == (returns, parameters)
            symbols = [(n, storage) for n, offset, section, kind, storage in image.functions()
                       if image.pe.OPTIONAL_HEADER.ImageBase + image.pe.sections[section - 1].VirtualAddress + offset == int(address, 16)]
            assert ('_' + name, 2) in symbols
            signatures[name] = dict(address=address, returns=returns, parameters=parameters, coff_storage=2)
    assert len(signatures) == 2 and flags and set(flags.values()) == {0x800000}
    path = root / 'cards/conspiracy-take-the-crown.c'
    original = path.read_bytes()
    content = original
    for declaration in [b'static int who_is_the_monarch(void)', b'static void you_become_the_monarch(int player)']:
        assert content.count(declaration) == 1
        content = content.replace(declaration, declaration.removeprefix(b'static '))
    header = root / 'manalink.h'
    declarations = b'/* Existing Monarch helpers: public in the October 2022 symbols. */\r\nint who_is_the_monarch(void);\r\nvoid you_become_the_monarch(int player);\r\n\r\n'
    data = header.read_bytes()
    assert not re.search(rb'\bwho_is_the_monarch\s*\(', data)
    marker = data.rfind(b'#ifdef __cplusplus')
    path.write_bytes(content)
    header.write_bytes(data[:marker] + declarations + data[marker:])
    evidence_path = root / 'docs/recovery-evidence/interface-recovery.json'
    evidence = json.loads(evidence_path.read_text())
    evidence['existing_monarch_interfaces'] = dict(source='cards/conspiracy-take-the-crown.c', interfaces=signatures,
        change='Only static linkage removed and header declarations added; existing bodies retained and not counted as recovered target implementations.',
        integration_remaining='The target public wrapper also checks Jared Carthalion; target worker dispatch/behavior is not supplied by this linkage adaptation.')
    evidence['state_mask_normalization'] = dict(function='check_state', argument=3, value=0x800000,
                                                name='STATE_OUBLIETTED', dwarf_units=flags,
                                                scope='Only simple three-argument calls with this exact literal mask.')
    evidence_path.write_text(json.dumps(evidence, indent=2))
    print(json.dumps(dict(exposed=list(signatures), state_mask=flags), indent=2))


if __name__ == '__main__':
    main()
