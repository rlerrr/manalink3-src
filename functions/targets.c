#include "manalink.h"

/* Canonical recovered helper declarations. */
void base_target_definition(int player, int card, target_definition_t *td, int type);
void default_target_definition(int player, int card, target_definition_t *td, int type);
void opponent_or_pw_target_definition(int player, int card, target_definition_t *td);
void counterspell_target_definition(int player, int card, target_definition_t *td,
                                         int type);
void counter_activated_target_definition(int player, int card, target_definition_t *td,
                                              int type);
void counter_triggered_target_definition(int player, int card, target_definition_t *td,
                                              int type);
int can_target(target_definition_t *td);
int target_available(int player, int card, target_definition_t *td);
int would_validate_arbitrary_target(target_definition_t *td, int tgt_player, int tgt_card);
int validate_arbitrary_target(target_definition_t *td, int tgt_player, int tgt_card);
int would_validate_target(int player, int card, target_definition_t *td,
                               int target_number);
int would_valid_target(target_definition_t *td);
int validate_target(int player, int card, target_definition_t *td, int target_number);
int valid_target(target_definition_t *td);
int pick_next_target_noload_arbitrary(target_definition_t *td, const char *prompt,
                                           int player, int card);
int pick_next_target_arbitrary(target_definition_t *td, const char *prompt, int player,
                                    int card);
int choose_default_target(int player, int card, target_definition_t *td);
int select_target(int player, int card, target_definition_t *td, const char *prompt,
                       target_t *ret_location);
int pick_target(target_definition_t *td, const char *prompt);
int new_pick_target(target_definition_t *td, const char *prompt, int ret_location,
                         int mode);
int pick_next_target_noload(target_definition_t *td, const char *prompt);
int pick_up_to_n_targets_noload(target_definition_t *td, const char *prompt, int num);
int pick_up_to_n_targets(target_definition_t *td, const char *prompt, int num);
int pick_player_duh(int player, int card, int preferred_controller, int allow_cancel);
int target_opponent(int player, int card);
int opponent_is_valid_target(int player, int card);
int mark_up_to_n_targets_noload(target_definition_t *td, const char *prompt, int num,
                                     char (*marked)[151]);
int get_protections_from(int player, int card);
static int target_match_type(card_instance_t *tgt_instance, int tgt_player, int tgt_card,
                                  int special, int type);
static int target_available_maybe_force(
    int player, int card, int *return_count, int who_chooses, int allowed_controller,
    int preferred_controller, int zone, int required_type, int illegal_type, int required_abilities,
    int illegal_abilities, int required_color, int illegal_color, int extra, int required_subtype,
    int power_requirement, int toughness_requirement, int special, int required_state,
    int illegal_state, int flags, int allow_cancel);
int real_validate_target(int tgt_player, int tgt_card, char *return_error_str, int who_chooses,
                              int allowed_controller, int preferred_controller, int zone,
                              int required_type, int illegal_type, int required_abilities,
                              int illegal_abilities, int required_color, int illegal_color,
                              int extra, int required_subtype, int power_requirement,
                              int toughness_requirement, int special, int required_state,
                              int illegal_state);
int real_target_available(int *return_count, int test_damage_target_or_source, int who_chooses,
                               int allowed_controller, int preferred_controller, int zone,
                               int required_type, int illegal_type, int required_abilities,
                               int illegal_abilities, int required_color, int illegal_color,
                               int extra, int required_subtype, int power_requirement,
                               int toughness_requirement, int special, int required_state,
                               int illegal_state);
int real_select_target(int who_chooses, int allowed_controller, int preferred_controller,
                            int zone, int required_type, int illegal_type, int required_abilities,
                            int illegal_abilities, int required_color, int illegal_color, int extra,
                            int required_subtype, int power_requirement, int toughness_requirement,
                            int special, int required_state, int illegal_state, const char *prompt,
                            int allow_cancel, target_t *ret_tgt);
int is_protected_from(int protected_player, int protected_card, int from_player, int from_card,
                           is_protected_from_mode_t new_protection_mode);
static int validate_target_impl(int player, int card, int tgt_player, int tgt_card,
                                     char *return_error_str, int who_chooses,
                                     int allowed_controller, int preferred_controller, int zone,
                                     int required_type, int illegal_type, int required_abilities,
                                     int illegal_abilities, int required_color, int illegal_color,
                                     int extra, int required_subtype, int power_requirement,
                                     int toughness_requirement, int special, int required_state,
                                     int illegal_state, int flags);
static int target_available_impl(int player, int card, int *return_count,
                                      int test_damage_target_or_source, int who_chooses,
                                      int allowed_controller, int preferred_controller, int zone,
                                      int required_type, int illegal_type, int required_abilities,
                                      int illegal_abilities, int required_color, int illegal_color,
                                      int extra, int required_subtype, int power_requirement,
                                      int toughness_requirement, int special, int required_state,
                                      int illegal_state, int flags);
static int select_target_impl(int player, int card, int who_chooses, int allowed_controller,
                                   int preferred_controller, int zone, int required_type,
                                   int illegal_type, int required_abilities, int illegal_abilities,
                                   int required_color, int illegal_color, int extra,
                                   int required_subtype, int power_requirement,
                                   int toughness_requirement, int special, int required_state,
                                   int illegal_state, int flags, const char *prompt,
                                   int allow_cancel, target_t *ret_tgt);
static int select_target_maybe_force(
    int player, int card, int who_chooses, int allowed_controller, int preferred_controller,
    int zone, int required_type, int illegal_type, int required_abilities, int illegal_abilities,
    int required_color, int illegal_color, int extra, int required_subtype, int power_requirement,
    int toughness_requirement, int special, int required_state, int illegal_state, int flags,
    const char *prompt, int allow_cancel, target_t *ret_tgt);
int autoselect_target(int player, int card, int i, int j, int k, int who_chooses,
                           int allowed_controller, int preferred_controller, int zone,
                           int required_type, int illegal_type, int required_abilities,
                           int illegal_abilities, int required_color, int illegal_color, int extra,
                           int required_subtype, int power_requirement, int toughness_requirement,
                           int special, int required_state, int illegal_state, int flags,
                           int prompt, int allow_cancel, int ret_location);

extern int target_controller, target_card;
int sub_4378E0(int, int, const char*, int, int, int, int, int, int*, target_t*, int, int);
int sub_45CEC0(void);
int sub_45CF00(void);
int sub_47E980(int, int);
int sub_47EF10(int, int);
int sub_47EF50(int, int);
int sub_498F20(void);
int sub_499050(void);

static int hack_gpf_player = -1;
static int hack_gpf_card = -1;



is_protected_from_mode_t protection_mode = IPF_TARGET;














const char* hack_prepend_prompt = NULL;







int auto_targets[500];
int auto_targets_target[500];
int read_auto_target_file = 0;

// Like default_target_definition, but doesn't set illegal_abilities.




// Like default_target_definition_t, but sets TARGET_TYPE_PLANESWALKER, TARGET_ZONE_CREATURE_OR_PLAYER, and TARGET_SPECIAL_OPPONENT.












// Selects a target into (td->player, td->card)->targets[0].  Loads prompt from Tetx.res.  Sets spell_fizzled if cancelled.


/* Selects a target into (td->player, td->card)->targets[ret_location], or into the next unused slot (counting by instance->number_of_targets) if
	ret_location is -1. */


/* Selects a target into next (td->player, td->card)'s unused target slot (counting by instance->number_of_targets).  prompt is used literally, not loaded from Text.res.  Sets
 * spell_fizzled if cancelled. */


// Just like pick_up_to_n_targets(), but uses prompt literally instead of loading from Text.res.


/* Selects up to num targets starting from (td->player, td->card)->targets[0].  Returns actual number chosen (also in number_of_targets).  Set allow_cancel to 3
 * to show both Done and Cancel buttons.  Sets spell_fizzled only if the Cancel button is clicked, not if the Done button is. */


/* Just like pick_up_to_n_targets_noload(), but doesn't change (td->player,td->card)'s number_of_targets or targets array, instead marking targets in
 * marked[][].  If the Cancel button is pushed, returns 0 and sets cancel to 1, but leaves previously-chosen targets set in marked.  marked should be a char
 * array[2][151] initialized to 0.  Set num to -1 to allow any number of targets.  This should only be used for non-targetted things resolving immediately after
 * chosen, since they won't track change of control or be seen by when-this-becomes-targeted triggers. */


/* Selects a target into next (player, card)'s unused target slot (counting by instance->number_of_targets), while using (td->player, td->card) as the
 * targeting source.  Sets spell_fizzled if cancelled. */


/* Selects a target into next (player, card)'s unused target slot (counting by instance->number_of_targets), while using (td->player, td->card) as the targeting
 * source.  prompt is used literally, not loaded from Text.res.  Sets spell_fizzled if cancelled. */


/* Selects a target into ret_location, or (player, card)->targets[0] if NULL.  Targeting source is player/card instead of td->player/td->card.  prompt is used
 * literally, not loaded from Text.res.  Never sets spell_fizzled. */


/* If duh mode is on, pick preferred_controller (which is opponent by default) if he's a valid target, or else cancel if that's allowed.
 * Otherwise, forwards to pick_target(td, "TARGET_PLAYER"). */


// (player/card) targets 1-player in its targets[0] and returns nonzero, else returns 0.


/* Returns nonzero if (player/card) can target 1-player, else 0.
 * The difference from target_opponent() is that it doesn't assign the target, increase number_of_targets, or set cancel. */













int is_protected_from_me(int player, int card, int t_player, int t_card){

	card_instance_t *instance = get_card_instance(player, card);
	int clr = instance->card_color;
	if( get_id(t_player, t_card) == CARD_ID_GAEAS_REVENGE && !(clr & COLOR_TEST_GREEN) ){
		return 1;
	}

	if( get_id(t_player, t_card) == CARD_ID_SUQATA_FIREWALKER && (clr & COLOR_TEST_RED) ){
		return 1;
	}

	if( get_id(t_player, t_card) == CARD_ID_AZORIUS_FIRST_WING && is_what(player, card, TYPE_ENCHANTMENT) ){
		return 1;
	}

	if( get_id(t_player, t_card) == CARD_ID_ENEMY_OF_THE_GUILDPACT && count_colors(player, card) > 1 ){
		return 1;
	}

	if( get_id(t_player, t_card) == CARD_ID_GUARDIAN_OF_THE_GUILDPACT && count_colors(player, card) == 1 ){
		return 1;
	}

	if( get_id(t_player, t_card) == CARD_ID_EMRAKUL_THE_AEONS_TORN && clr != COLOR_TEST_COLORLESS && is_what(player, card, TYPE_SPELL) &&
		! is_what(player, card, TYPE_PERMANENT)
	  ){
		return 1;
	}
	/*
	if( is_what(t_player, t_card, TYPE_CREATURE) && check_special_flags2(t_player, t_card, SF2_DENSE_FOLIAGE) && is_what(player, card, TYPE_SPELL) &&
		! is_what(player, card, TYPE_PERMANENT)
	  ){
		return 1;
	}
	*/
	if( player != t_player && get_id(t_player, t_card) == CARD_ID_FIENDSLAYER_PALADIN && ((clr & COLOR_TEST_BLACK) || (clr & COLOR_TEST_RED)) ){
		return 1;
	}
	return 0;
}

void give_shroud_to_player(int player, int card, event_t event){
	player_bits[player] |= PB_PLAYER_HAS_SHROUD;
}

void give_hexproof_to_player(int player, int card, event_t event){
	player_bits[player] |= PB_PLAYER_HAS_HEXPROOF;
}

int target_me(int t_player, int t_card, int player, int card)
{
  card_instance_t* instance = get_card_instance(player, card);
  int i;
  for (i = 0; i < instance->number_of_targets; ++i)
	if (instance->targets[i].player == t_player && instance->targets[i].card == t_card)
	  return 1;

  return 0;
}

/* When {t_player,t_card} becomes targeted by a spell controlled by controlled_by, returns the spell (else NULL).
 * {player,card} is solely the card that highlights for the trigger.  Doesn't check is_humiliated() or in_play(). */
const target_t* becomes_target_of_spell(int player, int card, event_t event, int t_player, int t_card, int controlled_by, resolve_trigger_t trigger_mode)
{
  static target_t rval;
  if (trigger_condition == TRIGGER_SPELL_CAST && affect_me(player, card) && player == reason_for_trigger_controller
	  && (controlled_by == ANYBODY || trigger_cause_controller == controlled_by))
	{
	  if (event != EVENT_TRIGGER && event != EVENT_RESOLVE_TRIGGER)
		return NULL;

	  if (target_me(t_player, t_card, trigger_cause_controller, trigger_cause) && !is_what(trigger_cause_controller, trigger_cause, TYPE_LAND | TYPE_EFFECT))
		{
		  if (event == EVENT_TRIGGER)
			{
			  if (trigger_mode == RESOLVE_TRIGGER_DUH)
				trigger_mode = duh_mode(player) ? RESOLVE_TRIGGER_MANDATORY : RESOLVE_TRIGGER_OPTIONAL;

			  event_result |= trigger_mode;
			  return NULL;
			}
		  else // event == EVENT_RESOLVE_TRIGGER;
			{
			  rval.player = trigger_cause_controller;
			  rval.card = trigger_cause;
			  return &rval;
			}
		}
	}

  return NULL;
}

/* When {t_player,t_card} becomes targeted by a spell or ability controlled by controlled_by, returns the spell or effect.
 * {player,card} is solely the card that highlights for the trigger.  Doesn't check is_humiliated() or in_play(). */
