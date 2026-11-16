"""fn_803349A0 of Game/EggLastMechArea.cpp (wave 39, second pass): the areas through inline getters."""
A = 'extern "C" bool fn_8033496C(UnkPart_80334C78 *part);\nextern "C" int fn_803349A0(UnkPart_80334C78 *part) {\n'
G = "inline int AreaOf(UnkPart_80334C78 *part, u32 index) {\n    return part->m_area[index];\n}\n"
GI = "inline int AreaOf(UnkPart_80334C78 *part, int index) {\n    return part->m_area[index];\n}\n"
PB = "inline int PlayerArea(UnkPart_80334C78 *part) {\n    return part->m_area[0];\n}\ninline int BossArea(UnkPart_80334C78 *part) {\n    return part->m_area[1];\n}\n"
RC = "inline int Row(int area) {\n    return area / 10;\n}\ninline int Column(int area) {\n    return area % 10;\n}\n"
B1 = "    int bossRow = boss / 10;\n    int bossColumn = boss % 10;\n"
P1 = "    int playerRow = player / 10;\n    int playerColumn = player % 10;\n"
D = "    int player = part->m_area[0];\n    int boss = part->m_area[1];\n"
RCR = [(B1, "    int bossRow = Row(boss);\n    int bossColumn = Column(boss);\n"), (P1, "    int playerRow = Row(player);\n    int playerColumn = Column(player);\n")]
REPL = {
 "g_u": [(A, G + A), (D, "    int player = AreaOf(part, 0);\n    int boss = AreaOf(part, 1);\n")],
 "g_i": [(A, GI + A), (D, "    int player = AreaOf(part, 0);\n    int boss = AreaOf(part, 1);\n")],
 "g_pb": [(A, PB + A), (D, "    int player = PlayerArea(part);\n    int boss = BossArea(part);\n")],
 "g_pb_rc": [(A, PB + RC + A), (D, "    int player = PlayerArea(part);\n    int boss = BossArea(part);\n")] + RCR,
 "g_boss_only": [(A, PB + A), (D, "    int player = part->m_area[0];\n    int boss = BossArea(part);\n")],
 "g_player_only": [(A, PB + A), (D, "    int player = PlayerArea(part);\n    int boss = part->m_area[1];\n")],
 "g_bp": [(A, PB + A), (D, "    int boss = BossArea(part);\n    int player = PlayerArea(part);\n")],
 "ten": [(D, D + "    int ten = 10;\n"), (B1, "    int bossRow = boss / ten;\n    int bossColumn = boss % ten;\n"), (P1, "    int playerRow = player / ten;\n    int playerColumn = player % ten;\n")],
}
SCORE = ["fn_803349A0"]
