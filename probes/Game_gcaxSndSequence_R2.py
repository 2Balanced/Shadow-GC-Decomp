"""Wave 41, Game/gcaxSndSequence.c round 2: fn_8043FDF8 with the sequence set-up in an inline."""
SCORE = ["fn_8043FDF8"]
NL = chr(10)
BODY = """    sequence = &player->m_sequence;
    file = (u8 *)table + (*entry & 0xFFFFFF);
    sequence->m_format = *(u16 *)(file + 0x10);
    memset(sequence, 0, sizeof(GcaxSequence));
    sequence->m_division = *(u16 *)(file + 0x1E);
    sequence->m_lastTime = sequence->m_startTime = lbl_805F1404->m_tickTime;
    sequence->m_tempo = 120.0f;
    sequence->m_messageFlags = flags;
    sequence->m_start = file + 0x28;
    sequence->m_cursor = file + 0x28;
"""
FHEAD = "/* Starts sequence `id` of a sequence bank on the player. */" + NL
INL = """static inline void StartSequence(GcaxSequence *sequence, u8 *file, u32 flags) {
    sequence->m_format = *(u16 *)(file + 0x10);
    memset(sequence, 0, sizeof(GcaxSequence));
    sequence->m_division = *(u16 *)(file + 0x1E);
    sequence->m_lastTime = sequence->m_startTime = lbl_805F1404->m_tickTime;
    sequence->m_tempo = 120.0f;
    sequence->m_messageFlags = flags;
    sequence->m_start = file + 0x28;
    sequence->m_cursor = file + 0x28;
}

"""
INL2 = """static inline void StartSequence(GcaxSequence *sequence, u8 *file, u32 flags) {
    sequence->m_format = ((u16 *)file)[8];
    memset(sequence, 0, sizeof(GcaxSequence));
    sequence->m_division = ((u16 *)file)[15];
    sequence->m_lastTime = sequence->m_startTime = lbl_805F1404->m_tickTime;
    sequence->m_tempo = 120.0f;
    sequence->m_messageFlags = flags;
    sequence->m_start = file + 0x28;
    sequence->m_cursor = file + 0x28;
}

"""
REPL = {
    "base": [],
    "inl": [(BODY, "    StartSequence(&player->m_sequence, (u8 *)table + (*entry & 0xFFFFFF), flags);" + NL), (FHEAD, INL + FHEAD)],
    "inl_vars": [(BODY, "    sequence = &player->m_sequence;" + NL + "    file = (u8 *)table + (*entry & 0xFFFFFF);" + NL + "    StartSequence(sequence, file, flags);" + NL), (FHEAD, INL + FHEAD)],
    "inl2": [(BODY, "    StartSequence(&player->m_sequence, (u8 *)table + (*entry & 0xFFFFFF), flags);" + NL), (FHEAD, INL2 + FHEAD)],
}
