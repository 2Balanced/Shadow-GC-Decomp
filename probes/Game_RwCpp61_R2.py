"""Wave 40 (r7): the temporary memory stack with the alignment written in each expression."""

S1 = "void *fn_8049B8BC(RwUInt32 size) {"
S2 = "void *fn_8049B92C(RwUInt32 size) {"
SYMBOLS = {"fn_8049B8BC": S1, "fn_8049B92C": S2}
NL = "\n"
E = "        RW_ERROR(0x80000013);" + NL + "        return NULL;" + NL
A = "((size + 3) & ~3)"
BODIES = {
    "fn_8049B8BC": {
        "g": "    if ((lbl_805EDF58[0] -= " + A + ") < lbl_805EDF58[1]) {" + NL + "        lbl_805EDF58[0] += " + A + ";" + NL + E + "    }" + NL + "    return lbl_805EDF58[0];" + NL,
        "h": "    lbl_805EDF58[0] -= " + A + ";" + NL + "    if (lbl_805EDF58[0] < lbl_805EDF58[1]) {" + NL + "        lbl_805EDF58[0] += " + A + ";" + NL + E + "    }" + NL + "    return lbl_805EDF58[0];" + NL,
    },
    "fn_8049B92C": {
        "g": "    if ((lbl_805EDF58[1] += " + A + ") > lbl_805EDF58[0]) {" + NL + "        lbl_805EDF58[1] -= " + A + ";" + NL + E + "    }" + NL + "    return lbl_805EDF58[1] - " + A + ";" + NL,
        "h": "    lbl_805EDF58[1] += " + A + ";" + NL + "    if (lbl_805EDF58[1] > lbl_805EDF58[0]) {" + NL + "        lbl_805EDF58[1] -= " + A + ";" + NL + E + "    }" + NL + "    return lbl_805EDF58[1] - " + A + ";" + NL,
    },
}
