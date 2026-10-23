"""Promote reconstructable functions into their DWARF source units.

This deliberately rejects unresolved machine state, absolute-address expressions,
unsupported types, missing dependencies and signatures incompatible with the
current headers. Clang is used only for C syntax/interface checking, not tests.
Raw exports stay in .recovery. Source mutation requires --apply.
"""
import argparse
import collections
import csv
import hashlib
import json
import re
import subprocess
from pathlib import Path


UNRESOLVED = re.compile(r"\b(?:undefined\w*|in_(?!play(?:_and_attacking)?\b|hand\b)\w+|extraout_\w+|unaff_\w+|_?DAT_[0-9a-fA-F]{8}\w*|UNK_\w+|PTR_\w+|func_0x[0-9a-fA-F]+|field_\w+|CONCAT\w*|SUB\d+|ZEXT\w*|SEXT\w*)\b")
LITERAL = r'"(?:\\.|[^"\\\r\n])*"|\'(?:\\.|[^\'\\\r\n])*\''
FUNCTION_DEFINITION = re.compile(r"\b([A-Za-z_]\w*)\s*\([^;{}()]*\)\s*\{")
STACK_AGGREGATES = ('test_definition_t', 'test_definition_2022_t', 'target_definition_t', 'target_definition_2022_t', 'token_generation_t', 'token_generation_2022_t')


def aggregate_stack_aliases(body):
    """Cross-type addresses of small locals need verified storage reconstruction."""
    declarations = {m[2]: m[1] for m in re.finditer(
        r'(?m)^\s+([A-Za-z_]\w*)\s+(\w+)\s*(?:\[[^]\n]*\])?\s*;', body)}
    return [(m[1], m[2], declarations[m[2]]) for m in re.finditer(
        r'\((' + '|'.join(STACK_AGGREGATES) + r')\s*\*\)\s*&\s*(\w+)\b', body)
        if m[2] in declarations and declarations[m[2]] != m[1]]


def strip_comments(text):
    return re.sub(LITERAL + r"|/\*.*?\*/|//[^\r\n]*", lambda m: "" if m[0].startswith(("/*", "//")) else m[0], text, flags=re.S)


def code_only(text):
    return re.sub(LITERAL, " ", text)


def bound_variables(text):
    """Ghidra emits simple named parameters and one local per declaration."""
    signature, body = code_only(text).split("{", 1)
    parameters = signature[signature.index("(") + 1:signature.rfind(")")]
    names = {m[1] for part in parameters.split(",")
             if (m := re.search(r"\b([A-Za-z_]\w*)\s*(?:\[[^]]*\])?\s*$", part))}
    declarations = re.finditer(
        r"(?m)^\s+(?!return\b|goto\b|else\b|case\b)"
        r"(?:(?:const|volatile|unsigned|signed|long|short|struct|enum)\s+)*"
        r"[A-Za-z_]\w*(?:[ \t]+\*?\s*|\s*\*\s*)"
        r"([A-Za-z_]\w*)\s*(?:\[[^]\n]*\]\s*)*(?:;|=)", body)
    names.update(m[1] for m in declarations)
    return names


def rewrite_arity_call(text, name, arity, replacement):
    edits = []
    for match in re.finditer(r"\b" + re.escape(name) + r"\s*\(", text):
        level, count = 0, 1
        for char in text[match.end():]:
            if char in "([":
                level += 1
            elif char in ")]":
                if char == ")" and level == 0:
                    if count == arity:
                        edits.append((match.start(), match.start() + len(name)))
                    break
                level -= 1
            elif char == "," and level == 0:
                count += 1
    for start, end in reversed(edits):
        text = text[:start] + replacement + text[end:]
    return text


def rewrite_argument(text, name, arity, argument, replacements):
    """Rewrite one exact argument token, after literals have been protected."""
    edits = []
    for match in re.finditer(r"\b" + re.escape(name) + r"\s*\(", text):
        start, level, arguments = match.end(), 0, []
        for at in range(start, len(text)):
            char = text[at]
            if char in "([":
                level += 1
            elif char in ")]":
                if char != ")" or level != 0:
                    level -= 1
                    continue
            if (char == "," or char == ")") and level == 0:
                arguments.append((start, at))
                start = at + 1
                if char == ")":
                    if arity is None or len(arguments) == arity:
                        for index in (range(len(arguments)) if argument is None else [argument]):
                            begin, end = arguments[index]
                            token = text[begin:end].strip()
                            if token in replacements:
                                edits.append((begin, end, replacements[token]))
                    break
    for begin, end, replacement in reversed(edits):
        text = text[:begin] + replacement + text[end:]
    return text


def project_extension_keywords(text):
    """Project only the two verified keyword words through typed instance pointers.

    extension-foundation.json proves instance.ext at 44, and the two uint32_t
    words at extension offsets 0 and 4. Other union members remain unresolved.
    """
    pointers = set(re.findall(r'\bcard_instance_t\s*\*\s*(\w+)\b', text))
    projections = []
    def replace(match):
        if match[1] not in pointers:
            return match[0]
        replacement = match[1] + '->ext->' + match[2]
        projections.append(dict(original=match[0], replacement=replacement,
                                instance_offset=44, extension_offset=4 if match[2] == 'sp_keywords2' else 0,
                                bytes=4))
        return replacement
    text = re.sub(r'\(\((\w+)->field_21\)\.ext\)->(sp_keywords2?)\b', replace, text)
    return text, projections


def project_replicate_count(text):
    """Verified one-byte extension counter at +159, through typed instances."""
    pointers=set(re.findall(r'\bcard_instance_t\s*\*\s*(\w+)\b',text));projections=[]
    def replace(match):
        if match[1] not in pointers:return match[0]
        replacement=match[1]+'->ext->times_replicate_paid'
        projections.append(dict(original=match[0],replacement=replacement,instance_offset=44,extension_offset=159,bytes=1))
        return replacement
    return re.sub(r'\(\((\w+)->field_21\)\.ext\)->times_replicate_paid\b',replace,text),projections


EXTENSION_FLAG_NAMES = ('get_special_flags',) + tuple(prefix + '_special_flags' + suffix for prefix in ('check','set','remove')
                             for suffix in ('','2','3')) + tuple('get_special_flags' + suffix + '_by_instance' for suffix in ('','2','3'))

MANA_API_NAMES = ('count_mana','choose_mana_all_one_color','produce_mana_tapped','produce_mana_tapped2','produce_mana_tapped_multi',
                  'produce_mana_tapped_all_one_color','produce_mana_tapped_all_one_color_with_default',
                  'produce_mana_tapped_any_combination_of_colors','produce_mana_tapped_all_different_colors')

TOKEN_API_NAMES = {'default_token_definition':'default_token_definition_2022',
                   'copy_token_definition':'copy_token_definition_2022', 'generate_token':'generate_token_2022',
                   'resolve_iid':'resolve_token_iid_2022', 'set_token':'set_token_2022',
                   'get_copyable_special_infos':'get_copyable_special_infos_part_2022',
                   'token_characteristic_setting_effects':'token_characteristic_setting_effects_2022',
                   'generic_token':'generic_token_2022'}

TEST_API_NAMES = {name:name+'_2022' for name in (
    'get_test_score','new_get_test_score','new_default_test_definition','default_test_definition',
    'new_make_test_in_play','make_test_in_play','new_make_test','make_test',
    'get_average_value_from_zone','select_card_from_zone','get_tutoring_denial','new_select_a_card',
    'select_a_card','select_multiple_cards_from_graveyard','select_multiple_cards_from_graveyard_to_array',
    'new_special_count_grave','new_select_target_from_grave','select_target_from_grave_source',
    'select_target_from_either_grave','check_battlefield_for_special_card','aura_attached_to_me',
    'select_multiple_cards_from_either_graveyard')}
SACRIFICE_API_NAMES = {name:name+'_2022' for name in (
    'can_sacrifice_this_as_cost','new_can_sacrifice_as_cost','max_can_sacrifice_as_cost','can_sacrifice_as_cost',
    'can_sacrifice_type_as_cost','can_cause_sacrifice','new_can_sacrifice','can_sacrifice',
    'is_nice_creature_to_sacrifice','get_best_permanent_for_sacrifice','new_get_special_permanent_for_sacrifice',
    'pick_creature_for_sacrifice','pick_permanent_for_sacrifice','pick_special_permanent_for_sacrifice',
    'new_sacrifice','mark_sacrifice','sacrifice','controller_sacrifices_a_permanent',
    'player_sacrifices_a_permanent','player_sacrifices_a_hacked_land','impose_sacrifice',
    'controller_sacrifices_a_creature','player_sacrifices_a_creature','prevent_damage_sacrificing_a_creature',
    'check_battlefield_for_sacrifice','can_activate_altar_basic','altar_basic_activation','altar_basic',
    'altar_extended','sacrifice_and_report_value')}
SACRIFICE_FLAGS_2022 = ('SAC_NO_CANCEL','SAC_DONE','SAC_JUST_MARK','SAC_ALL_OR_NONE','SAC_AS_COST',
                       'SAC_CAUSED','SAC_RETURN_CHOICE','SAC_OPTIONAL','SAC_SET_TARGETS')
SPELL_API_NAMES={'generic_spell':'generic_spell_2022','show_deck':'show_deck_2022'}
ABILITY_API_NAMES={name:name+'_2022' for name in (
    'default_pump_ability_definition','deathtouch','hexproof','indestructible','attack_if_able',
    'block_if_able','vigilance','granted_flanking','special_abilities','lure_effect','pump_attached',
    'generic_aura','generic_animating_aura','generic_animating_aura_custom_prompt',
    'generic_animating_aura_with_added_subtype')}
PUMP_API_NAMES={name:name+'_2022' for name in (
    'effect_pump_until_eot','get_sleighted_hacked_suppressed_ability','pump_until_eot',
    'pump_until_eot_merge_previous','pump_ability_until_eot_with_precomputed_sleight_hack_suppress',
    'pump_ability','pump_ability_until_eot','can_block_additional_until_eot','pump_ability_until_eot_no_repeat',
    'pump_ability_by_test','pump_creatures_until_eot_merge_pt_alternate_legacy_text',
    'pump_creatures_until_eot_merge_pt','pump_creatures_until_eot','pump_subtype_until_eot',
    'pump_color_until_eot','pump_creatures_until_eot_no_repeat','vanilla_pump','vanilla_instant_pump',
    'legacy_effect_pump_ability_until_eot','cannot_attack','humiliate','disable_all_activated_abilities',
    'disable_nonmana_activated_abilities','humiliate_and_set_pt_abilities','humility_legacy',
    'spirit_link_effect','force_a_subtype','add_a_subtype')}
ACTIVATED_API_NAMES={name:name+'_2022' for name in (
    'granted_generic_activated_ability','generic_activated_ability','attachment_granting_activated_ability',
    'equipment_granting_activated_ability','has_mana_for_activated_ability','charge_mana_for_activated_ability',
    'charge_mana_for_activated_ability_while_resolving','can_use_activated_abilities',
    'can_use_activated_abilities_by_instance','can_produce_mana','vanilla_creature_pumper',
    'generic_shade_amt_can_pump','get_cost_mod_for_activated_abilities',
    'generic_activated_ability_with_sacrifice_as_cost','generic_activated_ability_with_target_in_grave',
    'generic_activated_ability_with_tapsubtype_as_cost')}
