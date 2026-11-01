"""Compare direct-address integer/enum initializer inventories and explicit moves.

Generate inventories with global_initializers.py. This is an initializer-only
cross-check, not pointer/aggregate, runtime-state or declaration-ABI coverage.
"""
import argparse,collections,json,re
from pathlib import Path
from canonicalize_helpers import ROOT,read,span,sha
from global_initializers import load_bytes
from inspect_release import Image
from promote_recovery import strip_comments


def key(row):return row['source'].replace('cards/odyssey.c','cards/odissey.c'),row['name'],tuple(row['scopes'])


def main():
    p=argparse.ArgumentParser(__doc__);p.add_argument('--native',type=Path,default=ROOT/'.recovery/native-arithmetic-globals.json');p.add_argument('--rebuilt',type=Path,default=ROOT/'.recovery/rebuilt-arithmetic-globals.json');p.add_argument('--output',type=Path,default=ROOT/'docs/recovery-evidence/arithmetic-initializer-coverage.json');args=p.parse_args()
    old=json.loads(args.native.read_text());new=json.loads(args.rebuilt.read_text());images=[]
    for inventory in [old,new]:
        assert sha(Path(inventory['debug_path']).read_bytes())==inventory['debug_sha256']
        image=Image(Path(inventory['dll_path']).read_bytes());assert sha(image.data)==inventory['dll_sha256'];images.append(image)
        for row in inventory['globals']:
            data=load_bytes(image.pe,int(row['address'],16),row['size'])
            assert data is not None and data.hex()==row['bytes'] and sha(data)==row['sha256']
    assert old['dll_sha256']=='52793f60124a3b83d8b2728d145e918101a395b4e8d1c0af69cddcfc87ef10e5'
    a=collections.defaultdict(list);b=collections.defaultdict(list)
    for row in old['globals']:a[key(row)].append(row)
    for row in new['globals']:b[key(row)].append(row)
    coverage=[];pending=[];sources={}
    def source(path):
        t=read(ROOT/path);assert (ROOT/path).stat().st_mtime_ns<=(ROOT/'ManalinkEh.dll').stat().st_mtime_ns,path
        sources[path]=sha((ROOT/path).read_bytes());return t
    moves={
        ('cards/war_of_the_spark.c','hack_current_event',()):('cards/war_of_the_spark.c','vivien_current_event',()),
        ('functions/functions.c','hack_current_event',()):('functions/functions.c','flash_current_event',()),
        ('functions/windows.c','hack_show_deck_force_expansion',()):('functions/tutors.c','hack_show_deck_force_expansion',()),
        ('functions/windows.c','buf',('localtime_date',)):('functions/windows.c','date_buffer',('localtime_date',)),
        ('functions/packs.c','templ',('generate_choose_a_pack_dlg_template',)):('functions/packs.c','choose_a_pack_template',()),
    }
    for identity,rows in a.items():
        assert len(rows)==1,identity
        native=rows[0];entry=dict(native=native)
        matches=b.get(identity,[])
        if len(matches)==1:
            if native['bytes']==matches[0]['bytes']:entry.update(method='same_source_name_scope_and_initializer',rebuilt=matches[0])
            else:pending.append(dict(native=native,rebuilt=matches[0],reason='different load-time bytes'));continue
        elif identity in moves:
            replacement=moves[identity];matches=b.get(replacement,[]);assert len(matches)==1,(identity,replacement)
            actual=matches[0];assert native['bytes']==actual['bytes'] and native['size']==actual['size']
            t=source(actual['source'])
            if actual['name'] in ['vivien_current_event','flash_current_event']:
                assert re.search(r'(?m)^static event_t '+actual['name']+r';',t) and native['bytes']=='00000000'
            elif actual['name']=='date_buffer':assert 'static char date_buffer[18];' in t and native['size']==18
            elif actual['name']=='choose_a_pack_template':
                assert 'static unsigned char choose_a_pack_template[294] __attribute__((aligned(4)))' in t
                lo,hi=span(t,'generate_choose_a_pack_dlg_template');assert 'return (LPCDLGTEMPLATE)choose_a_pack_template;' in t[lo:hi]
            else:assert re.search(r'(?m)^int32_t hack_show_deck_force_expansion;',t)
            entry.update(method='explicit_source_scope_or_name_migration',rebuilt=actual)
        elif identity==('cards/unglued_unhinged.c','lc_unaccent_map',()):
            expected=bytes.fromhex(native['bytes']);assert len(expected)==256
            consumers=[]
            for path,name in [('cards/unglued_unhinged.c','num_letters_permanents_names_include'),('cards/unglued_unhinged.c','card_bloodletter'),('cards/unstable.c','card_staff_of_the_letter_magus')]:
                t=source(path);lo,hi=span(t,name);body=t[lo:hi]
                match=re.search(r'static const unsigned char native_name_fold\[256\]\s*=\s*\{([^}]+)\};',strip_comments(body));assert match,name
                literals=[s.strip() for s in match[1].split(',') if s.strip()]
                assert all(re.fullmatch(r'0x[0-9a-fA-F]+|[0-9]+',s) for s in literals)
                actual=bytes(int(s,16) if s.startswith('0x') else int(s) for s in literals);assert actual==expected
                consumers.append(dict(source=path,name=name,source_definition_sha256=sha(body.encode()),initializer_sha256=sha(actual)))
            physical=[r for r in new['globals'] if r['name']=='native_name_fold'];assert physical and all(r['bytes']==native['bytes'] for r in physical)
            entry.update(method='exact_const_table_copied_into_three_reviewed_consumers',source_consumers=consumers,rebuilt_objects=physical,scope='Full 256-byte table equality; source-level copies are recorded explicitly, not treated as one reconstructed global identity.')
        elif identity==('cards/unglued_unhinged.c','txtfx_not_found',()):
            assert native['bytes']=='ffffff7f' and native['size']==4
            section=images[0].pe.get_section_by_rva(int(native['address'],16)-0x2000000);assert section and not section.Characteristics&0x80000000
            t=source('cards/unstable.c');consumers=[]
            for name in ['fx_ineffable_blessing_f','fx_staff_of_the_letter_magus','fx_stamp_of_approval']:
                lo,hi=span(t,name);body=t[lo:hi];assert '0x7fffffff' in body
                consumers.append(dict(source='cards/unstable.c',name=name,source_definition_sha256=sha(body.encode())))
            entry.update(method='readonly_constant_inlined_in_three_reviewed_consumers',value=0x7fffffff,source_consumers=consumers,scope='Constant value coverage; no claim that the old addressable global was recreated.')
        else:pending.append(dict(native=native,reason='missing or ambiguous source identity'));continue
        coverage.append(entry)
    result=dict(status='Integer/enum initializer screen complete; no C/proof mutation.',native_objects=len(old['globals']),rebuilt_objects=len(new['globals']),covered=len(coverage),pending=len(pending),methods=dict(collections.Counter(r['method'] for r in coverage)),coverage=coverage,unresolved=pending,source_sha256=sources,inputs={str(path):sha(path.read_bytes()) for path in [args.native,args.rebuilt,Path(__file__),ROOT/'tools/recovery/global_initializers.py']},scope='Load-time bytes of direct-address project integer/enum scalars and arrays, including the resumed C++ picker. Pointer and aggregate objects, dynamic locations, EXE storage, runtime state and full declaration ABI are excluded. Source identity moves and representation changes are explicit; equal bits alone do not identify a different global object. Seven explicit mappings preserve values but do not assert original source identity reconstruction.')
    for inventory in [old,new]:
        for field in ['debug_path','dll_path']:result['inputs'][inventory[field]]=sha(Path(inventory[field]).read_bytes())
    assert json.loads(json.dumps(result))==result
    args.output.parent.mkdir(parents=True,exist_ok=True);temp=args.output.with_suffix('.pending.json');temp.write_text(json.dumps(result,indent=2)+'\n');temp.replace(args.output)
    print(json.dumps({k:result[k] for k in ['native_objects','rebuilt_objects','covered','pending','methods']},indent=2))


if __name__=='__main__':main()
