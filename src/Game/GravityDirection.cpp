/* The stage's gravity (no RTTI; file, struct and function names are inferred). A stage that changes
 * gravity creates a small record of the current direction (fn_80293B1C) and destroys it again
 * (fn_80293AE4); GravityChangeSwitch, GravityChangeCollision and GravityChangeZone set it
 * (fn_80293928), which sends every character a Player::SetGravityCommand towards the new
 * direction. The six directions are kept in a table (fn_80293B60): 0 up, 1 down, 2 +X, 3 -X, 4 +Z,
 * 5 -Z (from the static initializer). Definitions are in reverse address order (deferred inlining
 * emits functions last-defined first). */
#include "Game/CharCommand.h"
#include "Game/CharInfo.h"
#include "Game/MathTypes.h"
#include "boost/shared_ptr.hpp"
#include "boost/weak_ptr.hpp"
#include "size_assert.h"

struct UnkCharManager_8006F754;
struct UnkCharaList_80074194;

extern s32 lbl_805EE588;

extern "C" {
UnkCharaList_80074194 *fn_80074194(void);
boost::weak_ptr<CharInfo> fn_800A8B74(UnkCharaList_80074194 *list, int id);
/* The NPC manager and its character at index (behaviour inferred). */
UnkCharManager_8006F754 *fn_8006F754(void);
boost::weak_ptr<CharInfo> fn_8007488C(UnkCharManager_8006F754 *manager, int index);
}

/* The up direction (read by GravitySignBoard as well). */
Vec3f lbl_80584360(0.0f, 1.0f, 0.0f);

/* The six gravity directions. The original copy-constructs them through the vector type's
 * out-of-line copy constructor, fn_800091EC; this wrapper's constructor writes those copies out so
 * that the startup code stays the same (as in Game/BossSonicControlRestart.cpp). */
static struct GravityDirections {
    GravityDirections() {
        float back[3];
        float front[3];
        float left[3];
        float right[3];
        float down[3];

        fn_800091EC(reinterpret_cast<Vec3f *>(m_directions[0]), &lbl_80584360);
        down[0] = 0.0f;
        down[1] = -1.0f;
        down[2] = 0.0f;
        fn_800091EC(reinterpret_cast<Vec3f *>(m_directions[1]), reinterpret_cast<Vec3f *>(down));
        right[0] = 1.0f;
        right[1] = 0.0f;
        right[2] = 0.0f;
        fn_800091EC(reinterpret_cast<Vec3f *>(m_directions[2]), reinterpret_cast<Vec3f *>(right));
        left[0] = -1.0f;
        left[1] = 0.0f;
        left[2] = 0.0f;
        fn_800091EC(reinterpret_cast<Vec3f *>(m_directions[3]), reinterpret_cast<Vec3f *>(left));
        front[0] = 0.0f;
        front[1] = 0.0f;
        front[2] = 1.0f;
        fn_800091EC(reinterpret_cast<Vec3f *>(m_directions[4]), reinterpret_cast<Vec3f *>(front));
        back[0] = 0.0f;
        back[1] = 0.0f;
        back[2] = -1.0f;
        fn_800091EC(reinterpret_cast<Vec3f *>(m_directions[5]), reinterpret_cast<Vec3f *>(back));
    }

    /* 0x00 */ float m_directions[6][3];
} lbl_8058436C;

/* The current gravity (struct name unknown). */
struct GravityState {
    GravityState() : m_gravity(0) {}
    ~GravityState() {}

    /* 0x0 */ s32 m_gravity;
};
SIZE_ASSERT(GravityState, 0x4);

static GravityState *lbl_805F0340;

/* The direction of gravity (name inferred). */
extern "C" const Vec3f *fn_80293B60(int gravity) {
    return reinterpret_cast<Vec3f *>(lbl_8058436C.m_directions[gravity]);
}

/* Creates the gravity record (name inferred). */
extern "C" void fn_80293B1C(void) {
    if (lbl_805F0340 == 0) {
        lbl_805F0340 = new GravityState;
    }
}

/* Destroys the gravity record (name inferred). */
extern "C" void fn_80293AE4(void) {
    if (lbl_805F0340 != 0) {
        delete lbl_805F0340;
        lbl_805F0340 = 0;
    }
}

/* The current gravity, 0 without a record (name inferred). */
extern "C" int fn_80293AC8(void) {
    if (lbl_805F0340 != 0) {
        return lbl_805F0340->m_gravity;
    }
    return 0;
}

/* Makes gravity the current one and turns the player and the NPCs towards it (name inferred). */
extern "C" void fn_80293928(int gravity) {
    if (lbl_805F0340 != 0) {
        lbl_805F0340->m_gravity = gravity;
        Vec3f direction;
        fn_800091EC(&direction, reinterpret_cast<Vec3f *>(lbl_8058436C.m_directions[lbl_805F0340->m_gravity]));
        boost::shared_ptr<CharInfo> player = fn_800A8B74(fn_80074194(), lbl_805EE588).lock();
        if (player) {
            player->PushCommand(new Player::SetGravityCommand(direction, 4));
        }
        for (int i = 1; i < 14; i++) {
            boost::weak_ptr<CharInfo> npc = fn_8007488C(fn_8006F754(), i);
            if (!npc.expired()) {
                boost::shared_ptr<CharInfo> chara = npc.lock();
                chara->PushCommand(new Player::SetGravityCommand(direction, 4));
            }
        }
    }
}
