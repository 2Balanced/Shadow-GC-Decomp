"""Wave 41, RenderWare/RpClump: struct copies and small register differences."""
SCORE = ["fn_80456F0C"]
OLD = """                u32 *source = (u32 *)nested[23];
                fields[7] = source[1];
                fields[8] = source[2];
                fields[9] = source[3];
                fields[10] = source[4];"""
NEW = """                *(ClumpSphere *)&fields[7] = *(ClumpSphere *)((u8 *)nested[23] + 4);"""
TYPE_ANCHOR = "typedef struct ClumpFreeList {"
TYPE = """typedef struct ClumpSphere {
    f32 x;
    f32 y;
    f32 z;
    f32 radius;
} ClumpSphere;

typedef struct ClumpFreeList {"""
REPL = {
    "base": [],
    "struct": [(TYPE_ANCHOR, TYPE), (OLD, NEW)],
}
