"""Wave 41 (f1): EffectRay::Generate written from a template with knobs (variable placement), under
cflags_effect_nopool and the non-const constructor."""
import itertools
SCORE = ["Generate__Q36Effect6System9EffectRayFf"]
START = "void EffectRay::Generate(float dt) {\n"
END = "void EffectRay::Move(float dt) {\n"

def gen(k):
    L = []
    a = L.append
    a("void EffectRay::Generate(float dt) {")
    a("    const UnkRayShape_804298D4 *shape;")
    a("    const UnkRayDesc_804298D4 *desc;")
    for d in k.get("fntop", []):
        a("    " + d)
    a("    int count = 0;")
    a("    float quantity = m_rate * dt;")
    a("    desc = reinterpret_cast<const UnkRayDesc_804298D4 *>(m_desc);")
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
    a("    for (int i = 0; i < count; i++) {")
    for d in k.get("looptop", ["float spread;", "float angle;"]):
        a("        " + d)
    a("        if (m_unitCount == m_activeCount) {")
    a("            return;")
    a("        }")
    a("        fn_8042A5D0()->SetPool(m_unitPool);")
    a("        EffectRayExecUnit *ray = new EffectRayExecUnit(")
    a("            reinterpret_cast<const UnkRayDesc_804298D4 *>(m_desc), m_commandPool);")
    a("        if (ray == 0) {")
    a("            return;")
    a("        }")
    a("        m_activeCount++;")
    a("        if (m_tail != 0) {")
    a("            m_tail->m_next = ray;")
    a("            ray->m_prev = m_tail;")
    a("            m_tail = ray;")
    a("        } else {")
    a("            m_tail = ray;")
    a("            m_head = ray;")
    a("        }")
    a("        float velocity = desc->m_speedRange * RandomFloat() + desc->m_speed;")
    a("        shape = desc->m_shape;")
    a("        switch (shape->m_type) {")
    a("        default: {")
    a("            AssignVector(ray->m_start, m_lastPosition);")
    a("            Vec3f up;")
    a("            up.y = velocity;")
    a("            AssignVector(ray->m_velocity, up);")
    a("            break;")
    a("        }")
    a("        case 0: {")
    decl = lambda name, init: (name + " = " + init + ";") if name in k.get("shared", ["spread", "angle"]) else ("float " + name + " = " + init + ";")
    a("            " + decl("spread", "shape->m_spread"))
    if "angle" not in k.get("shared", ["spread", "angle"]):
        a("            float angle;")
    a("            if ((m_config & 0x40) && count > 1) {")
    a("                if (spread > 359.9999f) {")
    a("                    angle = spread * ((float)i / (float)count);")
    a("                } else {")
    a("                    angle = spread * ((float)i / (float)(count - 1));")
    a("                }")
    a("                " + k.get("halfsub", "angle -= spread * 0.5f;"))
    a("            } else {")
    a("                " + k.get("randangle", "angle = spread * RandomFloat() - shape->m_spread * 0.5f;"))
    a("            }")
    for line in k.get("c0_radius", [
        "float radius = 0.0f;",
        "float tilt = 0.0f;",
        "if (shape->m_radius > 0.0001f) {",
        "    float r;",
        "    if (m_config & 0x80) {",
        "        r = shape->m_radius;",
        "    } else {",
        "        r = shape->m_radius * RandomFloat();",
        "    }",
        "    radius = r;",
        "    tilt = shape->m_angle * (r / shape->m_radius);",
        "}"]):
        a("            " + line)
    for line in k.get("c0_sc", [
        "float s = std::sin(angle);",
        "float c = std::cos(angle);",
        "float height = 0.0f;",
        "if (shape->m_height > 0.0001f) {",
        "    height = shape->m_height * RandomFloat();",
        "}"]):
        a("            " + line)
    a("            ray->m_start.x = s * radius;")
    a("            ray->m_start.y = height;")
    a("            ray->m_start.z = c * radius;")
    for line in k.get("c0_tilt", [
        "float ts = std::sin(tilt);",
        "float tc = std::cos(tilt);"]):
        a("            " + line)
    a("            ray->m_velocity.x = s * (ts * velocity);")
    a("            ray->m_velocity.y = tc * velocity;")
    a("            ray->m_velocity.z = c * (ts * velocity);")
    a("            break;")
    a("        }")
    a("        case 1: {")
    for line in k.get("c1_head", [
        "spread = shape->m_spread;",
        "angle = spread * RandomFloat() - shape->m_spread;"]):
        a("            " + line)
    for line in k.get("c1_radius", [
        "float radius = 0.0f;",
        "if (shape->m_radius > 0.0001f) {",
        "    if (m_config & 0x80) {",
        "        radius = shape->m_radius;",
        "    } else {",
        "        radius = shape->m_radius * RandomFloat();",
        "    }",
        "}",
        "float tilt = shape->m_angle * RandomFloat();",
        "float ts = std::sin(tilt);",
        "float tc = std::cos(tilt);",
        "float x = ts * std::sin(angle);",
        "float z = ts * std::cos(angle);"]):
        a("            " + line)
    a("            ray->m_start.x = x * radius;")
    a("            ray->m_start.y = tc * radius;")
    a("            ray->m_start.z = z * radius;")
    a("            ray->m_velocity.x = x * velocity;")
    a("            ray->m_velocity.y = tc * velocity;")
    a("            ray->m_velocity.z = z * velocity;")
    a("            if (m_config & 0x20) {")
    a("                ray->m_velocity.x = -ray->m_velocity.x;")
    a("                ray->m_velocity.y = -ray->m_velocity.y;")
    a("                ray->m_velocity.z = -ray->m_velocity.z;")
    a("            }")
    a("            break;")
    a("        }")
    a("        case 2: {")
    for line in k.get("c2", [
        "float size = shape->m_radius;",
        "ray->m_start.x = size * RandomFloat() - 0.5f * size;",
        "size = shape->m_spread;",
        "ray->m_start.y = size * RandomFloat() - 0.5f * size;",
        "size = shape->m_angle;",
        "ray->m_start.z = size * RandomFloat() - 0.5f * size;"]):
        a("            " + line)
    a("            ray->m_velocity.x = 0.0f;")
    a("            ray->m_velocity.y = velocity;")
    a("            ray->m_velocity.z = 0.0f;")
    a("            break;")
    a("        }")
    a("        }")
    a("        if (!(m_config & 0x800)) {")
    a("            AssignVector(ray->m_start, fn_8040B67C(&m_worldMatrix, &ray->m_start));")
    a("            AssignVector(ray->m_velocity, fn_8040B618(&m_worldMatrix, &ray->m_velocity));")
    a("        }")
    a("        AssignVector(ray->m_end, ray->m_start);")
    a("        ray->m_cursor = desc->m_rayCommands;")
    a("    }")
    a("}")
    a("")
    a("")
    return "\n".join(L)

