# ESP32 CYD Home Assistant Touch Panel

<img src="https://img.shields.io/badge/ESPHome-000000?style=for-the-badge&logo=esphome&logoColor=white" alt="ESPHome" /> <img src="https://img.shields.io/badge/Home_Assistant-41BDF5?style=for-the-badge&logo=home-assistant&logoColor=white" alt="Home Assistant" />

Un panneau tactile intelligent pour contrôler Home Assistant à l'aide d'un ESP32-2432S028R (CYD - Cheap Yellow Display).

## 📋 Fonctionnalités

- **Affichage multi-pages** : 3 écrans défilant automatiquement toutes les 8 secondes
  - **Page Météo** : Conditions météo actuelles avec grande icône animée, température extérieure, pluie, vent, neige, gel et alertes Météo-France en temps réel (vigilance jaune/orange/rouge)
    - 🌧️ **Prévisions de pluie** : 9 rectangles colorés représentant les prévisions minute par minute (0-55 min) avec code couleur intuitif (vide=sec, bleu clair/moyen/foncé=pluie faible/modérée/forte)
    - 📍 **Texte "Prochaine pluie"** : Annonce automatique de la prochaine pluie ou "Pas de pluie prévue"
  - **Page Capteurs** : Températures et humidité de 2 zones (Salon/Cuisine et Bureau)
  - **Page Imprimante** : État BambuLab en temps réel (fichier, progression, températures buse/lit, temps restant)
- **Menu de contrôle** : Accessible au toucher, 8 boutons tactiles configurables pour contrôler des entités Home Assistant (volets, lumières, imprimante 3D)
- **En-tête global** : Nom du device et date/heure (JJ/MM HH:MM) sur toutes les pages
- **Interface tactile responsive** : Détection précise avec calibration XPT2046
- **Connexion sécurisée** : API chiffrée, OTA protégé par mot de passe
- **Architecture modulaire** : Configuration organisée en fichiers séparés pour faciliter la maintenance
- **Auto-retour** : Retour automatique au cycle d'affichage après 30s d'inactivité dans le menu

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
├── cyd_ha/                  # 📂 Sous-dossier modules
│   ├── common.yaml          # 🎨 Ressources UI (fonts, colors, icons)
│   ├── hardware.yaml        # ⚙️ Configuration matérielle (SPI, touch, outputs)
│   ├── sensors.yaml         # 📊 Intégration capteurs Home Assistant
│   ├── buttons.yaml         # 🔘 Définitions des zones tactiles
│   └── display_pages.yaml   # 🖥️ Logique de rendu UI multi-pages
├── secrets.yaml             # 🔐 Credentials (partagé entre projets ESPHome)
├── secrets.yaml.example     # 📄 Template de secrets
├── materialdesignicons-webfont.ttf  # � Police d'icônes météo
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
  internal_temp_sensor: sensor.votre_capteur_temp_salon
  internal_humidity_sensor: sensor.votre_capteur_humidity_salon
  int2_temp_sensor: sensor.votre_capteur_temp_bureau
  int2_humidity_sensor: sensor.votre_capteur_humidity_bureau
  outside_temp_sensor: sensor.votre_capteur_temp_exterieur
  
  # Météo
  weather_entity: weather.votre_ville
  freeze_chance: sensor.votre_ville_freeze_chance
  snow_chance: sensor.votre_ville_snow_chance
  rain_chance: sensor.votre_ville_rain_chance
  
  # Entités contrôlées par les boutons
  button1_service: cover.open_cover
  button1_entity: cover.votre_volet
  # ... etc
```

**Note sur les prévisions de pluie** : Le capteur `sensor.next_rain` doit avoir un attribut `1_hour_forecast` contenant un dictionnaire avec les clés `'0 min'`, `'5 min'`, etc. (format API Météo-France). Aucune configuration supplémentaire dans Home Assistant n'est requise - tout est parsé côté ESPHome.

#### c) Télécharger la font Material Design Icons

Ou téléchargez manuellement : [MaterialDesignIcons](https://github.com/Templarian/MaterialDesign-Webfont/blob/master/fonts/materialdesignicons-webfont.ttf)


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

### Capteurs affichent "--"

- Vérifiez que les `entity_id` dans `substitutions` correspondent aux entités Home Assistant
- Vérifiez la connexion API dans Home Assistant

## 📊 Architecture technique

### Flux de données

```
Home Assistant API
        ↓
  cyd_ha/sensors.yaml (import entités: météo, capteurs, imprimante, alertes)
        ↓
  cyd_ha/display_pages.yaml (logique rendering multi-pages avec auto-cycle 8s)
        ↓
    ESP32 Display (ILI9342 - 320x240, rotation 90°)
```

### Pages et navigation

```
3 Pages en cycle automatique (8s):
┌─────────────────────────────────────┐
│ Page 0: Météo                       │
│  - Grande icône météo (MDI)         │
│  - Alertes Météo-France (🔴🟠🟡)    │
│  - Prévisions pluie: 9 rectangles   │
│    colorés (0-55min) + texte        │
│    "Prochaine pluie: X min"         │
│  - Temp/Pluie/Vent/Neige/Gel        │
│  - Icônes 20x20 alignées            │
├─────────────────────────────────────┤
│ Page 1: Capteurs Maison             │
│  - Salon/Cuisine (temp + humidité)  │
│  - Bureau (temp + humidité)         │
│  - Cartes avec icônes               │
├─────────────────────────────────────┤
│ Page 2: Imprimante BambuLab         │
│  - Nom fichier (tronqué si > 26c)   │
│  - Barre progression (sans %)       │
│  - État / Temps restant / Fin       │
│  - Températures buse/lit            │
└─────────────────────────────────────┘

