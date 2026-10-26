#include "manalink.h"
static int effect_on_all_control_effects_attached_to_me(int player,int card,int this_legacy_player,int this_legacy_card,int effect);
#include <windows.h>

int suppress_legend_rule = 0;	// temporarily disables
void check_legend_rule(int player, int card, event_t event){
	if (suppress_legend_rule || player < 0 || card < 0){
		return;
	}

	if( trigger_condition == TRIGGER_COMES_INTO_PLAY && affect_me( player, card ) && reason_for_trigger_controller == player ){
		if( trigger_cause_controller == player && trigger_cause == card && ! check_battlefield_for_id(2, CARD_ID_MIRROR_GALLERY) ){
			if( event == EVENT_END_TRIGGER ){
					int id = get_card_name(player, card);
					verify_legend_rule(player, card, id);
#pragma message "'Temporary' hack so the legend's other come-into-play triggers work right, though this should really be checked in EVENT_STATIC_EFFECTS instead - http://www.slightlymagic.net/forum/viewtopic.php?t=13661"
					/* The kill_card() call in true_verify_legend_rule() overwrites at least affected_card_controller/affected_card and trigger_condition, and
					 * they don't get restored in time for other come-into-play triggers on this card to recognize them.  We know what almost all the conditions
					 * for comes_into_play() were at the start of this call, though, so restore them all. */
					if (in_play(player, card)){
						trigger_condition = TRIGGER_COMES_INTO_PLAY;
						affected_card_controller = player;
						affected_card = card;
						reason_for_trigger_controller = player;
						trigger_cause_controller = player;
						trigger_cause = card;
					} else {
						trigger_condition = -1;	// Note that this will only last until the containing function returns; resolve_trigger() restores it.
					}
					// end hack
			}
		}
	}

	if (event == EVENT_CARDCONTROLLED && affect_me(player, card)){
		int id = get_card_name(player, card);
		verify_legend_rule(player, card, id);
	}
}

int true_verify_legend_rule(int player, int card, int id){
	if( id == -1 ){ //Checking for a card with no name (a face-down card, maybe)
		return 0;
	}

	int l_count = 0, c = 0;
	for (c = 0; c < active_cards_count[player]; ++c){
		if( in_play(player, c) && !is_what(player, c, TYPE_EFFECT) ){
			if( get_card_name(player, c) == id ){
				++l_count;
			}
			else{
				state_untargettable(player, c, 1);
			}
		}
	}
	if( l_count > 1 ){
		target_definition_t td;
		default_target_definition(player, card, &td, TYPE_PERMANENT);
		td.allowed_controller = player;
		td.preferred_controller = player;
		td.illegal_abilities = 0;
		td.allow_cancel = 0;

		card_instance_t *instance = get_card_instance(player, card);

		instance->number_of_targets = 0;

		if( select_target(player, card, &td, "Legend Rule: choose a permanent to sacrifice.", &(instance->targets[0])) ){
			remove_state(player, -1, STATE_CANNOT_TARGET);	// before killing, in case it causes a trigger that targets something
			kill_card(instance->targets[0].player, instance->targets[0].card, KILL_STATE_BASED_ACTION);
		}
	}
	remove_state(player, -1, STATE_CANNOT_TARGET);
	if( l_count > 1 ){
		return 1;
	}
	return 0;
}

int verify_legend_rule(int player, int card, int id){
	if( is_legendary(player, card) && !check_battlefield_for_id(2, CARD_ID_MIRROR_GALLERY) ){
		return true_verify_legend_rule(player, card, id);
	}
	return 0;
}

extern int timestamp_card[500];
extern int timestamp_player[500];

