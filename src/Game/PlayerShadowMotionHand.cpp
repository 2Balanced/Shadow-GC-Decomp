/* Player::Shadow::MotionHand (class name from RTTI): the task that shows Shadow's hand models
 * (SHADOW_HAND_*.DFF) over his own hands while he holds a weapon. Its implementation (class name
 * unknown) loads the hand models of the motion files (Game/PlayerShadowMotionFiles.h) with an
 * animation each, keeps four slots of a left and a right hand, follows the player's hand nodes
 * and plays the hands' poses. ShadowMotion (Game/PlayerShadowMotion.cpp) calls the task's
 * functions. File name from the class; other names are inferred. Definitions are in reverse
 * address order (deferred inlining emits functions last-defined first).
 *
 * NonMatching (33/34). Wave 41 matched five: GetNodeMatrix (the helper's node parameter unsigned,
 * which puts the copy of node after the counter's copy, as retail has it), Update (copy propagation
 * off: the motion is loaded into r3 and copied after the null test), SetSlot (the same, and the
 * two zero times assigned one by one, which stores them as floats), SetMotion (both motions read
 * into u16 locals first, right before left, and the loop in the inline RestartHands, which copies
 * the zeroed counter) and Hand::Attach (the clump put in its world through the inline
 * SetClumpWorld, whose parameters take the first saved registers). Load (10 rows) has retail's
 * code with the second loop's hand set up through the inline SetupHand (which reads the
 * animation before the clump) and the counter declared first; GC/2.7 still gives the loader r31
 * and the first loop's address r29 where retail has r29 and r23
 * (probes/Game_PlayerShadowMotionHand_W41d.py, _W41f.py and _W41g.py: declaration orders, a shared
 * counter, block scopes, the loops in inline helpers, which GC/2.7 does not inline, and copy
 * propagation off). UpdatePose matches since wave 40: the hand's motion id is narrowed to u16 by a
 * cast with the base, less one, added first (`m_motionBase - 1 + motion`). */
#define PLAYER_MOTION_MEMBERS
#include "boost/scoped_ptr.hpp"
#include "Game/DebugDispShape.h"
#include "Game/DevilDoomCtrlUnit.h"
#include "Game/MathTypes.h"
#include "Game/Player.h"
#include "Game/PlayerShadowMotionFiles.h"
#include "Game/PlayerWeaponBase.h"
#include "Game/Task.h"
#include "boost/shared_ptr.hpp"
#include "size_assert.h"

using boost::shared_ptr;
using Player::PlayerBase;

struct RpClump;
struct _stMtnPlayWork;

extern "C" {
/* The model loader and its files (behaviour inferred; see Game/NpcMotion.cpp). */
void *fn_800122D4(void);
int fn_80012608(void *loader, s32 file, s32 index);
RpClump *fn_8001277C(void *loader, s32 file, s32 index);
s32 fn_80078C4C(PlayerBase *player);
/* The id of a bone file name, and the animation of an id (behaviour inferred). */
u32 fn_80410630(const char *name);
_stMtnPlayWork *fn_8041B908(u32 id, int arg1);
void fn_8041B884(_stMtnPlayWork *work);
/* Clump helpers: clone, destroy, the world it is in, remove from and add to a world, and the
 * colour fade of the player's motion (see Game/NpcMotion.cpp). */
RpClump *fn_80457090(RpClump *clump);
void fn_80457468(RpClump *clump);
void *fn_8046256C(RpClump *clump);
void fn_804623EC(void *world, RpClump *clump);
void fn_804622FC(void *world, RpClump *clump);
UnkClumpFade_80027D5C *fn_800AA584(Player::PlayerMotion *motion);
void fn_80038198(RpClump *clump, UnkClumpFade_80027D5C *fade);
/* The world of the clump's area (behaviour inferred). */
void *fn_80169364(RpClump *clump);
int fn_80169350(void *frame);
void fn_8003A840(int area);
/* The motion's node of an index (Game/PlayerMotion.cpp). */
Player::UnkMotionNode_800AA760 *fn_800AA760(Player::PlayerMotion *motion, int index);
void fn_8006CAE4(Player::UnkClumpList_8006CAE4 *list, RpClump *const *clump);
/* Motion work helpers (see Game/NpcMotion.cpp). */
void fn_804165D8(_stMtnPlayWork *work, int arg1, int arg2, int arg3);
void fn_80416148(_stMtnPlayWork *work, int arg1, float arg2);
void fn_80417568(_stMtnPlayWork *work, u16 motion);
int fn_80416F28(_stMtnPlayWork *work, int arg1, int arg2, int arg3);
Mtx34f *fn_80415E70(_stMtnPlayWork *work, int node);
void fn_80416E20(_stMtnPlayWork *work, int arg1, float frame);
void fn_80415E34(_stMtnPlayWork *work, Vec3f *position);
void fn_80415BDC(_stMtnPlayWork *work, const Vec3f *delta);
void fn_800163F4(void *node, const Mtx34f *matrix);
void fn_8041734C(_stMtnPlayWork *work, const void *node);
void fn_80416DC8(_stMtnPlayWork *work, int arg1, float dt);
void fn_80415EA4(_stMtnPlayWork *work);
void fn_800159AC(UnkClumpUpdater_80015A7C *updater);
}

