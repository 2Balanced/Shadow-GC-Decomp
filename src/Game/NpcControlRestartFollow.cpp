/* @unnamed@NpcControlRestartFollow_cpp@::NpcRestartFollow (class name from RTTI; base
 * Player::Control::ControlBase; its name is "NpcRestartFollow"): the control that brings a
 * following NPC back to the player it follows (control 4 of the NPC control factories). Once
 * fn_80073908 of the NPC reaches 1, it looks for a free point on the ground at one of four places
 * around that player and puts the NPC there, then switches the NPC back to control 2. The unit
 * also holds the mission manager's accessor (fn_8006D6A8, which other units call by its address).
 * File name from the RTTI name; function and member names are inferred. Definitions are in reverse
 * address order (deferred inlining emits functions last-defined first).
 *
 * NonMatching: every function body matches. The three macros below remove bad_weak_ptr and the
 * shared_ptr/base bodies and records, reducing our .text from 4648 to 3888 bytes against retail's
 * 3020. What remains is emission order: the constructor and the weak shared_ptr copies land in a
 * different order from retail's, so the flipped DOL's branch displacements differ. */
#define BOOST_SP_EXTERN_BASE_DESTRUCTOR
#define BOOST_SP_EXTERN_COUNTED_DESTRUCTORS
#define BOOST_SP_NO_BAD_WEAK_PTR
#define BOSSCTRLUNIT_HELPERS
#include "Game/BossCtrlUnit.h"
#include "Game/CharaColliBaseCharInfo.h"
#include "Game/CharInfo.h"
#include "Game/MathTypes.h"
#include "Game/Npc.h"
#include "Game/Player.h"
#include "Game/PlayerControl.h"
#include "boost/shared_ptr.hpp"
#include "boost/weak_ptr.hpp"
#include "size_assert.h"

using boost::shared_ptr;
using boost::weak_ptr;
using Player::PlayerBase;
using Player::Control::ControlBase;
using Player::Npc::Npc;
using SonicteamUSA::System::CharaColli::CharaColliBase;
using SonicteamUSA::System::CharaColli::CharaColliBaseCharInfo;

namespace Mission {

/* MissionManager (class name from its constructor's symbol; see Game/Mission.cpp): the current
 * player at 0x20. */
class MissionManager {
  public:
    MissionManager();
    ~MissionManager();

    /* 0x00 */ u8 field_0x0[0x20];
    /* 0x20 */ s32 m_currentPlayer;
    /* 0x24 */ u8 field_0x24[0x44 - 0x24];
};
SIZE_ASSERT(MissionManager, 0x44);

} // namespace Mission

/* A segment for the line checks (built by fn_80020A20; struct name unknown). */
struct UnkSegment_80020A20 {
    /* 0x00 */ float m_start[3];
    /* 0x0C */ float m_end[3];
};

/* A collision a line check meets (struct name unknown): its owner and its kind's flags. */
struct UnkCollision_80021E24 {
    /* 0x00 */ u8 field_0x0[0x8];
    /* 0x08 */ CharaColliBase *m_owner;
    /* 0x0C */ u32 m_flags;
};

typedef bool (*UnkCollisionFilter)(UnkCollision_80021E24 *collision);

