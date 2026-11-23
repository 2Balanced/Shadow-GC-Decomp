"""Wave 40 (r2), RwCpp16 round 8: the UV keyframe to matrix copies fn_804532E4 and fn_804535C8 with a
non-const key (each element's store keeps the next load behind it)."""
SCORE = ["fn_804532E4", "fn_804535C8"]
REPL = {
 "base": [("void fn_804532E4(float *, const float *);", "void fn_804532E4(float *, const float *);")],
 "nc": [("void fn_804532E4(float *, const float *);", "void fn_804532E4(float *, float *);"),
        ("void fn_804535C8(float *, const float *);", "void fn_804535C8(float *, float *);"),
        ("void fn_804532E4(float *dest, const float *source)", "void fn_804532E4(float *dest, float *source)"),
        ("void fn_804535C8(float *out, const float *key)", "void fn_804535C8(float *out, float *key)")],
}
