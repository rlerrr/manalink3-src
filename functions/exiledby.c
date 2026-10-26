#include <ctype.h>
#include <limits.h>
#include "manalink.h"

// Functions to remember which cards are exiled by which.

// empty effect, used only for data storage and comparison purposes
static int exiledby_legacy(int player, int card, event_t event)
{
  /* Targets:
   * [0] through [17]: each of .player and .card are an iid of an exiled card.  0x80000000 is set if the card is owned by player 1.
   * [18].player: unused
   * [18].card: stores identifying id if this is detached - see exiledby_detach() */
  return 0;
}

static int exiledby_legacymatches(card_instance_t* instance, int player, int card)
{
  if (instance->internal_card_id != LEGACY_EFFECT_CUSTOM)
	return 0;

  if (instance->info_slot != (int)exiledby_legacy)
	return 0;

  if ((instance->state & (STATE_OUBLIETTED | STATE_INVISIBLE | STATE_IN_PLAY)) != STATE_IN_PLAY)	// not just in_play(player, card), since player may be -1 or -2
	return 0;

  if (player < 0)
	return instance->targets[18].card == card;
  else
	return instance->damage_target_player == player && instance->damage_target_card == card;
}

/* player/card: The card that exiled the cards to search for.
 * csvid: Csvid to display in the dialog.  Typically the same as player/card's; but must be provided since player/card may have been obliterated.
 * mode, testarg: Action and constraints per exiledby_choose_mode_t, documented in manalink.h.
 * cardtype_txt: Descriptor of cards exiled (e.g. "creature"), or NULL if arbitrary cards.
 * faceup: Whether the cards are exiled face-up (and thus should exist in rfg_ptr[][]) or face-down (and thus should not yet). */
