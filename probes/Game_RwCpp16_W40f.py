"""Wave 40 (r2), RwCpp16 round 6: the UV keyframe callbacks fn_80453350 (interpolate), fn_80453500
(add) and fn_8045345C (mul recip) with non-const key pointers and the scheduler on."""
SCORE = ["fn_80453350", "fn_80453500", "fn_8045345C"]
P = [("void fn_80453350(float *, const float *, const float *, float);", "void fn_80453350(float *, float *, float *, float);"),
     ("void fn_80453500(float *, const float *, const float *);", "void fn_80453500(float *, float *, float *);")]
P2 = [("void fn_8045345C(float *, const float *);", "void fn_8045345C(float *, float *);")]
F1 = "#pragma scheduling off\nvoid fn_80453350(float *out, const float *from, const float *to, float time)\n"
F1n = "void fn_80453350(float *out, float *from, float *to, float time)\n"
F2 = """#pragma scheduling off
void fn_80453500(float *out, const float *from, const float *to)
{
    float first = *(volatile f32 *)(from + 3) * *(volatile f32 *)(to + 4);
    out[2] = from[2] * to[2] + first;
    float second = *(volatile f32 *)(from + 3) * *(volatile f32 *)(to + 5);
    out[3] = from[2] * to[3] + second;
    float third = *(volatile f32 *)(from + 5) * *(volatile f32 *)(to + 4);
    out[4] = from[4] * to[2] + third;
    float fourth = *(volatile f32 *)(from + 5) * *(volatile f32 *)(to + 5);
    out[5] = from[4] * to[3] + fourth;
"""
F2n = """#pragma scheduling on
void fn_80453500(float *out, float *from, float *to)
{
    out[2] = from[2] * to[2] + from[3] * to[4];
    out[3] = from[2] * to[3] + from[3] * to[5];
    out[4] = from[4] * to[2] + from[5] * to[4];
    out[5] = from[4] * to[3] + from[5] * to[5];
"""
F3 = "void fn_8045345C(float *out, const float *from)\n"
REPL = {
 "base": P[:0] + [(F1, F1)],
 "nc": P + [(F1, "#pragma scheduling on\n" + F1n), (F2, F2n)],
 "nc_off": P + [(F1, "#pragma scheduling off\n" + F1n), (F2, F2n.replace("scheduling on", "scheduling off"))],
 "nc3": P2 + [(F3, "void fn_8045345C(float *out, float *from)\n")],
}
