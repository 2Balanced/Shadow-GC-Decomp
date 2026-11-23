"""Wave 40, RwCpp13 round 11: fn_8044E408's index byte swap temporaries (retail: the cleared word
in r11, the shifted copy in r27) and the first packing loop's zeroing (li total, li k, li k*4)."""
SCORE = ["fn_8044E408"]
X0 = """                    u32 index;

                    ((f32 *)skin->weights + i * 4 + j)[0] = ((f32 *)skin->weights + i * 4 + j)[k];
                    ((f32 *)skin->weights + i * 4 + j)[k] = swap;
                    index = skin->indices[i];
                    skin->indices[i] &= ~(0xFF << (j * 8));
                    skin->indices[i] |= ((index >> ((j + k) * 8)) & 0xFF) << (j * 8);
                    skin->indices[i] &= ~(0xFF << ((j + k) * 8));
                    skin->indices[i] |= ((index >> (j * 8)) & 0xFF) << ((j + k) * 8);
"""
FW = """                    ((f32 *)skin->weights + i * 4 + j)[0] = ((f32 *)skin->weights + i * 4 + j)[k];
                    ((f32 *)skin->weights + i * 4 + j)[k] = swap;
"""
L0 = """            total = 0;
            for (k = 0; k < skin->maxWeights; k++) {
                *packed = (u8)(128.0f * ((f32 *)skin->weights + i * 4)[k]);
                total += *packed++;
            }
"""
REPL = {
 "base": [(X0, X0)],
 "ab": [(X0, "                    u32 a;\n                    u32 b;\n\n" + FW + "                    a = (skin->indices[i] >> ((j + k) * 8)) & 0xFF;\n                    b = (skin->indices[i] >> (j * 8)) & 0xFF;\n                    skin->indices[i] &= ~(0xFF << (j * 8));\n                    skin->indices[i] |= a << (j * 8);\n                    skin->indices[i] &= ~(0xFF << ((j + k) * 8));\n                    skin->indices[i] |= b << ((j + k) * 8);\n")],
 "ba": [(X0, "                    u32 a;\n                    u32 b;\n\n" + FW + "                    b = (skin->indices[i] >> (j * 8)) & 0xFF;\n                    a = (skin->indices[i] >> ((j + k) * 8)) & 0xFF;\n                    skin->indices[i] &= ~(0xFF << (j * 8));\n                    skin->indices[i] |= a << (j * 8);\n                    skin->indices[i] &= ~(0xFF << ((j + k) * 8));\n                    skin->indices[i] |= b << ((j + k) * 8);\n")],
 "shiftvar": [(X0, X0.replace("                    u32 index;\n", "                    u32 index;\n                    u32 shift;\n").replace("                    index = skin->indices[i];\n", "                    index = skin->indices[i];\n                    shift = (j + k) * 8;\n").replace("((j + k) * 8)", "shift"))],
 "t_sep": [(L0, "            total = 0;\n            for (m = 0; m < skin->maxWeights; m++) {\n                *packed = (u8)(128.0f * ((f32 *)skin->weights + i * 4)[m]);\n                total += *packed++;\n            }\n"),
           ("    u32 k;\n", "    u32 k;\n    u32 m;\n")],
 "t_sep2": [(L0, "            total = 0;\n            for (m = 0; m < skin->maxWeights; m++) {\n                *packed = (u8)(128.0f * ((f32 *)skin->weights + i * 4)[m]);\n                total += *packed++;\n            }\n"),
           ("    u32 k;\n", "    u32 m;\n    u32 k;\n")],
}
