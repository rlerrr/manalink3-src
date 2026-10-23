#include "manalink.h"

/* Aminatou's Augury	|6|U|U
 * Sorcery
 * Exile the top eight cards of your library. You may put a land card from among them onto the battlefield. Until end of turn, for each nonland card type, you may cast a card of that type from among the exiled cards without paying its mana cost. */

/* Aminatou, the Fateshifter	|W|U|B
 * Legendary Planeswalker - Aminatou (3)
 * +1: Draw a card, then put a card from your hand on top of your library.
 * -1: Exile another target permanent you own, then return it to the battlefield under your control.
 * -6: Choose left or right. Each player gains control of all nonland permanents other than ~ controlled by the next player in the chosen direction.
 * ~ can be your commander. */

/* Ancient Stone Idol	|10
 * Artifact Creature - Golem 12/12
 * Flash
 * This spell costs |1 less to cast for each attacking creature.
 * Trample
 * When ~ dies, create a 6/12 colorless Construct artifact creature token with trample. */

/* Arixmethes, Slumbering Isle	|2|G|U
 * Legendary Creature - Kraken 12/12
 * ~ enters the battlefield tapped with five slumber counters on it.
 * As long as ~ has a slumber counter on it, it's a land. (It's not a creature.)
 * Whenever you cast a spell, you may remove a slumber counter from ~.
 * |T: Add |G|U. */

/* Bloodtracker	|3|B
 * Creature - Vampire Wizard 2/2
 * Flying
 * |B, Pay 2 life: Put a +1/+1 counter on ~.
 * When ~ leaves the battlefield, draw a card for each +1/+1 counter on it. */

/* Boreas Charger	|2|W
 * Creature - Pegasus 2/1
 * Flying
 * When ~ leaves the battlefield, choose an opponent who controls more lands than you. Search your library for a number of |H1Plains cards equal to the difference and reveal them. Put one of them onto the battlefield tapped and the rest into your hand. Then shuffle your library. */

/* Brudiclad, Telchor Engineer	|4|U|R
 * Legendary Artifact Creature - Artificer 4/4
 * Creature tokens you control have haste.
 * At the beginning of combat on your turn, create a 2/1 |Sblue Myr artifact creature token. Then you may choose a token you control. If you do, each other token you control becomes a copy of that token. */

/* Coveted Jewel	|6
 * Artifact
 * When ~ enters the battlefield, draw three cards.
 * |T: Add three mana of any one color.
 * Whenever one or more creatures an opponent controls attack you and aren't blocked, that player draws three cards and gains control of ~. Untap it. */

/* Crash of Rhino Beetles	|4|G
 * Creature - Insect 5/5
 * Trample
 * ~ gets +10/+10 as long as you control ten or more lands. */

/* Echo Storm	|3|U|U
 * Sorcery
 * When you cast this spell, copy it for each time you've cast your commander from the command zone this game. You may choose new targets for the copies.
 * Create a token that's a copy of target artifact. */

/* Emissary of Grudges	|5|R
 * Creature - Efreet 6/5
 * Flying, haste
 * As ~ enters the battlefield, secretly choose an opponent.
 * Reveal the player you chose: Choose new targets for target spell or ability if it's controlled by the chosen player and if it targets you or a permanent you control. Activate this ability only once. */

/* Empyrial Storm	|4|W|W
 * Sorcery
 * When you cast this spell, copy it for each time you've cast your commander from the command zone this game.
 * Create a 4/4 |Swhite Angel creature token with flying. */

/* Enchanter's Bane	|1|R
 * Enchantment
 * At the beginning of your end step, target enchantment deals damage equal to its converted mana cost to its controller unless that player sacrifices it. */

/* Endless Atlas	|2
 * Artifact
 * |2, |T: Draw a card. Activate this ability only if you control three or more lands with the same name. */

/* Entreat the Dead	|X|X|B|B|B
 * Sorcery
 * Return X target creature cards from your graveyard to the battlefield.
 * Miracle |X|B|B (You may cast this card for its miracle cost when you draw it if it's the first card you drew this turn.) */

/* Estrid's Invocation	|2|U
 * Enchantment
 * You may have ~ enter the battlefield as a copy of any enchantment you control, except it has "At the beginning of your upkeep, you may exile this enchantment. If you do, return it to the battlefield under its owner's control." */

/* Estrid, the Masked	|1|G|W|U
 * Legendary Planeswalker - Estrid (3)
 * +2: Untap each enchanted permanent you control.
 * -1: Create a |Swhite Aura enchantment token named Mask attached to another target permanent. The token has enchant permanent and totem armor.
 * -7: Put the top seven cards of your library into your graveyard. Return all non-Aura enchantment cards from your graveyard to the battlefield, then do the same for Aura cards.
 * ~ can be your commander. */

/* Ever-Watching Threshold	|2|U
 * Enchantment
 * Whenever an opponent attacks you and/or a planeswalker you control with one or more creatures, draw a card. */

/* Forge of Heroes	""
 * Land
 * |T: Add |C.
 * |T: Choose target commander that entered the battlefield this turn. Put a +1/+1 counter on it if it's a creature and a loyalty counter on it if it's a planeswalker. */

/* Fury Storm	|2|R|R
 * Instant
 * When you cast this spell, copy it for each time you've cast your commander from the command zone this game. You may choose new targets for the copies.
 * Copy target instant or sorcery spell. You may choose new targets for the copy. */

/* Genesis Storm	|4|G|G
 * Sorcery
 * When you cast this spell, copy it for each time you've cast your commander from the command zone this game.
 * Reveal cards from the top of your library until you reveal a nonland permanent card. You may put that card onto the battlefield. Then put all cards revealed this way that weren't put onto the battlefield on the bottom of your library in a random order. */

/* Geode Golem	|5
 * Artifact Creature - Golem 5/3
 * Trample
 * Whenever ~ deals combat damage to a player, you may cast your commander from the command zone without paying its mana cost. (You still pay any additional costs.) */

/* Gyrus, Waker of Corpses	|X|B|R|G
 * Legendary Creature - Hydra 0/0
 * ~ enters the battlefield with a number of +1/+1 counters on it equal to the amount of mana spent to cast it.
 * Whenever ~ attacks, you may exile target creature card with lesser power from your graveyard. If you do, create a token that's a copy of that card and that's tapped and attacking. Exile the token at end of combat. */

/* Heavenly Blademaster	|5|W
 * Creature - Angel 3/6
 * Flying, double strike
 * When ~ enters the battlefield, you may attach any number of Auras and Equipment you control to it.
 * Other creatures you control get +1/+1 for each Aura and Equipment attached to ~. */

/* Isolated Watchtower	""
 * Land
 * |T: Add |C.
 * |2, |T: Scry 1, then you may reveal the top card of your library. If a basic land card is revealed this way, put it onto the battlefield tapped. Activate this ability only if an opponent controls at least two more lands than you. */

/* Kestia, the Cultivator	|1|G|W|U
 * Legendary Enchantment Creature - Nymph 4/4
 * Bestow |3|G|W|U (If you cast this card for its bestow cost, it's an Aura spell with enchant creature. It becomes a creature again if it's not attached to a creature.)
 * Enchanted creature gets +4/+4.
 * Whenever an enchanted creature or enchantment creature you control attacks, draw a card. */

/* Lord Windgrace	|2|B|R|G
 * Legendary Planeswalker - Windgrace (5)
 * +2: Discard a card, then draw a card. If a land card is discarded this way, draw an additional card.
 * -3: Return up to two target land cards from your graveyard to the battlefield.
 * -11: Destroy up to six target nonland permanents, then create six 2/2 |Sgreen Cat Warrior creature tokens with |H2forestwalk.
 * ~ can be your commander. */

/* Loyal Apprentice	|1|R
 * Creature - Human Artificer 2/1
 * Haste
 * Lieutenant - At the beginning of combat on your turn, if you control your commander, create a 1/1 colorless Thopter artifact creature token with flying. That token gains haste until end of turn. */

/* Loyal Drake	|2|U
 * Creature - Drake 2/2
 * Flying
 * Lieutenant - At the beginning of combat on your turn, if you control your commander, draw a card. */

/* Loyal Guardian	|4|G
 * Creature - Rhino 4/4
 * Trample
 * Lieutenant - At the beginning of combat on your turn, if you control your commander, put a +1/+1 counter on each creature you control. */

/* Loyal Subordinate	|2|B
 * Creature - Zombie 3/1
 * Menace
 * Lieutenant - At the beginning of combat on your turn, if you control your commander, each opponent loses 3 life. */

/* Loyal Unicorn	|3|W
 * Creature - Unicorn 3/4
 * Vigilance
 * Lieutenant - At the beginning of combat on your turn, if you control your commander, prevent all combat damage that would be dealt to creatures you control this turn. Other creatures you control gain vigilance until end of turn. */

/* Magus of the Balance	|1|W
 * Creature - Human Wizard 2/2
 * |4|W, |T, Sacrifice ~: Each player chooses a number of lands they control equal to the number of lands controlled by the player who controls the fewest, then sacrifices the rest. Players discard cards and sacrifice creatures the same way. */

/* Myth Unbound	|2|G
 * Enchantment
 * Your commander costs |1 less to cast for each time it's been cast from the command zone this game.
 * Whenever your commander is put into the command zone from anywhere, draw a card. */

/* Nesting Dragon	|3|R|R
 * Creature - Dragon 5/4
 * Flying
 * Landfall - Whenever a land enters the battlefield under your control, create a 0/2 |Sred Dragon Egg creature token with defender and "When this creature dies, create a 2/2 |Sred Dragon creature token with flying and '|R: This creature gets +1/+0 until end of turn.'" */

/* Night Incarnate	|4|B
 * Creature - Elemental 3/4
 * Deathtouch
 * When ~ leaves the battlefield, all creatures get -3/-3 until end of turn.
 * Evoke |3|B (You may cast this spell for its evoke cost. If you do, it's sacrificed when it enters the battlefield.) */

/* Nylea's Colossus	|6|G
 * Enchantment Creature - Giant 6/6
 * Constellation - Whenever ~ or another enchantment enters the battlefield under your control, double target creature's power and toughness until end of turn. */

/* Octopus Umbra	|3|U|U
 * Enchantment - Aura
 * Enchant creature
 * Enchanted creature has base power and toughness 8/8 and has "Whenever this creature attacks, you may tap target creature with power 8 or less."
 * Totem armor (If enchanted creature would be destroyed, instead remove all damage from it and destroy this Aura.) */

/* Primordial Mist	|4|U
 * Enchantment
 * At the beginning of your end step, you may manifest the top card of your library. (Put it onto the battlefield face down as a 2/2 creature. Turn it face up any time for its mana cost if it's a creature card.)
 * Exile a face-down permanent you control face up: You may play that card this turn. (You still pay its costs. Timing rules still apply.) */

/* Ravenous Slime	|2|G
 * Creature - Ooze 1/1
 * ~ can't be blocked by creatures with power 2 or less.
 * If a creature an opponent controls would die, instead exile it and put a number of +1/+1 counters equal to that creature's power on ~. */

/* Reality Scramble	|2|R|R
 * Sorcery
 * Put target permanent you own on the bottom of your library. Reveal cards from the top of your library until you reveal a card that shares a card type with that permanent. Put that card onto the battlefield and the rest on the bottom of your library in a random order.
 * Retrace (You may cast this card from your graveyard by discarding a land card in addition to paying its other costs.) */

/* Retrofitter Foundry	|1
 * Artifact
 * |3: Untap ~.
 * |2, |T: Create a 1/1 colorless Servo artifact creature token.
 * |1, |T, Sacrifice a Servo: Create a 1/1 colorless Thopter artifact creature token with flying.
 * |T, Sacrifice a Thopter: Create a 4/4 colorless Construct artifact creature token. */

/* Saheeli's Directive	|X|R|R|R
 * Sorcery
 * Improvise (Each artifact you tap after you're done activating mana abilities pays for |1.)
 * Reveal the top X cards of your library. You may put any number of artifact cards with converted mana cost X or less from among them onto the battlefield. Then put all cards revealed this way that weren't put onto the battlefield into your graveyard. */

/* Saheeli, the Gifted	|2|U|R
 * Legendary Planeswalker - Saheeli (4)
 * +1: Create a 1/1 colorless Servo artifact creature token.
 * +1: The next spell you cast this turn costs |1 less to cast for each artifact you control as you cast it.
 * -7: For each artifact you control, create a token that's a copy of it. Those tokens gain haste. Exile those tokens at the beginning of the next end step.
 * ~ can be your commander. */

/* Skull Storm	|7|B|B
 * Sorcery
 * When you cast this spell, copy it for each time you've cast your commander from the command zone this game.
 * Each opponent sacrifices a creature. Each opponent who can't loses half their life, rounded up. */

/* Sower of Discord	|4|B|B
 * Creature - Demon 6/6
 * Flying
 * As ~ enters the battlefield, choose two players.
 * Whenever damage is dealt to one of the chosen players, the other chosen player also loses that much life. */

/* Tawnos, Urza's Apprentice	|U|R
 * Legendary Creature - Human Artificer 1/3
 * Haste
 * |U|R, |T: Copy target activated or triggered ability you control from an artifact source. You may choose new targets for the copy. (Mana abilities can't be targeted.) */

/* Thantis, the Warweaver	|3|B|R|G
 * Legendary Creature - Spider 5/5
 * Vigilance, reach
 * All creatures attack each combat if able.
 * Whenever a creature attacks you or a planeswalker you control, put a +1/+1 counter on ~. */

/* Treasure Nabber	|2|R
 * Creature - Goblin Rogue 3/2
 * Whenever an opponent taps an artifact for mana, gain control of that artifact until the end of your next turn. */

/* Turntimber Sower	|2|G
 * Creature - Elf Druid 3/3
 * Whenever one or more land cards are put into your graveyard from anywhere, create a 0/1 |Sgreen Plant creature token.
 * |G, Sacrifice three creatures: Return target land card from your graveyard to your hand. */

/* Tuvasa the Sunlit	|G|W|U
 * Legendary Creature - Merfolk Shaman 1/1
 * ~ gets +1/+1 for each enchantment you control.
 * Whenever you cast your first enchantment spell each turn, draw a card. */

/* Varchild, Betrayer of Kjeldor	|2|R
 * Legendary Creature - Human Knight 3/3
 * Whenever ~ deals combat damage to a player, that player creates that many 1/1 |Sred Survivor creature tokens.
 * Survivors your opponents control can't block, and they can't attack you or a planeswalker you control.
 * When ~ leaves the battlefield, gain control of all Survivors. */

/* Varina, Lich Queen	|1|W|U|B
 * Legendary Creature - Zombie Wizard 4/4
 * Whenever you attack with one or more Zombies, draw that many cards, then discard that many cards. You gain that much life.
 * |2, Exile two cards from your graveyard: Create a tapped 2/2 |Sblack Zombie creature token. */

/* Vedalken Humiliator	|3|U
 * Creature - Vedalken Wizard 3/4
 * Metalcraft - Whenever ~ attacks, if you control three or more artifacts, creatures your opponents control lose all abilities and have base power and toughness 1/1 until end of turn. */

