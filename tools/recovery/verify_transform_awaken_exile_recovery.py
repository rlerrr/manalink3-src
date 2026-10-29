"""Audit transformation/awaken/exile bodies, optimized contracts and target-init order."""
import csv,json,re
from elftools.dwarf.locationlists import LocationParser
from canonicalize_helpers import ROOT,read,sha,span
from inspect_release import Image,inherited_value
from export_abi import type_of


BAD_SONG_ORDER='  td.preferred_controller = player;\n  default_target_definition(player, card, &td, TYPE_CREATURE);'
GOOD_SONG_ORDER='  default_target_definition(player, card, &td, TYPE_CREATURE);\n  td.preferred_controller = player;'


def verify(proof,image,root=ROOT):
    path=proof.get('native_transform_awaken_exile_recovery')
    if not path:return {}
    record=json.loads((root/path).read_text());assert sha(image.data)==record['dll_sha256']
    edits=proof['source_edits'];receipts={(e['source'],e.get('native_source_reconstruction',{}).get('function'),e['after_sha256']) for e in edits}
    for row in record['helpers']+record['support_helpers']+record['card_callers']:
        assert (row['source'],row['name'],row['definition_sha256']) in receipts,row['name']
        text=read(root/row['source']);a,b=span(text,row['name']);assert sha(text[a:b].encode())==row['definition_sha256'],row['name']
        for part in row['native_ranges']:
            lo=int(part['address'],16);hi=int(part['end'],16)
            assert sha(image.pe.get_data(lo-0x2000000,hi-lo))==part['bytes_sha256']
            assert sha((root/part['raw_export']).read_bytes())==part['raw_export_sha256']
    original=Image((root/'G:/Manalink/ManalinkEh_10_22_Hotfix.dbg').read_bytes())
    assert sha(original.data)=='7cdd6bfea74652b900250faae275c754ef1d32966b9f314a729e46b980337b85'
    dw=original.dwarf();parser=LocationParser(dw.location_lists())
    inventory={r['low_pc']:r for r in csv.DictReader((root/'.recovery/2022-functions.csv').open()) if r['low_pc']}
    def fn(at):return dw.get_DIE_from_refaddr(int(inventory[at]['die_offset'],16))
    def locs(d):
        a=d.attributes.get('DW_AT_location')
        if not a:return []
        parsed=parser.parse_from_attribute(a,d.cu['version'],die=d)
        return [parsed.loc_expr] if hasattr(parsed,'loc_expr') else [e.loc_expr for e in parsed if hasattr(e,'loc_expr')]
    def walk(d):
        yield d
        for c in d.iter_children():yield from walk(c)
    abi={}
    for at in record['optimized_parameter_evidence']:
        abi[at]={inherited_value(d,'DW_AT_name'):dict(first_location=next(iter(locs(d)),None),constant=inherited_value(d,'DW_AT_const_value')) for d in fn(at).iter_children() if d.tag=='DW_TAG_formal_parameter'}
    assert abi==record['optimized_parameter_evidence']
    assert abi['0x229f1e0']['sleight']['constant']==1
    assert {n:abi['0x229f240'][n]['constant'] for n in ['qty','sleight']}==dict(qty=1,sleight=1)
    assert abi['0x244d4c0']['num_cards']['constant']==1
    for at in ['0x24723f0','0x2472540']:
        assert abi[at]['red']['constant']==abi[at]['white']['constant']==0
    assert abi['0x244d140']['td']['first_location']==[145,28]
    assert abi['0x244d140']['prompt']['first_location']==[145,32]
    assert abi['0x244d140']['impl']['first_location']==[145,36]
    extents={}
    for at,names in [('0x2471a10',{'marked'}),('0x26d6260',{'iids','locs','buf'}),('0x26d6ac0',{'csvids'})]:
        for d in walk(fn(at)):
            name=inherited_value(d,'DW_AT_name')
            if d.tag!='DW_TAG_variable' or name not in names:continue
            typ=type_of(d);assert typ.tag=='DW_TAG_array_type';dims=[]
            for sub in typ.iter_children():
                upper=sub.attributes['DW_AT_upper_bound'];assert upper.form in {'DW_FORM_data1','DW_FORM_data2','DW_FORM_data4'};dims.append(upper.value+1)
            extents[at+'::'+name]=dims
    assert extents==record['native_array_extents']
    absolute={name:val for name,val,section,kind,storage in original.symbols if section==-1}
    for name,at in record['native_absolute_globals'].items():assert absolute[name]==int(at,16)
    lds=read(root/'manalink.lds')
    for name in ['_cancel','_spell_fizzled','_tapped_for_mana_color']:
        assert re.search(r'(?m)^'+re.escape(name)+r'\s*=\s*'+re.escape(record['native_absolute_globals'][name])+r';',lds,re.I)
    assert '#define ENGINE_DONE_LABEL 0x7375b0' in read(root/'manalink.h')
    # Ghidra hoisted these assignments in its C output; actual stores follow calls.
    from capstone import Cs,CS_ARCH_X86,CS_MODE_32
    md=Cs(CS_ARCH_X86,CS_MODE_32);counts={}
    for site in record['target_initialization_sites']:
        lo=int(site['call_address'],16);data=bytes.fromhex(site['bytes_hex'])
        assert image.pe.get_data(lo-0x2000000,len(data))==data
        ins=list(md.disasm(data,lo));assert ins[0].mnemonic=='call' and ins[0].op_str=='0x2660240'
        assert ins[-1].address==int(site['store_address'],16) and ins[-1].mnemonic=='mov'
        assert ins[-1].op_str.startswith('dword ptr [ebp - 0x58],')
        counts[site['function']]=counts.get(site['function'],0)+1
    assert counts=={'awaken_tgt':3,'song':4}
    refinements=[e for e in edits if e.get('native_target_initialization_refinement')]
    assert len(refinements)==1
    song=refinements[0];assert song['source']=='cards/legends.c'
    assert song['before'].count(BAD_SONG_ORDER)==1
    assert song['after']==song['before'].replace(BAD_SONG_ORDER,GOOD_SONG_ORDER,1)
    assert song['native_target_initialization_refinement']['supersedes']=='docs/recovery-evidence/classic-lands-slivers-recovery.json'
    assert len(record['helpers'])==record['helper_definitions']==35
    assert sum(len(r['native_ranges']) for r in record['helpers'])==record['native_helper_ranges']==36
    assert sum(r['source_changed'] for r in record['helpers'])==record['changed_helper_definitions']==23
    assert len(record['support_helpers'])==2 and all(r['source_changed'] for r in record['support_helpers'])
    assert len(record['card_callers'])==15 and sum(r['source_changed'] for r in record['card_callers'])==record['changed_card_callers']==4
    return dict(helper_definitions=35,native_helper_ranges=36,changed_helpers=23,complete_support_helpers=2,
                complete_card_callers=15,changed_card_callers=4,native_array_extents=extents,target_initialization_sites=counts,
                scope='Complete body/native-range receipts, optimized parameter mappings, fixed native arrays, named native globals and assembly-proven target initialization order. Built-in dialog backends and gameplay tests remain downstream.')


if __name__=='__main__':
    from canonicalize_helpers import load
    print(json.dumps(verify(load(),Image((ROOT/'.recovery/2022-ManalinkEh.dll').read_bytes())),indent=2))