const target_t* becomes_target_of_spell_or_effect(int player, int card, event_t event, int t_player, int t_card, int controlled_by)
{
  const target_t* btos;
  if ((btos = becomes_target_of_spell(player, card, event, t_player, t_card, controlled_by, RESOLVE_TRIGGER_MANDATORY)))
	return btos;

  static target_t rval;
  if ((event == EVENT_PLAY_ABILITY || event == EVENT_TAPPED_TO_PLAY_ABILITY || event == EVENT_TRIGGER_RESOLVED)
	  && (controlled_by == ANYBODY || affected_card_controller == controlled_by)
	  && target_me(t_player, t_card, affected_card_controller, affected_card)
	  && get_card_instance(affected_card_controller, affected_card)->info_slot != (int)legacy_effect_pump_ability_until_eot
	  )
	{
	  rval.player = affected_card_controller;
	  rval.card = affected_card;
	  return &rval;
	}

  return NULL;
}

static int is_targeting_type(int player, int card, int controller_of_target, int or_player, type_t targeted_type, int subtype)
{
  card_instance_t* instance = get_card_instance(player, card);
  int i;
  for (i = 0; i < instance->number_of_targets; ++i)
	if ((controller_of_target != ANYBODY ? instance->targets[i].player == controller_of_target
		 : (instance->targets[i].player == 0 || instance->targets[i].player == 1))
		&& (instance->targets[i].card == -1 ? or_player
			: (in_play(instance->targets[i].player, instance->targets[i].card)
			   && is_what(instance->targets[i].player, instance->targets[i].card, targeted_type)
			   && (subtype == -1 || has_subtype(instance->targets[i].player, instance->targets[i].card, subtype)))))
	  return 1;

  return 0;
}

// Populates arr with array of unique creatures controlled by controller_of_target targeted by (player,card), then returns it.
static target_t* fill_arr(int player, int card, int controller_of_target, int or_player, type_t targeted_type, int subtype, target_t* arr)
{
  card_instance_t* instance = get_card_instance(player, card);
  ASSERT(instance->number_of_targets < 20);
  int i, j, last = -1;
  for (i = 0; i < instance->number_of_targets; ++i)
	if ((controller_of_target != ANYBODY ? instance->targets[i].player == controller_of_target
		 : (instance->targets[i].player == 0 || instance->targets[i].player == 1))
		&& (instance->targets[i].card == -1 ? or_player
			: (in_play(instance->targets[i].player, instance->targets[i].card)
			   && is_what(instance->targets[i].player, instance->targets[i].card, targeted_type)
			   && (subtype == -1 || has_subtype(instance->targets[i].player, instance->targets[i].card, subtype)))))
	  {
		for (j = 0; j < i; ++j)
		  if (instance->targets[i].player == instance->targets[j].player
			  && instance->targets[i].card == instance->targets[j].card)
			goto outer_continue;	// only triggers once per creature even if it's targeted more than once by the same spell/effect, per Wild Defiance ruling

		arr[++last] = instance->targets[i];	// struct copy

	  outer_continue:;
	  }

  arr[++last].player = -1;
  arr[last].card = -1;

  return arr;
}

/* The general case.  Returns array of unique targeted_types (and players, if or_player is nonzero) controlled by controller_of_target currently being targeted
 * by a targeting_type (TYPE_EFFECT: an effect; other types: a spell of that type being cast) controlled by controller_of_trigger, ending in a {-1,-1} entry; or
 * NULL if none. */
const target_t* any_becomes_target(int player, int card, event_t event, int controller_of_target, int or_player, type_t targeted_type, int subtype, type_t targeting_type, int controller_of_trigger, resolve_trigger_t resolve_trigger)
{
  static target_t arr[20];

  if (!in_play(player, card) || is_humiliated(player, card, AL_TRIG))
	return NULL;

  if (trigger_condition == TRIGGER_SPELL_CAST && affect_me(player, card) && player == reason_for_trigger_controller)
	{
	  if (!(event == EVENT_TRIGGER || event == EVENT_RESOLVE_TRIGGER)
		  || !(targeting_type & TYPE_NONEFFECT)
		  || (controller_of_trigger != ANYBODY && trigger_cause_controller != controller_of_trigger))
		return NULL;

	  if (is_what(trigger_cause_controller, trigger_cause, targeting_type & TYPE_NONEFFECT)
		  && is_targeting_type(trigger_cause_controller, trigger_cause, controller_of_target, or_player, targeted_type, subtype))
		{
		  if (event == EVENT_TRIGGER)
			{
			  if (resolve_trigger == RESOLVE_TRIGGER_DUH)
				resolve_trigger = duh_mode(player) ? RESOLVE_TRIGGER_MANDATORY : RESOLVE_TRIGGER_OPTIONAL;

			  event_result |= resolve_trigger;
			  return NULL;
			}
		  else // event == EVENT_RESOLVE_TRIGGER;
			return fill_arr(trigger_cause_controller, trigger_cause, controller_of_target, or_player, targeted_type, subtype, arr);
		}
	}

  if (event == EVENT_PLAY_ABILITY || event == EVENT_TAPPED_TO_PLAY_ABILITY || event == EVENT_TRIGGER_RESOLVED)
	{
	  if ((targeting_type & TYPE_EFFECT)
		  && (controller_of_trigger == ANYBODY || controller_of_trigger == affected_card_controller)
		  && is_targeting_type(affected_card_controller, affected_card, controller_of_target, or_player, targeted_type, subtype))
		return fill_arr(affected_card_controller, affected_card, controller_of_target, or_player, targeted_type, subtype, arr);
	}

  return NULL;
}

/* A common case of any_becomes_target().  Returns array of unique creatures controlled by controller_of_target currently being targeted, ending in a {-1,-1}
 * entry; or NULL if none. */
const target_t* any_creature_becomes_target_of_spell_or_effect(int player, int card, event_t event, int controller_of_target)
{
  return any_becomes_target(player, card, event, controller_of_target, 0, TYPE_CREATURE, -1, TYPE_EFFECT|TYPE_NONEFFECT, ANYBODY, RESOLVE_TRIGGER_MANDATORY);
}

// "(Creature this effect is attached to) gains 'When this creature becomes the target of a spell or ability, sacrifice it.'"
int attached_creature_gains_sacrifice_when_becomes_target(int player, int card, event_t event)
{
  card_instance_t* instance;
  if ((instance = in_play(player, card))
	  && instance->damage_target_card >= 0
	  && in_play(instance->damage_target_player, instance->damage_target_card)
	  && !is_humiliated(instance->damage_target_player, instance->damage_target_card, AL_TRIG)
	  && becomes_target_of_spell_or_effect(player, card, event, instance->damage_target_player, instance->damage_target_card, ANYBODY))
	{
	  kill_card(instance->damage_target_player, instance->damage_target_card, KILL_SACRIFICE);
	  return 1;
	}
  return 0;
}

// "When enchanted creature becomes the target of a spell or ability, sacrifice/destroy/bury/exile (this aura)."
int kill_attachment_when_creature_is_targeted(int player, int card, event_t event, kill_t kill_code)
{
  card_instance_t* instance = get_card_instance(player, card);
  if (in_play(player, card)
	  && instance->damage_target_card >= 0
	  && in_play(instance->damage_target_player, instance->damage_target_card)
	  && !is_humiliated(player, card, AL_TRIG)
	  && becomes_target_of_spell_or_effect(player, card, event, instance->damage_target_player, instance->damage_target_card, ANYBODY))
	{
	  kill_card(player, card, kill_code);
	  return 1;
	}
  return 0;
}

// "When enchanted creature becomes the target of a spell, sacrifice/destroy/bury/exile (this aura)."
int kill_attachment_when_creature_is_targeted_by_spell(int player, int card, event_t event, kill_t kill_code)
{
  card_instance_t* instance = get_card_instance(player, card);
  if (in_play(player, card)
	  && instance->damage_target_card >= 0
	  && in_play(instance->damage_target_player, instance->damage_target_card)
	  && !is_humiliated(player, card, AL_TRIG)
	  && becomes_target_of_spell(player, card, event, instance->damage_target_player, instance->damage_target_card, ANYBODY, RESOLVE_TRIGGER_MANDATORY))
	{
	  kill_card(player, card, kill_code);
	  return 1;
	}
  return 0;
}

/* Recovered October 2022 implementations; see docs/recovery-progress.md. */

/* ManalinkEh 2022: 0x265d540; original functions/targets.c:16. */
keyword_t color_to_protection(color_t color) { return (color & 0x3e) << 10; }

/* Legacy 80-byte compatibility wrapper retained; the 84-byte 2022 implementation is below. */


/* ManalinkEh 2022: 0x2661490; original functions/targets.c:1795. */
int choose_player(int player, int card, int preferred_controller, int allow_cancel)
{
  card_instance_t *instance;
  int result;
  target_t chosen_player;
  target_definition_t td;

  instance = get_card_instance(player, card);
  default_target_definition(player, card, &td, 0);
  td.preferred_controller = preferred_controller;
  td.zone = 0x1000;
  td.allow_cancel = allow_cancel;
  td.illegal_abilities = 0;
  result = duh_mode(player);
  if (result != 0)
  {
    instance->number_of_targets = '\x01';
    instance->targets[0].player = preferred_controller;
    instance->targets[0].card = -1;
    result = would_validate_target(td.player, td.card, &td, 0);
    if (result != 0)
    {
      return 1;
    }
    if ((allow_cancel & 1U) != 0)
    {
      spell_fizzled = 1;
      return 0;
    }
  }
  instance->number_of_targets = '\0';
  result = can_target(&td);
  if (result != 0)
  {
    select_target(player, card, &td, "Choose a player.", &chosen_player);
  }
  if (chosen_player.card != -1)
  {
    backtrace_and_abort("functions/targets.c:1824\nchosen_player.card == -1");
  }
  return chosen_player.player;
}

/* ManalinkEh 2022: 0x2661860; original functions/targets.c:2034. */
int can_legally_target(int targeting_player, int targeting_card, target_t *targeted)
{
  int32_t iVar1;
  card_instance_t *instance;
  int iVar3;

  instance = get_card_instance(targeting_player, targeting_card);
  iVar3 = 0;
  if (instance->number_of_targets != '\0')
  {
    iVar1 = targeted->card;
    instance->targets[9].player = targeted->player;
    instance->targets[9].card = iVar1;
    set_special_flags2(targeting_player, targeting_card, 0x10000);
    iVar3 = call_card_function(targeting_player, targeting_card, EVENT_CAN_CAST);
    instance->targets[9].player = -1;
    instance->targets[9].card = -1;
    remove_special_flags2(targeting_player, targeting_card, 0x10000);
  }
  return iVar3;
}


#ifndef SHANDALAR
static int get_protections_from_part(int player, int card);
#endif

/* ManalinkEh 2022: 0x265d580; original functions/targets.c:107. */
int has_protection_from_impl(int protected_player, int protected_card, int from_player,
                             int from_card, is_protected_from_mode_t new_protection_mode,
                             keyword_t keywords)
{
  keyword_t kVar1;
  card_instance_t *instance;
  int result;
  unsigned int uVar4;
  int initial_event_result;

  if (((((keywords & (KEYWORD_PROT_CREATURES | KEYWORD_PROT_SORCERIES | KEYWORD_PROT_INSTANTS |
                      KEYWORD_PRISTINE)) != KEYWORD_0) &&
        (from_card != -1)) &&
       (from_player != -1)) &&
      (kVar1 = get_protections_from_part(from_player, from_card),
       (keywords &
        (KEYWORD_PROT_CREATURES | KEYWORD_PROT_SORCERIES | KEYWORD_PROT_INSTANTS |
         KEYWORD_PRISTINE) &
        kVar1) != KEYWORD_0))
  {
    return 1;
  }
  if (protected_card < 0)
  {
    if ((((unsigned int)player_bits[protected_player] >> 24 & 0xff) & 4) == 0)
    {
      return 0;
    }
  LAB_0265d680:
    initial_event_result = (new_protection_mode << 0x18);
  }
  else
  {
    instance = get_card_instance(protected_player, protected_card);
    uVar4 = instance->ext->sp_keywords2;
    if ((uVar4 & 1) == 0)
    {
      if ((((uVar4 & 4) == 0) || (new_protection_mode != IPF_TARGET)) ||
          (from_player == protected_player))
      {
        if ((uVar4 & 2) == 0)
        {
          return 0;
        }
        goto LAB_0265d680;
      }
      initial_event_result = (IPF_TARGET << 24);
    }
    else
    {
      initial_event_result = (new_protection_mode << 0x18);
    }
    if ((uVar4 & 2) == 0)
    {
      result = is_humiliated(protected_player, protected_card, AL_STATIC);
      if (result != 0)
      {
        return 0;
      }
      uVar4 = dispatch_event_arbitrary_to_one_card_with_initial_event_result(
          protected_player, protected_card, EVENT_PROT_MISC, protected_player, protected_card,
          from_player, from_card, (int)initial_event_result);
      return uVar4 & 0xffffff;
    }
  }
  uVar4 = dispatch_event_with_attacker_and_initial_event_result(
      protected_player, protected_card, EVENT_PROT_MISC, from_player, from_card,
      (int)initial_event_result);
  return uVar4 & 0xffffff;
}


