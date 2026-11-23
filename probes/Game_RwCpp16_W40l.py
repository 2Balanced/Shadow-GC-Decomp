"""Wave 40 (r2), RwCpp16 round 12: fn_80453980 with the globals as a struct (retail: addi r3,r31,0
for the first member, the address formed straight into r31)."""
SCORE = ["fn_80453980", "fn_80453A14"]
B0 = """u32 *fn_80453980(u32 *object)
{
    u8 *globals = lbl_8056D748;
    void *created = fn_8047FC04(0x44, (void *)lbl_805EDD90, 4, (void *)lbl_805EDD94,
                                (void *)lbl_805F1498, 0x40135);
    lbl_805F1498 = (u32)created;
    if (created == 0) {
        object = 0;
    }
    fn_80479040(globals);
    fn_80477344(globals + 0x34);
    fn_80477344(globals + 0x64);
"""
TY = """typedef struct R2UVAnimGlobals {
    u8 schema[0x34];
    u8 list34[0x30];
    u8 list64[0x30];
} R2UVAnimGlobals;
extern R2UVAnimGlobals lbl_8056D748;
"""
def body(first, init_after=False):
    s = "u32 *fn_80453980(u32 *object)\n{\n"
    if init_after:
        s += "    R2UVAnimGlobals *globals;\n    void *created;\n\n    globals = &lbl_8056D748;\n    created = fn_8047FC04(0x44, (void *)lbl_805EDD90, 4, (void *)lbl_805EDD94,\n                                (void *)lbl_805F1498, 0x40135);\n"
    else:
        s += "    R2UVAnimGlobals *globals = &lbl_8056D748;\n    void *created = fn_8047FC04(0x44, (void *)lbl_805EDD90, 4, (void *)lbl_805EDD94,\n                                (void *)lbl_805F1498, 0x40135);\n"
    s += "    lbl_805F1498 = (u32)created;\n    if (created == 0) {\n        object = 0;\n    }\n"
    s += "    fn_80479040(%s);\n    fn_80477344(globals->list34);\n    fn_80477344(globals->list64);\n" % first
    return s
REPL = {
 "base": [(B0, B0)],
 "s1": [("extern u8 lbl_8056D748[];\n", TY), (B0, body("globals->schema")), ("fn_8047909C((u32 *)lbl_8056D748);", "fn_8047909C((u32 *)&lbl_8056D748);")],
 "s2": [("extern u8 lbl_8056D748[];\n", TY), (B0, body("&globals->schema")), ("fn_8047909C((u32 *)lbl_8056D748);", "fn_8047909C((u32 *)&lbl_8056D748);")],
 "s3": [("extern u8 lbl_8056D748[];\n", TY), (B0, body("globals", True)), ("fn_8047909C((u32 *)lbl_8056D748);", "fn_8047909C((u32 *)&lbl_8056D748);")],
 "s4": [("extern u8 lbl_8056D748[];\n", TY), (B0, body("globals->schema", True)), ("fn_8047909C((u32 *)lbl_8056D748);", "fn_8047909C((u32 *)&lbl_8056D748);")],
}