extern "C" {
/* NPC helpers (behaviour unknown; fn_80073908 returns a time or ratio the control waits on,
 * fn_8007363C the player the NPC follows, fn_80073A08 the NPC's player number). */
float fn_80073908(Npc *npc);
void fn_8007393C(Npc *npc);
void fn_80073964(Npc *npc);
weak_ptr<PlayerBase> fn_8007363C(Npc *npc);
s32 fn_80073A08(Npc *npc);
/* Switches the control of the character task (see Game/BossSonicExecuter.cpp). */
void fn_8009EBA4(Task *task, const Player::Control::InitializeArgs *args);
/* Puts chara at position, turned to angle (see Game/BossSonicControlRestart.cpp). */
void fn_800B8000(const weak_ptr<CharInfo> &chara, const Vec3f &position, const Vec3f &angle);
/* A vector turned into the player's frame (behaviour inferred). */
Vec3f fn_80079628(PlayerBase *player, const Vec3f *offset);
Vec3f fn_8040CDA8(const Vec3f *vector, float scale); /* vector * scale */
void fn_8040CDD0(Vec3f *out, const Vec3f *a, const Vec3f *b); /* out = a + b */
/* Finds the ground below position along direction (see Game/BossCtrlUnit.h; behaviour
 * inferred). */
void fn_800935B4(UnkShadow_8009368C *shadow, const Vec3f *position, const Vec3f *direction, int arg3);
/* Line checks against the collisions of the world (see Game/GadgetUtil.h). */
UnkSegment_80020A20 *fn_80020A20(UnkSegment_80020A20 *segment, const Vec3f *start, const Vec3f *end);
void *fn_80020D40(void);
void *fn_80021E24(void *world, UnkSegment_80020A20 *segment, int arg2, int arg3,
                  UnkCollisionFilter filter, int arg5);
void fn_800265A4(void *hits, int arg1);
/* Tests a bit of a bit array (Game/NpcSonicMotion.cpp). */
bool fn_8006CD78(const u32 *bits, int bit);
/* The character a collider belongs to. */
weak_ptr<CharInfo> fn_800586EC(CharaColliBaseCharInfo *collider);
}

/* The mission manager, made on first use (other units call it by its address). */
/* Returns x (an inline call, so that the accessor below is not inlined itself; deferred inlining
 * is one level deep). */
inline Mission::MissionManager *ManagerRef(Mission::MissionManager *manager) {
    return manager;
}

/* The instance pointer of a class made on first use (name invented). */
template <class T> struct Instance {
    static T *s_instance;
};

template <class T> T *Instance<T>::s_instance;

extern "C" inline Mission::MissionManager *fn_8006D6A8(void) {
    if (Instance<Mission::MissionManager>::s_instance == 0) {
        static Mission::MissionManager s_manager;
        Instance<Mission::MissionManager>::s_instance = ManagerRef(&s_manager);
    }
    return Instance<Mission::MissionManager>::s_instance;
}

/* The player members the control reads (names inferred). */
struct UnkPlayerFields_8006D50C {
    /* 0x000 */ u8 field_0x0[0xE8];
    /* 0x0E8 */ Vec3f m_ground; /* (meaning unknown) */
};

inline UnkPlayerFields_8006D50C *Fields(PlayerBase *player) {
    return reinterpret_cast<UnkPlayerFields_8006D50C *>(player);
}

namespace {

class NpcRestartFollow : public ControlBase {
  public:
    NpcRestartFollow(const shared_ptr<PlayerBase> &player);
    virtual ~NpcRestartFollow();
    virtual void vf_0x0C();
    virtual void vf_0x10();
    virtual void vf_0x14(float dt);

    /* Puts the NPC back to following (control 2) after placing it. */
    void Restart();
    /* Gives up and switches the NPC back to control 2. */
    void GiveUp();
    /* A free point on the ground near leader (false: none). */
    bool FindRestartPoint(const shared_ptr<PlayerBase> &leader, Vec3f *point);

    static const char *s_taskName;

    /* 0x14 */ shared_ptr<Npc> m_npc;
};
SIZE_ASSERT(NpcRestartFollow, 0x1C);

const char *NpcRestartFollow::s_taskName = "NpcRestartFollow";

} // namespace

/* Whether a line check stops at collision: not at players' or their partners' (name inferred). */
static bool IsBlocking(UnkCollision_80021E24 *collision) {
    if (collision->m_flags & 0x40000) {
        return false;
    }
    if (collision->m_owner) {
        CharaColliBaseCharInfo *owner = dynamic_cast<CharaColliBaseCharInfo *>(collision->m_owner);
        if (owner) {
            shared_ptr<CharInfo> chara = fn_800586EC(owner).lock();
            if (chara) {
                if (chara->field_0x5C == 1) {
                    return false;
                }
                if (chara->field_0x5C == 2) {
                    return false;
                }
            }
        }
    }
    return (collision->m_flags & 0x1A93) != 0;
}

/* out = a + b (name inferred). */
inline void AddVectors(Vec3f *out, const Vec3f &a, const Vec3f &b) {
    fn_8040CDD0(out, &a, &b);
}

/* Passes a returned value on by address (as in Game/ArkCannon.cpp's casts). */
inline const Vec3f &Ref(const Vec3f &vector) {
    return vector;
}

