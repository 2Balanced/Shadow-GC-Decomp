"""Wave 40, RwCpp13 (RpSkin GameCube) round 1: parameter registers of fn_8044DC4C (retail: id r31,
normals r30), temporaries of fn_8044EE3C, the size sum of fn_8044E7D0."""
SCORE = ["fn_8044DC4C", "fn_8044EE3C", "fn_8044E7D0"]
SIG = "void fn_8044DC4C(R2Matrix *matrix, u32 id, s32 normals) {\n"
D0 = "    f32 mtx[3][4];\n\n"
T0 = "    fn_803991E8(mtx, id);\n    if (normals) {\n        fn_80399238(mtx, id);\n    }\n"
E0 = """        R2SkinInstance *instance = &((R2SkinResEntry *)atomic->repEntry)->instance;
        R2Geometry *geometry = atomic->geometry;
"""
R0 = "    return size + fn_8044BDBC(skin);\n"
REPL = {
 "base": [(R0, R0)],
 "u32n": [(SIG, SIG.replace("s32 normals", "u32 normals"))],
 "s32id": [(SIG, SIG.replace("u32 id", "s32 id"))],
 "s32both": [(SIG, SIG.replace("u32 id", "s32 id").replace("s32 normals", "u32 normals"))],
 "ne0": [(T0, T0.replace("if (normals)", "if (normals != 0)"))],
 "mtxptr": [(D0, "    f32 mtx[3][4];\n    f32 (*m)[4] = mtx;\n\n"), (T0, T0.replace("(mtx, id)", "(m, id)"))],
 "geo_first": [(E0, "        R2Geometry *geometry = atomic->geometry;\n        R2SkinInstance *instance = &((R2SkinResEntry *)atomic->repEntry)->instance;\n")],
 "geo_top": [("    R2SkinAtomic *data = SKIN_ATOMIC(atomic);\n\n    if (data->positions != NULL) {\n" + E0,
              "    R2Geometry *geometry;\n    R2SkinAtomic *data = SKIN_ATOMIC(atomic);\n\n    if (data->positions != NULL) {\n        R2SkinInstance *instance = &((R2SkinResEntry *)atomic->repEntry)->instance;\n        geometry = atomic->geometry;\n")],
 "acc": [(R0, "    size += fn_8044BDBC(skin);\n    return size;\n")],
}
