"""Reuse the spell-dispatch checker for a subsequent source projection stage."""
import json
from contextlib import contextmanager
from canonicalize_helpers import ROOT,read,sha
from verify_shared_spell_cleanup import verify_record
from source_reader_view import source_reader_view

KEY='native_shared_spell_extension'

def record(proof,root=ROOT):
    path=proof.get(KEY)
    if not path:return None
    data=(root/path).read_bytes()
    assert sha(data)==next(v['sha256'] for v in proof['interface_supporting_inputs'] if v['path']==path)
    return json.loads(data)

def verify(proof,root=ROOT):
    return verify_record(proof,record(proof,root),KEY,root,read)

@contextmanager
def historical_readers(proof,root=ROOT):
    r=record(proof,root)
    if not r:
        yield {};return
    stats=verify(proof,root)
    inputs={root/u['source']:(root/u['before_path']).read_bytes() for u in r['files']}
    texts={p:b.decode('latin1').replace('\r\n','\n') for p,b in inputs.items()}
    with source_reader_view(root,texts,exclude={__name__},byte_inputs=inputs):yield stats

if __name__=='__main__':
    from canonicalize_helpers import load
    print(json.dumps(verify(load()),indent=2))
