/* Player::Npc::Sonic::MotionImpl (class name from RTTI; bases Task, Player::PlayerMotion and
 * Player::Npc::NpcMotion): the motion of the Sonic NPC. Once the NPC's model resource is ready it
 * loads the motion, a model part and the hands (Player::Npc::NpcHand), then each frame places the
 * motion on the NPC, keeps the part on it and moves the NPC by the motion's root. Each motion of a
 * table brings two step callbacks: the run picks its speed's motion, the turn faces the player's
 * direction, and the others wait for the player before going on. The unit also holds
 * PlayerMotion's empty virtual functions and the game's copy of vector<void *>'s push_back and
 * fill-insert. The file name and all function and member names are inferred. Definitions are in
 * reverse address order (deferred inlining emits functions last-defined first). */
#define PTRVECTOR_INSERT
#define MSL_ALGORITHM_COPY
#include "Game/CharaColliBaseCharInfo.h"
#include "Game/DevilDoomCtrlUnit.h"
#include "Game/MathTypes.h"
#include "Game/Npc.h"
#include "Game/NpcHand.h"
#include "Game/NpcMotion.h"
#include "Game/NpcPartModel.h"
#include "Game/PlayerWeaponBase.h"
#include "Game/PtrVector.h"
#include "boost/shared_ptr.hpp"
#include <dolphin/types.h>
#include "size_assert.h"

extern "C" {
/* PlayerMotion's own functions (behaviour inferred). */
void fn_800A9F9C(Player::PlayerMotion *motion, float frame);
float fn_800AA064(Player::PlayerMotion *motion);
void fn_800AA3A4(Player::PlayerMotion *motion);
void fn_800A9EA0(Player::PlayerMotion *motion);
void *fn_800AA584(Player::PlayerMotion *motion);
/* Whether the player may start a motion (behaviour inferred). */
bool fn_800A9E1C(const boost::shared_ptr<Player::PlayerBase> &player);
/* The side a direction lies on as seen by the player (0 to 3; behaviour inferred). */
int fn_80093C60(const boost::shared_ptr<Player::PlayerBase> *player, const Vec3f *direction);

/* The resource loader (see Game/BossMotion.cpp). */
void *fn_800122D4(void);
void *fn_80012684(void *loader, void *resource, int index);
s32 fn_80012608(void *loader, void *resource, int index);

/* Motions (see Game/BossMotion.cpp). */
_stMtnPlayWork *fn_8041B908(void *data, int arg1);
void fn_8041B884(_stMtnPlayWork *work);
void fn_80415EA4(_stMtnPlayWork *work);

/* A flag word (see Game/GadgetModelInfo.h). */
void fn_80014E10(u32 *flags);
void fn_80014E1C(u32 *flags, int bit);
bool fn_80014E34(u32 *flags, int bit);
void fn_8006127C(u32 *flags, int bit);

/* The part: placed on a matrix, or on the motion; shown and hidden (behaviour inferred). */
void fn_80015954(UnkClumpUpdater_80015A7C *part, const Mtx34f *matrix);
void fn_800159AC(UnkClumpUpdater_80015A7C *part);
void fn_80038F94(RpClump *clump);
void fn_80038FC0(RpClump *clump);

/* The NPC: the offset of its model, an update and its shadow (behaviour inferred). */
Vec3f fn_8007378C(Player::Npc::Npc *npc);
void fn_80079860(Player::Npc::Npc *npc);
void fn_803563D0(void *shadow, float arg1);

/* Vectors (see Game/ArkCannon.cpp). */
void fn_8040F990(Vec3f *along, Vec3f *across, const Vec3f *vector, const Vec3f *normal);
float fn_8040CE74(Vec3f *v);
}

/* Passes a returned value on by address (as in Game/ArkCannon.cpp's casts). */
inline const Vec3f &Ref(const Vec3f &vector) {
    return vector;
}

