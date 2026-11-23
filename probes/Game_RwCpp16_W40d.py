"""Wave 40 (r2), RwCpp16 round 4: the angle blends with scheduling on and non-const key pointers
(stores to out then keep the loads of the next element behind them)."""
SCORE = ["fn_80453770", "fn_804536B4"]
P0 = "void fn_804536B4(float *, const float *, const float *, float);\nvoid fn_80453770(float *, const float *, const float *, float);\n"
P1 = "void fn_804536B4(float *, float *, float *, float);\nvoid fn_80453770(float *, float *, float *, float);\n"
F7 = "#pragma scheduling off\nvoid fn_80453770(float *out, const float *from, const float *to, float ratio)\n"
F6 = "#pragma scheduling off\nvoid fn_804536B4(float *out, const float *from, const float *to, float time)\n"
REPL = {
 "base": [(P0, P0)],
 "nc_on": [(P0, P1), (F7, "#pragma scheduling on\nvoid fn_80453770(float *out, float *from, float *to, float ratio)\n"),
           (F6, "#pragma scheduling on\nvoid fn_804536B4(float *out, float *from, float *to, float time)\n")],
 "nc_off": [(P0, P1), (F7, "#pragma scheduling off\nvoid fn_80453770(float *out, float *from, float *to, float ratio)\n"),
           (F6, "#pragma scheduling off\nvoid fn_804536B4(float *out, float *from, float *to, float time)\n")],
}
