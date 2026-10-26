"""Share compatible public helper bodies while preserving historical evidence.

Compatibility names use GCC/COFF aliases or explicitly typed const-return
tail adapters. Recovered names remain provenance anchors during migration.
"""
import argparse,hashlib,json,re,shutil
from functools import lru_cache
from pathlib import Path
from audit_source import function_body
from promote_recovery import LITERAL

ROOT=Path(__file__).resolve().parents[2]
EVIDENCE=Path('docs/recovery-evidence/helper-unification.json')
FAMILY={'is_what':('2681ca0','is_what_2022'),
        'is_token':('2680e20','is_token_2022')}
CORE_NAMES='''pump_ability_until_eot resolve_gfp_ability gain_life has_mana_multi_a has_mana_multi
get_global_color_hack get_color get_color_by_internal_id_no_hack get_color_by_internal_id
get_color_real_or_iid change_color get_hacked_walk keyword_to_color get_colors_to_hack_from_csvid
is_colorless colors_shared has_my_colors real_count_colors count_colors evaluate_colors
is_monocolor_hybrid get_deck_color choose_a_color choose_a_color_from select_a_color
is_planeswalker get_type_with_iid get_type get_base_power get_base_power_iid get_base_toughness
get_base_toughness_iid get_cmc_by_internal_id get_cmc get_highest_power get_power_sum
count_defenders count_creatures_by_toughness count_creatures_by_power get_highest_cmc_nonland
get_highest_cmc get_most_common_cmc_in_hand get_most_common_cmc_nonland count_permanents_by_type
count_permanents_by_color count_cards_by_id check_battlefield_for_subtype count_subtype count_shrines
is_x_spell is_hardcoded_card is_valid_card is_stolen is_target is_sick is_animated_and_sick
is_attacking in_play_and_attacking check_state check_status add_state remove_state add_status
remove_status state_untargettable tap_card untap_card untap_card_no_event is_creature_dead
has_dead_creature has_activation_on_stack is_basic_land_by_id is_basic_land is_mana_producer_land
has_no_mana_cost get_card_data no_combat_damage_this_turn negate_combat_damage_this_turn
my_damage_cannot_be_prevented prevent_damage prevent_the_next_n_damage fog_special2 fog_special
fog_effect negate_damage_this_turn prevent_all_damage_to_target prevent_all_damage
damage_target_creature_or_player damage_cannot_be_prevented_until_eot prevent_all_my_damage
prevent_all_damage_dealt_by_target prevent_damage_until_eot'''.split()
RECORD_NAMES=('target_definition','test_definition','token_generation','pump_ability')
RECORD_FAMILY='''base_target_definition default_target_definition opponent_or_pw_target_definition
counterspell_target_definition counter_activated_target_definition counter_triggered_target_definition
can_target target_available would_validate_arbitrary_target validate_arbitrary_target would_validate_target
would_valid_target validate_target valid_target pick_next_target_noload_arbitrary pick_next_target_arbitrary
choose_default_target select_target pick_target new_pick_target pick_next_target_noload
pick_up_to_n_targets_noload pick_up_to_n_targets new_get_test_score new_default_test_definition
default_test_definition new_make_test_in_play new_make_test default_token_definition copy_token_definition
generate_token default_pump_ability_definition pump_ability pump_ability_by_test
pump_ability_until_eot_with_precomputed_sleight_hack_suppress legacy_effect_pump_ability_until_eot'''.split()
RECORD_CONSUMERS='''select_multiple_cards_from_graveyard_to_array ninjutsu_custom_target
sp_ability_trigger_impl sp_ability_trigger can_counter_triggered_ability test_leaves_play test_leaves_play_mode
set_token new_select_a_card new_special_count_grave new_can_sacrifice_as_cost_impl new_can_sacrifice_as_cost
max_can_sacrifice_as_cost new_can_sacrifice new_get_special_permanent_for_sacrifice etb_ability_global
new_specific_cip_impl new_specific_cip_global new_specific_cip new_card_from_list
haste_and_sacrifice_token_at_end_of_next_turn haste_and_remove_token_at_end_of_next_turn
reveal_cards_from_your_hand mana_into_string ai_name_a_random_csvid name_a_card
select_permanent_to_disable counterspell_validate counterspell counterspell_resolve_unless_pay_x'''.split()
RUNTIME_BINDINGS='''charge_mana charge_mana_multi charge_mana_multi_a has_mana_for_activated_ability
charge_mana_for_activated_ability charge_mana_for_activated_ability_while_resolving
get_cost_mod_for_activated_abilities can_use_activated_abilities can_use_activated_abilities_by_instance
can_produce_mana charge_mana_while_resolving charge_mana_while_resolving_csvid
charge_mana_multi_while_resolving charge_mana_multi_while_resolving_csvid charge_mana_for_multi_x
charge_mana_for_double_x get_sleighted_color_test show_deck
generate_tokens_by_id generate_reserved_token_by_id copy_token generic_token token_characteristic_setting_effects
etb_ability target_opponent opponent_is_valid_target impose_sacrifice get_sleighted_hacked_suppressed_ability
pump_ability_until_eot_no_repeat disable_all_activated_abilities disable_nonmana_activated_abilities
produce_mana_tapped_all_one_color_with_default produce_mana_tapped_any_combination_of_colors
produce_mana_tapped_all_one_color produce_mana_tapped produce_mana_tapped2 produce_mana_tapped_multi
choose_mana_all_one_color produce_mana_any_combination_of_colors produce_mana_all_one_color
declare_mana_available_any_combination_of_colors produce_mana_of_any_type_tapped_for
produce_mana_when_subtype_is_tapped get_colors_of_mana_land_could_produce_ignoring_costs two_mana_land
mana_producing_creature mana_producing_creature_multi mana_producer mana_producer_tapped
is_a_tappable_mana_source force_activation_for_mana tap_card_for_mana mana_burn human_autotap_for_mana
autotap_mana_source format_manacost_into_global_allpurpose_buffer'''.split()
ACTIVATION_PROMPTS=['generic_activated_ability','granted_generic_activated_ability','attachment_granting_activated_ability']
EFFECT_BINDINGS='''new_damage_all check_battlefield_for_special_card copy_spell_from_stack pump_until_eot
upkeep_trigger_mode_impl count_for_gfp_ability_and_store_values reveal_card_until_you_reveal_test
cloning_and_verify_legend can_activate_altar_basic ai_mod_for_untap_effects pump_until_eot_merge_previous
pump_subtype_until_eot pump_creatures_until_eot count_for_gfp_ability trigger_gain_life lose_life
cast_counter_activated_ability validate_counter_activated_ability resolve_counter_activated_ability
can_counter_activated_ability can_block_additional_until_eot pump_color_until_eot
pump_creatures_until_eot_no_repeat vanilla_pump vanilla_instant_pump cannot_attack humiliate_and_set_pt_abilities
pump_creatures_until_eot_merge_pt pump_creatures_until_eot_merge_pt_alternate_legacy_text
remove_summoning_sickness twiddle ai_mod_for_tap_effects ai_modifier_for_targeted_card_drawing
ai_modifier_for_act_of_treason remove_special_ability check_for_special_ability get_special_abilities_by_instance
get_special_abilities special_abilities effect_pump_until_eot permanents_you_control_can_tap_for_mana'''.split()
REANIMATION_BINDINGS='''seek_grave_for_id_to_reanimate reanimate_permanent_impl new_reanimate_all
reanimate_permanent reanimate_permanent_with_effect reanimate_permanent_with_counter_and_effect
reanimate_permanent_with_function reanimate_all'''.split()
SPELL_AURA_BINDINGS='''get_sleighted_color_text2 get_sleighted_color_text get_card_name_by_id attach_aura_to_target
aura_attached_to_me vanilla_aura disabling_aura aura_with_variable_pt_boost_depending_on_condition
gain_control_and_attach_as_aura end_of_stealing_aura_effect
when_enchanted_permanent_dies_return_aura_to_hand_legacy when_enchanted_permanent_dies_return_aura_to_hand
cannot_be_enchanted_granted cannot_be_enchanted generic_aura get_tutoring_denial
select_multiple_cards_from_graveyard new_global_tutor global_tutor reveal_top_cards_of_library
generic_spell specific_cip copy_spell haste_and_kill_at_eot haste_and_remove_eot haste_and_sacrifice_eot
generic_haste_and_kill_at_eot_legacy haste_and_remove_at_beginning_of_next_end_step
haste_and_exile_your_upkeep effect_unearth ereboss_titan_effect aot_like_spell'''.split()
TYPE_SELECTION_BINDINGS='''global_type_change type_uncounterable type_change_legacy turn_into_artifact
turn_into_creature artifact_animation global_type_animation_legacy real_land_animation2 land_animation2
card_generic_animated_land manland_shared manland_animated manland_normal cloning
enters_the_battlefield_as_copy_of_any_creature shapeshift_target real_shapeshift_target shapeshift_target_effect
shapeshift_all untap_only_1_permanent_per_upkeep untap_permanents_during_opponents_untap choose_to_untap
activated_abilities_disabler_enters_bf activated_abilities_disabler has_activated_ability
tutor_card_with_the_same_name is_legal_block recalculate_all_cards_in_play lose_the_game
cannot_lose_the_game cannot_lose_the_game_for_having_less_than_0_life new_select_target_from_grave
select_target_from_grave_source select_target_from_either_grave store_attackers resolve_declare_attackers_trigger
declare_attackers_trigger declare_attackers_trigger_test get_test_score get_hacked_land_text
mark_up_to_n_targets_noload phase_in_impl phase_out_impl phase_in phase_out increment_storm_count
get_specific_storm_count get_storm_count get_stormcreature_count proliferate_auto_single_permanent_or_player
proliferate_manual_single_permanent_or_player proliferate'''.split()
ZONE_INPUTS=['get_average_value_from_zone','select_card_from_zone']
CANONICAL_FAMILIES={'records','record_consumers','runtime_bindings','activation_prompts','effect_bindings','reanimation_bindings','spell_aura_bindings','type_selection_bindings','zone_inputs'}
CALLBACK_TYPES={'CardFunction':('int',('int','int','event_t')),
                'ReanimationBeforeEtb_2022':('void',('int','int','int','int'))}
