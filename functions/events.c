// -*- c-basic-offset:2 -*-
// Low-level event, trigger, and card function handling.

#include "manalink.h"

/* October 2022 DWARF globals, original events.c:786-787; DLL .data contains 1. */
int raw_base_pt = 1;              /* 0x02721d64 */
int raw_characteristic_pt = 1;    /* 0x02721d60 */

// These are fairly common here, but shouldn't be used elsewhere
#define push_affected_card_stack	EXE_FN(void, 0x435C80, void)
#define pop_affected_card_stack		EXE_FN(void, 0x435CD0, void)

// A frontend for call_card_fn() where both address and instance should be computed from {player,card}.
int call_card_function(int player, int card, event_t event)
{
  card_instance_t *instance = get_card_instance(player, card);
  int iid = instance->internal_card_id;
  if (iid == -1)
    iid = instance->backup_internal_card_id;
  return call_card_fn(cards_data[iid].code_pointer, get_card_instance(player, card), player, card,
                      event);
}

// A frontend for call_card_fn() where address should be computed from instance.
int call_card_function_i(card_instance_t* instance, int player, int card, event_t event)
{
  CardFunction fn = cards_data[instance->internal_card_id].code_pointer;
  return call_card_fn(fn, instance, player, card, event);
}

int call_card_fn_impl(CardFunction address, card_instance_t* instance, int player, int card, event_t event);	// never call directly
// Puts instance into esi, then calls address(player, card, event).
int call_card_fn(CardFunction address, card_instance_t *instance, int player, int card,
                 event_t event)
{
  extern char etext __asm__("etext");
  void show_backtrace(const char *, const char *);
  uintptr_t entry = (uintptr_t)address;
  if (entry < 0x401000U || (entry > 0x4ea000U && entry < 0x2001000U) || entry > (uintptr_t)&etext)
  {
    char buf[100];
    sprintf(buf, "call_card_fn(%p)", address);
    show_backtrace("bad parameters", buf);
  }
  int old_trigger_condition = trigger_condition;
  if (event != EVENT_TRIGGER && event != EVENT_RESOLVE_TRIGGER && event != EVENT_END_TRIGGER)
    trigger_condition = -1;
  int rval = call_card_fn_impl(address, instance, player, card, event);
  trigger_condition = old_trigger_condition;
  return rval;
}

extern int timestamp_card[500];
extern int timestamp_player[500];
void dispatch_event_raw(event_t event)
{
  // 0x435a40

  minimize_nondraining_mana();	// Needs to happen quite frequently.  This'll do it.

  // Begin additions
  // These two are because something calls this without stashing them when it really should; http://www.slightlymagic.net/forum/viewtopic.php?t=13665 is a symptom.
  int old_affected_card_controller = affected_card_controller;
  int old_affected_card = affected_card;
  /* And this is an optimization; compare e.g. upkeep_trigger_ability_mode(), which just checks trigger_condition before calling expensive things like
   * count_upkeeps(), even though it'll only respond to EVENT_TRIGGER and EVENT_RESOLVE_TRIGGER.  When called from dispatch_event() or similar, it's already set
   * to -1, but plenty of things call dispatch_event_raw instead.  Shame on them. */
  int old_trigger_condition = trigger_condition;
  if (event != EVENT_TRIGGER && event != EVENT_RESOLVE_TRIGGER && event != EVENT_END_TRIGGER)
	trigger_condition = -1;
  // End additions

  //if (ai_is_speculating!=1)logg("dispatch_event_raw", "%s%s%s for (%d,%d)=%s%s", trigger_condition <= 0 ? "" : trigger_t_str(trigger_condition), trigger_condition <= 0 ? "" : "/", event_t_str(event), affected_card_controller, affected_card, get_name(affected_card_controller, affected_card));

  // Begin additions
  // In the exe version, cards attacking cards were tapped in the same loop as the event was dispatched.  Instead, do it before dispatching.
  // If this seems out-of-place here, well, it did to me too; but I'm not going to try to shoehorn it into this function's callers.
  unsigned int ts;
  unsigned char attackers[151];
  if (event == EVENT_DECLARE_ATTACKERS)
	{
	  memset(attackers, 0xff, sizeof(attackers));
	  number_of_attackers_declared = 0;
	  for (ts = 0; ts < 500 && timestamp_player[ts] != -1; ++ts)
		{
		  int player = timestamp_player[ts];
		  int card = timestamp_card[ts];

		  card_instance_t* instance;
		  if (player == current_turn && (instance = in_play(player, card)) && instance->timestamp == ts
			  && (instance->state & STATE_ATTACKING))
			{
			  attackers[number_of_attackers_declared++] = card;
			  if (!(instance->state & STATE_TAPPED)
				  && !(get_special_abilities_by_instance(instance) & SP_KEYWORD_VIGILANCE))
				tap_card(player, card);
			}
		}
	}

  if (event == EVENT_CAN_SKIP_TURN)	// First event dispatched every turn, used only by skip-turn effects and housekeeping.
	enable_xtrigger_flags = 0;
  // End additions

  for (ts = 0; ts < 500 && timestamp_player[ts] != -1; ++ts)
	{
	  int player = timestamp_player[ts];
	  int card = timestamp_card[ts];

	  card_instance_t* instance = get_card_instance(player, card);
	  if (instance->timestamp == ts
		  && instance->internal_card_id != -1
		  && !(instance->state & STATE_OUBLIETTED)
		  && (instance->state & (STATE_INVISIBLE | STATE_IN_PLAY)))
		{
		  CardFunction fn = cards_data[instance->internal_card_id].code_pointer;
		  call_card_fn(fn, instance, player, card, event);
		  // Begin removals
		  // if event == EVENT_DECLARE_ATTACKERS, then tap the card if it's attacking, untapped, and doesn't have vigilance; then resolve damage cards and run regeneration/graveyard triggers
		  // End removals
		}
	}

  if (event == EVENT_DECLARE_ATTACKERS && number_of_attackers_declared > 0)
	{
	  // Begin additions
	  dispatch_xtrigger2(current_turn, XTRIGGER_ATTACKING, "attacking", 0, 0, 0);

	  /* These were run after each card tapped.  That was both too many times *and* not enough, since they didn't run for non-attacking cards handling
	   * EVENT_DECLARE_ATTACKERS. */
	  EXE_FN(void, 0x477070, void)();	// resolve_damage_cards_and_prevent_damage();
	  EXE_FN(void, 0x477a90, void)();	// regenerate_or_graveyard_triggers();
	  // End additions

	  recalculate_all_cards_in_play();
	}

  // Begin additions
  affected_card_controller = old_affected_card_controller;
  affected_card = old_affected_card;
  trigger_condition = old_trigger_condition;

  if (event == EVENT_TAPPED_TO_PLAY_ABILITY || event == EVENT_PLAY_ABILITY
	  || (event == EVENT_CAST_SPELL && !check_state(affected_card_controller, affected_card, STATE_IN_PLAY)))	// actually cast, not put_into_play
	recalculate_all_cards_in_play();
  // End additions
}

// dispatch_event() forces the initial value of event_result to 0; this lets you set it arbitrarily.
int dispatch_event_with_initial_event_result(int new_affected_card_controller,
                                             int new_affected_card, event_t event,
                                             int initial_event_result)
{
  return dispatch_event_with_attacker_and_initial_event_result(
      new_affected_card_controller, new_affected_card, event, 1 - new_affected_card_controller, -1,
      initial_event_result);
}

/* dispatch_event() forces attacking_card_controller to 1-player and attacking_card to -1; this lets you set them explicitly.  Returns the value of event_result
 * as set by the cards' functions (before restoring event_result's previous value). */
int dispatch_event_with_attacker(int new_affected_card_controller, int new_affected_card,
                                 event_t event, int new_attacking_card_controller,
                                 int new_attacking_card)
{
  return dispatch_event_with_attacker_and_initial_event_result(
      new_affected_card_controller, new_affected_card, event, new_attacking_card_controller,
      new_attacking_card, 0);
}

/* Identical to dispatch_event_with_attacker(), but only sends the event to (new_affeced_card_controller,new_affected_card).  Differs from a bare
 * call_card_function() in that it preserves affected_card_controller, affected_card, trigger_condition, event_result, etc.  Returns the value of event_result
 * as set by the card function (before restoring event_result's previous value). */
