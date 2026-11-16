"""Round 3 for EggShadowAndroid::vfDisp_0x64 (wave 39): the loading in an inline helper, so that
its locals are created after `this`."""
SYMBOLS = {"vfDisp_0x64__Q216EggShadowAndroid16EggShadowAndroidFv": "void EggShadowAndroid::vfDisp_0x64() {"}
XTRA = {"vfDisp_0x64__Q216EggShadowAndroid16EggShadowAndroidFv": """inline boost::shared_ptr<EnemyRender> LoadRenderA_(int slot, int slot2) {
    void *loader = fn_800122D4();
    void *model = fn_80012684(loader, slot, 5);
    void *motion = fn_8001258C(loader, fn_80078C88()->GetHandle(0xC), "SHADOW.MTP");
    return fn_802CDB08(slot2, MotionTable(lbl_805F8670, 1), model, motion);
}
inline void *LoadModel_(void *loader, int slot) { return fn_80012684(loader, slot, 5); }
inline void *LoadMotion_(void *loader) { return fn_8001258C(loader, fn_80078C88()->GetHandle(0xC), "SHADOW.MTP"); }
inline void LoadBoth_(int slot, void **model, void **motion) {
    void *loader = fn_800122D4();
    *model = fn_80012684(loader, slot, 5);
    *motion = fn_8001258C(loader, fn_80078C88()->GetHandle(0xC), "SHADOW.MTP");
}
"""}
END = "    m_render = fn_802CDB08(m_enemy->m_motionBase, MotionTable(lbl_805F8670, 1), model, motion);\n"
BODIES = {"vfDisp_0x64__Q216EggShadowAndroid16EggShadowAndroidFv": {
    "helper_ret": "    m_render = LoadRenderA_(m_enemy->m_motionBase, m_enemy->m_motionBase);\n",
    "helper_parts": "    void *loader = fn_800122D4();\n    void *model = LoadModel_(loader, m_enemy->m_motionBase);\n    void *motion = LoadMotion_(loader);\n" + END,
    "helper_both": "    void *model;\n    void *motion;\n    LoadBoth_(m_enemy->m_motionBase, &model, &motion);\n" + END,
}}
