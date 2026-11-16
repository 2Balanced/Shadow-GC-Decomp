"""Source shapes for Chao::CSD::CScene::IsParentVisible (wave 39): retail forms the layer's address
(base + offset) and then reads its casts at +4; GC/2.7 folded the +4 into an indexed load."""
SCORE = ["IsParentVisible__Q34Chao3CSD6CSceneFv"]
OLD = "        if (state->m_layers[i].m_casts[info->m_layers[i].m_root].m_matrix.m_scale != 0) {\n"
REPL = {
  "ptr_plus": [(OLD, "        if ((state->m_layers + i)->m_casts[info->m_layers[i].m_root].m_matrix.m_scale != 0) {\n")],
  "ref_layer": [(OLD, "        if ((&state->m_layers[i])->m_casts[info->m_layers[i].m_root].m_matrix.m_scale != 0) {\n")],
  "deref": [(OLD, "        if ((*(state->m_layers + i)).m_casts[info->m_layers[i].m_root].m_matrix.m_scale != 0) {\n")],
  "info_ptr_plus": [(OLD, "        if (state->m_layers[i].m_casts[(info->m_layers + i)->m_root].m_matrix.m_scale != 0) {\n")],
  "casts_local": [(OLD, "        SCastState *casts = state->m_layers[i].m_casts;\n        if (casts[info->m_layers[i].m_root].m_matrix.m_scale != 0) {\n")],
  "root_local": [(OLD, "        s32 root = info->m_layers[i].m_root;\n        if (state->m_layers[i].m_casts[root].m_matrix.m_scale != 0) {\n")],
}
