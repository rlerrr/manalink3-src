#include "manalink.h"

/* Canonical recovered helper declarations. */
void human_autotap_for_mana(int player, int *amt, int *unknown_v46, int unknown_v47);
int remember_ai_value(int player, int value);
int recorded_rand(int player, int upperbound);
int check_timer_for_ai_speculation(void);
int ai_decision_phase(int player, int *phase_code_to_go_to,
                           int *becomes_second_arg_of_main_phase_and_discard_phase,
                           int *becomes_third_arg_of_main_phase);
void backup_data_for_ai(void);
void restore_data_for_ai(void);
void backup_data_for_ai_0(void);
void restore_data_for_ai_0(void);
int save_or_load_supplement(int status);
static int check_destroys_if_blocked_oneside(int lethal_player, int lethal_card, int other_player,
                                           int other_card);
int check_destroys_if_blocked(int attack_player, int attack_card, int defend_player,
                                   int defend_card);

/* October 2022 per-player card and counterability flag storage. */
static uint8_t backup_card_flags[8];
static uint8_t backup_your_spells_cant_be_countered_by_color[2];
static uint8_t backup0_card_flags[8];
static uint8_t backup0_your_spells_cant_be_countered_by_color[2];

/* October 2022 mana modifier table storage. */
static uint8_t backup_mana_colors_added[8];
static uint8_t backup_mana_color_override[64];
static uint8_t backup_mana_quantity_override[64];
static uint8_t backup_multi_mana_transmutation[16];
static uint8_t backup0_mana_colors_added[8];
static uint8_t backup0_mana_color_override[64];
static uint8_t backup0_mana_quantity_override[64];
static uint8_t backup0_multi_mana_transmutation[16];

/* October 2022 extension storage; fixed banks and original template. */
static char backup_card_ext[57600];
static char backup0_card_ext[57600];

/* October 2022 dice modifier cache storage. */
static uint8_t backup_add_to_die_rolls[8];
static uint8_t backup_roll_extra_dice[8];
static uint8_t backup0_add_to_die_rolls[8];
static uint8_t backup0_roll_extra_dice[8];

/* October 2022 private subtype-table snapshots. */
static uint8_t backup_added_subtypes_not_in_play[256];
static uint8_t backup0_added_subtypes_not_in_play[256];

/* October 2022 private AI snapshots of the 14-byte card-count cache. */
static uint8_t backup_card_counts[2][7];
static uint8_t backup0_card_counts[2][7];

int get_usertime_of_current_thread_in_ms(void);	// exe
extern int start_usertime_of_current_thread_in_ms;



int pay_mana_maximally_satisfied(int* pay_mana_array, int x_val, int max_x_val);
int try_to_pay_for_mana_by_autotapping(int player, int* amt, int* unknown_v46, int autotap_flags, int unknown_v47);



void memcpy_suppress_size_warning(void* dest, const void* src, size_t sz);

/* Copies the following data (all int or arrays of int unless noted):
0x4ef170			must_attack
0x4ef174			TENTATIVE_types_of_cards_in_graveyard[2]
0x4ef17c			types_of_cards_on_bf[2]
0x4ef184			current_phase
0x4ef188			event_flags
0x4ef18c			time_walk_flag
0x4ef190			land_can_be_palyed
0x4ef194			cancel
0x4ef198			max_x_value
0x4ef19c			card_on_stack_iid
0x4ef1a0			x_value
0x4ef1a4			player_bits[]
0x4ef1ac			dword_4EF1AC
0x4ef1b0			dword_4EF1B0
0x4ef1b4			dword_4EF1B4
0x4ef1b8			dword_4EF1B8
0x4ef1bc			dword_4EF1BC[2]
0x4ef1c4			hand_count[2]
0x4ef1cc			creature_count[2]
0x4ef1d4			artifact_count[2]
0x4ef1dc			enchantment_count[]
0x4ef1e4			(unreferenced)
0x4ef1e8			(unreferenced)
0x4ef1ec			damage_taken_this_turn[2]
0x4ef1f4			cards_drawn_this_turn[2]	// array of 2 uint16_t
0x4ef1f8			creatures_dead_this_turn
0x4ef1fc			lands_played
0x4ef200..0x4ef20d	char mana_doesnt_drain_from_pool[2][7]	// array of 14 uint8_t - cleared at the start of each turn in sub_435D20
0x4ef20e			number_of_attackers_declared			// int16_t - cleared at the start of each turn in sub_435D20
0x4ef210..0x4ef21b	(unreferenced, except to clear it at the start of each turn in sub_435D20)
0x4ef21c			raw_mana_available_hex[2][51]
0x4ef3b4..0x4ef3bf	(unreferenced)
0x4ef3c0			raw_mana_available[2][8]
0x4ef400			pay_mana_xbugrwaU[8]
0x4ef420..0x4ef45f	(unreferenced)
0x4ef460			basiclandtypes_controlled[2][8]
0x4ef4a0			landsofcolor_controlled[2][8]
0x4ef4e0			mana_pool[2][8]
0x4ef520			active_cards_count[2]
0x4ef528			life[2]
0x4ef530			player_counters[2]
0x4ef538			liched[2]
0x4ef540			card_instances[2][151]		// array of 302 card_instance_t
0x505728			ante_cards[2][16]
0x5057a8			stack_size
0x5057ac			(unreferenced)
0x5057b0			graveyard[2][500]
0x506750			library[2][500]
0x5076f0			TENTATIVE_timestamp_card[500]
0x507ec0			TENTATIVE_timestamp_player[500]
0x508690			exile[2][500]
0x509630			stack_data[32]
0x5096b0			stack_cards[32]				// array of 32 target_t
0x5097b0			stack_damage_targets[32]	// array of 32 target_t */
// All that needs to be done to backup, save, and load additional data is to add it to the following macro.  The copy is shallow, and assumes sizeof(data) is correct.
extern int mulligans_complete[2];
extern int challenge_round, challenge1, challenge2, challenge3, challenge4;
#define BACKUP_AND_SAVE_LIST				\
  BACKUP_ARRAY(graveyard_source);			\
  BACKUP_SCALAR(next_graveyard_source_id);	\
  BACKUP_SCALAR(turn_count);				\
  BACKUP_SCALAR(always_prompt_for_color);	\
  BACKUP_ARRAY(mulligans_complete);			\
  BACKUP_SCALAR(challenge_round);			\
  BACKUP_SCALAR(challenge1);				\
  BACKUP_SCALAR(challenge2);				\
  BACKUP_SCALAR(challenge3);				\
  BACKUP_SCALAR(challenge4);				\
  BACKUP_SCALAR(counters_added);			\
  /*end*/