static int real_switch_control(int player, int card)
{
// What this function do: 1-player gains control of (player, card)
  card_instance_t *v2; // esi
  int v3; // eax
  const void *v4; // ST04_4
  card_instance_t *v5; // esi
  card_instance_t *v6; // esi
  signed int i; // ecx
  card_instance_t *v8; // esi
  int v9; // ecx
  int v11; // [sp+Ch] [bp-14h]
  int v12; // [sp+10h] [bp-10h]
  int v14; // [sp+18h] [bp-8h]
  int v15; // [sp+1Ch] [bp-4h]

  v15 = 1 - player;
  v2 = get_card_instance(player, card);
  int v13 = v2->timestamp;
  v3 = add_card_to_hand_exe(1 - player, v2->internal_card_id);
  v14 = v3;
  if (v3 != -1)
    {
      v4 = v2;
      v5 = get_card_instance(v15, v3);
  /* Recovered 2022 extension fragment: gain_control_impl @ 0x2663990. */
  memcpy_inst_and_ext(v5, (const card_instance_t *)v4);
  /* End recovered 2022 extension fragment: gain_control_impl. */
      v5->state |= STATE_SUMMON_SICK;
      v5->state &= 0xFFFFFFF3u;                 // ~(STATE_ATTACKING|STATE_BLOCKING)
      timestamp_player[v13] = v15;
      timestamp_card[v13] = v14;
      v12 = -1;
      while (1)
	{
	  ++v12;
	  if (v12 >= 2)
	    break;
	  v11 = -1;
	  while (1)
	    {
	      ++v11;
	      if (active_cards_count[v12] <= v11)
		break;
	      v6 = get_card_instance(v12, v11);
	      if (v6->damage_target_player == player && v6->damage_target_card == card)
		{
		  v6->damage_target_player = v15;
		  v6->damage_target_card = v14;
		}
	      if (v6->damage_source_player == player && v6->damage_source_card == card)
		{
		  v6->damage_source_player = v15;
		  v6->damage_source_card = v14;
		}
	      if (v6->number_of_targets)
		for (i = 0; v6->number_of_targets > i; ++i)
		  {
		    if (v6->targets[i].player == player)
		      if (v6->targets[i].card == card)
			{
			  v6->targets[i].player = v15;
			  v6->targets[i].card = v14;
			}
		  }
	    }
	}
    }
  v8 = get_card_instance(player, card);
  v9 = cards_data[v8->internal_card_id].type;
  if (v9 & TYPE_CREATURE)
    ++creature_count[v15];
  if (v9 & TYPE_ARTIFACT)
    ++artifact_count[v15];
  if (v9 & TYPE_ENCHANTMENT)
    ++enchantment_count[v15];
  types_of_cards_on_bf[v15] |= v9;
  redraw_libraries();
  v8->token_status |= STATUS_CANNOT_LEAVE_PLAY | STATUS_OBLITERATED;
  kill_card_exe(player, card, KILL_EXILE);
  if (BYTE3(event_flags) & 0x10)                // EA_CONTROLLED
    dispatch_event(v15, v14, EVENT_CARDCONTROLLED);
  return v14;
}

int switch_control(int player, int card, int tar_player, int tar_card ){
	int legacy_card = create_legacy_effect_exe(player, card, LEGACY_EFFECT_ALADDIN, tar_player, tar_card );
	card_instance_t *legacy = get_card_instance(player, legacy_card);
	legacy->damage_source_player = -1;
	legacy->damage_source_card = -1;
	legacy->token_status |= 0x20;
	legacy->targets[0].player = player;
	legacy->targets[0].card = card;
	legacy->number_of_targets = 1;
	u4585f0(player, legacy_card);
	legacy->token_status |= 0x1000000;
	play_sound_effect(WAV_CONTROL);
	int result = u4b6860(legacy->damage_target_player, legacy->damage_target_card);
	legacy->damage_target_player = player;
	legacy->damage_target_card = result;
	if( check_special_flags(player, result, SF_ECHO_TO_PAY) ){
		remove_special_flags3(player, result, SF3_ECHO_PAID);
	}
	//legacy->info_slot = 333;
	return legacy_card;
}

int gain_control(int player, int card, int tar_player, int tar_card ){
	if( player == tar_player || check_for_guardian_beast_protection(tar_player, tar_card) ){
		return -1;
	}
	return switch_control(player, card, tar_player, tar_card );
}

