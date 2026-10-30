/* @unnamed@NpcControlFlyType_cpp@::NpcFlyType (class name from RTTI; base
 * Player::Control::ControlBase): the control of the flying NPCs (control 2 of
 * Game/FlyTypeControlFactory.cpp). It keeps a point to fly to beside the player the NPC follows,
 * ahead of it and to the side the NPC is on, and steers the NPC towards it; when the followed
 * player heads towards the NPC it also pushes the NPC along the player's recent path. The distances
 * come from the character's parameters (Game/PlayerCharaParam.h). fn_801F326C makes one; other
 * units call it by its address, so it keeps that name. File name from the RTTI name; function and
 * member names are inferred. Definitions are in reverse address order (deferred inlining emits
 * functions last-defined first). */
#define PLAYER_CONTROL_SLOT_ARGS
#include "Game/CharCommand.h"
#include "Game/MathTypes.h"
#include "Game/Npc.h"
#include "Game/Player.h"
#include "Game/PlayerCharaParam.h"
#include "Game/PlayerControl.h"
#include "boost/shared_ptr.hpp"
#include "boost/weak_ptr.hpp"
#include "size_assert.h"
#include <math.h>

using boost::shared_ptr;
using Player::PlayerBase;
using Player::Control::ControlBase;
using Player::Control::InitializeArgs;

/* A record of the player's history (struct name unknown; see Game/PlayerHistory.cpp): a position
 * at 0x24. */
struct UnkPlayerRecord_800A849C {
    /* 0x00 */ u8 field_0x0[0x24];
    /* 0x24 */ Vec3f m_position;
};

/* The player's history (class name unknown; see Game/PlayerHistory.cpp). */
class UnkPlayerHistory_800A82C8 {
  public:
    UnkPlayerRecord_800A849C *Get(int back);
};

/* The player's members read here (struct name unknown; Player::PlayerBase leaves them unmapped). */
struct UnkPlayerFields_801F32B4 {
    /* 0x000 */ u8 field_0x0[0x64];
    /* 0x064 */ s32 m_kind; /* the character (inferred) */
    /* 0x068 */ u8 field_0x68[0x100 - 0x68];
    /* 0x100 */ Vec3f field_0x100;
    /* 0x10C */ u8 field_0x10C[0x250 - 0x10C];
    /* 0x250 */ UnkPlayerHistory_800A82C8 *m_history;
};

#define FieldsOf(player) reinterpret_cast<UnkPlayerFields_801F32B4 *>(player)

extern "C" {
/* Whether the NPC follows a player, and the player it follows (see
 * Game/NpcControlRestartFollow.cpp; behaviour inferred). */
bool fn_80073708(Player::Npc::Npc *npc);
boost::weak_ptr<PlayerBase> fn_8007363C(Player::Npc::Npc *npc);
/* A player's forward and up directions (behaviour inferred). */
Vec3f fn_80079698(PlayerBase *player);
Vec3f fn_800796DC(PlayerBase *player);
/* Whether the control stays idle (behaviour unknown). */
bool fn_801F42C8(const boost::weak_ptr<PlayerBase> *self, PlayerBase *player);
/* A per-frame rate of value (behaviour inferred). */
float fn_8005AE20(float value);

/* Vector routines (see Game/MathTypes.h). */
Vec3f fn_800091A4(const Vec3f *vector, float scale); /* vector * scale */
Vec3f fn_8040CDA8(const Vec3f *v, float scale);
Vec3f fn_8040CDD0(const Vec3f *a, const Vec3f *b);  /* a + b */
Vec3f fn_8040CFB8(const Vec3f *a, const Vec3f *b);  /* a x b */
Vec3f fn_8040CEE8(const Vec3f *v);                  /* normalized */
float fn_8040CF90(const Vec3f *a, const Vec3f *b); /* a . b */
void fn_8040CE04(Vec3f *vector, const Vec3f *value); /* vector -= value */
void fn_8040CFF8(Vec3f *vector, const Vec3f *value); /* vector += value */
float fn_8040CE74(Vec3f *v);                         /* normalizes v */
void fn_8040CE38(Vec3f *vector, float scale);        /* vector *= scale */
/* Clamps (see Game/GunLift.cpp). */
const float &fn_80070460(const float &value, const float &low, const float &high);
}

/* fn_8040CD74 returns lhs - rhs through its first argument, as a function returning a Vec3f does. */
typedef Vec3f (*VectorSubtract)(const Vec3f *lhs, const Vec3f *rhs);
#define Subtract(lhs, rhs) (reinterpret_cast<VectorSubtract>(fn_8040CD74)(lhs, rhs))

namespace {

class NpcFlyType : public ControlBase {
  public:
    NpcFlyType(const shared_ptr<PlayerBase> &player);
    virtual ~NpcFlyType();
    virtual void Enter(int prevId, const InitializeArgs &args);
    virtual void Leave(int nextId);
    virtual void vf_0x14(float dt);

    void Steer();
    void UpdateGoal();

    static const char *s_taskName;

