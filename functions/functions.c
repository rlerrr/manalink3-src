#include "manalink.h"

int can_pay_life_impl(int player, int amount, int flags);

/* Canonical recovered helper declarations. */
static card_instance_t *find_obliviation_legacy(int player, int card, int *out_player,
                                                int *out_card, int *out_iid, int *out_owner);
int is_what(int player, int card, int test_type);
int is_token(int player, int card);
int get_special_flags(int player, int card, int type_of_special_flag);
int check_special_flags(int player, int card, int value);
int check_special_flags2(int player, int card, int value);
int check_special_flags3(int player, int card, int value);
void set_special_flags(int player, int card, int value);
void set_special_flags2(int player, int card, int value);
void set_special_flags3(int player, int card, int value);
void remove_special_flags(int player, int card, int value);
void remove_special_flags2(int player, int card, int value);
void remove_special_flags3(int player, int card, int value);
int get_special_flags_by_instance(card_instance_t *instance);
int get_special_flags2_by_instance(card_instance_t *instance);
int get_special_flags3_by_instance(card_instance_t *instance);
int pump_ability_until_eot(int player, int card, int t_player, int t_card, int power,
                                int toughness, int ability, int sp_ability);
int resolve_gfp_ability(int player, int card, event_t event, resolve_trigger_t trig_mode);
void gain_life(int player, int amount);
int has_mana_multi_a(int player, int colorless, int black, int blue, int green, int red,
                          int white);
int has_mana_multi(int player, int colorless, int black, int blue, int green, int red,
                        int white);
int get_global_color_hack(int player);
int get_color(int player, int card);
int get_color_by_internal_id_no_hack(int internal_id);
int get_color_by_internal_id(int player, int internal_id);
int get_color_real_or_iid(int player_or_owner, int player_or_negone, int card_or_iid);
void change_color(int player, int card, int t_player, int t_card, color_test_t new_color,
                       change_color_t mode);
int get_hacked_walk(int player, int card, int orig_walk);
int keyword_to_color(int keyword);
int get_colors_to_hack_from_csvid(int csvid);
int is_colorless(int player, int card);
int colors_shared(int clr1, int clr2);
int has_my_colors(int player, int card, int t_player, int t_card);
int real_count_colors(int clr);
int count_colors(int player, int card);
int evaluate_colors(int color1, int color2);
int is_monocolor_hybrid(card_ptr_t *cp);
int get_deck_color(int player, int whose_deck);
int choose_a_color(int player, int ai_choice);
int choose_a_color_from(int player, int ai_choice, color_test_t available_colors);
int select_a_color(int player);
int is_planeswalker(int player, int card);
type_t get_type_with_iid(int player, int card, int iid);
type_t get_type(int player, int card);
int get_base_power(int player, int card);
int get_base_power_iid(int owner, int iid);
int get_base_toughness(int player, int card);
int get_base_toughness_iid(int owner, int iid);
int get_cmc_by_internal_id(int iid);
int get_cmc(int player, int card);
int get_highest_power(int player);
int get_power_sum(int player);
int count_defenders(int player);
int count_creatures_by_toughness(int player, int pow, int mode);
int count_creatures_by_power(int player, int pow, int mode);
int get_highest_cmc_nonland(int player);
int get_highest_cmc(int player, type_t type);
int get_most_common_cmc_in_hand(int player, type_t type);
int get_most_common_cmc_nonland(int player);
int count_permanents_by_type(int player, type_t type);
int count_permanents_by_color(int player, int selected_type, int selected_color);
int count_cards_by_id(int player, int id);
int check_battlefield_for_subtype(int what_player, int type, int subtype);
int count_subtype(int what_player, int type, int subtype);
int count_shrines(int player, int card);
int is_x_spell(int player, int card);
int is_hardcoded_card(int player, int card);
int is_valid_card(int id);
int is_stolen(int player, int card);
int is_target(int player, int card, int controller_of_effect);
int is_sick(int player, int card);
int is_animated_and_sick(int player, int card);
int is_attacking(int player, int card);
card_instance_t *in_play_and_attacking(int player, int card);
int check_state(int player, int card, int state);
int check_status(int player, int card, int status);
void add_state(int player, int card, int state);
void remove_state(int player, int card, int state);
void add_status(int player, int card, int status);
void remove_status(int player, int card, int status);
void state_untargettable(int player, int card, int mode);
int tap_card(int player, int card);
void untap_card(int player, int card);
void untap_card_no_event(int player, int card);
int is_creature_dead(void);
int has_dead_creature(int player);
card_instance_t *has_activation_on_stack(int player, int card);
int is_basic_land_by_id(int id);
int is_basic_land(int player, int card);
int is_mana_producer_land(int player, int card);
int no_combat_damage_this_turn(int player, int card, event_t event);
void negate_combat_damage_this_turn(int player, int card, int t_player, int t_card,
                                         int alternate_legacy_text_number);
int my_damage_cannot_be_prevented(int player, int card, event_t event);
int prevent_damage(int damage_player, int damage_card_index, int amount);
void prevent_the_next_n_damage(int player, int card, int t_player, int t_card, int amount,
                                    prevent_t flags, int redir_to_player, int redir_to_card);
int fog_special2(int player, int card, int t_player, fog_special_flags_t flags, int param);
int fog_special(int player, int card, int t_player, int flags);
int fog_effect(int player, int card);
void negate_damage_this_turn(int player, int card, int t_player, int t_card, int unused);
int prevent_all_damage_to_target(int player, int card, int t_player, int t_card, int mode);
int prevent_all_damage(int player, int card, event_t event);
void damage_target_creature_or_player(int t_player, int t_card, int amount, int source_player,
                                           int source_card);
int damage_cannot_be_prevented_until_eot(int player, int card);
void prevent_all_my_damage(int player, int card, event_t event, int flags);
int prevent_all_damage_dealt_by_target(int player, int card, int t_player, int t_card,
                                            int flags);
int prevent_damage_until_eot(int player, int card, int t_player, int t_card, int maximum);
void raw_counter_activated_ability(int tgt_player, int tgt_card);
int charge_mana_hybrid_while_resolving(int player, int card, event_t event, int amt_colored,
                                            color_t first_color, color_t second_color,
                                            int amt_colorless);
int get_hacked_color(int player, int card, int color);
int get_sleighted_color(int player, int card, int color);
int get_colors_to_sleight_from_text(int csvid);
int get_hacked_subtype(int player, int card, int orig_subtype);
int get_sleighted_protection(int player, int card, int orig_prot);
int legacy_change_color(int player, int card, event_t event);
static int get_colors_to_hack_from_type_text(int csvid);
static int get_colors_to_hack_from_rules_text(int csvid);
const char *get_card_name_by_instance(card_instance_t *instance);
const char *get_card_or_subtype_name(int subtype_or_csvid);
static int subtype_name_legacy(int player, int card, event_t event);
static int legacy_name_with_rfg_check(int player, int card, event_t event);
int get_card_name(int player, int card);
int has_same_name(int player, int card, int player2, int card2);
int count_permanents_by_name(int player, type_t type, int csvid);
void ai_modifier_for_land_untappers(int player, int card, int target_pos);
void ai_modifier_for_haste_granting_effects(int player, int card);
void ai_modifier_twiddle(int player, int card, int tgtnum);
int choosing_to_untap(int player, int card, event_t event);
void remove_from_combat(int player, int card);
static int remove_untargettable_legacy(int player, int card, event_t event);
int is_artifact_creature_by_internal_id(int iid, int unused1, int unused2, int unused3);
void land_animation(int player, int card);
void true_transform(int player, int card);
int create_legacy_name_with_rfg_check(int player, int card, int owner, int iid);
int create_subtype_name_legacy(int player, int card, int subtype);
int create_card_name_legacy(int player, int card, int id);
int create_hardcoded_subtype_name_legacy(int player, int card, int subtype);
void default_pump_ability_definition(int player, int card, pump_ability_t *pump, int pow,
                                          int tou, int key, int skey);
int pump_ability(int player, int card, int t_player, int t_card, pump_ability_t *pump);
void pump_ability_by_test(int player, int card, int t_player, pump_ability_t *pump,
                               test_definition_t *this_test);
static int pump_ability_until_eot_with_precomputed_sleight_hack_suppress(
    int player, int card, int t_player, int t_card, pump_ability_t *pump);
int legacy_effect_pump_ability_until_eot(int player, int card, event_t event);
static void sp_ability_trigger_impl(int controller_of_trigger, event_t event, int trigger_mode,
                                         type_t type, test_definition_t *test,
                                         int triggering_player, int triggering_card,
                                         int allow_copies);
void sp_ability_trigger(int controller_of_trigger, event_t event, int trigger_mode,
                             type_t type, test_definition_t *test, int triggering_player,
                             int triggering_card);
int can_counter_triggered_ability(int player, int card, event_t event,
                                       target_definition_t *td);
int test_leaves_play(int triggering_player, int triggering_card, event_t event, int controller,
                          test_definition_t *test, int can_be_suppressed,
                          int can_be_duplicated);
int test_leaves_play_mode(int triggering_player, int triggering_card, event_t event,
                               int controller, resolve_trigger_t mode, test_definition_t *test,
                               int can_be_suppressed, int can_be_duplicated);
void etb_ability_global(int controller_of_trigger, event_t event, int trigger_mode,
                             type_t type, test_definition_t *test, int triggering_player,
                             int triggering_card);
static int new_specific_cip_impl(int controller_of_trigger, event_t event, int trigger_mode,
                                      type_t type, test_definition_t *test,
                                      int triggering_player, int triggering_card);
int new_specific_cip_global(int player, event_t event, int trigger_mode, type_t type,
                                 test_definition_t *this_test, int triggering_player,
                                 int triggering_card);
int new_specific_cip(int player, int card, event_t event, int t_player, int trigger_mode,
                          test_definition_t *this_test);
int new_card_from_list(int player, int mode, test_definition_t *test);
void haste_and_sacrifice_token_at_end_of_next_turn(token_generation_t *token, int beast,
                                                        int number);
void haste_and_remove_token_at_end_of_next_turn(token_generation_t *token, int beast,
                                                     int number);
int reveal_cards_from_your_hand(int player, int card, test_definition_t *this_test);
void mana_into_string(int cless, int black, int blue, int green, int red, int white,
                           test_definition_t *this_test);
int ai_name_a_random_csvid(int player, int card, int t_player, type_t type,
                                test_definition_t *test);
int name_a_card(int player, int ai_player, test_definition_t *test, int mode);
int select_permanent_to_disable(int player, int card, target_definition_t *td,
                                     const char *prompt, test_definition_t *test);
int counterspell_validate(int player, int card, target_definition_t *td, int target_num);
int counterspell(int player, int card, event_t event, target_definition_t *td,
                      int target_num);
void untap_lands(int player, int card, int num_lands);
int charge_mana_multi(int player, int colorless, int black, int blue, int green, int red,
                           int white);
int charge_mana_multi_a(int player, int colorless, int black, int blue, int green, int red,
                             int white, int artifact);
int has_mana_for_activated_ability(int player, int card, int cless, int black, int blue,
                                        int green, int red, int white);
int charge_mana_for_activated_ability(int player, int card, int cless, int black, int blue,
                                           int green, int red, int white);
int charge_mana_for_activated_ability_while_resolving(int player, int card, event_t event,
                                                           int colorless, int black, int blue,
                                                           int green, int red, int white);
int get_cost_mod_for_activated_abilities(int player, int card, int orig_cost, int black,
                                              int blue, int green, int red, int white);
int charge_mana_while_resolving(int player, int card, event_t event, int charged_player,
                                     color_t color, int amount);
int charge_mana_while_resolving_csvid(int csvid, event_t event, int charged_player,
                                           color_t color, int amount);
int charge_mana_multi_while_resolving(int player, int card, event_t event, int charged_player,
                                           int colorless, int black, int blue, int green, int red,
                                           int white);
int charge_mana_multi_while_resolving_csvid(int csvid, event_t event, int charged_player,
                                                 int colorless, int black, int blue, int green,
                                                 int red, int white);
int charge_mana_for_multi_x(int player, int color, int num_x);
int charge_mana_for_double_x(int player, int color);
int get_sleighted_color_test(int player, int card, int orig_color_test);
void etb_ability(int player, int card, event_t event, resolve_trigger_t trigger_mode);
void get_sleighted_hacked_suppressed_ability(int player, int card, int t_player,
                                                  int *p_ability, int *p_sp_ability);
int pump_ability_until_eot_no_repeat(int player, int card, int t_player, int t_card,
                                          int ability, int sp_ability);
int generic_activated_ability(int player, int card, event_t event, int mode, int cless,
                                   int black, int blue, int green, int red, int white,
                                   uint32_t variable_costs, target_definition_t *td,
                                   const char *prompt);
int granted_generic_activated_ability(int granting_player, int granting_card,
                                           int granted_to_player, int granted_to_card,
                                           event_t event, int mode, int cless, int black, int blue,
                                           int green, int red, int white, uint32_t variable_costs,
                                           target_definition_t *td, const char *prompt);
int attachment_granting_activated_ability(int player, int card, event_t event, int mode,
                                               int cless, int black, int blue, int green, int red,
                                               int white, uint32_t variable_costs,
                                               target_definition_t *td, const char *prompt);
int pump_until_eot(int player, int card, int t_player, int t_card, int power, int toughness);
int upkeep_trigger_mode_impl(int player, int card, event_t event,
                                  resolve_trigger_t trigger_mode, int no_check);
void count_for_gfp_ability_and_store_values(int player, int card, int event, int t_player,
                                                 int type, test_definition_t *this_test,
                                                 int mode, int storage_location);
int reveal_card_until_you_reveal_test(int player, int t_player, test_definition_t *test,
                                           int flags);
void cloning_and_verify_legend(int player, int card, int t_player, int t_card);
void ai_mod_for_untap_effects(int player, int card, int target_pos, int total_mana_spent);
int pump_until_eot_merge_previous(int player, int card, int t_player, int t_card, int power,
                                       int toughness);
int pump_subtype_until_eot(int player, int card, int t_player, int subtype, int power,
                                int toughness, int ability, int sp_ability);
void pump_creatures_until_eot(int player, int card, int t_player,
                                   int alternate_legacy_text_number, int power, int toughness,
                                   int ability, int sp_ability, test_definition_t *this_test);
void count_for_gfp_ability(int player, int card, int event, int t_player, int type,
                                test_definition_t *this_test);
int trigger_gain_life(int player, int card, event_t event, int who_gains,
                           resolve_trigger_t trigger_mode);
int lose_life(int player, int amount);
int cast_counter_activated_ability(int player, int card, int target_pos);
int validate_counter_activated_ability(int player, int card, target_definition_t *td,
                                            int target_pos);
int resolve_counter_activated_ability(int player, int card, target_definition_t *td,
                                           int target_pos);
int can_counter_activated_ability(int player, int card, event_t event,
                                       target_definition_t *td);
int can_block_additional_until_eot(int player, int card, int t_player, int t_card,
                                        int num_additional);
void pump_color_until_eot(int player, int card, int t_player, int clr, int power,
                               int toughness, int ability, int sp_ability);
void pump_creatures_until_eot_no_repeat(int player, int card, int t_player,
                                             int alternate_legacy_text_number, int ability,
                                             int sp_ability, test_definition_t *this_test);
int vanilla_pump(int player, int card, event_t event, target_definition_t *td, int power,
                      int toughness, int keyword, int sp_keyword);
int vanilla_instant_pump(int player, int card, event_t event, int allowed_controller,
                              int preferred_controller, int power, int toughness, int keyword,
                              int sp_keyword);
int cannot_attack(int player, int card, event_t event);
void pump_creatures_until_eot_merge_pt(int player, int card, int t_player, int power,
                                            int toughness, test_definition_t *this_test);
void pump_creatures_until_eot_merge_pt_alternate_legacy_text(int player, int card,
                                                                  int t_player, int power,
                                                                  int toughness,
                                                                  test_definition_t *this_test,
                                                                  int alt_text_number);
void remove_summoning_sickness(int player, int card);
void twiddle(int player, int card, int tgtnum);
void ai_mod_for_tap_effects(int player, int card, int target_pos);
void ai_modifier_for_targeted_card_drawing(int player, int card, int target_pos,
                                                int cards_to_draw);
void ai_modifier_for_act_of_treason(int player, int card);
static int effect_pump_until_eot(int player, int card, event_t event);
static int reanimate_permanent_impl(int player, int card, int target_graveyard, int selected,
                                  reanimate_mode_t action, counter_t counter_type,
                                  CardFunction effect_fn, ReanimationBeforeEtb before_etb);
int new_reanimate_all(int player, int card, int targ_player, test_definition_t *this_test,
                           reanimate_mode_t action);
int reanimate_permanent(int player, int card, int target_graveyard, int selected,
                             reanimate_mode_t action);
int reanimate_permanent_with_effect(int player, int card, int target_graveyard, int selected,
                                         reanimate_mode_t action, CardFunction effect_fn);
int reanimate_permanent_with_counter_and_effect(int player, int card, int target_graveyard,
                                                     int selected, reanimate_mode_t action,
                                                     counter_t counter_type, CardFunction effect_fn);
int reanimate_permanent_with_function(int player, int card, int target_graveyard, int selected,
                                           reanimate_mode_t action,
                                           ReanimationBeforeEtb before_etb);
void reanimate_all(int player, int card, int targ_player, int type, int flag1, int subtype,
                        int flag2, int color, int flag3, int id, int flag4, int cc, int flag5,
                        reanimate_mode_t action);
const char *get_sleighted_color_text2(int player, int card, const char *fmt,
                                           int orig_color_t_1, int orig_color_t_2);
const char *get_sleighted_color_text(int player, int card, const char *fmt, int orig_color_t);
const char *get_card_name_by_id(int csvid);
int generic_spell(int player, int card, event_t event, int flags, target_definition_t *td,
                       const char *prompt, int max_targets, test_definition_t *this_test);
int specific_cip(int player, int card, event_t event, int t_player, int trigger_mode, int type,
                      int flag1, int subtype, int flag2, int color, int flag3, int id, int flag4,
                      int cc, int flag5);
static int haste_and_kill_at_eot(int player, int card, event_t event, int kill_mode);
int haste_and_remove_eot(int player, int card, event_t event);
int haste_and_sacrifice_eot(int player, int card, event_t event);
static int generic_haste_and_kill_at_eot_legacy(int player, int card, event_t event);
int haste_and_remove_at_beginning_of_next_end_step(int player, int card, int t_player,
                                                        int t_card, int kill_mode);
static int haste_and_exile_your_upkeep(int player, int card, event_t event);
int effect_unearth(int player, int card, event_t event);
void ereboss_titan_effect(int player);
int aot_like_spell(int player, int card, event_t event, target_definition_t *td,
                        const char *msg, int additional_flags);
void global_type_change(int player, int card, event_t event, int t_player, int added_type,
                             test_definition_t *this_test, int pow, int tou, int key,
                             int subtype, int clr);
void type_uncounterable(int player, int card, event_t event, int t_player, int type,
                             test_definition_t *this_test);
int type_change_legacy(int player, int card, event_t event);
int turn_into_artifact(int player, int card, int t_player, int t_card, int mode);
int turn_into_creature(int player, int card, int t_player, int t_card, int mode, int pow,
                            int tou);
int artifact_animation(int player, int card, int t_player, int t_card, int mode, int pow,
                            int tou, int abilities, int sp_abilities);
static int global_type_animation_legacy(int player, int card, event_t event);
int real_land_animation2(int player, int card, int t_player, int t_card, int mode, int pow,
                              int tou, int key, int s_key, int clr);
int land_animation2(int player, int card, int t_player, int t_card, int mode, int pow, int tou,
                         int key, int s_key, int clr, test_definition_t *this_test);
int card_generic_animated_land(int player, int card, event_t event);
int manland_shared(int player, int card, event_t event, int colorless, int black, int blue,
                        int green, int red, int white);
int manland_animated(int player, int card, event_t event, int colorless, int black, int blue,
                          int green, int red, int white);
int manland_normal(int player, int card, event_t event, int colorless, int black, int blue,
                        int green, int red, int white);
void cloning(int player, int card, int t_player, int t_card);
int enters_the_battlefield_as_copy_of_any_creature(int player, int card, event_t event);
int shapeshift_target(int src_player, int src_card, int t_player, int t_card,
                           int turn_into_player, int turn_into_card, int mode);
int real_shapeshift_target(int src_player, int src_card, int t_player, int t_card,
                                int turn_into_player, int turn_into_card, int mode);
static int shapeshift_target_effect(int player, int card, event_t event);
void shapeshift_all(int src_player, int src_card, int t_player, test_definition_t *test,
                         int turn_into_player, int turn_into_card, int mode);
void untap_only_1_permanent_per_upkeep(int player, int card, event_t event, int t_player,
                                            int type);
void untap_permanents_during_opponents_untap(int player, int card, type_t types,
                                                  int *where_to_store);
void choose_to_untap(int player, int card, event_t event);
void activated_abilities_disabler_enters_bf(int player, int card, int action);
int activated_abilities_disabler(int player, int card, event_t event, int action);
static const char *has_activated_ability(int who_chooses, int player, int card);
int tutor_card_with_the_same_name(int player, int card, int who_tutors, int t_player,
                                       int tutor_from, int tutor_to, int must_tutor);
void lose_the_game(int player);
void cannot_lose_the_game(int player, int card, event_t event, int who_cant_lose);
void cannot_lose_the_game_for_having_less_than_0_life(int player, int card, event_t event,
                                                           int who_cant_lose);
void store_attackers(int player, int card, event_t event, declare_attackers_trigger_t mode,
                          int attacker_player, int attacker_card, test_definition_t *test);
int resolve_declare_attackers_trigger(int player, int card, event_t event,
                                           declare_attackers_trigger_t mode);
int declare_attackers_trigger(int player, int card, event_t event,
                                   declare_attackers_trigger_t mode, int attacker_player,
                                   int attacker_card);
int declare_attackers_trigger_test(int player, int card, event_t event,
                                        declare_attackers_trigger_t mode, int attacker_player,
                                        int attacker_card, test_definition_t *test);
const char *get_hacked_land_text(int player, int card, const char *fmt, ...);
int effect_this_dies(int player, int card, event_t event);
int vanilla_creature_pumper(int player, int card, event_t event, int req_cless, int req_black,
                                 int req_blue, int req_green, int req_red, int req_white, int flags,
                                 int pow, int tou, int key, int s_key, target_definition_t *td);
int generic_shade_amt_can_pump(int player, int card, int pump, int life_to_pay, int cless,
                                    int black, int blue, int green, int red, int white, int limit);
int when_attacks_pump_self(int player, int card, event_t event, int power, int toughness);
int can_lose_the_game(int player);
static int fx_permanents_destroyed_this_way_accumulate(int player, int card, event_t event);
card_instance_t *legacy_permanents_destroyed_this_way_accumulate(
    int src_player, int src_card, card_instance_t *previous_effect, int tgt_player, int tgt_card);
void if_a_card_would_be_put_into_graveyard_from_play_exile_it_instead(
    int player, int card, int event, int t_player, test_definition_t *test);
int if_a_card_would_be_put_into_graveyard_from_library_do_something_instead(
    int player, int card, int event, int t_player, resolve_trigger_t trigger_mode,
    test_definition_t *test);
int if_a_card_would_be_put_into_graveyard_from_anywhere_but_library_do_something_instead(
    int player, int card, int event, int t_player, resolve_trigger_t trigger_mode,
    test_definition_t *test);
void if_a_card_would_be_put_into_graveyard_from_anywhere_exile_it_instead(
    int player, int card, int event, int t_player, test_definition_t *test);
int color_to_color_test(int color);
void damage_creature_and_all_the_others_with_the_same_name(int player, int card, int t_player,
                                                                int t_card, int dmg);
int alternate_legacy_text(int textnum, int player, int legacycard);
static int get_pt_for_creature_with_variable_pt(int player, int iid, event_t event);
static int split_cards_cmc_table_by_id(int csvid, int half_played);
static int prevent_all_damage_dealt_by_target_legacy(int player, int card, event_t event);
int damage_cannot_be_prevented_until_eot_legacy(int player, int card, event_t event);
int no_damage_this_turn(int player, int card, event_t event);
static int prevent_all_damage_to_target_legacy(int player, int card, event_t event);
int fog_effect_legacy(int player, int card, event_t event);
static int effect_prevent_the_next_n_damage(int player, int card, event_t event);
int prevent_damage_until_eot_legacy(int player, int card, event_t event);
int equipment_granting_activated_ability(int player, int card, event_t event,
                                              const char *dialog_option, int equip_cost, int mode,
                                              int cless, int black, int blue, int green, int red,
                                              int white, uint32_t variable_costs,
                                              target_definition_t *td, const char *prompt);
int generic_activated_ability_with_sacrifice_as_cost(
    int player, int card, event_t event, generic_activated_ability_flags_t flags, int cless,
    int black, int blue, int green, int red, int white, int gvc_costs, target_definition_t *td,
    const char *prompt, type_t type, test_definition_t *test,
    sacrifice_and_report_value_flags_t sac_flags);
int generic_activated_ability_with_target_in_grave(
    int player, int card, event_t event, generic_activated_ability_flags_t flags, int cless,
    int black, int blue, int green, int red, int white, int gvc_costs, target_definition_t *td,
    const char *prompt, type_t type, test_definition_t *test, int gs_flags_for_grave_recycler);
int generic_activated_ability_with_tapsubtype_as_cost(
    int player, int card, event_t event, generic_activated_ability_flags_t flags, int cless,
    int black, int blue, int green, int red, int white, int gvc_costs, target_definition_t *td,
    const char *prompt, target_definition_t *td_subtype, int req_tap);
static void count_for_gfp_ability_and_store_values_impl(int player, int card, int event,
                                                             int t_player, int type,
                                                             test_definition_t *this_test,
                                                             CardPairFunction extra_check, int mode,
                                                             int storage_location);
void count_for_gfp_ability_and_store_values_extra(int player, int card, int event,
                                                       int t_player, int type,
                                                       CardPairFunction extra_check, int mode,
                                                       int storage_location);
int enters_the_battlefield_as_copy_of_any(int player, int card, event_t event,
                                               target_definition_t *td, const char *prompt);
static const char *has_mana_to_cast_target(int who_chooses, int player, int card,
                                         int targeting_player, int targeting_card);
int can_play_cards_as_though_they_had_flash(int player, int card, event_t event,
                                                 target_definition_t *td, const char *prompt,
                                                 int literal);
static int discard_card_by_int_id(int player, int int_crd_id, int effect,
                                     test_definition_t *this_test, int source_of_effect);
int new_effect_coercion(ec_definition_t *this_definition, test_definition_t *this_test);
int exile_top_x_card_you_may_play_them(int player, int card, int howmany,
                                            test_definition_t *test, int t_player,
                                            mpcfe_mode_t flags);
char *type_text(char *buf, int maxlen, type_t type, type_text_t flags);
static void cannot_lose_the_game_impl(int player, int card, event_t event, int who_cant_lose,
                                           int for_poison, int for_draw_on_empty_deck);
int beginning_of_combat(int player, int card, event_t event, int aff_player);
static int this_dies_trigger_impl(int player, int card, event_t event,
                                       resolve_trigger_t trigger_mode, int who_gets_trigger,
                                       int attached, int required_kill_code);
int lobotomy_effect(int player, int t_player, int csvid, int max, int flags);
void replace_all_instances_of_one_color_word_with_another(int player, int card, int t_player,
                                                               int t_card);
void replace_all_instances_of_one_basic_land_type_word_with_another(int player, int card,
                                                                         int t_player, int t_card);
int other_leaves_play(int triggering_player, int triggering_card, int t_player, int t_card,
                           event_t event, int can_be_suppressed, int can_be_duplicated);
int leaves_play(int player, int card, event_t event, int can_be_suppressed,
                     int can_be_duplicated);

/* DLL-owned storage shared by C and C++; card_coded is an EXE symbol. */
char *set_legacy_effect_name_addr = NULL;

#include <windows.h>

/* Private callback recovered below; used by the October 2022 deflection helpers. */
static char* has_only_1_target_and_could_legally_change_target(int who_chooses, int player, int card);

int discard_controller = 0;

unsigned int num_bits_set(unsigned int v){
  // From Bit Twiddling Hacks, http://graphics.stanford.edu/~seander/bithacks.html#CountBitsSetParallel
  v = v - ((v >> 1) & 0x55555555);                              // reuse input as temporary
  v = (v & 0x33333333) + ((v >> 2) & 0x33333333);               // temp
  return (((v + (v >> 4)) & 0xF0F0F0F) * 0x1010101) >> 24;      // count
}

// Allows use of memset() into an address we know is large enough, but the compiler thinks isn't.
void memset_suppress_size_warning(void* addr, int val, size_t sz)
{
	memset(addr, val, sz);
}

// Allows use of memcpy() into an address we know is large enough, but the compiler thinks isn't.
void memcpy_suppress_size_warning(void* dest, const void* src, size_t sz)
{
	memcpy(dest, src, sz);
}

int get_attack_power(int player, int card)
{
  for (int p = 0; p < 2; ++p)
    for (int c = 0; c < active_cards_count[p]; ++c)
    {
      card_instance_t *instance = in_play(p, c);
      if (!instance)
        continue;
      switch (cards_data[instance->internal_card_id].id)
      {
      case CARD_ID_ARCADES_THE_STRATEGIST:
        if (!check_for_ability(player, card, KEYWORD_DEFENDER))
          break;
        /* fall through */
      case CARD_ID_ASSAULT_FORMATION:
      case CARD_ID_THE_GREAT_FOREST:
      case CARD_ID_CONSPIRACY_WEIGHT_ADVANTAGE:
      case CARD_ID_HIGH_ALERT:
      case 0x4dc2:
        if (p != player)
          break;
        /* fall through */
      case CARD_ID_DORAN_THE_SIEGE_TOWER:
        if (!is_humiliated(p, c, AL_STATIC))
          return get_toughness(player, card);
        break;
      }
    }
  return get_power(player, card);
}

int get_discard_controller(void){
	return discard_controller;
}


void set_discard_flag(int player, int card){
	raw_set_poison(HUMAN, 3);
   // raw_set_poison(AI, 1+card);
   // life[AI] = 20+event;
   life[AI] = reason_for_trigger_controller;
   life[HUMAN] = card_on_stack_controller;
	discard_controller = player;
	return;
}



int duh_mode(int player){
	return IS_AI(player) || get_setting(SETTING_DUH_MODE);
}

int dealt_damage_to_player(int player, int card, int parent_player, int parent_card, int event){
	//card_instance_t *instance = get_card_instance(player, card);
	if( event == EVENT_DEAL_DAMAGE ){
		card_instance_t *damage= get_card_instance( affected_card_controller, affected_card);
		card_instance_t *parent_instance = get_card_instance(parent_player, parent_card);
		if( damage->internal_card_id == damage_card ){
			if( damage->damage_target_card == -1 && damage->damage_source_player == player ){
				if(  damage->damage_source_card == card && damage->info_slot > 0 ){
					parent_instance->targets[16].player = damage->info_slot;
				}
			}
		}
	}

	card_instance_t *parent_instance = get_card_instance(parent_player, parent_card);
	if( trigger_condition == TRIGGER_DEAL_DAMAGE && affect_me(parent_player, parent_card) &&
		reason_for_trigger_controller == parent_player && parent_instance->targets[16].player > 0 ){
		if(event == EVENT_TRIGGER){
			event_result |= 2;
		}
		else if(event == EVENT_RESOLVE_TRIGGER){
				parent_instance->targets[16].player = 0;
				return 1;
		}
	}
	return 0;
}



int has_combat_damage_been_inflicted_to_opponent(int player, int card, event_t event){
	return damage_dealt_by_me(player, card, event, DDBM_MUST_BE_COMBAT_DAMAGE|DDBM_MUST_DAMAGE_OPPONENT);
}

int has_combat_damage_been_inflicted_to_a_player(int player, int card, event_t event){
	return damage_dealt_by_me(player, card, event, DDBM_MUST_BE_COMBAT_DAMAGE|DDBM_MUST_DAMAGE_PLAYER);
}

int deathtouch_until_eot(int player, int card, event_t event){

	if (event == EVENT_ABILITIES){
		card_instance_t *instance = get_card_instance( player, card );
		deathtouch(instance->targets[0].player, instance->targets[0].card, event);
	}

	if( eot_trigger(player, card, event) ){
		kill_card(player, card, KILL_EXILE);
	}

	return 0;
}

int get_sum(int player, type_t type){
	int p = player;
	int count = 0;
	int sum = 0;
	while (count < active_cards_count[p]){
		if (in_play(p, count) && is_what(p, count, type)){
			sum += count;	// This doesn't do what you probably think it does.
		}
		count++;
	}
	return sum;
}

int get_hand_sum(int player, type_t type){
	int p = player;
	int count = 0;
	int sum = 0;
	while (count < active_cards_count[p]){
		if (in_hand(p, count) && is_what(p, count, type)){
			sum += count;	// This doesn't do what you probably think it does.
		}
		count++;
	}
	return sum;
}







static int id_updater_for_reveal_top_card_legacy(int player, int card, event_t event){
	card_instance_t *instance = get_card_instance(player, card);
	if( instance->damage_target_player > -1 ){
		card_instance_t *leg = get_card_instance(instance->damage_target_player, instance->damage_target_card);
		if( leg->damage_target_player > -1 ){
			int* deck = deck_ptr[leg->damage_target_player];
			if( deck[0] != -1 ){
				leg->info_slot = -cards_data[deck[0]].id-1;
			}
		}
	}
	return 0;
}

int reveal_top_card(int player, int card, event_t event){

	if( event == EVENT_RESOLVE_SPELL ){
		int* deck = deck_ptr[player];
		if( deck[0] != -1 ){
			card_data_t* card_d = &cards_data[ deck[0] ];
			int legacy = create_card_name_legacy(player, card, card_d->id );
			int leg2 = create_targetted_legacy_effect(player, card, &id_updater_for_reveal_top_card_legacy, player, legacy);
			add_status(player, leg2, STATUS_INVISIBLE_FX);
		}
	}

	return 0;
}

int reveal_top_card_both_player(int player, int card, event_t event){

  card_instance_t *instance = get_card_instance(player, card);
  int p;

  if (event == EVENT_RESOLVE_SPELL)
	for (p = 0; p <= 1; ++p)
	  {
		instance->targets[p].player = player;
		if (deck_ptr[p][0] == -1)
		  instance->targets[p].card = -1;
		else
		  instance->targets[p].card = create_card_name_legacy(player, card, cards_data[deck_ptr[p][0]].id);
	  }

  if (affect_me(player, card) && instance->targets[0].player >= 0)
	{
	  for (p = 0; p <= 1; ++p)
		if (instance->targets[p].card == -1)
		  {
			if (deck_ptr[p][0] != -1)	// was empty, now has a card
			instance->targets[p].card = create_card_name_legacy(instance->targets[p].player, card, cards_data[deck_ptr[p][0]].id);
		  }
		else
		  {
			if (deck_ptr[p][0] == -1)	// had a card, now empty
			  {
				kill_card(instance->targets[p].player, instance->targets[p].card, KILL_EXILE);
				instance->targets[p].card = -1;
			  }
			else	// had a card, still does
			  {
				card_instance_t* legacy = get_card_instance(instance->targets[p].player, instance->targets[p].card);
				int csvid = cards_data[deck_ptr[p][0]].id;
				legacy->info_slot = -csvid - 1;	// update csvid if it's different
			  }
		  }
	}

	return 0;
}

int get_random_card(void){
	int i;
	int crd = -1;
	for(i=0; i<100; i++){
		crd+=100*internal_rand(16);
	}
	if( crd > (available_slots-1) ){
		crd -= (available_slots-1);
	}

	if( is_valid_card(crd) ){
		return crd;
	}
	else{
		while(1){
			crd++;
			if( is_valid_card(crd) ){
				return crd;
			}
		}
	}
	return -1;
}

// Can a card with this iid be played (other than mana cost and timing restrictions)?
int can_legally_play_iid(int player, int internal_card_id)
{
  card_data_t *card_d = &cards_data[internal_card_id];

  // Lands can only be played once per turn, and only during your turn.
  if ((card_d->type & TYPE_LAND) &&
      ((land_can_be_played & LCBP_LAND_HAS_BEEN_PLAYED) || (current_turn != player)))
    return 0;

  // Instants, interrupts, sorceries, and enchantments (including permanents with flash and
  // planeswalkers) must return true from EVENT_CAN_CAST.
  if (card_d->type &
      (TYPE_INSTANT | TYPE_INTERRUPT | TYPE_SORCERY |
       TYPE_ENCHANTMENT)) // deliberately not is_what(), so flash cards returning 99 are correct
  {
    /* Calling code pointers directly is DANGEROUS; exe and especially asm cards make assumptions
     * about what's in each register.  One of the things needed is a valid card instance, so we have
     * to create a card.  Besides, it's needed so targeting works right; and some especially
     * misbehaving C cards set values on the card during EVENT_CAN_CAST. */
    int card_added = add_card_to_hand(player, internal_card_id);
    card_instance_t *instance = get_card_instance(player, card_added);
    instance->state |= STATE_INVISIBLE;
    --hand_count[player];
    set_special_flags(player, card_added, SF_PLAYED_FOR_FREE);
    int rval = call_card_fn(card_d->code_pointer, instance, player, card_added, EVENT_CAN_CAST);
    obliterate_card_and_recycle(player, card_added);
    return rval;
  }

  // A (non-flash) creature, artifact, or land (when one hasn't been played, and it's your turn).
  return 1;
}

// Can a card with this iid be played (other than mana cost)?
int can_legally_play_iid_now(int player, int internal_card_id, event_t event)
{
  if ((cards_data[internal_card_id].type & (TYPE_INSTANT | TYPE_INTERRUPT))	// deliberately not is_what(), to include permanents with flash
	  || can_sorcery_be_played(player, event))
	return can_legally_play_iid(player, internal_card_id);
  else
	return 0;
}

// Can a card with this csvid be played (other than mana cost and timing restrictions)?
int can_legally_play_csvid(int player, int csvid)
{
  return can_legally_play_iid(player, get_internal_card_id_from_csv_id(csvid));
}

// Can a card with this csvid be played (other than mana cost)?
int can_legally_play_csvid_now(int player, int csvid, event_t event)
{
  return can_legally_play_iid_now(player, get_internal_card_id_from_csv_id(csvid), event);
}

int can_legally_play_card_in_hand(int player, int card)
{
  card_data_t* card_d = &cards_data[get_card_instance(player, card)->internal_card_id];

  // Lands can only be played once per turn, and only during your turn.
  if ((card_d->type & TYPE_LAND)
	  && ((land_can_be_played & LCBP_LAND_HAS_BEEN_PLAYED) || (current_turn != player)))
	return 0;

  // Instants, interrupts, sorceries, and enchantments (including permanents with flash and planeswalkers) must return true from EVENT_CAN_CAST.
  if (card_d->type & (TYPE_INSTANT | TYPE_INTERRUPT | TYPE_SORCERY | TYPE_ENCHANTMENT))	// deliberately not is_what(), so flash cards returning 99 are correct
	{
		return call_card_function(player, card, EVENT_CAN_CAST);
	}

  // A (non-flash) creature, artifact, or land (when one hasn't been played, and it's your turn).
  return 1;
}

static int is_aura(int player, int card ){
	if( has_subtype(player, card, SUBTYPE_AURA) ){
		return 1;
	}
	return 0;
}

/* Exiles {t_player,t_card} and then returns it to the battlefield under its owner's control.  If before_etb is non-null, call that function with the card just
 * before it re-enters the battlefield. */
void blink_effect(int t_player, int t_card, void (*before_etb)(int, int))
{
  if (!in_play(t_player, t_card))
    return;
  int token = is_token(t_player, t_card);
  int iid = get_original_internal_card_id(t_player, t_card);
  card_instance_t *instance = get_card_instance(t_player, t_card);
  int owner = (instance->state & STATE_OWNED_BY_OPPONENT) ? 1 : 0;
  int csvid = cards_data[iid].id;
  int csvids[2] = {-1, -1};
  int parts = 1;
  if (csvid == 0x3eae || csvid == 0x3ef4 || csvid == 0x3efb)
  {
    unmeld(csvid, &csvids[0], &csvids[1]);
    iid = get_internal_card_id_from_csv_id(csvids[0]);
    parts = 2;
  }
  kill_card(t_player, t_card, KILL_EXILE);
  if (token || !is_what(-1, iid, TYPE_PERMANENT))
    return;
  for (int part = 0; part < parts; ++part)
  {
    if (part)
      iid = get_internal_card_id_from_csv_id(csvids[1]);
    int card_added = add_card_to_hand(owner, iid);
    if (has_subtype(owner, card_added, SUBTYPE_AURA))
    {
      if (!call_card_function(owner, card_added, EVENT_CAN_CAST))
      {
        obliterate_card(owner, card_added);
        continue;
      }
      set_special_flags3(owner, card_added, SF3_IN_PLAY_DIRECTLY);
    }
    remove_card_from_rfg(owner, cards_data[iid].id);
    if (before_etb)
      before_etb(owner, card_added);
    put_into_play(owner, card_added);
  }
}

/* Exiles {player, card} and then returns it to the battlefield under it's controllers control.  If before_etb is non-null, call that function with the card just
 * before it re-enters the battlefield. */
void blink_effect_controller(int player, int card, void (*before_etb)(int, int))
{
  if (!in_play(player, card))
    return;
  int token = is_token(player, card);
  int iid = get_original_internal_card_id(player, card);
  card_instance_t *instance = get_card_instance(player, card);
  int owner = (instance->state & STATE_OWNED_BY_OPPONENT) ? 1 : 0;
  int csvid = cards_data[iid].id;
  int csvids[2] = {-1, -1};
  int parts = 1;
  if (csvid == 0x3eae || csvid == 0x3ef4 || csvid == 0x3efb)
  {
    unmeld(csvid, &csvids[0], &csvids[1]);
    iid = get_internal_card_id_from_csv_id(csvids[0]);
    parts = 2;
  }
  kill_card(player, card, KILL_EXILE);
  if (token || !is_what(-1, iid, TYPE_PERMANENT))
    return;
  for (int part = 0; part < parts; ++part)
  {
    if (part)
      iid = get_internal_card_id_from_csv_id(csvids[1]);
    int card_added = add_card_to_hand(player, iid);
    if (has_subtype(player, card_added, SUBTYPE_AURA))
    {
      if (!call_card_function(player, card_added, EVENT_CAN_CAST))
      {
        obliterate_card(player, card_added);
        continue;
      }
      set_special_flags3(player, card_added, SF3_IN_PLAY_DIRECTLY);
    }
    remove_card_from_rfg(owner, cards_data[iid].id);
    if (before_etb)
      before_etb(player, card_added);
    if (player != owner)
    {
      if (player == AI)
        remove_state(player, card_added, STATE_OWNED_BY_OPPONENT);
      else
        add_state(player, card_added, STATE_OWNED_BY_OPPONENT);
    }
    put_into_play(player, card_added);
  }
}

static int remove_until_eot_legacy(int player, int card, event_t event)
{
  short sVar1;
  uint16_t uVar2;
  unsigned int uVar3;
  card_instance_t *pcVar4;
  int iVar5;
  int iVar6;
  card_instance_t *pcVar7;

  pcVar4 = get_card_instance(player, card);
  if (pcVar4->targets[1].card == 0)
    goto LAB_0269ed87;
  iVar5 = eot_trigger(player, card, event);
  if (iVar5 == 0)
    goto LAB_0269ed87;
  iVar5 =
      check_rfg(pcVar4->targets[0].player, (unsigned int)cards_data[pcVar4->targets[0].card].id);
  if (iVar5 != 0)
  {
    iVar5 = add_card_to_hand(pcVar4->targets[0].player, pcVar4->targets[0].card);
    iVar6 = has_subtype(pcVar4->targets[0].player, iVar5, SUBTYPE_AURA);
    if (iVar6 != 0)
    {
      iVar6 = call_card_function(pcVar4->targets[0].player, iVar5, EVENT_CAN_CAST);
      if (iVar6 == 0)
      {
        obliterate_card(pcVar4->targets[0].player, iVar5);
        goto LAB_0269ede4;
      }
      set_special_flags3(pcVar4->targets[0].player, iVar5, 0x40);
    }
    remove_card_from_rfg(pcVar4->targets[0].player,
                         (unsigned int)cards_data[pcVar4->targets[0].card].id);
    uVar3 = pcVar4->targets[1].player;
    if ((uVar3 & 1) == 0)
    {
      iVar6 = pcVar4->targets[0].player;
      if ((uVar3 & 2) != 0)
      {
        iVar6 = is_what(iVar6, iVar5, 2);
        if (iVar6 != 0)
          goto LAB_0269ef0c;
        goto LAB_0269ef36;
      }
    }
    else
    {
    LAB_0269ef0c:
      add_counters(player, card, pcVar4->targets[0].player, iVar5, COUNTER_P1_P1, 1);
    LAB_0269ef36:
      iVar6 = pcVar4->targets[0].player;
      if ((pcVar4->targets[1].player & 2) != 0)
      {
        iVar6 = is_what(iVar6, iVar5, 0x100);
        if (iVar6 != 0)
        {
          add_counters(player, card, pcVar4->targets[0].player, iVar5, COUNTER_LOYALTY,
                                   1);
        }
        iVar6 = pcVar4->targets[0].player;
      }
    }
    put_into_play(iVar6, iVar5);
    sVar1 = pcVar4->display_pic_csv_id;
    if (sVar1 == 0x2112)
    {
      iVar6 = get_abilities(pcVar4->targets[0].player, iVar5, EVENT_TOUGHNESS, -1);
      new_multidiscard(player, iVar6, 0, player);
      uVar2 = pcVar4->display_pic_csv_id;
      sVar1 = uVar2;
    }
    if (sVar1 == 0x474b)
    {
      pcVar7 = get_card_instance(pcVar4->targets[0].player, iVar5);
      pcVar7->state = pcVar7->state | 0x10;
    }
  }
LAB_0269ede4:
  kill_card(player, card, KILL_EXILE);
LAB_0269ed87:
  if (event == EVENT_CLEANUP)
  {
    pcVar4->targets[1].card = 1;
  }
  return 0;
}

int remove_until_eot(int player, int card, int t_player, int t_card)
{
  if (!in_play(t_player, t_card))
    return -1;
  int token = is_token(t_player, t_card);
  int iid = get_original_internal_card_id(t_player, t_card);
  card_instance_t *instance = get_card_instance(t_player, t_card);
  int owner = (instance->state & STATE_OWNED_BY_OPPONENT) ? 1 : 0;
  int csvid = cards_data[iid].id;
  int csvids[2] = {-1, -1};
  int parts = 1;
  if (csvid == 0x3eae || csvid == 0x3ef4 || csvid == 0x3efb)
  {
    unmeld(csvid, &csvids[0], &csvids[1]);
    iid = get_internal_card_id_from_csv_id(csvids[0]);
    parts = 2;
  }
  kill_card(t_player, t_card, KILL_EXILE);
  if (token || !is_what(-1, iid, TYPE_PERMANENT))
    return -1;
  int legacy = -1;
  for (int part = 0; part < parts; ++part)
  {
    if (part)
      iid = get_internal_card_id_from_csv_id(csvids[1]);
    legacy = create_legacy_effect(player, card, remove_until_eot_legacy);
    card_instance_t *leg = get_card_instance(player, legacy);
    leg->targets[0].player = owner;
    leg->targets[0].card = iid;
    leg->targets[1].player = 0;
    switch (get_id(player, card))
    {
    case CARD_ID_LONG_ROAD_HOME:
    case CARD_ID_OTHERWORLDLY_JOURNEY:
      leg->targets[1].player = 1;
      break;
    case 0x590a:
      leg->targets[1].player = 2;
      break;
    }
    leg->targets[1].card = current_phase != PHASE_CLEANUP;
  }
  return legacy;
}

void obliviation(int player, int card, int targ_player, int targ_card)
{
  if (!is_token(targ_player, targ_card) && in_play(targ_player, targ_card))
  {
    int iid = get_original_internal_card_id(targ_player, targ_card);
    int csvid = cards_data[iid].id;
    int csvids[2] = {-1, -1};
    int meld = csvid == CARD_ID_BRISELA_VOICE_OF_NIGHTMARES || csvid == CARD_ID_CHITTERING_HOST ||
               csvid == CARD_ID_HANWEIR_THE_WRITHING_TOWNSHIP;
    if (meld)
    {
      unmeld(csvid, &csvids[0], &csvids[1]);
      iid = get_internal_card_id_from_csv_id(csvids[0]);
    }
    for (int i = 0; i < (meld ? 2 : 1); ++i)
    {
      card_instance_t *legacy =
          get_card_instance(player, create_card_name_legacy(player, card, cards_data[iid].id));
      legacy->targets[7].player = get_owner(targ_player, targ_card);
      legacy->targets[7].card = iid;
      if (meld && i == 0)
        iid = get_internal_card_id_from_csv_id(csvids[1]);
    }
  }
  kill_card(targ_player, targ_card, KILL_EXILE);
}

void true_return_from_oblivion(int player, int card)
{
  int p, c, iid, owner;
  while (find_obliviation_legacy(player, card, &p, &c, &iid, &owner))
  {
    kill_card(p, c, KILL_EXILE);
    if (check_rfg(owner, cards_data[iid].id))
    {
      int card_added = add_card_to_hand(owner, iid);
      if (has_subtype(owner, card_added, SUBTYPE_AURA))
      {
        if (!call_card_function(owner, card_added, EVENT_CAN_CAST))
        {
          obliterate_card(owner, card_added);
          continue;
        }
        set_special_flags3(owner, card_added, SF3_IN_PLAY_DIRECTLY);
      }
      remove_card_from_rfg(owner, cards_data[iid].id);
      put_into_play(owner, card_added);
    }
  }
}

void return_from_oblivion(int player, int card, event_t event)
{
  leaves_play(player, card, event, RESOLVE_TRIGGER_OPTIONAL, 0);
  if (event == EVENT_LEAVES_PLAY_ABILITY)
    true_return_from_oblivion(player, card);
}

int check_for_cip_effects_removal(int player, int card)
{
  if (is_what(player, card, TYPE_CREATURE))
  {
    int i, k;
    for (i = 0; i < 2; i++)
    {
      for (k = 0; k < active_cards_count[i]; k++)
      {
        if (in_play(i, k) && !is_humiliated(i, k, AL_STATIC))
        {
          if (get_id(i, k) == CARD_ID_TORPOR_ORB || get_id(i, k) == CARD_ID_HUSHWING_GRYFF ||
              get_id(i, k) == CARD_ID_TOCATLI_HONOR_GUARD || get_id(i, k) == 0x5097)
          {
            return 1;
          }
        }
      }
    }
  }
  return 0;
}

int comes_into_play_mode(int player, int card, event_t event, resolve_trigger_t trigger_mode)
{
  if (trigger_condition == TRIGGER_COMES_INTO_PLAY && affect_me(player, card) && player == reason_for_trigger_controller
	  && player == trigger_cause_controller && card == trigger_cause
	  && !is_humiliated(player, card, AL_TRIG) && ! check_for_cip_effects_removal(player, card))
	{
	  if (event == EVENT_TRIGGER)
		{
		  if (trigger_mode == RESOLVE_TRIGGER_DUH)
			trigger_mode = duh_mode(player) ? RESOLVE_TRIGGER_MANDATORY : RESOLVE_TRIGGER_OPTIONAL;

		  event_result |= trigger_mode;
		}
	  else if (event == EVENT_RESOLVE_TRIGGER)
		return 1;
	}
  return 0;
}

int comes_into_play(int player, int card, event_t event)
{
  return comes_into_play_mode(player, card, event, RESOLVE_TRIGGER_MANDATORY);
}

void comes_into_play_tapped(int player, int card, event_t event)
{
  if ((event == EVENT_CAST_SPELL || event == EVENT_ENTERING_THE_BATTLEFIELD_AS_CLONE) &&
      affect_me(player, card))
  {
    card_instance_t *instance = get_card_instance(player, card);
    if (!((card_flags[0] | card_flags[1]) & 8) &&
        !(instance->ext->special_flags2 & SF2_FACE_DOWN_DUE_TO_MANIFEST) &&
        !is_humiliated(player, card, AL_TRIG))
      get_card_instance(player, card)->state |= STATE_TAPPED;
  }
}



// As leaves_play(), but allows a card ({triggering_player,triggering_card}) to watch for a different card ({t_player,t_card}) leaving play, not itself.






extern int hack_allow_sorcery_if_rules_engine_is_only_effect;
int can_sorcery_be_played(int player, event_t event){
	// Must be your own main phase
	if(current_turn != player || (current_phase != PHASE_MAIN1 && current_phase != PHASE_MAIN2)){
		return 0;
	}

	// If the stack is empty, good
	if (stack_size <= 0){
		return 1;
	}

	if (stack_size == 1){
		/* During EVENT_CAST_SPELL and EVENT_ACTIVATE, the card that's being cast or effect that's being activated is already on the stack.  If it's the only
		 * one on the stack, however, a sorcery was legal to play when it was put there. */
		if (event == EVENT_CAST_SPELL || event == EVENT_ACTIVATE){
			return 1;
		}

		/* A rules engine activation is a special case, since it'll indirectly call other card functions that themselves will call this with EVENT_CAN_ACTIVATE
		 * or similar instead of the original EVENT_ACTIVATE.  So we set hack_allow_sorcery_if_rules_engine_is_only_effect at the start of card_rules_engine()'s
		 * EVENT_CAN_ACTIVATE and clear it at the end. */
		if (hack_allow_sorcery_if_rules_engine_is_only_effect){
			card_instance_t* instance = get_card_instance(stack_cards[0].player, stack_cards[0].card);
			if (cards_data[instance->original_internal_card_id].id == CARD_ID_RULES_ENGINE){
				return 1;
			}
		}
	}

	// Nope, there's actually a spell or effect on the stack.
	return 0;
}

int count_subtype_in_hand(int player, subtype_t type){

  int i;
  int count = 0;

  for(i=0; i < active_cards_count[player]; i++){
	  if( in_hand(player, i) && has_subtype(player, i, type) ){
		 count++;
	  }
  }

  return count;
}





/* Don't send EVENT_UNTAP_CARD events.  Use only if this card is untapping only as an implementation detail, e.g. it was tapped to prevent it from tapping to
 * generate mana for its own activation cost, and the player cancelled during mana charging. */






























/* If there's another pump_until_eot effect card originating from player/card attached to t_player/t_card, controlled by player, then add the power/toughness
 * specified here to that effect and return it.  (If you've manually fiddled with the effect's other target settings, you're on your own; they're not checked
 * for.)  Otherwise, create a new effect and return that instead.  In either case, set the effect's counter_power and counter_toughness so they can be seen by
 * |n in the effect text - which must specifically have |n's in it or this'll be hopelessly confusing. */






/* Shared in October 2022 (COFF external); also used by recovered aura helpers. */








/* Just like pump_ability_until_eot(), but can't pump power or toughness, and if the creature already has an effect card from {player,card} and has all the
 * named keywords set, only gives the existing effect a new timestamp instead of making a new one. */








/* Just like pump_creatures_until_eot(), but can't pump power or toughness, and if the creature already has an effect card from {player,card} and has all the
 * named keywords set, only gives the existing effect a new timestamp instead of making a new one. */


// Just like pump_creatures_until_eot_merge_pt(), but allows an alternate legacy text number.


/* Just like pump_creatures_until_eot(), but can't pump abilities, and sends its effect through pump_until_eot_merge_previous() so they combine into a single
 * effect card.  Requires that the card's effect text uses |n substitutions. */






static int legacy_switch_power_and_toughness(int player, int card, event_t event)
{
  // targets[0] = permanent this is attached to (also in card_instance_t::damage_target_player/card)
  // targets[7].player = pending switch

  card_instance_t *instance = get_card_instance(player, card);
  int p = instance->damage_target_player, c = instance->damage_target_card;
  card_instance_t *aff = get_card_instance(p, c);

  if ((event == EVENT_POWER || event == EVENT_TOUGHNESS) && affect_me(p, c))
  {
    // Mark for revisit
    instance->targets[7].player = 1;

    // Make sure both power and toughness have been calculated
    get_abilities(p, c, EVENT_POWER, -1);
    get_abilities(p, c, EVENT_TOUGHNESS, -1);

    return 0;
  }

  if (event == EVENT_CLEANUP && affect_me(player, card))
  {
    if (get_card_instance(p, c)->ext->special_flags3 & SF3_SWITCH_POW_TGH)
      remove_special_flags3(p, c, SF3_SWITCH_POW_TGH);
    else
      set_special_flags3(p, c, SF3_SWITCH_POW_TGH);
    aff->regen_status |= KEYWORD_RECALC_POWER | KEYWORD_RECALC_TOUGHNESS;
    kill_card(player, card, KILL_EXILE);
    return 0;
  }

  // Any other event
  if (instance->targets[7].player == 1)
  {
    instance->targets[7].player = 0;

    SWAP(aff->power, aff->toughness);
  }

  return 0;
}

int switch_power_and_toughness_until_eot(int player, int card, int t_player, int t_card)
{
  if (get_card_instance(t_player, t_card)->ext->special_flags3 & SF3_SWITCH_POW_TGH)
    remove_special_flags3(t_player, t_card, SF3_SWITCH_POW_TGH);
  else
    set_special_flags3(t_player, t_card, SF3_SWITCH_POW_TGH);
  return create_targetted_legacy_effect_no_repeat(player, card, legacy_switch_power_and_toughness,
                                                  t_player, t_card);
}







int pick_card_from_deck(int player){

	int selected = -1;

	if( player != AI ){
		selected = show_deck( player, deck_ptr[player], 500, "Pick a card", 0, 0x7375B0 );
	}
	else{
		 int i;
		 int *deck = deck_ptr[player];
		 int max_value = -1;
		 int count = count_deck(player);
		 int pos = internal_rand(count);
		 for(i=pos; i<count; i++){
			 card_ptr_t* c = cards_ptr[ cards_data[deck[i]].id ];
			 if( c->ai_base_value > max_value ){
				 max_value = c->ai_base_value;
				 selected = i;
			 }
		 }
	}

	return selected;
}

// Deprecated.  Used only by pattern_of_rebirth_tutor() (called from Pattern of Rebirth at 0x1203847).
int tutor(int player, int type, int (*func_ptr)(int, int),  int tutor_location  ){
	int card_added = -1;
	int i=0;
	while(i++ < 50){
		int selected = pick_card_from_deck(player);
		if( selected && selected != -1){
			int *deck = deck_ptr[player];
			if (is_what(-1, deck[selected], type)){
				card_added = add_card_to_hand(player, deck[selected] );
				remove_card_from_deck( player, selected );
				if( func_ptr(player, card_added) ){
					if( tutor_location == TUTOR_PLAY ){
						put_into_play(player, card_added);
					}
					else if( tutor_location == TUTOR_GRAVE ){
						kill_card(player, card_added, KILL_DESTROY);
					}
					break;
				}
				else{
					put_on_top_of_deck(player, card_added);
				}
			}
		}
		else{
			break;
		}
	}
	shuffle(player);
	if( tutor_location == TUTOR_DECK ){
		put_on_top_of_deck(player, card_added);
	}
	return card_added;
}

int legacy_effect_activated(int player, int card, event_t event){
	card_instance_t* instance = get_card_instance(player, card);
	if (instance->info_slot > 4096){
		return ((int (*)(int, int, event_t))instance->info_slot)(player, card, event);
	} else {
		// Original Nafs Asp legagy

		if (event == EVENT_CAN_ACTIVATE){
			return has_mana(player, COLOR_ANY, 1);
		} else if (event == EVENT_ACTIVATE){
			if (affect_me(player, card) && has_mana(player, COLOR_ANY, 1)){
				charge_mana(player, COLOR_COLORLESS, 1);
			}
		}
		else if (event == EVENT_RESOLVE_ACTIVATION){
			kill_card(player, instance->parent_card, KILL_EXILE);
		}
		else if ((trigger_condition == TRIGGER_DRAW_PHASE || event == EVENT_SHOULD_AI_PLAY)
				 && affect_me(player, card)
				 && current_turn == player
				 && reason_for_trigger_controller == player
				 && get_card_instance(player, card)->info_slot == 0){
			if (event == EVENT_TRIGGER){
				event_result |= 2;
			} else if (event == EVENT_RESOLVE_TRIGGER || event == EVENT_SHOULD_AI_PLAY){
				load_text(0, "NAFS_ASP");
				do_dialog(player, player, card, -1, -1, text_lines[0], 0);
				lose_life(player, 1);
				kill_card(player, card, KILL_BURY);
			}
		} else if (event == EVENT_CLEANUP || event == EVENT_SHOULD_AI_PLAY){
			if (ai_is_speculating == 1){
				--life[player];
			}
			instance->info_slot = 0;
		}
		return 0;
	}
}

int create_legacy_effect(int player, int card, int (*func_ptr)(int, int, event_t) ){
	int result = create_legacy_effect_exe(player, card, LEGACY_EFFECT_CUSTOM, -1, -1);
	card_instance_t *instance = get_card_instance(player, result);
	instance->info_slot = (int) func_ptr;
	return result;
}

int create_legacy_activate(int player, int card, int (*func_ptr)(int, int, event_t) ){
	int result = create_legacy_effect_exe(player, card, LEGACY_EFFECT_ACTIVATED, -1, -1);
	card_instance_t *instance = get_card_instance(player, result);
	instance->info_slot = (int)func_ptr;
	return result;
}

int create_targetted_legacy_activate(int player, int card, int (*func_ptr)(int, int, event_t), int t_player, int t_card){
	int result = create_legacy_effect_exe(player, card, LEGACY_EFFECT_ACTIVATED, t_player, t_card);
	card_instance_t *instance = get_card_instance(player, result);
	instance->info_slot = (int)func_ptr;
	return result;
}

int create_targetted_legacy_effect(int player, int card, int (*func_ptr)(int, int, event_t), int target_player_id, int target_card ){
	int result = create_legacy_effect_exe(player, card, LEGACY_EFFECT_CUSTOM, target_player_id, target_card);
	card_instance_t *instance = get_card_instance(player, result);
	instance->info_slot =  (int) func_ptr;
	instance->targets[0].player = target_player_id;
	instance->targets[0].card = target_card;
	instance->number_of_targets = 1;
	return result;
}

/* If there's already an effect card attached to {target_player_id,target_card} with func_ptr created by {player,card}, give it a new timestamp and return it;
 * else return -1. */
int find_repeat_legacy_effect(int player, int card, CardFunction func_ptr, int target_player_id,
                              int target_card)
{
  target_t found = find_repeat_legacy_effect_alt(player, card, func_ptr, target_player_id,
                                                 target_card, -1, player);
  return found.card;
}

/* Just like create_targetted_legacy_effect(), but if there's an effect from the same source with the same func_ptr already affecting the card, just gives that
 * a new timestamp instead. */
int create_targetted_legacy_effect_no_repeat(int player, int card, int (*func_ptr)(int, int, event_t), int target_player_id, int target_card){
	int eff = find_repeat_legacy_effect(player, card, func_ptr, target_player_id, target_card);
	if (eff != -1){
		return eff;
	} else {
		return create_targetted_legacy_effect(player, card, func_ptr, target_player_id, target_card);
	}
}

// Creates a legacy effect from (player,card) under control of 1-player.
int create_legacy_effect_for_opponent(int player, int card, int (*func_ptr)(int, int, event_t), int t_player, int t_card)
{
  int card_added = add_card_to_hand(1-player, get_card_instance(player, card)->internal_card_id);
  ASSERT(card_added != -1);
  card_instance_t* temp = get_card_instance(1-player, card_added);

  // Make sure it's neither in hand nor on battlefield when it gets obliterated
  temp->state = (temp->state | STATE_INVISIBLE) & ~STATE_IN_PLAY;
  --hand_count[1-player];

  /* So all the values on the created legacy are correct - create_legacy_effect copies card_color, mana_color, color, STATUS_SLEIGHTED, STATUS_HACKED,
   * color_id[0..5], and hack_mode[0..5] from (player,card); and sets legacy->initial_color to (player,card)->color. */
  memcpy(temp, get_card_instance(player, card), sizeof(card_instance_t));

  // Still neither in hand nor on battlefield
  temp->state = (temp->state | STATE_INVISIBLE) & ~STATE_IN_PLAY;

  int legacy = create_legacy_effect(1-player, card_added, func_ptr);
  obliterate_card(1-player, card_added);

  // Fixup created legacy to point to original card as source
  card_instance_t* leg = get_card_instance(1-player, legacy);
  leg->damage_source_player = player;
  leg->damage_source_card = card;

  // And same art version
  leg->display_pic_num = get_card_image_number(leg->display_pic_csv_id, player, card);

  // Redundant attachment in targets[0], per create_targetted_legacy_effect()
  if (t_player >= 0)
	{
	  leg->targets[0].player = t_player;
	  leg->targets[0].card = t_card;
	  leg->number_of_targets = 1;
	}

  return legacy;
}

/* If this effect is controlled by a different player than the card it's attached to, then transfer control and ownership of the effect.  Calling effect should
 * return 0 if this returns true. */
int effect_follows_control_of_attachment(int player, int card, int event)
{
  if (event == EVENT_STATIC_EFFECTS)
	{
	  card_instance_t* instance = get_card_instance(player, card);
	  int p = instance->damage_target_player, c = instance->damage_target_card;

	  if (p != player)
		{
		  // Transfer control and ownership of effect
		  int leg = create_targetted_legacy_effect(p, 0, &empty, p, c);
		  card_instance_t* legacy = get_card_instance(p, leg);
		  int old_timestamp = legacy->timestamp;
		  memcpy(legacy, instance, sizeof(card_instance_t));
		  legacy->timestamp = old_timestamp;
		  legacy->state ^= STATE_OWNED_BY_OPPONENT;
		  kill_card(player, card, KILL_EXILE);
		  return 1;
		}
	}
  return 0;
}


int create_my_legacy(int player, int card, int (*func_ptr)(int, int, event_t)){
	int result = create_legacy_effect_exe(player, card, LEGACY_EFFECT_CUSTOM, -1, -1);
	card_instance_t *instance = get_card_instance(player, result);
	instance->info_slot =  (int) func_ptr;
	instance->targets[0].player = player;
	instance->targets[0].card = card;
	instance->number_of_targets = 1;
	// add_status(player, result, STATUS_INVISIBLE_FX);
	return result;
}

void add_legacy_effect_to_all(int player, int card, int (*func_ptr)(int, int, event_t), int t_player, test_definition_t *this_test){
	int i;
	for(i=0; i<2; i++){
		if( i == t_player || t_player == 2 ){
			int count = active_cards_count[i]-1;
			while( count > -1 ){
					if( in_play(i, count) && new_make_test_in_play(i, count, -1, this_test) ){
						int result = create_legacy_effect_exe(player, card, LEGACY_EFFECT_CUSTOM, i, count);
						card_instance_t *instance = get_card_instance(player, result);
						instance->info_slot =  (int) func_ptr;
						instance->targets[0].player = i;
						instance->targets[0].card = count;
					}
					count--;
			}
		}
	}
}

int resolve_graveyard_trigger(int player, int card, event_t event){
	card_instance_t *instance= get_card_instance(player, card);
	int good = 0;
	if( trigger_condition == TRIGGER_GRAVEYARD_FROM_PLAY && player == reason_for_trigger_controller){
		if( affect_me(player, card ) ){
			if( instance->targets[11].player > 0 ){
				good = 1;
			}
		}
	}

	if( good == 1 ){
		if(event == EVENT_TRIGGER){
			//Make all trigges mandatoy for now
			event_result |= RESOLVE_TRIGGER_MANDATORY;
		}
		else if(event == EVENT_RESOLVE_TRIGGER){
			return 1;
		}
	}
	return 0;
}

void does_not_untap(int player, int card, event_t event){
	if( current_phase == PHASE_UNTAP && event == EVENT_UNTAP && affect_me(player, card) ){
		card_instance_t *instance= get_card_instance(player, card);
		instance->untap_status &= ~3;
	}
}

static int effect_does_not_untap(int player, int card, event_t event)
{
  /* targets[0]: Permanent this is attached to (also in card_instance_t::damage_target_player/card)
   * targets[1]: Source of effect.
   * targets[2].player: Number of untap steps to skip.
   * targets[2].card: mode, per ednt_flags_t. */

  card_instance_t *instance = get_card_instance(player, card);

  int flags = instance->targets[2].card;

  int t_player = instance->damage_target_player;
  int t_card = instance->damage_target_card;

  if (flags && (flags & EDNT_TAP_AT_END_OF_COMBAT))
  {
    if (end_of_combat_trigger(player, card, event, 2))
    {
      tap_card(t_player, t_card);
      instance->targets[2].card &= ~EDNT_TAP_AT_END_OF_COMBAT;
    }
  }

  if (!flags ||
      (flags && (flags & EDNT_CHECK_ORIGINAL_CONTROLLER) &&
       current_turn == instance->targets[3].player) ||
      (flags && (flags & (EDNT_REMAIN_TAPPED_UNTIL_SOURCE_LEAVES_PLAY | EDNT_WONT_UNTAP_ANYMORE))))
  {
    does_not_untap(t_player, t_card, event);
  }

  if (event == EVENT_END_OF_UNTAP_STEP && affected_card_controller == t_player)
  {
    if (!flags || (flags && !(flags & (EDNT_REMAIN_TAPPED_UNTIL_SOURCE_LEAVES_PLAY |
                                       EDNT_WONT_UNTAP_ANYMORE))))
    {
      if (instance->targets[2].player > 1)
      {
        --instance->targets[2].player;
      }
      else
      {
        remove_special_flags2(t_player, t_card, SF2_COULD_NOT_UNTAP);
        kill_card(player, card, KILL_EXILE);
      }
    }
  }

  if (flags && (flags & EDNT_REMAIN_TAPPED_UNTIL_SOURCE_LEAVES_PLAY) &&
      instance->targets[2].player > -1)
  {
    leaves_play(instance->targets[1].player, instance->targets[1].card, event, 0, 0);
    if (event == EVENT_LEAVES_PLAY_ABILITY)
    {
      remove_special_flags2(t_player, t_card, SF2_COULD_NOT_UNTAP);
      kill_card(player, card, KILL_EXILE);
    }
  }
  if (event == EVENT_CARDCONTROLLED &&
      affect_me(instance->targets[1].player, instance->targets[1].card))
  {
    kill_card(player, card, KILL_EXILE);
  }

  return 0;
}

int does_not_untap_effect(int player, int card, int t_player, int t_card, int mode, int turns){
	if( mode & EDNT_TAP_TARGET ){
		tap_card(t_player, t_card);
	}
	int legacy = create_targetted_legacy_effect(player, card, &effect_does_not_untap, t_player, t_card);
	set_special_flags2(t_player, t_card, SF2_COULD_NOT_UNTAP);
	card_instance_t *instance = get_card_instance( player, legacy );

	instance->targets[1].player = player;
	instance->targets[1].card = card;
	instance->number_of_targets = 2;

	instance->targets[2].card = mode;
	instance->targets[2].player = turns;

	/*
	5/1/2007: "Your next untap..." refers to the player who controls [this permanent] when the triggered ability resolves.
	9/22/2011: If another player controls [this permanent] during your next untap step, the part of the effect that keeps it
				from untapping does nothing. It won't try to apply during a future untap step.
	*/
	instance->targets[3].player = t_player;

	return legacy;
}

static int dnuimt_legacy(int player, int card, event_t event){

	card_instance_t *instance = get_card_instance(player, card);

	if( instance->damage_target_player > -1 && instance->targets[1].player > -1){
		if( in_play(instance->targets[1].player, instance->targets[1].card) && is_tapped(instance->targets[1].player, instance->targets[1].card) ){
			does_not_untap(instance->damage_target_player, instance->damage_target_card, event);
		}
		else{
			kill_card(player, card, KILL_EXILE);
		}
	}

	return 0;
}

int does_not_untap_until_im_tapped(int player, int card, int t_player, int t_card){
	int legacy = create_targetted_legacy_effect(player, card, &dnuimt_legacy, t_player, t_card);
	card_instance_t *leg = get_card_instance(player, legacy);
	leg->targets[1].player = player;
	leg->targets[1].card = card;
	leg->number_of_targets = 2;
	return legacy;
}

static int ability_doesnt_untap_while_has_a_counter_and_remove_a_counter_at_upkeep(int player, int card,
                                                                            event_t event)
{
  counter_t type;
  card_instance_t *instance;
  int result;

  instance = get_card_instance(player, card);
  type = instance->targets[1].card;
  if (event == EVENT_UNTAP)
  {
    result =
        count_counters((int)instance->damage_target_player, instance->damage_target_card, type);
    if (result != 0)
    {
      does_not_untap((int)instance->damage_target_player, instance->damage_target_card,
                     EVENT_UNTAP);
    }
    if (trigger_condition != 0xc9)
    {
      return 0;
    }
  }
  else if (trigger_condition != 0xc9)
    goto LAB_02695358;
  if (((card == affected_card) && (player == affected_card_controller)) &&
      (result =
           count_counters((int)instance->damage_target_player, instance->damage_target_card, type),
       result != 0))
  {
    upkeep_trigger_ability_mode(player, card, event, (int)instance->damage_target_player,
                                RESOLVE_TRIGGER_MANDATORY);
  }
LAB_02695358:
  if (event != EVENT_UPKEEP_TRIGGER_ABILITY)
  {
    return 0;
  }
  remove_counters(player, card, (int)instance->damage_target_player,
                              instance->damage_target_card, type, 1);
  return 0;
}
void gains_doesnt_untap_while_has_a_counter_and_remove_a_counter_at_upkeep(int src_player, int src_card, int t_player, int t_card, counter_t counter_type)
{
  int legacy = create_targetted_legacy_effect(src_player, src_card, ability_doesnt_untap_while_has_a_counter_and_remove_a_counter_at_upkeep, t_player, t_card);
  get_card_instance(src_player, legacy)->targets[1].card = counter_type;
}



int select_card_from_graveyard(int player, int card, int whose_grave, int type ){
	int card_added = -1;
	while(1){
		int selected = show_deck( player, get_grave(whose_grave), 500, "Pick a card", 0, 0x7375B0 );
		if( selected != -1){
			const int *grave = get_grave(whose_grave);
			if (is_what(-1, grave[selected], type)){
				card_added =  add_card_to_hand(player, grave[selected] );

				card_instance_t *instance = get_card_instance(player, card);
				instance->targets[0].player = player;
				instance->targets[1].player = whose_grave;
				instance->targets[0].card = card_added;
				instance->number_of_targets = 1;
				instance->info_slot = selected;

				card_instance_t *target = get_card_instance(player, card_added);
				target->state |= STATE_INVISIBLE;
				--hand_count[player];

				return card_added;
			}
		}
		else{
			break;
		}
	}
	return card_added;
}









/* These variants should be called instead of charge_mana()/charge_mana_multi() during EVENT_RESOLVE_TRIGGER, EVENT_RESOLVE_SPELL, and RESOLVE_ACTIVATION, so
 * the prompt is correct. */










// An unusual case - the card is worded to be an activated, not a triggered ability, but we're charging for it during resolution.






int eot_trigger(int player, int card, event_t event){
	if(trigger_condition == TRIGGER_EOT &&  affect_me(player, card ) && reason_for_trigger_controller == player &&
		(is_what(player, card, TYPE_EFFECT) || ! is_humiliated(player, card, AL_TRIG)) ){
		if(event == EVENT_TRIGGER){
			event_result |= 2;
		}
		else if(event == EVENT_RESOLVE_TRIGGER){
			return 1;
		}
	}
	return 0;
}

int eot_trigger_mode(int player, int card, event_t event, int t_player, resolve_trigger_t trig_mode){
	if( (current_turn == t_player || t_player == ANYBODY) && trigger_condition == TRIGGER_EOT &&  affect_me(player, card ) && reason_for_trigger_controller == player &&
		(is_what(player, card, TYPE_EFFECT) || ! is_humiliated(player, card, AL_TRIG))
	  ){
		if(event == EVENT_TRIGGER){
			event_result |= (trig_mode == RESOLVE_TRIGGER_DUH ? (duh_mode(player) ? RESOLVE_TRIGGER_MANDATORY : RESOLVE_TRIGGER_OPTIONAL) : trig_mode);
		}
		else if(event == EVENT_RESOLVE_TRIGGER){
				return 1;
		}
	}
	return 0;
}

void all_must_attack_if_able(int player, event_t event, int subtype)
{
	event_flags |= EA_FORCE_ATTACK;	// just in case it's not set in ct_all.csv - it won't be for effect cards

	if( event == EVENT_MUST_ATTACK && current_turn == player && !forbid_attack ){
		int c;
		for(c=0; c<active_cards_count[player]; c++){
			if( in_play(player, c) && is_what(player, c, TYPE_CREATURE) && (subtype < 0 || has_subtype(player, c, subtype)) ){
				attack_if_able(player, c, event);
			}
		}
	}
}

int cycling(int player, int card, event_t event, int colorless, int black, int blue, int green,
            int red, int white)
{
  if (check_battlefield_for_id(player, CARD_ID_NEW_PERSPECTIVES) && hand_count[player] > 6)
    colorless = black = blue = green = red = white = 0;

  if (event == EVENT_CAN_ACTIVATE_FROM_HAND)
  {
    colorless -= (2 * count_cards_by_id(player, CARD_ID_FLUCTUATOR));
    colorless += (2 * count_cards_by_id(player, CARD_ID_SUPPRESSION_FIELD));

    if (colorless < 0)
    {
      colorless = 0;
    }
    if (has_mana_multi(player, colorless, black, blue, green, red, white))
    {
      return 1;
    }
  }
  else if (event == EVENT_ACTIVATE_FROM_HAND)
  {
    colorless -= (2 * count_cards_by_id(player, CARD_ID_FLUCTUATOR));
    colorless += (2 * count_cards_by_id(player, CARD_ID_SUPPRESSION_FIELD));

    if (colorless < 0)
    {
      colorless = 0;
    }

    charge_mana_multi(player, colorless, black, blue, green, red, white);
    if (spell_fizzled != 1)
    {
      card_is_being_cycled = 1;
      discard_card(player, card);
      card_is_being_cycled = 0;
      return 2;
    }
  }
  else if (event == EVENT_RESOLVE_ACTIVATION_FROM_HAND && spell_fizzled != 1)
  {
    draw_a_card(player);
  }
  return 0;
}

int cycling_hybrid(int player, int card, event_t event, int amt_colored, color_t first_color,
                   color_t second_color, int amt_colorless)
{
  if (check_battlefield_for_id(player, CARD_ID_NEW_PERSPECTIVES) && hand_count[player] >= 7)
  {
    amt_colorless = amt_colored = 0;
    first_color = second_color = COLOR_COLORLESS;
  }

  if (event == EVENT_CAN_ACTIVATE_FROM_HAND || event == EVENT_ACTIVATE_FROM_HAND)
  {
    amt_colorless += 2 * count_cards_by_id(player, CARD_ID_SUPPRESSION_FIELD);
    if (amt_colorless > 0)
    {
      amt_colorless -= 2 * count_cards_by_id(player, CARD_ID_FLUCTUATOR);
      amt_colorless = MAX(0, amt_colorless);
    }
  }

  if (event == EVENT_CAN_ACTIVATE_FROM_HAND)
    return has_mana_hybrid(player, amt_colored, first_color, second_color, amt_colorless) ? 1 : 0;

  if (event == EVENT_ACTIVATE_FROM_HAND &&
      charge_mana_hybrid(player, card, amt_colored, first_color, second_color, amt_colorless))
  {
    card_is_being_cycled = 1;
    discard_card(player, card);
    card_is_being_cycled = 0;
    return 2;
  }

  if (event == EVENT_RESOLVE_ACTIVATION_FROM_HAND && spell_fizzled != 1)
    draw_a_card(player);

  return 0;
}

// Keeps the cycling abilities with odd activation costs in close proximity to normal cycling implementation.
int cycling_special_cost(int player, int card, event_t event, int mode)
{
  // Mode == 1: sacrifice a land
  // Mode == 2: pay 2 life

  int amt_colorless = 0;
  if (check_battlefield_for_id(player, CARD_ID_NEW_PERSPECTIVES) && hand_count[player] >= 7)
  {
    mode = 0;
  }

  if (event == EVENT_CAN_ACTIVATE_FROM_HAND || event == EVENT_ACTIVATE_FROM_HAND)
  {
    amt_colorless += 2 * count_cards_by_id(player, CARD_ID_SUPPRESSION_FIELD);
    if (amt_colorless > 0)
    {
      amt_colorless -= 2 * count_cards_by_id(player, CARD_ID_FLUCTUATOR);
      amt_colorless = MAX(0, amt_colorless);
    }
  }

  if (event == EVENT_CAN_ACTIVATE_FROM_HAND)
  {
    if (amt_colorless > 0 && !has_mana(player, COLOR_ANY, amt_colorless))
      return 0;

    switch (mode)
    {
    case 1:
      return can_sacrifice_type_as_cost(player, 1, TYPE_LAND) ? 1 : 0;
    case 2:
      return can_pay_life(player, 2) ? 1 : 0;
    }

    return 1;
  }

  if (event == EVENT_ACTIVATE_FROM_HAND)
  {
    if (amt_colorless > 0)
    {
      charge_mana(player, COLOR_COLORLESS, amt_colorless);
      if (cancel == 1)
        return 0;
    }

    switch (mode)
    {
    case 1:
      if (!controller_sacrifices_a_permanent(player, card, TYPE_LAND, 0))
      {
        cancel = 1;
        return 0;
      }
      break;

    case 2:
      lose_life(player, 2);
      break;
    }

    card_is_being_cycled = 1;
    discard_card(player, card);
    card_is_being_cycled = 0;
    return 2;
  }

  if (event == EVENT_RESOLVE_ACTIVATION_FROM_HAND && spell_fizzled != 1)
    draw_a_card(player);

  return 0;
}

int choose_a_land(int player, int ai_choice){
	int result = -1;
	while(result == -1){
		result = choose_a_color_exe(player, "Choose a basic land type", 0, ai_choice, COLOR_TEST_ANY_COLORED);
	}
	return result;
}














int upkeep_trigger_mode(int player, int card, event_t event, resolve_trigger_t trigger_mode)
{
	return upkeep_trigger_mode_impl(player, card, event, trigger_mode, 0);
}

int upkeep_trigger_no_check(int player, int card, event_t event)
{
	//This won't check for cards that skips or adds Upkeep Phases, so it should be done by the card calling this functions.
	//Useful for cards that create an effect card that will trigger on next Upkeep.
	//Example of usage: "cantrip" in "ice_age.c"
	return upkeep_trigger_mode_impl(player, card, event, RESOLVE_TRIGGER_MANDATORY, 1);
}

int upkeep_trigger(int player, int card, event_t event)
{
  return upkeep_trigger_mode_impl(player, card, event, RESOLVE_TRIGGER_MANDATORY, 0);
}

int global_enchantment(int player, int card, event_t event){
	if( event == EVENT_SHOULD_AI_PLAY ){
		ai_modifier += (player == HUMAN) ? -8 : 8;
	}
	else if( event == EVENT_CAN_CAST ){
		return 1;
	}
	return 0;
}

int enchant_world(int player, int card, event_t event)
{
  if (event == EVENT_SHOULD_AI_PLAY)
	ai_modifier += (player == HUMAN) ? -8 : 8;

  if (event == EVENT_CAN_CAST)
	return 1;

  if (trigger_condition == TRIGGER_COMES_INTO_PLAY && affect_me(player, card) && reason_for_trigger_controller == player
	  && trigger_cause_controller == player && trigger_cause == card)
	{
	  if (event == EVENT_TRIGGER)
		event_result |= RESOLVE_TRIGGER_MANDATORY;

	  if (event == EVENT_RESOLVE_TRIGGER)
		{
		  int p, c;
		  card_instance_t* inst;
		  for (p = 0; p <= 1; ++p)
			for (c = 0; c < active_cards_count[p]; ++c)
			  if ((inst = in_play(p, c)) && !(c == card && p == player) && cards_data[inst->internal_card_id].subtype == SUB_ENCHANT_WORLD)
				kill_card(p, c, KILL_STATE_BASED_ACTION);
		}
	}

  return 0;
}



static int split_cards_color_table(int player, int card){
	int csvid = player != -1 ? get_id(player, card) : cards_data[card].id;
	int half_played = get_card_instance(player, card)->info_slot;
	int result = 0;
	if( half_played ){
		switch( csvid ){
				// Invasion
				case CARD_ID_STAND_DELIVER: //Not yet coded
					result = (half_played & 1) ? COLOR_TEST_WHITE : COLOR_TEST_BLUE;
					break;
				case CARD_ID_SPITE_MALICE:
					result = (half_played & 1) ? COLOR_TEST_BLUE : COLOR_TEST_BLACK;
					break;
				case CARD_ID_PAIN_SUFFERING: //Not yet coded
					result = (half_played & 1) ? COLOR_TEST_BLACK : COLOR_TEST_RED;
					break;
				case CARD_ID_ASSAULT_BATTERY:
					result = (half_played & 1) ? COLOR_TEST_RED : COLOR_TEST_GREEN;
					break;
				case CARD_ID_WAX_WANE:
					result = (half_played & 1) ? COLOR_TEST_GREEN : COLOR_TEST_WHITE;
					break;
				// Apocalypse
				case CARD_ID_FIRE_ICE:
					result = (half_played & 1) ? COLOR_TEST_RED : COLOR_TEST_BLUE;
					break;
				case CARD_ID_ILLUSION_REALITY: //Not yet coded
					result = (half_played & 1) ? COLOR_TEST_BLUE : COLOR_TEST_GREEN;
					break;
				case CARD_ID_LIFE_DEATH:
					result = (half_played & 1) ? COLOR_TEST_GREEN : COLOR_TEST_BLACK;
					break;
				case CARD_ID_NIGHT_DAY: //Not yet coded
					result = (half_played & 1) ? COLOR_TEST_BLACK : COLOR_TEST_WHITE;
					break;
				case CARD_ID_ORDER_CHAOS:
					result = (half_played & 1) ? COLOR_TEST_WHITE : COLOR_TEST_RED;
					break;
				// Dissension
				case CARD_ID_BOUND_DETERMINED:
					result = (half_played & 1) ? COLOR_TEST_BLACK | COLOR_TEST_GREEN : COLOR_TEST_BLUE | COLOR_TEST_GREEN;
					break;
				case CARD_ID_CRIME_PUNISHMENT:
					result = (half_played & 1) ? COLOR_TEST_BLACK | COLOR_TEST_WHITE : COLOR_TEST_BLACK | COLOR_TEST_GREEN;
					break;
				case CARD_ID_HIDE_SEEK:
					result = (half_played & 1) ? COLOR_TEST_RED | COLOR_TEST_WHITE : COLOR_TEST_BLACK | COLOR_TEST_WHITE;
					break;
				case CARD_ID_HIT_RUN:
					result = (half_played & 1) ? COLOR_TEST_BLACK |  COLOR_TEST_RED : COLOR_TEST_GREEN | COLOR_TEST_RED;
					break;
				case CARD_ID_ODDS_ENDS:
					result = (half_played & 1) ? COLOR_TEST_BLUE | COLOR_TEST_RED : COLOR_TEST_RED | COLOR_TEST_WHITE;
					break;
				case CARD_ID_PURE_SIMPLE:
					result = (half_played & 1) ? COLOR_TEST_GREEN | COLOR_TEST_RED : COLOR_TEST_GREEN | COLOR_TEST_WHITE;
					break;
				case CARD_ID_RESEARCH_DEVELOPMENT:
					result = (half_played & 1) ? COLOR_TEST_BLUE | COLOR_TEST_GREEN : COLOR_TEST_BLUE | COLOR_TEST_RED;
					break;
				case CARD_ID_RISE_FALL:
					result = (half_played & 1) ? COLOR_TEST_BLACK | COLOR_TEST_BLUE : COLOR_TEST_BLACK | COLOR_TEST_RED;
					break;
				case CARD_ID_SUPPLY_DEMAND:
					result = (half_played & 1) ?COLOR_TEST_GREEN | COLOR_TEST_WHITE : COLOR_TEST_BLUE | COLOR_TEST_WHITE;
					break;
				case CARD_ID_TRIAL_ERROR:
					result = (half_played & 1) ? COLOR_TEST_BLUE | COLOR_TEST_WHITE : COLOR_TEST_BLACK | COLOR_TEST_BLUE;
					break;
				// Dragon's Maze
				case CARD_ID_ALIVE_WELL:
					result = (half_played & 1) && (half_played & 2) ? COLOR_TEST_GREEN | COLOR_TEST_WHITE : ((half_played & 1) ? COLOR_TEST_GREEN : COLOR_TEST_WHITE);
					break;
				case CARD_ID_ARMED_DANGEROUS:
					result = (half_played & 1) && (half_played & 2) ? COLOR_TEST_GREEN | COLOR_TEST_RED : ((half_played & 1) ? COLOR_TEST_RED : COLOR_TEST_GREEN);
					break;
				case CARD_ID_BECK_CALL:
					result = (half_played & 1) && (half_played & 2) ? COLOR_TEST_BLUE | COLOR_TEST_GREEN | COLOR_TEST_WHITE :
												((half_played & 1) ? COLOR_TEST_BLUE | COLOR_TEST_GREEN : COLOR_TEST_BLUE | COLOR_TEST_WHITE);
					break;
				case CARD_ID_BREAKING_ENTERING:
					result = (half_played & 1) && (half_played & 2) ? COLOR_TEST_BLUE | COLOR_TEST_BLACK | COLOR_TEST_RED :
												((half_played & 1) ? COLOR_TEST_BLUE | COLOR_TEST_BLACK : COLOR_TEST_BLACK | COLOR_TEST_RED);
					break;
				case CARD_ID_CATCH_RELEASE:
					result = (half_played & 1) && (half_played & 2) ? COLOR_TEST_BLUE | COLOR_TEST_RED | COLOR_TEST_WHITE :
												((half_played & 1) ? COLOR_TEST_BLUE | COLOR_TEST_RED : COLOR_TEST_RED | COLOR_TEST_WHITE);
					break;
				case CARD_ID_DOWN_DIRTY:
					result = (half_played & 1) && (half_played & 2) ? COLOR_TEST_GREEN | COLOR_TEST_BLACK : ((half_played & 1) ? COLOR_TEST_BLACK : COLOR_TEST_GREEN);
					break;
				case CARD_ID_FAR_AWAY:
					result = (half_played & 1) && (half_played & 2) ? COLOR_TEST_BLACK | COLOR_TEST_BLUE : ((half_played & 1) ? COLOR_TEST_BLUE : COLOR_TEST_BLACK);
					break;
				case CARD_ID_FLESH_BLOOD:
					result = (half_played & 1) && (half_played & 2) ? COLOR_TEST_BLACK | COLOR_TEST_GREEN | COLOR_TEST_RED :
												((half_played & 1) ? COLOR_TEST_BLACK | COLOR_TEST_GREEN : COLOR_TEST_GREEN | COLOR_TEST_RED);
					break;
				case CARD_ID_GIVE_TAKE:
					result = (half_played & 1) && (half_played & 2) ? COLOR_TEST_BLUE | COLOR_TEST_GREEN : ((half_played & 1) ? COLOR_TEST_GREEN : COLOR_TEST_BLUE);
					break;
				case CARD_ID_PROFIT_LOSS:
					result = (half_played & 1) && (half_played & 2) ? COLOR_TEST_BLACK | COLOR_TEST_WHITE : ((half_played & 1) ? COLOR_TEST_WHITE : COLOR_TEST_BLACK);
					break;
				case CARD_ID_PROTECT_SERVE:
					result = (half_played & 1) && (half_played & 2) ? COLOR_TEST_BLUE | COLOR_TEST_WHITE : ((half_played & 1) ? COLOR_TEST_WHITE : COLOR_TEST_BLUE);
					break;
				case CARD_ID_READY_WILLING:
					result = (half_played & 1) && (half_played & 2) ? COLOR_TEST_BLACK | COLOR_TEST_GREEN | COLOR_TEST_WHITE :
												((half_played & 1) ? COLOR_TEST_GREEN | COLOR_TEST_WHITE : COLOR_TEST_BLACK | COLOR_TEST_WHITE);
					break;
				case CARD_ID_TOIL_TROUBLE:
					result = (half_played & 1) && (half_played & 2) ? COLOR_TEST_BLACK | COLOR_TEST_RED : ((half_played & 1) ? COLOR_TEST_BLACK : COLOR_TEST_RED);
					break;
				case CARD_ID_TURN_BURN:
					result = (half_played & 1) && (half_played & 2) ? COLOR_TEST_BLUE | COLOR_TEST_RED : ((half_played & 1) ? COLOR_TEST_BLUE : COLOR_TEST_RED);
					break;
				case CARD_ID_WEAR_TEAR:
					result = (half_played & 1) && (half_played & 2) ? COLOR_TEST_WHITE | COLOR_TEST_RED : ((half_played & 1) ? COLOR_TEST_RED : COLOR_TEST_WHITE);
					break;
				default:
					break;
		}
	}
	return result;
}





// Use this only for checking few cards in a row, otherwise store get_global_color_hack in a variable and use get_color_by_internal_id_no_hack


// A shim between get_color() and get_color_by_internal_id().




int pick_creature_to_regen(target_definition_t td){
	if( pick_target(&td, "TARGET_CREATURE_TO_REGEN") ){
		card_instance_t *instance = get_card_instance(td.player, td.card);
		while( spell_fizzled != 1 ){
			card_instance_t *creature = get_card_instance(instance->targets[0].player, instance->targets[0].card);
			if( creature->kill_code == KILL_DESTROY && !(creature->token_status & STATUS_CANNOT_REGENERATE) ){
				break;
			}
			pick_target(&td, "TARGET_CREATURE_TO_REGEN");
		}
		if( spell_fizzled != 1 ){
			return 1;
		}
	}
	return 0;
}



static int split_cards_cmc_table(int player, int card){
	// For putting things simple, we'll always assume that the CMC we get from any other zone aside from STATE_IN_PLAY is the lesser.
	int csvid = player != -1 ? get_id(player, card) : cards_data[card].id;
	int half_played = player != -1 ? get_card_instance(player, card)->info_slot : 0;
	return split_cards_cmc_table_by_id(csvid, half_played);
}

int get_cmc_by_id(int id)
{

  if (has_no_mana_cost(-1, get_internal_card_id_from_csv_id(id)))
  {
    return 0;
  }

  card_ptr_t *c = cards_ptr[id];

  int result = c->req_black + c->req_blue + c->req_green + c->req_red + c->req_white;
  int cless = c->req_colorless;

  if (cless == 40)
  {
    cless = 0;
  }
  result += cless;
  int result_split_card = split_cards_cmc_table_by_id(id, 0);
  return result_split_card >= 0 ? result_split_card : result;
}





int paying_mana(void){
	return !unknown62BCEC || needed_mana_colors;
}

// A permanent legacy effect that does nothing.
int empty(int player, int card, event_t event){
	/* (While this sometimes has an address assigned in ManalinkEh.asm, it's not referenced by ct_all.csv or the exe; it's used as a placeholder when an address
	 * is freed.) */
	return 0;
}

// A legacy effect that does nothing except get removed at end of turn.
int empty_until_eot(int player, int card, event_t event)
{
  if (event == EVENT_CLEANUP)
	kill_card(player, card, KILL_EXILE);

  return 0;
}













// Just like reanimate_permanent(), but attach an effect with function effect_fn to the animated permanent before putting it into play.


// Just like reanimate_permanent_with_effect(), but also add a counter to it before putting it into play.


// Just like reanimate_permanent(), but call an artibtrary function on {player,card} just before it's put on the battlefield.
// (source_of_effect_player, source_of_effect_card, player_who_control_reanimated_permanent, reanimated_card)


/* The unused eight-argument predecessor was retired; native mass reanimation uses the five-argument recovered worker. */





// did this card just go to a graveyard from play?
int graveyard_from_play(int player, int card, event_t event)
{
  if (event == EVENT_GRAVEYARD_FROM_PLAY)
  {
    if (affect_me(player, card) && in_play(player, card))
    {
      card_instance_t *instance = get_card_instance(player, card);
      if (instance->kill_code != KILL_EXILE && instance->kill_code != 0)
      {
        return !(instance->ext->special_flags2 & SF2_WILL_BE_EXILED_IF_PUTTED_IN_GRAVE);
      }
    }
  }
  return 0;
}

// Should be called only before entering or as enters the battlefield


// Should be called if changing types after already being in play.


void cloning_card(int player, int card, event_t event)
{
  if (event == EVENT_CHANGE_TYPE && affect_me(player, card)
	  && !(land_can_be_played & LCBP_DURING_EVENT_CHANGE_TYPE_SECOND_PASS))
	{
	  card_instance_t* instance = get_card_instance(player, card);
	  if (instance->targets[12].card != -1)
		event_result = instance->targets[12].card;
	}
}

// t_player and t_card only need to be initialized during EVENT_RESOLVE_SPELL.
void enters_the_battlefield_as_copy_of(int player, int card, event_t event, int t_player, int t_card)
{
  cloning_card(player, card, event);

  if (event == EVENT_RESOLVE_SPELL && t_card >= 0)
	{
	  cloning(player, card, t_player, t_card);

	  card_instance_t* instance = get_card_instance(player, card);

	  instance->regen_status |= KEYWORD_RECALC_CHANGE_TYPE;
	  get_abilities(player, card, EVENT_CHANGE_TYPE, -1);

	  call_card_function_i(instance, player, card, EVENT_RESOLVE_SPELL);
	  dispatch_event(player, card, EVENT_ENTERING_THE_BATTLEFIELD_AS_CLONE);
	}
}

// As enters_the_battlefield_as_copy_of(), but prompts for a target.  Returns nonzero if copies.  td only needs to be initialized during EVENT_RESOLVE_SPELL.


// Common case of enters_the_battlefield_as_copy_of_any().






// Shapeshifts a single permanent.


// Shapeshifts all cards matching test and controlled by t_player (or ANYBODY).










void cannot_be_countered(int player, int card, event_t event)
{
  if (event == EVENT_CAST_SPELL && affect_me(player, card))
    get_card_instance(player, card)->token_status |= STATUS_CANNOT_BE_COUNTERED;
}





int get_cost_for_propaganda_like_effect(int player, int card)
{
  int csvid = get_id(player, card);
  int tax = 0, annex_count = 0;
  switch (csvid)
  {
  case CARD_ID_SPHERE_OF_SAFETY:
    tax = count_subtype(player, TYPE_ENCHANTMENT, -1);
    break;

  case CARD_ID_PROPAGANDA:
  case CARD_ID_ELEPHANT_GRASS:
  case CARD_ID_GHOSTLY_PRISON:
  case CARD_ID_WINDBORN_MUSE:
  case CARD_ID_KOSKUN_FALLS:
    tax = 2;
    break;

  case CARD_ID_BAIRD_STEWARD_OF_ARGIVE:
    tax = 1;
    break;

  case CARD_ID_ARCHANGEL_OF_TITHES:
  {
    if (!is_tapped(player, card))
    {
      tax = 1;
    }
    break;
  }

  case CARD_ID_COLLECTIVE_RESTRAINT:
    tax = count_domain(player, card);
    break;

  case CARD_ID_NORNS_ANNEX:
    annex_count = 1;
    break;

  case CARD_ID_WAR_TAX:
  {
    card_instance_t *this = get_card_instance(player, card);
    if (this->targets[1].player > 0)
    {
      tax = this->targets[1].player;
    }
    break;
  }
  default:
    break;
  }

  int result = 0;
  SET_BYTE0(result) += tax;
  SET_BYTE1(result) += annex_count;
  return result;
}

void tax_attack(int player, int card, event_t event){

	if( trigger_condition == TRIGGER_PAY_TO_ATTACK && current_phase == PHASE_DECLARE_ATTACKERS && current_turn == 1-player &&
		forbid_attack == 0 && ! is_humiliated(player, card, AL_STATIC)
	  ){
		if( affect_me( trigger_cause_controller, trigger_cause ) && reason_for_trigger_controller == affected_card_controller ){
			int result = get_cost_for_propaganda_like_effect(player, card);
			int tax = BYTE0(result);
			int annex_count = BYTE1(result);

			if(event == EVENT_TRIGGER){
				event_result |= 2;
			}
			else if( event == EVENT_RESOLVE_TRIGGER){
					if( annex_count == 0 ){
						if( has_mana(1-player, COLOR_ANY, tax) ){
							int choice = do_dialog(1-player, player, card, -1, -1, " Pay to attack\n Don't Pay\n", 0);
							if( choice == 0 ){
								charge_mana_while_resolving(player, card, EVENT_RESOLVE_TRIGGER, 1-player, COLOR_COLORLESS, tax);
								if( spell_fizzled != 1 ){
									return;
								}
							}
						}
					}
					else{
						if( has_phyrexian_mana(1-player, MANACOST_XW(tax, annex_count), 0)){
							int choice = do_dialog(1-player, player, card, -1, -1, " Pay to attack\n Don't Pay\n", 0);
							if( choice == 0 ){
								if( charge_phyrexian_mana(1-player, card, event, MANACOST_XW(tax, annex_count), 0) ){
									return;
								}
							}
						}
					}
					forbid_attack = 1;
			}
		}
	}
}

/* This doesn't actually skip the step, it just reduces number of
   draws by 99.  Should be good enough 99& of the time. */
int skip_your_draw_step(int player, event_t event){
	if(event == EVENT_DRAW_PHASE && player==current_turn){
		event_result-=99;
	}
	return 1;
}



















// fmt should have exactly one %s in it to receive the color text.  Returns a pointer to a static buffer.


// As get_sleighted_color_text(), but fmt should have exactly two %s in it.






/* fmt should have exactly one %s, %a, or %l in it to receive the land text for each subtype provided as a variable argument.  (%a will insert a "a " or "an "
 * before the land text as approriate; %l will make unhacked land text all lowercase, appropriate for e.g. a landwalk ability rather than a type name.)  Returns
 * a pointer to a static buffer.  Each variable argument can be a SUBTYPE_*, HARDCODED_SUBTYPE_*, or COLOR_*. */








// Just like get_type(), but the iid is already known (and possibly may not be easily determinable from just player/card, for instance if it's left play)












static int actually_has_a_card_in_hand(int player){
	int c;
	for (c = 0; c < active_cards_count[player]; ++c){
		if (in_hand(player, c)){
			return 1;
		}
	}
	return 0;
}





int pick_card_from_graveyard(int player, int target_graveyard, const char * message){

	return show_deck( player, get_grave(target_graveyard), 500, message, 0, 0x7375B0 );
}

int changeling_switcher(int player, int card, event_t event){

   if( event == EVENT_ABILITIES && affect_me(player, card) ){
	   event_result |= KEYWORD_PROT_INTERRUPTS;
   }

   return 0;
}

int generic_creature_with_activated_tapsubtype_ability(int player, int card, event_t event, subtype_t subtype, int howmany, int illegalstate1, int illegalstate2){
  card_instance_t *instance = get_card_instance(player, card);

  target_definition_t td;
  default_target_definition(player, card, &td, TYPE_CREATURE);
  td.allowed_controller = player;
  td.preferred_controller = player;
  td.required_subtype = subtype;
  td.illegal_abilities = 0;
  td.illegal_state = illegalstate1 | illegalstate2;
  td.allow_cancel = 0;

  target_definition_t td1;
  default_target_definition(player, card, &td1, TYPE_CREATURE);
  td1.allowed_controller = player;
  td1.preferred_controller = player;
  td1.illegal_abilities = 0;
  td1.allow_cancel = 0;
  td1.illegal_state = illegalstate1 | illegalstate2;
  td1.required_abilities = KEYWORD_PROT_INTERRUPTS;

  if( event == EVENT_CAN_ACTIVATE &&
	  (target_available(player, card, &td) + target_available(player, card, &td1) >= howmany)   ){
	return 1;
  }

  if( event == EVENT_ACTIVATE){
	 int stop = 0;
	 int test = 0;
	 while( stop != 1 ){
		   if( can_target(&td) && pick_target(&td, "TARGET_CREATURE") ){
			  tap_card(instance->targets[0].player, instance->targets[0].card);
			  test++;
		   }
		   else if( can_target(&td1) && pick_target(&td1, "TARGET_CREATURE") ){
				   tap_card(instance->targets[0].player, instance->targets[0].card);
				   test++;

		   }

		   if( test == howmany ){
			  stop = 1;
		   }

	}

  }

  return 0;
 }

int specific_spell_played(int player, int card, event_t event, int t_player, int trigger_mode, int type, int flag1, int subtype, int flag2, int color, int flag3, int id, int flag4, int cc, int flag5){

	if( trigger_condition == TRIGGER_SPELL_CAST && affect_me(player, card) && player == reason_for_trigger_controller &&
		!is_humiliated(player, card, AL_TRIG) && (t_player == ANYBODY || trigger_cause_controller == t_player) &&
		!is_what(trigger_cause_controller, trigger_cause, TYPE_LAND) && ! is_token(trigger_cause_controller, trigger_cause) &&
		make_test_in_play(trigger_cause_controller, trigger_cause, -1, type, flag1, subtype, flag2, color, flag3, id, flag4, cc, flag5)
	  ){
		if(event == EVENT_TRIGGER){
			if (trigger_mode == RESOLVE_TRIGGER_DUH){
				trigger_mode = duh_mode(player) ? RESOLVE_TRIGGER_MANDATORY : RESOLVE_TRIGGER_OPTIONAL;
			}
			event_result |= trigger_mode;
		}
		else if(event == EVENT_RESOLVE_TRIGGER){
			card_instance_t* instance = get_card_instance(player, card);
			instance->targets[1].player = trigger_cause_controller;
			instance->targets[1].card = trigger_cause;
			return 1;
		}
	}
	return 0;
}

int new_specific_spell_played(int player, int card, event_t event, int t_player, int trigger_mode, test_definition_t* this_test/*optional*/){

	if( trigger_condition == TRIGGER_SPELL_CAST && affect_me(player, card) && player == reason_for_trigger_controller &&
		!is_humiliated(player, card, AL_TRIG) && (t_player == ANYBODY || trigger_cause_controller == t_player) &&
		!is_what(trigger_cause_controller, trigger_cause, TYPE_LAND) && ! is_token(trigger_cause_controller, trigger_cause) &&
		(!this_test || new_make_test_in_play(trigger_cause_controller, trigger_cause, -1, this_test))
	  ){
		if(event == EVENT_TRIGGER){
			if (trigger_mode == RESOLVE_TRIGGER_DUH){
				trigger_mode = duh_mode(player) ? RESOLVE_TRIGGER_MANDATORY : RESOLVE_TRIGGER_OPTIONAL;
			}
			event_result |= trigger_mode;
		}
		else if(event == EVENT_RESOLVE_TRIGGER){
			card_instance_t* instance = get_card_instance(player, card);
			instance->targets[1].player = trigger_cause_controller;
			instance->targets[1].card = trigger_cause;
			return 1;
		}
	}
	return 0;
}











int metamorphosis(int player, int card, type_t selected_type, kill_t kill_code){

	kill_card(player, card, kill_code);

	test_definition_t this_test;
	default_test_definition(&this_test, selected_type);
	return reveal_card_until_you_reveal_test(player, player, &this_test, RCUYRT_PUT_TEST_INTO_PLAY | RCUYRT_SHUFFLE_REST | RCUYRT_RETURN_ADDED_CARD);
}

int impulse_effect(int player, int amount, int whattodo)
{
  //  whattodo = 0 --> Impulse-like effect (add the selected card to hand and put the rest on
  //  bottom) whattodo = 1 --> Impulse-like effect, but put the selected card on the top of library
  //  whattodo = 3 --> add the selected card to hand and randomize the rest on bottom.
  //  whattodo = 2 --> add the selected card to hand and leave the rest on the top.

  int *deck = deck_ptr[player];
  int card_added = -1;
  if (count_deck(player) < amount)
  {
    amount = count_deck(player);
  }
  if (amount <= 0)
  {
    return 0;
  }
  if (deck[0] != -1)
  {
    char msg[100] = "Select a card.";
    test_definition_t this_test;
    new_default_test_definition(&this_test, TYPE_ANY, msg);
    this_test.create_minideck = amount;
    this_test.no_shuffle = 1;

    int selected =
        new_select_a_card(player, player, TUTOR_FROM_DECK, 1, AI_MAX_VALUE, -1, &this_test);
    card_added = add_card_to_hand(player, deck[selected]);
    remove_card_from_deck(player, selected);
    amount--;

    if (amount > 0 && whattodo != 2)
    {
      if (whattodo == 3)
        put_top_x_on_bottom_in_random_order(player, amount);
      else
        put_top_x_on_bottom(player, player, amount);
    }

    if (whattodo == 1)
    {
      put_on_top_of_deck(player, card_added);
    }
  }

  return card_added;
}

int count_free_mana(int player, int type){

	int count = active_cards_count[player]-1;
	int free_mana = 0;

	while( count > -1 ){
		  if( in_play(player, count) ){
			  if( get_id(player, count) == CARD_ID_SEMBLANCE_ANVIL ){
				  card_instance_t *leg = get_card_instance(player, count);
				  if( leg->info_slot & type ){
					  free_mana+=2;
				  }
			  }
			  if( get_id(player, count) == CARD_ID_HELM_OF_AWAKENING ){
				  free_mana+=1;
			  }
		  }
		  count--;
	}

  return free_mana;
}





int count_attackers(int player)
{
  int c, result = 0;
  for (c = 0; c < active_cards_count[player]; ++c)
	if (in_play_and_attacking(player, c))
	  ++result;

  return result;
}

int count_attackers_non_planeswalker(int player)
{
  int c, result = 0;
  for (c = 0; c < active_cards_count[player]; ++c)
	if (in_play_and_attacking(player, c) && !check_special_flags(player, c, SF_ATTACKING_PWALKER))
	  ++result;

  return result;
}

/* If player == whose_deck or player is an AI (actual AI, or AI speculating for a human player), then returns a color_t of the most common color in whose_deck's
 * library.  If player is a human and whose_deck isn't his, then always returns COLOR_BLACK.  If player is -1, then returns a color_test_t of the two most
 * common colors in whose_deck's library. */


int select_a_protection(int player){

  int ai_choice = 0;

  if( player == AI ){
	 ai_choice = get_deck_color(player, 1-player);
  }

  int color = choose_a_color(player, ai_choice);
  int keyword = KEYWORD_PROT_BLACK;

  if( color == 2 ){
	 keyword = KEYWORD_PROT_BLUE;
  }
  else if( color == 3 ){
		  keyword = KEYWORD_PROT_GREEN;
  }
  else if( color == 4 ){
		  keyword = KEYWORD_PROT_RED;
  }
  else if( color == 5 ){
		  keyword = KEYWORD_PROT_WHITE;
  }

  return keyword;
}

int sacrifice_at_end_of_combat(int player, int card, event_t event)
{
	if (end_of_combat_trigger(player, card, event, RESOLVE_TRIGGER_MANDATORY)){
		card_instance_t* instance = get_card_instance(player, card);
		if( ! check_state(instance->damage_target_player, instance->damage_target_card, STATE_OUBLIETTED) ){
			kill_card(instance->damage_target_player, instance->damage_target_card, KILL_SACRIFICE);
		}
		kill_card(player, card, KILL_EXILE);
	}

  return 0;
}

int remove_at_eot(int player, int card, event_t event){
	card_instance_t *instance = get_card_instance(player, card);
	if( eot_trigger(player, card, event) ){
		int p = instance->targets[0].player;
		int c = instance->targets[0].card;
		if( ! check_state(p, c, STATE_OUBLIETTED) ){
			kill_card(p, c, KILL_EXILE);
		}
		else{
			kill_card(player, card, KILL_SACRIFICE);
		}
	}
	return 0;
}





int total_playable_lands(int player)
{
  uint16_t uVar1;
  uint16_t uVar2;
  card_instance_t *instance;
  int iVar4;
  int iVar5;
  int iVar6;
  int iVar7;
  bool bVar8;

  iVar6 = current_turn;
  iVar5 = active_cards_count[current_turn] + -1;
  if (iVar5 < 0)
  {
    iVar7 = 1;
    iVar6 = 1 - current_turn;
    iVar5 = active_cards_count[iVar6] + -1;
    if (iVar5 < 0)
      goto LAB_02684c4c;
  LAB_02684965:
    do
    {
      while ((instance = in_play(iVar6, iVar5),
              instance == NULL ||
                  ((iVar4 = is_what(iVar6, iVar5, 0x80),
                    iVar4 == 0 &&
                        (iVar4 = is_humiliated(iVar6, iVar5, AL_STATIC), iVar4 != 0)))))
      {
      joined_r0x02684acf:
        iVar5 = iVar5 + -1;
        if (iVar5 < 0)
          goto LAB_02684a0f;
      }
      iVar4 = get_id(iVar6, iVar5);
      if (iVar4 == 0x1bf9)
      {
      LAB_02684ba0:
        iVar7 = iVar7 + (unsigned int)(iVar6 == player) * 2;
        goto joined_r0x02684acf;
      }
      if (iVar4 < 0x1bfa)
      {
        if (iVar4 == 0x387)
        {
          instance = get_card_instance(iVar6, iVar5);
          if ((instance->internal_card_id == LEGACY_EFFECT_CUSTOM) &&
              (instance->info_slot_fn == check_playable_lands_legacy))
          {
            uVar1 = instance->display_pic_csv_id;
            if (uVar1 == 0x1a24)
              goto LAB_02684ba0;
            if ((unsigned short)uVar1 < 0x1a25)
            {
              if (uVar1 == 0xae2)
              {
                iVar7 = iVar7 + (unsigned int)(iVar6 == player) * 3;
              }
            }
            else
            {
              if (uVar1 == 0x35ad)
                goto LAB_02684a48;
              if ((unsigned short)uVar1 < 0x35ae)
              {
                if ((uVar1 == 0x2a08) || (uVar1 == 0x3323))
                  goto LAB_02684a48;
              }
              else if ((uVar1 == 0x437d) || (uVar1 == 0x55cb))
                goto LAB_02684a48;
            }
          }
        }
        else if (0x386 < iVar4)
        {
          if (iVar4 == 0x937)
            goto LAB_02684b73;
          if (iVar4 == 0xeb3)
            goto LAB_02684a48;
        }
        goto joined_r0x02684acf;
      }
      if (iVar4 != 0x4008)
      {
        if (iVar4 < 0x4009)
        {
          if (iVar4 != 0x2982)
          {
            if (iVar4 < 0x2983)
            {
              if (iVar4 == 0x232c)
                goto LAB_02684b73;
            }
            else if ((iVar4 == 0x3d19) || (iVar4 == 0x3e70))
              goto LAB_02684a48;
            goto joined_r0x02684acf;
          }
        }
        else if ((iVar4 != 0x5202) && ((iVar4 != 0x562d && (iVar4 != 0x479c))))
          goto joined_r0x02684acf;
      LAB_02684a48:
        iVar7 = iVar7 + (unsigned int)(iVar6 == player);
        goto joined_r0x02684acf;
      }
    LAB_02684b73:
      iVar7 = iVar7 + 1;
      iVar5 = iVar5 + -1;
    } while (-1 < iVar5);
  }
  else
  {
    bVar8 = current_turn == player;
    iVar7 = 1;
    do
    {
      while ((instance = in_play(iVar6, iVar5),
              instance == NULL ||
                  ((iVar4 = is_what(iVar6, iVar5, 0x80),
                    iVar4 == 0 &&
                        (iVar4 = is_humiliated(iVar6, iVar5, AL_STATIC), iVar4 != 0)))))
      {
      joined_r0x02684b06:
        iVar5 = iVar5 + -1;
        if (iVar5 == -1)
          goto LAB_0268494a;
      }
      iVar4 = get_id(iVar6, iVar5);
      if (iVar4 == 0x1bf9)
      {
      LAB_02684bb8:
        iVar7 = iVar7 + (unsigned int)bVar8 * 2;
        goto joined_r0x02684b06;
      }
      if (iVar4 < 0x1bfa)
      {
        if (iVar4 == 0x387)
        {
          instance = get_card_instance(iVar6, iVar5);
          if ((instance->internal_card_id == LEGACY_EFFECT_CUSTOM) &&
              (instance->info_slot_fn == check_playable_lands_legacy))
          {
            uVar2 = instance->display_pic_csv_id;
            if (uVar2 == 0x1a24)
              goto LAB_02684bb8;
            if ((unsigned short)uVar2 < 0x1a25)
            {
              if (uVar2 == 0xae2)
              {
                iVar7 = iVar7 + (unsigned int)bVar8 * 3;
              }
            }
            else
            {
              if (uVar2 == 0x35ad)
                goto LAB_02684a88;
              if ((unsigned short)uVar2 < 0x35ae)
              {
                if ((uVar2 == 0x2a08) || (uVar2 == 0x3323))
                  goto LAB_02684a88;
              }
              else if ((uVar2 == 0x437d) || (uVar2 == 0x55cb))
                goto LAB_02684a88;
            }
          }
        }
        else if (0x386 < iVar4)
        {
          if (iVar4 == 0x937)
            goto LAB_02684b8b;
          if (iVar4 == 0xeb3)
            goto LAB_02684a88;
        }
        goto joined_r0x02684b06;
      }
      if (iVar4 != 0x4008)
      {
        if (iVar4 < 0x4009)
        {
          if (iVar4 != 0x2982)
          {
            if (iVar4 < 0x2983)
            {
              if (iVar4 == 0x232c)
                goto LAB_02684b8b;
            }
            else if ((iVar4 == 0x3d19) || (iVar4 == 0x3e70))
              goto LAB_02684a88;
            goto joined_r0x02684b06;
          }
        }
        else if ((iVar4 != 0x5202) && ((iVar4 != 0x562d && (iVar4 != 0x479c))))
          goto joined_r0x02684b06;
      LAB_02684a88:
        iVar7 = iVar7 + (unsigned int)(iVar6 == player);
        goto joined_r0x02684b06;
      }
    LAB_02684b8b:
      iVar5 = iVar5 + -1;
      iVar7 = iVar7 + 1;
    } while (iVar5 != -1);
  LAB_0268494a:
    iVar6 = 1 - current_turn;
    iVar5 = active_cards_count[iVar6] + -1;
    if (-1 < iVar5)
      goto LAB_02684965;
  }
LAB_02684a0f:
  if (iVar7 != 1)
  {
    return iVar7;
  }
LAB_02684c4c:
  player_bits[player] &= ~PB_COUNT_TOTAL_PLAYABLE_LANDS;
  return 1;
}

void check_playable_lands(int player)
{
	player_bits[player] |= PB_COUNT_TOTAL_PLAYABLE_LANDS;
}

int check_playable_lands_legacy(int player, int card, event_t event){

	player_bits[player] |= PB_COUNT_TOTAL_PLAYABLE_LANDS;

	if( eot_trigger(player, card, event) ){
		kill_card(player, card, KILL_EXILE);
	}

	return 0;
}

int generic_painland(int player, int card, event_t event)
{
  if (event == EVENT_CAN_ACTIVATE)
  {
    if (IS_AI(player) && life[player] <= 1 &&
        !(get_card_instance(player, card)->mana_color & COLOR_TEST_COLORLESS))
      return 0;
    return mana_producer(player, card, event);
  }
  if (event == EVENT_ACTIVATE)
  {
    color_test_t colors = get_card_instance(player, card)->mana_color | mana_colors_added[player];
    if (IS_AI(player) && life[player] <= 1 && (colors & COLOR_TEST_COLORLESS))
    {
      produce_mana_tapped_all_one_color(player, card,
                                        COLOR_TEST_COLORLESS | mana_colors_added[player], 1);
    }
    else
    {
      produce_mana_tapped_all_one_color(player, card, colors, 1);
      if (!check_special_flags2(player, card, SF2_CONTAMINATION))
      {
        if (chosen_colors & ~(COLOR_TEST_COLORLESS | mana_colors_added[player]))
          damage_player(player, 1, player, card);
      }
      else
      {
        int ok_colors = 0;
        for (int c = 1; c < 7; ++c)
        {
          // Native code tests the color index here and may deal damage more than once.
          if ((COLOR_TEST_COLORLESS | mana_colors_added[player]) & c)
          {
            ok_colors |= mana_color_override[player][c] == COLOR_ANY
                             ? COLOR_TEST_ANY_COLORED
                             : 1 << mana_color_override[player][c];
            if (chosen_colors & ~ok_colors)
              damage_player(player, 1, player, card);
          }
        }
      }
    }
  }
  else if (event == EVENT_COUNT_MANA && affect_me(player, card))
  {
    if (CAN_TAP_FOR_MANA(player, card))
    {
      int colors = get_card_instance(player, card)->mana_color | mana_colors_added[player];
      if (!check_special_flags2(player, card, SF2_CONTAMINATION))
      {
        if (IS_AI(player) && life[player] <= 1)
        {
          if (colors & COLOR_TEST_COLORLESS)
            declare_mana_available(player, COLOR_COLORLESS, 1);
        }
        else
          declare_mana_available_hex(player, colors, 1);
      }
      else if (IS_AI(player) && life[player] <= 1)
      {
        if (colors & COLOR_TEST_COLORLESS)
        {
          int quantity = mana_quantity_override[player][0];
          declare_mana_available(player, mana_color_override[player][0],
                                 quantity < 0 ? 1 : quantity);
        }
      }
      else
      {
        int quantity = 1;
        for (int c = 0; c < 7; ++c)
        {
          int mask = 1 << c;
          int replacement = mana_color_override[player][c];
          if ((colors & mask) && replacement)
          {
            colors = (colors & ~mask) |
                     (replacement == COLOR_ANY ? COLOR_TEST_ANY_COLORED : 1 << replacement);
            if (mana_quantity_override[player][c] >= 0)
              quantity = mana_quantity_override[player][c];
          }
        }
        declare_mana_available_hex(player, colors, quantity);
      }
    }
  }
  else
    return mana_producer(player, card, event);
  return 0;
}

int effect_defender_can_attack_until_eot(int player, int card, event_t event){
	if (event == EVENT_ABILITIES){
		card_instance_t* instance = get_card_instance(player, card);
		if (instance->damage_target_player >= 0){
			get_card_instance(instance->damage_target_player, instance->damage_target_card)->token_status |= STATUS_WALL_CAN_ATTACK;
		}
	}
	if (event == EVENT_CLEANUP || event == EVENT_SHOULD_AI_PLAY){
		kill_card(player, card, KILL_EXILE);
	}
	return 0;
}

// Also suitable as part of an enchantment
int effect_cannot_attack(int player, int card, event_t event)
{
  if (event == EVENT_ABILITIES && in_play(player, card))
	{
	  card_instance_t* instance = get_card_instance(player, card);
	  cannot_attack(instance->damage_target_player, instance->damage_target_card, event);
	}

	return 0;
}

int effect_cannot_attack_until_eot(int player, int card, event_t event)
{
  effect_cannot_attack(player, card, event);

  if (event == EVENT_CLEANUP || event == EVENT_SHOULD_AI_PLAY)
	kill_card(player, card, KILL_EXILE);

  return 0;
}



void enters_the_battlefield_with_counters(int player, int card, event_t event, counter_t type,
                                          int number)
{
  card_instance_t *instance;
  if (((0 < number) &&
       (((event == EVENT_CAST_SPELL || (event == EVENT_ENTERING_THE_BATTLEFIELD_AS_CLONE)) &&
         (card == affected_card)))) &&
      (player == affected_card_controller))
  {
    instance = get_card_instance(player, card);
    if ((instance->ext->special_flags2 & SF2_FACE_DOWN_DUE_TO_MANIFEST) == 0)
    {
      hack_silent_counters = hack_silent_counters + 1;
      add_counters(player, card, player, card, type, number);
      hack_silent_counters = hack_silent_counters + -1;
    }
  }
  return;
}

int count_nonbasic_lands(int player){

	int result = 0;
	int i;
	for(i=0; i<2; i++){
		if( player == 2 || i == player ){
			int count = 0;
			while( count < active_cards_count[i] ){
				   if( in_play(i, count) && is_what(i, count, TYPE_LAND) && ! is_basic_land(i, count) ){
					   result++;
				   }
				   count++;
			}
		}
	}

	return result;
}

int control_nonbasic_land(int player){

	int count = 0;
	while( count < active_cards_count[player] ){
		   if( in_play(player, count) && is_what(player, count, TYPE_LAND) && ! is_basic_land(player, count) ){
			   return 1;
		   }
		   count++;
	}

	return 0;
}

int control_basic_land(int player){
	if( player != 2 ){
		int count = 0;
		while( count < active_cards_count[player] ){
				if( in_play(player, count) && is_what(player, count, TYPE_LAND) && is_basic_land(player, count) ){
					return 1;
				}
				count++;
		}
		return 0;
	}
	else{
		int i;
		for(i=0; i<2; i++){
			int count = 0;
			while( count < active_cards_count[i] ){
					if( in_play(i, count) && is_what(i, count, TYPE_LAND) && is_basic_land(i, count) ){
						return 1;
					}
					count++;
			}
		}
		return 0;
	}
	return 0;
}

int check_base_value_for_ai(int card, int conf_base_value, int mode){
	// Mode = 1 -> Select the greatest
	// Mode = -1 -> Select the lowest

	if( card < 0 ){
		return 0;
	}

	int base_value = get_base_value(-1, cards_data[card].id);
	if( mode == 1 && base_value > conf_base_value ){
		return 1;
	}
	if( mode == -1 && base_value < conf_base_value ){
		return 1;
	}
	return 0;
}

int player_reveals_x_and_discard(int t_player, int who_choses, int howmany, int effect, int ai_selection_mode, int type, int flag1,
					int subtype, int flag2, int color, int flag3, int id, int flag4, int cmc, int flag5
  ){
	// The function will return the CSV ID of the discarded card, unless otherwise noted.
	// effect = 1 -> Discard
	// effect = 2 -> Exile
	// effect = 4 -> Used for Karn Liberated (AI will chosse the less AI Base value among non-permanents).
	// effect = 8 -> Select a card to play from opponent's hand. Will return the CARD value, not the CSV ID.
	// effect = 16 -> Select a creature or a planeswalker (Despise)

	test_definition_t this_test;
	default_test_definition(&this_test, type);
	this_test.type_flag = flag1;
	this_test.subtype = subtype;
	this_test.subtype_flag = flag2;
	this_test.color = color;
	this_test.color_flag = flag3;
	this_test.id = id;
	this_test.id_flag = flag4;
	this_test.cmc = cmc;
	this_test.cmc_flag = flag5;

	test_definition_t this_test2;
	default_test_definition(&this_test2, TYPE_ANY);

	int count = 0;
	int initial_index = 0;
	int hand_index = 0;
	int initial_hand[50];
	int ai_hand[50];

	if( howmany > hand_count[t_player] ){
		howmany = hand_count[t_player];
	}
	while( count < active_cards_count[t_player] ){
		   if( in_hand(t_player, count) ){
			   card_instance_t *crd = get_card_instance(t_player, count);
			   initial_hand[initial_index] = crd->internal_card_id;
			   initial_index++;
		   }
		   count++;
	}

	count = 0;
	int selected = -1;
	while( count < howmany ){
			selected = -1;
			selected = select_card_from_zone(t_player, t_player, initial_hand, initial_index, 1, AI_MIN_VALUE, -1, &this_test2);
			if( selected != -1 ){
				ai_hand[hand_index] = initial_hand[selected];
				hand_index++;
				int i;
				for(i=selected; i<(initial_index-1); i++){
					initial_hand[i] = initial_hand[i+1];
				}
				initial_index--;
				count++;
			}
	}

	int result = 0;
	selected = select_card_from_zone(who_choses, t_player, ai_hand, hand_index, 1, AI_MAX_VALUE, -1, &this_test);

	if( selected == -1 ){
		return -1;
	}

	count = active_cards_count[t_player]-1;
	while( count > -1 ){
		   if( in_hand(t_player, count) ){
			   card_instance_t *crd = get_card_instance(t_player, count);
			   if( crd->internal_card_id == ai_hand[selected] ){
				   result = get_id(t_player, count);
				   if( effect & 1 ){
					   new_discard_card(t_player, count, who_choses, 0);
				   }
				   if( effect & 2 ){
						rfg_card_in_hand(t_player, count);
				   }
				   if( effect & 32 ){
						put_on_top_of_deck(t_player, count);
				   }
				   if( effect & 4 || effect & 64 ){
					   result = count;
				   }
				   break;
			   }
		   }
		   count--;
	}

	return result;
}



void default_ec_definition(int t_player, int who_choses, ec_definition_t *this_definition){
	this_definition->target_player = t_player;
	this_definition->who_choses = who_choses;
	this_definition->effect = EC_DISCARD;
	this_definition->qty = 1;
	this_definition->cards_to_reveal = 0;
	this_definition->ai_selection_mode = AI_MAX_VALUE;
	this_definition->special_selection_function = NULL;
}



int check_battlefield_for_id(int player, int id){

	if( id < 0 ){
		return 0;
	}

	int i;
	for(i=0; i<2; i++){
		if( player == i || player == 2 ){
			int count = 0;
			while( count < active_cards_count[i] ){
					if( in_play(i, count) && get_id(i, count) == id && ! is_humiliated(i, count, AL_STATIC) ){
						// this should be right 99% of the cases, since an "humiliated" card lose all abilities
						return 1;
					}
					count++;
			}
		}
	}

	return 0;
}

int locate_id(int player, int id){

	int i;
	for(i=0; i<2; i++){
		if( player == 2 || i == player ){
			int count = 0;
			while( count < active_cards_count[i] ){
					if( in_play(i, count) && get_id(i, count) == id ){
						return count;
					}
					count++;
			}
		}
	}
	return -1;
}

int check_battlefield_for_targettable_id(int player, int card, int t_player, int id, int mode){

	if( id == -1 ){
		return 0;
	}

	if( t_player < 2 ){
		int count = 0;
		while( count < active_cards_count[t_player] ){
			   if( in_play(t_player, count) && get_id(t_player, count) == id ){
					if( ! is_protected_from_me(player, card, t_player, count) ){
						if( mode == 0 ){
							return 1;
						}
						if( mode == 1 ){
							return count;
						}
					}
			   }
			   count++;
		}
	}

	else if( player == 2 ){
			 int i;
			 for(i=0; i<2; i++){
				 int count = 0;
				 while( count < active_cards_count[i] ){
						if( in_play(i, count) && get_id(i, count) == id ){
							if( ! is_protected_from_me(player, card, i, count) ){
								if( mode == 0 ){
									return 1;
								}
								if( mode == 1 ){
									return count;
								}
							}
						}
						count++;
				 }
			 }
	}

	return 0;
}







void make_all_untargettable(void){

	 int i;
	 for(i=0; i<2; i++){
		 int count = 0;
		 while( count < active_cards_count[i] ){
				if( in_play(i, count) && is_what(i, count, TYPE_CREATURE) ){
					card_instance_t *this = get_card_instance(i, count);
					this->state |= STATE_CANNOT_TARGET;
				}
				count++;
		 }
	 }
}

void remove_untargettable_from_all(void){

	 int i;
	 for(i=0; i<2; i++){
		 int count = 0;
		 while( count < active_cards_count[i] ){
				if( in_play(i, count) && is_what(i, count, TYPE_PERMANENT) ){
					card_instance_t *this = get_card_instance(i, count);
					if( this->state & STATE_CANNOT_TARGET ){
						this->state  &= ~STATE_CANNOT_TARGET;
					}
				}
				count++;
		 }
	 }
}

void nice_creature_to_sacrifice(int player, int card)
{
  unsigned int uVar1;
  card_instance_t *instance;

  instance = get_card_instance(player, card);
  uVar1 = instance->ext->sp_keywords2;
  if ((uVar1 & 8) == 0)
  {
    instance->ext->sp_keywords2 = uVar1 | 8;
  }
  return;
}





void reveal_target_player_hand(int player ){

	 int cards_array[60];
	 int revealed = 0;
	 int i;
	 for(i=0;i<active_cards_count[player]; i++){
		 if( in_hand(player, i) ){
			 card_instance_t *rev_card = get_card_instance(player, i);
			 cards_array[revealed] = rev_card->internal_card_id;
			 revealed++;
		 }
	 }

	 if( revealed > 0){
		 show_deck( 1-player, cards_array, revealed, "Cards revealed", 0, 0x7375B0 );
	 }
}









void symbiotic_creature(int player, int card, event_t event, int token_type, int amount){

	nice_creature_to_sacrifice(player, card);

	if (this_dies_trigger(player, card, event, RESOLVE_TRIGGER_MANDATORY)){
		token_generation_t token;
		default_token_definition(player, card, token_type, &token);
		token.qty = amount;
		generate_token(&token);
	}
}

// AI leaves tapped if in_play(targets[1])


// Call as if (choosing_to_untap(player, card, event)) { return should_ai_untap; }.  AI will leave tapped if that return value is 0, otherwise will untap.












int pick_targets_for_multidamage(int player, int card, int max_targets, int damages, target_definition_t *td, const char *prompt){

	card_instance_t *instance = get_card_instance(player, card);

	load_text(0, prompt);
	int trgts = 0;
	if( player != AI ){
		int players[2] = {0, 0};
		while( trgts < max_targets && can_target(td) ){
				if( select_target(player, card, td, text_lines[0], &(instance->targets[trgts])) ){
					if( instance->targets[trgts].card == -1 ){
						if( players[instance->targets[trgts].player] != 1 ){
							players[instance->targets[trgts].player] = 1;
							trgts++;
						}
						else{
							if( td->allow_cancel != 0 ){
								break;
							}
						}
					}
					else{
						state_untargettable(instance->targets[trgts].player, instance->targets[trgts].card, 1);
						trgts++;
					}
				}
				else{
					if( td->allow_cancel != 0 ){
						break;
					}
				}
		}
		int i;
		for(i=0; i<trgts; i++){
			if( instance->targets[i].card != -1 ){
				state_untargettable(instance->targets[i].player, instance->targets[i].card, 0);
			}
		}
	}
	else{
		if( td->zone == TARGET_ZONE_CREATURE_OR_PLAYER ){
			instance->targets[0].player = 1-player;
			instance->targets[0].card = -1;
			if( would_validate_target(player, card, td, 0) ){
				trgts++;
			}
		}
		int count = 0;
		while( count < active_cards_count[1-player] && trgts < max_targets ){
				if( in_play(1-player, count) && is_what(1-player, count, TYPE_CREATURE) ){
					if( ! is_protected_from_me(player, card, 1-player, count) &&
						get_toughness(1-player, count) <= damages
					  ){
						instance->targets[trgts].player = 1-player;
						instance->targets[trgts].card = count;
						trgts++;
					}
				}
				count++;
		}

		if( trgts < max_targets && td->allow_cancel == 0 ){
			count = 0;
			while( count < active_cards_count[1-player] && trgts < max_targets ){
					if( in_play(1-player, count) && is_what(1-player, count, TYPE_CREATURE) ){
						if( ! is_protected_from_me(player, card, 1-player, count) &&
							get_toughness(1-player, count) > damages
						  ){
							instance->targets[trgts].player = 1-player;
							instance->targets[trgts].card = count;
							trgts++;
						}
					}
					count++;
			}
		}

		if( trgts < max_targets && td->allow_cancel == 0 ){
			count = 0;
			while( count < active_cards_count[player] && trgts < max_targets ){
					if( in_play(player, count) && is_what(player, count, TYPE_CREATURE) ){
						if( ! is_protected_from_me(player, card, player, count) &&
							get_toughness(player, count) > damages
						  ){
							instance->targets[trgts+1].player = player;
							instance->targets[trgts+1].card = count;
							trgts++;
						}
					}
					count++;
			}
		}

		if( trgts < max_targets && td->allow_cancel == 0 ){
			count = 0;
			while( count < active_cards_count[player] && trgts < max_targets ){
					if( in_play(player, count) && is_what(player, count, TYPE_CREATURE) ){
						if( ! is_protected_from_me(player, card, player, count) ){
							instance->targets[trgts+1].player = player;
							instance->targets[trgts+1].card = count;
							trgts++;
						}
					}
					count++;
			}
		}
	}
	return trgts;
}

void multidamage(int player, int card, int num_targets, int damages, target_definition_t *td){
	// requires the definition of targets via the "pick_targets_for_multidamage" function

	if( num_targets < 1 ){
		return;
	}

	card_instance_t *instance = get_card_instance(player, card);

	int i;
	for(i=0; i<num_targets; i++){
		if( validate_target(player, card, td, i) ){
			damage_creature(instance->targets[i].player, instance->targets[i].card, damages, player, card);
		}
	}
}

void nobody_can_attack(int player, int card, event_t event, int t_player)
{
  if (event == EVENT_ATTACK_LEGALITY
	  && (affected_card_controller == t_player || t_player == ANYBODY))
	event_result = 1;
}

static int effect_nobody_can_attack_until_eot(int player, int card, event_t event)
{
  /* Local storage:
   * damage_target_player/damage_target_card: this card can always still attack.
   * targets[1].player: the player who can't attack.
   * targets[1].card: &1: ends at end of turn.
   *                  &2: ends after target player's next declare attackers step or second main
   * phase. &4: ends after target player's next declare attackers step; during discard or cleanup,
   * changes to &2.  Suitable for assigning during combat. &8: ends at start of controller's next
   * turn &16: ends during cleanup after a begin-turn marker has been set targets[2].player: number
   * of combat phases to skip (default: 1 or just leave it unset)
   */

  event_flags |= EA_DECLARE_ATTACK;

  card_instance_t *instance = get_card_instance(player, card);

  if (event == EVENT_ATTACK_LEGALITY &&
      (affected_card_controller == instance->targets[1].player ||
       instance->targets[1].player == ANYBODY) &&
      !affect_me(instance->damage_target_player, instance->damage_target_card))
    event_result = 1;

  if (trigger_condition == TRIGGER_LEAVE_PLAY &&
      trigger_cause_controller == instance->damage_target_player &&
      trigger_cause == instance->damage_target_card)
    instance->damage_target_player = instance->damage_target_card = -1; // detach (immediately)

  int end_effect = 0;
  if (event == EVENT_CLEANUP && (instance->targets[1].card & 1))
    end_effect = 1;

  if (event == EVENT_PHASE_CHANGED && (instance->targets[1].card & 2) &&
      current_turn == instance->targets[1].player &&
      (current_phase == PHASE_MAIN2 || current_phase == PHASE_BEFORE_BLOCKING))
    end_effect = 1;

  if (event == EVENT_PHASE_CHANGED && (instance->targets[1].card & 4) &&
      current_turn == instance->targets[1].player && current_phase == PHASE_BEFORE_BLOCKING)
    end_effect = 1;

  if (event == EVENT_BEGIN_TURN && current_turn == player && (instance->targets[1].card & 8))
    end_effect = 1;

  if (event == EVENT_BEGIN_TURN)
    instance->targets[2].card = 66;
  if (event == EVENT_CLEANUP && instance->targets[2].card == 66 && (instance->targets[1].card & 16))
    kill_card(player, card, KILL_EXILE);

  if (end_effect)
  {
    if (instance->targets[2].player > 0)
    {
      instance->targets[2].player--;
    }
    if (instance->targets[2].player <= 0)
    {
      instance->targets[1].card = 0;
      kill_card(player, card, KILL_EXILE);
    }
  }

  if ((instance->targets[1].card & 4) && current_phase > PHASE_MAIN2 &&
      current_turn == instance->targets[1].player)
  {
    instance->targets[1].card &= ~4;
    instance->targets[1].card |= 2;
  }

  return 0;
}

/* Only except_player/except_card, and creatures not controlled by t_player, can attack this turn.  t_player can be 2 for anybody, and except_player/except_card
 * can be -1 to not provide an exception. */
void nobody_can_attack_until_eot(int player, int card, int t_player, int except_player, int except_card)
{
  int leg = (except_card < 0 ? create_legacy_effect(player, card, effect_nobody_can_attack_until_eot)
			 : create_targetted_legacy_effect(player, card, effect_nobody_can_attack_until_eot, except_player, except_card));

  if (leg != -1)
	{
	  card_instance_t* legacy = get_card_instance(player, leg);
	  legacy->targets[1].player = t_player;
	  legacy->targets[1].card = 1;
	}
}

int target_player_skips_his_next_attack_step(int player, int card, int t_player, int this_turn_only)
{
	int leg = create_legacy_effect(player, card, effect_nobody_can_attack_until_eot);

	if (leg != -1)
	{
	  card_instance_t* legacy = get_card_instance(player, leg);
	  legacy->targets[1].player = t_player;
	  legacy->targets[1].card = (current_phase > PHASE_MAIN1 && current_phase < PHASE_MAIN2) ? 4 : 2;
	  if (this_turn_only)
		legacy->targets[1].card |= 1;
	}
	return leg;
}

void nobody_can_attack_until_your_next_turn(int player, int card, int t_player)
{
  int leg = create_legacy_effect(player, card, effect_nobody_can_attack_until_eot);

  if (leg != -1)
	{
	  // Not sure whether it's better or worse to always set this here than to return the legacy and let the caller muck with it.
	  alternate_legacy_text(2, player, leg);

	  card_instance_t* legacy = get_card_instance(player, leg);
	  legacy->targets[1].player = t_player;
	  legacy->targets[1].card = 8;
	}
}

int count_planeswalkers(int player, int mode){

	int result = 0;
	int count = 0;
	while( count < active_cards_count[player] ){
			if( in_play(player, count) && is_planeswalker(player, count) ){
				if( mode == 1 ){
					return 1;
				}
				result++;
			}
			count++;
	}

	return result;
}














void modify_pt_and_abilities(int player, int card, event_t event, int p_mod, int t_mod, int a_mod ){

	if( event == EVENT_POWER && affect_me(player, card) ){
		if( p_mod > 100 ){
			event_result = (p_mod-100);
		}
		else{
			event_result += p_mod;
		}
	}

	if( event == EVENT_TOUGHNESS && affect_me(player, card) ){
		if( t_mod > 100 ){
			event_result = (t_mod-100);
		}
		else{
			event_result += t_mod;
		}
	}

	if( event == EVENT_ABILITIES && affect_me(player, card) ){
		event_result |= a_mod;
	}
}



void real_set_cost_mod_for_activated_abilities(int player, int card, int mode){
	card_instance_t *instance = get_card_instance(player, card);
	int i;
	if( instance->targets[17].card < 0 ){
		instance->targets[17].card = 0;
	}
	for(i=12+((mode-1)*4); i<12+(mode*4); i++){
		if( !( instance->targets[17].card & (1<<i)) ){
			instance->targets[17].card |= (1<<i);
			break;
		}
	}
}

void set_cost_mod_for_activated_abilities(int player, int card, int mode, test_definition_t *this_test){
	// mode = 1 --> -1 colorless mana (min 1)
	// mode = 2 --> -2 colorless mana (min 1)
	// mode = 3 --> +2 colorless mana
	// mode = 4 --> +3 colorless mana
	if( card > -1 ){
		real_set_cost_mod_for_activated_abilities(player, card, mode);
	}
	else{
		int i;
		int test = new_get_test_score(this_test);
		for(i=0; i<2; i++){
			if( i == player || player == 2 ){
				int count = 0;
				while( count < active_cards_count[i] ){
						if( in_play(i, count) && new_make_test_in_play(i, count, test, this_test) ){
							real_set_cost_mod_for_activated_abilities(i, count, mode);
						}
						count++;
				}
			}
		}
	}
}

void real_remove_cost_mod_for_activated_abilities(int player, int card, int mode){
	// mode = 1 --> -1 colorless mana (min 1)
	// mode = 2 --> -2 colorless mana (min 1)
	// mode = 3 --> +2 colorless mana
	// mode = 4 --> +3 colorless mana
	card_instance_t *instance = get_card_instance(player, card);
	int i;
	if( instance->targets[17].card < 0 ){
		instance->targets[17].card = 0;
	}
	for(i=(12+(mode*4))-1; i>(12+((mode-1)*4))-1; i--){
		if( instance->targets[17].card & (1<<i) ){
			instance->targets[17].card &= ~ (1<<i);
			break;
		}
	}
}

void remove_cost_mod_for_activated_abilities(int player, int card, int mode, test_definition_t *this_test){
	// mode = 1 --> -1 colorless mana (min 1)
	// mode = 2 --> -2 colorless mana (min 1)
	// mode = 3 --> +2 colorless mana
	// mode = 4 --> +3 colorless mana
	if( card > -1 ){
		real_remove_cost_mod_for_activated_abilities(player, card, mode);
	}
	else{
		int i;
		int test = new_get_test_score(this_test);
		for(i=0; i<2; i++){
			if( i == player || player == 2 ){
				int count = 0;
				while( count < active_cards_count[i] ){
						if( in_play(i, count) && new_make_test_in_play(i, count, test, this_test) ){
							real_remove_cost_mod_for_activated_abilities(i, count, mode);
						}
						count++;
				}
			}
		}
	}
}

void phantom_effect(int player, int card, event_t event, int requires_counter)
{
  int result;
  card_instance_t *instance;

  if (((((event == EVENT_PREVENT_DAMAGE) &&
         (result = is_humiliated(player, card, AL_TRIG), result == 0)) &&
        (instance = get_card_instance(affected_card_controller, affected_card),
         instance->internal_card_id == damage_card)) &&
       ((instance->damage_target_card == card && (instance->damage_target_player == player)))) &&
      (0 < instance->info_slot))
  {
    if ((requires_counter != 0) &&
        (result = count_counters(player, card, COUNTER_P1_P1), result < 1))
    {
      return;
    }
    instance = get_card_instance(affected_card_controller, affected_card);
    instance->info_slot = 0;
    remove_counters(player, card, player, card, COUNTER_P1_P1, 1);
    return;
  }
  return;
}

int pick_target_by_type(int player, int card, int t_type, int must_select){

	target_definition_t td;
	default_target_definition(player, card, &td, t_type);
	td.allow_cancel = 1-must_select;

	int result = 0;

	if( t_type == TYPE_CREATURE ){
		if( pick_target(&td, "TARGET_CREATURE") ){
			result = 1;
		}
	}
	else if( t_type == TYPE_LAND ){
			if( pick_target(&td, "TARGET_LAND") ){
				result = 1;
			}
	}
	else if( t_type == TYPE_ENCHANTMENT ){
			if( pick_target(&td, "TARGET_ENCHANTMENT") ){
				result = 1;
			}
	}
	else if( t_type == TYPE_ARTIFACT ){
			if( pick_target(&td, "TARGET_ARTIFACT") ){
				result = 1;
			}
	}
	else{
		if( pick_target(&td, "TARGET_PERMANENT") ){
			result = 1;
		}
	}
	return result;
}





int cip_damage_creature(int player, int card, event_t event, target_definition_t *td1, const char *prompt, int amount){

	etb_ability(player, card, event, RESOLVE_TRIGGER_MANDATORY);

	if( event == EVENT_ETB_ABILITY ){

		if( amount == -1 ){
			if( player != AI ){
				amount = choose_a_number(player, "Deal how much damage?", life[player]);
			}
			else{
				amount = life[player]-6;
				if( amount < 0 ){
					amount = 0;
				}
			}
		}

		if( get_id(player, card) == CARD_ID_VOLCANO_HELLION && amount > 0 ){
			int legacy = create_legacy_effect(player, card, &my_damage_cannot_be_prevented);
			card_instance_t *leg = get_card_instance(player, legacy);
			leg->targets[0].player = player;
			leg->targets[0].card = card;
		}
		load_text(0, prompt);
		card_instance_t *instance = get_card_instance(player, card);
		instance->number_of_targets = 0;
		if (can_target(td1) && select_target(player, card, td1, text_lines[0], &(instance->targets[0]))){
			damage_target0(player, card, amount);

			if (get_id(player, card) == CARD_ID_VOLCANO_HELLION){
				damage_player(player, amount, player, card);
			}
		}
	}

	return 0;
}

int get_base_value( int player, int card ){
	int id = -1;
	if( player == -1 ){
		id = cards_data[card].id;
	}
	else{
		id = get_id(player, card);
	}
	card_ptr_t* c = cards_ptr[ id ];
	/*
	if( c->ai_base_value < 1 ){
		return my_base_value_by_id(id);
	}
	*/
	return c->ai_base_value;
}

// Selects total_damage targets, then deals 1 damage to each for each time it was targeted.  Returns number of different targets selected.
int target_and_divide_damage(int player, int card, target_definition_t* td, const char* prompt, int total_damage)
{
  target_definition_t td_default;
  if (!td)
	{
	  default_target_definition(player, card, &td_default, TYPE_CREATURE|TARGET_TYPE_PLANESWALKER);
	  td_default.zone = TARGET_ZONE_CREATURE_OR_PLAYER;
	  td_default.allow_cancel = 0;
	  td = &td_default;
	}

  if (!can_target(td))
	return 0;

  if (!prompt)
	prompt = "TARGET_ANY";

  int mode = 0;
  if (prompt[0] && prompt[1] >= 'a' && prompt[1] <= 'z')	// at least 2 characters, and 2nd is lowercase
	mode = GS_LITERAL_PROMPT;

  card_instance_t* inst = get_card_instance(player, card);
  inst->number_of_targets = 0;

  int i, failed = 0;;
  for (i = 0; i < total_damage; ++i)
	if (!new_pick_target(td, prompt, i, mode))
	  {
		failed = 1;
		break;
	  }
	else if (inst->targets[i].card >= 0)
	  get_card_instance(inst->targets[i].player, inst->targets[i].card)->state |= STATE_TARGETTED;

  for (i = 0; i < inst->number_of_targets; ++i)
	if (inst->targets[i].card >= 0)
	  get_card_instance(inst->targets[i].player, inst->targets[i].card)->state &= ~STATE_TARGETTED;

  if (failed)
	{
	  inst->number_of_targets = 0;
	  return 0;
	}

  inst->number_of_targets = total_damage;
  int rval = divide_damage(player, card, NULL);
  inst->number_of_targets = 0;

  return rval;
}

// Deals 1 damage to each of {player,card}'s targets for each time it was damaged.
int divide_damage(int player, int card, target_definition_t* td)
{
  card_instance_t* inst = get_card_instance(player, card);

  int i, validated = 0;
  for (i = 0; i < inst->number_of_targets; ++i)
	if (inst->targets[i].player >= 0)
	  {
		// Find any repeats of that target, so we deal the full amount of damage in one packet
		int j, dmg = 1;
		for (j = i + 1; j < inst->number_of_targets; ++j)
		  if (inst->targets[j].card == inst->targets[i].card
			  && inst->targets[j].player == inst->targets[i].player)
			{
			  inst->targets[j].player = inst->targets[j].card = -1;	// mark so it won't be processed again
			  ++dmg;
			}

		if (!td || validate_target(player, card, td, i))
		  {
			++validated;
			damage_creature(inst->targets[i].player, inst->targets[i].card, dmg, player, card);
		  }
		else
		  inst->targets[i].player = inst->targets[i].card = -1;
	  }

  return validated;
}

int check_playable_permanents(int player, int p_type, int mode){
	int count = 0;
	int max_value = -1;
	int result = -1;
	while( count < active_cards_count[player] ){
			if( in_hand(player, count) && is_what(player, count, p_type) ){
				int id = get_id(player, count);
				card_ptr_t* c = cards_ptr[ id ];
				if( has_mana_multi(player, c->req_colorless, c->req_black, c->req_blue, c->req_green,
					c->req_red, c->req_white)
				  ){
					card_instance_t *instance = get_card_instance(player, count);
					if( can_legally_play_iid(player, instance->internal_card_id) ){
						if( mode == 1 ){
							return 1;
						}
						if( c->ai_base_value > max_value ){
							max_value = c->ai_base_value;
							result = count;
						}
					}
				}
			}
			count++;
	}
	if( mode == 1 ){
		result = 0;
	}

	return result;
}

/* Returns the number of times {player,card} can activate for the given life and mana cost, multiplied by pump.  If limit is >= 0, it's an additional constraint
 * on the maximum number of times it can activate (e.g., to deal with a cost involving a sacrifice). */


// If p_pump >= 100, adds p_pump-100 +1/+1 counters, doesn't add power, and ignores t_pump.
int generic_shade(int player, int card, event_t event, int life_to_pay, int cless, int black,
                  int blue, int green, int red, int white, int p_pump, int t_pump, int k_pump,
                  int sp_key_pump)
{
  int card_00;
  int result;
  card_instance_t *instance;
  int iStack_18;
  iStack_18 = p_pump;
  if (event == EVENT_CAN_ACTIVATE)
  {
    result = can_use_activated_abilities(player, card);
    if (((result != 0) && (result = has_mana_for_activated_ability(player, card, cless, black, blue,
                                                                   green, red, white),
                           result != 0)) &&
        (result = can_pay_life_impl(player, life_to_pay, 0), result != 0))
    {
      return 1;
    }
  }
  else
  {
    if (event == EVENT_ACTIVATE)
    {
      result =
          charge_mana_for_activated_ability(player, card, cless, black, blue, green, red, white);
      if (result == 0)
      {
        return 0;
      }
      lose_life(player, life_to_pay);
      if (0 < p_pump)
      {
        return 0;
      }
      if (0 < t_pump)
      {
        return 0;
      }
      if ((k_pump == 0) ||
          ((result = check_for_ability(player, card, k_pump),
            result == 0 && (instance = has_activation_on_stack(player, card), instance == NULL))))
      {
        if (sp_key_pump == 0)
        {
          return 0;
        }
        result = check_for_special_ability(player, card, sp_key_pump);
        if ((result == 0) && (instance = has_activation_on_stack(player, card), instance == NULL))
        {
          return 0;
        }
      }
      ai_modifier = ai_modifier + -0x37;
      return 0;
    }
    if (event == EVENT_RESOLVE_ACTIVATION)
    {
      instance = get_card_instance(player, card);
      result = instance->parent_controller;
      card_00 = instance->parent_card;
      instance = in_play(result, card_00);
      if (instance == NULL)
      {
        return 0;
      }
      if (99 < p_pump)
      {
        add_counters(player, card, result, card_00, COUNTER_P1_P1, p_pump + -100);
        if (k_pump == 0 && sp_key_pump == 0)
        {
          return 0;
        }
        t_pump = 0;
        p_pump = 0;
      }
      pump_ability_until_eot(result, card_00, result, card_00, p_pump, t_pump, k_pump, sp_key_pump);
      return 0;
    }
  }
  if (1 < event - EVENT_POW_BOOST)
  {
    return 0;
  }
  if (p_pump < 100)
  {
    if (event != EVENT_POW_BOOST)
    {
      iStack_18 = t_pump;
    }
  }
  else
  {
    iStack_18 = p_pump + -100;
  }
  result = generic_shade_amt_can_pump(player, card, iStack_18, life_to_pay, cless, black, blue,
                                      green, red, white, -1);
  return result;
}

/* Just like generic_shade(), but can't pump abilities or add counters, and sends its effect through pump_until_eot_merge_previous() so they combine into a
 * single effect card.  Requires that the card's effect text uses |n substitutions. */
int generic_shade_merge_pt(int player, int card, event_t event, int life_to_pay, int cless,
                           int black, int blue, int green, int red, int white, int p_pump,
                           int t_pump)
{
  if (event == EVENT_CAN_ACTIVATE && can_use_activated_abilities(player, card) &&
      has_mana_for_activated_ability(player, card, cless, black, blue, green, red, white) &&
      can_pay_life(player, life_to_pay))
    return 1;

  if (event == EVENT_ACTIVATE &&
      charge_mana_for_activated_ability(player, card, cless, black, blue, green, red, white))
    lose_life(player, life_to_pay);

  if (event == EVENT_RESOLVE_ACTIVATION)
  {
    card_instance_t *instance = get_card_instance(player, card);
    int p = instance->parent_controller, c = instance->parent_card;
    if (in_play(p, c))
      pump_until_eot_merge_previous_alt(player, card, p, c, p_pump, t_pump, 1);
  }

  if (event == EVENT_POW_BOOST || event == EVENT_TOU_BOOST)
    return generic_shade_amt_can_pump(player, card, event == EVENT_POW_BOOST ? p_pump : t_pump,
                                      life_to_pay, cless, black, blue, green, red, white, -1);

  return 0;
}

// If p_pump >= 100, adds p_pump-100 +1/+1 counters, doesn't add power, and ignores t_pump.
int generic_husk(int player, int card, event_t event, int type, int p_pump, int t_pump, int k_pump,
                 int sp_key_pump)
{
  int card_00;
  card_instance_t *instance;
  int result;
  unsigned int uVar3;
  test_definition_t test;
  if (event - EVENT_POW_BOOST < 2)
  {
    if (event == EVENT_POW_BOOST)
    {
      t_pump = p_pump;
    }
    if (99 < p_pump)
    {
      t_pump = p_pump + -100;
    }
    if (0 < t_pump)
    {
      new_default_test_definition(&test, type, "");
      test.not_me = 1;
      result = max_can_sacrifice_as_cost(player, card, &test);
      if (0 < result)
      {
        result = generic_shade_amt_can_pump(player, card, t_pump, 0, 0, 0, 0, 0, 0, 0, result);
        return result;
      }
    }
  }
  else if ((event - EVENT_RESOLVE_ACTIVATION < 2) || (event == EVENT_ACTIVATE))
  {
    if (event == EVENT_CAN_ACTIVATE)
    {
      result = can_use_activated_abilities(player, card);
      if ((result != 0) &&
          (result = has_mana_for_activated_ability(player, card, 0, 0, 0, 0, 0, 0), result != 0))
      {
        result = can_sacrifice_type_as_cost(player, 1, type);
        return (unsigned int)(result != 0);
      }
    }
    else if (event == EVENT_ACTIVATE)
    {
      result = charge_mana_for_activated_ability(player, card, 0, 0, 0, 0, 0, 0);
      if (result != 0)
      {
        uVar3 =
            controller_sacrifices_a_permanent(player, card, type, SAC_RETURN_CHOICE | SAC_AS_COST);
        if (uVar3 == 0)
        {
          spell_fizzled = 1;
        }
        else if (((player == 1) && (card == uVar3 >> 0x18)) && ((char)(uVar3 >> 0x10) == '\x01'))
        {
          ai_modifier = ai_modifier + -0x60;
        }
      }
    }
    else if (event == EVENT_RESOLVE_ACTIVATION)
    {
      instance = get_card_instance(player, card);
      result = instance->parent_controller;
      card_00 = instance->parent_card;
      instance = in_play(result, card_00);
      if (instance != NULL)
      {
        if (p_pump < 100)
        {
          if ((k_pump == 0 && sp_key_pump == 0) &&
              (((0 < p_pump || ((p_pump == 0 && (0 < t_pump)))) ||
                ((p_pump < 0 && (t_pump == 0))))))
          {
            pump_until_eot_merge_previous_alt(result, card_00, result, card_00, p_pump, t_pump, 0);
            return 0;
          }
        }
        else
        {
          add_counters(player, card, result, card_00, COUNTER_P1_P1, p_pump + -100);
          if (k_pump == 0 && sp_key_pump == 0)
          {
            return 0;
          }
          t_pump = 0;
          p_pump = 0;
        }
        pump_ability_until_eot(result, card_00, result, card_00, p_pump, t_pump, k_pump,
                               sp_key_pump);
        return 0;
      }
    }
  }
  return 0;
}

// Like generic_shade, but includes T in activation cost.  Only supports modifications to power or toughness, due to the limited card pool.
int generic_shade_tap(int player, int card, event_t event, int cless, int black, int blue, int green, int red, int white, int power, int toughness)
{
  if (event == EVENT_RESOLVE_ACTIVATION)
	{
	  card_instance_t* instance = get_card_instance(player, card);
	  int p = instance->parent_controller, c = instance->parent_card;
	  if (in_play(p, c))
		pump_ability_until_eot(p, c, p, c, power, toughness, 0, 0);
	}

  if (event == EVENT_POW_BOOST || event == EVENT_TOU_BOOST)
	{
	  int pump = event == EVENT_POW_BOOST ? power : toughness;
	  if (pump > 0
		  && generic_shade_tap(player, card, EVENT_CAN_ACTIVATE, cless, black, blue, green, red, white, power, toughness))
		return pump;
	}

  return generic_activated_ability(player, card, event, GAA_UNTAPPED, cless, black, blue, green, red, white, 0, NULL, NULL);
}



// Only needs to be called during EVENT_GRAVEYARD_FROM_PLAY and TRIGGER_GRAVEYARD_FROM_PLAY.  (More to the point, only need to initialize test then.)
int store_and_trigger_graveyard_from_play(int player, int card, int event, int t_player,
                                          int trigger_mode, test_definition_t *test,
                                          int triggering_player, int triggering_card)
{
  card_instance_t *aff;
  if (event == EVENT_GRAVEYARD_FROM_PLAY &&
      (affected_card_controller == t_player || t_player == ANYBODY) &&
      (aff = in_play(affected_card_controller, affected_card)) &&
      !(test->not_me == 1 && affect_me(player, card)) && aff->kill_code > 0 &&
      aff->kill_code < KILL_EXILE &&
      new_make_test_in_play(affected_card_controller, affected_card, -1, test) &&
      !check_for_death_trigger_removal(affected_card_controller, affected_card))
  {
    card_instance_t *instance = get_card_instance(triggering_player, triggering_card);

    if (instance->targets[11].player < 0)
      instance->targets[11].player = 0;

    if (instance->targets[11].player < 10)
    {
      instance->targets[instance->targets[11].player].player = affected_card_controller;
      instance->targets[instance->targets[11].player].card = affected_card;
      instance->targets[11].player++;
    }
  }

  if (trigger_condition == TRIGGER_GRAVEYARD_FROM_PLAY &&
      triggering_player == reason_for_trigger_controller &&
      affect_me(triggering_player, triggering_card))
  {
    card_instance_t *instance = get_card_instance(triggering_player, triggering_card);
    if (instance->targets[11].player > 0)
    {
      if (event == EVENT_TRIGGER)
      {
        if (trigger_mode == RESOLVE_TRIGGER_DUH)
          trigger_mode =
              duh_mode(triggering_player) ? RESOLVE_TRIGGER_MANDATORY : RESOLVE_TRIGGER_OPTIONAL;
        event_result |= trigger_mode;
      }

      if (event == EVENT_RESOLVE_TRIGGER)
      {
        int num = instance->targets[11].player;
        instance->targets[11].player = 0;
        return num;
      }

      if (event == EVENT_END_TRIGGER)
        instance->targets[11].player = 0;
    }
  }

  return 0;
}

static int is_dying(int player, int card){
	if( ! in_play(player, card) ){
		return 0;
	}
	if( check_special_flags2(player, card, SF2_WILL_BE_EXILED_IF_PUTTED_IN_GRAVE) ){
		return 0;
	}
	int kill_code = get_card_instance(player, card)->kill_code;
	if( kill_code < KILL_BURY || kill_code > KILL_SACRIFICE ){
		return 0;
	}
	return 1;
}











int count_graveyard_from_play(int player, int card, int event, int trg, int type, int flag1,
                              int subtype, int flag2, int color, int flag3, int id, int flag4,
                              int cc, int flag5)
{

  int all_players = 0;
  int not_me = 0;
  int opt_trigger = 0;
  if (trg & 2)
  {
    all_players = 1;
  }
  if (trg & 4)
  {
    trg -= 4;
    not_me = 1;
  }
  if (trg & 8)
  {
    trg -= 8;
    opt_trigger = 1;
  }

  card_instance_t *instance = get_card_instance(player, card);

  if (event == EVENT_GRAVEYARD_FROM_PLAY && (affected_card_controller == trg || all_players == 1))
  {
    if (!in_play(affected_card_controller, affected_card))
    {
      return 0;
    }
    if (affect_me(player, card) && not_me == 1)
    {
      return 0;
    }
    if (check_for_death_trigger_removal(affected_card_controller, affected_card))
    {
      return 0;
    }
    if (make_test_in_play(affected_card_controller, affected_card, -1, type, flag1, subtype, flag2,
                          color, flag3, id, flag4, cc, flag5))
    {
      card_instance_t *affected = get_card_instance(affected_card_controller, affected_card);
      if (affected->kill_code > 0 && affected->kill_code < 4)
      {
        if (instance->targets[11].player < 0)
        {
          instance->targets[11].player = 0;
        }
        instance->targets[11].player++;
        if (get_id(player, card) == CARD_ID_KRESH_THE_BLOODBRAIDED ||
            get_id(player, card) == CARD_ID_GRAND_OSSUARY)
        {
          if (instance->targets[1].player < 0)
          {
            instance->targets[1].player = 0;
          }
          instance->targets[1].player += get_power(affected_card_controller, affected_card);
        }
        if (instance->targets[0].card == CARD_ID_SHOWSTOPPER)
        {
          if (instance->targets[1].player < 2)
          {
            instance->targets[1].player = 2;
          }
          int pos = instance->targets[1].player;
          if (pos < 18)
          {
            instance->targets[pos].player =
                get_protections_from(affected_card_controller, affected_card);
          }
        }
      }
    }
  }

  if (instance->targets[11].player > 0 && trigger_condition == TRIGGER_GRAVEYARD_FROM_PLAY &&
      player == reason_for_trigger_controller && affect_me(player, card))
  {
    if (event == EVENT_TRIGGER)
    {
      if (opt_trigger == 1)
      {
        event_result |= 1 + player;
      }
      else
      {
        event_result |= RESOLVE_TRIGGER_MANDATORY;
      }
    }
    else if (event == EVENT_RESOLVE_TRIGGER)
    {
      instance->targets[11].card = instance->targets[11].player;
      instance->targets[11].player = 0;
      return 1;
    }
    else if (event == EVENT_END_TRIGGER)
    {
      instance->targets[11].player = 0;
    }
  }
  return 0;
}



int dont_account_for_trinisphere = 0;
int havengul_lich_hack = 0;
// exactly one of card and iid must be >= 0.  Player must always be valid.
// this could return even negative values, used by Buyback and similar costs modifier.
int true_get_updated_casting_cost(int player, int card, int iid, event_t event, int orig_cless)
{
  card_ptr_t *pcVar1;
  bool bVar2;
  bool bVar3;
  char cVar4;
  int iVar5;
  bool bVar6;
  unsigned int uVar7;
  card_instance_t *instance;
  int iVar9;
  unsigned int uVar10;
  int iVar11;
  int iVar12;
  unsigned int uVar13;
  int card_00;
  int player_00;
  unsigned int uStack_14;

  if (1 < (unsigned int)player)
  {
    backtrace_and_abort("functions/functions.c:10186\nplayer == 0 || player == 1 || "
                        "!\"get_updated_casting_cost() needs a real player\"");
  }
  if ((char)-((char)((unsigned char)((unsigned int)card >> 0x18) ^ 0xff) >> 7) !=
      (char)-(char)(iid >> 0x1f))
  {
    backtrace_and_abort(
        "functions/functions.c:10187\n(card < 0) != (iid < 0) && \"get_updated_casting_cost() "
        "needs exactly one of card and iid to be valid\"");
  }
  if (iid < 0)
  {
    instance = get_card_instance(player, card);
    iid = instance->internal_card_id;
  }
  if ((cards_data[iid].type & 1) != 0)
  {
    return 0;
  }
  uVar13 = (unsigned int)cards_data[iid].id;
  pcVar1 = cards_ptr[uVar13];
  uStack_14 = orig_cless;
  if (orig_cless == -1)
  {
    uStack_14 = (unsigned int)pcVar1->req_colorless;
    if (uStack_14 < 0x10)
      goto LAB_026898ba;
  }
  else if (orig_cless < 0x10)
    goto LAB_026898ba;
  uStack_14 = 0;
LAB_026898ba:
  player_00 = 0;
  uVar7 = get_color_by_internal_id(player, iid);
  bVar3 = false;
  bVar2 = false;
  while (active_cards_count[player_00] < 1)
  {
  LAB_026899ce:
    player_00 = player_00 + 1;
    if (player_00 == 2)
    {
      if (((bVar2) && ((int)uStack_14 < 3)) && (dont_account_for_trinisphere < 1))
      {
        if (orig_cless == -1)
        {
          uVar13 = (unsigned int)pcVar1->req_black + (unsigned int)pcVar1->req_blue +
                   (unsigned int)pcVar1->req_green + (unsigned int)pcVar1->req_red +
                   (unsigned int)pcVar1->req_white;
          if (uVar13 < 3)
          {
            uStack_14 = 3 - uVar13;
          }
        }
        else
        {
          uStack_14 = 3 - orig_cless;
        }
      }
      return uStack_14 + havengul_lich_hack;
    }
  }
  card_00 = 0;
LAB_02689900:
  instance = in_play(player_00, card_00);
  bVar6 = bVar3;
  if ((instance != NULL) &&
      ((iVar9 = is_what(player_00, card_00, 0x80),
        iVar9 != 0 || (iVar9 = is_humiliated(player_00, card_00, AL_STATIC), iVar9 == 0))))
  {
    uVar10 = get_id(player_00, card_00);
    iVar5 = affected_card;
    iVar12 = affected_card_controller;
    iVar9 = event_result;
    cVar4 = COST_COLORLESS;
    if (uVar10 == 0x2470)
    {
      if ((player == player_00) &&
          ((iVar9 = has_subtype_by_id(player, uVar13, SUBTYPE_TREEFOLK),
            iVar9 != 0 ||
                (iVar9 = has_subtype_by_id(player, uVar13, SUBTYPE_SHAMAN), iVar9 != 0))))
      {
        uStack_14 = uStack_14 - 1;
      }
      goto LAB_026899c0;
    }
    if ((int)uVar10 < 0x2471)
    {
      if (uVar10 == 0x14bf)
      {
        if (player == player_00)
        {
          uVar10 = get_sleighted_color_test(player, card_00, 0x22);
          uStack_14 = (uStack_14 - 1) + (unsigned int)((uVar10 & uVar7) == 0);
        }
      }
      else if ((int)uVar10 < 0x14c0)
      {
        if (uVar10 == 0xb78)
        {
          if ((player != player_00) && (iVar9 = is_what(-1, iid, 0x44), iVar9 != 0))
          {
            uStack_14 = uStack_14 + 2;
          }
        }
        else if ((int)uVar10 < 0xb79)
        {
          if (uVar10 == 0x3f4)
          {
            if (current_turn != player)
            {
              card_00 = card_00 + 1;
              uStack_14 = uStack_14 + 3;
              COST_COLORLESS = cVar4;
              event_result = iVar9;
              affected_card_controller = iVar12;
              affected_card = iVar5;
              if (active_cards_count[player_00] <= card_00)
                goto LAB_026899ce;
              goto LAB_02689900;
            }
          }
          else
          {
            if (0x3f4 < (int)uVar10)
            {
              if (uVar10 == 0x883)
              {
              LAB_02689f50:
                iVar9 = 2;
              LAB_02689f58:
                iVar9 = is_what(-1, iid, iVar9);
                if (iVar9 != 0)
                {
                  return 99;
                }
                card_00 = card_00 + 1;
                if (active_cards_count[player_00] <= card_00)
                  goto LAB_026899ce;
              }
              else
              {
                if ((int)uVar10 < 0x884)
                {
                  if (uVar10 == 0x66a)
                    goto LAB_0268a538;
                  if ((uVar10 == 0x756) && (instance->damage_target_player == player))
                    goto LAB_02689f50;
                  goto LAB_026899c0;
                }
                if (uVar10 == 0x89b)
                {
                LAB_0268a538:
                  iVar11 = (int)COST_COLORLESS;
                  event_result = player;
                  if (card < 0)
                  {
                    affected_card = iid;
                    affected_card_controller = -1;
                  }
                  else
                  {
                    affected_card_controller = player;
                    affected_card = card;
                  }
                  call_card_function(player_00, card_00, EVENT_MODIFY_COST_GLOBAL);
                  uStack_14 = uStack_14 + (COST_COLORLESS - iVar11);
                  card_00 = card_00 + 1;
                  COST_COLORLESS = cVar4;
                  event_result = iVar9;
                  affected_card_controller = iVar12;
                  affected_card = iVar5;
                  if (active_cards_count[player_00] <= card_00)
                    goto LAB_026899ce;
                }
                else
                {
                  if (uVar10 != 0xa9f)
                    goto LAB_026899c0;
                  card_00 = card_00 + 1;
                  uStack_14 = uStack_14 - 1;
                  COST_COLORLESS = cVar4;
                  event_result = iVar9;
                  affected_card_controller = iVar12;
                  affected_card = iVar5;
                  if (active_cards_count[player_00] <= card_00)
                    goto LAB_026899ce;
                }
              }
              goto LAB_02689900;
            }
            if (uVar10 == 0x2a9)
            {
              iVar9 = is_what(-1, iid, 0x10);
              if ((iVar9 != 0) && (iVar9 = is_what(-1, iid, 2), iVar9 == 0))
              {
                iVar9 = get_cmc_by_id(uVar13);
                uStack_14 = uStack_14 + iVar9;
              }
            }
            else if ((int)uVar10 < 0x2aa)
            {
              if (uVar10 == 100)
              {
                if ((uVar7 & 0x20) != 0)
                {
                  card_00 = card_00 + 1;
                  uStack_14 = uStack_14 + 3;
                  COST_COLORLESS = cVar4;
                  event_result = iVar9;
                  affected_card_controller = iVar12;
                  affected_card = iVar5;
                  if (active_cards_count[player_00] <= card_00)
                    goto LAB_026899ce;
                  goto LAB_02689900;
                }
              }
              else if (uVar10 == 0x16b)
              {
                uStack_14 = uStack_14 - (player == player_00);
                card_00 = card_00 + 1;
                COST_COLORLESS = cVar4;
                event_result = iVar9;
                affected_card_controller = iVar12;
                affected_card = iVar5;
                if (active_cards_count[player_00] <= card_00)
                  goto LAB_026899ce;
                goto LAB_02689900;
              }
            }
            else if (uVar10 == 0x2d1)
            {
              if ((player == player_00) &&
                  ((iVar9 = is_what(-1, iid, 0x10),
                    iVar9 != 0 || (iVar9 = is_what(-1, iid, 4), iVar9 != 0))))
              {
                uStack_14 = uStack_14 - 2;
              }
            }
            else if (uVar10 == 0x2e8)
              goto LAB_0268a518;
          }
        }
        else if (uVar10 == 0xcb8)
        {
          if (player == player_00)
          {
            uStack_14 = (uStack_14 - 1) + (unsigned int)((uVar7 & 0x10) == 0);
          }
        }
        else if ((int)uVar10 < 0xcb9)
        {
          if (uVar10 == 0xc6c)
          {
            if (player == player_00)
            {
              uStack_14 = (uStack_14 - 1) + (unsigned int)((uVar7 & 2) == 0);
            }
          }
          else if ((int)uVar10 < 0xc6d)
          {
            if (uVar10 == 0xbe3)
            {
            LAB_0268b118:
              if (player == player_00)
              {
                iVar9 = 2;
              LAB_0268a404:
                uVar10 = is_what(-1, iid, iVar9);
              joined_r0x0268a471:
                if (uVar10 != 0)
                {
                  return 0x31;
                }
              }
            }
            else if ((uVar10 == 0xc3c) && (player == player_00))
            {
              uStack_14 = (uStack_14 - 1) + (unsigned int)((uVar7 & 8) == 0);
            }
          }
          else if ((uVar10 == 0xc99) && (player == player_00))
          {
            uStack_14 = (uStack_14 - 1) + (unsigned int)((uVar7 & 0x20) == 0);
          }
        }
        else if (uVar10 == 0xe70)
        {
        LAB_02689dce:
          uVar10 = instance->targets[1].player;
          if (0 < (int)uVar10)
          {
            if ((player + 1U & uVar10) != 0)
            {
              return 99;
            }
            card_00 = card_00 + 1;
            if (active_cards_count[player_00] <= card_00)
              goto LAB_026899ce;
            goto LAB_02689900;
          }
        }
        else if ((int)uVar10 < 0xe71)
        {
          if (uVar10 != 0xcbd)
          {
            if (uVar10 != 0xdee)
              goto LAB_026899c0;
            card_00 = card_00 + 1;
            uStack_14 = uStack_14 + 1;
            COST_COLORLESS = cVar4;
            event_result = iVar9;
            affected_card_controller = iVar12;
            affected_card = iVar5;
            if (active_cards_count[player_00] <= card_00)
              goto LAB_026899ce;
            goto LAB_02689900;
          }
          if (player == player_00)
          {
            uStack_14 = (uStack_14 - 1) + (unsigned int)((uVar7 & 4) == 0);
          }
        }
        else
        {
          if (uVar10 == 0x148b)
          {
          LAB_0268ab50:
            if (instance->targets[1].card == uVar13)
            {
              return 0x31;
            }
            card_00 = card_00 + 1;
            COST_COLORLESS = cVar4;
            event_result = iVar9;
            affected_card_controller = iVar12;
            affected_card = iVar5;
            if (active_cards_count[player_00] <= card_00)
              goto LAB_026899ce;
            goto LAB_02689900;
          }
          if ((uVar10 == 0x1495) && (player == player_00))
          {
            uVar10 = get_sleighted_color_test(player, card_00, 0x14);
            uStack_14 = (uStack_14 - 1) + (unsigned int)((uVar10 & uVar7) == 0);
          }
        }
      }
      else if (uVar10 == 0x2018)
      {
        if (player != player_00)
        {
          card_00 = card_00 + 1;
          uStack_14 = uStack_14 + 1;
          COST_COLORLESS = cVar4;
          event_result = iVar9;
          affected_card_controller = iVar12;
          affected_card = iVar5;
          if (active_cards_count[player_00] <= card_00)
            goto LAB_026899ce;
          goto LAB_02689900;
        }
        uStack_14 = uStack_14 - 1;
      }
      else if ((int)uVar10 < 0x2019)
      {
        if (uVar10 == 0x1989)
        {
          if (player == player_00)
          {
            iVar9 = has_subtype_by_id(player, uVar13, SUBTYPE_GOBLIN);
            uStack_14 = (uStack_14 - 1) + (unsigned int)(iVar9 == 0);
          }
        }
        else if ((int)uVar10 < 0x198a)
        {
          if (uVar10 == 0x14cb)
          {
            if (player == player_00)
            {
              uVar10 = get_sleighted_color_test(player_00, card_00, 10);
              uStack_14 = (uStack_14 - 1) + (unsigned int)((uVar10 & uVar7) == 0);
            }
          }
          else if ((int)uVar10 < 0x14cc)
          {
            if (uVar10 == 0x14c4)
            {
              if (player == player_00)
              {
                uVar10 = get_sleighted_color_test(player, card_00, 0xc);
                uStack_14 = (uStack_14 - 1) + (unsigned int)((uVar10 & uVar7) == 0);
              }
            }
            else if ((uVar10 == 0x14c9) && (player == player_00))
            {
              uVar10 = get_sleighted_color_test(player, card_00, 0x30);
              uStack_14 = (uStack_14 - 1) + (unsigned int)((uVar10 & uVar7) == 0);
            }
          }
          else if (uVar10 == 0x17ec)
          {
          LAB_0268a939:
            if ((player == player_00) && (cards_data[iid].id == 0x33c4))
            {
              uStack_14 = uStack_14 - 1;
            }
          }
          else if (((uVar10 == 0x1974) && (player == player_00)) &&
                   (iVar9 = has_subtype_by_id(player, uVar13, SUBTYPE_DRAGON), iVar9 != 0))
          {
            uStack_14 = uStack_14 - 2;
          }
        }
        else
        {
          if (uVar10 == 0x1b47)
          {
            card_00 = card_00 + 1;
            bVar2 = true;
            COST_COLORLESS = cVar4;
            event_result = iVar9;
            affected_card_controller = iVar12;
            affected_card = iVar5;
            if (active_cards_count[player_00] <= card_00)
              goto LAB_026899ce;
            goto LAB_02689900;
          }
          if ((int)uVar10 < 0x1b48)
          {
            if (uVar10 == 0x1a19)
              goto LAB_0268b118;
            if (uVar10 == 0x1a73)
              goto LAB_02689dce;
            if ((uVar10 == 0x19c6) && (player == player_00))
            {
              iVar9 = has_subtype_by_id(player, uVar13, SUBTYPE_ZOMBIE);
              uStack_14 = (uStack_14 - 1) + (unsigned int)(iVar9 == 0);
            }
          }
          else if (((uVar10 == 0x1bb3) && (iVar9 = is_what(-1, iid, 0x40), iVar9 != 0)) &&
                   (iVar9 = is_what(-1, iid, 2), iVar9 != 0))
          {
            iVar9 = count_subtype(player, 0x40, -1);
            uStack_14 = uStack_14 - iVar9;
          }
        }
      }
      else if (uVar10 == 0x22c8)
      {
        iVar9 = instance->targets[1].card;
        if (0 < iVar9)
        {
          iVar9 = is_what(-1, iid, iVar9);
          uStack_14 = (uStack_14 - 1) + (unsigned int)(iVar9 == 0);
          card_00 = card_00 + 1;
          if (active_cards_count[player_00] <= card_00)
            goto LAB_026899ce;
          goto LAB_02689900;
        }
      }
      else if ((int)uVar10 < 0x22c9)
      {
        if (uVar10 == 0x22ac)
        {
        LAB_0268a17f:
          if (instance->targets[1].card == uVar13)
          {
            return 99;
          }
          card_00 = card_00 + 1;
          COST_COLORLESS = cVar4;
          event_result = iVar9;
          affected_card_controller = iVar12;
          affected_card = iVar5;
          if (active_cards_count[player_00] <= card_00)
            goto LAB_026899ce;
          goto LAB_02689900;
        }
        if ((int)uVar10 < 0x22ad)
        {
          if (uVar10 == 0x2195)
          {
            if (player == player_00)
            {
              iVar9 = count_graveyard_by_id(player, uVar13);
              uStack_14 = uStack_14 - iVar9;
            }
          }
          else if ((uVar10 == 0x21f5) && (player != player_00))
          {
            iVar9 = can_sorcery_be_played(player, event);
            if (iVar9 == 0)
            {
              return 0x31;
            }
            card_00 = card_00 + 1;
            if (active_cards_count[player_00] <= card_00)
              goto LAB_026899ce;
            goto LAB_02689900;
          }
        }
        else if ((((uVar10 == 0x22c5) &&
                   (iVar9 = is_tapped(player_00, card_00), player == player_00)) &&
                  (iVar9 != 0)) &&
                 (iVar9 = is_what(-1, iid, 2), iVar9 != 0))
        {
          uStack_14 = uStack_14 - 2;
        }
      }
      else if (uVar10 == 0x2438)
      {
        if ((player == player_00) &&
            (iVar9 = has_subtype_by_id(player, uVar13, SUBTYPE_GIANT), iVar9 != 0))
        {
          uStack_14 = uStack_14 - 2;
        }
      }
      else if ((int)uVar10 < 0x2439)
      {
        if (((uVar10 == 0x23ba) && (iVar9 = is_what(-1, iid, 2), iVar9 == 0)) &&
            (iVar9 = get_cmc_by_id(uVar13), 3 < iVar9))
        {
          return 99;
        }
      }
      else
      {
        if (uVar10 == 0x2444)
        {
        LAB_0268a7e3:
          iVar9 = is_what(-1, iid, 2);
          uStack_14 = uStack_14 + (iVar9 == 0);
          card_00 = card_00 + 1;
          if (active_cards_count[player_00] <= card_00)
            goto LAB_026899ce;
          goto LAB_02689900;
        }
        if (((uVar10 == 0x246b) && (player == player_00)) &&
            ((iVar9 = has_subtype_by_id(player, uVar13, SUBTYPE_SOLDIER),
              iVar9 != 0 ||
                  (iVar9 = has_subtype_by_id(player, uVar13, SUBTYPE_KITHKIN), iVar9 != 0))))
        {
          uStack_14 = uStack_14 - 1;
        }
      }
    }
    else
    {
      if (uVar10 == 0x306c)
      {
        if ((player == player_00) &&
            ((iVar9 = has_subtype_by_id(player, uVar13, SUBTYPE_ANGEL),
              iVar9 != 0 ||
                  (iVar9 = has_subtype_by_id(player, uVar13, SUBTYPE_HUMAN), iVar9 != 0))))
        {
          iVar9 = count_counters(player_00, card_00, COUNTER_P1_P1);
          uStack_14 = uStack_14 - iVar9;
        }
        goto LAB_026899c0;
      }
      if (0x306c < (int)uVar10)
      {
        if (uVar10 == 0x3f84)
        {
          if (player == player_00)
          {
            iVar9 = is_what(-1, iid, 0x38);
            if (iVar9 != 0)
            {
              COST_WHITE = 'd';
              COST_RED = 'd';
              COST_GREEN = 'd';
              COST_BLUE = 'd';
              COST_BLACK = 'd';
              COST_COLORLESS = 'd';
            }
            iVar9 = is_what(-1, iid, 2);
            if ((iVar9 != 0) && (iVar9 = get_stormcreature_count(player_00), iVar9 == 0))
            {
              COST_COLORLESS = COST_COLORLESS + -1;
            }
          }
          goto LAB_026899c0;
        }
        if (0x3f84 < (int)uVar10)
        {
          if (uVar10 == 0x4258)
          {
            if ((player == player_00) && (iVar9 = is_what(-1, iid, 2), iVar9 != 0))
            {
              uVar10 = get_sleighted_color_test(player, card_00, 0x20);
              uStack_14 = (uStack_14 - 1) + (unsigned int)((uVar10 & uVar7) == 0);
            }
          }
          else if ((int)uVar10 < 0x4259)
          {
            if (uVar10 == 0x41d6)
            {
              if ((player == player_00) && (iVar9 = is_what(-1, iid, 2), iVar9 != 0))
              {
                uVar10 = get_sleighted_color_test(player_00, card_00, 2);
                uStack_14 = (uStack_14 - 1) + (unsigned int)((uVar10 & uVar7) == 0);
              }
            }
            else if ((int)uVar10 < 0x41d7)
            {
              if (uVar10 == 0x3ffd)
                goto LAB_0268a518;
              if (uVar10 == 0x407a)
              {
                iVar9 = instance->info_slot;
                iVar12 = is_what(-1, iid, 2);
                if ((iVar12 == 0) && (iVar12 = get_cmc_by_internal_id(iid), iVar12 == iVar9))
                {
                  COST_WHITE = 'd';
                  COST_RED = 'd';
                  COST_GREEN = 'd';
                  COST_BLUE = 'd';
                  COST_BLACK = 'd';
                  COST_COLORLESS = 'd';
                }
              }
            }
            else if (uVar10 == 0x4229)
            {
              if ((player == player_00) && (iVar9 = is_what(-1, iid, 2), iVar9 != 0))
              {
                uVar10 = get_sleighted_color_test(player, card_00, 0x10);
                uStack_14 = (uStack_14 - 1) + (unsigned int)((uVar10 & uVar7) == 0);
              }
            }
            else if (((uVar10 == 0x423a) && (player == player_00)) &&
                     (iVar9 = is_what(-1, iid, 2), iVar9 != 0))
            {
              uVar10 = get_sleighted_color_test(player, card_00, 4);
              uStack_14 = (uStack_14 - 1) + (unsigned int)((uVar10 & uVar7) == 0);
            }
          }
          else if (uVar10 == 0x591b)
          {
            if ((player == player_00) && (iVar9 = is_what(-1, iid, 0x38), iVar9 != 0))
            {
              iVar9 = get_cmc_by_id(uVar13);
              uStack_14 = uStack_14 - (4 < iVar9);
            }
          }
          else if ((int)uVar10 < 0x591c)
          {
            if (uVar10 == 0x426f)
            {
              if ((player == player_00) && (iVar9 = is_what(-1, iid, 2), iVar9 != 0))
              {
                uVar10 = get_sleighted_color_test(player, card_00, 8);
                uStack_14 = (uStack_14 - 1) + (unsigned int)((uVar10 & uVar7) == 0);
              }
            }
            else if ((uVar10 == 0x5673) && (player == player_00))
            {
              iVar9 = count_creatures_with_p1_p1_counter(player);
              uStack_14 = uStack_14 - iVar9;
            }
          }
          else
          {
            if (((uVar10 == 0x6160) && (uVar10 = instance->targets[0].card, (uVar10 & 6) != 0)) &&
                ((iVar9 = instance->targets[0].player, iVar9 == 2 || (iVar9 == player))))
            {
              if (((uVar10 & 2) != 0) &&
                  ((unsigned int)cards_data[iid].id == instance->targets[1].player))
              {
                return 99;
              }
              if (((uVar10 & 4) != 0) && (iVar9 = instance->targets[1].player, -1 < iVar9))
                goto LAB_02689f58;
            }
          }
          goto LAB_026899c0;
        }
        if (uVar10 == 0x35a4)
        {
          if ((player == player_00) && (iVar9 = has_subtype(-1, iid, SUBTYPE_AURA), iVar9 != 0))
          {
            iVar9 = is_humiliated(player, card_00, AL_STATIC);
            uStack_14 = uStack_14 - (iVar9 == 0);
          }
          goto LAB_026899c0;
        }
        if ((int)uVar10 < 0x35a5)
        {
          if (uVar10 == 0x31ff)
          {
            if ((player == player_00) && (iVar9 = is_what(-1, iid, 2), iVar9 != 0))
            {
              iVar9 = get_trap_condition(1 - player, 2);
              uStack_14 = uStack_14 - iVar9;
            }
          }
          else if ((int)uVar10 < 0x3200)
          {
            if (uVar10 == 0x3136)
            {
              if (player == player_00)
              {
                uStack_14 = uStack_14 - pcVar1->req_colorless;
              }
            }
            else if (((uVar10 == 0x31b9) && (player == player_00)) &&
                     (iVar9 = is_what(-1, iid, 2), iVar9 == 0))
            {
              iVar9 = is_what(-1, iid, 0x38);
              uStack_14 = (uStack_14 - 1) + (unsigned int)(iVar9 == 0);
            }
          }
          else if ((uVar10 == 0x3351) && (instance = get_card_instance(player_00, card_00),
                                          instance->targets[1].card == uVar13))
          {
            if (player != player_00)
            {
              return 99;
            }
            uStack_14 = uStack_14 - 2;
          }
          goto LAB_026899c0;
        }
        if (uVar10 == 0x3ae4)
        {
        LAB_0268aa1d:
          if (player != player_00)
          {
            bVar3 = true;
            goto LAB_0268a538;
          }
          goto LAB_026899c0;
        }
        if (0x3ae4 < (int)uVar10)
        {
          if (uVar10 != 0x3c7c)
          {
            if ((int)uVar10 < 0x3c7d)
            {
              if (uVar10 == 0x3b91)
                goto LAB_0268a7e3;
              if ((int)uVar10 < 0x3b92)
              {
                if ((uVar10 == 0x3b2d) || (uVar10 == 0x3b36))
                  goto LAB_0268a518;
              }
              else if ((uVar10 == 0x3bc3) || (uVar10 == 0x3bf4))
                goto LAB_0268a518;
              goto LAB_026899c0;
            }
            if (uVar10 != 0x3eae)
            {
              if ((int)uVar10 < 0x3eaf)
              {
                if (((uVar10 & 0xfffffeff) == 0x3cb4) || (uVar10 == 0x3caa))
                  goto LAB_0268a518;
              }
              else if ((uVar10 == 0x3ec5) || (uVar10 == 0x3eed))
              {
              LAB_0268a518:
                if ((player == player_00) || (bVar6 = false, bVar3))
                {
                  bVar3 = true;
                  goto LAB_0268a538;
                }
              }
              goto LAB_026899c0;
            }
          }
          goto LAB_0268aa1d;
        }
        if (uVar10 == 0x3953)
        {
          if (instance->targets[5].card == iid)
          {
            if (instance->targets[2].player == iid)
            {
              iVar9 = count_counters(player_00, card_00, COUNTER_VORTEX);
              iVar12 = count_cards_by_id(player, 0x4aa5);
              uVar10 = count_counters(player_00, card_00, COUNTER_VORTEX);
              if ((uVar10 & 1) != 0)
              {
                uVar10 = uVar10 - 1;
              }
              uStack_14 = (iVar9 + uStack_14) - (uVar10 >> 1) * iVar12;
            }
            else if (instance->targets[2].card == iid)
            {
              iVar9 = count_counters(player_00, card_00, COUNTER_VELOCITY);
              iVar12 = count_cards_by_id(player, 0x4aa5);
              uVar10 = count_counters(player_00, card_00, COUNTER_VELOCITY);
              if ((uVar10 & 1) != 0)
              {
                uVar10 = uVar10 - 1;
              }
              uStack_14 = (iVar9 + uStack_14) - (uVar10 >> 1) * iVar12;
            }
          }
          goto LAB_026899c0;
        }
        if (0x3953 < (int)uVar10)
        {
          if (uVar10 == 0x3a78)
            goto LAB_0268a939;
          if ((int)uVar10 < 0x3a79)
          {
            if (uVar10 == 0x3a2f)
              goto LAB_0268aa1d;
            if (uVar10 == 0x3a34)
              goto LAB_0268a518;
          }
          goto LAB_026899c0;
        }
        if (uVar10 == 0x3951)
          goto LAB_0268a518;
        card_00 = card_00 + 1;
        COST_COLORLESS = cVar4;
        event_result = iVar9;
        affected_card_controller = iVar12;
        affected_card = iVar5;
        if (active_cards_count[player_00] <= card_00)
          goto LAB_026899ce;
        goto LAB_02689900;
      }
      if (uVar10 == 0x2b51)
      {
        uVar10 = instance->targets[1].card;
        if (((0 < (int)uVar10) && ((uVar10 & 1) != 0)) && (player != player_00))
        {
          return 99;
        }
      }
      else if ((int)uVar10 < 0x2b52)
      {
        if (uVar10 == 0x26ea)
        {
          iVar9 = is_what(-1, iid, 0x40);
          if (player == player_00)
          {
            uStack_14 = (uStack_14 - 1) + (unsigned int)(iVar9 == 0);
            card_00 = card_00 + 1;
            if (active_cards_count[player_00] <= card_00)
              goto LAB_026899ce;
            goto LAB_02689900;
          }
        }
        else if ((int)uVar10 < 0x26eb)
        {
          if (uVar10 == 0x24e1)
          {
            if ((player == player_00) &&
                ((iVar9 = has_subtype_by_id(player, uVar13, SUBTYPE_MERFOLK),
                  iVar9 != 0 || (iVar9 = has_subtype_by_id(player, uVar13, SUBTYPE_WIZARD),
                                 iVar9 != 0))))
            {
              uStack_14 = uStack_14 - 1;
            }
          }
          else if ((int)uVar10 < 0x24e2)
          {
            if (uVar10 == 0x2472)
            {
              if ((player == player_00) &&
                  ((iVar9 = has_subtype_by_id(player, uVar13, SUBTYPE_ELEMENTAL),
                    iVar9 != 0 || (iVar9 = has_subtype_by_id(player, uVar13, SUBTYPE_WARRIOR),
                                   iVar9 != 0))))
              {
                uStack_14 = uStack_14 - 1;
              }
            }
            else if (((uVar10 == 0x2490) && (player == player_00)) &&
                     ((iVar9 = has_subtype_by_id(player, uVar13, SUBTYPE_GOBLIN),
                       iVar9 != 0 || (iVar9 = has_subtype_by_id(player, uVar13, SUBTYPE_ROGUE),
                                      iVar9 != 0))))
            {
              uStack_14 = uStack_14 - 1;
            }
          }
          else if ((uVar10 == 0x26ae) && (player != player_00))
          {
            if ((instance->targets[1].card == 0x42) && (iVar9 = is_what(-1, iid, 2), iVar9 != 0))
            {
              return 0x31;
            }
            if ((instance->targets[2].card == 0x42) && (iVar9 = is_what(-1, iid, 0x40), iVar9 != 0))
            {
              return 0x31;
            }
            if (instance->targets[3].card == 0x42)
            {
              iVar9 = 4;
              goto LAB_0268a404;
            }
          }
        }
        else if (uVar10 == 0x2a09)
        {
          if (((player == player_00) &&
               (iVar9 = has_subtype_by_id(player, uVar13, SUBTYPE_ELDRAZI), iVar9 != 0)) &&
              ((uVar7 & 0x3e) == 0))
          {
            uStack_14 = uStack_14 - 2;
          }
        }
        else if ((int)uVar10 < 0x2a0a)
        {
          if (uVar10 == 0x26eb)
          {
            iVar9 = is_what(-1, iid, 0x40);
            if (iVar9 == 0)
              goto LAB_02689dce;
            card_00 = card_00 + 1;
            if (active_cards_count[player_00] <= card_00)
              goto LAB_026899ce;
            goto LAB_02689900;
          }
          if (((uVar10 == 0x294b) && (1 - player == player_00)) &&
              (iVar9 = is_what(-1, iid, 1), iVar9 == 0))
          {
            uVar10 = instance->targets[10].card & uVar7;
            goto joined_r0x0268a471;
          }
        }
        else if (uVar10 == 0x2a2c)
        {
          iVar9 = is_what(-1, iid, 0x40);
          uStack_14 = uStack_14 + (iVar9 == 0);
          card_00 = card_00 + 1;
          if (active_cards_count[player_00] <= card_00)
            goto LAB_026899ce;
          goto LAB_02689900;
        }
      }
      else if (uVar10 == 0x2ecf)
      {
        if ((player == player_00) && (iVar9 = is_what(-1, iid, 2), iVar9 != 0))
        {
          uStack_14 = uStack_14 - 2;
        }
      }
      else if ((int)uVar10 < 0x2ed0)
      {
        if (uVar10 == 0x2de8)
        {
          if (current_phase - 0x15U < 9)
          {
            return 99;
          }
          card_00 = card_00 + 1;
          COST_COLORLESS = cVar4;
          event_result = iVar9;
          affected_card_controller = iVar12;
          affected_card = iVar5;
          if (active_cards_count[player_00] <= card_00)
            goto LAB_026899ce;
          goto LAB_02689900;
        }
        if ((int)uVar10 < 0x2de9)
        {
          if (uVar10 == 0x2d4e)
            goto LAB_0268a17f;
          if (((uVar10 == 0x2de7) && (player == player_00)) &&
              (iVar9 = is_what(-1, iid, 2), iVar9 != 0))
          {
            iVar9 = count_counters(player, card_00, COUNTER_P1_P1);
            uStack_14 = uStack_14 - iVar9;
          }
        }
        else if ((uVar10 == 0x2e26) && (player != player_00))
        {
          if (current_turn == player_00)
          {
            return 99;
          }
          card_00 = card_00 + 1;
          COST_COLORLESS = cVar4;
          event_result = iVar9;
          affected_card_controller = iVar12;
          affected_card = iVar5;
          if (active_cards_count[player_00] <= card_00)
            goto LAB_026899ce;
          goto LAB_02689900;
        }
      }
      else if (uVar10 == 0x2f7e)
      {
        if ((instance->targets[0].player == player) && (instance->targets[1].player == 0x42))
        {
          return 0x31;
        }
      }
      else if ((int)uVar10 < 0x2f7f)
      {
        if (uVar10 == 0x2f02)
          goto LAB_0268ab50;
        if (((uVar10 == 0x2f19) && (player == player_00)) &&
            (iVar9 = has_subtype_by_id(player, uVar13, SUBTYPE_ZOMBIE), iVar9 != 0))
        {
          uStack_14 = uStack_14 - pcVar1->req_colorless;
        }
      }
      else
      {
        if (uVar10 == 0x2ff3)
        {
          if (player == player_00)
            goto LAB_026899c0;
          iVar9 = is_what(-1, iid, 2);
          uStack_14 = uStack_14 + (iVar9 == 0);
          card_00 = card_00 + 1;
          if (active_cards_count[player_00] <= card_00)
            goto LAB_026899ce;
          goto LAB_02689900;
        }
        if (((uVar10 == 0x3014) && (iVar9 = is_what(-1, iid, 2), iVar9 == 0)) &&
            (iVar9 = is_what(-1, iid, 0x38), iVar9 != 0))
        {
          uStack_14 = uStack_14 - 2;
        }
      }
    }
  }
LAB_026899c0:
  bVar3 = bVar6;
  card_00 = card_00 + 1;
  if (active_cards_count[player_00] <= card_00)
    goto LAB_026899ce;
  goto LAB_02689900;
}

// This is a frontend and prevent the return of negative values when charging mana
int get_updated_casting_cost(int player, int card, int iid, event_t event, int cless){
	return MAX(0, true_get_updated_casting_cost(player, card, iid, event, cless));
}

int is_phyrexian(int id)
{
  if (id == CARD_ID_ACT_OF_AGGRESSION || id == CARD_ID_APOSTLES_BLESSING ||
      id == CARD_ID_BIRTHING_POD || id == CARD_ID_CATHEDRAL_MEMBRANE ||
      id == CARD_ID_CORROSIVE_GALE || id == CARD_ID_DISMEMBER || id == CARD_ID_GITAXIAN_PROBE ||
      id == CARD_ID_GUT_SHOT || id == CARD_ID_MARROW_SHARDS || id == CARD_ID_MENTAL_MISSTEP ||
      id == CARD_ID_MOLTENSTEEL_DRAGON || id == CARD_ID_MUTAGENIC_GROWTH ||
      id == CARD_ID_NORNS_ANNEX || id == CARD_ID_NOXIOUS_REVIVAL ||
      id == CARD_ID_PHYREXIAN_METAMORPH || id == CARD_ID_PITH_DRILLER ||
      id == CARD_ID_PORCELAIN_LEGIONNAIRE || id == CARD_ID_POSTMORTEM_LUNGE ||
      id == CARD_ID_RAGE_EXTRACTOR || id == CARD_ID_RUTHLESS_INVASION ||
      id == CARD_ID_SLASH_PANTHER || id == CARD_ID_SPINED_THOPTER ||
      id == CARD_ID_SURGICAL_EXTRACTION || id == CARD_ID_TEZZERETS_GAMBIT ||
      id == CARD_ID_THUNDERING_TANADON || id == CARD_ID_VAULT_SKIRGE ||
      id == 0x4ffa /* Native CSV id; absent from the historical card_id.h enum. */
  )
  {
    return 1;
  }
  return 0;
}

int is_id_in_grave(int player, int id){
	const int *grave = get_grave(player);
	int count = count_graveyard(player)-1;
	while( count > -1 ){
			if( cards_data[grave[count]].id == id ){
				return 1;
			}
			count--;
	}
	return 0;
}

int is_id_in_hand(int player, int id){
	int count = 0;
	while( count < active_cards_count[player] ){
			if( in_hand(player, count) && get_id(player, count) == id ){
				return 1;
			}
			count++;
	}
	return 0;
}

#define C(ch) ((ch) == 'B' || (ch) == 'U' || (ch) == 'G' || (ch) == 'R' || (ch) == 'W')
// These are fragile, but better than what we had before.


int is_normal_hybrid(card_ptr_t *cp)
{
  const char *c = cp->mana_cost_text;
  if (!c)
    return 0;

  int len = strlen(c);
  for (int i = 0; i < len - 1; ++i)
    if (C(c[i]) && C(c[i + 1]) && (c[i + 2] == '|' || c[i + 2] == 0))
      return 1;
  return 0;
}

int is_mixed_hybrid(card_ptr_t* cp)
{
  // All existing mixed hybrid/non-hybrid casting costs end with "|xx|x", where x is any character; no other cards do.
  const char* c = cp->mana_cost_text;
  int len;
  return c && (len = strlen(c)) >= 5 && c[len-2] == '|' && c[len-5] == '|';
}
#undef C


#define USE_HOPELESSLY_BROKEN 1

#ifdef USE_HOPELESSLY_BROKEN
int has_mana_to_cast_iid(int player, event_t event, int iid)
{
  /****************************************************************************************************************
   * Any further special cases for specific cards are unacceptable in this function and will be
   * reverted on sight. *
   ****************************************************************************************************************/
  int csvid = cards_data[iid].id;
  if (csvid == CARD_ID_REAPER_KING)
    return can_cast_reaper_king(player, -1, event);

  int split_half = split_cards_id_table(csvid, 1);
  if (split_half != -1)
    return has_mana_to_cast_id(player, event, split_half) ||
           has_mana_to_cast_id(player, event, split_cards_id_table(csvid, 2));

  card_ptr_t *cp = cards_ptr[csvid];
  int orig_cless = cp->req_colorless;
  if (orig_cless == 40)
    orig_cless = 0;

  int cless = get_updated_casting_cost(player, -1, iid, event, orig_cless);
  if (csvid == CARD_ID_KARADOR_GHOST_CHIEFTAIN)
  {
    cless -= count_graveyard_by_type(player, TYPE_CREATURE);
    if (cless < 0)
      cless = 0;
  }
  int is_artifact = cards_data[iid].type & TYPE_ARTIFACT;
  if (is_phyrexian(csvid))
    return has_phyrexian_mana(player, cless, cp->req_black, cp->req_blue, cp->req_green,
                              cp->req_red, cp->req_white, is_artifact);
  else if (is_normal_hybrid(cp))
  {
    int c1 = get_colors_for_hybrid(-1, iid, 0, 0);
    int c2 = get_colors_for_hybrid(-1, iid, c1, 0);
    return has_mana_hybrid(player, get_number_of_hybrid_mana(-1, iid, 0, 0), c1, c2, cless);
  }
  else if (is_monocolor_hybrid(cp))
  {
    int c1 = get_color_for_monocolored_hybrid(-1, iid);
    return has_mana_for_monocolor_hybrid(player, get_number_of_monocolored_hybrid_mana(-1, iid, 0),
                                         c1, 0);
  }
#if 0
  else if (is_mixed_hybrid(cp))
	{
	  // I can't bring myself to care.
	}
#endif
  else if (is_artifact)
    return has_mana_multi_a(player, cless, cp->req_black, cp->req_blue, cp->req_green, cp->req_red,
                            cp->req_white);
  else
    return has_mana_multi(player, cless, cp->req_black, cp->req_blue, cp->req_green, cp->req_red,
                          cp->req_white);
}

int charge_mana_from_id(int player, int card, event_t event, int csvid)
{
  return charge_mana_from_id_with_cost_mod(player, card, event, csvid, 0);
}
#elif defined(USE_EXE_VERSION)
/* These are almost right, but incompatible with the way alternative costs are defined - which is to force mana cost to either 0 or infinite during cost
 * modification, then charge for real during EVENT_CAST_SPELL.  Which is unqueryable and unsustainable.  Not to mention that they then try to call *this* if you
 * choose to decline the alternative costs.
 *
 * The right way is to use cost modification only for *cost modification*, and then have something else that's called for alternative costs.
 *
 * Besides which, at least half of the calls to this are followed by play_spell_for_free().  Such calls should just put the card on the stack normally and
 * charge normally. */
int has_mana_to_cast_iid(int player, event_t event, int iid)
{
  int added = add_card_to_hand(player, iid);
  card_instance_t* inst = get_card_instance(player, added);
  inst->state |= STATE_INVISIBLE;
  --hand_count[player];
  int rval = EXE_FN(int, 0x402930, int, int, int)(player, 0, added);
  obliterate_card_and_recycle(player, added);
  return rval;
}

//global _charge_mana_to_cast
//_charge_mana_to_cast:
//	push	ebp
//	mov	ebp, esp
//	push	edi
//	push	dword [ebp+0x10]	; card - param3
//	push	dword [ebp+0x0c]	; player - param2
//	mov	edi, [ebp+0x08]		; card_data_t* - param1
//	call	0x402680
//	add	esp, 2*4		; pop 2 params
//	pop	edi
//	leave
//	ret

int charge_mana_from_id(int player, int card, event_t event, int csvid)
{
  int iid = get_internal_card_id_from_csv_id(csvid);
  int added = add_card_to_hand(player, iid);
  card_instance_t* inst = get_card_instance(player, added);
  inst->state |= STATE_INVISIBLE;
  --hand_count[player];
  EXE_FN(int, 0x402930, int, int, int)(player, 0, added);
  int rval = charge_mana_multi(player, COST_COLORLESS, COST_BLACK, COST_BLUE, COST_GREEN, COST_RED, COST_WHITE);
  obliterate_card(player, added);
  return rval;
}
#endif

int has_mana_to_cast_id(int player, event_t event, int csvid){
	return has_mana_to_cast_iid(player, event, get_internal_card_id_from_csv_id(csvid));
}

void remove_id_from_grave(int player, int csvid, remove_id_from_grave_t mode){
	const int *grave = get_grave(player);
	int count = count_graveyard(player)-1;
	while( count > -1 ){
			if( cards_data[grave[count]].id == csvid ){
				if( mode & RIFG_OBLITERATE ){
					remove_card_from_grave(player, count);
				}
				else{
					rfg_card_from_grave(player, count);
				}
				if( !(mode & RIFG_REMOVE_ALL) ){
					break;
				}
			}
			count--;
	}
}

int already_dead(int player, int card){
	if( check_special_flags(player, card, SF_LETHAL_DAMAGE_DESTROY) ||
		check_special_flags(player, card, SF_LETHAL_DAMAGE_BURY) ||
		check_special_flags(player, card, SF_LETHAL_DAMAGE_EXILE) ||
		get_card_instance(player, card)->damage_on_card >= get_toughness(player, card)
	  ){
		return 1;
	}
	return 0;
}



/* Returns the amount of life gained, which will always be at least 1.  EA_LICH must be set in event_flags for the trigger to be dispatched (so a card with
 * Flags: Lich set in ct_all.csv must have been on the battlefield this turn, or set it manually each turn during EVENT_CAN_SKIP_TURN). */


void exchange_life_totals(int player, int t_player)
{
  if (player == t_player)
  {
    return;
  }
  if (life[player] == life[t_player])
  {
    return;
  }

  if (!life_total_can_change(player) || !life_total_can_change(1 - player))
  {
    return;
  }

  if (life[player] > life[1 - player] && (player_bits[1 - player] & PB_CANT_GAIN_LIFE))
  {
    return;
  }
  if (life[1 - player] > life[player] && (player_bits[player] & PB_CANT_GAIN_LIFE))
  {
    return;
  }

  int life_totals[2];
  APNAP(p, {
    life_totals[p] = life[p];
    if (check_battlefield_for_id(p, CARD_ID_LICH))
    {
      life_totals[p] = 0;
    };
  };);
  APNAP(p, {
    if (life_totals[p] > life_totals[1 - p])
    {
      lose_life(p, life_totals[p] - life_totals[1 - p]);
    }
    else if (life_totals[p] < life_totals[1 - p])
    {
      gain_life(p, life_totals[1 - p] - life_totals[p]);
    };
  };);
}

void set_life_total(int player, int amount)
{
  if (life[player] == amount)
  {
    return;
  }

  if (!life_total_can_change(player))
  {
    return;
  }

  if (life[player] < amount && (player_bits[player] & PB_CANT_GAIN_LIFE))
  {
    return;
  }

  if (life[player] < amount)
  {
    int lg = amount - life[player];
    gain_life(player, lg);
  }
  else if (life[player] > amount)
  {
    int ll = life[player] - amount;
    lose_life(player, ll);
  }
}





void kill_my_legacy(int player, int card){
	int id = get_id(player, card);
	int i;
	for(i=0; i<2; i++){
		int count = active_cards_count[i]-1;
		while( count > -1 ){
				if( is_what(i, count, TYPE_EFFECT) ){
					card_instance_t *this = get_card_instance( i, count );
					if( this->targets[2].card == id ){
						kill_card(i, count, KILL_EXILE);
					}
				}
				count--;
		}
	}
}



int until_eot_legacy(int player, int card, event_t event){

	if( eot_trigger(player, card, event) ){
		kill_card(player, card, KILL_EXILE);
	}

	return 0;
}

int can_block_me(int blocked_player, int blocked_card, int blocking_player, int blocking_card){
	return is_legal_block(blocking_player, blocking_card, blocked_player, blocked_card);
}

// Makes player/card block player_to_block/card_to_block.  Doesn't check legality.
void block(int player, int card, int player_to_block, int card_to_block)
{
  // This is inlined in generate_token(), since some of it happens before the token's put into play and some of it after.
  // Also inlined in human_assign_blockers, where STATE_UNKNOWN8000 isn't set.

  if (player == player_to_block)
	return;

  card_instance_t* to_block = get_card_instance(player_to_block, card_to_block);
  card_instance_t* blocker = get_card_instance(player, card);

  blocker->blocking = to_block->blocking == 255 ? card_to_block : to_block->blocking;
  blocker->state |= STATE_UNKNOWN8000|STATE_BLOCKING;

  play_sound_effect(WAV_BLOCK2);

  if (event_flags & EA_SELECT_BLOCK)
	dispatch_trigger2(current_turn, TRIGGER_BLOCKER_CHOSEN, EXE_STR(0x790074)/*PROMPT_BLOCKERSELECTION[0]*/, 0, player, card);
}

int player_flips_a_coin(int player, int card, int who_flips)
{
  int result = 0;
  int choice = (get_id(player, card) == CARD_ID_MANA_CLASH ||
                get_id(player, card) == CARD_ID_RAKDOS_THE_SHOWSTOPPER)
                   ? FC_TAILS
                   : FC_HEAD;
  if (get_id(player, card) != CARD_ID_RAL_ZAREK && get_id(player, card) != CARD_ID_MANA_CLASH &&
      get_id(player, card) != CARD_ID_PANDORAS_BOX &&
      get_id(player, card) != CARD_ID_RAKDOS_THE_SHOWSTOPPER)
  {
    choice = do_dialog(who_flips, player, card, -1, -1, " Heads\n Tails",
                       internal_rand(100) > 49 ? FC_TAILS : FC_HEAD);
  }
  int rounds = 2 * count_cards_by_id(player, CARD_ID_KRARKS_THUMB);
  if (!rounds)
  {
    rounds = 1;
  }
  char buffer[500];
  int count = 0;
  int cf[rounds];
  int pos = 0;
  while (count < rounds)
  {
    cf[count] = coin_flip(who_flips, "", 1);
    if (rounds > 1)
    {
      pos += scnprintf(buffer + pos, 500 - pos, " Choose");
      if (cf[count] == FC_HEAD)
      {
        pos += scnprintf(buffer + pos, 500 - pos, " Heads\n");
      }
      else
      {
        pos += scnprintf(buffer + pos, 500 - pos, " Tails\n");
      }
    }
    count++;
  }
  if (rounds == 1)
  {
    result = choice == cf[0];
  }
  else
  {
    int ai_choice = 0;
    count = 0;
    while (count < rounds)
    {
      if (cf[count] == choice)
      {
        ai_choice = count;
        break;
      }
      count++;
    }
    int choice2 = do_dialog(who_flips, player, card, -1, -1, buffer, ai_choice);
    if (choice == cf[choice2])
    {
      result = 1;
    }
  }
  if (get_id(player, card) != CARD_ID_RAL_ZAREK && get_id(player, card) != CARD_ID_MANA_CLASH &&
      get_id(player, card) != CARD_ID_PANDORAS_BOX &&
      get_id(player, card) != CARD_ID_RAKDOS_THE_SHOWSTOPPER)
    dispatch_event(who_flips, result, EVENT_COIN_FLIP_WON_OR_LOST);
  return result;
}

int flip_a_coin(int player, int card){
	return player_flips_a_coin(player, card, player);
}

/* Can be used with sorceries and with any target restrictions, but the AI can't fully understand the power/toughness modifications.
 * td only needs to be set if IS_CASTING(player, card, event). */


/* A more restricted version of vanilla_pump(), usable only for instants and only when there are no restrictions on targeting except for whose creature it can
 * target; but this lets us teach the AI about the power/toughness modifications. */


void shuffle_for_exe(int whose_computer_shuffles, int whose_library)
{
  if (ai_is_speculating != 1)
	{
	  play_sound_effect(WAV_SHUFFLE);
	  EXE_FN(void, 0x438940, int)(whose_library);	// shuffle_animation()
	}

  if ((trace_mode & 2)
	  && whose_computer_shuffles != 0)
	{
	  EXE_FN(void, 0x479cc0, int)(whose_library);	// receive_deck_order_from_network()
	  return;
	}

  // Shuffle ourselves, since the function doing the actual reorder in Magic.exe has a nonstandard calling convention

  int i, sz = 0;
  // First, ensure there are no "gaps" of an invalid iid of -1.
  int scratch[500];
  for (i = 0; i < 500; ++i)
	if (deck_ptr[whose_library][i] != -1)
	  scratch[sz++] = deck_ptr[whose_library][i];
  // Initialize the remaining elements to -1
  for (i = sz; i < 500; ++i)
	scratch[i] = -1;

  // Fisher-Yates shuffle
  for (i = sz - 1; i >= 1; --i)
	{
	  int j = internal_rand(i + 1);	// This introduces bias, since internal_rand() is a cruddy prng that restricts its range using mod, but no worse than what was there before.
	  if (i != j)
		SWAP(scratch[i], scratch[j]);
	}

  // Put it back.
  memcpy(deck_ptr[whose_library], scratch, sizeof(scratch));

  // And tell the other computer.
  if (trace_mode & 2)
	EXE_FN(void, 0x479c60, int)(whose_library);	// send_deck_order_over_network()
}

void shuffle(int player)
{
  int iVar1;
  card_instance_t *instance;
  int iVar3;
  int card;
  char *pcVar4;
  char *pcVar5;
  unsigned char bVar6;
  int iStack_150;
  char msg[100];
  test_definition_t this_test;

  bVar6 = 0;
  iVar1 = count_graveyard_by_type(player, 2);
  shuffle_for_exe(player, player);
  iStack_150 = 0;
  do
  {
    card = active_cards_count[iStack_150] + -1;
    if (-1 < active_cards_count[iStack_150] + -1)
    {
      do
      {
        while ((instance = in_play(iStack_150, card),
                instance == NULL || (iVar3 = is_what(iStack_150, card, 0x47), iVar3 == 0)))
        {
        LAB_0269a9a0:
          card = card + -1;
          if (card == -1)
            goto LAB_0269aab8;
        }
        iVar3 = get_id(iStack_150, card);
        if (iVar3 == 0x1a6b)
        {
          damage_player(player, 2, iStack_150, card);
        }
        iVar3 = get_id(iStack_150, card);
        if ((iVar3 == 0x3555) && (0 < hand_count[player]))
        {
          pcVar4 = "Select a card to put on top.";
          pcVar5 = msg;
          for (iVar3 = 7; iVar3 != 0; iVar3 = iVar3 + -1)
          {
            *(uint32_t *)pcVar5 = *(uint32_t *)pcVar4;
            pcVar4 = pcVar4 + (unsigned int)bVar6 * -8 + 4;
            pcVar5 = pcVar5 + (unsigned int)bVar6 * -8 + 4;
          }
          msg[0x1d] = '\0';
          *pcVar5 = *pcVar4;
          pcVar4 = msg + 0x20;
          for (iVar3 = 0x11; iVar3 != 0; iVar3 = iVar3 + -1)
          {
            pcVar4[0] = '\0';
            pcVar4[1] = '\0';
            pcVar4[2] = '\0';
            pcVar4[3] = '\0';
            pcVar4 = pcVar4 + (unsigned int)bVar6 * -8 + 4;
          }
          msg[0x1e] = '\0';
          msg[0x1f] = '\0';
          new_default_test_definition(&this_test, 0xff, msg);
          iVar3 = new_select_a_card(player, player, 4, 1, 3, -1, &this_test);
          put_on_top_of_deck(player, iVar3);
        }
        if (((player == iStack_150) || (iVar3 = get_id(iStack_150, card), iVar3 != 0x291c)) ||
            ((iVar3 = duh_mode(iStack_150),
              iVar3 == 0 && (iVar3 = do_dialog(iStack_150, iStack_150, card, -1, -1,
                                               " Add +1/+1 counter\n Pass", 0),
                             iVar3 != 0))))
          goto LAB_0269a9a0;
        iVar3 = card + -1;
        add_counters(iStack_150, card, iStack_150, card, COUNTER_P1_P1, 1);
        card = iVar3;
      } while (iVar3 != -1);
    }
  LAB_0269aab8:
    iStack_150 = iStack_150 + 1;
    if (iStack_150 == 2)
    {
      if (iVar1 != 0)
      {
        ereboss_titan_effect(1 - player);
        return;
      }
      return;
    }
  } while (true);
}













int end_of_combat_trigger(int player, int card, event_t event, int trigger_mode)
{

  if (trigger_condition == TRIGGER_END_COMBAT && affect_me(player, card) &&
      reason_for_trigger_controller == player && !is_humiliated(player, card, AL_TRIG))
  {
    if (event == EVENT_TRIGGER)
    {
      event_result |= trigger_mode;
    }
    else if (event == EVENT_RESOLVE_TRIGGER)
    {
      return 1;
    }
  }
  return 0;
}

int discard_trigger(int player, int card, event_t event, int t_player, int trigger_mode,
                    int discard_trigger_mode)
{

  if (trigger_condition == TRIGGER_DISCARD && affect_me(player, card) &&
      reason_for_trigger_controller == player && !is_humiliated(player, card, AL_TRIG))
  {
    if (t_player != 2 && t_player != trigger_cause_controller)
    {
      return 0;
    }
    if ((discard_trigger_mode & DISCARD_NO_NORMAL) && current_phase > PHASE_MAIN2)
    {
      return 0;
    }
    if ((discard_trigger_mode & DISCARD_STILL_IN_HAND) &&
        !in_hand(trigger_cause_controller, trigger_cause))
    {
      return 0;
    }

    if (event == EVENT_TRIGGER)
    {
      if (trigger_mode == RESOLVE_TRIGGER_DUH)
      {
        trigger_mode = duh_mode(player) ? RESOLVE_TRIGGER_MANDATORY : RESOLVE_TRIGGER_OPTIONAL;
      }
      event_result |= trigger_mode;
    }
    else if (event == EVENT_RESOLVE_TRIGGER)
    {
      return 1;
    }
  }

  return 0;
}

int lifegaining_charm(int player, int card, event_t event, int affected_player, int clr, int trigger_mode, int cless){

	if( specific_spell_played(player, card, event, affected_player, trigger_mode, TYPE_ANY, 0, 0, 0, clr, 0, 0, 0, -1, 0) ){
		charge_mana(player, COLOR_COLORLESS, cless);
		if( spell_fizzled != 1 ){
			gain_life(player, 1);
		}
	}

	return 0;
}

int cip_lifegain(int player, int card, event_t event, int amount){

	etb_ability(player, card, event, RESOLVE_TRIGGER_MANDATORY);

	if( event == EVENT_ETB_ABILITY ){
		gain_life(player, amount);
	}

	return 0;
}







int damage_redirection(int player, int card, event_t event){

	card_instance_t *instance = get_card_instance(player, card);

	// The damage directed to this target...
	int s_player = instance->targets[0].player;
	int s_card = instance->targets[0].card;

	// ...Will be redirected to this target
	int t_player = instance->targets[1].player;
	int t_card = instance->targets[1].card;

	// If "instance->targets[2].player" is set to "1", the effect end at EOT


	if( event == EVENT_PREVENT_DAMAGE && (current_phase == PHASE_FIRST_STRIKE_DAMAGE ||
		current_phase == PHASE_NORMAL_COMBAT_DAMAGE)
	  ){
		card_instance_t *damage = get_card_instance(affected_card_controller, affected_card);
		if( damage->internal_card_id == damage_card && damage->damage_target_player == s_player &&
			damage->damage_target_card == s_card && damage->info_slot > 0
		  ){
			int good = 1;
			if( t_card != -1 && ! in_play(t_player, t_card) ){
				good = 0;
			}
			if( good == 1 ){
				damage->damage_target_player = t_player;
				damage->damage_target_card = t_card;
			}
		}
	}

	if( eot_trigger(player, card, event) && instance->targets[2].player == 1 ){
		kill_card(player, card, KILL_EXILE);
	}
	return 0;
}

int generic_x_spell(int player, int card, event_t event, int zone, int mana_color, int effect){
	// Bitfields for effect
	// 1 --> Damage target
	// 8 --> Gain life if target for other effects is valid (Consume Spirit)
	// 8192 --> Pump target creature -X/-X
	// 16384 --> Mill target player

	// Use mana_color = COLOR_COLORLESS if X can be paid with any kind of mana

	/* This is currently used only by card_blaze() and card_swallowing_plague(), and nothing is assigned to
	 * card_swallowing_plague(). */

	target_definition_t td;
	default_target_definition(player, card, &td, TYPE_CREATURE|TARGET_TYPE_PLANESWALKER);
	td.zone = zone;

	card_instance_t *instance = get_card_instance(player, card);

	if( event == EVENT_CAN_CAST ){
		if( can_target(&td) ){
			return ! check_battlefield_for_id(2, CARD_ID_GADDOCK_TEEG);
		}
	}

	if( event == EVENT_CAST_SPELL && affect_me(player, card) ){

		if (!is_token(player, card)){
			if( mana_color > 0 ){
				charge_mana(player, mana_color, -1);
				if( spell_fizzled != 1 ){
					instance->info_slot = x_value;
				}
			}
			else{
				instance->info_slot = x_value;
			}
		}

		if( spell_fizzled != 1 ){
			if( zone == TARGET_ZONE_IN_PLAY ){
				pick_target(&td, "TARGET_CREATURE");	// nope; currently means "target creature or planeswalker"
			}
			if( zone == TARGET_ZONE_PLAYERS ){
				pick_target(&td, "TARGET_PLAYER");
			}
			if( zone == TARGET_ZONE_CREATURE_OR_PLAYER ){
				pick_target(&td, "TARGET_ANY");
			}
			if( get_id(player, card) == CARD_ID_FANNING_THE_FLAMES && has_mana(player, COLOR_COLORLESS, 3) ){//unused
				int choice = do_dialog(player, player, card, player, card, " Buyback\n Pass", 0);
				if( choice == 0 ){
					charge_mana(player, COLOR_COLORLESS, 3);
					if( spell_fizzled != 1 ){
						instance->targets[8].card = get_id(player, card);;
					}
				}
			}
		}

		set_special_flags2(player, card, SF2_X_SPELL);
	}

	else if(event == EVENT_RESOLVE_SPELL ){
			int amt = instance->info_slot;

			if( valid_target(&td)  ){
				if( effect & 1 ){	// used by: Blaze/Volcanic Geyser, Swallowing Plague
					damage_creature_or_player(player, card, event, amt);
				}
				if( effect & 8 ){	// used by: Swallowing Plague
					gain_life(player, amt);
				}
				if( effect & 8192 ){	// used by: nothing
					pump_until_eot(player, card, instance->targets[0].player, instance->targets[0].card, -amt, -amt);
				}
				if( effect & 16384 ){	// used by: nothing
					mill(instance->targets[0].player, amt);
				}
			}

			if( instance->targets[8].card == CARD_ID_FANNING_THE_FLAMES ){	// unused
				bounce_permanent(player, card);
			}
			else{
				kill_card(player, card, KILL_DESTROY);
			}
	}
	return 0;
}

int steal_permanent_from_target_opponent_deck(int player, int card, event_t event, test_definition_t *this_test){

	target_definition_t td;
	default_target_definition(player, card, &td, TYPE_CREATURE );
	td.zone = TARGET_ZONE_PLAYERS;

	card_instance_t *instance = get_card_instance(player, card);

	if( event == EVENT_RESOLVE_SPELL){
		if( valid_target(&td) ){
			new_global_tutor(player, instance->targets[0].player, TUTOR_FROM_DECK, TUTOR_PLAY, 0, AI_MAX_CMC, this_test);
		}
		kill_card(player, card, KILL_DESTROY);
	}

	return generic_spell(player, card, event, GS_CAN_ONLY_TARGET_OPPONENT, &td, NULL, 1, NULL);
}

static int timewalk_legacy(int player, int card, event_t event){

	card_instance_t *inst = get_card_instance( player, card );

	if (inst->token_status & STATUS_TIMEWALK)
    {
		if (event == EVENT_CAN_SKIP_TURN && time_walk_flag == -1)
			time_walk_flag = player;

/* This gets checked in the wrong order - EVENT_CLEANUP is set in order by card index, not timestamp,
	and the timewalk effect with the *newest* timestamp needs to be the one that takes precedence
*/
		if (event == EVENT_CLEANUP && !dword_4EF1B8)
        {
			dword_4EF1B8 |= 1 << player;
			inst->token_status &= ~STATUS_PERMANENT;

        }
    }

    if( !(inst->token_status & STATUS_PERMANENT) && (event == EVENT_CLEANUP || event == EVENT_SHOULD_AI_PLAY) ){
		kill_card(player, card, KILL_EXILE);
    }

	return 0;
}

static int create_time_walk_legacy(int player, int card){
	if (time_walk_flag == -1){
		int p, c;
		for (p = 0; p < 2; ++p){
			for (c = 0; c < active_cards_count[p]; ++c){
				card_instance_t* instance = get_card_instance(p, c);
				if (instance->info_slot == (int)timewalk_legacy && (instance->token_status & STATUS_TIMEWALK)){
					goto found;
				}
			}
		}
		// not found:
		time_walk_flag = player;
	}
 found:;
	int legacy_card = create_legacy_effect(player, card, &timewalk_legacy);
	card_instance_t* legacy_inst = get_card_instance(player, legacy_card);
	legacy_inst->token_status |= STATUS_TIMEWALK | STATUS_PERMANENT;
	ai_modifier += (player == AI ? 96 : -96);

	return legacy_card;
}
/* For reference:
int card_time_walk(int player, int card, event_t event){
	if (event == EVENT_CAN_CAST){
		return 1;
	} else if (event == EVENT_CAST_SPELL){
		if (affect_me(player, card)){
			ai_modifier += 96;
		}
	} else if (event == EVENT_RESOLVE_SPELL){
		create_time_walk_legacy(player, card);
		kill_card(player, card, KILL_BURY);
	}
	return 0;
}
*/

static int check_for_extra_turn_suppression(int player){
	int i;
	for(i=0; i<2; i++){
		int c;
		for(c=0; c<active_cards_count[i]; c++){
			if( in_play(i, c) && ! is_humiliated(i, c, AL_STATIC) && get_card_instance(i, c)->kill_code == 0){
				int id = get_id(i, c);
				if( id == CARD_ID_UGINS_NEXUS ){
					return 1;
				}
				if( id == CARD_ID_STRANGLEHOLD && i != player ){
					return 1;
				}
			}
		}
	}
	return 0;
}

int time_walk_effect(int player, int card)
{
  if (check_for_extra_turn_suppression(player))
    return -1;
  return create_time_walk_legacy(player, card);
}

static int skip_next_turn_legacy(int player, int card, event_t event)
{
  /* Targets:
   * [0].player: player whose turn is to be skipped */
  if (event == EVENT_CAN_SKIP_TURN)
	{
	  card_instance_t* instance = get_card_instance(player, card);
	  if (current_turn == instance->targets[0].player && !(land_can_be_played & LCBP_SKIP_TURN))
		{
		  land_can_be_played |= LCBP_SKIP_TURN;
		  kill_card(player, card, KILL_EXILE);
		}
	}

  return 0;
}

int skip_next_turn(int player, int card, int t_player){
	int legacy = create_legacy_effect(player, card, &skip_next_turn_legacy);
	card_instance_t *instance = get_card_instance(player, legacy);
	instance->targets[0].player = t_player;
	return legacy;
}

// Returns true if the turn about to be started will be skipped instead.  Accurate only during EVENT_CAN_SKIP_TURN (the first event of each turn).
int this_turn_is_being_skipped(void)
{
  if (land_can_be_played & LCBP_SKIP_TURN)	// A skip-turn effect has already resolved.
	return 1;

  /* Look for a skip_next_turn_legacy targeting the current player.  The point of this is to find a skip-your-next-turn effect that will resolve this turn, but
   * hasn't yet (perhaps because you have a tapped Time Vault that gets checked before the effect card).  No need to check for other legacies if this is itself
   * being called from a legacy, though; we can just make the first one seen trigger.  Hence it just checks for LCBP_SKIP_TURN directly. */
  int p, c;
  for (p = 0; p <= 1; ++p)
	for (c = 0; c < active_cards_count[p]; ++c)
	  if (in_play(p, c))
		{
		  card_instance_t* inst = get_card_instance(p, c);
		  if (inst->internal_card_id == LEGACY_EFFECT_CUSTOM && inst->info_slot == (int)skip_next_turn_legacy && inst->targets[0].player == current_turn)
			return 1;
		}

  return 0;
}

enum
	{
		CPL_PAY_LIFE_FOR_SPELLS_OR_ACTIVATED_ABILITY = 1<<0,
	};

int can_pay_life_impl(int player, int amount, int flags)
{
  int result;
  result = life_total_can_change(player);

  if ((result != 0) &&
      (((flags & 1U) == 0 ||
        ((result = check_battlefield_for_id(2, 0x3010),
          result == 0 && (result = check_battlefield_for_id(2, 0x5626), result == 0))))))
  {
    return (unsigned int)(amount <= life[player]);
  }
  return 0;
}

int can_pay_life_as_cost_for_spells_or_activated_abilities(int player, int amount){
	return can_pay_life_impl(player, amount, CPL_PAY_LIFE_FOR_SPELLS_OR_ACTIVATED_ABILITY);
}

int can_pay_life(int player, int amount){
	return can_pay_life_impl(player, amount, 0);
}







// Stores an internal count of attackers in (player,card)->targets[1].player; returns that amount.


// As declare_attackers_trigger, but attacking cards must also pass test.


int pick_target_nonbasic_land(int player, int card, int cannot_cancel)
{
  target_definition_t td;
  default_target_definition(player, card, &td, TYPE_LAND);
  td.required_subtype = SUBTYPE_NONBASIC;
  td.allow_cancel = cannot_cancel ? 0 : 1;

  card_instance_t *instance = get_card_instance(player, card);

  instance->number_of_targets = 0;

  if (can_target(&td) && pick_target(&td, "TARGET_NONBASIC_LAND"))
	{
	  if (instance->targets[0].player == player)
		ai_modifier -= 128;

	  return 1;
	}

  cancel = 1;
  return 0;
}

extern int hack_xx;
// A generalization of charge_mana_for_double_x().  num_x==2 => charge XX.  num_x==3 => charge XXX.  etc.  Returns total mana spent, not what X is.


// Charges XX of color.  Returns total mana spent, not what X is.


void check_damage_test(int source_of_damage_player, int source_of_damage_card, event_t event, ddbm_flags_t mode, 
						int triggering_player, int triggering_card, int type /*optional*/,test_definition_t *test/*optional*/)
{
	if (event == EVENT_DEAL_DAMAGE){
		card_instance_t* instance = get_card_instance(triggering_player, triggering_card);
		card_instance_t* damage = get_card_instance(affected_card_controller, affected_card);
		if (damage->internal_card_id == damage_card && damage->info_slot > 0 ){
			if( source_of_damage_player != ANYBODY && damage->damage_source_player != source_of_damage_player ){
				return;
			}
			if( source_of_damage_card != -1 && damage->damage_source_card != source_of_damage_card ){
				return;
			}
			if( test != NULL ){
				if( in_play(damage->damage_source_player, damage->damage_source_card) ){
					if( ! new_make_test_in_play(damage->damage_source_player, damage->damage_source_card, -1, test) ){
						return;
					}
				}
				else{
					int iid = get_internal_card_id_from_csv_id(damage->display_pic_csv_id);
					if( ! new_make_test(source_of_damage_player, iid, -1, test) ){
						return;
					}
				}
			}
			if( type ){
				int dmg_type = damage->targets[3].player;
				if( in_play(damage->damage_source_player, damage->damage_source_card) ){
					dmg_type = get_type(damage->damage_source_player, damage->damage_source_card);
				}
				if( ! is_what(-1, dmg_type, type) ){
					return;
				}
			}

			if( damage_is_to_planeswalker(damage) ){
				if (!(mode & DDBM_MAY_DAMAGE_PLANESWALKER)){
					return;
				}
			}

			if (mode & (DDBM_MUST_DAMAGE_PLAYER | DDBM_MUST_DAMAGE_CREATURE | DDBM_MUST_DAMAGE_TRIGGERING_PLAYER | 
						DDBM_MUST_DAMAGE_OPPONENT) )
			{
				if( damage->damage_target_card == -1 ){
					if( mode & DDBM_MUST_DAMAGE_CREATURE ){
						return;
					}
					else{
						if( !(mode & (DDBM_MUST_DAMAGE_PLAYER | DDBM_MUST_DAMAGE_TRIGGERING_PLAYER | DDBM_MUST_DAMAGE_OPPONENT)) ){
							return;
						}
						if( (mode & DDBM_MUST_DAMAGE_TRIGGERING_PLAYER) && damage->damage_target_player != triggering_player ){
							return;
						}
						if( (mode & DDBM_MUST_DAMAGE_OPPONENT) && damage->damage_target_player != 1-triggering_player ){
							return;
						}
					}
				}
				else{
					if( !(mode & DDBM_MUST_DAMAGE_CREATURE) ){
						return;
					}
				}
			}

			if ((mode & DDBM_MUST_BE_COMBAT_DAMAGE) && !(damage->token_status & (STATUS_COMBAT_DAMAGE|STATUS_FIRST_STRIKE_DAMAGE))){
				return;
			}

			if (is_humiliated(triggering_player, triggering_card, AL_TRIG)){
				return;
			}

			if( mode & DDBM_TRACE_DAMAGING_SOURCES ){
				if( instance->info_slot < 0 ){
					instance->info_slot = 0;
				}
				int pos = instance->info_slot;
				if( pos < 10 ){
					instance->targets[pos].player = damage->damage_source_player;
					instance->targets[pos].card = damage->damage_source_card;
					instance->info_slot++;
				}
			}
			else{
				int count_pos = mode & DDBM_STORE_IN_TARGETS_9 ? 9 : 8;
				if (instance->targets[count_pos].player < 0){
					instance->targets[count_pos].player = 0;
				}
				++instance->targets[count_pos].player;
				if (mode & DDBM_TRACE_DAMAGED_PLAYERS){
					if(instance->targets[1].player == -1){
						instance->targets[1].player = 0;
					}
					if (damage->damage_target_player == 1){
						++SET_BYTE1(instance->targets[1].player);
					} else {
						++SET_BYTE0(instance->targets[1].player);
					}
				}

				if (mode & DDBM_REPORT_DAMAGE_DEALT){
					if (instance->targets[16].player < 0){
						instance->targets[16].player = 0;
					}
					if (mode & DDBM_TRACE_DAMAGED_PLAYERS){
						if (damage->damage_target_player == 1){
							SET_BYTE1(instance->targets[16].player) += damage->info_slot;
						} 
						else{
							SET_BYTE0(instance->targets[16].player) += damage->info_slot;
						}
					}
					else{
						instance->targets[16].player += damage->info_slot;
					}
				}
			}
		}
	}
}

void check_damage(int source_of_damage_player, int source_of_damage_card, event_t event, ddbm_flags_t mode, int triggering_player, int triggering_card){
	check_damage_test(source_of_damage_player, source_of_damage_card, event, mode, triggering_player, triggering_card, -1, NULL);
}

int resolve_damage_trigger(int player, int card, event_t event, ddbm_flags_t mode,
                           int triggering_player, int triggering_card)
{
  // DDBM_TRACE_DAMAGING_SOURCES must be used here too if it's used in "check_damage_test" or this
  // either won't work or will glitch
  if (trigger_condition == TRIGGER_DEAL_DAMAGE && affect_me(triggering_player, triggering_card) &&
      reason_for_trigger_controller == triggering_player)
  {

    card_instance_t *instance = get_card_instance(triggering_player, triggering_card);
    int count_pos = mode & DDBM_STORE_IN_TARGETS_9 ? 9 : 8;
    if (mode & DDBM_TRACE_DAMAGING_SOURCES)
    {
      if (instance->info_slot <= 0)
      {
        return 0;
      }
    }
    else
    {
      if (instance->targets[count_pos].player <= 0)
      {
        return 0;
      }
    }

    if (event == EVENT_TRIGGER)
    {
      if (mode & DDBM_TRIGGER_OPTIONAL)
      {
        event_result |= RESOLVE_TRIGGER_AI(triggering_player);
      }
      else
      {
        event_result |= RESOLVE_TRIGGER_MANDATORY;
      }
    }
    else if (event == EVENT_RESOLVE_TRIGGER)
    {
      int packets_dealt = instance->targets[count_pos].player;
      if (mode & DDBM_TRACE_DAMAGING_SOURCES)
      {
        packets_dealt = instance->info_slot;
        instance->info_slot = 0;
      }
      else
      {
        instance->targets[count_pos].player = 0;
      }
      if (!(mode & DDBM_REPORT_DAMAGE_DEALT))
      {
        instance->targets[16].player = 0;
      }
      card_instance_t *source = get_card_instance(player, card);
      if ((int32_t)source->token_status < 0)
      {
        destroy_attached_auras_and_obliterate_card(player, card);
        obliterate_card(player, card);
      }
      return packets_dealt;
    }
    else if (event == EVENT_END_TRIGGER)
    {
      instance->targets[16].player = 0;
      if (mode & DDBM_TRACE_DAMAGING_SOURCES)
      {
        instance->info_slot = 0;
      }
      else
      {
        instance->targets[count_pos].player = 0;
      }
      if (mode & DDBM_TRACE_DAMAGED_PLAYERS)
      {
        instance->targets[1].player = 0;
      }
    }
  }

  return 0;
}

/* Just like damage_dealt_by_me(), except can specify a different triggering_player and triggering_card that highlights for the trigger and which gets its
 * targets[1,8,16].player overwritten instead of player/card. */
int damage_dealt_by_me_arbitrary(int player, int card, event_t event, ddbm_flags_t mode,
                                 int triggering_player, int triggering_card)
{
  if ((mode & DDBM_NOT_ME) || (unsigned int)player >= 3 || (unsigned int)card >= 151)
    return 0;
  ASSERT(!(mode & DDBM_TRACE_DAMAGED_CREATURES) &&
         "unimplemented for damage_dealt_by_me_arbitrary");
  if (event == EVENT_DEAL_DAMAGE)
    check_damage_test(player, card, event, mode, triggering_player, triggering_card, -1, NULL);
  else if (event == EVENT_GRAVEYARD_FROM_PLAY)
  {
    if (affect_me(player, card) && check_battlefield_for_id(ANYBODY, cards_data[damage_card].id) &&
        (int32_t)get_card_instance(player, card)->token_status >= 0)
    {
      card_instance_t *instance = get_card_instance(player, card);
      int dead = add_card_to_hand(player, instance->backup_internal_card_id);
      --hand_count[player];
      card_instance_t *dead_inst = get_card_instance(player, dead);
      dead_inst->kill_code = 0;
      dead_inst->damage_on_card = 0;
      dead_inst->token_status = instance->token_status | 0x80000000U;
      dead_inst->damage_target_player = instance->damage_target_player;
      dead_inst->damage_target_card = instance->damage_target_card;
      memcpy(dead_inst->targets, instance->targets, 152);
      dead_inst->info_slot = get_card_instance(player, card)->info_slot;
      dead_inst->state |= STATE_IN_PLAY;
      for (int p = 0; p < 2; ++p)
        for (int c = 0; c < active_cards_count[p]; ++c)
        {
          card_instance_t *current = get_card_instance(p, c);
          if (current->internal_card_id == damage_card && current->damage_source_player == player &&
              current->damage_source_card == card)
            current->damage_source_card = dead;
          current = get_card_instance(p, c);
          if (current->internal_card_id != damage_card && current->damage_target_player == player &&
              current->damage_target_card == card)
          {
            if (has_subtype(p, c, SUBTYPE_AURA))
              attach_aura_to_target(p, c, EVENT_GRAVEYARD_FROM_PLAY, player, dead);
            else if (is_equipment(p, c))
            {
              // Native inline attachment path also writes targets[8]; it does
              // not dispatch the normal equip event or play the equip sound.
              unattach(p, c);
              card_instance_t *eq = get_card_instance(p, c);
              eq->damage_target_player = player;
              eq->damage_target_card = dead;
              eq->targets[8].player = player;
              eq->targets[8].card = dead;
              int leg = create_targetted_legacy_effect(p, c, equipment_legacy, player, dead);
              get_card_instance(p, leg)->token_status |= STATUS_INVISIBLE_FX;
              if (get_id(p, c) == CARD_ID_NIM_DEATHMANTLE)
                force_a_subtype(player, dead, SUBTYPE_ZOMBIE);
              if (get_id(p, c) == CARD_ID_ASSAULT_SUIT)
                set_special_flags2(player, dead, 0x400000);
            }
            else
              get_card_instance(p, c)->damage_target_card = dead;
          }
        }
    }
  }
  else if (event == EVENT_CHANGE_TYPE &&
           !check_battlefield_for_id(ANYBODY, cards_data[damage_card].id) &&
           (int32_t)get_card_instance(player, card)->token_status < 0)
  {
    destroy_attached_auras_and_obliterate_card(player, card);
    obliterate_card(player, card);
  }
  return resolve_damage_trigger(player, card, event, mode, triggering_player, triggering_card);
}

// Overwrites instance->targets[8].player, instance->targets[16].player, and for some modes instance->targets[1].player.
int damage_dealt_by_me(int player, int card, event_t event, ddbm_flags_t mode){
	return damage_dealt_by_me_arbitrary(player, card, event, mode, player, card);
}

// A common case.  Uses targets[1], [8], [16].player, as usual.  DDBM_MUST_DAMAGE_PLAYER and DDBM_TRACE_DAMAGED_PLAYERS are added to mode.
void whenever_i_deal_damage_to_a_player_he_discards_a_card(int player, int card, int event, ddbm_flags_t mode, int discard_is_random)
{
  if (damage_dealt_by_me(player, card, event, DDBM_MUST_DAMAGE_PLAYER | DDBM_TRACE_DAMAGED_PLAYERS | mode))
	{
	  card_instance_t* instance = get_card_instance(player, card);
	  int times_damaged[2] = { BYTE0(instance->targets[1].player), BYTE1(instance->targets[1].player) };
	  instance->targets[1].player = 0;
	  new_multidiscard(0, times_damaged[0], discard_is_random, player);
	  new_multidiscard(1, times_damaged[1], discard_is_random, player);
	}
}

// A common case of damage_dealt_by_me_arbitrary(), this triggers on damage dealt by the card (not necessarily a creature) {player,card} is attached to.
int attached_creature_deals_damage(int player, int card, event_t event, ddbm_flags_t mode)
{
  card_instance_t *instance;
  if ((event == EVENT_DEAL_DAMAGE || event == EVENT_GRAVEYARD_FROM_PLAY ||
       trigger_condition == TRIGGER_DEAL_DAMAGE || event == EVENT_CHANGE_TYPE) &&
      player >= 0 && card >= 0 && (instance = in_play(player, card)))
    return damage_dealt_by_me_arbitrary(instance->damage_target_player,
                                        instance->damage_target_card, event, mode, player, card);
  return 0;
}

// A common case of damage_dealt_by_me_arbitrary(), this triggers on damage dealt by the card {player,card} is equipping.
int equipped_creature_deals_damage(int player, int card, event_t event, ddbm_flags_t mode)
{
  return attached_creature_deals_damage(player, card, event, mode | DDBM_STORE_IN_TARGETS_9);
}

/* Sets triggering_player/card's targets[7].player to number of times damaged and targets[7].card to total amount of damage.  Uses targets[6] internally.  Most
 * DDBM_* flags unimplemented. */
int damage_dealt_to_me_arbitrary(int player, int card, event_t event, ddbm_flags_t mode,
                                 int triggering_player, int triggering_card)
{
  ASSERT(!(mode & DDBM_MUST_DAMAGE_OPPONENT) && "unimplemented for damage_dealt_to_me_arbitrary");
  ASSERT(!(mode & DDBM_REPORT_DAMAGE_DEALT) && "unimplemented for damage_dealt_to_me_arbitrary");
  ASSERT(!(mode & DDBM_MUST_DAMAGE_PLAYER) && "unimplemented for damage_dealt_to_me_arbitrary");
  ASSERT(!(mode & DDBM_MUST_DAMAGE_CREATURE) && "unimplemented for damage_dealt_to_me_arbitrary");
  ASSERT(!(mode & DDBM_MAY_DAMAGE_PLANESWALKER) &&
         "unimplemented for damage_dealt_to_me_arbitrary");
  // Explicitly ignore DDBM_NOT_ME
  ASSERT(!(mode & DDBM_TRACE_DAMAGED_CREATURES) &&
         "unimplemented for damage_dealt_to_me_arbitrary");
  ASSERT(!(mode & DDBM_STORE_IN_TARGETS_9) && "unimplemented for damage_dealt_to_me_arbitrary");

  card_instance_t *damage = damage_being_dealt(event);
  if (damage)
  {
    if (card >= 0 && damage->damage_target_card != card)
      return 0;
    if ((unsigned int)player < 2 && damage->damage_target_player != player)
      return 0;
  }
  if (damage)
  {
    if ((mode & DDBM_MUST_BE_COMBAT_DAMAGE) &&
        !(damage->token_status & (STATUS_COMBAT_DAMAGE | STATUS_FIRST_STRIKE_DAMAGE)))
      return 0;

    card_instance_t *instance = get_card_instance(triggering_player, triggering_card);
    if (instance->targets[6].player < 0)
      instance->targets[6].player = 0;
    if (mode & DDBM_TRACE_DAMAGED_PLAYERS)
    {
      if (damage->damage_target_player == 0)
        ++SET_BYTE0(instance->targets[6].player);
      else
        ++SET_BYTE1(instance->targets[6].player);
    }
    else
      ++instance->targets[6].player;

    if (instance->targets[6].card < 0)
      instance->targets[6].card = damage->info_slot;
    else
      instance->targets[6].card += damage->info_slot;
  }

  if (trigger_condition == TRIGGER_DEAL_DAMAGE && affect_me(triggering_player, triggering_card) &&
      reason_for_trigger_controller == triggering_player &&
      !is_humiliated(triggering_player, triggering_card, AL_TRIG))
  {
    card_instance_t *instance = get_card_instance(triggering_player, triggering_card);
    if (instance->targets[6].player <= 0)
      return 0;

    if (event == EVENT_TRIGGER)
    {
      if (mode & DDBM_TRIGGER_OPTIONAL)
        event_result |= RESOLVE_TRIGGER_AI(triggering_player);
      else
        event_result |= RESOLVE_TRIGGER_MANDATORY;
    }
    else if (event == EVENT_RESOLVE_TRIGGER)
    {
      if (mode & DDBM_TRACE_DAMAGED_PLAYERS)
      {
        int result = instance->targets[6].player;
        instance->targets[6].player = instance->targets[6].card = 0;
        return result;
      }
      instance->targets[7] = instance->targets[6]; // struct copy
      instance->targets[6].player = 0;
      instance->targets[6].card = 0;
      return 1;
    }
    else if (event == EVENT_END_TRIGGER)
    {
      instance->targets[6].player = 0;
      instance->targets[6].card = 0;
    }
  }

  return 0;
}

#if 0
/* Returns total damage dealt in instance->targets[1].card, but gets very confused if targets[16].player (for replaced damage) is set.  Number of times damaged
 * in instance->info_slot - 2, but maxes at 10.  Overwrites all targets [1]-[9], but uses [1].player internally and nothing looks at what it writes to [2]-[9].
 * Caller is expected to clear info_slot if it does inspect them.  Should therefore never be used unless it's the triggering card being damaged.  Also isn't
 * usable with any of the damage_dealt_by_me() variants on the same card.  Doesn't appear fit for use except for the bare fact of damage having been dealt;
 * deprecating. */
int damage_dealt_to_me(int player, int card, event_t event, int mode){
	// Bitfields for mode
	// 1 --> Optional trigger
	// 2 --> Mandatory trigger

	card_instance_t *instance = get_card_instance(player, card);

	if( event == EVENT_DEAL_DAMAGE ){
		card_instance_t *damage= get_card_instance( affected_card_controller, affected_card);
		if( damage->internal_card_id == damage_card ){
			if( damage->damage_target_card == card && damage->damage_target_player == player ){
					int good = 0;
					if( damage->info_slot > 0 ){
						good = damage->info_slot;
					}
					else{
						card_instance_t *trg = get_card_instance(damage->damage_source_player, damage->damage_source_card);
						if( trg->targets[16].player > 0 ){
							good = trg->targets[16].player;
						}
					}

					if( good > 0 ){
						if( instance->info_slot < 2 ){
							instance->info_slot = 2;
						}
						if( instance->info_slot < 10 ){
							instance->targets[instance->info_slot].player = damage->damage_source_player;
							instance->targets[instance->info_slot].card = damage->damage_source_card;
							instance->info_slot++;
						}
						if( instance->targets[1].player < 0 ){
							instance->targets[1].player = 0;
						}
						instance->targets[1].player+=good;
					}
			}
		}
	}

	if( instance->targets[1].player > 0 && trigger_condition == TRIGGER_DEAL_DAMAGE && affect_me(player, card) &&
		reason_for_trigger_controller == player ){
		if(event == EVENT_TRIGGER){
			if( mode & 1 ){
				event_result |= 1+player;
			}
			else{
				event_result |= 2;
			}
		}
		else if(event == EVENT_RESOLVE_TRIGGER){
				instance->targets[1].card = instance->targets[1].player;
				instance->targets[1].player = 0;
				return 1;
		}
		else if(event == EVENT_END_TRIGGER){
			instance->targets[1].player = 0;
		}
	}

	return 0;
}
#endif

int sengir_vampire_trigger(int player, int card, event_t event, int trig_mode)
{

  card_instance_t *instance = get_card_instance(player, card);

  if (is_humiliated(player, card, AL_TRIG))
  {
    return 0;
  }

  if (event == EVENT_DEAL_DAMAGE)
  {
    card_instance_t *damage = get_card_instance(affected_card_controller, affected_card);
    if (damage->internal_card_id == damage_card && damage->info_slot > 0)
    {
      if (damage->damage_source_card == card && damage->damage_source_player == player &&
          damage->damage_target_card != -1)
      {
        if (instance->targets[1].player < 10)
        {
          int good = 1;
          if (instance->targets[1].player > 2)
          {
            int i;
            for (i = 2; i < instance->targets[1].player; i++)
            {
              if (damage->damage_target_player == instance->targets[i].player &&
                  damage->damage_target_card == instance->targets[i].card)
              {
                good = 0;
                break;
              }
            }
          }
          if (good == 1)
          {
            if (instance->targets[1].player < 2)
            {
              instance->targets[1].player = 2;
            }
            int pos = instance->targets[1].player;
            instance->targets[pos].player = damage->damage_target_player;
            instance->targets[pos].card = damage->damage_target_card;
            instance->targets[1].player++;
          }
        }
      }
    }
  }

  if (event == EVENT_GRAVEYARD_FROM_PLAY)
  {
    if (!in_play(affected_card_controller, affected_card))
    {
      return 0;
    }
    if (is_what(affected_card_controller, affected_card, TYPE_CREATURE))
    {
        if (is_this_dying(affected_card_controller, affected_card))
      {
        if (instance->targets[1].player > 2)
        {
          int i;
          for (i = 2; i < instance->targets[1].player; i++)
          {
            if (instance->targets[i].player == affected_card_controller &&
                instance->targets[i].card == affected_card &&
                !check_for_death_trigger_removal(affected_card_controller, affected_card))
            {
              if (instance->targets[11].player < 0)
              {
                instance->targets[11].player = 0;
              }
              instance->targets[11].player++;
              instance->targets[i].player =
                  get_owner(instance->targets[i].player, instance->targets[i].card);
              instance->targets[i].card = get_id(affected_card_controller, affected_card);
            }
          }
        }
      }
    }
  }

  if (instance->targets[11].player > 0 && trigger_condition == TRIGGER_GRAVEYARD_FROM_PLAY &&
      player == reason_for_trigger_controller && affect_me(player, card))
  {
    if (event == EVENT_TRIGGER)
    {
      event_result |= trig_mode;
    }
    else if (event == EVENT_RESOLVE_TRIGGER)
    {
      instance->targets[11].card = instance->targets[11].player;
      instance->targets[11].player = 0;
      return 1;
    }
    else if (event == EVENT_END_TRIGGER)
    {
      instance->targets[11].player = 0;
    }
  }

  if (current_phase == PHASE_UNTAP)
  {
    instance->targets[1].player = 2;
  }

  return 0;
}



int target_player_skips_untap(int player, int card, event_t event){
	// Skip-untap effect handled in engine.c:untap_phase()
	return 0;
}

int target_player_skips_next_untap(int player, int card, int t_player ){
	int legacy = create_legacy_effect(player, card, &target_player_skips_untap);
	card_instance_t *instance = get_card_instance(player, legacy);
	instance->targets[0].player = t_player;
	instance->targets[0].card = 1;
	return legacy;
}







/* Uses targets[9] to store the player/card it was attached to at the time it was activated.  Should be called *before* the EVENT_RESOLVE_ACTIVATION handler if
 * td is provided. */


/* Uses targets[9] to store the player/card it was attached to at the time it was activated and info_slot to store the choice made.  Returns 1 during
 * EVENT_RESOLVE_ACTIVATION if the activated ability was selected, 0 if it was equipped. */


#define ALLOW_REEQUIP 1

#ifndef ALLOW_REEQUIP
static const char* already_equipping(int who_chooses, int player, int card, int targeting_player, int targeting_card)
{
  if (is_equipping(targeting_player, targeting_card))
	{
	  card_instance_t* instance = get_card_instance(targeting_player, targeting_card);
	  if (instance->targets[8].card == card && instance->targets[8].player == player)
		return "already equipping";
	}
  return NULL;
}
#endif

int hack_what_was_sacced_for_gaa = 0;	// valid only immediately after calling generic_activated_ability with event == EVENT_ACTIVATE.








int pick_target_permanent(target_definition_t *td){
	int type = td->required_type;

	if( type == TYPE_CREATURE ){
		if( pick_target(td, "TARGET_CREATURE") ){
			return 1;
		}
	}
	else if( type == TYPE_ARTIFACT ){
		if( pick_target(td, "TARGET_ARTIFACT") ){
			return 1;
		}
	}

	else if( type == TYPE_ENCHANTMENT ){
		if( pick_target(td, "TARGET_ENCHANTMENT") ){
			return 1;
		}
	}

	else if( type == TYPE_LAND ){
		if( pick_target(td, "TARGET_LAND") ){
			return 1;
		}
	}
	else{
		if( pick_target(td, "TARGET_PERMANENT") ){
			return 1;
		}
	}
	return 0;
}

int bounce_permanent_at_upkeep(int player, int card, event_t event, target_definition_t *td){

	card_instance_t *instance = get_card_instance(player, card);

	if( current_turn == player && !is_humiliated(player, card, AL_TRIG) && trigger_condition == TRIGGER_UPKEEP && affect_me(player, card)
	     && reason_for_trigger_controller == player ){
		if(event == EVENT_TRIGGER && can_target(td) ){
			event_result |= 2;
		}
		else if(event == EVENT_RESOLVE_TRIGGER){
				int count = count_upkeeps(player);
				while( count > 0 && can_target(td) ){
						if( pick_target_permanent(td) ){
							instance->number_of_targets = 1;
							bounce_permanent(instance->targets[0].player, instance->targets[0].card);
						}
						count--;
				}
		}
	}

	return 0;
}



int card_from_list(int player, int mode, int type, int flag1, int subtype, int flag2, int color, int flag3, int id, int flag4, int cc, int flag5){
	// bitflags for mode :
	// 1 --> must return a correct value
	// 2 --> Will return a CSVID
	// 4 --> Will return an Internal Card Id
	int result = -1;
	if( ai_is_speculating != 1 ){
			while( 1 ){
				int cfl = choose_a_card("Choose a card", -1, -1);
				if( make_test(player, cfl, -1, type, flag1, subtype, flag2, color, flag3, id, flag4, cc, flag5) ){
					if( mode & 2 ){
						result = cards_data[cfl].id;
						break;
					}
					if( mode & 4 ){
						result = cfl;
						break;
					}
				}
				else{
					if( !(mode & 1)  ){
						break;
					}
				}
			}
	}
	return result;
}



int id_legacy(int player, int card, event_t event){
	card_instance_t *instance = get_card_instance(player, card);

	if( instance->targets[2].player > 0 ){
		if( (instance->targets[2].player & 1) && eot_trigger(player, card, event) ){
			kill_card(player, card, KILL_EXILE);
		}
	}
	return 0;
}

int create_id_legacy(int player, int card, int t_player, int t_card, int mode){
	int legacy = -1;
	if( t_card != -1 ){
		legacy = create_targetted_legacy_effect(player, card, &id_legacy, t_player, t_card);
	}
	else{
		legacy = create_legacy_effect(player, card, &id_legacy);
	}
	card_instance_t *leg = get_card_instance(player, legacy);
	leg->targets[1].player = player;
	leg->targets[1].card = card;
	if( t_card == -1 ){
		leg->targets[0].player = t_player;
	}
	leg->targets[2].player = mode;
	leg->targets[2].card = get_id(player, card);
	leg->targets[3].card = get_id(player, card);
	return legacy;
}

int check_for_id_legacy(int player, int id){
	int i;
	for(i=0; i<2; i++){
		if( i == player || player == 2 ){
			int count = 0;
			while( count < active_cards_count[i] ){
					if( in_play(i, count) && is_what(i, count, TYPE_EFFECT) ){
						card_instance_t *instance = get_card_instance(i, count);
						if( instance->targets[2].card == id || instance->targets[3].card == id ){
							return count;
						}
					}
					count++;
			}
		}
	}
	return -1;
}

int exile_if_would_be_put_into_graveyard_legacy(int player, int card, event_t event){
	card_instance_t *instance = get_card_instance( player, card );
	if( instance->targets[0].player > -1 ){
		if( event == EVENT_GRAVEYARD_FROM_PLAY && affect_me(instance->targets[0].player, instance->targets[0].card) ){
			card_instance_t *this = get_card_instance( instance->targets[0].player, instance->targets[0].card );
			if( this->kill_code > 0 && this->kill_code < KILL_EXILE ){
				this->kill_code = KILL_EXILE;
			}
		}
		if( instance->targets[1].player > 0 && (instance->targets[1].player & 1) && eot_trigger(player, card, event) ){
			remove_special_flags2(instance->targets[0].player, instance->targets[0].card, SF2_WILL_BE_EXILED_IF_PUTTED_IN_GRAVE);
			kill_card(player, card, KILL_EXILE);
		}
	}
	return 0;
}

void exile_if_would_be_put_into_graveyard(int player, int card, int t_player, int t_card, int mode){
	if( t_player > -1 && t_card > -1 ){
		set_special_flags2(t_player, t_card, SF2_WILL_BE_EXILED_IF_PUTTED_IN_GRAVE);
		int legacy = create_targetted_legacy_effect(player, card, &exile_if_would_be_put_into_graveyard_legacy, t_player, t_card);
		card_instance_t *instance = get_card_instance( player, legacy );
		instance->targets[1].player = mode;
	}
}

void null_casting_cost(int player, int card)
{
  card_ptr_t *c = cards_ptr[get_id(player, card)];
  COST_COLORLESS -= get_updated_casting_cost(player, card, -1, EVENT_MODIFY_COST, c->req_colorless);
  COST_BLACK -= c->req_black;
  COST_BLUE -= c->req_blue;
  COST_GREEN -= c->req_green;
  COST_RED -= c->req_red;
  COST_WHITE -= c->req_white;
}

void infinite_casting_cost(void){
	COST_COLORLESS = COST_BLACK = COST_BLUE = COST_GREEN = COST_RED = COST_WHITE = 100;
}

int die_at_end_of_combat(int player, int card, event_t event){

	if( end_of_combat_trigger(player, card, event, 2) ){
		card_instance_t *instance = get_card_instance(player, card);
		kill_card(instance->targets[0].player, instance->targets[0].card, KILL_DESTROY);
	}

    if (trigger_condition == TRIGGER_END_COMBAT && affect_me(player, card) && reason_for_trigger_controller == player
		&& event == EVENT_END_TRIGGER)	// If trigger is countered, it doesn't repeat next combat
	  kill_card(player, card, KILL_EXILE);

	return 0;
}

int reveal_any_number_of_cards_of_selected_color(int player, int card, int selected_color){

	target_definition_t td;
	default_target_definition(player, card, &td, TYPE_ANY);
	td.zone = TARGET_ZONE_HAND;
	td.illegal_abilities = 0;
	td.allowed_controller = player;
	td.preferred_controller = player;
	td.required_color = selected_color;

	card_instance_t *instance = get_card_instance(player, card);


	int revealed = 0;
	int cards_array[ 500 ];
	int choice = do_dialog(player, player, card, -1, -1, " Auto mode\n Manual mode", 0);

	if( choice == 1){
		int stop = 0;
		while( can_target(&td) && stop != 1){
			   if( pick_target(&td, "TARGET_CARD") ){
				   if( valid_target(&td) ){
					   card_instance_t *rev_card = get_card_instance(player, instance->targets[0].card);
					   cards_array[revealed] = rev_card->internal_card_id;
					   instance->targets[revealed+1].card = instance->targets[0].card;
					   rev_card->state |= STATE_CANNOT_TARGET;
					   revealed++;
				   }
			   }
			   else{
					stop = 1;
			   }
		}

		if( revealed > 0){
			int i;
			for(i=0;i<revealed; i++){
				card_instance_t *crd = get_card_instance(player, instance->targets[i+1].card);
				crd->state &= ~STATE_CANNOT_TARGET;
			}
		}
	}

	if( choice == 0){
		int i;
		for(i=0;i<active_cards_count[player]; i++){
			if( in_hand(player, i) && ( get_color(player, i) & selected_color) ){
				card_instance_t *rev_card = get_card_instance(player, i);
				cards_array[revealed] = rev_card->internal_card_id;
				revealed++;
			}
		}
	}

	if( revealed > 0){
		show_deck( player, cards_array, revealed, "Cards revealed", 0, 0x7375B0 );
	}

	return revealed;
}

int reveal_cards_from_hand(int player, int card, test_definition_t *this_test){

	this_test->id = CARD_ID_RULES_ENGINE;
	this_test->id_flag = 1;

	int ha[100];
	int hac = 0;
	int rev[100];
	int revealed = 0;

	int count = 0;
	while( count < active_cards_count[player] ){
			if( in_hand(player, count) ){
				card_instance_t *crd = get_card_instance(player, count);
				ha[hac] = crd->internal_card_id;
				hac++;
			}
			count++;
	}
	if( hac > 0 ){
		int choice = 0;
		if( player == HUMAN ){
			choice = do_dialog(player, player, card, -1, -1, " Auto mode\n Manual mode", 0);
		}
		if( choice == 1){
			while( 1 ){
					int selected = show_deck( HUMAN, ha, hac, "Select a card to reveal.", 0, 0x7375B0 );
					if( selected != -1 ){
						if( new_make_test(player, ha[selected], -1, this_test) ){
							rev[revealed] = ha[selected];
							revealed++;
							ha[selected] = get_internal_card_id_from_csv_id(CARD_ID_RULES_ENGINE);
						}
						else{
							break;
						}
					}
					else{
						break;
					}
			}
		}
		else{
			count = 0;
			while( count < hac ){
					if( new_make_test(player, ha[count], -1, this_test) ){
						rev[revealed] = ha[count];
						revealed++;
					}
					count++;
			}
		}
		if( revealed > 0){
			show_deck( HUMAN, rev, revealed, "Cards revealed", 0, 0x7375B0 );
		}
	}
	return revealed;
}

// source_player/source_card are the source of the effect.  If they're unknown, put the revealed player/card there and use -1,-1 for revealed_player, revealed_card.
void reveal_card(int source_player, int source_card, int revealed_player, int revealed_card)
{
  if (ai_is_speculating != 1)
	{
	  load_text(0, "REVEALS");
	  DIALOG(source_player, source_card, EVENT_ACTIVATE,
			 DLG_FULLCARD(source_player, source_card),
			 DLG_SMALLCARD(revealed_player, revealed_card),
			 DLG_MSG(text_lines[0]));
	  //DLG_WHO_CHOOSES(HUMAN),
	  //EXE_STR(0x78F0A8), 1, 1);	//localized "OK"
	}
}

void reveal_card_iid(int source_player, int source_card, int revealed_iid)
{
  if (ai_is_speculating != 1 && revealed_iid != -1)
	{
	  load_text(0, "REVEALS");
	  DIALOG(source_player, source_card, EVENT_ACTIVATE,
			 DLG_FULLCARD(source_player, source_card),
			 DLG_SMALLCARD_ID(revealed_iid),
			 DLG_MSG(text_lines[0]));
	  //DLG_WHO_CHOOSES(HUMAN),
	  //EXE_STR(0x78F0A8), 1, 1);	//localized "OK"
	}
}

// Identical to reveal_card_iid, but arbitrary text so you can pretend it's not a reveal
void look_at_iid(int source_player, int source_card, int revealed_iid, const char* prompt)
{
  if (ai_is_speculating != 1 && revealed_iid != -1)
	DIALOG(source_player, source_card, EVENT_ACTIVATE,
		   DLG_FULLCARD(source_player, source_card),
		   DLG_SMALLCARD_ID(revealed_iid),
		   DLG_MSG(prompt));
		   //DLG_WHO_CHOOSES(HUMAN),
		   //EXE_STR(0x78F0A8), 1, 1);	//localized "OK"
}

// Gives the human a choice whether to reveal a card or not.  The AI always does.  Returns 1 if revealed.
int reveal_card_optional_iid(int source_player, int source_card, int revealed_iid, const char* prompt/*optional*/)
{
  if (ai_is_speculating != 1 && revealed_iid != -1)
	{
	  if (source_player == AI)
		{
		  reveal_card_iid(source_player, source_card, revealed_iid);
		  return 1;
		}

	  return 1 == DIALOG(source_player, source_card, EVENT_ACTIVATE,
						 DLG_FULLCARD(source_player, source_card),
						 DLG_SMALLCARD_ID(revealed_iid),
						 DLG_NO_CANCEL,
						 prompt ? prompt : "Reveal", 1, 1,
						 "Pass", 1, 1);
	}
  else
	return 0;
}



int evincars(int player, int card, event_t event, color_test_t color)
{
  if ((event == EVENT_POWER || event == EVENT_TOUGHNESS)
	  && !affect_me(player, card)
	  && in_play(player, card) && in_play(affected_card_controller, affected_card)
	  && !is_humiliated(player, card, AL_STATIC))
	{
	  if (get_color(affected_card_controller, affected_card) & get_sleighted_color_test(player, card, color))
		++event_result;
	  else
		--event_result;
	}

  return 0;
}





void set_pt_and_abilities(int player, int card, event_t event, int pow, int tou, int key, int s_key)
{
  if (event == EVENT_POWER)
  {
    if (affect_me(player, card))
      set_base_pt(pow);
  }
  else if (event == EVENT_TOUGHNESS)
  {
    if (affect_me(player, card))
      set_base_pt(tou);
  }
  else if (event == EVENT_ABILITIES && affect_me(player, card))
    event_result |= key;
  if (s_key > 0)
    special_abilities(player, card, event, s_key, player, card);
}

int get_random_card_in_hand(int player){
	if( hand_count[player] <= 0 ){
		return -1;
	}

	if ((trace_mode & 2) && current_turn == 1){
		EXE_FN(int, 0x4A70C0, int, int)(1, 24);	// receive_network_packet(1, 24)
		return EXE_DWORD(0x628C10);
	}

	int hnd[151], hnd_count = 0, c;
	hnd[0] = -1;	// in case hand_count[] is wrong and none are found
	for (c = 0; c < active_cards_count[player]; ++c){
		if (in_hand(player, c)){
			hnd[hnd_count++] = c;
		}
	}

	int rnd = hnd_count > 1 ? internal_rand(hnd_count) : 0;

	if (trace_mode & 2){	// current_turn will be 0
		EXE_DWORD(0x628C10) = hnd[rnd];
		EXE_BYTE(0x628C0C) = 24;
		EXE_FN(int, 0x4A6BC0, int, int)(0, 24);	// send_network_packet(0, 24);
	}

	return hnd[rnd];
}





int can_cast_creature_from_grave(int player, int mode, event_t event){
	const int *grave = get_grave(player);
	if( grave[0] == -1 ){
		if( mode == 1 ){
			return 0;
		}
		if( mode == 2 ){
			return -1;
		}
	}
	int count = 0;
	int count_g = count_graveyard(player);
	int max_value = 0;
	int result = 0;
	if( mode == 2 ){
		result = -1;
	}

	while( count < count_g ){
			if( is_what(-1, grave[count], TYPE_CREATURE) ){
				if( !(mode & 4) ||
					((mode & 4) && ((cards_data[grave[count]].color & COLOR_TEST_BLACK) || (cards_data[grave[count]].color & COLOR_TEST_BLUE) ||
									(cards_data[grave[count]].color & COLOR_TEST_RED)))
				  ){
					int id = cards_data[grave[count]].id;
					card_ptr_t* c = cards_ptr[ id ];
					if( has_mana_to_cast_id(player, event, id) ){
						if( mode == 1 ){
							result = 1;
							break;
						}
						if( c->ai_base_value > max_value ){
							max_value = c->ai_base_value;
							result = count;
						}
					}
				}
			}
			count++;
	}
	return result;
}

static int creatures_cannot_block_legacy(int player, int card, event_t event){

	card_instance_t *instance = get_card_instance(player, card);

	if( event == EVENT_BLOCK_LEGALITY ){
		test_definition_t this_test;
		default_test_definition(&this_test, TYPE_CREATURE);
		if( instance->targets[0].player > 0 ){
			this_test.subtype = instance->targets[0].player;
		}
		if( instance->targets[0].card > -1 ){
			this_test.subtype_flag = instance->targets[0].card;
		}
		if( instance->targets[1].player > -1 ){
			this_test.color = instance->targets[1].player;
		}
		if( instance->targets[1].card > -1 ){
			this_test.color_flag = instance->targets[1].card;
		}
		if( instance->targets[2].player > 0 ){
			this_test.id = instance->targets[2].player;
		}
		if( instance->targets[2].card > -1 ){
			this_test.id_flag = instance->targets[2].card;
		}
		if( instance->targets[3].player > -1 ){
			this_test.cmc = instance->targets[3].player;
		}
		if( instance->targets[3].card > -1 ){
			this_test.cmc_flag = instance->targets[3].card;
		}
		if( instance->targets[4].player > 0 ){
			this_test.keyword = instance->targets[4].player;
		}
		if( instance->targets[4].card > -1 ){
			this_test.keyword_flag = instance->targets[4].card;
		}
		if( instance->targets[5].player > -1 ){
			this_test.power = instance->targets[5].player;
		}
		if( instance->targets[5].card > -1 ){
			this_test.power_flag = instance->targets[5].card;
		}
		if( instance->targets[6].player > -1 ){
			this_test.toughness = instance->targets[6].player;
		}
		if( instance->targets[6].card > -1 ){
			this_test.toughness_flag = instance->targets[6].card;
		}
		if( instance->targets[7].player > 0 ){
			this_test.state = instance->targets[7].player;
		}
		if( instance->targets[7].card > -1 ){
			this_test.state_flag = instance->targets[7].card;
		}
		if( new_make_test_in_play(affected_card_controller, affected_card, -1, &this_test) ){
			event_result = 1;
		}
	}

	if( instance->targets[8].player > 0 && (instance->targets[8].player & 1) && event == EVENT_CLEANUP ){
		kill_card(player, card, KILL_EXILE);
	}

	return 0;
}

int creatures_cannot_block(int player, int card, test_definition_t *this_test, int mode){
	int legacy = create_legacy_effect(player, card, &creatures_cannot_block_legacy);
	card_instance_t *instance = get_card_instance(player, legacy);
	if( this_test != NULL ){
		instance->targets[0].player = this_test->subtype;
		instance->targets[0].card = this_test->subtype_flag;
		instance->targets[1].player = this_test->color;
		instance->targets[1].card = this_test->color_flag;
		instance->targets[2].player = this_test->id;
		instance->targets[2].card = this_test->id_flag;
		instance->targets[3].player = this_test->cmc;
		instance->targets[3].card = this_test->cmc_flag;
		instance->targets[4].player = this_test->keyword;
		instance->targets[4].card = this_test->keyword_flag;
		instance->targets[5].player = this_test->power;
		instance->targets[5].card = this_test->power_flag;
		instance->targets[6].player = this_test->toughness;
		instance->targets[6].card = this_test->toughness_flag;
		instance->targets[7].player = this_test->state;
		instance->targets[7].card = this_test->state_flag;
	}
	instance->targets[8].player = mode;
	return legacy;
}

static int effect_creature1_cant_block_creature2_until_eot(int player, int card, event_t event)
{
  if (event == EVENT_BLOCK_LEGALITY)
	{
	  card_instance_t* instance = get_card_instance(player, card);
	  if (affect_me(instance->damage_target_player, instance->damage_target_card)
		  && attacking_card_controller == instance->targets[0].player && attacking_card == instance->targets[0].card)
		event_result = 1;
	}

  if (event == EVENT_CLEANUP)
	kill_card(player, card, KILL_EXILE);

  return 0;
}

void creature1_cant_block_creature2_until_eot(int src_player, int src_card, int t_player, int t_card, int unblockable_player, int unblockable_card)
{
  int leg = create_targetted_legacy_effect(src_player, src_card, effect_creature1_cant_block_creature2_until_eot, t_player, t_card);
  if (leg >= 0)
	{
	  card_instance_t* legacy = get_card_instance(src_player, leg);
	  legacy->targets[0].player = unblockable_player;
	  legacy->targets[0].card = unblockable_card;
	  legacy->number_of_targets = 1;
	}
}

int can_cast_spell_from_grave(int player, int mode){
	const int *grave = get_grave(player);
	if( grave[0] == -1 ){
		if( mode == 1 ){
			return 0;
		}
		if( mode == 2 ){
			return -1;
		}
	}
	int count = 0;
	int count_g = count_graveyard(player);
	int max_value = 0;
	int result = 0;
	if( mode == 2 ){
		result = -1;
	}

	while( count < count_g ){
			if( (is_what(-1, grave[count], TYPE_SORCERY) ||
				is_what(-1, grave[count], TYPE_INSTANT) ||
				is_what(-1, grave[count], TYPE_INTERRUPT)) &&
				! is_what(-1, grave[count], TYPE_CREATURE)
			  ){
				int id = cards_data[grave[count]].id;
				card_ptr_t* c = cards_ptr[ id ];
				if( has_mana_multi(player, c->req_colorless, c->req_black, c->req_blue, c->req_green,
					c->req_red, c->req_white)
				  ){
					if( can_legally_play_iid(player, grave[count]) ){
						if( mode == 1 ){
							result = 1;
							break;
						}
						if( c->ai_base_value > max_value ){
							max_value = c->ai_base_value;
							result = count;
						}
					}
				}
			}
			count++;
	}
	return result;
}



int get_owner(int player, int card)
{
  return check_state(player, card, STATE_OWNED_BY_OPPONENT) ? 1 : 0;
}





























int twincast(int player, int card, int event, target_definition_t *td /*optional*/,
             int *ret_copy /*optional*/)
{
  target_definition_t td_default;
  if (!td)
  {
    td = &td_default;
    counterspell_target_definition(player, card, td, TYPE_INSTANT | TYPE_SORCERY);
    td->preferred_controller = 2;
  }

  if (event == EVENT_CAN_CAST || event == EVENT_CAST_SPELL || event == EVENT_CAN_CHANGE_TARGET ||
      event == EVENT_CHANGE_TARGET)
    return counterspell(player, card, event, td, 0);

  if (event == EVENT_RESOLVE_SPELL)
  {
    if (counterspell_validate(player, card, td, 0))
    {
      card_instance_t *instance = get_card_instance(player, card);
      int copy =
          copy_spell_from_stack(player, instance->targets[0].player, instance->targets[0].card);
      if (ret_copy)
        *ret_copy = copy;
    }
  }

  return 0;
}

int activate_twincast(int player, int card, int event, target_definition_t* td /*optional*/, int* ret_copy /*optional*/)
{
  if (event == EVENT_CAN_ACTIVATE)
	return twincast(player, card, EVENT_CAN_CAST, td, ret_copy);

  if (event == EVENT_ACTIVATE)
	return twincast(player, card, EVENT_CAST_SPELL, td, ret_copy);

  if (event == EVENT_RESOLVE_ACTIVATION)
	return twincast(player, card, EVENT_RESOLVE_SPELL, td, ret_copy);

  return 0;
}















void get_an_additional_combat_phase(void){
	current_phase = PHASE_DECLARE_ATTACKERS;
	land_can_be_played &= ~TENTATIVE_LCBP_DURING_COMBAT;
}





/* Flags an effect card to use alternate text, set off with |#1, |#2, etc. tags in its text.
 * Works only on csvid 903 effects (which includes both LEGACY_EFFECT_CUSTOM and LEGACY_EFFECT_GENERIC) other than LEGACY_EFFECT_PUMP.  So it's nonfunctional
 * with create_card_name_legacy().
 * Returns legacycard again. */


// Sets {player,legacycard}'s image (and therefore title and text) to be csvid.  Works only on effect cards.  Returns legacycard again.
int set_legacy_image(int player, int csvid, int legacycard)
{
  if (legacycard >= 0)
	{
	  card_instance_t* leg = get_card_instance(player, legacycard);
	  leg->display_pic_csv_id = csvid;
	  leg->display_pic_num = get_card_image_number(csvid, leg->damage_source_player, leg->damage_source_card);
	}
  return legacycard;
}









void set_x_for_x_spells(int player, int card, event_t event, int value){
	card_instance_t *instance = get_card_instance(player, card);
	int diff = true_get_updated_casting_cost(player, card, -1, event, -1);
	instance->info_slot = value;
	if( diff < 0 ){
		instance->info_slot -= diff;
	}
}



int basic_spell(int player, int card, event_t event){
	//A spell that has no restriction such as targets, life to pay and so on, always returning 1 in EVENT_CAN_CAST and doing absolutely nothing in EVENT_CAST_SPELL
	return generic_spell(player, card, event, 0, NULL, NULL, 0, NULL);
}




void fight(int p1, int c1, int p2, int c2){
	/* 701.10. Fight
	 *
	 * 701.10a A spell or ability may instruct a creature to fight another creature or it may instruct two creatures to fight each other. Each of those
	 * creatures deals damage equal to its power to the other creature.
	 *
	 * 701.10b If a creature instructed to fight is no longer on the battlefield or is no longer a creature, no damage is dealt. If a creature is an illegal
	 * target for a resolving spell or ability that instructs it to fight, no damage is dealt.
	 *
	 * 701.10c If a creature fights itself, it deals damage equal to its power to itself twice.
	 *
	 * 701.10d The damage dealt when a creature fights isn't combat damage. */

	card_instance_t* i1 = in_play(p1, c1);
	if (!i1 || !is_what(p1, c1, TYPE_CREATURE)){
		return;
	}
	card_instance_t* i2 = in_play(p2, c2);
	if (!i2 || !is_what(p2, c2, TYPE_CREATURE)){
		return;
	}

	i1->regen_status |= KEYWORD_RECALC_POWER;
	int pow1 = get_abilities(p1, c1, EVENT_POWER, -1);

	i2->regen_status |= KEYWORD_RECALC_POWER;
	int pow2 = get_abilities(p2, c2, EVENT_POWER, -1);

	play_sound_effect(WAV_ASWANJAG);

	damage_creature(p2, c2, pow1, p1, c1);
	damage_creature(p1, c1, pow2, p2, c2);

	dispatch_event(p1, c1, EVENT_FIGHT);
	dispatch_event(p2, c2, EVENT_FIGHT);
}

void upkeep_trigger_ability_mode(int player, int card, event_t event, int whose_upkeep, resolve_trigger_t trigger_mode){
	if( trigger_condition == TRIGGER_UPKEEP && affect_me(player, card) && reason_for_trigger_controller == player &&
		(whose_upkeep == current_turn || whose_upkeep == 2) &&
		!is_humiliated(player, card, AL_TRIG)
	  ){
		int count = count_upkeeps(current_turn);
		if(event == EVENT_TRIGGER && count > 0){
			if (trigger_mode == RESOLVE_TRIGGER_DUH){
				trigger_mode = duh_mode(player) ? RESOLVE_TRIGGER_MANDATORY : RESOLVE_TRIGGER_OPTIONAL;
			}
			event_result |= trigger_mode;
		}
		else if(event == EVENT_RESOLVE_TRIGGER){
			while( count > 0 && call_card_function(player, card, EVENT_UPKEEP_TRIGGER_ABILITY) != -1){
				count--;
			}
		}
	}
}

void upkeep_trigger_ability(int player, int card, event_t event, int whose_upkeep){
	upkeep_trigger_ability_mode(player, card, event, whose_upkeep, RESOLVE_TRIGGER_MANDATORY);
}

// Untap all cards with type types controlled by player during his opponent's untap step.  Stores temporary data in where_to_store.


void set_flags_when_spell_is_countered(int player, int card, int c_player, int c_card){
	if( is_what(c_player, c_card, TYPE_CREATURE) && player != c_player ){
		increase_trap_condition(c_player, TRAP_SUMMONING_TRAP, 1);
	}
}









int this_dies_trigger(int player, int card, event_t event, resolve_trigger_t trigger_mode)
{
  return this_dies_trigger_impl(player, card, event, trigger_mode, player, 0, 0);
}

/* Same as this_dies_trigger(), but the card's owner, not its controller, gets the legacy effect if card is stolen as it dies.  In particular, when this returns
 * true, player will be the owner since it's the legacy effect, not the original card, at that point. */
int this_dies_trigger_for_owner(int player, int card, event_t event, resolve_trigger_t trigger_mode)
{
  return this_dies_trigger_impl(player, card, event, trigger_mode, get_owner(player, card), 0, 0);
}

int attached_creature_dies_trigger(int player, int card, event_t event, resolve_trigger_t trigger_mode)
{
  return this_dies_trigger_impl(player, card, event, trigger_mode, player, 1, 0);
}

// controller of attached creature, not of {player,card}, gets the trigger
int attached_creature_dies_trigger_for_controller(int player, int card, event_t event, resolve_trigger_t trigger_mode)
{
  return this_dies_trigger_impl(player, card, event, trigger_mode, 3, 1, 0);
}

// exiledby.c is more flexible
int exile_permanent_and_remember_it(int player, int card, int t_player, int t_card, int store_loc){
	card_instance_t *instance = get_card_instance( player, card );
	int result = -1;
	if( instance->targets[store_loc].card < store_loc+1 ){
		instance->targets[store_loc].card = store_loc+1;
	}
	int max_pos = is_what(player, card, TYPE_EFFECT) ? 18 : 10;
	int pos = instance->targets[store_loc].card;
	if( pos < max_pos && ! is_token(t_player, t_card) ){
		instance->targets[pos].player = is_stolen(t_player, t_card) ? 1-t_player : t_player;
		instance->targets[pos].card = get_original_internal_card_id(t_player, t_card);
		if( max_pos == 10 ){
			create_card_name_legacy(player, card, get_id(t_player, t_card));
		}
		instance->targets[store_loc].card++;
		result = instance->targets[pos].card;
	}
	kill_card(t_player, t_card, KILL_EXILE);
	return result;
}















/* Translates type into text, writing at most maxlen characters into buf (including terminating NUL).  Returns buf. */




// Resolve a Twiddle-like effect.


// Change ai_modifier for a Twiddle-like effect.  Should happen in EVENT_ACTIVATE or EVENT_CAST_SPELL after targetting.




int get_power(int player, int card)
{
	return get_abilities(player, card, EVENT_POWER, -1);
}

int get_toughness(int player, int card)
{
	return get_abilities(player, card, EVENT_TOUGHNESS, -1);
}











static event_t hack_current_event = 0;	// There's a couple exe variables that look like they always hold the current event (or maybe trigger), but I'm not 100% certain of any of them.



// See effect_werewolf_ransacker() in dark_ascension.c for intended usage.
int effect_put_into_a_graveyard_this_way(int player, int card, event_t event)
{
  card_instance_t *instance = get_card_instance(player, card);

  if (event == EVENT_GRAVEYARD_FROM_PLAY)
  {
    if (affect_me(instance->damage_target_player,
                  instance->damage_target_card)) // detach, so this isn't immediately buried
      instance->damage_target_player = instance->damage_target_card = -1;
  }

  if (event == EVENT_STATIC_EFFECTS ||
      (trigger_condition == TRIGGER_LEAVE_PLAY && affect_me(player, card) &&
       reason_for_trigger_controller == player &&
       trigger_cause_controller == instance->targets[0].player &&
       trigger_cause == instance->targets[0].card))
  {
    if (instance->targets[0].player < 0 || instance->targets[0].card < 0 ||
        instance->targets[3].player == 42)
    {
      kill_card(player, card, KILL_EXILE);
      return 0;
    }

    // May need to hack put_on_top_of_deck(), oubliette_helper_5(), and bounce_permanent() to clear
    // STATUS_DYING and/or kill_code before triggering.
    card_instance_t *aff =
        get_card_instance(instance->targets[0].player, instance->targets[0].card);
    if (!(aff->token_status & STATUS_DYING) ||
        (aff->kill_code != KILL_DESTROY && aff->kill_code != KILL_BURY))
    {
      kill_card(player, card, KILL_EXILE);
      return 0;
    }

    if (event == EVENT_TRIGGER)
      return RESOLVE_TRIGGER_MANDATORY;

    if (event == EVENT_RESOLVE_TRIGGER)
    {
      if (check_for_death_trigger_removal(instance->targets[0].player, instance->targets[0].card))
        return 0;
      instance->targets[3].player = 42;
      return 1;
    }
  }

  if (event == EVENT_PHASE_CHANGED || event == EVENT_CLEANUP)
    kill_card(player, card, KILL_EXILE);

  return 0;
}

// If test is NULL, then it defaults to all non-token cards.  Only needs to be called (and thus test only needs to be set) during EVENT_GRAVEYARD_FROM_PLAY.


/* Must set replace_milled as the very last thing after this returns true if anything is done with the card.  If test is NULL, then it defaults to all cards.
 * Only needs to be called (and thus test only needs to be set) if xtrigger_condition() == XTRIGGER_REPLACE_MILL. */


/* Must set replace_milled as the very last thing after this returns true if anything is done with the card.  If test is NULL, then it defaults to all cards.
 * Only needs to be called (and thus test only needs to be set) if xtrigger_condition() == XTRIGGER_REPLACE_CARD_TO_GY_FROM_ANYWHERE_BUT_LIBRARY. */


/* A front end to XTRIGGER_CARD_TO_GY_FROM_ANYWHERE_BUT_LIBRARY.  trigger_cause_controller (whose graveyard), trigger_cause (iid of card), gy_from_anywhere_pos
 * (tentative position), and gy_from_anywhere_source (canonical graveyard_source[][] value) are all still valid.  Only needs to be called (and thus test only
 * needs to be set) if xtrigger_condition() == XTRIGGER_CARD_TO_GY_FROM_ANYWHERE_BUT_LIBRARY. */
int when_a_card_is_put_into_graveyard_from_anywhere_but_library_do_something(
    int player, int card, int event, int t_player, resolve_trigger_t trigger_mode,
    test_definition_t *test /*optional*/)
{
  enable_xtrigger_flags |= ENABLE_XTRIGGER_CARD_TO_GY_FROM_ANYWHERE_BUT_LIBRARY;

  if (xtrigger_condition() == XTRIGGER_CARD_TO_GY_FROM_ANYWHERE_BUT_LIBRARY &&
      affect_me(player, card) && reason_for_trigger_controller == player &&
      (trigger_cause_controller == t_player || t_player == ANYBODY) &&
      (!is_humiliated(player, card, AL_STATIC) ||
       (is_this_dying(trigger_cause_controller, trigger_cause) &&
        check_for_death_trigger_removal(trigger_cause_controller, trigger_cause))) &&
      (!test || new_make_test(trigger_cause_controller, trigger_cause, -1, test)))
  {
    if (event == EVENT_TRIGGER)
    {
      if (trigger_mode == RESOLVE_TRIGGER_DUH)
        trigger_mode = duh_mode(player) ? RESOLVE_TRIGGER_MANDATORY : RESOLVE_TRIGGER_OPTIONAL;

      event_result |= trigger_mode;
    }
    if (event == EVENT_RESOLVE_TRIGGER)
      return 1;
  }

  return 0;
}

/* A fairly common case.  If test is NULL, then it defaults to all non-token cards.  Only needs to be called (and thus test only needs to be set) during
 * EVENT_GRAVEYARD_FROM_PLAY, XTRIGGER_REPLACE_CARD_TO_GY_FROM_ANYWHERE_BUT_LIBRARY, and XTRIGGER_REPLACE_MILL. */


int can_play_iid(int player, int event, int iid){
	if( can_legally_play_iid(player, iid) && has_mana_to_cast_iid(player, event, iid) )
		return 1;
	return 0;
}

void cannot_attack_alone(int player, int card, event_t event){
  if (event == EVENT_ATTACK_LEGALITY && affect_me(player, card))
	{
	  target_definition_t td;
	  default_target_definition(player, card, &td, TYPE_CREATURE);
	  td.allowed_controller = player;
	  td.preferred_controller = player;
	  td.illegal_abilities = 0;
	  td.required_state = TARGET_STATE_ATTACKING;

	  if (!can_target(&td))
		event_result = 1;
	}
}

void cannot_block_alone(int player, int card, event_t event){
  if ( event == EVENT_BLOCK_LEGALITY && affect_me(player, card))
	{
	  target_definition_t td;
	  default_target_definition(player, card, &td, TYPE_CREATURE);
	  td.allowed_controller = player;
	  td.preferred_controller = player;
	  td.illegal_abilities = 0;
	  td.required_state = TARGET_STATE_BLOCKING;

	  if (!can_target(&td))
		event_result = 1;
	}
}

int nightveil_specter_like_ability(int player, int card, event_t event, unsigned int flags, unsigned int cards_to_exile, unsigned int csvid_to_display){

	int exby_flags = (flags & NSLA_MUST_PAY_MANACOST_OF_EXILED_CARDS) ? EXBY_TEST_HAS_MANA_TO_CAST : 0;

	int ddbm_flags = (flags & NSLA_EXILE_ONLY_WITH_COMBAT_DAMAGE) ? DDBM_MUST_BE_COMBAT_DAMAGE : 0;
	ddbm_flags |= (flags & NSLA_EXILE_ONLY_WHEN_DAMAGING_PLAYER) ? DDBM_MUST_DAMAGE_PLAYER : 0;

	if( damage_dealt_by_me(player, card, event, ddbm_flags) ){
		int t_player = 1-player;
		if( flags & NSLA_EXILE_FROM_PLAYERS_DECK ){
			t_player = player;
		}
		unsigned int count = 0;
		while( count < cards_to_exile && deck_ptr[t_player][0] != -1){
				exiledby_remember(player, card, t_player, deck_ptr[t_player][0], NULL, NULL);
				rfg_card_in_deck(t_player, 0);
				count++;
		}
	}

  if (event == EVENT_CAN_ACTIVATE)
	{
	  int can_cast = exiledby_choose(player, card, csvid_to_display, EXBY_CAN_CAST|EXBY_TEST_CAN_CAST|exby_flags, event, NULL, 1);
	  if (IS_AI(player) || can_cast)
		return can_cast;
	  else
		return 1;	// human can always list which cards have been exiled
	}

  if (event == EVENT_ACTIVATE)
	{
	  if (!IS_AI(player))
		{
		  int can_cast = exiledby_choose(player, card, csvid_to_display, EXBY_CAN_CAST|EXBY_TEST_CAN_CAST|exby_flags, event, NULL, 1);
		  if (DIALOG(player, card, event, DLG_NO_STORAGE, DLG_NO_CANCEL,
					 "Show exiled cards", 1, 1,
					 "Play an exiled card", can_cast, 1) == 1)
			{
			  exiledby_choose(player, card, csvid_to_display, EXBY_CHOOSE, 0, NULL, 1);
			  cancel = 1;
			  return 0;
			}
		}

	  int rval = exiledby_choose(player, card, csvid_to_display, EXBY_CHOOSE|EXBY_TEST_CAN_CAST|exby_flags, event, NULL, 1);
	  int* loc = (int*)rval;
	  if (!loc)
		spell_fizzled = 1;
	  else
		{
			int raw = *loc;
			int t_player = *loc & 0x80000000 ? 1 : 0;
			int iid = *loc & ~0x80000000;
			int csvid = cards_data[iid].id;
			*loc = -1;
			if( (exby_flags & EXBY_TEST_HAS_MANA_TO_CAST) ){
				charge_mana_from_id(player, -1, event, csvid);
			}
			if( cancel != 1 && play_card_in_exile_for_free(player, t_player, csvid) != -1){
				cant_be_responded_to = 1;	// response to the spell allowed by put_card_on_stack(), but none for this activation
			}
			else{
				*loc = raw;
				cancel = 1;
			}
		}
	}

	if( (flags & NSLA_REMOVE_STORED_CARDS_AT_EOT) && event == EVENT_CLEANUP ){
		int id = exiledby_detach(player, card);
		exiledby_destroy_detached(player, id);
	}

	return 0;
}







int card_drawn_trigger(int player, int card, event_t event, int who_draws, resolve_trigger_t trigger_mode)
{
  if (trigger_condition == TRIGGER_CARD_DRAWN && affect_me(player, card) && reason_for_trigger_controller == player
	  && (trigger_cause_controller == who_draws || who_draws == ANYBODY)
	  && in_play(player, card) && !is_humiliated(player, card, AL_TRIG))
	{
	  if (event == EVENT_TRIGGER)
		{
		  if (trigger_mode == RESOLVE_TRIGGER_DUH)
			trigger_mode = duh_mode(player) ? RESOLVE_TRIGGER_MANDATORY : RESOLVE_TRIGGER_OPTIONAL;

		  event_result |= trigger_mode;
		}

	  if (event == EVENT_RESOLVE_TRIGGER)
		return 1;
	}

  return 0;
}

int beginning_of_phase(int player, int card, event_t event, int aff_player, int aff_card, int phase)
{
  card_instance_t *instance = get_card_instance(player, card);

  if (event == EVENT_PHASE_CHANGED && current_phase == phase && instance->eot_toughness == 0 &&
      in_play(player, card))
  {
    if ((aff_player == ANYBODY || current_turn == aff_player) &&
        (aff_card == -1 || aff_card == card))
    {
      instance->eot_toughness = 1; // prevent being asked multiple times
      return 1;
    }
  }

  if (event == EVENT_PHASE_CHANGED && current_phase != phase)
  {
    instance->eot_toughness = 0;
  }

  return 0;
}



void permanents_enters_battlefield_tapped(int player, int card, event_t event, int t_player, int type, test_definition_t *test){
	/* If this changes the events it looks at, please grep for its uses - at least one card filters by the same events
	 * so it doesn't construct a test_definition_t every single time its function is called. */
	if( (event == EVENT_CAST_SPELL || event == EVENT_ENTERING_THE_BATTLEFIELD_AS_CLONE) && ! affect_me(player, card) ){
		if( is_what(player, card, TYPE_EFFECT) || (in_play(player, card) && ! is_humiliated(player, card, AL_TRIG)) ){
			if( affected_card_controller == t_player || t_player == ANYBODY ){
				if( (type && is_what(affected_card_controller, affected_card, type)) ||
					(test != NULL && new_make_test_in_play(affected_card_controller, affected_card, -1, test))
				  ){
					if( ! check_state(affected_card_controller, affected_card, STATE_TAPPED) ){
						add_state(affected_card_controller, affected_card, STATE_TAPPED);
						if( spell_fizzled == 1 ){
							remove_state(affected_card_controller, affected_card, STATE_TAPPED);
						}
					}
				}
			}
		}
	}
}

void popup(const char* title, const char* fmt, ...)
{
  char buf[8000];
  va_list args;
  va_start(args, fmt);
  vsnprintf(buf, 8000, fmt, args);
  va_end(args);

  MessageBox(0, buf, title, MB_ICONERROR|MB_TASKMODAL);
}

int recycling_land(int player, int card, event_t event, int recycling_type, int cless, int black, int blue, int green, int red, int white){

	char msg[100] = "Select a card.";
	test_definition_t this_test;
	new_default_test_definition(&this_test, recycling_type, msg);

	card_instance_t *instance = get_card_instance( player, card );

	if( event == EVENT_ACTIVATE ){
		int choice = 0;
		int ai_choice = 0;
		if( ! paying_mana() && has_mana_for_activated_ability(player, card, cless+1, black, blue, green, red, white) &&
			count_graveyard_by_type(player, recycling_type) > 0
		  ){
			ai_choice = 1;
		}

		if( ai_choice == 1){
			choice = do_dialog(player, player, card, -1, -1, " Add 1\n Recycle\n Cancel", ai_choice);
		}

		if( choice == 0){
			return mana_producer(player, card, event);
		}

		else if( choice == 1 ){
				tap_card(player, card);
				if( charge_mana_for_activated_ability(player, card, cless, black, blue, green, red, white) ){
					if( new_select_target_from_grave(player, card, player, 0, AI_MAX_VALUE, &this_test, 0) != -1 ){
						instance->info_slot = 1;
					}
					else{
						untap_card_no_event(player, card);
						spell_fizzled = 1;
					}
				}
				else{
					untap_card_no_event(player, card);
				}
		}

		else{
			spell_fizzled = 1;
		}
	}
	else if( event == EVENT_RESOLVE_ACTIVATION ){
			if( instance->info_slot > 0 ){
				int selected = validate_target_from_grave(player, card, player, 0);
				if( selected != -1 ){
					from_graveyard_to_deck(player, selected, 1);
				}
				card_instance_t *parent = get_card_instance( instance->parent_controller, instance->parent_card );
				parent->info_slot = 0;
			}
	}
	else{
		return mana_producer(player, card, event);
	}

	return 0;
}

int card_fake_card(int player, int card, event_t event){


	card_instance_t *instance = get_card_instance(player, card);

	if( event == EVENT_CAN_ACTIVATE ){
		return 1;
	}
	else if( event == EVENT_RESOLVE_ACTIVATION ){
			 life[player] = get_color(player, instance->parent_card);
	}

	return 0;
}

int insurrection_effect(int player, int card){

	manipulate_all(player, card, player, TYPE_CREATURE, 0, 0, 0, 0, 0, 0, 0, -1, 0, ACT_UNTAP);
	pump_subtype_until_eot(player, card, player, -1, 0, 0, 0, SP_KEYWORD_HASTE);

	int count = active_cards_count[1-player]-1;
	int result = 0;
	while( count > -1 ){
			if( in_play(1-player, count) && is_what(1-player, count, TYPE_CREATURE) ){
				effect_act_of_treason(player, card, 1-player, count);
				result++;
			}
			count--;
	}
	return result;
}

static int default_tutor3(int player, int card){
	return 1;
}

int tutor(int, int, int (*func_ptr)(int, int),  int);	// deprecated
int pattern_of_rebirth_tutor (int player){
	return tutor(player, TYPE_CREATURE, &default_tutor3, TUTOR_PLAY );
}

static int is_karoo(int csvid){

	switch( csvid ){
			case (CARD_ID_AZORIUS_CHANCERY):
			case (CARD_ID_BOROS_GARRISON):
			case (CARD_ID_DIMIR_AQUEDUCT):
			case (CARD_ID_GOLGARI_ROT_FARM):
			case (CARD_ID_GRUUL_TURF):
			case (CARD_ID_IZZET_BOILERWORKS):
			case (CARD_ID_NOGGLE_BRIDGEBREAKER):
			case (CARD_ID_ORZHOV_BASILICA):
			case (CARD_ID_RAKDOS_CARNARIUM):
			case (CARD_ID_SELESNYA_SANCTUARY):
			case (CARD_ID_SIMIC_GROWTH_CHAMBER):
				return 1;
				break;
			default:
				return 0;
	}
}

static const char* is_not_karoo(int who_chooses, int player, int card){
	if( ! is_karoo(get_id(player, card)) ){
		return NULL;
	}
	return "AI helper function";
}

int karoo(int player, int card, event_t event, int color1, int color2, int ai_card)
{
  /*
  if (player == AI && !(trace_mode & 2)){
          if (event == EVENT_CHANGE_TYPE && affect_me(player, card)){
                  event_result = get_internal_card_id_from_csv_id(ai_card);
          }
          return 0;
  }
  */
  if (!is_humiliated(player, card, AL_TRIG))
    comes_into_play_tapped(player, card, event);

  if (event == EVENT_CAST_SPELL && affect_me(player, card) && !is_humiliated(player, card, AL_TRIG))
  {
    if (!IS_AI(player))
      return 0;
    int mod = -15;
    target_definition_t td;
    base_target_definition(player, card, &td, TYPE_LAND);
    td.allowed_controller = player;
    td.preferred_controller = player;
    td.allow_cancel = 0;
    td.extra = (int32_t)is_not_karoo;
    td.special = TARGET_SPECIAL_EXTRA_FUNCTION;

    if (can_target(&td))
    {
      mod = 24;
    }
    td.required_state = TARGET_STATE_TAPPED;
    if (can_target(&td))
    {
      mod = 48;
    }
    ai_modifier += mod;
  }

  etb_ability(player, card, event, RESOLVE_TRIGGER_MANDATORY);

  if (event == EVENT_ETB && !is_humiliated(player, card, AL_TRIG))
  {
    target_definition_t td;
    base_target_definition(player, card, &td, TYPE_LAND);
    td.allowed_controller = player;
    td.preferred_controller = player;
    td.allow_cancel = 0;
    td.illegal_abilities = 0;

    card_instance_t *instance = get_card_instance(player, card);

    if (can_target(&td))
    {
      if (IS_AI(player))
      {
        td.extra = (int32_t)is_not_karoo;
        td.special = TARGET_SPECIAL_EXTRA_FUNCTION;
      }
      if (new_pick_target(&td, "Choose a land to bounce", 0, GS_LITERAL_PROMPT))
      {
        bounce_permanent(player, instance->targets[0].card);
      }
      if (instance->number_of_targets == 0)
      {
        base_target_definition(player, card, &td, TYPE_LAND);
        td.allowed_controller = player;
        td.preferred_controller = player;
        td.allow_cancel = 0;
        td.illegal_abilities = 0;
        if (new_pick_target(&td, "Choose a land to bounce", 0, GS_LITERAL_PROMPT))
        {
          bounce_permanent(player, instance->targets[0].card);
        }
      }
      instance->number_of_targets = 0;
    }
  }

  return two_mana_land(player, card, event, color1, color2);
}

void play_land_sound_effect(int player, int card){
	card_data_t* cd = get_card_data(player, card);
	play_land_sound_effect_force_color(player, card, cd->color);
}

void play_land_sound_effect_force_color(int player, int card, int colors){
	if (!is_what(player, card, TYPE_LAND)){
		// For e.g. Magus of the Coffers and other cards call lands' functions
		return;
	}

	colors &= COLOR_TEST_ANY_COLORED;	// so e.g. COLOR_TEST_WHITE below also includes COLOR_TEST_WHITE|COLOR_TEST_COLORLESS, for Karoo; and COLOR_TEST_ARTIFACT is treated like COLOR_TEST_COLORLESS.

	wav_t wav1 = -1, wav2 = -1;
	switch (colors){
		// colorless
		case 0:					wav1 = WAV_GREY;	break;

		// one color
		case COLOR_TEST_BLACK:	wav1 = WAV_BLACK;	break;
		case COLOR_TEST_BLUE:	wav1 = WAV_BLUE;	break;
		case COLOR_TEST_GREEN:	wav1 = WAV_GREEN;	break;
		case COLOR_TEST_RED:	wav1 = WAV_RED;		break;
		case COLOR_TEST_WHITE:	wav1 = WAV_WHITE;	break;

		// two colors
		case COLOR_TEST_WHITE|COLOR_TEST_BLUE:	wav1 = WAV_WHITEBLUE;	break;
		case COLOR_TEST_BLUE|COLOR_TEST_BLACK:	wav1 = WAV_BLUEBLACK;	break;
		case COLOR_TEST_BLACK|COLOR_TEST_RED:	wav1 = WAV_BLACKRED;	break;
		case COLOR_TEST_RED|COLOR_TEST_GREEN:	wav1 = WAV_REDGREEN;	break;
		case COLOR_TEST_GREEN|COLOR_TEST_WHITE:	wav1 = WAV_GREENWHITE;	break;

		case COLOR_TEST_WHITE|COLOR_TEST_BLACK:	wav1 = WAV_WHITEBLACK;	break;
		case COLOR_TEST_BLACK|COLOR_TEST_GREEN:	wav1 = WAV_BLACKGREEN;	break;
		case COLOR_TEST_GREEN|COLOR_TEST_BLUE:	wav1 = WAV_GREENBLUE;	break;
		case COLOR_TEST_BLUE|COLOR_TEST_RED:	wav1 = WAV_BLUERED;		break;
		case COLOR_TEST_RED|COLOR_TEST_WHITE:	wav1 = WAV_REDWHITE;	break;

		// three colors - bit of a hack
		case COLOR_TEST_ANY_COLORED & ~(COLOR_TEST_WHITE|COLOR_TEST_BLUE):	wav1 = WAV_RED;		wav2 = WAV_BLACKGREEN;	break;
		case COLOR_TEST_ANY_COLORED & ~(COLOR_TEST_BLUE|COLOR_TEST_BLACK):	wav1 = WAV_GREEN;	wav2 = WAV_REDWHITE;	break;
		case COLOR_TEST_ANY_COLORED & ~(COLOR_TEST_BLACK|COLOR_TEST_RED):	wav1 = WAV_WHITE;	wav2 = WAV_GREENBLUE;	break;
		case COLOR_TEST_ANY_COLORED & ~(COLOR_TEST_RED|COLOR_TEST_GREEN):	wav1 = WAV_BLUE;	wav2 = WAV_WHITEBLACK;	break;
		case COLOR_TEST_ANY_COLORED & ~(COLOR_TEST_GREEN|COLOR_TEST_WHITE):	wav1 = WAV_BLACK;	wav2 = WAV_BLUERED;		break;

		case COLOR_TEST_ANY_COLORED & ~(COLOR_TEST_WHITE|COLOR_TEST_BLACK):	wav1 = WAV_BLUE;	wav2 = WAV_REDGREEN;	break;
		case COLOR_TEST_ANY_COLORED & ~(COLOR_TEST_BLACK|COLOR_TEST_GREEN):	wav1 = WAV_RED;		wav2 = WAV_WHITEBLUE;	break;
		case COLOR_TEST_ANY_COLORED & ~(COLOR_TEST_GREEN|COLOR_TEST_BLUE):	wav1 = WAV_WHITE;	wav2 = WAV_BLACKRED;	break;
		case COLOR_TEST_ANY_COLORED & ~(COLOR_TEST_BLUE|COLOR_TEST_RED):	wav1 = WAV_BLACK;	wav2 = WAV_GREENWHITE;	break;
		case COLOR_TEST_ANY_COLORED & ~(COLOR_TEST_RED|COLOR_TEST_WHITE):	wav1 = WAV_GREEN;	wav2 = WAV_BLUEBLACK;	break;


		// four colors - slightly less of a hack.
		// (While there aren't any naturally four-color lands that I know of, these can still be played by Reflecting Pool, Exotic Orchard, etc.)
		case COLOR_TEST_ANY_COLORED & ~COLOR_TEST_BLACK:	wav1 = WAV_REDGREEN;	wav2 = WAV_WHITEBLUE;	break;
		case COLOR_TEST_ANY_COLORED & ~COLOR_TEST_BLUE:		wav1 = WAV_BLACKRED;	wav2 = WAV_GREENWHITE;	break;
		case COLOR_TEST_ANY_COLORED & ~COLOR_TEST_GREEN:	wav1 = WAV_WHITEBLUE;	wav2 = WAV_BLACKRED;	break;
		case COLOR_TEST_ANY_COLORED & ~COLOR_TEST_RED:		wav1 = WAV_GREENWHITE;	wav2 = WAV_BLUEBLACK;	break;
		case COLOR_TEST_ANY_COLORED & ~COLOR_TEST_WHITE:	wav1 = WAV_BLUEBLACK;	wav2 = WAV_REDGREEN;	break;

		// five colors - going with gembazar, even though City of Brass is silent.  (It's better than polkamix?)
		case COLOR_TEST_ANY_COLORED:			wav1 = WAV_GEMBAZAR;	break;
	}

	if (wav1 != (wav_t)-1){
		play_sound_effect(wav1);
	}
	if (wav2 != (wav_t)-1){
		play_sound_effect(wav2);
	}
}

void for_each_creature_damaged_by_me(int player, int card, event_t event, int flags,
                                     void (*fn)(int, int, int, int), int arg1, int arg2)
{
  card_instance_t *instance = get_card_instance(player, card);
  if (event == EVENT_DEAL_DAMAGE)
  {
    card_instance_t *dmg = get_card_instance(affected_card_controller, affected_card);
    if (dmg->internal_card_id == damage_card && dmg->damage_source_player == player &&
        (dmg->damage_source_card == card ||
         is_multiblocker_card_copying(player, dmg->damage_source_card, player, card)))
    {
      if (dmg->info_slot > 0 ||
          get_card_instance(dmg->damage_source_player, dmg->damage_source_card)
                  ->targets[16]
                  .player > 0)
      {
        if (dmg->damage_target_card > -1)
        {
          int good = 1;
          if ((flags & DDBM_MUST_BE_COMBAT_DAMAGE) &&
              !(dmg->token_status & (STATUS_COMBAT_DAMAGE | STATUS_FIRST_STRIKE_DAMAGE)))
          {
            good = 0;
          }
          if (good)
          {
            int pos = instance->info_slot;
            if (pos < 10)
            {
              instance->targets[pos].player = dmg->damage_target_player;
              instance->targets[pos].card = dmg->damage_target_card;
              instance->info_slot++;
            }
          }
        }
      }
    }
  }
  if (instance->info_slot > 0 && trigger_condition == TRIGGER_DEAL_DAMAGE &&
      affect_me(player, card) && reason_for_trigger_controller == player)
  {
    if (event == EVENT_TRIGGER)
    {
      event_result |= RESOLVE_TRIGGER_MANDATORY;
    }
    else if (event == EVENT_RESOLVE_TRIGGER)
    {
      int i;
      for (i = 0; i < instance->info_slot; i++)
      {
        if (in_play(instance->targets[i].player, instance->targets[i].card))
        {
          fn(arg1, arg2, instance->targets[i].player, instance->targets[i].card);
        }
      }
      instance->info_slot = 0;
    }
  }
}

unsigned int round_down_value(unsigned int orig_value){
	unsigned int amount = orig_value/2;
	if( amount*2 != orig_value ){
		amount = (orig_value-1)/2;
	}
	return amount;
}

unsigned int round_up_value(unsigned int orig_value){
	unsigned int amount = orig_value/2;
	if( amount*2 != orig_value ){
		amount = (orig_value+1)/2;
	}
	return amount;
}

void no_more_than_x_creatures_can_attack(int player, int card, event_t event, int t_player,
                                         int max_value)
{
  if (event == EVENT_ATTACK_LEGALITY &&
      (affected_card_controller == t_player || t_player == ANYBODY))
  {
    card_instance_t *aff = get_card_instance(affected_card_controller, affected_card);
    if (aff->state & STATE_ATTACKING)
    { // Already attacking
      return;
    }

    int num_attacking = 0;
    int c;
    for (c = 0; c < active_cards_count[affected_card_controller]; ++c)
    {
      if ((aff = in_play(affected_card_controller, c)) &&
          is_what(affected_card_controller, c, TYPE_CREATURE) && (aff->state & STATE_ATTACKING) &&
          ++num_attacking >= max_value)
      {
        event_result = 1;
        break;
      }
    }
  }
}

void no_more_than_x_creatures_can_block(int player, int card, event_t event, int t_player, int max_value){
	if( current_turn != t_player || t_player == ANYBODY ){
		card_instance_t* aff = get_card_instance(affected_card_controller, affected_card);
		if (aff->state & STATE_BLOCKING){	// Already blocking
			return;
		}

		int num_blockers = 0;
		int c;
		for (c = 0; c < active_cards_count[affected_card_controller]; ++c){
			if ((aff = in_play(affected_card_controller, c)) && (aff->state & STATE_BLOCKING) && ++num_blockers >= max_value){
				event_result = 1;
				break;
			}
		}
	}
}

int choose_a_color_and_show_legacy(int player, int card, int ai_t_player, int ret_location){
	//	return chosen color
	//	store the legacy in "targets[ret_location].card" if "ret_location" > -1.
	int clr = choose_a_color(player, get_deck_color(player, ai_t_player));
	int legacy = create_card_name_legacy(player, card, CARD_ID_BLACK+(clr-1));
	if( ret_location > -1 ){
		get_card_instance(player, card)->targets[ret_location].card = legacy;
	}
	return clr;
}

static int discard_trigger_delayed_ability(int player, int card, event_t event)
{
  if (eot_trigger(player, card, event))
  {
    card_instance_t *instance = get_card_instance(player, card);
    int p = instance->targets[0].player;
    int csvid = instance->targets[0].card;
    int mode = instance->targets[1].player;
    if (p > -1 && csvid > -1)
    {
      int count = count_graveyard(p) - 1;
      while (count > -1)
      {
        if (cards_data[get_grave(p)[count]].id == csvid)
        {
          if (mode & 1)
          {
            add_card_to_hand(p, get_grave(p)[count]);
            remove_card_from_grave(p, count);
          }
          if (mode & 2)
          {
            reanimate_permanent(p, -1, p, count, REANIMATE_PLUS1_PLUS1_COUNTER);
          }
          break;
        }
        count--;
      }
    }
    kill_card(player, card, KILL_EXILE);
  }
  return 0;
}

int card_madness_effect(int player, int card, event_t event){
	//Madness ruling changed, so this is no more used.

	int iid = get_card_instance(player, card)->targets[1].card;

	if( event == EVENT_RESOLVE_SPELL ){
		card_instance_t *instance = get_card_instance(player, card);
		instance->targets[1].player = current_phase;
	}

	if( get_card_instance(player, card)->targets[1].player > -1 ){
		card_instance_t *instance = get_card_instance(player, card);

		if( current_phase != instance->targets[1].player ){
			if( iid > -1 ){
				int pos = find_iid_in_rfg(player, iid);
				if( pos > -1 ){
					from_exile_to_graveyard(player, pos);
				}
			}
			kill_card(player, card, KILL_SACRIFICE);
		}

		if( trigger_condition == TRIGGER_SPELL_CAST && affect_me(player, card) && reason_for_trigger_controller == player &&
		    instance->targets[2].card != 66 )
		{

			int trig = 0;

			if( ! is_what(trigger_cause_controller, trigger_cause, TYPE_LAND) ){
				trig = 1;
			}


			if( trig == 1 ){
				if( event == EVENT_TRIGGER){
					event_result |= RESOLVE_TRIGGER_MANDATORY;
				}
				else if( event == EVENT_RESOLVE_TRIGGER ){
						if( iid > -1 ){
							int pos = find_iid_in_rfg(player, iid);
							if( pos > -1 ){
								from_exile_to_graveyard(player, pos);
							}
						}
						kill_card(player, card, KILL_SACRIFICE);
				}
			}
		}

		if( event == EVENT_CAN_ACTIVATE ){
			int result = 0;
			if( iid > -1 && instance->targets[2].card != 66 ){
				int fake = add_card_to_hand(player, iid);
				add_state(player, fake, STATE_INVISIBLE);
				hand_count[player]--;
				if( call_card_function(player, fake, EVENT_CAN_PAY_MADNESS_COST) ){
					if( is_what(player, fake, TYPE_PERMANENT) ){
						result = 1;
					}
					else{
						result = call_card_function(player, fake, EVENT_CAN_CAST);
					}
				}
				obliterate_card(player, fake);
			}
			return result;
		}

		if( event == EVENT_ACTIVATE ){
			if( iid > -1 ){
				int fake = add_card_to_hand(player, iid);
				add_state(player, fake, STATE_INVISIBLE);
				hand_count[player]--;
				if( call_card_function(player, fake, EVENT_PAY_MADNESS_COST) ){
					instance->targets[2].card = 66;
					remove_state(player, fake, STATE_INVISIBLE);
					play_card_in_hand_for_free(player, fake);
					cant_be_responded_to = 1;
				}
				else{
					obliterate_card(player, fake);
				}
			}
		}

		if( event == EVENT_RESOLVE_ACTIVATION ){
			kill_card(instance->parent_controller, instance->parent_card, KILL_SACRIFICE);
		}
	}

	return 0;
}

int madness(int player, int card, event_t event, int colorless, int black, int blue, int green,
            int red, int white)
{
  if (event == EVENT_CAN_PAY_MADNESS_COST)
  {
    int adjustment = MAX(0, true_get_updated_casting_cost(player, card, -1, EVENT_CAN_CAST, 0));
    // Preserve the native query: the adjusted sum is tested, but only the original cost is used.
    int cless = adjustment + colorless >= 0 ? MAX(0, colorless) : 0;
    if (!has_mana_multi(player, cless, black, blue, green, red, white))
      return 0;
    if (is_what(player, card, TYPE_CREATURE | TYPE_ARTIFACT | TYPE_LAND))
      return 1;
    if (is_what(player, card, TYPE_ENCHANTMENT) && !has_subtype(player, card, SUBTYPE_AURA))
      return 1;
    return call_card_function(player, card, EVENT_CAN_CAST);
  }
  if (event == EVENT_PAY_MADNESS_COST)
  {
    int penalty =
        MAX(0, true_get_updated_casting_cost(player, card, -1, EVENT_CAN_CAST, colorless)) -
        MAX(0, colorless);
    if (penalty > 0 && colorless < 0)
    {
      charge_mana_multi(player, penalty, black, blue, green, red, white);
      if (spell_fizzled == 1)
        return 0;
      charge_mana(player, COLOR_ANY, colorless);
    }
    else
    {
      charge_mana_multi(player, colorless + (penalty > 0 ? penalty : 0), black, blue, green, red,
                        white);
    }
    if (spell_fizzled != 1)
    {
      if (colorless == -1)
        set_x_for_x_spells(player, card, event, x_value);
      set_special_flags(player, card, SF3_MADNESS_COST_PAID);
      return 1;
    }
  }
  return 0;
}

void discard_card_impl(int player, int card, int player_who_controls_effect, int flags)
{
  if (player != player_who_controls_effect)
    for (int c = 0; c < active_cards_count[player]; ++c)
      if (in_play(player, c) && get_id(player, c) == 0x4e30 && !is_humiliated(player, c, AL_STATIC))
        return;

  int v2; // ST14_4
  int v3; // ST10_4

  v2 = trigger_cause_controller;
  v3 = trigger_cause;
  trigger_cause_controller = player;
  trigger_cause = card;
  byte_786DD4 = 0;
  dispatch_trigger_twice_once_with_each_player_as_reason(player, TRIGGER_DISCARD, str_Draw_a_card,
                                                         0);
  trigger_cause = v3;
  trigger_cause_controller = v2;
  if (byte_786DD4 != 1)
  {
    // Begin addition
    int csvid = get_id(player, card);
    int quagnoth_flag = 0;
    int madness_flag = 0;
    int falkenrath_gorger_flag = check_battlefield_for_id(player, CARD_ID_FALKENRATH_GORGER);
    if (cards_data[get_card_instance(player, card)->internal_card_id].cc[2] == 11 ||
        csvid == CARD_ID_ICHOR_SLICK ||
        (falkenrath_gorger_flag && has_subtype(player, card, SUBTYPE_VAMPIRE)))
    {
      /* "Madness - If you discard this card, discard it into exile.
       * When you do, cast it for its madness cost or put it into your graveyard."
       * We're just faking it, until some cards with "when THIS is put into exile" appears.
       */
      enum
      {
        MADNESS_MODE_STANDARD = 1,
        MADNESS_MODE_FALKENRATH_GORGER
      };

      int mode = MADNESS_MODE_STANDARD;
      int options[2] = {call_card_function(player, card, EVENT_CAN_PAY_MADNESS_COST),
                        (falkenrath_gorger_flag && has_subtype(player, card, SUBTYPE_VAMPIRE) &&
                         has_mana_to_cast_iid(player, EVENT_CAN_CAST,
                                              get_original_internal_card_id(player, card)))};
      mode = DIALOG(player, card, EVENT_CAST_SPELL, DLG_NO_STORAGE, DLG_NO_CANCEL, DLG_OMIT_ILLEGAL,
                    "Pay Madness cost", options[0], 10, "Use Falkenrath Goger's Madness",
                    options[1], 10, "Decline", 1, 1);

      if (mode == MADNESS_MODE_STANDARD)
      {
        if (call_card_function(player, card, EVENT_PAY_MADNESS_COST))
        {
          madness_flag = 1;
          play_card_in_hand_madness(player, card);
        }
      }
      if (mode == MADNESS_MODE_FALKENRATH_GORGER)
      {
        if (charge_mana_from_id(player, -1, EVENT_CAST_SPELL, get_id(player, card)))
        {
          madness_flag = 1;
          play_card_in_hand_madness(player, card);
        }
      }
    }
    if (player != player_who_controls_effect)
    {
      if (check_battlefield_for_id(player, CARD_ID_NEPHALIA_ACADEMY) &&
          !is_humiliated(player, card, AL_STATIC))
      {
        /* Nephalia Academy
         * If a spell or ability an opponent controls causes you to discard a card,
         * you may reveal that card and put it on top of your library instead of putting it anywhere
         * else.*/
        int na_effect = DIALOG(player, card, EVENT_CAST_SPELL, DLG_NO_STORAGE, DLG_NO_CANCEL,
                               "Use Nephalia Academy ability", 1, 1, "Decline", 1, 1);
        if (na_effect == 1)
        {
          reveal_card(player, card, player, card);
          put_on_top_of_deck(player, card);
          return;
        }
      }

      switch (csvid)
      {
      case CARD_ID_DODECAPOD: // Replacement: it goes directly from hand to play
      {
        add_counters(player, card, player, card, COUNTER_P1_P1, 2);
        put_into_play(player, card);
      }
        return;

      case CARD_ID_LOXODON_SMITER:   // Replacement: it goes directly from hand to play
      case CARD_ID_OBSTINATE_BALOTH: // Replacement: it goes directly from hand to play
      case CARD_ID_NULLHIDE_FEROX:
      case CARD_ID_WILT_LEAF_LIEGE: // Replacement: it goes directly from hand to play
      {
        put_into_play(player, card);
        return;
      }

      case CARD_ID_GUERRILLA_TACTICS:
      {
        target_definition_t td;
        default_target_definition(player, card, &td, TYPE_CREATURE);
        td.zone = TARGET_ZONE_CREATURE_OR_PLAYER;
        td.allow_cancel = 0;

        card_instance_t *instance = get_card_instance(player, card);

        if (can_target(&td) && pick_target(&td, "TARGET_CREATURE_OR_PLAYER"))
        {
          damage_creature(instance->targets[0].player, instance->targets[0].card, 4, player, card);
        }
        break;
      }

      case CARD_ID_MANGARAS_BLESSING:
      {
        gain_life(player, 2);
        int legacy = create_legacy_effect(player, card, &discard_trigger_delayed_ability);
        card_instance_t *instance = get_card_instance(player, legacy);
        instance->targets[0].player = player;
        instance->targets[0].card = csvid;
        instance->targets[1].player = 1; // Return to hand at eot.
      }
      break;

      case CARD_ID_METROGNOME:
      {
        token_generation_t token;
        default_token_definition(player, card, CARD_ID_GNOME, &token);
        token.qty = 4;
        token.pow = token.tou = 1;
        generate_token(&token);
      }
      break;

      case CARD_ID_PSYCHIC_PURGE:
        lose_life(player_who_controls_effect, 5);
        break;

      case CARD_ID_PURE_INTENTIONS:
      {
        int legacy = create_legacy_effect(player, card, &discard_trigger_delayed_ability);
        card_instance_t *instance = get_card_instance(player, legacy);
        instance->targets[0].player = player;
        instance->targets[0].card = csvid;
        instance->targets[1].player = 1; // Return to hand at eot.
      }
      break;

      case CARD_ID_SAND_GOLEM:
      {
        int legacy = create_legacy_effect(player, card, &discard_trigger_delayed_ability);
        card_instance_t *instance = get_card_instance(player, legacy);
        instance->targets[0].player = player;
        instance->targets[0].card = csvid;
        instance->targets[1].player = 2; // Return to play with a +1/+1 counter at eot.
      }
      break;

      case CARD_ID_QUAGNOTH:
        quagnoth_flag = 1;
        break;

      default:
        break;
      }
    }
    // End addition

    dispatch_event_to_single_card(player, card, EVENT_DISCARD, 1 - player, -1);
    if (!madness_flag)
    { // If Madness was paid, the card is no more in hand, so callig this block will surely screw
      // things up.
      put_card_in_graveyard(player, card);
      get_card_instance(player, card)->internal_card_id = -1;
      call_sub_437E20_unless_ai_is_speculating();
      play_sound_effect(WAV_DISCARD);
      --hand_count[player];
    }

    // Begin addition
    if (quagnoth_flag)
    {
      seek_grave_for_id_to_reanimate(player, -1, player, csvid, REANIMATEXTRA_RETURN_TO_HAND2);
    }
    // End addition
  }
}

void discard_card(int player, int card){
	discard_card_impl(player, card, player, 0);
}

void new_discard_card(int player, int card, int player_who_controls_effect, int flags){
	discard_card_impl(player, card, player_who_controls_effect, flags);
}

void discard(int player, int flags, int player_who_controls_effect)
{
  if (player != player_who_controls_effect)
    for (int c = 0; c < active_cards_count[player]; ++c)
      if (in_play(player, c) && get_id(player, c) == 0x4e30 && !is_humiliated(player, c, AL_STATIC))
        return;

  if (hand_count[player] < 1 ||
      (player == AI && !(trace_mode & 2) && dword_60A4B0 + hand_count[AI] < 1))
    return;

  int random = flags & DISC_RANDOM;
  int card = -1;
  if ((player && !(trace_mode & 2)) || ai_is_speculating == 1 || random)
  {
    if ((trace_mode & 2) && player == AI)
    {
      receive_network_packet(AI, 20);
      card = dblword_628C10;
    }
    else
    {
      int found = 0;
      for (int tries = 0; tries < 999; ++tries)
      {
        card = internal_rand(active_cards_count[player]);
        card_instance_t *instance = get_card_instance(player, card);
        if (instance->internal_card_id != -1 &&
            !(instance->state & (STATE_CANNOT_TARGET | STATE_INVISIBLE | STATE_IN_PLAY)))
        {
          found = 1;
          break;
        }
      }
      if (!found)
        for (int c = 0; c < active_cards_count[player]; ++c)
        {
          card_instance_t *instance = get_card_instance(player, c);
          if (instance->internal_card_id != -1 &&
              !(instance->state & (STATE_CANNOT_TARGET | STATE_INVISIBLE | STATE_IN_PLAY)))
          {
            found = 1;
            card = c;
            break;
          }
        }
      if ((trace_mode & 2) && player == HUMAN)
      {
        if (!found)
          card = -1;
        dblword_628C10 = card;
        byte_628C0C = 20;
        send_network_packet(HUMAN, 20);
      }
    }
  }
  else
  {
    test_definition_t test;
    new_default_test_definition(&test, TYPE_ANY, "Select a card to discard.");
    test.state = STATE_CANNOT_TARGET;
    test.state_flag = DOESNT_MATCH;
    card = new_select_a_card(player, player, TUTOR_FROM_HAND, !(flags & DISC_OPTIONAL),
                             AI_MIN_VALUE, -1, &test);
    if ((flags & DISC_OPTIONAL) && card < 0)
    {
      spell_fizzled = 1;
      return;
    }
  }

  if (!(trace_mode & 2) && player == AI && ai_is_speculating != 1)
  {
    load_text(0, "PROMPT_DISCARDACARD");
    do_dialog(AI, AI, card, -1, -1, text_lines[random ? 1 : 2], 0);
  }
  if (flags & DISC_JUST_MARK)
    get_card_instance(player, card)->state |= STATE_CANNOT_TARGET;
  else
    discard_card_impl(player, card, player_who_controls_effect, flags);
}

void new_multidiscard(int player, int quantity, int flags, int player_who_controls_effect)
{
  if ((flags & DISC_OPTIONAL) && quantity > 1)
    flags |= DISC_JUST_MARK;
  int marked = flags & DISC_JUST_MARK;
  while (quantity)
  {
    if (spell_fizzled == 1)
      break;
    discard(player, flags, player_who_controls_effect);
    --quantity;
  }

  if (marked)
  {
    if (flags & DISC_OPTIONAL)
      flags &= ~DISC_JUST_MARK;
    for (int c = 0; c < active_cards_count[player]; ++c)
    {
      card_instance_t *instance = in_hand(player, c);
      if (instance && (instance->state & STATE_CANNOT_TARGET))
      {
        if (spell_fizzled == 1)
          instance->state &= ~STATE_CANNOT_TARGET;
        else
          discard_card_impl(player, c, player_who_controls_effect, flags);
      }
    }
  }
}

void multidiscard(int player, int quantity, int random){
	new_multidiscard(player, quantity, random ? DISC_RANDOM : 0, player);
}

void new_discard_all(int player, int player_who_controls_effect){
	int p;
	for (p = 0; p < 2; p++){
		if (p == player || player == 2){
			while (hand_count[p] > 0){
				if (actually_has_a_card_in_hand(p)){
					discard(p, 0, player_who_controls_effect);
				} else {
					if (ai_is_speculating != 1){
						popup("discard_all()", "Couldn't find card in hand for %s player\nhand_count[%d] still %d", p == 0 ? "human" : "ai", p, hand_count[p]);
					}
					hand_count[p] = 0;
				}
			}
		}
	}
}

void discard_all(int player){
	new_discard_all(player, player);
}

// Returns the first activation card currently on the stack controlled by player that was activated for {player,card}, or NULL if not found.


// Resets {player,card}'s timestamp to now.
void new_timestamp(int player, int card)
{
  EXE_FN(void, 0x4373b0, int, int)(player, card);	// set_timestamp()
  EXE_FN(void, 0x437400, void)();	// compact_timestamps()
}

// Uses {player, card}->targets[11].player.
int
whenever_a_player_sacrifices_a_permanent(int player, int card, event_t event, int who_sacs, type_t typ, resolve_trigger_t mode)
{
  card_instance_t* inst, *aff;
  if (event == EVENT_GRAVEYARD_FROM_PLAY
	  && (affected_card_controller == who_sacs || who_sacs == ANYBODY)
	  && ((aff = in_play(affected_card_controller, affected_card)))
	  && is_what(affected_card_controller, affected_card, typ)
	  && aff->kill_code == KILL_SACRIFICE && !check_special_flags(affected_card_controller, affected_card, SF_KILL_STATE_BASED_ACTION)
	  && (inst = in_play(player, card)) && !is_humiliated(player, card, AL_TRIG))
	{
	  if (inst->targets[11].player < 0)
		inst->targets[11].player = 0;
	  ++inst->targets[11].player;
	}

  if (trigger_condition == TRIGGER_GRAVEYARD_FROM_PLAY && affect_me(player, card) && player == reason_for_trigger_controller
	  && (inst = in_play(player, card)) && inst->targets[11].player > 0)
	{
	  if (event == EVENT_TRIGGER)
		event_result |= mode;
	  else if (event == EVENT_RESOLVE_TRIGGER)
		{
		  if (--inst->targets[11].player > 0)
			inst->state &= ~STATE_PROCESSING;	// trigger again
		  return 1;
		}
	  else if (event == EVENT_END_TRIGGER)
		inst->targets[11].player = 0;
	}

  return 0;
}

static int find_target(card_instance_t* inst, int player, int card)
{
  int i;
  for (i = inst->number_of_targets - 1; i >= 0; --i)
    if (inst->targets[i].card == card && inst->targets[i].player == player)
      break;
  return i;
}
static int fx_permanents_destroyed_this_way_accumulate_condition_lambda(int player, int card)
{
  if (BYTE1(get_card_instance(player, card)->eot_toughness) & 2)	// Already mid-resolution
	return 0;

  if (card == trigger_cause && player == trigger_cause_controller)	// This effect!
	return 0;

  card_instance_t* ins = get_card_instance(player, card);
  if (ins->number_of_targets == 0)	// no targets left in play
	return 1;

  card_instance_t* tgt;
  int i;
  for (i = 0; i < ins->number_of_targets; ++i)
	if ((tgt = in_play(ins->targets[i].player, ins->targets[i].card))	// a target that's still in play
		&& (tgt->token_status & STATUS_DYING)	// and dying
		&& tgt->kill_code == KILL_DESTROY)	// due to destruction
	  return 0;

  return 1;
}
static int fx_permanents_destroyed_this_way_accumulate_triggering(int player, int card, event_t event)
{
  if (trigger_condition == TRIGGER_GRAVEYARD_FROM_PLAY && affect_me(player, card) && player == reason_for_trigger_controller
	  && fx_permanents_destroyed_this_way_accumulate_condition_lambda(player, card))
	{
	  if (event == EVENT_TRIGGER)
		event_result |= RESOLVE_TRIGGER_MANDATORY;
	  if (event == EVENT_RESOLVE_TRIGGER)
		return 1;
	}
  return 0;
}

/* Call once for each target.  previous_effect should be NULL on the first call, else the return value of the previous call to this function.  When the last
 * target is destroyed (not counting ones that stop being destroyed for whatever reason), then the original card's function will be sent
 * EVENT_RESOLVE_GENERAL_EFFECT, with player/card set to one of the effect cards created.  The number of targets that were destroyed will be in
 * HIWORD(effect->eot_toughness).  The number of those that were controlled by player 0 will be in LOWORD(effect->targets[10].player), and the number that were
 * controlled by player 1 will be in HIWORD(effect->targets[10].player). */














void life_sucking(int player, int card, int amount){
	target_definition_t td;
	default_target_definition(player, card, &td, 0);
	td.zone = TARGET_ZONE_PLAYERS;
	td.allow_cancel = 0;

	int t_player = 1-player;

	if( ! would_validate_arbitrary_target(&td, t_player, -1) ){
		t_player = player;
		if( ! would_validate_arbitrary_target(&td, t_player, -1) ){
			return;
		}
	}

	lose_life(t_player, amount);
	gain_life(player, amount);
}















// Displays msg in the small horizontal window and sleeps for 2 seconds.
void display_error_message(const char* msg){
  set_centerwindow_txt(msg);
  Sleep(2000);
  set_centerwindow_txt("");
}

int you_may_spend_mana_as_though_it_were_mana_of_any_color_to_play_spell(int player, int card, event_t event, int type, 
	test_definition_t *test)
{
// Cards that use this function should have the "flags:activate" set to "1" in "ct_all.csv"

	if( event == EVENT_CAN_ACTIVATE ){
		int i;
		for(i=0; i<active_cards_count[player]; i++){
			if( in_hand(player, i) && 
				((type && is_what(player, i, type)) || (test != NULL && new_make_test_in_play(player, i, -1, test))) ) 
			{
				int new_cmc = get_updated_casting_cost(player, card, -1, event, get_cmc(player, i));
				if( has_mana(player, COLOR_ANY, new_cmc) ){
					int tr = can_legally_play_card_in_hand(player, i);
					if( tr ){
						if( tr == 99 ){
							return 99;
						}
						if( can_sorcery_be_played(player, event) ||
							(cards_data[get_original_internal_card_id(player, i)].type & (TYPE_INSTANT | TYPE_INTERRUPT)) )
						{
							return 1;
						}
					}
				}
			}
		}
	}

	if( event == EVENT_ACTIVATE ){
		int playable[2][hand_count[player]];
		int pc = 0;

		int i;
		for(i=0; i<active_cards_count[player]; i++){
			if( in_hand(player, i) && 
				((type && is_what(player, i, type)) || (test != NULL && new_make_test_in_play(player, i, -1, test))) ) 
			{
				int new_cmc = get_updated_casting_cost(player, card, -1, event, get_cmc(player, i));
				if( has_mana(player, COLOR_ANY, new_cmc) ){
					if( can_legally_play_card_in_hand(player, i) ){
						playable[0][pc] = get_card_instance(player, i)->internal_card_id;
						playable[1][pc] = i;
						pc++;
					}
				}
			}
		}

		test_definition_t this_test;
		new_default_test_definition(&this_test, TYPE_ANY, "Select a card to play.");

		int selected = select_card_from_zone(player, player, playable[0], pc, 0, AI_MAX_VALUE, -1, &this_test);
		if( selected != -1 ){
			int new_cmc = get_updated_casting_cost(player, card, -1, event, get_cmc(player, playable[1][selected]));
			if( charge_mana(player, COLOR_ANY, new_cmc) ){
				play_card_in_hand_for_free(player, playable[1][selected]);
				cant_be_responded_to = 1;	// The spell will be respondable to, but this (fake) activation won't
			}
			else{
				spell_fizzled = 1;
				return 0;
			}
		}
		else{
			spell_fizzled = 1;
			return 0;
		}
	}

	return vanguard_card(player, card, event, 7, 20, 11);
}

int is_this_dying(int player, int card){ 
//AKA will go into a graveyard from play, this should be checked only during EVENT_GRAVEYARD_FROM_PLAY to be realiable
	if( in_play(player, card) ){ 
		card_instance_t *instance = get_card_instance(player, card);
		if( instance->kill_code >= KILL_BURY && instance->kill_code < KILL_EXILE ){
			if( ! check_special_flags2(player, card, SF2_WILL_BE_EXILED_IF_PUTTED_IN_GRAVE) ){
				return 1;
			}
		}
	}
	return 0;
}



int reanimate_csvids_at_eot_legacy(int player, int card, event_t event)
{
  if (eot_trigger(player, card, event))
  {
    card_instance_t *instance = get_card_instance(player, card);
    for (int i = 0; i < 11; ++i)
    {
      if (instance->targets[i].player >= 0 &&
          !grafdiggers_cages(
              player,
              get_type_with_iid(player, card,
                                get_internal_card_id_from_csv_id(instance->targets[i].player)),
              TUTOR_FROM_GRAVE))
        seek_grave_for_id_to_reanimate(player, card, player, instance->targets[i].player,
                                       REANIMATE_DEFAULT);
      if (instance->targets[i].card >= 0 &&
          !grafdiggers_cages(
              player,
              get_type_with_iid(player, card,
                                get_internal_card_id_from_csv_id(instance->targets[i].card)),
              TUTOR_FROM_GRAVE))
        seek_grave_for_id_to_reanimate(player, card, player, instance->targets[i].card,
                                       REANIMATE_DEFAULT);
    }
    kill_card(player, card, KILL_EXILE);
  }
  return 0;
}





int whenever_i_deal_damage_to_a_player(int player, int card, int event, ddbm_flags_t mode)
{
	int result = 0;
	if (damage_dealt_by_me(player, card, event, DDBM_MUST_DAMAGE_PLAYER | DDBM_TRACE_DAMAGED_PLAYERS | mode))
	{
		card_instance_t* instance = get_card_instance(player, card);
		SET_BYTE0(result) = BYTE0(instance->targets[1].player);
		SET_BYTE1(result) = BYTE1(instance->targets[1].player);
		instance->targets[1].player = 0;
		return result;
	}
	return 0;
}







int whenever_a_player_plays_a_land(int player, int card, event_t event, int t_player, int trig_mode){

	if( trigger_condition == TRIGGER_SPELL_CAST || trigger_condition == TRIGGER_COMES_INTO_PLAY ){
		int trig = 0;
		// The normal case, a land played from hand
		if( trigger_condition == TRIGGER_SPELL_CAST && affect_me(player, card) && player == reason_for_trigger_controller ){
			if( is_what(trigger_cause_controller, trigger_cause, TYPE_LAND) ){
				if( t_player == ANYBODY || t_player == trigger_cause_controller ){
					trig = 1;
				}
			}
		}
		// We're emulating a land played from hand (see Crucible of Worlds)
		if( trigger_condition == TRIGGER_COMES_INTO_PLAY && affect_me(player, card) && player == reason_for_trigger_controller ){
			if( is_what(trigger_cause_controller, trigger_cause, TYPE_LAND) && 
				check_special_flags3(trigger_cause_controller, trigger_cause, SF3_EMULATING_LANDS_PLAYED) )
			{
				if( t_player == ANYBODY || t_player == trigger_cause_controller ){
					trig = 1;
				}
			}
		}
		if( trig ){
			if( trig_mode == RESOLVE_TRIGGER_DUH ){
				trig_mode = duh_mode(player) ? RESOLVE_TRIGGER_MANDATORY : RESOLVE_TRIGGER_OPTIONAL;
			}
			if(event == EVENT_TRIGGER){
				event_result |= trig_mode;
			}
			else if(event == EVENT_RESOLVE_TRIGGER){
					return 1;
			}
		}
	}
	return 0;
}

int spell_puts_up_to_x_target_cards_from_graveyard_on_top_of_deck(int player, int card,
                                                                  event_t event, int t_player,
                                                                  int max_targets,
                                                                  test_definition_t *test,
                                                                  int draw_else_slowtrip)
{

  card_instance_t *instance = get_card_instance(player, card);

  if (event == EVENT_CAN_CAST || event == EVENT_CAN_CHANGE_TARGET)
  {
    if (t_player == player)
    {
      return generic_spell(player, card, event, GS_GRAVE_RECYCLER, NULL, NULL, 1, test) >= 0;
    }
    if (t_player == 1 - player)
    {
      return generic_spell(player, card, event, GS_GRAVE_RECYCLER_OPP_GRAVE, NULL, NULL, 1, test) >=
             0;
    }
    if (t_player == ANYBODY)
    {
      return generic_spell(player, card, event, GS_GRAVE_RECYCLER_BOTH_GRAVES, NULL, NULL, 1,
                           test) >= 0;
    }
  }

  if ((event == EVENT_CAST_SPELL && affect_me(player, card)) || event == EVENT_CHANGE_TARGET)
  {
    if (t_player == ANYBODY)
    {
      target_definition_t td;
      default_target_definition(player, card, &td, 0);
      td.zone = TARGET_ZONE_PLAYERS;
      if (!new_pick_target(&td, "Select a graveyard owner", 0, 1 | GS_LITERAL_PROMPT))
      {
        return 0;
      }
      t_player = instance->targets[0].player;
      instance->number_of_targets = 0;
    }
    int pos = t_player == ANYBODY ? 1 : 0;
    int max_targ = MIN(count_graveyard_by_type(t_player, TYPE_ANY), max_targets);
    max_targ = MIN(max_targ, 10);
    int result = select_multiple_cards_from_graveyard(
        player, t_player, 0, t_player == IS_AI(player) ? AI_MAX_VALUE : AI_MIN_VALUE, test,
        max_targ, &instance->targets[pos]);
    instance->info_slot = result + pos;
  }

  if (event == EVENT_RESOLVE_SPELL)
  {
    int pos = t_player == ANYBODY ? 1 : 0;
    int validating_player = t_player == ANYBODY ? instance->targets[0].player : t_player;
    int i, num_validated = 0, selected, num_targets = 0;
    for (i = pos; i < instance->info_slot; ++i)
    {
      ++num_targets;
      if ((selected = validate_target_from_grave(player, card, validating_player, i)) != -1)
      {
        int card_added =
            add_card_to_hand(validating_player, get_grave(validating_player)[selected]);
        remove_card_from_grave(validating_player, selected);
        put_on_top_of_deck(validating_player, card_added);
        ++num_validated;
      }
    }

    if (num_validated > 1)
    {
      rearrange_top_x(t_player, player, num_validated);
    }
    if (num_validated == 0 && num_targets > 0)
    {
      spell_fizzled = 1;
    }
    else
    {
      if (draw_else_slowtrip == 1)
      {
        draw_a_card(player);
      }
      else if (draw_else_slowtrip == 2)
      {
        cantrip(player, card, 1);
      }
    }

    kill_card(player, card, KILL_DESTROY);
  }

  return 0;
}









int reanimate_test_from_any_graveyard(int player, int card, test_definition_t *test, int targeted, reanimate_mode_t mode){
	if( (new_special_count_grave(player, test) && (! targeted || ! graveyard_has_shroud(player))) || 
		(new_special_count_grave(1-player, test) && (! targeted || ! graveyard_has_shroud(1-player))) )
	{
		card_instance_t *instance = get_card_instance(player, card);
		instance->targets[0].player = 1-player;
		if( new_special_count_grave(1-player, test) && (! targeted || ! graveyard_has_shroud(1-player)) ){
			if( new_special_count_grave(player, test) && (! targeted || ! graveyard_has_shroud(player)) ){
				target_definition_t td1;
				default_target_definition(player, card, &td1, TYPE_CREATURE);
				td1.zone = TARGET_ZONE_PLAYERS;
				td1.illegal_abilities = 0;
				td1.allow_cancel = 0;
				new_pick_target(&td1, "Select a graveyard owner.", 0, GS_LITERAL_PROMPT);
				instance->number_of_targets = 0;
			}
		}
		else{
			instance->targets[0].player = player;
		}
		int selected = new_select_a_card(player, instance->targets[0].player, TUTOR_FROM_GRAVE, 1, AI_MAX_CMC, -1,test);
		return reanimate_permanent(player, card, instance->targets[0].player, selected, mode);
	}
	return -1;
}





















int x_spell_with_x_targets(int player, int card, event_t event, target_definition_t *td,
                           const char *msg, int additional_flags)
{

  if (event == EVENT_CAN_CAST || event == EVENT_CAN_CHANGE_TARGET)
  {
    return generic_spell(player, card, event, GS_CAN_TARGET | GS_X_SPELL | additional_flags, td,
                         NULL, 1, NULL);
  }

  if ((event == EVENT_CAST_SPELL && affect_me(player, card)) || event == EVENT_CHANGE_TARGET)
  {
    card_instance_t *instance = get_card_instance(player, card);
    int diff = true_get_updated_casting_cost(player, card, -1, event, -1);
    int max = MIN(10, has_mana(player, COLOR_ANY, 1) - diff);
    if (check_special_flags2(player, card, SF2_COPIED_FROM_STACK))
    {
      max = instance->info_slot;
    }
    if (event == EVENT_CHANGE_TARGET)
      td->who_chooses = attacking_card_controller;
    int i = 0;
    while (i < max && can_target(td))
    {
      if (new_pick_target(td, "TARGET_CREATURE", i, (additional_flags & GS_LITERAL_PROMPT)))
      {
        state_untargettable(instance->targets[i].player, instance->targets[i].card, 1);
        i++;
      }
    }
    if (!check_special_flags2(player, card, SF2_COPIED_FROM_STACK))
    {
      instance->info_slot = instance->number_of_targets;
    }
    for (i = 0; i < instance->number_of_targets; i++)
    {
      state_untargettable(instance->targets[i].player, instance->targets[i].card, 0);
    }
    if (!check_special_flags2(player, card, SF2_COPIED_FROM_STACK))
    {
      charge_mana(player, COLOR_ANY, instance->info_slot + diff);
    }
  }

  return 0;
}

void return_cards_exiled_by_me_to_hand(int player, int card){
	int leg = 0;
	int idx = 0;
	int* loc;
	while ((loc = exiledby_find_any(player, card, &leg, &idx)) != NULL){
			int owner = (*loc & 0x80000000) ? 1 : 0;
			int iid = *loc & ~0x80000000;
			*loc = -1;
			if( check_rfg(owner, cards_data[iid].id) ){
				add_card_to_hand(owner, iid);
				remove_card_from_rfg(owner, cards_data[iid].id);
			}
	}
}

void return_cards_exiled_by_me_to_play(int player, int card){
	int leg = 0;
	int idx = 0;
	int* loc;
	while ((loc = exiledby_find_any(player, card, &leg, &idx)) != NULL){
			int owner = (*loc & 0x80000000) ? 1 : 0;
			int iid = *loc & ~0x80000000;
			*loc = -1;
			if( check_rfg(owner, cards_data[iid].id) ){
				int card_added = add_card_to_hand(owner, iid);
				remove_card_from_rfg(owner, cards_data[iid].id);
				put_into_play(owner, card_added);
			}
	}
}

/* Recovered October 2022 implementations; see docs/recovery-progress.md. */
int life_total_can_change(int player);

/* ManalinkEh 2022: 0x268f070; original functions/functions.c:11362. */
int life_total_can_change(int player)
{
  int result;
  unsigned int uVar2;
  result = check_battlefield_for_id(player, 0x2c48);
  if (result != 0)
  {
    return 0;
  }
  uVar2 = check_for_id_legacy(player, 0x43b3);
  return uVar2 >> 0x1f;
}

/* ManalinkEh 2022: 0x26910e0; original functions/functions.c:12271. */
int how_much_life_can_i_pay(int player)
{
  int result;
  result = life_total_can_change(player);
  if (result != 0)
  {
    result = check_battlefield_for_id(2, 0x3010);
    if (result == 0)
    {
      return life[player];
    }
  }
  return 0;
}

/* ManalinkEh 2022: 0x26930f0; original functions/functions.c:15061. */
int cast_counter_triggered_ability(int player, int card, int target_pos)
{
  int iVar1;
  int32_t iVar2;
  card_instance_t *pcVar3;
  card_instance_t *pcVar4;
  unsigned int uVar5;
  if (stack_size < 1)
  {
    spell_fizzled = 1;
    return 0;
  }
  pcVar3 = get_card_instance(player, card);
  iVar1 = stack_size + -2;
  iVar2 = stack_cards[iVar1].card;
  pcVar3->targets[target_pos].player = stack_cards[iVar1].player;
  pcVar3->targets[target_pos].card = iVar2;
  pcVar3->number_of_targets = pcVar3->number_of_targets + '\x01';
  pcVar3->targets[9].player = stack_phase[iVar1];
  pcVar4 = get_card_instance(pcVar3->targets[target_pos].player, pcVar3->targets[target_pos].card);
  uVar5 = pcVar4->internal_card_id;
  if (uVar5 == 0xffffffff)
  {
    pcVar4 =
        get_card_instance(pcVar3->targets[target_pos].player, pcVar3->targets[target_pos].card);
    uVar5 = (unsigned int)pcVar4->backup_internal_card_id;
  }
  pcVar3->targets[9].card = uVar5;
  ai_modifier = ai_modifier + 100;
  return 1;
}

/* ManalinkEh 2022: 0x2677990; original functions/functions.c:15108. */
static int counter_triggered_ability_legacy(int player, int card, event_t event)
{
  card_instance_t *pcVar1;
  card_instance_t *pcVar2;
  int result;

  pcVar1 = get_card_instance(player, card);
  if (0 < stack_size)
  {
    result = 0;
    do
    {
      while (((pcVar2 = get_card_instance(stack_cards[result].player, stack_cards[result].card),
               pcVar2->original_internal_card_id != pcVar1->targets[9].card ||
                   (-1 < (int)pcVar2->token_status)) ||
              (stack_phase[result] != pcVar1->targets[9].player)))
      {
        result = result + 1;
        if (stack_size <= result)
          goto LAB_02677a30;
      }
      result = result + 1;
      pcVar2->state = pcVar2->state | 0x40000000;
      kill_card(player, card, KILL_EXILE);
    } while (result < stack_size);
  }
LAB_02677a30:
  if (event == EVENT_PHASE_CHANGED)
  {
    if (pcVar1->targets[2].player < current_phase)
      goto LAB_02677a48;
  }
  else if (event == EVENT_CAN_SKIP_TURN)
    goto LAB_02677a48;
  if (stack_size != 0)
  {
    return 0;
  }
LAB_02677a48:
  kill_card(player, card, KILL_EXILE);
  return 0;
}

/* ManalinkEh 2022: 0x26835b0; original functions/functions.c:15649. */
static int extra_combat_legacy(int player, int card, event_t event)
{
  int result;
  card_instance_t *instance;
  if (((event == EVENT_PHASE_CHANGED) && (0x1d < current_phase)) && (current_turn == player))
  {
    instance = get_card_instance(player, card);
    if (0x41 < instance->targets[1].player)
    {
      instance = get_card_instance(player, card);
      land_can_be_played = land_can_be_played & 0xfffffff7;
      current_phase = 0x15;
      instance->targets[1].player = instance->targets[1].player + -1;
    }
  }
  result = eot_trigger(player, card, event);
  if (result == 0)
  {
    return 0;
  }
  kill_card(player, card, KILL_EXILE);
  return 0;
}

/* Recovered October 2022 implementations; see docs/recovery-progress.md. */
int deflect_one_target(int player, int card, event_t event, target_definition_t *td);
int deflect_multi_targets(int player, int card, event_t event, target_definition_t *td);

/* ManalinkEh 2022: 0x26774f0; original functions/functions.c:629. */
static card_instance_t *find_obliviation_legacy(int player, int card, int *out_player,
                                                int *out_card, int *out_iid, int *out_owner)
{
  card_instance_t *instance;
  int card_00;
  int player_00;

  player_00 = 0;
  do
  {
    card_00 = 0;
    if (0 < active_cards_count[player_00])
    {
      do
      {
        instance = get_card_instance(player_00, card_00);
        if ((((instance != NULL) && (instance->internal_card_id == LEGACY_EFFECT_ASWAN)) &&
             (instance->damage_target_card == card)) &&
            (((instance->damage_target_player == player && (-1 < instance->targets[7].card)) &&
              (-1 < instance->targets[7].player))))
        {
          if (out_player != NULL)
          {
            *out_player = player_00;
          }
          if (out_card != NULL)
          {
            *out_card = card_00;
          }
          if (out_iid != NULL)
          {
            *out_iid = instance->targets[7].card;
          }
          *out_owner = instance->targets[7].player;
          return instance;
        }
        card_00 = card_00 + 1;
      } while (card_00 < active_cards_count[player_00]);
    }
    player_00 = player_00 + 1;
    if (player_00 == 2)
    {
      return NULL;
    }
  } while (true);
}

/* ManalinkEh 2022: 0x2679a60; original functions/functions.c:691. */
int owner_of_obliviated_card(int player, int card)
{
  int owner;

  owner = -1;
  find_obliviation_legacy(player, card, NULL, NULL, NULL, &owner);
  return owner;
}

/* ManalinkEh 2022: 0x26792c0; original functions/functions.c:7324. */
static int no_combat_damage_this_turn_to(int player, int card, event_t event)
{
  card_instance_t *pcVar1;
  card_instance_t *pcVar2;

  pcVar1 = combat_damage_being_prevented(event);
  if (pcVar1 == NULL)
  {
    if (event == EVENT_CLEANUP)
    {
      kill_card(player, card, KILL_EXILE);
    }
  }
  else
  {
    pcVar2 = get_card_instance(player, card);
    if ((pcVar1->damage_target_card == pcVar2->targets[0].card) &&
        ((int)pcVar1->damage_target_player == pcVar2->targets[0].player))
    {
      pcVar1->info_slot = 0;
    }
  }
  return 0;
}

/* ManalinkEh 2022: 0x26884f0; original functions/functions.c:8825. */
void nobody_can_attack_this_turn_and_next_turn(int player, int card, int t_player)
{
  int legacycard;
  card_instance_t *instance;

  legacycard = create_legacy_effect(player, card, effect_nobody_can_attack_until_eot);
  if (legacycard != -1)
  {
    if (-1 < legacycard)
    {
      alternate_legacy_text(2, player, legacycard);
    }
    instance = get_card_instance(player, legacycard);
    instance->targets[1].player = t_player;
    instance->targets[1].card = 0x10;
  }
  return;
}

/* ManalinkEh 2022: 0x26934a0; original functions/functions.c:15212. */
int deflect_one_target(int player, int card, event_t event, target_definition_t *td)
{
  int32_t iVar1;
  int player_00;
  card_instance_t *pcVar2;
  int iVar3;
  int iVar4;
  card_instance_t *pcVar5;
  target_definition_t td_default;

  if (td == NULL)
  {
    td = &td_default;
    counterspell_target_definition(player, card, td, 0xff);
  }
  if ((td->extra == 0) && (td->special == 0))
  {
    td->extra = (int32_t)has_only_1_target_and_could_legally_change_target;
    td->special = 0x8000;
  }
  pcVar2 = get_card_instance(player, card);
  player_00 = card_on_stack_controller;
  iVar4 = card_on_stack;
  if (event != EVENT_CAN_CAST)
  {
    if (event == EVENT_CAST_SPELL)
    {
      if ((card == affected_card) && (player == affected_card_controller))
      {
        pcVar2->targets[0].player = card_on_stack_controller;
        pcVar2->targets[0].card = iVar4;
        pcVar2->number_of_targets = '\x01';
        iVar3 = counterspell_validate(player, card, td, 0);
        if (iVar3 == 0)
        {
          pcVar2->targets[0].player = -1;
          pcVar2->targets[0].card = -1;
          spell_fizzled = 1;
        }
        else
        {
          dispatch_event_arbitrary_to_one_card(player_00, iVar4, EVENT_CHANGE_TARGET, player_00,
                                               iVar4, player, card);
        }
      }
    }
    else if ((event == EVENT_RESOLVE_SPELL) &&
             (iVar4 = counterspell_validate(player, card, td, 0), iVar4 != 0))
    {
      pcVar5 = get_card_instance(pcVar2->targets[0].player, pcVar2->targets[0].card);
      iVar1 = pcVar2->targets[1].player;
      pcVar5->targets[0].card = pcVar2->targets[1].card;
      pcVar5->targets[0].player = iVar1;
      iVar1 = pcVar2->targets[2].player;
      pcVar5->targets[1].card = pcVar2->targets[2].card;
      pcVar5->targets[1].player = iVar1;
    }
    return 0;
  }
  iVar4 = counterspell(player, card, EVENT_CAN_CAST, td, 0);
  return iVar4;
}

/* ManalinkEh 2022: 0x2693670; original functions/functions.c:15290. */
int deflect_multi_targets(int player, int card, event_t event, target_definition_t *td)
{
  unsigned char bVar1;
  int32_t iVar2;
  int iVar3;
  card_instance_t *pcVar4;
  int iVar5;
  card_instance_t *pcVar6;
  int iVar7;
  target_definition_t td_default;
  int iVar8;

  if (td == NULL)
  {
    td = &td_default;
    counterspell_target_definition(player, card, td, 0xff);
  }
  pcVar4 = get_card_instance(player, card);
  iVar5 = card_on_stack_controller;
  if (event == EVENT_CAN_CAST)
  {
    iVar5 = counterspell(player, card, EVENT_CAN_CAST, td, 0);
    return iVar5;
  }
  if (event == EVENT_CAST_SPELL)
  {
    if ((card == affected_card) && (player == affected_card_controller))
    {
      pcVar4->number_of_targets = '\x01';
      pcVar4->targets[0].player = iVar5;
      iVar8 = card_on_stack;
      pcVar4->targets[0].card = card_on_stack;
      iVar7 = counterspell_validate(player, card, td, 0);
      if (iVar7 != 0)
      {
        set_special_flags2(iVar5, iVar8, 0x40000000);
        pcVar6 = get_card_instance(iVar5, iVar8);
        bVar1 = pcVar6->number_of_targets;
        if (bVar1 == 10 || bVar1 < 10)
        {
          pcVar4->info_slot = (unsigned int)bVar1;
        }
        else
        {
          pcVar4->info_slot = 10;
        }
        iVar3 = attacking_card_controller;
        iVar7 = attacking_card;
        attacking_card_controller = player;
        attacking_card = card;
        call_card_function(iVar5, iVar8, EVENT_CHANGE_TARGET);
        attacking_card = iVar7;
        attacking_card_controller = iVar3;
        remove_special_flags2(iVar5, iVar8, 0x40000000);
      }
    }
  }
  else if ((event == EVENT_RESOLVE_SPELL) &&
           (iVar5 = counterspell_validate(player, card, td, 0), iVar5 != 0))
  {
    pcVar6 = get_card_instance(pcVar4->targets[0].player, pcVar4->targets[0].card);
    iVar5 = pcVar4->info_slot;
    if (0 < iVar5)
    {
      iVar8 = 0;
      do
      {
        iVar7 = iVar8 + 1;
        iVar2 = pcVar4->targets[iVar8 + 1].player;
        pcVar6->targets[iVar8].card = pcVar4->targets[iVar8 + 1].card;
        pcVar6->targets[iVar8].player = iVar2;
        iVar2 = pcVar4->targets[2].player;
        pcVar6->targets[1].card = pcVar4->targets[2].card;
        pcVar6->targets[1].player = iVar2;
        iVar8 = iVar7;
      } while (iVar7 != iVar5);
    }
  }
  return 0;
}

/* ManalinkEh 2022: 0x2693e80; original functions/functions.c:15742. */
int create_card_name_legacy_unattached(int player, int card, int id)
{
  int card_00;
  card_instance_t *instance;

  card_00 = create_legacy_effect_exe(player, card, LEGACY_EFFECT_ASWAN, -1, -1);
  instance = get_card_instance(player, card_00);
  instance->info_slot = id ^ 0xffffffff;
  return card_00;
}

/* Recovered October 2022 implementations; see docs/recovery-progress.md. */
void multi_x_spell(int player, int card, event_t event, int num_x);

/* ManalinkEh 2022: 0x2681450; original functions/functions.c:7340. */
void negate_combat_damage_this_turn_to(int player, int card, int t_player, int t_card,
                                       int alternate_legacy_text_number)
{
  int legacycard;

  legacycard =
      create_targetted_legacy_effect(player, card, no_combat_damage_this_turn_to, t_player, t_card);
  if ((-1 < legacycard) && (0 < alternate_legacy_text_number))
  {
    alternate_legacy_text(alternate_legacy_text_number, player, legacycard);
    return;
  }
  return;
}

/* ManalinkEh 2022: 0x2694000; original functions/functions.c:12637. */
void multi_x_spell(int player, int card, event_t event, int num_x)
{
  int result;

  if (((event == EVENT_CAST_SPELL) && (card == affected_card)) &&
      (player == affected_card_controller))
  {
    result = played_for_free(player, card);
    if (result == 0)
    {
      result = is_token(player, card);
      if (result == 0)
      {
        x_value = x_value / num_x;
        set_x_for_x_spells(player, card, EVENT_CAST_SPELL, x_value);
        set_special_flags2(player, card, 0x20000);
        return;
      }
    }
  }
  return;
}

/* ManalinkEh 2022: 0x26a81a0; original functions/functions.c:19973. */
char *get_color_text(int player, char *buffer, int clr)
{
  int iVar1;
  int iVar2;
  char *buf;
  size_t size;
  int iVar3;
  unsigned int uVar4;
  bool bVar5;

  if (ai_is_speculating != 1)
  {
    iVar1 = count_colors(player, clr);
    bVar5 = (clr & 2U) != 0;
    if (bVar5)
    {
      iVar3 = scnprintf(buffer, 100, "black");
    }
    else
    {
      iVar3 = 0;
    }
    uVar4 = (unsigned int)bVar5;
    if ((clr & 4U) != 0)
    {
      if ((int)(iVar1 - uVar4) < 2)
      {
        iVar2 = scnprintf(buffer + iVar3, 100U - iVar3, " or ");
      }
      else
      {
        iVar2 = scnprintf(buffer + iVar3, 100U - iVar3, ", ");
      }
      iVar3 = iVar3 + iVar2;
      uVar4 = uVar4 + 1;
      iVar2 = scnprintf(buffer + iVar3, 100 - iVar3, "blue");
      iVar3 = iVar3 + iVar2;
    }
    if ((clr & 8U) != 0)
    {
      if ((int)(iVar1 - uVar4) < 2)
      {
        iVar2 = scnprintf(buffer + iVar3, 100U - iVar3, " or ");
      }
      else
      {
        iVar2 = scnprintf(buffer + iVar3, 100U - iVar3, ", ");
      }
      iVar3 = iVar3 + iVar2;
      uVar4 = uVar4 + 1;
      iVar2 = scnprintf(buffer + iVar3, 100 - iVar3, "green");
      iVar3 = iVar3 + iVar2;
    }
    if ((clr & 0x10U) != 0)
    {
      if ((int)(iVar1 - uVar4) < 2)
      {
        iVar2 = scnprintf(buffer + iVar3, 100U - iVar3, " or ");
      }
      else
      {
        iVar2 = scnprintf(buffer + iVar3, 100U - iVar3, ", ");
      }
      iVar3 = iVar3 + iVar2;
      iVar2 = scnprintf(buffer + iVar3, 100 - iVar3, "red");
      iVar3 = iVar3 + iVar2;
    }
    if ((clr & 0x20U) != 0)
    {
      buf = buffer + iVar3;
      size = 100 - iVar3;
      if (iVar1 != 1)
      {
        iVar1 = scnprintf(buf, size, "or ");
        size = 100 - (iVar1 + iVar3);
        buf = buffer + iVar1 + iVar3;
      }
      scnprintf(buf, size, "white");
    }
    return buffer;
  }
  return "";
}

/* Recovered October 2022 implementations; see docs/recovery-progress.md. */

/* ManalinkEh 2022: 0x2678680; original functions/functions.c:15201. */
static char *has_only_1_target_and_could_legally_change_target(int who_chooses, int player,
                                                               int card)
{
  card_instance_t *instance;
  char *pcVar2;
  int result;

  instance = get_card_instance(player, card);
  pcVar2 = "must have only 1 target";
  if (instance->number_of_targets == '\x01')
  {

    result = call_card_function(player, card, EVENT_CAN_CHANGE_TARGET);
    pcVar2 = NULL;
    if (result == 0)
    {
      return "this spell cannot change its target";
    }
  }
  return pcVar2;
}

/* Recovered October 2022 implementations; see docs/recovery-progress.md. */
void dat_ability(int player, int card, event_t event, declare_attackers_trigger_t mode,
                 int skip_store_attackers);
int dfc_front_to_back(int csvid);
int dfc_back_to_front(int csvid);
int count_basic_lands(int player, int subtype);
int check_for_death_trigger_removal(int player, int card);
int extra_combat_effect(int player, int card, event_t event, int untap,
                        test_definition_t *this_test, int can_repeat);

/* ManalinkEh 2022: 0x269f000; original functions/functions.c:531. */
static int remove_until_upkeep_legacy(int player, int card, event_t event)
{
  card_instance_t *instance;
  int iVar2;
  int iVar3;

  upkeep_trigger_ability_mode(player, card, event, player, RESOLVE_TRIGGER_MANDATORY);
  if (event != EVENT_UPKEEP_TRIGGER_ABILITY)
  {
    return 0;
  }

  instance = in_play(player, card);
  if (instance == NULL)
  {
    return 0;
  }
  iVar2 = instance->targets[1].player;
  if ((iVar2 != 2) && (iVar2 != current_turn))
  {
    return 0;
  }
  iVar2 = check_rfg(instance->targets[0].player,
                    (unsigned int)cards_data[instance->targets[0].card].id);
  if (iVar2 != 0)
  {

    iVar2 = add_card_to_hand(instance->targets[0].player, instance->targets[0].card);
    iVar3 = has_subtype(instance->targets[0].player, iVar2, SUBTYPE_AURA);
    if (iVar3 != 0)
    {
      iVar3 = call_card_function(instance->targets[0].player, iVar2, EVENT_CAN_CAST);
      if (iVar3 == 0)
      {
        obliterate_card(instance->targets[0].player, iVar2);
        goto LAB_0269f087;
      }
      set_special_flags3(instance->targets[0].player, iVar2, 0x40);
    }
    remove_card_from_rfg(instance->targets[0].player,
                         (unsigned int)cards_data[instance->targets[0].card].id);
    put_into_play(instance->targets[0].player, iVar2);
    if (instance->targets[1].card == 0x42)
    {
      iVar2 = get_abilities(instance->targets[0].player, iVar2, EVENT_TOUGHNESS, -1);
      new_multidiscard(player, iVar2, 0, player);
    }
  }
LAB_0269f087:
  kill_card(player, card, KILL_EXILE);
  return 0;
}

/* ManalinkEh 2022: 0x2682020; original functions/functions.c:717. */
int check_for_death_trigger_removal(int player, int card)
{
  int iVar1;
  card_instance_t *instance;
  int iVar3;
  int card_00;

  iVar1 = is_what(player, card, 2);
  if ((iVar1 != 0) && ((instance = get_card_instance(player, card),
                        instance->kill_code < 4 || (instance = get_card_instance(player, card),
                                                    instance->kill_code == '\x11'))))
  {

    iVar1 = 0;
    do
    {
      card_00 = 0;
      if (0 < active_cards_count[iVar1])
      {
        do
        {
          instance = in_play(iVar1, card_00);
          if (((instance != NULL) &&
               (iVar3 = is_humiliated(iVar1, card_00, AL_STATIC), iVar3 == 0)) &&
              (iVar3 = get_id(iVar1, card_00), iVar3 == 0x5097))
          {
            return 1;
          }
          card_00 = card_00 + 1;
        } while (card_00 < active_cards_count[iVar1]);
      }
      iVar1 = iVar1 + 1;
    } while (iVar1 != 2);
  }
  return 0;
}

/* ManalinkEh 2022: 0x267c470; original functions/functions.c:3170. */
int split_cards_id_table(int csvid, int half_played)
{
  int result;

  if (half_played == 0)
  {
    return -1;
  }
  if (csvid == 0x4208)
  {
    return (-(unsigned int)((half_played & 1U) == 0) & 0xffffffe7) + 0x4af7;
  }
  if (0x4208 < csvid)
  {
    if (csvid == 0x4b79)
    {
      return (half_played & 1U) + 0x4b77;
    }
    if (0x4b79 < csvid)
    {
      if (csvid == 0x4c71)
      {
        return (half_played & 1U) + 0x4c6f;
      }
      if (csvid < 0x4c72)
      {
        if (csvid == 0x4bc9)
        {
          return (-(unsigned int)((half_played & 1U) == 0) & 2) + 0x4bc8;
        }
        if (csvid < 0x4bca)
        {
          if (csvid == 0x4ba2)
          {
            return (half_played & 1U) + 0x4ba0;
          }
          if (csvid < 0x4ba3)
          {
            if (csvid == 0x4b8b)
            {
              return (-(unsigned int)((half_played & 1U) == 0) & 3) + 0x4b8a;
            }
            if (csvid != 0x4b9c)
            {
              return -1;
            }
            return (-(unsigned int)((half_played & 1U) == 0) & 3) + 0x4b9b;
          }
          result = (half_played & 1U) + 0x4ba6;
          if (csvid != 0x4ba8)
          {
            return -1;
          }
        }
        else
        {
          if (csvid == 0x4c24)
          {
            return (half_played & 1U) + 0x4c22;
          }
          if (csvid < 0x4c25)
          {
            if (csvid == 0x4bce)
            {
              return (half_played & 1U) + 0x4bcc;
            }
            if (csvid != 0x4c0f)
            {
              return -1;
            }
            return (-(unsigned int)((half_played & 1U) == 0) & 2) + 0x4c0e;
          }
          result = (half_played & 1U) + 0x4c5f;
          if (csvid != 0x4c61)
          {
            return -1;
          }
        }
      }
      else
      {
        if (csvid == 0x4cff)
        {
          return (half_played & 1U) + 0x4cfd;
        }
        if (csvid < 0x4d00)
        {
          if (csvid == 0x4c8f)
          {
            return (half_played & 1U) + 0x4c8d;
          }
          if (csvid < 0x4c90)
          {
            if (csvid == 0x4c7f)
            {
              return (-(unsigned int)((half_played & 1U) == 0) & 2) + 0x4c7e;
            }
            if (csvid != 0x4c83)
            {
              return -1;
            }
            return (-(unsigned int)((half_played & 1U) == 0) & 3) + 0x4c82;
          }
          result = (half_played & 1U) + 0x4ccc;
          if (csvid != 0x4cce)
          {
            return -1;
          }
        }
        else
        {
          if (csvid == 0x4d3f)
          {
            return (-(unsigned int)((half_played & 1U) == 0) & 2) + 0x4d3e;
          }
          if (csvid != 0x4d50)
          {
            if (csvid != 0x4d03)
            {
              return -1;
            }
            return (half_played & 1U) + 0x4d01;
          }
          result = (half_played & 1U) + 0x4d4e;
        }
      }
      return result;
    }
    if (csvid == 0x42ce)
    {
      return (-(unsigned int)((half_played & 1U) == 0) & 0x39) + 0x4adf;
    }
    if (csvid < 0x42cf)
    {
      if (0x4289 < csvid)
      {
        if (csvid == 0x42bd)
        {
          return (-(unsigned int)((half_played & 1U) == 0) & 3) + 0x4aca;
        }
        if (csvid != 0x42cd)
        {
          return -1;
        }
        return (-(unsigned int)((half_played & 1U) == 0) & 0x1d) + 0x4adc;
      }
      if (0x4262 < csvid)
      {
        switch (csvid)
        {
        case 0x4263:
          return (-(unsigned int)((half_played & 1U) == 0) & 0xffffffe0) + 0x4b1d;
        default:
          return -1;
        case 0x426a:
          return (-(unsigned int)((half_played & 1U) == 0) & 10) + 0x4b22;
        case 0x426b:
          return (-(unsigned int)((half_played & 1U) == 0) & 9) + 0x4b26;
        case 0x4288:
          return (-(unsigned int)((half_played & 1U) == 0) & 0xffffffdd) + 0x4b35;
        case 0x4289:
          return (-(unsigned int)((half_played & 1U) == 0) & 0xffffffc7) + 0x4b37;
        }
      }
      if (csvid == 0x422b)
      {
        return (-(unsigned int)((half_played & 1U) == 0) & 0xffffffef) + 0x4b04;
      }
      if (csvid != 0x4259)
      {
        return -1;
      }
      return (-(unsigned int)((half_played & 1U) == 0) & 0x29) + 0x4b1a;
    }
    if (csvid == 0x4335)
    {
      return (-(unsigned int)((half_played & 1U) == 0) & 0xffffffac) + 0x4b25;
    }
    if (0x4335 < csvid)
    {
      if (csvid == 0x434e)
      {
        return (-(unsigned int)((half_played & 1U) == 0) & 3) + 0x4b38;
      }
      if (csvid != 0x4b55)
      {
        if (csvid != 0x4336)
        {
          return -1;
        }
        return (-(unsigned int)((half_played & 1U) == 0) & 0xffffffb9) + 0x4b27;
      }
      return (half_played & 1U) + 0x4b53;
    }
    if (csvid == 0x42f9)
    {
      return (-(unsigned int)((half_played & 1U) == 0) & 0xffffffef) + 0x4b03;
    }
    if (0x42f9 < csvid)
    {
      if (csvid != 0x430f)
      {
        return -1;
      }
      return (-(unsigned int)((half_played & 1U) == 0) & 0xffffffce) + 0x4b0c;
    }
    if (csvid == 0x42e4)
    {
      return (-(unsigned int)((half_played & 1U) == 0) & 0xfffffffa) + 0x4af0;
    }
    if (csvid != 0x42eb)
    {
      return -1;
    }
    return (-(unsigned int)((half_played & 1U) == 0) & 0x15) + 0x4afb;
  }
  if (csvid == 0x206d)
  {
    return (-(unsigned int)((half_played & 1U) == 0) & 0xffffffaf) + 0x4b3a;
  }
  if (0x206d < csvid)
  {
    if (csvid < 0x3393)
    {
      if (csvid < 0x3337)
      {
        if (csvid != 0x2072)
        {
          return -1;
        }
        return (-(unsigned int)((half_played & 1U) == 0) & 0xffffffb7) + 0x4b3f;
      }
      switch (csvid)
      {
      case 0x3337:
        goto code_r0x0267cbb7;
      case 0x3338:
        if ((half_played & 3U) == 3)
        {
          return csvid;
        }
        return (-(unsigned int)((half_played & 1U) == 0) & 0x18) + 0x4acb;
      default:
        return -1;
      case 0x333b:
        if ((half_played & 3U) == 3)
        {
          return csvid;
        }
        return (-(unsigned int)((half_played & 1U) == 0) & 0xffffffd4) + 0x4afa;
      case 0x3340:
        if ((half_played & 3U) == 3)
        {
          return csvid;
        }
        return (-(unsigned int)((half_played & 1U) == 0) & 8) + 0x4ad0;
      case 0x3344:
        if ((half_played & 3U) == 3)
        {
          return csvid;
        }
        return (-(unsigned int)((half_played & 1U) == 0) & 0xffffffd2) + 0x4b00;
      case 0x334a:
        if ((half_played & 3U) == 3)
        {
          return csvid;
        }
        return (-(unsigned int)((half_played & 1U) == 0) & 0x20) + 0x4ad5;
      case 0x334d:
        if ((half_played & 3U) == 3)
        {
          return csvid;
        }
        return (-(unsigned int)((half_played & 1U) == 0) & 0xffffff94) + 0x4b42;
      case 0x334f:
        if ((half_played & 3U) == 3)
        {
          return csvid;
        }
        return (-(unsigned int)((half_played & 1U) == 0) & 0x4f) + 0x4ad9;
      case 0x3357:
        if ((half_played & 3U) == 3)
        {
          return csvid;
        }
        return (half_played & 1U) + 0x4aee;
      case 0x335f:
        if ((half_played & 3U) == 3)
        {
          return csvid;
        }
        return (-(unsigned int)((half_played & 1U) == 0) & 0x3b) + 0x4b01;
      case 0x3370:
        if ((half_played & 3U) == 3)
        {
          return csvid;
        }
        return (-(unsigned int)((half_played & 1U) == 0) & 0xfffffff0) + 0x4b1e;
      case 0x338c:
        if ((half_played & 3U) == 3)
        {
          return csvid;
        }
        return (-(unsigned int)((half_played & 1U) == 0) & 0x13) + 0x4b1f;
      case 0x3392:
        if ((half_played & 3U) == 3)
        {
          return csvid;
        }
        return (-(unsigned int)((half_played & 1U) == 0) & 0x2a) + 0x4b23;
      }
    }
    if (csvid == 0x41f1)
    {
      return (-(unsigned int)((half_played & 1U) == 0) & 0x49) + 0x4ae2;
    }
    if (0x41f1 < csvid)
    {
      if (csvid != 0x41f8)
      {
        return -1;
      }
      return (-(unsigned int)((half_played & 1U) == 0) & 0x20) + 0x4aeb;
    }
    if (csvid != 0x33af)
    {
      if (csvid != 0x33b3)
      {
        return -1;
      }
      if ((half_played & 3U) == 3)
      {
        return 0x33b3;
      }
      return (-(unsigned int)((half_played & 1U) == 0) & 2) + 0x4b3e;
    }
    if ((half_played & 3U) == 3)
    {
      return 0x33af;
    }
    return (-(unsigned int)((half_played & 1U) == 0) & 0xfffffff7) + 0x4b46;
  }
  if (csvid == 0x1522)
  {
    return (-(unsigned int)((half_played & 1U) == 0) & 0xffffffc0) + 0x4b1b;
  }
  if (0x1522 < csvid)
  {
    if (csvid == 0x2035)
    {
      return (-(unsigned int)((half_played & 1U) == 0) & 0xffffffdb) + 0x4b19;
    }
    if (0x2035 < csvid)
    {
      if (csvid == 0x2052)
      {
        return (-(unsigned int)((half_played & 1U) == 0) & 0xffffffc4) + 0x4b29;
      }
      if (csvid != 0x2054)
      {
        if (csvid != 0x2048)
        {
          return -1;
        }
        return (-(unsigned int)((half_played & 1U) == 0) & 0x12) + 0x4b21;
      }
      return (-(unsigned int)((half_played & 1U) == 0) & 0xffffffcb) + 0x4b2d;
    }
    if (csvid == 0x201e)
    {
      return (-(unsigned int)((half_played & 1U) == 0) & 0x2c) + 0x4b05;
    }
    if (0x201e < csvid)
    {
      if (csvid != 0x201f)
      {
        return -1;
      }
      return (-(unsigned int)((half_played & 1U) == 0) & 0x2a) + 0x4b06;
    }
    if (csvid == 0x1ff3)
    {
      return (-(unsigned int)((half_played & 1U) == 0) & 0x18) + 0x4ad4;
    }
    if (csvid != 0x1ffd)
    {
      return -1;
    }
    return (-(unsigned int)((half_played & 1U) == 0) & 0x3f) + 0x4ae1;
  }
  if (csvid == 0x1443)
  {
    return (half_played & 1U) + 0x4b44;
  }
  if (csvid < 0x1444)
  {
    if (csvid == 0x1407)
    {
      return (-(unsigned int)((half_played & 1U) == 0) & 0xffffffdb) + 0x4b34;
    }
    if (0x1407 < csvid)
    {
      if (csvid != 0x140a)
      {
        return -1;
      }
      return (-(unsigned int)((half_played & 1U) == 0) & 0xffffffb2) + 0x4b36;
    }
    if (csvid == 0x1348)
    {
      return (-(unsigned int)((half_played & 1U) == 0) & 3) + 0x4acc;
    }
    if (csvid != 0x13c0)
    {
      return -1;
    }
    return (-(unsigned int)((half_played & 1U) == 0) & 0x1d) + 0x4b1c;
  }
  if (csvid == 0x1513)
  {
    return (-(unsigned int)((half_played & 1U) == 0) & 0xffffffda) + 0x4b0d;
  }
  if (0x1513 < csvid)
  {
    if (csvid != 0x1521)
    {
      return -1;
    }
    return (-(unsigned int)((half_played & 1U) == 0) & 0xffffffce) + 0x4b17;
  }
  if (csvid == 0x14f9)
  {
    return (-(unsigned int)((half_played & 1U) == 0) & 8) + 0x4aff;
  }
  if (csvid != 0x150a)
  {
    return -1;
  }
  return (-(unsigned int)((half_played & 1U) == 0) & 0x1c) + 0x4b08;
code_r0x0267cbb7:
  if ((half_played & 3U) == 3)
  {
    return csvid;
  }
  return (-(unsigned int)((half_played & 1U) == 0) & 0x7e) + 0x4ac9;
}

/* ManalinkEh 2022: 0x267cf50; original functions/functions.c:3417. */
int dfc_front_to_back(int csvid)
{
  int result;

  if (csvid == 0x5739)
  {
    return 0x573a;
  }
  if (csvid < 0x573a)
  {
    if (csvid < 0x3edd)
    {
      if (0x3eb9 < csvid)
      {
        switch (csvid)
        {
        case 0x3eba:
          return 0x3ebb;
        default:
          return 0;
        case 0x3ec1:
          return 0x3ec2;
        case 0x3ec4:
          return 0x3ec5;
        case 0x3ecf:
          return 0x3ed0;
        case 0x3edc:
          return 0x3edd;
        }
      }
      if (csvid == 0x3b40)
      {
        return 0x3b41;
      }
      if (csvid < 0x3b41)
      {
        if (0x2f77 < csvid)
        {
          if (csvid < 0x2fec)
          {
            if (0x2faf < csvid)
            {
              switch (csvid)
              {
              case 0x2fb0:
                return 0x2fb1;
              default:
                return 0;
              case 0x2fb5:
                return 0x2fb6;
              case 0x2fbf:
                return 0x2fc0;
              case 0x2fc3:
                return 0x2fc4;
              case 0x2fc9:
                return 0x2fca;
              case 0x2fd4:
                return 0x2fd5;
              case 0x2fdd:
                return 0x2fde;
              case 0x2feb:
                return 0x2fec;
              }
            }
            return (csvid != 0x2f89) - 1 & 0x2f8a;
          }
          if (csvid == 0x3af8)
          {
            return 0x3af9;
          }
          if (csvid == 0x3b37)
          {
            return 0x3b38;
          }
          return (csvid != 0x3004) - 1 & 0x3005;
        }
        if (0x2f3f < csvid)
        {
          switch (csvid)
          {
          case 0x2f40:
            return 0x2f41;
          default:
            return 0;
          case 0x2f42:
            return 0x2f43;
          case 0x2f4b:
            return 0x2f4c;
          case 0x2f57:
            return 0x2f58;
          case 0x2f59:
            return 0x2f5a;
          case 0x2f62:
            return 0x2f63;
          case 0x2f73:
            return 0x2f74;
          case 0x2f77:
            return 0x2f78;
          }
        }
        if (csvid < 0x2ef2)
        {
          if (0x2eb3 < csvid)
          {
            switch (csvid)
            {
            case 0x2eb4:
              return 0x2eb5;
            default:
              return 0;
            case 0x2eb6:
              return 0x2eb7;
            case 0x2ec7:
              return 0x2ec8;
            case 0x2ecc:
              return 0x2ecd;
            case 0x2ed6:
              return 0x2ed7;
            case 12000:
              return 0x2ee1;
            case 0x2ee6:
              return 0x2ee7;
            case 0x2ef1:
              return 0x2ef2;
            }
          }
          switch (csvid)
          {
          case 0x2e6f:
            return 0x2e70;
          case 0x2e70:
          case 0x2e71:
          case 0x2e72:
          case 0x2e73:
          case 0x2e74:
          case 0x2e75:
          case 0x2e76:
          case 0x2e77:
          case 0x2e78:
          case 0x2e79:
          case 0x2e7a:
          case 0x2e7b:
          case 0x2e7c:
          case 0x2e7d:
          case 0x2e7e:
          case 0x2e80:
          case 0x2e81:
          case 0x2e82:
          case 0x2e84:
          case 0x2e85:
          case 0x2e86:
          case 0x2e87:
          case 0x2e88:
          case 0x2e89:
          case 0x2e8a:
          case 0x2e8b:
          case 0x2e8c:
          case 0x2e8d:
          case 0x2e8e:
          case 0x2e8f:
          case 0x2e90:
          case 0x2e92:
          case 0x2e93:
          case 0x2e94:
            return 0;
          case 0x2e7f:
            return 0x2e80;
          case 0x2e83:
            return 0x2e84;
          case 0x2e91:
            return 0x2e92;
          case 0x2e95:
            return 0x2e96;
          default:
            return 0;
          }
        }
        result = 0x2f16;
        if (csvid != 0x2f15)
        {
          return (csvid != 0x2f1e) - 1 & 0x2f1f;
        }
      }
      else
      {
        if (0x3e2e < csvid)
        {
          if (csvid == 0x3e42)
          {
            return 0x3e43;
          }
          switch (csvid)
          {
          case 0x3e55:
            return 0x3e56;
          case 0x3e56:
          case 0x3e57:
          case 0x3e58:
          case 0x3e5a:
          case 0x3e5b:
          case 0x3e5c:
          case 0x3e5d:
          case 0x3e5e:
          case 0x3e5f:
          case 0x3e61:
          case 0x3e62:
          case 0x3e63:
          case 0x3e64:
          case 0x3e65:
          case 0x3e66:
          case 0x3e67:
          case 0x3e68:
          case 0x3e69:
          case 0x3e6a:
          case 0x3e6b:
          case 0x3e6c:
          case 0x3e6d:
          case 0x3e6e:
          case 0x3e6f:
          case 0x3e70:
          case 0x3e72:
          case 0x3e73:
          case 0x3e75:
          case 0x3e76:
          case 0x3e77:
          case 0x3e78:
          case 0x3e79:
          case 0x3e7a:
          case 0x3e7c:
          case 0x3e7d:
          case 0x3e7e:
          case 15999:
          case 16000:
          case 0x3e81:
          case 0x3e82:
          case 0x3e83:
          case 0x3e84:
          case 0x3e85:
          case 0x3e87:
          case 0x3e88:
          case 0x3e89:
          case 0x3e8a:
          case 0x3e8b:
          case 0x3e8c:
          case 0x3e8d:
          case 0x3e8e:
          case 0x3e90:
          case 0x3e91:
          case 0x3e92:
          case 0x3e93:
          case 0x3e94:
          case 0x3e95:
            return 0;
          case 0x3e59:
            return 0x3e5a;
          case 0x3e60:
            return 0x3e61;
          case 0x3e71:
            return 0x3e72;
          case 0x3e74:
            return 0x3e75;
          case 0x3e7b:
            return 0x3e7c;
          case 0x3e86:
            return 0x3e87;
          case 0x3e8f:
            return 0x3e90;
          case 0x3e96:
            return 0x3e97;
          default:
            return 0;
          }
        }
        if (0x3dd1 < csvid)
        {
          switch (csvid)
          {
          case 0x3dd2:
            return 0x3dd3;
          default:
            return 0;
          case 0x3dd4:
            return 0x3dd5;
          case 0x3de1:
            return 0x3de2;
          case 0x3de4:
            return 0x3de5;
          case 0x3de7:
            return 0x3de8;
          case 0x3de9:
            return 0x3dea;
          case 0x3dec:
            return 0x3ded;
          case 0x3e03:
            return 0x3e04;
          case 0x3e05:
            return 0x3e06;
          case 0x3e07:
            return 0x3e08;
          case 0x3e1e:
            return 0x3e1f;
          case 0x3e2e:
            return 0x3e2f;
          }
        }
        if (0x3db6 < csvid)
        {
          return 0;
        }
        if (csvid < 0x3d6e)
        {
          result = 0x3b45;
          if (csvid != 0x3b44)
          {
            return (csvid != 0x3b54) - 1 & 0x3b55;
          }
        }
        else
        {
          switch (csvid)
          {
          case 0x3d6e:
            return 0x3d6f;
          default:
            return 0;
          case 0x3d70:
            result = 0x3d71;
            break;
          case 0x3d7a:
            return 0x3d7b;
          case 0x3d7c:
            return 0x3d7d;
          case 0x3d80:
            return 0x3d81;
          case 0x3d83:
            return 0x3d84;
          case 0x3d8b:
            return 0x3d8c;
          case 0x3d9a:
            return 0x3d9b;
          case 0x3da3:
            return 0x3da4;
          case 0x3db3:
            return 0x3db4;
          case 0x3db6:
            return 0x3db7;
          }
        }
      }
    }
    else if (csvid < 0x5561)
    {
      if (0x5500 < csvid)
      {
        switch (csvid)
        {
        case 0x5501:
          return 0x5502;
        default:
          return 0;
        case 0x5505:
          return 0x5506;
        case 0x5513:
          return 0x5514;
        case 0x5516:
          return 0x5517;
        case 0x5518:
          return 0x5519;
        case 0x551d:
          return 0x551e;
        case 0x551f:
          return 0x5520;
        case 0x552c:
          return 0x552d;
        case 0x5536:
          return 0x5537;
        case 0x5544:
          return 0x5545;
        case 0x5556:
          return 0x5557;
        case 0x5560:
          return 0x5561;
        }
      }
      if (csvid == 0x4430)
      {
        return 0x4431;
      }
      if (csvid < 0x4431)
      {
        if (csvid < 0x3f69)
        {
          if (0x3f34 < csvid)
          {
            switch (csvid)
            {
            case 0x3f35:
              return 0x3f36;
            default:
              return 0;
            case 0x3f3b:
              return 0x3f3c;
            case 0x3f54:
              return 0x3f55;
            case 0x3f5e:
              return 0x3f5f;
            case 0x3f60:
              return 0x3f61;
            case 0x3f66:
              return 0x3f67;
            case 0x3f68:
              return 0x3f69;
            }
          }
          if (csvid != 0x3f0a)
          {
            if (csvid == 0x3f11)
            {
              return 0x3f12;
            }
            return (csvid != 0x3ef7) - 1 & 0x3ef8;
          }
          return 0x3f0b;
        }
        if (csvid == 0x43f5)
        {
          return 0x43f6;
        }
        if (0x43f5 < csvid)
        {
          return (csvid != 0x4412) - 1 & 0x4413;
        }
        result = 0x43c1;
        if (csvid != 0x43c0)
        {
          return (csvid != 0x43dc) - 1 & 0x43dd;
        }
      }
      else
      {
        if (csvid == 0x472b)
        {
          return 0x472c;
        }
        if (csvid < 0x472c)
        {
          if (csvid == 0x44a0)
          {
            return 0x44a1;
          }
          if (csvid < 0x44a1)
          {
            if (csvid == 0x446c)
            {
              return 0x446d;
            }
            if (csvid == 0x4497)
            {
              return 0x4498;
            }
            return (csvid != 0x444a) - 1 & 0x444b;
          }
          if (csvid == 0x46f0)
          {
            return 0x46f1;
          }
          if (csvid == 0x4727)
          {
            return 0x4728;
          }
          return (csvid != 0x44a6) - 1 & 0x44a7;
        }
        if (csvid == 0x477d)
        {
          return 0x477e;
        }
        if (csvid < 0x477e)
        {
          if (csvid == 0x4753)
          {
            return 0x4754;
          }
          if (csvid == 0x4759)
          {
            return 0x475a;
          }
          return (csvid != 0x4737) - 1 & 0x4738;
        }
        result = 0x4a1a;
        if (csvid != 0x4a19)
        {
          return (csvid != 0x4a74) - 1 & 0x4a75;
        }
      }
    }
    else if (csvid < 0x562a)
    {
      result = 0;
      if (0x556a < csvid)
      {
        switch (csvid)
        {
        case 0x556b:
          return 0x556c;
        default:
          return 0;
        case 0x556e:
          return 0x556f;
        case 0x5573:
          return 0x5574;
        case 0x5577:
          return 0x5578;
        case 0x5579:
          return 0x557a;
        case 0x558b:
          return 0x558c;
        case 0x558e:
          return 0x558f;
        case 0x55a3:
          return 0x55a4;
        case 0x55af:
          return 0x55b0;
        case 0x55b4:
          return 0x55b5;
        case 0x55c3:
          return 0x55c4;
        case 0x55d3:
          return 0x55d4;
        case 0x55d7:
          return 0x55d8;
        case 0x55dd:
          return 0x55de;
        case 0x55e1:
          return 0x55e2;
        case 0x55e5:
          return 0x55e6;
        case 0x55f1:
          return 0x55f2;
        case 0x55f5:
          return 0x55f6;
        case 0x5604:
          return 0x5605;
        case 0x5614:
          return 0x5615;
        case 0x5617:
          return 0x5618;
        case 0x561a:
          return 0x561b;
        case 0x561d:
          return 0x561e;
        case 0x5629:
          return 0x562a;
        }
      }
    }
    else
    {
      if (csvid == 0x5706)
      {
        return 0x5707;
      }
      if (csvid < 0x5707)
      {
        if (csvid == 0x56f5)
        {
          return 0x56f6;
        }
        if (csvid == 0x5703)
        {
          return 0x5704;
        }
        return (csvid != 0x56e7) - 1 & 0x56e8;
      }
      result = 0x571a;
      if (csvid != 0x5719)
      {
        return (csvid != 0x5721) - 1 & 0x5722;
      }
    }
  }
  else
  {
    if (0x5fc2 < csvid)
    {
      return 0;
    }
    if (0x5bfb < csvid)
    {
      switch (csvid)
      {
      case 0x5bfc:
        return 0x5bfd;
      default:
        return 0;
      case 0x5c01:
        return 0x5c02;
      case 0x5c05:
        return 0x5c06;
      case 0x5c07:
        return 0x5c08;
      case 0x5c0a:
        return 0x5c0b;
      case 0x5c0c:
        return 0x5c0d;
      case 0x5c0e:
        return 0x5c0f;
      case 0x5c1c:
        return 0x5c1d;
      case 0x5c1e:
        return 0x5c1f;
      case 0x5c2b:
        return 0x5c2c;
      case 0x5c33:
        return 0x5c34;
      case 0x5c35:
        return 0x5c36;
      case 0x5c3a:
        return 0x5c3b;
      case 0x5c42:
        return 0x5c43;
      case 0x5c44:
        return 0x5c45;
      case 0x5c48:
        return 0x5c49;
      case 0x5c4b:
        return 0x5c4c;
      case 0x5c57:
        return 0x5c58;
      case 0x5c5a:
        return 0x5c5b;
      case 0x5c62:
        return 0x5c63;
      case 0x5c67:
        return 0x5c68;
      case 0x5c6e:
        return 0x5c6f;
      case 0x5c7a:
        return 0x5c7b;
      case 0x5c7e:
        return 0x5c7f;
      case 0x5c83:
        return 0x5c84;
      case 0x5c87:
        return 0x5c88;
      case 0x5c89:
        return 0x5c8a;
      case 0x5c91:
        return 0x5c92;
      case 0x5c95:
        return 0x5c96;
      case 0x5c9f:
        return 0x5ca0;
      case 0x5ca2:
        return 0x5ca3;
      case 0x5ca4:
        return 0x5ca5;
      case 0x5caf:
        return 0x5cb0;
      case 0x5cb1:
        return 0x5cb2;
      case 0x5cb3:
        return 0x5cb4;
      case 0x5cc1:
        return 0x5cc2;
      case 0x5cc4:
        return 0x5cc5;
      case 0x5ccc:
        return 0x5ccd;
      case 0x5cd1:
        return 0x5cd2;
      case 0x5ce3:
        return 0x5ce4;
      case 0x5cf1:
        return 0x5cf2;
      case 0x5cf6:
        return 0x5cf7;
      case 0x5d03:
        return 0x5d04;
      case 0x5d07:
        return 0x5d08;
      case 0x5d0e:
        return 0x5d0f;
      case 0x5d10:
        return 0x5d11;
      case 0x5d12:
        return 0x5d13;
      case 0x5d1c:
        return 0x5d1d;
      case 0x5d1e:
        return 0x5d1f;
      case 0x5d50:
        return 0x5d51;
      case 0x5d5a:
        return 0x5d5b;
      case 0x5d5c:
        return 0x5d5d;
      case 0x5d5f:
        return 0x5d60;
      case 0x5d61:
        return 0x5d62;
      case 0x5d6a:
        return 0x5d6b;
      case 0x5d72:
        return 0x5d73;
      case 0x5d76:
        return 0x5d77;
      case 0x5d81:
        return 0x5d82;
      case 0x5d87:
        return 0x5d88;
      case 0x5d96:
        return 0x5d97;
      case 0x5d9a:
        return 0x5d9b;
      case 0x5d9f:
        return 0x5da0;
      case 0x5da1:
        return 0x5da2;
      case 0x5da9:
        return 0x5daa;
      case 0x5dad:
        return 0x5dae;
      case 0x5db2:
        return 0x5db3;
      case 0x5db7:
        return 0x5db8;
      case 0x5dbe:
        return 23999;
      case 0x5dcc:
        return 0x5dcd;
      case 0x5dd2:
        return 0x5dd3;
      case 0x5dd9:
        return 0x5dda;
      case 0x5ddd:
        return 0x5dde;
      case 0x5de1:
        return 0x5de2;
      case 0x5de3:
        return 0x5de4;
      case 0x5de5:
        return 0x5de6;
      case 0x5dea:
        return 0x5deb;
      case 0x5dec:
        return 0x5ded;
      case 0x5df1:
        return 0x5df2;
      case 0x5df5:
        return 0x5df6;
      case 0x5df7:
        return 0x5df8;
      case 0x5e05:
        return 0x5e06;
      case 0x5e07:
        return 0x5e08;
      case 0x5e0e:
        return 0x5e0f;
      case 0x5e17:
        return 0x5e18;
      case 0x5e21:
        return 0x5e22;
      case 0x5e23:
        return 0x5e24;
      case 0x5e2a:
        return 0x5e2b;
      case 0x5e2f:
        return 0x5e30;
      case 0x5e44:
        return 0x5e45;
      case 0x5e57:
        return 0x5e58;
      case 0x5e59:
        return 0x5e5a;
      case 0x5e64:
        return 0x5e65;
      case 0x5e66:
        return 0x5e67;
      case 0x5e6a:
        return 0x5e6b;
      case 0x5e6f:
        return 0x5e70;
      case 0x5e71:
        return 0x5e72;
      case 0x5e73:
        return 0x5e74;
      case 0x5e7d:
        return 0x5e7e;
      case 0x5eb4:
        return 0x5eb5;
      case 0x5eb9:
        return 0x5eba;
      case 0x5ebb:
        return 0x5ebc;
      case 0x5ec3:
        return 0x5ec4;
      case 0x5ee5:
        return 0x5ee6;
      case 0x5eea:
        return 0x5eeb;
      case 0x5f06:
        return 0x5f07;
      case 0x5f13:
        return 0x5f14;
      case 0x5f1f:
        return 0x5f20;
      case 0x5f33:
        return 0x5f34;
      case 0x5f39:
        return 0x5f3a;
      case 0x5f4a:
        return 0x5f4b;
      case 0x5f62:
        return 0x5f63;
      case 0x5f9f:
        return 0x5fa0;
      case 0x5fa7:
        return 0x5fa8;
      case 0x5fac:
        return 0x5fad;
      case 0x5fae:
        return 0x5faf;
      case 0x5fb0:
        return 0x5fb1;
      case 0x5fb2:
        return 0x5fb3;
      case 0x5fb4:
        return 0x5fb5;
      case 0x5fb7:
        return 0x5fb8;
      case 0x5fb9:
        return 0x5fba;
      case 0x5fc2:
        return 0x5fc3;
      }
    }
    if (csvid == 0x588a)
    {
      return 0x588b;
    }
    if (csvid < 0x588b)
    {
      if (csvid == 0x57f2)
      {
        return 0x57f3;
      }
      if (csvid < 0x57f3)
      {
        if (csvid == 0x5785)
        {
          return 0x5786;
        }
        if (csvid < 0x5786)
        {
          if (csvid == 0x576d)
          {
            return 0x576e;
          }
          if (csvid == 0x5772)
          {
            return 0x5773;
          }
          return (csvid != 0x5743) - 1 & 0x5744;
        }
        result = 0x5792;
        if (csvid != 0x5791)
        {
          return (csvid != 0x57bb) - 1 & 0x57bc;
        }
      }
      else
      {
        if (csvid == 0x5839)
        {
          return 0x583a;
        }
        if (csvid < 0x583a)
        {
          result = 0x57fe;
          if (csvid != 0x57fd)
          {
            return (csvid != 0x5809) - 1 & 0x580a;
          }
        }
        else
        {
          result = 0x5845;
          if (csvid != 0x5844)
          {
            return (csvid != 0x5884) - 1 & 0x5885;
          }
        }
      }
    }
    else
    {
      if (csvid == 0x5908)
      {
        return 0x5909;
      }
      if (csvid < 0x5909)
      {
        if (csvid == 0x58d9)
        {
          return 0x58da;
        }
        if (csvid < 0x58da)
        {
          if (csvid == 0x58b0)
          {
            return 0x58b1;
          }
          if (csvid == 0x58cb)
          {
            return 0x58cc;
          }
          return (csvid != 0x58ab) - 1 & 0x58ac;
        }
        result = 0x58e0;
        if (csvid != 0x58df)
        {
          return (csvid != 0x5901) - 1 & 0x5902;
        }
      }
      else
      {
        if (csvid == 0x5940)
        {
          return 0x5941;
        }
        if (csvid < 0x5941)
        {
          result = 0x590f;
          if (csvid != 0x590e)
          {
            return (csvid != 0x593b) - 1 & 0x593c;
          }
        }
        else
        {
          result = 0x5943;
          if (csvid != 0x5942)
          {
            return (csvid != 0x594a) - 1 & 0x594b;
          }
        }
      }
    }
  }
  return result;
}

/* ManalinkEh 2022: 0x267dce0; original functions/functions.c:3759. */
int dfc_back_to_front(int csvid)
{
  int result;

  if (csvid == 0x573a)
  {
    return 0x5739;
  }
  if (csvid < 0x573b)
  {
    if (csvid < 0x3ede)
    {
      if (0x3eba < csvid)
      {
        switch (csvid)
        {
        case 0x3ebb:
          return 0x3eba;
        default:
          return 0;
        case 0x3ec2:
          return 0x3ec1;
        case 0x3ec5:
          return 0x3ec4;
        case 0x3ed0:
          return 0x3ecf;
        case 0x3edd:
          return 0x3edc;
        }
      }
      if (csvid == 0x3b41)
      {
        return 0x3b40;
      }
      if (csvid < 0x3b42)
      {
        if (0x2f78 < csvid)
        {
          if (csvid < 0x2fed)
          {
            if (0x2fb0 < csvid)
            {
              switch (csvid)
              {
              case 0x2fb1:
                return 0x2fb0;
              default:
                return 0;
              case 0x2fb6:
                return 0x2fb5;
              case 0x2fc0:
                return 0x2fbf;
              case 0x2fc4:
                return 0x2fc3;
              case 0x2fca:
                return 0x2fc9;
              case 0x2fd5:
                return 0x2fd4;
              case 0x2fde:
                return 0x2fdd;
              case 0x2fec:
                return 0x2feb;
              }
            }
            return (csvid != 0x2f8a) - 1 & 0x2f89;
          }
          if (csvid == 0x3af9)
          {
            return 0x3af8;
          }
          if (csvid == 0x3b38)
          {
            return 0x3b37;
          }
          return (csvid != 0x3005) - 1 & 0x3004;
        }
        if (0x2f40 < csvid)
        {
          switch (csvid)
          {
          case 0x2f41:
            return 0x2f40;
          default:
            return 0;
          case 0x2f43:
            return 0x2f42;
          case 0x2f4c:
            return 0x2f4b;
          case 0x2f58:
            return 0x2f57;
          case 0x2f5a:
            return 0x2f59;
          case 0x2f63:
            return 0x2f62;
          case 0x2f74:
            return 0x2f73;
          case 0x2f78:
            return 0x2f77;
          }
        }
        if (csvid < 0x2ef3)
        {
          if (0x2eb4 < csvid)
          {
            switch (csvid)
            {
            case 0x2eb5:
              return 0x2eb4;
            default:
              return 0;
            case 0x2eb7:
              return 0x2eb6;
            case 0x2ec8:
              return 0x2ec7;
            case 0x2ecd:
              return 0x2ecc;
            case 0x2ed7:
              return 0x2ed6;
            case 0x2ee1:
              return 12000;
            case 0x2ee7:
              return 0x2ee6;
            case 0x2ef2:
              return 0x2ef1;
            }
          }
          switch (csvid)
          {
          case 0x2e70:
            return 0x2e6f;
          case 0x2e71:
          case 0x2e72:
          case 0x2e73:
          case 0x2e74:
          case 0x2e75:
          case 0x2e76:
          case 0x2e77:
          case 0x2e78:
          case 0x2e79:
          case 0x2e7a:
          case 0x2e7b:
          case 0x2e7c:
          case 0x2e7d:
          case 0x2e7e:
          case 0x2e7f:
          case 0x2e81:
          case 0x2e82:
          case 0x2e83:
          case 0x2e85:
          case 0x2e86:
          case 0x2e87:
          case 0x2e88:
          case 0x2e89:
          case 0x2e8a:
          case 0x2e8b:
          case 0x2e8c:
          case 0x2e8d:
          case 0x2e8e:
          case 0x2e8f:
          case 0x2e90:
          case 0x2e91:
          case 0x2e93:
          case 0x2e94:
          case 0x2e95:
            return 0;
          case 0x2e80:
            return 0x2e7f;
          case 0x2e84:
            return 0x2e83;
          case 0x2e92:
            return 0x2e91;
          case 0x2e96:
            return 0x2e95;
          default:
            return 0;
          }
        }
        result = 0x2f15;
        if (csvid != 0x2f16)
        {
          return (csvid != 0x2f1f) - 1 & 0x2f1e;
        }
      }
      else
      {
        if (0x3e2f < csvid)
        {
          if (csvid == 0x3e43)
          {
            return 0x3e42;
          }
          switch (csvid)
          {
          case 0x3e56:
            return 0x3e55;
          case 0x3e57:
          case 0x3e58:
          case 0x3e59:
          case 0x3e5b:
          case 0x3e5c:
          case 0x3e5d:
          case 0x3e5e:
          case 0x3e5f:
          case 0x3e60:
          case 0x3e62:
          case 0x3e63:
          case 0x3e64:
          case 0x3e65:
          case 0x3e66:
          case 0x3e67:
          case 0x3e68:
          case 0x3e69:
          case 0x3e6a:
          case 0x3e6b:
          case 0x3e6c:
          case 0x3e6d:
          case 0x3e6e:
          case 0x3e6f:
          case 0x3e70:
          case 0x3e71:
          case 0x3e73:
          case 0x3e74:
          case 0x3e76:
          case 0x3e77:
          case 0x3e78:
          case 0x3e79:
          case 0x3e7a:
          case 0x3e7b:
          case 0x3e7d:
          case 0x3e7e:
          case 15999:
          case 16000:
          case 0x3e81:
          case 0x3e82:
          case 0x3e83:
          case 0x3e84:
          case 0x3e85:
          case 0x3e86:
          case 0x3e88:
          case 0x3e89:
          case 0x3e8a:
          case 0x3e8b:
          case 0x3e8c:
          case 0x3e8d:
          case 0x3e8e:
          case 0x3e8f:
          case 0x3e91:
          case 0x3e92:
          case 0x3e93:
          case 0x3e94:
          case 0x3e95:
          case 0x3e96:
            return 0;
          case 0x3e5a:
            return 0x3e59;
          case 0x3e61:
            return 0x3e60;
          case 0x3e72:
            return 0x3e71;
          case 0x3e75:
            return 0x3e74;
          case 0x3e7c:
            return 0x3e7b;
          case 0x3e87:
            return 0x3e86;
          case 0x3e90:
            return 0x3e8f;
          case 0x3e97:
            return 0x3e96;
          default:
            return 0;
          }
        }
        if (0x3dd2 < csvid)
        {
          switch (csvid)
          {
          case 0x3dd3:
            return 0x3dd2;
          default:
            return 0;
          case 0x3dd5:
            return 0x3dd4;
          case 0x3de2:
            return 0x3de1;
          case 0x3de5:
            return 0x3de4;
          case 0x3de8:
            return 0x3de7;
          case 0x3dea:
            return 0x3de9;
          case 0x3ded:
            return 0x3dec;
          case 0x3e04:
            return 0x3e03;
          case 0x3e06:
            return 0x3e05;
          case 0x3e08:
            return 0x3e07;
          case 0x3e1f:
            return 0x3e1e;
          case 0x3e2f:
            return 0x3e2e;
          }
        }
        if (0x3db7 < csvid)
        {
          return 0;
        }
        if (csvid < 0x3d6f)
        {
          result = 0x3b44;
          if (csvid != 0x3b45)
          {
            return (csvid != 0x3b55) - 1 & 0x3b54;
          }
        }
        else
        {
          switch (csvid)
          {
          case 0x3d6f:
            return 0x3d6e;
          default:
            return 0;
          case 0x3d71:
            result = 0x3d70;
            break;
          case 0x3d7b:
            return 0x3d7a;
          case 0x3d7d:
            return 0x3d7c;
          case 0x3d81:
            return 0x3d80;
          case 0x3d84:
            return 0x3d83;
          case 0x3d8c:
            return 0x3d8b;
          case 0x3d9b:
            return 0x3d9a;
          case 0x3da4:
            return 0x3da3;
          case 0x3db4:
            return 0x3db3;
          case 0x3db7:
            return 0x3db6;
          }
        }
      }
    }
    else if (csvid < 0x5562)
    {
      if (0x5501 < csvid)
      {
        switch (csvid)
        {
        case 0x5502:
          return 0x5501;
        default:
          return 0;
        case 0x5506:
          return 0x5505;
        case 0x5514:
          return 0x5513;
        case 0x5517:
          return 0x5516;
        case 0x5519:
          return 0x5518;
        case 0x551e:
          return 0x551d;
        case 0x5520:
          return 0x551f;
        case 0x552d:
          return 0x552c;
        case 0x5537:
          return 0x5536;
        case 0x5545:
          return 0x5544;
        case 0x5557:
          return 0x5556;
        case 0x5561:
          return 0x5560;
        }
      }
      if (csvid == 0x4431)
      {
        return 0x4430;
      }
      if (csvid < 0x4432)
      {
        if (csvid < 0x3f6a)
        {
          if (0x3f35 < csvid)
          {
            switch (csvid)
            {
            case 0x3f36:
              return 0x3f35;
            default:
              return 0;
            case 0x3f3c:
              return 0x3f3b;
            case 0x3f55:
              return 0x3f54;
            case 0x3f5f:
              return 0x3f5e;
            case 0x3f61:
              return 0x3f60;
            case 0x3f67:
              return 0x3f66;
            case 0x3f69:
              return 0x3f68;
            }
          }
          if (csvid != 0x3f0b)
          {
            if (csvid == 0x3f12)
            {
              return 0x3f11;
            }
            return (csvid != 0x3ef8) - 1 & 0x3ef7;
          }
          return 0x3f0a;
        }
        if (csvid == 0x43f6)
        {
          return 0x43f5;
        }
        if (0x43f6 < csvid)
        {
          return (csvid != 0x4413) - 1 & 0x4412;
        }
        result = 0x43c0;
        if (csvid != 0x43c1)
        {
          return (csvid != 0x43dd) - 1 & 0x43dc;
        }
      }
      else
      {
        if (csvid == 0x472c)
        {
          return 0x472b;
        }
        if (csvid < 0x472d)
        {
          if (csvid == 0x44a1)
          {
            return 0x44a0;
          }
          if (csvid < 0x44a2)
          {
            if (csvid == 0x446d)
            {
              return 0x446c;
            }
            if (csvid == 0x4498)
            {
              return 0x4497;
            }
            return (csvid != 0x444b) - 1 & 0x444a;
          }
          if (csvid == 0x46f1)
          {
            return 0x46f0;
          }
          if (csvid == 0x4728)
          {
            return 0x4727;
          }
          return (csvid != 0x44a7) - 1 & 0x44a6;
        }
        if (csvid == 0x477e)
        {
          return 0x477d;
        }
        if (csvid < 0x477f)
        {
          if (csvid == 0x4754)
          {
            return 0x4753;
          }
          if (csvid == 0x475a)
          {
            return 0x4759;
          }
          return (csvid != 0x4738) - 1 & 0x4737;
        }
        result = 0x4a19;
        if (csvid != 0x4a1a)
        {
          return (csvid != 0x4a75) - 1 & 0x4a74;
        }
      }
    }
    else if (csvid < 0x562b)
    {
      result = 0;
      if (0x556b < csvid)
      {
        switch (csvid)
        {
        case 0x556c:
          return 0x556b;
        default:
          return 0;
        case 0x556f:
          return 0x556e;
        case 0x5574:
          return 0x5573;
        case 0x5578:
          return 0x5577;
        case 0x557a:
          return 0x5579;
        case 0x558c:
          return 0x558b;
        case 0x558f:
          return 0x558e;
        case 0x55a4:
          return 0x55a3;
        case 0x55b0:
          return 0x55af;
        case 0x55b5:
          return 0x55b4;
        case 0x55c4:
          return 0x55c3;
        case 0x55d4:
          return 0x55d3;
        case 0x55d8:
          return 0x55d7;
        case 0x55de:
          return 0x55dd;
        case 0x55e2:
          return 0x55e1;
        case 0x55e6:
          return 0x55e5;
        case 0x55f2:
          return 0x55f1;
        case 0x55f6:
          return 0x55f5;
        case 0x5605:
          return 0x5604;
        case 0x5615:
          return 0x5614;
        case 0x5618:
          return 0x5617;
        case 0x561b:
          return 0x561a;
        case 0x561e:
          return 0x561d;
        case 0x562a:
          return 0x5629;
        }
      }
    }
    else
    {
      if (csvid == 0x5707)
      {
        return 0x5706;
      }
      if (csvid < 0x5708)
      {
        if (csvid == 0x56f6)
        {
          return 0x56f5;
        }
        if (csvid == 0x5704)
        {
          return 0x5703;
        }
        return (csvid != 0x56e8) - 1 & 0x56e7;
      }
      result = 0x5719;
      if (csvid != 0x571a)
      {
        return (csvid != 0x5722) - 1 & 0x5721;
      }
    }
  }
  else
  {
    if (0x5fc3 < csvid)
    {
      return 0;
    }
    if (0x5bfc < csvid)
    {
      switch (csvid)
      {
      case 0x5bfd:
        return 0x5bfc;
      default:
        return 0;
      case 0x5c02:
        return 0x5c01;
      case 0x5c06:
        return 0x5c05;
      case 0x5c08:
        return 0x5c07;
      case 0x5c0b:
        return 0x5c0a;
      case 0x5c0d:
        return 0x5c0c;
      case 0x5c0f:
        return 0x5c0e;
      case 0x5c1d:
        return 0x5c1c;
      case 0x5c1f:
        return 0x5c1e;
      case 0x5c2c:
        return 0x5c2b;
      case 0x5c34:
        return 0x5c33;
      case 0x5c36:
        return 0x5c35;
      case 0x5c3b:
        return 0x5c3a;
      case 0x5c43:
        return 0x5c42;
      case 0x5c45:
        return 0x5c44;
      case 0x5c49:
        return 0x5c48;
      case 0x5c4c:
        return 0x5c4b;
      case 0x5c58:
        return 0x5c57;
      case 0x5c5b:
        return 0x5c5a;
      case 0x5c63:
        return 0x5c62;
      case 0x5c68:
        return 0x5c67;
      case 0x5c6f:
        return 0x5c6e;
      case 0x5c7b:
        return 0x5c7a;
      case 0x5c7f:
        return 0x5c7e;
      case 0x5c84:
        return 0x5c83;
      case 0x5c88:
        return 0x5c87;
      case 0x5c8a:
        return 0x5c89;
      case 0x5c92:
        return 0x5c91;
      case 0x5c96:
        return 0x5c95;
      case 0x5ca0:
        return 0x5c9f;
      case 0x5ca3:
        return 0x5ca2;
      case 0x5ca5:
        return 0x5ca4;
      case 0x5cb0:
        return 0x5caf;
      case 0x5cb2:
        return 0x5cb1;
      case 0x5cb4:
        return 0x5cb3;
      case 0x5cc2:
        return 0x5cc1;
      case 0x5cc5:
        return 0x5cc4;
      case 0x5ccd:
        return 0x5ccc;
      case 0x5cd2:
        return 0x5cd1;
      case 0x5ce4:
        return 0x5ce3;
      case 0x5cf2:
        return 0x5cf1;
      case 0x5cf7:
        return 0x5cf6;
      case 0x5d04:
        return 0x5d03;
      case 0x5d08:
        return 0x5d07;
      case 0x5d0f:
        return 0x5d0e;
      case 0x5d11:
        return 0x5d10;
      case 0x5d13:
        return 0x5d12;
      case 0x5d1d:
        return 0x5d1c;
      case 0x5d1f:
        return 0x5d1e;
      case 0x5d51:
        return 0x5d50;
      case 0x5d5b:
        return 0x5d5a;
      case 0x5d5d:
        return 0x5d5c;
      case 0x5d60:
        return 0x5d5f;
      case 0x5d62:
        return 0x5d61;
      case 0x5d6b:
        return 0x5d6a;
      case 0x5d73:
        return 0x5d72;
      case 0x5d77:
        return 0x5d76;
      case 0x5d82:
        return 0x5d81;
      case 0x5d88:
        return 0x5d87;
      case 0x5d97:
        return 0x5d96;
      case 0x5d9b:
        return 0x5d9a;
      case 0x5da0:
        return 0x5d9f;
      case 0x5da2:
        return 0x5da1;
      case 0x5daa:
        return 0x5da9;
      case 0x5dae:
        return 0x5dad;
      case 0x5db3:
        return 0x5db2;
      case 0x5db8:
        return 0x5db7;
      case 23999:
        return 0x5dbe;
      case 0x5dcd:
        return 0x5dcc;
      case 0x5dd3:
        return 0x5dd2;
      case 0x5dda:
        return 0x5dd9;
      case 0x5dde:
        return 0x5ddd;
      case 0x5de2:
        return 0x5de1;
      case 0x5de4:
        return 0x5de3;
      case 0x5de6:
        return 0x5de5;
      case 0x5deb:
        return 0x5dea;
      case 0x5ded:
        return 0x5dec;
      case 0x5df2:
        return 0x5df1;
      case 0x5df6:
        return 0x5df5;
      case 0x5df8:
        return 0x5df7;
      case 0x5e06:
        return 0x5e05;
      case 0x5e08:
        return 0x5e07;
      case 0x5e0f:
        return 0x5e0e;
      case 0x5e18:
        return 0x5e17;
      case 0x5e22:
        return 0x5e21;
      case 0x5e24:
        return 0x5e23;
      case 0x5e2b:
        return 0x5e2a;
      case 0x5e30:
        return 0x5e2f;
      case 0x5e45:
        return 0x5e44;
      case 0x5e58:
        return 0x5e57;
      case 0x5e5a:
        return 0x5e59;
      case 0x5e65:
        return 0x5e64;
      case 0x5e67:
        return 0x5e66;
      case 0x5e6b:
        return 0x5e6a;
      case 0x5e70:
        return 0x5e6f;
      case 0x5e72:
        return 0x5e71;
      case 0x5e74:
        return 0x5e73;
      case 0x5e7e:
        return 0x5e7d;
      case 0x5eb5:
        return 0x5eb4;
      case 0x5eba:
        return 0x5eb9;
      case 0x5ebc:
        return 0x5ebb;
      case 0x5ec4:
        return 0x5ec3;
      case 0x5ee6:
        return 0x5ee5;
      case 0x5eeb:
        return 0x5eea;
      case 0x5f07:
        return 0x5f06;
      case 0x5f14:
        return 0x5f13;
      case 0x5f20:
        return 0x5f1f;
      case 0x5f34:
        return 0x5f33;
      case 0x5f3a:
        return 0x5f39;
      case 0x5f4b:
        return 0x5f4a;
      case 0x5f63:
        return 0x5f62;
      case 0x5fa0:
        return 0x5f9f;
      case 0x5fa8:
        return 0x5fa7;
      case 0x5fad:
        return 0x5fac;
      case 0x5faf:
        return 0x5fae;
      case 0x5fb1:
        return 0x5fb0;
      case 0x5fb3:
        return 0x5fb2;
      case 0x5fb5:
        return 0x5fb4;
      case 0x5fb8:
        return 0x5fb7;
      case 0x5fba:
        return 0x5fb9;
      case 0x5fc3:
        return 0x5fc2;
      }
    }
    if (csvid == 0x588b)
    {
      return 0x588a;
    }
    if (csvid < 0x588c)
    {
      if (csvid == 0x57f3)
      {
        return 0x57f2;
      }
      if (csvid < 0x57f4)
      {
        if (csvid == 0x5786)
        {
          return 0x5785;
        }
        if (csvid < 0x5787)
        {
          if (csvid == 0x576e)
          {
            return 0x576d;
          }
          if (csvid == 0x5773)
          {
            return 0x5772;
          }
          return (csvid != 0x5744) - 1 & 0x5743;
        }
        result = 0x5791;
        if (csvid != 0x5792)
        {
          return (csvid != 0x57bc) - 1 & 0x57bb;
        }
      }
      else
      {
        if (csvid == 0x583a)
        {
          return 0x5839;
        }
        if (csvid < 0x583b)
        {
          result = 0x57fd;
          if (csvid != 0x57fe)
          {
            return (csvid != 0x580a) - 1 & 0x5809;
          }
        }
        else
        {
          result = 0x5844;
          if (csvid != 0x5845)
          {
            return (csvid != 0x5885) - 1 & 0x5884;
          }
        }
      }
    }
    else
    {
      if (csvid == 0x5909)
      {
        return 0x5908;
      }
      if (csvid < 0x590a)
      {
        if (csvid == 0x58da)
        {
          return 0x58d9;
        }
        if (csvid < 0x58db)
        {
          if (csvid == 0x58b1)
          {
            return 0x58b0;
          }
          if (csvid == 0x58cc)
          {
            return 0x58cb;
          }
          return (csvid != 0x58ac) - 1 & 0x58ab;
        }
        result = 0x58df;
        if (csvid != 0x58e0)
        {
          return (csvid != 0x5902) - 1 & 0x5901;
        }
      }
      else
      {
        if (csvid == 0x5941)
        {
          return 0x5940;
        }
        if (csvid < 0x5942)
        {
          result = 0x590e;
          if (csvid != 0x590f)
          {
            return (csvid != 0x593c) - 1 & 0x593b;
          }
        }
        else
        {
          result = 0x5942;
          if (csvid != 0x5943)
          {
            return (csvid != 0x594b) - 1 & 0x594a;
          }
        }
      }
    }
  }
  return result;
}

/* ManalinkEh 2022: 0x267eed0; original functions/functions.c:5054. */
int grafdiggers_cages(int player, type_t type, tutor_from_t from)
{
  card_instance_t *instance;
  int result;
  int card;
  int player_00;

  player_00 = 0;

  do
  {
    if (0 < active_cards_count[player_00])
    {
      card = 0;
      do
      {
        if ((((player == 2 || player == player_00) &&
              (instance = in_play(player_00, card), instance != NULL)) &&
             (result = is_humiliated(player_00, card, AL_STATIC), result == 0)) &&
            (((type & (TYPE_SPELL | TYPE_CREATURE)) != TYPE_NONE &&
              ((((from & TUTOR_FROM_RFG) != 0 &&
                 (result = get_id(player_00, card), result == 0x2f9e)) ||
                (((from & TUTOR_FROM_GRAVE) != 0 &&
                  (result = get_id(player_00, card), result == 0x523d))))))))
        {
          return 1;
        }
        card = card + 1;
      } while (card < active_cards_count[player_00]);
    }
    player_00 = player_00 + 1;
    if (player_00 == 2)
    {
      return 0;
    }
  } while (true);
}

/* ManalinkEh 2022: 0x26817d0; original functions/functions.c:7590. */
int count_basic_lands(int player, int subtype)
{
  card_instance_t *instance;
  int result;
  int card;
  int player_00;
  int iStack_14;

  iStack_14 = 0;
  player_00 = 0;
  do
  {

    if (((player == player_00) || (player == 2)) && (0 < active_cards_count[player_00]))
    {
      card = 0;
      do
      {
        instance = in_play(player_00, card);
        if (((instance != NULL) && (result = is_basic_land(player_00, card), result != 0)) &&
            ((subtype < 0 || (result = has_subtype(player_00, card, subtype), result != 0))))
        {
          iStack_14 = iStack_14 + 1;
        }
        card = card + 1;
      } while (card < active_cards_count[player_00]);
    }
    player_00 = player_00 + 1;
  } while (player_00 != 2);
  return iStack_14;
}

/* ManalinkEh 2022: 0x268d9a0; original functions/functions.c:12547. */
void dat_ability(int player, int card, event_t event, declare_attackers_trigger_t mode,
                 int skip_store_attackers)
{
  int iVar1;
  card_instance_t *instance;
  int iVar3;

  if (skip_store_attackers == 0)
  {
    store_attackers(player, card, event, mode, player, card, NULL);
  }
  iVar1 = resolve_declare_attackers_trigger(player, card, event, mode);
  if (iVar1 != 0)
  {

    if ((mode & DAT_DONT_REPORT_VALUE) == 0)
    {
      iVar3 = is_what(player, card, 0x80);
      if (iVar3 == 0)
      {
        instance = get_card_instance(player, card);
        instance->info_slot = iVar1;
      }
      else
      {
        instance = get_card_instance(player, card);
        instance->targets[1].player = iVar1;
      }
    }
    iVar1 = count_cards_by_id(player, 0x5bf9);
    for (iVar1 = iVar1 + 1; iVar1 != 0; iVar1 = iVar1 + -1)
    {
      call_card_function(player, card, EVENT_DAT_ABILITY);
    }
  }
  if ((event == EVENT_CLEANUP) && ((mode & DAT_DONT_REPORT_VALUE) == 0))
  {

    instance = get_card_instance(player, card);
    instance->info_slot = 0;
    return;
  }
  return;
}

/* ManalinkEh 2022: 0x2691b80; original functions/functions.c:14308. */
int get_toughness_sum(int player)
{
  card_instance_t *instance;
  int result;
  int player_00;
  int card;
  int iStack_14;

  iStack_14 = 0;
  player_00 = 0;
  do
  {

    if (((player == player_00) || (player == 2)) && (0 < active_cards_count[player_00]))
    {
      card = 0;
      do
      {
        instance = in_play(player_00, card);
        if (instance != NULL)
        {
          result = is_what(player_00, card, 2);
          if (result != 0)
          {
            result = get_abilities(player_00, card, EVENT_TOUGHNESS, -1);
            iStack_14 = iStack_14 + result;
          }
        }
        card = card + 1;
      } while (card < active_cards_count[player_00]);
    }
    player_00 = player_00 + 1;
  } while (player_00 != 2);
  return iStack_14;
}

/* ManalinkEh 2022: 0x26939a0; original functions/functions.c:15661. */
int extra_combat_effect(int player, int card, event_t event, int untap,
                        test_definition_t *this_test, int can_repeat)
{
  card_instance_t *instance;
  int iVar2;
  int iVar3;
  bool bVar4;
  test_definition_t default_test;

  if (untap != 0)
  {
    if (this_test == NULL)
    {
      this_test = &default_test;
      default_test_definition(this_test, 2);
      iVar3 = player;
    }
    else
    {
      iVar3 = 2;
    }
    new_manipulate_all(player, card, iVar3, this_test, ACT_UNTAP);
  }
  if (0 < active_cards_count[player])
  {
    bVar4 = false;
    iVar3 = 0;
    do
    {
      while (((instance = in_play(player, iVar3),
               instance == NULL || (iVar2 = is_what(player, iVar3, 0x80), iVar2 == 0)) ||
              (instance = get_card_instance(player, iVar3),
               instance->info_slot_fn != extra_combat_legacy)))
      {
        iVar3 = iVar3 + 1;
        if (active_cards_count[player] <= iVar3)
          goto LAB_02693a68;
      }
      if (can_repeat != 0)
      {
        instance = get_card_instance(player, iVar3);
        instance->targets[1].player = instance->targets[1].player + 1;
        if ((current_phase == 0x1e) &&
            (instance = get_card_instance(player, iVar3), 0x41 < instance->targets[1].player))
        {
          instance = get_card_instance(player, iVar3);
          land_can_be_played = land_can_be_played & 0xfffffff7;
          current_phase = 0x15;
          instance->targets[1].player = instance->targets[1].player + -1;
        }
      }
      iVar3 = iVar3 + 1;
      bVar4 = true;
    } while (iVar3 < active_cards_count[player]);
    bVar4 = true;
  LAB_02693a68:
    if (bVar4)
    {
      return 0;
    }
  }

  iVar3 = create_legacy_effect(player, card, extra_combat_legacy);
  if (-1 < iVar3)
  {
    set_legacy_image(player, 0x469, iVar3);
  }
  instance = get_card_instance(player, iVar3);
  instance->targets[1].player = 0x42;
  iVar2 = get_id(player, card);
  bVar4 = current_phase != 0x1e;
  instance->targets[1].card = iVar2;
  if (bVar4)
  {
    return iVar3;
  }
  instance->targets[1].player = instance->targets[1].player + -1;
  current_phase = 0x15;
  land_can_be_played = land_can_be_played & 0xfffffff7;
  return iVar3;
}

/* ManalinkEh 2022: 0x26780e0; original functions/functions.c:19052. */
static int count_harmonic_prodigy(int player, int card)
{
  card_instance_t *instance;
  int result;
  int card_00;
  int iStack_14;

  if (active_cards_count[player] < 1)
  {
    return 0;
  }
  iStack_14 = 0;
  card_00 = 0;
  do
  {
    while (((instance = in_play(player, card_00), card != card_00 && (instance != NULL)) &&
            (result = is_humiliated(player, card_00, AL_TRIG), result == 0)))
    {
      result = get_id(player, card_00);
      card_00 = card_00 + 1;
      iStack_14 = iStack_14 + (unsigned int)(result == 0x5a16);
      if (active_cards_count[player] <= card_00)
      {
        return iStack_14;
      }
    }
    card_00 = card_00 + 1;
  } while (card_00 < active_cards_count[player]);
  return iStack_14;
}

/* Recovered October 2022 implementations; see docs/recovery-progress.md. */
int pump_until_eot_merge_previous_alt(int player, int card, int t_player, int t_card, int power,
                                      int toughness, int alternate_txt);

#ifndef SHANDALAR
static int effect_pump_until_eot(int player, int card, event_t event);
static int pump_ability_until_eot_with_precomputed_sleight_hack_suppress(
    int player, int card, int t_player, int t_card, pump_ability_t *pump);

/* ManalinkEh 2022: 0x267ac00; original functions/functions.c:1247. */
int pump_until_eot_merge_previous_alt(int player, int card, int t_player, int t_card, int power,
                                      int toughness, int alternate_txt)
{
  uint16_t uVar1;
  card_instance_t *instance;
  int32_t iVar3;
  int iVar4;
  int iStack_14;

  instance = get_card_instance(player, card);
  if (instance->internal_card_id == activation_card)
  {
    iVar3 = instance->parent_controller;
    iStack_14 = instance->parent_card;
  }
  else
  {
    iStack_14 = card;
    iVar3 = player;
  }
  if (0 < active_cards_count[player])
  {
    iVar4 = 0;
    do
    {
      instance = in_play(player, iVar4);
      if (((((instance != NULL) && (instance->internal_card_id == LEGACY_EFFECT_CUSTOM)) &&
            (instance->info_slot_fn == effect_pump_until_eot)) &&
           ((instance->damage_source_player == iVar3 &&
             (instance->damage_source_card == iStack_14)))) &&
          ((instance->damage_target_player == t_player &&
            ((instance->damage_target_card == t_card &&
              (instance->eot_toughness == alternate_txt))))))
      {
        uVar1 = instance->counter_toughness;
        instance->counter_power = instance->counter_power + (short)power;
        instance->counter_toughness = uVar1 + (short)toughness;
        return iVar4;
      }
      iVar4 = iVar4 + 1;
    } while (iVar4 < active_cards_count[player]);
  }
  iVar4 = pump_until_eot(player, card, t_player, t_card, power, toughness);
  instance = get_card_instance(player, iVar4);
  instance->eot_toughness = alternate_txt;
  return iVar4;
}

#endif

/* Recovered October 2022 implementations; see docs/recovery-progress.md. */
int create_legacy_activate_for_opponent(int player, int card, CardFunction func_ptr, int t_player,
                                        int t_card);

/* ManalinkEh 2022: 0x267b020; original functions/functions.c:2027. */
int find_legacy_effect(int player, int card, CardFunction func_ptr)
{
  card_instance_t *instance;
  int card_00;

  if (0 < active_cards_count[player])
  {
    card_00 = 0;
    do
    {
      while ((((instance = in_play(player, card_00),
                instance == NULL || (instance->internal_card_id != LEGACY_EFFECT_CUSTOM)) ||
               (func_ptr != (CardFunction)instance->info_slot)) ||
              (instance->damage_source_player != player)))
      {
        card_00 = card_00 + 1;
        if (active_cards_count[player] <= card_00)
        {
          return -1;
        }
      }
      if (instance->damage_source_card == card)
      {
        return card_00;
      }
      card_00 = card_00 + 1;
    } while (card_00 < active_cards_count[player]);
  }
  return -1;
}

/* ManalinkEh 2022: 0x267b5a0; original functions/functions.c:2135. */
int create_legacy_activate_for_opponent(int player, int card, CardFunction func_ptr, int t_player,
                                        int t_card)
{
  card_instance_t *pcVar1;
  int result;
  card_instance_t *argument2;
  int card_00;
  int player_00;

  pcVar1 = get_card_instance(player, card);
  player_00 = 1 - player;
  result = add_card_to_hand(player_00, pcVar1->internal_card_id);
  if (result == -1)
  {
    backtrace_and_abort("functions/functions.c:2138\ncard_added != -1");
  }
  pcVar1 = get_card_instance(player_00, result);
  pcVar1->state = pcVar1->state & 0xffffffdd | 0x20;
  hand_count[player_00] = hand_count[player_00] + -1;
  argument2 = get_card_instance(player, card);
  memcpy(pcVar1, argument2, 300);
  pcVar1->state = pcVar1->state & 0xffffffdd | 0x20;
  card_00 = create_legacy_activate(player_00, result, func_ptr);
  obliterate_card(player_00, result);
  pcVar1 = get_card_instance(player_00, card_00);
  pcVar1->damage_source_player = (int8_t)player;
  pcVar1->damage_source_card = card;
  result = get_card_image_number((unsigned int)pcVar1->display_pic_csv_id, player, card);
  pcVar1->display_pic_num = (uint16_t)result;
  if (-1 < t_player)
  {
    pcVar1->number_of_targets = '\x01';
    pcVar1->targets[0].player = t_player;
    pcVar1->targets[0].card = t_card;
  }
  return card_00;
}

/* Retained legacy spell-trigger interface: sp_ability_trigger_impl @ 0x2684f40. */
/* Copied-spell flag read adapted to the existing source accessor.
 * The target stores this flag in card_ext_t at offset 184. */

/* End retained legacy spell-trigger interface: 2684f40. */

/* Focused October 2022 helpers using existing source flag storage. */

/* Legacy compatibility accessor: original target-slot storage.
 * The recovered extension-backed getter is below. */


/* ManalinkEh 2022: 0x2681710; original functions/functions.c:7559. */
void enters_the_battlefield_with_counters_fn(int player, int card, event_t event, counter_t type,
                                             CardPairFunction fn)
{
  card_instance_t *instance;
  int result;
  if ((((event == EVENT_CAST_SPELL) || (event == EVENT_ENTERING_THE_BATTLEFIELD_AS_CLONE)) &&
       (card == affected_card)) &&
      (player == affected_card_controller))
  {
    instance = get_card_instance(player, card);
    if ((get_special_flags2_by_instance(instance) & SF2_FACE_DOWN_DUE_TO_MANIFEST) == 0)
    {
      result = is_humiliated(player, card, AL_STATIC);
      if (result == 0)
      {
        hack_silent_counters = hack_silent_counters + 1;
        result = (*fn)(player, card);
        add_counters(player, card, player, card, type, result);
        hack_silent_counters = hack_silent_counters + -1;
        return;
      }
    }
  }
  return;
}

/* ManalinkEh 2022: 0x26955c0; original functions/functions.c:5075. */
void put_into_play_transformed(int player, int card)
{
  card_instance_t *pcVar1;
  int result;
  card_instance_t *pcVar3;

  pcVar1 = get_card_instance(player, card);
  if (pcVar1 != NULL)
  {
    result = get_id(player, card);
    pcVar1->targets[0xd].card = result;
    pcVar1->targets[0xd].player = result;
    true_transform(player, card);
    pcVar1->internal_card_id = pcVar1->targets[0xc].card;
    pcVar3 = get_card_instance(player, card);
    if ((get_special_flags3_by_instance(pcVar3) & SF3_CARD_IS_FLIPPED) == 0)
    {
      set_special_flags3(player, card, 0x200);
    }
    else
    {
      remove_special_flags3(player, card, 0x200);
    }
    put_into_play(player, card);
    pcVar1->regen_status = pcVar1->regen_status | 0x1f000000;
    get_abilities(player, card, EVENT_CHANGE_TYPE, -1);
    get_abilities(player, card, EVENT_SET_COLOR, -1);
    result = is_what(player, card, 2);
    if (result != 0)
    {
      get_abilities(player, card, EVENT_POWER, -1);
      get_abilities(player, card, EVENT_TOUGHNESS, -1);
    }
    verify_legend_rule(player, card, (unsigned int)cards_data[pcVar1->targets[0xc].card].id);
  }
  return;
}

/* Recovered October 2022 implementations; see docs/recovery-progress.md. */
void sp_ability_trigger(int controller_of_trigger, event_t event, int trigger_mode, type_t type,
                        test_definition_t *test, int triggering_player, int triggering_card);
void magecraft_trigger(int controller_of_trigger, event_t event, int trigger_mode, type_t type,
                       test_definition_t *test, int triggering_player, int triggering_card);

/* Retained legacy spell-trigger interface: sp_ability_trigger @ 0x26a8120. */

/* End retained legacy spell-trigger interface: 26a8120. */

/* ManalinkEh 2022: 0x26a8160; original functions/functions.c:19968. */
void magecraft_trigger(int controller_of_trigger, event_t event, int trigger_mode, type_t type,
                       test_definition_t *test, int triggering_player, int triggering_card)
{
  sp_ability_trigger_impl(controller_of_trigger, event, trigger_mode, type, test, triggering_player,
                          triggering_card, 1);
  return;
}

/* Recovered October 2022 implementations; see docs/recovery-progress.md. */
int can_counter_triggered_ability(int player, int card, event_t event, target_definition_t *td);

/* Retained legacy target-layout compatibility body for 0x2693060. */


/* ManalinkEh 2022: 0x267b0a0; original functions/functions.c:2043. */
target_t find_repeat_legacy_effect_alt(int player, int card, CardFunction func_ptr,
                                       int target_player_id, int target_card,
                                       int alternate_legacy_text, int whose_control)
{
  card_instance_t *instance;
  int card_00;
  int player_00;
  target_t tVar2;

  instance = get_card_instance(player, card);
  if (instance->internal_card_id == activation_card)
  {
    player = instance->parent_controller;
    card = instance->parent_card;
  }
  player_00 = 0;
  do
  {
    if (((whose_control == player_00) || (whose_control == 2)) &&
        (0 < active_cards_count[player_00]))
    {
      card_00 = 0;
      do
      {
        instance = in_play(player_00, card_00);
        if (((((instance != NULL) && (instance->internal_card_id == LEGACY_EFFECT_CUSTOM)) &&
              ((func_ptr == (CardFunction)instance->info_slot &&
                ((instance->damage_source_player == player &&
                  (instance->damage_source_card == card)))))) &&
             (instance->damage_target_player == target_player_id)) &&
            ((instance->damage_target_card == target_card &&
              ((alternate_legacy_text < 0 || (instance->eot_toughness == alternate_legacy_text))))))
        {
          new_timestamp(player_00, card_00);
          goto LAB_0267b0f8;
        }
        card_00 = card_00 + 1;
      } while (card_00 < active_cards_count[player_00]);
    }
    player_00 = player_00 + 1;
    if (player_00 == 2)
    {
      player_00 = -1;
      card_00 = -1;
    LAB_0267b0f8:
      tVar2.card = card_00;
      tVar2.player = player_00;
      return tVar2;
    }
  } while (true);
}

#ifndef SHANDALAR
/* ManalinkEh 2022: 0x267b1b0; original functions/functions.c:1580. */
int pump_ability_until_eot_no_repeat_alt(int player, int card, int t_player, int t_card,
                                         int ability, int sp_ability,
                                         int alternate_legacy_text_number)
{
  card_instance_t *instance;
  int card_00;
  unsigned int uVar2;
  target_t tVar3;
  pump_ability_t pump;

  get_sleighted_hacked_suppressed_ability(player, card, t_player, &ability, &sp_ability);
  instance = get_card_instance(t_player, t_card);
  if ((ability == (instance->regen_status & ability)) &&
      (uVar2 = get_special_abilities_by_instance(instance),
       sp_ability == (uVar2 & sp_ability)))
  {
    tVar3 = find_repeat_legacy_effect_alt(player, card, legacy_effect_pump_ability_until_eot,
                                          t_player, t_card, alternate_legacy_text_number, player);
    if (tVar3.card != -1)
    {
      return tVar3.card;
    }
  }
  pump.paue_flags = ((sp_ability ^ 0xffffffffU) >> 0x1f) << 9;
  pump.source_c = card;
  pump.key_mod = ability;
  pump.source_p = player;
  pump.pow_mod = 0;
  pump.tou_mod = 0;
  pump.can_block_additional_creatures = 0;
  pump.eot_removal_method = 0;
  pump.subtype = SUBTYPE_INVALID;
  pump.color = COLOR_TEST_0;
  pump.disabling_mode = 0;
  pump.skey_mod = sp_ability;
  card_00 = pump_ability_until_eot_with_precomputed_sleight_hack_suppress(
      player, card, t_player, t_card, &pump);
  if ((0 < alternate_legacy_text_number) && (-1 < card_00))
  {
    instance = get_card_instance(player, card_00);
    instance->eot_toughness = alternate_legacy_text_number;
    return card_00;
  }
  return card_00;
}

#endif

/* ManalinkEh 2022: 0x26a2080; original functions/functions.c:743. */
int leaves_play(int player, int card, event_t event, int can_be_suppressed,
                     int can_be_duplicated)
{
  int iVar1;
  int iVar2;
  int iVar3;
  if (trigger_condition == 0xd4)
  {
    iVar1 = is_humiliated(player, card, AL_TRIG);
    if ((((iVar1 == 0) || (can_be_suppressed == 0)) && (card == affected_card)) &&
        (((player == affected_card_controller && (trigger_cause_controller == player)) &&
          ((trigger_cause == card && (reason_for_trigger_controller == player))))))
    {
      iVar1 = is_this_dying(player, card);
      if ((iVar1 == 0) ||
          ((iVar1 = check_for_death_trigger_removal(player, card),
            iVar1 == 0 && ((((unsigned int)player_bits[player] >> 24 & 0xff) & 2) == 0))))
      {
        if (event != EVENT_TRIGGER)
        {
          if (event != EVENT_RESOLVE_TRIGGER)
          {
            return 0;
          }
          if (can_be_suppressed == 0)
          {
            return 0;
          }
          if (((can_be_duplicated == 0) ||
               (iVar1 = is_what(trigger_cause_controller, trigger_cause, 2), iVar1 == 0)) ||
              (iVar1 = is_this_dying(trigger_cause_controller, trigger_cause), iVar1 == 0))
          {
            iVar3 = 1;
            iVar1 = iVar3;
          }
          else
          {
            iVar3 = card_counts[player][5] + 1;
            iVar1 = iVar3;
          }
          do
          {
            iVar2 = is_what(player, card, 0x80);
            if (iVar2 == 0)
            {
              call_card_function(player, card, EVENT_LEAVES_PLAY_ABILITY);
            }
            else
            {
              dispatch_event_with_attacker_to_one_card(player, card, EVENT_LEAVES_PLAY_ABILITY, -1,
                                                       -1);
            }
            iVar3 = iVar3 + -1;
          } while (iVar3 != 0);
          return iVar1;
        }
        if (can_be_suppressed != 0)
        {
          event_result = event_result | 2;
          return 0;
        }
      }
      else
      {
        if (can_be_suppressed != 0)
        {
          return 0;
        }
        if (event != EVENT_TRIGGER)
        {
          return 0;
        }
      }
      if (((can_be_duplicated == 0) ||
           (iVar1 = is_what(trigger_cause_controller, trigger_cause, 2), iVar1 == 0)) ||
          (iVar1 = is_this_dying(trigger_cause_controller, trigger_cause), iVar1 == 0))
      {
        iVar1 = 1;
      }
      else
      {
        iVar1 = card_counts[player][5] + 1;
      }
      do
      {
        iVar3 = is_what(player, card, 0x80);
        if (iVar3 == 0)
        {
          call_card_function(player, card, EVENT_LEAVES_PLAY_ABILITY);
        }
        else
        {
          dispatch_event_with_attacker_to_one_card(player, card, EVENT_LEAVES_PLAY_ABILITY, -1, -1);
        }
        iVar1 = iVar1 + -1;
      } while (iVar1 != 0);
    }
  }
  return 0;
}

/* ManalinkEh 2022: 0x26a2e90; original functions/functions.c:19809. */
static int end_the_turn_legacy(int player, int card, event_t event)
{
  int result;
  card_instance_t *instance;
  if (event == EVENT_PHASE_CHANGED)
  {
    if ((current_phase == 0x14) || (current_phase == 0x1e))
    {
      cant_be_responded_to = 1;
      instance = get_card_instance(player, card);
      if (instance->targets[6].card != 1)
      {
        current_phase = 0x1f;
        instance = get_card_instance(player, card);
        instance->targets[6].card = 1;
      }
    }
    suppress_draw = 1;
    land_can_be_played = land_can_be_played | 1;
    lands_played = 99;
  }
  else
  {
    suppress_draw = 1;
    land_can_be_played = land_can_be_played | 1;
    lands_played = 99;
    if (event == EVENT_STATIC_EFFECTS)
    {
      player_bits[0] = player_bits[0] | 0x3000000;
      player_bits[1] = player_bits[1] | 0x3000000;
    }
    else if (event == EVENT_ABILITIES)
    {
      result = is_what(affected_card_controller, affected_card, 0x47);
      if (result != 0)
      {
        instance = get_card_instance(affected_card_controller, affected_card);
        instance->targets[0x11].card = instance->targets[0x11].card | 0x7e;
      }
    }
  }
  leaves_play(player, card, event, 0, 0);
  result = eot_trigger(player, card, event);
  if ((event == EVENT_LEAVES_PLAY_ABILITY) || (result != 0))
  {
    cant_be_responded_to = 1;
    instance = in_play(player, card);
    if (instance != NULL)
    {
      kill_card(player, card, KILL_EXILE);
      return 0;
    }
  }
  return 0;
}

/* Recovered October 2022 implementations; see docs/recovery-progress.md. */
int attached_leaves_play(int triggering_player, int triggering_card, event_t event,
                         int can_be_suppressed, int can_be_duplicated);
int test_leaves_play(int triggering_player, int triggering_card, event_t event, int controller,
                     test_definition_t *test, int can_be_suppressed, int can_be_duplicated);
int other_leaves_play_mode(int triggering_player, int triggering_card, int t_player, int t_card,
                           event_t event, resolve_trigger_t mode, int can_be_suppressed,
                           int can_be_duplicated);
int attached_leaves_play_mode(int triggering_player, int triggering_card, event_t event,
                              resolve_trigger_t mode, int can_be_suppressed, int can_be_duplicated);
int test_leaves_play_mode(int triggering_player, int triggering_card, event_t event, int controller,
                          resolve_trigger_t mode, test_definition_t *test, int can_be_suppressed,
                          int can_be_duplicated);
int end_the_turn(int player, int card, event_t event);

/* ManalinkEh 2022: 0x26a33f0; original functions/functions.c:799. */
int other_leaves_play_mode(int triggering_player, int triggering_card, int t_player, int t_card,
                           event_t event, resolve_trigger_t mode, int can_be_suppressed,
                           int can_be_duplicated)
{
  int iVar1;
  int iVar2;
  int iVar3;

  if (((((trigger_condition == 0xd4) && (triggering_card == affected_card)) &&
        (triggering_player == affected_card_controller)) &&
       ((trigger_cause == t_card && (trigger_cause_controller == t_player)))) &&
      (reason_for_trigger_controller == triggering_player))
  {
    iVar1 = is_humiliated(triggering_player, triggering_card, AL_TRIG);
    if (((iVar1 == 0) || (can_be_suppressed == 0)) &&
        ((iVar1 = is_this_dying(t_player, t_card),
          iVar1 == 0 || ((iVar1 = check_for_death_trigger_removal(t_player, t_card),
                          can_be_suppressed == 0 || (iVar1 == 0))))))
    {
      if (event == EVENT_TRIGGER)
      {
        if (can_be_suppressed == 0)
        {

          if (((can_be_duplicated == 0) ||
               (iVar1 = is_what(trigger_cause_controller, trigger_cause, 2), iVar1 == 0)) ||
              (iVar1 = is_this_dying(trigger_cause_controller, trigger_cause), iVar1 == 0))
          {
            iVar3 = 1;
            iVar1 = iVar3;
          }
          else
          {
            iVar3 = card_counts[triggering_player][5] + 1;
            iVar1 = iVar3;
          }
          do
          {
            iVar2 = is_what(triggering_player, triggering_card, 0x80);
            if (iVar2 == 0)
            {
              call_card_function(triggering_player, triggering_card, EVENT_LEAVES_PLAY_ABILITY);
            }
            iVar3 = iVar3 + -1;
          } while (iVar3 != 0);
          return iVar1;
        }
        event_result = event_result | mode;
        return 0;
      }

      if ((event == EVENT_RESOLVE_TRIGGER) && (can_be_suppressed != 0))
      {
        if ((can_be_duplicated == 0) ||
            ((iVar1 = is_what(trigger_cause_controller, trigger_cause, 2),
              iVar1 == 0 ||
                  (iVar1 = is_this_dying(trigger_cause_controller, trigger_cause), iVar1 == 0))))
        {
          iVar3 = 1;
          iVar1 = iVar3;
        }
        else
        {
          iVar3 = card_counts[triggering_player][5] + 1;
          iVar1 = iVar3;
        }
        do
        {
          iVar2 = is_what(triggering_player, triggering_card, 0x80);
          if (iVar2 == 0)
          {
            call_card_function(triggering_player, triggering_card, EVENT_LEAVES_PLAY_ABILITY);
          }
          iVar3 = iVar3 + -1;
        } while (iVar3 != 0);
        return iVar1;
      }
    }
  }
  return 0;
}

/* ManalinkEh 2022: 0x26a3680; original functions/functions.c:861. */
int attached_leaves_play_mode(int triggering_player, int triggering_card, event_t event,
                              resolve_trigger_t mode, int can_be_suppressed, int can_be_duplicated)
{
  int result;
  card_instance_t *instance;

  if (trigger_condition != 0xd4)
  {
    return 0;
  }
  if (((triggering_card == affected_card) && (triggering_player == affected_card_controller)) &&
      (reason_for_trigger_controller == triggering_player))
  {
    result = is_humiliated(triggering_player, triggering_card, AL_TRIG);
    if ((result == 0) || (can_be_suppressed == 0))
    {

      instance = get_card_instance(triggering_player, triggering_card);
      if ((instance->damage_target_card == trigger_cause) &&
          (instance->damage_target_player == trigger_cause_controller))
      {
        result = other_leaves_play_mode(
            triggering_player, triggering_card, (int)instance->damage_target_player,
            instance->damage_target_card, event, mode, can_be_suppressed, can_be_duplicated);
        return result;
      }
    }
  }
  return 0;
}

/* ManalinkEh 2022: 0x26a3750; original functions/functions.c:874. */
int attached_leaves_play(int triggering_player, int triggering_card, event_t event,
                         int can_be_suppressed, int can_be_duplicated)
{
  int result;

  result =
      attached_leaves_play_mode(triggering_player, triggering_card, event,
                                RESOLVE_TRIGGER_MANDATORY, can_be_suppressed, can_be_duplicated);
  return result;
}

/* Retained legacy leaves-play test interface: test_leaves_play_mode @ 0x26a3790. */

/* End retained legacy leaves-play test interface: 26a3790. */

/* Retained legacy leaves-play test interface: test_leaves_play @ 0x26a3a80. */

/* End retained legacy leaves-play test interface: 26a3a80. */

/* ManalinkEh 2022: 0x26a7e80; original functions/functions.c:19846. */
int end_the_turn(int player, int card, event_t event)
{
  int result;
  card_instance_t *instance;
  unsigned int player_00;
  unsigned int uStack_14;

  result = target_player_cant_cast_type(player, card, 2, 0x1fe);
  instance = get_card_instance(player, result);
  instance->targets[2].player = 0;

  instance = get_card_instance(player, result);
  instance->token_status = instance->token_status | 0x10000;
  result = target_player_cant_cast_type(player, card, 1 - player, 0x1fe);
  instance = get_card_instance(player, result);
  instance->targets[2].player = 0;

  instance = get_card_instance(player, result);
  instance->token_status = instance->token_status | 0x10000;
  if (current_phase - 0x15U < 7)
  {

    uStack_14 = 0;
    do
    {
      player_00 = uStack_14 ^ current_turn;

      if (0 < active_cards_count[player_00])
      {
        result = 0;
        do
        {

          instance = get_card_instance(player_00, result);
          if ((instance->state & 0xc) != 0)
          {
            remove_state(player_00, result, 0xc);
          }
          result = result + 1;
        } while (result < active_cards_count[player_00]);
      }
      uStack_14 = uStack_14 + 1;
    } while (uStack_14 != 2);
  }
  stack_size = 0;
  result = 0;
  do
  {
    if ((stack_cards[result].player != -1) && (stack_cards[result].card != -1))
    {

      instance = get_card_instance(stack_cards[result].player, stack_cards[result].card);
      if ((instance->state & 0x10000000) == 0)
      {
        kill_card(stack_cards[result].player, stack_cards[result].card, KILL_EXILE);
      }
      else
      {
        add_state(stack_cards[result].player, stack_cards[result].card, 0x40000000);
      }
    }
    stack_phase[result] = -1;
    result = result + 1;
  } while (result != 0x20);
  result = create_legacy_effect(player, card, end_the_turn_legacy);

  instance = get_card_instance(player, result);
  current_phase = 0x1f;
  instance->token_status = instance->token_status | 0x10000;
  return 0;
}

/* ManalinkEh 2022: 0x2697910; original functions/functions.c:17156. */
int beginning_of_combat(int player, int card, event_t event, int aff_player)
{
  int result;
  if (event == EVENT_CHANGE_TYPE)
  {
    if ((((aff_player == 2) || (current_turn == aff_player)) &&
         (result = EXE_FN(int, 0x434be0, int)(current_turn), result == 0)) &&
        (result = is_humiliated(player, card, AL_TRIG), result == 0))
    {
      event_flags = event_flags | 0x1000000;
    }
  }
  else if ((event == EVENT_BEGIN_COMBAT) &&
           (result = is_humiliated(player, card, AL_TRIG), result == 0))
  {
    return (unsigned int)(current_turn == aff_player || aff_player == 2);
  }
  return 0;
}


/* ManalinkEh 2022: 0x26a3630; original functions/functions.c:856. */
int other_leaves_play(int triggering_player, int triggering_card, int t_player, int t_card,
                           event_t event, int can_be_suppressed, int can_be_duplicated)
{
  int result;
  result = other_leaves_play_mode(triggering_player, triggering_card, t_player, t_card, event,
                                  RESOLVE_TRIGGER_MANDATORY, can_be_suppressed, can_be_duplicated);
  return result;
}

/* Recovered October 2022 implementations; see docs/recovery-progress.md. */
int query(int player, int card, iid_t iid, event_query_t quer);

/* ManalinkEh 2022: 0x26a7dd0; original functions/functions.c:19793. */
int query(int player, int card, iid_t iid, event_query_t quer)
{
  int iVar1;
  int iVar2;

  iVar1 = attacking_card;
  attacking_card = quer;
  if (iid == -1)
  {
    iVar2 = call_card_function(player, card, EVENT_QUERY);
    attacking_card = iVar1;
    return iVar2;
  }
  if ((player != -1) && (card == -1))
  {
    iVar2 = (*cards_data[iid].code_pointer)(player, 0, EVENT_QUERY);
    attacking_card = iVar1;
    return iVar2;
  }
  iVar2 = (*cards_data[iid].code_pointer)(0, 0, EVENT_QUERY);
  attacking_card = iVar1;
  return iVar2;
}

int effect_this_dies(int player, int card, event_t event);
static int this_dies_trigger_impl(int player, int card, event_t event,
                                       resolve_trigger_t trigger_mode, int who_gets_trigger,
                                       int attached, int required_kill_code);


/* ManalinkEh 2022: 0x2679600; original functions/functions.c:9717. */
int effect_this_dies(int player, int card, event_t event)
{
  card_instance_t *instance;
  int result;
  unsigned int uVar2;
  if (trigger_condition != 0xd5)
  {
    return 0;
  }
  if ((((reason_for_trigger_controller == player) && (card == affected_card)) &&
       (player == affected_card_controller)) &&
      ((((unsigned int)player_bits[player] >> 24 & 0xff) & 2) == 0))
  {
    instance = get_card_instance(player, card);
    if (event == EVENT_TRIGGER)
    {
      uVar2 = (unsigned int)instance->eot_toughness3;
      if (uVar2 == 3)
      {
        result = duh_mode(player);
        uVar2 = (result != 0) + 1;
      }
      event_result = event_result | uVar2;
    }
    else if (event == EVENT_RESOLVE_TRIGGER)
    {
      if (instance->eot_toughness3 != '\0')
      {
        instance->eot_toughness3 = '\0';
        result = get_internal_card_id_from_csv_id((unsigned int)instance->display_pic_csv_id);
        call_card_fn(cards_data[result].code_pointer, instance, player, card,
                     EVENT_RESOLVE_THIS_DIES_TRIGGER);
      }
    }
    else if (event == EVENT_END_TRIGGER)
    {
      kill_card(player, card, KILL_EXILE);
    }
  }
  return 0;
}


/* ManalinkEh 2022: 0x2686130; original functions/functions.c:9752. */
static int this_dies_trigger_impl(int player, int card, event_t event,
                                       resolve_trigger_t trigger_mode, int who_gets_trigger,
                                       int attached, int required_kill_code)
{
  card_instance_t *pcVar1;
  card_instance_t *pcVar2;
  int result;
  unsigned int uVar4;
  unsigned int uVar5;
  target_t *ptVar6;
  target_t *ptVar7;
  target_t *ptVar8;
  bool bVar9;
  unsigned char bVar10;
  int iStack_2c;
  int iStack_20;
  int iStack_1c;
  card_instance_t *pcStack_18;
  bVar10 = 0;
  if (event == EVENT_GRAVEYARD_FROM_PLAY)
  {
    if (attached == 0)
    {
      pcVar1 = in_play(player, card);
      iStack_20 = player;
      iStack_1c = card;
      pcStack_18 = pcVar1;
    }
    else
    {
      pcStack_18 = in_play(player, card);
      if (pcStack_18 == NULL)
      {
        return 0;
      }
      iStack_20 = (int)pcStack_18->damage_target_player;
      if (pcStack_18->damage_target_player < '\0')
      {
        return 0;
      }
      iStack_1c = pcStack_18->damage_target_card;
      if (iStack_1c < 0)
      {
        return 0;
      }
      pcVar1 = in_play(iStack_20, iStack_1c);
      if (who_gets_trigger == 3)
      {
        who_gets_trigger = (int)pcStack_18->damage_target_player;
      }
    }
    if ((((pcVar1 == NULL) || (iStack_1c != affected_card)) ||
         (iStack_20 != affected_card_controller)) ||
        (((pcVar2 = get_card_instance(iStack_20, iStack_1c),
           (get_special_flags2_by_instance(pcVar2) & SF2_WILL_BE_EXILED_IF_PUTTED_IN_GRAVE) != 0 ||
               (result = check_for_death_trigger_removal(iStack_20, iStack_1c), result != 0)) ||
          (result = is_humiliated(player, card, AL_TRIG), result != 0))))
    {
      return 0;
    }
    if (required_kill_code == 0)
    {
      if (pcVar1->kill_code == 4)
      {
        return 0;
      }
    }
    else if ((unsigned int)pcVar1->kill_code != required_kill_code)
    {
      return 0;
    }
    result = is_what(iStack_20, iStack_1c, 2);
    iStack_2c = 1;
    if (result != 0)
    {
      iStack_2c = card_counts[player][5] + 1;
    }
    do
    {
      if (who_gets_trigger == player)
      {
        result = create_legacy_effect(who_gets_trigger, card, effect_this_dies);
      }
      else
      {
        if (1 - player != who_gets_trigger)
        {
          backtrace_and_abort("functions/functions.c:9804\nwho_gets_trigger == 1-player");
        }
        result = create_legacy_effect_for_opponent(player, card, effect_this_dies, -1, -1);
      }
      if (result == -1)
      {
        backtrace_and_abort("functions/functions.c:9808\nlegacy != -1");
      }
      pcVar1 = get_card_instance(who_gets_trigger, result);
      pcVar2 = get_card_instance(iStack_20, iStack_1c);
      if ((pcVar2->state & 0x1000) == 0)
      {
        uVar4 = pcVar1->state & 0xffffefff;
      }
      else
      {
        uVar4 = pcVar1->state | 0x1000;
      }
      pcVar1->state = uVar4;
      memcpy(pcVar1->targets, pcStack_18->targets, sizeof(pcVar1->targets));
      pcVar1->number_of_targets = pcStack_18->number_of_targets;
      pcVar1->parent_controller = player;
      pcVar1->parent_card = card;
      pcVar1->eot_toughness0 = '\x01';
      pcVar1->eot_toughness3 = (uint8_t)trigger_mode;
      pcVar1->special_counters = pcStack_18->special_counters;
      pcVar1->special_counter_type = pcStack_18->special_counter_type;
      pcVar1->counters2 = pcStack_18->counters2;
      pcVar1->counters3 = pcStack_18->counters3;
      pcVar1->counters4 = pcStack_18->counters4;
      pcVar1->counters = pcStack_18->counters;
      pcVar1->counters5 = pcStack_18->counters5;
      pcVar1->counters_m1m1 = pcStack_18->counters_m1m1;
      pcVar1->damage_target_player = pcStack_18->damage_target_player;
      pcVar1->damage_target_card = pcStack_18->damage_target_card;
      result = is_what(player, card, 2);
      if (result != 0)
      {
        result = get_abilities(player, card, EVENT_POWER, -1);
        pcVar1->power = (int16_t)result;
        result = get_abilities(player, card, EVENT_TOUGHNESS, -1);
        pcVar1->toughness = (int16_t)result;
      }
      iStack_2c = iStack_2c + -1;
    } while (iStack_2c != 0);
  }
  return (unsigned int)(event == EVENT_RESOLVE_THIS_DIES_TRIGGER);
}

/* Recovered October 2022 implementations; see docs/recovery-progress.md. */
int this_is_sacrificed_trigger(int player, int card, event_t event, resolve_trigger_t trigger_mode);

/* ManalinkEh 2022: 0x2689140; original functions/functions.c:9853. */
int this_is_sacrificed_trigger(int player, int card, event_t event, resolve_trigger_t trigger_mode)
{
  int result;

  result = this_dies_trigger_impl(player, card, event, trigger_mode, player, 0, 3);
  return result;
}

/* ManalinkEh 2022: 0x2689170; original functions/functions.c:9857. */
int this_is_exiled_trigger(int player, int card, event_t event, resolve_trigger_t trigger_mode)
{
  int result;

  result = this_dies_trigger_impl(player, card, event, trigger_mode, player, 0, 4);
  return result;
}

/* ManalinkEh 2022: 0x269b2e0; original functions/functions.c:8011. */
int lobotomy_effect(int player, int t_player, int csvid, int max, int flags)
{
  int iVar1;
  int *piVar2;
  unsigned int uVar3;
  int iVar4;
  int iStack_fc;
  int iStack_f8;
  test_definition_t this_test;

  iVar4 = 0;
  new_default_test_definition(&this_test, 0, "");
  this_test.id = csvid;
  this_test.no_shuffle = 1;
  iStack_f8 = 0;
  iStack_fc = 0;
  do
  {
    if (ai_is_speculating != 1)
    {
      iVar1 = is_id_in_hand(t_player, csvid);
      if (iVar1 == 0)
      {
        scnprintf(this_test.message, 100, "Here is %s\'s hand.", opponent_name);
      }
      else
      {
        scnprintf(this_test.message, 100, "Hand: Select a card named %s to exile.",
                  cards_ptr[csvid]->full_name);
      }
    }
    if (hand_count[t_player] == 0)
    {
    LAB_0269b3ba:
      iStack_f8 = iStack_f8 + 1;
    }
    else if ((0 < hand_count[t_player]) && ((iVar4 < max || (max < 0))))
    {
      do
      {
        iVar1 = new_global_tutor(player, t_player, 4, 6, 0, 5, &this_test);
        uVar3 = flags & 1;
        if (-1 < iVar1)
        {
          iStack_fc = iStack_fc + 1;
          iVar4 = iVar4 + 1;
          uVar3 = hand_count[t_player];
        }
        if ((uVar3 == 0) || (iVar1 = is_id_in_hand(t_player, csvid), iVar1 == 0))
          goto LAB_0269b3ba;
      } while ((0 < hand_count[t_player]) && ((iVar4 < max || (max < 0))));
    }
    if (ai_is_speculating != 1)
    {
      scnprintf(this_test.message, 100, "Graveyard: Select a card named %s to exile.",
                cards_ptr[csvid]->full_name);
    }
    iVar1 = new_special_count_grave(t_player, &this_test);
    if (iVar1 != 0)
    {
      do
      {
        iVar1 = new_special_count_grave(t_player, &this_test);
        if ((iVar1 < 1) || ((-1 < max && (max <= iVar4))))
          goto LAB_0269b41a;
        iVar1 = new_global_tutor(player, t_player, 2, 6, flags & 1U, 5, &this_test);
        uVar3 = flags & 1U;
        if (-1 < iVar1)
        {
          uVar3 = new_special_count_grave(t_player, &this_test);
          iVar4 = iVar4 + 1;
        }
      } while ((uVar3 != 0) && (iVar1 = new_special_count_grave(t_player, &this_test), iVar1 != 0));
    }
    iStack_f8 = iStack_f8 + 1;
  LAB_0269b41a:
    piVar2 = deck_ptr[t_player];
    if (ai_is_speculating != 1)
    {
      iVar1 = special_count_zone(t_player, piVar2, &this_test);
      if (iVar1 == 0)
      {
        scnprintf(this_test.message, 100, "Here is %s\'s library.", opponent_name);
        piVar2 = deck_ptr[t_player];
      }
      else
      {
        scnprintf(this_test.message, 100, "Library: Select a card named %s to exile.",
                  cards_ptr[csvid]->full_name);
        piVar2 = deck_ptr[t_player];
      }
    }
    if (*piVar2 == -1)
    {
    LAB_0269b348:
      iStack_f8 = iStack_f8 + 1;
    }
    else if ((iVar4 < max) || (max < 0))
    {
      do
      {
        iVar1 = new_global_tutor(player, t_player, 1, 6, 0, 5, &this_test);
        if (iVar1 < 0)
        {
          if ((flags & 1U) == 0)
            goto LAB_0269b348;
          piVar2 = deck_ptr[t_player];
        }
        else
        {
          piVar2 = deck_ptr[t_player];
          iVar4 = iVar4 + 1;
          if (*piVar2 == -1)
            goto LAB_0269b348;
        }
        iVar1 = special_count_zone(t_player, piVar2, &this_test);
        if (iVar1 == 0)
          goto LAB_0269b348;
      } while ((*deck_ptr[t_player] != -1) && ((iVar4 < max || (max < 0))));
    }
    if (2 < iStack_f8)
    {
      if ((flags & 2U) == 0)
      {
        shuffle(t_player);
      }
      if ((flags & 4U) == 0)
      {
        iStack_fc = iVar4;
      }
      return iStack_fc;
    }
  } while (true);
}

/* Recovered October 2022 implementations; see docs/recovery-progress.md. */
int validate_counter_triggered_ability(int player, int card, target_definition_t *td,
                                       int target_pos);
int resolve_counter_triggered_ability(int player, int card, target_definition_t *td,
                                      int target_pos);

/* ManalinkEh 2022: 0x26931c0; original functions/functions.c:15087. */
int validate_counter_triggered_ability(int player, int card, target_definition_t *td,
                                       int target_pos)
{
  card_instance_t *instance;
  unsigned int uVar2;
  target_definition_t td_default;

  instance = get_card_instance(player, card);
  if (td == NULL)
  {
    counter_triggered_target_definition(player, card, &td_default, 0);
  }
  instance =
      get_card_instance(instance->targets[target_pos].player, instance->targets[target_pos].card);
  uVar2 = 0;
  if ((instance->state & 0x10000000) != 0)
  {

    uVar2 = (unsigned int)(cards_data[stack_data[stack_size - 1].internal_card_id].id != 0x388);
  }
  return uVar2;
}

/* ManalinkEh 2022: 0x2693260; original functions/functions.c:15134. */
int resolve_counter_triggered_ability(int player, int card, target_definition_t *td, int target_pos)
{
  int32_t iVar1;
  unsigned int uVar2;
  unsigned int uVar3;
  card_instance_t *pcVar4;
  int iVar5;
  card_instance_t *pcVar6;

  pcVar4 = get_card_instance(player, card);
  iVar5 = validate_counter_triggered_ability(player, card, td, target_pos);
  if (iVar5 != 0)
  {

    pcVar4 =
        get_card_instance(pcVar4->targets[target_pos].player, pcVar4->targets[target_pos].card);
    pcVar4->state = pcVar4->state | 0x40000000;
    return 1;
  }

  iVar5 = create_legacy_effect(player, card, counter_triggered_ability_legacy);
  pcVar6 = get_card_instance(player, iVar5);
  iVar5 = current_phase;
  iVar1 = pcVar4->targets[9].card;
  pcVar6->targets[9].player = pcVar4->targets[9].player;
  pcVar6->targets[2].player = iVar5;
  uVar2 = pcVar6->state;
  uVar3 = pcVar6->token_status;
  pcVar6->targets[9].card = iVar1;
  pcVar6->token_status = uVar3 | 0x10000;
  pcVar6->state = uVar2 | 0x22;
  return 1;
}


/* October 2022 extension-backed flag APIs. */
int get_special_flags_by_instance(card_instance_t *instance);
int get_special_flags(int player, int card, int type_of_special_flag);
int check_special_flags(int player, int card, int value);
void set_special_flags(int player, int card, int value);
void remove_special_flags(int player, int card, int value);
int get_special_flags2_by_instance(card_instance_t *instance);
int check_special_flags2(int player, int card, int value);
void set_special_flags2(int player, int card, int value);
void remove_special_flags2(int player, int card, int value);
int get_special_flags3_by_instance(card_instance_t *instance);
int check_special_flags3(int player, int card, int value);
void set_special_flags3(int player, int card, int value);
void remove_special_flags3(int player, int card, int value);

/* ManalinkEh 2022: 0x2679db0; original functions/functions.c:1062. */
int get_special_flags_by_instance(card_instance_t *instance)
{
  return instance->ext->special_flags;
}

/* ManalinkEh 2022: 0x2679d40; original functions/functions.c:1048. */
int get_special_flags(int player, int card, int type_of_special_flag)
{
  card_instance_t *instance;
  special_flags3_t sVar2;

  instance = get_card_instance(player, card);
  if (type_of_special_flag == 1)
  {
    return instance->ext->special_flags;
  }
  if (type_of_special_flag != 2)
  {
    if (type_of_special_flag == 3)
    {
      sVar2 = instance->ext->special_flags3;
    }
    else
    {
      sVar2 = ~0U;
    }
    return sVar2;
  }
  return instance->ext->special_flags2;
}

/* ManalinkEh 2022: 0x2679dd0; original functions/functions.c:1066. */
int check_special_flags(int player, int card, int value)
{
  card_instance_t *instance;

  instance = get_card_instance(player, card);
  return value & instance->ext->special_flags;
}

/* ManalinkEh 2022: 0x2679e00; original functions/functions.c:1071. */
void set_special_flags(int player, int card, int value)
{
  card_instance_t *instance;
  int player_00;
  int card_00;
  if (card != -1)
  {
    instance = get_card_instance(player, card);
    instance->ext->special_flags = instance->ext->special_flags | value;
    return;
  }
  player_00 = 0;
  do
  {
    if (((player == player_00) || (player == 2)) &&
        (card_00 = active_cards_count[player_00] + -1, -1 < card_00))
    {
      do
      {
        instance = in_play(player_00, card_00);
        if (instance != NULL)
        {
          instance = get_card_instance(player_00, card_00);
          instance->ext->special_flags = instance->ext->special_flags | value;
        }
        card_00 = card_00 + -1;
      } while (card_00 != -1);
    }
    player_00 = player_00 + 1;
  } while (player_00 != 2);
  return;
}

/* ManalinkEh 2022: 0x267a050; original functions/functions.c:1093. */
void remove_special_flags(int player, int card, int value)
{
  card_instance_t *instance;
  int card_00;
  int player_00;
  if (card != -1)
  {
    instance = get_card_instance(player, card);
    instance->ext->special_flags = instance->ext->special_flags & (~(uint32_t)value);
    return;
  }
  player_00 = 0;
  do
  {
    if (((player == player_00) || (player == 2)) &&
        (card_00 = active_cards_count[player_00] + -1, -1 < card_00))
    {
      do
      {
        instance = in_play(player_00, card_00);
        if (instance != NULL)
        {
          instance = get_card_instance(player_00, card_00);
          instance->ext->special_flags = instance->ext->special_flags & (~(uint32_t)value);
        }
        card_00 = card_00 + -1;
      } while (card_00 != -1);
    }
    player_00 = player_00 + 1;
  } while (player_00 != 2);
  return;
}

/* ManalinkEh 2022: 0x267a110; original functions/functions.c:1115. */
int get_special_flags2_by_instance(card_instance_t *instance)
{
  return instance->ext->special_flags2;
}

/* ManalinkEh 2022: 0x267a130; original functions/functions.c:1119. */
int check_special_flags2(int player, int card, int value)
{
  card_instance_t *instance;
  instance = get_card_instance(player, card);
  return value & instance->ext->special_flags2;
}

/* ManalinkEh 2022: 0x267a160; original functions/functions.c:1123. */
void set_special_flags2(int player, int card, int value)
{
  card_instance_t *instance;
  int player_00;
  int card_00;
  if (card != -1)
  {
    instance = get_card_instance(player, card);
    instance->ext->special_flags2 = instance->ext->special_flags2 | value;
    return;
  }
  player_00 = 0;
  do
  {
    if (((player == player_00) || (player == 2)) &&
        (card_00 = active_cards_count[player_00] + -1, -1 < card_00))
    {
      do
      {
        instance = in_play(player_00, card_00);
        if (instance != NULL)
        {
          instance = get_card_instance(player_00, card_00);
          instance->ext->special_flags2 = instance->ext->special_flags2 | value;
        }
        card_00 = card_00 + -1;
      } while (card_00 != -1);
    }
    player_00 = player_00 + 1;
  } while (player_00 != 2);
  return;
}

/* ManalinkEh 2022: 0x267a210; original functions/functions.c:1145. */
void remove_special_flags2(int player, int card, int value)
{
  card_instance_t *instance;
  int card_00;
  int player_00;
  if (card != -1)
  {
    instance = get_card_instance(player, card);
    instance->ext->special_flags2 = instance->ext->special_flags2 & (~(uint32_t)value);
    return;
  }
  player_00 = 0;
  do
  {
    if (((player == player_00) || (player == 2)) &&
        (card_00 = active_cards_count[player_00] + -1, -1 < card_00))
    {
      do
      {
        instance = in_play(player_00, card_00);
        if (instance != NULL)
        {
          instance = get_card_instance(player_00, card_00);
          instance->ext->special_flags2 = instance->ext->special_flags2 & (~(uint32_t)value);
        }
        card_00 = card_00 + -1;
      } while (card_00 != -1);
    }
    player_00 = player_00 + 1;
  } while (player_00 != 2);
  return;
}

/* ManalinkEh 2022: 0x267a2d0; original functions/functions.c:1167. */
int get_special_flags3_by_instance(card_instance_t *instance)
{
  return instance->ext->special_flags3;
}

/* ManalinkEh 2022: 0x267a2f0; original functions/functions.c:1171. */
int check_special_flags3(int player, int card, int value)
{
  card_instance_t *instance;
  instance = get_card_instance(player, card);
  return value & instance->ext->special_flags3;
}

/* ManalinkEh 2022: 0x267a320; original functions/functions.c:1175. */
void set_special_flags3(int player, int card, int value)
{
  card_instance_t *instance;
  int player_00;
  int card_00;
  if (card != -1)
  {
    instance = get_card_instance(player, card);
    instance->ext->special_flags3 = instance->ext->special_flags3 | value;
    return;
  }
  player_00 = 0;
  do
  {
    if (((player == player_00) || (player == 2)) &&
        (card_00 = active_cards_count[player_00] + -1, -1 < card_00))
    {
      do
      {
        instance = in_play(player_00, card_00);
        if (instance != NULL)
        {
          instance = get_card_instance(player_00, card_00);
          instance->ext->special_flags3 = instance->ext->special_flags3 | value;
        }
        card_00 = card_00 + -1;
      } while (card_00 != -1);
    }
    player_00 = player_00 + 1;
  } while (player_00 != 2);
  return;
}

/* ManalinkEh 2022: 0x267a510; original functions/functions.c:1197. */
void remove_special_flags3(int player, int card, int value)
{
  card_instance_t *instance;
  int card_00;
  int player_00;
  if (card != -1)
  {
    instance = get_card_instance(player, card);
    instance->ext->special_flags3 = instance->ext->special_flags3 & (value ^ ~0U);
    return;
  }
  player_00 = 0;
  do
  {
    if (((player == player_00) || (player == 2)) &&
        (card_00 = active_cards_count[player_00] + -1, -1 < card_00))
    {
      do
      {
        instance = in_play(player_00, card_00);
        if (instance != NULL)
        {
          instance = get_card_instance(player_00, card_00);
          instance->ext->special_flags3 = instance->ext->special_flags3 & (value ^ ~0U);
        }
        card_00 = card_00 + -1;
      } while (card_00 != -1);
    }
    player_00 = player_00 + 1;
  } while (player_00 != 2);
  return;
}


#ifndef SHANDALAR
/* Recovered October 2022 typed consumers and compiler parts. */

/* ManalinkEh 2022: 0x2692a40; original functions/functions.c:14935. */
int counterspell_validate(int player, int card, target_definition_t *td, int target_num)
{
  int32_t iVar1;
  int iVar2;
  card_instance_t *instance;
  target_definition_t td_default;

  if (td == NULL)
  {
    counterspell_target_definition(player, card, &td_default, 0);
    td_default.preferred_controller = 2;
    iVar2 = validate_target(player, card, &td_default, target_num);
  }
  else
  {
    iVar1 = td->preferred_controller;
    td->preferred_controller = td->allowed_controller;
    iVar2 = validate_target(player, card, td, target_num);
    td->preferred_controller = iVar1;
  }
  instance = get_card_instance(player, card);
  if (iVar2 != 0)
  {
    instance =
        get_card_instance(instance->targets[target_num].player, instance->targets[target_num].card);
    if ((instance->state & 0x20) != 0)
    {
      return 1;
    }
  }
  spell_fizzled = 1;
  return 0;
}


/* ManalinkEh 2022: 0x2692b30; original functions/functions.c:14863. */
int counterspell(int player, int card, event_t event, target_definition_t *td,
                      int target_num)
{
  int32_t iVar1;
  int32_t iVar2;
  card_instance_t *instance;
  int iVar4;
  target_definition_t td_default;

  instance = get_card_instance(player, card);
  if (event == EVENT_CAN_CAST)
  {
    if (card_on_stack_controller == -1)
    {
      return 0;
    }
    if (td == NULL)
    {
      td = &td_default;
      counterspell_target_definition(player, card, td, 0);
    }
    iVar4 = card_on_stack;
    if (target_num < 0)
    {
      iVar1 = instance->targets[0].player;
      instance->targets[0].player = card_on_stack_controller;
      iVar2 = instance->targets[0].card;
      instance->targets[0].card = iVar4;
      iVar4 = would_validate_target(player, card, td, 0);
      instance->targets[0].player = iVar1;
      instance->targets[0].card = iVar2;
      if (iVar4 == 0)
      {
        return 0;
      }
    }
    else
    {
      instance->targets[target_num].player = card_on_stack_controller;
      instance->targets[target_num].card = iVar4;
      if ((int)(unsigned int)instance->number_of_targets <= target_num)
      {
        instance->number_of_targets = (char)target_num + '\x01';
      }
      iVar4 = would_validate_target(player, card, td, target_num);
      if (iVar4 == 0)
      {
        return 0;
      }
    }
    return 99;
  }
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
    if (card_on_stack_controller == -1)
    {
      spell_fizzled = 1;
    }
    else
    {
      instance->targets[target_num].player = card_on_stack_controller;
      instance->targets[target_num].card = card_on_stack;
      if ((int)(unsigned int)instance->number_of_targets <= target_num)
      {
        instance->number_of_targets = (char)target_num + '\x01';
      }
    }
    ai_modifier = ai_modifier + -0x24;
    return 0;
  }
  if (event == EVENT_CAN_CHANGE_TARGET)
  {
    instance = get_card_instance(player, card);
    return (instance->state >> 0x14 ^ 1) & 1;
  }
  if (event == EVENT_CHANGE_TARGET)
  {
    instance->targets[9].player = player;
    instance->targets[9].card = card;
    instance->targets[target_num].player = player;
    instance->targets[target_num].card = card;
    return 0;
  }
  if (event != EVENT_CAN_COUNTER)
  {
    if (event == EVENT_RESOLVE_SPELL)
    {
      iVar4 = counterspell_validate(player, card, td, target_num);
      if (iVar4 != 0)
      {
        real_counter_a_spell(player, card, instance->targets[target_num].player,
                             instance->targets[target_num].card);
      }
      instance->number_of_targets = '\0';
      kill_card(player, card, KILL_DESTROY);
      return 0;
    }
    return 0;
  }
  if (raw_mana_available[player][2] < 2)
  {
    return 0;
  }
  ai_modifier = ai_modifier + 0x18;
  return 0;
}

#endif


#ifndef SHANDALAR
typedef union
{
  card_instance_t *instance;
  int32_t number;
  uint32_t bits;
  uint8_t low;
} generic_spell_word_t;

typedef union
{
  test_definition_t test;
  target_definition_t target;
  char error[200];
} generic_spell_scratch_t;

_Static_assert(sizeof(generic_spell_word_t) == 4, "generic spell reused words");
_Static_assert(sizeof(generic_spell_scratch_t) == 216, "generic spell shared scratch");

/* ManalinkEh 2022: 0x26a0250; original functions/functions.c:15783. */
int generic_spell(int player, int card, event_t event, int flags, target_definition_t *td,
                       const char *prompt, int max_targets, test_definition_t *this_test)
{
  enum
  {
    SF2_REDIRECT = 0x40000000
  };

  target_t *selected_target;
  uint8_t uVar2;
  int iVar3;
  target_t *ptVar4;
  card_instance_t *pcVar5;
  int iVar6;
  int32_t t_player;
  int iVar7;
  unsigned int uVar8;
  card_instance_t *pcVar9;
  const char *prompt_00;
  card_instance_t *pcVar10;
  const char **ppcVar11;
  int iVar12;
  char cVar13;
  unsigned char bVar14;
  unsigned int card_00;
  target_t *ptVar15;
  bool bVar16;
  uint8_t uVar17;
  int aiStack_114[2];
  target_t *saved_target;
  int32_t base_target;
  int32_t selected_targets;
  int32_t cleanup_end;
  unsigned int uStack_104;
  unsigned int uStack_100;
  generic_spell_word_t pcStack_fc;
  generic_spell_word_t pcStack_f8;
  generic_spell_word_t pcStack_f4;
  int targeted_player[2];
  generic_spell_scratch_t scratch;

  pcVar5 = get_card_instance(player, card);
  pcStack_f4.number = (flags & 0x200);
  if (event == EVENT_GET_SELECTED_CARD && pcStack_f4.bits != 0)
  {
    engine_selected_card = (int)pcVar5->damage_target_player << 8 | pcVar5->damage_target_card;
    return 0;
  }
  if (event == EVENT_CAN_CAST)
  {
    if (flags == 0)
    {
      return 1;
    }
    if ((flags & 1U) != 0)
    {
      pcVar9 = get_card_instance(player, card);
      if ((pcVar9->ext->special_flags2 & SF2_SPELLSKITE) != 0)
      {
        uVar2 = pcVar5->number_of_targets;
        pcVar5->number_of_targets = '\n';
        pcStack_f4.low = uVar2;
        iVar7 = would_validate_target(player, card, td, 9);
        pcVar5->number_of_targets = pcStack_f4.low;
        return iVar7;
      }
      pcVar9 = get_card_instance(player, card);
      if ((pcVar9->ext->special_flags2 & SF2_QUICKSILVER_DRAGON) != 0)
      {
        td->zone = 0x200;
        td->required_type = 2;
      }
      pcVar9 = get_card_instance(player, card);
      if ((pcVar9->ext->special_flags2 & SF2_REBOUND) != 0)
      {
        td->zone = 0x1000;
      }
      if ((flags & 0x20U) == 0)
      {
        if ((flags & 0x4000U) == 0)
        {
          if ((pcStack_f4.bits != 0) && (0 < td->illegal_abilities))
          {
            iVar7 = target_available(player, card, td);
            if (iVar7 == 1)
            {
              iVar7 = 0;
              pcStack_f4.instance = pcVar5;
              do
              {
                iVar6 = 0;
                if (0 < active_cards_count[iVar7])
                {
                  do
                  {
                    pcVar5 = in_play(iVar7, iVar6);
                    if (pcVar5 != NULL)
                    {
                      iVar12 = is_what(iVar7, iVar6, 0x47);
                      if (iVar12 != 0)
                      {
                        iVar12 = would_validate_arbitrary_target(td, iVar7, iVar6);
                        if (iVar12 != 0)
                        {
                          iVar12 = is_humiliated(iVar7, iVar6, AL_STATIC);
                          if (iVar12 == 0)
                          {
                            iVar12 = get_id(iVar7, iVar6);
                            if (iVar12 == 0x32b)
                            {
                              return 0;
                            }
                            if (iVar12 == 0x242)
                            {
                              return 0;
                            }
                          }
                        }
                      }
                    }
                    iVar6 = iVar6 + 1;
                  } while (iVar6 < active_cards_count[iVar7]);
                }
                iVar7 = iVar7 + 1;
                pcVar5 = pcStack_f4.instance;
              } while (iVar7 != 2);
            }
          }
          if (max_targets != 1)
          {
            iVar7 = target_available(td->player, td->card, td);
            if (iVar7 < max_targets)
            {
              return 0;
            }
            goto LAB_026a02ec;
          }
        }
        iVar7 = can_target(td);
        if (iVar7 == 0)
        {
          return 0;
        }
      }
    }
  LAB_026a02ec:
    if ((flags & 4U) != 0)
    {
      iVar7 = check_battlefield_for_id(2, 0x23ba);
      if (iVar7 != 0)
      {
        return 0;
      }
    }
    if ((flags & 0x1808U) != 0)
    {
      iVar7 = player;
      if ((flags & 0x800U) != 0)
      {
        iVar7 = 1 - player;
      }
      bVar16 = (flags & 0x1000U) != 0;
      iVar6 = iVar7;
      if (bVar16)
      {
        iVar6 = 2;
      }
      cVar13 = bVar16 || iVar7 == 2;
      iVar7 = 0;
      pcStack_f8.instance = pcVar5;
      do
      {
        if ((iVar6 == iVar7) || (cVar13 != '\0'))
        {
          pcStack_f4.low = cVar13;
          iVar12 = new_special_count_grave(iVar7, this_test);
          cVar13 = pcStack_f4.low;
          if (0 < iVar12)
          {
            iVar12 = graveyard_has_shroud(iVar7);
            pcVar5 = pcStack_f8.instance;
            cVar13 = pcStack_f4.low;
            if (iVar12 == 0)
              break;
          }
        }
        iVar7 = iVar7 + 1;
        if (iVar7 == 2)
        {
          return 0;
        }
      } while (true);
    }
    if (((flags & 0x10U) != 0) && ((((unsigned int)land_can_be_played >> 8 & 0xff) & 2) == 0))
    {
      return 0;
    }
    if (((flags & 0x100U) != 0) && (((unsigned char)land_can_be_played & 4) == 0))
    {
      return 0;
    }
    if ((flags & 0x40U) != 0)
    {
      if (td == NULL)
      {
        iVar7 = opponent_is_valid_target(player, card);
      }
      else
      {
        pcVar5->targets[0].card = -1;
        pcVar5->targets[0].player = 1 - player;
        pcVar5->number_of_targets = '\x01';
        iVar7 = would_validate_target(player, card, td, 0);
        pcVar5->number_of_targets = '\0';
      }
      if (iVar7 == 0)
      {
        return 0;
      }
    }
    if ((flags & 2U) != 0)
    {
      iVar7 = counterspell(player, card, EVENT_CAN_CAST, td, -1);
      if (iVar7 == 0)
      {
        return 0;
      }
    }
    if ((flags & 0x400U) != 0)
    {
      iVar7 = can_sacrifice_type_as_cost(player, 1, TYPE_CREATURE);
      if (iVar7 == 0)
      {
        return 0;
      }
    }
    if ((flags & 0x8000U) != 0)
    {
      iVar7 = new_can_sacrifice_as_cost(player, 1, this_test);
      if (iVar7 == 0)
      {
        return 0;
      }
    }
    if ((flags & 0x112U) == 0)
    {
      return 1;
    }
    return 99;
  }
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
  else if (event != EVENT_CHANGE_TARGET)
    goto LAB_026a0618;
  pcStack_f8.low = 0;
  pcStack_fc.low = pcStack_f4.bits != 0;
  if (flags == 0)
  {
    return 0;
  }
  pcVar9 = get_card_instance(player, card);
  if ((pcVar9->ext->special_flags2 & SF2_FACE_DOWN_DUE_TO_MANIFEST) != 0)
  {
    return 0;
  }
  if (((player == 1) && (pcStack_fc.low != '\0')) && (((unsigned char)trace_mode & 2) == 0))
  {
    ai_modifier = ai_modifier +
                  ((pcVar5->targets[0].player == td->preferred_controller) - 1 & 0xffffffd0) + 0x18;
  }
  iVar7 = 0;
  if (event == EVENT_CHANGE_TARGET)
  {
    pcVar9 = get_card_instance(player, card);
    if ((pcVar9->ext->special_flags2 & SF2_SPELLSKITE) != 0)
    {
      uVar8 = (unsigned int)pcVar5->number_of_targets;
      int target_cards[uVar8];
      iVar6 = 0;
      while (iVar6 < (int)(unsigned int)pcVar5->number_of_targets)
      {
        iVar12 = pcVar5->targets[iVar6].card;
        iVar3 = pcVar5->targets[iVar6].player;
        iVar6 = iVar6 + 1;
        if (iVar12 == -1)
        {
          iVar12 = get_internal_card_id_from_csv_id(iVar3 == player ? 0x615a : 0x6161);
          target_cards[iVar6 - 1] = iVar12;
        }
        else
        {
          pcVar9 = get_card_instance(iVar3, iVar12);
          target_cards[iVar6 - 1] = pcVar9->internal_card_id;
        }
      }
      iVar7 =
          show_deck(player, target_cards, iVar6,
                         " Select the target to change (Rules Engine = you, Deadbox = opponent)", 0,
                         ENGINE_DONE_LABEL);
      return iVar7;
    }
    pcVar9 = get_card_instance(player, card);
    if ((pcVar9->ext->special_flags2 & SF2_QUICKSILVER_DRAGON) == 0)
    {
      pcVar9 = get_card_instance(player, card);
      if ((pcVar9->ext->special_flags2 & SF2_REBOUND) != 0)
      {
        td->zone = 0x1000;
      }
    }
    else
    {
      td->zone = 0x200;
      td->required_type = 2;
    }
    uVar17 = ((char)pcStack_f8.low);
    if ((pcVar5->targets[0].card != -1) && ((flags & 10U) == 0))
    {
      pcVar9 = get_card_instance(player, card);
      uVar17 = ((char)pcStack_f8.low);
      if ((pcVar9->ext->special_flags2 & SF2_REDIRECT) == 0)
      {
        pcVar9 = get_card_instance(pcVar5->targets[0].player, pcVar5->targets[0].card);
        pcVar9->state = pcVar9->state | 0x100000;
        uVar17 = ((char)pcStack_f8.low);
      }
    }
    if ((flags & 4U) != 0)
      goto LAB_026a03d0;
  LAB_026a042b:
    uStack_104 = 0;
    uStack_100 = (flags & 0x21U) != 0;
    pcStack_fc.number = (flags & 0x1808);
    if (event != EVENT_CHANGE_TARGET)
      goto LAB_026a045c;
    pcStack_f8.instance = get_card_instance(attacking_card_controller, attacking_card);
    max_targets = (int)pcVar5->number_of_targets;
    get_card_instance(player, card);
    if ((pcStack_fc.bits == 0) && ((char)uStack_100 != '\0'))
    {
      base_target = 1;
      aiStack_114[1] = max_targets;
      goto LAB_026a102c;
    }
    uStack_104 = 0;
    base_target = 1;
  }
  else
  {
    if ((flags & 4U) != 0)
    {
    LAB_026a03d0:
      iVar6 = is_token(player, card);
      if (iVar6 == 0)
      {
        set_x_for_x_spells(player, card, event, x_value);
      }
      set_special_flags2(player, card, 0x20000);
    }
    uVar17 = event != EVENT_CHANGE_TARGET;
    bVar14 = (unsigned char)((unsigned int)flags >> 10) & uVar17;
    if (bVar14 == 0)
    {
      if (((flags & 0x8000U) == 0) || (!(bool)uVar17))
        goto LAB_026a042b;
    LAB_026a0e40:
      uStack_104 = new_sacrifice(player, card, player,
                                      SAC_RETURN_CHOICE | SAC_AS_COST | SAC_JUST_MARK,
                                      this_test);
      if (uStack_104 == 0)
        goto LAB_026a10c8;
      uStack_100 = (flags & 0x21U) != 0;
      uVar17 = 1;
    }
    else
    {
      pcStack_f8.low = bVar14;
      new_default_test_definition(&scratch.test, 2, "Select a creature to sacrifice.");
      uStack_104 = new_sacrifice(player, card, player,
                                      SAC_RETURN_CHOICE | SAC_AS_COST | SAC_JUST_MARK,
                                      &scratch.test);
      if (uStack_104 == 0)
      {
      LAB_026a10c8:
        spell_fizzled = 1;
        goto LAB_026a0618;
      }
      if ((flags & 0x8000U) != 0)
        goto LAB_026a0e40;
      uStack_100 = (flags & 0x21U) != 0;
      uVar17 = ((char)pcStack_f8.low);
    }
  LAB_026a045c:
    pcStack_fc.number = (flags & 0x1808);
    pcStack_f8.low = uVar17;
    pcVar9 = get_card_instance(player, card);
    if (((((pcVar9->ext->special_flags & SF_TARGETS_ALREADY_SET) == 0) ||
          (((char)pcStack_f8.low) == '\0')) &&
         (pcStack_fc.bits == 0)) &&
        ((char)uStack_100 != '\0'))
    {
      aiStack_114[1] = 0;
      base_target = 0;
      pcStack_f8.instance = pcVar5;
      if (event == EVENT_CHANGE_TARGET)
      {
      LAB_026a102c:
        td->allow_cancel = 0;
        td->who_chooses = attacking_card_controller;
      }
      pcStack_fc.number = (base_target + max_targets);
      uStack_100 = flags & 0x80;
      pcVar5->number_of_targets = '\0';
      targeted_player[0] = 0;
      targeted_player[1] = 0;
      selected_targets = base_target;
      aiStack_114[0] = iVar7;
      while (true)
      {
        iVar7 = can_target(td);
        if ((iVar7 == 0) || (pcStack_fc.number <= selected_targets))
          break;
        prompt_00 = prompt;
        if (uStack_100 == 0)
        {
          ppcVar11 = load_text_nocopy(prompt, NULL);
          prompt_00 = *ppcVar11;
        }
        iVar7 = select_target(player, card, td, prompt_00,
                                   pcStack_f8.instance->targets + selected_targets);
        if (iVar7 == 0)
          break;
        selected_target = &pcStack_f8.instance->targets[selected_targets];
        iVar7 = selected_target->card;
        iVar6 = selected_target->player;
        if (iVar7 == -1)
        {
          if (targeted_player[iVar6] == 0)
          {
            targeted_player[iVar6] = 1;
            selected_targets = selected_targets + 1;
          }
          else if ((flags & 0x4000U) == 0)
          {
            pcStack_f8.instance->number_of_targets = pcStack_f8.instance->number_of_targets + 0xff;
            sprintf(scratch.error, EXE_STR(0x73940c), EXE_STR(0x785e65));
            display_error_message(scratch.error);
          }
          else
          {
          LAB_026a0c70:
            selected_targets = selected_targets + 1;
          }
        }
        else
        {
          if (pcStack_f4.bits == 0)
          {
          LAB_026a0c4c:
            if ((flags & 0x4000U) == 0)
            {
              pcVar10 = get_card_instance(iVar6, iVar7);
              pcVar10->state = pcVar10->state | 0x100000;
              goto LAB_026a0c70;
            }
            selected_targets = selected_targets + 1;
            add_state(iVar6, iVar7, 0x200000);
          }
          else
          {
            saved_target = selected_target;
            iVar6 = is_humiliated(iVar6, iVar7, AL_STATIC);
            iVar7 = saved_target->card;
            if (iVar6 != 0)
            {
            LAB_026a0d3b:
              iVar6 = saved_target->player;
              goto LAB_026a0c4c;
            }
            iVar7 = get_id(saved_target->player, iVar7);
            if ((iVar7 != 0x32b) && (iVar7 != 0x242))
            {
              iVar7 = saved_target->card;
              goto LAB_026a0d3b;
            }
            sprintf(scratch.error, EXE_STR(0x73940c), EXE_STR(0x785e65));
            display_error_message(scratch.error);
            pcVar5->number_of_targets = pcVar5->number_of_targets + 0xff;
          }
        }
      }
      iVar7 = aiStack_114[0];
      cleanup_end = base_target + selected_targets;
      pcStack_f4.number = selected_targets;
      if (base_target < cleanup_end)
      {
        ptVar15 = pcStack_f8.instance->targets + base_target;
        ptVar4 = pcStack_f8.instance->targets;
        pcStack_fc.instance = pcVar5;
        do
        {
          if (ptVar15->card != -1)
          {
            pcVar5 = get_card_instance(ptVar15->player, ptVar15->card);
            if ((pcVar5->state & 0x100000) != 0)
            {
              pcVar5->state = pcVar5->state & 0xffefffff;
            }
            remove_state(ptVar15->player, ptVar15->card, 0x200000);
          }
          ptVar15 = ptVar15 + 1;
          pcVar5 = pcStack_fc.instance;
        } while (ptVar15 != ptVar4 + cleanup_end);
      }
      if (event == EVENT_CHANGE_TARGET)
      {
        pcVar9 = get_card_instance(player, card);
        if (((pcVar9->ext->special_flags2 & SF2_REDIRECT) == 0) &&
            (iVar6 = pcVar5->targets[0].card, iVar6 != -1))
        {
          pcVar9 = get_card_instance(pcVar5->targets[0].player, iVar6);
          if ((pcVar9->state & 0x100000) != 0)
          {
            pcVar9->state = pcVar9->state & 0xffefffff;
          }
        }
      }
      if ((flags & 0x20U) == 0)
      {
        if ((flags & 0x4000U) == 0)
        {
          if (pcStack_f4.number < (int)(base_target + max_targets))
          {
          LAB_026a0f5f:
            spell_fizzled = 1;
          }
        }
        else if (base_target == pcStack_f4.number)
          goto LAB_026a0f5f;
      }
      if (pcStack_f4.number < (int)(base_target + aiStack_114[1]))
      {
        spell_fizzled = 1;
      }
      pcStack_fc.number = 0x0;
    }
    else
    {
      base_target = 0;
      pcStack_f8.instance = pcVar5;
    }
  }
  if ((flags & 0x40U) != 0)
  {
    pcVar5->targets[0].card = -1;
    pcVar5->targets[0].player = 1 - player;
    pcVar5->number_of_targets = '\x01';
  }
  if ((flags & 2U) == 0)
  {
  LAB_026a050c:
    if (pcStack_fc.bits != 0)
    {
      t_player = player;
      if ((flags & 0x800U) != 0)
      {
        t_player = 1 - player;
      }
      if ((flags & 0x1000U) != 0)
      {
        t_player = 2;
      }
      if (event == EVENT_CHANGE_TARGET)
      {
        pcStack_f4.number = attacking_card_controller;
        pcStack_fc.number = attacking_card;
      }
      else
      {
        pcStack_fc.number = card;
        pcStack_f4.number = player;
      }
      if (max_targets < 2)
      {
        uVar8 = (unsigned int)this_test->ai_selection_mode;
        if (t_player == 2)
        {
          iVar6 = select_target_from_either_grave(pcStack_f4.number, pcStack_fc.number, SFG_0,
                                                       uVar8, uVar8, this_test, base_target,
                                                       (base_target + 1));
        }
        else
        {
          iVar6 = select_target_from_grave_source(
              pcStack_f4.number, pcStack_fc.number, t_player, SFG_0, uVar8, this_test, base_target);
        }
        if (iVar6 == -1)
        {
          spell_fizzled = 1;
          return iVar7;
        }
        pcStack_f8.instance->number_of_targets = pcStack_f8.instance->number_of_targets + '\x01';
      }
      else
      {
        if (t_player == 2)
        {
          default_target_definition(player, card, &scratch.target, 0);
          scratch.target.zone = 0x1000;
          scratch.target.illegal_abilities = 0;
          scratch.target.player = pcStack_f4.number;
          base_target = pcVar5->targets[0].player;
          uStack_100 = pcVar5->targets[0].card;
          pcStack_fc.low = pcVar5->number_of_targets;
          iVar7 = pick_target(&scratch.target, "TARGET_PLAYER");
          if (iVar7 == 0)
          {
            pcVar5->targets[0].player = base_target;
            pcVar5->targets[0].card = uStack_100;
            pcVar5->number_of_targets = pcStack_fc.low;
            return 0;
          }
          t_player = pcVar5->targets[0].player;
          pcVar5->targets[0].card = uStack_100;
          pcVar5->targets[0].player = base_target;
          pcVar5->number_of_targets = pcStack_fc.low;
        }
        iVar7 = select_multiple_cards_from_graveyard(
            pcStack_f4.number, t_player, (unsigned int)((flags & 0x20U) == 0),
            (unsigned int)this_test->ai_selection_mode, this_test, max_targets,
            pcStack_f8.instance->targets);
        if (spell_fizzled == 1)
        {
          return 0;
        }
        if ((flags & 0x24U) == 0x20)
        {
          pcVar5->info_slot = iVar7;
        }
        pcVar5->number_of_targets = pcVar5->number_of_targets + (char)iVar7;
      }
    }
    if (uStack_104 == 0)
      goto LAB_026a0618;
    card_00 = uStack_104 >> 0x18;
    uVar8 = (int)uStack_104 >> 0x10 & 0xff;
    if (spell_fizzled != 1)
    {
      kill_card(uVar8, card_00, KILL_SACRIFICE);
      goto LAB_026a0618;
    }
  }
  else
  {
    if (event == EVENT_CHANGE_TARGET)
    {
      iVar6 = validate_arbitrary_target(td, player, card);
      if (iVar6 != 0)
      {
        pcVar5->targets[1].player = player;
        pcVar5->targets[1].card = card;
      }
      goto LAB_026a050c;
    }
    counterspell(player, card, event, td, 0);
    if (spell_fizzled != 1)
      goto LAB_026a050c;
    if (uStack_104 == 0)
      goto LAB_026a0618;
    card_00 = uStack_104 >> 0x18;
    uVar8 = (int)uStack_104 >> 0x10 & 0xff;
  }
  pcVar9 = get_card_instance(uVar8, card_00);
  if ((pcVar9->state & 0x100000) != 0)
  {
    pcVar9->state = pcVar9->state & 0xffefffff;
  }
LAB_026a0618:
  if (event != EVENT_CAN_CHANGE_TARGET)
  {
    return 0;
  }
  if (((pcVar5->targets[0].card != -1) && ((flags & 1U) != 0)) && (max_targets == 1))
  {
    pcVar9 = get_card_instance(player, card);
    if ((pcVar9->ext->special_flags2 & SF2_REDIRECT) == 0)
    {
      pcVar9 = get_card_instance(pcVar5->targets[0].player, pcVar5->targets[0].card);
      pcVar9->state = pcVar9->state | 0x100000;
    }
  }
  if ((flags & 2U) == 0)
  {
    iVar7 =
        generic_spell(player, card, EVENT_CAN_CAST, flags, td, prompt, max_targets, this_test);
  }
  else
  {
    pcVar9 = get_card_instance(player, card);
    if ((pcVar9->state & 0x100000) == 0)
    {
      return 1;
    }
    iVar7 = 0;
  }
  if (pcVar5->targets[0].card == -1)
  {
    return iVar7;
  }
  if ((flags & 1U) == 0)
  {
    return iVar7;
  }
  if (max_targets == 1)
  {
    pcVar9 = get_card_instance(player, card);
    if ((pcVar9->ext->special_flags2 & SF2_REDIRECT) != 0)
    {
      return iVar7;
    }
    pcVar5 = get_card_instance(pcVar5->targets[0].player, pcVar5->targets[0].card);
    if ((pcVar5->state & 0x100000) != 0)
    {
      pcVar5->state = pcVar5->state & 0xffefffff;
      return iVar7;
    }
    return iVar7;
  }
  return iVar7;
}

#endif


#ifndef SHANDALAR
/* Recovered October 2022 ability writers and Aura pump consumers. */


/* ManalinkEh 2022: 0x267a730; original functions/functions.c:1293. */
void default_pump_ability_definition(int player, int card, pump_ability_t *pump, int pow,
                                          int tou, int key, int skey)
{
  pump->source_p = player;
  pump->source_c = card;
  pump->pow_mod = pow;
  pump->tou_mod = tou;
  pump->key_mod = key;
  pump->skey_mod = skey;
  pump->paue_flags = 0;
  pump->can_block_additional_creatures = 0;
  pump->eot_removal_method = 0;
  pump->subtype = SUBTYPE_INVALID;
  pump->color = COLOR_TEST_0;
  pump->disabling_mode = 0;
  return;
}

#endif


#ifndef SHANDALAR
/* Recovered October 2022 pump effects and cleanup helpers. */
static int effect_pump_until_eot(int player, int card, event_t event);
static int pump_ability_until_eot_with_precomputed_sleight_hack_suppress(
    int player, int card, int t_player, int t_card, pump_ability_t *pump);

/* ManalinkEh 2022: 0x26775c0; original functions/functions.c:1219. */
static int effect_pump_until_eot(int player, int card, event_t event)
{
  short sVar1;
  card_instance_t *instance;
  if ((uint32_t)event - (uint32_t)EVENT_POWER < 2U)
  {
    instance = get_card_instance(player, card);
    if ((instance->damage_target_card == affected_card) &&
        (instance->damage_target_player == affected_card_controller))
    {
      if (event == EVENT_POWER)
      {
        sVar1 = instance->counter_power;
      }
      else
      {
        sVar1 = instance->counter_toughness;
      }
      event_result = event_result + sVar1;
    }
  }
  else if (event == EVENT_CLEANUP)
  {
    kill_card(player, card, KILL_EXILE);
    return 0;
  }
  return 0;
}


/* ManalinkEh 2022: 0x267a790; original functions/functions.c:1480. */
void get_sleighted_hacked_suppressed_ability(int player, int card, int t_player,
                                                  int *p_ability, int *p_sp_ability)
{
  unsigned int uVar1;
  unsigned int uVar2;
  card_instance_t *instance;
  unsigned int uVar4;
  unsigned int uVar5;
  unsigned int uVar6;
  unsigned int uVar7;

  uVar5 = *p_ability;
  uVar6 = *p_sp_ability;
  if ((uVar5 & 0x1000f81f) != 0)
  {
    if ((uVar5 & 0x10000000) == 0)
    {
      uVar2 = uVar5 & 0xf800;
      if (uVar2 != 0)
      {
        uVar5 = uVar5 & 0xffff07ff;
        uVar4 = 0;
        do
        {
          while (uVar1 = uVar4 + 1, (uVar2 & 0x800 << ((unsigned char)uVar4 & 0x1f)) == 0)
          {
            uVar4 = uVar1;
            if (uVar1 == 5)
              goto LAB_0267a887;
          }
          instance = get_card_instance(player, card);
          uVar7 = uVar1;
          if (instance->color_id[uVar4 + 1] != 0)
          {
            uVar7 = (unsigned int)instance->color_id[uVar4 + 1];
          }
          uVar5 = uVar5 | 0x800 << ((char)uVar7 - 1U & 0x1f);
          uVar4 = uVar1;
        } while (uVar1 != 5);
      }
    LAB_0267a887:
      uVar2 = uVar5 & 0x1f;
      if (uVar2 != 0)
      {
        uVar5 = uVar5 & 0xffffffe0;
        uVar4 = 0;
        do
        {
          while (uVar1 = uVar4 + 1, (uVar2 & 1 << ((unsigned char)uVar4 & 0x1f)) == 0)
          {
            uVar4 = uVar1;
            if (uVar1 == 5)
              goto LAB_0267a7b9;
          }
          instance = get_card_instance(player, card);
          uVar7 = uVar1;
          if (instance->hack_mode[uVar4 + 1] != 0)
          {
            uVar7 = (unsigned int)instance->hack_mode[uVar4 + 1];
          }
          uVar5 = uVar5 | 1 << ((char)uVar7 - 1U & 0x1f);
          uVar4 = uVar1;
        } while (uVar1 != 5);
      }
    }
    else
    {
      uVar5 = uVar5 & 0xefffffff;
    }
  }
LAB_0267a7b9:
  uVar2 = player_bits[t_player];
  if ((uVar2 & 0x1001f0) != 0)
  {
    if ((uVar2 & 0x10) != 0)
    {
      uVar5 = uVar5 & 0xfffffeff;
    }
    if ((uVar2 & 0x20) != 0)
    {
      uVar5 = uVar5 & 0xffffffdf;
    }
    if ((uVar2 & 0x40) != 0)
    {
      uVar6 = uVar6 & 0xfffffdff;
    }
    if ((uVar2 & 0x80) != 0)
    {
      uVar5 = uVar5 & 0xffffff7f;
    }
    if ((uVar2 & 0x100000) != 0)
    {
      uVar5 = uVar5 & 0xfffdffff;
    }
    if ((uVar2 & 0x100) != 0)
    {
      uVar6 = uVar6 & 0xff7fffff;
    }
  }
  *p_ability = uVar5;
  *p_sp_ability = uVar6;
  return;
}


/* ManalinkEh 2022: 0x267ab60; original functions/functions.c:1234. */
int pump_until_eot(int player, int card, int t_player, int t_card, int power, int toughness)
{
  int card_00;
  card_instance_t *instance;

  card_00 =
      create_targetted_legacy_effect(player, card, effect_pump_until_eot, t_player, t_card);
  instance = get_card_instance(player, card_00);
  instance->counter_power = (int16_t)power;
  instance->counter_toughness = (int16_t)toughness;
  instance = in_play(t_player, t_card);
  if (instance != NULL)
  {
    instance->regen_status = instance->regen_status | 0x8000000;
    get_abilities(t_player, t_card, EVENT_ABILITIES, -1);
  }
  return card_00;
}


/* ManalinkEh 2022: 0x267ad30; original functions/functions.c:1288. */
int pump_until_eot_merge_previous(int player, int card, int t_player, int t_card, int power,
                                       int toughness)
{
  int result;
  result = pump_until_eot_merge_previous_alt(player, card, t_player, t_card, power, toughness, 0);
  return result;
}


/* ManalinkEh 2022: 0x267ad70; original functions/functions.c:1533. */
static int pump_ability_until_eot_with_precomputed_sleight_hack_suppress(
    int player, int card, int t_player, int t_card, pump_ability_t *pump)
{
  keyword_t kVar1;
  subtype_t sVar2;
  color_test_t cVar3;
  int card_00;
  card_instance_t *instance;

  if ((pump->paue_flags & 0x10000) != 0)
  {
    untap_card(t_player, t_card);
  }
  card_00 = create_targetted_legacy_effect(player, card, legacy_effect_pump_ability_until_eot,
                                           t_player, t_card);
  instance = get_card_instance(player, card_00);
  instance->number_of_targets = '\x01';
  instance->targets[0].player = t_player;
  kVar1 = pump->key_mod;
  instance->targets[0].card = t_card;
  instance->targets[1].player = kVar1;
  instance->targets[1].card = pump->skey_mod;
  instance->counter_power = (int16_t)pump->pow_mod;
  instance->counter_toughness = (int16_t)pump->tou_mod;
  instance->targets[3].player = pump->paue_flags;
  instance->targets[3].card = pump->eot_removal_method;
  instance->targets[5].player = pump->can_block_additional_creatures;
  instance->targets[6].player = pump->source_p;
  instance->targets[6].card = pump->source_c;
  sVar2 = pump->subtype;
  cVar3 = pump->color;
  instance->targets[4].player = 1;
  instance->targets[8].player = sVar2;
  instance->targets[8].card = cVar3;
  instance = in_play(t_player, t_card);
  if (instance != NULL)
  {
    instance->regen_status = instance->regen_status | 0x8000000;
    get_abilities(t_player, t_card, EVENT_ABILITIES, -1);
  }
  return card_00;
}


/* ManalinkEh 2022: 0x267ae90; original functions/functions.c:1563. */
int pump_ability(int player, int card, int t_player, int t_card, pump_ability_t *pump)
{
  int result;
  if ((pump->key_mod != KEYWORD_0) || (pump->skey_mod != SP_KEYWORD_0))
  {
    get_sleighted_hacked_suppressed_ability(player, card, t_player, &pump->key_mod,
                                                 &pump->skey_mod);
  }
  result = pump_ability_until_eot_with_precomputed_sleight_hack_suppress(
      player, card, t_player, t_card, pump);
  return result;
}


/* ManalinkEh 2022: 0x267af00; original functions/functions.c:1570. */
int pump_ability_until_eot(int player, int card, int t_player, int t_card, int power,
                                int toughness, int ability, int sp_ability)
{
  int result;
  pump_ability_t pump;
  pump.skey_mod = sp_ability;
  pump.paue_flags = ((sp_ability ^ 0xffffffffU) >> 0x1f) << 9;
  pump.pow_mod = power;
  pump.tou_mod = toughness;
  pump.key_mod = ability;
  pump.source_p = player;
  pump.source_c = card;
  pump.can_block_additional_creatures = 0;
  pump.eot_removal_method = 0;
  pump.subtype = SUBTYPE_INVALID;
  pump.color = COLOR_TEST_0;
  pump.disabling_mode = 0;
  result = pump_ability(player, card, t_player, t_card, &pump);
  return result;
}


/* ManalinkEh 2022: 0x267af90; original functions/functions.c:1633. */
int can_block_additional_until_eot(int player, int card, int t_player, int t_card,
                                        int num_additional)
{
  int card_00;
  card_instance_t *instance;

  card_00 = create_targetted_legacy_effect(player, card, legacy_effect_pump_ability_until_eot,
                                           t_player, t_card);
  instance = get_card_instance(player, card_00);
  instance->targets[0].player = t_player;
  instance->targets[0].card = t_card;
  instance->targets[1].player = 0;
  instance->targets[1].card = 0;
  instance->counter_power = 0;
  instance->counter_toughness = 0;
  instance->targets[3].player = 0x200;
  instance->targets[4].player = 1;
  instance->targets[5].player = num_additional;
  return card_00;
}


/* ManalinkEh 2022: 0x267b300; original functions/functions.c:1607. */
int pump_ability_until_eot_no_repeat(int player, int card, int t_player, int t_card,
                                          int ability, int sp_ability)
{
  int result;
  result =
      pump_ability_until_eot_no_repeat_alt(player, card, t_player, t_card, ability, sp_ability, -1);
  return result;
}


/* ManalinkEh 2022: 0x26825d0; original functions/functions.c:1612. */
void pump_ability_by_test(int player, int card, int t_player, pump_ability_t *pump,
                               test_definition_t *this_test)
{
  card_instance_t *instance;
  int iVar2;
  int card_00;
  int player_00;
  int iVar3;

  if ((pump->key_mod != KEYWORD_0) || (pump->skey_mod != SP_KEYWORD_0))
  {
    get_sleighted_hacked_suppressed_ability(player, card, t_player, &pump->key_mod,
                                                 &pump->skey_mod);
  }
  iVar3 = 0;
  iVar2 = current_turn;
  player_00 = current_turn;
LAB_026825fb:
  if (iVar3 == 0)
    goto LAB_02682622;
LAB_026825ff:
  player_00 = 1 - iVar2;
  if (((t_player == player_00) || (t_player == 2)) &&
      (card_00 = active_cards_count[player_00] + -1, -1 < card_00))
    goto LAB_026826ce;
LAB_02682611:
  do
  {
    iVar3 = iVar3 + 1;
    iVar2 = current_turn;
    if (1 < iVar3)
    {
      return;
    }
    while (true)
    {
      player_00 = iVar2;
      if (iVar3 != 0)
        goto LAB_026825ff;
    LAB_02682622:
      if ((t_player != iVar2) && (t_player != 2))
      {
        iVar3 = 1;
        goto LAB_026825fb;
      }
      card_00 = active_cards_count[iVar2] + -1;
      if (-1 < card_00)
        break;
      iVar3 = 1;
    }
  LAB_026826ce:
    do
    {
      while (((instance = in_play(player_00, card_00),
               instance == NULL || (iVar2 = is_what(player_00, card_00, 2), iVar2 == 0)) ||
              ((this_test != NULL &&
                ((iVar2 = new_make_test_in_play(player_00, card_00, -1, this_test),
                  iVar2 == 0 || ((this_test->not_me != 0 &&
                                  ((this_test->not_me != 1 ||
                                    ((player == player_00 && (card == card_00)))))))))))))
      {
        card_00 = card_00 + -1;
        if (card_00 == -1)
          goto LAB_02682611;
      }
      iVar2 = card_00 + -1;
      pump_ability_until_eot_with_precomputed_sleight_hack_suppress(player, card, player_00,
                                                                         card_00, pump);
      card_00 = iVar2;
    } while (iVar2 != -1);
  } while (true);
}


/* ManalinkEh 2022: 0x2682750; original functions/functions.c:1737. */
void pump_creatures_until_eot_merge_pt_alternate_legacy_text(int player, int card,
                                                                  int t_player, int power,
                                                                  int toughness,
                                                                  test_definition_t *this_test,
                                                                  int alt_text_number)
{
  card_instance_t *instance;
  int iVar2;
  int card_00;
  int iVar3;
  int player_00;
  int iStack_18;

  iStack_18 = 0;
  if (this_test != NULL)
  {
    iStack_18 = new_get_test_score(this_test);
  }
  player_00 = 0;
  do
  {
    if (((t_player == player_00) || (t_player == 2)) &&
        (card_00 = active_cards_count[player_00] + -1, -1 < card_00))
    {
      do
      {
        while ((((instance = in_play(player_00, card_00),
                  instance == NULL || (iVar2 = is_what(player_00, card_00, 2), iVar2 == 0)) ||
                 ((this_test != NULL &&
                   (((this_test->not_me != 0 &&
                      ((this_test->not_me != 1 || ((player == player_00 && (card == card_00)))))) ||
                     (iVar2 = new_make_test_in_play(player_00, card_00, iStack_18, this_test),
                      iVar2 == 0)))))) ||
                ((instance->state & 0x100000) != 0)))
        {
          card_00 = card_00 + -1;
          if (card_00 == -1)
            goto LAB_0268278a;
        }
        iVar2 = alt_text_number;
        if (alt_text_number < 1)
        {
          iVar2 = 0;
        }
        iVar3 = card_00 + -1;
        pump_until_eot_merge_previous_alt(player, card, player_00, card_00, power, toughness,
                                          iVar2);
        card_00 = iVar3;
      } while (iVar3 != -1);
    }
  LAB_0268278a:
    player_00 = player_00 + 1;
    if (player_00 == 2)
    {
      return;
    }
  } while (true);
}


/* ManalinkEh 2022: 0x2682890; original functions/functions.c:1761. */
void pump_creatures_until_eot_merge_pt(int player, int card, int t_player, int power,
                                            int toughness, test_definition_t *this_test)
{
  pump_creatures_until_eot_merge_pt_alternate_legacy_text(player, card, t_player, power,
                                                               toughness, this_test, 0);
  return;
}


/* ManalinkEh 2022: 0x2685dd0; original functions/functions.c:1648. */
void pump_creatures_until_eot(int player, int card, int t_player,
                                   int alternate_legacy_text_number, int power, int toughness,
                                   int ability, int sp_ability, test_definition_t *this_test)
{
  unsigned char bVar1;
  card_instance_t *instance;
  int result;
  int card_00;
  int t_player_00;
  int iStack_50;
  int shs_ability;
  int shs_sp_ability;
  pump_ability_t pump;

  iStack_50 = 0;
  if (this_test != NULL)
  {
    iStack_50 = new_get_test_score(this_test);
  }
  instance = get_card_instance(player, card);
  if (instance->internal_card_id == activation_card)
  {
    player = instance->parent_controller;
    card = instance->parent_card;
  }
  t_player_00 = 0;
  do
  {
    if ((t_player == t_player_00) || (t_player == 2))
    {
      shs_ability = ability;
      shs_sp_ability = sp_ability;
      if (ability == 0 && sp_ability == 0)
      {
        pump.key_mod = KEYWORD_0;
        pump.skey_mod = SP_KEYWORD_0;
      LAB_02685ea9:
        pump.paue_flags = 0x200;
      }
      else
      {
        get_sleighted_hacked_suppressed_ability(player, card, t_player_00, &shs_ability,
                                                     &shs_sp_ability);
        pump.key_mod = shs_ability;
        pump.skey_mod = shs_sp_ability;
        pump.paue_flags = 0;
        if (-1 < sp_ability)
          goto LAB_02685ea9;
      }
      pump.tou_mod = toughness;
      pump.pow_mod = power;
      pump.source_c = card;
      pump.source_p = player;
      pump.disabling_mode = 0;
      pump.color = COLOR_TEST_0;
      pump.subtype = SUBTYPE_INVALID;
      pump.eot_removal_method = 0;
      pump.can_block_additional_creatures = 0;
      card_00 = active_cards_count[t_player_00] + -1;
      if (-1 < card_00)
      {
        do
        {
          instance = in_play(t_player_00, card_00);
          if ((instance != NULL) && (result = is_what(t_player_00, card_00, 2), result != 0))
          {
            if (this_test == NULL)
            {
              bVar1 = ((uint32_t)instance->state >> 16 & 0xffU);
            }
            else
            {
              if (((this_test->not_me != 0) &&
                   ((this_test->not_me != 1 || ((player == t_player_00 && (card == card_00)))))) ||
                  (result = new_make_test_in_play(t_player_00, card_00, iStack_50, this_test),
                   result == 0))
                goto LAB_02685ee0;
              bVar1 = ((uint32_t)instance->state >> 16 & 0xffU);
            }
            if ((((bVar1 & 0x10) == 0) &&
                 (result = pump_ability_until_eot_with_precomputed_sleight_hack_suppress(
                      player, card, t_player_00, card_00, &pump),
                  -1 < result)) &&
                (0 < alternate_legacy_text_number))
            {
              alternate_legacy_text(alternate_legacy_text_number, player, result);
            }
          }
        LAB_02685ee0:
          card_00 = card_00 + -1;
        } while (card_00 != -1);
      }
    }
    t_player_00 = t_player_00 + 1;
    if (t_player_00 == 2)
    {
      return;
    }
  } while (true);
}


/* ManalinkEh 2022: 0x2686030; original functions/functions.c:1766. */
int pump_subtype_until_eot(int player, int card, int t_player, int subtype, int power,
                                int toughness, int ability, int sp_ability)
{
  test_definition_t this_test;
  new_default_test_definition(&this_test, 2, "");
  if (0 < subtype)
  {
    this_test.subtype = subtype;
  }
  pump_creatures_until_eot(player, card, t_player, 0, power, toughness, ability, sp_ability,
                                &this_test);
  return 0;
}


/* ManalinkEh 2022: 0x26860b0; original functions/functions.c:1778. */
void pump_color_until_eot(int player, int card, int t_player, int clr, int power,
                               int toughness, int ability, int sp_ability)
{
  test_definition_t this_test;
  default_test_definition(&this_test, 2);
  if (0 < clr)
  {
    this_test.color = clr;
  }
  pump_creatures_until_eot(player, card, t_player, 0, power, toughness, ability, sp_ability,
                                &this_test);
  return;
}


/* ManalinkEh 2022: 0x2686520; original functions/functions.c:1689. */
void pump_creatures_until_eot_no_repeat(int player, int card, int t_player,
                                             int alternate_legacy_text_number, int ability,
                                             int sp_ability, test_definition_t *this_test)
{
  card_instance_t *instance;
  int result;
  unsigned int uVar3;
  int card_00;
  int t_player_00;
  target_t tVar4;
  int iStack_50;
  int shs_ability;
  int shs_sp_ability;
  pump_ability_t pump;

  iStack_50 = 0;
  if (this_test != NULL)
  {
    iStack_50 = new_get_test_score(this_test);
  }
  instance = get_card_instance(player, card);
  if (instance->internal_card_id == activation_card)
  {
    player = instance->parent_controller;
    card = instance->parent_card;
  }
  t_player_00 = 0;
  do
  {
    if ((t_player == t_player_00) || (t_player == 2))
    {
      shs_ability = ability;
      shs_sp_ability = sp_ability;
      if (ability == 0 && sp_ability == 0)
      {
        pump.key_mod = KEYWORD_0;
        pump.skey_mod = SP_KEYWORD_0;
      LAB_026865fb:
        pump.paue_flags = 0x200;
      }
      else
      {
        get_sleighted_hacked_suppressed_ability(player, card, t_player_00, &shs_ability,
                                                     &shs_sp_ability);
        pump.key_mod = shs_ability;
        pump.skey_mod = shs_sp_ability;
        pump.paue_flags = 0;
        if (-1 < sp_ability)
          goto LAB_026865fb;
      }
      pump.source_c = card;
      pump.source_p = player;
      pump.disabling_mode = 0;
      pump.color = COLOR_TEST_0;
      pump.subtype = SUBTYPE_INVALID;
      pump.eot_removal_method = 0;
      pump.can_block_additional_creatures = 0;
      pump.tou_mod = 0;
      pump.pow_mod = 0;
      card_00 = active_cards_count[t_player_00] + -1;
      if (-1 < card_00)
      {
        do
        {
          instance = in_play(t_player_00, card_00);
          if ((((instance != NULL) && (result = is_what(t_player_00, card_00, 2), result != 0)) &&
               ((this_test == NULL ||
                 (((this_test->not_me == 0 ||
                    ((this_test->not_me == 1 && ((player != t_player_00 || (card != card_00)))))) &&
                   (result = new_make_test_in_play(t_player_00, card_00, iStack_50, this_test),
                    result != 0)))))) &&
              ((((instance->state & 0x100000) == 0 &&
                 (((shs_ability != (instance->regen_status & shs_ability) ||
                    (uVar3 = get_special_abilities_by_instance(instance),
                     shs_sp_ability != (uVar3 & shs_sp_ability))) ||
                   (tVar4 = find_repeat_legacy_effect_alt(player, card,
                                                          legacy_effect_pump_ability_until_eot,
                                                          t_player_00, card_00, -1, player),
                    tVar4.card == -1)))) &&
                ((result = pump_ability_until_eot_with_precomputed_sleight_hack_suppress(
                      player, card, t_player_00, card_00, &pump),
                  -1 < result && (0 < alternate_legacy_text_number))))))
          {
            alternate_legacy_text(alternate_legacy_text_number, player, result);
          }
          card_00 = card_00 + -1;
        } while (card_00 != -1);
      }
    }
    t_player_00 = t_player_00 + 1;
    if (t_player_00 == 2)
    {
      return;
    }
  } while (true);
}


/* ManalinkEh 2022: 0x26a1550; original functions/functions.c:11588. */
int vanilla_pump(int player, int card, event_t event, target_definition_t *td, int power,
                      int toughness, int keyword, int sp_keyword)
{
  bool bVar1;
  uint32_t puVar2;
  card_instance_t *instance;
  int result;
  const char *prompt;
  target_definition_t *ptVar5;
  target_definition_t *ptVar6;
  target_definition_t *ptVar7;
  target_definition_t td_prompt;
  target_definition_t td_regen;
  puVar2 = ((uint32_t)keyword & 0x1f000000U);
  ptVar5 = td;
  ptVar6 = &td_prompt;
  td_prompt = *td;
  if (td_prompt.allowed_controller == player)
  {
    prompt = "TARGET_CREATURE_YOU_CONTROL";
  }
  else
  {
    prompt = "TARGET_CREATURE";
    if (td_prompt.allowed_controller == 1 - player)
    {
      prompt = "TARGET_CREATURE_OPPONENTS_CONTROL";
    }
  }
  bVar1 = puVar2 == 0x0a000000U || puVar2 == 0x18000000U;
  if (bVar1)
  {
    ptVar5 = &td_regen;
    ptVar6 = td;
    ptVar7 = ptVar5;
    td_regen = *td;
    td_regen.required_state = td_regen.required_state | 0x4000;
    if (player == 1)
    {
      td_regen.special = td_regen.special | 0x800000;
    }
    if (event == EVENT_CAN_CAST)
    {
      result = 0x11;
      event = EVENT_CAN_CAST;
      goto LAB_026a15f2;
    }
    if (event != EVENT_CAST_SPELL)
      goto LAB_026a15be;
    if ((card == affected_card) && (player == affected_card_controller))
    {
      result = 0x11;
      event = EVENT_CAST_SPELL;
      goto LAB_026a15f2;
    }
  }
  else
  {
  LAB_026a15be:
    if (event == EVENT_RESOLVE_SPELL)
    {
      result = valid_target(td);
      if (result != 0)
      {
        instance = get_card_instance(player, card);
        if ((keyword & 0xe0ffffffU) == 0 && sp_keyword == 0)
        {
          result = pump_until_eot(player, card, instance->targets[0].player,
                                       instance->targets[0].card, power, toughness);
        }
        else
        {
          result = pump_ability_until_eot(player, card, instance->targets[0].player,
                                               instance->targets[0].card, power, toughness,
                                               keyword & 0xe0ffffffU, sp_keyword);
        }
        if (puVar2 == 0x3000000U)
        {
          if (-1 < result)
          {
            alternate_legacy_text(1, player, result);
          }
          result = cantrip(player, card, 1);
          if (-1 < result)
          {
            alternate_legacy_text(2, player, result);
          }
        }
        else if (puVar2 < 0x3000001U)
        {
          if (puVar2 == 0x1000000U)
          {
            draw_a_card(player);
          }
          else if (puVar2 == 0x2000000U)
          {
            scry(player, 1);
          }
        }
        else if ((puVar2 == 0x0a000000U) || (bVar1))
        {
          if (-1 < result)
          {
            alternate_legacy_text(1, player, result);
          }
          result = regenerate_or_shield(player, card, instance->targets[0].player,
                                        instance->targets[0].card);
          if (-1 < result)
          {
            alternate_legacy_text(2, player, result);
          }
          if (puVar2 == 0x18000000U)
          {
            return 1;
          }
        }
        else if (puVar2 == 0x4000000U)
        {
          if (-1 < result)
          {
            alternate_legacy_text(1, player, result);
          }
          return 1;
        }
      }
      kill_card(player, card, KILL_DESTROY);
    }
  }
  result = 1;
  ptVar5 = td;
LAB_026a15f2:
  result = generic_spell(player, card, event, result, ptVar5, prompt, 1, NULL);
  return result;
}


/* ManalinkEh 2022: 0x26a1900; original functions/functions.c:11658. */
int vanilla_instant_pump(int player, int card, event_t event, int allowed_controller,
                              int preferred_controller, int power, int toughness, int keyword,
                              int sp_keyword)
{
  int result;
  card_instance_t *instance;
  target_definition_t td;
  if (event != EVENT_CHECK_PUMP)
  {
    if (event != EVENT_CAN_CAST)
    {
      if (event == EVENT_CAST_SPELL)
      {
        if ((card != affected_card) || (player != affected_card_controller))
        {
          return 0;
        }
      }
      else if ((event != EVENT_RESOLVE_SPELL && event != EVENT_CAN_CHANGE_TARGET) &&
               (event != EVENT_CHANGE_TARGET))
      {
        return 0;
      }
    }
    default_target_definition(player, card, &td, 2);
    td.preferred_controller = preferred_controller;
    td.allowed_controller = allowed_controller;
    result = vanilla_pump(player, card, event, &td, power, toughness, keyword, sp_keyword);
    return result;
  }
  if ((power == 0 && (sp_keyword & 0x800U) == 0) && toughness == 0)
  {
    return 0;
  }
  instance = in_hand(player, card);
  if (instance != NULL)
  {
    result = has_mana_to_cast_iid(player, EVENT_CAN_CAST, instance->internal_card_id);
    if (result == 0)
    {
      return 0;
    }
    if ((sp_keyword & 0x800U) != 0)
    {
      toughness = 99;
    }
    result = 1 - player;
    if ((result == preferred_controller) || (preferred_controller == 2))
    {
      if (power < 0)
      {
        pumpable_power[result] = pumpable_power[result] + power;
      }
      if (toughness < 0)
      {
        pumpable_toughness[result] = pumpable_toughness[result] + toughness;
      }
    }
    if ((player != preferred_controller) && (preferred_controller != 2))
    {
      return 0;
    }
    if (0 < power)
    {
      pumpable_power[player] = pumpable_power[player] + power;
    }
    if (0 < toughness)
    {
      pumpable_toughness[player] = pumpable_toughness[player] + toughness;
      return 0;
    }
    return 0;
  }
  return 0;
}


/* ManalinkEh 2022: 0x26a2390; original functions/functions.c:1308. */
int legacy_effect_pump_ability_until_eot(int player, int card, event_t event)
{
  subtype_t subtype;
  color_test_t color;
  int player_00;
  bool bVar1;
  bool bVar2;
  card_instance_t *pcVar3;
  unsigned int uVar4;
  card_instance_t *pcVar5;
  int result;
  kill_t kVar7;

  pcVar3 = get_card_instance(player, card);
  uVar4 = pcVar3->targets[1].player;
  if ((((uint32_t)event - (uint32_t)EVENT_POWER < 3U) &&
       (pcVar3->targets[0].card == affected_card)) &&
      (pcVar3->targets[0].player == affected_card_controller))
  {
    if (event == EVENT_POWER)
    {
      event_result = pcVar3->counter_power + event_result;
    }
    else if (event == EVENT_TOUGHNESS)
    {
      event_result = pcVar3->counter_toughness + event_result;
    }
    else if (pcVar3->targets[4].player == 1)
    {
      event_result = event_result | uVar4;
    }
    else
    {
      event_result = (uVar4 ^ 0xffffffff) & event_result;
    }
  }
  if (uVar4 == 0xffffffff)
  {
    pcVar3->targets[1].player = 0;
  }
  result = pcVar3->targets[1].card;
  if (result == -1)
  {
    pcVar3->targets[1].card = 0;
  }
  else if (result != 0)
  {
    special_abilities(pcVar3->targets[0].player, pcVar3->targets[0].card, event, result,
                           player, card);
  }
  uVar4 = pcVar3->targets[4].card;
  if (0 < (int)uVar4)
  {
    if ((uVar4 & 3) == 3)
    {
      result = pcVar3->targets[0].player;
      spirit_link_effect(result, pcVar3->targets[0].card, event, result);
    }
    else
    {
      if ((uVar4 & 1) != 0)
      {
        spirit_link_effect(pcVar3->targets[0].player, pcVar3->targets[0].card, event, 0);
        uVar4 = pcVar3->targets[4].card;
      }
      if ((uVar4 & 2) != 0)
      {
        spirit_link_effect(pcVar3->targets[0].player, pcVar3->targets[0].card, event, 1);
      }
    }
  }
  uVar4 = pcVar3->targets[3].player;
  if (0 < (int)uVar4)
  {
    if ((uVar4 & 0x10) == 0)
    {
      if ((uVar4 & 0x20) == 0)
        goto joined_r0x026a27de;
    LAB_026a27a4:
      leaves_play(pcVar3->targets[0].player, pcVar3->targets[0].card, event, 0, 0);
      if (event != EVENT_LEAVES_PLAY_ABILITY)
      {
        uVar4 = pcVar3->targets[3].player;
        goto joined_r0x026a27de;
      }
      if ((pcVar3->targets[0].player == trigger_cause_controller) &&
          ((pcVar3->targets[0].card == trigger_cause &&
            (pcVar5 = in_play((int)pcVar3->damage_source_player, pcVar3->damage_source_card),
             pcVar5 != NULL))))
      {
        kVar7 = pcVar3->targets[3].card;
        if (3U < (uint32_t)kVar7 - (uint32_t)KILL_BURY)
        {
          kVar7 = KILL_SACRIFICE;
        }
        kill_card((int)pcVar3->damage_source_player, pcVar3->damage_source_card, kVar7);
      }
      if ((pcVar3->targets[3].player & 8) == 0)
        goto LAB_026a2435;
      leaves_play((int)pcVar3->damage_source_player, pcVar3->damage_source_card,
                       EVENT_LEAVES_PLAY_ABILITY, 0, 0);
    }
    else
    {
      cannot_attack(pcVar3->targets[0].player, pcVar3->targets[0].card, event);
      uVar4 = pcVar3->targets[3].player;
      if ((uVar4 & 0x20) != 0)
        goto LAB_026a27a4;
    joined_r0x026a27de:
      if (((uVar4 & 8) == 0) || (leaves_play((int)pcVar3->damage_source_player,
                                                  pcVar3->damage_source_card, event, 0, 0),
                                 event != EVENT_LEAVES_PLAY_ABILITY))
        goto LAB_026a2435;
    }
    if (((pcVar3->damage_source_player == trigger_cause_controller) &&
         (pcVar3->damage_source_card == trigger_cause)) &&
        (pcVar5 = in_play(pcVar3->targets[0].player, pcVar3->targets[0].card), pcVar5 != NULL))
    {
      kVar7 = pcVar3->targets[3].card;
      if (3U < (uint32_t)kVar7 - (uint32_t)KILL_BURY)
      {
        kVar7 = KILL_SACRIFICE;
      }
      kill_card(pcVar3->targets[0].player, pcVar3->targets[0].card, kVar7);
    }
  }
LAB_026a2435:
  result = pcVar3->targets[5].player;
  if (0 < result)
  {
    event_flags = event_flags | 0x100000;
    attached_creature_can_block_additional(player, card, event, result);
  }
  subtype = pcVar3->targets[8].player;
  uVar4 = pcVar3->targets[3].player;
  if (0 < (int)subtype)
  {
    if ((uVar4 & 0x40000) == 0)
    {
      attached_is_additional_subtype(player, card, event, subtype);
      uVar4 = pcVar3->targets[3].player;
    }
    else
    {
      attached_is_forced_subtype(player, card, event, subtype);
      uVar4 = pcVar3->targets[3].player;
    }
  }
  if (0 < pcVar3->targets[8].card)
  {
    /* Native 2022 code tests the color slot, then reads this subtype slot. */
    color = pcVar3->targets[8].player;
    if ((uVar4 & 0x20000) == 0)
    {
      attached_is_additional_color(player, card, event, color);
      uVar4 = pcVar3->targets[3].player;
    }
    else
    {
      attached_is_forced_color(player, card, event, color);
      uVar4 = pcVar3->targets[3].player;
    }
  }
  bVar2 = false;
  if ((uVar4 & 0x200) != 0)
  {
    if ((pcVar3->targets[1].card & 0x10000000U) == 0 && (uVar4 & 0x4000) == 0)
    {
      bVar2 = false;
      if (event == EVENT_CLEANUP)
      {
        result = pcVar3->targets[7].player;
      joined_r0x026a2770:
        bVar2 = true;
        if (0 < result)
        {
          bVar2 = false;
          pcVar3->targets[7].player = result + -1;
        }
      }
    }
    else
    {
      result = eot_trigger(player, card, event);
      uVar4 = pcVar3->targets[3].player;
      bVar2 = false;
      if (result != 0)
      {
        result = pcVar3->targets[7].player;
        goto joined_r0x026a2770;
      }
    }
  }
  bVar1 = bVar2;
  if ((uVar4 & 0x40) != 0)
  {
    result = end_of_combat_trigger(player, card, event, 2);
    uVar4 = pcVar3->targets[3].player;
    bVar1 = true;
    if (result == 0)
    {
      bVar1 = bVar2;
    }
  }
  if ((uVar4 & 0x180) != 0)
  {
    leaves_play(pcVar3->targets[6].player, pcVar3->targets[6].card, event, 0, 0);
    uVar4 = pcVar3->targets[3].player;
    if (event == EVENT_LEAVES_PLAY_ABILITY)
    {
      bVar1 = true;
    }
  }
  if ((uVar4 & 0x80) == 0)
  {
  LAB_026a2522:
    if (((((uVar4 & 0x8000) == 0) || (current_turn != player)) || (current_phase != 1)) && (!bVar1))
      goto LAB_026a2536;
  }
  else
  {
    result = pcVar3->targets[6].player;
    player_bits[result] = player_bits[result] | 4;
    if (((event != EVENT_UNTAP_CARD) || (pcVar3->targets[6].card != affected_card)) ||
        (result != affected_card_controller))
      goto LAB_026a2522;
  }
  pcVar5 = get_card_instance(pcVar3->targets[0].player, pcVar3->targets[0].card);
  if ((pcVar5->state & 0x800000) == 0)
  {
    uVar4 = pcVar3->targets[3].player;
    if ((uVar4 & 4) != 0)
    {
      humiliate(player, card, pcVar3->targets[0].player, pcVar3->targets[0].card, 0);
      uVar4 = pcVar3->targets[3].player;
    }
    if ((uVar4 & 2) != 0)
    {
      disable_all_activated_abilities(pcVar3->targets[0].player, pcVar3->targets[0].card, 0);
      uVar4 = pcVar3->targets[3].player;
    }
    if ((uVar4 & 1) != 0)
    {
      disable_nonmana_activated_abilities(pcVar3->targets[0].player, pcVar3->targets[0].card,
                                               0);
    }
    if (((pcVar3->targets[1].card & 0x10000000) != 0) ||
        ((pcVar3->targets[3].player & 0x4000) != 0))
    {
      kVar7 = pcVar3->targets[3].card;
      result = pcVar3->targets[0].card;
      player_00 = pcVar3->targets[0].player;
      if (kVar7 == (KILL_EXILE | KILL_BURY))
      {
        bounce_permanent(player_00, result);
      }
      else
      {
        kill_card(player_00, result, kVar7);
      }
    }
  }
  kill_card(player, card, KILL_EXILE);
LAB_026a2536:
  uVar4 = pcVar3->targets[5].card;
  return uVar4 & (int)(uVar4 ^ 0xffffffff) >> 0x1f;
}


/* ManalinkEh 2022: 0x2681550; original functions/functions.c:7536. */
int cannot_attack(int player, int card, event_t event)
{
  card_instance_t *instance;
  if ((((event == EVENT_ABILITIES) && (card == affected_card)) &&
       (player == affected_card_controller)) &&
      ((-1 < player && (-1 < card))))
  {
    instance = get_card_instance(player, card);
    instance->token_status = instance->token_status | 0x8000;
  }
  return 0;
}

#endif


#ifndef SHANDALAR
/* Recovered October 2022 activated-ability core. */
typedef union
{
  target_definition_t target;
  test_definition_t test;
  target_t selected;
} activated_storage_t;

/* ManalinkEh 2022: 0x269c8c0; original functions/functions.c:13396. */
int granted_generic_activated_ability(int granting_player, int granting_card,
                                           int granted_to_player, int granted_to_card,
                                           event_t event, int mode, int cless, int black, int blue,
                                           int green, int red, int white, uint32_t variable_costs,
                                           target_definition_t *td, const char *prompt)
{
  uint8_t uVar1;
  char cVar2;
  card_instance_t *pcVar3;
  int iVar4;
  unsigned int uVar5;
  unsigned int uVar6;
  card_instance_t *pcVar7;
  int iVar8;
  unsigned int uVar9;
  target_definition_t *td_00;
  card_instance_t *pcVar10;
  unsigned int uVar11;
  unsigned int uVar12;
  unsigned int uStack_f0;
  activated_storage_t storage;
  if (event == EVENT_CLEANUP)
  {
    pcVar3 = get_card_instance(granting_player, granting_card);
    if (granting_card != affected_card)
    {
      return 0;
    }
    if (granting_player != affected_card_controller)
    {
      return 0;
    }
    if ((mode & 0x2000U) == 0)
    {
      return 0;
    }
    pcVar3->targets[2].player = 0;
    return 0;
  }
  cVar2 = (char)(variable_costs >> 0x18);
  if (event == EVENT_CAN_ACTIVATE)
  {
    pcVar3 = get_card_instance(granting_player, granting_card);
    pcVar7 = get_card_instance(granted_to_player, granted_to_card);
    if ((mode & 1U) != 0)
    {
      iVar4 = is_tapped(granted_to_player, granted_to_card);
      if (iVar4 != 0)
      {
        return 0;
      }
      pcVar10 = get_card_instance(granted_to_player, granted_to_card);
      if ((pcVar10->ext->special_flags2 & SF2_KATABATIC_WINDS) != 0)
      {
        return 0;
      }
    }
    if ((((mode & 0x80001U) != 0) &&
         (iVar4 = is_what(granted_to_player, granted_to_card, 2), iVar4 != 0)) &&
        (iVar4 = is_sick(granted_to_player, granted_to_card), iVar4 != 0))
    {
      return 0;
    }
    if ((mode & 0x100U) == 0)
    {
      if ((mode & 4U) != 0)
        goto LAB_0269d128;
    }
    else
    {
      iVar4 = can_sorcery_be_played(granted_to_player, EVENT_CAN_ACTIVATE);
      if (iVar4 == 0)
      {
        return 0;
      }
      iVar4 = is_what(granted_to_player, granted_to_card, 2);
      if (iVar4 != 0)
      {
        return 0;
      }
      mode = mode | 4;
      td->flags = td->flags | 5;
      cless = get_updated_equip_cost(granted_to_player, granted_to_card, cless);
    LAB_0269d128:
      if (((mode & 0x40000U) == 0) || ((pcVar7->state & 0x100000) != 0))
      {
        iVar4 = can_target(td);
      }
      else
      {
        pcVar10 = get_card_instance(granted_to_player, granted_to_card);
        pcVar10->state = pcVar10->state | 0x100000;
        iVar4 = can_target(td);
        pcVar10 = get_card_instance(granted_to_player, granted_to_card);
        if ((pcVar10->state & 0x100000) != 0)
        {
          pcVar10->state = pcVar10->state & 0xffefffff;
        }
      }
      if (iVar4 == 0)
      {
        return 0;
      }
    }
    if (((mode & 0x10U) != 0) &&
        (iVar4 = can_sorcery_be_played(granted_to_player, EVENT_CAN_ACTIVATE), iVar4 == 0))
    {
      return 0;
    }
    if (((mode & 0x20000020U) != 0) && (hand_count[granted_to_player] < 1))
    {
      return 0;
    }
    if (((mode & 0x40U) != 0) &&
        (iVar4 = count_counters(granted_to_player, granted_to_card, COUNTER_P1_P1), iVar4 < 1))
    {
      return 0;
    }
    if (((mode & 0x80U) != 0) &&
        (iVar4 = count_counters(granted_to_player, granted_to_card, COUNTER_M1_M1), iVar4 < 1))
    {
      return 0;
    }
    if (cVar2 == '\x01')
    {
      iVar4 = count_counters(granted_to_player, granted_to_card,
                             variable_costs >> 0x10 & COUNTER_invalid);
      if (iVar4 < (int)(variable_costs >> 8 & 0xff))
      {
        return 0;
      }
    }
    else if ((cVar2 == '\x02') &&
             (iVar4 = count_player_counters(granted_to_player, variable_costs >> 0x10 & 0xff),
              iVar4 < (int)(variable_costs >> 8 & 0xff)))
    {
      return 0;
    }
    if (((mode & 0x400U) != 0) &&
        (iVar4 = counterspell(granted_to_player, granted_to_card, EVENT_CAN_CAST, td, -1),
         iVar4 == 0))
    {
      return 0;
    }
    if ((((mode & 0x2000U) != 0) && (iVar4 = pcVar3->targets[2].player, 0 < iVar4)) &&
        ((iVar4 >> ((unsigned char)granted_to_player & 0x1f) & 1U) != 0))
    {
      return 0;
    }
    if ((mode & 0x8000U) != 0)
    {
      if (current_phase != 4)
      {
        return 0;
      }
      iVar4 = count_upkeeps(granted_to_player);
      if (iVar4 < 1)
      {
        return 0;
      }
    }
    if ((mode & 0x10000U) == 0)
    {
      if (((mode & 0x40000000U) != 0) && (current_turn == granted_to_player))
      {
        return 0;
      }
    }
    else
    {
      if (current_turn != granted_to_player)
      {
        return 0;
      }
      if ((mode & 0x40000000U) != 0)
      {
        return 0;
      }
    }
    if (((mode & 0x80000U) != 0) &&
        (iVar4 = is_tapped(granted_to_player, granted_to_card), iVar4 == 0))
    {
      return 0;
    }
    if (((mode & 0x200U) != 0) &&
        (iVar4 = can_sacrifice_as_cost(granted_to_player, 1, 2, 0, 0, 0, 0, 0, 0, 0, -1, 0),
         iVar4 == 0))
    {
      return 0;
    }
    if (((mode & 8U) != 0) &&
        (iVar4 = can_sacrifice_this_as_cost(granted_to_player, granted_to_card), iVar4 == 0))
    {
      return 0;
    }
    if ((((mode & 0x208U) != 0) && ((mode & 0x40000U) != 0)) &&
        (iVar4 = count_subtype(granted_to_player, 2, -1), iVar4 < 2))
    {
      return 0;
    }
    if (((mode & 0x100000U) != 0) &&
        (((ai_is_speculating == 1 ||
           ((granted_to_player == 1 && (((unsigned char)trace_mode & 2) == 0)))) &&
          (pcVar3->targets[2].card == 1))))
    {
      return 0;
    }
    if (('\0' < (char)variable_costs) &&
        (iVar4 = can_pay_life_impl(granted_to_player, (int)(char)variable_costs, 1), iVar4 == 0))
    {
      return 0;
    }
    if ((mode & 0x400000U) != 0)
    {
      td_00 = td;
      if (td == NULL)
      {
        default_target_definition(granted_to_player, granted_to_card, &storage.target, 0);
        storage.target.zone = 0x1000;
        td_00 = &storage.target;
        storage.target.allowed_controller = 1 - granted_to_player;
      }
      iVar4 = would_validate_arbitrary_target(td_00, 1 - granted_to_player, -1);
      if (iVar4 == 0)
      {
        return 0;
      }
    }
    if (((mode & 0x10000000U) != 0) && (0x14 < current_phase))
    {
      return 0;
    }
    if (((mode & 0x1000U) != 0) && (8 < current_phase - 0x15U))
    {
      return 0;
    }
    if ((mode & 0x800000U) != 0)
    {
      if ((((unsigned int)land_can_be_played >> 8 & 0xff) & 2) == 0)
      {
        return 0;
      }
      if (td == NULL)
      {
        if (pcVar7->kill_code != '\x02')
        {
          return 0;
        }
        if (granted_to_player != 1)
          goto LAB_0269cec6;
        iVar4 = can_regenerate(1, granted_to_card);
        if (iVar4 == 0)
        {
          return 0;
        }
      }
    }
    if ((mode < 0) && (granted_to_player == 1))
    {
      pcVar3 = get_card_instance(1, granted_to_card);
      if ((pcVar3->ext->special_flags & SF_TYPE_ALREADY_CHANGED) != 0)
      {
        return 0;
      }
      iVar4 = is_what(1, granted_to_card, 2);
      if (iVar4 != 0)
      {
        return 0;
      }
    }
  LAB_0269cec6:
    if ((mode & 0x800U) == 0)
    {
      if (cless == -2)
      {
        cless = 0;
      }
      iVar4 = has_mana_for_activated_ability(granted_to_player, granted_to_card, cless, black,
                                                  blue, green, red, white);
    }
    else
    {
      if (cless == -2)
      {
        cless = 0;
      }
      iVar4 = has_mana_multi(granted_to_player, cless, black, blue, green, red, white);
    }
    if (iVar4 != 0)
    {
      iVar4 = can_use_activated_abilities(granted_to_player, granted_to_card);
      if (iVar4 == 0)
      {
        if ((mode & 0x200000U) == 0)
        {
          return 0;
        }
        iVar4 = can_produce_mana(granted_to_player, granted_to_card);
        if (iVar4 == 0)
        {
          return 0;
        }
      }
      else
      {
        iVar4 = can_produce_mana(granted_to_player, granted_to_card);
        if ((iVar4 == 0) && ((mode & 0x200000U) != 0))
        {
          return 0;
        }
      }
      if ((((granting_player == granted_to_player) && (granting_card == granted_to_card)) ||
           (iVar4 = is_humiliated(granting_player, granting_card, AL_STATIC),
            iVar4 == 0)) &&
          (((unknown62BCEC != 0 && (needed_mana_colors == 0)) || ((mode & 0x200000U) != 0))))
      {
        return (-(unsigned int)((mode & 0xf800400U) == 0) & 0xffffff9e) + 99;
      }
    }
    return 0;
  }
  if (event != EVENT_ACTIVATE)
  {
    return 0;
  }
  hack_what_was_sacced_for_gaa = 0;
  pcVar3 = get_card_instance(granting_player, granting_card);
  if ((mode & 0x100U) != 0)
  {
    mode = mode | 4;
    cless = get_updated_equip_cost(granted_to_player, granted_to_card, cless);
  }
  if ((mode & 0x800U) == 0)
  {
    iVar4 = 0;
    if (cless != -2)
    {
      iVar4 = cless;
    }
    iVar4 = charge_mana_for_activated_ability(granted_to_player, granted_to_card, iVar4, black,
                                                   blue, green, red, white);
    if ((mode & 0x200000U) == 0)
    {
      tapped_for_mana_color = -1;
    }
    if (iVar4 == 0)
    {
      return 0;
    }
  }
  else
  {
    pay_mana_xbugrwaU[0] = cless;
    if (cless == -2)
    {
      pay_mana_xbugrwaU[0] = 0;
    }
    pay_mana_xbugrwaU[1] = black;
    pay_mana_xbugrwaU[2] = blue;
    pay_mana_xbugrwaU[3] = green;
    pay_mana_xbugrwaU[4] = red;
    pay_mana_xbugrwaU[5] = white;
    charge_mana(granted_to_player, COLOR_COLORLESS, 0);
    if (spell_fizzled == 1)
    {
      if ((mode & 0x200000U) != 0)
      {
        return 0;
      }
      tapped_for_mana_color = -1;
      return 0;
    }
    if ((mode & 0x200000U) == 0)
    {
      tapped_for_mana_color = -1;
    }
  }
  if (cless == -2)
  {
    iVar4 = charge_mana_for_multi_x(granted_to_player, 0, 2);
    pcVar3->info_slot = iVar4 / 2;
  }
  else if (((cless + 2U < 2) || (black == -1)) ||
           ((blue == -1 || ((green == -1 || red == -1 || (white == -1))))))
  {
    pcVar3->info_slot = x_value;
  }
  uStack_f0 = mode & 0x40000;
  if (uStack_f0 != 0)
  {
    pcVar7 = get_card_instance(granted_to_player, granted_to_card);
    pcVar7->state = pcVar7->state | 0x100000;
  }
  uVar11 = 0;
  uVar5 = mode & 0x80000;
  uVar6 = mode & 0x200;
  if (uVar6 == 0)
  {
  LAB_0269ca10:
    if ((mode & 4U) == 0)
    {
      if ((mode & 0x400000U) != 0)
      {
        iVar4 = 1 - granted_to_player;
        if (td == NULL)
        {
          default_target_definition(granted_to_player, granted_to_card, &storage.target, 0);
          td = &storage.target;
          storage.target.zone = 0x1000;
          storage.target.allowed_controller = iVar4;
        }
      LAB_0269caf1:
        iVar8 = validate_arbitrary_target(td, iVar4, -1);
        if (iVar8 == 0)
          goto LAB_0269d378;
        pcVar3->targets[0].card = -1;
        pcVar3->targets[0].player = iVar4;
        pcVar3->number_of_targets = '\x01';
      }
    LAB_0269cb2a:
      if ((mode & 0x400U) != 0)
      {
        if (card_on_stack_controller == -1)
          goto LAB_0269d378;
        pcVar3->targets[0].player = card_on_stack_controller;
        pcVar3->targets[0].card = card_on_stack;
        pcVar3->number_of_targets = '\x01';
      }
      if ((mode & 1U) != 0)
      {
        add_state(granted_to_player, granted_to_card, 0x10);
      }
      if (uVar5 != 0)
      {
        remove_state(granted_to_player, granted_to_card, 0x10);
      }
      if ('\0' < (char)variable_costs)
      {
        lose_life(granted_to_player, (int)(char)variable_costs);
      }
      if ((mode & 8U) != 0)
      {
        kill_card(granted_to_player, granted_to_card, KILL_SACRIFICE);
      }
      if (((mode & 0x20U) == 0) ||
          (discard(granted_to_player, 4, granting_player), spell_fizzled != 1))
      {
        if ((mode & 0x20000000U) != 0)
        {
          discard(granted_to_player, 1, granting_player);
        }
        if ((mode & 0x40U) != 0)
        {
          remove_counters(granted_to_player, granted_to_card, granted_to_player,
                                      granted_to_card, COUNTER_P1_P1, 1);
        }
        if ((mode & 0x80U) != 0)
        {
          remove_counters(granted_to_player, granted_to_card, granted_to_player,
                                      granted_to_card, COUNTER_M1_M1, 1);
        }
        if (cVar2 == '\x01')
        {
          remove_counters(granted_to_player, granted_to_card, granted_to_player,
                                      granted_to_card, variable_costs >> 0x10 & COUNTER_invalid,
                                      variable_costs >> 8 & 0xff);
        }
        else if (cVar2 == '\x02')
        {
          remove_player_counters(granted_to_player, variable_costs >> 0x10 & 0xff,
                                 variable_costs >> 8 & 0xff);
        }
        if ((mode & 0x2000U) != 0)
        {
          uVar12 = pcVar3->targets[2].player;
          uVar9 = 0;
          if (0 < (int)uVar12)
          {
            uVar9 = uVar12;
          }
          pcVar3->targets[2].player = uVar9 | 1 << ((unsigned char)granted_to_player & 0x1f);
        }
        if ((mode & 0x4000U) != 0)
        {
          kill_card(granted_to_player, granted_to_card, KILL_EXILE);
        }
        if ((mode & 0x20000U) != 0)
        {
          bounce_permanent(granted_to_player, granted_to_card);
        }
        if ((mode < 0) && (granted_to_player == 1))
        {
          set_special_flags(1, granted_to_card, 4);
        }
        if (uVar6 != 0)
        {
          kill_card((int)uVar11 >> 0x10 & 0xff, uVar11 >> 0x18, KILL_SACRIFICE);
        }
        if (uStack_f0 != 0)
        {
          pcVar3 = get_card_instance(granted_to_player, granted_to_card);
          if ((pcVar3->state & 0x100000) != 0)
          {
            pcVar3->state = pcVar3->state & 0xffefffff;
          }
        }
        if (uVar5 != 0)
        {
          untap_card(granted_to_player, granted_to_card);
        }
        hack_what_was_sacced_for_gaa = uVar11;
        return 1;
      }
    }
    else
    {
      if ((mode & 2U) == 0)
      {
        load_text(NULL, prompt);
        prompt = text_lines[0];
      }
      pcVar7 = get_card_instance(td->player, td->card);
      uVar1 = pcVar7->number_of_targets;
      iVar4 = select_target(td->player, td->card, td, prompt, &storage.selected);
      pcVar7->number_of_targets = uVar1;

      if ((((iVar4 != 0) &&
            (((((mode & 0x800000U) == 0 || (granted_to_player != 1)) ||
               (iVar4 = can_regenerate(storage.selected.player, storage.selected.card),
                iVar4 != 0)) &&
              (((mode & 0x2000000U) == 0 ||
                (pcVar10 = get_card_instance(storage.selected.player, storage.selected.card),
                 pcVar10->damage_target_card == -1)))))) &&
           (((mode & 0x4000000U) == 0 ||
             (pcVar10 = get_card_instance(storage.selected.player, storage.selected.card),
              pcVar10->damage_target_card != -1)))) &&
          (((mode & 0x8000000U) == 0 ||
            ((pcVar10 = get_card_instance(storage.selected.player, storage.selected.card),
              pcVar10->damage_target_card == -1 &&
                  (pcVar10->damage_target_player == granted_to_player))))))
      {
        if (((mode & 0x100U) != 0) && (((pcVar10 = is_equipping(granted_to_player, granted_to_card),
                                         pcVar10 != NULL && (pcVar7->targets[0].player ==
                                                             (int)pcVar3->damage_target_player)) &&
                                        (pcVar7->targets[0].card == pcVar3->damage_target_card))))
        {
          ai_modifier = ai_modifier + -0x60;
        }
        pcVar3->targets[0].card = storage.selected.card;
        pcVar3->targets[0].player = storage.selected.player;
        pcVar3->number_of_targets = '\x01';
        if ((mode & 0x400000U) != 0)
        {
          iVar4 = 1 - granted_to_player;
          goto LAB_0269caf1;
        }
        goto LAB_0269cb2a;
      }
    }
  LAB_0269d378:
    if (uStack_f0 != 0)
    {
      pcVar3 = get_card_instance(granted_to_player, granted_to_card);
      uVar12 = pcVar3->state;
      if ((uVar12 & 0x100000) != 0)
      {
      LAB_0269d761:
        pcVar3->state = uVar12 & 0xffefffff;
      }
    }
    if (uVar6 == 0)
      goto LAB_0269d394;
  LAB_0269d83d:
    uVar12 = uVar11 >> 0x18;
    uStack_f0 = (int)uVar11 >> 0x10 & 0xff;
  }
  else
  {
    new_default_test_definition(&storage.test, 2, "Select a creature to sacrifice.");
    uVar11 = new_sacrifice(granted_to_player, granted_to_card, granted_to_player,
                                SAC_RETURN_CHOICE | SAC_AS_COST | SAC_JUST_MARK,
                                &storage.test);
    if (uVar11 != 0)
      goto LAB_0269ca10;
    spell_fizzled = 1;
    uVar12 = 0;
    if (uStack_f0 != 0)
    {
      pcVar3 = get_card_instance(granted_to_player, granted_to_card);
      uVar12 = pcVar3->state;
      if ((uVar12 & 0x100000) != 0)
        goto LAB_0269d761;
      goto LAB_0269d83d;
    }
  }
  pcVar3 = get_card_instance(uStack_f0, uVar12);
  if ((pcVar3->state & 0x100000) != 0)
  {
    pcVar3->state = pcVar3->state & 0xffefffff;
  }
LAB_0269d394:
  if ((mode & 1U) != 0)
  {
    remove_state(granted_to_player, granted_to_card, 0x10);
  }
  if (uVar5 != 0)
  {
    add_state(granted_to_player, granted_to_card, 0x10);
  }
  spell_fizzled = 1;
  hack_what_was_sacced_for_gaa = 0;
  return 0;
}


/* ManalinkEh 2022: 0x269db00; original functions/functions.c:13271. */
int generic_activated_ability(int player, int card, event_t event, int mode, int cless,
                                   int black, int blue, int green, int red, int white,
                                   uint32_t variable_costs, target_definition_t *td,
                                   const char *prompt)
{
  int result;
  result =
      granted_generic_activated_ability(player, card, player, card, event, mode, cless, black,
                                             blue, green, red, white, variable_costs, td, prompt);
  return result;
}


/* ManalinkEh 2022: 0x269e690; original functions/functions.c:13279. */
int attachment_granting_activated_ability(int player, int card, event_t event, int mode,
                                               int cless, int black, int blue, int green, int red,
                                               int white, uint32_t variable_costs,
                                               target_definition_t *td, const char *prompt)
{
  int32_t iVar1;
  card_instance_t *pcVar2;
  int iVar3;
  card_instance_t *pcVar4;
  int32_t player_00;
  if ((event == EVENT_CAN_ACTIVATE) || (event == EVENT_ACTIVATE))
  {
    pcVar2 = in_play(player, card);
    if (pcVar2 == NULL)
    {
      return 0;
    }
    if (event == EVENT_CAN_ACTIVATE)
    {
      if (pcVar2->damage_target_card < 0)
      {
        return 0;
      }
      if (pcVar2->damage_target_player < '\0')
      {
        return 0;
      }
      pcVar4 = in_play((int)pcVar2->damage_target_player, pcVar2->damage_target_card);
      if (pcVar4 == NULL)
      {
        return 0;
      }
    }
    player_00 = (int32_t)pcVar2->damage_target_player;
    iVar1 = pcVar2->damage_target_card;
    pcVar2->targets[9].player = player_00;
    pcVar2->targets[9].card = iVar1;
  }
  else
  {
    if ((event != EVENT_RESOLVE_ACTIVATION) && (event != EVENT_CLEANUP))
    {
      return 0;
    }
    pcVar2 = in_play(player, card);
    if (pcVar2 == NULL)
    {
      return 0;
    }
    player_00 = pcVar2->targets[9].player;
  }
  if ((-1 < player_00) && (iVar3 = pcVar2->targets[9].card, -1 < iVar3))
  {
    if (td != NULL)
    {
      td->player = player_00;
      td->card = iVar3;
      if ((td->illegal_abilities & 0x20000) != 0)
      {
        iVar3 = get_protections_from(player_00, iVar3);
        td->illegal_abilities = iVar3;
        iVar3 = pcVar2->targets[9].card;
        player_00 = pcVar2->targets[9].player;
      }
    }
    iVar3 = granted_generic_activated_ability(player, card, player_00, iVar3, event, mode,
                                                   cless, black, blue, green, red, white,
                                                   variable_costs, td, prompt);
    return iVar3;
  }
  return 0;
}


/* ManalinkEh 2022: 0x269e810; original functions/functions.c:13321. */
int equipment_granting_activated_ability(int player, int card, event_t event,
                                              const char *dialog_option, int equip_cost, int mode,
                                              int cless, int black, int blue, int green, int red,
                                              int white, uint32_t variable_costs,
                                              target_definition_t *td, const char *prompt)
{
  card_instance_t *pcVar1;
  card_instance_t *pcVar2;
  int iVar3;
  int iVar4;
  if (((event != EVENT_ACTIVATE) && (1U < (uint32_t)event - (uint32_t)EVENT_RESOLVE_ACTIVATION)) &&
      (event != EVENT_CLEANUP))
  {
    return 0;
  }
  pcVar1 = get_card_instance(player, card);
  if (event == EVENT_RESOLVE_ACTIVATION)
  {
    iVar3 = can_activate_basic_equipment(player, card, EVENT_RESOLVE_ACTIVATION, equip_cost);
    iVar4 = attachment_granting_activated_ability(player, card, EVENT_CAN_ACTIVATE, mode,
                                                       cless, black, blue, green, red, white,
                                                       variable_costs, td, prompt);
    iVar3 = dialog_impl(player, card, EVENT_RESOLVE_ACTIVATION, dialog_option, iVar4, 2, "Equip",
                        iVar3, 1, 0, 0, 0, 0);
  }
  else
  {
    pcVar2 = is_equipping(player, card);
    if (pcVar2 == NULL)
    {
      if (event == EVENT_ACTIVATE)
      {
        pcVar1->info_slot = 2;
      }
      iVar3 = basic_equipment(player, card, event, equip_cost);
      return iVar3;
    }
    iVar3 = can_activate_basic_equipment(player, card, event, equip_cost);
    iVar4 = attachment_granting_activated_ability(player, card, EVENT_CAN_ACTIVATE, mode,
                                                       cless, black, blue, green, red, white,
                                                       variable_costs, td, prompt);
    iVar3 =
        dialog_impl(player, card, event, dialog_option, iVar4, 2, "Equip", iVar3, 1, 0, 0, 0, 0);
    if (event == EVENT_CAN_ACTIVATE)
    {
      return iVar3;
    }
    if (event == EVENT_ACTIVATE)
    {
      if (iVar3 == 1)
      {
        attachment_granting_activated_ability(player, card, EVENT_ACTIVATE, mode, cless, black,
                                                   blue, green, red, white, variable_costs, td,
                                                   prompt);
        return 0;
      }
      if (iVar3 != 2)
      {
        return 0;
      }
      activate_basic_equipment(player, card, equip_cost);
      return 0;
    }
  }
  if (iVar3 == 1)
  {
    attachment_granting_activated_ability(player, card, event, mode, cless, black, blue, green,
                                               red, white, variable_costs, td, prompt);
    return 1;
  }
  if (iVar3 != 2)
  {
    return 0;
  }
  resolve_activation_basic_equipment(player, card);
  return 0;
}


/* ManalinkEh 2022: 0x26901c0; original functions/functions.c:13251. */
int has_mana_for_activated_ability(int player, int card, int cless, int black, int blue,
                                        int green, int red, int white)
{
  card_instance_t *instance;
  int iVar2;
  unsigned int colorless;
  unsigned int uVar3;
  int iVar4;
  unsigned int uVar5;

  iVar4 = white;
  instance = get_card_instance(player, card);
  if ((instance->ext->special_flags2 & SF2_MYCOSYNTH_LATTICE) != 0)
  {
    iVar2 = blue + black + green + red;
    white = 0;
    red = 0;
    cless = cless + iVar2 + iVar4;
    green = 0;
    blue = 0;
    black = 0;
  }
  colorless = get_cost_mod_for_activated_abilities(player, card, cless, black, blue, green,
                                                        red, white);
  instance = get_card_instance(player, card);
  if ((instance->ext->special_flags2 & SF2_CELESTIAL_DAWN) == 0)
  {
    uVar5 = black | blue | green | red | colorless;
  }
  else
  {
    iVar4 = black + blue + green + red;
    red = 0;
    white = white + iVar4;
    green = 0;
    blue = 0;
    black = 0;
    uVar5 = colorless;
  }
  uVar3 = 1;
  if (uVar5 != 0 || white != 0)
  {
    iVar4 = has_mana_multi(player, colorless, black, blue, green, red, white);
    uVar3 = (unsigned int)(iVar4 != 0);
  }
  return uVar3;
}


/* ManalinkEh 2022: 0x268e850; original functions/functions.c:13220. */
int charge_mana_for_activated_ability(int player, int card, int cless, int black, int blue,
                                           int green, int red, int white)
{
  card_instance_t *instance;
  int iVar2;
  int iVar3;

  iVar3 = white;
  instance = get_card_instance(player, card);
  if ((instance->ext->special_flags2 & SF2_MYCOSYNTH_LATTICE) != 0)
  {
    iVar2 = black + blue + green + red;
    white = 0;
    red = 0;
    cless = cless + iVar2 + iVar3;
    green = 0;
    blue = 0;
    black = 0;
  }
  iVar3 = blue;
  iVar2 = get_cost_mod_for_activated_abilities(player, card, cless, black, blue, green, red,
                                                    white);
  instance = get_card_instance(player, card);
  if ((instance->ext->special_flags2 & SF2_CELESTIAL_DAWN) != 0)
  {
    blue = 0;
    iVar3 = black + iVar3 + green;
    green = 0;
    white = white + iVar3 + red;
    red = 0;
    black = 0;
  }
  if ((cless == -1) && (iVar2 != -1))
  {
    charge_mana(player, COLOR_COLORLESS, iVar2);
    iVar3 = 0;
    if (spell_fizzled != 1)
    {
      pay_mana_xbugrwaU[0] = -1;
      iVar2 = pay_mana_xbugrwaU[0];
    LAB_0268e98e:
      pay_mana_xbugrwaU[0] = iVar2;
      pay_mana_xbugrwaU[2] = blue;
      pay_mana_xbugrwaU[3] = green;
      pay_mana_xbugrwaU[5] = white;
      pay_mana_xbugrwaU[1] = black;
      pay_mana_xbugrwaU[4] = red;
      charge_mana(player, COLOR_COLORLESS, 0);
      return (unsigned int)(spell_fizzled != 1);
    }
  }
  else
  {
    iVar3 = 1;
    if (((((blue != 0 || black != 0) || iVar2 != 0) || green != 0) || red != 0) || white != 0)
      goto LAB_0268e98e;
  }
  return iVar3;
}


/* ManalinkEh 2022: 0x268e9e0; original functions/functions.c:2604. */
int charge_mana_for_activated_ability_while_resolving(int player, int card, event_t event,
                                                           int colorless, int black, int blue,
                                                           int green, int red, int white)
{
  int result;

  if (event == EVENT_UPKEEP_TRIGGER_ABILITY)
  {
    event = EVENT_RESOLVE_TRIGGER;
  }
  put_card_or_activation_onto_stack(player, card, event, 0, 0);
  spell_fizzled = 0;
  ldoubleclicked = 0;
  result = charge_mana_for_activated_ability(player, card, colorless, black, blue, green, red,
                                                  white);
  obliterate_top_card_of_stack();
  return result;
}


/* ManalinkEh 2022: 0x269e2b0; original functions/functions.c:11835. */
int vanilla_creature_pumper(int player, int card, event_t event, int req_cless, int req_black,
                                 int req_blue, int req_green, int req_red, int req_white, int flags,
                                 int pow, int tou, int key, int s_key, target_definition_t *td)
{
  int result;
  card_instance_t *instance;
  target_definition_t td_default;
  if (event == EVENT_ACTIVATE)
  {
    if (td != NULL)
      goto LAB_0269e2f8;
  LAB_0269e36c:
    td = &td_default;
    default_target_definition(player, card, td, 2);
    td_default.preferred_controller = player;
  }
  else
  {
    if ((1U < (uint32_t)event - (uint32_t)EVENT_RESOLVE_ACTIVATION) && (event != EVENT_CLEANUP))
    {
      return 0;
    }
    if (td == NULL)
      goto LAB_0269e36c;
  }
  if ((event == EVENT_RESOLVE_ACTIVATION) && (result = valid_target(td), result != 0))
  {
    instance = get_card_instance(player, card);
    pump_ability_until_eot(player, card, instance->targets[0].player,
                                instance->targets[0].card, pow, tou, key, s_key);
  }
LAB_0269e2f8:
  result = granted_generic_activated_ability(player, card, player, card, event, flags | 4,
                                                  req_cless, req_black, req_blue, req_green,
                                                  req_red, req_white, 0, td, "TARGET_CREATURE");
  return result;
}


/* ManalinkEh 2022: 0x2690310; original functions/functions.c:9438. */
int generic_shade_amt_can_pump(int player, int card, int pump, int life_to_pay, int cless,
                                    int black, int blue, int green, int red, int white, int limit)
{
  char cVar1;
  int iVar2;
  int iVar3;
  int iVar4;
  int iVar5;
  int iVar6;

  if (pump < 1)
  {
    return 0;
  }
  iVar2 = can_use_activated_abilities(player, card);
  if (iVar2 != 0)
  {
    iVar2 = get_cost_mod_for_activated_abilities(player, card, cless, black, blue, green, red,
                                                      white);
    cVar1 = (iVar2 != 0) + (black != 0) + (blue != 0) + (green != 0) + (red != 0) + (white != 0);
    if (cVar1 == '\0')
    {
      iVar5 = 99;
    }
    else if (cVar1 == '\x01')
    {
      if (iVar2 == 0)
      {
        if (black == 0)
        {
          if (blue == 0)
          {
            if (green == 0)
            {
              if (red == 0)
              {
                iVar5 = has_mana(player, 5, 1);
                iVar5 = iVar5 / white;
              }
              else
              {
                iVar5 = has_mana(player, 4, 1);
                iVar5 = iVar5 / red;
              }
            }
            else
            {
              iVar5 = has_mana(player, 3, 1);
              iVar5 = iVar5 / green;
            }
          }
          else
          {
            iVar5 = has_mana(player, 2, 1);
            iVar5 = iVar5 / blue;
          }
        }
        else
        {
          iVar5 = has_mana(player, 1, 1);
          iVar5 = iVar5 / black;
        }
      }
      else
      {
        iVar5 = has_mana(player, 7, 1);
        iVar5 = iVar5 / iVar2;
      }
    }
    else
    {
      iVar3 = has_mana(player, 7, 1);
      iVar3 = iVar3 / (black + iVar2 + blue + green + red + white);
      if (black != 0)
      {
        iVar5 = has_mana(player, 1, 1);
        if (iVar5 / black < iVar3)
        {
          iVar3 = iVar5 / black;
        }
      }
      if (blue != 0)
      {
        iVar5 = has_mana(player, 2, 1);
        if (iVar5 / blue < iVar3)
        {
          iVar3 = iVar5 / blue;
        }
      }
      if (green != 0)
      {
        iVar5 = has_mana(player, 3, 1);
        if (iVar5 / green < iVar3)
        {
          iVar3 = iVar5 / green;
        }
      }
      if (red != 0)
      {
        iVar5 = has_mana(player, 4, 1);
        if (iVar5 / red < iVar3)
        {
          iVar3 = iVar5 / red;
        }
      }
      if (white != 0)
      {
        iVar5 = has_mana(player, 5, 1);
        if (iVar5 / white < iVar3)
        {
          iVar3 = iVar5 / white;
        }
      }
      if (iVar3 < 1)
      {
        return 0;
      }
      iVar6 = 0;
      do
      {
        iVar5 = (iVar6 + 1 + iVar3) / 2;
        iVar4 = has_mana_multi(player, iVar2 * iVar5, black * iVar5, blue * iVar5, green * iVar5,
                               red * iVar5, white * iVar5);
        if (iVar4 == 0)
        {
          iVar3 = iVar5 + -1;
          iVar5 = iVar6;
        }
        iVar6 = iVar5;
      } while (iVar5 != iVar3);
      if (iVar5 < 1)
      {
        return 0;
      }
      iVar5 = pump * iVar5;
    }
    if (0 < life_to_pay)
    {
      iVar2 = can_pay_life_impl(player, 1, 0);
      if ((iVar2 == 0) || (iVar2 = (life[player] + -1) / life_to_pay, iVar2 < 1))
      {
        return 0;
      }
      if (iVar2 < iVar5)
      {
        iVar5 = iVar2;
      }
    }
    if ((-1 < limit) && (limit < iVar5))
    {
      iVar5 = limit;
    }
    return pump * iVar5;
  }
  return 0;
}


/* ManalinkEh 2022: 0x26888a0; original functions/functions.c:9038. */
int get_cost_mod_for_activated_abilities(int player, int card, int orig_cost, int black,
                                              int blue, int green, int red, int white)
{
  int iVar1;
  card_instance_t *instance;
  unsigned int uVar3;
  int iVar4;

  instance = get_card_instance(player, card);
  iVar1 = instance->targets[0x11].card;
  uVar3 = (int)(orig_cost ^ 0xffffffffU) >> 0x1f & orig_cost;
  if (0x800 < iVar1)
  {
    iVar4 = 0x14;
    do
    {
      if ((iVar1 >> ((unsigned char)iVar4 & 0x1f) & 1U) != 0)
      {
        uVar3 = uVar3 + 2;
      }
      iVar4 = iVar4 + 1;
    } while (iVar4 != 0x18);
    do
    {
      if ((iVar1 >> ((unsigned char)iVar4 & 0x1f) & 1U) != 0)
      {
        uVar3 = uVar3 + 3;
      }
      iVar4 = iVar4 + 1;
    } while (iVar4 != 0x1c);
    do
    {
      uVar3 = (uVar3 + 1) - (unsigned int)((iVar1 >> ((unsigned char)iVar4 & 0x1f) & 1U) == 0);
      iVar4 = iVar4 + 1;
    } while (iVar4 != 0x20);
    if (0 < (int)uVar3)
    {
      iVar4 = 0xc;
      do
      {
        uVar3 = (uVar3 - 1) + (unsigned int)((iVar1 >> ((unsigned char)iVar4 & 0x1f) & 1U) == 0);
        iVar4 = iVar4 + 1;
      } while (iVar4 != 0x10);
      do
      {
        if ((iVar1 >> ((unsigned char)iVar4 & 0x1f) & 1U) != 0)
        {
          uVar3 = uVar3 - 2;
        }
        iVar4 = iVar4 + 1;
      } while (iVar4 != 0x14);
      uVar3 = uVar3 & (int)(uVar3 ^ 0xffffffff) >> 0x1f;
      if ((((((white == 0 && red == 0) && green == 0) && blue == 0) && black == 0) && uVar3 == 0) &&
          (0 < orig_cost))
      {
        return 1;
      }
    }
  }
  if ((uVar3 == 0) && (orig_cost < 0))
  {
    return orig_cost;
  }
  return uVar3;
}

#endif

#ifndef SHANDALAR
/* Recovered October 2022 implementations; see docs/recovery-progress.md. */
int charge_mana_from_id_with_cost_mod(int player, int card, event_t event, int csvid, int cost_mod);

/* ManalinkEh 2022: 0x268b720; original functions/functions.c:11106. */
int charge_mana_from_id_with_cost_mod(int player, int card, event_t event, int csvid, int cost_mod)
{
  card_ptr_t *cp;
  unsigned int uVar1;
  bool bVar2;
  card_instance_t *instance;
  int iVar4;
  int iVar5;
  int number;
  unsigned int uVar6;
  unsigned int clred_artifact;
  unsigned int uStack_2c;
  int iStack_1c;
  unsigned int uStack_18;
  int iStack_14;

  if (csvid < 0)
  {
    csvid = get_id(player, card);
  }
  if (card == -1)
  {
    iStack_14 = get_internal_card_id_from_csv_id(csvid);
    if (csvid == 0x25ad)
      goto LAB_0268b987;
  }
  else
  {
    instance = get_card_instance(player, card);
    iStack_14 = instance->internal_card_id;
    if ((unsigned int)cards_data[iStack_14].id != csvid)
    {
      backtrace_and_abort("functions/functions.c:11121\ncards_data[iid].id == csvid");
    }
    if (csvid == 0x25ad)
    {

      if (-1 < card)
      {
        iVar4 = casting_reaper_king(player, card, event);
        return iVar4;
      }
    LAB_0268b987:
      iStack_1c = add_card_to_hand(player, iStack_14);
      iVar4 = casting_reaper_king(player, iStack_1c, event);
      goto LAB_0268b91e;
    }
  }
  cp = cards_ptr[csvid];
  uVar6 = (unsigned int)cp->req_colorless;
  if (uVar6 == 0x28)
  {
    uVar6 = 0;
  }
  clred_artifact = (unsigned int)(cards_data[iStack_14].type & 0x40);
  iVar4 = is_phyrexian(csvid);
  if (iVar4 == 0)
  {

    if (card < 0)
    {

      uStack_18 = true_get_updated_casting_cost(player, -1, iStack_14, event, uVar6 - cost_mod);
      uStack_18 = uStack_18 & (int)(uStack_18 ^ 0xffffffff) >> 0x1f;
      iVar4 = is_normal_hybrid(cp);
      if (iVar4 == 0)
      {
        iVar4 = is_monocolor_hybrid(cp);
        if (iVar4 == 0)
          goto LAB_0268ba70;
        card = add_card_to_hand(player, iStack_14);
        instance = get_card_instance(player, card);
        instance->state = instance->state | 0x20;
        hand_count[player] = hand_count[player] + -1;
        iStack_1c = card;
        goto LAB_0268b9de;
      }
      card = add_card_to_hand(player, iStack_14);
      instance = get_card_instance(player, card);
      uVar6 = instance->state;
      hand_count[player] = hand_count[player] + -1;
      instance->state = uVar6 | 0x20;
      iStack_1c = card;
    }
    else
    {
      uStack_18 = true_get_updated_casting_cost(player, card, -1, event, uVar6 - cost_mod);
      uStack_18 = uStack_18 & (int)(uStack_18 ^ 0xffffffff) >> 0x1f;
      iVar4 = is_normal_hybrid(cp);
      iStack_1c = -1;
      if (iVar4 == 0)
      {
        iVar4 = is_monocolor_hybrid(cp);
        if (iVar4 == 0)
        {
        LAB_0268ba70:

          if (csvid == 0x2ded)
          {
            iVar4 = count_graveyard_by_type(player, 2);
            uStack_18 = uStack_18 - iVar4 & (int)(uStack_18 - iVar4 ^ 0xffffffff) >> 0x1f;
          }
          bVar2 = false;
          if (cards_data[iStack_14].cc[1] == 0xff)
          {
            if (uStack_18 == 0)
            {
              uStack_18 = 0xffffffff;
            }
            else
            {
              bVar2 = true;
            }
          }
          pay_mana_xbugrwaU[2] = (int)cp->req_blue;
          pay_mana_xbugrwaU[5] = (int)cp->req_white;
          pay_mana_xbugrwaU[1] = (int)cp->req_black;
          pay_mana_xbugrwaU[4] = (int)cp->req_red;
          pay_mana_xbugrwaU[3] = (int)cp->req_green;
          uStack_2c = 0;
          if (clred_artifact == 0)
          {
            uStack_2c = uStack_18;
            uStack_18 = 0;
          }
          pay_mana_xbugrwaU[0] = uStack_2c;
          pay_mana_xbugrwaU[6] = uStack_18;
          charge_mana(player, COLOR_COLORLESS, 0);
          if (!bVar2)
          {
            return (unsigned int)(spell_fizzled != 1);
          }
          if (spell_fizzled == 1)
          {
            return 0;
          }
          pay_mana_xbugrwaU[0] = -(unsigned int)(clred_artifact == 0);
          pay_mana_xbugrwaU[6] = -(unsigned int)(clred_artifact != 0);
          pay_mana_xbugrwaU[1] = 0;
          pay_mana_xbugrwaU[2] = 0;
          pay_mana_xbugrwaU[3] = 0;
          pay_mana_xbugrwaU[4] = 0;
          pay_mana_xbugrwaU[5] = 0;
          charge_mana(player, COLOR_COLORLESS, 0);
          return (unsigned int)(spell_fizzled != 1);
        }
      LAB_0268b9de:

        iVar4 = get_color_for_monocolored_hybrid(player, card);
        iVar5 = get_number_of_monocolored_hybrid_mana(player, card, 0);
        iVar4 = charge_mana_for_monocolor_hybrid(player, card, iVar5, iVar4, uStack_18);
        goto joined_r0x0268b918;
      }
    }

    iVar4 = get_colors_for_hybrid(player, card, 0, 0);
    iVar5 = get_colors_for_hybrid(player, card, iVar4, 0);
    number = get_number_of_hybrid_mana(player, card, 0, 0);
    iVar4 = charge_mana_hybrid(player, card, number, iVar4, iVar5, uStack_18);
  }
  else
  {
    iStack_1c = -1;
    if (card < 0)
    {
      card = add_card_to_hand(player, iStack_14);
      instance = get_card_instance(player, card);
      uVar1 = instance->state;
      hand_count[player] = hand_count[player] + -1;
      instance->state = uVar1 | 0x20;
      iStack_1c = card;
    }
    iVar4 = charge_phyrexian_mana(player, card, event, uVar6, (unsigned int)cp->req_black,
                                  (unsigned int)cp->req_blue, (unsigned int)cp->req_green,
                                  (unsigned int)cp->req_red, (unsigned int)cp->req_white,
                                  clred_artifact);
  }
joined_r0x0268b918:
  if (iStack_1c < 0)
  {
    return iVar4;
  }
LAB_0268b91e:
  obliterate_card(player, iStack_1c);
  return iVar4;
}

#endif


#ifndef SHANDALAR
/* Recovered October 2022 specialized activation costs. */
typedef union
{
  char text[100];
  target_definition_t target;
} activation_grave_storage_t;

/* ManalinkEh 2022: 0x26a4540; original functions/functions.c:18743. */
int generic_activated_ability_with_sacrifice_as_cost(
    int player, int card, event_t event, generic_activated_ability_flags_t flags, int cless,
    int black, int blue, int green, int red, int white, int gvc_costs, target_definition_t *td,
    const char *prompt, type_t type, test_definition_t *test,
    sacrifice_and_report_value_flags_t sac_flags)
{
  unsigned short uVar1;
  int iVar2;
  card_instance_t *instance;
  uint32_t aVar4;
  int iVar5;
  unsigned int uVar6;
  int iVar7;
  unsigned int card_00;
  uint32_t p_Stack_264;
  char marked_for_sacrifice[151];
  test_definition_t test2;
  test_definition_t test2_1;
  if (event == EVENT_CLEANUP)
  {
    instance = get_card_instance(player, card);
    if (card != affected_card)
    {
      return 0;
    }
    if (player != affected_card_controller)
    {
      return 0;
    }
    if ((flags & GAA_ONCE_PER_TURN) == GAA_NONE)
    {
      return 0;
    }
    instance->targets[2].player = 0;
    return 0;
  }
  if (event == EVENT_CAN_ACTIVATE)
  {
    if (test == NULL)
    {
      if (type == TYPE_NONE)
      {
        return 0;
      }
      iVar2 = can_sacrifice_type_as_cost(player, 1, type);
    }
    else
    {
      iVar2 = new_can_sacrifice_as_cost(player, card, test);
      if ((type != TYPE_NONE) &&
          (iVar7 = can_sacrifice_type_as_cost(player, 1, type), iVar7 != 0))
        goto LAB_026a480e;
    }
    if (iVar2 == 0)
    {
      return 0;
    }
  LAB_026a480e:
    iVar2 = granted_generic_activated_ability(player, card, player, card, EVENT_CAN_ACTIVATE,
                                                   flags, cless, black, blue, green, red, white,
                                                   gvc_costs, td, prompt);
    return iVar2;
  }
  if (event != EVENT_ACTIVATE)
  {
    return 0;
  }
  granted_generic_activated_ability(player, card, player, card, EVENT_ACTIVATE, flags, cless,
                                         black, blue, green, red, white, gvc_costs, td, prompt);
  if (spell_fizzled == 1)
  {
    return 0;
  }
  if (type != TYPE_NONE)
  {
    if (type == TYPE_ANY)
    {
      test = &test2;
      new_default_test_definition(test, 0xff, "Select a card to sacrifice.");
    }
    else
    {
      if (ai_is_speculating == 1)
      {
        test2.message[0] = '\0';
      }
      else
      {
        type_text(test2.message, 100, type, TYPETEXT_0);
      }
      scnprintf(marked_for_sacrifice, 100, "Select a %s card to sacrifice.", test2.message);
      test = &test2_1;
      new_default_test_definition(test, type, marked_for_sacrifice);
    }
  }
  uVar1 = test->qty;
  if ((sac_flags & SARV_REPORT_COUNT) == SARV_0)
  {
    if (uVar1 < 2)
    {
      uVar6 = new_sacrifice(
          player, card, player,
          SAC_RETURN_CHOICE | SAC_AS_COST | SAC_ALL_OR_NONE | SAC_JUST_MARK, test);
      if (uVar6 != 0)
      {
        card_00 = uVar6 >> 0x18;
        uVar6 = (int)uVar6 >> 0x10 & 0xff;
        instance = get_card_instance(uVar6, card_00);
        if ((instance->state & 0x100000) != 0)
        {
          instance->state = instance->state & 0xffefffff;
        }
        if (spell_fizzled == 1)
        {
          return 0;
        }
        if (sac_flags != SARV_0)
        {
          instance = get_card_instance(uVar6, card_00);
          if ((sac_flags & SARV_REPORT_POWER) == SARV_0)
          {
            aVar4 = (uint32_t)instance->internal_card_id;
          }
          else
          {
            aVar4 = get_abilities(uVar6, card_00, EVENT_POWER, -1);
          }
          if ((sac_flags & SARV_REPORT_TOUGHNESS) != SARV_0)
          {
            aVar4 = get_abilities(uVar6, card_00, EVENT_TOUGHNESS, -1);
          }
          if ((sac_flags & SARV_REPORT_COLORS) != SARV_0)
          {
            aVar4 = get_color(uVar6, card_00);
          }
          if ((sac_flags & SARV_REPORT_CMC) != SARV_0)
          {
            aVar4 = get_cmc(uVar6, card_00);
          }
          instance = get_card_instance(player, card);
          instance->info_slot = aVar4;
        }
        kill_card(uVar6, card_00, KILL_SACRIFICE);
        return 0;
      }
      goto LAB_026a49d5;
    }
    iVar2 = mark_sacrifice(player, card, player,
                                SAC_RETURN_CHOICE | SAC_AS_COST | SAC_ALL_OR_NONE |
                                    SAC_JUST_MARK,
                                test, marked_for_sacrifice);
  }
  else
  {
    test->qty = 99;
    iVar2 = mark_sacrifice(player, card, player,
                                SAC_AS_COST | SAC_JUST_MARK | SAC_DONE, test,
                                marked_for_sacrifice);
    test->qty = uVar1;
  }
  if (iVar2 != 0)
  {
    if (spell_fizzled == 1)
    {
      return 0;
    }
    iVar7 = 0;
    if (0 < active_cards_count[player])
    {
      do
      {
        if (marked_for_sacrifice[iVar7] != '\0')
        {
          instance = get_card_instance(player, iVar7);
          instance->state = instance->state & 0xffefffff;
          if ((spell_fizzled != 1) && (instance = in_play(player, iVar7), instance != NULL))
          {
            if (sac_flags != SARV_0)
            {
              p_Stack_264 = (uint32_t)0x0;
              if ((sac_flags & (SARV_REPORT_CMC | SARV_REPORT_COLORS | SARV_REPORT_TOUGHNESS |
                                SARV_REPORT_POWER)) == SARV_0)
              {
                instance = get_card_instance(player, iVar7);
                p_Stack_264 = (uint32_t)instance->internal_card_id;
              }
              if ((sac_flags & SARV_REPORT_POWER) != SARV_0)
              {
                iVar5 = get_abilities(player, iVar7, EVENT_POWER, -1);
                p_Stack_264 = p_Stack_264 + iVar5;
              }
              if ((sac_flags & SARV_REPORT_TOUGHNESS) != SARV_0)
              {
                iVar5 = get_abilities(player, iVar7, EVENT_TOUGHNESS, -1);
                p_Stack_264 = p_Stack_264 + iVar5;
              }
              if ((sac_flags & SARV_REPORT_COLORS) != SARV_0)
              {
                uVar6 = get_color(player, iVar7);
                p_Stack_264 = (uint32_t)((unsigned int)p_Stack_264 | uVar6);
              }
              if ((sac_flags & SARV_REPORT_CMC) != SARV_0)
              {
                iVar5 = get_cmc(player, iVar7);
                p_Stack_264 = p_Stack_264 + iVar5;
              }
              instance = get_card_instance(player, card);
              instance->info_slot = p_Stack_264;
            }
            kill_card(player, iVar7, KILL_SACRIFICE);
          }
        }
        iVar7 = iVar7 + 1;
      } while (iVar7 < active_cards_count[player]);
    }
    if ((sac_flags & SARV_REPORT_COUNT) == SARV_0)
    {
      return 0;
    }
    instance = get_card_instance(player, card);
    instance->info_slot = iVar2;
    return 0;
  }
LAB_026a49d5:
  spell_fizzled = 1;
  if ((flags & GAA_UNTAPPED) != GAA_NONE)
  {
    remove_state(player, card, 0x10);
  }
  return 0;
}


/* ManalinkEh 2022: 0x26a4c00; original functions/functions.c:18868. */
int generic_activated_ability_with_target_in_grave(
    int player, int card, event_t event, generic_activated_ability_flags_t flags, int cless,
    int black, int blue, int green, int red, int white, int gvc_costs, target_definition_t *td,
    const char *prompt, type_t type, test_definition_t *test, int gs_flags_for_grave_recycler)
{
  unsigned int uVar1;
  unsigned int uVar2;
  unsigned int uVar3;
  card_instance_t *instance;
  int iVar5;
  int32_t iVar6;
  int iVar7;
  bool bVar8;
  int iStack_228;
  activation_grave_storage_t scratch;
  test_definition_t test2;
  test_definition_t test2_1;

  iStack_228 = player;
  if ((gs_flags_for_grave_recycler & 0x3808U) == 0)
  {
    backtrace_and_abort(
        "functions/functions.c:18876\n0 && \"unsupported \'gs_flags_for_grave_recycler\'\"");
  }
  if ((flags & (GAA_CAN_ONLY_TARGET_OPPONENT | GAA_CAN_TARGET)) != GAA_NONE)
  {
    backtrace_and_abort(
        "functions/functions.c:18879\n0 && \"unsupported \'generic_activated_ability_flags_t\'\"");
  }
  if (event == EVENT_CAN_ACTIVATE)
  {
    if ((gs_flags_for_grave_recycler & 0x2000U) == 0)
    {
      iVar7 = 0;
      if (gs_flags_for_grave_recycler == 0x800)
      {
        iStack_228 = 1 - player;
        bVar8 = iStack_228 == 2;
      }
      else if (gs_flags_for_grave_recycler == 0x1000)
      {
        iStack_228 = 2;
        bVar8 = true;
      }
      else
      {
        bVar8 = player == 2;
      }
      do
      {
        if (((iStack_228 == iVar7) || (bVar8)) && (iVar5 = graveyard_has_shroud(iVar7), iVar5 == 0))
        {
          if (type == TYPE_NONE)
          {
            if (test == NULL)
              goto LAB_026a4eb4;
          }
          else
          {
            iVar5 = count_graveyard_by_type(player, type);
            if ((int)(unsigned int)test->qty <= iVar5)
              goto LAB_026a4fbb;
          }
          iVar5 = new_special_count_grave(iVar7, test);
          if ((int)(unsigned int)test->qty <= iVar5)
            goto LAB_026a4fbb;
        }
      LAB_026a4eb4:
        iVar7 = iVar7 + 1;
        if (iVar7 == 2)
        {
          return 0;
        }
      } while (true);
    }
    iStack_228 = 0;
    iVar7 = 0;
    do
    {
      iVar5 = graveyard_has_shroud(iVar7);
      if (iVar5 == 0)
      {
        if (type != TYPE_NONE)
        {
          iVar5 = count_graveyard_by_type(player, type);
          iStack_228 = iStack_228 + iVar5;
        }
        if (test != NULL)
        {
          iVar5 = new_special_count_grave(iVar7, test);
          iStack_228 = iStack_228 + iVar5;
        }
        if ((int)(unsigned int)test->qty <= iStack_228)
        {
        LAB_026a4fbb:
          iVar7 = granted_generic_activated_ability(
              player, card, player, card, EVENT_CAN_ACTIVATE, flags, cless, black, blue, green, red,
              white, gvc_costs, td, prompt);
          return iVar7;
        }
      }
      iVar7 = iVar7 + 1;
      if (iVar7 == 2)
      {
        return 0;
      }
    } while (true);
  }
  if (event != EVENT_ACTIVATE)
  {
    return 0;
  }
  if (type != TYPE_NONE)
  {
    if (type == TYPE_ANY)
    {
      test = &test2;
      new_default_test_definition(test, 0xff, "Select target card.");
    }
    else
    {
      if (ai_is_speculating == 1)
      {
        test2.message[0] = '\0';
      }
      else
      {
        type_text(test2.message, 100, type, TYPETEXT_0);
      }
      scnprintf(scratch.text, 100, "Select target %s card.", test2.message);
      test = &test2_1;
      new_default_test_definition(test, type, scratch.text);
    }
  }
  if (test->qty == 1)
  {
    iVar7 = player;
    if (gs_flags_for_grave_recycler == 0x800)
    {
      iVar7 = 1 - player;
    }
    uVar1 = (uint32_t)(uint16_t)test->ai_selection_mode | ((uint32_t)(uint16_t)test->qty << 16);
    if ((gs_flags_for_grave_recycler == 0x1000) || (iVar7 == 2))
    {
      iVar7 = select_target_from_either_grave(player, card, SFG_0, uVar1 & 0xff,
                                                   uVar1 >> 8 & 0xff, test, 0, 1);
    }
    else
    {
      iVar7 =
          select_target_from_grave_source(player, card, iVar7, SFG_0, uVar1 & 0xffff, test, 0);
    }
    if (iVar7 == -1)
    {
      spell_fizzled = 1;
      return 0;
    }
    goto LAB_026a4d5b;
  }
  if ((gs_flags_for_grave_recycler & 0x2000U) != 0)
  {
    instance = get_card_instance(player, card);
    uVar2 = (uint32_t)(uint16_t)test->ai_selection_mode | ((uint32_t)(uint16_t)test->qty << 16);
    select_multiple_cards_from_either_graveyard(player, card, SFG_0, uVar2 & 0xff,
                                                     uVar2 >> 8 & 0xff, test,
                                                     (unsigned int)test->qty, instance->targets);
    goto LAB_026a4d5b;
  }
  instance = get_card_instance(player, card);
  iVar7 = player;
  if (gs_flags_for_grave_recycler == 0x800)
  {
    iVar6 = 1 - player;
    if (iVar6 == 2)
      goto LAB_026a50a4;
  LAB_026a5111:
    uVar3 = (uint32_t)(uint16_t)test->ai_selection_mode | ((uint32_t)(uint16_t)test->qty << 16);
    if (iVar6 != player)
    {
      uVar3 = uVar3 >> 8;
      iVar7 = iVar6;
    }
  }
  else
  {
    if ((player == 2) || (gs_flags_for_grave_recycler == 0x1000))
    {
    LAB_026a50a4:
      default_target_definition(player, card, &scratch.target, 0);
      scratch.target.zone = 0x1000;
      new_pick_target(&scratch.target, "Select whose graveyard.", 0, 0x81);
      if (spell_fizzled == 1)
      {
        return 0;
      }
      iVar6 = instance->targets[0].player;
      goto LAB_026a5111;
    }
    uVar3 = (uint32_t)(uint16_t)test->ai_selection_mode | ((uint32_t)(uint16_t)test->qty << 16);
  }
  select_multiple_cards_from_graveyard(player, iVar7, -1, uVar3 & 0xff, test,
                                            (unsigned int)test->qty, instance->targets + 1);
LAB_026a4d5b:
  granted_generic_activated_ability(player, card, player, card, EVENT_ACTIVATE, flags, cless,
                                         black, blue, green, red, white, gvc_costs, td, prompt);
  return 0;
}


/* ManalinkEh 2022: 0x26a66a0; original functions/functions.c:19344. */
int generic_activated_ability_with_tapsubtype_as_cost(
    int player, int card, event_t event, generic_activated_ability_flags_t flags, int cless,
    int black, int blue, int green, int red, int white, int gvc_costs, target_definition_t *td,
    const char *prompt, target_definition_t *td_subtype, int req_tap)
{
  if (event == EVENT_CAN_ACTIVATE)
  {
    if (target_available(player, card, td_subtype) < req_tap)
      return 0;
    return granted_generic_activated_ability(player, card, player, card, event, flags, cless,
                                                  black, blue, green, red, white, gvc_costs, td,
                                                  prompt);
  }
  if (event == EVENT_ACTIVATE)
  {
    card_instance_t *instance = get_card_instance(player, card);
    int to_tap[req_tap];
    for (int i = 0; i < req_tap; ++i)
    {
      char typetext[100];
      char clr[100];
      char buffer[500];
      const char *subtype_text = "";
      const char *subtype_space = "";
      const char *color_text = "";
      const char *color_space = "";
      if (ai_is_speculating == 1)
        typetext[0] = '\0';
      else
        type_text(typetext, 100, td_subtype->required_type, 0);
      if (td_subtype->required_subtype >= 0)
      {
        subtype_text = raw_get_subtype_text(td_subtype->required_subtype);
        subtype_space = " ";
      }
      if (td_subtype->required_color != 0)
      {
        color_text = get_sleighted_color_text(player, card, clr, td_subtype->required_color);
        color_space = " ";
      }
      scnprintf(buffer, 500, "Select an untapped %s%s%s%s%s to tap (%d of %d)", color_text,
                color_space, subtype_text, subtype_space, typetext, i + 1, req_tap);
      if (!new_pick_target(td_subtype, buffer, 9, 0x81))
        break;
      to_tap[i] = instance->targets[9].card;
      get_card_instance(player, to_tap[i])->state |= 0x100000;
    }
    for (int i = 0; i < instance->number_of_targets; ++i)
    {
      card_instance_t *target = get_card_instance(player, to_tap[i]);
      if (target->state & 0x100000)
        target->state &= ~0x100000U;
    }
    int good = spell_fizzled != 1;
    instance->number_of_targets = 0;
    if (good)
    {
      granted_generic_activated_ability(player, card, player, card, EVENT_ACTIVATE, flags,
                                             cless, black, blue, green, red, white, gvc_costs, td,
                                             prompt);
      if (spell_fizzled != 1)
        for (int i = 0; i < req_tap; ++i)
          tap_card(player, to_tap[i]);
      instance->number_of_targets = 0;
    }
  }
  return 0;
}

#endif


#ifndef SHANDALAR
/* Recovered October 2022 attacker/keyword helpers. */

/* ManalinkEh 2022: 0x268d340; original functions/functions.c:12407. */
void store_attackers(int player, int card, event_t event, declare_attackers_trigger_t mode,
                          int attacker_player, int attacker_card, test_definition_t *test)
{
  int iVar1;
  int32_t iVar2;
  card_instance_t *pcVar3;
  card_instance_t *pcVar4;
  int iVar5;
  int32_t count;
  int32_t *storage;
  if (event == EVENT_DECLARE_ATTACKERS)
  {
    iVar1 = is_humiliated(player, card, AL_TRIG);
    if ((iVar1 == 0) && (((current_turn == attacker_player || (attacker_player == 2)) &&
                          (number_of_attackers_declared != 0))))
    {
      pcVar4 = get_card_instance(player, card);
      if ((mode & DAT_STORE_IN_INFO_SLOT) == 0)
      {
        if ((mode & DAT_STORE_IN_TARGETS_3) == 0)
        {
          storage = &pcVar4->targets[1].player;
          iVar2 = pcVar4->targets[1].player;
        }
        else
        {
          storage = &pcVar4->targets[3].player;
          iVar2 = pcVar4->targets[3].player;
        }
      }
      else
      {
        storage = &pcVar4->info_slot;
        iVar2 = pcVar4->info_slot;
      }
      if (iVar2 < 0)
      {
        (*storage) = 0;
      }
      if ((mode & DAT_ATTACKS_ALONE) == 0)
      {
        if (((mode & DAT_ATTACKS_WITH_3_OR_MORE) != 0) && (number_of_attackers_declared < 3))
        {
          return;
        }
      }
      else
      {
        if (number_of_attackers_declared != 1)
        {
          return;
        }
        if ((mode & DAT_ATTACKS_WITH_3_OR_MORE) != 0)
        {
          return;
        }
      }
      if (test != NULL)
      {
        test->state = test->state | 4;
        iVar1 = check_battlefield_for_special_card(player, card, current_turn, 4, test);
        if (iVar1 < (int)(unsigned int)test->qty)
        {
          return;
        }
      }
      if ((((mode & DAT_ATTACKS_WITH_2_OR_MORE) == 0) || (1 < number_of_attackers_declared)) &&
          (((mode & DAT_BATTALION) == 0 ||
            ((2 < number_of_attackers_declared &&
              (pcVar3 = get_card_instance(attacker_player, attacker_card),
               (pcVar3->state & 4) != 0))))))
      {
        iVar1 = attacker_card;
        if (attacker_card < 0)
        {
          iVar1 = active_cards_count[current_turn] + -1;
          if (iVar1 < 0)
          {
            return;
          }
          attacker_card = 0;
        }
        do
        {
          pcVar3 = in_play(current_turn, attacker_card);

          if ((((pcVar3 != NULL) && ((pcVar3->state & 4) != 0)) &&
               ((((mode & DAT_ATTACKS_PLAYER) == 0 ||
                  (pcVar3 = get_card_instance(current_turn, attacker_card),
                   (pcVar3->ext->special_flags & SF_ATTACKING_PWALKER) == 0)) &&
                 (((mode & DAT_ATTACKS_PLANESWALKER) == 0 ||
                   (pcVar3 = get_card_instance(current_turn, attacker_card),
                    (pcVar3->ext->special_flags & SF_ATTACKING_PWALKER) != 0)))))) &&
              ((test == NULL ||
                (((test->not_me == 0 || ((test->not_me == 1 && ((card != attacker_card ||
                                                                 (current_turn != player)))))) &&
                  (iVar5 = new_make_test_in_play(current_turn, attacker_card, -1, test),
                   iVar5 != 0))))))
          {
            count = (*storage);
            if (((mode & DAT_TRACK) != 0) && ((int)count < 0x40))
            {
              ((unsigned char *)&pcVar4->targets[2].player)[count] = (unsigned char)attacker_card;
              count = (*storage);
            }
            (*storage) = count + 1;
          }
          attacker_card = attacker_card + 1;
        } while (attacker_card <= iVar1);
      }
    }
  }
  else if (event == EVENT_CLEANUP)
  {
    pcVar4 = get_card_instance(player, card);
    storage = &pcVar4->info_slot;
    if (((mode & DAT_STORE_IN_INFO_SLOT) == 0) &&
        (storage = &pcVar4->targets[1].player, (mode & DAT_STORE_IN_TARGETS_3) != 0))
    {
      storage = &pcVar4->targets[3].player;
    }
    (*storage) = 0;
    return;
  }
  return;
}


/* ManalinkEh 2022: 0x268d6a0; original functions/functions.c:12483. */
int resolve_declare_attackers_trigger(int player, int card, event_t event,
                                           declare_attackers_trigger_t mode)
{
  unsigned char bVar1;
  xtrigger_t xVar2;
  int iVar3;
  card_instance_t *instance;
  declare_attackers_trigger_t dVar5;
  int32_t iVar6;
  int32_t *storage;
  xVar2 = trigger_condition;
  if (trigger_condition == 0xe5)
  {
    xVar2 = xtrigger_impl_value_dont_use_directly;
  }
  if ((((xVar2 != XTRIGGER_ATTACKING) || (card != affected_card)) ||
       (player != affected_card_controller)) ||
      ((reason_for_trigger_controller != player ||
        (iVar3 = is_humiliated(player, card, AL_TRIG), iVar3 != 0))))
  {
    return 0;
  }
  instance = get_card_instance(player, card);
  if ((mode & DAT_STORE_IN_INFO_SLOT) == 0)
  {
    if ((mode & DAT_STORE_IN_TARGETS_3) == 0)
    {
      storage = &instance->targets[1].player;
      iVar6 = instance->targets[1].player;
    }
    else
    {
      storage = &instance->targets[3].player;
      iVar6 = instance->targets[3].player;
    }
  }
  else
  {
    storage = &instance->info_slot;
    iVar6 = instance->info_slot;
  }
  if (iVar6 < 1)
  {
    return 0;
  }
  iVar3 = 0;
  if (event != EVENT_TRIGGER)
  {
    if (event == EVENT_RESOLVE_TRIGGER)
    {
      iVar3 = (*storage);
      if ((mode & DAT_SEPARATE_TRIGGERS) != 0)
      {
        bVar1 = ((unsigned char *)&instance->targets[2].player)[iVar3];
        if (1 < iVar3)
        {
          instance->state = instance->state & 0xfffffeff;
        }
        (*storage) = (*storage) + -1;
        return bVar1 + 1;
      }
    }
    else if (event != EVENT_END_TRIGGER)
    {
      return 0;
    }
    (*storage) = 0;
    return iVar3;
  }
  dVar5 = mode & 0xf;
  if (dVar5 == DAT_RESOLVE_TRIGGER_DUH)
  {
    iVar3 = duh_mode(player);
    dVar5 = DAT_RESOLVE_TRIGGER_OPTIONAL;
    if (iVar3 == 0)
      goto LAB_0268d7a8;
  }
  else if (dVar5 != 0)
    goto LAB_0268d7a8;
  dVar5 = DAT_RESOLVE_TRIGGER_MANDATORY;
LAB_0268d7a8:
  event_result = event_result | dVar5;
  return 0;
}


/* ManalinkEh 2022: 0x268d7e0; original functions/functions.c:12534. */
int declare_attackers_trigger(int player, int card, event_t event,
                                   declare_attackers_trigger_t mode, int attacker_player,
                                   int attacker_card)
{
  int result;
  store_attackers(player, card, event, mode, attacker_player, attacker_card, NULL);
  result = resolve_declare_attackers_trigger(player, card, event, mode);
  return result;
}


/* ManalinkEh 2022: 0x268d940; original functions/functions.c:12541. */
int declare_attackers_trigger_test(int player, int card, event_t event,
                                        declare_attackers_trigger_t mode, int attacker_player,
                                        int attacker_card, test_definition_t *test)
{
  int result;
  store_attackers(player, card, event, mode, attacker_player, attacker_card, test);
  result = resolve_declare_attackers_trigger(player, card, event, mode);
  return result;
}


/* ManalinkEh 2022: 0x268d840; original functions/functions.c:9701. */
int when_attacks_pump_self(int player, int card, event_t event, int power, int toughness)
{
  int result;
  result = declare_attackers_trigger(player, card, event, 0, player, card);
  if (result != 0)
  {
    pump_until_eot_merge_previous_alt(player, card, player, card, power, toughness, 0);
  }
  if (event == EVENT_POW_BOOST)
  {
    if (current_turn != player)
    {
      return 0;
    }
    if (0x15 < current_phase)
    {
      return 0;
    }
    if (power < 1)
    {
      return 0;
    }
    result = is_humiliated(player, card, AL_TRIG);
  }
  else
  {
    if (event != EVENT_TOU_BOOST)
    {
      return 0;
    }
    if (current_turn != player)
    {
      return 0;
    }
    if (0x15 < current_phase)
    {
      return 0;
    }
    if (toughness < 1)
    {
      return 0;
    }
    result = is_humiliated(player, card, AL_TRIG);
    power = toughness;
  }
  if (result != 0)
  {
    return 0;
  }
  return power;
}

#endif


#ifndef SHANDALAR
/* Recovered October 2022 entry-trigger helpers. */

/* ManalinkEh 2022: 0x26a5370; original functions/functions.c:19061. */
void etb_ability(int player, int card, event_t event, resolve_trigger_t trigger_mode)
{
  unsigned char bVar1;
  int iVar2;
  int iVar3;
  if (trigger_condition == 0xdb)
  {
    if (((((card == affected_card) && (player == affected_card_controller)) &&
          (reason_for_trigger_controller == player)) &&
         ((trigger_cause_controller == player && (trigger_cause == card)))) &&
        ((iVar2 = is_humiliated(player, card, AL_TRIG),
          iVar2 == 0 && (iVar2 = check_for_cip_effects_removal(player, card), iVar2 == 0))))
    {
      if (event == EVENT_TRIGGER)
      {
        if (trigger_mode == RESOLVE_TRIGGER_DUH)
        {
          iVar2 = duh_mode(player);
          trigger_mode = (iVar2 != 0) + RESOLVE_TRIGGER_OPTIONAL;
        }
        event_result = event_result | trigger_mode;
      }
      else if (event == EVENT_RESOLVE_TRIGGER)
      {
        iVar3 = 1;
        iVar2 = is_what(player, card, 0x47);
        if (iVar2 != 0)
        {
          iVar3 = card_counts[player][4] + 1;
        }
        iVar2 = is_what(player, card, 0x42);
        if (iVar2 != 0)
        {
          iVar3 = iVar3 + (unsigned int)card_counts[player][2];
        }
        iVar2 = is_what(player, card, 1);
        if (iVar2 != 0)
        {
          iVar3 = iVar3 + (unsigned int)card_counts[player][1];
        }
        iVar2 = has_subtype(player, card, SUBTYPE_WIZARD);
        if (iVar2 != 0)
        {
          bVar1 = card_counts[player][3];
          iVar2 = count_harmonic_prodigy(player, card);
          iVar3 = iVar3 + (unsigned int)bVar1 + iVar2;
        }
        iVar2 = has_subtype(player, card, SUBTYPE_SHAMAN);
        if ((iVar2 != 0) && (iVar2 = has_subtype(player, card, SUBTYPE_WIZARD), iVar2 == 0))
        {
          iVar2 = count_harmonic_prodigy(player, card);
          iVar3 = iVar3 + iVar2;
        }
        for (; iVar3 != 0; iVar3 = iVar3 + -1)
        {
          call_card_function(player, card, EVENT_ETB);
        }
      }
    }
    return;
  }
  return;
}


/* ManalinkEh 2022: 0x26a5a60; original functions/functions.c:19100. */
void etb_ability_global(int controller_of_trigger, event_t event, int trigger_mode,
                             type_t type, test_definition_t *test, int triggering_player,
                             int triggering_card)
{
  unsigned char bVar1;
  int iVar2;
  card_instance_t *instance;
  int iVar4;
  if (trigger_condition != 0xdb)
  {
    return;
  }
  if (reason_for_trigger_controller != triggering_player)
  {
    return;
  }
  if (triggering_card != affected_card)
  {
    return;
  }
  if (triggering_player != affected_card_controller)
  {
    return;
  }
  iVar2 = is_humiliated(triggering_player, triggering_card, AL_TRIG);
  if (iVar2 != 0)
  {
    return;
  }
  if ((controller_of_trigger != 2) && (trigger_cause_controller != controller_of_trigger))
  {
    return;
  }
  iVar2 = check_for_cip_effects_removal(trigger_cause_controller, trigger_cause);
  if (iVar2 != 0)
  {
    return;
  }
  if ((type != TYPE_NONE) &&
      (iVar2 = is_what(trigger_cause_controller, trigger_cause, type), iVar2 == 0))
  {
    return;
  }
  if (test != NULL)
  {
    iVar2 = new_make_test_in_play(trigger_cause_controller, trigger_cause, -1, test);
    if (iVar2 == 0)
    {
      return;
    }
    if (((test->not_me == 1) && (trigger_cause_controller == triggering_player)) &&
        (trigger_cause == triggering_card))
    {
      return;
    }
  }
  if (event == EVENT_TRIGGER)
  {
    if (trigger_mode == 3)
    {
      iVar2 = duh_mode(triggering_player);
      trigger_mode = (iVar2 != 0) + 1;
    }
    event_result = event_result | trigger_mode;
    return;
  }
  if (event != EVENT_RESOLVE_TRIGGER)
  {
    return;
  }
  iVar4 = 1;
  instance = get_card_instance(trigger_cause_controller, trigger_cause);
  instance->regen_status = instance->regen_status | 0x6000000;
  iVar2 = is_what(trigger_cause_controller, trigger_cause, 0x47);
  if (iVar2 != 0)
  {
    iVar4 = card_counts[triggering_player][4] + 1;
  }
  iVar2 = is_what(trigger_cause_controller, trigger_cause, 0x42);
  if (iVar2 != 0)
  {
    iVar4 = iVar4 + (unsigned int)card_counts[triggering_player][2];
  }
  iVar2 = is_what(trigger_cause_controller, trigger_cause, 1);
  if (iVar2 != 0)
  {
    iVar4 = iVar4 + (unsigned int)card_counts[triggering_player][1];
  }
  iVar2 = has_subtype(trigger_cause_controller, trigger_cause, SUBTYPE_WIZARD);
  if (iVar2 == 0)
  {
    iVar2 = has_subtype(triggering_player, triggering_card, SUBTYPE_SHAMAN);
    if ((iVar2 == 0) ||
        (iVar2 = has_subtype(triggering_player, triggering_card, SUBTYPE_WIZARD), iVar2 != 0))
      goto LAB_026a5c41;
  LAB_026a5d03:
    iVar2 = count_harmonic_prodigy(triggering_player, triggering_card);
    iVar4 = iVar4 + iVar2;
  }
  else
  {
    bVar1 = card_counts[triggering_player][3];
    iVar2 = count_harmonic_prodigy(triggering_player, triggering_card);
    iVar4 = iVar4 + (unsigned int)bVar1 + iVar2;
    iVar2 = has_subtype(triggering_player, triggering_card, SUBTYPE_SHAMAN);
    if ((iVar2 != 0) &&
        (iVar2 = has_subtype(triggering_player, triggering_card, SUBTYPE_WIZARD), iVar2 == 0))
      goto LAB_026a5d03;
  }
  if (iVar4 == 0)
  {
    return;
  }
LAB_026a5c41:
  do
  {
    call_card_function(triggering_player, triggering_card, EVENT_ETB);
    iVar4 = iVar4 + -1;
  } while (iVar4 != 0);
  return;
}

#endif

#ifndef SHANDALAR
/* Recovered October 2022 implementations; see docs/recovery-progress.md. */
int deflect_away_from_me(int player, int card, event_t event, int mode, int cless, int black,
                         int blue, int green, int red, int white, uint32_t variable_costs,
                         target_definition_t *td, int must_be_creature);
static char *has_only_1_target_and_could_legally_change_target_and_targets_me(int who_chooses,
                                                                              int player, int card,
                                                                              int targeting_player,
                                                                              int targeting_card);

/* ManalinkEh 2022: 0x2678000; original functions/functions.c:15356. */
static char *has_only_1_target_and_could_legally_change_target_and_targets_me(int who_chooses,
                                                                              int player, int card,
                                                                              int targeting_player,
                                                                              int targeting_card)
{
  card_instance_t *instance;
  char *pcVar2;
  int result;

  instance = get_card_instance(player, card);
  pcVar2 = "must have only 1 target";
  if (instance->number_of_targets == '\x01')
  {
    result = call_card_function(player, card, EVENT_CAN_CHANGE_TARGET);
    if (result == 0)
    {
      return "cannot change target";
    }
    pcVar2 = "must target this card";
    if ((instance->targets[0].player == targeting_player) &&
        (instance->targets[0].card == targeting_card))
    {
      pcVar2 = NULL;
    }
  }
  return pcVar2;
}

/* ManalinkEh 2022: 0x269e400; original functions/functions.c:15373. */
int deflect_away_from_me(int player, int card, event_t event, int mode, int cless, int black,
                         int blue, int green, int red, int white, uint32_t variable_costs,
                         target_definition_t *td, int must_be_creature)
{
  int32_t iVar1;
  int iVar2;
  card_instance_t *pcVar3;
  int iVar4;
  int iVar5;
  int iVar6;
  card_instance_t *pcVar7;
  target_definition_t td_default;

  if (td == NULL)
  {
    td = &td_default;
    counterspell_target_definition(player, card, td, 0xff);
    td_default.special = td_default.special | 0x8000;
    td_default.extra = (int32_t)has_only_1_target_and_could_legally_change_target_and_targets_me;
  }
  pcVar3 = get_card_instance(player, card);
  if (event != EVENT_CAN_ACTIVATE)
  {
    if (event == EVENT_ACTIVATE)
    {
      if ((1 < stack_size) && (iVar4 = granted_generic_activated_ability(
                                   player, card, player, card, EVENT_ACTIVATE, mode | 0x400, cless,
                                   black, blue, green, red, white, variable_costs, td, NULL),
                               iVar6 = card_on_stack_controller, iVar4 != 0))
      {

        pcVar3->number_of_targets = '\x01';
        pcVar3->targets[0].player = iVar6;
        iVar4 = card_on_stack;
        pcVar3->targets[0].card = card_on_stack;
        iVar5 = counterspell_validate(player, card, td, 0);
        if (iVar5 != 0)
        {

          if (must_be_creature != 0)
          {
            set_special_flags2(iVar6, iVar4, 0x4000);
          }
          iVar2 = attacking_card_controller;
          iVar5 = attacking_card;
          attacking_card_controller = player;
          attacking_card = card;
          call_card_function(iVar6, iVar4, EVENT_CHANGE_TARGET);
          attacking_card = iVar5;
          attacking_card_controller = iVar2;
          remove_special_flags2(iVar6, iVar4, 0x4000);
        }
      }
    }
    else if ((event == EVENT_RESOLVE_ACTIVATION) &&
             (iVar6 = counterspell_validate(player, card, td, 0), iVar6 != 0))
    {

      pcVar7 = get_card_instance(pcVar3->targets[0].player, pcVar3->targets[0].card);
      iVar1 = pcVar3->targets[1].player;
      pcVar7->targets[0].card = pcVar3->targets[1].card;
      pcVar7->targets[0].player = iVar1;
    }
    return 0;
  }
  iVar6 = granted_generic_activated_ability(player, card, player, card, EVENT_CAN_ACTIVATE,
                                                 mode | 0x400, cless, black, blue, green, red,
                                                 white, variable_costs, td, NULL);
  return iVar6;
}

#endif


#ifndef SHANDALAR
/* Recovered native October 2022 spell-cast trigger interfaces. */

/* ManalinkEh 2022: 0x2684f40; original functions/functions.c:19902. */
static void sp_ability_trigger_impl(int controller_of_trigger, event_t event, int trigger_mode,
                                         type_t type, test_definition_t *test,
                                         int triggering_player, int triggering_card,
                                         int allow_copies)
{
  int iVar1;
  card_instance_t *instance;
  int iVar3;
  if (trigger_condition != 0xd3)
  {
    return;
  }
  if (reason_for_trigger_controller != triggering_player)
  {
    return;
  }
  iVar1 = is_what(trigger_cause_controller, trigger_cause, 1);
  if (iVar1 != 0)
  {
    return;
  }
  if (triggering_card != affected_card)
  {
    return;
  }
  if (triggering_player != affected_card_controller)
  {
    return;
  }
  iVar1 = is_humiliated(triggering_player, triggering_card, AL_TRIG);
  if (iVar1 != 0)
  {
    return;
  }
  if ((controller_of_trigger != 2) && (controller_of_trigger != trigger_cause_controller))
  {
    return;
  }
  instance = get_card_instance(trigger_cause_controller, trigger_cause);
  if (((((uint32_t)instance->ext->special_flags2) & 0x80000000U) != 0) && (allow_copies == 0))
  {
    return;
  }
  if ((type != TYPE_NONE) &&
      (iVar1 = is_what(trigger_cause_controller, trigger_cause, type), iVar1 == 0))
  {
    return;
  }
  if (test != NULL)
  {
    iVar1 = new_make_test_in_play(trigger_cause_controller, trigger_cause, -1, test);
    if (iVar1 == 0)
    {
      return;
    }
    if (((test->not_me == 1) && (triggering_player == trigger_cause_controller)) &&
        (triggering_card == trigger_cause))
    {
      return;
    }
  }
  if (event == EVENT_TRIGGER)
  {
    if (trigger_mode == 3)
    {
      iVar1 = duh_mode(triggering_player);
      trigger_mode = (iVar1 != 0) + 1;
    }
    event_result = event_result | trigger_mode;
    return;
  }
  if (event != EVENT_RESOLVE_TRIGGER)
  {
    return;
  }
  instance = get_card_instance(trigger_cause_controller, trigger_cause);
  instance->regen_status = instance->regen_status | 0x6000000;
  iVar1 = has_subtype(triggering_player, triggering_card, SUBTYPE_WIZARD);
  if (iVar1 == 0)
  {
    iVar1 = has_subtype(triggering_player, triggering_card, SUBTYPE_SHAMAN);
    if (iVar1 == 0)
    {
      call_card_function(triggering_player, triggering_card, EVENT_SPA);
      return;
    }
    iVar1 = 1;
    iVar3 = has_subtype(triggering_player, triggering_card, SUBTYPE_WIZARD);
    if (iVar3 != 0)
      goto LAB_0268517e;
  LAB_026851cc:
    iVar3 = count_harmonic_prodigy(triggering_player, triggering_card);
    iVar1 = iVar1 + iVar3;
  }
  else
  {
    iVar1 = count_harmonic_prodigy(triggering_player, triggering_card);
    iVar1 = iVar1 + 1;
    iVar3 = has_subtype(triggering_player, triggering_card, SUBTYPE_SHAMAN);
    if ((iVar3 != 0) &&
        (iVar3 = has_subtype(triggering_player, triggering_card, SUBTYPE_WIZARD), iVar3 == 0))
      goto LAB_026851cc;
  }
  if (iVar1 == 0)
  {
    return;
  }
LAB_0268517e:
  do
  {
    call_card_function(triggering_player, triggering_card, EVENT_SPA);
    iVar1 = iVar1 + -1;
  } while (iVar1 != 0);
  return;
}


/* ManalinkEh 2022: 0x26a8120; original functions/functions.c:19963. */
void sp_ability_trigger(int controller_of_trigger, event_t event, int trigger_mode,
                             type_t type, test_definition_t *test, int triggering_player,
                             int triggering_card)
{
  sp_ability_trigger_impl(controller_of_trigger, event, trigger_mode, type, test,
                               triggering_player, triggering_card, 0);
  return;
}

#endif


#ifndef SHANDALAR
/* ManalinkEh 2022: 0x26a8080; original functions/functions.c:19896. */
void disable_legend_rule(int player, int card, event_t event, int t_player)
{
  card_instance_t *instance;
  int result;
  if (((event == EVENT_STATIC_EFFECTS) && (card == affected_card)) &&
      (player == affected_card_controller))
  {
    instance = in_play(player, card);
    if (instance != NULL)
    {
      result = is_humiliated(player, card, AL_STATIC);
      if (result == 0)
      {
        if ((t_player & 0xfffffffdU) == 0)
        {
          player_bits[0] = (player_bits_t)((uint32_t)player_bits[0] | PB_LEGEND_RULE_DISABLED);
        }
        if (t_player - 1U < 2)
        {
          player_bits[1] = (player_bits_t)((uint32_t)player_bits[1] | PB_LEGEND_RULE_DISABLED);
          return;
        }
      }
    }
  }
  return;
}

#endif


#ifndef SHANDALAR
/* Recovered October 2022 death-trigger counting and resolution. */

/* ManalinkEh 2022: 0x2694580; original functions/functions.c:9944. */
static void count_for_gfp_ability_and_store_values_impl(int player, int card, int event,
                                                             int t_player, int type,
                                                             test_definition_t *this_test,
                                                             CardPairFunction extra_check, int mode,
                                                             int storage_location)
{
  char *pcVar1;
  card_instance_t *pcVar2;
  int iVar3;
  int iVar4;
  card_instance_t *pcVar5;
  unsigned int player_00;
  int iVar6;
  int iVar7;
  unsigned int uVar8;
  int csvids[3];
  iVar4 = affected_card;
  iVar3 = affected_card_controller;
  if (event != 0x77)
  {
    return;
  }

  pcVar2 = in_play(affected_card_controller, affected_card);
  if (pcVar2 == NULL)
  {
    return;
  }
  pcVar2 = get_card_instance(iVar3, iVar4);
  if ((pcVar2->ext->special_flags2 & SF2_WILL_BE_EXILED_IF_PUTTED_IN_GRAVE) != 0)
  {
    return;
  }
  pcVar2 = get_card_instance(iVar3, iVar4);
  if (2 < pcVar2->kill_code - 1)
  {
    return;
  }
  uVar8 = mode & 0x20;
  if (t_player == 2)
  {
    if (uVar8 != 0)
    {
      get_card_instance(affected_card_controller, affected_card);
      return;
    }
  }
  else
  {
    if (t_player == affected_card_controller)
    {
      if (uVar8 == 0)
        goto LAB_02694650;
    }
    else if (uVar8 == 0)
    {
      return;
    }
    pcVar2 = get_card_instance(affected_card_controller, affected_card);
    if (t_player != (pcVar2->state >> 0xc & 1))
    {
      return;
    }
  }
LAB_02694650:
  if (this_test == NULL)
  {
    if (type != 0)
    {
      iVar3 = is_what(affected_card_controller, affected_card, type);
      goto joined_r0x02694880;
    }
  }
  else
  {
    if (((this_test->not_me == 1) && (card == affected_card)) &&
        (player == affected_card_controller))
    {
      return;
    }
    if ((type != 0) && (iVar3 = is_what(affected_card_controller, affected_card, type), iVar3 == 0))
    {
      return;
    }
    iVar3 = new_make_test_in_play(affected_card_controller, affected_card, -1, this_test);
  joined_r0x02694880:
    if (iVar3 == 0)
    {
      return;
    }
  }
  if ((extra_check != (CardPairFunction)0x0) &&
      (iVar3 = (*extra_check)(affected_card_controller, affected_card), iVar3 == 0))
  {
    return;
  }
  iVar3 = check_for_death_trigger_removal(affected_card_controller, affected_card);
  if (iVar3 != 0)
  {
    return;
  }
  pcVar2 = get_card_instance(player, card);
  if (pcVar2->targets[0xb].player < 0)
  {
    pcVar2->targets[0xb].player = 0;
  }
  iVar4 = is_token(affected_card_controller, affected_card);
  iVar3 = affected_card;
  player_00 = affected_card_controller;
  uVar8 = mode & 0x400;
  if (iVar4 == 0)
  {
  joined_r0x026948d2:
    if (uVar8 != 0)
    {
      return;
    }
  }
  else if (uVar8 == 0)
  {
    uVar8 = mode & 0x440;
    goto joined_r0x026948d2;
  }
  if ((mode & 0x189fU) != 0)
  {
    if ((mode & 0x10U) == 0)
    {
      if (pcVar2->targets[storage_location].player < 0)
      {
        pcVar2->targets[storage_location].player = 0;
      }
      if ((mode & 1U) != 0)
      {
        iVar4 = get_abilities(player_00, iVar3, EVENT_POWER, -1);
        iVar3 = affected_card;
        pcVar2->targets[storage_location].player =
            pcVar2->targets[storage_location].player + (card_counts[player][5] + 1) * iVar4;
        player_00 = affected_card_controller;
      }
      if ((mode & 2U) != 0)
      {
        iVar3 = get_abilities(player_00, iVar3, EVENT_TOUGHNESS, -1);
        player_00 = affected_card_controller;
        pcVar2->targets[storage_location].player =
            pcVar2->targets[storage_location].player + (card_counts[player][5] + 1) * iVar3;
        iVar3 = affected_card;
      }
      if ((mode & 4U) != 0)
      {
        iVar4 = get_cmc(player_00, iVar3);
        iVar6 = is_what(affected_card_controller, affected_card, 2);
        iVar3 = affected_card;
        iVar7 = 1;
        if (iVar6 != 0)
        {
          iVar7 = card_counts[player][5] + 1;
        }
        pcVar2->targets[storage_location].player =
            pcVar2->targets[storage_location].player + iVar7 * iVar4;
        player_00 = affected_card_controller;
      }
      if ((mode & 8U) != 0)
      {
        uVar8 = get_color(player_00, iVar3);
        iVar3 = affected_card;
        pcVar2->targets[storage_location].player = pcVar2->targets[storage_location].player | uVar8;
        player_00 = affected_card_controller;
      }
      if ((mode & 0x800U) != 0)
      {
        iVar3 = count_counters(player_00, iVar3, COUNTER_FUNGUS);
        player_00 = affected_card_controller;
        pcVar2->targets[storage_location].player =
            pcVar2->targets[storage_location].player + (card_counts[player][5] + 1) * iVar3;
        iVar3 = affected_card;
      }
    }
    else
    {
      iVar4 = (unsigned int)(unsigned char)pcVar2->targets[0xb].player + storage_location;
      iVar3 = get_id(affected_card_controller, affected_card);
      if ((iVar3 == 0x3eae) ||
          (iVar3 = get_id(affected_card_controller, affected_card), iVar3 == 0x3ef4))
      {
      LAB_02694a83:
        iVar6 = 2;
        *(char *)&pcVar2->targets[0xb].player = (char)pcVar2->targets[0xb].player + '\x01';
      }
      else
      {
        iVar3 = get_id(affected_card_controller, affected_card);
        iVar6 = 1;
        if (iVar3 == 0x3efb)
          goto LAB_02694a83;
      }
      unmeld(iVar3, csvids + 1, csvids + 2);
      if ((iVar4 < 0xb) && ((t_player == 2 || ((mode & 0x80U) != 0))))
      {
        do
        {
          pcVar5 = get_card_instance(affected_card_controller, affected_card);
          pcVar2->targets[iVar4].player = pcVar5->state >> 0xc & 1;
          iVar3 = get_internal_card_id_from_csv_id(csvids[iVar6]);
          pcVar2->targets[iVar4].card = iVar3;
          if ((mode & 0x1000U) != 0)
          {
            iVar3 = get_original_internal_card_id(affected_card_controller, affected_card);
            pcVar2->targets[iVar4].player = pcVar2->targets[iVar4].player | iVar3 << 4;
          }
          iVar4 = iVar4 + 1;
          iVar6 = iVar6 + -1;
          player_00 = affected_card_controller;
          iVar3 = affected_card;
        } while ((iVar6 != 0) && (iVar4 < 0xb));
      }
      else
      {
        player_00 = affected_card_controller;
        iVar3 = affected_card;
        if (storage_location < 0xb)
        {
          do
          {
            if (pcVar2->targets[storage_location].player == -1)
            {
              iVar3 = get_internal_card_id_from_csv_id(csvids[iVar6]);
              pcVar2->targets[storage_location].player = iVar3;
            }
            else if (pcVar2->targets[storage_location].card == -1)
            {
              iVar3 = get_internal_card_id_from_csv_id(csvids[iVar6]);
              pcVar2->targets[storage_location].card = iVar3;
            }
            else
            {
              iVar6 = iVar6 + -1;
              storage_location = storage_location + 1;
            }
            player_00 = affected_card_controller;
            iVar3 = affected_card;
          } while ((storage_location < 0xb) && (iVar6 != 0));
        }
      }
    }
  }
  if ((mode & 0x300U) == 0)
  {
    *(char *)&pcVar2->targets[0xb].player = (char)pcVar2->targets[0xb].player + '\x01';
    iVar3 = is_what(player_00, iVar3, 2);
  }
  else
  {
    uVar8 = player_00;
    if ((mode & 0x100U) != 0)
    {
      pcVar5 = get_card_instance(player_00, iVar3);
      player_00 = pcVar5->state >> 0xc & 1;
      iVar3 = affected_card;
      uVar8 = affected_card_controller;
    }
    if (player_00 == 1)
    {
      *(char *)((int)&pcVar2->targets[0xb].player + 1) =
          *(char *)((int)&pcVar2->targets[0xb].player + 1) + '\x01';
      iVar3 = is_what(uVar8, iVar3, 2);
      if ((iVar3 != 0) && ((mode & 0x10U) == 0))
      {
        pcVar1 = (char *)((int)&pcVar2->targets[0xb].player + 1);
        *pcVar1 = *pcVar1 + card_counts[player][5];
      }
      goto LAB_02694810;
    }
    *(char *)&pcVar2->targets[0xb].player = (char)pcVar2->targets[0xb].player + '\x01';
    iVar3 = is_what(uVar8, iVar3, 2);
  }
  if ((iVar3 != 0) && ((mode & 0x10U) == 0))
  {
    *(uint8_t *)&pcVar2->targets[0xb].player =
        (char)pcVar2->targets[0xb].player + card_counts[player][5];
  }
LAB_02694810:
  pcVar2->targets[0xb].card = 0;
  return;
}


/* ManalinkEh 2022: 0x2694c90; original functions/functions.c:10062. */
void count_for_gfp_ability_and_store_values_extra(int player, int card, int event,
                                                       int t_player, int type,
                                                       CardPairFunction extra_check, int mode,
                                                       int storage_location)
{
  count_for_gfp_ability_and_store_values_impl(player, card, event, t_player, type, NULL,
                                                   extra_check, mode, storage_location);
  return;
}


/* ManalinkEh 2022: 0x2694ce0; original functions/functions.c:10066. */
void count_for_gfp_ability_and_store_values(int player, int card, int event, int t_player,
                                                 int type, test_definition_t *this_test,
                                                 int mode, int storage_location)
{
  count_for_gfp_ability_and_store_values_impl(player, card, event, t_player, type, this_test,
                                                   (CardPairFunction)0x0, mode, storage_location);
  return;
}


/* ManalinkEh 2022: 0x2694d30; original functions/functions.c:10070. */
void count_for_gfp_ability(int player, int card, int event, int t_player, int type,
                                test_definition_t *this_test)
{
  count_for_gfp_ability_and_store_values_impl(player, card, event, t_player, type, this_test,
                                                   (CardPairFunction)0x0, 0, 0);
  return;
}


/* ManalinkEh 2022: 0x2689440; original functions/functions.c:10074. */
int resolve_gfp_ability(int player, int card, event_t event, resolve_trigger_t trig_mode)
{
  card_instance_t *instance;
  int result;

  if (trigger_condition != 0xd5)
  {
    return 0;
  }
  if ((reason_for_trigger_controller == player) &&
      (instance = in_play(player, card), instance != NULL))
  {
    result = instance->targets[0xb].player;
    if ((0 < result) && ((card == affected_card && (player == affected_card_controller))))
    {
      if (event == EVENT_TRIGGER)
      {
        if (trig_mode == RESOLVE_TRIGGER_DUH)
        {
          result = duh_mode(player);
          trig_mode = (result != 0) + RESOLVE_TRIGGER_OPTIONAL;
        }
        event_result = event_result | trig_mode;
        return 0;
      }
      if (event == EVENT_RESOLVE_TRIGGER)
      {
        instance->targets[0xb].card = result;
        instance->targets[0xb].player = 0;
        return result;
      }
      if (event == EVENT_END_TRIGGER)
      {
        instance->targets[0xb].player = 0;
      }
    }
  }
  return 0;
}

#endif


#ifndef SHANDALAR
/* Recovered October 2022 action and copy helpers. */

/* ManalinkEh 2022: 0x2686bb0; original functions/functions.c:5847. */
void shapeshift_all(int src_player, int src_card, int t_player, test_definition_t *test,
                         int turn_into_player, int turn_into_card, int mode)
{
  int iVar1;
  int iVar2;
  card_instance_t *instance;
  unsigned int uVar4;
  int iVar5;
  char *pcVar6;
  int iVar7;
  int iVar8;
  char (*pacVar9)[151];
  bool bVar10;
  unsigned char bVar11;
  int iStack_154;
  char marked[2][151];

  bVar11 = 0;
  pcVar6 = marked[0];
  iVar1 = new_get_test_score(test);
  uVar4 = 0x12e;
  bVar10 = ((unsigned int)pcVar6 & 2) != 0;
  if (bVar10)
  {
    marked[0][0] = '\0';
    marked[0][1] = '\0';
    pcVar6 = marked[0] + 2;
    uVar4 = 300;
  }
  for (uVar4 = uVar4 >> 2; uVar4 != 0; uVar4 = uVar4 - 1)
  {
    pcVar6[0] = '\0';
    pcVar6[1] = '\0';
    pcVar6[2] = '\0';
    pcVar6[3] = '\0';
    pcVar6 = pcVar6 + (unsigned int)bVar11 * -8 + 4;
  }
  if (!bVar10)
  {
    pcVar6[0] = '\0';
    pcVar6[1] = '\0';
  }
  iVar2 = is_legendary(turn_into_player, turn_into_card);
  iStack_154 = 0;
  bVar10 = t_player == 2;
  iVar7 = 0;
  do
  {
    if (((t_player == iVar7) || (bVar10)) && (0 < active_cards_count[iVar7]))
    {
      iVar8 = 0;
      do
      {
        instance = in_play(iVar7, iVar8);
        if ((instance != NULL) &&
            ((((test->not_me != 1 || (src_card != iVar8)) || (src_player != iVar7)) &&
              (iVar5 = new_make_test_in_play(iVar7, iVar8, iVar1, test), iVar5 != 0))))
        {
          marked[iVar7][iVar8] = '\x01';
          iStack_154 = iStack_154 + 1;
        }
        iVar8 = iVar8 + 1;
      } while (iVar8 < active_cards_count[iVar7]);
    }
    iVar7 = iVar7 + 1;
  } while (iVar7 != 2);
  iVar1 = 0;
  iVar7 = current_turn;
  iVar8 = current_turn;
  do
  {
    if ((t_player == iVar7) || (bVar10))
    {
      if (0 < active_cards_count[iVar7])
        goto LAB_02686e2f;
      iVar1 = 1;
      iVar8 = iVar7;
    }
    else
    {
      iVar1 = 1;
    }
    while (iVar1 != 0)
    {
      iVar8 = 1 - iVar7;
      if (((iVar8 == t_player) || (bVar10)) && (0 < active_cards_count[iVar8]))
      {
      LAB_02686e2f:
        iVar7 = 0;
        do
        {
          while ((marked[iVar8][iVar7] != '\0' &&
                  (instance = in_play(iVar8, iVar7), instance != NULL)))
          {
            iVar5 = iVar7 + 1;
            real_shapeshift_target(src_player, src_card, iVar8, iVar7, turn_into_player,
                                   turn_into_card, mode);
            iVar7 = iVar5;
            if (active_cards_count[iVar8] <= iVar5)
              goto LAB_02686cb1;
          }
          iVar7 = iVar7 + 1;
        } while (iVar7 < active_cards_count[iVar8]);
      }
    LAB_02686cb1:
      iVar1 = iVar1 + 1;
      iVar7 = current_turn;
      iVar8 = current_turn;
      if (1 < iVar1)
      {
        if ((0 < iStack_154) && (iVar2 != 0))
        {
          pacVar9 = marked;
          iVar1 = 0;
          do
          {
            iVar2 = 0;
            if (0 < active_cards_count[iVar1])
            {
              do
              {
                if ((((*pacVar9)[iVar2] != '\0') &&
                     (instance = in_play(iVar1, iVar2), instance != NULL)) &&
                    (iVar7 = is_legendary(iVar1, iVar2), iVar7 != 0))
                {
                  iVar7 = get_id(iVar1, iVar2);
                  verify_legend_rule(iVar1, iVar2, iVar7);
                  return;
                }
                iVar2 = iVar2 + 1;
              } while (iVar2 < active_cards_count[iVar1]);
            }
            iVar1 = iVar1 + 1;
            pacVar9 = pacVar9 + 1;
          } while (iVar1 != 2);
        }
        return;
      }
    }
  } while (true);
}


/* ManalinkEh 2022: 0x267f4a0; original functions/functions.c:5667. */
int enters_the_battlefield_as_copy_of_any(int player, int card, event_t event,
                                               target_definition_t *td, const char *prompt)
{
  unsigned int uVar1;
  card_instance_t *instance;
  unsigned int t_card;
  int result;

  if (event == EVENT_RESOLVE_SPELL)
  {
    instance = get_card_instance(player, card);
    instance->state = instance->state | 0x800000;
    result = pick_target(td, prompt);
    instance->state = instance->state & 0xff7fffff;
    if (result == 0)
    {
      spell_fizzled = 0;
      t_card = 0xffffffff;
      result = -1;
      uVar1 = 0;
      goto LAB_0267f4bc;
    }
    t_card = instance->targets[0].card;
    instance->number_of_targets = '\0';
    result = instance->targets[0].player;
    if ((t_card != card) || (player != result))
    {
      uVar1 = (t_card ^ 0xffffffff) >> 0x1f;
      goto LAB_0267f4bc;
    }
  }
  uVar1 = 0;
  t_card = 0xffffffff;
  result = -1;
LAB_0267f4bc:
  enters_the_battlefield_as_copy_of(player, card, event, result, t_card);
  return uVar1;
}

#endif


#ifndef SHANDALAR
/* Recovered October 2022 specific enters-play helpers. */

/* ManalinkEh 2022: 0x2684210; original functions/functions.c:6906. */
static int new_specific_cip_impl(int controller_of_trigger, event_t event, int trigger_mode,
                                      type_t type, test_definition_t *test,
                                      int triggering_player, int triggering_card)
{
  int iVar1;
  card_instance_t *instance;
  int iVar3;
  if (reason_for_trigger_controller == trigger_cause_controller)
  {
    iVar1 = trigger_cause_controller;
    iVar3 = trigger_cause;
    if (triggering_player != -1 && triggering_card != -1)
    {
      iVar1 = triggering_player;
      iVar3 = triggering_card;
    }
    if (((((iVar3 == affected_card) && (iVar1 == affected_card_controller)) &&
          ((trigger_cause_controller == controller_of_trigger || (controller_of_trigger == 2)))) &&
         (iVar1 = check_for_cip_effects_removal(trigger_cause_controller, trigger_cause),
          iVar1 == 0)) &&
        (((type == TYPE_NONE ||
           (iVar1 = is_what(trigger_cause_controller, trigger_cause, type), iVar1 != 0)) &&
          ((test == NULL ||
            ((iVar1 = new_make_test_in_play(trigger_cause_controller, trigger_cause, -1, test),
              iVar1 != 0 &&
                  (((test->not_me != 1 || (triggering_player != trigger_cause_controller)) ||
                    (triggering_card != trigger_cause))))))))))
    {
      if (event == EVENT_TRIGGER)
      {
        if (trigger_mode == 3)
        {
          iVar1 = duh_mode(triggering_player);
          trigger_mode = (iVar1 != 0) + 1;
        }
        event_result = event_result | trigger_mode;
        return 0;
      }
      if (event == EVENT_RESOLVE_TRIGGER)
      {
        iVar3 = trigger_cause;
        iVar1 = trigger_cause_controller;
        if (triggering_player != -1 && triggering_card != -1)
        {
          instance = get_card_instance(triggering_player, triggering_card);
          iVar3 = trigger_cause;
          iVar1 = trigger_cause_controller;
          instance->targets[1].player = trigger_cause_controller;
          instance->targets[1].card = iVar3;
        }
        instance = get_card_instance(iVar1, iVar3);
        instance->regen_status = instance->regen_status | 0x6000000;
        return 1;
      }
    }
  }
  return 0;
}


/* ManalinkEh 2022: 0x2684410; original functions/functions.c:6885. */
int specific_cip(int player, int card, event_t event, int t_player, int trigger_mode, int type,
                      int flag1, int subtype, int flag2, int color, int flag3, int id, int flag4,
                      int cc, int flag5)
{
  int iStack_e4;
  test_definition_t test;
  iStack_e4 = 0;
  if (trigger_condition == 0xdb)
  {
    if (((card == affected_card) && (player == affected_card_controller)) &&
        (reason_for_trigger_controller == player))
    {
      iStack_e4 = 0;
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
      if (trigger_condition == 0xdb)
      {
        iStack_e4 =
            new_specific_cip_impl(player, event, trigger_mode, TYPE_NONE, &test, player, card);
      }
    }
    return iStack_e4;
  }
  return iStack_e4;
}


/* ManalinkEh 2022: 0x2684500; original functions/functions.c:6966. */
int new_specific_cip_global(int player, event_t event, int trigger_mode, type_t type,
                                 test_definition_t *this_test, int triggering_player,
                                 int triggering_card)
{
  int result;
  if (trigger_condition != 0xdb)
  {
    return 0;
  }
  result = new_specific_cip_impl(player, event, trigger_mode, type, this_test,
                                      triggering_player, triggering_card);
  return result;
}


/* ManalinkEh 2022: 0x2684560; original functions/functions.c:6962. */
int new_specific_cip(int player, int card, event_t event, int t_player, int trigger_mode,
                          test_definition_t *this_test)
{
  int result;
  if (trigger_condition != 0xdb)
  {
    return 0;
  }
  result =
      new_specific_cip_impl(player, event, trigger_mode, TYPE_NONE, this_test, player, card);
  return result;
}

#endif


#ifndef SHANDALAR
/* Recovered October 2022 life-gain and loss-prevention helpers. */

#define life_gain_context_push EXE_FN(void, 0x435C80, void)
#define life_gain_context_pop EXE_FN(void, 0x435CD0, void)
int life_to_lose_on_gain_life;
int suspend_cannot_lose_the_game;

/* ManalinkEh 2022: 0x2685c90; original functions/functions.c:14700. */
static int can_lose_the_game_impl(int player)
{
  card_instance_t *instance;
  int result;
  int card;
  int player_00;

  player_00 = 0;
  do
  {
    card = 0;
    if (0 < active_cards_count[player_00])
    {
      do
      {
        instance = in_play(player_00, card);
        if (instance != NULL)
        {
          result = is_what(player_00, card, 0x47);
          if (result == 0)
          {
            result = is_what(player_00, card, 0x80);
            if (((result != 0) && (instance = get_card_instance(player_00, card),
                                   instance->display_pic_csv_id == 0x211f)) &&
                (player == player_00))
            {
              return 0;
            }
          }
          else
          {
            result = get_id(player_00, card);
            if ((result == 0x51c) || (result == 0x4814))
            {
              if (player != player_00)
                goto LAB_02685cdd;
            LAB_02685ce8:
              result = is_humiliated(player_00, card, AL_STATIC);
              if (result == 0)
              {
                return 0;
              }
            }
            else if (result == 0x4214)
            {
              result = check_battlefield_for_subtype(player, 0x100, 0x7005);
              if ((result != 0) && (player == player_00))
                goto LAB_02685ce8;
            }
            else
            {
            LAB_02685cdd:
              if ((result == 0x29dc) && (player != player_00))
                goto LAB_02685ce8;
            }
          }
        }
        card = card + 1;
      } while (card < active_cards_count[player_00]);
    }
    player_00 = player_00 + 1;
    if (player_00 == 2)
    {
      return 1;
    }
  } while (true);
}


/* ManalinkEh 2022: 0x26927f0; original functions/functions.c:14700. */
int can_lose_the_game(int player)
{
  int result;

  if (suspend_cannot_lose_the_game == 0)
  {
    result = can_lose_the_game_impl(player);
    return result;
  }
  return 1;
}


/* ManalinkEh 2022: 0x268f0b0; original functions/functions.c:11369. */
int player_can_gain_life_as_cost(int player)
{
  int result;
  if (((uint32_t)player_bits[player] & PB_CANT_GAIN_LIFE) != 0)
  {
    return 0;
  }
  result = life_total_can_change(player);
  return (unsigned int)(result != 0);
}


/* ManalinkEh 2022: 0x268f730; original functions/functions.c:11288. */
static void gain_life_impl(int player, int amount, int from_lifelink)
{
  int iVar1;
  int iVar2;
  if (player == 2)
  {
    gain_life(current_turn, amount);
    gain_life(1 - current_turn, amount);
    return;
  }
  if (amount < 1)
  {
    return;
  }
  if (((uint32_t)player_bits[player] & PB_CANT_GAIN_LIFE) != 0)
  {
    return;
  }
  iVar2 = life_total_can_change(player);
  if (iVar2 == 0)
  {
    return;
  }
  if (((uint32_t)event_flags & EA_LICH) != 0)
  {
    life_gain_context_push();
    iVar1 = life_to_lose_on_gain_life;
    iVar2 = life_gained;
    life_to_lose_on_gain_life = 0;
    life_gained = amount;
    dispatch_xtrigger2(current_turn, XTRIGGER_REPLACE_GAIN_LIFE, "gaining life", 0, player,
                       from_lifelink);
    amount = life_gained;
    life_gained = iVar2;
    life_to_lose_on_gain_life = iVar1;
    life_gain_context_pop();
    if (amount < 1)
    {
      if (amount < 0)
      {
        lose_life(player, (amount ^ amount >> 0x1f) - (amount >> 0x1f));
      }
      goto LAB_0268f7f5;
    }
  }
  life[player] = life[player] + amount;
  increase_trap_condition(player, 1, amount);
  iVar2 = life_gained;
  if (((uint32_t)event_flags & EA_LICH) != 0)
  {
    life_gained = amount;
    dispatch_trigger2(current_turn, TRIGGER_GAIN_LIFE, "gained life", 0, player, -1);
  }
  life_gained = iVar2;
  if (life_gain_draw_count[player] != 0)
  {
    draw_cards(player, amount);
  }
LAB_0268f7f5:
  if (0 < life_to_lose_on_gain_life)
  {
    lose_life(player, life_to_lose_on_gain_life);
  }
  return;
}


/* ManalinkEh 2022: 0x268f900; original functions/functions.c:11331. */
void gain_life(int player, int amount)
{
  gain_life_impl(player, amount, 0);
  return;
}


/* ManalinkEh 2022: 0x268f9f0; original functions/functions.c:11335. */
void gain_life_from_lifelink(int player, int amount)
{
  gain_life_impl(player, amount, 1);
  return;
}


/* ManalinkEh 2022: 0x2692810; original functions/functions.c:14734. */
void lose_the_game(int player)
{
  int result;

  result = check_battlefield_for_id(player, 0x413a);
  if (result == 0)
  {
    if (player != 2)
    {
      if ((suspend_cannot_lose_the_game == 0) &&
          (result = can_lose_the_game_impl(player), result == 0))
      {
        return;
      }
      if (ai_is_speculating != 1)
      {
        real_lose_the_game(player);
        return;
      }
      life[player] = -99;
      life[1 - player] = 0x14;
      return;
    }
    if (ai_is_speculating != 1)
    {
      real_lose_the_game(2);
      return;
    }
    life[1] = -99;
    life[0] = -99;
  }
  else
  {
    life[player] = -1;
  }
  return;
}


/* ManalinkEh 2022: 0x268be60; original functions/functions.c:11341. */
int trigger_gain_life(int player, int card, event_t event, int who_gains,
                           resolve_trigger_t trigger_mode)
{
  int iVar1;
  int iVar2;

  iVar2 = life_gained;
  if (((((trigger_condition == 0xe0) && (reason_for_trigger_controller == player)) &&
        (0 < life_gained)) &&
       ((who_gains == 2 || (trigger_cause_controller == who_gains)))) &&
      ((card == affected_card &&
        ((player == affected_card_controller &&
          (iVar1 = is_humiliated(player, card, AL_TRIG), iVar1 == 0))))))
  {
    if (event == EVENT_TRIGGER)
    {
      if (trigger_mode == RESOLVE_TRIGGER_DUH)
      {
        iVar2 = duh_mode(player);
        trigger_mode = (iVar2 != 0) + RESOLVE_TRIGGER_OPTIONAL;
      }
      event_result = event_result | trigger_mode;
    }
    else if (event == EVENT_RESOLVE_TRIGGER)
    {
      return iVar2;
    }
  }
  return 0;
}

#undef life_gain_context_push
#undef life_gain_context_pop
#endif


#ifndef SHANDALAR
/* Recovered October 2022 cannot-lose protection helpers. */

/* ManalinkEh 2022: 0x26a3060; original functions/functions.c:11961. */
static void cannot_lose_the_game_impl(int player, int card, event_t event, int who_cant_lose,
                                           int for_poison, int for_draw_on_empty_deck)
{
  unsigned char bVar1;
  int iVar2;
  card_instance_t *instance;
  int32_t player_00;
  int iVar4;
  int iVar5;

  instance = in_play(player, card);
  if (instance == NULL)
  {
    return;
  }
  if ((instance->targets[5].player < 0) || (instance->targets[5].card < 0))
  {
    instance->targets[5].player = 0;
    instance->targets[5].card = 0;
    instance->targets[6].player = who_cant_lose;
    leaves_play(player, card, event, 0, 0);
    if (event != EVENT_LEAVES_PLAY_ABILITY)
      goto LAB_026a3128;
  LAB_026a31f0:
    instance->targets[6].card = 0x11;
    player_00 = instance->targets[6].player;
  }
  else
  {
    iVar2 = instance->targets[6].player;
    leaves_play(player, card, event, 0, 0);
    if (event == EVENT_LEAVES_PLAY_ABILITY)
      goto LAB_026a31f0;
    if (who_cant_lose == iVar2)
      goto LAB_026a3128;
    player_00 = instance->targets[6].player;
  }
  if (for_poison != 0)
  {
    raw_set_poison(player_00, (char)instance->targets[5].card + POISON_COUNTERS(player_00));
    player_00 = instance->targets[6].player;
  }
  iVar2 = life[player_00];
  instance->targets[5].card = 0;
  instance->targets[6].player = who_cant_lose;
  life[player_00] = iVar2 - instance->targets[5].player;
  instance->targets[5].player = 0;
LAB_026a3128:
  if (((((trigger_condition == 0xcf) && (*deck_ptr[who_cant_lose] == -1)) &&
        (who_cant_lose == reason_for_trigger_controller)) &&
       ((card == affected_card && (affected_card_controller == player)))) &&
      ((for_draw_on_empty_deck != 0 && (suppress_draw == 0))))
  {
    if (event == EVENT_TRIGGER)
    {
      event_result = 2;
    }
    else if (event == EVENT_RESOLVE_TRIGGER)
    {
      suppress_draw = 1;
    }
  }
  iVar2 = life[who_cant_lose];
  if (iVar2 < 1)
  {
    if (instance->targets[6].card != 0x11)
    {
      iVar5 = instance->targets[5].player;
      life[who_cant_lose] = 1;
      instance->targets[5].player = iVar5 + (1 - iVar2);
    }
  }
  else if ((1 < iVar2) && (iVar5 = instance->targets[5].player, 0 < iVar5))
  {
    iVar4 = iVar2 + -1;
    if (iVar5 < iVar2 + -1)
    {
      iVar4 = iVar5;
    }
    life[who_cant_lose] = iVar2 - iVar4;
    instance->targets[5].player = iVar5 - iVar4;
  }
  if (for_poison != 0)
  {
    bVar1 = POISON_COUNTERS(who_cant_lose);
    if (bVar1 < 10)
    {
      if ((bVar1 < 9) && (iVar2 = instance->targets[5].card, 0 < iVar2))
      {
        iVar5 = 9 - (unsigned int)bVar1;
        if (iVar2 < (int)(9 - (unsigned int)bVar1))
        {
          iVar5 = iVar2;
        }
        raw_set_poison(who_cant_lose, bVar1 + (char)iVar5);
        instance->targets[5].card = instance->targets[5].card - iVar5;
      }
    }
    else if (instance->targets[6].card != 0x11)
    {
      raw_set_poison(who_cant_lose, '\t');
      instance->targets[5].card = instance->targets[5].card + (bVar1 - 9);
      return;
    }
  }
  return;
}


/* ManalinkEh 2022: 0x26a3390; original functions/functions.c:12056. */
void cannot_lose_the_game_for_having_less_than_0_life(int player, int card, event_t event,
                                                           int who_cant_lose)
{
  cannot_lose_the_game_impl(player, card, event, who_cant_lose, 0, 0);
  return;
}


/* ManalinkEh 2022: 0x26a33c0; original functions/functions.c:12052. */
void cannot_lose_the_game(int player, int card, event_t event, int who_cant_lose)
{
  cannot_lose_the_game_impl(player, card, event, who_cant_lose, 1, 1);
  return;
}

#endif


#ifndef SHANDALAR
/* Recovered October 2022 native life-loss engine. */

/* ManalinkEh 2022: 0x268f1b0; original functions/functions.c:12300. */
int lose_life(int player, int amount)
{
  bool bVar1;
  bool bVar2;
  int iVar3;
  card_instance_t *instance;
  int iVar5;
  int iVar6;
  unsigned int uVar7;
  int iStack_4cc;
  int marked[2][151];

  if (amount < 1)
  {
    return 0;
  }
  if (player == 2)
  {
    iStack_4cc = 0;
    iVar3 = current_turn;
    do
    {
      iVar3 = lose_life(iVar3, amount);
      iStack_4cc = iStack_4cc + iVar3;
      player = 1 - current_turn;
      iVar3 = -1;
    } while (player == 2);
  }
  else
  {
    iStack_4cc = 0;
  }
  if (1 < (unsigned int)player)
  {
    return iStack_4cc;
  }
  iVar3 = life_total_can_change(player);
  if (iVar3 == 0)
  {
    return iStack_4cc;
  }
  memset(marked, 0, sizeof(marked));
  if (active_cards_count[player] < 1)
  {
    iVar3 = 1 - player;
    bVar1 = false;
    if (active_cards_count[iVar3] < 1)
    {
      increase_trap_condition(player, 2, amount);
      life[player] = life[player] - amount;
      play_sound_effect(WAV_LIFELOSS);
      return iStack_4cc + amount;
    }
  LAB_0268f330:
    bVar2 = false;
    iVar5 = 0;
    do
    {
      while (
          ((instance = in_play(iVar3, iVar5),
            instance != NULL && ((uVar7 = (unsigned int)cards_data[instance->internal_card_id].id,
                                  uVar7 == 0x2d75 || (uVar7 == 0x304d)))) &&
           (iVar6 = is_humiliated(iVar3, iVar5, AL_TRIG), iVar6 == 0)))
      {
        bVar2 = true;
        marked[iVar3][iVar5] = uVar7;
        iVar5 = iVar5 + 1;
        if (active_cards_count[iVar3] <= iVar5)
          goto LAB_0268f3d0;
      }
      iVar5 = iVar5 + 1;
    } while (iVar5 < active_cards_count[iVar3]);
  LAB_0268f3d0:
    increase_trap_condition(player, 2, amount);
    life[player] = life[player] - amount;
    play_sound_effect(WAV_LIFELOSS);
    if ((!bVar1) || (active_cards_count[player] < 1))
      goto LAB_0268f526;
  }
  else
  {
    bVar1 = false;
    iVar3 = 0;
    do
    {
      while ((
          (instance = in_play(player, iVar3),
           instance != NULL && (((((uVar7 = (unsigned int)cards_data[instance->internal_card_id].id,
                                    uVar7 == 0x1705 || (uVar7 == 0x804)) ||
                                   (uVar7 == 0x1aff)) ||
                                  ((uVar7 == 0x4148 || (uVar7 == 0x4814)))) ||
                                 (uVar7 == 0x4fce)))) &&
          (iVar5 = is_humiliated(player, iVar3, AL_TRIG), iVar5 == 0)))
      {
        bVar1 = true;
        marked[player][iVar3] = uVar7;
        iVar3 = iVar3 + 1;
        if (active_cards_count[player] <= iVar3)
          goto LAB_0268f307;
      }
      iVar3 = iVar3 + 1;
    } while (iVar3 < active_cards_count[player]);
  LAB_0268f307:
    iVar3 = 1 - player;
    if (0 < active_cards_count[iVar3])
      goto LAB_0268f330;
    increase_trap_condition(player, 2, amount);
    life[player] = life[player] - amount;
    play_sound_effect(WAV_LIFELOSS);
    if (!bVar1)
    {
      return iStack_4cc + amount;
    }
    bVar2 = false;
    if (active_cards_count[player] < 1)
    {
      return iStack_4cc + amount;
    }
  }
  iVar5 = 0;
  do
  {
    iVar6 = marked[player][iVar5];
    if (iVar6 == 0x4148)
    {
      instance = get_card_instance(player, iVar5);
      if (instance->targets[1].player != 0x42)
      {
        add_energy(player, 1);
        instance = get_card_instance(player, iVar5);
        instance->targets[1].player = 0x42;
      }
    }
    else if (iVar6 < 0x4149)
    {
      if (iVar6 == 0x1705)
      {
        gain_life_impl(player, amount * 2, 0);
      }
      else if (iVar6 == 0x1aff)
      {
        impose_sacrifice(player, iVar5, player, amount, 0x47, 0, 0, 0, 0, 0, 0, 0, -1, 0);
      }
      else if (iVar6 == 0x804)
      {
        oath_of_lim_dul_lifeloss_trigger(player, iVar5, amount);
      }
    }
    else if (iVar6 == 0x4814)
    {
      lichs_mastery_lifeloss_trigger(player, iVar5, amount);
    }
    else if (iVar6 == 0x4fce)
    {
      draw_cards(player, amount);
    }
    iVar5 = iVar5 + 1;
  } while (iVar5 < active_cards_count[player]);
LAB_0268f526:
  iStack_4cc = iStack_4cc + amount;
  if ((bVar2) && (0 < active_cards_count[iVar3]))
  {
    iVar5 = 0;
    do
    {
      if (marked[iVar3][iVar5] == 0x2d75)
      {
        mill(player, amount);
      }
      else if (marked[iVar3][iVar5] == 0x304d)
      {
        gain_life_impl(iVar3, amount, 0);
      }
      iVar5 = iVar5 + 1;
    } while (iVar5 < active_cards_count[iVar3]);
  }
  return iStack_4cc;
}

#endif


#ifndef SHANDALAR
/* ManalinkEh 2022: 0x2685a40; original functions/functions.c:562. */
int remove_until_upkeep_impl(int player, int card, int t_player, int t_card, int whose_upkeep,
                             int discard_cards_equal_to_toughness)
{
  unsigned int uVar1;
  unsigned int uVar2;
  bool bVar3;
  card_instance_t *instance;
  int iVar5;
  int card_00;
  int iVar6;
  unsigned int csvid;
  uint16_t uStack_28;
  int csvids[2];

  instance = in_play(t_player, t_card);
  if (instance != NULL)
  {
    iVar5 = is_token(t_player, t_card);
    card_00 = get_original_internal_card_id(t_player, t_card);
    instance = get_card_instance(t_player, t_card);
    csvids[0] = -1;
    uVar1 = instance->state;
    csvids[1] = -1;
    uVar2 =
        (unsigned int)cards_data[card_00].id | ((unsigned int)cards_data[card_00].reserved2 << 16);
    csvid = uVar2 & 0xffff;
    iVar6 = get_card_image_number(csvid, t_player, t_card);
    uStack_28 = (uint16_t)iVar6;
    if ((csvid == 0x3eae || csvid == 0x3ef4) || (bVar3 = false, csvid == 0x3efb))
    {
      unmeld(csvid, csvids, csvids + 1);
      card_00 = get_internal_card_id_from_csv_id(csvids[0]);
      bVar3 = true;
      uStack_28 = 0;
    }
    kill_card(t_player, t_card, KILL_EXILE);
    if (iVar5 == 0)
    {
      iVar5 = is_what(-1, card_00, 0x47);
      if (iVar5 != 0)
      {
        while (true)
        {
          iVar5 = create_legacy_effect(player, card, remove_until_upkeep_legacy);
          instance = get_card_instance(player, iVar5);
          instance->targets[0].card = card_00;
          instance->targets[0].player = uVar1 >> 0xc & 1;
          instance->targets[1].player = whose_upkeep;
          instance->targets[1].card = -(unsigned int)(discard_cards_equal_to_toughness == 0) | 0x42;
          instance->display_pic_csv_id = (uint16_t)uVar2;
          instance->display_pic_num = uStack_28;
          if (!bVar3)
            break;
          card_00 = get_internal_card_id_from_csv_id(csvids[1]);
          bVar3 = false;
        }
        return iVar5;
      }
    }
  }
  return -1;
}

#endif

#ifndef SHANDALAR
/* Recovered October 2022 implementations; see docs/recovery-progress.md. */

/* ManalinkEh 2022: 0x2685c10; original functions/functions.c:596. */
int remove_until_upkeep(int player, int card, int t_player, int t_card, int whose_upkeep)
{
  int result;

  result = remove_until_upkeep_impl(player, card, t_player, t_card, whose_upkeep, 0);
  return result;
}

/* ManalinkEh 2022: 0x2685c50; original functions/functions.c:600. */
int remove_until_upkeep_discard_cards_equal_to_toughness(int player, int card, int t_player,
                                                         int t_card, int whose_upkeep)
{
  int result;

  result = remove_until_upkeep_impl(player, card, t_player, t_card, whose_upkeep, 1);
  return result;
}

#endif


#ifndef SHANDALAR
/* ManalinkEh 2022: 0x267f930; original functions/functions.c:6234. */
void replace_all_instances_of_one_color_word_with_another(int player, int card, int t_player,
                                                               int t_card)
{
  unsigned int uVar1;
  card_instance_t *instance;
  int32_t iVar3;
  int iVar4;
  int iVar5;
  int iVar6;
  uint32_t uVar7;
  int iVar8;
  uint32_t uStack_38;
  uint32_t uStack_34;
  unsigned int uStack_30;
  int iStack_2c;
  int options[6];

  if (player == 1)
  {
    options[1] = 0;
    options[2] = 0;
    options[3] = 0;
    options[4] = 0;
    options[5] = 0;
    iVar5 = get_id(t_player, t_card);
    iVar3 = cards_ptr[iVar5]->sleight_color;
    instance = get_card_instance(t_player, t_card);
    if ((instance->ext->special_flags3 & SF3_HARDCODED_SLEIGHT_WORDS_REPLACED) == SF3_0)
    {
      set_special_flags3(t_player, t_card, 0x2000);
      iVar5 = get_id(t_player, t_card);
      iVar3 = get_colors_to_sleight_from_text(iVar5);
      iVar5 = get_id(t_player, t_card);
      cards_ptr[iVar5]->sleight_color = iVar3;
    }
    iVar5 = 1;
    do
    {
      while (((iVar3 >> ((unsigned char)iVar5 & 0x1f) & 1U) != 0 &&
              (instance = get_card_instance(t_player, t_card), instance->color_id[iVar5] == '\0')))
      {
        options[iVar5] = 1;
        iVar5 = iVar5 + 1;
        if (iVar5 == 6)
          goto LAB_0267fcf0;
      }
      iVar5 = iVar5 + 1;
    } while (iVar5 != 6);
  LAB_0267fcf0:
    iVar5 = 1;
    do
    {
      while (instance = get_card_instance(t_player, t_card), instance->color_id[iVar5] == '\0')
      {
        iVar5 = iVar5 + 1;
        if (iVar5 == 6)
          goto LAB_0267fd33;
      }
      instance = get_card_instance(t_player, t_card);
      iVar8 = iVar5 + 1;
      options[instance->color_id[iVar5]] = 1;
      iVar5 = iVar8;
    } while (iVar8 != 6);
  LAB_0267fd33:
    iStack_2c = options[3];
    iVar5 = options[1];
    iVar8 = options[4];
    iVar4 = options[2];
    iVar6 = options[5];
  }
  else
  {
    iStack_2c = 1;
    iVar5 = 1;
    iVar8 = 1;
    iVar4 = 1;
    iVar6 = 1;
  }
  uVar1 = dialog_impl(player, card, EVENT_RESOLVE_SPELL, "\001\014", "\001\003", "\001\004",
                      "From Black...", iVar5, 1, "From Blue...", iVar4, 1, "From Green...",
                      iStack_2c, 1, "From Red...", iVar8, 1, "From White...", iVar6, 1, 0, 0, 0, 0);
  options[1] = 0;
  options[2] = 0;
  options[3] = 0;
  options[4] = 0;
  options[5] = 0;
  options[uVar1] = 1;
  if ((uVar1 == 4) || (uVar1 == 1))
  {
    uStack_34 = 10;
  }
  else
  {
    uStack_34 = 1;
  }
  if ((uVar1 == 2) || (uVar1 == 5))
  {
    uStack_38 = 10;
  }
  else
  {
    uStack_38 = 1;
  }
  if (uVar1 - 1 < 2)
  {
    uVar7 = 10;
    iVar5 = 1;
  }
  else
  {
    uVar7 = 1;
    iVar5 = (-(unsigned int)(uVar1 - 3 < 2) & 9) + 1;
  }
  uStack_30 = (unsigned int)(options[3] == 0);
  iVar5 = dialog_impl(player, card, EVENT_RESOLVE_SPELL, "\001\002", "\001\014", "\001\003",
                      "\001\004", "...to Black", (unsigned int)(options[1] == 0),
                      (-(unsigned int)((uVar1 - 3 & 0xfffffffd) == 0) & 9) + 1, "...to Blue",
                      (unsigned int)(options[2] == 0), iVar5, "...to Green", uStack_30, uVar7,
                      "...to Red", (unsigned int)(options[4] == 0), uStack_38, "...to White",
                      (unsigned int)(options[5] == 0), uStack_34, 0, 0, 0, 0);
  instance = get_card_instance(t_player, t_card);
  uStack_30 = (unsigned int)iVar5;
  if (instance->color_id[uVar1] == '\0')
  {
    instance = get_card_instance(t_player, t_card);
    instance->color_id[uVar1] = (uint8_t)uStack_30;
  }
  iVar5 = 1;
  do
  {
    while (instance = get_card_instance(t_player, t_card), instance->color_id[iVar5] == uVar1)
    {
      instance = get_card_instance(t_player, t_card);
      iVar8 = iVar5 + 1;
      instance->color_id[iVar5] = (uint8_t)uStack_30;
      iVar5 = iVar8;
      if (iVar8 == 6)
      {
        return;
      }
    }
    iVar5 = iVar5 + 1;
  } while (iVar5 != 6);
  return;
}

#endif


#ifndef SHANDALAR
/* ManalinkEh 2022: 0x267fdc0; original functions/functions.c:6289. */
void replace_all_instances_of_one_basic_land_type_word_with_another(int player, int card,
                                                                         int t_player, int t_card)
{
  unsigned int uVar1;
  card_instance_t *instance;
  unsigned int uVar3;
  int iVar4;
  int iVar5;
  int iVar6;
  uint32_t uVar7;
  int iVar8;
  uint32_t uStack_38;
  uint32_t uStack_34;
  unsigned int uStack_30;
  int iStack_2c;
  int options[6];

  if (player == 1)
  {
    options[1] = 0;
    options[2] = 0;
    options[3] = 0;
    options[4] = 0;
    options[5] = 0;
    iVar5 = get_id(t_player, t_card);
    uVar1 = (unsigned int)cards_ptr[iVar5]->hack_colors;
    instance = get_card_instance(t_player, t_card);
    if ((instance->ext->special_flags3 & SF3_HARDCODED_HACK_WORDS_REPLACED) == SF3_0)
    {
      set_special_flags3(t_player, t_card, 0x4000);
      iVar5 = get_id(t_player, t_card);
      uVar1 = get_colors_to_hack_from_type_text(iVar5);
      uVar3 = get_colors_to_hack_from_rules_text(iVar5);
      uVar1 = uVar1 | uVar3;
      iVar5 = get_id(t_player, t_card);
      cards_ptr[iVar5]->hack_colors = (uint8_t)uVar1;
    }
    iVar5 = 1;
    do
    {
      while ((((int)uVar1 >> ((unsigned char)iVar5 & 0x1f) & 1U) != 0 &&
              (instance = get_card_instance(t_player, t_card), instance->hack_mode[iVar5] == '\0')))
      {
        options[iVar5] = 1;
        iVar5 = iVar5 + 1;
        if (iVar5 == 6)
          goto LAB_02680190;
      }
      iVar5 = iVar5 + 1;
    } while (iVar5 != 6);
  LAB_02680190:
    iVar5 = 1;
    do
    {
      while (instance = get_card_instance(t_player, t_card), instance->hack_mode[iVar5] == '\0')
      {
        iVar5 = iVar5 + 1;
        if (iVar5 == 6)
          goto LAB_026801d9;
      }
      instance = get_card_instance(t_player, t_card);
      iVar8 = iVar5 + 1;
      options[instance->hack_mode[iVar5]] = 1;
      iVar5 = iVar8;
    } while (iVar8 != 6);
  LAB_026801d9:
    iStack_2c = options[3];
    iVar5 = options[1];
    iVar8 = options[4];
    iVar4 = options[2];
    iVar6 = options[5];
  }
  else
  {
    iStack_2c = 1;
    iVar5 = 1;
    iVar8 = 1;
    iVar4 = 1;
    iVar6 = 1;
  }
  uVar1 = dialog_impl(player, card, EVENT_RESOLVE_SPELL, "\001\014", "\001\003", "\001\004",
                      "From Swamp...", iVar5, 1, "From Island...", iVar4, 1, "From Forest...",
                      iStack_2c, 1, "From Mountain...", iVar8, 1, "From Plain...", iVar6, 1, 0, 0,
                      0, 0);
  options[1] = 0;
  options[2] = 0;
  options[3] = 0;
  options[4] = 0;
  options[5] = 0;
  options[uVar1] = 1;
  if ((uVar1 == 4) || (uVar1 == 1))
  {
    uStack_34 = 10;
  }
  else
  {
    uStack_34 = 1;
  }
  if ((uVar1 == 2) || (uVar1 == 5))
  {
    uStack_38 = 10;
  }
  else
  {
    uStack_38 = 1;
  }
  if (uVar1 - 1 < 2)
  {
    uVar7 = 10;
    iVar5 = 1;
  }
  else
  {
    uVar7 = 1;
    iVar5 = (-(unsigned int)(uVar1 - 3 < 2) & 9) + 1;
  }
  uStack_30 = (unsigned int)(options[3] == 0);
  iVar5 = dialog_impl(player, card, EVENT_RESOLVE_SPELL, "\001\014", "\001\002", "\001\003",
                      "\001\004", "...to Swamp", (unsigned int)(options[1] == 0),
                      (-(unsigned int)((uVar1 - 3 & 0xfffffffd) == 0) & 9) + 1, "...to Island",
                      (unsigned int)(options[2] == 0), iVar5, "...to Forest", uStack_30, uVar7,
                      "...to Mountain", (unsigned int)(options[4] == 0), uStack_38, "...to Plain",
                      (unsigned int)(options[5] == 0), uStack_34, 0, 0, 0, 0);
  instance = get_card_instance(t_player, t_card);
  uStack_30 = (unsigned int)iVar5;
  if (instance->hack_mode[uVar1] == '\0')
  {
    instance = get_card_instance(t_player, t_card);
    instance->hack_mode[uVar1] = (uint8_t)uStack_30;
  }
  iVar5 = 1;
  do
  {
    while (instance = get_card_instance(t_player, t_card), instance->hack_mode[iVar5] == uVar1)
    {
      instance = get_card_instance(t_player, t_card);
      iVar8 = iVar5 + 1;
      instance->hack_mode[iVar5] = (uint8_t)uStack_30;
      iVar5 = iVar8;
      if (iVar8 == 6)
      {
        return;
      }
    }
    iVar5 = iVar5 + 1;
  } while (iVar5 != 6);
  return;
}

#endif


#ifndef SHANDALAR
/* ManalinkEh 2022: 0x268ecf0; original functions/functions.c:13906.
 * Built-in chooser UI is stubbed; see functions/choose_a_card.cpp. */
int new_card_from_list(int player, int mode, test_definition_t *test)
{
  uint16_t uVar1;
  int result;
  color_test_t colors;
  type_t types;

  if (ai_is_speculating == 1)
  {
    result = -1;
  }
  else
  {
    types = test->type;
    if (types == TYPE_ANY)
    {
      types = TARGET_TYPE_PLANESWALKER | TYPE_ANY;
    }
    uVar1 = test->type_flag;
    if (uVar1 == 4)
    {
      types = types & ~TARGET_TYPE_PLANESWALKER;
    }
    else if ((unsigned short)uVar1 < 5)
    {
      if (uVar1 == 1)
      {
        types = types ^ ~TYPE_NONE;
      }
      else if (uVar1 == 2)
      {
        types = types & ~TYPE_CREATURE;
      }
    }
    else if (uVar1 == 9)
    {
      types = types & ~TYPE_ARTIFACT | TYPE_CREATURE;
    }
    colors = (test->color_flag != 2) - COLOR_TEST_BLACK;
    if ((mode & 1U) == 0)
    {
      result = choose_a_card_test_ct_allow_cancel(test->message, test, colors, types);
    }
    else
    {
      result = choose_a_card_test_ct(test->message, test, colors, types);
    }
    if ((mode & 6U) == 4)
    {
      result = get_internal_card_id_from_csv_id(result);
      return result;
    }
  }
  return result;
}

#endif


#ifndef SHANDALAR
/* Recovered October 2022 haste/delayed-removal family. */
static int haste_and_kill_at_eot(int player, int card, event_t event, int kill_mode);
int haste_and_remove_eot(int player, int card, event_t event);
int haste_and_sacrifice_eot(int player, int card, event_t event);
static int generic_haste_and_kill_at_eot_legacy(int player, int card, event_t event);
int haste_and_remove_at_beginning_of_next_end_step(int player, int card, int t_player,
                                                        int t_card, int kill_mode);
void haste_and_sacrifice_token_at_end_of_next_turn(token_generation_t *token, int beast,
                                                        int number);
void haste_and_remove_token_at_end_of_next_turn(token_generation_t *token, int beast,
                                                     int number);

/* ManalinkEh 2022: 0x2678910; original functions/functions.c:18504. */
static int haste_and_kill_at_eot(int player, int card, event_t event, int kill_mode)
{
  int card_00;
  int result;
  card_instance_t *pcVar2;
  card_instance_t *pcVar3;
  int player_00;
  pcVar2 = get_card_instance(player, card);
  if (-1 < pcVar2->damage_target_player)
  {
    pcVar2 = get_card_instance(player, card);
    player_00 = (int)pcVar2->damage_target_player;
    card_00 = pcVar2->damage_target_card;
    haste(player_00, card_00, event);
    if ((((trigger_condition == TRIGGER_EOT) && (card == affected_card)) &&
         (player == affected_card_controller)) &&
        (player == reason_for_trigger_controller))
    {
      result = pcVar2->targets[1].player;
      if (result < 1)
      {
        if (event == EVENT_TRIGGER)
        {
          event_result = event_result | 2;
        }
        else if (event == EVENT_RESOLVE_TRIGGER)
        {
          pcVar3 = get_card_instance(player_00, card_00);
          if (((pcVar3->state & STATE_OUBLIETTED) == 0) && (kill_mode != 0))
          {
            if (kill_mode != 4)
            {
              if (4 < kill_mode)
              {
                if (kill_mode != 5)
                {
                  return 0;
                }
                bounce_permanent(player_00, card_00);
                return 0;
              }
              if (2 < kill_mode)
              {
                if (kill_mode != 3)
                {
                  return 0;
                }
                if (player_00 != pcVar2->targets[0].player)
                {
                  return 0;
                }
                kill_card(player_00, card_00, KILL_SACRIFICE);
                return 0;
              }
              if (kill_mode < 1)
              {
                return 0;
              }
            }
            kill_card(player_00, card_00, kill_mode);
          }
          else
          {
            kill_card(player, card, KILL_EXILE);
          }
        }
      }
      else
      {
        pcVar2->targets[1].player = result + -1;
      }
    }
  }
  return 0;
}


/* ManalinkEh 2022: 0x2678a80; original functions/functions.c:18556. */
int haste_and_remove_eot(int player, int card, event_t event)
{
  int result;
  result = haste_and_kill_at_eot(player, card, event, 4);
  return result;
}


/* ManalinkEh 2022: 0x2678aa0; original functions/functions.c:18560. */
int haste_and_sacrifice_eot(int player, int card, event_t event)
{
  int result;
  result = haste_and_kill_at_eot(player, card, event, 3);
  return result;
}


/* ManalinkEh 2022: 0x2678ac0; original functions/functions.c:18552. */
static int generic_haste_and_kill_at_eot_legacy(int player, int card, event_t event)
{
  card_instance_t *instance;
  int result;
  instance = get_card_instance(player, card);
  result = haste_and_kill_at_eot(player, card, event, instance->targets[1].card);
  return result;
}


/* ManalinkEh 2022: 0x26a0080; original functions/functions.c:18564. */
int haste_and_remove_at_beginning_of_next_end_step(int player, int card, int t_player,
                                                        int t_card, int kill_mode)
{
  int result;
  int card_00;
  card_instance_t *instance;

  if (kill_mode == 4)
  {
    card_00 =
        create_targetted_legacy_effect(player, card, haste_and_remove_eot, t_player, t_card);
    result = current_phase;
  joined_r0x026a0176:
    if ((result == 0x1f) && (-1 < card_00))
    {
      instance = get_card_instance(player, card_00);
      instance->targets[1].player = 1;
    }
    return card_00;
  }
  if (kill_mode < 5)
  {
    if (2 < kill_mode)
    {
      card_00 = create_targetted_legacy_effect(player, card, haste_and_sacrifice_eot, t_player,
                                               t_card);
      instance = get_card_instance(player, card_00);
      result = current_phase;
      instance->number_of_targets = '\0';
      goto joined_r0x026a0176;
    }
    if (kill_mode < 1)
    {
      return -1;
    }
  }
  else if (kill_mode != 5)
  {
    return -1;
  }
  create_targetted_legacy_effect(player, card, generic_haste_and_kill_at_eot_legacy, t_player,
                                 t_card);
  instance = get_card_instance(player, -1);
  instance->targets[1].card = kill_mode;
  return -1;
}


/* ManalinkEh 2022: 0x26a01a0; original functions/functions.c:18598. */
void haste_and_sacrifice_token_at_end_of_next_turn(token_generation_t *token, int beast,
                                                        int number)
{
  haste_and_remove_at_beginning_of_next_end_step(token->s_player, token->s_card,
                                                      token->t_player, beast, 3);
  return;
}


/* ManalinkEh 2022: 0x26a01e0; original functions/functions.c:18603. */
void haste_and_remove_token_at_end_of_next_turn(token_generation_t *token, int beast,
                                                     int number)
{
  haste_and_remove_at_beginning_of_next_end_step(token->s_player, token->s_card,
                                                      token->t_player, beast, 4);
  return;
}

#endif


#ifndef SHANDALAR
/* Recovered October 2022 reanimation family. */
static int new_reanimate_all_impl(int player, int card, int targ_player,
                                test_definition_t *this_test, reanimate_mode_t action);
int new_reanimate_all(int player, int card, int targ_player, test_definition_t *this_test,
                           reanimate_mode_t action);
static int reanimate_permanent_impl(int player, int card, int target_graveyard, int selected,
                                  reanimate_mode_t action, counter_t counter_type,
                                  CardFunction effect_fn, ReanimationBeforeEtb before_etb);
int reanimate_permanent(int player, int card, int target_graveyard, int selected,
                             reanimate_mode_t action);
int reanimate_permanent_with_effect(int player, int card, int target_graveyard, int selected,
                                         reanimate_mode_t action, CardFunction effect_fn);
int reanimate_permanent_with_counter_and_effect(int player, int card, int target_graveyard,
                                                     int selected, reanimate_mode_t action,
                                                     counter_t counter_type, CardFunction effect_fn);
int reanimate_permanent_with_function(int player, int card, int target_graveyard, int selected,
                                           reanimate_mode_t action,
                                           ReanimationBeforeEtb before_etb);
void reanimate_all(int player, int card, int targ_player, int type, int flag1, int subtype,
                        int flag2, int color, int flag3, int id, int flag4, int cc, int flag5,
                        reanimate_mode_t action);
static int haste_and_exile_your_upkeep(int player, int card, event_t event);
int effect_unearth(int player, int card, event_t event);
void ereboss_titan_effect(int player);
int upkeep_trigger_mode_impl(int player, int card, event_t event,
                                  resolve_trigger_t trigger_mode, int no_check);

/* ManalinkEh 2022: 0x269ba70; original functions/functions.c:5286. */
static int new_reanimate_all_impl(int player, int card, int targ_player,
                                test_definition_t *this_test, reanimate_mode_t action)
{
  int iVar1;
  reanimate_mode_t rVar2;
  int iVar3;
  card_instance_t *instance;
  int iVar5;
  int *piVar6;
  int *piVar7;
  int iVar8;
  unsigned int uVar9;
  int *piVar10;
  int iVar11;
  unsigned int player_00;
  int iStack_2060;
  int iStack_205c;
  int *piStack_2058;
  int iStack_2054;
  int iStack_2050;
  int iStack_204c;
  int iStack_2048;
  unsigned int uStack_2044;
  unsigned int uStack_2040;
  int trc[2];
  int ac[2];
  test_definition_t this_test2;
  int to_reanimate[1000];
  int auras[1000];

  iStack_2054 = -1;
  if (card != -1)
  {
    iStack_2054 = get_id(player, card);
  }
  iVar1 = grafdiggers_cages(player, TYPE_CREATURE, TUTOR_FROM_GRAVE);
  rVar2 = action & REANIMATE_SPECIAL_LIVING_DEAD;
  trc[0] = 0;
  trc[1] = 0;
  ac[0] = 0;
  ac[1] = 0;
  iStack_205c = 0;
  iStack_2060 = -1;
  iVar11 = 0;
  do
  {
    if ((targ_player == iVar11) || (targ_player == 2))
    {
      if ((iStack_2054 == 0x16a1) && (targ_player == iVar11))
      {
        instance = get_card_instance(player, card);
        uStack_2040 = instance->info_slot;
      }
      else
      {
        uStack_2040 = -1;
      }
      piVar10 = graveyard_ptr_mutable[iVar11];
      iVar3 = count_graveyard(iVar11);
      iVar3 = iVar3 + -1;
      if ((int)uStack_2040 < iVar3)
      {
        do
        {
          iVar5 = is_what(-1, piVar10[iVar3], 0x47);
          if ((iVar5 != 0) &&
              (iVar5 = new_make_test(iVar11, piVar10[iVar3], -1, this_test), iVar5 != 0))
          {
            iVar5 = piVar10[iVar3];
            if (targ_player != 2)
            {
              iVar5 = is_what(-1, iVar5, 2);
              if (iVar5 != 0)
              {
                iStack_2060 = 1 - iVar11;
              }
              iVar5 = piVar10[iVar3];
            }
            iVar5 = is_what(-1, iVar5, 2);
            if ((iVar5 == 0) ||
                ((iVar5 = is_what(-1, piVar10[iVar3], 2), iVar1 == 0 && (iVar5 != 0))))
            {
              iVar5 = has_subtype_by_id(player, (unsigned int)cards_data[piVar10[iVar3]].id,
                                             SUBTYPE_AURA);
              if (iVar5 == 0)
              {
                iVar5 = trc[iVar11];
                if (iVar5 < 500)
                {
                  to_reanimate[(iVar11) * 500 + (iVar5)] = piVar10[iVar3];
                  trc[iVar11] = iVar5 + 1;
                joined_r0x0269c7ff:
                  if (rVar2 == REANIMATE_DEFAULT)
                  {
                    remove_card_from_grave(iVar11, iVar3);
                  }
                  else
                  {
                    rfg_card_from_grave(iVar11, iVar3);
                  }
                  iStack_205c = iStack_205c + 1;
                }
              }
              else
              {
                iVar5 = add_card_to_hand(player, piVar10[iVar3]);
                instance = get_card_instance(player, iVar5);
                instance->state = instance->state | 0x20;
                hand_count[player] = hand_count[player] + -1;
                iVar8 = call_card_function_i(instance, player, iVar5, EVENT_CAN_CAST);
                obliterate_card_and_recycle(player, iVar5);
                if (iVar8 != 0)
                {
                  iVar5 = ac[iVar11];
                  if (iVar5 < 500)
                  {
                    auras[(iVar11) * 500 + (iVar5)] = piVar10[iVar3];
                    ac[iVar11] = iVar5 + 1;
                    goto joined_r0x0269c7ff;
                  }
                }
              }
            }
          }
          iVar3 = iVar3 + -1;
        } while (iVar3 != uStack_2040);
      }
    }
    iVar11 = iVar11 + 1;
  } while (iVar11 != 2);
  if (rVar2 != REANIMATE_DEFAULT)
  {
    default_test_definition(&this_test2, 2);
    new_manipulate_all(player, card, 2, &this_test2, ACT_KILL_SACRIFICE);
    piStack_2058 = ac;
    iStack_2050 = 0;
    do
    {
      iVar1 = *piStack_2058;
      if (0 < iVar1)
      {
        piVar10 = (auras + (0) * 500) + iStack_2050;
        iStack_2054 = iVar1;
        if (500 < iVar1)
        {
          iStack_2054 = 500;
        }
        iVar11 = 0;
      LAB_0269c5e0:
        do
        {
          iVar3 = add_card_to_hand(player, *piVar10);
          if (-1 < iVar3)
          {
            instance = get_card_instance(player, iVar3);
            uVar9 = instance->state;
            hand_count[player] = hand_count[player] + -1;
            instance->state = uVar9 | 0x20;
            iVar5 = call_card_function_i(instance, player, iVar3, EVENT_CAN_CAST);
            obliterate_card_and_recycle(player, iVar3);
            if ((iVar5 == 0) && (piVar6 = piVar10, iVar11 < iStack_2054))
            {
              do
              {
                piVar7 = piVar6 + 1;
                *piVar6 = piVar6[1];
                piVar6 = piVar7;
              } while (piVar7 != (auras + (0) * 500) + iStack_2054 + iStack_2050);
              iVar11 = iVar11 + 1;
              piVar10 = piVar10 + 1;
              if (iVar11 == iVar1)
                break;
              goto LAB_0269c5e0;
            }
          }
          iVar11 = iVar11 + 1;
          piVar10 = piVar10 + 1;
        } while (iVar11 != iVar1);
      }
      piStack_2058 = piStack_2058 + 1;
      iStack_2050 = iStack_2050 + 500;
    } while (iStack_2050 != 1000);
  }
  if ((action & REANIMATE_ALL_UNDER_CASTERS_CONTROL) == REANIMATE_DEFAULT)
  {
    iVar1 = 0;
    iStack_204c = 0;
    do
    {
      iVar11 = ac[iVar1];
      if (0 < iVar11)
      {
        piVar10 = (auras + (0) * 500) + iStack_204c;
        do
        {
          iVar3 = *piVar10;
          if (rVar2 == REANIMATE_DEFAULT)
          {
          LAB_0269be48:
            iVar3 = add_card_to_hand_invisible(iVar1, iVar3);
            *piVar10 = iVar3;
          }
          else
          {
            iVar5 = check_rfg(iVar1, (unsigned int)cards_data[iVar3].id);
            if (iVar5 != 0)
            {
              remove_card_from_rfg(iVar1, (unsigned int)cards_data[iVar3].id);
              goto LAB_0269be48;
            }
          }
          piVar10 = piVar10 + 1;
        } while (piVar10 != (auras + (0) * 500) + iVar11 + iStack_204c);
      }
      iVar11 = trc[iVar1];
      if (0 < iVar11)
      {
        piVar10 = (to_reanimate + (0) * 500) + iStack_204c;
        do
        {
          iVar3 = *piVar10;
          if (rVar2 == REANIMATE_DEFAULT)
          {
          LAB_0269bef0:
            iVar3 = add_card_to_hand_invisible(iVar1, iVar3);
            *piVar10 = iVar3;
          }
          else
          {
            iVar5 = check_rfg(iVar1, (unsigned int)cards_data[iVar3].id);
            if (iVar5 != 0)
            {
              remove_card_from_rfg(iVar1, (unsigned int)cards_data[iVar3].id);
              goto LAB_0269bef0;
            }
          }
          piVar10 = piVar10 + 1;
        } while (piVar10 != (to_reanimate + (0) * 500) + iVar11 + iStack_204c);
      }
      iVar1 = iVar1 + 1;
      iStack_204c = iStack_204c + 500;
    } while (iVar1 != 2);
    uStack_2044 = 0;
    uVar9 = current_turn;
    do
    {
      player_00 = uStack_2044 ^ uVar9;
      iVar1 = ac[player_00];
      if (0 < iVar1)
      {
        piVar10 = (auras + (player_00) * 500);
        do
        {
          iVar11 = *piVar10;
          piVar10 = piVar10 + 1;
          reanimate_permanent_impl(player_00, -1, player_00, iVar11,
                                        action | REANIMATE_SPECIAL_R_ALL, COUNTER_invalid,
                                        (CardFunction)0x0, (ReanimationBeforeEtb)0x0);
          uVar9 = current_turn;
        } while (piVar10 != (auras + (player_00) * 500) + iVar1);
      }
      uStack_2044 = uStack_2044 + 1;
    } while (uStack_2044 != 2);
    uStack_2040 = 0;
    do
    {
      uVar9 = uVar9 ^ uStack_2040;
      iVar1 = trc[uVar9];
      if (0 < iVar1)
      {
        piVar10 = (to_reanimate + (uVar9) * 500);
        do
        {
          iVar11 = *piVar10;
          piVar10 = piVar10 + 1;
          reanimate_permanent_impl(uVar9, -1, uVar9, iVar11,
                                        action | REANIMATE_SPECIAL_R_ALL, COUNTER_invalid,
                                        (CardFunction)0x0, (ReanimationBeforeEtb)0x0);
        } while ((to_reanimate + (uVar9) * 500) + iVar1 != piVar10);
      }
      uStack_2040 = uStack_2040 + 1;
      uVar9 = current_turn;
    } while (uStack_2040 != 2);
  }
  else
  {
    iStack_2048 = ac[player];
    if (0 < iStack_2048)
    {
      piVar10 = (auras + (player) * 500);
      do
      {
        iVar1 = *piVar10;
        if (rVar2 == REANIMATE_DEFAULT)
        {
        LAB_0269bbc8:
          iVar1 = add_card_to_hand(player, iVar1);
          *piVar10 = iVar1;
        }
        else
        {
          iVar11 = check_rfg(player, (unsigned int)cards_data[iVar1].id);
          if (iVar11 != 0)
          {
            remove_card_from_rfg(player, (unsigned int)cards_data[iVar1].id);
            goto LAB_0269bbc8;
          }
        }
        piVar10 = piVar10 + 1;
      } while ((auras + (player) * 500) + iStack_2048 != piVar10);
    }
    uStack_2040 = trc[player];
    if (0 < (int)uStack_2040)
    {
      piVar10 = (to_reanimate + (player) * 500);
      do
      {
        iVar1 = *piVar10;
        if (rVar2 == REANIMATE_DEFAULT)
        {
        LAB_0269c118:
          iVar1 = add_card_to_hand(player, iVar1);
          *piVar10 = iVar1;
        }
        else
        {
          iVar11 = check_rfg(player, (unsigned int)cards_data[iVar1].id);
          if (iVar11 != 0)
          {
            remove_card_from_rfg(player, (unsigned int)cards_data[iVar1].id);
            goto LAB_0269c118;
          }
        }
        piVar10 = piVar10 + 1;
      } while ((to_reanimate + (player) * 500) + uStack_2040 != piVar10);
    }
    iVar11 = 1 - player;
    iVar1 = ac[iVar11];
    if (0 < iVar1)
    {
      piVar10 = (auras + (iVar11) * 500);
      do
      {
        iVar3 = *piVar10;
        if (rVar2 == REANIMATE_DEFAULT)
        {
        LAB_0269c368:
          iVar3 = add_card_to_hand_invisible(player, iVar3);
          instance = get_card_instance(player, iVar3);
          iStack_2048 = iStack_2048 + 1;
          instance->state = instance->state ^ 0x1000;
          auras[(player) * 500 + (uStack_2040)] = iVar3;
        }
        else
        {
          iVar5 = check_rfg(iVar11, (unsigned int)cards_data[iVar3].id);
          if (iVar5 != 0)
          {
            remove_card_from_rfg(player, (unsigned int)cards_data[iVar3].id);
            goto LAB_0269c368;
          }
        }
        piVar10 = piVar10 + 1;
      } while ((auras + (iVar11) * 500) + iVar1 != piVar10);
    }
    iVar1 = trc[iVar11];
    if (0 < iVar1)
    {
      piVar10 = (to_reanimate + (iVar11) * 500);
      do
      {
        iVar3 = *piVar10;
        if (rVar2 == REANIMATE_DEFAULT)
        {
        LAB_0269c468:
          iVar3 = add_card_to_hand_invisible(player, iVar3);
          instance = get_card_instance(player, iVar3);
          instance->state = instance->state ^ 0x1000;
          to_reanimate[(player) * 500 + (uStack_2040)] = iVar3;
          uStack_2040 = uStack_2040 + 1;
        }
        else
        {
          iVar5 = check_rfg(iVar11, (unsigned int)cards_data[iVar3].id);
          if (iVar5 != 0)
          {
            remove_card_from_rfg(player, (unsigned int)cards_data[iVar3].id);
            goto LAB_0269c468;
          }
        }
        piVar10 = piVar10 + 1;
      } while ((to_reanimate + (iVar11) * 500) + iVar1 != piVar10);
    }
    ac[player] = iStack_2048;
    trc[player] = uStack_2040;
    ac[iVar11] = 0;
    trc[iVar11] = 0;
    iVar1 = ac[player];
    if (0 < iVar1)
    {
      piVar10 = (auras + (player) * 500);
      do
      {
        iVar11 = *piVar10;
        piVar10 = piVar10 + 1;
        instance = get_card_instance(player, iVar11);
        instance->state = instance->state & 0xffffffdf;
        reanimate_permanent_impl(player, card, player, iVar11,
                                      action | REANIMATE_SPECIAL_R_ALL, COUNTER_invalid,
                                      (CardFunction)0x0, (ReanimationBeforeEtb)0x0);
      } while (piVar10 != (auras + (player) * 500) + iVar1);
    }
    iVar1 = trc[player];
    if (0 < iVar1)
    {
      iVar11 = player * 2000;
      do
      {
        instance = get_card_instance(player, *(int *)((int)(auras + (0) * 500) + iVar11));
        instance->state = instance->state & 0xffffffdf;
        piVar10 = (int *)((int)(to_reanimate + (0) * 500) + iVar11);
        iVar11 = iVar11 + 4;
        reanimate_permanent_impl(player, card, player, *piVar10,
                                      action | REANIMATE_SPECIAL_R_ALL, COUNTER_invalid,
                                      (CardFunction)0x0, (ReanimationBeforeEtb)0x0);
      } while ((iVar1 + player * 500) * 4 != iVar11);
    }
  }
  if (-1 < iStack_2060)
  {
    ereboss_titan_effect(iStack_2060);
  }
  return iStack_205c;
}


/* ManalinkEh 2022: 0x269c810; original functions/functions.c:5532. */
int new_reanimate_all(int player, int card, int targ_player, test_definition_t *this_test,
                           reanimate_mode_t action)
{
  int result;
  result = new_reanimate_all_impl(player, card, targ_player, this_test, action);
  return result;
}


/* ManalinkEh 2022: 0x2695730; original functions/functions.c:5108. */
static int reanimate_permanent_impl(int player, int card, int target_graveyard, int selected,
                                  reanimate_mode_t action, counter_t counter_type,
                                  CardFunction effect_fn, ReanimationBeforeEtb before_etb)
{
  int *piVar1;
  int player_00;
  reanimate_mode_t rVar2;
  int t_card;
  int result;
  card_instance_t *instance;
  reanimate_mode_t rStack_24;
  int iStack_1c;

  if (selected == -1)
  {
    return -1;
  }
  rVar2 = (action ^ REANIMATEXTRA_RETURN_TO_HAND2) >> 0x1f & action;
  piVar1 = graveyard_ptr_mutable[target_graveyard];
  player_00 = player;
  if ((rVar2 & REANIMATE_UNDER_OWNER_CONTROL) != REANIMATE_DEFAULT)
  {
    player_00 = target_graveyard;
  }
  t_card = selected;
  if ((rVar2 & REANIMATE_SPECIAL_R_ALL) == REANIMATE_DEFAULT)
  {
    t_card = add_card_to_hand(player_00, piVar1[selected]);
    remove_card_from_grave(target_graveyard, selected);
  }
  if ((((rVar2 & REANIMATE_RETURN_TO_HAND) == REANIMATE_DEFAULT) &&
       (result = is_what(-1, piVar1[selected], 2), result != 0)) &&
      (result = grafdiggers_cages(player, TYPE_CREATURE, TUTOR_FROM_GRAVE), result != 0))
  {
    return -1;
  }
  rStack_24 = rVar2 & REANIMATE_RETURN_TO_HAND;
  iStack_1c = card;
  if ((rVar2 & REANIMATE_ATTACH_AS_AURA) == REANIMATE_DEFAULT)
  {
    if ((((rVar2 & REANIMATE_ATTACH_AURA_TO_REANIMATOR) == REANIMATE_DEFAULT) ||
         (player < 0)) ||
        (card < 0))
      goto LAB_026957b7;
  }
  else
  {
    if ((player < 0) || (card < 0))
    {
    LAB_026957b7:
      iStack_1c = card;
      if (card < 0)
      {
        iStack_1c = t_card;
      }
      goto LAB_026957c4;
    }
    set_special_flags(player, card, 0x20000000);
    attach_aura_to_target(player, card, EVENT_RESOLVE_SPELL, player_00, t_card);
    instance = get_card_instance(player, card);
    instance->targets[0].player = player_00;
    instance->damage_target_player = (int8_t)player_00;
    instance->targets[0].card = t_card;
    instance->damage_target_card = t_card;
    if ((rVar2 & REANIMATE_ATTACH_AURA_TO_REANIMATOR) == REANIMATE_DEFAULT)
      goto LAB_026957c4;
  }
  set_special_flags(player_00, t_card, 0x20000000);
  attach_aura_to_target(player_00, t_card, EVENT_RESOLVE_SPELL, player, card);
  instance = get_card_instance(player_00, t_card);
  instance->damage_target_player = (int8_t)player;
  instance->targets[0].player = player;
  instance->targets[0].card = card;
  instance->damage_target_card = card;
LAB_026957c4:
  if (counter_type != COUNTER_invalid)
  {
    hack_silent_counters = hack_silent_counters + 1;
    add_counters(player, card, player_00, t_card, counter_type, 1);
    hack_silent_counters = hack_silent_counters + -1;
  }
  if ((rVar2 & REANIMATE_TAP) != REANIMATE_DEFAULT)
  {
    instance = get_card_instance(player_00, t_card);
    instance->state = instance->state | 0x10;
  }
  if ((rVar2 & REANIMATE_PLUS1_PLUS1_COUNTER) != REANIMATE_DEFAULT)
  {
    add_counters(player, card, player_00, t_card, COUNTER_P1_P1, 1);
  }
  if ((rVar2 & REANIMATE_MINUS1_MINUS1_COUNTER) != REANIMATE_DEFAULT)
  {
    add_counters(player, card, player_00, t_card, COUNTER_M1_M1, 1);
  }
  if (effect_fn != (CardFunction)0x0)
  {
    create_targetted_legacy_effect(player, iStack_1c, effect_fn, player_00, t_card);
  }
  if (action == REANIMATE_UNEARTH)
  {
    convert_to_token(player_00, t_card);
    set_special_flags(player_00, t_card, 0x4000);
    set_special_flags2(player_00, t_card, 0x10000000);
  }
  else
  {
    if ((rVar2 & REANIMATE_ADD_BLACK_ZOMBIE) != REANIMATE_DEFAULT)
    {
      change_color(player, iStack_1c, player_00, t_card, COLOR_TEST_BLACK,
                   CHANGE_COLOR_NO_SOUND | CHANGE_COLOR_ADD);
      add_a_subtype_permanently(player, iStack_1c, player_00, t_card, SUBTYPE_ZOMBIE);
    }
    if ((rVar2 & REANIMATE_HASTE_AND_EXILE_AT_EOT) != REANIMATE_DEFAULT)
    {
      create_targetted_legacy_effect(player, iStack_1c, haste_and_remove_eot, player_00,
                                     t_card);
    }
  }
  if ((rVar2 & REANIMATE_HASTE_AND_EXILE_AT_YOUR_UPKEEP) != REANIMATE_DEFAULT)
  {
    create_targetted_legacy_effect(player, iStack_1c, haste_and_exile_your_upkeep, player_00,
                                   t_card);
  }
  if ((rVar2 & REANIMATE_HASTE_UNTIL_EOT) != REANIMATE_DEFAULT)
  {
    pump_ability_until_eot(player, iStack_1c, player_00, t_card, 0, 0, 0, 4);
  }
  if (player_00 != target_graveyard)
  {
    if (player == 0)
    {
      add_state(player_00, t_card, 0x1000);
    }
    else
    {
      remove_state(player_00, t_card, 0x1000);
    }
  }
  if (before_etb != (ReanimationBeforeEtb)0x0)
  {
    (*before_etb)(player, card, player_00, t_card);
  }
  if ((((rVar2 & (REANIMATE_RETURN_TO_HAND | REANIMATE_ATTACH_AS_AURA |
                  REANIMATE_HASTE_AND_EXILE_AT_EOT | REANIMATE_ADD_BLACK_ZOMBIE |
                  REANIMATE_UNEARTH | REANIMATE_NO_CONTROL_LEGACY)) ==
            REANIMATE_DEFAULT &&
        effect_fn == (CardFunction)0x0) &&
       (iStack_1c != -1)) &&
      ((target_graveyard != player ||
        (instance = get_card_instance(player_00, t_card), player != (instance->state >> 0xc & 1)))))
  {
    create_targetted_legacy_effect(player, iStack_1c, empty, player_00, t_card);
  }
  if (rStack_24 == REANIMATE_DEFAULT)
  {
    set_special_flags3(player_00, t_card, 1);
    if ((rVar2 & REANIMATE_RETURN_TO_PLAY_TRANSFORMED) == REANIMATE_DEFAULT)
    {
      put_into_play(player_00, t_card);
    }
    else
    {
      put_into_play_transformed(player_00, t_card);
    }
    if (action == REANIMATE_UNEARTH)
    {
      instance = in_play(player_00, t_card);
      if (instance != NULL)
      {
        create_targetted_legacy_effect(player, iStack_1c, effect_unearth, player_00, t_card);
        return t_card;
      }
      return t_card;
    }
    return t_card;
  }
  return t_card;
}


/* ManalinkEh 2022: 0x269b760; original functions/functions.c:5249. */
int reanimate_permanent(int player, int card, int target_graveyard, int selected,
                             reanimate_mode_t action)
{
  unsigned int card_00;
  int result;

  card_00 = reanimate_permanent_impl(player, card, target_graveyard, selected, action,
                                          COUNTER_invalid, (CardFunction)0x0,
                                          (ReanimationBeforeEtb)0x0);
  if ((card_00 != 0xffffffff) && ((card_00 & 0x800) == 0))
  {
    result = is_what(player, card_00, 2);
    if (result != 0)
    {
      ereboss_titan_effect(1 - player);
    }
  }
  return card_00;
}


/* ManalinkEh 2022: 0x269b8f0; original functions/functions.c:5258. */
int reanimate_permanent_with_effect(int player, int card, int target_graveyard, int selected,
                                         reanimate_mode_t action, CardFunction effect_fn)
{
  unsigned int card_00;
  int result;

  card_00 =
      reanimate_permanent_impl(player, card, target_graveyard, selected, action,
                                    COUNTER_invalid, effect_fn, (ReanimationBeforeEtb)0x0);
  if ((card_00 != 0xffffffff) && ((card_00 & 0x800) == 0))
  {
    result = is_what(player, card_00, 2);
    if (result != 0)
    {
      ereboss_titan_effect(1 - player);
    }
  }
  return card_00;
}


/* ManalinkEh 2022: 0x269b970; original functions/functions.c:5267. */
int reanimate_permanent_with_counter_and_effect(int player, int card, int target_graveyard,
                                                     int selected, reanimate_mode_t action,
                                                     counter_t counter_type, CardFunction effect_fn)
{
  unsigned int card_00;
  int result;

  card_00 = reanimate_permanent_impl(player, card, target_graveyard, selected, action,
                                          counter_type, effect_fn, (ReanimationBeforeEtb)0x0);
  if ((card_00 != 0xffffffff) && ((card_00 & 0x800) == 0))
  {
    result = is_what(player, card_00, 2);
    if (result != 0)
    {
      ereboss_titan_effect(1 - player);
    }
  }
  return card_00;
}


/* ManalinkEh 2022: 0x269b9f0; original functions/functions.c:5278. */
int reanimate_permanent_with_function(int player, int card, int target_graveyard, int selected,
                                           reanimate_mode_t action,
                                           ReanimationBeforeEtb before_etb)
{
  unsigned int card_00;
  int result;

  card_00 = reanimate_permanent_impl(player, card, target_graveyard, selected, action,
                                          COUNTER_invalid, (CardFunction)0x0, before_etb);
  if ((card_00 != 0xffffffff) && ((card_00 & 0x800) == 0))
  {
    result = is_what(player, card_00, 2);
    if (result != 0)
    {
      ereboss_titan_effect(1 - player);
    }
  }
  return card_00;
}


/* ManalinkEh 2022: 0x269c830; original functions/functions.c:5536. */
void reanimate_all(int player, int card, int targ_player, int type, int flag1, int subtype,
                        int flag2, int color, int flag3, int id, int flag4, int cc, int flag5,
                        reanimate_mode_t action)
{
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
  new_reanimate_all_impl(player, card, targ_player, &test, action);
  return;
}


/* ManalinkEh 2022: 0x2683db0; original functions/functions.c:5039. */
static int haste_and_exile_your_upkeep(int player, int card, event_t event)
{
  card_instance_t *instance;
  int result;

  instance = get_card_instance(player, card);
  result = (int)instance->damage_target_player;
  if (-1 < instance->damage_target_player)
  {
    haste(result, instance->damage_target_card, event);
    result = (int)instance->damage_target_player;
  }
  if (result == current_turn)
  {
    result = upkeep_trigger_mode_impl(player, card, event, RESOLVE_TRIGGER_MANDATORY, 0);
    if (result != 0)
    {
      kill_card((int)instance->damage_target_player, instance->damage_target_card, KILL_EXILE);
      kill_card(player, card, KILL_EXILE);
    }
  }
  return 0;
}


/* ManalinkEh 2022: 0x26a2d40; original functions/functions.c:5013. */
int effect_unearth(int player, int card, event_t event)
{
  int player_00;
  int card_00;
  card_instance_t *pcVar1;
  int result;
  card_instance_t *pcVar3;

  pcVar1 = get_card_instance(player, card);
  player_00 = pcVar1->targets[0].player;
  card_00 = pcVar1->targets[0].card;
  haste(player_00, card_00, event);
  if (pcVar1->targets[1].player < 0)
  {
    pcVar3 = in_play(player_00, card_00);
    if (pcVar3 != NULL)
    {
      pcVar1->targets[1].player = 0;
      set_special_flags(player_00, card_00, 0x4000);
    }
  }
  leaves_play(player_00, card_00, event, 0, 0);
  if (event == EVENT_LEAVES_PLAY_ABILITY)
  {
    result = get_original_internal_card_id(player_00, card_00);
    pcVar3 = get_card_instance(player_00, card_00);
    add_card_to_rfg(pcVar3->state >> 0xc & 1, result);
  }
  if (pcVar1->targets[1].card != 0x42)
  {
    result = eot_trigger(player, card, event);
    if (result != 0)
    {
      pcVar1->targets[1].card = 0x42;
      pcVar1 = get_card_instance(player_00, card_00);
      if ((pcVar1->state & 0x800000) == 0)
      {
        kill_card(player_00, card_00, KILL_EXILE);
        return 0;
      }
    }
  }
  return 0;
}


/* ManalinkEh 2022: 0x269a810; original functions/functions.c:75. */
void ereboss_titan_effect(int player)
{
  int iVar1;
  int card;
  int iVar2;

  iVar1 = count_graveyard(player);
  iVar1 = iVar1 + -1;
  if (-1 < iVar1)
  {
    do
    {
      while ((cards_data[graveyard_ptr_mutable[player][iVar1]].id != 0x3b12 ||
              (hand_count[player] == 0)))
      {
        iVar1 = iVar1 + -1;
        if (iVar1 == -1)
        {
          return;
        }
      }
      card = add_card_to_hand(player, graveyard_ptr_mutable[player][iVar1]);
      iVar2 = dialog_impl(player, card, EVENT_ACTIVATE, "\001\014", "\001\002",
                          "Return Erebos's Titan to your hand", 1, 10, "Decline", 1, 1, 0, 0, 0, 0);
      if (iVar2 == 1)
      {
        discard(player, 0, player);
        from_grave_to_hand(player, iVar1, 1);
      }
      iVar1 = iVar1 + -1;
      obliterate_card(player, card);
    } while (iVar1 != -1);
  }
  return;
}


/* ManalinkEh 2022: 0x2683c00; original functions/functions.c:2855. */
int upkeep_trigger_mode_impl(int player, int card, event_t event,
                                  resolve_trigger_t trigger_mode, int no_check)
{
  int result;

  if (trigger_condition != 0xc9)
  {
    return 0;
  }
  if (((card == affected_card) && (player == affected_card_controller)) &&
      (reason_for_trigger_controller == player))
  {
    result = is_what(player, card, 0x80);
    if ((result == 0) && (result = is_humiliated(player, card, AL_TRIG), result != 0))
    {
      return 0;
    }
    result = 0;
    if (no_check == 0)
    {
      result = count_upkeeps(current_turn);
      if (result < 1)
      {
        return 0;
      }
      result = 1;
    }
    if (event == EVENT_TRIGGER)
    {
      if (trigger_mode == RESOLVE_TRIGGER_DUH)
      {
        result = duh_mode(player);
        trigger_mode = (result != 0) + RESOLVE_TRIGGER_OPTIONAL;
      }
      event_result = event_result | trigger_mode;
      return 0;
    }
    if (event == EVENT_RESOLVE_TRIGGER)
    {
      if (no_check != 0)
      {
        return 1;
      }
      return result;
    }
  }
  return 0;
}

#endif


#ifndef SHANDALAR
/* Recovered native October 2022 leaves-play test interfaces. */

/* ManalinkEh 2022: 0x26a3790; original functions/functions.c:878. */
int test_leaves_play_mode(int triggering_player, int triggering_card, event_t event,
                               int controller, resolve_trigger_t mode, test_definition_t *test,
                               int can_be_suppressed, int can_be_duplicated)
{
  int iVar1;
  int iVar2;
  int iVar3;
  if ((((trigger_condition == 0xd4) && (triggering_card == affected_card)) &&
       (triggering_player == affected_card_controller)) &&
      (((iVar1 = new_make_test_in_play(trigger_cause_controller, trigger_cause, 0, test),
         iVar1 != 0 && (reason_for_trigger_controller == triggering_player)) &&
        ((trigger_cause_controller == controller || (controller == 2))))))
  {
    iVar1 = is_humiliated(triggering_player, triggering_card, AL_TRIG);
    if (((iVar1 == 0) || (can_be_suppressed == 0)) &&
        ((iVar1 = is_this_dying(trigger_cause_controller, trigger_cause),
          iVar1 == 0 ||
              ((iVar1 = check_for_death_trigger_removal(trigger_cause_controller, trigger_cause),
                can_be_suppressed == 0 || (iVar1 == 0))))))
    {
      if (event == EVENT_TRIGGER)
      {
        if (can_be_suppressed != 0)
        {
          event_result = event_result | mode;
          return 0;
        }
        if (((can_be_duplicated == 0) ||
             (iVar1 = is_what(trigger_cause_controller, trigger_cause, 2), iVar1 == 0)) ||
            (iVar1 = is_this_dying(trigger_cause_controller, trigger_cause), iVar1 == 0))
        {
          iVar1 = 1;
        }
        else
        {
          iVar1 = card_counts[triggering_player][5] + 1;
        }
        do
        {
          iVar3 = is_what(triggering_player, triggering_card, 0x80);
          if (iVar3 == 0)
          {
            call_card_function(triggering_player, triggering_card, EVENT_LEAVES_PLAY_ABILITY);
          }
          else
          {
            dispatch_event_arbitrary_to_one_card(triggering_player, triggering_card,
                                                 EVENT_LEAVES_PLAY_ABILITY,
                                                 trigger_cause_controller, trigger_cause, -1, -1);
          }
          iVar1 = iVar1 + -1;
        } while (iVar1 != 0);
      }
      else
      {
        if ((event == EVENT_RESOLVE_TRIGGER) && (can_be_suppressed != 0))
        {
          if ((can_be_duplicated == 0) ||
              ((iVar1 = is_what(trigger_cause_controller, trigger_cause, 2),
                iVar1 == 0 ||
                    (iVar1 = is_this_dying(trigger_cause_controller, trigger_cause), iVar1 == 0))))
          {
            iVar3 = 1;
            iVar1 = iVar3;
          }
          else
          {
            iVar3 = card_counts[triggering_player][5] + 1;
            iVar1 = iVar3;
          }
          do
          {
            iVar2 = is_what(triggering_player, triggering_card, 0x80);
            if (iVar2 == 0)
            {
              call_card_function(triggering_player, triggering_card, EVENT_LEAVES_PLAY_ABILITY);
            }
            else
            {
              dispatch_event_arbitrary_to_one_card(triggering_player, triggering_card,
                                                   EVENT_LEAVES_PLAY_ABILITY,
                                                   trigger_cause_controller, trigger_cause, -1, -1);
            }
            iVar3 = iVar3 + -1;
          } while (iVar3 != 0);
          return iVar1;
        }
      }
    }
  }
  return 0;
}


/* ManalinkEh 2022: 0x26a3a80; original functions/functions.c:939. */
int test_leaves_play(int triggering_player, int triggering_card, event_t event, int controller,
                          test_definition_t *test, int can_be_suppressed,
                          int can_be_duplicated)
{
  int result;
  result = test_leaves_play_mode(triggering_player, triggering_card, event, controller,
                                      RESOLVE_TRIGGER_MANDATORY, test, can_be_suppressed,
                                      can_be_duplicated);
  return result;
}

#endif


#ifndef SHANDALAR
/* ManalinkEh 2022: 0x2677c50; original functions/functions.c:18225. */
static int fx_permanents_destroyed_this_way_accumulate(int player, int card, event_t event)
{
  unsigned char bVar2;
  int32_t iVar4;
  int card_00;
  card_instance_t *instance;
  card_instance_t *pcVar5;
  card_instance_t *pcVar6;
  unsigned int uVar7;
  int iVar8;
  int iVar9;

  instance = get_card_instance(player, card);
  if (trigger_condition == 0xd4)
  {
    if ((((card == affected_card) && (player == affected_card_controller)) &&
         (reason_for_trigger_controller == player)) &&
        (event == EVENT_TRIGGER))
    {
      pcVar6 = get_card_instance(player, card);
      card_00 = trigger_cause;
      iVar9 = trigger_cause_controller;
      if ((pcVar6->eot_toughness1 & 2) == 0)
      {
        bVar2 = instance->number_of_targets;
        uVar7 = (unsigned int)bVar2;
        iVar8 = uVar7 - 1;
        if (iVar8 < 0)
        {
          return 0;
        }
        while ((trigger_cause != instance->targets[iVar8].card ||
                (trigger_cause_controller != instance->targets[iVar8].player)))
        {
          iVar8 = iVar8 + -1;
          if (iVar8 == -1)
          {
            return 0;
          }
        }
        if (iVar8 < 0)
        {
          return 0;
        }
        iVar4 = instance->targets[uVar7 - 1].card;
        instance->targets[iVar8].player = instance->targets[uVar7 - 1].player;
        instance->targets[iVar8].card = iVar4;
        instance->number_of_targets = bVar2 - 1;
        pcVar6 = get_card_instance(iVar9, card_00);
        if (pcVar6->kill_code == '\x02')
        {
          instance->eot_toughness = (int32_t)((uint32_t)instance->eot_toughness + 0x10000U);
          if (instance->targets[10].card != 0xdeadcad)
          {
            instance->targets[10].card = 0xdeadcad;
            instance->targets[10].player = 0;
          }
          if (trigger_cause_controller == 0)
          {
            instance->targets[10].player =
                (int32_t)(((uint32_t)instance->targets[10].player & 0xffff0000U) |
                          (((uint32_t)instance->targets[10].player + 1U) & 0xffffU));
          }
          else if (trigger_cause_controller == 1)
          {
            instance->targets[10].player =
                (int32_t)((uint32_t)instance->targets[10].player + 0x10000U);
          }
        }
      }
      if (trigger_condition != 0xd5)
      {
        return 0;
      }
    LAB_02677caf:
      if (((card == affected_card) && (player == affected_card_controller)) &&
          (player == reason_for_trigger_controller))
      {
        pcVar6 = get_card_instance(player, card);
        bVar2 = pcVar6->eot_toughness1;
        uVar7 = bVar2 & 2;
        if (((bVar2 & 2) == 0) && ((card != trigger_cause || (player != trigger_cause_controller))))
        {
          pcVar6 = get_card_instance(player, card);
          if (pcVar6->number_of_targets != '\0')
          {
            do
            {
              pcVar5 = in_play(pcVar6->targets[uVar7].player, pcVar6->targets[uVar7].card);
              if (((pcVar5 != NULL) && ((pcVar5->token_status & 0x80) != 0)) &&
                  (pcVar5->kill_code == '\x02'))
                goto LAB_02677c87;
              uVar7 = uVar7 + 1;
            } while ((int)uVar7 < (int)(unsigned int)pcVar6->number_of_targets);
          }
          if (event == EVENT_TRIGGER)
          {
            event_result = event_result | 2;
            return 0;
          }
          if (event == EVENT_RESOLVE_TRIGGER)
            goto LAB_02677d60;
        }
      }
    }
  }
  else if (trigger_condition == 0xd5)
    goto LAB_02677caf;
LAB_02677c87:
  if ((event != EVENT_PHASE_CHANGED) && (event != EVENT_CLEANUP))
  {
    return 0;
  }
LAB_02677d60:
  instance->eot_toughness1 = instance->eot_toughness1 | 2;
  if (((instance->eot_toughness1 & 1) != 0) && (0 < active_cards_count[player]))
  {
    iVar9 = 0;
    do
    {
      pcVar6 = in_play(player, iVar9);
      if (((pcVar6 != NULL) && (pcVar6->damage_source_card == instance->damage_source_card)) &&
          ((pcVar6->damage_source_player == instance->damage_source_player &&
            (((pcVar6->internal_card_id == instance->internal_card_id &&
               (pcVar6->info_slot == instance->info_slot)) &&
              (card != iVar9))))))
      {
        pcVar6->eot_toughness2 =
            pcVar6->eot_toughness2 + (char)((uint32_t)instance->eot_toughness >> 16);
        pcVar6->eot_toughness3 = pcVar6->eot_toughness3 + instance->eot_toughness3;
        instance->eot_toughness = (int32_t)((uint32_t)instance->eot_toughness & 0xffffU);
        if (instance->targets[10].card == 0xdeadcad)
        {
          if (pcVar6->targets[10].card != 0xdeadcad)
          {
            pcVar6->targets[10].card = 0xdeadcad;
            pcVar6->targets[10].player = 0;
          }
          if (10 < pcVar6->number_of_targets)
          {
            pcVar6->number_of_targets = pcVar6->number_of_targets - 1;
          }
          pcVar6->targets[10].player =
              (int32_t)((((uint32_t)pcVar6->targets[10].player +
                          (uint32_t)instance->targets[10].player) &
                         0xffffU) |
                        ((((uint32_t)pcVar6->targets[10].player & 0xffff0000U) +
                          ((uint32_t)instance->targets[10].player & 0xffff0000U)) &
                         0xffff0000U));
          instance->targets[10].player = 0;
        }
        goto LAB_02677ebf;
      }
      iVar9 = iVar9 + 1;
    } while (iVar9 < active_cards_count[player]);
  }
  if (instance->targets[10].card != 0xdeadcad)
  {
    instance->targets[10].card = 0xdeadcad;
    instance->targets[10].player = 0;
  }
  iVar9 = get_internal_card_id_from_csv_id((unsigned int)instance->display_pic_csv_id);
  call_card_fn(cards_data[iVar9].code_pointer, instance, player, card,
               EVENT_RESOLVE_GENERAL_EFFECT);
LAB_02677ebf:
  kill_card(player, card, KILL_EXILE);
  return 0;
}

#endif


#ifndef SHANDALAR
/* ManalinkEh 2022: 0x269f880; original functions/functions.c:18336. */
card_instance_t *legacy_permanents_destroyed_this_way_accumulate(
    int src_player, int src_card, card_instance_t *previous_effect, int tgt_player, int tgt_card)
{
  unsigned char bVar1;
  int result;
  if (previous_effect == NULL)
  {
    result = create_legacy_effect(src_player, src_card,
                                  fx_permanents_destroyed_this_way_accumulate);
    previous_effect = get_card_instance(src_player, result);
  }
  else if (previous_effect->number_of_targets == '\x12')
  {
    previous_effect->eot_toughness1 = previous_effect->eot_toughness1 | 1;
    result = create_legacy_effect(src_player, src_card,
                                  fx_permanents_destroyed_this_way_accumulate);
    previous_effect = get_card_instance(src_player, result);
    previous_effect->eot_toughness1 = previous_effect->eot_toughness1 | 1;
  }
  if (-1 < tgt_player)
  {
    bVar1 = previous_effect->number_of_targets;
    previous_effect->targets[bVar1].player = tgt_player;
    previous_effect->number_of_targets = bVar1 + 1;
    previous_effect->targets[bVar1].card = tgt_card;
  }
  return previous_effect;
}

#endif


#ifndef SHANDALAR
/* ManalinkEh 2022: 0x269ad40; original functions/functions.c:6972. */
int reveal_card_until_you_reveal_test(int player, int t_player, test_definition_t *test,
                                           int flags)
{
  int *deck = deck_ptr[player];
  if (flags & 0x4000)
    deck = deck_ptr[t_player];
  unsigned int qty = test->qty;
  int wanted, limit;
  if (flags & 0x1000)
  {
    wanted = count_deck(t_player);
    if ((int)(qty & 255) < wanted)
      wanted = qty & 255;
    limit = (qty >> 8) & 255;
  }
  else
  {
    wanted = count_deck(t_player);
    if ((int)qty < wanted)
      wanted = qty;
    limit = count_deck(t_player);
  }
  int target_deck_size = count_deck(t_player);
  int remaining = 0;
  uint32_t packed_result = (uint32_t)deck[0];
  if (deck[0] != -1)
  {
    if (target_deck_size < limit)
      limit = target_deck_size;
    int deck_copy[count_deck(player)];
    int recipient = (flags & 0x100) ? t_player : player;
    int removed = 0;
    int found = 0;
    int last_added = -1;
    packed_result = UINT32_MAX;
    int scanned = 0;
    while (deck[scanned] != -1)
    {
      deck_copy[scanned] = deck[scanned];
      if (new_make_test(t_player, deck[scanned], -1, test))
      {
        int iid = deck[scanned];
        if (flags & 0xe0)
        {
          if ((flags & 0xc0) && is_what(-1, iid, 2) && grafdiggers_cages(player, TYPE_CREATURE, 1))
            goto after_move;
          ++removed;
          last_added = add_card_to_hand(recipient, iid);
          remove_card_from_deck(t_player, scanned);
          if (!(flags & 0x100) && player != t_player)
          {
            if (player == 0 && t_player == 1)
              add_state(player, last_added, 0x1000);
            else if (player == 1 && t_player == 0)
              remove_state(player, last_added, 0x1000);
          }
          if (flags & 0x80)
            add_state(recipient, last_added, 0x10);
          if (flags & 0xc0)
          {
            set_special_flags(recipient, last_added, 0x10000);
            put_into_play(recipient, last_added);
          }
        }
      after_move:
        if (flags & 0x2000)
        {
          play_card_in_deck_for_free(player, recipient, scanned);
          ++removed;
        }
        if (flags & 0x800)
        {
          add_card_to_rfg(player, iid);
          rfg_card_in_deck(player, scanned);
          ++removed;
        }
        if (flags & 0x200)
        {
          int report = last_added;
          if (last_added < 0)
          {
            report = iid;
            if (((uint32_t)last_added & packed_result) == UINT32_MAX)
              packed_result = 0;
          }
          if (found == 0)
            packed_result |= (uint32_t)(int32_t)(int16_t)report;
          else if (found == 1)
            packed_result |= (uint32_t)report << 16;
        }
        ++found;
      }
      ++scanned;
      if (found == wanted || scanned == limit)
        break;
    }
    show_deck(current_turn, deck_copy, scanned, "Cards revealed.", 0, ENGINE_DONE_LABEL);
    show_deck(current_turn ^ 1, deck_copy, scanned, "Cards revealed.", 0, ENGINE_DONE_LABEL);
    remaining = scanned - removed;
    if (remaining > 0)
    {
      if (flags & 1)
        mill(t_player, remaining);
      if (flags & 2)
        put_top_x_on_bottom(player, t_player, remaining);
      if (flags & 4)
        put_top_x_on_bottom_in_random_order(t_player, remaining);
      if (flags & 8)
        for (int i = 0; i < remaining; ++i)
          rfg_top_card_of_deck(t_player);
      if (flags & 0x10)
        shuffle(t_player);
    }
  }
  if (flags & 0x400)
    packed_result = (uint32_t)remaining;
  return (int32_t)packed_result;
}

#endif


#ifndef SHANDALAR
/* October 2022 private event context at 0x2775a74. */
static event_t flash_current_event;
#endif


#ifndef SHANDALAR
/* ManalinkEh 2022: 0x2691090; original functions/functions.c:16759. */
static const char *has_mana_to_cast_target(int who_chooses, int player, int card,
                                         int targeting_player, int targeting_card)
{
  event_t event;
  int result;
  char *pcVar2;
  result = get_id(player, card);
  event = flash_current_event;
  result = get_internal_card_id_from_csv_id(result);
  result = has_mana_to_cast_iid(who_chooses, event, result);
  pcVar2 = NULL;
  if (result == 0)
  {
    pcVar2 = "not enough mana";
  }
  return pcVar2;
}

#endif


#ifndef SHANDALAR
/* ManalinkEh 2022: 0x2696b60; original functions/functions.c:16763. */
int can_play_cards_as_though_they_had_flash(int player, int card, event_t event,
                                                 target_definition_t *td, const char *prompt,
                                                 int literal)
{
  card_instance_t *instance;
  int result;
  if ((event != EVENT_CAN_ACTIVATE) && (event != EVENT_ACTIVATE))
  {
    return 0;
  }
  if ((ai_is_speculating == 1) || ((player == 1 && (((unsigned char)trace_mode & 2) == 0))))
  {
    if (td->extra == -1)
      goto LAB_02696c68;
  }
  else
  {
    if (event != EVENT_CAN_ACTIVATE)
      goto LAB_02696bb4;
    if (td->extra != -1)
    {
      flash_current_event = EVENT_CAN_ACTIVATE;
      goto LAB_02696c88;
    }
  LAB_02696c68:
    td->special = td->special | 0x8000;
    td->extra = (int32_t)has_mana_to_cast_target;
  }
  flash_current_event = event;
  if (event == EVENT_CAN_ACTIVATE)
  {
  LAB_02696c88:
    result = can_target(td);
    return result;
  }
LAB_02696bb4:
  flash_current_event = event;
  if (event == EVENT_ACTIVATE)
  {
    if (literal == 0)
    {
      load_text(NULL, prompt);
      prompt = text_lines[0];
    }
    instance = get_card_instance(player, card);
    instance->number_of_targets = '\0';
    result = pick_next_target_noload(td, prompt);
    if ((result != 0) && (result = charge_mana_from_id_with_cost_mod(
                              player, instance->targets[0].card, EVENT_ACTIVATE, -1, 0),
                          result != 0))
    {
      result = instance->targets[0].card;
      instance->number_of_targets = '\0';
      play_card_in_hand_for_free(instance->targets[0].player, result);
      cant_be_responded_to = 1;
    }
  }
  return 0;
}

#endif


#ifndef SHANDALAR
/* ManalinkEh 2022: 0x2691e10; original functions/functions.c:14372. */
int reveal_cards_from_your_hand(int player, int card, test_definition_t *this_test)
{
  card_instance_t *instance;
  int iVar2;
  int iVar3;
  int *piVar4;
  int *piVar5;
  int iVar6;
  int iVar7;
  int revealed[100];
  int your_hand[100];

  iVar7 = active_cards_count[1];
  if (player != 1)
  {
    iVar7 = do_dialog(player, player, card, -1, -1, " Auto mode\n Manual mode", 0);
    if (iVar7 != 0)
    {
      iVar7 = 0;
      if (active_cards_count[player] < 1)
      {
        return 0;
      }
      iVar6 = 0;
      do
      {
        instance = in_hand(player, iVar6);
        if (instance != NULL)
        {
          instance = get_card_instance(player, iVar6);
          your_hand[iVar7] = instance->internal_card_id;
          iVar7 = iVar7 + 1;
        }
        iVar6 = iVar6 + 1;
      } while (iVar6 < active_cards_count[player]);
      if (iVar7 < 1)
      {
        return 0;
      }
      iVar6 = 0;
      do
      {
        iVar2 = show_deck(0, your_hand, iVar7 - iVar6, "Choose a card to reveal", 0,
                               ENGINE_DONE_LABEL);
        if ((iVar2 == -1) ||
            (iVar3 = new_make_test(player, your_hand[iVar2], -1, this_test), iVar3 == 0))
          break;
        revealed[iVar6] = your_hand[iVar2];
        if (iVar2 < iVar7)
        {
          piVar4 = your_hand + iVar2;
          do
          {
            piVar5 = piVar4 + 1;
            *piVar4 = piVar4[1];
            piVar4 = piVar5;
          } while (your_hand + iVar7 != piVar5);
        }
        iVar6 = iVar6 + 1;
      } while (iVar7 != iVar6);
      goto LAB_02691ea0;
    }
    iVar7 = active_cards_count[player];
  }
  if (iVar7 < 1)
  {
    return 0;
  }
  iVar7 = 0;
  iVar6 = 0;
  do
  {
    while ((instance = in_hand(player, iVar7),
            instance == NULL ||
                (iVar2 = new_make_test_in_play(player, iVar7, -1, this_test), iVar2 == 0)))
    {
      iVar7 = iVar7 + 1;
      if (active_cards_count[player] <= iVar7)
        goto LAB_02691ea0;
    }
    instance = get_card_instance(player, iVar7);
    iVar7 = iVar7 + 1;
    revealed[iVar6] = instance->internal_card_id;
    iVar6 = iVar6 + 1;
  } while (iVar7 < active_cards_count[player]);
LAB_02691ea0:
  if (0 < iVar6)
  {
    show_deck(0, revealed, iVar6, "Player revealed these cards", 0, ENGINE_DONE_LABEL);
    return iVar6;
  }
  return iVar6;
}

#endif


#ifndef SHANDALAR
/* ManalinkEh 2022: 0x2699e10; original functions/functions.c:7761. */
static int discard_card_by_int_id(int player, int int_crd_id, int effect,
                                     test_definition_t *this_test, int source_of_effect)
{
  card_instance_t *instance;
  int result;
  int card;
  int iStack_18;

  card = active_cards_count[player] + -1;
  if (card < 0)
  {
    iStack_18 = -1;
  }
  else
  {
    iStack_18 = -1;
    do
    {
      while (instance = in_hand(player, card), instance != NULL)
      {
        instance = get_card_instance(player, card);
        if (-1 < int_crd_id)
        {
          if (instance->internal_card_id == int_crd_id)
          {
          LAB_02699e4d:
            iStack_18 = get_id(player, card);
            if ((effect & 0x4cU) != 0)
            {
              iStack_18 = card;
            }
            if ((effect & 1U) != 0)
            {
              discard_card_impl(player, card, source_of_effect, 0);
            }
            if ((effect & 2U) != 0)
            {
              rfg_card_in_hand(player, card);
            }
            if ((effect & 0x20U) != 0)
            {
              put_on_top_of_deck(player, card);
            }
            if ((effect & 0x100U) != 0)
            {
              put_on_bottom_of_deck(player, card);
            }
            if (-1 < int_crd_id)
            {
              return iStack_18;
            }
          }
          break;
        }
        if (int_crd_id != -1)
          break;
        result = new_make_test_in_play(player, card, -1, this_test);
        if (result != 0)
          goto LAB_02699e4d;
        card = card + -1;
        if (card == -1)
        {
          return iStack_18;
        }
      }
      card = card + -1;
    } while (card != -1);
  }
  return iStack_18;
}

#endif


#ifndef SHANDALAR
/* ManalinkEh 2022: 0x2699f60; original functions/functions.c:7811. */
int new_effect_coercion(ec_definition_t *this_definition, test_definition_t *this_test)
{
  card_instance_t *instance;
  int iVar2;
  int iVar3;
  int *piVar4;
  int *piVar5;
  int iVar6;
  int *piStack_360;
  int iStack_35c;
  int iStack_358;
  int iStack_354;
  int initial_hand[50];
  int ai_hand[50];
  test_definition_t test_default;
  test_definition_t this_test2;

  if (this_test == NULL)
  {
    this_test = &test_default;
    default_test_definition(this_test, 0xff);
  }
  if (0 < this_definition->cards_to_reveal)
  {
    iVar2 = this_definition->target_player;
    if (hand_count[iVar2] < this_definition->cards_to_reveal)
    {
      this_definition->cards_to_reveal = hand_count[iVar2];
    }
    iVar6 = 0;
    iStack_35c = 0;
    if (0 < active_cards_count[iVar2])
    {
      iStack_35c = 0;
      do
      {
        instance = in_hand(iVar2, iVar6);
        if (instance != NULL)
        {
          iVar2 = get_original_internal_card_id(this_definition->target_player, iVar6);
          initial_hand[iStack_35c] = iVar2;
          iStack_35c = iStack_35c + 1;
        }
        iVar2 = this_definition->target_player;
        iVar6 = iVar6 + 1;
      } while (iVar6 < active_cards_count[iVar2]);
    }
    new_default_test_definition(&this_test2, 0xff, "Select a card to reveal.");
    piStack_360 = initial_hand + iStack_35c + -1;
    iStack_354 = 0;
    do
    {
      do
      {
        if (this_definition->cards_to_reveal <= iStack_354)
        {
          if ((this_definition->effect & 0x80) == 0)
            goto LAB_0269a16c;
          if (0 < this_definition->cards_to_reveal)
          {
            piVar5 = ai_hand;
            show_deck(this_definition->who_choses, piVar5, iStack_354,
                           "Target player revealed these.", 0, ENGINE_DONE_LABEL);
            if (iStack_354 < 1)
            {
              return -1;
            }
            piVar4 = piVar5 + iStack_354;
            do
            {
              iVar2 = new_make_test(this_definition->target_player, *piVar5, -1, this_test);
              if (iVar2 != 0)
              {
                discard_card_by_int_id(this_definition->target_player, *piVar5, 1, NULL,
                                                 this_definition->who_choses);
              }
              piVar5 = piVar5 + 1;
            } while (piVar4 != piVar5);
            return -1;
          }
          goto LAB_0269a3d6;
        }
        iVar2 = select_card_from_zone(this_definition->target_player,
                                           this_definition->target_player, initial_hand,
                                           iStack_35c - iStack_354, 1, 3, -1, &this_test2);
      } while (iVar2 == -1);
      ai_hand[iStack_354] = initial_hand[iVar2];
      iStack_354 = iStack_354 + 1;
      piVar5 = initial_hand + iVar2;
      if (iVar2 < iStack_35c - iStack_354)
      {
        do
        {
          piVar4 = piVar5 + 1;
          *piVar5 = piVar5[1];
          piVar5 = piVar4;
        } while (piVar4 != piStack_360);
      }
      piStack_360 = piStack_360 + -1;
    } while (true);
  }
  if ((this_definition->effect & 0x80) == 0)
  {
    iStack_354 = 0;
  LAB_0269a16c:
    iStack_358 = 0;
    iStack_35c = -1;
    if (0 < this_definition->qty)
    {
      if (0 < this_definition->cards_to_reveal)
        goto LAB_0269a276;
      do
      {
        iVar2 = this_definition->target_player;
        if (hand_count[iVar2] < 1)
        {
          return iStack_35c;
        }
        this_test->zone = 0x100;
        iVar2 = check_battlefield_for_special_card(-1, -1, iVar2, 0, this_test);
        iVar2 = new_select_a_card(this_definition->who_choses, this_definition->target_player,
                                       4, (unsigned int)(iVar2 != 0),
                                       this_definition->ai_selection_mode, -1, this_test);
        if (iVar2 == -1)
        {
          return iStack_35c;
        }
        iVar6 = get_original_internal_card_id(this_definition->target_player, iVar2);
        iVar2 = iStack_354;
        while (true)
        {
          iStack_354 = iVar2;
          if (iVar6 != -1)
          {
            iStack_35c = discard_card_by_int_id(this_definition->target_player, iVar6,
                                                          this_definition->effect, NULL,
                                                          this_definition->who_choses);
          }
          iStack_358 = iStack_358 + 1;
          if (this_definition->qty <= iStack_358)
          {
            return iStack_35c;
          }
          if (this_definition->cards_to_reveal < 1)
            break;
        LAB_0269a276:
          if (iStack_354 < 1)
          {
            return iStack_35c;
          }
          iVar2 = 0;
          do
          {
            iVar6 =
                new_make_test(this_definition->target_player, ai_hand[iVar2], -1, this_test);
            if (iVar6 != 0)
            {
              iVar2 = 1;
              goto LAB_0269a2c8;
            }
            iVar2 = iVar2 + 1;
          } while (iVar2 != iStack_354);
          iVar2 = 0;
        LAB_0269a2c8:
          iVar3 = select_card_from_zone(
              this_definition->who_choses, this_definition->target_player, ai_hand, iStack_354,
              iVar2, this_definition->ai_selection_mode, -1, this_test);
          if (iVar3 == -1)
          {
            return iStack_35c;
          }
          iVar2 = iStack_354 + -1;
          iVar6 = ai_hand[iVar3];
          if (iVar3 < iVar2)
          {
            piVar5 = ai_hand + iVar3;
            do
            {
              piVar4 = piVar5 + 1;
              *piVar5 = piVar5[1];
              piVar5 = piVar4;
            } while (piVar4 != ai_hand + iStack_354 - 1);
          }
        }
      } while (true);
    }
  }
  else
  {
  LAB_0269a3d6:
    reveal_target_player_hand(this_definition->target_player);
    discard_card_by_int_id(this_definition->target_player, -1, this_definition->effect,
                                     this_test, this_definition->who_choses);
  }
  return -1;
}

#endif


#ifndef SHANDALAR
/* ManalinkEh 2022: 0x269fe40; original functions/functions.c:18459. */
int exile_top_x_card_you_may_play_them(int player, int card, int howmany,
                                            test_definition_t *test, int t_player,
                                            mpcfe_mode_t flags)
{
  int *deck;
  int iVar1;
  int iVar2;
  int iVar3;
  int iStack_80;
  int iStack_78;
  char buffer[100];

  iVar1 = count_deck(t_player);
  iStack_78 = howmany;
  if (iVar1 < howmany)
  {
    iStack_78 = iVar1;
  }
  if (iStack_78 != 0)
  {
    iVar1 = scnprintf(buffer, 100, "Cards exiled");
    if (card != -1)
    {
      iVar3 = get_id(player, card);
      scnprintf(buffer + iVar1, 100 - iVar1, " by %s", cards_ptr[iVar3]->name);
    }
    deck = deck_ptr[t_player];
    show_deck(player, deck, iStack_78, buffer, 0, ENGINE_DONE_LABEL);
    if ((*deck != -1) && (0 < iStack_78))
    {
      iVar1 = 0;
      iStack_80 = -1;
      do
      {
        iVar3 = *deck_ptr[t_player];
        if (iVar3 != -1)
        {
          rfg_top_card_of_deck(t_player);
          if (test != NULL)
          {
            iVar2 = new_make_test(t_player, iVar3, -1, test);
            if (iVar2 == 0)
              goto LAB_0269ff5c;
          }
          iStack_80 = create_may_play_card_from_exile_effect(
              player, card, t_player, (unsigned int)cards_data[iVar3].id, flags);
        }
      LAB_0269ff5c:
        iVar1 = iVar1 + 1;
        if ((*deck == -1) || (iStack_78 <= iVar1))
        {
          return iStack_80;
        }
      } while (true);
    }
  }
  return -1;
}

#endif


#ifndef SHANDALAR
/* ManalinkEh 2022: 0x2683090; original functions/functions.c:5703. */
static int shapeshift_target_effect(int player, int card, event_t event)
{
  unsigned int uVar1;
  int card_00;
  CardFunction func_ptr;
  card_instance_t *pcVar2;
  int result;
  card_instance_t *pcVar4;
  unsigned int uVar5;
  int player_00;

  pcVar2 = get_card_instance(player, card);
  uVar1 = pcVar2->targets[1].player;
  card_00 = pcVar2->damage_target_card;
  player_00 = (int)pcVar2->damage_target_player;
  func_ptr = (CardFunction)pcVar2->targets[2].player;
  uVar5 = uVar1 & 0x400000;
  if ((((event == EVENT_CHANGE_TYPE) && (card_00 == affected_card)) &&
       (result = pcVar2->targets[1].card, affected_card_controller == player_00)) &&
      ((result != -1 && ((((unsigned int)land_can_be_played >> 16 & 0xff) & 2) == 0))))
  {
    event_result = result;
    if (uVar5 != 0)
    {
      set_special_flags2(player_00, card_00, 0x400);
    LAB_02683250:
      token_characteristic_setting_effects(player, card, event, player_00, card_00);
      goto LAB_026830d6;
    }
    if ((uVar1 & 1) == 0)
    {
      return 0;
    }
  LAB_02683130:
    result = eot_trigger(player, card, event);
    if (result != 0)
      goto LAB_02683159;
  }
  else
  {
    if (uVar5 != 0)
      goto LAB_02683250;
  LAB_026830d6:
    if ((uVar1 & 1) != 0)
      goto LAB_02683130;
  }
  if (event != EVENT_STATIC_EFFECTS)
  {
    return 0;
  }
  if ((uVar1 & 0x800000) == 0)
  {
    return 0;
  }
  result = count_counters(player_00, card_00, uVar1 >> 0x18);
  if (result != 0)
  {
    return 0;
  }
LAB_02683159:
  if (((uVar1 & 2) != 0) && (func_ptr != (CardFunction)0xffffffff))
  {
    create_targetted_legacy_activate((int)pcVar2->damage_source_player, pcVar2->damage_source_card,
                                     func_ptr, player_00, card_00);
  }
  if ((uVar5 != 0) && (pcVar4 = get_card_instance(player, card),
                       (pcVar4->ext->special_flags2 & SF2_TEMPORARY_COPY_OF_TOKEN) == 0))
  {
    set_special_infos(player_00, card_00, pcVar2->targets[10].player);
  }
  if (((uVar1 & 8) != 0) && (pcVar2 = get_card_instance(player, card),
                             (pcVar2->ext->special_flags3 & 0x1000000U) != SF3_0))
  {
    remove_special_flags(player_00, card_00, 0x1000000);
  }
  kill_card(player, card, KILL_EXILE);
  pcVar2 = get_card_instance(player_00, card_00);
  pcVar2->regen_status = pcVar2->regen_status | 0x1000000;
  get_abilities(player_00, card_00, EVENT_CHANGE_TYPE, -1);
  pcVar2 = in_play(player_00, card_00);
  if (pcVar2 == NULL)
  {
    return 0;
  }
  result = get_id(player_00, card_00);
  verify_legend_rule(player_00, card_00, result);
  return 0;
}

#endif


#ifndef SHANDALAR
/* ManalinkEh 2022: 0x26867e0; original functions/functions.c:5769. */
int real_shapeshift_target(int src_player, int src_card, int t_player, int t_card,
                                int turn_into_player, int turn_into_card, int mode)
{
  int card;
  card_instance_t *pcVar1;
  int iVar2;
  card_instance_t *pcVar3;
  int iVar4;
  unsigned int argument1;
  int card_00;
  target_t tVar5;

  tVar5 = find_repeat_legacy_effect_alt(src_player, src_card, shapeshift_target_effect,
                                        t_player, t_card, -1, src_player);
  if (tVar5.card != -1)
  {
    kill_card(src_player, tVar5.card, KILL_EXILE);
  }
  card = create_targetted_legacy_effect(src_player, src_card, shapeshift_target_effect,
                                        t_player, t_card);
  pcVar1 = get_card_instance(src_player, card);
  pcVar1->targets[0].player = t_player;
  pcVar1->targets[0].card = t_card;
  pcVar1->targets[10].player = 0;
  pcVar1->targets[1].player = mode & 0xffbfffff;
  if (turn_into_player == -1)
  {
    pcVar1->targets[1].card = turn_into_card;
  }
  else
  {
    pcVar3 = get_card_instance(turn_into_player, turn_into_card);
    argument1 = pcVar3->internal_card_id;
    if (argument1 == 0xffffffff)
    {
      argument1 = (unsigned int)pcVar3->backup_internal_card_id;
    }
    if (((mode & 4U) != 0) && ((cards_data[argument1].type & 0x40) == 0))
    {
      argument1 = create_a_card_type(argument1);
      cards_data[argument1].type = cards_data[argument1].type | 0x40;
    }
    pcVar1->targets[1].card = argument1;
    iVar2 = copy_token_characteristics_for_clone(src_player, card, turn_into_player,
                                                      turn_into_card);
    if (iVar2 != 0)
    {
      pcVar1->targets[1].player = pcVar1->targets[1].player | 0x400000;
      iVar2 = get_special_infos(t_player, t_card);
      pcVar1->targets[10].player = iVar2;
      pcVar3 = get_card_instance(t_player, t_card);
      if ((pcVar3->ext->special_flags2 & SF2_TEMPORARY_COPY_OF_TOKEN) != 0)
      {
        iVar2 = 0;
        do
        {
          card_00 = 0;
          if (0 < active_cards_count[iVar2])
          {
            do
            {
              pcVar3 = in_play(iVar2, card_00);
              if (((((pcVar3 != NULL) && (pcVar3->internal_card_id == LEGACY_EFFECT_CUSTOM)) &&
                    (pcVar3->info_slot_fn == shapeshift_target_effect)) &&
                   ((pcVar3->damage_target_player == iVar2 &&
                     (pcVar3->damage_target_card == card_00)))) &&
                  (((pcVar3->targets[1].player & 0x400000) != 0 &&
                    (pcVar3 = get_card_instance(iVar2, card_00),
                     (pcVar3->ext->special_flags2 & SF2_TEMPORARY_COPY_OF_TOKEN) == 0))))
              {
                set_special_flags2(iVar2, card_00, 0x400);
                iVar4 = get_special_infos(iVar2, card_00);
                pcVar1->targets[10].player = iVar4;
              }
              card_00 = card_00 + 1;
            } while (card_00 < active_cards_count[iVar2]);
          }
          iVar2 = iVar2 + 1;
        } while (iVar2 != 2);
      }
      iVar2 = get_special_infos(src_player, card);
      set_special_infos(t_player, t_card, iVar2);
    }
  }
  pcVar1 = get_card_instance(t_player, t_card);
  pcVar1->regen_status = pcVar1->regen_status | 0x1f000000;
  get_abilities(t_player, t_card, EVENT_CHANGE_TYPE, -1);
  get_abilities(t_player, t_card, EVENT_SET_COLOR, -1);
  get_abilities(t_player, t_card, EVENT_ABILITIES, -1);
  iVar2 = is_what(t_player, t_card, 2);
  if ((iVar2 != 0) &&
      (get_abilities(t_player, t_card, EVENT_TOUGHNESS, -1), pcVar1->internal_card_id != -1))
  {
    get_abilities(t_player, t_card, EVENT_POWER, -1);
    return card;
  }
  return card;
}

#endif


#ifndef SHANDALAR
/* ManalinkEh 2022: 0x2692630; original functions/functions.c:14669. */
void mana_into_string(int cless, int black, int blue, int green, int red, int white,
                           test_definition_t *this_test)
{
  int iVar1;
  int iVar2;
  int iVar3;
  char buffer[100];

  iVar1 = 0;
  if (cless != 0)
  {
    if ((cless == -1) || (cless == 0x28))
    {
      iVar1 = scnprintf(buffer, 100, "X");
    }
    else
    {
      iVar1 = scnprintf(buffer, 100, "%d", cless);
    }
  }
  if (0 < black)
  {
    iVar3 = 0;
    do
    {
      iVar3 = iVar3 + 1;
      iVar2 = scnprintf(buffer + iVar1, 100 - iVar1, "B");
      iVar1 = iVar1 + iVar2;
    } while (black != iVar3);
  }
  if (0 < blue)
  {
    iVar3 = 0;
    do
    {
      iVar3 = iVar3 + 1;
      iVar2 = scnprintf(buffer + iVar1, 100 - iVar1, "U");
      iVar1 = iVar1 + iVar2;
    } while (blue != iVar3);
  }
  if (0 < green)
  {
    iVar3 = 0;
    do
    {
      iVar3 = iVar3 + 1;
      iVar2 = scnprintf(buffer + iVar1, 100 - iVar1, "G");
      iVar1 = iVar1 + iVar2;
    } while (green != iVar3);
  }
  if (0 < red)
  {
    iVar3 = 0;
    do
    {
      iVar3 = iVar3 + 1;
      iVar2 = scnprintf(buffer + iVar1, 100 - iVar1, "R");
      iVar1 = iVar1 + iVar2;
    } while (red != iVar3);
  }
  if (0 < white)
  {
    iVar3 = 0;
    do
    {
      iVar3 = iVar3 + 1;
      iVar2 = scnprintf(buffer + iVar1, 100 - iVar1, "W");
      iVar1 = iVar1 + iVar2;
    } while (white != iVar3);
  }
  strcpy(this_test->message, buffer);
  return;
}

#endif


#ifndef SHANDALAR
/* October 2022 shared cycling state at 0x2775470. */
int card_is_being_cycled = 0;
#endif


#ifndef SHANDALAR
/* ManalinkEh 2022: 0x2692dd0; original functions/functions.c:14958. */
int can_counter_activated_ability(int player, int card, event_t event,
                                       target_definition_t *td)
{
  int iVar1;
  int iVar2;
  target_definition_t td_default;

  if (td == NULL)
  {
    td = &td_default;
    counter_activated_target_definition(player, card, td, 0);
  }
  iVar2 = ((unsigned int)event - (unsigned int)EVENT_CAST_SPELL < 2U) + 1;
  if (iVar2 <= stack_size)
  {
    iVar1 = would_validate_arbitrary_target(td, stack_cards[stack_size - iVar2].player,
                                                 stack_cards[stack_size - iVar2].card);
    if (((iVar1 != 0) && ((char)stack_data[stack_size - iVar2].generating_event != '~')) &&
        (cards_data[stack_data[stack_size - iVar2].internal_card_id].id != 0x388))
    {
      return 99;
    }
  }
  return 0;
}

#endif


#ifndef SHANDALAR
/* ManalinkEh 2022: 0x2693060; original functions/functions.c:15042. */
int can_counter_triggered_ability(int player, int card, event_t event,
                                       target_definition_t *td)
{
  int result;
  target_definition_t td_default;
  if (td == NULL)
  {
    td = &td_default;
    counter_triggered_target_definition(player, card, td, 0xff);
  }
  if ((((0 < stack_size) &&
        (result = would_validate_arbitrary_target(td, player, card), result != 0)) &&
       ((char)stack_data[stack_size - 1].generating_event == '~')) &&
      ((((unsigned int)land_can_be_played >> 8 & 0xff) & 2) == 0))
  {
    return 99;
  }
  return 0;
}

#endif


#ifndef SHANDALAR
/* ManalinkEh 2022: 0x2692e80; original functions/functions.c:14977. */
int cast_counter_activated_ability(int player, int card, int target_pos)
{
  int32_t result;
  card_instance_t *instance;
  if (stack_size < 2)
  {
    spell_fizzled = 1;
    return 0;
  }
  instance = get_card_instance(player, card);
  result = stack_cards[stack_size - 2].card;
  instance->targets[target_pos].player = stack_cards[stack_size - 2].player;
  instance->targets[target_pos].card = result;
  instance->number_of_targets = instance->number_of_targets + '\x01';
  ai_modifier = ai_modifier + 100;
  return 1;
}

#endif


#ifndef SHANDALAR
/* ManalinkEh 2022: 0x2692ef0; original functions/functions.c:14996. */
int validate_counter_activated_ability(int player, int card, target_definition_t *td,
                                            int target_pos)
{
  uint16_t uVar1;
  int32_t iVar2;
  card_instance_t *instance;
  int iVar4;
  target_definition_t td_default;

  instance = get_card_instance(player, card);
  if (td == NULL)
  {
    td = &td_default;
    counter_activated_target_definition(player, card, td, 0);
  }
  iVar2 = td->preferred_controller;
  td->preferred_controller = td->allowed_controller;
  iVar4 = validate_arbitrary_target(td, instance->targets[target_pos].player,
                                         instance->targets[target_pos].card);
  if (iVar4 != 0)
  {
    instance =
        get_card_instance(instance->targets[target_pos].player, instance->targets[target_pos].card);
    if ((instance->state & 0x80) != 0)
    {
      uVar1 = cards_data[stack_data[stack_size - 1].internal_card_id].id;
      td->preferred_controller = iVar2;
      return (unsigned int)(uVar1 != 0x388);
    }
  }
  td->preferred_controller = iVar2;
  return 0;
}

#endif


#ifndef SHANDALAR
/* ManalinkEh 2022: 0x2692fc0; original functions/functions.c:15025. */
void raw_counter_activated_ability(int tgt_player, int tgt_card)
{
  card_instance_t *instance;
  instance = get_card_instance(tgt_player, tgt_card);
  instance->internal_card_id = -1;
  return;
}

#endif


#ifndef SHANDALAR
/* ManalinkEh 2022: 0x2692ff0; original functions/functions.c:15030. */
int resolve_counter_activated_ability(int player, int card, target_definition_t *td,
                                           int target_pos)
{
  int result;
  card_instance_t *instance;
  result = validate_counter_activated_ability(player, card, td, target_pos);
  if (result == 0)
  {
    return 0;
  }
  instance = get_card_instance(player, card);
  instance =
      get_card_instance(instance->targets[target_pos].player, instance->targets[target_pos].card);
  instance->internal_card_id = -1;
  return 1;
}

#endif


#ifndef SHANDALAR
/* ManalinkEh 2022: 0x268db60; original functions/functions.c:12601. */
int charge_mana_for_multi_x(int player, int color, int num_x)
{
  int result;
  int amount;
  result = hack_xx;
  if ((player == 0) && (ai_is_speculating != 1))
  {
    hack_xx = num_x;
    charge_mana(0, color, -1);
    amount = x_value;
  }
  else
  {
    result = has_mana(player, color, 1);
    if (result / num_x < 1)
    {
      spell_fizzled = 1;
      return 0;
    }
    amount = (result / num_x) * num_x;
    charge_mana(player, color, amount);
    result = hack_xx;
  }
  if (spell_fizzled != 1)
  {
    hack_xx = result;
    return amount;
  }
  hack_xx = result;
  return 0;
}

#endif


#ifndef SHANDALAR
/* ManalinkEh 2022: 0x268dc30; original functions/functions.c:12632. */
int charge_mana_for_double_x(int player, int color)
{
  int result;
  result = charge_mana_for_multi_x(player, color, 2);
  return result;
}

#endif


#ifndef SHANDALAR
/* ManalinkEh 2022: 0x2696e30; original functions/functions.c:16852. */
void if_a_card_would_be_put_into_graveyard_from_play_exile_it_instead(
    int player, int card, int event, int t_player, test_definition_t *test)
{
  card_instance_t *instance;
  int result;
  bool bVar3;

  if (event == 0x77)
  {
    instance = in_play(player, card);
    if (instance != NULL)
    {
      result = is_humiliated(player, card, AL_STATIC);
      if ((result == 0) && ((t_player == affected_card_controller || (t_player == 2))))
      {
        instance = in_play(affected_card_controller, affected_card);
        if ((instance != NULL) && ((unsigned char)(instance->kill_code - 1) < 3))
        {
          if (test == NULL)
          {
            result = is_token(affected_card_controller, affected_card);
            bVar3 = result == 0;
          }
          else
          {
            result = new_make_test_in_play(affected_card_controller, affected_card, -1, test);
            bVar3 = result != 0;
          }
          if (bVar3)
          {
            set_special_flags2(affected_card_controller, affected_card, 0x10000000);
            instance->kill_code = '\x04';
          }
        }
      }
    }
  }
  return;
}

#endif


#ifndef SHANDALAR
/* ManalinkEh 2022: 0x2696f20; original functions/functions.c:16868. */
int if_a_card_would_be_put_into_graveyard_from_library_do_something_instead(
    int player, int card, int event, int t_player, resolve_trigger_t trigger_mode,
    test_definition_t *test)
{
  xtrigger_t xVar1;
  int result;
  enable_xtrigger_flags = enable_xtrigger_flags | ENABLE_XTRIGGER_REPLACE_MILL;
  xVar1 = trigger_condition;
  if (trigger_condition == 0xe5)
  {
    xVar1 = xtrigger_impl_value_dont_use_directly;
  }
  if (xVar1 == XTRIGGER_REPLACE_MILL)
  {
    if (((((card == affected_card) && (player == affected_card_controller)) &&
          (reason_for_trigger_controller == player)) &&
         (replace_milled == 0)) &&
        (((trigger_cause_controller == t_player || (t_player == 2)) &&
          (result = is_humiliated(player, card, AL_STATIC), result == 0))))
    {
      if ((test != NULL) &&
          (result = new_make_test(trigger_cause_controller,
                                       deck_ptr[trigger_cause_controller][trigger_cause], -1, test),
           result == 0))
      {
        return 0;
      }
      if (event == 0x7d)
      {
        if (trigger_mode == RESOLVE_TRIGGER_DUH)
        {
          result = duh_mode(player);
          trigger_mode = (result != 0) + RESOLVE_TRIGGER_OPTIONAL;
        }
        event_result = event_result | trigger_mode;
      }
      return (unsigned int)(event == 0x7e);
    }
    return 0;
  }
  return 0;
}

#endif


#ifndef SHANDALAR
/* ManalinkEh 2022: 0x2697040; original functions/functions.c:16892. */
int if_a_card_would_be_put_into_graveyard_from_anywhere_but_library_do_something_instead(
    int player, int card, int event, int t_player, resolve_trigger_t trigger_mode,
    test_definition_t *test)
{
  xtrigger_t xVar1;
  int result;
  enable_xtrigger_flags =
      enable_xtrigger_flags | ENABLE_XTRIGGER_REPLACE_CARD_TO_GY_FROM_ANYWHERE_BUT_LIBRARY;
  xVar1 = trigger_condition;
  if (trigger_condition == 0xe5)
  {
    xVar1 = xtrigger_impl_value_dont_use_directly;
  }
  if (xVar1 == XTRIGGER_REPLACE_CARD_TO_GY_FROM_ANYWHERE_BUT_LIBRARY)
  {
    if (((((card == affected_card) && (player == affected_card_controller)) &&
          (reason_for_trigger_controller == player)) &&
         (replace_milled == 0)) &&
        (((trigger_cause_controller == t_player || (t_player == 2)) &&
          (result = is_humiliated(player, card, AL_STATIC), result == 0))))
    {
      if ((test != NULL) &&
          (result = new_make_test(trigger_cause_controller, trigger_cause, -1, test),
           result == 0))
      {
        return 0;
      }
      if (event == 0x7d)
      {
        if (trigger_mode == RESOLVE_TRIGGER_DUH)
        {
          result = duh_mode(player);
          trigger_mode = (result != 0) + RESOLVE_TRIGGER_OPTIONAL;
        }
        event_result = event_result | trigger_mode;
      }
      return (unsigned int)(event == 0x7e);
    }
    return 0;
  }
  return 0;
}

#endif


#ifndef SHANDALAR
/* ManalinkEh 2022: 0x2697150; original functions/functions.c:16943. */
void if_a_card_would_be_put_into_graveyard_from_anywhere_exile_it_instead(
    int player, int card, int event, int t_player, test_definition_t *test)
{
  int result;
  result = if_a_card_would_be_put_into_graveyard_from_library_do_something_instead(
      player, card, event, t_player, RESOLVE_TRIGGER_MANDATORY, test);
  if (result != 0)
  {
    rfg_top_card_of_deck(trigger_cause_controller);
    replace_milled = 1;
  }
  result =
      if_a_card_would_be_put_into_graveyard_from_anywhere_but_library_do_something_instead(
          player, card, event, t_player, RESOLVE_TRIGGER_MANDATORY, test);
  if (result != 0)
  {
    add_card_to_rfg(trigger_cause_controller, trigger_cause);
    play_sound_effect(WAV_DESTROY);
    replace_milled = 1;
  }
  if_a_card_would_be_put_into_graveyard_from_play_exile_it_instead(player, card, event,
                                                                        t_player, test);
  return;
}

#endif


#ifndef SHANDALAR
/* ManalinkEh 2022: 0x268fdf0; original functions/functions.c:12260. */
static int how_much_life_can_i_pay_for_mana(int player)
{
  if (life_total_can_change(player) && !check_battlefield_for_id(2, 0x3010))
    return life[player];
  return 0;
}

#endif


#ifndef SHANDALAR
/* ManalinkEh 2022: 0x268fe30; original functions/functions.c:2466. */
int has_mana_multi_a(int player, int colorless, int black, int blue, int green, int red,
                          int white)
{
  if (card_flags[player] & 2)
  {
    int krrik_offset = how_much_life_can_i_pay_for_mana(player) / 2;
    black = black < krrik_offset ? 0 : black - krrik_offset;
  }
  colorless = MAX(0, colorless);
  black = MAX(0, black);
  blue = MAX(0, blue);
  green = MAX(0, green);
  red = MAX(0, red);
  white = MAX(0, white);
  if (!(black || blue || green || red || white))
    return has_mana(player, COLOR_ARTIFACT, colorless) != 0;
  for (int generic = colorless; generic >= 0; --generic)
  {
    MANA_BLACK = black;
    MANA_BLUE = blue;
    MANA_GREEN = green;
    MANA_RED = red;
    MANA_WHITE = white;
    MANA_COLORLESS = generic;
    MANA_ARTIFACT = colorless - generic;
    MANA_UNKNOWN = 0;
    if (check_mana_multi(player))
      return 1;
  }
  return 0;
}

#endif


#ifndef SHANDALAR
/* ManalinkEh 2022: 0x268ff90; original functions/functions.c:2449. */
int has_mana_multi(int player, int colorless, int black, int blue, int green, int red,
                        int white)
{
  if (card_flags[player] & 2)
  {
    int krrik_offset = how_much_life_can_i_pay_for_mana(player) / 2;
    black = black < krrik_offset ? 0 : black - krrik_offset;
  }
  MANA_ARTIFACT = 0;
  MANA_UNKNOWN = 0;
  MANA_COLORLESS = MAX(0, colorless);
  MANA_BLACK = MAX(0, black);
  MANA_BLUE = MAX(0, blue);
  MANA_GREEN = MAX(0, green);
  MANA_RED = MAX(0, red);
  MANA_WHITE = MAX(0, white);
  return check_mana_multi(player);
}

#endif


#ifndef SHANDALAR
/* ManalinkEh 2022: 0x267bc50; original functions/functions.c:2503. */
int charge_mana_multi(int player, int colorless, int black, int blue, int green, int red,
                           int white)
{
  pay_mana_xbugrwaU[0] = colorless;
  pay_mana_xbugrwaU[1] = black;
  pay_mana_xbugrwaU[2] = blue;
  pay_mana_xbugrwaU[3] = green;
  pay_mana_xbugrwaU[4] = red;
  pay_mana_xbugrwaU[5] = white;
  charge_mana(player, COLOR_COLORLESS, 0);
  return spell_fizzled != 1;
}

#endif


#ifndef SHANDALAR
/* ManalinkEh 2022: 0x267bcb0; original functions/functions.c:2517. */
int charge_mana_multi_a(int player, int colorless, int black, int blue, int green, int red,
                             int white, int artifact)
{
  pay_mana_xbugrwaU[0] = colorless;
  pay_mana_xbugrwaU[1] = black;
  pay_mana_xbugrwaU[2] = blue;
  pay_mana_xbugrwaU[3] = green;
  pay_mana_xbugrwaU[4] = red;
  pay_mana_xbugrwaU[5] = white;
  pay_mana_xbugrwaU[6] = artifact;
  charge_mana(player, COLOR_COLORLESS, 0);
  return spell_fizzled != 1;
}

#endif


#ifndef SHANDALAR
/* ManalinkEh 2022: 0x267bd20; original functions/functions.c:2534. */
int charge_mana_while_resolving(int player, int card, event_t event, int charged_player,
                                     color_t color, int amount)
{
  if (event == EVENT_UPKEEP_TRIGGER_ABILITY)
    event = EVENT_RESOLVE_TRIGGER;
  put_card_or_activation_onto_stack(player, card, event, 0, 0);
  spell_fizzled = 0;
  ldoubleclicked = 0;
  charge_mana(charged_player, color, amount);
  obliterate_top_card_of_stack();
  return spell_fizzled != 1;
}

#endif


#ifndef SHANDALAR
/* ManalinkEh 2022: 0x267bda0; original functions/functions.c:2545. */
int charge_mana_while_resolving_csvid(int csvid, event_t event, int charged_player,
                                           color_t color, int amount)
{
  int card_added = add_card_to_hand(charged_player, get_internal_card_id_from_csv_id(csvid));
  card_instance_t *instance = get_card_instance(charged_player, card_added);
  unsigned state = instance->state;
  --hand_count[charged_player];
  instance->state = state | STATE_INVISIBLE;
  if (event == EVENT_UPKEEP_TRIGGER_ABILITY)
    event = EVENT_RESOLVE_TRIGGER;
  put_card_or_activation_onto_stack(charged_player, card_added, event, 0, 0);
  spell_fizzled = 0;
  ldoubleclicked = 0;
  charge_mana(charged_player, color, amount);
  obliterate_top_card_of_stack();
  obliterate_card(charged_player, card_added);
  return spell_fizzled != 1;
}

#endif


#ifndef SHANDALAR
/* ManalinkEh 2022: 0x267be70; original functions/functions.c:2563. */
int charge_mana_multi_while_resolving(int player, int card, event_t event, int charged_player,
                                           int colorless, int black, int blue, int green, int red,
                                           int white)
{
  if (event == EVENT_UPKEEP_TRIGGER_ABILITY)
    event = EVENT_RESOLVE_TRIGGER;
  put_card_or_activation_onto_stack(player, card, event, 0, 0);
  pay_mana_xbugrwaU[0] = colorless;
  pay_mana_xbugrwaU[1] = black;
  pay_mana_xbugrwaU[2] = blue;
  pay_mana_xbugrwaU[3] = green;
  pay_mana_xbugrwaU[4] = red;
  pay_mana_xbugrwaU[5] = white;
  spell_fizzled = 0;
  ldoubleclicked = 0;
  charge_mana(charged_player, COLOR_COLORLESS, 0);
  int result = spell_fizzled != 1;
  obliterate_top_card_of_stack();
  return result;
}

#endif


#ifndef SHANDALAR
/* ManalinkEh 2022: 0x267bf30; original functions/functions.c:2574. */
int charge_mana_multi_while_resolving_csvid(int csvid, event_t event, int charged_player,
                                                 int colorless, int black, int blue, int green,
                                                 int red, int white)
{
  int card_added = add_card_to_hand(charged_player, get_internal_card_id_from_csv_id(csvid));
  card_instance_t *instance = get_card_instance(charged_player, card_added);
  unsigned state = instance->state;
  --hand_count[charged_player];
  instance->state = state | STATE_INVISIBLE;
  if (event == EVENT_UPKEEP_TRIGGER_ABILITY)
    event = EVENT_RESOLVE_TRIGGER;
  put_card_or_activation_onto_stack(charged_player, card_added, event, 0, 0);
  pay_mana_xbugrwaU[0] = colorless;
  pay_mana_xbugrwaU[1] = black;
  pay_mana_xbugrwaU[2] = blue;
  pay_mana_xbugrwaU[3] = green;
  pay_mana_xbugrwaU[4] = red;
  spell_fizzled = 0;
  ldoubleclicked = 0;
  pay_mana_xbugrwaU[5] = white;
  charge_mana(charged_player, COLOR_COLORLESS, 0);
  int result = spell_fizzled != 1;
  obliterate_top_card_of_stack();
  obliterate_card(charged_player, card_added);
  return result;
}

#endif


#ifndef SHANDALAR
/* ManalinkEh 2022: 0x267c030; original functions/functions.c:2592. */
int charge_mana_hybrid_while_resolving(int player, int card, event_t event, int amt_colored,
                                            color_t first_color, color_t second_color,
                                            int amt_colorless)
{
  if (event == EVENT_UPKEEP_TRIGGER_ABILITY)
    event = EVENT_RESOLVE_TRIGGER;
  put_card_or_activation_onto_stack(player, card, event, 0, 0);
  spell_fizzled = 0;
  ldoubleclicked = 0;
  int result =
      charge_mana_hybrid(player, card, amt_colored, first_color, second_color, amt_colorless);
  obliterate_top_card_of_stack();
  return result;
}

#endif


#ifndef SHANDALAR
/* ManalinkEh 2022: 0x267f8c0; original functions/functions.c:6208. */
int get_hacked_color(int player, int card, int color)
{
  card_instance_t *instance;
  int result;

  instance = get_card_instance(player, card);
  result = instance->hack_mode[color];
  if (!instance->hack_mode[color])
    result = color;
  return result;
}

#endif


#ifndef SHANDALAR
/* ManalinkEh 2022: 0x267f900; original functions/functions.c:6221. */
int get_sleighted_color(int player, int card, int color)
{
  card_instance_t *instance;
  color_t result;

  instance = get_card_instance(player, card);
  result = instance->color_id[color];
  if (!instance->color_id[color])
    result = color;
  return result;
}

#endif


#ifndef SHANDALAR
/* ManalinkEh 2022: 0x267f7a0; original functions/functions.c:6116. */
int get_colors_to_sleight_from_text(int csvid)
{
  char *txt = cards_ptr[csvid]->rules_text;
  int colors_to_sleight = 0;
  if (txt)
  {
    while ((txt = strstr(txt, "|S")))
    {
      txt += 2;
      int c;
      for (c = COLOR_BLACK; c <= COLOR_WHITE; ++c)
      {
        int i;
        for (i = 0; i < num_words_to_sleight; ++i)
        {
          int l = strlen(strs_words_to_sleight[c][i]);
          if (!strncmp(txt, strs_words_to_sleight[c][i], l))
          {
            txt += l;
            colors_to_sleight |= 1 << c;
            goto outer_continue;
          }
        }
      }
    outer_continue:;
    }
  }
  return colors_to_sleight;
}

#endif


#ifndef SHANDALAR
/* ManalinkEh 2022: 0x2680be0; original functions/functions.c:6546. */
int get_hacked_subtype(int player, int card, int orig_subtype)
{
  if (card < 0 || player < 0)
  {
    return orig_subtype;
  }

  static int subtypes[][4] = {
      {0, 0, 0, 0},
      {SUBTYPE_SWAMP, HARDCODED_SUBTYPE_SWAMP, COLOR_BLACK, CARD_ID_SWAMP},
      {SUBTYPE_ISLAND, HARDCODED_SUBTYPE_ISLAND, COLOR_BLUE, CARD_ID_ISLAND},
      {SUBTYPE_FOREST, HARDCODED_SUBTYPE_FOREST, COLOR_GREEN, CARD_ID_FOREST},
      {SUBTYPE_MOUNTAIN, HARDCODED_SUBTYPE_MOUNTAIN, COLOR_RED, CARD_ID_MOUNTAIN},
      {SUBTYPE_PLAINS, HARDCODED_SUBTYPE_PLAINS, COLOR_WHITE, CARD_ID_PLAINS}};

  int column, orig_color_t;

  switch (orig_subtype)
  {
  case SUBTYPE_SWAMP:
    column = 0;
    orig_color_t = COLOR_BLACK;
    break;
  case HARDCODED_SUBTYPE_SWAMP:
    column = 1;
    orig_color_t = COLOR_BLACK;
    break;
  case COLOR_BLACK:
    column = 2;
    orig_color_t = COLOR_BLACK;
    break;
  case CARD_ID_SWAMP:
    column = 3;
    orig_color_t = COLOR_BLACK;
    break;

  case SUBTYPE_ISLAND:
    column = 0;
    orig_color_t = COLOR_BLUE;
    break;
  case HARDCODED_SUBTYPE_ISLAND:
    column = 1;
    orig_color_t = COLOR_BLUE;
    break;
  case COLOR_BLUE:
    column = 2;
    orig_color_t = COLOR_BLUE;
    break;
  case CARD_ID_ISLAND:
    column = 3;
    orig_color_t = COLOR_BLUE;
    break;

  case SUBTYPE_FOREST:
    column = 0;
    orig_color_t = COLOR_GREEN;
    break;
  case HARDCODED_SUBTYPE_FOREST:
    column = 1;
    orig_color_t = COLOR_GREEN;
    break;
  case COLOR_GREEN:
    column = 2;
    orig_color_t = COLOR_GREEN;
    break;
  case CARD_ID_FOREST:
    column = 3;
    orig_color_t = COLOR_GREEN;
    break;

  case SUBTYPE_MOUNTAIN:
    column = 0;
    orig_color_t = COLOR_RED;
    break;
  case HARDCODED_SUBTYPE_MOUNTAIN:
    column = 1;
    orig_color_t = COLOR_RED;
    break;
  case COLOR_RED:
    column = 2;
    orig_color_t = COLOR_RED;
    break;
  case CARD_ID_MOUNTAIN:
    column = 3;
    orig_color_t = COLOR_RED;
    break;

  case SUBTYPE_PLAINS:
    column = 0;
    orig_color_t = COLOR_WHITE;
    break;
  case HARDCODED_SUBTYPE_PLAINS:
    column = 1;
    orig_color_t = COLOR_WHITE;
    break;
  case COLOR_WHITE:
    column = 2;
    orig_color_t = COLOR_WHITE;
    break;
  case CARD_ID_PLAINS:
    column = 3;
    orig_color_t = COLOR_WHITE;
    break;

  default:
    return orig_subtype;
  }

  return subtypes[get_hacked_color(player, card, orig_color_t)][column];
}

#endif


#ifndef SHANDALAR
/* ManalinkEh 2022: 0x26808c0; original functions/functions.c:6454. */
const char *get_hacked_land_text(int player, int card, const char *fmt, ...)
{
  static char buf[500];
  if (ai_is_speculating == 1)
  {
    *buf = 0;
    return buf;
  }

  static const char *words[][3] = {
      {"land", "Land", "LAND"},
      {"swamp", "Swamp", "SWAMP"},
      {"island", "Island", "ISLAND"},
      {"forest", "Forest", "FOREST"},
      {"mountain", "Mountain", "MOUNTAIN"},
      {"plains", "Plains", "PLAINS"},
      {"land", "Land", "LAND"},
      {"land", "Land", "LAND"},
  };

  va_list args;
  va_start(args, fmt);

  const char *p = fmt;
  char *q = buf;
  for (; (*q = *p); ++p, ++q)
  {
    if (*p == '%' && (*(p + 1) == 's' || *(p + 1) == 'a' || *(p + 1) == 'l'))
    {

      int hacked, orig_color_t, orig_subtype = va_arg(args, int);

      switch (orig_subtype)
      {
      case HARDCODED_SUBTYPE_SWAMP:
      case SUBTYPE_SWAMP:
      case COLOR_BLACK:
        orig_color_t = COLOR_BLACK;
        break;
      case HARDCODED_SUBTYPE_ISLAND:
      case SUBTYPE_ISLAND:
      case COLOR_BLUE:
        orig_color_t = COLOR_BLUE;
        break;
      case HARDCODED_SUBTYPE_FOREST:
      case SUBTYPE_FOREST:
      case COLOR_GREEN:
        orig_color_t = COLOR_GREEN;
        break;
      case HARDCODED_SUBTYPE_MOUNTAIN:
      case SUBTYPE_MOUNTAIN:
      case COLOR_RED:
        orig_color_t = COLOR_RED;
        break;
      case HARDCODED_SUBTYPE_PLAINS:
      case SUBTYPE_PLAINS:
      case COLOR_WHITE:
        orig_color_t = COLOR_WHITE;
        break;
      default:
        hacked = orig_color_t = 0;
        goto skip_hack;
      }
      hacked = (player >= 0 && card >= 0) ? get_hacked_color(player, card, orig_color_t)
                                          : orig_color_t;

    skip_hack:
      ++p;
      int article = *p == 'a';
      int lowercase = *p == 'l';

      if (article)
      {
        *q++ = 'a';
        if (hacked == COLOR_BLUE)
        {
          *q++ = 'n';
        }
        *q++ = ' ';
      }
      if (hacked != orig_color_t)
      {
        q += sprintf(q, "%s", words[hacked][2]) - 1;
      }
      else if (lowercase)
      {
        q += sprintf(q, "%s", words[hacked][0]) - 1;
      }
      else
      {
        q += sprintf(q, "%s", words[hacked][1]) - 1;
      }
    }
  }

  va_end(args);

  return buf;
}

#endif


#ifndef SHANDALAR
/* ManalinkEh 2022: 0x26807a0; original functions/functions.c:6426. */
int get_sleighted_protection(int player, int card, int orig_prot)
{
  switch (orig_prot)
  {
  case KEYWORD_PROT_BLACK:
    orig_prot = COLOR_BLACK;
    break;
  case KEYWORD_PROT_BLUE:
    orig_prot = COLOR_BLUE;
    break;
  case KEYWORD_PROT_GREEN:
    orig_prot = COLOR_GREEN;
    break;
  case KEYWORD_PROT_RED:
    orig_prot = COLOR_RED;
    break;
  case KEYWORD_PROT_WHITE:
    orig_prot = COLOR_WHITE;
    break;
  default:
  {
    if (orig_prot & KEYWORD_PROT_COLORED)
    {
      uint32_t col, new_prot = 0;
      for (col = COLOR_BLACK; col <= COLOR_WHITE; ++col)
      {
        if (orig_prot & (KEYWORD_PROT_BLACK << (col - 1)))
        {
          new_prot |= KEYWORD_PROT_BLACK << (get_sleighted_color(player, card, col) - 1);
        }
      }
      return (orig_prot & ~KEYWORD_PROT_COLORED) | new_prot;
    }
    return orig_prot;
  }
  }
  return KEYWORD_PROT_BLACK << (get_sleighted_color(player, card, orig_prot) - 1);
}

#endif


#ifndef SHANDALAR
/* ManalinkEh 2022: 0x26802f0; original functions/functions.c:6372. */
const char *get_sleighted_color_text2(int player, int card, const char *fmt,
                                           int orig_color_t_1, int orig_color_t_2)
{
  if (ai_is_speculating == 1)
  {
    return "";
  }

  static char buf[500];

  static const char *words[][2] = {
      {"colorless", "COLORLESS"}, {"black", "BLACK"}, {"blue", "BLUE"},
      {"green", "GREEN"},         {"red", "RED"},     {"white", "WHITE"},
      {"artifact", "ARTIFACT"},   {"any", "ANY"}};

  int sleighted1 = get_sleighted_color(player, card, orig_color_t_1);
  int sleighted2 = get_sleighted_color(player, card, orig_color_t_2);

  sprintf(buf, fmt, words[sleighted1][sleighted1 == orig_color_t_1 ? 0 : 1],
          words[sleighted2][sleighted2 == orig_color_t_2 ? 0 : 1]);

  return buf;
}

#endif


#ifndef SHANDALAR
/* ManalinkEh 2022: 0x2680270; original functions/functions.c:6346. */
const char *get_sleighted_color_text(int player, int card, const char *fmt, int orig_color_t)
{
  if (ai_is_speculating == 1)
  {
    return "";
  }

  static char buf[500];

  static const char *words[][2] = {
      {"colorless", "COLORLESS"}, {"black", "BLACK"}, {"blue", "BLUE"},
      {"green", "GREEN"},         {"red", "RED"},     {"white", "WHITE"},
      {"artifact", "ARTIFACT"},   {"any", "ANY"}};

  int sleighted = get_sleighted_color(player, card, orig_color_t);

  sprintf(buf, fmt, words[sleighted][sleighted == orig_color_t ? 0 : 1]);

  return buf;
}

#endif


#ifndef SHANDALAR
/* ManalinkEh 2022: 0x26803c0; original functions/functions.c:6398. */
int get_sleighted_color_test(int player, int card, int orig_color_test)
{
  if (card < 0 || player < 0)
  {
    return orig_color_test;
  }
  switch (orig_color_test)
  {
  case COLOR_TEST_BLACK:
    orig_color_test = COLOR_BLACK;
    break;
  case COLOR_TEST_BLUE:
    orig_color_test = COLOR_BLUE;
    break;
  case COLOR_TEST_GREEN:
    orig_color_test = COLOR_GREEN;
    break;
  case COLOR_TEST_RED:
    orig_color_test = COLOR_RED;
    break;
  case COLOR_TEST_WHITE:
    orig_color_test = COLOR_WHITE;
    break;
  default:
  {
    if (orig_color_test & COLOR_TEST_ANY_COLORED)
    {
      uint32_t col, new_color_test = 0;
      for (col = COLOR_BLACK; col <= COLOR_WHITE; ++col)
      {
        if (orig_color_test & (COLOR_TEST_BLACK << (col - 1)))
        {
          new_color_test |= COLOR_TEST_BLACK << (get_sleighted_color(player, card, col) - 1);
        }
      }
      return (orig_color_test & ~COLOR_TEST_ANY_COLORED) | new_color_test;
    }
    return orig_color_test;
  }
  }
  return COLOR_TEST_BLACK << (get_sleighted_color(player, card, orig_color_test) - 1);
}

#endif


#ifndef SHANDALAR
/* ManalinkEh 2022: 0x2680da0; original functions/functions.c:6703. */
int color_to_color_test(int color)
{
  if (color == COLOR_ANY)
    return COLOR_TEST_ANY_COLORED;
  return (int)(1U << ((unsigned int)color & 31));
}

#endif


#ifndef SHANDALAR
/* ManalinkEh 2022: 0x267c3a0; original functions/functions.c:2935. */
int get_global_color_hack(int player)
{
  unsigned int timestamp = 0;
  int result = 0;
  for (int p = 0; p < 2; ++p)
    for (int c = 0; c < active_cards_count[p]; ++c)
      if (in_play(p, c))
      {
        if (get_id(p, c) == CARD_ID_PAINTERS_SERVANT)
        {
          card_instance_t *instance = get_card_instance(p, c);
          if (instance->targets[1].card >= 0 && instance->timestamp > timestamp)
          {
            result |= instance->targets[1].card;
            timestamp = instance->timestamp;
          }
        }
        if (get_id(p, c) == CARD_ID_MYCOSYNTH_LATTICE)
        {
          card_instance_t *instance = get_card_instance(p, c);
          if (instance->timestamp > timestamp)
          {
            result = COLOR_TEST_COLORLESS;
            timestamp = instance->timestamp;
          }
        }
      }
  return result;
}

#endif


#ifndef SHANDALAR
/* ManalinkEh 2022: 0x267ea70; original functions/functions.c:4094. */
int get_color(int player, int card)
{
  int result = get_abilities(player, card, EVENT_SET_COLOR, -1);
  int half_played = get_card_instance(player, card)->info_slot;
  int csvid = get_id(player, card);
  int split_csvid = split_cards_id_table(csvid, half_played);
  int iid = get_internal_card_id_from_csv_id(split_csvid);
  if (cards_data[iid].color)
    result = cards_data[iid].color;
  return result;
}

#endif


#ifndef SHANDALAR
/* ManalinkEh 2022: 0x267eaf0; original functions/functions.c:4103. */
int get_color_by_internal_id_no_hack(int internal_id)
{
  card_data_t *data = &cards_data[internal_id];
  int color = data->color;
  if (data->type == TYPE_LAND)
  {
    if (data->id == CARD_ID_DRYAD_ARBOR)
      color = COLOR_TEST_GREEN;
    else if (data->id == 0x5186)
      color = COLOR_TEST_BLUE;
    else
      color = 0;
  }
  else if ((data->type & TYPE_ARTIFACT) && data->cc[0] == 0)
    color = 0;
  if ((data->extra_ability & EA_MANA_SOURCE) && data->color != COLOR_TEST_COLORLESS)
    color &= COLOR_TEST_ANY_COLORED;
  return color;
}

#endif


#ifndef SHANDALAR
/* ManalinkEh 2022: 0x26804d0; original functions/functions.c:4124. */
int get_color_by_internal_id(int player, int internal_id)
{
  unsigned int timestamp = 0;
  int changer = -1;
  int hack = 0;
  for (int p = 0; p < 2; ++p)
    for (int c = 0; c < active_cards_count[p]; ++c)
      if (in_play(p, c) && !is_humiliated(p, c, AL_STATIC))
      {
        if (get_id(p, c) == CARD_ID_PAINTERS_SERVANT)
        {
          card_instance_t *instance = get_card_instance(p, c);
          if (instance->targets[1].card >= 0 && instance->timestamp > timestamp)
          {
            hack |= instance->targets[1].card;
            changer = CARD_ID_PAINTERS_SERVANT;
            timestamp = instance->timestamp;
          }
        }
        if (p == player && get_id(player, c) == CARD_ID_CELESTIAL_DAWN)
        {
          card_instance_t *instance = get_card_instance(player, c);
          unsigned int next_timestamp = instance->timestamp;
          if (next_timestamp > timestamp)
          {
            hack = get_sleighted_color_test(player, c, COLOR_TEST_WHITE);
            changer = CARD_ID_CELESTIAL_DAWN;
            timestamp = next_timestamp;
          }
        }
        if (get_id(p, c) == CARD_ID_MYCOSYNTH_LATTICE)
        {
          card_instance_t *instance = get_card_instance(p, c);
          if (instance->timestamp > timestamp)
          {
            hack = COLOR_TEST_COLORLESS;
            changer = CARD_ID_MYCOSYNTH_LATTICE;
            timestamp = instance->timestamp;
          }
        }
      }
  int color = get_color_by_internal_id_no_hack(internal_id);
  if (changer == CARD_ID_MYCOSYNTH_LATTICE)
    return hack;
  if (changer == CARD_ID_PAINTERS_SERVANT)
    return color | hack;
  if (changer == CARD_ID_CELESTIAL_DAWN && !(cards_data[internal_id].type & TYPE_LAND))
    return hack;
  return color;
}

#endif


#ifndef SHANDALAR
/* ManalinkEh 2022: 0x26806c0; original functions/functions.c:4185. */
int get_color_real_or_iid(int player_or_owner, int player_or_negone, int card_or_iid)
{
  return (player_or_negone >= 0 ? get_color(player_or_negone, card_or_iid)
                                : get_color_by_internal_id(player_or_owner, card_or_iid));
}

#endif


#ifndef SHANDALAR
/* ManalinkEh 2022: 0x2682fe0; original functions/functions.c:4973. */
int legacy_change_color(int player, int card, event_t event)
{
  card_instance_t *instance = get_card_instance(player, card);
  int p = instance->targets[0].player;
  int c = instance->targets[0].card;

  if (instance->targets[1].player > 0)
  {
    if (event == EVENT_SET_COLOR && affect_me(p, c))
    {
      if (instance->targets[1].player & CHANGE_COLOR_ADD)
      {
        event_result |= instance->targets[1].card;
      }
      if (instance->targets[1].player & CHANGE_COLOR_SET)
      {
        event_result = instance->targets[1].card;
      }
    }
    if ((instance->targets[1].player & CHANGE_COLOR_END_AT_EOT) && eot_trigger(player, card, event))
    {
      kill_card(player, card, KILL_EXILE);
    }
  }

  return 0;
}

#endif


#ifndef SHANDALAR
/* ManalinkEh 2022: 0x26806f0; original functions/functions.c:4995. */
void change_color(int player, int card, int t_player, int t_card, color_test_t new_color,
                       change_color_t mode)
{

  int legacy = create_targetted_legacy_effect_no_repeat(player, card, &legacy_change_color,
                                                        t_player, t_card);
  card_instance_t *instance = get_card_instance(player, legacy);
  instance->targets[1].player = mode;
  if (!(mode & CHANGE_COLOR_NO_SLEIGHT))
  {
    new_color = get_sleighted_color_test(player, card, new_color);
  }
  instance->targets[1].card = new_color;
  if (!(mode & CHANGE_COLOR_NO_SOUND))
  {
    play_sound_effect(WAV_CHANGEC);
  }
}

#endif


#ifndef SHANDALAR
/* ManalinkEh 2022: 0x2680af0; original functions/functions.c:6521. */
int get_hacked_walk(int player, int card, int orig_walk)
{
  switch (orig_walk)
  {
  case KEYWORD_SWAMPWALK:
    orig_walk = COLOR_BLACK;
    break;
  case KEYWORD_ISLANDWALK:
    orig_walk = COLOR_BLUE;
    break;
  case KEYWORD_FORESTWALK:
    orig_walk = COLOR_GREEN;
    break;
  case KEYWORD_MOUNTAINWALK:
    orig_walk = COLOR_RED;
    break;
  case KEYWORD_PLAINSWALK:
    orig_walk = COLOR_WHITE;
    break;
  default:
  {
    if (orig_walk & KEYWORD_BASIC_LANDWALK)
    {
      uint32_t col, new_walk = 0;
      for (col = COLOR_BLACK; col <= COLOR_WHITE; ++col)
      {
        if (orig_walk & (KEYWORD_SWAMPWALK << (col - 1)))
        {
          new_walk |= KEYWORD_SWAMPWALK << (get_hacked_color(player, card, col) - 1);
        }
      }
      return (orig_walk & ~KEYWORD_BASIC_LANDWALK) | new_walk;
    }
    return orig_walk;
  }
  }
  return KEYWORD_SWAMPWALK << (get_hacked_color(player, card, orig_walk) - 1);
}

#endif


#ifndef SHANDALAR
/* ManalinkEh 2022: 0x2680dc0; original functions/functions.c:6709. */
int keyword_to_color(int keyword)
{

  int i;
  for (i = 0; i < 5; i++)
  {
    if ((1 << (11 + i)) == keyword)
    {
      return i + 1;
    }
  }
  return 0;
}

#endif


#ifndef SHANDALAR
/* ManalinkEh 2022: 0x2678330; original functions/functions.c:6173. */
static int get_colors_to_hack_from_type_text(int csvid)
{
  const char *txt = cards_ptr[csvid]->type_text;
  int colors_to_sleight = 0;
  if (txt)
  {
    while ((txt = strstr(txt, "|H")))
    {
      txt += 2;
      int c;
      for (c = COLOR_BLACK; c <= COLOR_WHITE; ++c)
      {
        int i;
        for (i = 0; i < num_words_to_hack; ++i)
        {
          int l = strlen(strs_words_to_hack[c][i]);
          if (!strncmp(txt, strs_words_to_hack[c][i], l))
          {
            txt += l;
            colors_to_sleight |= 1 << c;
            goto outer_continue;
          }
        }
      }
    outer_continue:;
    }
  }
  return colors_to_sleight;
}

#endif


#ifndef SHANDALAR
/* ManalinkEh 2022: 0x2678430; original functions/functions.c:6145. */
static int get_colors_to_hack_from_rules_text(int csvid)
{
  char *txt = cards_ptr[csvid]->rules_text;
  int colors_to_sleight = 0;
  if (txt)
  {
    while ((txt = strstr(txt, "|H")))
    {
      txt += 2;
      int c;
      for (c = COLOR_BLACK; c <= COLOR_WHITE; ++c)
      {
        int i;
        for (i = 0; i < num_words_to_hack; ++i)
        {
          int l = strlen(strs_words_to_hack[c][i]);
          if (!strncmp(txt, strs_words_to_hack[c][i], l))
          {
            txt += l;
            colors_to_sleight |= 1 << c;
            goto outer_continue;
          }
        }
      }
    outer_continue:;
    }
  }
  return colors_to_sleight;
}

#endif


#ifndef SHANDALAR
/* ManalinkEh 2022: 0x267f8a0; original functions/functions.c:6201. */
int get_colors_to_hack_from_csvid(int csvid)
{
  int result = get_colors_to_hack_from_type_text(csvid);
  result |= get_colors_to_hack_from_rules_text(csvid);
  return result;
}

#endif


#ifndef SHANDALAR
/* ManalinkEh 2022: 0x267f6a0; original functions/functions.c:5991. */
int is_colorless(int player, int card)
{
  if (get_color(player, card) < 2)
  {
    return 1;
  }
  return 0;
}

#endif


#ifndef SHANDALAR
/* ManalinkEh 2022: 0x2688770; original functions/functions.c:8990. */
int colors_shared(int clr1, int clr2) { return clr1 & clr2 & COLOR_TEST_ANY_COLORED; }

#endif


#ifndef SHANDALAR
/* ManalinkEh 2022: 0x2688780; original functions/functions.c:8994. */
int has_my_colors(int player, int card, int t_player, int t_card)
{

  int clrs[2];
  if (player != -1)
  {
    clrs[0] = get_color(player, card);
  }
  else
  {
    clrs[0] = cards_data[card].color;
  }
  if (t_player != -1)
  {
    clrs[1] = get_color(t_player, t_card);
  }
  else
  {
    clrs[1] = cards_data[t_card].color;
  }

  return colors_shared(clrs[0], clrs[1]);
}

#endif


#ifndef SHANDALAR
/* ManalinkEh 2022: 0x268c860; original functions/functions.c:11862. */
int real_count_colors(int clr)
{
  return num_bits_set((unsigned int)clr & COLOR_TEST_ANY_COLORED);
}

#endif


#ifndef SHANDALAR
/* ManalinkEh 2022: 0x268c8b0; original functions/functions.c:11867. */
int count_colors(int player, int card)
{

  int clr;
  if (player != -1)
  {
    clr = get_color(player, card);
  }
  else
  {
    clr = cards_data[card].color;
    if (cards_data[card].type == TYPE_LAND)
    {
      return cards_data[card].id == CARD_ID_DRYAD_ARBOR ? 1 : 0;
    }
  }
  return real_count_colors(clr);
}

#endif


#ifndef SHANDALAR
/* ManalinkEh 2022: 0x268c950; original functions/functions.c:11882. */
int evaluate_colors(int color1, int color2)
{
  if (color2 != COLOR_TEST_COLORLESS)
  {
    int i;
    for (i = 1; i < 6; i++)
    {
      if (!(color1 & (1 << i)) && (color2 & (1 << i)))
      {
        return 0;
      }
    }
  }
  return 1;
}

#endif


#ifndef SHANDALAR
/* ManalinkEh 2022: 0x268b5f0; original functions/functions.c:11021. */
int is_monocolor_hybrid(card_ptr_t *cp)
{
  const char *c = cp->mana_cost_text;
  return c && c[0] == '|' && c[1] == '2' &&
         (c[2] == 'B' || c[2] == 'U' || c[2] == 'G' || c[2] == 'R' || c[2] == 'W');
}

#endif


#ifndef SHANDALAR
/* ManalinkEh 2022: 0x26845a0; original functions/functions.c:7194. */
int get_deck_color(int player, int whose_deck)
{
  if (player >= 0 && !IS_AI(player) && player != whose_deck)
  {
    return COLOR_BLACK;
  }
  int *deck = deck_ptr[whose_deck];
  int count = count_deck(whose_deck) - 1;
  int clrs[5] = {0, 0, 0, 0, 0};
  while (count > -1)
  {
    int color = cards_data[deck[count]].color;
    if ((color & COLOR_TEST_ANY_COLORED) && !is_what(-1, deck[count], TYPE_LAND))
    {
      int i;
      for (i = COLOR_BLACK; i <= COLOR_WHITE; i++)
      {
        if (color & (1 << i))
        {
          clrs[i - 1]++;
        }
      }
    }
    count--;
  }
  if (player != -1)
  {
    int ai_choice = 1;
    int i;
    int par = -1;
    for (i = COLOR_BLACK; i <= COLOR_WHITE; i++)
    {
      if (clrs[i - 1] > par)
      {
        par = clrs[i - 1];
        ai_choice = i;
      }
    }
    return ai_choice;
  }
  else
  {
    int clrs_res[2] = {0, 0};
    int par = -1;
    int i;
    for (i = COLOR_BLACK; i <= COLOR_WHITE; i++)
    {
      if (clrs[i - 1] > par)
      {
        par = clrs[i - 1];
        clrs_res[0] = i;
      }
    }
    par = -1;
    for (i = COLOR_BLACK; i <= COLOR_WHITE; i++)
    {
      if (clrs[i - 1] > par && i != clrs_res[0])
      {
        par = clrs[i - 1];
        clrs_res[1] = i;
      }
    }
    return (1 << clrs_res[0]) | (1 << clrs_res[1]);
  }
}

#endif


#ifndef SHANDALAR
/* ManalinkEh 2022: 0x267c190; original functions/functions.c:2828. */
int choose_a_color(int player, int ai_choice)
{
  int result = -1;
  while (result == -1)
  {
    result = choose_a_color_exe(player, "Choose a color", 1, ai_choice, COLOR_TEST_ANY_COLORED);
  }
  return result;
}

#endif


#ifndef SHANDALAR
/* ManalinkEh 2022: 0x267c1d0; original functions/functions.c:2836. */
int choose_a_color_from(int player, int ai_choice, color_test_t available_colors)
{
  int result = -1;
  while (result == -1)
  {
    result = choose_a_color_exe(player, "Choose a color", 1, ai_choice, available_colors);
  }
  return result;
}

#endif


#ifndef SHANDALAR
/* ManalinkEh 2022: 0x2684730; original functions/functions.c:6720. */
int select_a_color(int player)
{

  int ai_choice = 0;

  if (player == AI)
  {
    ai_choice = get_deck_color(player, 1 - player);
  }

  int color = choose_a_color(player, ai_choice);
  int keyword = color_to_color_test(color);

  return keyword;
}

#endif


#ifndef SHANDALAR
/* ManalinkEh 2022: 0x26798f0; original functions/functions.c:197. */
const char *get_card_name_by_id(int csvid) { return cards_ptr[csvid]->name; }

#endif


#ifndef SHANDALAR
/* ManalinkEh 2022: 0x2679910; original functions/functions.c:202. */
const char *get_card_name_by_instance(card_instance_t *instance)
{
  return cards_ptr[cards_data[instance->internal_card_id].id]->name;
}

#endif


#ifndef SHANDALAR
/* ManalinkEh 2022: 0x2679940; original functions/functions.c:207. */
const char *get_card_or_subtype_name(int subtype_or_csvid)
{
  if (subtype_or_csvid < 0)
    return cards_ptr[-(subtype_or_csvid + 1)]->name;
  else if (subtype_or_csvid <= 232)
    return strs_hunting_subtypenames[subtype_or_csvid];
  else
    return "";
}

#endif


#ifndef SHANDALAR
/* ManalinkEh 2022: 0x26786e0; original functions/functions.c:15749. */
static int subtype_name_legacy(int player, int card, event_t event)
{
  card_instance_t *instance = get_card_instance(player, card);
  if (event == EVENT_SET_LEGACY_EFFECT_NAME && instance->targets[0].player > -1)
  {
    scnprintf(set_legacy_effect_name_addr, 51, "%s",
              raw_get_subtype_text(instance->targets[0].player));
  }
  if (instance->targets[0].card == 2 && event == EVENT_CLEANUP)
  {
    kill_card(player, card, KILL_EXILE);
  }
  return 0;
}

#endif


#ifndef SHANDALAR
/* ManalinkEh 2022: 0x2678770; original functions/functions.c:19495. */
static int legacy_name_with_rfg_check(int player, int card, event_t event)
{

  if (event == EVENT_SET_LEGACY_EFFECT_NAME)
  {
    card_instance_t *instance = get_card_instance(player, card);
    if (instance->targets[1].card > -1)
    {
      scnprintf(set_legacy_effect_name_addr, 51, "%s",
                cards_ptr[cards_data[instance->targets[1].card].id]->name);
    }
  }

  if (event == EVENT_STATIC_EFFECTS && affect_me(player, card))
  {
    card_instance_t *instance = get_card_instance(player, card);
    if (instance->targets[1].card > -1 && instance->targets[1].player > -1)
    {
      if (!check_rfg(instance->targets[1].player, cards_data[instance->targets[1].card].id))
      {
        kill_card(player, card, KILL_EXILE);
      }
    }
  }

  return 0;
}

#endif


#ifndef SHANDALAR
/* ManalinkEh 2022: 0x2693c00; original functions/functions.c:15734. */
int create_card_name_legacy(int player, int card, int id)
{
  int legacy_card = create_legacy_effect_exe(player, card, LEGACY_EFFECT_ASWAN, player, card);
  card_instance_t *legacy = get_card_instance(player, legacy_card);
  legacy->info_slot = -id - 1;
  return legacy_card;
}

#endif


#ifndef SHANDALAR
/* ManalinkEh 2022: 0x2693f40; original functions/functions.c:15767. */
int create_hardcoded_subtype_name_legacy(int player, int card, int subtype)
{
  int legacy_card = create_legacy_effect_exe(player, card, LEGACY_EFFECT_ASWAN, player, card);
  card_instance_t *legacy = get_card_instance(player, legacy_card);
  legacy->info_slot = subtype;
  return legacy_card;
}

#endif


#ifndef SHANDALAR
/* ManalinkEh 2022: 0x2693ee0; original functions/functions.c:15760. */
int create_subtype_name_legacy(int player, int card, int subtype)
{
  int legacy_card =
      create_targetted_legacy_effect(player, card, &subtype_name_legacy, player, card);
  get_card_instance(player, legacy_card)->targets[0].player = subtype;
  get_card_instance(player, legacy_card)->eot_toughness = EVENT_SET_LEGACY_EFFECT_NAME;
  return legacy_card;
}

#endif


#ifndef SHANDALAR
/* ManalinkEh 2022: 0x26a6fe0; original functions/functions.c:19518. */
int create_legacy_name_with_rfg_check(int player, int card, int owner, int iid)
{

  int legacy_card =
      create_targetted_legacy_effect(player, card, &legacy_name_with_rfg_check, player, card);
  get_card_instance(player, legacy_card)->targets[1].player = owner;
  get_card_instance(player, legacy_card)->targets[1].card = iid;
  get_card_instance(player, legacy_card)->eot_toughness = EVENT_SET_LEGACY_EFFECT_NAME;
  return legacy_card;
}

#endif


#ifndef SHANDALAR
/* ManalinkEh 2022: 0x269f930; original functions/functions.c:18357. */
int get_card_name(int player, int card)
{
  int csvid = player == -1 ? cards_data[card].id : get_id(player, card);
  if (player != -1)
  {

    if (cards_data[get_card_instance(player, card)->original_internal_card_id].id ==
        CARD_ID_SAKASHIMA_THE_IMPOSTOR)
    {
      return CARD_ID_SAKASHIMA_THE_IMPOSTOR;
    }
    if (cards_data[get_card_instance(player, card)->original_internal_card_id].id ==
        CARD_ID_LAZAV_DIMIR_MASTERMIND)
    {
      return CARD_ID_LAZAV_DIMIR_MASTERMIND;
    }
    if (get_id(player, card) == CARD_ID_DARKSTEEL_MUTATION_INSECT)
    {
      return cards_data[get_card_instance(player, card)->targets[1].card].id;
    }

    if (get_id(player, card) == CARD_ID_WASTES)
    {
      int iid = get_card_instance(player, card)->targets[1].card;
      return iid == -1 ? CARD_ID_WASTES : cards_data[iid].id;
    }
  }
  if (csvid == CARD_ID_FACE_DOWN_CREATURE)
  {
    return -1;
  }
  return csvid;
}

#endif


#ifndef SHANDALAR
/* ManalinkEh 2022: 0x269fa60; original functions/functions.c:18382. */
int has_same_name(int player, int card, int player2, int card2)
{
  int id1 = get_card_name(player, card);
  int id2 = get_card_name(player2, card2);
  if (id1 == -1 || id2 == -1)
  {
    return 0;
  }

  if (id1 == CARD_ID_RIPTIDE_REPLICATOR_TOKEN && id2 == CARD_ID_RIPTIDE_REPLICATOR_TOKEN)
  {
    if (get_card_instance(player, card)->targets[1].card != -1 &&
        get_card_instance(player2, card2)->targets[1].card != -1 &&
        get_card_instance(player, card)->targets[1].card ==
            get_card_instance(player2, card2)->targets[1].card)
    {
      return 1;
    }
    return 0;
  }
  if (id1 == id2)
  {
    return 1;
  }
  return 0;
}

#endif


#ifndef SHANDALAR
/* ManalinkEh 2022: 0x269fb40; original functions/functions.c:18408. */
void damage_creature_and_all_the_others_with_the_same_name(int player, int card, int t_player,
                                                                int t_card, int dmg)
{
  int id = get_card_name(t_player, t_card);
  if (id != -1)
  {
    test_definition_t this_test;
    default_test_definition(&this_test, TYPE_CREATURE);
    this_test.id = id;
    new_damage_all(player, card, ANYBODY, dmg, 0, &this_test);
  }
  else
    damage_creature(t_player, t_card, dmg, player, card);
}

#endif


#ifndef SHANDALAR
/* ManalinkEh 2022: 0x269fbf0; original functions/functions.c:18421. */
int tutor_card_with_the_same_name(int player, int card, int who_tutors, int t_player,
                                       int tutor_from, int tutor_to, int must_tutor)
{
  int id = get_card_name(player, card);

  if (id == CARD_ID_SPLINTER_TOKEN)
  {
    id = CARD_ID_SPLINTER;
  }
  char msg[100];
  if (id == -1)
  {
    scnprintf(msg, 100,
              "You won't be able to tutor any card as you're searching for a card with no name.");
  }
  else
  {
    scnprintf(msg, 100, "Select a card named %s", cards_ptr[id]->name);
  }
  test_definition_t this_test;
  new_default_test_definition(&this_test, TYPE_ANY, msg);
  this_test.id = id;
  int result = new_global_tutor(who_tutors, t_player, tutor_from, tutor_to, must_tutor,
                                     AI_FIRST_FOUND, &this_test);
  return result;
}

#endif


#ifndef SHANDALAR
/* ManalinkEh 2022: 0x269fcf0; original functions/functions.c:18441. */
int count_permanents_by_name(int player, type_t type, int csvid)
{
  if (csvid == -1)
    return 0;
  int result = 0;
  int p = current_turn;
  for (int c = active_cards_count[p] - 1; c >= 0; --c)
    if ((p == player || player == ANYBODY) && in_play(p, c) && is_what(p, c, type))
      result += get_card_name(p, c) == csvid;
  p = 1 - current_turn;
  for (int c = active_cards_count[p] - 1; c >= 0; --c)
    if ((player == ANYBODY || p == player) && in_play(p, c) && is_what(p, c, type))
      result += get_card_name(p, c) == csvid;
  return result;
}

#endif


#ifndef SHANDALAR
/* ManalinkEh 2022: 0x26a5250; original functions/functions.c:19023. */
int ai_name_a_random_csvid(int player, int card, int t_player, type_t type,
                                test_definition_t *test)
{
  int csvid = CARD_ID_LIGHTNING_BOLT;
  for (int rounds = 0; rounds < 10; ++rounds)
  {
    int count = internal_rand(count_deck(t_player));
    int iid = deck_ptr[t_player][count];
    if (iid != -1 &&
        (type ? is_what(-1, iid, type) : test && new_make_test(t_player, iid, -1, test)))
    {
      csvid = cards_data[deck_ptr[t_player][count]].id;
      break;
    }
  }
  char buffer[100];
  scnprintf(buffer, 100, "Opponent named %s", cards_ptr[csvid]->name);
  do_dialog(1 - player, player, card, -1, -1, buffer, 0);
  return csvid;
}

#endif


#ifndef SHANDALAR
/* ManalinkEh 2022: 0x268edc0; original functions/functions.c:13955. */
int name_a_card(int player, int ai_player, test_definition_t *test, int mode)
{
  if (ai_is_speculating != 1 && (player != AI || (trace_mode & 2)))
    return new_card_from_list(player, mode, test);
  int selected = new_select_a_card(player, ai_player, 1, 7, 0, -1, test);
  int iid;
  if (selected != -1)
    iid = deck_ptr[ai_player][selected];
  else
  {
    selected = new_select_a_card(player, ai_player, 2, 7, 0, -1, test);
    if (selected != -1)
      iid = graveyard_ptr_mutable[ai_player][selected];
    else
    {
      selected = new_select_a_card(player, ai_player, 3, 7, 0, -1, test);
      if (selected == -1)
        return -1;
      iid = rfg_ptr[ai_player][selected];
    }
  }
  return mode & 2 ? cards_data[iid].id : iid;
}

#endif


#ifndef SHANDALAR
/* ManalinkEh 2022: 0x2678c50; original functions/functions.c:15710. */
static void alternate_legacy_text_impl(int textnum, int player, int legacycard)
{
  card_instance_t *instance = get_card_instance(player, legacycard);
  if (cards_data[instance->internal_card_id].id != 902 &&
      cards_data[instance->internal_card_id].id != 903)
    backtrace_and_abort("functions/functions.c:15713\n(cards_data[instance->internal_card_id].id "
                        "== 902 || cards_data[instance->internal_card_id].id == 903) && "
                        "\"alternate_legacy_text only works on csvid 902 or 903 effects\"");
  if (instance->internal_card_id == LEGACY_EFFECT_PUMP)
    backtrace_and_abort(
        "functions/functions.c:15714\ninstance->internal_card_id != LEGACY_EFFECT_PUMP && "
        "\"alternate_legacy_text doesn't work on LEGACY_EFFECT_PUMP\"");
  instance->eot_toughness = (instance->eot_toughness & ~0x300) | textnum;
}

#endif


#ifndef SHANDALAR
/* ManalinkEh 2022: 0x2693bc0; original functions/functions.c:15710. */
int alternate_legacy_text(int textnum, int player, int legacycard)
{
  if (legacycard >= 0)
    alternate_legacy_text_impl(textnum, player, legacycard);
  return legacycard;
}

#endif


#ifndef SHANDALAR
/* ManalinkEh 2022: 0x2678d10; original functions/functions.c:16525. */
static void type_text_impl(char *buf, int maxlen, type_t type, type_text_t flags)
{

  const char *types[17] = {0};
  int num_types = 0;

  load_text(0, "TYPE");

  if (!(flags & TYPETEXT_NO_ABBREV))
  {

    if ((type & (TYPE_INSTANT | TYPE_INTERRUPT)) == (TYPE_INSTANT | TYPE_INTERRUPT))
      type &= ~TYPE_INTERRUPT;

    if ((type & (TYPE_PERMANENT | TYPE_INSTANT | TYPE_SORCERY)) ==
        (TYPE_PERMANENT | TYPE_INSTANT | TYPE_SORCERY))
    {
      type &= ~(TYPE_NONEFFECT | TARGET_TYPE_PLANESWALKER);
      types[num_types++] = text_lines[2];
    }

    if ((type & TYPE_PERMANENT) == TYPE_PERMANENT)
    {
      type &= ~(TYPE_PERMANENT | TARGET_TYPE_PLANESWALKER);
      types[num_types++] = text_lines[9];
    }
  }

#define TYPE(typ, idx)                                                                             \
  if (type & typ)                                                                                  \
  types[num_types++] = text_lines[idx]
  TYPE(TYPE_ARTIFACT, 1);
  TYPE(TYPE_CREATURE, 3);
  TYPE(TYPE_ENCHANTMENT, 5);
  TYPE(TYPE_INSTANT, 6);
  TYPE(TYPE_INTERRUPT, 7);
  TYPE(TYPE_LAND, 8);
  TYPE(TARGET_TYPE_PLANESWALKER, 12);
  TYPE(TYPE_SORCERY, 14);

  sort_ArrayConstCharPtr(types, num_types);

  TYPE(TYPE_EFFECT, 4);

  if (num_types == 0)
    types[num_types++] = text_lines[0];

  const char *non_txt = (flags & TYPETEXT_INVERT) ? "non" : "";

  if (num_types == 1)
    scnprintf(buf, maxlen, "%s%s", non_txt, types[0]);
  else
  {
    int i;
    char *p = buf;
    int end = num_types - ((flags & TYPETEXT_INVERT) ? 1 : 2);
    for (i = 0; i < end; ++i)
      p += scnprintf(p, maxlen - (p - buf), "%s%s, ", non_txt, types[i]);

    if (flags & TYPETEXT_INVERT)
      scnprintf(p, maxlen - (p - buf), "%s%s", non_txt, types[num_types - 1]);
    else
      scnprintf(p, maxlen - (p - buf), "%s or %s", types[num_types - 2], types[num_types - 1]);
  }

  return;
#undef TYPE
}

#endif


#ifndef SHANDALAR
/* ManalinkEh 2022: 0x26965d0; original functions/functions.c:16525. */
char *type_text(char *buf, int maxlen, type_t type, type_text_t flags)
{
  if (ai_is_speculating == 1)
    *buf = 0;
  else
    type_text_impl(buf, maxlen, type, flags);
  return buf;
}

#endif


#ifndef SHANDALAR
/* ManalinkEh 2022: 0x2678530; original functions/functions.c:19535. */
static const char *has_activated_ability(int who_chooses, int player, int card)
{
  int iid = get_card_instance(player, card)->internal_card_id;
  return has_an_activated_ability(iid) ? NULL : "AI helper function";
}

#endif


#ifndef SHANDALAR
/* ManalinkEh 2022: 0x26a70d0; original functions/functions.c:19544. */
int select_permanent_to_disable(int player, int card, target_definition_t *td,
                                     const char *prompt, test_definition_t *test)
{
  td->allow_cancel = 0;
  td->illegal_abilities = 0;
  if (IS_AI(player))
  {
    td->special = TARGET_SPECIAL_EXTRA_FUNCTION;
    td->extra = (int32_t)has_activated_ability;
  }

  card_instance_t *instance = get_card_instance(player, card);

  int choice = 1;
  if (IS_AI(player))
  {
    state_untargettable(player, card, 1);
  }
  if (can_target(td))
  {
    choice = do_dialog(player, player, card, -1, -1,
                       " Select a card in play\n Choose a card from a list", 0);
  }
  int csvid = 0;
  if (choice == 0)
  {
    if (new_pick_target(td, prompt, 0, GS_LITERAL_PROMPT))
    {
      csvid = get_card_name(instance->targets[0].player, instance->targets[0].card);
    }
  }
  else
  {
    if (IS_AI(player))
    {
      csvid = ai_name_a_random_csvid(player, card, 1 - player, 0, test);
    }
    else
    {
      csvid = new_card_from_list(player, 3, test);
    }
  }
  if (IS_AI(player))
  {
    state_untargettable(player, card, 0);
  }
  return csvid;
}

#endif


#ifndef SHANDALAR
/* ManalinkEh 2022: 0x26a7330; original functions/functions.c:19582. */
void activated_abilities_disabler_enters_bf(int player, int card, int action)
{
  int csvid = get_card_instance(player, card)->info_slot;
  if (csvid > 0)
    create_card_name_legacy(player, card, csvid);
}

#endif


#ifndef SHANDALAR
/* ManalinkEh 2022: 0x26a7370; original functions/functions.c:19590. */
int activated_abilities_disabler(int player, int card, event_t event, int action)
{
  if (event == EVENT_ABILITIES && !affect_me(player, card) &&
      !is_humiliated(player, card, AL_STATIC))
  {
    int csvid = get_card_instance(player, card)->info_slot;
    if (csvid > 0 && get_id(affected_card_controller, affected_card) ==
                         get_card_instance(player, card)->info_slot)
      action_on_card(player, card, affected_card_controller, affected_card, action);
  }
  return 0;
}

#endif


#ifndef SHANDALAR
/* ManalinkEh 2022: 0x26a7050; original functions/functions.c:19528. */
void ai_modifier_for_land_untappers(int player, int card, int target_pos)
{
  card_instance_t *instance = get_card_instance(player, card);
  int iid =
      get_card_instance(instance->targets[target_pos].player, instance->targets[target_pos].card)
          ->internal_card_id;
  ai_modifier +=
      (is_tapped(instance->targets[target_pos].player, instance->targets[target_pos].card)
           ? cards_ptr[cards_data[iid].id]->ai_base_value
           : -48);
}

#endif


#ifndef SHANDALAR
/* ManalinkEh 2022: 0x26a6f40; original functions/functions.c:19479. */
void ai_modifier_for_haste_granting_effects(int player, int card)
{
  card_instance_t *instance = get_card_instance(player, card);
  int mod = -48;
  if (is_sick(instance->targets[0].player, instance->targets[0].card))
  {
    mod = 0;
    if (current_turn == player && current_phase < PHASE_DECLARE_ATTACKERS)
    {
      mod += 48;
    }
    int iid =
        get_card_instance(instance->targets[0].player, instance->targets[0].card)->internal_card_id;
    if (cards_data[iid].extra_ability & EA_ACT_ABILITY)
    {
      mod += 24;
    }
  }
  ai_modifier += mod;
}

#endif


#ifndef SHANDALAR
/* ManalinkEh 2022: 0x26a6a90; original functions/functions.c:19400. */
void ai_modifier_for_targeted_card_drawing(int player, int card, int target_pos,
                                                int cards_to_draw)
{
  card_instance_t *instance = get_card_instance(player, card);
  int targeted_player = instance->targets[target_pos].player;
  int damage_from_triggers = 0;
  CYCLE_ALL_CARDS(p, c, {
    if (in_play(p, c) && !is_humiliated(p, c, AL_TRIG))
    {
      int csvid = get_id(p, c);
      switch (csvid)
      {
      case CARD_ID_OB_NIXILIS_REIGNITED_EMBLEM:
      {
        if (p == targeted_player)
        {
          damage_from_triggers += 2;
        }
        break;
      }

      case CARD_ID_PHYREXIAN_TYRANNY:
        damage_from_triggers += 2;
        break;

      case CARD_ID_SPITEFUL_VISIONS:
        damage_from_triggers++;
        break;

      case CARD_ID_UNDERWORLD_DREAMS:
      case CARD_ID_FATE_UNRAVELER:
      case CARD_ID_NEKUSAR_THE_MINDRAZER:
      {
        damage_from_triggers += (p != targeted_player ? 1 : 0);
      }
      }
    }
  });
  if (targeted_player == player)
  {
    if (life[player] - (damage_from_triggers * cards_to_draw) < 6)
    {
      ai_modifier -= 96;
    }
    else
    {
      int amount = get_total_available_lands(player);
      ai_modifier += (amount < 3 || amount >= 6 ? 48 : -48);
    }
  }
  if (targeted_player == 1 - player)
  {
    if (life[player] - (damage_from_triggers * cards_to_draw) <= 0)
    {
      ai_modifier += 96;
    }
    else
    {
      ai_modifier -= 96;
    }
  }
}

#endif


#ifndef SHANDALAR
/* ManalinkEh 2022: 0x26a6340; original functions/functions.c:19302. */
void ai_modifier_for_act_of_treason(int player, int card)
{

  card_instance_t *instance = get_card_instance(player, card);
  if (instance->targets[0].player > -1)
  {
    if (is_what(instance->targets[0].player, instance->targets[0].card, TYPE_CREATURE))
    {
      int pow = get_power(instance->targets[0].player, instance->targets[0].card);
      if (instance->targets[0].player == player)
      {
        int mod = -(pow * 8);
        if (is_sick(instance->targets[0].player, instance->targets[0].card) &&
            current_phase < PHASE_DECLARE_ATTACKERS && current_turn == player)
        {
          mod = pow * 8;
        }
        if (is_tapped(instance->targets[0].player, instance->targets[0].card) &&
            current_phase < PHASE_DECLARE_BLOCKERS && current_turn == 1 - player)
        {
          mod = pow * 8;
        }
        ai_modifier += mod;
      }
      else
      {
        int mod = pow * 8;
        if ((get_card_instance(instance->targets[0].player, instance->targets[0].card)
                 ->regen_status &
             KEYWORD_DEFENDER) &&
            check_for_special_ability(instance->targets[0].player, instance->targets[0].card,
                                           SP_KEYWORD_CANNOT_BLOCK))
        {
          mod = -(pow * 8);
        }
        ai_modifier += mod;
      }
    }
    else
    {
      int cmc = get_cmc(instance->targets[0].player, instance->targets[0].card);
      if (instance->targets[0].player == player)
      {
        ai_modifier +=
            (is_tapped(instance->targets[0].player, instance->targets[0].card) ? (cmc * 8)
                                                                               : -(cmc * 8));
      }
      else
      {
        ai_modifier += (cmc * 8);
      }
    }
  }
}

#endif


#ifndef SHANDALAR
/* ManalinkEh 2022: 0x26a6500; original functions/functions.c:19277. */
int aot_like_spell(int player, int card, event_t event, target_definition_t *td,
                        const char *msg, int additional_flags)
{
  card_instance_t *instance = get_card_instance(player, card);
  int flags = GS_CAN_TARGET | additional_flags;
  if ((event == EVENT_CAST_SPELL && affect_me(player, card)) || event == EVENT_CHANGE_TARGET)
  {
    generic_spell(player, card, event, flags, td, msg, 1, NULL);
    if (event == EVENT_CHANGE_TARGET)
      td->who_chooses = attacking_card_controller;
    if (spell_fizzled != 1)
      ai_modifier_for_act_of_treason(player, card);
    return 0;
  }
  if (event == EVENT_RESOLVE_SPELL)
  {
    if (valid_target(td))
      effect_act_of_treason(player, card, instance->targets[0].player, instance->targets[0].card);
    kill_card(player, card, KILL_DESTROY);
  }
  return generic_spell(player, card, event, flags, td, msg, 1, NULL);
}

#endif


#ifndef SHANDALAR
/* ManalinkEh 2022: 0x26a7450; original functions/functions.c:19604. */
void ai_mod_for_tap_effects(int player, int card, int target_pos)
{

  card_instance_t *instance = get_card_instance(player, card);
  int p = instance->targets[target_pos].player;
  int c = instance->targets[target_pos].card;
  int mod = 0;
  if (is_tapped(p, c))
  {
    ai_modifier -= 96;
    return;
  }
  if (current_turn == player)
  {
    if (is_what(p, c, TYPE_CREATURE))
    {
      if (current_phase < PHASE_DECLARE_BLOCKERS)
      {
        int pow = get_power(p, c);
        int mul = 1;
        if (check_for_ability(p, c, KEYWORD_INFECT | KEYWORD_FIRST_STRIKE | KEYWORD_DOUBLE_STRIKE))
        {
          mul++;
        }
        if (check_for_special_ability(p, c, SP_KEYWORD_DEATHTOUCH))
        {
          mul++;
        }
        mod += (!check_for_special_ability(p, c, SP_KEYWORD_CANNOT_BLOCK) ? (pow * mul) : -24);
      }
      else
      {
        mod -= 96;
      }
    }
    int iid = get_card_instance(p, c)->internal_card_id;
    if (cards_data[iid].extra_ability & EA_ACT_ABILITY)
    {
      mod += (get_cmc(p, c) * 4);
    }
  }
  if (current_turn == 1 - player)
  {
    int mul = 2;
    if (is_what(p, c, TYPE_CREATURE))
    {
      int pow = get_power(p, c);
      if (current_phase < PHASE_DECLARE_ATTACKERS)
      {
        mul = 3;
        if (check_for_ability(p, c, KEYWORD_FLYING))
        {
          mul++;
        }
        if (check_for_special_ability(p, c, SP_KEYWORD_SHADOW | SP_KEYWORD_UNBLOCKABLE))
        {
          mul++;
        }
        mod += (!check_for_ability(p, c, KEYWORD_DEFENDER) ? (pow * mul) : -24);
      }
      else
      {
        if (check_for_ability(p, c, KEYWORD_FLYING))
        {
          mul++;
        }
        mod += (!check_for_special_ability(p, c, SP_KEYWORD_CANNOT_BLOCK) ? (pow * mul) : -24);
      }
    }
    int iid = get_card_instance(p, c)->internal_card_id;
    if (cards_data[iid].extra_ability & EA_ACT_ABILITY)
    {
      mod += (get_cmc(p, c) * 4);
    }
  }
  ai_modifier += mod;
}

#endif


#ifndef SHANDALAR
/* ManalinkEh 2022: 0x26a7700; original functions/functions.c:19666. */
void ai_mod_for_untap_effects(int player, int card, int target_pos, int total_mana_spent)
{

  card_instance_t *instance = get_card_instance(player, card);
  int p = instance->targets[target_pos].player;
  int c = instance->targets[target_pos].card;
  int pow_mod = current_turn == player ? 8 : 12;
  int ab_mod = current_turn == player ? 4 : 8;
  int csvid = get_id(p, c);
  int mod = 0;
  if (!is_tapped(p, c))
  {
    mod -= 48;
  }
  if (is_what(p, c, TYPE_CREATURE))
  {
    mod += (get_power(p, c) * pow_mod);
    int iid = get_card_instance(p, c)->internal_card_id;
    if ((cards_data[iid].extra_ability & EA_ACT_ABILITY) &&
        can_use_activated_abilities(p, c) && !is_sick(p, c))
    {
      mod += ((get_cmc(p, c) - total_mana_spent) * ab_mod);
    }
  }
  else
  {
    if (can_use_activated_abilities(p, c) && !is_animated_and_sick(p, c))
    {
      switch (csvid)
      {
      case CARD_ID_TOLARIAN_ACADEMY:
      {
        mod += ((count_subtype(p, TYPE_ARTIFACT, -1) - total_mana_spent) * (ab_mod * 2));
        break;
      }
      case CARD_ID_SERRAS_SANCTUM:
      {
        mod += ((count_subtype(p, TYPE_ENCHANTMENT, -1) - total_mana_spent) * (ab_mod * 2));
        break;
      }
      case CARD_ID_GAEAS_CRADLE:
      {
        mod += ((count_subtype(p, TYPE_CREATURE, -1) - total_mana_spent) * (ab_mod * 2));
        break;
      }
      case CARD_ID_GRIM_MONOLITH:
      case CARD_ID_MANA_VAULT:
      case CARD_ID_MISHRAS_WORKSHOP:
      {
        mod += ((3 - total_mana_spent) * (ab_mod * 2));
        break;
      }
      case CARD_ID_SOL_RING:
      {
        mod += ((2 - total_mana_spent) * (ab_mod * 2));
        break;
      }
      default:
      {
        mod += ((get_cmc(p, c) - total_mana_spent) * ab_mod);
        break;
      }
      }
    }
  }
  ai_modifier += (p == player ? mod : -mod);
}

#endif


#ifndef SHANDALAR
/* ManalinkEh 2022: 0x2696810; original functions/functions.c:16637. */
void twiddle(int player, int card, int tgtnum)
{
  card_instance_t *instance = get_card_instance(player, card);
  int p = instance->targets[tgtnum].player;
  int c = instance->targets[tgtnum].card;

  load_text(0, "TWIDDLE");
  switch (DIALOG(player, card, EVENT_ACTIVATE, DLG_RANDOM, DLG_NO_STORAGE, DLG_SMALLCARD(p, c),
                 text_lines[1], 1, p == AI ? 1 : 3, text_lines[2], 1, p == AI ? 3 : 1))
  {
  case 1:
    tap_card(p, c);
    break;

  case 2:
    untap_card(p, c);
    break;
  }
}

#endif


#ifndef SHANDALAR
/* ManalinkEh 2022: 0x2696930; original functions/functions.c:16662. */
void ai_modifier_twiddle(int player, int card, int tgtnum)
{
  card_instance_t *instance = get_card_instance(player, card);
  int p = instance->targets[tgtnum].player;
  int c = instance->targets[tgtnum].card;

  if (!is_what(p, c, TYPE_CREATURE) &&
      (!(get_card_data(p, c)->extra_ability & (EA_ACT_ABILITY | EA_ACT_INTERRUPT)) ||
       !is_what(p, c, TYPE_CREATURE | TYPE_ARTIFACT | TYPE_LAND)))
    ai_modifier -= 24;

  if (!(p == AI) != !(is_tapped(p, c)) || (player == p && card == c))
    ai_modifier -= 96;
}

#endif


#ifndef SHANDALAR
/* ManalinkEh 2022: 0x2696610; original functions/functions.c:16601. */
void untap_only_1_permanent_per_upkeep(int player, int card, event_t event, int t_player,
                                            int type)
{
  target_definition_t td;
  default_target_definition(player, card, &td, type);
  td.required_state = TARGET_STATE_TAPPED;
  td.illegal_abilities = 0;
  td.allowed_controller = t_player != 2 ? t_player : current_turn;
  td.preferred_controller = t_player != 2 ? t_player : current_turn;
  td.who_chooses = t_player != 2 ? t_player : current_turn;
  td.allow_cancel = 0;

  card_instance_t *instance = get_card_instance(player, card);

  upkeep_trigger_ability(player, card, event, t_player);

  if (event == EVENT_UPKEEP_TRIGGER_ABILITY)
  {
    char prompt[200];
    if (ai_is_speculating == 1)
    {
      prompt[0] = 0;
    }
    else
    {
      char typetext[100];
      type_text_impl(typetext, 100, type, TYPETEXT_0);
      scnprintf(prompt, 200, "PROCESSING %s: Select %s to untap.",
                cards_ptr[get_id(player, card)]->full_name, typetext);
    }
    instance->number_of_targets = 0;
    if (can_target(&td) && pick_next_target_noload(&td, prompt))
    {
      instance->number_of_targets = 0;
      untap_card(instance->targets[0].player, instance->targets[0].card);
    }
  }

  if (current_phase == PHASE_UNTAP && event == EVENT_UNTAP &&
      (current_turn == t_player || t_player == 2) &&
      is_what(affected_card_controller, affected_card, type))
  {
    card_instance_t *instance2 = get_card_instance(affected_card_controller, affected_card);
    instance2->untap_status &= ~3;
  }
}

#endif


#ifndef SHANDALAR
/* ManalinkEh 2022: 0x2695480; original functions/functions.c:16311. */
void untap_permanents_during_opponents_untap(int player, int card, type_t types,
                                                  int *where_to_store)
{
  if (current_phase == PHASE_UNTAP && current_turn == 1 - player)
  {
    if (*where_to_store != 77)
    {
      manipulate_all(player, card, player, types, MATCH, 0, 0, 0, 0, 0, 0, -1, 0, ACT_UNTAP);
      *where_to_store = 77;
    }
  }
  else if (*where_to_store == 77)
  {
    *where_to_store = 0;
  }
}

#endif


#ifndef SHANDALAR
/* ManalinkEh 2022: 0x2687840; original functions/functions.c:8476. */
int choosing_to_untap(int player, int card, event_t event)
{
  if (event == EVENT_UNTAP && affect_me(player, card))
    get_card_instance(player, card)->untap_status &= ~2;

  if (current_phase == PHASE_UNTAP && affect_me(player, card))
  {
    if (event == EVENT_TRIGGER)
    {
      card_instance_t *instance = get_card_instance(player, card);
      if ((instance->untap_status & 3) == 1 &&
          !(cards_data[instance->internal_card_id].type & types_that_dont_untap))
      {
        if (!IS_AI(player))
          event_result |= 1;
        else if (call_card_function_i(instance, player, card, EVENT_CHOOSING_TO_UNTAP))
          event_result |= 2;
      }
    }
    else if (event == EVENT_RESOLVE_TRIGGER)
      get_card_instance(player, card)->untap_status |= 2;
  }

  return event == EVENT_CHOOSING_TO_UNTAP;
}

#endif


#ifndef SHANDALAR
/* ManalinkEh 2022: 0x2687680; original functions/functions.c:8439. */
void choose_to_untap(int player, int card, event_t event)
{

  card_instance_t *instance = get_card_instance(player, card);

  if (event == EVENT_CAST_SPELL && affect_me(player, card))
  {
    set_special_flags2(player, card, SF2_COULD_NOT_UNTAP);
  }

  if (instance->targets[1].player > -1 && instance->targets[1].card > -1 &&
      !in_play(instance->targets[1].player, instance->targets[1].card))
  {
    instance->targets[1].player = -1;
  }

  if (event == EVENT_UNTAP && affect_me(player, card) && is_tapped(player, card))
  {
    instance->untap_status &= ~2;
  }

  if (current_phase == PHASE_UNTAP && affect_me(player, card))
  {
    if (event == EVENT_TRIGGER)
    {
      if (instance->untap_status & 1)
      {
        if (!(instance->untap_status & 2) &&
            !(cards_data[instance->internal_card_id].type & types_that_dont_untap))
        {
          if ((player != 1 || (trace_mode & 2)) && ai_is_speculating != 1)
          {
            event_result |= 1;
          }
          else if (instance->targets[1].player > -1 && instance->targets[1].card > -1 &&
                   in_play(instance->targets[1].player, instance->targets[1].card))
          {
            ;
          }
          else
          {
            event_result |= 2;
          }
        }
      }
    }
    else if (event == EVENT_RESOLVE_TRIGGER)
    {
      instance->untap_status |= 2;
    }
  }
}

#endif


#ifndef SHANDALAR
/* ManalinkEh 2022: 0x268c010; original functions/functions.c:11472. */
void untap_lands(int player, int card, int num_lands)
{
  int i = 0;
  card_instance_t *instance = get_card_instance(player, card);
  target_definition_t td;
  default_target_definition(player, card, &td, TYPE_LAND);
  td.preferred_controller = player;
  td.illegal_abilities = 0;
  while (i++ < num_lands)
  {
    if (can_target(&td))
    {
      if (select_target(player, card, &td, "Choose a land to untap", NULL))
      {
        instance->number_of_targets = 1;
        untap_card(instance->targets[0].player, instance->targets[0].card);
      }
      else
      {
        i = num_lands;
      }
    }
  }
}

#endif


#ifndef SHANDALAR
/* ManalinkEh 2022: 0x2689800; original functions/functions.c:10171. */
void remove_summoning_sickness(int player, int card)
{
  card_instance_t *instance = get_card_instance(player, card);
  if (instance->state & STATE_SUMMON_SICK)
    instance->state &= ~(STATE_SUMMON_SICK | STATE_SICKNESS_UNUSED);
}

#endif


#ifndef SHANDALAR
/* ManalinkEh 2022: 0x2696a00; original functions/functions.c:16678. */
void remove_from_combat(int player, int card)
{
  card_instance_t *instance = get_card_instance(player, card);

  instance->blocking = -1;

  if (instance->state & STATE_ATTACKING)
    instance->state |= STATE_ATTACKED;

  if (instance->state & STATE_BLOCKING)
    instance->state |= STATE_BLOCKED;

  instance->state &= ~(STATE_ATTACKING | STATE_BLOCKING);
}

#endif


#ifndef SHANDALAR
/* ManalinkEh 2022: 0x2677a90; original functions/functions.c:16365. */
static int remove_untargettable_legacy(int player, int card, event_t event)
{
  card_instance_t *instance = get_card_instance(player, card);
  if (trigger_condition == TRIGGER_COMES_INTO_PLAY && affect_me(player, card) &&
      reason_for_trigger_controller == player)
  {
    if (trigger_cause_controller == instance->targets[0].player &&
        check_state(trigger_cause_controller, trigger_cause, STATE_CANNOT_TARGET))
    {
      if (event == EVENT_TRIGGER)
      {
        event_result |= 2;
      }
      else if (event == EVENT_RESOLVE_TRIGGER)
      {
        state_untargettable(trigger_cause_controller, trigger_cause, 0);
        kill_card(player, card, KILL_EXILE);
      }
    }
  }
  if (event == EVENT_CLEANUP)
  {
    kill_card(player, card, KILL_EXILE);
  }
  return 0;
}

#endif


#ifndef SHANDALAR
/* ManalinkEh 2022: 0x2695e20; original functions/functions.c:16384. */
void type_uncounterable(int player, int card, event_t event, int t_player, int type,
                             test_definition_t *this_test)
{
  if (is_what(player, card, TYPE_PERMANENT) && is_humiliated(player, card, AL_STATIC))
  {
    return;
  }
  if (event == EVENT_CAST_SPELL && !affect_me(player, card) &&
      !is_what(affected_card_controller, affected_card, TYPE_LAND) && in_play(player, card))
  {
    if (t_player == affected_card_controller || t_player == ANYBODY)
    {
      if (this_test != NULL)
      {
        if (new_make_test(
                player,
                get_card_instance(affected_card_controller, affected_card)->internal_card_id, -1,
                this_test))
        {
          state_untargettable(affected_card_controller, affected_card, 1);
        }
      }
      if (type)
      {
        if (is_what(affected_card_controller, affected_card, type))
        {
          state_untargettable(affected_card_controller, affected_card, 1);
        }
      }
      if (this_test == NULL && !type)
      {
        state_untargettable(affected_card_controller, affected_card, 1);
      }
    }
  }

  if (this_dies_trigger_impl(player, card, event, RESOLVE_TRIGGER_MANDATORY, player, 0, 0))
  {
    int legacy = create_legacy_effect(player, card, &remove_untargettable_legacy);
    get_card_instance(player, legacy)->targets[0].player = player;
    get_card_instance(player, legacy)->token_status |= STATUS_INVISIBLE_FX;
  }

  if (trigger_condition == TRIGGER_COMES_INTO_PLAY && affect_me(player, card) &&
      reason_for_trigger_controller == player)
  {
    if ((trigger_cause_controller == t_player || t_player == ANYBODY) &&
        check_state(trigger_cause_controller, trigger_cause, STATE_CANNOT_TARGET))
    {
      if (event == EVENT_TRIGGER)
      {
        event_result |= 2;
      }
      else if (event == EVENT_RESOLVE_TRIGGER)
      {
        state_untargettable(trigger_cause_controller, trigger_cause, 0);
      }
    }
  }
}

#endif


#ifndef SHANDALAR
/* ManalinkEh 2022: 0x2696130; original functions/functions.c:16465. */
int is_artifact_creature_by_internal_id(int iid, int unused1, int unused2, int unused3)
{
  return is_what(-1, iid, TYPE_CREATURE) && is_what(-1, iid, TYPE_ARTIFACT) ? 1 : 0;
}

#endif


#ifndef SHANDALAR
/* ManalinkEh 2022: 0x2696190; original functions/functions.c:16469. */
void global_type_change(int player, int card, event_t event, int t_player, int added_type,
                             test_definition_t *this_test, int pow, int tou, int key,
                             int subtype, int clr)
{
  if (event == EVENT_CHANGE_TYPE && in_play(player, card))
  {
    if (affected_card_controller == t_player || t_player == 2)
    {
      if (!is_what(affected_card_controller, affected_card, TYPE_EFFECT) &&
          new_make_test_in_play(affected_card_controller, affected_card, -1, this_test))
      {
        if (!(land_can_be_played & LCBP_DURING_EVENT_CHANGE_TYPE_SECOND_PASS))
        {

          land_can_be_played |= LCBP_NEED_EVENT_CHANGE_TYPE_SECOND_PASS;
        }
        else
        {
          int newtype = *(int *)(0x7a5380);
          int end = newtype + 16;

          for (; newtype < end; ++newtype)
          {
            if (cards_data[newtype].id == cards_data[event_result].id)
            {
              if (new_make_test(player, newtype, -1, this_test))
              {
                event_result = newtype;
                goto found;
              }
            }
          }

          newtype = create_a_card_type(event_result);
          if (newtype != -1)
          {
            cards_data[newtype].type |= added_type;
            if (pow == 16384)
              pow = get_cmc(affected_card_controller, affected_card);
            cards_data[newtype].power = pow;
            if (tou == 16384)
              tou = get_cmc(affected_card_controller, affected_card);
            cards_data[newtype].toughness = tou;
            cards_data[newtype].static_ability = key;
            event_result = newtype;
          }

        found:
          get_card_instance(affected_card_controller, affected_card)->token_status |=
              STATUS_ANIMATED;
          if (subtype > 0 && !has_subtype(affected_card_controller, affected_card, subtype))
          {
            add_a_subtype(affected_card_controller, affected_card, subtype);
          }
        }
      }
    }
  }

  if (event == EVENT_SET_COLOR && in_play(player, card) && clr > 0)
  {
    if (affected_card_controller == t_player || t_player == 2)
    {
      if (new_make_test_in_play(affected_card_controller, affected_card, -1, this_test))
      {
        event_result = get_sleighted_color_test(player, card, clr);
      }
    }
  }
}

#endif


#ifndef SHANDALAR
/* ManalinkEh 2022: 0x268cab0; original functions/functions.c:15433. */
int type_change_legacy(int player, int card, event_t event)
{
  card_instance_t *instance = get_card_instance(player, card);
  if (instance->targets[0].player < 0)
    return 0;
  int p = instance->targets[0].player;
  int c = instance->targets[0].card;
  int destroyed = 0;
  if (event == EVENT_CHANGE_TYPE && affect_me(p, c))
    event_result = instance->dummy3;
  if (instance->targets[9].player > 0)
  {
    if (instance->targets[9].player & 1)
      attached_is_forced_subtype(player, card, event, instance->targets[9].card);
    if (instance->targets[9].player & 2)
      attached_is_additional_subtype(player, card, event, instance->targets[9].card);
  }
  if (event == EVENT_STATIC_EFFECTS && instance->targets[1].player > 0)
  {
    if ((instance->targets[1].player & 2) && instance->targets[4].player >= 0 &&
        !in_play(instance->targets[4].player, instance->targets[4].card))
      destroyed = 1;
    if ((instance->targets[1].player & 4) &&
        !count_counters(p, c, (unsigned char)instance->targets[7].player))
      destroyed = 1;
  }
  if (event == EVENT_ABILITIES && affect_me(p, c) && instance->targets[8].player > 0)
    event_result |= instance->targets[8].player;
  if (event == EVENT_SET_COLOR && affect_me(p, c))
  {
    if (instance->targets[6].player > 0)
      event_result = instance->targets[6].player;
    if (instance->targets[6].card > 0)
      event_result |= instance->targets[6].card;
  }
  if (instance->targets[2].card == CARD_ID_GENJU_OF_THE_FIELDS)
    spirit_link_effect(p, c, event, p);
  if (instance->targets[8].card > 0)
    special_abilities(p, c, event, instance->targets[8].card, player, card);
  if (instance->targets[1].player > 0 && (instance->targets[1].player & 1) &&
      eot_trigger(player, card, event))
  {
    if (instance->targets[2].card == CARD_ID_CHIMERIC_COILS)
    {
      destroyed = 0;
      kill_card(p, c, KILL_SACRIFICE);
    }
    else
      destroyed = 1;
  }
  if (instance->targets[1].player > 0 && (instance->targets[1].player & 8) &&
      current_turn == instance->targets[4].player &&
      upkeep_trigger_mode_impl(player, card, event, RESOLVE_TRIGGER_MANDATORY, 0))
    destroyed = 1;
  if (instance->targets[1].player > 0 && (instance->targets[1].player & 0x20) &&
      event == EVENT_BEGIN_TURN && current_turn == player)
    destroyed = 1;
  if (instance->targets[1].player > 0 && (instance->targets[1].player & 0x10) &&
      end_of_combat_trigger(player, card, event, RESOLVE_TRIGGER_MANDATORY))
    destroyed = 1;
  if (destroyed)
  {
    remove_special_flags(p, c, SF_TYPE_ALREADY_CHANGED);
    kill_card(player, card, KILL_EXILE);
    recalculate_all_cards_in_play();
  }
  return 0;
}

#endif


#ifndef SHANDALAR
/* ManalinkEh 2022: 0x2693880; original functions/functions.c:15528. */
int turn_into_artifact(int player, int card, int t_player, int t_card, int mode)
{
  int legacy = -1;
  if (t_player != -1 && t_card != -1)
  {
    card_instance_t *instance = get_card_instance(t_player, t_card);
    int newtype = create_a_card_type(instance->internal_card_id);
    cards_at_7c7000[newtype]->type |= (cards_data[instance->internal_card_id].type | TYPE_ARTIFACT);
    legacy =
        create_targetted_legacy_effect(player, card, &type_change_legacy, t_player, t_card);
    card_instance_t *leg = get_card_instance(player, legacy);
    leg->dummy3 = newtype;
    leg->token_status |= STATUS_PERMANENT;
    leg->targets[1].player = mode;
  }
  return legacy;
}

#endif


#ifndef SHANDALAR
/* ManalinkEh 2022: 0x2694d70; original functions/functions.c:15543. */
int turn_into_creature(int player, int card, int t_player, int t_card, int mode, int pow,
                            int tou)
{

  int legacy = -1;
  if (t_player != -1 && t_card != -1)
  {
    card_instance_t *instance = get_card_instance(t_player, t_card);
    int newtype = create_a_card_type(instance->internal_card_id);
    cards_at_7c7000[newtype]->type |= (cards_data[instance->internal_card_id].type | TYPE_CREATURE);
    if (pow != 0)
    {
      cards_at_7c7000[newtype]->power = pow != -1 ? pow : get_cmc(t_player, t_card);
    }
    if (tou != 0)
    {
      cards_at_7c7000[newtype]->toughness = tou != -1 ? tou : get_cmc(t_player, t_card);
    }
    legacy = create_targetted_legacy_effect_no_repeat(player, card, &type_change_legacy,
                                                      t_player, t_card);
    card_instance_t *leg = get_card_instance(player, legacy);
    leg->dummy3 = newtype;
    leg->token_status |= STATUS_PERMANENT;
    leg->targets[1].player = mode;
  }
  return legacy;
}

#endif


#ifndef SHANDALAR
/* ManalinkEh 2022: 0x2695040; original functions/functions.c:15569. */
int artifact_animation(int player, int card, int t_player, int t_card, int mode, int pow,
                            int tou, int abilities, int sp_abilities)
{

  int legacy_card = turn_into_creature(player, card, t_player, t_card, mode, pow, tou);
  card_instance_t *legacy_instance = get_card_instance(player, legacy_card);
  legacy_instance->targets[2].card = get_id(player, card);
  legacy_instance->targets[8].player = abilities;
  legacy_instance->targets[8].card = sp_abilities;
  return legacy_card;
}

#endif


#ifndef SHANDALAR
/* ManalinkEh 2022: 0x2696490; original functions/functions.c:14805. */
static int global_type_animation_legacy(int player, int card, event_t event)
{

  card_instance_t *instance = get_card_instance(player, card);

  if (instance->targets[0].player > -1)
  {

    test_definition_t this_test;
    default_test_definition(&this_test, instance->targets[4].player);
    this_test.type_flag = instance->targets[4].card;
    this_test.subtype = instance->targets[5].player;
    this_test.subtype_flag = instance->targets[5].card;
    this_test.color = instance->targets[6].player;
    this_test.color_flag = instance->targets[6].card;
    this_test.cmc = instance->targets[7].player;
    this_test.cmc_flag = instance->targets[7].card;

    global_type_change(player, card, event, instance->targets[0].player, TYPE_CREATURE,
                            &this_test, instance->targets[1].player, instance->targets[1].card,
                            instance->targets[2].player, instance->targets[3].card,
                            instance->targets[3].player);

    if (instance->targets[0].card > -1 && (instance->targets[0].card & 1))
    {
      if (eot_trigger(player, card, event))
      {
        kill_card(player, card, KILL_EXILE);
      }
    }
  }

  return 0;
}

#endif


#ifndef SHANDALAR
/* ManalinkEh 2022: 0x2694ea0; original functions/functions.c:14787. */
int real_land_animation2(int player, int card, int t_player, int t_card, int mode, int pow,
                              int tou, int key, int s_key, int clr)
{

  int legacy = turn_into_creature(player, card, t_player, t_card, mode, pow, tou);
  card_instance_t *leg = get_card_instance(player, legacy);
  leg->targets[1].player = mode;

  leg->targets[2].card = get_id(player, card);
  leg->targets[4].player = player;
  leg->targets[4].card = card;
  leg->targets[6].player = clr;
  leg->targets[8].player = key;
  leg->targets[8].card = s_key;
  return legacy;
}

#endif


#ifndef SHANDALAR
/* ManalinkEh 2022: 0x2694f40; original functions/functions.c:14835. */
int land_animation2(int player, int card, int t_player, int t_card, int mode, int pow, int tou,
                         int key, int s_key, int clr, test_definition_t *this_test)
{
  if (t_card != -1)
    return real_land_animation2(player, card, t_player, t_card, mode, pow, tou, key, s_key,
                                     clr);
  int legacy = create_legacy_effect(player, card, &global_type_animation_legacy);
  card_instance_t *leg = get_card_instance(player, legacy);
  leg->targets[0].player = t_player;
  leg->targets[0].card = mode;
  leg->targets[1].player = pow;
  leg->targets[1].card = tou;
  leg->targets[2].player = key;
  leg->targets[2].card = s_key;
  leg->targets[3].player = clr;
  leg->targets[3].card = -1;
  leg->targets[4].player = this_test->type;
  leg->targets[4].card = this_test->type_flag;
  leg->targets[5].player = this_test->subtype;
  leg->targets[5].card = this_test->subtype_flag;
  leg->targets[6].player = this_test->color_flag;
  leg->targets[7].card = this_test->cmc_flag;
  return legacy;
}

#endif


#ifndef SHANDALAR
/* ManalinkEh 2022: 0x26928c0; original functions/functions.c:14756. */
int card_generic_animated_land(int player, int card, event_t event)
{
  card_instance_t *instance = get_card_instance(player, card);
  if (instance->targets[5].player > 0)
  {
    instance->display_pic_csv_id = instance->targets[5].player;
  }
  if (instance->targets[5].card > 0)
  {
    instance->display_pic_num = instance->targets[5].card;
  }
  if (event == EVENT_POWER && affect_me(player, card) && instance->targets[7].player > 0)
  {
    set_base_pt(instance->targets[7].player - 1);
  }
  if (event == EVENT_TOUGHNESS && affect_me(player, card) && instance->targets[7].card > 0)
  {
    set_base_pt(instance->targets[7].card - 1);
  }
  if (event == EVENT_ABILITIES && affect_me(player, card) && instance->targets[8].player > 0)
  {
    event_result |= instance->targets[8].player;
  }
  if (event == EVENT_SET_COLOR && affect_me(player, card) && instance->targets[6].player > 0)
  {
    event_result = instance->targets[6].player;
  }
  if (instance->targets[8].card > 0)
  {
    special_abilities(player, card, event, instance->targets[8].card, player, card);
  }
  int result = 0;
  if (instance->targets[6].card > -1)
  {
    int (*ptFunction)(int, int, event_t) = (void *)instance->targets[6].card;
    result = ptFunction(player, card, event);
  }
  return result;
}

#endif


#ifndef SHANDALAR
/* ManalinkEh 2022: 0x267f5e0; original functions/functions.c:5887. */
void land_animation(int player, int card)
{

  card_instance_t *instance = get_card_instance(player, card);

  if (instance->targets[13].player == instance->targets[13].card)
  {
    int id = instance->targets[13].player;
    id++;
    instance->targets[12].card = get_internal_card_id_from_csv_id(id);
    instance->targets[13].card = id;
  }
  else
  {
    int id = instance->targets[13].player;
    instance->targets[12].card = get_internal_card_id_from_csv_id(id);
    instance->targets[13].card = id;
  }
}

#endif


#ifndef SHANDALAR
/* ManalinkEh 2022: 0x2695570; original functions/functions.c:16331. */
void true_transform(int player, int card)
{

  card_instance_t *instance = get_card_instance(player, card);

  int id = instance->targets[13].player;
  if (instance->targets[13].player == instance->targets[13].card)
  {
    id++;
  }
  instance->targets[12].card = get_internal_card_id_from_csv_id(id);
  instance->targets[13].card = id;
}

#endif


#ifndef SHANDALAR
/* ManalinkEh 2022: 0x269db70; original functions/functions.c:5904. */
int manland_shared(int player, int card, event_t event, int colorless, int black, int blue,
                        int green, int red, int white)
{
  card_instance_t *instance = get_card_instance(player, card);
  if (event == EVENT_CAN_ACTIVATE)
  {
    set_special_flags(player, card, SF_MANA_PRODUCER_DISABLED_FOR_AI);
    int result = granted_generic_activated_ability(player, card, player, card, event,
                                                        GAA_TYPE_CHANGE, colorless, black, blue,
                                                        green, red, white, 0, NULL, NULL);
    if (result && (get_id(player, card) != CARD_ID_NANTUKO_MONASTERY || has_threshold(player)))
    {
      remove_special_flags(player, card, SF_MANA_PRODUCER_DISABLED_FOR_AI);
      return 1;
    }
    remove_special_flags(player, card, SF_MANA_PRODUCER_DISABLED_FOR_AI);
  }
  else if (event == EVENT_ACTIVATE)
  {
    instance->info_slot = 0;
    int choice = 1;
    if (mana_producer(player, card, EVENT_CAN_ACTIVATE))
    {
      set_special_flags(player, card, SF_MANA_PRODUCER_DISABLED_FOR_AI);
      choice = 0;
      if (unknown62BCEC != 0 && needed_mana_colors == 0 &&
          granted_generic_activated_ability(player, card, player, card, EVENT_CAN_ACTIVATE,
                                                 GAA_TYPE_CHANGE, colorless, black, blue, green,
                                                 red, white, 0, NULL, NULL) &&
          (get_id(player, card) != CARD_ID_NANTUKO_MONASTERY || has_threshold(player)))
        choice = do_dialog(player, player, card, -1, -1, " Generate Mana\n Animate\n Cancel", 1);
      remove_special_flags(player, card, SF_MANA_PRODUCER_DISABLED_FOR_AI);
    }
    if (choice == 1)
    {
      set_special_flags(player, card, SF_MANA_PRODUCER_DISABLED_FOR_AI);
      if (granted_generic_activated_ability(player, card, player, card, event, GAA_TYPE_CHANGE,
                                                 colorless, black, blue, green, red, white, 0, NULL,
                                                 NULL))
        instance->info_slot = 66;
      remove_special_flags(player, card, SF_MANA_PRODUCER_DISABLED_FOR_AI);
      return 0;
    }
    if (choice != 0)
    {
      spell_fizzled = 1;
      return 0;
    }
  }
  else if (event == EVENT_RESOLVE_ACTIVATION && instance->info_slot == 66 &&
           instance->targets[13].player == instance->targets[13].card)
  {
    get_card_instance(player, instance->parent_card)->token_status |= STATUS_ANIMATED;
    true_transform(player, instance->parent_card);
    return 0;
  }
  return mana_producer(player, card, event);
}

#endif


#ifndef SHANDALAR
/* ManalinkEh 2022: 0x269df80; original functions/functions.c:5966. */
int manland_animated(int player, int card, event_t event, int colorless, int black, int blue,
                          int green, int red, int white)
{
  card_instance_t *instance = get_card_instance(player, card);
  if (get_id(player, card) != CARD_ID_STALKING_STONES_ANIMATED && eot_trigger(player, card, event))
  {
    remove_status(player, card, STATUS_ANIMATED);
    remove_special_flags(player, card, SF_TYPE_ALREADY_CHANGED);
    true_transform(player, card);
  }
  else if (event == EVENT_SET_COLOR && affect_me(player, card))
  {
    event_result = cards_data[instance->internal_card_id].color;
  }
  return manland_shared(player, card, event, colorless, black, blue, green, red, white);
}

#endif


#ifndef SHANDALAR
/* ManalinkEh 2022: 0x269e0b0; original functions/functions.c:5979. */
int manland_normal(int player, int card, event_t event, int colorless, int black, int blue,
                        int green, int red, int white)
{
  double_faced_card(player, card, event);
  return manland_shared(player, card, event, colorless, black, blue, green, red, white);
}

#endif


#ifndef SHANDALAR
/* ManalinkEh 2022: 0x267f030; original functions/functions.c:5568. */
void cloning(int player, int card, int t_player, int t_card)
{
  card_instance_t *instance = get_card_instance(player, card);
  if (t_player == -1)
  {
    instance->targets[12].card = t_card;
  }
  else
  {
    card_instance_t *target = get_card_instance(t_player, t_card);
    int iid = target->internal_card_id;
    if (iid == -1)
      iid = target->backup_internal_card_id;

    switch (get_id(player, card))
    {
    case CARD_ID_PHYREXIAN_METAMORPH:
      if (!(cards_data[iid].type & TYPE_ARTIFACT))
      {
        iid = create_a_card_type(iid);
        cards_data[iid].type |= TYPE_ARTIFACT;
      }
      break;

    case CARD_ID_COPY_ARTIFACT:
      if (!(cards_data[iid].type & TYPE_ENCHANTMENT) || cards_data[iid].cc[2] == 9)
      {
        iid = create_a_card_type(iid);
        cards_data[iid].type |= TYPE_ENCHANTMENT;
      }
      break;
    }
    if (target->targets[13].player > -1 && target->targets[13].card > -1)
    {
      if (t_player != -1 && check_status(t_player, t_card, STATUS_ANIMATED))
      {
        instance->targets[13].player =
            cards_data[get_original_internal_card_id(t_player, t_card)].id;
        instance->targets[13].card =
            cards_data[get_original_internal_card_id(t_player, t_card)].id + 1;
      }
      else
      {
        instance->targets[13].player = target->targets[13].player;
        instance->targets[13].card = target->targets[13].card;
      }
    }
    if (target->targets[12].player > -1)
    {
      if (t_player != -1 && check_status(t_player, t_card, STATUS_ANIMATED))
      {
        instance->targets[12].player =
            cards_data[get_original_internal_card_id(t_player, t_card)].id;
      }
      else
      {
        instance->targets[12].player = target->targets[12].player;
      }
    }
    if (t_player != -1 && check_status(t_player, t_card, STATUS_ANIMATED))
    {
      instance->targets[12].card = get_original_internal_card_id(t_player, t_card);
    }
    else
    {
      instance->targets[12].card = iid;
    }
    instance->initial_color = 0;
    copy_token_characteristics_for_clone(player, card, t_player, t_card);
  }
}

#endif


#ifndef SHANDALAR
/* ManalinkEh 2022: 0x267f2d0; original functions/functions.c:5624. */
void cloning_and_verify_legend(int player, int card, int t_player, int t_card)
{
  cloning(player, card, t_player, t_card);

  card_instance_t *instance = get_card_instance(player, card);
  instance->regen_status |= KEYWORD_RECALC_CHANGE_TYPE;
  get_abilities(player, card, EVENT_CHANGE_TYPE, -1);
  if (in_play(player, card))
    verify_legend_rule(player, card, get_id(player, card));
}

#endif


#ifndef SHANDALAR
/* ManalinkEh 2022: 0x267f570; original functions/functions.c:5692. */
int enters_the_battlefield_as_copy_of_any_creature(int player, int card, event_t event)
{
  target_definition_t td;
  if (event == EVENT_RESOLVE_SPELL)
  {
    base_target_definition(player, card, &td, TYPE_CREATURE);
    td.preferred_controller = ANYBODY;
  }
  return enters_the_battlefield_as_copy_of_any(player, card, event, &td, "SELECT_A_CREATURE");
}

#endif


#ifndef SHANDALAR
/* ManalinkEh 2022: 0x2686b30; original functions/functions.c:5836. */
int shapeshift_target(int src_player, int src_card, int t_player, int t_card,
                           int turn_into_player, int turn_into_card, int mode)
{
  int legacy = real_shapeshift_target(src_player, src_card, t_player, t_card, turn_into_player,
                                           turn_into_card, mode);

  if (in_play(t_player, t_card))
    verify_legend_rule(t_player, t_card, get_id(t_player, t_card));

  return legacy;
}

#endif


#ifndef SHANDALAR
/* ManalinkEh 2022: 0x2681af0; original functions/functions.c:8124. */
int is_planeswalker(int player, int card)
{

  if (player == -1)
  {
    card_data_t *card_d = &cards_data[card];
    if ((card_d->type & TYPE_ENCHANTMENT) && card_d->cc[2] == 9)
    {
      return 1;
    }
  }
  else
  {
    card_data_t *card_d = get_card_data(player, card);
    if ((card_d->type & TYPE_ENCHANTMENT) && card_d->cc[2] == 9)
    {
      return 1;
    }
  }

  return 0;
}

#endif


#ifndef SHANDALAR
/* ManalinkEh 2022: 0x2681b50; original functions/functions.c:6596. */
type_t get_type_with_iid(int player, int card, int iid)
{
  type_t typ = cards_data[iid].type;
  if ((typ & (TYPE_INSTANT | TYPE_INTERRUPT)) && (typ & (TYPE_PERMANENT | TYPE_SORCERY)))
    typ &= ~(TYPE_INSTANT | TYPE_INTERRUPT);
  if (typ & TYPE_INTERRUPT)
    typ |= TYPE_INSTANT;
  if (player == -1 || card == -1)
  {
    if ((cards_data[iid].type & TYPE_ENCHANTMENT) && cards_data[iid].cc[2] == 9)
      typ = (typ & ~TYPE_ENCHANTMENT) | TARGET_TYPE_PLANESWALKER;
  }
  else
  {
    if (is_planeswalker(player, card) ||
        ((cards_data[iid].type & TYPE_ENCHANTMENT) && cards_data[iid].cc[2] == 9))
      typ = (typ & ~TYPE_ENCHANTMENT) | TARGET_TYPE_PLANESWALKER;
    if (typ & (TYPE_PERMANENT | TARGET_TYPE_PLANESWALKER))
    {
      int flags =
          check_special_flags2(player, card, SF2_ENCHANTED_EVENING | SF2_MYCOSYNTH_LATTICE);
      if (flags & SF2_ENCHANTED_EVENING)
        typ |= TYPE_ENCHANTMENT;
      if (flags & SF2_MYCOSYNTH_LATTICE)
        typ |= TYPE_ARTIFACT;
    }
  }
  return typ;
}

#endif


#ifndef SHANDALAR
/* ManalinkEh 2022: 0x2681c50; original functions/functions.c:6630. */
type_t get_type(int player, int card)
{
  int iid;
  if (player < 0)
    iid = card;
  else
  {
    card_instance_t *instance = get_card_instance(player, card);
    iid = instance->internal_card_id;
    if (iid < 0)
      iid = instance->backup_internal_card_id;
  }
  return get_type_with_iid(player, card, iid);
}

#endif


#ifndef SHANDALAR
/* ManalinkEh 2022: 0x2681ca0; original functions/functions.c:6645. */
int is_what(int player, int card, int test_type)
{

  int typ;
  if (player == -1)
  {
    typ = cards_data[card].type;
    if ((test_type & TYPE_CREATURE) && cards_data[card].id == 0x5a13)
      return 1;
  }
  else
  {
    typ = get_card_data(player, card)->type;
  }

  if ((test_type & TARGET_TYPE_PLANESWALKER) && is_planeswalker(player, card))
  {
    return 1;
  }

  if (test_type & TYPE_ENCHANTMENT)
  {
    if (player != -1 && (typ & TYPE_PERMANENT) &&
        check_special_flags2(player, card, SF2_ENCHANTED_EVENING))
    {
      typ |= TYPE_ENCHANTMENT;
    }
    else if ((test_type & TYPE_PERMANENT) != TYPE_PERMANENT && is_planeswalker(player, card))
    {
      return 0;
    }
  }

  if (test_type & TYPE_ARTIFACT)
  {
    if (player != -1 && (typ & TYPE_PERMANENT) &&
        check_special_flags2(player, card, SF2_MYCOSYNTH_LATTICE))
    {
      typ |= TYPE_ARTIFACT;
    }
  }

  if (typ & TYPE_INTERRUPT)
  {
    typ |= TYPE_INSTANT;
  }

  if ((typ & (TYPE_INSTANT | TYPE_INTERRUPT)) && (typ & (TYPE_PERMANENT | TYPE_SORCERY)))
  {
    typ &= ~(TYPE_INSTANT | TYPE_INTERRUPT);
  }

  return typ & test_type ? 1 : 0;
}

#endif


#ifndef SHANDALAR
/* ManalinkEh 2022: 0x2677b70; original functions/functions.c:16703. */
static int get_pt_for_creature_with_variable_pt(int player, int iid, event_t event)
{
  if (cards_data[iid].id == 0x5a13)
    return 1;
  int curr = event == EVENT_POWER ? cards_data[iid].power : cards_data[iid].toughness;
  if ((curr > 0 && (curr & 0x4000)))
  {
    curr &= ~0x4000;

    int old_affected_card_controller = affected_card_controller;
    int old_affected_card = affected_card;
    int old_event_result = event_result;

    affected_card_controller = player;
    affected_card = -1;
    event_result = curr;
    call_card_fn(cards_data[iid].code_pointer, NULL, player, -1, event);
    curr = event_result;

    affected_card_controller = old_affected_card_controller;
    affected_card = old_affected_card;
    event_result = old_event_result;
  }
  return curr;
}

#endif


#ifndef SHANDALAR
/* ManalinkEh 2022: 0x2696aa0; original functions/functions.c:16731. */
int get_base_power(int player, int card)
{
  card_instance_t *instance = get_card_instance(player, card);
  int iid = instance->internal_card_id;
  if (iid == -1)
    iid = instance->backup_internal_card_id;
  return get_pt_for_creature_with_variable_pt(player, iid, EVENT_POWER);
}

#endif


#ifndef SHANDALAR
/* ManalinkEh 2022: 0x2696ae0; original functions/functions.c:16740. */
int get_base_power_iid(int owner, int iid)
{
  return get_pt_for_creature_with_variable_pt(owner, iid, EVENT_POWER);
}

#endif


#ifndef SHANDALAR
/* ManalinkEh 2022: 0x2696b00; original functions/functions.c:16744. */
int get_base_toughness(int player, int card)
{
  card_instance_t *instance = get_card_instance(player, card);
  int iid = instance->internal_card_id;
  if (iid == -1)
    iid = instance->backup_internal_card_id;
  return get_pt_for_creature_with_variable_pt(player, iid, EVENT_TOUGHNESS);
}

#endif


#ifndef SHANDALAR
/* ManalinkEh 2022: 0x2696b40; original functions/functions.c:16753. */
int get_base_toughness_iid(int owner, int iid)
{
  return get_pt_for_creature_with_variable_pt(owner, iid, EVENT_TOUGHNESS);
}

#endif


#ifndef SHANDALAR
/* ManalinkEh 2022: 0x267ed20; original functions/functions.c:4847. */
static int split_cards_cmc_table_by_id(int csvid, int half_played)
{
  int first = split_cards_id_table(csvid, 1);
  if (first < 0)
    return -1;
  if (num_bits_set((unsigned int)half_played) - 1U <= 1U)
  {
    int selected = split_cards_id_table(csvid, half_played);
    return get_cmc_by_internal_id(get_internal_card_id_from_csv_id(selected));
  }
  int result = get_cmc_by_internal_id(get_internal_card_id_from_csv_id(first));
  int second = split_cards_id_table(csvid, 2);
  return result + get_cmc_by_internal_id(get_internal_card_id_from_csv_id(second));
}

#endif


#ifndef SHANDALAR
/* ManalinkEh 2022: 0x267ecb0; original functions/functions.c:4938. */
int get_cmc_by_internal_id(int iid)
{
  int result = cards_data[iid].cc[0];
  if (cards_data[iid].cc[1] <= 16)
    result += cards_data[iid].cc[1];
  if (has_no_mana_cost(-1, iid))
    return 0;
  int split = split_cards_cmc_table_by_id(cards_data[iid].id, 0);
  return split < 0 ? result : split;
}

#endif


#ifndef SHANDALAR
/* ManalinkEh 2022: 0x2694100; original functions/functions.c:4885. */
int get_cmc(int player, int card)
{
  if (player == -1)
    return get_cmc_by_internal_id(card);
  card_instance_t *instance = get_card_instance(player, card);
  if (is_token(player, card))
    return 0;
  int iid = instance->internal_card_id;
  if (iid == -1)
    iid = instance->backup_internal_card_id;
  if (in_play(player, card) && check_special_flags3(player, card, SF3_CARD_IS_FLIPPED))
    iid = get_original_internal_card_id(player, card);
  int result = get_cmc_by_internal_id(iid);
  if (is_x_spell(player, card) && (instance->state & STATE_INVISIBLE))
    result += instance->info_slot;
  int csvid = get_id(player, card);
  int half = get_card_instance(player, card)->info_slot;
  int split = split_cards_cmc_table_by_id(csvid, half);
  if (split >= 0)
  {
    is_x_spell(player, card);
    csvid = get_id(player, card);
    if ((csvid == CARD_ID_CRIME_PUNISHMENT && (instance->info_slot & 2)) ||
        (csvid == CARD_ID_SUPPLY_DEMAND && (instance->info_slot & 1)))
      split += instance->targets[1].card > 0 ? instance->targets[1].card : 0;
    return split;
  }
  return result;
}

#endif


#ifndef SHANDALAR
/* ManalinkEh 2022: 0x2692070; original functions/functions.c:14424. */
int get_highest_power(int player)
{
  int result = 0;
  for (int p = 0; p < 2; ++p)
    if (p == player || player == ANYBODY)
      for (int c = 0; c < active_cards_count[p]; ++c)
        if (in_play(p, c) && is_what(p, c, TYPE_CREATURE))
        {
          int power = get_power(p, c);
          if (power > result)
            result = power;
        }
  return result;
}

#endif


#ifndef SHANDALAR
/* ManalinkEh 2022: 0x2691ae0; original functions/functions.c:14289. */
int get_power_sum(int player)
{

  int result = 0;
  int i;
  for (i = 0; i < 2; i++)
  {
    if (i == player || player == 2)
    {
      int count = 0;
      while (count < active_cards_count[i])
      {
        if (in_play(i, count) && is_what(i, count, TYPE_CREATURE))
        {
          result += get_power(i, count);
        }
        count++;
      }
    }
  }

  return result;
}

#endif


#ifndef SHANDALAR
/* ManalinkEh 2022: 0x2691a40; original functions/functions.c:14277. */
int count_defenders(int player)
{
  int p, c, rval = 0;
  for (p = 0; p < 2; ++p)
    if (p == player || player == ANYBODY)
      for (c = 0; c < active_cards_count[p]; ++c)
        if (in_play(p, c) && is_what(p, c, TYPE_CREATURE) &&
            check_for_ability(p, c, KEYWORD_DEFENDER))
          ++rval;

  return rval;
}

#endif


#ifndef SHANDALAR
/* ManalinkEh 2022: 0x268c7a0; original functions/functions.c:11810. */
int count_creatures_by_toughness(int player, int pow, int mode)
{

  int count = 0;
  int result = 0;
  while (count < active_cards_count[player])
  {
    if (in_play(player, count) && is_what(player, count, TYPE_CREATURE))
    {
      int his_pow = get_toughness(player, count);
      if (mode == 0 && his_pow == pow)
      {
        result++;
      }
      if (mode == 1 && his_pow > pow)
      {
        result++;
      }
      if (mode == 2 && his_pow < pow)
      {
        result++;
      }
    }
    count++;
  }
  return result;
}

#endif


#ifndef SHANDALAR
/* ManalinkEh 2022: 0x268c6e0; original functions/functions.c:11785. */
int count_creatures_by_power(int player, int pow, int mode)
{

  int count = 0;
  int result = 0;
  while (count < active_cards_count[player])
  {
    if (in_play(player, count) && is_what(player, count, TYPE_CREATURE))
    {
      int his_pow = get_power(player, count);
      if (mode == 0 && his_pow == pow)
      {
        result++;
      }
      if (mode == 1 && his_pow > pow)
      {
        result++;
      }
      if (mode == 2 && his_pow < pow)
      {
        result++;
      }
    }
    count++;
  }
  return result;
}

#endif


#ifndef SHANDALAR
/* ManalinkEh 2022: 0x26944f0; original functions/functions.c:8412. */
int get_highest_cmc_nonland(int player)
{
  int c, result = 0;
  for (c = 0; c < active_cards_count[player]; ++c)
    if (in_play(player, c) && is_what(player, c, TYPE_PERMANENT) &&
        !is_what(player, c, TYPE_LAND))
    {
      int cmc = get_cmc(player, c);
      if (cmc > result)
        result = cmc;
    }

  return result;
}

#endif


#ifndef SHANDALAR
/* ManalinkEh 2022: 0x2694440; original functions/functions.c:8395. */
int get_highest_cmc(int player, type_t type)
{
  int p, c, result = 0;
  for (p = 0; p < 2; p++)
  {
    if (player == p || player == ANYBODY)
    {
      for (c = 0; c < active_cards_count[p]; ++c)
      {
        if (in_play(p, c) && is_what(p, c, type))
        {
          int cmc = get_cmc(p, c);
          if (cmc > result)
          {
            result = cmc;
          }
        }
      }
    }
  }
  return result;
}

#endif


#ifndef SHANDALAR
/* ManalinkEh 2022: 0x2694390; original functions/functions.c:8376. */
int get_most_common_cmc_in_hand(int player, type_t type)
{
  int c, cmc[17] = {0};

  for (c = 0; c < active_cards_count[player]; ++c)
    if (in_hand(player, c) && is_what(player, c, type))
      cmc[get_cmc(player, c)]++;

  int i, result = -1, par = 0;
  for (i = 0; i < 17; ++i)
    if (cmc[i] > par)
    {
      par = cmc[i];
      result = i;
    }

  return result;
}

#endif


#ifndef SHANDALAR
/* ManalinkEh 2022: 0x26942c0; original functions/functions.c:8357. */
int get_most_common_cmc_nonland(int player)
{
  int c, cmc[17] = {0};

  for (c = 0; c < active_cards_count[player]; ++c)
    if (in_play(player, c) && is_what(player, c, TYPE_PERMANENT) &&
        !is_what(player, c, TYPE_LAND))
      cmc[get_cmc(player, c)]++;

  int i, result = -1, par = 0;
  for (i = 0; i < 17; ++i)
    if (cmc[i] > par)
    {
      par = cmc[i];
      result = i;
    }

  return result;
}

#endif


#ifndef SHANDALAR
/* ManalinkEh 2022: 0x2683b50; original functions/functions.c:2844. */
int count_permanents_by_type(int player, type_t type)
{
  int p, c, count = 0;
  for (p = 0; p <= 1; ++p)
    if (player == ANYBODY || player == p)
      for (c = 0; c < active_cards_count[p]; ++c)
        if (in_play(p, c) && is_what(p, c, type) && !is_what(p, c, TYPE_EFFECT))
          ++count;
  return count;
}

#endif


#ifndef SHANDALAR
/* ManalinkEh 2022: 0x2683e60; original functions/functions.c:6744. */
int count_permanents_by_color(int player, int selected_type, int selected_color)
{
  int result = 0;
  if (player == ANYBODY)
  {
    result = count_permanents_by_color(0, selected_type, selected_color);
    player = 1;
  }
  for (int c = 0; c < active_cards_count[player]; ++c)
    if (is_what(player, c, selected_type) && in_play(player, c) &&
        (get_color(player, c) & selected_color))
      ++result;
  return result;
}

#endif


#ifndef SHANDALAR
/* ManalinkEh 2022: 0x267bbb0; original functions/functions.c:2402. */
int count_cards_by_id(int player, int id)
{
  int total = 0;
  int i;
  for (i = 0; i < 2; i++)
  {
    if (player == 2 || i == player)
    {
      int count = 0;
      while (count < active_cards_count[i])
      {
        if (in_play(i, count) && !is_humiliated(i, count, AL_STATIC) &&
            get_id(i, count) == id)
        {
          total++;
        }
        count++;
      }
    }
  }
  return total;
}

#endif


#ifndef SHANDALAR
/* ManalinkEh 2022: 0x2682d30; original functions/functions.c:1868. */
int check_battlefield_for_subtype(int what_player, int type, int subtype)
{
  int p, c;
  for (p = 0; p <= 1; ++p)
    if (what_player == ANYBODY || what_player == p)
      for (c = 0; c < active_cards_count[p]; ++c)
        if (in_play(p, c) && is_what(p, c, type) &&
            (subtype == -1 || has_subtype(p, c, subtype)))
          return 1;
  return 0;
}

#endif


#ifndef SHANDALAR
/* ManalinkEh 2022: 0x26828d0; original functions/functions.c:1857. */
int count_subtype(int what_player, int type, int subtype)
{
  int p, c, count = 0;
  for (p = 0; p <= 1; ++p)
    if (what_player == ANYBODY || what_player == p)
      for (c = 0; c < active_cards_count[p]; ++c)
        if (in_play(p, c) && is_what(p, c, type) && !is_what(p, c, TYPE_EFFECT) &&
            (subtype == -1 || has_subtype(p, c, subtype)))
          ++count;
  return count;
}

#endif


#ifndef SHANDALAR
/* ManalinkEh 2022: 0x267a900; original functions/functions.c:1847. */
int count_shrines(int player, int card)
{
  target_definition_t td;
  default_target_definition(player, card, &td, TYPE_ENCHANTMENT);
  td.required_subtype = SUBTYPE_SHRINE;
  td.allowed_controller = player;
  td.preferred_controller = player;
  td.illegal_abilities = 0;
  return target_available(player, card, &td);
}

#endif


#ifndef SHANDALAR
/* ManalinkEh 2022: 0x2680e20; original functions/functions.c:6763. */
int is_token(int player, int card)
{
  card_instance_t *instance = get_card_instance(player, card);
  if (!(instance->token_status & STATUS_TOKEN))
    return 0;
  return !check_special_flags(player, card, SF_UNEARTH);
}

#endif


#ifndef SHANDALAR
/* ManalinkEh 2022: 0x26940a0; original functions/functions.c:16233. */
int is_x_spell(int player, int card)
{
  card_data_t *data = player == -1 ? &cards_data[card] : get_card_data(player, card);
  if (data->cc[1] == 255)
    return 1;
  return check_special_flags2(player, card, SF2_X_SPELL);
}

#endif


#ifndef SHANDALAR
/* ManalinkEh 2022: 0x2693940; original functions/functions.c:15586. */
int is_hardcoded_card(int player, int card)
{
  card_instance_t *instance = get_card_instance(player, card);
  return (int)cards_data[instance->internal_card_id].code_pointer < 0x02001006;
}

#endif


#ifndef SHANDALAR
/* ManalinkEh 2022: 0x2679820; original functions/functions.c:125. */
int is_valid_card(int id)
{
  if (id < 0 || id >= available_slots || !(card_coded[id / 8] & (1 << (id % 8))))
    return 0;
  int iid = get_internal_card_id_from_csv_id(id);
  if (cards_data[iid].cc[2] == 3 || cards_ptr[id]->expansion == 0x1000 ||
      cards_data[iid].type == TYPE_EFFECT)
    return 0;
  return (id & ~4) != 0x38 && id != 0x2b && id != 0x1df && id != 0x1af && id != 0x2fb &&
         id != 0x32a && id != 0x394 && id != 0x615a && id != 0x6161;
}

#endif


#ifndef SHANDALAR
/* ManalinkEh 2022: 0x26925a0; original functions/functions.c:14654. */
int is_stolen(int player, int card)
{
  if (player == 0)
    return (get_card_instance(0, card)->state & STATE_OWNED_BY_OPPONENT) != 0;
  if (player == 1)
    return (get_card_instance(1, card)->state & STATE_OWNED_BY_OPPONENT) == 0;
  return 0;
}

#endif


#ifndef SHANDALAR
/* ManalinkEh 2022: 0x267f6d0; original functions/functions.c:5998. */
int is_target(int player, int card, int controller_of_effect)
{
  for (int i = 0; i < stack_size; ++i)
  {
    if (controller_of_effect == ANYBODY || stack_cards[i].player == controller_of_effect)
    {
      card_instance_t *instance = get_card_instance(stack_cards[i].player, stack_cards[i].card);
      for (int j = 0; j < instance->number_of_targets; ++j)
        if (instance->targets[j].player == player && instance->targets[j].card == card)
          return 1;
    }
  }
  return 0;
}

#endif


#ifndef SHANDALAR
/* ManalinkEh 2022: 0x2682100; original functions/functions.c:945. */
int is_sick(int player, int card)
{
  card_instance_t *instance = get_card_instance(player, card);
  if (instance->state & STATE_SUMMON_SICK)
    if (is_what(player, card, TYPE_CREATURE))
      return !check_special_flags2(player, card, SF2_THOUSAND_YEAR_ELIXIR);
  return 0;
}

#endif


#ifndef SHANDALAR
/* ManalinkEh 2022: 0x2682170; original functions/functions.c:955. */
int is_animated_and_sick(int player, int card)
{
  card_instance_t *instance = get_card_instance(player, card);
  if (instance->state & STATE_SUMMON_SICK)
    if (is_what(player, card, TYPE_CREATURE))
      return !check_special_flags2(player, card, SF2_THOUSAND_YEAR_ELIXIR);
  return 0;
}

#endif


#ifndef SHANDALAR
/* ManalinkEh 2022: 0x26812d0; original functions/functions.c:7156. */
int is_attacking(int player, int card)
{
  return (get_card_instance(player, card)->state & STATE_ATTACKING) != 0;
}

#endif


#ifndef SHANDALAR
/* ManalinkEh 2022: 0x2681300; original functions/functions.c:7165. */
card_instance_t *in_play_and_attacking(int player, int card)
{
  card_instance_t *instance = in_play(player, card);
  if (instance && !(instance->state & STATE_ATTACKING))
    return NULL;
  return instance;
}

#endif


#ifndef SHANDALAR
/* ManalinkEh 2022: 0x268bf20; original functions/functions.c:11439. */
int check_state(int player, int card, int state)
{
  return (get_card_instance(player, card)->state & state) != 0;
}

#endif


#ifndef SHANDALAR
/* ManalinkEh 2022: 0x268bf50; original functions/functions.c:11447. */
int check_status(int player, int card, int status)
{
  return (get_card_instance(player, card)->token_status & status) != 0;
}

#endif


#ifndef SHANDALAR
/* ManalinkEh 2022: 0x2687990; original functions/functions.c:8502. */
void add_state(int player, int card, int state)
{
  if (card != -1)
  {
    get_card_instance(player, card)->state |= state;
    return;
  }
  for (int p = 0; p < 2; ++p)
    if (player == p || player == ANYBODY)
      for (int c = 0; c < active_cards_count[p]; ++c)
        if (in_play(p, c) && is_what(p, c, TYPE_CREATURE))
          get_card_instance(p, c)->state |= state;
}

#endif


#ifndef SHANDALAR
/* ManalinkEh 2022: 0x2687a50; original functions/functions.c:8524. */
void remove_state(int player, int card, int state)
{
  if (card != -1)
  {
    card_instance_t *instance = get_card_instance(player, card);
    if (instance->state & state)
      instance->state &= ~state;
    return;
  }
  for (int p = 0; p < 2; ++p)
    if (player == p || player == ANYBODY)
      for (int c = 0; c < active_cards_count[p]; ++c)
        if (in_play(p, c) && is_what(p, c, TYPE_PERMANENT))
        {
          card_instance_t *instance = get_card_instance(p, c);
          if (instance->state & state)
            instance->state &= ~state;
        }
}

#endif


#ifndef SHANDALAR
/* ManalinkEh 2022: 0x2687d40; original functions/functions.c:8550. */
void add_status(int player, int card, int status)
{
  get_card_instance(player, card)->token_status |= status;
}

#endif


#ifndef SHANDALAR
/* ManalinkEh 2022: 0x2687d70; original functions/functions.c:8555. */
void remove_status(int player, int card, int status)
{
  card_instance_t *instance = get_card_instance(player, card);
  if (instance->token_status & status)
    instance->token_status &= ~status;
}

#endif


#ifndef SHANDALAR
/* ManalinkEh 2022: 0x2687da0; original functions/functions.c:8562. */
void state_untargettable(int player, int card, int mode)
{
  card_instance_t *instance = get_card_instance(player, card);
  if (mode == 0)
  {
    if (instance->state & STATE_CANNOT_TARGET)
      instance->state &= ~STATE_CANNOT_TARGET;
  }
  else if (mode == 1)
    instance->state |= STATE_CANNOT_TARGET;
}

#endif


#ifndef SHANDALAR
/* ManalinkEh 2022: 0x2679c30; original functions/functions.c:1013. */
int tap_card(int player, int card)
{
  card_instance_t *instance = get_card_instance(player, card);
  if (!(instance->state & STATE_TAPPED))
  {
    tapped_for_mana_color = -1;
    instance->state |= STATE_TAPPED;
    dispatch_event(player, card, EVENT_TAP_CARD);
  }
  return 0;
}

#endif


#ifndef SHANDALAR
/* ManalinkEh 2022: 0x2679c90; original functions/functions.c:1027. */
void untap_card(int player, int card)
{
  card_instance_t *instance = get_card_instance(player, card);
  if (instance->state & STATE_TAPPED)
  {
    unsigned char bits = player_bits[player];
    instance->state &= ~STATE_TAPPED;
    if (bits & PB_SEND_EVENT_UNTAP_CARD_TO_ALL)
      dispatch_event(player, card, EVENT_UNTAP_CARD);
    else
      dispatch_event_with_attacker_to_one_card(player, card, EVENT_UNTAP_CARD, 1 - player, -1);
  }
}

#endif


#ifndef SHANDALAR
/* ManalinkEh 2022: 0x2679d10; original functions/functions.c:1043. */
void untap_card_no_event(int player, int card)
{
  get_card_instance(player, card)->state &= ~STATE_TAPPED;
}

#endif


#ifndef SHANDALAR
/* ManalinkEh 2022: 0x267eb80; original functions/functions.c:4193. */
int is_creature_dead(void)
{
  for (int p = 0; p < 2; ++p)
    for (int c = 0; c < active_cards_count[p]; ++c)
    {
      card_data_t *data = get_card_data(p, c);
      if ((data->type & TYPE_CREATURE) && in_play(p, c) && can_regenerate(p, c))
        return 99;
    }
  return 0;
}

#endif


#ifndef SHANDALAR
/* ManalinkEh 2022: 0x268e7a0; original functions/functions.c:13190. */
int has_dead_creature(int player)
{
  for (int p = 0; p < 2; ++p)
    if (player == p || player == ANYBODY)
      for (int *grave = graveyard_ptr_mutable[p]; *grave != -1; ++grave)
        if (cards_data[*grave].type & TYPE_CREATURE)
          return 1;
  return 0;
}

#endif


#ifndef SHANDALAR
/* ManalinkEh 2022: 0x269f2e0; original functions/functions.c:18129. */
card_instance_t *has_activation_on_stack(int player, int card)
{
  for (int c = 0; c < active_cards_count[player]; ++c)
  {
    card_instance_t *instance = get_card_instance(player, c);
    if (instance && instance->internal_card_id == activation_card &&
        instance->parent_card == card && instance->parent_controller == player)
      return instance;
  }
  return NULL;
}

#endif


#ifndef SHANDALAR
/* ManalinkEh 2022: 0x267c0c0; original functions/functions.c:2626. */
int is_basic_land_by_id(int id)
{
  return id == 0x5b || id == 0x7e || id == 0xef || id == 0xbc || id == 0xa4 ||
         (unsigned int)id - 0x834U <= 4U || id == 0x3d67;
}

#endif


#ifndef SHANDALAR
/* ManalinkEh 2022: 0x267c110; original functions/functions.c:2615. */
int is_basic_land(int player, int card)
{
  return is_basic_land_by_id(player == -1 ? cards_data[card].id : get_id(player, card));
}

#endif


#ifndef SHANDALAR
/* ManalinkEh 2022: 0x2684ee0; original functions/functions.c:8112. */
int is_mana_producer_land(int player, int card)
{
  int iid = player == -1 ? card : get_card_instance(player, card)->internal_card_id;
  return is_what(-1, iid, TYPE_LAND) && (cards_data[iid].extra_ability & EA_MANA_SOURCE);
}

#endif


#ifndef SHANDALAR
/* ManalinkEh 2022: 0x2679080; original functions/functions.c:17071. */
static void prevent_all_my_damage_impl(int player, int card, int flags)
{
  card_instance_t *damage = get_card_instance(affected_card_controller, affected_card);
  if (damage->internal_card_id == damage_card && damage->info_slot > 0 &&
      damage->damage_source_card == card && damage->damage_source_player == player)
  {
    int combat = 1;
    if (flags & DDBM_MUST_BE_COMBAT_DAMAGE)
      combat = (damage->token_status & (STATUS_COMBAT_DAMAGE | STATUS_FIRST_STRIKE_DAMAGE)) != 0;
    if ((!(flags & DDBM_MUST_DAMAGE_PLAYER) ||
         (damage->damage_target_card == -1 &&
          !check_special_flags(damage->damage_source_player, damage->damage_source_card,
                                    SF_ATTACKING_PWALKER))) &&
        (!(flags & DDBM_MUST_DAMAGE_OPPONENT) ||
         (damage->damage_target_card == -1 && damage->damage_target_player == 1 - player)) &&
        combat)
      get_card_instance(affected_card_controller, affected_card)->info_slot = 0;
  }
}

#endif


#ifndef SHANDALAR
/* ManalinkEh 2022: 0x2679180; original functions/functions.c:17096. */
static int prevent_all_damage_dealt_by_target_legacy(int player, int card, event_t event)
{
  card_instance_t *instance = get_card_instance(player, card);
  if (instance->damage_target_player >= 0 && event == EVENT_PREVENT_DAMAGE)
  {
    prevent_all_my_damage_impl(instance->damage_target_player, instance->damage_target_card,
                                    instance->targets[1].player);
    return 0;
  }
  if (event == EVENT_CLEANUP)
    kill_card(player, card, KILL_EXILE);
  return 0;
}

#endif


#ifndef SHANDALAR
/* ManalinkEh 2022: 0x2679240; original functions/functions.c:7301. */
int no_combat_damage_this_turn(int player, int card, event_t event)
{
  card_instance_t *damage = combat_damage_being_prevented(event);
  if (damage)
  {
    card_instance_t *instance = get_card_instance(player, card);
    if (damage->damage_source_card == instance->targets[0].card &&
        damage->damage_source_player == instance->targets[0].player)
      damage->info_slot = 0;
  }
  else if (event == EVENT_CLEANUP)
    kill_card(player, card, KILL_EXILE);
  return 0;
}

#endif


#ifndef SHANDALAR
/* ManalinkEh 2022: 0x26813f0; original functions/functions.c:7317. */
void negate_combat_damage_this_turn(int player, int card, int t_player, int t_card,
                                         int alternate_legacy_text_number)
{
  int legacy = create_targetted_legacy_effect(player, card, no_combat_damage_this_turn,
                                              t_player, t_card);
  if (legacy >= 0 && alternate_legacy_text_number > 0)
    alternate_legacy_text_impl(alternate_legacy_text_number, player, legacy);
}

#endif


#ifndef SHANDALAR
/* ManalinkEh 2022: 0x2683470; original functions/functions.c:16446. */
int damage_cannot_be_prevented_until_eot_legacy(int player, int card, event_t event)
{
  if (event == EVENT_PREVENT_DAMAGE)
  {
    card_instance_t *damage = get_card_instance(affected_card_controller, affected_card);
    if (damage->internal_card_id == damage_card)
      get_card_instance(affected_card_controller, affected_card)->state |= STATE_CANNOT_TARGET;
  }
  if (eot_trigger(player, card, event))
    kill_card(player, card, KILL_EXILE);
  return 0;
}

#endif


#ifndef SHANDALAR
/* ManalinkEh 2022: 0x2683660; original functions/functions.c:9225. */
int no_damage_this_turn(int player, int card, event_t event)
{
  card_instance_t *instance = get_card_instance(player, card);
  if (event == EVENT_PREVENT_DAMAGE)
  {
    card_instance_t *damage = get_card_instance(affected_card_controller, affected_card);
    if (damage->internal_card_id == damage_card && damage->info_slot > 0 &&
        damage->damage_source_player == instance->targets[0].player &&
        damage->damage_source_card == instance->targets[0].card)
      get_card_instance(affected_card_controller, affected_card)->info_slot = 0;
  }
  else if (event == EVENT_GRAVEYARD_FROM_PLAY)
  {
    if (card == affected_card && player == affected_card_controller &&
        instance->damage_target_player >= 0 && (land_can_be_played & LCBP_PENDING_DAMAGE_CARDS))
    {
      instance->damage_target_card = -1;
      instance->damage_target_player = -1;
      event_result = 1;
    }
  }
  else if (event == EVENT_AFTER_DAMAGE && instance->damage_target_player == -1 &&
           !(land_can_be_played & LCBP_PENDING_DAMAGE_CARDS))
    kill_card(player, card, KILL_EXILE);
  if (eot_trigger(player, card, event))
    kill_card(player, card, KILL_EXILE);
  return 0;
}

#endif


#ifndef SHANDALAR
/* ManalinkEh 2022: 0x26837c0; original functions/functions.c:13791. */
static int prevent_all_damage_to_target_legacy(int player, int card, event_t event)
{
  card_instance_t *instance = get_card_instance(player, card);
  if (event == EVENT_PREVENT_DAMAGE)
  {
    card_instance_t *damage = get_card_instance(affected_card_controller, affected_card);
    if (damage->internal_card_id == damage_card && damage->info_slot > 0 &&
        damage->damage_target_card == instance->targets[0].card &&
        damage->damage_target_player == instance->targets[0].player)
    {
      int good = instance->targets[1].player < 0 ||
                 (damage->damage_source_card == instance->targets[1].card &&
                  damage->damage_source_player == instance->targets[1].player);
      if (instance->targets[2].player > 1)
      {
        int color = in_play(damage->damage_source_player, damage->damage_source_card)
                        ? get_color(damage->damage_source_player, damage->damage_source_card)
                        : damage->initial_color;
        if (!(color & instance->targets[2].player))
          good = 0;
      }
      if (good)
        get_card_instance(affected_card_controller, affected_card)->info_slot = 0;
    }
  }
  if (instance->targets[2].player > 0 && (instance->targets[2].player & 1) &&
      eot_trigger(player, card, event))
    kill_card(player, card, KILL_EXILE);
  return 0;
}

#endif


#ifndef SHANDALAR
/* ManalinkEh 2022: 0x2685200; original functions/functions.c:16424. */
int my_damage_cannot_be_prevented(int player, int card, event_t event)
{
  if (event == EVENT_PREVENT_DAMAGE)
  {
    card_instance_t *instance = get_card_instance(player, card);
    int source_player =
        is_what(player, card, TYPE_EFFECT) ? instance->targets[0].player : player;
    int source_card = is_what(player, card, TYPE_EFFECT) ? instance->targets[0].card : card;
    card_instance_t *damage = get_card_instance(affected_card_controller, affected_card);
    if (damage->internal_card_id == damage_card && damage->damage_source_player == source_player &&
        damage->damage_source_card == source_card)
      get_card_instance(affected_card_controller, affected_card)->state |= STATE_CANNOT_TARGET;
  }
  else if (event == EVENT_CLEANUP && is_what(player, card, TYPE_EFFECT))
    kill_card(player, card, KILL_EXILE);
  return 0;
}

#endif


#ifndef SHANDALAR
/* ManalinkEh 2022: 0x2686f00; original functions/functions.c:8856. */
int fog_effect_legacy(int player, int card, event_t event)
{
  if (event == EVENT_PREVENT_DAMAGE)
  {
    card_instance_t *instance = get_card_instance(player, card);
    card_instance_t *damage = get_card_instance(affected_card_controller, affected_card);
    if (damage->internal_card_id == damage_card && damage->info_slot > 0 &&
        !(get_card_instance(affected_card_controller, affected_card)->state & STATE_CANNOT_TARGET))
    {
      if (instance->targets[2].player >= 0 && instance->targets[2].card >= 0 &&
          instance->targets[2].player == damage->damage_source_player &&
          instance->targets[2].card == damage->damage_source_card)
        return 0;
      fog_special_flags_t flags = instance->targets[0].card;
      if ((flags & FOG_BY_ANY_TYPE) ||
          is_what(damage->damage_source_player, damage->damage_source_card, TYPE_CREATURE))
      {
        if (instance->targets[0].player != ANYBODY &&
            instance->targets[0].player != ((flags & FOG_BY_CONTROLLED_BY_PLAYER)
                                                ? damage->damage_source_player
                                                : damage->damage_target_player))
          return 0;
        if ((flags & FOG_COMBAT_DAMAGE_ONLY) &&
            !(damage->token_status & (STATUS_COMBAT_DAMAGE | STATUS_FIRST_STRIKE_DAMAGE)))
          return 0;
        if ((flags & FOG_ONLY_DAMAGE_TO_T_PLAYER) &&
            (damage->damage_target_card != -1 || damage_is_to_planeswalker(damage)))
          return 0;
        if ((flags & FOG_UNBLOCKED_CREATURES_ONLY) &&
            !is_unblocked(damage->damage_source_player, damage->damage_source_card))
          return 0;
        if ((flags & FOG_CREATURES_WITHOUT_TRAMPLE) &&
            check_for_ability(damage->damage_source_player, damage->damage_source_card,
                              KEYWORD_TRAMPLE))
          return 0;
        if (flags & FOG_COLOR_MASK)
        {
          int color = damage->initial_color;
          if (in_play(damage->damage_source_player, damage->damage_source_card))
            color = get_color(damage->damage_source_player, damage->damage_source_card);
          if (flags & FOG_INVERSE_COLOR_CHECK)
          {
            if (color & flags)
              return 0;
          }
          else if (!(color & flags))
            return 0;
        }
        if ((flags & FOG_BY_NON_SUBTYPE) && instance->targets[1].player > 0 &&
            has_subtype(damage->damage_source_player, damage->damage_source_card,
                        instance->targets[1].player))
          return 0;
        if (instance->display_pic_csv_id == 0x2efd &&
            (has_subtype(damage->damage_source_player, damage->damage_source_card, SUBTYPE_WOLF) ||
             has_subtype(damage->damage_source_player, damage->damage_source_card,
                         SUBTYPE_WEREWOLF)))
          return 0;
        get_card_instance(affected_card_controller, affected_card)->info_slot = 0;
      }
    }
  }
  else if (event == EVENT_CLEANUP)
    kill_card(player, card, KILL_EXILE);
  return 0;
}

#endif


#ifndef SHANDALAR
/* ManalinkEh 2022: 0x26a3f20; original functions/functions.c:18701. */
int prevent_damage(int damage_player, int damage_card_index, int amount)
{
  card_instance_t *damage = get_card_instance(damage_player, damage_card_index);
  int original = damage->info_slot;
  if (amount != -1 && amount < original)
  {
    damage->info_slot = original - amount;
    return amount;
  }
  damage->info_slot = 0;
  return original;
}

#endif


#ifndef SHANDALAR
/* ManalinkEh 2022: 0x26a3f70; original functions/functions.c:8183. */
static int effect_prevent_the_next_n_damage(int player, int card, event_t event)
{
  if (event == EVENT_PREVENT_DAMAGE)
  {
    card_instance_t *instance = get_card_instance(player, card);
    card_instance_t *damage = get_card_instance(affected_card_controller, affected_card);
    if (damage->internal_card_id == damage_card &&
        damage->damage_target_player == instance->targets[0].player &&
        damage->damage_target_card == instance->targets[0].card && damage->info_slot > 0 &&
        !(get_card_instance(affected_card_controller, affected_card)->state & STATE_CANNOT_TARGET))
    {
      int amount = damage->info_slot;
      if (!(instance->targets[2].player & PREVENT_INFINITE) && instance->counter_toughness < amount)
        amount = instance->counter_toughness;
      if (amount > 0)
      {
        prevent_damage(affected_card_controller, affected_card, amount);
        if (!(instance->targets[2].player & PREVENT_INFINITE))
          instance->counter_toughness -= amount;
        if ((instance->targets[2].player & PREVENT_ADD_1_1_COUNTER) &&
            damage->damage_target_card >= 0)
          add_counters(player, card, damage->damage_target_player,
                                   damage->damage_target_card, COUNTER_P1_P1, amount);
        if (instance->targets[2].player & PREVENT_GAIN_LIFE)
          gain_life_impl(player, amount, 0);
        if ((instance->targets[2].player & PREVENT_REDIRECT) && instance->targets[1].player >= 0 &&
            (instance->targets[1].card == -1 ||
             in_play(instance->targets[1].player, instance->targets[1].card)))
          damage_creature(instance->targets[1].player, instance->targets[1].card, amount,
                          damage->damage_source_player, damage->damage_source_card);
      }
      if (instance->counter_toughness <= 0 && !(instance->targets[2].player & PREVENT_INFINITE))
        kill_card(player, card, KILL_EXILE);
      else
      {
        unsigned char *low = (unsigned char *)&instance->untap_status;
        unsigned char value = *low & 0xfc;
        value = value < 0xfc ? value + 4 : 0;
        *low = (*low & 3) | value;
      }
    }
  }
  else if (event == EVENT_SET_LEGACY_EFFECT_NAME)
  {
    card_instance_t *instance = get_card_instance(player, card);
    snprintf(set_legacy_effect_name_addr, 51, "%s (%d)",
             cards_ptr[instance->display_pic_csv_id]->name, (int)instance->counter_toughness);
  }
  if (eot_trigger(player, card, event))
    kill_card(player, card, KILL_EXILE);
  return 0;
}

#endif


#ifndef SHANDALAR
/* ManalinkEh 2022: 0x2687320; original functions/functions.c:8269. */
void prevent_the_next_n_damage(int player, int card, int t_player, int t_card, int amount,
                                    prevent_t flags, int redir_to_player, int redir_to_card)
{
  if (amount <= 0 && !(flags & PREVENT_INFINITE))
    return;
  int legacy;
  int opponent = 0;
  if (t_card != -1)
    legacy = create_targetted_legacy_effect(player, card, effect_prevent_the_next_n_damage,
                                            t_player, t_card);
  else if (player == t_player)
    legacy = create_legacy_effect(player, card, effect_prevent_the_next_n_damage);
  else
  {
    legacy = create_legacy_effect_for_opponent(player, card, effect_prevent_the_next_n_damage,
                                               -1, -1);
    player = 1 - player;
    opponent = 1;
  }
  if (legacy == -1)
    return;
  card_instance_t *instance = get_card_instance(player, legacy);
  instance->targets[0].player = t_player;
  instance->targets[0].card = t_card;
  instance->number_of_targets = 1;
  if (opponent)
    instance->state ^= STATE_OWNED_BY_OPPONENT;
  if (flags & PREVENT_REDIRECT)
  {
    if ((unsigned int)redir_to_player < 2U && (unsigned int)redir_to_card + 1U < 151U)
    {
      instance->number_of_targets = 2;
      instance->targets[1].player = redir_to_player;
      instance->targets[1].card = redir_to_card;
    }
    else
      flags &= ~PREVENT_REDIRECT;
  }
  instance->targets[2].player = flags;
  if (!(flags & PREVENT_INFINITE))
  {
    instance->eot_toughness = EVENT_SET_LEGACY_EFFECT_NAME;
    instance->counter_toughness = (int16_t)amount;
  }
  if (land_can_be_played & LCBP_DAMAGE_PREVENTION)
    for (int p = 0; p < 2; ++p)
      for (int c = 0; c < active_cards_count[p]; ++c)
      {
        card_instance_t *damage = in_play(p, c);
        if (damage && damage->internal_card_id == damage_card && !(damage->state & STATE_TAPPED))
        {
          dispatch_event_arbitrary_to_one_card(player, legacy, EVENT_PREVENT_DAMAGE, p, c, 1 - p,
                                               -1);
          if (instance->internal_card_id == -1)
            return;
        }
      }
}

#endif


#ifndef SHANDALAR
/* ManalinkEh 2022: 0x26885d0; original functions/functions.c:8952. */
int fog_special2(int player, int card, int t_player, fog_special_flags_t flags, int param)
{
  if (!(flags & FOG_ONLY_DAMAGE_TO_T_PLAYER))
  {
    int protect = ANYBODY;
    if (t_player != ANYBODY)
      protect = (flags & FOG_BY_CONTROLLED_BY_PLAYER) ? 1 - t_player : t_player;
    for (int p = 0; p < 2; ++p)
      if (protect == ANYBODY || protect == p)
        for (int c = 0; c < active_cards_count[p]; ++c)
          if (in_play(p, c) && is_planeswalker(p, c))
            set_special_flags(p, c, SF_WONT_RECEIVE_DAMAGE);
  }
  int legacy = create_legacy_effect(player, card, fog_effect_legacy);
  card_instance_t *instance = get_card_instance(player, legacy);
  instance->targets[0].player = t_player;
  instance->targets[0].card = flags;
  instance->targets[1].player = param;
  instance->targets[1].card = get_id(player, card);
  return legacy;
}

#endif


#ifndef SHANDALAR
/* ManalinkEh 2022: 0x2688700; original functions/functions.c:8982. */
int fog_special(int player, int card, int t_player, int flags)
{
  return fog_special2(player, card, t_player, flags, 0);
}

#endif


#ifndef SHANDALAR
/* ManalinkEh 2022: 0x2688730; original functions/functions.c:8986. */
int fog_effect(int player, int card)
{
  return fog_special2(player, card, ANYBODY, FOG_COMBAT_DAMAGE_ONLY, 0);
}

#endif


#ifndef SHANDALAR
/* ManalinkEh 2022: 0x2688d80; original functions/functions.c:9261. */
void negate_damage_this_turn(int player, int card, int t_player, int t_card, int unused)
{
  create_targetted_legacy_effect(player, card, no_damage_this_turn, t_player, t_card);
}

#endif


#ifndef SHANDALAR
/* ManalinkEh 2022: 0x268eaf0; original functions/functions.c:13832. */
int prevent_all_damage_to_target(int player, int card, int t_player, int t_card, int mode)
{
  int legacy;
  card_instance_t *instance;
  if (t_card == -1)
  {
    legacy = create_legacy_effect(player, card, prevent_all_damage_to_target_legacy);
    instance = get_card_instance(player, legacy);
    instance->targets[0].player = t_player;
    instance->targets[0].card = -1;
  }
  else
  {
    legacy = create_targetted_legacy_effect(player, card, prevent_all_damage_to_target_legacy,
                                            t_player, t_card);
    instance = get_card_instance(player, legacy);
  }
  instance->targets[2].player = mode;
  return legacy;
}

#endif


#ifndef SHANDALAR
/* ManalinkEh 2022: 0x268ea80; original functions/functions.c:13776. */
int prevent_all_damage(int player, int card, event_t event)
{
  if (event == EVENT_PREVENT_DAMAGE)
  {
    card_instance_t *damage = get_card_instance(affected_card_controller, affected_card);
    if (damage->internal_card_id == damage_card && damage->damage_target_card == card &&
        damage->damage_target_player == player && damage->info_slot > 0)
      get_card_instance(affected_card_controller, affected_card)->info_slot = 0;
  }
  return 0;
}

#endif


#ifndef SHANDALAR
/* ManalinkEh 2022: 0x2695540; original functions/functions.c:16322. */
void damage_target_creature_or_player(int t_player, int t_card, int amount, int source_player,
                                           int source_card)
{
  if (t_card != -1)
    damage_creature(t_player, t_card, amount, source_player, source_card);
  else
    damage_player(t_player, amount, source_player, source_card);
}

#endif


#ifndef SHANDALAR
/* ManalinkEh 2022: 0x2696100; original functions/functions.c:16461. */
int damage_cannot_be_prevented_until_eot(int player, int card)
{
  return create_legacy_effect(player, card, damage_cannot_be_prevented_until_eot_legacy);
}

#endif


#ifndef SHANDALAR
/* ManalinkEh 2022: 0x2697740; original functions/functions.c:17071. */
void prevent_all_my_damage(int player, int card, event_t event, int flags)
{
  if (event == EVENT_PREVENT_DAMAGE)
    prevent_all_my_damage_impl(player, card, flags);
}

#endif


#ifndef SHANDALAR
/* ManalinkEh 2022: 0x2697760; original functions/functions.c:17110. */
int prevent_all_damage_dealt_by_target(int player, int card, int t_player, int t_card,
                                            int flags)
{
  int legacy = create_targetted_legacy_effect(
      player, card, prevent_all_damage_dealt_by_target_legacy, t_player, t_card);
  get_card_instance(player, legacy)->targets[1].player = flags;
  return legacy;
}

#endif


#ifndef SHANDALAR
/* ManalinkEh 2022: 0x26a4230; original functions/functions.c:15594. */
int prevent_damage_until_eot_legacy(int player, int card, event_t event)
{
  card_instance_t *instance = get_card_instance(player, card);
  if (event == EVENT_PREVENT_DAMAGE)
  {
    card_instance_t *damage = get_card_instance(affected_card_controller, affected_card);
    if (damage->internal_card_id == damage_card && damage->info_slot > 0 &&
        damage->damage_target_player == instance->targets[0].player &&
        damage->damage_target_card == instance->targets[0].card)
    {
      int amount = damage->info_slot;
      int remaining = instance->targets[1].player;
      if (remaining != -1 && remaining < amount)
      {
        prevent_damage(affected_card_controller, affected_card, remaining);
        instance->targets[1].player = 0;
      }
      else
      {
        if (remaining != -1)
        {
          instance->targets[1].player = remaining - amount;
          damage = get_card_instance(affected_card_controller, affected_card);
        }
        damage->info_slot = 0;
      }
    }
  }
  if (eot_trigger(player, card, event))
    kill_card(player, card, KILL_EXILE);
  return 0;
}

#endif


#ifndef SHANDALAR
/* ManalinkEh 2022: 0x26a4350; original functions/functions.c:15628. */
int prevent_damage_until_eot(int player, int card, int t_player, int t_card, int maximum)
{
  prevent_damage(t_player, t_card, maximum);
  if (maximum != -1 && maximum <= 0)
    return -1;
  card_instance_t *damage = get_card_instance(t_player, t_card);
  int legacy = create_legacy_effect(player, card, prevent_damage_until_eot_legacy);
  card_instance_t *instance = get_card_instance(player, legacy);
  instance->targets[0].player = damage->damage_source_player;
  instance->targets[0].card = damage->damage_source_card;
  instance->targets[1].player = maximum;
  instance->targets[2].card = get_id(player, card);
  return legacy;
}

#endif
