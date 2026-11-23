"""fn_804210E4 (wave 40): the 16-bit swap fn_8042118C inlined by the compiler itself (auto-inline
scoped to this function: not inlined, the call stays) or declared inline."""
OLD = """        u8 *p0 = (u8 *)&half[0];
        SwapBytes16(p0, p0, 1);
        u8 *p1 = (u8 *)&half[1];
        SwapBytes16(p1, p1, 1);
"""
D = "void fn_8042118C(u8 *dst, const u8 *src, int count) {\n"
DECL = "void fn_8042118C(u8 *dst, const u8 *src, int count);\n"
def v(body, inl="inline ", decl=DECL):
    return [(D, inl + D), (OLD, body), (DECL, decl)]
E0 = "        fn_8042118C((u8 *)half, (u8 *)half, 1);\n"
E1 = "        fn_8042118C((u8 *)&half[1], (u8 *)&half[1], 1);\n"
REPL = {
 "inl_expr": v(E0 + E1),
 "inl_loc": v("        u8 *p0 = (u8 *)&half[0];\n        fn_8042118C(p0, p0, 1);\n        u8 *p1 = (u8 *)&half[1];\n        fn_8042118C(p1, p1, 1);\n"),
 "inl_plus": v(E0 + "        fn_8042118C((u8 *)(half + 1), (u8 *)(half + 1), 1);\n"),
 "inl_mixed": v(E0 + "        u8 *p1 = (u8 *)&half[1];\n        fn_8042118C(p1, p1, 1);\n"),
 "inl_u16": v("        fn_8042118C((u8 *)&half[0], (const u8 *)&half[0], 1);\n        fn_8042118C((u8 *)&half[1], (const u8 *)&half[1], 1);\n"),
}
SCORE = ["fn_804210E4"]
