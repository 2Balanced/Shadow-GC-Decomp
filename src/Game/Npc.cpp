/* Player::Npc::Npc (class name from RTTI; base Player::PlayerBase): a non-player character, the
 * partner that follows a player. Its state (class name unknown) is kept in a boost::scoped_ptr at
 * 0x290: flag bits, its player number, how far it has appeared, two ranges, a height, the number of
 * times it met a player and the player it follows. Other units reach the state through the C
 * functions below, which keep their fn_ names. The unit also holds the out-of-line copies of three
 * accessors other units call by their addresses: the character list (fn_80074194), the stage
 * action manager (fn_80074470) and a player's mission (fn_800740E4). File name from the class;
 * function and member names are inferred. Definitions are in reverse address order (deferred
 * inlining emits functions last-defined first). */
#define NPC_IMPLEMENTATION
#define PLAYER_OBJECT_MEMBERS
#define PLAYER_BASE_INLINE_SLOTS
#define CHARINFO_INLINE_SLOT_2C
#define PLAYER_EXECUTER_MEMBERS

#include "Game/CharInfo.h"
#include "Game/MathTypes.h"
#include "Game/Npc.h"
#include "Game/Player.h"
#include "Game/PlayerBehavior.h"
#include "Game/PlayerExecuter.h"
#include "Game/StageAction.h"
#include "boost/scoped_ptr.hpp"
#include "boost/shared_ptr.hpp"
#include "boost/weak_ptr.hpp"
#include <dolphin/types.h>
#include "size_assert.h"

using boost::shared_ptr;
using boost::weak_ptr;
using Player::PlayerBase;
using Player::Behavior::Behavior;
using Player::Behavior::InitializeArgs;
using Player::Behavior::UnkBehaviorExecuter_800A7F14;
using Player::Npc::Npc;

namespace Mission {

class Mission {
  public:
    void SetFlag() {
        m_flag = 1;
    }
    bool IsState(s32 state) const {
        return m_state == state;
    }

    /* 0x00 */ u8 field_0x0[0xC];
    /* 0x0C */ s32 m_state;
    /* 0x10 */ u8 field_0x10[0x14 - 0x10];
    /* 0x14 */ u8 m_flag;
};

/* The missions of the three players (see Game/Mission.cpp). */
class MissionManager {
  public:
    void SetPlayer(int player);

    /* 0x00 */ u8 field_0x0[0x8];
    /* 0x08 */ shared_ptr<Mission> m_missions[3];
};

} // namespace Mission

/* The list of the players (Game/PlayerManageUnit.cpp). */
class UnkCharaList_80074194 {
  public:
    UnkCharaList_80074194();
    ~UnkCharaList_80074194();

    /* 0x0 */ u8 field_0x0[0x10];
};

/* The game state (struct name unknown; see Game/NpcHint.cpp). */
struct UnkGameState_8057D6A0 {
    /* 0x000 */ u8 field_0x0[0xB1];
    /* 0x0B1 */ u8 field_0xB1; /* meaning unknown */
    /* 0x0B2 */ u8 field_0xB2[0xD4 - 0xB2];
    /* 0x0D4 */ s32 m_greeted[1]; /* per character kind: the partner has greeted (inferred) */
};
extern UnkGameState_8057D6A0 lbl_8057D6A0;

extern "C" {
/* A flag word (Game/NpcControlSonicType.cpp): cleared, set, tested and reset bit by bit. */
void fn_80014E04(u32 *flags);
void fn_80014E1C(u32 *flags, int bit);
bool fn_80014E34(u32 *flags, int bit);
void fn_8006127C(u32 *flags, int bit);
/* The mission manager (Game/NpcControlRestartFollow.cpp). */
Mission::MissionManager *fn_8006D6A8(void);
/* The player nearest to a position (Game/PlayerManageUnit.cpp). */
weak_ptr<CharInfo> fn_800A8A7C(UnkCharaList_80074194 *list, const Vec3f *position);
/* Whether the behavior executer runs behavior id, and switching it to another. */
bool fn_800A7EEC(UnkBehaviorExecuter_800A7F14 *executer, int id);
void fn_800A7F14(UnkBehaviorExecuter_800A7F14 *executer, const InitializeArgs *args);
float fn_8040D180(const Vec3f *a, const Vec3f *b); /* |a - b|^2 */
Vec3f fn_8040B67C(const Mtx34f *m, const Vec3f *point);
void fn_8040B790(Mtx34f *m, const Vec3f *scale); /* apply a scale */
/* A point of a character (behaviour unknown). */
Vec3f fn_80016284(CharInfo *chara);
}

