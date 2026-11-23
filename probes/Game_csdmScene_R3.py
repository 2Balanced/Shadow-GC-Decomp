"""Wave 40 (resumed agent): IsParentVisible: a layer local, the state declared first, and copy propagation off (the winner: swap + opt_propagation off).
Written as a JOBS list (unit, name, [(old, new)], [symbols]) against the source of the time;
the REPL/SCORE lines at the end give tools/pprobe.py the same variants."""
F = "s32 CScene::IsParentVisible() {"
OLD = "        if (state->m_layers[i].m_casts[info->m_layers[i].m_root].m_matrix.m_scale != 0) {"
NEW = "        SLayer *layer = &state->m_layers[i];\n        if (layer->m_casts[info->m_layers[i].m_root].m_matrix.m_scale != 0) {"
D = "    SSceneInfo *info = m_resource;\n    SSceneState *state = m_palette->m_resource;\n"
DS = "    SSceneState *state = m_palette->m_resource;\n    SSceneInfo *info = m_resource;\n"
DS2 = "    SSceneState *state;\n    SSceneInfo *info;\n    info = m_resource;\n    state = m_palette->m_resource;\n"
DS3 = "    SSceneInfo *info;\n    SSceneState *state;\n    state = m_palette->m_resource;\n    info = m_resource;\n"
JOBS = []
for dn, dd in (("swap", DS), ("split_sw", DS2), ("split_as", DS3), ("orig", D)):
    for p in ("opt_propagation off", "opt_dead_assignments off", None):
        rep = [(OLD, NEW), (D, dd)] if dd != D else [(OLD, NEW)]
        if p:
            rep.append((F, "#pragma " + p + "\n" + F))
        JOBS.append(("csdmScene", dn + "_" + (p.split()[0] if p else "none"), rep, ["IsParentVisible"]))
DIFF = True
LIMIT = 8

REPL = {name: pairs for (_unit, name, pairs, _syms) in JOBS}
SCORE = ['IsParentVisible__Q34Chao3CSD6CSceneFv']
