#ifndef GAME_CSDSCENESET_H
#define GAME_CSDSCENESET_H

#include "Game/CsdSceneCtrl.h"
#include <dolphin/types.h>
#include "size_assert.h"

namespace Chao {
namespace CSD {
class CTexList;
} // namespace CSD
} // namespace Chao

class UnkCsdHandle_80042DF0;

#ifdef TPJS_SCENE_TOUR_IMPL
#include "boost/shared_ptr.hpp"

/* A scene of the set: its tour, name and whether it is shown (struct name unknown). */
struct UnkCsdSceneSetEntry_8003D5D8 {
    /* 0x00 */ boost::shared_ptr<TPjsSceneTour> m_tour;
    /* 0x08 */ std::string m_name;
    /* 0x14 */ bool m_visible;
};
#endif

/* A named set of CSD scenes, each with its motion queue (class name unknown; 0xC bytes, built by
 * the constructor at 0x8003D5D8 and destroyed at 0x8003D534). The front-end phases keep one. Its
 * functions are around 0x8003C800; their names and meanings are inferred. */
class UnkCsdSceneSet_8003D5D8 {
  public:
    UnkCsdSceneSet_8003D5D8();
    ~UnkCsdSceneSet_8003D5D8();

#ifdef TPJS_SCENE_TOUR_IMPL
    /* (Where TPJS_SCENE_TOUR_IMPL is defined, Game/TPjsCsd.cpp, the unit with the class's code: the
     * real type, a vector of named scenes.) */
    /* 0x0 */ std::vector<UnkCsdSceneSetEntry_8003D5D8> m_entries;
#else
    /* 0x0 */ u8 field_0x0[0xC];
#endif
};
SIZE_ASSERT(UnkCsdSceneSet_8003D5D8, 0xC);

extern "C" {
/* Adds count scenes of the project, named names, to the set (behaviour inferred). */
bool fn_8003CEF8(UnkCsdSceneSet_8003D5D8 *set, UnkCsdHandle_80042DF0 project, const char *const *names,
                 int count);
/* The scene named name, or 0 (behaviour inferred). */
TPjsSceneTour *fn_8003CEAC(UnkCsdSceneSet_8003D5D8 *set, const char *name);
/* Shows or hides the scene named name (behaviour inferred). */
bool fn_8003C8D8(UnkCsdSceneSet_8003D5D8 *set, const char *name, bool visible);
/* Rewinds every scene (behaviour inferred). */
void fn_8003CE5C(UnkCsdSceneSet_8003D5D8 *set);
/* Passes the project's texture list to the visible scenes (behaviour inferred). */
bool fn_8003C92C(UnkCsdSceneSet_8003D5D8 *set, const Chao::CSD::RCPtr<Chao::CSD::CTexList> &texList);
/* Plays the queued motions; whether any scene is still busy (behaviour inferred). */
bool fn_8003CDF4(UnkCsdSceneSet_8003D5D8 *set);
/* Whether a scene's queued motions have all been played (behaviour inferred). */
bool fn_8003DBB0(TPjsSceneTour *scene);
}

#endif