int dispatch_event_with_attacker_to_one_card(int new_affected_card_controller,
                                             int new_affected_card, event_t event,
                                             int new_attacking_card_controller,
                                             int new_attacking_card)
{
  return dispatch_event_arbitrary_to_one_card_with_initial_event_result(
      new_affected_card_controller, new_affected_card, event, new_affected_card_controller,
      new_affected_card, new_attacking_card_controller, new_attacking_card, 0);
}

int event_rval;
/* Identical to dispatch_event_with_attacker_to_one_card(), but sends the event to an arbitrary (player, card) pair which can differ from
 * (affected_card_controller, affected_card). */
int dispatch_event_arbitrary_to_one_card(int player, int card, event_t event,
                                         int new_affected_card_controller, int new_affected_card,
                                         int new_attacking_card_controller, int new_attacking_card)
{
  return dispatch_event_arbitrary_to_one_card_with_initial_event_result(
      player, card, event, new_affected_card_controller, new_affected_card,
      new_attacking_card_controller, new_attacking_card, 0);
}

/* The semantics of this one are a bit unusual.  Avoid unless you know exactly why you need it. */
int dispatch_event_to_single_card(int player, int card, event_t event, int new_attacking_card_controller, int new_attacking_card)
{
  card_instance_t* inst = get_card_instance(player, card);
  if (inst->internal_card_id == -1)
	return 0;

  if (event == EVENT_CAN_ACTIVATE
	  && ((inst->state & STATE_OUBLIETTED) || inst->upkeep_flags & 8))
	return 0;

  push_affected_card_stack();

  event_result = 0;
  affected_card_controller = player;
  affected_card = card;
  attacking_card_controller = new_attacking_card_controller;
  attacking_card = new_attacking_card;
  int old_736808 = EXE_DWORD(0x736808);

  int old_trigger_condition = trigger_condition;
  if (event != EVENT_TRIGGER && event != EVENT_RESOLVE_TRIGGER && event != EVENT_END_TRIGGER)
    trigger_condition = -1;

  int rval = call_card_function_i(inst, player, card, event);

  trigger_condition = old_trigger_condition;

  if (rval == 99
	  || !(land_can_be_played & (LCBP_REGENERATION | LCBP_SPELL_BEING_PLAYED | LCBP_DAMAGE_PREVENTION))
	  || !(event == EVENT_CAN_CAST || event == EVENT_CAN_ACTIVATE)
	  || EXE_FN(int, 0x435c30, int, int)(player, card)	// is_mana_source_but_not_act_ability
	  || (land_can_be_played & LCBP_CHARGING_MANA))
	EXE_DWORD(0x739D58) = event_result;
  else
	{
	  EXE_DWORD(0x736808) = old_736808;
	  rval = 0;
	}
  pop_affected_card_stack();
  return rval;
}

// Properly stashes globals then calls an arbitrary iid's card function for player/card (instead of that card's own one).  Returns what the function does.
int dispatch_event_to_single_card_overriding_function(int player, int card, event_t event, int iid)
{
  push_affected_card_stack();

  event_result = 0;
  affected_card_controller = player;
  affected_card = card;
  attacking_card_controller = 1-player;
  attacking_card = -1;

  int old_trigger_condition = trigger_condition;
  if (event != EVENT_TRIGGER && event != EVENT_RESOLVE_TRIGGER && event != EVENT_END_TRIGGER)
	trigger_condition = -1;

  CardFunction fn = cards_data[iid].code_pointer;
  int rval = call_card_fn(fn, get_card_instance(player, card), player, card, event);

  trigger_condition = old_trigger_condition;
  pop_affected_card_stack();

  return rval;
}

int dispatch_trigger_to_one_card(int player, int card, event_t event)
{
  // 0x477970
  if (event == EVENT_TRIGGER
	  && (EXE_DWORD(0x60E9F8)	// suppress_this_trigger
		  || (get_card_instance(player, card)->state & STATE_PROCESSING)))
	return 0;

  if (trigger_condition < 200)
	return 0;

  push_affected_card_stack();	// missing in original (and the pop below)

  event_result = 0;
  affected_card_controller = player;
  affected_card = card;
  attacking_card = -1;
  dispatch_event_raw(event);
  int rval = event_result;

  pop_affected_card_stack();

  return rval;
}

xtrigger_t xtrigger_impl_value_dont_use_directly = 0;
enable_xtrigger_flags_t enable_xtrigger_flags = 0;

static void dispatch_trigger_impl(int player, trigger_t trig, xtrigger_t xtrig, const char *prompt,
                                  int TENTATIVE_allow_response)
{
  // Original at 0x4371E0
#define TRIGGER_DEPTH EXE_DWORD(0x785E60)

  ++TRIGGER_DEPTH;
  int orig_tcc = trigger_cause_controller;
  int orig_tc = trigger_cause;
  int old_62C180 = EXE_DWORD(0x62C180);
  int old_reason_for_trigger_controller = reason_for_trigger_controller;
  int old_62BCE8 = EXE_DWORD(0x62BCE8);
  EXE_DWORD(0x62BCE8) = trig;
  reason_for_trigger_controller = player;
  int p, c;

  // Begin additions
  char processing[2][150];
  int processing_set = 0;
  if (TRIGGER_DEPTH != 1)
  {
    processing_set = 1;
    for (p = 0; p < 2; ++p)
    {
      ASSERT(active_cards_count[p] <= 150);
      for (c = 0; c < active_cards_count[p]; ++c)
      {
        card_instance_t *instance = get_card_instance(p, c);
        if (instance->internal_card_id == -1)
          processing[p][c] = 2;
        else if (instance->state & STATE_PROCESSING)
        {
          processing[p][c] = 1;
          instance->state &= ~STATE_PROCESSING;
        }
        else
          processing[p][c] = 0;

        if (instance->state & STATE_IS_TRIGGERING)
        {
          processing[p][c] |= 4;
          instance->state &= ~STATE_IS_TRIGGERING;
        }
      }

      for (c = active_cards_count[p]; c < 150; ++c)
        processing[p][c] = 2;
    }
  }
  // End additions

  int old_620860 = EXE_DWORD(0x620860);
  int old_60A554 = EXE_DWORD(0x60A554);
  unsigned int v8;
  do
  {
    if (trace_mode & 2)
      EXE_DWORD(0x620860) = 1;
    else if (player)
      EXE_DWORD(0x620860) = 2;
    else
      EXE_DWORD(0x620860) = 1;

    trigger_condition = trig;
    xtrigger_impl_value_dont_use_directly = xtrig;
    if (TENTATIVE_allow_response)
      EXE_DWORD(0x60A554) = TYPE_INSTANT | TYPE_INTERRUPT;
    else
      EXE_DWORD(0x60A554) = 0;
    cancel = 0;
    EXE_DWORD(0x62852C) = 0;
    EXE_DWORD(0x62C180) = 0;
    v8 = EXE_FN(int, 0x475A30, int, const char *)(player, prompt);
    EXE_DWORD(0x60A53C) = old_60A554 & (TYPE_INSTANT | TYPE_INTERRUPT);
  } while (((v8 < 1 ? 4 : 6) & EXE_DWORD(0x62C180)) || (TENTATIVE_allow_response && v8));
  EXE_DWORD(0x60A554) = old_60A554;
  EXE_DWORD(0x620860) = old_620860;

  // Begin additions
  // Not just dispatch_event(... EVENT_END_TRIGGER), since that'll clear trigger_condition
  // Set again, just in case something changed them and didn't restore
  trigger_condition = trig;
  xtrigger_impl_value_dont_use_directly = xtrig;
  reason_for_trigger_controller = player;
  trigger_cause = orig_tc;
  trigger_cause_controller = orig_tcc;
  int old_aff_cc = affected_card_controller, old_aff_c = affected_card, old_ev = event_result;
  card_instance_t *inst;
  for (p = 0; p < 2; ++p)
    for (c = 0; c < active_cards_count[p]; ++c)
      if ((inst = in_play(p, c)))
      {
        affected_card_controller = p;
        affected_card = c;
        call_card_function_i(inst, p, c, EVENT_END_TRIGGER);
      }
  affected_card_controller = old_aff_cc;
  affected_card = old_aff_c;
  event_result = old_ev;
  trigger_cause = orig_tc;
  trigger_cause_controller = orig_tcc;
  // End additions

  trigger_condition = -1;

  --TRIGGER_DEPTH;
  if (TRIGGER_DEPTH == 0)
  {
    for (p = 0; p < 2; ++p)
      for (c = 0; c < active_cards_count[p]; ++c)
        get_card_instance(p, c)->state &= ~(STATE_PROCESSING | STATE_IS_TRIGGERING);

    if (!EXE_DWORD(0x7A31AC))
    {
      EXE_DWORD(0x60A538) = 0;
      if (ai_is_speculating != 1)
        EXE_DWORD(0x60A54C) = 0;
    }
  }
  // Begin additions
  else
  {
    ASSERT(processing_set);
    for (p = 0; p < 2; ++p)
    {
      ASSERT(active_cards_count[p] <= 150);
      for (c = 0; c < active_cards_count[p]; ++c)
      {
        card_instance_t *instance = get_card_instance(p, c);

        if (instance->internal_card_id ==
            -1) // Any newly-obliterated cards always get processing removed.
          instance->state &= ~(STATE_PROCESSING | STATE_IS_TRIGGERING);
        else
        {
          if (processing[p][c] & 4) // Restore the previous is-triggering state
          {
            instance->state |= STATE_IS_TRIGGERING;
            processing[p][c] &= ~4;
          }

          if (processing[p][c]) // Newly-created cards (processing[p][c] == 2) and cards that had
                                // processing before (processing[p][c] == 1)
            instance->state |= STATE_PROCESSING;
          else // Unprocessed cards that existed before and after get it removed.
            instance->state &= ~STATE_PROCESSING;
        }
      }
    }
  }
  // End additions

  EXE_DWORD(0x62C180) = old_62C180;
  reason_for_trigger_controller = old_reason_for_trigger_controller;
  EXE_DWORD(0x62BCE8) = old_62BCE8;

#undef TRIGGER_DEPTH
}

