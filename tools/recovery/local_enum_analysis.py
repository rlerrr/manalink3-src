"""Check function-local enum identities and emitted values without evaluating C."""
import collections,re
from canonicalize_helpers import read,mask,span,sha
from inspect_release import value,inherited_value


def enum_values(die):
    assert die.tag=='DW_TAG_enumeration_type' and not value(die,'DW_AT_declaration',False)
    return [[value(c,'DW_AT_name'),value(c,'DW_AT_const_value')] for c in die.iter_children() if c.tag=='DW_TAG_enumerator']


def validate_native(dw,row):
    die=dw.get_DIE_from_refaddr(int(row['native_enum_die'],16))
    assert enum_values(die)==row['enumerators']
    assert value(die,'DW_AT_byte_size')==row['size']==4
    assert value(die,'DW_AT_name')==row['tag_name']==None
    assert value(die,'DW_AT_enum_class',False)==row['scoped']==False
    assert value(die,'DW_AT_decl_line')==row['decl_line']
    line=dw.line_program_for_CU(die.cu);file_index=value(die,'DW_AT_decl_file')
    assert line['file_entry'][file_index-1].name.decode('utf8')==row['decl_file']
    owner=die.get_parent()
    while owner.tag not in ['DW_TAG_subprogram','DW_TAG_compile_unit']:owner=owner.get_parent()
    assert owner.tag=='DW_TAG_subprogram' and hex(owner.offset)==row['native_owner_die']
    assert inherited_value(owner,'DW_AT_name')==row['native_owner']
    source=value(die.cu.get_top_DIE(),'DW_AT_name','').replace('\\','/')
    assert row['source']==source.replace('cards/odyssey.c','cards/odissey.c')
    return die,owner


def current_enums(debug,rows):
    wanted={(r['source'],r['name']) for r in rows};sources={s for s,n in wanted}
    dw=debug.dwarf();result=collections.defaultdict(list)
    def local_walk(die):
        for child in die.iter_children():
            if child.tag=='DW_TAG_subprogram':continue
            yield child
            if child.has_children:yield from local_walk(child)
    for cu in dw.iter_CUs():
        source=value(cu.get_top_DIE(),'DW_AT_name','').replace('\\','/')
        if source not in sources:continue
        for fn in cu.get_top_DIE().iter_children():
            if fn.tag!='DW_TAG_subprogram':continue
            key=(source,inherited_value(fn,'DW_AT_name'))
            if key not in wanted:continue
            for die in local_walk(fn):
                if die.tag!='DW_TAG_enumeration_type' or value(die,'DW_AT_declaration',False):continue
                values=enum_values(die)
                if values:result[key].append(dict(die=hex(die.offset),size=value(die,'DW_AT_byte_size'),
                    tag_name=value(die,'DW_AT_name'),scoped=value(die,'DW_AT_enum_class',False),values=values))
        cu._dielist.clear();cu._diemap.clear()
    return dw,result


def source_declaration(root,row,cache=None):
    cache={} if cache is None else cache
    source=row['source']
    if source not in cache:cache[source]=read(root/source)
    text=cache[source];a,b=span(text,row['name']);body=text[a:b];code=mask(body)
    names=[n for n,v in row['enumerators']]
    if row.get('source_name_alias'):names=[row['source_name_alias'].get(n,n) for n in names]
    present=[n for n in names if re.search(r'\b'+re.escape(n)+r'\b',code)]
    declarations=[]
    for match in re.finditer(r'\benum\s*\{',code):
        end=code.index('}',match.end())
        block=code[match.start():end+1]
        if all(re.search(r'\b'+re.escape(n)+r'\b',block) for n in names):
            declarations.append(body[match.start():end+1])
    if not present:
        assert not declarations
        return dict(kind='missing_local_enum',function_sha256=sha(body.encode()))
    assert present==names and len(declarations)==1,(source,row['name'],names,present,declarations)
    return dict(kind='existing_name_alias' if row.get('source_name_alias') else 'existing_native_names',
        declaration=declarations[0],declaration_sha256=sha(declarations[0].encode()))


def match_compiled(row,enums):
    expected=[[row.get('source_name_alias',{}).get(n,n),v] for n,v in row['enumerators']]
    names={n for n,v in expected}
    candidates=[e for e in enums if names&{n for n,v in e['values']}]
    if not candidates:return None
    assert len(candidates)==1,(row['source'],row['name'],candidates)
    found=candidates[0]
    assert found['values']==expected and found['size']==row['size']==4
    assert found['tag_name']==None and found['scoped']==False
    return found
