#include "manalink.h"

/* Alms Collector	|3|W
 * Creature - Cat Cleric 3/4
 * Flash
 * If an opponent would draw two or more cards, instead you and that player each draw a card. */

int card_balan_wandering_knight(int player, int card, event_t event)
{
  /* Balan, Wandering Knight	|2|W|W
   * Legendary Creature - Cat Knight 3/3
   * First strike
   * ~ has double strike as long as two or more Equipment are attached to it.
   * |1|W: Attach all Equipment you control to ~. */

  check_legend_rule(player, card, event);

  if (event == EVENT_ABILITIES && affect_me(player, card) && in_play(player, card)
	  && equipments_attached_to_me(player, card, EATM_REPORT_TOTAL) >= 2)
	event_result |= KEYWORD_DOUBLE_STRIKE;

  int c;
  if (event == EVENT_RESOLVE_ACTIVATION)
	for (c = 0; c < active_cards_count[player]; ++c)
	  if (in_play(player, c) && is_what(player, c, TYPE_ARTIFACT) && has_subtype(player, c, SUBTYPE_EQUIPMENT))
		equip_target_creature(player, c, player, card);

  return generic_activated_ability(player, card, event, GAA_NONE, MANACOST_XW(1, 1), 0, NULL, NULL);
}

int card_curse_of_vitality(int player, int card, event_t event)
{
  /* Curse of Vitality	|2|W
   * Enchantment - Aura Curse
   * Enchant player
   * Whenever enchanted player is attacked, you gain 2 life. Each opponent attacking that player does the same. */

  card_instance_t* instance;
  int cursed;
  if ((instance = in_play(player, card)) && !is_humiliated(player, card)
	  && (cursed = instance->targets[4].player) >= 0
	  && declare_attackers_trigger(player, card, event, DAT_ATTACKS_PLAYER|RESOLVE_TRIGGER_AI(player), 1-cursed, -1))
	gain_life(player, 2);

  return curse(player, card, event);
}

int card_fortunate_few(int player, int card, event_t event)
{
  /* Fortunate Few	|3|W|W
   * Sorcery
   * Choose a nonland permanent you don't control, then each other player chooses a nonland permanent they don't control that hasn't been chosen this way. Destroy all other nonland permanents. */

  if (!IS_GS_EVENT(player, card, event))
	return 0;

  target_definition_t td;
  default_target_definition(player, card, &td, TYPE_PERMANENT);
  td.illegal_type = TYPE_LAND;

  if (event == EVENT_RESOLVE_SPELL)
	{
	  card_instance_t* inst = get_card_instance(player, card);
	  APNAP(p,
			inst->targets[p].player = inst->targets[p].card = -1;
			td.allowed_controller = 1-p;
			if (can_target(&td)
				&& new_pick_target(&td, "Select a nonland permanent you don't control.", p, GS_LITERAL_PROMPT))
			  get_card_instance(inst->targets[p].player, inst->targets[p].card)->state |= STATE_CANNOT_TARGET);
	  inst->number_of_targets = 2;

	  int p, c;
	  for (p = 0; p <= 1; ++p)
		if (inst->targets[p].player >= 0)
		  get_card_instance(inst->targets[p].player, inst->targets[p].card)->state &= ~STATE_CANNOT_TARGET;

	  for (p = 0; p <= 1; ++p)
		for (c = 0; c < active_cards_count[p]; ++c)
		  if (in_play(p, c)
			  && !(c == inst->targets[0].card && p == inst->targets[0].player)
			  && !(c == inst->targets[1].card && p == inst->targets[1].player)
			  && is_what(p, c, TYPE_PERMANENT)
			  && !is_what(p, c, TYPE_LAND))
			kill_card(p, c, KILL_DESTROY);

	  kill_card(player, card, KILL_DESTROY);
	}

  return basic_spell(player, card, event);
}

/* Kindred Boon	|2|W|W
 * Enchantment
 * As ~ enters the battlefield, choose a creature type.
 * |1|W: Put a divinity counter on target creature you control of the chosen type.
 * Each creature you control with a divinity counter on it has indestructible. */

/* Scalelord Reckoner	|3|W|W
 * Creature - Dragon 4/4
 * Flying
 * Whenever a Dragon you control becomes the target of a spell or ability an opponent controls, destroy target nonland permanent that player controls. */

