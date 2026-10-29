"""Verify land/hybrid/core receipts and native payment/audio storage."""
import json
from canonicalize_helpers import ROOT,read,sha,span
from inspect_release import Image,value,inherited_value
from elftools.dwarf.dwarf_expr import DWARFExprParser


def unqualified(die):
    while die.tag in {'DW_TAG_typedef','DW_TAG_const_type','DW_TAG_volatile_type','DW_TAG_restrict_type'}:
        die=die.get_DIE_from_attribute('DW_AT_type')
    return die


def const_expression(expr,dwarf):
    ops=DWARFExprParser(dwarf.structs).parse_expr(expr)
    if ops and ops[-1].op_name=='DW_OP_stack_value':ops=ops[:-1]
    assert len(ops)==1,ops
    op=ops[0]
    if op.op_name.startswith('DW_OP_const'):return op.args[0]
    if op.op_name.startswith('DW_OP_lit'):return int(op.op_name[9:])
    raise AssertionError(('nonconstant bound',ops))


def bound(die,field,dwarf):
    attr=die.attributes[field]
    if attr.form.startswith('DW_FORM_ref'):
        target=die.get_DIE_from_attribute(field)
        const=value(target,'DW_AT_const_value')
        if const is not None:return const
        location=target.attributes['DW_AT_location']
        if location.form=='DW_FORM_exprloc':return const_expression(location.value,dwarf)
        entries=dwarf.location_lists().get_location_list_at_offset(location.value,die=target)
        values={const_expression(entry.loc_expr,dwarf) for entry in entries if hasattr(entry,'loc_expr')}
        assert len(values)==1,values
        return values.pop()
    if attr.form=='DW_FORM_exprloc':return const_expression(attr.value,dwarf)
    return attr.value


def array_shape(die,dwarf):
    die=unqualified(die);assert die.tag=='DW_TAG_array_type'
    return [bound(c,'DW_AT_count',dwarf) if 'DW_AT_count' in c.attributes else bound(c,'DW_AT_upper_bound',dwarf)+1
            for c in die.iter_children() if c.tag=='DW_TAG_subrange_type']


def record_layout(die):
    die=unqualified(die)
    return dict(byte_size=value(die,'DW_AT_byte_size'),fields={value(c,'DW_AT_name'):value(c,'DW_AT_data_member_location') for c in die.iter_children() if c.tag=='DW_TAG_member'})


def variables(die):
    for child in die.iter_children():
        if child.tag=='DW_TAG_variable':yield child
        if child.has_children:yield from variables(child)


def verify(proof,image,root=ROOT):
    path=proof.get('native_land_hybrid_core_recovery')
    if not path:return {}
    record=json.loads((root/path).read_text());assert sha(image.data)==record['dll_sha256']
    receipts={(e['source'],e.get('native_source_reconstruction',{}).get('function'),e['after_sha256']) for e in proof['source_edits']}
    for row in record['helpers']:
        assert (row['source'],row['name'],row['definition_sha256']) in receipts,row['name']
        text=read(root/row['source']);a,b=span(text,row['name']);assert sha(text[a:b].encode())==row['definition_sha256'],row['name']
    for row in record['inline_support_helpers']:
        text=read(root/row['source']);a,b=span(text,row['name']);assert sha(text[a:b].encode())==row['definition_sha256']
        assert any(e['source']==row['source'] and e['after_sha256']==row['definition_sha256'] and
                   e.get('native_inline_source_reconstruction',{}).get('owner')==row['native_owner'] for e in proof['source_edits'])
    original=Image((root/'G:/Manalink/ManalinkEh_10_22_Hotfix.dbg').read_bytes())
    assert sha(original.data)=='7cdd6bfea74652b900250faae275c754ef1d32966b9f314a729e46b980337b85'
    dwarf=original.dwarf();sound=record['native_sound_record'];layout={k:sound[k] for k in ['byte_size','fields']}
    assert record_layout(dwarf.get_DIE_from_refaddr(int(sound['die_offset'],16)))==layout
    expected={n:r['shape'] for n,r in record['native_reaper_arrays'].items()}
    for name,row in record['native_reaper_arrays'].items():
        assert array_shape(dwarf.get_DIE_from_refaddr(int(row['die_offset'],16)),dwarf)==row['shape'],name
    debug=Image((root/'ManalinkEh.dbg').read_bytes());built_dwarf=debug.dwarf();rebuilt={};sounds=[]
    for cu in built_dwarf.iter_CUs():
        source=value(cu.get_top_DIE(),'DW_AT_name','').replace('\\','/').removeprefix('./')
        if source not in ['cards/shadowmoor.c','functions/windows.c']:continue
        for fn in cu.iter_DIEs():
            if fn.tag!='DW_TAG_subprogram' or not value(fn,'DW_AT_low_pc'):continue
            name=inherited_value(fn,'DW_AT_name','')
            if name not in ['casting_reaper_king','play_sound_effect']:continue
            for var in variables(fn):
                varname=inherited_value(var,'DW_AT_name','')
                if name=='casting_reaper_king' and varname in expected:
                    rebuilt[varname]=array_shape(var.get_DIE_from_attribute('DW_AT_type'),built_dwarf)
                if name=='play_sound_effect' and varname=='snd':sounds.append(record_layout(var.get_DIE_from_attribute('DW_AT_type')))
    assert rebuilt==expected,(rebuilt,expected)
    assert sounds and all(s==layout for s in sounds),sounds
    assert len(record['helpers'])==record['helper_definitions']==record['new_native_helper_ranges']==29
    assert sum(r['source_changed'] for r in record['helpers'])==record['changed_helper_definitions']==11
    assert len(record['inline_support_helpers'])==2
    return dict(helper_definitions=29,new_manual_ranges=29,changed_helpers=11,inline_support_definitions=2,
                native_and_rebuilt_payment_arrays=expected,native_and_rebuilt_sound_descriptor_bytes=32,
                scope='Complete source/native-range receipts, native/rebuilt payment-array extents including optimized VLA bounds, and complete sound descriptor offsets. Built-in UI/audio backends and gameplay tests remain downstream.')


if __name__=='__main__':
    from canonicalize_helpers import load
    print(json.dumps(verify(load(),Image((ROOT/'.recovery/2022-ManalinkEh.dll').read_bytes())),indent=2))
