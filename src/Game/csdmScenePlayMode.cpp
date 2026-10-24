/* The Chao CSD library's scene play modes (the unit after Game/csdmProject.cpp; its file name is not
 * known): what a scene's play mode does when the scene's motion reaches its end or its start. The
 * class and member names are inferred (see Game/CsdScene.h).
 *
 * Code generation follows Game/csdmScene.cpp: deferred inlining, so the functions are defined in
 * the reverse of their address order. */
#define CSD_OBJECT_LAYER
#define CSD_SCENE_OBSERVER
#include "Game/CsdScene.h"

#pragma pool_data off
#pragma auto_inline off

namespace Chao {
namespace CSD {

extern "C" {
/* Has the project remove the scene at its next update (Game/csdmProject.cpp). */
void fn_8036D6E0(CProject *project, CScene *scene);
}

void CScenePlayMode::OnEnd(CScene *scene) {
    switch (m_endMode) {
    case 0:
        fn_803659F0(scene);
        break;
    case 1:
        fn_80366740(scene, scene->GetStartFrame());
        break;
    case 2:
        fn_80367140(scene, -1.0f * fn_80367138(scene));
        break;
    case 3:
        fn_8036D6E0(scene->m_project, scene);
        break;
    }
}

void CScenePlayMode::OnStart(CScene *scene) {
    switch (m_startMode) {
    case 0:
        fn_803659F0(scene);
        break;
    case 1:
        fn_80366740(scene, fn_80366728(scene));
        break;
    case 2:
        fn_80367140(scene, -1.0f * fn_80367138(scene));
        break;
    case 3:
        fn_8036D6E0(scene->m_project, scene);
        break;
    }
}

} // namespace CSD
} // namespace Chao
