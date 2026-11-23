"""Wave 41: EffectParticle::Generate (the unit is now built with -pool off). Retail keeps the new
particle in r28 straight from both news; GC/2.7 copies it into another register after each."""
G = "Generate__Q36Effect6System14EffectParticleFf"
SCORE = [G]
SW = """        EffectParticleExecUnit *particle;
        switch (m_desc->m_type) {
        case 2:
            particle = new EffectParticleExecUnit(
                reinterpret_cast<const UnkParticleDesc_80427EC8 *>(m_desc), m_commandPool);
            break;
        case 5:
            particle = new EffectParticleExecUnit(
                reinterpret_cast<const UnkParticle3DDesc_80427EC8 *>(m_desc), m_commandPool);
            break;
        }
"""
REPL = {
    "base": [],
    "init0": [(SW, SW.replace("        EffectParticleExecUnit *particle;\n", "        EffectParticleExecUnit *particle = 0;\n"))],
    "def_ret": [(SW, SW.replace("            break;\n        }\n", "            break;\n        default:\n            return;\n        }\n"))],
    "placement": [(SW, """        EffectParticleExecUnit *particle;
        switch (m_desc->m_type) {
        case 2:
            particle = (EffectParticleExecUnit *)EffectParticleExecUnit::operator new(sizeof(EffectParticleExecUnit));
            if (particle != 0) {
                new (particle) EffectParticleExecUnit(
                    reinterpret_cast<const UnkParticleDesc_80427EC8 *>(m_desc), m_commandPool);
            }
            break;
        case 5:
            particle = (EffectParticleExecUnit *)EffectParticleExecUnit::operator new(sizeof(EffectParticleExecUnit));
            if (particle != 0) {
                new (particle) EffectParticleExecUnit(
                    reinterpret_cast<const UnkParticle3DDesc_80427EC8 *>(m_desc), m_commandPool);
            }
            break;
        }
""")],
}

INC_OLD = '#include "Effect/System/EffectCommandExec.h"' + chr(10)
INC_NEW = '#include <new>' + chr(10) + INC_OLD
_P = REPL["placement"][0][1].replace("                new (particle)", "                ::new (particle)")
REPL["placement"] = [(SW, _P), (INC_OLD, INC_NEW)]
_P2 = _P
for _d in ("UnkParticleDesc_80427EC8", "UnkParticle3DDesc_80427EC8"):
    _old = ("            if (particle != 0) {" + chr(10) + "                ::new (particle) EffectParticleExecUnit(" + chr(10)
            + "                    reinterpret_cast<const " + _d + " *>(m_desc), m_commandPool);" + chr(10) + "            }")
    _new = ("            particle = ::new (particle) EffectParticleExecUnit(" + chr(10)
            + "                reinterpret_cast<const " + _d + " *>(m_desc), m_commandPool);")
    _P2 = _P2.replace(_old, _new)
REPL["placement2"] = [(SW, _P2), (INC_OLD, INC_NEW)]
