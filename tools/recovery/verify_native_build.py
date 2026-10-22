"""Inspect the GCC recovery DLL/debug pair without loading or executing either."""
import csv,json,re,struct
from pathlib import Path
import pefile
from inspect_release import Image
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
    for name in ['_card_coded','_set_legacy_effect_name_addr']:
        records=[s for s in debug.symbols if s[0]==name];assert len(records)==1 and records[0][4]==2
        if name=='_card_coded':
            address=int(re.search(r'(?m)^_card_coded\s*=\s*(0x[0-9a-fA-F]+)',(root/'manalink.lds').read_text())[1],16)
            assert records[0][2]==-1 and records[0][1]==address
        else:assert records[0][2]>0
    log=(cache/'build-gcc93/build.log').read_text();link=next(line for line in log.splitlines() if line.startswith('g++ -m32 -shared'))
    inputs=re.findall(r'\.recovery/build-gcc93/obj/[^\s]+\.(?:o|obj|ao)\b',link)
    counts={suffix:sum(x.endswith(suffix) for x in inputs) for suffix in ['.o','.obj','.ao']};assert counts=={'.o':164,'.obj':2,'.ao':20}
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
        machine='IMAGE_FILE_MACHINE_I386',optional_header='PE32',image_base=hex(pe.OPTIONAL_HEADER.ImageBase),entry_rva=hex(pe.OPTIONAL_HEADER.AddressOfEntryPoint),exports=exports,
        files={p.name:dict(bytes=p.stat().st_size,sha256=sha(p.read_bytes())) for p in [dll,dbg]},debug_info_bytes=debug.sections['.debug_info'].SizeOfRawData,
        new_batch_public_symbols_verified=len(public),import_library_i386_members={n:archive_members(root/n) for n in ['libDrawcardlib.a','libcapturestackbacktrace.a']},
        imported_dlls=[x.dll.decode() for x in pe.DIRECTORY_ENTRY_IMPORT],excluded_c_source_units=excluded,
        repairs=['Two terminal labels receive null statements for GCC 9 C grammar; exact previous records and historical scalar proof retained.',
                 'Header globals become extern declarations: card_coded remains the EXE absolute symbol from manalink.lds; the DLL-owned set_legacy_effect_name_addr pointer has one zero-initialized definition in functions/functions.c.',
                 'Ten duplicate assembly wrappers are omitted; in_play.asm retains is_not_dead while conditionally omitting the two C-replaced wrappers.',
                 'Root veneer object is assembled as a dependency and loaded once by the linker script.',
                 'Both import archives are regenerated with i386 machine and --32 assembler flags.'],
        scope='Compile, assemble, link, separate debug data and inspect output. No game/DLL execution, deployment, gameplay tests or byte matching. The two user-directed Wish dialog stubs remain present and out of scope.')
    save_json(cache/'build-gcc93/verification.json',report);print(json.dumps({k:v for k,v in report.items() if k not in {'excluded_c_source_units','repairs'}},indent=2))


if __name__=='__main__':main()
