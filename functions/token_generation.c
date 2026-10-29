#include "manalink.h"

/* Canonical recovered helper declarations. */
static int legacy_negate_static_abilities(int player, int card, event_t event);
void default_token_definition(int player, int card, int id, token_generation_t *token);
void copy_token_definition(int player, int card, token_generation_t *token, int t_player,
                                int t_card);
void generate_token(token_generation_t *token);
static void set_token(int player, int card, token_generation_t *token);
void generate_tokens_by_id(int player, int card, int csvid, int howmany);
int generate_reserved_token_by_id(int player, int csvid);
void copy_token(int player, int card, int tok_player, int tok_card);
int generic_token(int player, int card, event_t event);
int token_characteristic_setting_effects(int player, int card, event_t event, int t_player,
                                              int t_card);
int copy_token_characteristics_for_clone(int player, int card, int t_player, int t_card);

static int generate_token_by_id_impl(int player, int card, int csvid, int number, int reserved);
int generate_token_by_id(int player, int card, int csvid);



static int get_copyable_special_infos(int t_player, int t_card);

// Create a token definition that's a copy of (t_player,t_card).




/* Just like generic_token(), but supports storing the values on an effect card to set them on a different permanent.  The idea is that changing a permanent to
 * temporarily be a copy of a token shouldn't have to overwrite all of its targets. */






void convert_to_token(int player, int card){
	card_instance_t *instance = get_card_instance(player, card);
	instance->token_status |= STATUS_TOKEN;
}

static int get_updated_tokens_number(int player, int number)
{
  card_instance_t* instance;
  int p, c;
  for (p = 0; p <= 1; ++p)
	for (c = 0; c < active_cards_count[p]; ++c)
	  if ((instance = in_play(p, c)))
		switch (cards_data[instance->internal_card_id].id)
		  {
			case CARD_ID_DOUBLING_SEASON:		// self only: doubles
			case CARD_ID_PARALLEL_LIVES:
			case CARD_ID_ANOINTED_PROCESSION:
			  if (p != player)
				continue;
			  // else fall through
			case CARD_ID_SELESNYA_LOFT_GARDENS:	// everyone: doubles
			case CARD_ID_PRIMAL_VIGOR:
			  number *= 2;
			  break;
		  }
  return number;
}

static int is_reserved_id(int csvid){
	int iid = get_internal_card_id_from_csv_id(csvid);
	return (cards_data[iid].cc[2] == 3	// vanguard card, edh, etc.
			|| cards_data[iid].type == TYPE_EFFECT);
}

void set_special_infos(int player, int card, int infos){
	card_instance_t *instance = get_card_instance(player, card);
	instance->targets[14].player = infos;
}

int get_special_infos(int player, int card)
{
  if (player < 0 || card < 0)
    return 0;
  card_instance_t *instance = get_card_instance(player, card);
  if (instance->targets[14].player > -1)
  {
    return instance->targets[14].player;
  }
  return 0;
}



static int real_generate_token(int player, int int_id, token_generation_t *token, int delay_put_into_play){
	int card_added = add_card_to_hand( player, int_id );
	set_token(player, card_added, token);
	if (delay_put_into_play){
		get_card_instance(player, card_added)->state |= STATE_INVISIBLE;
		--hand_count[player];
	} else {
		put_into_play(player, card_added);
	}
	return card_added;
}













void copy_all_tokens(int player, int card, int type, test_definition_t *test)
{
  int c, marked[151] = {0};
  for (c = 0; c < active_cards_count[player]; ++c)
  {
    if (in_play(player, c) && is_token(player, c))
    {
      if ((type && is_what(player, c, type)) ||
          (test != NULL && new_make_test_in_play(player, c, -1, test)))
      {
        marked[c] = 1;
      }
    }
  }

  for (c = active_cards_count[player] - 1; c > -1; c--)
  {
    if (marked[c] && player >= 0)
    {
      copy_token(player, card, player, c);
    }
  }
}

static int from_subtype_to_token_id(int subtype)
{
  /* Native signed 16-bit token IDs, indexed by creature subtype minus two. */
  static const int16_t token_ids[221] = {
      18687, 0,     0,     18689, 0,     0,     0,     18691, 0,     0,     0,     18692, 0,
      0,     0,     18693, 18694, 18695, 0,     0,     18696, 0,     18698, 0,     0,     18701,
      0,     18702, 0,     18704, 18708, 0,     0,     18709, 18710, 0,     18712, 0,     0,
      0,     0,     0,     18714, 18715, 18716, 0,     18719, 18721, 0,     0,     0,     0,
      0,     0,     0,     18722, 18726, 18729, 0,     0,     0,     18735, 0,     18738, 0,
      0,     0,     0,     18740, 18741, 18742, 18744, 18745, 18746, 18752, 0,     18753, 0,
      18755, 0,     18756, 18757, 18758, 0,     0,     18759, 18761, 0,     18763, 18764, 18768,
      0,     18769, 0,     0,     18770, 0,     0,     18773, 0,     0,     18776, 0,     0,
      18780, 0,     0,     0,     0,     0,     18783, 0,     0,     0,     18785, 0,     18787,
      18788, 0,     0,     18789, 0,     0,     18792, 0,     0,     0,     0,     0,     0,
      0,     0,     18794, 18795, 18796, 18797, 0,     0,     0,     0,     0,     18798, 18799,
      18800, 0,     0,     18801, 0,     18803, 0,     18804, 0,     18806, 0,     18807, 18808,
      0,     0,     0,     0,     0,     18813, 18814, 0,     0,     0,     18815, 0,     0,
      0,     18817, 18818, 0,     18819, 0,     18820, 0,     18821, 18822, 0,     18824, 0,
      0,     18826, 18827, 18828, 18829, 18831, 0,     18832, 18833, 18834, 18836, 0,     0,
      0,     18837, 18838, 18841, 18844, 0,     0,     0,     18848, 0,     0,     0,     0,
      18852, 18853, 0,     18854, 0,     18856, 0,     0,     18858, 0,     0,     0,     18860,
  };
  unsigned int index = (unsigned int)subtype - 2;
  return index < 221 ? token_ids[index] : 0;
}

