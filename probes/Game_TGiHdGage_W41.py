"""Wave 41: TGiHdGage::Tick. Retail addresses the gauge as this + i*8 (r30) with the member
offsets 48/52, keeps the save data's gauges in r28 below the counter (r29), and reads the scene
before the empty scene in the null test. The conversion slot is already shared (both 16(r1))."""
T = "Tick__9TGiHdGageFf"
SYMBOLS = {T: "void TGiHdGage::Tick(float dt) {"}

SAVED = "    UnkHdGauge_80079808 *saved = (UnkHdGauge_80079808 *)((u8 *)fn_80079808() + 0x70C);\n"


def body(top, loop, cond, named_value, inner_decl=""):
    if named_value:
        v = "value->"
        vdecl = "            UnkHdGauge_80079808 *value = &saved[i];\n"
    else:
        v = "saved[i]."
        vdecl = ""
    g = "m_gauges[i]."
    return (top + "    " + loop + " {\n"
            "        RCPtr<CScene> scene = fn_8036D660(m_gauges[i].m_project, s_files[i].m_scene);\n"
            "        if (" + cond + ") {\n"
            "            int frames = fn_80366728(scene.Get());\n" + vdecl + inner_decl +
            "            if (" + v + "m_value > " + g + "m_value) {\n"
            "                " + g + "m_value = (" + g + "m_value + " + v + "m_value) / 2;\n"
            "            } else {\n"
            "                " + g + "m_value = " + v + "m_value;\n"
            "            }\n"
            "            if (" + g + "m_value > " + v + "m_max) {\n"
            "                " + g + "m_value = " + v + "m_max;\n"
            "            }\n"
            "            fn_80366740(scene.Get(), frames * " + g + "m_value / " + v + "m_max);\n"
            "            fn_8036DAD4(m_gauges[i].m_project);\n"
            "        }\n"
            "    }\n")


C = "scene.Get() != lbl_805EE118.Get()"
CR = "lbl_805EE118.Get() != scene.Get()"
LD = "for (int i = 0; i < 2; i++)"
LI = "for (i = 0; i < 2; i++)"
V = {}
for cn, c in (("c", C), ("cr", CR)):
    for vn, nv in (("val", True), ("noval", False)):
        V["sfirst_%s_%s" % (cn, vn)] = body(SAVED, LD, c, nv)
        V["itop_%s_%s" % (cn, vn)] = body("    int i;\n" + SAVED, LI, c, nv)
        V["iafter_%s_%s" % (cn, vn)] = body(SAVED + "    int i;\n", LI, c, nv)
BODIES = {T: V}

# Round 2: the counter declared first and no named gauge (1 row left: the compare's operands).
HELP = "void TGiHdGage::Tick(float dt) {"
DIFF = """/* Whether two scene pointers hold different scenes (name inferred). */
inline bool Differ_(const RCPtr<CScene> &a, const RCPtr<CScene> &b) {
    CScene *pa = a.Get();
    CScene *pb = b.Get();
    if (pa != pb) {
        return true;
    }
    return false;
}

"""
XTRA = {T: DIFF}
V2 = {
    "loc": body("    int i;\n" + SAVED, LI, "current != lbl_805EE118.Get()", True).replace(
        "        if (current", "        CScene *current = scene.Get();\n        if (current"),
    "differ": body("    int i;\n" + SAVED, LI, "Differ_(scene, lbl_805EE118)", True),
}
BODIES[T].update(V2)
