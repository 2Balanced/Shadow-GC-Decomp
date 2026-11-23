"""HandleUser's constructor (wave 40): a basic-block boundary between the vtable store and the two
members, so that the scheduler cannot lift the constants above the store."""
O = "HandleUser::HandleUser() {\n    field_0x4 = 0;\n    field_0x6 = -1;\n}\n"
B = "    field_0x4 = 0;\n    field_0x6 = -1;\n"
def v(body, pre=""):
    return [(O, pre + "HandleUser::HandleUser() {\n" + body + "}\n")]
REPL = {
 "nop_ret": v("    Nop40();\n" + B, "static inline void Nop40() {\n    return;\n}\n\n"),
 "dowhile": v("    do {\n" + B + "    } while (0);\n"),
 "goto": v("    goto set;\nset:\n" + B),
 "label": v("set:\n" + B + "    if (0) goto set;\n"),
 "for1": v("    for (int i = 0; i < 1; i++) {\n" + B + "    }\n"),
 "switch": v("    switch (0) {\n    default:\n" + B + "    }\n"),
 "init_ret": v("    Init40(this);\n", "static inline void Init40(HandleUser *self) {\n    self->field_0x4 = 0;\n    self->field_0x6 = -1;\n    return;\n}\n\n"),
 "init_if": v("    Init40(this);\n", "static inline void Init40(HandleUser *self) {\n    if (self == 0) {\n        return;\n    }\n    self->field_0x4 = 0;\n    self->field_0x6 = -1;\n}\n\n"),
 "ret_end": v(B + "    return;\n"),
 "block": v("    {\n" + B + "    }\n"),
 "try": v("    try {\n" + B + "    } catch (...) {\n    }\n"),
 "while1": v("    while (1) {\n" + B + "        break;\n    }\n"),
}
SCORE = ["__ct__10HandleUserFv"]