/* ManalinkEh 2022: 0x265d6c0; original functions/targets.c:116. */
int is_protected_from(int protected_player, int protected_card, int from_player, int from_card,
                           is_protected_from_mode_t new_protection_mode)
{
  keyword_t keywords;
  int result;

  keywords = KEYWORD_0;
  if (-1 < protected_card)
  {
    keywords = get_abilities(protected_player, protected_card, EVENT_ABILITIES, -1);
  }
  result = has_protection_from_impl(protected_player, protected_card, from_player, from_card,
                                    new_protection_mode, keywords);
  return result;
}


#ifndef SHANDALAR

/* ManalinkEh 2022: 0x265d2e0; original functions/targets.c:21. */
static int get_protections_from_part(int player, int card)
{
  unsigned char bVar1;
  card_instance_t *instance;
  unsigned int uVar3;
  unsigned int uVar4;

  instance = get_card_instance(player, card);
  uVar4 = instance->internal_card_id;
  if (uVar4 == activation_card)
  {
    do
    {
      player = instance->parent_controller;
      card = instance->parent_card;
      instance = get_card_instance(player, card);
      uVar4 = instance->internal_card_id;
    } while (uVar4 == activation_card);
  }
  if (uVar4 == 0xffffffff)
  {
    uVar4 = (unsigned int)instance->backup_internal_card_id;
  }
  bVar1 = cards_data[uVar4].type;
  uVar4 = (-(unsigned int)((bVar1 & 8) == 0) & 0xfff00000) + 0x120000;
  if (((bVar1 & 0x30) != 0) && ((bVar1 & 0x4f) == 0))
  {
    uVar4 = uVar4 | 0x40000;
  }
  if ((bVar1 & 2) != 0)
  {
    uVar4 = uVar4 | 0x200000;
  }
  if ((bVar1 & 1) != 0)
  {
    uVar4 = uVar4 | 0x400000;
  }
  if ((bVar1 & 0x40) != 0)
  {
    uVar4 = uVar4 | 0x10000;
  }
  uVar3 = get_color(player, card);
  hack_gpf_player = player;
  hack_gpf_card = card;
  return (uVar3 & 0x3e) << 10 | uVar4;
}


/* ManalinkEh 2022: 0x265d550; original functions/targets.c:21. */
int get_protections_from(int player, int card)
{
  int result;

  if ((player != -1) && (card != -1))
  {
    result = get_protections_from_part(player, card);
    return result;
  }
  return 0;
}


/* ManalinkEh 2022: 0x26601a0; original functions/targets.c:1332. */
void base_target_definition(int player, int card, target_definition_t *td, int type)
{
  td->who_chooses = player;
  td->preferred_controller = 1 - player;
  td->player = player;
  td->allowed_controller = 2;
  td->zone = 0x200;
  td->required_type = type;
  td->illegal_type = 0;
  td->required_abilities = 0;
  td->illegal_abilities = 0;
  td->required_color = 0;
  td->illegal_color = 0;
  td->extra = -1;
  td->required_subtype = -1;
  td->power_requirement = -1;
  td->toughness_requirement = -1;
  td->special = 0;
  td->required_state = 0;
  td->illegal_state = 0;
  td->allow_cancel = 1;
  td->card = card;
  td->flags = 0;
  return;
}


/* ManalinkEh 2022: 0x2660240; original functions/targets.c:1357. */
void default_target_definition(int player, int card, target_definition_t *td, int type)
{
  int result;
  td->preferred_controller = 1 - player;
  td->who_chooses = player;
  td->allowed_controller = 2;
  td->zone = 0x200;
  td->required_type = type;
  td->illegal_type = 0;
  td->required_abilities = 0;
  td->illegal_abilities = 0;
  td->required_color = 0;
  td->illegal_color = 0;
  td->extra = -1;
  td->required_subtype = -1;
  td->power_requirement = -1;
  td->toughness_requirement = -1;
  td->special = 0;
  td->required_state = 0;
  td->illegal_state = 0;
  td->allow_cancel = 1;
  td->player = player;
  td->card = card;
  td->flags = 0;
  if ((player != -1) && (card != -1))
  {
    result = get_protections_from_part(player, card);
    td->illegal_abilities = result;
    return;
  }
  td->illegal_abilities = 0;
  return;
}


/* ManalinkEh 2022: 0x2660300; original functions/targets.c:1364. */
void opponent_or_pw_target_definition(int player, int card, target_definition_t *td)
{
  default_target_definition(player, card, td, 0x100);
  td->zone = 0x1200;
  td->special = 0x2000000;
  return;
}


/* ManalinkEh 2022: 0x2660340; original functions/targets.c:1371. */
void counterspell_target_definition(int player, int card, target_definition_t *td,
                                         int type)
{
  td->who_chooses = player;
  td->preferred_controller = 1 - player;
  td->player = player;
  td->allowed_controller = 2;
  td->required_type = type;
  td->illegal_type = 0;
  td->required_abilities = 0;
  td->illegal_abilities = 0;
  td->required_color = 0;
  td->illegal_color = 0;
  td->extra = -1;
  td->required_subtype = -1;
  td->power_requirement = -1;
  td->toughness_requirement = -1;
  td->required_state = 0;
  td->illegal_state = 0;
  td->allow_cancel = 1;
  td->card = card;
  td->flags = 0;
  td->zone = 0;
  td->special = 2;
  return;
}


/* ManalinkEh 2022: 0x26603e0; original functions/targets.c:1378. */
void counter_activated_target_definition(int player, int card, target_definition_t *td,
                                              int type)
{
  td->who_chooses = player;
  td->preferred_controller = 1 - player;
  td->player = player;
  td->allowed_controller = 2;
  td->required_type = type;
  td->illegal_type = 0;
  td->required_abilities = 0;
  td->illegal_abilities = 0;
  td->required_color = 0;
  td->illegal_color = 0;
  td->extra = -1;
  td->required_subtype = -1;
  td->power_requirement = -1;
  td->toughness_requirement = -1;
  td->required_state = 0;
  td->illegal_state = 0;
  td->allow_cancel = 1;
  td->card = card;
  td->flags = 0;
  td->zone = 0;
  td->special = 0x20000;
  return;
}


/* ManalinkEh 2022: 0x2660480; original functions/targets.c:1385. */
void counter_triggered_target_definition(int player, int card, target_definition_t *td,
                                              int type)
{
  default_target_definition(player, card, td, type);
  td->zone = 0;
  td->special = 0x8000000;
  return;
}

#endif


#ifndef SHANDALAR

/* ManalinkEh 2022: 0x265d1a0; original functions/targets.c:217. */
static int target_match_type(card_instance_t *tgt_instance, int tgt_player, int tgt_card,
                                  int special, int type)
{
  int iVar1;
  int iVar2;

  iVar1 = tgt_instance->internal_card_id;
  if ((special & 0x20000U) == 0)
  {
    iVar2 = is_what(tgt_player, tgt_card, type & 0x1ffU);
  }
  else
  {
    iVar2 = is_what(-1, tgt_instance->original_internal_card_id, type & 0x1ffU);
    type = type | 0x80;
  }
  if ((((iVar2 == 0) &&
        (((type & 0x1000U) == 0 || (iVar2 = is_token(tgt_player, tgt_card), iVar2 == 0)))) &&
       (((type & 0x2000000U) == 0 || ((tgt_instance->state & 0x2000000) == 0)))) &&
      (((((type & 0x80U) == 0 ||
          ((((uint32_t)cards_data[iVar1].id | ((uint32_t)cards_data[iVar1].reserved2 << 16)) -
                0x387U &
            0xfffd) != 0)) &&
         (((type & 0x2000U) == 0 || (cards_data[iVar1].id != 0x385)))) &&
        (((type & 0x4000U) == 0 || (cards_data[iVar1].id != 0x386))))))
  {
    iVar2 = 0;
    if ((type & 0x8000U) != 0)
    {
      return (unsigned int)(cards_data[iVar1].id == 0x388);
    }
  }
  else
  {
    iVar2 = 1;
  }
  return iVar2;
}


