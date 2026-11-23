"""Wave 41: fn_801EEB2C (retail: the pooled .data base in r4, the guards' strings and the jump
table in r5)."""
SCORE = ["fn_801EEB2C"]
H = 'extern "C" const char *fn_801EEB2C(UnkLabelQuery_800CBC20 *query) {\n'
P0 = "#pragma opt_propagation off\n"
CHK = """    if (s_shape < 0 || s_shape > 2) {
        s_shape = 0;
    }
"""
G = """    static const char *effectNumber = "EffectNumber";
    static const char *doNotUse = "DoNotUse";
"""
Q = "    Query *q = reinterpret_cast<Query *>(query);\n"
REPL = {
  "prag": [(H, P0 + H)],
  "q_first": [(CHK + G + Q, Q + CHK + G)],
  "prag_q_first": [(H, P0 + H), (CHK + G + Q, Q + CHK + G)],
  "q_between": [(CHK + G + Q, CHK + Q + G)],
  "idx_local": [(Q, Q + "    s32 value = q->value;\n")],
}
