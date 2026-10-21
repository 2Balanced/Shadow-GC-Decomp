/* Mission::RingMission (class name from RTTI): the mission of collecting a number of rings. Each
 * frame it copies the player's ring count into the mission's count. The mission factory at
 * 0x8016B384 builds it for mission type 7. Member names are inferred. Definitions are in reverse
 * address order (deferred inlining emits functions last-defined first). */
#include "Game/Mission.h"

/* The game's save data and a player's record in it (struct names unknown; see
 * Game/PlayerShadowWeapon.cpp): the record keeps the ring count at 0x28. */
struct UnkObject_80079808;
struct UnkRecord_80178B5C {
    /* 0x00 */ u8 field_0x0[0x28];
    /* 0x28 */ s32 m_rings;
};

extern "C" {
UnkObject_80079808 *fn_80079808(void);
UnkRecord_80178B5C *fn_80178B5C(UnkObject_80079808 *data, int player);
/* Reads a ring count. */
s32 fn_8016F280(s32 *rings);
}

namespace Mission {

class RingMission : public CountMission {
  public:
    RingMission(int id, int player, int target, int count);
    virtual ~RingMission();
    virtual int GetType();
    virtual void OnUpdate();
};
SIZE_ASSERT(RingMission, 0x9C);

RingMission::RingMission(int id, int player, int target, int count)
    : Mission(id, player), CountMission(id, player, target, 0, count, 0) {}

RingMission::~RingMission() {}

void RingMission::OnUpdate() {
    UnkRecord_80178B5C *record = fn_80178B5C(fn_80079808(), 0);
    if (record) {
        SetCount(fn_8016F280(&record->m_rings), 0);
    }
}

int RingMission::GetType() {
    return 7;
}

} // namespace Mission
