"""Verify concrete GCC inline instances when a private helper has no COFF entry.

Inline ranges belong to their callers. They never supply an invented callable
address or a whole-function comparison score for the original helper.
"""
from inspect_release import value, inherited_value
from unify_scalar_helpers import sha


def ranges(info, die):
    low = value(die, 'DW_AT_low_pc')
    if low is not None:
        high = die.attributes['DW_AT_high_pc']
        end = high.value if high.form == 'DW_FORM_addr' else low + high.value
        return [(low, end)] if end > low else []
    offset = value(die, 'DW_AT_ranges')
    if offset is None:
        return []
    base = value(die.cu.get_top_DIE(), 'DW_AT_low_pc', 0)
    result = []
    for entry in info.range_lists().get_range_list_at_offset(offset, cu=die.cu):
        if hasattr(entry, 'base_address'):
            base = entry.base_address
        else:
            low = entry.begin_offset + (0 if entry.is_absolute else base)
            end = entry.end_offset + (0 if entry.is_absolute else base)
            if end > low:
                result.append((low, end))
    return result


def verify(image, entries, code_image):
    if not entries:
        return {}
    wanted = {(e['source'], e['canonical_name']): e for e in entries}
    assert len(wanted) == len(entries)
    assert all(e['static'] for e in entries), 'Public helpers require physical entries'
    assert image.pe.OPTIONAL_HEADER.ImageBase == code_image.pe.OPTIONAL_HEADER.ImageBase
    assert image.pe.FILE_HEADER.TimeDateStamp == code_image.pe.FILE_HEADER.TimeDateStamp
    def code_sections(pe):
        return [(s.Name, s.VirtualAddress, s.Misc_VirtualSize) for s in pe.sections
                if s.Characteristics & 0x20000000]
    assert code_sections(image.pe) == code_sections(code_image.pe)
    origins = {key: set() for key in wanted}
    instances = {key: [] for key in wanted}
    info = image.dwarf()
    for cu in info.iter_CUs():
        source = value(cu.get_top_DIE(), 'DW_AT_name', '').replace('\\', '/').removeprefix('./')
        if source not in {key[0] for key in wanted}:
            continue
        for die in cu.iter_DIEs():
            key = (source, inherited_value(die, 'DW_AT_name'))
            if key not in wanted:
                continue
            if die.tag == 'DW_TAG_subprogram':
                assert value(die, 'DW_AT_inline') in {1, 3}, key
                assert not value(die, 'DW_AT_external', False), key
                assert not ranges(info, die), key
                origins[key].add(die.offset)
            elif die.tag == 'DW_TAG_inlined_subroutine':
                origin = die.get_DIE_from_attribute('DW_AT_abstract_origin')
                assert origin.tag == 'DW_TAG_subprogram' and origin.cu is cu
                assert value(origin, 'DW_AT_name') == key[1]
                parent = die.get_parent()
                while parent is not None and parent.tag != 'DW_TAG_subprogram':
                    parent = parent.get_parent()
                assert parent is not None, key
                owner_ranges = ranges(info, parent)
                spans = ranges(info, die)
                assert spans and owner_ranges, key
                checked = []
                for low, end in spans:
                    assert any(a <= low < end <= b for a, b in owner_ranges), key
                    rva = low - image.pe.OPTIONAL_HEADER.ImageBase
                    section = code_image.pe.get_section_by_rva(rva)
                    assert section is not None and section.Characteristics & 0x20000000
                    assert end - image.pe.OPTIONAL_HEADER.ImageBase <= section.VirtualAddress + section.Misc_VirtualSize
                    data = code_image.pe.get_data(rva, end - low)
                    assert len(data) == end - low
                    checked.append(dict(address=hex(low), bytes=end-low, sha256=sha(data)))
                instances[key].append(dict(die_offset=hex(die.offset), origin_offset=hex(origin.offset),
                    parent=inherited_value(parent, 'DW_AT_name'), ranges=checked))
    result = {}
    for key, entry in wanted.items():
        assert len(origins[key]) == 1 and instances[key], ('Missing concrete inline evidence', key)
        assert {int(row['origin_offset'], 16) for row in instances[key]} == origins[key]
        result[key[1]] = dict(source=key[0], recovery_name=entry['recovery_name'],
                             instances=instances[key])
    return result