int exiledby_choose(int player, int card, int csvid, exiledby_choose_mode_t mode, int testarg, const char* cardtype_txt, int faceup)
{
  int iids[1000];
  int* locs[1000];
  int num_found = 0;

  int tests = mode & ~EXBY_MODE_MASK;
  mode &= EXBY_MODE_MASK;

  if (mode != EXBY_MAX_VALUE && mode != EXBY_FIRST_FOUND)
	{
	  memset(iids, -1, 1000 * sizeof(int));
	  memset(locs, 0, 1000 * sizeof(int*));
	}

  int p = player;
  if (player < 0)
	player += 2;

  card_instance_t* instance;
  int i, leg, field, ai_choice = -1, highest_value = INT_MIN, can_cast = 0;
  /* We look at all cards each call, so if a card's no longer exiled, we remove it immediately.  Not quite as good as we'd like - if you exile two Atogs, then
   * Pull From Reality one, it'll still be listed twice. */
  for (leg = 0; leg < active_cards_count[player]; ++leg)
	{
	  instance = get_card_instance(player, leg);
	  if (exiledby_legacymatches(instance, p, card))
		for (i = 0; i < 18; ++i)
		  for (field = 0; field < 2; ++field)
			{
			  int* loc = field == 0 ? &instance->targets[i].player : &instance->targets[i].card;

			  if (*loc != -1)
				{
				  int plr = (*loc & 0x80000000) ? 1 : 0;
				  int iid = *loc & ~0x80000000;
				  if (faceup && !check_rfg(plr, cards_data[iid].id))
					*loc = -1;
				  else
					{
					  // > 0: pass test.  == 0: pass test, but just barely.  < 0: fail.
					  int test_cmc = (tests & EXBY_TEST_CMC_LE) ? testarg - get_cmc_by_id(cards_data[iid].id) : 99;
					  if (test_cmc < 0)
						continue;

					  int test_iid = ((tests & EXBY_TEST_IID) && !(iid == testarg || cards_data[iid].id == csvid)) ? -1 : 99;
					  if (test_iid < 0)
						continue;

					  int test_can_cast = 99;
					  if (tests & EXBY_TEST_CAN_CAST)
						{
							if( testarg )
								test_can_cast = can_legally_play_iid_now(player, iid, testarg);
							else
								test_can_cast = can_legally_play_iid(player, iid);
						  if (!test_can_cast)
							continue;
						  else if (test_can_cast != 99)
							test_can_cast = 1;
						}

					  int test_has_mana_to_cast = ((tests & EXBY_TEST_HAS_MANA_TO_CAST) && !has_mana_to_cast_iid(player, testarg, iid)) ? -1 : 99;
					  if( tests & EXBY_TEST_HAS_GENERIC_MANA_TO_PAY_CMC ){
						  int cmc = get_cmc_by_internal_id(iid);
						  test_has_mana_to_cast = has_mana_for_activated_ability(player, card, MANACOST_X(cmc)) ? 99 : -1;
					  }
					  if (test_has_mana_to_cast < 0)
						continue;

					  int pass = MIN(test_cmc, test_iid);
					  pass = MIN(pass, test_can_cast);

					  if (pass < 0)
						continue;

					  if (mode == EXBY_FIRST_FOUND)
						return (int)loc;

					  if (mode == EXBY_CAN_CAST)
						{
						  if (test_can_cast == 99)
							return 99;
						  can_cast = 1;
						}

					  if (mode == EXBY_MAX_VALUE || mode == EXBY_CHOOSE)
						{
						  int val = my_base_value_by_id(iid);
						  if (pass == 0)
							val /= 2;
						  if (val > highest_value)
							{
							  highest_value = val;
							  ai_choice = num_found;
							}
						}

					  if (mode == EXBY_CHOOSE)
						{
						  iids[num_found] = iid;
						  locs[num_found] = loc;
						  ++num_found;
						}
					}
				}
			}
	}

  if (mode == EXBY_MAX_VALUE)
	return highest_value;

  if (mode == EXBY_FIRST_FOUND)
	return 0;

  if (mode == EXBY_CAN_CAST)
	return can_cast;

  if (IS_AI(player))
	{
	  if (ai_choice == -1)
		return 0;
	  else
		return (int)(locs[ai_choice]);
	}

  char buf[200];
  if (num_found == 0)
	{
	  sprintf(buf, "No %s%scards have been exiled by %s.", cardtype_txt ? cardtype_txt : "", cardtype_txt ? " " : "", cards_ptr[csvid]->name);
	  DIALOG(player, card, EVENT_ACTIVATE,
			 DLG_FULLCARD_CSVID(csvid),
			 DLG_MSG(buf));
	  return 0;
	}

  sprintf(buf, "%s%scards exiled by %s", cardtype_txt ? cardtype_txt : "", cardtype_txt ? " " : "", cards_ptr[csvid]->name);
  buf[0] = toupper(buf[0]);
  int selected = show_deck(player, iids, num_found, buf, 0, 0x7375B0);

  if (selected == -1){
	  return 0;
  } else {
	  return (int)(locs[selected]);
  }
}

// Find the first card exiled by player/card starting at ret_leg/ret_idx.
int* exiledby_find_any(int player, int card, int* ret_leg, int* ret_idx)
{
  int leg = ret_leg ? *ret_leg : 0, idx = ret_idx ? *ret_idx : 0, *loc = NULL;
  card_instance_t* instance;

  int p = player;
  if (player < 0)
	player += 2;

  for (; leg < active_cards_count[player]; ++leg)
	{
	  instance = get_card_instance(player, leg);
	  if (exiledby_legacymatches(instance, p, card))
		for (; idx < 18; ++idx)
		  {
			if (instance->targets[idx].player != -1)
			  {
				loc = &instance->targets[idx].player;
				goto break_outer;
			  }
			if (instance->targets[idx].card != -1)
			  {
				loc = &instance->targets[idx].card;
				goto break_outer;
			  }
		  }
	  idx = 0;
	}
 break_outer:

  if (!loc)	// i.e., not found
	leg = idx = 0;

  // Remember last-searched position, so we start here again in subsequent searches this activation
  if (ret_leg)
	*ret_leg = leg;

  if (ret_idx)
	*ret_idx = idx;

  return loc;
}

