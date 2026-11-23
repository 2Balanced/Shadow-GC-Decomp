"""Wave 40: fn_8009339C (Game/PlayerUtil.cpp). Retail copies the fields pointer (r31) to r4 before
the dot product and before the slerp, and reuses r31 for the tilt's address: a const local copy is
the one kind of copy GC/2.7 does not merge back (seen in Game/UnkMotion_8022F058.cpp)."""
F = "fn_8009339C"
SYMBOLS = {F: 'extern "C" void fn_8009339C(boost::shared_ptr<PlayerBase> *player, const Vec3f *up) {'}
A = """    fn_800091D0(&Fields(player->px)->m_groundUp, up);
    UnkPlayerFields_800930AC *fields = Fields(player->px);
    fn_8007A34C(&fields->field_0x1D4, &(const Quatf &)fn_800934FC(&fields->m_groundUp));
    Fields(player->operator->())->field_0x1E4 =
        fn_8040B228(&Fields(player->operator->())->field_0x1D4);
"""
RATE = """    float rate = 15.0f * (lbl_805FAD38 / 180.0f) /
                 fn_80015D68(fn_8040CF90(&%s->field_0x17C, &%s->m_groundUp));
    rate = fn_800704AC(rate, s_maxTilt);
"""
SL = """    fn_8007A34C(%s,
                &(const Quatf &)fn_8040C3EC(%s, &%s->field_0x1D4, rate));
"""
Z = """    Fields(player->operator->())->field_0x198 =
        fn_8040B228(&Fields(player->operator->())->field_0x188);
    fn_800091D0(&Fields(player->px)->field_0x17C,
                &(const Vec3f &)fn_8040B67C(&Fields(player->px)->field_0x198,
                                            &(const Vec3f &)Vec3f(0.0f, 1.0f, 0.0f)));
    fn_8040C958(&Fields(player->px)->field_0x188);
    fn_8040C958(&Fields(player->px)->field_0x1D4);
"""
T = "UnkPlayerFields_800930AC"
CF = "    %s *const f = fields;\n" % T
BODIES = {F: {
    "base": A + RATE % ("fields", "fields") + SL % ("&fields->field_0x188", "&fields->field_0x188", "fields") + Z,
    "c1": A + CF + RATE % ("f", "f") + SL % ("&f->field_0x188", "&f->field_0x188", "f") + Z,
    "c2": A + "    {\n" + CF + RATE.replace("    float rate", "    float rate;\n    rate") % ("f", "f") + "    }\n",
    "c_blocks": A + "    float rate;\n    {\n    " + CF + "        rate = 15.0f * (lbl_805FAD38 / 180.0f) / fn_80015D68(fn_8040CF90(&f->field_0x17C, &f->m_groundUp));\n    }\n    rate = fn_800704AC(rate, s_maxTilt);\n    {\n    " + CF + "        Quatf *tilt = &f->field_0x188;\n        fn_8007A34C(tilt, &(const Quatf &)fn_8040C3EC(tilt, &f->field_0x1D4, rate));\n    }\n" + Z,
    "c_blocks_ctilt": A + "    float rate;\n    {\n    " + CF + "        rate = 15.0f * (lbl_805FAD38 / 180.0f) / fn_80015D68(fn_8040CF90(&f->field_0x17C, &f->m_groundUp));\n    }\n    rate = fn_800704AC(rate, s_maxTilt);\n    {\n    " + CF + "        Quatf *const tilt = &f->field_0x188;\n        fn_8007A34C(tilt, &(const Quatf &)fn_8040C3EC(tilt, &f->field_0x1D4, rate));\n    }\n" + Z,
    "tilt_only": A + RATE % ("fields", "fields") + "    Quatf *tilt = &fields->field_0x188;\n" + SL % ("tilt", "tilt", "fields") + Z,
    "ctilt_only": A + RATE % ("fields", "fields") + "    Quatf *const tilt = &fields->field_0x188;\n" + SL % ("tilt", "tilt", "fields") + Z,
    "cfields": (A + RATE % ("fields", "fields") + SL % ("&fields->field_0x188", "&fields->field_0x188", "fields") + Z).replace(T + " *fields =", T + " *const fields ="),
    "cfields_ctilt": (A + RATE % ("fields", "fields") + "    Quatf *const tilt = &fields->field_0x188;\n" + SL % ("tilt", "tilt", "fields") + Z).replace(T + " *fields =", T + " *const fields ="),
}}
del BODIES[F]["c2"]
XTRA = {F: """struct UnkQ_Probe : public Quatf {
    UnkQ_Probe &operator=(const Quatf &q) {
        fn_8007A34C(this, &q);
        return *this;
    }
};
inline float UpDotRef(const UnkPlayerFields_800930AC &f) {
    return fn_8040CF90(&f.field_0x17C, &f.m_groundUp);
}
inline void TiltRef(UnkPlayerFields_800930AC &f, float rate) {
    fn_8007A34C(&f.field_0x188, &(const Quatf &)fn_8040C3EC(&f.field_0x188, &f.field_0x1D4, rate));
}
inline void TiltRefLocal(UnkPlayerFields_800930AC &f, float rate) {
    Quatf *tilt = &f.field_0x188;
    fn_8007A34C(tilt, &(const Quatf &)fn_8040C3EC(tilt, &f.field_0x1D4, rate));
}
inline void TiltPtrLocal(UnkPlayerFields_800930AC *f, float rate) {
    Quatf *tilt = &f->field_0x188;
    fn_8007A34C(tilt, &(const Quatf &)fn_8040C3EC(tilt, &f->field_0x1D4, rate));
}
inline float UpDotPtr(const UnkPlayerFields_800930AC *f) {
    return fn_8040CF90(&f->field_0x17C, &f->m_groundUp);
}
inline void TiltAssign(UnkPlayerFields_800930AC *f, float rate) {
    *static_cast<UnkQ_Probe *>(&f->field_0x188) = fn_8040C3EC(&f->field_0x188, &f->field_0x1D4, rate);
}
"""}
R2 = """    float rate = 15.0f * (lbl_805FAD38 / 180.0f) / fn_80015D68(%s);
    rate = fn_800704AC(rate, s_maxTilt);
"""
B = BODIES[F]
B["ref_both"] = A + R2 % "UpDotRef(*fields)" + "    TiltRef(*fields, rate);\n" + Z
B["ref_local"] = A + R2 % "UpDotRef(*fields)" + "    TiltRefLocal(*fields, rate);\n" + Z
B["ptr_local"] = A + R2 % "UpDotPtr(fields)" + "    TiltPtrLocal(fields, rate);\n" + Z
B["ptr_assign"] = A + R2 % "UpDotPtr(fields)" + "    TiltAssign(fields, rate);\n" + Z
B["assign_direct"] = A + RATE % ("fields", "fields") + "    *static_cast<UnkQ_Probe *>(&fields->field_0x188) = fn_8040C3EC(&fields->field_0x188, &fields->field_0x1D4, rate);\n" + Z
B["dot_only_ref"] = A + R2 % "UpDotRef(*fields)" + SL % ("&fields->field_0x188", "&fields->field_0x188", "fields") + Z
XTRA[F] += """struct UnkDerived_Probe : public UnkPlayerFields_800930AC {};
struct UnkPad_Probe { int pad; };
struct UnkDerived2_Probe : public UnkPad_Probe, public UnkPlayerFields_800930AC {};
"""
AD = A.replace("UnkPlayerFields_800930AC *fields = Fields(player->px);", "UnkDerived_Probe *fields = reinterpret_cast<UnkDerived_Probe *>(player->px);")
B["der_ptr"] = AD + R2 % "UpDotPtr(fields)" + "    TiltPtrLocal(fields, rate);\n" + Z
B["der_ref"] = AD + R2 % "UpDotRef(*fields)" + "    TiltRef(*fields, rate);\n" + Z
B["der_cast"] = AD + RATE % ("static_cast<UnkPlayerFields_800930AC *>(fields)", "static_cast<UnkPlayerFields_800930AC *>(fields)") + SL % ("&fields->field_0x188", "&fields->field_0x188", "fields") + Z
B["void_cast"] = A.replace("UnkPlayerFields_800930AC *fields = Fields(player->px);", "void *raw = player->px;\n#define fields Fields(raw)") + RATE % ("fields", "fields") + SL % ("&fields->field_0x188", "&fields->field_0x188", "fields") + Z + "#undef fields\n"
B["pb_cast"] = A.replace("UnkPlayerFields_800930AC *fields = Fields(player->px);", "PlayerBase *raw = player->px;\n#define fields Fields(raw)") + RATE % ("fields", "fields") + SL % ("&fields->field_0x188", "&fields->field_0x188", "fields") + Z + "#undef fields\n"
B["pb_helpers"] = A.replace("UnkPlayerFields_800930AC *fields = Fields(player->px);", "PlayerBase *raw = player->px;\n#define fields Fields(raw)") + R2 % "UpDotPtr(fields)" + "    TiltPtrLocal(fields, rate);\n" + Z + "#undef fields\n"
def raw(kind, s2, s4, s5):
    a = A.replace("UnkPlayerFields_800930AC *fields = Fields(player->px);", kind + " *raw = player->px;")
    a = a.replace("fields->", s2)
    return a + s4 + s5 + Z
