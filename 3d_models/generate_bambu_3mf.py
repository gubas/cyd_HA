#!/usr/bin/env python3
"""
Générateur de projet 3MF Bambu Lab natif de A à Z pour le boîtier CYD Home Assistant.
Assemble les 2 composants (Façade Avant et Dos SKADIS) sur un plateau complet Bambu Studio
avec orientation optimale à plat sur le lit (Z=0, sans supports) et profils PETG.
"""

import sys
import os
import struct
import zipfile
import json

BAMBU_STUDIO_VERSION = "02.08.04.57"

CONTENT_TYPES_XML = """<?xml version="1.0" encoding="UTF-8"?>
<Types xmlns="http://schemas.openxmlformats.org/package/2006/content-types">
  <Default Extension="rels" ContentType="application/vnd.openxmlformats-package.relationships+xml"/>
  <Default Extension="model" ContentType="application/vnd.ms-package.3dmanufacturing-3dmodel+xml"/>
  <Default Extension="config" ContentType="text/xml"/>
  <Default Extension="json" ContentType="application/json"/>
  <Default Extension="xml" ContentType="application/xml"/>
</Types>"""

RELS_XML = """<?xml version="1.0" encoding="UTF-8"?>
<Relationships xmlns="http://schemas.openxmlformats.org/package/2006/relationships">
  <Relationship Target="/3D/3dmodel.model" Id="rel-1" Type="http://schemas.microsoft.com/3dmanufacturing/2013/01/3dmodel"/>
</Relationships>"""

SLICE_INFO_CONFIG = f"""<?xml version="1.0" encoding="UTF-8"?>
<config>
  <header>
    <header_item key="X-BBL-Client-Type" value="slicer"/>
    <header_item key="X-BBL-Client-Version" value="{BAMBU_STUDIO_VERSION}"/>
  </header>
</config>
"""

PLATE_JSON = json.dumps({
    "bed_type": "textured_plate",
    "first_extruder": 0,
    "nozzle_diameter": 0.4,
    "version": 2
}, indent=2)

FILAMENT_SEQUENCE_JSON = json.dumps({
    "plate_1": {
        "nozzle_sequence": [],
        "optimal_assignment": [],
        "sequence": []
    }
}, indent=2)

CUT_INFORMATION_XML = """<?xml version="1.0" encoding="utf-8"?>
<objects/>
"""

def read_stl(filepath):
    """Lit un fichier STL binaire ou ascii et renvoie la liste des sommets et les triangles."""
    with open(filepath, 'rb') as f:
        data = f.read()

    if len(data) < 84:
        raise ValueError(f"Fichier STL trop court : {filepath}")

    num_triangles = struct.unpack('<I', data[80:84])[0]
    expected_size = 84 + num_triangles * 50

    if len(data) == expected_size:
        # STL binaire
        vertices = []
        triangles = []
        vertex_map = {}
        
        offset = 84
        for _ in range(num_triangles):
            v_data = data[offset+12:offset+48]
            offset += 50
            x1, y1, z1, x2, y2, z2, x3, y3, z3 = struct.unpack('<9f', v_data)
            
            tri = []
            for (x, y, z) in [(x1, y1, z1), (x2, y2, z2), (x3, y3, z3)]:
                key = (round(x, 4), round(y, 4), round(z, 4))
                if key not in vertex_map:
                    idx = len(vertices)
                    vertex_map[key] = idx
                    vertices.append(key)
                else:
                    idx = vertex_map[key]
                tri.append(idx)
            triangles.append(tri)
        return vertices, triangles
    else:
        # Fallback STL ascii
        vertices = []
        triangles = []
        vertex_map = {}
        lines = data.decode('utf-8', errors='ignore').splitlines()
        current_tri = []
        for line in lines:
            line = line.strip()
            if line.startswith('vertex'):
                parts = line.split()
                x, y, z = float(parts[1]), float(parts[2]), float(parts[3])
                key = (round(x, 4), round(y, 4), round(z, 4))
                if key not in vertex_map:
                    idx = len(vertices)
                    vertex_map[key] = idx
                    vertices.append(key)
                else:
                    idx = vertex_map[key]
                current_tri.append(idx)
                if len(current_tri) == 3:
                    triangles.append(current_tri)
                    current_tri = []
        return vertices, triangles