/* ManalinkEh 2022: 0x265d720; original functions/targets.c:260. */
static int validate_target_impl(int player, int card, int tgt_player, int tgt_card,
                                     char *return_error_str, int who_chooses,
                                     int allowed_controller, int preferred_controller, int zone,
                                     int required_type, int illegal_type, int required_abilities,
                                     int illegal_abilities, int required_color, int illegal_color,
                                     int extra, int required_subtype, int power_requirement,
                                     int toughness_requirement, int special, int required_state,
                                     int illegal_state, int flags)
{
  uint16_t uVar2;
  uint16_t uVar3;
  int iVar4;
  card_instance_t *pcVar5;
  card_instance_t *pcVar6;
  const char *pcVar7;
  subtype_t sVar8;
  int iVar9;
  counter_t type;
  unsigned int uVar10;
  unsigned int uVar12;
  int32_t iVar13;
  unsigned int uVar14;

  int iVar18;
  bool bVar19;
  unsigned int uStack_144;
  char buffer[100];
  char error_str[200];

  if (tgt_player == -1)
  {
  LAB_0265d940:
    if (return_error_str == NULL)
    {
      return 0;
    }
    *return_error_str = '\0';
    return 0;
  }
  if (tgt_card == -1)
  {
    error_str[0] = '\0';
    if (zone == 0)
    {
    LAB_0265d8e8:
      if ((who_chooses == 0) || ((trace_mode & 2U) != 0))
      {
        if ((allowed_controller & 2U) != 0)
          goto LAB_0265db9a;
        if (who_chooses == 1)
          goto LAB_0265db90;
        if (who_chooses != 0)
          goto LAB_0265dce8;
      LAB_0265d912:
        bVar19 = false;
        uVar12 = allowed_controller & 1;
        if (uVar12 == 0)
        {
        LAB_0265d920:
          if (who_chooses != 1)
            goto LAB_0265db85;
          uVar12 = preferred_controller & 1;
          bVar19 = uVar12 == 0;
        }
      }
      else if (who_chooses == 1)
      {
      LAB_0265db90:
        if ((preferred_controller & 2U) == 0)
        {
        LAB_0265dce8:
          if ((trace_mode & 2U) != 0)
            goto LAB_0265d912;
          goto LAB_0265d920;
        }
      LAB_0265db9a:
        uVar12 = 1;
        bVar19 = true;
      }
      else
      {
      LAB_0265db85:
        uVar12 = 0;
        bVar19 = true;
      }
    }
    else
    {
      uVar12 = 0;
      bVar19 = false;
      if ((zone & 0x1000U) != 0)
        goto LAB_0265d8e8;
    }
    if ((special & 0x2000000U) == 0)
    {
    LAB_0265d786:
      if ((special & 0x4000000U) != 0)
      {
        iVar4 = player;
        if (player < 0)
        {
          iVar4 = who_chooses;
        }
        if (iVar4 != tgt_player)
        {
          strncpy(error_str, ",player", 8);
          goto LAB_0265d8b5;
        }
      }
      if (((illegal_abilities & 0x20000U) != 0) &&
          ((((player_bits[tgt_player] & 0x10000) != 0 ||
             (((tgt_player != who_chooses && ((player_bits[tgt_player] & 0x20000) != 0)) &&
               (iVar4 = check_battlefield_for_id(1 - tgt_player, 0x4dd9), iVar4 == 0)))) ||
            (iVar4 = is_protected_from(tgt_player, -1, player, card,
                                            (-(unsigned int)((flags & 2U) == 0) & 3) +
                                                IPF_ATTACH_AURA),
             iVar4 != 0))))
        goto LAB_0265dc88;
      if ((bVar19 || tgt_player != 0) && ((tgt_player != 1 || (uVar12 != 0))))
      {
      LAB_0265dbd8:
        iVar4 = 1;
        if (return_error_str == NULL)
        {
          return 1;
        }
        goto LAB_0265d8cf;
      }
      strcpy(error_str, EXE_STR(0x7366d8));
    }
    else
    {
      iVar4 = player;
      if (player < 0)
      {
        iVar4 = who_chooses;
      }
      if (iVar4 != tgt_player)
        goto LAB_0265d786;
      strncpy(error_str, ",opponent", 10);
    }
  }
  else
  {
    pcVar5 = get_card_instance(tgt_player, tgt_card);
    if ((pcVar5->internal_card_id == -1) ||
        (pcVar5 = get_card_instance(tgt_player, tgt_card), (int)pcVar5->token_status < 0))
      goto LAB_0265d940;
    error_str[0] = '\0';
    pcVar5 = get_card_instance(tgt_player, tgt_card);
    if ((pcVar5->internal_card_id < 0) || ((pcVar5->state & 0x900000) != 0))
    {
    LAB_0265d89f:
      strcat(error_str, EXE_STR(0x785e64));
      goto LAB_0265d8b5;
    }
    pcVar6 = in_play(tgt_player, tgt_card);
    if (pcVar6 == NULL)
    {
      pcVar6 = in_hand(tgt_player, tgt_card);
      uStack_144 = (-(unsigned int)(pcVar6 == NULL) & 0x3f00) + 0x100;
    }
    else
    {
      if ((cards_data[pcVar5->internal_card_id].type & 199) == 0)
        goto LAB_0265d89f;
      uStack_144 = 0x200;
    }
    if ((zone != 0) && ((zone & uStack_144) == 0))
    {
      strcat(error_str, EXE_STR(0x7875bc));
      goto LAB_0265d8b5;
    }
    if ((who_chooses == 0) || ((trace_mode & 2U) != 0))
    {
    LAB_0265d9d0:
      if ((allowed_controller & 4U) == 0)
      {
        if ((allowed_controller & 2U) == 0)
        {
          if ((allowed_controller & 0xcU) == 0xc)
            goto LAB_0265dcc4;
          if (tgt_player != (allowed_controller & 1U))
            goto LAB_0265dccc;
        }
      }
      else if ((allowed_controller & 2U) == 0)
      {
        if ((allowed_controller & 0xcU) == 0xc)
        {
        LAB_0265dcc4:
          if (tgt_player != (allowed_controller & 1U))
          {
          LAB_0265dccc:
            strcat(error_str, EXE_STR(0x786398));
            goto LAB_0265d8b5;
          }
        }
        bVar19 = ((allowed_controller & 1U) != 0);
        if ((pcVar5->state & 0x1000) != 0)
        {
          bVar19 = (allowed_controller & 1U) == 0;
        }
        if (bVar19 != false)
        {
          strcat(error_str, EXE_STR(0x62bbbc));
          goto LAB_0265d8b5;
        }
      }
    }
    else if (extra != damage_card)
    {
      allowed_controller = preferred_controller;
      goto LAB_0265d9d0;
    }
    if (((special & 0x20000U) != 0) && (-1 < (char)cards_data[pcVar5->internal_card_id].type))
    {
      strcat(error_str, ",effect");
      goto LAB_0265d8b5;
    }
    if ((((required_type != 0) &&
          (iVar4 = target_match_type(pcVar5, tgt_player, tgt_card, special, required_type),
           iVar4 == 0)) ||
         ((illegal_type != 0 &&
           (iVar4 = target_match_type(pcVar5, tgt_player, tgt_card, special, illegal_type),
            iVar4 != 0)))) ||
        ((cards_data[pcVar5->internal_card_id].code_pointer == (CardFunction)0x401010 &&
          ((special & 0x400U) == 0))))
    {
      strcat(error_str, EXE_STR(0x728f6c));
      goto LAB_0265d8b5;
    }
    if ((required_abilities == 0) ||
        (required_abilities == (required_abilities & pcVar5->regen_status)))
    {
      if ((illegal_abilities != 0) &&
          ((damage_card != pcVar5->internal_card_id && (uStack_144 == 0x200))))
      {
        if ((illegal_abilities & pcVar5->regen_status & 0xfffdffff) == 0)
        {
          if ((illegal_abilities & 0x20000U) == 0)
            goto LAB_0265da8d;
          if (flags == 0)
          {
            iVar4 = is_protected_from(tgt_player, tgt_card, player, card, IPF_TARGET);
            if (((iVar4 == 0) &&
                 (((pcVar5->regen_status & 0x20000) == 0 ||
                   (((iVar4 = get_id(tgt_player, tgt_card),
                      iVar4 == 0x65e && (iVar4 = pcVar5->targets[1].player, 0 < iVar4)) &&
                     ((iVar4 >> ((unsigned char)who_chooses & 0x1f) & 1U) != 0)))))) &&
                (((tgt_player == who_chooses ||
                   (iVar4 = check_for_special_ability(tgt_player, tgt_card, 0x800000),
                    iVar4 == 0)) ||
                  ((iVar4 = is_what(tgt_player, tgt_card, 2),
                    iVar4 != 0 && (iVar4 = check_special_flags(tgt_player, tgt_card, 0x20000),
                                   iVar4 != 0))))))
              goto LAB_0265da8d;
          }
          else if ((((flags & 2U) == 0) ||
                    (iVar4 = is_protected_from(tgt_player, tgt_card, player, card,
                                                    IPF_ATTACH_AURA),
                     iVar4 == 0)) &&
                   (((flags & 4U) == 0 ||
                     (iVar4 = is_protected_from(tgt_player, tgt_card, player, card,
                                                     IPF_ATTACH_EQUIPMENT),
                      iVar4 == 0))))
            goto LAB_0265da8d;
        }
        goto LAB_0265dc88;
      }
    LAB_0265da8d:
      if (((required_color != 0) &&
           (uVar12 = get_color(tgt_player, tgt_card), (required_color & uVar12) == 0)) ||
          ((illegal_color != 0 &&
            (uVar12 = get_color(tgt_player, tgt_card), (illegal_color & uVar12) != 0))))
      {
        strcat(error_str, EXE_STR(0x739060));
        goto LAB_0265d8b5;
      }
      if ((special & 0x8000U) != 0)
      {
        if (extra + 1U < 2)
        {
          backtrace_and_abort("functions/targets.c:450\nextra != -1 && extra != 0");
        }
        iVar4 = player;
        iVar13 = card;
        if (((player != -1) && (card != -1)) &&
            (pcVar6 = get_card_instance(player, card), pcVar6->internal_card_id == activation_card))
        {
          iVar4 = pcVar6->parent_controller;
          iVar13 = pcVar6->parent_card;
        }
        pcVar7 = ((const char *(*)(int, int, int, int, int))extra)(who_chooses, tgt_player,
                                                                   tgt_card, iVar4, iVar13);
        if (pcVar7 == NULL)
          goto LAB_0265ddf0;
        if (*pcVar7 != ',')
        {
          strcat(error_str, ",");
        }
        strcat(error_str, pcVar7);
        goto LAB_0265d8b5;
      }
      if (extra != -1)
      {
        if ((special & 0x800U) != 0)
        {
          if ((-1 < extra) && (iVar4 = get_cmc(tgt_player, tgt_card), extra < iVar4))
          {
          LAB_0265e0d2:
            strcat(error_str, ",converted mana cost");
            goto LAB_0265d8b5;
          }
          goto LAB_0265ddf0;
        }
        if ((special & 0x1000000U) != 0)
        {
          if ((-1 < extra) && (iVar4 = get_cmc(tgt_player, tgt_card), iVar4 < extra))
            goto LAB_0265e0d2;
          goto LAB_0265ddf0;
        }
        if ((special & 0x100U) == 0)
        {
          if ((special & 4U) == 0)
          {
            if ((special & 0x40000U) == 0)
            {
              if (extra < 5)
              {
                sVar8 = SUBTYPE_SWAMP;
                if (extra != 0)
                {
                  if (extra == 1)
                  {
                    sVar8 = SUBTYPE_ISLAND;
                  }
                  else if (extra == 2)
                  {
                    sVar8 = SUBTYPE_FOREST;
                  }
                  else
                  {
                    sVar8 = (extra != 3) + SUBTYPE_MOUNTAIN;
                  }
                }
                iVar4 = has_subtype(tgt_player, tgt_card, sVar8);
                goto joined_r0x0265eb62;
              }
              if (pcVar5->internal_card_id == extra)
              {
                if ((special & 0x10000U) != 0)
                  goto LAB_0265eb33;
              }
              else
              {
                uVar2 = cards_data[pcVar5->internal_card_id].id;
                uVar3 = cards_data[extra].id;
                if ((special & 0x10000U) == 0)
                {
                  if (uVar2 != uVar3)
                    goto LAB_0265eb33;
                }
                else if (uVar2 == uVar3)
                {
                LAB_0265eb33:
                  strcat(error_str, EXE_STR(0x73b57c));
                  goto LAB_0265d8b5;
                }
              }
            }
            else
            {
              uVar12 = (unsigned int)extra >> 8 & 0xff;
              if ((char)((unsigned int)extra >> 8) == '\0')
              {
                uVar12 = 1;
              }
              type = ~COUNTER_P1_P1;
              if ((char)extra != '\0')
              {
                type = extra & COUNTER_invalid;
              }
              iVar4 = count_counters(tgt_player, tgt_card, type);
              if (iVar4 < (int)uVar12)
              {
                strcat(error_str, ",must have counters");
                goto LAB_0265d8b5;
              }
            }
          }
          else
          {
            iVar4 = sub_47EF50(pcVar5->internal_card_id, extra);
          joined_r0x0265eb62:
            if (iVar4 == 0)
              goto LAB_0265e5fe;
          }
          goto LAB_0265ddf0;
        }
        sVar8 = SUBTYPE_SWAMP;
        if (extra != 0)
        {
          if (extra == 1)
          {
            sVar8 = SUBTYPE_ISLAND;
          }
          else if (extra == 2)
          {
            sVar8 = SUBTYPE_FOREST;
          }
          else
          {
            sVar8 = (extra != 3) + SUBTYPE_MOUNTAIN;
          }
        }
        iVar4 = has_subtype(tgt_player, tgt_card, sVar8);
        if (iVar4 == 0)
          goto LAB_0265ddf0;
        goto LAB_0265e5fe;
      }
    LAB_0265ddf0:
      if (-1 < required_subtype)
      {
        uVar12 = special & 0x1000;
        if (required_subtype == 0x2005)
        {
          iVar4 = is_basic_land(tgt_player, tgt_card);
          if (iVar4 == 0)
          {
            iVar4 = is_what(tgt_player, tgt_card, 1);
          joined_r0x0265e97d:
            if (iVar4 != 0)
              goto joined_r0x0265e5f8;
          }
        joined_r0x0265e700:
          if (uVar12 == 0)
          {
          LAB_0265e5fe:
            strcat(error_str, EXE_STR(0x73964c));
            goto LAB_0265d8b5;
          }
        }
        else
        {
          if (required_subtype == 0x2001)
          {
            iVar4 = is_legendary(tgt_player, tgt_card);
            if (iVar4 == 0)
            {
              iVar4 = is_what(tgt_player, tgt_card, 1);
              if (iVar4 == 0)
              {
                iVar4 = check_battlefield_for_id(2, 0x1f93);
                goto joined_r0x0265e97d;
              }
              goto joined_r0x0265e700;
            }
          }
          else
          {
            uVar10 = has_subtype(tgt_player, tgt_card, required_subtype);
            bVar19 = uVar12 == 0;
            uVar12 = uVar10;
            if (bVar19)
            {
              if (uVar10 == 0)
                goto LAB_0265e5fe;
              if ((required_subtype == 0xd1) && (cards_data[pcVar5->internal_card_id].id == 0x34f))
              {
                uVar12 = is_humiliated(tgt_player, tgt_card, AL_STATIC);
                goto joined_r0x0265e700;
              }
              goto LAB_0265de50;
            }
          }
        joined_r0x0265e5f8:
          if (uVar12 != 0)
            goto LAB_0265e5fe;
        }
      }
    LAB_0265de50:
      if (power_requirement != -1)
      {
        uVar14 = power_requirement & 0xfff;
        uVar10 = power_requirement & 0x3000;
        uVar12 = (unsigned int)pcVar5->power;
        if ((((uVar10 == 0) && (uVar14 != uVar12)) ||
             (((int)uVar12 < (int)uVar14 && (uVar10 == 0x1000)))) ||
            ((uVar10 == 0x2000 && ((int)uVar14 < (int)uVar12))))
        {
          strcat(error_str, EXE_STR(0x618c1c));
          goto LAB_0265d8b5;
        }
      }
      if (toughness_requirement == -1)
        goto LAB_0265def1;
      uVar14 = toughness_requirement & 0xfff;
      uVar10 = toughness_requirement & 0x3000;
      uVar12 = (unsigned int)pcVar5->toughness;
      if ((toughness_requirement & 0x4000U) != 0)
      {
        uVar12 = uVar12 - pcVar5->damage_on_card;
      }
      if ((uVar10 == 0) && (uVar12 != uVar14))
      {
      LAB_0265e5be:
        strcat(error_str, EXE_STR(0x620600));
        goto LAB_0265d8b5;
      }
      if (uVar10 == 0x1000)
      {
        if ((int)(short)uVar12 < (int)uVar14)
          goto LAB_0265e5be;
      }
      else if ((uVar10 == 0x2000) && ((int)uVar14 < (int)(short)uVar12))
        goto LAB_0265e5be;
    LAB_0265def1:
      if (special != 0)
      {
        if ((((special & 1U) == 0) ||
             ((iVar4 = has_subtype(tgt_player, tgt_card, SUBTYPE_WALL),
               iVar4 != 0 && ((cards_data[pcVar5->internal_card_id].id != 0x34f ||
                               (iVar4 = is_humiliated(tgt_player, tgt_card, AL_STATIC),
                                iVar4 != 0)))))) &&
            (((special & 0x80U) == 0 ||
              (iVar4 = has_subtype(tgt_player, tgt_card, SUBTYPE_WALL), iVar4 == 0))))
        {
          if ((((special & 2U) == 0) ||
               ((((card_on_stack_controller == tgt_player && (card_on_stack_controller != -1)) &&
                  (card_on_stack == tgt_card)) &&
                 ((pcVar5->state & 0x80) == 0)))) &&
              (((special & 0x8000000U) == 0 ||
                (((pcVar6 = get_card_instance(stack_cards[stack_size + -1].player,
                                              stack_cards[stack_size + -1].card),
                   -1 < (int)pcVar6->state && (card_on_stack_controller != tgt_player)) &&
                  ((card_on_stack != tgt_card &&
                    ((stack_size != 0 && (-1 < (int)pcVar5->state))))))))))
          {
            if (((special & 4U) == 0) || (iVar4 = sub_47EF10(tgt_player, tgt_card), iVar4 != 0))
            {
              if (((special & 8U) == 0) ||
                  ((cards_data[pcVar5->internal_card_id].type & 0x42) == 0x42))
              {
                if ((special & 0x20U) == 0)
                {
                  if (((special & 0x100000U) != 0) && (pcVar5->damage_target_card == -1))
                    goto LAB_0265dfbd;
                LAB_0265e809:
                  if (((special & 0x200000U) == 0) ||
                      (iVar4 =
                           get_cmc((int)pcVar5->damage_target_player, pcVar5->damage_target_card),
                       iVar4 == extra))
                  {
                    if (((special & 0x2000U) == 0) ||
                        (iVar4 = has_subtype(tgt_player, tgt_card, SUBTYPE_ELEPHANT), iVar4 != 0))
                    {
                      if ((((special & 0x40U) == 0) ||
                           (iVar4 = has_subtype(tgt_player, tgt_card, SUBTYPE_EFREET),
                            iVar4 != 0)) ||
                          (iVar4 = has_subtype(tgt_player, tgt_card, SUBTYPE_DJINN), iVar4 != 0))
                      {
                        if ((special & 0x4000U) == 0)
                          goto LAB_0265e1e2;
                        if ((player == -1) || (card == -1))
                        {
                          backtrace_and_abort(
                              "functions/targets.c:644\nplayer != -1 && card != -1");
                        }
                        if ((tgt_player != player) || (tgt_card != card))
                        {
                          if (uStack_144 == 0x4000)
                            goto LAB_0265e873;
                          goto LAB_0265e1f2;
                        }
                        strcat(error_str, ",cannot target self");
                      }
                      else
                      {
                        strcat(error_str, EXE_STR(0x4ec75b));
                      }
                    }
                    else
                    {
                      strcat(error_str, EXE_STR(0x4ec749));
                    }
                  }
                  else
                  {
                    scnprintf(buffer, 100, "CMC must be %d", extra);
                    if (buffer[0] != ',')
                    {
                      strcat(error_str, ",");
                    }
                    strcat(error_str, buffer);
                  }
                }
                else if ((pcVar5->damage_target_player == who_chooses) &&
                         (((pcVar5->damage_target_card == -1 && (pcVar5->targets[4].player < 0)) &&
                           (pcVar5->targets[4].card < 0))))
                {
                  if ((special & 0x100000U) == 0)
                    goto LAB_0265e809;
                LAB_0265dfbd:
                  strcat(error_str, ",must damage creature");
                }
                else
                {
                  strcat(error_str, EXE_STR(0x786c30));
                }
              }
              else
              {
                strcat(error_str, EXE_STR(0x7398ec));
              }
            }
            else
            {
              strcat(error_str, EXE_STR(0x7a3730));
            }
          }
          else
          {
            strcat(error_str, EXE_STR(0x7865f8));
          }
        }
        else
        {
          strcat(error_str, EXE_STR(0x7369ac));
        }
        goto LAB_0265d8b5;
      }
    LAB_0265e1e2:
      if (uStack_144 == 0x4000)
      {
        if (((tgt_card != card) || (tgt_player != player)) || ((special & 0x4000U) != 0))
        {
        LAB_0265e873:
          if (((pcVar5->internal_card_id != activation_card) || (pcVar5->parent_card != card)) ||
              ((pcVar5->parent_controller != player || ((player == -1 || (card == -1))))))
            goto LAB_0265e1f2;
        }
        strcat(error_str, ",cannot target self");
        goto LAB_0265d8b5;
      }
    LAB_0265e1f2:
      if (required_state != 0)
      {
        if (((required_state & 0x2000U) == 0) || ((pcVar5->state & 0x18) != 0))
        {
          if ((required_state & 0x4000U) == 0)
          {
          LAB_0265e236:
            if (((required_state & 1U) != 0) && ((pcVar5->state & 0x10) == 0))
              goto LAB_0265eaa9;
            if (((required_state & 2U) != 0) && ((pcVar5->state & 4) == 0))
              goto LAB_0265ebf8;
            if (((required_state & 4U) != 0) && ((pcVar5->state & 0x40) == 0))
              goto LAB_0265ec13;
            if (((required_state & 8U) != 0) && ((pcVar5->state & 0x200) == 0))
              goto LAB_0265ec49;
            if (((required_state & 0x10U) != 0) &&
                (((current_turn == tgt_player || ((pcVar5->state & 8) == 0)) ||
                  (pcVar5->blocking == 0xff))))
              goto LAB_0265eb6d;
            if (((required_state & 0x20U) == 0) ||
                ((((current_phase - 0x15U < 9 && (current_turn != tgt_player)) &&
                   (pcVar5->blocking != 0xff)) ||
                  ((pcVar5->state & 4) != 0))))
            {
              if ((required_state & 0x40U) == 0)
              {
              LAB_0265e352:
                if (((required_state & 0x80U) == 0) || ((pcVar5->state & 0x80) != 0))
                {
                  if (((required_state & 0x100U) == 0) || ((pcVar5->state & 0xa0) == 0x80))
                  {
                    if (((required_state & 0x200U) == 0) ||
                        (iVar4 = sub_47E980(tgt_player, tgt_card), iVar4 != 0))
                    {
                      if (((required_state & 0x400U) == 0) || ((pcVar5->untap_status & 1) != 0))
                      {
                        if (((required_state & 0x800U) == 0) || ((pcVar5->untap_status & 2) != 0))
                        {
                          if (((required_state & 0x1000U) == 0) || ((pcVar5->state & 0x20000) != 0))
                            goto LAB_0265e3c8;
                          goto LAB_0265ea29;
                        }
                        goto LAB_0265ef2f;
                      }
                      goto LAB_0265ee51;
                    }
                    goto LAB_0265ecd2;
                  }
                  goto LAB_0265ee36;
                }
                goto LAB_0265ee6c;
              }
              iVar4 = 0;
              do
              {
                iVar18 = 0;
                if (0 < active_cards_count[iVar4])
                {
                  do
                  {
                    pcVar6 = get_card_instance(iVar4, iVar18);
                    if (((pcVar6->internal_card_id != -1) &&
                         (iVar9 = is_what(iVar4, iVar18, 4), iVar9 != 0)) &&
                        ((pcVar6->damage_target_player == tgt_player &&
                          (pcVar6->damage_target_card == tgt_card))))
                      goto LAB_0265e352;
                    iVar18 = iVar18 + 1;
                  } while (iVar18 < active_cards_count[iVar4]);
                }
                iVar4 = iVar4 + 1;
              } while (iVar4 != 2);
            LAB_0265e4b6:
              strcat(error_str, EXE_STR(0x739f60));
            }
            else
            {
              strcat(error_str, EXE_STR(0x738b4c));
            }
          }
          else if (pcVar5->kill_code == '\x02')
          {
            if (((special & 0x800000U) == 0) || ((pcVar5->token_status & 0x800000) == 0))
              goto LAB_0265e236;
            strcat(error_str, ",cannot be regenerated");
          }
          else
          {
            strcat(error_str, EXE_STR(0x786770));
          }
        }
        else
        {
        LAB_0265eaa9:
          strcat(error_str, EXE_STR(0x78f10c));
        }
        goto LAB_0265d8b5;
      }
    LAB_0265e3c8:
      if (illegal_state != 0)
      {
        if ((illegal_state & 0x4000U) != 0)
        {
          backtrace_and_abort("functions/targets.c:761\n!(illegal_state & TARGET_STATE_DESTROYED)");
        }
        if (((illegal_state & 1U) != 0) && ((pcVar5->state & 0x10) != 0))
          goto LAB_0265eaa9;
        if (((illegal_state & 0x22U) == 0) || ((pcVar5->state & 4) == 0))
        {
          if (((illegal_state & 4U) == 0) || ((pcVar5->state & 0x40) == 0))
          {
            if (((illegal_state & 8U) == 0) || ((pcVar5->state & 0x200) == 0))
            {
              if (((illegal_state & 0x30U) == 0) ||
                  ((pcVar5->blocking == 0xff || (current_turn == tgt_player))))
              {
                if ((illegal_state & 0x40U) != 0)
                {
                  iVar4 = 0;
                  do
                  {
                    iVar18 = 0;
                    if (0 < active_cards_count[iVar4])
                    {
                      do
                      {
                        pcVar6 = get_card_instance(iVar4, iVar18);
                        if ((((pcVar6->internal_card_id != -1) &&
                              (iVar9 = is_what(iVar4, iVar18, 4), iVar9 != 0)) &&
                             (pcVar6->damage_target_player == tgt_player)) &&
                            (pcVar6->damage_target_card == tgt_card))
                          goto LAB_0265e4b6;
                        iVar18 = iVar18 + 1;
                      } while (iVar18 < active_cards_count[iVar4]);
                    }
                    iVar4 = iVar4 + 1;
                  } while (iVar4 != 2);
                }
                if (((illegal_state & 0x80U) == 0) || ((pcVar5->state & 0x80) == 0))
                {
                  if (((illegal_state & 0x100U) == 0) || ((pcVar5->state & 0xa0) != 0x80))
                  {
                    if (((illegal_state & 0x200U) == 0) ||
                        (iVar4 = sub_47E980(tgt_player, tgt_card), iVar4 == 0))
                    {
                      if (((illegal_state & 0x400U) == 0) || ((pcVar5->untap_status & 1) == 0))
                      {
                        if (((illegal_state & 0x800U) == 0) || ((pcVar5->untap_status & 2) == 0))
                        {
                          if (((illegal_state & 0x1000U) == 0) ||
                              (((pcVar5->state & 0x20000) == 0 ||
                                ((cards_data[pcVar5->internal_card_id].type & 2) == 0))))
                            goto LAB_0265dbd8;
                        LAB_0265ea29:
                          strcat(error_str, EXE_STR(0x4ec769));
                        }
                        else
                        {
                        LAB_0265ef2f:
                          strcat(error_str, EXE_STR(0x73977c));
                        }
                      }
                      else
                      {
                      LAB_0265ee51:
                        strcat(error_str, EXE_STR(0x7370c8));
                      }
                    }
                    else
                    {
                    LAB_0265ecd2:
                      strcat(error_str, EXE_STR(0x6282cc));
                    }
                  }
                  else
                  {
                  LAB_0265ee36:
                    strcat(error_str, EXE_STR(0x7392d4));
                  }
                }
                else
                {
                LAB_0265ee6c:
                  strcat(error_str, EXE_STR(0x7a2d7c));
                }
              }
              else
              {
              LAB_0265eb6d:
                strcat(error_str, EXE_STR(0x62c044));
              }
            }
            else
            {
            LAB_0265ec49:
              strcat(error_str, EXE_STR(0x78735c));
            }
          }
          else
          {
          LAB_0265ec13:
            strcat(error_str, EXE_STR(0x78689c));
          }
        }
        else
        {
        LAB_0265ebf8:
          strcat(error_str, EXE_STR(0x715b3c));
        }
        goto LAB_0265d8b5;
      }
      goto LAB_0265dbd8;
    }
  LAB_0265dc88:
    strcat(error_str, EXE_STR(0x785f94));
  }
LAB_0265d8b5:
  if (return_error_str == NULL)
  {
    return 0;
  }
  iVar4 = 0;
  if (error_str[0] != '\0')
  {
    strcpy(return_error_str, error_str + 1);
    return 0;
  }
LAB_0265d8cf:
  *return_error_str = '\0';
  return iVar4;
}


