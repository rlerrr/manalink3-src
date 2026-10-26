#include "manalink.h"

/* Canonical recovered helper declarations. */
int new_damage_all(int player, int card, int targ_player, int dmg, int mode,
                        test_definition_t *this_test);
static actions_t cook_action(int player, int card, actions_t raw_action,
                                  counter_t *counter_type, int *counter_num);
int action_on_card(int player, int card, int t_player, int t_card, actions_t raw_action);
int new_manipulate_all(int player, int card, int t_player, test_definition_t *this_test,
                            actions_t raw_action);
int manipulate_all(int player, int card, int t_player, int type, int flag1, int subtype,
                        int flag2, int color, int flag3, int id, int flag4, int cc, int flag5,
                        actions_t action);
static int action_on_card_impl(int player, int card, int p, int c, actions_t action,
                                    counter_t counter_type, int counter_num);







int action_on_target(int player, int card, unsigned int target_number, actions_t raw_action)
{
  card_instance_t* instance = get_card_instance(player, card);
  ASSERT((instance->targets[target_number].player & ~1) == 0);	// i.e., it's either 0 or 1
  return action_on_card(player, card, instance->targets[target_number].player, instance->targets[target_number].card, raw_action);
}





int manipulate_type(int player, int card, int t_player, type_t type, actions_t action)
{
  test_definition_t this_test;
  new_default_test_definition(&this_test, type, "");
  return new_manipulate_all(player, card, t_player, &this_test, action);
}

int manipulate_auras_enchanting_target(int player, int card, int t_player, int t_card,
                                      test_definition_t *this_test, actions_t raw_action)
{
  int result = 0;
  for (int p = 0; p < 2; ++p)
    for (int c = active_cards_count[p] - 1; c >= 0; --c)
    {
      counter_t counter_type = COUNTER_invalid;
      int counter_num = 0;
      actions_t action = cook_action(p, c, raw_action, &counter_type, &counter_num);
      if (action == (actions_t)-1)
        return result;
      if (action == (actions_t)-2)
        continue;
      card_instance_t *aura = in_play(p, c);
      if (aura && aura->damage_target_player == t_player && aura->damage_target_card == t_card
          && is_what(p, c, TYPE_ENCHANTMENT) && has_subtype(p, c, SUBTYPE_AURA)
          && (!this_test || (new_make_test_in_play(p, c, -1, this_test)
              && (!this_test->not_me || player != p || card != c))))
        result += action_on_card_impl(player, card, p, c, action, counter_type, counter_num);
    }
  return result;
}

