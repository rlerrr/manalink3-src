#include "manalink.h"

/* Canonical recovered helper declarations. */
static int new_can_sacrifice_as_cost_impl(int player, int card, int amount,
                                               test_definition_t *test);
int new_can_sacrifice_as_cost(int player, int card, test_definition_t *test);
int max_can_sacrifice_as_cost(int player, int card, test_definition_t *test);
int new_can_sacrifice(int player, int card, int t_player, test_definition_t *test);
int new_get_special_permanent_for_sacrifice(int player, int card, test_definition_t *test);
int can_sacrifice_as_cost(int player, int amount, int type, int flag1, int subtype, int flag2,
                               int color, int flag3, int id, int flag4, int cc, int flag5);
int can_sacrifice_type_as_cost(int player, int amount, type_t type);
int can_sacrifice(int player, int t_player, int amount, type_t type, subtype_t subtype);
int impose_sacrifice(int player, int card, int t_player, int amount, int type, int flag1,
                          int subtype, int flag2, int color, int flag3, int id, int flag4, int cc,
                          int flag5);
int can_activate_altar_basic(int player, int card, int act_mode, int type);
static const char *can_sacrifice_this(int who_chooses, int player, int card);
int can_sacrifice_this_as_cost(int player, int card);
int can_cause_sacrifice(int player, int t_player);
int is_nice_creature_to_sacrifice(int player, int card);
int get_best_permanent_for_sacrifice(int player, int type);
int pick_creature_for_sacrifice(int player, int card, int cannot_cancel);
int pick_permanent_for_sacrifice(int player, int card, int type, int cannot_cancel);
int pick_special_permanent_for_sacrifice(int player, int card, int cannot_cancel, int type,
                                              int flag1, int subtype, int flag2, int color,
                                              int flag3, int id, int flag4, int cc, int flag5);
int sacrifice(int player, int card, int t_player, int cannot_cancel, int type, int flag1,
                   int subtype, int flag2, int color, int flag3, int id, int flag4, int cc,
                   int flag5);
void controller_sacrifices_a_creature(int player, int card);
void player_sacrifices_a_creature(int s_player, int s_card, int t_player);
int prevent_damage_sacrificing_a_creature(int player, int card, event_t event, int mode);
int check_battlefield_for_sacrifice(int player, int type, int flag1, int subtype, int flag2,
                                         int color, int flag3, int id, int flag4, int cc, int flag5);
int altar_basic_activation(int player, int card, int act_mode, int type);
int altar_basic(int player, int card, event_t event, int act_mode, int type);
int altar_extended(int player, int card, event_t event, int act_mode, int type, int flag1,
                        int subtype, int flag2, int color, int flag3, int id, int flag4, int cmc,
                        int flag5);
int sacrifice_and_report_value(int player, int card, int t_player, int flags,
                                    test_definition_t *test);
static void get_sacrifice_prompt_text(char *prompt, int type, int invert);
static int new_sacrifice_impl(int player, int card, int t_player, sacrifice_t options,
                                   test_definition_t *test, char *marked_for_sacrifice);
int new_sacrifice(int player, int card, int t_player, sacrifice_t options,
                       test_definition_t *test);
int mark_sacrifice(int player, int card, int t_player, sacrifice_t options,
                        test_definition_t *test, char *marked_for_sacrifice);
int controller_sacrifices_a_permanent(int player, int card, int req_type,
                                           sacrifice_t options);
int player_sacrifices_a_permanent(int src_player, int src_card, int t_player, int req_type,
                                       sacrifice_t options);
int player_sacrifices_a_hacked_land(int src_player, int src_card, int t_player,
                                         subtype_t land_subtype, sacrifice_t options);

// Returns nonzero if there's a non-humiliated Angel of Jubilation in play.  (check_battlefield_for_special_card returns true for humiliated ones.)






// Verify that "player" could sacrifice amount permanents matching test for paying a cost.












// Verify that a spell or an ability from "player" could make "t_player" sacrifice a specific type of permanent








static int get_special_permanent_for_sacrifice(int player, int type, int flag1, int subtype, int flag2,
											   int color, int flag3, int id, int flag4, int cc, int flag5){
	test_definition_t test;
	new_default_test_definition(&test, type, "");
	test.type_flag = flag1;
	test.subtype = subtype;
	test.subtype_flag = flag2;
	test.color = color;
	test.color_flag = flag3;
	test.id = id;
	test.id_flag = flag4;
	test.cmc = cc;
	test.cmc_flag = flag5;
	return new_get_special_permanent_for_sacrifice(player, -1, &test);
}

























// only usable with basic land subtypes; use player_sacrifices_a_permanent if you just want a land






















/* Recovered October 2022 implementations; see docs/recovery-progress.md. */

/* ManalinkEh 2022: 0x26e36e0; original functions/sacrifices.c:4. */
static type_t types_cannot_sac(void)
{
  card_instance_t *instance;
  int result;
  int card;
  int player;
  type_t tVar3;

  tVar3 = TYPE_NONE;
  player = 0;
  do
  {
    card = 0;
    if (0 < active_cards_count[player])
    {
      do
      {
        while (true)
        {
          instance = in_play(player, card);
          if (((instance != NULL) && (cards_data[instance->internal_card_id].id == 0x3010)) &&
              (result = is_humiliated(player, card, AL_STATIC), result == 0))
          {
            tVar3 = tVar3 | TYPE_CREATURE;
          }
          instance = in_play(player, card);
          if (((instance != NULL) && (cards_data[instance->internal_card_id].id == 0x5626)) &&
              (result = is_humiliated(player, card, AL_STATIC), result == 0))
            break;
          card = card + 1;
          if (active_cards_count[player] <= card)
            goto LAB_026e3798;
        }
        card = card + 1;
        tVar3 = TARGET_TYPE_PLANESWALKER | TYPE_ARTIFACT | TYPE_ENCHANTMENT | TYPE_CREATURE;
      } while (card < active_cards_count[player]);
    }
  LAB_026e3798:
    player = player + 1;
    if (player == 2)
    {
      return tVar3;
    }
  } while (true);
}


#ifndef SHANDALAR
/* Recovered October 2022 sacrifice family. */
typedef union
{
  int32_t rval;
  target_t target;
  token_generation_t token;
} sacrifice_selection_storage_t;

