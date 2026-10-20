"""Record the fixed random sample's manual reviews and stratified estimates.

The estimates count observed local code/API changes, not proven behavioral
inequivalence. Compiler/layout candidates remain candidates, not proofs.
"""
import argparse
import collections
import csv
import json
import math
import subprocess
from pathlib import Path

from audit_source import function_body
from compare_releases import digest
from record_comparison_pilot import REVIEWED_RELEASE_HASHES


REVIEWS = {
    'card_arena_athlete': ('compiler-layout-candidate', 'Same heroic trigger, target selection and until-EOT blocking restriction; decompiled local statements correspond.'),
    'card_rampant_growth': ('compiler-layout-candidate', 'Same AI adjustment, tutor_basic_land arguments and destroy-on-resolution path.'),
    'card_azorius_ploy': ('confirmed-change', 'Two target definitions and target slots replace the old single Maze-of-Ith-style effect; resolution separately prevents damage dealt by target 0 and damage dealt to target 1.'),
    'card_phytotitan': ('compiler-layout-candidate', 'Same mandatory death trigger and same legacy callback; early-return/block layout differs. Callback behavior is not assessed here.'),
    'card_runehorn_hellkite': ('compiler-layout-candidate', 'Same graveyard activation costs, exile result and APNAP discard/draw-seven sequence; conditional code generation differs.'),
    'card_reap_intellect': ('confirmed-change', 'Adds special-flag checks, explicit variable mana charging and set_x_for_x_spells; target definition changes and lobotomy_effect gains an argument. VLA stack probing prevents trusting the entire raw resolution decompilation.'),
    'card_serene_offering': ('compiler-layout-candidate', 'Same targeting and resolve path (CMC lookup, destroy enchantment, gain life); event block grouping differs.'),
    'card_chatter_of_squirrel': ('compiler-layout-candidate', 'Same token ID, flashback costs, resolve and exile/destroy paths; scheduling differs.'),
    'card_caverns_of_despair': ('compiler-layout-candidate', 'Same attack/block count thresholds and enchant_world tail call; common condition is duplicated between event blocks.'),
    'card_harbor_bandit': ('same-instructions', 'Same symbolic instruction stream and forwarded helper arguments. Raw C exposes an incomplete callee signature, so the local instruction comparison is the evidence.'),
    'card_anvil_of_bagardan': ('confirmed-change', 'Replaces one broad humiliation check with AL_TRIG for draw/discard and AL_STATIC for the maximum-hand-size branch.'),
    'card_thrull_wizard': ('compiler-layout-candidate', 'Reviewed counterspell target, three-generic/one-black payment choices, dialog and counterspell paths correspond despite larger code and split branches.'),
    'card_cage_of_hands': ('confirmed-change', 'Adds an explicit aura-event/casting guard before the disabling_aura fallback. Activated bounce behavior remains. This is a call-path change, not evidence of a new card effect.'),
    'card_verdeloth_the_ancient': ('same-instructions', 'Same local instruction stream, legend/ETB/kicker calls, token amount and subtype boosts; union member spelling differs only in Ghidra C.'),
    'card_brindle_shoat': ('confirmed-change', 'The 3/3 token now receives literal green color 8 instead of calling get_sleighted_color_test for that value. Further token API behavior is outside this local assessment.'),
    'player_sacrifices_a_creature': ('compiler-layout-candidate', 'Same fourteen sacrifice arguments; argument setup order differs.'),
    'glyph_of_destruction_boost_legacy': ('compiler-layout-candidate', 'Same damage-target power boost and end-of-combat removal; local C corresponds.'),
    'when_attacks_pump_self': ('confirmed-change', 'Uses pump_until_eot_merge_previous_alt with trailing 0 and category-aware AL_TRIG checks in AI boost paths.'),
    'copy_spell_from_stack': ('confirmed-change', 'Migrates to a six-argument copy worker with forced_target=NULL. The apparent 0-to-0xc0000 flag change is compiler specialization: old DWARF fixes special_flags=0xc0000 and ECX=0 supplies special_flags3.'),
    'show_deck': ('confirmed-change', 'New assembly store resets hack_show_deck_force_expansion to 0 before the native show_deck_exe tail jump. Raw C ends at halt_baddata due to the incomplete native model; the store is independently verified in assembly.'),
    'is_legal_block_impl': ('confirmed-change', 'Delegates base legality to new is_legal_block_impl2 and adds scans/counting of other blockers to satisfy multi-blocker requirements. The new worker was not substituted for the sampled entry.'),
    'manifest_iid_impl': ('compiler-layout-candidate', 'Same regparm3 manifest setup, ownership, legacy creation and face-down handling; equality operand order/scheduling differ.'),
    'genju': ('confirmed-change', 'Adds check_for_death_trigger_removal before return-legacy creation. Spirit subtype setup moves from add_a_subtype on the land to targets[9].card=0xbc on the animation legacy.'),
    'lose_2_life_thunk': ('same-instructions', 'Same lose_life(player,age_counters*2) and return 1; callee behavior may differ.'),
    'boost_creature_type': ('same-instructions', 'Same forwarding of subtype, power, toughness, abilities and flags to boost_subtype.'),
    'in_play_and_attacking': ('compiler-layout-candidate', 'Equivalent NULL/state-bit test expressed with an inverted branch; same returned instance or NULL.'),
    'gain_control_until_source_is_in_play_and_tapped': ('confirmed-change', 'After successful gain_control_until_clause, calls alternate_legacy_text(1,player,legacy). The added behavior is display text, not a claimed new control rule.'),
    'wild_growth_aura_all_one_color': ('compiler-layout-candidate', 'Same seven-argument wga_backend call and mode; argument setup scheduling differs.'),
    'get_hacked_land_text': ('compiler-layout-candidate', 'Reviewed subtype/color mappings, article/case formatting and hacking paths correspond; switch and loop lowering differ. All 24 pointed-to land-word strings were independently checked equal.'),
    'dash_cost_cless': ('confirmed-change', 'The cost-reducer humiliation check gains AL_STATIC; same two-mana reduction and zero floor.'),
}

