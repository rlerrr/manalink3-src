"""Resolve copy_token's excluded helper-inline token without inventing a range."""
import argparse,json,sys
from pathlib import Path
sys.path.insert(0,'tools/recovery')
from array_role_analysis import effects
from canonicalize_helpers import sha,read,span
from global_initializers import inherited_type
from inspect_release import Image,inherited_value,value
from object_initializers import unqualify,sizeof
from shared_stack_test_analysis import walk,frame_offset
from shared_stack_record_analysis import collect_bytes
from verify_inline_helpers import ranges
from verify_wide_stack_test_recovery import symbol_at


def inspect(image,debug,native):
    dw=debug.dwarf();owners=[]
    for cu in dw.iter_CUs():
        if value(cu.get_top_DIE(),'DW_AT_name','').replace('\\','/')!='functions/token_generation.c':continue
        for d in cu.get_top_DIE().iter_children():
            if d.tag!='DW_TAG_subprogram' or inherited_value(d,'DW_AT_name')!='copy_token':continue
            parts=ranges(dw,d)
            if len(parts)==1 and symbol_at(image,debug,parts[0][0])=='_copy_token.part.0':owners.append(d)
    assert len(owners)==1;d=owners[0];lo,hi=ranges(dw,d)[0]
    tokens=[v for v in walk(d) if v.tag=='DW_TAG_variable' and inherited_value(v,'DW_AT_name')=='token']
    assert len(tokens)==1;v=tokens[0];t=unqualify(inherited_type(v));assert sizeof(t)==76
    frame=frame_offset(dw,v);assert frame==-88
    scope=v.get_parent()
    while scope.tag=='DW_TAG_lexical_block':scope=scope.get_parent()
    scope_name=inherited_value(scope,'DW_AT_name')
    if native:
        assert lo==0x2666640 and d.offset==0x61cc49e and scope.tag=='DW_TAG_subprogram'
    else:
        assert scope.tag=='DW_TAG_inlined_subroutine' and scope_name=='copy_token_part'
    scope_ranges=ranges(dw,scope);assert scope_ranges
    view=dict(address=hex(lo),end=hex(hi),bytes_sha256=sha(image.pe.get_data(lo-image.pe.OPTIONAL_HEADER.ImageBase,hi-lo)))
    calls=effects(image,debug,view)['calls'];witnesses=[]
    for name,index in [('_copy_token_definition',2),('_generate_token',0)]:
        matches=[c for c in calls if c['name']==name];assert len(matches)==1
        c=matches[0];assert c['arguments'][index]==['frame',frame+8]
        assert any(a<=int(c['address'],16)<b for a,b in scope_ranges)
        witnesses.append(dict(call=c,token_argument_index=index))
    stores=collect_bytes(image,dw,d,frame,76)['stores']
    stores=[s for s in stores if any(a<=int(s['address'],16)<b for a,b in scope_ranges)]
    assert not stores
    return dict(owner_die=hex(d.offset),owner_symbol='_copy_token.part.0',owner_view=view,
        variable_die=hex(v.offset),variable_type=hex(t.offset),variable_bytes=76,frame_offset=frame,
        scope_tag=scope.tag,scope_name=scope_name,scope_ranges=[[a,b] for a,b in scope_ranges],
        token_api_operands=witnesses,direct_scalar_stores=stores)


parser=argparse.ArgumentParser(description=__doc__)
parser.add_argument('--output',default='docs/recovery-evidence/token-inline-owner-review.json')
args=parser.parse_args()
ni=Image(Path('.recovery/2022-ManalinkEh.dll').read_bytes());nd=Image(Path('G:/Manalink/ManalinkEh_10_22_Hotfix.dbg').read_bytes())
ci=Image(Path('ManalinkEh.dll').read_bytes());cd=Image(Path('ManalinkEh.dbg').read_bytes())
n=inspect(ni,nd,True);c=inspect(ci,cd,False)
source=read('functions/token_generation.c');definitions={}
for name in ['copy_token_part','copy_token']:
    a,b=span(source,name);definitions[name]=sha(source[a:b].encode())
record=dict(native_dll_sha256=sha(ni.data),native_dbg_sha256=sha(nd.data),current_dll_sha256=sha(ci.data),
    current_dbg_sha256=sha(cd.data),tool_sha256=sha(Path(__file__).read_bytes()),native=n,current=c,
    source_definition_sha256=definitions,
    scope="The native private copy_token.part.0 token is implemented in the current copy_token.part.0 concrete range by an actual DW_TAG_inlined_subroutine for source copy_token_part. Its76-byte token, fixed CFA-88 location and initializer/generator pointer operands are verified inside that inline scope. Neither scope has direct scalar token writes. The broad inventory deliberately excluded other-helper inline scopes; this explains its one unbound instance. The public wrapper and private register-argument clone remain distinct ranges/ABIs. This checks declaration, owner and API operands, not copied token contents, transitive helper semantics or gameplay.")
out=Path(args.output);assert not out.exists();out.write_bytes((json.dumps(record,indent=2)+'\n').encode())
print('Resolved native copy_token.part.0 token to the actual rebuilt helper-inline scope; both API operands bind CFA-88.')