typedef union
{
  char prompt[200];
  test_definition_t test;
} sacrifice_prompt_storage_t;

_Static_assert(sizeof(sacrifice_selection_storage_t) == 76, "sacrifice shared target/token storage");
_Static_assert(sizeof(sacrifice_prompt_storage_t) == 216, "sacrifice shared prompt/test storage");

static test_definition_t *sacrifice_test;

/* ManalinkEh 2022: 0x26e37b0; original functions/sacrifices.c:31. */
static int new_can_sacrifice_as_cost_impl(int player, int card, int amount,
                                               test_definition_t *test)
{
  type_t test_type;
  card_instance_t *instance;
  int result;
  int card_00;
  unsigned int uStack_18;

  test_type = types_cannot_sac();
  uStack_18 = 0;
  if (0 < active_cards_count[player])
  {
    card_00 = 0;
    do
    {
      instance = in_play(player, card_00);
      if ((((instance != NULL) && (result = is_what(player, card_00, 0x47), result != 0)) &&
           ((card != card_00 || (test->not_me != 1)))) &&
          ((result = new_make_test_in_play(player, card_00, -1, test),
            result != 0 &&
                ((((test_type == TYPE_NONE ||
                    (result = is_what(player, card_00, test_type), result == 0)) &&
                   (result = check_special_flags2(player, card_00, 0x400000), result == 0)) &&
                  ((uStack_18 = uStack_18 + 1,
                    (char)((unsigned char)((unsigned int)amount >> 0x18) ^ 0xff) < '\0' &&
                        (amount <= (int)uStack_18))))))))
      {
        return 1;
      }
      card_00 = card_00 + 1;
    } while (card_00 < active_cards_count[player]);
  }
  return uStack_18 & amount >> 0x1f;
}


/* ManalinkEh 2022: 0x26e38d0; original functions/sacrifices.c:348. */
static const char *passes_test_definition_and_not_dying(int who_chooses, int player, int card,
                                                             int targeting_player,
                                                             int targeting_card)
{
  int result;
  card_instance_t *instance;
  result = new_make_test_in_play(player, card, -1, sacrifice_test);
  if (((result != 0) &&
       (result = check_special_flags2(player, card, 0x400000), result == 0)) &&
      (instance = get_card_instance(player, card), instance->kill_code == '\0'))
  {
    return NULL;
  }
  return "Illegal sacrifice";
}


/* ManalinkEh 2022: 0x26e3940; original functions/sacrifices.c:234. */
static const char *can_sacrifice_this(int who_chooses, int player, int card)
{
  int result;
  card_instance_t *instance;
  result = check_special_flags2(player, card, 0x400000);
  if ((result == 0) && (instance = get_card_instance(player, card), instance->kill_code == '\0'))
  {
    return NULL;
  }
  return "can\'t sacririce this";
}


/* ManalinkEh 2022: 0x26e3990; original functions/sacrifices.c:87. */
static int can_cause_sacrifice_part(int t_player)
{
  card_instance_t *instance;
  int result;
  int card;

  if (active_cards_count[t_player] < 1)
  {
    return 1;
  }
  card = 0;
  do
  {
    instance = in_play(t_player, card);
    if (instance != NULL)
    {
      result = get_id(t_player, card);
      if (((result == 0x30b1) || (result == 0x4e30)) || (result == 0x2b35))
      {
        result = is_humiliated(t_player, card, AL_STATIC);
        if (result == 0)
        {
          return 0;
        }
      }
    }
    card = card + 1;
  } while (card < active_cards_count[t_player]);
  return 1;
}


/* ManalinkEh 2022: 0x26e3a30; original functions/sacrifices.c:219. */
static void get_sacrifice_prompt_text(char *prompt, int type, int invert)
{
  const char *pcVar1;
  char typetext[100];
  if (ai_is_speculating == 1)
  {
    *prompt = '\0';
    return;
  }
  if (invert != 0)
  {
    pcVar1 = type_text(typetext, 100, type & (TARGET_TYPE_PLANESWALKER | TYPE_PERMANENT),
                       TYPETEXT_INVERT);
    scnprintf(prompt, 200, "Select %s permanent to sacrifice.", pcVar1);
    return;
  }
  pcVar1 = type_text(typetext, 100, type & (TARGET_TYPE_PLANESWALKER | TYPE_PERMANENT), TYPETEXT_0);
  scnprintf(prompt, 200, "Select %s to sacrifice.", pcVar1);
  return;
}


/* ManalinkEh 2022: 0x26e3af0; original functions/sacrifices.c:19. */
int can_sacrifice_this_as_cost(int player, int card)
{
  int result;
  type_t test_type;
  result = check_special_flags2(player, card, 0x400000);
  if (result != 0)
  {
    return 0;
  }
  test_type = types_cannot_sac();
  result = is_what(player, card, test_type);
  return (unsigned int)(result == 0);
}


/* ManalinkEh 2022: 0x26e3b50; original functions/sacrifices.c:53. */
int new_can_sacrifice_as_cost(int player, int card, test_definition_t *test)
{
  int result;
  result = new_can_sacrifice_as_cost_impl(player, card, (unsigned int)test->qty, test);
  return result;
}


/* ManalinkEh 2022: 0x26e3b70; original functions/sacrifices.c:58. */
int max_can_sacrifice_as_cost(int player, int card, test_definition_t *test)
{
  int result;
  result = new_can_sacrifice_as_cost_impl(player, card, -1, test);
  return result;
}


/* ManalinkEh 2022: 0x26e3b90; original functions/sacrifices.c:63. */
int can_sacrifice_as_cost(int player, int amount, int type, int flag1, int subtype, int flag2,
                               int color, int flag3, int id, int flag4, int cc, int flag5)
{
  int result;
  test_definition_t test;
  new_default_test_definition(&test, type, "");
  test.type_flag = (unsigned short)flag1;
  test.subtype = subtype;
  test.subtype_flag = (unsigned short)flag2;
  test.color = color;
  test.color_flag = (unsigned short)flag3;
  test.id = id;
  test.id_flag = (unsigned short)flag4;
  test.cmc = cc;
  test.cmc_flag = (unsigned short)flag5;
  test.qty = (unsigned short)amount;
  result = new_can_sacrifice_as_cost_impl(player, -1, amount & 0xffff, &test);
  return result;
}


