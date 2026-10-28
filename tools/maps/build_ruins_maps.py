#!/usr/bin/env python3
"""build_ruins_maps.py - Author the first Ruins rooms for nUndertale (T2).

Generates Tiled JSON maps (40x40 px tiles, 2x from 20x20 ruins tileset):
  - data/maps/ruins_fall.json     (R1 The Fall Room)
  - data/maps/ruins_registration.json (R1a Registration)
  - data/maps/ruins_panelhall.json    (R1b Panel Hall)
  - data/maps/ruins_floofy.json       (R2 Floofy)
  - data/maps/ruins_entrance.json (R3 Ruins Entrance)
  - data/maps/ruins_notices.json  (R8 Notices, CRT, Art Show)
  - data/maps/ruins_barky.json    (R9 Barky's Arena, all data-driven: C1-009)
"""

# Each room lives in tools/maps/rooms/<map id>.py; shared tables and helpers in tools/maps/ruins_common.py.
# A new room: add its module under rooms/, import it here and register it in main().

import json
import os
import sys

sys.path.insert(0, os.path.dirname(os.path.abspath(__file__)))

from ruins_common import ensure_aurora_glimpse_prop  # noqa: E402
from rooms.ruins_fall import build_ruins_fall  # noqa: E402
from rooms.ruins_registration import build_ruins_registration  # noqa: E402
from rooms.ruins_panelhall import build_ruins_panelhall  # noqa: E402
from rooms.ruins_floofy import build_ruins_floofy  # noqa: E402
from rooms.ruins_entrance import build_ruins_entrance  # noqa: E402
from rooms.ruins_switches import build_ruins_switches  # noqa: E402
from rooms.ruins_dummy import build_ruins_dummy  # noqa: E402
from rooms.ruins_cables import build_ruins_cables  # noqa: E402
from rooms.ruins_hallway import build_ruins_hallway  # noqa: E402
from rooms.ruins_notices import build_ruins_notices  # noqa: E402
from rooms.ruins_barky import build_ruins_barky  # noqa: E402
from rooms.ruins_bakesale import build_ruins_bakesale  # noqa: E402
from rooms.ruins_house import build_ruins_house  # noqa: E402
from rooms.ruins_house_hallway import build_ruins_house_hallway  # noqa: E402
from rooms.ruins_house_pepe import build_ruins_house_pepe  # noqa: E402
from rooms.ruins_basement import build_ruins_basement  # noqa: E402
from rooms.ruins_exit import build_ruins_exit  # noqa: E402
from rooms.ruins_booth import build_ruins_booth  # noqa: E402


def main():
    repo_root =os.path.dirname(os.path.dirname(os.path.dirname(os.path.abspath(__file__))))
    ensure_aurora_glimpse_prop(repo_root)
    maps_dir = os.path.join(repo_root, "data", "maps")
    os.makedirs(maps_dir, exist_ok=True)

    builders = {
        "ruins_fall.json": build_ruins_fall,
        "ruins_registration.json": build_ruins_registration,
        "ruins_panelhall.json": build_ruins_panelhall,
        "ruins_floofy.json": build_ruins_floofy,
        "ruins_entrance.json": build_ruins_entrance,
        "ruins_switches.json": build_ruins_switches,
        "ruins_dummy.json": build_ruins_dummy,
        "ruins_cables.json": build_ruins_cables,
        "ruins_hallway.json": build_ruins_hallway,
        "ruins_notices.json": build_ruins_notices,
        "ruins_barky.json": build_ruins_barky,
        "ruins_booth.json": build_ruins_booth,
        "ruins_bakesale.json": build_ruins_bakesale,
        "ruins_house.json": build_ruins_house,
        "ruins_house_hallway.json": build_ruins_house_hallway,
        "ruins_house_pepe.json": build_ruins_house_pepe,
        "ruins_basement.json": build_ruins_basement,
        "ruins_exit.json": build_ruins_exit,
    }

    print("Building Ruins maps...")
    for filename, builder in builders.items():
        filepath = os.path.join(maps_dir, filename)
        map_data = builder()
        with open(filepath, "w", encoding="utf-8") as f:
            # Keep canonical player-facing punctuation as literal UTF-8. The
            # runtime's compact JSON parser intentionally does not implement
            # ``\uXXXX`` escapes, which Python emits by default.
            json.dump(map_data, f, indent=2, ensure_ascii=False)
            f.write("\n")
        print(f"  -> Wrote {filename} ({map_data['width']}x{map_data['height']} tiles)")
    print("Done!")


if __name__ == "__main__":
    main()