TYPE_NAMES=('int','void','char','int32_t','uint32_t','counter_t','reanimate_mode_t','reanimate_mode_2022_t',*CALLBACK_TYPES,'color_t','target_t','card_instance_t','card_data_t','card_ptr_t','event_t','resolve_trigger_t',
            'type_t','subtype_t','ability_loss_t','color_test_t','change_color_t','prevent_t','fog_special_flags_t','select_grave_t','declare_attackers_trigger_t')
TYPE_NAMES+=tuple(n+s for n in RECORD_NAMES for s in ('_t','_2022_t'))
TYPE_PATTERN=r'(?:const\s+)?(?:'+ '|'.join(TYPE_NAMES)+r')\b\s*(?:\*\s*)?'

def parameters(text):
    result=[];depth=0;start=0
    for at,c in enumerate(text):
        depth+=(c=='(')-(c==')');assert depth>=0
        if c==',' and depth==0:result.append(text[start:at]);start=at+1
    assert depth==0
    result.append(text[start:]);return result

def sha(data):return hashlib.sha256(data).hexdigest()
def read(path):return Path(path).read_bytes().decode('latin1').replace('\r\n','\n')
@lru_cache(maxsize=16)
def mask(text):
    return re.sub(LITERAL+r'|/\*.*?\*/|//[^\r\n]*',lambda m:''.join('\n' if c=='\n' else ' ' for c in m[0]),text,flags=re.S)

