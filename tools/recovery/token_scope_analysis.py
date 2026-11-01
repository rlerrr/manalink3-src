"""Inspect separate native token scopes and their complete scalar-byte writes."""
from scoped_array_analysis import inspect as scoped_inspect,lifetime
from shared_stack_test_analysis import walk,frame_offset
from global_initializers import inherited_type
from object_initializers import unqualify,sizeof
from inspect_release import value,inherited_value
from shared_stack_record_analysis import collect_bytes

NAMES={'amass','card_rite_of_belzenlok','card_shark_typhoon','card_the_circle_of_loyalty'}
UNION='''  /* DWARF locals share this frame offset; keep every typed view. */
  union
  {
    test_definition_t test;
    token_generation_t token;
    uint8_t bytes[216];
  } %s;
'''


def source_changes(name,text):
    assert name in NAMES;changes=[]
    def replace(old,new,count=1):
        nonlocal text
        assert text.count(old)==count,(name,old,text.count(old),count)
        changes.append(dict(before=old,after=new,occurrences=count));text=text.replace(old,new)
    if name=='amass':
        replace('  target_definition_t td;\n  token_generation_t token;\n','')
        replace('    if (iVar1 < 2)\n    {','    if (iVar1 < 2)\n    {\n      token_generation_t token;')
        replace('    default_target_definition(player, card, &td, 2);','    {\n    target_definition_t td;\n    default_target_definition(player, card, &td, 2);')
        replace('    }\n  }\n  return;\n}','    }\n    }\n  }\n  return;\n}')
    elif name=='card_rite_of_belzenlok':
        replace('  token_generation_t token;\n','')
        for csvid in ['0x4916','0x491a']:
            replace('    default_token_definition(player, card, '+csvid+', &token);',
                    '    token_generation_t token;\n    default_token_definition(player, card, '+csvid+', &token);')
    elif name=='card_shark_typhoon':
        replace(UNION%'recovered_stack_token','')
        replace('      default_token_definition(player, card, 0x5394,','      token_generation_t token;\n      default_token_definition(player, card, 0x5394,')
        # The six-space occurrence also contains a four-space suffix, so use
        # the preceding CMC call to select the other original scope exactly.
        replace('    iVar2 = get_cmc(trigger_cause_controller, trigger_cause);',
                '    token_generation_t token;\n    iVar2 = get_cmc(trigger_cause_controller, trigger_cause);')
        replace('      default_test_definition(&recovered_stack_token.test, 2);','      test_definition_t test;\n      default_test_definition(&recovered_stack_token.test, 2);')
        replace('recovered_stack_token.test','test',3);replace('recovered_stack_token.token','token',10)
    else:
        replace(UNION%'recovered_stack_test','')
        replace('        default_test_definition(&recovered_stack_test.test, 0xff);','        test_definition_t test;\n        default_test_definition(&recovered_stack_test.test, 0xff);')
        replace('      default_token_definition(player, card, 0x4958,','      token_generation_t token;\n      default_token_definition(player, card, 0x4958,')
        replace('  }\n  default_token_definition(player, card, 0x4958,','  }\n  {\n  token_generation_t token;\n  default_token_definition(player, card, 0x4958,')
        for indent in ['      ','  ']:
            old=''.join(indent+"recovered_stack_test.test.message[0x"+offset+"] = "+val+";\n" for offset,val in [('24',"'\\b'"),('25',"'\\0'"),('26',"'\\0'"),('27',"'\\0'")])
            replace(old,indent+'token.s_key_plus = SP_KEYWORD_VIGILANCE;\n')
        replace('  generate_token(&recovered_stack_test.token);\nLAB_025d55dc:', '  generate_token(&recovered_stack_test.token);\n  }\nLAB_025d55dc:')
        replace('recovered_stack_test.test','test',3);replace('recovered_stack_test.token','token',4)
    return changes,text


def inspect(image,debug,rows):
    result=scoped_inspect(image,debug,rows);dw=debug.dwarf()
    for key,snapshot in result.items():
        die=dw.get_DIE_from_refaddr(int(snapshot['die_offset'],16))
        for token in [r for r in snapshot['typed_records'] if r['bytes']==76]:
            found=[]
            for v in walk(die):
                if v.tag!='DW_TAG_variable' or inherited_value(v,'DW_AT_name')!=token['name'] or value(v,'DW_AT_location') is None:continue
                t=unqualify(inherited_type(v))
                if t.tag=='DW_TAG_structure_type' and sizeof(t)==76 and frame_offset(dw,v)==token['frame_offset']:
                    scope=lifetime(dw,v,die)
                    from verify_inline_helpers import ranges
                    if [[a,b] for a,b in ranges(dw,scope)]==token['scope_ranges']:found.append(v)
            assert len(found)==1
            view=collect_bytes(image,dw,die,token['frame_offset'],76)
            token['scalar_stores']=[s for s in view['stores'] if any(a<=int(s['address'],16)<b for a,b in token['scope_ranges'])]
    return result


def plain_test_record(dw,row):
    assert row['name'] in ['card_shark_typhoon','card_the_circle_of_loyalty'];found=[]
    from verify_inline_helpers import ranges
    from shared_stack_test_analysis import test_layout
    for cu in dw.iter_CUs():
        if value(cu.get_top_DIE(),'DW_AT_name','').replace('\\','/')!=row['source']:continue
        for d in cu.get_top_DIE().iter_children():
            if d.tag!='DW_TAG_subprogram' or inherited_value(d,'DW_AT_name')!=row['name'] or not ranges(dw,d):continue
            variables=[v for v in walk(d) if v.tag=='DW_TAG_variable' and inherited_value(v,'DW_AT_name')=='test']
            assert len(variables)==1;v=variables[0];t=unqualify(inherited_type(v));assert t.tag=='DW_TAG_structure_type' and sizeof(t)==216
            found.append(dict(die=d,frame_offset=frame_offset(dw,v),layout=test_layout(t)))
    assert len(found)==1
    return found[0]
