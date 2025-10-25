# =============================================================================
# ESPHome Deployment Script - ESP32 CYD Home Assistant Touch Panel
# =============================================================================
# 
# This script provides a menu-driven interface for common ESPHome operations
# Author: Refactored Setup
# Date: October 2025
# =============================================================================

param(
    [string]$Action = "menu"
)

$ErrorActionPreference = "Stop"
$ConfigFile = "cyd_ha_refactored.yaml"
$DeviceName = "cyd_ha"

# Colors
function Write-Success { Write-Host $args -ForegroundColor Green }
function Write-Error { Write-Host $args -ForegroundColor Red }
function Write-Info { Write-Host $args -ForegroundColor Cyan }
function Write-Warning { Write-Host $args -ForegroundColor Yellow }

# Banner
function Show-Banner {
    Write-Host @"

╔═══════════════════════════════════════════════════════════════╗
║                                                               ║
║   ESP32 CYD - Home Assistant Touch Panel                      ║
║   ESPHome Deployment Manager                                  ║
║                                                               ║
╚═══════════════════════════════════════════════════════════════╝

"@ -ForegroundColor Cyan
}

# Menu
function Show-Menu {
    Write-Host "`n=== Actions disponibles ===" -ForegroundColor Yellow
    Write-Host "1. Valider la configuration YAML"
    Write-Host "2. Compiler le firmware"
    Write-Host "3. Flash via USB (première installation)"
    Write-Host "4. Flash via OTA (mise à jour sans fil)"
    Write-Host "5. Voir les logs en temps réel"
    Write-Host "6. Nettoyer les builds"
    Write-Host "7. Vérifier secrets.yaml"
    Write-Host "8. Télécharger font Material Design Icons"
    Write-Host "9. Ouvrir la configuration dans l'éditeur"
    Write-Host "0. Quitter"
    Write-Host ""
}

# Check ESPHome installed
function Test-ESPHome {
    try {
        $version = esphome version 2>$null
        Write-Success "✓ ESPHome détecté: $version"
        return $true
    } catch {
        Write-Error "✗ ESPHome n'est pas installé!"
        Write-Info "Installation: pip install esphome"
        return $false
    }
}

# Check config file exists
function Test-ConfigFile {
    if (Test-Path $ConfigFile) {
        Write-Success "✓ Fichier de configuration trouvé: $ConfigFile"
        return $true
    } else {
        Write-Error "✗ Fichier $ConfigFile introuvable!"
        return $false
    }
}

# Validate YAML
function Invoke-Validate {
    Write-Info "`n[1] Validation de la configuration..."
    esphome config $ConfigFile
    if ($LASTEXITCODE -eq 0) {
        Write-Success "`n✓ Configuration valide!"
    } else {
        Write-Error "`n✗ Erreurs de configuration détectées."
    }
}

# Compile
function Invoke-Compile {
    Write-Info "`n[2] Compilation du firmware..."
    esphome compile $ConfigFile
    if ($LASTEXITCODE -eq 0) {
        Write-Success "`n✓ Compilation réussie!"
        Write-Info "Firmware: .esphome/build/$DeviceName/.pioenvs/$DeviceName/firmware.bin"
    } else {
        Write-Error "`n✗ Échec de la compilation."
    }
}

# Flash USB
function Invoke-FlashUSB {
    Write-Info "`n[3] Flash via USB..."
    Write-Warning "Connectez l'ESP32 via USB et appuyez sur ENTER"
    Read-Host
    esphome run $ConfigFile
    if ($LASTEXITCODE -eq 0) {
        Write-Success "`n✓ Flash USB réussi!"
    } else {
        Write-Error "`n✗ Échec du flash USB."
    }
}

# Flash OTA
function Invoke-FlashOTA {
    Write-Info "`n[4] Flash via OTA..."
    $device = "${DeviceName}.local"
    Write-Info "Tentative de connexion à $device..."
    esphome run $ConfigFile --device $device
    if ($LASTEXITCODE -eq 0) {
        Write-Success "`n✓ Flash OTA réussi!"
    } else {
        Write-Error "`n✗ Échec du flash OTA. Vérifiez la connexion réseau."
    }
}

# View logs
function Invoke-Logs {
    Write-Info "`n[5] Logs en temps réel..."
    Write-Info "Appuyez sur Ctrl+C pour quitter"
    Start-Sleep -Seconds 2
    esphome logs $ConfigFile --device "${DeviceName}.local"
}

