"""Check identifier-only source cleanup and the native packet-call repair.

Historical names remain evidence identities. Product source and physical
symbols use the canonical names; this checker creates no compatibility aliases.
"""
import json,re
from pathlib import Path
from canonicalize_helpers import ROOT,read,mask,sha,span


def substitute(text,mapping):
    code=mask(text)
    edits=[(m.start(),m.end(),mapping[m[0]]) for m in re.finditer(r'\b[A-Za-z_]\w*\b',code) if m[0] in mapping]
    for lo,hi,new in reversed(edits):text=text[:lo]+new+text[hi:]
    return text


def records(proof,root):
    inputs={r['path']:r['sha256'] for r in proof.get('interface_supporting_inputs',[])}
    for ref in proof.get('source_domain_migrations',[]):
        if ref['kind']!='source_identifier_spellings':continue
        assert sha((root/ref['path']).read_bytes())==ref['sha256']==inputs[ref['path']]
        yield ref['path'],json.loads((root/ref['path']).read_text())


def verify(proof,root=ROOT,source_root=None):
    source_root=root if source_root is None else source_root
    rows=list(records(proof,root))
    if not rows:return {}
    from inspect_release import Image
    from capstone import Cs,CS_ARCH_X86,CS_MODE_32
    from capstone.x86_const import X86_OP_IMM,X86_OP_MEM,X86_OP_REG
    runtime=Image((root/'.recovery/2022-ManalinkEh.dll').read_bytes())
    magic=Image((root/'.recovery/2022-Magic.exe').read_bytes())
    count=0;calls=0
    for path,record in rows:
        assert sha(runtime.data)==record['dll_sha256'] and sha(magic.data)==record['exe_sha256']
        mapping=record['names'];link_mapping={'_'+k:'_'+v for k,v in mapping.items()}
        for edit in proof['source_edits']:
            if edit.get('identifier_spelling_migration')!=path:continue
            m=link_mapping if edit['source'].endswith(('.lds','.asm')) else mapping
            assert substitute(edit['before'],m)==edit['after']
            count+=1
        for source in record['source_files']:
            words=set(re.findall(r'\b[A-Za-z_]\w*\b',mask(read(source_root/source))))
            assert not (words&set(mapping)),(source,words&set(mapping))
        linker=read(source_root/'manalink.lds')
        for binding in record['absolute_bindings']:
            name=binding['canonical_name'];old=binding['recovery_name'];at=binding['address']
            assert len(re.findall(r'(?mi)^_'+name+r'\s*=\s*'+at+r';',linker))==1
            assert not re.search(r'\b_'+old+r'\b',mask(linker))
        packet=record['packet_calls'];decoder=Cs(CS_ARCH_X86,CS_MODE_32);decoder.detail=True
        source=read(source_root/packet['source']);bounds=span(source,packet['caller']);assert bounds
        body=mask(source[bounds[0]:bounds[1]])
        header=read(source_root/'manalink.h')
        for call in packet['calls']:
            lo,hi=int(call['start'],16),int(call['end'],16)
            data=runtime.pe.get_data(lo-0x2000000,hi-lo);assert sha(data)==call['sha256']
            ins=list(decoder.disasm(data,lo));last=ins[-1]
            assert last.mnemonic=='call' and last.address+last.size==hi and last.operands[0].type==X86_OP_IMM and last.operands[0].imm==int(call['target'],16)
            slots={}
            for i in ins[:-1]:
                assert not i.mnemonic.startswith('j') and i.mnemonic not in {'call','push','pop','ret','leave'}
                assert all(i.reg_name(r) not in {'esp','ebp'} for r in i.regs_access()[1])
                if i.mnemonic=='mov' and i.operands[0].type==X86_OP_MEM:
                    mem=i.operands[0].mem
                    if i.reg_name(mem.base)=='esp':
                        assert not mem.index and i.operands[0].size==4 and i.operands[1].type==X86_OP_IMM
                        slots[mem.disp]=i.operands[1].imm
            assert [slots[0],slots[4]]==call['arguments']
            actual=call['name']+'('+', '.join(str(a) for a in call['arguments'])+');'
            assert len(re.findall(r'\b'+call['name']+r'\s*\(',body))==1 and actual in source[bounds[0]:bounds[1]]
            assert 'int '+call['name']+'(int player, int packet_type);' in header
            for anchor in call['callee_argument_reads']:
                at=int(anchor['address'],16);bs=bytes.fromhex(anchor['bytes_hex'])
                assert magic.pe.get_data(at-0x400000,len(bs))==bs
                i=list(decoder.disasm(bs,at));assert len(i)==1
                assert any(o.type==X86_OP_MEM and i[0].reg_name(o.mem.base)=='ebp' and not o.mem.index and o.mem.disp==anchor['offset'] for o in i[0].operands)
            assert sorted(a['offset'] for a in call['callee_argument_reads'])==[8,12]
            calls+=1
    return dict(identifiers=sum(len(r['names']) for _,r in rows),identifier_edits=count,packet_calls=calls,
                scope='Identifier-only edits preserve literals, types and algorithms. Packet call repair independently checks two physical arguments at each native caller and both stack-argument reads in Magic.exe.')


def verify_compiled(debug,proof,root=ROOT):
    result={}
    for _,record in records(proof,root):
        for old,new in record['names'].items():
            assert not any(s[0]=='_'+old or s[0].startswith('_'+old+'.') for s in debug.symbols),old
        for binding in record['absolute_bindings']:
            matches=[s for s in debug.symbols if s[0]=='_'+binding['canonical_name']]
            assert len(matches)==1 and matches[0][1]==int(binding['address'],16) and matches[0][2]==-1,binding
            result[binding['canonical_name']]=binding['address']
    return dict(absolute_bindings=result,scope='Physical canonical COFF names and original absolute addresses; no emitted recovery-name aliases.')
