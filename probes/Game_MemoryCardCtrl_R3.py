"""Round 3 for MemoryCard::PollOpen's switch (wave 39): statements that compile to nothing between
the failure labels, to keep them separate blocks until after the switch is lowered."""
SCORE = ["PollOpen__10MemoryCardFv"]
OLD = """        switch (m_request) {
        case 2:
        case 3:
        case 6:
        default:
            m_result = 2;
            Fail();
            break;
        case 4:
        case 5:
            SetCommand(CMD_GET_STATUS);
            break;
        }
"""
A = "            m_result = 2;\n            Fail();\n            break;\n"
B = "        case 4:\n        case 5:\n            SetCommand(CMD_GET_STATUS);\n            break;\n"
F = "void MemoryCard::PollOpen() {"
HELP = "inline void Nop_() {}\ninline void NopArg_(int) {}\nstatic const bool kDebug_ = false;\n"
SEPS = {
  "void0": "(void)0;",
  "nop": "Nop_();",
  "noparg": "NopArg_(m_request);",
  "do0": "do {} while (0);",
  "if0": "if (0) { Fail(); }",
  "ifdbg": "if (kDebug_) { Fail(); }",
  "goto": None,
}
REPL = {}
for n, sep in SEPS.items():
    if sep is None: continue
    for order in (("2", "3", "6", "d"), ("3", "6", "2", "d"), ("2", "d", "3", "6"), ("6", "3", "2", "d")):
        ga = ""
        for k, x in enumerate(order):
            ga += ("        default:\n" if x == "d" else f"        case {x}:\n")
            if k < len(order) - 1: ga += f"            {sep}\n"
        REPL[f"{n}_{''.join(order)}"] = [(F, HELP + F), (OLD, "        switch (m_request) {\n" + ga + A + B + "        }\n")]
# explicit gotos to one label
REPL["goto_fail"] = [(OLD, """        switch (m_request) {
        case 2:
            goto fail;
        case 3:
            goto fail;
        case 6:
            goto fail;
        default:
        fail:
            m_result = 2;
            Fail();
            break;
        case 4:
        case 5:
            SetCommand(CMD_GET_STATUS);
            break;
        }
""")]
REPL["goto_fail2"] = [(OLD, """        switch (m_request) {
        case 2:
            goto fail;
        case 3:
            goto fail;
        case 4:
        case 5:
            SetCommand(CMD_GET_STATUS);
            break;
        case 6:
            goto fail;
        default:
        fail:
            m_result = 2;
            Fail();
            break;
        }
""")]
