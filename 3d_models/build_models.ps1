$openscad = "C:\Program Files\OpenSCAD\openscad.com"
if (-not (Test-Path $openscad)) {
    if (Get-Command openscad.com -ErrorAction SilentlyContinue) {
        $openscad = "openscad.com"
    } elseif (Get-Command openscad -ErrorAction SilentlyContinue) {
        $openscad = "openscad"
    } elseif (Test-Path "C:\Program Files (x86)\OpenSCAD\openscad.com") {
        $openscad = "C:\Program Files (x86)\OpenSCAD\openscad.com"
    }
}

Write-Host "=========================================================="
Write-Host "   EXPORT DES FICHIERS STL DU CYD_HA (ESP32-2432S028R)"
Write-Host "=========================================================="

Write-Host "1/3 Export STL : Façade avant (cyd_ha_front.stl)..."
& $openscad --export-format binstl -o "3d_models\cyd_ha_front.stl" "3d_models\cyd_ha_front.scad"

Write-Host "2/3 Export STL : Dos SKADIS (cyd_ha_back_skadis.stl)..."
& $openscad --export-format binstl -o "3d_models\cyd_ha_back_skadis.stl" "3d_models\cyd_ha_back_skadis.scad"

Write-Host "3/4 Export STL : Plateau complet (cyd_ha_full_plate.stl)..."
& $openscad --export-format binstl -o "3d_models\cyd_ha_full_plate.stl" -D 'mode="""print_bed"""' "3d_models\cyd_ha_case_skadis.scad"

Write-Host "4/4 Génération Projet Bambu 3MF natif (cyd_ha_case_skadis.3mf)..."
python "3d_models\generate_bambu_3mf.py"

Write-Host "Tous les modèles (STL + 3MF) ont été générés avec succès !"
