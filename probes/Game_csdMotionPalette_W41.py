"""Wave 41: InitScene pools the matrix-pool statics but not its constants: SetMatrixPool and
ResetMatrixPool inlined into it (-inline auto) instead of the three stores written out."""
SCORE = ["InitScene__Q24Chao3CSDFPQ34Chao3CSD11SSceneStatePCQ34Chao3CSD10SSceneInfoPv"]
SRC = open("src/Game/csdMotionPalette.cpp", encoding="utf-8").read()
OLD_TAIL = """    s_matrixPool = (f32 *)state->m_matrices;
    s_matrixPoolSize = n;
    s_matrixPoolUsed = 0;
    InitLayers(state, info, texList);
    s_matrixPool = 0;
}
#pragma pool_data off
"""
NEW_TAIL = """    SetMatrixPool((f32 *)state->m_matrices, n);
    InitLayers(state, info, texList);
    ResetMatrixPool();
}
#pragma pool_data off
"""
OLD_HEAD = "#pragma pool_data on\nvoid InitScene("
REPL = {
    "inl_poolon": [(OLD_TAIL, NEW_TAIL)],
    "inl_pooloff": [(OLD_TAIL, NEW_TAIL), (OLD_HEAD, "#pragma pool_data off\nvoid InitScene(")],
    "inl_pooloff_setonly": [(OLD_TAIL, NEW_TAIL.replace("    ResetMatrixPool();\n", "    s_matrixPool = 0;\n")), (OLD_HEAD, "#pragma pool_data off\nvoid InitScene(")],
}
UNUSED = """#pragma pool_data off
void UnusedConstants_(SSceneState *state) {
    state->m_frame = -1.0f;
    state->m_speed = 1.0f;
}
"""
REPL["unused_consts"] = [(OLD_HEAD, UNUSED + OLD_HEAD)]
REPL["unused_consts_inl"] = [(OLD_HEAD, UNUSED + OLD_HEAD), (OLD_TAIL, NEW_TAIL)]
