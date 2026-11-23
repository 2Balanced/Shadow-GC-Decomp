"""Wave 40 (r2), RwCpp14 round 1: the index type argument as a conditional expression evaluated in
argument order (retail loads the count, forms the descriptor address and the attribute, then picks
2 or 3), not an inline call evaluated first."""
SCORE = ['fn_8044EEAC', 'fn_8044F4D0', 'fn_8044F740', 'fn_8044F9E4', 'fn_80450448', 'fn_8045066C']
H0 = """static inline u32 RwCpp14ValueType(u32 value) {
    u32 type = 2;
    if (value >= 0xFF) {
        type = 3;
    }
    return type;
}
"""
NL = chr(10)
REPL = {
 "base": [(H0, H0)],
 "macro": [(H0, "#define RwCpp14ValueType(value) ((value) < 0xFF ? 2 : 3)" + NL)],
 "macro2": [(H0, "#define RwCpp14ValueType(value) ((value) >= 0xFF ? 3 : 2)" + NL)],
}