/* ManalinkEh 2022: 0x26e3c20; original functions/sacrifices.c:79. */
int can_sacrifice_type_as_cost(int player, int amount, type_t type)
{
  int result;
  test_definition_t test;
  new_default_test_definition(&test, type, "");
  test.qty = (unsigned short)amount;
  result = new_can_sacrifice_as_cost_impl(player, -1, amount & 0xffff, &test);
  return result;
}


/* ManalinkEh 2022: 0x26e3c70; original functions/sacrifices.c:87. */
int can_cause_sacrifice(int player, int t_player)
{
  int result;

  if (player != t_player)
  {
    result = can_cause_sacrifice_part(t_player);
    return result;
  }
  return 1;
}


/* ManalinkEh 2022: 0x26e3c90; original functions/sacrifices.c:105. */
int new_can_sacrifice(int player, int card, int t_player, test_definition_t *test)
{
  int iVar1;
  card_instance_t *instance;
  int iVar3;
  unsigned int uStack_18;
  int iStack_14;

  if (test == NULL)
  {
    if ((player != t_player) && (iVar1 = can_cause_sacrifice_part(t_player), iVar1 == 0))
    {
      return 0;
    }
    uStack_18 = 1;
  }
  else
  {
    uStack_18 = (unsigned int)test->qty;
    if (uStack_18 == 0)
    {
      return 1;
    }
    if (player == t_player)
    {
      if (test->not_me == 1)
        goto LAB_026e3cda;
    }
    else
    {
      iVar1 = can_cause_sacrifice_part(t_player);
      if (iVar1 == 0)
      {
        return 0;
      }
    }
  }
  card = -1;
LAB_026e3cda:
  if (0 < active_cards_count[t_player])
  {
    iStack_14 = 0;
    iVar1 = 0;
    do
    {
      instance = in_play(t_player, iVar1);
      if ((card != iVar1) && (instance != NULL))
      {
        if (test == NULL)
        {
          iVar3 = is_what(t_player, iVar1, 0x47);
        }
        else
        {
          iVar3 = new_make_test_in_play(t_player, iVar1, -1, test);
        }
        if (((iVar3 != 0) &&
             (iVar3 = check_special_flags2(t_player, iVar1, 0x400000), iVar3 == 0)) &&
            (iStack_14 = iStack_14 + 1, (int)uStack_18 <= iStack_14))
        {
          return 1;
        }
      }
      iVar1 = iVar1 + 1;
    } while (iVar1 < active_cards_count[t_player]);
  }
  return 0;
}


/* ManalinkEh 2022: 0x26e3de0; original functions/sacrifices.c:128. */
int can_sacrifice(int player, int t_player, int amount, type_t type, subtype_t subtype)
{
  int result;
  test_definition_t test;
  new_default_test_definition(&test, type, "");
  test.subtype = subtype;
  test.qty = (unsigned short)amount;
  result = new_can_sacrifice(player, -1, t_player, &test);
  return result;
}


/* ManalinkEh 2022: 0x26e3e40; original functions/sacrifices.c:138. */
int is_nice_creature_to_sacrifice(int player, int card)
{
  uint8_t uVar1;
  card_data_t *pcVar2;
  card_instance_t *instance;
  int result;

  pcVar2 = get_card_data(player, card);
  uVar1 = pcVar2->cc[2];
  if (uVar1 == '\t')
  {
    result = is_what(player, card, 4);
    if (result == 0)
    {
      return 1;
    }
    uVar1 = pcVar2->cc[2];
  }
  if ((uVar1 != '\x02') && (uVar1 != '\x05'))
  {
    instance = get_card_instance(player, card);
    return instance->ext->sp_keywords2 >> 3 & 1;
  }
  return 1;
}


/* ManalinkEh 2022: 0x26e3ec0; original functions/sacrifices.c:156. */
int get_best_permanent_for_sacrifice(int player, int type)
{
  card_instance_t *instance;
  int result;
  int card;

  if (0 < active_cards_count[player])
  {
    card = 0;
    do
    {
      while ((((instance = in_play(player, card),
                instance != NULL && (result = is_what(player, card, type), result != 0)) &&
               (result = check_special_flags2(player, card, 0x400000), result == 0)) &&
              (result = is_nice_creature_to_sacrifice(player, card), result != 0)))
      {
        instance = get_card_instance(player, card);
        if (instance->kill_code < 2)
        {
          return card;
        }
        card = card + 1;
        if (active_cards_count[player] <= card)
        {
          return -1;
        }
      }
      card = card + 1;
    } while (card < active_cards_count[player]);
  }
  return -1;
}


/* ManalinkEh 2022: 0x26e3f70; original functions/sacrifices.c:171. */
int new_get_special_permanent_for_sacrifice(int player, int card, test_definition_t *test)
{
  card_instance_t *instance;
  int iVar2;
  int iVar3;
  int iVar4;
  int iStack_14;

  iVar4 = 0;
  if (0 < active_cards_count[player])
  {
    do
    {
      instance = in_play(player, iVar4);
      if (((((instance != NULL) &&
             (iVar2 = new_make_test_in_play(player, iVar4, -1, test), iVar2 != 0)) &&
            (iVar2 = is_nice_creature_to_sacrifice(player, iVar4), iVar2 != 0)) &&
           (((iVar2 = check_special_flags2(player, iVar4, 0x400000),
              iVar2 == 0 && (instance = get_card_instance(player, iVar4),
                             (instance->state & 0x100000) == 0)) &&
             ((test->not_me != 1 || (card != iVar4)))))) &&
          (instance->kill_code < 2))
      {
        return iVar4;
      }
      iVar4 = iVar4 + 1;
    } while (iVar4 < active_cards_count[player]);
    if (0 < active_cards_count[player])
    {
      iStack_14 = 1000;
      iVar4 = -1;
      iVar2 = 0;
      do
      {
        instance = in_play(player, iVar2);
        if ((((instance != NULL) &&
              (iVar3 = new_make_test_in_play(player, iVar2, -1, test), iVar3 != 0)) &&
             (((iVar3 = check_special_flags2(player, iVar2, 0x400000),
                iVar3 == 0 && (instance = get_card_instance(player, iVar2),
                               (instance->state & 0x100000) == 0)) &&
               (((test->not_me != 1 || (card != iVar2)) && (instance->kill_code < 2)))))) &&
            (iVar3 = get_base_value(player, iVar2), iVar3 < iStack_14))
        {
          iStack_14 = get_base_value(player, iVar2);
          iVar4 = iVar2;
        }
        iVar2 = iVar2 + 1;
      } while (iVar2 < active_cards_count[player]);
      return iVar4;
    }
  }
  return -1;
}


