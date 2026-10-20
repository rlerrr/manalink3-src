"""Pair release identities and prepare a conservative, 30-function change pilot.

Requires the verified inspection artifacts, pefile, and capstone. This reports
candidates, not semantic equivalence. It never changes recovered source or DLLs.
"""
import collections
import csv
import difflib
import hashlib
import json
from pathlib import Path

import capstone as cs
from capstone import x86_const as x86
from inspect_release import Image


PILOT = [
    'card_black_lotus', 'card_ancestral_recall', 'card_lightning_bolt',
    'card_giant_growth', 'card_counterspell', 'card_sol_ring', 'card_dark_ritual',
    'card_winter_orb', 'card_howling_mine', 'card_static_orb', 'card_mystic_remora',
    'card_rhystic_study', 'card_sylvan_library', 'card_doubling_season',
    'untap_only_one_permanent_type', 'draw_cards', 'draw_a_card', 'gain_life',
    'lose_life', 'damage_creature', 'get_power', 'get_toughness', 'count_graveyard',
    'is_what', 'global_enchantment', 'generic_activated_ability',
    'create_legacy_effect', 'add_counters', 'can_regenerate', 'check_for_ability',
]


def digest(data):
    return hashlib.sha256(data).hexdigest()


def read_functions(path):
    # Collapse repeated DIEs of the same identity/address, not aliases or clones.
    rows = {}
    for row in csv.DictReader(path.open()):
        if row['low_pc'] and row['high_pc'] and row['name']:
            row['start'] = int(row['low_pc'], 16)
            row['end'] = int(row['high_pc'], 16)
            rows[(row['source'], row['name'], row['start'], row['end'])] = row
    return list(rows.values())


def pair_identities(old, new):
    def group(rows, key):
        result = collections.defaultdict(list)
        for row in rows:
            result[key(row)].append(row)
        return result

    a = group(old, lambda r: (r['source'], r['name']))
    b = group(new, lambda r: (r['source'], r['name']))
    pairs = [(a[key][0], b[key][0], 'source-and-name')
             for key in sorted(a.keys() & b.keys()) if len(a[key]) == len(b[key]) == 1]
    used_a = {id(x) for x, _, _ in pairs}
    used_b = {id(y) for _, y, _ in pairs}
    # A moved-file match must be unique across the entire release, not merely
    # unique among the leftovers (which could hide another file's static).
    an = group(old, lambda r: r['name'])
    bn = group(new, lambda r: r['name'])
    for name in sorted(an.keys() & bn.keys()):
        if len(an[name]) == len(bn[name]) == 1:
            x, y = an[name][0], bn[name][0]
            if id(x) not in used_a and id(y) not in used_b:
                pairs.append((x, y, 'unique-name-moved-source'))
                used_a.add(id(x)); used_b.add(id(y))
    ambiguous = [dict(source=key[0], name=key[1], old=len(a[key]), new=len(b[key]))
                 for key in sorted(a.keys() & b.keys()) if len(a[key]) != 1 or len(b[key]) != 1]
    return pairs, ambiguous, len(old) - len(used_a), len(new) - len(used_b)


