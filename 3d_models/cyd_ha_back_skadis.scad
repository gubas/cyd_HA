// ==============================================================================
// 🛠️ BOÎTIER CYD HOME ASSISTANT - PIÈCE 2 : DOS AVEC CROCHETS IKEA SKADIS
// ==============================================================================
// Fichier dédié exclusivement au dos avec système d'accroche SKADIS.
// Compatible OpenSCAD Customizer (Fenêtre > Personnalisateur).
// Appuyez sur F6 (Rendu) puis F7 pour exporter en STL.
// ==============================================================================

/* [Source & Disposition] */
// Source géométrique de la pièce
source = "mesh"; // [mesh:Modèle 3MF/STL Original (Haute Fidélité), parametric:Reconstruction 100% Paramétrique]

// Orientation de la pièce
orientation = "print_bed"; // [print_bed:À Plat sur le Plateau (Crochets vers le haut, Z=0), upright:Position Verticale (Accroché au SKADIS)]

/* [Personnalisations & Découpes CSG] */
// Fentes de ventilation arrière (utile pour sonde de température interne)
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

/* [Paramètres Géométriques (Mode Paramétrique Uniquement)] */
case_w = 54.1;         // Largeur extérieure (mm)
case_h = 90.0;         // Hauteur extérieure (mm)
corner_r = 3.5;        // Rayon des arrondis extérieurs (mm)
wall_t = 2.0;          // Épaisseur de paroi (mm)
skadis_pitch = 40.0;   // Entraxe horizontal standard IKEA SKADIS (mm)

/* [Résolution Arcs] */
$fn = 40;

// Couleur d'affichage
color_back = [0.20, 0.55, 0.90, 1.0];


// ==============================================================================
// MODULE PRINCIPAL : DOS SKADIS
// ==============================================================================

module cyd_ha_back_skadis(
    src = source,
    vent = enable_ventilation_slots,
    vents_n = vent_slot_count,
    txt_on = enable_custom_text,
    txt_str = custom_text,
    txt_sz = custom_text_size,
    txt_d = deboss_depth
) {
    difference() {
        if (src == "mesh") {
            // Modèle issu du 3MF d'origine
            import("cyd_ha_back_skadis.stl");
        } else {
            // Modèle 100% paramétrique natif
            back_parametric_body();
        }
        
        // Fentes d'aération
        if (vent) {
            for (i = [0 : vents_n - 1]) {
                y_pos = -25 + i * 5;
                translate([0, y_pos, 0])
                    cube([25, 2.0, 20], center = true);
            }
        }
        
        // Texte débossé
        if (txt_on && txt_str != "") {
            z_pos = (src == "mesh") ? -6.99 + txt_d - 0.01 : txt_d - 0.01;
            translate([0, -25, z_pos])
                rotate([180, 0, 0])
                    linear_extrude(height = txt_d + 0.1)
                        text(txt_str, size = txt_sz, font = "Liberation Sans:style=Bold", halign = "center", valign = "center");
        }
    }
}

module skadis_hook() {
    // Crochet IKEA SKADIS standard : fente de 5x15mm, épaisseur panneau 5mm
    union() {
        // Corps traversant
        translate([0, 0, 2.75])
            cube([4.6, 14.5, 5.5], center = true);
            
        // Dent de verrouillage supérieure
        translate([0, 5.25, 5.5 + 1.75])
            cube([4.6, 4.0, 3.5], center = true);
            
        // Biseau d'engagement
        translate([0, 5.25, 7.25 + 0.8])
            rotate([45, 0, 0])
                cube([4.6, 2.0, 2.0], center = true);
    }
}

module back_parametric_body() {
    plate_t = 2.5;
    lip_t = 2.5;
    
    union() {
        // Plaque arrière de base
        linear_extrude(height = plate_t)
            rounded_rect([case_w, case_h], corner_r);
            
        // Lèvre de centrage s'insérant dans la façade
        translate([0, -1.8, plate_t])
            linear_extrude(height = lip_t)
                rounded_rect([case_w - 2 * wall_t - 0.4, case_h - 2 * wall_t - 0.4], max(0.5, corner_r - wall_t - 0.2));
                
        // Deux crochets SKADIS espacés de 40mm
        for (sx = [-1, 1]) {
            translate([sx * skadis_pitch / 2, 10, -5.5])
                rotate([180, 0, 0])
                    skadis_hook();
        }
    }
}

module rounded_rect(size, r) {
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
// RENDU DE LA PIÈCE
// ==============================================================================

if (orientation == "print_bed") {
    // À plat sur le lit d'impression : crochets vers le haut, Z=0
    // => Zéro support d'impression requis !
    color(color_back) {
        if (source == "mesh") {
            translate([0, 0, 6.985])
                cyd_ha_back_skadis();
        } else {
            cyd_ha_back_skadis();
        }
    }
} else {
    // Position verticale d'accroche (crochets vers l'arrière)
    color(color_back) {
        if (source == "mesh") {
            rotate([0, 180, 0])
                cyd_ha_back_skadis();
        } else {
            rotate([0, 180, 0])
                cyd_ha_back_skadis();
        }
    }
}

