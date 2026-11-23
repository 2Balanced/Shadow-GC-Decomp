"""Wave 40: EffectLensFlareRW3's constructor with a description that is const nowhere (the ctor's
parameter, the inline reader's parameter and the cast between them)."""
R = "__ct__Q36Effect3RW318EffectLensFlareRW3FPCQ36Effect6System22UnkEffectDesc_801D0AFCPQ36Effect6System16EffectLaunchInfoPQ36Effect6System13EffectManager"
N = R.replace("FPCQ36Effect6System22", "FPQ36Effect6System22")
SCORE = [(R, R), (R, N)]
HP = "                              const UnkLensFlareDesc_80430CAC *desc) {"
CALL = "        ReadLensFlareDesc(&m_param, reinterpret_cast<const UnkLensFlareDesc_80430CAC *>(desc));"
C = "EffectLensFlareRW3::EffectLensFlareRW3(const UnkEffectDesc_801D0AFC *desc, EffectLaunchInfo *info,"
C2 = "    : EffectLensFlare(const_cast<UnkEffectDesc_801D0AFC *>(desc), info, manager) {"
H = "    EffectLensFlareRW3(const ::Effect::System::UnkEffectDesc_801D0AFC *desc,"
REPL = {
    "helper_nc": [(HP, HP.replace("const ", "")), (CALL, "        ReadLensFlareDesc(&m_param, reinterpret_cast<UnkLensFlareDesc_80430CAC *>(const_cast<UnkEffectDesc_801D0AFC *>(desc)));")],
    "all_nc": [(HP, HP.replace("const ", "")), (CALL, CALL.replace("const ", "")), (C, C.replace("const ", "")), (C2, "    : EffectLensFlare(desc, info, manager) {")],
    "ctor_nc": [(C, C.replace("const ", "")), (C2, "    : EffectLensFlare(desc, info, manager) {")],
}
H2 = "        : EffectLensFlareRW3(desc, info, manager) {}"
_h = [(H, H.replace("const ", "")), (H2, "        : EffectLensFlareRW3(const_cast< ::Effect::System::UnkEffectDesc_801D0AFC *>(desc), info, manager) {}")]
HDR = {"all_nc": {"Game/EffectManagerPJS.h": _h}, "ctor_nc": {"Game/EffectManagerPJS.h": _h}}
