"""Where GC/2.7 emits UnkWords_8007A4D4's constructor in Game/PlayerBase.cpp (wave 39): the
constructor declared in the class and defined inline at different places of the unit."""
SCORE = ["__ct__17UnkWords_8007A4D4Fv"]
H = "Game/UnkWords_8007A4D4.h"
OLD = """    UnkWords_8007A4D4() {
        for (int i = 0; i < 3; i++) {
            Word(i) = 0;
        }
    }"""
DEF = """inline UnkWords_8007A4D4::UnkWords_8007A4D4() {
    for (int i = 0; i < 3; i++) {
        Word(i) = 0;
    }
}
"""
DECL = {H: [(OLD, "    UnkWords_8007A4D4();")]}
HDR = {k: DECL for k in ["after_includes", "before_vf0C", "after_vf0C", "after_ctor", "eof"]}
REPL = {
    "after_includes": [("using boost::shared_ptr;", DEF + "using boost::shared_ptr;")],
    "before_vf0C": [("/* CharInfo's slot 0x0C, inline in the game", DEF + "/* CharInfo's slot 0x0C, inline in the game")],
    "after_vf0C": [("namespace Player {\n\nPlayerBase::PlayerBase(int type", DEF + "namespace Player {\n\nPlayerBase::PlayerBase(int type")],
    "after_ctor": [("/* The game's character file manager, made on first use", DEF + "/* The game's character file manager, made on first use")],
    "eof": [("void fn_80078B54(PlayerBase *player) {", DEF + "void fn_80078B54(PlayerBase *player) {")],
}
