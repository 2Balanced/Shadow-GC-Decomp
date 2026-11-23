"""fn_8003A380 (wave 40): retail has the block data in r27, the constant 1 in r26 and the found
array's address in r25; GC/2.7 gave 1, the array, the data."""
O = """    UnkBlockData_8003AB68 *data;
    s16 i;
    UnkBlockBox_8003AB68 *boxes = self->m_data->m_boxes;
    s16 count = 0;
    u8 found[TBlockManage::kBlockCount];
    memset(found, 0, sizeof(found));
    data = self->m_data;
    for (i = 0; i < data->m_count; i++) {
        UnkBlockBox_8003AB68 *box = &boxes[i];
        s32 id = box->m_id;
        if (self->IsInside(&box->m_min, &box->m_max, position) && !found[id]) {
            found[id] = 1;
            count++;
        }
    }
"""
L = """        UnkBlockBox_8003AB68 *box = &boxes[i];
        s32 id = box->m_id;
        if (self->IsInside(&box->m_min, &box->m_max, position) && !found[id]) {
            found[id] = 1;
            count++;
        }
    }
"""
REPL = {
 "nodata": [(O, """    s16 i;
    UnkBlockBox_8003AB68 *boxes = self->m_data->m_boxes;
    s16 count = 0;
    u8 found[TBlockManage::kBlockCount];
    memset(found, 0, sizeof(found));
    for (i = 0; i < self->m_data->m_count; i++) {
""" + L)],
 "data_init": [(O, """    s16 i;
    UnkBlockBox_8003AB68 *boxes = self->m_data->m_boxes;
    s16 count = 0;
    u8 found[TBlockManage::kBlockCount];
    memset(found, 0, sizeof(found));
    UnkBlockData_8003AB68 *data = self->m_data;
    for (i = 0; i < data->m_count; i++) {
""" + L)],
 "ptr_found": [(O, """    UnkBlockData_8003AB68 *data;
    s16 i;
    UnkBlockBox_8003AB68 *boxes = self->m_data->m_boxes;
    s16 count = 0;
    u8 found[TBlockManage::kBlockCount];
    memset(found, 0, sizeof(found));
    data = self->m_data;
    u8 *seen = found;
    for (i = 0; i < data->m_count; i++) {
""" + L.replace("found[id]", "seen[id]"))],
 "true": [(O, O.replace("found[id] = 1;", "found[id] = true;"))],
 "bool_found": [(O, O.replace("u8 found[", "bool found[").replace("found[id] = 1;", "found[id] = true;"))],
 "count_first": [(O, O.replace("    UnkBlockData_8003AB68 *data;\n    s16 i;\n", "").replace("    s16 count = 0;\n", "    s16 count = 0;\n    UnkBlockData_8003AB68 *data;\n    s16 i;\n"))],
 "data_last": [(O, O.replace("    UnkBlockData_8003AB68 *data;\n    s16 i;\n", "    s16 i;\n").replace("    u8 found[TBlockManage::kBlockCount];\n", "    u8 found[TBlockManage::kBlockCount];\n    UnkBlockData_8003AB68 *data;\n"))],
 "found_first": [(O, O.replace("    u8 found[TBlockManage::kBlockCount];\n", "").replace("    UnkBlockData_8003AB68 *data;\n", "    u8 found[TBlockManage::kBlockCount];\n    UnkBlockData_8003AB68 *data;\n"))],
 "one": [(O, O.replace("    s16 count = 0;\n", "    s16 count = 0;\n    u8 one = 1;\n").replace("found[id] = 1;", "found[id] = one;"))],
 "inc": [(O, O.replace("found[id] = 1;", "found[id]++;"))],
 "cnt_m": [(O, O.replace("i < data->m_count", "i < (s16)data->m_count"))],
 "while": [(O, O.replace("    for (i = 0; i < data->m_count; i++) {\n", "    i = 0;\n    while (i < data->m_count) {\n").replace("            count++;\n        }\n    }\n", "            count++;\n        }\n        i++;\n    }\n"))],
}
D1 = "s32 fn_8003A380(TBlockManage *self, s16 **ids, const Vec3f *position);\n"
D2 = "extern \"C\" s32 fn_8003A380(TBlockManage *self, s16 **ids, const Vec3f *position) {\n"
C = [(D1, D1.replace("TBlockManage *self", "const TBlockManage *self")), (D2, D2.replace("TBlockManage *self", "const TBlockManage *self"))]
IS = "self->IsInside("
IC = "const_cast<TBlockManage *>(self)->IsInside("
REPL2 = {
 "const": C + [(O, O.replace(IS, IC))],
 "const_nodata": C + [(O, REPL["nodata"][0][1].replace(IS, IC))],
 "const_init": C + [(O, REPL["data_init"][0][1].replace(IS, IC))],
 "const_cdata": C + [(O, O.replace(IS, IC).replace("    UnkBlockData_8003AB68 *data;", "    const UnkBlockData_8003AB68 *data;"))],
}
REPL.update(REPL2)
SCORE = ["fn_8003A380"]
