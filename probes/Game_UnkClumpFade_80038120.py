"""Wave 40: the two atomic callbacks that test the vertex colours (retail: atomic r31, root r30,
frame r29, geometry r28; the geometry is loaded before the frame)."""
SCORE = ["fn_80039288", "fn_80039178"]
D = "    RpGeometry *geometry = atomic->geometry;\n    RwFrame *frame = atomic->frame;\n"
S1 = 'extern "C" RpAtomic *fn_80039288(RpAtomic *atomic, RwFrame *root) {\n'
S2 = 'extern "C" RpAtomic *fn_80039178(RpAtomic *atomic, RwFrame *root) {\n'
P1 = "RpAtomic *fn_80039288(RpAtomic *atomic, RwFrame *root);"
P2 = "RpAtomic *fn_80039178(RpAtomic *atomic, RwFrame *root);"
def sig():
    return [(P1, P1.replace("RwFrame *root", "void *data")), (P2, P2.replace("RwFrame *root", "void *data")),
            (S1, S1.replace("RwFrame *root", "void *data")), (S2, S2.replace("RwFrame *root", "void *data"))]
def body(d, void=True):
    return (sig() if void else []) + [(D, d), (D, d)]
R = "    RwFrame *root = static_cast<RwFrame *>(data);\n"
REPL = {
  "void_root": body(R + D),
  "void_root_fg": body(R + "    RwFrame *frame = atomic->frame;\n    RpGeometry *geometry = atomic->geometry;\n"),
  "void_root_decl": body(R + "    RwFrame *frame;\n    RpGeometry *geometry;\n    geometry = atomic->geometry;\n    frame = atomic->frame;\n"),
  "void_gfr": body("    RpGeometry *geometry = atomic->geometry;\n    RwFrame *frame = atomic->frame;\n" + R),
  "void_frg": body("    RwFrame *frame = atomic->frame;\n" + R + "    RpGeometry *geometry = atomic->geometry;\n"),
  "fg": body("    RwFrame *frame = atomic->frame;\n    RpGeometry *geometry = atomic->geometry;\n", False),
  "decl": body("    RwFrame *frame;\n    RpGeometry *geometry;\n    geometry = atomic->geometry;\n    frame = atomic->frame;\n", False),
}
