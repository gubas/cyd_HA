# 🖨️ Modèles 3D : Boîtier CYD Home Assistant (Support IKEA SKADIS)

Boîtier modulaire optimisé pour l'écran tactile **ESP32-2432S028R** (Cheap Yellow Display - CYD 2.8"), conçu pour s'accrocher directement sur un panneau perforé **IKEA SKADIS**.

---

## 📑 Fichiers Disponibles

| Fichier | Format | Rôle / Description | Prêt à Imprimer |
| :--- | :---: | :--- | :---: |
| [`cyd_ha_front.scad`](cyd_ha_front.scad) | **SCAD** | Pièce 1 : Façade avant avec fenêtre écran et colonnettes de centrage | ✅ F6 → F7 |
| [`cyd_ha_front.stl`](cyd_ha_front.stl) | **STL** | Maillage STL étanche de la façade avant (face à plat sur Z=0) | ✅ 1 clic |
| [`cyd_ha_back_skadis.scad`](cyd_ha_back_skadis.scad) | **SCAD** | Pièce 2 : Dos avec crochets SKADIS intégrés (entraxe 40 mm) | ✅ F6 → F7 |
| [`cyd_ha_back_skadis.stl`](cyd_ha_back_skadis.stl) | **STL** | Maillage STL étanche du dos SKADIS (crochets vers le haut) | ✅ 1 clic |
| [`cyd_ha_case_skadis.scad`](cyd_ha_case_skadis.scad) | **SCAD** | Assemblage maître (vue assemblée, éclatée, maquette CYD) | Modèle d'étude |
| [`cyd_ha_full_plate.stl`](cyd_ha_full_plate.stl) | **STL** | Plateau complet réunissant les 2 pièces prêtes pour le slicer | ✅ Plateau prêt |
| [`cyd_ha_case_skadis.3mf`](cyd_ha_case_skadis.3mf) | **3MF** | Projet Bambu Lab natif (généré de 0, plateau 256x256, zéro support) | ✅ Prêt à trancher |
| [`generate_bambu_3mf.py`](generate_bambu_3mf.py) | **Python** | Générateur autonome de l'archive 3MF Bambu Lab à partir des STL | Script de build |
| [`build_models.ps1`](build_models.ps1) / [`.sh`](build_models.sh) | **Script** | Compilation automatique complète (tous les STL + package 3MF) | Automatisation totale |

---

## ⚙️ Paramètres d'Impression Recommandés (DfAM)

* **Imprimante** : Bambu Lab (P1S / X1C / A1), Prusa, Creality ou équivalent.
* **Matière** : **PETG** ou **ABS/ASA** fortement recommandé (les crochets SKADIS subissent une flexion continue qui provoquerait du fluage à froid sur du PLA classique).
* **Buse** : `0.4 mm`.
* **Hauteur de couche** : `0.20 mm` (Standard).
* **Parois (Wall Loops)** : `4` tours (solidité maximale des crochets et de l'emboîtement).
* **Remplissage** : `20% à 25%` en motif **Gyroid**.
* **Supports** : **Aucun support nécessaire (0 support)** :
  * Le dos SKADIS s'imprime à plat, les crochets pointant vers le haut.
  * La façade s'imprime face avant contre le plateau (idéal sur lit PEI texturé pour un rendu de façade uniforme).

---

## 🛠️ Personnalisations OpenSCAD (Customizer)

Ouvrez les fichiers dans OpenSCAD (*Fenêtre > Personnalisateur*) pour ajuster :
* **Bouton RESET** : Trou d'épingle sur la tranche (`enable_reset_pinhole = true`).
* **Dégagement USB** : Passage élargi pour les câbles surmoulés épais (`enable_extra_usb_clearance`).
* **Ventilation** : Fentes d'aération arrière pour capteur I2C interne (`enable_ventilation_slots`).
* **Texte débossé** : Gravure personnalisée au dos du boîtier (`custom_text = "CYD HA"`).
* **Moteur natif** : Basculez `source = "parametric"` pour modifier directement les dimensions de la boîte sans passer par les STL.

