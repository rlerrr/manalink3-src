"""Typed storage views for the 2022 selector's mutually exclusive human/AI branches."""
import re

STORAGE = '''typedef union
{
  char error_reason[208];
  char multiblocker_error[200];
  target_t candidates[60];
} target_selection_storage_2022_t;
'''


def reconstruct(text):
    changes = []

    def replace(original, replacement, kind, **detail):
        nonlocal text
        count = text.count(original)
        assert count == detail.pop('occurrences', 1), (kind, count)
        text = text.replace(original, replacement)
        changes.append(dict(original=original, replacement=replacement, kind=kind, occurrences=count, **detail))

    replace('char err_reason [208];', 'target_selection_storage_2022_t selection;', 'shared selector storage')
    replace('*(int *)(err_reason + iVar3 * 8) = iVar7;', 'selection.candidates[iVar3].player = iVar7;',
            'candidate player store', sites=['265f729'])
    replace('*(int *)(err_reason + iVar3 * 8 + 4) = iVar5;', 'selection.candidates[iVar3].card = iVar5;',
            'candidate card store', sites=['265f730'])
    for player, first in [(1, "'\\x01'"), (0, "'\\0'")]:
        start = text.index('pcVar1 = err_reason + iVar3 * 8;')
        end = text.index('iVar3 = iVar3 + 1;', start)
        block = text[start:end]
        assert 'pcVar1[0] = ' + first + ';' in block
        assert block.count('pcVar1[') == 8
        replace(block, f'selection.candidates[iVar3].player = {player};\n      selection.candidates[iVar3].card = -1;\n      ',
                'player candidate words', player=player,
                sites=['265f75f', '265f76a'] if player else ['265f780', '265f78b'])
    start = text.index('CONCAT11(')
    end = text.index(' | 0x4000;', start) + len(' | 0x4000')
    replace(text[start:end], '(selection.candidates[_DAT_007a2fe4].player < 1 ? 0 : 0x100) |\n'
            '             (selection.candidates[_DAT_007a2fe4].card & 0xff) | 0x4000',
            'selected target word', machine_range=['265fa1f', '265fa47'])
    for original, replacement, site in [
            ('*(undefined4 *)(err_reason + _DAT_007a2fe4 * 8)', 'selection.candidates[_DAT_007a2fe4].player', '265f7d4'),
            ('*(int32_t *)(err_reason + _DAT_007a2fe4 * 8 + 4)', 'selection.candidates[_DAT_007a2fe4].card', '265f7ef'),
            ('*(int32_t *)(err_reason + _DAT_007a2fe4 * 8)', 'selection.candidates[_DAT_007a2fe4].player', '265f7e8')]:
        replace(original, replacement, 'selected candidate word', sites=[site])
    replace('char *pcVar1;', '', 'removed byte-store pointer')
    # This separate 200-byte local shares the first byte of the 208-byte error
    # view, but exists only in the multiblocker rejection branch.
    replace('sprintf(err_reason,(char *)0x73940c,0x728f6c);',
            'sprintf(selection.multiblocker_error,EXE_STR(0x73940c),EXE_STR(0x728f6c));',
            'multiblocker format pointers', addresses=['73940c', '728f6c'])
    replace('display_error_message(err_reason);', 'display_error_message(selection.multiblocker_error);',
            'multiblocker error view')
    replace('err_reason', 'selection.error_reason', 'human error view', occurrences=text.count('err_reason'))
    addresses = sorted(set(re.findall(r'\b_?DAT_([0-9a-fA-F]{8})\b', text)))
    for address in addresses:
        original = next(m[0] for m in re.finditer(r'\b_?DAT_' + address + r'\b', text))
        replace(original, 'EXE_DWORD(0x' + address.lstrip('0') + ')', 'live engine word',
                address=address.lstrip('0'), occurrences=text.count(original))
    addresses = sorted(set(re.findall(r'\(char \*\)0x([0-9a-f]+)', text)) - {'0'})
    for address in addresses:
        replace('(char *)0x' + address, 'EXE_STR(0x' + address + ')', 'live engine string',
                address=address, occurrences=text.count('(char *)0x' + address))
    replace('sprintf(EXE_STR(0x60a690),', 'sprintf((char *)EXE_STR(0x60a690),',
            'writable engine prompt', address='60a690')
    return text, changes