/* Whiptongue Hydra	|5|G
 * Creature - Lizard Hydra 4/4
 * Reach
 * When ~ enters the battlefield, destroy all creatures with flying. Put a +1/+1 counter on ~ for each creature destroyed this way. */

/* Windgrace's Judgment	|3|B|G
 * Instant
 * For any number of opponents, destroy target nonland permanent that player controls. */

/* Xantcha, Sleeper Agent	|1|B|R
 * Legendary Creature - Minion 5/5
 * As ~ enters the battlefield, an opponent of your choice gains control of it.
 * ~ attacks each combat if able and can't attack its owner or planeswalkers its owner controls.
 * |3: ~'s controller loses 2 life and you draw a card. Any player may activate this ability. */

/* Yennett, Cryptic Sovereign	|2|W|U|B
 * Legendary Creature - Sphinx 3/5
 * Flying, vigilance, menace
 * Whenever ~ attacks, reveal the top card of your library. If that card's converted mana cost is odd, you may cast it without paying its mana cost. Otherwise, draw a card. */

/* Yuriko, the Tiger's Shadow	|1|U|B
 * Legendary Creature - Human Ninja 1/3
 * Commander ninjutsu |U|B (|U|B, Return an unblocked attacker you control to hand: Put this card onto the battlefield from your hand or the command zone tapped and attacking.)
 * Whenever a Ninja you control deals combat damage to a player, reveal the top card of your library and put that card into your hand. Each opponent loses life equal to that card's converted mana cost. */

/* Recovered October 2022 implementations; see docs/recovery-progress.md. */
static int effect_estrids_invocation(int player, int card, event_t event);

/* ManalinkEh 2022: 0x2560da0; original cards/commander_2018.c:283. */
int card_ancient_stone_idol(int player, int card, event_t event)
{
  int result;
  token_generation_t token;
  if (event == EVENT_MODIFY_COST)
  {
    result = count_attackers(current_turn);
    COST_COLORLESS = COST_COLORLESS - (char)result;
  }
  result = this_dies_trigger(player, card, event, RESOLVE_TRIGGER_MANDATORY);
  if (result != 0)
  {
    default_token_definition(player, card, 0x4918, &token);
    token.pow = 6;
    token.tou = 0xc;
    token.key_plus = 0x80;
    generate_token(&token);
  }
  result = flash(player, card, event);
  return result;
}

/* ManalinkEh 2022: 0x255f760; original cards/commander_2018.c:373. */
static char *is_token_targ(int who_chooses, int player, int card)
{
  int result;
  char *pcVar2;
  result = is_token(player, card);
  pcVar2 = NULL;
  if (result == 0)
  {
    pcVar2 = "must be a token.";
  }
  return pcVar2;
}

/* ManalinkEh 2022: 0x255f730; original cards/commander_2018.c:380. */
static int is_token_test(int unused1, int unused2, int player, int card)
{
  int result;
  result = is_token(player, card);
  return (unsigned int)(result != 0);
}

/* ManalinkEh 2022: 0x2561630; original cards/commander_2018.c:454. */
int card_crash_of_rhino_beetles(int player, int card, event_t event)
{
  int result;
  if (((event - EVENT_POWER < 2) && (card == affected_card)) &&
      (player == affected_card_controller))
  {
    result = count_permanents_by_type(player, TYPE_LAND);
    if (9 < result)
    {
      event_result = event_result + 10;
    }
  }
  return 0;
}

/* ManalinkEh 2022: 0x25603e0; original cards/commander_2018.c:674. */
static int effect_estrids_invocation(int player, int card, event_t event)
{
  int iVar1;
  card_instance_t *instance;
  int iVar3;
  int player_00;

  iVar1 = effect_follows_control_of_attachment(player, card, event);
  if (iVar1 == 0)
  {
    instance = get_card_instance(player, card);
    if (player == current_turn)
    {
      player_00 = (int)instance->damage_target_player;
      iVar1 = instance->damage_target_card;
      iVar3 = duh_mode(player_00);
      iVar3 = upkeep_trigger_mode(player_00, iVar1, event, (iVar3 != 0) + RESOLVE_TRIGGER_OPTIONAL);
      if (iVar3 != 0)
      {
        blink_effect(player_00, iVar1, NULL);
      }
    }
  }
  return 0;
}

/* ManalinkEh 2022: 0x25621a0; original cards/commander_2018.c:686. */
int card_estrids_invocation(int player, int card, event_t event)
{
  int result;
  target_definition_t td;
  default_target_definition(player, card, &td, 4);
  td.allowed_controller = player;
  result = enters_the_battlefield_as_copy_of_any(player, card, event, &td,
                                                 "SELECT_AN_ENCHANTMENT_YOU_CONTROL");
  if (result != 0)
  {
    create_targetted_legacy_effect(player, card, effect_estrids_invocation, player, card);
  }
  result = global_enchantment(player, card, event);
  return result;
}

/* ManalinkEh 2022: 0x255f790; original cards/commander_2018.c:732. */
static int is_enchanted_test(int unused1, int unused2, int player, int card)
{
  int result;
  result = is_enchanted(player, card);
  return (unsigned int)(result != 0);
}

/* ManalinkEh 2022: 0x2564120; original cards/commander_2018.c:1366. */
int card_nesting_dragon(int player, int card, event_t event)
{
  landfall(player, card, event);
  if (event != EVENT_ETB)
  {
    return 0;
  }
  generate_token_by_id_2022(player, card, 0x33e2);
  return 0;
}

/* Recovered October 2022 implementations; see docs/recovery-progress.md. */

static int amaug_leg_part_2022(int player, int card, event_t event);

/* ManalinkEh 2022: 0x2560360; original cards/commander_2018.c:3. */
static int amaug_leg(int player, int card, event_t event)
{
  int result;

  result = eot_trigger(player, card, event);
  if (result != 0)
  {
    kill_card(player, card, KILL_DESTROY);
  }
  if (((event != EVENT_ACTIVATE) && (1 < event - EVENT_RESOLVE_ACTIVATION)) &&
      (event != EVENT_CLEANUP))
  {
    return 0;
  }
  result = amaug_leg_part_2022(player, card, event);
  return result;
}

/* Recovered October 2022 implementations; see docs/recovery-progress.md. */
static int effect_xantcha(int player, int card, event_t event);

/* ManalinkEh 2022: 0x25607b0; original cards/commander_2018.c:177. */
int card_aminatou_the_fateshifter(int player, int card, event_t event)
{
  card_instance_t *pcVar1;
  int iVar2;
  card_instance_t *pcVar3;
  int iVar4;
  int card_00;
  int iVar5;
  int iStack_470;
  test_definition_t *ptStack_468;
  int mc[2];
  target_definition_t td;
  test_definition_t this_test;
  test_definition_t test;

  check_legend_rule(player, card, event);
  if ((event == EVENT_CAN_ACTIVATE) || (event == EVENT_ACTIVATE))
  {
    pcVar1 = get_card_instance(player, card);
    default_target_definition(player, card, &td, 0x47);
    td.allowed_controller = player | 4;
    td.special = 0x4000;
    default_test_definition(&this_test, 0x47);
    this_test.type = 1;
    this_test.type_flag = 1;
    this_test.not_me = 1;
    if (event == EVENT_ACTIVATE)
    {

      pcVar1->number_of_targets = '\0';
      pcVar1->info_slot = 0;
      iVar2 = check_battlefield_for_special_card(player, card, 1 - player, 4, &this_test);
      iVar5 = check_battlefield_for_special_card(player, card, player, 4, &this_test);
      iVar4 = can_target(&td);
      iVar2 = dialog_impl(player, card, EVENT_ACTIVATE, "\001\002", "\001\017",
                          "Draw and put a card on your library", 1, 1, 1, "Blink", iVar4, 4,
                          0xffffffff, "Trade permanents", 1, iVar2 - iVar5, 0xfffffffa, 0, 0, 0, 0);
      if (iVar2 == 0)
      {
        spell_fizzled = 1;
        return 0;
      }
      if (iVar2 == 2)
      {
        new_pick_target(&td, "Select a permanent you own.", 0, 0x81);
      }
      goto LAB_02560885;
    }
  }
  else
  {
    if ((event != EVENT_RESOLVE_ACTIVATION) && (event != EVENT_CLEANUP))
      goto LAB_02560885;
    pcVar1 = get_card_instance(player, card);
    default_target_definition(player, card, &td, 0x47);
    td.allowed_controller = player | 4;
    td.special = 0x4000;
    default_test_definition(&this_test, 0x47);
    this_test.type = 1;
    this_test.type_flag = 1;
    this_test.not_me = 1;
  }
  ptStack_468 = &this_test;
  this_test.not_me = 1;
  this_test.type_flag = 1;
  this_test.type = 1;
  if (event == EVENT_RESOLVE_ACTIVATION)
  {

    iVar2 = pcVar1->info_slot;
    if (iVar2 == 1)
    {

      draw_a_card(player);
      new_default_test_definition(&test, 0xff, "Select a card to put on top.");

      if ((hand_count[player] != 0) &&
          (iVar2 = new_select_a_card(player, player, 4, 1, 3, -1, &test), iVar2 != -1))
      {
        put_on_top_of_deck(player, iVar2);
      }
    }
    else if (iVar2 == 2)
    {
      iVar2 = valid_target(&td);
      if (iVar2 != 0)
      {
        blink_effect(pcVar1->targets[0].player, pcVar1->targets[0].card, NULL);
      }
    }
    else if (iVar2 == 3)
    {

      mc[0] = 0;
      mc[1] = 0;
      iVar5 = 0;
      iVar2 = current_turn;
      do
      {
        if (iVar5 == 0)
        {
          card_00 = active_cards_count[iVar2] + -1;
          iVar4 = iVar2;
          if (card_00 < 0)
          {
            iVar5 = 1;
            goto LAB_025608fe;
          }
        LAB_0256092a:
          do
          {
            pcVar3 = in_play(iVar4, card_00);
            if ((((pcVar3 != NULL) &&
                  (iVar2 = check_state(iVar4, card_00, 0x100000), iVar2 == 0)) &&
                 (iVar2 = is_what(iVar4, card_00, 0x47), iVar2 != 0)) &&
                ((iVar2 = new_make_test_in_play(iVar4, card_00, -1, ptStack_468),
                  iVar2 != 0 &&
                      ((pcVar1->parent_controller != iVar4 || (pcVar1->parent_card != card_00))))))
            {
              iVar2 = mc[iVar4];
              mc[iVar4] = iVar2 + 1;
              *(int *)((int)&test + (iVar2 + iVar4 * 100) * 4) = card_00;
            }
            card_00 = card_00 + -1;
            iVar2 = current_turn;
          } while (card_00 != -1);
        }
        else
        {
        LAB_025608fe:
          card_00 = active_cards_count[1 - iVar2] + -1;
          iVar4 = 1 - iVar2;
          if (-1 < card_00)
            goto LAB_0256092a;
        }
        iVar5 = iVar5 + 1;
      } while (iVar5 != 2);
      iStack_470 = 0;
    LAB_02560bf2:
      do
      {

        if (iStack_470 == 0)
        {
          ptStack_468 = (test_definition_t *)mc[iVar2];
          if ((int)ptStack_468 < 1)
          {
            iStack_470 = 1;
            goto LAB_02560bf2;
          }
        LAB_02560c1a:
          iVar5 = 0;
          do
          {
            while (true)
            {
              iVar4 = *(int *)((int)&test + iVar5 * 4 + iVar2 * 400);
              if (pcVar1->parent_controller != iVar2)
                break;
              iVar5 = iVar5 + 1;
              give_control(iVar2, pcVar1->parent_card, iVar2, iVar4);
              if ((int)ptStack_468 <= iVar5)
                goto LAB_02560c87;
            }
            iVar5 = iVar5 + 1;
            gain_control_permanently(pcVar1->parent_controller, iVar2, iVar4);
          } while (iVar5 < (int)ptStack_468);
        }
        else
        {
          iVar2 = 1 - iVar2;
          ptStack_468 = (test_definition_t *)mc[iVar2];
          if (0 < (int)ptStack_468)
            goto LAB_02560c1a;
        }
      LAB_02560c87:
        iStack_470 = iStack_470 + 1;
        iVar2 = current_turn;
      } while (iStack_470 < 2);
    }
  }
LAB_02560885:
  iVar2 = planeswalker(player, card, event, 3);
  return iVar2;
}

/* ManalinkEh 2022: 0x2561390; original cards/commander_2018.c:430. */
int card_coveted_jewel(int player, int card, event_t event)
{
  int iVar1;
  int iVar2;
  card_instance_t *instance;
  int iVar4;
  int iVar5;

  etb_ability(player, card, event, RESOLVE_TRIGGER_MANDATORY);
  if (event == EVENT_ETB)
  {
    draw_cards(player, 3);
  }
  else if ((((event == EVENT_PHASE_CHANGED) && (current_phase == 0x18)) &&
            (current_turn != player)) &&
           (iVar2 = is_humiliated_for_ability(player, card, AL_TRIG), iVar1 = current_turn,
            iVar2 == 0))
  {

    iVar2 = active_cards_count[current_turn] + -1;
    if (-1 < iVar2)
    {
      do
      {
        if ((((iVar1 == 1 - player) && (instance = in_play(iVar1, iVar2), instance != NULL)) &&
             ((iVar4 = is_what(iVar1, iVar2, 2),
               iVar4 != 0 &&
                   ((instance = get_card_instance(iVar1, iVar2),
                     (instance->state & 4) != 0 && (instance = get_card_instance(iVar1, iVar2),
                                                    (instance->state & 0x200) == 0)))))) &&
            (iVar4 = check_special_flags_2022(player, card, 0x40), iVar4 == 0))
        {
          draw_cards(iVar1, 3);
          untap_card(player, card);
          give_control_of_self(player, card);
        }
        iVar2 = iVar2 + -1;
      } while (iVar2 != -1);
    }
    iVar1 = current_turn;
    iVar2 = 1 - current_turn;
    iVar4 = active_cards_count[iVar2] + -1;
    if (-1 < iVar4)
    {
      do
      {
        if (((((player == iVar1) && (instance = in_play(iVar2, iVar4), instance != NULL)) &&
              (iVar5 = is_what(iVar2, iVar4, 2), iVar5 != 0)) &&
             ((instance = get_card_instance(iVar2, iVar4),
               (instance->state & 4) != 0 && (instance = get_card_instance(iVar2, iVar4),
                                              (instance->state & 0x200) == 0)))) &&
            (iVar5 = check_special_flags_2022(player, card, 0x40), iVar5 == 0))
        {
          draw_cards(1 - player, 3);
          untap_card(player, card);
          give_control_of_self(player, card);
        }
        iVar4 = iVar4 + -1;
      } while (iVar4 != -1);
    }
  }
  iVar1 = artifact_mana_all_one_color(player, card, event, 3, 0);
  return iVar1;
}

