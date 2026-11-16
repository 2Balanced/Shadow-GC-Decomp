"""UnkHeadTurn::Update (wave 39): retail pools 1.0f after 0.5f although the code never loads it."""
SCORE = ["Update__Q36Player6Shadow20UnkHeadTurn_800B1308FPQ36Player6Shadow20UnkNodeArgs_800B10CCf"]
O = """    float t;
    if (angle >= speed) {
        t = speed / angle;
        if (t >= 0.5f) {
            t = 0.5f;
        }
    } else {
        t = 0.5f;
    }
"""
REPL = {
 "v1": [(O, """    float t;
    if (angle >= speed) {
        t = speed / angle;
        if (t >= 0.5f) {
            t = 0.5f;
        }
    } else {
        t = 1.0f;
        if (t >= 0.5f) {
            t = 0.5f;
        }
    }
""")],
 "v2": [(O, """    float t;
    if (angle >= speed) {
        t = speed / angle;
    } else {
        t = 1.0f;
    }
    if (t >= 0.5f) {
        t = 0.5f;
    }
""")],
 "v3": [(O, """    float t = 1.0f;
    if (angle >= speed) {
        t = speed / angle;
    }
    if (t >= 0.5f) {
        t = 0.5f;
    }
""")],
 "v4": [(O, """    float t = angle >= speed ? speed / angle : 1.0f;
    if (t >= 0.5f) {
        t = 0.5f;
    }
""")],
}
