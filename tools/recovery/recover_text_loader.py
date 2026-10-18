"""Recover the resource cache and fatal formatter using DWARF and x86 evidence."""
import argparse
import collections
import csv
import hashlib
import json
import re
import shutil
import subprocess
from pathlib import Path

from inspect_release import Image, value, inherited_value
from export_abi import type_of
from promote_recovery import clean
from audit_source import function_body, literal_tokens

ENTRIES = [('2675d70', 'load_text_stash', 'functions/load_text.c', True),
           ('2676360', 'load_text_nocopy', 'functions/load_text.c', False),
           ('2676450', 'load_text_nocopy_from_file', 'functions/load_text.c', False),
           ('26765b0', 'load_text', 'functions/load_text.c', False),
           ('26c11d0', 'fatal', 'functions/show_backtrace.c', False)]


def chain(die):
    result = []
    seen = set()
    while die is not None:
        assert die.offset not in seen
        seen.add(die.offset)
        result.append(dict(offset=hex(die.offset), tag=die.tag,
                           name=value(die, 'DW_AT_name'), size=value(die, 'DW_AT_byte_size')))
        die = type_of(die)
    return result


def main():
    parser = argparse.ArgumentParser(__doc__)
    parser.add_argument('--apply', action='store_true')
    args = parser.parse_args()
    root = Path(__file__).resolve().parents[2]
    cache = root / '.recovery'
    debug = Image(Path('G:/Manalink/ManalinkEh_10_22_Hotfix.dbg').read_bytes())
    runtime = Image((cache / '2022-ManalinkEh.dll').read_bytes())
    evidence = dict(functions={}, globals={}, layout={})
    for cu in debug.dwarf().iter_CUs():
        source = value(cu.get_top_DIE(), 'DW_AT_name')
        if source not in {e[2] for e in ENTRIES}:
            continue
        for die in cu.get_top_DIE().iter_children():
            name = inherited_value(die, 'DW_AT_name')
            at = value(die, 'DW_AT_low_pc')
            if die.tag == 'DW_TAG_typedef' and name == 'LoadText':
                record = type_of(die)
                assert value(record, 'DW_AT_byte_size') == 12
                evidence['layout'] = dict(size=12, members={value(c, 'DW_AT_name'): dict(
                    offset=value(c, 'DW_AT_data_member_location'), type=chain(type_of(c)))
                    for c in record.iter_children() if c.tag == 'DW_TAG_member'})
            entry = next((e for e in ENTRIES if int(e[0], 16) == at), None)
            if entry:
                assert name == entry[1] and source == entry[2]
                evidence['functions'][entry[0]] = dict(name=name, source=source,
                    line=str(inherited_value(die, 'DW_AT_decl_line')),
                    end=hex(at + value(die, 'DW_AT_high_pc')),
                    return_type=chain(type_of(die)), parameters=[dict(name=inherited_value(p, 'DW_AT_name'),
                    type=chain(type_of(p))) for p in die.iter_children() if p.tag == 'DW_TAG_formal_parameter'])
        cu._dielist.clear()
        cu._diemap.clear()
    assert len(evidence['functions']) == 5
    assert {k:v['offset'] for k,v in evidence['layout']['members'].items()} == dict(key=0, num=4, lines=8)
    assert [x['tag'] for x in evidence['layout']['members']['key']['type']] == [
        'DW_TAG_pointer_type', 'DW_TAG_const_type', 'DW_TAG_base_type']
    assert [x['tag'] for x in evidence['layout']['members']['lines']['type']] == [
        'DW_TAG_pointer_type', 'DW_TAG_pointer_type', 'DW_TAG_const_type', 'DW_TAG_base_type']
    for at, name, source, private in ENTRIES:
        assert any(n == '_' + name and storage == (3 if private else 2) and
            debug.pe.OPTIONAL_HEADER.ImageBase + debug.pe.sections[s-1].VirtualAddress + off == int(at, 16)
            for n,off,s,kind,storage in debug.functions())
        for parameter in evidence['functions'][at]['parameters']:
            if parameter['name'] in {'filename', 'header', 'ignored', 'fmt'}:
                assert [t['tag'] for t in parameter['type']] == [
                    'DW_TAG_pointer_type', 'DW_TAG_const_type', 'DW_TAG_base_type']
    sites = {
        '2675f73': 'c7038df77502', '2675fa0': 'c7008df77502',
        '2676006': '80cc0c', '2676016': 'd96c2438db5c242cd96c243a',
        '26761d6': '80cc0c', '26761de': 'd96c2438db5c2434d96c243a',
        '275f80c': '0000c03f',
        '2676336': '8d4514', '2676353': 'e8b86d0800',
        '26765ee': '8944240c', '2676603': 'e828fdffff',
        '26764f0': 'a1485477028b55ec8d04408b4482fc',
        '26c11da': '8d450c', '26c11f9': 'e812bf0300', '26c1201': 'e8aaffffff',
    }
    for at, data in sites.items():
        assert runtime.pe.get_data(int(at, 16) - runtime.pe.OPTIONAL_HEADER.ImageBase, len(bytes.fromhex(data))).hex() == data
    evidence['machine_sites'] = sites
    magic_data = (cache / '2022-Magic.exe').read_bytes()
    assert hashlib.sha256(magic_data).hexdigest() == '0170f02b1c381a20a97ca0c12e99c8d338453ce4edbfc99148e726d565bee0de'
    magic = Image(magic_data)
    path_body = magic.pe.get_data(0x466020 - magic.pe.OPTIONAL_HEADER.ImageBase, 38)
    assert path_body.hex() == '558bec68050100008b4508506a00e8a1fd06006a5c8b450850e82cfe060083c408c60000c9c3'
    evidence['native_path_helper'] = dict(address='466020', argument='char *path', return_type='void',
        bytes=path_body.hex(), executable_sha256=hashlib.sha256(magic_data).hexdigest(),
        scope='One cdecl buffer argument; GetModuleFileNameA then strrchr backslash and terminating zero.')
    evidence['crt_imports'] = json.loads((cache / 'runtime-interfaces.json').read_text())
    for log, markers in [('text-core-console.log', ['RECOVERY_RUNTIME_COMPLETE',
        'RECOVERY_TEXT_INTERFACES_COMPLETE count=2', 'RECOVERY_EXPORT_COMPLETE success=5 failed=0']),
        ('text-locals-console.log', ['RECOVERY_TEXT_LOCALS_COMPLETE count=1', 'RECOVERY_EXPORT_COMPLETE success=5 failed=0'])]:
        data = (cache / log).read_text()
        assert 'ERROR' not in data and all(m in data for m in markers)
    manifest_path = root / 'docs/recovery-evidence/recovered-functions.json'
    manifest = json.loads(manifest_path.read_text())
    assert not {e[0] for e in ENTRIES} & {f['address'] for f in manifest['accepted_functions']}
    raw_dir = cache / 'decompiled-text-locals'
    raw = {at:(raw_dir / (at + '.c')).read_text() for at,_,_,_ in ENTRIES}
    bodies = {at:clean(text.replace('&UNK_0275f78d', '""')) for at,text in raw.items()}
    body = bodies['2675d70']
    start = body.index('  engine_get_path_base_2022(')
    end = body.index('  if (argument1 ==', start)
    body = body[:start] + '''  EXE_FN(void, 0x466020, char *)(path);
  sprintf(path + strlen(path),"\\\\%s",filename);
  argument1 = fopen(path,"rt");
''' + body[end:]
    body = body.replace('auStack_204', 'path')
    body = re.sub(r'^  (?:unsigned int \*puVar[12]|uint32_t \*puVar(?:6|16)|unsigned int uVar15);\n', '', body, flags=re.M)
    body = body.replace('unsigned int path [125];', 'char path[500];')
    body = body.replace('(int)ROUND((float)iStack_214 * 1.5)', '(int)((double)iStack_214 * 1.5)')
    start = body.index('      puVar16 = ')
    end = body.index('        pLVar7 = malloc(', start)
    body = body[:start] + '''      LoadText *sentinel = (LoadText *)((char *)lt + iVar12);
      sentinel->key = "";
      sentinel->num = 250;
      const char **empty_lines = malloc(250 * sizeof(*empty_lines));
      sentinel->lines = empty_lines;
      if (empty_lines != NULL) {
        const char **end_lines = empty_lines + 250;
        do {
          *empty_lines = "";
          empty_lines = empty_lines + 1;
        } while (empty_lines != end_lines);
''' + body[end:]
    body = re.sub(r'(?m)^(\s+)const char \*(pcVar\d+);', r'\1char *\2;', body)
    body = body.replace('char **ppcVar', 'const char **ppcVar')
    body = body.replace('(char **)0x0', 'NULL')
    body = body.replace('load_text_stash(char *filename', 'load_text_stash(const char *filename')
    body = 'static ' + body
    bodies['2675d70'] = body
    for at in ['2676360', '2676450']:
        body = bodies[at]
        body = re.sub(r'^char \*\*', 'const char **', body)
        body = body.replace('char *filename', 'const char *filename').replace('char *header', 'const char *header')
        bodies[at] = body
    body = bodies['26765b0'].replace('char *ignored', 'const char *ignored').replace('char *header', 'const char *header')
    body = body.replace('char **ppcVar', 'const char **ppcVar')
    # The call stores only the %s argument at ESP+12; num_lines is stale
    # outgoing stack state in Ghidra, unused by this specialized CRT wrapper.
    body = body.replace('snprintf(*pacVar4,300,"%s",*ppcVar1,num_lines)', '_snprintf(*pacVar4,300,"%s",*ppcVar1)')
    body = body.replace('(char (*) [300])0x728078', '(text_lines + 249)')
    bodies['26765b0'] = body
    bodies['26c11d0'] = '''void fatal(const char *fmt, ...)
{
  char buf[2000];
  va_list args;
  va_start(args, fmt);
  _vsnprintf(buf, 2000, fmt, args);
  va_end(args);
  backtrace_and_abort(buf);
}
'''
    for at, body in bodies.items():
        assert literal_tokens(raw[at].replace('&UNK_0275f78d', '""')) == literal_tokens(body), at
        bodies[at] = subprocess.run(['C:/Program Files/LLVM/bin/clang-format.exe',
            '-style={BasedOnStyle: LLVM, IndentWidth: 2, BreakBeforeBraces: Allman, ColumnLimit: 100}'],
            input=body.encode(), capture_output=True, check=True).stdout.decode().replace('\r\n', '\n')
    declarations = [('loadtexts', '277544c', 'static LoadText *loadtexts;'),
                    ('num_loadtexts', '2775448', 'static int num_loadtexts;'),
                    ('last_loadtext_from_file', '2775340', 'static char last_loadtext_from_file[262];'),
                    ('loadtexts_from_file', '2775324', 'static LoadText *loadtexts_from_file;'),
                    ('num_loadtexts_from_file', '2775320', 'static int num_loadtexts_from_file;')]
    for name, at, declaration in declarations:
        match = [(n,off,s,storage) for n,off,s,kind,storage in debug.symbols if n == '_' + name and s > 0 and
            debug.pe.OPTIONAL_HEADER.ImageBase + debug.pe.sections[s-1].VirtualAddress + off == int(at,16)]
        assert len(match) == 1 and match[0][3] == 3
        assert debug.pe.sections[match[0][2]-1].Name.rstrip(b'\0') == b'.bss'
        evidence['globals'][name] = dict(address=at, declaration=declaration, linkage='static', zero_initialized=True)
    scope = ('DWARF-qualified 12-byte LoadText records and five private zero-initialized caches. '
        'Preserves retained file/line buffers, sentinel behavior, main-cache count in file-cache fallback, '
        'and the target 250-row copy cap despite text_lines having 249 rows. Uses verified native path helper '
        'and Microsoft CRT formatting; no lifetime, error-handling or bounds fixes were added.')
    new_source = '''#include "manalink.h"
#include <stdio.h>
#include <stdlib.h>
#include <stddef.h>

typedef struct
{
  const char *key;
  int num;
  const char **lines;
} LoadText;

_Static_assert(sizeof(LoadText) == 12, "LoadText size");
_Static_assert(offsetof(LoadText, key) == 0 && offsetof(LoadText, num) == 4 &&
               offsetof(LoadText, lines) == 8, "LoadText fields");

/* Recovered private caches; retained raw buffers own the borrowed strings. */
''' + '\n'.join(d[2] for d in declarations) + '\n'
    additions = collections.defaultdict(str)
    entries = []
    for at, original_name, source, private in ENTRIES:
        name = 'load_text_2022' if original_name == 'load_text' else original_name
        body = bodies[at]
        marker = f'/* ManalinkEh 2022: 0x{at}; original {source}:{evidence["functions"][at]["line"]}. */'
        comment = ''
        if at == '2676450':
            comment = '/* The released fallback indexes this cache using num_loadtexts from the main cache. */\n'
        if at == '26765b0':
            comment = '/* Preserve the released 250-row cap; the native declaration has only 249 rows. */\n'
        additions[source] += '\n' + marker + '\n' + comment + body
        entry = dict(name=name, original_name=original_name, address=at, source=source,
            line=evidence['functions'][at]['line'], static=private,
            prototype=body[:body.index('{')].strip() + ';', raw_sha256=hashlib.sha256(raw[at].encode()).hexdigest(),
            recovered_literals=[], reconstruction=scope,
            reconstruction_body_sha256=hashlib.sha256(function_body(body,name).encode()).hexdigest())
        if name == 'fatal':
            entry['prototype'] = 'void fatal(const char *fmt, ...) __attribute__((noreturn));'
        if at == '2675d70':
            entry['reconstructed_literal_pointers'] = [dict(symbol='UNK_0275f78d', address='0x275f78d', bytes='',
                function_end=evidence['functions'][at]['end'], materializations=[
                    dict(address='2675f73',bytes=sites['2675f73']), dict(address='2675fa0',bytes=sites['2675fa0'])])]
        if at == '26765b0':
            entry['array_boundaries'] = [dict(original='(char (*) [300])0x728078', replacement='(text_lines + 249)')]
        entries.append(entry)
    assert not (root / 'functions/load_text.c').exists()
    public = '\n'.join(e['prototype'] for e in entries if not e['static'])
    header = (root / 'manalink.h').read_bytes()
    at = header.rfind(b'#ifdef __cplusplus')
    header = header[:at] + ('/* October 2022 resource-cache interfaces. */\n' + public + '\n\n').replace('\n','\r\n').encode() + header[at:]
    # Candidates compile against the exact proposed header, without altering
    # the working source until every interface and body has passed.
    candidate_dir = cache / 'text-loader-candidate'
    candidate_dir.mkdir(exist_ok=True)
    (candidate_dir / 'manalink.h').write_bytes(header)
    candidate_sources = {'functions/load_text.c': new_source + additions['functions/load_text.c'],
        'functions/show_backtrace.c': (root/'functions/show_backtrace.c').read_text().replace(
            '#include <stdlib.h>', '#include <stdlib.h>\n#include <stdarg.h>') + additions['functions/show_backtrace.c']}
    diagnostics = {}
    for source, content in candidate_sources.items():
        path = candidate_dir / Path(source).name
        path.write_text(content)
        result = subprocess.run(['C:/Program Files/LLVM/bin/clang.exe', '-target','i686-pc-windows-msvc',
            '-std=gnu99','-fsyntax-only','-Werror=implicit-function-declaration','-Werror=incompatible-pointer-types',
            '-Werror=return-type','-Werror=uninitialized','-Werror=sometimes-uninitialized',
            '-I',str(candidate_dir),'-I',str(root),str(path)],capture_output=True,text=True)
        diagnostics[source] = result.stderr
        if result.returncode:
            (cache/'text-loader-check.json').write_text(json.dumps(diagnostics,indent=2))
            raise RuntimeError(result.stderr)
    (cache/'text-loader-check.json').write_text(json.dumps(diagnostics,indent=2))
    if args.apply:
        for source, content in candidate_sources.items():
            (root/source).write_bytes(content.replace('\r\n','\n').replace('\n','\r\n').encode())
        (root/'manalink.h').write_bytes(header)
        manifest['accepted_functions'].extend(entries)
        manifest['accepted'] = len(manifest['accepted_functions'])
        manifest['source_files']['functions/load_text.c'] = 4
        manifest['source_files']['functions/show_backtrace.c'] = 1
        for name, at, declaration in declarations:
            manifest.setdefault('source_globals',[]).append(dict(source='functions/load_text.c',name=name,
                target_address=at, declaration=declaration, zero_initialized=True,linkage='static'))
        manifest_path.write_text(json.dumps(manifest,indent=2))
        for at,_,_,_ in ENTRIES:
            shutil.copyfile(raw_dir/(at+'.c'),cache/'decompiled-abi'/(at+'.c'))
        path = cache / 'decompiled-abi/export.tsv'
        rows = list(csv.DictReader(path.open(), delimiter='\t'))
        updated = {r['address']:r for r in csv.DictReader((raw_dir/'export.tsv').open(),delimiter='\t')}
        with path.open('w', newline='') as stream:
            writer = csv.DictWriter(stream, fieldnames=rows[0],delimiter='\t');writer.writeheader()
            writer.writerows(updated.get(r['address'],r) for r in rows)
        evidence['scope'] = scope
        path = root/'docs/recovery-evidence/interface-recovery.json'
        interfaces = json.loads(path.read_text()); interfaces['text_loader'] = evidence
        path.write_text(json.dumps(interfaces,indent=2))
    print(json.dumps(dict(applied=args.apply, helpers=len(entries),private_globals=len(declarations)),indent=2))


if __name__ == '__main__':
    main()