int dispatch_trigger(int player, trigger_t trig, const char *prompt, int TENTATIVE_allow_response)
{
  xtrigger_t old_xtrigger = xtrigger_impl_value_dont_use_directly;
  dispatch_trigger_impl(player, trig, 0, prompt, TENTATIVE_allow_response);
  xtrigger_impl_value_dont_use_directly = old_xtrigger;
  return 0;
}

int dispatch_trigger2(int player, trigger_t trig, const char *prompt, int TENTATIVE_allow_response, int new_trigger_cause_controller, int new_trigger_cause)
{
  int old_trigger_cause = trigger_cause;
  int old_trigger_cause_controller = trigger_cause_controller;
  xtrigger_t old_xtrigger = xtrigger_impl_value_dont_use_directly;

  trigger_cause_controller = new_trigger_cause_controller;
  trigger_cause = new_trigger_cause;

  dispatch_trigger_impl(  player, trig, 0, prompt, TENTATIVE_allow_response);
  dispatch_trigger_impl(1-player, trig, 0, prompt, TENTATIVE_allow_response);

  trigger_cause = old_trigger_cause;
  trigger_cause_controller = old_trigger_cause_controller;
  xtrigger_impl_value_dont_use_directly = old_xtrigger;

  return 0;
}

int dispatch_xtrigger2(int player, xtrigger_t xtrig, const char *prompt, int TENTATIVE_allow_response, int new_trigger_cause_controller, int new_trigger_cause)
{
  int old_trigger_cause = trigger_cause;
  int old_trigger_cause_controller = trigger_cause_controller;
  xtrigger_t old_xtrigger = xtrigger_impl_value_dont_use_directly;

  trigger_cause_controller = new_trigger_cause_controller;
  trigger_cause = new_trigger_cause;

  dispatch_trigger_impl(  player, TRIGGER_XTRIGGER, xtrig, prompt, TENTATIVE_allow_response);
  dispatch_trigger_impl(1-player, TRIGGER_XTRIGGER, xtrig, prompt, TENTATIVE_allow_response);

  trigger_cause = old_trigger_cause;
  trigger_cause_controller = old_trigger_cause_controller;
  xtrigger_impl_value_dont_use_directly = old_xtrigger;

  return 0;
}

