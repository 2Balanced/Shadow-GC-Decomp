"""Wave 40, RwCpp12 (RpSkin): saved registers of the geometry read callback fn_8044C754
(retail: geometry r31, maxWeights r30, numUsedBones r29, numVertices r28, skin r27, stream r26)."""
import itertools
SCORE = ["fn_8044C754"]
D = """    R2Geometry *geometry = (R2Geometry *)object;
    u32 maxWeights;
    u32 numUsedBones;
    s32 numVertices;
    R2Skin *skin;
    u32 header;
"""
L = {"w": "    u32 maxWeights;\n", "u": "    u32 numUsedBones;\n", "v": "    s32 numVertices;\n", "s": "    R2Skin *skin;\n"}
G = "    R2Geometry *geometry = (R2Geometry *)object;\n"
REPL = {}
for p in itertools.permutations("wuvs"):
    REPL["g" + "".join(p)] = [(D, G + "".join(L[c] for c in p) + "    u32 header;\n")]
REPL["v_u32"] = [(D, D.replace("s32 numVertices", "u32 numVertices"))]
A = """        numVertices = geometry->numVertices;
        numUsedBones = (header >> 8) & 0xFF;
        maxWeights = (header >> 16) & 0xFF;
"""
REPL["assign_rev"] = [(A, "        maxWeights = (header >> 16) & 0xFF;\n        numUsedBones = (header >> 8) & 0xFF;\n        numVertices = geometry->numVertices;\n")]
REPL["assign_uwv"] = [(A, "        numUsedBones = (header >> 8) & 0xFF;\n        maxWeights = (header >> 16) & 0xFF;\n        numVertices = geometry->numVertices;\n")]
