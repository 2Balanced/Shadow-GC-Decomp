"""fn_80334A70 of Game/EggLastMechArea.cpp (wave 39, second pass): the row and the column through
inline functions."""
A = 'extern "C" void fn_80334A70(UnkPart_80334C78 *part) {\n'
ROWCOL = """        int row;
        if (character.get()->m_position.z < negBorder) {
            row = 10;
        } else if (character.get()->m_position.z < border) {
            row = 20;
        } else {
            row = 30;
        }
        int column;
        if (character.get()->m_position.x < negBorder) {
            column = 1;
        } else if (character.get()->m_position.x < border) {
            column = 2;
        } else {
            column = 3;
        }
        int area = row + column;
        SetArea(part, i, area);
"""
BAND = "inline int Band(float value, float low, float high, int a, int b, int c) {\n    if (value < low) {\n        return a;\n    } else if (value < high) {\n        return b;\n    }\n    return c;\n}\n\n"
BAND2 = "inline int Band(float value, float low, float high, int a, int b, int c) {\n    int band;\n    if (value < low) {\n        band = a;\n    } else if (value < high) {\n        band = b;\n    } else {\n        band = c;\n    }\n    return band;\n}\n\n"
REPL = {
 "band_expr": [(A, BAND2 + A), (ROWCOL, "        SetArea(part, i, Band(character.get()->m_position.z, negBorder, border, 10, 20, 30) + Band(character.get()->m_position.x, negBorder, border, 1, 2, 3));\n")],
 "band_locals": [(A, BAND2 + A), (ROWCOL, "        int row = Band(character.get()->m_position.z, negBorder, border, 10, 20, 30);\n        int column = Band(character.get()->m_position.x, negBorder, border, 1, 2, 3);\n        SetArea(part, i, row + column);\n")],
 "band_col": [(A, BAND2 + A), (ROWCOL, ROWCOL[:ROWCOL.index("        int column;")] + "        int column = Band(character.get()->m_position.x, negBorder, border, 1, 2, 3);\n        int area = row + column;\n        SetArea(part, i, area);\n")],
 "band_ret": [(A, BAND + A), (ROWCOL, "        int row = Band(character.get()->m_position.z, negBorder, border, 10, 20, 30);\n        int column = Band(character.get()->m_position.x, negBorder, border, 1, 2, 3);\n        SetArea(part, i, row + column);\n")],
 "area_fn": [(A, "inline int AreaOf(const Vec3f &p, float negBorder, float border) {\n" + ROWCOL.replace("character.get()->m_position", "p").replace("        int area = row + column;\n        SetArea(part, i, area);\n", "        return row + column;\n").replace("\n        ", "\n    ").replace("        int row;", "    int row;") + "}\n\n" + A), (ROWCOL, "        SetArea(part, i, AreaOf(character.get()->m_position, negBorder, border));\n")],
 "area_assign": [(ROWCOL, ROWCOL.replace("        int area = row + column;\n        SetArea(part, i, area);\n", "        int area;\n        area = row + column;\n        SetArea(part, i, area);\n").replace("        int row;\n", "        int row;\n", 1))],
}
SCORE = ["fn_80334A70"]