void resolve_trigger(int player, int card, int TENTATIVE_reason_for_trigger_controller)
{
  uint16_t uVar1;
  uint32_t uVar2;
  unsigned int uVar3;
  xtrigger_t xVar4;
  card_instance_t *pcVar5;
  int iVar6;
  int iVar7;
  uint32_t uVar8;
  card_instance_t *pcVar9;
  unsigned int uVar10;
  int iVar11;
  bool bVar12;
  card_instance_t *pcStack_218;
  int iStack_214;
  int iStack_208;
  char buf[500];

  if (((uint8_t)trace_mode & 2) != 0)
  {
    pcVar5 = get_card_instance(player, card);
    iVar11 = EXE_DWORD(0x60ec40);
    EXE_DWORD(0x60ec40) = EXE_DWORD(0x60ec40) + 1;
    sprintf(buf, "%d: Player #%d is processing %s(%d).\n", iVar11, player,
            cards_data[pcVar5->internal_card_id].name, card);
    EXE_FN(void, 0x4a7d80, const char *)(buf);
  }
  EXE_DWORD(0x60e9f8) = 0;
  put_card_or_activation_onto_stack(player, card, EVENT_RESOLVE_TRIGGER,
                                    TENTATIVE_reason_for_trigger_controller, 0);
  if (spell_fizzled == 1)
  {
    obliterate_top_card_of_stack();
    EXE_DWORD(0x60e9f8) = 0;
    return;
  }

  if (ai_is_speculating != 1)
  {
    EXE_FN(void, 0x436700, void)();
    if (reason_for_trigger_controller != 0)
    {
      load_text((char *)0x0, "PROMPT_PROC1");
      sprintf(buf, text_lines[0], opponent_name);
      EXE_FN(int, 0x471b60, int, int, int, int, const char *, int)(player, card, -1, -1, buf, 0);
    }
    iVar11 = 0;
    if (0 < active_cards_count[0])
    {
      do
      {
        pcVar5 = get_card_instance(0, iVar11);
        iVar6 = active_cards_count[0];
        iVar11 = iVar11 + 1;
        pcVar5->state = pcVar5->state & 0xefffffff;
      } while (iVar11 < iVar6);
    }
    iVar11 = 0;
    if (0 < active_cards_count[1])
    {
      do
      {
        pcVar5 = get_card_instance(1, iVar11);
        iVar6 = active_cards_count[1];
        iVar11 = iVar11 + 1;
        pcVar5->state = pcVar5->state & 0xefffffff;
      } while (iVar11 < iVar6);
    }
  }
  pcVar5 = get_card_instance(player, card);
  uVar3 = trigger_condition;
  pcVar5->state = pcVar5->state | 0x10000100;
  xVar4 = xtrigger_impl_value_dont_use_directly;
  iVar6 = trigger_cause;
  iVar11 = trigger_cause_controller;
  iStack_208 = trigger_cause;
  if ((((pcVar5->token_status & 0x80) == 0) && (pcVar5->kill_code == '\0')) &&
      (-1 < (int)pcVar5->token_status))
  {
    pcVar9 = in_play(player, card);
    uVar10 = trigger_condition;
    if (pcVar9 == (card_instance_t *)0x0)
    {
      bVar12 = true;
      iStack_214 = -1;
      pcStack_218 = (card_instance_t *)0x0;
    }
    else
    {
      iVar7 = pcVar5->internal_card_id;
      if (((((cards_data[iVar7].cc[2] == '\x03') ||
             (uVar1 = cards_data[iVar7].id, (uint16_t)(uVar1 - 0x385) < 8)) ||
            (((uint16_t)(uVar1 - 0x38f) < 0x13 ||
              (((trigger_condition & 0xfffffffb) == 0xd8 ||
                ((unsigned int)trigger_condition - 0xd4 < 2)))))) ||
           (cards_data[iVar7].type == 0x80)) ||
          (((ai_is_speculating == 1 || (player == -1)) || (card == -1))))
      {
        bVar12 = true;
        pcStack_218 = (card_instance_t *)0x0;
        iStack_214 = -1;
      }
      else
      {
        iStack_214 = create_legacy_effect(player, card, empty);
        if (iStack_214 == -1)
        {
          backtrace_and_abort("functions/events.c:633\nstack_card != -1");
        }
        pcStack_218 = get_card_instance(player, iStack_214);
        pcStack_218->state = 0x800022;
        pcStack_218->token_status = 0x80010000;
        pcStack_218->damage_source_player = (int8_t)player;
        pcStack_218->damage_source_card = card;
        uVar8 = pcStack_218->timestamp;
        memcpy(pcStack_218, pcVar5, 300);
        pcStack_218->kill_code = '\0';
        pcStack_218->state = 0x800022;
        uVar2 = pcVar5->internal_card_id;
        pcStack_218->token_status = 0x80010000;
        pcStack_218->original_internal_card_id = uVar2;
        pcStack_218->unknown0x14 = 0;
        if (uVar2 == 0xffffffff)
        {
          pcStack_218->original_internal_card_id = (unsigned int)pcVar5->backup_internal_card_id;
        }
        iVar7 = get_card_image_number((unsigned int)pcStack_218->display_pic_csv_id, player, card);
        pcStack_218->parent_controller = player;
        bVar12 = false;
        pcStack_218->display_pic_num = (uint16_t)iVar7;
        pcStack_218->parent_card = card;
        pcStack_218->timestamp = uVar8;
        uVar10 = trigger_condition;
      }
    }
  }
  else
  {
    bVar12 = true;
    pcStack_218 = (card_instance_t *)0x0;
    iStack_214 = -1;
    uVar10 = uVar3;
  }
  cant_be_responded_to = (unsigned int)(uVar10 == 0xcd) | cant_be_responded_to;
  if (cant_be_responded_to == 0)
  {
    allow_response_to_trigger(player, card);
  }
  trigger_cause_controller = iVar11;
  trigger_cause = iVar6;
  trigger_condition = uVar3;
  xtrigger_impl_value_dont_use_directly = xVar4;
  if ((pcVar5->kill_code == '\0') &&
      (pcVar9 = in_play(player, card), pcVar9 != (card_instance_t *)0x0))
  {
    uVar8 = pcVar5->state;
    if ((uVar8 & 0x40000000) == 0)
    {
    LAB_026cc4fb:
      if (bVar12)
      {
        EXE_FN(void, 0x436740, void)();
        goto LAB_026cc50a;
      }
      bVar12 = iStack_214 != -1;
      if ((pcStack_218->state & 0x40000000) == 0)
      {
        EXE_FN(void, 0x436740, void)();
        if (iStack_214 == -1)
          goto LAB_026cc50a;
        goto LAB_026cc727;
      }
    }
    else
    {
      bVar12 = !bVar12 && iStack_214 != -1;
    }
    if (!bVar12)
      goto LAB_026cc2f2;
  }
  else
  {
    uVar8 = pcVar5->state;
    if ((bVar12) || (iStack_214 == -1))
    {
      if ((uVar8 & 0x40000000) != 0)
        goto LAB_026cc2f2;
      goto LAB_026cc4fb;
    }
    pcVar5->state = uVar8 & 0xeffffeff;
    pcStack_218->state = pcStack_218->state & 0xff7fffdd;
    pcStack_218->token_status = pcStack_218->token_status & 0xfffeffff;
    dispatch_event_to_single_card(player, iStack_214, EVENT_TRIGGER, player, iStack_214);
    iVar6 = stack_size;
    stack_data[stack_size].internal_card_id = (short)pcStack_218->internal_card_id;
    stack_data[iVar6].generating_event = 0x7e;
    stack_data[iVar6].unknown_sdt = (uint8_t)TENTATIVE_reason_for_trigger_controller;
    stack_cards[iVar6].player = player;
    stack_cards[iVar6].card = iStack_214;
    stack_damage_targets[iVar6].player = (int)pcVar5->damage_target_player;
    stack_damage_targets[iVar6].card = pcVar5->damage_target_card;
    uVar10 = trigger_condition;
    if (trigger_condition == 0xffffffff)
    {
      uVar10 = current_phase;
    }
    bVar12 = ai_is_speculating != 1;
    stack_phase[iVar6] = uVar10;
    if (bVar12)
    {
      EXE_DWORD_PTR(0x628674)[iVar6] = 0;
      iVar6 = stack_size;
    }
    stack_size = iVar6 + 1;
    stack_cards[stack_size].player = -1;
    if (affected_card == card)
    {
      affected_card = iStack_214;
    }
    if (trigger_cause == card)
    {
      trigger_cause = iStack_214;
      iStack_208 = trigger_cause;
    }
    bVar12 = cant_be_responded_to == 0;
    pcStack_218->state = pcStack_218->state | 0x10000100;
    if (bVar12)
    {
      allow_response_to_trigger(player, iStack_214);
    }
    trigger_cause = iStack_208;
    trigger_cause_controller = iVar11;
    trigger_condition = uVar3;
    xtrigger_impl_value_dont_use_directly = xVar4;
    if (((pcVar5->state & 0x40000000) == 0) && ((pcStack_218->state & 0x40000000) == 0))
    {
      EXE_FN(void, 0x436740, void)();
    LAB_026cc727:
      obliterate_card_and_recycle(player, iStack_214);
    LAB_026cc50a:
      xtrigger_impl_value_dont_use_directly = xVar4;
      pcVar5->state = pcVar5->state & 0xefffffff;
      trigger_cause = iStack_208;
      EXE_DWORD(0x60e9f8) = 0;
      trigger_cause_controller = iVar11;
      trigger_condition = uVar3;
      dispatch_event(player, card, EVENT_TRIGGER_RESOLVED);
      trigger_condition = uVar3;
      xtrigger_impl_value_dont_use_directly = xVar4;
      trigger_cause = iStack_208;
      trigger_cause_controller = iVar11;
      return;
    }
  }
  obliterate_card_and_recycle(player, iStack_214);
  uVar8 = pcVar5->state;
LAB_026cc2f2:
  trigger_cause = iStack_208;
  xtrigger_impl_value_dont_use_directly = xVar4;
  pcVar5->state = uVar8 & 0xafffffff;
  trigger_condition = uVar3;
  trigger_cause_controller = iVar11;
  return;
}

