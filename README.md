# ESP32 CYD Home Assistant Touch Panel

<img src="https://img.shields.io/badge/ESPHome-000000?style=for-the-badge&logo=esphome&logoColor=white" alt="ESPHome" /> <img src="https://img.shields.io/badge/Home_Assistant-41BDF5?style=for-the-badge&logo=home-assistant&logoColor=white" alt="Home Assistant" />

Un panneau tactile intelligent pour contrôler Home Assistant à l'aide d'un ESP32-2432S028R (CYD - Cheap Yellow Display).

## 📋 Fonctionnalités

- **Écran principal** : Affichage de l'heure, de la date et des conditions météorologiques
- **Capteurs rotatifs** : Affichage cyclique des données de température et d'humidité (intérieur/extérieur)
- **Menu de contrôle** : 8 boutons tactiles configurables pour contrôler des entités Home Assistant
- **Interface tactile responsive** : Détection précise avec calibration XPT2046
- **Connexion sécurisée** : API chiffrée, OTA protégé par mot de passe
- **Architecture modulaire** : Configuration organisée en fichiers séparés pour faciliter la maintenance

## 🛠️ Matériel requis

- **ESP32-2432S028R** (Cheap Yellow Display)
  - ESP32 (240 MHz dual-core)
  - Écran ILI9342 320x240 TFT
  - Contrôleur tactile XPT2046
  - LED RVB intégrée
  - Rétroéclairage PWM

## 📁 Structure du projet

```
cyd_HA/
├── cyd_ha_refactored.yaml   # ✅ Fichier principal (UTILISEZ CELUI-CI)
├── cyd_ha/                  # � Sous-dossier modules
│   ├── common.yaml          # 🎨 Ressources UI (fonts, colors, icons)
│   ├── hardware.yaml        # ⚙️ Configuration matérielle (SPI, touch, outputs)
│   ├── sensors.yaml         # 📊 Intégration capteurs Home Assistant
│   ├── buttons.yaml         # 🔘 Définitions des zones tactiles
│   └── display.yaml         # 🖥️ Logique de rendu UI
├── secrets.yaml             # 🔐 Credentials (partagé entre projets ESPHome)
├── secrets.yaml.example     # 📄 Template de secrets
├── deploy.ps1               # ⚡ Script déploiement PowerShell
├── SECRETS_GUIDE.md         # 🔐 Guide secrets partagés
├── INSTALLATION.md          # 🚀 Guide d'installation
├── ARCHITECTURE.md          # 🏗️ Architecture technique
├── CHECKLIST.md             # ☑️ Checklist déploiement
├── CHANGELOG.md             # 📝 Changements
└── README.md                # 📖 Documentation
```

**💡 Note importante** : `secrets.yaml` peut être **partagé entre tous vos projets ESPHome**. Les secrets sont préfixés par le nom du device (ex: `cyd_ha_api_encryption_key`). Voir `SECRETS_GUIDE.md` pour plus de détails.

## 🚀 Installation rapide

### 1. Prérequis

