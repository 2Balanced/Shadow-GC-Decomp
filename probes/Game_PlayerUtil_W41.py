"""Wave 41: fn_8009339C with copy propagation off (retail copies the fields pointer to r4 per
statement)."""
SCORE = ["fn_8009339C"]
F = "extern \"C\" void fn_8009339C(boost::shared_ptr<PlayerBase> *player, const Vec3f *up) {\n"
N = "/* Whether the player is on ground no steeper than s_maxSlope. */\n"
PRAG = [(F, "#pragma opt_propagation off\n" + F), (N, "#pragma opt_propagation reset\n" + N)]
BODY = """    fn_800091D0(&Fields(player->px)->m_groundUp, up);
    UnkPlayerFields_800930AC *fields = Fields(player->px);
    fn_8007A34C(&fields->field_0x1D4, &(const Quatf &)fn_800934FC(&fields->m_groundUp));
    Fields(player->operator->())->field_0x1E4 =
        fn_8040B228(&Fields(player->operator->())->field_0x1D4);
    float rate = 15.0f * (lbl_805FAD38 / 180.0f) /
                 fn_80015D68(fn_8040CF90(&fields->field_0x17C, &fields->m_groundUp));
    rate = fn_800704AC(rate, s_maxTilt);
    fn_8007A34C(&fields->field_0x188,
                &(const Quatf &)fn_8040C3EC(&fields->field_0x188, &fields->field_0x1D4, rate));
"""


def v(*extra, prag=True):
    return list(extra) + (PRAG if prag else [])


B2 = BODY.replace("    float rate = 15.0f * (lbl_805FAD38 / 180.0f) /\n                 fn_80015D68(fn_8040CF90(&fields->field_0x17C, &fields->m_groundUp));\n",
                  "    UnkPlayerFields_800930AC *f = fields;\n    float rate = 15.0f * (lbl_805FAD38 / 180.0f) /\n                 fn_80015D68(fn_8040CF90(&f->field_0x17C, &f->m_groundUp));\n")
B2 = B2.replace("    fn_8007A34C(&fields->field_0x188,\n                &(const Quatf &)fn_8040C3EC(&fields->field_0x188, &fields->field_0x1D4, rate));\n",
                "    UnkPlayerFields_800930AC *g = fields;\n    fn_8007A34C(&g->field_0x188,\n                &(const Quatf &)fn_8040C3EC(&g->field_0x188, &g->field_0x1D4, rate));\n")
REPL = {
    "prag": v(),
    "prag_copies": v((BODY, B2)),
    "copies": v((BODY, B2), prag=False),
}
HELP = """inline float TiltRate_(UnkPlayerFields_800930AC *f) {
    return 15.0f * (lbl_805FAD38 / 180.0f) / fn_80015D68(fn_8040CF90(&f->field_0x17C, &f->m_groundUp));
}

inline void TiltTo_(UnkPlayerFields_800930AC *f, float rate) {
    fn_8007A34C(&f->field_0x188, &(const Quatf &)fn_8040C3EC(&f->field_0x188, &f->field_0x1D4, rate));
}

"""
B3 = BODY.replace("    float rate = 15.0f * (lbl_805FAD38 / 180.0f) /\n                 fn_80015D68(fn_8040CF90(&fields->field_0x17C, &fields->m_groundUp));\n",
                  "    float rate = TiltRate_(fields);\n")
B3 = B3.replace("    fn_8007A34C(&fields->field_0x188,\n                &(const Quatf &)fn_8040C3EC(&fields->field_0x188, &fields->field_0x1D4, rate));\n",
                "    TiltTo_(fields, rate);\n")
HELP2 = """inline float Dot_(UnkPlayerFields_800930AC *f) {
    return fn_80015D68(fn_8040CF90(&f->field_0x17C, &f->m_groundUp));
}

inline const Quatf &Slerp_(UnkPlayerFields_800930AC *f, float rate) {
    return fn_8040C3EC(&f->field_0x188, &f->field_0x1D4, rate);
}

"""
B4 = BODY.replace("                 fn_80015D68(fn_8040CF90(&fields->field_0x17C, &fields->m_groundUp));\n", "                 Dot_(fields);\n")
B4 = B4.replace("    fn_8007A34C(&fields->field_0x188,\n                &(const Quatf &)fn_8040C3EC(&fields->field_0x188, &fields->field_0x1D4, rate));\n",
                "    fn_8007A34C(&fields->field_0x188, &Slerp_(fields, rate));\n")
REPL.update({
    "prag_help": v((F, HELP + F), (BODY, B3)),
    "help": v((F, HELP + F), (BODY, B3), prag=False),
    "prag_help2": v((F, HELP2 + F), (BODY, B4)),
    "help2": v((F, HELP2 + F), (BODY, B4), prag=False),
})
