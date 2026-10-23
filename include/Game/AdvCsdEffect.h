#ifndef GAME_ADVCSDEFFECT_H
#define GAME_ADVCSDEFFECT_H

#include "Game/CsdRCPtr.h"
#include <dolphin/types.h>
#include "size_assert.h"

namespace Chao {
namespace CSD {
/* CTexList (class name from the RTTI of RCPtr<CTexList>::RCObject<CTexList>): a project's texture
 * list. */
class CTexList;
} // namespace CSD
} // namespace Chao

/* The value a CSD project returns from fn_8036D598 (class name unknown; declared as in TGi.h). */
class UnkCsdHandle_80042DF0;

namespace AdvReal {

/* AdvCsdEffect (class name from the RTTI of its shared_ptr's control block): two CSD scenes the
 * front-end phases (Game/AdvOption.cpp, Game/AdvStoryLibrary.cpp) play over their menus. Its
 * functions are in Game/AdvCsdEffect.cpp: the constructor at 0x80356044, the destructor at
 * 0x80355FE4, and fn_80355EB0 and fn_80355F54 (see below). Member names are inferred. */
class AdvCsdEffect {
  public:
    AdvCsdEffect(UnkCsdHandle_80042DF0 project);
    ~AdvCsdEffect();

    /* Takes the scenes from a project and plays their loop; true when one was found (0x80355DAC;
     * name inferred). */
    bool Bind(UnkCsdHandle_80042DF0 project);
    /* Ends and drops the scenes (0x80355D0C; name inferred). */
    void Release();

    /* 0x0 */ Chao::CSD::RCPtr<Chao::CSD::CScene> m_scenes[2];
};
SIZE_ASSERT(AdvCsdEffect, 0x8);

} // namespace AdvReal

extern "C" {
/* Passes a project's texture list to the scenes, and advances them (behaviour inferred). */
void fn_80355EB0(AdvReal::AdvCsdEffect *effect, const Chao::CSD::RCPtr<Chao::CSD::CTexList> &texList);
void fn_80355F54(AdvReal::AdvCsdEffect *effect);
}

#endif
