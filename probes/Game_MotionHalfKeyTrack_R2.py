"""fn_804156D8 of Game/MotionHalfKeyTrack.cpp (wave 39, second pass): declaration orders of the
locals (retail: inv f12, inv2 f10, b f11; GC/2.7 as written f10, f11, f12)."""
import itertools
SYMBOLS = {"fn_804156D8": 'extern "C" float fn_804156D8(const HalfKey *prev, const HalfKey *next, float time) {'}
ASG = """    s = time - prev->m_time;
    len = next->m_time - prev->m_time;
    inv = 1.0f / len;
    s2 = s * s;
    inv2 = inv * inv;
    s3 = s2 * s;
    a = inv2 * s3;
    b = s2 * inv;
    d = 2.0f * a * inv;
    c = 3.0f * s2 * inv2;
    h11 = a - b;
    h10 = s + (h11 - b);
    h00 = 1.0f + (d - c);
    h01 = -d + c;
    return prev->m_value * h00 + next->m_value * h01 + prev->m_out * h10 + next->m_in * h11;
"""
OTHERS = ["s", "len", "s2", "s3", "a", "d", "c"]
def decl(names):
    return "    float h00, h01, h10, h11;\n" + "".join("    float %s;\n" % n for n in names)
B = {}
for perm in itertools.permutations(["inv", "inv2", "b"]):
    B["last_" + "_".join(perm)] = decl(OTHERS + list(perm)) + ASG
    B["first_" + "_".join(perm)] = decl(list(perm) + OTHERS) + ASG
BODIES = {"fn_804156D8": B}
