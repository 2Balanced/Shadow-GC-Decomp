#ifndef GAME_EGGLASTMECHSLOTSWITCH_H
#define GAME_EGGLASTMECHSLOTSWITCH_H

/* EggLastMech::SlotSwitch (class name from the RTTI of its attack callback's bind_t): a reel of the
 * boss EggLastMech's slot machine, defined in Game/EggLastMechSlotSwitch.cpp and owned by the slot
 * machine of Game/EggLastMechSlot.cpp. Member names are inferred. */

#include "Effect/System/EffectElement.h"
#include "Game/CharaColli.h"
#include "Game/CharaColliAttack.h"
#include "Game/EnemyRender.h"
#include "boost/shared_ptr.hpp"
#include "boost/weak_ptr.hpp"
#include <dolphin/types.h>
#include "size_assert.h"

class Target;

namespace EggLastMech {

class EggLastMech;

class SlotSwitch {
  public:
    SlotSwitch(EggLastMech *mech, int index);
    ~SlotSwitch();

    /* The attack callback of the reel's collider. */
    void OnAttack(SonicteamUSA::System::CharaColli::AttackCallbackParam &param);
    /* Starts the reel spinning. */
    void Start();
    /* Stops the reel on symbol. */
    void Stop(int symbol);
    /* Turns the reel and places its model, collider and target. */
    void Tick();

    /* 0x00 */ s32 m_state; /* 0 start, 1 spinning, 2 stopping, 3 settling, 4 stopped */
    /* 0x04 */ EggLastMech *m_mech;
    /* 0x08 */ s32 m_index;
    /* 0x0C */ s32 m_node; /* the boss's motion node the reel sits on */
    /* 0x10 */ s32 m_symbol;
    /* 0x14 */ float m_timer;
    /* 0x18 */ boost::shared_ptr<EnemyRender> m_render;
    /* 0x20 */ boost::shared_ptr<SonicteamUSA::System::CharaColli::CharaColliBase> m_collider;
    /* 0x28 */ boost::shared_ptr<Target> m_target;
    /* 0x30 */ boost::weak_ptr<Effect::System::EffectElement> m_effect;
    /* 0x38 */ float m_angle; /* the reel's turn, 0 to 5/6 */
    /* 0x3C */ u8 m_spinning;
    /* 0x40 */ void *m_motion;
};

} // namespace EggLastMech

#endif