def scalar_signature(prototype):
    """Compare exact reviewed C types; never equate different enum/record types."""
    from promote_recovery import strip_comments
    prototype=strip_comments(prototype)
    # These are actual C typedef aliases after canonicalize_records.py. Their
    # single definitions/layouts are independently checked by verify_source.
    if (ROOT/'docs/recovery-evidence/canonical-records.json').exists():
        for name in RECORD_NAMES:prototype=re.sub(r'\b'+name+r'_2022_t\b',name+'_t',prototype)
    if (ROOT/'docs/recovery-evidence/canonical-reanimation.json').exists():
        prototype=re.sub(r'\breanimate_mode_2022_t\b','reanimate_mode_t',prototype)
    prototype=re.sub(r'^\s*static\s+','',prototype)
    def reviewed_type(text):
        compact=re.sub(r'\s+','',text)
        if compact in CALLBACK_TYPES:return ('callback',CALLBACK_TYPES[compact])
        records={'card_instance_t','card_data_t','card_ptr_t','target_t'}|{n+'_t' for n in RECORD_NAMES}
        assert compact.removeprefix('const') not in records,('Aggregate by value requires separate ABI review',prototype)
        return compact
    m=re.fullmatch(r'\s*('+TYPE_PATTERN+r')([A-Za-z_]\w*)\s*\((.*)\)\s*;',prototype,re.S)
    assert m,('Unsupported prototype; review separately',prototype)
    args=[]
    for arg in parameters(m[3]):
        if arg.strip()=='...':
            assert m[2] in {'get_hacked_land_text','get_hacked_land_text_2022'}
            assert args==['int','int','constchar*'] and parameters(m[3])[-1].strip()=='...'
            args.append('...');continue
        if re.fullmatch(r'\s*char\s*\(\s*\*\s*(?:\w+)?\s*\)\s*\[\s*151\s*\]\s*',arg):
            assert m[2] in {'mark_up_to_n_targets_noload','mark_up_to_n_targets_noload_2022'}
            args.append('char(*)[151]');continue
        callback=re.fullmatch(r'\s*(int|void)\s*\(\s*\*\s*(?:[A-Za-z_]\w*)?\s*\)\s*\((.*)\)\s*',arg,re.S)
        if callback:
            signature=scalar_signature(callback[1]+' callback('+callback[2]+');')
            assert signature in CALLBACK_TYPES.values(),('Unreviewed callback ABI',arg)
            args.append(('callback',signature));continue
        p=re.fullmatch(r'\s*('+TYPE_PATTERN+r')(?:[A-Za-z_]\w*)?\s*',arg)
        assert p,('Unsupported argument; review separately',arg)
        args.append(reviewed_type(p[1]))
    return reviewed_type(m[1]),tuple([] if args==['void'] else args)

def span(text,name):
    code=mask(text)
    m=re.search(r'(?m)^[ \t]*(?:[A-Za-z_]\w*[ \t\r\n*]+)+'+re.escape(name)+r'\s*\([^;{}]*\)\s*\{',code)
    if m is None:return None
    depth=1
    for at in range(m.end(),len(code)):
        depth+=(code[at]=='{')-(code[at]=='}')
        if depth==0:return m.start(),at+1
    raise ValueError('Unclosed function '+name)

def qualified_activation_definition(entry):
    """Only add a read-only pointee annotation to the reviewed prompt argument."""
    assert entry['family']=='activation_prompts' and entry['retained_name'] in ACTIVATION_PROMPTS
    q=entry['source_qualification'];native=q['native_definition']
    assert sha(native.encode())==q['native_definition_sha256']
    head,body=native.split('{',1)
    assert head.count('char *prompt')==1 and 'const char *prompt' not in head
    assert re.sub(r'\s+',' ',head.strip()+';')==re.sub(r'\s+',' ',entry['recovered_prototype'])
    qualified=head.replace('char *prompt','const char *prompt')+'{'+body
    assert entry['source_recovered_prototype']==entry['recovered_prototype'].replace('char *prompt','const char *prompt')
    assert sha(qualified.encode())==q['qualified_definition_sha256']
    assert q['parameter']=='prompt' and q['native_pointee']=='char' and q['source_pointee']=='const char'
    return native,qualified