/* ManalinkEh 2022: 0x265ef60; original functions/targets.c:875. */
static int target_available_impl(int player, int card, int *return_count,
                                      int test_damage_target_or_source, int who_chooses,
                                      int allowed_controller, int preferred_controller, int zone,
                                      int required_type, int illegal_type, int required_abilities,
                                      int illegal_abilities, int required_color, int illegal_color,
                                      int extra, int required_subtype, int power_requirement,
                                      int toughness_requirement, int special, int required_state,
                                      int illegal_state, int flags)
{
  int iVar1;
  card_instance_t *instance;
  unsigned int tgt_player;
  int iVar3;
  unsigned int player_00;
  bool bVar4;
  int iStack_28;
  int iStack_20;
  int iStack_14;

  if ((EXE_DWORD(0x60e9fc) & 1) != 0)
  {
    who_chooses = test_damage_target_or_source;
  }
  iStack_20 = 0;
  iVar3 = 0;
  do
  {
    iVar1 = validate_target_impl(
        player, card, iVar3, -1, NULL, who_chooses, allowed_controller, preferred_controller, zone,
        required_type, illegal_type, required_abilities, illegal_abilities, required_color,
        illegal_color, extra, required_subtype, power_requirement, toughness_requirement, special,
        required_state, illegal_state, flags);
    if (iVar1 != 0)
    {
      if (return_count == NULL)
      {
        return 1;
      }
      iStack_20 = iStack_20 + 1;
    }
    iVar3 = iVar3 + 1;
  } while (iVar3 != 2);
  if ((who_chooses == 0) || (((unsigned char)trace_mode & 2) != 0))
  {
    bVar4 = (allowed_controller & 2U) == 0;
  }
  else
  {
    bVar4 = (preferred_controller & 3U) != 0;
  }
  player_00 = (unsigned int)bVar4;
  iStack_28 = 2;
  do
  {
    iStack_14 = active_cards_count[0];
    if (active_cards_count[0] < active_cards_count[1])
    {
      iStack_14 = active_cards_count[1];
    }
    if (0 < iStack_14)
    {
      iVar3 = 0;
      do
      {
        instance = get_card_instance(player_00, iVar3);
        if (instance->internal_card_id != -1)
        {
          iVar1 = iVar3;
          tgt_player = player_00;
          if (test_damage_target_or_source != 0)
          {
            if (instance->internal_card_id != damage_card)
              goto LAB_0265f14e;
            if (test_damage_target_or_source == 1)
            {
              iVar1 = instance->damage_target_card;
              tgt_player = (int)instance->damage_target_player;
            }
            else
            {
              iVar1 = instance->damage_source_card;
              tgt_player = (int)instance->damage_source_player;
            }
          }
          iVar1 = validate_target_impl(
              player, card, tgt_player, iVar1, NULL, who_chooses, allowed_controller,
              preferred_controller, zone, required_type, illegal_type, required_abilities,
              illegal_abilities, required_color, illegal_color, extra, required_subtype,
              power_requirement, toughness_requirement, special, required_state, illegal_state,
              flags);
          if (iVar1 != 0)
          {
            if (return_count == NULL)
            {
              return 1;
            }
            iStack_20 = iStack_20 + 1;
          }
        }
      LAB_0265f14e:
        iVar3 = iVar3 + 1;
      } while (iStack_14 != iVar3);
    }
    player_00 = player_00 ^ 1;
    iStack_28 = iStack_28 + -1;
    if (iStack_28 == 0)
    {
      if (return_count != NULL)
      {
        *return_count = iStack_20;
      }
      return (unsigned int)(0 < iStack_20);
    }
  } while (true);
}


