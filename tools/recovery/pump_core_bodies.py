"""Scoped source reconstructions for verified native pump storage/operands."""
import re

def reconstruct(address,text,instructions):
    from capstone.x86_const import X86_OP_MEM
    changes=[]
    def replace(old,new,kind,sites):
        nonlocal text
        count=text.count(old);assert count,(address,old);text=text.replace(old,new)
        changes.append(dict(original=old,replacement=new,occurrences=count,kind=kind,
            machine_sites=[dict(address=f'{i.address:x}',bytes=i.bytes.hex(),instruction=i.mnemonic+' '+i.op_str) for i in sites]))
    def displacement(value):return [i for i in instructions if any(o.type==X86_OP_MEM and o.mem.disp==value for o in i.operands)]
    if address=='267a790':
        sites=displacement(0x4ef1a4);assert sites
        replace('*(uint *)(&player_bits + t_player * 4)','player_bits[t_player]','typed native player-bit word',sites)
    if address=='26a2390':
        sites=displacement(0x4ef1a4);assert sites
        replace('*(uint *)(&player_bits + iVar6 * 4)','player_bits[iVar6]','typed native player-bit word',sites)
        sites=[i for i in instructions if i.mnemonic=='jbe'];assert len(sites)==2
        replace('3 < kVar7 - KILL_BURY','3U < (uint32_t)kVar7 - (uint32_t)KILL_BURY','native unsigned kill-code range',sites)
    if address in {'26775c0','26a2390'}:
        n=2 if address=='26775c0' else 3
        sites=[i for i in instructions if i.mnemonic in {'ja','jbe'}];assert sites
        replace(f'event - EVENT_POWER < {n}',f'(uint32_t)event - (uint32_t)EVENT_POWER < {n}U','native unsigned characteristic-event range',sites)
    if address=='2685dd0':
        sites=displacement(10);assert sites
        replace('*(byte *)((int)&pcVar2->state + 2)','((uint32_t)pcVar2->state >> 16 & 0xffU)','native state-byte projection',sites)
    if address=='26a1550':
        sites=[i for i in instructions if 'movsd' in i.mnemonic];assert len(sites)==2
        copies=[(r'for \(iVar4 = 0x15; iVar4 != 0; iVar4 = iVar4 \+ -1\) \{\s*ptVar6->who_chooses = ptVar5->who_chooses;\s*ptVar5 = \(target_definition_t \*\)&ptVar5->allowed_controller;\s*ptVar6 = \(target_definition_t \*\)&ptVar6->allowed_controller;\s*\}', 'td_prompt = *td;'),
                (r'for \(iVar4 = 0x15; iVar4 != 0; iVar4 = iVar4 \+ -1\) \{\s*ptVar7->who_chooses = ptVar6->who_chooses;\s*ptVar6 = \(target_definition_t \*\)&ptVar6->allowed_controller;\s*ptVar7 = \(target_definition_t \*\)&ptVar7->allowed_controller;\s*\}', 'td_regen = *td;')]
        for (pattern,new),site in zip(copies,sites):
            match=re.search(pattern,text);assert match
            replace(match[0],new,'native 21-word target-record copy',[site])
        replace('undefined *puVar2;','uint32_t puVar2;','native integer side-effect selector',[])
        replace('(undefined *)(keyword & 0x1f000000)','((uint32_t)keyword & 0x1f000000U)','native integer side-effect selector',[])
        replace('&UNK_0a000000','0x0a000000U','native side-effect constant',[])
        for literal in sorted(set(re.findall(r'\(undefined \*\)(0x[0-9a-f]+)',text))):
            replace('(undefined *)'+literal,literal+'U','native side-effect constant',[])
    return text,changes