#define BACKUP_ARRAY(data)	BACKUP_CMD_A(data)
#define BACKUP_SCALAR(data)	BACKUP_CMD_S(data)

#define BACKUP_CMD_A(data)	static char backup_##data[sizeof(data)]
#define BACKUP_CMD_S(data)	static __typeof__(data) backup_##data
BACKUP_AND_SAVE_LIST
#undef BACKUP_CMD_A
#undef BACKUP_CMD_S




/* Copies the same data as backup_data_for_ai()/restore_data_for_ai(), as well as the following four arrays.  They seem to keep track of damage dealt for
 * Reverse Damage, Reverse Polarity, and Simulacrum.
0x5098b0	dword_5098B0[]
0x5098b4	dword_5098B4[]
0x50ab70	dword_50AB70[]
0x50be50	dword_50BE50[] */
#define BACKUP_CMD_A(data)	static char backup0_##data[sizeof(data)]
#define BACKUP_CMD_S(data)	static __typeof__(data) backup0_##data
BACKUP_AND_SAVE_LIST
#undef BACKUP_CMD_A
#undef BACKUP_CMD_S




void copy_to_display_supplement(void)
{
  /* copy_to_display() itself is complex, since it copies out of order, and also translates library[][], graveyard[][], and exile[][] from internal_card_ids to
   * csvids; so I injected a call to this into the function rather than moving the whole thing into C. */
  // (On the other hand, we clearly haven't needed it yet.)
}

// Not, strictly speaking, the correct place for this, but it lets us use the same list to both backup data for ai speculation and to save it in savefiles.

#undef BACKUP
#undef BACKUP_AND_SAVE_LIST



/* Sets bit 1 of return value if (attack_player,attack_card) will auto-destroy (defend_player,defend_card) if the latter blocks the former, and bit 2 if
 * (attack_player,attack_card) will be auto-destroyed by (defend_player,defend_card) if the latter blocks the former.
 *
 * Formerly checked for intersection between each card's color and the other's enemy_against_color, with an attempt to deal with walls.  Almost no cards used
 * the functionality, and not even all of those did so correctly - it worked properly for Thicket Basilisk/Cockatrice, Abu Ja'far, Abomination, and Venom;
 * partially for Deathgazer; and not at all for Battering Ram. */


/* What this seems to do is, during speculation, store value for later retrieval and returns it; and during actual execution, retrieve the value that was stored
 * during the best path.  Not well understood. */


// Like internal_rand(), but if player is the AI, stores and retrieves the choice per remember_ai_value()