/* ManalinkEh 2022: 0x2561ab0; original cards/commander_2018.c:554. */
int card_endless_atlas(int player, int card, event_t event)
{
  int iVar1;
  int iVar2;
  card_instance_t *instance;
  int iVar4;

  if (event == EVENT_CAN_ACTIVATE)
  {
    iVar2 = generic_activated_ability(player, card, EVENT_CAN_ACTIVATE, 1, 2, 0, 0, 0, 0, 0, 0,
                                      NULL, NULL);
    iVar1 = current_turn;
    if (iVar2 != 0)
    {

      iVar2 = active_cards_count[current_turn] + -1;
      if (-1 < iVar2)
      {
        do
        {
          if ((player == iVar1) && (instance = in_play(player, iVar2), instance != NULL))
          {
            iVar4 = get_card_name(player, iVar2);
            iVar4 = count_permanents_by_name(player, TYPE_LAND, iVar4);
            if (2 < iVar4)
            {
              return 1;
            }
          }
          iVar2 = iVar2 + -1;
        } while (iVar2 != -1);
      }
      iVar1 = 1 - current_turn;
      iVar2 = active_cards_count[iVar1] + -1;
      if (-1 < iVar2)
      {
        do
        {
          if ((player == iVar1) && (instance = in_play(player, iVar2), instance != NULL))
          {
            iVar4 = get_card_name(player, iVar2);
            iVar4 = count_permanents_by_name(player, TYPE_LAND, iVar4);
            if (2 < iVar4)
            {
              return 1;
            }
          }
          iVar2 = iVar2 + -1;
        } while (iVar2 != -1);
      }
    }
    return 0;
  }
  if (event == EVENT_ACTIVATE)
  {
  LAB_02561af1:
    if (event != EVENT_RESOLVE_ACTIVATION)
      goto LAB_02561af6;
  }
  else if (event != EVENT_RESOLVE_ACTIVATION)
  {
    if (event != EVENT_CLEANUP)
    {
      return 0;
    }
    goto LAB_02561af1;
  }
  draw_a_card(player);
LAB_02561af6:
  iVar1 = generic_activated_ability(player, card, event, 1, 2, 0, 0, 0, 0, 0, 0, NULL, NULL);
  return iVar1;
}

/* ManalinkEh 2022: 0x2562370; original cards/commander_2018.c:738. */
int card_estrid_the_masked(int player, int card, event_t event)
{
  card_instance_t *instance;
  int iVar2;
  int iVar3;
  int iVar4;
  int iVar5;
  test_definition_t test;
  test_definition_t test2;
  test_definition_t test_ench;

  check_legend_rule(player, card, event);
  if ((event == EVENT_CAN_ACTIVATE) || (event == EVENT_ACTIVATE))
  {
    instance = get_card_instance(player, card);
    default_test_definition(&test, 0x47);
    test.special_selection_function = is_enchanted_test;
    default_test_definition(&test2, 0x47);
    if (event != EVENT_ACTIVATE)
      goto LAB_02562408;

    instance->number_of_targets = '\0';
    instance->info_slot = 0;
    test.state = 0x10;
    iVar5 = check_battlefield_for_special_card(player, card, player, 4, &test);
    iVar2 = check_battlefield_for_special_card(player, card, player, 4, &test2);
    iVar3 = count_cards_by_id(player, 0x4aa4);
    iVar4 = count_graveyard_by_type(player, 4);
    iVar5 = dialog_impl(player, card, EVENT_ACTIVATE, "\001\002", "\001\017",
                        "Untap all enchanted permanents", 1, iVar5, 2, "Create a Mask", 1,
                        (iVar2 - iVar3) * 3, 0xffffffff, "Replenish enchantments", 1, iVar4 * 5,
                        0xfffffff9, 0, 0, 0, 0);
    if (iVar5 == 0)
    {
      spell_fizzled = 1;
      return 0;
    }
  }
  else
  {
    if ((event != EVENT_RESOLVE_ACTIVATION) && (event != EVENT_CLEANUP))
    {
      iVar5 = 4;
      goto LAB_02562415;
    }
    instance = get_card_instance(player, card);
    default_test_definition(&test, 0x47);
    test.special_selection_function = is_enchanted_test;
    default_test_definition(&test2, 0x47);
  LAB_02562408:
    if (event == EVENT_RESOLVE_ACTIVATION)
    {

      iVar5 = instance->info_slot;
      if (iVar5 == 1)
      {
        new_manipulate_all(player, card, player, &test, ACT_UNTAP);
      }
      else if (iVar5 == 2)
      {
        generate_token_by_id_2022(player, card, 0x4aa4);
      }
      else if (iVar5 == 3)
      {
        mill(player, 7);
        default_test_definition(&test_ench, 4);
        test_ench.subtype = 0x4000;
        test_ench.subtype_flag = 1;
        new_reanimate_all(player, card, player, &test_ench, REANIMATE_DEFAULT);
        test_ench.subtype_flag = 0;
        new_reanimate_all(player, card, player, &test_ench, REANIMATE_DEFAULT);
      }
    }
  }
  iVar5 = 3;
LAB_02562415:
  iVar5 = planeswalker(player, card, event, iVar5);
  return iVar5;
}

/* ManalinkEh 2022: 0x25626e0; original cards/commander_2018.c:813. */
int card_ever_watching_threshold(int player, int card, event_t event)
{
  int result;

  store_attackers(player, card, event, 0, 1 - player, -1, NULL);
  dat_ability(player, card, event, DAT_RESOLVE_TRIGGER_MANDATORY, 1);
  if (event == EVENT_DAT_ABILITY)
  {
    draw_a_card(player);
  }
  result = global_enchantment(player, card, event);
  return result;
}

/* ManalinkEh 2022: 0x255f870; original cards/commander_2018.c:1261. */
static int loyal_unicorn_legacy(int player, int card, event_t event)
{
  card_instance_t *instance;
  int result;

  instance = combat_damage_being_prevented(event);
  if (((instance != NULL) && (instance->damage_target_card != -1)) &&
      (instance->damage_target_player == player))
  {
    prevent_damage(affected_card_controller, affected_card, -1);
  }
  instance = get_card_instance(player, card);
  if (((event == EVENT_ABILITIES) && (affected_card_controller == player)) &&
      (result = instance->targets[0].card, result != affected_card))
  {
    result = is_humiliated_for_ability(instance->targets[0].player, result, AL_STATIC);
    if (result == 0)
    {
      special_abilities_2022(affected_card_controller, affected_card, EVENT_ABILITIES, 8,
                        instance->targets[0].player, instance->targets[0].card);
    }
  }
  result = eot_trigger(player, card, event);
  if ((event != EVENT_CAN_SKIP_TURN) && (result == 0))
  {
    return 0;
  }
  kill_card(player, card, KILL_EXILE);
  return 0;
}

/* ManalinkEh 2022: 0x2563ca0; original cards/commander_2018.c:1297. */
int card_magus_of_the_balance(int player, int card, event_t event)
{
  int iVar1;
  int iVar2;
  int iVar3;
  card_instance_t *instance;
  unsigned int uVar5;
  int t_player;
  int t_player_00;
  unsigned int what_player;
  int diff[2][3];
  test_definition_t test;

  if (event != EVENT_ACTIVATE)
  {
    if ((1 < event - EVENT_RESOLVE_ACTIVATION) && (event != EVENT_CLEANUP))
    {
      return 0;
    }
    if (event == EVENT_RESOLVE_ACTIVATION)
    {

      uVar5 = 0;
      do
      {
        what_player = uVar5 ^ current_turn;
        iVar1 = count_subtype(what_player, 1, -1);
        iVar2 = count_subtype(1 - what_player, 1, -1);
        iVar3 = current_turn;
        uVar5 = uVar5 + 1;
        diff[what_player][0] = iVar1 - iVar2;
      } while (uVar5 != 2);

      iVar2 = 1 - current_turn;
      diff[current_turn][1] = hand_count[current_turn] - hand_count[iVar2];
      uVar5 = current_turn ^ 1;

      diff[uVar5][1] = hand_count[uVar5] - hand_count[1 - uVar5];
      iVar1 = count_subtype(current_turn, 2, -1);
      iVar2 = count_subtype(iVar2, 2, -1);
      diff[iVar3][2] = iVar1 - iVar2;
      uVar5 = current_turn ^ 1;
      iVar3 = count_subtype(uVar5, 2, -1);
      iVar1 = count_subtype(1 - uVar5, 2, -1);
      t_player = 1 - player;
      diff[uVar5][2] = iVar3 - iVar1;
      iVar3 = diff[t_player][0];
      iVar1 = diff[player][0];
      iVar2 = iVar1;
      if (iVar1 <= iVar3)
      {
        iVar2 = iVar3;
      }
      if (iVar2 != 0)
      {

        t_player_00 = player;
        if (iVar1 <= iVar3)
        {
          t_player_00 = t_player;
        }
        new_default_test_definition(&test, 1, "Select a land to sacrifice.");
        do
        {
          new_sacrifice(player, card, t_player_00, SAC_JUST_MARK, &test);
          iVar2 = iVar2 + -1;
        } while (iVar2 != 0);
        iVar3 = active_cards_count[t_player_00] + -1;
        if (-1 < iVar3)
        {
          do
          {
            instance = in_play(t_player_00, iVar3);
            if (((instance != NULL) && (iVar1 = is_what(t_player_00, iVar3, 1), iVar1 != 0)) &&
                (iVar1 = check_state(t_player_00, iVar3, 0x100000), iVar1 != 0))
            {
              kill_card(t_player_00, iVar3, KILL_SACRIFICE);
            }
            iVar3 = iVar3 + -1;
          } while (iVar3 != -1);
        }
      }
      iVar3 = diff[t_player][1];
      iVar1 = diff[player][1];
      iVar2 = iVar3;
      if (iVar3 < iVar1)
      {
        iVar2 = iVar1;
      }
      if (iVar2 != 0)
      {

        iVar2 = player;
        if (iVar1 <= iVar3)
        {
          iVar1 = iVar3;
          iVar2 = t_player;
        }
        new_multidiscard(iVar2, iVar1, 0, player);
      }
      iVar3 = diff[t_player][2];
      iVar1 = diff[player][2];
      iVar2 = iVar3;
      if (iVar3 < iVar1)
      {
        iVar2 = iVar1;
      }
      if (iVar2 != 0)
      {

        if (iVar3 < iVar1)
        {
          t_player = player;
        }
        new_default_test_definition(&test, 2, "Select a creature to sacrifice.");
        do
        {
          new_sacrifice(player, card, t_player, SAC_JUST_MARK, &test);
          iVar2 = iVar2 + -1;
        } while (iVar2 != 0);
        iVar3 = active_cards_count[t_player] + -1;
        if (-1 < iVar3)
        {
          do
          {
            instance = in_play(t_player, iVar3);
            if (((instance != NULL) && (iVar1 = is_what(t_player, iVar3, 2), iVar1 != 0)) &&
                (iVar1 = check_state(t_player, iVar3, 0x100000), iVar1 != 0))
            {
              kill_card(t_player, iVar3, KILL_SACRIFICE);
            }
            iVar3 = iVar3 + -1;
          } while (iVar3 != -1);
        }
      }
    }
  }
  iVar3 = generic_activated_ability(player, card, event, 9, 4, 0, 0, 0, 0, 1, 0, NULL, NULL);
  return iVar3;
}

/* ManalinkEh 2022: 0x25640e0; original cards/commander_2018.c:1355. */
int card_myth_unbound(int player, int card, event_t event)
{
  int result;

  if (event == EVENT_COMMANDER_TO_COMMAND_ZONE)
  {
    draw_a_card(player);
  }
  result = global_enchantment(player, card, event);
  return result;
}

/* ManalinkEh 2022: 0x2564180; original cards/commander_2018.c:1388. */
int card_octopus_umbra(int player, int card, event_t event)
{
  card_instance_t *instance;
  int iVar2;
  int iVar3;
  int attacker_player;
  target_definition_t td;

  instance = get_card_instance(player, card);
  if (-1 < instance->damage_target_player)
  {

    instance = get_card_instance(player, card);
    attacker_player = (int)instance->damage_target_player;
    iVar2 = instance->damage_target_card;
    store_attackers(player, card, event, 0, attacker_player, iVar2, NULL);
    iVar3 = duh_mode(player);
    dat_ability(player, card, event, (iVar3 != 0) + DAT_RESOLVE_TRIGGER_OPTIONAL, 1);
    if (event == EVENT_DAT_ABILITY)
    {
      default_target_definition(attacker_player, iVar2, &td, 2);
      td.power_requirement = 0x2008;
      instance = get_card_instance(attacker_player, iVar2);
      instance->number_of_targets = '\0';
      iVar3 = new_pick_target(&td, "Select target creature with power 8 or less", 0, 0x81);
      if (iVar3 != 0)
      {
        instance = get_card_instance(attacker_player, iVar2);
        iVar3 = instance->targets[0].card;
        instance = get_card_instance(attacker_player, iVar2);
        tap_card(instance->targets[0].player, iVar3);
      }
    }
  }
  instance = in_play(player, card);
  if (instance != NULL)
  {
    iVar2 = is_humiliated_for_ability(player, card, AL_STATIC);
    if (iVar2 == 0)
    {
      instance = get_card_instance(player, card);
      if (instance->damage_target_player != -1)
      {

        instance = get_card_instance(player, card);
        if ((((event == EVENT_POWER) || (event == EVENT_TOUGHNESS)) &&
             (instance->damage_target_card == affected_card)) &&
            (instance->damage_target_player == affected_card_controller))
        {
          set_base_pt(8);
        }
      }
    }
  }
  iVar2 = totem_armor(player, card, event, 0, 0, 0, 0);
  return iVar2;
}

/* ManalinkEh 2022: 0x2565870; original cards/commander_2018.c:1775. */
int card_treasure_nabber(int player, int card, event_t event)
{
  int result;
  card_instance_t *instance;

  if (event == EVENT_TAP_CARD)
  {
    result = is_what(affected_card_controller, affected_card, 0x40);
    if (((result != 0) && (1 - player == affected_card_controller)) && (-1 < tapped_for_mana_color))
    {
      instance = get_card_instance(1 - player, affected_card);
      if ((cards_data[instance->internal_card_id].extra_ability & 0x1000) != 0)
      {
        result = is_humiliated_for_ability(player, card, AL_TRIG);
        if (result == 0)
        {
          gain_control_until_eoyt(player, card, affected_card_controller, affected_card);
        }
      }
    }
  }
  return 0;
}

