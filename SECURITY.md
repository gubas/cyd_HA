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
2. **API Encryption**: Always enable encryption keys in `cyd_ha/substitutions.yaml` (`cyd_ha_api_encryption_key`) for secure communications with Home Assistant.
3. **OTA Passwords**: Protect Over-The-Air updates with a strong password (`cyd_ha_ota_password`).
4. **Fallback Hotspot**: Change the default fallback Wi-Fi AP password (`cyd_ha_ap_password`) to prevent unauthorized access if the device loses connection to your main router.
