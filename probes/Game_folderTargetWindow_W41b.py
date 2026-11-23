"""Wave 41: folderTargetWindow::Update: the player check's registers (retail: manager's word r0,
mission's r3) and the busy loop (retail: i copied from the zeroed sum)."""
SCORE = ["Update__18folderTargetWindowFv"]
H = "bool folderTargetWindow::Update() {\n"
P0 = "#pragma opt_propagation off\n"
C0 = """                    UnkPlayerManager_8006D6A8 *manager = fn_8006D6A8();
                    if (manager->m_pausePlayer == mission->m_player) {
"""
C2 = """                    UnkPlayerManager_8006D6A8 *manager = fn_8006D6A8();
                    if (manager->m_pausePlayer != mission->m_player) {
"""
L = """    int busy = 0;
    for (i = 0; i < 2; i++) {
        busy += m_panels[i].m_scene.Update(false);
    }
    return busy != 0;
"""
INL = """inline int UpdatePanels_(folderTargetWindow *self) {
    int busy = 0;
    for (int i = 0; i < 2; i++) {
        busy += self->m_panels[i].m_scene.Update(false);
    }
    return busy;
}
"""
LI = "    return UpdatePanels_(this) != 0;\n"
def cmpv(fmt):
    a = fmt.replace("OP", "==")
    b = fmt.replace("OP", "!=")
    return [(C0, a), (C2, b)]
MPL = """                    UnkPlayerManager_8006D6A8 *manager = fn_8006D6A8();
                    s32 player = mission->m_player;
                    if (manager->m_pausePlayer OP player) {
"""
PPL = """                    UnkPlayerManager_8006D6A8 *manager = fn_8006D6A8();
                    s32 pause = manager->m_pausePlayer;
                    if (pause OP mission->m_player) {
"""
RPL = """                    UnkPlayerManager_8006D6A8 *manager = fn_8006D6A8();
                    if (mission->m_player OP manager->m_pausePlayer) {
"""
REPL = {
  "inl_loop": [(L, LI), (H, INL + H)],
  "mpl": cmpv(MPL),
  "ppl": cmpv(PPL),
  "rpl": cmpv(RPL),
  "prag_mpl": [(H, P0 + H)] + cmpv(MPL),
  "prag_ppl": [(H, P0 + H)] + cmpv(PPL),
  "prag": [(H, P0 + H)],
  "inl_loop_mpl": [(L, LI), (H, INL + H)] + cmpv(MPL),
  "inl_loop_ppl": [(L, LI), (H, INL + H)] + cmpv(PPL),
  "inl_loop_prag_ppl": [(L, LI), (H, INL + P0 + H)] + cmpv(PPL),
  "inl_loop_prag_mpl": [(L, LI), (H, INL + P0 + H)] + cmpv(MPL),
}
