"""Wave 40 (r7): fn_80495428 (mip level size): the clamp of the level's width and height."""
import itertools

SYM = "fn_80495428"
SYMBOLS = {SYM: "RwInt32 fn_80495428(RwDlRaster *raster, RwUInt8 level) {"}

TAIL = """    switch (raster->depth) {
    case 4:
        size = (((width + 7) & ~7) * ((height + 7) & ~7)) >> 1;
        break;
    case 8:
        size = ((width + 7) & ~7) * ((height + 3) & ~3);
        break;
    case 16:
        size = (((width + 3) & ~3) * ((height + 3) & ~3)) << 1;
        break;
    case 32:
        size = (((width + 3) & ~3) * ((height + 3) & ~3)) << 2;
        break;
    case 24:
    default:
        RW_ERROR(0x8000000C);
        return 0;
    }
    return (size + 31) & ~31;
"""
CLAMPS = {
    "tern_u": ("    RwUInt32 w;\n    RwUInt32 h;\n", "        w = parent->width >> level;\n        h = parent->height >> level;\n        width = w ? w : 1;\n        height = h ? h : 1;\n"),
    "tern_s": ("    RwInt32 w;\n    RwInt32 h;\n", "        w = parent->width >> level;\n        h = parent->height >> level;\n        width = w ? w : 1;\n        height = h ? h : 1;\n"),
    "tern_us": ("    RwInt32 w;\n    RwUInt32 h;\n", "        w = parent->width >> level;\n        h = parent->height >> level;\n        width = w ? w : 1;\n        height = h ? h : 1;\n"),
    "self": ("", "        width = parent->width >> level;\n        height = parent->height >> level;\n        width = width ? width : 1;\n        height = height ? height : 1;\n"),
    "inline": ("", "        width = (parent->width >> level) ? (parent->width >> level) : 1;\n        height = (parent->height >> level) ? (parent->height >> level) : 1;\n"),
    "if1": ("    RwUInt32 w;\n    RwUInt32 h;\n", "        w = parent->width >> level;\n        h = parent->height >> level;\n        width = 1;\n        if (w) width = w;\n        height = 1;\n        if (h) height = h;\n"),
}
BODIES = {SYM: {}}
for name, (extra, clamp) in CLAMPS.items():
    for tname, decl in (("u", "    RwUInt32 width;\n    RwUInt32 height;\n    RwUInt32 size;\n"),
                        ("s", "    RwInt32 width;\n    RwInt32 height;\n    RwUInt32 size;\n"),
                        ("u2", "    RwUInt32 height;\n    RwUInt32 width;\n    RwUInt32 size;\n")):
        BODIES[SYM][name + "_" + tname] = ("    RwDlRaster *parent = raster->parent;\n" + decl + extra +
            "    if (raster->privateFlags & 6) {\n        width = parent->width;\n        height = parent->height;\n    } else {\n" + clamp + "    }\n" + TAIL)
