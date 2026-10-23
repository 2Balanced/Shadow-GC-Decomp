/* Boss::Diablon::Effect (class name from RTTI; declared in Game/Diablon.h): the task that loads the
 * Diablon boss's effect files while the boss exists and keeps the launcher of its effects. The
 * boss keeps the task at Diablon::field_0x6C, and its parts launch their effects through the two
 * C functions at the end (other units call them with that word). File name inferred from the
 * class. Names marked "(name inferred)" are not original. Definitions are in reverse address
 * order (deferred inlining emits functions last-defined first). */
#include "Effect/System/EffectElement.h"
#include "Game/Diablon.h"
#include "Game/EffectPJSLauncher.h"
#include "Game/MathTypes.h"
#include "Game/Task.h"
#include "boost/shared_ptr.hpp"
#include "boost/weak_ptr.hpp"
#include <dolphin/types.h>
#include "size_assert.h"

using Effect::System::EffectElement;

/* What CharInfo's word at 0x60 points to (struct name unknown; see Game/DiablonPartsBody.cpp). */
struct UnkCharTime {
    /* 0x0 */ float m_frameTime;
    /* 0x4 */ u32 field_0x4;
};

extern "C" {
/* Loads and unloads the boss's effect files, and the launcher of a loaded effect file (see
 * Game/DiablonSetData.cpp and Game/BlackDoom.cpp; behaviour inferred). */
void fn_801D2DE8(int arg0);
void fn_801D2D14(int arg0);
boost::shared_ptr<Effect::PJS::Launcher> fn_801D2CA4(int index);
/* Launch an effect of launcher, following matrix or not (Game/EffectPJS.cpp). */
boost::weak_ptr<EffectElement> fn_803056F8(Effect::PJS::Launcher *launcher, s32 id, u32 *owner);
boost::weak_ptr<EffectElement> fn_80305900(Effect::PJS::Launcher *launcher, const Mtx34f *matrix,
                                           s32 id, u32 *owner);
}

namespace Boss {
namespace Diablon {

Effect::Effect(Executer *executer, const boost::shared_ptr<Diablon> &owner)
    : Task(executer), m_owner(owner) {
    m_name = "Effect";
    m_owner->field_0x6C = reinterpret_cast<u32>(this);
    fn_801D2DE8(1);
    m_launcher = fn_801D2CA4(1);
}

Effect::~Effect() {
    m_owner->field_0x6C = 0;
    fn_801D2D14(1);
}

void Effect::Tick(float dt) {}

} // namespace Diablon
} // namespace Boss

/* Launches effect id of the boss's effects following matrix (name of the function unknown). */
extern "C" boost::weak_ptr<EffectElement> fn_802DC998(Boss::Diablon::Effect *effect,
                                                      const Mtx34f *matrix, s32 id) {
    return fn_80305900(effect->m_launcher.get(), matrix, id,
                       &reinterpret_cast<UnkCharTime *&>(effect->m_owner->field_0x60)->field_0x4);
}

/* Launches effect id of the boss's effects (name of the function unknown). */
extern "C" boost::weak_ptr<EffectElement> fn_802DC95C(Boss::Diablon::Effect *effect, s32 id) {
    return fn_803056F8(effect->m_launcher.get(), id,
                       &reinterpret_cast<UnkCharTime *&>(effect->m_owner->field_0x60)->field_0x4);
}