static int generic_control_legacy(int player, int card, event_t event)
{
  enum
  {
    SUBTYPE_NOBLE = 255
  };
  int card_00;
  int t_card;
  unsigned int uVar1;
  card_instance_t *instance;
  int iVar3;
  int iVar4;
  int player_00;
  subtype_t sVar5;
  unsigned int uVar6;
  bool bVar7;

  if (player < 0)
  {
    return 0;
  }
  if (card < 0)
  {
    return 0;
  }
  instance = get_card_instance(player, card);
  if (instance->damage_target_player < '\0')
  {
    return 0;
  }
  player_00 = (int)instance->damage_target_player;
  iVar4 = instance->targets[0].player;
  card_00 = instance->damage_target_card;
  t_card = instance->targets[0].card;
  iVar3 = instance->targets[1].player;
  uVar6 = instance->targets[1].card;
  if (0 < iVar3)
  {
    special_abilities(player_00, card_00, event, iVar3, player, card);
  }
  iVar3 = instance->targets[2].player;
  if (iVar3 < 1 || event != EVENT_POWER)
  {
    iVar3 = instance->targets[2].card;
    if ((event == EVENT_TOUGHNESS) && (0 < iVar3))
    {
      bVar7 = false;
      if ((card_00 == affected_card) && (player_00 == affected_card_controller))
      {
        event_result = event_result + iVar3;
      }
    }
    else
    {
      iVar3 = instance->targets[3].player;
      bVar7 = event == EVENT_ABILITIES;
      if ((0 < iVar3 && bVar7) &&
          ((bVar7 = true, card_00 == affected_card && (player_00 == affected_card_controller))))
      {
        event_result = event_result + iVar3;
      }
    }
  }
  else
  {
    if ((card_00 == affected_card) && (player_00 == affected_card_controller))
    {
      event_result = event_result + iVar3;
    }
    bVar7 = event == EVENT_ABILITIES;
  }
  sVar5 = instance->targets[4].player;
  if (-1 < (int)sVar5)
  {
    if ((char)sVar5 != '\0')
    {
      attached_is_additional_subtype(player, card, event, sVar5 & SUBTYPE_NOBLE);
      sVar5 = instance->targets[4].player;
    }
    if ((char)(sVar5 >> 0x10) != '\0')
    {
      attached_is_forced_subtype(player, card, event, (int)sVar5 >> 0x10 & SUBTYPE_NOBLE);
    }
  }
  uVar1 = instance->targets[4].card;
  if (((0 < (int)uVar1) && ((uVar1 & 1) != 0)) && (instance->targets[3].card == 0x1ea5))
  {
    spirit_link_effect(player_00, card_00, event, instance->targets[0].player);
  }
  if (((event == EVENT_DECLARE_BLOCKERS) || (bVar7)) && (instance->targets[3].card == 0x3a6a))
  {
    fx_menace(player, card, event);
  }
  if (uVar6 == 0)
  {
    return 0;
  }
  if ((((uVar6 & 0x81) != 0) && (iVar3 = eot_trigger(player, card, event), iVar3 != 0)) &&
      (((uVar6 & 0x80) == 0 || (player == current_turn))))
  {
    iVar3 = instance->targets[3].card;
    if (iVar3 == 0x286b)
    {
      kill_card(instance->targets[0].player, instance->targets[0].card, KILL_SACRIFICE);
      return 0;
    }
    if ((iVar3 == 0x7e9) || (iVar3 == 0x81b))
    {
      tap_card(player_00, card_00);
    }
    effect_on_all_control_effects_attached_to_me(player_00, card_00, player, card, 1);
    iVar3 = check_state(player, card, 0x400000);
    if ((iVar3 != 0) && (iVar3 = check_status(player, card, 0x1000000), iVar3 != 0))
    {
      gain_control_impl(player_00, card_00);
    }
    kill_card(player, card, KILL_EXILE);
  }
  if ((uVar6 & 0x7e) == 0)
  {
    return 0;
  }
  if ((event != EVENT_CARDCONTROLLED && event != EVENT_STATIC_EFFECTS) &&
      (event != EVENT_LEAVES_PLAY_ABILITY))
  {
    if (trigger_condition != 0xd4)
    {
      return 0;
    }
    if (card != affected_card)
    {
      return 0;
    }
    if (player != affected_card_controller)
    {
      return 0;
    }
    if (instance->targets[5].player != -1)
    {
      other_leaves_play(player, card, iVar4, t_card, event, 0, 0);
      return 0;
    }
  LAB_02664118:
    instance->targets[5].player = 1;
    return 0;
  }
  if (instance->targets[5].player == -1)
    goto LAB_02664118;
  other_leaves_play(player, card, iVar4, t_card, event, 0, 0);
  if (event == EVENT_LEAVES_PLAY_ABILITY)
  {
    if ((uVar6 & 8) != 0)
    {
      kill_card(player_00, card_00, KILL_BURY);
      goto LAB_02663fa8;
    }
    uVar6 = uVar6 & 0x40;
  }
  else
  {
    if (event != EVENT_CARDCONTROLLED)
    {
      if (iVar4 < 0)
      {
        return 0;
      }
      if (event != EVENT_STATIC_EFFECTS)
      {
        return 0;
      }
      if (instance->targets[5].player != 1)
      {
        return 0;
      }
      iVar3 = is_tapped(iVar4, t_card);
      if (iVar3 == 0)
      {
        if ((uVar6 & 8) != 0)
        {
          kill_card(player_00, card_00, KILL_BURY);
        }
        if ((uVar6 & 4) == 0)
          goto LAB_02664210;
        instance->targets[5].player = -1;
        bVar7 = true;
        if ((uVar6 & 0x10) != 0)
          goto LAB_02664281;
      joined_r0x026642c0:
        if (((uVar6 & 0x20) == 0) || (iVar4 = is_enchanted(player_00, card_00), iVar4 != 0))
          goto LAB_02663fa8;
      }
      else
      {
      LAB_02664210:
        if ((uVar6 & 0x10) != 0)
        {
          bVar7 = false;
        LAB_02664281:
          iVar3 = get_power(player_00, card_00);
          iVar4 = get_power(iVar4, t_card);
          if (iVar3 <= iVar4)
          {
            if ((uVar6 & 0x20) != 0)
              goto LAB_02664220;
            goto LAB_02664250;
          }
          instance->targets[5].player = -1;
          goto joined_r0x026642c0;
        }
        bVar7 = false;
        if ((uVar6 & 0x20) == 0)
        {
          return 0;
        }
      LAB_02664220:
        iVar4 = is_enchanted(player_00, card_00);
        if (iVar4 != 0)
        {
        LAB_02664250:
          if (!bVar7)
          {
            return 0;
          }
          goto LAB_02663fa8;
        }
      }
      instance->targets[5].player = -1;
      goto LAB_02663fa8;
    }
    if (t_card != affected_card)
    {
      return 0;
    }
    if (iVar4 != affected_card_controller)
    {
      return 0;
    }
    uVar6 = uVar6 & 2;
  }
  if (uVar6 == 0)
  {
    return 0;
  }
LAB_02663fa8:
  instance = in_play(player_00, card_00);
  if (instance != NULL)
  {
    effect_on_all_control_effects_attached_to_me(player_00, card_00, player, card, 1);
    iVar4 = check_state(player, card, 0x400000);
    if ((iVar4 != 0) && (iVar4 = check_status(player, card, 0x1000000), iVar4 != 0))
    {
      gain_control_impl(player_00, card_00);
    }
  }
  kill_card(player, card, KILL_EXILE);
  return 0;
}

