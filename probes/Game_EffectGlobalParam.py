"""Source shapes for EffectGlobalParam's constructor (wave 39): retail keeps each description
field's load after the previous store.  A description pointer to non-const should stop GC/2.7
hoisting the loads (the reverse of Game/CameraMotion.cpp's const pointer)."""
SCORE = ["__ct__Q36Effect6System17EffectGlobalParamFPCQ36Effect6System22UnkEffectDesc_801D0AFCPQ36Effect6System16EffectLaunchInfoPQ36Effect6System13EffectManager"]
OLD = """    const UnkGlobalParamDesc_8042A6E4 *params =
        reinterpret_cast<const UnkGlobalParamDesc_8042A6E4 *>(desc);
"""
REPL = {
  "nonconst": [(OLD, "    UnkGlobalParamDesc_8042A6E4 *params = (UnkGlobalParamDesc_8042A6E4 *)desc;\n")],
  "nonconst_cc": [(OLD, "    UnkGlobalParamDesc_8042A6E4 *params = const_cast<UnkGlobalParamDesc_8042A6E4 *>(reinterpret_cast<const UnkGlobalParamDesc_8042A6E4 *>(desc));\n")],
}
