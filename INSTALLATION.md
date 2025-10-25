# Guide d'installation ESPHome - Windows

## Prérequis

- Windows 10/11
- Python 3.9 ou supérieur
- PowerShell (inclus dans Windows)

## Installation ESPHome

### Option 1 : Installation via pip (recommandé)

1. **Installer Python** (si pas déjà installé)
   - Télécharger depuis https://www.python.org/downloads/
   - ✅ Cocher "Add Python to PATH" lors de l'installation

2. **Ouvrir PowerShell** et exécuter :

```powershell
# Mettre à jour pip
python -m pip install --upgrade pip

# Installer ESPHome
pip install esphome

# Vérifier l'installation
esphome version
```

### Option 2 : Installation via Home Assistant Add-on

Si vous utilisez Home Assistant OS :

1. Naviguer vers **Supervisor → Add-on Store**
2. Installer **ESPHome**
3. Démarrer l'add-on
4. Ouvrir l'interface web

### Option 3 : Installation via Docker

```powershell
# Pull l'image Docker
docker pull esphome/esphome

# Lancer ESPHome Dashboard
docker run --rm -v "${PWD}:/config" -p 6052:6052 esphome/esphome
```

## Vérification de l'installation

```powershell
# Version ESPHome
esphome version

# Aide
esphome --help
```

## Installation des drivers USB (pour flash via USB)

### Pour ESP32 avec CH340/CH341 (CYD utilise souvent ce chip)

1. Télécharger : http://www.wch-ic.com/downloads/CH341SER_ZIP.html
2. Extraire et installer `CH341SER.EXE`
3. Redémarrer Windows

### Pour ESP32 avec CP2102

1. Télécharger : https://www.silabs.com/developers/usb-to-uart-bridge-vcp-drivers
2. Installer le driver
3. Redémarrer Windows

### Vérifier le port COM

```powershell
# Lister les ports COM disponibles
[System.IO.Ports.SerialPort]::getportnames()
```

## Premier test

```powershell
# Naviguer vers le dossier du projet
cd "c:\Users\guillaume.FLACE\Documents\dev\esphome\cyd_HA"

# Valider la configuration (après avoir créé secrets.yaml)
esphome config cyd_ha_refactored.yaml

# Compiler (sans flasher)
esphome compile cyd_ha_refactored.yaml
```

## Dépannage

### Erreur "esphome n'est pas reconnu"

**Cause** : Python ou ESPHome n'est pas dans le PATH

**Solution** :
```powershell
# Trouver l'emplacement de Python
where.exe python

# Ajouter manuellement au PATH (temporaire)
$env:Path += ";C:\Users\VOTRE_NOM\AppData\Local\Programs\Python\Python311\Scripts"

# Réessayer
esphome version
```

### Erreur "No module named 'esphome'"

**Solution** :
```powershell
python -m pip install --upgrade esphome
```

### Erreur lors du flash USB "Could not open port"

**Causes possibles** :
1. Driver USB manquant → installer CH340 ou CP2102 driver
2. Port COM utilisé par autre programme → fermer Arduino IDE, PlatformIO, etc.
3. Cable USB défectueux → essayer un autre cable (data + power, pas juste power)

**Solution** :
```powershell
# Vérifier les ports disponibles
[System.IO.Ports.SerialPort]::getportnames()

# Spécifier manuellement le port
esphome run cyd_ha_refactored.yaml --device COM3
```

### Erreur "Timeout waiting for the device"

**Solution** :
- Maintenir le bouton BOOT pendant le flash
- Ou réinitialiser l'ESP32 pendant la connexion

## Utilisation du script deploy.ps1

Une fois ESPHome installé :

```powershell
# Lancer le menu interactif
.\deploy.ps1

# Ou utiliser directement une action
.\deploy.ps1 -Action validate
.\deploy.ps1 -Action compile
.\deploy.ps1 -Action flash-usb
.\deploy.ps1 -Action flash-ota
```

## Ressources

- Documentation ESPHome : https://esphome.io/
- Forum ESPHome : https://community.home-assistant.io/c/esphome/
- Discord ESPHome : https://discord.gg/KhAMKrd

---

**Note** : Si vous rencontrez des problèmes, consultez les logs détaillés avec :
```powershell
esphome --verbose logs cyd_ha_refactored.yaml
```