inline const Mtx34f &RefM(const Mtx34f &matrix) {
    return matrix;
}

/* Tests a bit of a bit array (the player's state bits; other units call it by its address). */
extern "C" bool fn_8006CD78(const u32 *bits, int bit) {
    return (bits[(u32)bit >> 5] & (1 << (bit & 0x1F))) != 0;
}

/* The fields of a player the turn reads (names inferred). */
struct UnkPlayerFields_8006C550 {
    /* 0x000 */ u8 field_0x0[0x18];
    /* 0x018 */ Vec3f m_direction;
    /* 0x024 */ u8 field_0x24[0xF4 - 0x24];
    /* 0x0F4 */ Vec3f m_up;
};

namespace Player {
namespace Npc {
namespace Sonic {

class MotionImpl;

/* What a motion runs before and after the motion is placed each frame (struct name unknown). */
struct MotionSteps {
    void (MotionImpl::*m_before)();
    void (MotionImpl::*m_after)();
};

/* A motion of the table (struct name unknown): the motion's number, its index in the model's
 * motions, the index it switches to once the player allows (-1: none), the hands' pose and its
 * steps. */
struct MotionEntry {
    /* 0x00 */ s32 m_motion;
    /* 0x04 */ s32 m_index;
    /* 0x08 */ s32 m_nextIndex;
    /* 0x0C */ s32 m_handPose;
    /* 0x10 */ const MotionSteps *m_steps;
};

class MotionImpl : public NpcMotion {
  public:
    MotionImpl(Task *parent, const boost::shared_ptr<Npc> &npc)
        : NpcMotion(parent, npc), m_state(0), m_part(0), m_hand(0) {
        m_before = 0;
        m_after = 0;
    }
    virtual ~MotionImpl();
    virtual void Tick(float dt);
    virtual void vf_0x10(int motion, float blendTime);
    virtual void vf_0x14(UnkClumpList_8006CAE4 *list);
    virtual void Show();
    virtual void Hide();
    virtual void Load();

    static NpcMotion *Create(Task *parent, const boost::shared_ptr<Npc> &npc);

    /* The run's speed and how far it is into its speed's motion (the words at 0x438 and 0x448;
     * names inferred). */
    float &RunSpeed() {
        return *reinterpret_cast<float *>(&field_0x438);
    }
    float &RunBlend() {
        return *reinterpret_cast<float *>(&field_0x444[4]);
    }

    void Run(float dt);
    void StartMotion();
    void WaitForPlayer();
    void SelectRunMotion();
    void TurnToPlayer();
    void FinishWithPlayer();
    void WaitForLanding();

