"""Wave 41 (f1): EffectParticle::Generate with EffectRay's Generate shapes (probes/Game_EffectRay_W41h.py):
the shape declared first, spread/angle/size shared at the top of the loop, `spread / 2.0f`, the
height's 0.0f between the sine and the cosine, the radius pick as a conditional expression."""
import itertools, os
SCORE = ["Generate__Q36Effect6System14EffectParticleFf"]
START = "void EffectParticle::Generate(float dt) {\n"
END = "void EffectParticle::Move(float dt) {\n"

def gen(k):
    L = []
    a = L.append
    a("void EffectParticle::Generate(float dt) {")
    for d in k.get("fntop", ["const UnkParticleShape_80427EC8 *shape;"]):
        a("    " + d)
    a("    int count = 0;")
    a("    float quantity = m_rate * dt;")
    a("    if (quantity < 1.0f) {")
    a("        if (RandomFloat() < quantity) {")
    a("            count = 1;")
    a("        }")
    a("    } else {")
    a("        count = (int)quantity;")
    a("    }")
    a("    if (count <= 0) {")
    a("        return;")
    a("    }")
    a("    float speed = 0.0f;")
    a("    if (m_config & 0x1000) {")
    a("        speed = fn_8040CF50(&m_velocity);")
    a("    }")
    a("    for (int i = 0; i < count; i++) {")
    for d in k.get("looptop", ["float spread;", "float angle;", "float size;"]):
        a("        " + d)
    a("        if (m_count == m_liveCount) {")
    a("            return;")
    a("        }")
    a("        fn_8042A5D0()->SetPool(m_particlePool);")
    if k.get("pdecl", "loop") == "loop":
        a("        EffectParticleExecUnit *particle;")
    a("        switch (m_desc->m_type) {")
    a("        case 2:")
    a("            particle = new EffectParticleExecUnit(")
    a("                reinterpret_cast<const UnkParticleDesc_80427EC8 *>(m_desc), m_commandPool);")
    a("            break;")
    a("        case 5:")
    a("            particle = new EffectParticleExecUnit(")
    a("                reinterpret_cast<const UnkParticle3DDesc_80427EC8 *>(m_desc), m_commandPool);")
    a("            break;")
    a("        }")
    a("        if (particle == 0) {")
    a("            return;")
    a("        }")
    a("        m_liveCount++;")
    a("        if (m_lastParticle != 0) {")
    a("            m_lastParticle->m_next = particle;")
    a("            particle->m_prev = m_lastParticle;")
    a("            m_lastParticle = particle;")
    a("        } else {")
    a("            m_lastParticle = particle;")
    a("            m_particles = particle;")
    a("        }")
    a("        const UnkParticleParams_80427EC8 *params = m_params;")
    a("        float velocity = params->m_speedRange * RandomFloat() + params->m_speed;")
    a("        " + ("shape = m_emitter->m_shape;" if k.get("shape_top", True) else "const UnkParticleShape_80427EC8 *shape = m_emitter->m_shape;"))
    a("        switch (shape->m_type) {")
    a("        default: {")
    a("            AssignVector(particle->m_position, m_lastPosition);")
    a("            Vec3f up;")
    a("            up.y = velocity;")
    a("            AssignVector(particle->m_velocity, up);")
    a("            break;")
    a("        }")
    a("        case 0: {")
    a("            spread = shape->m_spread;")
    a("            if ((m_config & 0x40) && count > 1) {")
    a("                if (spread > 359.9999f) {")
    a("                    angle = spread * ((float)i / (float)count);")
    a("                } else {")
    a("                    angle = spread * ((float)i / (float)(count - 1));")
    a("                }")
    a("                angle -= spread / 2.0f;")
    a("            } else {")
    a("                angle = spread * RandomFloat() - shape->m_spread / 2.0f;")
    a("            }")
    a("            float radius = 0.0f;")
    a("            float tilt = 0.0f;")
    a("            if (shape->m_radius > 0.0001f) {")
    a("                float r = (m_config & 0x80) ? shape->m_radius : shape->m_radius * RandomFloat();")
    a("                radius = r;")
    a("                tilt = shape->m_angle * (r / shape->m_radius);")
    a("            }")
    a("            float s = std::sin(angle);")
    a("            float height = 0.0f;")
    a("            float c = std::cos(angle);")
    a("            if (shape->m_height > 0.0001f) {")
    a("                height = shape->m_height * RandomFloat();")
    a("            }")
    a("            particle->m_position.x = s * radius;")
    a("            particle->m_position.y = height;")
    a("            particle->m_position.z = c * radius;")
    a("            float ts = std::sin(tilt);")
    a("            float tc = std::cos(tilt);")
    a("            particle->m_velocity.x = s * (ts * velocity);")
    a("            particle->m_velocity.y = tc * velocity;")
    a("            particle->m_velocity.z = c * (ts * velocity);")
    a("            break;")
    a("        }")
    a("        case 1: {")
    a("            float spread = shape->m_spread;")
    a("            angle = spread * RandomFloat() - shape->m_spread;")
    a("            float radius = 0.0f;")
    a("            if (shape->m_radius > 0.0001f) {")
    a("                if (m_config & 0x80) {")
    a("                    radius = shape->m_radius;")
    a("                } else {")
    a("                    radius = shape->m_radius * RandomFloat();")
    a("                }")
    a("            }")
    a("            float tilt = shape->m_angle * RandomFloat();")
    a("            float ts = std::sin(tilt);")
    a("            float tc = std::cos(tilt);")
    a("            float x = ts * std::sin(angle);")
    a("            float z = ts * std::cos(angle);")
    a("            particle->m_position.x = x * radius;")
    a("            particle->m_position.y = tc * radius;")
    a("            particle->m_position.z = z * radius;")
    a("            particle->m_velocity.x = x * velocity;")
    a("            particle->m_velocity.y = tc * velocity;")
    a("            particle->m_velocity.z = z * velocity;")
    a("            if (m_config & 0x20) {")
    a("                particle->m_velocity.x = -particle->m_velocity.x;")
    a("                particle->m_velocity.y = -particle->m_velocity.y;")
    a("                particle->m_velocity.z = -particle->m_velocity.z;")
    a("            }")
    a("            break;")
    a("        }")
    a("        case 2: {")
    a("            size = shape->m_radius;")
    a("            particle->m_position.x = size * RandomFloat() - 0.5f * size;")
    a("            size = shape->m_spread;")
    a("            particle->m_position.y = size * RandomFloat() - 0.5f * size;")
    a("            size = shape->m_angle;")
    a("            particle->m_position.z = size * RandomFloat() - 0.5f * size;")
    a("            particle->m_velocity.x = 0.0f;")
    a("            particle->m_velocity.y = velocity;")
    a("            particle->m_velocity.z = 0.0f;")
    a("            break;")
    a("        }")
    a("        }")
    a("        if (!(m_config & 0x800)) {")
    a("            AssignVector(particle->m_position, fn_8040B67C(&m_worldMatrix, &particle->m_position));")
    a("            AssignVector(particle->m_velocity, fn_8040B618(&m_worldMatrix, &particle->m_velocity));")
    a("            if (speed > 0.0001f) {")
    a("                float dot = fn_8040CF90(&particle->m_velocity, &m_velocity);")
    a("                float scale = dot / (speed * fn_8040CF50(&particle->m_velocity));")
    a("                fn_8040CFF8(&particle->m_velocity,")
    a("                            &(const Vec3f &)fn_800091A4(&m_velocity,")
    a("                                                        scale > 0.0 ? scale : -scale));")
    a("            }")
    a("        }")
    a("        particle->m_cursor = m_params->m_commands;")
    a("    }")
    a("}")
    a("")
    a("")
    return "\n".join(L)