/* ManalinkEh 2022: 0x265f1f0; original functions/targets.c:1003. */
static int target_available_maybe_force(
    int player, int card, int *return_count, int who_chooses, int allowed_controller,
    int preferred_controller, int zone, int required_type, int illegal_type, int required_abilities,
    int illegal_abilities, int required_color, int illegal_color, int extra, int required_subtype,
    int power_requirement, int toughness_requirement, int special, int required_state,
    int illegal_state, int flags, int allow_cancel)
{
  int result;

  result = target_available_impl(
      player, card, return_count, 0, who_chooses, allowed_controller, preferred_controller, zone,
      required_type, illegal_type, required_abilities, illegal_abilities, required_color,
      illegal_color, extra, required_subtype, power_requirement, toughness_requirement, special,
      required_state, illegal_state, flags);
  if (((result == 0) && ((allow_cancel & 1U) == 0)) &&
      ((who_chooses == 1 || (ai_is_speculating == 1))))
  {
    result = 0;
    if ((allowed_controller & 2U) != 0)
    {
      if ((preferred_controller & 2U) == 0)
      {
        result = target_available_impl(
            player, card, return_count, 0, who_chooses, allowed_controller,
            preferred_controller ^ 1, zone, required_type, illegal_type, required_abilities,
            illegal_abilities, required_color, illegal_color, extra, required_subtype,
            power_requirement, toughness_requirement, special, required_state, illegal_state,
            flags);
        return result;
      }
      result = 0;
    }
  }
  return result;
}


/* ManalinkEh 2022: 0x265fcf0; original functions/targets.c:840. */
int real_validate_target(int tgt_player, int tgt_card, char *return_error_str, int who_chooses,
                              int allowed_controller, int preferred_controller, int zone,
                              int required_type, int illegal_type, int required_abilities,
                              int illegal_abilities, int required_color, int illegal_color,
                              int extra, int required_subtype, int power_requirement,
                              int toughness_requirement, int special, int required_state,
                              int illegal_state)
{
  int iVar1;
  int iVar2;
  int iVar3;
  unsigned int flags;
  uint32_t uStack_14;
  iVar3 = hack_gpf_player;
  iVar1 = hack_gpf_card;
  if ((illegal_abilities & 0x20000U) == 0)
  {
    iVar1 = -1;
    uStack_14 = -1;
    flags = 0;
  }
  else
  {
    uStack_14 = hack_gpf_player;
    if ((hack_gpf_player < 0) || (hack_gpf_card < 0))
    {
      flags = 0;
    }
    else
    {
      iVar2 = has_subtype(hack_gpf_player, hack_gpf_card, SUBTYPE_AURA);
      flags = 0;
      if (iVar2 != 0)
      {
        flags = 2;
      }
      iVar3 = has_subtype(iVar3, iVar1, SUBTYPE_EQUIPMENT);
      if (iVar3 != 0)
      {
        flags = flags | 4;
      }
    }
  }
  iVar1 = validate_target_impl(
      uStack_14, iVar1, tgt_player, tgt_card, return_error_str, who_chooses, allowed_controller,
      preferred_controller, zone, required_type, illegal_type, required_abilities,
      illegal_abilities, required_color, illegal_color, extra, required_subtype, power_requirement,
      toughness_requirement, special, required_state, illegal_state, flags);
  return iVar1;
}


/* ManalinkEh 2022: 0x265fe80; original functions/targets.c:970. */
int real_target_available(int *return_count, int test_damage_target_or_source, int who_chooses,
                               int allowed_controller, int preferred_controller, int zone,
                               int required_type, int illegal_type, int required_abilities,
                               int illegal_abilities, int required_color, int illegal_color,
                               int extra, int required_subtype, int power_requirement,
                               int toughness_requirement, int special, int required_state,
                               int illegal_state)
{
  int iVar1;
  int iVar2;
  unsigned int flags;
  int iStack_14;
  iStack_14 = hack_gpf_player;
  iVar1 = hack_gpf_card;
  if ((illegal_abilities & 0x20000U) == 0)
  {
    iVar1 = -1;
    iStack_14 = -1;
    flags = 0;
  }
  else if ((hack_gpf_player < 0) || (hack_gpf_card < 0))
  {
    flags = 0;
  }
  else
  {
    iVar2 = has_subtype(hack_gpf_player, hack_gpf_card, SUBTYPE_AURA);
    flags = 0;
    if (iVar2 != 0)
    {
      flags = 2;
    }
    iVar2 = has_subtype(iStack_14, iVar1, SUBTYPE_EQUIPMENT);
    if (iVar2 != 0)
    {
      flags = flags | 4;
    }
  }
  if ((unsigned int)test_damage_target_or_source < 3)
  {
    iVar1 = target_available_impl(iStack_14, iVar1, return_count, test_damage_target_or_source,
                                       who_chooses, allowed_controller, preferred_controller, zone,
                                       required_type, illegal_type, required_abilities,
                                       illegal_abilities, required_color, illegal_color, extra,
                                       required_subtype, power_requirement, toughness_requirement,
                                       special, required_state, illegal_state, flags);
    return iVar1;
  }
  return 0;
}


/* ManalinkEh 2022: 0x26604c0; original functions/targets.c:1393. */
int can_target(target_definition_t *td)
{
  int result;
  result = target_available_maybe_force(
      td->player, td->card, NULL, td->who_chooses, td->allowed_controller, td->preferred_controller,
      td->zone, td->required_type, td->illegal_type, td->required_abilities, td->illegal_abilities,
      td->required_color, td->illegal_color, td->extra, td->required_subtype, td->power_requirement,
      td->toughness_requirement, td->special, td->required_state, td->illegal_state, td->flags,
      td->allow_cancel);
  return result;
}


/* ManalinkEh 2022: 0x2660570; original functions/targets.c:1419. */
int target_available(int player, int card, target_definition_t *td)
{
  int count;
  count = 0;
  target_available_maybe_force(
      player, card, &count, td->who_chooses, td->allowed_controller, td->preferred_controller,
      td->zone, td->required_type, td->illegal_type, td->required_abilities, td->illegal_abilities,
      td->required_color, td->illegal_color, td->extra, td->required_subtype, td->power_requirement,
      td->toughness_requirement, td->special, td->required_state, td->illegal_state, td->flags,
      td->allow_cancel);
  return count;
}


/* ManalinkEh 2022: 0x2660750; original functions/targets.c:1895. */
int would_validate_arbitrary_target(target_definition_t *td, int tgt_player, int tgt_card)
{
  int result;
  target_card = tgt_card;
  target_controller = tgt_player;
  result = validate_target_impl(
      td->player, td->card, tgt_player, tgt_card, NULL, td->who_chooses, td->allowed_controller,
      td->preferred_controller, td->zone, td->required_type, td->illegal_type,
      td->required_abilities, td->illegal_abilities, td->required_color, td->illegal_color,
      td->extra, td->required_subtype, td->power_requirement, td->toughness_requirement,
      td->special, td->required_state, td->illegal_state, td->flags);
  return result;
}


/* ManalinkEh 2022: 0x2660810; original functions/targets.c:1888. */
int validate_arbitrary_target(target_definition_t *td, int tgt_player, int tgt_card)
{
  int32_t iVar1;
  int iVar2;

  iVar1 = td->preferred_controller;
  td->preferred_controller = td->allowed_controller;
  iVar2 = would_validate_arbitrary_target(td, tgt_player, tgt_card);
  td->preferred_controller = iVar1;
  return iVar2;
}


/* ManalinkEh 2022: 0x2660850; original functions/targets.c:1915. */
int would_validate_target(int player, int card, target_definition_t *td,
                               int target_number)
{
  card_instance_t *instance;
  int result;

  instance = get_card_instance(player, card);
  if ((int)(unsigned int)instance->number_of_targets <= target_number)
  {
    target_number = 0;
  }
  target_controller = instance->targets[target_number].player;
  target_card = instance->targets[target_number].card;
  result = validate_target_impl(
      player, card, target_controller, target_card, NULL, td->who_chooses, td->allowed_controller,
      td->preferred_controller, td->zone, td->required_type, td->illegal_type,
      td->required_abilities, td->illegal_abilities, td->required_color, td->illegal_color,
      td->extra, td->required_subtype, td->power_requirement, td->toughness_requirement,
      td->special, td->required_state, td->illegal_state, td->flags);
  return result;
}


/* ManalinkEh 2022: 0x2661360; original functions/targets.c:1884. */
int would_valid_target(target_definition_t *td)
{
  int result;
  result = would_validate_target(td->player, td->card, td, 0);
  return result;
}


/* ManalinkEh 2022: 0x2661710; original functions/targets.c:1908. */
int validate_target(int player, int card, target_definition_t *td, int target_number)
{
  int32_t iVar1;
  int iVar2;

  iVar1 = td->preferred_controller;
  td->preferred_controller = td->allowed_controller;
  iVar2 = would_validate_target(player, card, td, target_number);
  td->preferred_controller = iVar1;
  return iVar2;
}


/* ManalinkEh 2022: 0x2661750; original functions/targets.c:1881. */
int valid_target(target_definition_t *td)
{
  int32_t iVar1;
  int iVar2;

  iVar1 = td->preferred_controller;
  td->preferred_controller = td->allowed_controller;
  iVar2 = would_validate_target(td->player, td->card, td, 0);
  td->preferred_controller = iVar1;
  return iVar2;
}

#endif


#ifndef SHANDALAR
typedef union
{
  char error_reason[208];
  char multiblocker_error[200];
  target_t candidates[60];
} target_selection_storage_t;

