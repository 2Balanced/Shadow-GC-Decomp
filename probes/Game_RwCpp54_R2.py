"""Wave 40 (r7): declaration orders tried for RwIm2DRenderTriangle (fn_80490AB8)."""
import itertools

SYM = "fn_80490AB8"
SYMBOLS = {SYM: "s32 fn_80490AB8(RwDlIm2DVertex *vertices, s32 numVertices, s32 vert1, s32 vert2, s32 vert3) {"}

def emit(p, tex):
    s = """
        GXPosition3f32(%s->x, %s->y, %s->z);
        GXColor4u8(%s->color.red, %s->color.green, %s->color.blue, %s->color.alpha);""" % ((p,) * 7)
    if tex:
        s += "\n        GXTexCoord2f32(%s->u, %s->v);" % (p, p)
    return s

def body(decl):
    return decl + """
    fn_8049043C();
    GXBegin(0x90, 0, 3);
    if (*lbl_805F1664 != 0) {""" + emit("a", 1) + emit("b", 1) + emit("c", 1) + """
    } else {""" + emit("a", 0) + emit("b", 0) + emit("c", 0) + """
    }
    fn_80490724();
    return 1;
"""

IDX = {"a": "vert1", "b": "vert2", "c": "vert3"}
BODIES = {SYM: {}}
for order in itertools.permutations("abc"):
    decl = "\n".join("    RwDlIm2DVertex *%s = &vertices[%s];" % (v, IDX[v]) for v in order)
    BODIES[SYM]["".join(order)] = body(decl)
    decl2 = "    RwDlIm2DVertex *a;\n    RwDlIm2DVertex *b;\n    RwDlIm2DVertex *c;\n" + "\n".join("    %s = &vertices[%s];" % (v, IDX[v]) for v in order)
    BODIES[SYM]["abc_assign_" + "".join(order)] = body(decl2)
