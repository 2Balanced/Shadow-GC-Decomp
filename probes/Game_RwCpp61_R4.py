"""Wave 40 (r7): the temporary memory stack with the alignment in an inline function."""
NL = "\n"
OLD1 = "    size = (size + 3) & ~3;" + NL + "    lbl_805EDF58[0] -= size;"
OLD2 = "    size = (size + 3) & ~3;" + NL + "    lbl_805EDF58[1] += size;"
FN = "/* Temporary memory from the top of the stack. */"
H = "static inline RwUInt32 RwDlAlign4(RwUInt32 size) {" + NL + "    return (size + 3) & ~3;" + NL + "}" + NL + NL
REPL = {
    "inl": [(FN, H + FN), (OLD1, "    size = RwDlAlign4(size);" + NL + "    lbl_805EDF58[0] -= size;"),
            (OLD2, "    size = RwDlAlign4(size);" + NL + "    lbl_805EDF58[1] += size;")],
    "inl2": [(FN, H.replace("RwUInt32 size) {" + NL + "    return", "RwUInt32 size) {" + NL + "    size = (size + 3) & ~3;" + NL + "    return size;" + NL + "    return").replace("    return (size + 3) & ~3;" + NL, "") + FN),
             (OLD1, "    size = RwDlAlign4(size);" + NL + "    lbl_805EDF58[0] -= size;"),
             (OLD2, "    size = RwDlAlign4(size);" + NL + "    lbl_805EDF58[1] += size;")],
}
SCORE = ["fn_8049B8BC", "fn_8049B92C"]
