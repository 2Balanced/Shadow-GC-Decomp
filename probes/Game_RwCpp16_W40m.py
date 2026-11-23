"""Wave 40 (r2), RwCpp16 round 13: the pragmas around the UV keyframe callbacks dropped (scheduling
is on by default); fn_804533E0 with non-const keys like its siblings."""
SCORE = ["fn_80453350", "fn_804533E0", "fn_80453500", "fn_804536B4", "fn_80453770"]
def drop(sig):
    return ("#pragma scheduling on\n" + sig, sig)
REPL = {
 "base": [("#pragma scheduling on\nvoid fn_80453350", "#pragma scheduling on\nvoid fn_80453350")],
 "clean": [drop("void fn_80453350(float *out, float *from, float *to, float time)\n"),
           drop("void fn_80453500(float *out, float *from, float *to)\n"),
           drop("void fn_804536B4(float *out, float *from, float *to, float time)\n"),
           drop("void fn_80453770(float *out, float *from, float *to, float ratio)\n")],
 "clean33e0": [drop("void fn_80453350(float *out, float *from, float *to, float time)\n"),
           drop("void fn_80453500(float *out, float *from, float *to)\n"),
           drop("void fn_804536B4(float *out, float *from, float *to, float time)\n"),
           drop("void fn_80453770(float *out, float *from, float *to, float ratio)\n"),
           ("#pragma scheduling off\nvoid fn_804533E0(float *out, const float *from, const float *to, float ratio)\n", "void fn_804533E0(float *out, float *from, float *to, float ratio)\n"),
           ("void fn_804533E0(float *, const float *, const float *, float);", "void fn_804533E0(float *, float *, float *, float);")],
}
