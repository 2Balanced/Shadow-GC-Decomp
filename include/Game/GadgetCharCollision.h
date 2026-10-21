#ifndef GAME_GADGETCHARCOLLISION_H
#define GAME_GADGETCHARCOLLISION_H

#include <boost/function.hpp>

#include "Game/CharaColli.h"
#include "Game/GadgetColliDetectArgs.h"
#include "boost/shared_ptr.hpp"
#include "size_assert.h"

#ifdef GADGETCHARCOLLISION_IMPL
/* Only Game/GadgetCharCollision.cpp defines GADGETCHARCOLLISION_IMPL: <vector>'s templates and
 * the inline CallIfSet below would renumber the compiler's @N local symbols in the units that
 * include this header for the handle's other members. */
#include <utility>
#include <vector>
#endif

/* A callback member of GadgetCharCollision (class name unknown): a function1 over the collision
 * event. Its implicit copy assignment is the game's fn_800F5980; its implicit constructor and
 * destructor are emitted by Game/GadgetCharCollision.cpp (the destructor destroys the function as a
 * member, not as a base). */
class UnkColliCallback_800F5980 {
  public:
#ifdef GADGETCHARCOLLISION_IMPL
    /* Calls the function when one is set (name inferred). */
    void CallIfSet(GadgetColliDetectArgs &args) const {
        if (!m_function.empty()) {
            m_function(args);
        }
    }

#endif
    /* 0x0 */ boost::function1<void, GadgetColliDetectArgs &> m_function;
};

/* The characters a GadgetCharCollision ignores, as (word 0x5C, word 0x50) pairs of their
 * CharInfo (class name unknown; a std::vector in the game, wrapped as UnkTargetList_80224850
 * wraps its vector). Other units see only its size. */
#ifdef GADGETCHARCOLLISION_IMPL
struct UnkIgnoreList_8015C2A0 : public std::vector<std::pair<s32, s32> > {};
#else
struct UnkIgnoreList_8015C2A0 {
    /* 0x0 */ u8 field_0x0[0xC];
};
#endif
SIZE_ASSERT(UnkIgnoreList_8015C2A0, 0xC);

/* GadgetCharCollision (class name from the RTTI of its shared_ptr control block,
 * sp_counted_base_impl<GadgetCharCollision *, checked_deleter<GadgetCharCollision> >): the
 * contact handle fn_8015B8C4 wraps a gadget's collider in. fn_8015BE9C, called every frame, calls
 * m_onEnter when characters start touching the collider, m_onContact while they touch it and
 * m_onLeave when they stop (member names inferred from that use). Game/GadgetCharCollision.cpp. */
class GadgetCharCollision {
  public:
    GadgetCharCollision(boost::shared_ptr<SonicteamUSA::System::CharaColli::CharaColliBase> collider);
    ~GadgetCharCollision();

    /* True when every character touching the collider is in m_ignore (name inferred). */
    bool IsAllIgnored(UnkColliContacts_8001EE04 *contacts);

    /* Bit 0: m_timer runs (see fn_8015BE9C); bit 1: contacts are not reported; bit 2: characters
     * are touching (meanings inferred). */
    enum Flag {
        FLAG_TIMER = 1,
        FLAG_DISABLED = 2,
        FLAG_TOUCHING = 4,
    };

    /* Turn the contact reports off and on (names inferred; inline in the units that use them). */
    void TurnOff() {
        m_flags |= FLAG_DISABLED;
    }
    void TurnOn() {
        m_flags &= ~FLAG_DISABLED;
    }
    /* Stops the reports for time (name inferred; see fn_8015BE9C). */
    void StartTimer(float time) {
        m_flags |= FLAG_TIMER;
        m_timer = time;
    }

    /* 0x00 */ UnkColliCallback_800F5980 m_onContact;
    /* 0x0C */ UnkColliCallback_800F5980 m_onEnter;
    /* 0x18 */ UnkColliCallback_800F5980 m_onLeave;
    /* 0x24 */ boost::shared_ptr<SonicteamUSA::System::CharaColli::CharaColliBase> m_collider;
    /* 0x2C */ u32 m_flags;
    /* 0x30 */ float m_timer;
    /* 0x34 */ UnkIgnoreList_8015C2A0 m_ignore;
};
SIZE_ASSERT(GadgetCharCollision, 0x40);

#endif