/* The point at place index around the player the NPC of self follows, returned through out (names
 * inferred). */
static void GetFollowPoint(Vec3f *out, const weak_ptr<PlayerBase> &self, int index) {
    float right = 15.0f;
    float above = 20.0f;
    float front = 20.0f;
    float left = -15.0f;
    float back = -20.0f;
    float offsets[4][3] = {
        {right, above, front},
        {left, above, front},
        {right, above, back},
        {left, above, back},
    };
    shared_ptr<Npc> npc = boost::dynamic_pointer_cast<Npc>(self.lock());
    if (npc) {
        shared_ptr<PlayerBase> leader = fn_8007363C(npc.get()).lock();
        if (leader) {
            Vec3f turned;
            fn_800091D0(&turned, &fn_80079628(leader.get(), &Vec3f(offsets[index][0], offsets[index][1],
                                                                   offsets[index][2])));
            AddVectors(out, leader->m_position, turned);
            return;
        }
        fn_800091EC(out, &npc.get()->m_position);
        return;
    }
    fn_800091EC(out, &lbl_805E07A0);
}
/* GetFollowPoint called as returning the point by value. */
typedef Vec3f (*FollowPointFn)(const weak_ptr<PlayerBase> &self, int index);

namespace {

NpcRestartFollow::NpcRestartFollow(const shared_ptr<PlayerBase> &player)
    : ControlBase(player, s_taskName, 4) {
    m_npc = boost::dynamic_pointer_cast<Npc>(m_player);
}

NpcRestartFollow::~NpcRestartFollow() {}

void NpcRestartFollow::vf_0x0C() {
    fn_80073964(m_npc.get());
}

void NpcRestartFollow::vf_0x10() {}

void NpcRestartFollow::vf_0x14(float dt) {
    if (fn_80073908(m_npc.get()) >= 1.0f) {
        shared_ptr<PlayerBase> leader = fn_8007363C(m_npc.get()).lock();
        if (!leader) {
            GiveUp();
            return;
        }
        s32 current = fn_8006D6A8()->m_currentPlayer;
        if (current != fn_80073A08(m_npc.get())) {
            GiveUp();
            return;
        }
        Vec3f point;
        if (FindRestartPoint(leader, &point)) {
            fn_800B8000(m_player.get()->m_self, point, leader.get()->m_rotation);
            fn_800091D0(&Fields(m_npc.get())->m_ground, &Fields(leader.get())->m_ground);
            Restart();
        }
    }
}

bool NpcRestartFollow::FindRestartPoint(const shared_ptr<PlayerBase> &leader, Vec3f *point) {
    if (!fn_8006CD78(leader.get()->m_stateBits, 2)) {
        return false;
    }
    if (fn_8006CD78(leader.get()->m_stateBits, 0x31)) {
        return false;
    }
    if (fn_8006CD78(leader.get()->m_stateBits, 0x10)) {
        return false;
    }
    for (int i = 0; i < 4; i++) {
        Vec3f candidate;
        fn_800091EC(&candidate, &(const Vec3f &)((FollowPointFn)GetFollowPoint)(m_player.get()->m_self, i));
        UnkShadow_8009368C ground;
        fn_800935B4(&ground, &candidate, &fn_8040CDA8(&Fields(m_npc.get())->m_ground, -50.0f), 0);
        if (ground.m_visible) {
            UnkSegment_80020A20 segment;
            fn_80020A20(&segment, &leader.get()->m_position, &candidate);
            void *hits = fn_80021E24(fn_80020D40(), &segment, 3, 2, IsBlocking, 0);
            if (hits) {
                fn_800265A4(hits, 1);
            } else {
                fn_800091D0(point, &candidate);
                return true;
            }
        }
    }
    return false;
}

void NpcRestartFollow::GiveUp() {
    fn_8009EBA4(m_npc.get()->m_task, &Player::Control::InitializeArgs(2));
}

void NpcRestartFollow::Restart() {
    fn_8007393C(m_npc.get());
    fn_8009EBA4(m_npc.get()->m_task, &Player::Control::InitializeArgs(2));
}

} // namespace

/* Makes the control (called by the NPC control factories by its address). */
extern "C" ControlBase *fn_8006D268(const shared_ptr<PlayerBase> &player) {
    return new NpcRestartFollow(player);
}
