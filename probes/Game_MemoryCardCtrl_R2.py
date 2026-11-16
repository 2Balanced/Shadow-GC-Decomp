"""Round 2 for MemoryCard::PollOpen's switch (wave 39): the failure labels split into groups by an
empty statement (as a debug print compiled away would leave), so that 2 and 3 are not one range."""
import itertools
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
def lab(x): return "        default:\n" if x == "d" else f"        case {x}:\n"
REPL = {}
items = ["2", "3", "6", "d"]
for perm in itertools.permutations(items):
    for cut in (1, 2, 3):
        g1, g2 = perm[:cut], perm[cut:]
        if list(g1) != sorted(g1) or list(g2) != sorted(g2): continue
        for sep in (";", "{}"):
            name = "".join(g1) + "_" + "".join(g2) + ("_s" if sep == ";" else "_b")
            ga = "".join(lab(x) for x in g1) + f"            {sep}\n" + "".join(lab(x) for x in g2) + A
            REPL[name + "_Blast"] = [(OLD, "        switch (m_request) {\n" + ga + B + "        }\n")]
            REPL[name + "_Bfirst"] = [(OLD, "        switch (m_request) {\n" + B + ga + "        }\n")]
