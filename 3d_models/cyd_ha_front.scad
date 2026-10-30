// ==============================================================================
// 🛠️ BOÎTIER CYD HOME ASSISTANT - PIÈCE 1 : FAÇADE AVANT (ESP32-2432S028R)
// ==============================================================================
// Fichier dédié exclusivement à la façade avant du boîtier CYD_HA.
// Compatible OpenSCAD Customizer (Fenêtre > Personnalisateur).
// Appuyez sur F6 (Rendu) puis F7 pour exporter en STL.
// ==============================================================================

/* [Source & Disposition] */
// Source géométrique de la pièce
source = "mesh"; // [mesh:Modèle 3MF/STL Original (Haute Fidélité), parametric:Reconstruction 100% Paramétrique]

// Orientation de la pièce
orientation = "print_bed"; // [print_bed:À Plat sur le Plateau (Prêt à Imprimer), upright:Position Verticale (Usage Normal)]

// Face contre le plateau (uniquement si orientation = print_bed)
print_face = "face_down"; // [face_down:Face avant contre le lit (Idéal PEI Texturé), face_up:Face avant vers le haut]

// Afficher la maquette de la carte CYD en transparence pour vérifier l'ajustement
show_cyd_preview = false;

/* [Personnalisations & Découpes CSG] */
// Percer un trou d'accès pour le bouton RESET sur le flanc
enable_reset_pinhole = true;
// Diamètre du trou de reset (mm)
reset_pinhole_dia = 2.0;

// Élargir l'ouverture inférieure pour câbles USB-C à connecteur épais
enable_extra_usb_clearance = false;
// Dégagement supplémentaire en largeur (mm)
extra_usb_clearance_w = 4.0;

/* [Paramètres Géométriques (Mode Paramétrique Uniquement)] */
case_w = 54.1;         // Largeur extérieure (mm)
case_h = 90.0;         // Hauteur extérieure (mm)
case_d = 13.0;         // Profondeur totale (mm)
corner_r = 3.5;        // Rayon des arrondis extérieurs (mm)
wall_t = 2.0;          // Épaisseur de paroi (mm)

screen_w = 44.4;       // Largeur fenêtre écran (mm)
screen_h = 58.4;       // Hauteur fenêtre écran (mm)
screen_y_offset = 0.7; // Décalage vertical du centre écran (mm)

/* [Résolution Arcs] */
$fn = 40;

// Couleur d'affichage
color_front = [0.95, 0.77, 0.20, 1.0];


// ==============================================================================
// MODULE PRINCIPAL : FAÇADE AVANT
// ==============================================================================

module cyd_ha_front(
    src = source,
    pinhole = enable_reset_pinhole,
    pinhole_d = reset_pinhole_dia,
    extra_usb = enable_extra_usb_clearance,
    usb_extra_w = extra_usb_clearance_w
) {
    difference() {
        if (src == "mesh") {
            // Modèle issu du 3MF d'origine
            import("cyd_ha_front.stl");
        } else {
            // Modèle 100% paramétrique natif
            front_parametric_body();
        }
        
        // Découpes CSG personnalisées
        if (extra_usb) {
            translate([0, -case_h / 2, 0])
                cube([26 + usb_extra_w, 20, 20], center = true);
        }
        
        if (pinhole) {
            translate([-case_w / 2 - 1, 20, 0])
                rotate([0, 90, 0])
                    cylinder(r = pinhole_d / 2, h = 6);
        }
    }
}

module front_parametric_body() {
    difference() {
        // Enveloppe extérieure chanfreinée
        linear_extrude(height = case_d, center = true)
            rounded_rect([case_w, case_h], corner_r);
        
        // Cavité intérieure pour la carte CYD
        translate([0, -1.8, -wall_t])
            linear_extrude(height = case_d, center = false)
                rounded_rect([case_w - 2 * wall_t, case_h - 2 * wall_t], max(0.5, corner_r - wall_t));
        
        // Fenêtre d'affichage pour l'écran 2.8"
        translate([0, screen_y_offset, case_d / 2 - wall_t - 0.5])
            linear_extrude(height = wall_t + 2)
                rounded_rect([screen_w, screen_h], 1.5);
                
        // Évidement inférieur pour câbles et connecteur USB-C
        translate([0, -case_h / 2 + 5, -case_d / 2])
            cube([26, 18, case_d], center = true);
    }
    
    // 4 colonnettes de centrage de la carte CYD (entraxe 45 x 80 mm)
    for (sx = [-1, 1]) {
        for (sy = [-1, 1]) {
            translate([sx * 45 / 2, -1.8 + sy * 80 / 2, -1.0])
                cylinder(r = 1.3, h = 4.5);
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
// MAQUETTE CYD (OPTIONNELLE)
// ==============================================================================

module cyd_preview() {
    color([0.1, 0.1, 0.1, 0.5])
        translate([0, screen_y_offset, 2.5])
            cube([46.0, 65.0, 2.4], center = true);
    color([0.85, 0.65, 0.15, 0.4])
        translate([0, -1.8, 0])
            linear_extrude(1.6)
                rounded_rect([50.0, 86.0], 2.0);
}


// ==============================================================================
// RENDU DE LA PIÈCE
// ==============================================================================

if (orientation == "print_bed") {
    if (print_face == "face_down") {
        // Face avant contre le lit d'impression (Z=0, PEI texturé)
        color(color_front)
            if (source == "mesh") {
                translate([0, 0, 6.5]) cyd_ha_front();
            } else {
                translate([0, 0, case_d / 2]) rotate([180, 0, 0]) cyd_ha_front();
            }
    } else {
        // Face avant vers le haut (hollow vers le lit)
        color(color_front)
            if (source == "mesh") {
                translate([0, 0, 6.5]) rotate([0, 180, 0]) cyd_ha_front();
            } else {
                translate([0, 0, case_d / 2]) cyd_ha_front();
            }
    }
} else {
    // Position verticale d'utilisation
    color(color_front)
        if (source == "mesh") {
            rotate([0, 180, 0]) cyd_ha_front();
        } else {
            cyd_ha_front();
        }
        
    if (show_cyd_preview) {
        cyd_preview();
    }
}

