"""Source shapes for fn_80228348 of Game/GrindGadget.cpp (wave 39): retail sets the default path
file (r0), reads the stage into r3 and overrides the file for stage 13."""
SYMBOLS = {"fn_80228348": 'extern "C" void fn_80228348(void) {'}
T = '    fn_8006B474("GRINDGADGET.BIN", 1);\n    fn_8006B474(*s_pathFile, 0);\n'
BODIES = {"fn_80228348": {
    "default_override": "    const char *const *file = &lbl_805F6F18;\n    if (lbl_8057D6A0.m_stage == 13) {\n        file = &lbl_805F6F1C;\n    }\n    s_pathFile = file;\n" + T,
    "static_override": "    s_pathFile = &lbl_805F6F18;\n    if (lbl_8057D6A0.m_stage == 13) {\n        s_pathFile = &lbl_805F6F1C;\n    }\n" + T,
    "stage_local": "    const char *const *file = &lbl_805F6F18;\n    s32 stage = lbl_8057D6A0.m_stage;\n    if (stage == 13) {\n        file = &lbl_805F6F1C;\n    }\n    s_pathFile = file;\n" + T,
    "switch": "    const char *const *file = &lbl_805F6F18;\n    switch (lbl_8057D6A0.m_stage) {\n    case 13:\n        file = &lbl_805F6F1C;\n        break;\n    }\n    s_pathFile = file;\n" + T,
    "decl_then": "    const char *const *file;\n    file = &lbl_805F6F18;\n    if (lbl_8057D6A0.m_stage == 13) {\n        file = &lbl_805F6F1C;\n    }\n    s_pathFile = file;\n" + T,
}}