/* Stalking Leonin	|2|W
 * Creature - Cat Archer 3/3
 * When ~ enters the battlefield, secretly choose an opponent.
 * Reveal the player you chose: Exile target creature that's attacking you if it's controlled by the chosen player. Activate this ability only once. */

/* Teferi's Protection	|2|W
 * Instant
 * Until your next turn, your life total can't change and you have protection from everything. All permanents you control phase out.
 * Exile ~. */

/* Curse of Verbosity	|2|U
 * Enchantment - Aura Curse
 * Enchant player
 * Whenever enchanted player is attacked, you draw a card. Each opponent attacking that player does the same. */

/* Galecaster Colossus	|5|U|U
 * Creature - Giant Wizard 5/6
 * Tap an untapped Wizard you control: Return target nonland permanent you don't control to its owner's hand. */

/* Kindred Discovery	|3|U|U
 * Enchantment
 * As ~ enters the battlefield, choose a creature type.
 * Whenever a creature you control of the chosen type enters the battlefield or attacks, draw a card. */

/* Magus of the Mind	|4|U|U
 * Creature - Human Wizard 4/5
 * |U, |T, Sacrifice ~: Shuffle your library, then exile the top X cards, where X is one plus the number of spells cast this turn. Until end of turn, you may play cards exiled this way without paying their mana costs. */

/* Portal Mage	|2|U
 * Creature - Human Wizard 2/2
 * Flash
 * When ~ enters the battlefield during the declare attackers step, you may reselect which player or planeswalker target attacking creature is attacking. */

/* Bloodline Necromancer	|4|B
 * Creature - Vampire Wizard 3/2
 * Lifelink
 * When ~ enters the battlefield, you may return target Vampire or Wizard creature card from your graveyard to the battlefield. */

/* Boneyard Scourge	|2|B|B
 * Creature - Zombie Dragon 4/3
 * Flying
 * Whenever a Dragon you control dies while ~ is in your graveyard, you may pay |1|B. If you do, return ~ from your graveyard to the battlefield. */

/* Curse of Disturbance	|2|B
 * Enchantment - Aura Curse
 * Enchant player
 * Whenever enchanted player is attacked, create a 2/2 |Sblack Zombie creature token. Each opponent attacking that player does the same. */

/* Kheru Mind-Eater	|2|B
 * Creature - Vampire 1/3
 * Menace
 * Whenever ~ deals combat damage to a player, that player exiles a card from their hand face down.
 * You may look at and play cards exiled with ~. */

/* Kindred Dominance	|5|B|B
 * Sorcery
 * Choose a creature type. Destroy all creatures that aren't of the chosen type. */

/* New Blood	|2|B|B
 * Sorcery
 * As an additional cost to cast this spell, tap an untapped Vampire you control.
 * Gain control of target creature. Change the text of that creature by replacing all instances of one creature type with Vampire. */

/* Patron of the Vein	|4|B|B
 * Creature - Vampire Shaman 4/4
 * Flying
 * When ~ enters the battlefield, destroy target creature an opponent controls.
 * Whenever a creature an opponent controls dies, exile it and put a +1/+1 counter on each Vampire you control. */

/* Vindictive Lich	|3|B
 * Creature - Zombie Wizard 4/1
 * When ~ dies, choose one or more. Each mode must target a different player.
 * * Target opponent sacrifices a creature.
 * * Target opponent discards two cards.
 * * Target opponent loses 5 life. */

/* Bloodsworn Steward	|2|R|R
 * Creature - Vampire Knight 4/4
 * Flying
 * Commander creatures you control get +2/+2 and have haste. */

/* Crimson Honor Guard	|3|R|R
 * Creature - Vampire Knight 4/5
 * Trample
 * At the beginning of each player's end step, ~ deals 4 damage to that player unless they control a commander. */

/* Curse of Opulence	|R
 * Enchantment - Aura Curse
 * Enchant player
 * Whenever enchanted player is attacked, create a colorless artifact token named Gold. It has "Sacrifice this artifact: Add one mana of any color." Each opponent attacking that player does the same. */

/* Disrupt Decorum	|2|R|R
 * Sorcery
 * Goad all creatures you don't control. */

/* Izzet Chemister	|2|R
 * Creature - Goblin Wizard 1/3
 * Haste
 * |R, |T: Exile target instant or sorcery card from your graveyard.
 * |1|R, |T, Sacrifice ~: Cast any number of cards exiled with ~ without paying their mana costs. */