# Clean builds
function Invoke-Clean {
    Write-Info "`n[6] Nettoyage des builds..."
    if (Test-Path ".esphome") {
        Remove-Item -Recurse -Force ".esphome"
        Write-Success "✓ Cache ESPHome nettoyé"
    }
    if (Test-Path ".pioenvs") {
        Remove-Item -Recurse -Force ".pioenvs"
        Write-Success "✓ Builds PlatformIO nettoyés"
    }
    Write-Success "✓ Nettoyage terminé"
}

# Check secrets
function Invoke-CheckSecrets {
    Write-Info "`n[7] Vérification de secrets.yaml..."
    if (-not (Test-Path "secrets.yaml")) {
        Write-Error "✗ secrets.yaml introuvable!"
        Write-Warning "Créez-le avec (secrets préfixés par nom du device) :"
        Write-Host @"
# WiFi global
wifi_ssid: "VOTRE_SSID"
wifi_password: "VOTRE_PASSWORD"

# Secrets pour ce device "cyd_ha"
cyd_ha_api_encryption_key: "GENERER_AVEC_ESPHOME"
cyd_ha_ota_password: "VOTRE_PASSWORD_OTA"
cyd_ha_ap_ssid: "CYD HA Fallback Hotspot"
cyd_ha_ap_password: "PASSWORD_AP"
"@
        return
    }
    
    $secrets = Get-Content "secrets.yaml" -Raw
    $required = @("wifi_ssid", "wifi_password", "cyd_ha_api_encryption_key", "cyd_ha_ota_password", "cyd_ha_ap_ssid", "cyd_ha_ap_password")
    
    foreach ($key in $required) {
        if ($secrets -match $key) {
            Write-Success "✓ $key présent"
        } else {
            Write-Error "✗ $key manquant!"
        }
    }
    
    Write-Info "`nNote: Les secrets sont préfixés par 'cyd_ha_' pour permettre un secrets.yaml partagé."
}

# Download font
function Invoke-DownloadFont {
    Write-Info "`n[8] Téléchargement de Material Design Icons..."
    $url = "https://github.com/Templarian/MaterialDesign-Webfont/raw/master/fonts/materialdesignicons-webfont.ttf"
    $output = "materialdesignicons-webfont.ttf"
    
    if (Test-Path $output) {
        Write-Warning "Font déjà présente. Remplacer? (O/N)"
        $response = Read-Host
        if ($response -ne "O" -and $response -ne "o") {
            Write-Info "Téléchargement annulé"
            return
        }
    }
    
    try {
        Invoke-WebRequest -Uri $url -OutFile $output
        Write-Success "✓ Font téléchargée: $output"
    } catch {
        Write-Error "✗ Échec du téléchargement: $_"
    }
}

# Open editor
function Invoke-OpenEditor {
    Write-Info "`n[9] Ouverture dans l'éditeur..."
    if (Get-Command code -ErrorAction SilentlyContinue) {
        code $ConfigFile
        Write-Success "✓ Ouvert dans VS Code"
    } elseif (Get-Command notepad++ -ErrorAction SilentlyContinue) {
        notepad++ $ConfigFile
        Write-Success "✓ Ouvert dans Notepad++"
    } else {
        notepad $ConfigFile
        Write-Success "✓ Ouvert dans Notepad"
    }
}

# Main
function Main {
    Show-Banner
    
    if (-not (Test-ESPHome)) {
        exit 1
    }
    
    if (-not (Test-ConfigFile)) {
        exit 1
    }
    
    if ($Action -ne "menu") {
        switch ($Action) {
            "validate" { Invoke-Validate }
            "compile" { Invoke-Compile }
            "flash-usb" { Invoke-FlashUSB }
            "flash-ota" { Invoke-FlashOTA }
            "logs" { Invoke-Logs }
            "clean" { Invoke-Clean }
            "secrets" { Invoke-CheckSecrets }
            "font" { Invoke-DownloadFont }
            default { Write-Error "Action inconnue: $Action" }
        }
        return
    }
    
    # Interactive menu
    while ($true) {
        Show-Menu
        $choice = Read-Host "Choisissez une action"
        
        switch ($choice) {
            "1" { Invoke-Validate }
            "2" { Invoke-Compile }
            "3" { Invoke-FlashUSB }
            "4" { Invoke-FlashOTA }
            "5" { Invoke-Logs }
            "6" { Invoke-Clean }
            "7" { Invoke-CheckSecrets }
            "8" { Invoke-DownloadFont }
            "9" { Invoke-OpenEditor }
            "0" { 
                Write-Success "`nAu revoir!"
                exit 0
            }
            default { Write-Warning "Choix invalide!" }
        }
        
        Write-Host "`nAppuyez sur ENTER pour continuer..."
        Read-Host
    }
}

# Run
Main