int get_abilities(int player, int card, event_t event, int new_attacking_card)
{
  uint8_t uVar1;
  uint16_t uVar2;
  card_ext_t *pcVar3;
  card_instance_t *inst;
  int iVar4;
  unsigned int uVar5;
  unsigned int uVar6;
  int iVar7;
  card_instance_t *pcVar8;
  unsigned int uVar9;
  uint8_t bVar10;
  uint32_t uVar11;
  uint32_t uVar12;
  unsigned int uVar13;
  bool bVar14;
  kill_t kill_mode;

  while (true)
  {
    inst = get_card_instance(player, card);
    uVar5 = inst->internal_card_id;
    if (activation_card != uVar5)
      break;
    if (event == EVENT_CHANGE_TYPE)
    {
      return uVar5;
    }
    card = inst->parent_card;
    player = inst->parent_controller;
  }
  if (EXE_DWORD(0x60a548) != 0)
  {
    push_affected_card_stack();
  }
  affected_card_controller = player;
  affected_card = card;
  if (uVar5 == 0xffffffff)
  {
    uVar5 = (unsigned int)inst->backup_internal_card_id;
    inst->regen_status = inst->regen_status & 0xe0ffffff;
  }
  iVar4 = raw_base_pt;
  attacking_card = new_attacking_card;
  switch (event)
  {
  case EVENT_POWER:
    if ((inst->regen_status & 0x4000000) != 0)
    {
      inst->regen_status = inst->regen_status & 0xfbffffff;
      uVar6 = get_special_flags3_by_instance(inst);
      if ((uVar6 & 0x100000) != 0)
      {
        uVar11 = inst->state;
        raw_characteristic_pt = (int)cards_data[uVar5].toughness;
        raw_base_pt = raw_characteristic_pt;
        if (((uVar11 & 0x800002) == 2) && (0 < raw_characteristic_pt))
        {
          raw_base_pt = (int)(short)(cards_data[uVar5].toughness & 0xbfff);
        }
        event_result = raw_base_pt + inst->counter_toughness;
        if (EXE_DWORD(0x60a548) == 0)
          goto code_r0x026cc906;
        event = EVENT_TOUGHNESS;
      LAB_026ccfcf:
        dispatch_event_raw(event);
        if (((land_can_be_played & 0x10000U) != 0) && (event == EVENT_CHANGE_TYPE))
        {
        LAB_026ccbf2:
          iVar7 = event_result;
          inst->internal_card_id = event_result;
          if (iVar7 != -1)
          {
            inst->backup_internal_card_id = (uint16_t)iVar7;
          }
          land_can_be_played = land_can_be_played & 0xfffeffffU | 0x20000;
          dispatch_event_raw(EVENT_CHANGE_TYPE);
          land_can_be_played = land_can_be_played & 0xfffdffff;
          goto LAB_026cc8ef;
        }
        goto LAB_026cc8df;
      }
      raw_characteristic_pt = (int)cards_data[uVar5].power;
      raw_base_pt = raw_characteristic_pt;
      if (((inst->state & 0x800002) == 2) && (0 < raw_characteristic_pt))
      {
        raw_base_pt = (int)(short)(cards_data[uVar5].power & 0xbfff);
      }
      event_result = inst->counter_power + raw_base_pt;
      if (EXE_DWORD(0x60a548) != 0)
        goto LAB_026ccfcf;
      goto LAB_026cd0d9;
    }
    uVar6 = (int)inst->power;
    break;
  case EVENT_TOUGHNESS:
    if ((inst->regen_status & 0x2000000) != 0)
    {
      inst->regen_status = inst->regen_status & 0xfdffffff;
      uVar6 = get_special_flags3_by_instance(inst);
      if ((uVar6 & 0x100000) == 0)
      {
        raw_characteristic_pt = (int)cards_data[uVar5].toughness;
        raw_base_pt = raw_characteristic_pt;
        if (((inst->state & 0x800002) == 2) && (0 < raw_characteristic_pt))
        {
          raw_base_pt = (int)(short)(cards_data[uVar5].toughness & 0xbfff);
        }
        event_result = inst->counter_toughness + raw_base_pt;
        if (EXE_DWORD(0x60a548) == 0)
          goto code_r0x026cc8f9;
      }
      else
      {
        raw_characteristic_pt = (int)cards_data[uVar5].power;
        raw_base_pt = raw_characteristic_pt;
        if (((inst->state & 0x800002) == 2) && (0 < raw_characteristic_pt))
        {
          raw_base_pt = (int)(short)(cards_data[uVar5].power & 0xbfff);
        }
        event_result = inst->counter_power + raw_base_pt;
        if (EXE_DWORD(0x60a548) == 0)
          goto LAB_026cd0d9;
        event = EVENT_POWER;
      }
      goto LAB_026ccfcf;
    }
    event_result = (int)inst->toughness;
    uVar11 = inst->state;
  code_r0x026cc906:
    uVar12 = event_result;
    if (((((uVar11 & 0x800022) == 2) && ((cards_data[uVar5].type & 2) != 0)) &&
         ((event_result < 1 || (event_result <= (int)(unsigned int)inst->damage_on_card)))) &&
        (((inst->token_status & 0x4000000) == 0 &&
          ((land_can_be_played & 0x204U) == 0 && EXE_DWORD(0x785e60) == 0))))
    {
      if (event_result < 1)
      {
        kill_mode = KILL_STATE_BASED_ACTION;
      }
      else
      {
        kill_mode = KILL_DESTROY;
      }
      kill_card(player, card, kill_mode);
      EXE_FN(void, 0x477a90, void)();
    }
    if (EXE_DWORD(0x60a548) != 0)
    {
      pop_affected_card_stack();
    }
    goto LAB_026cc92c;
  case EVENT_ABILITIES:
    uVar13 = inst->regen_status;
    uVar6 = uVar13;
    if ((uVar13 & 0x8000000) != 0)
    {
      bVar14 = damage_card != uVar5;
      inst->regen_status = uVar13 & 0xf7ffffff;
      uVar6 = uVar13 & 0xf7ffffff;
      if (bVar14)
      {
        uVar6 = cards_data[uVar5].static_ability;
        uVar13 = uVar13 & 0x17000000 | uVar6;
        if ((uVar6 & 0xf81f) != 0)
        {
          if ((uVar6 & 0xf800) != 0)
          {
            uVar9 = get_sleighted_protection(player, card, uVar6 & 0xf800);
            uVar13 = ((uVar13 & 0xffff0000U) | (uVar6 & 0xffffU)) & 0xffff07ff | uVar9;
          }
          if ((uVar13 & 0x1f) != 0)
          {
            uVar6 = get_hacked_walk(player, card, uVar13 & 0x1f);
            uVar13 = uVar13 & 0xffffffe0 | uVar6;
          }
        }

        if (((EXE_DWORD(0x60a548) != 0) && ((cards_data[uVar5].type & 0x47) != 0)) &&
            (-1 < (char)cards_data[uVar5].type))
        {
          pcVar3 = inst->ext;
          pcVar3->sp_keywords = 0;
          pcVar3->sp_keywords2 = 0;
          inst->targets[0x11].card = inst->targets[0x11].card & 0xffffff81;
          inst->state = inst->state & 0xfffddfff;
          inst->token_status = inst->token_status & 0xffff77ff;
          remove_special_flags(player, card, 0x28000);
          remove_special_flags2(player, card, 0x1000);
          remove_special_flags2(player, card, 0x10);
        }
        event_result = uVar13;
        if (EXE_DWORD(0x60a548) != 0)
          goto LAB_026ccfcf;
        goto LAB_026ccce2;
      }
    }
    break;
  case EVENT_RECALC_DAMAGE:
    event_result = (unsigned int)inst->damage_on_card;
    uVar6 = (unsigned int)inst->damage_on_card;
    if (EXE_DWORD(0x60a548) != 0)
      goto LAB_026ccfcf;
    break;
  case EVENT_SET_COLOR:
    if (((inst->regen_status & 0x10000000) == 0) ||
        (bVar14 = damage_card == uVar5, inst->regen_status = inst->regen_status & 0xefffffff,
         bVar14))
    {
      uVar6 = (int)inst->color;
    }
    else
    {
      bVar10 = inst->initial_color;
      if (bVar10 == 0)
      {
        if ((cards_data[uVar5].type & 1) == 0)
        {
          if (((((cards_data[uVar5].type & 0x40) == 0) ||
                ((cards_data[uVar5].extra_ability & 0x1000) == 0)) ||
               (iVar7 = get_internal_card_id_from_csv_id((unsigned int)cards_data[uVar5].id),
                (cards_data[iVar7].type & 0x40) == 0)) ||
              ((cards_data[iVar7].extra_ability & 0x1000) == 0))
          {
            bVar10 = cards_data[uVar5].color;
            goto code_r0x026cd446;
          }
        }
        else if (cards_data[uVar5].id == 0x22d4)
        {
          event_result = 8;
          goto code_r0x026cc995;
        }
        event_result = 0;
      }
      else
      {
      code_r0x026cd446:
        event_result = bVar10 & 0x3e;
      }
    code_r0x026cc995:
      if (EXE_DWORD(0x60a548) != 0)
        goto LAB_026ccfcf;
    code_r0x026cc8f9:
      uVar6 = event_result;
      if (event == EVENT_TOUGHNESS)
      {
        uVar11 = inst->state;
        goto code_r0x026cc906;
      }
    }
    break;
  default:
    event_result = 0;
    if (EXE_DWORD(0x60a548) != 0)
      goto LAB_026ccfcf;
  LAB_026cc8df:
    if (event == EVENT_ABILITIES)
    {
    LAB_026ccce2:

      pcVar8 = get_card_instance(player, card);
      if (((((((uint8_t)(pcVar8->special_counter_type + 0x70) < 0xb) &&
              (pcVar8->special_counters != '\0')) ||
             (((uint8_t)((char)pcVar8->targets[0x12].player + 0x70U) < 0xb &&
               (pcVar8->counters2 != '\0')))) ||
            ((((uint8_t)(*(char *)((int)&pcVar8->targets[0x12].player + 1) + 0x70U) < 0xb &&
               (pcVar8->counters3 != '\0')) ||
              (((uint8_t)(*(char *)((int)&pcVar8->targets[0x12].player + 2) + 0x70U) < 0xb &&
                (pcVar8->counters4 != '\0')))))) ||
           ((((uint8_t)(*(char *)((int)&pcVar8->targets[0x12].player + 3) + 0x70U) < 0xb &&
              (pcVar8->counters5 != '\0')) ||
             (((uint8_t)(*(char *)((int)&pcVar8->targets[0x12].card + 2) + 0x70U) < 0xb &&
               ((char)pcVar8->targets[0x12].card != '\0')))))) ||
          (((uint8_t)(*(char *)((int)&pcVar8->targets[0x12].card + 3) + 0x70U) < 0xb &&
            (*(char *)((int)&pcVar8->targets[0x12].card + 1) != '\0'))))
      {
        if (((cards_data[uVar5].type & 0x47) != 0) && (-1 < (char)cards_data[uVar5].type))
        {
          iVar7 = count_counters(player, card, COUNTER_DEATHTOUCH);
          if (iVar7 != 0)
          {
            (inst->ext)->sp_keywords = (inst->ext)->sp_keywords | 0x200;
          }
          iVar7 = count_counters(player, card, COUNTER_HEXPROOF);
          if (iVar7 != 0)
          {
            (inst->ext)->sp_keywords = (inst->ext)->sp_keywords | 0x800000;
          }
          iVar7 = count_counters(player, card, COUNTER_INDESTRUCTIBLE);
          if (iVar7 != 0)
          {
            (inst->ext)->sp_keywords = (inst->ext)->sp_keywords | 0x800;
          }
          iVar7 = count_counters(player, card, COUNTER_LIFELINK);
          if (iVar7 != 0)
          {
            (inst->ext)->sp_keywords = (inst->ext)->sp_keywords | 2;
          }
          iVar7 = count_counters(player, card, COUNTER_MENACE);
          if (iVar7 != 0)
          {
            (inst->ext)->sp_keywords = (inst->ext)->sp_keywords | 0x10000;
          }
          iVar7 = count_counters(player, card, COUNTER_VIGILANCE);
          if (iVar7 != 0)
          {
            (inst->ext)->sp_keywords = (inst->ext)->sp_keywords | 8;
          }
          iVar7 = count_counters(player, card, COUNTER_DOUBLE_STRIKE);
          if (iVar7 != 0)
          {
            event_result = event_result | 0x40000000;
          }
          iVar7 = count_counters(player, card, COUNTER_FIRST_STRIKE);
          if (iVar7 != 0)
          {
            event_result = event_result | 0x100;
          }
          iVar7 = count_counters(player, card, COUNTER_FLYING);
          if (iVar7 != 0)
          {
            event_result = event_result | 0x20;
          }
          iVar7 = count_counters(player, card, COUNTER_REACH);
          if (iVar7 != 0)
          {
            event_result = event_result | 0x400;
          }
          iVar7 = count_counters(player, card, COUNTER_TRAMPLE);
          if (iVar7 != 0)
          {
            event_result = event_result | 0x80;
          }
          goto LAB_026ccd94;
        }
      }
      else
      {
      LAB_026ccd94:
        uVar6 = (unsigned int)player_bits[player];

        if ((((uVar6 & 0x1001f0) != 0) && ((cards_data[uVar5].type & 0x47) != 0)) &&
            (-1 < (char)cards_data[uVar5].type))
        {
          if ((uVar6 & 0x10) != 0)
          {
            event_result = event_result & 0xfffffeff;
          }
          if ((uVar6 & 0x20) != 0)
          {
            event_result = event_result & 0xffffffdf;
          }
          if ((uVar6 & 0x40) != 0)
          {
            (inst->ext)->sp_keywords = (inst->ext)->sp_keywords & 0xfffffdff;
          }
          if ((uVar6 & 0x80) != 0)
          {
            event_result = event_result & 0xffffff7f;
          }
          if ((uVar6 & 0x100000) != 0)
          {
            event_result = event_result & 0xfffdffff;
          }
          if ((uVar6 & 0x100) != 0)
          {
            (inst->ext)->sp_keywords = (inst->ext)->sp_keywords & 0xff7fffff;
          }
        }
      }
      iVar7 = is_humiliated(player, card, AL_KEY);
      if (iVar7 != 0)
      {
        iVar7 = is_humiliated(player, card, AL_DEFENDER);
        if (iVar7 != 0)
        {
          event_result = event_result | 0x800000;
        }
        iVar7 = is_humiliated(player, card, AL_INDESTRUCTIBLE);
        if (iVar7 != 0)
        {
          (inst->ext)->sp_keywords = (inst->ext)->sp_keywords | 0x800;
        }
      }
      if (((inst->state & 0x10000) != 0) && (((inst->ext)->sp_keywords & 4) == 0))
      {
        inst->state = inst->state | 0x20000;
      }
      event = EVENT_ABILITIES;
      uVar6 = event_result;
    }
    else
    {
    LAB_026cc8ef:
      if (event != EVENT_POWER)
        goto code_r0x026cc8f9;
    LAB_026cd0d9:
      if (event_result < 0)
      {
        event_result = 0;
      }
      event = EVENT_POWER;
      uVar6 = event_result;
      if ((inst->token_status & 0x4000) != 0)
      {
        uVar6 = event_result * 2;
      }
    }
    break;
  case EVENT_CHANGE_TYPE:
    if (inst->internal_card_id == -1)
    {
      event_result = -1;
      uVar6 = event_result;
    }
    else
    {
      uVar6 = uVar5;
      if (((inst->regen_status & 0x1000000) != 0) &&
          (((int)uVar5 < EXE_DWORD(0x628c64) || (EXE_DWORD(0x786d5c) < (int)uVar5))))
      {
        uVar12 = inst->original_internal_card_id;
        inst->regen_status = inst->regen_status & 0xfeffffff;
        inst->internal_card_id = uVar12;
        inst->backup_internal_card_id = (uint16_t)uVar12;
        uVar1 = cards_data[uVar12].color;
        inst->destroys_if_blocked = '\0';
        inst->mana_color = uVar1;
        inst->token_status = inst->token_status & 0xffffefff;
        remove_special_flags2(player, card, 0x484);
        iVar7 = is_what(player, card, 0x80);
        if (iVar7 == 0)
        {
          reset_subtypes_from_instance(inst, uVar5);
        }
        event_result = uVar12;
        if (EXE_DWORD(0x60a548) == 0)
          goto code_r0x026cc9f0;
        dispatch_event_raw(EVENT_CHANGE_TYPE);
        if ((land_can_be_played & 0x10000U) != 0)
          goto LAB_026ccbf2;
        goto LAB_026cc8ef;
      }
    }
  }
  event_result = uVar6;
  uVar12 = event_result;
  if (EXE_DWORD(0x60a548) != 0)
  {
    pop_affected_card_stack();
  }
  if (event == EVENT_CHANGE_TYPE)
  {
  code_r0x026cc9f0:
    inst->internal_card_id = uVar12;
    if (uVar12 != 0xffffffff)
    {
      inst->backup_internal_card_id = (uint16_t)uVar12;
    }
    if (((cards_data[uVar12].extra_ability & 0x1000) != 0) ||
        ((iVar4 = is_what(player, card, 1), iVar4 != 0 && (mana_colors_added[player] != 0))))
    {
      uVar2 = cards_data[uVar12].id;
      uVar5 = get_color_of_mana_produced_by_id((unsigned int)uVar2, inst->info_slot, player);
      if (uVar5 == 0xffffffff)
      {
        uVar5 = (unsigned int)(uint8_t)cards_ptr[uVar2]->mana_source_colors;
      }
      inst->mana_color = (uint8_t)uVar5;
      iVar4 = is_what(player, card, 1);
      if (iVar4 != 0)
      {
        inst->mana_color = inst->mana_color | (uint8_t)mana_colors_added[player];
      }
      iVar4 = check_special_flags2(player, card, 0x40);
      bVar10 = inst->mana_color;
      uVar5 = (unsigned int)bVar10;
      if (iVar4 != 0)
      {
        iVar4 = 0;
        do
        {
          uVar6 = color_to_color_test(iVar4);
          if ((uVar5 & 0xff & uVar6) == 0)
          {
            uVar5 = (unsigned int)inst->mana_color;
          }
          else
          {
            iVar7 = color_to_color_test(iVar4);
            inst->mana_color = inst->mana_color & ((uint8_t)iVar7 ^ 0xff);
            uVar5 = color_to_color_test(mana_color_override[player][iVar4]);
            uVar6 = (unsigned int)inst->mana_color | ((unsigned int)inst->card_color << 8) |
                    ((unsigned int)inst->unk5a << 16) | ((unsigned int)inst->unk5b << 24);
            uVar5 = uVar5 | uVar6;
            inst->mana_color = (uint8_t)uVar5;
          }
          iVar4 = iVar4 + 1;
        } while (iVar4 != 7);
        bVar10 = (uint8_t)uVar5;
      }
      inst->card_color = bVar10;
    }
    if (((inst->token_status & 0x40) != 0) && ((cards_data[uVar12].type & 2) == 0))
    {
      iVar4 = 0;
      do
      {
        iVar7 = 0;
        if (-1 < active_cards_count[iVar4])
        {
          do
          {
            pcVar8 = in_play(iVar4, iVar7);

            if ((((pcVar8 != (card_instance_t *)0x0) && (pcVar8 = get_card_instance(iVar4, iVar7),
                                                         pcVar8->damage_target_player == player)) &&
                 (pcVar8->damage_target_card == card)) &&
                (((cards_data[pcVar8->internal_card_id].type & 4) != 0 &&
                  (cards_ptr[cards_data[pcVar8->internal_card_id].id]->subtype1 == 0x2c))))
            {
              kill_card(iVar4, iVar7, KILL_STATE_BASED_ACTION);
            }
            iVar7 = iVar7 + 1;
          } while (iVar7 <= active_cards_count[iVar4]);
        }
        iVar4 = iVar4 + 1;
      } while (iVar4 != 2);
    }
    if ((inst->state & 2) == 0)
    {
      return uVar12;
    }
    event_flags = event_flags | cards_data[inst->internal_card_id].extra_ability & 0x1ffc0000;
    return uVar12;
  }
  if (event == EVENT_ABILITIES)
  {
    inst->regen_status = uVar12;

    if (((((inst->ext)->sp_keywords & 0x200) != 0) && ((cards_data[uVar5].type & 0x47) != 0)) &&
        (-1 < (char)cards_data[uVar5].type))
    {
      inst->destroys_if_blocked = inst->destroys_if_blocked | 0x10;
    }
  }
  else if (event < EVENT_RECALC_DAMAGE)
  {
    if (event == EVENT_POWER)
    {
      inst->power = (int16_t)uVar12;
    }
    else if (event == EVENT_TOUGHNESS)
    {
    LAB_026cc92c:
      inst->toughness = (int16_t)uVar12;
    }
  }
  else if (event == EVENT_SET_COLOR)
  {
    inst->color = (uint8_t)uVar12 & 0x3e;
  }
  raw_base_pt = iVar4;
  return uVar12;
}

