"""fn_80334A70 of Game/EggLastMechArea.cpp (wave 39, second pass): retail adds to m_areaTime[i]
through part + (i * 4 + 0x24) with lfsx/stfsx."""
OLD = """        if (scale.m_scale != 0) {
            part->m_areaTime[i] += **reinterpret_cast<float *const *>(character.get()->field_0x60);
        } else {
            part->m_areaTime[i] += lbl_8057D6A0.m_frameTime;
        }
"""
A = 'extern "C" void fn_80334A70(UnkPart_80334C78 *part) {\n'
ADD = "inline void AddTime(float &time, float dt) {\n    time += dt;\n}\n\n"
ADDP = "inline void AddTime(UnkPart_80334C78 *part, int index, float dt) {\n    part->m_areaTime[index] += dt;\n}\n\n"
REPL = {
 "ref_before": [(OLD, OLD.replace("        if (scale.m_scale != 0) {", "        float &time = part->m_areaTime[i];\n        if (scale.m_scale != 0) {").replace("part->m_areaTime[i] +=", "time +="))],
 "inl_ref": [(A, ADD + A), (OLD, """        if (scale.m_scale != 0) {
            AddTime(part->m_areaTime[i], **reinterpret_cast<float *const *>(character.get()->field_0x60));
        } else {
            AddTime(part->m_areaTime[i], lbl_8057D6A0.m_frameTime);
        }
""")],
 "inl_part": [(A, ADDP + A), (OLD, """        if (scale.m_scale != 0) {
            AddTime(part, i, **reinterpret_cast<float *const *>(character.get()->field_0x60));
        } else {
            AddTime(part, i, lbl_8057D6A0.m_frameTime);
        }
""")],
 "ptr_each": [(OLD, """        if (scale.m_scale != 0) {
            float *time = &part->m_areaTime[i];
            *time += **reinterpret_cast<float *const *>(character.get()->field_0x60);
        } else {
            float *time = &part->m_areaTime[i];
            *time += lbl_8057D6A0.m_frameTime;
        }
""")],
 "dt_local": [(OLD, """        float dt;
        if (scale.m_scale != 0) {
            dt = **reinterpret_cast<float *const *>(character.get()->field_0x60);
        } else {
            dt = lbl_8057D6A0.m_frameTime;
        }
        part->m_areaTime[i] += dt;
""")],
 "assign": [(OLD, """        if (scale.m_scale != 0) {
            part->m_areaTime[i] = part->m_areaTime[i] + **reinterpret_cast<float *const *>(character.get()->field_0x60);
        } else {
            part->m_areaTime[i] = part->m_areaTime[i] + lbl_8057D6A0.m_frameTime;
        }
""")],
 "star": [(OLD, OLD.replace("part->m_areaTime[i] +=", "*(part->m_areaTime + i) +="))],
 "arr_ptr": [(OLD, OLD.replace("part->m_areaTime[i] +=", "(&part->m_areaTime[0])[i] +="))],
}
SCORE = ["fn_80334A70"]
