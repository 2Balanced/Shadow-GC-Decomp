/* Player::PathImpl (class name from the RTTI of its boost::shared_ptr control block): the path a
 * player follows (rails, circuits), behind UnkCircuitTarget_800ABCE8 (Game/UnkCircuitTarget_800ABCE8.h),
 * which keeps it in a shared_ptr and creates it the first time a player uses it. It holds a
 * PathCtrl, finds a path near the player through the path manager, moves along it and keeps the
 * position and the frame (forward and up) there. File name inferred from the class; other names
 * marked "(inferred)" are not original. Functions other units call by address keep their fn_ names
 * (C functions taking the target). Definitions are in reverse address order (deferred inlining
 * emits functions last-defined first). */
#include <boost/bind.hpp>
#include <boost/function.hpp>
#include <cstdio>
#include <memory>

#include "Game/CharInfo.h"
#include "Game/MathTypes.h"
#include "Game/PathCtrl.h"
#include "Game/Player.h"
#include "Game/UnkCircuitTarget_800ABCE8.h"
#include "boost/shared_ptr.hpp"
#include "boost/weak_ptr.hpp"
#include <dolphin/types.h>
#include "size_assert.h"

using SonicteamUSA::System::PathCtrl;

namespace SonicteamUSA {
namespace System {

/* A point of a path where it branches (class name from the RTTI of the bound callback; layout
 * unknown beyond the rotation at 0). */
class JointPathData {
  public:
    /* 0x0 */ Vec3f m_rotation; /* the direction of the branch as Euler angles (inferred) */
};

/* What the path passes to its branch callback (class name from the RTTI of the bound callback). */
class BranchCallBackParam {
  public:
    /* 0x0 */ JointPathData *m_joint; /* the joint reached (inferred) */
};

} // namespace System
} // namespace SonicteamUSA

using SonicteamUSA::System::BranchCallBackParam;
using SonicteamUSA::System::JointPathData;

/* The path manager fn_8001B494 returns (class name unknown; see Game/EggBalloonController.cpp). */
class UnkPathManager_8001B494;
/* The mission manager fn_8006D6A8 returns (class name unknown): the word at 0x20 is the mission
 * kind (inferred). */
struct UnkMission_8006D6A8 {
    /* 0x00 */ u8 field_0x0[0x20];
    /* 0x20 */ s32 m_kind;
};

/* A point of a path (class name unknown): its first word holds its flags (inferred). */
class UnkPathPoint_80016718 {
  public:
    /* 0x0 */ u32 m_flags;
};

/* An iterator over the branches of a joint (a list node pointer; type name unknown). */
struct UnkJointIterator_80016E18 {
    UnkJointIterator_80016E18(const UnkJointIterator_80016E18 &other) : m_node(other.m_node) {}
    bool operator!=(const UnkJointIterator_80016E18 &other) const {
        return m_node != other.m_node;
    }

    /* 0x0 */ u8 *m_node;
};

/* The player's members used here (struct name unknown; Player::PlayerBase leaves them unmapped). */
struct UnkPlayerFields_800ABDF8 {
    /* 0x000 */ Vec3f m_position;
    /* 0x00C */ u8 field_0xC[0xA8 - 0xC];
    /* 0x0A8 */ u32 m_stateBits[2];
    /* 0x0B0 */ u8 field_0xB0[0xB8 - 0xB0];
    /* 0x0B8 */ Vec3f m_velocity; /* (inferred) */
    /* 0x0C4 */ u8 field_0xC4[0x274 - 0xC4];
    /* 0x274 */ float m_radius; /* (inferred) */
};

#define Fields(player) reinterpret_cast<UnkPlayerFields_800ABDF8 *>(player)

typedef boost::function1<JointPathData *, BranchCallBackParam &> BranchCallBack;

