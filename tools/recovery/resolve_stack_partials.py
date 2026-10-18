"""Resolve partial stack views only when one verified member matches offset and width."""
import re


def resolve_partials(body, views, layouts):
    resolved = []; unresolved = []
    for view in views:
        storage = view['storage']; members = view['members']
        inverse = {member: typ for typ, member in members.items()}
        pattern = re.compile(r'\b' + re.escape(storage) + r'\.(\w+)(?:\.(\w+))?\._(\d+)_(\d+)_')

        def matching(match):
            member, field, offset, size = match.groups(); offset = int(offset); size = int(size)
            typ = inverse[member]
            if field:
                if typ not in layouts or field not in layouts[typ]['members']:
                    return None
                if offset + size > layouts[typ]['member_sizes'][field]:
                    return None
                offset += layouts[typ]['members'][field]
            elif offset + size > view['target_extent']:
                return None
            candidates = []
            for target, alias in members.items():
                if target == 'int' and offset == 0 and size == 4:
                    candidates.append((storage + '.' + alias, 'integer'))
                if target not in layouts:
                    continue
                record = layouts[target]
                for name, at in record['members'].items():
                    # These fields require migration of the old aggregate APIs.
                    if name in {'flags', 'special_flags3', 'sp_keyword', 'sp_keyword_flag'} or (target == 'test_definition_t' and at >= 140):
                        continue
                    if at == offset and record['member_sizes'][name] == size:
                        candidates.append((storage + '.' + alias + '.' + name, record['member_kinds'][name]))
            if len(candidates) != 1:
                return None
            return candidates[0], offset, size

        # Standalone assignments have no assignment-expression result to model.
        assignment = re.compile(r'(?m)^(\s*)(' + pattern.pattern + r')\s*=\s*([^;\n]+);')
        def store(match):
            partial = pattern.fullmatch(match[2]); found = matching(partial)
            if found is None:
                return match[0]
            (field, kind), offset, size = found
            rhs = match[7]
            if kind == 'integer' and size == 4:
                rhs = '(int32_t)(' + rhs + ')'
            resolved.append(dict(original=match[2], replacement=field, access='store', offset=offset, bytes=size))
            return match[1] + field + ' = ' + rhs + ';'
        body = assignment.sub(store, body)

        def read(match):
            found = matching(match)
            if found is None:
                unresolved.append(match[0]); return match[0]
            # A remaining lvalue needs explicit handling, never a casted read.
            tail = body[match.end():]
            if re.match(r'\s*(?:=(?!=)|[+*/%&|^-]=|\+\+|--)', tail):
                unresolved.append(match[0]); return match[0]
            (field, kind), offset, size = found
            if kind not in {'integer', 'pointer'}:
                unresolved.append(match[0]); return match[0]
            replacement = '(uint32_t)(' + field + ')'
            if size < 4:
                replacement = '(' + replacement + ' & ' + hex((1 << (size * 8)) - 1) + 'U)'
            resolved.append(dict(original=match[0], replacement=replacement, access='read', offset=offset, bytes=size))
            return replacement
        body = pattern.sub(read, body)
    return body, resolved, sorted(set(unresolved))
