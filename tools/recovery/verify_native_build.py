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


def main():
    root=Path(__file__).resolve().parents[2];cache=root/'.recovery';dll=root/'ManalinkEh.dll';dbg=root/'ManalinkEh.dbg'
    pe=pefile.PE(str(dll));pe.parse_data_directories();debug=Image(dbg.read_bytes())
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
    assert all('_'+f['name'] in names for f in public),[f['name'] for f in public if '_'+f['name'] not in names]
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
        records=[s for s in debug.symbols if s[0]=='_'+global_['name']]
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
        records=[s for s in debug.symbols if s[0]=='_'+global_['name']]
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
                    assert callees=={'in_play','add_state','new_default_test_definition_2022','get_card_name_by_instance','snprintf','reveal_cards_from_your_hand_2022','add_counters_from_source','remove_state'}
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
                    size=value(unqual(type_of(pointer)),'DW_AT_byte_size');assert size==(216 if name.endswith('_2022') else 208)
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
        assert len(alternate_filters)==8 and len(alternate_vlas)==6
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
        retained_havengul_attachment_symbol=havengul_attachment_symbol,
        private_havengul_symbols=havengul_symbols,havengul_storage_bytes=havengul_storage,havengul_utility_exe_boundaries=havengul_boundaries,
        private_legacy_family_symbols=family_symbols,legacy_family_vla_shapes=family_vlas,bruna_distinct_layout_bytes=bruna_layouts,
        final_card_handler_array_shapes=final_arrays,final_card_handler_filter_bytes=final_filters,aryel_token_layout=final_token,
        selection_storage_array_shapes=selection_arrays,selection_storage_filter_bytes=selection_filters,
        private_exile_return_symbols=exile_return_symbols,exile_return_array_shapes=exile_return_arrays,chemister_distinct_filter_bytes=exile_return_filters,
        private_letter_effect_helpers_verified=len(letter_private),private_clock_buffers_verified=len(clock_buffers),private_clock_buffers=clock_buffers,clock_api_imports_verified=clock_imports,
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
    save_json(cache/'build-gcc93/verification.json',report);print(json.dumps({k:v for k,v in report.items() if k not in {'excluded_c_source_units','repairs'}},indent=2))


if __name__=='__main__':main()
