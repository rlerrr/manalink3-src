"""Remove the unused player argument and predecessor from private zone counting."""
import argparse,csv,json,re,shutil
from pathlib import Path
from unify_scalar_helpers import ROOT,read,sha,span,mask
from audit_source import function_body

EVIDENCE='docs/recovery-evidence/retired-zone-counter.json'
SOURCE='functions/count_graveyard.c';OLD='count_zone_by_type';NEW=OLD+'_2022'
CALLS={'count_graveyard_by_type':('count_zone_by_type(i, type, get_grave(i))','count_zone_by_type_2022(type, get_grave(i))'),
       'count_deck_by_type':('count_zone_by_type(player, type, deck_ptr[player])','count_zone_by_type_2022(type, deck_ptr[player])')}
REPLACEMENT='/* Native zone counting has two physical arguments; the unused player predecessor is retired. */\nstatic int count_zone_by_type_2022(int type, const int *zone);'

def verify(root=ROOT):
    path=root/EVIDENCE
    if not path.exists():return {}
    e=json.loads(path.read_text());assert e['source']==SOURCE and e['retained_name']==OLD and e['recovered_name']==NEW
    before=e['predecessor_definition'];assert sha(before.encode())==e['predecessor_definition_sha256']
    assert len(re.findall(r'\bplayer\b',mask(before)))==1
    text=read(root/SOURCE);assert span(text,OLD) is None and not re.search(r'\b'+OLD+r'\b',mask(text)) and text.count(REPLACEMENT)==1
    assert set(e['callers'])==set(CALLS)
    for name,(old,new) in CALLS.items():
        c=e['callers'][name];assert c['old_call']==old and c['native_call']==new
        assert sha(c['predecessor_definition'].encode())==c['predecessor_sha256']
        assert c['predecessor_definition'].count(old)==1
        expected=c['predecessor_definition'].replace(old,new,1);b=span(text,name)
        assert b and text[b[0]:b[1]]==expected
        assert sha(expected.encode())==c['native_source_sha256']
    m=json.loads((root/'docs/recovery-evidence/recovered-functions.json').read_text());f=next(f for f in m['accepted_functions'] if f['name']==NEW)
    assert f==e['accepted_recovery_record'] and f['static'] and f['address']=='26e5c70'
    assert f['specialization']['symbol']=='_count_zone_by_type.isra.0'
    assert [p['name'] for p in f['specialization']['parameters'] if p['entry_location'] in [[80],[82]]]==['type','zone']
    from qualify_zone_inputs import audit_sources
    assert audit_sources({},root)['functions']==3
    from inspect_release import Image
    from capstone import Cs,CS_ARCH_X86,CS_MODE_32
    from capstone.x86_const import X86_OP_IMM
    data=(root/'.recovery/2022-ManalinkEh.dll').read_bytes()
    assert sha(data)==e['native_runtime_sha256']=='52793f60124a3b83d8b2728d145e918101a395b4e8d1c0af69cddcfc87ef10e5'
    image=Image(data);decoder=Cs(CS_ARCH_X86,CS_MODE_32);decoder.detail=True
    for c in e['callers'].values():
        n=c['native_range'];lo=int(n['start'],16);hi=int(n['end'],16);code=image.pe.get_data(lo-0x2000000,hi-lo)
        assert sha(code)==n['sha256']
        instructions=list(decoder.disasm(code,lo));assert sum(i.size for i in instructions)==len(code)
        edges=[dict(address=hex(i.address),bytes=i.bytes.hex()) for i in instructions if i.mnemonic in {'call','jmp'} and i.operands[0].type==X86_OP_IMM and i.operands[0].imm==0x26e5c70]
        assert edges==n['worker_edges'] and edges
    return dict(retired_unused_private_bodies=1,migrated_callers=2,physical_worker_arguments=2,
                scope='Only the unused private player argument is removed; discarded arguments are plain local reads. Native caller ranges and direct worker edges verified; recovered counting algorithm unchanged.')

def main():
    p=argparse.ArgumentParser(__doc__);p.add_argument('--apply',action='store_true');p.add_argument('--verify',action='store_true');args=p.parse_args()
    if args.verify:print(json.dumps(verify(),indent=2));return
    assert not (ROOT/EVIDENCE).exists()
    text=read(ROOT/SOURCE);before=text;b=span(text,OLD);assert b;old=text[b[0]:b[1]]
    assert len(re.findall(r'\bplayer\b',mask(old)))==1
    text=text[:b[0]]+REPLACEMENT+text[b[1]:];callers={}
    from inspect_release import Image
    from capstone import Cs,CS_ARCH_X86,CS_MODE_32
    from capstone.x86_const import X86_OP_IMM
    data=(ROOT/'.recovery/2022-ManalinkEh.dll').read_bytes();image=Image(data);decoder=Cs(CS_ARCH_X86,CS_MODE_32);decoder.detail=True
    metadata=list(csv.DictReader((ROOT/'.recovery/2022-functions.csv').open()))
    for name,(old_call,new_call) in CALLS.items():
        b=span(text,name);definition=text[b[0]:b[1]];assert definition.count(old_call)==1
        after=definition.replace(old_call,new_call,1);text=text[:b[0]]+after+text[b[1]:]
        rows=[r for r in metadata if r['name']==name and r['source']==SOURCE and r['low_pc'] and r['high_pc']];assert len(rows)==1
        lo=int(rows[0]['low_pc'],16);hi=int(rows[0]['high_pc'],16);code=image.pe.get_data(lo-0x2000000,hi-lo)
        ins=list(decoder.disasm(code,lo));assert sum(i.size for i in ins)==len(code)
        edges=[dict(address=hex(i.address),bytes=i.bytes.hex()) for i in ins if i.mnemonic in {'call','jmp'} and i.operands[0].type==X86_OP_IMM and i.operands[0].imm==0x26e5c70];assert edges
        callers[name]=dict(old_call=old_call,native_call=new_call,predecessor_definition=definition,predecessor_sha256=sha(definition.encode()),
            native_source_sha256=sha(after.encode()),native_range=dict(start=hex(lo),end=hex(hi),sha256=sha(code),worker_edges=edges))
    assert not re.search(r'\b'+OLD+r'\b',mask(text))
    m=json.loads((ROOT/'docs/recovery-evidence/recovered-functions.json').read_text());f=next(f for f in m['accepted_functions'] if f['name']==NEW)
    e=dict(source=SOURCE,retained_name=OLD,recovered_name=NEW,predecessor_definition=old,predecessor_definition_sha256=sha(old.encode()),
        source_replacement=REPLACEMENT,callers=callers,accepted_recovery_record=f,native_runtime_sha256=sha(data))
    if args.apply:
        backup=ROOT/'.recovery/retired-zone-counter-before';assert not backup.exists();backup.mkdir();shutil.copy2(ROOT/SOURCE,backup/'count_graveyard.c')
        (ROOT/SOURCE).write_bytes(text.replace('\n','\r\n').encode('latin1'));(ROOT/EVIDENCE).write_text(json.dumps(e,indent=2)+'\n');verify()
    print(json.dumps(dict(applied=args.apply,retired_private_bodies=1,migrated_callers=2)))

if __name__=='__main__':main()