static void store_selected_subtype(token_generation_t* token, int beast, int unused)
{
	get_card_instance(token->t_player, beast)->targets[1].card = token->action_argument;
}

void generate_custom_token_from_subtype(int player, int card, int subtype, int color, int pow,
                                        int tou, int amount)
{
  int csvid = from_subtype_to_token_id(subtype);
  token_generation_t token;
  if (csvid)
  {
    default_token_definition(player, card, csvid, &token);
    int iid = get_internal_card_id_from_csv_id(csvid);
    if (cards_data[iid].static_ability)
    {
      token.legacy = 1;
      token.special_code_for_legacy = legacy_negate_static_abilities;
    }
  }
  else
  {
    default_token_definition(player, card, CARD_ID_RIPTIDE_REPLICATOR_TOKEN, &token);
    token.action = TOKEN_ACTION_ADD_SUBTYPE;
    token.special_code_on_generation = store_selected_subtype;
    token.action_argument = subtype;
  }
  token.qty = amount;
  token.color_forced = color;
  token.pow = pow;
  token.tou = tou;
  token.no_sleight = 1;
  generate_token(&token);
}

/* Recovered October 2022 implementations; see docs/recovery-progress.md. */

/* ManalinkEh 2022: 0x2664e30; original functions/token_generation.c:945. */
static int legacy_negate_static_abilities(int player, int card, event_t event)
{
  card_instance_t *instance;

  instance = get_card_instance(player, card);
  if (event == EVENT_ABILITIES)
  {
    if ((instance->damage_target_card == affected_card) &&
        (instance->damage_target_player == affected_card_controller))
    {
      event_result = event_result & 0xffffffdf;
    }
  }
  else if (((event == EVENT_CHANGE_TYPE) && (card == affected_card)) &&
           (player == affected_card_controller))
  {
    instance->regen_status = instance->regen_status | 0x1f000000;
  }
  return 0;
}

/* Recovered October 2022 implementations; see docs/recovery-progress.md. */

/* ManalinkEh 2022: 0x26668f0; original functions/token_generation.c:680. */
void copy_all_tokens_both_players(int player, int card, int type, test_definition_t *test)
{
  card_instance_t *instance;
  int iVar2;
  int iVar3;
  int iVar4;
  int *piVar5;
  unsigned char bVar6;
  int marked[151];

  iVar3 = current_turn;

  bVar6 = 0;
  iVar2 = active_cards_count[current_turn];
  if (0 < iVar2)
  {
    piVar5 = marked;
    for (iVar4 = 0x97; iVar4 != 0; iVar4 = iVar4 + -1)
    {
      *piVar5 = 0;
      piVar5 = piVar5 + 1;
    }
    if (0 < iVar2)
    {
      iVar4 = 0;
      do
      {
        instance = in_play(iVar3, iVar4);
        if (((instance != NULL) && (iVar2 = is_token(iVar3, iVar4), iVar2 != 0)) &&
            (((type != 0 && (iVar2 = is_what(iVar3, iVar4, type), iVar2 != 0)) ||
              ((test != NULL &&
                (iVar2 = new_make_test_in_play(iVar3, iVar4, -1, test), iVar2 != 0))))))
        {
          marked[iVar4] = 1;
        }
        iVar4 = iVar4 + 1;
        iVar2 = active_cards_count[iVar3];
      } while (iVar4 < iVar2);
    }
    iVar2 = iVar2 + -1;
    if (-1 < iVar2)
    {
      do
      {

        if ((marked[iVar2] != 0) && (-1 < iVar3))
        {
          copy_token(iVar3, iVar2, iVar3, iVar2);
        }
        iVar2 = iVar2 + -1;
      } while (iVar2 != -1);
    }
  }
  iVar3 = 1 - current_turn;
  iVar2 = active_cards_count[iVar3];
  if (0 < iVar2)
  {
    piVar5 = marked;
    for (iVar4 = 0x97; iVar4 != 0; iVar4 = iVar4 + -1)
    {
      *piVar5 = 0;
      piVar5 = piVar5 + (unsigned int)bVar6 * -2 + 1;
    }
    if (0 < iVar2)
    {
      iVar4 = 0;
      do
      {
        instance = in_play(iVar3, iVar4);
        if (((instance != NULL) && (iVar2 = is_token(iVar3, iVar4), iVar2 != 0)) &&
            (((type != 0 && (iVar2 = is_what(iVar3, iVar4, type), iVar2 != 0)) ||
              ((test != NULL &&
                (iVar2 = new_make_test_in_play(iVar3, iVar4, -1, test), iVar2 != 0))))))
        {
          marked[iVar4] = 1;
        }
        iVar4 = iVar4 + 1;
        iVar2 = active_cards_count[iVar3];
      } while (iVar4 < iVar2);
    }
    iVar2 = iVar2 + -1;
    if (-1 < iVar2)
    {
      do
      {

        if ((marked[iVar2] != 0) && (-1 < iVar3))
        {
          copy_token(iVar3, iVar2, iVar3, iVar2);
        }
        iVar2 = iVar2 + -1;
      } while (iVar2 != -1);
    }
  }
  return;
}


/* October 2022 token creation component. */
void generate_token(token_generation_t *token);
static void set_token(int player, int card, token_generation_t *token);
void default_token_definition(int player, int card, int id, token_generation_t *token);
static int resolve_token_iid(iid_t int_id, token_generation_t *token);
static int get_copyable_special_infos(int t_player, int t_card);
void copy_token_definition(int player, int card, token_generation_t *token, int t_player,
                                int t_card);
int token_characteristic_setting_effects(int player, int card, event_t event, int t_player,
                                              int t_card);
int generic_token(int player, int card, event_t event);

