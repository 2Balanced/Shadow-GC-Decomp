"""Wave 40 (r7): source shapes tried for RwIm2DRenderLine/Triangle (fn_80490CBC, fn_80490AB8)."""

SYM = "fn_80490CBC"
SYMBOLS = {SYM: "s32 fn_80490CBC(RwDlIm2DVertex *vertices, s32 numVertices, s32 vert1, s32 vert2) {"}

LINE_TEX = """
        GXPosition3f32(a->x, a->y, a->z);
        GXColor4u8(a->color.red, a->color.green, a->color.blue, a->color.alpha);
        GXTexCoord2f32(a->u, a->v);
        GXPosition3f32(b->x, b->y, b->z);
        GXColor4u8(b->color.red, b->color.green, b->color.blue, b->color.alpha);
        GXTexCoord2f32(b->u, b->v);
"""
LINE_NOTEX = """
        GXPosition3f32(a->x, a->y, a->z);
        GXColor4u8(a->color.red, a->color.green, a->color.blue, a->color.alpha);
        GXPosition3f32(b->x, b->y, b->z);
        GXColor4u8(b->color.red, b->color.green, b->color.blue, b->color.alpha);
"""

def body(decl):
    return decl + """
    fn_8049043C();
    GXBegin(0xA8, 0, 2);
    if (*lbl_805F1664 != 0) {""" + LINE_TEX + """    } else {""" + LINE_NOTEX + """    }
    fn_80490724();
    return 1;
"""

BODIES = {SYM: {
    "ab": body("    RwDlIm2DVertex *a = &vertices[vert1];\n    RwDlIm2DVertex *b = &vertices[vert2];"),
    "ba": body("    RwDlIm2DVertex *b = &vertices[vert2];\n    RwDlIm2DVertex *a = &vertices[vert1];"),
    "ab_sep": body("    RwDlIm2DVertex *a;\n    RwDlIm2DVertex *b;\n    a = &vertices[vert1];\n    b = &vertices[vert2];"),
    "ab_sep2": body("    RwDlIm2DVertex *a;\n    RwDlIm2DVertex *b;\n    b = &vertices[vert2];\n    a = &vertices[vert1];"),
    "ab_plus": body("    RwDlIm2DVertex *a = vertices + vert1;\n    RwDlIm2DVertex *b = vertices + vert2;"),
}}

HDR = {
    "fifo_ptr": {"Game/rw/r7.h": [
        ("    GXWGFifo.f32 = x;\n    GXWGFifo.f32 = y;\n    GXWGFifo.f32 = z;", "    *(volatile f32 *)0xCC008000 = x;\n    *(volatile f32 *)0xCC008000 = y;\n    *(volatile f32 *)0xCC008000 = z;"),
        ("    GXWGFifo.u8 = r;\n    GXWGFifo.u8 = g;\n    GXWGFifo.u8 = b;\n    GXWGFifo.u8 = a;", "    *(volatile u8 *)0xCC008000 = r;\n    *(volatile u8 *)0xCC008000 = g;\n    *(volatile u8 *)0xCC008000 = b;\n    *(volatile u8 *)0xCC008000 = a;"),
        ("    GXWGFifo.f32 = s;\n    GXWGFifo.f32 = t;", "    *(volatile f32 *)0xCC008000 = s;\n    *(volatile f32 *)0xCC008000 = t;"),
    ]},
    "const_args": {"Game/rw/r7.h": [
        ("GXPosition3f32(f32 x, f32 y, f32 z)", "GXPosition3f32(const f32 x, const f32 y, const f32 z)"),
        ("GXColor4u8(u8 r, u8 g, u8 b, u8 a)", "GXColor4u8(const u8 r, const u8 g, const u8 b, const u8 a)"),
        ("GXTexCoord2f32(f32 s, f32 t)", "GXTexCoord2f32(const f32 s, const f32 t)"),
    ]},
}
SCORE = [SYM]
