"""Wave 41: fn_800221A8 (retail: radius in f31) with a named copy of the radius under
'#pragma opt_propagation off' (the fn_8029EB5C lever)."""
SCORE = ["fn_800221A8"]
H = """extern "C" bool fn_800221A8(const Vec3f *center, RwV3d *move, const Vec3f **v, Vec3f *point, float *distance,
                            s32 *type, s32 *index, PlaneList *planes, CollisionResultList *results, bool contacts,
                            float radius) {
    Vec3f d;
"""
H2 = H.replace("float radius) {", "float radius_) {")
P0 = "#pragma opt_propagation off\n"
REPL = {
  "prag": [(H, P0 + H)],
  "prag_copy_first": [(H, P0 + H2.replace("    Vec3f d;\n", "    float radius = radius_;\n    Vec3f d;\n"))],
  "copy_first": [(H, H2.replace("    Vec3f d;\n", "    float radius = radius_;\n    Vec3f d;\n"))],
  "prag_copy_after_vecs": [(H, P0 + H2), ("    Vec3f q;\n    float s;\n", "    Vec3f q;\n    float radius = radius_;\n    float s;\n")],
}