/* ManalinkEh 2022: 0x26659d0; original functions/token_generation.c:503. */
void generate_token(token_generation_t *token)
{
  uint16_t uVar1;
  uint16_t uVar2;
  int32_t iVar3;
  int32_t iVar4;
  bool bVar5;
  bool bVar6;
  uint8_t uVar7;
  int iVar8;
  card_instance_t *pcVar9;
  token_action_t tVar10;
  int iVar11;
  card_instance_t *pcVar12;
  int iVar13;
  int card;
  int iVar14;
  unsigned int uVar15;
  unsigned int uStack_ac;
  int iStack_9c;
  card_instance_t *pcStack_98;
  int iStack_94;
  int iStack_90;
  iid_t iStack_8c;
  int iStack_88;
  int aiStack_84[8];
  target_definition_t td;

  iVar8 = get_internal_card_id_from_csv_id(token->id);
  uStack_ac = (unsigned int)token->qty;
  iVar8 = is_what(-1, iVar8, 0x80);
  if (iVar8 == 0)
  {
    iVar8 = token->t_player;
    iVar14 = 0;
    do
    {
      iVar13 = 0;
      if (0 < active_cards_count[iVar14])
      {
      LAB_026664f0:
        do
        {
          pcVar9 = in_play(iVar14, iVar13);
          if (pcVar9 != NULL)
          {
            uVar2 = cards_data[pcVar9->internal_card_id].id;
            if (uVar2 != 0x3541)
            {
              if ((unsigned short)uVar2 < 0x3542)
              {
                if ((uVar2 != 0x1e90) && (uVar2 != 0x2f09))
                {
                  iVar13 = iVar13 + 1;
                  if (active_cards_count[iVar14] <= iVar13)
                    break;
                  goto LAB_026664f0;
                }
              }
              else
              {
                if (uVar2 == 0x39ba)
                  goto LAB_02666529;
                if (uVar2 != 0x41c4)
                  goto LAB_0266652b;
              }
              if (iVar8 != iVar14)
                goto LAB_0266652b;
            }
          LAB_02666529:
            uStack_ac = uStack_ac * 2;
          }
        LAB_0266652b:
          iVar13 = iVar13 + 1;
        } while (iVar13 < active_cards_count[iVar14]);
      }
      iVar14 = iVar14 + 1;
    } while (iVar14 != 2);
  }
  if ((int)uStack_ac < 1)
  {
    return;
  }
  iStack_94 = token->id;
  aiStack_84[2] = 0xffffffff;
  aiStack_84[0] = iStack_94;
  aiStack_84[3] = 0;
  aiStack_84[1] = uStack_ac;
  iVar8 = token->t_player;
  aiStack_84[4] = 0xffffffff;
  aiStack_84[5] = 0;
  aiStack_84[6] = 0xffffffff;
  aiStack_84[7] = 0;
  if (active_cards_count[iVar8] < 1)
  {
    bVar5 = false;
  }
  else
  {
    iVar13 = 0;
    bVar5 = false;
    iStack_88 = 0;
    iVar14 = 0;
  LAB_02665aa9:
    do
    {
      pcVar9 = in_play(iVar8, iVar13);
      if ((pcVar9 != NULL) &&
          (iVar8 = is_humiliated(token->t_player, iVar13, AL_STATIC), iVar8 == 0))
      {
        iVar8 = get_id(token->t_player, iVar13);
        iVar14 = iVar14 + (unsigned int)(iVar8 == 0x59a8);
        iVar8 = get_id(token->t_player, iVar13);
        iStack_88 = iStack_88 + (unsigned int)(iVar8 == 0x59ce);
        iVar8 = get_id(token->t_player, iVar13);
        if (iVar8 == 0x4b8f)
        {
          iVar8 = token->t_player;
          iVar13 = iVar13 + 1;
          bVar5 = true;
          if (active_cards_count[iVar8] <= iVar13)
            break;
          goto LAB_02665aa9;
        }
      }
      iVar8 = token->t_player;
      iVar13 = iVar13 + 1;
    } while (iVar13 < active_cards_count[iVar8]);
    for (; iVar14 != 0; iVar14 = iVar14 + -1)
    {
      while (iStack_94 != 0x4917)
      {
        if (iStack_94 == 0x5077)
        {
          if (1 < iVar14)
          {
            aiStack_84[1] = aiStack_84[1] + uStack_ac;
          }
          aiStack_84[2] = 0x4917;
          aiStack_84[3] = aiStack_84[3] + uStack_ac;
          aiStack_84[4] = 0x4998;
          aiStack_84[5] = aiStack_84[5] + uStack_ac;
        }
        else if (iStack_94 == 0x4998)
        {
          if (1 < iVar14)
          {
            aiStack_84[1] = aiStack_84[1] + uStack_ac;
          }
          aiStack_84[2] = 0x5077;
          aiStack_84[3] = aiStack_84[3] + uStack_ac;
          aiStack_84[4] = 0x4917;
          aiStack_84[5] = aiStack_84[5] + uStack_ac;
        }
        iVar14 = iVar14 + -1;
        if (iVar14 == 0)
          goto LAB_02665be0;
      }
      if (1 < iVar14)
      {
        aiStack_84[1] = aiStack_84[1] + uStack_ac;
      }
      aiStack_84[2] = 0x5077;
      aiStack_84[3] = aiStack_84[3] + uStack_ac;
      aiStack_84[4] = 0x4998;
      aiStack_84[5] = aiStack_84[5] + uStack_ac;
    }
  LAB_02665be0:
    if (iStack_88 != 0)
    {
      aiStack_84[6] = 0x4991;
      aiStack_84[7] = (iStack_88 + -1) * uStack_ac + uStack_ac;
    }
  }
  iStack_90 = 0;
  do
  {
    if (-1 < iStack_94)
    {
      if (((bVar5) && (iVar8 = get_internal_card_id_from_csv_id(iStack_94),
                       cards_data[iVar8].cc[2] != '\x03')) &&
          (cards_data[iVar8].type != 0x80))
      {
        iStack_94 = 0x48ff;
        aiStack_84[iStack_90 * 2] = 0x48ff;
      }
      iStack_8c = get_internal_card_id_from_csv_id(iStack_94);
      iStack_88 = 0;
      iVar8 = aiStack_84[iStack_90 * 2 + 1];
      if (0 < iVar8)
      {
        do
        {
          if ((0 < iStack_90) &&
              (default_token_definition(token->s_player, token->s_card, iStack_94, token),
               iStack_94 == 0x4991))
          {
            token->tou = 1;
            token->pow = 1;
            token->color_forced = '\b';
          }
          if (((bVar5) && (iVar14 = get_internal_card_id_from_csv_id(iStack_94),
                           cards_data[iVar14].cc[2] != '\x03')) &&
              (cards_data[iVar14].type != 0x80))
          {
            token->key_plus = 0x20;
            token->s_key_plus = 8;
            token->tou = 4;
            token->pow = 4;
            token->color_forced = ' ';
            token->special_infos = 0;
          }
          iStack_8c = resolve_token_iid(iStack_8c, token);
          if (token->eff_card == -1)
          {
            if (((token->keep_track_of_tokens_generated != 0) || (token->legacy == '\x01')) ||
                ((token->action & (TOKEN_ACTION_PUMP_TOUGHNESS | TOKEN_ACTION_PUMP_POWER |
                                   TOKEN_ACTION_HASTE | TOKEN_ACTION_EQUIP)) != 0))
            {
              backtrace_and_abort("functions/token_generation.c:392\ntoken->eff_card != -1 || "
                                  "!(token->keep_track_of_tokens_generated > 0 || token->legacy == "
                                  "1 || (token->action & (TOKEN_ACTION_EQUIP | TOKEN_ACTION_HASTE "
                                  "| TOKEN_ACTION_PUMP_POWER | TOKEN_ACTION_PUMP_TOUGHNESS)))");
            }
          LAB_02665cab:
            pcStack_98 = NULL;
          }
          else
          {
            if (token->keep_track_of_tokens_generated == 0)
              goto LAB_02665cab;
            pcStack_98 = get_card_instance(token->eff_player, token->eff_card);
          }
          iVar14 = token->t_player;
          iVar13 = add_card_to_hand(iVar14, iStack_8c);
          set_token(iVar14, iVar13, token);
          pcVar9 = get_card_instance(iVar14, iVar13);
          pcVar9->state = pcVar9->state | 0x20;
          hand_count[iVar14] = hand_count[iVar14] + -1;
          pcVar9 = get_card_instance(token->t_player, iVar13);
          if (token->legacy == '\x01')
          {
            iVar14 = create_targetted_legacy_effect(token->eff_player, token->eff_card,
                                                    token->special_code_for_legacy, token->t_player,
                                                    iVar13);
            pcVar12 = get_card_instance(token->eff_player, iVar14);
            pcVar12->eot_toughness = 0x300;
            uVar1 = cards_data[iStack_8c].id;
            pcVar12->display_pic_csv_id = uVar1;
          }
          tVar10 = token->action;
          if ((tVar10 & TOKEN_ACTION_TAPPED) != 0)
          {
            pcVar9->state = pcVar9->state | 0x10;
          }
          if (((tVar10 & TOKEN_ACTION_ATTACKING_UNTAPPED) != 0) &&
              (pcVar9->state = pcVar9->state | 4,
               (tVar10 & TOKEN_ACTION_SKIP_ATTACK_CHECK) == 0))
          {
            choose_who_attack(token->t_player, iVar13);
            tVar10 = token->action;
          }
          if ((tVar10 & TOKEN_ACTION_BLOCKING) != 0)
          {
            pcVar12 = get_card_instance(1 - token->t_player, token->action_argument);
            uVar7 = pcVar12->blocking;
            if (uVar7 == 0xff)
            {
              uVar7 = (uint8_t)token->action_argument;
            }
            pcVar9->blocking = uVar7;
            tVar10 = token->action;
            pcVar9->state = pcVar9->state | 0x8008;
          }
          if (((tVar10 & TOKEN_ACTION_EQUIP) != 0) && (iStack_88 == 0))
          {
            equip_target_creature(token->eff_player, token->eff_card, token->t_player, iVar13);
            tVar10 = token->action;
          }
          if (((tVar10 & TOKEN_ACTION_EQUIP_ANY) != 0) && (iStack_88 == 0))
          {
            default_target_definition(token->eff_player, token->eff_card, &td, 0xff);
            td.required_subtype = 0x3001;
            td.illegal_abilities = 0;
            pcVar12 = get_card_instance(token->eff_player, token->eff_card);
            iVar3 = pcVar12->targets[0].player;
            iVar4 = pcVar12->targets[0].card;
            iVar14 = can_target(&td);
            if ((iVar14 != 0) &&
                (iVar14 = new_pick_target(&td, "Select an equipment you control", 0, 0x80),
                 iVar14 != 0))
            {
              equip_target_creature(pcVar12->targets[0].player, pcVar12->targets[0].card,
                                    token->t_player, iVar13);
            }
            pcVar12->targets[0].player = iVar3;
            pcVar12->targets[0].card = iVar4;
            tVar10 = token->action;
          }
          if ((tVar10 & TOKEN_ACTION_DONT_COPY_TOKEN_SOURCE) != 0)
          {
            set_special_flags(token->t_player, iVar13, 0x40000000);
            tVar10 = token->action;
          }
          if ((tVar10 & (TOKEN_ACTION_PUMP_TOUGHNESS | TOKEN_ACTION_PUMP_POWER |
                         TOKEN_ACTION_HASTE)) != 0)
          {
            iStack_9c = 0;
            if ((tVar10 & TOKEN_ACTION_PUMP_TOUGHNESS) != 0)
            {
              iStack_9c = token->action_argument;
            }
            iVar14 = 0;
            if ((tVar10 & TOKEN_ACTION_PUMP_POWER) != 0)
            {
              iVar14 = token->action_argument;
            }
            pump_ability_until_eot(token->eff_player, token->eff_card, token->t_player, iVar13,
                                   iVar14, iStack_9c, 0, tVar10 >> 2 & 4);
            tVar10 = token->action;
          }
          if ((tVar10 & TOKEN_ACTION_ADD_SUBTYPE) != 0)
          {
            add_a_subtype_permanently(token->eff_player, token->eff_card, token->t_player, iVar13,
                                      token->action_argument);
            tVar10 = token->action;
          }
          if ((tVar10 & TOKEN_ACTION_RECORD_SOURCE_ID) != 0)
          {
            iVar14 = get_id(token->s_player, token->s_card);
            pcVar9->damage_source_card = iVar14;
          }
          if (token->special_code_on_generation != NULL)
          {
            (*token->special_code_on_generation)(token, iVar13, iStack_88);
          }
          iVar14 = token->t_player;
          if (iStack_88 < (int)token->keep_track_of_tokens_generated)
          {
            pcStack_98->targets[iStack_88].player = iVar14;
            pcStack_98->targets[iStack_88].card = iVar13;
          }
          put_into_play(iVar14, iVar13);
          tVar10 = token->action;
          if ((tVar10 & TOKEN_ACTION_ADD_COUNTERS) != 0)
          {
            hack_silent_counters = hack_silent_counters + 1;
            add_counters(token->s_player, token->s_card, token->t_player, iVar13,
                                     (unsigned int)token->action_argument >> 0x10,
                                     token->action_argument & 0xffff);
            hack_silent_counters = hack_silent_counters + -1;
            tVar10 = token->action;
          }
          if ((((tVar10 & TOKEN_ACTION_BLOCKING) == 0) ||
               (pcVar9 = in_play(token->t_player, iVar13), pcVar9 == NULL)) ||
              (play_sound_effect(WAV_BLOCK2),
               (((unsigned int)event_flags >> 16 & 0xff) & 0x10) == 0))
          {
            uVar15 = token->special_infos;
          }
          else
          {
            dispatch_trigger2(current_turn, TRIGGER_BLOCKER_CHOSEN, EXE_STR(0x790074), 0,
                              token->t_player, iVar13);
            uVar15 = token->special_infos;
          }
          if (uVar15 == 99)
          {
            uVar15 = 0;
            iVar14 = get_id(token->t_player, iVar13);
            bVar6 = false;
            do
            {
              card = active_cards_count[uVar15] + -1;
              if (-1 < card)
              {
                do
                {
                  pcVar9 = in_play(uVar15, card);
                  if ((((pcVar9 != NULL) && ((token->t_player != uVar15 || (iVar13 != card)))) &&
                       (iVar11 = get_id(uVar15, card), iVar14 == iVar11)) &&
                      (((int)(uVar15 ^ 0xffffffff) < 0 && (pcVar9 = get_card_instance(uVar15, card),
                                                           pcVar9->targets[0xe].player == 99))))
                  {
                    kill_card(uVar15, card, KILL_SACRIFICE);
                    bVar6 = true;
                  }
                  card = card + -1;
                } while (card != -1);
              }
              uVar15 = uVar15 + 1;
            } while (uVar15 != 2);
            if (bVar6)
            {
              kill_card(token->t_player, iVar13, KILL_SACRIFICE);
            }
          }
          if (iStack_88 < (int)token->keep_track_of_tokens_generated)
          {
            pcVar9 = get_card_instance(token->eff_player, token->eff_card);
            pcVar9->targets[iStack_88].player = token->t_player;
            pcVar9->targets[iStack_88].card = iVar13;
          }
          iStack_88 = iStack_88 + 1;
        } while (iStack_88 != iVar8);
      }
      increase_trap_condition(token->t_player, 0x206, iVar8);
    }
    iStack_90 = iStack_90 + 1;
    if (iStack_90 == 4)
    {
      uVar15 = token->keep_track_of_tokens_generated;
      if (uVar15 == 0)
      {
        return;
      }
      if (uVar15 < uStack_ac)
      {
        uStack_ac = uVar15;
      }
      token->keep_track_of_tokens_generated = uStack_ac;
      return;
    }
    iStack_94 = aiStack_84[iStack_90 * 2];
  } while (true);
}

