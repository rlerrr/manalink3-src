"""Check restored inline wrappers and their complete native caller witnesses.

These checks establish provenance, interfaces, recorded instructions and build
identity. The per-helper reconstruction notes state the semantic review and its
limits; hashes alone do not establish general behavioral equivalence.
"""
import json
from pathlib import Path
from capstone import Cs, CS_ARCH_X86, CS_MODE_32
from canonicalize_helpers import ROOT, read, sha, span
from verify_additional_local_names import source_hash_matches
from inspect_release import Image, value, inherited_value
from verify_inline_helpers import ranges
from verify_split_cards_recovery import contract
from global_initializers import inherited_type
from object_initializers import unqualify
from verify_native_absolute_bindings import runtime_sections, stable_sections


def callback_contract(die):
    pointer = unqualify(inherited_type(die))
    assert pointer.tag == 'DW_TAG_pointer_type'
    function = unqualify(inherited_type(pointer))
    assert function.tag == 'DW_TAG_subroutine_type'
    result = unqualify(inherited_type(function))
    assert result.tag == 'DW_TAG_base_type' and value(result, 'DW_AT_name') == 'int'
    params = [unqualify(inherited_type(c)) for c in function.iter_children()
              if c.tag == 'DW_TAG_formal_parameter']
    assert len(params) == 3
    assert all(t.tag == 'DW_TAG_base_type' and value(t, 'DW_AT_name') == 'int' for t in params[:2])
    assert params[2].tag == 'DW_TAG_enumeration_type' and value(params[2], 'DW_AT_byte_size') == 4
    enumerators = [(value(c, 'DW_AT_name'), value(c, 'DW_AT_const_value'))
                   for c in params[2].iter_children() if c.tag == 'DW_TAG_enumerator']
    return dict(result='int', parameters=['int', 'int', 'event enum32'],
                event_enumerators_sha256=sha(json.dumps(enumerators).encode()))


