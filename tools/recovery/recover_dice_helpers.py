"""Reconstruct the VLA dice worker and its verified random/sort dependencies."""
import argparse
import csv
import hashlib
import json
import re
import shutil
import subprocess
from pathlib import Path

import capstone
from inspect_release import Image, value, inherited_value
from export_abi import type_of, type_name
from promote_recovery import clean
from recover_literals import readonly_string, byte_literal
from audit_source import literal_tokens, function_body


DICE_BODY = r'''int roll_xdy(int player, int src_player, int src_card, int tgt_player,
             int tgt_card, int x, int y, int *natural_result, int plus,
             int *first2, RollXDY show_math)
{
  char buf[1500];
  char *prompt = buf;
  int pos = 0;
  if (x < 1)
    backtrace_and_abort("functions/dice.c:19\nx >= 1 && y >= 1");
  if (ai_is_speculating != 0)
    prompt = "r";
  int adj_num_to_roll = (int)((unsigned int)x + (unsigned int)roll_extra_dice[player]);
  if (adj_num_to_roll < 1)
    backtrace_and_abort("functions/dice.c:26\nadj_num_to_roll >= 1");
  int rolls[adj_num_to_roll];
  if (adj_num_to_roll == 1)
  {
    int result = network_rand(player, y) + 1;
    rolls[0] = result;
    if (ai_is_speculating != 1)
    {
      const char *suffix;
      const char *subject;
      if (player == 0)
      {
        suffix = "";
        subject = "You";
      }
      else
      {
        suffix = "s";
        subject = opponent_name;
      }
      pos = sprintf(buf, "%s roll%s a d%d...\n%d", subject, suffix, y, result);
    }
  }
  else
  {
    if (ai_is_speculating != 1)
    {
      const char *suffix;
      const char *subject;
      if (player == 0)
      {
        suffix = "";
        subject = "You";
      }
      else
      {
        suffix = "s";
        subject = opponent_name;
      }
      pos = sprintf(buf, "%s roll%s %dd%d...\n", subject, suffix, adj_num_to_roll, y);
    }
    for (int i = 0; i < adj_num_to_roll; ++i)
    {
      rolls[i] = network_rand(player, y) + 1;
      if (ai_is_speculating != 1)
      {
        const char *separator;
        if (i == 0)
          separator = "";
        else
          separator = " ";
        pos += sprintf(buf + pos, "%s%d", separator, rolls[i]);
      }
    }
    if (roll_extra_dice[player] > 0)
    {
      descending_qsort(adj_num_to_roll, rolls);
      /* These two headings are formatted even during AI speculation. */
      if (x == 1)
        pos += sprintf(buf + pos, "\nignoring all but the highest...\n%d", rolls[0]);
      else
      {
        pos += sprintf(buf + pos, "\nignoring all but the highest %d...\n", x);
        if (ai_is_speculating != 1 && x > 0)
        {
          const char *separator = "";
          pos += sprintf(buf + pos, "%s%d", separator, rolls[0]);
          separator = " ";
          for (int i = 1; i < x; ++i)
            pos += sprintf(buf + pos, "%s%d", separator, rolls[i]);
        }
      }
    }
  }
  if (natural_result != NULL)
    *natural_result = rolls[0];
  int adjustment = (int)((unsigned int)plus + (unsigned int)add_to_die_rolls[player]);
  if (adjustment != 0)
  {
    if (ai_is_speculating != 1)
    {
      const char *each = " to each";
      if (x < 2)
        each = "";
      const char *verb;
      const char *suffix;
      const char *subject;
      if (player == 0)
      {
        if (adjustment < 1)
        {
          verb = "subtract";
          suffix = "";
          subject = "You";
        }
        else
        {
          verb = "add";
          suffix = "";
          subject = "You";
        }
      }
      else if (adjustment < 1)
      {
        verb = "subtract";
        suffix = "s";
        subject = opponent_name;
      }
      else
      {
        verb = "add";
        suffix = "s";
        subject = opponent_name;
      }
      unsigned int magnitude = adjustment < 0 ? 0U - (unsigned int)adjustment : (unsigned int)adjustment;
      pos += sprintf(buf + pos, "\n%s %s%s %d%s...\n", subject, verb, suffix, (int)magnitude, each);
    }
    for (int i = 0; i < x; ++i)
    {
      rolls[i] = (int)((unsigned int)rolls[i] + (unsigned int)adjustment);
      if (ai_is_speculating != 1)
      {
        const char *separator;
        if (i == 0)
          separator = "";
        else
          separator = " ";
        pos += sprintf(buf + pos, "%s%d", separator, rolls[i]);
      }
    }
  }
  unsigned int total = 0;
  for (int i = 0; i < x; ++i)
    total += (unsigned int)rolls[i];
  if (x >= 2 && ai_is_speculating != 1)
  {
    if (show_math == RXDY_SHOW_TOTAL)
      pos += sprintf(buf + pos, "\ntotalling %d.", (int)total);
    else if (show_math == RXDY_SHOW_DIFFERENCE)
    {
      int difference = (int)((unsigned int)rolls[0] - (unsigned int)rolls[1]);
      unsigned int magnitude = difference < 0 ? 0U - (unsigned int)difference : (unsigned int)difference;
      pos += sprintf(buf + pos, "\nThe difference is %d.", (int)magnitude);
    }
  }
  if (ai_is_speculating != 1)
  {
    play_sound_effect(0x58);
    buf[pos] = '\n';
    buf[pos + 1] = '\0';
    EXE_FN(int, 0x471b60, int, int, int, int, const char *, int)(
        src_player, src_card, tgt_player, tgt_card, prompt, player == 0);
  }
  if (first2 != NULL)
  {
    first2[0] = rolls[0];
    first2[1] = x > 1 ? rolls[1] : 0;
  }
  return (int)total;
}
'''