/* ManalinkEh 2022: 0x26e4110; original functions/sacrifices.c:355. */
static int new_sacrifice_impl(int player, int card, int t_player, sacrifice_t options,
                                   test_definition_t *test, char *marked_for_sacrifice)
{
  uint8_t uVar1;
  uint16_t uVar2;
  int32_t iVar3;
  int32_t iVar4;
  int iVar5;
  card_instance_t *pcVar6;
  int iVar7;
  card_instance_t *pcVar8;
  type_t tVar9;
  unsigned int uVar10;
  int iVar11;
  unsigned int uVar13;
  int iStack_23c;
  const char *prompt;
  int iStack_22c;
  unsigned int uStack_220;
  int iStack_218;
  sacrifice_selection_storage_t selection;
  target_definition_t td;
  char marked[151];
  char default_prompt[200];

  base_target_definition(player, card, &td, 0x47);
  td.allow_cancel = options & (SAC_DONE | SAC_NO_CANCEL) ^ SAC_NO_CANCEL;
  td.special = 0x8000;
  td.extra = (int32_t)passes_test_definition_and_not_dying;
  if (test->not_me == 1)
  {
    td.special = 0xc000;
  }
  uVar13 = test->type & 0x147;
  uVar10 = td.required_type;
  if (((uVar13 != 0) && (uVar2 = test->type_flag, uVar10 = uVar13, uVar2 != 0)) &&
      (uVar10 = td.required_type, uVar2 == 1))
  {
    td.illegal_type = uVar13;
  }
  td.required_type = uVar10;
  td.who_chooses = t_player;
  td.allowed_controller = t_player;
  td.preferred_controller = t_player;
  if (((options & SAC_AS_COST) != 0) && (tVar9 = types_cannot_sac(), tVar9 != TYPE_NONE))
  {
    td.illegal_type = td.illegal_type | tVar9;
  }
  if ((((options & SAC_CAUSED) != 0) && (player != t_player)) &&
      (iVar5 = can_cause_sacrifice_part(t_player), iVar5 == 0))
  {
    spell_fizzled = 1;
    return 0;
  }
  pcVar6 = get_card_instance(player, card);
  iVar5 = spell_fizzled;
  sacrifice_test = test;
  if ((options & SAC_ALL_OR_NONE) == 0)
  {
    iVar7 = can_target(&td);
    if (iVar7 == 0)
    {
      spell_fizzled = 1;
      return 0;
    }
  }
  else
  {
    iVar7 = target_available(player, card, &td);
    if (iVar7 < (int)(unsigned int)test->qty)
    {
      spell_fizzled = 1;
      return 0;
    }
  }
  uStack_220 = (unsigned int)test->qty;
  iVar7 = target_available(player, card, &td);
  if (iVar7 < (int)uStack_220)
  {
    uStack_220 = target_available(player, card, &td);
  }
  if (test->message[0] == '\0')
  {
    get_sacrifice_prompt_text(default_prompt, uVar13,
                                        (unsigned int)(test->type_flag == 1));
    prompt = default_prompt;
  }
  else
  {
    prompt = test->message;
  }
  uVar1 = pcVar6->number_of_targets;
  iVar3 = pcVar6->targets[0].player;
  iVar4 = pcVar6->targets[0].card;
  memset(marked, 0, sizeof(marked));
  if ((int)uStack_220 < 1)
  {
    if ((spell_fizzled != 1) && ((options & SAC_OPTIONAL) != 0))
    {
      iStack_23c = -1;
      iStack_22c = -1;
      iStack_218 = 0;
    LAB_026e445c:
      spell_fizzled = iVar5;
      if ((options & SAC_SET_TARGETS) != 0)
      {
        pcVar6->number_of_targets = '\x01';
        pcVar6->targets[0].player = iStack_23c;
        pcVar6->targets[0].card = iStack_22c;
        iVar7 = get_color(iStack_23c, iStack_22c);
        pcVar6->targets[1].player = iVar7;
        iVar7 = get_id(iStack_23c, iStack_22c);
        pcVar6->targets[1].card = iVar7;
        iVar7 = get_power(iStack_23c, iStack_22c);
        pcVar6->targets[2].player = iVar7;
        iVar7 = get_toughness(iStack_23c, iStack_22c);
        pcVar6->targets[2].card = iVar7;
        pcVar8 = get_card_instance(iStack_23c, iStack_22c);
        pcVar6->targets[3].card = pcVar8->internal_card_id;
        iVar7 = get_id(player, card);
        if (iVar7 == 0x2f91)
        {
          iVar7 = has_subtype(iStack_23c, iStack_22c, SUBTYPE_HUMAN);
          pcVar6->targets[3].player = iVar7;
        }
        else if ((iVar7 == 0x24f3) &&
                 (iVar7 = has_subtype(iStack_23c, iStack_22c, SUBTYPE_GOBLIN), iVar7 != 0))
        {
          default_token_definition(player, card, 0x493b, &selection.token);
          selection.token.t_player = iStack_23c;
          selection.token.qty = '\x02';
          selection.token.action = TOKEN_ACTION_HASTE;
          generate_token(&selection.token);
        }
      }
      iVar7 = 0;
      if (0 < active_cards_count[t_player])
      {
        do
        {
          while (marked[iVar7] != '\0')
          {
            pcVar6 = get_card_instance(t_player, iVar7);
            iVar7 = iVar7 + 1;
            pcVar6->state = pcVar6->state & 0xffdfffff | 0x100000;
            iVar11 = active_cards_count[t_player];
            if (iVar11 <= iVar7)
              goto LAB_026e44d4;
          }
          iVar7 = iVar7 + 1;
          iVar11 = active_cards_count[t_player];
        } while (iVar7 < iVar11);
      LAB_026e44d4:
        if (((options & SAC_JUST_MARK) == 0) && (0 < iVar11))
        {
          iVar7 = 0;
        LAB_026e44f2:
          do
          {
            if (marked[iVar7] != '\0')
            {
              pcVar6 = get_card_instance(t_player, iVar7);
              pcVar6->state = pcVar6->state & 0xffefffff;
              pcVar6 = in_play(t_player, iVar7);
              if (pcVar6 != NULL)
              {
                iVar11 = iVar7 + 1;
                kill_card(t_player, iVar7, KILL_SACRIFICE);
                iVar7 = iVar11;
                if (active_cards_count[t_player] <= iVar11)
                  break;
                goto LAB_026e44f2;
              }
            }
            iVar7 = iVar7 + 1;
          } while (iVar7 < active_cards_count[t_player]);
        }
      }
      if ((options & SAC_RETURN_CHOICE) != 0)
      {
        selection.rval = (int32_t)((uint16_t)iStack_218 | ((uint32_t)(uint8_t)iStack_23c << 16) |
                                   ((uint32_t)(uint8_t)iStack_22c << 24));
        iStack_218 = selection.rval;
      }
      if (marked_for_sacrifice == NULL)
      {
        spell_fizzled = iVar5;
        return iStack_218;
      }
      memcpy(marked_for_sacrifice, marked, sizeof(marked));
      spell_fizzled = iVar5;
      return iStack_218;
    }
  }
  else
  {
    iVar7 = -1;
    if (player == t_player)
    {
      iVar7 = card;
    }
    iStack_23c = -1;
    iStack_22c = -1;
    iStack_218 = 0;
    do
    {
      spell_fizzled = 0;
      selection.rval = -1;
      selection.target.card = -1;
      if ((ai_is_speculating == 1) || ((t_player == 1 && ((trace_mode & 2U) == 0))))
      {
        iVar11 = new_get_special_permanent_for_sacrifice(t_player, iVar7, test);
        if (iVar11 == -1)
        {
          if (selection.target.card == -1)
            goto LAB_026e4361;
          goto LAB_026e43ad;
        }
        selection.rval = t_player;
        selection.target.card = iVar11;
        if (iVar11 < 0)
          goto LAB_026e468a;
      LAB_026e43b5:
        if (marked[selection.target.card] == '\0')
        {
          pcVar8 = get_card_instance(selection.rval, selection.target.card);
          if (ai_is_speculating == 1)
          {
            uVar10 = 0x300000;
          }
          else
          {
            uVar10 = 0x200000;
            if (t_player == 1)
            {
              uVar10 = (-(unsigned int)((trace_mode & 2U) == 0) & 0x100000) + 0x200000;
            }
          }
          pcVar8->state = pcVar8->state | uVar10;
          iStack_218 = iStack_218 + 1;
          iStack_22c = selection.target.card;
          marked[selection.target.card] = '\x01';
          iStack_23c = selection.rval;
        }
        else
        {
          pcVar8 = get_card_instance(selection.rval, selection.target.card);
          iStack_218 = iStack_218 + -1;
          pcVar8->state = pcVar8->state & 0xffdfffff;
          marked[selection.target.card] = '\0';
        }
      }
      else
      {
      LAB_026e4361:
        sacrifice_test = test;
        pcVar6->number_of_targets = '\0';
        select_target(player, card, &td, prompt, &selection.target);
      LAB_026e43ad:
        if (-1 < selection.target.card)
          goto LAB_026e43b5;
      LAB_026e468a:
        if ((options & (SAC_DONE | SAC_NO_CANCEL)) != SAC_NO_CANCEL)
        {
          pcVar6->number_of_targets = uVar1;
          pcVar6->targets[0].player = iVar3;
          pcVar6->targets[0].card = iVar4;
          if ((selection.target.card == -1) ||
              ((iStack_218 == 0 && ((options & SAC_OPTIONAL) == 0))))
            goto LAB_026e46d3;
          goto LAB_026e4440;
        }
      }
    } while (iStack_218 < (int)uStack_220);
    pcVar6->number_of_targets = uVar1;
    pcVar6->targets[0].player = iVar3;
    pcVar6->targets[0].card = iVar4;
    if (spell_fizzled != 1)
    {
    LAB_026e4440:
      if (((int)uStack_220 <= iStack_218) || ((options & SAC_ALL_OR_NONE) == 0))
        goto LAB_026e445c;
    }
  }
LAB_026e46d3:
  iVar5 = 0;
  spell_fizzled = 1;
  if (0 < active_cards_count[t_player])
  {
    do
    {
      while (marked[iVar5] == '\0')
      {
        iVar5 = iVar5 + 1;
        if (active_cards_count[t_player] <= iVar5)
        {
          return 0;
        }
      }
      pcVar6 = get_card_instance(t_player, iVar5);
      iVar5 = iVar5 + 1;
      iVar7 = active_cards_count[t_player];
      pcVar6->state = pcVar6->state & 0xffcfffff;
    } while (iVar5 < iVar7);
  }
  return 0;
}


