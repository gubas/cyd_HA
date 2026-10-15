#!/usr/bin/env python3
import os
import re
import sys

def main():
    if len(sys.argv) < 2:
        print("Usage: update-version.py <version>", file=sys.stderr)
        sys.exit(1)

    version = sys.argv[1].lstrip('v').strip()
    v_version = f"v{version}"
    print(f"Updating codebase to version: {v_version} ({version})")

    # 1. cyd_ui.h
    cyd_ui_path = "cyd_ui.h"
    if os.path.exists(cyd_ui_path):
        with open(cyd_ui_path, "r", encoding="utf-8", newline="") as f:
            content = f.read()
        content = re.sub(r'#define\s+CYD_VERSION\s+"v[^"]+"', f'#define CYD_VERSION "{v_version}"', content)
        with open(cyd_ui_path, "w", encoding="utf-8", newline="") as f:
            f.write(content)
        print(f"OK: Updated {cyd_ui_path}")

    # 2. cyd_ha/substitutions.yaml
    subs_path = os.path.join("cyd_ha", "substitutions.yaml")
    if os.path.exists(subs_path):
        with open(subs_path, "r", encoding="utf-8", newline="") as f:
            content = f.read()
        content = re.sub(r'project_version:\s*"v[^"]+"', f'project_version: "{v_version}"', content)
        with open(subs_path, "w", encoding="utf-8", newline="") as f:
            f.write(content)
        print(f"OK: Updated {subs_path}")

    # 3. cyd_screen_streamer.h
    streamer_path = "cyd_screen_streamer.h"
    if os.path.exists(streamer_path):
        with open(streamer_path, "r", encoding="utf-8", newline="") as f:
            content = f.read()
        content = re.sub(r'<span class="badge">v[^<]+</span>', f'<span class="badge">{v_version}</span>', content)
        content = re.sub(r'CYD Panel v[0-9.]+', f'CYD Panel {v_version}', content)
        with open(streamer_path, "w", encoding="utf-8", newline="") as f:
            f.write(content)
        print(f"OK: Updated {streamer_path}")

    # 4. README.md
    readme_path = "README.md"
    if os.path.exists(readme_path):
        with open(readme_path, "r", encoding="utf-8", newline="") as f:
            content = f.read()
        content = re.sub(r'> Current version:\s*\*\*v[^*]+\*\*', f'> Current version: **{v_version}**', content)
        with open(readme_path, "w", encoding="utf-8", newline="") as f:
            f.write(content)
        print(f"OK: Updated {readme_path}")

    # 5. web_flasher/manifest.json
    manifest_path = os.path.join("web_flasher", "manifest.json")
    if os.path.exists(manifest_path):
        with open(manifest_path, "r", encoding="utf-8", newline="") as f:
            content = f.read()
        content = re.sub(r'"version":\s*"[^"]+"', f'"version": "{version}"', content)
        with open(manifest_path, "w", encoding="utf-8", newline="") as f:
            f.write(content)
        print(f"OK: Updated {manifest_path}")

    print("Version update complete.")

if __name__ == "__main__":
    main()