def get_project_settings(colors=None):
    """Charge le template Bambu Studio et configure les filaments."""
    cur_dir = os.path.dirname(os.path.abspath(__file__))
    template_path = os.path.join(cur_dir, 'bambu_project_template.json')
    if not os.path.exists(template_path):
        return None
    try:
        with open(template_path, 'r', encoding='utf-8') as f:
            cfg = json.load(f)
        if colors:
            hex_colors = [c[:7] if c.startswith('#') else f"#{c}" for c in colors]
            cfg['filament_colour'] = hex_colors
            cfg['filament_type'] = ['PETG'] * len(hex_colors)
            cfg['filament_vendor'] = ['Bambu'] * len(hex_colors)
            cfg['filament_settings_id'] = ['Bambu PETG Basic @BBL X1C'] * len(hex_colors)
        return json.dumps(cfg, indent=2)
    except Exception as e:
        print(f"Attention : impossible de charger le template de projet : {e}")
        return None

def write_bambu_3mf_package(output_3mf, model_content, model_settings_content, proj_cfg=None):
    """Assemble l'archive ZIP 3MF standardisé avec métadonnées Bambu Studio."""
    os.makedirs(os.path.dirname(os.path.abspath(output_3mf)), exist_ok=True)
    with zipfile.ZipFile(output_3mf, 'w', compression=zipfile.ZIP_DEFLATED) as z:
        z.writestr('[Content_Types].xml', CONTENT_TYPES_XML)
        z.writestr('_rels/.rels', RELS_XML)
        z.writestr('3D/3dmodel.model', model_content)
        z.writestr('Metadata/model_settings.config', model_settings_content)
        z.writestr('Metadata/slice_info.config', SLICE_INFO_CONFIG)
        z.writestr('Metadata/plate_1.json', PLATE_JSON)
        z.writestr('Metadata/filament_sequence.json', FILAMENT_SEQUENCE_JSON)
        z.writestr('Metadata/cut_information.xml', CUT_INFORMATION_XML)
        if proj_cfg:
            z.writestr('Metadata/project_settings.config', proj_cfg)

