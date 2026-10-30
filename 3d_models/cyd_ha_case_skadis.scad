// ==============================================================================
// 🛠️ BOÎTIER CYD HOME ASSISTANT (ESP32-2432S028R) - ASSEMBLAGE MAÎTRE SKADIS
// ==============================================================================
// Ce fichier réunit les pièces séparées pour la visualisation et l'assemblage :
// - Façade avant : cyd_ha_front.scad
// - Dos SKADIS   : cyd_ha_back_skadis.scad
//
// Chaque pièce possède son propre fichier indépendant (.scad) pour l'export STL.
// ==============================================================================

use <cyd_ha_front.scad>
use <cyd_ha_back_skadis.scad>

/* [Mode d'Affichage] */
// Choix de la vue dans l'espace 3D
mode = "assembled"; // [assembled:Boîtier Assemblé, exploded:Vue Éclatée, front_only:Façade Avant Seule, back_only:Dos SKADIS Seul, print_bed:Plateau d'Impression (À Plat)]

// Source de la géométrie 3D
source = "mesh"; // [mesh:Modèle 3MF/STL Original (Haute Fidélité), parametric:Modèle 100% Paramétrique Natif]

// Distance d'écartement pour la vue éclatée (mm)
explosion_distance = 35; // [10:5:100]

// Afficher la maquette 3D réaliste du CYD (ESP32-2432S028R)
show_cyd_board = true;

// Orientation d'impression de la façade (mode print_bed)
print_front_orientation = "face_down"; // [face_down:Face avant contre le plateau (Texture PEI), face_up:Face avant vers le haut (Sans ponts)]

/* [Personnalisations & Découpes CSG] */
// Percer un trou d'accès pour le bouton RESET
enable_reset_pinhole = true;
// Diamètre du trou de reset (mm)
reset_pinhole_dia = 2.0;

// Élargir l'ouverture inférieure pour connecteurs USB-C épais
enable_extra_usb_clearance = false;
// Dégagement supplémentaire en largeur (mm)
extra_usb_clearance_w = 4.0;

// Ajouter des fentes de ventilation arrière
enable_ventilation_slots = false;
// Nombre de fentes d'aération
vent_slot_count = 5;

// Graver un texte personnalisé sur la face arrière (débossé)
enable_custom_text = true;
// Texte à graver
custom_text = "CYD HA";
// Taille de police (mm)
custom_text_size = 5.0;
// Profondeur de gravure (mm)
deboss_depth = 0.6;

/* [Résolution Arcs] */
$fn = 40;

// Couleurs de prévisualisation
color_front = [0.95, 0.77, 0.20, 1.0]; // Jaune / Or Silk
color_back  = [0.20, 0.55, 0.90, 1.0]; // Bleu Skadis
color_pcb   = [0.85, 0.65, 0.15, 0.9]; // Jaune CYD
color_tft   = [0.10, 0.10, 0.12, 1.0]; // Écran TFT noir
color_usb   = [0.75, 0.75, 0.78, 1.0]; // Métal connecteurs


// ==============================================================================
// MAQUETTE CYD ESP32-2432S028R
// ==============================================================================

module cyd_board_mockup() {
    pcb_w = 50.0;
    pcb_h = 86.0;
    pcb_t = 1.6;
    
    translate([0, -1.8, 0]) {
        color(color_pcb) {
            difference() {
                linear_extrude(pcb_t)
                    rounded_box([pcb_w, pcb_h], 2.0);
                for (sx = [-1, 1]) {
                    for (sy = [-1, 1]) {
                        translate([sx * 45 / 2, sy * 80 / 2, -1])
                            cylinder(r = 1.6, h = pcb_t + 2);
                    }
                }
            }
        }
        
        // Écran LCD TFT 2.8"
        color(color_tft)
            translate([0, 0.7 - (-1.8), pcb_t + 1.2])
                cube([46.0, 65.0, 2.4], center = true);
                
        // Port USB-C
        color(color_usb)
            translate([0, -pcb_h / 2 + 3.5, pcb_t + 1.6])
                cube([9.0, 7.5, 3.2], center = true);
                
        // Port Micro-USB secondaire
        color(color_usb)
            translate([12.5, -pcb_h / 2 + 3.0, pcb_t + 1.4])
                cube([7.8, 6.0, 2.8], center = true);
                
        // Puce ESP32 blindée
        color([0.8, 0.8, 0.82])
            translate([0, 24, -1.8])
                cube([18.0, 25.5, 3.2], center = true);
                
        // Lecteur carte SD
        color(color_usb)
            translate([-13, 0, -1.0])
                cube([14.0, 15.0, 1.8], center = true);
    }
}

