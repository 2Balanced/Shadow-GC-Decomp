"""fn_803349A0 (wave 40): the grid's width (10) as a variable assigned in a loop of one pass, and
declared before the two areas."""
O = """    int player = part->m_area[0];
    int boss = part->m_area[1];
    if (player == boss) {
        return 0;
    }
    if (player == 22) {
        return 2;
    }
    int bossRow = boss / 10;
    int bossColumn = boss % 10;
    if (player == (4 - bossRow) * 10 + (4 - bossColumn)) {
        return 4;
    }
    int playerRow = player / 10;
    int playerColumn = player % 10;
"""
W = "    for (int n = 0; n < 1; n++) {\n        ten = 10;\n    }\n"
def t(o):
    return o.replace("boss / 10", "boss / ten").replace("boss % 10", "boss % ten").replace("player / 10", "player / ten").replace("player % 10", "player % ten")
P = "    int player = part->m_area[0];\n    int boss = part->m_area[1];\n"
REPL = {
 "ten_first": [(O, "    int ten = 10;\n" + t(O))],
 "ten_first_for": [(O, "    int ten;\n" + W + t(O))],
 "ten_after": [(O, t(O).replace(P, P + "    int ten = 10;\n"))],
 "ten_after_for": [(O, t(O).replace(P, P + "    int ten;\n" + W))],
 "ten_mid_for": [(O, t(O).replace("    int bossRow", "    int ten;\n" + W + "    int bossRow"))],
 "ten_first_mid_for": [(O, "    int ten;\n" + t(O).replace("    int bossRow", W + "    int bossRow"))],
 "ten_first_for_all": [(O, "    int ten;\n" + W + t(O).replace("(4 - bossRow) * 10", "(4 - bossRow) * ten"))],
 "ten_first_mid_for_all": [(O, "    int ten;\n" + t(O).replace("    int bossRow", W + "    int bossRow").replace("(4 - bossRow) * 10", "(4 - bossRow) * ten"))],
}
SCORE = ["fn_803349A0"]
