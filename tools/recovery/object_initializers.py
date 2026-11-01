"""Typed, read-only checks for the non-arithmetic project initializer inventory.

Pointer words are translated only through matched object identities, exact
readonly character strings, or unique same-name code symbols. Unknown addresses
remain unresolved; raw byte equality never hides a moved DLL pointer target.
"""
import argparse,ast,collections,json,re
from pathlib import Path
from canonicalize_helpers import ROOT,sha,read,span,mask
from global_initializers import inherited_type,load_bytes,shape
from inspect_release import Image,value,inherited_value

QUALIFIERS={'DW_TAG_typedef','DW_TAG_const_type','DW_TAG_volatile_type','DW_TAG_restrict_type'}


def unqualify(d):
    seen=set()
    while d is not None and d.tag in QUALIFIERS and d.offset not in seen:
        seen.add(d.offset);d=inherited_type(d)
    return d


def array_count(d):
    result=1;found=False
    for c in d.iter_children():
        if c.tag!='DW_TAG_subrange_type':continue
        n=value(c,'DW_AT_count');upper=value(c,'DW_AT_upper_bound');lower=value(c,'DW_AT_lower_bound',0)
        if n is None and isinstance(upper,int) and isinstance(lower,int):n=upper-lower+1
        if not isinstance(n,int) or n<0:raise ValueError('dynamic array bound')
        result*=n;found=True
    if not found:raise ValueError('no array bound')
    return result


def sizeof(d):
    d=unqualify(d)
    if d is None:raise ValueError('missing type')
    n=value(d,'DW_AT_byte_size')
    if isinstance(n,int):return n
    if d.tag in ['DW_TAG_pointer_type','DW_TAG_reference_type','DW_TAG_rvalue_reference_type']:return 4
    if d.tag=='DW_TAG_array_type':return array_count(d)*sizeof(inherited_type(d))
    raise ValueError('type has no fixed size: '+d.tag)


def member_offset(d):
    loc=value(d,'DW_AT_data_member_location',0)
    if isinstance(loc,int):return loc
    if isinstance(loc,list) and loc and loc[0]==0x23:
        result=0;shift=0
        for x in loc[1:]:result|=(x&127)<<shift;shift+=7
        return result
    raise ValueError('unsupported member/base location')


def pointer_slots(d,cache,seen=()):
    d=unqualify(d)
    if d is None:return []
    if d.offset in cache:return cache[d.offset]
    if d.offset in seen:raise ValueError('recursive by-value type')
    seen=seen+(d.offset,);result=[]
    if d.tag in ['DW_TAG_pointer_type','DW_TAG_reference_type','DW_TAG_rvalue_reference_type']:
        assert sizeof(d)==4
        target=unqualify(inherited_type(d));char=target is not None and target.tag=='DW_TAG_base_type' and sizeof(target)==1 and value(target,'DW_AT_encoding') in [5,6,7,8]
        result=[dict(offset=0,kind='character' if char else ('function' if target is not None and target.tag=='DW_TAG_subroutine_type' else 'object'))]
    elif d.tag=='DW_TAG_array_type':
        inner=inherited_type(d);slots=pointer_slots(inner,cache,seen)
        if slots:
            stride=sizeof(inner)
            result=[dict(slot,offset=i*stride+slot['offset']) for i in range(array_count(d)) for slot in slots]
    elif d.tag in ['DW_TAG_structure_type','DW_TAG_class_type','DW_TAG_union_type']:
        for field in d.iter_children():
            if field.tag not in ['DW_TAG_member','DW_TAG_inheritance'] or value(field,'DW_AT_declaration',False):continue
            slots=pointer_slots(inherited_type(field),cache,seen)
            if slots:
                offset=0 if d.tag=='DW_TAG_union_type' else member_offset(field)
                result.extend(dict(slot,offset=offset+slot['offset']) for slot in slots)
    elif d.tag=='DW_TAG_ptr_to_member_type':raise ValueError('member-function pointer requires separate ABI review')
    unique={(r['offset'],r['kind']):r for r in result};cache[d.offset]=list(unique.values());return cache[d.offset]


def key(r):return r['source'].replace('cards/odyssey.c','cards/odissey.c'),r['name'],tuple(r['scopes'])


