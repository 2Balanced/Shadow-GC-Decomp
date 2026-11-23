"""Wave 40, RwCpp13 round 2: declaration order of the stream write fn_8044E89C (retail: skin r31,
numVertices r30, maxWeights r29; header at 8, format at 12) and read fn_8044EAD0 (retail: format 8,
header 12, length 16, version 20, error 24; numVertices r31, skin r30)."""
import itertools
SCORE = ["fn_8044E89C", "fn_8044EAD0"]
W0 = """    s32 numVertices;
    R2Skin *skin;
    u32 maxWeights;
    u32 header;
    s32 format = 6;
"""
R0 = """    s32 format;
    u32 header;
    u32 length;
    u32 version;
    R2Error error;
    R2Skin *skin;
    s32 numVertices;
"""
WD = {"n": "    s32 numVertices;\n", "s": "    R2Skin *skin;\n", "m": "    u32 maxWeights;\n"}
REPL = {}
for p in itertools.permutations("nsm"):
    REPL["w_" + "".join(p)] = [(W0, "".join(WD[c] for c in p) + "    s32 format = 6;\n    u32 header;\n")]
REPL["r_rev"] = [(R0, "    s32 numVertices;\n    R2Skin *skin;\n    R2Error error;\n    u32 version;\n    u32 length;\n    u32 header;\n    s32 format;\n")]
REPL["r_rev2"] = [(R0, "    R2Error error;\n    s32 numVertices;\n    R2Skin *skin;\n    u32 version;\n    u32 length;\n    u32 header;\n    s32 format;\n")]
REPL["r_rev3"] = [(R0, "    s32 numVertices;\n    R2Skin *skin;\n    u32 version;\n    u32 length;\n    u32 header;\n    s32 format;\n    R2Error error;\n")]
REPL["r_rev4"] = [(R0, "    R2Skin *skin;\n    s32 numVertices;\n    R2Error error;\n    u32 version;\n    u32 length;\n    u32 header;\n    s32 format;\n")]