class Release:
    def __init__(self, cache, year, rows, identities):
        self.year = year
        self.image = Image((cache / f'{year}-ManalinkEh.dll').read_bytes())
        self.base = self.image.pe.OPTIONAL_HEADER.ImageBase
        candidates = collections.defaultdict(list)
        for name, value, section, kind, storage in self.image.symbols:
            if section > 0:
                address = self.base + self.image.pe.sections[section - 1].VirtualAddress + value
            elif section == -1:
                address = value
            else:
                continue
            # Import-library placeholders and linker configuration symbols can
            # have absolute values 0, 1, 3, 4, etc. They are not addresses.
            if address >= 0x10000 and name and not name.startswith('.'):
                candidates[address].append((storage != 2, name.removeprefix('_')))
        self.symbols = {a: sorted(names)[0][1] for a, names in candidates.items()}
        by_address = collections.defaultdict(set)
        for row in rows:
            by_address[row['start']].add(row['name'])
        for address, names in by_address.items():
            if len(names) == 1:
                self.symbols[address] = next(iter(names))
        self.identities = identities
        symbol_counts = collections.Counter(self.symbols.values())
        self.normalized_symbols = {
            address: identities.get(address, name if symbol_counts[name] == 1 else f'{name}@{address:x}')
            for address, name in self.symbols.items()
        }
        self.decoder = cs.Cs(cs.CS_ARCH_X86, cs.CS_MODE_32)
        self.decoder.detail = True

    def bytes(self, row):
        return self.image.pe.get_data(row['start'] - self.base, row['end'] - row['start'])

    def describe(self, row):
        raw = self.bytes(row)
        instructions = list(self.decoder.disasm(raw, row['start']))
        indices = {ins.address: i for i, ins in enumerate(instructions)}
        tokens, calls, constants, branches, listing = [], [], [], [], []
        for ins in instructions:
            operands = []
            flow = ins.group(cs.CS_GRP_CALL) or ins.group(cs.CS_GRP_JUMP)
            for op in ins.operands:
                if op.type == x86.X86_OP_REG:
                    value = ins.reg_name(op.reg)
                elif op.type == x86.X86_OP_IMM:
                    number = op.imm & 0xffffffff
                    if flow:
                        if row['start'] <= number < row['end']:
                            value = f'local-insn:{indices.get(number, "unresolved")}:'
                            if number not in indices:
                                value += hex(number - row['start'])
                        else:
                            value = self.normalized_symbols.get(number, hex(number))
                            if ins.group(cs.CS_GRP_CALL) or ins.mnemonic == 'jmp':
                                calls.append(self.symbols.get(number, hex(number)))
                    else:
                        # Resolve pointer immediates only at known symbol entries;
                        # never discard arbitrary constants that resemble addresses.
                        value = self.normalized_symbols.get(number, hex(number))
                        if number not in self.symbols:
                            constants.append(hex(number))
                elif op.type == x86.X86_OP_MEM:
                    mem = op.mem
                    displacement = mem.disp & 0xffffffff
                    value = (ins.reg_name(mem.segment), ins.reg_name(mem.base),
                             ins.reg_name(mem.index), mem.scale,
                             self.normalized_symbols.get(displacement, hex(displacement)))
                else:
                    value = f'operand-type:{op.type}'
                operands.append((op.size, value))
            token = json.dumps([ins.mnemonic, operands], separators=(',', ':'))
            tokens.append(token)
            listing.append(f'{ins.address:08x}  {ins.bytes.hex():24} {ins.mnemonic:8} {ins.op_str}  ; {token}')
            if ins.group(cs.CS_GRP_JUMP):
                branches.append(ins.mnemonic)
        consumed = sum(ins.size for ins in instructions)
        return dict(address=hex(row['start']), rva=hex(row['start'] - self.base),
                    source=row['source'], line=row['line'], bytes=len(raw),
                    sha256=digest(raw), instructions=len(instructions),
                    decoded_bytes=consumed, fully_decoded=consumed == len(raw),
                    calls=calls, branches=branches,
                    immediate_counts=dict(sorted(collections.Counter(constants).items())),
                    normalized_sha256=digest('\n'.join(tokens).encode()),
                    tokens=tokens, listing=listing)


