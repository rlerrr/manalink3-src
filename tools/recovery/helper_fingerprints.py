"""Conservative local-code comparison; never wildcard unknown addresses."""
import hashlib,json
from dataclasses import dataclass
from capstone import Cs,CS_ARCH_X86,CS_MODE_32,CS_GRP_JUMP,CS_GRP_CALL
from capstone.x86_const import X86_OP_IMM,X86_OP_MEM,X86_OP_REG


def digest(value):
    return hashlib.sha256(json.dumps(value,sort_keys=True,separators=(',',':')).encode()).hexdigest()


@dataclass
class Fingerprint:
    instructions:list
    assembly:list
    issues:list
    bindings:list
    dependencies:list

    @property
    def sha256(self):return digest(self.instructions)


class LocalCode:
    def __init__(self,pe,functions,legacy=None,accepted=(),unsafe=()):
        self.pe=pe;self.functions=functions;self.legacy=legacy or {};self.accepted=set(accepted)
        self.unsafe=set(unsafe)
        self.base=pe.OPTIONAL_HEADER.ImageBase;self.end=self.base+pe.OPTIONAL_HEADER.SizeOfImage
        self.relocations={self.base+e.rva for b in getattr(pe,'DIRECTORY_ENTRY_BASERELOC',[]) for e in b.entries if e.type==3}
        self.decoder=Cs(CS_ARCH_X86,CS_MODE_32);self.decoder.detail=True

    def read(self,at,size):return self.pe.get_data(at-self.base,size)

    def readonly_string(self,at,allowed):
        rva=at-self.base;section=self.pe.get_section_by_rva(rva)
        if section is None or section.Name.rstrip(b'\0')!=b'.rdata' or section.Characteristics&0x80000000:return None
        end=self.pe.get_data(rva,4096).find(b'\0')
        if end<4:return None
        data=self.pe.get_data(rva,end)
        # A zero or integer table must not be mistaken for an empty string.
        # The exact string must also occur in the source definition.
        return data if data in allowed else None

    def address_token(self,at,allowed,same_image,issues,bindings,dependencies):
        if same_image:return ('same-image-address',hex(at))
        found=self.functions.get(at,())
        if len(found)==1:
            ident=next(iter(found));dependencies.add(ident)
            if ident in self.unsafe:issues.add('stub dependency requires integration')
            return ('function',hex(ident))
        old=self.legacy.get(at,())
        if not found and len(old)==1:
            ident=next(iter(old));dependencies.add(ident)
            if ident in self.unsafe:issues.add('stub dependency requires integration')
            if ident in self.accepted:bindings.append(dict(address=hex(at),native_target=hex(ident)))
            return ('function',hex(ident))
        if not self.base<=at<self.end:return ('fixed-external-address',hex(at))
        literal=self.readonly_string(at,allowed)
        if literal is not None:return ('verified-source-string',hashlib.sha256(literal).hexdigest())
        issues.add('ambiguous function reference' if len(found)>1 or len(old)>1 else 'unresolved DLL code/data reference')
        return ('unresolved-address',hex(at))

    @staticmethod
    def noop(i):
        if i.mnemonic=='nop':return True
        if i.mnemonic!='lea' or len(i.operands)!=2:return False
        a,b=i.operands
        return a.type==X86_OP_REG and b.type==X86_OP_MEM and a.reg==b.mem.base and not b.mem.index and not b.mem.disp

    def fingerprint(self,at,size,literals=(),same_image=False):
        raw=self.read(at,size);decoded=list(self.decoder.disasm(raw,at));issues=set();bindings=[];dependencies=set()
        if sum(i.size for i in decoded)!=size:issues.add('disassembly does not cover the complete DWARF range')
        active=[i for i in decoded if not self.noop(i)]
        ordinal={i.address:n for n,i in enumerate(active)}
        # An alignment NOP at a branch destination leads to the next real instruction.
        next_index=len(active)
        for i in reversed(decoded):
            if i.address in ordinal:next_index=ordinal[i.address]
            else:ordinal[i.address]=next_index
        result=[];assembly=[]
        for i in active:
            binding_start=len(bindings)
            data=bytearray(i.bytes);references=[];control=i.group(CS_GRP_JUMP) or i.group(CS_GRP_CALL)
            direct=control and len(i.operands)==1 and i.operands[0].type==X86_OP_IMM
            if direct:
                target=i.operands[0].imm & 0xffffffff
                if at<=target<at+size:
                    if target not in ordinal:issues.add('branch enters the middle of an instruction');token=('invalid-branch',hex(target))
                    else:token=('local-instruction',ordinal[target])
                else:token=self.address_token(target,literals,same_image,issues,bindings,dependencies)
                data[i.imm_offset:i.imm_offset+i.imm_size]=bytes(i.imm_size)
                references.append(('control',token))
            elif control and not same_image:
                issues.add('indirect control flow requires dependency review')
            for kind,offset,width in [('displacement',i.disp_offset,i.disp_size),('immediate',i.imm_offset,i.imm_size)]:
                if not width or (direct and kind=='immediate') or same_image:continue
                if i.address+offset not in self.relocations:continue
                if width!=4:issues.add('unsupported relocation width');continue
                target=int.from_bytes(i.bytes[offset:offset+width],'little')
                token=self.address_token(target,literals,False,issues,bindings,dependencies)
                if kind=='displacement' and token[0]=='function' and i.mnemonic!='lea':
                    issues.add('code address dereferenced as data');continue
                if token[0] in {'function','verified-source-string'}:
                    data[offset:offset+width]=bytes(width);references.append((kind,token))
                # Unknown references and fixed external words retain exact bytes.
            for binding in bindings[binding_start:]:
                binding.update(instruction_address=hex(i.address),instruction_bytes=i.bytes.hex())
            result.append((i.mnemonic,data.hex(),references));assembly.append(dict(address=hex(i.address),instruction=i.mnemonic+' '+i.op_str))
        return Fingerprint(result,assembly,sorted(issues),bindings,sorted(dependencies))
