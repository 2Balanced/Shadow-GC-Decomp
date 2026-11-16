"""PlayerShadowMotion (wave 39, second pass): Step800B2EA4 and the end of Walk."""
S = "Step800B2EA4__Q36Player6Shadow12ShadowMotionFv"
W = "Walk__Q36Player6Shadow12ShadowMotionFv"
SCORE = [S, W]
OS = """        int motion = RunSpeed() > 0.0f ? 0x90 : 0x8E;
        m_step++;
        next = m_motionBase - 1 + motion;
"""
OW = """    int base = m_motionBase - 1;
    m_speed = RunSpeed();
    int next = a + base;
    int nextArg = b + base;
"""
REPL = {}
SV = {
 "s1": """        int motion = RunSpeed() > 0.0f ? 0x90 : 0x8E;
        m_step++;
        next = m_motionBase + motion - 1;
""",
 "s2": """        int motion = RunSpeed() > 0.0f ? 0x90 : 0x8E;
        next = m_motionBase + motion - 1;
        m_step++;
""",
 "s3": """        int motion = RunSpeed() > 0.0f ? 0x90 : 0x8E;
        next = m_motionBase + motion;
        m_step++;
        next--;
""",
 "s4": """        int motion = RunSpeed() > 0.0f ? 0x90 : 0x8E;
        m_step++;
        next = motion + m_motionBase - 1;
""",
 "s5": """        int motion = RunSpeed() > 0.0f ? 0x90 : 0x8E;
        m_step++;
        next = (m_motionBase + motion) - 1;
""",
 "s6": """        int motion = RunSpeed() > 0.0f ? 0x90 : 0x8E;
        next = motion + m_motionBase - 1;
        m_step++;
""",
 "s7": """        int motion = RunSpeed() > 0.0f ? 0x90 : 0x8E;
        m_step++;
        next = m_motionBase + (motion - 1);
""",
}
WV = {
 "w1": """    int next;
    int nextArg;
    int base = m_motionBase - 1;
    m_speed = RunSpeed();
    next = a + base;
    nextArg = b + base;
""",
 "w2": """    m_speed = RunSpeed();
    int base = m_motionBase - 1;
    int next = a + base;
    int nextArg = b + base;
""",
 "w3": """    int base = m_motionBase - 1;
    m_speed = RunSpeed();
    a += base;
    b += base;
    int next = a;
    int nextArg = b;
""",
 "w4": """    int base = m_motionBase;
    m_speed = RunSpeed();
    int next = a + (base - 1);
    int nextArg = b + (base - 1);
""",
 "w5": """    m_speed = RunSpeed();
    int next = a + (m_motionBase - 1);
    int nextArg = b + (m_motionBase - 1);
""",
 "w6": """    int base = m_motionBase - 1;
    m_speed = RunSpeed();
    int next = a + base;
    int nextArg = base + b;
""",
 "w7": """    int base = m_motionBase - 1;
    m_speed = RunSpeed();
    int nextArg = b + base;
    int next = a + base;
""",
}
for k, b in SV.items():
    REPL[k] = [(OS, b)]
for k, b in WV.items():
    REPL[k] = [(OW, b)]
SCORE = [S]
REPL = {}
H = "        int motion = RunSpeed() > 0.0f ? 0x90 : 0x8E;\n"
SV = {
 "t1": H + "        next = motion + m_motionBase;\n        m_step++;\n        next--;\n",
 "t2": H + "        next = motion + m_motionBase;\n        next--;\n        m_step++;\n",
 "t3": H + "        m_step++;\n        next = motion + m_motionBase;\n        next--;\n",
 "t4": H + "        next = (motion + m_motionBase) - 1;\n        m_step++;\n",
 "t5": H + "        int base = m_motionBase;\n        m_step++;\n        next = base + motion - 1;\n",
 "t6": H + "        int step = m_step;\n        next = m_motionBase + motion - 1;\n        m_step = step + 1;\n",
 "t7": H + "        int step = m_step;\n        int base = m_motionBase;\n        m_step = step + 1;\n        next = base + motion - 1;\n",
 "t8": H + "        next = m_motionBase + motion;\n        next--;\n        m_step++;\n",
 "t9": H + "        m_step++;\n        next = m_motionBase + motion;\n        next -= 1;\n",
 "t10": H + "        next = motion - 1 + m_motionBase;\n        m_step++;\n",
 "t11": H + "        next = motion + m_motionBase - 1;\n        m_step += 1;\n",
 "t12": H + "        next = motion;\n        next += m_motionBase - 1;\n        m_step++;\n",
 "t13": H + "        next = motion;\n        m_step++;\n        next += m_motionBase;\n        next--;\n",
}
for k, b in SV.items():
    REPL[k] = [(OS, b)]
REPL = {}
H2 = "        int motion;\n        if (RunSpeed() > 0.0f) {\n            motion = 0x90;\n        } else {\n            motion = 0x8E;\n        }\n"
TAILS = {
 "a": "        next = m_motionBase + motion;\n        m_step++;\n        next--;\n",
 "b": "        next = m_motionBase + motion;\n        next--;\n        m_step++;\n",
 "c": "        m_step++;\n        next = m_motionBase + motion;\n        next--;\n",
 "d": "        m_step++;\n        next = m_motionBase + motion - 1;\n",
 "e": "        next = m_motionBase + motion - 1;\n        m_step++;\n",
 "f": "        m_step++;\n        next = m_motionBase - 1 + motion;\n",
 "g": "        next = m_motionBase - 1 + motion;\n        m_step++;\n",
}
for k, b in TAILS.items():
    REPL["if_" + k] = [(OS, H2 + b)]
