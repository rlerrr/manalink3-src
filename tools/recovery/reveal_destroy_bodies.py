"""Native reveal VLA and packed destruction-counter reconstructions."""
import re

REVEAL = r'''int reveal_card_until_you_reveal_test_2022(int player, int t_player, test_definition_t *test, int flags)
{
  int *deck = deck_ptr[player];
  if (flags & 0x4000)
    deck = deck_ptr[t_player];
  unsigned int qty = test->qty;
  int wanted, limit;
  if (flags & 0x1000)
  {
    wanted = count_deck(t_player);
    if ((int)(qty & 255) < wanted)
      wanted = qty & 255;
    limit = (qty >> 8) & 255;
  }
  else
  {
    wanted = count_deck(t_player);
    if ((int)qty < wanted)
      wanted = qty;
    limit = count_deck(t_player);
  }
  int target_deck_size = count_deck(t_player);
  int remaining = 0;
  uint32_t packed_result = (uint32_t)deck[0];
  if (deck[0] != -1)
  {
    if (target_deck_size < limit)
      limit = target_deck_size;
    /* Native allocation uses player, independently of the selected deck pointer. */
    int deck_copy[count_deck(player)];
    int recipient = (flags & 0x100) ? t_player : player;
    int removed = 0;
    int found = 0;
    int last_added = -1;
    packed_result = UINT32_MAX;
    int scanned = 0;
    while (deck[scanned] != -1)
    {
      deck_copy[scanned] = deck[scanned];
      if (new_make_test(t_player, deck[scanned], -1, test))
      {
        int iid = deck[scanned];
        if (flags & 0xe0)
        {
          if ((flags & 0xc0) && is_what(-1, iid, 2) && grafdiggers_cages(player, TYPE_CREATURE, 1))
            goto after_move;
          ++removed;
          last_added = add_card_to_hand(recipient, iid);
          remove_card_from_deck(t_player, scanned);
          if (!(flags & 0x100) && player != t_player)
          {
            if (player == 0 && t_player == 1)
              add_state(player, last_added, 0x1000);
            else if (player == 1 && t_player == 0)
              remove_state(player, last_added, 0x1000);
          }
          if (flags & 0x80)
            add_state(recipient, last_added, 0x10);
          if (flags & 0xc0)
          {
            set_special_flags(recipient, last_added, 0x10000);
            put_into_play(recipient, last_added);
          }
        }
after_move:
        /* Native flags are independent, including when the cage blocked movement. */
        if (flags & 0x2000)
        {
          play_card_in_deck_for_free(player, recipient, scanned);
          ++removed;
        }
        if (flags & 0x800)
        {
          add_card_to_rfg(player, iid);
          rfg_card_in_deck(player, scanned);
          ++removed;
        }
        if (flags & 0x200)
        {
          int report = last_added;
          if (last_added < 0)
          {
            report = iid;
            if (((uint32_t)last_added & packed_result) == UINT32_MAX)
              packed_result = 0;
          }
          if (found == 0)
            packed_result |= (uint32_t)(int32_t)(int16_t)report;
          else if (found == 1)
            packed_result |= (uint32_t)report << 16;
        }
        ++found;
      }
      ++scanned;
      if (found == wanted || scanned == limit)
        break;
    }
    show_deck(current_turn, deck_copy, scanned, &UNK_027642a0, 0, str_Done);
    show_deck(current_turn ^ 1, deck_copy, scanned, &UNK_027642a0, 0, str_Done);
    remaining = scanned - removed;
    if (remaining > 0)
    {
      if (flags & 1)
        mill(t_player, remaining);
      if (flags & 2)
        put_top_x_on_bottom(player, t_player, remaining);
      if (flags & 4)
        put_top_x_on_bottom_in_random_order(t_player, remaining);
      if (flags & 8)
        for (int i = 0; i < remaining; ++i)
          rfg_top_card_of_deck(t_player);
      if (flags & 0x10)
        shuffle(t_player);
    }
  }
  if (flags & 0x400)
    packed_result = (uint32_t)remaining;
  return (int32_t)packed_result;
}
'''