extern "C" {
/* True while the path has no point (behaviour inferred). */
bool fn_80016980(PathCtrl *path);
UnkPathManager_8001B494 *fn_8001B494(void);
/* Looks for a path near position (filter selects the path kinds); fills path and direction
 * (behaviour inferred). */
bool fn_8001CDA0(UnkPathManager_8001B494 *manager, PathCtrl *path, const Vec3f *position,
                 const Vec3f *direction, int *outDirection, bool (*filter)(const u32 *flags),
                 int arg6, float radius);
/* Moves along the path by distance; false at its end (behaviour inferred). */
bool fn_8001C91C(UnkPathManager_8001B494 *manager, PathCtrl *path, int direction,
                 Vec3f *position, Vec3f *rotation, u8 arg5, float distance);
/* PathCtrl helpers (behaviour inferred). */
bool fn_80017E2C(PathCtrl *path);
bool fn_80018030(PathCtrl *path);
bool fn_80018150(PathCtrl *path, const Vec3f *position, const Vec3f *direction, int *outDirection,
                 float radius);
void fn_80018FFC(PathCtrl *path, PathCtrl *from);
JointPathData *fn_80016BB4(PathCtrl *path, int arg1, int arg2, float distance);
void fn_80018F1C(JointPathData *joint, PathCtrl *path, int *direction);
void fn_80019910(JointPathData *joint, int arg1);
/* The point the path is at (behaviour inferred). */
boost::shared_ptr<UnkPathPoint_80016718> fn_80016718(PathCtrl *path);
/* Joint helpers (behaviour inferred). */
JointPathData *fn_80016A34(JointPathData *joint);
JointPathData *fn_800169C8(JointPathData *joint);
JointPathData *fn_8001C5D8(JointPathData *joint, u32 mask, int arg2);
UnkJointIterator_80016E18 fn_80016E18(JointPathData *joint);
UnkJointIterator_80016E18 fn_80016D84(JointPathData *joint);
UnkJointIterator_80016E18 fn_80016DB8(UnkJointIterator_80016E18 *it, int);
UnkMission_8006D6A8 *fn_8006D6A8(void);
bool fn_8006CD78(u32 *bits, int bit);
/* The player's facing direction (behaviour inferred). */
Vec3f fn_800796DC(Player::PlayerBase *player);
/* Vector routines. */
Vec2f fn_8040F664(const Vec3f &rotation);
Vec3f fn_8040D0AC(const Vec3f *v, float angle);
Vec3f fn_8040D118(const Vec3f *v, float angle);
Vec3f fn_8040D044(const Vec3f *v, float angle);
float fn_8040CE74(Vec3f *vector);
float fn_8040CF90(const Vec3f *a, const Vec3f *b);
float fn_8040D1B4(const Vec3f *a, const Vec3f *b);
}

inline void AssignVector(Vec3f &to, const Vec3f &from) {
    fn_800091D0(&to, &from);
}

/* The ranges a path end is looked for in (names inferred). The game sets them in __sinit. */
inline float DynamicFloat(float value) {
    return value;
}
static float s_endRange = DynamicFloat(10.0f);
static float s_startRange = DynamicFloat(10.0f);
static float s_jointRange = DynamicFloat(600.0f);

/* A vector copied through fn_800091EC (the game's out-of-line copy; a Vec3f with copy
 * constructors, type name unknown). */
struct UnkPathVector_800ABE4C {
    UnkPathVector_800ABE4C(const Vec3f &other) {
        fn_800091EC(reinterpret_cast<Vec3f *>(this), &other);
    }
    UnkPathVector_800ABE4C(const UnkPathVector_800ABE4C &other) {
        fn_800091EC(reinterpret_cast<Vec3f *>(this), reinterpret_cast<const Vec3f *>(&other));
    }

    float x;
    float y;
    float z;
};

#define AS_VEC(v) (*reinterpret_cast<const Vec3f *>(&(v)))

/* A path search (struct name unknown; names inferred). */
struct UnkPathSearch_800ABE4C {
    UnkPathSearch_800ABE4C(const Vec3f &position, const Vec3f &direction, float radius);

    /* 0x00 */ UnkPathVector_800ABE4C m_position;
    /* 0x0C */ UnkPathVector_800ABE4C m_direction;
    /* 0x18 */ float m_radius;
    /* 0x1C */ u8 m_anyEnd;
};

