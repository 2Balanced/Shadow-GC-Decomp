"""Wave 40, RwCpp12 round 3: fn_8044BDF0 (RpSkinCreate): where the loop locals are declared and
their types (retail keeps the new-bone flag and the bone index in r3/r4)."""
import itertools
SCORE = ["fn_8044BDF0"]
D = """    u8 usedBones[256];
    R2Skin *skin;
    u32 i;
    u32 j;
    u32 k;
"""
B = """                    u8 bone = (u8)(indices[i] >> (j * 8));
                    s32 isNew = 1;
                    for (k = 0; k < numUsedBones; k++) {"""
B2 = """                    bone = (u8)(indices[i] >> (j * 8));
                    isNew = 1;
                    for (k = 0; k < numUsedBones; k++) {"""
REPL = {}
for bt, nt, pos, it in itertools.product(["u8", "u32", "s32"], ["s32", "u32"], ["in", "top_first", "top_last", "top_mid"], ["u32", "s32"]):
    d = D.replace("u32 i;", it + " i;").replace("u32 j;", it + " j;").replace("u32 k;", it + " k;")
    decl = "    %s bone;\n    %s isNew;\n" % (bt, nt)
    if pos == "in":
        r = [(D, d), (B, B.replace("u8 bone", bt + " bone").replace("s32 isNew", nt + " isNew"))]
    elif pos == "top_first":
        r = [(D, d.replace("    R2Skin *skin;\n", decl + "    R2Skin *skin;\n")), (B, B2)]
    elif pos == "top_last":
        r = [(D, d + decl), (B, B2)]
    else:
        r = [(D, d.replace("    R2Skin *skin;\n", "    R2Skin *skin;\n" + decl)), (B, B2)]
    REPL["%s_%s_%s_%s" % (bt, nt, pos, it)] = r
