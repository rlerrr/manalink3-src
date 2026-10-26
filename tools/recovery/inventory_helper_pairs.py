"""Inventory retained/recovered helper pairs beyond identical-code candidates.

An identical declaration is only an ABI review opportunity, not proof that
arguments, callers, globals or behavior are already correct.
"""
import argparse,collections,csv,json,re
from pathlib import Path
from unify_scalar_helpers import read,span,sha,scalar_signature,parameters
from audit_source import function_body

ROOT=Path(__file__).resolve().parents[2]
LAYOUTS={'target_definition_t':[80,84],'test_definition_t':[208,216],
         'token_generation_t':[72,76],'pump_ability_t':[36,48]}

def main():
    p=argparse.ArgumentParser(__doc__);p.add_argument('--output',type=Path,default=ROOT/'docs/recovery-evidence/helper-pairs.json');args=p.parse_args()
    ledger=ROOT/'docs/recovery-evidence/recovered-functions.json';manifest=json.loads(ledger.read_text())
    from canonicalize_records import verify as verify_records
    canonical=verify_records(ROOT)
    layouts={name:([canonical['runtime_bytes'][name]]*2 if canonical else sizes) for name,sizes in LAYOUTS.items()}
    native_file=ROOT/'.recovery/2022-functions.csv'
    native={int(r['low_pc'],16):r for r in csv.DictReader(native_file.open()) if r['low_pc'] and r['high_pc']}
    sharing_file=ROOT/'docs/recovery-evidence/helper-unification.json'
    shared={(e['source'],e['retained_name']):e for e in json.loads(sharing_file.read_text())['helpers']}
    from canonicalize_helpers import load as canonical_load,verify as canonical_verify,historical_sources
    canonical_helpers=canonical_verify(ROOT)
    canonical_data=canonical_load(ROOT)
    canonical_by_address={e['native_address']:e for e in canonical_data['functions']} if canonical_data else {}
    canonical_by_name={e['canonical_name']:e for e in canonical_data['functions']} if canonical_data else {}
    accepted_by_name={f['name']:f for f in manifest['accepted_functions']}
    retired={}
    from importlib import import_module
    for filename,module in [('retired-reanimation-worker.json','retire_reanimation_worker'),('retired-zone-counter.json','retire_zone_counter')]:
        path=ROOT/'docs/recovery-evidence'/filename
        if path.exists():
            with historical_sources(ROOT):assert import_module(module).verify(ROOT)
            e=json.loads(path.read_text())
            retired[(e['source'],e['retained_name'],e['accepted_recovery_record']['address'])]=path
    texts={};hashes={};rows=[];card_prefix=[]
    for f in manifest['accepted_functions']:
        a=int(f['address'],16)
        if a not in native:continue
        old=native[a]['name'];source=f['source']
        # Keep function-unit helpers such as card_instances_should_be_...
        # in scope. Card-prefixed set-unit handlers are listed separately.
        if source.startswith('cards/') and old.startswith('card_'):
            card_prefix.append(dict(address=f['address'],name=old,source=source));continue
        row=dict(address=f['address'],source=source,retained_name=old,recovered_name=f['name'],recovered_prototype=f['prototype'])
        entry=shared.get((source,old))
        if f['address'] in canonical_by_address:
            canonical_entry=canonical_by_address[f['address']]
            row['status']=('canonical native worker name; separate frontend preserved' if canonical_entry.get('rename_only') else 'canonical recovered definition; obsolete body and compatibility name removed')
            row['physical_name']=canonical_entry['canonical_name']
        elif old in canonical_by_name and canonical_by_name[old]['source']==source and canonical_by_name[old]['native_address']!=f['address']:
            row['status']='distinct native worker and canonical frontend; not duplicate definitions'
            row['frontend_address']=canonical_by_name[old]['native_address']
        elif old in accepted_by_name and accepted_by_name[old]['source']==source and accepted_by_name[old]['address']!=f['address']:
            row['status']='distinct native worker and separately recovered frontend; not duplicate definitions'
            row['frontend_address']=accepted_by_name[old]['address']
        elif entry and int(entry['native_address'],16)==a:
            row['status']='shared implementation'
            if entry.get('source_qualification'):
                row['source_recovered_prototype']=entry['source_recovered_prototype']
                row['source_signature_annotation']=('Read-only card-ID input' if entry['family']=='zone_inputs' else 'Read-only prompt pointee')+'; native debug/recovery prototype preserved separately'
        elif (source,old,f['address']) in retired:
            row['status']='unused private predecessor retired; native recovered worker preserved'
            row['retirement_evidence']=str(retired[(source,old,f['address'])])
        elif f['name']==old:row['status']='native name already preserved'
        elif not re.fullmatch(r'[A-Za-z_]\w*',old):row['status']='compiler-derived native name; review parent coverage'
        else:
            if source not in texts:
                path=ROOT/source;texts[source]=read(path) if path.exists() else ''
                if path.exists():hashes[source]=sha(path.read_bytes())
            bounds=span(texts[source],old)
            if bounds is None:row['status']='no retained C definition under native name'
            else:
                definition=texts[source][bounds[0]:bounds[1]]
                old_proto=definition[:definition.index('{')].strip()+';'
                row.update(retained_prototype=old_proto,retained_body_sha256=sha(function_body(definition,old).encode()),
                           recovered_body_sha256=f.get('reconstruction_body_sha256'))
                differences=[]
                for typename,sizes in layouts.items():
                    modern=typename.removesuffix('_t')+'_2022_t'
                    if sizes[0]!=sizes[1] and re.search(r'\b'+typename+r'\b',old_proto) and re.search(r'\b'+modern+r'\b',f['prototype']):
                        differences.append(dict(retained_type=typename,recovered_type=modern,bytes=sizes))
                row['record_layout_differences']=differences
                try:equal=scalar_signature(old_proto)==scalar_signature(f['prototype'])
                except AssertionError:equal=None
                if equal is True:row['status']='retained pair with matching reviewed C types; semantic review required'
                elif differences:row['status']='retained pair requires record-layout migration'
                elif equal is False:row['status']='retained pair has different reviewed C signature'
                else:row['status']='retained pair needs additional type/calling-convention review'
        rows.append(row)
    counts=collections.Counter(r['status'] for r in rows)
    result=dict(status='inventory; no source edits or resolution claims',canonical_helpers=canonical_helpers,counts=dict(counts),functions=rows,
                card_prefixed_set_unit_functions=len(card_prefix),card_prefixed_set_unit_inventory=card_prefix,
                known_record_layout_bytes=layouts,historical_precanonical_record_layout_bytes=LAYOUTS,source_sha256=hashes,
                inputs={str(path):sha(path.read_bytes()) for path in [ledger,native_file,sharing_file,Path(__file__),ROOT/'tools/recovery/unify_scalar_helpers.py']},
                scope='Accepted native ranges with retained names, excluding card-prefixed set-unit handlers listed separately. Matching declarations are review opportunities, not semantic equivalence. Missing definitions and compiler names do not prove missing recovery. Includes differing bodies that the exact-code duplicate detector intentionally does not accept.')
    if canonical:
        for path in [ROOT/'defs.h',ROOT/'docs/recovery-evidence/interface-recovery.json',ROOT/'docs/recovery-evidence/canonical-records.json',ROOT/'tools/recovery/canonicalize_records.py']:
            result['inputs'][str(path)]=sha(path.read_bytes())
    for name in ['docs/recovery-evidence/canonical-reanimation.json','tools/recovery/canonicalize_reanimation.py',
                 'docs/recovery-evidence/canonical-spell-tutor.json','tools/recovery/canonicalize_spell_tutor.py',
                 'docs/recovery-evidence/retired-reanimation-worker.json','tools/recovery/retire_reanimation_worker.py',
                 'docs/recovery-evidence/zone-input-qualifiers.json','tools/recovery/qualify_zone_inputs.py',
                 'docs/recovery-evidence/retired-zone-counter.json','tools/recovery/retire_zone_counter.py',
                 'docs/recovery-evidence/canonical-storm.json','tools/recovery/canonicalize_storm.py',
                 'docs/recovery-evidence/helper-canonicalization.json','tools/recovery/canonicalize_helpers.py',
                 'docs/recovery-evidence/attachment-predicate-equivalence.json','tools/recovery/verify_helper_equivalences.py']:
        path=ROOT/name
        if path.exists():result['inputs'][str(path)]=sha(path.read_bytes())
    args.output.parent.mkdir(parents=True,exist_ok=True);args.output.write_text(json.dumps(result,indent=2)+'\n')
    print(json.dumps(dict(counts=result['counts'],card_prefixed_set_unit_functions=len(card_prefix)),indent=2))

if __name__=='__main__':main()
