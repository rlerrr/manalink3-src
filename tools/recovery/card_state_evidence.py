"""Native card-state storage, dispatch paths and bounded ID classifiers."""
from functools import lru_cache
from inspect_release import Image,value,inherited_value

ENUMS=dict(STATUS_TOKEN=0x10,SF_UNEARTH=0x4000,SF2_X_SPELL=0x20000,
           SF2_THOUSAND_YEAR_ELIXIR=0x1000,STATE_SUMMON_SICK=0x20000,
           STATE_ATTACKING=4,STATE_TAPPED=0x10,STATE_CANNOT_TARGET=0x100000,
           STATE_OWNED_BY_OPPONENT=0x1000,TYPE_CREATURE=2,TYPE_LAND=1,
           TYPE_PERMANENT=0x47,TYPE_EFFECT=0x80,EA_MANA_SOURCE=0x1000,
           ANYBODY=2,PB_SEND_EVENT_UNTAP_CARD_TO_ALL=4,EVENT_TAP_CARD=0x81,
           EVENT_UNTAP_CARD=0x83)
FIELDS={
    'card_instance_t': {'state','token_status','number_of_targets','backup_internal_card_id',
                        'internal_card_id','targets','parent_controller','parent_card'},
    'card_data_t': {'id','type','cc','code_pointer','extra_ability'},
    'card_ptr_t': {'expansion'},
    'card_ext_t': {'special_flags','special_flags2'},
}


def accessed_records(debug):
    from color_hack_evidence import layout
    from pack_native_tables import unqual
    records={};enums={}
    for cu in debug.dwarf().iter_CUs():
        if value(cu.get_top_DIE(),'DW_AT_name')!='functions/functions.c':continue
        for d in cu.iter_DIEs():
            n=inherited_value(d,'DW_AT_name')
            if d.tag=='DW_TAG_enumerator' and n in ENUMS:enums[n]=value(d,'DW_AT_const_value')
            if d.tag=='DW_TAG_typedef' and n in FIELDS:
                t=unqual(d)
                records[n]=dict(bytes=value(t,'DW_AT_byte_size'),members={value(c,'DW_AT_name'):dict(offset=value(c,'DW_AT_data_member_location'),**layout(c)) for c in t.iter_children() if c.tag=='DW_TAG_member' and value(c,'DW_AT_name') in FIELDS[n]})
    assert enums==ENUMS,(enums,ENUMS)
    assert {n:r['bytes'] for n,r in records.items()}==dict(card_instance_t=300,card_data_t=72,card_ptr_t=152,card_ext_t=192)
    assert all(set(records[n]['members'])==fields for n,fields in FIELDS.items())
    return records,enums


def inspect_native(runtime,debug,meta,root):
    import csv,re
    from capstone import Cs,CS_ARCH_X86,CS_MODE_32
    from capstone.x86_const import X86_OP_IMM
    from export_abi import type_of,type_name
    from card_state_bodies import BODIES
    from recover_temporary_exile import sha
    symbols={};returns={};records,enums=accessed_records(debug)
    for a in BODIES:
        ss=[dict(name=n,storage=s) for n,v,sec,k,s in debug.functions() if sec>0 and k==32 and debug.pe.OPTIONAL_HEADER.ImageBase+debug.pe.sections[sec-1].VirtualAddress+v==int(a,16)]
        assert len(ss)==1 and ss[0]['storage']==2;symbols[a]=ss[0]
    for cu in debug.dwarf().iter_CUs():
        if value(cu.get_top_DIE(),'DW_AT_name') not in {'functions/functions.c','functions/deck.c','functions/play_spell_for_free.c'}:continue
        for d in cu.iter_DIEs():
            if d.tag!='DW_TAG_subprogram':continue
            lo=value(d,'DW_AT_low_pc');a=f'{lo:x}' if lo is not None else None
            if a in BODIES:returns[a]=type_name(type_of(d))
    assert set(returns)==set(BODIES)
    for a in ['2681300','269f2e0']:assert returns[a]=='card_instance_t *'
    assert returns['26b3560']=='card_data_t *'
    for a in ['2687990','2687a50','2687d40','2687d70','2687da0','2679c90','2679d10']:assert returns[a]=='void'
    classifier=decode_no_mana_cost_classifier(runtime.pe.get_data(0x667b30,0xdf))
    cases=sorted(int(s,0) for s in re.findall(r'case\s+(0x[0-9a-f]+|\d+)\s*:',BODIES['2667b10']))
    assert cases==classifier['accepted_ids'] and len(cases)==16
    rows=list(csv.DictReader((root/'.recovery/2022-registrations.csv').open()));by_id={int(r['csvid']):r for r in rows}
    ids=cases+[0x615a,0x6161]+[0x5b,0x7e,0xef,0xbc,0xa4,*range(0x834,0x839),0x3d67]
    names={str(i):by_id[i]['full_name'] for i in ids}
    assert names[str(0x615a)]=='Rules Engine' and names[str(0x6161)]=='Deadbox' and names[str(0x3d67)]=='Wastes'
    decoder=Cs(CS_ARCH_X86,CS_MODE_32);decoder.detail=True
    def code(a):return list(decoder.disasm(runtime.pe.get_data(int(a,16)-0x2000000,int(meta[a]['high_pc'],16)-int(a,16)),int(a,16)))
    calls=lambda a:[dict(address=hex(i.address),target=hex(i.operands[0].imm),bytes=i.bytes.hex()) for i in code(a) if i.mnemonic=='call' and i.operands[0].type==X86_OP_IMM]
    tap_calls=calls('2679c30');untap_calls=calls('2679c90')
    assert [c['target'] for c in tap_calls]==['0x26b2ab0','0x4359b0']
    assert [c['target'] for c in untap_calls]==['0x26b2ab0','0x4359b0','0x26cb650']
    # Identical independent native bodies, ignoring addresses in disassembly.
    # Relative conditional branches are normalized to offsets within the body.
    def sickness(a):
        return [(i.mnemonic,hex(i.operands[0].imm-int(a,16)) if i.mnemonic.startswith('j') else i.op_str) for i in code(a)]
    assert sickness('2682100')==sickness('2682170')
    return dict(native_symbols=symbols,return_types=returns,records=records,enum_values=enums,
                no_mana_cost_classifier=classifier,csvid_names=names,
                registration_metadata_sha256=sha((root/'.recovery/2022-registrations.csv').read_bytes()),
                tap_calls=tap_calls,untap_calls=untap_calls,identical_native_sickness_bodies=['2682100','2682170'],
                behavior=['Preserve token exclusion for unearth, a second instance lookup through the extension flags, and raw SF2_X_SPELL mask results, including the native negative-player fallback.',
                          'Keep signed hardcoded-function comparison, packed coding bitmap and native CSVID exclusions; normalize only upper bits masked away by zero-extended booleans.',
                          'The two sickness checks are identical. Add-state bulk mode selects creatures; remove-state bulk mode selects all permanents. Single-card modes skip the bulk filters. Unsupported untargettable modes do nothing.',
                          'Tap clears the mana-color marker only on the first tap and always returns zero. Untap reads a low-byte player flag and dispatches either to all cards or to one card; no-event untap only clears the bit.',
                          'Retain pointer-returning attack/activation queries, ascending slot scans and uncapped target iteration, graveyard printed types, creature-death sentinel 99 and get-card-data-before-in-play ordering.',
                          'Get-card-data falls back only for IID -1 to an unsigned 16-bit backup; mana-producing-land intentionally reads the current IID without that fallback. The 16 no-mana-cost IDs are decoded independently over the 16-bit domain and signed boundary cases.'])


