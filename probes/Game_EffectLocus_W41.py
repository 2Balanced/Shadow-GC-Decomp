"""Wave 41: EffectLocus's constructor with a description that is not const (retail reloads the rate
after storing the point count, as EffectParticleRW3's constructor does)."""
CT = "__ct__Q36Effect6System11EffectLocusFPCQ36Effect6System22UnkEffectDesc_801D0AFCPQ36Effect6System16EffectLaunchInfoPQ36Effect6System13EffectManager"
CTN = "__ct__Q36Effect6System11EffectLocusFPQ36Effect6System22UnkEffectDesc_801D0AFCPQ36Effect6System16EffectLaunchInfoPQ36Effect6System13EffectManager"
SCORE = [(CT, CTN)]
D = "EffectLocus::EffectLocus(const UnkEffectDesc_801D0AFC *desc, EffectLaunchInfo *info,"
H = {"Game/EffectManagerPJS.h": [("    EffectLocus(const UnkEffectDesc_801D0AFC *desc, EffectLaunchInfo *info,", "    EffectLocus(UnkEffectDesc_801D0AFC *desc, EffectLaunchInfo *info,")]}
REPL = {
    "nonconst": [(D, "EffectLocus::EffectLocus(UnkEffectDesc_801D0AFC *desc, EffectLaunchInfo *info,")],
}
HDR = {"nonconst": H}