    /* 0x460 */ s32 m_state; /* 0: waiting for the model resource, 1: running */
    /* 0x464 */ UnkClumpUpdater_80015A7C *m_part;
    /* 0x468 */ NpcHand *m_hand;
    /* 0x46C */ void (MotionImpl::*m_before)();
    /* 0x478 */ void (MotionImpl::*m_after)();
    /* 0x484 */ u32 field_0x484;
};
SIZE_ASSERT(MotionImpl, 0x488);

/* The model's hands and the skeleton nodes. */
static const int s_hands[4] = {3, 5, 4, 6};
static const int s_nodes[14] = {-1, -2, 0x1E, 0x18, 0x18, 0x1C, 0x1C, 0x2C, 0x2B, 0x30, 0x2F, 0x28, -1, -1};

static MotionSteps s_steps = {&MotionImpl::StartMotion, &MotionImpl::WaitForPlayer};
static MotionSteps s_runSteps = {&MotionImpl::SelectRunMotion, 0};
static MotionSteps s_turnSteps = {&MotionImpl::TurnToPlayer, 0};
static MotionSteps s_finishSteps = {0, &MotionImpl::FinishWithPlayer};
static MotionSteps s_landSteps = {0, &MotionImpl::WaitForLanding};

static const MotionEntry s_motions[] = {
    {0, 0x11, -1, 0, &s_steps},
    {1, -1, -1, 0, &s_runSteps},
    {9, 3, 4, 1, &s_steps},
    {5, 0x13, -1, 1, &s_steps},
    {6, 0xD, -1, 0, &s_steps},
    {7, -1, -1, 0, &s_turnSteps},
    {0xC, 0x14, -1, 0, &s_finishSteps},
    {3, 0x17, -1, 0, &s_steps},
    {4, 0x18, -1, 0, &s_steps},
    {0x26, 0xD, -1, 0, &s_steps},
    {0xB, 0xB, -1, 1, &s_steps},
    {0x29, 8, -1, 0, &s_landSteps},
    {0x1D, 0x19, -1, 0, &s_steps},
    {0x1E, 1, -1, 0, &s_steps},
    {8, 2, -1, 0, &s_steps},
    {0x2E, 0x1A, -1, 0, &s_steps},
    {0x30, 0, 0, 0, 0},
};

/* The run's motions by speed: the motion and the one blended with it. */
static const int s_runMotions[4][2] = {{0x11, 0x23}, {0x23, 0x1E}, {0x1E, 0x1B}, {0x1B, 0x1F}};

/* The table's entry of motion (the first if it has none; name inferred). */
inline const MotionEntry *FindMotion(int motion) {
    for (const MotionEntry *entry = s_motions; entry->m_motion < 0x30; entry++) {
        if (entry->m_motion == motion) {
            return entry;
        }
    }
    return s_motions;
}

MotionImpl::~MotionImpl() {
    DestroyPart(m_part);
    m_part = 0;
    if (m_work) {
        fn_8041B884(m_work);
    }
    m_work = 0;
}

void MotionImpl::Tick(float dt) {
    PlayerMotion::Tick(dt);
    switch (m_state) {
    case 0:
        if (LoadWhenReady()) {
            m_state = 1;
        }
        break;
    case 1:
        Run(dt);
        break;
    }
}

void MotionImpl::vf_0x10(int motion, float blendTime) {
    if (m_motionId != motion) {
        m_prevMotionId = m_motionId;
        m_motionId = motion;
        m_blendTime = blendTime;
        m_speed = 1.0f;
        const MotionEntry *entry = FindMotion(m_motionId);
        int index = entry->m_index;
        if (index >= 0) {
            m_nextMotion = index + (m_motionBase - 1);
            m_nextMotionArg = -1;
        }
        m_before = entry->m_steps->m_before;
        m_after = entry->m_steps->m_after;
        m_step = 0;
        fn_80014E10(&reinterpret_cast<UnkNpcMotionFields_801FD130 *>(m_npc.get())->m_motionFlags);
        if (m_hand) {
            m_hand->SetPose(entry->m_handPose);
        }
    }
}

} // namespace Sonic
} // namespace Npc
} // namespace Player

/* The clump list's push_back (an instance of vector<RpClump *>; other units call it by its
 * address). */
extern "C" void fn_8006CB04(std::vector<void *, std::allocator<void *> > *list, void *const *x) {
    list->push_back(*x);
}

extern "C" void fn_8006CAE4(Player::UnkClumpList_8006CAE4 *list, RpClump *const *clump) {
    fn_8006CB04(reinterpret_cast<std::vector<void *, std::allocator<void *> > *>(list),
                reinterpret_cast<void *const *>(clump));
}

namespace Player {
namespace Npc {
namespace Sonic {

void MotionImpl::vf_0x14(UnkClumpList_8006CAE4 *list) {
    if (fn_80014E34(&m_flags, 2)) {
        RpClump *clump = *reinterpret_cast<RpClump **>(m_part);
        fn_8006CAE4(list, &clump);
        if (m_hand) {
            m_hand->AddClumps(list);
        }
    }
}

} // namespace Sonic
} // namespace Npc
} // namespace Player

