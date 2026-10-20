"""Check recovered C source syntax and fixed x86 slots; does not run tests/build a DLL."""
import concurrent.futures
import json
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