/* The areas' worlds (struct unknown; the world at 0x10 of each 0x18-byte entry). */
struct UnkArea_8051D208 {
    /* 0x00 */ u8 field_0x0[0x10];
    /* 0x10 */ void *m_world;
    /* 0x14 */ u8 field_0x14[0x18 - 0x14];
};
extern UnkArea_8051D208 lbl_8051D208[];
extern const float lbl_805FAD38; /* pi */

/* A node of a motion as fn_800163F4 fills it (struct name unknown). */
struct UnkMotionNodeMtx_800163F4 {
    /* 0x00 */ u8 field_0x0[0x30];
};

/* The hand models and the bone file each one's animation reads (-1: none). */
struct UnkHandModel_804B36A0 {
    const char *m_name;
    s32 m_bone;
};

static const UnkHandModel_804B36A0 s_handModels[5] = {
    {"SHADOW_HAND_L.DFF", 0}, {"SHADOW_HAND_R.DFF", 1}, {"SHADOW_HAND_GUN.DFF", -1},
    {"SHADOW_HAND_SWORD.DFF", -1}, {"SHADOW_HAND_BAR.DFF", -1},
};

/* The hand poses: the motion of each hand (-1: none) and the pose that follows. */
struct UnkHandPose_804B36C8 {
    s32 m_motion[2];
    s32 m_next;
};

static const UnkHandPose_804B36C8 s_poses[15] = {
    {{-1, -1}, 0},   {{0x10, 0x22}, 1}, {{0x11, 0x23}, 2}, {{0x12, 0x24}, 2},  {{0x04, 0x16}, 0},
    {{0x0C, 0x1E}, 5}, {{0x05, 0x17}, 5}, {{0x0A, 0x1C}, 7}, {{0x06, 0x18}, 7}, {{0x0E, 0x20}, 9},
    {{0x0F, 0x21}, 9}, {{0x01, 0x13}, 0}, {{0x02, 0x14}, 0xD}, {{0x03, 0x15}, 0xD},
    {{0x12, 0x24}, 0xE},
};

/* The hand models of the four slots' left and right hands. */
static const s32 s_slotModels[4][2] = {{0, 1}, {2, 1}, {3, 1}, {4, 1}};

/* What MotionHand's slot 0x14 gets (struct name unknown): the motions of the two hands. */
struct UnkHandMotion_800B07FC {
    /* 0x0 */ u32 field_0x0;
    /* 0x4 */ s32 m_left;
    /* 0x8 */ s32 m_right;
};

namespace Player {
namespace Shadow {

/* MotionHand's implementation (class name unknown). */
class MotionHandImpl {
  public:
    /* A hand: its clump, its animation and what keeps the clump in step (names inferred). */
    struct Hand {
        Hand() : m_updater(0), m_clump(0), m_anim(0) {}
        ~Hand() {}

