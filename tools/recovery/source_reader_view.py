"""Scoped C-source spellings for historical Python receipts; filesystem I/O stays real."""
import sys
from contextlib import contextmanager
from pathlib import Path

_byte_views=[]


def source_bytes(path):
    """Explicit historical C-input bytes, or physical bytes outside that scope."""
    p=Path(path).resolve()
    for view in reversed(_byte_views):
        if p in view:return view[p]
    return p.read_bytes()


@contextmanager
def source_reader_view(root,texts,exclude=(),byte_inputs=None):
    from canonicalize_helpers import read as base_reader
    texts={Path(p).resolve():text for p,text in texts.items()};active=[True]
    def scoped_read(path):
        p=Path(path).resolve()
        return texts[p] if active[0] and p in texts else base_reader(path)
    inputs={Path(p).resolve():raw for p,raw in (byte_inputs or {}).items()}
    assert all(raw.decode('latin1').replace('\r\n','\n')==texts[p] for p,raw in inputs.items())
    patched=[];directory=(root/'tools/recovery').resolve();_byte_views.append(inputs)
    try:
        for module in list(sys.modules.values()):
            file=getattr(module,'__file__',None)
            if not file or module.__name__ in exclude or not Path(file).resolve().is_relative_to(directory):continue
            for name,value in list(vars(module).items()):
                if value is base_reader:patched.append((module,name,value));setattr(module,name,scoped_read)
        yield
    finally:
        active[0]=False
        assert _byte_views.pop() is inputs
        for module,name,value in reversed(patched):setattr(module,name,value)
        for module in list(sys.modules.values()):
            if module is None:continue
            for name,value in list(vars(module).items()):
                if value is scoped_read:setattr(module,name,base_reader)