/* The NPC pointer's out-of-line operator-> (see Game/NpcPartModel.h). */
extern "C" Player::Npc::Npc *fn_8006CA70(const boost::shared_ptr<Player::Npc::Npc> *npc) {
    return npc->get();
}

namespace Player {
namespace Npc {
namespace Sonic {

void MotionImpl::Run(float dt) {
    fn_8006127C(&m_flags, 2);
    UpdateVisibility();
    if (m_npc.get()->IsWeaponHidden()) {
        return;
    }
    if (m_work) {
        Vec3f offset;
        fn_800091EC(&offset, &Ref(fn_8007378C(m_npc.get())));
        if (m_before) {
            (this->*m_before)();
        }
        PlaceOnNpc(offset);
        fn_80415EA4(m_work);
        fn_80014E1C(&m_flags, 2);
        if (fn_80073908(m_npc.get()) < 1.0f) {
            Mtx34f matrix;
            fn_8040C148(&matrix, &RefM(fn_80073844(fn_8006CA70(&m_npc))));
            if (m_part) {
                fn_80015954(m_part, &matrix);
            }
        } else {
            if (m_part) {
                fn_800159AC(m_part);
            }
        }
        fn_800A9EA0(this);
        MoveByRoot(offset);
        if (m_after) {
            (this->*m_after)();
        }
    }
    fn_80079860(m_npc.get());
    if (!m_npc.get()->IsWeaponHidden()) {
        fn_803563D0(reinterpret_cast<UnkNpcMotionFields_801FD130 *>(m_npc.get())->m_shadow, 4.0f);
    }
}

void MotionImpl::StartMotion() {
    switch (m_step) {
    case 0:
        if (m_playingMotion == m_nextMotion) {
            fn_800A9F9C(this, 0.0f);
        }
        m_step++;
        break;
    }
}

void MotionImpl::WaitForPlayer() {
    switch (m_step) {
    case 1:
        const MotionEntry *entry = FindMotion(m_motionId);
        if (entry->m_nextIndex >= 0) {
            if (fn_800A9E1C(m_npc)) {
                m_nextMotion = entry->m_nextIndex + (m_motionBase - 1);
                m_nextMotionArg = -1;
                m_step++;
            }
        } else {
            m_step++;
        }
        break;
    }
}

void MotionImpl::SelectRunMotion() {
    float speed = RunSpeed();
    int a;
    int b;
    int step;
    if (speed < 1.0f) {
        RunBlend() = speed;
        a = s_runMotions[0][0];
        step = 1;
        m_speed = 1.0f;
        b = s_runMotions[0][1];
    } else if (speed < 2.0f) {
        RunBlend() = speed - 1.0f;
        step = 2;
        a = s_runMotions[1][0];
        m_speed = 1.0f;
        b = s_runMotions[1][1];
    } else if (speed < 3.0f) {
        RunBlend() = speed - 2.0f;
        step = 3;
        a = s_runMotions[2][0];
        m_speed = 1.0f;
        b = s_runMotions[2][1];
    } else {
        RunBlend() = speed - 3.0f;
        a = s_runMotions[3][0];
        b = s_runMotions[3][1];
        RunBlend() = RunBlend() < 1.0f ? RunBlend() : 1.0f;
        step = 4;
        m_speed = 1.0f;
    }
    m_nextMotion = a + (m_motionBase - 1);
    m_nextMotionArg = b + (m_motionBase - 1);
    if (m_step != step) {
        if (m_step > 0) {
            m_blendTime = 0.0f;
            float frame = fn_800AA064(this);
            fn_800AA3A4(this);
            fn_800A9F9C(this, frame);
        }
        m_step = step;
    }
}

void MotionImpl::TurnToPlayer() {
    switch (m_step) {
    case 0: {
        Vec3f direction;
        UnkPlayerFields_8006C550 *player = reinterpret_cast<UnkPlayerFields_8006C550 *>(m_player.get());
        fn_8040F990(0, &direction, &player->m_direction, &player->m_up);
        fn_8040CE74(&direction);
        switch (fn_80093C60(&m_player, &direction)) {
        case 0:
        case 3:
            m_nextMotion = m_motionBase + 0x14;
            break;
        case 1:
            m_nextMotion = m_motionBase + 0x1C;
            break;
        case 2:
            m_nextMotion = m_motionBase + 0x1B;
            break;
        }
        m_nextMotionArg = -1;
        m_step++;
        break;
    }
    case 1:
        if (fn_800A9E1C(m_player)) {
            m_nextMotion = m_motionBase + 0x16;
            m_blendTime = 10.0f;
            m_step++;
        }
        break;
    }
}

void MotionImpl::FinishWithPlayer() {
    switch (m_step) {
    case 0:
        if (m_playingMotion == m_nextMotion && fn_800A9E1C(m_npc)) {
            fn_8006127C(&reinterpret_cast<UnkNpcMotionFields_801FD130 *>(m_npc.get())->m_motionFlags, 3);
            m_nextMotion = m_motionBase + 0x15;
            m_nextMotionArg = -1;
            m_blendTime = 0.0f;
            m_speed = 1.0f;
            m_step++;
        }
        break;
    }
}

void MotionImpl::WaitForLanding() {
    switch (m_step) {
    case 0:
        if (fn_800A9E1C(m_npc)) {
            fn_8006127C(&reinterpret_cast<UnkNpcMotionFields_801FD130 *>(m_npc.get())->m_motionFlags, 3);
            m_nextMotion = m_motionBase + 8;
            m_step++;
        }
        break;
    case 1:
        fn_8006127C(&reinterpret_cast<UnkNpcMotionFields_801FD130 *>(m_npc.get())->m_motionFlags, 3);
        if (fn_8006CD78(m_npc.get()->m_stateBits, 2)) {
            m_nextMotion = m_motionBase + 9;
            m_step++;
        }
        break;
    }
}

void MotionImpl::Show() {
    if (fn_80014E34(&m_flags, 3)) {
        fn_8006127C(&m_flags, 3);
        if (m_part) {
            fn_80038F94(*reinterpret_cast<RpClump **>(m_part));
        }
        if (m_hand) {
            m_hand->Show();
        }
    }
}

void MotionImpl::Hide() {
    if (!fn_80014E34(&m_flags, 3)) {
        fn_80014E1C(&m_flags, 3);
        if (m_part) {
            fn_80038FC0(*reinterpret_cast<RpClump **>(m_part));
        }
        if (m_hand) {
            m_hand->Hide();
        }
    }
}

void MotionImpl::Load() {
    void *loader = fn_800122D4();
    void *model = fn_80078C4C(m_npc.get());
    void *data = fn_80012684(loader, model, 8);
    m_motionBase = fn_80012608(loader, model, 9);
    m_work = fn_8041B908(data, 2);
    m_part = CreatePart(m_npc, m_work, model, 2);
    m_hand = new NpcHand(this, m_npc);
    m_hand->Load(model, s_hands, 2, fn_800AA584(this));
    BindNodes(s_nodes);
}

NpcMotion *MotionImpl::Create(Task *parent, const boost::shared_ptr<Npc> &npc) {
    return new MotionImpl(parent, npc);
}

} // namespace Sonic
} // namespace Npc

/* PlayerMotion's empty virtual functions. */
float PlayerMotion::vf_0x30() {
    return 0.0f;
}

void PlayerMotion::vf_0x2C() {}

#ifdef PLAYER_MOTION_HAND_MATRIX
#else
void PlayerMotion::vf_0x24() {}
#endif

void PlayerMotion::SetIKCallback(int slot, const IKCallback &callback) {}

void PlayerMotion::vf_0x20(int arg) {}

void PlayerMotion::vf_0x1C() {}

void PlayerMotion::SetWeaponPose(int pose) {}

} // namespace Player