// Fixes a bug in the exe.  Do not call directly.
void event_activate_then_duplicate_into_stack(int player, int card, int event, int new_attacking_card_controller, int new_attacking_card)
{
  /* Normally, when a card is activated, 12 steps happen in activate() (at 0x434040) and two more happen elsewhere:
   * 1) If it's the AI playing the activation, do some unclear proprocessing, including (among much else) sending EVENT_GET_SELECTED_CARD to player/card and
   *    showing a dialog announcing the activation.
   * 2) an activation card is put onto the stack (put_card_or_activation_onto_stack() at 0x436550).  This involves:
   *   a) creating an activation card
   *   b) memcpy()ing the card as it currently exists into the activation card
   *   c) setting the activation card's internal_card_id (back to iid_activation_card), original_internal_card_id (to the creating card's internal_card_id),
   *      unknown0x14 and kill_code (both to 0), timestamp (back to what it was before the memcpy), and parent_controller and parent_card
   *   d) setting various stack variables (stack_size, stack_cards[], stack_damage_targets[], stack_phase[], and dword_628674[stack_size].
   *   For basic lands and for EVENT_RESOLVE_SPELL and EVENT_RESOLVE_TRIGGER, a, b, and c are skipped; a references to actual card is left on the stack.
   * 3) EVENT_UNKNOWN80 is dispatched, setting affected_card_controller/affected_card to player/card.  If event_result is set to nonzero, the top card of the
   *    stack is obliterated, and nothing else is done.  Nothing in the exe handles this event, and it seems dangerous to me - what if the AI keeps on trying
   *    to activate a card, and something responding to EVENT_UNKNOWN80 keeps cancelling it?
   * 4) Set tapped_for_mana_color = -1, and add an entry to the mana_spent stack (sub_430150)
   * 5) EVENT_ACTIVATE is dispatched to player/card (not the activation card) with dispatch_event_to_single_card() at 0x435B50 (almost the same as
   *    dispatch_event_with_attacker_to_one_card() above)
   * 6) If it's a network game and the remote player is activating, then (usually) show a dialog saying what he's doing.
   * 7) If cancel == 1, refund all mana spent since the mana_spent stack state was stored (sub_430200), obliterate the top card of the stack, and return.
   * 8) Remove the mana_stack entry (so cancelling afterwards won't refund mana spent so far) (sub_4301E0)
   * 9) Dispatch either EVENT_PLAY_ABILITY or both EVENT_TAP_CARD and EVENT_TAPPED_TO_PLAY_ABILITY, with affected_card_controller/affected_card == player/card.
   *10) Copy player/card's current state into the activation card again, preserving the same things as before; and set the activation card's display_pic_csv_id
   *    to the original card's csvid and display_pic_num to 0.  (Digression: The original_internal_card_id saved when the activation_card was put on the stack
   *    is copied anew from player/card here, instead of preserving it; if player/card's internal_card_id is now -1 since it's no longer in play due to
   *    sacrificing or exiling or bouncing itself during activation or anything else that immediately gets rid of it, then it's pulled from player/card's
   *    original_internal_card.  This is why sacrifice-self abilities from Clone etc. get screwed up.)  Also set dword_628674[] and stack_damage_targets[] again
   *    here.  This happens in recopy_card_onto_stack().  If the top card on the stack wasn't an activation card (see the note at the end of step 2), then none
   *    of this except setting dword_628674[] and stack_damage_targets[] is done.
   *11) Play WAV_TAP if the card is flagged EA_MANA_SOURCE and tapped_for_mana_color != -1; otherwise, play WAV_FASTFX.
   *12) And call set_stack_damage_targets() at 0x436700, which updates stack_damage_targets[] for everything on the stack.  I suspect it's redundant.
   *13) Outside of activate(), (usually) allow response.
   *14) Eventually, resolve_top_card_on_stack() at 0x436740 is called.  This puts the stack down to the point it was at when this instance of activate() was
   *    called.
   *
   * When a mana source is activated during charge_mana(), an abbreviated sequence occurs:
   * 1) Send EVENT_CAN_ACTIVATE to the card, unless it's a land.  (I think that exception should be removed, but it's a problem for another day.)  On failure,
   *    do nothing else.
   * 2) Same as step 2 from above - activation card onto the stack.
   * 3) Store the colors of mana currently needed in needed_mana_colors.
   * 4) set tapped_for_mana_color = -1.
   * 5) Dispatch EVENT_ACTIVATE, just as in step 5 from above.
   * 6) (nothing)
   * 7) If cancel == 1, obliterate the top card of the stack, and continue onto the rest of charge_mana().
   * 8) (nothing)
   * 9) If the activating card is newly tapped, dispatch EVENT_TAP_CARD, otherwise do nothing.
   *10) (nothing)
   *11) Play WAV_TAP.
   *12) (nothing)
   *13) (nothing)
   *14) call resolve_top_card_on_stack() at 0x436740 and TENTATIVE_reassess_all_cards() at 0x472260.
   *
   * The sequence during autotap_mana_source() at 0x42FED0 is similar, though it at least always checks EVENT_CAN_ACTIVATE.
   *
   * The bug is that step 10 is missing - if a card sets its internal targets or info_slot during EVENT_ACTIVATE, and its EVENT_RESOLVE_ACTIVATION looks at
   * them, it'll see them as they were *before* EVENT_ACTIVATE was called.  This breaks mana sources that have non-mana abilities.  I'm currently seeing it with
   * Iceberg - if you activate it to add an ice counter (which takes place at resolution), and then cast something and pay for its mana with Iceberg's mana
   * ability, you get a free counter.  I'm mostly just amazed that it hasn't seemed to occur on lands with non-mana abilities.
   *
   * So what I'm doing is replacing the call to dispatch_event_to_single_card(player, card, EVENT_ACTIVATE, 1-player, -1) with a call here, ignoring the last
   * three parameters.  Here, we dispatch EVENT_ACTIVATE, then manually duplicate most of recopy_card_onto_stack() from step 10 in the full sequence. */

  dispatch_event_with_attacker_to_one_card(player, card, event, new_attacking_card_controller, new_attacking_card);

  // 0x436450
  int idx = stack_size - 1;
  card_instance_t* instance = get_card_instance(stack_cards[idx].player, stack_cards[idx].card);
  if (instance->internal_card_id == activation_card)
	{
	  int old_parent_card = instance->parent_card;
	  int old_parent_controller = instance->parent_controller;
	  uint32_t old_timestamp = instance->timestamp;
	  card_instance_t* parent_instance = get_card_instance(instance->parent_controller, instance->parent_card);

	  if (parent_instance->state & STATE_DONT_RECOPY_ONTO_STACK)
		parent_instance->state &= ~STATE_DONT_RECOPY_ONTO_STACK;
	  else
		{
		  STATIC_ASSERT(sizeof(card_instance_t) == 300, card_instance_t_size_differs_from_exe);
		  memcpy(instance, parent_instance, sizeof(card_instance_t));
		  instance->internal_card_id = activation_card;
		  instance->unknown0x14 = 0;
		  instance->kill_code = 0;
		  instance->state |= STATE_IN_PLAY;
		  instance->parent_controller = old_parent_controller;
		  instance->parent_card = old_parent_card;
		  instance->timestamp = old_timestamp;

		  if (parent_instance->internal_card_id != -1)
			instance->original_internal_card_id = parent_instance->internal_card_id;
		  // Begin additions
		  else
			instance->original_internal_card_id = parent_instance->backup_internal_card_id;
		  // End additions

		  // Begin removals
		  // if (instance->original_internal_card_id < iid_damage_card
		  //     || instance->original_internal_card_id >= iid_damage_card + 47)
		  //   {
		  //     instance->display_pic_csv_id = cards_data[parent_instance->original_internal_card_id].id;
		  //     instance->display_pic_num = 0;
		  //   }
		  // End removals
		}

	  // Begin additions
	  instance->token_status &= ~STATUS_DYING;	// so kill_card() will reap it, even if the original card sacrificed itself as an activation card
	  // End additions
	}
  // Begin removals
  // if (ai_is_speculating != 1)
  //   dword_628674[idx] = arg1;	// 0 for EA_MANA_SOURCE cards that set tapped_for_mana_color; else 1
  // stack_damage_targets[idx].player = instance->damage_target_player;
  // stack_damage_targets[idx].card = instance->damage_target_card;
  // End removals
}