/* Kindred Charge	|4|R|R
 * Sorcery
 * Choose a creature type. For each creature you control of the chosen type, create a token that's a copy of that creature. Those tokens gain haste. Exile them at the beginning of the next end step. */

/* Shifting Shadow	|2|R
 * Enchantment - Aura
 * Enchant creature
 * Enchanted creature has haste and "At the beginning of your upkeep, destroy this creature. Reveal cards from the top of your library until you reveal a creature card. Put that card onto the battlefield and attach ~ to it, then put all other cards revealed this way on the bottom of your library in a random order." */

/* Territorial Hellkite	|2|R|R
 * Creature - Dragon 6/5
 * Flying, haste
 * At the beginning of combat on your turn, choose an opponent at random that ~ didn't attack during your last combat. ~ attacks that player this combat if able. If you can't choose an opponent this way, tap ~. */

/* Curse of Bounty	|1|G
 * Enchantment - Aura Curse
 * Enchant player
 * Whenever enchanted player is attacked, untap all nonland permanents you control. Each opponent attacking that player untaps all nonland permanents they control. */

/* Hungry Lynx	|1|G
 * Creature - Cat 2/2
 * Cats you control have protection from Rats.
 * At the beginning of your end step, target opponent creates a 1/1 |Sblack Rat creature token with deathtouch.
 * Whenever a Rat dies, put a +1/+1 counter on each Cat you control. */

/* Kindred Summons	|5|G|G
 * Instant
 * Choose a creature type. Reveal cards from the top of your library until you reveal X creature cards of the chosen type, where X is the number of creatures you control of that type. Put those cards onto the battlefield, then shuffle the rest of the revealed cards into your library. */

/* Qasali Slingers	|4|G
 * Creature - Cat Warrior 3/5
 * Reach
 * Whenever ~ or another Cat enters the battlefield under your control, you may destroy target artifact or enchantment. */

/* Traverse the Outlands	|4|G
 * Sorcery
 * Search your library for up to X basic land cards, where X is the greatest power among creatures you control. Put those cards onto the battlefield tapped, then shuffle your library. */

/* Arahbo, Roar of the World	|3|G|W
 * Legendary Creature - Cat Avatar 5/5
 * Eminence - At the beginning of combat on your turn, if ~ is in the command zone or on the battlefield, another target Cat you control gets +3/+3 until end of turn.
 * Whenever another Cat you control attacks, you may pay |1|G|W. If you do, it gains trample and gets +X/+X until end of turn, where X is its power. */

/* Edgar Markov	|3|R|W|B
 * Legendary Creature - Vampire Knight 4/4
 * Eminence - Whenever you cast another Vampire spell, if ~ is in the command zone or on the battlefield, create a 1/1 |Sblack Vampire creature token.
 * First strike, haste
 * Whenever ~ attacks, put a +1/+1 counter on each Vampire you control. */

/* Fractured Identity	|3|W|U
 * Sorcery
 * Exile target nonland permanent. Each player other than its controller creates a token that's a copy of it. */

/* Inalla, Archmage Ritualist	|2|U|B|R
 * Legendary Creature - Human Wizard 4/5
 * Eminence - Whenever another nontoken Wizard enters the battlefield under your control, if ~ is in the command zone or on the battlefield, you may pay |1. If you do, create a token that's a copy of that Wizard. The token gains haste. Exile it at the beginning of the next end step.
 * Tap five untapped Wizards you control: Target player loses 7 life. */

/* Kess, Dissident Mage	|1|U|B|R
 * Legendary Creature - Human Wizard 3/4
 * Flying
 * During each of your turns, you may cast an instant or sorcery card from your graveyard. If a card cast this way would be put into your graveyard, exile it instead. */

/* Licia, Sanguine Tribune	|5|R|W|B
 * Legendary Creature - Vampire Soldier 4/4
 * This spell costs |1 less to cast for each 1 life you gained this turn.
 * First strike, lifelink
 * Pay 5 life: Put three +1/+1 counters on ~. Activate this ability only during your turn and only once each turn. */

