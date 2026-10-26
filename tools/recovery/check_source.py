"""Check recovered C source syntax and fixed x86 slots; does not run tests/build a DLL."""
import concurrent.futures
import csv
import json
import re
import subprocess
from pathlib import Path


def main():
    root = Path(__file__).resolve().parents[2]
    from canonicalize_records import verify as verify_records
    from canonicalize_reanimation import verify as verify_reanimation
    from canonicalize_spell_tutor import verify as verify_spell_tutor
    from canonicalize_storm import verify as verify_storm
    from canonicalize_helpers import load,historical_sources,source_names,rename
    def original_record_evidence():
        return (verify_records(root),verify_reanimation(root),
                verify_spell_tutor(root),verify_storm(root))
    canonical_helpers={}
    if load(root):
        # Earlier record proofs describe their original alias declarations.
        # Replay those sources only for the Python proof readers. Compilers
        # below always read the real files with the aliases removed.
        with historical_sources(root) as canonical_helpers:
            canonical_records,canonical_reanimation,canonical_spell_tutor,canonical_storm=original_record_evidence()
    else:
        canonical_records,canonical_reanimation,canonical_spell_tutor,canonical_storm=original_record_evidence()
    analysis = root / ".recovery"
    analysis.mkdir(exist_ok=True)
    manifest = json.loads((root / "docs/recovery-evidence/recovered-functions.json").read_text())
    command = ["C:/Program Files/LLVM/bin/clang.exe", "-target", "i686-pc-windows-msvc",
               "-std=gnu99", "-fsyntax-only", "-ferror-limit=0", "-fno-color-diagnostics",
               "-Werror=implicit-function-declaration", "-Werror=implicit-int",
               "-Werror=incompatible-pointer-types", "-Werror=return-type",
               "-Werror=duplicate-decl-specifier",
               "-Wno-ignored-attributes", "-I", str(root)]

    def check(source):
        compatibility = {'functions/rules_engine.c':'strncasecmp=_strnicmp',
                         'functions/helper_functions.c':'ssize_t=int'}.get(source)
        # MinGW crtdefs.h defines ssize_t as int for the real -m32 build. The
        # Microsoft SDK used by this syntax checker does not define that type.
        result = subprocess.run(command + (["-D"+compatibility] if compatibility else []) +
                                [str(root / source)], capture_output=True, text=True)
        return source, dict(exit_code=result.returncode, diagnostics=result.stderr,
                            checker_only_compatibility_define=compatibility)

    # Compile all present native C units, including retained units without any
    # accepted recovered function. Public canonical names can collide there too.
    native_units={r['source'] for year in ['2020','2022'] for r in csv.DictReader((analysis/f'{year}-units.csv').open())}
    sources=sorted(p.relative_to(root).as_posix() for folder in ['cards','functions'] for p in (root/folder).glob('*.c') if p.relative_to(root).as_posix() in native_units)
    assert set(manifest['source_files'])<=set(sources)
    with concurrent.futures.ThreadPoolExecutor(max_workers=4) as pool:
        results = dict(pool.map(check, sources))
    (analysis / "source-syntax-final.json").write_text(json.dumps(results, indent=2))

    layout = analysis / "source-abi.c"
    layout.write_text('''#include "manalink.h"
#include <stddef.h>
_Static_assert(sizeof(card_instance_t) == 300, "instance size");
_Static_assert(sizeof(select_grave_t)==4 && sizeof(declare_attackers_trigger_t)==4, "grave/attack mode words");
_Static_assert(__builtin_types_compatible_p(__typeof__(&select_card_from_zone),__typeof__(&select_card_from_zone_2022)), "read-only zone selector type");
_Static_assert(__builtin_types_compatible_p(__typeof__(&get_hacked_land_text),const char *(*)(int,int,const char *,...)), "three fixed variadic land-text parameters");
_Static_assert(__builtin_types_compatible_p(__typeof__(&mark_up_to_n_targets_noload),int (*)(target_definition_t *,const char *,int,char (*)[151])), "151-byte marked target rows");
_Static_assert(sizeof(stack_data_t) == 4 && offsetof(stack_data_t, internal_card_id) == 0, "stack card ID word");
_Static_assert(sizeof(text_lines) == 249 * 300 && sizeof(text_lines[0]) == 300, "native text rows");
_Static_assert(offsetof(card_instance_t, state) == 8 && sizeof(((card_instance_t*)0)->state) == 4, "state word");
_Static_assert(offsetof(card_instance_t, damage_on_card) == 16 && offsetof(card_instance_t, counter_power) == 18 && sizeof(((card_instance_t*)0)->damage_on_card) == 2 && sizeof(((card_instance_t*)0)->counter_power) == 2, "damage and counter halfwords");
_Static_assert(offsetof(card_instance_t, mode) == 0x37, "mode byte");
_Static_assert(offsetof(card_instance_t, info_slot) == 0x38, "info slot");
_Static_assert(offsetof(card_instance_t, info_slot_fn) == 0x38, "function slot");
_Static_assert(offsetof(card_instance_t, eot_toughness) == 0x4c, "toughness slot");
_Static_assert(offsetof(card_instance_t, eot_toughness0) == 0x4c, "byte 0");
_Static_assert(offsetof(card_instance_t, eot_toughness1) == 0x4d, "byte 1");
_Static_assert(offsetof(card_instance_t, eot_toughness2) == 0x4e, "byte 2");
_Static_assert(offsetof(card_instance_t, eot_toughness3) == 0x4f, "byte 3");
_Static_assert(offsetof(card_instance_t, targets) == 0x74, "targets slot");
_Static_assert(sizeof(card_counts) == 14, "card-count cache size");
_Static_assert(sizeof(card_counts[0]) == 7, "card-count row stride");
_Static_assert(sizeof(add_to_die_rolls) == 8 && sizeof(roll_extra_dice) == 8, "dice modifier caches");
_Static_assert(sizeof(subtypes_not_in_play_t) == 8, "subtype modifier size");
_Static_assert(offsetof(subtypes_not_in_play_t, added_forced) == 4, "subtype modifier mode");
_Static_assert(sizeof(added_subtypes_not_in_play) == 256, "subtype modifier table size");
_Static_assert(sizeof(added_subtypes_not_in_play[0]) == 128, "subtype modifier row stride");
_Static_assert(sizeof(event_query_t) == 4, "query selector width");
_Static_assert(sizeof(event_result) == 4 && sizeof(trigger_cause_controller) == 4, "engine word widths");
_Static_assert(sizeof(((card_instance_t*)0)->info_slot) == 4, "scalar info-slot width");
_Static_assert(sizeof(target_t) == 8 && offsetof(target_t, player) == 0 && offsetof(target_t, card) == 4, "target words");
''')
    storage_evidence = root / "docs/recovery-evidence/stack-storage.json"
    extension_path = root / 'docs/recovery-evidence/extension-foundation.json'
    if extension_path.exists():
        extension = json.loads(extension_path.read_text())
        if extension.get('source_recovered'):
            assertions = ['_Static_assert(offsetof(card_instance_t, ext) == 44 && offsetof(card_instance_t, unused30) == 48, "extension pointer slot");']
            for typename, record in extension['types'].items():
                assertions.append(f'_Static_assert(sizeof({typename}) == {record["bytes"]}, "extension record size");')
                assertions.extend(f'_Static_assert(offsetof({typename}, {member["name"]}) == {member["offset"]}, "extension field offset");'
                                  for member in record['members'])
                assertions.extend(f'_Static_assert(sizeof((({typename}*)0)->{member["name"]}) == {member["bytes"]}, "extension field width");'
                                  for member in record['members'] if 'bytes' in member)
            assertions.append('_Static_assert(sizeof(card_ext) == 57600 && sizeof(disp_card_ext) == 57600, "extension banks");')
            with layout.open('a') as stream:
                stream.write('\n' + '\n'.join(assertions) + '\n')
    state_values = {}
    keyword_values = {}
    interfaces = json.loads((root / 'docs/recovery-evidence/interface-recovery.json').read_text())
    if (root/'docs/recovery-evidence/helper-unification.json').exists():
        with layout.open('a') as stream:
            for typename in ['event_t','resolve_trigger_t','type_t','subtype_t','ability_loss_t','color_t','color_test_t','change_color_t','prevent_t','fog_special_flags_t']:
                stream.write(f'\n_Static_assert(sizeof({typename})==4, "shared helper enum ABI: {typename}");\n')
            stream.write('_Static_assert(sizeof(void*)==4 && sizeof(card_instance_t)==300 && sizeof(card_data_t)==72 && sizeof(card_ptr_t)==152, "shared helper common pointer records");\n')
            stream.write('_Static_assert(sizeof(int32_t)==4 && sizeof(uint32_t)==4, "native scalar word widths");\n')
            sharing=json.loads((root/'docs/recovery-evidence/helper-unification.json').read_text())
            for entry in sharing['helpers']:
                if entry.get('source_qualification') and entry.get('prototype_scope','header')=='header':
                    stream.write(f'_Static_assert(__builtin_types_compatible_p(__typeof__(&{entry["retained_name"]}), __typeof__(&{entry["recovered_name"]})), "shared read-only input function type");\n')
    test_core=interfaces.get('test_core_2022')
    if test_core:
        with layout.open('a') as stream:
            stream.write('\n_Static_assert(sizeof(test_definition_t)==216 && sizeof(test_definition_2022_t)==216 && sizeof(test_selection_predicate_2022_t)==4, "canonical test layout/callback");\n')
            for member in test_core['layout']['members']:
                name,offset,width=member['name'],member['offset'],member['bytes']
                stream.write(f'_Static_assert(offsetof(test_definition_2022_t,{name})=={offset} && sizeof(((test_definition_2022_t*)0)->{name})=={width}, "2022 test member {name}");\n')
                stream.write(f'_Static_assert(offsetof(test_definition_t,{name})=={offset} && sizeof(((test_definition_t*)0)->{name})=={width}, "canonical test member {name}");\n')
    target_selection = interfaces.get('target_selection_2022')
    if target_selection:
        targets_source = (root / 'functions/targets.c').read_text()
        storage_name = source_names(root).get('target_selection_storage_2022_t', 'target_selection_storage_2022_t')
        storage = re.search(r'typedef union\s*\{[^}]+\}\s*' + re.escape(storage_name) + ';', targets_source)
        expected_storage = rename(target_selection['shared_storage_declaration'], source_names(root))
        assert storage and re.sub(r'\s+', '', storage[0]) == re.sub(r'\s+', '', expected_storage)
        cache_declarations = []
        for name in target_selection['native_globals']:
            declaration = re.search(r'(?m)^int\s+' + re.escape(name) + r'(?:\[\d+\])?\s*(?:=\s*0)?;', targets_source)
            assert declaration
            cache_declarations.append('extern ' + re.sub(r'\s*=\s*0', '', declaration[0]))
        with layout.open('a') as stream:
            stream.write('\n' + storage[0] + '\n' + '\n'.join(cache_declarations) + '\n'
                         '_Static_assert(sizeof(target_selection_storage_2022_t)==480 && sizeof(((target_selection_storage_2022_t*)0)->candidates)==60*sizeof(target_t), "selector candidate storage");\n'
                         '_Static_assert(offsetof(target_selection_storage_2022_t,error_reason)==0 && sizeof(((target_selection_storage_2022_t*)0)->error_reason)==208 && offsetof(target_selection_storage_2022_t,multiblocker_error)==0 && sizeof(((target_selection_storage_2022_t*)0)->multiblocker_error)==200 && offsetof(target_selection_storage_2022_t,candidates)==0, "selector human/AI shared frame");\n'
                         '_Static_assert(sizeof(auto_targets)==2000 && sizeof(auto_targets_target)==2000 && sizeof(read_auto_target_file)==4, "auto-target cache words");\n'
                         '_Static_assert((STATE_TARGETTED|STATE_CANNOT_TARGET)==0x300000 && AI==1, "multi-target native marker and AI values");\n')
    target_initializers = interfaces.get('target_initializers_2022')
    if target_initializers:
        with layout.open('a') as stream:
            stream.write('\n_Static_assert(sizeof(target_definition_t)==84 && sizeof(target_definition_2022_t)==84, "canonical target layout");\n')
            for member in target_initializers['layout']['members']:
                name, offset = member['name'], member['offset']
                stream.write(f'_Static_assert(offsetof(target_definition_2022_t,{name})=={offset} && sizeof(((target_definition_2022_t*)0)->{name})==4, "2022 target member {name}");\n')
                if name != 'flags':
                    stream.write(f'_Static_assert(offsetof(target_definition_t,{name})=={offset} && sizeof(((target_definition_t*)0)->{name})==4, "retained target member {name}");\n')
    if interfaces.get('target_validation_2022'):
        with layout.open('a') as stream:
            stream.write('\n_Static_assert(sizeof(card_data_t)==72 && offsetof(card_data_t,id)==36 && sizeof(((card_data_t*)0)->id)==2 && offsetof(card_data_t,reserved2)==38 && sizeof(((card_data_t*)0)->reserved2)==2, "native target matcher ID word");\n'
                         '_Static_assert(sizeof(player_bits)==8 && sizeof(player_bits[0])==4, "target validator player-bit words");\n'
                         'typedef const char* (*recovery_target_predicate_2022_t)(int,int,int,int,int);\n'
                         '_Static_assert(sizeof(recovery_target_predicate_2022_t)==sizeof(int32_t), "target predicate extra slot");\n')
    if interfaces.get('type_text_empty_flag'):
        with layout.open('a') as stream:
            stream.write('\n_Static_assert(TYPETEXT_0 == 0 && sizeof(type_text_t) == 4, "empty type-text flag");\n')
    if interfaces.get('extension_flags_2022'):
        with layout.open('a') as stream:
            stream.write('\n_Static_assert(sizeof(special_flags_t)==4 && sizeof(special_flags2_t)==4 && sizeof(special_flags3_t)==4, "extension flag words");\n_Static_assert(SF3_CARD_IS_FLIPPED==512, "verified inline flip flag");\n')
    if interfaces.get('mana_modifier_caches_2022'):
        with layout.open('a') as stream:
            stream.write('''
_Static_assert(sizeof(mana_colors_added)==8, "added mana colors");
_Static_assert(sizeof(mana_color_override)==64 && sizeof(mana_color_override[0])==32, "mana color override shape");
_Static_assert(sizeof(mana_quantity_override)==64 && sizeof(mana_quantity_override[0])==32, "mana quantity override shape");
_Static_assert(sizeof(multi_mana_transmutation)==16 && sizeof(multi_mana_transmutation[0])==8, "multi mana replacement shape");
''')
    if interfaces.get('mana_counter_2022'):
        with layout.open('a') as stream:
            stream.write('''
_Static_assert(sizeof(engine_raw_mana_available_hex_2022)==408 && sizeof(engine_raw_mana_available_hex_2022[0])==204, "2022 native hex mana stride");
_Static_assert(sizeof(engine_mana_may_be_spent_as_color_2022)==88 && sizeof(engine_mana_may_be_spent_as_color_2022[0])==44, "native spend-as-color stride");
_Static_assert(sizeof(player_bits[0])==4 && EVENT_MODIFY_MANA_PROD==0xa6 && EVENT_COUNT_MANA==0x7f, "mana count event/word interfaces");
''')
    if interfaces.get('extension_subtype_mutators_2022'):
        with layout.open('a') as stream:
            stream.write('\n_Static_assert(offsetof(card_ext_t,subtype)==8 && sizeof(((card_ext_t*)0)->subtype)==24 && sizeof(((card_ext_t*)0)->subtype[0])==2, "extension subtype words");\n'
                         '_Static_assert(offsetof(card_ext_t,inst_info_data)==offsetof(card_ext_t,subtype)+sizeof(((card_ext_t*)0)->subtype) && offsetof(inst_info_t,id)==0 && sizeof(((inst_info_t*)0)->id)==2, "native subtype lookahead boundary");\n'
                         '_Static_assert(offsetof(card_instance_t,ext)==44 && sizeof(((card_instance_t*)0)->ext)==4 && SUBTYPE_INVALID==0, "subtype instance pointer/sentinel");\n')
    token_core = interfaces.get('token_core_2022')
    if token_core:
        with layout.open('a') as stream:
            stream.write('\n_Static_assert(sizeof(token_generation_t)==76 && sizeof(token_generation_2022_t)==76 && sizeof(token_action_2022_t)==4, "canonical token layout/actions");\n')
            for member in token_core['layout']['members']:
                stream.write(f'_Static_assert(offsetof(token_generation_2022_t,{member["name"]})=={member["offset"]}, "2022 token member {member["name"]}");\n')
                if member['name'] != 'special_flags3':
                    stream.write(f'_Static_assert(offsetof(token_generation_t,{member["name"]})=={member["offset"]}, "retained token prefix {member["name"]}");\n')
            for name,number in token_core['actions']['members'].items():
                stream.write(f'_Static_assert({name.replace("TOKEN_ACTION_","TOKEN2022_ACTION_")}=={number}, "2022 token action {name}");\n')
            stream.write('_Static_assert(TOKEN_ACTION_HASTE==16 && TOKEN_ACTION_CONVERT_INTO_ARTIFACT==64, "canonical token actions");\n'
                         '_Static_assert(offsetof(card_instance_t,token_special_keywords_2022)==292 && sizeof(((card_instance_t*)0)->token_special_keywords_2022)==4 && offsetof(card_instance_t,upkeep_colorless)==292 && offsetof(card_instance_t,upkeep_green)==295, "token keyword mirror overlay");\n'
                         '_Static_assert(sizeof(card_data_t)==72 && offsetof(card_data_t,id)==36 && offsetof(card_data_t,reserved2)==38 && sizeof(engine_dynamic_iid_start_2022)==4, "dynamic token card-data view");\n')
    if interfaces.get('yorion_2022'):
        with layout.open('a') as stream:
            stream.write('\n_Static_assert(sizeof(singleplayer_mode_t)==4 && sizeof(singleplayer_mode)==4, "native singleplayer-mode word");\n'
                         '_Static_assert(SPM_NONE==0 && SPM_MOMIR==1 && SPM_DRAFT==2 && SPM_TESTING==3, "retained singleplayer-mode values");\n'
                         '_Static_assert(sizeof(char[2][151])==302, "Yorion selection matrix");\n')
    if interfaces.get('game_mode_2022'):
        with layout.open('a') as stream:
            stream.write('\n_Static_assert(sizeof(other_game_mode_2022_t)==4 && sizeof(other_game_mode)==4, "independent game-mode word");\n'
                         '_Static_assert(OGM_NONE==0 && OGM_EDH==1 && OGM_PLANECHASE==2 && OGM_RANDOM_SINGLETON==4 && OGM_SPAT==8, "independent game-mode values");\n'
                         '_Static_assert(CARD_ID_ELDER_DRAGON_HIGHLANDER==14675 && GAMETYPE_SOLO_DUEL==1, "pregame mode selectors");\n')
    if interfaces.get('player_flag_caches_2022'):
        with layout.open('a') as stream:
            stream.write('\n_Static_assert(sizeof(card_flags)==8 && sizeof(card_flags[0])==4, "per-player card-flag words");\n'
                         '_Static_assert(sizeof(your_spells_cant_be_countered_by_color)==2 && sizeof(your_spells_cant_be_countered_by_color[0])==1, "per-player counterability color bytes");\n')
    if interfaces.get('test_consumers_2022'):
        with layout.open('a') as stream:
            stream.write('\n_Static_assert(sizeof(hack_show_deck_force_expansion)==4, "native deck expansion word");\n'
                         '_Static_assert(sizeof(hack_allow_sorcery_if_rules_engine_is_only_effect)==4, "rules-engine sorcery gate word");\n')
    if interfaces.get('sacrifice_core_2022'):
        with layout.open('a') as stream:
            stream.write('\n_Static_assert(sizeof(sacrifice_2022_t)==4 && sizeof(sacrifice_t)==4, "sacrifice flag domains");\n'+
                         ('_Static_assert(SAC_SET_TARGETS==256 && SAC_OPTIONAL==128, "native canonical sacrifice flags");\n' if (root/'docs/recovery-evidence/canonical-helper-domains.json').exists() else '_Static_assert(SAC_SET_TARGETS==128, "retained legacy sacrifice flag");\n')+
                         '\n'.join(f'_Static_assert({name.replace("SAC_","SAC2022_")}=={number}, "native sacrifice flag");'
                                   for name,number in interfaces['sacrifice_core_2022']['enum']['members'].items())+'\n')
    if interfaces.get('generic_spell_2022'):
        with layout.open('a') as stream:
            stream.write('\n_Static_assert(sizeof(generic_spell_flags_2022_t)==4 && sizeof(generic_spell_flags_t)==4, "spell flag domains");\n'+
                         ('_Static_assert(GS_ONE_OR_MULTIPLE_TARGETS==16384 && GS_SAC_PERMANENT_AS_COST==32768, "native canonical spell flags");\n' if canonical_spell_tutor else '_Static_assert(GS_ONE_OR_MULTIPLE_TARGETS==8192 && GS_SAC_PERMANENT_AS_COST==16384, "retained spell flag values");\n')+
                         '\n'.join(f'_Static_assert({name.replace("GS_","GS2022_")}=={number}, "native spell flag");' for name,number in interfaces['generic_spell_2022']['flags']['members'].items())+'\n'+
                         '\n'.join(f'_Static_assert({name}=={number}, "native spell extension flag");' for name,number in interfaces['generic_spell_2022']['extension_flags'].items() if name not in {'SF_0','SF2_0','SF2_REDIRECT'})+'\n')
    if interfaces.get('split_card_helpers_2022'):
        with layout.open('a') as stream:
            stream.write('\n'+ '\n'.join(f'_Static_assert({name}=={number}, "native hand-event range");' for name,number in interfaces['split_card_helpers_2022']['events'].items())+'\n')
    if interfaces.get('aura_consumers_2022'):
        with layout.open('a') as stream:
            stream.write('\n'+ '\n'.join(f'_Static_assert({name}=={number}, "native Aura enum value");' for name,number in interfaces['aura_consumers_2022']['enum_values'].items())+'\n')
    if interfaces.get('ability_writers_2022'):
        component=interfaces['ability_writers_2022']
        with layout.open('a') as stream:
            stream.write('\n_Static_assert(sizeof(pump_ability_t)==48 && sizeof(pump_ability_2022_t)==48, "canonical pump layout");\n')
            for member in component['pump_layout']['members']:
                name,offset,width=member['name'],member['offset'],member['bytes']
                stream.write(f'_Static_assert(offsetof(pump_ability_2022_t,{name})=={offset} && sizeof(((pump_ability_2022_t*)0)->{name})=={width}, "native pump member {name}");\n')
                if offset<36:stream.write(f'_Static_assert(offsetof(pump_ability_t,{name})=={offset} && sizeof(((pump_ability_t*)0)->{name})=={width}, "retained pump member {name}");\n')
            stream.write('\n'.join(f'_Static_assert({name}=={number}, "native ability writer enum value");' for name,number in component['enum_values'].items())+'\n')
    if interfaces.get('pump_core_2022'):
        with layout.open('a') as stream:
            stream.write('\n_Static_assert(offsetof(card_instance_t,targets[8].player)==180 && offsetof(card_instance_t,targets[8].card)==184, "native pump subtype/color slots");\n')
            stream.write('\n'.join(f'_Static_assert({name}=={number}, "native pump enum value");' for name,number in interfaces['pump_core_2022']['enum_values'].items())+'\n')
    if interfaces.get('activated_core_2022'):
        from activated_core_bodies import STORAGE
        component=interfaces['activated_core_2022']
        with layout.open('a') as stream:
            stream.write('\n'+STORAGE+'\n_Static_assert(sizeof(activated_storage_2022_t)==216 && offsetof(activated_storage_2022_t,target)==0 && offsetof(activated_storage_2022_t,test)==0 && offsetof(activated_storage_2022_t,selected)==0, "native activation scratch views");\n')
            stream.write('_Static_assert(offsetof(target_definition_2022_t,who_chooses)==offsetof(target_t,player) && offsetof(target_definition_2022_t,allowed_controller)==offsetof(target_t,card), "native activation selected pair prefix");\n')
            stream.write('_Static_assert(sizeof(pay_mana_xbugrwaU)==32 && sizeof(pay_mana_xbugrwaU[0])==4 && sizeof(generic_activated_ability_flags_t)==4, "native activation interfaces");\n')
            stream.write('_Static_assert(offsetof(card_instance_t,targets[17].card)==256, "common activation permission word");\n')
            stream.write('\n'.join(f'_Static_assert({name}=={number}, "native activation flag");' for name,number in component['flags']['members'].items())+'\n')
            stream.write('\n'.join(f'_Static_assert({name}=={number}, "native activation enum value");' for name,number in component['enum_values'].items())+'\n')
    if interfaces.get('ability_filter_2022'):
        with layout.open('a') as stream:
            stream.write('\n_Static_assert(sizeof(test_definition_2022_t)==216 && sizeof(test_definition_t)==216 && sizeof(ability_loss_t)==4 && offsetof(card_instance_t,targets[17].card)==256 && sizeof(((card_instance_t*)0)->targets[17].card)==4, "native ability-loss filter and stored mask");\n')
    if interfaces.get('reveal_destroy_native_2022'):
        with layout.open('a') as stream:
            stream.write('\n_Static_assert(sizeof(card_instance_t)==300 && offsetof(card_instance_t,eot_toughness)==76 && sizeof(((card_instance_t*)0)->eot_toughness)==4 && offsetof(card_instance_t,targets[10].player)==196 && sizeof(target_t)==8, "native packed destruction counters");\n')
            stream.write('_Static_assert(sizeof(test_definition_2022_t)==216 && offsetof(test_definition_2022_t,qty)==174 && sizeof(((test_definition_2022_t*)0)->qty)==2 && sizeof(target_definition_2022_t)==84, "native reveal quantity and separate target/test storage");\n')
    if interfaces.get('native_word_cards_2022'):
        with layout.open('a') as stream:
            stream.write('\n_Static_assert(sizeof(cards_drawn_this_turn)==4 && sizeof(cards_drawn_this_turn[0])==2 && sizeof(player_bits)==8 && sizeof(player_bits[0])==4, "native indexed draw-count and player flag words");\n')
            stream.write('_Static_assert(PB2022_CAST_APPROACH_OF_THE_SECOND_SUN1==4194304 && PB2022_CAST_APPROACH_OF_THE_SECOND_SUN2==8388608 && PB_COMMANDER_IN_PLAY==524288 && SF3_CANNOT_PHASE_IN==8388608, "native and retained legacy cast-history flags");\n')
            stream.write('_Static_assert(sizeof(target_definition_2022_t)==84 && sizeof(test_definition_2022_t)==216 && sizeof(token_generation_2022_t)==76 && sizeof(((test_definition_2022_t*)0)->keyword)==4 && sizeof(((token_generation_2022_t*)0)->key_plus)==4 && sizeof(((target_definition_2022_t*)0)->special)==4 && ((char)0x80)<0, "native scalar-word aggregate fields and signed phasing type byte");\n')
            component=interfaces['native_word_cards_2022']
            values={name:number for enums in component['unit_enums'].values() for name,number in enums.items()}
            for name,number in values.items():
                if name.startswith('PB_CAST_APPROACH'):name=name.replace('PB_','PB2022_',1)
                stream.write(f'_Static_assert({name}=={number}, "native caller flag enumerator");\n')
    if interfaces.get('library_reveal_2022'):
        from library_reveal_body import STORAGE
        with layout.open('a') as stream:
            stream.write('\n'+STORAGE+'\n_Static_assert(sizeof(library_reveal_text_2022_t)==200 && offsetof(library_reveal_text_2022_t,display)==0 && offsetof(library_reveal_text_2022_t,diagnostic)==0 && sizeof(((library_reveal_text_2022_t*)0)->display)==100, "native library reveal overlapping text views");\n')
            stream.write('_Static_assert(sizeof(library_reveal_callback_2022_t)==4 && sizeof(test_definition_2022_t)==216 && offsetof(test_definition_2022_t,qty)==174 && offsetof(test_definition_2022_t,create_minideck)==194 && sizeof(((test_definition_2022_t*)0)->qty)==2 && sizeof(((test_definition_2022_t*)0)->create_minideck)==2, "native library reveal filter and callback");\n')
            stream.write('_Static_assert(sizeof(card_ptr_t)==152 && offsetof(card_ptr_t,name)==8 && offsetof(card_ptr_t,full_name)==4 && sizeof(card_data_t)==72 && offsetof(card_data_t,id)==36 && sizeof(((card_data_t*)0)->id)==2 && STATE_OWNED_BY_OPPONENT==4096 && (STATE_TAPPED | STATE_ATTACKING)==20 && SF_PLAYED_FROM_DECK==65536 && TYPE_EFFECT==128 && TYPE_NONEFFECT==127 && TYPE_ANY==255 && TYPE_NONE==0, "native library reveal name ID and entry state");\n')
    if interfaces.get('wish_helpers_2022'):
        with layout.open('a') as stream:
            stream.write('\n_Static_assert(sizeof(void*)==4 && sizeof(color_test_t)==4 && sizeof(type_t)==4 && sizeof(test_definition_2022_t)==216, "typed Wish dialog stub boundary widths");\n')
    if interfaces.get('token_emblem_cards_2022'):
        with layout.open('a') as stream:
            stream.write('\n_Static_assert(offsetof(card_instance_t,targets[1].player)==124 && offsetof(card_instance_t,targets[1].card)==128 && offsetof(card_instance_t,hack_mode[2])==278 && sizeof(((card_instance_t*)0)->hack_mode[2])==1, "native emblem payload stores");\n')
            stream.write('_Static_assert(sizeof(target_definition_2022_t)==84 && sizeof(test_definition_2022_t)==216 && sizeof(token_generation_2022_t)==76, "native emblem card aggregate locals");\n')
    if interfaces.get('text_change_helpers_2022'):
        with layout.open('a') as stream:
            stream.write('\n_Static_assert(offsetof(card_instance_t,color_id)==64 && offsetof(card_instance_t,hack_mode)==276 && sizeof(((card_instance_t*)0)->color_id)==6 && sizeof(((card_instance_t*)0)->hack_mode)==6 && sizeof(int[6])==24, "native text mapping and chooser storage");\n')
            stream.write('_Static_assert(SF3_0==0 && SF3_HARDCODED_SLEIGHT_WORDS_REPLACED==8192 && SF3_HARDCODED_HACK_WORDS_REPLACED==16384, "native text-cache flags");\n')
    if interfaces.get('phase_ai_cards_2022'):
        with layout.open('a') as stream:
            stream.write('\n_Static_assert(sizeof(current_phase)==4 && sizeof(current_turn)==4 && sizeof(ai_modifier)==4 && PHASE_MAIN1==20 && PHASE_BEFORE_BLOCKING==22, "native signed phase AI comparisons");\n')
            stream.write('_Static_assert(sizeof(target_definition_2022_t)==84 && sizeof(int[151])==604 && sizeof(char*)==4, "native phase and prompt scratch storage");\n')
    if interfaces.get('spell_copy_2022'):
        with layout.open('a') as stream:
            stream.write('\n_Static_assert(sizeof(card_instance_t)==300 && offsetof(card_instance_t,info_slot)==56 && sizeof(((card_instance_t*)0)->info_slot)==4 && offsetof(card_instance_t,eot_toughness)==76 && sizeof(((card_instance_t*)0)->eot_toughness)==4, "native copied scalar words");\n')
            stream.write('_Static_assert(offsetof(card_instance_t,targets)==116 && sizeof(target_t)==8 && sizeof(((card_instance_t*)0)->targets)==152 && offsetof(card_instance_t,number_of_targets)==54 && sizeof(((card_instance_t*)0)->number_of_targets)==1, "native forced-copy target block");\n')
            stream.write('_Static_assert(sizeof(int[151])==604 && sizeof(int[2][151])==1208 && sizeof(int[2])==8, "native spell-copy card snapshots");\n')
            stream.write('\n'.join(f'_Static_assert((uint32_t){name}=={number & 0xffffffff}U, "native spell-copy enum bits");' for name,number in interfaces['spell_copy_2022']['enum_values'].items())+'\n')
    if interfaces.get('temporary_exile_2022'):
        with layout.open('a') as stream:
            stream.write('\n_Static_assert(sizeof(card_data_t)==72 && offsetof(card_data_t,id)==36 && offsetof(card_data_t,reserved2)==38 && sizeof(((card_data_t*)0)->id)==2 && sizeof(((card_data_t*)0)->reserved2)==2, "temporary exile packed ID load");\n')
            stream.write('_Static_assert(offsetof(card_instance_t,display_pic_csv_id)==100 && offsetof(card_instance_t,display_pic_num)==102 && sizeof(((card_instance_t*)0)->display_pic_csv_id)==2 && sizeof(((card_instance_t*)0)->display_pic_num)==2 && sizeof(int[2])==8, "temporary exile picture and meld pair storage");\n')
            stream.write('_Static_assert(offsetof(card_instance_t,targets[0].player)==116 && offsetof(card_instance_t,targets[0].card)==120 && offsetof(card_instance_t,targets[1].player)==124 && offsetof(card_instance_t,targets[1].card)==128, "temporary exile return packet");\n')
            stream.write('_Static_assert(offsetof(card_instance_t,targets[13].player)==220 && offsetof(card_instance_t,targets[13].card)==224 && offsetof(card_instance_t,targets[12].card)==216 && offsetof(card_instance_t,internal_card_id)==108, "retained transform callback stores");\n')
    if interfaces.get('delayed_callbacks_2022'):
        with layout.open('a') as stream:
            stream.write('\n'+'\n'.join(f'_Static_assert({name}=={number}, "native delayed callback enum");'
                for name,number in interfaces['delayed_callbacks_2022']['enum_values'].items())+'\n')
            stream.write('_Static_assert(offsetof(card_instance_t,targets[1].player)==124 && sizeof(((card_instance_t*)0)->targets[1].player)==4 && sizeof(((card_instance_t*)0)->damage_target_player)==1, "native delayed counter and attachment storage");\n')
    if interfaces.get('shared_card_exposures_2022'):
        with layout.open('a') as stream:
            stream.write('\n'+'\n'.join(f'_Static_assert({name}=={number}, "native retained card helper enum");'
                for name,number in interfaces['shared_card_exposures_2022']['enum_values'].items())+'\n')
            stream.write('_Static_assert(SUBTYPE_MIN_CREATURE_SUBTYPE==1 && SUBTYPE_MAX_CREATURE_SUBTYPE==4095, "native creature subtype predicate bounds");\n')
    if interfaces.get('shared_card_frontends_2022'):
        with layout.open('a') as stream:
            stream.write('\n'+'\n'.join(f'_Static_assert({name}=={number}, "native Vow/Lace enum");'
                for name,number in interfaces['shared_card_frontends_2022']['enum_values'].items())+'\n')
            stream.write('_Static_assert(sizeof(target_definition_2022_t)==84 && offsetof(card_instance_t,info_slot)==56 && sizeof(((card_instance_t*)0)->info_slot)==4, "native Lace target locals and boolean slot");\n')
    if interfaces.get('oath_life_loss_2022'):
        with layout.open('a') as stream:
            stream.write('\n_Static_assert(sizeof(target_t)==8 && sizeof(target_definition_2022_t)==84 && TARGET_SPECIAL_NOT_ME==0x4000 && TARGET_SPECIAL_EXTRA_FUNCTION==0x8000 && TYPE_PERMANENT==0x47 && KILL_SACRIFICE==3, "native Oath target/VLA/predicate/sacrifice layout");\n')
    if interfaces.get('life_loss_2022'):
        with layout.open('a') as stream:
            stream.write('\n_Static_assert(sizeof(int[2][151])==1208 && sizeof(int)==4 && sizeof(int[151])==604, "native life-loss saved callback matrix");\n')
            stream.write('\n'.join(f'_Static_assert({name}=={value}, "native life-loss enum");' for name,value in interfaces['life_loss_2022']['enum_values'].items())+'\n')
    if interfaces.get('lichs_mastery_loss_2022'):
        from lich_loss_body import STORAGE
        with layout.open('a') as stream:
            stream.write('\n'+STORAGE+'\n_Static_assert(sizeof(lichs_mastery_storage_2022_t)==216 && offsetof(lichs_mastery_storage_2022_t,text)==0 && offsetof(lichs_mastery_storage_2022_t,test)==0 && sizeof(((lichs_mastery_storage_2022_t*)0)->text)==100, "native Lich shared text/test storage");\n')
            stream.write('_Static_assert(offsetof(test_definition_2022_t,message)==0 && sizeof(((test_definition_2022_t*)0)->message)==100 && sizeof(target_t)==8 && sizeof(target_definition_2022_t)==84 && sizeof(int)==4, "native Lich fixed and VLA element layouts");\n')
            stream.write('_Static_assert(TARGET_SPECIAL_EXTRA_FUNCTION==0x8000 && KILL_EXILE==4 && TUTOR_FROM2022_GRAVE==2 && TUTOR2022_RFG==6, "native Lich selection/exile constants");\n')
    if interfaces.get('cannot_lose_core_2022'):
        with layout.open('a') as stream:
            stream.write('\n_Static_assert(sizeof(PlayerCounters)==4 && offsetof(PlayerCounters,poison_)==0 && sizeof(((PlayerCounters*)0)->poison_)==1, "native cannot-lose poison byte");\n')
            stream.write('_Static_assert(offsetof(card_instance_t,targets[5].player)==156 && offsetof(card_instance_t,targets[5].card)==160 && offsetof(card_instance_t,targets[6].player)==164 && offsetof(card_instance_t,targets[6].card)==168, "native cannot-lose hidden-debt slots");\n')
    if interfaces.get('life_core_2022'):
        with layout.open('a') as stream:
            stream.write('\n_Static_assert(XTRIGGER2022_REPLACE_GAIN_LIFE==0x01000600 && TRIGGER_GAIN_LIFE==224 && sizeof(xtrigger_t)==4, "native life replacement event");\n')
            stream.write('_Static_assert(PB_CANT_GAIN_LIFE==0x200000 && EA_LICH==0x200000 && sizeof(event_flags)==4 && sizeof(player_bits[0])==4, "native life event/player flag words");\n')
            stream.write('_Static_assert(sizeof(life_to_lose_on_gain_life)==4 && sizeof(suspend_cannot_lose_the_game)==4 && sizeof(life_gain_draw_count_2022)==8 && sizeof(life_gain_draw_count_2022[0])==4, "native life state and draw counter words");\n')
    if interfaces.get('specific_cip_2022'):
        component=interfaces['specific_cip_2022']
        with layout.open('a') as stream:
            stream.write('\n_Static_assert(TRIGGER_COMES_INTO_PLAY==219 && EVENT_TRIGGER==125 && EVENT_RESOLVE_TRIGGER==126, "native specific enters-play events");\n')
            stream.write('_Static_assert(sizeof(test_definition_2022_t)==216 && offsetof(test_definition_2022_t,not_me)==170 && sizeof(((test_definition_2022_t*)0)->not_me)==2, "native specific enters-play filter");\n')
            stream.write('_Static_assert(offsetof(card_instance_t,regen_status)==40 && offsetof(card_instance_t,targets[1].player)==124 && offsetof(card_instance_t,targets[1].card)==128, "native specific enters-play context stores");\n')
            for name,offset in component['scalar_flag_offsets'].items():
                stream.write(f'_Static_assert(offsetof(test_definition_2022_t,{name})=={offset} && sizeof(((test_definition_2022_t*)0)->{name})==2, "native scalar enters-play flag");\n')
    if interfaces.get('activation_operand_cards_2022'):
        with layout.open('a') as stream:
            for enums in interfaces['activation_operand_cards_2022']['unit_enums'].values():
                stream.write('\n'.join(f'_Static_assert({name}=={value}, "native activation operand enum");' for name,value in enums.items())+'\n')
            stream.write('_Static_assert(GVC_PLAYER_COUNTERS(PCOUNTER_ENERGY,8)==0x2020800, "native eight-energy cost");\n')
    if interfaces.get('energy_card_operands_2022'):
        component=interfaces['energy_card_operands_2022']
        with layout.open('a') as stream:
            stream.write('\n_Static_assert(sizeof(PlayerCounters)==4 && offsetof(PlayerCounters,energy_)==2 && sizeof(((PlayerCounters*)0)->energy_)==1, "native unsigned energy byte");\n')
            stream.write('\n'.join(f'_Static_assert({name}=={number}, "native energy-card enum");' for name,number in component['enum_values'].items())+'\n')
            stream.write('\n'.join(f'_Static_assert(GVC_PLAYER_COUNTERS(PCOUNTER_ENERGY,{n})=={0x2020000|(n<<8)}, "native packed energy cost");' for n in [1,2,3])+'\n')
    if interfaces.get('global_tutor_core_2022'):
        component=interfaces['global_tutor_core_2022']
        with layout.open('a') as stream:
            stream.write('\n_Static_assert(sizeof(tutor_destination_2022_t)==4 && sizeof(tutor_from_2022_t)==4 && TUTOR_HAND=='+('1' if canonical_spell_tutor else '0')+' && TUTOR2022_HAND==1, "native tutor domains");\n')
            stream.write('_Static_assert(sizeof(test_definition_2022_t)==216 && offsetof(test_definition_2022_t,qty)==174 && sizeof(((test_definition_2022_t*)0)->qty)==2 && offsetof(test_definition_2022_t,no_shuffle)==192 && offsetof(test_definition_2022_t,create_minideck)==194 && offsetof(test_definition_2022_t,storage)==212, "native tutor fields");\n')
            stream.write('_Static_assert(sizeof(target_t)==8 && offsetof(card_instance_t,targets)==116 && offsetof(card_instance_t,number_of_targets)==54, "native control-transfer target cursor");\n')
            for group,prefix in [(component['destinations'],'TUTOR_'),(component['origins'],'TUTOR_FROM_')]:
                stream.write('\n'.join(f'_Static_assert({name.replace(prefix,("TUTOR2022_" if prefix=="TUTOR_" else "TUTOR_FROM2022_"),1)}=={number}, "native tutor value");' for name,number in group.items())+'\n')
    if interfaces.get('action_core_2022'):
        component=interfaces['action_core_2022']
        with layout.open('a') as stream:
            stream.write('\n_Static_assert(sizeof(test_definition_2022_t)==216 && offsetof(test_definition_2022_t,not_me)==170 && sizeof(target_definition_2022_t)==84 && sizeof(actions_2022_t)==4 && sizeof(counter_t)==4, "native action/copy interfaces");\n'+
                         ('_Static_assert(__builtin_types_compatible_p(actions_t,actions_2022_t), "single native action type");\n'
                          '#ifndef __clang__\n'
                          '_Static_assert(((actions_t)-1)>0, "GCC native unsigned action enum");\n'
                          '#endif\n' if (root/'docs/recovery-evidence/canonical-helper-domains.json').exists() else
                          '_Static_assert(((actions_2022_t)-1)>0, "native unsigned action word");\n'))
            stream.write('_Static_assert(sizeof(int[2][100])==800 && sizeof(int[151])==604 && sizeof(char[2][151])==302 && sizeof(dblword_628C10)==4 && sizeof(byte_628C0C)==1, "native action/copy fixed arrays and network fields");\n')
            stream.write('\n'.join(f'_Static_assert((uint32_t){("ACT_KILL_REMOVE" if name=="ACT_KILL_EXILE" else name)}=={number}U, "native action enum bits");' for name,number in component['enum_values'].items())+'\n')
    if interfaces.get('death_trigger_core_2022'):
        component=interfaces['death_trigger_core_2022']
        with layout.open('a') as stream:
            stream.write('\n_Static_assert(sizeof(test_definition_2022_t)==216 && offsetof(test_definition_2022_t,not_me)==170 && sizeof(((test_definition_2022_t*)0)->not_me)==2, "native death-trigger test");\n')
            stream.write('_Static_assert(offsetof(card_instance_t,targets[11].player)==204 && offsetof(card_instance_t,targets[11].card)==208 && sizeof(int[3])==12 && sizeof(card_counts[0])==7 && sizeof(card_counts[0][5])==1, "native death-trigger storage/cache");\n')
            stream.write('\n'.join(f'_Static_assert({name}=={number}, "native death-trigger enum");' for name,number in component['enum_values'].items() if name!='SF2_0')+'\n')
    if interfaces.get('dependency_repairs_2022'):
        with layout.open('a') as stream:
            stream.write('\n_Static_assert(sizeof(PlayerCounters)==4 && sizeof(player_counters)==8 && offsetof(PlayerCounters,poison_)==0 && offsetof(PlayerCounters,experience_)==1 && offsetof(PlayerCounters,energy_)==2, "native player counter bytes");\n')
            stream.write('_Static_assert(sizeof(player_bits)==8 && sizeof(player_bits[0])==4 && STATE_OUBLIETTED==0x800000 && sizeof(counter_t[9])==36, "native player flags, phase mask and counter-type array");\n')
    if interfaces.get('spell_trigger_native_2022'):
        with layout.open('a') as stream:
            stream.write('\n_Static_assert(sizeof(test_definition_2022_t)==216 && offsetof(test_definition_2022_t,not_me)==170, "native spell-trigger test");\n')
            stream.write('_Static_assert(offsetof(card_ext_t,special_flags2)==184, "native copied-spell word");\n')
    if interfaces.get('phyrexian_costs_2022'):
        with layout.open('a') as stream:
            stream.write('\n_Static_assert(sizeof(char*[6])==24 && sizeof(char*[6])/sizeof(char*)==6 && sizeof(charge_mana_called_from_pay_phyrexian_mana)==4, "native Phyrexian table/flag");\n')
            stream.write('_Static_assert(sizeof(card_flags)==8 && sizeof(pay_mana_xbugrwaU)==32 && COLOR_BLACK==1 && COLOR_COLORLESS==0 && COLOR_ARTIFACT==6, "native Krrik mana words/indices");\n')
            stream.write('\n'.join(f'_Static_assert({name}=={number}, "native Phyrexian enum value");' for name,number in interfaces['phyrexian_costs_2022']['enum_values'].items())+'\n')
    if interfaces.get('ward_recursion_guard_2022'):
        with layout.open('a') as stream:
            stream.write('\n_Static_assert(sizeof(target_t)==8 && offsetof(target_t,player)==0 && offsetof(target_t,card)==4, "native Ward recursion pair");\n')
            stream.write('_Static_assert(sizeof(target_definition_2022_t)==84 && offsetof(target_definition_2022_t,flags)==80 && offsetof(card_instance_t,regen_status)==40, "native Ward target/context flags");\n')
            stream.write('\n'.join(f'_Static_assert({name}=={number}, "native Ward guard event");' for name,number in interfaces['ward_recursion_guard_2022']['events'].items())+'\n')
    if interfaces.get('level_buddy_2022'):
        component=interfaces['level_buddy_2022']
        with layout.open('a') as stream:
            stream.write('\n_Static_assert(sizeof(keyword_t)==4 && sizeof(sp_keyword_t)==4 && sizeof(subtype_t)==4, "native Level Up/Buddy enum words");\n')
            stream.write('_Static_assert(sizeof(test_definition_2022_t)==216 && offsetof(test_definition_2022_t,message)==0 && sizeof(((test_definition_2022_t*)0)->message)==100, "native Buddy shared message prefix");\n')
            stream.write('\n'.join(f'_Static_assert({name}=={number}, "native Level Up/Buddy enum value");' for name,number in component['enum_values'].items())+'\n')
    if interfaces.get('entry_triggers_2022'):
        component=interfaces['entry_triggers_2022']
        with layout.open('a') as stream:
            stream.write('\n_Static_assert(sizeof(card_counts)==14 && sizeof(card_counts[0])==7 && sizeof(card_counts[0][0])==1, "native entry trigger count cache");\n')
            stream.write('_Static_assert(sizeof(test_definition_2022_t)==216 && offsetof(test_definition_2022_t,not_me)==170 && sizeof(((test_definition_2022_t*)0)->not_me)==2, "native entry trigger test filter");\n')
            stream.write('_Static_assert(offsetof(card_instance_t,regen_status)==40 && sizeof(resolve_trigger_t)==4, "native entry trigger context");\n')
            stream.write('\n'.join(f'_Static_assert({name}=={number}, "native entry trigger enum");' for name,number in component['enum_values'].items())+'\n')
    if interfaces.get('attacker_core_2022'):
        component=interfaces['attacker_core_2022']
        with layout.open('a') as stream:
            stream.write('\n_Static_assert(sizeof(declare_attackers_trigger_t)==4 && sizeof(((card_instance_t*)0)->info_slot)==4, "native attacker counter/flags width");\n')
            stream.write('_Static_assert(offsetof(card_instance_t,info_slot)==56 && offsetof(card_instance_t,targets[1].player)==124 && offsetof(card_instance_t,targets[3].player)==140 && offsetof(card_instance_t,targets[2].player)==132, "native attacker storage offsets");\n')
            stream.write('_Static_assert(sizeof(((card_instance_t*)0)->number_of_targets)==1 && sizeof(target_t[152])==1216 && sizeof(((card_instance_t*)0)->targets)==152, "native Support target copies");\n')
            values={**component['flags'],**component['enum_values']}
            stream.write('\n'.join(f'_Static_assert({name}=={number}, "native attacker/keyword enum value");' for name,number in values.items() if name!='SF_0')+'\n')
    if interfaces.get('activation_costs_2022'):
        from activation_cost_bodies import GRAVE_STORAGE
        component=interfaces['activation_costs_2022']
        with layout.open('a') as stream:
            stream.write('\n'+GRAVE_STORAGE+'\n_Static_assert(sizeof(activation_grave_storage_2022_t)==100 && offsetof(activation_grave_storage_2022_t,text)==0 && offsetof(activation_grave_storage_2022_t,target)==0, "native grave activation shared storage");\n')
            stream.write('_Static_assert(sizeof(int)==4 && offsetof(card_instance_t,targets[9].card)==192 && sizeof(((card_instance_t*)0)->info_slot)==4, "native activation index and report words");\n')
            for name,number in component['enum_values'].items():
                if name.startswith('GS_'):name=name.replace('GS_','GS2022_',1)
                stream.write(f'_Static_assert({name}=={number}, "native specialized activation enum");\n')
    if interfaces.get('mana_production_2022'):
        with layout.open('a') as stream:
            stream.write('\n_Static_assert(SF2_QUARUM_TRENCH_GNOMES==0x800000, "typed mana replacement mask");\n')
    keyword_enum = interfaces.get('extension_keyword_enum')
    protection_dispatch = interfaces.get('protection_dispatch_2022')
    if protection_dispatch:
        with layout.open('a') as stream:
            stream.write('\n_Static_assert(sizeof(is_protected_from_mode_2022_t) == 4, "protection mode width");\n' +
                         '_Static_assert(sizeof(player_bits) == 8 && sizeof(player_bits[0]) == 4, "native player-bit words");\n' +
                         '\n'.join(f'_Static_assert({name.replace("IPF_", "IPF2022_")} == {number}, "2022 protection mode");'
                                   for name,number in protection_dispatch['modes']['members'].items()) + '\n')
    if keyword_enum:
        with layout.open('a') as stream:
            stream.write('\n_Static_assert(sizeof(sp_keyword2_t) == 4, "second keyword word");\n' +
                         '\n'.join(f'_Static_assert({name} == {number}, "second keyword bit");'
                                   for name, number in keyword_enum['members'].items()) + '\n')
    for function in manifest['accepted_functions']:
        for replacement in function.get('keyword_mask_evidence', {}).get('replacements', []):
            for name,number in replacement['bits'].items():
                assert name not in keyword_values or keyword_values[name] == number
                keyword_values[name] = number
        for replacement in function.get('state_mask_evidence', {}).get('replacements', []):
            for name, number in replacement['bits'].items():
                assert name not in state_values or state_values[name] == number
                state_values[name] = number
    with layout.open('a') as stream:
        stream.write('\n' + '\n'.join(f'_Static_assert({name} == {number}, "recovered keyword mask");'
                                      for name,number in keyword_values.items()) + '\n')
        stream.write('\n' + '\n'.join(f'_Static_assert({name} == {number}, "recovered state mask");'
                                      for name,number in state_values.items()) + '\n')
    if canonical_storm:
        with layout.open('a') as stream:
            stream.write('_Static_assert(sizeof(storm_mode_t)==4 && __builtin_types_compatible_p(storm_mode_t,storm_mode_2022_t), "canonical storm mode type");\n')
            for name,number in canonical_storm['native_values'].items():
                stream.write(f'_Static_assert({name}=={number} && {name.replace("STORM_","STORM2022_",1)}=={number}, "native canonical storm bit");\n')
    if canonical_spell_tutor:
        from canonicalize_spell_tutor import SPECS
        with layout.open('a') as stream:
            for typename,modern,_,_ in SPECS:
                stream.write(f'_Static_assert(sizeof({typename})==4 && __builtin_types_compatible_p({typename},{modern}), "canonical spell/tutor typedef identity");\n')
                for name,number in canonical_spell_tutor['values'][typename].items():
                    stream.write(f'_Static_assert({name}=={number}, "canonical native spell/tutor value");\n')
    if canonical_reanimation:
        with layout.open('a') as stream:
            stream.write('\n_Static_assert(sizeof(reanimate_mode_t)==4 && (reanimate_mode_t)-1 < 0, "signed native reanimation word");\n')
            stream.write('_Static_assert(sizeof(counter_t)==4, "native counter word");\n')
            stream.write('_Static_assert(__builtin_types_compatible_p(reanimate_mode_t,reanimate_mode_2022_t), "one reanimation enum type");\n')
            stream.write('_Static_assert(__builtin_types_compatible_p(CardFunction,int (*)(int,int,event_t)), "effect callback type");\n')
            stream.write('_Static_assert(__builtin_types_compatible_p(ReanimationBeforeEtb_2022,void (*)(int,int,int,int)), "before ETB callback type");\n')
            from promote_recovery import REANIMATION_DOMAIN_NAMES
            for name,number in canonical_reanimation['values'].items():
                stream.write(f'_Static_assert({name}=={number} && {REANIMATION_DOMAIN_NAMES[name]}=={number}, "canonical reanimation action");\n')
    if storage_evidence.exists():
        storage = json.loads(storage_evidence.read_text())
        assertions = []
        for typename, size in storage["source_layouts"].items():
            size=canonical_records.get('runtime_bytes',{}).get(typename,size)
            assertions.append(f'_Static_assert(sizeof({typename}) == {size}, "source stack view size");')
        for typename, record in storage["target_layouts"].items():
            for member, offset in record["members"].items():
                assertions.append(f'_Static_assert(offsetof({typename}, {member}) == {offset}, "shared stack view offset");')
        for typename in canonical_records.get('runtime_bytes',{}):
            modern=typename.removesuffix('_t')+'_2022_t'
            if source_names(root).get(modern,modern)==typename:
                continue  # The alias is retired; an identity assertion would be tautological.
            assertions.append(f'_Static_assert(__builtin_types_compatible_p({typename}, {modern}), "canonical record typedef identity");')
        with layout.open("a") as stream:
            stream.write("\n" + "\n".join(assertions) + "\n")
    # Layout assertions retain ledger identities; compile them against the actual
    # canonical declarations, without recreating compatibility aliases in C.
    from canonicalize_helpers import source_names,rename,verify as verify_helpers
    if not canonical_helpers:canonical_helpers=verify_helpers(root)
    if canonical_helpers:layout.write_text(rename(layout.read_text(),source_names(root)))
    abi = subprocess.run(command + [str(layout)], capture_output=True, text=True)
    cpp_results={}
    cpp_sources=sorted(p.relative_to(root).as_posix() for p in (root/'functions').glob('*.cpp'))
    for source in cpp_sources:
        result=subprocess.run([command[0],'-target','i686-pc-windows-msvc','-std=gnu++17','-fsyntax-only',
            '-Werror=return-type','-Werror=uninitialized','-Werror=duplicate-decl-specifier','-I',str(root),str(root/source)],capture_output=True,text=True)
        cpp_results[source]=dict(exit_code=result.returncode,diagnostics=result.stderr)
    stub_results={s['source']:cpp_results[s['source']] for s in manifest.get('source_stubs',[])}
    (analysis/'source-cpp-syntax-final.json').write_text(json.dumps(cpp_results,indent=2))
    (analysis/'source-stub-syntax-final.json').write_text(json.dumps(stub_results,indent=2))
    manifest['source_stub_syntax_check']=dict(passed=sum(r['exit_code']==0 for r in stub_results.values()),
        failed=[s for s,r in stub_results.items() if r['exit_code']!=0],target='i686-pc-windows-msvc',language='gnu++17',
        counted_as_recovered_implementations=False,tests_run=False)
    summary = dict(passed=sum(r["exit_code"] == 0 for r in results.values()),
                   failed=[s for s,r in results.items() if r["exit_code"] != 0],
                   abi_exit_code=abi.returncode, abi_diagnostics=abi.stderr,
                   target="i686-pc-windows-msvc", language="gnu99", tests_run=False,
                   compatibility_defines={"functions/rules_engine.c": "strncasecmp=_strnicmp",
                                          "functions/helper_functions.c": "ssize_t=int"})
    summary['stub_source_units_checked']=len(stub_results)
    summary['cpp_source_units_checked']=len(cpp_results)
    summary['cpp_source_failed']=[s for s,r in cpp_results.items() if r['exit_code']]
    if canonical_helpers:summary['canonical_helpers']=canonical_helpers
    summary['stub_source_failed']=manifest['source_stub_syntax_check']['failed']
    (analysis / "source-check-summary.json").write_text(json.dumps(summary, indent=2))
    manifest["source_syntax_check"] = summary
    (root / "docs/recovery-evidence/recovered-functions.json").write_text(json.dumps(manifest, indent=2))
    print(json.dumps(summary, indent=2))
    raise SystemExit(bool(summary["failed"] or summary['cpp_source_failed'] or abi.returncode))


if __name__ == "__main__":
    main()
