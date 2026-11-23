"""Wave 40: fn_80049BC0 keeps the branch in r31, ahead of the other parameters (a local of its
own)."""
SCORE = ["fn_80049BC0"]
P = "UnkMessage_80049D38 *fn_80049BC0(TMessageDisp *self, s32 id, s32 branch, s32 index, float x, float y, s32 *voice);"
S = 'extern "C" UnkMessage_80049D38 *fn_80049BC0(TMessageDisp *self, s32 id, s32 branch, s32 index,\n                                            float x, float y, s32 *voice) {\n'
B = """    const u16 *text = fn_80049D38(self, id, branch, index, 0, voice);
    if (text == 0 && branch != 0) {
        branch = 0;
        text = fn_80049D38(self, id, branch, index, 0, voice);
    }
"""
T = "    return self->Find(id, branch, index);\n"
def v(ptype, decl, name="b"):
    out = []
    if ptype != "s32":
        out += [(S, S.replace("s32 branch", ptype + " branch"))]
    out += [(S.split("\n")[1] + "\n", S.split("\n")[1] + "\n" + decl), (B, B.replace("branch", name).replace(decl, "")), (T, T.replace("branch", name))]
    return out
REPL = {
  "local_s32": v("s32", "    s32 b = branch;\n"),
  "p_int": v("int", "    s32 b = branch;\n"),
  "p_u32": v("u32", "    s32 b = branch;\n"),
  "p_bool": v("bool", "    s32 b = branch;\n"),
  "p_u8": v("u8", "    s32 b = branch;\n"),
  "l_int": v("s32", "    int b = branch;\n"),
  "l_u32": v("s32", "    u32 b = branch;\n"),
}

def w(b): return [(B, b)]
REPL = {
  "arg_assign": w("""    const u16 *text = fn_80049D38(self, id, branch, index, 0, voice);
    if (text == 0 && branch != 0) {
        text = fn_80049D38(self, id, branch = 0, index, 0, voice);
    }
"""),
  "after": w("""    const u16 *text = fn_80049D38(self, id, branch, index, 0, voice);
    if (text == 0 && branch != 0) {
        text = fn_80049D38(self, id, 0, index, 0, voice);
        branch = 0;
    }
"""),
  "register": [(S, S.replace("s32 branch", "register s32 branch"))],
  "text_first": w("""    const u16 *text;
    text = fn_80049D38(self, id, branch, index, 0, voice);
    if (text == 0 && branch != 0) {
        branch = 0;
        text = fn_80049D38(self, id, branch, index, 0, voice);
    }
"""),
  "nested_if": w("""    const u16 *text = fn_80049D38(self, id, branch, index, 0, voice);
    if (text == 0) {
        if (branch != 0) {
            branch = 0;
            text = fn_80049D38(self, id, branch, index, 0, voice);
        }
    }
"""),
  "local_decl_first": [(S.split("\n")[1] + "\n", S.split("\n")[1] + "\n    s32 b;\n"), (B, "    b = branch;\n" + B.replace("branch", "b").replace("    b = b;\n", "")), (T, T.replace("branch", "b"))],
  "loop": w("""    const u16 *text;
    while ((text = fn_80049D38(self, id, branch, index, 0, voice)) == 0 && branch != 0) {
        branch = 0;
    }
"""),
}

H = S.split("\n")[1] + "\n"
def loc(decl, bn="b", idx="index", vo="voice"):
    b = B.replace("branch", bn).replace("index", idx).replace("voice", vo)
    return [(H, H + decl), (B, b), (T, T.replace("branch", bn).replace("index", idx))]
REPL = {
  "L_b_v_i": loc("    s32 b = branch;\n    int *v = (int *)voice;\n    int i = index;\n", idx="i", vo="(s32 *)v"),
  "L_b_i_v": loc("    s32 b = branch;\n    int i = index;\n    int *v = (int *)voice;\n", idx="i", vo="(s32 *)v"),
  "L_b_i": loc("    s32 b = branch;\n    int i = index;\n", idx="i"),
  "L_b_v": loc("    s32 b = branch;\n    int *v = (int *)voice;\n", vo="(s32 *)v"),
  "L_b_vv": loc("    s32 b = branch;\n    void *v = voice;\n", vo="(s32 *)v"),
  "L_b_vv_i": loc("    s32 b = branch;\n    void *v = voice;\n    int i = index;\n", idx="i", vo="(s32 *)v"),
}

