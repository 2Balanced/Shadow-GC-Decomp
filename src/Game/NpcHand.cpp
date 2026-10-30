/* Player::Npc::NpcHand (class name from RTTI; base Task): the hand models of a non-player
 * character. It keeps a list of hand pairs (one left and one right model per pose), shows the
 * pair of the current pose in the NPC's stage area, and each frame puts the two models on the
 * hand nodes of the NPC's motion. The work is done by an object it owns (class name unknown).
 * File name from the RTTI name; function, member and helper class names are inferred.
 * Definitions are in reverse address order (deferred inlining emits functions last-defined
 * first). */
#define NPCHAND_IMPLEMENTATION
#define MSL_VECTOR_MIDDLE
#define MSL_VECTOR_INDEX
#define MSL_VECTOR_ERASE_ONE
#define MSL_ALGORITHM_COPY
#define MSL_MEMORY_DESTROY_OUTLINE
#include <vector>
#include "Game/DebugDispShape.h"
#include "Game/MathConstants.h"
#include "Game/MathTypes.h"
#include "Game/Npc.h"
#include "Game/NpcHand.h"
#include "Game/PlayerWeaponBase.h"
#include "Game/Task.h"
#include "boost/scoped_ptr.hpp"
#include "boost/shared_ptr.hpp"
#include <dolphin/types.h>
#include "size_assert.h"

using boost::shared_ptr;
using Player::PlayerMotion;
using Player::UnkClumpList_8006CAE4;
using Player::UnkMotionNode_800AA760;
using Player::Npc::Npc;

/* The world of each stage area (as in Game/BlackDoomCtrlUnit.cpp). */
struct UnkWorldEntry_8051D208 {
    /* 0x00 */ u8 field_0x0[0x10];
    /* 0x10 */ void *m_world;
    /* 0x14 */ u8 field_0x14[0x18 - 0x14];
};
extern UnkWorldEntry_8051D208 lbl_8051D208[];

/* The skeleton a model is bound to (see Game/BlackDoomCtrlUnit.h). */
struct UnkClumpFade_80027D5C;

extern "C" {
/* The world a clump is in, and taking it out of or putting it into a world (RenderWare). */
void *fn_8046256C(RpClump *clump);
void fn_804623EC(void *world, RpClump *clump);
void fn_804622FC(void *world, RpClump *clump);
/* The stage area of a clump's frame, and making it current (see Game/BlackDoomCtrlUnit.cpp). */
void *fn_80169364(RpClump *clump);
int fn_80169350(void *frame);
void fn_8003A840(int area);
/* The resource loader, a model of a resource, and binding a model to a skeleton. */
void *fn_800122D4(void);
RpClump *fn_8001277C(void *loader, void *model, int part);
void fn_80038198(RpClump *clump, void *skeleton);
/* A node of a motion's skeleton. */
UnkMotionNode_800AA760 *fn_800AA760(PlayerMotion *motion, int index);
/* The NPC pointer's out-of-line operator-> (Game/NpcSonicMotion.cpp). */
Npc *fn_8006CA70(const shared_ptr<Npc> *npc);
/* The NPC's appearance ratio (1: fully there) and its model matrix (behaviour inferred). */
float fn_80073908(Npc *npc);
Mtx34f fn_80073844(Npc *npc);
/* Hands a clump to a clump list (Game/NpcSonicMotion.cpp). */
void fn_8006CAE4(UnkClumpList_8006CAE4 *list, RpClump *const *clump);
/* Matrices (see Game/MathTypes.h). */
Mtx34f fn_8001E044(const Mtx34f *a, const Mtx34f *b);
}

/* Passes a returned matrix on by address. */
inline const Mtx34f &RefM(const Mtx34f &matrix) {
    return matrix;
}

/* Degrees to radians. */
inline float DegToRad(float degrees) {
    return degrees * (lbl_805FAD38 / 180.0f);
}

