"""Read native DWARF call-site arguments for retained callers of changed APIs.

This is an evidence inventory, not a source rewrite or equivalence claim. GCC
records physical argument locations and values at individual call sites; read
those directly instead of decompiling every caller. No game code is executed.
"""
import argparse,collections,csv,json,re
from pathlib import Path
from inspect_release import Image,value,inherited_value
from unify_scalar_helpers import ROOT,read,mask,parameters,sha

HELPERS=set('''is_humiliated add_or_remove_counters_impl add_counters remove_counters
remove_all_counters copy_counters add_counters_predoubled move_counters
generic_split_card leaves_play other_leaves_play get_subtypes_by_csvid
has_subtype_by_id reset_subtypes'''.split())
DEBUG_SHA='7cdd6bfea74652b900250faae275c754ef1d32966b9f314a729e46b980337b85'
DLL_SHA='52793f60124a3b83d8b2728d145e918101a395b4e8d1c0af69cddcfc87ef10e5'
SOURCE_ALIASES={'cards/odissey.c':'cards/odyssey.c'}

def source_calls(root=ROOT):
    # A canonicalized API now uses its original name for valid native calls.
    # Keep malformed/legacy arities visible, rather than recounting the native
    # calls as pending migrations after each consolidation.
    checkpoint=root/'docs/recovery-evidence/helper-canonicalization.json'
    canonical_arities={}
    if checkpoint.exists():
        for entry in json.loads(checkpoint.read_text())['functions']:
            prototype=entry['source_prototype']
            args=parameters(prototype[prototype.index('(')+1:prototype.rindex(')')])
            canonical_arities[entry['canonical_name']]=0 if args==['void'] else len(args)
    pattern=re.compile(r'\b('+'|'.join(sorted(HELPERS))+r')\s*\(')
    definition=re.compile(r'(?m)^[ \t]*(?:[A-Za-z_]\w*[ \t\r\n*]+)+([A-Za-z_]\w*)\s*\([^;{}]*\)\s*\{')
    result=collections.defaultdict(list)
    for folder in ['cards','functions']:
        for path in (root/folder).glob('*.c'):
            text=read(path)
            if not any(n in text for n in HELPERS):continue
            code=mask(text);source=path.relative_to(root).as_posix()
            definitions=[(m.start(),m[1]) for m in definition.finditer(code) if m[1] not in {'if','for','while','switch'}]
            for m in pattern.finditer(code):
                name=m[1];caller=next((n for lo,n in reversed(definitions) if lo<=m.start()),None)
                # These obsolete helper bodies will themselves be replaced.
                if not caller or caller in HELPERS:continue
                depth=1
                for at in range(m.end(),len(code)):
                    depth+=(code[at]=='(')-(code[at]==')')
                    if not depth:break
                assert not depth,(source,caller,name)
                arguments=[a.strip() for a in parameters(text[m.end():at])]
                if canonical_arities.get(name)==len(arguments):continue
                result[(source,caller)].append(dict(helper=name,line=text.count('\n',0,m.start())+1,
                    call=text[m.start():at+1],arguments=arguments))
    return result

def operations(parser,raw):
    if raw is None:return None
    try:return [dict(operation=o.op_name,arguments=o.args) for o in parser.parse_expr(raw)]
    except (KeyError,ValueError,AssertionError) as e:return dict(raw=raw,error=str(e))

