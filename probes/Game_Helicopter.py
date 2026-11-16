"""Helicopter (wave 39, second pass): UpdateTilt's stfs before frsp, CatchPlayer's r3 before r4."""
SCORE = ["UpdateTilt__10HelicopterFf", "CatchPlayer__10HelicopterFR15ColliDetectArgsb"]
D = "double fn_8040F8FC(const Vec3f *a, const Vec3f *b);"
F = (D, "float fn_8040F8FC(const Vec3f *a, const Vec3f *b);")
C = "            if (!fn_8000A950(angle)) {\n"
INL = ("void Helicopter::UpdateTilt(float dt) {", "inline bool IsTiny(double v) {\n    return fn_8000A950(v);\n}\nvoid Helicopter::UpdateTilt(float dt) {")
REPL = {
 "base": [],
 "f_only": [F],
 "f_dbl": [F, (C, "            if (!fn_8000A950((double)angle)) {\n")],
 "f_fdbl": [F, (C, "            if (!fn_8000A950((float)(double)angle)) {\n")],
 "f_inl": [F, INL, (C, "            if (!IsTiny(angle)) {\n")],
 "d_inl": [INL, (C, "            if (!IsTiny(angle)) {\n")],
 "d_assign": [("            float angle = fn_8040F8FC(&forward, &horizontal);\n" + C, "            float angle;\n            if (!fn_8000A950(angle = fn_8040F8FC(&forward, &horizontal))) {\n")],
 "d_dbl2": [("            float angle = fn_8040F8FC(&forward, &horizontal);\n" + C, "            double a = fn_8040F8FC(&forward, &horizontal);\n            float angle = a;\n            if (!fn_8000A950(a)) {\n")],
}
SCORE = ["CatchPlayer__10HelicopterFR15ColliDetectArgsb"]
L1 = "                const UnkSegment_80021DE4 &line = fn_80021DE4("
L2 = "                            &(const Vec3f &)fn_8040FC3C(line, &m_chara->m_position));\n"
ST = "struct UnkSegment_80021DE4 {\n    u8 m_data[0x18];\n};\n"
REPL = {
 "value": [(L1, "                UnkSegment_80021DE4 line = fn_80021DE4(")],
 "cvalue": [(L1, "                const UnkSegment_80021DE4 line = fn_80021DE4(")],
 "ptr": [(L1, "                const UnkSegment_80021DE4 *line = &(const UnkSegment_80021DE4 &)fn_80021DE4("), (L2, L2.replace("(line,", "(*line,"))],
 "member": [(ST, "struct UnkSegment_80021DE4;\nextern \"C\" Vec3f fn_8040FC3C(const UnkSegment_80021DE4 &segment, const Vec3f *point);\nstruct UnkSegment_80021DE4 {\n    Vec3f Closest(const Vec3f *point) const {\n        return fn_8040FC3C(*this, point);\n    }\n    u8 m_data[0x18];\n};\n"),
            (L2, "                            &(const Vec3f &)line.Closest(&m_chara->m_position));\n")],
 "member_val": [(ST, "struct UnkSegment_80021DE4;\nextern \"C\" Vec3f fn_8040FC3C(const UnkSegment_80021DE4 &segment, const Vec3f *point);\nstruct UnkSegment_80021DE4 {\n    Vec3f Closest(const Vec3f *point) const {\n        return fn_8040FC3C(*this, point);\n    }\n    u8 m_data[0x18];\n};\n"),
            (L2, "                            &(const Vec3f &)line.Closest(&m_chara->m_position));\n"), (L1, "                UnkSegment_80021DE4 line = fn_80021DE4(")],
 "pos_first": [(L2, "                            &(const Vec3f &)fn_8040FC3C(line, position));\n"), ("                fn_800091D0(&m_hangPosition,\n", "                const Vec3f *position = &m_chara->m_position;\n                fn_800091D0(&m_hangPosition,\n")],
}
REPL = {}
V = (L1, "                UnkSegment_80021DE4 line = fn_80021DE4(")
V2 = (L1, "                UnkSegment_80021DE4 line(fn_80021DE4(")
REPL["val_ctor"] = [V, (ST, "struct UnkSegment_80021DE4 {\n    UnkSegment_80021DE4() {}\n    u8 m_data[0x18];\n};\n")]
REPL["val_cctor"] = [V, (ST, "struct UnkSegment_80021DE4 {\n    UnkSegment_80021DE4() {}\n    UnkSegment_80021DE4(const UnkSegment_80021DE4 &other);\n    u8 m_data[0x18];\n};\n")]
REPL["val_dtor"] = [V, (ST, "struct UnkSegment_80021DE4 {\n    ~UnkSegment_80021DE4() {}\n    u8 m_data[0x18];\n};\n")]
REPL["ref_cctor"] = [(ST, "struct UnkSegment_80021DE4 {\n    UnkSegment_80021DE4() {}\n    UnkSegment_80021DE4(const UnkSegment_80021DE4 &other);\n    u8 m_data[0x18];\n};\n")]
REPL = {}
OLD = """                const UnkSegment_80021DE4 &line = fn_80021DE4((const UnkSegment_80020A20 *)fn_80020A20(
                    &UnkSegment_80020A20(), &(const Vec3f &)fn_8040B67C(&matrix, skid->Start()),
                    &(const Vec3f &)fn_8040B67C(&matrix, skid->End())));
                fn_800091D0(&m_hangPosition,
                            &(const Vec3f &)fn_8040FC3C(line, &m_chara->m_position));
"""
REPL["nested"] = [(OLD, """                fn_800091D0(&m_hangPosition,
                            &(const Vec3f &)fn_8040FC3C(fn_80021DE4((const UnkSegment_80020A20 *)fn_80020A20(
                    &UnkSegment_80020A20(), &(const Vec3f &)fn_8040B67C(&matrix, skid->Start()),
                    &(const Vec3f &)fn_8040B67C(&matrix, skid->End()))), &m_chara->m_position));
""")]
REPL["nested2"] = [(OLD, """                const UnkSegment_80020A20 *segment = (const UnkSegment_80020A20 *)fn_80020A20(
                    &UnkSegment_80020A20(), &(const Vec3f &)fn_8040B67C(&matrix, skid->Start()),
                    &(const Vec3f &)fn_8040B67C(&matrix, skid->End()));
                fn_800091D0(&m_hangPosition,
                            &(const Vec3f &)fn_8040FC3C(fn_80021DE4(segment), &m_chara->m_position));
""")]
REPL = {}
STN = "struct UnkDebugLine_80020A20 {\n    UnkDebugLine_80020A20() {}\n    UnkDebugLine_80020A20(const UnkDebugLine_80020A20 &line);\n    u8 m_data[0x18];\n};\n"
DEC = ("Vec3f fn_8040FC3C(const UnkSegment_80021DE4 &segment, const Vec3f *point);\nUnkSegment_80021DE4 fn_80021DE4(const UnkSegment_80020A20 *segment);\n",
       "Vec3f fn_8040FC3C(%s, const Vec3f *point);\n")
