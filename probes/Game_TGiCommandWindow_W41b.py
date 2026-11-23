"""Wave 41: UpdateWindow. The state's icon read into a local before the comparison (the
comparison-operand lever of wave 40), and copy propagation off."""
SRC = open("src/Game/TGiCommandWindow.cpp", encoding="utf-8").read()
SCORE = ["UpdateWindow__20TGiCommandWindowImplFPQ34Chao3CSD8CProjectP22UnkPlayerInfo_80178B5CP15TGiCommandState"]
s = SRC.index("void TGiCommandWindowImpl::UpdateWindow(")
e = SRC.index("\n}\n", s) + 3
FN = SRC[s:e]
W_OLD = """    if (scene.Get() != 0 && scene->IsEnd() && next.m_weapon.m_icon != state->m_weapon.m_icon) {
        if (state->m_weapon.m_icon == 16) {"""
W_NEW = """    if (scene.Get() != 0 && scene->IsEnd()) {
        s32 icon = state->m_weapon.m_icon;
        if (next.m_weapon.m_icon != icon) {
        if (icon == 16) {"""
W_END_OLD = """            next.m_weapon.m_icon = 16;
        }
        state->m_weapon = next.m_weapon;
    }
"""
W_END_NEW = """            next.m_weapon.m_icon = 16;
        }
        state->m_weapon = next.m_weapon;
        }
    }
"""
C_OLD = """        if (next.m_car.m_icon != state->m_car.m_icon || next.m_car.m_button != state->m_car.m_button) {
            if (next.m_car.m_icon != 16 && state->m_car.m_icon == 16) {"""
C_NEW = """        s32 icon = state->m_car.m_icon;
        if (next.m_car.m_icon != icon || next.m_car.m_button != state->m_car.m_button) {
            if (next.m_car.m_icon != 16 && icon == 16) {"""
def sub(t, pairs):
    for a, b in pairs:
        assert a in t, a[:60]
        t = t.replace(a, b, 1)
    return t
V = {}
V["w"] = sub(FN, [(W_OLD, W_NEW), (W_END_OLD, W_END_NEW)])
V["c"] = sub(FN, [(C_OLD, C_NEW)])
V["wc"] = sub(FN, [(W_OLD, W_NEW), (W_END_OLD, W_END_NEW), (C_OLD, C_NEW)])
REPL = {}
for k, v in V.items():
    REPL[k] = [(FN, v)]
    REPL["prag_" + k] = [(FN, "#pragma opt_propagation off\n" + v + "#pragma opt_propagation reset\n")]
REPL["prag_plain"] = [(FN, "#pragma opt_propagation off\n" + FN + "#pragma opt_propagation reset\n")]
W_NEW2 = """    if (scene.Get() != 0 && scene->IsEnd()) {
        s32 icon;
        if (next.m_weapon.m_icon != (icon = state->m_weapon.m_icon)) {
        if (icon == 16) {"""
C_NEW2 = """        s32 icon;
        if (next.m_car.m_icon != (icon = state->m_car.m_icon) || next.m_car.m_button != state->m_car.m_button) {
            if (next.m_car.m_icon != 16 && icon == 16) {"""
C_NEW3 = """        s32 icon;
        s32 carIcon = next.m_car.m_icon;
        if (carIcon != (icon = state->m_car.m_icon) || next.m_car.m_button != state->m_car.m_button) {
            if (carIcon != 16 && icon == 16) {"""
V2 = {}
V2["wc2"] = sub(FN, [(W_OLD, W_NEW2), (W_END_OLD, W_END_NEW), (C_OLD, C_NEW2)])
V2["wc3"] = sub(FN, [(W_OLD, W_NEW2), (W_END_OLD, W_END_NEW), (C_OLD, C_NEW3)])
V2["wc3b"] = sub(V2["wc3"], [("                fn_8036AD14(node.Get(), next.m_car.m_icon);", "                fn_8036AD14(node.Get(), carIcon);")])
for k, v in V2.items():
    REPL[k] = [(FN, v)]
    REPL["prag_" + k] = [(FN, "#pragma opt_propagation off\n" + v + "#pragma opt_propagation reset\n")]
BASE2 = V2["wc2"]
CI = "            if (next.m_car.m_icon != 16 && icon == 16) {"
V3 = {}
V3["d_not"] = BASE2.replace(CI, "            if (!(next.m_car.m_icon == 16) && icon == 16) {")
V3["d_nested"] = BASE2.replace(CI, "            if (next.m_car.m_icon != 16 && (icon == 16)) {")
V3["d_ref"] = BASE2.replace("        s32 icon;\n        if (next.m_car.m_icon != (icon = state->m_car.m_icon) || next.m_car.m_button != state->m_car.m_button) {\n            if (next.m_car.m_icon != 16 && icon == 16) {",
    "        s32 icon;\n        const TGiCommand &car = next.m_car;\n        if (car.m_icon != (icon = state->m_car.m_icon) || car.m_button != state->m_car.m_button) {\n            if (car.m_icon != 16 && icon == 16) {")
V3["d_ifs"] = BASE2.replace("        if (next.m_car.m_icon != (icon = state->m_car.m_icon) || next.m_car.m_button != state->m_car.m_button) {",
    "        if ((icon = state->m_car.m_icon, next.m_car.m_icon != icon) || next.m_car.m_button != state->m_car.m_button) {")
for k, v in V3.items():
    REPL[k] = [(FN, v)]
V4 = {}
V4["f_bool"] = BASE2.replace(CI, "            bool show = next.m_car.m_icon != 16 && icon == 16;\n            if (show) {")
V4["f_inline"] = "inline bool Appears_(s32 next, s32 now) {\n    return next != 16 && now == 16;\n}\n\n" + BASE2.replace(CI, "            if (Appears_(next.m_car.m_icon, icon)) {")
V4["f_inline2"] = "inline bool Appears_(const TGiCommand &next, s32 now) {\n    return next.m_icon != 16 && now == 16;\n}\n\n" + BASE2.replace(CI, "            if (Appears_(next.m_car, icon)) {")
V4["f_neq_first"] = BASE2.replace("        if (next.m_car.m_icon != (icon = state->m_car.m_icon) || next.m_car.m_button != state->m_car.m_button) {",
    "        if (!(next.m_car.m_icon == (icon = state->m_car.m_icon) && next.m_car.m_button == state->m_car.m_button)) {")
for k, v in V4.items():
    REPL[k] = [(FN, v)]
