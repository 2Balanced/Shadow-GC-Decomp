"""Source shapes for Player::Behavior::Jump::Enter (wave 40). Two spots: the frame time read through
the reference after the scaled gravity's result slot is set up (mr r4,r3 before the lfs), and the
jump table's address in r3 with the player in r4 at the curl check."""
SCORE = ["Enter__Q36Player8Behavior4JumpFiRCQ36Player8Behavior14InitializeArgs"]
G_OLD = """        AddVector(m_player->m_position,
                  fn_800091A4(&gravity, FrameTime(m_player.operator->())));
"""
C_OLD = """    if (!s_params[CharaKind(m_player.get())].m_curl) {
        m_flags.Clear(0);
    }
    fn_80076CA8(m_player.get()->m_stateBits, 0x35);
"""
C_TAIL = """        m_flags.Clear(0);
    }
    fn_80076CA8(m_player.get()->m_stateBits, 0x35);
"""
H_OLD = "/* Whether a vector is about zero (as in Game/PlayerBehaviorIdle.cpp). */\n"
H_NEW = """inline Vec3f ScaledBy(const Vec3f &vector, const float &scale) {
    return fn_800091A4(&vector, scale);
}
inline Vec3f ScaledByP(const Vec3f *vector, const float &scale) {
    return fn_800091A4(vector, scale);
}
inline Vec3f ScaledByV(const Vec3f &vector, float scale) {
    return fn_800091A4(&vector, scale);
}
""" + H_OLD
P_OLD = "const char *Jump::s_name = \"Jump\";\n"
P_NEW = """inline bool CanCurl(Player::PlayerBase *player) {
    return s_params[FieldsOf(player)->m_kind].m_curl;
}
inline const UnkJumpParam_804B0558 &ParamOf(s32 kind) {
    return s_params[kind];
}
inline const UnkJumpParam_804B0558 *ParamTable() {
    return s_params;
}
""" + P_OLD
HP = [(H_OLD, H_NEW), (P_OLD, P_NEW)]


def g(new):
    return HP + [(G_OLD, new)]


def c(new):
    return HP + [(C_OLD, new + C_TAIL)]


REPL = {
    "g_ref": g("        AddVector(m_player->m_position,\n                  ScaledBy(gravity, FrameTime(m_player.operator->())));\n"),
    "g_refp": g("        AddVector(m_player->m_position,\n                  ScaledByP(&gravity, FrameTime(m_player.operator->())));\n"),
    "g_val": g("        AddVector(m_player->m_position,\n                  ScaledByV(gravity, FrameTime(m_player.operator->())));\n"),
    "g_local": g("        const float &dt = FrameTime(m_player.operator->());\n        AddVector(m_player->m_position, fn_800091A4(&gravity, dt));\n"),
    "g_ptr": g("        const float *dt = &FrameTime(m_player.operator->());\n        AddVector(m_player->m_position, fn_800091A4(&gravity, *dt));\n"),
    "c_direct": c("    if (!s_params[FieldsOf(m_player.get())->m_kind].m_curl) {\n"),
    "c_local": c("    const UnkJumpParam_804B0558 *param = &s_params[CharaKind(m_player.get())];\n    if (!param->m_curl) {\n"),
    "c_cancurl": c("    if (!CanCurl(m_player.get())) {\n"),
    "c_paramof": c("    if (!ParamOf(CharaKind(m_player.get())).m_curl) {\n"),
    "c_table": c("    if (!ParamTable()[CharaKind(m_player.get())].m_curl) {\n"),
    "c_table2": c("    const UnkJumpParam_804B0558 *table = s_params;\n    if (!table[CharaKind(m_player.get())].m_curl) {\n"),
    "c_eq": c("    if (s_params[CharaKind(m_player.get())].m_curl == false) {\n"),
    "c_kind": c("    s32 kind = CharaKind(m_player.get());\n    if (!s_params[kind].m_curl) {\n"),
    "c_px": c("    if (!s_params[FieldsOf(m_player.px)->m_kind].m_curl) {\n"),
    "g_nonconst": [("inline const float &FrameTime(Player::PlayerBase *player) {", "inline float &FrameTime(Player::PlayerBase *player) {"),
                   ("    /* 0x060 */ const float *m_frameTime;", "    /* 0x060 */ float *m_frameTime;")],
    "g_nonconst_cast": [("inline const float &FrameTime(Player::PlayerBase *player) {\n    return *FieldsOf(player)->m_frameTime;", "inline float &FrameTime(Player::PlayerBase *player) {\n    return *const_cast<float *>(FieldsOf(player)->m_frameTime);")],
    "g_byvalue": [("inline const float &FrameTime(Player::PlayerBase *player) {", "inline float FrameTime(Player::PlayerBase *player) {")],
    "c_u8": c("    if (s_params[CharaKind(m_player.get())].m_curl == 0) {\n") + [("    /* 0x8 */ bool m_curl;", "    /* 0x8 */ u8 m_curl;")],
    "c_plus": c("    if (!(s_params + CharaKind(m_player.get()))->m_curl) {\n"),
    "c_constkind": c("    if (!s_params[CharaKind(m_player.get())].m_curl) {\n") + [("inline s32 CharaKind(Player::PlayerBase *player) {", "inline s32 CharaKind(const Player::PlayerBase *player) {"), ("    return FieldsOf(player)->m_kind;", "    return reinterpret_cast<const UnkPlayerFields_800871D8 *>(player)->m_kind;")],
    "c_nonconst_table": c("    if (!const_cast<UnkJumpParam_804B0558 *>(s_params)[CharaKind(m_player.get())].m_curl) {\n"),
    "c_wait_after": [(C_OLD.replace("    fn_80076CA8(m_player.get()->m_stateBits, 0x35);\n".replace("\n", chr(10)), ""), "")] if False else c("    if (!s_params[CharaKind(m_player.get())].m_curl) {\n"),
    "c_bool": c("    bool curl = s_params[CharaKind(m_player.get())].m_curl;\n    if (!curl) {\n"),
    "c_true": c("    if (s_params[CharaKind(m_player.get())].m_curl) {\n    } else {\n"),
    "c_inc": c("    const UnkJumpParam_804B0558 *param = s_params;\n    param += CharaKind(m_player.get());\n    if (!param->m_curl) {\n"),
    "c_ref": c("    const UnkJumpParam_804B0558 &param = s_params[CharaKind(m_player.get())];\n    if (!param.m_curl) {\n"),
    "c_byteptr": c("    if (!*(&s_params[0].m_curl + CharaKind(m_player.get()) * sizeof(UnkJumpParam_804B0558))) {\n"),
    "c_curlptr": c("    const bool *curl = &s_params[0].m_curl;\n    if (!curl[CharaKind(m_player.get()) * 12]) {\n"),
    "c_oper": c("    if (!s_params[CharaKind(m_player.operator->())].m_curl) {\n"),
    "c_this": c("    if (!s_params[FieldsOf(this->m_player.px)->m_kind].m_curl) {\n"),
    "c_wait_in": [(HP[0]), (HP[1]), ("    m_wait = 10;\n" + C_OLD, "    if (!s_params[CharaKind(m_player.get())].m_curl) {\n        m_wait = 10;\n" + C_TAIL)],
    "c_copy": c("    UnkJumpParam_804B0558 param = s_params[CharaKind(m_player.get())];\n    if (!param.m_curl) {\n"),
}
