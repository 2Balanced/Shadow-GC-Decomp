"""Wave 40, RwCpp13 round 10: fn_8044E408's packing loops (retail zeroes the counter and its
strength-reduced offset with two li, keeps packed in r5 under the counter in r6, and addresses the
weight to bump as packed - (maxWeights - k))."""
SCORE = ["fn_8044E408"]
D0 = """    u32 i;
    u32 j;
    u32 k;
    u8 *packed;
    u32 total;
"""
A0 = "                    packed[k - skin->maxWeights]++;\n"
L1 = """        for (i = 0; i < numVertices; i++) {
            for (k = 0; k < skin->maxWeights; k++) {
                *packed++ = skin->indices[i] >> (k * 8);
            }
        }
"""
L0 = """            total = 0;
            for (k = 0; k < skin->maxWeights; k++) {
"""
ADJ = (A0, "                    *(packed - (skin->maxWeights - k)) += 1;\n")
ADJ2 = (A0, "                    (packed - (skin->maxWeights - k))[0]++;\n")
REPL = {
 "base": [(D0, D0)],
 "adj": [ADJ],
 "adj2": [ADJ2],
 "adj_dp": [ADJ, (D0, "    u32 i;\n    u32 j;\n    u32 k;\n    u32 total;\n    u8 *packed;\n")],
 "adj_dk": [ADJ, (D0, "    u32 i;\n    u32 j;\n    u8 *packed;\n    u32 total;\n    u32 k;\n")],
 "adj_dk2": [ADJ, (D0, "    u8 *packed;\n    u32 total;\n    u32 i;\n    u32 j;\n    u32 k;\n")],
 "adj_sep": [ADJ, (D0, D0 + "    u32 n;\n"), (L1, L1.replace("(k = 0; k < skin->maxWeights; k++)", "(n = 0; n < skin->maxWeights; n++)").replace("(k * 8)", "(n * 8)"))],
 "adj_tot": [ADJ, (L0, "            for (k = 0, total = 0; k < skin->maxWeights; k++) {\n")],
 "adj_s32": [ADJ, (D0, D0.replace("u32 k;", "s32 k;"))],
}
