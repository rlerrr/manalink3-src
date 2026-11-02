"""Check dead call-result inlining using source/expanded CFGs and GCC types."""
import collections,gzip,json,re,sys
from contextlib import contextmanager,ExitStack
from pathlib import Path
from canonicalize_helpers import ROOT,read,span,sha,mask
from inspect_release import Image
from verify_native_absolute_bindings import runtime_sections,stable_sections
from call_condition_source import analyze,project
from preprocessed_conditions import preprocess_view
from call_result_types import inspect as compiler_types
from call_condition_codegen import verify as verify_codegen

_records={};_projected={}


def artifact(root,entry,extension):
    p=root/entry[extension+'_path'];data=p.read_bytes()
    if p.suffix=='.gz':
        assert sha(data)==entry[extension+'_archive_sha256'];data=gzip.decompress(data)
    assert sha(data)==entry[extension+'_sha256'];return data


def record(proof,root=ROOT):
    path=proof.get('native_call_condition_cleanup')
    if not path:return None
    p=root/path;key=str(p),p.stat().st_mtime_ns
    if key not in _records:
        data=p.read_bytes();pin=next(v for v in proof['interface_supporting_inputs'] if v['path']==path);assert sha(data)==pin['sha256'];_records[key]=json.loads(data)
    return _records[key]