/* ManalinkEh 2022: 0x255fb00; original cards/commander_2018.c:1966. */
static int effect_xantcha(int player, int card, event_t event)
{
  int card_00;
  card_instance_t *instance;
  int iVar2;
  int iVar3;

  if (event == EVENT_STATIC_EFFECTS)
  {
    if ((card == affected_card) && (player == affected_card_controller))
    {

      instance = get_card_instance(player, card);
      iVar2 = instance->targets[0].player;
      if (-1 < iVar2)
      {
        iVar2 = get_id(iVar2, instance->targets[0].card);
        if (iVar2 != 0x4abc)
        {
          kill_card(player, card, KILL_EXILE);
        }
        instance = in_play(instance->targets[0].player, instance->targets[0].card);
        if (instance == NULL)
        {
          kill_card(player, card, KILL_EXILE);
        }
      }
    }
    instance = get_card_instance(player, card);
    if (instance->targets[0].player != -1)
    {
      get_card_instance(player, card);
    }
  }
  else
  {
    instance = get_card_instance(player, card);
    if (instance->targets[0].player != -1)
    {

      instance = get_card_instance(player, card);
      iVar2 = instance->targets[0].player;
      card_00 = instance->targets[0].card;
      if (event == EVENT_CAN_ACTIVATE)
      {
        iVar3 = can_use_activated_abilities_2022(iVar2, card_00);
        if (iVar3 != 0)
        {

          iVar2 = get_cost_mod_for_activated_abilities_2022(iVar2, card_00, 3, 0, 0, 0, 0, 0);
          iVar2 = has_mana_multi(player, iVar2, 0, 0, 0, 0, 0);
          if (iVar2 != 0)
          {
            return 1;
          }
        }
      }
      else
      {
        if (event == EVENT_ACTIVATE)
        {

          iVar2 = get_cost_mod_for_activated_abilities_2022(iVar2, card_00, 3, 0, 0, 0, 0, 0);
          charge_mana_multi(player, iVar2, 0, 0, 0, 0, 0);
          return 0;
        }
        if (event == EVENT_RESOLVE_ACTIVATION)
        {
          lose_life(iVar2, 2);
          draw_a_card(player);
        }
      }
    }
  }
  return 0;
}

/* ManalinkEh 2022: 0x2566100; original cards/commander_2018.c:2007. */
int card_xantcha_sleeper_agent(int player, int card, event_t event)
{
  bool bVar1;
  card_instance_t *pcVar2;
  int iVar3;
  card_instance_t *pcVar4;
  int iVar5;
  int iVar6;

  check_legend_rule(player, card, event);
  if (event == EVENT_RESOLVE_SPELL)
  {
    give_control_of_self(player, card);
  }
  pcVar2 = in_play(player, card);
  if (((pcVar2 != NULL) &&
       (iVar3 = is_humiliated_for_ability(player, card, AL_STATIC), iVar3 == 0)) &&
      (iVar3 = get_owner(player, card), iVar3 != player))
  {
    cannot_attack(player, card, event);
  }
  iVar3 = is_humiliated_for_ability(player, card, AL_STATIC);
  if (iVar3 == 0)
  {
    attack_if_able_2022(player, card, event);
  }
  pcVar2 = get_card_instance(player, card);
  if (event == EVENT_STATIC_EFFECTS)
  {
    if (((card == affected_card) && (player == affected_card_controller)) &&
        (pcVar4 = in_play(player, card), pcVar4 != NULL))
    {

      bVar1 = false;
      iVar3 = 0;
      do
      {
        iVar5 = 0;
        if (-1 < active_cards_count[iVar3])
        {
          do
          {
            pcVar4 = in_play(iVar3, iVar5);

            if (((pcVar4 != NULL) && (iVar6 = is_what(iVar3, iVar5, 0x80), iVar6 != 0)) &&
                ((pcVar4 = get_card_instance(iVar3, iVar5),
                  pcVar4->info_slot_fn == effect_xantcha && ((pcVar4->targets[0].player == player &&
                                                              (pcVar4->targets[0].card == card))))))
            {
              if (player == iVar3)
              {
                kill_card(player, iVar5, KILL_EXILE);
              }
              else
              {
                if (iVar3 == 1)
                  goto LAB_02566196;
                bVar1 = true;
              }
            }
            iVar5 = iVar5 + 1;
          } while (iVar5 <= active_cards_count[iVar3]);
        }
        iVar3 = iVar3 + 1;
      } while (iVar3 != 2);
      if (!bVar1)
      {

        iVar6 = 1 - player;
        iVar3 = add_card_to_hand(iVar6, pcVar2->internal_card_id);
        iVar5 = create_legacy_activate(iVar6, iVar3, effect_xantcha);
        pcVar2 = get_card_instance(iVar6, iVar5);
        pcVar2->targets[0].player = player;
        pcVar2->targets[0].card = card;
        pcVar2->number_of_targets = '\x01';
        obliterate_card(iVar6, iVar3);
      }
    }
  }
  else if (event == EVENT_RESOLVE_ACTIVATION)
  {
    lose_life(player, 2);
    draw_a_card(player);
  }
LAB_02566196:
  iVar3 = generic_activated_ability(player, card, event, 0, 3, 0, 0, 0, 0, 0, 0, NULL, NULL);
  return iVar3;
}

/* Recovered October 2022 implementations; see docs/recovery-progress.md. */

/* ManalinkEh 2022: 0x2565750; original cards/commander_2018.c:1751. */
int card_thantis_the_warweaver(int player, int card, event_t event)
{
  card_instance_t *instance;
  int result;

  check_legend_rule(player, card, event);
  vigilance_2022(player, card, event);
  instance = in_play(player, card);
  if (instance != NULL)
  {
    result = is_humiliated_for_ability(player, card, AL_STATIC);
    if (result == 0)
    {
      all_must_attack_if_able(current_turn, event, -1);
    }
  }
  store_attackers(player, card, event, DAT_SEPARATE_TRIGGERS, 1 - player, -1, NULL);
  dat_ability(player, card, event, DAT_SEPARATE_TRIGGERS | DAT_RESOLVE_TRIGGER_MANDATORY, 1);
  if (event != EVENT_DAT_ABILITY)
  {
    return 0;
  }
  add_counters_from_source(player, card, player, card, COUNTER_P1_P1, 1);
  return 0;
}

/* ManalinkEh 2022: 0x2566040; original cards/commander_2018.c:1945. */
int card_whiptongue_hydra(int player, int card, event_t event)
{
  int num;
  test_definition_t this_test;

  etb_ability_2022(player, card, event, RESOLVE_TRIGGER_MANDATORY);
  if (event != EVENT_ETB)
  {
    return 0;
  }

  default_test_definition(&this_test, 2);
  this_test.keyword = 0x20;
  num = new_manipulate_all(player, card, 2, &this_test, ACT_KILL_DESTROY);
  add_counters_from_source(player, card, player, card, COUNTER_P1_P1, num);
  return 0;
}

/* Recovered October 2022 implementations; see docs/recovery-progress.md. */

/* ManalinkEh 2022: 0x25604e0; original cards/commander_2018.c:123. */
int card_aminatous_augury(int player, int card, event_t event)
{
  int *piVar1;
  int iVar2;
  int iVar3;
  int iVar4;
  card_instance_t *instance;
  int32_t *piVar6;
  int *piVar7;
  bool bVar8;
  int iStack_f4;
  int iStack_f0;
  test_definition_t test;

  if (event == EVENT_CAN_CAST)
    goto LAB_025606e7;
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
    goto LAB_025606e7;
  }
  if (event != EVENT_RESOLVE_SPELL && event != EVENT_CAN_CHANGE_TARGET)
  {
    if (event != EVENT_CHANGE_TARGET)
    {
      return 0;
    }
    goto LAB_025606e7;
  }
  if (event != EVENT_RESOLVE_SPELL)
    goto LAB_025606e7;

  iVar2 = count_deck(player);
  if (iVar2 < 1)
  {
    return 0;
  }
  if (8 < iVar2)
  {
    iVar2 = 8;
  }
  new_default_test_definition(&test, 1, "Select a land card to put onto the battlefield.");
  test.create_minideck = (unsigned short)iVar2;
  iVar3 = select_card_from_zone(player, player, deck_ptr[player], iVar2, 0, 1, -1, &test);
  iStack_f0 = iVar3;
  if (iVar3 == -1)
  {
  LAB_02560640:

    iVar3 = create_legacy_activate(player, card, amaug_leg);
    instance = get_card_instance(player, iVar3);
    bVar8 = false;
  LAB_02560663:
    piVar7 = deck_ptr[player];
    piVar6 = &instance->targets[0].card;
    piVar1 = piVar7 + iVar2;
    do
    {
      iVar3 = *piVar7;
      piVar7 = piVar7 + 1;
      *piVar6 = iVar3;
      piVar6 = piVar6 + 2;
    } while (piVar1 != piVar7);
  LAB_02560689:
    rfg_top_n_cards_of_deck(player, iVar2);
    iStack_f4 = iStack_f0;
  }
  else
  {
    iStack_f4 = deck_ptr[player][iVar3];
    reveal_card_iid(player, card, iStack_f4);
    iVar4 = grafdiggers_cages(player, TYPE_CREATURE, TUTOR_FROM_DECK);
    iStack_f0 = iStack_f4;
    if ((iVar4 != 0) && (iVar4 = is_what(-1, deck_ptr[player][iVar3], 2), iVar4 != 0))
      goto LAB_02560640;
    remove_card_from_deck(player, iVar3);
    bVar8 = iStack_f4 != -1;
    iVar2 = iVar2 + -1;
    if (iVar2 != 0)
    {
      iVar3 = create_legacy_activate(player, card, amaug_leg);
      instance = get_card_instance(player, iVar3);
      if (0 < iVar2)
        goto LAB_02560663;
      goto LAB_02560689;
    }
  }
  if (bVar8)
  {

    iVar2 = add_card_to_hand(player, iStack_f4);
    set_special_flags_2022(player, iVar2, 0x10000);
  }
  kill_card(player, card, KILL_DESTROY);
LAB_025606e7:
  iVar2 = basic_spell(player, card, event);
  return iVar2;
}

/* ManalinkEh 2022: 0x2562230; original cards/commander_2018.c:729. */
int card_mask(int player, int card, event_t event)
{
  card_instance_t *pcVar1;
  int player_00;
  card_instance_t *pcVar2;
  int iVar3;
  int iVar4;
  target_definition_t td;

  pcVar1 = get_card_instance(player, card);
  player_00 = (int)pcVar1->damage_target_player;
  iVar4 = pcVar1->damage_target_card;
  pcVar2 = in_play(player, card);
  if ((pcVar2 != NULL) && (player_00 != -1))
  {
    if (pcVar1->targets[7].player == 1)
    {
      return 0;
    }
    pcVar1->targets[7].player = 1;
    iVar3 = is_this_dying(player_00, iVar4);
    if (iVar3 != 0)
    {

      pcVar2 = get_card_instance(player_00, iVar4);
      if ((unsigned char)(pcVar2->kill_code - 1) < 2)
      {

        iVar3 = is_tapped(player_00, iVar4);
        regenerate_target_exe(player_00, iVar4);
        if (iVar3 == 0)
        {
          untap_card_no_event(player_00, iVar4);
        }
        kill_card(player, card, KILL_SACRIFICE);
      }
    }
    pcVar1->targets[7].player = 0;
  }
  default_target_definition(player, card, &td, 0x47);
  td.preferred_controller = player;
  iVar4 = targeted_aura(player, card, event, &td, "TARGET_PERMANENT");
  return iVar4;
}

/* ManalinkEh 2022: 0x2564390; original cards/commander_2018.c:1445. */
int card_retrofitter_foundry(int player, int card, event_t event)
{
  card_instance_t *instance;
  int iVar2;
  int iVar3;
  int iVar4;
  uint32_t uVar5;
  int iVar6;
  const char *prompt;
  test_definition_t *test;
  int iStack_274;
  char servo_buffer[50];
  char thopter_buffer[50];
  token_generation_t token;
  test_definition_t test_servo;
  test_definition_t test_thopter;

  if ((event == EVENT_CAN_ACTIVATE) || (event == EVENT_ACTIVATE))
  {
    scnprintf(servo_buffer, 0x32, " Select a Servo to sacrifice");
    new_default_test_definition(&test_servo, 0xff, servo_buffer);
    test_servo.subtype = 0xee;
    scnprintf(thopter_buffer, 0x32, " Select a Thopter to sacrifice");
    new_default_test_definition(&test_thopter, 0xff, thopter_buffer);
    test_thopter.subtype = 0xc6;
    instance = get_card_instance(player, card);
    if (event == EVENT_CAN_ACTIVATE)
    {
      iVar6 = generic_activated_ability(player, card, EVENT_CAN_ACTIVATE, 1, 2, 0, 0, 0, 0, 0, 0,
                                        NULL, NULL);
      if (iVar6 != 0)
      {
        return 1;
      }
      iVar6 = generic_activated_ability(player, card, EVENT_CAN_ACTIVATE, 1, 1, 0, 0, 0, 0, 0, 0,
                                        NULL, NULL);
      if ((iVar6 != 0) &&
          (iVar6 = new_can_sacrifice_as_cost(player, card, &test_servo), iVar6 != 0))
      {
        return 1;
      }
      iVar6 = generic_activated_ability(player, card, EVENT_CAN_ACTIVATE, 1, 0, 0, 0, 0, 0, 0, 0,
                                        NULL, NULL);
      if ((iVar6 != 0) &&
          (iVar6 = new_can_sacrifice_as_cost(player, card, &test_thopter), iVar6 != 0))
      {
        return 1;
      }
      iVar6 = generic_activated_ability(player, card, EVENT_CAN_ACTIVATE, 0, 3, 0, 0, 0, 0, 0, 0,
                                        NULL, NULL);
      return iVar6;
    }
    if (event == EVENT_ACTIVATE)
    {

      if (player == 1)
      {
        iStack_274 = generic_activated_ability(1, card, EVENT_CAN_ACTIVATE, 0x80000, 3, 0, 0, 0, 0,
                                               0, 0, NULL, NULL);
      }
      else
      {
        iStack_274 = generic_activated_ability(player, card, EVENT_CAN_ACTIVATE, 0, 3, 0, 0, 0, 0,
                                               0, 0, NULL, NULL);
      }
      iVar6 = generic_activated_ability(player, card, EVENT_CAN_ACTIVATE, 1, 2, 0, 0, 0, 0, 0, 0,
                                        NULL, NULL);
      iVar2 = generic_activated_ability_with_sacrifice_as_cost(
          player, card, EVENT_CAN_ACTIVATE, GAA_UNTAPPED, 1, 0, 0, 0, 0, 0, 0, NULL, NULL,
          TYPE_NONE, &test_servo, SARV_0);
      iVar3 = generic_activated_ability_with_sacrifice_as_cost(
          player, card, EVENT_CAN_ACTIVATE, GAA_UNTAPPED, 0, 0, 0, 0, 0, 0, 0, NULL, NULL,
          TYPE_NONE, &test_thopter, SARV_0);
      iVar4 = is_tapped(player, card);
      uVar5 = 0;
      if (iVar4 != 0)
      {
        uVar5 = 0x14;
      }
      iVar6 =
          dialog_impl(player, card, EVENT_ACTIVATE, "\001\002", "\001\016",
                      "Untap Retrofitter Foundry", iStack_274, uVar5, "Create a Servo", iVar6, 5,
                      "Create a Thopter", iVar2, 10, "Create a Construct", iVar3, 0xf, 0, 0, 0, 0);
      if (iVar6 == 0)
      {
        spell_fizzled = 1;
        return 0;
      }
      if (iVar6 == 1)
      {
        iVar6 = generic_activated_ability(player, card, EVENT_ACTIVATE, 0, 3, 0, 0, 0, 0, 0, 0,
                                          NULL, NULL);
        return iVar6;
      }
      if (iVar6 != 2)
      {
        if (iVar6 == 3)
        {
          test = &test_servo;
          prompt = "Select a a Servo to sacrifice";
          iVar6 = 1;
        }
        else
        {
          if (iVar6 != 4)
          {
            return 0;
          }
          test = &test_thopter;
          prompt = "Select a a Thopter to sacrifice";
          iVar6 = 0;
        }
        iVar6 = generic_activated_ability_with_sacrifice_as_cost(
            player, card, EVENT_ACTIVATE, GAA_LITERAL_PROMPT | GAA_UNTAPPED, iVar6, 0, 0, 0, 0, 0,
            0, NULL, prompt, TYPE_NONE, test, SARV_0);
        return iVar6;
      }
      iVar6 = generic_activated_ability(player, card, EVENT_ACTIVATE, 1, 2, 0, 0, 0, 0, 0, 0, NULL,
                                        NULL);
      return iVar6;
    }
  }
  else
  {
    if ((event != EVENT_RESOLVE_ACTIVATION) && (event != EVENT_CLEANUP))
    {
      return 0;
    }
    scnprintf(servo_buffer, 0x32, " Select a Servo to sacrifice");
    new_default_test_definition(&test_servo, 0xff, servo_buffer);
    test_servo.subtype = 0xee;
    scnprintf(thopter_buffer, 0x32, " Select a Thopter to sacrifice");
    new_default_test_definition(&test_thopter, 0xff, thopter_buffer);
    test_thopter.subtype = 0xc6;
    instance = get_card_instance(player, card);
  }
  if (event != EVENT_RESOLVE_ACTIVATION)
  {
    return 0;
  }
  iVar6 = instance->info_slot;
  if (iVar6 == 3)
  {
    iVar6 = 0x4995;
  }
  else
  {
    if (3 < iVar6)
    {
      if (iVar6 != 4)
      {
        return 0;
      }
      default_token_definition(instance->parent_controller, instance->parent_card, 0x4918, &token);
      token.tou = 4;
      token.pow = 4;
      generate_token(&token);
      return 0;
    }
    if (iVar6 == 1)
    {
      untap_card(instance->parent_controller, instance->parent_card);
      return 0;
    }
    if (iVar6 != 2)
    {
      return 0;
    }
    iVar6 = 0x4980;
  }
  generate_token_by_id_2022(instance->parent_controller, instance->parent_card, iVar6);

  return 0;
}

