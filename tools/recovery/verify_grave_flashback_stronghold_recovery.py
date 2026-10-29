"""Audit graveyard/flashback helpers, optimized arguments and native source units."""
import csv,json
from canonicalize_helpers import ROOT,read,sha,span
from inspect_release import Image,value,inherited_value
from elftools.dwarf.dwarf_expr import DWARFExprParser


def verify(proof,image,root=ROOT):
    path=proof.get('native_grave_flashback_stronghold_recovery')
    if not path:return {}
    record=json.loads((root/path).read_text());assert sha(image.data)==record['dll_sha256']
    receipts={(e['source'],e.get('native_source_reconstruction',{}).get('function'),e['after_sha256']):e for e in proof['source_edits']}
    original=Image((root/'G:/Manalink/ManalinkEh_10_22_Hotfix.dbg').read_bytes())
    assert sha(original.data)=='7cdd6bfea74652b900250faae275c754ef1d32966b9f314a729e46b980337b85'
    dw=original.dwarf();parser=DWARFExprParser(dw.structs)
    inventory={r['low_pc']:r for r in csv.DictReader((root/'.recovery/2022-functions.csv').open()) if r['low_pc']}
    def die(at):return dw.get_DIE_from_refaddr(int(inventory[at]['die_offset'],16))
    aliases=[]
    for row in record['helpers']+record['card_callers']:
        key=(row['source'],row['name'],row['definition_sha256']);assert key in receipts,row['name']
        e=receipts[key];text=read(root/row['source']);a,b=span(text,row['name'])
        assert sha(text[a:b].encode())==row['definition_sha256'],row['name']
        for part in row['native_ranges']:
            lo=int(part['address'],16);hi=int(part['end'],16)
            assert sha(image.pe.get_data(lo-0x2000000,hi-lo))==part['bytes_sha256']
            assert sha((root/part['raw_export']).read_bytes())==part['raw_export_sha256']
            fn=die(part['address']);source=value(fn.cu.get_top_DIE(),'DW_AT_name').replace('\\','/').removeprefix('./')
            assert source==row['native_source']==inventory[part['address']]['source']
            assert source==e['native_source_reconstruction']['native_source']
        if row['source']!=row['native_source']:
            assert (row['source'],row['native_source'])==('cards/odissey.c','cards/odyssey.c')
            aliases.append(row['name'])
    assert len(aliases)==18
    def parameters(at):
        result={};constants={};stack={}
        for p in die(at).iter_children():
            if p.tag!='DW_TAG_formal_parameter':continue
            name=inherited_value(p,'DW_AT_name');const=value(p,'DW_AT_const_value')
            if const is not None:constants[name]=const
            a=p.attributes.get('DW_AT_location')
            if not a:continue
            if a.form=='DW_FORM_sec_offset':
                result[name]=[[(op.op_name,op.args) for op in parser.parse_expr(x.loc_expr)]
                    for x in dw.location_lists().get_location_list_at_offset(a.value,p) if hasattr(x,'loc_expr')]
            elif a.form=='DW_FORM_exprloc':stack[name]=a.value
        return result,constants,stack
    for at,registers,eliminated in [
        ('0x212ba50',{'player':0,'event':2},['card']),
        ('0x2310dc0',{'player':0,'card':2,'crds':1},['event']),
        ('0x2310f80',{'player':0,'card':2},['event'])]:
        locs,constants,stack=parameters(at)
        for name,reg in registers.items():assert locs[name][0]==[('DW_OP_reg'+str(reg),[])]
        for name in eliminated:
            assert all([op[0] for op in expr]==['DW_OP_GNU_parameter_ref','DW_OP_stack_value'] for expr in locs[name])
    locs,constants,stack=parameters('0x23114c0')
    assert constants=={'lif':3,'white':0,'red':0,'colorless':1}
    assert stack=={'black':[145,0],'blue':[145,4],'green':[145,8]}
    locs,constants,stack=parameters('0x2311120')
    assert constants=={'key':0,'tou':1,'pow':1} and stack=={'s_key':[145,0]}
    assert inherited_value(die('0x2502360'),'DW_AT_type') is None
    rebuilt=Image((root/'ManalinkEh.dbg').read_bytes());found_void=False;found_aliases=set()
    for cu in rebuilt.dwarf().iter_CUs():
        source=value(cu.get_top_DIE(),'DW_AT_name','').replace('\\','/').removeprefix('./')
        if source!='cards/odissey.c':continue
        for fn in cu.iter_DIEs():
            if fn.tag!='DW_TAG_subprogram' or not value(fn,'DW_AT_low_pc'):continue
            name=inherited_value(fn,'DW_AT_name')
            if name in aliases:found_aliases.add(name)
            if name=='od_shrine':
                assert inherited_value(fn,'DW_AT_type') is None
                found_void=True
    assert found_void and found_aliases==set(aliases),(found_void,set(aliases)-found_aliases)
    assert len(record['helpers'])==record['helper_definitions']==32
    assert sum(len(r['native_ranges']) for r in record['helpers'])==record['native_helper_ranges']==34
    assert sum(r['source_changed'] for r in record['helpers'])==record['changed_helper_definitions']==13
    assert len(record['card_callers'])==12 and sum(r['source_changed'] for r in record['card_callers'])==record['changed_card_callers']==5
    return dict(helper_definitions=32,native_helper_ranges=34,changed_helpers=13,complete_card_callers=12,
      changed_card_callers=5,native_and_rebuilt_void_shrine=True,verified_historical_filename_mappings=18,
      optimized_argument_mapping=True,scope='Complete native/source receipts; native/rebuilt Odyssey filename and void API checks; private worker register, eliminated and fixed parameter identities. Built-in dialogs and gameplay tests remain downstream.')


if __name__=='__main__':
    from canonicalize_helpers import load
    print(json.dumps(verify(load(),Image((ROOT/'.recovery/2022-ManalinkEh.dll').read_bytes())),indent=2))