class FullRepl:
    pass

def V(**k):
    return k

VARIANTS = {
    "t_base": V(),
    "t_half05": V(halfsub="angle -= 0.5f * spread;"),
    "t_halfdiv": V(halfsub="angle -= spread / 2.0f;"),
    "t_rand05": V(randangle="angle = spread * RandomFloat() - 0.5f * shape->m_spread;"),
    "t_rdecl": V(c0_radius=[
        "float radius = 0.0f;",
        "float tilt = 0.0f;",
        "if (shape->m_radius > 0.0001f) {",
        "    float r = shape->m_radius;",
        "    if (!(m_config & 0x80)) {",
        "        r = shape->m_radius * RandomFloat();",
        "    }",
        "    radius = r;",
        "    tilt = shape->m_angle * (r / shape->m_radius);",
        "}"]),
    "t_rdecl2": V(c0_radius=[
        "float radius = 0.0f;",
        "float tilt = 0.0f;",
        "float r;",
        "if (shape->m_radius > 0.0001f) {",
        "    if (m_config & 0x80) {",
        "        r = shape->m_radius;",
        "    } else {",
        "        r = shape->m_radius * RandomFloat();",
        "    }",
        "    radius = r;",
        "    tilt = shape->m_angle * (r / shape->m_radius);",
        "}"]),
    "t_hmid": V(c0_sc=[
        "float s = std::sin(angle);",
        "float height = 0.0f;",
        "float c = std::cos(angle);",
        "if (shape->m_height > 0.0001f) {",
        "    height = shape->m_height * RandomFloat();",
        "}"]),
    "t_hfirst": V(c0_sc=[
        "float height = 0.0f;",
        "float s = std::sin(angle);",
        "float c = std::cos(angle);",
        "if (shape->m_height > 0.0001f) {",
        "    height = shape->m_height * RandomFloat();",
        "}"]),
    "t_c1nosp": V(c1_head=["angle = shape->m_spread * RandomFloat() - shape->m_spread;"]),
}