/* ManalinkEh 2022: 0x2664eb0; original functions/token_generation.c:150. */
static void set_token(int player, int card, token_generation_t *token)
{
  uint32_t uVar1;
  unsigned int uVar2;
  card_instance_t *instance;

  instance = get_card_instance(player, card);
  instance->damage_source_player = (int8_t)token->s_player;
  instance->damage_source_card = token->s_card;
  uVar1 = token->s_key_plus;
  instance->ext->sp_keywords = uVar1;
  instance->token_special_keywords = uVar1;
  set_special_flags2(player, card, token->special_flags2);
  instance = get_card_instance(player, card);
  uVar2 = token->special_infos;
  instance->token_status = instance->token_status | 0x10;
  instance = get_card_instance(player, card);
  instance->targets[0xe].player = uVar2;
  return;
}

/* ManalinkEh 2022: 0x2665470; original functions/token_generation.c:5. */
void default_token_definition(int player, int card, int id, token_generation_t *token)
{
  int result;
  card_instance_t *instance;
  token->id = id;
  token->eff_player = player;
  token->eff_card = card;
  token->t_player = player;
  if ((player < 0) || (card < 0))
  {
  LAB_02665498:
    token->s_card = -1;
    token->s_player = -1;
    token->no_sleight = '\x01';
  }
  else
  {
    instance = get_card_instance(player, card);
    result = instance->internal_card_id;
    if (result == activation_card)
    {
      player = instance->parent_controller;
      card = instance->parent_card;
    LAB_0266554c:
      if ((player < 0) || (card < 0))
        goto LAB_02665498;
    }
    else
    {
      if ((-1 < result) && (cards_data[result].id - 0x385 < 8))
      {
        player = (int)instance->damage_source_player;
        card = instance->damage_source_card;
        goto LAB_0266554c;
      }
    }
    token->s_player = player;
    token->s_card = card;
    token->no_sleight = '\0';
  }
  token->pow = -1;
  token->tou = -1;
  token->key_plus = 0;
  token->s_key_plus = 0;
  token->color_forced = '\0';
  token->special_code_for_legacy = empty;
  token->special_code_on_generation = NULL;
  token->legacy = '\0';
  token->qty = '\x01';
  token->special_infos = 0;
  token->action = 0;
  token->action_argument = 0;
  token->keep_track_of_tokens_generated = 0;
  token->special_flags2 = 0;
  token->special_flags3 = 0;
  return;
}

