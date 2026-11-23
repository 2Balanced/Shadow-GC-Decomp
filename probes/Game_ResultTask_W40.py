"""ResultTask::ShowRank (wave 40): a block boundary behind `type = 1` so that the constant is not
folded into the threshold's offset."""
T = "            type = 1;\n            if (lbl_8057D6A0.m_total <= "
def v(x):
    return [(T, x + "            if (lbl_8057D6A0.m_total <= ")]
REPL = {
 "for1": v("            for (int n = 0; n < 1; n++) {\n                type = 1;\n            }\n"),
 "switch_def": v("            switch (m_kind) {\n            default:\n                type = 1;\n                break;\n            }\n"),
 "switch0": v("            switch (0) {\n            default:\n                type = 1;\n                break;\n            }\n"),
 "do0": v("            do {\n                type = 1;\n            } while (0);\n"),
 "tern": v("            type = lbl_8057D6A0.field_0xB1 ? 1 : 0;\n"),
 "bool": v("            type = lbl_8057D6A0.field_0xB1 != 0;\n"),
 "for1_after": [(T, "            type = 1;\n            for (int n = 0; n < 1; n++) {\n            }\n            if (lbl_8057D6A0.m_total <= ")],
}
SCORE = ["ShowRank__10ResultTaskFv"]
