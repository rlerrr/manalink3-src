"""Typed reconstruction of the shared target/test/selection slot in GAA."""
import re

STORAGE='''typedef union
{
  target_definition_2022_t target;
  test_definition_2022_t test;
  target_t selected;
} activated_storage_2022_t;'''

def reconstruct(address,text,instructions):
    from capstone.x86_const import X86_OP_MEM
    changes=[]
    def replace(old,new,kind,sites=()):
        nonlocal text
        count=text.count(old);assert count,(address,old);text=text.replace(old,new)
        changes.append(dict(original=old,replacement=new,occurrences=count,kind=kind,
            machine_sites=[dict(address=f'{i.address:x}',bytes=i.bytes.hex(),instruction=i.mnemonic+' '+i.op_str) for i in sites]))
    if address=='269c8c0':
        sites=[i for i in instructions if any(o.type==X86_OP_MEM and o.mem.disp in {-228,-224} for o in i.operands)];assert sites
        replace('target_definition_t td_default;','activated_storage_2022_t storage;','native shared 216-byte target/test/selection storage',sites)
        replace('(test_definition_t *)&td_default','&storage.test','native shared test view',sites)
        replace('(target_t *)&td_default','&storage.selected','native shared selected-pair view',sites)
        # Only the reads following selection interpret the two first words as
        # selected player/card. Earlier writes initialize a real target record.
        at=text.index('iVar4 = select_target(');head,tail=text[:at],text[at:]
        for old,new in [('td_default.who_chooses','storage.selected.player'),('td_default.allowed_controller','storage.selected.card')]:
            count=tail.count(old);assert count
            # The recorded strings are unique to the tail (the earlier default
            # target controller writes have a different right-hand side).
            tail=tail.replace(old,new)
            changes.append(dict(original=old,replacement=new,occurrences=count,kind='selected pair prefix at native offsets zero/four',machine_sites=[] ,after_anchor='iVar4 = select_target('))
        text=head+tail
        replace('td_default','storage.target','native shared default target view')
    for name in ['SF_0','SF2_0']:
        count=len(re.findall(r'\b'+name+r'\b',text))
        if count:
            text=re.sub(r'\b'+name+r'\b','0',text)
            changes.append(dict(original=name,replacement='0',occurrences=count,kind='native zero flag spelling',machine_sites=[]))
    if address in {'269e2b0','269e810'}:
        sites=[i for i in instructions if i.mnemonic in {'ja','jbe'}];assert sites
        replace('1 < event - EVENT_RESOLVE_ACTIVATION','1U < (uint32_t)event - (uint32_t)EVENT_RESOLVE_ACTIVATION','native unsigned activation event range',sites)
    return text,changes
