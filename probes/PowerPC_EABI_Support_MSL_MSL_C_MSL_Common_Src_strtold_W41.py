"""Wave 41: __strtold with copy propagation off and the parameters read through local copies
declared after the locals (retail: overflow, context and read in r18-r16 below the locals)."""
F = "__strtold"
SCORE = [F]
SIG = "long double __strtold(int maxWidth, ReadFn read, void *context, int *charsScanned, int *overflow) {\n"
SIG2 = "long double __strtold(int maxWidth, ReadFn read_, void *context_, int *charsScanned, int *overflow_) {\n"
LAST = "    short hexExponent = 0;\n"
OFF_HEAD = "#pragma opt_propagation off\n"
import re as _re
_s = open("src/PowerPC_EABI_Support/MSL/MSL_C/MSL_Common/Src/strtold.c", encoding="utf-8").read()
_i = _s.index(SIG)
_j = _s.index("\n}\n", _i) + 3
BODY = _s[_i:_j]
def v(copies, off=True):
    b = BODY.replace(SIG, SIG2).replace(LAST, LAST + copies)
    if off:
        b = OFF_HEAD + b + "#pragma opt_propagation reset\n"
    return [(BODY, b)]
REPL = {
    "base": [],
    "off_ovf_ctx_rd": v("    int *overflow = overflow_;\n    void *context = context_;\n    ReadFn read = read_;\n"),
    "off_rd_ctx_ovf": v("    ReadFn read = read_;\n    void *context = context_;\n    int *overflow = overflow_;\n"),
    "on_ovf_ctx_rd": v("    int *overflow = overflow_;\n    void *context = context_;\n    ReadFn read = read_;\n", False),
}