/* ManalinkEh 2022: 0x25663f0; original cards/commander_2018.c:2072. */
int card_yennett_cryptic_sovereign(int player, int card, event_t event)
{
  int *deck;
  unsigned int uVar1;
  int result;
  char buffer[50];

  check_legend_rule(player, card, event);
  vigilance_2022(player, card, event);
  menace(player, card, event);
  dat_ability(player, card, event, DAT_RESOLVE_TRIGGER_MANDATORY, 0);
  if (event != EVENT_DAT_ABILITY)
  {
    return 0;
  }

  deck = deck_ptr[player];
  show_deck(player, deck, 1, "Here\'s the first card of the deck", 0, ENGINE_DONE_LABEL);
  uVar1 = get_cmc_by_internal_id(*deck);
  if ((uVar1 - ((int)uVar1 >> 0x1f) & 0xfffffffe) != uVar1)
  {

    scnprintf(buffer, 0x32, " Play %s\n Pass", cards_ptr[cards_data[*deck].id]->name);
    result = do_dialog(player, player, card, -1, -1, buffer, 0);
    if (result == 0)
    {
      play_card_in_deck_for_free(player, player, 0);
      return 0;
    }
  }
  draw_a_card(player);
  return 0;
}

/* ManalinkEh 2022: 0x2566560; original cards/commander_2018.c:2108. */
int card_yuriko_the_tigers_shadow(int player, int card, event_t event)
{
  int iVar1;
  int iVar2;
  int iVar3;
  test_definition_t test;

  check_legend_rule(player, card, event);
  if (event == EVENT_DEAL_DAMAGE)
  {
    default_test_definition(&test, 2);
    test.subtype = 0x83;
    check_damage_test(player, -1, EVENT_DEAL_DAMAGE,
                      DDBM_TRACE_DAMAGED_PLAYERS | DDBM_MUST_DAMAGE_PLAYER |
                          DDBM_MUST_BE_COMBAT_DAMAGE,
                      player, card, 0, &test);
  }
  iVar1 = resolve_damage_trigger(
      player, card, event, DDBM_MUST_DAMAGE_PLAYER | DDBM_MUST_BE_COMBAT_DAMAGE, player, card);
  if (0 < iVar1)
  {

    iVar3 = 0;
    do
    {
      iVar2 = *deck_ptr[player];
      if (iVar2 != -1)
      {
        remove_card_from_deck(player, 0);
        reveal_card_iid(player, card, iVar2);
        add_card_to_hand(player, iVar2);
        iVar2 = get_cmc_by_internal_id(iVar2);
        lose_life(1 - player, iVar2);
      }
      iVar3 = iVar3 + 1;
    } while (iVar1 != iVar3);
  }
  iVar1 = ninjutsu_2022(player, card, event, 0, 1, 1, 0, 0, 0, 0x4abe);
  return iVar1;
}

/* Recovered October 2022 implementations; see docs/recovery-progress.md. */

/* ManalinkEh 2022: 0x2560e50; original cards/commander_2018.c:305. */
int card_arixmethes_slumbering_isle(int player, int card, event_t event)
{
  unsigned int uVar1;
  int result;
  card_instance_t *instance;
  unsigned char bVar4;
  int iStack_18;

  check_legend_rule(player, card, event);
  comes_into_play_tapped(player, card, event);
  enters_the_battlefield_with_counters(player, card, event, COUNTER_SLUMBER, 5);
  if ((event == EVENT_CHANGE_TYPE) && (instance = in_play(player, card), instance != NULL))
  {
    if (card != affected_card)
      goto LAB_02560ebb;
    if (player != affected_card_controller)
    {
      if (trigger_condition != 0xd3)
        goto LAB_02560ebb;
      goto LAB_02560f69;
    }
    if ((land_can_be_played & 0x20000U) == 0)
    {
      result = count_counters(player, card, COUNTER_SLUMBER);
      if (result != 0)
      {

        result = instance->internal_card_id;
        bVar4 = cards_data[result].type & 0xfd | 1;
        if (cards_data[result].type == bVar4)
        {
          return 0;
        }
        if ((instance->targets[7].player != result) ||
            (iStack_18 = instance->dummy3, cards_data[iStack_18].type != bVar4))
        {

          iStack_18 = create_a_card_type(result);
          if (iStack_18 == -1)
          {
            return 0;
          }
          cards_data[iStack_18].type = bVar4;
          instance->dummy3 = iStack_18;
          uVar1 = instance->regen_status;
          instance->targets[7].player = result;
          instance->regen_status = uVar1 | 0x1000000;
        }
        event_result = iStack_18;
      }
      goto LAB_02560eab;
    }
    if (trigger_condition != 0xd3)
      goto LAB_02560ebb;
  }
  else
  {
  LAB_02560eab:
    if ((trigger_condition != 0xd3) || (card != affected_card))
      goto LAB_02560ebb;
  LAB_02560f69:
    if (player != affected_card_controller)
      goto LAB_02560ebb;
  }
  if ((trigger_cause != card) &&
      (result = count_counters(player, card, COUNTER_SLUMBER), result != 0))
  {
    result = duh_mode(player);
    result = new_specific_spell_played(player, card, event, player, (result != 0) + 1, NULL);
    if (result != 0)
    {
      instance = in_play(player, card);
      remove_counters_from_source(player, card, player, card, COUNTER_SLUMBER, 1);
      instance->regen_status = instance->regen_status | 0x1000000;
      get_abilities(player, card, EVENT_CHANGE_TYPE, -1);
      result = is_what(player, card, 1);
      if (result == 0)
      {
        play_sound_effect(WAV_CHANGET);
      }
    }
  }
LAB_02560ebb:
  result = is_what(player, card, 1);
  if (result == 0)
  {
    result = mana_producing_creature_multi(player, card, event, 0, 0, 0, 1, 1, 0, 0);
    return result;
  }
  result = two_mana_land(player, card, event, COLOR_GREEN, COLOR_BLUE);
  return result;
}

/* Recovered October 2022 implementations; see docs/recovery-progress.md. */

/* ManalinkEh 2022: 0x2561690; original cards/commander_2018.c:467. */
int card_echo_storm(int player, int card, event_t event)
{
  card_instance_t *instance;
  int iVar2;
  int iVar3;
  token_generation_t token;
  target_definition_t td;

  if (event == EVENT_CAN_CAST)
  {
    default_target_definition(player, card, &td, 0x40);
    get_card_instance(player, card);
    iVar3 = generic_spell(player, card, EVENT_CAN_CAST, 1, &td, NULL, 1, NULL);
    return iVar3;
  }
  if (event == EVENT_CAST_SPELL)
  {
    if ((card == affected_card) && (player == affected_card_controller))
    {
      default_target_definition(player, card, &td, 0x40);
      instance = get_card_instance(player, card);
      if ((card == affected_card) && (player == affected_card_controller))
      {
        generic_spell(player, card, EVENT_CAST_SPELL, 1, &td, "TARGET_ARTIFACT", 1, NULL);
        if (spell_fizzled != 1)
        {
          iVar3 = check_special_flags_2022(player, card, 0x40000);
          if (iVar3 == 0)
          {
            iVar3 = locate_edh_card_2022(player);
            if (-1 < iVar3)
            {
              iVar3 = 0;
              while (true)
              {

                iVar2 = get_commander_storm_count(player);
                if (iVar2 <= iVar3)
                  break;
                iVar2 = new_pick_target(&td, "TARGET_ARTIFACT", 1, 0);
                if (iVar2 == 0)
                {
                  iVar3 = iVar3 + 1;
                }
                else
                {
                  iVar3 = iVar3 + 1;
                  copy_token_definition(player, card, &token, instance->targets[0].player,
                                        instance->targets[0].card);
                  generate_token(&token);
                }
              }
              instance->number_of_targets = '\x01';
            }
          }
        }
      }
    }
  }
  else if ((event == EVENT_RESOLVE_SPELL) || (event == EVENT_CAN_CHANGE_TARGET))
  {
    default_target_definition(player, card, &td, 0x40);
    instance = get_card_instance(player, card);
    if (event == EVENT_RESOLVE_SPELL)
    {
      iVar3 = valid_target(&td);
      if (iVar3 != 0)
      {
        copy_token_definition(player, card, &token, instance->targets[0].player,
                              instance->targets[0].card);
        generate_token(&token);
      }
      kill_card(player, card, KILL_DESTROY);
      return 0;
    }
  }
  else if (event == EVENT_CHANGE_TARGET)
  {
    default_target_definition(player, card, &td, 0x40);
    get_card_instance(player, card);
  }
  return 0;
}

/* ManalinkEh 2022: 0x25619a0; original cards/commander_2018.c:518. */
int card_empyrial_storm(int player, int card, event_t event)
{
  int iVar1;
  int iVar2;

  if (event == EVENT_CAN_CAST)
  {
    iVar1 = basic_spell(player, card, EVENT_CAN_CAST);
    return iVar1;
  }
  if (event == EVENT_CAST_SPELL)
  {
    if ((((card == affected_card) && (player == affected_card_controller)) &&
         (basic_spell(player, card, EVENT_CAST_SPELL), spell_fizzled != 1)) &&
        (iVar1 = check_special_flags_2022(player, card, 0x40000), iVar1 == 0))
    {
      iVar2 = locate_edh_card_2022(player);
      iVar1 = 0;
      if (-1 < iVar2)
      {
        for (; iVar2 = get_commander_storm_count(player), iVar1 < iVar2; iVar1 = iVar1 + 1)
        {
          generate_token_by_id(player, card, 0x48ff);
        }
      }
    }
  }
  else if (event == EVENT_RESOLVE_SPELL)
  {
    generate_token_by_id(player, card, 0x48ff);
    kill_card(player, card, KILL_DESTROY);
    return 0;
  }
  return 0;
}

/* ManalinkEh 2022: 0x2562780; original cards/commander_2018.c:832. */
int card_fury_storm(int player, int card, event_t event)
{
  int result;
  card_instance_t *instance;
  int iStack_14;

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
      twincast(player, card, 0x6c, NULL, NULL);
      result = counterspell_validate(player, card, NULL, 0);
      if (((result != 0) && (result = check_special_flags_2022(player, card, 0x40000), result == 0)) &&
          (result = locate_edh_card_2022(player), -1 < result))
      {

        instance = get_card_instance(player, card);
        for (iStack_14 = 0; result = get_commander_storm_count(player), iStack_14 < result;
             iStack_14 = iStack_14 + 1)
        {
          copy_spell_from_stack(player, instance->targets[0].player, instance->targets[0].card);
        }
      }
    }
    else if (event == EVENT_RESOLVE_SPELL)
    {
      result = counterspell_validate(player, card, NULL, 0);
      if (result != 0)
      {

        instance = get_card_instance(player, card);
        copy_spell_from_stack(player, instance->targets[0].player, instance->targets[0].card);
      }
      kill_card(player, card, KILL_DESTROY);
    }
    else if ((event != EVENT_CAN_CHANGE_TARGET) && (event != EVENT_CHANGE_TARGET))
    {
      return 0;
    }
  }
  result = twincast(player, card, event, NULL, NULL);
  return result;
}

/* ManalinkEh 2022: 0x2562940; original cards/commander_2018.c:863. */
int card_genesis_storm(int player, int card, event_t event)
{
  card_instance_t *instance;
  int iVar2;
  int iVar3;
  test_definition_t test;

  if (event == EVENT_CAN_CAST)
  {
    default_test_definition(&test, 1);
    test.type_flag = 1;
    get_card_instance(player, card);
    iVar2 = basic_spell(player, card, EVENT_CAN_CAST);
    return iVar2;
  }
  if (event == EVENT_CAST_SPELL)
  {
    if ((card == affected_card) && (player == affected_card_controller))
    {
      default_test_definition(&test, 1);
      test.type_flag = 1;
      instance = get_card_instance(player, card);
      if ((card == affected_card) &&
          (((player == affected_card_controller &&
             (basic_spell(player, card, EVENT_CAST_SPELL), spell_fizzled != 1)) &&
            (iVar2 = check_special_flags_2022(player, card, 0x40000), iVar2 == 0))))
      {
        iVar3 = locate_edh_card_2022(player);
        iVar2 = 0;
        if (-1 < iVar3)
        {
          for (; iVar3 = get_commander_storm_count(player), iVar2 < iVar3; iVar2 = iVar2 + 1)
          {
            reveal_card_until_you_reveal_test(player, player, &test, 0x44);
          }
          instance->number_of_targets = '\x01';
        }
      }
    }
  }
  else if ((event == EVENT_RESOLVE_SPELL) || (event == EVENT_CAN_CHANGE_TARGET))
  {
    default_test_definition(&test, 1);
    test.type_flag = 1;
    get_card_instance(player, card);
    if (event == EVENT_RESOLVE_SPELL)
    {
      reveal_card_until_you_reveal_test(player, player, &test, 0x44);
      kill_card(player, card, KILL_DESTROY);
      return 0;
    }
  }
  else if (event == EVENT_CHANGE_TARGET)
  {
    default_test_definition(&test, 1);
    test.type_flag = 1;
    get_card_instance(player, card);
  }
  return 0;
}