CHANGE_KINDS = {
    'card_azorius_ploy': 'logic', 'card_reap_intellect': 'logic',
    'card_anvil_of_bagardan': 'interface-migration', 'card_cage_of_hands': 'event-filter',
    'card_brindle_shoat': 'token-color', 'when_attacks_pump_self': 'interface-migration',
    'copy_spell_from_stack': 'interface-migration', 'show_deck': 'display-state',
    'is_legal_block_impl': 'logic', 'genju': 'logic',
    'gain_control_until_source_is_in_play_and_tapped': 'display-text',
    'dash_cost_cless': 'interface-migration',
}


def wilson(k, n):
    z = 1.959963984540054
    p = k / n
    denominator = 1 + z * z / n
    center = (p + z * z / (2 * n)) / denominator
    margin = z * math.sqrt(p * (1 - p) / n + z * z / (4 * n * n)) / denominator
    return [center - margin, center + margin]


def main():
    parser = argparse.ArgumentParser(__doc__)
    parser.add_argument('--session', default='2020-to-2022-random-v1')
    args = parser.parse_args()
    root = Path(__file__).resolve().parents[2]
    output = root / '.recovery/comparison-random'
    frozen = json.loads((output / 'selection.json').read_text())
    observations = json.loads((output / 'observations.json').read_text())
    assert set(REVIEWS) == {r['name'] for r in observations}
    assert len(observations) == len(frozen['selection']) == 30
    vt = list(csv.DictReader((output / 'version-tracking.tsv').open(), delimiter='\t'))
    matches = {(int(r['source'], 16), int(r['destination'], 16)) for r in vt if r['correlator'] == 'Exact Symbol Name Match'}
    expected = {(r['old']['start'], r['new']['start']) for r in frozen['selection']}
    assert matches == expected
    for year, expected_hash in REVIEWED_RELEASE_HASHES.items():
        assert digest((root / f'.recovery/{year}-ManalinkEh.dll').read_bytes()) == expected_hash
        decompiled = list(csv.DictReader((output / f'{year}-decompiled.tsv').open(), delimiter='\t'))
        assert len(decompiled) == 30 and all(r['status'] == 'ok' for r in decompiled)
    old_by_name = {r['old']['name']: r['old'] for r in frozen['selection']}
    for record in observations:
        name = record['name']
        category, note = REVIEWS[name]
        record['review'] = dict(category=category, note=note, gameplay_validated=False)
        if name in CHANGE_KINDS:
            record['review']['change_kind'] = CHANGE_KINDS[name]
        if category == 'same-instructions':
            assert record['status'] == 'same-symbolic-instructions'
        for year, data in record['releases'].items():
            assert data['fully_decoded']
            for suffix in ['c', 'asm']:
                path = output / f'{year}-{record["artifact"]}.{suffix}'
                data[f'{suffix}_artifact'] = path.relative_to(root).as_posix()
                data[f'{suffix}_sha256'] = digest(path.read_bytes())
        source = old_by_name[name]['source']
        old_text = subprocess.check_output(['git', 'show', f'427968c:{source}'], cwd=root).decode('latin1').replace('\r\n', '\n')
        current_text = (root / source).read_bytes().decode('latin1').replace('\r\n', '\n')
        old_body = function_body(old_text, name)
        current_body = function_body(current_text, name)
        assert old_body and current_body, name
        record['source_reference'] = dict(path=source, baseline_commit='427968c',
            baseline_body_sha256=digest(old_body.encode()), current_body_sha256=digest(current_body.encode()),
            body_text_identical_to_baseline=old_body == current_body)
    strata = {}
    total_frame = sum(frozen['frame_counts'].values())
    for stratum, population in frozen['frame_counts'].items():
        selected = [r for r in observations if r['stratum'] == stratum]
        n = len(selected)
        changed = sum(r['review']['category'] == 'confirmed-change' for r in selected)
        strata[stratum] = dict(eligible_population=population, sample=n, confirmed_changes=changed,
            categories=dict(collections.Counter(r['review']['category'] for r in selected)),
            observed_change_rate=changed / n, wilson_95=wilson(changed, n),
            population_weight=population / total_frame)
    estimate = sum(s['population_weight'] * s['observed_change_rate'] for s in strata.values())
    report = dict(selection=frozen, selection_sha256=digest((output / 'selection.json').read_bytes()),
        sampling_frame_csv_sha256=digest((output / 'sampling-frame.csv').read_bytes()),
        repository_head=subprocess.check_output(['git', 'rev-parse', 'HEAD'], cwd=root, text=True).strip(),
        release_sha256=REVIEWED_RELEASE_HASHES,
        results=dict(categories=dict(collections.Counter(r['review']['category'] for r in observations)),
            change_kinds=dict(collections.Counter(CHANGE_KINDS.values())), strata=strata,
            population_weighted_observed_change_rate=estimate,
            inference='Exploratory local code/API-change rate in the eligible shared-C-function frame. Not the rate of changed gameplay, nor a total remaining-recovery estimate. Wilson intervals cover sampling uncertainty under the review labels, not classification errors, unpaired functions or indirect callee/data changes.'),
        version_tracking=dict(project='.recovery/ghidra/ComparisonPilot.gpr', session=args.session,
            expected_pairs_verified=len(matches), markup_applied=False,
            correlator_counts=dict(collections.Counter(r['correlator'] for r in vt)),
            raw_sha256=digest((output / 'version-tracking.tsv').read_bytes())),
        supporting_context=json.loads((output / 'supporting-context.json').read_text()),
        functions=observations,
        limitations=[
            'No selection by size/similarity and no replacement of hard functions; seed and frame frozen before decoding.',
            'Only unambiguously paired C functions are in the frame; known pilot entries and catalogued recovered target entries are excluded.',
            'Same local instructions or corresponding C does not prove unchanged callee/data behavior.',
            'Raw C has unresolved optimized parameter locations and incomplete native call models. Assembly and DWARF are used to resolve or qualify claims.',
            'Code/API changes include display changes, optional arguments and event filters; they are not all new card effects or bug fixes.',
        ])
    path = root / 'docs/recovery-evidence/random-modification-sample.json'
    path.write_text(json.dumps(report, indent=2) + '\n')
    print(json.dumps(report['results'], indent=2))


if __name__ == '__main__':
    main()