int current_trigger_or_event_is_forced(void)
{
  // 0x476F10

  /* I'm not positive what this function's exact effects are, but if a trigger that's not listed here runs, and the player has a stop on the current phase, he's
   * always prompted to select "Done" even if there's only one (forced) trigger handler, or none at all. */

  uint8_t ctoe = EXE_DWORD(0x62bce8) & 0xff;	// TENTATIVE_current_trigger_or_event
  if (ctoe == EVENT_UNKNOWN8E)
	return must_attack ? 1 : 0;

  if (ctoe != EVENT_CAN_SKIP_TURN
	  && ctoe != 107	//0x6B or -148 - I can't find any uses.
	  && ctoe != EVENT_CAST_SPELL
	  && ctoe != EVENT_ACTIVATE
	  && ctoe != EVENT_DEAL_DAMAGE
	  && ctoe != 111	//0x6F or -144 - I can't find any uses.
	  && ctoe != EVENT_REGENERATE
	  && ctoe != EVENT_RESOLVE_SPELL
	  && ctoe != EVENT_RESOLVE_ACTIVATION
	  && ctoe != EVENT_CAN_ACTIVATE
	  && ctoe != EVENT_CAN_CAST
	  && ctoe != 117	//0x75 or -138 - I can't find any uses.
	  && ctoe != 118	//0x76 or -137 - I can't find any uses.
	  && ctoe != EVENT_GRAVEYARD_FROM_PLAY
	  && ctoe != EVENT_BLOCK_LEGALITY
	  && ctoe != EVENT_ATTACK_LEGALITY
	  && ctoe != 122	//0x7A or -133 - I can't find any uses.
	  && ctoe != 123	//0x7B or -132 - I can't find any uses.
	  && ctoe != 124	//0x7C or -131 - I can't find any uses.
	  && ctoe != EVENT_TRIGGER
	  && ctoe != EVENT_RESOLVE_TRIGGER
	  && ctoe != EVENT_COUNT_MANA
	  && ctoe != EVENT_UNKNOWN80
	  && ctoe != EVENT_TAP_CARD
	  && ctoe != EVENT_UNTAP
	  && ctoe != EVENT_UNTAP_CARD
	  && ctoe != EVENT_SET_UNTAP_COST
	  && ctoe != EVENT_SETUP_UPKEEP_COSTS
	  && ctoe != EVENT_UPKEEP_COSTS_UNPAID
	  && ctoe != EVENT_CHECK_UPK_PAYMENT
	  && ctoe != EVENT_CHECK_UNTAP_PAYMENT
	  && ctoe != EVENT_MUST_ATTACK
	  && ctoe != EVENT_UNKNOWN8E
	  && ctoe != EVENT_SHOULD_AI_PLAY
	  && ctoe != 200	//0xC8 or -55 - I didn't even attempt to search.
	  && ctoe != TRIGGER_UPKEEP
	  && ctoe != TRIGGER_DURING_UPKEEP
	  && ctoe != TRIGGER_END_UPKEEP
	  && ctoe != TRIGGER_END_COMBAT
	  && ctoe != TRIGGER_EOT
	  && ctoe != TRIGGER_DRAW_PHASE
	  && ctoe != TRIGGER_REPLACE_CARD_DRAW
	  && ctoe != TRIGGER_CARD_DRAWN
	  && ctoe != TRIGGER_DISCARD
	  && ctoe != TRIGGER_TAP_CARD
	  && ctoe != TRIGGER_SPELL_CAST
	  && ctoe != TRIGGER_LEAVE_PLAY
	  && ctoe != TRIGGER_GRAVEYARD_FROM_PLAY
	  && ctoe != TRIGGER_GRAVEYARD_ORDER
	  && ctoe != TRIGGER_DEAL_DAMAGE
	  && ctoe != TRIGGER_BOUNCE_PERMANENT
	  && ctoe != TRIGGER_MUST_ATTACK
	  && ctoe != TRIGGER_COMES_INTO_PLAY
	  && ctoe != TRIGGER_PAY_TO_ATTACK
	  && ctoe != TRIGGER_XTRIGGER
	  )
	return 1;

  return 0;
}