/* ManalinkEh 2022: 0x25654a0; original cards/commander_2018.c:1694. */
int card_skull_storm(int player, int card, event_t event)
{
  card_instance_t *instance;
  int iVar2;
  unsigned int uVar3;
  int iVar4;
  int iStack_ec;
  test_definition_t test;

  if (event == EVENT_CAN_CAST)
  {
    get_card_instance(player, card);
    iVar2 = basic_spell(player, card, EVENT_CAN_CAST);
    return iVar2;
  }
  if (event == EVENT_CAST_SPELL)
  {
    if ((card == affected_card) && (player == affected_card_controller))
    {
      instance = get_card_instance(player, card);
      default_test_definition(&test, 2);
      if ((card == affected_card) && (player == affected_card_controller))
      {
        basic_spell(player, card, EVENT_CAST_SPELL);
        if (spell_fizzled != 1)
        {
          iVar2 = check_special_flags_2022(player, card, 0x40000);
          if (iVar2 == 0)
          {
            iVar2 = locate_edh_card_2022(player);
            if (-1 < iVar2)
            {

              iVar2 = 1 - player;
              iStack_ec = 0;
              while (true)
              {
                iVar4 = get_commander_storm_count(player);
                if (iVar4 <= iStack_ec)
                  break;
                iVar4 = new_can_sacrifice(player, card, iVar2, &test);
                if (iVar4 == 0)
                {
                  uVar3 = round_up_value(life[iVar2]);
                  lose_life(iVar2, uVar3);
                }
                else
                {
                  new_sacrifice(player, card, iVar2, SAC_CAUSED | SAC_NO_CANCEL, &test);
                }
                iStack_ec = iStack_ec + 1;
              }
              instance->number_of_targets = '\x01';
            }
          }
        }
      }
    }
  }
  else if ((event == EVENT_RESOLVE_SPELL) || (event == EVENT_CAN_CHANGE_TARGET))
  {
    get_card_instance(player, card);
    default_test_definition(&test, 2);
    if (event == EVENT_RESOLVE_SPELL)
    {
      iVar4 = 1 - player;
      iVar2 = new_can_sacrifice(player, card, iVar4, &test);
      if (iVar2 == 0)
      {
        uVar3 = round_up_value(life[iVar4]);
        lose_life(iVar4, uVar3);
      }
      else
      {
        new_sacrifice(player, card, iVar4, SAC_CAUSED | SAC_NO_CANCEL, &test);
      }
      kill_card(player, card, KILL_DESTROY);
      return 0;
    }
  }
  else if (event == EVENT_CHANGE_TARGET)
  {
    get_card_instance(player, card);
    default_test_definition(&test, 2);
  }
  return 0;
}

/* Recovered October 2022 implementations; see docs/recovery-progress.md. */

/* ManalinkEh 2022: 0x2565a50; original cards/commander_2018.c:1810. */
int card_varchild_betrayer_of_kjeldor(int player, int card, event_t event)
{
  int32_t iVar1;
  int iVar2;
  card_instance_t *instance;
  unsigned char uStack_ec;

  /* DWARF locals share this frame offset; keep every typed view. */
  union
  {
    test_definition_t test;
    token_generation_t token;
    uint8_t bytes[216];
  } recovered_stack_token;

  check_legend_rule(player, card, event);
  iVar2 = whenever_i_deal_damage_to_a_player(player, card, event,
                                             DDBM_REPORT_DAMAGE_DEALT | DDBM_MUST_BE_COMBAT_DAMAGE);
  if (iVar2 != 0)
  {

    instance = get_card_instance(player, card);
    iVar1 = instance->targets[0x10].player;
    instance->targets[0x10].player = 0;
    uStack_ec = (char)((unsigned int)iVar1 >> 8) + (char)iVar1;
    default_token_definition(instance->parent_controller, instance->parent_card, 0x4994,
                             &recovered_stack_token.token);
    recovered_stack_token.token.t_player = 1 - player;
    recovered_stack_token.token.qty = uStack_ec;
    generate_token(&recovered_stack_token.token);
  }
  if (event != EVENT_ABILITIES)
  {
    leaves_play_2022(player, card, event, 1, 1);
    if (event != EVENT_LEAVES_PLAY_ABILITY)
    {
      return 0;
    }
    default_test_definition(&recovered_stack_token.test, 2);
    new_manipulate_all(player, card, 2, &recovered_stack_token.test, ACT_GAIN_CONTROL);
    return 0;
  }
  if (1 - player == affected_card_controller)
  {
    iVar2 = has_subtype(1 - player, affected_card, SUBTYPE_SURVIVOR);
    if (iVar2 != 0)
    {
      cannot_attack(affected_card_controller, affected_card, EVENT_ABILITIES);
      cannot_block(affected_card_controller, affected_card, EVENT_ABILITIES);
    }
  }
  leaves_play_2022(player, card, EVENT_ABILITIES, 1, 1);
  return 0;
}

/* Recovered October 2022 implementations; see docs/recovery-progress.md. */

/* ManalinkEh 2022: 0x25611a0; original cards/commander_2018.c:388. */
int card_brudiclad_telchor_engineer(int player, int card, event_t event)
{
  int result;
  card_instance_t *instance;
  token_generation_t token;
  target_definition_t td;
  test_definition_t test;

  check_legend_rule(player, card, event);
  result = is_humiliated_for_ability(player, card, AL_STATIC);
  if (result == 0)
  {
    instance = in_play(player, card);
    if (((event == EVENT_ABILITIES) && (instance != NULL)) && (affected_card_controller == player))
    {
      result = is_token(player, affected_card);
      if (result != 0)
      {
        haste(affected_card_controller, affected_card, EVENT_ABILITIES);
      }
    }
  }
  result = beginning_of_combat_2022(player, card, event, player);
  if (result != 0)
  {
    default_token_definition(player, card, 0x4968, &token);
    token.pow = 2;
    token.tou = 1;
    token.color_forced = '\x04';
    generate_token(&token);
    default_target_definition(player, card, &td, 0x47);
    td.allowed_controller = player;
    td.illegal_abilities = 0;
    td.special = 0x8000;
    td.extra = (int32_t)is_token_targ;
    result = can_target(&td);
    if (result != 0)
    {
      result = new_pick_target(&td, "Choose a token you control.", 0, 0x80);
      if (result != 0)
      {
        default_test_definition(&test, 0x47);
        test.special_selection_function = is_token_test;
        instance = get_card_instance(player, card);
        result = instance->targets[0].card;
        instance = get_card_instance(player, card);
        shapeshift_all(player, card, player, &test, instance->targets[0].player, result, 0);
      }
    }
  }
  return 0;
}

/* ManalinkEh 2022: 0x25639c0; original cards/commander_2018.c:1202. */
int card_loyal_apprentice(int player, int card, event_t event)
{
  int result;
  token_generation_t token;

  result = beginning_of_combat_2022(player, card, event, player);
  if (result != 0)
  {
    result = liutenant(player);
    if (result != 0)
    {
      default_token_definition(player, card, 0x4995, &token);
      token.action = TOKEN_ACTION_HASTE;
      generate_token(&token);
      return 0;
    }
  }
  return 0;
}

/* ManalinkEh 2022: 0x2563a40; original cards/commander_2018.c:1217. */
int card_loyal_drake(int player, int card, event_t event)
{
  int result;

  result = beginning_of_combat_2022(player, card, event, player);
  if (result != 0)
  {
    result = liutenant(player);
    if (result != 0)
    {
      draw_a_card(player);
      return 0;
    }
  }
  return 0;
}

/* ManalinkEh 2022: 0x2563a90; original cards/commander_2018.c:1230. */
int card_loyal_guardian(int player, int card, event_t event)
{
  int result;
  test_definition_t this_test;

  result = beginning_of_combat_2022(player, card, event, player);
  if (result != 0)
  {
    result = liutenant(player);
    if (result != 0)
    {
      default_test_definition(&this_test, 2);
      new_manipulate_all(player, card, player, &this_test, ACT_ADD_COUNTERS_BASE | ACT_KILL_BURY);
      return 0;
    }
  }
  return 0;
}

/* ManalinkEh 2022: 0x2563b20; original cards/commander_2018.c:1245. */
int card_loyal_subordinate(int player, int card, event_t event)
{
  int result;

  menace(player, card, event);
  result = beginning_of_combat_2022(player, card, event, player);
  if (result != 0)
  {
    result = liutenant(player);
    if (result != 0)
    {
      lose_life_2022(1 - player, 3);
      return 0;
    }
  }
  return 0;
}

/* ManalinkEh 2022: 0x2563ba0; original cards/commander_2018.c:1275. */
int card_loyal_unicorn(int player, int card, event_t event)
{
  int result;
  card_instance_t *instance;
  test_definition_t test;

  result = beginning_of_combat_2022(player, card, event, player);
  if (result != 0)
  {
    result = liutenant(player);
    if (result != 0)
    {

      result = create_legacy_effect(player, card, loyal_unicorn_legacy);
      alternate_legacy_text(1, player, result);
      instance = get_card_instance(player, result);
      instance->targets[0].player = player;
      instance = get_card_instance(player, result);
      instance->targets[0].card = card;
      default_test_definition(&test, 2);
      test.not_me = 1;
      pump_creatures_until_eot(player, card, player, 2, 0, 0, 0, 8, &test);
      return 0;
    }
  }
  return 0;
}

/* Recovered October 2022 implementations; see docs/recovery-progress.md. */

/* ManalinkEh 2022: 0x25650d0; original cards/commander_2018.c:1617. */
int card_saheeli_the_gifted(int player, int card, event_t event)
{
  int iVar1;
  card_instance_t *pcVar2;
  int iVar3;
  unsigned int uVar4;
  unsigned int uVar5;
  char *pcVar6;
  bool bVar7;
  unsigned char bVar8;
  card_instance_t *pcStack_1d0;
  token_generation_t token;
  char marked[151];
  test_definition_t test;

  bVar8 = 0;
  check_legend_rule(player, card, event);
  if ((event == EVENT_CAN_ACTIVATE) || (event == EVENT_ACTIVATE))
  {
    pcStack_1d0 = get_card_instance(player, card);
    default_test_definition(&test, 0x40);
    if (event == EVENT_ACTIVATE)
    {

      pcStack_1d0->number_of_targets = '\0';
      pcStack_1d0->info_slot = 0;
      test.state = 0x10;
      iVar1 = check_battlefield_for_special_card(player, card, player, 4, &test);
      iVar1 = dialog_impl(player, card, EVENT_ACTIVATE, "\001\002", "\001\017", "Create a Servo", 1,
                          3, 1, "Your next spell costs less", 1, iVar1, 1, "Copy your artifacts", 1,
                          iVar1 * 2, 0xfffffff9, 0, 0, 0, 0);
      if (iVar1 == 0)
      {
        spell_fizzled = 1;
        return 0;
      }
      goto LAB_02565141;
    }
  }
  else
  {
    if ((event != EVENT_RESOLVE_ACTIVATION) && (event != EVENT_CLEANUP))
      goto LAB_02565141;
    pcStack_1d0 = get_card_instance(player, card);
    default_test_definition(&test, 0x40);
  }
  if (event == EVENT_RESOLVE_ACTIVATION)
  {

    iVar1 = pcStack_1d0->info_slot;
    if (iVar1 == 1)
    {
      generate_token_by_id_2022(player, card, 0x4980);
    }
    else if (iVar1 == 2)
    {

      iVar1 = generate_reserved_token_by_id_2022(player, 0x6160);
      pcVar2 = get_card_instance(player, iVar1);
      pcVar2->targets[0].player = player;
      pcVar2->targets[0].card = 0x41;
      iVar3 = get_id(pcStack_1d0->parent_controller, pcStack_1d0->parent_card);
      pcVar2->targets[1].card = iVar3;
      put_into_play(player, iVar1);
    }
    else if (iVar1 == 3)
    {

      pcVar6 = marked;
      uVar5 = 0x97;
      bVar7 = ((unsigned int)pcVar6 & 1) != 0;
      if (bVar7)
      {
        marked[0] = '\0';
        pcVar6 = marked + 1;
        uVar5 = 0x96;
      }
      if (((unsigned int)pcVar6 & 2) != 0)
      {
        pcVar6[0] = '\0';
        pcVar6[1] = '\0';
        uVar5 = uVar5 - 2;
        pcVar6 = pcVar6 + 2;
      }
      for (uVar4 = uVar5 >> 2; uVar4 != 0; uVar4 = uVar4 - 1)
      {
        pcVar6[0] = '\0';
        pcVar6[1] = '\0';
        pcVar6[2] = '\0';
        pcVar6[3] = '\0';
        pcVar6 = pcVar6 + (unsigned int)bVar8 * -8 + 4;
      }
      if ((uVar5 & 2) != 0)
      {
        pcVar6[0] = '\0';
        pcVar6[1] = '\0';
        pcVar6 = pcVar6 + 2;
      }
      if (!bVar7)
      {
        *pcVar6 = '\0';
      }
      iVar1 = 0;
      if (0 < active_cards_count[player])
      {
        do
        {
          pcVar2 = in_play(player, iVar1);
          if ((pcVar2 != NULL) && (iVar3 = is_what(player, iVar1, 0x40), iVar3 != 0))
          {
            marked[iVar1] = '\x01';
          }
          iVar1 = iVar1 + 1;
        } while (iVar1 < active_cards_count[player]);
        if (0 < active_cards_count[player])
        {
          iVar1 = 0;
          do
          {
            if (marked[iVar1] != '\0')
            {
              copy_token_definition(pcStack_1d0->parent_controller, pcStack_1d0->parent_card,
                                    &token, player, iVar1);
              token.legacy = '\x01';
              token.special_code_for_legacy = haste_and_remove_eot;
              generate_token(&token);
            }
            iVar1 = iVar1 + 1;
          } while (iVar1 < active_cards_count[player]);
        }
      }
    }
  }
LAB_02565141:
  iVar1 = planeswalker(player, card, event, 4);
  return iVar1;
}

/* Recovered October 2022 implementations; see docs/recovery-progress.md. */

