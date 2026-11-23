"""Wave 40, RwCpp13 round 3: the header word of fn_8044E89C (retail loads numUsedBones, then
maxWeights into its saved register, then numBones)."""
SCORE = ["fn_8044E89C"]
H0 = """    maxWeights = skin->maxWeights;
    header = ((skin->numUsedBones & 0xFF) << 8) | ((maxWeights & 0xFF) << 16) |
             (skin->numBones & 0xFF);
"""
REPL = {
 "after": [(H0, "    header = ((skin->numUsedBones & 0xFF) << 8) | ((skin->maxWeights & 0xFF) << 16) |\n             (skin->numBones & 0xFF);\n    maxWeights = skin->maxWeights;\n")],
 "inexpr": [(H0, "    header = ((skin->numUsedBones & 0xFF) << 8) | (((maxWeights = skin->maxWeights) & 0xFF) << 16) |\n             (skin->numBones & 0xFF);\n")],
 "order2": [(H0, "    maxWeights = skin->maxWeights;\n    header = ((maxWeights & 0xFF) << 16) | ((skin->numUsedBones & 0xFF) << 8) |\n             (skin->numBones & 0xFF);\n")],
 "order3": [(H0, "    maxWeights = skin->maxWeights;\n    header = (skin->numBones & 0xFF) | ((skin->numUsedBones & 0xFF) << 8) |\n             ((maxWeights & 0xFF) << 16);\n")],
 "order4": [(H0, "    maxWeights = skin->maxWeights;\n    header = ((skin->numUsedBones & 0xFF) << 8) | (skin->numBones & 0xFF) |\n             ((maxWeights & 0xFF) << 16);\n")],
 "u8casts": [(H0, "    maxWeights = skin->maxWeights;\n    header = ((u8)skin->numUsedBones << 8) | ((u8)maxWeights << 16) | (u8)skin->numBones;\n")],
 "after_o3": [(H0, "    header = (skin->numBones & 0xFF) | ((skin->numUsedBones & 0xFF) << 8) |\n             ((skin->maxWeights & 0xFF) << 16);\n    maxWeights = skin->maxWeights;\n")],
 "after_o2": [(H0, "    header = ((skin->maxWeights & 0xFF) << 16) | ((skin->numUsedBones & 0xFF) << 8) |\n             (skin->numBones & 0xFF);\n    maxWeights = skin->maxWeights;\n")],
}