        void Release();
        bool Update(int index, const UnkMotionNode_800AA760 *node, const float *time);
        void AddTo(UnkClumpList_8006CAE4 *list);
        void Attach();
        void Detach() {
            if (m_clump != 0) {
                void *world = fn_8046256C(m_clump);
                if (world != 0) {
                    fn_804623EC(world, m_clump);
                }
            }
        }

        /* 0x0 */ UnkClumpUpdater_80015A7C *m_updater;
        /* 0x4 */ RpClump *m_clump;
        /* 0x8 */ _stMtnPlayWork *m_anim;
    };

    MotionHandImpl(const shared_ptr<PlayerBase> &player, const UnkMotionFiles_800ADE78 &files);
    ~MotionHandImpl();

    void Update();
    void AddTo(UnkClumpList_8006CAE4 *list);
    void SetSlot(int slot);
    void SetMotion(const UnkHandMotion_800B07FC *motion);
    void Show();
    void Hide();
    void UpdatePose();
    void Load();
    void Unload();

    void GetAnims(_stMtnPlayWork **left, _stMtnPlayWork **right) {
        *left = m_anims[0];
        *right = m_anims[1];
    }
    void SetTime(float time) {
        m_time = time;
    }
    void SetPose(int pose, float blend) {
        if (m_request != pose) {
            m_request = pose;
            m_pose = pose;
            m_blend = blend;
        }
    }

    /* 0x000 */ shared_ptr<PlayerBase> m_player;
    /* 0x008 */ s32 m_pose;
    /* 0x00C */ s32 m_request;
    /* 0x010 */ s32 m_shownPose;
    /* 0x014 */ float m_time;
    /* 0x018 */ float m_blend;
    /* 0x01C */ s32 m_motionBase;
    /* 0x020 */ UnkMotionFiles_800ADE78 m_files;
    /* 0x098 */ Hand *m_current;
    /* 0x09C */ Hand m_hands[4][2];
    /* 0x0FC */ RpClump *m_clumps[5];
    /* 0x110 */ _stMtnPlayWork *m_anims[5];
};
SIZE_ASSERT(MotionHandImpl, 0x124);

/* Deletes an implementation (name inferred; see checked_delete below). */
inline void DeleteImpl(MotionHandImpl *impl) {
    delete impl;
}

} // namespace Shadow
} // namespace Player

/* Retail keeps boost::checked_delete<MotionHandImpl> out of line (0x800B02C0); this specialization
 * calls an inline helper, which gives retail's code and order (see docs/research-notes.md and
 * Game/Rocket.cpp). */
namespace boost {
template<> inline void checked_delete<Player::Shadow::MotionHandImpl>(Player::Shadow::MotionHandImpl *x) {
    Player::Shadow::DeleteImpl(x);
}
} // namespace boost

