"""Source shapes for EggShadowAndroid::vfDisp_0x64 (wave 39): retail gives this r31, the loader r30
and the model r29, and moves the loader into r3 before the file name's low half."""
SYMBOLS = {"vfDisp_0x64__Q216EggShadowAndroid16EggShadowAndroidFv": "void EggShadowAndroid::vfDisp_0x64() {"}
XTRA = {"vfDisp_0x64__Q216EggShadowAndroid16EggShadowAndroidFv": """inline void *LoadMotion_(void *loader, int handle, const char *name) { return fn_8001258C(loader, handle, name); }
inline void *PlayerMotion_(void *loader, const char *name) { return fn_8001258C(loader, fn_80078C88()->GetHandle(0xC), name); }
inline int ShadowHandle_() { return fn_80078C88()->GetHandle(0xC); }
"""}
END = "    m_render = fn_802CDB08(m_enemy->m_motionBase, MotionTable(lbl_805F8670, 1), model, motion);\n"
L = "    void *loader = fn_800122D4();\n"
M = "    void *model = fn_80012684(loader, m_enemy->m_motionBase, 5);\n"
BODIES = {"vfDisp_0x64__Q216EggShadowAndroid16EggShadowAndroidFv": {
    "comma_name": L + M + '    void *motion = fn_8001258C(loader, fn_80078C88()->GetHandle(0xC), (0, "SHADOW.MTP"));\n' + END,
    "name_local": L + M + '    const char *name = "SHADOW.MTP";\n    void *motion = fn_8001258C(loader, fn_80078C88()->GetHandle(0xC), name);\n' + END,
    "handle_local": L + M + '    int handle = fn_80078C88()->GetHandle(0xC);\n    void *motion = fn_8001258C(loader, handle, "SHADOW.MTP");\n' + END,
    "load_inline": L + M + '    void *motion = LoadMotion_(loader, fn_80078C88()->GetHandle(0xC), "SHADOW.MTP");\n' + END,
    "handle_inline": L + M + '    void *motion = fn_8001258C(loader, ShadowHandle_(), "SHADOW.MTP");\n' + END,
    "mgr_local": L + M + '    CharacterFileManager *files = fn_80078C88();\n    void *motion = fn_8001258C(loader, files->GetHandle(0xC), "SHADOW.MTP");\n' + END,
    "decl_first": "    void *loader;\n    void *model;\n    void *motion;\n    loader = fn_800122D4();\n    model = fn_80012684(loader, m_enemy->m_motionBase, 5);\n" + '    motion = fn_8001258C(loader, fn_80078C88()->GetHandle(0xC), "SHADOW.MTP");\n' + END,
    "decl_rev": "    void *motion;\n    void *model;\n    void *loader;\n    loader = fn_800122D4();\n    model = fn_80012684(loader, m_enemy->m_motionBase, 5);\n" + '    motion = fn_8001258C(loader, fn_80078C88()->GetHandle(0xC), "SHADOW.MTP");\n' + END,
    "enemy_first": "    Enemy *enemy = m_enemy;\n" + L + "    void *model = fn_80012684(loader, enemy->m_motionBase, 5);\n" + '    void *motion = fn_8001258C(loader, fn_80078C88()->GetHandle(0xC), "SHADOW.MTP");\n' + "    m_render = fn_802CDB08(enemy->m_motionBase, MotionTable(lbl_805F8670, 1), model, motion);\n",
    "self_local": "    EggShadowAndroid *self = this;\n" + L + "    void *model = fn_80012684(loader, self->m_enemy->m_motionBase, 5);\n" + '    void *motion = fn_8001258C(loader, fn_80078C88()->GetHandle(0xC), "SHADOW.MTP");\n' + "    self->m_render = fn_802CDB08(self->m_enemy->m_motionBase, MotionTable(lbl_805F8670, 1), model, motion);\n",
    "table_local": L + M + '    void *motion = fn_8001258C(loader, fn_80078C88()->GetHandle(0xC), "SHADOW.MTP");\n' + "    MotionTable table(lbl_805F8670, 1);\n    m_render = fn_802CDB08(m_enemy->m_motionBase, table, model, motion);\n",
    "table_first": "    MotionTable table(lbl_805F8670, 1);\n" + L + M + '    void *motion = fn_8001258C(loader, fn_80078C88()->GetHandle(0xC), "SHADOW.MTP");\n' + "    m_render = fn_802CDB08(m_enemy->m_motionBase, table, model, motion);\n",
}}
