"""Audit guild/Spirit/tribal recovery receipts and optimized parameter identities."""
import csv,json
from canonicalize_helpers import ROOT,read,sha,span
from inspect_release import Image,value,inherited_value
from elftools.dwarf.dwarf_expr import DWARFExprParser


def verify(proof,image,root=ROOT):
    path=proof.get('native_guild_spirit_tribal_recovery')
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
    dw=original.dwarf();parser=DWARFExprParser(dw.structs)
    inventory={r['low_pc']:r for r in csv.DictReader((root/'.recovery/2022-functions.csv').open()) if r['low_pc']}
    def die(at):return dw.get_DIE_from_refaddr(int(inventory[at]['die_offset'],16))
    def locations(fn):
        result={}
        for p in fn.iter_children():
            if p.tag!='DW_TAG_formal_parameter':continue
            a=p.attributes.get('DW_AT_location')
            if not a:continue
            name=inherited_value(p,'DW_AT_name')
            if a.form=='DW_FORM_sec_offset':
                result[name]=[[(op.op_name,op.args) for op in parser.parse_expr(x.loc_expr)]
                    for x in dw.location_lists().get_location_list_at_offset(a.value,p) if hasattr(x,'loc_expr')]
        return result
    soul=locations(die('0x260b780'))
    assert list(soul)==['player','cmc','num','event','card']
    for name,reg in [('player',0),('cmc',2),('num',1)]:
        assert soul[name][0]==[('DW_OP_reg'+str(reg),[])],(name,soul[name])
    for name in ['event','card']:
        assert all([op[0] for op in expr]==['DW_OP_GNU_parameter_ref','DW_OP_stack_value'] for expr in soul[name])
    vivid=locations(die('0x258e050'))
    assert all(expr==[('DW_OP_fbreg',[0])] for expr in vivid['test_color'])
    assert all([op[0] for op in expr]==['DW_OP_GNU_parameter_ref','DW_OP_stack_value'] for expr in vivid['color'])
    native_void={'battalion':'0x2516b20','arcane_spirit_spell_trigger':'0x260c340'}
    for name,at in native_void.items():
        assert inherited_value(die(at),'DW_AT_type') is None,name
    rebuilt=Image((root/'ManalinkEh.dbg').read_bytes());void_entries={}
    for cu in rebuilt.dwarf().iter_CUs():
        source=value(cu.get_top_DIE(),'DW_AT_name','').replace('\\','/').removeprefix('./')
        if source not in {'cards/gatecrash.c','cards/champions_of_kamigawa.c'}:continue
        for fn in cu.iter_DIEs():
            name=inherited_value(fn,'DW_AT_name') if fn.tag=='DW_TAG_subprogram' else None
            if name in native_void and value(fn,'DW_AT_low_pc'):
                assert inherited_value(fn,'DW_AT_type') is None,name
                void_entries[name]=hex(value(fn,'DW_AT_low_pc'))
    assert set(void_entries)==set(native_void)
    assert len(record['helpers'])==record['helper_definitions']==38
    assert sum(len(r['native_ranges']) for r in record['helpers'])==record['new_native_helper_ranges']==39
    assert sum(r['source_changed'] for r in record['helpers'])==record['changed_helper_definitions']==21
    assert len(record['card_callers'])==38
    assert sum(r['source_changed'] for r in record['card_callers'])==record['changed_card_callers']==38
    return dict(helper_definitions=38,native_helper_ranges=39,changed_helpers=21,
      complete_card_callers=38,changed_card_callers=38,native_and_rebuilt_void_dispatch=True,
      soulshift_private_registers=dict(player='eax',cmc='edx',num='ecx'),vivid_stack_parameter='test_color',
      scope='Complete source/native-range receipts, native and rebuilt void dispatch, and DWARF optimized parameter mapping. Built-in dialogs and gameplay tests remain downstream.')


if __name__=='__main__':
    from canonicalize_helpers import load
    print(json.dumps(verify(load(),Image((ROOT/'.recovery/2022-ManalinkEh.dll').read_bytes())),indent=2))
