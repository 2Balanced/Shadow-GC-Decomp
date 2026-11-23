"""Wave 41 (f1): EffectRay's constructor with the description as a pointer to non-const (the wave 40
constructor lever: retail keeps every description load behind the store before it), under
cflags_effect_nopool."""
CT = "__ct__Q36Effect6System9EffectRayFPCQ36Effect6System22UnkEffectDesc_801D0AFCPQ36Effect6System16EffectLaunchInfoPQ36Effect6System13EffectManager"
CT_NC = "__ct__Q36Effect6System9EffectRayFPQ36Effect6System22UnkEffectDesc_801D0AFCPQ36Effect6System16EffectLaunchInfoPQ36Effect6System13EffectManager"
SCORE = [(CT, CT_NC)]
NC_CPP = [("EffectRay::EffectRay(const UnkEffectDesc_801D0AFC *desc,", "EffectRay::EffectRay(UnkEffectDesc_801D0AFC *desc,")]
NC_H = {"Effect/System/EffectRay.h": [("EffectRay(const UnkEffectDesc_801D0AFC *desc, EffectLaunchInfo *info, EffectManager *manager);",
                                       "EffectRay(UnkEffectDesc_801D0AFC *desc, EffectLaunchInfo *info, EffectManager *manager);")]}
P_NC = [("    const UnkRayDesc_804298D4 *params = reinterpret_cast<const UnkRayDesc_804298D4 *>(desc);\r\n    m_unitCount",
         "    UnkRayDesc_804298D4 *params = reinterpret_cast<UnkRayDesc_804298D4 *>(desc);\r\n    m_unitCount")]
REPL = {
    "base": [],
    "nc": NC_CPP,
    "nc_params": NC_CPP + P_NC,
}
HDR = {"nc": NC_H, "nc_params": NC_H}