namespace Player {
namespace Shadow {

void MotionHandImpl::Hand::Release() {
    if (m_clump != 0) {
        void *world = fn_8046256C(m_clump);
        if (world != 0) {
            fn_804623EC(world, m_clump);
        }
    }
    m_clump = 0;
    m_anim = 0;
    if (m_updater != 0) {
        delete m_updater;
        m_updater = 0;
    }
}

bool MotionHandImpl::Hand::Update(int index, const UnkMotionNode_800AA760 *node, const float *time) {
    if (m_updater == 0) {
        if (m_clump != 0) {
            RwFrame *frame = m_clump->frame;
            Mtx34f matrix;
            fn_8040C148(&matrix, node->m_matrix);
            fn_8040B80C(&matrix, 180.0f * (lbl_805FAD38 / 180.0f));
            frame->modelling = fn_800591F0(&matrix);
            fn_8048722C(frame);
        }
    } else if (node->m_matrix != 0) {
        Mtx34f matrix;
        fn_8040C148(&matrix, node->m_matrix);
        Vec3f position;
        fn_80415E34(m_anim, &position);
        Vec3f delta;
        fn_8040CD74(&delta, &node->m_position, &position);
        fn_800091D0(&position, &delta);
        fn_80415BDC(m_anim, &position);
        UnkMotionNodeMtx_800163F4 nodeMatrix;
        fn_800163F4(&nodeMatrix, &matrix);
        fn_8041734C(m_anim, &nodeMatrix);
        fn_80416DC8(m_anim, 0, time[1]);
        fn_80415EA4(m_anim);
        fn_800159AC(m_updater);
        if (fn_80416F28(m_anim, 0, 3, 1) != 0) {
            return true;
        }
    }
    return false;
}

void MotionHandImpl::Hand::AddTo(UnkClumpList_8006CAE4 *list) {
    if (m_clump != 0) {
        fn_8006CAE4(list, &m_clump);
    }
}

/* Puts clump in world (name inferred; as in Game/BossCtrlUnit.cpp). */
inline void SetClumpWorld(RpClump *clump, void *world) {
    void *current = fn_8046256C(clump);
    if (current != world) {
        if (current != 0) {
            fn_804623EC(current, clump);
        }
        fn_804622FC(world, clump);
    }
}

void MotionHandImpl::Hand::Attach() {
    if (m_clump != 0) {
        int area = fn_80169350(fn_80169364(m_clump));
        fn_8003A840(area);
        void *world = lbl_8051D208[area].m_world;
        if (world != 0) {
            SetClumpWorld(m_clump, world);
        }
    }
}

MotionHandImpl::MotionHandImpl(const shared_ptr<PlayerBase> &player, const UnkMotionFiles_800ADE78 &files)
    : m_player(player), m_time(-1.0f), m_blend(0.0f), m_motionBase(0), m_files(files), m_current(0) {
    for (int i = 0; i < 5; i++) {
        m_anims[i] = 0;
        m_clumps[i] = 0;
    }
    Load();
    SetSlot(0);
    m_request = 0xF;
    m_shownPose = 0xF;
    SetPose(0, 0.0f);
}

MotionHandImpl::~MotionHandImpl() {
    Unload();
}

/* Copy propagation off: retail loads the player's motion into r3 and copies it to a saved register
 * after the null test (wave 41). */
#pragma opt_propagation off
void MotionHandImpl::Update() {
    if (m_current != 0 && m_player->m_motion != 0) {
        PlayerMotion *motion = m_player->m_motion;
        bool done = false;
        const UnkMotionNode_800AA760 *nodes[2];
        nodes[0] = fn_800AA760(motion, 5);
        nodes[1] = fn_800AA760(motion, 3);
        UpdatePose();
        for (int i = 0; i < 2; i++) {
            Hand *hand = &m_current[i];
            if (m_time >= 0.0f) {
                fn_80416E20(hand->m_anim, 0, m_time);
            }
            done |= hand->Update(i, nodes[i], *reinterpret_cast<float **>(m_player->field_0x60));
        }
        m_time = -1.0f;
        if (done) {
            m_pose = s_poses[m_shownPose].m_next;
        }
    }
}
#pragma opt_propagation reset

void MotionHandImpl::AddTo(UnkClumpList_8006CAE4 *list) {
    if (m_current != 0) {
        for (int i = 0; i < 2; i++) {
            m_current[i].AddTo(list);
        }
    }
}

/* Copy propagation off and the two times assigned one by one (wave 41): retail stores them as
 * floats and copies the player's motion to a saved register after the null test. */
#pragma opt_propagation off
void MotionHandImpl::SetSlot(int slot) {
    Hand *hands = m_hands[slot];
    if (m_current != hands) {
        if (m_current != 0) {
            m_current[0].Detach();
            m_current[1].Detach();
        }
        m_current = hands;
        if (m_current != 0) {
            m_current[0].Attach();
            m_current[1].Attach();
            if (m_player->m_motion != 0) {
                PlayerMotion *motion = m_player->m_motion;
                float time[2];
                time[0] = 0.0f;
                time[1] = 0.0f;
                const UnkMotionNode_800AA760 *nodes[2];
                nodes[0] = fn_800AA760(motion, 5);
                nodes[1] = fn_800AA760(motion, 3);
                for (int i = 0; i < 2; i++) {
                    m_current[i].Update(i, nodes[i], time);
                }
            }
        }
        if (m_player->IsWeaponHidden()) {
            Hide();
        }
    }
}
#pragma opt_propagation reset

/* Restarts the current pose of both hands of slot 0 (name inferred). Retail's loop copies the
 * zeroed counter into the offset, as GC/2.7 does only for a loop in an inline function
 * (wave 41). */
inline void RestartHands(MotionHandImpl *impl) {
    for (int i = 0; i < 2; i++) {
        MotionHandImpl::Hand *hand = &impl->m_hands[0][i];
        if (hand->m_anim != 0) {
            fn_80416F28(hand->m_anim, 0, 0, 0);
            fn_80416F28(hand->m_anim, 0, 1, 0);
        }
    }
}

/* The two motions are read first, right before left (wave 41). */
void MotionHandImpl::SetMotion(const UnkHandMotion_800B07FC *motion) {
    u16 right = motion->m_right;
    u16 left = motion->m_left;
    m_pose = 0xE;
    m_request = 0xE;
    m_shownPose = m_pose;
    fn_80417568(m_hands[0][0].m_anim, left);
    fn_80417568(m_hands[0][1].m_anim, right);
    fn_80416F28(m_hands[0][0].m_anim, 0, 0xB, 0);
    fn_80416F28(m_hands[0][1].m_anim, 0, 0xB, 0);
    RestartHands(this);
}

void MotionHandImpl::Show() {
    if (m_current != 0) {
        m_current[0].Attach();
        m_current[1].Attach();
    }
}

void MotionHandImpl::Hide() {
    if (m_current != 0) {
        m_current[0].Detach();
        m_current[1].Detach();
    }
}

void MotionHandImpl::UpdatePose() {
    if (m_shownPose != m_pose) {
        for (int i = 0; i < 2; i++) {
            Hand *hand = &m_hands[0][i];
            if (hand->m_anim != 0) {
                if (m_blend > 1.0f) {
                    fn_804165D8(hand->m_anim, 0, -1, -1);
                    fn_80416148(hand->m_anim, 0, m_blend);
                }
                s32 motion = s_poses[m_pose].m_motion[i];
                fn_80417568(hand->m_anim, motion < 0 ? (u16)0xFFFF : (u16)(m_motionBase - 1 + motion));
                fn_80416F28(hand->m_anim, 0, 0, 0);
                fn_80416F28(hand->m_anim, 0, 0xB, 0);
                if (m_pose == s_poses[m_pose].m_next) {
                    fn_80416F28(hand->m_anim, 0, 1, 0);
                }
            }
        }
        m_shownPose = m_pose;
        m_blend = 0.0f;
    }
}

/* Gives hand its clump and animation and the updater that keeps them in step (name inferred). */
inline void SetupHand(MotionHandImpl::Hand *hand, RpClump *clump, _stMtnPlayWork *anim) {
    hand->m_clump = clump;
    hand->m_anim = anim;
    if (hand->m_clump != 0 && hand->m_anim != 0) {
        hand->m_updater = new UnkClumpUpdater_80015A7C(hand->m_clump, hand->m_anim);
    } else {
        hand->m_updater = 0;
    }
}

void MotionHandImpl::Load() {
    u32 i;
    void *loader = fn_800122D4();
    fn_80078C4C(m_player.get());
    u32 bones[2];
    bones[0] = fn_80410630("lte.bon");
    bones[1] = fn_80410630("rte.bon");
    m_motionBase = fn_80012608(loader, m_files.m_common1.m_file, m_files.m_common1.m_index);
    for (i = 0; i < 5; i++) {
        m_anims[i] = 0;
        if (m_files.m_set1[i].m_index >= 0) {
            m_clumps[i] = fn_8001277C(loader, m_files.m_set1[i].m_file, m_files.m_set1[i].m_index);
        }
        if (m_clumps[i] != 0) {
            m_clumps[i] = fn_80457090(m_clumps[i]);
            if (m_player->m_motion != 0) {
                fn_80038198(m_clumps[i], fn_800AA584(m_player->m_motion));
            }
            if (s_handModels[i].m_bone >= 0) {
                m_anims[i] = fn_8041B908(bones[s_handModels[i].m_bone], 1);
            }
        }
    }
    for (int slot = 0; slot < 4; slot++) {
        for (int i = 0; i < 2; i++) {
            Hand *hand = &m_hands[slot][i];
            s32 model = s_slotModels[slot][i];
            SetupHand(hand, m_clumps[model], m_anims[model]);
        }
    }
}

void MotionHandImpl::Unload() {
    for (int slot = 0; slot < 4; slot++) {
        for (int i = 0; i < 2; i++) {
            m_hands[slot][i].Release();
        }
    }
    for (u32 i = 0; i < 5; i++) {
        fn_8041B884(m_anims[i]);
        m_anims[i] = 0;
        if (m_clumps[i] != 0) {
            void *world = fn_8046256C(m_clumps[i]);
            if (world != 0) {
                fn_804623EC(world, m_clumps[i]);
            }
            fn_80457468(m_clumps[i]);
            m_clumps[i] = 0;
        }
    }
    m_current = 0;
}

class MotionHand : public Task {
  public:
    MotionHand(Task *parent, const shared_ptr<PlayerBase> &player, const UnkMotionFiles_800ADE78 &files);
    virtual ~MotionHand();
    virtual void Tick(float dt);