/* The player members used here (names inferred). */
struct UnkNpcFields_80070F28 {
    /* 0x000 */ u8 field_0x0[0x230];
    /* 0x230 */ PlayerMotion *m_motion;
};

/* Puts clump into world, out of the world it is in (as in Game/BlackDoomCtrlUnit.cpp). */
inline void SetClumpWorld(RpClump *clump, void *world) {
    void *current = fn_8046256C(clump);
    if (current != world) {
        if (current != 0) {
            fn_804623EC(current, clump);
        }
        fn_804622FC(world, clump);
    }
}

/* A pair of hand models (struct name unknown): the left and the right hand, and whether they are
 * in the world. */
struct UnkHandPair_80071620 {
    UnkHandPair_80071620() {
        m_clumps[0] = 0;
        m_clumps[1] = 0;
        m_shown = false;
    }
    ~UnkHandPair_80071620();
    /* Puts the models into the world of their stage area. */
    void Show();
    /* Takes the models out of their world. */
    void Hide() {
        if (m_shown) {
            for (int i = 0; i < 2; i++) {
                void *world;
                if (m_clumps[i] && (world = fn_8046256C(m_clumps[i])) != 0) {
                    fn_804623EC(world, m_clumps[i]);
                }
            }
            m_shown = false;
        }
    }

    /* 0x0 */ RpClump *m_clumps[2];
    /* 0x8 */ bool m_shown;
};
SIZE_ASSERT(UnkHandPair_80071620, 0xC);

namespace Player {
namespace Npc {

/* What NpcHand does its work with (class name unknown): the NPC, the hand pairs, one per pose,
 * and the current pose. */
class UnkNpcHandImpl_80071508 {
  public:
    UnkNpcHandImpl_80071508(const shared_ptr<Npc> &npc);
    ~UnkNpcHandImpl_80071508();
    void Tick();
    void AddClumps(UnkClumpList_8006CAE4 *list);
    void SetPose(int pose);
    void Load(void *model, const int *parts, int count, void *skeleton);
    void Show();
    void Hide();
    /* Puts the current pair on the hand nodes of the NPC's motion. */
    void Update();
    /* Drops every pair. */
    void Clear();

    /* 0x00 */ shared_ptr<Npc> m_npc;
    /* 0x08 */ std::vector<UnkHandPair_80071620> m_hands;
    /* 0x14 */ s32 m_pose;
};

} // namespace Npc
} // namespace Player

using Player::Npc::NpcHand;
using Player::Npc::UnkNpcHandImpl_80071508;

SIZE_ASSERT(UnkNpcHandImpl_80071508, 0x18);

/* Deletes the object NpcHand works with (name inferred; see checked_delete below). */
inline void DeleteImpl(UnkNpcHandImpl_80071508 *impl) {
    delete impl;
}

/* Retail keeps boost::checked_delete of it out of line (0x80070DCC); GC/2.7 only does when its body
 * calls an inline function (see docs/research-notes.md, EnergyCore). */
namespace boost {
template <> inline void checked_delete<UnkNpcHandImpl_80071508>(UnkNpcHandImpl_80071508 *x) {
    DeleteImpl(x);
}
} // namespace boost

UnkHandPair_80071620::~UnkHandPair_80071620() {
    Hide();
}

void UnkHandPair_80071620::Show() {
    if (!m_shown) {
        for (int i = 0; i < 2; i++) {
            if (m_clumps[i]) {
                int area = fn_80169350(fn_80169364(m_clumps[i]));
                fn_8003A840(area);
                if (lbl_8051D208[area].m_world) {
                    void *world = lbl_8051D208[area].m_world;
                    SetClumpWorld(m_clumps[i], world);
                }
            }
        }
        m_shown = true;
    }
}