void human_assign_blockers(int player);
void ai_assign_blockers(int player)
{
  // 0x4b18e0

  EXE_FN(void, 0x4AF6F0, int)(player);	//TENTATIVE_ai_speculate_on_combat(player);

  EXE_DWORD(0x60827C) = -9999;

  int i;
  for (i = 0; i < 16; ++i)
	EXE_DWORD_PTR(0x6076D8)[i] = 0;

  EXE_FN(int, 0x4B1A50, int, int)(player, 0);

  if (!(ai_is_speculating == 1 || current_turn == HUMAN))
	return;

  // Begin additions
  if (event_flags & EF_ATTACKER_CHOOSES_BLOCKERS)
	{
	  if (!(ai_is_speculating == 1 || current_turn == AI))	// if speculating or AI is attacking, nothing blocks
		human_assign_blockers(player);
	  return;
	}
  // End additions

  for (i = 0; i < EXE_DWORD(0x607D54); ++i)
	{
      forbid_attack = 0;
	  if (event_flags & EA_PAID_BLOCK)
		{
		  EXE_FN(void, 0x435C80, void)();	//push_affected_card_stack()
		  trigger_cause_controller = EXE_DWORD(0x607C2C);	//TENTATIVE_ai_speculation_opponent
		  trigger_cause = EXE_DWORD_PTR(0x608304)[i];		//WILDGUESS_ai_combat_block_card[i]
		  dispatch_trigger(1 - current_turn, TRIGGER_PAY_TO_BLOCK, EXE_STR(0x790248)/*PROMPT_TURNSEQUENCE[0]*/, 1);
		  EXE_FN(void, 0x435CD0, void)();	//pop_affected_card_stack()
		  if (forbid_attack)
			{
			  forbid_attack = 0;
			  continue;
			}
		}

	  uint8_t blk = BYTE0(EXE_DWORD_PTR(0x60775C)[i]);

	  card_instance_t* instance = get_card_instance(EXE_DWORD(0x607C2C),	// TENTATIVE_ai_speculation_opponent
													EXE_DWORD_PTR(0x608304)[i]);	// WILDGUESS_ai_combat_block_card[i]
	  // Begin additions
	  if (instance->blocking != 255)	// Already during one of the two triggers dispatched from here
		continue;
	  // End additions
	  instance->blocking = blk;

	  if (blk != 255)	// Originally !(blk & 0x80) - this failed when blocking ids 128-149
		instance->state |= STATE_BLOCKING;

	  if (ai_is_speculating != 1)
		{
		  /* All this does is SendMessageA(hwnd_MainClass, 0x464u, 0xFFu, 0).  It's always called through a wrapper that checks ai_is_speculating != 1
		   * (except from C where it's already been checked).  wndproc_MainClass responds by calling TENTATIVE_redisplay_all(0, 0xff) at 0x437ec0. */
		  EXE_FN(void, 0x437E20, void)();

		  if (blk != 255)
			play_sound_effect(WAV_BLOCK2);
		}

	  /* This immediately followed the EA_PAID_BLOCK/TRIGGER_PAY_TO_BLOCK.  Putting it down here matches the order when a human blocks; in particular,
	   * instance->blocking is correct during TRIGGER_BLOCKER_CHOSEN, which is necessary for Tromokratis. */
	  if ((event_flags & EA_SELECT_BLOCK) && blk != 255)
		dispatch_trigger2(current_turn, TRIGGER_BLOCKER_CHOSEN, EXE_STR(0x790074), 0,//PROMPT_BLOCKERSELECTION[0]
						  EXE_DWORD(0x607C2C),	//TENTATIVE_ai_speculation_opponent
						  EXE_DWORD_PTR(0x608304)[i]);		//WILDGUESS_ai_combat_block_card[i]
	}
}

extern int raw_mana_available[2][8];
void dispatch_event_raw(event_t event);
void mana_burn(void);
int hack_ai_decision_phase_upkeep = 0;


/* ManalinkEh 2022: 0x26b2200; original functions/ai.c:377. */
int network_rand(int player, int upperbound)
{
  int result;
  if (((unsigned char)trace_mode & 2) != 0)
  {
    result = EXE_FN(int, 0x44e070, int, int)(player, upperbound);
    if ((-1 < result) && (result < upperbound))
    {
      return result;
    }
  }
  result = internal_rand(upperbound);
  return result;
}

/* Recovered October 2022 implementations; see docs/recovery-progress.md. */
int can_autotap(int player, int card, autotap_t autotap_flags);

/* ManalinkEh 2022: 0x26b12c0; original functions/ai.c:15. */
int can_autotap(int player, int card, autotap_t autotap_flags)
{
  enum
  {
    QUERY_IS_PLAIN_MANA_PRODUCER = 3
  };

  card_instance_t *instance;
  int iVar2;
  int iVar3;
  unsigned int uVar4;

  instance = in_play(player, card);
  if (instance != NULL)
  {
    iVar3 = instance->internal_card_id;
    uVar4 = instance->card_color & 0x7f;
    iVar2 = is_what(player, card, 1);
    if (iVar2 != 0)
    {
      uVar4 = uVar4 | mana_colors_added[player];
    }
    if ((uVar4 & 0x3f) != 0)
    {
      uVar4 = cards_data[iVar3].extra_ability;
      if ((uVar4 & 0x1000) == 0)
      {
        iVar2 = is_what(player, card, 1);
        if (iVar2 == 0)
        {
          return 0;
        }
        if ((*(unsigned char *)(mana_colors_added + player) & 0x3f) == 0)
        {
          return 0;
        }
        uVar4 = cards_data[iVar3].extra_ability;
      }
      if (((((uVar4 & 0x10000) == 0) && (iVar3 = can_produce_mana(player, card), iVar3 != 0)) &&
           (((autotap_flags & AUTOTAP_NO_BASIC_LANDS) == AUTOTAP_0 ||
             ((iVar3 = is_basic_land(player, card),
               iVar3 == 0 && (iVar3 = query(player, card, -1, QUERY_IS_PLAIN_MANA_PRODUCER),
                              iVar3 == 0)))))) &&
          ((((autotap_flags & AUTOTAP_NO_NONBASIC_LANDS) == AUTOTAP_0 ||
             (((iVar3 = is_what(player, card, 1),
                iVar3 == 0 || (iVar3 = is_basic_land(player, card), iVar3 != 0)) ||
               (iVar3 = query(player, card, -1, QUERY_IS_PLAIN_MANA_PRODUCER), iVar3 != 0)))) &&
            ((((autotap_flags & AUTOTAP_NO_DONT_AUTO_TAP) == AUTOTAP_0 ||
               ((instance->state & 0x40000) == 0)) &&
              (((autotap_flags & AUTOTAP_NO_ARTIFACTS) == AUTOTAP_0 ||
                ((iVar3 = is_what(player, card, 0x40),
                  iVar3 == 0 || (iVar3 = query(player, card, -1, QUERY_IS_PLAIN_MANA_PRODUCER),
                                 iVar3 != 0))))))))))
      {
        if ((autotap_flags & AUTOTAP_NO_CREATURES) != AUTOTAP_0)
        {
          iVar3 = is_what(player, card, 2);
          return (unsigned int)(iVar3 == 0);
        }
        return 1;
      }
    }
  }
  return 0;
}


