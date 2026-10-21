"""Restore integer attacker storage and the native Support save/restore copies."""
import re

SUPPORT_BODY = '''void support(int player, int card, int max_targets)
{
  target_definition_t td;
  char prompt[100];
  target_t saved_targets[152];
  default_target_definition(player, card, &td, 2);
  td.preferred_controller = player;
  td.special = 0x4000;
  td.allow_cancel = 2;
  if (can_target(&td))
  {
    card_instance_t *instance = get_card_instance(player, card);
    uint8_t old_num_targets = instance->number_of_targets;
    /* Native DWARF declares 152 pairs, but both copies transfer 152 bytes. */
    memcpy(saved_targets, instance->targets, 152);
    instance->number_of_targets = 0;
    for (int i = 0; i < max_targets; ++i)
    {
      scnprintf(prompt, 100, "Select another target creature. (%d of %d)", i + 1, max_targets);
      if (new_pick_target(&td, prompt, i, 0x80))
        state_untargettable(instance->targets[i].player, instance->targets[i].card, 1);
    }
    if (max_targets > 0)
      for (int i = 0; i < instance->number_of_targets; ++i)
      {
        state_untargettable(instance->targets[i].player, instance->targets[i].card, 0);
        add_counters(player, card, instance->targets[i].player, instance->targets[i].card,
            COUNTER_P1_P1, 1);
      }
    instance->number_of_targets = old_num_targets;
    memcpy(instance->targets, saved_targets, 152);
  }
}'''


def reconstruct(address, text):
    if address == '225a8c0':
        return SUPPORT_BODY
    if address not in {'268d340', '268d6a0'}:
        return text
    text = text.replace('anon_union_4_2_7165ea48_for_card_instance_struct_26 *paVar7;',
                        'int32_t *storage;')
    for index in [1, 3]:
        text = text.replace('(anon_union_4_2_7165ea48_for_card_instance_struct_26 *)(pcVar4->targets + '+str(index)+')',
                            '&pcVar4->targets['+str(index)+'].player')
    text = text.replace('&pcVar4->field_26', '&pcVar4->info_slot')
    text = text.replace('(pcVar4->field_26).info_slot', 'pcVar4->info_slot')
    text = text.replace('paVar7->info_slot_fn', '(*storage)').replace('paVar7->info_slot', '(*storage)')
    if address == '268d340':
        old = '''p_Var6[(int)&pcVar4->targets[2].player] =
                   (_func_int_int_int_event_t)(undefined1)attacker_card;'''
        assert old in text
        text = text.replace(old, '((unsigned char *)&pcVar4->targets[2].player)[count] = (unsigned char)attacker_card;')
        text = re.sub(r'\bp_Var6\b', 'count', text).replace('CardFunction count;', 'int32_t count;')
    else:
        text = text.replace('*(byte *)((int)&pcVar4->targets[2].player + iVar3)',
                            '((unsigned char *)&pcVar4->targets[2].player)[iVar3]')
    text = re.sub(r'\bpaVar7\b', 'storage', text)
    assert 'anon_union' not in text and 'info_slot_fn' not in text and 'CardFunction' not in text
    return text
