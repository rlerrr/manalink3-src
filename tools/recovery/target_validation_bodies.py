"""Scoped reconstructions for the 2022 targeting validator's optimized string operations."""
import re


def reconstruct(text):
    changes = []

    def replace(original, replacement, kind, **detail):
        nonlocal text
        count = text.count(original)
        assert count == detail.pop('occurrences', 1), (kind, count)
        text = text.replace(original, replacement)
        changes.append(dict(original=original, replacement=replacement, occurrences=count, kind=kind, **detail))

    replace('error_str._0_4_ = error_str._0_4_ & 0xffffff00;', "error_str[0] = '\\0';",
            'byte initialization', occurrences=2,
            sites=['265d74c', '265d87d'])
    replace('SUB41(allowed_controller & 1U,0)', '((allowed_controller & 1U) != 0)',
            'low-bit boolean', sites=['265d9ea', '265d9ed', '265d9f9', '265d9ff', '265da01', '265da04'])
    start = text.index('puVar15 = (uint *)error_str;')
    end = text.index("error_str[iVar18 + 7] = '\\0';", start) + len("error_str[iVar18 + 7] = '\\0';")
    replace(text[start:end], 'strcat(error_str,",effect");', 'inline effect append',
            machine_range=['265e183', '265e1d0'], literal_hex='2c65666665637400')
    # The second word-scan computes the terminator used by the optional comma.
    replace('puVar15 = (uint *)error_str;', '', 'removed scan setup')
    start = text.index('do {\n            puVar11 = puVar15;')
    end = text.index("pcVar16[1] = '\\0';", start) + len("pcVar16[1] = '\\0';")
    replace(text[start:end], 'strcat(error_str,",");', 'inline predicate comma',
            machine_range=['265db24', '265db5a'])
    for literal, start_at, end_at in [
            (',converted mana cost', '265e0d2', '265e0f7'),
            ('must have counters', '265e914', '265e93f'),
            ('cannot target self', '265ee06', '265ee31'),
            ('must damage creature', '265dfbd', '265dfe8'),
            ('cannot be regenerated', '265ee87', '265eeb2')]:
        at = text.index('pcVar7 = "' + literal + '";')
        start = text.rfind('uVar12 = 0xffffffff;', 0, at)
        loop = text.index('for (iVar4 = ', at)
        end = text.index('}', loop) + 1
        original = text[start:end]
        assert original.count('pcVar7 = error_str;') == 1 and original.count('for (') == 1
        assert '*pcVar16 = *pcVar7;' in original and original.count('do {') == 1
        result = literal if literal.startswith(',') else ',' + literal
        replace(original, 'strcat(error_str,"' + result + '");', 'inline literal append',
                machine_range=[start_at, end_at], literal=literal, prefix_comma=not literal.startswith(','))
    at = text.index("if (buffer[0] != ',')")
    start = text.index('uVar12 = 0xffffffff;', at)
    end = text.index("(buffer + ~uVar12 + 99)[1] = '\\0';", start) + len("(buffer + ~uVar12 + 99)[1] = '\\0';")
    replace(text[start:end], 'strcat(error_str,",");', 'inline formatted comma',
            machine_range=['265edd0', '265ede4'])
    at = text.index('puVar17 = (undefined4 *)&UNK_0275dde2;')
    start = text.rfind('uVar12 = 0xffffffff;', 0, at)
    loop = text.index('for (iVar4 = 5;', at)
    end = text.index('}', loop) + 1
    replace(text[start:end], 'strcat(error_str,",cannot target self");', 'inline five-word append',
            machine_range=['265e72f', '265e754'], literal_address='275dde2', literal_bytes=20)
    replace('(char *)(*(code *)extra)(who_chooses,tgt_player,tgt_card,iVar4,iVar13)',
            '((const char* (*)(int,int,int,int,int))extra)(who_chooses,tgt_player,tgt_card,iVar4,iVar13)',
            'five-argument predicate', sites=['265db0e'])
    replace('char *pcVar7;', 'const char *pcVar7;', 'borrowed predicate error')
    # No generated integer-as-pointer scan arithmetic remains after the appends.
    for declaration in ['char cVar1;', 'uint *puVar11;', 'uint *puVar15;', 'char *pcVar16;',
                        'undefined4 *puVar17;', 'byte bVar20;', 'bVar20 = 0;']:
        replace(declaration, '', 'removed scan temporary')
    # Native player_bits is a two-word array, not a pointer-shaped scalar.
    replace('*(uint *)(&player_bits + tgt_player * 4)', 'player_bits[tgt_player]',
            'player-bit array', occurrences=2, sites=['265d7a9'])
    addresses = sorted(set(re.findall(r'\(char \*\)0x([0-9a-f]+)', text)) - {'0'})
    for address in addresses:
        replace('(char *)0x' + address, 'EXE_STR(0x' + address + ')', 'live engine error string',
                address=address, occurrences=text.count('(char *)0x' + address))
    return text, changes
