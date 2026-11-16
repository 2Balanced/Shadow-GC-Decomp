"""Round 2 for EggShadowAndroid::vfDisp_0x64 (wave 39): the loader through an inline call, so that
`this` is saved before the locals exist."""
SYMBOLS = {"vfDisp_0x64__Q216EggShadowAndroid16EggShadowAndroidFv": "void EggShadowAndroid::vfDisp_0x64() {"}
XTRA = {"vfDisp_0x64__Q216EggShadowAndroid16EggShadowAndroidFv": """inline void *Loader_() { return fn_800122D4(); }
struct LoaderRef_ { static void *Get() { return fn_800122D4(); } };
"""}
END = "    m_render = fn_802CDB08(m_enemy->m_motionBase, MotionTable(lbl_805F8670, 1), model, motion);\n"
M = "    void *model = fn_80012684(loader, m_enemy->m_motionBase, 5);\n"
MO = '    void *motion = fn_8001258C(loader, fn_80078C88()->GetHandle(0xC), "SHADOW.MTP");\n'
BODIES = {"vfDisp_0x64__Q216EggShadowAndroid16EggShadowAndroidFv": {
    "loader_inline": "    void *loader = Loader_();\n" + M + MO + END,
    "loader_static": "    void *loader = LoaderRef_::Get();\n" + M + MO + END,
    "call_then_decl": "    fn_800122D4();\n    void *loader = fn_800122D4();\n" + M + MO + END,
    "nested_assign": "    void *loader;\n    void *model = fn_80012684(loader = fn_800122D4(), m_enemy->m_motionBase, 5);\n" + MO + END,
    "nested_assign_inl": "    void *loader;\n    void *model = fn_80012684(loader = Loader_(), m_enemy->m_motionBase, 5);\n" + MO + END,
}}