/* ManalinkEh 2022: 0x265f3d0; original functions/targets.c:1042. */
static int select_target_impl(int player, int card, int who_chooses, int allowed_controller,
                                   int preferred_controller, int zone, int required_type,
                                   int illegal_type, int required_abilities, int illegal_abilities,
                                   int required_color, int illegal_color, int extra,
                                   int required_subtype, int power_requirement,
                                   int toughness_requirement, int special, int required_state,
                                   int illegal_state, int flags, const char *prompt,
                                   int allow_cancel, target_t *ret_tgt)
{
  int32_t iVar2;
  int iVar3;
  card_instance_t *instance;
  int iVar5;
  int iVar6;
  int iVar7;
  unsigned int argument11;
  unsigned int uStack_2d8;
  unsigned int uStack_2d4;
  int unknown_v44;
  target_t tmp_tgt;
  char illegal_tgt[200];
  target_selection_storage_t selection;
  if (((who_chooses != 1) || (((unsigned char)trace_mode & 2) != 0)) && (ai_is_speculating != 1))
  {
    if (zone == 0)
    {
      if ((allowed_controller & 2U) == 0)
      {
        argument11 = allowed_controller & 1;
        iVar7 = 0;
        if (argument11 == 0)
          goto LAB_0265f9e4;
      }
      else
      {
        argument11 = 1;
        iVar7 = 1;
      }
    }
    else
    {
      argument11 = 0;
      iVar7 = 0;
      if ((zone & 0x1000U) != 0)
      {
        argument11 = 1;
        if ((allowed_controller & 2U) == 0)
        {
          argument11 = allowed_controller & 1;
          iVar7 = 0;
          if (argument11 != 0)
            goto LAB_0265f41e;
        }
      LAB_0265f9e4:
        iVar7 = 1;
      }
    LAB_0265f41e:
      if ((zone & 0x4000U) != 0)
      {
        if (1 < stack_size)
        {
          uStack_2d4 = 1;
          goto joined_r0x0265f8ae;
        }
        if ((stack_size == 1) &&
            ((uStack_2d4 = 1, stack_cards[0].card != card || (stack_cards[0].player != player))))
          goto joined_r0x0265f8ae;
      }
    }
    uStack_2d4 = sub_45CEC0();
  joined_r0x0265f8ae:
    if (prompt == NULL)
    {
      prompt = EXE_STR(0x60a690);
    }
    else if ((hack_prepend_prompt != NULL) && (ai_is_speculating != 1))
    {
      sprintf((char *)EXE_STR(0x60a690), "%s: %s", hack_prepend_prompt, prompt);
      prompt = EXE_STR(0x60a690);
    }
  LAB_0265f467:
    while (iVar3 = sub_4378E0(who_chooses, -1, prompt, allow_cancel, -1, -1, -1, -1, &unknown_v44,
                              &tmp_tgt, argument11, iVar7),
           iVar3 == 0)
    {
      if (unknown_v44 == -2)
      {
        if ((EXE_DWORD(0x4ef1b0) == -1) && (EXE_DWORD(0x4ef1ac) == -1))
          goto LAB_0265f930;
        if ((zone & 0x2000U) != 0)
        {
          tmp_tgt.card = -3;
          tmp_tgt.player = -1;
        LAB_0265f930:
          ret_tgt->player = tmp_tgt.player;
          ret_tgt->card = tmp_tgt.card;
          if (uStack_2d4 == 0)
          {
            sub_45CF00();
          }
        LAB_0265f5e0:
          set_centerwindow_txt("");
          text_lines[0][0] = '\0';
          return iVar3;
        }
      }
    }
    if (-1 < tmp_tgt.card)
    {
      instance = get_card_instance(tmp_tgt.player, tmp_tgt.card);
      if (((-1 < instance->internal_card_id) && ((special & 0x400U) == 0)) &&
          (cards_at_7c7000[instance->internal_card_id]->code_pointer == (CardFunction)0x401010))
      {
        if (ai_is_speculating != 1)
        {
          sprintf(selection.multiblocker_error, EXE_STR(0x73940c), EXE_STR(0x728f6c));
          display_error_message(selection.multiblocker_error);
        }
        goto LAB_0265f467;
      }
    }
    iVar5 = validate_target_impl(
        player, card, tmp_tgt.player, tmp_tgt.card, selection.error_reason, who_chooses,
        allowed_controller, preferred_controller, zone, required_type, illegal_type,
        required_abilities, illegal_abilities, required_color, illegal_color, extra,
        required_subtype, power_requirement, toughness_requirement, special, required_state,
        illegal_state, flags);
    if (iVar5 != 0)
    {
      ret_tgt->player = tmp_tgt.player;
      ret_tgt->card = tmp_tgt.card;
      if (uStack_2d4 == 0)
      {
        sub_45CF00();
      }
      EXE_DWORD(0x60a530) = 0;
      goto LAB_0265f5e0;
    }
    if (ai_is_speculating != 1)
    {
      if (selection.error_reason[0] == '\0')
      {
        display_error_message(EXE_STR(0x73732c));
      }
      else
      {
        sprintf(illegal_tgt, EXE_STR(0x73940c), selection.error_reason);
        display_error_message(illegal_tgt);
      }
    }
    goto LAB_0265f467;
  }
  uStack_2d8 = EXE_DWORD(0x60e9fc) & 1;
  if ((uStack_2d8 == 0) || ((allowed_controller & 2U) == 0))
  {
    if (zone != 0)
      goto LAB_0265f62c;
  }
  else
  {
    preferred_controller = allowed_controller;
    uStack_2d4 = uStack_2d8;
    if (zone == 0)
      goto joined_r0x0265f86d;
  LAB_0265f62c:
    uStack_2d8 = 0;
    uStack_2d4 = 0;
    if ((zone & 0x1000U) == 0)
      goto joined_r0x0265f86d;
  }
  if ((preferred_controller & 2U) == 0)
  {
    uStack_2d8 = preferred_controller & 1;
    uStack_2d4 = 0;
    if (uStack_2d8 != 0)
      goto joined_r0x0265f86d;
  }
  else
  {
    uStack_2d8 = 1;
  }
  uStack_2d4 = 1;
joined_r0x0265f86d:
  if (spell_fizzled != 1)
  {
    iVar7 = 0;
    iVar3 = 0;
    do
    {
      iVar5 = 0;
      if (0 < active_cards_count[iVar7])
      {
        do
        {
          instance = get_card_instance(iVar7, iVar5);
          if ((instance->internal_card_id != -1) &&
              (iVar6 = validate_target_impl(
                   player, card, iVar7, iVar5, NULL, who_chooses, allowed_controller,
                   preferred_controller, zone, required_type, illegal_type, required_abilities,
                   illegal_abilities, required_color, illegal_color, extra, required_subtype,
                   power_requirement, toughness_requirement, special, required_state, illegal_state,
                   flags),
               iVar6 != 0))
          {
            selection.candidates[iVar3].player = iVar7;
            selection.candidates[iVar3].card = iVar5;
            iVar3 = iVar3 + 1;
            if (0x39 < iVar3)
              goto LAB_0265f755;
          }
          iVar5 = iVar5 + 1;
        } while (iVar5 < active_cards_count[iVar7]);
      }
      iVar7 = iVar7 + 1;
    } while (iVar7 != 2);
  LAB_0265f755:
    if (uStack_2d8 != 0)
    {
      selection.candidates[iVar3].player = 1;
      selection.candidates[iVar3].card = -1;
      iVar3 = iVar3 + 1;
    }
    if (uStack_2d4 != 0)
    {
      selection.candidates[iVar3].player = 0;
      selection.candidates[iVar3].card = -1;
      iVar3 = iVar3 + 1;
    }
    if (iVar3 != 0)
    {
      EXE_DWORD(0x4ed954) = 3;
      if (ai_is_speculating == 1)
      {
        EXE_DWORD(0x7a2fe4) = internal_rand(iVar3);
        EXE_DWORD(0x78fa30) = (selection.candidates[EXE_DWORD(0x7a2fe4)].player < 1 ? 0 : 0x100) |
                              (selection.candidates[EXE_DWORD(0x7a2fe4)].card & 0xff) | 0x4000;
        sub_498F20();
      }
      else
      {
        sub_499050();
        if ((iVar3 <= EXE_DWORD(0x7a2fe4)) || (EXE_DWORD(0x7a2fe4) == 99))
        {
          EXE_DWORD(0x7a2fe4) = internal_rand(iVar3);
        }
      }
      EXE_DWORD(0x60a550) = selection.candidates[EXE_DWORD(0x7a2fe4)].player;
      iVar2 = selection.candidates[EXE_DWORD(0x7a2fe4)].card;
      ret_tgt->player = selection.candidates[EXE_DWORD(0x7a2fe4)].player;
      ret_tgt->card = iVar2;
      return 1;
    }
  }
  return 0;
}


/* ManalinkEh 2022: 0x265fb20; original functions/targets.c:1258. */
static int select_target_maybe_force(
    int player, int card, int who_chooses, int allowed_controller, int preferred_controller,
    int zone, int required_type, int illegal_type, int required_abilities, int illegal_abilities,
    int required_color, int illegal_color, int extra, int required_subtype, int power_requirement,
    int toughness_requirement, int special, int required_state, int illegal_state, int flags,
    const char *prompt, int allow_cancel, target_t *ret_tgt)
{
  int result;

  result = select_target_impl(
      player, card, who_chooses, allowed_controller, preferred_controller, zone, required_type,
      illegal_type, required_abilities, illegal_abilities, required_color, illegal_color, extra,
      required_subtype, power_requirement, toughness_requirement, special, required_state,
      illegal_state, flags, prompt, allow_cancel, ret_tgt);
  if ((result == 0) && ((allow_cancel & 1U) == 0))
  {
    if (((who_chooses == 1) || (ai_is_speculating == 1)) &&
        ((result = 0,
          (allowed_controller & 2U) != 0 && (result = 0, (preferred_controller & 2U) == 0))))
    {
      result = select_target_impl(
          player, card, who_chooses, allowed_controller, preferred_controller ^ 1, zone,
          required_type, illegal_type, required_abilities, illegal_abilities, required_color,
          illegal_color, extra, required_subtype, power_requirement, toughness_requirement, special,
          required_state, illegal_state, flags, prompt, allow_cancel, ret_tgt);
      if (result != 0)
      {
        ai_modifier = ai_modifier + -0x20;
      }
    }
  }
  return result;
}


/* ManalinkEh 2022: 0x2660010; original functions/targets.c:1295. */
int real_select_target(int who_chooses, int allowed_controller, int preferred_controller,
                            int zone, int required_type, int illegal_type, int required_abilities,
                            int illegal_abilities, int required_color, int illegal_color, int extra,
                            int required_subtype, int power_requirement, int toughness_requirement,
                            int special, int required_state, int illegal_state, const char *prompt,
                            int allow_cancel, target_t *ret_tgt)
{
  int iVar1;
  int iVar2;
  int iVar3;
  unsigned int flags;
  uint32_t uStack_14;
  iVar3 = hack_gpf_player;
  iVar1 = hack_gpf_card;
  if ((illegal_abilities & 0x20000U) == 0)
  {
    iVar1 = -1;
    uStack_14 = -1;
    flags = 0;
  }
  else
  {
    uStack_14 = hack_gpf_player;
    if ((hack_gpf_player < 0) || (hack_gpf_card < 0))
    {
      flags = 0;
    }
    else
    {
      iVar2 = has_subtype(hack_gpf_player, hack_gpf_card, SUBTYPE_AURA);
      flags = 0;
      if (iVar2 != 0)
      {
        flags = 2;
      }
      iVar3 = has_subtype(iVar3, iVar1, SUBTYPE_EQUIPMENT);
      if (iVar3 != 0)
      {
        flags = flags | 4;
      }
    }
  }
  iVar1 = select_target_maybe_force(
      uStack_14, iVar1, who_chooses, allowed_controller, preferred_controller, zone, required_type,
      illegal_type, required_abilities, illegal_abilities, required_color, illegal_color, extra,
      required_subtype, power_requirement, toughness_requirement, special, required_state,
      illegal_state, flags, prompt, allow_cancel, ret_tgt);
  return iVar1;
}


/* ManalinkEh 2022: 0x2660620; original functions/targets.c:1700. */
int pick_next_target_noload_arbitrary(target_definition_t *td, const char *prompt,
                                           int player, int card)
{
  card_instance_t *instance;
  int result;

  instance = get_card_instance(player, card);
  result = select_target_maybe_force(
      td->player, td->card, td->who_chooses, td->allowed_controller, td->preferred_controller,
      td->zone, td->required_type, td->illegal_type, td->required_abilities, td->illegal_abilities,
      td->required_color, td->illegal_color, td->extra, td->required_subtype, td->power_requirement,
      td->toughness_requirement, td->special, td->required_state, td->illegal_state, td->flags,
      prompt, td->allow_cancel, instance->targets + instance->number_of_targets);
  if (result != 0)
  {
    instance->number_of_targets = instance->number_of_targets + '\x01';
    return result;
  }
  spell_fizzled = 1;
  return 0;
}


/* ManalinkEh 2022: 0x2660710; original functions/targets.c:1692. */
int pick_next_target_arbitrary(target_definition_t *td, const char *prompt, int player,
                                    int card)
{
  int result;
  load_text(NULL, prompt);
  result = pick_next_target_noload_arbitrary(td, text_lines[0], player, card);
  return result;
}


/* ManalinkEh 2022: 0x2660930; original functions/targets.c:1447. */
int choose_default_target(int player, int card, target_definition_t *td)
{
  int32_t iVar1;
  int32_t iVar2;
  card_instance_t *instance;
  int iVar4;
  FILE *pFVar5;
  int iVar6;
  int iVar7;
  char buffer[500];

  instance = get_card_instance(player, card);
  iVar4 = get_setting(7);
  if (iVar4 != 0)
  {
    if (read_auto_target_file == 0)
    {
      pFVar5 = fopen("TargetsHuman.txt", "r");
      iVar4 = 0;
      while (iVar7 = fscanf(pFVar5, "%[.]", buffer), iVar7 == 1)
      {
        fscanf(pFVar5, "%[0-9]", buffer);
        iVar7 = atoi(buffer);
        auto_targets[iVar4] = iVar7;
        auto_targets_target[iVar4] = 0;
        fscanf(pFVar5, "%[^\n]", buffer);
        fscanf(pFVar5, "%[\n]", buffer);
        iVar4 = iVar4 + 1;
      }
      fclose(pFVar5);
      pFVar5 = fopen("TargetsAI.txt", "r");
      while (iVar7 = fscanf(pFVar5, "%[.]", buffer), iVar7 == 1)
      {
        fscanf(pFVar5, "%[0-9]", buffer);
        iVar7 = atoi(buffer);
        auto_targets[iVar4] = iVar7;
        auto_targets_target[iVar4] = 1;
        fscanf(pFVar5, "%[^\n]", buffer);
        fscanf(pFVar5, "%[\n]", buffer);
        iVar4 = iVar4 + 1;
      }
      fclose(pFVar5);
      auto_targets[iVar4] = -1;
      read_auto_target_file = 1;
    }
    iVar4 = get_id(player, card);
    iVar1 = td->allowed_controller;
    iVar7 = 0;
    iVar2 = td->preferred_controller;
    do
    {
      if ((auto_targets[iVar7] == iVar4) && (-1 < auto_targets[iVar7]))
      {
        iVar4 = auto_targets_target[iVar7];
        td->allowed_controller = iVar4;
        td->preferred_controller = iVar4;
        break;
      }
      iVar7 = iVar7 + 1;
    } while (iVar7 != 500);
    iVar4 = target_available(player, card, td);
    if (iVar4 == 1)
    {
      iVar4 = 0;
      do
      {
        iVar7 = -1;
        if (-1 < active_cards_count[iVar4])
        {
          do
          {
            instance->targets[0].player = iVar4;
            instance->targets[0].card = iVar7;
            instance->number_of_targets = '\0';
            iVar6 = would_validate_target(player, card, td, 0);
            if (iVar6 != 0)
            {
              return 1;
            }
            iVar7 = iVar7 + 1;
          } while (iVar7 < active_cards_count[iVar4]);
        }
        iVar4 = iVar4 + 1;
      } while (iVar4 != 2);
    }
    else
    {
      td->allowed_controller = iVar1;
      td->preferred_controller = iVar2;
    }
  }
  return 0;
}


