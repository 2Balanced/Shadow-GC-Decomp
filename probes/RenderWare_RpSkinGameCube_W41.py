"""Wave 41, RenderWare/RpSkinGameCube: fn_8044DA3C's skin-globals address."""
SCORE = ["fn_8044DA3C"]
REPL = {
    "base": [],
    "const_obj": [("R2Atomic *fn_8044DA3C(void *object, R2ResEntry **repEntry) {\n    R2SkinInstance *instance;\n    R2Atomic *atomic = object;",
                   "R2Atomic *fn_8044DA3C(void *object, R2ResEntry **repEntry) {\n    R2SkinInstance *instance;\n    R2Atomic *const atomic = object;")],
    "geom_first": [("    geometry = atomic->geometry;\n    skin = SKIN_OF(geometry);\n    if (skin->maxWeights > 1 || skin->unk2C == 3) {",
                    "    skin = SKIN_OF(atomic->geometry);\n    geometry = atomic->geometry;\n    if (skin->maxWeights > 1 || skin->unk2C == 3) {")],
    "skin_fn": [("    geometry = atomic->geometry;\n    skin = SKIN_OF(geometry);\n    if (skin->maxWeights > 1 || skin->unk2C == 3) {",
                 "    geometry = atomic->geometry;\n    skin = fn_8044CCAC(geometry);\n    if (skin->maxWeights > 1 || skin->unk2C == 3) {")],
    "decl_skin_first": [("    R2SkinInstance *instance;\n    R2Atomic *atomic = object;\n    R2Geometry *geometry;\n    R2Skin *skin;",
                         "    R2Skin *skin;\n    R2SkinInstance *instance;\n    R2Atomic *atomic = object;\n    R2Geometry *geometry;")],
}
