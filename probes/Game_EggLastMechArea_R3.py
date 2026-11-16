"""fn_80334A70 of Game/EggLastMechArea.cpp (wave 39, second pass): on SetArea(u32 index), shapes for
the area: retail has the column in r3 and the address in r4."""
S = "inline void SetArea(UnkPart_80334C78 *part, int index, int area) {\n"
U = (S, S.replace("int index", "u32 index"))
C = "        int area = row + column;\n        SetArea(part, i, area);\n"
BODY = """    if (part->m_area[index] != area) {
        part->m_lastArea[index] = part->m_area[index];
        part->m_area[index] = area;
        part->m_areaTime[index] = 0.0f;
    }
"""
REPL = {
 "expr": [U, (C, "        SetArea(part, i, row + column);\n")],
 "expr_rev": [U, (C, "        SetArea(part, i, column + row);\n")],
 "area_u32": [(S, S.replace("int index, int area", "u32 index, u32 area"))],
 "area_s32": [(S, S.replace("int index, int area", "u32 index, s32 area"))],
 "member": [(S, S.replace("int index", "u32 index")), (BODY, "    s32 old = part->m_area[index];\n    if (old != area) {\n        part->m_lastArea[index] = old;\n        part->m_area[index] = area;\n        part->m_areaTime[index] = 0.0f;\n    }\n")],
 "ne_rev": [U, (BODY, BODY.replace("part->m_area[index] != area", "area != part->m_area[index]"))],
 "col_first": [U, ("        int row;\n        if (character.get()->m_position.z < negBorder) {", "        int column;\n        int row;\n        if (character.get()->m_position.z < negBorder) {"), ("        int column;\n        if (character.get()->m_position.x", "        if (character.get()->m_position.x")],
 "area_first": [U, ("        int row;\n        if (character.get()->m_position.z < negBorder) {", "        int area;\n        int row;\n        if (character.get()->m_position.z < negBorder) {"), (C, "        area = row + column;\n        SetArea(part, i, area);\n")],
 "area_plus": [U, (C, "        int area = row;\n        area += column;\n        SetArea(part, i, area);\n")],
 "row_plus": [U, (C, "        row += column;\n        SetArea(part, i, row);\n")],
 "col_plus": [U, (C, "        column += row;\n        SetArea(part, i, column);\n")],
}
SCORE = ["fn_80334A70"]
