"""Wave 40, RwCpp12 round 6: the size sum of fn_8044BDF0's inlined allocation (retail:
((numBones << 6) + numUsedBones) + 15, then numVertices * 20 + that)."""
SCORE = ["fn_8044BDF0"]
S0 = "    size = numBones * 64 + numUsedBones + 15;\n    size += numVertices * 20;\n"
SIG = """static inline s32 SkinAllocate(R2Skin *skin, u32 numBones, u32 numUsedBones, u32 numVertices,"""
REPL = {
 "base": [(S0, S0)],
 "one": [(S0, "    size = numVertices * 20 + (numBones * 64 + numUsedBones + 15);\n")],
 "two_rev": [(S0, "    size = numBones * 64 + numUsedBones + 15;\n    size = numVertices * 20 + size;\n")],
 "s_bones": [(SIG, SIG.replace("u32 numBones", "s32 numBones"))],
 "s_used": [(SIG, SIG.replace("u32 numUsedBones", "s32 numUsedBones"))],
 "s_all": [(SIG, SIG.replace("u32", "s32"))],
 "s_all_one": [(SIG, SIG.replace("u32", "s32")), (S0, "    size = numVertices * 20 + (numBones * 64 + numUsedBones + 15);\n")],
 "s_all_flat": [(SIG, SIG.replace("u32", "s32")), (S0, "    size = numBones * 64 + numUsedBones + 15 + numVertices * 20;\n")],
 "sizeofs": [(S0, "    size = (sizeof(R2Matrix) * numBones + numUsedBones) + 15;\n    size += numVertices * 20;\n")],
 "paren": [(S0, "    size = (numBones * 64 + numUsedBones) + 15 + numVertices * 20;\n")],
 "three": [(S0, "    size = numBones * 64 + numUsedBones;\n    size += 15;\n    size += numVertices * 20;\n")],
 "three_s": [(SIG, SIG.replace("u32", "s32")), (S0, "    size = numBones * 64 + numUsedBones;\n    size += 15;\n    size += numVertices * 20;\n")],
 "vfirst": [(S0, "    size = numVertices * 20 + numBones * 64 + numUsedBones + 15;\n")],
}
