"""Find literal uses of one native dialog-choice domain in a C definition.

This is a conservative naming aid. A separate literal/enum token projection
proves each edit; the flow analysis chooses where the names are meaningful.
Secondary dialogs, escaped locals, arithmetic and mixed-purpose storage are
excluded rather than assigned the primary dialog's choice vocabulary.
"""
import ast
import collections
import re
from pycparser import c_ast
from source_record_flow import inspect
from canonicalize_helpers import mask


def children(node):
    if node is None:
        return
    yield node
    for _, child in node.children():
        yield from children(child)


def integer(node):
    if not isinstance(node, c_ast.Constant) or node.type != 'int':
        return None
    value = node.value
    if not re.fullmatch(r'0[xX][0-9a-fA-F]+|0[0-7]*|[1-9][0-9]*', value):
        return None
    return int(value, 16 if value.lower().startswith('0x') else 8 if len(value) > 1 and value.startswith('0') else 10)


def literal_int(literal):
    """Read a plain int or single ASCII C character constant, both type int."""
    value = integer(c_ast.Constant('int', literal))
    if value is not None:
        return value
    if not re.fullmatch(r"'(?:[^'\\\n]|\\(?:[abfnrtv\\'\"?]|[0-7]{1,3}|x[0-9a-fA-F]+))'", literal):
        return None
    # Python and C share the accepted escapes, except C's optional \? escape.
    value = ast.literal_eval(literal.replace(r'\?', '?'))
    return ord(value) if len(value) == 1 and ord(value) < 128 else None


def key(node):
    if isinstance(node, c_ast.ID):
        return ('id', node.name)
    if isinstance(node, c_ast.StructRef):
        base = key(node.name)
        if base is not None:
            return (node.type, base, node.field.name)
    if isinstance(node, c_ast.ArrayRef):
        base, index = key(node.name), integer(node.subscript)
        if base is not None and index is not None:
            return ('array', base, index)
    return None


