## Purpose

Short, actionable guidance for AI coding assistants working on this repository (ESPHome configs for an ESP32 touch panel). Focus on small, precise edits unless explicitly asked to refactor C++ lambdas.

## Entry point

- Primary file to edit/build: `cyd_ha_refactored.yaml` (imports the modular files in `cyd_ha/`).
- Do not edit `secrets.yaml` in the repo; it is intended to be provided locally and contains credentials.

## Key files and what they contain

- `cyd_ha/common.yaml`: fonts, colors, and image/icon assets. Change colors or add icons here.
- `cyd_ha/hardware.yaml`: pin mappings, SPI buses, touchscreen calibration (`touchscreen.calibration`) and PWM outputs.
- `cyd_ha/substitutions.yaml`: centralized entity IDs, i18n includes, and button service mappings — first place to change Home Assistant entity ids.
- `cyd_ha/buttons.yaml`: touch zones → Home Assistant service calls. Edits here change what a button does.
- `cyd_ha/display_pages.yaml`: active `display` component with the large C++ `lambda` (page cycle, menu, rain parsing). Keep edits minimal and test on device.
- `cyd_ha/sensors.yaml`: Home Assistant sensors/text_sensors used by the display (weather, alerts, printer, button states).

## Project conventions / patterns

- Modular YAML: the project uses a single top-level YAML (`cyd_ha_refactored.yaml`) that includes smaller module files in `cyd_ha/`.
- Centralized substitutions: put user-tunable Home Assistant entity ids and labels in `cyd_ha/substitutions.yaml` (preferred over editing lambdas).
- i18n: language packs live in `cyd_ha/i18n/*.yaml`. Enable exactly one pack by uncommenting the `<<: !include i18n/<lang>.yaml` line at the top of `cyd_ha/substitutions.yaml`. Do not override i18n_* keys locally.
- C++ lambdas: `display_main.yaml` uses heavy lambdas (the rendering logic). Avoid sweeping refactors here; limit changes to text, colors, or small logic unless a full review is requested.

## Important implementation details agents should know

- The rain-forecast parsing lives inside the display lambda (`display_pages.yaml`) and expects a sensor attribute (`next_rain_forecast_data`) containing keys like `'0 min'`, `'5 min'`, etc.; code handles single/double quotes and falls back to "Temps sec". See the `extract_forecast` helper in that file.
- Page state and menu flags are exposed via `id(current_page)` and `id(show_return_page)` in lambdas—changes to navigation should preserve these ids.
- UI text/fonts: fonts are declared in `common.yaml` (`id: date, hour, info, buttons, fontmeteo`). When adding new text or icons, reference these ids.
- Touch calibration: `hardware.yaml` exposes calibration values (`x_min`, `x_max`, etc.). For tactile issues, update those values rather than rewriting touch logic.

## Build, deploy, and debug (practical commands)

- Build/compile the configured firmware (from repo root):

  - `esphome compile cyd_ha_refactored.yaml`

- Upload locally (USB):

  - `esphome run --upload-port /dev/ttyUSB0 cyd_ha_refactored.yaml` (replace port)

- OTA upload (device must be reachable and OTA enabled):

  - `esphome upload --ota cyd_ha_refactored.yaml`

- Live logs for debugging:

  - `esphome logs cyd_ha_refactored.yaml` (look for tags like `rain_forecast` used by the display code)

Note: the README documents setup and the requirement to create a `secrets.yaml` with `cyd_ha_api_encryption_key` and `cyd_ha_ota_password`.

## Safety and source-control rules

- Never commit `secrets.yaml` or real credentials. Use a local `secrets.yaml` (shared across ESPHome projects) and `.gitignore` for it.
- Keep changes to `display_main.yaml` minimal and tested on hardware — lambdas run on-device and can crash the firmware if invalid C++ is introduced.

## Small, concrete editing patterns (examples)

- To change the primary UI color, update `hex` under `color:` in `cyd_ha/common.yaml` (`id: blue`).
- To add or change a button action: edit `buttonN_service` and `buttonN_entity` in `cyd_ha/substitutions.yaml` and tweak `cyd_ha/buttons.yaml` if you must adjust touch zones.
- To adjust touchscreen sensitivity/calibration: update `calibration` values inside `cyd_ha/hardware.yaml`.
- To troubleshoot rain rectangles: check the sensor referenced in `substitutions.yaml` (e.g., `next_rain_forecast_data`) and tail logs (`esphome logs`) to inspect the raw payload (display code emits a `rain_forecast` debug log once per cycle).

## When to ask for human review

- Any change that edits the lambdas in `display_main.yaml` beyond small UI tweaks.
- Any change to `hardware.yaml` pin assignments (risk of bricking or hardware conflict).
- Introductions of new dependencies or components not already present in repo (describe why and add to README).

---
If anything here is unclear or you'd like a shorter/longer variant, tell me which section to expand or examples to add and I'll iterate. 