/* ManalinkEh 2022: 0x2664f30; original functions/token_generation.c:295. */
static int resolve_token_iid(iid_t int_id, token_generation_t *token)
{
  unsigned char bVar1;
  unsigned int uVar2;
  int iVar3;
  token_action_t tVar4;
  unsigned char bVar5;
  char cVar6;
  char cVar7;
  char cVar8;
  int iVar9;
  card_instance_t *instance;
  int iVar11;
  int iVar12;
  uint32_t uVar13;
  card_data_t *pcVar14;
  int *piVar15;
  card_data_t *pcVar16;
  bool bVar17;
  unsigned char bVar18;
  unsigned int uStack_6c;
  unsigned int uStack_64;
  unsigned int uStack_58;
  int dynused[16];

  bVar18 = 0;
  iVar9 = get_internal_card_id_from_csv_id((unsigned int)cards_data[int_id].id);
  iVar11 = engine_dynamic_iid_start;
  if ((cards_data[iVar9].cc[2] != '\x03') && (cards_data[iVar9].type != 0x80))
  {
    iVar9 = engine_dynamic_iid_start + 0x10;
    iVar3 = engine_dynamic_iid_start;
    do
    {
      iVar12 = iVar3;
      uVar2 = (uint32_t)cards_data[iVar12].id | ((uint32_t)cards_data[iVar12].reserved2 << 16);
      if (((((uint16_t)uVar2 == cards_data[int_id].id) && (token->id == (uVar2 & 0xffff))) &&
           ((token->pow < 0 || (token->pow == (int)cards_data[iVar12].power)))) &&
          (((token->tou < 0 || (token->tou == (int)cards_data[iVar12].toughness)) &&
            ((cards_data[iVar12].static_ability & token->key_plus) != 0))))
      {
        bVar1 = 1;
        if ((token->action & TOKEN_ACTION_CONVERT_INTO_ARTIFACT) != 0)
        {
          bVar1 = cards_data[iVar12].type >> 6 & 1;
        }
        uStack_58 = (unsigned int)cards_data[iVar12].color;
        bVar5 = token->color_forced;
        if (token->color_forced == 0)
        {
          bVar5 = cards_data[iVar12].color;
        }
        uStack_6c = (unsigned int)bVar5;
        if (((-1 < token->s_player) && (-1 < token->s_card)) && (token->no_sleight == '\0'))
        {
          uStack_6c = get_sleighted_color_test(token->s_player, token->s_card, bVar5 & 0x3e);
          uStack_58 = (unsigned int)cards_data[iVar12].color;
        }
        if ((uStack_58 == uStack_6c) && (bVar1 != 0))
        {
          return iVar12;
        }
      }
      iVar3 = iVar12 + 1;
    } while (iVar12 + 1 != iVar9);
    bVar1 = cards_data[int_id].color;
    if (token->color_forced != 0)
    {
      bVar1 = token->color_forced;
    }
    uStack_64 = (unsigned int)bVar1;
    if (((-1 < token->s_player) && (-1 < token->s_card)) && (token->no_sleight == '\0'))
    {
      uStack_64 = get_sleighted_color_test(token->s_player, token->s_card, uStack_64 & 0x3e);
    }
    if (((((-1 < token->pow) && (token->pow != (int)cards_data[int_id].power)) ||
          ((-1 < token->tou && (token->tou != (int)cards_data[int_id].toughness)))) ||
         ((token->key_plus != 0 && (token->key_plus != cards_data[int_id].static_ability)))) ||
        ((cards_data[int_id].color != uStack_64 ||
          ((((cards_data[int_id].type & 0x40) == 0 &&
             ((token->action & TOKEN_ACTION_CONVERT_INTO_ARTIFACT) != 0)) ||
            ((token->action & TOKEN_ACTION_ISNT_LEGENDARY) != 0))))))
    {
      uStack_58 = 0;
      piVar15 = dynused;
      for (iVar9 = 0x10; iVar9 != 0; iVar9 = iVar9 + -1)
      {
        *piVar15 = 0;
        piVar15 = piVar15 + (unsigned int)bVar18 * -2 + 1;
      }
      do
      {
        iVar9 = 0;
        do
        {
          while (true)
          {
            instance = get_card_instance(uStack_58, iVar9);
            uVar13 = instance->original_internal_card_id;
            if (uVar13 == 0xffffffff)
            {
              uVar13 = (uint32_t)instance->backup_internal_card_id;
            }
            if ((iVar11 <= (int)uVar13) && ((int)uVar13 <= iVar11 + 0xf))
            {
              dynused[uVar13 - iVar11] = 1;
            }
            iVar3 = instance->dummy3;
            if ((iVar11 + 0xf < iVar3 || iVar3 < iVar11) || (uVar13 == 0))
              break;
            iVar9 = iVar9 + 1;
            dynused[iVar3 - iVar11] = 1;
            if (iVar9 == 0x96)
              goto LAB_0266519c;
          }
          iVar9 = iVar9 + 1;
        } while (iVar9 != 0x96);
      LAB_0266519c:
        uStack_58 = uStack_58 + 1;
        iVar9 = iVar11;
        if (uStack_58 == 2)
        {
          do
          {
            if (((dynused[iVar9 - iVar11] == 0) && (engine_dynamic_iid_start <= iVar9)) &&
                (iVar9 <= engine_dynamic_iid_start + 0xf))
            {
              cards_data[iVar9].id = 0xffff;
            }
            bVar17 = iVar12 != iVar9;
            iVar9 = iVar9 + 1;
          } while (bVar17);
          iVar11 = create_a_card_type(int_id);
          cards_data[iVar11] = cards_data[int_id];
          iVar9 = token->pow;
          if ((-1 < iVar9) && (iVar9 != cards_data[int_id].power))
          {
            cards_data[iVar11].power = (int16_t)iVar9;
          }
          iVar9 = token->tou;
          if ((-1 < iVar9) && (iVar9 != cards_data[int_id].toughness))
          {
            cards_data[iVar11].toughness = (int16_t)iVar9;
          }
          if (token->key_plus != 0)
          {
            cards_data[iVar11].static_ability = cards_data[iVar11].static_ability | token->key_plus;
          }
          tVar4 = token->action;
          if (((cards_data[int_id].type & 0x40) == 0) &&
              ((tVar4 & TOKEN_ACTION_CONVERT_INTO_ARTIFACT) != 0))
          {
            cards_data[iVar11].type = cards_data[iVar11].type | 0x40;
          }
          if ((tVar4 & TOKEN_ACTION_ISNT_LEGENDARY) != 0)
          {
            cards_data[iVar11].subtype = '\0';
          }
          cards_data[iVar11].color = (uint8_t)uStack_64;
          return iVar11;
        }
      } while (true);
    }
  }
  return int_id;
}