def projected(row):
    key=row['before_sha256'],row['after_sha256']
    if key not in _projected:
        assert sha(row['before_definition'].encode())==row['before_sha256']
        from special_call_condition_source import analysis_view
        view=analysis_view(row['before_definition'],row['name'])[0] if row.get('syntax_adapter') else None
        if row.get('payment_helpers_cleanup'):
            assert row['name'] in ['charge_mana_from_id_with_cost_mod','charge_mana_for_activated_ability','has_mana_for_activated_ability','attachment_granting_activated_ability','equipment_granting_activated_ability']
            after=row['after_definition'];facts=None
        elif row.get('casting_cost_cleanup'):
            assert row['name']=='true_get_updated_casting_cost'
            after=row['after_definition'];facts=None
        elif row.get('life_helpers_cleanup'):
            assert row['name'] in ['life_total_can_change','can_lose_the_game_impl','gain_life_impl','trigger_gain_life','cannot_lose_the_game_impl','lose_life']
            after=row['after_definition'];facts=None
        elif row.get('counter_tail_cleanup'):
            assert row['name'] in ['quarry_hauler_helper','remove_counters_impl','move_counters']
            after=row['after_definition'];facts=None
        elif row.get('counter_core_cleanup'):
            assert row['name'] in ['add_or_remove_counters_impl','copy_counters','increment_all_counters','get_updated_counters_number_impl','clear_all_counters']
            after=row['after_definition'];facts=None
        elif row.get('mana_tail_cleanup'):
            assert row['name'] in ['produce_mana_tapped2_impl','produce_mana_tapped_all_one_color_with_default_impl','produce_mana_tapped_any_combination_of_colors_impl','mana_producer','mana_doublers','basic_subtype_to_color_test']
            after=row['after_definition'];facts=None
        elif row.get('tapped_mana_cleanup'):
            assert row['name'] in ['produce_mana_tapped_impl','produce_mana_tapped_multi_impl','produce_mana_tapped_all_different_colors_impl','choose_mana_all_one_color']
            after=row['after_definition'];facts=None
        elif row.get('mana_color_cleanup'):
            assert row['name'] in ['get_colors_of_mana_land_could_produce_ignoring_costs','get_color_of_mana_produced_by_id']
            after=row['after_definition'];facts=None
        elif row.get('land_allowance_cleanup'):
            # This manually reconstructed loop has a pinned native comparison
            # receipt. Keep source succession available without replaying builds.
            assert row['name']=='total_playable_lands'
            after=row['after_definition'];facts=None
        elif row.get('keyword_dispatch_cleanup'):
            if row['name']=='special_abilities':
                from special_dispatch_source import project as dispatch_project
                after=dispatch_project(row['before_definition'])
            else:
                from keyword_worker_source import project as worker_project
                after=worker_project(row['before_definition'],row['name'])
            facts=None
        elif row.get('ability_query_cleanup'):
            from ability_query_source import project as query_project
            after=query_project(row['before_definition'],row['name'],row.get('excluded_cards'),{v['csvid']:v['name'] for v in row.get('card_name_bindings',[])})
            facts=None
        elif row.get('keyword_lookup_cleanup'):
            from keyword_lookup_source import project as keyword_project
            after,facts=keyword_project(row['before_definition'],row['bdd']['table'],row['bindings'])
        elif row.get('instance_lifetime_cleanup'):
            from instance_lifetime_source import project as instance_project
            after,facts=instance_project(row['before_definition'],[dict(group=v['source_group'],new_name=v['new_name']) for v in row['renames']])
        elif row.get('call_argument_cleanup'):
            from call_argument_source import project as argument_project
            after,facts=argument_project(row['before_definition'],[v['source_fact'] for v in row['checks']])
        elif row.get('player_lifetime_cleanup'):
            from player_lifetime_source import project as player_project
            after,facts=player_project(row['before_definition'],[dict(group=v['source_group'],new_name=v['new_name']) for v in row['renames']])
        elif row.get('loop_lifetime_cleanup'):
            from loop_lifetime_source import project as counter_project
            after,facts=counter_project(row['before_definition'],[dict(group=v['source_group'],new_name=v['new_name']) for v in row['renames']])
        elif row.get('getter_lifetime_cleanup'):
            from getter_lifetime_source import project as getter_project
            after,facts=getter_project(row['before_definition'],[dict(group=v['source_group'],new_name=v['new_name']) for v in row['renames']])
        elif row.get('scalar_lifetime_cleanup'):
            from scalar_lifetime_source import project as lifetime_project
            after,facts=lifetime_project(row['before_definition'],[dict(group=v['source_group'],new_name=v['new_name']) for v in row['renames']])
        elif row.get('forward_guard_cleanup'):
            from forward_guard_source import project as guard_project
            after,facts=guard_project(row['before_definition'],row['facts']['rounds'])
        elif row.get('return_tail_cleanup'):
            from return_tail_source import project as tail_project
            after,facts=tail_project(row['before_definition'],row['facts']['source_candidates'])
        elif row.get('residual_direction_cleanup'):
            from residual_direction_source import project as residual_project
            after,facts=residual_project(row['before_definition'],row['facts']['source_candidates'],row['name'])
        elif row.get('direction_flag_cleanup'):
            from direction_flag_source import project as direction_project
            after,facts=direction_project(row['before_definition'],row['facts']['source_candidates'])
        elif row.get('target_loop_cleanup'):
            from target_loop_source import project as loop_project
            after,facts=loop_project(row['before_definition'],row['facts']['source_candidates'])
        elif row.get('comma_call_cleanup'):
            from comma_call_source import project as comma_project
            after,facts=comma_project(row['before_definition'],row['facts']['source_candidates'])
        elif row.get('shared_spell_cleanup'):
            from shared_spell_source import project as spell_project
            after,facts=spell_project(row['before_definition'])
        elif row.get('role_names_cleanup'):
            from role_local_source import project as role_project
            after,count=role_project(row['before_definition'],row['renames']);assert count==row['occurrences']
            facts=None
        elif row.get('return_cleanup'):
            from call_return_source import project as return_project
            after,facts=return_project(row['before_definition'],row['facts']['source_candidates'],remove_only=row['facts']['removed_local_declarations'])
        else:after,facts=project(row['before_definition'],row['facts']['source_candidates'],analysis_text=view)
        assert after==row['after_definition'] and sha(after.encode())==row['after_sha256'] and (facts is None or facts==row['facts']);_projected[key]=after
    return _projected[key]


