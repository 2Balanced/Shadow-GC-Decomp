"""fn_803349A0 of Game/EggLastMechArea.cpp (wave 39, second pass): statement shapes."""
SYMBOLS = {"fn_803349A0": 'extern "C" int fn_803349A0(UnkPart_80334C78 *part) {'}
TAIL = """    if (fn_8033496C(part) && playerRow != bossRow && playerColumn != bossColumn) {
        return 3;
    }
    int rows = Gap(playerRow, bossRow);
    return rows + Gap(playerColumn, bossColumn);
"""
HEAD = """    int player = part->m_area[0];
    int boss = part->m_area[1];
    if (player == boss) {
        return 0;
    }
    if (player == 22) {
        return 2;
    }
"""
BODIES = {"fn_803349A0": {
 "opp_locals": HEAD + """    int bossRow = boss / 10;
    int bossColumn = boss % 10;
    int oppositeRow = 4 - bossRow;
    int oppositeColumn = 4 - bossColumn;
    if (player == oppositeRow * 10 + oppositeColumn) {
        return 4;
    }
    int playerRow = player / 10;
    int playerColumn = player % 10;
""" + TAIL,
 "opp_interleaved": HEAD + """    int bossRow = boss / 10;
    int oppositeRow = 4 - bossRow;
    int bossColumn = boss % 10;
    int oppositeColumn = 4 - bossColumn;
    if (player == oppositeRow * 10 + oppositeColumn) {
        return 4;
    }
    int playerRow = player / 10;
    int playerColumn = player % 10;
""" + TAIL,
 "all_first": HEAD + """    int bossRow = boss / 10;
    int bossColumn = boss % 10;
    int playerRow = player / 10;
    int playerColumn = player % 10;
    if (player == (4 - bossRow) * 10 + (4 - bossColumn)) {
        return 4;
    }
""" + TAIL,
 "opp_area": HEAD + """    int bossRow = boss / 10;
    int bossColumn = boss % 10;
    int opposite = (4 - bossRow) * 10;
    opposite += 4 - bossColumn;
    if (player == opposite) {
        return 4;
    }
    int playerRow = player / 10;
    int playerColumn = player % 10;
""" + TAIL,
 "result": """    int player = part->m_area[0];
    int boss = part->m_area[1];
    if (player == boss) {
        return 0;
    } else if (player == 22) {
        return 2;
    }
    int bossRow = boss / 10;
    int bossColumn = boss % 10;
    if (player == (4 - bossRow) * 10 + (4 - bossColumn)) {
        return 4;
    }
    int playerRow = player / 10;
    int playerColumn = player % 10;
""" + TAIL,
 "boss_eq": """    int player = part->m_area[0];
    int boss = part->m_area[1];
    if (boss == player) {
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
""" + TAIL,
 "opp_swapped": HEAD + """    int bossRow = boss / 10;
    int bossColumn = boss % 10;
    if ((4 - bossRow) * 10 + (4 - bossColumn) == player) {
        return 4;
    }
    int playerRow = player / 10;
    int playerColumn = player % 10;
""" + TAIL,
 "member_cmp": """    if (part->m_area[0] == part->m_area[1]) {
        return 0;
    }
    int player = part->m_area[0];
    int boss = part->m_area[1];
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
""" + TAIL,
 "unsigned_idx": HEAD.replace("m_area[0]", "m_area[0u]").replace("m_area[1]", "m_area[1u]") + """    int bossRow = boss / 10;
    int bossColumn = boss % 10;
    if (player == (4 - bossRow) * 10 + (4 - bossColumn)) {
        return 4;
    }
    int playerRow = player / 10;
    int playerColumn = player % 10;
""" + TAIL,
}}
