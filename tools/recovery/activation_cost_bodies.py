"""Recover cost-wrapper scalar words, reused scratch and VLA call frames."""
import re
from promote_recovery import LITERAL

GRAVE_STORAGE='''typedef union
{
  char text[100];
  target_definition_2022_t target;
} activation_grave_storage_2022_t;'''

TAP_BODY='''int generic_activated_ability_with_tapsubtype_as_cost(
    int player, int card, event_t event, generic_activated_ability_flags_t flags,
    int cless, int black, int blue, int green, int red, int white, int gvc_costs,
    target_definition_t *td, char *prompt, target_definition_t *td_subtype, int req_tap)
{
  if (event == EVENT_CAN_ACTIVATE)
  {
    if (target_available(player, card, td_subtype) < req_tap) return 0;
    return granted_generic_activated_ability(player, card, player, card, event, flags,
        cless, black, blue, green, red, white, gvc_costs, td, prompt);
  }
  if (event == EVENT_ACTIVATE)
  {
    card_instance_t *instance = get_card_instance(player, card);
    int to_tap[req_tap];
    for (int i = 0; i < req_tap; ++i)
    {
      char typetext[100];
      char clr[100];
      char buffer[500];
      const char *subtype_text = "";
      const char *subtype_space = "";
      const char *color_text = "";
      const char *color_space = "";
      if (ai_is_speculating == 1) typetext[0] = '\\0';
      else type_text(typetext, 100, td_subtype->required_type, 0);
      if (td_subtype->required_subtype >= 0)
      {
        subtype_text = raw_get_subtype_text(td_subtype->required_subtype);
        subtype_space = " ";
      }
      if (td_subtype->required_color != 0)
      {
        /* Native code passes the uninitialized clr buffer as the format. */
        color_text = get_sleighted_color_text(player, card, clr, td_subtype->required_color);
        color_space = " ";
      }
      scnprintf(buffer, 500, RECOVERY_TAP_PROMPT, color_text, color_space,
          subtype_text, subtype_space, typetext, i + 1, req_tap);
      /* The release uses target slot 9, independently of the VLA index. */
      if (!new_pick_target(td_subtype, buffer, 9, 0x81)) break;
      to_tap[i] = instance->targets[9].card;
      get_card_instance(player, to_tap[i])->state |= 0x100000;
    }
    for (int i = 0; i < instance->number_of_targets; ++i)
    {
      card_instance_t *target = get_card_instance(player, to_tap[i]);
      if (target->state & 0x100000) target->state &= ~0x100000U;
    }
    int good = spell_fizzled != 1;
    instance->number_of_targets = 0;
    if (good)
    {
      granted_generic_activated_ability(player, card, player, card, EVENT_ACTIVATE, flags,
          cless, black, blue, green, red, white, gvc_costs, td, prompt);
      if (spell_fizzled != 1)
        for (int i = 0; i < req_tap; ++i) tap_card(player, to_tap[i]);
      instance->number_of_targets = 0;
    }
  }
  return 0;
}'''

def replace_call(text,name,new,ordinal=0):
    masked=re.sub(LITERAL,lambda m:' '*len(m[0]),text)
    matches=list(re.finditer(r'\b'+name+r'\s*\(',masked));assert len(matches)>ordinal,(name,ordinal)
    match=matches[ordinal];depth=1
    for end in range(match.end(),len(masked)):
        if masked[end]=='(':depth+=1
        elif masked[end]==')':
            depth-=1
            if not depth:return text[:match.start()]+new+text[end+1:]
    raise AssertionError('Unclosed call '+name)

def sacrifice(text):
    text=text.replace('anon_union_4_2_7165ea48_for_card_instance_struct_26','uint32_t')
    text=text.replace('aVar4.info_slot','aVar4').replace('pcVar3->field_26 = aVar4','pcVar3->info_slot = aVar4')
    text=text.replace('CardFunction','uint32_t').replace('(pcVar3->field_26).info_slot_fn','pcVar3->info_slot')
    old='"Select a %s card to sacrifice.",&test2';assert old in text
    return text.replace(old,'"Select a %s card to sacrifice.",test2.message')

