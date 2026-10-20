"""Record the manually reviewed release-comparison pilot and its raw evidence hashes.

Review notes are local-body assessments, not gameplay tests or equivalence proofs.
Run after compare_releases.py and RunRecoveryComparison.java.
"""
import collections
import csv
import hashlib
import json
import subprocess
from pathlib import Path

from audit_source import function_body


REVIEWS = {
    'card_black_lotus': ('same-instructions', 'Same five-argument mana-helper call; only its address moves.'),
    'card_giant_growth': ('same-instructions', 'Same pump-helper call and argument constants.'),
    'card_counterspell': ('same-instructions', 'Same counterspell-helper call and argument constants.'),
    'get_power': ('same-instructions', 'Same EVENT_POWER query wrapper; referenced helper behavior is outside this assessment.'),
    'get_toughness': ('same-instructions', 'Same EVENT_TOUGHNESS query wrapper; referenced helper behavior is outside this assessment.'),
    'card_ancestral_recall': ('compiler-layout-candidate', 'Event blocks are laid out differently; reviewed draw-three, targeting and AI adjustment paths correspond. Target-definition layout differs between releases.'),
    'card_lightning_bolt': ('compiler-layout-candidate', 'Reviewed targeting, damage-three and resolve/retarget paths correspond despite different event grouping and code layout.'),
    'card_sol_ring': ('compiler-layout-candidate', 'Decompiled local bodies correspond, including mana production and AI adjustments.'),
    'card_dark_ritual': ('compiler-layout-candidate', 'Decompiled local bodies correspond; can-cast and resolve blocks move.'),
    'card_doubling_season': ('compiler-layout-candidate', 'Both tail-jump to global_enchantment. The 2020 wrapper has an extra frame-pointer prologue/epilogue. This wrapper does not contain the doubling logic.'),
    'draw_cards': ('compiler-layout-candidate', 'Both tail-jump to draw_cards_exe. The 2020 wrapper has an extra frame-pointer prologue/epilogue; native callee behavior is not compared.'),
    'global_enchantment': ('compiler-layout-candidate', 'Same local decision logic; an extra NOP changes branch instruction indices.'),
    'generic_activated_ability': ('compiler-layout-candidate', 'Same forwarded argument list to granted_generic_activated_ability; argument setup is scheduled differently.'),
    'create_legacy_effect': ('compiler-layout-candidate', 'Same native constructor call and info-slot assignment; EBX/ESI roles are exchanged.'),
    'count_graveyard': ('compiler-layout-candidate', 'Same two-player selection, -1 sentinel and 500-entry cap; loop organization/register allocation differ.'),
    'check_for_ability': ('compiler-layout-candidate', 'Same direct query and battlefield scan; keyword AND operand order and block layout differ.'),
    'card_winter_orb': ('confirmed-change', 'Known positive control: 2022 adds is_tapped(player,card) before the untap-limiting helper. The existing source still lacks that guard.'),
    'card_howling_mine': ('confirmed-change', 'The is_humiliated call changes from two arguments to three, passing AL_TRIG (2).'),
    'card_static_orb': ('confirmed-change', 'The is_humiliated call now passes AL_STATIC (4); its existing same-card duplicate scan is retained.'),
    'card_mystic_remora': ('confirmed-change', 'Replaces new_specific_spell_played with sp_ability_trigger, sets test.type_flag=2, and resolves the payment/draw path through EVENT_SPA.'),
    'card_rhystic_study': ('confirmed-change', 'Replaces return-value-gated new_specific_spell_played with sp_ability_trigger registration and a separate EVENT_SPA payment/draw branch.'),
    'card_sylvan_library': ('confirmed-change', 'The is_humiliated call now passes AL_TRIG (2). Other draw/return-card paths remain compiler/layout candidates.'),
    'untap_only_one_permanent_type': ('confirmed-change', 'Adds a battlefield scan: other in-play copies with matching card ID have info_slot set to 2. Also passes AL_TRIG to is_humiliated.'),
    'draw_a_card': ('confirmed-change', 'Replaces native add_card_to_hand_exe calls plus preincrement/decrement hand-count pairs with DLL add_card_to_hand calls and decrements. The final successful-draw increment remains.'),
    'lose_life': ('confirmed-change', 'Adds life_total_can_change, Lichs Mastery and another draw-cards reaction path; uses AL_TRIG and the new gain_life_impl. Full APNAP/callback behavior still requires focused recovery.'),
    'damage_creature': ('confirmed-change', 'Adds planeswalker attack metadata, target type, source-being-cast tagging and expanded damage-source keyword handling; sp_keywords is copied through the extension. Full recovery needs extension integration.'),
    'is_what': ('confirmed-change', 'Adds an out-of-play creature-type exception for card CSV ID 0x5a13 and reads special_flags2 through the extension rather than targets[17].player.'),
    'add_counters': ('confirmed-change', 'DWARF/API changes from four parameters to six, carrying source and target separately; positive-count guard precedes adjustment. Already recovered as add_counters_from_source, with old API retained for compatibility.'),
    'can_regenerate': ('confirmed-change', 'Replaces can_use_activated_abilities with direct targets[17].card eligibility: flags<=0 or !(flags&0x28). Requires the newer ability-loss representation; not safe to copy into the old mask scheme independently.'),
    'gain_life': ('refactor-needs-followup', '2022 is a 17-byte wrapper calling gain_life_impl(player,amount,0), replacing a 2544-byte body. Compare the old body to that worker before classifying behavior.'),
}

