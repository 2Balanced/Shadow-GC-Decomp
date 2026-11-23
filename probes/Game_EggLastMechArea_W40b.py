"""fn_803349A0 (wave 40, round 2): where the width is declared and set, crossed with locals for the
opposite area's row and column."""
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
P = "    int player = part->m_area[0];\n    int boss = part->m_area[1];\n"
BR = "    int bossRow = boss / ten;\n"
BC = "    int bossColumn = boss % ten;\n"
IF = "    if (player == (4 - bossRow) * 10 + (4 - bossColumn)) {\n"
def t(o):
    return o.replace("boss / 10", "boss / ten").replace("boss % 10", "boss % ten").replace("player / 10", "player / ten").replace("player % 10", "player % ten")
DECL = {"first": lambda o: "    int ten;\n" + o, "after": lambda o: o.replace(P, P + "    int ten;\n"), "mid": lambda o: o.replace(BR, "    int ten;\n" + BR)}
SET = {"for": "    for (int n = 0; n < 1; n++) {\n        ten = 10;\n    }\n", "plain": "    ten = 10;\n"}
OPP = {
 "none": lambda o: o,
 "rc": lambda o: o.replace(BR + BC + IF, BR + "    int oppRow = 4 - bossRow;\n" + BC + "    int oppColumn = 4 - bossColumn;\n    if (player == oppRow * 10 + oppColumn) {\n"),
 "rc2": lambda o: o.replace(BR + BC + IF, BR + BC + "    int oppRow = 4 - bossRow;\n    int oppColumn = 4 - bossColumn;\n    if (player == oppRow * 10 + oppColumn) {\n"),
 "r": lambda o: o.replace(BR + BC + IF, BR + "    int oppRow = 4 - bossRow;\n" + BC + "    if (player == oppRow * 10 + (4 - bossColumn)) {\n"),
 "rten": lambda o: o.replace(BR + BC + IF, BR + "    int oppRow = 4 - bossRow;\n" + BC + "    int oppColumn = 4 - bossColumn;\n    if (player == oppRow * ten + oppColumn) {\n"),
}
REPL = {}
for dn, d in DECL.items():
    for sn, s in SET.items():
        for on, op in OPP.items():
            REPL[dn + "_" + sn + "_" + on] = [(O, op(d(t(O)).replace(BR, s + BR)))]
SCORE = ["fn_803349A0"]