def main():
    root = Path(__file__).resolve().parents[2]
    cache = root / '.recovery'
    output = cache / 'comparison-pilot'
    output.mkdir(exist_ok=True)
    rows = {y: read_functions(cache / f'{y}-functions.csv') for y in ['2020', '2022']}
    pairs, ambiguous, unmatched_old, unmatched_new = pair_identities(rows['2020'], rows['2022'])
    identities = {y: collections.defaultdict(set) for y in rows}
    for old, new, _ in pairs:
        key = f'{old["source"]}::{old["name"]}'
        identities['2020'][old['start']].add(key)
        identities['2022'][new['start']].add(key)
    # Aliased entry points can have more than one legitimate identity. Keep the
    # full set rather than silently picking whichever alias was visited last.
    releases = {y: Release(cache, y, rows[y], {a: '|'.join(sorted(keys)) for a, keys in identities[y].items()})
                for y in rows}
    selected = {}
    for old, new, method in pairs:
        if old['name'] in PILOT:
            assert old['name'] not in selected, old['name']
            selected[old['name']] = old, new, method
    assert set(selected) == set(PILOT), set(PILOT) - set(selected)
    evidence = []
    for name in PILOT:
        old, new, method = selected[name]
        descriptions = {y: releases[y].describe(r) for y, r in [('2020', old), ('2022', new)]}
        for year, description in descriptions.items():
            (output / f'{year}-{name}.asm').write_text('\n'.join(description.pop('listing')) + '\n')
        a, b = descriptions['2020'], descriptions['2022']
        equal = a['fully_decoded'] and b['fully_decoded'] and a['tokens'] == b['tokens']
        # Masked/normalized equality remains a local-body observation. It does
        # not establish equal behavior of callees or referenced data.
        record = dict(name=name, identity_method=method, releases=descriptions,
                      status='same-symbolic-instructions' if equal else 'review-required',
                      direct_targets_added=sorted(set(b['calls']) - set(a['calls'])),
                      direct_targets_removed=sorted(set(a['calls']) - set(b['calls'])))
        for description in descriptions.values():
            description.pop('tokens')
        evidence.append(record)
        atext = (output / f'2020-{name}.asm').read_text().splitlines()
        btext = (output / f'2022-{name}.asm').read_text().splitlines()
        (output / f'{name}.diff').write_text('\n'.join(difflib.unified_diff(atext, btext, '2020', '2022')) + '\n')
    for year, index in [('2020', 0), ('2022', 1)]:
        requested = [selected[n][index] for n in PILOT]
        (output / f'{year}-requests.tsv').write_text(''.join(
            f'{r["start"]:x}\t{r["source"]}\t{r["name"]}\t{r["end"]:x}\n' for r in requested))
    summary = dict(pilot_functions=len(evidence),
                   statuses=dict(collections.Counter(r['status'] for r in evidence)),
                   paired_identities=len(pairs), pairing_methods=dict(collections.Counter(p[2] for p in pairs)),
                   unpaired_old_records=unmatched_old, unpaired_new_records=unmatched_new,
                   ambiguous_shared_keys=len(ambiguous),
                   note='Unpaired is not equivalent to added/removed. Pairing does not establish unchanged behavior.')
    report = dict(summary=summary,
                  inputs={y: dict(sha256=digest(releases[y].image.data),
                                  inventory_sha256=digest((cache / f'{y}-functions.csv').read_bytes())) for y in rows},
                  normalization='Preserve registers, operand widths, constants and memory offsets; replace exact known symbol addresses and internal branch targets. No instruction/register reordering or callee-body equivalence inference.',
                  pilot=evidence, ambiguous_shared_keys=ambiguous)
    (output / 'pilot.json').write_text(json.dumps(report, indent=2))
    with (output / 'identity-pairs.csv').open('w', newline='') as stream:
        writer = csv.writer(stream); writer.writerow(['name', 'source_2020', 'va_2020', 'source_2022', 'va_2022', 'method'])
        writer.writerows((x['name'], x['source'], x['low_pc'], y['source'], y['low_pc'], method) for x, y, method in pairs)
    print(json.dumps(summary, indent=2))
    for r in evidence:
        print(r['name'], r['status'], '+', r['direct_targets_added'], '-', r['direct_targets_removed'])


if __name__ == '__main__':
    main()