extern "C" double pow(double x, double y);

/* The partner's level hint (Game/NpcHint.cpp). */

void ShowLevelHint(const weak_ptr<PlayerBase> &player);

/* Returns x (an inline call, so that the accessors below are not inlined themselves; deferred
 * inlining is one level deep). */
template <class T> inline T &Ref(T &x) {
    return x;
}

/* The instance pointer of a class made on first use (as in Game/NpcControlRestartFollow.cpp). */
template <class T> struct Instance {
    static T *s_instance;
};

template <class T> T *Instance<T>::s_instance;

/* The holder of the card controls made on first use by fn_801E5B80 (class name unknown). */
class UnkCardCtrlHolder_801E5B80;

/* Never called: retail's .sbss holds the instance pointer of UnkCardCtrlHolder_801E5B80 here, after
 * this unit's own, so the unit names it. */
UnkCardCtrlHolder_801E5B80 *fn_UnkCardCtrlHolderInstance() {
    return Instance<UnkCardCtrlHolder_801E5B80>::s_instance;
}

/* A flag word whose constructor clears it. */
struct UnkFlags_80014E04 {
    UnkFlags_80014E04() {
        fn_80014E04(&m_bits);
    }

    /* 0x0 */ u32 m_bits;
};

/* The NPC's state (class name unknown). Flag bits: 0 it follows a player, 1 its weapon is hidden,
 * 2 it is appearing, 3 it has met a player, 4 it is kept when the list drops NPCs (meanings
 * inferred; Game/NpcManageUnit.cpp). */
class UnkNpcState_800745C8 {
  public:
    UnkNpcState_800745C8(int player);
    ~UnkNpcState_800745C8();
    /* Whether the NPC may start following the character nearest to it. */
    bool CanFollow(const shared_ptr<PlayerBase> &self, const weak_ptr<CharInfo> &nearest);

    /* 0x00 */ UnkFlags_80014E04 m_flags;
    /* 0x04 */ s32 m_player;
    /* 0x08 */ float m_appear; /* 0 hidden .. 1 shown */
    /* 0x0C */ float m_contactRange;
    /* 0x10 */ float m_followRange;
    /* 0x14 */ float m_height;
    /* 0x18 */ s32 m_metCount;
    /* 0x1C */ weak_ptr<PlayerBase> m_leader;
};
SIZE_ASSERT(UnkNpcState_800745C8, 0x24);

/* Deletes a state (name inferred; see checked_delete below). */
inline void DeleteNpcState(UnkNpcState_800745C8 *state) {
    delete state;
}

/* Retail keeps boost::checked_delete<UnkNpcState_800745C8> out of line (0x800742D8) and the
 * state's destructor at its own place (0x80074578); this specialization calls an inline helper,
 * which gives retail's code and order (see docs/research-notes.md, EnergyCore). */
namespace boost {
template <> inline void checked_delete<UnkNpcState_800745C8>(UnkNpcState_800745C8 *x) {
    DeleteNpcState(x);
}
} // namespace boost

#define STATE(npc) ((npc)->m_state)

UnkNpcState_800745C8::UnkNpcState_800745C8(int player) : m_player(player) {
    m_appear = 1.0f;
    m_contactRange = 400.0f;
    m_followRange = 100.0f;
    m_height = 0.0f;
    m_metCount = 0;
}

UnkNpcState_800745C8::~UnkNpcState_800745C8() {}

/* The stage action manager, made on first use (other units call it by its address). */
extern "C" inline StageActionManager *fn_80074470(void) {
    if (Instance<StageActionManager>::s_instance == 0) {
        static StageActionManager s_manager;
        Instance<StageActionManager>::s_instance = &Ref(s_manager);
    }
    return Instance<StageActionManager>::s_instance;
}

bool UnkNpcState_800745C8::CanFollow(const shared_ptr<PlayerBase> &self, const weak_ptr<CharInfo> &nearest) {
    if (nearest.expired()) {
        return false;
    }
    shared_ptr<CharInfo> chara = nearest.lock();
    if (fn_8040D180(&self->m_position, &chara->m_position) > m_followRange * m_followRange) {
        return false;
    }
    if (!fn_800A7EEC(self->m_behaviors, 0) && !fn_800A7EEC(self->m_behaviors, 1)) {
        return false;
    }
    if (fn_80074470()->m_prevState != 3) {
        return false;
    }
    return true;
}

