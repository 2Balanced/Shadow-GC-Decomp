"""Wave 40: CScene::Update and IsParentVisible."""
SCORE = ["IsParentVisible__Q34Chao3CSD6CSceneFv"]
D_OLD = "void UpdateScene(float frame, SSceneState *state, const SSceneInfo *info, const SMotion *motion, void *fontList,\n                 SCast *parent);"
C_OLD = "    UpdateScene(m_frame, m_palette->m_resource, m_resource, motion, fontList, parent);\n"
OLD = "        if (state->m_layers[i].m_casts[info->m_layers[i].m_root].m_matrix.m_scale != 0) {\n"
HDR = {
  "float_last": {"Game/CsdScene.h": [(D_OLD, "void UpdateScene(SSceneState *state, const SSceneInfo *info, const SMotion *motion, void *fontList, SCast *parent, float frame);")]},
  "float_2": {"Game/CsdScene.h": [(D_OLD, "void UpdateScene(SSceneState *state, float frame, const SSceneInfo *info, const SMotion *motion, void *fontList, SCast *parent);")]},
  "float_3": {"Game/CsdScene.h": [(D_OLD, "void UpdateScene(SSceneState *state, const SSceneInfo *info, float frame, const SMotion *motion, void *fontList, SCast *parent);")]},
}
REPL = {
  "float_last": [(C_OLD, "    UpdateScene(m_palette->m_resource, m_resource, motion, fontList, parent, m_frame);\n")],
  "float_2": [(C_OLD, "    UpdateScene(m_palette->m_resource, m_frame, m_resource, motion, fontList, parent);\n")],
  "float_3": [(C_OLD, "    UpdateScene(m_palette->m_resource, m_resource, m_frame, motion, fontList, parent);\n")],
  "layer_local": [(OLD, "        SLayer *layer = &state->m_layers[i];\n        if (layer->m_casts[info->m_layers[i].m_root].m_matrix.m_scale != 0) {\n")],
  "layer_ref": [(OLD, "        SLayer &layer = state->m_layers[i];\n        if (layer.m_casts[info->m_layers[i].m_root].m_matrix.m_scale != 0) {\n")],
  "root_first": [(OLD, "        s32 root = info->m_layers[i].m_root;\n        SLayer *layer = &state->m_layers[i];\n        if (layer->m_casts[root].m_matrix.m_scale != 0) {\n")],
  "layer_fn": [("s32 CScene::IsParentVisible() {", "inline SLayer *LayerOf_(SSceneState *state, s32 i) { return &state->m_layers[i]; }\ns32 CScene::IsParentVisible() {"), (OLD, "        if (LayerOf_(state, i)->m_casts[info->m_layers[i].m_root].m_matrix.m_scale != 0) {\n")],
  "layers_local": [(OLD, "        SLayer *layers = state->m_layers;\n        if (layers[i].m_casts[info->m_layers[i].m_root].m_matrix.m_scale != 0) {\n")],
  "ptr_loop": [("    for (s32 i = 0; i < state->m_layerCount; i++) {\n" + OLD, "    for (s32 i = 0; i < state->m_layerCount; i++) {\n        SLayer *layer = state->m_layers + i;\n        SCast *cast = &layer->m_casts[info->m_layers[i].m_root];\n        if (cast->m_matrix.m_scale != 0) {\n")],
  "cast_local": [(OLD, "        SCast *cast = &state->m_layers[i].m_casts[info->m_layers[i].m_root];\n        if (cast->m_matrix.m_scale != 0) {\n")],
}