def main():
    checkpoint=ROOT/'docs/recovery-evidence/helper-canonicalization.json'
    suffix=sha(checkpoint.read_bytes())[:12] if checkpoint.exists() else 'initial'
    p=argparse.ArgumentParser(__doc__);p.add_argument('--output',type=Path,default=ROOT/'.recovery'/('native-helper-call-arguments-'+suffix+'.json'));args=p.parse_args()
    assert not args.output.exists(),'Call inventories can be sealed evidence inputs; choose a new output path instead of overwriting one.'
    callers=source_calls();index=collections.defaultdict(list)
    census=ROOT/'.recovery/2022-functions.csv'
    for row in csv.DictReader(census.open()):
        if not row['low_pc'] or not row['high_pc']:continue
        keys=[(row['source'],row['name'])]+[(old,row['name']) for old,new in SOURCE_ALIASES.items() if row['source']==new]
        for key in keys:
            if key in callers:index[key].append(row)
    debug=Path('G:/Manalink/ManalinkEh_10_22_Hotfix.dbg').read_bytes();assert sha(debug)==DEBUG_SHA
    dwarf=Image(debug).dwarf()
    runtime=Image((ROOT/'.recovery/2022-ManalinkEh.dll').read_bytes());assert sha(runtime.data)==DLL_SHA
    from elftools.dwarf.dwarf_expr import DWARFExprParser
    parser=DWARFExprParser(dwarf.structs);result=[]
    def walk(die):
        yield die
        for child in die.iter_children():yield from walk(child)
    for (source,name),calls in sorted(callers.items()):
        ranges=[];sites=[]
        for native in index[(source,name)]:
            die=dwarf.get_DIE_from_refaddr(int(native['die_offset'],16))
            assert inherited_value(die,'DW_AT_name')==name
            assert value(die.cu.get_top_DIE(),'DW_AT_name')==native['source']
            parameters_native=[]
            for child in die.iter_children():
                if child.tag!='DW_TAG_formal_parameter':continue
                location=value(child,'DW_AT_location')
                parameters_native.append(dict(name=inherited_value(child,'DW_AT_name'),
                    location=operations(parser,location) if isinstance(location,list) else dict(location_list_offset=location),
                    constant=value(child,'DW_AT_const_value')))
            ranges.append(dict(source=native['source'],die_offset=native['die_offset'],low_pc=native['low_pc'],high_pc=native['high_pc'],
                               frame_base=operations(parser,value(die,'DW_AT_frame_base')),parameters=parameters_native))
            for child in walk(die):
                if child.tag!='DW_TAG_GNU_call_site' or 'DW_AT_abstract_origin' not in child.attributes:continue
                callee=child.get_DIE_from_attribute('DW_AT_abstract_origin');target=inherited_value(callee,'DW_AT_name')
                if target not in HELPERS:continue
                pc=value(child,'DW_AT_low_pc');arguments=[]
                for argument in child.iter_children():
                    if argument.tag!='DW_TAG_GNU_call_site_parameter':continue
                    arguments.append(dict(location=operations(parser,value(argument,'DW_AT_location')),
                        value=operations(parser,value(argument,'DW_AT_GNU_call_site_value')),
                        data_value=operations(parser,value(argument,'DW_AT_GNU_call_site_data_value'))))
                anchor=runtime.pe.get_data(pc-5-0x2000000,5) if pc else b''
                direct=None
                if len(anchor)==5 and anchor[0]==0xe8:
                    direct=hex(pc+int.from_bytes(anchor[1:],'little',signed=True))
                sites.append(dict(helper=target,die_offset=hex(child.offset),return_pc=hex(pc) if pc else None,
                    tail_call=bool(value(child,'DW_AT_GNU_tail_call',False)),arguments=arguments,
                    preceding_bytes=anchor.hex(),direct_call_target=direct))
        result.append(dict(source=source,caller=name,source_calls=calls,native_ranges=ranges,native_sites=sites,
                           status='native call-site evidence' if ranges else 'no matching concrete native caller range'))
        if len(result)%200==0:print('CALL_ARGUMENTS',len(result),'/',len(callers),flush=True)
    report=dict(format=1,debug_sha256=DEBUG_SHA,dll_sha256=DLL_SHA,census_sha256=sha(census.read_bytes()),
        callers=result,counts=dict(callers=len(result),source_calls=sum(len(r['source_calls']) for r in result),
                                  native_sites=sum(len(r['native_sites']) for r in result),
                                  unmatched_callers=sum(not r['native_ranges'] for r in result)),
        scope='Read-only native physical argument inventory. Call order, matching names or matching counts do not alone prove source-call correspondence.')
    args.output.write_text(json.dumps(report,indent=2)+'\n');print(json.dumps(report['counts']))

if __name__=='__main__':main()
