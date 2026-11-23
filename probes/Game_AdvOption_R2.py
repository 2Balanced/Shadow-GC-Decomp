"""Wave 40: Option::Impl::UpdateList, the item in r29 (GC/2.7: r28)."""
SCORE = ["UpdateList__Q37AdvReal6Option4ImplFv"]
OLD = "    int item = s_itemGrid[s_data.m_current.m_row][s_data.m_current.m_column];\n    if (fn_8003DBB0(fn_8003CEAC(&m_scenes, s_sceneNames[1]))) {\n"
IF = "    if (fn_8003DBB0(fn_8003CEAC(&m_scenes, s_sceneNames[1]))) {\n"
F = "void Impl::UpdateList() {"
H = "inline int CurrentItem_() { return s_itemGrid[s_data.m_current.m_row][s_data.m_current.m_column]; }\ninline int ItemAt_(int row, int column) { return s_itemGrid[row][column]; }\n" + F
REPL = {
  "getter": [(F, H), (OLD, "    int item = CurrentItem_();\n" + IF)],
  "getter2": [(F, H), (OLD, "    int item = ItemAt_(s_data.m_current.m_row, s_data.m_current.m_column);\n" + IF)],
  "const": [(OLD, "    const int item = s_itemGrid[s_data.m_current.m_row][s_data.m_current.m_column];\n" + IF)],
  "u32": [(OLD, "    u32 item = s_itemGrid[s_data.m_current.m_row][s_data.m_current.m_column];\n" + IF)],
  "s8": [(OLD, "    s32 item;\n    item = s_itemGrid[s_data.m_current.m_row][s_data.m_current.m_column];\n" + IF)],
  "rowcol": [(OLD, "    int row = s_data.m_current.m_row;\n    int column = s_data.m_current.m_column;\n    int item = s_itemGrid[row][column];\n" + IF)],
  "save_outer": [(OLD, "    int item = s_itemGrid[s_data.m_current.m_row][s_data.m_current.m_column];\n    UnkSaveData_800BB74C *save;\n" + IF), ("                UnkSaveData_800BB74C *save = fn_800BB74C();", "                save = fn_800BB74C();")],
  "save_outer_first": [(OLD, "    UnkSaveData_800BB74C *save;\n    int item = s_itemGrid[s_data.m_current.m_row][s_data.m_current.m_column];\n" + IF), ("                UnkSaveData_800BB74C *save = fn_800BB74C();", "                save = fn_800BB74C();")],
  "all_outer": [(OLD, "    int item = s_itemGrid[s_data.m_current.m_row][s_data.m_current.m_column];\n    int i;\n    int entry;\n    UnkSaveData_800BB74C *save;\n" + IF), ("                int i;\n                int entry = 0;\n                UnkSaveData_800BB74C *save = fn_800BB74C();", "                entry = 0;\n                save = fn_800BB74C();")],
  "all_outer2": [(OLD, "    UnkSaveData_800BB74C *save;\n    int entry;\n    int i;\n    int item = s_itemGrid[s_data.m_current.m_row][s_data.m_current.m_column];\n" + IF), ("                int i;\n                int entry = 0;\n                UnkSaveData_800BB74C *save = fn_800BB74C();", "                entry = 0;\n                save = fn_800BB74C();")],
  "value_inl": [(F, "inline int CurrentValue_(int item) { return s_data.m_current.m_value[item]; }\n" + F), ("                        if (entry == s_data.m_current.m_value[item]) {", "                        if (entry == CurrentValue_(item)) {")],
}