_text = open(os.path.join("src", "Game", "EffectParticle.cpp"), encoding="utf-8").read()
OLD = _text[_text.index(START):_text.index(END)]
SH = "const UnkParticleShape_80427EC8 *shape;"
PD = "EffectParticleExecUnit *particle;"
VARIANTS = {
    "p_base": dict(),
    "p_noshape_top": dict(fntop=[], shape_top=False),
    "p_part_fn": dict(fntop=[SH, PD], pdecl="fn"),
    "p_part_fn_after": dict(fntop=[SH], pdecl="fn", looptop=["float spread;", "float angle;", "float size;"]),
}
REPL = {n: [(OLD, gen(k))] for n, k in VARIANTS.items()}
# particle declared at function top after count
REPL["p_part_after_count"] = [(OLD, gen(dict(pdecl="fn")).replace("    int count = 0;\n", "    int count = 0;\n    EffectParticleExecUnit *particle;\n", 1))]
REPL["p_part_after_i"] = [(OLD, gen(dict()).replace("        float spread;\n        float angle;\n        float size;\n        if (m_count", "        float spread;\n        float angle;\n        float size;\n        EffectParticleExecUnit *particle;\n        if (m_count", 1).replace("        fn_8042A5D0()->SetPool(m_particlePool);\n        EffectParticleExecUnit *particle;\n", "        fn_8042A5D0()->SetPool(m_particlePool);\n", 1))]
REPL["p_part_looptop_first"] = [(OLD, gen(dict(looptop=["EffectParticleExecUnit *particle;", "float spread;", "float angle;", "float size;"], pdecl="x")))]

# Round 2: retail gives spread f31 and speed f30, so spread is declared ahead of speed.
F3 = ["float spread;", "float angle;", "float size;"]
REPL["q_fn_first"] = [(OLD, gen(dict(fntop=F3 + [SH], looptop=[])))]
REPL["q_fn_after_shape"] = [(OLD, gen(dict(fntop=[SH] + F3, looptop=[])))]
_g = gen(dict(looptop=[]))
REPL["q_before_speed"] = [(OLD, _g.replace("    float speed = 0.0f;\n", "    float spread;\n    float angle;\n    float size;\n    float speed = 0.0f;\n", 1))]
REPL["q_sp_before_speed"] = [(OLD, gen(dict(looptop=["float angle;", "float size;"])).replace("    float speed = 0.0f;\n", "    float spread;\n    float speed = 0.0f;\n", 1))]
REPL["q_sp_first"] = [(OLD, gen(dict(fntop=["float spread;", SH], looptop=["float angle;", "float size;"])))]
REPL["q_sp_after_shape"] = [(OLD, gen(dict(fntop=[SH, "float spread;"], looptop=["float angle;", "float size;"])))]