/* The branch callback slot of a PathCtrl (at 0x44; name inferred). */
struct UnkBranchSlot_800AC16C {
    UnkBranchSlot_800AC16C &operator=(const BranchCallBack &callback) {
        m_callback = callback;
        return *this;
    }

    BranchCallBack m_callback;
};

inline void SetBranchCallBack(PathCtrl *path, const BranchCallBack &callback) {
    *reinterpret_cast<UnkBranchSlot_800AC16C *>(path->field_0x0 + 0x44) = callback;
}

/* The point the path is at: its flags (name inferred). */
inline u32 GetPointFlags(PathCtrl *path) {
    return fn_80016980(path) ? 0 : fn_80016718(path)->m_flags;
}

extern "C" bool fn_800ABDCC(PathCtrl *path);
extern "C" void fn_800AB8BC(UnkCircuitTarget_800ABCE8 *target,
                            const boost::shared_ptr<Player::PlayerBase> &player);

namespace Player {

class PathImpl {
  public:
    PathImpl(const boost::shared_ptr<PlayerBase> &player);
    ~PathImpl();

    void SetPath(PathCtrl *path, int direction);
    void Attach();
    int FindJoint();
    int FindNear(const Vec3f &position, const Vec3f &direction, u8 anyEnd, float radius);
    int Find();
    bool Restart();
    UnkPathSearch_800ABE4C MakeSearch(const boost::shared_ptr<PlayerBase> &player);
    UnkPathSearch_800ABE4C MakeSearch(const Vec3f &position, const Vec3f &direction, float radius);
    void Reattach();
    bool Advance(float distance);
    void GetFrame(Vec3f *position, Vec3f *forward, Vec3f *up);
    void SetFrame(const Vec3f &position, const Vec3f &rotation);
    JointPathData *ChooseBranch(JointPathData *joint);
    JointPathData *ChooseBranch(JointPathData *joint, int arg);
    JointPathData *OnBranch(BranchCallBackParam &param);
    int Search(const UnkPathSearch_800ABE4C &search);

    int GetState() {
        return m_state;
    }
    void SetLoose(bool loose) {
        m_loose = loose;
    }

    /* 0x00 */ boost::shared_ptr<PlayerBase> m_player;
    /* 0x08 */ Vec3f m_position;
    /* 0x14 */ Vec3f m_forward;
    /* 0x20 */ Vec3f m_up;
    /* 0x2C */ int m_state; /* what the search found (0 none, 1..5; inferred) */
    /* 0x30 */ u8 m_loose;
    /* 0x31 */ u8 field_0x31[3]; /* (PathCtrl.h declares no alignment for PathCtrl) */
    /* 0x34 */ PathCtrl m_path;
    /* 0x8C */ int m_direction;
};
SIZE_ASSERT(PathImpl, 0x90);

/* Deletes a path (name inferred; see checked_delete below). */
inline void DeletePath(PathImpl *path) {
    delete path;
}

} // namespace Player

/* Retail keeps boost::checked_delete<PathImpl> out of line (0x800ABA4C) and ~PathImpl at its own
 * place; this specialization calls an inline helper, which gives retail's code and order (see
 * docs/research-notes.md and Game/Rocket.cpp). */
namespace boost {
template<> inline void checked_delete<Player::PathImpl>(Player::PathImpl *x) {
    Player::DeletePath(x);
}
} // namespace boost