REPL = {}
AN = "void ShadowMotion::Step800B2EA4() {"
P1 = "inline int MotionBase(const ShadowMotion *m) {\n    return m->m_motionBase;\n}\n"
P2 = "inline int MotionId(const ShadowMotion *m, int motion) {\n    return m->m_motionBase + motion - 1;\n}\n"
P3 = "inline int MotionId(const ShadowMotion *m, int motion) {\n    return m->m_motionBase - 1 + motion;\n}\n"
P4 = "inline int MotionBase(ShadowMotion *m) {\n    return m->m_motionBase;\n}\n"
TA = {
 "g1": (P1, "        next = MotionBase(this) + motion - 1;\n        m_step++;\n"),
 "g2": (P1, "        m_step++;\n        next = MotionBase(this) + motion - 1;\n"),
 "g3": (P2, "        next = MotionId(this, motion);\n        m_step++;\n"),
 "g4": (P2, "        m_step++;\n        next = MotionId(this, motion);\n"),
 "g5": (P3, "        m_step++;\n        next = MotionId(this, motion);\n"),
 "g6": (P4, "        next = MotionBase(this) + motion - 1;\n        m_step++;\n"),
 "g7": (P4, "        m_step++;\n        next = MotionBase(this) + motion - 1;\n"),
 "g8": (P1, "        next = MotionBase(this) + motion;\n        m_step++;\n        next--;\n"),
}
for k, (p, b) in TA.items():
    REPL[k] = [(OS, H + b), (AN, p + AN)]
REPL = {}
TA = {
 "h1": (P1, "        next = motion + MotionBase(this);\n        m_step++;\n        next--;\n"),
 "h2": (P1, "        next = motion + MotionBase(this) - 1;\n        m_step++;\n"),
 "h3": (P1, "        next = motion + MotionBase(this);\n        next--;\n        m_step++;\n"),
 "h4": (P1, "        m_step++;\n        next = motion + MotionBase(this) - 1;\n"),
 "h5": (P1, "        next = motion - 1 + MotionBase(this);\n        m_step++;\n"),
}
for k, (p, b) in TA.items():
    REPL[k] = [(OS, H + b), (AN, p + AN)]
REPL = {}
Q = "RunSpeed() > 0.0f ? 0x90 : 0x8E"
BV = {
 "j1": "        int step;\n        int base;\n        int motion;\n        motion = %s;\n        step = m_step;\n        base = m_motionBase;\n        m_step = step + 1;\n        next = base - 1 + motion;\n" % Q,
 "j2": "        int base;\n        int motion;\n        motion = %s;\n        m_step++;\n        base = m_motionBase;\n        next = base - 1 + motion;\n" % Q,
 "j3": "        int base;\n        int motion = %s;\n        m_step++;\n        base = m_motionBase - 1;\n        next = base + motion;\n" % Q,
 "j4": "        int base;\n        int motion = %s;\n        m_step++;\n        base = m_motionBase;\n        next = base + motion - 1;\n" % Q,
 "j5": "        int base;\n        int motion = %s;\n        base = m_motionBase;\n        m_step++;\n        next = base + motion - 1;\n" % Q,
 "j6": "        int motion = %s;\n        m_step++;\n        int base = m_motionBase;\n        next = base - 1 + motion;\n" % Q,
 "j7": "        int motion = %s;\n        m_step++;\n        int base = m_motionBase;\n        next = base + motion - 1;\n" % Q,
 "j8": "        int motion = %s;\n        m_step++;\n        int base = m_motionBase - 1;\n        next = base + motion;\n" % Q,
}
for k, b in BV.items():
    REPL[k] = [(OS, b)]
REPL = {}
BV = {
 "k1": H + "        next = motion;\n        next += m_motionBase;\n        m_step++;\n        next--;\n",
 "k2": H + "        next = m_motionBase;\n        next += motion;\n        m_step++;\n        next--;\n",
 "k3": H + "        int id = motion + m_motionBase;\n        m_step++;\n        next = id - 1;\n",
 "k4": H + "        motion += m_motionBase;\n        m_step++;\n        next = motion - 1;\n",
 "k5": H + "        next = m_motionBase + motion;\n        m_step++;\n        next = next - 1;\n",
 "k6": H + "        next = motion + m_motionBase;\n        m_step++;\n        next = next - 1;\n",
 "k7": H + "        next = -1 + (motion + m_motionBase);\n        m_step++;\n",
 "k8": H + "        next = (motion + m_motionBase) + -1;\n        m_step++;\n",
 "k9": H + "        next = motion + (m_motionBase - 1);\n        m_step++;\n",
 "k10": "        next = (%s) + m_motionBase;\n        m_step++;\n        next--;\n" % Q,
 "k11": "        next = m_motionBase + (%s);\n        m_step++;\n        next--;\n" % Q,
 "k12": "        next = %s;\n        next = m_motionBase + next;\n        m_step++;\n        next--;\n" % Q,
 "k13": "        next = %s;\n        next += m_motionBase;\n        m_step++;\n        next--;\n" % Q,
 "k14": "        next = %s;\n        next = m_motionBase + next - 1;\n        m_step++;\n" % Q,
}
for k, b in BV.items():
    REPL[k] = [(OS, b)]
