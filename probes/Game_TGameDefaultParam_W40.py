"""TGameDefaultParam::ParseLine (wave 40): the loop that cuts the stage name at ']'. Retail keeps
the pointer in r3 and the character, as a byte it extends at both tests, in r4."""
NUL = "'" + chr(92) + "0'"
O = """            for (char *p = self->m_cursor; *p != NUL; p++) {
                if (*p == ']') {
                    *p = NUL;
                    break;
                }
            }
""".replace("NUL", NUL)
W = "for (int n = 0; n < 1; n++) {\n"
B = "                if (%s == ']') {\n                    *p = NUL;\n                    break;\n                }\n            }\n".replace("NUL", NUL)
F = "            for (char *p = self->m_cursor; (%s = *p) != NUL; p++) {\n".replace("NUL", NUL)
G = F.replace("for (char *p = self->m_cursor; ", "for (; ")
T = "    char c;\n"
S = "    TGameDefaultParam *self = this;\n"
REPL = {
 # 6 rows each (the two registers exchanged) unless noted
 "c_reuse": [(O, F % "c" + B % "c")],
 "c_new": [(O, "            char d;\n" + F % "d" + B % "d")],
 "p_for": [(O, "            char *p;\n            " + W + "                p = self->m_cursor;\n            }\n" + O.replace("for (char *p = self->m_cursor; ", "for (; "))],
 "c_body": [(O, O.split("\n")[0] + "\n                c = *p;\n" + B % "c")],
 "c_body_for": [(O, O.split("\n")[0] + "\n                " + W + "                    c = *p;\n                }\n" + B % "c")],
 "p_top": [(T, T + "    char *p;\n"), (O, O.replace("for (char *p = ", "for (p = "))],
 "p_top_first": [(S, "    char *p;\n" + S), (O, O.replace("for (char *p = ", "for (p = "))],
 "line_reuse": [(O, O.replace("for (char *p = ", "for (line = ").replace("*p", "*line").replace("p++", "line++"))],
 "const_self": [(O, O.replace("self->m_cursor", "((const TGameDefaultParam *)self)->m_cursor"))],
 "preinc": [(O, O.replace("p++", "++p"))],
 # 3 rows: the pointer in r3, but the character extended once, in r0
 "int_c": [(O, "            int d;\n" + F % "d" + B % "d")],
 "u8": [(O, O.replace("char *p = self->m_cursor", "u8 *p = (u8 *)self->m_cursor"))],
 # round 2
 "p_then_d": [(O, "            char *p = self->m_cursor;\n            char d;\n" + G % "d" + B % "d")],
 "p_then_d_top": [(T, T + "    char *p;\n    char d;\n"), (O, (F % "d").replace("for (char *p = ", "for (p = ") + B % "d")],
 "d_then_p_top": [(T, T + "    char d;\n    char *p;\n"), (O, (F % "d").replace("for (char *p = ", "for (p = ") + B % "d")],
 "p_before_c": [(T, "    char *p;\n" + T), (O, (F % "c").replace("for (char *p = ", "for (p = ") + B % "c")],
 "p_first_c": [(S, "    char *p;\n" + S), (O, (F % "c").replace("for (char *p = ", "for (p = ") + B % "c")],
 "s8": [(O, "            s8 d;\n" + F % "d" + B % "d")],
 "d_for_p": [(O, "            char d;\n            char *p;\n            " + W + "                p = self->m_cursor;\n            }\n" + G % "d" + B % "d")],
 "tern": [(O, "            for (char *p = self->m_cursor; *p != NUL; p++) {\n                if (*p != ']') {\n                    continue;\n                }\n                *p = NUL;\n                break;\n            }\n".replace("NUL", NUL))],
}
SCORE = ["ParseLine__17TGameDefaultParamFv"]
