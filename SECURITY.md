# Security Policy

## Supported Versions

| Version | Supported          |
| ------- | ------------------ |
| 4.6.x   | :white_check_mark: |
| < 4.6   | :x:                |

## Reporting a Vulnerability

If you discover a security issue or vulnerability with CYD HA Panel, please report it privately:

- **Do NOT** open a public issue on GitHub.
- Submit a report via GitHub's [Private Vulnerability Reporting](https://github.com/gubas/cyd_HA/security/advisories/new) if enabled, or contact the maintainer directly.

## Security Best Practices for Users

1. **Keep Secrets Private**: Never commit `secrets.yaml` to public Git repositories. The file is listed in `.gitignore` by default.
2. **API Encryption**: Always enable encryption keys in `secrets.yaml` (`cyd_ha_api_encryption_key`) for secure encrypted communications with Home Assistant.
3. **OTA Passwords**: Protect Over-The-Air updates with a strong password in `secrets.yaml` (`cyd_ha_ota_password`).
4. **Fallback Hotspot & QR Code**: Change the default fallback Wi-Fi AP password in `cyd_ha/substitutions.local.yaml` (`wifi_ap_password`) to prevent unauthorized access if the device loses connection to your main router. This setting is defined in substitutions so that the scannable Wi-Fi setup QR code can be generated at compile-time.
5. **Local Web Server Access**: The embedded ESPHome web server (`port 80`) and screen streaming endpoints (`/screen`, `/screenshot.bmp`, `/api/screen/page`) are accessible without authentication on your local network by default. If your local network is untrusted or you want restricted access, configure authentication under `web_server:` in `cyd_ha.yaml`.
6. **Privacy when Sharing Configs**: Keep all your personal Home Assistant entity IDs, Zigbee IEEE hardware addresses, printer serial numbers, and home location inside `cyd_ha/substitutions.local.yaml` (which is gitignored). Never put real credentials or personal identifiers in the public template `cyd_ha/substitutions.yaml`.