int new_damage_all(int player, int card, int targ_player, int dmg, int mode,
                        test_definition_t *this_test)
{
  int player_00;
  int iVar1;
  card_instance_t *instance;
  int iVar3;
  int iStack_1c;
  int iStack_18;
  int iStack_14;

  if (dmg < 1)
  {
    return 0;
  }
  iStack_18 = 0;
  if (this_test != NULL)
  {
    iStack_18 = new_get_test_score(this_test);
  }
  iStack_14 = 0;
  iStack_1c = 0;
  player_00 = current_turn;
LAB_0266787a:
  if ((targ_player != player_00) && (iVar1 = 1, targ_player != 2))
    goto LAB_02667870;
  do
  {
    iVar1 = active_cards_count[player_00] + -1;
    if (-1 < iVar1)
    {
    LAB_026678e2:
      do
      {
        instance = in_play(player_00, iVar1);
        if (instance != NULL)
        {
          if (this_test == NULL)
          {
            iVar3 = is_what(player_00, iVar1, 2);
            if (((iVar3 != 0) && ((mode & 1U) != 0)) ||
                ((iVar3 = is_what(player_00, iVar1, 0x100), iVar3 != 0 && ((mode & 0x20U) != 0))))
              goto LAB_02667915;
          }
          else
          {
            iVar3 = new_make_test_in_play(player_00, iVar1, iStack_18, this_test);
            if (iVar3 == 0)
            {
              is_what(player_00, iVar1, 2);
              is_what(player_00, iVar1, 0x100);
            }
            else
            {
            LAB_02667915:
              if (((card != iVar1) || (player != player_00)) ||
                  (((mode & 4U) == 0 && ((this_test == NULL || (this_test->not_me != 1))))))
              {
                iVar3 = damage_creature(player_00, iVar1, dmg, player, card);
                if (-1 < iVar3)
                {
                  instance = get_card_instance(player, iVar3);
                  if ((mode & 0x10U) != 0)
                  {
                    instance->targets[3].card = instance->targets[3].card | 0x20;
                  }
                  if ((mode & 8U) != 0)
                  {
                    instance->targets[3].card = instance->targets[3].card | 0x80;
                  }
                }
                iStack_1c = iStack_1c + dmg;
                iVar1 = iVar1 + -1;
                if (iVar1 == -1)
                  break;
                goto LAB_026678e2;
              }
            }
          }
        }
        iVar1 = iVar1 + -1;
      } while (iVar1 != -1);
    }
    if ((mode & 2U) != 0)
    {
      damage_player(player_00, dmg, player, card);
    }
    do
    {
      player_00 = current_turn;
      iVar1 = iStack_14 + 1;
      current_turn = player_00;
      if (1 < iVar1)
      {
        return iStack_1c;
      }
    LAB_02667870:
      iStack_14 = iVar1;
      if (iVar1 == 0)
        goto LAB_0266787a;
      player_00 = 1 - player_00;
    } while ((player_00 != targ_player) && (targ_player != 2));
  } while (true);
}

void damage_all(int p, int c, int controller, int amount_damage, int all, int player_too, int keyword, int flag1, int subtype, int flag2, int color, int flag3, int id, int flag4, int cc, int flag5){
	test_definition_t this_test;
	default_test_definition(&this_test, TYPE_CREATURE);
	this_test.subtype = subtype;
	this_test.subtype_flag = flag2;
	this_test.color = color;
	this_test.color_flag = flag3;
	this_test.id = id;
	this_test.id_flag = flag4;
	this_test.cmc = cc;
	this_test.cmc_flag = flag5;
	this_test.keyword = keyword;
	this_test.keyword_flag = flag1;
	new_damage_all(p, c, controller, amount_damage, (1*all)+(2*player_too), &this_test);
}


#ifndef SHANDALAR
/* Recovered October 2022 action and copy helpers. */

