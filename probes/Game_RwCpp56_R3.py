"""Wave 40 (r7): fn_80493888: operand order of the final OR (retail: call result | flags)."""

OLDOR = "        raster->cFormat = (RwInt32)((format & 0x9000) | fn_8049375C(image)) >> 8;"
V = {
    "a": "        format = fn_8049375C(image) | (format & 0x9000);\n        raster->cFormat = (RwInt32)format >> 8;",
    "b": "        {\n            RwInt32 f = fn_8049375C(image);\n            raster->cFormat = (f | (format & 0x9000)) >> 8;\n        }",
    "c": "        {\n            RwInt32 f = fn_8049375C(image);\n            raster->cFormat = ((format & 0x9000) | f) >> 8;\n        }",
    "d": "        raster->cFormat = (RwInt32)(fn_8049375C(image) | (flags & 0x9000)) >> 8;",
    "e": "        raster->cFormat = (fn_8049375C(image) | (flags & 0x9000)) >> 8;",
    "f": "        raster->cFormat = ((flags & 0x9000) | fn_8049375C(image)) >> 8;",
    "g": "        flags = (flags & 0x9000) | fn_8049375C(image);\n        raster->cFormat = flags >> 8;",
    "h": "        flags &= 0x9000;\n        flags |= fn_8049375C(image);\n        raster->cFormat = flags >> 8;",
    "i": "        format = (format & 0x9000) | fn_8049375C(image);\n        raster->cFormat = (RwInt32)format >> 8;",
    "j": "        format &= 0x9000;\n        format |= fn_8049375C(image);\n        raster->cFormat = (RwInt32)format >> 8;",
}
REPL = {k: [(OLDOR, v)] for k, v in V.items()}
SCORE = ["fn_80493888"]