/* Mairsil, the Pretender	|1|U|B|R
 * Legendary Creature - Human Wizard 4/4
 * When ~ enters the battlefield, you may exile an artifact or creature card from your hand or graveyard and put a cage counter on it.
 * ~ has all activated abilities of all cards you own in exile with cage counters on them. You may activate each of those abilities only once each turn. */

/* Mathas, Fiend Seeker	|R|W|B
 * Legendary Creature - Vampire 3/3
 * Menace
 * At the beginning of your end step, put a bounty counter on target creature an opponent controls. For as long as that creature has a bounty counter on it, it has "When this creature dies, each opponent draws a card and gains 2 life." */

/* Mirri, Weatherlight Duelist	|1|G|W
 * Legendary Creature - Cat Warrior 3/2
 * First strike
 * Whenever ~ attacks, each opponent can't block with more than one creature this combat.
 * As long as ~ is tapped, no more than one creature can attack you each combat. */

/* Nazahn, Revered Bladesmith	|4|G|W
 * Legendary Creature - Cat Artificer 5/4
 * When ~ enters the battlefield, search your library for an Equipment card and reveal it. If you reveal a card named Hammer of ~ this way, put it onto the battlefield. Otherwise, put that card into your hand. Then shuffle your library.
 * Whenever an equipped creature you control attacks, you may tap target creature defending player controls. */

/* O-Kagachi, Vengeful Kami	|1|W|U|B|R|G
 * Legendary Creature - Dragon Spirit 6/6
 * Flying, trample
 * Whenever ~ deals combat damage to a player, if that player attacked you during their last turn, exile target nonland permanent that player controls. */

/* Taigam, Ojutai Master	|2|W|U
 * Legendary Creature - Human Monk 3/4
 * Instant, sorcery, and Dragon spells you control can't be countered.
 * Whenever you cast an instant or sorcery spell from your hand, if ~ attacked this turn, that spell gains rebound. */

/* Taigam, Sidisi's Hand	|3|U|B
 * Legendary Creature - Human Wizard 3/4
 * Skip your draw step.
 * At the beginning of your upkeep, look at the top three cards of your library. Put one of them into your hand and the rest into your graveyard.
 * |B, |T, Exile X cards from your graveyard: Target creature gets -X/-X until end of turn. */

/* The Ur-Dragon	|4|W|U|B|R|G
 * Legendary Creature - Dragon Avatar 10/10
 * Eminence - As long as ~ is in the command zone or on the battlefield, other Dragon spells you cast cost |1 less to cast.
 * Flying
 * Whenever one or more Dragons you control attack, draw that many cards, then you may put a permanent card from your hand onto the battlefield. */

/* Wasitora, Nekoru Queen	|2|B|R|G
 * Legendary Creature - Cat Dragon 5/4
 * Flying, trample
 * Whenever ~ deals combat damage to a player, that player sacrifices a creature. If the player can't, you create a 3/3 |Sblack, |Sred, and |Sgreen Cat Dragon creature token with flying. */

/* Bloodforged Battle-Axe	|1
 * Artifact - Equipment
 * Equipped creature gets +2/+0.
 * Whenever equipped creature deals combat damage to a player, create a token that's a copy of ~.
 * Equip |2 */

/* Hammer of Nazahn	|4
 * Legendary Artifact - Equipment
 * Whenever ~ or another Equipment enters the battlefield under your control, you may attach that Equipment to target creature you control.
 * Equipped creature gets +2/+0 and has indestructible.
 * Equip |4 */

/* Heirloom Blade	|3
 * Artifact - Equipment
 * Equipped creature gets +3/+1.
 * Whenever equipped creature dies, you may reveal cards from the top of your library until you reveal a creature card that shares a creature type with it. Put that card into your hand and the rest on the bottom of your library in a random order.
 * Equip |1 */

/* Herald's Horn	|3
 * Artifact
 * As ~ enters the battlefield, choose a creature type.
 * Creature spells you cast of the chosen type cost |1 less to cast.
 * At the beginning of your upkeep, look at the top card of your library. If it's a creature card of the chosen type, you may reveal it and put it into your hand. */

/* Mirror of the Forebears	|2
 * Artifact
 * As ~ enters the battlefield, choose a creature type.
 * |1: Until end of turn, ~ becomes a copy of target creature you control of the chosen type, except it's an artifact in addition to its other types. */