SPELL_FLAGS_2022=('GS_CAN_TARGET','GS_COUNTERSPELL','GS_X_SPELL','GS_GRAVE_RECYCLER','GS_REGENERATION',
    'GS_OPTIONAL_TARGET','GS_CAN_ONLY_TARGET_OPPONENT','GS_LITERAL_PROMPT','GS_DAMAGE_PREVENTION','GS_AURA',
    'GS_SAC_CREATURE_AS_COST','GS_GRAVE_RECYCLER_OPP_GRAVE','GS_GRAVE_RECYCLER_BOTH_GRAVES',
    'GS_GRAVE_RECYCLER_FROM_GRAVEYARDS','GS_ONE_OR_MULTIPLE_TARGETS','GS_SAC_PERMANENT_AS_COST')
ATTACK_API_NAMES={name:name+'_2022' for name in (
    'store_attackers','resolve_declare_attackers_trigger','declare_attackers_trigger',
    'declare_attackers_trigger_test','when_attacks_pump_self','support','battle_cry','renown','retrace')}
ENTRY_API_NAMES={name:name+'_2022' for name in ('etb_ability','etb_ability_global','evolve','exploit')}
LEVEL_BUDDY_API_NAMES={name:name+'_2022' for name in ('level_up','buddy_creature')}
GLOBAL_TUTOR_API_NAMES={name:name+'_2022' for name in ('new_global_tutor','global_tutor','gain_control_impl')}
SPECIFIC_CIP_API_NAMES={name:name+'_2022' for name in ('specific_cip','new_specific_cip','new_specific_cip_global')}
LIFE_API_NAMES={name:name+'_2022' for name in ('can_lose_the_game','player_can_gain_life_as_cost','gain_life','gain_life_from_lifelink','lose_the_game','trigger_gain_life')}
LIFE_ENUM_NAMES={'XTRIGGER_REPLACE_GAIN_LIFE':'XTRIGGER2022_REPLACE_GAIN_LIFE'}
CANNOT_LOSE_API_NAMES={name:name+'_2022' for name in ('cannot_lose_the_game','cannot_lose_the_game_for_having_less_than_0_life')}
LIFE_LOSS_API_NAMES={'lose_life':'lose_life_2022','oath_of_lim_dul_lifeloss_trigger':'oath_of_lim_dul_lifeloss_trigger_2022'}
SHARED_CARD_API_NAMES={'vow':'vow_2022','lace':'lace_2022','exile_at_end_of_combat':'exile_at_end_of_combat_2022'}
DELAYED_CALLBACK_API_NAMES={'last_chance_legacy':'last_chance_legacy_2022','effect_nettling_imp':'effect_nettling_imp_2022'}
COPY_API_NAMES={'copy_spell':'copy_spell_2022','copy_spell_from_stack':'copy_spell_from_stack_2022'}
TEXT_CHANGE_API_NAMES={name:name+'_2022' for name in ('replace_all_instances_of_one_color_word_with_another','replace_all_instances_of_one_basic_land_type_word_with_another')}
TOKEN_FRONTEND_API_NAMES={name:name+'_2022' for name in ('generate_token_by_id','generate_tokens_by_id','generate_reserved_token_by_id','copy_token')}
WISH_API_NAMES={name:name+'_2022' for name in ('new_card_from_list','generic_wish_effect')}
ABILITY_FILTER_API_NAMES={'remove_abilities_by_test':'remove_abilities_by_test_2022'}
LIBRARY_REVEAL_API_NAMES={name:name+'_2022' for name in ('reveal_top_cards_of_library','reveal_top_cards_of_library_and_choose','reveal_top_cards_of_library_and_choose_type','reveal_top_cards_of_library_and_choose_fn','reveal_top_cards_of_library_and_choose_who_chooses','reveal_top_cards_of_library_and_choose_fn_who_chooses')}
TOKEN_DELAY_API_NAMES={name:name+'_2022' for name in ('haste_and_remove_eot','haste_and_sacrifice_eot','haste_and_remove_at_beginning_of_next_end_step','haste_and_sacrifice_token_at_end_of_next_turn','haste_and_remove_token_at_end_of_next_turn')}
REANIMATION_API_NAMES={name:name+'_2022' for name in ('new_reanimate_all','reanimate_permanent','reanimate_permanent_with_effect','reanimate_permanent_with_counter_and_effect','reanimate_permanent_with_function','reanimate_all','effect_unearth','ereboss_titan_effect','upkeep_trigger_mode_impl')}
REANIMATION_ENUM_VALUES=dict(REANIMATE_DEFAULT=0,REANIMATE_TAP=1,REANIMATE_NO_CONTROL_LEGACY=2,REANIMATE_UNEARTH=4,REANIMATE_ADD_BLACK_ZOMBIE=8,REANIMATE_HASTE_AND_EXILE_AT_EOT=16,REANIMATE_PLUS1_PLUS1_COUNTER=32,REANIMATE_MINUS1_MINUS1_COUNTER=64,REANIMATE_HASTE_UNTIL_EOT=128,REANIMATE_ATTACH_AS_AURA=256,REANIMATE_RETURN_TO_HAND=512,REANIMATE_ALL_UNDER_CASTERS_CONTROL=1024,REANIMATE_UNDER_OWNER_CONTROL=2048,REANIMATE_SPECIAL_R_ALL=4096,REANIMATE_HASTE_AND_EXILE_AT_YOUR_UPKEEP=8192,REANIMATE_SPECIAL_LIVING_DEAD=16384,REANIMATE_RETURN_TO_PLAY_TRANSFORMED=32768,REANIMATE_ATTACH_AURA_TO_REANIMATOR=65536,REANIMATEXTRA_RETURN_TO_HAND2=-1,REANIMATEXTRA_LEAVE_IN_GRAVEYARD=-2,REANIMATEXTRA_EXILE_FROM_GRAVEYARD=-3)
REANIMATION_DOMAIN_NAMES={name:name.replace('REANIMATE_','REANIMATE2022_',1).replace('REANIMATEXTRA_','REANIMATEXTRA2022_',1) for name in REANIMATION_ENUM_VALUES}
REANIMATION_DOMAIN_NAMES['reanimate_mode_t']='reanimate_mode_2022_t'
LEAVES_TEST_API_NAMES={name:name+'_2022' for name in ('test_leaves_play','test_leaves_play_mode')}
DAMAGE_ALL_API_NAMES={'new_damage_all':'new_damage_all_2022'}
REVEAL_DESTROY_API_NAMES={name:name+'_2022' for name in ('reveal_card_until_you_reveal_test','legacy_permanents_destroyed_this_way_accumulate')}
FLASH_API_NAMES={'can_play_cards_as_though_they_had_flash':'can_play_cards_as_though_they_had_flash_2022'}
SNOW_API_NAMES={'control_a_snow_land':'control_a_snow_land_2022'}
COMMAND_ZONE_API_NAMES={'put_commander_from_command_zone_to':'put_commander_from_command_zone_to_2022'}
TUTOR_DESTINATION_NAMES={name:name.replace('TUTOR_','TUTOR2022_',1) for name in ('TUTOR_0','TUTOR_HAND','TUTOR_DECK','TUTOR_GRAVE','TUTOR_PLAY','TUTOR_PLAY_TAPPED','TUTOR_RFG','TUTOR_EXILE','TUTOR_GET_ID','TUTOR_PLAY_ATTACKING','TUTOR_GET_POSITION','TUTOR_BOTTOM_OF_DECK','TUTOR_TPLAYER_HAND','TUTOR_BOTTOM_OF_DECK_RANDOM','TUTOR_HAND_AND_MARK','TUTOR_OPPOSITION_AGENT')}
TUTOR_ORIGIN_NAMES={name:name.replace('TUTOR_FROM_','TUTOR_FROM2022_',1) for name in ('TUTOR_FROM_DECK','TUTOR_FROM_GRAVE','TUTOR_FROM_RFG','TUTOR_FROM_HAND','TUTOR_FROM_GRAVE_NOTARGET','TUTOR_FROM_DECK_AUTO')}
TUTOR_DOMAIN_NAMES={'tutor_t':'tutor_destination_2022_t','tutor_from_t':'tutor_from_2022_t'}
ACTION_API_NAMES={name:name+'_2022' for name in ('action_on_card','new_manipulate_all','manipulate_all','get_random_permanent','shapeshift_all','enters_the_battlefield_as_copy_of_any')}
DEATH_TRIGGER_API_NAMES={name:name+'_2022' for name in ('count_for_gfp_ability','count_for_gfp_ability_and_store_values','count_for_gfp_ability_and_store_values_extra','resolve_gfp_ability')}
SPELL_TRIGGER_API_NAMES={'sp_ability_trigger':'sp_ability_trigger_2022'}
PHYREXIAN_API_NAMES={name:name+'_2022' for name in ('has_phyrexian_mana','charge_phyrexian_mana',
    'has_phyrexian_mana_for_activated_ability','charge_phyrexian_mana_for_activated_ability','phyrexian_casting_costs')}
TARGET_API_NAMES = {name:name+'_2022' for name in (
    'get_protections_from','base_target_definition','default_target_definition',
    'counterspell_validate','counterspell','counterspell_resolve_unless_pay_x',
    'opponent_or_pw_target_definition','counterspell_target_definition','counter_activated_target_definition',
    'counter_triggered_target_definition','real_validate_target','real_target_available','can_target',
    'target_available','would_validate_arbitrary_target','validate_arbitrary_target','would_validate_target',
    'would_valid_target','validate_target','valid_target','real_select_target','choose_default_target',
    'select_target','pick_target','new_pick_target','pick_next_target_noload_arbitrary','pick_next_target_arbitrary',
    'pick_next_target_noload','pick_up_to_n_targets_noload','pick_up_to_n_targets','mark_up_to_n_targets_noload',
    'pick_player_duh','target_opponent','opponent_is_valid_target','autoselect_target',
    'ninjutsu_custom_target','ninjutsu','attach_aura_to_target','targeted_aura',
    'targeted_aura_custom_prompt','disabling_targeted_aura','vanilla_aura','disabling_aura',
    'aura_with_variable_pt_boost_depending_on_condition','put_into_play_aura_attached_to_target',
    'gain_control_and_attach_as_aura','end_of_stealing_aura_effect','generic_stealing_aura',
    'when_enchanted_permanent_dies_return_aura_to_hand','cannot_be_enchanted_granted',
    'cannot_be_enchanted','enchant_player')}


def project_extension_flags(text):
    """Only the three verified four-byte flag words on typed instances."""
    pointers=set(re.findall(r'\bcard_instance_t\s*\*\s*(\w+)\b',text));projections=[]
    offsets={'special_flags':180,'special_flags2':184,'special_flags3':188}
    def replace(match):
        if match[1] not in pointers:return match[0]
        replacement=match[1]+'->ext->'+match[2]
        projections.append(dict(original=match[0],replacement=replacement,instance_offset=44,extension_offset=offsets[match[2]],bytes=4))
        return replacement
    return re.sub(r'\(\((\w+)->field_21\)\.ext\)->(special_flags[23]?)\b',replace,text),projections


