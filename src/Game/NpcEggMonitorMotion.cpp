/* Player::Npc::EggMonitor::MotionImpl (class name from RTTI; bases Task, Player::PlayerMotion and
 * Player::Npc::NpcMotion): the motion of the Egg Monitor NPC. Once the NPC's model resource is
 * ready it loads the motion and five model parts, then each frame places the motion on the NPC,
 * keeps the parts on it and moves the NPC by the motion's root. Each motion of a small table brings
 * two step callbacks that start the motion's animation. The file name and all function and member
 * names are inferred. Definitions are in reverse address order (deferred inlining emits functions
 * last-defined first). */
#include "Game/CharaColliBaseCharInfo.h"
#include "Game/DevilDoomCtrlUnit.h"
#include "Game/MathTypes.h"
#include "Game/Npc.h"
#include "Game/NpcMotion.h"
#include "Game/NpcPartModel.h"
#include "Game/PlayerWeaponBase.h"
#include "boost/shared_ptr.hpp"
#include <dolphin/types.h>
#include "size_assert.h"

extern "C" {
/* PlayerMotion's own functions (behaviour inferred). */
void fn_800A9F9C(Player::PlayerMotion *motion, float frame);
/* Whether the player may start a motion (behaviour inferred). */
bool fn_800A9E1C(boost::shared_ptr<Player::PlayerBase> player);

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

/* The parts: placed on a matrix, or on the motion; shown and hidden; animated (behaviour
 * inferred). */
void fn_80015954(UnkClumpUpdater_80015A7C *part, const Mtx34f *matrix);
void fn_800159AC(UnkClumpUpdater_80015A7C *part);
void fn_80038F94(RpClump *clump);
void fn_80038FC0(RpClump *clump);
void fn_80039B94(RpClump *clump, float dt, float arg2);
void fn_8006CAE4(Player::UnkClumpList_8006CAE4 *list, RpClump *const *clump);

/* The NPC: the offset of its model, an update and its shadow (behaviour inferred). */
Vec3f fn_8007378C(Player::Npc::Npc *npc);
void fn_80079860(Player::Npc::Npc *npc);
void fn_803563D0(void *shadow, float arg1);
}

/* Passes a returned value on by address (as in Game/ArkCannon.cpp's casts). */
inline const Vec3f &Ref(const Vec3f &vector) {
    return vector;
}

inline const Mtx34f &RefM(const Mtx34f &matrix) {
    return matrix;
}

namespace Player {
namespace Npc {
namespace EggMonitor {

class MotionImpl;

/* What a motion runs before and after the motion is placed each frame (struct name unknown). */
struct MotionSteps {
    void (MotionImpl::*m_before)();
    void (MotionImpl::*m_after)();
};

/* A motion of the table (struct name unknown): the motion's number, its index in the model's
 * motions, the index it switches to once the player allows (-1: none) and its steps. */
struct MotionEntry {
    /* 0x00 */ s32 m_motion;
    /* 0x04 */ s32 m_index;
    /* 0x08 */ s32 m_nextIndex;
    /* 0x0C */ s32 field_0xC;
    /* 0x10 */ const MotionSteps *m_steps;
};

class MotionImpl : public NpcMotion {
  public:
    MotionImpl(Task *parent, const boost::shared_ptr<Npc> &npc) : NpcMotion(parent, npc), m_state(0) {
        for (int i = 0; i < 5; i++) {
            m_parts[i] = 0;
        }
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

    /* Destroys the parts (name inferred). */
    void ReleaseParts() {
        for (int i = 0; i < 5; i++) {
            DestroyPart(m_parts[i]);
            m_parts[i] = 0;
        }
    }

    void Run(float dt);
    void StartMotion();
    void WaitForPlayer();

    /* 0x460 */ s32 m_state; /* 0: waiting for the model resource, 1: running */
    /* 0x464 */ void (MotionImpl::*m_before)();
    /* 0x470 */ void (MotionImpl::*m_after)();
    /* 0x47C */ UnkClumpUpdater_80015A7C *m_parts[5];
};
SIZE_ASSERT(MotionImpl, 0x490);

/* The model's parts and the skeleton nodes (all the motion's own). */
static const int s_parts[5] = {5, 6, 7, 8, 9};
static const int s_nodes[14] = {-1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1};

static MotionSteps s_steps = {&MotionImpl::StartMotion, &MotionImpl::WaitForPlayer};

static const MotionEntry s_motions[] = {
    {0, 1, -1, 0, &s_steps},
    {1, 1, -1, 0, &s_steps},
    {0x2E, 2, -1, 0, &s_steps},
    {0x30, 0, 0, 0, 0},
};

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
    ReleaseParts();
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
    }
}

void MotionImpl::vf_0x14(UnkClumpList_8006CAE4 *list) {
    if (fn_80014E34(&m_flags, 2)) {
        for (int i = 0; i < 5; i++) {
            if (m_parts[i]) {
                RpClump *clump = *reinterpret_cast<RpClump **>(m_parts[i]);
                if (clump) {
                    fn_8006CAE4(list, &clump);
                }
            }
        }
    }
}

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
            for (int i = 0; i < 5; i++) {
                if (m_parts[i]) {
                    fn_80015954(m_parts[i], &matrix);
                }
            }
        } else {
            for (int i = 0; i < 5; i++) {
                if (m_parts[i]) {
                    fn_800159AC(m_parts[i]);
                }
            }
        }
        MoveByRoot(offset);
        if (m_after) {
            (this->*m_after)();
        }
    }
    fn_80079860(m_npc.get());
    if (!m_npc.get()->IsWeaponHidden()) {
        fn_803563D0(reinterpret_cast<UnkNpcMotionFields_801FD130 *>(m_npc.get())->m_shadow, 2.0f);
    }
    for (int i = 0; i < 5; i++) {
        if (m_parts[i] && *reinterpret_cast<RpClump **>(m_parts[i])) {
            fn_80039B94(*reinterpret_cast<RpClump **>(m_parts[i]), dt, -1.0f);
        }
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

void MotionImpl::Show() {
    if (fn_80014E34(&m_flags, 3)) {
        fn_8006127C(&m_flags, 3);
        for (int i = 0; i < 5; i++) {
            if (m_parts[i]) {
                fn_80038F94(*reinterpret_cast<RpClump **>(m_parts[i]));
            }
        }
    }
}

void MotionImpl::Hide() {
    if (!fn_80014E34(&m_flags, 3)) {
        fn_80014E1C(&m_flags, 3);
        for (int i = 0; i < 5; i++) {
            if (m_parts[i]) {
                fn_80038FC0(*reinterpret_cast<RpClump **>(m_parts[i]));
            }
        }
    }
}

void MotionImpl::Load() {
    void *loader = fn_800122D4();
    void *model = fn_80078C4C(m_npc.get());
    void *data = fn_80012684(loader, model, 0xB);
    m_motionBase = fn_80012608(loader, model, 0xC);
    m_work = fn_8041B908(data, 2);
    for (int i = 0; i < 5; i++) {
        m_parts[i] = CreatePart(m_npc, m_work, model, s_parts[i]);
    }
    BindNodes(s_nodes);
}

NpcMotion *MotionImpl::Create(Task *parent, const boost::shared_ptr<Npc> &npc) {
    return new MotionImpl(parent, npc);
}

} // namespace EggMonitor
} // namespace Npc
} // namespace Player
