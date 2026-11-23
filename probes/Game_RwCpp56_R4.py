"""Wave 40 (r7): fn_80493888: the final OR as separate statements."""

OLDOR = "        raster->cFormat = (RwInt32)((format & 0x9000) | fn_8049375C(image)) >> 8;"
V = {
    "k": "        {\n            RwInt32 f;\n            f = fn_8049375C(image);\n            f |= flags & 0x9000;\n            raster->cFormat = f >> 8;\n        }",
    "l": "        {\n            RwInt32 f;\n            f = fn_8049375C(image);\n            f |= format & 0x9000;\n            raster->cFormat = f >> 8;\n        }",
    "m": "        flags &= 0x9000;\n        raster->cFormat = (fn_8049375C(image) | flags) >> 8;",
    "n": "        {\n            RwInt32 mip = flags & 0x9000;\n            raster->cFormat = (fn_8049375C(image) | mip) >> 8;\n        }",
    "o": "        {\n            RwInt32 mip = flags & 0x9000;\n            raster->cFormat = (mip | fn_8049375C(image)) >> 8;\n        }",
    "p": "        {\n            RwInt32 f = fn_8049375C(image);\n            f |= flags & 0x9000;\n            raster->cFormat = (RwUInt8)(f >> 8);\n        }",
}
REPL = {k: [(OLDOR, v)] for k, v in V.items()}
SCORE = ["fn_80493888"]