#ifndef SHANDALAR
/* ManalinkEh 2022: 0x26b1240; original functions/ai.c:340. */
static int remember_ai_value_impl_2022(int value)
{
  if (ai_is_speculating == 1)
  {
    if (value >= 99)
      ++value;
    EXE_DWORD(0x7a2fe4) = value;
    EXE_FN(void, 0x498f20, void)();
  }
  else
    EXE_FN(int, 0x499050, void)();
  value = EXE_DWORD(0x7a2fe4);
  if (value >= 99)
    --value;
  return value;
}

#endif


#ifndef SHANDALAR
/* ManalinkEh 2022: 0x26b2190; original functions/ai.c:340. */
int remember_ai_value(int player, int value)
{
  EXE_DWORD(0x7a2fe4) = 0;
  if (player == AI && !(trace_mode & 2))
    return remember_ai_value_impl_2022(value);
  return value;
}

#endif


#ifndef SHANDALAR
/* ManalinkEh 2022: 0x26b21c0; original functions/ai.c:368. */
int recorded_rand(int player, int upperbound)
{
  int value = internal_rand(upperbound);
  EXE_DWORD(0x7a2fe4) = 0;
  if (player == AI && !(trace_mode & 2))
    return remember_ai_value_impl_2022(value);
  return value;
}

#endif


#ifndef SHANDALAR
/* ManalinkEh 2022: 0x26b1280; original functions/ai.c:6. */
int check_timer_for_ai_speculation(void)
{
  int decision_time = get_setting(SETTING_AI_DECISION_TIME);
  if (decision_time <= 0)
  {
    decision_time = 5405;
  }
  return (100 * (get_usertime_of_current_thread_in_ms() - start_usertime_of_current_thread_in_ms)) /
         decision_time;
}

#endif


