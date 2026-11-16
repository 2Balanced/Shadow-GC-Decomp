"""Source shapes for TGameDefaultParam::ParseLine's bracket loop (wave 39): retail walks the name
with the pointer in r3 and the character in r4; GC/2.7 had them the other way round."""
SCORE = ["ParseLine__17TGameDefaultParamFv"]
OLD = """            for (char *p = self->m_cursor; *p != '\\0'; p++) {
                if (*p == ']') {
                    *p = '\\0';
                    break;
                }
            }
"""
REPL = {
  "reuse_c": [(OLD, """            for (char *p = self->m_cursor; (c = *p) != '\\0'; p++) {
                if (c == ']') {
                    *p = '\\0';
                    break;
                }
            }
""")],
  "local_c": [(OLD, """            for (char *p = self->m_cursor; *p != '\\0'; p++) {
                char ch = *p;
                if (ch == ']') {
                    *p = '\\0';
                    break;
                }
            }
""")],
  "p_top": [("    char c;\n    while ((c = line[length])", "    char c;\n    char *p;\n    while ((c = line[length])"), (OLD, OLD.replace("for (char *p = self->m_cursor;", "for (p = self->m_cursor;"))],
  "p_top_first": [("    int length = 0;\n", "    char *p;\n    int length = 0;\n"), (OLD, OLD.replace("for (char *p = self->m_cursor;", "for (p = self->m_cursor;"))],
  "while_form": [(OLD, """            char *p = self->m_cursor;
            while (*p != '\\0') {
                if (*p == ']') {
                    *p = '\\0';
                    break;
                }
                p++;
            }
""")],
  "index_form": [(OLD, """            char *name = self->m_cursor;
            for (int k = 0; name[k] != '\\0'; k++) {
                if (name[k] == ']') {
                    name[k] = '\\0';
                    break;
                }
            }
""")],
  "reuse_line": [(OLD, """            for (line = self->m_cursor; *line != '\\0'; line++) {
                if (*line == ']') {
                    *line = '\\0';
                    break;
                }
            }
""")],
}