/* ManalinkEh 2022: 0x2666bc0; original functions/manipulate_and_damage_all.c:3. */
static int action_on_card_impl(int player, int card, int p, int c, actions_t action,
                                    counter_t counter_type, int counter_num)
{
  int result;
  if (ACT_DETAIN < action)
  {
    if (action == ACT_REMOVE_COUNTERS_BASE)
    {
      result = count_counters(p, c, counter_type);
      if (counter_num < result)
      {
        result = counter_num;
      }
      remove_counters(player, card, p, c, counter_type, result);
      return result;
    }
    if (action != ACT_PARAMETERIZED_ACTIONS_MASK)
    {
      if (action == ACT_ADD_COUNTERS_BASE)
      {
        add_counters_predoubled(player, card, p, c, counter_type, counter_num);
        return counter_num;
      }
      return 0;
    }
    abort();
    return -1;
  }
  switch (action)
  {
  case ACT_KILL_BURY:
  case ACT_KILL_DESTROY:
    result = check_for_special_ability(p, c, 0x800);
    if (result != 0)
    {
      return 0;
    }
  case ACT_KILL_SACRIFICE:
  case ACT_KILL_REMOVE:
  case ACT_KILL_STATE_BASED_ACTION:
    kill_card(p, c, action);
    break;
  case ACT_BOUNCE:
    bounce_permanent(p, c);
    break;
  case ACT_TAP:
    result = is_tapped(p, c);
    if (result != 0)
    {
      return 0;
    }
    tap_card(p, c);
    break;
  case ACT_UNTAP:
    result = is_tapped(p, c);
    if (result == 0)
    {
      return 0;
    }
    untap_card(p, c);
    break;
  default:
    return 0;
  case ACT_RFG_UNTIL_EOT:
    remove_until_eot(player, card, p, c);
    break;
  case ACT_PUT_ON_TOP:
    put_on_top_of_deck(p, c);
    break;
  case ACT_PUT_ON_BOTTOM:
    put_on_bottom_of_deck(p, c);
    break;
  case ACT_DISABLE_NONMANA_ACTIVATED_ABILITIES:
    result = 1;
    goto code_r0x02666db9;
  case ACT_ENABLE_NONMANA_ACTIVATED_ABILITIES:
    result = 0;
  code_r0x02666db9:
    disable_nonmana_activated_abilities(p, c, result);
    break;
  case ACT_HUMILIATE:
    result = 2;
    goto code_r0x02666d86;
  case ACT_DE_HUMILIATE:
    result = 0;
  code_r0x02666d86:
    humiliate(player, card, p, c, result);
    break;
  case ACT_MAKE_UNTARGETTABLE:
    result = 1;
    goto code_r0x02666d5b;
  case ACT_REMOVE_UNTARGETTABLE:
    result = 0;
  code_r0x02666d5b:
    state_untargettable(p, c, result);
    break;
  case ACT_DISABLE_ALL_ACTIVATED_ABILITIES:
    result = 1;
    goto code_r0x02666d30;
  case ACT_ENABLE_ALL_ACTIVATED_ABILITIES:
    result = 0;
  code_r0x02666d30:
    disable_all_activated_abilities(p, c, result);
    break;
  case ACT_GET_COUNT:
    break;
  case ACT_GAIN_CONTROL:
    gain_control_permanently(player, p, c);
    break;
  case ACT_DOES_NOT_UNTAP:
    does_not_untap_effect(player, card, p, c, 0, 1);
    break;
  case ACT_PHASE_OUT:
    phase_out(p, c);
    break;
  case ACT_OF_TREASON:
    effect_act_of_treason(player, card, p, c);
    break;
  case ACT_DETAIN:
    detain(player, card, p, c);
  }
  return 1;
}


/* ManalinkEh 2022: 0x2666f10; original functions/manipulate_and_damage_all.c:127. */
static actions_t cook_action(int player, int card, actions_t raw_action,
                                  counter_t *counter_type, int *counter_num)
{
  short sVar1;
  int result;
  actions_t aVar3;
  aVar3 = raw_action & ACT_PARAMETERIZED_ACTIONS_MASK;
  sVar1 = (short)raw_action;
  if (aVar3 == ACT_ADD_COUNTERS_BASE)
  {
    *counter_type = raw_action >> 0x10 & COUNTER_invalid;
    result = (int)sVar1;
    *counter_num = result;
    if (0 < result)
    {
      result = get_updated_counters_number(player, card, *counter_type, result);
      *counter_num = result;
      if (0 < result)
      {
        return ACT_ADD_COUNTERS_BASE;
      }
      return ~ACT_KILL_BURY;
    }
  }
  else
  {
    if (aVar3 != ACT_REMOVE_COUNTERS_BASE)
    {
      if (aVar3 == 0)
      {
        return raw_action;
      }
      backtrace_and_abort(
          "functions/manipulate_and_damage_all.c:155\n!\"Unknown parameterized action\"");
    }
    *counter_type = raw_action >> 0x10 & COUNTER_invalid;
    *counter_num = (int)sVar1;
    if (0 < sVar1)
    {
      return ACT_REMOVE_COUNTERS_BASE;
    }
  }
  return 0xffffffff;
}


