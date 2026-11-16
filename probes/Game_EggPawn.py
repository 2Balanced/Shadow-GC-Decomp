"""EggPawn (wave 39, second pass)."""
SCORE = ["vfPara_0x60__11EggPawnBaseFv", "vfPara_0x5C__11EggPawnBaseFv", "vfStat_0x20__17EnemyStatusCommonFib"]
A = "int EggPawnBase::vfPara_0x5C() {\n    return m_status->vfStat_0x74() == 0 ? 17 : 18;\n}\n\nint EggPawnBase::vfPara_0x60() {\n    return m_status->vfStat_0x74() == 0 ? 19 : 20;\n}\n"
def b(e, pre=""):
    return [(A, pre + "int EggPawnBase::vfPara_0x5C() {\n    return %s->vfStat_0x74() == 0 ? 17 : 18;\n}\n\nint EggPawnBase::vfPara_0x60() {\n    return %s->vfStat_0x74() == 0 ? 19 : 20;\n}\n" % (e, e))]
REPL = {
 "free_const": b("StatusOf(this)", "inline EnemyStatFunc *StatusOf(const EnemyStatusObjBase *o) {\n    return o->m_status;\n}\n"),
 "free_cref": b("StatusOf(*this)", "inline EnemyStatFunc *StatusOf(const EnemyStatusObjBase &o) {\n    return o.m_status;\n}\n"),
 "cast": b("((const EggPawnBase *)this)->m_status"),
 "cast2": b("static_cast<const EnemyStatusObjBase *>(this)->m_status"),
}
SCORE = [("vfPara_0x60__11EggPawnBaseFv", "vfPara_0x60__11EggPawnBaseCFv"), ("vfPara_0x5C__11EggPawnBaseFv", "vfPara_0x5C__11EggPawnBaseCFv")]
REPL = {"const": [("int EggPawnBase::vfPara_0x5C() {", "int EggPawnBase::vfPara_0x5C() const {"), ("int EggPawnBase::vfPara_0x60() {", "int EggPawnBase::vfPara_0x60() const {")]}
HDR = {"const": {"Game/EggPawn.h": [("    virtual int vfPara_0x5C();", "    virtual int vfPara_0x5C() const;"), ("    virtual int vfPara_0x60();", "    virtual int vfPara_0x60() const;")],
                 "Game/Enemy.h": [("    virtual int vfStat_0x74();", "    virtual int vfStat_0x74() const;")]}}
del HDR["const"]["Game/Enemy.h"]
SCORE = ["vfStat_0x20__17EnemyStatusCommonFib"]
HDR = {}
OLDF = """inline int EnemyStatusCommon::vfStat_0x20(int motion, bool restart) {
    field_0x70 = motion;
    if (motion < 0) {
        m_motion.Reset();
    } else {
        m_motion.field_0x4 = 0;
        if (restart) {
            m_motion.Start(motion);
        } else if (motion != m_motion.m_index) {
            m_motion.Start(motion);
        } else {
            return 1;
        }
    }
}
"""
def st(ret, fail):
    return """inline %s StartSequence(MotionSequenceModule *module, int index) {
    if (module->m_table != 0 && index < module->m_table->count) {
        module->m_index = index;
        return fn_800C961C(module, &module->m_table->sequences[index]);
    }
    return %s;
}
""" % (ret, fail)
BODY = """inline int EnemyStatusCommon::vfStat_0x20(int motion, bool restart) {
    field_0x70 = motion;
    if (motion < 0) {
        m_motion.Reset();
    } else {
        m_motion.field_0x4 = 0;
        if (restart) {
            return StartSequence(&m_motion, motion);
        } else if (motion != m_motion.m_index) {
            return StartSequence(&m_motion, motion);
        } else {
            return 1;
        }
    }
}
"""
REPL = {
 "ret_int0": [(OLDF, st("int", "0") + BODY)],
 "ret_bool": [(OLDF, st("bool", "false") + BODY)],
 "ret_local": [(OLDF, st("int", "0") + BODY.replace("return StartSequence(&m_motion, motion);", "int r = StartSequence(&m_motion, motion);\n            return r;"))],
}
H = {"Game/Enemy.h": [("    virtual int vfStat_0x20(int motion, bool restart) {", "    virtual bool vfStat_0x20(int motion, bool restart) {"),
                      ("    virtual int vfStat_0x20(int motion, bool restart); /* 0x8017B61C */", "    virtual bool vfStat_0x20(int motion, bool restart); /* 0x8017B61C */")]}
BB = BODY.replace("inline int EnemyStatusCommon", "inline bool EnemyStatusCommon")
REPL = {
 "b_ret": [(OLDF, st("bool", "false") + BB.replace("return 1;", "return true;"))],
 "b_var": [(OLDF, st("bool", "false") + """inline bool EnemyStatusCommon::vfStat_0x20(int motion, bool restart) {
    field_0x70 = motion;
    if (motion < 0) {
        m_motion.Reset();
    } else {
        bool result = false;
        m_motion.field_0x4 = 0;
        if (restart) {
            result = StartSequence(&m_motion, motion);
        } else if (motion != m_motion.m_index) {
            result = StartSequence(&m_motion, motion);
        } else {
            result = true;
        }
        return result;
    }
}
""")],
 "b_var2": [(OLDF, """inline bool StartSequence(MotionSequenceModule *module, int index) {
    bool result = false;
    if (module->m_table != 0 && index < module->m_table->count) {
        module->m_index = index;
        result = fn_800C961C(module, &module->m_table->sequences[index]);
    }
    return result;
}
""" + BB.replace("return 1;", "return true;"))],
}
HDR = {k: H for k in REPL}
SCORE = ["vfSet_0x1C__11EggPawnBaseFP21UnkPlacement_800C9CD0"]
HDR = {}
O = """    Vec3fWords rotation = *reinterpret_cast<Vec3fWords *>(GetRotation());
    vfDisp_0x38(GetPosition(), reinterpret_cast<Vec3f *>(&rotation));
"""
REPL = {
 "temp": [(O, "    vfDisp_0x38(GetPosition(), reinterpret_cast<Vec3f *>(&(const Vec3fWords &)Vec3fWords(*reinterpret_cast<Vec3fWords *>(GetRotation()))));\n")],
 "dead": [(O, "    if (false) {\n        GetPosition();\n    }\n" + O)],
 "addr": [(O, "    (void)&Enemy::GetPosition;\n" + O)],
 "sizeof": [(O, "    (void)sizeof(GetPosition());\n" + O)],
}
REPL = {
 "temp": [(O, "    vfDisp_0x38(GetPosition(), reinterpret_cast<const Vec3f *>(&(const Vec3fWords &)Vec3fWords(*reinterpret_cast<Vec3fWords *>(GetRotation()))));\n")],
 "temp2": [(O, "    vfDisp_0x38(GetPosition(), reinterpret_cast<const Vec3f *>(&(const Vec3fWords &)*reinterpret_cast<Vec3fWords *>(GetRotation())));\n")],
 "wrap": [(O, "    SetTransform(this, GetPosition(), *reinterpret_cast<Vec3fWords *>(GetRotation()));\n"),
          ("void EggPawnBase::vfSet_0x1C(", "inline void SetTransform(EnemyDispFunc *disp, const Vec3f *position, Vec3fWords rotation) {\n    disp->vfDisp_0x38(position, reinterpret_cast<Vec3f *>(&rotation));\n}\nvoid EggPawnBase::vfSet_0x1C(")],
}