// Find the first card exiled by player/card starting at ret_leg/ret_idx with stored value needle.
int* exiledby_find(int player, int card, int needle, int* ret_leg, int* ret_idx)
{
  int leg = ret_leg ? *ret_leg : 0, idx = ret_idx ? *ret_idx : 0, *loc = NULL;
  card_instance_t* instance;

  int p = player;
  if (player < 0)
	player += 2;

  for (; leg < active_cards_count[player]; ++leg)
	{
	  instance = get_card_instance(player, leg);
	  if (exiledby_legacymatches(instance, p, card))
		for (; idx < 18; ++idx)
		  {
			if (instance->targets[idx].player == needle)
			  {
				loc = &instance->targets[idx].player;
				goto break_outer;
			  }
			if (instance->targets[idx].card == needle)
			  {
				loc = &instance->targets[idx].card;
				goto break_outer;
			  }
		  }
	  idx = 0;
	}
 break_outer:

  if (!loc)	// i.e., not found
	leg = idx = 0;

  // Remember last-searched position, so we start here again in subsequent searches this activation
  if (ret_leg)
	*ret_leg = leg;

  if (ret_idx)
	*ret_idx = idx;

  return loc;
}

// Returns total number of cards exiled by player/card owned by owned_by.
int exiledby_count(int player, int card, int owned_by)
{
  int leg = 0;
  int idx = 0;
  int count = 0;
  int* loc;

  int p = player;
  if (player < 0)
	player += 2;

  while ((loc = exiledby_find_any(p, card, &leg, &idx)))
	{
	  card_instance_t* instance = get_card_instance(player, leg);
	  if (owned_by == 0 ? !(*loc & 0x80000000)
		  : owned_by == 1 ? (*loc & 0x80000000)
		  : 1)
		++count;

	  if (loc == &instance->targets[idx].player
		  && instance->targets[idx].card != -1)
		{
		  loc = &instance->targets[idx].card;
		  if (owned_by == 0 ? !(*loc & 0x80000000)
			  : owned_by == 1 ? (*loc & 0x80000000)
			  : 1)
			++count;
		}

	  ++idx;
	}
  return count;
}

// Remembers that a card with iid owned by t_player was exiled.  This doesn't actually exiled the card; if the exiled card is to be face-up, the caller should do so.
void exiledby_remember(int player, int card, int t_player, int iid, int* ret_leg, int* ret_idx)
{
  if (iid == -1)
	return;

  ASSERT(player >= 0 && "exiledby_remember() cannot be called on detached effects");

  int* loc = exiledby_find(player, card, -1, ret_leg, ret_idx);

  if (!loc)
	{
	  int leg = create_targetted_legacy_effect(player, card, exiledby_legacy, player, card);
	  card_instance_t* instance = get_card_instance(player, leg);
	  instance->targets[0].player = instance->targets[0].card = -1;
	  instance->token_status |= STATUS_INVISIBLE_FX;
	  if (ret_leg)
		*ret_leg = leg;
	  if (ret_idx)
		*ret_idx = 0;
	  loc = &instance->targets[0].player;
	}

  ASSERT(!(iid & 0x80000000));
  if (t_player == 1)
	{
	  iid |= 0x80000000;
	  ASSERT(iid != -1);
	}
  *loc = iid;
}

/* Normally, these legacies are attached to the card that exiled them.  This is problematic when that permanent leaves the battlefield: all the attached effects
 * will be obliterated.  This function turns the effect cards into freestanding legacies, determines a unique id, and stores that id in their targets[18].card
 * so they can be distinguished from each other.  Returns the id.  Other functions in this package - except for exiledby_remember() - can then be called with
 * player=(player - 2), card=id to examine the legacies.  Caller will have to call exiledby_destroy_detached() later with that id, or else the legacies stay in place
 * permanently. */
