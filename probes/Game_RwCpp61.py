"""Wave 40 (r7): the temporary memory stack (fn_8049B8BC/fn_8049B92C): top in r5, size in r4."""

S1 = "void *fn_8049B8BC(RwUInt32 size) {"
S2 = "void *fn_8049B92C(RwUInt32 size) {"
SYMBOLS = {"fn_8049B8BC": S1, "fn_8049B92C": S2}
E = """        RW_ERROR(0x80000013);
        return NULL;
"""
BODIES = {
    "fn_8049B8BC": {
        "a": "    size = (size + 3) & ~3;\n    lbl_805EDF58[0] = lbl_805EDF58[0] - size;\n    if (lbl_805EDF58[0] < lbl_805EDF58[1]) {\n        lbl_805EDF58[0] = lbl_805EDF58[0] + size;\n" + E + "    }\n    return lbl_805EDF58[0];\n",
        "b": "    u8 *top;\n    size = (size + 3) & ~3;\n    top = lbl_805EDF58[0] - size;\n    lbl_805EDF58[0] = top;\n    if (top < lbl_805EDF58[1]) {\n        lbl_805EDF58[0] = top + size;\n" + E + "    }\n    return top;\n",
        "c": "    u8 *top;\n    size = (size + 3) & ~3;\n    lbl_805EDF58[0] = top = lbl_805EDF58[0] - size;\n    if (top < lbl_805EDF58[1]) {\n        lbl_805EDF58[0] += size;\n" + E + "    }\n    return top;\n",
        "d": "    size = (size + 3) & ~3;\n    if ((lbl_805EDF58[0] -= size) < lbl_805EDF58[1]) {\n        lbl_805EDF58[0] += size;\n" + E + "    }\n    return lbl_805EDF58[0];\n",
        "e": "    RwUInt32 bytes;\n    bytes = (size + 3) & ~3;\n    if ((lbl_805EDF58[0] -= bytes) < lbl_805EDF58[1]) {\n        lbl_805EDF58[0] += bytes;\n" + E + "    }\n    return lbl_805EDF58[0];\n",
    },
    "fn_8049B92C": {
        "a": "    size = (size + 3) & ~3;\n    lbl_805EDF58[1] = lbl_805EDF58[1] + size;\n    if (lbl_805EDF58[1] > lbl_805EDF58[0]) {\n        lbl_805EDF58[1] = lbl_805EDF58[1] - size;\n" + E + "    }\n    return lbl_805EDF58[1] - size;\n",
        "d": "    size = (size + 3) & ~3;\n    if ((lbl_805EDF58[1] += size) > lbl_805EDF58[0]) {\n        lbl_805EDF58[1] -= size;\n" + E + "    }\n    return lbl_805EDF58[1] - size;\n",
        "e": "    RwUInt32 bytes;\n    bytes = (size + 3) & ~3;\n    if ((lbl_805EDF58[1] += bytes) > lbl_805EDF58[0]) {\n        lbl_805EDF58[1] -= bytes;\n" + E + "    }\n    return lbl_805EDF58[1] - bytes;\n",
        "f": "    u8 *bottom;\n    size = (size + 3) & ~3;\n    bottom = lbl_805EDF58[1] + size;\n    lbl_805EDF58[1] = bottom;\n    if (bottom > lbl_805EDF58[0]) {\n        lbl_805EDF58[1] = bottom - size;\n" + E + "    }\n    return bottom - size;\n",
    },
}