/* ManalinkEh 2022: 0x25632a0; original cards/commander_2018.c:1066. */
int card_lord_windgrace(int player, int card, event_t event)
{
  uint8_t uVar1;
  card_instance_t *instance;
  unsigned int uVar2;
  int iVar3;
  int iVar4;
  int *piVar5;
  target_t *ptVar6;
  target_t *ptVar7;
  char *pcVar8;
  unsigned char bVar9;
  inst_info_t iVar10;
  char msg[50];
  int dead[10];
  target_definition_t td;
  test_definition_t this_test;
  test_definition_t land_test;

  bVar9 = 0;
  check_legend_rule(player, card, event);
  if ((event == EVENT_CAN_ACTIVATE) || (event == EVENT_ACTIVATE))
  {
    instance = get_card_instance(player, card);
    default_test_definition(&this_test, 0xff);
    iVar4 = 7;
    this_test.zone = 0x100;
    strncpy(msg, "Select a land card.", 0x14);
    msg[0x14] = '\0';
    msg[0x15] = '\0';
    pcVar8 = msg + 0x16;
    for (; iVar4 != 0; iVar4 = iVar4 + -1)
    {
      pcVar8[0] = '\0';
      pcVar8[1] = '\0';
      pcVar8[2] = '\0';
      pcVar8[3] = '\0';
      pcVar8 = pcVar8 + (unsigned int)bVar9 * -8 + 4;
    }
    new_default_test_definition(&land_test, 1, msg);
    default_target_definition(player, card, &td, 0xff);
    td.illegal_type = 1;
    td.allow_cancel = 3;
    if (event == EVENT_ACTIVATE)
    {

      instance->number_of_targets = '\0';
      instance->mode = '\0';
      clear_info(instance, 0x4a9d);
      iVar3 = count_graveyard_by_type(player, 1);
      iVar4 = 0;
      if (0 < iVar3)
      {
        iVar4 = (unsigned int)(1 < iVar3) * 5 + 5;
      }
      iVar4 = dialog_impl(player, card, EVENT_ACTIVATE, "\001\002", "\001\017", "\001\033",
                          "Discard and Draw", (unsigned int)(0 < hand_count[player]), 1, 2,
                          "Reanimate lands", 1, iVar4, 0xfffffffd, "Destroy and make cats", 1, 0xf,
                          0xfffffff5, 0, 0, 0, 0);
      if (iVar4 == 0)
      {
        spell_fizzled = 1;
        return 0;
      }
      if (iVar4 == 2)
      {

        instance->number_of_targets = '\0';
        iVar4 = select_multiple_cards_from_graveyard(player, player, 0, 1, &land_test, 2,
                                                     instance->targets);
        set_inst_info(instance, 0x4a9d, 0, (uint16_t)iVar4);
      }
      else if (iVar4 == 3)
      {

        iVar3 = 0;
        iVar4 = can_target(&td);
        do
        {
          if ((iVar4 == 0) ||
              (iVar4 = new_pick_target(&td, "TARGET_NONLAND_PERMANENT", iVar3, 0), iVar4 == 0))
            break;
          ptVar7 = instance->targets + iVar3;
          ptVar6 = instance->targets + iVar3;
          iVar3 = iVar3 + 1;
          state_untargettable(ptVar6->player, ptVar7->card, 1);
          iVar4 = can_target(&td);
        } while (iVar3 < 6);
        if (0 < iVar3)
        {
          ptVar6 = instance->targets;
          do
          {
            ptVar7 = ptVar6 + 1;
            state_untargettable(ptVar6->player, ptVar6->card, 0);
            ptVar6 = ptVar7;
          } while (ptVar7 != instance->targets + iVar3);
        }
      }
      goto LAB_02563488;
    }
  }
  else
  {
    if ((event != EVENT_RESOLVE_ACTIVATION) && (event != EVENT_CLEANUP))
      goto LAB_02563488;
    instance = get_card_instance(player, card);
    default_test_definition(&this_test, 0xff);
    iVar4 = 7;
    this_test.zone = 0x100;
    strncpy(msg, "Select a land card.", 0x14);
    msg[0x14] = '\0';
    msg[0x15] = '\0';
    pcVar8 = msg + 0x16;
    for (; iVar4 != 0; iVar4 = iVar4 + -1)
    {
      pcVar8[0] = '\0';
      pcVar8[1] = '\0';
      pcVar8[2] = '\0';
      pcVar8[3] = '\0';
      pcVar8 = pcVar8 + (unsigned int)bVar9 * -8 + 4;
    }
    new_default_test_definition(&land_test, 1, msg);
    default_target_definition(player, card, &td, 0xff);
  }
  td.allow_cancel = 3;
  td.illegal_type = 1;
  if (event == EVENT_RESOLVE_ACTIVATION)
  {

    uVar1 = instance->mode;
    if (uVar1 == '\x01')
    {
      if (0 < hand_count[player])
      {

        if (player == 1)
        {
          this_test.type = 1;
        }
        iVar4 = new_select_a_card(player, player, 4, 1, 3, -1, &this_test);
        iVar3 = is_what(player, iVar4, 1);
        discard_card(player, iVar4);
        iVar4 = 1;
        if (iVar3 != 0)
        {
          iVar4 = 2;
        }
        draw_cards(player, iVar4);
      }
    }
    else
    {
      if (uVar1 == '\x02')
      {

        iVar10 = inst_info(instance, 0x4a9d);
        uVar2 = ((uint32_t)iVar10.id | ((uint32_t)iVar10.data2 << 16)) >> 0x10;
        if (uVar2 != 0)
        {
          iVar3 = graveyard_has_shroud(player);
          iVar4 = 0;
          if (iVar3 == 0)
          {
            do
            {
              if ((iVar4 < (int)uVar2) &&
                  (iVar3 = validate_target_from_grave(player, card, player, iVar4), iVar3 != -1))
              {
                dead[iVar4] = graveyard_ptr_mutable[player][iVar3];
                remove_card_from_grave(player, iVar3);
              }
              else
              {
                dead[iVar4] = -1;
              }
              iVar4 = iVar4 + 1;
            } while (iVar4 != 2);
            piVar5 = dead;
            do
            {
              if (*piVar5 != -1)
              {

                iVar4 = raw_put_iid_on_top_of_graveyard(player, *piVar5);
                increase_trap_condition(player, 0x202, -1);
                reanimate_permanent(player, card, player, iVar4, REANIMATE_DEFAULT);
              }
              piVar5 = piVar5 + 1;
            } while (piVar5 != dead + 2);
            instance->targets[9].card = -1;
            goto LAB_02563488;
          }
        }
        return 0;
      }
      if (uVar1 == '\x03')
      {

        if (instance->number_of_targets != '\0')
        {
          iVar4 = 0;
          do
          {
            ptVar7 = instance->targets + iVar4;
            ptVar6 = instance->targets + iVar4;
            iVar4 = iVar4 + 1;
            kill_card(ptVar6->player, ptVar7->card, KILL_DESTROY);
          } while (iVar4 < (int)(unsigned int)instance->number_of_targets);
        }
        default_token_definition(player, card, 0x4910, (token_generation_t *)dead);
        dead[8] = get_hacked_walk(player, card, 4);
        generate_token((token_generation_t *)dead);
      }
    }
  }
LAB_02563488:
  iVar4 = planeswalker(player, card, event, 5);
  return iVar4;
}

/* Recovered October 2022 implementations; see docs/recovery-progress.md. */
static int vedalken_humiliation(int player, int card, event_t event);

/* ManalinkEh 2022: 0x255f990; original cards/commander_2018.c:1903. */
static int vedalken_humiliation(int player, int card, event_t event)
{
  card_instance_t *instance;
  int result;

  if ((event - EVENT_POWER < 3) || (event == EVENT_CAST_SPELL))
  {
    instance = in_play(player, card);
    if (instance == NULL)
    {
      return 0;
    }
    instance = in_play(affected_card_controller, affected_card);
    if (instance == NULL)
    {
      return 0;
    }
    if (affected_card_controller == player)
    {
      return 0;
    }
    result = is_what(affected_card_controller, affected_card, 2);
    if (result == 0)
    {
      return 0;
    }
    if (event - EVENT_POWER < 2)
    {
      set_base_pt(1);
      return 0;
    }
    if ((event == EVENT_ABILITIES) || (event == EVENT_CAST_SPELL))
    {

      event_result = event_result | 0x1f000000;
      instance = get_card_instance(affected_card_controller, affected_card);
      event_result = event_result & 0x9f080000;
      instance->state = instance->state & 0xffffdfff;
      instance->ext->sp_keywords = instance->ext->sp_keywords & 0x993c00c0;
      humiliate_2022(player, card, affected_card_controller, affected_card, 1);
    }
  }
  if (event != EVENT_CLEANUP)
  {
    return 0;
  }
  kill_card(player, card, KILL_EXILE);
  return 0;
}

/* ManalinkEh 2022: 0x2565fd0; original cards/commander_2018.c:1931. */
int card_vedalken_humiliator(int player, int card, event_t event)
{
  int result;

  dat_ability(player, card, event, DAT_RESOLVE_TRIGGER_MANDATORY, 0);
  if (event == EVENT_DAT_ABILITY)
  {
    result = metalcraft(player, card);
    if (result != 0)
    {
      create_legacy_effect(player, card, vedalken_humiliation);
    }
  }
  return 0;
}


/* ManalinkEh 2022: 0x2565c60; original cards/commander_2018.c:1845. */
int card_varina_lich_queen(int player, int card, event_t event)
{
  card_instance_t *instance;
  int result;

  char msg[100];

  /* DWARF locals share this frame offset; keep every typed view. */
  union
  {
    test_definition_t test;
    token_generation_2022_t token;
    uint8_t bytes[216];
  } recovered_stack_test;

  check_legend_rule(player, card, event);
  if (event == EVENT_DECLARE_ATTACKERS)
  {
    default_test_definition(&recovered_stack_test.test, 2);
    recovered_stack_test.test.subtype = 0xde;
    store_attackers(player, card, EVENT_DECLARE_ATTACKERS, 0, player, -1,
                    &recovered_stack_test.test);
    dat_ability(player, card, EVENT_DECLARE_ATTACKERS, DAT_RESOLVE_TRIGGER_MANDATORY, 1);
  }
  else
  {
    dat_ability(player, card, event, DAT_RESOLVE_TRIGGER_MANDATORY, 1);
    if (event == EVENT_DAT_ABILITY)
    {
      instance = get_card_instance(player, card);
      result = instance->info_slot;
      draw_cards(player, result);
      new_multidiscard(player, result, 0, player);
      gain_life(player, result);
      return 0;
    }
    if (event == EVENT_CAN_ACTIVATE)
    {
      result = count_graveyard(player);
      if (result < 2)
      {
        return 0;
      }
      result = generic_activated_ability(player, card, EVENT_CAN_ACTIVATE, 0, 2, 0, 0, 0, 0, 0, 0,
                                         NULL, NULL);
      return result;
    }
    if (event == EVENT_ACTIVATE)
    {
      generic_activated_ability(player, card, EVENT_ACTIVATE, 0, 2, 0, 0, 0, 0, 0, 0, NULL, NULL);
      if (spell_fizzled == 1)
      {
        return 0;
      }
      memcpy(msg, "Select a card to exile.", 24);
      memset(msg + 24, 0, 76);
      new_default_test_definition(&recovered_stack_test.test, 0xff, msg);
      result = new_global_tutor(player, player, 2, 6, 0, 3, &recovered_stack_test.test);
      if (result < 0)
      {
        spell_fizzled = 1;
        return 0;
      }
      new_global_tutor(player, player, 2, 6, 1, 3, &recovered_stack_test.test);
      return 0;
    }
  }
  if (event != EVENT_RESOLVE_ACTIVATION)
  {
    return 0;
  }
  instance = get_card_instance(player, card);
  default_token_definition_2022(instance->parent_controller, instance->parent_card, 0x49ac,
                                &recovered_stack_test.token);
  recovered_stack_test.token.action = TOKEN2022_ACTION_TAPPED;
  generate_token_2022(&recovered_stack_test.token);
  return 0;
}


#ifndef SHANDALAR
/* Recovered October 2022 typed consumers and compiler parts. */

/* ManalinkEh 2022: 0x255fd50; original cards/commander_2018.c:3. */
static int amaug_leg_part_2022(int player, int card, event_t event)
{
  card_instance_t *instance;
  int iVar2;
  type_t tVar3;
  type_t tVar4;
  unsigned int uVar5;
  uint32_t uVar6;
  int iVar7;
  unsigned int uVar8;
  int iVar9;
  int32_t *piVar10;
  unsigned int uVar11;
  type_t tStack_140;
  type_t tStack_13c;
  type_t tStack_138;
  int iStack_134;
  int playable[2][8];
  test_definition_2022_t this_test;

  instance = get_card_instance(player, card);
  if (event == EVENT_CAN_ACTIVATE)
  {
    tStack_138 = instance->targets[0].player;
    if (tStack_138 == ~TYPE_NONE)
    {
      instance->targets[0].player = 0;
      tStack_138 = TYPE_NONE;
    }
    piVar10 = &instance->targets[0].card;
    iStack_134 = 0;
    do
    {
      iVar9 = *piVar10;
      if (iVar9 != -1)
      {
        tVar4 = get_type_with_iid(-1, -1, iVar9);
        tVar3 = tVar4 & (TARGET_TYPE_PLANESWALKER | TYPE_ARTIFACT | TYPE_SPELL | TYPE_ENCHANTMENT |
                         TYPE_CREATURE);
        if ((tVar4 & TYPE_INTERRUPT) == TYPE_NONE)
        {
          iVar7 = is_tribal(-1, iVar9);
          if (iVar7 != 0)
            goto LAB_0255ff16;
          if (tVar3 != TYPE_NONE)
            goto LAB_0255ff1c;
        }
        else
        {
          iVar7 = is_tribal(-1, iVar9);
          tVar3 = tVar3 | TYPE_INSTANT;
          if (iVar7 != 0)
          {
          LAB_0255ff16:
            tVar3 = tVar3 | TYPE_EFFECT;
          }
        LAB_0255ff1c:
          if (((tVar3 | tStack_138) != tStack_138) &&
              (iVar9 = can_legally_play_iid_now(player, iVar9, EVENT_CAN_ACTIVATE), iVar9 != 0))
          {
            if (iVar9 == 99)
            {
              return 99;
            }
            iStack_134 = 1;
          }
        }
      }
      piVar10 = piVar10 + 2;
      if (&instance->targets[7].card == piVar10)
      {
        return iStack_134;
      }
    } while (true);
  }
  if (event != EVENT_ACTIVATE)
  {
    return 0;
  }
  tVar4 = instance->targets[0].player;
  tStack_138 = 0;
  iVar9 = 0;
  do
  {
    iVar7 = instance->targets[iVar9].card;
    if ((iVar7 != -1) &&
        (iVar2 = check_rfg(player, (unsigned int)cards_data[iVar7].id), iVar2 != 0))
    {
      tVar3 = get_type_with_iid(-1, -1, iVar7);
      tStack_13c = tVar3 & (TARGET_TYPE_PLANESWALKER | TYPE_ARTIFACT | TYPE_SPELL |
                            TYPE_ENCHANTMENT | TYPE_CREATURE);
      if ((tVar3 & TYPE_INTERRUPT) == TYPE_NONE)
      {
        iVar2 = is_tribal(-1, iVar7);
        if (iVar2 != 0)
          goto LAB_0255fe43;
        if (tStack_13c != TYPE_NONE)
          goto LAB_0255fe46;
      }
      else
      {
        tStack_13c = tStack_13c | TYPE_INSTANT;
        iVar2 = is_tribal(-1, iVar7);
        if (iVar2 != 0)
        {
        LAB_0255fe43:
          tStack_13c = tStack_13c | TYPE_EFFECT;
        }
      LAB_0255fe46:
        if ((tVar4 != (tStack_13c | tVar4)) &&
            (iVar2 = can_legally_play_iid_now(player, iVar7, EVENT_ACTIVATE), iVar2 != 0))
        {
          playable[0][tStack_138] = iVar7;
          playable[1][tStack_138] = iVar9;
          tStack_138 = tStack_138 + 1;
        }
      }
    }
    iVar9 = iVar9 + 1;
  } while (iVar9 != 7);
  new_default_test_definition_2022(&this_test, 0xff, "Select a card to cast.");
  iVar9 = select_card_from_zone_2022(player, player, playable[0], tStack_138, 0, 1, -1, &this_test);
  if (iVar9 == -1)
  {
  LAB_02560240:
    spell_fizzled = 1;
  }
  else
  {
    tVar3 = get_type_with_iid(-1, -1, playable[0][iVar9]);
    uVar5 = num_bits_set(tVar3);
    if (uVar5 < 2)
    {
      iVar7 = is_tribal(-1, playable[0][iVar9]);
      if ((iVar7 != 0) && ((tVar4 & TYPE_EFFECT) == TYPE_NONE))
      {
        if ((tVar3 & TYPE_EFFECT) == TYPE_NONE)
          goto LAB_02560057;
        goto LAB_02560298;
      }
      instance->targets[0].player = instance->targets[0].player | tVar3;
    }
    else
    {
      if (((tVar3 & TYPE_EFFECT) == TYPE_NONE) || ((tVar4 & TYPE_EFFECT) != TYPE_NONE))
      {
      LAB_02560057:
        uVar6 = 0;
      }
      else
      {
      LAB_02560298:
        uVar6 = 1;
      }
      uVar5 = 0;
      if ((tVar3 & TYPE_SORCERY) != TYPE_NONE)
      {
        uVar5 = (unsigned int)((tVar4 & TYPE_SORCERY) == TYPE_NONE);
      }
      tStack_138 = tVar3 & TARGET_TYPE_PLANESWALKER;
      if (tStack_138 != TYPE_NONE)
      {
        tStack_138 = (type_t)((tVar4 & TARGET_TYPE_PLANESWALKER) == TYPE_NONE);
      }
      tStack_13c = tVar3 & TYPE_INSTANT;
      if (tStack_13c != TYPE_NONE)
      {
        tStack_13c = (type_t)((tVar4 & TYPE_INSTANT) == TYPE_NONE);
      }
      tStack_140 = tVar3 & TYPE_ENCHANTMENT;
      if (tStack_140 != TYPE_NONE)
      {
        tStack_140 = (type_t)((tVar4 & TYPE_ENCHANTMENT) == TYPE_NONE);
      }
      uVar8 = 0;
      if ((tVar3 & TYPE_CREATURE) != TYPE_NONE)
      {
        uVar8 = (unsigned int)((tVar4 & TYPE_CREATURE) == TYPE_NONE);
      }
      uVar11 = 0;
      if ((tVar3 & TYPE_ARTIFACT) != TYPE_NONE)
      {
        uVar11 = (unsigned int)((tVar4 & TYPE_ARTIFACT) == TYPE_NONE);
      }
      iVar7 = dialog_impl(player, card, EVENT_ACTIVATE, "\001\002", "\001\014", "\001\003",
                          "\001\004", "\001\016", "Artifact", uVar11, 1, "Creature", uVar8, 1,
                          "Enchantment", tStack_140, 1, "Instant", tStack_13c, 1, "Planeswalker",
                          tStack_138, 1, "Sorcery", uVar5, 1, "Tribal", uVar6, 1, 0, 0, 0, 0);
      switch (iVar7)
      {
      default:
        goto LAB_02560240;
      case 1:
        instance->targets[0].player = instance->targets[0].player | 0x40;
        break;
      case 2:
        instance->targets[0].player = instance->targets[0].player | 2;
        break;
      case 3:
        instance->targets[0].player = instance->targets[0].player | 4;
        break;
      case 4:
        instance->targets[0].player = instance->targets[0].player | 0x10;
        break;
      case 5:
        instance->targets[0].player = instance->targets[0].player | 0x100;
        break;
      case 6:
        instance->targets[0].player = instance->targets[0].player | 8;
        break;
      case 7:
        instance->targets[0].player = instance->targets[0].player | 0x80;
      }
    }
    play_card_in_exile_for_free(player, player, (unsigned int)cards_data[playable[0][iVar9]].id);
    cant_be_responded_to = 1;
  }
  return 0;
}

