# Architecture technique - ESP32 CYD Touch Panel

## Vue d'ensemble

Ce projet implémente un panneau de contrôle tactile pour Home Assistant en utilisant une architecture modulaire et maintenable.

## Structure des fichiers

```
cyd_HA/
│
├── cyd_ha_refactored.yaml   # 🎯 FICHIER PRINCIPAL (point d'entrée)
│   ├── Substitutions (variables globales)
│   ├── Configuration ESPHome core
│   ├── WiFi/API/OTA (avec références !secret)
│   ├── Globals (show_return_page)
│   ├── Time (sync Home Assistant)
│   └── Includes (modules ci-dessous)
│
├── secrets.yaml              # 🔐 CREDENTIALS (ne JAMAIS commiter)
│   ├── wifi_ssid
│   ├── wifi_password
│   ├── api_encryption_key
│   ├── ota_password
│   ├── ap_ssid
│   └── ap_password
│
├── cyd_ha/common.yaml        # 🎨 RESSOURCES UI
│   ├── Fonts (Verdana + Material Design Icons)
│   ├── Colors (black, blue, grey)
│   ├── Substitutions (variables globales)
│
├── cyd_ha/hardware.yaml      # ⚙️ CONFIGURATION MATERIELLE
│   ├── SPI (2 bus: tft + touch)
│   ├── Touchscreen (XPT2046 avec calibration)
│   ├── Outputs (PWM backlight + RGB LED)
│   └── Lights (backlight + led)
│
│   ├── wifi_ssid
│   ├── wifi_password
│   ├── cyd_ha_api_encryption_key
│   ├── cyd_ha_ota_password
│   ├── cyd_ha_ap_ssid
│   └── cyd_ha_ap_password
│   └── Actions on_press (toggle show_return_page ou service HA)
│
│   ├── Fonts (Verdana + Material Design Icons)
    ├── ILI9342 configuration (320x240, rotation 270)
    └── Lambda rendering:
        ├── Page menu (show_return_page = true)
        └── Page principale (show_return_page = false)
│   ├── SPI (2 bus: tft + touch)

## Flux de données

### 1. Démarrage ESP32

│   ├── Numeric sensors (temp, humidity, weather probabilities)
ESP32 Boot
    ↓
cyd_ha_refactored.yaml (parse)
│   ├── 8 binary_sensor (touchscreen platform)
Load secrets.yaml
    ↓
WiFi Connect (fast_connect, power_save_mode: none)
        ├── ILI9342 configuration (320x240, rotation 270)
Home Assistant API (encrypted connection)
    ↓
Time Sync (esptime ← Home Assistant)
    ↓
Load modules (!include)
    ↓
Initialize hardware (SPI, touch, display, outputs)
    ↓
Start display loop (1s update_interval)
```

### 2. Affichage UI (Display Loop)

```
display.yaml lambda (appelé chaque 1s)
    ↓
Check show_return_page global
    ├─ TRUE → Render Menu Page
    │   ├─ Draw 8 button rectangles
    │   ├─ Draw button labels
    │   └─ Draw icons (color based on entity states)
    │
    └─ FALSE → Render Main Page
        ├─ Display date/time (esptime)
        ├─ Display weather icon (map lookup avec fallback)
        └─ Rotating info display (millis timer, 5s interval)
            ├─ Index 0: Room 1 temp/humidity
            ├─ Index 1: Room 2 temp/humidity
            ├─ Index 2: Outdoor temp/rain chance
            └─ Index 3: Snow/freeze probability
```

### 3. Interaction tactile

```
User Touch
    ↓
XPT2046 detects coordinates
    ↓
touchscreen platform (50ms update, threshold 400)
    ↓
binary_sensor zone match (x_min/max, y_min/max)
    ↓
on_press action
    ├─ Check show_return_page
    │   ├─ FALSE → Toggle to TRUE (show menu)
    │   └─ TRUE → Call Home Assistant service
    │       └─ homeassistant.service:
    │           ├─ service: $buttonX_service
    │           └─ entity_id: $buttonX_entity
    └─ Button 8 (always toggle show_return_page)
```

### 4. Synchronisation capteurs

```
Home Assistant
    ↓
API (encrypted, reboot_timeout: 15min)
    ↓
sensors.yaml (homeassistant platform)
    ↓
ESP32 internal sensors (id: temp_int, hum_int, etc.)
    ↓
display.yaml lambda (read sensor.state)
    ↓
Render on screen (with has_state() checks)
```
     esphome logs cyd_ha_refactored.yaml --device cyd_ha.local
## Diagrammes

### Architecture logicielle (couches)

```
┌─────────────────────────────────────────────┐
│         User Interface Layer                │
└────────────────┬────────────────────────────┘
                 │
┌────────────────┴────────────────────────────┐
│         Control Layer                       │
│  (buttons.yaml - touch event handlers)      │
└────────────────┬────────────────────────────┘
                 │
┌────────────────┴────────────────────────────┐
│         Data Layer                          │
│  (sensors.yaml - HA entity integration)     │
└────────────────┬────────────────────────────┘
                 │
┌────────────────┴────────────────────────────┐
│         Hardware Abstraction Layer          │
│  (hardware.yaml - SPI, touch, outputs)      │
└────────────────┬────────────────────────────┘
                 │
┌────────────────┴────────────────────────────┐
│         Physical Hardware                   │
│  (ESP32, ILI9342, XPT2046, GPIO)           │
└─────────────────────────────────────────────┘
```