    void Hide();
    void Show();
    void GetAnims(_stMtnPlayWork **left, _stMtnPlayWork **right);
    Mtx34f *GetNodeMatrix(int node);
    void SetTime(float time);
    void SetMotion(const UnkHandMotion_800B07FC *motion, float blendTime);
    void SetPose(int pose, float blend);
    void SetSlot(int slot);
    void AddTo(UnkClumpList_8006CAE4 *list);
    void Update();

    /* 0x28 */ boost::scoped_ptr<MotionHandImpl> m_impl;
};
SIZE_ASSERT(MotionHand, 0x2C);

MotionHand::MotionHand(Task *parent, const shared_ptr<PlayerBase> &player, const UnkMotionFiles_800ADE78 &files)
    : Task(parent), m_impl(new MotionHandImpl(player, files)) {
    m_name = "MotionHand";
}

MotionHand::~MotionHand() {}

void MotionHand::Tick(float dt) {}

void MotionHand::Update() {
    m_impl->Update();
}

void MotionHand::AddTo(UnkClumpList_8006CAE4 *list) {
    m_impl->AddTo(list);
}

void MotionHand::SetSlot(int slot) {
    m_impl->SetSlot(slot);
}

void MotionHand::SetPose(int pose, float blend) {
    m_impl->SetPose(pose, blend);
}

void MotionHand::SetMotion(const UnkHandMotion_800B07FC *motion, float blendTime) {
    m_impl->SetMotion(motion);
}

void MotionHand::SetTime(float time) {
    m_impl->SetTime(time);
}

inline Mtx34f *FindNodeMatrix(MotionHandImpl *impl, u32 node) {
    for (u32 i = 0; i < 5; i++) {
        if (impl->m_anims[i] != 0) {
            Mtx34f *matrix = fn_80415E70(impl->m_anims[i], node);
            if (matrix != 0) {
                return matrix;
            }
        }
    }
    return 0;
}

Mtx34f *MotionHand::GetNodeMatrix(int node) {
    return FindNodeMatrix(m_impl.get(), node);
}

void MotionHand::GetAnims(_stMtnPlayWork **left, _stMtnPlayWork **right) {
    m_impl->GetAnims(left, right);
}

void MotionHand::Show() {
    m_impl->Show();
}

void MotionHand::Hide() {
    m_impl->Hide();
}

} // namespace Shadow
} // namespace Player