/* ManalinkEh 2022: 0x26e4a50; original functions/sacrifices.c:242. */
int pick_creature_for_sacrifice(int player, int card, int cannot_cancel)
{
  card_instance_t *instance;
  int result;
  target_definition_t td;

  default_target_definition(player, card, &td, 2);
  td.allow_cancel = (unsigned int)(cannot_cancel == 0) * 2;
  td.allowed_controller = player;
  td.preferred_controller = player;
  td.illegal_abilities = 0;
  td.who_chooses = player;
  td.extra = (int32_t)can_sacrifice_this;
  td.special = 0x8000;
  instance = get_card_instance(player, card);
  result = can_target(&td);
  if (result != 0)
  {
    if ((player == 1) && (result = get_best_permanent_for_sacrifice(1, 2), result != -1))
    {
      return result;
    }
    result = new_pick_target(&td, "LORD_OF_THE_PIT", 0, 0);
    if (result != 0)
    {
      instance->number_of_targets = '\x01';
      return instance->targets[0].card;
    }
  }
  return -1;
}


/* ManalinkEh 2022: 0x26e4b30; original functions/sacrifices.c:279. */
int pick_permanent_for_sacrifice(int player, int card, int type, int cannot_cancel)
{
  card_instance_t *instance;
  int result;
  target_definition_t td;
  char prompt[200];

  default_target_definition(player, card, &td, type);
  td.allow_cancel = 1 - cannot_cancel;
  td.allowed_controller = player;
  td.preferred_controller = player;
  td.illegal_abilities = 0;
  td.who_chooses = player;
  td.extra = (int32_t)can_sacrifice_this;
  td.special = 0x8000;
  instance = get_card_instance(player, card);
  result = can_target(&td);
  if (result != 0)
  {
    if (player == 1)
    {
      result = get_best_permanent_for_sacrifice(1, type);
      if (result != -1)
      {
        return result;
      }
    }
    get_sacrifice_prompt_text(prompt, type, 0);
    select_target(player, card, &td, prompt, instance->targets);
    if (spell_fizzled != 1)
    {
      instance->number_of_targets = '\x01';
      return instance->targets[0].card;
    }
  }
  return -1;
}