def all_rows(proof,root=ROOT,*,include_returns=True):
    from verify_assertion_call_conditions import record as assertion_record
    from verify_special_call_conditions import record as special_record
    from verify_shared_spell_cleanup import record as shared_record
    from verify_shared_spell_extension import record as extension_record
    from verify_comma_call_cleanup import record as comma_record
    from verify_comma_comparison_cleanup import record as comparison_record
    from verify_target_loop_cleanup import record as loop_record
    from verify_direction_flag_cleanup import record as direction_record
    from verify_keyword_dispatch_cleanup import record as dispatch_record
    from verify_ability_query_cleanup import record as query_record
    from verify_keyword_lookup_cleanup import record as keyword_record
    from verify_instance_lifetime_cleanup import record as instance_record
    from verify_call_argument_cleanup import record as argument_record
    from verify_player_lifetime_cleanup import record as player_record
    from verify_loop_lifetime_cleanup import record as counter_record
    from verify_getter_lifetime_cleanup import record as getter_record
    from verify_scalar_lifetime_cleanup import record as lifetime_record
    from verify_forward_guard_cleanup import record as guard_record
    from verify_return_tail_cleanup import record as tail_record
    from verify_residual_direction_cleanup import record as residual_record
    from verify_role_local_names import record as role_record
    from verify_call_return_cleanup import record as return_record
    r=record(proof,root);followup=assertion_record(proof,root);special=special_record(proof,root);returns=return_record(proof,root) if include_returns else None;roles=role_record(proof,root) if include_returns else None;shared=shared_record(proof,root) if include_returns else None
    extension=extension_record(proof,root) if include_returns else None
    commas=comma_record(proof,root) if include_returns else None
    comparisons=comparison_record(proof,root) if include_returns else None
    loops=loop_record(proof,root) if include_returns else None
    directions=direction_record(proof,root) if include_returns else None
    residual=residual_record(proof,root) if include_returns else None
    tails=tail_record(proof,root) if include_returns else None
    guards=guard_record(proof,root) if include_returns else None
    lifetimes=lifetime_record(proof,root) if include_returns else None
    getters=getter_record(proof,root) if include_returns else None
    counters=counter_record(proof,root) if include_returns else None
    dispatches=dispatch_record(proof,root) if include_returns else None
    queries=query_record(proof,root) if include_returns else None
    keywords=keyword_record(proof,root) if include_returns else None
    instances=instance_record(proof,root) if include_returns else None
    arguments=argument_record(proof,root) if include_returns else None
    players=player_record(proof,root) if include_returns else None
    land_rows=[]
    if include_returns and proof.get('native_land_allowance_cleanup'):
        path=proof['native_land_allowance_cleanup'];data=(root/path).read_bytes()
        assert sha(data)==next(v['sha256'] for v in proof['interface_supporting_inputs'] if v['path']==path)
        land_rows=json.loads(data)['rows']
    if include_returns and proof.get('native_mana_color_cleanup'):
        path=proof['native_mana_color_cleanup'];data=(root/path).read_bytes()
        assert sha(data)==next(v['sha256'] for v in proof['interface_supporting_inputs'] if v['path']==path)
        land_rows+=json.loads(data)['rows']
    if include_returns and proof.get('native_tapped_mana_cleanup'):
        path=proof['native_tapped_mana_cleanup'];data=(root/path).read_bytes()
        assert sha(data)==next(v['sha256'] for v in proof['interface_supporting_inputs'] if v['path']==path)
        land_rows+=json.loads(data)['rows']
    if include_returns and proof.get('native_mana_tail_cleanup'):
        path=proof['native_mana_tail_cleanup'];data=(root/path).read_bytes()
        assert sha(data)==next(v['sha256'] for v in proof['interface_supporting_inputs'] if v['path']==path)
        land_rows+=json.loads(data)['rows']
    if include_returns and proof.get('native_counter_core_cleanup'):
        path=proof['native_counter_core_cleanup'];data=(root/path).read_bytes()
        assert sha(data)==next(v['sha256'] for v in proof['interface_supporting_inputs'] if v['path']==path)
        land_rows+=json.loads(data)['rows']
    if include_returns and proof.get('native_counter_tail_cleanup'):
        path=proof['native_counter_tail_cleanup'];data=(root/path).read_bytes()
        assert sha(data)==next(v['sha256'] for v in proof['interface_supporting_inputs'] if v['path']==path)
        land_rows+=json.loads(data)['rows']
    if include_returns and proof.get('native_life_helpers_cleanup'):
        path=proof['native_life_helpers_cleanup'];data=(root/path).read_bytes()
        assert sha(data)==next(v['sha256'] for v in proof['interface_supporting_inputs'] if v['path']==path)
        land_rows+=json.loads(data)['rows']
    if include_returns and proof.get('native_casting_cost_cleanup'):
        path=proof['native_casting_cost_cleanup'];data=(root/path).read_bytes()
        assert sha(data)==next(v['sha256'] for v in proof['interface_supporting_inputs'] if v['path']==path)
        land_rows+=json.loads(data)['rows']
    if include_returns and proof.get('native_payment_helpers_cleanup'):
        path=proof['native_payment_helpers_cleanup'];data=(root/path).read_bytes()
        assert sha(data)==next(v['sha256'] for v in proof['interface_supporting_inputs'] if v['path']==path)
        land_rows+=json.loads(data)['rows']
    return (r['rows'] if r else [])+(followup['rows'] if followup else [])+(special['rows'] if special else [])+(returns['rows'] if returns else [])+(roles['rows'] if roles else [])+(shared['rows'] if shared else [])+(extension['rows'] if extension else [])+(commas['rows'] if commas else [])+(comparisons['rows'] if comparisons else [])+(loops['rows'] if loops else [])+(directions['rows'] if directions else [])+(residual['rows'] if residual else [])+(tails['rows'] if tails else [])+(guards['rows'] if guards else [])+(lifetimes['rows'] if lifetimes else [])+(getters['rows'] if getters else [])+(counters['rows'] if counters else [])+(players['rows'] if players else [])+(arguments['rows'] if arguments else [])+(instances['rows'] if instances else [])+(keywords['rows'] if keywords else [])+(queries['rows'] if queries else [])+(dispatches['rows'] if dispatches else [])+land_rows