int exiledby_detach(int player, int card)
{
  card_instance_t* instance;
  int leg, detach_id = 0;

  // First, find an unused detach_id.
  for (leg = 0; leg < active_cards_count[player]; ++leg)
	{
	  instance = get_card_instance(player, leg);
	  if (instance->internal_card_id == LEGACY_EFFECT_CUSTOM
		  && instance->info_slot == (int)exiledby_legacy
		  && instance->targets[18].card > detach_id)
		detach_id = instance->targets[18].card;
	}

  ++detach_id;

  // Now detach all exiledby legacies attached to player/card and set their detach_id.
  for (leg = 0; leg < active_cards_count[player]; ++leg)
	{
	  instance = get_card_instance(player, leg);
	  if (exiledby_legacymatches(instance, player, card))
		{
		  instance->damage_target_player = instance->damage_target_card = -1;
		  instance->targets[18].card = detach_id;
		}
	}

  return detach_id;
}

// Destroys all detached exiledby legacies set to detach_id.
void exiledby_destroy_detached(int player, int detach_id)
{
  int leg;
  for (leg = 0; leg < active_cards_count[player]; ++leg)
	if (exiledby_legacymatches(get_card_instance(player, leg), player - 2, detach_id))
	  kill_card(player, leg, KILL_EXILE);
}

void exile_card_and_remember_it_on_exiledby(int player, int card, int t_player, int t_card){
	int owner = get_owner(t_player, t_card);
	int iid = get_card_instance(t_player, t_card)->original_internal_card_id;
	kill_card(t_player, t_card, KILL_EXILE);
	exiledby_remember(player, card, owner, iid, NULL, NULL);
}

// empty effect, used only for data storage and comparison purposes
static int store_counters_legacy(int player, int card, event_t event)
{
  /* Targets:
   * [17].card: stores a "card" value used to identify a specific legacy of this type */
  return 0;
}

static void return_auras_and_counters_to_play(int player, int card)
{
  int player_00;
  unsigned int uVar1;
  card_instance_t *pcVar2;
  int result;
  card_instance_t *pcVar4;
  target_t *ptVar5;
  target_t *ptVar6;
  int32_t iStack_18;
  int8_t iStack_14;

  pcVar2 = get_card_instance(player, card);
  player_00 = pcVar2->targets[0].player;
  uVar1 = pcVar2->targets[0x12].player;
  result = check_rfg(player_00, (unsigned int)cards_data[pcVar2->targets[0].card].id);
  iStack_18 = -1;
  if (result != 0)
  {
    iStack_18 = add_card_to_hand(player_00, pcVar2->targets[0].card);
    remove_card_from_rfg(player_00, (unsigned int)cards_data[pcVar2->targets[0].card].id);
    if ((int)uVar1 < 1)
    {
      put_into_play(player_00, iStack_18);
      return;
    }
    if ((uVar1 & 0x10) != 0)
    {
      add_state(player_00, iStack_18, 0x10);
    }
    put_into_play(player_00, iStack_18);
    if (((-1 < iStack_18) && ((uVar1 & 0x2a) != 0)) && (0 < active_cards_count[player]))
    {
      result = 0;
      do
      {
        pcVar4 = in_play(player, result);
        if (((pcVar4 != NULL) && (pcVar4->internal_card_id == LEGACY_EFFECT_CUSTOM)) &&
            (((CardFunction)pcVar4->info_slot == store_counters_legacy &&
              (pcVar4->targets[0x11].card == pcVar2->targets[0x12].card))))
        {
          copy_counters(player, card, player_00, iStack_18, player, result, 1);
          kill_card(player, result, KILL_EXILE);
          break;
        }
        result = result + 1;
      } while (result < active_cards_count[player]);
    }
  }
  if ((uVar1 & 0x25) != 0)
  {
    ptVar5 = pcVar2->targets + 1;
    do
    {
      while ((ptVar5->player != -1 &&
              (result = check_rfg(ptVar5->player, (unsigned int)cards_data[ptVar5->card].id),
               result != 0)))
      {
        ptVar6 = ptVar5 + 1;
        result = add_card_to_hand(ptVar5->player, ptVar5->card);
        remove_card_from_rfg(ptVar5->player, (unsigned int)cards_data[ptVar5->card].id);
        pcVar4 = get_card_instance(ptVar5->player, result);
        iStack_14 = (int8_t)player_00;
        pcVar4->damage_target_player = iStack_14;
        pcVar4->damage_target_card = iStack_18;
        set_special_flags(ptVar5->player, result, 0x20000000);
        put_into_play(ptVar5->player, result);
        ptVar5 = ptVar6;
        if (ptVar6 == pcVar2->targets + 0x12)
        {
          return;
        }
      }
      ptVar5 = ptVar5 + 1;
    } while (ptVar5 != pcVar2->targets + 0x12);
  }
  return;
}

