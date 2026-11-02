"""Render the recovered finite keyword map using checked project enumerators."""
import collections,re
from canonicalize_helpers import mask
from keyword_lookup_analysis import recover


def bindings(table,card_header,definitions):
    cards=collections.defaultdict(list)
    for name,value in re.findall(r'\b(CARD_ID_\w+)\s*=\s*(\d+)',card_header):cards[int(value)].append(name)
    keywords={int(bit):name for name,bit in re.findall(r'^\s*(SP_KEYWORD_\w+)\s*=\s*1\s*<<\s*(\d+)',definitions,re.M)}
    used={i for row in table for i in range(32) if row['keywords']&(1<<i)}
    assert all(len(cards[row['csvid']])==1 for row in table) and used<=set(keywords)
    return dict(cards=[dict(csvid=row['csvid'],name=cards[row['csvid']][0]) for row in table],keywords=[dict(bit=i,name=keywords[i]) for i in sorted(used)])


def render(table,names):
    cards={v['csvid']:v['name'] for v in names['cards']};keywords={v['bit']:v['name'] for v in names['keywords']}
    assert set(cards)=={row['csvid'] for row in table} and len(set(cards.values()))==len(cards)
    assert all(re.fullmatch(r'CARD_ID_\w+',n) for n in cards.values()) and all(re.fullmatch(r'SP_KEYWORD_\w+',n) for n in keywords.values())
    groups=collections.defaultdict(list)
    for row in table:assert 0<row['keywords']<1<<31;groups[row['keywords']].append(row['csvid'])
    lines=['static int has_sp_key_by_csvid_impl(int csvid, int sp_key)','{','  switch (csvid)','  {']
    for mask_,ids in sorted(groups.items()):
        for card in sorted(ids):lines.append('  case '+cards[card]+':')
        flags=[keywords[i] for i in range(32) if mask_&(1<<i)];value=flags[0] if len(flags)==1 else '('+' | '.join(flags)+')'
        lines.append('    return sp_key & '+value+';')
        lines.append('')
    lines.extend(['  default:','    return 0;','  }','}'])
    return '\n'.join(lines)


def project(before,table,names):
    proof=recover(before);assert proof['table']==table;after=render(table,names)
    return after,dict(cards=len(table),keyword_bits=proof['counts']['keyword_bits'],keyword_combinations=len({r['keywords'] for r in table}),removed_gotos=len(re.findall(r'\bgoto\b',mask(before))),removed_lab_gotos=len(re.findall(r'\bgoto\s+LAB_[a-fA-F0-9]+\s*;',mask(before))),removed_labels=len(re.findall(r'(?m)^\s*(?:LAB_[a-fA-F0-9]+|joined_r0x[a-fA-F0-9]+):',mask(before))),source_lines_removed=before.count('\n')-after.count('\n'),table_sha256=proof['table_sha256'])


def enum_probes(names):
    checks=[f'_Static_assert({v["name"]} == {v["csvid"]}, "keyword_card_id");' for v in names['cards']]
    checks += [f'_Static_assert({v["name"]} == {1<<v["bit"]}U, "keyword_mask_bit");' for v in names['keywords']]
    checks += ['_Static_assert(sizeof(int) == 4 && sizeof(unsigned int) == 4 && sizeof(char) == 1 && (char)0xff == -1 && (char)0x80 == -128, "keyword_integer_model");']
    return '\n'.join(checks)+'\n'
