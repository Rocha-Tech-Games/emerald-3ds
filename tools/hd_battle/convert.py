#!/usr/bin/env python3
"""Convert the five pilot species from the user's PokeMMO mod.
Requires Pillow. Assets stay outside version control; no archive extraction.
"""
import argparse
import json
import struct
import zipfile
from pathlib import Path
from PIL import Image

FRAMES = 16
SIZE = 128
ATLAS = 512

def morton(x, y):
    return sum(((x >> i) & 1) << (2*i) | ((y >> i) & 1) << (2*i+1) for i in range(3))

def convert(source, output):
    output.mkdir(parents=True, exist_ok=True)
    report = []
    with zipfile.ZipFile(source) as archive:
        for name in sorted(archive.namelist()):
            base = Path(name).name
            if not name.startswith('sprites/battlesprites/') or not base.endswith('.gif'):
                continue
            if base.split('-')[0] not in ('3', '6', '9', '258', '261'):
                continue
            with archive.open(name) as stream, Image.open(stream) as gif:
                frames, times, bounds = [], [], None
                for i in range(gif.n_frames):
                    gif.seek(i)
                    frame = gif.convert('RGBA')
                    box = frame.getbbox()
                    if box:
                        bounds = box if bounds is None else (min(bounds[0],box[0]), min(bounds[1],box[1]), max(bounds[2],box[2]), max(bounds[3],box[3]))
                    frames.append(frame)
                    times.append(max(10, int(gif.info.get('duration', 30))))
            if not bounds or sum(times) > 60000:
                raise ValueError('Invalid animation: ' + name)
            duration = sum(times)
            edges = [round(i*duration/FRAMES) for i in range(FRAMES+1)]
            atlas = Image.new('RGBA', (ATLAS, ATLAS))
            for i in range(FRAMES):
                t, index = edges[i], 0
                while index < len(times)-1 and t >= times[index]:
                    t -= times[index]
                    index += 1
                frame = frames[index].crop(bounds)
                frame.thumbnail((124,124), Image.Resampling.LANCZOS)
                atlas.paste(frame, ((i%4)*SIZE+(SIZE-frame.width)//2, (i//4)*SIZE+SIZE-2-frame.height))
            pixels = atlas.load()
            data = bytearray(ATLAS*ATLAS*2)
            for y in range(ATLAS):
                for x in range(ATLAS):
                    r,g,b,a = pixels[x,y]
                    value = ((r>>3)<<11)|((g>>3)<<6)|((b>>3)<<1)|(a>=128)
                    offset = (((y//8)*(ATLAS//8)+x//8)*64+morton(x,y))*2
                    struct.pack_into('<H', data, offset, value)
            header = b'EM3DHD01'+struct.pack('<16I', *(edges[i+1]-edges[i] for i in range(FRAMES)))
            (output/(base[:-4]+'.bin')).write_bytes(header+data)
            atlas.save(output/(base[:-4]+'.png'))
            report.append(dict(source=name, frames=len(frames), duration_ms=duration, bytes=len(header)+len(data)))
    (output/'manifest.json').write_text(json.dumps(report, indent=2)+'\n')
    return report

if __name__ == '__main__':
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument('mod', type=Path)
    parser.add_argument('output', type=Path)
    args = parser.parse_args()
    print(json.dumps(convert(args.mod, args.output), indent=2))