namespace Player {
namespace Npc {

Npc::Npc(int arg0, int arg1, const char *name, int arg3)
    : PlayerBase(arg0, arg1, name), m_state(new UnkNpcState_800745C8(arg3)) {}

Npc::~Npc() {}

} // namespace Npc
} // namespace Player

/* The behavior an executer runs (other units call it by its address). */
extern "C" inline shared_ptr<Behavior> fn_80074200(Player::Executer *executer) {
    return executer->m_behavior;
}

/* The list of the players, made on first use (other units call it by its address). */
extern "C" inline UnkCharaList_80074194 *fn_80074194(void) {
    if (Instance<UnkCharaList_80074194>::s_instance == 0) {
        static UnkCharaList_80074194 s_list;
        Instance<UnkCharaList_80074194>::s_instance = &Ref(s_list);
    }
    return Instance<UnkCharaList_80074194>::s_instance;
}

/* The mission of a player (other units call it by its address). */
extern "C" inline weak_ptr<Mission::Mission> fn_800740E4(Mission::MissionManager *manager, int player) {
    return weak_ptr<Mission::Mission>(manager->m_missions[player]);
}

extern "C" {

bool fn_80073708(Npc *npc);
void fn_80073730(Npc *npc, bool following);
void fn_80073664(Npc *npc, const weak_ptr<PlayerBase> &leader);
bool fn_80073914(Npc *npc);
s32 fn_80073A08(Npc *npc);

/* The frame update: fades the NPC in or out, keeps its weapon behavior in step, and lets it start
 * following the nearest player when it is close enough. */
void fn_80073AD0(Npc *npc) {
    float dt = **reinterpret_cast<float **>(npc->field_0x60);
    {
        shared_ptr<Behavior> behavior = fn_80074200(reinterpret_cast<Player::Executer *>(npc->m_behaviors));
        if (npc->IsWeaponHidden()) {
            if (!behavior || behavior->m_id != 0x23) {
                fn_800A7F14(npc->m_behaviors, &(const InitializeArgs &)InitializeArgs(0x23));
            }
        } else {
            if (!behavior || behavior->m_id == 0x23) {
                fn_800A7F14(npc->m_behaviors, &(const InitializeArgs &)InitializeArgs(0));
            }
        }
    }

    if (fn_80073914(npc)) {
        if (STATE(npc)->m_appear < 1.0f) {
            STATE(npc)->m_appear += dt / 0.1f;
            if (STATE(npc)->m_appear > 1.0f) {
                STATE(npc)->m_appear = 1.0f;
            }
        }
        return;
    }
    if (STATE(npc)->m_appear > 0.0f) {
        STATE(npc)->m_appear -= dt / 0.1f;
        if (STATE(npc)->m_appear < 0.0f) {
            STATE(npc)->m_appear = 0.0f;
        }
    }
    if (fn_80073708(npc)) {
        return;
    }
    weak_ptr<CharInfo> nearest = fn_800A8A7C(fn_80074194(), &npc->m_position);
    if (STATE(npc)->CanFollow(npc->m_self.lock(), nearest)) {
        shared_ptr<PlayerBase> leader = boost::dynamic_pointer_cast<PlayerBase>(nearest.lock());
        fn_80073730(npc, true);
        fn_80073664(npc, weak_ptr<PlayerBase>(leader));
        fn_8006D6A8()->SetPlayer(fn_80073A08(npc));
        s32 *greeted = &lbl_8057D6A0.m_greeted[npc->m_kind];
        {
            weak_ptr<Mission::Mission> missionRef = fn_800740E4(fn_8006D6A8(), fn_80073A08(npc));
            shared_ptr<Mission::Mission> mission = missionRef.lock();
            if (mission && !mission->IsState(2)) {
                mission->SetFlag();
            }
        }

        ShowLevelHint(npc->m_self);
        if (*greeted == 0) {
            *greeted = 1;
            if (lbl_8057D6A0.field_0xB1 == 0) {
                fn_800A7F14(npc->m_behaviors, &(const InitializeArgs &)InitializeArgs(0x24));
            }
        }
        STATE(npc)->m_metCount++;
    }
}

} // extern "C"

namespace Player {
namespace Npc {

void Npc::vf_0x34() {
    fn_8006127C(&STATE(this)->m_flags.m_bits, 1);
}

void Npc::vf_0x38() {
    fn_80014E1C(&STATE(this)->m_flags.m_bits, 1);
}

bool Npc::IsWeaponHidden() {
    return fn_80014E34(&STATE(this)->m_flags.m_bits, 1) || STATE(this)->m_appear >= 1.0f;
}

} // namespace Npc
} // namespace Player

