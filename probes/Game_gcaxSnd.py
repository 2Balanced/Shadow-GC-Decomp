"""Wave 41, Game/gcaxSnd.c: fn_8043E6F4 (retail copies the zeroed bank counter into the offset:
`mr r31,r29`, the inline-loop lever), fn_8043E9C4 and fn_8043F70C."""
SCORE = ["fn_8043E6F4", "fn_8043E9C4", "fn_8043F70C"]

NL = chr(10)
INIT_HEAD = "/* Initialises the library: memory, ARAM (a silent 0x500-byte block first), the default bank sizes. */" + NL
LOOP = """    for (i = 0; i < 16; i++) {
        if (lbl_805F1404->m_config[i] != 0) {
            fn_8043B1AC(i, (lbl_805F1404->m_config[i] + 0x1F) & 0xFFFFFFE0);
        }
    }
"""
DECL = """    void *zero;
    int i;
    fn_8043E9C4(memory, size);
"""
BANKS = """static inline void NewAramBanks(void) {
    int i;
    for (i = 0; i < 16; i++) {
        if (lbl_805F1404->m_config[i] != 0) {
            fn_8043B1AC(i, (lbl_805F1404->m_config[i] + 0x1F) & 0xFFFFFFE0);
        }
    }
}

"""
CALL = "    NewAramBanks();" + NL
REPL = {
    "base": [],
    "banks_inline": [(LOOP, CALL), (INIT_HEAD, BANKS + INIT_HEAD),
                     (DECL, "    void *zero;" + NL + "    fn_8043E9C4(memory, size);" + NL)],
    "banks_inline_keepi": [(LOOP, CALL), (INIT_HEAD, BANKS + INIT_HEAD)],
}