REVIEWED_RELEASE_HASHES = {
    '2020': '927d076ca9e6a46b8a2bde0aeeed5e3b847ec8f55d22cebc9f92744bea460759',
    '2022': '52793f60124a3b83d8b2728d145e918101a395b4e8d1c0af69cddcfc87ef10e5',
}


def sha(data):
    return hashlib.sha256(data).hexdigest()


def main():
    root = Path(__file__).resolve().parents[2]
    cache = root / '.recovery/comparison-pilot'
    report = json.loads((cache / 'pilot.json').read_text())
    for year, expected_hash in REVIEWED_RELEASE_HASHES.items():
        assert report['inputs'][year]['sha256'] == expected_hash, 'Manual review applies only to the recorded release pair'
    assert set(REVIEWS) == {r['name'] for r in report['pilot']}
    vt_rows = list(csv.DictReader((cache / 'version-tracking.tsv').open(), delimiter='\t'))
    vt_pairs = {(int(r['source'], 16), int(r['destination'], 16)) for r in vt_rows
                if r['correlator'] == 'Exact Symbol Name Match'}
    expected = {(int(r['releases']['2020']['address'], 16), int(r['releases']['2022']['address'], 16))
                for r in report['pilot']}
    assert vt_pairs == expected, (vt_pairs - expected, expected - vt_pairs)
    accepted = {int(f['address'], 16): f for f in json.loads(
        (root / 'docs/recovery-evidence/recovered-functions.json').read_text())['accepted_functions']}
    for record in report['pilot']:
        name = record['name']
        category, note = REVIEWS[name]
        record['review'] = dict(category=category, note=note, gameplay_validated=False)
        for year, release in record['releases'].items():
            for suffix in ['c', 'asm']:
                path = cache / f'{year}-{name}.{suffix}'
                release[f'{suffix}_artifact'] = path.relative_to(root).as_posix()
                release[f'{suffix}_sha256'] = sha(path.read_bytes())
        source = record['releases']['2020']['source']
        baseline = subprocess.check_output(['git', 'show', f'427968c:{source}'], cwd=root).decode('latin1').replace('\r\n', '\n')
        current = (root / source).read_bytes().decode('latin1').replace('\r\n', '\n')
        old_body, current_body = function_body(baseline, name), function_body(current, name)
        assert old_body and current_body, name
        record['existing_source'] = dict(path=source, baseline_commit='427968c',
            baseline_body_sha256=sha(old_body.encode()), current_body_sha256=sha(current_body.encode()),
            body_text_identical_to_baseline=old_body == current_body)
        recovered = accepted.get(int(record['releases']['2022']['address'], 16))
        if recovered:
            record['existing_source']['recovered_target_adapter'] = recovered['name']
    categories = collections.Counter(r['review']['category'] for r in report['pilot'])
    report['review_summary'] = dict(categories=categories, sample='Purposive controls and shared helpers; not a random sample and not an estimate of project-wide change counts.')
    report['repository_head'] = subprocess.check_output(['git', 'rev-parse', 'HEAD'], cwd=root, text=True).strip()
    report['identity_pairs_csv_sha256'] = sha((cache / 'identity-pairs.csv').read_bytes())
    for year in ['2020', '2022']:
        units = list(csv.DictReader((root / f'.recovery/{year}-units.csv').open()))
        report['inputs'][year]['project_compilers'] = dict(collections.Counter(
            r['producer'] for r in units if r['source'].startswith(('cards/', 'functions/'))))
    report['version_tracking'] = dict(project='.recovery/ghidra/ComparisonPilot.gpr',
        session='2020-to-2022-pilot-abi', source='2020-ManalinkEh.analysis.dll',
        destination='2022-ManalinkEh.analysis.dll', markup_applied=False,
        correlator_counts=dict(collections.Counter(r['correlator'] for r in vt_rows)),
        all_30_expected_symbol_pairs_verified=True, raw_sha256=sha((cache / 'version-tracking.tsv').read_bytes()),
        entry_regparm_conventions_applied={'2020': 423, '2022': 516})
    report['limitations'] = [
        'Local-body comparison does not prove equal behavior of callees or referenced data.',
        'Ghidra DWARF import reports unsupported optimized local locations. Native external call models are incomplete.',
        'Exact instruction correlators mask operands and may miss changed constants; their score is not a semantic similarity percentage.',
        'Gain-life refactoring requires matching the old body to the new worker, not just comparing equal names.',
        'Existing source implementations were not changed by this pilot. API/mask/extension prerequisites must be ported before caller changes.',
    ]
    destination = root / 'docs/recovery-evidence/modified-functions-pilot.json'
    destination.write_text(json.dumps(report, indent=2) + '\n')
    print(json.dumps(dict(categories=categories, verified_symbol_pairs=len(vt_pairs), evidence=str(destination)), indent=2))


if __name__ == '__main__':
    main()
