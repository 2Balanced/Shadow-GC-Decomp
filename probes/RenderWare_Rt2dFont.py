"""Wave 41: fn_80472A60's texture loop (counter and texture pointer swap r29/r30)."""
SCORE = ["fn_80472A60"]
D = "    char name[32];\n    s32 numTextures;\n    Rt2dFontGlyph *glyph;\n    s32 i;\n    Rt2dFontGlyph *glyphs;\n"
L = "    for (i = 0; i < numTextures; i++) {\n        if (fn_80480D4C(stream, name, 32) != 32) {\n            return NULL;\n        }\n        if ((font->texture[i] = fn_8048D130(name, NULL)) == NULL) {\n            return NULL;\n        }\n    }\n"
import itertools
parts = {"n": "    char name[32];\n", "t": "    s32 numTextures;\n", "g": "    Rt2dFontGlyph *glyph;\n", "i": "    s32 i;\n", "s": "    Rt2dFontGlyph *glyphs;\n"}
REPL = {}
for o in ["ntgis", "ntigs", "ntisg", "ingts", "tngis", "gints", "ngist", "nsgit"]:
    REPL["o_" + o] = [(D, "".join(parts[c] for c in o))]
REPL["ptr_loop"] = [(L, "    {\n        RwTexture **texture = font->texture;\n\n        for (i = 0; i < numTextures; i++) {\n            if (fn_80480D4C(stream, name, 32) != 32) {\n                return NULL;\n            }\n            if ((*texture++ = fn_8048D130(name, NULL)) == NULL) {\n                return NULL;\n            }\n        }\n    }\n")]
REPL["sep_counter"] = [(D, D + "    s32 t;\n"), (L, L.replace("(i = 0; i < numTextures; i++)", "(t = 0; t < numTextures; t++)").replace("font->texture[i]", "font->texture[t]"))]