def generate_cyd_ha_3mf(output_3mf="cyd_ha_case_skadis.3mf"):
    """
    Génère de 0 le projet 3MF complet pour Bambu Studio contenant :
    - CYD_HA_Front (Façade Avant à plat sur le lit)
    - CYD_HA_Back_Skadis (Dos SKADIS à plat, crochets vers le haut)
    """
    cur_dir = os.path.dirname(os.path.abspath(__file__))
    front_stl = os.path.join(cur_dir, "cyd_ha_front.stl")
    back_stl = os.path.join(cur_dir, "cyd_ha_back_skadis.stl")

    for f in [front_stl, back_stl]:
        if not os.path.exists(f):
            raise FileNotFoundError(f"Fichier STL manquant : {f}. Lancez build_models d'abord !")

    color_front = "#F5C242FF" # Jaune / Or Silk
    color_back = "#3388E6FF"  # Bleu SKADIS

    model_xml = [
        '<?xml version="1.0" encoding="UTF-8"?>',
        '<model unit="millimeter" xml:lang="fr-FR" xmlns="http://schemas.microsoft.com/3dmanufacturing/core/2015/02" xmlns:BambuStudio="http://schemas.bambulab.com/package/2021" xmlns:p="http://schemas.microsoft.com/3dmanufacturing/production/2015/06" requiredextensions="p">',
        f'  <metadata name="Application">BambuStudio-{BAMBU_STUDIO_VERSION}</metadata>',
        '  <metadata name="BambuStudio:3mfVersion">1</metadata>',
        '  <metadata name="Title">CYD Home Assistant Case (IKEA SKADIS)</metadata>',
        '  <metadata name="Description">Boîtier modulaire pour écran CYD ESP32-2432S028R avec fixation IKEA SKADIS pour Home Assistant.</metadata>',
        '  <resources>',
        '    <basematerials id="1">',
        f'      <base name="PETG_Front" displaycolor="{color_front}" />',
        f'      <base name="PETG_Back" displaycolor="{color_back}" />',
        '    </basematerials>'
    ]

    model_settings_lines = [
        '<?xml version="1.0" encoding="UTF-8"?>',
        '<config>',
    ]

    build_items = []
    current_obj_id = 2

    # Centre du plateau Bambu Lab (256x256 mm -> centre à 128, 128)
    BED_CENTER_X = 128.0
    BED_CENTER_Y = 128.0

    items_to_place = [
        ("CYD_HA_Front", front_stl, BED_CENTER_X - 42.0, BED_CENTER_Y, 0, 1),
        ("CYD_HA_Back_Skadis", back_stl, BED_CENTER_X + 42.0, BED_CENTER_Y, 1, 1),
    ]

    for (part_name, stl_path, target_x, target_y, pindex, ext_id) in items_to_place:
        verts, tris = read_stl(stl_path)
        
        min_x = min(v[0] for v in verts)
        max_x = max(v[0] for v in verts)
        min_y = min(v[1] for v in verts)
        max_y = max(v[1] for v in verts)
        min_z = min(v[2] for v in verts)

        cx = (min_x + max_x) / 2.0
        cy = (min_y + max_y) / 2.0
        cz = min_z  # Repose exactement sur le lit à Z=0

        shift_x = target_x - cx
        shift_y = target_y - cy
        shift_z = -cz

        obj_id = current_obj_id
        current_obj_id += 1

        model_xml.append(f'    <object id="{obj_id}" name="{part_name}" type="model" pid="1" pindex="{pindex}">')
        model_xml.append('      <mesh>')
        model_xml.append('        <vertices>')
        for (x, y, z) in verts:
            model_xml.append(f'          <vertex x="{x+shift_x:.4f}" y="{y+shift_y:.4f}" z="{z+shift_z:.4f}" />')
        model_xml.append('        </vertices>')
        model_xml.append('        <triangles>')
        for (v1, v2, v3) in tris:
            model_xml.append(f'          <triangle v1="{v1}" v2="{v2}" v3="{v3}" pid="1" p1="{pindex}" />')
        model_xml.append('        </triangles>')
        model_xml.append('      </mesh>')
        model_xml.append('    </object>')

        build_items.append(obj_id)

        model_settings_lines.append(f'  <object id="{obj_id}">')
        model_settings_lines.append(f'    <metadata key="name" value="{part_name}"/>')
        model_settings_lines.append('    <metadata key="plate_id" value="1"/>')
        model_settings_lines.append(f'    <part id="{obj_id}" subtype="normal">')
        model_settings_lines.append(f'      <metadata key="name" value="{part_name}"/>')
        model_settings_lines.append('      <metadata key="matrix" value="1 0 0 0 0 1 0 0 0 0 1 0 0 0 0 1"/>')
        model_settings_lines.append(f'      <metadata key="extruder" value="{ext_id}"/>')
        model_settings_lines.append('    </part>')
        model_settings_lines.append('  </object>')

    model_xml.append('  </resources>')
    model_xml.append('  <build>')
    for bid in build_items:
        model_xml.append(f'    <item objectid="{bid}" />')
    model_xml.append('  </build>')
    model_xml.append('</model>')

    model_settings_lines.append('  <plate>')
    model_settings_lines.append('    <metadata key="plater_id" value="1"/>')
    model_settings_lines.append('    <metadata key="plater_name" value="Plateau CYD HA"/>')
    model_settings_lines.append('    <metadata key="locked" value="false"/>')
    model_settings_lines.append('  </plate>')
    model_settings_lines.append('</config>')

    model_content = "\n".join(model_xml)
    model_settings_content = "\n".join(model_settings_lines)

    proj_cfg = get_project_settings([color_front, color_back])
    out_file = os.path.join(cur_dir, output_3mf)
    write_bambu_3mf_package(out_file, model_content, model_settings_content, proj_cfg)

    sz_kb = os.path.getsize(out_file) / 1024
    print(f"[OK] Projet Bambu Lab 3MF genere de zero : {out_file} ({sz_kb:.1f} KB)")
    print("  Composants : CYD_HA_Front, CYD_HA_Back_Skadis")
    print("  Plateau 256x256 mm : Z=0 a plat, aucun support requis")

if __name__ == "__main__":
    out_name = sys.argv[1] if len(sys.argv) > 1 else "cyd_ha_case_skadis.3mf"
    generate_cyd_ha_3mf(out_name)
