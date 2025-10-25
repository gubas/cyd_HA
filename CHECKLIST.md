# Checklist de déploiement - ESP32 CYD Touch Panel

Suivez cette checklist pour déployer votre panneau tactile avec succès.

## ☑️ Phase 1 : Préparation (avant de flasher)

### 1.1 Installation logicielle
- [ ] Python 3.9+ installé
- [ ] ESPHome installé (`pip install esphome`)
- [ ] Driver USB installé (CH340 ou CP2102)
- [ ] Port COM fonctionnel (tester avec `[System.IO.Ports.SerialPort]::getportnames()`)

### 1.2 Fichiers du projet
- [ ] Tous les fichiers présents dans le dossier :
  - `cyd_ha_refactored.yaml` (configuration principale)
  - `secrets.yaml` (à créer)
  - `cyd_ha/common.yaml` (ressources UI)
  - `cyd_ha/hardware.yaml` (config matérielle)
  - `cyd_ha/sensors.yaml` (capteurs HA)
  - `cyd_ha/buttons.yaml` (zones tactiles)
  - `cyd_ha/display.yaml` (logique UI)
  - `materialdesignicons-webfont.ttf` (à télécharger)

### 1.3 Configuration secrets.yaml
- [ ] Créer ou éditer `secrets.yaml` (peut être partagé avec tous vos projets ESPHome)
- [ ] Renseigner `wifi_ssid` (réseau 2.4GHz obligatoire)
- [ ] Renseigner `wifi_password`
- [ ] Vérifier/générer `cyd_ha_api_encryption_key` (préfixé par le nom du device)
- [ ] Définir `cyd_ha_ota_password` (mot de passe fort, préfixé)
- [ ] Définir `cyd_ha_ap_ssid` et `cyd_ha_ap_password` (fallback, préfixés)

**Exemple secrets.yaml partagé** :
```yaml
# WiFi global (tous les devices)
wifi_ssid: "MonWiFi24GHz"
wifi_password: "MotDePasseSecurise123"

# Secrets pour le device "cyd_ha" (ce projet)
cyd_ha_api_encryption_key: ""  # Sera généré automatiquement
cyd_ha_ota_password: "MonOTAPassword2024!"
cyd_ha_ap_ssid: "CYD HA Fallback Hotspot"
cyd_ha_ap_password: "FallbackPass123"

# Pour d'autres projets ESPHome :
# salon_api_encryption_key: "..."
# salon_ota_password: "..."
# cuisine_api_encryption_key: "..."
```

**Note** : Les secrets sont préfixés (`cyd_ha_*`) pour permettre un seul `secrets.yaml` pour tous vos projets.

### 1.4 Personnalisation entités Home Assistant
- [ ] Éditer `cyd_ha_refactored.yaml` section `substitutions`
- [ ] Vérifier que tous les `entity_id` existent dans Home Assistant :
  - `internal_temp_sensor`
  - `internal_humidity_sensor`
  - `int2_temp_sensor`
  - `int2_humidity_sensor`
  - `outside_temp_sensor`
  - `weather_entity`
  - `rain_chance`, `snow_chance`, `freeze_chance`
  - `button1_entity` à `button6_entity`

### 1.5 Télécharger la font Material Design Icons
```powershell
Invoke-WebRequest -Uri "https://github.com/Templarian/MaterialDesign-Webfont/raw/master/fonts/materialdesignicons-webfont.ttf" -OutFile "materialdesignicons-webfont.ttf"
```
- [ ] Font `materialdesignicons-webfont.ttf` présente dans le dossier

## ☑️ Phase 2 : Validation (avant compilation)

### 2.1 Valider la configuration
```powershell
esphome config cyd_ha_refactored.yaml
```
- [ ] Aucune erreur YAML
- [ ] API key générée automatiquement (copiée dans secrets.yaml)
- [ ] Tous les `!include` résolus correctement
- [ ] Substitutions valides

### 2.2 Vérifier les logs de validation
- [ ] Pas d'avertissement critique
- [ ] Tous les sensors/entities reconnus
- [ ] Fonts chargées correctement
- [ ] Images/icônes trouvées

## ☑️ Phase 3 : Compilation

### 3.1 Compiler le firmware
```powershell
esphome compile cyd_ha_refactored.yaml
```
- [ ] Compilation réussie (0 erreurs)
- [ ] Firmware généré : `.esphome/build/cyd_ha/.pioenvs/cyd_ha/firmware.bin`
- [ ] Taille firmware < 1.4MB (limite ESP32)

### 3.2 Vérifier les warnings
- [ ] Pas de warning mémoire critique
- [ ] Pas de conflit GPIO
- [ ] Pas d'image trop volumineuse

## ☑️ Phase 4 : Flash initial (USB)

### 4.1 Préparer le matériel
- [ ] ESP32 CYD connecté via USB (cable DATA+POWER, pas juste POWER)
- [ ] Port COM détecté (ex: COM3)
- [ ] Driver USB fonctionnel

### 4.2 Premier flash
```powershell
esphome run cyd_ha_refactored.yaml
```
- [ ] Flash réussi (100%)
- [ ] ESP32 redémarre automatiquement
- [ ] Écran s'allume (rétroéclairage actif)