def has_source_repair(proof,root,source,name,old_sha):
    r=record(proof,root)
    if not r:return False
    rows=[v for v in all_rows(proof,root) if (v['source'],v['name'],v['before_sha256'])==(source,name,old_sha)]
    if not rows:return False
    assert len(rows)==1;row=rows[0];text=read(root/source);a,b=span(text,name);expected=projected(row)
    seen=set()
    while text[a:b]!=expected:
        assert row['after_sha256'] not in seen;seen.add(row['after_sha256'])
        successors=[v for v in all_rows(proof,root) if (v['source'],v['name'],v['before_sha256'])==(source,name,row['after_sha256'])]
        assert len(successors)==1;row=successors[0];expected=projected(row)
    return True


def verify_superseded_definition(proof,image,root,source,name,old_sha):
    assert has_source_repair(proof,root,source,name,old_sha);assert sha(image.data)==record(proof,root)['native_dll_sha256']


@contextmanager
def _pre_return_historical_readers(proof,image,root=ROOT):
    """Replay old format-specific receipts after validating the live cleanup.

    Only imported Python C-source readers receive the checked predecessor
    spelling. Files, compilers, binaries, DWARF and current-source validation
    always use the real worktree. This view is limited to full-history replay.
    """
    r=record(proof,root)
    if not r:
        yield {};return
    stats=_verify_pre_return(proof,image,root);texts={}
    for row in reversed(all_rows(proof,root,include_returns=False)):
        p=(root/row['source']).resolve();text=texts.get(p,read(p));a,b=span(text,row['name']);assert text[a:b]==projected(row);texts[p]=text[:a]+row['before_definition']+text[b:]
    base_reader=read;active=[True]
    def historical_read(path):
        p=Path(path).resolve();return texts[p] if active[0] and p in texts else base_reader(path)
    patched=[];tools=(root/'tools/recovery').resolve()
    try:
        for module in list(sys.modules.values()):
            path=getattr(module,'__file__',None)
            if not path or module.__name__==__name__:continue
            if not Path(path).resolve().is_relative_to(tools):continue
            for name,value in list(vars(module).items()):
                if value is base_reader:patched.append((module,name,value));setattr(module,name,historical_read)
        yield stats
    finally:
        active[0]=False
        for module,name,value in reversed(patched):setattr(module,name,value)
        # Lazy imports may have captured the scoped reader. Restore their
        # module bindings too; retained defaults remain safe via active=False.
        for module in list(sys.modules.values()):
            if module is None:continue
            for name,value in list(vars(module).items()):
                if value is historical_read:setattr(module,name,base_reader)