/* ManalinkEh 2022: 0x2665380; original functions/token_generation.c:218. */
static int get_copyable_special_infos(int t_player, int t_card)
{
  unsigned int uVar1;
  card_instance_t *instance;
  int result;
  unsigned int uVar4;

  instance = get_card_instance(t_player, t_card);
  uVar1 = instance->targets[0xe].player;
  uVar4 = (int)(uVar1 ^ 0xffffffff) >> 0x1f & uVar1;
  if ((int)uVar1 < 1)
  {
    return uVar4;
  }
  result = get_backup_id(t_player, t_card);
  if (result < 0x493b)
  {
    if (0x4902 < result)
    {
      switch (result)
      {
      case 0x4903:
      case 0x4905:
      case 0x4916:
      case 0x4925:
        goto LAB_02665409;
      default:
        goto LAB_026653e3;
      case 0x491f:
        goto LAB_0266545b;
      case 0x4926:
      case 0x493a:
      code_r0x0266544e:
        if (uVar1 != 0x43)
        {
          return 0;
        }
        return uVar4;
      }
    }
    if (result != 0x38e)
    {
    LAB_026653e3:
      return 0;
    }
  }
  else
  {
    if (result == 0x496c)
    {
    LAB_0266545b:
      if (1 < uVar4 - 0x42)
      {
        return 0;
      }
      return uVar4;
    }
    if (result < 0x496d)
    {
      if (result != 0x4951)
      {
        if (result != 0x4952)
        {
          return 0;
        }
        goto code_r0x0266544e;
      }
    }
    else
    {
      if (result == 0x49a8)
        goto LAB_0266545b;
      if (result < 0x49a9)
      {
        if ((result - 0x497dU & 0xffffffef) != 0)
        {
          return 0;
        }
      }
      else if (result != 0x500b)
      {
        return 0;
      }
    }
  }
LAB_02665409:
  if (uVar1 == 0x42)
  {
    return uVar4;
  }
  return 0;
}

