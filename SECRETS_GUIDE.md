# 🔐 Guide de gestion des secrets partagés ESPHome

## Concept : Un seul secrets.yaml pour tous vos projets

Cette architecture permet d'utiliser **un seul fichier `secrets.yaml`** partagé entre **tous vos projets ESPHome**, tout en gardant chaque projet isolé et sécurisé.

## 📁 Structure recommandée

### Option A : secrets.yaml dans chaque projet (copie locale)

```
esphome/
├── cyd_HA/
│   ├── cyd_ha_refactored.yaml
│   └── secrets.yaml              ← Copie locale (ou symlink)
├── salon_display/
│   ├── salon.yaml
│   └── secrets.yaml              ← Même fichier (ou symlink)
└── garage_sensor/
    ├── garage.yaml
    └── secrets.yaml              ← Même fichier (ou symlink)
```

### Option B : secrets.yaml partagé (recommandé Windows)

```
esphome/
├── secrets.yaml                  ← FICHIER UNIQUE PARTAGÉ
├── cyd_HA/
│   └── cyd_ha_refactored.yaml    (référence ../secrets.yaml)
├── salon_display/
│   └── salon.yaml                (référence ../secrets.yaml)
└── garage_sensor/
    └── garage.yaml               (référence ../secrets.yaml)
```

**Sur Windows** : ESPHome cherche automatiquement `secrets.yaml` dans le dossier parent si absent localement.

## 🔑 Convention de nommage des secrets

### Règle : `{device_name}_{secret_type}`

Chaque secret est **préfixé par le nom du device** pour éviter les conflits.

### Exemples

| Device name | Secrets utilisés |
|-------------|------------------|
| `cyd_ha` | `cyd_ha_api_encryption_key`, `cyd_ha_ota_password`, `cyd_ha_ap_ssid`, `cyd_ha_ap_password` |
| `salon` | `salon_api_encryption_key`, `salon_ota_password`, `salon_ap_ssid`, `salon_ap_password` |
| `garage` | `garage_api_encryption_key`, `garage_ota_password`, `garage_ap_ssid`, `garage_ap_password` |

### Secrets globaux (non préfixés)

Certains secrets sont partagés par **tous** les devices :

```yaml
# Partagés globalement
wifi_ssid: "MonWiFi"           # Tous les devices utilisent le même WiFi
wifi_password: "..."

# Optionnel : MQTT partagé
mqtt_broker: "192.168.1.100"
mqtt_username: "esphome"
mqtt_password: "..."
```

## 📝 Exemple complet secrets.yaml

```yaml
# =============================================================================
# WiFi Global (tous les devices)
# =============================================================================
wifi_ssid: "MonWiFi24GHz"
wifi_password: "SuperSecurePassword123!"

# =============================================================================
# Device: cyd_ha (ESP32 CYD Touch Panel)
# =============================================================================
cyd_ha_api_encryption_key: "UBlyTwtLy37Uojq3/99P13a2B6SxWIBkk8RYvH9zM4Y="
cyd_ha_ota_password: "CydHaOTA2024!"
cyd_ha_ap_ssid: "CYD HA Fallback Hotspot"
cyd_ha_ap_password: "CydHaFallback123"

# =============================================================================
# Device: salon (ESP32 avec capteurs DHT22)
# =============================================================================
salon_api_encryption_key: "AbC123dEfG456hIjK789lMnO012pQrS345tUvW678xYz="
salon_ota_password: "SalonOTA2024!"
salon_ap_ssid: "Salon Fallback"
salon_ap_password: "SalonFallback456"

# =============================================================================
# Device: garage (ESP8266 avec relay)
# =============================================================================
garage_api_encryption_key: "XyZ987wVu654tSr321qPo098nMl765kJi543hGf210eD="
garage_ota_password: "GarageOTA2024!"
garage_ap_ssid: "Garage Fallback"
garage_ap_password: "GarageFallback789"

# =============================================================================
# MQTT Partagé (optionnel)
# =============================================================================
mqtt_broker: "192.168.1.50"
mqtt_username: "esphome"
mqtt_password: "MqttSecure2024!"
mqtt_port: "1883"
```

## 🔧 Utilisation dans les fichiers YAML

### Dans cyd_ha_refactored.yaml

```yaml
esphome:
  name: cyd_ha  # ← Nom du device

wifi:
  ssid: !secret wifi_ssid        # ← Global (non préfixé)
  password: !secret wifi_password

api:
  encryption:
    key: !secret cyd_ha_api_encryption_key  # ← Préfixé "cyd_ha_"

ota:
  password: !secret cyd_ha_ota_password     # ← Préfixé "cyd_ha_"

ap:
  ssid: !secret cyd_ha_ap_ssid              # ← Préfixé "cyd_ha_"
  password: !secret cyd_ha_ap_password
```

