"""Restore the remaining native prompt, diagnostic and selection arrays."""
NAMES={'lichs_mastery_lifeloss_trigger','reveal_top_cards_of_library_and_choose_fn_who_chooses',
       'select_target_impl','generic_activated_ability_with_target_in_grave',
       'generic_activated_ability_with_sacrifice_as_cost','generic_spell','pick_special_permanent_for_sacrifice'}


def source_changes(name,text):
    assert name in NAMES;changes=[]
    def replace(old,new,count=1):
        nonlocal text
        assert text.count(old)==count,(name,old,text.count(old),count)
        changes.append(dict(before=old,after=new,occurrences=count));text=text.replace(old,new)
    if name=='lichs_mastery_lifeloss_trigger':
        replace('  lichs_mastery_storage_t storage;\n','')
        replace('    if (choice == 1)\n    {','    if (choice == 1)\n    {\n      char buffer[100];')
        replace('      scnprintf(storage.text, 100,\n','      scnprintf(buffer, 100,\n')
        replace('&td, storage.text, &tgts[i]','&td, buffer, &tgts[i]')
        replace('      default_test_definition(&storage.test, 0xff);','      test_definition_t test;\n      default_test_definition(&test, 0xff);')
        replace('scnprintf(storage.text, 100, "Select a card to exile','scnprintf(test.message, 100, "Select a card to exile')
        replace('&storage.test','&test')
    elif name=='reveal_top_cards_of_library_and_choose_fn_who_chooses':
        replace('  library_reveal_text_t text;\n','')
        replace('  if (num_to_pick <= 0)\n  {','  if (num_to_pick <= 0)\n  {\n    char buffer[100];')
        replace('text.display','buffer',2)
        replace('      if (num_to_pick > 1)\n      {','      if (num_to_pick > 1)\n      {\n        char buf[200];')
        # Case-local declarations need their own statement blocks. All four
        # native200-byte diagnostic buffers retain the original formats/lines.
        for marker in ['    case TUTOR_BOTTOM_OF_DECK_RANDOM:\n      sprintf(text.diagnostic,',
                       '    case TUTOR_GET_POSITION:\n      sprintf(text.diagnostic,']:
            lo=text.index(marker);hi=text.index('      break;',lo)+len('      break;')
            old=text[lo:hi];head,body=old.split('\n',1)
            replace(old,head+'\n    {\n      char buf[200];\n'+body+'\n    }')
        replace('  sprintf(text.diagnostic, "%s:%d\\012illegal destination_chosen: %d with num_chosen=%d",',
                '  {\n  char buf[200];\n  sprintf(text.diagnostic, "%s:%d\\012illegal destination_chosen: %d with num_chosen=%d",')
        replace('text.diagnostic','buf',8)
        replace('  backtrace_and_abort(buf);\n  return num_chosen;\n}', '  backtrace_and_abort(buf);\n  return num_chosen;\n  }\n}')
    elif name=='select_target_impl':
        replace('  target_t tmp_tgt;\n  char illegal_tgt[200];\n  target_selection_storage_t selection;\n','')
        replace('  {\n    if (zone == 0)','  {\n    target_t tmp_tgt;\n    if (zone == 0)')
        replace('    iVar5 = validate_target_impl(\n','    char err_reason[208];\n    iVar5 = validate_target_impl(\n')
        replace('selection.error_reason','err_reason',3)
        replace('          sprintf(selection.multiblocker_error,','          char illegal_tgt[200];\n          sprintf(selection.multiblocker_error,')
        replace('selection.multiblocker_error','illegal_tgt',2)
        replace('\n        sprintf(illegal_tgt,','\n        char illegal_tgt[200];\n        sprintf(illegal_tgt,')
        replace('  if (spell_fizzled != 1)\n  {','  if (spell_fizzled != 1)\n  {\n    target_t candidates[60];')
        replace('selection.candidates','candidates',11)
    elif name in ['generic_activated_ability_with_target_in_grave','generic_activated_ability_with_sacrifice_as_cost']:
        # These test pointers are consumed after the type-selection block.
        # Keep the two existing enclosing-scope records; only replace the
        # unrelated character-array uses of their100-byte message members.
        replace('    else\n    {\n      if (ai_is_speculating == 1)','    else\n    {\n      char buffer[100];\n      char typetext[100];\n      if (ai_is_speculating == 1)')
        replace('test2.message','typetext',3)
        if name=='generic_activated_ability_with_target_in_grave':
            replace('  activation_grave_storage_t scratch;\n','')
            replace('scratch.text','buffer',2)
            replace('    LAB_026a50a4:\n','      target_definition_t td2;\n    LAB_026a50a4:\n')
            replace('scratch.target','td2',3)
        else:
            replace('scnprintf(marked_for_sacrifice, 100,','scnprintf(buffer, 100,')
            replace('new_default_test_definition(test, type, marked_for_sacrifice);','new_default_test_definition(test, type, buffer);')
    elif name=='generic_spell':
        replace('  generic_spell_scratch_t scratch;\n','')
        replace('      new_default_test_definition(&scratch.test,','      test_definition_t test;\n      new_default_test_definition(&scratch.test,')
        replace('scratch.test','test',2)
        replace('            sprintf(scratch.error,','            char illegal_tgt[200];\n            sprintf(scratch.error,',2)
        replace('scratch.error','illegal_tgt',4)
        replace('          default_target_definition(player, card, &scratch.target, 0);','          target_definition_t td9;\n          default_target_definition(player, card, &scratch.target, 0);')
        replace('scratch.target','td9',5)
    else:
        replace('  sacrifice_prompt_storage_t storage;\n','')
        replace('      new_default_test_definition(&storage.test,','      test_definition_t test;\n      new_default_test_definition(&storage.test,')
        replace('storage.test','test',11)
        replace('    get_sacrifice_prompt_text(storage.prompt,','    char prompt[200];\n    get_sacrifice_prompt_text(storage.prompt,')
        replace('storage.prompt','prompt',2)
    return changes,text


TYPEDEFS=[
 ('cards/dominaria.c','''typedef union
{
  char text[100];
  test_definition_t test;
} lichs_mastery_storage_t;

'''),
 ('functions/tutors.c','''typedef union
{
  char display[100];
  char diagnostic[200];
} library_reveal_text_t;

'''),
 ('functions/targets.c','''typedef union
{
  char error_reason[208];
  char multiblocker_error[200];
  target_t candidates[60];
} target_selection_storage_t;

'''),
 ('functions/functions.c','''typedef union
{
  test_definition_t test;
  target_definition_t target;
  char error[200];
} generic_spell_scratch_t;

'''),
 ('functions/functions.c','_Static_assert(sizeof(generic_spell_scratch_t) == 216, "generic spell shared scratch");\n'),
 ('functions/functions.c','''typedef union
{
  char text[100];
  target_definition_t target;
} activation_grave_storage_t;

'''),
 ('functions/sacrifices.c','''typedef union
{
  char prompt[200];
  test_definition_t test;
} sacrifice_prompt_storage_t;

'''),
 ('functions/sacrifices.c','_Static_assert(sizeof(sacrifice_prompt_storage_t) == 216, "sacrifice shared prompt/test storage");\n')]