/* ManalinkEh 2022: 0x2665580; original functions/token_generation.c:62. */
void copy_token_definition(int player, int card, token_generation_t *token, int t_player,
                                int t_card)
{
  special_flags2_t sVar1;
  int result;
  card_instance_t *instance;
  unsigned int uVar4;

  result = get_id(t_player, t_card);
  default_token_definition(player, card, result, token);
  token->no_sleight = '\x01';
  if (cards_ptr[result]->card_type == 8)
  {
    instance = get_card_instance(t_player, t_card);
    result = check_special_flags(t_player, t_card, 0x40000000);
    if (result == 0)
    {
      token->s_player = (int)instance->damage_source_player;
      token->s_card = instance->damage_source_card;
    }
    else
    {
      token->s_card = -1;
      token->s_player = -1;
    }
    sVar1 = instance->ext->special_flags2;
    token->s_key_plus = instance->ext->sp_keywords;
    token->special_flags2 = sVar1;
    if ((t_player < 0) || (t_card < 0))
    {
      uVar4 = 0;
    }
    else
    {
      uVar4 = get_copyable_special_infos(t_player, t_card);
    }
    token->special_infos = uVar4;
    result = check_special_flags2(t_player, t_card, 0x80);
    if (result != 0)
    {
      token->special_flags2 = 0x80;
      return;
    }
  }
  return;
}

/* ManalinkEh 2022: 0x2665820; original functions/token_generation.c:124. */
int token_characteristic_setting_effects(int player, int card, event_t event, int t_player,
                                              int t_card)
{
  special_flags2_t value;
  int result;
  card_instance_t *instance;

  if (card != -1)
  {
    result = check_special_flags2(player, card, 0x400);
    if (result == 0)
    {
      instance = get_card_instance(player, card);
      special_abilities(t_player, t_card, event, instance->token_special_keywords, t_player,
                        t_card);
      if ((((event == EVENT_CHANGE_TYPE) && (t_card == affected_card)) &&
           (t_player == affected_card_controller)) &&
          (value = instance->ext->special_flags2, 0 < value))
      {
        if ((land_can_be_played & 0x20000U) == 0)
        {
          land_can_be_played = land_can_be_played | 0x10000;
        }
        else
        {
          set_special_flags2(t_player, t_card, value);
        }
      }
    }
  }
  return 0;
}

/* ManalinkEh 2022: 0x2665900; original functions/token_generation.c:146. */
int generic_token(int player, int card, event_t event)
{
  int result;
  result = token_characteristic_setting_effects(player, card, event, player, card);
  return result;
}