/* ManalinkEh 2022: 0x2666fe0; original functions/manipulate_and_damage_all.c:160. */
int action_on_card(int player, int card, int t_player, int t_card, actions_t raw_action)
{
  actions_t action;
  int result;
  counter_t counter_type;
  int counter_num;

  counter_type = COUNTER_invalid;
  counter_num = 0;
  action = cook_action(t_player, t_card, raw_action, &counter_type, &counter_num);
  result = 0;
  if (action < ~ACT_KILL_BURY)
  {
    result =
        action_on_card_impl(player, card, t_player, t_card, action, counter_type, counter_num);
  }
  return result;
}


/* ManalinkEh 2022: 0x26670c0; original functions/manipulate_and_damage_all.c:178. */
int new_manipulate_all(int player, int card, int t_player, test_definition_t *this_test,
                            actions_t raw_action)
{
  unsigned int uVar1;
  int iVar2;
  actions_t action;
  card_instance_t *instance;
  int iVar4;
  int iVar5;
  int *piVar6;
  int iVar7;
  int iVar8;
  int iVar9;
  int *piVar10;
  bool bVar11;
  int *piStack_358;
  int iStack_354;
  int iStack_350;
  int iStack_344;
  counter_t counter_type;
  int counter_num;
  int mc[2];
  int marked[2][100];

  iStack_344 = 0;
  bVar11 = t_player == 2;
  mc[0] = 0;
  mc[1] = 0;
  iVar4 = current_turn;
  iVar9 = current_turn;
  do
  {
    if ((t_player == iVar4) || (bVar11))
    {
      iVar7 = 1;
      iVar8 = active_cards_count[iVar4] + -1;
      iVar2 = iVar4;
      if (-1 < iVar8)
        goto LAB_026672f9;
    }
    else
    {
      iVar7 = 1;
      iVar2 = iVar9;
    }
    while (iVar9 = iVar2, iStack_344 = iVar7, iVar7 != 0)
    {
      iVar9 = 1 - iVar4;
      if (((t_player == iVar9) || (bVar11)) && (iVar8 = active_cards_count[iVar9] + -1, -1 < iVar8))
      {
      LAB_026672f9:
        do
        {
          uVar1 = this_test->zone;
          if (uVar1 == 0x200)
          {
            instance = in_play(iVar9, iVar8);
            if (((instance == NULL) || (iVar4 = is_what(iVar9, iVar8, 0x47), iVar4 == 0)) ||
                (iVar4 = check_state(iVar9, iVar8, 0x100000), iVar4 != 0))
            {
              uVar1 = this_test->zone;
              goto joined_r0x026672ed;
            }
          LAB_02667362:
            iVar4 = new_make_test_in_play(iVar9, iVar8, -1, this_test);
            if ((iVar4 != 0) &&
                (((this_test->not_me == 0 || (player != iVar9)) || (card != iVar8))))
            {
              iVar4 = mc[iVar9];
              mc[iVar9] = iVar4 + 1;
              marked[iVar9][iVar4] = iVar8;
              if ((raw_action < ACT_BOUNCE) || (raw_action == ACT_KILL_STATE_BASED_ACTION))
              {
                instance->kill_code = (uint8_t)raw_action;
                if (raw_action == ACT_KILL_STATE_BASED_ACTION)
                {
                  set_special_flags(iVar9, iVar8, 0x800);
                }
              }
            }
          }
          else
          {
          joined_r0x026672ed:
            if ((uVar1 == 0x100) && (instance = in_hand(iVar9, iVar8), instance != NULL))
              goto LAB_02667362;
          }
          iVar8 = iVar8 + -1;
        } while (iVar8 != -1);
      }
      iVar7 = iStack_344 + 1;
      iVar4 = current_turn;
      iVar2 = current_turn;
      if (1 < iVar7)
      {
        if (raw_action != ACT_BOUNCE)
          goto LAB_02667172;
        piStack_358 = marked[0];
        iStack_344 = 0;
        goto LAB_02667426;
      }
    }
  } while (true);
LAB_02667426:
  do
  {
    if (1 < mc[iStack_344])
    {
      iVar9 = mc[iStack_344] + -1;
      iVar4 = 0;
      piVar10 = piStack_358;
      do
      {
        while (true)
        {
          iVar2 = *piVar10;
          iVar7 = has_subtype(iStack_344, iVar2, SUBTYPE_AURA);
          iVar8 = iVar4 + 1;
          if (iVar7 == 0)
            break;
        LAB_02667450:
          piVar10 = piVar10 + 1;
          iVar4 = iVar8;
          if (iVar9 == iVar8)
            goto LAB_026674f8;
        }
        iVar7 = piVar10[1];
        iVar5 = has_subtype(iStack_344, iVar7, SUBTYPE_AURA);
        if (iVar5 == 0)
          goto LAB_02667450;
        piVar6 = piVar10;
        if (0 < iVar4)
        {
          while (true)
          {
            piVar6[1] = iVar2;
            piVar6 = piVar6 + -1;
            if (piStack_358 == piVar6)
              break;
            iVar2 = *piVar6;
          }
        }
        piVar10 = piVar10 + 1;
        *piStack_358 = iVar7;
        iVar4 = iVar8;
      } while (iVar9 != iVar8);
    }
  LAB_026674f8:
    iStack_344 = iStack_344 + 1;
    piStack_358 = piStack_358 + 100;
  } while (iStack_344 != 2);
LAB_02667172:
  iStack_344 = 1;
  iStack_354 = 0;
  iVar4 = current_turn;
  iVar9 = current_turn;
  do
  {
    if (iStack_344 == 1)
    {
      if (((t_player == iVar4) || (iVar2 = iVar9, bVar11)) &&
          (iStack_350 = mc[iVar4], iVar2 = iVar4, 0 < iStack_350))
        goto LAB_026671d9;
    }
    else
    {
      iVar9 = 1 - iVar4;
      if (((iVar9 == t_player) || (bVar11)) && (iStack_350 = mc[iVar9], 0 < iStack_350))
      {
      LAB_026671d9:
        iVar4 = 0;
        do
        {
          counter_type = COUNTER_invalid;
          counter_num = 0;
          iVar2 = marked[iVar9][iVar4];
          action = cook_action(iVar9, iVar2, raw_action, &counter_type, &counter_num);
          if (action == 0xffffffff)
          {
            return iStack_354;
          }
          if (action != ~ACT_KILL_BURY)
          {
            iVar2 = action_on_card_impl(player, card, iVar9, iVar2, action, counter_type,
                                             counter_num);
            iStack_354 = iStack_354 + iVar2;
          }
          iVar4 = iVar4 + 1;
        } while (iVar4 != iStack_350);
      }
      iVar4 = current_turn;
      iVar2 = current_turn;
      if (1 < iStack_344)
      {
        return iStack_354;
      }
    }
    iStack_344 = iStack_344 + 1;
    iVar9 = iVar2;
  } while (true);
}


/* ManalinkEh 2022: 0x26675a0; original functions/manipulate_and_damage_all.c:245. */
int manipulate_all(int player, int card, int t_player, int type, int flag1, int subtype,
                        int flag2, int color, int flag3, int id, int flag4, int cc, int flag5,
                        actions_t action)
{
  int result;
  test_definition_t this_test;
  new_default_test_definition(&this_test, type, "");
  this_test.type_flag = (unsigned short)flag1;
  this_test.subtype = subtype;
  this_test.subtype_flag = (unsigned short)flag2;
  this_test.color = color;
  this_test.color_flag = (unsigned short)flag3;
  this_test.id = id;
  this_test.id_flag = (unsigned short)flag4;
  this_test.cmc = cc;
  this_test.cmc_flag = (unsigned short)flag5;
  this_test.type = type;
  result = new_manipulate_all(player, card, t_player, &this_test, action);
  return result;
}

#endif


#ifndef SHANDALAR
/* ManalinkEh 2022: 0x26677f0; original functions/manipulate_and_damage_all.c:299. */


#endif
