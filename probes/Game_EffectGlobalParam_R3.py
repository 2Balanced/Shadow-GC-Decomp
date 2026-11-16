"""Round 3 for EffectGlobalParam's constructor (wave 39): the description parameter as a pointer to
non-const (the symbol name changes with it; this round only looks at the code)."""
R = "__ct__Q36Effect6System17EffectGlobalParamFPCQ36Effect6System22UnkEffectDesc_801D0AFCPQ36Effect6System16EffectLaunchInfoPQ36Effect6System13EffectManager"
SCORE = [(R, R.replace("FPCQ36Effect6System22UnkEffectDesc", "FPQ36Effect6System22UnkEffectDesc"))]
H = "Effect/System/EffectGlobalParam.h"
HDR = {"nonconst_param": {H: [("    EffectGlobalParam(const UnkEffectDesc_801D0AFC *desc, EffectLaunchInfo *info,", "    EffectGlobalParam(UnkEffectDesc_801D0AFC *desc, EffectLaunchInfo *info,")]}}
REPL = {"nonconst_param": [("EffectGlobalParam::EffectGlobalParam(const UnkEffectDesc_801D0AFC *desc, EffectLaunchInfo *info,", "EffectGlobalParam::EffectGlobalParam(UnkEffectDesc_801D0AFC *desc, EffectLaunchInfo *info,"),
  ("    const UnkGlobalParamDesc_8042A6E4 *params =\n        reinterpret_cast<const UnkGlobalParamDesc_8042A6E4 *>(desc);\n", "    UnkGlobalParamDesc_8042A6E4 *params = reinterpret_cast<UnkGlobalParamDesc_8042A6E4 *>(desc);\n")]}