/* October 2022 token ID/copy entry points. */
static int generate_token_by_id_impl(int player, int card, int csvid, int number, int reserved);
static void copy_token_part(int player, int card, int tok_player, int tok_card);
void copy_token(int player, int card, int tok_player, int tok_card);
int generate_token_by_id(int player, int card, int csvid);
void generate_tokens_by_id(int player, int card, int csvid, int howmany);
int generate_reserved_token_by_id(int player, int csvid);

/* ManalinkEh 2022: 0x2666580; original functions/token_generation.c:624. */
static int generate_token_by_id_impl(int player, int card, int csvid, int number, int reserved)
{
  int iVar1;
  int iid;
  int iVar2;
  token_generation_t token;

  default_token_definition(player, card, csvid, &token);
  if (reserved == 0)
  {
    iVar1 = -1;
    token.qty = (unsigned char)number;
    generate_token(&token);
  }
  else
  {
    token.no_sleight = '\x01';
    iVar1 = get_internal_card_id_from_csv_id(csvid);
    iid = resolve_token_iid(iVar1, &token);
    if (number < 1)
    {
      return -1;
    }
    iVar2 = 0;
    do
    {
      iVar2 = iVar2 + 1;
      iVar1 = add_card_to_hand(player, iid);
      set_token(player, iVar1, &token);
      put_into_play(player, iVar1);
    } while (number != iVar2);
  }
  return iVar1;
}

/* ManalinkEh 2022: 0x2666640; original functions/token_generation.c:614. */
static void copy_token_part(int player, int card, int tok_player, int tok_card)
{
  token_generation_t token;
  copy_token_definition(player, card, &token, tok_player, tok_card);
  generate_token(&token);
  return;
}

/* ManalinkEh 2022: 0x2666680; original functions/token_generation.c:614. */
void copy_token(int player, int card, int tok_player, int tok_card)
{
  if ((-1 < tok_player) && (-1 < tok_card))
  {
    copy_token_part(player, card, tok_player, tok_card);
    return;
  }
  return;
}

/* ManalinkEh 2022: 0x26666b0; original functions/token_generation.c:649. */
int generate_token_by_id(int player, int card, int csvid)
{
  int result;
  unsigned int reserved;

  result = get_internal_card_id_from_csv_id(csvid);
  if (cards_data[result].cc[2] == '\x03')
  {
    reserved = 1;
  }
  else
  {
    reserved = (unsigned int)(cards_data[result].type == 0x80);
  }
  result = generate_token_by_id_impl(player, card, csvid, 1, reserved);
  return result;
}

/* ManalinkEh 2022: 0x2666720; original functions/token_generation.c:653. */
void generate_tokens_by_id(int player, int card, int csvid, int howmany)
{
  int result;
  unsigned int reserved;

  result = get_internal_card_id_from_csv_id(csvid);
  if (cards_data[result].cc[2] == '\x03')
  {
    reserved = 1;
  }
  else
  {
    reserved = (unsigned int)(cards_data[result].type == 0x80);
  }
  if (0 < howmany)
  {
    generate_token_by_id_impl(player, card, csvid, howmany, reserved);
    return;
  }
  return;
}

/* ManalinkEh 2022: 0x2666790; original functions/token_generation.c:657. */
int generate_reserved_token_by_id(int player, int csvid)
{
  int result;

  result = get_internal_card_id_from_csv_id(csvid);
  if ((cards_data[result].cc[2] != '\x03') && (cards_data[result].type != 0x80))
  {
    backtrace_and_abort("functions/token_generation.c:658\nis_reserved_id(csvid)");
  }
  result = generate_token_by_id_impl(player, -1, csvid, 1, 1);
  return result;
}


#ifndef SHANDALAR
/* ManalinkEh 2022: 0x2665690; original functions/token_generation.c:87. */
int copy_token_characteristics_for_clone(int player, int card, int t_player, int t_card)
{
  uint8_t uVar1;
  int32_t iVar2;
  unsigned char bVar3;
  int iVar4;
  card_instance_t *pcVar5;
  card_instance_t *pcVar6;
  uint32_t uStack_14;

  iVar4 = get_id(t_player, t_card);
  uStack_14 = 0;
  if (cards_ptr[iVar4]->card_type == 8)
  {
    pcVar5 = get_card_instance(player, card);
    pcVar6 = get_card_instance(t_player, t_card);
    uStack_14 = pcVar6->original_internal_card_id;
    if (uStack_14 == 0xffffffff)
    {
      uStack_14 = (uint32_t)pcVar6->backup_internal_card_id;
    }
    iVar2 = pcVar6->targets[7].player;
    pcVar5->targets[7].card = pcVar6->targets[7].card;
    pcVar5->targets[7].player = iVar2;
    iVar2 = pcVar6->targets[8].card;
    pcVar5->targets[8].player = pcVar6->targets[8].player;
    pcVar5->targets[8].card = iVar2;
    uVar1 = cards_data[uStack_14].color;
    iVar4 = get_internal_card_id_from_csv_id(iVar4);
    if (uVar1 == cards_data[iVar4].color)
    {
      bVar3 = pcVar5->initial_color & 0x3e;
    }
    else
    {
      bVar3 = cards_data[uStack_14].color & 0x3e;
      pcVar5->initial_color = bVar3;
    }
    pcVar5->color = bVar3;
    pcVar5->regen_status = pcVar5->regen_status | 0x10000000;
    if ((t_player < 0) || (t_card < 0))
    {
      iVar4 = 0;
    }
    else
    {
      iVar4 = get_copyable_special_infos(t_player, t_card);
    }
    pcVar5 = get_card_instance(player, card);
    pcVar5->targets[0xe].player = iVar4;
    iVar4 = check_special_flags2(t_player, t_card, 0x80);
    uStack_14 = 1;
    if (iVar4 != 0)
    {
      uStack_14 = 1;
      set_special_flags2(player, card, 0x80);
    }
  }
  return uStack_14;
}

#endif
