"""Wave 40: EffectParticle3DRW3's constructor reaches each float constant by its own address
(lis/addi per constant, no pool base): data pooling switched off for the function or the unit."""
R = "__ct__Q36Effect3RW319EffectParticle3DRW3FPCQ36Effect6System22UnkEffectDesc_801D0AFCPQ36Effect6System16EffectLaunchInfoPQ36Effect6System13EffectManager"
SCORE = [R]
C = "EffectParticle3DRW3::EffectParticle3DRW3(const UnkEffectDesc_801D0AFC *desc,"
D = "EffectParticle3DRW3::~EffectParticle3DRW3() {"
REPL = {
    "pool_fn": [(C, "#pragma pool_data off\n" + C), (D, "#pragma pool_data reset\n" + D)],
    "pool_unit": [("namespace Effect {\nnamespace RW3 {", "#pragma pool_data off\nnamespace Effect {\nnamespace RW3 {")],
}