    /* 0x14 */ shared_ptr<Player::Npc::Npc> m_npc;
    /* 0x1C */ Vec3f m_goal;     /* the point the NPC flies to */
    /* 0x28 */ float m_nearRange; /* the character's parameters (Game/PlayerCharaParam.h) */
    /* 0x2C */ float m_ahead;
    /* 0x30 */ float m_aside;
};
SIZE_ASSERT(NpcFlyType, 0x34);

const char *NpcFlyType::s_taskName = "NpcFlyType";

NpcFlyType::NpcFlyType(const shared_ptr<PlayerBase> &player) : ControlBase(player, s_taskName, 2) {
    m_npc = boost::dynamic_pointer_cast<Player::Npc::Npc>(m_player);
    m_nearRange = lbl_804F1834[FieldsOf(m_player.get())->m_kind].m_flyNearRange;
    m_ahead = lbl_804F1834[FieldsOf(m_player.get())->m_kind].m_flyAhead;
    m_aside = lbl_804F1834[FieldsOf(m_player.get())->m_kind].m_flyAside;
}

NpcFlyType::~NpcFlyType() {}

void NpcFlyType::Enter(int prevId, const InitializeArgs &args) {
    fn_800091D0(&m_goal, &m_player.get()->m_position);
}

void NpcFlyType::Leave(int nextId) {}

void NpcFlyType::vf_0x14(float dt) {
    if (!fn_801F42C8(&m_player->m_self, m_player.get())) {
        UpdateGoal();
        Steer();
    }
}

void NpcFlyType::UpdateGoal() {
    shared_ptr<PlayerBase> leader = fn_8007363C(m_npc.get()).lock();
    if (leader) {
        Vec3f forward;
        fn_800091EC(&forward, &(const Vec3f &)fn_80079698(leader.px));
        Vec3f up;
        fn_800091EC(&up, &(const Vec3f &)fn_800796DC(leader.px));
        Vec3f side;
        fn_800091EC(&side, &(const Vec3f &)fn_8040CEE8(&(const Vec3f &)fn_8040CFB8(&up, &forward)));
        Vec3f goal;
        fn_800091EC(&goal, &(const Vec3f &)fn_8040CDD0(&leader.px->m_position,
                                                       &(const Vec3f &)fn_8040CDA8(&forward, m_ahead)));
        if (fn_8040CF90(&side, &(const Vec3f &)Subtract(&m_player.get()->m_position, &leader.px->m_position)) >=
            0.0f) {
            fn_8040CFF8(&goal, &(const Vec3f &)fn_800091A4(&side, m_aside));
        } else {
            fn_8040CE04(&goal, &(const Vec3f &)fn_800091A4(&side, m_aside));
        }
        Vec3f offset;
        fn_800091EC(&offset, &(const Vec3f &)Subtract(&m_goal, &goal));
        float length = fn_8040CF50(&offset);
        float along = fn_8040CF90(&offset, &forward);
        if (along < 0.0f) {
            fn_8040CE04(&offset, &(const Vec3f &)fn_800091A4(&forward, along));
        } else if (along > 10.0f) {
            fn_8040CE04(&offset, &(const Vec3f &)fn_800091A4(&forward, along - fn_8005AE20(1.0f)));
        }
        float range = m_nearRange;
        if (length > range) {
            fn_800091D0(&offset, &(const Vec3f &)fn_8040CDA8(&(const Vec3f &)fn_8040CEE8(&offset), range));
        }
        fn_800091D0(&m_goal, &(const Vec3f &)fn_8040CDD0(&goal, &offset));
    }
}

void NpcFlyType::Steer() {
    if (fn_80073708(m_npc.get())) {
        shared_ptr<PlayerBase> leader = fn_8007363C(m_npc.get()).lock();
        if (leader) {
            Vec3f toLeader;
            fn_800091EC(&toLeader, &(const Vec3f &)Subtract(&leader.px->m_position, &m_npc.get()->m_position));
            float facing = fn_8040CF90(&(const Vec3f &)fn_800796DC(leader.px),
                                       &(const Vec3f &)fn_8040CEE8(&toLeader));
            if (facing > 0.26f) {
                float rate = (facing - 0.26f) / 0.74f;
                rate = fn_80070460(rate, 0.0f, 1.0f);
                rate = pow(rate, 0.25);
                UnkPlayerRecord_800A849C *record = FieldsOf(leader.px)->m_history->Get(0);
                Vec3f path;
                fn_800091EC(&path, &(const Vec3f &)Subtract(&record->m_position, &FieldsOf(leader.px)->field_0x100));
                m_player->PushCommand(new AccelerateCommand(fn_8040CDA8(&path, rate), 4));
            }
            Vec3f direction;
            fn_800091EC(&direction, &(const Vec3f &)Subtract(&m_goal, &m_npc.get()->m_position));
            float distance = fn_8040CF50(&direction);
            fn_8040CE74(&direction);
            if (distance < 20.0f) {
                fn_8040CE38(&direction, distance / 20.0f);
            }
            if (fn_8040CF50(&toLeader) < m_nearRange) {
                fn_8040CE38(&direction, 0.25f);
            }
            m_player->PushCommand(new AccelerateInputCommand(direction, 4));
        }
    }
}

} // namespace

extern "C" ControlBase *fn_801F326C(const shared_ptr<PlayerBase> &player) {
    return new NpcFlyType(player);
}