def worker_local_evidence(function):
    result={}
    def walk(die):
        for child in die.iter_children():
            name=inherited_value(child,'DW_AT_name')
            if child.tag=='DW_TAG_variable' and name in {'buf','prompt','pos','adj_num_to_roll','rolls','tot'}:
                typ=type_of(child)
                entry=dict(die=hex(child.offset),type_die=hex(typ.offset))
                if name=='rolls':
                    assert typ.tag=='DW_TAG_array_type' and type_name(type_of(typ))=='int'
                    dimension=next(typ.iter_children());bound=dimension.attributes['DW_AT_upper_bound']
                    assert bound.form=='DW_FORM_ref4'
                    dynamic=dimension.get_DIE_from_attribute('DW_AT_upper_bound')
                    entry.update(type='int[dynamic upper bound + 1]',bound_reference=hex(dynamic.offset),
                        bound_location=value(dynamic,'DW_AT_location'),
                        note='The upper bound is a DIE reference, not the numeric array extent printed by the ABI type exporter.')
                else:
                    entry['type']=type_name(typ)
                location=child.attributes.get('DW_AT_location')
                if location:entry['location']=dict(form=location.form,value=location.value)
                result[name]=entry
            if child.has_children:walk(child)
    walk(function)
    assert set(result)=={'buf','prompt','pos','adj_num_to_roll','rolls','tot'} and result['buf']['type']=='char[1500]'
    return result


