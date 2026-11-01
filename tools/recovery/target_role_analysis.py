"""Frame/operand checks for recovered target, prompt and selection roles."""
from array_role_analysis import snapshots,effects
from inspect_release import value,inherited_value
from global_initializers import inherited_type
from object_initializers import unqualify,sizeof
from shared_stack_test_analysis import walk


def inspect(image,debug,rows):
    result=snapshots(image,debug,rows,with_effects=False)
    for key,row in result.items():row['effects']=effects(image,debug,row['view'],include_frame_copies=True,call_argument_words=24)
    dw=debug.dwarf()
    for row in result.values():
        die=dw.get_DIE_from_refaddr(int(row['die_offset'],16));scalars=[]
        for variable in walk(die):
            if variable.tag!='DW_TAG_variable':continue
            t=unqualify(inherited_type(variable))
            if t is None or t.tag not in ['DW_TAG_base_type','DW_TAG_enumeration_type','DW_TAG_pointer_type']:continue
            scalars.append(dict(name=inherited_value(variable,'DW_AT_name'),kind=t.tag,size=sizeof(t),encoding=value(t,'DW_AT_encoding')))
        row['scalar_types']=scalars
    return result
