/* AdvReal::AdvCsdEffect (class name from the RTTI of its shared_ptr's control block; see
 * Game/AdvCsdEffect.h): the two looping CSD scenes ("Loop" and "Loop/Loop") the front-end phases
 * play over their menus. The functions other units call keep their fn_ names; the file name comes
 * from the class name and the other names are inferred. Definitions are in reverse address order
 * (deferred inlining emits functions last-defined first). */
#include "Game/AdvCsdEffect.h"
#include "Game/CsdRCPtr.h"
#include <dolphin/types.h>
#include "size_assert.h"

using Chao::CSD::CScene;
using Chao::CSD::CTexList;
using Chao::CSD::RCPtr;

/* The value a project returns from fn_8036D598 (class name unknown; declared as in Game/TGi.h). */
class UnkCsdHandle_80042DF0 {
  public:
    UnkCsdHandle_80042DF0(const UnkCsdHandle_80042DF0 &other);
    ~UnkCsdHandle_80042DF0();

    /* The object the handle holds (name inferred; see Game/TGi.h). */
    void *Get() const {
        return m_object != 0 ? *(void **)m_object : 0;
    }

    /* 0x0 */ void *m_object;
};

/* An empty scene pointer (meaning inferred; a scene equal to it is missing). */
extern RCPtr<CScene> lbl_805EE118;

extern "C" {
/* A scene of the object a project handle holds, by name (see Game/ResultTask.cpp). */
RCPtr<CScene> fn_8036F470(void *object, const char *name);
/* Scenes (see Game/TGi.h and Game/CsdRCPtr.h): the motion, playing, ending, and a step of the
 * scene's animation (behaviour inferred). */
void fn_80367238(CScene *scene, const char *motion);
void fn_803659E4(CScene *scene);
void fn_803659F0(CScene *scene);
void fn_80366F9C(CScene *scene);
}

/* The motion both scenes play, and the scenes (names inferred). */
static const char *s_motion = "Loop";
static const char *s_sceneNames[2] = {"Loop", "Loop/Loop"};

namespace AdvReal {

AdvCsdEffect::AdvCsdEffect(UnkCsdHandle_80042DF0 project) {
    Bind(project);
}

AdvCsdEffect::~AdvCsdEffect() {
    Release();
}

} // namespace AdvReal

/* Advances the scenes (behaviour inferred). */
extern "C" void fn_80355F54(AdvReal::AdvCsdEffect *effect) {
    for (int i = 0; i < 2; i++) {
        CScene *scene = effect->m_scenes[i].Get();
        if (scene != lbl_805EE118.Get()) {
            fn_80366F9C(effect->m_scenes[i].Get());
        }
    }
}

/* Passes a project's texture list to the scenes (behaviour inferred). */
extern "C" void fn_80355EB0(AdvReal::AdvCsdEffect *effect, const RCPtr<CTexList> &texList) {
    for (int i = 0; i < 2; i++) {
        CScene *scene = effect->m_scenes[i].Get();
        if (scene != lbl_805EE118.Get()) {
            effect->m_scenes[i].Get()->SetTexList(&texList);
        }
    }
}

namespace AdvReal {

bool AdvCsdEffect::Bind(UnkCsdHandle_80042DF0 project) {
    bool found = false;
    for (int i = 0; i < 2; i++) {
        m_scenes[i] = fn_8036F470(project.Get(), s_sceneNames[i]);
        CScene *current = m_scenes[i].Get();
        if (current != lbl_805EE118.Get()) {
            fn_80367238(m_scenes[i].Get(), s_motion);
            fn_803659E4(m_scenes[i].Get());
            found = true;
        }
    }
    return found;
}

void AdvCsdEffect::Release() {
    for (int i = 0; i < 2; i++) {
        CScene *current = m_scenes[i].Get();
        if (current != lbl_805EE118.Get()) {
            fn_803659F0(m_scenes[i].Get());
            m_scenes[i] = lbl_805EE118;
        }
    }
}

} // namespace AdvReal
