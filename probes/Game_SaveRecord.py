"""Source shapes for fn_8016F4E8 of Game/SaveRecord.cpp (wave 39).

Retail keeps the stage id in r30, the register the entry offset (index * 0xC0) takes next; GC/2.7
puts the id in r28.  Everything else matches.
"""

SYMBOLS = {
    "fn_8016F4E8": "extern \"C\" void fn_8016F4E8(UnkCheckpoints *points) {",
}

HEAD = """    points->m_last = 0;
    points->m_frame = 0;
    points->m_time = 0.0f;
"""
LOOP = """    for (int i = 0; i < 2; i++) {
        fn_800091D0(&points->m_points[i][0].m_position, &stage->m_starts[i].m_position);
        points->m_points[i][0].m_rotation.x = stage->m_starts[i].m_rotation.x;
        points->m_points[i][0].m_rotation.y = stage->m_starts[i].m_rotation.y;
        points->m_points[i][0].m_rotation.z = stage->m_starts[i].m_rotation.z;
        for (int j = 0; j < 32; j++) {
            points->m_points[i][j].m_valid = 0;
        }
    }
"""

BODIES = {
    "fn_8016F4E8": {
        "committed": HEAD + """    int id = lbl_8057D6A0.m_stageId;
    UnkStageInfo_800CBAA4 *stage = &fn_800CBAA4()->m_stages[fn_80176C04(fn_800B74B4(), id)];
""" + LOOP,
        "reuse_index": HEAD + """    int index = lbl_8057D6A0.m_stageId;
    index = fn_80176C04(fn_800B74B4(), index);
    UnkStageInfo_800CBAA4 *stage = &fn_800CBAA4()->m_stages[index];
""" + LOOP,
        "two_ints": HEAD + """    int id = lbl_8057D6A0.m_stageId;
    int index = fn_80176C04(fn_800B74B4(), id);
    UnkStageInfo_800CBAA4 *stage = &fn_800CBAA4()->m_stages[index];
""" + LOOP,
        "table_local": HEAD + """    int id = lbl_8057D6A0.m_stageId;
    int index = fn_80176C04(fn_800B74B4(), id);
    UnkStageTable_800CBAA4 *table = fn_800CBAA4();
    UnkStageInfo_800CBAA4 *stage = &table->m_stages[index];
""" + LOOP,
        "stage_idx_in_loop": HEAD + """    int id = lbl_8057D6A0.m_stageId;
    int index = fn_80176C04(fn_800B74B4(), id);
    UnkStageTable_800CBAA4 *table = fn_800CBAA4();
    for (int i = 0; i < 2; i++) {
        fn_800091D0(&points->m_points[i][0].m_position, &table->m_stages[index].m_starts[i].m_position);
        points->m_points[i][0].m_rotation.x = table->m_stages[index].m_starts[i].m_rotation.x;
        points->m_points[i][0].m_rotation.y = table->m_stages[index].m_starts[i].m_rotation.y;
        points->m_points[i][0].m_rotation.z = table->m_stages[index].m_starts[i].m_rotation.z;
        for (int j = 0; j < 32; j++) {
            points->m_points[i][j].m_valid = 0;
        }
    }
""",
        "reuse_i": HEAD + """    int i = lbl_8057D6A0.m_stageId;
    UnkStageInfo_800CBAA4 *stage = &fn_800CBAA4()->m_stages[fn_80176C04(fn_800B74B4(), i)];
    for (i = 0; i < 2; i++) {
        fn_800091D0(&points->m_points[i][0].m_position, &stage->m_starts[i].m_position);
        points->m_points[i][0].m_rotation.x = stage->m_starts[i].m_rotation.x;
        points->m_points[i][0].m_rotation.y = stage->m_starts[i].m_rotation.y;
        points->m_points[i][0].m_rotation.z = stage->m_starts[i].m_rotation.z;
        for (int j = 0; j < 32; j++) {
            points->m_points[i][j].m_valid = 0;
        }
    }
""",
        "reuse_j": HEAD + """    int i, j;
    j = lbl_8057D6A0.m_stageId;
    UnkStageInfo_800CBAA4 *stage = &fn_800CBAA4()->m_stages[fn_80176C04(fn_800B74B4(), j)];
    for (i = 0; i < 2; i++) {
        fn_800091D0(&points->m_points[i][0].m_position, &stage->m_starts[i].m_position);
        points->m_points[i][0].m_rotation.x = stage->m_starts[i].m_rotation.x;
        points->m_points[i][0].m_rotation.y = stage->m_starts[i].m_rotation.y;
        points->m_points[i][0].m_rotation.z = stage->m_starts[i].m_rotation.z;
        for (j = 0; j < 32; j++) {
            points->m_points[i][j].m_valid = 0;
        }
    }
""",
        "ptr_walk": HEAD + """    int id = lbl_8057D6A0.m_stageId;
    UnkStageStart_800CBAA4 *start = fn_800CBAA4()->m_stages[fn_80176C04(fn_800B74B4(), id)].m_starts;
    for (int i = 0; i < 2; i++) {
        fn_800091D0(&points->m_points[i][0].m_position, &start[i].m_position);
        points->m_points[i][0].m_rotation.x = start[i].m_rotation.x;
        points->m_points[i][0].m_rotation.y = start[i].m_rotation.y;
        points->m_points[i][0].m_rotation.z = start[i].m_rotation.z;
        for (int j = 0; j < 32; j++) {
            points->m_points[i][j].m_valid = 0;
        }
    }
""",
        "offset_var": HEAD + """    int id = lbl_8057D6A0.m_stageId;
    id = fn_80176C04(fn_800B74B4(), id) * sizeof(UnkStageInfo_800CBAA4);
    UnkStageInfo_800CBAA4 *stage = (UnkStageInfo_800CBAA4 *)((char *)fn_800CBAA4()->m_stages + id);
""" + LOOP,
        "offset_var2": HEAD + """    int id = lbl_8057D6A0.m_stageId;
    int offset = fn_80176C04(fn_800B74B4(), id) * sizeof(UnkStageInfo_800CBAA4);
    UnkStageInfo_800CBAA4 *stage = (UnkStageInfo_800CBAA4 *)((char *)fn_800CBAA4()->m_stages + offset);
""" + LOOP,
        "inline_get": HEAD + """    UnkStageInfo_800CBAA4 *stage = &fn_800CBAA4()->m_stages[fn_80176C04(fn_800B74B4(), lbl_8057D6A0.m_stageId)];
""" + LOOP,
        "const_id": HEAD + """    const int id = lbl_8057D6A0.m_stageId;
    const UnkStageInfo_800CBAA4 &stage_ = fn_800CBAA4()->m_stages[fn_80176C04(fn_800B74B4(), id)];
    const UnkStageInfo_800CBAA4 *stage = &stage_;
""" + LOOP,
    },
}
