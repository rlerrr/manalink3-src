"""Inspect the GCC recovery DLL/debug pair without loading or executing either."""
import csv,json,re,struct
from pathlib import Path
import pefile
from elftools.dwarf.locationlists import LocationParser
from inspect_release import Image,value,inherited_value
from recover_temporary_exile import save_json,sha


def archive_members(path):
    data=path.read_bytes();assert data.startswith(b'!<arch>\n');at=8;members=0
    while at<len(data):
        header=data[at:at+60];assert len(header)==60 and header[58:60]==b'`\n'
        size=int(header[48:58]);name=header[:16].decode().strip();payload=data[at+60:at+60+size]
        if name not in {'/','//','/SYM64/'}:
            assert struct.unpack_from('<H',payload)[0]==0x14c,(path,name)
            members+=1
        at+=60+size+(size&1)
    assert members>0;return members


def literal_register_call_targets(insns):
    """Resolve short straight-line literal loads across unrelated register moves."""
    from capstone.x86_const import X86_OP_IMM,X86_OP_REG
    aliases={r:full for full,parts in {'eax':['eax','ax','al','ah'],'ebx':['ebx','bx','bl','bh'],
             'ecx':['ecx','cx','cl','ch'],'edx':['edx','dx','dl','dh'],
             'esi':['esi','si'],'edi':['edi','di'],'ebp':['ebp','bp'],'esp':['esp','sp']}.items() for r in parts}
    targets=set()
    for index,call in enumerate(insns):
        if call.mnemonic not in {'call','jmp'} or call.operands[0].type!=X86_OP_REG:continue
        register=aliases.get(call.reg_name(call.operands[0].reg),call.reg_name(call.operands[0].reg))
        for previous in reversed(insns[max(0,index-8):index]):
            if previous.mnemonic in {'call','ret'} or previous.mnemonic.startswith('j'):break
            written={aliases.get(previous.reg_name(r),previous.reg_name(r)) for r in previous.regs_access()[1]}
            if register not in written:continue
            if (previous.mnemonic=='mov' and len(previous.operands)==2 and previous.operands[0].type==X86_OP_REG and
                previous.operands[0].reg==call.operands[0].reg and previous.operands[1].type==X86_OP_IMM):targets.add(previous.operands[1].imm)
            break
    return targets