#ifndef SHANDALAR
/* ManalinkEh 2022: 0x26b2240; original functions/ai.c:394. */
int ai_decision_phase(int player, int *phase_code_to_go_to,
                           int *becomes_second_arg_of_main_phase_and_discard_phase,
                           int *becomes_third_arg_of_main_phase)
{

  int upkeep_speculation = hack_ai_decision_phase_upkeep;
  hack_ai_decision_phase_upkeep = 0;

  int store_raw_mana_available[2][8];

  if (trace_mode & 2)
  {
    char str[100];
    sprintf(str, "%d: Entering AI Decision Phase.\n", EXE_DWORD(0x60EC40)++);
    EXE_FN(void, 0x4a7d80, const char *)(str);
  }

  *phase_code_to_go_to = -1;

  if (ai_is_speculating == 1)
  {
    int p = 1 - player;
    int col;
    for (col = 0; col <= 7; ++col)
    {
      store_raw_mana_available[p][col] = raw_mana_available[p][col];
      raw_mana_available[p][col] = landsofcolor_controlled[p][col];
    }

    dispatch_event_raw(EVENT_SHOULD_AI_PLAY);

    card_instance_t *instance;
    int c;
    for (c = 0; c < active_cards_count[AI]; ++c)
      if ((instance = get_card_instance(AI, c))->internal_card_id != -1)
        call_card_function_i(instance, AI, c, EVENT_CAN_COUNTER);

    EXE_FN(void, 0x477070, void)();
    EXE_FN(void, 0x477a90, void)();

    for (p = 0; p <= 1; ++p)
      for (col = 0; col <= 7; ++col)
        raw_mana_available[p][col] = store_raw_mana_available[p][col];

    recalculate_all_cards_in_play();

    int ai_opinion = EXE_FN(int, 0x499160, int)(AI) + ai_modifier;

    if (life[AI] > 0)
      EXE_DWORD(0x60E9FC) |= 4u;

    int v16 = 0;
    if (EXE_DWORD(0x715B30) < ai_opinion && !EXE_DWORD(0x7A372C))
    {
      EXE_DWORD(0x715B30) = ai_opinion;
      EXE_FN(void, 0x4990B0, void)();
      v16 = EXE_DWORD(0x60E9FC);
    }

    if (EXE_DWORD(0x7282F4) == 999)
      EXE_DWORD(0x7282F4) = -1;

    EXE_DWORD(0x7282EC) = 0;
    *becomes_third_arg_of_main_phase = 0;
    EXE_DWORD(0x7A372C) = 0;

    if (check_timer_for_ai_speculation() > EXE_DWORD(0x786DCC) / 2 &&
        (5 * (5 * opponent_skill + 5) <= EXE_DWORD(0x738B48) ||
         ((EXE_DWORD(0x60E9FC) & 4) < 1 ? 200 : 50) < check_timer_for_ai_speculation()))
    {
      ai_is_speculating = 0;
      EXE_DWORD(0x7282F4) = -1;
      EXE_DWORD(0x60E9FC) = v16;
    }

    ++EXE_DWORD(0x738B48);

    if (EXE_DWORD(0x60EC3C) == 1)
    {
      if (land_can_be_played & TENTATIVE_LCBP_DURING_SECOND_MAIN_PHASE)
        current_phase = PHASE_MAIN2;
      else
        current_phase = PHASE_MAIN1;
      *becomes_second_arg_of_main_phase_and_discard_phase = 1;
      *phase_code_to_go_to = 6;
    }

    if (EXE_DWORD(0x60EC3C) == 2)
    {
      current_phase = PHASE_NORMAL_COMBAT_DAMAGE;
      *becomes_second_arg_of_main_phase_and_discard_phase = 2;
      *phase_code_to_go_to = 6;
    }

    if (EXE_DWORD(0x60EC3C) == 3)
    {
      *becomes_second_arg_of_main_phase_and_discard_phase = 1;
      *phase_code_to_go_to = 7;
    }

    if (EXE_DWORD(0x60EC3C) == 4)
    {
      if (land_can_be_played & TENTATIVE_LCBP_DURING_COMBAT)
        current_phase = PHASE_MAIN2;
      else
        current_phase = PHASE_MAIN1;
      *becomes_third_arg_of_main_phase = 0;
      *becomes_second_arg_of_main_phase_and_discard_phase = 4;
      *phase_code_to_go_to = 6;
    }

    if (EXE_DWORD(0x60EC3C) == 5)
    {
      current_phase = PHASE_NORMAL_COMBAT_DAMAGE;
      *becomes_second_arg_of_main_phase_and_discard_phase = 5;
      *phase_code_to_go_to = 6;
    }

    if (EXE_DWORD(0x60EC3C) == 6)
    {
      current_phase = PHASE_NORMAL_COMBAT_DAMAGE;
      *becomes_second_arg_of_main_phase_and_discard_phase = 6;
      *phase_code_to_go_to = 6;
    }

    if (EXE_DWORD(0x60EC3C) == 7)
    {
      if (land_can_be_played & TENTATIVE_LCBP_DURING_COMBAT)
        current_phase = PHASE_MAIN2;
      else
        current_phase = PHASE_MAIN1;
      *becomes_third_arg_of_main_phase = 0;
      *becomes_second_arg_of_main_phase_and_discard_phase = 7;
      *phase_code_to_go_to = 6;
    }

    if (EXE_DWORD(0x60EC3C) == 8)
    {
      current_phase = PHASE_NORMAL_COMBAT_DAMAGE;
      *becomes_second_arg_of_main_phase_and_discard_phase = 8;
      *phase_code_to_go_to = 6;
    }

    if (EXE_DWORD(0x60EC3C) == 17)
      *phase_code_to_go_to = 4;
  }

  if (upkeep_speculation)
    return 0;

  if (*phase_code_to_go_to == -1)
  {
    current_phase = PHASE_CLEANUP2;

    EXE_FN(int, 0x439C90, int)(PHASE_CLEANUP2);
    mana_burn();

    if (is_anyone_dead())
      return 1;

    if (current_turn == EXE_DWORD(0x4EF1B0) && EXE_DWORD(0x4EF1AC) == PHASE_CLEANUP2)
    {
      EXE_DWORD(0x4EF1B0) = -1;
      EXE_DWORD(0x4EF1AC) = -1;
    }

    EXE_FN(int, 0x439A80, void)();
    play_sound_effect(WAV_ENDTURN);
  }

  return 0;
}

#endif


#ifndef SHANDALAR
/* October 2022 private counter-type snapshot scalars. */
static int backup_counters_added_type;
static int backup0_counters_added_type;
#endif


