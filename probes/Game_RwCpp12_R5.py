"""Wave 40, RwCpp12 round 5: fn_8044C8C0's used-bone loop: the shift as a variable of its own and
the zero stored to numUsedBones (retail copies the zeroed counters: li r8,0 / stw r8; li r7,0 /
mr r11,r7)."""
import itertools
SCORE = ["fn_8044C8C0"]
D = """        const u32 *weight;
        const R2MatrixWeights *weights;
        u32 numUsedBones;
        const u32 *indices;
"""
Z = """        skin->numUsedBones = 0;
        for (i = 0; i < numVertices; i++) {
            weight = &weights->w[0];
            for (j = 0; j < skin->maxWeights; j++) {
                if (*weight != 0) {
                    bone = (u8)(*indices >> (j * 8));
"""
E = """                weight++;
            }
            weights++;
            indices++;
"""
ZV = {
 "z0": "        skin->numUsedBones = 0;\n        for (i = 0; i < numVertices; i++) {\n",
 "z1": "        i = 0;\n        skin->numUsedBones = 0;\n        for (; i < numVertices; i++) {\n",
 "z2": "        i = 0;\n        skin->numUsedBones = i;\n        for (; i < numVertices; i++) {\n",
 "z3": "        for (skin->numUsedBones = 0, i = 0; i < numVertices; i++) {\n",
 "z4": "        for (i = 0, skin->numUsedBones = 0; i < numVertices; i++) {\n",
}
SV = {
 "s0": ("            weight = &weights->w[0];\n            for (j = 0; j < skin->maxWeights; j++) {\n", "j * 8", "", ""),
 "s1": ("            weight = &weights->w[0];\n            for (j = 0, shift = 0; j < skin->maxWeights; j++) {\n", "shift", "                shift += 8;\n", "        u32 shift;\n"),
 "s2": ("            j = 0;\n            weight = &weights->w[0];\n            shift = j;\n            for (; j < skin->maxWeights; j++) {\n", "shift", "                shift += 8;\n", "        u32 shift;\n"),
 "s3": ("            weight = &weights->w[0];\n            for (j = 0, shift = j; j < skin->maxWeights; j++) {\n", "shift", "                shift += 8;\n", "        u32 shift;\n"),
 "s4": ("            j = 0;\n            weight = &weights->w[0];\n            shift = 0;\n            for (; j < skin->maxWeights; j++) {\n", "shift", "                shift += 8;\n", "        u32 shift;\n"),
 "s5": ("            weight = &weights->w[0];\n            shift = 0;\n            for (j = 0; j < skin->maxWeights; j++) {\n", "shift", "                shift += 8;\n", "        s32 shift;\n"),
}
REPL = {}
for z, sv in itertools.product(ZV, SV):
    h, e, inc, decl = SV[sv]
    newz = ZV[z] + h + "                if (*weight != 0) {\n                    bone = (u8)(*indices >> (%s));\n" % e
    newe = "                weight++;\n" + inc + "            }\n            weights++;\n            indices++;\n"
    newd = "        const u32 *weight;\n" + decl + "        const R2MatrixWeights *weights;\n        u32 numUsedBones;\n        const u32 *indices;\n"
    REPL[z + sv] = [(D, newd), (Z, newz), (E, newe)]
