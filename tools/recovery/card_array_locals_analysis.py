"""Restore and inspect native card-local arrays and their disjoint scopes."""
import collections
from array_role_analysis import shape
from array_census_analysis import symbol_map,call_targets
from canonicalize_helpers import read,span
from global_initializers import inherited_type
from inspect_release import value,inherited_value
from object_initializers import unqualify,sizeof,member_offset
from scoped_array_analysis import inspect as scoped_inspect
from shared_stack_record_analysis import layout
from shared_stack_test_analysis import walk,frame_offset,test_layout
from cast_stack_storage_analysis import lifetime,scoped_view
from verify_wide_stack_test_recovery import normalized_events
from verify_inline_helpers import ranges
from verify_wide_stack_test_recovery import decode

NAMES={'card_mana_cache','card_gusthas_scepter','card_slaughter_the_strong','card_vraska_relic_seeker',
       'card_fast_furious','card_kayas_guile','card_merrow_bonegnawer','card_reign_of_terror','card_clone_shell','card_lord_windgrace'}
TEST_NAMES={n:'this_test' for n in ['card_mana_cache','card_gusthas_scepter','card_slaughter_the_strong','card_fast_furious','card_kayas_guile','card_reign_of_terror']}
TOKEN_NAMES={'card_vraska_relic_seeker','card_lord_windgrace'}