#ifndef SHANDALAR
/* ManalinkEh 2022: 0x26b1700; original functions/ai.c:195. */
void backup_data_for_ai(void)
{
  memcpy_suppress_size_warning((void *)0x50be70, &must_attack, 0x1a740);
  STATIC_ASSERT(sizeof(card_data_t) == 72, sizeof_card_data_t_must_be_72);
  memcpy((void *)0x5265b0, &cards_data[EXE_DWORD(0x7a5380)], 16 * sizeof(card_data_t));
  EXE_DWORD(0x5b9b3c) = ai_modifier;
  memcpy(backup_card_ext, card_ext, sizeof(card_ext));
  memcpy(backup_graveyard_source, graveyard_source, sizeof(graveyard_source));
  backup_counters_added_type = counters_added_type;
  backup_next_graveyard_source_id = next_graveyard_source_id;
  backup_turn_count = turn_count;
  backup_always_prompt_for_color = always_prompt_for_color;
  memcpy(backup_mulligans_complete, mulligans_complete, sizeof(mulligans_complete));
  backup_challenge_round = challenge_round;
  backup_challenge1 = challenge1;
  backup_challenge2 = challenge2;
  backup_challenge3 = challenge3;
  backup_challenge4 = challenge4;
  backup_counters_added = counters_added;
  memcpy(backup_mana_colors_added, mana_colors_added, sizeof(mana_colors_added));
  memcpy(backup_mana_color_override, mana_color_override, sizeof(mana_color_override));
  memcpy(backup_mana_quantity_override, mana_quantity_override, sizeof(mana_quantity_override));
  memcpy(backup_multi_mana_transmutation, multi_mana_transmutation,
         sizeof(multi_mana_transmutation));
  memcpy(backup_card_counts, card_counts, sizeof(card_counts));
  memcpy(backup_card_flags, card_flags, sizeof(card_flags));
  memcpy(backup_your_spells_cant_be_countered_by_color, your_spells_cant_be_countered_by_color,
         sizeof(your_spells_cant_be_countered_by_color));
  memcpy(backup_roll_extra_dice, roll_extra_dice, sizeof(roll_extra_dice));
  memcpy(backup_add_to_die_rolls, add_to_die_rolls, sizeof(add_to_die_rolls));
  memcpy(backup_added_subtypes_not_in_play, added_subtypes_not_in_play,
         sizeof(added_subtypes_not_in_play));
}

#endif


#ifndef SHANDALAR
/* ManalinkEh 2022: 0x26b1900; original functions/ai.c:211. */
void restore_data_for_ai(void)
{
  memcpy_suppress_size_warning(&must_attack, (void *)0x50be70, 0x1a740);
  STATIC_ASSERT(sizeof(card_data_t) == 72, sizeof_card_data_t_must_be_72);
  memcpy(&cards_data[EXE_DWORD(0x7a5380)], (void *)0x5265b0, 16 * sizeof(card_data_t));
  ai_modifier = EXE_DWORD(0x5b9b3c);
  memcpy(card_ext, backup_card_ext, sizeof(card_ext));
  memcpy(graveyard_source, backup_graveyard_source, sizeof(graveyard_source));
  counters_added_type = backup_counters_added_type;
  next_graveyard_source_id = backup_next_graveyard_source_id;
  turn_count = backup_turn_count;
  always_prompt_for_color = backup_always_prompt_for_color;
  memcpy(mulligans_complete, backup_mulligans_complete, sizeof(mulligans_complete));
  challenge_round = backup_challenge_round;
  challenge1 = backup_challenge1;
  challenge2 = backup_challenge2;
  challenge3 = backup_challenge3;
  challenge4 = backup_challenge4;
  counters_added = backup_counters_added;
  memcpy(mana_colors_added, backup_mana_colors_added, sizeof(mana_colors_added));
  memcpy(mana_color_override, backup_mana_color_override, sizeof(mana_color_override));
  memcpy(mana_quantity_override, backup_mana_quantity_override, sizeof(mana_quantity_override));
  memcpy(multi_mana_transmutation, backup_multi_mana_transmutation,
         sizeof(multi_mana_transmutation));
  memcpy(card_counts, backup_card_counts, sizeof(card_counts));
  memcpy(card_flags, backup_card_flags, sizeof(card_flags));
  memcpy(your_spells_cant_be_countered_by_color, backup_your_spells_cant_be_countered_by_color,
         sizeof(your_spells_cant_be_countered_by_color));
  memcpy(roll_extra_dice, backup_roll_extra_dice, sizeof(roll_extra_dice));
  memcpy(add_to_die_rolls, backup_add_to_die_rolls, sizeof(add_to_die_rolls));
  memcpy(added_subtypes_not_in_play, backup_added_subtypes_not_in_play,
         sizeof(added_subtypes_not_in_play));
}

#endif


#ifndef SHANDALAR
/* ManalinkEh 2022: 0x26b1b00; original functions/ai.c:237. */
void backup_data_for_ai_0(void)
{
  memcpy_suppress_size_warning((void *)0x526a30, &must_attack, 0x1cd00);
  EXE_DWORD(0x5bbb44) = 0;
  STATIC_ASSERT(sizeof(card_data_t) == 72, sizeof_card_data_t_must_be_72);
  memcpy((void *)0x543730, &cards_data[EXE_DWORD(0x7a5380)], 16 * sizeof(card_data_t));
  if (stack_size < 0)
    backtrace_and_abort("functions/ai.c:247\nstack_size >= 0");
  memcpy(backup0_card_ext, card_ext, sizeof(card_ext));
  memcpy(backup0_graveyard_source, graveyard_source, sizeof(graveyard_source));
  backup0_counters_added_type = counters_added_type;
  backup0_next_graveyard_source_id = next_graveyard_source_id;
  backup0_turn_count = turn_count;
  backup0_always_prompt_for_color = always_prompt_for_color;
  memcpy(backup0_mulligans_complete, mulligans_complete, sizeof(mulligans_complete));
  backup0_challenge_round = challenge_round;
  backup0_challenge1 = challenge1;
  backup0_challenge2 = challenge2;
  backup0_challenge3 = challenge3;
  backup0_challenge4 = challenge4;
  backup0_counters_added = counters_added;
  memcpy(backup0_mana_colors_added, mana_colors_added, sizeof(mana_colors_added));
  memcpy(backup0_mana_color_override, mana_color_override, sizeof(mana_color_override));
  memcpy(backup0_mana_quantity_override, mana_quantity_override, sizeof(mana_quantity_override));
  memcpy(backup0_multi_mana_transmutation, multi_mana_transmutation,
         sizeof(multi_mana_transmutation));
  memcpy(backup0_card_counts, card_counts, sizeof(card_counts));
  memcpy(backup0_card_flags, card_flags, sizeof(card_flags));
  memcpy(backup0_your_spells_cant_be_countered_by_color, your_spells_cant_be_countered_by_color,
         sizeof(your_spells_cant_be_countered_by_color));
  memcpy(backup0_roll_extra_dice, roll_extra_dice, sizeof(roll_extra_dice));
  memcpy(backup0_add_to_die_rolls, add_to_die_rolls, sizeof(add_to_die_rolls));
  memcpy(backup0_added_subtypes_not_in_play, added_subtypes_not_in_play,
         sizeof(added_subtypes_not_in_play));
}

