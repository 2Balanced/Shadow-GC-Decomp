"""Wave 41, Game/gcaxSndPlayer.c: fn_8043E0E8 (retail copies the zeroed channel counter into the
register that holds 0 for the channel stores: `mr r23,r21`) and fn_8043DAE4 (bend value and channel
pointer registers)."""
SCORE = ["fn_8043E0E8", "fn_8043DAE4"]

RESET_FN = """/* Resets a channel's controllers (MIDI controller 121). */
static inline void ResetChannel(GcaxPlayer *player, int channel) {
"""
LOOP = """    for (j = 0; j < 16; j++) {
        player->m_channels[j].field_0x4 = j;
        ResetChannel(player, j);
    }
"""
MACRO = """#define RESET_CHANNEL(player, channel)                                                             \\
    {                                                                                              \\
        u32 interrupt = OSDisableInterrupts();                                                     \\
        (player)->m_channels[channel].field_0x0 = 0;                                               \\
        (player)->m_channels[channel].m_volume = 100;                                              \\
        (player)->m_channels[channel].m_pan = 0x40;                                                \\
        (player)->m_channels[channel].m_bendRaw = 0x2000;                                          \\
        (player)->m_channels[channel].m_expression = 0x7F;                                         \\
        (player)->m_channels[channel].m_depth = 0;                                                 \\
        (player)->m_channels[channel].field_0x28 = 0;                                              \\
        (player)->m_channels[channel].field_0x24 = -1;                                             \\
        (player)->m_channels[channel].field_0x38 = 0x88888888;                                     \\
        (player)->m_channels[channel].field_0x3C = 2290649344.0f;                                  \\
        (player)->m_channels[channel].field_0x40 = 2290649344.0f;                                  \\
        OSRestoreInterrupts(interrupt);                                                            \\
    }

"""
INIT_INLINE = MACRO + """static inline void InitChannels(GcaxPlayer *player) {
    int j;
    for (j = 0; j < 16; j++) {
        player->m_channels[j].field_0x4 = j;
        RESET_CHANNEL(player, j);
    }
}

"""
INIT_INLINE2 = MACRO + """static inline void InitChannels(GcaxPlayer *player) {
    int i;
    for (i = 0; i < 16; i++) {
        player->m_channels[i].field_0x4 = i;
        RESET_CHANNEL(player, i);
    }
}

"""
SETUP_INLINE = MACRO + """static inline GcaxPlayer *SetUpPlayer(GcaxPlayer *player) {
    int j;
    memset(player, 0, sizeof(GcaxPlayer));
    player->m_flags |= 0x80000000;
    player->m_volume = 0.0f;
    player->field_0xC = 0.0f;
    player->m_speed = 1.0f;
    player->field_0x28 = 0.0f;
    player->field_0x10 = 1.0f;
    player->m_volumeFade.m_remaining = -1.0f;
    player->m_speedFade.m_remaining = -1.0f;
    for (j = 0; j < 16; j++) {
        player->m_channels[j].field_0x4 = j;
        RESET_CHANNEL(player, j);
    }
    return player;
}

"""
SETUP_BODY = """    player = &lbl_805F1404->m_players[i];
    memset(player, 0, sizeof(GcaxPlayer));
    player->m_flags |= 0x80000000;
    player->m_volume = 0.0f;
    player->field_0xC = 0.0f;
    player->m_speed = 1.0f;
    player->field_0x28 = 0.0f;
    player->field_0x10 = 1.0f;
    player->m_volumeFade.m_remaining = -1.0f;
    player->m_speedFade.m_remaining = -1.0f;
    for (j = 0; j < 16; j++) {
        player->m_channels[j].field_0x4 = j;
        ResetChannel(player, j);
    }
"""
BEND = """    value &= 0x3FFF;
    player->m_channels[channel].m_bendRaw = value;
    player->m_channels[channel].m_bend = player->m_channels[channel].m_bendRaw - 0x2000;
"""
REPL = {
    "base": [],
    "init_inline": [(RESET_FN, INIT_INLINE + RESET_FN), (LOOP, "    InitChannels(player);\n")],
    "init_inline_i": [(RESET_FN, INIT_INLINE2 + RESET_FN), (LOOP, "    InitChannels(player);\n")],
    "setup_inline": [(RESET_FN, SETUP_INLINE + RESET_FN), (SETUP_BODY, "    player = SetUpPlayer(&lbl_805F1404->m_players[i]);\n")],
    "setup_inline_void": [(RESET_FN, SETUP_INLINE.replace("static inline GcaxPlayer *SetUpPlayer", "static inline void SetUpPlayer").replace("    return player;\n}", "}") + RESET_FN),
                          (SETUP_BODY, "    player = &lbl_805F1404->m_players[i];\n    SetUpPlayer(player);\n")],
    "bend_ptr": [(BEND, "    GcaxChannel *c = &player->m_channels[channel];\n    value &= 0x3FFF;\n    c->m_bendRaw = value;\n    c->m_bend = c->m_bendRaw - 0x2000;\n")],
    "bend_ptr2": [(BEND, "    GcaxChannel *c;\n    value &= 0x3FFF;\n    c = &player->m_channels[channel];\n    c->m_bendRaw = value;\n    c->m_bend = c->m_bendRaw - 0x2000;\n")],
    "bend_noassign": [(BEND, "    player->m_channels[channel].m_bendRaw = value & 0x3FFF;\n    player->m_channels[channel].m_bend = player->m_channels[channel].m_bendRaw - 0x2000;\n")],
    "bend_local": [(BEND, "    u32 bend = value & 0x3FFF;\n    player->m_channels[channel].m_bendRaw = bend;\n    player->m_channels[channel].m_bend = player->m_channels[channel].m_bendRaw - 0x2000;\n")],
    "bend_value": [(BEND, "    value &= 0x3FFF;\n    player->m_channels[channel].m_bendRaw = value;\n    player->m_channels[channel].m_bend = value - 0x2000;\n")],
}
DECL = """GcaxPlayer *fn_8043E0E8(void) {
    int j;
    u32 interrupt = OSDisableInterrupts();
    int i;
    GcaxPlayer *player;
"""
DECL_NOJ = """GcaxPlayer *fn_8043E0E8(void) {
    u32 interrupt = OSDisableInterrupts();
    int i;
    GcaxPlayer *player;
"""
INIT_U32 = INIT_INLINE.replace("    int j;\n", "    u32 j;\n")
INIT_PRE = INIT_INLINE.replace("    for (j = 0; j < 16; j++) {", "    for (j = 0; j < 16; ++j) {")
INIT_CNT = MACRO + """static inline void InitChannels(GcaxPlayer *player, int count) {
    int j;
    for (j = 0; j < count; j++) {
        player->m_channels[j].field_0x4 = j;
        RESET_CHANNEL(player, j);
    }
}

"""
REPL.update({
    "init_noj": [(RESET_FN, INIT_INLINE + RESET_FN), (LOOP, "    InitChannels(player);\n"), (DECL, DECL_NOJ)],
    "init_u32_noj": [(RESET_FN, INIT_U32 + RESET_FN), (LOOP, "    InitChannels(player);\n"), (DECL, DECL_NOJ)],
    "init_pre_noj": [(RESET_FN, INIT_PRE + RESET_FN), (LOOP, "    InitChannels(player);\n"), (DECL, DECL_NOJ)],
    "init_cnt_noj": [(RESET_FN, INIT_CNT + RESET_FN), (LOOP, "    InitChannels(player, 16);\n"), (DECL, DECL_NOJ)],
    "setup_void_noj": [(RESET_FN, SETUP_INLINE.replace("static inline GcaxPlayer *SetUpPlayer", "static inline void SetUpPlayer").replace("    return player;\n}", "}") + RESET_FN),
                          (SETUP_BODY, "    player = &lbl_805F1404->m_players[i];\n    SetUpPlayer(player);\n"), (DECL, DECL_NOJ)],
    "setup_ret_noj": [(RESET_FN, SETUP_INLINE + RESET_FN), (SETUP_BODY, "    player = SetUpPlayer(&lbl_805F1404->m_players[i]);\n"), (DECL, DECL_NOJ)],
})
MACRO_C = MACRO.replace("RESET_CHANNEL(player, channel)", "RESET_CHANNEL_P(c)").replace("(player)->m_channels[channel].", "(c)->")
def chinit(decl, setp):
    return MACRO_C + "static inline void InitChannels(GcaxPlayer *player) {\n" + decl + """    for (j = 0; j < 16; j++) {
""" + setp + """        channel->field_0x4 = j;
        RESET_CHANNEL_P(channel);
    }
}

"""
REPL.update({
    "ch_before": [(RESET_FN, chinit("    GcaxChannel *channel;\n    int j;\n", "        channel = &player->m_channels[j];\n") + RESET_FN), (LOOP, "    InitChannels(player);\n"), (DECL, DECL_NOJ)],
    "ch_after": [(RESET_FN, chinit("    int j;\n    GcaxChannel *channel;\n", "        channel = &player->m_channels[j];\n") + RESET_FN), (LOOP, "    InitChannels(player);\n"), (DECL, DECL_NOJ)],
    "ch_before_j": [(RESET_FN, chinit("    GcaxChannel *channel;\n    int j;\n", "        channel = &player->m_channels[j];\n") + RESET_FN), (LOOP, "    InitChannels(player);\n")],
    "ch_inner": [(RESET_FN, chinit("    int j;\n", "        GcaxChannel *channel = &player->m_channels[j];\n") + RESET_FN), (LOOP, "    InitChannels(player);\n"), (DECL, DECL_NOJ)],
})
INIT_WHILE = MACRO + """static inline void InitChannels(GcaxPlayer *player) {
    int j = 0;
    while (j < 16) {
        player->m_channels[j].field_0x4 = j;
        RESET_CHANNEL(player, j);
        j++;
    }
}

"""
INIT_DO = MACRO + """static inline void InitChannels(GcaxPlayer *player) {
    int j = 0;
    do {
        player->m_channels[j].field_0x4 = j;
        RESET_CHANNEL(player, j);
    } while (++j < 16);
}

"""
INIT_PJ = MACRO + """static inline void InitChannels(GcaxPlayer *player, int j) {
    for (; j < 16; j++) {
        player->m_channels[j].field_0x4 = j;
        RESET_CHANNEL(player, j);
    }
}

"""
REPL.update({
    "init_while": [(RESET_FN, INIT_WHILE + RESET_FN), (LOOP, "    InitChannels(player);\n"), (DECL, DECL_NOJ)],
    "init_do": [(RESET_FN, INIT_DO + RESET_FN), (LOOP, "    InitChannels(player);\n"), (DECL, DECL_NOJ)],
    "init_pj": [(RESET_FN, INIT_PJ + RESET_FN), (LOOP, "    InitChannels(player, 0);\n"), (DECL, DECL_NOJ)],
})
SIG = "void fn_8043DAE4(GcaxPlayer *player, u8 channel, u32 value) {\n"
PROTO = "void fn_8043DAE4(GcaxPlayer *, u8, u32);\n"
REPL.update({
    "b_u16": [(SIG, "void fn_8043DAE4(GcaxPlayer *player, u8 channel, u16 value) {\n"), (PROTO, "void fn_8043DAE4(GcaxPlayer *, u8, u16);\n")],
    "b_int": [(SIG, "void fn_8043DAE4(GcaxPlayer *player, u8 channel, int value) {\n"), (PROTO, "void fn_8043DAE4(GcaxPlayer *, u8, int);\n")],
    "b_u16_noassign": [(SIG, "void fn_8043DAE4(GcaxPlayer *player, u8 channel, u16 value) {\n"), (PROTO, "void fn_8043DAE4(GcaxPlayer *, u8, u16);\n"),
                       (BEND, "    player->m_channels[channel].m_bendRaw = value & 0x3FFF;\n    player->m_channels[channel].m_bend = player->m_channels[channel].m_bendRaw - 0x2000;\n")],
    "b_int_ch": [(SIG, "void fn_8043DAE4(GcaxPlayer *player, int channel, u32 value) {\n"), (PROTO, "void fn_8043DAE4(GcaxPlayer *, int, u32);\n"),
                 (BEND, "    value &= 0x3FFF;\n    player->m_channels[(u8)channel].m_bendRaw = value;\n    player->m_channels[(u8)channel].m_bend = player->m_channels[(u8)channel].m_bendRaw - 0x2000;\n")],
    "b_mask_first": [(BEND, "    u32 bend;\n    bend = value & 0x3FFF;\n    player->m_channels[channel].m_bendRaw = bend;\n    player->m_channels[channel].m_bend = player->m_channels[channel].m_bendRaw - 0x2000;\n")],
    "b_volatile_like": [(BEND, "    GcaxChannel *c;\n    c = player->m_channels + channel;\n    c->m_bendRaw = value & 0x3FFF;\n    c->m_bend = c->m_bendRaw - 0x2000;\n")],
})
