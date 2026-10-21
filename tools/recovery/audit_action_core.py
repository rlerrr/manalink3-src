"""Audit native action/copy consumers, split function ranges and external aliases."""
import hashlib,re
from recover_action_core import ADDRESSES,PRIVATE,CONTROL_SITES,ACTION_TYPE_DECLARATION
from audit_source import function_body


def audit_component(component,manifest,sources,machine_bytes,image,root):
    def require(condition,message):
        if not condition:raise ValueError(message)
    entries={f['address']:f for f in manifest['accepted_functions'] if f.get('action_core_evidence')}
    require(set(entries)==set(ADDRESSES)==set(component['functions']),'Action/copy core membership mismatch')
    require({a for a,f in entries.items() if f['static']}==PRIVATE,'Action/copy private linkage mismatch')
    for address,detail in component['functions'].items():
        require(entries[address]['action_core_evidence']==detail,'Action/copy native evidence mismatch')
    require([len(component['parameters'][a]) for a in ADDRESSES]==[7,5,5,5,14,2,7,5],'Action/copy physical parameter count mismatch')
    require(component['actions_layout']==dict(bytes=4,encoding=7) and
            (root/'defs.h').read_text().count(ACTION_TYPE_DECLARATION)==1,
            'Native unsigned action representation mismatch')
    for address in ['2666bc0','2666f10','2666fe0','26670c0','26675a0']:
        require('actions_2022_t' in entries[address]['prototype'] and
                not re.search(r'\bactions_t\b',entries[address]['prototype']),
                'Native action prototype uses legacy enum representation')
    for address in PRIVATE:
        for p,expression in zip(component['parameters'][address][:3],['DW_OP_reg0 (r0)','DW_OP_reg2 (r2)','DW_OP_reg1 (r1)']):
            require(expression in str(p['location']),'Action/copy private register argument mismatch')
    require([p['location'] for p in component['parameters']['2666bc0'][3:]]==
            ['(DW_OP_fbreg: 0)','(DW_OP_fbreg: 4)','(DW_OP_fbreg: 8)','(DW_OP_fbreg: 12)'],
            'Native action worker stack argument mismatch')
    ranges=component['action_worker_ranges']
    require(component['dwarf_worker_ranges']==[dict(start='2666bc0',end='2666f0f'),dict(start='2710008',end='271000d')] and
            [(r['start'],r['end']) for r in ranges]==[('2666bc0','2666f0f'),('2710008','271000d')],
            'Native action worker disjoint ranges mismatch')
    for record in ranges:
        start,end=int(record['start'],16),int(record['end'],16)
        require(end-start==record['bytes'] and hashlib.sha256(machine_bytes(start,end-start)).hexdigest()==record['sha256'],
                'Native action worker hot/cold bytes mismatch')
    for address,name,typ,location in [('26670c0','marked','int[2][100]','(DW_OP_fbreg: -820)'),
                                    ('26670c0','mc','int[2]','(DW_OP_fbreg: -828)'),
                                    ('20277e0','prm','int[151]','(DW_OP_fbreg: -624)'),
                                    ('2686bb0','marked','char[2][151]','(DW_OP_fbreg: -322)')]:
        require(any(v['name']==name and v['type']==typ and v['location']==location for v in component['locals'][address]),
                'Native action/copy fixed-array layout mismatch')
    require(component['test_filter']==dict(bytes=216,not_me_offset=170,not_me_bytes=2) and
            component['target_filter']==dict(bytes=84),'Native action/copy record layout mismatch')
    require([s['address'] for s in component['control_sites']]==CONTROL_SITES,'Action/copy instruction membership mismatch')
    for site in component['control_sites']:
        data=bytes.fromhex(site['bytes'])
        require(machine_bytes(int(site['address'],16),len(data))==data,'Action/copy native instruction mismatch')
    controls={s['address']:s['bytes'] for s in component['control_sites']}
    require(controls['2666bcd']=='83fb38' and controls['2666bd0']=='7746',
            'Native action unsigned comparison mismatch')
    imp=component['abort_import'];image.pe.parse_data_directories()
    require(imp==dict(dll='msvcrt.dll',name='abort',iat='27b5484',thunk='2701eb8',thunk_bytes='ff2584547b02') and
            machine_bytes(0x2701eb8,6).hex()==imp['thunk_bytes'] and
            any(d.dll==b'msvcrt.dll' and any(f.name==b'abort' and f.address==0x27b5484 for f in d.imports)
                for d in image.pe.DIRECTORY_ENTRY_IMPORT),'Native action abort import mismatch')
    require(component['external_interfaces']=={n:dict(return_type='void',parameter_count=0) for n in ['receive_network_packet','send_network_packet']} and
            component['network_bindings']=={'receive_network_packet_2022':'4a70c0','send_network_packet_2022':'4a6bc0'},
            'Native network interface signature/address mismatch')
    require(component['network_alias']==dict(name='dblword_628C10',address='628c10',bytes=4,packet_tag_name='byte_628C0C',packet_tag_address='628c0c',packet_tag_bytes=1),
            'Native network packet field mismatch')
    linker=(root/'manalink.lds').read_text()
    for name,address in {**component['network_bindings'],'dblword_628C10':'628c10','byte_628C0C':'628c0c'}.items():
        require(re.search('_'+name+r'\s*=\s*0x'+address+r';',linker,re.IGNORECASE),'Native network interface linker binding mismatch')
    exposure=component['counter_number_exposure'];previous=exposure['previous_record']
    expected={**previous,'static':False,'prototype':previous['prototype'].removeprefix('static '),
              'reconstruction_body_sha256':exposure['body_sha256'],'public_counter_number_2022':exposure['native_symbol']}
    entry=next(f for f in manifest['accepted_functions'] if f['address']=='26d4810')
    require(entry==expected and previous['static'] and exposure['native_symbol']==dict(name='_get_updated_counters_number',storage=2),
            'Counter-number public interface exposure mismatch')
    require(hashlib.sha256(function_body(sources['functions/counters.c'],'counter_number_2022').replace('\r\n','\n').encode()).hexdigest()==exposure['body_sha256'],
            'Counter-number source body changed during exposure')
    require(any(c['target']=='26d4810' and c['name']=='counter_number_2022' for c in component['functions']['2666f10']['canonical_calls']),
            'Native action decoder counter-number callee mismatch')