def qualified_source_definition(entry):
    if entry['family']=='zone_inputs':
        from qualify_zone_inputs import definition
        return definition(entry)
    return qualified_activation_definition(entry)

def audit_entry(entry,root=ROOT,context=None):
    if context is None:context={}
    sources=context.setdefault('sources',{})
    if entry['source'] not in sources:sources[entry['source']]=read(root/entry['source'])
    source=sources[entry['source']];old=entry['retained_name'];new=entry['recovered_name']
    method=entry.get('method','alias');assert method in {'alias','const_return_adapter'}
    if method=='alias':
        assert span(source,old) is None,('A duplicate body remains',old)
        assert source.count(entry['alias_declaration'])==1
    else:
        adapter_span=span(source,old);assert adapter_span is not None
        assert function_body(source[adapter_span[0]:adapter_span[1]],old)==entry['adapter_body']
        assert sha(entry['adapter_body'].encode())==entry['adapter_body_sha256']
    if 'source_replacement' in entry:assert source.count(entry['source_replacement'])==1
    if entry.get('source_activation'):
        a=entry['source_activation'];assert old=='phase_in' and entry['family']=='type_selection_bindings'
        expected='#if 0\n'+entry['predecessor_definition']+'\n#endif'
        assert a['disabled_predecessor_region']==expected and sha(expected.encode())==a['disabled_predecessor_sha256']
        assert '#if 0\n'+entry['source_replacement']+'\n#endif' not in source
    assert sha(entry['predecessor_body'].encode())==entry['predecessor_body_sha256']
    assert function_body(entry['predecessor_body'],old)==entry['predecessor_body']
    bounds=span(source,new);assert bounds is not None,new
    definition=source[bounds[0]:bounds[1]]
    if entry.get('source_qualification'):
        native,qualified=qualified_source_definition(entry)
        assert definition==qualified
        definition=native
    current=function_body(definition,new)
    assert sha(current.encode())==entry['recovered_body_sha256']
    if 'accepted' not in context:
        ledger=json.loads((root/'docs/recovery-evidence/recovered-functions.json').read_text())
        context['accepted']={f['address']:f for f in ledger['accepted_functions']}
    f=context['accepted'][entry['native_address']]
    assert f['source']==entry['source'] and f['name']==new
    assert f['prototype']==entry['recovered_prototype']
    assert f['reconstruction_body_sha256']==entry['recovered_body_sha256']
    assert f==entry['predecessor_recovery_record'],('Recovery record changed; migrate this evidence explicitly',new)
    if 'header' not in context:context['header']=read(root/'manalink.h')
    header=context['header']
    if entry.get('prototype_scope','header')=='header':assert entry.get('canonical_retained_prototype',entry['retained_prototype']) in header
    else:
        assert entry['prototype_scope']=='source'
        assert function_body(entry['predecessor_definition'],old)==entry['predecessor_body']
        assert sha(entry['predecessor_definition'].encode())==entry['predecessor_definition_sha256']
    assert re.sub(r'\s+',' ',entry.get('source_recovered_prototype',entry['recovered_prototype'])) in re.sub(r'\s+',' ',source if f['static'] or entry.get('source_forward_declaration') else header),new
    if entry.get('family') in CANONICAL_FAMILIES:
        assert entry['canonical_record_evidence_sha256']==sha((root/'docs/recovery-evidence/canonical-records.json').read_bytes())
    if entry.get('family')=='reanimation_bindings':
        assert entry['canonical_reanimation_evidence_sha256']==sha((root/'docs/recovery-evidence/canonical-reanimation.json').read_bytes())
    if entry.get('family')=='spell_aura_bindings':
        assert entry['canonical_spell_tutor_evidence_sha256']==sha((root/'docs/recovery-evidence/canonical-spell-tutor.json').read_bytes())
    if entry.get('canonical_storm_evidence_sha256'):
        assert entry['canonical_storm_evidence_sha256']==sha((root/'docs/recovery-evidence/canonical-storm.json').read_bytes())
    # __typeof__ enforces the exact recovered function type. The retained
    # declaration independently checks source compatibility at compile time.
    old_signature=scalar_signature(entry.get('canonical_retained_prototype',entry['retained_prototype']));new_signature=scalar_signature(entry.get('source_recovered_prototype',entry['recovered_prototype']))
    if 'canonical_retained_prototype' in entry:
        assert old in ACTIVATION_PROMPTS and entry['family']=='activation_prompts'
        assert entry['canonical_retained_prototype']==entry['retained_prototype'].replace('const char *prompt','char *prompt')
        previous=scalar_signature(entry['retained_prototype'])
        assert previous[0]==new_signature[0] and previous[1][:-1]==new_signature[1][:-1]
        assert previous[1][-1]=='constchar*' and new_signature[1][-1]=='char*'
    if method=='alias':
        prefix='static' if entry.get('retained_storage',2)==3 else 'extern'
        attributes='used, ' if prefix=='static' else ''
        assert entry['alias_declaration']==f'{prefix} __typeof__({new}) {old} __attribute__(({attributes}alias("{new}")));'
        assert old_signature==new_signature
    else:
        assert old_signature[0]=='constchar*' and new_signature[0]=='char*' and old_signature[1]==new_signature[1]
        assert entry['adapter_body']==old+'('+entry['adapter_parameters']+')\n{\n  return '+new+'('+', '.join(entry['argument_names'])+');\n}'
        assert entry['canonical_control_declaration']==entry['recovered_prototype'][:-1]+' __attribute__((noinline));'
    return entry['predecessor_body']