Touch écran → Menu 8 boutons (30s timeout)
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

### v3.0 (Octobre 2025) - Prévisions de pluie Météo-France

- 🌧️ **Visualisation radar de pluie** :
  - **9 rectangles** (22x8px) représentant les prévisions de pluie à 0, 5, 10, 15, 20, 25, 35, 45, 55 minutes
  - Intégration directe de l'API Météo-France via attribut `1_hour_forecast` du capteur `sensor.next_rain`
  - **Code couleur intuitif** :
    - Rectangle **vide** (contour gris) : Temps sec
    - **Bleu clair** (100, 150, 200) : Pluie faible
    - **Bleu moyen** (50, 100, 180) : Pluie modérée
    - **Bleu foncé** (0, 50, 150) : Pluie forte
  - **Parsing intelligent** côté ESPHome (aucune configuration Home Assistant requise)
  - Support des formats JSON avec guillemets simples ou doubles
- 📍 **Texte "Prochaine pluie"** :
  - Affiché juste sous les rectangles
  - "Prochaine pluie: X min" si pluie détectée dans l'heure
  - "Pas de pluie prévue" si toutes les cases sont "temps sec"
  - Centré et espacé pour ne pas chevaucher la grille météo
- 🔍 **Debug dans les logs** :
  - Logging ESPHome (tag `rain_forecast`) pour diagnostiquer le format de données reçu
  - Pas d'affichage debug à l'écran (interface propre)
- ⚡ **Performance optimisée** :
  - Un seul `text_sensor` pour récupérer l'attribut complet
  - Parsing avec lambdas C++ efficaces (recherche de sous-chaînes)
  - Normalisation insensible à la casse pour robustesse

### v2.2 (Octobre 2025) - Style HA et page imprimante redessinée

- 🌤️ **Page météo style Home Assistant** :
  - Grande icône météo à gauche (70px Material Design Icons)
  - Bloc température + humidité à droite avec petites icônes
  - Ligne alertes météo centrée sous l'icône avec agrégation multi-sources
  - Grille 4x2 compacte : Pluie, Vent (direction cardinale N/NE/E/etc + vitesse), Neige, Pression atmosphérique, Gel, Lever/Coucher soleil (HH:MM)
  - Icônes 20x20 avec valeurs alignées
- 🖨️ **Page imprimante BambuLab redesignée** :
  - Icône imprimante 3D grande à gauche (40x40)
  - Nom de fichier à droite avec **défilement automatique** si > 28 caractères (vitesse 150ms/caractère)
  - État de l'impression sous le nom
  - **Barre de progression agrandie** (24px de hauteur) avec pourcentage centré
  - Grille style météo avec icônes :
    - Ligne 1: ⏱️ Temps restant (min) | 🕐 Heure de fin (HH:MM)
    - Ligne 2: 🔧 Température buse (actuelle/cible°) | 🔥 Température lit (actuelle/cible°)
  - Format compact sans "C" (ex: "220/210°")
  - Espacements optimisés pour éviter les chevauchements
- 📐 **Alignements parfaits** :
  - Tous les textes avec icônes utilisent `CENTER_LEFT` avec offset +2px
  - Colonne gauche COL1_X=15, colonne droite COL2_X=125
  - Grilles centrées et symétriques
- 🎨 **Palette étendue** : Ajout couleur `white` pour futures utilisations
- 🆕 **Nouvelles icônes** (20x20) :
  - timer_small (mdi:timer-outline), clock_end_small (mdi:clock-end)
  - nozzle_small (mdi:printer-3d-nozzle), bed_small (mdi:radiator)
  - temp_small (mdi:thermometer), humidity_small (mdi:water-percent)
  - pressure_small (mdi:gauge), sunrise_small/sunset_small
- 🛠️ **Optimisations** : Simplification du code (pas de buffers statiques inutiles pour températures)

### v2.1 (Octobre 2025) - Interface multi-pages et alertes météo

- 🔄 **3 pages auto-cycliques** (8s) : Météo / Capteurs / Imprimante 3D
- 🌤️ **Page météo améliorée** :
  - Grande icône météo avec 14 conditions (Material Design Icons)
  - Alertes Météo-France en temps réel (Vent/Pluie/Orages/Neige/Inondation) avec niveaux (Jaune/Orange/Rouge)
  - Affichage compact avec icônes 20x20 alignées : température ext., pluie, vent, neige, gel
  - Capteur vitesse du vent depuis attribut weather entity
- 🏠 **Page capteurs** : 2 zones (Salon/Cuisine + Bureau) avec température et humidité
- 🖨️ **Page imprimante BambuLab** :
  - Nom fichier avec troncature intelligente
  - Barre de progression sans texte (clean)
  - État, temps restant, heure de fin
  - Températures buse/lit (actuelle/cible)
- 📱 **En-tête global** : Device name + date/heure (JJ/MM HH:MM) sur toutes les pages
- 🎯 **Indicateur de page** : 3 points en bas (• • •) avec mise en évidence page active
- ⏱️ **Auto-retour menu** : 30s timeout vers cycle automatique
- 🎨 **Alignement parfait** : Icônes et textes centrés verticalement avec `TextAlign::CENTER_LEFT`
- 🛠️ **Optimisations** : Buffers statiques, pas d'allocation dynamique dans lambda

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
