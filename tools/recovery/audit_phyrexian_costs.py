"""Audit Phyrexian private parameters, native data and engine payment fragments."""
import hashlib
import struct
from audit_source import function_body
from recover_literals import readonly_string
from recover_phyrexian_costs import ADDRESSES,PRIVATE,CONTROL_SITES,FLAG


def audit_component(component,manifest,sources,machine_bytes,image):
    def require(condition,message):
        if not condition:raise ValueError(message)
    entries={f['address']:f for f in manifest['accepted_functions'] if f.get('phyrexian_cost_evidence')}
    require(set(entries)==set(ADDRESSES)==set(component['functions']), 'Phyrexian component membership mismatch')
    require({a for a,f in entries.items() if f['static']}==PRIVATE, 'Phyrexian private linkage mismatch')
    for address,detail in component['functions'].items():
        require(entries[address]['phyrexian_cost_evidence']==detail, 'Phyrexian function evidence mismatch')
    require([len(component['parameters'][a]) for a in ADDRESSES]==[8,8,8,10,8,10], 'Phyrexian physical parameter count mismatch')
    private=component['parameters']['226caa0']
    require([p['name'] for p in private]==['player','cless','black','blue','green','red','white','clred_artifact'], 'Phyrexian private parameter order mismatch')
    for p,expression in zip(private[:3],['DW_OP_reg0 (r0)','DW_OP_reg2 (r2)','DW_OP_reg1 (r1)']):
        require(p['location'][0]['expression']=='('+expression+')', 'Phyrexian private register parameter mismatch')
    flag=component['payment_flag'];words=component['color_words']
    require((flag['address'],flag['bytes'],flag['zero_fill'],flag['location'])==('277401c',4,True,[3,28,64,119,2]), 'Phyrexian mode flag native layout mismatch')
    rva=int(flag['address'],16)-image.pe.OPTIONAL_HEADER.ImageBase
    section=next(s for s in image.pe.sections if s.VirtualAddress<=rva<s.VirtualAddress+s.Misc_VirtualSize)
    require(section.SizeOfRawData==0, 'Phyrexian mode flag is not zero-fill storage')
    require(words['address']=='2739c58' and words['location']==[3,88,156,115,2], 'Phyrexian color-table location mismatch')
    table=machine_bytes(int(words['address'],16),24)
    require(table.hex()==words['bytes'], 'Phyrexian color-table pointer bytes mismatch')
    expected=[b'colorless',b'black',b'blue',b'green',b'red',b'white']
    require(len(words['words'])==6, 'Phyrexian color-table length mismatch')
    for pointer,word,text in zip(struct.unpack('<6I',table),words['words'],expected):
        require(f'{pointer:x}'==word['address'] and readonly_string(image,pointer)==bytes.fromhex(word['bytes'])==text, 'Phyrexian color-table string mismatch')
    for record in [flag,words]:
        require(record['declaration'] in manifest['source_globals'], 'Phyrexian global source registry mismatch')
    require(dict(name='_'+FLAG,address='277401c',storage=2) in component['data_symbols'] and
            any(s['address']=='2739c58' and s['storage']==3 for s in component['data_symbols']), 'Phyrexian public/private data linkage mismatch')
    require(len(flag['sites'])==3 and len(words['sites'])==1 and
            [s['address'] for s in component['control_sites']]==CONTROL_SITES, 'Phyrexian native instruction membership mismatch')
    for site in flag['sites']+words['sites']+component['control_sites']:
        data=bytes.fromhex(site['bytes'])
        require(machine_bytes(int(site['address'],16),len(data))==data, 'Phyrexian native word/control operand mismatch')
    sites={s['address']:s['bytes'] for s in component['control_sites']}
    require(sites['26ae87d']=='f604b51872790202' and sites['26ae898']=='8b151c407702' and
            sites['26af4fe']=='39c6' and sites['26af525']=='83f8ff' and sites['26af7be']=='891504f44e00',
            'Native Krrik flag, cancellation, bound or cost update mismatch')
    engine=component['engine'];source=sources['functions/engine.c'].replace('\r\n','\n')
    require(hashlib.sha256(machine_bytes(0x26ae610,engine['machine_bytes'])).hexdigest()==engine['machine_sha256'], 'Native engine payment body hash mismatch')
    require(hashlib.sha256(function_body(source,'charge_mana').encode()).hexdigest()==engine['body_sha256'], 'Recovered engine payment fragment body mismatch')
    require([f['piece'] for f in engine['fragments']]==['locals','selection','settlement'] and
            all(f in manifest['source_fragments'] for f in engine['fragments']), 'Engine payment fragment registry mismatch')
    for fragment in engine['fragments']:
        start=source.index(fragment['start_marker']);end=source.index(fragment['end_marker'],start)+len(fragment['end_marker'])
        require(source[end]=='\n','Engine payment fragment boundary mismatch')
        source=source[:start]+source[end+1:]
    require(hashlib.sha256(function_body(source,'charge_mana').encode()).hexdigest()==engine['previous_body_sha256'], 'Original engine body changed outside payment fragments')
    prompt=engine['prompt']
    require(prompt['address']=='2764a2c' and readonly_string(image,0x2764a2c)==bytes.fromhex(prompt['bytes'])==b' Pay 2 life for how much black mana? (max %d, min %d)', 'Native Krrik prompt mismatch')
    require(engine['native_cancel_returns_without_cleanup'] is True and engine['native_minimum_is_display_only'] is True and engine['native_settlement_is_not_fizzle_gated'] is True,
            'Native Krrik cancellation/bounds/settlement behavior mismatch')
