"""Wave 40 (r2), RwCpp16 round 11: fn_80453980 keeps the UV animation globals' address in r31 and
the object in r30."""
SCORE = ["fn_80453980"]
B0 = """u32 *fn_80453980(u32 *object)
{
    void *created = fn_8047FC04(0x44, (void *)lbl_805EDD90, 4, (void *)lbl_805EDD94,
                                (void *)lbl_805F1498, 0x40135);
    lbl_805F1498 = (u32)created;
    if (created == 0) {
        object = 0;
    }
    fn_80479040(lbl_8056D748);
    fn_80477344(lbl_8056D748 + 0x34);
    fn_80477344(lbl_8056D748 + 0x64);
"""
REPL = {
 "base": [(B0, B0)],
 "gptr": [(B0, """u32 *fn_80453980(u32 *object)
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
""")],
 "gptr2": [(B0, """u32 *fn_80453980(u32 *object)
{
    u8 *globals = lbl_8056D748;

    lbl_805F1498 = (u32)fn_8047FC04(0x44, (void *)lbl_805EDD90, 4, (void *)lbl_805EDD94,
                                    (void *)lbl_805F1498, 0x40135);
    if (lbl_805F1498 == 0) {
        object = 0;
    }
    fn_80479040(globals + 0);
    fn_80477344(globals + 0x34);
    fn_80477344(globals + 0x64);
""")],
}