namespace Player {

PathImpl::PathImpl(const boost::shared_ptr<PlayerBase> &player) : m_player(player), m_loose(0) {}

PathImpl::~PathImpl() {}

void PathImpl::SetPath(PathCtrl *path, int direction) {
    fn_80018FFC(&m_path, path);
    m_direction = direction;
}

void PathImpl::Attach() {
    if (fn_800ABDCC(&m_path)) {
        SetBranchCallBack(&m_path, boost::bind(&PathImpl::OnBranch, this, _1));
    }
}

/* The path filters fn_8001CDA0 takes (names inferred). */
static bool IsSearchPath(const u32 *flags) {
    return (*flags & 0x88300) != 0;
}

static bool IsJointPath(const u32 *flags) {
    return (*flags & 0x6000) != 0;
}

int PathImpl::FindJoint() {
    UnkPathManager_8001B494 *manager = fn_8001B494();
    m_state = 0;
    Vec3f direction(0.0f, 0.0f, -1.0f);
    if (fn_8001CDA0(manager, &m_path, &m_player->m_position, &direction, &m_direction, IsJointPath, 0,
                    0.0f)) {
        if (fn_80017E2C(&m_path)) {
            return 0;
        }
        JointPathData *joint = fn_80016BB4(&m_path, 1, 0, s_jointRange);
        if (joint != 0) {
            JointPathData *branch = ChooseBranch(joint, 1);
            if (branch != 0) {
                fn_80018F1C(branch, &m_path, &m_direction);
            }
            fn_80019910(joint, 1);
        }
        Advance(0.0f);
        m_direction = 0;
        m_state = 5;
        SetBranchCallBack(&m_path, boost::bind(&PathImpl::OnBranch, this, _1));
        return 1;
    }
    return 0;
}

int PathImpl::FindNear(const Vec3f &position, const Vec3f &direction, u8 anyEnd, float radius) {
    UnkPathSearch_800ABE4C search = MakeSearch(position, direction, radius);
    search.m_anyEnd = anyEnd;
    return Search(search);
}

int PathImpl::Find() {
    UnkPathSearch_800ABE4C search = MakeSearch(m_player);
    return Search(search);
}

bool PathImpl::Restart() {
    UnkPathSearch_800ABE4C search = MakeSearch(m_player);
    int direction;
    if (fn_800ABDCC(&m_path) &&
        fn_80018150(&m_path, &AS_VEC(search.m_position), &AS_VEC(search.m_direction), &direction, search.m_radius)) {
        Advance(0.0f);
        return true;
    }
    return false;
}

void PathImpl::Reattach() {
    UnkPathSearch_800ABE4C search = MakeSearch(m_player);
    if (fn_800ABDCC(&m_path) &&
        fn_80018150(&m_path, &AS_VEC(search.m_position), &AS_VEC(search.m_direction), &m_direction, search.m_radius)) {
        Advance(0.0f);
    }
}

bool PathImpl::Advance(float distance) {
    UnkPathManager_8001B494 *manager = fn_8001B494();
    Vec3f position;
    Vec3f rotation(0.0f, 0.0f, 0.0f);
    bool end = false;
    if (fn_800ABDCC(&m_path)) {
        end = !fn_8001C91C(manager, &m_path, m_direction, &position, &rotation, m_loose, distance);
        SetFrame(position, rotation);
    }
    return end;
}

void PathImpl::GetFrame(Vec3f *position, Vec3f *forward, Vec3f *up) {
    if (fn_800ABDCC(&m_path)) {
        if (position != 0) {
            AssignVector(*position, m_position);
        }
        if (up != 0) {
            AssignVector(*up, m_up);
        }
        if (forward != 0) {
            AssignVector(*forward, m_forward);
        }
    }
}

void PathImpl::SetFrame(const Vec3f &position, const Vec3f &rotation) {
    AssignVector(m_position, position);
    fn_800091D0(&m_up, &(const Vec3f &)Vec3f(0.0f, 1.0f, 0.0f));
    AssignVector(m_up, fn_8040D0AC(&m_up, rotation.y));
    AssignVector(m_up, fn_8040D118(&m_up, rotation.x));
    AssignVector(m_up, fn_8040D044(&m_up, rotation.z));
    fn_800091D0(&m_forward, &(const Vec3f &)Vec3f(0.0f, 0.0f, m_direction == 1 ? 1.0f : -1.0f));
    AssignVector(m_forward, fn_8040D0AC(&m_forward, rotation.y));
    AssignVector(m_forward, fn_8040D118(&m_forward, rotation.x));
    AssignVector(m_forward, fn_8040D044(&m_forward, rotation.z));
}

JointPathData *PathImpl::ChooseBranch(JointPathData *joint) {
    float best = -100.0f;
    Vec3f direction;
    JointPathData *result = 0;
    fn_800091EC(&direction, &Fields(m_player.get())->m_velocity);
    if (direction.x * direction.x + direction.y * direction.y + direction.z * direction.z < 0.0001f) {
        fn_800091D0(&direction, &(const Vec3f &)Vec3f(0.0f, 0.0f, 1.0f));
    } else {
        Vec2f angles = fn_8040F664(m_forward);
        AssignVector(direction, fn_8040D118(&direction, -angles.x));
        AssignVector(direction, fn_8040D0AC(&direction, -angles.y));
        fn_8040CE74(&direction);
    }
    for (UnkJointIterator_80016E18 it = fn_80016E18(joint); it != fn_80016D84(joint);
         fn_80016DB8(&it, 0)) {
        JointPathData *branch = reinterpret_cast<JointPathData *>(it.m_node + 8);
        float y = branch->m_rotation.y;
        float x = branch->m_rotation.x;
        float z = branch->m_rotation.z;
        Vec3f forward(0.0f, 0.0f, 1.0f);
        AssignVector(forward, fn_8040D0AC(&forward, y));
        AssignVector(forward, fn_8040D118(&forward, x));
        AssignVector(forward, fn_8040D044(&forward, z));
        if (!(forward.z < -0.12186934f)) {
            float dot = fn_8040CF90(&forward, &direction);
            if (best < dot) {
                best = dot;
                result = branch;
            }
        }
    }
    return result;
}

JointPathData *PathImpl::ChooseBranch(JointPathData *joint, int arg) {
    u32 mask;
    switch (fn_8006D6A8()->m_kind) {
    case 0:
        mask = 0x40000;
        break;
    case 2:
        mask = 0x20000;
        break;
    default:
        mask = 0;
        break;
    }
    return fn_8001C5D8(joint, mask, arg);
}

JointPathData *PathImpl::OnBranch(BranchCallBackParam &param) {
    JointPathData *branch;
    if (fn_8006CD78(m_player->m_stateBits, 0x14)) {
        branch = ChooseBranch(param.m_joint, 0);
    } else {
        branch = ChooseBranch(param.m_joint);
    }
    if (branch == 0 && fn_80016A34(param.m_joint) != 0) {
        branch = fn_800169C8(param.m_joint);
    }
    return branch;
}

int PathImpl::Search(const UnkPathSearch_800ABE4C &search) {
    if (fn_8001CDA0(fn_8001B494(), &m_path, &AS_VEC(search.m_position), &AS_VEC(search.m_direction), &m_direction,
                    IsSearchPath, 0, search.m_radius)) {
        if (search.m_anyEnd || !(fn_80017E2C(&m_path) | fn_80018030(&m_path))) {
            Advance(0.0f);
            SetBranchCallBack(&m_path, boost::bind(&PathImpl::OnBranch, this, _1));
            u32 flags = GetPointFlags(&m_path);
            if (flags & 0x80200) {
                Vec3f position;
                GetFrame(&position, 0, 0);
                if (search.m_anyEnd || fn_8040D1B4(&position, &AS_VEC(search.m_position)) < s_endRange) {
                    if (flags & 0x80000) {
                        m_state = 3;
                    } else {
                        m_state = 2;
                    }
                    return m_state;
                }
            }
            if (flags & 0x100) {
                m_state = 1;
                return m_state;
            }
            if (flags & 0x8000) {
                Vec3f position;
                GetFrame(&position, 0, 0);
                if (search.m_anyEnd || fn_8040D1B4(&position, &AS_VEC(search.m_position)) < s_startRange) {
                    m_state = 4;
                    return m_state;
                }
            }
        }
    }
    m_state = 0;
    return m_state;
}

UnkPathSearch_800ABE4C PathImpl::MakeSearch(const Vec3f &position, const Vec3f &direction,
                                            float radius) {
    return UnkPathSearch_800ABE4C(position, direction, radius);
}

} // namespace Player