def _verify_pre_return(proof,image,root=ROOT):
    r=record(proof,root)
    if not r:return {}
    from verify_special_call_conditions import verify as verify_special
    special=verify_special(proof,root)
    assert sha(image.data)==r['native_dll_sha256'];pp_path=root/r['preprocess_record'];types_path=root/r['compiler_type_record'];assert sha(pp_path.read_bytes())==r['preprocess_record_sha256'] and sha(types_path.read_bytes())==r['compiler_type_record_sha256'];pp={v['source']:v for v in json.loads(pp_path.read_bytes())['rows']};types={v['source']:v for v in json.loads(types_path.read_bytes())['rows']}
    archive_path=root/r['preprocess_archive_index'];assert sha(archive_path.read_bytes())==r['preprocess_archive_index_sha256'];archives={v['raw_path']:v for v in json.loads(archive_path.read_bytes())};by_source=collections.defaultdict(list)
    for row in r['rows']:by_source[row['source']].append(row)
    count=removed=0
    for source,rows in by_source.items():
        original=(root/'.recovery/call-conditions-before-c91'/source).read_bytes();assert sha(original)==pp[source]['source_bytes_sha256']==types[source]['source_bytes_sha256'];whole=original.decode('latin1').replace('\r\n','\n');a=archives[pp[source]['preprocessed_path']];packed=(root/a['archive_path']).read_bytes();assert sha(packed)==a['archive_sha256'];expanded_bytes=gzip.decompress(packed);assert sha(expanded_bytes)==a['raw_sha256']==pp[source]['preprocessed_sha256'];get_function,sensitive=preprocess_view(expanded_bytes.decode('latin1'));assert not set(re.findall(r'\b\w+\b',mask(whole)))&sensitive
        t=types[source];data=(root/t['archive_path']).read_bytes();assert sha(data)==t['archive_sha256'];raw=gzip.decompress(data);assert sha(raw)==t['raw_sha256'];observed_types=compiler_types(raw.decode('latin1'),{v['name'] for v in rows})
        current=read(root/source)
        for row in rows:
            a,b=span(current,row['name']);assert current[a:b]==projected(row) or has_source_repair(proof,root,source,row['name'],row['before_sha256']);expanded,origin=get_function(row['name']);assert expanded==row['expanded_before_definition'];facts=analyze(expanded);assert len(row['checks'])==row['facts']['inlined_assignments']
            for c in row['checks']:
                assert c['expanded_fact'] in facts and origin(c['expanded_fact']['assignment_offset'])==(source,c['source_assignment_line']) and origin(c['expanded_fact']['condition_offset'])==(source,c['source_condition_line']);e=observed_types[row['name']][c['expanded_callee']];assert e==c['compiler_type_evidence'] and e and all(v['signed_int32'] for v in e)
            count+=row['facts']['inlined_assignments'];removed+=len(row['facts']['removed_local_declarations'])
            edits=[e for e in proof['source_edits'] if e.get('call_condition_cleanup_extension')==proof['native_call_condition_cleanup'] and (e['source'],e.get('name'))==(source,row['name'])];assert len(edits)==1 and edits[0]['before_sha256']==row['before_sha256'] and edits[0]['after_sha256']==row['after_sha256']
    builds=[]
    for label in ['before','after']:
        a=r['runtime_'+label];im=Image(artifact(root,a,'dll'));debug=Image(artifact(root,a,'dbg'));assert runtime_sections(im)==a['sections'];builds.append((im,debug))
    assert verify_codegen(*builds[0],*builds[1],r['codegen_review'])==r['codegen_validation']
    assert len(r['rows'])==718 and len(by_source)==110 and count==1515 and removed==131 and len(r['pending'])==158
    from verify_assertion_call_conditions import verify as verify_assertions
    followup=verify_assertions(proof,root)
    return dict(cleaned_definitions=718,source_units=110,inlined_call_results=1515,removed_unused_int_locals=131,pending_candidates=0 if special else (4 if followup else 158),assertion_followup=followup,special_followup=special,codegen=r['codegen_validation'],scope=r['scope'])