M = "Fields(raw)->"
S4M = RATE % ("Fields(raw)", "Fields(raw)")
S5M = SL % ("&Fields(raw)->field_0x188", "&Fields(raw)->field_0x188", "Fields(raw)")
TF = "        UnkPlayerFields_800930AC *f = Fields(raw);\n"
S4B = "    float rate;\n    {\n" + TF + "        rate = 15.0f * (lbl_805FAD38 / 180.0f) / fn_80015D68(fn_8040CF90(&f->field_0x17C, &f->m_groundUp));\n    }\n    rate = fn_800704AC(rate, s_maxTilt);\n"
S5B = "    {\n" + TF + "        fn_8007A34C(&f->field_0x188, &(const Quatf &)fn_8040C3EC(&f->field_0x188, &f->field_0x1D4, rate));\n    }\n"
S5T = "    {\n" + TF + "        Quatf *tilt = &f->field_0x188;\n        fn_8007A34C(tilt, &(const Quatf &)fn_8040C3EC(tilt, &f->field_0x1D4, rate));\n    }\n"
for k, kind in (("pb", "PlayerBase"), ("v", "void")):
    B[k + "_m_b_b"] = raw(kind, M, S4B, S5B)
    B[k + "_m_b_t"] = raw(kind, M, S4B, S5T)
    B[k + "_m_m_t"] = raw(kind, M, S4M, S5T)
    B[k + "_m_b_m"] = raw(kind, M, S4B, S5M)
