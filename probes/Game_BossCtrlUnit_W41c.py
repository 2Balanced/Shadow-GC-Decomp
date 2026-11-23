"""Wave 41: BossCtrlUnit_StageObject::Tick with the line tests' distance check in an inline helper
(its parameters are coloured first) or the line test itself in one."""
SCORE = ["Tick__24BossCtrlUnit_StageObjectFf"]
SRC = open("src/Game/BossCtrlUnit.cpp", encoding="utf-8").read()
s = SRC.index("int BossCtrlUnit_StageObject::Tick(float dt) {")
e = SRC.index("    return false;\n}\n", s) + len("    return false;\n}\n")
FN = SRC[s:e]
HIT1 = """    UnkLineHits_80021FE4 *hits = fn_80021FE4(world, &line, 1, 6, 0, 0);
    if (hits != 0) {
        Vec3f hit;
        fn_800091EC(&hit, fn_8001E9BC(HitObject(hits)));
        if (fn_8040CF50(&Ref(((VectorSubtractFn)fn_8040CD74)(&hit, &m_position))) < m_radius) {
            hidden = true;
        }
        fn_80026FB4(hits, 1);
    } else {
        UnkLine_80020A20 back;
        fn_80020A20(&back, &target, &camera);
        UnkLineHits_80021FE4 *backHits = fn_80021FE4(world, &back, 1, 6, 0, 0);
        if (backHits != 0) {
            Vec3f hit;
            fn_800091EC(&hit, fn_8001E9BC(HitObject(backHits)));
            if (fn_8040CF50(&Ref(((VectorSubtractFn)fn_8040CD74)(&hit, &m_position))) < m_radius) {
                hidden = true;
            }
            fn_80026FB4(backHits, 1);
        }
    }
"""
assert HIT1 in FN
HELP = """inline bool HitNear_(BossCtrlUnit_StageObject *self, UnkLineHits_80021FE4 *hits) {
    Vec3f hit;
    fn_800091EC(&hit, fn_8001E9BC(fn_8002735C(hits)->m_object));
    Vec3f offset = ((VectorSubtractFn)fn_8040CD74)(&hit, &self->m_position);
    return fn_8040CF50(&offset) < self->m_radius;
}

"""
HIT2 = """    UnkLineHits_80021FE4 *hits = fn_80021FE4(world, &line, 1, 6, 0, 0);
    if (hits != 0) {
        if (HitNear_(this, hits)) {
            hidden = true;
        }
        fn_80026FB4(hits, 1);
    } else {
        UnkLine_80020A20 back;
        fn_80020A20(&back, &target, &camera);
        UnkLineHits_80021FE4 *backHits = fn_80021FE4(world, &back, 1, 6, 0, 0);
        if (backHits != 0) {
            if (HitNear_(this, backHits)) {
                hidden = true;
            }
            fn_80026FB4(backHits, 1);
        }
    }
"""
HELP2 = """inline void ReleaseHits_(UnkLineHits_80021FE4 *hits) {
    fn_80026FB4(hits, 1);
}

"""
HIT3 = HIT1.replace("        fn_80026FB4(hits, 1);\n", "        ReleaseHits_(hits);\n").replace("            fn_80026FB4(backHits, 1);\n", "            ReleaseHits_(backHits);\n")
REPL = {
  "near_helper": [(FN, HELP + FN.replace(HIT1, HIT2))],
  "release_helper": [(FN, HELP2 + FN.replace(HIT1, HIT3))],
}
