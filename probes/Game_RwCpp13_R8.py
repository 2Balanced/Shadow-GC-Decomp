"""Wave 40, RwCpp13 round 8: fn_8044E408's weight addressing (retail: weights + i*16 + j*4 as one
offset, the compared weight k*4 past it, the skin's weights pointer reloaded for every access)."""
SCORE = ["fn_8044E408"]
S0 = """                if (WEIGHT(skin, i, j + k) > WEIGHT(skin, i, j)) {
                    f32 swap = WEIGHT(skin, i, j);
                    u32 index;

                    WEIGHT(skin, i, j) = WEIGHT(skin, i, j + k);
                    WEIGHT(skin, i, j + k) = swap;
"""
def v(a, b):
    return [(S0, S0.replace("WEIGHT(skin, i, j + k)", a).replace("WEIGHT(skin, i, j)", b))]
REPL = {
 "base": [(S0, S0)],
 "wj_k": v("(&WEIGHT(skin, i, j))[k]", "WEIGHT(skin, i, j)"),
 "wj_k0": v("(&WEIGHT(skin, i, j))[k]", "(&WEIGHT(skin, i, j))[0]"),
 "ptr_k": v("((f32 *)&skin->weights[i] + j)[k]", "((f32 *)&skin->weights[i] + j)[0]"),
 "ptr_k2": v("*((f32 *)&skin->weights[i] + j + k)", "*((f32 *)&skin->weights[i] + j)"),
 "ptr_k3": v("((f32 *)(skin->weights + i) + j)[k]", "*((f32 *)(skin->weights + i) + j)"),
}