@contextmanager
def historical_readers(proof,image,root=ROOT):
    from verify_keyword_dispatch_cleanup import historical_readers as dispatch_readers
    from verify_ability_query_cleanup import historical_readers as query_readers
    from verify_keyword_lookup_cleanup import historical_readers as keyword_readers
    from verify_instance_lifetime_cleanup import historical_readers as instance_readers
    from verify_call_argument_cleanup import historical_readers as argument_readers
    from verify_player_lifetime_cleanup import historical_readers as player_readers
    from verify_loop_lifetime_cleanup import historical_readers as counter_readers
    from verify_getter_lifetime_cleanup import historical_readers as getter_readers
    from verify_scalar_lifetime_cleanup import historical_readers as lifetime_readers
    from verify_forward_guard_cleanup import historical_readers as guard_readers
    from verify_return_tail_cleanup import historical_readers as tail_readers
    from verify_residual_direction_cleanup import historical_readers as residual_readers
    from verify_direction_flag_cleanup import historical_readers as direction_readers
    from verify_target_loop_cleanup import historical_readers as loop_readers
    from verify_comma_comparison_cleanup import historical_readers as comparison_readers
    from verify_comma_call_cleanup import historical_readers as comma_readers
    from verify_shared_spell_extension import historical_readers as extension_readers
    from verify_shared_spell_cleanup import historical_readers as shared_readers
    from verify_role_local_names import historical_readers as role_readers
    from verify_call_return_cleanup import historical_readers as return_readers
    with ExitStack() as recovery_views:
        dispatches = recovery_views.enter_context(dispatch_readers(proof,root))
        queries = recovery_views.enter_context(query_readers(proof,root))
        keywords = recovery_views.enter_context(keyword_readers(proof,root))
        instances = recovery_views.enter_context(instance_readers(proof,root))
        arguments = recovery_views.enter_context(argument_readers(proof,root))
        players = recovery_views.enter_context(player_readers(proof,root))
        counters = recovery_views.enter_context(counter_readers(proof,root))
        getters = recovery_views.enter_context(getter_readers(proof,root))
        lifetimes = recovery_views.enter_context(lifetime_readers(proof,root))
        guards = recovery_views.enter_context(guard_readers(proof,root))
        tails = recovery_views.enter_context(tail_readers(proof,root))
        residual = recovery_views.enter_context(residual_readers(proof,root))
        directions = recovery_views.enter_context(direction_readers(proof,root))
        loops = recovery_views.enter_context(loop_readers(proof,root))
        comparisons = recovery_views.enter_context(comparison_readers(proof,root))
        commas = recovery_views.enter_context(comma_readers(proof,root))
        extension = recovery_views.enter_context(extension_readers(proof,root))
        shared = recovery_views.enter_context(shared_readers(proof,root))
        with role_readers(proof,root) as roles:
            with return_readers(proof,root) as returns:
                with _pre_return_historical_readers(proof,image,root) as stats:
                    stats['call_return_cleanup']=returns
                    stats['role_local_names']=roles
                    stats['shared_spell_cleanup']=shared
                    stats['shared_spell_extension']=extension
                    stats['comma_call_cleanup']=commas
                    stats['comma_comparison_cleanup']=comparisons
                    stats['target_loop_cleanup']=loops
                    stats['direction_flag_cleanup']=directions
                    stats['residual_direction_cleanup']=residual
                    stats['return_tail_cleanup']=tails
                    stats['forward_guard_cleanup']=guards
                    stats['scalar_lifetime_cleanup']=lifetimes
                    stats['getter_lifetime_cleanup']=getters
                    stats['loop_lifetime_cleanup']=counters
                    stats['player_lifetime_cleanup']=players;stats['call_argument_cleanup']=arguments;stats['instance_lifetime_cleanup']=instances;stats['keyword_lookup_cleanup']=keywords;stats['ability_query_cleanup']=queries;stats['keyword_dispatch_cleanup']=dispatches
                    yield stats