module rounded_box(size, r) {
    w = size[0];
    h = size[1];
    r_c = min(r, w / 2, h / 2);
    hull() {
        for (sx = [-1, 1]) {
            for (sy = [-1, 1]) {
                translate([sx * (w / 2 - r_c), sy * (h / 2 - r_c)])
                    circle(r = r_c);
            }
        }
    }
}


// ==============================================================================
// POSITIONNEMENT ET ASSEMBLAGE DES DEUX COMPOSANTS
// ==============================================================================

module render_front_part() {
    color(color_front) {
        if (source == "mesh") {
            rotate([0, 180, 0])
                cyd_ha_front(
                    src = "mesh",
                    pinhole = enable_reset_pinhole,
                    pinhole_d = reset_pinhole_dia,
                    extra_usb = enable_extra_usb_clearance,
                    usb_extra_w = extra_usb_clearance_w
                );
        } else {
            cyd_ha_front(
                src = "parametric",
                pinhole = enable_reset_pinhole,
                pinhole_d = reset_pinhole_dia,
                extra_usb = enable_extra_usb_clearance,
                usb_extra_w = extra_usb_clearance_w
            );
        }
    }
}

module render_back_part() {
    color(color_back) {
        if (source == "mesh") {
            translate([0, 0, -8.5])
                rotate([0, 180, 0])
                    cyd_ha_back_skadis(
                        src = "mesh",
                        vent = enable_ventilation_slots,
                        vents_n = vent_slot_count,
                        txt_on = enable_custom_text,
                        txt_str = custom_text,
                        txt_sz = custom_text_size,
                        txt_d = deboss_depth
                    );
        } else {
            translate([0, 0, -13.0 / 2])
                rotate([0, 180, 0])
                    cyd_ha_back_skadis(
                        src = "parametric",
                        vent = enable_ventilation_slots,
                        vents_n = vent_slot_count,
                        txt_on = enable_custom_text,
                        txt_str = custom_text,
                        txt_sz = custom_text_size,
                        txt_d = deboss_depth
                    );
        }
    }
}

module render_cyd() {
    if (show_cyd_board) {
        translate([0, 0, -0.5])
            cyd_board_mockup();
    }
}


// ==============================================================================
// GESTION DU MODE CHOISI
// ==============================================================================

if (mode == "assembled") {
    render_front_part();
    render_cyd();
    render_back_part();
}
else if (mode == "exploded") {
    translate([0, 0, explosion_distance])
        render_front_part();
        
    render_cyd();
        
    translate([0, 0, -explosion_distance])
        render_back_part();
}
else if (mode == "front_only") {
    render_front_part();
    if (show_cyd_board) render_cyd();
}
else if (mode == "back_only") {
    color(color_back)
        cyd_ha_back_skadis(
            src = source,
            vent = enable_ventilation_slots,
            vents_n = vent_slot_count,
            txt_on = enable_custom_text,
            txt_str = custom_text,
            txt_sz = custom_text_size,
            txt_d = deboss_depth
        );
}
else if (mode == "print_bed") {
    // 1. Façade avant (à plat sur Z=0)
    if (print_front_orientation == "face_down") {
        translate([-35, 0, 6.5])
            color(color_front)
                if (source == "mesh") {
                    cyd_ha_front(src = "mesh", pinhole = enable_reset_pinhole, extra_usb = enable_extra_usb_clearance);
                } else {
                    rotate([180, 0, 0]) cyd_ha_front(src = "parametric", pinhole = enable_reset_pinhole, extra_usb = enable_extra_usb_clearance);
                }
    } else {
        translate([-35, 0, 6.5])
            render_front_part();
    }
    
    // 2. Dos SKADIS (à plat sur Z=0, crochets vers le haut)
    translate([35, 0, 6.985])
        color(color_back)
            cyd_ha_back_skadis(
                src = source,
                vent = enable_ventilation_slots,
                vents_n = vent_slot_count,
                txt_on = enable_custom_text,
                txt_str = custom_text,
                txt_sz = custom_text_size,
                txt_d = deboss_depth
            );
}