def source_changes(name,text):
    assert name in NAMES;changes=[]
    def replace(old,new,count=1):
        nonlocal text
        assert text.count(old)==count,(name,old,text.count(old),count)
        changes.append(dict(before=old,after=new,occurrences=count));text=text.replace(old,new)
    def rename(old,new):replace(old,new,text.count(old))
    def remove_union(members):
        replace('  union\n  {\n'+''.join('    '+m+';\n' for m in members)+'  } storage;\n','')
    if name=='card_mana_cache':
        remove_union(['int found[2]','test_definition_t this_test'])
        replace('      storage.found[0] = -1;','      int found[2];\n      storage.found[0] = -1;')
        rename('storage.found','found')
        replace('  LAB_0209aaac:\n','  LAB_0209aaac:\n    {\n    test_definition_t this_test;')
        replace('    add_counters(player, card, player, card, COUNTER_CHARGE, iVar1);',
                '    add_counters(player, card, player, card, COUNTER_CHARGE, iVar1);\n    }')
        rename('storage.this_test','this_test')
    elif name=='card_gusthas_scepter':
        replace('  int leg_1;\n  int idx_1;\n','')
        old='''  /* DWARF locals share this frame offset; keep every typed view. */
  union
  {
    char buffer[100];
    int value;
    uint8_t bytes[100];
  } recovered_stack_leg;

  /* DWARF locals share this frame offset; keep every typed view. */
  union
  {
    int value;
    test_definition_t test;
    uint8_t bytes[216];
  } recovered_stack_idx;
'''
        replace(old,'')
        replace('  if (event == EVENT_CARDCONTROLLED)\n  {','  if (event == EVENT_CARDCONTROLLED)\n  {\n    int leg;\n    int idx;')
        replace('    if (0 < hand_count[player])\n    {','    if (0 < hand_count[player])\n    {\n      int leg;\n      int idx;\n      char buffer[100];\n      test_definition_t this_test;')
        for a,b in [('recovered_stack_leg.value','leg'),('recovered_stack_idx.value','idx'),('recovered_stack_leg.buffer','buffer'),('recovered_stack_idx.test','this_test'),('leg_1','leg'),('idx_1','idx')]:rename(a,b)
    elif name=='card_slaughter_the_strong':
        remove_union(['char msg[100]','test_definition_t this_test'])
        replace('          default_target_definition(player, card, &td, 2);','          char msg[100];\n          default_target_definition(player, card, &td, 2);')
        replace('          default_test_definition(&storage.this_test, 2);','          test_definition_t this_test;\n          default_test_definition(&storage.this_test, 2);')
        rename('storage.msg','msg');rename('storage.this_test','this_test')
    elif name=='card_vraska_relic_seeker':
        remove_union(['char buffer[50]','token_generation_t token'])
        replace('        scnprintf(storage.buffer, 0x32,','        char buffer[50];\n        scnprintf(storage.buffer, 0x32,')
        replace('      default_token_definition(player, card, 0x4972,','      token_generation_t token;\n      default_token_definition(player, card, 0x4972,')
        replace('        default_token_definition(player, card, 0x4998,','        token_generation_t token;\n        default_token_definition(player, card, 0x4998,')
        rename('storage.buffer','buffer');rename('storage.token','token')
    elif name=='card_fast_furious':
        remove_union(['int totals[2]','test_definition_t this_test'])
        replace('      storage.totals[0] = 0;','      int totals[2];\n      storage.totals[0] = 0;')
        replace('          default_test_definition(&storage.this_test, 2);','          test_definition_t this_test;\n          default_test_definition(&storage.this_test, 2);')
        rename('storage.totals','totals');rename('storage.this_test','this_test')
    elif name=='card_kayas_guile':
        remove_union(['int priority[6]','test_definition_t this_test','token_generation_t token'])
        replace('      if ((ai_is_speculating == 1)', '      int priority[6];\n      if ((ai_is_speculating == 1)')
        replace('    default_token_definition(player, card, 0x498d,','    token_generation_t token;\n    default_token_definition(player, card, 0x498d,')
        replace('      new_default_test_definition(&storage.this_test, 2,','      test_definition_t this_test;\n      new_default_test_definition(&storage.this_test, 2,')
        rename('storage.priority','priority');rename('storage.this_test','this_test');rename('storage.token','token')
        replace('    default_token_definition(player, card, 0x498d, &token);',
                '    default_token_definition(player, card, 0x498d, &token);\n    token.key_plus = KEYWORD_FLYING;\n    token.color_forced = COLOR_TEST_BLACK | COLOR_TEST_WHITE;')
    elif name=='card_merrow_bonegnawer':
        replace('\t    new_default_test_definition(&test, TYPE_ANY, "Select a card to exile.");',
                '\t    char msg[100] = "Select a card to exile.";\n\t    new_default_test_definition(&test, TYPE_ANY, msg);')
        # Only the resolution scope uses this original native local name.
        replace('\t    test_definition_t test;\n\t    char msg[100]', '\t    test_definition_t this_test;\n\t    char msg[100]')
        replace('new_default_test_definition(&test, TYPE_ANY, msg);','new_default_test_definition(&this_test, TYPE_ANY, msg);')
        replace('AI_MIN_VALUE, &test);','AI_MIN_VALUE, &this_test);')
    elif name=='card_reign_of_terror':
        remove_union(['char buffer[100]','test_definition_t this_test'])
        replace('      instance = get_card_instance(player, card);','      char buffer[100];\n      instance = get_card_instance(player, card);')
        replace('  default_test_definition(&storage.this_test, 2);','  {\n  test_definition_t this_test;\n  default_test_definition(&storage.this_test, 2);')
        replace('  kill_card(player, card, KILL_DESTROY);\nLAB_0239062e:', '  kill_card(player, card, KILL_DESTROY);\n  }\nLAB_0239062e:')
        rename('storage.buffer','buffer');rename('storage.this_test','this_test')
    elif name=='card_clone_shell':
        replace('\t        test_definition_t test;', '\t        char msg[100] = "Select a card.";\n\t        test_definition_t this_test;')
        replace('new_default_test_definition(&test, TYPE_ANY, "Select a card.");','new_default_test_definition(&this_test, TYPE_ANY, msg);',2)
        replace('new_default_test_definition(&test, TYPE_CREATURE, "Select a card.");','new_default_test_definition(&this_test, TYPE_CREATURE, msg);')
        rename('test.create_minideck','this_test.create_minideck');rename('test.no_shuffle','this_test.no_shuffle');replace('&test);','&this_test);',3)
        replace('int iid = get_card_instance(player, legacy)->targets[0].card;',
                'int stored_iid[1] = {get_card_instance(player, legacy)->targets[0].card};',2)
        replace('&iid,','stored_iid,',2);replace('cards_data[iid]','cards_data[stored_iid[0]]',2)
        replace('is_what(-1, iid,','is_what(-1, stored_iid[0],');replace('add_card_to_hand(player, iid)','add_card_to_hand(player, stored_iid[0])')
    else:
        assert name=='card_lord_windgrace';remove_union(['int dead[10]','token_generation_t token'])
        replace('          if (iVar3 == 0)\n          {','          if (iVar3 == 0)\n          {\n            int dead[10];')
        replace('        default_token_definition(player, card, 0x4910,','        token_generation_t token;\n        default_token_definition(player, card, 0x4910,')
        rename('storage.dead','dead');rename('storage.token','token')
    return changes,text


