"""Wave 41, Game/gcaxSndSequence.c: fn_8043FDF8 (retail stores the format through the sequence
pointer, so the file pointer can take the player's r30)."""
SCORE = ["fn_8043FDF8"]
NL = chr(10)
A = """    sequence = &player->m_sequence;
    file = (u8 *)table + (*entry & 0xFFFFFF);
    sequence->m_format = *(u16 *)(file + 0x10);
"""
REPL = {
    "base": [],
    "file_first": [(A, "    file = (u8 *)table + (*entry & 0xFFFFFF);" + NL + "    sequence = &player->m_sequence;" + NL + "    sequence->m_format = *(u16 *)(file + 0x10);" + NL)],
    "header_file": [(A, "    sequence = &player->m_sequence;" + NL + "    file = (u8 *)header;" + NL + "    sequence->m_format = *(u16 *)(file + 0x10);" + NL)],
    "hdr_fmt": [(A, "    sequence = &player->m_sequence;" + NL + "    file = (u8 *)table + (*entry & 0xFFFFFF);" + NL + "    sequence->m_format = ((u16 *)file)[8];" + NL)],
    "local_fmt": [(A, "    sequence = &player->m_sequence;" + NL + "    file = (u8 *)table + (*entry & 0xFFFFFF);" + NL + "    {" + NL + "        u16 format = *(u16 *)(file + 0x10);" + NL + "        sequence->m_format = format;" + NL + "    }" + NL)],
}
