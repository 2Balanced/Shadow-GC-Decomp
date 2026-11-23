"""Wave 40: fn_8004A684 keeps the saved image path in r29 (the register of the message pointer)."""
SCORE = ["fn_8004A684"]
F = 'extern "C" bool fn_8004A684(TMessageDisp *self, const char *name) {\n'
A = """    char path[0x100];
    char *previous = fn_80051810("./fonts/");
    fn_80473460();
    fn_80051830(previous);
"""
D = """    UnkMessage_80049D38 *message;
    int i;
    UnkMessage_80049D38 *messages;
"""
CALLS = """    previous = fn_80051810("./fonts/");
    fn_80473460();
    fn_80051830(previous);
"""
P = "    char *previous;\n"
REPL = {
  "inl": [(F, 'inline void SetFontPath_() {\n    char *previous = fn_80051810("./fonts/");\n    fn_80473460();\n    fn_80051830(previous);\n}\n' + F), (A, "    char path[0x100];\n    SetFontPath_();\n")],
  "top_mpi": [(D, ""), (A, "    char path[0x100];\n    UnkMessage_80049D38 *message;\n" + P + "    int i;\n    UnkMessage_80049D38 *messages;\n" + CALLS)],
  "top_mip": [(D, ""), (A, "    char path[0x100];\n    UnkMessage_80049D38 *message;\n    int i;\n" + P + "    UnkMessage_80049D38 *messages;\n" + CALLS)],
  "top_mimp": [(D, ""), (A, "    char path[0x100];\n    UnkMessage_80049D38 *message;\n    int i;\n    UnkMessage_80049D38 *messages;\n" + P + CALLS)],
  "top_pmim": [(D, ""), (A, "    char path[0x100];\n" + P + "    UnkMessage_80049D38 *message;\n    int i;\n    UnkMessage_80049D38 *messages;\n" + CALLS)],
  "scope": [(A, "    char path[0x100];\n    {\n" + A.split("\n", 1)[1].replace("    ", "        ") + "    }\n")],
  "nested": [(A, '    char path[0x100];\n    char *previous = fn_80051810("./fonts/");\n    fn_80473460();\n    fn_80051830(previous);\n'.replace("    fn_80051830(previous);", "    fn_80051830(previous);"))],
}