def inventory(label,debug_path,dll_path,output):
    assert label in ['native','rebuilt']
    debug=Image(Path(debug_path).read_bytes());image=Image(Path(dll_path).read_bytes());dw=debug.dwarf();rows=[]
    assert image.pe.FILE_HEADER.Machine==0x14c and image.pe.OPTIONAL_HEADER.ImageBase==0x2000000
    for cu in dw.iter_CUs():
        source=value(cu.get_top_DIE(),'DW_AT_name','').replace('\\','/')
        if not source.startswith(('cards/','functions/')):continue
        for d in cu.iter_DIEs():
            if d.tag!='DW_TAG_variable' or value(d,'DW_AT_declaration',False):continue
            location=d.attributes.get('DW_AT_location');name=inherited_value(d,'DW_AT_name')
            if not location or not isinstance(location.value,list) or len(location.value)!=5 or location.value[0]!=3 or not name:continue
            t=inherited_type(d)
            if shape(t) is not None:continue
            at=int.from_bytes(bytes(location.value[1:]),'little');length=sizeof(t)
            data=load_bytes(image.pe,at,length) if 0<=length<=1000000 else None
            scopes=[];parent=d.get_parent()
            while parent and parent.tag!='DW_TAG_compile_unit':
                n=inherited_value(parent,'DW_AT_name')
                if n:scopes.append(n)
                parent=parent.get_parent()
            rows.append(dict(source=source,name=name,scopes=list(reversed(scopes)),die_offset=hex(d.offset),address=hex(at),type_die=hex(t.offset),type_tag=t.tag,type_name=inherited_value(t,'DW_AT_name'),size=length,bytes=data.hex() if data is not None else None,sha256=sha(data) if data is not None else None))
        cu._dielist.clear();cu._diemap.clear()
    result={label+'_dll_sha256':sha(image.data),label+'_dbg_sha256':sha(debug.data),'objects':rows,'extractor_sha256':sha(Path(__file__).read_bytes()),'scope':'Non-arithmetic direct-address project variables, including resumed C++ picker. Inventory only; initializer equivalence is checked separately.'}
    Path(output).write_text(json.dumps(result,indent=2)+'\n');print(json.dumps(dict(inventory=label,objects=len(rows),output=output)))


def readonly_string(image,at):
    section=image.pe.get_section_by_rva(at-0x2000000)
    if section is None or section.Characteristics&0x80000000:return None
    maximum=min(4096,section.VirtualAddress+section.Misc_VirtualSize-(at-0x2000000))
    data=image.pe.get_data(at-0x2000000,maximum);end=data.find(b'\0')
    return data[:end+1] if end>=0 else None


def match_pointer(native,current,identity_map,symbols,x,y,kind,expected_string=None):
    if x==0 or y==0:return (x==y,'null',None)
    old_inside=0x2000000<=x<0x2000000+native.pe.OPTIONAL_HEADER.SizeOfImage
    new_inside=0x2000000<=y<0x2000000+current.pe.OPTIONAL_HEADER.SizeOfImage
    if not old_inside or not new_inside:return (x==y and not old_inside and not new_inside,'fixed_external_word',None)
    candidates=[(at,binding) for at,binding in identity_map.items() if at==x]
    if not candidates:candidates=[(at,binding) for at,binding in identity_map.items() if at<x<at+binding[1]['size']]
    for at,(new_at,old_object,new_object) in candidates:
        if y==new_at+x-at:
            return True,'matched_project_object',dict(source=old_object['source'],name=old_object['name'],scopes=old_object['scopes'],offset=x-at)
    if kind=='character' and expected_string is not None:
        left,right=readonly_string(native,x),readonly_string(current,y)
        if left is not None and left==right==expected_string:return True,'identical_source_literal_readonly_string',dict(bytes=left.hex(),sha256=sha(left))
    aliases=[name for name,values in symbols[0].items() if values=={x} and symbols[1].get(name)=={y}]
    if aliases:return True,'unique_same_name_code_entry',dict(names=sorted(aliases))
    return False,'unresolved_pointer',None


