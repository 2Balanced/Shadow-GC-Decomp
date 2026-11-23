# Round 2 for src/RenderWare/RwStreamChunk.c: the shift-written byte swap's declarations.
import itertools
SYMBOLS = {"fn_8047D158": "void *fn_8047D158(void *mem, u32 size) {"}
SW = "(v >> 24) | ((v >> 8) & 0xFF00) | ((v & 0xFF00) << 8) | (v << 24)"
BODIES = {"fn_8047D158": {}}
decls = ["u32 *p = (u32 *)mem;", "u32 n;", "u32 v;"]
for i, perm in enumerate(itertools.permutations(decls)):
    BODIES["fn_8047D158"]["d%d" % i] = ("\n    " + "\n    ".join(perm) + "\n\n    for (n = size >> 2; n != 0; n--) {\n        v = *p;\n        *p = " + SW + ";\n        p++;\n    }\n    return mem;\n")
BODIES["fn_8047D158"]["ptrloop"] = ("\n    u32 *p;\n    u32 n;\n\n    for (p = (u32 *)mem, n = size >> 2; n != 0; n--, p++) {\n        u32 v = *p;\n\n        *p = " + SW + ";\n    }\n    return mem;\n")
BODIES["fn_8047D158"]["sizeloop"] = ("\n    u32 *p = (u32 *)mem;\n\n    size >>= 2;\n    while (size--) {\n        u32 v = *p;\n\n        *p++ = " + SW + ";\n    }\n    return mem;\n")
BODIES["fn_8047D158"]["memloop"] = ("\n    u32 n = size >> 2;\n    u32 *p = (u32 *)mem;\n\n    while (n--) {\n        u32 v = *p;\n\n        *p++ = " + SW + ";\n    }\n    return mem;\n")
BODIES["fn_8047D158"]["sw2"] = ("\n    u32 *p = (u32 *)mem;\n    u32 n;\n\n    for (n = size >> 2; n != 0; n--) {\n        u32 v = *p;\n\n        *p = (v << 24) | ((v & 0xFF00) << 8) | ((v >> 8) & 0xFF00) | (v >> 24);\n        p++;\n    }\n    return mem;\n")
