"""Round 4 for EggShadowAndroid::vfDisp_0x64 (wave 39): `this` as a local of another type declared
first (the lever that gave Game/CameraMotion.cpp its registers)."""
SYMBOLS = {"vfDisp_0x64__Q216EggShadowAndroid16EggShadowAndroidFv": "void EggShadowAndroid::vfDisp_0x64() {"}
L = "    void *loader = fn_800122D4();\n"
def body(decl, s="self"):
    return decl + L + f"    void *model = fn_80012684(loader, {s}->m_enemy->m_motionBase, 5);\n" + '    void *motion = fn_8001258C(loader, fn_80078C88()->GetHandle(0xC), "SHADOW.MTP");\n' + f"    {s}->m_render = fn_802CDB08({s}->m_enemy->m_motionBase, MotionTable(lbl_805F8670, 1), model, motion);\n"
BODIES = {"vfDisp_0x64__Q216EggShadowAndroid16EggShadowAndroidFv": {
    "void_cast": body("    EggShadowAndroid *self = (EggShadowAndroid *)(void *)this;\n"),
    "void_local": body("    void *raw = this;\n    EggShadowAndroid *self = (EggShadowAndroid *)raw;\n"),
    "const_self": body("    const EggShadowAndroid *cself = this;\n    EggShadowAndroid *self = const_cast<EggShadowAndroid *>(cself);\n"),
    "u32_cast": body("    EggShadowAndroid *self = (EggShadowAndroid *)(u32)this;\n"),
    "reint": body("    EggShadowAndroid *self = reinterpret_cast<EggShadowAndroid *>(reinterpret_cast<u8 *>(this));\n"),
}}