MK = """(const UnkDebugLine_80020A20 *)fn_80020A20(
                    &UnkSegment_80020A20(), &(const Vec3f &)fn_8040B67C(&matrix, skid->Start()),
                    &(const Vec3f &)fn_8040B67C(&matrix, skid->End()))"""
def v(param, body):
    return [(ST, ST + STN), (DEC[0], DEC[1] % param), (OLD, body)]
REPL["byval_nested"] = v("UnkDebugLine_80020A20 segment", """                fn_800091D0(&m_hangPosition,
                            &(const Vec3f &)fn_8040FC3C(*%s, &m_chara->m_position));
""" % MK)
REPL["ref_local"] = v("const UnkDebugLine_80020A20 &segment", """                UnkDebugLine_80020A20 line(*%s);
                fn_800091D0(&m_hangPosition,
                            &(const Vec3f &)fn_8040FC3C(line, &m_chara->m_position));
""" % MK)
REPL["ref_local_eq"] = v("const UnkDebugLine_80020A20 &segment", """                UnkDebugLine_80020A20 line = *%s;
                fn_800091D0(&m_hangPosition,
                            &(const Vec3f &)fn_8040FC3C(line, &m_chara->m_position));
""" % MK)
REPL["ptr_local"] = v("const UnkDebugLine_80020A20 *segment", """                UnkDebugLine_80020A20 line(*%s);
                fn_800091D0(&m_hangPosition,
                            &(const Vec3f &)fn_8040FC3C(&line, &m_chara->m_position));
""" % MK)
REPL["ref_temp"] = v("const UnkDebugLine_80020A20 &segment", """                fn_800091D0(&m_hangPosition,
                            &(const Vec3f &)fn_8040FC3C(UnkDebugLine_80020A20(*%s), &m_chara->m_position));
""" % MK)