def retained_alias_body(source,name,root=ROOT):
    """Replay a removed predecessor only for an explicitly verified alias.

    Used solely by retained-body history checks. Current implementation/body
    checks continue to inspect the live recovered definition normally.
    """
    path=root/EVIDENCE
    if not path.is_file():raise ValueError('No retained definition or recorded alias: '+name)
    entries=[e for e in json.loads(path.read_text())['helpers'] if e['retained_name']==name]
    assert len(entries)==1,('No unique recorded alias',name)
    entry=entries[0]
    assert source==read(root/entry['source']),('Historical source unit mismatch',name)
    return audit_entry(entry,root)

def retained_source_body(root,source,name,source_view=None):
    """Replay historical retained hashes; audit live recovered bodies separately.

    Untouched helpers keep their existing parser contract, including old
    unanchored checks. Moved definitions use the independently verified archive.
    """
    text=read(root/source)
    path=root/EVIDENCE
    if path.exists() and any(e['retained_name']==name and e['source']==source for e in json.loads(path.read_text())['helpers']):
        return retained_alias_body(text,name,root)
    if path.exists():
        qualified=[e for e in json.loads(path.read_text())['helpers'] if e['recovered_name']==name and e['source']==source and e.get('source_qualification')]
        if qualified:
            assert len(qualified)==1
            audit_entry(qualified[0],root)
            native,_=qualified_source_definition(qualified[0])
            return function_body(native,name)
    if source=='functions/functions.c' and name=='new_reanimate_all_impl' and (root/'docs/recovery-evidence/retired-reanimation-worker.json').exists():
        from retire_reanimation_worker import verify as verify_retirement,EVIDENCE as retirement_evidence
        assert verify_retirement(root)
        return json.loads((root/retirement_evidence).read_text())['predecessor_body']
    zone_retirement=root/'docs/recovery-evidence/retired-zone-counter.json'
    if source=='functions/count_graveyard.c' and zone_retirement.exists():
        e=json.loads(zone_retirement.read_text())
        if name in {e['retained_name'],*e['callers']}:
            from retire_zone_counter import verify as verify_zone_retirement
            assert verify_zone_retirement(root)
            return function_body(e['predecessor_definition'] if name==e['retained_name'] else e['callers'][name]['predecessor_definition'],name)
    if source_view is not None:text=source_view[source].replace('\r\n','\n')
    return function_body(text,name)

def retained_source_definition(root,source,name):
    """Full checked preimage, including comments used by old fragment proofs."""
    path=root/EVIDENCE
    entries=json.loads(path.read_text())['helpers'] if path.exists() else []
    moved=[e for e in entries if e['source']==source and e['retained_name']==name]
    if moved:
        assert len(moved)==1
        audit_entry(moved[0],root)
        definition=moved[0]['predecessor_definition']
        assert sha(definition.encode())==moved[0]['predecessor_definition_sha256']
        return definition
    text=read(root/source);bounds=span(text,name)
    assert bounds is not None,('Missing retained definition',source,name)
    return text[bounds[0]:bounds[1]]


def verify_source(root=ROOT):
    import canonicalize_helpers
    if canonicalize_helpers.load(root) and not canonicalize_helpers._historical_active:
        return canonicalize_helpers.verify(root)
    from canonicalize_records import verify as verify_records
    canonical_records=verify_records(root)
    from canonicalize_reanimation import verify as verify_reanimation
    canonical_reanimation=verify_reanimation(root)
    from canonicalize_spell_tutor import verify as verify_spell_tutor
    canonical_spell_tutor=verify_spell_tutor(root)
    from retire_reanimation_worker import verify as verify_retirement
    retired_private_helpers=verify_retirement(root)
    from qualify_zone_inputs import audit_sources as verify_zone_inputs
    zone_inputs=verify_zone_inputs({},root)
    from retire_zone_counter import verify as verify_zone_retirement
    retired_zone_worker=verify_zone_retirement(root)
    from canonicalize_storm import verify as verify_storm
    canonical_storm=verify_storm(root)
    path=root/EVIDENCE
    if not path.exists():return {}
    data=json.loads(path.read_text());assert data['format']==1
    assert len({e['retained_name'] for e in data['helpers']})==len(data['helpers'])
    context={}
    for entry in data['helpers']:audit_entry(entry,root,context)
    adapters=sum(e.get('method')=='const_return_adapter' for e in data['helpers'])
    return dict(shared_implementations=len(data['helpers']),alias_pairs=len(data['helpers'])-adapters,
                const_return_adapters=adapters,retained_predecessors='hash-verified',
                read_only_prompt_annotations=sum(bool(e.get('source_qualification')) and e['family']=='activation_prompts' for e in data['helpers']),
                read_only_zone_inputs=zone_inputs,
                retired_zone_worker=retired_zone_worker,
                canonical_storm=canonical_storm,
                canonical_records=canonical_records,canonical_reanimation=canonical_reanimation,canonical_spell_tutor=canonical_spell_tutor,
                retired_private_helpers=retired_private_helpers,
                compatibility_names='Same-address GCC aliases and explicitly typed const-return tail adapters')