def main():
    parser = argparse.ArgumentParser(__doc__)
    parser.add_argument('--apply', action='store_true')
    args = parser.parse_args()
    root = Path(__file__).resolve().parents[2];cache = root/'.recovery'
    debug = Image(Path('G:/Manalink/ManalinkEh_10_22_Hotfix.dbg').read_bytes())
    runtime = Image((cache/'2022-ManalinkEh.dll').read_bytes())
    log = (cache/'dice-console.log').read_text()
    assert 'ERROR' not in log and 'RECOVERY_EXPORT_COMPLETE success=17 failed=0' in log
    assert 'RECOVERY_DICE_INTERFACES_COMPLETE count=2' in log
    rows = {r['address']:r for r in csv.DictReader((cache/'decompiled-dice/export.tsv').open(),delimiter='\t')}
    selected = ['2650020','26b2200','2650b10','2650b30','2652ab0','2652ae0']
    proof = dict(functions={}, native_interfaces=[], enum=None)
    for cu in debug.dwarf().iter_CUs():
        source = value(cu.get_top_DIE(),'DW_AT_name')
        if source not in {'functions/dice.c','functions/ai.c','functions/tutors.c'}:
            continue
        for die in cu.get_top_DIE().iter_children():
            name = inherited_value(die,'DW_AT_name');at=value(die,'DW_AT_low_pc')
            if name == 'RollXDY' and source == 'functions/dice.c':
                typ=type_of(die)
                assert value(typ,'DW_AT_byte_size')==4 and value(typ,'DW_AT_encoding')==7
                values={value(c,'DW_AT_name'):value(c,'DW_AT_const_value') for c in typ.iter_children()}
                assert values==dict(RXDY_NONE=0,RXDY_SHOW_TOTAL=1,RXDY_SHOW_DIFFERENCE=2)
                proof['enum']=dict(bytes=4,encoding='unsigned',values=values,die=hex(typ.offset))
            if at and f'{at:x}' in selected:
                key=f'{at:x}';assert name==rows[key]['name'] and source==rows[key]['source']
                proof['functions'][key]=dict(name=name,source=source,line=str(inherited_value(die,'DW_AT_decl_line')),
                    end=hex(at+value(die,'DW_AT_high_pc')),parameters=[dict(name=inherited_value(p,'DW_AT_name'),
                    type=type_name(type_of(p))) for p in die.iter_children() if p.tag=='DW_TAG_formal_parameter'])
                if key in ['2650b10','2650b30']:
                    assert all(type_of(type_of(p)).tag=='DW_TAG_const_type' for p in die.iter_children() if p.tag=='DW_TAG_formal_parameter')
                if key=='2650020':proof['worker_locals']=worker_local_evidence(die)
        cu._dielist.clear();cu._diemap.clear()
    assert len(proof['functions'])==6 and proof['enum']
    raw={at:(cache/'decompiled-dice'/(at+'.c')).read_text() for at in selected}
    decoder=capstone.Cs(capstone.CS_ARCH_X86,capstone.CS_MODE_32);decoder.detail=True
    end=int(proof['functions']['2650020']['end'],16)
    instructions=list(decoder.disasm(runtime.pe.get_data(0x650020,end-0x2650020),0x2650020))
    constants=[];expected=raw['2650020']
    for symbol in sorted(set(re.findall(r'&UNK_([0-9a-f]{8})',expected))):
        address=int(symbol,16);literal=readonly_string(runtime,address);assert literal is not None
        sites=[dict(address=f'{i.address:x}',bytes=i.bytes.hex()) for i in instructions
            if i.mnemonic=='mov' and len(i.operands)==2 and i.operands[1].type==capstone.x86.X86_OP_IMM and i.operands[1].imm==address]
        assert sites
        token='&UNK_'+symbol
        constants.append(dict(symbol='UNK_'+symbol,address=hex(address),bytes=literal.hex(),
            raw_occurrences=expected.count(token),function_end=hex(end),materializations=sites))
        expected=expected.replace(token,byte_literal(literal))
    if literal_tokens(expected)!=literal_tokens(DICE_BODY):
        (cache/'dice-literal-comparison.json').write_text(json.dumps(dict(expected=literal_tokens(expected),actual=literal_tokens(DICE_BODY)),indent=2))
        raise RuntimeError('Manual dice literal order differs; see comparison')
    # Every call and outgoing argument setup remains available for review of
    # the restored VLA calls. Keep exact machine bytes, not invented traces.
    proof['worker_machine_sha256']=hashlib.sha256(runtime.pe.get_data(0x650020,end-0x2650020)).hexdigest()
    allocation=runtime.pe.get_data(0x650072,45)
    assert allocation.hex()=='8b0c859871790201cf85ff8d04bd000000000f8e2e040000e841c10a0029c48d74241f89f383e6fcc1eb0283ff'
    proof['vla_allocation']=dict(address='2650072',bytes=allocation.hex(),
        interpretation='Add roll_extra_dice[player] to x, reject nonpositive count, probe count*4 bytes, subtract from ESP, align the int array pointer.')
    proof['worker_calls']=[dict(address=f'{i.address:x}',bytes=i.bytes.hex(),target=i.op_str,
        preceding=[dict(address=f'{p.address:x}',bytes=p.bytes.hex(),instruction=p.mnemonic+' '+p.op_str)
                   for p in instructions[max(0,n-10):n]]) for n,i in enumerate(instructions) if i.mnemonic=='call']
    bodies={'2650020':'static '+DICE_BODY}
    for at in selected[1:]:
        body=clean(raw[at])
        if at=='26b2200':
            body=body.replace('engine_network_rand_raw_2022(player,upperbound)',
                'EXE_FN(int, 0x44e070, int, int)(player,upperbound)')
        if at in ['2650b10','2650b30']:
            body=body.replace('void *lhs','const void *lhs').replace('void *rhs','const void *rhs')
            body=body.replace('*(int *)rhs','*(const int *)rhs').replace('*(int *)lhs','*(const int *)lhs')
        private=any(n=='_'+rows[at]['name'] and storage==3 for n,off,s,k,storage in debug.functions())
        if private:body='static '+body
        bodies[at]=body
    import pefile
    runtime.pe.parse_data_directories(directories=[pefile.DIRECTORY_ENTRY['IMAGE_DIRECTORY_ENTRY_IMPORT']])
    imports={i.address:(e.dll.decode().lower(),i.name.decode() if i.name else '') for e in runtime.pe.DIRECTORY_ENTRY_IMPORT for i in e.imports}
    assert imports[0x27b54e4]==('msvcrt.dll','qsort')
    proof['qsort_import']=dict(address='2701e00',bytes='ff25e4547b02',slot='27b54e4',library='msvcrt.dll',symbol='qsort')
    magic_bytes=(cache/'2022-Magic.exe').read_bytes();assert hashlib.sha256(magic_bytes).hexdigest()=='0170f02b1c381a20a97ca0c12e99c8d338453ce4edbfc99148e726d565bee0de'
    magic=Image(magic_bytes)
    rng=magic.pe.get_data(0x44e070-magic.pe.OPTIONAL_HEADER.ImageBase,90)
    assert rng[:3]==bytes.fromhex('558bec')
    assert rng[13:17].hex()=='837d0c01' and rng[-7:].hex()=='a1108c6200c9c3'
    proof['native_interfaces']=[dict(address='44e070',signature='int(int player, int upperbound)',bytes=rng.hex(),
        evidence='EBP+8 selects player; EBP+12 bounds signed division; returns recorded result in EAX.'),
        dict(address='471b60',signature='int(int, int, int, int, const char *, int)',
        source='functions/events.c existing EXE_FN call and Magic-trace.c signature; address matches absolute COFF.')]
    scope=('Typed VLA of adjusted dice count, restoring outgoing argument slots from machine call sites. '
        'Preserves x-only entry assertion, highest-dice ordering, pre-modifier natural result, '
        'AI prompt/logging predicates, unconditional highest-dice headings and x86 word arithmetic. '
        'Native RNG/dialog bindings and CRT qsort verified independently; no gameplay validation.')
    additions={};entries=[]
    manifest_path=root/'docs/recovery-evidence/recovered-functions.json';manifest=json.loads(manifest_path.read_text())
    previous={f['address']:f for f in manifest['accepted_functions']}
    assert set(selected)&set(previous)=={'2650b10','2650b30'}
    corrections={}
    for at in selected:
        body=subprocess.run(['C:/Program Files/LLVM/bin/clang-format.exe',
            '-style={BasedOnStyle: LLVM, IndentWidth: 2, BreakBeforeBraces: Allman, ColumnLimit: 100}'],
            input=bodies[at].encode(),capture_output=True,check=True).stdout.decode().replace('\r\n','\n')
        meta=proof['functions'][at];name=meta['name'];source=meta['source']
        entry=dict(name=name,address=at,source=source,line=meta['line'],static=body.startswith('static '),
            prototype=body[:body.index('{')].strip()+';',raw_sha256=hashlib.sha256(raw[at].encode()).hexdigest(),
            recovered_literals=[],reconstruction=scope,reconstruction_body_sha256=hashlib.sha256(function_body(body,name).encode()).hexdigest())
        if at=='2650020':entry['reconstructed_literal_constants']=constants
        if at in previous:
            corrections[name]=body
            previous[at].update(prototype=entry['prototype'],reconstruction='Restore DWARF const void * comparator parameters for the CRT qsort contract; body arithmetic unchanged.',
                reconstruction_body_sha256=entry['reconstruction_body_sha256'])
            continue
        marker=f'/* ManalinkEh 2022: 0x{at}; original {source}:{meta["line"]}. */'
        present=(root/source).read_text() if (root/source).exists() else ''
        if marker in present:
            # Resume an interrupted apply only when the exact proposed body is
            # already present. Never append a duplicate or replace other work.
            assert hashlib.sha256(function_body(present,name).encode()).hexdigest()==entry['reconstruction_body_sha256']
            additions.setdefault(source,'')
        else:
            additions[source]=additions.get(source,'')+'\n'+marker+'\n'+body
        entries.append(entry)
    enum='typedef enum { RXDY_NONE = 0, RXDY_SHOW_TOTAL = 1, RXDY_SHOW_DIFFERENCE = 2 } RollXDY;'
    header_path=root/'manalink.h';header=header_path.read_bytes()
    at=header.rfind(b'#ifdef __cplusplus')
    declaration='/* October 2022 dice interfaces; modifier caches reset during recalculation. */\n'
    declaration+='extern int add_to_die_rolls[2];\nextern int roll_extra_dice[2];\n'
    declaration+='\n'.join(e['prototype'] for e in entries if not e['static'])+'\n\n'
    block=declaration.replace('\n','\r\n').encode()
    if block in header:
        assert header.count(block)==1
    else:
        header=header[:at]+block+header[at:]
    candidate=cache/'dice-check';candidate.mkdir(exist_ok=True);(candidate/'manalink.h').write_bytes(header)
    # Compile all proposed complete units before adding them to the tree.
    content={s:((root/s).read_bytes().decode('latin1').replace('\r\n','\n') if (root/s).exists() else '#include "manalink.h"\n#include <stdio.h>\n#include <stdlib.h>\n')+v for s,v in additions.items()}
    if enum not in content['functions/dice.c']:
        content['functions/dice.c']=content['functions/dice.c'].replace('#include <stdlib.h>\n',
            '#include <stdlib.h>\n\n'+enum+'\n_Static_assert(sizeof(RollXDY) == 4, "dice selector width");\n',1)
    for entry in entries:
        symbol=[s for s in debug.functions() if s[0]=='_'+entry['name']]
        assert len(symbol)==1 and symbol[0][4]==(3 if entry['static'] else 2)
        proof['functions'][entry['address']]['coff_storage_class']=symbol[0][4]
    for name,body in corrections.items():
        old_body=function_body(content['functions/tutors.c'],name)
        assert content['functions/tutors.c'].count(old_body)==1
        content['functions/tutors.c']=content['functions/tutors.c'].replace(old_body,function_body(body,name),1)
    diagnostics={}
    for source,text in content.items():
        path=candidate/Path(source).name;path.write_text(text)
        result=subprocess.run(['C:/Program Files/LLVM/bin/clang.exe','-target','i686-pc-windows-msvc','-std=gnu99',
            '-fsyntax-only','-Werror=implicit-function-declaration','-Werror=incompatible-pointer-types','-Werror=return-type',
            '-Werror=uninitialized','-Werror=sometimes-uninitialized','-Werror=format','-I',str(candidate),'-I',str(root),str(path)],capture_output=True,text=True)
        diagnostics[source]=result.stderr
        if result.returncode:
            (cache/'dice-source-check.json').write_text(json.dumps(diagnostics,indent=2));raise RuntimeError(result.stderr)
    (cache/'dice-source-check.json').write_text(json.dumps(diagnostics,indent=2))
    if args.apply:
        header_path.write_bytes(header)
        for source,text in content.items():(root/source).write_bytes(text.replace('\n','\r\n').encode('latin1'))
        manifest['accepted_functions'].extend(entries);manifest['accepted']=len(manifest['accepted_functions'])
        for source in additions:manifest['source_files'][source]=manifest['source_files'].get(source,0)+sum(e['source']==source for e in entries)
        manifest_path.write_text(json.dumps(manifest,indent=2))
        path=root/'docs/recovery-evidence/interface-recovery.json';interfaces=json.loads(path.read_text());interfaces['dice_helpers']=dict(scope=scope,**proof);path.write_text(json.dumps(interfaces,indent=2))
        rows_old=list(csv.DictReader((cache/'decompiled-abi/export.tsv').open(),delimiter='\t'));by_address={r['address']:r for r in rows_old}
        accepted=set(previous)
        for at,row in rows.items():
            if at in accepted:
                assert at in {'2650b10','2650b30'}
                continue
            shutil.copyfile(cache/'decompiled-dice'/(at+'.c'),cache/'decompiled-abi'/(at+'.c'));by_address[at]=row
        with (cache/'decompiled-abi/export.tsv').open('w',newline='') as stream:
            writer=csv.DictWriter(stream,fieldnames=rows_old[0],delimiter='\t');writer.writeheader();writer.writerows(by_address.values())
    print(json.dumps(dict(applied=args.apply,helpers=len(entries),comparator_qualifier_corrections=len(corrections),
        literal_constants=sum(e['raw_occurrences'] for e in constants)),indent=2))


if __name__=='__main__':
    main()
