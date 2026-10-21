"""Check recovered C source syntax and fixed x86 slots; does not run tests/build a DLL."""
import concurrent.futures
import json
import re
import subprocess
from pathlib import Path


def main():
    root = Path(__file__).resolve().parents[2]
    analysis = root / ".recovery"
    analysis.mkdir(exist_ok=True)
    manifest = json.loads((root / "docs/recovery-evidence/recovered-functions.json").read_text())
    command = ["C:/Program Files/LLVM/bin/clang.exe", "-target", "i686-pc-windows-msvc",
               "-std=gnu99", "-fsyntax-only", "-ferror-limit=0", "-fno-color-diagnostics",
               "-Werror=implicit-function-declaration", "-Werror=implicit-int",
               "-Werror=incompatible-pointer-types", "-Werror=return-type",
               "-Wno-ignored-attributes", "-I", str(root)]

    def check(source):
        compatibility = source == "functions/rules_engine.c"
        result = subprocess.run(command + (["-Dstrncasecmp=_strnicmp"] if compatibility else []) +
                                [str(root / source)], capture_output=True, text=True)
        return source, dict(exit_code=result.returncode, diagnostics=result.stderr,
                            checker_only_compatibility_define=compatibility)

    with concurrent.futures.ThreadPoolExecutor(max_workers=4) as pool:
        results = dict(pool.map(check, sorted(manifest["source_files"])))
    (analysis / "source-syntax-final.json").write_text(json.dumps(results, indent=2))

    layout = analysis / "source-abi.c"
    layout.write_text('''#include "manalink.h"
#include <stddef.h>
_Static_assert(sizeof(card_instance_t) == 300, "instance size");
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
    test_core=interfaces.get('test_core_2022')
    if test_core:
        with layout.open('a') as stream:
            stream.write('\n_Static_assert(sizeof(test_definition_t)==208 && sizeof(test_definition_2022_t)==216 && sizeof(test_selection_predicate_2022_t)==4, "separate test layouts/callback");\n')
            for member in test_core['layout']['members']:
                name,offset,width=member['name'],member['offset'],member['bytes']
                stream.write(f'_Static_assert(offsetof(test_definition_2022_t,{name})=={offset} && sizeof(((test_definition_2022_t*)0)->{name})=={width}, "2022 test member {name}");\n')
                if name not in {'sp_keyword','sp_keyword_flag'}:
                    old_offset=offset-(8 if offset>=148 else 0)
                    stream.write(f'_Static_assert(offsetof(test_definition_t,{name})=={old_offset} && sizeof(((test_definition_t*)0)->{name})=={width}, "retained test member {name}");\n')
    target_selection = interfaces.get('target_selection_2022')
    if target_selection:
        targets_source = (root / 'functions/targets.c').read_text()
        storage = re.search(r'typedef union\s*\{[^}]+\}\s*target_selection_storage_2022_t;', targets_source)
        assert storage and re.sub(r'\s+', '', storage[0]) == re.sub(r'\s+', '', target_selection['shared_storage_declaration'])
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
            stream.write('\n_Static_assert(sizeof(target_definition_t)==80 && sizeof(target_definition_2022_t)==84, "separate target layouts");\n')
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
            stream.write('\n_Static_assert(sizeof(token_generation_t)==72 && sizeof(token_generation_2022_t)==76 && sizeof(token_action_2022_t)==4, "separate token layouts/actions");\n')
            for member in token_core['layout']['members']:
                stream.write(f'_Static_assert(offsetof(token_generation_2022_t,{member["name"]})=={member["offset"]}, "2022 token member {member["name"]}");\n')
                if member['name'] != 'special_flags3':
                    stream.write(f'_Static_assert(offsetof(token_generation_t,{member["name"]})=={member["offset"]}, "retained token prefix {member["name"]}");\n')
            for name,number in token_core['actions']['members'].items():
                stream.write(f'_Static_assert({name.replace("TOKEN_ACTION_","TOKEN2022_ACTION_")}=={number}, "2022 token action {name}");\n')
            stream.write('_Static_assert(TOKEN_ACTION_HASTE==8 && TOKEN_ACTION_CONVERT_INTO_ARTIFACT==32, "retained legacy token actions");\n'
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
            stream.write('\n_Static_assert(sizeof(sacrifice_2022_t)==4 && sizeof(sacrifice_t)==4, "sacrifice flag domains");\n'
                         '_Static_assert(SAC_SET_TARGETS==128, "retained legacy sacrifice flag");\n'+
                         '\n'.join(f'_Static_assert({name.replace("SAC_","SAC2022_")}=={number}, "native sacrifice flag");'
                                   for name,number in interfaces['sacrifice_core_2022']['enum']['members'].items())+'\n')
    if interfaces.get('generic_spell_2022'):
        with layout.open('a') as stream:
            stream.write('\n_Static_assert(sizeof(generic_spell_flags_2022_t)==4 && sizeof(generic_spell_flags_t)==4, "spell flag domains");\n'
                         '_Static_assert(GS_ONE_OR_MULTIPLE_TARGETS==8192 && GS_SAC_PERMANENT_AS_COST==16384, "retained spell flag values");\n'+
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
            stream.write('\n_Static_assert(sizeof(pump_ability_t)==36 && sizeof(pump_ability_2022_t)==48, "separate pump layouts");\n')
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
    if interfaces.get('global_tutor_core_2022'):
        component=interfaces['global_tutor_core_2022']
        with layout.open('a') as stream:
            stream.write('\n_Static_assert(sizeof(tutor_destination_2022_t)==4 && sizeof(tutor_from_2022_t)==4 && TUTOR_HAND==0 && TUTOR2022_HAND==1, "separate native tutor domains");\n')
            stream.write('_Static_assert(sizeof(test_definition_2022_t)==216 && offsetof(test_definition_2022_t,qty)==174 && sizeof(((test_definition_2022_t*)0)->qty)==2 && offsetof(test_definition_2022_t,no_shuffle)==192 && offsetof(test_definition_2022_t,create_minideck)==194 && offsetof(test_definition_2022_t,storage)==212, "native tutor fields");\n')
            stream.write('_Static_assert(sizeof(target_t)==8 && offsetof(card_instance_t,targets)==116 && offsetof(card_instance_t,number_of_targets)==54, "native control-transfer target cursor");\n')
            for group,prefix in [(component['destinations'],'TUTOR_'),(component['origins'],'TUTOR_FROM_')]:
                stream.write('\n'.join(f'_Static_assert({name.replace(prefix,("TUTOR2022_" if prefix=="TUTOR_" else "TUTOR_FROM2022_"),1)}=={number}, "native tutor value");' for name,number in group.items())+'\n')
    if interfaces.get('action_core_2022'):
        component=interfaces['action_core_2022']
        with layout.open('a') as stream:
            stream.write('\n_Static_assert(sizeof(test_definition_2022_t)==216 && offsetof(test_definition_2022_t,not_me)==170 && sizeof(target_definition_2022_t)==84 && sizeof(actions_2022_t)==4 && ((actions_2022_t)-1)>0 && sizeof(counter_t)==4, "native action/copy interfaces");\n')
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
    if storage_evidence.exists():
        storage = json.loads(storage_evidence.read_text())
        assertions = []
        for typename, size in storage["source_layouts"].items():
            assertions.append(f'_Static_assert(sizeof({typename}) == {size}, "source stack view size");')
        for typename, record in storage["target_layouts"].items():
            for member, offset in record["members"].items():
                # The old test API predates the inserted special-keyword fields;
                # target flags and token flags3 are separate migration work.
                if (typename == "test_definition_t" and offset >= 140) or member in {"flags", "special_flags3"}:
                    continue
                assertions.append(f'_Static_assert(offsetof({typename}, {member}) == {offset}, "shared stack view offset");')
        with layout.open("a") as stream:
            stream.write("\n" + "\n".join(assertions) + "\n")
    abi = subprocess.run(command + [str(layout)], capture_output=True, text=True)
    summary = dict(passed=sum(r["exit_code"] == 0 for r in results.values()),
                   failed=[s for s,r in results.items() if r["exit_code"] != 0],
                   abi_exit_code=abi.returncode, abi_diagnostics=abi.stderr,
                   target="i686-pc-windows-msvc", language="gnu99", tests_run=False,
                   compatibility_defines={"functions/rules_engine.c": "strncasecmp=_strnicmp"})
    (analysis / "source-check-summary.json").write_text(json.dumps(summary, indent=2))
    manifest["source_syntax_check"] = summary
    (root / "docs/recovery-evidence/recovered-functions.json").write_text(json.dumps(manifest, indent=2))
    print(json.dumps(summary, indent=2))
    raise SystemExit(bool(summary["failed"] or abi.returncode))


if __name__ == "__main__":
    main()