/* ManalinkEh 2022: 0x2660bb0; original functions/targets.c:1722. */
int select_target(int player, int card, target_definition_t *td, const char *prompt,
                       target_t *ret_location)
{
  int result;
  card_instance_t *instance;

  if (card < -500)
  {
    card = card + 1000;
    instance = NULL;
    if (ret_location == NULL)
    {
      backtrace_and_abort("functions/targets.c:1727\nret_location");
    }
  }
  else
  {
    instance = get_card_instance(player, card);
    if (ret_location == NULL)
    {
      ret_location = instance->targets;
    }
    result = check_special_flags(player, card, 0x20000000);
    if ((result != 0) && (ret_location->player != -1))
    {
      return 1;
    }
    result = choose_default_target(player, card, td);
    if (result != 0)
    {
      return 1;
    }
  }
  result = check_special_flags(player, card, 0x20000000);
  if ((result == 0) || (ret_location->player == -1))
  {
    result = select_target_maybe_force(
        player, card, td->who_chooses, td->allowed_controller, td->preferred_controller, td->zone,
        td->required_type, td->illegal_type, td->required_abilities, td->illegal_abilities,
        td->required_color, td->illegal_color, td->extra, td->required_subtype,
        td->power_requirement, td->toughness_requirement, td->special, td->required_state,
        td->illegal_state, td->flags, prompt, td->allow_cancel, ret_location);
    if (result == 0)
    {
      return 0;
    }
    if (instance != NULL)
    {
      instance->number_of_targets = instance->number_of_targets + '\x01';
    }
  }
  return 1;
}


/* ManalinkEh 2022: 0x2660d50; original functions/targets.c:1520. */
int pick_target(target_definition_t *td, const char *prompt)
{
  int result;

  load_text(NULL, prompt);
  result = select_target(td->player, td->card, td, text_lines[0], NULL);
  if (result == 0)
  {
    spell_fizzled = 1;
  }
  return result;
}


/* ManalinkEh 2022: 0x2660db0; original functions/targets.c:1531. */
int new_pick_target(target_definition_t *td, const char *prompt, int ret_location,
                         int mode)
{
  card_instance_t *instance;
  int result;

  instance = get_card_instance(td->player, td->card);
  if (ret_location == -1)
  {
    ret_location = (int)instance->number_of_targets;
  }
  if ((mode & 0x80U) == 0)
  {
    load_text(NULL, prompt);
    prompt = text_lines[0];
  }
  result = select_target(td->player, td->card, td, prompt, instance->targets + ret_location);
  if ((result == 0) && ((mode & 1U) != 0))
  {
    spell_fizzled = 1;
  }
  return result;
}


/* ManalinkEh 2022: 0x2660e50; original functions/targets.c:1552. */
int pick_next_target_noload(target_definition_t *td, const char *prompt)
{
  card_instance_t *instance;
  int result;

  instance = get_card_instance(td->player, td->card);
  result = select_target(td->player, td->card, td, prompt,
                              instance->targets + instance->number_of_targets);
  if (result == 0)
  {
    spell_fizzled = 1;
  }
  return result;
}


/* ManalinkEh 2022: 0x2660eb0; original functions/targets.c:1562. */
int pick_up_to_n_targets_noload(target_definition_t *td, const char *prompt, int num)
{
  if (num <= 0)
    return 0;

  card_instance_t *instance = get_card_instance(td->player, td->card);
  instance->number_of_targets = 0;

  int len = strlen(prompt) + 50;
  char prompt_w_count[len];
  prompt_w_count[0] = 0;

  char fmt[100];
  if (IS_AI(td->who_chooses))
    strcpy(fmt, "%s %d %d");
  else
  {
    load_text(0, "TARGET_COUNT");
    strcpy(fmt, (td->allow_cancel & 2) ? text_lines[1] : text_lines[0]);
  }

  int already_chose_player[2] = {0};
  target_t tgt;

  int i, picked_cancel = 0;
  for (i = 0; i < num && can_target(td); ++i)
  {
    if (!IS_AI(td->who_chooses))
      scnprintf(prompt_w_count, len, fmt, prompt, i + 1, num);

    if (select_target(td->player, td->card - 1000, td, prompt_w_count, &tgt))
    {
      if (tgt.card != -1)
        get_card_instance(tgt.player, tgt.card)->state |= STATE_TARGETTED | STATE_CANNOT_TARGET;
      else if (!already_chose_player[tgt.player])
        already_chose_player[tgt.player] = 1;
      else
      {
        if (!IS_AI(td->who_chooses))
        {
          load_text(0, "FIREBALL");
          display_error_message(text_lines[1]);
        }
        --i;
        continue;
      }

      instance->targets[i] = tgt;
      ++instance->number_of_targets;
    }
    else
    {
      picked_cancel = tgt.card == -1;
      break;
    }
  }

  for (i = 0; i < instance->number_of_targets; ++i)
    if (instance->targets[i].card != -1)
      get_card_instance(instance->targets[i].player, instance->targets[i].card)->state &=
          ~(STATE_TARGETTED | STATE_CANNOT_TARGET);

  if (picked_cancel)
  {
    instance->number_of_targets = 0;
    spell_fizzled = 1;
  }

  return instance->number_of_targets;
}


/* ManalinkEh 2022: 0x2661180; original functions/targets.c:1636. */
int pick_up_to_n_targets(target_definition_t *td, const char *prompt, int num)
{
  int result;
  char loaded[300];
  if (ai_is_speculating == 1)
  {
    loaded[0] = '\0';
  }
  else
  {
    load_text(NULL, prompt);
    scnprintf(loaded, 300, "%s", text_lines[0]);
  }
  result = pick_up_to_n_targets_noload(td, loaded, num);
  return result;
}


/* ManalinkEh 2022: 0x2661200; original functions/targets.c:1653. */
int mark_up_to_n_targets_noload(target_definition_t *td, const char *prompt, int num,
                                     char (*marked)[151])
{
  int iVar1;
  int iVar2;
  card_instance_t *instance;
  int card;
  int iVar4;
  bool bVar5;
  target_t tgt;

  if (num == 0)
  {
    return 0;
  }
  if (num < 0)
  {
    num = 500;
  }
  iVar4 = 0;
  while (true)
  {
    iVar2 = can_target(td);
    if (iVar2 == 0)
    {
      bVar5 = false;
      goto LAB_026612a1;
    }
    iVar2 = select_target(td->player, td->card + -1000, td, prompt, &tgt);
    if (iVar2 == 0)
      break;
    iVar4 = iVar4 + 1;
    marked[tgt.player][tgt.card] = '\x01';
    instance = get_card_instance(tgt.player, tgt.card);
    instance->state = instance->state | 0x300000;
    if (num <= iVar4)
    {
      bVar5 = false;
    LAB_026612a1:
      iVar2 = 0;
      do
      {
        card = 0;
        if (0 < active_cards_count[iVar2])
        {
          do
          {
            while ((*marked)[card] == '\0')
            {
              card = card + 1;
              if (active_cards_count[iVar2] <= card)
                goto LAB_026612f2;
            }
            instance = get_card_instance(iVar2, card);
            card = card + 1;
            iVar1 = active_cards_count[iVar2];
            instance->state = instance->state & 0xffcfffff;
          } while (card < iVar1);
        }
      LAB_026612f2:
        iVar2 = iVar2 + 1;
        marked = marked + 1;
        if (iVar2 == 2)
        {
          if (bVar5)
          {
            spell_fizzled = 1;
            iVar4 = 0;
          }
          return iVar4;
        }
      } while (true);
    }
  }
  bVar5 = tgt.card == -1;
  goto LAB_026612a1;
}


/* ManalinkEh 2022: 0x2661390; original functions/targets.c:1766. */
int pick_player_duh(int player, int card, int preferred_controller, int allow_cancel)
{
  card_instance_t *instance;
  int result;
  target_definition_t td;

  instance = get_card_instance(player, card);
  default_target_definition(player, card, &td, 0);
  td.preferred_controller = preferred_controller;
  td.zone = 0x1000;
  td.allow_cancel = allow_cancel;
  result = duh_mode(player);
  if (result != 0)
  {
    instance->number_of_targets = '\x01';
    instance->targets[0].player = preferred_controller;
    instance->targets[0].card = -1;
    result = would_validate_target(td.player, td.card, &td, 0);
    if (result != 0)
    {
      return 1;
    }
    if ((allow_cancel & 1U) != 0)
    {
      spell_fizzled = 1;
      return 0;
    }
  }
  instance->number_of_targets = '\0';
  result = can_target(&td);
  if (result == 0)
  {
    return 0;
  }
  result = pick_target(&td, "TARGET_PLAYER");
  return (unsigned int)(result != 0);
}


/* ManalinkEh 2022: 0x26615c0; original functions/targets.c:1829. */
int target_opponent(int player, int card)
{
  card_instance_t *instance;
  int result;
  target_definition_t td;

  instance = get_card_instance(player, card);
  default_target_definition(player, card, &td, 0);
  td.preferred_controller = 1 - player;
  td.allow_cancel = 0;
  instance->number_of_targets = '\x01';
  instance->targets[0].player = td.preferred_controller;
  instance->targets[0].card = -1;
  td.zone = 0x1000;
  result = would_validate_target(td.player, td.card, &td, 0);
  if (result == 0)
  {
    instance->targets[0].player = -1;
    instance->number_of_targets = '\0';
    spell_fizzled = 1;
    return 0;
  }
  return 1;
}


/* ManalinkEh 2022: 0x2661670; original functions/targets.c:1856. */
int opponent_is_valid_target(int player, int card)
{
  uint8_t uVar1;
  int32_t iVar2;
  int32_t iVar3;
  card_instance_t *instance;
  int iVar5;
  target_definition_t td;

  instance = get_card_instance(player, card);
  default_target_definition(player, card, &td, 0);
  td.allow_cancel = 0;
  uVar1 = instance->number_of_targets;
  td.preferred_controller = 1 - player;
  instance->number_of_targets = '\x01';
  iVar2 = instance->targets[0].player;
  iVar3 = instance->targets[0].card;
  instance->targets[0].player = td.preferred_controller;
  instance->targets[0].card = -1;
  td.zone = 0x1000;
  iVar5 = would_validate_target(td.player, td.card, &td, 0);
  instance->targets[0].player = iVar2;
  instance->targets[0].card = iVar3;
  instance->number_of_targets = uVar1;
  return iVar5;
}


/* ManalinkEh 2022: 0x2661790; original functions/targets.c:1953. */
int autoselect_target(int player, int card, int i, int j, int k, int who_chooses,
                           int allowed_controller, int preferred_controller, int zone,
                           int required_type, int illegal_type, int required_abilities,
                           int illegal_abilities, int required_color, int illegal_color, int extra,
                           int required_subtype, int power_requirement, int toughness_requirement,
                           int special, int required_state, int illegal_state, int flags,
                           int prompt, int allow_cancel, int ret_location)
{
  int result;
  target_definition_t td;

  if (who_chooses != 1)
  {
    default_target_definition(player, card, &td, 0xff);
    td.allowed_controller = allowed_controller;
    td.preferred_controller = preferred_controller;
    td.zone = zone;
    td.required_type = required_type;
    td.illegal_type = illegal_type;
    td.required_color = required_color;
    td.illegal_color = illegal_color;
    td.extra = extra;
    td.required_subtype = required_subtype;
    td.power_requirement = power_requirement;
    td.toughness_requirement = toughness_requirement;
    td.special = special;
    td.required_state = required_state;
    td.illegal_state = illegal_state;
    td.allow_cancel = allow_cancel;
    td.flags = flags;
    result = choose_default_target(player, card, &td);
    return (unsigned int)(result != 0);
  }
  return 0;
}

#endif


#ifndef SHANDALAR
/* October 2022 private targeting result at 0x27748c8 (native function-local static). */
static target_t targeted_subtype_spell_result;
#endif


#ifndef SHANDALAR
/* ManalinkEh 2022: 0x2661a70; original functions/targets.c:2092. */
target_t *becomes_target_of_subtype_spell(int player, int card, event_t event, int t_player,
                                               int t_card, int controlled_by,
                                               resolve_trigger_t trigger_mode,
                                               int targeting_subtype)
{
  int result;
  if (trigger_condition != TRIGGER_SPELL_CAST)
  {
    return NULL;
  }
  if ((((((card == affected_card) && (player == affected_card_controller)) &&
         (reason_for_trigger_controller == player)) &&
        ((controlled_by == 2 || (trigger_cause_controller == controlled_by)))) &&
       (((unsigned int)event - (unsigned int)EVENT_TRIGGER < 2U &&
         ((result = target_me(t_player, t_card, trigger_cause_controller, trigger_cause),
           result != 0 &&
               (result = is_what(trigger_cause_controller, trigger_cause, TYPE_LAND | TYPE_EFFECT),
                result == 0)))))) &&
      (result = has_subtype(trigger_cause_controller, trigger_cause, targeting_subtype),
       result != 0))
  {
    if (event != EVENT_TRIGGER)
    {
      targeted_subtype_spell_result.player = trigger_cause_controller;
      targeted_subtype_spell_result.card = trigger_cause;
      return &targeted_subtype_spell_result;
    }
    if (trigger_mode == RESOLVE_TRIGGER_DUH)
    {
      result = duh_mode(player);
      trigger_mode = (result != 0) + RESOLVE_TRIGGER_OPTIONAL;
    }
    event_result = event_result | trigger_mode;
  }
  return NULL;
}

#endif
