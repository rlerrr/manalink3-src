"""Verify Lich's Mastery shared storage, VLA bounds and native text arguments."""
import ast,hashlib,re
from capstone import Cs,CS_ARCH_X86,CS_MODE_32
from capstone.x86_const import X86_OP_IMM,X86_OP_MEM
from inspect_release import Image
from recover_literals import readonly_string
from promote_recovery import LITERAL,code_only
from audit_source import function_body
from recover_lich_loss import ADDRESSES,PRIVATE,CONTROL_SITES,TEXT_ADDRESSES,DIALOG_STORES
from lich_loss_body import STORAGE


def audit_component(component,manifest,sources,machine_bytes,image,root):
    def require(condition,message):
        if not condition:raise ValueError(message)
    entries={f['address']:f for f in manifest['accepted_functions'] if f.get('lichs_mastery_evidence')}
    require(set(entries)==set(ADDRESSES)==set(component['functions']),'Lich life-loss membership mismatch')
    require({a for a,f in entries.items() if f['static']}==PRIVATE,'Lich life-loss private linkage mismatch')
    for address,detail in component['functions'].items():
        require(entries[address]['lichs_mastery_evidence']==detail,'Lich life-loss function evidence mismatch')
    require([len(component['parameters'][a]) for a in ADDRESSES]==[3,3],'Lich life-loss parameter count mismatch')
    arrays=component['arrays']
    require(arrays['grave']['element_type']=='int' and arrays['grave']['upper_bound']==[145,8,6,49,28] and
            arrays['tgts']['element_type']=='target_t' and arrays['tgts']['upper_bound']==[145,8,6],
            'Lich life-loss VLA bounds mismatch')
    fixed={(v['name'],v['type'],v['location']) for v in component['locals']['22d0100'] if isinstance(v['location'],str)}
    require({('td','target_definition_t','(DW_OP_fbreg: -320)'),('buffer','char[100]','(DW_OP_fbreg: -236)'),
             ('test','test_definition_t','(DW_OP_fbreg: -236)')}<=fixed,'Lich life-loss shared test/text local mismatch')
    require(component['storage_declaration']==STORAGE and sources['cards/dominaria.c'].replace('\r\n','\n').count(STORAGE)==1 and
            component['shared_storage']==dict(bytes=216,text_bytes=100,text_offset=0,test_offset=0,test_message_offset=0),
            'Lich life-loss union declaration/layout mismatch')
    require(component['call_frame']==dict(dialog_words=18,outgoing_bytes=72,tutor_words=7,scnprintf_words=5),
            'Lich life-loss outgoing frame mismatch')
    require(component['native_edge_cases']==dict(target_array_extra_element=1,failed_select_advances_index=True,
            failed_select_keeps_count=True,grave_tutor_return_ignored=True,unmark_all_before_exiling=True),
            'Lich life-loss edge-case scope changed')
    body=function_body(sources['cards/dominaria.c'],'lichs_mastery_lifeloss_trigger_2022')
    require('int grave[amount];' in body and 'target_t tgts[amount + 1];' in body,'Lich life-loss source array capacities mismatch')
    dialog=re.search(r'\bdialog_impl\s*\(([^()]*)\)',code_only(body))
    require(dialog and len(dialog[1].split(','))==18,'Lich life-loss source dialog argument count mismatch')
    stores=component['dialog_argument_stores']
    require([s['address'] for s in stores]==DIALOG_STORES and [s['argument'] for s in stores]==list(range(18)),
            'Lich life-loss outgoing dialog word membership mismatch')
    decoder=Cs(CS_ARCH_X86,CS_MODE_32);decoder.detail=True
    constants={2:0x72,3:0x273cdb8,4:0x273cd85,5:0x273ceea,6:0x273d151,7:0x273cd55,8:0x273d139,
               10:5,11:0x273d124,13:10,14:0,15:0,16:0,17:0}
    registers={0:'ebx',1:'eax',9:'eax',12:'edi'}
    for store in stores:
        raw=bytes.fromhex(store['bytes']);at=int(store['address'],16);instructions=list(decoder.disasm(raw,at))
        require(machine_bytes(at,len(raw))==raw and len(instructions)==1,'Lich life-loss dialog argument instruction mismatch')
        ins=instructions[0];index=store['argument']
        require(ins.mnemonic=='mov' and ins.operands[0].type==X86_OP_MEM and ins.reg_name(ins.operands[0].mem.base)=='esp' and
                ins.operands[0].mem.disp==index*4 and ins.operands[0].size==4,'Lich life-loss dialog argument slot mismatch')
        if index in constants:
            require(ins.operands[1].type==X86_OP_IMM and ins.operands[1].imm==constants[index],
                    'Lich life-loss dialog constant argument mismatch')
        else:
            require(ins.reg_name(ins.operands[1].reg)==registers[index],'Lich life-loss dynamic dialog argument mismatch')
    observed=[ast.literal_eval(m[0]).encode('latin1') for m in re.finditer(LITERAL,body) if m[0].startswith('"')]
    require([s['address'] for s in component['native_texts']]==TEXT_ADDRESSES,'Lich life-loss native text membership mismatch')
    for item in component['native_texts']:
        raw=bytes.fromhex(item['bytes'])
        require(readonly_string(image,int(item['address'],16))==raw and raw in observed,'Lich life-loss native text bytes mismatch')
    error=component['error_text'];executable=Image((root/'.recovery/2022-Magic.exe').read_bytes());rva=0x728f6c-executable.pe.OPTIONAL_HEADER.ImageBase
    section=next(s for s in executable.pe.sections if s.VirtualAddress<=rva<s.VirtualAddress+s.Misc_VirtualSize)
    require(error['address']=='728f6c' and error['expression']=='(char *)EXE_STR(0x728F6C)' and
            error['kind']=='runtime_executable_buffer' and error['file_backed'] is False and
            section.Name.rstrip(b'\0')==b'.data' and section.Characteristics & 0x80000000 and rva-section.VirtualAddress>=section.SizeOfRawData and
            hashlib.sha256(executable.data).hexdigest()==error['executable_sha256'],
            'Lich life-loss runtime error buffer binding mismatch')
    require(error['expression'] in function_body(sources['cards/dominaria.c'],'target_only_permanents_unless_in_hand_2022'),
            'Lich life-loss mutable native error-buffer return mismatch')
    require([s['address'] for s in component['control_sites']]==CONTROL_SITES,'Lich life-loss native site membership mismatch')
    for site in component['control_sites']:
        raw=bytes.fromhex(site['bytes'])
        require(machine_bytes(int(site['address'],16),len(raw))==raw,'Lich life-loss instruction mismatch')
    sites={s['address']:s['bytes'] for s in component['control_sites']}
    expected={'22d0133':'c1e002','22d0179':'29c4','22d0188':'8d04c508000000','22d0194':'29c4',
              '22d016a':'c785f0feffff70a52c02','22d0160':'c78500ffffff00800000',
              '22d02d3':'c7442408a4d17302','22d0351':'c744240854d17302',
              '22d031c':'e86f163800','22d0394':'e817083900','22d047d':'e88e3e3e00','22d0489':'e8a2673e00','22d04b6':'e845553e00'}
    require(all(sites[a]==b for a,b in expected.items()),'Lich life-loss allocation/target/text/tutor/exile semantics mismatch')