/* Ramos, Dragon Engine	|6
 * Legendary Artifact Creature - Dragon 4/4
 * Flying
 * Whenever you cast a spell, put a +1/+1 counter on ~ for each of that spell's colors.
 * Remove five +1/+1 counters from ~: Add |W|W|U|U|B|B|R|R|G|G. Activate this ability only once each turn. */

/* Path of Ancestry	""
 * Land
 * ~ enters the battlefield tapped.
 * |T: Add one mana of any color in your commander's color identity. When that mana is spent to cast a creature spell that shares a creature type with your commander, scry 1. */

/* Recovered October 2022 implementations; see docs/recovery-progress.md. */

/* ManalinkEh 2022: 0x228db60; original cards/commander_2017.c:159. */
int card_galecaster_colossus(int player, int card, event_t event)
{
  card_instance_t *instance;
  int result;
  target_definition_t td;
  target_definition_t td_subtype;
  if (event != EVENT_ACTIVATE)
  {
    if ((1 < event - EVENT_RESOLVE_ACTIVATION) && (event != EVENT_CLEANUP))
    {
      return 0;
    }
    if (event == EVENT_RESOLVE_ACTIVATION)
    {
      instance = get_card_instance(player, card);
      result = instance->targets[0].card;
      instance = get_card_instance(player, card);
      bounce_permanent(instance->targets[0].player, result);
    }
  }
  default_target_definition(player, card, &td, 0x47);
  td.allowed_controller = 1 - player;
  td.illegal_type = 1;
  default_target_definition(player, card, &td_subtype, 0x47);
  td_subtype.allowed_controller = player;
  td_subtype.preferred_controller = player;
  td_subtype.illegal_state = 1;
  td_subtype.illegal_abilities = 0;
  td_subtype.required_subtype = 0xd6;
  result = generic_activated_ability_with_tapsubtype_as_cost(
      player, card, event, GAA_CAN_TARGET | GAA_LITERAL_PROMPT, 0, 0, 0, 0, 0, 0, 0, &td,
      "Select target nonland permanent you don\'t control", &td_subtype, 1);
  return result;
}

/* ManalinkEh 2022: 0x228deb0; original cards/commander_2017.c:230. */
int card_bloodline_necromancer(int player, int card, event_t event)
{
  int result;
  test_definition_t test;
  lifelink(player, card, event);
  result = duh_mode(player);
  etb_ability(player, card, event, (result != 0) + RESOLVE_TRIGGER_OPTIONAL);
  if (event == EVENT_ETB)
  {
    new_default_test_definition(&test, 2, "Select target Vampire or Wizard creature card.");
    test.subtype = 0xcd;
    test.sub2 = 0xd6;
    test.subtype_flag = 3;
    result = new_special_count_grave(player, &test);
    if (result != 0)
    {
      result = graveyard_has_shroud(player);
      if (result == 0)
      {
        new_global_tutor(player, player, 2, 4, 0, 2, &test);
      }
    }
  }
  return 0;
}

/* ManalinkEh 2022: 0x228ec90; original cards/commander_2017.c:523. */
int card_shifting_shadow(int player, int card, event_t event)
{
  int result;
  card_instance_t *instance;
  test_definition_t test;
  if ((current_turn == player) && (result = upkeep_trigger(player, card, event), result != 0))
  {
    default_test_definition(&test, 2);
    instance = get_card_instance(player, card);
    kill_card((int)instance->damage_target_player, instance->damage_target_card, KILL_DESTROY);
    result = reveal_card_until_you_reveal_test(player, player, &test, 0x244);
    if (result < 0)
    {
      kill_card(player, card, KILL_DESTROY);
    }
    else
    {
      instance->targets[0].player = player;
      instance->targets[0].card = result;
      call_card_function(player, card, EVENT_RESOLVE_MOVING_AURA);
    }
  }
  if (event != EVENT_CAN_CAST)
  {
    if (event == EVENT_CAST_SPELL)
    {
      if ((card == affected_card) && (player == affected_card_controller))
        goto LAB_0228ecd0;
    }
    else if ((event == EVENT_RESOLVE_SPELL) ||
             ((event == EVENT_CAN_CHANGE_TARGET || (event == EVENT_CHANGE_TARGET))))
      goto LAB_0228ecd0;
    if ((((event & ~EVENT_SET_LEGACY_EFFECT_NAME) != EVENT_CAN_MOVE_AURA) &&
         (((2 < event - EVENT_POWER && (event != EVENT_RESOLVE_MOVING_AURA)) &&
           (event != EVENT_CHANGE_TYPE)))) &&
        (((event != EVENT_SET_COLOR && (event != EVENT_GET_SELECTED_CARD)) &&
          (event != EVENT_STATIC_EFFECTS))))
    {
      return 0;
    }
  }
LAB_0228ecd0:
  result = generic_aura(player, card, event, 1 - player, 0, 0, 0, 4, 0, 0, 0);
  return result;
}

