"""Wave 40 (r2), RwCpp16 round 14: fn_80453D08 (RpUVAnim material frames) with unsigned index and
element counters, a signed found flag and the null interpolator as the else branch."""
SCORE = ["fn_80453D08"]
B0 = """    for (u32 slot = 0; slot < 2; slot++) {
        u32 found = 0;
        s32 element = 0;
        s32 index = 0;
        while (index < 8) {
            u32 node = object[2 + index];
            if (node == 0) {
                index++;
                continue;
            }
            u32 list = *(u32 *)node;
            u32 *elements = *(u32 **)((u8 *)list + 0x14);
            if (*(u32 *)((u8 *)elements + 0x20 + 4 * element) == slot) {
                found = 1;
                break;
            }
            element++;
            if (element < (s32)*(u32 *)((u8 *)node + 0x2C)) {
                continue;
            }
            element = 0;
            index++;
        }
"""
def body(decls):
    return ("    for (u32 slot = 0; slot < 2; slot++) {\n" + decls +
"""        while (index < 8) {
            u32 node = object[2 + index];
            if (node != 0) {
                u32 list = *(u32 *)node;
                u32 *elements = *(u32 **)((u8 *)list + 0x14);
                if (*(u32 *)((u8 *)elements + 0x20 + 4 * element) == slot) {
                    found = 1;
                    break;
                }
                element++;
                if (element >= *(u32 *)((u8 *)node + 0x2C)) {
                    element = 0;
                    index++;
                }
            } else {
                index++;
            }
        }
""")
D1 = "        u32 index = 0;\n        u32 element = 0;\n        s32 found = 0;\n"
D2 = "        s32 found = 0;\n        u32 element = 0;\n        u32 index = 0;\n"
D3 = "        u32 index;\n        u32 element;\n        s32 found;\n        index = 0;\n        element = 0;\n        found = 0;\n"
REPL = {"base": [(B0, B0)], "d1": [(B0, body(D1))], "d2": [(B0, body(D2))], "d3": [(B0, body(D3))]}
