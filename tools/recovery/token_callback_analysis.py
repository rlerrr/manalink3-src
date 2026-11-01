"""Bind Mirror Match's token callback to its actual native helper identity."""
import collections
from array_role_analysis import snapshots
from global_initializers import inherited_type
from inspect_release import inherited_value,value
from object_initializers import unqualify,sizeof,member_offset
from shared_stack_test_analysis import walk,frame_offset
from shared_stack_record_analysis import collect_bytes

SOURCE='cards/commander_2015.c'
NAME='card_mirror_match'
OLD='token.special_code_for_legacy = &exile_at_end_of_combat;'
NEW='token.special_code_for_legacy = &innistrad_exile_at_end_of_combat_legacy;'


def source_changes(text):
    assert text.count(OLD)==1
    return [dict(before=OLD,after=NEW,occurrences=1)],text.replace(OLD,NEW)


def inspect(image,debug):
    snapshot=snapshots(image,debug,[dict(source=SOURCE,name=NAME)],with_effects=False)[SOURCE,NAME]
    dw=debug.dwarf();die=dw.get_DIE_from_refaddr(int(snapshot['die_offset'],16))
    variables=[v for v in walk(die) if v.tag=='DW_TAG_variable' and inherited_value(v,'DW_AT_name')=='token']
    assert len(variables)==1;variable=variables[0];t=unqualify(inherited_type(variable));assert sizeof(t)==76
    member=next(m for m in t.iter_children() if m.tag=='DW_TAG_member' and value(m,'DW_AT_name')=='special_code_for_legacy')
    pointer=unqualify(inherited_type(member));assert pointer.tag=='DW_TAG_pointer_type' and sizeof(pointer)==4 and member_offset(member)==60
    function=unqualify(inherited_type(pointer));assert function.tag=='DW_TAG_subroutine_type'
    result=unqualify(inherited_type(function));args=[unqualify(inherited_type(p)) for p in function.iter_children() if p.tag=='DW_TAG_formal_parameter']
    assert sizeof(result)==4 and value(result,'DW_AT_encoding')==5 and len(args)==3 and all(sizeof(a)==4 for a in args)
    signature=dict(return_bytes=4,return_encoding=5,argument_kinds=[a.tag for a in args],argument_bytes=[sizeof(a) for a in args])
    frame=frame_offset(dw,variable);view=collect_bytes(image,dw,die,frame,76)
    stores=[s for s in view['stores'] if s['offset']==60];assert len(stores)==1 and stores[0]['size']==4 and stores[0]['value'] is not None
    address=stores[0]['value'];names={n for n,v,s,k,c in debug.symbols if k==32 and 0<s<=len(image.pe.sections)
        and image.pe.OPTIONAL_HEADER.ImageBase+image.pe.sections[s-1].VirtualAddress+v==address}
    assert len(names)==1
    other=collections.Counter()
    for s in view['stores']:
        for j in range(s['size']):
            offset=s['offset']+j
            if 0<=offset<76 and not 60<=offset<64:other[offset,None if s['value'] is None else s['value']>>(8*j)&255]+=1
    snapshot.update(callback_signature=signature,token_frame=frame,callback_store=stores[0],callback_symbol=next(iter(names)),
        noncallback_byte_events=[[o,v,c] for (o,v),c in sorted(other.items(),key=lambda x:(x[0][0],str(x[0][1])))])
    return snapshot
