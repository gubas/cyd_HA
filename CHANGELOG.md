# Changelog

All notable changes to this project will be documented in this file.

The format is based on [Keep a Changelog](https://keepachangelog.com/en/1.1.0/).

## [4.5.0] - 2026-10-07

### Added
- **On-Screen Wi-Fi QR Code Setup**: Automatically displays an interactive Wi-Fi configuration screen with a scannable QR code whenever the device is disconnected or enters fallback Access Point mode (`CYD-HA-Fallback`).
- **Captive Portal & Web Setup**: Connect instantly with your smartphone to enter your local Wi-Fi credentials without touching code.
- **Improv Serial USB Provisioning**: Full support for ESP Web Tools USB provisioning via browser ([web.esphome.io](https://web.esphome.io)).
- **Network Diagnostics & IP Indicator**:
  - Transient IP address notification on boot.
  - Persistent IP address indicator displayed in the Classic Menu footer.
- **Web Flasher Integration**: Browser-based flashing using ESP Web Tools located in `web_flasher/`.
- **Hardware UI Gallery**: Added physical display captures and English documentation to README.

### Changed
- **Config Standardization**: Standardized main config to `cyd_ha.yaml` with backward-compatible `cyd_ha_refactored.yaml` shim.
- **CI/CD Acceleration**: Added PlatformIO build cache to GitHub Actions workflows and automated binary release publishing.

## [4.2.0] - 2026-10-07

### Added
- **Live Screen Mirror & Streaming (`/screenshot.bmp`)**: Direct zero-RAM HTTP chunked framebuffer streaming of the CYD display in pixel-perfect 8-bit indexed BMP format (RGB332 palette). Works without requiring PSRAM!
- **Interactive Web Simulator (`/screen`)**: Beautiful embedded web interface simulating the CYD hardware enclosure, real-time screen mirror (1s auto-refresh), virtual touch interactions, and telemetry.
- **Remote Display API (`/api/screen/page`)**: HTTP endpoints to dynamically switch pages (`?page=0..4`), toggle the configuration menu (`?menu=1` / `?action=toggle_menu`), and cycle forward/backward.

## [4.1.0] - 2026-10-07

### Added
- **Energy & Climate Page**: New 2x2 grid dashboard page monitoring:
  - Global home power (W / kW auto-scaling)
  - Daily electricity cost (€)
  - Air conditioning instantaneous power (W / kW auto-scaling)
  - Air conditioning energy consumption today (kWh)
- **Embedded Web Server**: Activated modern ESPHome v3 web interface on port 80.
- **Dynamic 5-Dot Pagination**: Extended pagination dots to support up to 5 dynamic pages.
- **Euro Currency Symbol**: Added `€` glyph to font definitions in `common.yaml`.
- **Automatic GitHub Releases**: Added `.github/workflows/release.yml` workflow to automatically publish releases on tag push.

### Changed
- **Ultra-Smooth Screen Refresh**: Configured full framebuffer (`buffer_size: 1.0`) and 40MHz SPI data rate on `mipi_spi`, eliminating multi-pass slicing and screen stutter.

## [4.0.0] - 2026-10-05

### Added
- **Windows Build & Upload Script**: Added `esphome-build-upload.bat` helper for local compilation and flashing on Windows.
- **Bedroom Sensor Blocks**: Configured blocks 3 & 4 with dedicated bedroom climate sensors and icons.

### Changed
- **Display Driver Migration**: Migrated display platform from deprecated `ili9xxx` to modern `mipi_spi` using model `ESP32-2432S028-9342` and `rotation: 270`.
- **Git Ignore**: Updated `.gitignore` to ignore all `.ttf` font files.

### Fixed
- **i18n Cleanup**: Removed duplicate button definitions in `cyd_ha/i18n/en.yaml`.

## [3.5.0] - 2025-12-05

### Added
- **Media Page Improvements**:
  - Text wrapping for Artist and Title (automatically adjusts to screen width)
  - Dynamic pagination dots: now shows 2, 3, or 4 dots depending on available pages (skips inactive Media/Printer pages)
  - New layout: Clean look without header title, larger fonts, centered status
- **Media Configuration**: `media_player_entity` variable in `substitutions.yaml`
- **Dynamic Cycling**: Pages are now skipped if their corresponding entity is inactive (Media or Printer)

## [3.4.0] - 2025-12-05

### Added
- **4-block sensor grid**: New 2x2 grid layout on Sensors page (up from 2 blocks)
- **Unified configuration**: All entity IDs now in `substitutions.yaml` with consistent naming (`*_entity` suffix)
- **Sensor block labels**: Added `sensor_bloc1_label` to `sensor_bloc4_label` in i18n files

### Changed
- **Variable naming convention**: Harmonized all variables:
  - Buttons: `button*` → `btn*` (e.g., `btn1_service`, `btn1_entity`)
  - Sensors: `internal_temp_sensor` → `sensor_bloc1_temp_entity`
  - Weather: `rain_chance` → `rain_chance_entity`
  - Printer: `printer_progress` → `printer_progress_entity`
- **Sensors page**: Removed title to maximize space for 4 sensor cards
- **Substitutions organization**: Clear sections with visual separators
- **i18n files**: Reorganized with consistent structure (FR, EN, ES)

### Fixed
- **Menu timeout bug**: Button 8 (Back) now properly resets menu timer when opening menu
- **Button action timeout**: Pressing any menu button now resets the 10s timeout

---

## [3.3.0] - 2025-12-04

### Added
- **GitHub Actions CI/CD**: Automatic ESPHome build validation on push/PR
- **Build status badge**: Added to README

### Changed
- **3D Printer config**: Migrated all 9 BambuLab entity IDs to `substitutions.yaml`
- **Single-file printer change**: Switch printers by editing one file only

### Removed
- `display_main.yaml`: Deleted obsolete file

---

## [3.2.0] - 2025-11-15

### Changed
- **Rain forecast bars**: Sized by interval (narrower for 5min, wider for 10min)
- **Time labels**: Added minute indicators under each rain bar (0, 5, 10... 55)
- **Better spacing**: Adjusted to avoid overlap between labels and "Next rain" text

---

## [3.1.0] - 2025-11-10

### Added
- **Full i18n support**: FR, EN, ES language packs with all UI strings
- **AI contribution guide**: `.github/copilot-instructions.md`

### Changed
- **Menu layout**: Removed header to maximize button space
- **Icon states**: Blue = active, Grey = inactive
- **Button 7**: Now functional with light bulb icon
- **i18n policy**: Include-only, no local overrides in `substitutions.yaml`

---

## [3.0.0] - 2025-10-25

### Added
- **Rain radar visualization**: 9 colored rectangles showing 0-55min forecast
- **Météo-France integration**: Direct parsing of `1_hour_forecast` attribute
- **Color-coded rain intensity**: Empty=dry, light/medium/dark blue = light/moderate/heavy rain
- **"Next rain" text**: Shows when rain is expected or "No rain planned"
- **Debug logging**: ESPHome logs with `rain_forecast` tag

---

## [2.2.0] - 2025-10-20

### Added
- **Weather icons**: 14 MDI weather conditions with animation
- **Extended icon set**: 20x20 icons for timer, clock, nozzle, bed, temperature, etc.

### Changed
- **Weather page**: HA-style layout with large icon, temp/humidity block, alerts line
- **Printer page redesign**:
  - Large 3D printer icon
  - Scrolling filename (if > 28 chars)
  - Larger progress bar (24px)
  - Grid layout for temps and times
- **Wind display**: Cardinal direction (N/NE/E/SE/S/SW/W/NW) + speed

---

## [2.1.0] - 2025-10-15

### Added
- **3 auto-cycling pages** (8s interval): Weather / Sensors / Printer
- **Weather alerts**: Real-time Météo-France vigilance (Wind/Rain/Storm/Snow/Flood)
- **Page indicator**: 3 dots at bottom, active page highlighted
- **Auto-return**: 30s timeout from menu back to page cycle

### Changed
- **Global header**: Device name + date/time on all data pages
- **Printer page**: File truncation, clean progress bar, temp display

---

## [2.0.0] - 2025-10-10

### Changed
- **Modular architecture**: Split into 7 files
- **Secured credentials**: Moved to `secrets.yaml`
- **Display timer fix**: Using millis() properly
- **Robust fallback**: Better weather entity handling

---

## [1.0.0] - 2025-10-01

### Added
- Initial monolithic release
- Basic touchscreen control
- Home Assistant integration