def _src():
    import os
    return open(os.path.join(os.path.dirname(os.path.dirname(os.path.abspath(__file__))) if "__file__" in globals() else ".", "src", "Game", "EffectRay.cpp"), encoding="utf-8").read()

REPL = {}
import os
_text = open(os.path.join("src", "Game", "EffectRay.cpp"), encoding="utf-8").read()
_i = _text.index(START)
_j = _text.index(END)
OLD = _text[_i:_j]
for name, k in VARIANTS.items():
    REPL[name] = [(OLD, gen(k))]

HMID = [
        "float s = std::sin(angle);",
        "float height = 0.0f;",
        "float c = std::cos(angle);",
        "if (shape->m_height > 0.0001f) {",
        "    height = shape->m_height * RandomFloat();",
        "}"]
V2 = {
    "u_hd": V(c0_sc=HMID, halfsub="angle -= spread / 2.0f;"),
    "u_hdc": V(c0_sc=HMID, halfsub="angle -= spread / 2.0f;", c1_head=["angle = shape->m_spread * RandomFloat() - shape->m_spread;"]),
    "u_hc": V(c0_sc=HMID, c1_head=["angle = shape->m_spread * RandomFloat() - shape->m_spread;"]),
}
for name, k in V2.items():
    REPL[name] = [(OLD, gen(k))]

# Round 3: on top of u_hd.
def c2_with(var, decl):
    return ([decl] if decl else []) + [
        var + " = shape->m_radius;",
        "ray->m_start.x = " + var + " * RandomFloat() - 0.5f * " + var + ";",
        var + " = shape->m_spread;",
        "ray->m_start.y = " + var + " * RandomFloat() - 0.5f * " + var + ";",
        var + " = shape->m_angle;",
        "ray->m_start.z = " + var + " * RandomFloat() - 0.5f * " + var + ";"]
C2 = {
    "own": (None, ["float spread;", "float angle;"]),
    "sp": (c2_with("spread", None), ["float spread;", "float angle;"]),
    "an": (c2_with("angle", None), ["float spread;", "float angle;"]),
    "szA": (c2_with("size", None), ["float size;", "float spread;", "float angle;"]),
    "szB": (c2_with("size", None), ["float spread;", "float angle;", "float size;"]),
}
R0 = {
    "rdef": None,
    "rpre": [
        "float radius = 0.0f;",
        "float tilt = 0.0f;",
        "float r;",
        "if (shape->m_radius > 0.0001f) {",
        "    if (m_config & 0x80) {",
        "        r = shape->m_radius;",
        "    } else {",
        "        r = shape->m_radius * RandomFloat();",
        "    }",
        "    radius = r;",
        "    tilt = shape->m_angle * (r / shape->m_radius);",
        "}"],
}
C1 = {
    "c1sh": None,
    "c1own": ["float spread = shape->m_spread;", "angle = spread * RandomFloat() - shape->m_spread;"],
}
RA = {
    "ra05": None,
    "radiv": "angle = spread * RandomFloat() - shape->m_spread / 2.0f;",
}
for (c2n, (c2, lt)), (rn, r0), (c1n, c1), (ran, ra) in itertools.product(C2.items(), R0.items(), C1.items(), RA.items()):
    k = dict(c0_sc=HMID, halfsub="angle -= spread / 2.0f;", looptop=lt)
    if c2: k["c2"] = c2
    if r0: k["c0_radius"] = r0
    if c1: k["c1_head"] = c1
    if ra: k["randangle"] = ra
    REPL["w_" + "_".join([c2n, rn, c1n, ran])] = [(OLD, gen(k))]