def shared_recovered_names(root=ROOT):
    path=root/EVIDENCE
    return {e['recovered_name'] for e in json.loads(path.read_text())['helpers']} if path.exists() else set()

def verify_build(debug,root=ROOT):
    from canonicalize_helpers import load,verify_symbols
    if load(root):
        return verify_symbols(getattr(debug,'physical_image',debug),root)
    result=verify_source(root)
    if not result:return {}
    if result.get('retired_private_helpers'):
        assert not [s for s in debug.symbols if s[0]=='_new_reanimate_all_impl'], 'Retired private predecessor still emitted'
    if result.get('retired_zone_worker'):
        assert not [s for s in debug.symbols if s[0]=='_count_zone_by_type'], 'Retired private zone predecessor still emitted'
    pairs={};adapters={}
    data=json.loads((root/EVIDENCE).read_text());wrapper_ranges={}
    wrapper_sources={e['source'] for e in data['helpers'] if e.get('method')=='const_return_adapter'}
    if wrapper_sources:
        from inspect_release import value,inherited_value
        for cu in debug.dwarf().iter_CUs():
            if value(cu.get_top_DIE(),'DW_AT_name') not in wrapper_sources:continue
            for die in cu.iter_DIEs():
                lo=value(die,'DW_AT_low_pc');hi=die.attributes.get('DW_AT_high_pc')
                if die.tag=='DW_TAG_subprogram' and lo is not None and hi:
                    wrapper_ranges[(inherited_value(die,'DW_AT_name'),lo)]=hi.value if hi.form=='DW_FORM_addr' else lo+hi.value
    for e in json.loads((root/EVIDENCE).read_text())['helpers']:
        symbols=[]
        for name in (e['retained_name'],e['recovered_name']):
            found=[s for s in debug.symbols if s[0]=='_'+name]
            assert len(found)==1,(name,found)
            _,off,sec,kind,storage=found[0]
            expected_storage=e.get('retained_storage',2) if name==e['retained_name'] else (3 if e['predecessor_recovery_record']['static'] else 2)
            assert sec>0 and kind==32 and storage==expected_storage,(name,found)
            symbols.append(debug.pe.OPTIONAL_HEADER.ImageBase+debug.pe.sections[sec-1].VirtualAddress+off)
        if e.get('method','alias')=='alias':
            assert symbols[0]==symbols[1],('Alias has separate machine code',e['retained_name'])
            pairs[e['retained_name']]=dict(recovered_name=e['recovered_name'],shared_address=hex(symbols[0]))
        else:
            import pefile,struct
            pe=pefile.PE(str(root/'ManalinkEh.dll'));code=pe.get_data(symbols[0]-pe.OPTIONAL_HEADER.ImageBase,5)
            assert wrapper_ranges[(e['retained_name'],symbols[0])]-symbols[0]==5
            assert code[0]==0xe9 and symbols[0]+5+struct.unpack('<i',code[1:])[0]==symbols[1]
            adapters[e['retained_name']]=dict(recovered_name=e['recovered_name'],address=hex(symbols[0]),target=hex(symbols[1]),bytes=code.hex())
    return dict(**result,aliases=pairs,adapters=adapters)