/* ManalinkEh 2022: 0x26e4c50; original functions/sacrifices.c:311. */
int pick_special_permanent_for_sacrifice(int player, int card, int cannot_cancel, int type,
                                              int flag1, int subtype, int flag2, int color,
                                              int flag3, int id, int flag4, int cc, int flag5)
{
  card_instance_t *instance;
  int result;
  target_definition_t td;
  sacrifice_prompt_storage_t storage;

  default_target_definition(player, card, &td, type);
  td.allow_cancel = 1 - cannot_cancel;
  td.allowed_controller = player;
  td.preferred_controller = player;
  td.illegal_abilities = 0;
  td.who_chooses = player;
  td.extra = (int32_t)can_sacrifice_this;
  td.special = 0x8000;
  instance = get_card_instance(player, card);
  result = can_target(&td);
  if (result != 0)
  {
    if (player == 1)
    {
      new_default_test_definition(&storage.test, type, "");
      storage.test.type_flag = (uint16_t)flag1;
      storage.test.subtype = subtype;
      storage.test.subtype_flag = (uint16_t)flag2;
      storage.test.color = color;
      storage.test.color_flag = (uint16_t)flag3;
      storage.test.id = id;
      storage.test.id_flag = (uint16_t)flag4;
      storage.test.cmc = cc;
      storage.test.cmc_flag = (uint16_t)flag5;
      result = new_get_special_permanent_for_sacrifice(1, -1, &storage.test);
      return result;
    }
    get_sacrifice_prompt_text(storage.prompt, type, (unsigned int)(flag1 == 1));
    select_target(player, card, &td, storage.prompt, instance->targets);
    if (spell_fizzled != 1)
    {
      result =
          make_test_in_play(instance->targets[0].player, instance->targets[0].card, -1, type,
                                 flag1, subtype, flag2, color, flag3, id, flag4, cc, flag5);
      if (result != 0)
      {
        instance->number_of_targets = '\x01';
        return instance->targets[0].card;
      }
    }
  }
  return -1;
}


/* ManalinkEh 2022: 0x26e4e30; original functions/sacrifices.c:536. */
int new_sacrifice(int player, int card, int t_player, sacrifice_t options,
                       test_definition_t *test)
{
  int result;
  result = new_sacrifice_impl(player, card, t_player, options, test, NULL);
  return result;
}


/* ManalinkEh 2022: 0x26e4e60; original functions/sacrifices.c:541. */
int mark_sacrifice(int player, int card, int t_player, sacrifice_t options,
                        test_definition_t *test, char *marked_for_sacrifice)
{
  int result;
  result = new_sacrifice_impl(player, card, t_player, options, test, marked_for_sacrifice);
  return result;
}


/* ManalinkEh 2022: 0x26e4e90; original functions/sacrifices.c:546. */
int sacrifice(int player, int card, int t_player, int cannot_cancel, int type, int flag1,
                   int subtype, int flag2, int color, int flag3, int id, int flag4, int cc,
                   int flag5)
{
  int result;
  sacrifice_t options;
  test_definition_t test;

  new_default_test_definition(&test, type, "");
  test.type_flag = (unsigned short)flag1;
  test.subtype = subtype;
  test.subtype_flag = (unsigned short)flag2;
  test.color = color;
  test.color_flag = (unsigned short)flag3;
  test.id = id;
  test.id_flag = (unsigned short)flag4;
  test.cmc = cc;
  test.cmc_flag = (unsigned short)flag5;
  if (cannot_cancel == 1)
  {
    options = SAC_SET_TARGETS | SAC_NO_CANCEL;
  }
  else if (cannot_cancel == -1)
  {
    options = SAC_SET_TARGETS | SAC_DONE;
  }
  else
  {
    options = cannot_cancel | SAC_SET_TARGETS;
  }
  result = new_sacrifice_impl(player, card, t_player, options, &test, NULL);
  return result;
}


/* ManalinkEh 2022: 0x26e4f40; original functions/sacrifices.c:565. */
int controller_sacrifices_a_permanent(int player, int card, int req_type,
                                           sacrifice_t options)
{
  int result;
  test_definition_t test;
  new_default_test_definition(&test, req_type, "");
  result = new_sacrifice_impl(player, card, player, options, &test, NULL);
  return result;
}


/* ManalinkEh 2022: 0x26e4f90; original functions/sacrifices.c:572. */
int player_sacrifices_a_permanent(int src_player, int src_card, int t_player, int req_type,
                                       sacrifice_t options)
{
  int result;
  test_definition_t test;
  new_default_test_definition(&test, req_type, "");
  result = new_sacrifice_impl(src_player, src_card, t_player, options, &test, NULL);
  return result;
}


