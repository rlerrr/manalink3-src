"""Verify the specialized activation component's native storage and call evidence."""
import hashlib
import struct

from activation_cost_bodies import GRAVE_STORAGE
from audit_source import function_body
from recover_activation_costs import ADDRESSES, CONTROL_SITES
from recover_literals import readonly_string


def audit_component(component, manifest, sources, machine_bytes, image):
    def require(condition, message):
        if not condition:
            raise ValueError(message)

    entries = {f['address']: f for f in manifest['accepted_functions']
               if f.get('activation_cost_evidence')}
    require(set(entries) == set(ADDRESSES) == set(component['functions']) and
            not any(f['static'] for f in entries.values()),
            'Specialized activation membership/linkage mismatch')
    for address, detail in component['functions'].items():
        require(entries[address]['activation_cost_evidence'] == detail,
                'Specialized activation function evidence mismatch')
    require([len(component['parameters'][a]) for a in ADDRESSES] == [16, 16, 15, 8],
            'Specialized activation physical parameter count mismatch')
    for address, name, bound in [('26a66a0', 'to_tap', 56), ('2652be0', 'tcigfu', 24)]:
        array = component['arrays'][address]
        require((array['name'], array['element_type'], array['element_bytes'], array['upper_bound']) ==
                (name, 'int', 4, [145, bound, 6, 49, 28]),
                'Specialized activation dynamic integer-array layout mismatch')
    fixed = lambda address: {(v['name'], v['type'], v['location'])
                             for v in component['locals'][address]
                             if isinstance(v['location'], str)}
    require({('buffer', 'char[100]', '(DW_OP_fbreg: -552)'),
             ('td2', 'target_definition_t', '(DW_OP_fbreg: -552)')} <= fixed('26a4c00'),
            'Grave activation overlapping text/target scratch mismatch')
    require({('marked_for_sacrifice', 'char[151]', '(DW_OP_fbreg: -603)'),
             ('buffer', 'char[100]', '(DW_OP_fbreg: -603)')} <= fixed('26a4540'),
            'Sacrifice activation overlapping scratch mismatch')
    require(component['storage_declaration'] == GRAVE_STORAGE and
            sources['functions/functions.c'].replace('\r\n', '\n').count(GRAVE_STORAGE) == 1,
            'Grave activation shared storage declaration mismatch')
    require(component['added_flag_members'] == dict(SARV_REPORT_COUNT=8),
            'Sacrifice report-count native flag mismatch')
    sites = component['control_sites']
    require([s['address'] for s in sites] == CONTROL_SITES,
            'Specialized activation control evidence membership mismatch')
    for site in sites + component['scalar_report']['sites']:
        data = bytes.fromhex(site['bytes'])
        require(machine_bytes(int(site['address'], 16), len(data)) == data,
                'Specialized activation native instruction mismatch')
    observed = {s['address']: s['bytes'] for s in sites}
    require(all(observed[a] == b for a, b in {
        '26a677d': 'c1e002', '26a680e': 'c744240809000000',
        '26a6836': '8b80c0000000', '26a69b1': '8a4736',
        '2652dd8': 'c1e002', '26530fc': '2de8030000',
        '2652d14': '89442408'}.items()),
        'Specialized activation VLA/slot/format semantics mismatch')
    report = component['scalar_report']
    require((report['bytes'], report['field_offset'], report['variable_type']) == (4, 56, 'int')
            and bool(report['sites']), 'Sacrifice activation scalar report mismatch')
    require(component['tap_selection'] == dict(target_index=9, target_card_offset=192,
            vla_element_bytes=4, cleanup_uses_number_of_targets=True),
            'Native tap activation selection/cleanup mismatch')
    require(component['grave_selection'] == dict(maximum_targets=19, retries=3,
            packed_player_shift=16, index_mask=65535, face_down_iid=904,
            allow_cancel_card_bias=-1000), 'Native grave selection encoding mismatch')
    for literal in component['native_texts']:
        require(readonly_string(image, int(literal['address'], 16)) == bytes.fromhex(literal['bytes']),
                'Specialized activation native prompt mismatch')
    color = component['source_color_helper']
    body = function_body(sources[color['source']], color['name']).replace('\r\n', '\n')
    require(hashlib.sha256(body.encode()).hexdigest() == color['body_sha256'] and
            color['preserves_native_uninitialized_format'] is True and
            'get_sleighted_color_text' in color['inline_names'],
            'Native inlined color helper/source template mismatch')
    table = machine_bytes(int(color['word_table_address'], 16), 64)
    words = [readonly_string(image, p).decode() for p in struct.unpack('<16I', table)]
    expected = [word for name in ['colorless', 'black', 'blue', 'green', 'red', 'white',
                                 'artifact', 'any'] for word in [name, name.upper()]]
    require(table.hex() == color['word_table_bytes'] and words == color['words'] == expected,
            'Native sleighted color word table mismatch')
