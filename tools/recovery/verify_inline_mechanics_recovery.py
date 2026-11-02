"""Verify restored inline mechanics, enum operators and implicit C++ members."""
import json
from pathlib import Path
from capstone import Cs, CS_ARCH_X86, CS_MODE_32
from canonicalize_helpers import ROOT, read, sha, span
from verify_additional_local_names import source_hash_matches
from inspect_release import Image, value, inherited_value
from global_initializers import inherited_type
from object_initializers import unqualify
from verify_inline_helpers import ranges
from verify_split_cards_recovery import contract
from verify_inline_wrapper_recovery import verify as verify_definitions


def type_shape(d):
    d = unqualify(d)
    if d.tag == 'DW_TAG_reference_type':
        return dict(kind='reference', size=value(d, 'DW_AT_byte_size'), to=type_shape(inherited_type(d)))
    if d.tag == 'DW_TAG_enumeration_type':
        values = [(value(c, 'DW_AT_name'), value(c, 'DW_AT_const_value'))
                  for c in d.iter_children() if c.tag == 'DW_TAG_enumerator']
        return dict(kind='enum', size=value(d, 'DW_AT_byte_size'),
                    values_sha256=sha(json.dumps(values).encode()))
    assert d.tag == 'DW_TAG_structure_type', d.tag
    fields = [c for c in d.iter_children() if c.tag == 'DW_TAG_member']
    assert len(fields) == 1 and value(fields[0], 'DW_AT_name') == 't'
    return dict(kind='SafeBool', name=value(d, 'DW_AT_name'), size=value(d, 'DW_AT_byte_size'),
                field_offset=value(fields[0], 'DW_AT_data_member_location'),
                field_type=type_shape(inherited_type(fields[0])))


def operator_types(d):
    return dict(result=type_shape(inherited_type(d)),
                parameters=[type_shape(inherited_type(p)) for p in d.iter_children()
                            if p.tag == 'DW_TAG_formal_parameter'])


def class_shape(t):
    return dict(class_size=value(t, 'DW_AT_byte_size'), members=[
        dict(name=value(c, 'DW_AT_name'), offset=value(c, 'DW_AT_data_member_location'),
             type=value(unqualify(inherited_type(c)), 'DW_AT_name'))
        for c in t.iter_children() if c.tag == 'DW_TAG_member'])


def compiled_classes(dw):
    found = {}
    for cu in dw.iter_CUs():
        top = cu.get_top_DIE()
        name = value(top, 'DW_AT_name') or ''
        if name.replace('\\', '/').split('/')[-1] != 'utility.cpp':
            continue
        for d in top.iter_children():
            name = value(d, 'DW_AT_name')
            if name in ['SetVoidptr_t', 'UnorderedMapIntInt_t'] and value(d, 'DW_AT_byte_size') is not None:
                assert name not in found
                found[name] = class_shape(d)
    assert set(found) == {'SetVoidptr_t', 'UnorderedMapIntInt_t'}
    return found


def verify_origins(origins, image, dw, root, proof=None):
    md = Cs(CS_ARCH_X86, CS_MODE_32)
    instances = fragments = 0
    owners = set()
    for origin in origins:
        d = dw.get_DIE_from_refaddr(int(origin['die_offset'], 16))
        assert d.tag == 'DW_TAG_subprogram' and not ranges(dw, d)
        assert inherited_value(d, 'DW_AT_name') == origin['name']
        assert value(d, 'DW_AT_inline') == origin['inline']
        assert contract(dw, d) == origin['contract']
        for witness in origin['instances']:
            inline = dw.get_DIE_from_refaddr(int(witness['die_offset'], 16))
            assert inline.tag == 'DW_TAG_inlined_subroutine'
            assert inline.get_DIE_from_attribute('DW_AT_abstract_origin').offset == d.offset
            assert contract(dw, inline) == witness['contract']
            parent = dw.get_DIE_from_refaddr(int(witness['owner_die_offset'], 16))
            actual = inline.get_parent()
            while actual and actual.tag != 'DW_TAG_subprogram':
                actual = actual.get_parent()
            assert actual.offset == parent.offset
            assert inherited_value(parent, 'DW_AT_name') == witness['owner']
            bounds = ranges(dw, parent)
            assert bounds == [(int(p['address'], 16), int(p['end'], 16)) for p in witness['native_owner_ranges']]
            for p, (lo, hi) in zip(witness['native_owner_ranges'], bounds):
                assert sha(image.pe.get_data(lo - 0x2000000, hi - lo)) == p['sha256']
            source = witness['source_owner']
            t = read(root / source['source'])
            a, b = span(t, source['name'])
            assert source_hash_matches(proof,root,source['source'],source['name'],sha(t[a:b].encode()),source['definition_sha256'])
            parts = ranges(dw, inline)
            assert parts == [(int(p['address'], 16), int(p['end'], 16)) for p in witness['ranges']]
            for p, (lo, hi) in zip(witness['ranges'], parts):
                assert any(a <= lo < hi <= b for a, b in bounds)
                raw = image.pe.get_data(lo - 0x2000000, hi - lo)
                assert raw.hex() == p['bytes'] and sha(raw) == p['sha256']
                ins = list(md.disasm(raw, lo))
                assert sum(i.size for i in ins) == hi - lo
                assert [[hex(i.address), i.mnemonic, i.op_str] for i in ins] == p['instructions']
                fragments += 1
            instances += 1
            owners.add((origin['source'], parent.offset))
    return dict(instances=instances, fragments=fragments, callers=len(owners))


