#ifndef GAME_NPCMOTION_H
#define GAME_NPCMOTION_H

#include "Game/MathTypes.h"
#include "Game/Npc.h"
#include "Game/PlayerWeaponBase.h"
#include "boost/shared_ptr.hpp"
#include <dolphin/types.h>
#include "size_assert.h"

class UnkClumpUpdater_80015A7C;

/* The fields of an NPC its motion reads (the NPC class's layout is not mapped; names inferred). */
struct UnkNpcMotionFields_801FD130 {
    /* 0x000 */ Vec3f m_position;
    /* 0x00C */ Vec3f m_rotation;
    /* 0x018 */ u8 field_0x18[0xB4 - 0x18];
    /* 0x0B4 */ u32 m_motionFlags; /* a flag word, cleared when the motion changes */
    /* 0x0B8 */ u8 field_0xB8[0xC4 - 0xB8];
    /* 0x0C4 */ Vec3f m_velocity;
    /* 0x0D0 */ u8 field_0xD0[0x198 - 0xD0];
    /* 0x198 */ Mtx34f m_modelMatrix; /* (meaning inferred) */
    /* 0x1C8 */ u8 field_0x1C8[0x278 - 0x1C8];
    /* 0x278 */ void *m_shadow; /* passed to fn_803563D0 (meaning inferred) */
};

namespace Player {
namespace Npc {

/* NpcMotion (class name from RTTI; bases Task and Player::PlayerMotion; vtable 0x80542788): the
 * motion of a non-player character. The characters' MotionImpl classes derive from it; it keeps the
 * NPC and offers them the model handling they share. Slots 0x3C-0x44 are pure. The file name and
 * all method names are inferred. */
class NpcMotion : public PlayerMotion {
  public:
    NpcMotion(Task *parent, const boost::shared_ptr<Npc> &npc);
    virtual ~NpcMotion();
    /* Shows and hides the model, and loads the motion once the model's resource is ready (names
     * inferred from Player::Npc::EggMonitor::MotionImpl). */
    virtual void Show() = 0;
    virtual void Hide() = 0;
    virtual void Load() = 0;

    /* Creates and destroys a part of the model: a clump of the NPC's model resource kept on the
     * motion, in the NPC's world. */
    static UnkClumpUpdater_80015A7C *CreatePart(boost::shared_ptr<PlayerBase> player,
                                                _stMtnPlayWork *work, void *model, int index);
    static void DestroyPart(UnkClumpUpdater_80015A7C *part);

    /* Moves the NPC by the motion's root. */
    void MoveByRoot(const Vec3f &offset);
    /* Places the motion and the model's matrices on the NPC. */
    void PlaceOnNpc(const Vec3f &offset);
    /* Finds the skeleton nodes (-1: the model's matrix, -2: the character's). */
    void BindNodes(const int *nodes);
    /* Loads the motion when the NPC's model resource is ready; whether it was. */
    bool LoadWhenReady();
    /* Shows or hides the model (hidden while the NPC is, and blinking); whether it is shown. */
    bool UpdateVisibility();

    /* 0x458 */ boost::shared_ptr<Npc> m_npc;
};
SIZE_ASSERT(NpcMotion, 0x460);

} // namespace Npc
} // namespace Player

#endif
