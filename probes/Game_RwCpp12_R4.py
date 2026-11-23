"""Wave 40, RwCpp12 round 4: fn_8044BDF0's inlined allocation: the size sum and the array
addresses (retail shifts the counts again after the memset instead of keeping them)."""
import itertools
SCORE = ["fn_8044BDF0"]
S0 = """    size = numBones * sizeof(R2Matrix) + numUsedBones + 15 +
           numVertices * (sizeof(u32) + sizeof(R2MatrixWeights));
"""
P0 = """    skin->matrices = (R2Matrix *)(((u32)skin->usedBones + (numUsedBones + 15)) & ~15);
    skin->indices = (u32 *)&skin->matrices[numBones];
    skin->weights = (R2MatrixWeights *)&skin->indices[numVertices];
"""
SZ = {
 "a": "    size = numBones * 64 + numUsedBones + 15 + numVertices * 20;\n",
 "b": "    size = numVertices * 20 + (numBones * 64 + numUsedBones + 15);\n",
 "c": "    size = (numBones << 6) + numUsedBones + 15 + numVertices * 20;\n",
 "d": "    size = numBones * 64 + numUsedBones + 15;\n    size += numVertices * 20;\n",
 "e": "    size = (numBones << 6) + numUsedBones + 15;\n    size += numVertices * 20;\n",
 "f": "    size = numBones * 64 + numUsedBones + 15 + numVertices * 4 + numVertices * 16;\n",
 "g": "    size = sizeof(R2Matrix) * numBones + numUsedBones + 15 + (sizeof(u32) + sizeof(R2MatrixWeights)) * numVertices;\n",
}
PT = {
 "a": P0,
 "b": """    skin->matrices = (R2Matrix *)(((u32)skin->usedBones + (numUsedBones + 15)) & ~15);
    skin->indices = (u32 *)((u8 *)skin->matrices + (numBones << 6));
    skin->weights = (R2MatrixWeights *)((u8 *)skin->indices + (numVertices << 2));
""",
 "c": """    skin->matrices = (R2Matrix *)(((u32)skin->usedBones + (numUsedBones + 15)) & ~15);
    skin->indices = (u32 *)((u32)skin->matrices + numBones * 64);
    skin->weights = (R2MatrixWeights *)((u32)skin->indices + numVertices * 4);
""",
 "d": """    skin->matrices = (R2Matrix *)(((u32)skin->usedBones + numUsedBones + 15) & ~15);
    skin->indices = (u32 *)((u8 *)skin->matrices + (numBones << 6));
    skin->weights = (R2MatrixWeights *)((u8 *)skin->indices + (numVertices << 2));
""",
}
REPL = {}
for a, b in itertools.product(SZ, PT):
    REPL["s%s_p%s" % (a, b)] = [(S0, SZ[a]), (P0, PT[b])]
