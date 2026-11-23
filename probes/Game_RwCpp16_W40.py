"""Wave 40 (r2), RwCpp16 round 1: saved registers of the node stream read fn_804530CC (retail:
node r31, i r30, base r29) and write fn_804531B0 (retail: count r31, node r30, i r29, base r28)."""
SCORE = ["fn_804530CC", "fn_804531B0"]
RD = """    record[5] = list;
    u8 *base = (u8 *)record[4];
    u8 *node = base;
    for (s32 i = 0; i < (s32)record[1]; i++, node += 0x20) {
"""
WR = """    u8 *base = (u8 *)record[4];
    u8 *node = base;
    s32 count = (s32)record[1];
    for (s32 i = 0; i < count; i++, node += 0x20) {
"""
REPL = {
 "base": [(RD, RD)],
 "rd_nib": [(RD, "    record[5] = list;\n    u8 *node;\n    s32 i;\n    u8 *base = (u8 *)record[4];\n    node = base;\n    for (i = 0; i < (s32)record[1]; i++, node += 0x20) {\n")],
 "wr_cnib": [(WR, "    s32 count;\n    u8 *node;\n    s32 i;\n    u8 *base = (u8 *)record[4];\n    node = base;\n    count = (s32)record[1];\n    for (i = 0; i < count; i++, node += 0x20) {\n")],
 "wr_cnib2": [(WR, "    s32 count;\n    u8 *node;\n    s32 i;\n    u8 *base;\n    base = (u8 *)record[4];\n    node = base;\n    count = (s32)record[1];\n    for (i = 0; i < count; i++, node += 0x20) {\n")],
}
