"""Recover constant C strings only at known string-argument positions."""
import re


LITERAL = r'"(?:\\.|[^"\\\r\n])*"|\'(?:\\.|[^\'\\\r\n])*\''


def byte_literal(data):
    return '"' + "".join(chr(b) if 32 <= b < 127 and b not in (34, 92)
                         else '\\"' if b == 34 else '\\\\' if b == 92
                         else f"\\{b:03o}" for b in data) + '"'


def readonly_string(image, address):
    for name, section in image.sections.items():
        start = image.pe.OPTIONAL_HEADER.ImageBase + section.VirtualAddress
        offset = address - start
        if name != ".rdata" or section.Characteristics & 0x80000000 or not 0 <= offset < section.SizeOfRawData:
            continue
        data = image.section_data(section)[offset:offset + 4096]
        end = data.find(b"\0")
        if end < 0:
            return None
        data = data[:end]
        # Dialog control strings are exactly the prefix byte plus an opcode.
        if len(data) == 2 and data[0] == 1 and 1 <= data[1] <= 27:
            return data
        if all(b in (9, 10, 13) or 32 <= b < 127 for b in data):
            return data
    return None


def recover_strings(text, image, aliases):
    masked = re.sub(LITERAL, lambda m: " " * len(m[0]), text)
    replacements = []
    evidence = []
    for match in re.finditer(r"\b(dialog_impl|new_default_test_definition)\s*\(", masked):
        name = match[1]
        start = match.end()
        level = 0
        argument = 0
        for at in range(start, len(masked)):
            char = masked[at]
            if char in "([":
                level += 1
            elif char in ")]":
                if char == ")" and level == 0:
                    pass
                else:
                    level -= 1
                    continue
            if not ((char == "," and level == 0) or (char == ")" and level == 0)):
                continue
            eligible = argument >= 3 if name == "dialog_impl" else argument == 2
            token = text[start:at]
            pointer = re.fullmatch(r"\s*(?:\(\s*(?:const\s+)?char\s*\*\s*\)\s*)?&"
                                   r"(UNK_[0-9a-fA-F]+|_rdata)\s*", token)
            if eligible and pointer:
                symbol = pointer[1]
                address = int(symbol[4:], 16) if symbol.startswith("UNK_") else aliases.get(symbol)
                data = readonly_string(image, address) if address is not None else None
                if data is not None:
                    replacements.append((start, at, byte_literal(data)))
                    evidence.append(dict(symbol=symbol, address=f"0x{address:x}", bytes=data.hex(),
                                         call=name, argument=argument))
            argument += 1
            start = at + 1
            if char == ")":
                break
    for start, end, replacement in sorted(set(replacements), reverse=True):
        text = text[:start] + replacement + text[end:]
    return text, evidence