def main():
    p=argparse.ArgumentParser(__doc__);p.add_argument('--output',type=Path,default=ROOT/'docs/recovery-evidence/object-initializer-coverage.json');p.add_argument('--inventory',nargs=4,metavar=('LABEL','DEBUG','DLL','OUTPUT'));args=p.parse_args()
    if args.inventory:inventory(*args.inventory);return
    native_path=ROOT/'.recovery/native-other-global-initializers.json';rebuilt_path=ROOT/'.recovery/rebuilt-other-global-initializers.json'
    old=json.loads(native_path.read_text());new=json.loads(rebuilt_path.read_text())
    paths=[ROOT/'.recovery/2022-ManalinkEh.dll',Path('G:/Manalink/ManalinkEh_10_22_Hotfix.dbg'),ROOT/'ManalinkEh.dll',ROOT/'ManalinkEh.dbg']
    native,nd,current,cd=[Image(path.read_bytes()) for path in paths]
    assert sha(native.data)==old['native_dll_sha256']=='52793f60124a3b83d8b2728d145e918101a395b4e8d1c0af69cddcfc87ef10e5'
    assert sha(nd.data)==old['native_dbg_sha256'] and sha(current.data)==new['rebuilt_dll_sha256'] and sha(cd.data)==new['rebuilt_dbg_sha256']
    dw=nd.dwarf();cw=cd.dwarf();caches=[{},{}]
    a=collections.defaultdict(list);b=collections.defaultdict(list)
    for r in old['objects']:a[key(r)].append(r)
    for r in new['objects']:b[key(r)].append(r)
    moved={
        ('cards/new_phyrexia.c','words',('charge_phyrexian_mana_impl',)):('cards/new_phyrexia.c','phyrexian_color_words',()),
        ('functions/windows.c','set_legacy_effect_name_addr',()):('functions/functions.c','set_legacy_effect_name_addr',()),
        ('functions/targets.c','rval',('becomes_target_of_subtype_spell',)):('functions/targets.c','targeted_subtype_spell_result',()),
        ('functions/auras.c','recalculating_ward',('this_effect_doesnt_remove_this',)):('functions/auras.c','ward_recalculation',()),
        ('functions/functions.c','words',('get_sleighted_color_text',)):('functions/functions.c','words',('get_sleighted_color_text2',)),
    }
    pairs=[];pending=[];identity_map={}
    for identity,rows in a.items():
        candidates=b.get(moved.get(identity,identity),[])
        if len(rows)!=1 or len(candidates)!=1:pending.append(dict(identity=list(identity[:2])+[list(identity[2])],reason='missing or ambiguous current object'));continue
        r,s=rows[0],candidates[0];pairs.append((r,s,identity in moved));identity_map[int(r['address'],16)]=(int(s['address'],16),r,s)
    # Extend object-address translation with independently checked arithmetic
    # identity moves. A representation-only inlining is not an object binding.
    arithmetic_path=ROOT/'docs/recovery-evidence/arithmetic-initializer-coverage.json';arithmetic=json.loads(arithmetic_path.read_text())
    for path,digest in arithmetic['inputs'].items():assert sha(Path(path).read_bytes())==digest,path
    for r in arithmetic['coverage']:
        if 'rebuilt' in r:identity_map[int(r['native']['address'],16)]=(int(r['rebuilt']['address'],16),r['native'],r['rebuilt'])
    symbols=[]
    for image in [nd,cd]:
        names=collections.defaultdict(set)
        for name,v,s,k,c in image.symbols:
            if s>0 and k==32:names[name].add(0x2000000+image.pe.sections[s-1].VirtualAddress+v)
        symbols.append(names)
    source_hashes={};literal_cache={}
    def character_literals(row):
        identity=key(row)
        if identity in literal_cache:return literal_cache[identity]
        path=ROOT/row['source'];text=read(path)
        assert path.stat().st_mtime_ns<=(ROOT/'ManalinkEh.dll').stat().st_mtime_ns,path
        source_hashes[row['source']]=sha(path.read_bytes())
        if row['scopes']:
            bounds=span(text,row['scopes'][-1]);assert bounds,row['scopes']
            text=text[slice(*bounds)]
        clean=mask(text)
        found=re.search(r'\b'+re.escape(row['name'])+r'\s*(?:\[[^\]]*\]\s*)+=\s*\{',clean)
        if found is None:literal_cache[identity]=None;return None
        start=found.end()-1;end=start+1;depth=1
        while depth:
            depth+=(clean[end]=='{')-(clean[end]=='}');end+=1
        block=text[start:end];pattern=r'"(?:\\.|[^"\\])*"'
        # This array's elements must actually be C string literals. A char*
        # type alone cannot establish a NUL-terminated-string extent.
        rest=re.sub(pattern,'',block)
        if re.sub(r'[\s{},]','',rest):literal_cache[identity]=None;return None
        literals=[ast.literal_eval(m[0]).encode('latin1')+b'\0' for m in re.finditer(pattern,block)]
        if len(literals)*4!=row['size']:literal_cache[identity]=None;return None
        literal_cache[identity]={4*i:value for i,value in enumerate(literals)}
        return literal_cache[identity]
    pointer_bindings=[]
    covered=[]
    for r,s,migration in pairs:
        before=bytes.fromhex(r['bytes']);after=bytes.fromhex(s['bytes']);entry=dict(native=r,rebuilt=s,source_identity_migration=migration)
        for data,row in [(dw,r),(cw,s)]:
            die=data.get_DIE_from_refaddr(int(row['die_offset'],16));location=die.attributes['DW_AT_location'].value
            assert die.tag=='DW_TAG_variable' and inherited_value(die,'DW_AT_name')==row['name']
            assert isinstance(location,list) and len(location)==5 and location[0]==3
            assert int.from_bytes(bytes(location[1:]),'little')==int(row['address'],16)
            declared=inherited_type(die);assert declared.offset==int(row['type_die'],16) and sizeof(declared)==row['size']
            assert value(die.cu.get_top_DIE(),'DW_AT_name').replace('\\','/')==row['source']
        assert load_bytes(native.pe,int(r['address'],16),r['size'])==before and load_bytes(current.pe,int(s['address'],16),s['size'])==after
        if len(before)!=len(after):pending.append(dict(**entry,reason='different object extent'));continue
        try:
            old_slots=pointer_slots(dw.get_DIE_from_refaddr(int(r['type_die'],16)),caches[0]);new_slots=pointer_slots(cw.get_DIE_from_refaddr(int(s['type_die'],16)),caches[1])
        except ValueError as error:pending.append(dict(**entry,reason=str(error)));continue
        offsets={x['offset'] for x in old_slots}|{x['offset'] for x in new_slots};masked=set();pointers=[];issues=[]
        for offset in sorted(offsets):
            assert 0<=offset and offset+4<=len(before),(r['name'],offset)
            x=int.from_bytes(before[offset:offset+4],'little');y=int.from_bytes(after[offset:offset+4],'little')
            old_kinds={t['kind'] for t in old_slots if t['offset']==offset};new_kinds={t['kind'] for t in new_slots if t['offset']==offset}
            kind='character' if old_kinds==new_kinds=={'character'} else 'object'
            literals=character_literals(s) if kind=='character' and x and y else None
            ok,method,target=match_pointer(native,current,identity_map,symbols,x,y,kind,literals.get(offset) if literals else None)
            if not old_kinds or not new_kinds:
                # A new union pointer view over an unchanged null/sentinel
                # word does not establish a declaration-ABI match.
                ok=ok and x==y and method in ['null','fixed_external_word']
            if not ok:issues.append(dict(offset=offset,native=hex(x),rebuilt=hex(y),reason=method));continue
            masked.update(range(offset,offset+4));pointers.append(dict(offset=offset,native=hex(x),rebuilt=hex(y),method=method,target=target))
            if method not in ['null','fixed_external_word']:pointer_bindings.append((x,y))
        changes=[dict(offset=i,native=x,rebuilt=y) for i,(x,y) in enumerate(zip(before,after)) if x!=y and i not in masked]
        entry.update(pointer_words=pointers,native_pointer_slots=old_slots,rebuilt_pointer_slots=new_slots)
        if issues or changes:pending.append(dict(**entry,reason='unresolved pointer or changed non-pointer bytes',pointer_issues=issues,changed_bytes=changes));continue
        entry['method']='exact_bytes_with_pointer_review' if before==after else 'typed_pointer_translation_and_exact_remaining_bytes';covered.append(entry)
    # Retain alias relationships among the initialized internal pointers that
    # actually occur in this inventory. Equal strings alone cannot waive this.
    forward=collections.defaultdict(set);reverse=collections.defaultdict(set)
    for x,y in pointer_bindings:forward[x].add(y);reverse[y].add(x)
    alias_changes=dict(native_one_to_many={hex(x):[hex(y) for y in sorted(ys)] for x,ys in forward.items() if len(ys)>1},rebuilt_many_to_one={hex(y):[hex(x) for x in sorted(xs)] for y,xs in reverse.items() if len(xs)>1})
    result=dict(status='Typed initializer comparison; no source/proof edits.',native_objects=len(old['objects']),rebuilt_objects=len(new['objects']),covered=len(covered),pending=len(pending),method_counts=dict(collections.Counter(r['method'] for r in covered)),coverage=covered,unresolved=pending,pointer_alias_changes=alias_changes,inputs={str(path):sha(path.read_bytes()) for path in paths+[native_path,rebuilt_path,arithmetic_path,Path(__file__),ROOT/'tools/recovery/global_initializers.py']},source_sha256=source_hashes,scope='Load-time object bytes and typed pointer words only. Pointer translation requires an explicit project-object binding, exact readonly C string literals checked against the current source initializer, or a unique same-name code entry. A char* type alone never establishes string extent. Unknown addresses remain unresolved. Alias changes are reported separately and prevent a full pointer-identity claim. Runtime state, constructor effects, global declaration ABI and gameplay remain separate evidence.')
    assert json.loads(json.dumps(result))==result
    args.output.parent.mkdir(parents=True,exist_ok=True);temp=args.output.with_suffix('.pending.json');temp.write_text(json.dumps(result,indent=2)+'\n');temp.replace(args.output)
    print(json.dumps({k:result[k] for k in ['native_objects','rebuilt_objects','covered','pending','method_counts','pointer_alias_changes']},indent=2))
    print('Pending:',[(r.get('native',{}).get('source'),r.get('native',{}).get('name'),r['reason']) for r in pending])


if __name__=='__main__':main()
