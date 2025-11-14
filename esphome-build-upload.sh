#!/usr/bin/env bash
set -euo pipefail

# Simple helper to compile and upload the ESPHome config.
# Usage:
#   ./esphome-build-upload.sh [OPTIONS] [config.yaml]
#   ./esphome-build-upload.sh                      # OTA upload (default)
#   ./esphome-build-upload.sh -u                   # USB upload (/dev/ttyUSB0)
#   ./esphome-build-upload.sh -u /dev/ttyACM0      # USB upload (custom port)
#   ./esphome-build-upload.sh -h                   # Show help
# Default config is cyd_ha_refactored.yaml if not provided.

show_help() {
  cat << EOF
Usage: ${0##*/} [OPTIONS] [config.yaml]

Options:
  -u [PORT]   Upload via USB (default: /dev/ttyUSB0)
  -h          Show this help message

Examples:
  ${0##*/}                        # OTA upload with default config
  ${0##*/} -u                     # USB upload with /dev/ttyUSB0
  ${0##*/} -u /dev/ttyACM0        # USB upload with custom port
  ${0##*/} custom.yaml            # OTA with custom config
  ${0##*/} -u custom.yaml         # USB with custom config
EOF
  exit 0
}

# Parse options
USB_MODE=false
USB_PORT="/dev/ttyUSB0"

while getopts "hu:" opt; do
  case "$opt" in
    h) show_help ;;
    u) USB_MODE=true
       if [[ -n "${OPTARG:-}" && "$OPTARG" != -* ]]; then
         USB_PORT="$OPTARG"
       fi
       ;;
    *) show_help ;;
  esac
done
shift $((OPTIND-1))

# Activate local ESPHome virtualenv if present (as requested)
# Uses: source ~/esphome-venv/bin/activate
if [[ -f "$HOME/esphome-venv/bin/activate" ]]; then
  # shellcheck disable=SC1090
  source "$HOME/esphome-venv/bin/activate"
fi

if ! command -v esphome >/dev/null 2>&1; then
  echo "Error: 'esphome' CLI not found. Install with: pip install esphome" >&2
  exit 127
fi

# Update ESPHome to latest version
echo "Checking for ESPHome updates..."
pip install --upgrade esphome --quiet
echo "ESPHome version: $(esphome version)"

SCRIPT_DIR="$(cd -- "$(dirname -- "${BASH_SOURCE[0]}")" >/dev/null 2>&1 && pwd)"
cd "$SCRIPT_DIR"

CONFIG_FILE="${1:-cyd_ha_refactored.yaml}"

if [[ ! -f "$CONFIG_FILE" ]]; then
  echo "Error: Config file not found: $CONFIG_FILE" >&2
  exit 1
fi

echo "Validating $CONFIG_FILE..."
esphome config "$CONFIG_FILE"

echo "Compiling $CONFIG_FILE..."
esphome compile "$CONFIG_FILE"

echo "Uploading $CONFIG_FILE..."
if [[ "$USB_MODE" == true ]]; then
  echo "USB mode: using port $USB_PORT"
  esphome upload --device "$USB_PORT" "$CONFIG_FILE"
else
  esphome upload "$CONFIG_FILE"
fi

echo "Done."
