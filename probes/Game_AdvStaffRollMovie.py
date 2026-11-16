"""AdvStaffRollMovie (wave 39, second pass): const Context parameters."""
P = "Q37AdvReal9StaffRoll5MovieFPQ37AdvReal9StaffRoll7Context"
PC = "Q37AdvReal9StaffRoll5MovieFPCQ37AdvReal9StaffRoll7Context"
SCORE = [("__ct__" + P, "__ct__" + PC), ("Draw__" + P, "Draw__" + PC), ("BuildList__" + P, "BuildList__" + PC), "DrawFrame__Q37AdvReal9StaffRoll5MovieFllf"]
REPL = {"const": [("Movie::Movie(Context *context) {", "Movie::Movie(const Context *context) {"),
                  ("void Movie::Draw(Context *context) {", "void Movie::Draw(const Context *context) {"),
                  ("void Movie::BuildList(Context *context) {", "void Movie::BuildList(const Context *context) {")]}
HDR = {"const": {"Game/AdvStaffRoll.h": [("    Movie(Context *context);", "    Movie(const Context *context);"),
                                          ("    void Draw(Context *context);\n    void UpdateMovie", "    void Draw(const Context *context);\n    void UpdateMovie"),
                                          ("    void BuildList(Context *context);", "    void BuildList(const Context *context);")]}}
SCORE = ["DrawFrame__Q37AdvReal9StaffRoll5MovieFllf"]
HDR = {}
D = """    float outerLeft;
    float innerLeft;
    float outerTop;
    float innerTop;
    float outerBottom;
    float outerRight;
    float innerBottom;
    float innerRight;
"""
IB = "    innerBottom = frameY[3] * scale + y;\n"
IR = "    innerRight = frameX[3] * scale + x;\n"
OR = "    outerRight = frameX[1] * scale + x;\n"
REPL = {
 "v1": [(D, D.replace("    float innerBottom;\n", "")), (IB, "    float " + IB.strip() + "\n")],
 "v2": [(D, D.replace("    float innerRight;\n", "")), (IR, "    float " + IR.strip() + "\n")],
 "v3": [(D, D.replace("    float innerBottom;\n", "").replace("    float innerRight;\n", "")), (IB, "    float " + IB.strip() + "\n"), (IR, "    float " + IR.strip() + "\n")],
 "v4": [(D, D.replace("    float innerBottom;\n", "").replace("    float innerRight;\n", "").replace("    float outerRight;\n", "")), (IB, "    float " + IB.strip() + "\n"), (IR, "    float " + IR.strip() + "\n"), (OR, "    float " + OR.strip() + "\n")],
 "v5": [(D, D.replace("    float outerBottom;\n", "    float outerBottom;\n    float s = scale;\n"))] ,
 "v6": [(D, "    float s = scale;\n" + D)],
 "v7": [(D, D.replace("    float outerRight;\n", "    float outerRight;\n    float fx;\n    float fy;\n"))],
}
for k in ("v5", "v6"):
    REPL[k].append(("    outerLeft = frameX[0] * scale + x;\n    outerTop = frameY[0] * scale + y;\n    innerLeft = frameX[2] * scale + x;\n    innerTop = frameY[2] * scale + y;\n    outerBottom = frameY[1] * scale + y;\n    innerBottom = frameY[3] * scale + y;\n",
                    "    outerLeft = frameX[0] * s + x;\n    outerTop = frameY[0] * s + y;\n    innerLeft = frameX[2] * s + x;\n    innerTop = frameY[2] * s + y;\n    outerBottom = frameY[1] * s + y;\n    innerBottom = frameY[3] * s + y;\n"))
    REPL[k].append((OR + IR, OR.replace("scale", "s") + IR.replace("scale", "s")))
SCORE = [("__ct__" + P, "__ct__" + PC)]
C = [("Movie::Movie(Context *context) {", "Movie::Movie(const Context *context) {"), ("void Movie::BuildList(Context *context) {", "void Movie::BuildList(const Context *context) {")]
H = {"Game/AdvStaffRoll.h": [("    Movie(Context *context);", "    Movie(const Context *context);"), ("    void BuildList(Context *context);", "    void BuildList(const Context *context);")]}
O = """    s32 type = context->m_type;
    u32 i;
    u32 movie;
    MovieEntry *entry;
    Page *page;
    File *file;
    m_time = -1.0f;
    m_entries = 0;
    m_current = 0;
    m_brightness = 1.0f;
    movie = 0;
    if (type != 4) {
"""
DECL = "    u32 i;\n    u32 movie;\n    MovieEntry *entry;\n    Page *page;\n    File *file;\n"
ST = "    m_time = -1.0f;\n    m_entries = 0;\n    m_current = 0;\n    m_brightness = 1.0f;\n"
REPL = {
 "c1": C + [(O, "    s32 type = context->m_type;\n" + DECL + "    movie = 0;\n" + ST + "    if (type != 4) {\n")],
 "c2": C + [(O, DECL + "    movie = 0;\n" + ST + "    if (context->m_type != 4) {\n")],
 "c3": C + [(O, DECL + ST + "    movie = 0;\n    if (context->m_type != 4) {\n")],
 "c4": C + [(O, DECL + "    m_time = -1.0f;\n    movie = 0;\n    m_entries = 0;\n    m_current = 0;\n    m_brightness = 1.0f;\n    if (context->m_type != 4) {\n")],
 "c5": C + [(O, DECL + "    movie = 0;\n    m_time = -1.0f;\n    m_entries = (MovieEntry *)movie;\n    m_current = (MovieEntry *)movie;\n    m_brightness = 1.0f;\n    if (context->m_type != 4) {\n")],
}
HDR = {k: H for k in REPL}
T = "    if (context->m_type != 4) {\n"
REPL = {
 "k1": C + [(O, DECL + "    m_time = -1.0f;\n    m_current = m_entries = 0;\n    m_brightness = 1.0f;\n    movie = 0;\n" + T)],
 "k2": C + [(O, DECL + "    m_time = -1.0f;\n    movie = (u32)(m_current = m_entries = 0);\n    m_brightness = 1.0f;\n" + T)],
 "k3": C + [(O, DECL + "    m_time = -1.0f;\n    m_entries = 0;\n    movie = (u32)(m_current = 0);\n    m_brightness = 1.0f;\n" + T)],
 "k4": C + [(O, DECL + "    movie = 0;\n    m_time = -1.0f;\n    m_entries = 0;\n    m_current = 0;\n    m_brightness = 1.0f;\n" + T), ("        entry->m_firstPage = 0;\n", "        entry->m_firstPage = movie;\n")],
 "k5": C + [(O, DECL.replace("    u32 movie;\n", "    u32 movie = 0;\n") + ST + T)],
 "k6": C + [(O, DECL.replace("    u32 i;\n    u32 movie;\n", "    u32 movie = 0;\n    u32 i;\n") + ST + T)],
}
HDR = {k: H for k in REPL}