#endif

#ifndef SHANDALAR
/* Recovered October 2022 implementations; see docs/recovery-progress.md. */
static int is_enchanted_or_enchantment_creature(int unused1, int unused2, int player, int card);
static char *is_aura_or_equipment(int who_chooses, int player, int card);

/* ManalinkEh 2022: 0x255f810; original cards/commander_2018.c:978. */
static char *is_aura_or_equipment(int who_chooses, int player, int card)
{
  int result;

  result = has_subtype(player, card, SUBTYPE_AURA);
  if (result == 0)
  {
    result = has_subtype(player, card, SUBTYPE_EQUIPMENT);
    if (result == 0)
    {
      return "must be an aura or equipment.";
    }
  }
  return NULL;
}

/* ManalinkEh 2022: 0x2562ee0; original cards/commander_2018.c:985. */
int card_heavenly_blademaster(int player, int card, event_t event)
{
  target_t *ptVar1;
  target_t *ptVar2;
  int iVar3;
  card_instance_t *pcVar4;
  card_instance_t *pcVar5;
  int iVar6;
  target_definition_2022_t td;

  iVar3 = duh_mode(player);
  etb_ability_2022(player, card, event, (iVar3 != 0) + RESOLVE_TRIGGER_OPTIONAL);
  pcVar4 = get_card_instance(player, card);
  if (event != EVENT_ETB)
  {
    if (event - EVENT_POWER < 2)
    {
      pcVar5 = in_play(player, card);
      if (((pcVar5 != NULL) && (affected_card_controller == player)) && (affected_card != card))
      {
        iVar3 = is_humiliated_for_ability(pcVar4->targets[0].player, pcVar4->targets[0].card,
                                          AL_STATIC);
        if (iVar3 == 0)
        {
          iVar3 = count_auras_enchanting_me(player, card);
          iVar6 = equipments_attached_to_me(player, card, 1);
          event_result = event_result + iVar3 + iVar6;
        }
      }
    }
    return 0;
  }

  pcVar4->number_of_targets = '\0';
  default_target_definition_2022(player, card, &td, 0x47);
  td.allowed_controller = player;
  td.illegal_abilities = 0;
  td.special = 0x8000;
  td.extra = (int32_t)is_aura_or_equipment;
  td.allow_cancel = 2;
  iVar3 = 0;
  do
  {
    iVar6 = can_target_2022(&td);
    if (iVar6 == 0)
      break;
    iVar6 = new_pick_target_2022(&td, "Select target Aura or Equipment you control", iVar3, 0x80);
    if (iVar6 == 0)
      break;
    ptVar2 = pcVar4->targets + iVar3;
    ptVar1 = pcVar4->targets + iVar3;
    iVar3 = iVar3 + 1;
    state_untargettable(ptVar1->player, ptVar2->card, 1);
  } while (iVar3 != 10);
  iVar3 = 0;
  if (pcVar4->number_of_targets != '\0')
  {
    do
    {
      state_untargettable(pcVar4->targets[iVar3].player, pcVar4->targets[iVar3].card, 0);
      iVar6 = has_subtype(pcVar4->targets[iVar3].player, pcVar4->targets[iVar3].card, SUBTYPE_AURA);
      if (iVar6 == 0)
      {
        iVar6 = has_subtype(pcVar4->targets[iVar3].player, pcVar4->targets[iVar3].card,
                            SUBTYPE_EQUIPMENT);
        if (iVar6 != 0)
        {
          equip_target_creature(pcVar4->targets[iVar3].player, pcVar4->targets[iVar3].card, player,
                                card);
        }
      }
      else
      {
        attach_aura_to_target_2022(pcVar4->targets[iVar3].player, pcVar4->targets[iVar3].card,
                                   EVENT_ETB, player, card);
      }
      iVar3 = iVar3 + 1;
    } while (iVar3 < (int)(unsigned int)pcVar4->number_of_targets);
  }
  pcVar4->number_of_targets = '\0';
  return 0;
}

/* ManalinkEh 2022: 0x255f7c0; original cards/commander_2018.c:1033. */
static int is_enchanted_or_enchantment_creature(int unused1, int unused2, int player, int card)
{
  int result;
  unsigned int uVar2;

  result = is_enchanted(player, card);
  uVar2 = 1;
  if (result == 0)
  {
    result = is_what(player, card, 4);
    uVar2 = (unsigned int)(result != 0);
  }
  return uVar2;
}

/* ManalinkEh 2022: 0x2563140; original cards/commander_2018.c:1041. */
int card_kestia_the_cultivator(int player, int card, event_t event)
{
  int result;
  test_definition_2022_t test;

  check_legend_rule(player, card, event);
  if (event == EVENT_DECLARE_ATTACKERS)
  {
    default_test_definition_2022(&test, 2);
    test.special_selection_function = is_enchanted_or_enchantment_creature;
    store_attackers_2022(player, card, EVENT_DECLARE_ATTACKERS, DAT_SEPARATE_TRIGGERS, player, -1,
                         &test);
    dat_ability(player, card, EVENT_DECLARE_ATTACKERS,
                DAT_SEPARATE_TRIGGERS | DAT_RESOLVE_TRIGGER_MANDATORY, 1);
  }
  else
  {
    dat_ability(player, card, event, DAT_SEPARATE_TRIGGERS | DAT_RESOLVE_TRIGGER_MANDATORY, 1);
    if (event == EVENT_DAT_ABILITY)
    {
      draw_cards(player, 1);
    }
  }
  result = new_generic_creature_with_bestow(player, card, event, 3, 0, 1, 1, 0, 1, 4, 4, 0, 0);
  return result;
}

#endif


#ifndef SHANDALAR
/* ManalinkEh 2022: 0x2561cc0; original cards/commander_2018.c:580. */
int card_entreat_the_dead(int player, int card, event_t event)
{
  card_instance_t *pcVar1;
  int iVar2;
  card_instance_t *pcVar3;
  unsigned int uVar4;
  unsigned int uVar5;
  int32_t value;
  int iVar6;

  unsigned int uStack_154;
  char buf[100];
  test_definition_2022_t this_test;

  new_default_test_definition_2022(&this_test, 2, "Select a creature card.");
  pcVar1 = get_card_instance(player, card);
  if (event == EVENT_CAN_CAST)
  {
    iVar6 = generic_spell_2022(player, card, EVENT_CAN_CAST, 0xc, NULL, NULL, 1, &this_test);
    return iVar6;
  }
  if (event != EVENT_CAST_SPELL)
  {
    if (event != EVENT_RESOLVE_SPELL)
    {
      return 0;
    }
    if (0 < pcVar1->info_slot)
    {
      iVar6 = 0;
      do
      {
        iVar2 = validate_target_from_grave(player, card, player, iVar6);
        if (iVar2 != -1)
        {
          reanimate_permanent_2022(player, card, player, iVar2, REANIMATE2022_DEFAULT);
        }
        iVar6 = iVar6 + 1;
      } while (iVar6 < pcVar1->info_slot);
    }
    kill_card(player, card, KILL_DESTROY);
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
  iVar6 = check_special_flags2_2022(player, card, -0x80000000);
  if (iVar6 != 0)
  {
    return 0;
  }
  pcVar3 = get_card_instance(player, card);
  pcVar3->info_slot = 0;
  iVar6 = check_special_flags2_2022(player, card, 0x100000);
  if (iVar6 == 0)
  {
    iVar6 = played_for_free(player, card);
    if (iVar6 != 0)
    {
      return 0;
    }
    iVar6 = is_token(player, card);
    if (iVar6 != 0)
    {
      return 0;
    }
    uVar4 = count_graveyard_by_type(player, 2);
    uVar5 = has_mana(player, 7, 1);
    uVar5 = round_down_value(uVar5);
    if ((int)uVar4 < (int)uVar5)
    {
      uVar5 = uVar4;
    }
    if ((0 < (int)uVar5) && (iVar6 = graveyard_has_shroud(player), iVar6 == 0))
    {
      uStack_154 = uVar5;
      if (0x13 < (int)uVar5)
      {
        uStack_154 = 0x13;
      }
      if (ai_is_speculating == 1)
      {
        buf[0] = '\0';
      }
      else if (uVar5 == 1)
      {
        memcpy(buf, "Select a creature card.", 24);
      }
      else
      {
        sprintf(buf, "Select up to %d creature cards.", uStack_154);
      }
      new_default_test_definition_2022(&this_test, 2, buf);
      iVar6 = select_multiple_cards_from_graveyard_2022(player, player, 0, 1, &this_test,
                                                        uStack_154, pcVar1->targets);
      pcVar1->info_slot = iVar6;
      if (iVar6 < 1)
      {
        return 0;
      }
      iVar6 = charge_mana_for_double_x(player, 0);
      value = iVar6 / 2;
    LAB_02561f41:
      set_x_for_x_spells(player, card, EVENT_CAST_SPELL, value);
      return 0;
    }
  }
  else
  {
    iVar6 = count_graveyard_by_type(player, 2);
    iVar2 = has_mana(player, 7, 1);
    if (iVar2 < iVar6)
    {
      iVar6 = iVar2;
    }
    if ((0 < iVar6) && (iVar2 = graveyard_has_shroud(player), iVar2 == 0))
    {
      uStack_154 = iVar6;
      if (0x13 < iVar6)
      {
        uStack_154 = 0x13;
      }
      if (ai_is_speculating == 1)
      {
        buf[0] = '\0';
      }
      else if (iVar6 == 1)
      {
        memcpy(buf, "Select a creature card.", 24);
      }
      else
      {
        sprintf(buf, "Select up to %d creature cards.", uStack_154);
      }
      new_default_test_definition_2022(&this_test, 2, buf);
      iVar6 = select_multiple_cards_from_graveyard_2022(player, player, 0, 1, &this_test,
                                                        uStack_154, pcVar1->targets);
      pcVar1->info_slot = iVar6;
      if (0 < iVar6)
      {
        charge_mana(player, COLOR_COLORLESS, iVar6);
      }
      if (spell_fizzled == 1)
      {
        return 0;
      }
      value = pcVar1->info_slot;
      goto LAB_02561f41;
    }
  }
  ai_modifier = ai_modifier + -0x30;
  pcVar1->info_slot = 0;
  return 0;
}

#endif


#ifndef SHANDALAR
/* ManalinkEh 2022: 0x2565940; original cards/commander_2018.c:1789. */
int card_tuvasa_the_sunlit(int player, int card, event_t event)
{
  int result;
  check_legend_rule(player, card, event);
  if (((trigger_condition == 0xd3) && (card == affected_card)) &&
      (player == affected_card_controller))
  {
    result = get_stormenchantment_count_2022(player);
    if (result == 1)
    {
      sp_ability_trigger_2022(player, event, 2, TYPE_ENCHANTMENT, NULL, player, card);
    }
  }
  if (event != EVENT_SPA)
  {
    if (((event - EVENT_POWER < 2) && (card == affected_card)) &&
        (player == affected_card_controller))
    {
      result = is_humiliated_for_ability(player, card, AL_STATIC);
      if (result == 0)
      {
        result = count_subtype(player, 4, -1);
        event_result = event_result + result;
      }
    }
    return 0;
  }
  draw_a_card(player);
  return 0;
}

#endif