/* ManalinkEh 2022: 0x26e4fe0; original functions/sacrifices.c:580. */
int player_sacrifices_a_hacked_land(int src_player, int src_card, int t_player,
                                         subtype_t land_subtype, sacrifice_t options)
{
  int result;
  const char *msg;
  test_definition_t test;

  if ((src_player != t_player) && (result = can_cause_sacrifice_part(t_player), result == 0))
  {
    return 0;
  }
  msg = get_hacked_land_text(src_player, src_card, "Select %a to sacrifice.", land_subtype);
  new_default_test_definition(&test, 1, msg);
  test.subtype = get_hacked_subtype(src_player, src_card, land_subtype);
  result = new_sacrifice_impl(src_player, src_card, t_player, options, &test, NULL);
  return result;
}


/* ManalinkEh 2022: 0x26e5080; original functions/sacrifices.c:593. */
int impose_sacrifice(int player, int card, int t_player, int amount, int type, int flag1,
                          int subtype, int flag2, int color, int flag3, int id, int flag4, int cc,
                          int flag5)
{
  int iVar1;
  int iVar2;

  if (amount < 1)
  {
    return 0;
  }
  if ((player != t_player) && (iVar1 = can_cause_sacrifice_part(t_player), iVar1 == 0))
  {
    return 0;
  }
  iVar1 = 0;
  do
  {
    iVar2 = sacrifice(player, card, t_player, 1, type, flag1, subtype, flag2, color, flag3, id,
                           flag4, cc, flag5);
    if (iVar2 == 0)
    {
      return iVar1;
    }
    iVar1 = iVar1 + 1;
  } while (amount != iVar1);
  return iVar1;
}


/* ManalinkEh 2022: 0x26e5140; original functions/sacrifices.c:618. */
void controller_sacrifices_a_creature(int player, int card)
{
  sacrifice(player, card, player, 1, 2, 0, 0, 0, 0, 0, 0, 0, -1, 0);
  return;
}


/* ManalinkEh 2022: 0x26e51c0; original functions/sacrifices.c:622. */
void player_sacrifices_a_creature(int s_player, int s_card, int t_player)
{
  sacrifice(s_player, s_card, t_player, 1, 2, 0, 0, 0, 0, 0, 0, 0, -1, 0);
  return;
}


/* ManalinkEh 2022: 0x26e5240; original functions/sacrifices.c:626. */
int prevent_damage_sacrificing_a_creature(int player, int card, event_t event, int mode)
{
  card_instance_t *instance;
  int result;
  target_definition_t td;
  target_definition_t td1;

  default_target_definition(player, card, &td, 0);
  td.extra = damage_card;
  td.allow_cancel = 0;
  default_target_definition(player, card, &td1, 2);
  td1.allowed_controller = player;
  td1.preferred_controller = player;
  td1.illegal_abilities = 0;
  instance = get_card_instance(player, card);
  if (event == EVENT_CAN_ACTIVATE)
  {
    result = can_sacrifice_as_cost(player, 1, 2, 0, 0, 0, 0, 0, 0, 0, -1, 0);
    if ((result != 0) && (result = can_target(&td), result != 0))
    {
      if (mode == 1)
      {
        result = has_mana(player, 0, 1);
        if (result != 0)
        {
          return 99;
        }
      }
      else if (mode == 2)
      {
        return 99;
      }
      return 0;
    }
    goto LAB_026e52cc;
  }
  if (event != EVENT_ACTIVATE)
  {
    if ((event == EVENT_RESOLVE_ACTIVATION) &&
        (instance = get_card_instance(instance->targets[0].player, instance->targets[0].card),
         0 < instance->info_slot))
    {
      instance->info_slot = 0;
    }
    goto LAB_026e52cc;
  }
  if (mode == 1)
  {
    charge_mana(player, COLOR_COLORLESS, 1);
    if (spell_fizzled != 1)
      goto LAB_026e53cd;
  }
  else if (mode == 2)
  {
  LAB_026e53cd:
    result = controller_sacrifices_a_permanent(player, card, 2, 0);
    if (result != 0)
    {
      pick_target(&td, "TARGET_DAMAGE");
      goto LAB_026e52cc;
    }
  }
  spell_fizzled = 1;
LAB_026e52cc:
  result = global_enchantment(player, card, event);
  return result;
}


/* ManalinkEh 2022: 0x26e5460; original functions/sacrifices.c:684. */
int check_battlefield_for_sacrifice(int player, int type, int flag1, int subtype, int flag2,
                                         int color, int flag3, int id, int flag4, int cc, int flag5)
{
  card_instance_t *instance;
  int iVar2;
  int iVar3;
  int iVar4;
  if (player < 2)
  {
    if (0 < active_cards_count[player])
    {
      iVar3 = 0;
      do
      {
        instance = in_play(player, iVar3);
        if ((instance != NULL) &&
            (iVar4 = make_test_in_play(player, iVar3, -1, type, flag1, subtype, flag2, color,
                                            flag3, id, flag4, cc, flag5),
             iVar4 != 0))
        {
          return 1;
        }
        iVar3 = iVar3 + 1;
      } while (iVar3 < active_cards_count[player]);
    }
  }
  else if (player == 2)
  {
    iVar3 = 0;
    do
    {
      iVar4 = 0;
      if (0 < active_cards_count[iVar3])
      {
        do
        {
          instance = in_play(iVar3, iVar4);
          if ((instance != NULL) &&
              (iVar2 = make_test_in_play(iVar3, iVar4, -1, type, flag1, subtype, flag2, color,
                                              flag3, id, flag4, cc, flag5),
               iVar2 != 0))
          {
            return 1;
          }
          iVar4 = iVar4 + 1;
        } while (iVar4 < active_cards_count[iVar3]);
      }
      iVar3 = iVar3 + 1;
    } while (iVar3 != 2);
  }
  return 0;
}


/* ManalinkEh 2022: 0x26e55d0; original functions/sacrifices.c:717. */
int can_activate_altar_basic(int player, int card, int act_mode, int type)
{
  int result;
  unsigned int uVar2;

  uVar2 = 0;
  result = is_sick(player, card);
  if (result == 0)
  {
    uVar2 = act_mode & 1;
  }
  result = is_animated_and_sick(player, card);
  if ((result == 0) && ((act_mode & 2U) != 0))
  {
    uVar2 = uVar2 + 2;
  }
  result = is_tapped(player, card);
  if ((result == 0) && ((act_mode & 4U) != 0))
  {
    uVar2 = uVar2 + 4;
  }
  if ((current_phase == 4) && ((act_mode & 8U) != 0))
  {
    uVar2 = uVar2 + 8;
  }
  if ((current_turn == player) && ((act_mode & 0x10U) != 0))
  {
    uVar2 = uVar2 + 0x10;
  }
  if (uVar2 != act_mode)
  {
    return 0;
  }
  result = can_sacrifice_as_cost(player, 1, type, 0, 0, 0, 0, 0, 0, 0, -1, 0);
  return result;
}