UnkPathSearch_800ABE4C::UnkPathSearch_800ABE4C(const Vec3f &position, const Vec3f &direction,
                                               float radius)
    : m_position(position), m_direction(direction) {
    m_radius = radius;
    m_anyEnd = 0;
}

namespace Player {

UnkPathSearch_800ABE4C PathImpl::MakeSearch(const boost::shared_ptr<PlayerBase> &player) {
    PlayerBase *chara = player.get();
    return UnkPathSearch_800ABE4C(chara->m_position, fn_800796DC(chara), 40.0f + chara->field_0x274);
}

} // namespace Player

extern "C" bool fn_800ABDCC(PathCtrl *path) {
    return !fn_80016980(path);
}

UnkCircuitTarget_800ABCE8::UnkCircuitTarget_800ABCE8() {}

UnkCircuitTarget_800ABCE8::UnkCircuitTarget_800ABCE8(const boost::weak_ptr<CharInfo> &character,
                                                     PathCtrl *path, int direction) {
    boost::shared_ptr<Player::PlayerBase> player =
        boost::dynamic_pointer_cast<Player::PlayerBase>(character.lock());
    if (player) {
        fn_800AB8BC(this, player);
        m_object->SetPath(path, direction);
        m_object->Attach();
    }
}

UnkCircuitTarget_800ABCE8::~UnkCircuitTarget_800ABCE8() {}