### État de l'application (State Machine)

```
        ┌─────────────┐
        │   BOOT      │
        └──────┬──────┘
               │
               ▼
        ┌─────────────┐
        │ CONNECTING  │◄───── WiFi reconnect
        └──────┬──────┘
               │
               ▼
        ┌─────────────┐
        │  CONNECTED  │
        └──────┬──────┘
               │
               ▼
┌──────────────────────────────┐
│     MAIN PAGE DISPLAY        │
│  (show_return_page = false)  │
│                              │
│  - Date/Time                 │
│  - Weather Icon              │
│  - Rotating Sensor Data      │
└──────┬───────────────────────┘
       │                    ▲
       │ Touch Buttons 1-7  │
       │ (toggle)           │ Touch Button 8
       ▼                    │ (return)
┌──────────────────────────┴───┐
│     MENU PAGE DISPLAY        │
│  (show_return_page = true)   │
│                              │
│  - 8 Control Buttons         │
│  - Entity State Icons        │
└──────┬───────────────────────┘
       │
       │ Touch Buttons 1-6
       │ (HA service call)
       ▼
    [Action in Home Assistant]
```

## Optimisations appliquées

### Performance

1. **WiFi rapide**
   ```yaml
   fast_connect: true          # Connexion rapide au dernier AP connu
   power_save_mode: none       # Désactive économie d'énergie (réactivité touch)
   ```

2. **Display update optimal**
   ```yaml
   update_interval: 1s         # Balance entre fluidité et CPU
   ```

3. **Touch responsive**
   ```yaml
   update_interval: 50ms       # Polling rapide (20 Hz)
   threshold: 400              # Seuil de détection tactile
   ```

### Robustesse

1. **Timer display précis**
   ```cpp
   static uint32_t last_change_time = 0;
   uint32_t current_time = millis();
   if (current_time - last_change_time >= TEXT_INTERVAL_MS) {
       // Change display
   }
   ```
   
   Au lieu de :
   ```cpp
   text_timer += 1.0;  // ❌ Dépend de la fréquence d'appel
   ```

2. **Fallback météo**
   ```cpp
   auto icon_it = weather_icon_map.find(weather_str);
   if (icon_it != weather_icon_map.end()) {
       // Use icon
   } else {
       // Fallback to sunny icon
   }
   ```

3. **Vérifications sensor state**
   ```cpp
   if (id(temp_int).has_state()) {
       it.printf("%.1f C", id(temp_int).state);
   } else {
       it.print("-- C");
   }
   ```

### Sécurité

1. **Credentials isolés**
   - Tous les secrets dans `secrets.yaml`
   - Références `!secret` dans config principale
   - `.gitignore` protège `secrets.yaml`

2. **Connexions chiffrées**
   ```yaml
   api:
     encryption:
       key: !secret api_encryption_key
   ota:
     password: !secret ota_password
   ```

3. **Fallback AP sécurisé**
   ```yaml
   ap:
     ssid: !secret ap_ssid
     password: !secret ap_password  # Au lieu de hardcodé
   ```

## Extensibilité

### Ajouter un bouton

1. Ajouter substitutions dans `cyd_ha_refactored.yaml`
2. Ajouter zone tactile dans `buttons.yaml`
3. Ajouter text_sensor dans `sensors.yaml` (si besoin état)
4. Modifier lambda display dans `display.yaml` (icône)

### Ajouter un capteur

1. Ajouter substitution dans `cyd_ha_refactored.yaml`
2. Ajouter sensor dans `sensors.yaml`
3. Modifier lambda display dans `display.yaml` (nouvel index rotation)

### Changer le thème

1. Modifier `common.yaml` (couleurs)
2. Remplacer icônes dans `common.yaml`
3. Ajuster fonts si nécessaire

## Technologies utilisées

| Composant | Technologie | Rôle |
|-----------|-------------|------|
| MCU | ESP32 (240MHz dual-core) | Processeur principal |
| Display | ILI9342 (320x240 TFT) | Affichage graphique |
| Touch | XPT2046 (resistive) | Contrôleur tactile |
| Framework | ESPHome (Python/C++) | Firmware/Config |
| Protocol | Home Assistant API | Communication |
| UI Fonts | Verdana + MDI | Rendu texte/icônes |

## Limites connues

1. **WiFi 2.4GHz uniquement** (ESP32 hardware)
2. **Tactile résistif** (moins précis que capacitif)
3. **Pas de multi-touch** (XPT2046 limitation)
4. **Mémoire limitée** (320KB RAM) → éviter trop de fonts/images
5. **Rotation display fixe** (270°) → modifier dans display.yaml si besoin

## Maintenance

### Vérifications régulières

- [ ] Vérifier logs ESPHome (`esphome logs`)
- [ ] Tester OTA updates (tous les 3 mois)
- [ ] Recalibrer touch si dérive
- [ ] Nettoyer écran tactile (résidu → mauvaise détection)
- [ ] Backup `secrets.yaml` (stockage sécurisé)

### Debugging

1. **Activer logs verbeux** (temporaire)
   ```yaml
   logger:
     level: DEBUG
   ```

2. **Vérifier API connection**
   ```powershell
   esphome logs cyd_ha_refactored.yaml --device entree.local
   ```

3. **Test calibration touch**
   - Activer logs touch events
   - Taper coins écran
   - Ajuster x_min/max, y_min/max

---

**Document vivant** : Cette architecture évolue avec le projet.
Dernière mise à jour : Octobre 2025