extern "C" {

void fn_80073A14(Npc *npc, s32 player) {
    STATE(npc)->m_player = player;
}

/* The NPC's player number. */
s32 fn_80073A08(Npc *npc) {
    return STATE(npc)->m_player;
}

/* Marks the NPC to be kept. */
void fn_800739E0(Npc *npc) {
    fn_80014E1C(&STATE(npc)->m_flags.m_bits, 4);
}

bool fn_800739B8(Npc *npc) {
    return fn_80014E34(&STATE(npc)->m_flags.m_bits, 4);
}

void fn_80073964(Npc *npc);

/* Shows the NPC at once. */
void fn_8007398C(Npc *npc) {
    STATE(npc)->m_appear = 1.0f;
    fn_80073964(npc);
}

/* Starts the NPC appearing. */
void fn_80073964(Npc *npc) {
    fn_80014E1C(&STATE(npc)->m_flags.m_bits, 2);
}

/* Starts the NPC disappearing. */
void fn_8007393C(Npc *npc) {
    fn_8006127C(&STATE(npc)->m_flags.m_bits, 2);
}

bool fn_80073914(Npc *npc) {
    return fn_80014E34(&STATE(npc)->m_flags.m_bits, 2);
}

float fn_80073908(Npc *npc) {
    return STATE(npc)->m_appear;
}

/* The NPC's draw matrix: at its position, swollen and flattened by how far it has appeared. */
void fn_80073844(Mtx34f *out, Npc *npc) {
    Mtx34f matrix;
    Mtx34f translation;
    fn_8040B034(&translation, &npc->m_position);
    fn_8040C148(&matrix, &translation);
    float z = 1.5f * STATE(npc)->m_appear + 1.0f;
    float y = (float)pow(STATE(npc)->m_appear, 0.4f);
    float x = 1.5f * STATE(npc)->m_appear + 1.0f;
    Vec3f scale(x, 1.0f - y, z);
    fn_8040B790(&matrix, &scale);
    fn_8040B59C(&matrix, &(const Vec3f &)fn_80016284(npc));
    fn_8040C148(out, &matrix);
}

void fn_80073838(Npc *npc, float range) {
    STATE(npc)->m_contactRange = range;
}

float fn_8007382C(Npc *npc) {
    return STATE(npc)->m_contactRange;
}

void fn_80073820(Npc *npc, float height) {
    STATE(npc)->m_height = height;
}

float fn_80073814(Npc *npc) {
    return STATE(npc)->m_height;
}

/* The point below the NPC at its height, in its frame. */
Vec3f fn_8007378C(Npc *npc) {
    Mtx34f matrix;
    fn_8040C148(&matrix, reinterpret_cast<const Mtx34f *>(&npc->field_0x68[0x198 - 0x68]));
    fn_8040B848(&matrix, npc->m_rotation.y);
    fn_8040B884(&matrix, npc->m_rotation.x);
    fn_8040B80C(&matrix, npc->m_rotation.z);
    Vec3f below;
    fn_80008E1C(&below, 0.0f, -fn_80073814(npc), 0.0f);
    return fn_8040B67C(&matrix, &below);
}

/* Sets how close a player must come for the NPC to follow it (never for kind 13). */
void fn_80073770(Npc *npc, float range) {
    if (npc->m_kind == 13) {
        range = 0.0f;
    }
    STATE(npc)->m_followRange = range;
}

void fn_80073730(Npc *npc, bool following) {
    if (following) {
        fn_80014E1C(&STATE(npc)->m_flags.m_bits, 0);
    } else {
        fn_8006127C(&STATE(npc)->m_flags.m_bits, 0);
    }
}

/* Whether the NPC follows a player. */
bool fn_80073708(Npc *npc) {
    return fn_80014E34(&STATE(npc)->m_flags.m_bits, 0);
}

s32 fn_800736FC(Npc *npc) {
    return STATE(npc)->m_metCount;
}

/* Whether the NPC has met a player. */
bool fn_800736E4(Npc *npc) {
    return STATE(npc)->m_metCount > 0;
}

/* Sets the player the NPC follows. */
void fn_80073664(Npc *npc, const weak_ptr<PlayerBase> &leader) {
    STATE(npc)->m_leader = leader;
    fn_80014E1C(&STATE(npc)->m_flags.m_bits, 3);
}

/* The player the NPC follows. */
weak_ptr<PlayerBase> fn_8007363C(Npc *npc) {
    return STATE(npc)->m_leader;
}

} // extern "C"