/* ManalinkEh 2022: 0x228ee90; original cards/commander_2017.c:570. */
int card_qasali_slingers(int player, int card, event_t event)
{
  int result;
  card_instance_t *instance;
  target_definition_t td;
  test_definition_t test;
  default_test_definition(&test, 2);
  test.subtype = 0x1f;
  result = duh_mode(player);
  etb_ability_global(player, event, (result != 0) + 1, TYPE_CREATURE, &test, player, card);
  if (event == EVENT_ETB)
  {
    default_target_definition(player, card, &td, 0x44);
    instance = get_card_instance(player, card);
    result = can_target(&td);
    if (result != 0)
    {
      result = pick_target(&td, "DISENCHANT");
      if (result != 0)
      {
        kill_card(instance->targets[0].player, instance->targets[0].card, KILL_DESTROY);
      }
    }
    instance->number_of_targets = '\0';
  }
  result = global_enchantment(player, card, event);
  return result;
}

/* ManalinkEh 2022: 0x228efb0; original cards/commander_2017.c:597. */
int card_traverse_the_outlands(int player, int card, event_t event)
{
  int result;
  test_definition_t this_test;
  if (event == EVENT_RESOLVE_SPELL)
  {
    result = get_highest_power(player);
    if (0 < result)
    {
      default_test_definition(&this_test, 1);
      this_test.subtype = 0x2000;
      this_test.qty = (unsigned short)result;
      new_global_tutor(player, player, 1, 5, 0, 5, &this_test);
    }
    kill_card(player, card, KILL_DESTROY);
  }
  result = basic_spell(player, card, event);
  return result;
}

/* ManalinkEh 2022: 0x228f600; original cards/commander_2017.c:706. */
int card_fractured_identity(int player, int card, event_t event)
{
  card_instance_t *instance;
  int result;
  token_generation_t token;
  target_definition_t td;

  if (event != EVENT_CAN_CAST)
  {
    if (event == EVENT_CAST_SPELL)
    {
      if (card != affected_card)
      {
        return 0;
      }
      if (player != affected_card_controller)
      {
        return 0;
      }
    }
    else
    {
      if ((event == EVENT_RESOLVE_SPELL) || (event == EVENT_CAN_CHANGE_TARGET))
      {
        default_target_definition(player, card, &td, 0x47);
        td.illegal_type = 1;
        instance = get_card_instance(player, card);
        if (event == EVENT_RESOLVE_SPELL)
        {
          result = valid_target(&td);
          if (result != 0)
          {
            copy_token_definition(player, card, &token, instance->targets[0].player,
                                  instance->targets[0].card);
            result = instance->targets[0].player;
            token.t_player = 1 - result;
            kill_card(result, instance->targets[0].card, KILL_EXILE);
            generate_token(&token);
          }
          kill_card(player, card, KILL_DESTROY);
        }
        goto LAB_0228f677;
      }
      if (event != EVENT_CHANGE_TARGET)
      {
        return 0;
      }
    }
  }
  default_target_definition(player, card, &td, 0x47);
  td.illegal_type = 1;
  get_card_instance(player, card);
LAB_0228f677:
  result = generic_spell(player, card, event, 1, &td, "TARGET_NONLAND_PERMANENT", 1, NULL);
  return result;
}