def verify(proof, image, root=ROOT):
    path = proof.get('native_inline_mechanics_recovery')
    if not path:
        return {}
    result = verify_definitions(proof, image, root, record_key='native_inline_mechanics_recovery',
                                expected_count=14, edit_key='native_inline_mechanics_extension')
    record = json.loads((root / path).read_bytes())
    debug = Image(Path('G:/Manalink/ManalinkEh_10_22_Hotfix.dbg').read_bytes())
    assert sha(debug.data) == record['native_dbg_sha256']
    dw = debug.dwarf()
    operators = record['cpp_operators']
    assert operators['source_append'] in read(root / 'functions/choose_a_card.cpp')
    assert read(root / 'functions/choose_a_card.cpp').count('OP_BITWISE(color_test_t)') == 1
    assert read(root / 'functions/choose_a_card.cpp').count('OP_BITWISE(type_t)') == 1
    assert operators['macro_definition'] in read(root / 'defs.h')
    assert sha(operators['macro_definition'].encode()) == operators['macro_sha256']
    enum_shapes = {}
    overloads = set()
    for origin in operators['native_origins']:
        d = dw.get_DIE_from_refaddr(int(origin['die_offset'], 16))
        assert value(d, 'DW_AT_inline') == 3 and inherited_value(d, 'DW_AT_external')
        shape = operator_types(d)
        assert shape == origin['native_type_shapes']
        enum_name = origin['contract']['parameters'][1]['type']
        assert enum_name in ['type_t', 'color_test_t']
        assert origin['line'] == (12 if enum_name == 'type_t' else 11)
        rhs = shape['parameters'][1]
        assert rhs['kind'] == 'enum' and rhs['size'] == 4
        enum_shapes.setdefault(enum_name, rhs)
        assert enum_shapes[enum_name] == rhs
        if origin['name'] in ['operator&=', 'operator|=']:
            assert shape['result'] == shape['parameters'][0] == dict(kind='reference', size=4, to=rhs)
        else:
            assert shape['parameters'][0] == rhs
            if origin['name'] == 'operator|':
                assert shape['result'] == rhs
            else:
                assert origin['name'] == 'operator&'
                assert shape['result'] == dict(kind='SafeBool', name='SafeBool<' + enum_name + '>',
                                               size=4, field_offset=0, field_type=rhs)
        overloads.add((origin['name'], enum_name))
    assert len(overloads) == len(operators['native_origins']) == 8
    result['cpp_operator_witnesses'] = verify_origins(operators['native_origins'], image, dw, root, proof)
    rebuilt = compiled_classes(Image((root / 'ManalinkEh.dbg').read_bytes()).dwarf())
    members = record['cpp_implicit_members']
    assert len(members) == 3
    for row in members:
        origin = row['native_origin']
        d = dw.get_DIE_from_refaddr(int(origin['die_offset'], 16))
        assert inherited_value(d, 'DW_AT_artificial') and value(d, 'DW_AT_inline') == 2
        this = next(c for c in d.iter_children() if value(c, 'DW_AT_name') == 'this')
        t = unqualify(inherited_type(unqualify(inherited_type(this))))
        assert value(t, 'DW_AT_name') == row['class_name']
        assert class_shape(t) == row['class_shape'] == rebuilt[row['class_name']]
        assert row['source_class_definition'] in read(root / origin['source'])
    result['cpp_implicit_member_witnesses'] = verify_origins([r['native_origin'] for r in members], image, dw, root, proof)
    result.update(restored_cpp_operator_overloads=8, restored_cpp_operator_names=4,
                  accounted_implicit_cpp_members=3, remaining_inline_review_names=39)
    return result


if __name__ == '__main__':
    from canonicalize_helpers import load
    print(json.dumps(verify(load(), Image((ROOT / '.recovery/2022-ManalinkEh.dll').read_bytes())), indent=2))
