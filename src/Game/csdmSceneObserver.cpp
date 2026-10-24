/* The Chao CSD library's scene observers (the unit after Game/csdmScenePlayMode.cpp; its file name
 * is not known): what a scene observer does when it goes away. The class and member names are
 * inferred (see Game/CsdScene.h).
 *
 * Code generation follows Game/csdmScene.cpp: deferred inlining, so the functions are defined in
 * the reverse of their address order. CSD_SCENE_OBSERVER makes the observer's OnEnd and OnStart
 * this unit's functions, and CSD_PTRLIST_ITERATOR_COPY gives CPtrList's iterator the copy
 * constructor the game calls out of line here. */
#define CSD_OBJECT_LAYER
#define CSD_SCENE_OBSERVER
#define CSD_PTRLIST_ITERATOR_COPY
#include "Game/CsdScene.h"

#pragma pool_data off
#pragma auto_inline off

namespace Chao {
namespace CSD {

/* Stops observing every scene it observes. */
CSceneObserver::~CSceneObserver() {
    CPtrList<CScene>::iterator it;
    for (it = m_subjects.begin(); it != m_subjects.end();) {
        (*it++)->Detach(this);
    }
}

void CSceneObserver::OnStart(CScene *scene) {}

void CSceneObserver::OnEnd(CScene *scene) {}

} // namespace CSD
} // namespace Chao
