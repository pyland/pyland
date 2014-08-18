"""
Usage:
  remap <map_file> <out_file> <old_directory> <new_directory>

Modifies <map_file> (JSON) in place, changing all usages of
tiles from <old_directory> to mappings from <new_directory>.

This requires all tiles in <old_directory> to be in
<new_directory>.
"""


from copy import deepcopy
import json
from pprint import pprint
import struct

try:
    import docopt
except ImportError:
    raise SystemExit("docopt needs to be installed (pip install docopt)")

try:
    from pathlib import Path
except ImportError:
    raise SystemExit("Python needs to be at least version 3.4 or have pathlib installed")


def shitty_get_image_dimensions(path):
    with path.open("rb") as image:
        data = image.read(24)

    width, height = struct.unpack('>LL', data[16:24])
    return int(width), int(height)


def shitty_parse_fml_line(line):
    key, val = line.split(": ")
    val = int(val)
    return key, val


def shitty_parse_fml(lines):
    return dict(map(shitty_parse_fml_line, lines))


def shitty_diff_fml(old_path, new_path):
    with old_path.open() as original_fml:
        old = shitty_parse_fml(original_fml)

    with new_path.open() as new_fml:
        new = shitty_parse_fml(new_fml)

    # Old must be subset of new
    return {old[key]: new[key] for key in old.keys()}


def all_relative_to(paths, dir):
    for path in paths:
        yield path.relative_to(dir)



if __name__ == "__main__":
    arguments = docopt.docopt(__doc__, version=(0, 1, 0))

    map_file = Path(arguments["<map_file>"])
    out_file = Path(arguments["<out_file>"])
    old_dir  = Path(arguments["<old_directory>"])
    new_dir  = Path(arguments["<new_directory>"])

    # All FML files must be in new directory too
    changed_fmls = all_relative_to(old_dir.glob("*.fml"), old_dir)

    with map_file.open() as mapfile:
        mapdata = json.load(mapfile)

    # new_tilesets should mutate the JSON but the old stuff should not
    new_tilesets = {Path(tileset["image"]).stem: tileset for tileset in mapdata["tilesets"]}
    old_tilesets = deepcopy(new_tilesets)

    min_gid_so_far = 1
    for image_path in new_dir.glob("*.png"):
        if image_path.stem not in new_tilesets:
            continue

        tileset = new_tilesets[image_path.stem]
        tile_width  = int(tileset["tilewidth"])
        tile_height = int(tileset["tileheight"])

        width, height = shitty_get_image_dimensions(image_path)
        tileset["imagewidth"]  = width
        tileset["imageheight"] = height

        size = width // tile_width * height // tile_height

        tileset["firstgid"] = min_gid_so_far
        min_gid_so_far += size

    # Calculate gid changes
    gid_to_gid = {}
    for relpath in changed_fmls:
        offset_new = new_tilesets[relpath.stem]["firstgid"]
        offset_old = old_tilesets[relpath.stem]["firstgid"]

        id_to_id = shitty_diff_fml(old_dir / relpath, new_dir / relpath)
        gid_to_gid.update({k+offset_old: v+offset_new for k, v in id_to_id.items()})

    # Mutate
    for layer in mapdata["layers"]:
      if layer["type"] == "tilelayer":
          layer["data"] = [gid_to_gid.get(x, x) for x in layer["data"]]

    # Export
    with out_file.open("w") as mapfile:
      json.dump(mapdata, mapfile)