static int return_auras_counters_on_eot(int player, int card, event_t event){
	if( eot_trigger(player, card, event) ){
		return_auras_and_counters_to_play(player, card);
		kill_card(player, card, KILL_EXILE);
	}
	return 0;
}

static int return_auras_counters_if_source_leaves_play(int player, int card, event_t event){
	card_instance_t *instance = get_card_instance(player, card);
	if (event == EVENT_SET_LEGACY_EFFECT_NAME && instance->targets[0].card > -1 ){
		scnprintf(set_legacy_effect_name_addr, 51, "%s", cards_ptr[cards_data[instance->targets[0].card].id]->name);
	}
	if( trigger_condition == TRIGGER_LEAVE_PLAY ){
		if( affect_me( player, card) && trigger_cause == instance->damage_target_card && trigger_cause_controller == instance->damage_target_player
			&& reason_for_trigger_controller == player )
		{
			if(event == EVENT_TRIGGER){
				event_result |= RESOLVE_TRIGGER_MANDATORY;
			}
			else if(event == EVENT_RESOLVE_TRIGGER){
					return_auras_and_counters_to_play(player, card);
					kill_card(player, card, KILL_EXILE);
			}
		}
	}
	return 0;
}

static int tawnoss_coffin_legacy(int player, int card, event_t event){
	card_instance_t *instance = get_card_instance(player, card);
	if (event == EVENT_SET_LEGACY_EFFECT_NAME && instance->targets[0].card > -1 ){
		scnprintf(set_legacy_effect_name_addr, 51, "%s", cards_ptr[cards_data[instance->targets[0].card].id]->name);
	}
	if( instance->damage_target_player > -1 ){
		player_bits[player] |= PB_SEND_EVENT_UNTAP_CARD_TO_ALL;
		if( leaves_play(instance->damage_target_player, instance->damage_target_card, event, 0, 0) ||
			(event == EVENT_UNTAP_CARD && affect_me(instance->damage_target_player, instance->damage_target_card))
		  ){
			return_auras_and_counters_to_play(player, card);
			kill_card(player, card, KILL_EXILE);
		}
	}
	return 0;
}

static void store_permanent_on_legacy(card_instance_t* leg, int to_copy_player, int to_copy_card, int targ_min, int targ_max){
	targ_max = targ_max > 19 ? 19 : targ_max;
	int k;
	for(k=targ_min; k<targ_max; k++){
		if( leg->targets[k].player == -1 ){
			leg->targets[k].player = get_owner(to_copy_player, to_copy_card);
			leg->targets[k].card = get_original_internal_card_id(to_copy_player, to_copy_card);
			break;
		}
	}
}