def main():
    p=argparse.ArgumentParser(__doc__);p.add_argument('--apply',action='store_true');p.add_argument('--verify',action='store_true');p.add_argument('--family',choices=['foundation','flags','core','duplicates','name_closure','duplicate_closure','records','record_consumers','canonical_duplicates','runtime_bindings','activation_prompts','effect_bindings','reanimation_bindings','spell_aura_bindings','type_selection_bindings','zone_inputs'],default='foundation');args=p.parse_args()
    if args.verify:print(json.dumps(verify_source(),indent=2));return
    from canonicalize_helpers import load as canonical_helpers
    assert not canonical_helpers(ROOT),'Canonical helper cleanup is active; update canonical definitions/callers directly instead of generating compatibility aliases.'
    ledger=json.loads((ROOT/'docs/recovery-evidence/recovered-functions.json').read_text())
    duplicate_proofs={}
    if args.family in {'duplicates','duplicate_closure','canonical_duplicates'}:
        triage=json.loads((ROOT/'docs/recovery-evidence/helper-triage.json').read_text())
        duplicate_proofs={e['native_address'].removeprefix('0x'):e for e in triage['duplicates']}
        assert len(duplicate_proofs)=={'duplicates':43,'duplicate_closure':4,'canonical_duplicates':9}[args.family]
        if args.family=='canonical_duplicates':assert {e['retained_name'] for e in duplicate_proofs.values()}=={'remove_abilities_by_test','pick_player_duh','make_test_in_play','make_test','select_a_card','can_sacrifice_as_cost','can_sacrifice_type_as_cost','can_sacrifice','untap_lands'}
        if args.family=='duplicate_closure':assert {e['retained_name'] for e in duplicate_proofs.values()}=={'declare_mana_available_maybe_hex','ai_can_sleight_card','create_card_name_legacy','create_hardcoded_subtype_name_legacy'}
        family={e['retained_name']:(address,e['recovered_name']) for address,e in duplicate_proofs.items()}
    elif args.family in CANONICAL_FAMILIES:
        from canonicalize_records import verify as verify_records
        assert verify_records(), 'Canonical runtime layouts must precede caller migration'
        selected={'records':RECORD_FAMILY,'record_consumers':RECORD_CONSUMERS,'runtime_bindings':RUNTIME_BINDINGS,'activation_prompts':ACTIVATION_PROMPTS,'effect_bindings':EFFECT_BINDINGS,'reanimation_bindings':REANIMATION_BINDINGS,'spell_aura_bindings':SPELL_AURA_BINDINGS,'type_selection_bindings':TYPE_SELECTION_BINDINGS,'zone_inputs':ZONE_INPUTS}[args.family]
        family={old:(f['address'],f['name']) for old in selected for f in ledger['accepted_functions'] if f['name']==old+'_2022'}
        assert set(family)==set(selected)
    elif args.family=='name_closure':
        names={'create_subtype_name_legacy','create_legacy_name_with_rfg_check'}
        family={old:(f['address'],f['name']) for old in names for f in ledger['accepted_functions'] if f['name']==old+'_2022'}
        assert set(family)==names
    elif args.family=='flags':
        from promote_recovery import EXTENSION_FLAG_NAMES
        family={old:(f['address'],f['name']) for old in EXTENSION_FLAG_NAMES for f in ledger['accepted_functions'] if f['name']==old+'_2022'}
        assert set(family)==set(EXTENSION_FLAG_NAMES)
    elif args.family=='core':
        assert len(CORE_NAMES)==len(set(CORE_NAMES))==92
        family={old:(f['address'],f['name']) for old in CORE_NAMES for f in ledger['accepted_functions'] if f['name']==old+'_2022' and not f['static']}
        assert set(family)==set(CORE_NAMES)
    else:family=FAMILY
    entries=json.loads((ROOT/EVIDENCE).read_text())['helpers'] if (ROOT/EVIDENCE).exists() else []
    assert not set(family)&{e['retained_name'] for e in entries},'Family already migrated.'
    sources={};header=read(ROOT/'manalink.h');added=[]
    for old,(address,new) in family.items():
        f=next(f for f in ledger['accepted_functions'] if f['address']==address)
        assert f['name']==new
        source=f['source'];text=sources.setdefault(source,read(ROOT/source));bounds=span(text,old)
        assert bounds is not None
        before=function_body(text[bounds[0]:bounds[1]],old)
        current_bounds=span(text,new);assert current_bounds is not None,new
        native_definition=text[current_bounds[0]:current_bounds[1]]
        zone_entry=None
        if args.family=='zone_inputs':
            from qualify_zone_inputs import EVIDENCE as zone_evidence
            zone_entry=json.loads((ROOT/zone_evidence).read_text())['functions'][old]
            native_definition=zone_entry['native_definition']
        current=function_body(native_definition,new)
        assert sha(current.encode())==f['reconstruction_body_sha256']
        definition=text[bounds[0]:bounds[1]];old_head=definition[:definition.index('{')].strip()
        prototypes=list(dict.fromkeys(re.findall(r'(?m)^'+TYPE_PATTERN+re.escape(old)+r'\s*\([^;{}]*\);',header)))
        assert len(prototypes)<=1,(old,prototypes)
        prototype=prototypes[0] if prototypes else old_head+';'
        canonical_prototype=prototype
        if args.family=='activation_prompts':
            assert len(prototypes)==1 and prototype.count('const char *prompt')==1
            canonical_prototype=prototype.replace('const char *prompt','char *prompt')
            header=header.replace(prototype,canonical_prototype,1)
        if args.family=='reanimation_bindings':
            from canonicalize_reanimation import verify as verify_reanimation
            assert verify_reanimation()
        source_prototype=f['prototype'] if zone_entry is None else f['prototype'].replace('int *s_location','const int *s_location')
        old_signature=scalar_signature(canonical_prototype);new_signature=scalar_signature(source_prototype)
        retained_storage=3 if re.match(r'^static\b',old_head) else 2
        method='alias' if old_signature==new_signature else 'const_return_adapter'
        if method=='const_return_adapter':
            assert old in {'damage_from_artifact_source','is_land_or_nonblack_creature','colored_damage_source_that_damages_me'}
            assert old_signature[0]=='constchar*' and new_signature[0]=='char*' and old_signature[1]==new_signature[1]
        prefix='static' if retained_storage==3 else 'extern';attributes='used, ' if retained_storage==3 else ''
        alias=f'{prefix} __typeof__({new}) {old} __attribute__(({attributes}alias("{new}")));'
        entry=dict(source=source,native_address=address,retained_name=old,recovered_name=new,
                   retained_prototype=prototype,recovered_prototype=f['prototype'],
                   alias_declaration=alias,predecessor_body=before,predecessor_body_sha256=sha(before.encode()),
                   recovered_body_sha256=sha(current.encode()),predecessor_recovery_record=f,
                   family=args.family,
                   rationale='Same declared return and scalar/enum/common-record pointer cdecl parameter types, with matching argument roles. Use one recovered body for both public names; preserve native behavior and function pointer identity. Qualified spelling remains a provenance anchor until the coordinated name migration.')
        if zone_entry is not None:
            entry['source_recovered_prototype']=source_prototype
            entry['source_qualification']=zone_entry
        if args.family=='activation_prompts':
            entry['canonical_retained_prototype']=canonical_prototype
            entry['rationale']='Match the recovered native prompt declaration in the canonical header. All rebuilt callers and actual alias types are checked; the old const-prompt declaration is preserved solely as history. No runtime adapter or cast is introduced.'
        if duplicate_proofs or args.family in CANONICAL_FAMILIES:
            entry.update(method=method,retained_storage=retained_storage,
                         prototype_scope='header' if prototypes else 'source',
                         predecessor_definition=definition,predecessor_definition_sha256=sha(definition.encode()))
            if duplicate_proofs:
                proof=duplicate_proofs[address];assert sha(before.encode())==proof['retained_body_sha256']
                entry['duplicate_predecessor_proof']=proof
            else:
                entry['canonical_record_evidence_sha256']=sha((ROOT/'docs/recovery-evidence/canonical-records.json').read_bytes())
                if args.family!='activation_prompts':entry['rationale']='The whole source tree uses the canonical 2022 records and token action values. Original and recovered prototypes now name identical C types with matching argument roles. Route original callers to the recovered native body, including initialization of added fields; preserve the old definition only as historical evidence.'
        if args.family=='reanimation_bindings':
            entry['canonical_reanimation_evidence_sha256']=sha((ROOT/'docs/recovery-evidence/canonical-reanimation.json').read_bytes())
        if args.family=='spell_aura_bindings':
            from canonicalize_spell_tutor import verify as verify_spell_tutor
            assert verify_spell_tutor()
            entry['canonical_spell_tutor_evidence_sha256']=sha((ROOT/'docs/recovery-evidence/canonical-spell-tutor.json').read_bytes())
        declarations=[]
        if f['static']:declarations.append(source_prototype)
        elif re.sub(r'\s+',' ',source_prototype) not in re.sub(r'\s+',' ',header):
            declarations.append(source_prototype);entry['source_forward_declaration']=source_prototype
        if method=='alias':replacement='\n'.join(declarations+[alias])
        else:
            old_params=old_head[old_head.index('(')+1:old_head.rindex(')')]
            argument_names=[re.search(r'([A-Za-z_]\w*)\s*$',p)[1] for p in parameters(old_params)]
            body=old+'('+old_params+')\n{\n  return '+new+'('+', '.join(argument_names)+');\n}'
            control=f['prototype'][:-1]+' __attribute__((noinline));'
            keep=old_head+' __attribute__((used));'
            replacement='\n'.join([control,keep,old_head+'\n'+body[len(old+'('+old_params+')')+1:]])
            entry.update(adapter_body=body,adapter_body_sha256=sha(body.encode()),adapter_parameters=old_params,
                         argument_names=argument_names,canonical_control_declaration=control,
                         rationale='Preserve the retained const-char-pointer callback type with a standard C return conversion. A verified five-byte tail adapter reaches the sole non-inlined recovered implementation; no incompatible function alias/cast is used.')
        replacement='/* Both public spellings share the recovered implementation; see helper-unification.json. */\n'+replacement
        if duplicate_proofs or args.family in CANONICAL_FAMILIES:entry['source_replacement']=replacement
        entries.append(entry);added.append(entry)
        sources[source]=text[:bounds[0]]+replacement+text[bounds[1]:]
    result=dict(format=1,helpers=entries,scope='Remove retained duplicate/obsolete bodies using ABI-compatible public aliases. Historical retained-body checks replay only the explicitly saved, hash-verified predecessors; live recovered bodies and actual COFF alias addresses are checked independently.')
    if args.apply:
        backup=ROOT/('.recovery/helper-unification-before' if args.family=='foundation' else '.recovery/helper-unification-'+args.family+'-before');assert not backup.exists();backup.mkdir()
        for name in [*sources,'docs/recovery-evidence/recovered-functions.json','docs/recovery-evidence/interface-recovery.json','config/reccmp/functions.csv','manalink.h']:
            target=backup/name;target.parent.mkdir(parents=True,exist_ok=True);shutil.copy2(ROOT/name,target)
        shutil.copy2(ROOT/'.git/index',backup/'git-index')
        if (ROOT/EVIDENCE).exists():shutil.copy2(ROOT/EVIDENCE,backup/'helper-unification.json')
        if duplicate_proofs:
            shutil.copy2(ROOT/'docs/recovery-evidence/helper-triage.json',backup/'helper-triage.json')
        for source,text in sources.items():(ROOT/source).write_bytes(text.replace('\n','\r\n').encode('latin1'))
        if args.family=='activation_prompts':(ROOT/'manalink.h').write_bytes(header.replace('\n','\r\n').encode('latin1'))
        (ROOT/EVIDENCE).write_bytes((json.dumps(result,indent=2)+'\n').encode())
        verify_source()
    print(json.dumps(dict(applied=args.apply,shared_implementations=len(entries),removed_retained_bodies=[e['retained_name'] for e in added],recovery_records_changed=0),indent=2))

if __name__=='__main__':main()
