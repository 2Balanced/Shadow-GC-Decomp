"""Wave 41 (f1): EffectRay::Generate with the floats shared by the cases declared at the top of the
function (as EffectParticle's Generate needs them) instead of the top of the loop."""
SCORE = ["Generate__Q36Effect6System9EffectRayFf"]
TOP = """void EffectRay::Generate(float dt) {
    const UnkRayShape_804298D4 *shape;
    const UnkRayDesc_804298D4 *desc;
"""
LOOP = """    for (int i = 0; i < count; i++) {
        float spread;
        float angle;
        float size;
"""
REPL = {
    "fn_after_desc": [(TOP, TOP + "    float spread;\n    float angle;\n    float size;\n"), (LOOP, "    for (int i = 0; i < count; i++) {\n")],
    "fn_after_shape": [(TOP, TOP.replace("    const UnkRayDesc_804298D4 *desc;\n", "    float spread;\n    float angle;\n    float size;\n    const UnkRayDesc_804298D4 *desc;\n")), (LOOP, "    for (int i = 0; i < count; i++) {\n")],
    "fn_first": [(TOP, TOP.replace("    const UnkRayShape_804298D4 *shape;\n", "    float spread;\n    float angle;\n    float size;\n    const UnkRayShape_804298D4 *shape;\n")), (LOOP, "    for (int i = 0; i < count; i++) {\n")],
}
