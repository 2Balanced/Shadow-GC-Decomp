# Round 2 for src/RenderWare/RwMatrix.c: RwMatrixRotateOneMinusCosineSine's statement order.
import itertools
exec(open('probes/RenderWare_RwMatrix.py').read().split('A = """')[0])
SYMBOLS = {"fn_8047ECD4": SYMBOLS["fn_8047ECD4"]}
DECL_V = """
    RwMatrix rotation;
    RwMatrix result;
    RwV3d leading;
    RwV3d crossed;
    RwV3d sine;
    f32 x = axis->x;
    f32 y = axis->y;
    f32 z = axis->z;

"""
COMP_V = """    leading.x = x * x;
    leading.y = y * y;
    leading.z = z * z;
    leading.x = 1.0f - leading.x;
    leading.y = 1.0f - leading.y;
    leading.z = 1.0f - leading.z;
    crossed.x = y * z;
    crossed.y = z * x;
    crossed.z = x * y;
    leading.x *= oneMinusCos;
    leading.y *= oneMinusCos;
    leading.z *= oneMinusCos;
    crossed.x *= oneMinusCos;
    crossed.y *= oneMinusCos;
    crossed.z *= oneMinusCos;
    sine.x = x * sinA;
    sine.y = y * sinA;
    sine.z = z * sinA;
"""
STORES = """    rotation.pos.x = 0.0f;
    rotation.pos.y = 0.0f;
    rotation.pos.z = 0.0f;
    rotation.flags = 3;
    rotation.right.x = 1.0f - leading.x;
    rotation.up.y = 1.0f - leading.y;
    rotation.at.z = 1.0f - leading.z;
    rotation.up.z = crossed.x + sine.x;
    rotation.right.z = crossed.y - sine.y;
    rotation.at.x = crossed.y + sine.y;
    rotation.at.y = crossed.x - sine.x;
    rotation.right.y = crossed.z + sine.z;
    rotation.up.x = crossed.z - sine.z;
"""
BODIES = {"fn_8047ECD4": {
    "D": DECL_V + COMP_V + STORES + SWITCH,
    "D2": DECL_V + STORES.split("    rotation.right.x")[0] + COMP_V + "    rotation.right.x" + STORES.split("    rotation.right.x")[1] + SWITCH,
}}
# scalar temporaries
DECL_S = """
    RwMatrix rotation;
    RwMatrix result;
    f32 x = axis->x;
    f32 y = axis->y;
    f32 z = axis->z;
    f32 lx, ly, lz;
    f32 cx, cy, cz;
    f32 sx, sy, sz;

"""
COMP_S = COMP_V.replace("leading.", "l").replace("crossed.", "c").replace("sine.", "s")
STORES_S = STORES.replace("leading.", "l").replace("crossed.", "c").replace("sine.", "s")
BODIES["fn_8047ECD4"]["E"] = DECL_S + COMP_S + STORES_S + SWITCH
BODIES["fn_8047ECD4"]["E2"] = DECL_S + STORES_S.split("    rotation.right.x")[0] + COMP_S + "    rotation.right.x" + STORES_S.split("    rotation.right.x")[1] + SWITCH
