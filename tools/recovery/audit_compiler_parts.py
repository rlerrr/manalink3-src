"""Find source self-calls whose native destination is a distinct same-named compiler part."""
import csv,json,re
from pathlib import Path
from capstone import Cs,CS_ARCH_X86,CS_MODE_32
from capstone.x86_const import X86_OP_IMM
from inspect_release import Image
from audit_source import function_body
from promote_recovery import LITERAL

def main():
    root=Path(__file__).resolve().parents[2];cache=root/'.recovery';m=json.loads((root/'docs/recovery-evidence/recovered-functions.json').read_text())
    metadata={int(r['low_pc'],16):r for r in csv.DictReader((cache/'2022-functions.csv').open()) if r['low_pc'] and r['high_pc']}
    runtime=Image((cache/'2022-ManalinkEh.dll').read_bytes());sources={s:(root/s).read_bytes().decode('latin1') for s in m['source_files']}
    decoder=Cs(CS_ARCH_X86,CS_MODE_32);decoder.detail=True;issues=[];self_calls=0
    from canonicalize_helpers import names as canonical_names,verify as verify_canonical
    canonical_helpers=verify_canonical(root);canonical=canonical_names(root)
    for entry in m['accepted_functions']:
        name=canonical.get(entry['name'],entry['name']);body=function_body(sources[entry['source']],name);body=body[body.index('{'):]
        if not re.search(r'\b'+re.escape(name)+r'\s*\(',re.sub(LITERAL,' ',body)):continue
        self_calls+=1;address=int(entry['address'],16);row=metadata[address]
        code=runtime.pe.get_data(address-0x2000000,int(row['high_pc'],16)-address)
        for i in decoder.disasm(code,address):
            if i.mnemonic not in {'call','jmp'} or i.operands[0].type!=X86_OP_IMM:continue
            target=i.operands[0].imm
            if target==address or metadata.get(target,{}).get('name')!=row['name']:continue
            issues.append(dict(name=name,address=entry['address'],source=entry['source'],original_name=row['name'],
                site=f'{i.address:x}',bytes=i.bytes.hex(),target=f'{target:x}'))
    report=dict(functions=len(m['accepted_functions']),canonical_helpers=canonical_helpers,functions_with_source_self_calls=self_calls,issues=issues,
        scope='Checks direct same-DWARF-name call/jump misbindings only; this is not a complete call-graph or gameplay audit.')
    (cache/'compiler-part-audit.json').write_text(json.dumps(report,indent=2));print(json.dumps(report,indent=2));raise SystemExit(bool(issues))

if __name__=='__main__':main()
