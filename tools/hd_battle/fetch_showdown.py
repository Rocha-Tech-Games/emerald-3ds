#!/usr/bin/env python3
"""Fetch pinned PokéAPI/Showdown GIFs for National Dex 1–386.

Produces a conversion-compatible ZIP and provenance JSON. No assets go in Git.
"""
import argparse
import concurrent.futures
import hashlib
import json
import time
import urllib.request
import zipfile
from pathlib import Path

def get(url):
    for attempt in range(5):
        try:
            with urllib.request.urlopen(urllib.request.Request(url, headers={'User-Agent': 'Emerald3DS-asset-preparation'}), timeout=60) as response:
                return response.read()
        except Exception:
            if attempt == 4:
                raise
            time.sleep(2 ** attempt)

def fetch(output, commit=None):
    output.mkdir(parents=True, exist_ok=True)
    if commit is None:
        commit = json.loads(get('https://api.github.com/repos/PokeAPI/sprites/commits/master'))['sha']
    entries = json.loads(get(f'https://api.github.com/repos/PokeAPI/sprites/contents/sprites/pokemon/other?ref={commit}'))
    tree_url = next(e['git_url'] for e in entries if e['name'] == 'showdown')
    tree = json.loads(get(tree_url + '?recursive=1'))
    assert not tree['truncated'], 'Incomplete source inventory'
    inventory = {e['path']: e['sha'] for e in tree['tree'] if e['type'] == 'blob'}
    records = []
    for number in range(1, 387):
        for prefix, view, color in [('', 'front', 'n'), ('back/', 'back', 'n'), ('shiny/', 'front', 's'), ('back/shiny/', 'back', 's')]:
            path = f'{prefix}{number}.gif'
            records.append(dict(path=path, name=f'{number}-{view}-{color}.gif', git_blob=inventory[path]))
    def download(record):
        target = output / record['name']
        data = target.read_bytes() if target.exists() else b''
        def valid(data):
            return hashlib.sha1(b'blob ' + str(len(data)).encode() + b'\0' + data).hexdigest() == record['git_blob']
        if not valid(data):
            data = get(f'https://raw.githubusercontent.com/PokeAPI/sprites/{commit}/sprites/pokemon/other/showdown/{record["path"]}')
            assert valid(data), record['path']
            assert data[:6] in (b'GIF87a', b'GIF89a'), record['path']
            target.write_bytes(data)
        record['sha256'] = hashlib.sha256(data).hexdigest()
        return record
    with concurrent.futures.ThreadPoolExecutor(max_workers=8) as pool:
        for index, _ in enumerate(pool.map(download, records), 1):
            if index % 100 == 0:
                print(f'Download verified: {index}/1544', flush=True)
    provenance = dict(repository='https://github.com/PokeAPI/sprites', commit=commit, directory='sprites/pokemon/other/showdown', credit='Pokémon Showdown / Smogon; Pokémon assets belong to their respective owners.', records=records)
    (output / 'provenance.json').write_text(json.dumps(provenance, indent=2) + '\n')
    with zipfile.ZipFile(output / 'showdown.zip', 'w') as archive:
        for record in records:
            archive.write(output / record['name'], 'sprites/battlesprites/' + record['name'])
    print(f'Complete: 1544 GIFs at {commit}', flush=True)

if __name__ == '__main__':
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument('output', type=Path)
    parser.add_argument('--commit')
    args = parser.parse_args()
    fetch(args.output, args.commit)