### 4.3 Vérifier les logs
```powershell
esphome logs cyd_ha_refactored.yaml
```
- [ ] WiFi connecté
- [ ] API Home Assistant connectée
- [ ] Time synchronisé
- [ ] Sensors initialisés
- [ ] Display actif

## ☑️ Phase 5 : Test fonctionnel

### 5.1 Affichage
- [ ] Date/heure affichée correctement
- [ ] Icône météo affichée
- [ ] Rotation capteurs (changement toutes les 5s)
- [ ] Températures affichées (ou "--" si non disponible)

### 5.2 Interface tactile
- [ ] Touch détecté (taper écran → logs)
- [ ] Bouton menu (n'importe quel bouton sur écran principal)
- [ ] Menu s'affiche (8 boutons)
- [ ] Icônes colorées selon états entités
- [ ] Bouton retour fonctionne

### 5.3 Contrôles Home Assistant
- [ ] Bouton 1 déclenche service ($button1_service)
- [ ] Bouton 2 déclenche service ($button2_service)
- [ ] ... (tester chaque bouton)
- [ ] État des entités mis à jour dans HA
- [ ] Feedback visuel (couleur icône change)

### 5.4 Calibration tactile (si nécessaire)
Si les zones ne répondent pas correctement :
1. Activer logs touch :
   ```yaml
   logger:
     level: DEBUG
     logs:
       touchscreen: DEBUG
   ```
2. Taper coins écran et noter coordonnées
3. Ajuster dans `cyd_ha/hardware.yaml` :
   ```yaml
   touchscreen:
     calibration:
       x_min: VALEUR_MESURÉE
       x_max: VALEUR_MESURÉE
       y_min: VALEUR_MESURÉE
       y_max: VALEUR_MESURÉE
   ```
4. Reflasher

## ☑️ Phase 6 : Flash OTA (mises à jour sans fil)

### 6.1 Premier test OTA
```powershell
esphome run cyd_ha_refactored.yaml --device cyd_ha.local
```
- [ ] Connexion OTA réussie
- [ ] Upload firmware via WiFi
- [ ] Redémarrage automatique
- [ ] Fonctionnement normal après update

### 6.2 Configurer mDNS (si échec)
Si `entree.local` ne fonctionne pas :
- [ ] Utiliser l'IP directe : `--device 192.168.X.X`
- [ ] Ou installer Bonjour Print Services (Windows)

## ☑️ Phase 7 : Intégration Home Assistant

### 7.1 Découverte automatique
- [ ] Home Assistant détecte "CYD HA" (Notifications)
- [ ] Ajouter l'intégration ESPHome
- [ ] Configurer l'encryption key

### 7.2 Entités créées
Vérifier dans Home Assistant → Paramètres → Appareils :
- [ ] Device "Entree" présent
- [ ] Entity "Display Backlight" (light)
- [ ] Entity "LED" (light)
- [ ] Sensors disponibles (si exposés)

### 7.3 Automatisations (optionnel)
Créer automatisations HA basées sur touch events :
- [ ] Logs touch events dans HA
- [ ] Automatisation testée

## ☑️ Phase 8 : Optimisation et finitions

### 8.1 Performance
- [ ] Affichage fluide (pas de lag)
- [ ] Touch réactif (<200ms)
- [ ] WiFi stable (pas de reconnexion)
- [ ] Mémoire RAM suffisante (logs: free heap > 50KB)

### 8.2 Sécurité
- [ ] `secrets.yaml` non commité (vérifier `.gitignore`)
- [ ] Mot de passe OTA fort
- [ ] Réseau WiFi sécurisé (WPA2/WPA3)
- [ ] API encryption activée

### 8.3 Sauvegarde
- [ ] Backup `secrets.yaml` (stockage sécurisé hors repo)
- [ ] Backup firmware compilé (`.esphome/build/`)
- [ ] Backup configuration complète (tous les .yaml)

## ☑️ Phase 9 : Documentation et maintenance

### 9.1 Documentation projet
- [ ] Lire `README.md`
- [ ] Lire `ARCHITECTURE.md`
- [ ] Lire `INSTALLATION.md`
- [ ] Personnaliser documentation si modifications

### 9.2 Plan de maintenance
- [ ] Calendrier updates ESPHome (tous les 3 mois)
- [ ] Test OTA régulier
- [ ] Nettoyage écran tactile (tous les mois)
- [ ] Vérification logs (hebdomadaire)

## 🎉 Déploiement terminé !

Une fois toutes les cases cochées, votre panneau tactile ESP32 CYD est opérationnel !

## 📞 Support

En cas de problème :

1. **Vérifier les logs** : `esphome logs cyd_ha_refactored.yaml`
2. **Consulter ARCHITECTURE.md** : section Dépannage
3. **Communauté ESPHome** : https://community.home-assistant.io/c/esphome/
4. **Discord ESPHome** : https://discord.gg/KhAMKrd

## 📊 Métriques de succès

| Métrique | Cible | Votre valeur |
|----------|-------|--------------|
| Temps boot | < 10s | ______ |
| Réactivité touch | < 200ms | ______ |
| Uptime WiFi | > 99% | ______ |
| Mémoire libre | > 50KB | ______ |
| Update display | 1s | ______ |

---

**Version checklist** : 2.0 (Octobre 2025)
**Temps estimé déploiement** : 30-60 minutes (première fois)