- [ESPHome](https://esphome.io/) installé
- Home Assistant fonctionnel avec API activée
- Connexion USB vers l'ESP32

### 2. Configuration

#### a) Créer `secrets.yaml`

Créez le fichier `secrets.yaml` (peut être partagé avec tous vos projets ESPHome) :

```yaml
# WiFi global (partagé)
wifi_ssid: "VOTRE_SSID"
wifi_password: "VOTRE_MOT_DE_PASSE_WIFI"

# Secrets spécifiques au projet "cyd_ha"
cyd_ha_api_encryption_key: "VOTRE_CLE_API"
cyd_ha_ota_password: "VOTRE_MOT_DE_PASSE_OTA"
cyd_ha_ap_ssid: "CYD HA Fallback Hotspot"
cyd_ha_ap_password: "CHANGEZ_MOI_12345"

# Pour d'autres projets, ajoutez des secrets préfixés :
# salon_api_encryption_key: "..."
# cuisine_ota_password: "..."
```

**Note** : Les secrets sont préfixés par le nom du device (`cyd_ha_*`) pour permettre un fichier `secrets.yaml` partagé entre tous vos projets ESPHome.

#### b) Personnaliser les entités

Éditez `cyd_ha_refactored.yaml` dans la section `substitutions` :

```yaml
substitutions:
  # Capteurs de température/humidité
  internal_temp_sensor: sensor.votre_capteur_temp
  internal_humidity_sensor: sensor.votre_capteur_humidity
  
  # Entités contrôlées par les boutons
  button1_service: cover.open_cover
  button1_entity: cover.votre_volet
  # ... etc
```

#### c) Télécharger la font Material Design Icons

```powershell
# PowerShell
Invoke-WebRequest -Uri "https://github.com/Templarian/MaterialDesign-Webfont/raw/master/fonts/materialdesignicons-webfont.ttf" -OutFile "materialdesignicons-webfont.ttf"
```

Ou téléchargez manuellement : [MaterialDesignIcons](https://github.com/Templarian/MaterialDesign-Webfont/blob/master/fonts/materialdesignicons-webfont.ttf)

### 3. Compilation et flash

#### Première installation (via USB)

```powershell
# Valider la configuration
esphome config cyd_ha_refactored.yaml

# Compiler et flasher via USB
esphome run cyd_ha_refactored.yaml
```

#### Mises à jour ultérieures (OTA sans fil)

```powershell
# Flash OTA (après première installation USB)
esphome run cyd_ha_refactored.yaml --device cyd_ha.local
```

## 🎨 Personnalisation

### Modifier les couleurs

Éditez `cyd_ha/common.yaml` :

```yaml
color:
  - id: black
    hex: '000000'
  - id: blue
    hex: '16afd9'  # Changez cette valeur
  - id: grey
    hex: '464646'
```

### Ajouter des icônes

1. Trouvez l'icône sur [Material Design Icons](https://pictogrammers.com/library/mdi/)
2. Ajoutez dans `cyd_ha/common.yaml` :

```yaml
image:
  - file: mdi:VOTRE_ICONE
    id: mon_icone
    resize: 40x40
    type: BINARY
```

### Modifier les boutons

Éditez `cyd_ha_refactored.yaml` (substitutions) et `cyd_ha/buttons.yaml` pour changer les services/entités.

## 🐛 Dépannage

### Erreur "Could not connect to WiFi"

- Vérifiez `secrets.yaml` (SSID/password corrects)
- Le WiFi 5GHz n'est pas supporté (utilisez 2.4GHz)

### Écran tactile ne répond pas

- Ajustez la calibration dans `cyd_ha/hardware.yaml` :

```yaml
touchscreen:
  calibration:
    x_min: 280  # Modifiez ces valeurs
    x_max: 3860
    y_min: 280
    y_max: 3860
```

### API encryption key invalide

Générez une nouvelle clé :

```powershell
esphome config cyd_ha_refactored.yaml
# La clé sera générée automatiquement si absente
```

### Capteurs affichent "--"

- Vérifiez que les `entity_id` dans `substitutions` correspondent aux entités Home Assistant
- Vérifiez la connexion API dans Home Assistant

## 📊 Architecture technique

### Flux de données

```
Home Assistant API
        ↓
  cyd_ha/sensors.yaml (import entités)
        ↓
  cyd_ha/display.yaml (logique rendering)
        ↓
    ESP32 Display (ILI9342)
```

### Gestion tactile

```
Touch XPT2046
        ↓
  cyd_ha/buttons.yaml (zones tactiles)
        ↓
    Lambda conditionnels
        ↓
    Home Assistant Service Calls
```

### Amélirations appliquées (refactorisation)

✅ **Sécurité**
- Tous les credentials déplacés dans `secrets.yaml`
- API et OTA sécurisés par références `!secret`

✅ **Robustesse**
- Timer display basé sur `millis()` (précis à 5s)
- Fallback météo avec lookup sécurisé (`.find()` au lieu de `[]`)
- Vérifications `has_state()` avant affichage des capteurs

✅ **Maintenabilité**
- Configuration modulaire (7 fichiers séparés)
- Commentaires détaillés
- Architecture claire et documentée

✅ **Performance**
- `fast_connect: true` pour WiFi rapide
- `power_save_mode: none` pour touch réactif
- `update_interval: 1s` pour affichage fluide

## 📝 Changelog

### v2.0 (Octobre 2025) - Refactorisation complète

- ♻️ Architecture modulaire (7 fichiers)
- 🔐 Sécurisation des credentials
- 🐛 Correction timer display (millis)
- 🛡️ Fallback robuste pour météo
- 📚 Documentation complète
- ⚡ Optimisations performance

### v1.0 (Original)

- ✨ Version initiale monolithique

## 🤝 Contribution

Pour améliorer ce projet :

1. Testez sur votre matériel
2. Signalez les bugs via issues
3. Proposez des améliorations
4. Partagez vos configurations personnalisées

## 📄 Licence

Ce projet est fourni "tel quel" sans garantie.
Utilisez-le, modifiez-le, partagez-le librement.

## 🔗 Ressources

- [ESPHome Documentation](https://esphome.io/)
- [Home Assistant](https://www.home-assistant.io/)
- [ESP32-2432S028R](https://github.com/witnessmenow/ESP32-Cheap-Yellow-Display)
- [Material Design Icons](https://pictogrammers.com/library/mdi/)

---

**Made with ❤️ for Home Assistant community**
