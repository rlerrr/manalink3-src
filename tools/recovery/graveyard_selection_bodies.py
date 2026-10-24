"""Typed native VLA bodies; allocation, call frames and quirks are audited separately."""
import re
from promote_recovery import strip_comments


def selection_callback(raw, zombie=False):
    text=strip_comments(raw)
    if zombie:
        start=text.index('      if (event == EVENT_CAN_ACTIVATE)')
        end=text.index('\n      else {',start)
        name='liliana_reanimation_legacy'
        declarations='int iVar5; int iVar7; int iVar9; int *piVar6;'
        guard='if (!in_play(player, card)) return 0;\n  if (is_humiliated(player, card, AL_STATIC)) return 0;'
        subtype='''if (!has_subtype_by_id_2022(player, cards_data[iid].id, SUBTYPE_ZOMBIE)) {
        grave = graveyard_ptr_mutable[player];
        goto next_card;
      }
      iid = graveyard_ptr_mutable[player][i];'''
        prompt='Select a Zombie card to play.'
        cast='''/* Native call passes the selection index as argument two and literal 1 as argument three. */
      play_card_in_grave_for_free(player, selected, 1);'''
        finish=''
    else:
        start=text.index('  if (event == EVENT_CAN_ACTIVATE)')
        end=text.index('\n  else {',start)
        name='chainers_legacy'
        declarations='int iVar6; int iVar7; int iVar8; int *piVar4;'
        guard=''
        subtype=''
        prompt='Select a creature card to play.'
        cast='play_card_in_grave_for_free(player, player, playable[1][selected]);'
        finish='kill_card(player, card, KILL_EXILE);'
    can=re.sub(r'(?m)^\s*uStackY_\w+ = 0x[0-9a-f]+;\n','\n',text[start:end])
    return f'''int {name}(int player, int card, event_t event)
{{
  {declarations}
  {guard}
{can}
  else if (event == EVENT_ACTIVATE) {{
    int grave_count = count_graveyard(player);
    int playable[2][grave_count];
    int count = 0;
    int i = 0;
    int *grave = graveyard_ptr_mutable[player];
    int iid = *grave;
    test_definition_t this_test;
    while (iid != -1) {{
      {subtype}
      if (!is_what(-1, iid, TYPE_CREATURE)) {{
        grave = graveyard_ptr_mutable[player];
        goto next_card;
      }}
      grave = graveyard_ptr_mutable[player];
      int legal = can_legally_play_iid(player, grave[i]);
      grave = graveyard_ptr_mutable[player];
      if (!legal) goto next_card;
      if (!has_mana_to_cast_iid(player, EVENT_ACTIVATE, grave[i])) {{
        grave = graveyard_ptr_mutable[player];
        goto next_card;
      }}
      int sorcery_ok = can_sorcery_be_played(player, EVENT_ACTIVATE);
      grave = graveyard_ptr_mutable[player];
      iid = grave[i];
      if (sorcery_ok || (cards_data[iid].type & 0x30)) {{
        playable[0][count] = iid;
        playable[1][count] = i;
        ++count;
      }}
next_card:
      iid = grave[i + 1];
      ++i;
    }}
    new_default_test_definition(&this_test, 0xff, "{prompt}");
    int selected = select_card_from_zone(player, player, playable[0], count, 0, 1, -1, &this_test);
    if (selected != -1 && charge_mana_from_id(player, -1, EVENT_ACTIVATE,
                                              cards_data[playable[0][selected]].id)) {{
      {cast}
      cant_be_responded_to = 1;
      {finish}
      return 0;
    }}
    spell_fizzled = 1;
    return 0;
  }}
  else if (event == EVENT_CLEANUP) {{
    kill_card(player, card, KILL_EXILE);
  }}
  return 0;
}}
'''


