"""Source shapes for AudioGCN::vf_0x50 (wave 40): retail keeps the group's slot in r31 and the
sign-extended voice in r30; GC/2.7 the other way round."""
SYMBOLS = {"vf_0x50__8AudioGCNFScUcScSc": "void AudioGCN::vf_0x50(s8 group, u8 voice, s8 bank, s8 program) {"}
XTRA = {"vf_0x50__8AudioGCNFScUcScSc": """inline void *Group_(AudioGCN *a, s8 g) { return a->m_groups[g]; }
inline int Status_(u8 voice, int kind) { return (s8)voice | kind; }
inline void Prog_(AudioGCN *a, s8 group, s8 voice, s8 bank, s8 program) {
    UnkAudioMessage_8043F41C message;
    fn_8043F41C(a->m_groups[group], &message, voice | 0xB0, 0x20, bank);
    fn_8043F35C(a->m_groups[group], &message, 0);
    fn_8043F41C(a->m_groups[group], &message, voice | 0xC0, program);
    fn_8043F35C(a->m_groups[group], &message, 0);
}
inline void Bank_(AudioGCN *a, s8 group, s8 voice, s8 bank) {
    UnkAudioMessage_8043F41C message;
    fn_8043F41C(a->m_groups[group], &message, voice | 0xB0, 0x20, bank);
    fn_8043F35C(a->m_groups[group], &message, 0);
}
inline void Program_(AudioGCN *a, s8 group, s8 voice, s8 program) {
    UnkAudioMessage_8043F41C message;
    fn_8043F41C(a->m_groups[group], &message, voice | 0xC0, program);
    fn_8043F35C(a->m_groups[group], &message, 0);
}
inline s8 Voice_(u8 v) { return v; }
"""}
M = "    UnkAudioMessage_8043F41C message;\n"
A = "    fn_8043F41C(%s, &message, %s | 0xB0, 0x20, bank);\n    fn_8043F35C(%s, &message, 0);\n"
B = "    fn_8043F41C(%s, &message, %s | 0xC0, program);\n    fn_8043F35C(%s, &message, 0);\n"
G = "m_groups[group]"


def body(g, v, pre=""):
    return pre + M + A % (g, v, g) + B % (g, v, g)


BODIES = {"vf_0x50__8AudioGCNFScUcScSc": {
    "slot_first": body("*slot", "(s8)voice", "    void **slot = &m_groups[group];\n"),
    "slot_voice": body("*slot", "v", "    void **slot = &m_groups[group];\n    s8 v = voice;\n"),
    "voice_slot": body("*slot", "v", "    s8 v = voice;\n    void **slot = &m_groups[group];\n"),
    "getter": body("Group_(this, group)", "(s8)voice"),
    "status": M + "    fn_8043F41C(m_groups[group], &message, Status_(voice, 0xB0), 0x20, bank);\n"
        "    fn_8043F35C(m_groups[group], &message, 0);\n"
        "    fn_8043F41C(m_groups[group], &message, Status_(voice, 0xC0), program);\n"
        "    fn_8043F35C(m_groups[group], &message, 0);\n",
    "int_group": body("m_groups[g]", "(s8)voice", "    int g = group;\n"),
    "char_cast": body(G, "(char)voice"),
    "inl_all": "    Prog_(this, group, voice, bank, program);\n",
    "inl_two": "    Bank_(this, group, voice, bank);\n    Program_(this, group, voice, program);\n",
    "inl_v": body(G, "v", "    s8 v = Voice_(voice);\n"),
    "inl_v2": body(G, "Voice_(voice)"),
    "assign": body(G, "(s8)voice", "    voice = (s8)voice;\n"),
    "this_local": body("self->m_groups[group]", "(s8)voice", "    AudioGCN *self = this;\n"),
    "status_local": "    int status = (s8)voice | 0xB0;\n" + M +
        "    fn_8043F41C(m_groups[group], &message, status, 0x20, bank);\n"
        "    fn_8043F35C(m_groups[group], &message, 0);\n" + B % (G, "(s8)voice", G),
    "groups_local": body("groups[group]", "(s8)voice", "    void **groups = m_groups;\n"),
    "g_v_locals": body("m_groups[g]", "v", "    s8 g = group;\n    s8 v = voice;\n"),
    "g_v_int": body("m_groups[g]", "v", "    int g = group;\n    int v = (s8)voice;\n"),
    "prog_local": body(G, "(s8)voice", "    s8 p = program;\n").replace(", program)", ", p)"),
    "player1": "    void *player = m_groups[group];\n" + M + A % ("player", "(s8)voice", G) + B % (G, "(s8)voice", G),
    "player1b": M + "    void *player = m_groups[group];\n" + A % ("player", "(s8)voice", G) + B % (G, "(s8)voice", G),
    "player2": "    void *player = m_groups[group];\n" + M + A % ("player", "(s8)voice", "player") + B % (G, "(s8)voice", G),
    "player_all": "    void *player = m_groups[group];\n" + M + A % ("player", "(s8)voice", "player") + B % ("player", "(s8)voice", "player"),
    "player_v": "    void *player = m_groups[group];\n    s8 v = voice;\n" + M + A % ("player", "v", G) + B % (G, "v", G),
    "bool_grp": "    if (m_groups[group] == 0) {\n    }\n" + body(G, "(s8)voice"),
}}
