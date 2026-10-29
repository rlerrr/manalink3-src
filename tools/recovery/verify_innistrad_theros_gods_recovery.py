"""Audit Innistrad/Theros/Gods body receipts, optimized Tribute ABI and native arrays."""
import csv,json
from elftools.dwarf.locationlists import LocationParser
from canonicalize_helpers import ROOT,read,sha,span
from inspect_release import Image,inherited_value
from export_abi import type_of


def verify(proof,image,root=ROOT):
    path=proof.get('native_innistrad_theros_gods_recovery')
    if not path:return {}
    record=json.loads((root/path).read_text());assert sha(image.data)==record['dll_sha256']
    receipts={(e['source'],e.get('native_source_reconstruction',{}).get('function'),e['after_sha256']) for e in proof['source_edits']}
    for row in record['helpers']+record['card_callers']:
        assert (row['source'],row['name'],row['definition_sha256']) in receipts,row['name']
        text=read(root/row['source']);a,b=span(text,row['name'])
        assert sha(text[a:b].encode())==row['definition_sha256'],row['name']
        for part in row['native_ranges']:
            lo=int(part['address'],16);hi=int(part['end'],16)
            assert sha(image.pe.get_data(lo-0x2000000,hi-lo))==part['bytes_sha256']
            assert sha((root/part['raw_export']).read_bytes())==part['raw_export_sha256']
    original=Image((root/'G:/Manalink/ManalinkEh_10_22_Hotfix.dbg').read_bytes())
    assert sha(original.data)=='7cdd6bfea74652b900250faae275c754ef1d32966b9f314a729e46b980337b85'
    dw=original.dwarf();parser=LocationParser(dw.location_lists())
    inventory={r['low_pc']:r for r in csv.DictReader((root/'.recovery/2022-functions.csv').open()) if r['low_pc']}
    def walk(d):
        yield d
        for c in d.iter_children():yield from walk(c)
    def fn(at):return dw.get_DIE_from_refaddr(int(inventory[at]['die_offset'],16))
    def locs(d):return [e.loc_expr for e in parser.parse_from_attribute(d.attributes['DW_AT_location'],d.cu['version'],die=d) if hasattr(e,'loc_expr')]
    tribute=fn('0x20354d0');params={inherited_value(d,'DW_AT_name'):d for d in tribute.iter_children() if d.tag=='DW_TAG_formal_parameter'}
    # The optimized private worker uses eax, edx, ecx for player/card/count.
    entry={n:locs(params[n])[0] for n in ['player','card','num_counters','event']}
    assert entry==record['tribute_entry_locations']==dict(player=[80],card=[82],num_counters=[81],event=[250,114,73,12,0,159])
    fixed={};bounds={}
    for d in walk(tribute):
        if d.tag=='DW_TAG_variable' and inherited_value(d,'DW_AT_name')=='buf':
            typ=type_of(d);sub=list(typ.iter_children());assert len(sub)==1
            upper=sub[0].attributes['DW_AT_upper_bound'];assert upper.form=='DW_FORM_data1'
            fixed['tribute.buf']=upper.value+1
    for d in walk(fn('0x2073d00')):
        name=inherited_value(d,'DW_AT_name')
        if d.tag!='DW_TAG_variable' or name not in ['backup','backup_special']:continue
        typ=type_of(d);assert typ.tag=='DW_TAG_array_type'
        element=typ.get_DIE_from_attribute('DW_AT_type');assert element.attributes['DW_AT_byte_size'].value==4
        sub=list(typ.iter_children());assert sub[0].attributes['DW_AT_upper_bound'].form=='DW_FORM_data1'
        fixed['gather_body_parts.'+name+'.rows']=sub[0].attributes['DW_AT_upper_bound'].value+1
        if name=='backup':
            assert len(sub)==2 and sub[1].attributes['DW_AT_upper_bound'].form=='DW_FORM_ref4'
            bound=sub[1].get_DIE_from_attribute('DW_AT_upper_bound')
            bounds['backup.columns.upper_bound_locations']=locs(bound)
    assert fixed==record['native_fixed_array_extents']=={'tribute.buf':100,'gather_body_parts.backup.rows':3,'gather_body_parts.backup_special.rows':3}
    assert bounds==record['native_dynamic_array_bounds']=={'backup.columns.upper_bound_locations':[[112,127,159],[145,8,6,49,28,159],[145,8,6,49,28,159]]}
    assert len(record['helpers'])==record['native_helper_ranges']==37
    assert sum(r['source_changed'] for r in record['helpers'])==record['changed_helper_definitions']==29
    assert len(record['card_callers'])==16 and sum(r['source_changed'] for r in record['card_callers'])==record['changed_card_callers']==4
    def body(path,name):
        t=read(root/path);a,b=span(t,name);return t[a:b]
    assert 'aff->ext->sp_keywords' in body('cards/born_of_the_gods.c','archetype')
    assert 'aff->targets[16]' not in body('cards/born_of_the_gods.c','archetype')
    # Distinct detach bytes are intentional: Reaper retains byte1, most others use byte2.
    assert 'SET_BYTE1(instance->targets[11].player)' in body('cards/theros.c','reaper_of_the_wilds_effect')
    for name in ['gutter_grime_effect','murder_of_crows_legacy','selhoff_occultist_legacy','falkenrath_noble_effect','rage_thrower_legacy']:
        text=body('cards/innistrad.c',name);assert 'SET_BYTE2(instance->targets[11].player)' in text
        assert text.index('resolve_gfp_ability(')<text.index('if (result && !check_status(')
    # Preserve the already established source name for this legacy; no duplicate added.
    assert any(r['name']=='innistrad_exile_at_end_of_combat_legacy' and r['native_ranges'][0]['address']=='0x2073940' for r in record['helpers'])
    return dict(helper_definitions=37,changed_helpers=29,complete_card_callers=16,changed_card_callers=4,
                native_fixed_array_extents=fixed,native_dynamic_array_bounds=bounds,tribute_entry_locations=entry,
                scope='Complete native-range/source receipts, optimized Tribute parameter mapping, and fixed/dynamic native frame arrays. Built-in chooser backends and downstream gameplay tests remain separate.')


if __name__=='__main__':
    from canonicalize_helpers import load
    print(json.dumps(verify(load(),Image((ROOT/'.recovery/2022-ManalinkEh.dll').read_bytes())),indent=2))
