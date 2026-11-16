"""Round 3 for fn_800AA0D4 of Game/PlayerMotion.cpp (wave 39): the inner switch as an inline
function of the motion, so that the word is an argument (r4)."""
SCORE = ["fn_800AA0D4"]
OLD = """            switch (static_cast<s32 *>(data)[0]) {
            case 4:
            case 5:
            case 6:
                fn_800A29C0(Fields(motion->m_player.get())->m_effect);
                break;
            }
            break;
"""
F = "void fn_800AA0D4(Player::PlayerMotion *motion) {"
HELP = """inline void StepA_(Player::PlayerMotion *motion, s32 kind) {
    switch (kind) { case 4: case 5: case 6: fn_800A29C0(Fields(*(Player::PlayerBase **)&motion->m_player)->m_effect); break; }
}
inline void StepB_(UnkPlayerFields_800AA0D4 *fields, s32 kind) {
    switch (kind) { case 4: case 5: case 6: fn_800A29C0(fields->m_effect); break; }
}
inline void StepC_(s32 kind, Player::PlayerMotion *motion) {
    switch (kind) { case 4: case 5: case 6: fn_800A29C0(Fields(*(Player::PlayerBase **)&motion->m_player)->m_effect); break; }
}
"""
REPL = {
  "stepA": [(F, HELP + F), (OLD, "            StepA_(motion, static_cast<s32 *>(data)[0]);\n            break;\n")],
  "stepB": [(F, HELP + F), (OLD, "            StepB_(Fields(motion->m_player.get()), static_cast<s32 *>(data)[0]);\n            break;\n")],
  "stepC": [(F, HELP + F), (OLD, "            StepC_(static_cast<s32 *>(data)[0], motion);\n            break;\n")],
}