### Dans salon.yaml (autre projet)

```yaml
esphome:
  name: salon  # ← Nom différent

wifi:
  ssid: !secret wifi_ssid        # ← MÊME global
  password: !secret wifi_password

api:
  encryption:
    key: !secret salon_api_encryption_key  # ← Préfixé "salon_"

ota:
  password: !secret salon_ota_password     # ← Préfixé "salon_"
```

## 🛡️ Sécurité et bonnes pratiques

### ✅ À FAIRE

1. **Utiliser des mots de passe uniques** pour chaque device OTA
2. **Stocker backup de secrets.yaml** dans un gestionnaire de mots de passe
3. **Ajouter secrets.yaml au .gitignore** (déjà fait)
4. **Générer nouvelles API keys** pour chaque device (avec `esphome config`)
5. **Utiliser WPA2/WPA3** pour le WiFi

### ❌ À ÉVITER

1. **Ne JAMAIS commiter secrets.yaml** sur Git/GitHub
2. **Ne pas réutiliser les mots de passe OTA** entre devices
3. **Éviter les mots de passe faibles** (min 12 caractères)
4. **Ne pas partager secrets.yaml** publiquement
5. **Ne pas hardcoder les secrets** dans les YAML

## 🔄 Migration depuis l'ancien système

### Si vous aviez des secrets non préfixés

**Ancien (secrets.yaml)** :
```yaml
api_encryption_key: "ABC123..."
ota_password: "password"
```

**Nouveau (secrets.yaml)** :
```yaml
cyd_ha_api_encryption_key: "ABC123..."  # Ajoutez préfixe
cyd_ha_ota_password: "password"
```

**Mettre à jour le YAML** :
```yaml
# Ancien
api:
  encryption:
    key: !secret api_encryption_key

# Nouveau
api:
  encryption:
    key: !secret cyd_ha_api_encryption_key
```

## 🚀 Avantages de cette approche

| Avantage | Description |
|----------|-------------|
| **Centralisation** | Un seul fichier à gérer pour tous les projets |
| **Isolation** | Chaque device a ses propres secrets (préfixés) |
| **Sécurité** | Rotation facile des credentials par device |
| **Maintenance** | Changement WiFi → modifier une seule fois |
| **Scalabilité** | Ajouter nouveau projet = ajouter 4 lignes |

## 📦 Template pour nouveau projet

Quand vous créez un nouveau projet ESPHome (ex: `cuisine`) :

### 1. Ajouter dans secrets.yaml

```yaml
# Device: cuisine
cuisine_api_encryption_key: ""  # Sera généré
cuisine_ota_password: "CuisineOTA2024!"
cuisine_ap_ssid: "Cuisine Fallback"
cuisine_ap_password: "CuisineFallback123"
```

### 2. Dans cuisine.yaml

```yaml
esphome:
  name: cuisine

wifi:
  ssid: !secret wifi_ssid
  password: !secret wifi_password
  ap:
    ssid: !secret cuisine_ap_ssid
    password: !secret cuisine_ap_password

api:
  encryption:
    key: !secret cuisine_api_encryption_key

ota:
  password: !secret cuisine_ota_password
```

## 🔍 Vérification

Pour vérifier que vos secrets sont bien configurés :

```powershell
# Avec le script deploy.ps1
.\deploy.ps1 -Action secrets

# Manuellement
esphome config cyd_ha_refactored.yaml
```

Les secrets manquants ou mal nommés généreront une erreur.

## 📞 Troubleshooting

### Erreur : "Secret 'cyd_ha_api_encryption_key' not found"

**Cause** : Secret mal nommé ou absent de `secrets.yaml`

**Solution** :
1. Ouvrir `secrets.yaml`
2. Vérifier que `cyd_ha_api_encryption_key` existe
3. Vérifier l'orthographe exacte (sensible à la casse)

### Erreur : "Could not find secrets.yaml"

**Cause** : Fichier absent du dossier

**Solutions** :
- **Option A** : Copier `secrets.yaml` dans le dossier du projet
- **Option B** : Créer symlink vers secrets.yaml parent
- **Option C** : ESPHome cherche automatiquement dans dossier parent sur Windows

---

**Cette approche vous permet de gérer 10, 20, 50 devices ESPHome avec un seul fichier secrets.yaml !** 🎉