static int effect_on_all_control_effects_attached_to_me(int player, int card, int this_legacy_player, int this_legacy_card, int effect){
// effect = 0 --> check only
// effect = 1 -> enable the "control legacy" with the highest timestamp (AKA the most recent)
// effect = -1 -> disable all "control legacies"
	int result = 0;
	int legacies[100][3];
	int lc = 0;
	card_instance_t* inst;
	int p, c;
	for (p = 0; p <= 1; ++p){
		for (c = active_cards_count[p]; c > -1; c--){
			if ( (inst = in_play(p, c)) && inst->damage_target_player == player && inst->damage_target_card == card ){
				if( is_what(p, c, TYPE_EFFECT) && inst->info_slot == (int)generic_control_legacy ){
					//STATE_POWER_STRUGGLE = This "control legacy" has stolen a permanent from someone, so it needs to be processed.
					if( check_state(p, c, STATE_POWER_STRUGGLE) ){
						if( effect == 1 ){
							if( ! check_status(p, c, STATUS_CONTROLLED) ){
								if( this_legacy_player == -1 || 
									(this_legacy_player != -1 && this_legacy_card != -1 && !(p == this_legacy_player && c == this_legacy_card)))
								{
									legacies[lc][0] = inst->timestamp;
									legacies[lc][1] = p;
									legacies[lc][2] = c;
									lc++;
									result = 1;
								}
							}
						}
						//STATUS_CONTROLLED = This "control legacy" is active and the enchanted permanent will return to its previous controller if this legacy leaves play
						if( effect == -1 ){
							remove_status(p, c, STATUS_CONTROLLED);
							result = 1;
						}
					}
				}
			}
		}
	}
	if( lc ){
		int max_timestamp = 0;
		int id_of_legacy_to_activate = 0;
		int i;
		for(i=0; i<lc; i++){
			if( legacies[i][0] > max_timestamp ){
				max_timestamp = legacies[i][0];
			}
			id_of_legacy_to_activate = i;
		}
		add_status(legacies[id_of_legacy_to_activate][1], legacies[id_of_legacy_to_activate][2], STATUS_CONTROLLED);
	}
	return result;
}

