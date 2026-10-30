#!/usr/bin/env bash
set -e

DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
OPENSCAD="openscad"
if command -v openscad.com &> /dev/null; then
    OPENSCAD="openscad.com"
elif [ -f "/c/Program Files/OpenSCAD/openscad.com" ]; then
    OPENSCAD="/c/Program Files/OpenSCAD/openscad.com"
fi

echo "=========================================================="
echo "   EXPORT DES FICHIERS STL DU CYD_HA (ESP32-2432S028R)"
echo "=========================================================="

echo "1/3 Export STL : Façade avant (cyd_ha_front.stl)..."
"$OPENSCAD" --export-format binstl -o "$DIR/cyd_ha_front.stl" "$DIR/cyd_ha_front.scad"

echo "2/3 Export STL : Dos SKADIS (cyd_ha_back_skadis.stl)..."
"$OPENSCAD" --export-format binstl -o "$DIR/cyd_ha_back_skadis.stl" "$DIR/cyd_ha_back_skadis.scad"

echo "3/4 Export STL : Plateau complet (cyd_ha_full_plate.stl)..."
"$OPENSCAD" --export-format binstl -o "$DIR/cyd_ha_full_plate.stl" -D 'mode="print_bed"' "$DIR/cyd_ha_case_skadis.scad"

echo "4/4 Génération Projet Bambu 3MF natif (cyd_ha_case_skadis.3mf)..."
python3 "$DIR/generate_bambu_3mf.py" 2>/dev/null || python "$DIR/generate_bambu_3mf.py"

echo "Tous les modèles (STL + 3MF) ont été générés avec succès !"

