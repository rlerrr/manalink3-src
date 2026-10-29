"""Native receipts for Kamigawa, Sliver, delve and related card recovery."""
import re
from canonicalize_helpers import span,mask


def source_span(text,name):
    bounds=span(text,name)
    if bounds:return bounds
    # ARCANE generates a public wrapper and a private name_impl worker.
    public=name.removesuffix('_impl')
    code=mask(text)
    m=re.search(r'(?m)^[ \t]*ARCANE\(\s*'+re.escape(public)+r'\s*\)\s*\{',code)
    if m is None:return None
    depth=1
    for end in range(m.end(),len(code)):
        depth+=(code[end]=='{')-(code[end]=='}')
        if depth==0:return m.start(),end+1
    raise ValueError('Unclosed ARCANE body '+name)

import csv,json
from elftools.dwarf.locationlists import LocationParser
from canonicalize_helpers import ROOT,read,sha
from inspect_release import Image,value,inherited_value
from export_abi import type_of


def verify(proof,image,root=ROOT):
    path=proof.get('native_kamigawa_sliver_delve_recovery')
    if not path:return {}
    record=json.loads((root/path).read_text());assert sha(image.data)==record['dll_sha256']
    receipts={(e['source'],e.get('native_source_reconstruction',{}).get('function'),e['after_sha256']) for e in proof['source_edits']}
    inventory={r['low_pc']:r for r in csv.DictReader((root/'.recovery/2022-functions.csv').open()) if r['low_pc']}
    from capstone import Cs,CS_ARCH_X86,CS_MODE_32
    md=Cs(CS_ARCH_X86,CS_MODE_32)
    arcane=[r for r in inventory.values() if r['name']=='arcane'];assert len(arcane)==1
    for row in record['helpers']+record['support_helpers']+record['card_callers']:
        assert (row['source'],row['name'],row['definition_sha256']) in receipts,row['name']
        text=read(root/row['source']);a,b=source_span(text,row['name']);assert sha(text[a:b].encode())==row['definition_sha256'],row['name']
        for part in row['native_ranges']:
            lo=int(part['address'],16);hi=int(part['end'],16)
            assert sha(image.pe.get_data(lo-0x2000000,hi-lo))==part['bytes_sha256']
            assert sha((root/part['raw_export']).read_bytes())==part['raw_export_sha256']
        if row.get('source_macro'):
            assert row['source_macro']=='ARCANE' and text[a:b].lstrip().startswith('ARCANE('+row['name']+')')
            parts={inventory[p['address']]['name']:p for p in row['native_ranges']}
            assert set(parts)=={row['name'],row['name']+'_impl'}
            public=parts[row['name']];worker=parts[row['name']+'_impl'];lo=int(public['address'],16);hi=int(public['end'],16)
            instructions=list(md.disasm(image.pe.get_data(lo-0x2000000,hi-lo),lo))
            assert any(i.mnemonic=='mov' and i.op_str=='dword ptr [esp + 0xc], '+worker['address'] for i in instructions)
            assert any(i.mnemonic=='call' and i.op_str==arcane[0]['low_pc'] for i in instructions)
    original=Image((root/'G:/Manalink/ManalinkEh_10_22_Hotfix.dbg').read_bytes())
    assert sha(original.data)=='7cdd6bfea74652b900250faae275c754ef1d32966b9f314a729e46b980337b85'
    dw=original.dwarf();parser=LocationParser(dw.location_lists())
    def fn(at):return dw.get_DIE_from_refaddr(int(inventory[at]['die_offset'],16))
    def locs(d):
        a=d.attributes.get('DW_AT_location')
        if not a:return []
        parsed=parser.parse_from_attribute(a,d.cu['version'],die=d)
        return [parsed.loc_expr] if hasattr(parsed,'loc_expr') else [e.loc_expr for e in parsed if hasattr(e,'loc_expr')]
    def walk(d):
        yield d
        for c in d.iter_children():yield from walk(c)
    abi={at:{inherited_value(d,'DW_AT_name'):dict(first_location=next(iter(locs(d)),None),constant=inherited_value(d,'DW_AT_const_value')) for d in fn(at).iter_children() if d.tag=='DW_TAG_formal_parameter'} for at in record['optimized_parameter_evidence']}
    assert abi==record['optimized_parameter_evidence']
    assert abi['0x25692e0']['s_key']==dict(first_location=None,constant=0)
    assert abi['0x25692e0']['clr']['first_location']==[145,16]
    assert abi['0x2566700']['td']['first_location']==[145,4] and abi['0x2566700']['td2']['first_location']==[145,12]
    assert abi['0x25718e0']['black']['first_location']==[81]
    arrays={};dynamic={};wanted={}
    for key in record['native_arrays']:
        at,name=key.split('::');wanted.setdefault(at,set()).add(name)
    for at,names in wanted.items():
        for d in walk(fn(at)):
            name=inherited_value(d,'DW_AT_name')
            if d.tag!='DW_TAG_variable' or name not in names:continue
            typ=type_of(d)
            if typ.tag!='DW_TAG_array_type':continue
            key=at+'::'+name;dims=[]
            for sub in typ.iter_children():
                upper=sub.attributes['DW_AT_upper_bound']
                if upper.form in {'DW_FORM_ref4','DW_FORM_exprloc'}:
                    dims.append('dynamic');dynamic[key]=dict(form=upper.form,expressions=[list(upper.value)] if upper.form=='DW_FORM_exprloc' else locs(sub.get_DIE_from_attribute('DW_AT_upper_bound')))
                else:
                    assert upper.form in {'DW_FORM_data1','DW_FORM_data2','DW_FORM_data4'};dims.append(upper.value+1)
            row=dict(dimensions=dims,element_bytes=type_of(typ).attributes['DW_AT_byte_size'].value)
            if key in arrays:assert arrays[key]==row
            arrays[key]=row
    assert arrays==record['native_arrays'] and len(arrays)==12
    assert dynamic==record['native_dynamic_array_bounds']=={'0x23bd870::g_copy':dict(form='DW_FORM_exprloc',expressions=[[112,127]])}
    assert arrays['0x23bd870::g_copy']==dict(dimensions=['dynamic'],element_bytes=4)
    assert arrays['0x25718e0::total']==dict(dimensions=[6],element_bytes=4)
    for row in record['native_card_ids']:
        die=dw.get_DIE_from_refaddr(int(row['die_offset'],16));assert die.tag=='DW_TAG_enumerator'
        assert value(die,'DW_AT_name')==row['name'] and value(die,'DW_AT_const_value')==row['value']
        assert re.search(r'\b'+row['name']+r'\s*=\s*'+str(row['value'])+r'\b',read(root/'card_id.h'))
    assert record['native_card_ids']==[dict(name='CARD_ID_MURKTIDE_REGENT',value=23095,die_offset='0x1f493b9')]
    blocks={}
    for block in record['native_instruction_blocks']:
        at=int(block['address'],16);data=bytes.fromhex(block['bytes_hex']);assert image.pe.get_data(at-0x2000000,len(data))==data
        instructions=[dict(mnemonic=i.mnemonic,op_str=i.op_str) for i in md.disasm(data,at)]
        assert instructions==block['instructions'];blocks[block['name']]=instructions
    assert len(blocks)==10
    legacy=blocks['nezumi_legacy_decreasing_count'];card=blocks['nezumi_card_decreasing_count']
    assert dict(mnemonic='mov',op_str='dword ptr [esp + 4], edi') in legacy and dict(mnemonic='dec',op_str='edi') in legacy
    assert dict(mnemonic='mov',op_str='dword ptr [esp + 4], edx') in card and dict(mnemonic='dec',op_str='edx') in card
    assert all(dict(mnemonic='call',op_str='0x268f1b0') in block for block in [legacy,card])
    assert inventory['0x268f1b0']['name']=='lose_life'
    allocation=blocks['shrouded_lore_deck_sized_allocation']
    assert dict(mnemonic='call',op_str='0x26e5d70') in allocation and inventory['0x26e5d70']['name']=='count_deck'
    assert allocation[-3:]==[dict(mnemonic='shl',op_str='eax, 2'),dict(mnemonic='call',op_str='0x26fc1d0'),dict(mnemonic='sub',op_str='esp, eax')]
    for name in ['bridge_death_reset','bridge_resolution_reset']:
        assert blocks[name]==[dict(mnemonic='mov',op_str='dword ptr [ebx + 0x4c], 0')]
    snapshot=blocks['bridge_snapshot_before_resolver']
    assert snapshot[0]==dict(mnemonic='mov',op_str='eax, dword ptr [ebx + 0x4c]')
    assert dict(mnemonic='mov',op_str='dword ptr [ebp - 0x10], eax') in snapshot and snapshot[-1]==dict(mnemonic='call',op_str='0x2689440')
    assert inventory['0x2689440']['name']=='resolve_gfp_ability'
    assert blocks['volute_player_register']==[dict(mnemonic='mov',op_str='ebx, dword ptr [ebp + 8]')]
    own=dict(mnemonic='mov',op_str='ecx, dword ptr [ebx*4 + 0x4d7018]')
    assert own in blocks['volute_resolve_own_grave_cache'] and own in blocks['volute_static_own_grave_cache']
    assert dict(mnemonic='mov',op_str='eax, dword ptr [edx + 0x74]') in blocks['volute_retarget_target_grave_cache']
    dependency=record['inlined_callback_dependency'];text=read(root/dependency['source']);a,b=source_span(text,dependency['function']);assert sha(text[a:b].encode())==dependency['definition_sha256']
    assert dependency['owner_range']=='0x23b4470' and dependency['standalone_native_range'] is None
    assert '#define ARCANE(spell_fn)' in read(root/'manalink.h')
    macro=next(line for line in read(root/'manalink.h').splitlines() if line.startswith('#define ARCANE(spell_fn)'))
    assert sha(macro.encode())==record['arcane_macro_definition_sha256']
    assert len(record['helpers'])==record['helper_definitions']==record['native_helper_ranges']==32
    assert len(record['support_helpers'])==4 and not any(r['source_changed'] for r in record['support_helpers'])
    assert sum(r['source_changed'] for r in record['helpers'])==record['changed_helper_definitions']==18
    assert len(record['card_callers'])==34 and sum(r['source_changed'] for r in record['card_callers'])==record['changed_card_callers']==6
    assert sum(bool(r.get('source_macro')) for r in record['card_callers'])==5
    assert sum(len(r['native_ranges']) for r in record['card_callers'])==39
    return dict(helper_definitions=32,native_helper_ranges=32,changed_helpers=18,supporting_helpers=4,complete_card_definitions=34,native_card_ranges=39,changed_card_definitions=6,
                native_frame_arrays=12,native_instruction_blocks=10,native_card_ids=1,
                scope='Complete direct C/ARCANE body and native-range receipts. Genju specialization, fixed/dynamic frame arrays, native loop/reset/cache instructions, canonical callback owner mapping and five Arcane wrapper/worker pairs checked. Independent callees remain separately tracked; built-in dialog backends and gameplay tests remain downstream.')


if __name__=='__main__':
    from canonicalize_helpers import load
    print(json.dumps(verify(load(),Image((ROOT/'.recovery/2022-ManalinkEh.dll').read_bytes())),indent=2))
