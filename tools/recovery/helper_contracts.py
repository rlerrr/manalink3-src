"""Conservative DWARF calling/storage contracts for automatic source retention."""
import json,struct
from pathlib import Path
from inspect_release import Image,value,inherited_value
from helper_fingerprints import digest
from recover_temporary_exile import sha


def owner(die):
    seen=set()
    while die is not None and die.offset not in seen:
        seen.add(die.offset)
        ref=next((name for name in ['DW_AT_abstract_origin','DW_AT_specification'] if name in die.attributes),None)
        if not ref:break
        die=die.get_DIE_from_attribute(ref)
    return die


def referenced_type(die):
    die=owner(die)
    return die.get_DIE_from_attribute('DW_AT_type') if die is not None and 'DW_AT_type' in die.attributes else None


def type_contract(die,depth=0):
    if die is None:return ['void']
    if depth>6:return ['unknown','type recursion']
    die=owner(die)
    if die.tag in {'DW_TAG_typedef','DW_TAG_const_type','DW_TAG_volatile_type','DW_TAG_restrict_type','DW_TAG_atomic_type'}:
        return type_contract(referenced_type(die),depth+1)
    size=value(die,'DW_AT_byte_size')
    if die.tag=='DW_TAG_base_type':return ['scalar',size,value(die,'DW_AT_encoding')]
    if die.tag=='DW_TAG_enumeration_type':return ['enum',size]
    if die.tag in {'DW_TAG_structure_type','DW_TAG_union_type','DW_TAG_class_type'}:
        return ['aggregate',die.tag,size]
    if die.tag in {'DW_TAG_pointer_type','DW_TAG_reference_type','DW_TAG_rvalue_reference_type'}:
        target=referenced_type(die)
        if target is not None:
            while target.tag in {'DW_TAG_typedef','DW_TAG_const_type','DW_TAG_volatile_type','DW_TAG_restrict_type'}:
                target=referenced_type(target)
                if target is None:break
        if target is not None and target.tag=='DW_TAG_subroutine_type':
            return ['code_pointer',size or die.cu['address_size'],function_contract(target,depth+1)]
        # Include pointee storage: a 400-byte predecessor report is not the
        # native 104-byte record, even in a body that never dereferences it.
        return ['data_pointer',size or die.cu['address_size'],type_contract(target,depth+1)]
    if die.tag=='DW_TAG_array_type':
        dims=[]
        for child in die.iter_children():
            if child.tag!='DW_TAG_subrange_type':continue
            count=value(child,'DW_AT_count');upper=value(child,'DW_AT_upper_bound')
            dims.append(count if count is not None else upper+1 if isinstance(upper,int) else 'unknown')
        return ['array',dims,type_contract(referenced_type(die),depth+1)] if dims and 'unknown' not in dims else ['unknown','array extent']
    if die.tag=='DW_TAG_unspecified_type' and value(die,'DW_AT_name')=='void':return ['void']
    return ['unknown',die.tag]


def function_contract(die,depth=0):
    die=owner(die);parameters=[];variadic=False
    for child in die.iter_children():
        if child.tag=='DW_TAG_formal_parameter':
            target=referenced_type(child)
            parameters.append(type_contract(target,depth+1) if target is not None else ['unknown','parameter type'])
        elif child.tag=='DW_TAG_unspecified_parameters':variadic=True
    return dict(result=type_contract(referenced_type(die),depth+1),parameters=parameters,
                variadic=variadic,calling_convention=inherited_value(die,'DW_AT_calling_convention',1))


def supported(contract):
    if isinstance(contract,dict):return all(supported(v) for v in contract.values())
    if isinstance(contract,list):return (not contract or contract[0]!='unknown') and all(supported(v) for v in contract)
    return contract is not None


def key(source,name,address):return source+'|'+name+'|'+hex(address)


def inventory(data):
    image=Image(data);dwarf=image.dwarf();info=image.section_data(image.sections['.debug_info']);offset=0;result={}
    while offset+4<=len(info):
        length=struct.unpack_from('<I',info,offset)[0]
        if length==0:break
        assert length!=0xffffffff,'DWARF64 contract inventory unsupported'
        cu=dwarf.get_CU_at(offset);offset+=4+length
        source=value(cu.get_top_DIE(),'DW_AT_name','').replace('\\','/').removeprefix('./')
        if source.startswith(('cards/','functions/')):
            for die in cu.iter_DIEs():
                if die.tag!='DW_TAG_subprogram' or not value(die,'DW_AT_low_pc'):continue
                name=inherited_value(die,'DW_AT_name','')
                if not name:continue
                contract=function_contract(die)
                k=key(source,name,value(die,'DW_AT_low_pc'));values=result.setdefault(k,[])
                if contract not in values:values.append(contract)
        cu._dielist.clear();cu._diemap.clear()
    return result


def annotate(rows,root):
    """Cache pure contracts by both debug-file hashes and extractor source."""
    original_path=Path('G:/Manalink/ManalinkEh_10_22_Hotfix.dbg');rebuilt_path=root/'ManalinkEh.dbg'
    original=original_path.read_bytes();rebuilt=rebuilt_path.read_bytes()
    inputs=dict(original_debug_sha256=sha(original),rebuilt_debug_sha256=sha(rebuilt),extractor_sha256=sha(Path(__file__).read_bytes()))
    assert inputs['original_debug_sha256']=='7cdd6bfea74652b900250faae275c754ef1d32966b9f314a729e46b980337b85'
    cache=root/'.recovery/reccmp/triage-function-contracts.json'
    stored=json.loads(cache.read_text()) if cache.exists() else None
    if not stored or stored.get('inputs')!=inputs:
        previous=stored or {};previous_inputs=previous.get('inputs',{})
        # The pinned original debug file does not change on each source build.
        # Reuse each side independently, but only under the same extractor.
        same_extractor=previous_inputs.get('extractor_sha256')==inputs['extractor_sha256']
        inventories={}
        for side,data in [('original',original),('rebuilt',rebuilt)]:
            field=side+'_debug_sha256'
            reusable=same_extractor and previous_inputs.get(field)==inputs[field] and side in previous
            inventories[side]=previous[side] if reusable else inventory(data)
        stored=dict(inputs=inputs,**inventories)
        cache.write_text(json.dumps(stored,separators=(',',':'))+'\n')
    checked=matched=0
    for row in rows:
        if row['category']!='same_local_code' or not row.get('rebuilt_address'):continue
        checked+=1
        old=stored['original'].get(key(row['source'],row['name'],int(row['address'],16)),[])
        new=stored['rebuilt'].get(key(row['source'],row['name'],int(row['rebuilt_address'],16)),[])
        passed=len(old)==len(new)==1 and old==new and supported(old[0])
        matched+=bool(passed)
        row['storage_contract']=dict(verified=bool(passed),native=old,rebuilt=new,
            fingerprint_sha256=digest(old[0]) if len(old)==1 else None,
            scope='Return/parameter count, variadic/calling convention, scalar encoding and widths, enum storage, pointer/pointee storage, and by-value record sizes. Not complete record-member or enum-value equivalence.')
    return dict(checked_ranges=checked,matching_ranges=matched,manual_contract_review=checked-matched,
                cache=cache.relative_to(root).as_posix(),inputs=inputs,cache_sha256=sha(cache.read_bytes()),
                original_debug_path=str(original_path),scope='Calling/storage contracts are an additional gate; unknown or ambiguous types remain manual review.')
