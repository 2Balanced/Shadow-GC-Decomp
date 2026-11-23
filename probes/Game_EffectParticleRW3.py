"""Wave 40: the description parameter of EffectParticleRW3's constructor without const (the lever of
the EffectGlobalParam/EffectLensFlare/EffectArranger/EffectShimmer constructors in wave 39)."""
R = "__ct__Q36Effect3RW317EffectParticleRW3FPCQ36Effect6System22UnkEffectDesc_801D0AFCPQ36Effect6System16EffectLaunchInfoPQ36Effect6System13EffectManager"
SCORE = [(R, R.replace("FPCQ36Effect6System22", "FPQ36Effect6System22"))]
C = "EffectParticleRW3::EffectParticleRW3(const UnkEffectDesc_801D0AFC *desc, EffectLaunchInfo *info,"
H = "    EffectParticleRW3(const ::Effect::System::UnkEffectDesc_801D0AFC *desc,"
REPL = {"nonconst": [(C, C.replace("const ", ""))]}
HDR = {"nonconst": {"Game/EffectManagerPJS.h": [(H, H.replace("const ", ""))]}}
