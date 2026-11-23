"""Wave 40 (r7): the temporary memory stack as integers or as a struct."""
NL = "\n"
DECL = "extern u8 *lbl_805EDF58[2];           /* the temporary memory: [0] top, [1] bottom */"
OLD1 = """    size = (size + 3) & ~3;
    lbl_805EDF58[0] -= size;
    if (lbl_805EDF58[0] < lbl_805EDF58[1]) {
        lbl_805EDF58[0] += size;
        RW_ERROR(0x80000013);
        return NULL;
    }
    return lbl_805EDF58[0];"""
OLD2 = """    size = (size + 3) & ~3;
    lbl_805EDF58[1] += size;
    if (lbl_805EDF58[1] > lbl_805EDF58[0]) {
        lbl_805EDF58[1] -= size;
        RW_ERROR(0x80000013);
        return NULL;
    }
    return lbl_805EDF58[1] - size;"""
OLD1b = OLD1.replace("    size = (size + 3) & ~3;\n    lbl_805EDF58[0] -= size;", "    RwUInt32 bytes = (size + 3) & ~3;\n    lbl_805EDF58[0] -= bytes;").replace("        lbl_805EDF58[0] += size;", "        lbl_805EDF58[0] += bytes;")
def body1(T, B, cast):
    return OLD1.replace("lbl_805EDF58[0]", T).replace("lbl_805EDF58[1]", B).replace("    return " + T + ";", "    return " + cast + T + ";")
def body2(T, B, cast):
    return OLD2.replace("lbl_805EDF58[0]", T).replace("lbl_805EDF58[1]", B).replace("    return " + B + " - size;", "    return " + cast + "(" + B + " - size);")
REPL = {}
for name, decl, T, B, cast in (
        ("int", "extern RwUInt32 lbl_805EDF58[2];", "lbl_805EDF58[0]", "lbl_805EDF58[1]", "(void *)"),
        ("struct", "typedef struct RwDlStack { u8 *top; u8 *bottom; } RwDlStack;" + NL + "extern RwDlStack lbl_805EDF58;", "lbl_805EDF58.top", "lbl_805EDF58.bottom", ""),
        ("structi", "typedef struct RwDlStack { RwUInt32 top; RwUInt32 bottom; } RwDlStack;" + NL + "extern RwDlStack lbl_805EDF58;", "lbl_805EDF58.top", "lbl_805EDF58.bottom", "(void *)"),
        ("structs", "typedef struct RwDlStack { RwInt32 top; RwInt32 bottom; } RwDlStack;" + NL + "extern RwDlStack lbl_805EDF58;", "lbl_805EDF58.top", "lbl_805EDF58.bottom", "(void *)")):
    REPL[name] = [(DECL, decl), (OLD1, body1(T, B, cast)), (OLD2, body2(T, B, cast))]
SCORE = ["fn_8049B8BC", "fn_8049B92C"]
