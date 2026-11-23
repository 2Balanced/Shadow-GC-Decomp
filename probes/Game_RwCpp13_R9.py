"""Wave 40, RwCpp13 round 9: fn_8044E408 with the weights indexed as one float array."""
SCORE = ["fn_8044E408"]
S0 = """                if (((f32 *)&skin->weights[i] + j)[k] > ((f32 *)&skin->weights[i] + j)[0]) {
                    f32 swap = ((f32 *)&skin->weights[i] + j)[0];
                    u32 index;

                    ((f32 *)&skin->weights[i] + j)[0] = ((f32 *)&skin->weights[i] + j)[k];
                    ((f32 *)&skin->weights[i] + j)[k] = swap;
"""
P0 = "                *packed = (u8)(128.0f * WEIGHT(skin, i, k));\n"
def v(a, b):
    return [(S0, S0.replace("((f32 *)&skin->weights[i] + j)[k]", a).replace("((f32 *)&skin->weights[i] + j)[0]", b))]
REPL = {
 "base": [(S0, S0)],
 "flat1": v("((f32 *)skin->weights)[i * 4 + j + k]", "((f32 *)skin->weights)[i * 4 + j]"),
 "flat2": v("((f32 *)skin->weights + i * 4 + j)[k]", "((f32 *)skin->weights + i * 4 + j)[0]"),
 "flat3": v("((f32 *)skin->weights)[(i * 4 + j) + k]", "((f32 *)skin->weights)[i * 4 + j]"),
 "flat2p": v("((f32 *)skin->weights + i * 4 + j)[k]", "((f32 *)skin->weights + i * 4 + j)[0]") + [(P0, "                *packed = (u8)(128.0f * ((f32 *)skin->weights)[i * 4 + k]);\n")],
 "flat2p2": v("((f32 *)skin->weights + i * 4 + j)[k]", "((f32 *)skin->weights + i * 4 + j)[0]") + [(P0, "                *packed = (u8)(128.0f * ((f32 *)skin->weights + i * 4)[k]);\n")],
}