int exile_permanent_and_auras_attached(int player, int card, int t_player, int t_card,
                                       unsigned int mode)
{
  unsigned int uVar1;
  int iVar2;
  int iVar3;
  card_instance_t *pcVar4;
  int iVar5;
  int iVar6;
  CardFunction func_ptr;
  card_instance_t *pcStack_18;
  int iStack_14;

  iVar2 = is_token(t_player, t_card);
  if (iVar2 == 0)
  {
    iVar2 = get_original_internal_card_id(t_player, t_card);
    iVar2 = is_what(-1, iVar2, 0x47);
    if (iVar2 != 0)
    {
      iVar2 = 0;
      if ((mode & 1) == 0)
      {
        if ((mode & 4) != 0)
        {
          func_ptr = return_auras_counters_if_source_leaves_play;
        LAB_026d728e:
          iStack_14 = create_targetted_legacy_effect(player, card, func_ptr, player, card);
          pcVar4 = get_card_instance(player, iStack_14);
          pcVar4->eot_toughness = 0x100;
          goto LAB_026d72b9;
        }
        if ((mode & 0x20) != 0)
        {
          func_ptr = tawnoss_coffin_legacy;
          goto LAB_026d728e;
        }
        iStack_14 = -1;
        pcStack_18 = NULL;
      }
      else
      {
        iVar6 = create_legacy_effect(player, card, return_auras_counters_on_eot);
        iVar3 = get_id(t_player, t_card);
        iStack_14 = set_legacy_image(player, iVar3, iVar6);
      LAB_026d72b9:
        pcStack_18 = NULL;
        if (iStack_14 != -1)
        {
          pcStack_18 = get_card_instance(player, iStack_14);
          pcStack_18->targets[0x12].player = mode;
          pcStack_18->targets[0x12].card = iStack_14;
        }
      }
      do
      {
        iVar6 = active_cards_count[iVar2] + -1;
        if (-1 < active_cards_count[iVar2] + -1)
        {
          do
          {
            while ((((pcVar4 = in_play(iVar2, iVar6),
                      pcVar4 == NULL || (iVar3 = is_what(iVar2, iVar6, 4), iVar3 == 0)) ||
                     (iVar3 = has_subtype(iVar2, iVar6, SUBTYPE_AURA), iVar3 == 0)) ||
                    ((pcVar4->damage_target_player != t_player ||
                      (pcVar4->damage_target_card != t_card)))))
            {
              iVar6 = iVar6 + -1;
              if (iVar6 == -1)
                goto LAB_026d7200;
            }
            if ((iStack_14 != -1) && (iVar3 = is_token(iVar2, iVar6), iVar3 == 0))
            {
              iVar3 = 1;
              do
              {
                if (pcStack_18->targets[iVar3].player == -1)
                {
                  iVar5 = get_owner(iVar2, iVar6);
                  pcStack_18->targets[iVar3].player = iVar5;
                  iVar5 = get_original_internal_card_id(iVar2, iVar6);
                  pcStack_18->targets[iVar3].card = iVar5;
                  break;
                }
                iVar3 = iVar3 + 1;
              } while (iVar3 != 0x12);
            }
            iVar3 = iVar6 + -1;
            kill_card(iVar2, iVar6, KILL_EXILE);
            iVar6 = iVar3;
          } while (iVar3 != -1);
        }
      LAB_026d7200:
        iVar2 = iVar2 + 1;
      } while (iVar2 != 2);
      if ((mode & 0x2a) != 0)
      {
        iVar2 = create_legacy_effect(player, card, (CardFunction)&store_counters_legacy);
        pcVar4 = get_card_instance(player, iVar2);
        uVar1 = pcVar4->token_status;
        pcVar4->targets[0x11].card = iStack_14;
        pcVar4->token_status = uVar1 | 0x10000;
        copy_counters(player, card, player, iVar2, t_player, t_card, 1);
      }
      if (pcStack_18 == NULL)
      {
        iStack_14 = -1;
      }
      else
      {
        iVar2 = get_owner(t_player, t_card);
        pcStack_18->targets[0].player = iVar2;
        pcVar4 = get_card_instance(t_player, t_card);
        pcStack_18->targets[0].card = pcVar4->original_internal_card_id;
      }
      goto LAB_026d724c;
    }
  }
  manipulate_auras_enchanting_target(player, card, t_player, t_card, NULL, ACT_KILL_REMOVE);
  iStack_14 = -1;
LAB_026d724c:
  kill_card(t_player, t_card, KILL_EXILE);
  return iStack_14;
}

