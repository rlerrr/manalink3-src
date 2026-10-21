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
  etb_ability_2022(player, card, event, (result != 0) + RESOLVE_TRIGGER_OPTIONAL);
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
  result = generic_aura_2022(player, card, event, 1 - player, 0, 0, 0, 4, 0, 0, 0);
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
            result == 0 && (result = check_special_flags2_2022(affected_card_controller, affected_card,
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

/* Recovered October 2022 implementations; see docs/recovery-progress.md. */

/* ManalinkEh 2022: 0x228fe90; original cards/commander_2017.c:916. */
int card_taigam_sidisis_hand(int player, int card, event_t event)
{
  int t_card;
  int t_player;
  card_instance_t *instance;
  unsigned int must_select;
  int result;
  CardFunction p_Var3;
  int iStack_140;
  target_definition_t td;
  test_definition_t test;

  check_legend_rule(player, card, event);
  skip_your_draw_step(player, event);
  upkeep_trigger_ability(player, card, event, player);
  if (event == EVENT_UPKEEP_TRIGGER_ABILITY)
  {
    select_one_and_mill_the_rest(player, player, 3, 0xff);
    return 0;
  }
  if ((event == EVENT_CAN_ACTIVATE) || (event == EVENT_ACTIVATE))
  {
    default_target_definition(player, card, &td, 2);
    iStack_140 = generic_activated_ability(player, card, event, 4, 0, 1, 0, 0, 0, 0, 0, &td,
                                           "TARGET_CREATURE");
    if ((event == EVENT_CAN_ACTIVATE) && (iStack_140 != 0))
    {
      result = count_graveyard(player);
      if (result < 1)
      {
        return 0;
      }
      return iStack_140;
    }
  }
  else
  {
    if ((event != EVENT_RESOLVE_ACTIVATION) && (event != EVENT_CLEANUP))
    {
      return 0;
    }
    default_target_definition(player, card, &td, 2);
    iStack_140 = generic_activated_ability(player, card, event, 4, 0, 1, 0, 0, 0, 0, 0, &td,
                                           "TARGET_CREATURE");
  }
  if (event == EVENT_ACTIVATE)
  {
    if (spell_fizzled != 1)
    {
      new_default_test_definition(&test, 0xff, "SELECT_CARD");
      p_Var3 = (CardFunction)0x0;
      while (true)
      {
        if (ai_is_speculating == 1)
        {
          must_select = (unsigned int)(p_Var3 == (CardFunction)0x0);
        }
        else
        {
          must_select = 0;
          if ((player == 1) && (((unsigned char)trace_mode & 2) == 0))
          {
            must_select = (unsigned int)(p_Var3 == (CardFunction)0x0);
          }
        }
        result = new_global_tutor(player, player, 5, 6, must_select, 3, &test);
        if (result < 0)
          break;
        p_Var3 = p_Var3 + 1;
      }
      if (p_Var3 == (CardFunction)0x0)
      {
        spell_fizzled = 1;
      }
      else
      {
        instance = get_card_instance(player, card);
        instance->info_slot_fn = p_Var3;
      }
    }
  }
  else if (event == EVENT_RESOLVE_ACTIVATION)
  {
    instance = get_card_instance(player, card);
    result = instance->info_slot;
    instance = get_card_instance(player, card);
    t_card = instance->targets[0].card;
    instance = get_card_instance(player, card);
    t_player = instance->targets[0].player;
    instance = get_card_instance(player, card);
    pump_until_eot_2022(player, instance->parent_card, t_player, t_card, -result, -result);
    instance = get_card_instance(player, card);
    instance->info_slot = 0;
  }
  return iStack_140;
}

/* Recovered October 2022 implementations; see docs/recovery-progress.md. */
static int teferis_protection_legacy(int player, int card, event_t event);

/* ManalinkEh 2022: 0x228d4a0; original cards/commander_2017.c:115. */
static int teferis_protection_legacy(int player, int card, event_t event)
{
  card_instance_t *instance;
  int result;

  instance = in_play(player, card);
  if ((instance != NULL) &&
      (result = is_humiliated_for_ability(player, card, AL_STATIC), result == 0))
  {
    *(unsigned int *)(&player_bits + player * 4) =
        *(unsigned int *)(&player_bits + player * 4) | 0x4200000;
  }
  if (event == EVENT_PROT_MISC)
  {
    if ((affected_card_controller == player) && (affected_card == -1))
    {
      event_result = event_result | 1;
    }
  }
  else if ((event == EVENT_BEGIN_TURN) && (player == current_turn))
  {
    kill_card(player, card, KILL_EXILE);
  }
  return 0;
}

/* ManalinkEh 2022: 0x228da90; original cards/commander_2017.c:131. */
int card_teferis_protection(int player, int card, event_t event)
{
  int result;
  card_instance_t *instance;
  test_definition_t this_test;

  if (event == EVENT_RESOLVE_SPELL)
  {

    result = create_legacy_effect(player, card, teferis_protection_legacy);
    instance = get_card_instance(player, result);
    result = get_id(player, card);
    instance->targets[2].card = result;
    result = get_id(player, card);
    instance->targets[3].card = result;
    default_test_definition(&this_test, 0x147);
    new_manipulate_all(player, card, player, &this_test, ACT_PHASE_OUT);
    kill_card(player, card, KILL_EXILE);
  }
  result = basic_spell(player, card, event);
  return result;
}

/* ManalinkEh 2022: 0x228dcb0; original cards/commander_2017.c:185. */
int card_kindred_discovery(int player, int card, event_t event)
{
  unsigned int uVar1;
  int result;
  card_instance_t *instance;
  test_definition_t test;

  if (event == EVENT_RESOLVE_SPELL)
  {
    instance = get_card_instance(player, card);
    result = select_a_subtype(player, card);
    instance->info_slot = result;
  LAB_0228dcd3:
    dat_ability(player, card, event, DAT_RESOLVE_TRIGGER_MANDATORY, 0);
    if (trigger_condition == 0xdb)
      goto LAB_0228ddea;
  }
  else
  {
    if (event != EVENT_DECLARE_ATTACKERS)
      goto LAB_0228dcd3;
    result = is_humiliated_for_ability(player, card, AL_TRIG);
    if (result == 0)
    {

      instance = get_card_instance(player, card);
      uVar1 = instance->info_slot;
      default_test_definition(&test, 2);
      test.subtype = uVar1;
      store_attackers(player, card, EVENT_DECLARE_ATTACKERS,
                      DAT_DONT_REPORT_VALUE | DAT_SEPARATE_TRIGGERS, player, -1, &test);
    }
    dat_ability(player, card, EVENT_DECLARE_ATTACKERS, DAT_RESOLVE_TRIGGER_MANDATORY, 0);
    if (trigger_condition != 0xdb)
      goto LAB_0228dd1b;
  LAB_0228ddea:
    if ((card == affected_card) && (player == affected_card_controller))
    {

      instance = get_card_instance(player, card);
      uVar1 = instance->info_slot;
      default_test_definition(&test, 2);
      test.subtype = uVar1;
      etb_ability_global(player, event, 2, TYPE_NONE, &test, player, card);
    }
  }
  if ((event == EVENT_ETB) || (event == EVENT_DAT_ABILITY))
  {
    draw_a_card(player);
  }
LAB_0228dd1b:
  result = global_enchantment(player, card, event);
  return result;
}

/* ManalinkEh 2022: 0x228dfa0; original cards/commander_2017.c:286. */
int card_vindictive_lich(int player, int card, event_t event)
{
  int result;
  card_instance_t *instance;
  test_definition_t test;

  result = this_dies_trigger(player, card, event, RESOLVE_TRIGGER_MANDATORY);
  if (result != 0)
  {
    result = opponent_is_valid_target(player, card);
    if (result != 0)
    {

      result = dialog_impl(player, card, EVENT_CAST_SPELL, "\001\002", "\001\014", "\001\003",
                           "Sacrifice", 1, 1, "Discard 2", 1, 1, "Lose 5 life", 1, 1, 0, 0, 0, 0);
      if (result == 0)
      {
        spell_fizzled = 1;
      }
      else
      {
        target_opponent(player, card);
        instance = get_card_instance(player, card);
        if (result == 1)
        {
          new_default_test_definition(&test, 2, "Select a creature to sacrifice.");
          new_sacrifice(player, card, instance->targets[0].player, SAC_CAUSED | SAC_NO_CANCEL,
                        &test);
        }
        else if (result == 2)
        {
          new_multidiscard(instance->targets[0].player, 2, 0, player);
        }
        else if (result == 3)
        {
          lose_life_2022(instance->targets[0].player, 5);
        }
      }
    }
  }
  return 0;
}

/* ManalinkEh 2022: 0x22901f0; original cards/commander_2017.c:970. */
int card_the_ur_dragon(int player, int card, event_t event)
{
  int iVar1;
  card_instance_t *pcVar2;
  card_instance_t *pcVar3;
  int iVar4;
  test_definition_t test1;
  test_definition_t test2;

  check_legend_rule(player, card, event);
  if (((event == EVENT_MODIFY_COST_GLOBAL) && (affected_card_controller == player)) &&
      (affected_card != card))
  {
    iVar1 = is_what(player, affected_card, 2);
    if (iVar1 != 0)
    {
      iVar1 = has_subtype(affected_card_controller, affected_card, SUBTYPE_DRAGON);
      if (iVar1 != 0)
      {
        COST_COLORLESS = COST_COLORLESS + -1;
      }
    }
  }
  default_test_definition(&test1, 2);
  test1.subtype = 0x30;
  iVar1 = declare_attackers_trigger_test(player, card, event, DAT_TRACK, player, -1, &test1);
  if (iVar1 != 0)
  {

    pcVar2 = get_card_instance(player, card);
    iVar4 = iVar1 + -1;
    if (-1 < iVar4)
    {
      do
      {
        pcVar3 = in_play(player,
                         (unsigned int)*(unsigned char *)((int)&pcVar2->targets[2].player + iVar4));
        if (pcVar3 != NULL)
        {
          draw_cards(player, iVar1);
          new_default_test_definition(&test2, 0x47, "Select a permanent card.");
          new_global_tutor(player, player, 4, 4, 0, 2, &test2);
        }
        iVar4 = iVar4 + -1;
      } while (iVar4 != -1);
      return 0;
    }
  }
  return 0;
}

/* ManalinkEh 2022: 0x2290700; original cards/commander_2017.c:1096. */
int card_mirror_of_the_forebears(int player, int card, event_t event)
{
  int src_card;
  card_instance_t *pcVar1;
  int iVar2;
  int iVar3;
  card_instance_t *pcVar4;
  target_definition_t td;

  if (event != EVENT_RESOLVE_SPELL)
  {
    if ((((event == EVENT_CAN_ACTIVATE) || (event == EVENT_ACTIVATE)) ||
         (event == EVENT_RESOLVE_ACTIVATION)) ||
        (event == EVENT_CLEANUP))
    {
      default_target_definition(player, card, &td, 2);
      pcVar1 = get_card_instance(player, card);
      td.required_subtype = pcVar1->info_slot;
      if (event == EVENT_RESOLVE_ACTIVATION)
      {

        pcVar1 = get_card_instance(player, card);
        iVar2 = pcVar1->parent_controller;
        src_card = pcVar1->parent_card;
        iVar3 = valid_target(&td);
        if (iVar3 != 0)
        {
          pcVar4 = in_play(player, card);
          if (pcVar4 != NULL)
          {
            shapeshift_target(iVar2, src_card, iVar2, src_card, pcVar1->targets[0].player,
                              pcVar1->targets[0].card, 1);
          }
        }
      }
      iVar2 = generic_activated_ability(player, card, event, 4, 1, 0, 0, 0, 0, 0, 0, &td,
                                        "TARGET_CREATURE");
    }
    else
    {
      iVar2 = 0;
    }
    return iVar2;
  }
  pcVar1 = get_card_instance(player, card);
  iVar2 = select_a_subtype(player, card);
  pcVar1->info_slot = iVar2;
  return 0;
}

/* Recovered October 2022 implementations; see docs/recovery-progress.md. */

/* ManalinkEh 2022: 0x228fd70; original cards/commander_2017.c:861. */
int card_licia_sanguine_tribune(int player, int card, event_t event)
{
  int result;

  check_legend_rule(player, card, event);
  lifelink(player, card, event);
  if (event == EVENT_MODIFY_COST)
  {
    result = get_trap_condition(player, 1);
    COST_COLORLESS = COST_COLORLESS - (char)result;
    return 0;
  }
  if ((event == EVENT_CAN_ACTIVATE) || (event == EVENT_ACTIVATE))
  {
  LAB_0228fdf1:
    if (event != EVENT_RESOLVE_ACTIVATION)
      goto LAB_0228fdf6;
  }
  else if (event != EVENT_RESOLVE_ACTIVATION)
  {
    if (event != EVENT_CLEANUP)
    {
      return 0;
    }
    goto LAB_0228fdf1;
  }
  add_counters_from_source(player, card, player, card, COUNTER_P1_P1, 3);
LAB_0228fdf6:
  result = generic_activated_ability(player, card, event, 0x12000, 0, 0, 0, 0, 0, 0, 5, NULL, NULL);
  return result;
}

/* Recovered October 2022 implementations; see docs/recovery-progress.md. */

/* ManalinkEh 2022: 0x228f7b0; original cards/commander_2017.c:736. */
int card_inalla_archmage_ritualist(int player, int card, event_t event)
{
  card_instance_t *instance;
  int result;
  target_definition_t td1;

  /* DWARF locals share this frame offset; keep every typed view. */
  union
  {
    target_definition_t target;
    test_definition_t test;
    token_generation_t token;
    uint8_t bytes[216];
  } recovered_stack_td2;

  check_legend_rule(player, card, event);
  if ((((trigger_condition == 0xdb) && (card == affected_card)) &&
       (player == affected_card_controller)) &&
      (reason_for_trigger_controller == player))
  {
    default_test_definition(&recovered_stack_td2.test, 2);
    result = duh_mode(player);
    etb_ability_global(player, event, (result != 0) + 1, TYPE_NONE, &recovered_stack_td2.test,
                       player, card);
  }
  if (event == EVENT_ETB)
  {
    result = has_mana(player, 0, 1);
    if (result != 0)
    {
      result = charge_mana_while_resolving(player, card, EVENT_RESOLVE_TRIGGER, player,
                                           COLOR_COLORLESS, 1);
      if (result != 0)
      {
        copy_token_definition(player, card, &recovered_stack_td2.token, trigger_cause_controller,
                              trigger_cause);
        recovered_stack_td2.target.illegal_state =
            (int32_t)haste_and_remove_token_at_end_of_next_turn;
        generate_token(&recovered_stack_td2.token);
      }
    }
  }
  else if (((event == EVENT_CAN_ACTIVATE) || (event == EVENT_ACTIVATE)) ||
           ((event == EVENT_RESOLVE_ACTIVATION || (event == EVENT_CLEANUP))))
  {
    default_target_definition(player, card, &td1, 0x47);
    td1.illegal_state = 1;
    td1.required_subtype = 0xd6;
    td1.preferred_controller = player;
    td1.allowed_controller = player;
    td1.illegal_abilities = 0;
    default_target_definition(player, card, &recovered_stack_td2.target, 2);
    recovered_stack_td2.target.zone = 0x1000;
    instance = get_card_instance(player, card);
    if (event == EVENT_RESOLVE_ACTIVATION)
    {
      result = valid_target(&recovered_stack_td2.target);
      if (result != 0)
      {
        lose_life(instance->targets[0].player, 7);
      }
    }
    result = generic_activated_ability_with_tapsubtype_as_cost(
        player, card, event, GAA_CAN_TARGET, 0, 0, 0, 0, 0, 0, 0, &recovered_stack_td2.target,
        "TARGET_PLAYER", &td1, 5);
    return result;
  }
  return 0;
}

/* Recovered October 2022 implementations; see docs/recovery-progress.md. */

/* ManalinkEh 2022: 0x228f460; original cards/commander_2017.c:670. */
int card_edgar_markov(int player, int card, event_t event)
{

  /* DWARF locals share this frame offset; keep every typed view. */
  union
  {
    test_definition_t test;
    token_generation_t token;
    uint8_t bytes[216];
  } recovered_stack_test;

  check_legend_rule(player, card, event);
  haste(player, card, event);
  if (((trigger_condition == 0xd3) && (card == affected_card)) &&
      (player == affected_card_controller))
  {
    default_test_definition(&recovered_stack_test.test, 0x47);
    recovered_stack_test.test.subtype = 0xcd;
    sp_ability_trigger(player, event, 2, TYPE_NONE, &recovered_stack_test.test, player, card);
  }
  if (event != EVENT_SPA)
  {
    dat_ability(player, card, event, DAT_RESOLVE_TRIGGER_MANDATORY, 0);
    if (event != EVENT_DAT_ABILITY)
    {
      return 0;
    }
    default_test_definition(&recovered_stack_test.test, 0x47);
    recovered_stack_test.test.subtype = 0xcd;
    new_manipulate_all(player, card, player, &recovered_stack_test.test,
                       ACT_ADD_COUNTERS_BASE | ACT_KILL_BURY);
    return 0;
  }
  default_token_definition(player, card, 0x49a0, &recovered_stack_test.token);
  recovered_stack_test.test.message[0x1c] = '\x01';
  recovered_stack_test.test.message[0x1d] = '\0';
  recovered_stack_test.test.message[0x1e] = '\0';
  recovered_stack_test.test.message[0x1f] = '\0';
  recovered_stack_test.test.message[0x18] = '\x01';
  recovered_stack_test.test.message[0x19] = '\0';
  recovered_stack_test.test.message[0x1a] = '\0';
  recovered_stack_test.test.message[0x1b] = '\0';
  generate_token(&recovered_stack_test.token);
  dat_ability(player, card, EVENT_SPA, DAT_RESOLVE_TRIGGER_MANDATORY, 0);
  return 0;
}

/* ManalinkEh 2022: 0x22908a0; original cards/commander_2017.c:1127. */
int card_ramos_dragon_engine(int player, int card, event_t event)
{
  int result;

  check_legend_rule(player, card, event);
  sp_ability_trigger(player, event, 2, TYPE_ANY, NULL, player, card);
  if (event == EVENT_SPA)
  {
    result = count_colors(trigger_cause_controller, trigger_cause);
    add_counters_from_source(player, card, player, card, COUNTER_P1_P1, result);
  }
  else
  {
    if (event == EVENT_CAN_ACTIVATE)
    {
      result = generic_activated_ability(player, card, EVENT_CAN_ACTIVATE, 0x2000, 0, 0, 0, 0, 0, 0,
                                         0x1000500, NULL, NULL);
      return result;
    }
    if ((event == EVENT_ACTIVATE) &&
        (result = generic_activated_ability(player, card, EVENT_ACTIVATE, 0x2000, 0, 0, 0, 0, 0, 0,
                                            0x1000500, NULL, NULL),
         result != 0))
    {
      produce_mana_multi(player, 0, 2, 2, 2, 2, 2);
      tapped_for_mana_color = -2;
    }
  }
  return 0;
}

/* Recovered October 2022 implementations; see docs/recovery-progress.md. */

/* ManalinkEh 2022: 0x228eaa0; original cards/commander_2017.c:494. */
int card_kindred_charge(int player, int card, event_t event)
{
  int iVar1;
  subtype_t subtype;
  card_instance_t *instance;
  int iVar3;
  unsigned int uVar4;
  char *pcVar5;
  bool bVar6;
  unsigned char bVar7;
  token_generation_t token;
  char marked[151];

  bVar7 = 0;
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
    else if (event == EVENT_RESOLVE_SPELL)
    {

      pcVar5 = marked + 5;
      subtype = select_a_subtype(player, card);
      uVar4 = 0x92;
      bVar6 = ((unsigned int)pcVar5 & 2) != 0;
      if (bVar6)
      {
        marked[5] = '\0';
        marked[6] = '\0';
        uVar4 = 0x90;
        pcVar5 = marked + 7;
      }
      for (uVar4 = uVar4 >> 2; uVar4 != 0; uVar4 = uVar4 - 1)
      {
        pcVar5[0] = '\0';
        pcVar5[1] = '\0';
        pcVar5[2] = '\0';
        pcVar5[3] = '\0';
        pcVar5 = pcVar5 + (unsigned int)bVar7 * -8 + 4;
      }
      if (!bVar6)
      {
        pcVar5[0] = '\0';
        pcVar5[1] = '\0';
      }
      marked[4] = '\0';
      marked[0] = '\0';
      marked[1] = '\0';
      marked[2] = '\0';
      marked[3] = '\0';
      iVar1 = 0;
      if (0 < active_cards_count[player])
      {
        do
        {
          while (((instance = in_play(player, iVar1),
                   instance == NULL || (iVar3 = is_what(player, iVar1, 2), iVar3 == 0)) ||
                  (iVar3 = has_creature_type(player, iVar1, subtype), iVar3 == 0)))
          {
            iVar1 = iVar1 + 1;
            iVar3 = active_cards_count[player];
            if (iVar3 <= iVar1)
              goto LAB_0228ebf0;
          }
          marked[iVar1] = '\x01';
          iVar3 = active_cards_count[player];
          iVar1 = iVar1 + 1;
        } while (iVar1 < iVar3);
      LAB_0228ebf0:
        if (0 < iVar3)
        {
          iVar1 = 0;
          do
          {
            while (marked[iVar1] == '\0')
            {
              iVar1 = iVar1 + 1;
              if (active_cards_count[player] <= iVar1)
                goto LAB_0228ec58;
            }
            copy_token_definition(player, card, &token, player, iVar1);
            iVar1 = iVar1 + 1;
            token.special_code_on_generation = haste_and_remove_token_at_end_of_next_turn;
            generate_token(&token);
          } while (iVar1 < active_cards_count[player]);
        }
      }
    LAB_0228ec58:
      kill_card(player, card, KILL_DESTROY);
    }
    else if ((event != EVENT_CAN_CHANGE_TARGET) && (event != EVENT_CHANGE_TARGET))
    {
      return 0;
    }
  }
  iVar1 = basic_spell(player, card, event);
  return iVar1;
}
