"""DevilDoomModeCtrlUnit_BELOW_HALF's constructor (wave 39): this in r30, moveCamera in r31."""
SCORE = ["__ct__32DevilDoomModeCtrlUnit_BELOW_HALFFP17DevilDoomVarParam"]
A = """        bool moveCamera = false;
        fn_804165D8(m_owner->m_motion, 0, -1, -1);
"""
B = """            fn_80416F28(m_owner->m_motion, 0, 7, 0);
            moveCamera = true;
        }
        m_owner->m_stopped = 1;
        m_owner->m_fixed = 1;
        m_owner->m_eyeClosing = 0;
        m_owner->m_neckUp = 0;
        if (moveCamera) {
"""
def v(decl, test, true="true"):
    return [(A, A.replace("bool moveCamera = false;", decl)), (B, B.replace("if (moveCamera) {", "if (%s) {" % test).replace("moveCamera = true;", "moveCamera = %s;" % true))]
REPL = {
 "int_u8": v("int moveCamera = 0;", "(u8)moveCamera", "1"),
 "int_bool": v("int moveCamera = false;", "(bool)moveCamera"),
 "int_and": v("int moveCamera = 0;", "moveCamera & 0xFF", "1"),
 "u32_u8": v("u32 moveCamera = 0;", "(u8)moveCamera", "1"),
 "u8": v("u8 moveCamera = 0;", "moveCamera", "1"),
 "s8": v("s8 moveCamera = 0;", "moveCamera", "1"),
 "char": v("char moveCamera = 0;", "moveCamera", "1"),
 "bool_eq": v("bool moveCamera = false;", "moveCamera == true"),
 "bool_ne": v("bool moveCamera = false;", "moveCamera != false"),
 "reg": v("register bool moveCamera = false;", "moveCamera"),
}
REPL = {
 "s8_u8": v("s8 moveCamera = 0;", "(u8)moveCamera", "1"),
 "s8_bool": v("s8 moveCamera = 0;", "(bool)moveCamera", "1"),
 "s8_and": v("s8 moveCamera = 0;", "moveCamera & 0xFF", "1"),
 "s8_true": v("s8 moveCamera = false;", "(u8)moveCamera", "true"),
 "s16_u8": v("s16 moveCamera = 0;", "(u8)moveCamera", "1"),
 "enum": v("enum { NO, YES } moveCamera = NO;", "(u8)moveCamera", "YES"),
}
REPL = {
 "p1": [(A, A.replace("bool moveCamera = false;", "bool moveCamera;\n        moveCamera = false;"))],
 "p2": [(A, A.replace("bool moveCamera = false;", "bool moveCamera(false);"))],
 "p3": [(A, A.replace("bool moveCamera = false;", "DevilDoomModeCtrlUnit_BELOW_HALF *self = this;\n        bool moveCamera = false;"))],
 "p4": [(A, A.replace("bool moveCamera = false;", "bool moveCamera = false;\n        DevilDoomVarParam *unused;"))],
 "p5": [(A, A.replace("bool moveCamera = false;", "u8 moveCamera = false;\n        u8 other = false;")), (B, B.replace("moveCamera = true;", "moveCamera = true;\n            other = moveCamera;"))],
 "p6": [(A, A.replace("bool moveCamera = false;", "volatile int dummy;\n        bool moveCamera = false;"))],
 "p7": [("    DevilDoomModeCtrlUnit_BELOW_HALF(DevilDoomVarParam *owner) : BossCtrlUnit(owner), DevilDoomCtrlUnit(owner) {\n        bool moveCamera = false;", "    DevilDoomModeCtrlUnit_BELOW_HALF(DevilDoomVarParam *owner) : BossCtrlUnit(owner), DevilDoomCtrlUnit(owner), m_side(0) {\n        bool moveCamera = false;")],
}
