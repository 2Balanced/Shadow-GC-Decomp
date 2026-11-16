"""ResultTask::Init (wave 39, second pass): declarations of the suffix table and the game state."""
T = "extern const char *lbl_8051D528[];"
S = '    sprintf(path, "CsdFiles/Result%s", lbl_8051D528[lbl_8057D6A0.m_language]);\n'
REPL = {
 "const_tbl": [(T, "extern const char *const lbl_8051D528[];")],
 "char_tbl": [(T, "extern char *lbl_8051D528[];")],
 "tbl_sized": [(T, "extern const char *lbl_8051D528[6];")],
 "u32_idx": [(S, '    sprintf(path, "CsdFiles/Result%s", lbl_8051D528[(u32)lbl_8057D6A0.m_language]);\n')],
 "const_u32": [(T, "extern const char *const lbl_8051D528[];"), (S, '    sprintf(path, "CsdFiles/Result%s", lbl_8051D528[(u32)lbl_8057D6A0.m_language]);\n')],
}
SCORE = ["Init__10ResultTaskFv"]