def analyze(text, native_enum):
    values = collections.defaultdict(list)
    for name, value in native_enum['enumerators']:
        if not name.startswith('CHOICE_') or not 0 <= value <= 127:
            return dict(uses=[], reason='non-dialog enum domain')
        values[value].append(name)
    names = {value: names[0] for value, names in values.items() if len(names) == 1}
    cfg = inspect(text, cfg_only=True)
    fn, offset, nodes, edges = cfg['function'], cfg['offset'], cfg['nodes'], cfg['edges']
    all_nodes = list(children(fn.body))
    dialogs = [n for n in all_nodes if isinstance(n, c_ast.FuncCall) and isinstance(n.name, c_ast.ID) and n.name.name == 'dialog_impl']
    if len(dialogs) != 1:
        return dict(uses=[], reason='primary dialog is not unique')
    primary = dialogs[0]
    escaped = {n.expr.name for n in all_nodes if isinstance(n, c_ast.UnaryOp) and n.op == '&' and isinstance(n.expr, c_ast.ID)}

    # Only pointers whose every source assignment identifies this card qualify.
    pointer_writes = collections.defaultdict(list)
    for n in all_nodes:
        if isinstance(n, c_ast.Assignment) and isinstance(n.lvalue, c_ast.ID):
            pointer_writes[n.lvalue.name].append(n.rvalue if n.op == '=' else None)
        elif isinstance(n, c_ast.Decl) and n.name and n.init is not None:
            pointer_writes[n.name].append(n.init)
    def owner(value):
        return (isinstance(value, c_ast.FuncCall) and isinstance(value.name, c_ast.ID) and
                value.name.name in ['get_card_instance', 'in_play'] and value.args is not None and
                len(value.args.exprs) == 2 and all(isinstance(a, c_ast.ID) and a.name == expected
                    for a, expected in zip(value.args.exprs, ['player', 'card'])))
    parameters = {p.name for p in fn.decl.type.args.params if isinstance(p, c_ast.Decl)}
    owners = {name for name, writes in pointer_writes.items() if name not in escaped | parameters and writes and all(owner(w) for w in writes)}
    storage = set()
    args = primary.args.exprs if primary.args else []
    no_storage = False
    explicit_storage = False
    for i, arg in enumerate(args[3:], 3):
        if not isinstance(arg, c_ast.Constant) or arg.type != 'string':
            continue
        try:
            directive = ast.literal_eval(arg.value)
        except (ValueError, SyntaxError):
            continue
        if directive == '\x01\x0c':
            no_storage = True
        elif directive == '\x01\x13':
            explicit_storage = True
            if i + 1 < len(args) and isinstance(args[i + 1], c_ast.UnaryOp) and args[i + 1].op == '&':
                destination = args[i + 1].expr
                if isinstance(destination, c_ast.StructRef) and isinstance(destination.name, c_ast.ID) and destination.name.name in owners:
                    storage.add(key(destination))
    if not no_storage and not explicit_storage:
        storage.update(('->', ('id', name), 'info_slot') for name in owners)

    # Integer zero resets and copies of a direct dialog-result local retain a
    # mode role. Count arithmetic, X costs and unrelated writes disqualify it.
    direct_results = {n.lvalue.name for n in all_nodes if isinstance(n, c_ast.Assignment) and
                      n.op == '=' and n.rvalue is primary and isinstance(n.lvalue, c_ast.ID)}
    direct_results.update(n.name for n in all_nodes if isinstance(n, c_ast.Decl) and n.init is primary)
    direct_results = {name for name in direct_results if all(
        value is primary or key(value) in storage or integer(value) == 0
        for value in pointer_writes[name])}
    for n in all_nodes:
        if isinstance(n, c_ast.Assignment) and key(n.lvalue) in storage:
            rhs = n.rvalue
            if not (n.op == '=' and (integer(rhs) == 0 or rhs is primary or
                    isinstance(rhs, c_ast.ID) and rhs.name in direct_results)):
                storage.discard(key(n.lvalue))
        elif isinstance(n, c_ast.UnaryOp) and n.op in ['p++', 'p--', '++', '--']:
            storage.discard(key(n.expr))

    uses = {}
    def candidate(node, record, reason):
        value = integer(node)
        if not record or value not in names:
            return
        at = offset(node)
        assert text[at:at + len(node.value)] == node.value
        uses[at] = dict(offset=at, literal=node.value, name=names[value], value=value,
                        native_enum_die=native_enum['native_enum_die'], reason=reason)

    def expression(node, state, record):
        if node is None:
            return False
        if isinstance(node, c_ast.ID):
            return node.name not in escaped and key(node) in state
        if isinstance(node, (c_ast.StructRef, c_ast.ArrayRef)):
            return key(node) in storage and key(node) in state
        if isinstance(node, c_ast.Cast):
            type_names = {name for n in children(node.to_type) if isinstance(n, c_ast.IdentifierType) for name in n.names}
            if isinstance(node.to_type.type, c_ast.PtrDecl) or type_names & {'bool', '_Bool'}:
                expression(node.expr, state, record)
                return False
            return expression(node.expr, state, record)
        if isinstance(node, c_ast.Assignment):
            choice = expression(node.rvalue, state, record)
            destination = key(node.lvalue)
            if destination is not None:
                if node.op == '=' and (choice or destination in storage and integer(node.rvalue) == 0) and not (destination[0] == 'id' and destination[1] in escaped):
                    state.add(destination)
                else:
                    state.discard(destination)
            return choice and node.op == '='
        if isinstance(node, c_ast.Decl):
            destination = ('id', node.name)
            if node.init is not None:
                choice = expression(node.init, state, record)
                if choice and node.name not in escaped:
                    state.add(destination)
                else:
                    state.discard(destination)
            else:
                state.discard(destination)
            return False
        if isinstance(node, c_ast.ExprList):
            choice = False
            for expr in node.exprs:
                choice = expression(expr, state, record)
            return choice
        if isinstance(node, c_ast.FuncCall):
            # Do not infer an argument evaluation order or writable-alias purity.
            argument_state = set(state)
            for n in children(node.args):
                if isinstance(n, c_ast.Assignment):
                    argument_state.discard(key(n.lvalue))
                elif isinstance(n, c_ast.UnaryOp) and n.op in ['p++', 'p--', '++', '--']:
                    argument_state.discard(key(n.expr))
            for arg in node.args.exprs if node.args else []:
                # Function-like macros can stringify their arguments. Keep
                # argument text unchanged, including ASSERT diagnostics.
                expression(arg, set(argument_state), False)
                for n in children(arg):
                    if isinstance(n, c_ast.Assignment):
                        state.discard(key(n.lvalue))
                    elif isinstance(n, c_ast.UnaryOp) and n.op in ['p++', 'p--', '++', '--']:
                        state.discard(key(n.expr))
            return node is primary
        if isinstance(node, c_ast.BinaryOp):
            left = expression(node.left, state, record)
            if node.op in ['&&', '||']:
                skipped = set(state)
                expression(node.right, state, record)
                state.intersection_update(skipped)
                return False
            right = expression(node.right, state, record)
            if node.op in ['==', '!=', '<', '>', '<=', '>=']:
                if left:
                    candidate(node.right, record, 'comparison with primary dialog choice')
                if right:
                    candidate(node.left, record, 'comparison with primary dialog choice')
            return node.op == '&' and (left and integer(node.right) == 255 or right and integer(node.left) == 255)
        if isinstance(node, c_ast.TernaryOp):
            expression(node.cond, state, record)
            left_state, right_state = set(state), set(state)
            left = expression(node.iftrue, left_state, record)
            right = expression(node.iffalse, right_state, record)
            state.clear()
            state.update(left_state & right_state)
            return left and right
        if isinstance(node, c_ast.UnaryOp):
            expression(node.expr, state, record)
            if node.op in ['p++', 'p--', '++', '--']:
                state.discard(key(node.expr))
            return False
        if isinstance(node, (c_ast.Case, c_ast.Default)):
            return False
        for _, child in node.children():
            expression(child, state, record)
        return False

    states = {cfg['entry']: set(storage)}
    todo = collections.deque([cfg['entry']])
    updates = 0
    while todo:
        at = todo.popleft()
        state = set(states[at])
        expression(nodes[at], state, False)
        for target in edges[at]:
            merged = state if target not in states else states[target] & state
            if states.get(target) != merged:
                states[target] = set(merged)
                todo.append(target)
                updates += 1
                assert updates < 100000
    for at, state in states.items():
        expression(nodes[at], set(state), True)

    def cases(node):
        if isinstance(node, c_ast.Switch):
            return
        if isinstance(node, c_ast.Case):
            yield node.expr
        for _, child in node.children():
            yield from cases(child)
    condition_nodes = {id(node): at for at, node in nodes.items()}
    for switch in (n for n in all_nodes if isinstance(n, c_ast.Switch)):
        at = condition_nodes.get(id(switch.cond))
        if at in states and expression(switch.cond, set(states[at]), False):
            for case in cases(switch.stmt):
                candidate(case, True, 'switch on primary dialog choice')
    return dict(uses=sorted(uses.values(), key=lambda u: u['offset']), owner_pointers=sorted(owners),
                choice_storage=sorted(storage, key=str), reason='unique primary dialog and conservative reaching choice values')


