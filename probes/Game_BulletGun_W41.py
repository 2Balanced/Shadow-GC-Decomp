"""Wave 41: BulletGun's static initializer. Retail stores the two speeds between the fourth and the
fifth record's range (right after restoring r28-r31): shapes that define the speeds between those
records, with the records moved above the functions that use the speeds (forward declarations of
the Create functions) and an unused function that creates the speeds' constants first."""
SRC = open("src/Game/BulletGun.cpp", encoding="utf-8").read()
SCORE = ["__sinit_BulletGun_cpp"]
SPEEDS = """/* The speeds of the bullets. */
static const float s_speed = Plain(500.0f);
static const float s_slowSpeed = Plain(300.0f);
"""
REC_START = SRC.index("UnkBulletType_80546018 lbl_8051DB34 = {")
REC_END = SRC.index("UnkBulletType_80546018 lbl_8051DC38 = {")
REC_END = SRC.index("\n", REC_END) + 1
RECORDS = SRC[REC_START:REC_END]
R1 = RECORDS[:RECORDS.index("UnkBulletType_80546018 lbl_8051DB84")]
R2 = RECORDS[len(R1):]
FWD = "".join("static void Create%d(const ShotInfo &info);\n" % i for i in (0, 1, 2, 3, 5, 6, 7, 8, 9, 10, 11, 12, 13))
FWD += "static HitResult HitTest9(const ShotInfo &info, float time);\nstatic HitResult HitTest10(const ShotInfo &info, float time);\n\n"
EXTERNS_END = "extern UnkBulletType_80546018 lbl_8051DC38;\n"
UNUSED = """/* Not original: never called (the linker drops it); it pools the speeds' constants ahead of the
 * records' ones, as retail has them. */
void BulletGunSpeedOrder_(float *speeds) {
    speeds[0] = 500.0f;
    speeds[1] = 300.0f;
}

"""
def build(mid, unused=True):
    t = SRC.replace(SPEEDS, "")
    t = t.replace(RECORDS, "")
    t = t.replace(EXTERNS_END, EXTERNS_END + "\n" + (UNUSED if unused else "") + FWD + mid)
    return t
V = {}
V["mid_unused"] = build(R1 + SPEEDS + R2)
V["mid"] = build(R1 + SPEEDS + R2, unused=False)
V["after_unused"] = build(RECORDS + SPEEDS)
V["before_unused"] = build(SPEEDS + RECORDS)
REPL = {}
for k, v in V.items():
    REPL[k] = [(SRC, v)]
