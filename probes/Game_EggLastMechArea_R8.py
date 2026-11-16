"""fn_803349A0 of Game/EggLastMechArea.cpp (wave 39, second pass): forms of the opposite area."""
A = 'extern "C" bool fn_8033496C(UnkPart_80334C78 *part);\nextern "C" int fn_803349A0(UnkPart_80334C78 *part) {\n'
O1 = "    if (player == (4 - bossRow) * 10 + (4 - bossColumn)) {\n"
MK = "inline int MakeArea(int row, int column) {\n    return row * 10 + column;\n}\n"
MK2 = "inline int MakeArea(int column, int row) {\n    return row * 10 + column;\n}\n"
MK3 = "inline int MakeArea(int row, int column) {\n    return column + row * 10;\n}\n"
REPL = {
 "ten_first": [(O1, "    if (player == 10 * (4 - bossRow) + (4 - bossColumn)) {\n")],
 "col_ten": [(O1, "    if (player == (4 - bossColumn) + 10 * (4 - bossRow)) {\n")],
 "mk": [(A, MK + A), (O1, "    if (player == MakeArea(4 - bossRow, 4 - bossColumn)) {\n")],
 "mk2": [(A, MK2 + A), (O1, "    if (player == MakeArea(4 - bossColumn, 4 - bossRow)) {\n")],
 "mk3": [(A, MK3 + A), (O1, "    if (player == MakeArea(4 - bossRow, 4 - bossColumn)) {\n")],
 "mk_swapped": [(A, MK + A), (O1, "    if (MakeArea(4 - bossRow, 4 - bossColumn) == player) {\n")],
 "mk_local": [(A, MK + A), (O1, "    int opposite = MakeArea(4 - bossRow, 4 - bossColumn);\n    if (player == opposite) {\n")],
 "flip": [(A, "inline int Flip(int value) {\n    return 4 - value;\n}\n" + A), (O1, "    if (player == Flip(bossRow) * 10 + Flip(bossColumn)) {\n")],
 "flip_mk": [(A, "inline int Flip(int value) {\n    return 4 - value;\n}\n" + MK + A), (O1, "    if (player == MakeArea(Flip(bossRow), Flip(bossColumn))) {\n")],
}
SCORE = ["fn_803349A0"]