#endif


#ifndef SHANDALAR
/* ManalinkEh 2022: 0x26b1d20; original functions/ai.c:256. */
void restore_data_for_ai_0(void)
{
  memcpy_suppress_size_warning(&must_attack, (void *)0x526a30, 0x1cd00);
  STATIC_ASSERT(sizeof(card_data_t) == 72, sizeof_card_data_t_must_be_72);
  memcpy(&cards_data[EXE_DWORD(0x7a5380)], (void *)0x543730, 16 * sizeof(card_data_t));
  memcpy(card_ext, backup0_card_ext, sizeof(card_ext));
  memcpy(graveyard_source, backup0_graveyard_source, sizeof(graveyard_source));
  counters_added_type = backup0_counters_added_type;
  next_graveyard_source_id = backup0_next_graveyard_source_id;
  turn_count = backup0_turn_count;
  always_prompt_for_color = backup0_always_prompt_for_color;
  memcpy(mulligans_complete, backup0_mulligans_complete, sizeof(mulligans_complete));
  challenge_round = backup0_challenge_round;
  challenge1 = backup0_challenge1;
  challenge2 = backup0_challenge2;
  challenge3 = backup0_challenge3;
  challenge4 = backup0_challenge4;
  counters_added = backup0_counters_added;
  memcpy(mana_colors_added, backup0_mana_colors_added, sizeof(mana_colors_added));
  memcpy(mana_color_override, backup0_mana_color_override, sizeof(mana_color_override));
  memcpy(mana_quantity_override, backup0_mana_quantity_override, sizeof(mana_quantity_override));
  memcpy(multi_mana_transmutation, backup0_multi_mana_transmutation,
         sizeof(multi_mana_transmutation));
  memcpy(card_counts, backup0_card_counts, sizeof(card_counts));
  memcpy(card_flags, backup0_card_flags, sizeof(card_flags));
  memcpy(your_spells_cant_be_countered_by_color, backup0_your_spells_cant_be_countered_by_color,
         sizeof(your_spells_cant_be_countered_by_color));
  memcpy(roll_extra_dice, backup0_roll_extra_dice, sizeof(roll_extra_dice));
  memcpy(add_to_die_rolls, backup0_add_to_die_rolls, sizeof(add_to_die_rolls));
  memcpy(added_subtypes_not_in_play, backup0_added_subtypes_not_in_play,
         sizeof(added_subtypes_not_in_play));
}

#endif


#ifndef SHANDALAR
/* ManalinkEh 2022: 0x26b1f10; original functions/ai.c:270. */
int save_or_load_supplement(int status)
{
  status &= save_or_load_data(card_ext, sizeof(card_ext));
  status &= save_or_load_data(graveyard_source, sizeof(graveyard_source));
  status &= save_or_load_data(&next_graveyard_source_id, sizeof(next_graveyard_source_id));
  status &= save_or_load_data(&turn_count, sizeof(turn_count));
  status &= save_or_load_data(&always_prompt_for_color, sizeof(always_prompt_for_color));
  status &= save_or_load_data(mulligans_complete, sizeof(mulligans_complete));
  status &= save_or_load_data(&challenge_round, sizeof(challenge_round));
  status &= save_or_load_data(&challenge1, sizeof(challenge1));
  status &= save_or_load_data(&challenge2, sizeof(challenge2));
  status &= save_or_load_data(&challenge3, sizeof(challenge3));
  status &= save_or_load_data(&challenge4, sizeof(challenge4));
  status &= save_or_load_data(&counters_added, sizeof(counters_added));
  status &= save_or_load_data(&counters_added_type, sizeof(counters_added_type));
  status &= save_or_load_data(mana_colors_added, sizeof(mana_colors_added));
  status &= save_or_load_data(mana_color_override, sizeof(mana_color_override));
  status &= save_or_load_data(mana_quantity_override, sizeof(mana_quantity_override));
  status &= save_or_load_data(multi_mana_transmutation, sizeof(multi_mana_transmutation));
  status &= save_or_load_data(card_counts, sizeof(card_counts));
  status &= save_or_load_data(card_flags, sizeof(card_flags));
  status &= save_or_load_data(your_spells_cant_be_countered_by_color,
                              sizeof(your_spells_cant_be_countered_by_color));
  status &= save_or_load_data(roll_extra_dice, sizeof(roll_extra_dice));
  status &= save_or_load_data(add_to_die_rolls, sizeof(add_to_die_rolls));
  status &= save_or_load_data(added_subtypes_not_in_play, sizeof(added_subtypes_not_in_play));
  if (EXE_DWORD(0x5bc5f8))
    --turn_count;
  return status;
}

