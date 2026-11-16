"""fn_803349A0 of Game/EggLastMechArea.cpp (wave 39, second pass): retail has the player's area in
r5, the boss's in r6 and the constant 10 in r4."""
A = 'extern "C" bool fn_8033496C(UnkPart_80334C78 *part);\nextern "C" int fn_803349A0(UnkPart_80334C78 *part) {\n'
RC = "inline int Row(int area) {\n    return area / 10;\n}\ninline int Column(int area) {\n    return area % 10;\n}\n"
OPP = "inline int Opposite(int row, int column) {\n    return (4 - row) * 10 + (4 - column);\n}\n"
B1 = "    int bossRow = boss / 10;\n    int bossColumn = boss % 10;\n"
P1 = "    int playerRow = player / 10;\n    int playerColumn = player % 10;\n"
O1 = "    if (player == (4 - bossRow) * 10 + (4 - bossColumn)) {\n"
D = "    int player = part->m_area[0];\n    int boss = part->m_area[1];\n"
REPL = {
 "rc": [(A, RC + A), (B1, "    int bossRow = Row(boss);\n    int bossColumn = Column(boss);\n"), (P1, "    int playerRow = Row(player);\n    int playerColumn = Column(player);\n")],
 "rc_opp": [(A, RC + OPP + A), (B1, "    int bossRow = Row(boss);\n    int bossColumn = Column(boss);\n"), (P1, "    int playerRow = Row(player);\n    int playerColumn = Column(player);\n"), (O1, "    if (player == Opposite(bossRow, bossColumn)) {\n")],
 "opp": [(A, OPP + A), (O1, "    if (player == Opposite(bossRow, bossColumn)) {\n")],
 "opp_rev": [(O1, "    if (player == (4 - bossColumn) + (4 - bossRow) * 10) {\n")],
 "boss_first": [(D, "    int boss = part->m_area[1];\n    int player = part->m_area[0];\n")],
 "decl_all": [(D, "    int player;\n    int boss;\n    int bossRow;\n    int bossColumn;\n    int playerRow;\n    int playerColumn;\n    player = part->m_area[0];\n    boss = part->m_area[1];\n"), (B1, "    bossRow = boss / 10;\n    bossColumn = boss % 10;\n"), (P1, "    playerRow = player / 10;\n    playerColumn = player % 10;\n")],
 "opp_local": [(O1, "    int opposite = (4 - bossRow) * 10 + (4 - bossColumn);\n    if (player == opposite) {\n")],
 "opp_sub": [(O1, "    if (player == 44 - (bossRow * 10 + bossColumn)) {\n")],
 "col_sub": [(B1, "    int bossRow = boss / 10;\n    int bossColumn = boss - bossRow * 10;\n")],
}
SCORE = ["fn_803349A0"]
