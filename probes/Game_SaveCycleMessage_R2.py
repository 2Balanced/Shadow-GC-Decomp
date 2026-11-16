"""SaveCycleMessage's DrawWindow (wave 39, second pass): a const this."""
SCORE = [("DrawWindow__23UnkSaveMessage_802E5B74Fv", "DrawWindow__23UnkSaveMessage_802E5B74CFv")]
REPL = {"const": [("    void DrawWindow();", "    void DrawWindow() const;"), ("void UnkSaveMessage_802E5B74::DrawWindow() {", "void UnkSaveMessage_802E5B74::DrawWindow() const {")]}
CN = REPL["const"]
O = """    s32 top = 0x46;
    s32 height = 0x14A;
    RwRect window = {left + 5, top + 5, width - 10, height - 10};
    fn_800396D0(&window, &s_windowColor, 0);
    RwRect frame[4] = {
        {left, top, 4, height},
        {left + width - 4, top, 4, height},
        {left + 4, top, width - 8, 4},
        {left + 4, top + height - 4, width - 8, 4},
    };
"""
W = "    RwRect window = {left + 5, top + 5, width - 10, height - 10};\n    fn_800396D0(&window, &s_windowColor, 0);\n"
F = "    RwRect frame[4] = {\n        {left, top, 4, height},\n        {right, top, 4, height},\n        {innerLeft, top, innerWidth, 4},\n        {innerLeft, top + height - 4, innerWidth, 4},\n    };\n"
import itertools
names = {"iw": "    s32 innerWidth = width - 8;\n", "il": "    s32 innerLeft = left + 4;\n", "t": "    s32 top = 0x46;\n", "h": "    s32 height = 0x14A;\n", "r": "    s32 right = left + width - 4;\n"}
REPL = {}
# top and height must precede the window; the three others may be declared before it or after it
for order in (("iw", "il", "t", "h", "r"), ("t", "h", "iw", "il", "r"), ("t", "h", "r", "il", "iw"), ("r", "h", "t", "il", "iw"), ("iw", "il", "t", "h"), ("r", "t", "h"), ("il", "t", "h"), ("iw", "t", "h")):
    pre = "".join(names[n] for n in order)
    post = "".join(names[n] for n in ("r", "il", "iw") if n not in order)
    REPL["o_" + "_".join(order)] = CN + [(O, pre + W + post + F)]
    post2 = "".join(names[n] for n in ("iw", "il", "r") if n not in order)
    if post2 != post:
        REPL["o_" + "_".join(order) + "_b"] = CN + [(O, pre + W + post2 + F)]