# Round 4: on top of w_szB_rdef_c1own_radiv, the CSE'd m_radius load of case 0 (retail loads it
# straight into r's register).
BASE4 = dict(c0_sc=HMID, halfsub="angle -= spread / 2.0f;", looptop=["float spread;", "float angle;", "float size;"],
             c2=c2_with("size", None), c1_head=C1["c1own"], randangle=RA["radiv"])
def r0(*lines):
    return list(lines)
R4 = {
    "tern": r0("float radius = 0.0f;", "float tilt = 0.0f;", "if (shape->m_radius > 0.0001f) {",
               "    float r = (m_config & 0x80) ? shape->m_radius : shape->m_radius * RandomFloat();",
               "    radius = r;", "    tilt = shape->m_angle * (r / shape->m_radius);", "}"),
    "rfirst": r0("float r;", "float radius = 0.0f;", "float tilt = 0.0f;", "if (shape->m_radius > 0.0001f) {",
               "    if (m_config & 0x80) {", "        r = shape->m_radius;", "    } else {", "        r = shape->m_radius * RandomFloat();", "    }",
               "    radius = r;", "    tilt = shape->m_angle * (r / shape->m_radius);", "}"),
    "rt_first": r0("float radius = 0.0f;", "float tilt = radius;", "if (shape->m_radius > 0.0001f) {",
               "    float r;", "    if (m_config & 0x80) {", "        r = shape->m_radius;", "    } else {", "        r = shape->m_radius * RandomFloat();", "    }",
               "    radius = r;", "    tilt = shape->m_angle * (r / shape->m_radius);", "}"),
    "tilt_r": r0("float radius = 0.0f;", "float tilt = 0.0f;", "if (shape->m_radius > 0.0001f) {",
               "    float r;", "    if (m_config & 0x80) {", "        r = shape->m_radius;", "    } else {", "        r = shape->m_radius * RandomFloat();", "    }",
               "    tilt = shape->m_angle * (r / shape->m_radius);", "    radius = r;", "}"),
    "rdiv": r0("float radius = 0.0f;", "float tilt = 0.0f;", "if (shape->m_radius > 0.0001f) {",
               "    float r;", "    if (m_config & 0x80) {", "        r = shape->m_radius;", "    } else {", "        r = shape->m_radius * RandomFloat();", "    }",
               "    radius = r;", "    tilt = r / shape->m_radius * shape->m_angle;", "}"),
    "rad_direct": r0("float radius = 0.0f;", "float tilt = 0.0f;", "if (shape->m_radius > 0.0001f) {",
               "    if (m_config & 0x80) {", "        radius = shape->m_radius;", "    } else {", "        radius = shape->m_radius * RandomFloat();", "    }",
               "    tilt = shape->m_angle * (radius / shape->m_radius);", "}"),
}
LT4 = {
    "lt": ["float spread;", "float angle;", "float size;"],
    "lt_r": ["float spread;", "float angle;", "float size;", "float r;"],
    "lt_rt": ["float spread;", "float angle;", "float size;", "float radius;", "float tilt;"],
}
R4LT = {
    "lt_r": r0("float radius = 0.0f;", "float tilt = 0.0f;", "if (shape->m_radius > 0.0001f) {",
               "    if (m_config & 0x80) {", "        r = shape->m_radius;", "    } else {", "        r = shape->m_radius * RandomFloat();", "    }",
               "    radius = r;", "    tilt = shape->m_angle * (r / shape->m_radius);", "}"),
    "lt_rt": r0("radius = 0.0f;", "tilt = 0.0f;", "if (shape->m_radius > 0.0001f) {",
               "    float r;", "    if (m_config & 0x80) {", "        r = shape->m_radius;", "    } else {", "        r = shape->m_radius * RandomFloat();", "    }",
               "    radius = r;", "    tilt = shape->m_angle * (r / shape->m_radius);", "}"),
}
for n, lines in R4.items():
    k = dict(BASE4); k["c0_radius"] = lines
    REPL["x_" + n] = [(OLD, gen(k))]
for n, lines in R4LT.items():
    k = dict(BASE4); k["c0_radius"] = lines; k["looptop"] = LT4[n]
    REPL["x_" + n] = [(OLD, gen(k))]