/* ManalinkEh 2022: 0x22903a0; original cards/commander_2017.c:1009. */
int card_wasitora_nekoru_queen(int player, int card, event_t event)
{
  int result;
  card_instance_t *instance;
  unsigned int uVar3;
  int times_damaged[2];
  token_generation_t token;
  test_definition_t test;
  check_legend_rule(player, card, event);
  result = damage_dealt_by_me(player, card, event,
                              DDBM_TRACE_DAMAGED_PLAYERS | DDBM_MUST_DAMAGE_PLAYER |
                                  DDBM_MUST_BE_COMBAT_DAMAGE);
  if (result != 0)
  {
    new_default_test_definition(&test, 2, "Select a creature to sacrifice.");
    instance = get_card_instance(player, card);
    times_damaged[0] = (int)(unsigned char)instance->targets[1].player;
    instance = get_card_instance(player, card);
    times_damaged[1] = (int)*(unsigned char *)((int)&instance->targets[1].player + 1);
    instance = get_card_instance(player, card);
    uVar3 = 0;
    instance->targets[1].player = 0;
    do
    {
      if ((times_damaged[current_turn ^ uVar3] != 0) &&
          (result = new_sacrifice(player, card, current_turn ^ uVar3,
                                  SAC_RETURN_CHOICE | SAC_CAUSED | SAC_NO_CANCEL, &test),
           result == 0))
      {
        default_token_definition(player, card, 0x4911, &token);
        generate_token(&token);
      }
      uVar3 = uVar3 + 1;
    } while (uVar3 != 2);
  }
  return 0;
}

/* ManalinkEh 2022: 0x22904e0; original cards/commander_2017.c:1040. */
int card_bloodforged_battle_axe(int player, int card, event_t event)
{
  int result;
  result = equipped_creature_deals_damage(player, card, event,
                                          DDBM_MUST_DAMAGE_PLAYER | DDBM_MUST_BE_COMBAT_DAMAGE);
  if (result != 0)
  {
    copy_token(player, card, player, card);
  }
  result = vanilla_equipment(player, card, event, 2, 2, 0, 0, 0);
  return result;
}

/* ManalinkEh 2022: 0x2290570; original cards/commander_2017.c:1053. */
int card_hammer_of_nazahn(int player, int card, event_t event)
{
  int result;
  card_instance_t *instance;
  target_definition_t td;
  check_legend_rule(player, card, event);
  if ((trigger_condition == 0xdb) && (reason_for_trigger_controller == player))
  {
    result = is_equipment(trigger_cause_controller, trigger_cause);
    if (result != 0)
    {
      etb_ability_global(player, event, 2, TYPE_PERMANENT, NULL, player, card);
    }
  }
  if (event == EVENT_ETB)
  {
    default_target_definition(trigger_cause_controller, trigger_cause, &td, 2);
    td.preferred_controller = player;
    td.allowed_controller = player;
    result = can_target(&td);
    if (result != 0)
    {
      result = new_pick_target(&td, "Attaching equipment: select target creature.", 0, 0x80);
      if (result != 0)
      {
        instance = get_card_instance(trigger_cause_controller, trigger_cause);
        equip_target_creature(trigger_cause_controller, trigger_cause, instance->targets[0].player,
                              instance->targets[0].card);
        instance->number_of_targets = '\0';
      }
    }
  }
  result = vanilla_equipment(player, card, event, 4, 2, 0, 0, 0x800);
  return result;
}

/* ManalinkEh 2022: 0x228d2e0; original cards/commander_2017.c:1158. */
static int path_of_ancestry_legacy(int player, int card, event_t event)
{
  card_instance_t *instance;
  int result;

  instance = get_card_instance(player, card);
  if (event == EVENT_CAST_SPELL)
  {
    if (((((card != affected_card) || (affected_card_controller != player)) &&
          (result = played_for_free(affected_card_controller, affected_card), result == 0)) &&
         (((result = is_token(affected_card_controller, affected_card),
            result == 0 && (result = check_special_flags2(affected_card_controller, affected_card,
                                                          -0x80000000),
                            result == 0)) &&
           ((affected_card_controller == player &&
             ((result = is_what(player, affected_card, 2),
               result != 0 && (0 < mana_paid[instance->targets[0].player])))))))) &&
        ((result = shares_creature_subtype(-1, instance->targets[2].player,
                                           affected_card_controller, affected_card),
          result != 0 || ((result = instance->targets[2].card,
                           -1 < result && (result = shares_creature_subtype(
                                               -1, result, affected_card_controller, affected_card),
                                           result != 0))))))
    {
      scry(player, 1);
      kill_card(player, card, KILL_EXILE);
    }
  }
  else if (((event == EVENT_STATIC_EFFECTS) && (stack_size == 0)) &&
           (mana_pool[player][instance->targets[0].player] < 1))
  {
    kill_card(player, card, KILL_EXILE);
  }
  return 0;
}
