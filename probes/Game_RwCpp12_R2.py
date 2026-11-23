"""Wave 40, RwCpp12 round 2: the inlined split-data creation in fn_8044BBFC (retail: split r31,
skin r30, stream r29, boneLimit r28, numBones r27, numRLE r26, numMeshes r25, size r24)."""
import itertools
SCORE = ["fn_8044BBFC"]
SIG = """static inline R2Skin *SkinSplitCreate(R2Skin *skin, u32 boneLimit, u32 numBones, u32 numMeshes,
                                      u32 numRLE) {
    u32 size;
"""
LOC = """    u32 numMeshes;
    u32 numRLE;
    u32 boneLimit;
    R2SkinSplit *split = &skin->split;
"""
T = "    if ((s32)numMeshes > 0) {\n"
REPL = {}
for b, n, m, r, z, lm, lo in itertools.product("us", "us", "us", "us", "us", "us", "ab"):
    sig = SIG.replace("u32 boneLimit", b + "32 boneLimit").replace("u32 numMeshes", m + "32 numMeshes").replace("u32 numRLE", r + "32 numRLE").replace("u32 numBones", n + "32 numBones").replace("u32 size", z + "32 size")
    loc = LOC.replace("u32 numMeshes", lm + "32 numMeshes")
    if lo == "b":
        loc = "    R2SkinSplit *split = &skin->split;\n" + loc.replace("    R2SkinSplit *split = &skin->split;\n", "")
    t = T if lm == "u" else "    if (numMeshes > 0) {\n"
    REPL[b + n + m + r + z + lm + lo] = [(SIG, sig), (LOC, loc), (T, t)]
