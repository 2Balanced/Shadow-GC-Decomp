"""fn_80415530 of Game/MotionHalfKeyTrack.cpp (wave 39, second pass): a hill climb over the
declaration order of the twelve locals. BASE is the current best order; the table holds every order
that moves one name to another place. Set the environment variable HK_BASE to continue from
another order (names separated by commas)."""
import os
SYMBOLS = {"fn_80415530": 'extern "C" void fn_80415530(float *value, float *slope, const HalfKey *prev, const HalfKey *next,'}
ASG = """    s = time - prev->m_time;
    s2 = s * s;
    inv = 1.0f / (next->m_time - prev->m_time);
    inv2 = inv * inv;
    s3 = s2 * s;
    a = inv2 * s3;
    b = s2 * inv;
    d = 2.0f * a * inv;
    c = 3.0f * s2 * inv2;
    c6 = 6.0f * s2 * inv2 * inv;
    d6 = 6.0f * s * inv2;
    f = 2.0f * s * inv;
    h11 = a - b;
    h10 = s + (h11 - b);
    h00 = 1.0f + (d - c);
    h01 = -d + c;
    dh00 = c6 - d6;
    dh01 = -c6 + d6;
    dh11 = c - f;
    dh10 = 1.0f + (dh11 - f);
    *value = prev->m_value * h00 + next->m_value * h01 + prev->m_out * h10 + next->m_in * h11;
    *slope = prev->m_value * dh00 + next->m_value * dh01 + prev->m_out * dh10 + next->m_in * dh11;
"""
# HK_LEN=1 gives the key distance a local of its own (as fn_804156D8 has), HK_LEN=2 computes it
# before s2.
if os.environ.get("HK_LEN") == "1":
    ASG = ASG.replace("    inv = 1.0f / (next->m_time - prev->m_time);\n", "    len = next->m_time - prev->m_time;\n    inv = 1.0f / len;\n")
elif os.environ.get("HK_LEN") == "2":
    ASG = ASG.replace("    s2 = s * s;\n    inv = 1.0f / (next->m_time - prev->m_time);\n", "    len = next->m_time - prev->m_time;\n    inv = 1.0f / len;\n    s2 = s * s;\n")
BASE = os.environ.get("HK_BASE", "s,s2,s3,a,d,c,c6,d6,f,inv2,b,inv").split(",")
def body(names):
    hs = "" if "h00" in names else "    float h00, h01, h10, h11;\n    float dh00, dh01, dh10, dh11;\n"
    return "                            float time) {\n" + hs + "".join("    float %s;\n" % n for n in names) + ASG
B = {"base": body(BASE)}
seen = {tuple(BASE)}
for i, n in enumerate(BASE):
    rest = BASE[:i] + BASE[i + 1:]
    for j in range(len(BASE)):
        o = rest[:j] + [n] + rest[j:]
        if tuple(o) not in seen:
            seen.add(tuple(o))
            B["m_" + ",".join(o)] = body(o)
BODIES = {"fn_80415530": B}
# HK_RANDOM="count,seed" replaces the table with random orders (to find a start for the climb).
if os.environ.get("HK_RANDOM"):
    import random
    count, seed = os.environ["HK_RANDOM"].split(",")
    rng = random.Random(int(seed))
    B = {}
    for _ in range(int(count)):
        o = BASE[:]
        rng.shuffle(o)
        B["m_" + ",".join(o)] = body(o)
    BODIES = {"fn_80415530": B}
# HK_TWO=1 replaces the table with every order two moves away from BASE.
if os.environ.get("HK_TWO"):
    def moves(order):
        out = []
        for i, n in enumerate(order):
            rest = order[:i] + order[i + 1:]
            for j in range(len(order)):
                out.append(rest[:j] + [n] + rest[j:])
        return out
    seen = set()
    B = {}
    for o1 in moves(BASE):
        for o2 in moves(o1):
            if tuple(o2) not in seen:
                seen.add(tuple(o2))
                B["m_" + ",".join(o2)] = body(o2)
    BODIES = {"fn_80415530": B}