def verify(proof,image,root=ROOT):
    from verify_keyword_dispatch_cleanup import historical_readers as dispatch_readers
    from verify_ability_query_cleanup import historical_readers as query_readers
    from verify_keyword_lookup_cleanup import historical_readers as keyword_readers
    from verify_instance_lifetime_cleanup import historical_readers as instance_readers
    from verify_call_argument_cleanup import historical_readers as argument_readers
    from verify_player_lifetime_cleanup import historical_readers as player_readers
    from verify_loop_lifetime_cleanup import historical_readers as counter_readers
    from verify_getter_lifetime_cleanup import historical_readers as getter_readers
    from verify_scalar_lifetime_cleanup import historical_readers as lifetime_readers
    from verify_forward_guard_cleanup import historical_readers as guard_readers
    from verify_return_tail_cleanup import historical_readers as tail_readers
    from verify_residual_direction_cleanup import historical_readers as residual_readers
    from verify_direction_flag_cleanup import historical_readers as direction_readers
    from verify_target_loop_cleanup import historical_readers as loop_readers
    from verify_comma_comparison_cleanup import historical_readers as comparison_readers
    from verify_comma_call_cleanup import historical_readers as comma_readers
    from verify_shared_spell_extension import historical_readers as extension_readers
    from verify_shared_spell_cleanup import historical_readers as shared_readers
    from verify_role_local_names import historical_readers as role_readers
    from verify_call_return_cleanup import historical_readers as return_readers
    with ExitStack() as recovery_views:
        dispatches = recovery_views.enter_context(dispatch_readers(proof,root))
        queries = recovery_views.enter_context(query_readers(proof,root))
        keywords = recovery_views.enter_context(keyword_readers(proof,root))
        instances = recovery_views.enter_context(instance_readers(proof,root))
        arguments = recovery_views.enter_context(argument_readers(proof,root))
        players = recovery_views.enter_context(player_readers(proof,root))
        counters = recovery_views.enter_context(counter_readers(proof,root))
        getters = recovery_views.enter_context(getter_readers(proof,root))
        lifetimes = recovery_views.enter_context(lifetime_readers(proof,root))
        guards = recovery_views.enter_context(guard_readers(proof,root))
        tails = recovery_views.enter_context(tail_readers(proof,root))
        residual = recovery_views.enter_context(residual_readers(proof,root))
        directions = recovery_views.enter_context(direction_readers(proof,root))
        loops = recovery_views.enter_context(loop_readers(proof,root))
        comparisons = recovery_views.enter_context(comparison_readers(proof,root))
        commas = recovery_views.enter_context(comma_readers(proof,root))
        extension = recovery_views.enter_context(extension_readers(proof,root))
        shared = recovery_views.enter_context(shared_readers(proof,root))
        with role_readers(proof,root) as roles:
            with return_readers(proof,root) as returns:
                stats=_verify_pre_return(proof,image,root)
    stats['call_return_cleanup']=returns
    stats['role_local_names']=roles
    stats['shared_spell_cleanup']=shared
    stats['shared_spell_extension']=extension
    stats['comma_call_cleanup']=commas
    stats['comma_comparison_cleanup']=comparisons
    stats['target_loop_cleanup']=loops
    stats['direction_flag_cleanup']=directions
    stats['residual_direction_cleanup']=residual
    stats['return_tail_cleanup']=tails
    stats['forward_guard_cleanup']=guards
    stats['scalar_lifetime_cleanup']=lifetimes
    stats['getter_lifetime_cleanup']=getters
    stats['loop_lifetime_cleanup']=counters
    stats['player_lifetime_cleanup']=players;stats['call_argument_cleanup']=arguments;stats['instance_lifetime_cleanup']=instances;stats['keyword_lookup_cleanup']=keywords;stats['ability_query_cleanup']=queries;stats['keyword_dispatch_cleanup']=dispatches
    return stats


if __name__=='__main__':
    from canonicalize_helpers import load
    print(json.dumps(verify(load(),Image((ROOT/'.recovery/2022-ManalinkEh.dll').read_bytes())),indent=2))
