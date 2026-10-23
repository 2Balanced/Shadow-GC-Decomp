/* csdmMotionPalette.cpp (file name from the allocator's records): CMotionPalette, the run-time
 * state of a scene (Game/csdMotionPalette.cpp sets it up): making it, rebuilding its layers with
 * the companion matrices its cast tree needs, and making the pattern of one of its casts. Class
 * names other than RCPtr, and all member names, are inferred.
 *
 * Code generation follows Game/csdmScene.cpp: deferred inlining, so the functions are defined in
 * the reverse of their address order. */
#define CSD_OBJECT_LAYER
#include "Game/CsdScene.h"

#pragma pool_data off
#pragma auto_inline off

namespace Chao {
namespace CSD {

s32 GetSceneStateSize(const SSceneInfo *info);
void InitScene(SSceneState *state, const SSceneInfo *info, void *fontList);
void DestroyScene(SSceneState *state);
s32 CountMatrices(SSceneState *state, const SSceneInfo *info);
void SetMatrixPool(f32 *pool, s32 size);
void ResetMatrixPool();

/* Frees a run-time state (its data block's deallocation). */
static void DeleteState(unsigned char *state) {
    DestroyScene((SSceneState *)state);
    if (state != 0) {
        fn_80360528(state);
    }
}

CMotionPalette::CMotionPalette(const SSceneInfo *info, void *fontList) {
    m_resource = (SSceneState *)fn_80360564(GetSceneStateSize(info), "csdmMotionPalette.cpp", 0x37);
    m_data.Reset((unsigned char *)m_resource, RCFreeFunc<unsigned char>(DeleteState));
    InitScene(m_resource, info, fontList);
}

/* The pattern of the cast entry names, made in memory. */
CMotionPattern *CMotionPalette::CreatePattern(void *memory, const SNameIndex2 *entry) {
    SCast *cast = &m_resource->m_layers[entry->m_index].m_casts[entry->m_value];
    return new (memory) CMotionPattern(cast, m_data);
}

void CMotionPalette::Rebuild(CScene *scene, CFontList *fontList) {
    SSceneState *state = m_resource;
    if (state->m_matrices != 0) {
        fn_80360528(state->m_matrices);
        state->m_matrices = 0;
    }
    s32 count = CountMatrices(state, scene->m_resource);
    if (count > 0) {
        state->m_matrices = fn_80360564(count * 0x18, "csdmMotionPalette.cpp", 100);
    }
    SetMatrixPool((f32 *)state->m_matrices, count);
    InitLayers(state, scene->m_resource, fontList->m_resource);
    ResetMatrixPool();
}

} // namespace CSD
} // namespace Chao
