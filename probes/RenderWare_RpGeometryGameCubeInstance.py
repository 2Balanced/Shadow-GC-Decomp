"""Wave 41: fn_8046C0D4. Retail keeps the header in a register of its own (r21, a copy of the
instance data made after the memset), computes the lists from the instance data, and walks the
texture-coordinate loops with the counter in r3."""
SCORE = ["fn_8046C0D4"]
A_OLD = ("    instSize = size;\n    instData = (u8 *)(entry + 1);\n    memset(instData, 0, size);\n"
         "    header = (RwGameCubeInstHeader *)instData;\n")
A_NEW = ("    instData = (u8 *)(entry + 1);\n    instSize = size;\n    memset(instData, 0, size);\n"
         "    header = (RwGameCubeInstHeader *)instData;\n")
L_OLD = "    lists = (RwGameCubeDisplayList *)((u8 *)header + headerSize);\n    data = (u8 *)(((u32)lists + listsSize + 31) & ~31);\n"
L1 = "    lists = (RwGameCubeDisplayList *)(instData + headerSize);\n    data = (u8 *)(((u32)lists + listsSize + 31) & ~31);\n"
L2 = "    lists = (RwGameCubeDisplayList *)(instData + headerSize);\n    data = (u8 *)lists + listsSize;\n    data = (u8 *)(((u32)data + 31) & ~31);\n"
L3 = "    lists = (RwGameCubeDisplayList *)(instData + headerSize);\n    data = (u8 *)((u32)((u8 *)lists + listsSize + 31) & ~31);\n"
H_OLD = "    header->serial = lbl_805F1678;\n    header->meshSerial = geom->mesh->serialNum;\n    header->flags = 0;\n    SET_ALPHA_FLAG(header, geom);\n"
REPL = {
    "base": [],
    "a": [(A_OLD, A_NEW)],
    "l1": [(L_OLD, L1)],
    "a_l1": [(A_OLD, A_NEW), (L_OLD, L1)],
    "a_l2": [(A_OLD, A_NEW), (L_OLD, L2)],
    "a_l3": [(A_OLD, A_NEW), (L_OLD, L3)],
    "l2": [(L_OLD, L2)],
}