int disable_all_control_effects_attached_to_me(int player, int card){
	return effect_on_all_control_effects_attached_to_me(player, card, -1, -1, -1);
}

int re_enable_the_most_recent_control_effect_attached_to_me(int player, int card){
	return effect_on_all_control_effects_attached_to_me(player, card, -1, -1, 1);
}

int re_enable_the_other_most_recent_control_effect_attached_to_me(int player, int card, int legacy_p, int legacy_c){
	return effect_on_all_control_effects_attached_to_me(player, card, legacy_p, legacy_c, 1);
}

int gain_control_permanently_impl(int player, int t_player, int t_card, int sound){
	int result = -1;
	disable_all_control_effects_attached_to_me(t_player, t_card);
	if( player == t_player ){
		result = t_card;
	}
	else{
		if( check_for_guardian_beast_protection(t_player, t_card) ){
			return -1;
		}
		result = real_switch_control(t_player, t_card);
		if( result > -1 ){
			if( check_special_flags(player, result, SF_ECHO_TO_PAY) ){
				remove_special_flags3(player, result, SF3_ECHO_PAID);
			}
			if( sound ){
				play_sound_effect(WAV_CONTROL);
			}
		}
	}
	return result;
}

int gain_control_permanently(int player, int t_player, int t_card){
	return gain_control_permanently_impl(player, t_player, t_card, 1);
}

int gain_control_permanently_no_sound(int player, int t_player, int t_card){
	return gain_control_permanently_impl(player, t_player, t_card, 0);
}

void exchange_control_of_target_permanents(int player, int card, int player1, int card1, int player2, int card2)
{
  if (player1 != player2)
	{
	  suppress_legend_rule = 1;

	  int new_card1 = gain_control_permanently_no_sound(player1, player2, card2);
	  int new_card2 = gain_control_permanently_no_sound(player2, player1, card1);

	  suppress_legend_rule = 0;

	  play_sound_effect(WAV_CONTROL);

	  dispatch_event(player1, new_card1, EVENT_CARDCONTROLLED);
	  dispatch_event(player2, new_card2, EVENT_CARDCONTROLLED);
	}
}

