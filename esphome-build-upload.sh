#!/usr/bin/env bash
set -euo pipefail

# Simple helper to compile and upload the ESPHome config.
# Usage:
#   ./esphome-build-upload.sh [config.yaml]
# Default config is cyd_ha_refactored.yaml if not provided.

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
esphome upload "$CONFIG_FILE"

echo "Done."
