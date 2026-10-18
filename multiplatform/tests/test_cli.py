"""Non-destructive migration checks; no user saves are read."""
from pathlib import Path
import subprocess
import tempfile
import zlib
import struct
exe = (Path(__file__).resolve().parents[1] / 'build/world-tool')
with tempfile.TemporaryDirectory() as folder:
    p = Path(folder)
    raw = bytes(range(256)) * 512
    (p / 'input.sav').write_bytes(raw)
    def run(*args):
        return subprocess.run([str(exe), *args], cwd=p, capture_output=True)
    assert run('wrap', 'input.sav', 'copy.emworld').returncode == 0
    wire = (p / 'copy.emworld').read_bytes()
    assert struct.unpack_from('<I', wire, 20)[0] == zlib.crc32(raw)
    assert struct.unpack_from('<I', wire, 28)[0] == zlib.crc32(wire[:28])
    assert run('export', 'copy.emworld', 'copy.sav').returncode == 0
    assert (p / 'copy.sav').read_bytes() == raw
    assert run('wrap', 'input.sav', 'copy.emworld').returncode != 0
    assert (p / 'input.sav').read_bytes() == raw
    (p / 'short.sav').write_bytes(raw[:-1])
    assert run('wrap', 'short.sav', 'bad.emworld').returncode != 0
    assert not (p / 'bad.emworld').exists()
print('PASS CLI: round trip, overwrite refusal, source preservation, invalid input')