def main(canonical_helpers=None):
    root=Path(__file__).resolve().parents[2];cache=root/'.recovery';dll=root/'ManalinkEh.dll';dbg=root/'ManalinkEh.dbg'
    pe=pefile.PE(str(dll));pe.parse_data_directories();debug=Image(dbg.read_bytes())
    physical_helpers={}
    if canonical_helpers:
        from canonicalize_helpers import ledger_debug_view
        debug,physical_helpers=ledger_debug_view(debug,root)
    from verify_helper_domains import verify_compiled as verify_helper_domains
    helper_domains=verify_helper_domains(debug,root,source=canonical_helpers.get('native_interfaces',{}).get('native_domains',{}) if canonical_helpers else {})
    from verify_counter_trigger_domains import verify_compiled as verify_counter_domains
    from canonicalize_helpers import load as helper_proof
    counter_trigger_domains=verify_counter_domains(debug,helper_proof(root) or {},root)
    from verify_global_array_domains import verify_compiled as verify_global_arrays
    absolute_global_arrays=verify_global_arrays(debug,helper_proof(root) or {},root)
    from verify_source_spelling_edits import verify_compiled as verify_spelling_symbols
    source_spelling_symbols=verify_spelling_symbols(debug,helper_proof(root) or {},root)
    from verify_engine_boundaries import verify_compiled as verify_engine_boundaries
    executable_fallbacks=verify_engine_boundaries(debug,helper_proof(root) or {},root)
    from canonicalize_helpers import source_names
    source_spellings=source_names(root)
    def source_symbol(name):
        return '_'+source_spellings.get(name,name)
    from unify_scalar_helpers import verify_build as verify_helper_unification
    helper_unification=verify_helper_unification(debug,root)
    from restore_record_storage import verify_build as verify_record_storage
    canonical_record_storage=verify_record_storage(debug,root)
    assert pe.FILE_HEADER.Machine==debug.pe.FILE_HEADER.Machine==0x14c and pe.OPTIONAL_HEADER.Magic==0x10b
    exports={s.name.decode():s.address for s in pe.DIRECTORY_ENTRY_EXPORT.symbols}
    assert exports['DllEntry']==pe.OPTIONAL_HEADER.AddressOfEntryPoint==0x1000 and exports['CodeSectionEx']==0x1006
    assert pe.OPTIONAL_HEADER.ImageBase==0x2000000 and debug.sections['.debug_info'].SizeOfRawData>0
    names={s[0] for s in debug.symbols};manifest=json.loads((root/'docs/recovery-evidence/recovered-functions.json').read_text())
    assert '_is_not_dead' in names
    header=(root/'manalink.h').read_text();functions=(root/'functions/functions.c').read_text()
    assert 'extern char card_coded[30000/8];' in header and 'extern char* set_legacy_effect_name_addr;' in header
    assert functions.count('char *set_legacy_effect_name_addr = NULL;')==1 and 'char card_coded[30000/8] =' not in functions
    baseline=json.loads((cache/'batch70-before/docs/recovery-evidence/recovered-functions.json').read_text());before={f['address'] for f in baseline['accepted_functions']}
    public=[f for f in manifest['accepted_functions'] if f['address'] not in before and not f['static']]
    def public_symbol(f):
        if f.get('display_hook_evidence') and 'CALLBACK' in f['prototype']:
            assert f['name'] in {'wndproc_ShowListCard_hook','wndproc_FullCardClass_hook'}
            return '_'+f['name']+'@16'
        return '_'+f['name']
    assert all(public_symbol(f) in names for f in public),[f['name'] for f in public if public_symbol(f) not in names]
    migrated_leaves=[f for f in manifest['accepted_functions'] if f.get('leaves_test_evidence')]
    assert all('_'+f['name'] in names and '_'+f['name'].removesuffix('_2022') in names for f in migrated_leaves)
    reveal_callbacks=[f for f in manifest['accepted_functions'] if f.get('reveal_destroy_evidence') and f['static']]
    for callback in reveal_callbacks:
        records=[s for s in debug.symbols if s[0]=='_'+callback['name']]
        assert len(records)==1 and records[0][2]>0 and records[0][4]==3
    flash_private=[f for f in manifest['accepted_functions'] if f.get('flash_casting_evidence') and f['static']]
    for function in flash_private:
        records=[s for s in debug.symbols if s[0]=='_'+function['name']]
        assert len(records)==1 and records[0][2]>0 and records[0][4]==3
    flash_globals=[g for g in manifest['source_globals'] if g['name'] in {'vivien_current_event_2022','flash_current_event_2022'}]
    for global_ in flash_globals:
        records=[s for s in debug.symbols if s[0]==source_symbol(global_['name'])]
        assert len(records)==1 and records[0][2]>0 and records[0][4]==3
        assert debug.pe.sections[records[0][2]-1].Name.rstrip(b'\0')==b'.bss'
    snow_callbacks=[f for f in manifest['accepted_functions'] if f.get('snow_consumer_evidence') and f['static']]
    for callback in snow_callbacks:
        records=[s for s in debug.symbols if s[0]=='_'+callback['name']]
        assert len(records)==1 and records[0][2]>0 and records[0][4]==3
    embalm_workers=[f for f in manifest['accepted_functions'] if f.get('embalm_family_evidence') and f['static']]
    for worker in embalm_workers:
        records=[s for s in debug.symbols if s[0]=='_'+worker['name']]
        assert len(records)==1 and records[0][2]>0 and records[0][4]==3
    memorial_private=[f for f in manifest['accepted_functions'] if f.get('memorial_family_evidence') and f['static']]
    for worker in memorial_private:
        records=[s for s in debug.symbols if s[0]=='_'+worker['name']]
        assert len(records)==1 and records[0][2]>0 and records[0][4]==3
    spell_cost_private=[f for f in manifest['accepted_functions'] if f.get('spell_cost_evidence') and f['static']]
    spell_cost_symbols={}
    for worker in spell_cost_private:
        prefix='_'+worker['name']
        records=[s for s in debug.symbols if s[0]==prefix or re.fullmatch(re.escape(prefix)+r'\.constprop\.\d+',s[0])]
        assert len(records)==1 and records[0][2]>0 and records[0][3]==32 and records[0][4]==3
        assert debug.pe.sections[records[0][2]-1].Characteristics & 0x20000000
        spell_cost_symbols[worker['name']]=records[0][0]
    aer_free_helpers=[f for f in manifest['accepted_functions'] if f.get('aer_free_helper_evidence')]
    for helper in aer_free_helpers:
        records=[s for s in debug.symbols if s[0]=='_'+helper['name']]
        assert helper['static'] and len(records)==1 and records[0][2]>0 and records[0][3]==32 and records[0][4]==3
        assert debug.pe.sections[records[0][2]-1].Characteristics & 0x20000000
    card_batch_private=[f for f in manifest['accepted_functions'] if f.get('card_helper_batch_evidence') and f['static']]
    for worker in card_batch_private:
        prefix='_'+worker['name']
        records=[s for s in debug.symbols if s[0]==prefix or re.fullmatch(re.escape(prefix)+r'\.constprop\.\d+',s[0])]
        assert len(records)==1 and records[0][2]>0 and records[0][3]==32 and records[0][4]==3
        assert debug.pe.sections[records[0][2]-1].Characteristics & 0x20000000
    subtype_result=[g for g in manifest['source_globals'] if g['name']=='targeted_subtype_spell_result_2022']
    for global_ in subtype_result:
        records=[s for s in debug.symbols if s[0]==source_symbol(global_['name'])]
        assert len(records)==1 and records[0][2]>0 and records[0][4]==3
        assert debug.pe.sections[records[0][2]-1].Name.rstrip(b'\0')==b'.bss'
    combat_ai_private=[f for f in manifest['accepted_functions'] if f.get('combat_ai_consumer_evidence') and f['static']]
    for callback in combat_ai_private:
        records=[s for s in debug.symbols if s[0]=='_'+callback['name']]
        assert len(records)==1 and records[0][2]>0 and records[0][3]==32 and records[0][4]==3
        assert debug.pe.sections[records[0][2]-1].Characteristics & 0x20000000
    casting_reveal_private=[f for f in manifest['accepted_functions'] if f.get('casting_reveal_family_evidence') and f['static']]
    casting_reveal_symbols={};casting_reveal_inlined={}
    for worker in casting_reveal_private:
        prefix='_'+worker['name']
        records=[s for s in debug.symbols if s[0]==prefix or re.fullmatch(re.escape(prefix)+r'\.constprop\.\d+',s[0])]
        if not records and worker['address']=='24a6470' and worker['name']=='amplify_2022_impl':
            # GCC O2 inlines this once-used private helper into its only new caller.
            # Verify the concrete inline instance, origin, parameter and callees.
            dwarf=debug.dwarf();instances=[]
            for cu in dwarf.iter_CUs():
                if not str(value(cu.get_top_DIE(),'DW_AT_name')).replace('\\','/').endswith('cards/legions.c'):continue
                for die in cu.iter_DIEs():
                    if die.tag!='DW_TAG_inlined_subroutine' or inherited_value(die,'DW_AT_name')!=worker['name']:continue
                    parent=die.get_parent();origin=die.get_DIE_from_attribute('DW_AT_abstract_origin')
                    assert parent.tag=='DW_TAG_subprogram' and value(parent,'DW_AT_name')=='card_aven_warhawk'
                    assert origin.tag=='DW_TAG_subprogram' and value(origin,'DW_AT_inline')==1 and value(origin,'DW_AT_low_pc') is None
                    low=value(die,'DW_AT_low_pc');high=die.attributes['DW_AT_high_pc'];end=high.value if high.form=='DW_FORM_addr' else low+high.value
                    parent_low=value(parent,'DW_AT_low_pc');parent_high=parent.attributes['DW_AT_high_pc'];parent_end=parent_high.value if parent_high.form=='DW_FORM_addr' else parent_low+parent_high.value
                    assert parent_low<=low<end<=parent_end
                    children=list(die.iter_children());amount=next(c for c in children if c.tag=='DW_TAG_formal_parameter' and inherited_value(c,'DW_AT_name')=='amount')
                    locations=LocationParser(dwarf.location_lists()).parse_from_attribute(amount.attributes['DW_AT_location'],cu['version'],die=amount)
                    assert len(locations)==1 and locations[0].loc_expr==[0x31,0x9f]
                    assert value(cu.get_top_DIE(),'DW_AT_low_pc')+locations[0].begin_offset==low and value(cu.get_top_DIE(),'DW_AT_low_pc')+locations[0].end_offset==end
                    callees={inherited_value(c,'DW_AT_name') for c in children if c.tag=='DW_TAG_GNU_call_site'}
                    expected_callees={'in_play','add_state','new_default_test_definition_2022','get_card_name_by_instance','snprintf','reveal_cards_from_your_hand_2022','add_counters_from_source','remove_state'}
                    if canonical_helpers:
                        from canonicalize_helpers import names as canonical_names
                        logical={v:k for k,v in canonical_names(root).items()}
                        expected_callees={logical.get(n,n) for n in expected_callees}
                    assert callees==expected_callees,(callees,expected_callees)
                    native=pe.get_data(low-pe.OPTIONAL_HEADER.ImageBase,end-low);assert len(native)==end-low
                    section=next(s for s in pe.sections if s.VirtualAddress<=low-pe.OPTIONAL_HEADER.ImageBase<s.VirtualAddress+s.Misc_VirtualSize);assert section.Characteristics & 0x20000000
                    instances.append(dict(parent='card_aven_warhawk',address=hex(low),bytes=end-low,sha256=sha(native),amount=1,callees=sorted(callees)))
            assert len(instances)==1
            casting_reveal_inlined[worker['name']]=instances[0]
            continue
        assert len(records)==1 and records[0][2]>0 and records[0][3]==32 and records[0][4]==3,(worker['name'],records)
        assert debug.pe.sections[records[0][2]-1].Characteristics & 0x20000000
        casting_reveal_symbols[worker['name']]=records[0][0]
    grave_pump_private=[f for f in manifest['accepted_functions'] if f.get('grave_pump_consumer_evidence') and f['static']]
    for callback in grave_pump_private:
        records=[s for s in debug.symbols if s[0]=='_'+callback['name']]
        assert len(records)==1 and records[0][2]>0 and records[0][3]==32 and records[0][4]==3
        assert debug.pe.sections[records[0][2]-1].Characteristics & 0x20000000
    copy_exile_private=[f for f in manifest['accepted_functions'] if f.get('copy_exile_consumer_evidence') and f['static']]
    for callback in copy_exile_private:
        records=[s for s in debug.symbols if s[0]=='_'+callback['name']]
        assert len(records)==1 and records[0][2]>0 and records[0][3]==32 and records[0][4]==3
        assert debug.pe.sections[records[0][2]-1].Characteristics & 0x20000000
    graveyard_selection_private=[f for f in manifest['accepted_functions'] if f.get('graveyard_selection_evidence') and f['static']]
    graveyard_selection_symbols={}
    if graveyard_selection_private:
        wanted={(f['source'],f['name']) for f in graveyard_selection_private};owned={}
        for cu in debug.dwarf().iter_CUs():
            source=value(cu.get_top_DIE(),'DW_AT_name')
            if source not in {s for s,n in wanted}:continue
            for die in cu.get_top_DIE().iter_children():
                key=(source,inherited_value(die,'DW_AT_name'))
                if die.tag!='DW_TAG_subprogram' or key not in wanted:continue
                low=value(die,'DW_AT_low_pc');high=die.attributes.get('DW_AT_high_pc')
                if not low or not high:continue
                end=high.value if high.form=='DW_FORM_addr' else low+high.value
                owned.setdefault(key,[]).append((low,end))
        for callback in graveyard_selection_private:
            ranges=set(owned[(callback['source'],callback['name'])])
            candidates=[(s,low,end) for s in debug.symbols if s[0]=='_'+callback['name'] and s[2]>0
                        for low,end in ranges if debug.pe.OPTIONAL_HEADER.ImageBase+debug.pe.sections[s[2]-1].VirtualAddress+s[1]==low]
            assert len(candidates)==1
            record,low,end=candidates[0]
            assert record[3]==32 and record[4]==3 and end>low
            assert debug.pe.sections[record[2]-1].Characteristics & 0x20000000
            graveyard_selection_symbols[callback['name']]=dict(source=callback['source'],address=hex(low),end=hex(end),coff_name=record[0])
    hogaak_private=[f for f in manifest['accepted_functions'] if f.get('hogaak_family_evidence') and f['static']]
    for worker in hogaak_private:
        records=[s for s in debug.symbols if s[0]=='_'+worker['name']]
        assert len(records)==1 and records[0][2]>0 and records[0][3]==32 and records[0][4]==3
        assert debug.pe.sections[records[0][2]-1].Characteristics & 0x20000000
    letter_private=[f for f in manifest['accepted_functions'] if f.get('letter_effect_evidence') and f['static']]
    for helper in letter_private:
        records=[s for s in debug.symbols if s[0]=='_'+helper['name']]
        assert len(records)==1 and records[0][2]>0 and records[0][3]==32 and records[0][4]==3,(helper['name'],records)
        assert debug.pe.sections[records[0][2]-1].Characteristics & 0x20000000
    clock_buffers=[];clock_imports={}
    if any(f.get('clock_card_evidence') for f in manifest['accepted_functions']):
        from export_abi import type_of
        for cu in debug.dwarf().iter_CUs():
            if value(cu.get_top_DIE(),'DW_AT_name')!='functions/windows.c':continue
            for die in cu.iter_DIEs():
                if die.tag!='DW_TAG_variable' or value(die,'DW_AT_name')!='date_buffer' or inherited_value(die.get_parent(),'DW_AT_name')!='localtime_date':continue
                t=type_of(die);assert t.tag=='DW_TAG_array_type' and value(type_of(t),'DW_AT_byte_size')==1
                assert [value(c,'DW_AT_upper_bound') for c in t.iter_children() if c.tag=='DW_TAG_subrange_type']==[17]
                loc=die.attributes['DW_AT_location'].value;assert len(loc)==5 and loc[0]==3
                address=struct.unpack('<I',bytes(loc[1:]))[0]
                records=[s for s in debug.symbols if s[0].startswith('_date_buffer.') and s[2]>0 and debug.pe.OPTIONAL_HEADER.ImageBase+debug.pe.sections[s[2]-1].VirtualAddress+s[1]==address]
                assert len(records)==1 and records[0][4]==3
                section=debug.pe.sections[records[0][2]-1];assert section.Name.rstrip(b'\0')==b'.bss'
                assert records[0][1]+18<=section.Misc_VirtualSize
                clock_buffers.append(dict(address=hex(address),bytes=18,coff_name=records[0][0],source='functions/windows.c'))
        assert len(clock_buffers)==1
        expected={'GetLocalTime','QueryPerformanceCounter','QueryPerformanceFrequency','Sleep'}
        clock_imports={s.name.decode():d.dll.decode() for d in pe.DIRECTORY_ENTRY_IMPORT for s in d.imports if s.name and s.name.decode() in expected}
        assert set(clock_imports)==expected and all(d.lower()=='kernel32.dll' for d in clock_imports.values())
    pack_tables_verified=0;pack_utility_private_symbols={};pack_utility_unreferenced=[]
    interfaces=json.loads((root/'docs/recovery-evidence/interface-recovery.json').read_text())
    pack_proof=interfaces.get('pack_utilities_native_2022')
    if pack_proof:
        locations={}
        for name,record in pack_proof['native_tables']['tables'].items():
            symbols=[s for s in debug.symbols if s[0]=='_'+name and s[2]>0]
            assert len(symbols)==1 and symbols[0][4]==3,(name,symbols)
            symbol=symbols[0];address=debug.pe.OPTIONAL_HEADER.ImageBase+debug.pe.sections[symbol[2]-1].VirtualAddress+symbol[1]
            locations[name]=address
            data=pe.get_data(address-pe.OPTIONAL_HEADER.ImageBase,record['count']*record['element_bytes'])
            if name!='pack_contents':assert data.hex()==record['bytes'],name
        record=pack_proof['native_tables']['tables']['pack_contents'];address=locations['pack_contents']
        rows=list(struct.iter_unpack('<III',pe.get_data(address-pe.OPTIONAL_HEADER.ImageBase,198*12)))
        for row,expected in zip(rows,record['rows']):
            bn,bo=expected['booster'];sn,so=expected['starter']
            assert row==(expected['expansion'],locations[bn]+bo*8,locations[sn]+so*8)
        pack_tables_verified=len(locations);assert pack_tables_verified==44
        for helper in [f for f in manifest['accepted_functions'] if f.get('pack_utility_evidence') and f['static']]:
            symbols=[s for s in debug.symbols if s[0]=='_'+helper['name'] and s[2]>0]
            if symbols:
                assert len(symbols)==1 and symbols[0][3]==32 and symbols[0][4]==3
                assert debug.pe.sections[symbols[0][2]-1].Characteristics & 0x20000000
                pack_utility_private_symbols[helper['name']]=symbols[0][0]
            else:pack_utility_unreferenced.append(helper['name'])
    pack_weights_verified=0;pack_generator_private_symbols={};pack_once=[]
    generator_proof=interfaces.get('pack_generator_native_2022')
    if generator_proof:
        for name,record in generator_proof['native_tables']['tables'].items():
            symbols=[s for s in debug.symbols if s[0]=='_'+name and s[2]>0]
            assert len(symbols)==1 and symbols[0][4]==3,(name,symbols)
            symbol=symbols[0];address=debug.pe.OPTIONAL_HEADER.ImageBase+debug.pe.sections[symbol[2]-1].VirtualAddress+symbol[1]
            assert pe.get_data(address-pe.OPTIONAL_HEADER.ImageBase,record['count']*8).hex()==record['bytes'],name
            pack_weights_verified+=1
        assert pack_weights_verified==18 and not pack_utility_unreferenced and len(pack_utility_private_symbols)==2
        for helper in [f for f in manifest['accepted_functions'] if f.get('pack_generator_evidence') and f['static']]:
            symbols=[s for s in debug.symbols if s[0]=='_'+helper['name'] and s[2]>0]
            assert len(symbols)==1 and symbols[0][3]==32 and symbols[0][4]==3
            assert debug.pe.sections[symbols[0][2]-1].Characteristics & 0x20000000
            pack_generator_private_symbols[helper['name']]=symbols[0][0]
        assert len(pack_generator_private_symbols)==1
        from export_abi import type_of
        for cu in debug.dwarf().iter_CUs():
            if value(cu.get_top_DIE(),'DW_AT_name')!='functions/packs.c':continue
            for die in cu.iter_DIEs():
                if die.tag!='DW_TAG_variable' or value(die,'DW_AT_name')!='once' or inherited_value(die.get_parent(),'DW_AT_name')!='test_packs':continue
                from pack_native_tables import unqual
                assert value(unqual(type_of(die)),'DW_AT_byte_size')==4
                loc=die.attributes['DW_AT_location'].value;assert len(loc)==5 and loc[0]==3
                address=struct.unpack('<I',bytes(loc[1:]))[0]
                records=[s for s in debug.symbols if s[0].startswith('_once.') and s[2]>0 and debug.pe.OPTIONAL_HEADER.ImageBase+debug.pe.sections[s[2]-1].VirtualAddress+s[1]==address]
                assert len(records)==1 and records[0][4]==3
                section=debug.pe.sections[records[0][2]-1];assert section.Name.rstrip(b'\0')==b'.bss' and records[0][1]+4<=section.Misc_VirtualSize
                pack_once.append(dict(address=hex(address),bytes=4,coff_name=records[0][0],source='functions/packs.c',function='test_packs'))
        assert len(pack_once)==1
    pack_dialog_symbols={};pack_dialog_statics=[];pack_dialog_imports={};pack_template_verified=0
    chooser_proof=interfaces.get('pack_chooser_native_2022')
    if chooser_proof:
        records=[s for s in debug.symbols if s[0]=='_dlgproc_choose_a_pack@16']
        assert len(records)==1 and records[0][2]>0 and records[0][3]==32 and records[0][4]==3
        assert debug.pe.sections[records[0][2]-1].Characteristics & 0x20000000
        pack_dialog_symbols['dlgproc_choose_a_pack']=records[0][0]
        records=[s for s in debug.symbols if s[0]=='_choose_a_pack_template' and s[2]>0]
        assert len(records)==1 and records[0][4]==3
        symbol=records[0];address=debug.pe.OPTIONAL_HEADER.ImageBase+debug.pe.sections[symbol[2]-1].VirtualAddress+symbol[1]
        assert address%4==0 and pe.get_data(address-pe.OPTIONAL_HEADER.ImageBase,294).hex()==chooser_proof['template']['bytes']
        pack_template_verified=294
        for cu in debug.dwarf().iter_CUs():
            if value(cu.get_top_DIE(),'DW_AT_name')!='functions/packs.c':continue
            for die in cu.iter_DIEs():
                if die.tag!='DW_TAG_variable' or 'DW_AT_location' not in die.attributes:continue
                owner=inherited_value(die.get_parent(),'DW_AT_name');name=value(die,'DW_AT_name')
                if owner not in {'dlgproc_choose_a_pack','choose_packs'}:continue
                expected=chooser_proof['native_storage']['statics'].get(owner+'::'+str(name))
                if expected is None:continue
                loc=die.attributes['DW_AT_location'].value;assert len(loc)==5 and loc[0]==3
                from pack_native_tables import unqual
                assert value(unqual(type_of(die)),'DW_AT_byte_size')==4
                address=struct.unpack('<I',bytes(loc[1:]))[0]
                symbols=[s for s in debug.symbols if s[0].startswith('_'+name+'.') and s[2]>0 and debug.pe.OPTIONAL_HEADER.ImageBase+debug.pe.sections[s[2]-1].VirtualAddress+s[1]==address]
                assert len(symbols)==1 and symbols[0][4]==3
                section=debug.pe.sections[symbols[0][2]-1]
                if owner=='choose_packs':assert pe.get_data(address-pe.OPTIONAL_HEADER.ImageBase,4)==b'\xff'*4
                else:assert section.Name.rstrip(b'\0')==b'.bss' and symbols[0][1]+4<=section.Misc_VirtualSize
                pack_dialog_statics.append(dict(name=name,function=owner,address=hex(address),bytes=4,coff_name=symbols[0][0]))
        assert len(pack_dialog_statics)==7
        expected={v['name'] for v in chooser_proof['native_interfaces']['imports'].values()}
        pack_dialog_imports={s.name.decode():d.dll.decode() for d in pe.DIRECTORY_ENTRY_IMPORT for s in d.imports if s.name and s.name.decode() in expected}
        assert set(pack_dialog_imports)==expected and pack_dialog_imports['blt_expansion_symbol'].lower()=='drawcardlib.dll'
    alternate_filters={};alternate_vlas={}
    alternate_proof=interfaces.get('alternate_casting_native_2022')
    if alternate_proof:
        from export_abi import type_of
        from pack_native_tables import unqual
        added=[f for f in manifest['accepted_functions'] if f.get('alternate_casting_evidence')]
        card_names={f['name'] for f in added if f['name'].startswith('card_')}
        helper_names={h['name'] for h in alternate_proof['retained_helpers']}
        helper_names|={n+'_2022' for n in list(helper_names)}
        units={f['source'] for f in added}
        for cu in debug.dwarf().iter_CUs():
            source=value(cu.get_top_DIE(),'DW_AT_name')
            if source not in units:continue
            for die in cu.iter_DIEs():
                name=inherited_value(die,'DW_AT_name')
                if die.tag=='DW_TAG_subprogram' and name in helper_names and not value(die,'DW_AT_declaration',False):
                    if 'DW_AT_low_pc' not in die.attributes and 'DW_AT_ranges' not in die.attributes:continue
                    assert source=='functions/functions.c'
                    parameter=next(p for p in die.iter_children() if p.tag=='DW_TAG_formal_parameter' and inherited_value(p,'DW_AT_name')=='test')
                    pointer=unqual(type_of(parameter));assert pointer.tag=='DW_TAG_pointer_type'
                    size=value(unqual(type_of(pointer)),'DW_AT_byte_size');assert size==216
                    assert name not in alternate_filters;alternate_filters[name]=size
                    symbols=[s for s in debug.symbols if s[0]=='_'+name]
                    assert len(symbols)==1 and symbols[0][2]>0 and symbols[0][3]==32 and symbols[0][4]==2
                if die.tag!='DW_TAG_variable' or name!='playable':continue
                parent=die.get_parent()
                while parent is not None and parent.tag!='DW_TAG_subprogram':parent=parent.get_parent()
                owner=inherited_value(parent,'DW_AT_name') if parent is not None else None
                if owner not in card_names:continue
                t=unqual(type_of(die));assert t.tag=='DW_TAG_array_type' and value(unqual(type_of(t)),'DW_AT_byte_size')==4
                bounds=[c.attributes.get('DW_AT_upper_bound') for c in t.iter_children() if c.tag=='DW_TAG_subrange_type']
                assert len(bounds)==2 and bounds[0].form.startswith('DW_FORM_data') and bounds[0].value==1
                if bounds[1] is None:
                    assert owner in {'card_as_foretold','card_fires_of_invention','card_jodah_archmage_eternal'}
                else:assert bounds[1].form.startswith('DW_FORM_ref')
                assert owner not in alternate_vlas
                alternate_vlas[owner]=dict(source=source,rows=2,element_bytes=4,dynamic_bound_form=bounds[1].form if bounds[1] else None,dynamic_bound_reference=bounds[1].value if bounds[1] else None,
                    bound_evidence='DWARF runtime reference' if bounds[1] else 'GCC omits this hand-bound value; source declaration and original native probe/scale are checked by provenance')
        if canonical_helpers:
            from canonicalize_helpers import names as canonical_names
            logical={v:k for k,v in canonical_names(root).items()}
            expected_filters={logical.get(n,n) for n in helper_names}
            assert set(alternate_filters)==expected_filters,(alternate_filters,expected_filters)
        else:assert len(alternate_filters)==8
        assert len(alternate_vlas)==6
    exile_return_symbols={};exile_return_arrays={};exile_return_filters={}
    exile_return_proof=interfaces.get('exile_return_cards_native_2022')
    if exile_return_proof:
        from export_abi import type_of
        from pack_native_tables import unqual
        wanted={'create_dauthi_voidwalker_legacy','card_exiled_with_void_counter','card_exiled_with_aegis_counter'}
        for name in wanted:
            symbols=[s for s in debug.symbols if s[0]=='_'+name]
            assert len(symbols)==1 and symbols[0][2]>0 and symbols[0][3]==32 and symbols[0][4]==3
            assert debug.pe.sections[symbols[0][2]-1].Characteristics & 0x20000000
            exile_return_symbols[name]=symbols[0][0]
        fixed={('card_dauthi_voidwalker','exiled'):[30,2],('card_livio_oathsworn_sentinel','marked'):[99,1]}
        units={f['source'] for f in manifest['accepted_functions'] if f.get('exile_return_card_evidence')}
        for cu in debug.dwarf().iter_CUs():
            source=value(cu.get_top_DIE(),'DW_AT_name')
            if source not in units:continue
            for die in cu.iter_DIEs():
                if die.tag!='DW_TAG_variable':continue
                name=inherited_value(die,'DW_AT_name');parent=die.get_parent()
                while parent is not None and parent.tag!='DW_TAG_subprogram':parent=parent.get_parent()
                owner=inherited_value(parent,'DW_AT_name') if parent is not None else None
                key=(owner,name)
                if key in fixed or key==('card_izzet_chemister','ic'):
                    t=unqual(type_of(die));assert t.tag=='DW_TAG_array_type' and value(unqual(type_of(t)),'DW_AT_byte_size')==4
                    bounds=[c.attributes.get('DW_AT_upper_bound') for c in t.iter_children() if c.tag=='DW_TAG_subrange_type']
                    if key in fixed:assert [b.value for b in bounds]==fixed[key] and all(b.form.startswith('DW_FORM_data') for b in bounds)
                    else:assert len(bounds)==1 and (bounds[0] is None or bounds[0].form.startswith('DW_FORM_ref'))
                    exile_return_arrays[owner+'::'+name]=dict(source=source,upper_bounds=[b.value if b is not None else None for b in bounds],bound_forms=[b.form if b is not None else None for b in bounds],element_bytes=4)
                if owner=='card_izzet_chemister' and name in {'test','this_test'}:
                    assert value(unqual(type_of(die)),'DW_AT_byte_size')==216
                    exile_return_filters[name]=216
        assert len(exile_return_arrays)==3 and exile_return_filters==dict(test=216,this_test=216)
    selection_arrays={};selection_filters={}
    if interfaces.get('selection_storage_cards_native_2022'):
        from export_abi import type_of
        from pack_native_tables import unqual
        expected={('card_ormos_archive_keeper','searched'):[1,2],('card_ormos_archive_keeper','hand_copy'):None,('card_pelt_collector','array'):None}
        units={f['source'] for f in manifest['accepted_functions'] if f.get('selection_storage_card_evidence')}
        for cu in debug.dwarf().iter_CUs():
            source=value(cu.get_top_DIE(),'DW_AT_name')
            if source not in units:continue
            for die in cu.iter_DIEs():
                if die.tag!='DW_TAG_variable':continue
                name=inherited_value(die,'DW_AT_name');parent=die.get_parent()
                while parent is not None and parent.tag!='DW_TAG_subprogram':parent=parent.get_parent()
                owner=inherited_value(parent,'DW_AT_name') if parent is not None else None
                key=(owner,name)
                if key in expected:
                    t=unqual(type_of(die));assert t.tag=='DW_TAG_array_type' and value(unqual(type_of(t)),'DW_AT_byte_size')==4
                    bounds=[c.attributes.get('DW_AT_upper_bound') for c in t.iter_children() if c.tag=='DW_TAG_subrange_type']
                    if expected[key] is not None:assert [b.value for b in bounds]==expected[key] and all(b.form.startswith('DW_FORM_data') for b in bounds)
                    elif owner=='card_ormos_archive_keeper':assert len(bounds)==2 and bounds[0].value==1 and (bounds[1] is None or bounds[1].form.startswith('DW_FORM_ref'))
                    else:assert len(bounds)==1 and (bounds[0] is None or bounds[0].form.startswith('DW_FORM_ref'))
                    selection_arrays[owner+'::'+name]=dict(source=source,upper_bounds=[b.value if b is not None else None for b in bounds],bound_forms=[b.form if b is not None else None for b in bounds],element_bytes=4)
                if (owner,name) in {('card_ormos_archive_keeper','test'),('card_pelt_collector','test'),('card_mise','test')}:
                    assert value(unqual(type_of(die)),'DW_AT_byte_size')==216
                    selection_filters[owner+'::'+name]=216
        assert len(selection_arrays)==3 and len(selection_filters)==3
    final_arrays={};final_filters={};final_token={}
    if interfaces.get('final_card_handlers_native_2022'):
        from export_abi import type_of
        from pack_native_tables import unqual
        fixed={('card_tamiyo_collector_of_tales','counts'):([500],8),('card_nicol_bolas_dragon_god','activable'):([13],4),('card_aryel_knight_of_windgrace','buffer'):([74],1)}
        vlas={('card_nicol_bolas_god_pharaoh','removed'),('card_aryel_knight_of_windgrace','to_tap')}
        units={f['source'] for f in manifest['accepted_functions'] if f.get('final_card_handler_evidence')}
        for cu in debug.dwarf().iter_CUs():
            source=value(cu.get_top_DIE(),'DW_AT_name')
            if source not in units:continue
            for die in cu.iter_DIEs():
                if die.tag!='DW_TAG_variable':continue
                name=inherited_value(die,'DW_AT_name');parent=die.get_parent()
                while parent is not None and parent.tag!='DW_TAG_subprogram':parent=parent.get_parent()
                owner=inherited_value(parent,'DW_AT_name') if parent is not None else None
                key=(owner,name)
                if key in fixed or key in vlas:
                    t=unqual(type_of(die));assert t.tag=='DW_TAG_array_type'
                    size=value(unqual(type_of(t)),'DW_AT_byte_size');assert size==(fixed[key][1] if key in fixed else 4)
                    bounds=[c.attributes.get('DW_AT_upper_bound') for c in t.iter_children() if c.tag=='DW_TAG_subrange_type']
                    if key in fixed:assert [b.value for b in bounds]==fixed[key][0] and all(b.form.startswith('DW_FORM_data') for b in bounds)
                    else:assert len(bounds)==1 and bounds[0] is not None and bounds[0].form=='DW_FORM_exprloc' and bounds[0].value==[0x70,0x7f]
                    final_arrays[owner+'::'+name]=dict(source=source,upper_bounds=[b.value if b is not None else None for b in bounds],bound_forms=[b.form if b is not None else None for b in bounds],element_bytes=size)
                if owner in {'card_nicol_bolas_god_pharaoh','card_tamiyo_collector_of_tales'} and name in {'test','test2'}:
                    assert value(unqual(type_of(die)),'DW_AT_byte_size')==216
                    final_filters[owner+'::'+name]=216
                if key==('card_aryel_knight_of_windgrace','token'):
                    t=unqual(type_of(die));assert value(t,'DW_AT_byte_size')==76
                    fields={value(c,'DW_AT_name'):value(c,'DW_AT_data_member_location') for c in t.iter_children() if c.tag=='DW_TAG_member'}
                    assert fields['s_key_plus']==36
                    final_token=dict(bytes=76,special_keywords_offset=36)
        assert len(final_arrays)==5 and len(final_filters)==3 and final_token
    family_symbols={};family_vlas={};bruna_layouts={}
    if interfaces.get('legacy_helper_families_native_2022'):
        from export_abi import type_of
        from pack_native_tables import unqual
        for f in manifest['accepted_functions']:
            if not f.get('legacy_helper_family_evidence') or not f['static']:continue
            prefix='_'+f['name']
            syms=[s for s in debug.symbols if s[0]==prefix or re.fullmatch(re.escape(prefix)+r'\.(?:constprop|isra)\.\d+',s[0])]
            assert len(syms)==1 and syms[0][2]>0 and syms[0][3]==32 and syms[0][4]==3,(f['name'],syms)
            assert debug.pe.sections[syms[0][2]-1].Characteristics&0x20000000
            family_symbols[f['name']]=syms[0][0]
        units={f['source'] for f in manifest['accepted_functions'] if f.get('legacy_helper_family_evidence')}
        for cu in debug.dwarf().iter_CUs():
            source=value(cu.get_top_DIE(),'DW_AT_name')
            if source not in units:continue
            for die in cu.iter_DIEs():
                if die.tag!='DW_TAG_variable':continue
                name=inherited_value(die,'DW_AT_name');parent=die.get_parent()
                while parent is not None and parent.tag!='DW_TAG_subprogram':parent=parent.get_parent()
                owner=inherited_value(parent,'DW_AT_name') if parent is not None else None
                if owner in {'toshiro_effect','card_toshiro_umezawa_2022'} and name=='playable':
                    t=unqual(type_of(die));assert t.tag=='DW_TAG_array_type' and value(unqual(type_of(t)),'DW_AT_byte_size')==4
                    bounds=[c.attributes.get('DW_AT_upper_bound') for c in t.iter_children() if c.tag=='DW_TAG_subrange_type']
                    assert len(bounds)==2 and bounds[0].value==1 and bounds[0].form.startswith('DW_FORM_data')
                    assert bounds[1] is None or bounds[1].form.startswith('DW_FORM_ref') or bounds[1].form=='DW_FORM_exprloc'
                    family_vlas[owner]=dict(source=source,rows=2,element_bytes=4,upper_bounds=[b.value if b is not None else None for b in bounds],bound_forms=[b.form if b is not None else None for b in bounds])
                if owner=='bruna_ability' and name in {'td','this_test'}:
                    size=value(unqual(type_of(die)),'DW_AT_byte_size');assert size==(84 if name=='td' else 216)
                    bruna_layouts[name]=size
        assert len(family_symbols)==4 and len(family_vlas)==2 and bruna_layouts==dict(td=84,this_test=216)
    combat_foundation_symbols={};combat_foundation_inlined={};combat_foundation_arrays={};combat_foundation_filters={};combat_foundation_layout={};combat_foundation_exe={};combat_foundation_table_clear=False
    if interfaces.get('combat_foundations_native_2022'):
        from export_abi import type_of
        from pack_native_tables import unqual
        from capstone import Cs,CS_ARCH_X86,CS_MODE_32
        from capstone.x86_const import X86_OP_IMM,X86_OP_REG,X86_OP_MEM
        decoder=Cs(CS_ARCH_X86,CS_MODE_32);decoder.detail=True
        for f in manifest['accepted_functions']:
            if not f.get('combat_foundation_evidence') or not f['static']:continue
            prefix='_'+f['name'];records=[s for s in debug.symbols if s[0]==prefix or re.fullmatch(re.escape(prefix)+r'\.(?:constprop|isra)\.\d+',s[0])]
            if not records and f['name']=='enforce_minimum_blockers':continue
            assert len(records)==1 and records[0][2]>0 and records[0][3]==32 and records[0][4]==3,(f['name'],records)
            combat_foundation_symbols[f['name']]=records[0][0]
        for cu in debug.dwarf().iter_CUs():
            if value(cu.get_top_DIE(),'DW_AT_name')!='functions/engine.c':continue
            for die in cu.iter_DIEs():
                name=inherited_value(die,'DW_AT_name')
                if die.tag=='DW_TAG_inlined_subroutine' and name=='enforce_minimum_blockers':
                    parent=die.get_parent();assert parent.tag=='DW_TAG_subprogram' and value(parent,'DW_AT_name')=='human_assign_blockers_2022'
                    low=value(die,'DW_AT_low_pc');high=die.attributes['DW_AT_high_pc'];end=high.value if high.form=='DW_FORM_addr' else low+high.value
                    parent_low=value(parent,'DW_AT_low_pc');parent_high=parent.attributes['DW_AT_high_pc'];parent_end=parent_high.value if parent_high.form=='DW_FORM_addr' else parent_low+parent_high.value
                    assert parent_low<=low<end<=parent_end
                    count=next(c for c in die.iter_children() if c.tag=='DW_TAG_variable' and inherited_value(c,'DW_AT_name')=='num_blocking')
                    assert 'DW_AT_location' in count.attributes
                    t=unqual(type_of(count));assert t.tag=='DW_TAG_array_type' and value(unqual(type_of(t)),'DW_AT_byte_size')==4
                    assert [value(c,'DW_AT_upper_bound') for c in t.iter_children() if c.tag=='DW_TAG_subrange_type']==[150]
                    combat_foundation_inlined[name]=dict(parent='human_assign_blockers_2022',address=hex(low),bytes=end-low,count_array_bytes=604)
                if die.tag=='DW_TAG_subprogram' and name in {'setup_ai_combat_data','human_assign_blockers_2022','is_legal_block_2022'} and value(die,'DW_AT_low_pc') is not None:
                    low=value(die,'DW_AT_low_pc');high=die.attributes['DW_AT_high_pc'];end=high.value if high.form=='DW_FORM_addr' else low+high.value
                    insns=list(decoder.disasm(pe.get_data(low-pe.OPTIONAL_HEADER.ImageBase,end-low),low))
                    if name=='setup_ai_combat_data':
                        literals={o.imm for i in insns for o in i.operands if o.type==X86_OP_IMM}
                        assert {0x609164,1200}<=literals and any(i.mnemonic=='rep stosd' for i in insns)
                        activating=[o for i in insns for o in i.operands if o.type==X86_OP_MEM and o.mem.disp==0x7a3610]
                        assert len(activating)==3 and all(o.size==4 for o in activating)
                        combat_foundation_table_clear=True
                    else:
                        targets={i.operands[0].imm for i in insns if i.mnemonic in {'call','jmp'} and i.operands[0].type==X86_OP_IMM}
                        targets.update(literal_register_call_targets(insns))
                        for previous,call in zip(insns,insns[1:]):
                            if (call.mnemonic in {'call','jmp'} and call.operands[0].type==X86_OP_REG and
                                previous.mnemonic=='mov' and len(previous.operands)==2 and previous.operands[0].type==X86_OP_REG and
                                previous.operands[0].reg==call.operands[0].reg and previous.operands[1].type==X86_OP_IMM):targets.add(previous.operands[1].imm)
                        expected={0x499df0} if name=='is_legal_block_2022' else {0x472260,0x4350e0,0x435c80,0x435cd0,0x4d5d32}
                        assert expected<=targets,(name,expected-targets)
                        combat_foundation_exe[name]=[f'{a:x}' for a in sorted(expected)]
                if die.tag not in {'DW_TAG_variable','DW_TAG_typedef'}:continue
                parent=die.get_parent()
                while parent is not None and parent.tag!='DW_TAG_subprogram':parent=parent.get_parent()
                owner=inherited_value(parent,'DW_AT_name') if parent is not None else None
                if die.tag=='DW_TAG_typedef' and owner=='setup_ai_combat_data' and name=='ai_combat_data_t':
                    t=unqual(type_of(die));combat_foundation_layout=dict(bytes=value(t,'DW_AT_byte_size'),members={value(c,'DW_AT_name'):value(c,'DW_AT_data_member_location') for c in t.iter_children() if c.tag=='DW_TAG_member'})
                if owner=='enforce_minimum_blockers' and name=='num_blocking':
                    t=unqual(type_of(die));assert t.tag=='DW_TAG_array_type' and value(unqual(type_of(t)),'DW_AT_byte_size')==4
                    bounds=[value(c,'DW_AT_upper_bound') for c in t.iter_children() if c.tag=='DW_TAG_subrange_type'];assert bounds==[150]
                    combat_foundation_arrays[name]=dict(elements=151,element_bytes=4)
                if owner=='human_assign_blockers_2022' and name in {'td_choose_blocker','td_block_which_attacker'}:
                    assert value(unqual(type_of(die)),'DW_AT_byte_size')==84;combat_foundation_filters[name]=84
        assert len(combat_foundation_symbols)+len(combat_foundation_inlined)==2 and len(combat_foundation_arrays)==1 and len(combat_foundation_filters)==2 and len(combat_foundation_exe)==2 and combat_foundation_table_clear
        assert combat_foundation_layout==interfaces['combat_foundations_native_2022']['native_storage']['types']['ai_combat_data_t']
    draft_mana_symbols={};draft_mana_retained={};draft_mana_arrays={};draft_mana_exe={};draft_mana_bytes={};draft_mana_inline=[]
    if interfaces.get('draft_mana_native_2022'):
        from collections import Counter
        from export_abi import type_of
        from pack_native_tables import unqual
        from capstone import Cs,CS_ARCH_X86,CS_MODE_32
        from capstone.x86_const import X86_OP_IMM,X86_OP_MEM
        decoder=Cs(CS_ARCH_X86,CS_MODE_32);decoder.detail=True
        owners={'write_deck_file','card_draft_2022','how_much_life_can_i_pay_for_mana_2022','has_mana_multi_a_2022','has_mana_multi_2022'}
        for f in manifest['accepted_functions']:
            if not f.get('draft_mana_evidence'):continue
            prefix='_'+f['name'];syms=[s for s in debug.symbols if s[0]==prefix or (f['static'] and re.fullmatch(re.escape(prefix)+r'\.(?:constprop|isra)\.\d+',s[0]))]
            assert len(syms)==1 and syms[0][2]>0 and syms[0][3]==32 and syms[0][4]==(3 if f['static'] else 2),(f['name'],syms)
            draft_mana_symbols[f['name']]=syms[0][0]
        records=[s for s in debug.symbols if s[0]=='_pick_card'];assert len(records)==1 and records[0][2]>0 and records[0][3]==32 and records[0][4]==3
        draft_mana_retained['pick_card']=records[0][0]
        for cu in debug.dwarf().iter_CUs():
            if value(cu.get_top_DIE(),'DW_AT_name') not in {'functions/draft.c','functions/functions.c'}:continue
            for die in cu.iter_DIEs():
                name=inherited_value(die,'DW_AT_name')
                if die.tag=='DW_TAG_inlined_subroutine' and name=='how_much_life_can_i_pay_for_mana_2022':
                    parent=die.get_parent()
                    while parent is not None and parent.tag!='DW_TAG_subprogram':parent=parent.get_parent()
                    owner=inherited_value(parent,'DW_AT_name');assert owner in {'has_mana_multi_a_2022','has_mana_multi_2022','how_much_life_can_i_pay_for_mana_2022','how_much_life_can_i_pay'}
                    low=value(die,'DW_AT_low_pc');high=die.attributes['DW_AT_high_pc'];end=high.value if high.form=='DW_FORM_addr' else low+high.value
                    parent_low=value(parent,'DW_AT_low_pc');parent_high=parent.attributes['DW_AT_high_pc'];parent_end=parent_high.value if parent_high.form=='DW_FORM_addr' else parent_low+parent_high.value
                    assert parent_low<=low<end<=parent_end;draft_mana_inline.append(dict(parent=owner,address=hex(low),bytes=end-low))
                if die.tag=='DW_TAG_subprogram' and name in owners and value(die,'DW_AT_low_pc') is not None:
                    low=value(die,'DW_AT_low_pc');high=die.attributes['DW_AT_high_pc'];end=high.value if high.form=='DW_FORM_addr' else low+high.value
                    insns=list(decoder.disasm(pe.get_data(low-pe.OPTIONAL_HEADER.ImageBase,end-low),low))
                    targets={i.operands[0].imm for i in insns if i.mnemonic in {'call','jmp'} and i.operands[0].type==X86_OP_IMM};targets.update(literal_register_call_targets(insns))
                    expected={'write_deck_file':{0x466020},'card_draft_2022':{0x472120,0x471eb0,0x44e050},'has_mana_multi_a_2022':{0x402be0,0x49d510},'has_mana_multi_2022':{0x402be0}}.get(name)
                    if expected:
                        assert expected<=targets,(name,expected-targets);draft_mana_exe[name]=[f'{a:x}' for a in sorted(expected)]
                    if name in {'has_mana_multi_a_2022','has_mana_multi_2022'}:
                        stores=[];covered=set()
                        for i in insns:
                            if not i.mnemonic.startswith('mov') or not i.operands or i.operands[0].type!=X86_OP_MEM:continue
                            op=i.operands[0]
                            if op.mem.base==op.mem.index==0 and 0x55cf10<=op.mem.disp<0x55cf18:
                                assert 1<=op.size<=8 and op.mem.disp+op.size<=0x55cf18
                                covered.update(range(op.mem.disp,op.mem.disp+op.size));stores.append(dict(address=hex(i.address),destination=hex(op.mem.disp),bytes=op.size))
                        assert covered==set(range(0x55cf10,0x55cf18));draft_mana_bytes[name]=stores
                if die.tag!='DW_TAG_variable':continue
                parent=die.get_parent()
                while parent is not None and parent.tag!='DW_TAG_subprogram':parent=parent.get_parent()
                owner=inherited_value(parent,'DW_AT_name') if parent is not None else None
                if owner not in {'write_deck_file','card_draft_2022'}:continue
                t=unqual(type_of(die))
                if t.tag!='DW_TAG_array_type':continue
                size=value(unqual(type_of(t)),'DW_AT_byte_size');bounds=[c.attributes.get('DW_AT_upper_bound') for c in t.iter_children() if c.tag=='DW_TAG_subrange_type'];assert len(bounds)==1
                if name in {'picks','colors','packs','csvids','deck'}:
                    assert size==(8 if name=='deck' else 4)
                    if name=='picks':assert bounds[0] is not None and bounds[0].form=='DW_FORM_ref4'
                    else:assert bounds[0] is None
                else:
                    expected=(499,1) if name=='path' else (5,4) if name in {'exps','is_booster'} else (4,4) if name=='basics' else (bounds[0].value,1)
                    assert bounds[0] is not None and bounds[0].form.startswith('DW_FORM_data') and (bounds[0].value,size)==expected
                    if name=='buf':assert bounds[0].value in {599,49}
                key=f'{owner}::{name}:{inherited_value(die,"DW_AT_decl_line")}'
                draft_mana_arrays[key]=dict(name=name,element_bytes=size,upper_bound=bounds[0].value if bounds[0] else None,bound_form=bounds[0].form if bounds[0] else None)
        assert len(draft_mana_symbols)==5 and len(draft_mana_retained)==1 and len(draft_mana_arrays)==11 and len(draft_mana_exe)==4 and len(draft_mana_bytes)==2
        assert Counter((v['name'],v['element_bytes'],v['upper_bound']) for v in draft_mana_arrays.values() if v['name'] in {'path','exps','is_booster','basics','buf'})==Counter([('exps',4,5),('is_booster',4,5),('buf',1,599),('buf',1,49),('basics',4,4),('path',1,499)])
    display_symbols={};display_arrays={};display_types={};display_statics={};display_exe={};display_stdcall={};display_imports={};display_emptystr=False
    if interfaces.get('display_hooks_native_2022'):
        from export_abi import type_of
        from pack_native_tables import unqual
        from capstone import Cs,CS_ARCH_X86,CS_MODE_32
        from capstone.x86_const import X86_OP_IMM
        decoder=Cs(CS_ARCH_X86,CS_MODE_32);decoder.detail=True
        worker='draw_fullcard_special_effect_card_impl_2022'
        for f in manifest['accepted_functions']:
            if not f.get('display_hook_evidence'):continue
            name=public_symbol(f);syms=[s for s in debug.symbols if s[0]==name or (f['static'] and re.fullmatch(re.escape(name)+r'\.(?:constprop|isra)\.\d+',s[0]))]
            assert len(syms)==1 and syms[0][2]>0 and syms[0][3]==32 and syms[0][4]==(3 if f['static'] else 2),(f['name'],syms)
            display_symbols[f['name']]=syms[0][0]
        for cu in debug.dwarf().iter_CUs():
            if value(cu.get_top_DIE(),'DW_AT_name')!='functions/windows.c':continue
            for die in cu.iter_DIEs():
                name=inherited_value(die,'DW_AT_name')
                if die.tag=='DW_TAG_subprogram' and name in {worker,'wndproc_ShowListCard_hook','wndproc_FullCardClass_hook'} and value(die,'DW_AT_low_pc') is not None:
                    low=value(die,'DW_AT_low_pc');high=die.attributes['DW_AT_high_pc'];end=high.value if high.form=='DW_FORM_addr' else low+high.value
                    insns=list(decoder.disasm(pe.get_data(low-pe.OPTIONAL_HEADER.ImageBase,end-low),low))
                    targets={i.operands[0].imm for i in insns if i.mnemonic in {'call','jmp'} and i.operands[0].type==X86_OP_IMM};targets.update(literal_register_call_targets(insns))
                    expected={0x439010,0x439070,0x466e90} if name==worker else {0x4d34e0,0x46d9a0} if name=='wndproc_ShowListCard_hook' else {0x4389a0,0x438e80,0x438dc0,0x438b00,0x4d1950,0x4d1780,0x4cff20}
                    assert expected<=targets,(name,expected-targets)
                    display_exe[name]=[f'{a:x}' for a in sorted(expected)]
                    if name.startswith('wndproc_'):
                        returns=[i for i in insns if i.mnemonic=='ret'];assert returns and all(i.op_str=='0x10' for i in returns)
                        display_stdcall[name]=dict(symbol=display_symbols[name],callee_popped_bytes=16,returns=len(returns))
                if die.tag not in {'DW_TAG_variable','DW_TAG_typedef'}:continue
                t=unqual(type_of(die))
                if die.tag=='DW_TAG_typedef' and name=='inst_and_ext_t':
                    display_types[name]=dict(bytes=value(t,'DW_AT_byte_size'),members={value(c,'DW_AT_name'):value(c,'DW_AT_data_member_location') for c in t.iter_children() if c.tag=='DW_TAG_member'})
                if die.tag!='DW_TAG_variable':continue
                parent=die.get_parent()
                while parent is not None and parent.tag!='DW_TAG_subprogram':parent=parent.get_parent()
                owner=inherited_value(parent,'DW_AT_name') if parent is not None else None
                if name in {'kws_strs','spkws_strs'} or (owner=='wndproc_FullCardClass_hook' and name=='hack_force_expansion'):
                    loc=value(die,'DW_AT_location');assert isinstance(loc,list) and len(loc)==5 and loc[0]==3
                    address=int.from_bytes(bytes(loc[1:]),'little');section=next(s for s in pe.sections if s.VirtualAddress<=address-pe.OPTIONAL_HEADER.ImageBase<s.VirtualAddress+s.Misc_VirtualSize)
                    assert section.Name.rstrip(b'\0')==b'.bss'
                    size=value(t,'DW_AT_byte_size') if name=='hack_force_expansion' else 4*(1+value(next(c for c in t.iter_children() if c.tag=='DW_TAG_subrange_type'),'DW_AT_upper_bound'))
                    assert size==(4 if name=='hack_force_expansion' else 128)
                    display_statics[name]=dict(bytes=size,address=hex(address),section='.bss')
                key=(owner,name)
                fixed={('keywords_to_str','strs'):(64,4),('legacy_name_or_text','tmp'):(1999,1),(worker,'card_name'):(99,1),(worker,'rules_text'):(1999,1),(worker,'kwstr'):(999,1),(worker,'et'):(14,1),(worker,'pt'):(14,1)}
                if key in fixed:
                    assert t.tag=='DW_TAG_array_type';size=value(unqual(type_of(t)),'DW_AT_byte_size');bounds=[c.attributes.get('DW_AT_upper_bound') for c in t.iter_children() if c.tag=='DW_TAG_subrange_type']
                    assert len(bounds)==1 and bounds[0].form.startswith('DW_FORM_data') and (bounds[0].value,size)==fixed[key]
                    display_arrays[owner+'::'+name]=dict(elements=bounds[0].value+1,element_bytes=size)
                if key==(worker,'emptystr'):
                    assert value(t,'DW_AT_byte_size')==1;display_emptystr=True
        assert len(display_symbols)==7 and len(display_arrays)==7 and len(display_statics)==3 and len(display_exe)==3 and len(display_stdcall)==2 and display_emptystr
        assert display_types['inst_and_ext_t']==interfaces['display_hooks_native_2022']['native_storage']['types']['inst_and_ext_t']
        for module in pe.DIRECTORY_ENTRY_IMPORT:
            if module.dll.decode().lower()!='drawcardlib.dll':continue
            for imported in module.imports:
                if imported.name and imported.name.decode() in {'DrawFullCard','DrawFullCardEx','DrawSmallCardEx'}:display_imports[imported.name.decode()]=hex(imported.address)
        assert len(display_imports)==3
    combat_search_symbols={};combat_search_retained={};combat_search_arrays={};combat_search_types={};combat_search_exe={};combat_search_signed_division=False
    if interfaces.get('combat_search_native_2022'):
        from export_abi import type_of
        from pack_native_tables import unqual
        from capstone import Cs,CS_ARCH_X86,CS_MODE_32
        from capstone.x86_const import X86_OP_IMM
        decoder=Cs(CS_ARCH_X86,CS_MODE_32);decoder.detail=True
        for name in ['mod_pow_unblocked','ai_assign_one_blocker','enforce_minimum_blockers','qsort_compare_attacker_effectiveness','qsort_cardvalue_ascending']:
            records=[s for s in debug.symbols if s[0]=='_'+name]
            assert len(records)==1 and records[0][2]>0 and records[0][3]==32 and records[0][4]==3,(name,records)
            (combat_search_symbols if name in {'mod_pow_unblocked','ai_assign_one_blocker'} else combat_search_retained)[name]=records[0][0]
        division=next(s for s in debug.symbols if s[0]=='___divdi3' and s[2]>0)
        division_address=pe.OPTIONAL_HEADER.ImageBase+debug.pe.sections[division[2]-1].VirtualAddress+division[1]
        for cu in debug.dwarf().iter_CUs():
            if value(cu.get_top_DIE(),'DW_AT_name')!='functions/engine.c':continue
            for die in cu.iter_DIEs():
                name=inherited_value(die,'DW_AT_name')
                if die.tag=='DW_TAG_subprogram' and name in {'ai_assign_one_blocker','ai_assign_blockers_2022','ai_choose_attackers'} and value(die,'DW_AT_low_pc') is not None:
                    low=value(die,'DW_AT_low_pc');high=die.attributes['DW_AT_high_pc'];end=high.value if high.form=='DW_FORM_addr' else low+high.value
                    insns=list(decoder.disasm(pe.get_data(low-pe.OPTIONAL_HEADER.ImageBase,end-low),low))
                    targets={i.operands[0].imm for i in insns if i.mnemonic in {'call','jmp'} and i.operands[0].type==X86_OP_IMM}
                    targets.update(literal_register_call_targets(insns))
                    if name=='ai_assign_one_blocker':
                        assert division_address in targets;combat_search_signed_division=True
                    else:
                        expected={0x4af6f0,0x435c80,0x435cd0,0x437e20} if name=='ai_assign_blockers_2022' else {0x4b4350,0x4af6f0,0x498e30,0x498e70}
                        assert expected<=targets,(name,expected-targets)
                        combat_search_exe[name]=[f'{a:x}' for a in sorted(expected)]
                if die.tag not in {'DW_TAG_variable','DW_TAG_typedef'}:continue
                parent=die.get_parent()
                while parent is not None and parent.tag!='DW_TAG_subprogram':parent=parent.get_parent()
                owner=inherited_value(parent,'DW_AT_name') if parent is not None else None
                if owner not in {'ai_assign_one_blocker','ai_choose_attackers'}:continue
                t=unqual(type_of(die))
                if die.tag=='DW_TAG_typedef' and name in {'PotentialAttacker','CardPower','ai_combat_data_t'}:
                    combat_search_types[name]=dict(bytes=value(t,'DW_AT_byte_size'),members={value(c,'DW_AT_name'):value(c,'DW_AT_data_member_location') for c in t.iter_children() if c.tag=='DW_TAG_member'})
                if die.tag!='DW_TAG_variable' or t.tag!='DW_TAG_array_type':continue
                bounds=[c.attributes.get('DW_AT_upper_bound') for c in t.iter_children() if c.tag=='DW_TAG_subrange_type'];assert len(bounds)==1 and bounds[0] is not None
                size=value(unqual(type_of(t)),'DW_AT_byte_size')
                if name=='outflanked':
                    assert size==4 and bounds[0].form=='DW_FORM_ref4'
                    bound=next(c for c in t.iter_children() if c.tag=='DW_TAG_subrange_type').get_DIE_from_attribute('DW_AT_upper_bound')
                    assert bound.tag=='DW_TAG_variable' and value(bound,'DW_AT_artificial') and 'DW_AT_location' in bound.attributes
                else:
                    expected=(149,12) if name=='pot_atck' else (149,1) if name in {'card_to_blocker_pt_idx','card_to_attacker_pt_idx'} else (7,8) if name=='cardpow' else (15,4)
                    assert bounds[0].form.startswith('DW_FORM_data') and (bounds[0].value,size)==expected,(name,bounds,size)
                key=f'{owner}::{name}:{inherited_value(die,"DW_AT_decl_line")}'
                combat_search_arrays[key]=dict(element_bytes=size,upper_bound=bounds[0].value,bound_form=bounds[0].form)
        assert len(combat_search_arrays)==23 and len(combat_search_exe)==2 and combat_search_signed_division
        assert combat_search_types['PotentialAttacker']==dict(bytes=12,members=dict(card=0,effectiveness=4,bands=8,one_of_best_7=9))
        assert combat_search_types['CardPower']==dict(bytes=8,members=dict(card=0,pow=4))
        assert combat_search_types['ai_combat_data_t']==interfaces['combat_search_native_2022']['native_storage']['types']['ai_combat_data_t']
    stack_utility_arrays={};stack_utility_boundaries={};stack_utility_rogue_layout={}
    if interfaces.get('stack_utility_helpers_native_2022'):
        from export_abi import type_of
        from pack_native_tables import unqual
        from capstone import Cs,CS_ARCH_X86,CS_MODE_32
        from capstone.x86_const import X86_OP_IMM, X86_OP_REG
        decoder=Cs(CS_ARCH_X86,CS_MODE_32);decoder.detail=True
        expected_exe={0x430150,0x4301e0,0x430200,0x435940,0x436700,0x4373b0,0x439690,0x436a20,0x471b60,0x4779f0,0x4a7d80,0x4d5d32}
        for cu in debug.dwarf().iter_CUs():
            source=value(cu.get_top_DIE(),'DW_AT_name')
            if source not in {'functions/engine.c','functions/rules_engine.c','functions/events.c'}:continue
            for die in cu.iter_DIEs():
                name=inherited_value(die,'DW_AT_name')
                if die.tag=='DW_TAG_subprogram' and name in {'put_card_on_stack_2022','allow_response_to_trigger'} and value(die,'DW_AT_low_pc') is not None:
                    low=value(die,'DW_AT_low_pc');high=die.attributes['DW_AT_high_pc'];end=high.value if high.form=='DW_FORM_addr' else low+high.value
                    code=pe.get_data(low-pe.OPTIONAL_HEADER.ImageBase,end-low)
                    insns=list(decoder.disasm(code,low))
                    targets={i.operands[0].imm for i in insns if i.mnemonic in {'call','jmp'} and i.operands[0].type==X86_OP_IMM}
                    for previous,call in zip(insns,insns[1:]):
                        if (call.mnemonic in {'call','jmp'} and call.operands[0].type==X86_OP_REG and
                            previous.mnemonic=='mov' and len(previous.operands)==2 and
                            previous.operands[0].type==X86_OP_REG and previous.operands[0].reg==call.operands[0].reg and
                            previous.operands[1].type==X86_OP_IMM):targets.add(previous.operands[1].imm)
                    expected=expected_exe if name=='put_card_on_stack_2022' else {0x439690,0x436a20}
                    assert expected<=targets,(name,expected-targets)
                    stack_utility_boundaries[name]=[f'{a:x}' for a in sorted(expected)]
                if die.tag not in {'DW_TAG_variable','DW_TAG_typedef'}:continue
                parent=die.get_parent()
                while parent is not None and parent.tag!='DW_TAG_subprogram':parent=parent.get_parent()
                owner=inherited_value(parent,'DW_AT_name') if parent is not None else None
                if die.tag=='DW_TAG_typedef' and owner=='random_rogue' and name=='rogue_t':
                    t=unqual(type_of(die));stack_utility_rogue_layout=dict(bytes=value(t,'DW_AT_byte_size'),members={value(c,'DW_AT_name'):value(c,'DW_AT_data_member_location') for c in t.iter_children() if c.tag=='DW_TAG_member'})
                key=(owner,name)
                fixed={('put_card_on_stack_2022','str'):(499,1),('put_card_on_stack_2022','trace'):(299,1),('limit_allowed_in_deck','unlimited_list'):(16,4)}
                if key not in fixed and key!=('random_rogue','ok_rogues'):continue
                t=unqual(type_of(die));assert t.tag=='DW_TAG_array_type'
                size=value(unqual(type_of(t)),'DW_AT_byte_size');bounds=[c.attributes.get('DW_AT_upper_bound') for c in t.iter_children() if c.tag=='DW_TAG_subrange_type']
                assert len(bounds)==1 and bounds[0] is not None
                if key in fixed:
                    assert size==fixed[key][1] and bounds[0].value==fixed[key][0] and bounds[0].form.startswith('DW_FORM_data')
                else:
                    assert size==4 and bounds[0].form=='DW_FORM_ref4'
                    subrange=next(c for c in t.iter_children() if c.tag=='DW_TAG_subrange_type')
                    bound=subrange.get_DIE_from_attribute('DW_AT_upper_bound')
                    assert bound.tag=='DW_TAG_variable' and value(bound,'DW_AT_artificial')
                    assert value(unqual(type_of(bound)),'DW_AT_byte_size')==4 and 'DW_AT_location' in bound.attributes
                    locations=LocationParser(debug.dwarf().location_lists()).parse_from_attribute(bound.attributes['DW_AT_location'],cu['version'],die=bound)
                    assert locations
                stack_utility_arrays[owner+'::'+name]=dict(element_bytes=size,upper_bound=bounds[0].value,bound_form=bounds[0].form)
                if key==('limit_allowed_in_deck','unlimited_list'):
                    loc=value(die,'DW_AT_location');assert loc[0]==3 and len(loc)==5
                    address=int.from_bytes(bytes(loc[1:]),'little')
                    native=interfaces['stack_utility_helpers_native_2022']['native_storage']['unlimited_list']
                    assert list(struct.unpack('<17i',pe.get_data(address-pe.OPTIONAL_HEADER.ImageBase,68)))==native
        assert len(stack_utility_arrays)==4 and len(stack_utility_boundaries)==2
        assert stack_utility_rogue_layout==interfaces['stack_utility_helpers_native_2022']['native_storage']['rogue_layout']
        linker=(root/'manalink.lds').read_text()
        assert '_put_card_on_stack = 0x4338F0;' in linker and '_put_card_on_stack2 = 0x433850;' in linker
    havengul_symbols={};havengul_storage={};havengul_boundaries={};havengul_attachment_symbol=None
    if interfaces.get('havengul_utility_helpers_native_2022'):
        from export_abi import type_of
        from pack_native_tables import unqual
        from capstone import Cs,CS_ARCH_X86,CS_MODE_32
        from capstone.x86_const import X86_OP_IMM
        decoder=Cs(CS_ARCH_X86,CS_MODE_32);decoder.detail=True
        for f in manifest['accepted_functions']:
            if not f.get('havengul_utility_evidence') or not f['static']:continue
            prefix='_'+f['name']
            syms=[s for s in debug.symbols if s[0]==prefix or re.fullmatch(re.escape(prefix)+r'\.(?:constprop|isra)\.\d+',s[0])]
            assert len(syms)==1 and syms[0][2]>0 and syms[0][3]==32 and syms[0][4]==3,(f['name'],syms)
            assert debug.pe.sections[syms[0][2]-1].Characteristics&0x20000000
            havengul_symbols[f['name']]=syms[0][0]
        attached=[s for s in debug.symbols if s[0]=='_havengul_lich_legacy']
        assert len(attached)==1 and attached[0][2]>0 and attached[0][3]==32 and attached[0][4]==3
        assert debug.pe.sections[attached[0][2]-1].Characteristics&0x20000000
        havengul_attachment_symbol=attached[0][0]
        for cu in debug.dwarf().iter_CUs():
            source=value(cu.get_top_DIE(),'DW_AT_name')
            if source not in {'cards/dark_ascension.c','functions/tutors.c','functions/produce_mana.c'}:continue
            for die in cu.iter_DIEs():
                if die.tag=='DW_TAG_subprogram' and inherited_value(die,'DW_AT_name') in {'mana_producer_fixed_2022','show_deck_force_expansion_2022'} and value(die,'DW_AT_low_pc') is not None:
                    name=inherited_value(die,'DW_AT_name');low=value(die,'DW_AT_low_pc');high=die.attributes['DW_AT_high_pc'];end=high.value if high.form=='DW_FORM_addr' else low+high.value
                    expected={0x47b6c0} if name=='mana_producer_fixed_2022' else {0x471d60,0x479360}
                    code=pe.get_data(low-pe.OPTIONAL_HEADER.ImageBase,end-low)
                    targets={i.operands[0].imm for i in decoder.disasm(code,low) if i.mnemonic in {'call','jmp'} and i.operands[0].type==X86_OP_IMM}
                    assert expected<=targets,(name,targets)
                    havengul_boundaries[name]=dict(exe_targets=[f'{x:x}' for x in sorted(expected)])
                if die.tag!='DW_TAG_variable':continue
                name=inherited_value(die,'DW_AT_name');parent=die.get_parent()
                while parent is not None and parent.tag!='DW_TAG_subprogram':parent=parent.get_parent()
                owner=inherited_value(parent,'DW_AT_name') if parent is not None else None
                if owner!='card_havengul_lich_2022':continue
                if name=='act_array':
                    t=unqual(type_of(die));assert t.tag=='DW_TAG_array_type' and value(unqual(type_of(t)),'DW_AT_byte_size')==4
                    bounds=[value(c,'DW_AT_upper_bound') for c in t.iter_children() if c.tag=='DW_TAG_subrange_type'];assert bounds==[7]
                    havengul_storage[name]=dict(elements=8,element_bytes=4)
                elif name in {'test','this_test'}:
                    assert value(unqual(type_of(die)),'DW_AT_byte_size')==216
                    havengul_storage[name]=216
        assert len(havengul_symbols)==5 and len(havengul_storage)==3 and len(havengul_boundaries)==2
    from verify_mana_payment_build import verify as verify_mana_payment
    mana_payment_build = verify_mana_payment(debug, pe, interfaces)
    from verify_stack_activation_build import verify as verify_stack_activation
    stack_activation_build = verify_stack_activation(debug, pe, interfaces)
    from verify_engine_state_build import verify as verify_engine_state
    engine_state_build = verify_engine_state(debug, pe, interfaces)
    from verify_turn_flow_build import verify as verify_turn_flow
    turn_flow_build = verify_turn_flow(debug, pe, interfaces)
    from verify_damage_prevention_build import verify as verify_damage_prevention
    damage_prevention_build = verify_damage_prevention(debug, pe, interfaces)
    from verify_card_state_build import verify as verify_card_state
    card_state_build = verify_card_state(debug, pe, interfaces)
    from verify_card_queries_build import verify as verify_card_queries
    card_queries_build = verify_card_queries(debug, pe, interfaces)
    from verify_type_animation_build import verify as verify_type_animation
    type_animation_build = verify_type_animation(debug, pe, interfaces)
    from verify_tap_ai_build import verify as verify_tap_ai
    tap_ai_build = verify_tap_ai(debug, pe, interfaces)
    from verify_name_type_build import verify as verify_name_type
    name_type_build = verify_name_type(debug, pe, interfaces)
    from verify_color_core_build import verify as verify_color_core
    color_core_build = verify_color_core(debug, pe, interfaces)
    from verify_color_hacks_build import verify as verify_color_hacks
    color_hacks_build = verify_color_hacks(debug, pe, interfaces)
    from verify_mana_sources_build import verify as verify_mana_sources
    mana_sources_build = verify_mana_sources(debug, pe, interfaces)
    from verify_mana_producers_build import verify as verify_mana_producers
    mana_producers_build = verify_mana_producers(debug, pe, interfaces)
    from verify_ai_snapshots_build import verify as verify_ai_snapshots
    ai_snapshots_build = verify_ai_snapshots(debug, pe, interfaces)
    from verify_mana_ai_phase_build import verify as verify_mana_ai_phase
    mana_ai_phase_build = verify_mana_ai_phase(debug, pe, interfaces)
    migrated_eligibility=[f for f in manifest['accepted_functions'] if f.get('flag_target_consumer_evidence') and f.get('previous_recovery')]
    for helper in migrated_eligibility:
        for name in [helper['name'],helper['previous_recovery']['name']]:
            records=[s for s in debug.symbols if s[0]=='_'+name]
            assert len(records)==1 and records[0][2]>0 and records[0][3]==32 and records[0][4]==2
            assert debug.pe.sections[records[0][2]-1].Characteristics & 0x20000000
    flag_private=[f for f in manifest['accepted_functions'] if f.get('flag_target_consumer_evidence') and f['static']]
    for helper in flag_private:
        records=[s for s in debug.symbols if s[0]=='_'+helper['name']]
        assert len(records)==1 and records[0][2]>0 and records[0][3]==32 and records[0][4]==3
        assert debug.pe.sections[records[0][2]-1].Characteristics & 0x20000000
    cycling_globals=[g for g in manifest['source_globals'] if g['name']=='card_is_being_cycled']
    for global_ in cycling_globals:
        records=[s for s in debug.symbols if s[0]=='_'+global_['name']]
        assert len(records)==1 and records[0][2]>0 and records[0][4]==2
        section=debug.pe.sections[records[0][2]-1]
        assert section.Name.rstrip(b'\0')==b'.bss' and section.SizeOfRawData==0
        assert global_['zero_initialized'] and 'extern int card_is_being_cycled;' in header
    drawcard_config_imports=[]
    if any(f.get('mode_storage_consumer_evidence') for f in manifest['accepted_functions']):
        drawcard_config_imports=[i for d in pe.DIRECTORY_ENTRY_IMPORT if d.dll.decode().lower()=='drawcardlib.dll'
                                for i in d.imports if i.name==b'get_config_data']
        assert len(drawcard_config_imports)==1
        assert '__attribute__((dllimport)) void get_config_data(int config, int item, int *result);' in header
        assert (root/'Drawcardlib.def').read_text().splitlines().count('  get_config_data')==1
    mode_counter_thunks={}
    for helper in [f for f in manifest['accepted_functions'] if f.get('mode_storage_consumer_evidence') and f['address']=='2692fc0']:
        symbols={}
        for name in [helper['name'],'raw_counter_activated_ability']:
            records=[s for s in debug.symbols if s[0]=='_'+name]
            assert len(records)==1 and records[0][2]>0 and records[0][3]==32 and records[0][4]==2
            record=records[0];symbols[name]=debug.pe.OPTIONAL_HEADER.ImageBase+debug.pe.sections[record[2]-1].VirtualAddress+record[1]
        address=symbols[helper['name']];target=symbols['raw_counter_activated_ability'];code=pe.get_data(address-pe.OPTIONAL_HEADER.ImageBase,5)
        from unify_scalar_helpers import shared_recovered_names
        if helper['name'] in shared_recovered_names():
            assert address==target
            concrete=[]
            for cu in debug.dwarf().iter_CUs():
                if value(cu.get_top_DIE(),'DW_AT_name')!=helper['source']:continue
                for die in cu.iter_DIEs():
                    if die.tag!='DW_TAG_subprogram' or inherited_value(die,'DW_AT_name')!=helper['name'] or value(die,'DW_AT_low_pc')!=address:continue
                    high=die.attributes['DW_AT_high_pc'];concrete.append(high.value if high.form=='DW_FORM_addr' else address+high.value)
            assert len(set(concrete))==1
            end=concrete[0]
            from capstone import Cs,CS_ARCH_X86,CS_MODE_32
            from capstone.x86_const import X86_OP_MEM,X86_OP_IMM
            counter_decoder=Cs(CS_ARCH_X86,CS_MODE_32);counter_decoder.detail=True
            insns=list(counter_decoder.disasm(pe.get_data(address-pe.OPTIONAL_HEADER.ImageBase,end-address),address))
            assert sum(i.size for i in insns)==end-address
            instance=next(s for s in debug.symbols if s[0]=='_get_card_instance')
            instance_at=instance[1] if instance[2]==-1 else debug.pe.OPTIONAL_HEADER.ImageBase+debug.pe.sections[instance[2]-1].VirtualAddress+instance[1]
            assert [i.operands[0].imm for i in insns if i.mnemonic=='call' and i.operands[0].type==X86_OP_IMM]==[instance_at]
            stores=[i for i in insns if i.mnemonic=='mov' and i.operands[0].type==X86_OP_MEM and i.operands[0].size==4 and i.operands[0].mem.base and i.operands[0].mem.disp==108 and i.operands[1].type==X86_OP_IMM and i.operands[1].imm & 0xffffffff==0xffffffff]
            assert len(stores)==1
            native_counter=Image((cache/'2022-ManalinkEh.dll').read_bytes())
            assert end-address==33 and stores[0].bytes.hex()==native_counter.pe.get_data(0x2692fd8-0x2000000,7).hex()=='c7406cffffffff'
            mode_counter_thunks[helper['name']]=dict(address=hex(address),target=hex(target),representation='canonical body resolved through ledger name' if canonical_helpers else 'same-address alias',body_bytes=end-address,internal_id_store=dict(address=hex(stores[0].address),bytes=stores[0].bytes.hex(),offset=108,value=-1,native_instruction='0x2692fd8'))
            continue
        assert code[0]==0xe9 and address+5+struct.unpack_from('<i',code,1)[0]==target
        mode_counter_thunks[helper['name']]=dict(address=hex(address),target=hex(target),bytes=code.hex(),retained_target='raw_counter_activated_ability')
    outlined_proliferate={}
    for helper in [f for f in manifest['accepted_functions'] if f.get('library_vla_consumer_evidence') and f['address']=='2438530']:
        prefix='_'+helper['name'];records=[s for s in debug.symbols if s[0] in {prefix,prefix+'.part.0'}]
        assert len(records)==2 and {s[4] for s in records}=={2,3} and all(s[2]>0 and s[3]==32 for s in records)
        ranges={}
        for cu in debug.dwarf().iter_CUs():
            if value(cu.get_top_DIE(),'DW_AT_name')!=helper['source']:continue
            for die in cu.iter_DIEs():
                if die.tag!='DW_TAG_subprogram' or inherited_value(die,'DW_AT_name')!=helper['name']:continue
                low=value(die,'DW_AT_low_pc');high=die.attributes.get('DW_AT_high_pc')
                if low and high:ranges[low]=high.value if high.form=='DW_FORM_addr' else low+high.value
        lows={debug.pe.OPTIONAL_HEADER.ImageBase+debug.pe.sections[s[2]-1].VirtualAddress+s[1] for s in records}
        assert lows==set(ranges) and all(ranges[low]>low for low in lows)
        outlined_proliferate[helper['name']]=[dict(coff_name=s[0],address=hex(debug.pe.OPTIONAL_HEADER.ImageBase+debug.pe.sections[s[2]-1].VirtualAddress+s[1]),storage=s[4]) for s in records]
    storm_state=0
    if any(f.get('storm_family_evidence') for f in manifest['accepted_functions']):
        records=[s for s in debug.symbols if s[0]=='_rules_engine_card']
        assert len(records)==1 and records[0][2]>0 and records[0][4]==3
        section=debug.pe.sections[records[0][2]-1]
        assert section.Name.rstrip(b'\0')==b'.data'
        assert pe.get_data(section.VirtualAddress+records[0][1],8)==b'\xff'*8
        storm_state=1
    for name in ['_card_coded','_set_legacy_effect_name_addr']:
        records=[s for s in debug.symbols if s[0]==name];assert len(records)==1 and records[0][4]==2
        if name=='_card_coded':
            address=int(re.search(r'(?m)^_card_coded\s*=\s*(0x[0-9a-fA-F]+)',(root/'manalink.lds').read_text())[1],16)
            assert records[0][2]==-1 and records[0][1]==address
        else:assert records[0][2]>0
    raw_log=(cache/'build-gcc93/build.log').read_bytes()
    log=raw_log.decode('utf-16' if raw_log.startswith((b'\xff\xfe',b'\xfe\xff')) else 'utf-8-sig')
    link=next(line for line in log.splitlines() if line.startswith('g++ -m32 -shared'))
    inputs=re.findall(r'\.recovery/build-gcc93/obj/[^\s]+\.(?:o|obj|ao)\b',link)
    counts={suffix:sum(x.endswith(suffix) for x in inputs) for suffix in ['.o','.obj','.ao']};assert counts['.obj']==2 and counts['.ao']==20
    # C inputs are verified exactly against the present native-CU inventory below.
    assert len(inputs)==len(set(inputs)) and 'obj/ManalinkEh.ao' not in link
    assert 'objcopy --only-keep-debug' in log and 'objcopy --strip-debug' in log and not re.search(r'error:|undefined reference|multiple definition',log)
    headers=[root/p for p in ['manalink.h','defs.h','subtypes.h','card_id.h']]
    for relative in inputs:
        obj=root/relative;source=root/re.sub(r'\.(?:o|obj|ao)$',lambda m:{'.o':'.c','.obj':'.cpp','.ao':'.asm'}[m[0]],relative.split('/obj/',1)[1]);assert obj.exists() and source.exists()
        assert obj.stat().st_mtime_ns>=source.stat().st_mtime_ns
        if source.suffix in {'.c','.cpp'}:assert all(obj.stat().st_mtime_ns>=p.stat().st_mtime_ns for p in headers)
    units={r['source'] for year in ['2020','2022'] for r in csv.DictReader((cache/f'{year}-units.csv').open())}
    excluded=(root/'Makefile-excluded').read_text().split('EXCLUDED_CFILES =',1)[1].split('# These old wrappers',1)[0]
    excluded=re.findall(r'(?:cards|functions)/[\w_]+\.c',excluded);assert len(excluded)==24 and not set(excluded)&units
    selected={x.split('/obj/',1)[1][:-2]+'.c' for x in inputs if x.endswith('.o')}
    present={p.relative_to(root).as_posix() for folder in ['cards','functions'] for p in (root/folder).glob('*.c')}
    assert selected==present&units
    report=dict(status='success',compiler='GCC/G++ 9.3.0, x86_64-w64-mingw32 host using -m32',
        build_command='powershell -ExecutionPolicy Bypass -File tools/build-recovery.ps1',warnings_as_errors=False,
        object_inputs=counts,veneer_object='ManalinkEh.ao; loaded exactly once by manalink.lds',
        private_embalm_workers_verified=len(embalm_workers),
        private_memorial_helpers_verified=len(memorial_private),
        private_spell_cost_helpers_verified=len(spell_cost_private),private_spell_cost_symbols=spell_cost_symbols,
        private_aer_free_helpers_verified=len(aer_free_helpers),
        private_card_batch_helpers_verified=len(card_batch_private),private_subtype_result_globals_verified=len(subtype_result),
        private_combat_ai_callbacks_verified=len(combat_ai_private),
        private_casting_reveal_helpers_verified=len(casting_reveal_private),private_casting_reveal_symbols=casting_reveal_symbols,private_casting_reveal_inlined=casting_reveal_inlined,
        private_grave_pump_callbacks_verified=len(grave_pump_private),private_copy_exile_callbacks_verified=len(copy_exile_private),private_graveyard_selection_callbacks_verified=len(graveyard_selection_private),private_hogaak_workers_verified=len(hogaak_private),private_graveyard_selection_symbols=graveyard_selection_symbols,
        migrated_counter_eligibility_helpers_verified=len(migrated_eligibility),private_flag_target_callbacks_verified=len(flag_private),public_cycling_state_globals_verified=len(cycling_globals),
        drawcard_config_imports_verified=len(drawcard_config_imports),mode_counter_tail_thunks_verified=mode_counter_thunks,outlined_proliferate_symbols_verified=outlined_proliferate,
        native_pack_tables_verified=pack_tables_verified,private_pack_utility_symbols=pack_utility_private_symbols,private_pack_utility_unreferenced=pack_utility_unreferenced,
        native_pack_weight_tables_verified=pack_weights_verified,private_pack_generator_symbols=pack_generator_private_symbols,private_pack_once_buffers=pack_once,
        private_pack_dialog_symbols=pack_dialog_symbols,private_pack_dialog_statics=pack_dialog_statics,native_pack_dialog_template_bytes_verified=pack_template_verified,pack_dialog_api_imports_verified=pack_dialog_imports,
        alternate_casting_filter_abi_bytes=alternate_filters,alternate_casting_vla_shapes=alternate_vlas,
        combat_foundation_private_symbols=combat_foundation_symbols,combat_foundation_inlined=combat_foundation_inlined,combat_foundation_array_shapes=combat_foundation_arrays,combat_foundation_target_bytes=combat_foundation_filters,combat_foundation_record_layout=combat_foundation_layout,combat_foundation_exe_boundaries=combat_foundation_exe,combat_foundation_table_clear=combat_foundation_table_clear,
        damage_prevention_build=damage_prevention_build,
        card_state_build=card_state_build,
        card_queries_build=card_queries_build,
        type_animation_build=type_animation_build,
        tap_ai_build=tap_ai_build,
        name_type_build=name_type_build,
        color_core_build=color_core_build,
        color_hacks_build=color_hacks_build,
        mana_sources_build=mana_sources_build,
        mana_producers_build=mana_producers_build,
        ai_snapshots_build=ai_snapshots_build,
        mana_ai_phase_build=mana_ai_phase_build,
        turn_flow_build=turn_flow_build,
        engine_state_build=engine_state_build,
        stack_activation_build=stack_activation_build,
        mana_payment_build=mana_payment_build,
        draft_mana_symbols=draft_mana_symbols,draft_mana_retained_symbols=draft_mana_retained,draft_mana_array_shapes=draft_mana_arrays,draft_mana_exe_boundaries=draft_mana_exe,draft_mana_byte_stores=draft_mana_bytes,draft_mana_inlined=draft_mana_inline,
        display_hook_symbols=display_symbols,display_hook_array_shapes=display_arrays,display_hook_record_layouts=display_types,display_hook_private_statics=display_statics,display_hook_exe_boundaries=display_exe,display_hook_stdcall=display_stdcall,display_hook_drawcard_imports=display_imports,display_hook_empty_string_verified=display_emptystr,
        combat_search_private_symbols=combat_search_symbols,combat_search_retained_symbols=combat_search_retained,combat_search_array_shapes=combat_search_arrays,combat_search_record_layouts=combat_search_types,combat_search_exe_boundaries=combat_search_exe,combat_search_signed_division=combat_search_signed_division,
        stack_utility_array_shapes=stack_utility_arrays,stack_utility_exe_boundaries=stack_utility_boundaries,stack_utility_rogue_layout=stack_utility_rogue_layout,
        retained_havengul_attachment_symbol=havengul_attachment_symbol,
        private_havengul_symbols=havengul_symbols,havengul_storage_bytes=havengul_storage,havengul_utility_exe_boundaries=havengul_boundaries,
        private_legacy_family_symbols=family_symbols,legacy_family_vla_shapes=family_vlas,bruna_distinct_layout_bytes=bruna_layouts,
        final_card_handler_array_shapes=final_arrays,final_card_handler_filter_bytes=final_filters,aryel_token_layout=final_token,
        selection_storage_array_shapes=selection_arrays,selection_storage_filter_bytes=selection_filters,
        private_exile_return_symbols=exile_return_symbols,exile_return_array_shapes=exile_return_arrays,chemister_distinct_filter_bytes=exile_return_filters,
        private_letter_effect_helpers_verified=len(letter_private),private_clock_buffers_verified=len(clock_buffers),private_clock_buffers=clock_buffers,clock_api_imports_verified=clock_imports,
        helper_unification=helper_unification,canonical_record_storage=canonical_record_storage,
        machine='IMAGE_FILE_MACHINE_I386',optional_header='PE32',image_base=hex(pe.OPTIONAL_HEADER.ImageBase),entry_rva=hex(pe.OPTIONAL_HEADER.AddressOfEntryPoint),exports=exports,
        files={p.name:dict(bytes=p.stat().st_size,sha256=sha(p.read_bytes())) for p in [dll,dbg]},debug_info_bytes=debug.sections['.debug_info'].SizeOfRawData,
        new_batch_public_symbols_verified=len(public),migrated_leaves_test_native_symbols_verified=len(migrated_leaves),private_reveal_callback_symbols_verified=len(reveal_callbacks),private_flash_callback_symbols_verified=len(flash_private),private_flash_event_globals_verified=len(flash_globals),private_snow_callback_symbols_verified=len(snow_callbacks),retained_storm_state_verified=storm_state,import_library_i386_members={n:archive_members(root/n) for n in ['libDrawcardlib.a','libcapturestackbacktrace.a']},
        imported_dlls=[x.dll.decode() for x in pe.DIRECTORY_ENTRY_IMPORT],excluded_c_source_units=excluded,
        repairs=['Two terminal labels receive null statements for GCC 9 C grammar; exact previous records and historical scalar proof retained.',
                 'Header globals become extern declarations: card_coded remains the EXE absolute symbol from manalink.lds; the DLL-owned set_legacy_effect_name_addr pointer has one zero-initialized definition in functions/functions.c.',
                 'Ten duplicate assembly wrappers are omitted; in_play.asm retains is_not_dead while conditionally omitting the two C-replaced wrappers.',
                 'Root veneer object is assembled as a dependency and loaded once by the linker script.',
                 'Both import archives are regenerated with i386 machine and --32 assembler flags.'],
        scope='Compile, assemble, link, separate debug data and inspect output. No game/DLL execution, deployment, gameplay tests or byte matching. The two user-directed Wish dialog stubs remain present and out of scope.')
    if canonical_helpers:
        report['canonical_helpers']=dict(source=canonical_helpers,physical_symbols=physical_helpers)
        report['canonical_helper_domains']=helper_domains
        report['counter_trigger_domains']=counter_trigger_domains
        report['absolute_global_arrays']=absolute_global_arrays
        report['source_spelling_symbols']=source_spelling_symbols
        report['executable_fallbacks']=executable_fallbacks
        report['name_view']='Per-family recovery checks resolve immutable ledger names through the explicit canonical-name map; physical COFF names checked separately, with no emitted compatibility aliases.'
    save_json(cache/'build-gcc93/verification.json',report);print(json.dumps({k:v for k,v in report.items() if k not in {'excluded_c_source_units','repairs'}},indent=2))


if __name__=='__main__':
    from canonicalize_helpers import load,verify,historical_sources
    if load():
        with historical_sources() as canonical:main(canonical)
    else:main()