def clean(text, overlapping_globals=(), local_type_declarations=(), array_boundaries=(), state_mask_replacements=(), keyword_mask_replacements=(), use_keyword_accessors_2022=False, use_extension_flags_2022=False, use_mana_api_2022=False, use_token_api_2022=False, use_test_api_2022=False, use_target_api_2022=False, use_sacrifice_api_2022=False, use_spell_api_2022=False, use_ability_api_2022=False, use_pump_api_2022=False, use_activated_api_2022=False, use_attack_api_2022=False, use_entry_api_2022=False, use_level_buddy_api_2022=False, use_phyrexian_api_2022=False, use_spell_trigger_api_2022=False, use_death_trigger_api_2022=False, use_action_api_2022=False, use_global_tutor_api_2022=False, use_specific_cip_api_2022=False, use_life_api_2022=False, use_cannot_lose_api_2022=False, use_life_loss_api_2022=False, use_shared_card_api_2022=False, use_delayed_callback_api_2022=False, use_copy_api_2022=False, use_text_change_api_2022=False, use_token_frontend_api_2022=False, use_wish_api_2022=False, use_ability_filter_api_2022=False, use_library_reveal_api_2022=False, use_token_delay_api_2022=False, use_reanimation_api_2022=False, use_leaves_test_api_2022=False, use_damage_all_api_2022=False, use_reveal_destroy_api_2022=False, use_flash_api_2022=False, use_snow_api_2022=False, use_command_zone_api_2022=False):
    text = strip_comments(text).strip()
    for replacement in state_mask_replacements:
        text = text.replace(replacement['original'], replacement['replacement'])
    literals = []
    def save_literal(match):
        literals.append(match[0])
        return f"RECOVERY_LITERAL_{len(literals)-1}"
    text = re.sub(LITERAL, save_literal, text)
    if use_reanimation_api_2022:
        text=re.sub(r'\b_func_void_int_int_int_int\s*\*','ReanimationBeforeEtb_2022 ',text)
    if use_token_api_2022:
        mapping = {**TOKEN_API_NAMES, 'token_generation_t':'token_generation_2022_t', 'token_action_t':'token_action_2022_t'}
        text = re.sub(r'\b(?:TOKEN_ACTION_\w+|' + '|'.join(mapping) + r')\b',
                      lambda m: m[0].replace('TOKEN_ACTION_', 'TOKEN2022_ACTION_') if m[0].startswith('TOKEN_ACTION_') else mapping[m[0]], text)
    for enabled,mapping in [
        (use_test_api_2022,{**TEST_API_NAMES,'test_definition_t':'test_definition_2022_t'}),
        (use_target_api_2022,{**TARGET_API_NAMES,'target_definition_t':'target_definition_2022_t'}),
        (use_sacrifice_api_2022,{**SACRIFICE_API_NAMES,'sacrifice_t':'sacrifice_2022_t',
                               **{name:name.replace('SAC_','SAC2022_') for name in SACRIFICE_FLAGS_2022}}),
        (use_spell_api_2022,{**SPELL_API_NAMES,'generic_spell_flags_t':'generic_spell_flags_2022_t',
                            **{name:name.replace('GS_','GS2022_') for name in SPELL_FLAGS_2022}}),
        (use_ability_api_2022,{**ABILITY_API_NAMES,'pump_ability_t':'pump_ability_2022_t'}),
        (use_pump_api_2022,PUMP_API_NAMES),(use_activated_api_2022,ACTIVATED_API_NAMES),
        (use_attack_api_2022,ATTACK_API_NAMES),(use_entry_api_2022,ENTRY_API_NAMES),
        (use_level_buddy_api_2022,LEVEL_BUDDY_API_NAMES),(use_phyrexian_api_2022,PHYREXIAN_API_NAMES),
        (use_spell_trigger_api_2022,SPELL_TRIGGER_API_NAMES),
        (use_death_trigger_api_2022,DEATH_TRIGGER_API_NAMES),(use_action_api_2022,{**ACTION_API_NAMES,'actions_t':'actions_2022_t'}),
        (use_global_tutor_api_2022,{**GLOBAL_TUTOR_API_NAMES,**TUTOR_DESTINATION_NAMES,**TUTOR_ORIGIN_NAMES,**TUTOR_DOMAIN_NAMES}),
        (use_specific_cip_api_2022,SPECIFIC_CIP_API_NAMES),
        (use_life_api_2022,{**LIFE_API_NAMES,**LIFE_ENUM_NAMES}),
        (use_cannot_lose_api_2022,CANNOT_LOSE_API_NAMES),
        (use_life_loss_api_2022,LIFE_LOSS_API_NAMES),
        (use_shared_card_api_2022,SHARED_CARD_API_NAMES),
        (use_delayed_callback_api_2022,DELAYED_CALLBACK_API_NAMES),
        (use_copy_api_2022,COPY_API_NAMES),(use_text_change_api_2022,TEXT_CHANGE_API_NAMES),(use_token_frontend_api_2022,TOKEN_FRONTEND_API_NAMES),(use_wish_api_2022,WISH_API_NAMES),(use_ability_filter_api_2022,ABILITY_FILTER_API_NAMES),(use_library_reveal_api_2022,LIBRARY_REVEAL_API_NAMES),(use_token_delay_api_2022,TOKEN_DELAY_API_NAMES),(use_reanimation_api_2022,{**REANIMATION_API_NAMES,**REANIMATION_DOMAIN_NAMES}),(use_leaves_test_api_2022,LEAVES_TEST_API_NAMES),(use_damage_all_api_2022,DAMAGE_ALL_API_NAMES),(use_reveal_destroy_api_2022,REVEAL_DESTROY_API_NAMES),(use_flash_api_2022,FLASH_API_NAMES),(use_snow_api_2022,SNOW_API_NAMES),(use_command_zone_api_2022,COMMAND_ZONE_API_NAMES)]:
        if enabled:
            text=re.sub(r'\b(?:'+'|'.join(mapping)+r')\b',lambda m:mapping[m[0]],text)
    for replacement in keyword_mask_replacements:
        if replacement['context'] == 'argument':
            text = rewrite_argument(text, replacement['function'], replacement['arity'], replacement['argument'],
                                    {replacement['literal']: replacement['expression']})
        else:
            assert replacement['context'] == 'extension_word'
            text = text.replace(replacement['original'], replacement['replacement'])
    text, _ = project_extension_keywords(text)
    text, _ = project_replicate_count(text)
    text, _ = project_extension_flags(text)
    if use_extension_flags_2022:
        for name in EXTENSION_FLAG_NAMES:
            text=re.sub(r'\b'+name+r'\b',name+'_2022',text)
    if use_mana_api_2022:
        for name in MANA_API_NAMES:text=re.sub(r'\b'+name+r'\b',name+'_2022',text)
    # Keep the 2020 APIs available for original callers. Recovered 2022 paths
    # use the separately recovered extension-backed implementations.
    text = re.sub(r'\bget_special_abilities_by_instance\b', 'get_special_abilities_by_instance_2022', text)
    text = re.sub(r'\bremove_special_ability\b', 'remove_special_ability_2022', text)
    if use_keyword_accessors_2022:
        text = re.sub(r'\bget_special_abilities\b', 'get_special_abilities_2022', text)
        text = re.sub(r'\bcheck_for_special_ability\b', 'check_for_special_ability_2022', text)
    # Protection modes were renumbered in 2022; keep that domain separate.
    text = re.sub(r'\bis_protected_from_mode_t\b', 'is_protected_from_mode_2022_t', text)
    text = re.sub(r'\bIPF_(DAMAGE|ATTACH_AURA|ATTACH_EQUIPMENT|BLOCK|TARGET|QUERY)\b', r'IPF2022_\1', text)
    text = re.sub(r'\bis_protected_from\b', 'is_protected_from_2022', text)
    if re.search(r'\bload_text_2022\s*\(', text):
        # Both spellings point at the first byte of the verified 249x300
        # native buffer. A string formatter needs the first char row.
        for formatter in ('scnprintf', 'snprintf', '_snprintf', 'sprintf'):
            text = rewrite_argument(text, formatter, None, None, {'text_lines':'text_lines[0]'})
    for boundary in array_boundaries:
        text = text.replace(boundary["original"], boundary["replacement"])
    # DWARF callback int (*)(int, int, event_t) is the existing CardFunction.
    text = re.sub(r"\b_func_int_int_int_event_t\s*\*", "CardFunction ", text)
    # DWARF callback int (*)(int, int), used by quantity/predicate helpers.
    text = re.sub(r"\b_func_int_int_int\s*\*", "CardPairFunction ", text)
    # Preserve the old four-argument ABI. New six-argument calls retain their
    # source/recipient pair through the separately recovered target core.
    text = rewrite_arity_call(text, "add_counters", 6, "add_counters_from_source")
    # show_deck deliberately accepts an integer address for its final label
    # argument in this source tree. Keep the localized engine strings live.
    text = rewrite_argument(text, "show_deck", 6, 5,
                            {"str_Done": "ENGINE_DONE_LABEL", "str_OK": "ENGINE_OK_LABEL"})
    text = rewrite_argument(text, "show_deck_2022", 6, 5,
                            {"str_Done": "ENGINE_DONE_LABEL", "str_OK": "ENGINE_OK_LABEL"})
    # Identical kill-code value and meaning in the two interfaces.
    text = re.sub(r"\bACT_KILL_EXILE\b", "ACT_KILL_REMOVE", text)
    # Existing auras.c documents this packed player/card engine global at
    # exactly 0x78fa30; give the recovered access a descriptive source name.
    text = re.sub(r"\b_DAT_0078fa30\b", "engine_selected_card", text)
    # These are flag arguments, not executable addresses. Keep the match
    # restricted to simple three-argument calls to this typed accessor.
    text = re.sub(r"(\bcheck_special_flags(?:_2022)?\([^(),]*,[^(),]*,)\s*0x800000(\s*\))",
                  r"\1SF_MOVE_AURA_LEGAL_TARGET\2", text)
    text = re.sub(r"(\bcheck_special_flags2(?:_2022)?\([^(),]*,[^(),]*,)\s*0x800000(\s*\))",
                  r"\1SF2_QUARUM_TRENCH_GNOMES\2", text)
    # Both DWARF units and the existing header identify this state mask.
    text = re.sub(r"(\bcheck_state\([^(),]*,[^(),]*,)\s*0x800000(\s*\))",
                  r"\1STATE_OUBLIETTED\2", text)
    text = re.sub(r"\b__(?:cdecl|regparm[123])\b", "", text)
    # Ghidra's fixed-length string-copy intrinsic has the C strncpy contract.
    text = re.sub(r"\bbuiltin_strncpy\b", "strncpy", text)
    for old, new in (("uint", "unsigned int"), ("ushort", "unsigned short"),
                     ("byte", "unsigned char"), ("uchar", "unsigned char"), ("ulonglong", "unsigned long long"),
                     ("longlong", "long long")):
        text = re.sub(r"\b" + old + r"\b", new, text)
    # The corrected EDX:EAX return can still expose the packed low word.
    # Its two uint16_t fields are fixed at offsets zero and two.
    for local in set(re.findall(r'\binst_info_t\s+(\w+)\s*;', text)):
        for suffix, member in [('_0_2_', 'id'), ('_2_2_', 'data2'), ('_4_4_', 'data')]:
            text = re.sub(r'\b' + re.escape(local) + r'\.' + suffix + r'\b', local + '.' + member, text)
        text = re.sub(r'\b' + re.escape(local) + r'\._0_4_\b(?!\s*(?:=(?!=)|[+*/%&|^-]=))',
            '((uint32_t)' + local + '.id | ((uint32_t)' + local + '.data2 << 16))', text)
        # This value returns in EDX:EAX; a scalar read packs all three fields.
        packed = '((uint64_t)' + local + '.id | ((uint64_t)' + local + '.data2 << 16) | ((uint64_t)' + local + '.data << 32))'
        text = re.sub(r'\((?:unsigned long long|uint64_t)\)\s*' + re.escape(local) + r'\b', packed, text)
    def damage_word(match):
        name, instance = match[1], match[2]
        if not re.search(r'\b(?:unsigned int|uint32_t)\s+' + re.escape(name) + r'\s*;', text):
            return match[0]
        return '\n  ' + name + ' = (uint32_t)' + instance + '->damage_on_card | ((uint32_t)(uint16_t)' + instance + '->counter_power << 16);'
    text = re.sub(r'(?m)^\s*(\w+)\._0_2_\s*=\s*(\w+)->damage_on_card\s*;\s*'
                  r'\1\._2_2_\s*=\s*\2->counter_power\s*;', damage_word, text)
    # Verified anonymous card_instance_t unions: offset 0x38 contains
    # info_slot/info_slot_fn; offset 0x4c contains eot_toughness and its bytes.
    # Retain the real member names rather than Ghidra's generated containers.
    text = re.sub(r"\((\w+)->field_26\)\.(info_slot(?:_fn)?)\b", r"\1->\2", text)
    # The four-byte slot union copied into the first word of a target entry
    # is a bit-preserving scalar copy on the verified 32-bit instance layout.
    text = re.sub(r"\*\(anon_union_4_2_[0-9a-f]+_for_card_instance_struct_26\s*\*\)"
                  r"\((\w+)->targets\s*\+\s*(\w+)\)\s*=\s*(\w+)->field_26\s*;",
                  r"\1->targets[\2].player = \3->info_slot;", text)
    text = re.sub(r"\(int32_t\)\s*(\w+)->field_26\b", r"\1->info_slot", text)
    text = re.sub(r"\((\w+)->field_31\)\.field_1\.(eot_toughness[0-3])\b", r"\1->\2", text)
    text = re.sub(r"\((\w+)->field_31\)\.(eot_toughness)\b", r"\1->\2", text)
    text = re.sub(r"\(code\s*\*\)\s*(\w+)->info_slot\b", r"\1->info_slot_fn", text)
    # DWARF identifies this events.c global as a four-byte int. Its second
    # byte is a Ghidra partial-value expression, not a real C struct member.
    text = text.replace("raw_characteristic_pt._1_1_", "((unsigned int)raw_characteristic_pt >> 8 & 0xff)")
    # DWARF + matching executable bindings prove these are four-byte ints.
    # Only scalar reads are representable here; writes to an extracted value
    # remain invalid C and are rejected by the compiler.
    for word in ("event_flags", "land_can_be_played", "trigger_cause_controller", "event_result"):
        def partial_word(match):
            offset, size = int(match[1]), int(match[2])
            assert offset + size <= 4
            return "((unsigned int)" + word + " >> " + str(offset * 8) + " & " + hex((1 << (size * 8)) - 1) + ")"
        text = re.sub(r"\b" + word + r"\._([0-3])_([12])_", partial_word, text)
    # stack_data records are four bytes; generating_event is at offset two.
    text = text.replace("*(char *)(stack_size * 4 + 0x50962e)",
                        "(char)stack_data[stack_size - 1].generating_event")
    text = text.replace("*(short *)(stack_size * 4 + 0x50962c)",
                        "stack_data[stack_size - 1].internal_card_id")
    text = rewrite_arity_call(text, "is_humiliated", 3, "is_humiliated_for_ability")
    # Verified EXE interface player_bits[2] at 0x4ef1a4, four bytes per
    # element; this load selects byte 3 of player_bits[player].
    text = re.sub(r"\*\(unsigned char \*\)\(player \* 4 \+ 0x4ef1a7\)",
                  "((unsigned int)player_bits[player] >> 24 & 0xff)", text)
    # Ghidra loses const on local string pointers. Keep parameters/return types
    # unchanged; compilation still rejects uses that require writable buffers.
    if "{" in text:
        signature, body = text.split("{", 1)
        if local_type_declarations:
            body = "\n  " + "\n  ".join(local_type_declarations) + "\n" + body
        # Opaque scalar loads/stores have known machine widths. Do not infer
        # missing parameter/return types, pointers, aggregate layouts or state.
        for old, new in (("undefined1", "uint8_t"), ("undefined2", "uint16_t"),
                         ("undefined4", "uint32_t"), ("undefined8", "uint64_t")):
            body = re.sub(r"\b" + old + r"\b", new, body)
        if not re.match(r"\s*char\s*\*", signature):
            def character_pointer(match):
                name = re.escape(match[2])
                # Byte views used by lowered copies/clears must stay writable.
                # Header signatures still reject writes through borrowed const
                # pointers and uses that require a mutable string argument.
                direct_write = re.search(r"(?:\*\s*" + name + r"\b|\b" + name +
                    r"\s*\[[^]\n]*\])\s*(?:=(?!=)|[+*/%&|^-]=)", body)
                buffer_writer = re.search(r"\b(?:scnprintf|snprintf|sprintf|vsnprintf|strcpy|strncpy|"
                    r"strcat|strncat|memcpy|memmove|memset)\s*\(\s*" + name + r"\b", body)
                return match[0] if direct_write or buffer_writer else match[1] + "const char *" + match[2] + ";"
            body = re.sub(r"(?m)^(\s+)char\s*\*\s*(\w+)\s*;", character_pointer, body)
        # These source APIs return borrowed, read-only target lists. Restore
        # const only on the locals assigned their results; writes still fail.
        target_readers = ("any_becomes_target", "becomes_target_of_spell_or_effect",
                          "any_creature_becomes_target_of_spell_or_effect", "becomes_target_of_spell")
        for local in set(re.findall(r"\b(\w+)\s*=\s*(?:" + "|".join(target_readers) + r")\s*\(", body)):
            body = re.sub(r"(?m)^(\s+)target_t\s*\*\s*" + re.escape(local) + r"\s*;",
                          r"\1const target_t *" + local + ";", body)
            # Taking the address of a member of the borrowed const list also
            # yields a pointer to const. Writes remain compiler errors.
            aliases = set(re.findall(r"\b(\w+)\s*=\s*&" + re.escape(local) + r"->(?:card|player)\b", body))
            for alias in aliases:
                body = re.sub(r"(?m)^(\s+)(int32_t|int)\s*\*\s*" + re.escape(alias) + r"\s*;",
                              r"\1const \2 *" + alias + ";", body)
        text = signature + "{" + body
    text = re.sub(r"\([\w\s]+\*\)0x0\b", "NULL", text)
    text = re.sub(r"\n[ \t]+\n", "\n", text)
    text = re.sub(r"\n{3,}", "\n\n", text)
    pointers = re.findall(r"card_instance_t\s*\*\s*(\w+)\s*;", text)
    if len(pointers) == 1:
        text = re.sub(r"\b" + re.escape(pointers[0]) + r"\b", "instance", text)
    temporaries = set(re.findall(r"\biVar\d+\b", text))
    if len(temporaries) == 1:
        text = re.sub(r"\b" + temporaries.pop() + r"\b", "result", text)
    for name in overlapping_globals:
        text = re.sub(r"\b_" + re.escape(name) + r"\b", name, text)
    # GCC 9 rejects a label immediately before the closing brace.
    text = re.sub(r"(\b[A-Za-z_]\w*:\s*)(?=\})", r"\1;\n", text)
    text = re.sub(r"\bRECOVERY_LITERAL_(\d+)\b", lambda m: literals[int(m[1])], text)
    text = re.sub(r"\)\s*\n\s*\n\{", ")\n{", text)
    return text + "\n"