/* Recovered October 2022 implementations; see docs/recovery-progress.md. */
void set_base_pt(int newval);
void set_characteristic_pt(int newval);

/* ManalinkEh 2022: 0x26cd650; original functions/events.c:1190. */
void set_base_pt(int newval)
{
  int result;

  result = raw_base_pt;
  raw_base_pt = newval;
  raw_characteristic_pt = raw_characteristic_pt & 0xffffbfff;
  event_result = event_result + (newval - result);
  return;
}

/* ManalinkEh 2022: 0x26cd690; original functions/events.c:1200. */
void set_characteristic_pt(int newval)
{
  if ((((unsigned int)raw_characteristic_pt >> 8 & 0xff) & 0x40) != 0)
  {
    event_result = event_result + (newval - raw_base_pt);
    raw_base_pt = newval;
  }
  return;
}


/* ManalinkEh 2022: 0x26cb5b0; original functions/events.c:223. */
int dispatch_event_arbitrary_to_one_card_with_initial_event_result(
    int player, int card, event_t event, int new_affected_card_controller, int new_affected_card,
    int new_attacking_card_controller, int new_attacking_card, int initial_event_result)
{
  int result;
  uint32_t uVar2;

  push_affected_card_stack();
  uVar2 = trigger_condition;
  event_result = initial_event_result;
  affected_card_controller = new_affected_card_controller;
  affected_card = new_affected_card;
  attacking_card_controller = new_attacking_card_controller;
  attacking_card = new_attacking_card;
  if ((1 < event - EVENT_TRIGGER) && (event != EVENT_END_TRIGGER))
  {
    trigger_condition = 0xffffffff;
  }
  event_rval = call_card_function(player, card, event);
  result = event_result;
  trigger_condition = uVar2;
  pop_affected_card_stack();
  return result;
}


/* ManalinkEh 2022: 0x26cbdb0; original functions/events.c:155. */
int dispatch_event_with_attacker_and_initial_event_result(int new_affected_card_controller,
                                                          int new_affected_card, event_t event,
                                                          int new_attacking_card_controller,
                                                          int new_attacking_card,
                                                          int initial_event_result)
{
  int result;
  uint32_t uVar2;

  push_affected_card_stack();
  uVar2 = trigger_condition;
  event_result = initial_event_result;
  affected_card_controller = new_affected_card_controller;
  affected_card = new_affected_card;
  attacking_card_controller = new_attacking_card_controller;
  attacking_card = new_attacking_card;
  if ((1 < event - EVENT_TRIGGER) && (event != EVENT_END_TRIGGER))
  {
    trigger_condition = 0xffffffff;
  }
  dispatch_event_raw(event);
  result = event_result;
  trigger_condition = uVar2;
  pop_affected_card_stack();
  return result;
}


#ifndef SHANDALAR
/* ManalinkEh 2022: 0x26cbf50; original functions/events.c:553. */
void allow_response_to_trigger(int player, int card)
{
  const char *name = EXE_FN(const char *, 0x439690, int, int)(player, card);
  sprintf((char *)0x60a690, "Trigger %s", name);
  int a1 = EXE_DWORD(0x60a54c) && (player == 0 || (trace_mode & 2)) ? -1 : -2;
  EXE_FN(int, 0x436a20, int, int, const char *, int)(a1, current_phase, EXE_STR(0x60a690),
                                                     EVENT_RESOLVE_TRIGGER);
}

#endif
