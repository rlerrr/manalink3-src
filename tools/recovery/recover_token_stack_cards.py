"""Recover three token stack unions and Urza's printed-cost predicate from exact native writes."""
import argparse
import ast
import collections
import copy
import csv
import hashlib
import json
import re
import subprocess
from pathlib import Path
from capstone import Cs,CS_ARCH_X86,CS_MODE_32
from capstone.x86_const import X86_OP_MEM,X86_OP_IMM,X86_REG_EBP
from elftools.dwarf.locationlists import LocationParser,LocationExpr,BaseAddressEntry
from elftools.dwarf.dwarf_expr import DWARFExprParser
from inspect_release import Image,value,inherited_value
from export_abi import type_of,type_name
from promote_recovery import clean,strip_comments,aggregate_stack_aliases,UNRESOLVED
from recover_stack_storage import rewrite,descendants,MEMBERS
from recover_literals import byte_literal,recover_strings
from audit_source import function_body

CARDS = {'2242640':'card_urzas_saga','235e100':'card_kiora_bests_the_sea_god','2565c60':'card_varina_lich_queen'}


def main():
    parser=argparse.ArgumentParser(__doc__);parser.add_argument('--apply',action='store_true');args=parser.parse_args()
    root=Path(__file__).resolve().parents[2];cache=root/'.recovery'
    storage_path=root/'docs/recovery-evidence/stack-storage.json';storage=json.loads(storage_path.read_text())
    debug_data=Path('G:/Manalink/ManalinkEh_10_22_Hotfix.dbg').read_bytes();assert hashlib.sha256(debug_data).hexdigest()==storage['debug_sha256']
    debug=Image(debug_data);dwarf=debug.dwarf();locations=LocationParser(dwarf.location_lists());expressions=DWARFExprParser(dwarf.structs)
    verified={}
    for cu in dwarf.iter_CUs():
        source=value(cu.get_top_DIE(),'DW_AT_name')
        if source not in {storage['candidates'][a]['source'] for a in CARDS}:continue
        for die in cu.get_top_DIE().iter_children():
            at=value(die,'DW_AT_low_pc');address=f'{at:x}' if at else ''
            if address not in CARDS:continue
            groups=collections.defaultdict(list)
            for variable in descendants(die):
                if variable.tag!='DW_TAG_variable' or 'DW_AT_location' not in variable.attributes:continue
                typ=type_name(type_of(variable))
                if typ not in MEMBERS:continue
                location=locations.parse_from_attribute(variable.attributes['DW_AT_location'],cu['version'],variable)
                entries=[location.loc_expr] if isinstance(location,LocationExpr) else [e.loc_expr for e in location if not isinstance(e,BaseAddressEntry)]
                offsets=set()
                for expression in entries:
                    ops=expressions.parse_expr(expression)
                    if len(ops)==1 and ops[0].op_name=='DW_OP_fbreg':offsets.add(ops[0].args[0])
                for offset in offsets:groups[str(offset)].append(dict(name=inherited_value(variable,'DW_AT_name'),type=typ,die=hex(variable.offset)))
            prepared=storage['candidates'][address]
            assert dict(groups)==prepared['groups'] and value(die,'DW_AT_frame_base')==prepared['frame_base']==[156]
            verified[address]=dict(groups=dict(groups),frame_base=[156])
        cu._dielist.clear();cu._diemap.clear()
    assert set(verified)==set(CARDS)
    runtime=Image((cache/'2022-ManalinkEh.dll').read_bytes());metadata={r['low_pc'][2:]:r for r in csv.DictReader((cache/'2022-functions.csv').open()) if r['low_pc'] and r['high_pc']}
    native_cost=metadata['2667b10'];assert native_cost['name']=='has_no_mana_cost' and native_cost['source']=='functions/play_spell_for_free.c'
    assert 'int has_no_mana_cost(int player, int card){' in (root/native_cost['source']).read_text()
    assert any(n=='_has_no_mana_cost' and s==2 and runtime.pe.OPTIONAL_HEADER.ImageBase+runtime.pe.sections[sec-1].VirtualAddress+offset==0x2667b10 for n,offset,sec,kind,s in runtime.functions())
    header=(root/'manalink.h').read_bytes().decode('latin1').replace('\r\n','\n');assert not re.search(r'\bint\s+has_no_mana_cost\s*\(',header)
    at=header.rfind('#ifdef __cplusplus');header=header[:at]+'/* Existing shared no-mana-cost predicate used by the recovered printed-cost test. */\nint has_no_mana_cost(int player, int card);\n\n'+header[at:]
    decoder=Cs(CS_ARCH_X86,CS_MODE_32);decoder.detail=True
    checks={'224270f':'c745e0b0b72202','22429cd':'c78538ffffff00000000','22429d7':'c78534ffffff00000000','22429e1':'c78548ffffff42000000',
            '235e249':'8b5508','235e24c':'b801000000','235e254':'29d0','235e256':'c745e400000000','235e25d':'8945a4',
            '2565da5':'c7854cffffff01000000','2565f09':'b806000000','2565f0e':'8dbdb8feffff','2565f14':'89c1','2565f16':'beeb417502',
            '2565f1b':'f3a5','2565f1d':'8dbdd0feffff','2565f23':'89c8','2565f25':'b913000000','2565f30':'f3ab',
            '222b7c9':'8b34c534707e00'}
    for at,expected_hex in checks.items():
        data=bytes.fromhex(expected_hex);assert runtime.pe.get_data(int(at,16)-0x2000000,len(data))==data
    message=runtime.pe.get_data(0x27541eb-0x2000000,24);assert message==b'Select a card to exile.\0'
    proof=dict(debug_sha256=storage['debug_sha256'],dwarf_locals=verified,cfa=dict(register='ebp',addend=8),machine_checks=checks,
               prompt=dict(address='27541eb',bytes=message.hex(),copy_bytes=24,zero_bytes=76,total_bytes=100),functions={},scope='Revalidated native DWARF shared frame offsets; use the full 76-byte token view with unchanged test/target prefix views and exact native storage extents. Normalize only verified contiguous token word writes and Varina prompt initialization.')
    proof['existing_shared_interface']=dict(name='has_no_mana_cost',prototype='int has_no_mana_cost(int player, int card);',source=native_cost['source'],target_address='2667b10',new_implementations=0)
    target_token=storage['target_layouts']['token_generation_t'];assert target_token['size']==76
    storage['target_layouts']['token_generation_2022_t']=copy.deepcopy(target_token);storage['source_layouts']['token_generation_2022_t']=76
    storage.setdefault('source_type_aliases',{})['token_generation_2022_t']='token_generation_t'
    entries=[];bodies={};new_storage={}
    for address in ['222b7b0',*CARDS]:
        row=metadata[address];raw=(cache/'decompiled-abi'/(address+'.c')).read_text();text=strip_comments(raw)
        code=runtime.pe.get_data(int(address,16)-0x2000000,int(row['high_pc'],16)-int(address,16));instructions=list(decoder.disasm(code,int(address,16)))
        assert sum(i.size for i in instructions)==len(code) and code[:3].hex()=='5589e5'
        detail=dict(machine_bytes=len(code),machine_sha256=hashlib.sha256(code).hexdigest(),literal_blocks=[],word_stores=[])
        literals=[];storage_proof=None
        if address=='222b7b0':
            assert any(n=='_has_mana_cost_0_or_1' and s==3 and runtime.pe.OPTIONAL_HEADER.ImageBase+runtime.pe.sections[sec-1].VirtualAddress+offset==int(address,16) for n,offset,sec,kind,s in runtime.functions())
            pair='  uVar1._0_2_ = cards_data[iid].id;\n  uVar1._2_2_ = cards_data[iid].reserved2;';assert text.count(pair)==1
            text=text.replace(pair,'  uVar1 = (uint32_t)cards_data[iid].id | ((uint32_t)cards_data[iid].reserved2 << 16);')
        else:
            prepared=storage['candidates'][address];assert prepared['raw_sha256']==hashlib.sha256(raw.encode()).hexdigest()
            aliases=aggregate_stack_aliases(clean(text,use_token_api_2022=True))
            expected=[tuple('token_generation_2022_t' if part=='token_generation_t' else part for part in alias) for alias in prepared['expected_aliases']]
            assert aliases==expected
            if address=='2565c60':
                start=text.index('      puVar3 = (undefined4 *)&UNK_027541eb;');end=text.index('      new_default_test_definition',start)
                original=text[start:end];replacement='      memcpy(msg, '+byte_literal(message[:-1])+', 24);\n      memset(msg + 24, 0, 76);\n'
                detail['literal_blocks'].append(dict(original=original,replacement=replacement,original_sha256=hashlib.sha256(original.encode()).hexdigest(),kind='100-byte prompt initializer'))
                text=text[:start]+replacement+text[end:]
                for item in ('undefined4 *puVar3;','char *pcVar4;','byte bVar5;','bVar5 = 0;'):
                    assert text.count(item)==1;text=text.replace(item,'',1)
            if address in {'2242640','2565c60'}:
                matches=list(re.finditer(r"(?m)^([ \t]*)test\.message\[(0x[0-9a-f]+)\] = ('(?:\\.|[^'\\])*');\n",text))
                assert len(matches)==(12 if address=='2242640' else 4)
                stores=[]
                for start in range(0,len(matches),4):
                    block=matches[start:start+4];offset=int(block[0][2],16)
                    assert [int(m[2],16) for m in block]==list(range(offset,offset+4))
                    value_bytes=bytes(ord(ast.literal_eval(m[3])) for m in block);number=int.from_bytes(value_bytes,'little')
                    member={24:'pow',28:'tou',44:'special_infos',48:'action'}[offset]
                    assert target_token['members'][member]==offset and target_token['member_sizes'][member]==4
                    expr='TOKEN2022_ACTION_TAPPED' if member=='action' else str(number)
                    original=text[block[0].start():block[-1].end()]
                    replacement=block[0][1]+'((token_generation_2022_t *)&test)->'+member+' = '+expr+';\n'
                    detail['literal_blocks'].append(dict(original=original,replacement=replacement,original_sha256=hashlib.sha256(original.encode()).hexdigest(),kind='token word store'))
                    frame=next(int(off) for off,variables in prepared['groups'].items() if any(v['type']=='token_generation_t' for v in variables))
                    sites=[i for i in instructions if i.mnemonic=='mov' and len(i.operands)==2 and i.operands[0].type==X86_OP_MEM and i.operands[0].mem.base==X86_REG_EBP and i.operands[0].mem.disp==frame+8+offset and i.operands[0].size==4 and i.operands[1].type==X86_OP_IMM and i.operands[1].imm==number]
                    assert len(sites)==1
                    detail['word_stores'].append(dict(member=member,offset=offset,value=number,frame_offset=frame,site=dict(address=f'{sites[0].address:x}',bytes=sites[0].bytes.hex())))
                    stores.append((block[0].start(),block[-1].end(),replacement))
                for begin,end,replacement in reversed(stores):text=text[:begin]+replacement+text[end:]
            groups=copy.deepcopy(prepared['groups'])
            for variables in groups.values():
                for variable in variables:
                    if variable['type']=='token_generation_t':variable.update(type='token_generation_2022_t',dwarf_type='token_generation_t')
            sizes=dict(prepared['target_sizes'],token_generation_2022_t=76)
            resolved,literals=recover_strings(text,runtime,{})
            text=clean(resolved,use_keyword_accessors_2022=True,use_extension_flags_2022=True,use_mana_api_2022=True,use_token_api_2022=True)
            before=text;text,views=rewrite(text,aggregate_stack_aliases(text),groups,sizes)
            for view in views:
                token=view['storage']+'.token'
                text=text.replace('(&'+token+')->',token+'.')
            if address=='235e100':
                for original,replacement,offset in [('recovered_stack_token.token.s_player = 1 - player;','recovered_stack_token.target.allowed_controller = 1 - player;',4),('recovered_stack_token.token.special_flags2 = 0;','recovered_stack_token.target.allow_cancel = 0;',68)]:
                    assert text.count(original)==1;text=text.replace(original,replacement,1)
                detail['target_view_writes']=[dict(member='allowed_controller',offset=4,site='235e25d'),dict(member='allow_cancel',offset=68,site='235e256')]
            storage_proof=dict(storage=views,frame_base=[156],previous_body_sha256=hashlib.sha256(function_body(before,row['name']).encode()).hexdigest(),scope=proof['scope'],resolved_partial_views=[],unresolved_partial_views=[])
        if address=='222b7b0':text=clean(text)
        assert not UNRESOLVED.search(strip_comments(text)) and not aggregate_stack_aliases(text) and not re.search(r'\._\d|\.field_',text)
        private=address=='222b7b0'
        if private:text='static '+text
        text=subprocess.run(['C:/Program Files/LLVM/bin/clang-format.exe','-style={BasedOnStyle: LLVM, IndentWidth: 2, BreakBeforeBraces: Allman, ColumnLimit: 100}'],input=text.encode(),capture_output=True,check=True).stdout.decode().replace('\r\n','\n')
        body_hash=hashlib.sha256(function_body(text,row['name']).encode()).hexdigest()
        if storage_proof:storage_proof['body_sha256']=body_hash;new_storage[address]=dict(name=row['name'],source=row['source'],**storage_proof)
        entry=dict(name=row['name'],address=address,source=row['source'],line=row['line'],static=private,prototype=text[:text.index('{')].strip()+';',raw_sha256=hashlib.sha256(raw.encode()).hexdigest(),recovered_literals=literals,reconstruction=proof['scope'],reconstruction_body_sha256=body_hash,token_stack_evidence=detail)
        if storage_proof:entry['stack_storage_reconstruction']=storage_proof
        entries.append(entry);bodies[address]=text;proof['functions'][address]=detail
    sources={}
    for entry in entries:
        source=entry['source'];sources.setdefault(source,(root/source).read_bytes().decode('latin1').replace('\r\n','\n'))
        sources[source]+=f"\n\n/* ManalinkEh 2022: 0x{entry['address']}; original {source}:{entry['line']}. */\n"+bodies[entry['address']]
    scratch=cache/'token-stack-check';scratch.mkdir(exist_ok=True)
    (scratch/'manalink.h').write_bytes(header.encode('latin1'))
    for source,text in sources.items():
        candidate=scratch/Path(source).name;candidate.write_bytes(text.encode('latin1'))
        result=subprocess.run(['C:/Program Files/LLVM/bin/clang.exe','-target','i686-pc-windows-msvc','-std=gnu99','-fsyntax-only','-Werror=implicit-function-declaration','-Werror=incompatible-pointer-types','-Werror=return-type','-Werror=uninitialized','-Werror=sometimes-uninitialized','-Werror=format','-I',str(scratch),'-I',str(root),str(candidate)],capture_output=True,text=True)
        (scratch/(candidate.name+'.log')).write_text(result.stderr);result.check_returncode()
    if args.apply:
        p=root/'docs/recovery-evidence/recovered-functions.json';manifest=json.loads(p.read_text());assert not set(bodies)&{f['address'] for f in manifest['accepted_functions']}
        for source,text in sources.items():(root/source).write_bytes(text.replace('\n','\r\n').encode('latin1'))
        (root/'manalink.h').write_bytes(header.replace('\n','\r\n').encode('latin1'))
        manifest['accepted_functions'].extend(entries);manifest['accepted']=len(manifest['accepted_functions']);p.write_text(json.dumps(manifest,indent=2))
        storage['functions'].update(new_storage);storage_path.write_text(json.dumps(storage,indent=2))
        p=root/'docs/recovery-evidence/interface-recovery.json';interfaces=json.loads(p.read_text());interfaces['token_stack_cards_2022']=proof;p.write_text(json.dumps(interfaces,indent=2))
    print(json.dumps(dict(applied=args.apply,card_functions=3,private_helpers=1,stack_unions=3,typed_token_word_stores=4)))


if __name__=='__main__':main()