def main():
    parser = argparse.ArgumentParser(__doc__)
    parser.add_argument("--apply", action="store_true")
    parser.add_argument("--max-characters", type=int, default=3000)
    args = parser.parse_args()
    root = Path(__file__).resolve().parents[2]
    analysis = root / ".recovery"
    exports = analysis / "decompiled-abi"
    local_types_path = root / "docs/recovery-evidence/local-enums.json"
    local_types = json.loads(local_types_path.read_text())["functions"] if local_types_path.exists() else {}
    card_ids_path = root / "docs/recovery-evidence/local-card-ids.json"
    if card_ids_path.exists():
        for address, item in json.loads(card_ids_path.read_text())["functions"].items():
            if address in local_types:
                old = local_types[address]
                assert old["name"] == item["name"] and old["source"] == item["source"]
                old["types"] += item["types"]
                old["declarations"] += item["declarations"]
            else:
                local_types[address] = item
    clang = Path("C:/Program Files/LLVM/bin/clang.exe")
    source_text = {p.relative_to(root).as_posix(): p.read_bytes().decode("utf-8", errors="replace")
                   for folder in ("cards", "functions") for p in (root / folder).glob("*.c")}
    all_sources = "\n".join(source_text.values())
    definitions = set(FUNCTION_DEFINITION.findall(code_only(strip_comments(all_sources))))
    file_definitions = {path:set(FUNCTION_DEFINITION.findall(code_only(strip_comments(text))))
                        for path,text in source_text.items()}
    # Existing set-local helpers and some older shared functions have no
    # declaration in manalink.h. Use their actual source signatures, not the
    # newer binary's potentially changed interfaces, to check new callers.
    source_prototypes = collections.defaultdict(list)
    for source, text in source_text.items():
        for match in re.finditer(r"(?m)^[ \t]*([A-Za-z_][\w\s*]*?\b([A-Za-z_]\w*)\s*\([^;{}()]*\))\s*\{", strip_comments(text)):
            signature, name = match.groups()
            if name in {"if", "for", "while", "switch", "return", "sizeof"}:
                continue
            if name in file_definitions[source]:
                source_prototypes[name].append(dict(source=source, prototype=signature.strip() + ";",
                                                    static=bool(re.search(r"\bstatic\b", signature))))
    functions = {}
    for row in csv.DictReader((analysis / "2022-functions.csv").open()):
        if row["low_pc"] and row["high_pc"]:
            functions[int(row["low_pc"], 16)] = row
    from inspect_release import Image
    from recover_literals import recover_strings
    image = Image((analysis / "2022-ManalinkEh.dll").read_bytes())
    string_aliases = collections.defaultdict(dict)
    aliases_file = analysis / "string-aliases.tsv"
    if aliases_file.exists():
        for row in csv.DictReader(aliases_file.open(), delimiter="\t"):
            string_aliases[int(row["function"], 16)][row["alias"]] = int(row["address"], 16)
    address_symbols = collections.defaultdict(set)
    for symbol, offset, section, kind, storage in image.functions():
        address_symbols[image.pe.OPTIONAL_HEADER.ImageBase + image.pe.sections[section-1].VirtualAddress + offset].add(symbol)
    static_addresses = {image.pe.OPTIONAL_HEADER.ImageBase + image.pe.sections[s-1].VirtualAddress + v
                        for n, v, s, t, storage in image.functions() if storage == 3}
    static_names = {n[1:] for n,v,s,t,storage in image.functions() if storage == 3 and "." not in n}
    overlapping_globals = [f[1] for line in (analysis / "absolute-interfaces.tsv").read_text().splitlines()
                           if (f := line.split("\t"))[2] == "global"]
    candidates, rejected = {}, {}
    recovered_interfaces=json.loads((root / 'docs/recovery-evidence/interface-recovery.json').read_text())
    token_interface_ready = 'token_core_2022' in recovered_interfaces
    test_interface_ready = 'test_core_2022' in recovered_interfaces
    sacrifice_interface_ready = 'sacrifice_core_2022' in recovered_interfaces
    spell_interface_ready = 'generic_spell_2022' in recovered_interfaces
    ability_interface_ready = 'ability_writers_2022' in recovered_interfaces
    pump_interface_ready = 'pump_core_2022' in recovered_interfaces
    activated_interface_ready = 'activated_core_2022' in recovered_interfaces
    attack_interface_ready = 'attacker_core_2022' in recovered_interfaces
    entry_interface_ready = 'entry_triggers_2022' in recovered_interfaces
    level_buddy_interface_ready = 'level_buddy_2022' in recovered_interfaces
    phyrexian_interface_ready = 'phyrexian_costs_2022' in recovered_interfaces
    spell_trigger_interface_ready = 'spell_trigger_native_2022' in recovered_interfaces
    death_trigger_interface_ready = 'death_trigger_core_2022' in recovered_interfaces
    action_interface_ready = 'action_core_2022' in recovered_interfaces
    global_tutor_interface_ready = 'global_tutor_core_2022' in recovered_interfaces
    specific_cip_interface_ready = 'specific_cip_2022' in recovered_interfaces
    life_interface_ready = 'life_core_2022' in recovered_interfaces
    cannot_lose_interface_ready = 'cannot_lose_core_2022' in recovered_interfaces
    life_loss_interface_ready = 'life_loss_2022' in recovered_interfaces
    shared_card_interface_ready = 'shared_card_frontends_2022' in recovered_interfaces
    delayed_callback_interface_ready = 'delayed_callbacks_2022' in recovered_interfaces
    copy_interface_ready = 'spell_copy_2022' in recovered_interfaces
    text_change_interface_ready = 'text_change_helpers_2022' in recovered_interfaces
    token_frontend_interface_ready = 'token_frontends_2022' in recovered_interfaces
    wish_interface_ready = 'wish_helpers_2022' in recovered_interfaces
    ability_filter_interface_ready = 'ability_filter_2022' in recovered_interfaces
    library_reveal_interface_ready = 'library_reveal_2022' in recovered_interfaces
    token_delay_interface_ready = 'token_delay_2022' in recovered_interfaces
    reanimation_interface_ready = 'reanimation_2022' in recovered_interfaces
    leaves_test_interface_ready = 'leaves_test_native_2022' in recovered_interfaces
    damage_all_interface_ready = 'native_aggregate_consumers_2022' in recovered_interfaces
    reveal_destroy_interface_ready = 'reveal_destroy_native_2022' in recovered_interfaces
    flash_interface_ready = 'flash_casting_native_2022' in recovered_interfaces
    snow_interface_ready = 'snow_consumers_native_2022' in recovered_interfaces
    command_zone_interface_ready = 'commander_consumers_native_2022' in recovered_interfaces
    recovered_manifest = root / 'docs/recovery-evidence/recovered-functions.json'
    recovered_state = json.loads(recovered_manifest.read_text()) if recovered_manifest.exists() else {}
    recovered_addresses = {f['address']:f['name'] for f in recovered_state.get('accepted_functions',[])}
    target_interface_ready = all(key in recovered_interfaces for key in ['target_initializers_2022','target_validation_2022','target_selection_2022'])
    storage_path = root / 'docs/recovery-evidence/stack-storage.json'
    storage_evidence = json.loads(storage_path.read_text()) if storage_path.exists() else {}
    masks_path = root / 'docs/recovery-evidence/state-masks.json'
    state_masks = json.loads(masks_path.read_text())['functions'] if masks_path.exists() else {}
    keyword_path = root / 'docs/recovery-evidence/keyword-masks.json'
    keyword_masks = json.loads(keyword_path.read_text())['functions'] if keyword_path.exists() else {}
    for row in csv.DictReader((exports / "export.tsv").open(), delimiter="\t"):
        name = row["name"]
        if row['source'] in recovered_state.get('out_of_scope_source_units',{}):
            rejected[name]='out of scope: '+recovered_state['out_of_scope_source_units'][row['source']]
            continue
        if row["status"] != "ok" or not row["source"].endswith(".c"):
            rejected[name] = "unsupported export or C++ source"
            continue
        address = int(row["address"], 16)
        if row['address'] in recovered_addresses:
            rejected[name] = 'native address already recovered as '+recovered_addresses[row['address']]
            continue
        static = address in static_addresses
        if name in (file_definitions.get(row["source"], set()) if static else definitions):
            rejected[name] = "implementation already present in source"
            continue
        if "_" + name not in address_symbols[int(row["address"], 16)]:
            rejected[name] = "compiler clone or noncanonical entry"
            continue
        raw = (exports / (row["address"] + ".c")).read_text(encoding="utf-8")
        # DWARF compiler parts can share the public name. A raw self-call may
        # actually target a distinct private entry; defer it until that binding
        # and the private implementation have been recovered explicitly.
        raw_code=code_only(strip_comments(raw))
        if '{' in raw_code and re.search(r'\b'+re.escape(name)+r'\s*\(',raw_code.split('{',1)[1]):
            from capstone import Cs, CS_ARCH_X86, CS_MODE_32
            from capstone.x86_const import X86_OP_IMM
            bound=functions[address]
            code=image.pe.get_data(address-image.pe.OPTIONAL_HEADER.ImageBase,int(bound['high_pc'],16)-address)
            decoder=Cs(CS_ARCH_X86,CS_MODE_32);decoder.detail=True
            clone_calls=[i for i in decoder.disasm(code,address) if i.mnemonic in {'call','jmp'} and
                         i.operands[0].type==X86_OP_IMM and i.operands[0].imm!=address and
                         functions.get(i.operands[0].imm,{}).get('name')==name]
            if clone_calls:
                rejected[name]='public/private compiler-part name binding awaiting recovery'
                continue
        if re.search(r"WARNING:.*(?:bad instruction|Could not recover|uninitialized)", raw, re.I):
            rejected[name] = "decompiler control-flow warning"
            continue
        resolved, recovered_literals = recover_strings(strip_comments(raw), image, string_aliases[address])
        if (spell_interface_ready and re.search(r'\bGS_(?:GRAVE_RECYCLER_FROM_GRAVEYARDS|ONE_OR_MULTIPLE_TARGETS|SAC_PERMANENT_AS_COST)\b',code_only(resolved))
            and not re.search(r'\bgeneric_spell\s*\(',code_only(resolved))
            and not ('activation_costs_2022' in recovered_interfaces and
                         re.search(r'\bgeneric_activated_ability_with_target_in_grave\s*\(',code_only(resolved)))):
            rejected[name]='native generic-spell flags awaiting a recovered consumer'
            continue
        local = local_types.get(row["address"], {})
        if local:
            assert local["source"] == row["source"] and local["name"] == name
        state_proof = state_masks.get(row['address'])
        if state_proof:
            assert state_proof['name'] == name and state_proof['source'] == row['source']
            assert state_proof['raw_sha256'] == hashlib.sha256(raw.encode()).hexdigest()
        keyword_proof = keyword_masks.get(row['address'])
        if keyword_proof:
            assert keyword_proof['name'] == name and keyword_proof['source'] == row['source']
            assert keyword_proof['raw_sha256'] == hashlib.sha256(raw.encode()).hexdigest()
        use_token_interface = token_interface_ready and bool(re.search(r'\btoken_generation_t\b', strip_comments(raw)))
        prepared_storage = storage_evidence.get('candidates', {}).get(row['address'])
        if use_token_interface and prepared_storage:
            rejected[name] = '76-byte token storage awaiting layout migration'
            continue
        if ability_interface_ready and prepared_storage and re.search(r'\bpump_ability_t\b',strip_comments(raw)):
            rejected[name] = '48-byte pump storage awaiting layout migration'
            continue
        if prepared_storage and ((test_interface_ready and re.search(r'\btest_definition_t\b',strip_comments(raw))) or
                                 (target_interface_ready and re.search(r'\btarget_definition_t\b',strip_comments(raw)))):
            rejected[name]='Native test/target storage awaiting layout migration'
            continue
        body = clean(resolved, overlapping_globals, local.get("declarations", []), local.get("array_boundaries", []),
                     state_proof['replacements'] if state_proof else [], keyword_proof['replacements'] if keyword_proof else [],
                     use_keyword_accessors_2022=True, use_extension_flags_2022=True, use_mana_api_2022=True,
                     use_token_api_2022=use_token_interface,use_test_api_2022=test_interface_ready,use_target_api_2022=target_interface_ready,
                     use_sacrifice_api_2022=sacrifice_interface_ready,use_spell_api_2022=spell_interface_ready,
                     use_ability_api_2022=ability_interface_ready,use_pump_api_2022=pump_interface_ready,
                     use_activated_api_2022=activated_interface_ready,
                     use_attack_api_2022=attack_interface_ready,
                     use_entry_api_2022=entry_interface_ready,
                     use_level_buddy_api_2022=level_buddy_interface_ready,
                     use_phyrexian_api_2022=phyrexian_interface_ready,
                     use_spell_trigger_api_2022=spell_trigger_interface_ready,
                     use_death_trigger_api_2022=death_trigger_interface_ready,
                     use_action_api_2022=action_interface_ready,
                     use_global_tutor_api_2022=global_tutor_interface_ready,
                     use_specific_cip_api_2022=specific_cip_interface_ready,
                     use_life_api_2022=life_interface_ready,
                     use_cannot_lose_api_2022=cannot_lose_interface_ready,
                     use_life_loss_api_2022=life_loss_interface_ready,
                     use_shared_card_api_2022=shared_card_interface_ready,
                     use_delayed_callback_api_2022=delayed_callback_interface_ready,
                     use_copy_api_2022=copy_interface_ready,
                     use_text_change_api_2022=text_change_interface_ready,
                     use_token_frontend_api_2022=token_frontend_interface_ready,
                     use_wish_api_2022=wish_interface_ready,
                     use_ability_filter_api_2022=ability_filter_interface_ready,
                     use_library_reveal_api_2022=library_reveal_interface_ready,
                     use_token_delay_api_2022=token_delay_interface_ready,
                     use_reanimation_api_2022=reanimation_interface_ready,
                     use_leaves_test_api_2022=leaves_test_interface_ready,
                     use_damage_all_api_2022=damage_all_interface_ready,
                     use_reveal_destroy_api_2022=reveal_destroy_interface_ready,
                     use_flash_api_2022=flash_interface_ready,
                     use_snow_api_2022=snow_interface_ready,
                     use_command_zone_api_2022=command_zone_interface_ready)
        storage_proof = None
        prepared_storage = storage_evidence.get('candidates', {}).get(row['address'])
        if prepared_storage:
            assert prepared_storage['source'] == row['source'] and prepared_storage['name'] == name
            assert prepared_storage['raw_sha256'] == hashlib.sha256(raw.encode()).hexdigest()
            assert aggregate_stack_aliases(body) == [tuple(a) for a in prepared_storage['expected_aliases']]
            from recover_stack_storage import rewrite
            previous_body = body
            body, views = rewrite(body, aggregate_stack_aliases(body), prepared_storage['groups'], prepared_storage['target_sizes'])
            from resolve_stack_partials import resolve_partials
            body, partials, unresolved_partials = resolve_partials(body, views, storage_evidence['target_layouts'])
            body = subprocess.run(["C:/Program Files/LLVM/bin/clang-format.exe",
                '-style={BasedOnStyle: LLVM, IndentWidth: 2, BreakBeforeBraces: Allman, ColumnLimit: 100}'],
                input=body.encode(), capture_output=True, check=True).stdout.decode()
            from audit_source import function_body
            storage_proof = dict(storage=views, frame_base=prepared_storage['frame_base'],
                previous_body_sha256=hashlib.sha256(function_body(previous_body, name).encode()).hexdigest(),
                body_sha256=hashlib.sha256(function_body(body, name).encode()).hexdigest(), scope=prepared_storage['scope'],
                resolved_partial_views=partials, unresolved_partial_views=unresolved_partials)
        code = code_only(strip_comments(body))
        if aggregate_stack_aliases(code):
            rejected[name] = "overlapping stack objects awaiting verified storage"
            continue
        if (re.search(r'=\s*0x15\s*;', code) and re.search(r'target_definition(?:_2022)?_t\s*\*\)\(\(int\)',code)):
            rejected[name] = '84-byte target-definition copy awaiting trailing-flags layout migration'
            continue
        if UNRESOLVED.search(code) or re.search(r"::|\bthis\b|\._\d|\.field_", code):
            rejected[name] = "unresolved machine state or layout"
            continue
        # Literal executable addresses are still unresolved interfaces.
        literals = re.findall(r"\b0x[0-9a-fA-F]+\b", code)
        if any(0x400000 <= int(x, 16) < 0xa00000 or 0x2000000 <= int(x, 16) < 0x2800000 for x in literals):
            rejected[name] = "absolute address or ambiguous large constant"
            continue
        if len(body) > args.max_characters:
            rejected[name] = "larger function awaiting focused reconstruction"
            continue
        if state_proof or keyword_proof:
            body = subprocess.run(['C:/Program Files/LLVM/bin/clang-format.exe',
                '-style={BasedOnStyle: LLVM, IndentWidth: 2, BreakBeforeBraces: Allman, ColumnLimit: 100}'],
                input=body.encode(), capture_output=True, check=True).stdout.decode().replace('\r\n', '\n')
        if static:
            body = "static " + body
        signature = body[:body.index("{")].strip()
        metadata = functions.get(address, {})
        candidates[name] = dict(name=name, address=row["address"], source=row["source"],
                                line=metadata.get("line", ""), static=static,
                                body=body, prototype=signature + ";", raw_sha256=hashlib.sha256(raw.encode()).hexdigest(),
                                recovered_literals=recovered_literals)
        _, keyword_projections = project_extension_keywords(strip_comments(raw))
        if keyword_projections:
            candidates[name]['extension_keyword_projections'] = keyword_projections
            candidates[name]['extension_layout_evidence'] = 'extension-foundation.json'
        _, replicate_projections = project_replicate_count(strip_comments(raw))
        if replicate_projections:
            candidates[name]['extension_replicate_projections'] = replicate_projections
            candidates[name]['extension_layout_evidence'] = 'extension-foundation.json'
        _, flag_projections = project_extension_flags(strip_comments(raw))
        if flag_projections:
            candidates[name]['extension_flag_projections'] = flag_projections
            candidates[name]['extension_layout_evidence'] = 'extension-foundation.json'
        if state_proof:
            from audit_source import function_body
            candidates[name].update(state_mask_evidence=state_proof,
                reconstruction='State masks named from verified per-unit DWARF enumerators and machine operands; numeric value and operators preserved.',
                reconstruction_body_sha256=hashlib.sha256(function_body(body,name).encode()).hexdigest())
        if keyword_proof:
            from audit_source import function_body
            candidates[name].update(keyword_mask_evidence=keyword_proof,
                reconstruction='Keyword masks named only in verified keyword arguments/extension words; values and operations preserved.',
                reconstruction_body_sha256=hashlib.sha256(function_body(body,name).encode()).hexdigest())
        packed_views = {local:len(re.findall(r'\(ulonglong\)\s*' + re.escape(local) + r'\b', raw))
                        for local in re.findall(r'\binst_info_t\s+(\w+)\s*;', raw)}
        packed_views = {local:count for local,count in packed_views.items() if count}
        if packed_views:
            candidates[name]['packed_information_value_views'] = packed_views
            candidates[name]['extension_layout_evidence'] = 'extension-foundation.json'
        if storage_proof:
            candidates[name]['stack_storage_reconstruction'] = storage_proof
            candidates[name]['reconstruction'] = storage_proof['scope']
        if local:
            candidates[name]["local_type_declarations"] = local["declarations"]
            candidates[name]["local_enum_evidence"] = local["types"]
            candidates[name]["array_boundaries"] = local.get("array_boundaries", [])
            if local.get('array_boundary_evidence'):
                candidates[name]['array_boundary_evidence'] = local['array_boundary_evidence']
        if "is_humiliated_for_ability(" in body:
            candidates[name]["interface_adaptations"] = ["Three-argument is_humiliated calls use is_humiliated_for_ability."]
        adaptations = candidates[name].setdefault("interface_adaptations", [])
        for enabled,label,typename,names in [(test_interface_ready,'test','test_definition_2022_t',TEST_API_NAMES),
                                             (target_interface_ready,'target','target_definition_2022_t',TARGET_API_NAMES)]:
            calls={old:new for old,new in names.items() if re.search(r'\b'+old+r'\s*\(',code_only(strip_comments(raw)))}
            definition_present=bool(re.search(r'\b'+typename+r'\b',body))
            if enabled and (definition_present or calls):
                candidates[name][label+'_interface_2022']=dict(type=typename,definition_present=definition_present,calls=calls)
                adaptations.append('Verified 2022 '+label+' layout and recovered APIs; original aggregate interfaces retained.')
        if re.search(r'\btoken_generation_2022_t\b', body):
            candidates[name]['token_interface_2022'] = dict(type='token_generation_2022_t',
                actions=sorted(set(re.findall(r'\bTOKEN_ACTION_\w+\b',code_only(strip_comments(raw))))),
                calls={old:new for old,new in TOKEN_API_NAMES.items() if re.search(r'\b'+old+r'\s*\(',code_only(strip_comments(raw)))})
            adaptations.append('Versioned 76-byte token definition, native action values and recovered token APIs; original token interfaces retained.')
        if sacrifice_interface_ready:
            calls={old:new for old,new in SACRIFICE_API_NAMES.items() if re.search(r'\b'+old+r'\s*\(',code_only(strip_comments(raw)))}
            flags=[flag for flag in SACRIFICE_FLAGS_2022 if re.search(r'\b'+flag+r'\b',code_only(strip_comments(raw)))]
            definition_present=bool(re.search(r'\bsacrifice_2022_t\b',body))
            if calls or flags or definition_present:
                candidates[name]['sacrifice_interface_2022']=dict(type='sacrifice_2022_t',definition_present=definition_present,calls=calls,flags=flags)
                adaptations.append('Native sacrifice flags and recovered 2022 APIs; original flag and aggregate domains retained.')
        if spell_interface_ready:
            calls={old:new for old,new in SPELL_API_NAMES.items() if re.search(r'\b'+old+r'\s*\(',code_only(strip_comments(raw)))}
            flags=[flag for flag in SPELL_FLAGS_2022 if re.search(r'\b'+flag+r'\b',code_only(strip_comments(raw)))]
            definition_present=bool(re.search(r'\bgeneric_spell_flags_2022_t\b',body))
            if calls or flags or definition_present:
                candidates[name]['spell_interface_2022']=dict(type='generic_spell_flags_2022_t',definition_present=definition_present,calls=calls,flags=flags)
                adaptations.append('Native generic-spell flags and recovered core/display APIs; original flag and aggregate domains retained.')
        if ability_interface_ready:
            calls={old:new for old,new in ABILITY_API_NAMES.items() if re.search(r'\b'+old+r'\s*\(',code_only(strip_comments(raw)))}
            definition_present=bool(re.search(r'\bpump_ability_2022_t\b',body))
            if calls or definition_present:
                candidates[name]['ability_interface_2022']=dict(type='pump_ability_2022_t',definition_present=definition_present,calls=calls)
                adaptations.append('Native extension-backed ability writers and 48-byte pump record; original interfaces retained.')
        if pump_interface_ready:
            calls={old:new for old,new in PUMP_API_NAMES.items() if re.search(r'\b'+old+r'\s*\(',code_only(strip_comments(raw)))}
            if calls:
                candidates[name]['pump_interface_2022']=dict(calls=calls)
                adaptations.append('Native pump-effect/cleanup APIs and subtype frontends; original interfaces retained.')
        if activated_interface_ready:
            calls={old:new for old,new in ACTIVATED_API_NAMES.items() if re.search(r'\b'+old+r'\s*\(',code_only(strip_comments(raw)))}
            if calls:
                candidates[name]['activated_interface_2022']=dict(calls=calls)
                adaptations.append('Native granted activation, mana/cost and permission APIs; original interfaces retained.')
        if attack_interface_ready:
            calls={old:new for old,new in ATTACK_API_NAMES.items() if re.search(r'\b'+old+r'\s*\(',code_only(strip_comments(raw)))}
            if calls:
                candidates[name]['attack_interface_2022']=dict(calls=calls)
                adaptations.append('Native attacker storage/resolution and keyword helper APIs; original interfaces retained.')
        if entry_interface_ready:
            calls={old:new for old,new in ENTRY_API_NAMES.items() if re.search(r'\b'+old+r'\s*\(',code_only(strip_comments(raw)))}
            if calls:
                candidates[name]['entry_interface_2022']=dict(calls=calls)
                adaptations.append('Native entry-trigger suppression/repetition and Evolve/Exploit helper APIs; original interfaces retained.')
        if level_buddy_interface_ready:
            calls={old:new for old,new in LEVEL_BUDDY_API_NAMES.items() if re.search(r'\b'+old+r'\s*\(',code_only(strip_comments(raw)))}
            if calls:
                candidates[name]['level_buddy_interface_2022']=dict(calls=calls)
                adaptations.append('Native Level Up and Buddy APIs with verified private compiler parts; original interfaces retained.')
        if phyrexian_interface_ready:
            calls={old:new for old,new in PHYREXIAN_API_NAMES.items() if re.search(r'\b'+old+r'\s*\(',code_only(strip_comments(raw)))}
            if calls:
                candidates[name]['phyrexian_interface_2022']=dict(calls=calls)
                adaptations.append('Native Phyrexian availability, casting and activated-cost APIs; original interfaces retained.')
        if command_zone_interface_ready:
            translations={old:new for old,new in COMMAND_ZONE_API_NAMES.items() if re.search(r'\b'+old+r'\b',code_only(strip_comments(raw)))}
            if translations:
                candidates[name]['command_zone_interface_2022']=dict(translations=translations)
        if snow_interface_ready:
            translations={old:new for old,new in SNOW_API_NAMES.items() if re.search(r'\b'+old+r'\b',code_only(strip_comments(raw)))}
            if translations:
                candidates[name]['snow_interface_2022']=dict(translations=translations)
        if flash_interface_ready:
            translations={old:new for old,new in FLASH_API_NAMES.items() if re.search(r'\b'+old+r'\b',code_only(strip_comments(raw)))}
            if translations:
                candidates[name]['flash_interface_2022']=dict(translations=translations)
        if reveal_destroy_interface_ready:
            translations={old:new for old,new in REVEAL_DESTROY_API_NAMES.items() if re.search(r'\b'+old+r'\b',code_only(strip_comments(raw)))}
            if translations:
                candidates[name]['reveal_destroy_interface_2022']=dict(translations=translations)
        if damage_all_interface_ready:
            translations={old:new for old,new in DAMAGE_ALL_API_NAMES.items() if re.search(r'\b'+old+r'\b',code_only(strip_comments(raw)))}
            if translations:
                candidates[name]['damage_all_interface_2022']=dict(translations=translations)
        if leaves_test_interface_ready:
            translations={old:new for old,new in LEAVES_TEST_API_NAMES.items() if re.search(r'\b'+old+r'\b',code_only(strip_comments(raw)))}
            if translations:
                candidates[name]['leaves_test_interface_2022']=dict(translations=translations)
        if reanimation_interface_ready:
            translations={old:new for old,new in {**REANIMATION_API_NAMES,**REANIMATION_DOMAIN_NAMES}.items() if re.search(r'\b'+old+r'\b',code_only(strip_comments(raw)))}
            if translations:
                candidates[name]['reanimation_interface_2022']=dict(translations=translations)
        if token_delay_interface_ready:
            translations={old:new for old,new in TOKEN_DELAY_API_NAMES.items() if re.search(r'\b'+old+r'\b',code_only(strip_comments(raw)))}
            if translations:
                candidates[name]['token_delay_interface_2022']=dict(translations=translations)
                adaptations.append('Native haste/delayed-removal helpers and typed 76-byte token callbacks; original APIs retained.')
        if library_reveal_interface_ready:
            calls={old:new for old,new in LIBRARY_REVEAL_API_NAMES.items() if re.search(r'\b'+old+r'\s*\(',code_only(strip_comments(raw)))}
            if calls:
                candidates[name]['library_reveal_interface_2022']=dict(calls=calls)
                adaptations.append('Native library reveal and selection helpers with 216-byte filters; existing engine deck-viewer boundary retained.')
        if ability_filter_interface_ready:
            calls={old:new for old,new in ABILITY_FILTER_API_NAMES.items() if re.search(r'\b'+old+r'\s*\(',code_only(strip_comments(raw)))}
            if calls:
                candidates[name]['ability_filter_interface_2022']=dict(calls=calls)
                adaptations.append('Native ability-suppression helper with the recovered 216-byte filter API; legacy interface retained.')
        if wish_interface_ready:
            calls={old:new for old,new in WISH_API_NAMES.items() if re.search(r'\b'+old+r'\s*\(',code_only(strip_comments(raw)))}
            if calls:
                candidates[name]['wish_interface_2022']=dict(calls=calls,dialog_selection_stubbed=True)
                adaptations.append('Native filter-aware Wish/card-list helpers; built-in chooser UI is explicitly stubbed out of scope.')
        if token_frontend_interface_ready:
            calls={old:new for old,new in TOKEN_FRONTEND_API_NAMES.items() if re.search(r'\b'+old+r'\s*\(',code_only(strip_comments(raw)))}
            if calls:
                candidates[name]['token_frontend_interface_2022']=dict(calls=calls)
                adaptations.append('Native token ID/copy frontends, including the single-token integer result; original interfaces retained.')
        if text_change_interface_ready:
            calls={old:new for old,new in TEXT_CHANGE_API_NAMES.items() if re.search(r'\b'+old+r'\s*\(',code_only(strip_comments(raw)))}
            if calls:
                candidates[name]['text_change_interface_2022']=dict(calls=calls)
                adaptations.append('Native four-argument color and basic-land text replacement APIs; original source interfaces retained.')
        if copy_interface_ready:
            calls={old:new for old,new in COPY_API_NAMES.items() if re.search(r'\b'+old+r'\s*\(',code_only(strip_comments(raw)))}
            if calls:
                candidates[name]['copy_interface_2022']=dict(calls=calls)
                adaptations.append('Native spell-copy APIs with preserved flags and copy-event dispatch; original source helpers retained.')
        if delayed_callback_interface_ready:
            translations={old:new for old,new in DELAYED_CALLBACK_API_NAMES.items() if re.search(r'\b'+old+r'\b',code_only(strip_comments(raw)))}
            if translations:
                candidates[name]['delayed_callback_interface_2022']=dict(translations=translations)
                adaptations.append('Native delayed callbacks using recovered life and attack APIs; original source helpers retained.')
        if shared_card_interface_ready:
            translations={old:new for old,new in SHARED_CARD_API_NAMES.items() if re.search(r'\b'+old+r'\b',code_only(strip_comments(raw)))}
            if translations:
                candidates[name]['shared_card_interface_2022']=dict(translations=translations)
                adaptations.append('Native Vow and Lace helpers; original source interfaces retained.')
        if life_loss_interface_ready:
            calls={old:new for old,new in LIFE_LOSS_API_NAMES.items() if re.search(r'\b'+old+r'\s*\(',code_only(strip_comments(raw)))}
            if calls:
                candidates[name]['life_loss_interface_2022']=dict(calls=calls)
                adaptations.append('Native life-loss engine and recovered gain/Mastery callbacks; original interfaces retained.')
        if cannot_lose_interface_ready:
            calls={old:new for old,new in CANNOT_LOSE_API_NAMES.items() if re.search(r'\b'+old+r'\s*\(',code_only(strip_comments(raw)))}
            if calls:
                candidates[name]['cannot_lose_interface_2022']=dict(calls=calls)
                adaptations.append('Native specialized loss-protection worker and public wrappers; original interfaces retained.')
        if life_interface_ready:
            translations={old:new for old,new in {**LIFE_API_NAMES,**LIFE_ENUM_NAMES}.items() if re.search(r'\b'+old+r'\b',code_only(strip_comments(raw)))}
            if translations:
                candidates[name]['life_interface_2022']=dict(translations=translations)
                adaptations.append('Native life gain/loss-prevention frontends and replacement trigger value; original interfaces and event domain retained.')
        if specific_cip_interface_ready:
            calls={old:new for old,new in SPECIFIC_CIP_API_NAMES.items() if re.search(r'\b'+old+r'\s*\(',code_only(strip_comments(raw)))}
            if calls:
                candidates[name]['specific_cip_interface_2022']=dict(calls=calls)
                adaptations.append('Native specific enters-play frontends use the recovered worker and native test layout; original interfaces retained.')
        if global_tutor_interface_ready:
            translations={old:new for old,new in {**GLOBAL_TUTOR_API_NAMES,**TUTOR_DESTINATION_NAMES,**TUTOR_ORIGIN_NAMES,**TUTOR_DOMAIN_NAMES}.items() if re.search(r'\b'+old+r'\b',code_only(strip_comments(raw)))}
            if translations:
                candidates[name]['global_tutor_interface_2022']=dict(translations=translations)
                adaptations.append('Native global tutoring/control transfer and destination/origin domains; legacy tutor values/interfaces retained.')
        if action_interface_ready:
            calls={old:new for old,new in ACTION_API_NAMES.items() if re.search(r'\b'+old+r'\s*\(',code_only(strip_comments(raw)))}
            if calls:
                candidates[name]['action_interface_2022']=dict(calls=calls)
                adaptations.append('Native action, random permanent and copy consumers with 216-byte tests/84-byte targets; original interfaces retained.')
        if death_trigger_interface_ready:
            calls={old:new for old,new in DEATH_TRIGGER_API_NAMES.items() if re.search(r'\b'+old+r'\s*\(',code_only(strip_comments(raw)))}
            if calls:
                candidates[name]['death_trigger_interface_2022']=dict(calls=calls)
                adaptations.append('Native death-trigger counting/storage and resolution APIs with 216-byte tests; original interfaces retained.')
        if spell_trigger_interface_ready:
            calls={old:new for old,new in SPELL_TRIGGER_API_NAMES.items() if re.search(r'\b'+old+r'\s*\(',code_only(strip_comments(raw)))}
            if calls:
                candidates[name]['spell_trigger_interface_2022']=dict(calls=calls)
                adaptations.append('Native spell-cast trigger API with 216-byte tests; legacy callers retain their original interface.')
        if re.search(r'\b(?:' + '|'.join(MANA_API_NAMES) + r')_2022\s*\(', body):
            adaptations.append("Versioned 2022 mana APIs preserve added-color, override and transmutation handling; original producers remain available.")
        if any(name+'_2022(' in body for name in EXTENSION_FLAG_NAMES):
            adaptations.append("Versioned extension-backed flag API; original 2020 flag storage/accessors retained.")
        if "get_special_abilities_2022(" in body or "check_for_special_ability_2022(" in body:
            adaptations.append("Versioned 2022 keyword queries retain card-ID lookup and extension-backed instance handling.")
        if "is_protected_from_2022(" in body or "IPF2022_" in body:
            adaptations.append("Protection modes use the separately recovered 2022 enum and API; original mode values remain unchanged.")
        if "get_special_abilities_by_instance_2022(" in body or "remove_special_ability_2022(" in body:
            adaptations.append("Recovered 2022 keyword accessors use extension storage; original 2020 APIs remain available.")
        if "add_counters_from_source(" in body:
            adaptations.append("Six-argument add_counters calls retain source/recipient through add_counters_from_source.")
        if "ENGINE_DONE_LABEL" in body or "ENGINE_OK_LABEL" in body:
            adaptations.append("Localized show_deck label pointers use the legacy integer-address argument.")
        if "builtin_strncpy(" in raw:
            adaptations.append("Ghidra fixed-length builtin_strncpy uses the standard C strncpy interface.")
        if re.search(r"anon_union_4_2_[0-9a-f]+_for_card_instance_struct_26|\(int32_t\)\s*\w+->field_26", raw):
            adaptations.append("Four-byte info-slot union payloads copy through the scalar member into the corresponding target word.")
        if re.search(r"\bconst target_t\s*\*", body):
            adaptations.append("Borrowed target-list locals preserve the existing APIs' const qualifier.")
        if not adaptations:
            candidates[name].pop("interface_adaptations", None)

    initial = set(candidates)
    # References to any exported function require its implementation to be
    # present already or accepted in this same batch, including callbacks.
    exported_names = {r["name"] for r in csv.DictReader((exports / "export.tsv").open(), delimiter="\t")}
    dependency_names = exported_names | set(source_prototypes)
    known = definitions - static_names
    known -= {n for n, entries in source_prototypes.items() if all(p["static"] for p in entries)}
    dependencies = {name: (set(re.findall(r"\b[A-Za-z_]\w*\b", code_only(c["body"]))) & dependency_names) - {name} - bound_variables(c["body"])
                    for name, c in candidates.items()}
    scratch = analysis / "source-candidates.c"
    compiler_diagnostics = []
    iterations = 0
    existing_interfaces = {}
    while candidates:
        iterations += 1
        dropped = True
        while dropped:
            dropped = False
            for name in list(candidates):
                available = {other for other,c in candidates.items()
                             if not c["static"] or c["source"] == candidates[name]["source"]}
                missing = dependencies[name] - available - known - file_definitions.get(candidates[name]["source"], set())
                if missing:
                    rejected[name] = "unrecovered dependencies: " + ", ".join(sorted(missing))
                    del candidates[name]
                    dropped = True
        if not candidates:
            break
        parts = ['#include "manalink.h"\n']
        # A verified private global belongs only to its original source unit.
        # Make it visible in the scratch check when all candidate users share
        # that owner, as they will in the complete recovered source file.
        manifest_path = root / 'docs/recovery-evidence/recovered-functions.json'
        globals_evidence = json.loads(manifest_path.read_text()).get('source_globals', [])
        for declaration in globals_evidence:
            if declaration.get('linkage') != 'static':
                continue
            users = [c for c in candidates.values() if re.search(
                r'\b' + re.escape(declaration['name']) + r'\b', code_only(c['body']))]
            if (users and all(c['source'] == declaration['source'] for c in users) and
                source_text[declaration['source']].count(declaration['declaration']) == 1):
                parts.append(declaration['declaration'] + '\n')
        watermark_users = [c for c in candidates.values() if re.search(r"\bwatermarks\b", code_only(c["body"]))]
        if watermark_users and all(c["source"] == "cards/unstable.c" for c in watermark_users):
            # This private cache was recovered with COFF/.bss and DWARF layout
            # evidence. Check consumers in the declaration context they have
            # in their complete source unit, without changing its linkage.
            owner = (root / "cards/unstable.c").read_text()
            declaration = "static const Watermark* watermarks = NULL;"
            if declaration in owner:
                parts.append(declaration + "\n")
        existing_interfaces = {}
        private_interfaces = {}
        for c in candidates.values():
            for name in set(re.findall(r"\b[A-Za-z_]\w*\b", code_only(c["body"]))) - set(candidates) - bound_variables(c["body"]):
                matches = [p for p in source_prototypes.get(name, [])
                           if not p["static"] or p["source"] == c["source"]]
                # Historical source units can contain duplicate public
                # definitions. Their identical declarations still establish
                # one interface; keep conflicting signatures ambiguous.
                if matches and all(not p['static'] for p in matches):
                    unique = {re.sub(r'\s+', '', p['prototype']):p for p in matches}
                    if len(unique) == 1:
                        matches = [next(iter(unique.values()))]
                if len(matches) == 1:
                    p = matches[0]
                    (private_interfaces if p["static"] else existing_interfaces)[name] = p["prototype"]
        for name, prototype in {**existing_interfaces, **private_interfaces}.items():
            parts.extend([f'#line 1 "existing/{name}"\n', prototype + "\n"])
        for name, c in candidates.items():
            parts.extend([f'#line 1 "prototype/{name}"\n', c["prototype"] + "\n"])
        for name, c in candidates.items():
            parts.extend([f'#line 1 "body/{name}"\n', c["body"] + "\n"])
        scratch.write_text("".join(parts), encoding="utf-8")
        command = [str(clang), "-target", "i686-pc-windows-msvc", "-std=gnu99", "-fsyntax-only",
                   "-ferror-limit=0", "-fno-color-diagnostics", "-Werror=implicit-function-declaration",
                   "-Werror=implicit-int", "-Werror=incompatible-pointer-types", "-Werror=return-type",
                   "-Werror=uninitialized", "-Werror=sometimes-uninitialized", "-Wno-ignored-attributes",
                   "-Werror=format",
                   "-I", str(root), str(scratch)]
        result = subprocess.run(command, capture_output=True, text=True)
        compiler_diagnostics.append(result.stderr)
        if result.returncode == 0:
            break
        invalid = set()
        for match in re.finditer(r"^(?:prototype|body)/([^:]+):\d+:\d+: (?:fatal )?error: (.+)$", result.stderr, re.M):
            invalid.add(match[1])
            rejected[match[1]] = "interface/syntax: " + match[2]
        if not invalid:
            (analysis / "promotion-compiler.log").write_text(result.stderr)
            raise RuntimeError("Compiler errors outside candidate functions; see promotion-compiler.log")
        for name in invalid:
            candidates.pop(name, None)
    (analysis / "promotion-compiler.log").write_text("\n".join(compiler_diagnostics))
    if not candidates:
        existing_interfaces = {}
    report = dict(exported_functions=len(exported_names), initially_reconstructable=len(initial),
                  accepted=len(candidates), compiler_passes=iterations,
                  maximum_function_characters=args.max_characters,
                  source_files=dict(collections.Counter(c["source"] for c in candidates.values())),
                  accepted_functions=[{k:v for k,v in c.items() if k not in ("body",)} for c in candidates.values()],
                  deferred=rejected)
    header_text = (root / "manalink.h").read_text()
    report["existing_source_interfaces"] = {n:p for n,p in existing_interfaces.items()
                                             if not re.search(r"\b" + re.escape(n) + r"\s*\(", header_text)}
    for name in candidates:
        report["deferred"].pop(name, None)
    (analysis / "promotion-report.json").write_text(json.dumps(report, indent=2))
    if args.apply:
        by_file = collections.defaultdict(list)
        for candidate in candidates.values():
            by_file[candidate["source"]].append(candidate)
        public = list(report["existing_source_interfaces"].values())
        referenced_from = collections.defaultdict(set)
        for name, c in candidates.items():
            for dependency in dependencies[name]:
                referenced_from[dependency].add(c["source"])
        for relative, entries in by_file.items():
            destination = root / relative
            content = destination.read_bytes() if destination.exists() else b'#include "manalink.h"\n'
            # These exports belong to the Manalink DLL configuration, including
            # scalar frontends whose callee has a native aggregate interface.
            addition = "\n\n#ifndef SHANDALAR\n/* Recovered October 2022 implementations; see docs/recovery-progress.md. */\n"
            needed = set().union(*(dependencies[c["name"]] for c in candidates.values()))
            addition += "\n".join(c["prototype"] for c in entries if c["name"] in needed) + "\n\n"
            for c in sorted(entries, key=lambda c: int(c["line"] or 0)):
                addition += f"/* ManalinkEh 2022: 0x{c['address']}; original {relative}:{c['line']}. */\n"
                addition += c["body"] + "\n"
                if not c["static"] and (not c["name"].startswith("card_") or
                                        referenced_from[c["name"]] - {relative}):
                    public.append(c["prototype"])
            addition+='#endif\n'
            destination.write_bytes((content.rstrip() + addition.encode()).replace(b"\r\n", b"\n").replace(b"\n", b"\r\n").rstrip(b"\r\n") + b"\r\n")
        if public:
            header = root / "manalink.h"
            content = header.read_bytes()
            marker = content.rfind(b"#ifdef __cplusplus")
            extension_public = [p for p in sorted(public) if re.search(r'\b(?:sp_keyword2_t|\w+_2022_t)\b', p)]
            addition = "/* Interfaces recovered from October 2022 DWARF. */\n" + "\n".join(p for p in sorted(public) if p not in extension_public) + "\n"
            if extension_public:
                addition += '#ifndef SHANDALAR\n' + '\n'.join(extension_public) + '\n#endif\n'
            addition += '\n'
            header.write_bytes((content[:marker] + addition.encode() + content[marker:]).replace(b"\r\n", b"\n").replace(b"\n", b"\r\n"))
        report["applied"] = True
        manifest = root / "docs/recovery-evidence/recovered-functions.json"
        previous = json.loads(manifest.read_text()) if manifest.exists() else {}
        merged = {(f["source"], f["address"]):f for f in previous.get("accepted_functions", [])}
        merged.update({(f["source"], f["address"]):f for f in report["accepted_functions"]})
        saved = dict(previous)
        saved.update(report)
        saved["accepted_functions"] = list(merged.values())
        saved["accepted"] = len(merged)
        saved["latest_batch_accepted"] = len(candidates)
        saved["source_files"] = dict(collections.Counter(f["source"] for f in merged.values()))
        for fragment in saved.get("source_fragments", []):
            saved["source_files"].setdefault(fragment["source"], 0)
        saved["card_handler_functions"] = sum(f["name"].startswith("card_") for f in merged.values())
        saved["helper_functions"] = len(merged) - saved["card_handler_functions"]
        saved["private_functions"] = sum(f["static"] for f in merged.values())
        saved["private_helpers"] = sum(f["static"] and not f["name"].startswith("card_") for f in merged.values())
        saved["shared_helper_interfaces"] = sum(not f["static"] and not f["name"].startswith("card_")
                                                 for f in merged.values())
        saved["existing_source_interfaces"] = {**previous.get("existing_source_interfaces", {}),
                                                **report["existing_source_interfaces"]}
        for f in merged.values():
            saved["deferred"].pop(f["name"], None)
        saved["deferred_reason_counts"] = dict(collections.Counter(reason.split(":")[0]
                                                                   for reason in saved["deferred"].values()))
        registrations = list(csv.DictReader((analysis / "2022-registrations.csv").open()))
        recovered_names = {f["name"] for f in merged.values()}
        saved["registered_handler_symbols"] = len({r["symbol"].lstrip("_") for r in registrations
                                                   if r["symbol"].lstrip("_") in recovered_names})
        saved["registered_handler_symbols_marked_coded"] = len({r["symbol"].lstrip("_") for r in registrations
                                                                if r["coded"] == "True" and
                                                                r["symbol"].lstrip("_") in recovered_names})
        saved["source_syntax_check"] = dict(status="Pending complete source-unit check after promotion.", tests_run=False)
        saved["tests"] = "Deferred by user request; no tests added or run."
        saved["native_build"] = "Not run; downstream integration and behavioral validation remain."
        manifest.write_text(json.dumps(saved, indent=2))
        if storage_evidence:
            for c in candidates.values():
                if c.get('stack_storage_reconstruction'):
                    storage_evidence.setdefault('functions', {})[c['address']] = dict(
                        name=c['name'], source=c['source'], **c['stack_storage_reconstruction'])
            storage_path.write_text(json.dumps(storage_evidence, indent=2))
    print(json.dumps({k:v for k,v in report.items() if k not in ("accepted_functions", "deferred")}, indent=2))


if __name__ == "__main__":
    main()
