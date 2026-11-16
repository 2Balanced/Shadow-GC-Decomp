"""Round 2 for fn_800AA0D4 of Game/PlayerMotion.cpp (wave 39): the inner switch in an inline
function, or on a reused variable."""
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
HELP = """inline bool IsStep_(s32 kind) { switch (kind) { case 4: case 5: case 6: return true; } return false; }
template <class E> inline void Step_(E effect, s32 kind) { switch (kind) { case 4: case 5: case 6: fn_800A29C0(effect); break; } }
"""
REPL = {
  "is_step": [(F, HELP + F), (OLD, "            if (IsStep_(static_cast<s32 *>(data)[0])) {\n                fn_800A29C0(Fields(motion->m_player.get())->m_effect);\n            }\n            break;\n")],
  "step_fn": [(F, HELP + F), (OLD, "            Step_(Fields(motion->m_player.get())->m_effect, static_cast<s32 *>(data)[0]);\n            break;\n")],
  "reuse_var": [("    u16 event;\n    void *data;\n", "    u16 event;\n    void *data;\n    s32 kind;\n"), ("        switch (event) {\n        case 0x5000:\n", "        kind = event;\n        switch (kind) {\n        case 0x5000:\n"), (OLD, OLD.replace("            switch (static_cast<s32 *>(data)[0]) {", "            kind = static_cast<s32 *>(data)[0];\n            switch (kind) {"))],
  "kind_top": [("    u16 event;\n    void *data;\n", "    u16 event;\n    void *data;\n    s32 kind;\n"), (OLD, OLD.replace("            switch (static_cast<s32 *>(data)[0]) {", "            kind = static_cast<s32 *>(data)[0];\n            switch (kind) {"))],
  "kind_top_first": [("    _stMtnPlayWork *work = motion->m_work;\n", "    s32 kind;\n    _stMtnPlayWork *work = motion->m_work;\n"), (OLD, OLD.replace("            switch (static_cast<s32 *>(data)[0]) {", "            kind = static_cast<s32 *>(data)[0];\n            switch (kind) {"))],
}
