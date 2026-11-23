"""Wave 40 (r2), RwCpp16 round 3: the wrapped angle blend fn_80453770 (retail keeps the delta in the
register of to[2] and from[2] in f3) and fn_804536B4."""
SCORE = ["fn_80453770", "fn_804536B4"]
F0 = """#pragma scheduling off
void fn_80453770(float *out, const float *from, const float *to, float ratio)
{
    float delta = to[2] - from[2];
"""
B = "void fn_80453770(float *out, const float *from, const float *to, float ratio)\n{\n"
REPL = {
 "base": [(F0, F0)],
 "sched_on": [(F0, "#pragma scheduling on\n" + B + "    float delta = to[2] - from[2];\n")],
 "sched_on_sep": [(F0, "#pragma scheduling on\n" + B + "    float delta;\n    delta = to[2] - from[2];\n")],
 "sched_on_minus": [(F0, "#pragma scheduling on\n" + B + "    float delta = to[2];\n    delta -= from[2];\n")],
 "sched_on_start": [(F0, "#pragma scheduling on\n" + B + "    float start = from[2];\n    float delta = to[2] - start;\n")],
 "minus": [(F0, "#pragma scheduling off\n" + B + "    float delta = to[2];\n    delta -= from[2];\n")],
 "nc_sched_on": [(F0, "#pragma scheduling on\nvoid fn_80453770(float *out, float *from, float *to, float ratio)\n{\n    float delta = to[2] - from[2];\n")],
}