def verify(proof, image, root=ROOT, *, record_key='native_inline_wrapper_recovery',
           expected_count=23, edit_key='native_inline_wrapper_extension', runtime_check=None):
    path = proof.get(record_key)
    if not path:
        return {}
    record = json.loads((root / path).read_bytes())
    assert sha(image.data) == record['native_dll_sha256']
    debug = Image(Path('G:/Manalink/ManalinkEh_10_22_Hotfix.dbg').read_bytes())
    assert sha(debug.data) == record['native_dbg_sha256']
    dw = debug.dwarf()
    md = Cs(CS_ARCH_X86, CS_MODE_32)
    instances = fragments = 0
    owners = set()
    for row in record['helpers']:
        text = read(root / row['source'])
        a, b = span(text, row['name'])
        assert sha(row['definition'].encode()) == row['definition_sha256']
        if not source_hash_matches(proof,root,row['source'],row['name'],sha(text[a:b].encode()),row['definition_sha256']):
            from verify_global_index_recovery import has_source_repair
            from verify_token_aggregate_recovery import has_source_repair as aggregate_repair
            if aggregate_repair(proof,root,row['source'],row['name'],row['definition_sha256']):
                from verify_token_aggregate_recovery import verify_superseded_definition
            elif has_source_repair(proof,root,row['source'],row['name'],row['definition_sha256']):
                from verify_global_index_recovery import verify_superseded_definition
            else:
                from verify_selection_storage_recovery import verify_superseded_definition
            verify_superseded_definition(proof,image,root,row['source'],row['name'],row['definition_sha256'])
        assert row['reconstruction_notes']
        for origin in row['native_origins']:
            d = dw.get_DIE_from_refaddr(int(origin['die_offset'], 16))
            assert d.tag == 'DW_TAG_subprogram' and not ranges(dw, d)
            assert value(d, 'DW_AT_inline') == origin['inline'] == 1
            assert not value(d, 'DW_AT_external', False)
            assert inherited_value(d, 'DW_AT_name') == row['name']
            assert contract(dw, d) == origin['contract']
            if 'callback_contract' in origin:
                params = [p for p in d.iter_children() if value(p, 'DW_AT_name') == 'func_ptr']
                assert len(params) == 1 and callback_contract(params[0]) == origin['callback_contract']
                assert 'typedef int (*CardFunction)(int, int, event_t);' in read(root / 'defs.h')
            for witness in origin['instances']:
                inline = dw.get_DIE_from_refaddr(int(witness['die_offset'], 16))
                assert inline.tag == 'DW_TAG_inlined_subroutine'
                assert inline.get_DIE_from_attribute('DW_AT_abstract_origin').offset == d.offset
                assert contract(dw, inline) == witness['contract']
                parent = dw.get_DIE_from_refaddr(int(witness['owner_die_offset'], 16))
                owner_ranges = ranges(dw, parent)
                actual = inline.get_parent()
                while actual and actual.tag != 'DW_TAG_subprogram':
                    actual = actual.get_parent()
                assert actual.offset == parent.offset
                assert inherited_value(parent, 'DW_AT_name') == witness['owner']
                assert owner_ranges == [(int(p['address'], 16), int(p['end'], 16))
                                        for p in witness['native_owner_ranges']]
                for p, (lo, hi) in zip(witness['native_owner_ranges'], owner_ranges):
                    assert sha(image.pe.get_data(lo - 0x2000000, hi - lo)) == p['sha256']
                source_owner = witness['source_owner']
                t = read(root / source_owner['source'])
                x, y = span(t, source_owner['name'])
                if not source_hash_matches(proof,root,source_owner['source'],source_owner['name'],sha(t[x:y].encode()),source_owner['definition_sha256']):
                    from verify_global_index_recovery import has_source_repair
                    from verify_card_array_locals_recovery import has_source_repair as card_repair
                    from verify_token_aggregate_recovery import has_source_repair as aggregate_repair
                    if aggregate_repair(proof,root,source_owner['source'],source_owner['name'],source_owner['definition_sha256']):
                        from verify_token_aggregate_recovery import verify_superseded_definition
                    elif card_repair(proof,root,source_owner['source'],source_owner['name'],source_owner['definition_sha256']):
                        from verify_card_array_locals_recovery import verify_superseded_definition
                    elif has_source_repair(proof,root,source_owner['source'],source_owner['name'],source_owner['definition_sha256']):
                        from verify_global_index_recovery import verify_superseded_definition
                    elif source_owner['name']=='adventure':
                        from verify_selection_storage_recovery import verify_superseded_definition
                    else:
                        from verify_target_role_recovery import verify_superseded_definition
                    verify_superseded_definition(proof,image,root,source_owner['source'],source_owner['name'],source_owner['definition_sha256'])
                parts = ranges(dw, inline)
                assert parts == [(int(p['address'], 16), int(p['end'], 16)) for p in witness['ranges']]
                for p, (lo, hi) in zip(witness['ranges'], parts):
                    assert any(x <= lo < hi <= y for x, y in owner_ranges)
                    raw = image.pe.get_data(lo - 0x2000000, hi - lo)
                    assert raw.hex() == p['bytes'] and sha(raw) == p['sha256']
                    ins = list(md.disasm(raw, lo))
                    assert sum(i.size for i in ins) == hi - lo
                    assert [[hex(i.address), i.mnemonic, i.op_str] for i in ins] == p['instructions']
                    fragments += 1
                instances += 1
                owners.add((origin['source'], parent.offset))
    edits = [e for e in proof['source_edits'] if e.get(edit_key) == path]
    assert len(edits) == len(record['source_edits']) == record['source_units']
    for r in record['source_edits']:
        assert any(all(e[k] == r[k] for k in ['source', 'before', 'after']) for e in edits)
    builds = []
    for key in ['runtime_before', 'runtime_after']:
        artifact = record[key]
        im = Image((root / artifact['path']).read_bytes())
        assert sha(im.data) == artifact['dll_sha256']
        builds.append(im)
    if runtime_check is None:
        assert stable_sections(runtime_sections(builds[0])) == stable_sections(runtime_sections(builds[1]))
    else:
        runtime_check(record, builds[0], builds[1], root)
    assert len(record['helpers']) == expected_count
    return dict(restored_inline_helper_definitions=expected_count, native_inline_instances=instances,
                native_inline_fragments=fragments, native_callers=len(owners),
                source_units=record['source_units'], scope=record['scope'])


if __name__ == '__main__':
    from canonicalize_helpers import load
    print(json.dumps(verify(load(), Image((ROOT / '.recovery/2022-ManalinkEh.dll').read_bytes())), indent=2))
