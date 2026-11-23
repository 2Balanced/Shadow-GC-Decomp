"""Wave 41: fn_801E2878 (the file index's constructor) with copy propagation off and named copies."""
SCORE = ["fn_801E2878"]
F = 'extern "C" UnkFileIndex_801E2878 *fn_801E2878(void) {'
END = 'extern "C" UnkFileIndex_801E2878 *fn_801E2844(void) {'
P = [(F, "#pragma opt_propagation off\n" + F), (END, "#pragma opt_propagation reset\n" + END)]
L2 = """        lbl_80583AF8.m_count = count;
        for (int i = 25; i >= 0; i--) {
            if (lbl_80583AF8.m_first[i] < 0) {
                lbl_80583AF8.m_first[i] = lbl_80583AF8.m_first[i + 1];
            }
        }
"""
REPL = {
  "prag": P,
  "prag_ptr": P + [(L2, """        lbl_80583AF8.m_count = count;
        UnkFileTable_80583AF8 *table = &lbl_80583AF8;
        for (int i = 25; i >= 0; i--) {
            if (table->m_first[i] < 0) {
                table->m_first[i] = table->m_first[i + 1];
            }
        }
""")],
  "prag_i": P + [(L2, """        lbl_80583AF8.m_count = count;
        int i;
        for (i = 25; i >= 0; i--) {
            if (lbl_80583AF8.m_first[i] < 0) {
                lbl_80583AF8.m_first[i] = lbl_80583AF8.m_first[i + 1];
            }
        }
""")],
}