def packed_callback(text):
    """Use word masks for native halfword writes; do not alias int objects as short*."""
    changes=[]
    def replace(old,new,kind):
        nonlocal text
        count=text.count(old);assert count,(kind,old)
        changes.append(dict(original=old,replacement=new,occurrences=count,kind=kind))
        text=text.replace(old,new)
    first=re.search(r'psVar1 = \(short \*\)\(\(int\)&instance->field_31 \+ 2\);\s*\*psVar1 = \*psVar1 \+ 1;',text)
    assert first
    replace(first[0],'instance->eot_toughness = (int32_t)((uint32_t)instance->eot_toughness + 0x10000U);','increment high halfword modulo 65536')
    replace('*(short *)&instance->targets[10].player = (short)instance->targets[10].player + 1;',
        'instance->targets[10].player = (int32_t)(((uint32_t)instance->targets[10].player & 0xffff0000U) | (((uint32_t)instance->targets[10].player + 1U) & 0xffffU));','increment low halfword without carry into high halfword')
    second=re.search(r'psVar1 = \(short \*\)\(\(int\)&instance->targets\[10\].player \+ 2\);\s*\*psVar1 = \*psVar1 \+ 1;',text);assert second
    replace(second[0],'instance->targets[10].player = (int32_t)((uint32_t)instance->targets[10].player + 0x10000U);','increment high player-count halfword modulo 65536')
    replace('(char)*(undefined2 *)((int)&instance->field_31 + 2)','(char)((uint32_t)instance->eot_toughness >> 16)','low byte of native high-halfword read')
    replace('*(undefined2 *)((int)&instance->field_31 + 2) = 0;','instance->eot_toughness = (int32_t)((uint32_t)instance->eot_toughness & 0xffffU);','clear high halfword only')
    combined=re.search(r'sVar3 = \*\(short \*\)\(\(int\)&pcVar6->targets\[10\].player \+ 2\);\s*\*\(short \*\)&pcVar6->targets\[10\].player =\s*\(short\)pcVar6->targets\[10\].player \+ \(short\)instance->targets\[10\].player;\s*\*\(short \*\)\(\(int\)&pcVar6->targets\[10\].player \+ 2\) =\s*sVar3 \+ \*\(short \*\)\(\(int\)&instance->targets\[10\].player \+ 2\);',text)
    assert combined
    replace(combined[0],'''pcVar6->targets[10].player = (int32_t)(
      (((uint32_t)pcVar6->targets[10].player + (uint32_t)instance->targets[10].player) & 0xffffU) |
      ((((uint32_t)pcVar6->targets[10].player & 0xffff0000U) + ((uint32_t)instance->targets[10].player & 0xffff0000U)) & 0xffff0000U));''','independent modulo-16-bit player-count sums')
    for old in ['short *psVar1;','short sVar3;']:replace(old,'','remove dead halfword alias temporary')
    return text,changes


def creativity(text):
    """Remove only verified compiler call-frame artifacts and split logical locals."""
    changes=[]
    def replace(old,new,kind):
        nonlocal text
        count=text.count(old);assert count,(kind,old)
        changes.append(dict(original=old,replacement=new,occurrences=count,kind=kind))
        text=text.replace(old,new)
    start=text.index('  if (event == EVENT_RESOLVE_GENERAL_EFFECT)')
    end=text.index('  if (event == EVENT_CAN_CAST)',start)
    replace(text[start:end],'''  if (event == EVENT_RESOLVE_GENERAL_EFFECT) {
    default_test_definition(&reveal_test, 0x42);
    uint32_t counts = (uint32_t)pcVar4->targets[10].player;
    unsigned int first_count = counts & 0xffffU;
    unsigned int second_count = counts >> 16;
    if (first_count != 0) {
      for (unsigned int i = 0; i != first_count; ++i)
        reveal_card_until_you_reveal_test(player, player, &reveal_test, 0x144);
      shuffle(player);
    }
    if ((int)second_count < 1)
      return 0;
    for (unsigned int i = 0; i != second_count; ++i)
      reveal_card_until_you_reveal_test(player, 1-player, &reveal_test, 0x144);
    shuffle(1-player);
    return 0;
  }
''','native packed controller counts and separate reveal-test lifetime')
    start=text.index('        ptStack_ec = (test_definition_t *)&stack0xfffffeec;')
    end=text.index('return iVar5;',start)+len('return iVar5;')
    replace(text[start:end],'''        int target_count = pcVar4->number_of_targets;
        int t_array[target_count];
        int shown = 0;
        while (shown < pcVar4->number_of_targets) {
          int target_card = pcVar4->targets[shown].card;
          int target_player = pcVar4->targets[shown].player;
          ++shown;
          if (target_card == -1)
            t_array[shown-1] = get_internal_card_id_from_csv_id(player == target_player ? 0x615a : 0x6161);
          else
            t_array[shown-1] = get_card_instance(target_player, target_card)->internal_card_id;
        }
        return show_deck(player, t_array, shown, &UNK_02729450, 0, str_Done);''','native target-count VLA and player placeholder IDs')
    replace('ptStack_ec = (test_definition_t *)&td;','','remove generated alias assignment to native target local')
    replace('(target_definition_t *)ptStack_ec','&td','native 84-byte target view')
    replace('target_definition_t td;','target_definition_t td;\n  test_definition_t reveal_test;','separate 216-byte test lifetime at the same native frame offset')
    stores=list(re.finditer(r'(?m)^\s*uStackY_118 = 0x[0-9a-f]+;\s*$',text))
    assert stores
    for m in reversed(stores):replace(m[0],'','remove implicit CALL return-address temporary')
    for old in ['test_definition_t *test;','test_definition_t *ptStack_ec;','undefined4 uStackY_118;','uint uStack_f4;','uint uVar9;','uint uVar10;','card_instance_t *pcVar7;','int iVar8;','int iVar2;']:
        replace(old,'','remove dead generated call-frame local')
    return text,changes
