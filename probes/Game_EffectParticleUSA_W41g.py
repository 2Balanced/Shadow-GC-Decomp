"""Wave 41 (f1): EffectParticleUSA's inline destructor.  Retail emits it between Update and the
constructor of EffectParticleUSA; GC/2.7 emits the in-class inline destructor after the unit's last
function (a late vtable's inline virtual)."""
DT = "__dt__Q412SonicteamUSA6System6Effect17EffectParticleUSAFv"
SCORE = [DT]
UPD = "void EffectParticleUSA::Update(float dt) {\n"
HD = {"Game/EffectManagerPJS.h": [("    virtual ~EffectParticleUSA() {}\n", "    virtual ~EffectParticleUSA();\n")]}
HD_INL = {"Game/EffectManagerPJS.h": [("    virtual ~EffectParticleUSA() {}\n", "    virtual inline ~EffectParticleUSA();\n")]}
REPL = {
    "base": [],
    "dt_out": [(UPD, "EffectParticleUSA::~EffectParticleUSA() {}\n\n" + UPD)],
    "dt_out_inl": [(UPD, "inline EffectParticleUSA::~EffectParticleUSA() {}\n\n" + UPD)],
    "dt_out_after": [("void EffectParticleUSA::Update(float dt) {", "void EffectParticleUSA::Update(float dt) {")],
}
HDR = {"dt_out": HD, "dt_out_inl": HD_INL}