def grave_wrapper(text):
    text=text.replace('char buffer [100];','activation_grave_storage_2022_t scratch;')
    text=text.replace('"Select target %s card.",&test2','"Select target %s card.",test2.message')
    for word in ['uVar1','uVar2','uVar3']:
        pattern=re.escape(word)+r'\._0_2_ = test->ai_selection_mode;\s*'+re.escape(word)+r'\._2_2_ = test->qty;'
        text,count=re.subn(pattern,word+' = (uint32_t)(uint16_t)test->ai_selection_mode | ((uint32_t)(uint16_t)test->qty << 16);',text)
        assert count,word
    text=text.replace('(target_definition_t *)buffer','&scratch.target')
    byte_stores="buffer[0xc] = '\\0';\n      buffer[0xd] = '\\x10';\n      buffer[0xe] = '\\0';\n      buffer[0xf] = '\\0';"
    assert byte_stores in text;text=text.replace(byte_stores,'scratch.target.zone = 0x1000;')
    text=re.sub(r'\bbuffer\b','scratch.text',text)
    return text

def multi_grave(text):
    text=text.replace('sprintf(this_test->message,"Select up to %d cards.");','sprintf(this_test->message,"Select up to %d cards.",max_targets);')
    text=text.replace('sprintf(this_test->message,"Select %d cards.");','sprintf(this_test->message,"Select %d cards.",max_targets);')
    at=text.index('iVar2 = iStack_148;');head,tail=text[:at],text[at:]
    for old,new in [('psVar5','legal_count'),('psStack_14c','minimum_or_other_count')]:head=re.sub(r'\b'+old+r'\b',new,head)
    head=head.replace('select_grave_t *legal_count;','int legal_count;\n  int *psVar5;').replace('select_grave_t *minimum_or_other_count;','int minimum_or_other_count;\n  int *psStack_14c;')
    head=head.replace('(select_grave_t *)','')
    tail=tail.replace('iVar2 = iStack_148;','iVar2 = iStack_148;\n    int face_down_cards[iVar2];',1)
    tail=tail.replace('psStack_14c = asStack_178 + (4 - iStack_148);','psStack_14c = face_down_cards;')
    tail=tail.replace('iStack_148 = CONCAT31(iStack_148._1_3_,bVar4);','iStack_148 = ((uint32_t)iStack_148 & 0xffffff00U) | (uint8_t)bVar4;')
    text=head+tail
    # Keep the control flow and its native argument values. Only lowered
    # outgoing call stores/return-address artifacts are removed below.
    text=replace_call(text,'turn_card_in_grave_face_up','turn_card_in_grave_face_up((int)uVar8 >> 16, uVar8 & 0xffff, psVar5[iVar11 - 1])',0)
    text=replace_call(text,'turn_card_in_grave_face_up','turn_card_in_grave_face_up((int)uVar8 >> 16, uVar8 & 0xffff, *psVar5)',1)
    text=replace_call(text,'new_select_a_card','new_select_a_card(player, tgt_gy.player, iStack_154, sStack_150, sVar9, -1, this_test)')
    text=replace_call(text,'turn_card_in_grave_face_down','turn_card_in_grave_face_down(tgt_gy.player, uVar8)')
    text=replace_call(text,'base_target_definition','base_target_definition(player, card, &td, 0)')
    text=replace_call(text,'load_text_2022','load_text_2022((char *)0x0, "TARGET_GRAVEYARD")')
    text=replace_call(text,'select_target','select_target(player, card - 1000, &td, text_lines[0], &tgt_gy)')
    text=re.sub(r'(?m)^[ \t]*undefined4 uStackY_188;[ \t]*$','',text)
    text=re.sub(r'(?m)^[ \t]*select_grave_t asStack_178 \[4\];[ \t]*$','',text)
    text=re.sub(r'(?m)^[ \t]*(?:uStackY_188|\(&uStackY_188\)\[-iVar2\]) = 0x[0-9a-f]+;[ \t]*$','',text)
    text=re.sub(r'(?m)^[ \t]*asStack_178\[[^]]+\] = [^;]+;[ \t]*$','',text)
    text=re.sub(r'(?m)^[ \t]*\*\([^\n]*?\)\(&stack0x[0-9a-f]+ \+ iVar6\) = [^;]+;[ \t]*$','',text)
    text=text.replace('iVar6 = iStack_148 * -4;','')
    assert not re.search(r'stack0x|asStack_178|uStackY_188|CONCAT31',text),re.findall(r'.*(?:stack0x|asStack_178|uStackY_188|CONCAT31).*',text)
    return text

def reconstruct(address,text,tap_prompt):
    if address=='26a4540':return sacrifice(text)
    if address=='26a4c00':return grave_wrapper(text)
    if address=='2652be0':return multi_grave(text)
    if address=='26a66a0':return TAP_BODY.replace('RECOVERY_TAP_PROMPT',tap_prompt)
    raise AssertionError(address)
