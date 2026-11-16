"""fn_80334A70 of Game/EggLastMechArea.cpp (wave 39, second pass): index types and forms that keep
the m_areaTime address apart from SetArea's."""
S = "inline void SetArea(UnkPart_80334C78 *part, int index, int area) {\n"
T = "part->m_areaTime[i] +="
F = "    for (int i = 0; i < 2; i++) {\n        boost::shared_ptr<CharInfo> character = part->m_characters[i].lock();"
REPL = {
 "set_u32": [(S, S.replace("int index", "u32 index"))],
 "set_ref": [(S, S.replace("int index", "const int &index"))],
 "set_s16": [(S, S.replace("int index", "s16 index"))],
 "loop_u32": [(F, F.replace("int i = 0", "u32 i = 0"))],
 "cast_float9": [(T, "reinterpret_cast<float *>(part)[i + 9] +=")],
 "time_m1": [(T, "(part->m_areaTime + 1)[i - 1] +=")],
 "set_ptr": [(S, "inline void SetArea(UnkPart_80334C78 *part, int index, int area) {\n    s32 *p = &part->m_area[index];\n    if (*p != area) {\n        p[2] = *p;\n        *p = area;\n        *(float *)&p[4] = 0.0f;\n    }\n}\ninline void SetAreaX(UnkPart_80334C78 *part, int index, int area) {\n")],
}
SCORE = ["fn_80334A70"]
