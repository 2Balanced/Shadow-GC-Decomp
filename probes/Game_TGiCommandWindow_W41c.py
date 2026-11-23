"""Wave 41: UpdateWindow's last two rows (retail keeps next's car icon in r26 for the test against 16)."""
SRC = open("src/Game/TGiCommandWindow.cpp", encoding="utf-8").read()
SCORE = ["UpdateWindow__20TGiCommandWindowImplFPQ34Chao3CSD8CProjectP22UnkPlayerInfo_80178B5CP15TGiCommandState"]
s = SRC.index("void TGiCommandWindowImpl::UpdateWindow(")
e = SRC.index("\n}\n", s) + 3
FN = SRC[s:e]
OLD = """        s32 icon;
        if (next.m_car.m_icon != (icon = state->m_car.m_icon) || next.m_car.m_button != state->m_car.m_button) {
            if (next.m_car.m_icon != 16 && icon == 16) {"""
assert OLD in FN
V = {
 "assign2": """        s32 icon;
        s32 carIcon;
        if ((carIcon = next.m_car.m_icon) != (icon = state->m_car.m_icon) || next.m_car.m_button != state->m_car.m_button) {
            if (carIcon != 16 && icon == 16) {""",
 "assign2b": """        s32 carIcon;
        s32 icon;
        if ((carIcon = next.m_car.m_icon) != (icon = state->m_car.m_icon) || next.m_car.m_button != state->m_car.m_button) {
            if (carIcon != 16 && icon == 16) {""",
}
REPL = {}
for k, v in V.items():
    REPL[k] = [(FN, FN.replace(OLD, v))]
    REPL[k + "_call"] = [(FN, FN.replace(OLD, v).replace("                fn_8036AD14(node.Get(), next.m_car.m_icon);", "                fn_8036AD14(node.Get(), carIcon);"))]