/* ManalinkEh 2022: 0x26e56f0; original functions/sacrifices.c:741. */
int altar_basic_activation(int player, int card, int act_mode, int type)
{
  card_instance_t *instance;
  int iVar2;
  int iVar3;

  instance = get_card_instance(player, card);
  iVar2 = charge_mana_for_activated_ability(player, card, 0, 0, 0, 0, 0, 0);
  if ((iVar2 != 0) &&
      (iVar2 = pick_permanent_for_sacrifice(player, card, type, 0), iVar2 != -1))
  {
    instance->number_of_targets = '\x01';
    iVar3 = get_power(player, iVar2);
    instance->targets[1].player = iVar3;
    iVar3 = get_toughness(player, iVar2);
    instance->targets[1].card = iVar3;
    iVar3 = get_color(player, iVar2);
    instance->targets[2].player = iVar3;
    iVar3 = get_cmc(player, iVar2);
    instance->targets[2].card = iVar3;
    kill_card(player, iVar2, KILL_SACRIFICE);
    if ((act_mode & 4U) == 0)
    {
      return 1;
    }
    tap_card(player, card);
    return 1;
  }
  return 0;
}


/* ManalinkEh 2022: 0x26e5810; original functions/sacrifices.c:763. */
int altar_basic(int player, int card, event_t event, int act_mode, int type)
{
  int result;
  if (event == EVENT_CAN_ACTIVATE)
  {
    result = generic_activated_ability(player, card, EVENT_CAN_ACTIVATE, 0, 0, 0, 0, 0, 0, 0, 0,
                                       NULL, NULL);
    if (result != 0)
    {
      result = can_activate_altar_basic(player, card, act_mode, type);
      return result;
    }
  }
  else if (event == EVENT_ACTIVATE)
  {
    result = altar_basic_activation(player, card, act_mode, type);
    if (result == 0)
    {
      spell_fizzled = 1;
    }
  }
  return 0;
}


/* ManalinkEh 2022: 0x26e5900; original functions/sacrifices.c:782. */
int altar_extended(int player, int card, event_t event, int act_mode, int type, int flag1,
                        int subtype, int flag2, int color, int flag3, int id, int flag4, int cmc,
                        int flag5)
{
  card_instance_t *instance;
  int iVar2;
  int iVar3;
  unsigned int uVar4;

  instance = get_card_instance(player, card);
  if (event == EVENT_CAN_ACTIVATE)
  {
    iVar2 = can_use_activated_abilities(player, card);
    if (iVar2 != 0)
    {
      iVar2 = is_sick(player, card);
      uVar4 = 0;
      if (iVar2 == 0)
      {
        uVar4 = act_mode & 1;
      }
      iVar2 = is_animated_and_sick(player, card);
      if ((iVar2 == 0) && ((act_mode & 2U) != 0))
      {
        uVar4 = uVar4 + 2;
      }
      iVar2 = is_tapped(player, card);
      if ((iVar2 == 0) && ((act_mode & 4U) != 0))
      {
        uVar4 = uVar4 + 4;
      }
      if ((current_phase == 4) && ((act_mode & 8U) != 0))
      {
        uVar4 = uVar4 + 8;
      }
      if ((current_turn == player) && ((act_mode & 0x10U) != 0))
      {
        uVar4 = uVar4 + 0x10;
      }
      if (uVar4 == act_mode)
      {
        iVar2 = can_sacrifice_as_cost(player, 1, type, flag1, subtype, flag2, color, flag3, id,
                                           flag4, cmc, flag5);
        return iVar2;
      }
    }
  }
  else if (event == EVENT_ACTIVATE)
  {
    iVar2 = pick_special_permanent_for_sacrifice(player, card, 0, type, flag1, subtype, flag2,
                                                      color, flag3, id, flag4, cmc, flag5);
    if (iVar2 == -1)
    {
      spell_fizzled = 1;
    }
    else
    {
      iVar3 = get_power(player, iVar2);
      instance->targets[1].player = iVar3;
      iVar3 = get_toughness(player, iVar2);
      instance->targets[1].card = iVar3;
      iVar3 = get_color(player, iVar2);
      instance->targets[2].player = iVar3;
      iVar3 = get_cmc(player, iVar2);
      instance->targets[2].card = iVar3;
      kill_card(player, iVar2, KILL_SACRIFICE);
      if ((act_mode & 4U) != 0)
      {
        tap_card(player, card);
      }
    }
  }
  return 0;
}


/* ManalinkEh 2022: 0x26e5b80; original functions/sacrifices.c:830. */
int sacrifice_and_report_value(int player, int card, int t_player, int flags,
                                    test_definition_t *test)
{
  unsigned int uVar1;
  card_instance_t *instance;
  int32_t result;
  unsigned int card_00;

  uVar1 = new_sacrifice_impl(
      player, card, t_player, (flags >> 0x1f & 0x21U) + (SAC_RETURN_CHOICE | SAC_JUST_MARK),
      test, NULL);
  if ((int)uVar1 < 0)
  {
    result = -1;
  }
  else
  {
    card_00 = uVar1 >> 0x18;
    uVar1 = (int)uVar1 >> 0x10 & 0xff;
    instance = get_card_instance(uVar1, card_00);
    if ((flags & 1U) == 0)
    {
      result = instance->internal_card_id;
    }
    else
    {
      result = get_power(uVar1, card_00);
    }
    if ((flags & 2U) != 0)
    {
      result = get_toughness(uVar1, card_00);
    }
    if ((flags & 4U) != 0)
    {
      result = get_color(uVar1, card_00);
    }
    if ((flags & 0x10U) != 0)
    {
      result = get_cmc(uVar1, card_00);
    }
    kill_card(uVar1, card_00, KILL_SACRIFICE);
  }
  return result;
}

#endif
