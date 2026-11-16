"""The byte-order helpers of Game/TExpandBG.cpp (wave 39, second pass): retail loads the value into a
free argument register (r4, r7) where a direct __sthbrx(*p, p, 0) uses r0."""
A = 'extern "C" void fn_80043D0C(void *value, int size) {\n'
S16 = "__sthbrx(*p, p, 0);"
S32 = "__stwbrx(*p, p, 0);"
def v(pre, c16, c32):
    return [(A, pre + "\n" + A), (S16, c16), (S32, c32)]
REPL = {
 "val_param": v("inline void Store16(u16 value, u16 *p) {\n    __sthbrx(value, p, 0);\n}\ninline void Store32(u32 value, u32 *p) {\n    __stwbrx(value, p, 0);\n}\n", "Store16(*p, p);", "Store32(*p, p);"),
 "ptr_param": v("inline void Swap16(u16 *p) {\n    __sthbrx(*p, p, 0);\n}\ninline void Swap32(u32 *p) {\n    __stwbrx(*p, p, 0);\n}\n", "Swap16(p);", "Swap32(p);"),
 "ptr_local": v("inline void Swap16(u16 *p) {\n    u16 value = *p;\n    __sthbrx(value, p, 0);\n}\ninline void Swap32(u32 *p) {\n    u32 value = *p;\n    __stwbrx(value, p, 0);\n}\n", "Swap16(p);", "Swap32(p);"),
 "ref_param": v("inline void Swap16(u16 &r) {\n    __sthbrx(r, &r, 0);\n}\ninline void Swap32(u32 &r) {\n    __stwbrx(r, &r, 0);\n}\n", "Swap16(*p);", "Swap32(*p);"),
 "ret_val": v("inline u16 Load16(const u16 *p) {\n    return *p;\n}\ninline u32 Load32(const u32 *p) {\n    return *p;\n}\n", "__sthbrx(Load16(p), p, 0);", "__stwbrx(Load32(p), p, 0);"),
 "int_param": v("inline void Store16(u32 value, void *p) {\n    __sthbrx(value, p, 0);\n}\ninline void Store32(u32 value, void *p) {\n    __stwbrx(value, p, 0);\n}\n", "Store16(*p, p);", "Store32(*p, p);"),
 "local": [(S16, "u16 value = *p;\n        __sthbrx(value, p, 0);"), (S32, "u32 value = *p;\n        __stwbrx(value, p, 0);")],
}
SCORE = ["fn_80043CE4", "fn_80043D0C", "fn_80043C58"]