def inspect(image,debug,rows):
    result=scoped_inspect(image,debug,rows);dw=debug.dwarf();symbols=symbol_map(image,debug)
    for key,snapshot in result.items():
        die=dw.get_DIE_from_refaddr(int(snapshot['die_offset'],16));decls=[];scalars=[]
        for v in walk(die):
            if v.tag!='DW_TAG_variable':continue
            t=unqualify(inherited_type(v))
            if t is None:continue
            if t.tag=='DW_TAG_array_type':
                try:
                    for b in t.iter_children():
                        if b.tag=='DW_TAG_subrange_type':assert not b.attributes['DW_AT_upper_bound'].form.startswith('DW_FORM_ref')
                    s=shape(t)
                except (AssertionError,KeyError,TypeError):continue
                try:offset=frame_offset(dw,v)
                except (AssertionError,ValueError,KeyError):offset=None
                decls.append(dict(name=inherited_value(v,'DW_AT_name'),shape=s,frame_offset=offset))
            if t.tag=='DW_TAG_base_type' and sizeof(t)==4:
                try:offset=frame_offset(dw,v)
                except (AssertionError,ValueError,KeyError):continue
                scalars.append(dict(name=inherited_value(v,'DW_AT_name'),frame_offset=offset,size=4,encoding=value(t,'DW_AT_encoding')))
        snapshot['array_declarations']=decls;snapshot['scalar_frame_locals']=scalars
        md,ins=decode(snapshot['view'],image);targets=call_targets(md,ins);extra={'_exiledby_find_any','_exiledby_remember','_show_deck','_show_deck_exe','_sort_ArrayConstCharPtr'}
        for c in snapshot['effects']['calls']:
            names=symbols.get(targets[c['address']],set())&extra
            if names:assert len(names)==1;snapshot['storage_calls'].append(dict(c,name=next(iter(names))))
        for token in [r for r in snapshot['typed_records'] if r['bytes']==76]:
            calls=[c for c in snapshot['storage_calls'] if c['name']=='_default_token_definition'
                and any(lo<=int(c['address'],16)<hi for lo,hi in token['scope_ranges'])]
            assert len(calls)==1,(key,token,calls)
            pointer=calls[0]['arguments'][3]
            assert pointer==['frame',token['frame_offset']+8] or key[1]=='card_lord_windgrace' and pointer is None
            token['initializer_pointer']=pointer
            token['initializer_csvid']=calls[0]['arguments'][2]
        for record in [r for r in snapshot['typed_records'] if r['bytes']==216]:
            variables=[v for v in walk(die) if v.tag=='DW_TAG_variable' and inherited_value(v,'DW_AT_name')==record['name']
                and value(v,'DW_AT_location') is not None and frame_offset(dw,v)==record['frame_offset']
                and [[a,b] for a,b in ranges(dw,lifetime(dw,v,die))]==record['scope_ranges']]
            assert len(variables)==1
            view=scoped_view(image,dw,die,lifetime(dw,variables[0],die),record['frame_offset'],record['layout'])
            events=normalized_events(view,image,debug)
            record['normalized_parameters']=[dict(field=f,byte=b,value=v,count=c) for (f,b,v),c in sorted(events.items(),key=lambda kv:(kv[0][0],kv[0][1],str(kv[0][2])))]
    return result


def plain_record(dw,row,*,token=False):
    """Bind current actual locals when replaying immutable old union audits."""
    name=row['name'];assert name in (TOKEN_NAMES if token else TEST_NAMES)
    local='token' if token else TEST_NAMES[name];found=[]
    for cu in dw.iter_CUs():
        if value(cu.get_top_DIE(),'DW_AT_name','').replace('\\','/')!=row['source']:continue
        for die in cu.get_top_DIE().iter_children():
            if die.tag!='DW_TAG_subprogram' or inherited_value(die,'DW_AT_name')!=name or not ranges(dw,die):continue
            variables=[v for v in walk(die) if v.tag=='DW_TAG_variable' and inherited_value(v,'DW_AT_name')==local]
            assert len(variables)==(2 if name=='card_vraska_relic_seeker' else 1)
            frames={frame_offset(dw,v) for v in variables};assert len(frames)==1
            types=[unqualify(inherited_type(v)) for v in variables];assert all(t.tag=='DW_TAG_structure_type' and sizeof(t)==(76 if token else 216) for t in types)
            item=dict(die=die,frame_offset=next(iter(frames)))
            if token:item.update(size=76,members={'token':layout(types[0])})
            else:item['layout']=test_layout(types[0])
            found.append(item)
    assert len(found)==1
    return found[0]
