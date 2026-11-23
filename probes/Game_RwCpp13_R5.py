"""Wave 40, RwCpp13 round 5: fn_8044DA3C's first statements (retail copies the object to r30 before
forming lbl_8056D708's address in r3) and fn_8044D924 with a typed atomic made from the object."""
SCORE = ["fn_8044DA3C", "fn_8044D924"]
A0 = """    R2Atomic *atomic = object;
    R2Geometry *geometry;
    R2Skin *skin;
"""
B0 = """    geometry = atomic->geometry;
    skin = SKIN_OF(geometry);
    if (skin->maxWeights > 1 || skin->unk2C == 3) {
        instance = &"""
D0 = """R2Atomic *fn_8044D924(R2Atomic *atomic, R2ResEntry **instance) {
    R2Geometry *geometry = atomic->geometry;
"""
REPL = {
 "base": [(A0, A0)],
 "init": [(A0, "    R2Atomic *atomic = object;\n    R2Geometry *geometry = atomic->geometry;\n    R2Skin *skin = SKIN_OF(geometry);\n"),
          (B0, "    if (skin->maxWeights > 1 || skin->unk2C == 3) {\n        instance = &")],
 "cast": [(A0, A0.replace("= object;", "= (R2Atomic *)object;"))],
 "late": [(A0, "    R2Atomic *atomic;\n    R2Geometry *geometry;\n    R2Skin *skin;\n"), (B0, "    atomic = object;\n" + B0)],
 "skin_ptr": [(B0, B0.replace("    skin = SKIN_OF(geometry);\n", "    skin = *(R2Skin **)((u8 *)geometry + lbl_8056D708.geometryOffset);\n"))],
 "d_obj": [(D0, "R2Atomic *fn_8044D924(void *object, R2ResEntry **instance) {\n    R2Atomic *atomic = object;\n    R2Geometry *geometry = atomic->geometry;\n")],
 "d_obj_late": [(D0, "R2Atomic *fn_8044D924(void *object, R2ResEntry **instance) {\n    R2Atomic *atomic = object;\n    R2Geometry *geometry;\n"),
                ("    R2Skin *skin = fn_8044CCAC(geometry);\n    void *owner;\n    R2ResEntry **ownerRef;\n", "    R2Skin *skin;\n    void *owner;\n    R2ResEntry **ownerRef;\n\n    geometry = atomic->geometry;\n    skin = fn_8044CCAC(geometry);\n")],
}