def project(text, native_enum, uses):
    """Replace only proven equal, plain C-int literal tokens in this owner."""
    domains = native_enum if isinstance(native_enum, list) else [native_enum]
    code = mask(text)
    by_die = {}
    for domain in domains:
        assert domain['size'] == 4 and domain['tag_name'] is None and not domain['scoped']
        values = domain['enumerators']
        assert values and all(re.fullmatch(r'[A-Za-z_]\w*', name) and isinstance(value, int) and 0 <= value <= 0x7fffffff for name, value in values)
        expected = 'enum {' + ','.join(name + '=' + str(value) for name, value in values) + '}'
        assert re.sub(r'\s+', '', expected) in re.sub(r'\s+', '', code)
        assert domain['native_enum_die'] not in by_die
        by_die[domain['native_enum_die']] = dict(values)
    # mask() blanks character constants. Lex strings and comments together so a
    # quoted fragment inside either cannot be mistaken for a character token.
    quoted = re.compile(r'''/\*[\s\S]*?\*/|//[^\n]*|"(?:\\[\s\S]|[^"\\])*"|'(?:\\[\s\S]|[^'\\])*' ''', re.X)
    chars = {m.start(): m.group() for m in quoted.finditer(text) if m.group().startswith("'")}
    assert len(uses) == len({u['offset'] for u in uses}) and uses
    end = len(text)
    for use in sorted(uses, key=lambda u: u['offset'], reverse=True):
        at, literal, name = use['offset'], use['literal'], use['name']
        value = literal_int(literal)
        assert value is not None and value == by_die[use['native_enum_die']][name] == use['value']
        assert at + len(literal) <= end and text[at:at + len(literal)] == literal
        assert code[at:at + len(literal)] == literal or chars.get(at) == literal
        assert (at == 0 or not re.match(r'\w', code[at - 1])) and (at + len(literal) == len(code) or not re.match(r'\w', code[at + len(literal)]))
        text = text[:at] + name + text[at + len(literal):]
        end = at
    return text
