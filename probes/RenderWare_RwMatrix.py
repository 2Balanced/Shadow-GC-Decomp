# Probe table for src/RenderWare/RwMatrix.c (tools/pprobe.py -u RenderWare/<Unit> -p <table>): the rotation builders.
SYMBOLS = {
    "fn_8047ECD4": "RwMatrix *fn_8047ECD4(RwMatrix *matrix, const RwV3d *axis, f32 oneMinusCos, f32 sinA, s32 combineOp) {",
    "fn_8047EFC8": "RwMatrix *fn_8047EFC8(RwMatrix *matrix, const RwV3d *axis, f32 angle, s32 combineOp) {",
}
SWITCH = """    switch (combineOp) {
    case 0:
        *matrix = rotation;
        break;
    case 1:
        fn_8047EBB8(&result, &rotation, matrix);
        *matrix = result;
        break;
    case 2:
        fn_8047EBB8(&result, matrix, &rotation);
        *matrix = result;
        break;
    default:
        R5ERROR(1, (0x80000003, "Invalid combination type"));
        matrix = 0;
        break;
    }
    return matrix;
"""
A = """
    RwMatrix result;
    RwMatrix rotation;
    RwV3d leading;
    RwV3d crossed;
    RwV3d axisSine;
    f32 x = axis->x;
    f32 y = axis->y;
    f32 z = axis->z;

    leading.x = 1.0f - x * x;
    leading.y = 1.0f - y * y;
    leading.z = 1.0f - z * z;
    crossed.x = y * z;
    crossed.y = z * x;
    crossed.z = x * y;
    leading.x *= oneMinusCos;
    leading.y *= oneMinusCos;
    leading.z *= oneMinusCos;
    crossed.x *= oneMinusCos;
    crossed.y *= oneMinusCos;
    crossed.z *= oneMinusCos;
    axisSine.x = x * sinA;
    axisSine.y = y * sinA;
    axisSine.z = z * sinA;
    rotation.right.x = 1.0f - leading.x;
    rotation.right.y = crossed.z + axisSine.z;
    rotation.right.z = crossed.y - axisSine.y;
    rotation.up.x = crossed.z - axisSine.z;
    rotation.up.y = 1.0f - leading.y;
    rotation.up.z = crossed.x + axisSine.x;
    rotation.at.x = crossed.y + axisSine.y;
    rotation.at.y = crossed.x - axisSine.x;
    rotation.at.z = 1.0f - leading.z;
    rotation.pos.x = 0.0f;
    rotation.pos.y = 0.0f;
    rotation.pos.z = 0.0f;
    rotation.flags = 3;
"""
B = A.replace("""    leading.x = 1.0f - x * x;
    leading.y = 1.0f - y * y;
    leading.z = 1.0f - z * z;""", """    leading.x = x * x;
    leading.y = y * y;
    leading.z = z * z;
    leading.x = 1.0f - leading.x;
    leading.y = 1.0f - leading.y;
    leading.z = 1.0f - leading.z;""")
C = B.replace("""    rotation.pos.x = 0.0f;
    rotation.pos.y = 0.0f;
    rotation.pos.z = 0.0f;
    rotation.flags = 3;
""", "").replace("""    crossed.x = y * z;""", """    rotation.pos.x = 0.0f;
    rotation.pos.y = 0.0f;
    rotation.pos.z = 0.0f;
    rotation.flags = 3;
    crossed.x = y * z;""")
BODIES = {
    "fn_8047ECD4": {"rotA": A + SWITCH, "rotB": B + SWITCH, "rotC": C + SWITCH},
    "fn_8047EFC8": {
        "rotdeg_a": """
    f32 sinA;
    f32 radians = (3.1415927f / 180.0f) * angle;
    f32 recip = fn_80482394(axis->x * axis->x + axis->y * axis->y + axis->z * axis->z);
    RwV3d unit;
    f32 cosA;

    unit.x = axis->x * recip;
    unit.y = axis->y * recip;
    unit.z = axis->z * recip;
    sinA = (f32)sin(radians);
    cosA = (f32)cos(radians);
    fn_8047ECD4(matrix, &unit, 1.0f - cosA, sinA, combineOp);
    return matrix;
""",
        "rotdeg_b": """
    RwV3d unit;
    f32 sinA;
    f32 cosA;
    f32 recip;
    f32 radians;

    radians = (3.1415927f / 180.0f) * angle;
    recip = fn_80482394(axis->x * axis->x + axis->y * axis->y + axis->z * axis->z);
    unit.x = axis->x * recip;
    unit.y = axis->y * recip;
    unit.z = axis->z * recip;
    sinA = (f32)sin(radians);
    cosA = (f32)cos(radians);
    fn_8047ECD4(matrix, &unit, 1.0f - cosA, sinA, combineOp);
    return matrix;
""",
    },
}