GENESIS='''int card_genesis_ultimatum(int player, int card, event_t event)
{
  if (event == EVENT_CAST_SPELL) {
    if (card != affected_card || player != affected_card_controller) return 0;
  } else if (event == EVENT_RESOLVE_SPELL) {
    test_definition_t test;
    new_default_test_definition(&test, 0x47, "Select a permanent card to put into play.");
    int to_look = count_deck(player);
    if (to_look != 0) {
      if (to_look > 5) to_look = 5;
      int added[to_look];
      int count = 0;
      int remaining = to_look;
      do {
        test.create_minideck = (unsigned short)remaining;
        int selected = new_select_a_card(player, player, 1, 0, 5, -1, &test);
        if (selected == -1) break;
        added[count] = deck_ptr[player][selected];
        remove_card_from_deck(player, selected);
        ++count;
        --remaining;
      } while (remaining != 0);
      for (int i = 0; i < count; ++i) {
        int card_added = add_card_to_hand(player, added[i]);
        set_special_flags(player, card_added, 0x10000);
        put_into_play(player, card_added);
      }
      for (int i = 0; i < remaining; ++i) {
        add_card_to_hand(player, *deck_ptr[player]);
        remove_card_from_deck(player, 0);
      }
    }
    kill_card(player, card, KILL_DESTROY);
  } else if (event != EVENT_CAN_CAST && event != EVENT_CAN_CHANGE_TARGET &&
             event != EVENT_CHANGE_TARGET) {
    return 0;
  }
  return basic_spell(player, card, event);
}
'''


GYRUS='''int card_gyrus_waker_of_corpses(int player, int card, event_t event)
{
  check_legend_rule(player, card, event);
  if ((event == EVENT_CAST_SPELL || event == EVENT_ENTERING_THE_BATTLEFIELD_AS_CLONE) &&
      card == affected_card && player == affected_card_controller) {
    if (!check_special_flags2(player, card, 0x100) && !played_for_free(player, card) &&
        !is_token(player, card)) {
      int mp = x_value + 3;
      int cost = get_updated_casting_cost(player, card, -1, event, 0);
      ++hack_silent_counters;
      add_counters(player, card, player, card, COUNTER_P1_P1, cost + mp);
      --hack_silent_counters;
    }
  }
  dat_ability(player, card, event, DAT_RESOLVE_TRIGGER_MANDATORY, 0);
  if (event == EVENT_DAT_ABILITY) {
    test_definition_t this_test;
    default_test_definition(&this_test, TYPE_CREATURE);
    int power = get_power(player, card);
    scnprintf(this_test.message, 100, "Select target creatrue card with power less than %d", power);
    this_test.power = get_power(player, card);
    this_test.power_flag = 3;
    if (new_special_count_grave(player, &this_test) && !graveyard_has_shroud(player)) {
      int grave_count = count_graveyard(player);
      int playable[2][grave_count];
      memset(playable, 0xff, sizeof playable);
      int count = 0;
      int scanned = 0;
      int *grave = graveyard_ptr_mutable[player];
      int iid = *grave;
      while (iid != -1) {
        if (is_what(-1, iid, TYPE_CREATURE)) {
          int base_power = cards_data[grave[scanned]].power;
          if (base_power < get_power(player, card)) {
            playable[0][count] = grave[scanned];
            playable[1][count] = scanned;
            ++count;
          }
        }
        ++scanned;
        iid = grave[scanned];
      }
      /* Native gate uses all scanned graveyard entries, including when no candidates remain. */
      if (scanned != 0) {
        int selected = select_card_from_zone(player, player, playable[0], count, 0, 1, -1,
                                             &this_test);
        if (selected == -1) {
          spell_fizzled = 1;
        } else {
          int csvid = cards_data[playable[0][selected]].id;
          rfg_card_from_grave(player, playable[1][selected]);
          token_generation_t token;
          default_token_definition(player, card, csvid, &token);
          token.no_sleight = 1;
          token.legacy = 1;
          token.special_code_for_legacy = rfg_at_end_of_combat;
          token.action = TOKEN_ACTION_ATTACKING;
          generate_token(&token);
        }
      }
    }
  }
  return 0;
}
'''