static int gain_control_until_clause(int player, int card, int t_player, int t_card, int mode, int pow, int tou, int key, int s_key){
// Mode:	see 'gcus_t' in 'defs.h'.

	int legacy_card = -1;
	int result = gain_control_permanently(player, t_player, t_card);
	if( result > -1 ){
		legacy_card = create_targetted_legacy_effect(player, card, &generic_control_legacy, player, result);
		if( legacy_card > -1 ){
			card_instance_t* legacy_instance = get_card_instance(player, legacy_card);
			if( player != t_player ){
				//STATUS_POWER_STRUGGLE = this "control legacy" has stolen a permanent from someone
				add_state(player, legacy_card, STATE_POWER_STRUGGLE);
				//STATUS_CONTROLLED = this "control legacy" is active and will return the permanent to his previous controller when leave play
				add_status(player, legacy_card, STATUS_CONTROLLED);
			}
			legacy_instance->targets[0].player = player;
			legacy_instance->targets[0].card = card;
			legacy_instance->targets[1].player = s_key;
			legacy_instance->targets[1].card = mode;
			legacy_instance->targets[2].player = pow;
			legacy_instance->targets[2].card = tou;
			legacy_instance->targets[3].player = key;
			legacy_instance->targets[3].card = get_id(player, card);
		}
	}
	return legacy_card;
}

int give_control(int effect_source_player, int effect_source_card, int tar_player, int tar_card)
{
	disable_all_control_effects_attached_to_me(tar_player, tar_card);
	int result = real_switch_control(tar_player, tar_card);
	play_sound_effect(WAV_CONTROL);	// Since removing control effects doesn't do this itself
	int legacy = create_targetted_legacy_effect(effect_source_player, effect_source_card, &empty, 1-tar_player, result);
	return legacy;
}

int give_control_until_eot(int effect_source_player, int effect_source_card, int tar_player, int tar_card)
{
	card_instance_t* instance = get_card_instance(effect_source_player, effect_source_card);

	int iid = instance->internal_card_id == -1 ? (int)instance->original_internal_card_id : instance->internal_card_id;
	int fake = add_card_to_hand(1-effect_source_player, iid);
	card_instance_t* temp = get_card_instance(1-effect_source_player, fake);
	temp->state |= STATE_INVISIBLE;
	--hand_count[1-effect_source_player];

	int legacy = gain_control_until_eot(1-effect_source_player, fake, tar_player, tar_card);

	obliterate_card(1-effect_source_player, fake);

	return legacy;
}

void give_control_of_self(int player, int card)
{
	disable_all_control_effects_attached_to_me(player, card);
	real_switch_control(player, card);
	play_sound_effect(WAV_CONTROL);	// Since removing control effects doesn't do this itself
}

int effect_act_of_treason_and_modify_pt_or_abilities(int player, int card, int t_player, int t_card, int pow, int tou, int key, int s_key){
	int legacy_card = -1;
	if (in_play(t_player, t_card)){
		untap_card(t_player, t_card);
		if( player != t_player ){
			gain_control_until_clause(player, card, t_player, t_card, 1, pow, tou, key, s_key | SP_KEYWORD_HASTE);
		}
		else{
			legacy_card = pump_ability_until_eot(player, card, t_player, t_card, pow, tou, key, s_key | SP_KEYWORD_HASTE);
		}
	}
	return legacy_card;
}

int effect_act_of_treason(int player, int card, int t_player, int t_card)
{
	return effect_act_of_treason_and_modify_pt_or_abilities(player, card, t_player, t_card, 0, 0, 0, 0);
}

int gain_control_until_eot(int player, int card, int t_player, int t_card){
	return gain_control_until_clause(player, card, t_player, t_card, 1, 0, 0, 0, 0);
}

void gain_control_until_source_is_in_play_and_tapped(int player, int card, int t_player, int t_card, gcus_t mode)
{
  if (in_play(t_player, t_card))
	{
	  int source_player, source_card;
	  card_instance_t* instance = get_card_instance(player, card);
	  if (instance->internal_card_id == activation_card)
		{
		  source_player = instance->parent_controller;
		  source_card = instance->parent_card;
		}
	  else
		{
		  source_player = player;
		  source_card = card;
		}

	  if (!in_play(player, card)
		  || ((mode & GCUS_CONTROLLED) && player != source_player)
		  || ((mode & GCUS_TAPPED) && !is_tapped(player, card)))
		return;

	  gain_control_until_clause(source_player, source_card, t_player, t_card, mode, 0, 0, 0, 0);
	}
}

