/* The out-of-line parts of the bosses' control-unit helpers (see Game/BossCtrlUnit.h): the
 * callback containers' Get functions, which bind the containers' handlers into boost::function
 * objects (MotionCallbackContainer::GetCallback, AttackCallbackContainer::GetHitCallback,
 * GetEndCallback and GetDamageCallback), fn_803358D0, which hands a new unit to a boss's unit list,
 * and fn_80335844, the C callback the units register with a motion, which forwards the motion's
 * callback to the boost::function it was given. The file name is inferred. Definitions are in
 * reverse address order (deferred inlining emits functions last-defined first). */
#define MSL_RUNTIME_ERROR_STRING
#define BOSS_CALLBACK_CONTAINER_UNIT
#include <boost/bind.hpp>
#include <boost/function.hpp>

#include "Game/BossCtrlUnit.h"
#include "Game/PooledList.h"
#include "boost/shared_ptr.hpp"

using SonicteamUSA::System::CharaColli::AttackCallback;
using SonicteamUSA::System::CharaColli::AttackCallbackParam;

/* A motion's callback record (see Game/BlackBullCtrlUnit.cpp): the data registered with the
 * callback at 0x4. */
struct _stMtnCallbackParam {
    /* 0x0 */ void *m_frame;
    /* 0x4 */ void *m_data;
    /* 0x8 */ s32 m_id;
};

/* The containers' handlers do nothing; this unit emits the linked weak copy of OnMotion
 * (AttackCallbackContainer's are linked from Game/BlackBull0210.cpp). */
inline void MotionCallbackContainer::OnMotion(_stMtnCallbackParam &param) {}

inline void AttackCallbackContainer::OnHit(AttackCallbackParam &param) {}

inline void AttackCallbackContainer::OnEnd(AttackCallbackParam &param) {}

inline void AttackCallbackContainer::OnDamage(AttackCallbackParam &param) {}

MotionCallbackFunction MotionCallbackContainer::GetCallback() {
    return boost::bind(&MotionCallbackContainer::OnMotion, this, _1);
}

AttackCallback AttackCallbackContainer::GetHitCallback() {
    return boost::bind(&AttackCallbackContainer::OnHit, this, _1);
}

AttackCallback AttackCallbackContainer::GetEndCallback() {
    return boost::bind(&AttackCallbackContainer::OnEnd, this, _1);
}

AttackCallback AttackCallbackContainer::GetDamageCallback() {
    return boost::bind(&AttackCallbackContainer::OnDamage, this, _1);
}

extern "C" {

void fn_803358D0(BossVarParam *param, BossCtrlUnit *unit) {
    param->m_units->PushBack(boost::shared_ptr<BossCtrlUnit>(unit));
}

/* The motion callback the units register (with fn_80415808): data is the unit's motion callback
 * object (name inferred). */
void fn_80335844(_stMtnCallbackParam *param) {
    UnkMotionCallback_802414D4 *callback = static_cast<UnkMotionCallback_802414D4 *>(param->m_data);
    if (callback != 0 && !callback->IsEmpty()) {
        callback->Call(*param);
    }
}
}
