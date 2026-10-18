import importlib.util
import io
import struct
import tempfile
import unittest
import zipfile
from pathlib import Path
from PIL import Image

spec = importlib.util.spec_from_file_location('hdconvert', Path(__file__).with_name('convert.py'))
converter = importlib.util.module_from_spec(spec)
spec.loader.exec_module(converter)

class ConversionTest(unittest.TestCase):
    def test_selection_timing_alpha_and_native_layout(self):
        frames = [Image.new('RGBA', (12, 16), color) for color in ('red', 'blue')]
        stream = io.BytesIO()
        frames[0].save(stream, format='GIF', save_all=True, append_images=frames[1:], duration=[100,300], loop=0)
        with tempfile.TemporaryDirectory() as directory:
            root = Path(directory)
            with zipfile.ZipFile(root/'sample.mod', 'w') as archive:
                archive.writestr('sprites/battlesprites/1-front-n.gif', stream.getvalue())
                archive.writestr('sprites/battlesprites/386-back-n.gif', stream.getvalue())
                archive.writestr('sprites/battlesprites/0-front-n.gif', stream.getvalue())
                archive.writestr('sprites/battlesprites/6-front-n.gif', stream.getvalue())
                archive.writestr('sprites/battlesprites/258-back-n.gif', stream.getvalue())
                archive.writestr('sprites/battlesprites/261-front-n.gif', stream.getvalue())
                archive.writestr('sprites/battlesprites/387-front-n.gif', stream.getvalue())
                archive.writestr('../../escape.gif', stream.getvalue())
            report = converter.convert(root/'sample.mod', root/'pack')
            self.assertEqual(len(report), 5)
            self.assertTrue((root/'pack/1-front-n.bin').exists())
            self.assertTrue((root/'pack/386-back-n.bin').exists())
            self.assertFalse((root/'pack/0-front-n.bin').exists())
            self.assertFalse((root/'pack/387-front-n.bin').exists())
            self.assertTrue((root/'pack/258-back-n.bin').exists())
            self.assertTrue((root/'pack/261-front-n.bin').exists())
            raw = (root/'pack/6-front-n.bin').read_bytes()
            self.assertEqual(len(raw), 524360)
            self.assertEqual(raw[:8], b'EM3DHD01')
            self.assertEqual(sum(struct.unpack('<16I', raw[8:72])), 400)
            self.assertEqual(struct.unpack_from('<H', raw, 72)[0], 0)
            # Frame 0 red, frame 4 blue: sampling respects unequal durations.
            for x,y,value in [(64,120,0xf801),(64,248,0x003f)]:
                offset = (((y//8)*64+x//8)*64+converter.morton(x,y))*2+72
                self.assertEqual(struct.unpack_from('<H', raw, offset)[0], value)
            self.assertFalse((root/'escape.gif').exists())

if __name__ == '__main__':
    unittest.main()
