"""Source shapes for MemoryCard::PollOpen's switch (wave 39): retail tests 3, then 2 below it and 6
above it, with 4 and 5 as the one range.  Label orders and groupings."""
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
B = "            SetCommand(CMD_GET_STATUS);\n            break;\n"
def lab(x): return "        default:\n" if x == "d" else f"        case {x}:\n"
REPL = {}
for perm in itertools.permutations(["2", "3", "6", "d"]):
    for bfirst in (0, 1):
        for b in (("4", "5"), ("5", "4")):
            name = "A" + "".join(perm) + ("_Bfirst" if bfirst else "_Blast") + "_" + "".join(b)
            ga = "".join(lab(x) for x in perm) + A
            gb = "".join(lab(x) for x in b) + B
            REPL[name] = [(OLD, "        switch (m_request) {\n" + (gb + ga if bfirst else ga + gb) + "        }\n")]