namespace Player {
namespace Npc {

UnkNpcHandImpl_80071508::UnkNpcHandImpl_80071508(const shared_ptr<Npc> &npc) : m_npc(npc) {
    m_pose = 0;
}

UnkNpcHandImpl_80071508::~UnkNpcHandImpl_80071508() {
    Clear();
}

void UnkNpcHandImpl_80071508::Tick() {
    Update();
}

void UnkNpcHandImpl_80071508::AddClumps(UnkClumpList_8006CAE4 *list) {
    for (int i = 0; i < 2; i++) {
        fn_8006CAE4(list, &m_hands[m_pose].m_clumps[i]);
    }
}

void UnkNpcHandImpl_80071508::SetPose(int pose) {
    if (pose < (int)m_hands.size()) {
        m_hands[m_pose].Hide();
        m_pose = pose;
        m_hands[m_pose].Show();
        Update();
    }
}

void UnkNpcHandImpl_80071508::Load(void *model, const int *parts, int count, void *skeleton) {
    Clear();
    m_hands.reserve(count);
    void *loader = fn_800122D4();
    for (int i = 0; i < count; i++) {
        UnkHandPair_80071620 hand;
        hand.m_clumps[0] = fn_8001277C(loader, model, parts[i * 2]);
        hand.m_clumps[1] = fn_8001277C(loader, model, parts[i * 2 + 1]);
        fn_80038198(hand.m_clumps[0], skeleton);
        fn_80038198(hand.m_clumps[1], skeleton);
        m_hands.push_back(hand);
    }
}

void UnkNpcHandImpl_80071508::Show() {
    m_hands[m_pose].Show();
}

void UnkNpcHandImpl_80071508::Hide() {
    m_hands[m_pose].Hide();
}

void UnkNpcHandImpl_80071508::Update() {
    PlayerMotion *motion = reinterpret_cast<UnkNpcFields_80070F28 *>(m_npc.get())->m_motion;
    if (motion) {
        static float s_angles[2] = {DegToRad(180.0f), DegToRad(90.0f)};
        UnkMotionNode_800AA760 *nodes[2];
        nodes[0] = fn_800AA760(motion, 5);
        nodes[1] = fn_800AA760(motion, 3);
        Mtx34f base;
        if (fn_80073908(m_npc.get()) < 1.0f) {
            fn_8040C148(&base, &RefM(fn_80073844(fn_8006CA70(&m_npc))));
        } else {
            fn_8040B6F8(&base);
        }
        for (int i = 0; i < 2; i++) {
            RwFrame *frame = m_hands[m_pose].m_clumps[i]->frame;
            Mtx34f matrix;
            fn_8040C148(&matrix, nodes[i]->m_matrix);
            fn_8040B80C(&matrix, s_angles[i]);
            fn_8040C148(&matrix, &RefM(fn_8001E044(&base, &matrix)));
            frame->modelling = fn_800591F0(&matrix);
            fn_8048722C(frame);
        }
    }
}

void UnkNpcHandImpl_80071508::Clear() {
    while (m_hands.size() != 0) {
        UnkHandPair_80071620 *hand = m_hands.begin();
        hand->Hide();
        m_hands.erase(hand);
    }
}

NpcHand::NpcHand(Task *parent, const shared_ptr<Npc> &npc)
    : Task(parent), m_impl(new UnkNpcHandImpl_80071508(npc)) {
    m_name = "NpcHand";
}

NpcHand::~NpcHand() {}

void NpcHand::Tick(float dt) {
    m_impl->Tick();
}

void NpcHand::AddClumps(UnkClumpList_8006CAE4 *list) {
    m_impl->AddClumps(list);
}

void NpcHand::SetPose(int pose) {
    m_impl->SetPose(pose);
}

void NpcHand::Load(void *model, const int *parts, int count, void *skeleton) {
    m_impl->Load(model, parts, count, skeleton);
}

void NpcHand::Show() {
    m_impl->Show();
}

void NpcHand::Hide() {
    m_impl->Hide();
}

} // namespace Npc
} // namespace Player