extern "C" {

int fn_800ABC68(UnkCircuitTarget_800ABCE8 *target,
                const boost::shared_ptr<Player::PlayerBase> &player) {
    fn_800AB8BC(target, player);
    return target->m_object->FindJoint();
}

int fn_800ABC0C(UnkCircuitTarget_800ABCE8 *target,
                const boost::shared_ptr<Player::PlayerBase> &player, const Vec3f *position,
                const Vec3f *direction, u8 anyEnd, float radius) {
    fn_800AB8BC(target, player);
    return target->m_object->FindNear(*position, *direction, anyEnd, radius);
}

int fn_800ABBD8(UnkCircuitTarget_800ABCE8 *target,
                const boost::shared_ptr<Player::PlayerBase> &player) {
    fn_800AB8BC(target, player);
    return target->m_object->Find();
}

bool fn_800ABBA4(UnkCircuitTarget_800ABCE8 *target,
                 const boost::shared_ptr<Player::PlayerBase> &player) {
    fn_800AB8BC(target, player);
    return target->m_object->Restart();
}

void fn_800ABB80(UnkCircuitTarget_800ABCE8 *target) {
    target->m_object->Reattach();
}

int fn_800ABB48(UnkCircuitTarget_800ABCE8 *target) {
    if (target->m_object) {
        return target->m_object->GetState();
    }
    return 0;
}

bool fn_800ABB14(UnkCircuitTarget_800ABCE8 *target, float distance) {
    if (target->m_object) {
        return target->m_object->Advance(distance);
    }
    return false;
}

void fn_800ABAE8(const UnkCircuitTarget_800ABCE8 *target, Vec3f *position, Vec3f *forward,
                 Vec3f *up) {
    if (target->m_object) {
        target->m_object->GetFrame(position, forward, up);
    }
}

void fn_800ABAA4(UnkCircuitTarget_800ABCE8 *target, bool loose) {
    if (target->m_object) {
        target->m_object->SetLoose(loose);
    }
}

/* Creates the path the first time a player uses it (name inferred). */
void fn_800AB8BC(UnkCircuitTarget_800ABCE8 *target,
                 const boost::shared_ptr<Player::PlayerBase> &player) {
    if (!target->m_object) {
        target->m_object = boost::shared_ptr<Player::PathImpl>(new Player::PathImpl(player));
    }
}

} // extern "C"

/* Not called anywhere: the linker strips it. The original pools the frame's literals (0, 1, -1)
 * and the end range (10) ahead of the other float literals, in source order; GC/2.7 pools them in
 * emission order, and compiling this function last puts them first (see docs/research-notes.md,
 * "the order of float constants in .sdata2"). */
void PlayerPathLiteralOrder(float *out) {
    out[0] = 0.0f;
    out[1] = 1.0f;
    out[2] = -1.0f;
    out[3] = 10.0f;
}