@lru_cache(maxsize=2)
def decode_no_mana_cost_classifier(blob,start=0x2667b30):
    """Decode the read-only integer decision tree; no DLL code is executed."""
    from capstone import Cs,CS_ARCH_X86,CS_MODE_32
    from capstone.x86_const import X86_OP_IMM,X86_OP_REG
    from recover_temporary_exile import sha
    decoder=Cs(CS_ARCH_X86,CS_MODE_32);decoder.detail=True;nodes={}
    for i in decoder.disasm(blob,start):
        operands=[]
        for op in i.operands:
            if op.type==X86_OP_IMM:operands.append(('imm',op.imm & 0xffffffff))
            elif op.type==X86_OP_REG:operands.append(('reg',i.reg_name(op.reg)))
            else:operands.append(('unused',None))
        nodes[i.address]=(i.mnemonic,operands,i.address+i.size)
    def classify(csvid):
        regs={'eax':0,'ecx':csvid & 0xffffffff};pc=start;zf=sf=of=cf=False
        def read(op):
            if op[0]=='imm':return op[1]
            assert op[0]=='reg',op
            return regs[{'al':'eax','cl':'ecx'}.get(op[1],op[1])] & (255 if op[1] in {'al','cl'} else 0xffffffff)
        def write(op,v):
            assert op[0]=='reg'
            if op[1] in {'al','cl'}:
                n={'al':'eax','cl':'ecx'}[op[1]];regs[n]=(regs[n] & 0xffffff00) | (v & 255)
            else:regs[op[1]]=v & 0xffffffff
        for step in range(64):
            m,ops,nxt=nodes[pc]
            if m in {'cmp','sub'}:
                a=read(ops[0]);b=read(ops[1]);r=(a-b)&0xffffffff
                zf=r==0;sf=bool(r&0x80000000);of=bool((a^b)&(a^r)&0x80000000);cf=a<b
                if m=='sub':write(ops[0],r)
            elif m in {'xor','and','test'}:
                a=read(ops[0]);b=read(ops[1]);r=(a^b) if m=='xor' else (a&b)
                zf=r==0;sf=bool(r&0x80000000);of=cf=False
                if m!='test':write(ops[0],r)
            elif m=='mov':write(ops[0],read(ops[1]))
            elif m=='shl':write(ops[0],read(ops[0]) << (read(ops[1])&31))
            elif m in {'sete','setne'}:write(ops[0],int(zf if m=='sete' else not zf))
            elif m=='je':
                if zf:nxt=read(ops[0])
            elif m=='jg':
                if not zf and sf==of:nxt=read(ops[0])
            elif m=='jmp':nxt=read(ops[0])
            elif m=='ret':
                assert regs['eax'] in {0,1};return regs['eax']
            elif m not in {'leave','nop'}:raise AssertionError((hex(pc),m,ops))
            pc=nxt
        raise AssertionError('Classifier decision tree did not terminate')
    bitmap=bytes(classify(i) for i in range(65536));ids=[i for i,r in enumerate(bitmap) if r]
    boundaries={str(i):classify(i) for i in [-2147483648,-1,2147483647]}
    assert all(v==0 for v in boundaries.values())
    return dict(start=hex(start),bytes=len(blob),machine_sha256=sha(blob),tested_csvids=65536,signed_boundary_results=boundaries,accepted_ids=ids,result_sha256=sha(bitmap),instructions=len(nodes))