void get_back_cards_stored_in_exiledby(int player, int exby_player, int exby_card, get_back_cards_stored_in_exiledby_mode flags){
	int leg;
	int idx;
	int* loc;
	while( (loc = exiledby_find_any(exby_player, exby_card, &leg, &idx)) != NULL ){
			int owner = (*loc & 0x80000000) ? 1 : 0;
			int iid = *loc & ~0x80000000;
			*loc = -1;
			int who_gets_the_card = (flags & GBC_PLAYER_GET_ALL_CARDS) ? player : owner;
			int card_added = add_card_to_hand(who_gets_the_card, iid);
			if( owner != player ){
				if( player == AI ){
					remove_state(who_gets_the_card, card_added, STATE_OWNED_BY_OPPONENT);
				}
				else{
					add_state(who_gets_the_card, card_added, STATE_OWNED_BY_OPPONENT);
				}
			}
			if( flags & GBC_PUT_PERMANENTS_INTO_BF ){
				put_into_play(who_gets_the_card, card_added);
			}
	}
}

/* Recovered October 2022 implementations; see docs/recovery-progress.md. */

/* ManalinkEh 2022: 0x26d6d50; original functions/exiledby.c:309. */
int exiledby_count_cmcs(int player, int card, int owned_by)
{
  target_t *ptVar1;
  int *piVar2;
  card_instance_t *instance;
  unsigned char bVar4;
  int result;
  int iStack_70;
  int leg;
  int idx;
  int cmcs[21];

  leg = 0;
  piVar2 = cmcs;

  for (result = 0x15; result != 0; result = result + -1)
  {
    *piVar2 = 0;
    piVar2 = piVar2 + 1;
  }
  idx = 0;
  iStack_70 = player;
  if (player < 0)
  {
    iStack_70 = player + 2;
  }
  do
  {
    piVar2 = exiledby_find_any(player, card, &leg, &idx);
    if (piVar2 == NULL)
    {
      piVar2 = cmcs;
      result = 0;
      do
      {
        if (*piVar2 == 1)
        {
          result = result + 1;
        }
        piVar2 = piVar2 + 1;
      } while (cmcs + 0x14 != piVar2);
      return result;
    }
    instance = get_card_instance(iStack_70, leg);
    bVar4 = (unsigned char)((unsigned int)*piVar2 >> 0x18);
    if (owned_by == 0)
    {
      bVar4 = bVar4 ^ 0xff;
    LAB_026d6d90:
      if ((char)bVar4 < '\0')
        goto LAB_026d6def;
      ptVar1 = instance->targets + idx;
    }
    else
    {
      if (owned_by == 1)
        goto LAB_026d6d90;
    LAB_026d6def:
      result = get_cmc_by_id((unsigned int)cards_data[*piVar2].id);
      cmcs[result] = 1;
      ptVar1 = instance->targets + idx;
    }
    if (((target_t *)piVar2 == ptVar1) && (result = instance->targets[idx].card, result != -1))
    {
      bVar4 = (unsigned char)((unsigned int)result >> 0x18);
      if (owned_by == 0)
      {
        bVar4 = (bVar4 ^ 0xff) >> 7;
      }
      else
      {
        bVar4 = owned_by != 1 | bVar4 >> 7;
      }
      if (bVar4 != 0)
      {
        result = get_cmc_by_id((unsigned int)cards_data[result].id);
        cmcs[result] = 1;
      }
    }
    idx = idx + 1;
  } while (true);
}