#endif


#ifndef SHANDALAR
/* ManalinkEh 2022: 0x26b1500; original functions/ai.c:54. */
void human_autotap_for_mana(int player, int *amt, int *unknown_v46, int unknown_v47)
{

  if (!pay_mana_maximally_satisfied(&PAY_MANA_COLORLESS, x_value, max_x_value))
  {
    try_to_pay_for_mana_by_autotapping(player, amt, unknown_v46,
                                       AUTOTAP_NO_CREATURES | AUTOTAP_NO_ARTIFACTS |
                                           AUTOTAP_NO_DONT_AUTO_TAP | AUTOTAP_NO_NONBASIC_LANDS,
                                       unknown_v47);
  }

  if (!pay_mana_maximally_satisfied(&PAY_MANA_COLORLESS, x_value, max_x_value))
  {
    try_to_pay_for_mana_by_autotapping(
        player, amt, unknown_v46,
        AUTOTAP_NO_CREATURES | AUTOTAP_NO_ARTIFACTS | AUTOTAP_NO_DONT_AUTO_TAP, unknown_v47);
  }

  if ((player != 1 || (trace_mode & 2)) && ai_is_speculating != 1)
  {
    if (get_setting(SETTING_AUTOTAP_ARTIFACTS))
    {

      if (!pay_mana_maximally_satisfied(&PAY_MANA_COLORLESS, x_value, max_x_value))
      {
        try_to_pay_for_mana_by_autotapping(
            player, amt, unknown_v46, AUTOTAP_NO_CREATURES | AUTOTAP_NO_DONT_AUTO_TAP, unknown_v47);
      }

      if (get_setting(SETTING_AUTOTAP_CREATURES))
      {

        if (!pay_mana_maximally_satisfied(&PAY_MANA_COLORLESS, x_value, max_x_value))
        {
          try_to_pay_for_mana_by_autotapping(player, amt, unknown_v46, AUTOTAP_NO_DONT_AUTO_TAP,
                                             unknown_v47);
        }
      }
    }
    else if (get_setting(SETTING_AUTOTAP_CREATURES))
    {

      if (!pay_mana_maximally_satisfied(&PAY_MANA_COLORLESS, x_value, max_x_value))
      {
        try_to_pay_for_mana_by_autotapping(
            player, amt, unknown_v46, AUTOTAP_NO_ARTIFACTS | AUTOTAP_NO_DONT_AUTO_TAP, unknown_v47);
      }
    }
  }
}

#endif


#ifndef SHANDALAR
/* ManalinkEh 2022: 0x26b1100; original functions/ai.c:292. */
static int check_destroys_if_blocked_oneside(int lethal_player, int lethal_card, int other_player,
                                           int other_card)
{
  card_instance_t *instance = get_card_instance(lethal_player, lethal_card);
  int lethal = instance->destroys_if_blocked;
  int result = 0;
  if (lethal)
  {
    if ((lethal & DIFB_DESTROYS_ALL) == DIFB_DESTROYS_ALL)
      result = 1;
    else if ((lethal & DIFB_DESTROYS_WALLS) && has_subtype(other_player, other_card, SUBTYPE_WALL))
      result = 1;
    else if ((lethal & DIFB_DESTROYS_NONWALLS) &&
             !has_subtype(other_player, other_card, SUBTYPE_WALL))
      result = 1;

    if (result != 1 && (lethal & DIFB_DESTROYS_UNPROTECTED) &&
        !is_protected_from(other_player, other_card, lethal_player, lethal_card,
                                IPF2022_DAMAGE))
      result = 1;

    if (lethal & (DIFB_ASK_CARD | DIFB_ASK_ALL_CARDS))
    {
      if (lethal & DIFB_ASK_ALL_CARDS)
        result |= dispatch_event_with_attacker(
            lethal_player, lethal_card, EVENT_CHECK_DESTROY_IF_BLOCKED, other_player, other_card);
      else
        result |= dispatch_event_with_attacker_to_one_card(
            lethal_player, lethal_card, EVENT_CHECK_DESTROY_IF_BLOCKED, other_player, other_card);
    }
  }
  return result;
}

#endif


#ifndef SHANDALAR
/* ManalinkEh 2022: 0x26b2140; original functions/ai.c:327. */
int check_destroys_if_blocked(int attack_player, int attack_card, int defend_player,
                                   int defend_card)
{
  int result1 = check_destroys_if_blocked_oneside(attack_player, attack_card, defend_player,
                                                       defend_card);

  int result2 = check_destroys_if_blocked_oneside(defend_player, defend_card, attack_player,
                                                       attack_card);

  result2 = ((result2 & 1) << 1) | ((result2 & 2) >> 1);

  return result1 | result2;
}

#endif