void get_back_your_permanents(int player, int card, type_t typ)
{
	// First, destroy any effect cards controlling permanents stolen from player.
	int marked[151];
	int mc = 0;
	int c;
	int p = 1-player;
	for(c=0; c<active_cards_count[p]; c++){
		if( in_play(p, c) && is_what(p, c, TYPE_EFFECT) ){
			card_instance_t *inst = get_card_instance(p, c);
			if( inst->info_slot == (int)generic_control_legacy ){
				if( is_stolen(inst->damage_target_player, inst->damage_target_card) && 
					is_what(inst->damage_target_player, inst->damage_target_card, typ) )
				{
					marked[mc] = c;
					mc++;
				}
			}
		}
	}

	for(c=0; c<mc; c++){
		kill_card(p, marked[c], KILL_EXILE);
	}

	// Anything left over.
	for(c=0; c<active_cards_count[p]; c++){
		if( in_play(p, c) && is_what(p, c, typ) ){
			if( is_stolen(p, c) ){
				gain_control_permanently(player, p, c);
			}
		}
	}
}

/* Recovered October 2022 implementations; see docs/recovery-progress.md. */

/* ManalinkEh 2022: 0x2664aa0; original functions/gain_control_and_legend_rule.c:577. */
int gain_control_until_eot_haste(int player, int card, int t_player, int t_card)
{
  int result;

  result = gain_control_until_clause(player, card, t_player, t_card, 1, 0, 0, 0, 4);
  return result;
}

/* ManalinkEh 2022: 0x2664c10; original functions/gain_control_and_legend_rule.c:611. */
int gain_control_until_eoyt(int player, int card, int t_player, int t_card)
{
  int result;

  result = gain_control_until_clause(player, card, t_player, t_card, 0x80, 0, 0, 0, 0);
  return result;
}


#ifndef SHANDALAR
/* Recovered October 2022 global tutoring and control transfer. */

/* ManalinkEh 2022: 0x2663990; original functions/gain_control_and_legend_rule.c:133. */
int gain_control_impl(int player, int card)
{
  card_instance_t *src = get_card_instance(player, card);
  uint32_t old_timestamp = src->timestamp;
  int new_player = 1 - player;
  int new_card = add_card_to_hand_exe(new_player, src->internal_card_id);
  if (new_card != -1)
  {
    card_instance_t *dest = get_card_instance(new_player, new_card);
    memcpy_inst_and_ext(dest, src);
    dest->state = (dest->state & 0xfffffff3U) | 0x30000U;
    timestamp_card[old_timestamp] = new_card;
    timestamp_player[old_timestamp] = new_player;
    for (int p = 0; p < 2; ++p)
      for (int c = 0; c < active_cards_count[p]; ++c)
      {
        card_instance_t *instance = get_card_instance(p, c);
        if (instance->damage_target_player == player && instance->damage_target_card == card)
        {
          instance->damage_target_player = new_player;
          instance->damage_target_card = new_card;
        }
        if (instance->damage_source_player == player && instance->damage_source_card == card)
        {
          instance->damage_source_player = new_player;
          instance->damage_source_card = new_card;
        }
        unsigned int count = instance->number_of_targets;
        for (unsigned int i = 0; i < count; ++i)
          if (instance->targets[i].player == player && instance->targets[i].card == card)
          {
            instance->targets[i].player = new_player;
            instance->targets[i].card = new_card;
          }
      }
  }
  uint8_t type = cards_data[src->internal_card_id].type;
  if ((type & 2) != 0)
    ++creature_count[new_player];
  if ((type & 0x40) != 0)
    ++artifact_count[new_player];
  if ((type & 4) != 0)
    ++enchantment_count[new_player];
  types_of_cards_on_bf[new_player] |= type;
  redraw_libraries();
  src->token_status |= 0x40000008U;
  kill_card_exe(player, card, KILL_EXILE);
  if (((uint32_t)event_flags & 0x10000000U) != 0)
    dispatch_event(new_player, new_card, (event_t)0x94);
  return new_card;
}

#endif
