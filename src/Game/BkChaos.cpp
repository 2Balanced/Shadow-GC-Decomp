/* BkChaos (namespace and class names from RTTI; its task's s_taskName is "BkChaos"): the Black
 * Arms chaos, BK_CHAOS in the stage editor (enemy/BkChaosData.one). Its body BkChaos::BkChaos is an
 * EnemyFlyerCommon (Game/BkChaos.h) made of ChibiNum small chaos, BkChaos::BkChibiChaos: with
 * ShapeType CHIBI they fly about the body's home and gather into it after CombineStartTime (never
 * when it is negative), with ShapeType OYA they start gathered. Gathered, the body grows with the
 * chibis in it and attacks; a hit or the vacuum lets them go again, and each chibi can be hit on
 * its own. Its AI is BkChaosAI (Game/BkChaosAI.cpp). File name inferred from the class. Names
 * marked "(name inferred)" and all member names are not original. Definitions are in reverse
 * address order (deferred inlining emits functions last-defined first). The unit's code reaches
 * its .data through a pooled base address, so the linker keeps that section whole:
 * Game/CharaColliBaseCharInfo.h and Game/TNumEdit.h supply the RTTI base lists the .data starts
 * with (as in BkSoldier.cpp).
 *
 * NonMatching: 133 of 135 functions match. Left: StretchCallback (15 rows: retail keeps the
 * callback's parameter in r31 and the node index in r27 and tests the index unsigned, GC/2.7 swaps
 * the two registers; it also loads the pitch straight into f29 where GC/2.7 goes through f4) and
 * BkChibiChaos::Update (2 rows: case 1 sets up r5, r3, r4 for the subtraction where GC/2.7 has r4,
 * r5, r3; about 25 spellings were tried, and in wave 40 Subtract with pointer or mixed parameters,
 * its operands swapped, the position through a pointer or an inline helper and the character
 * pointer in a local, probes/Game_BkChaos_R3.py and _R6.py). The upper height test matches since
 * wave 40: the position's height is read through the inline PositionY. For StretchCallback,
 * `u32 index = param->m_id != 0x1774;` gives retail's registers and its unsigned test but adds a
 * clrlwi (bool to int); the ternary in u32 keeps subfe without it but swaps the registers back
 * (probes/Game_BkChaos_R4.py, _R5.py). Retail also pools 2.0 ahead of Update's constants in
 * .sdata2.
 *
 * What matched the other four (wave 39): vfStat_0x98 takes its attacker through a non-const
 * reference and GetChibiLife is defined over a const chaos - GC/2.7 moves a load above the
 * prologue's stores exactly when the object it reads is const; vfSet_0x20 passes its three
 * values to an inline function (arguments are evaluated right to left); vfDisp_0x40 names the
 * cylinder's two constants; StretchCallback reads field_0x2FC as volatile for its test, because
 * retail loads it again for the decrement. */
#define BK_CHAOS_UNIT
/* See EnemyBaseAI (Game/EnemyAI.h): the area radius helper, fn_8018346C. */
#define ENEMY_BASE_AI_HELPERS

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <string>

/* CharaColliBaseCharInfo.h before TNumEdit.h: the order of the casts sets the order of the RTTI
 * base lists in .data (as in BkLarva.cpp). */
#include "Game/CharaColliBaseCharInfo.h"
#include "Game/TNumEdit.h"

#include "Game/CharInfo.h"
#include "Game/CharCommand.h"
#include "Game/CharaColli.h"
#include "Game/CharaColliAttack.h"
#include "Game/EnemyCharInfo.h"
#include "Game/EnemyRender.h"
#include "Game/Gadget.h"
#include "Game/GadgetClassDesc.h"
#include "Game/GadgetSound.h"
#include "Game/MathConstants.h"
#include "Game/MathTypes.h"
#include "Game/Random.h"
#include "Game/ResourceOneFile.h"
#include "Game/BossDef.h"
#include "Game/DebugDisp.h"
#include "Effect/System/EffectElement.h"
#include "boost/shared_ptr.hpp"
#include "boost/weak_ptr.hpp"
#include <boost/function.hpp>
/* After the shared_ptr headers: retail emits the instances of shared_ptr's function templates
 * before those of boost::bind (as in ThrowBomb.cpp). */
#include <boost/bind.hpp>

/* The links of a node of a list of pointers; the node keeps its value after them (see
 * Game/BulletManagerTask.cpp and Game/ElecSecurityCode.h, which declare the same list). */
struct UnkPtrListLink_8001B5B8 {
    /* 0x0 */ UnkPtrListLink_8001B5B8 *prev;
    /* 0x4 */ UnkPtrListLink_8001B5B8 *next;
};

/* A position in the list implementation below. */
struct UnkPtrListPosition_8001B674 {
    UnkPtrListPosition_8001B674(const UnkPtrListPosition_8001B674 &other) : node(other.node) {}

    /* 0x0 */ UnkPtrListLink_8001B5B8 *node;
};

/* The list implementation the game's lists of pointers share. */
struct UnkPtrList_8001B5B8 {
    /* 0x0 */ u32 m_size;
    /* 0x4 */ UnkPtrListLink_8001B5B8 m_end;
};

extern "C" {
/* The list implementation's constructor and destructor, its first and its end position, its
 * size and first value, removing and appending a value (see Game/BulletManagerTask.cpp). */
void fn_8001BE04(UnkPtrList_8001B5B8 *list);
void fn_8001B5B8(UnkPtrList_8001B5B8 *list, short flag);
UnkPtrListPosition_8001B674 fn_8001B6B0(UnkPtrList_8001B5B8 *list);
UnkPtrListPosition_8001B674 fn_8001B674(UnkPtrList_8001B5B8 *list);
u32 fn_80058F00(UnkPtrList_8001B5B8 *list);
void *&fn_80058F48(UnkPtrList_8001B5B8 *list);
void fn_8001B42C(UnkPtrList_8001B5B8 *list, void *const &value);
void fn_8001D820(UnkPtrList_8001B5B8 *list, void *const &value);
}

namespace BkChaos {

class BkChibiChaos;

/* Keeps the inline function that calls it out of line, as the list's members are. */
inline void ChibiListKeep() {}

/* The chibis of a chaos: the game's list of pointers holding BkChibiChaos pointers. Its members
 * are inline and emitted after their users, as the game's list template's are (names inferred). */
class ChibiList : public UnkPtrList_8001B5B8 {
  public:
    struct Node : public UnkPtrListLink_8001B5B8 {
        /* 0x8 */ BkChibiChaos *value;
    };

    class iterator {
      public:
        iterator(const iterator &other) : m_node(other.m_node) {
            ChibiListKeep();
        }
        iterator(const UnkPtrListPosition_8001B674 &position) : m_node(position.node) {
            ChibiListKeep();
        }

        BkChibiChaos *&operator*() const {
            ChibiListKeep();
            return static_cast<Node *>(m_node)->value;
        }
        iterator &operator++() {
            ChibiListKeep();
            m_node = m_node->next;
            return *this;
        }
        iterator operator++(int) {
            iterator old(*this);
            m_node = m_node->next;
            return old;
        }
        bool operator!=(const iterator &other) const {
            ChibiListKeep();
            return m_node != other.m_node;
        }

        /* 0x0 */ UnkPtrListLink_8001B5B8 *m_node;
    };

    ChibiList() {
        ChibiListKeep();
        fn_8001BE04(this);
    }
    ~ChibiList() {
        ChibiListKeep();
        fn_8001B5B8(this, 0);
    }

    iterator begin() {
        return iterator(fn_8001B6B0(this));
    }
    iterator end() {
        return iterator(fn_8001B674(this));
    }
    u32 size() {
        return fn_80058F00(this);
    }
    BkChibiChaos *&front() {
        ChibiListKeep();
        return reinterpret_cast<BkChibiChaos *&>(fn_80058F48(this));
    }
    void push_back(BkChibiChaos *const &value) {
        ChibiListKeep();
        fn_8001D820(this, reinterpret_cast<void *const &>(value));
    }
    void remove(BkChibiChaos *const &value) {
        ChibiListKeep();
        fn_8001B42C(this, reinterpret_cast<void *const &>(value));
    }
};

} // namespace BkChaos

#include "Game/BkChaos.h"

using Effect::System::EffectElement;
using SonicteamUSA::System::CharaColli::AttackCallback;
using SonicteamUSA::System::CharaColli::AttackCallbackParam;
using SonicteamUSA::System::CharaColli::CharaColliAttack;
using SonicteamUSA::System::CharaColli::CharaColliBase;

/* MSL's std::fabs for floats, which the game calls out of line (as in BkSoldier.cpp). */
namespace std {
float fabs(float x);
}

/* The cursor a parameter's label callback receives (layout as in Helicopter.cpp; the value of a
 * float parameter is at 0xC). */
class UnkLabelQuery_800CBC20 {
  public:
    /* 0x0 */ const GadgetParamDesc *params;
    /* 0x4 */ s32 index; /* the parameter being shown */
    /* 0x8 */ s32 value; /* its current value */
    /* 0xC */ float fvalue;
};

/* Target (class name from RTTI; see EnemyTarget.cpp). */
class Target {
  public:
    void fn_800CCE38(const Vec3f *position);

    void Activate() {
        m_flags |= 1;
    }
    void Deactivate() {
        m_flags &= ~1;
    }

    /* 0x00 */ u8 field_0x0[0x30];
    /* 0x30 */ u32 m_flags;
};

/* What a motion passes to its node callbacks (layout as in BlackBull0412ModeCtrlUnit.cpp). */
struct _stMtnCallbackParam {
    /* 0x0 */ void *m_frame; /* the node's matrix */
    /* 0x4 */ void *m_data;
    /* 0x8 */ s32 m_id;
};
struct _stMtnPlayWork;

/* The matrix stack (as in MotionPlayWork.cpp). */
struct MotionMatrixStack {
    /* 0x0 */ Mtx34f *m_matrices;
    /* 0x4 */ s32 m_size;
    /* 0x8 */ s32 m_top;
};
extern MotionMatrixStack *lbl_805F13A8;

class UnkUVAnimManager_801EB994;
UnkUVAnimManager_801EB994 *GetUVAnimManager();

/* a - b (0x8000AD64, Game/CameraAdjust.cpp). */
Vec3f operator-(const Vec3f &a, const Vec3f &b);

extern "C" {
/* The enemies' shared helpers (behaviour inferred). */
void fn_8019AA40(Enemy *enemy, u16 sound, int arg);
UnkSoundHandle_801D3DC8 fn_8019A908(Enemy *enemy, u16 sound, int arg);
/* The distance from the enemy to a point, and the enemy's centre (behaviours inferred). */
float fn_8019B298(Enemy *enemy, const Vec3f *point);
Vec3f *fn_80198BD8(Enemy *enemy);
/* Resources and models (see BkSoldier.cpp). */
void *fn_80012684(void *loader, int slot, int kind);
void *fn_80012608(void *loader, int slot, int kind);
void *fn_8001277C(void *loader, int slot, int kind);
boost::shared_ptr<EnemyRender> fn_802CDB08(int slot, const EnemyMotionTable &table, void *model,
                                           void *motion);
boost::shared_ptr<EnemyRender> fn_802CD8E4(int slot, const EnemyMotionTable &table);
/* The model: its motion work, a node's matrix, hiding and showing it, its fade, placing it and
 * its update (behaviours inferred). */
_stMtnPlayWork *fn_802CD420(EnemyRender *render);
Mtx34f *fn_802CD560(EnemyRender *render, int node);
void fn_802CD5CC(EnemyRender *render);
void fn_802CD5F0(EnemyRender *render);
void fn_802CD638(EnemyRender *render, float alpha);
void fn_802CD768(EnemyRender *render, const Vec3f *position, const Vec3f *rotation);
void fn_802CD7E8(EnemyRender *render);
/* A motion's node callbacks (see BossCallbackContainer.cpp). */
void fn_80415808(_stMtnPlayWork *work, int id, void (*callback)(_stMtnCallbackParam *), void *data);
Mtx34f *fn_80415E70(_stMtnPlayWork *work, int node);
/* The matrix of a node, and back (see BkSoldier.cpp). */
void fn_80016370(Mtx34f *matrix, void *node);
void fn_800163F4(void *node, const Mtx34f *matrix);
/* The UV animations (see EggMecaCtrlUnit.cpp). */
void fn_801EB660(UnkUVAnimManager_801EB994 *manager, const std::string &name, void *data);
void fn_801EB550(UnkUVAnimManager_801EB994 *manager, const std::string &name);
/* The kind of a model's motion (behaviour inferred). */
u16 fn_801A4494(void *model);
/* The collider of the status: the attack collider's shape and its size (see
 * EnemyStatusCommon.cpp). */
void fn_801A9540(EnemyCharaColliModule *module, int part);
void fn_801A9508(EnemyCharaColliModule *module, int part);
void fn_801A95B4(EnemyCharaColliModule *module, float size);
/* Effects at a matrix and a position (see BkWorm.cpp). */
boost::weak_ptr<EffectElement> fn_801A1ED8(const Mtx34f *matrix, int id);
boost::weak_ptr<EffectElement> fn_801A1E48(const Mtx34f *matrix, int id);
boost::weak_ptr<EffectElement> fn_801A1F1C(const Vec3f *position, int id);
/* The collider manager, a collider of the shapes of an attack table, its attack collision and
 * the callbacks (see EggPierrot.cpp). */
void *fn_80021B08(void);
void *fn_8019A890(int slot, int index);
boost::shared_ptr<CharaColliBase> fn_800C81F4(void *manager, boost::weak_ptr<CharInfo> info,
                                              const void *shapes, int type, u16 typeArg);
boost::weak_ptr<CharaColliAttack> fn_800C4374(void *manager,
                                              boost::shared_ptr<CharaColliBase> collider);
void fn_80054DD0(CharaColliAttack *attack, int arg1, int arg2, float arg3, const AttackCallback &onHit,
                 const AttackCallback &onEnd);
void fn_80096B64(CharaColliAttack *attack, int arg1, const AttackCallback &callback);
/* Targets (see BKTank.cpp). */
boost::shared_ptr<Target> fn_801E7128(const boost::weak_ptr<CharInfo> &owner, int flags);
/* Sounds (see ThrowBomb.cpp). */
void *fn_80053164(void);
void fn_801D54F4(void *manager, int id, const CharInfo *source, int arg3, int arg4);
/* Whether the attacker was the player (see BkLarva.cpp). */
bool fn_8021C958(const boost::shared_ptr<CharInfo> &attacker);
/* The vacuum command's owner, and the vacuum taking the enemy's model (see BkLarva.cpp). */
boost::weak_ptr<CharInfo> fn_801DADFC(VacuumCommand *command);
void fn_8028CFC4(VacuumCommand *command, const boost::shared_ptr<EnemyRender> &render,
                 const Vec3f *position, const Vec3f *rotation, int arg4);
/* The debug print. */
void fn_803A953C(const char *format, ...);
/* The smaller of two floats, and whether a float is zero (behaviours inferred). */
const float &fn_800704AC(const float &a, const float &b);
bool fn_8000B044(float value);
/* Moves position by velocity and velocity by acceleration (behaviour inferred). */
void fn_804103E8(Vec3f *position, Vec3f *velocity, const Vec3f *acceleration, float drag,
                 float dt);
/* Moves an angle toward a target (behaviour inferred). */
float fn_8040F314(float value, float target, float step);
/* The angles of a direction (behaviour inferred). */
Vec2f fn_8040F760(const Vec3f *direction);
/* Vector and matrix routines (see BkLarva.cpp and Cage.cpp). */
Vec3f fn_8040CDD0(const Vec3f *a, const Vec3f *b);
Vec3f fn_8040CDA8(const Vec3f *v, float scale);
Vec3f fn_800091A4(const Vec3f *v, float scale);
Vec3f fn_8040CEE8(const Vec3f *v);
Vec3f fn_8040D0AC(const Vec3f *v, float angle);
void fn_8040CE38(Vec3f *v, float scale);
void fn_8040CE60(Vec3f *vector);
void fn_8040CFF8(Vec3f *v, const Vec3f *value);
void fn_8040E970(Vec3f *vector);
Vec3f fn_800496F8(const Mtx34f *matrix);
Vec3f fn_8040B67C(const Mtx34f *matrix, const Vec3f *point);
Mtx34f fn_8040BEF8(const Mtx34f *matrix);
void fn_8040B790(Mtx34f *matrix, const Vec3f *scale);
void fn_8040BAC4(Mtx34f *matrix, const Mtx34f *other);
void fn_804114D0(Vec3f *translation, const Mtx34f *matrix);
}

/* Turns current toward target by at most step (0x8012773C; see Game/AngleUtil.h, which this unit
 * does not include: its inline copy would be emitted here). */
float TurnAngleToward(float target, float current, float step);

/* fn_8040CD74 called as returning a - b by value (see ArkCannon.cpp). */
typedef Vec3f (*VectorSubtractFn)(const Vec3f *a, const Vec3f *b);
/* fn_800496F8 called as returning a vector the game copies out of line (not original: the game's
 * vector class has its copy constructor out of line). */
typedef Fn8040CopyVector3 (*MatrixTranslationFn)(const Mtx34f *matrix);

/* a - b through fn_8040CD74 (name inferred). Inline: its arguments are evaluated before the
 * statement that calls it, as retail has them. */
inline Vec3f Subtract(const Vec3f &a, const Vec3f &b) {
    return ((VectorSubtractFn)fn_8040CD74)(&a, &b);
}

/* The vector type's out-of-line assignment and addition (names inferred; as in BkLarva.cpp). */
inline void AssignVector(Vec3f &dst, const Vec3f &src) {
    fn_800091D0(&dst, &src);
}
inline void AddVector(Vec3f &dst, const Vec3f &value) {
    fn_8040CFF8(&dst, &value);
}
inline void ScaleVector(Vec3f &v, float scale) {
    fn_8040CE38(&v, scale);
}

/* v / scale (not original). */
inline Vec3f operator/(const Vec3f &v, float scale) {
    return fn_8040CDA8(&v, 1.0f / scale);
}

/* Degrees to radians (name inferred; as in BalloonGenerator.cpp). */
inline float DegToRad(float degrees) {
    return degrees * (lbl_805FAD38 / 180.0f);
}

/* A motion table built in place (not original; as in BkWorm.cpp). */
struct MotionTable : public EnemyMotionTable {
    MotionTable(const EnemyMotionEntry *e, s32 c) {
        entries = e;
        count = c;
    }
};

/* The chaos's home and the time its chibis take to gather (names inferred; inline, so they are
 * read before the statements that use them, as retail has it). */
inline Vec3f *GetHome(Enemy *enemy) {
    return &enemy->Enemy::field_0x48;
}

/* Three floats copied as a plain struct (not original; as in BkGiant.cpp). */
struct Vec3fWords {
    float x;
    float y;
    float z;
};

/* The vectors an attack passes (offsets from OnHit; names inferred; as in BkLarva.cpp). */
#define HIT_POSITION(param) (*reinterpret_cast<Vec3f *>(&(param).field_0x8[0x18 - 0x8]))
#define HIT_DIRECTION(param) (*reinterpret_cast<Vec3f *>(&(param).field_0x8[0x34 - 0x8]))

/* A value (name inferred). As a call it is not a constant expression, so the range below is
 * stored by a static initializer, as in retail (as in VehicleArrow.cpp). */
inline float DynamicFloat(float value) {
    return value;
}

/* How close the target has to be for the rolling attack, by the chaos's size (name inferred). */
static const float s_rollRange = DynamicFloat(50.0f);

namespace BkChaos {

/* BkChaos::BkChibiChaos (class name from the RTTI of its callback binders): one of the small chaos
 * a chaos is made of. Its state: 0 flying about the body's home, 1 flying into the body, 2 flying
 * out of it, 3 in it, 4 hit (names inferred). */
class BkChibiChaos {
  public:
    BkChibiChaos(BkChaos *owner);
    ~BkChibiChaos();

    Fn8040CopyVector3 RandomPoint();
    void Scatter();
    void Update(float dt);
    void OnHit(AttackCallbackParam &param);
    void OnAttacked(AttackCallbackParam &param);
    void OnCommand(CharCommand &command);
    void OnHitEnd(AttackCallbackParam &param) {}

    /* 0x00 */ s32 m_state;
    /* 0x04 */ u32 field_0x4;
    /* 0x08 */ BkChaos *m_owner;
    /* 0x0C */ boost::shared_ptr<UnkEnemyInfo_8017D014> m_info;
    /* 0x14 */ boost::shared_ptr<CharaColliBase> m_collider;
    /* 0x1C */ boost::shared_ptr<EnemyRender> m_render;
    /* 0x24 */ boost::shared_ptr<Target> m_target;
    /* 0x2C */ u32 m_flags; /* 1 leave the body, 2 gather, 4 gone */
    /* 0x30 */ Vec3f m_offset;
};
SIZE_ASSERT(BkChibiChaos, 0x3C);

/* BkChaos::BkChaosTask (class name from RTTI; vtable 0x805564A4): the task that runs a chaos. */
class BkChaosTask : public EnemyTemplate<BkChaos, BkChaosAI> {
  public:
    BkChaosTask(Task *parent, UnkPlacement_800C9CD0 *placement);
    virtual ~BkChaosTask();

    /* The stage editor's hooks (names inferred). */
    static void Instantiate(UnkPlacement_800C9CD0 *placement);
    static void LoadFiles();
    static void DropFiles();

    static const char *s_taskName;
};
SIZE_ASSERT(BkChaosTask, 0x3F8);

} // namespace BkChaos

/* The motion tables (steps in .sdata2 and .rodata). */
static const MotionSequenceStep lbl_805F88C0[] = {{3, -1, -1, 10, 1}};
static const MotionSequenceStep lbl_805F88C8[] = {{3, -1, -1, 10, 0}};
static const MotionSequenceStep lbl_805F88D0[] = {{2, -1, -1, 10, 0}};
static const MotionSequenceStep lbl_805F88D8[] = {{1, -1, -1, 10, 0}};
static const MotionSequence lbl_804FD558[] = {
    {lbl_805F88C0, 1, -3}, {lbl_805F88C8, 1, -3}, {lbl_805F88D0, 1, -3}, {lbl_805F88D8, 1, -3},
};
static const MotionSequenceTable lbl_805F88E0 = {lbl_804FD558, 4};

/* The chibi's model's motion table. */
static const EnemyMotionEntry lbl_804FD578[] = {{9, -1}, {10, 2000}, {6, 2000}};

/* The body's model's motion table. */
static const EnemyMotionEntry lbl_804FD6FC[] = {{8, -1}, {4, -1}, {7, 5000}};

namespace BkChaos {

inline float GetCombineTime(BkChaos *chaos) {
    return chaos->m_params.CombineTime;
}

const char *BkChaosTask::s_taskName = "BkChaos";

BkChibiChaos::BkChibiChaos(BkChaos *owner) : m_owner(owner) {
    m_flags = 0;
    m_info = fn_801A0030(0xA3);
    fn_8019FD64(reinterpret_cast<EnemyCharInfo *>(m_info.get()),
                boost::bind(&BkChibiChaos::OnCommand, this, _1));
    fn_800091D0(&m_info->m_position, &m_owner->Enemy::field_0x48);
    m_render = fn_802CD8E4(0x11, MotionTable(lbl_804FD578, 3));
    if (m_render.get() != 0) {
        fn_802CD768(m_render.get(), &m_info->m_position, &m_info->m_rotation);
        fn_802CD7E8(m_render.get());
    }
    void *shapes = fn_8019A890(0x11, 0xD);
    if (m_render.get() != 0 && m_info.get() != 0 && shapes != 0) {
        m_collider = fn_800C81F4(fn_80021B08(), boost::weak_ptr<CharInfo>(m_info), shapes, 5, 0);
        m_collider->SetField2C(1);
        boost::weak_ptr<CharaColliAttack> attack(fn_800C4374(fn_80021B08(), m_collider));
        if (boost::shared_ptr<CharaColliAttack> collision = attack.lock()) {
            fn_80054DD0(collision.get(), 1, 1, 1.0f,
                        AttackCallback(boost::bind(&BkChibiChaos::OnHit, this, _1)),
                        AttackCallback(boost::bind(&BkChibiChaos::OnHitEnd, this, _1)));
            fn_80096B64(collision.get(), 1,
                        AttackCallback(boost::bind(&BkChibiChaos::OnAttacked, this, _1)));
        }
    }
    m_target = fn_801E7128(boost::weak_ptr<CharInfo>(m_info), 0x20C80);
    m_owner->m_chibis.push_back(this);
    if (m_owner->m_params.ShapeType == 0) {
        m_state = 0;
        Scatter();
        fn_800091D0(&m_offset, &(const Vec3f &)Vec3f(fn_8040EC80(10.0f, fn_8018346C(m_owner)), 0.0f, 0.0f));
        AssignVector(m_offset, fn_8040D0AC(&m_offset, fn_8040EC80(0.0f, 2.0f * lbl_805FAD38)));
        m_offset.y = fn_8040EC80(m_owner->m_minY, m_owner->m_maxY);
        m_collider->TurnOn(-1);
        m_target->Activate();
        fn_802CD5F0(m_render.get());
    } else {
        m_state = 3;
        m_collider->TurnOff(-1);
        m_target->Deactivate();
        fn_802CD5CC(m_render.get());
        m_owner->m_combined++;
    }
}

BkChibiChaos::~BkChibiChaos() {
    m_owner->m_chibis.remove(this);
}

Fn8040CopyVector3 BkChibiChaos::RandomPoint() {
    Vec3f point(fn_8040EC80(10.0f, fn_8018346C(m_owner)), 0.0f, 0.0f);
    AssignVector(point, fn_8040D0AC(&point, fn_8040EC80(0.0f, 2.0f * lbl_805FAD38)));
    point.x += m_owner->Enemy::field_0x48.x;
    point.y = fn_8040EC80(m_owner->m_minY, m_owner->m_maxY);
    point.z += m_owner->Enemy::field_0x48.z;
    return Fn8040CopyVector3(point);
}

void BkChibiChaos::Scatter() {
    fn_800091D0(&m_info->m_position, &reinterpret_cast<const Vec3f &>(RandomPoint()));
    Vec2f angles = fn_8040F760(&(m_info->m_position - *fn_80198BD8(m_owner)));
    m_info->m_rotation.x = angles.x;
    m_info->m_rotation.y = angles.y;
}

/* The character's height (name inferred). Read through an inline function, the position is loaded
 * before m_maxY in the upper height test below, which compares m_maxY with it as retail does
 * (wave 40, probes/Game_BkChaos_R2.py). */
inline float PositionY(CharInfo *info) {
    return info->m_position.y;
}

void BkChibiChaos::Update(float dt) {
    if (m_flags & 4) {
        delete this;
        return;
    }
    switch (m_state) {
    case 0: {
        float turn = dt * DegToRad(0.5f);
        AssignVector(m_offset, fn_8040D0AC(&m_offset, fn_8040EC80(-turn, turn)));
        m_offset.y = 0.8f * m_offset.y + 0.2f * fn_8040EC80(m_owner->m_minY, m_owner->m_maxY);
        Vec3f target;
        fn_800091EC(&target, &m_owner->Enemy::field_0x48);
        target.x += m_offset.x;
        target.y = m_offset.y;
        target.z += m_offset.z;
        const Vec3f &toTarget = ((VectorSubtractFn)fn_8040CD74)(&target, &m_info->m_position);
        fn_800091D0(&m_info->field_0x24, &toTarget);
        ScaleVector(m_info->field_0x24, fn_8040EC80(0.0f, 1.0f));
        for (ChibiList::iterator it = m_owner->m_chibis.begin(); it != m_owner->m_chibis.end();) {
            BkChibiChaos *other = *it++;
            if (other == this) {
                continue;
            }
            Vec3f away;
            fn_800091EC(&away, &(const Vec3f &)((VectorSubtractFn)fn_8040CD74)(
                                   &m_info->m_position, &other->m_info->m_position));
            float distance = fn_8040CF50(&away);
            if (distance > 0.1f && distance < 50.0f) {
                AddVector(m_info->field_0x24,
                          fn_800091A4(&(const Vec3f &)fn_8040CEE8(&away), 10.0f) / distance);
            }
        }
        fn_804103E8(&m_info->m_position, &m_info->field_0x18, &m_info->field_0x24, 0.1f, dt);
        Vec2f angles = fn_8040F760(&m_info->field_0x18);
        m_info->m_rotation.x = fn_8040F314(m_info->m_rotation.x, angles.x, 0.1f);
        m_info->m_rotation.y = fn_8040F314(m_info->m_rotation.y, angles.y, 0.1f);
        Vec3f fromHome;
        fn_800091EC(&fromHome, &(const Vec3f &)((VectorSubtractFn)fn_8040CD74)(
                                   &m_info->m_position, &m_owner->Enemy::field_0x48));
        fromHome.y = 0.0f;
        if (fn_8040CF50(&fromHome) > fn_8018346C(m_owner)) {
            Vec3f edge;
            fn_800091EC(&edge, &(const Vec3f &)fn_8040CDD0(
                                   &(const Vec3f &)fn_8040CDA8(&(const Vec3f &)fn_8040CEE8(&fromHome),
                                                               fn_8018346C(m_owner)),
                                   GetHome(m_owner)));
            m_info->m_position.x = edge.x;
            m_info->m_position.z = edge.z;
        }
        if (m_info->m_position.y < m_owner->m_minY) {
            m_info->m_position.y = m_owner->m_minY;
            m_info->field_0x18.y = 0.0f;
        }
        if (m_owner->m_maxY < PositionY(m_info.get())) {
            m_info->m_position.y = m_owner->m_maxY;
            m_info->field_0x18.y = 0.0f;
        }
        if (m_flags & 2) {
            m_state = 1;
        }
        break;
    }
    case 1: {
        Vec3f toCore;
        fn_800091EC(&toCore, &(const Vec3f &)Subtract(
                                 reinterpret_cast<const Vec3f &>(m_owner->GetCorePosition()),
                                 m_info->m_position));
        Vec3f velocity;
        fn_800091EC(&velocity,
                    &(const Vec3f &)fn_8040CDA8(
                        &(const Vec3f &)fn_8040CDA8(&(const Vec3f &)fn_8040CEE8(&toCore),
                                                    fn_8018346C(m_owner)),
                        1.0f / GetCombineTime(m_owner)));
        fn_800091D0(&m_info->field_0x18, &velocity);
        fn_804103E8(&m_info->m_position, &m_info->field_0x18, &lbl_805E07A0, 0.1f, dt);
        Vec2f angles = fn_8040F760(&m_info->field_0x18);
        m_info->m_rotation.x = fn_8040F314(m_info->m_rotation.x, angles.x, 0.1f);
        m_info->m_rotation.y = fn_8040F314(m_info->m_rotation.y, angles.y, 0.1f);
        if (fn_8040CF50(&toCore) < dt * fn_8040CF50(&velocity)) {
            m_collider->TurnOff(-1);
            m_target->Deactivate();
            fn_802CD5CC(m_render.get());
            m_state = 3;
            m_owner->m_combined++;
            fn_801D54F4(fn_80053164(), 0x606E, m_info.get(), 0, 1);
        }
        break;
    }
    case 2: {
        Vec3f toPoint;
        fn_800091EC(&toPoint, &(const Vec3f &)((VectorSubtractFn)fn_8040CD74)(
                                  &m_offset, &m_info->m_position));
        float distance = fn_8040CF50(&toPoint);
        if (distance > 0.01f) {
            float speed = distance / 0.9f;
            speed = fn_800704AC(speed, fn_8018346C(m_owner) / GetCombineTime(m_owner));
            Vec3f velocity;
            fn_800091EC(&velocity,
                        &(const Vec3f &)fn_8040CDA8(&(const Vec3f &)fn_8040CDA8(&toPoint, speed),
                                                    1.0f / distance));
            if (!fn_8000B044(dt)) {
                const Vec3f &acceleration = fn_8040CDA8(
                    &(const Vec3f &)((VectorSubtractFn)fn_8040CD74)(&velocity, &m_info->field_0x18),
                    1.0f / dt);
                fn_800091D0(&m_info->field_0x24, &acceleration);
            } else {
                fn_8040CE60(&m_info->field_0x24);
            }
        } else {
            fn_8040CE60(&m_info->field_0x24);
        }
        fn_804103E8(&m_info->m_position, &m_info->field_0x18, &m_info->field_0x24, 0.1f, dt);
        Vec2f angles = fn_8040F760(&m_info->field_0x18);
        m_info->m_rotation.x = fn_8040F314(m_info->m_rotation.x, angles.x, 0.1f);
        m_info->m_rotation.y = fn_8040F314(m_info->m_rotation.y, angles.y, 0.1f);
        if (distance < 5.0f) {
            m_state = 1;
        }
        break;
    }
    case 3:
        if (m_flags & 1) {
            m_flags &= ~1;
            m_collider->TurnOn(-1);
            m_target->Activate();
            fn_802CD5F0(m_render.get());
            m_owner->m_combined--;
            fn_800091D0(&m_info->m_position,
                        &reinterpret_cast<const Vec3f &>(m_owner->GetCorePosition()));
            fn_800091D0(&m_offset, &reinterpret_cast<const Vec3f &>(RandomPoint()));
            m_state = 2;
            break;
        }
        return;
    case 4:
        fn_801A1F1C(&m_info->m_position, 0x2D0);
        delete this;
        return;
    }
    fn_802CD638(m_render.get(), m_owner->Enemy::field_0xEC);
    fn_802CD768(m_render.get(), &m_info->m_position, &m_info->m_rotation);
    fn_802CD7E8(m_render.get());
    m_target->fn_800CCE38(&m_info->m_position);
}

/* Not called anywhere: the linker strips it. Retail pools 2 ahead of Update's float constants in
 * .sdata2; this function, emitted just before Update, makes the compiler pool it there (the name
 * is a placeholder). */
static float UnkConstantUser2(float x) {
    return x * 2.0f;
}

void BkChibiChaos::OnHit(AttackCallbackParam &param) {
    boost::shared_ptr<CharInfo> attacker = param.m_attacker.lock()->m_owner.lock();
    fn_800091D0(&HIT_DIRECTION(param), &(const Vec3f &)((VectorSubtractFn)fn_8040CD74)(
                                           &HIT_POSITION(param), &attacker->m_position));
}

void BkChibiChaos::OnAttacked(AttackCallbackParam &param) {
    if (param.m_damage > 0.0f) {
        param.m_result = 1;
        boost::shared_ptr<CharaColliAttack> attack = param.m_attacker.lock();
        if (attack.get() != 0) {
            boost::shared_ptr<CharInfo> attacker = attack->m_owner.lock();
            m_owner->m_hits++;
            m_owner->TakeHit(attacker);
            m_owner->Gather();
        }
        m_state = 4;
    } else {
        param.m_result = 8;
    }
}

void BkChibiChaos::OnCommand(CharCommand &command) {
    switch (command.TypeId()) {
    case 0x14:
        if (m_render.get() != 0) {
            VacuumCommand *vacuum = static_cast<VacuumCommand *>(&command);
            fn_8028CFC4(vacuum, m_render, &m_info->m_position, &m_info->m_rotation, 1);
            boost::shared_ptr<CharInfo> attacker = fn_801DADFC(vacuum).lock();
            m_owner->TakeHit(attacker);
            m_owner->Gather();
            m_flags |= 4;
        }
        command.Flags() |= 2;
    }
    if (!(command.Flags() & 2)) {
        command.Reject(boost::weak_ptr<CharInfo>(m_info));
    }
    if (!(command.Flags() & 2)) {
        command.Flags() |= 2;
        fn_803A953C("Command isn't Received ! : (%d)%s\n",
                    *reinterpret_cast<const u16 *>(command.field_0x4), command.GetName());
    }
}

/* The motion's node callbacks (names inferred). The body's scale follows its size. */
static void ScaleCallback(_stMtnCallbackParam *param) {
    float size = *static_cast<float *>(param->m_data);
    Vec3f scale(size, size, size);
    fn_8040B790(&lbl_805F13A8->m_matrices[lbl_805F13A8->m_top], &scale);
}

/* A node follows node 3000. */
static void FollowCallback(_stMtnCallbackParam *param) {
    Vec3f position;
    /* A named node: retail moves the call's result through r0 (as in BkLarva.cpp). */
    Mtx34f *node = fn_80415E70(static_cast<_stMtnPlayWork *>(param->m_data), 3000);
    fn_804114D0(&position, node);
    static_cast<Mtx34f *>(param->m_frame)->values[0][3] = position.x;
    static_cast<Mtx34f *>(param->m_frame)->values[1][3] = position.y;
    static_cast<Mtx34f *>(param->m_frame)->values[2][3] = position.z;
}

/* The two stretching nodes lean toward the target and stretch with its distance. */
static void StretchCallback(_stMtnCallbackParam *param) {
    BkChaos *chaos = static_cast<BkChaos *>(param->m_data);
    Mtx34f matrix;
    fn_80016370(&matrix, param->m_frame);
    float stretch = 1.0f;
    float scale;
    float pitch = 0.0f;
    float yaw = pitch;
    Vec3f toTarget;
    fn_800091EC(&toTarget, &(const Vec3f &)((VectorSubtractFn)fn_8040CD74)(&chaos->Enemy::field_0x6C,
                                                                          chaos->GetPosition()));
    float distance = fn_8040CF50(&toTarget);
    scale = 1.0f;
    float reach = 50.0f * chaos->m_size;
    if (distance > reach) {
        scale = distance / reach;
    }
    int index = param->m_id == 0x1774 ? 0 : 1;
    if (fn_801A4494(chaos->vfDisp_0x1C()) == 1) {
        Vec3f local;
        fn_800091EC(&local, &(const Vec3f &)fn_8040B67C(&(const Mtx34f &)fn_8040BEF8(&matrix),
                                                        &chaos->Enemy::field_0x6C));
        Vec3f direction;
        fn_800091EC(&direction, &(const Vec3f &)fn_8040CEE8(&local));
        Vec2f angles = fn_8040F760(&direction);
        pitch = angles.x;
        yaw = angles.y;
        float delta = (1.0f - scale) * (index ? 0.0012f : -0.0012f);
        yaw += delta;
        if (yaw > DegToRad(60.0f)) {
            yaw = DegToRad(60.0f);
        } else if (yaw < -DegToRad(60.0f)) {
            yaw = -DegToRad(60.0f);
        }
        if (pitch > DegToRad(40.0f)) {
            pitch = DegToRad(40.0f);
        } else if (pitch < DegToRad(-80.0f)) {
            pitch = DegToRad(-80.0f);
        }
        /* Read as volatile (not original as far as is known): retail loads the count again for
         * the decrement, which GC/2.7 only does for a volatile object. */
        if (*(volatile s32 *)&chaos->field_0x2FC > 0) {
            stretch = scale;
            chaos->field_0x2FC--;
        }
    }
    float turn = 10.0f * chaos->GetFrameTime();
    float rate = (scale / 0.16666667f) * chaos->GetFrameTime();
    chaos->m_tilt[index].y = TurnAngleToward(yaw, chaos->m_tilt[index].y, turn);
    chaos->m_tilt[index].x = TurnAngleToward(pitch, chaos->m_tilt[index].x, turn);
    chaos->m_tilt[index].z = TurnAngleToward(0.0f, chaos->m_tilt[index].z, turn);
    float value = chaos->m_tiltRate[index];
    if (stretch > value) {
        value += rate;
        if (value > stretch) {
            value = stretch;
        }
    } else if (stretch < value) {
        value -= rate;
        if (value < stretch) {
            value = stretch;
        }
    } else {
        value = stretch;
    }
    chaos->m_tiltRate[index] = value;
    Mtx34f rotation;
    fn_8040B6F8(&rotation);
    fn_8001E070(&rotation, &chaos->m_tilt[index], 1);
    fn_8040B720(&rotation, 1.0f, 1.0f, chaos->m_tiltRate[index]);
    fn_8040BAC4(&matrix, &rotation);
    fn_800163F4(param->m_frame, &matrix);
}

BkChaos::BkChaos() : EnemyFlyerCommon(10), m_minY(0.0f), m_maxY(0.0f) {
    m_enemy = this;
    vfPara_0x40(reinterpret_cast<int>(&lbl_805F88E0));
    m_combined = 0;
    m_hits = 0;
    m_combineTimer = 0.0f;
    void *loader = fn_800122D4();
    void *body = fn_8001277C(loader, m_motionBase, 4);
    fn_801EB660(GetUVAnimManager(), "CHAOSBODY_UVA", body);
    void *drop = fn_8001277C(loader, m_motionBase, 6);
    fn_801EB660(GetUVAnimManager(), "CHIBIDROP_UVA", drop);
    fn_8040E970(&m_tilt[0]);
    fn_8040E970(&m_tilt[1]);
    m_tiltRate[0] = 1.0f;
    m_tiltRate[1] = 1.0f;
    field_0x2FC = 0;
    field_0x300 = 0;
}

BkChaos::~BkChaos() {
    if (!m_effect.expired()) {
        m_effect.lock()->Stop();
        m_effect.reset();
    }
    m_sound.fn_801D455C();
    m_rollSound.fn_801D455C();
    while (m_chibis.size() != 0) {
        delete m_chibis.front();
    }
    if (m_render.get() != 0) {
        _stMtnPlayWork *work = fn_802CD420(m_render.get());
        if (work != 0) {
            fn_80415808(work, 0x1773, 0, 0);
            fn_80415808(work, 0xFA0, 0, 0);
            fn_80415808(work, 0x1776, 0, 0);
            fn_80415808(work, 0x1774, 0, 0);
        }
    }
    fn_801EB550(GetUVAnimManager(), "CHIBIDROP_UVA");
    fn_801EB550(GetUVAnimManager(), "CHAOSBODY_UVA");
}

void BkChaos::vfSet_0x1C(UnkPlacement_800C9CD0 *placement) {
    vfSet_0x20(placement);
    /* Copied as a plain struct (word loads and stores), unlike Vec3f's own copy. */
    Vec3fWords rotation = *reinterpret_cast<Vec3fWords *>(GetRotation());
    vfDisp_0x38(GetPosition(), reinterpret_cast<Vec3f *>(&rotation));
    m_ctrl->vfCtrl_0x20();
}

/* An inline function: GC/2.7 evaluates its arguments right to left, which is the order retail
 * loads MoveHeightOffset and MoveHeight in, ahead of the call. */
inline void BkChaos::SetRange(float y, float height, float offset) {
    m_maxY = offset + (y + height);
    m_minY = offset + (y - height);
}

void BkChaos::vfSet_0x20(UnkPlacement_800C9CD0 *placement) {
    TEnemySetBase::vfSet_0x20(placement);
    fn_800C9DBC(placement, &m_params, sizeof(m_params));
    SetRange(GetPosition()->y, m_params.MoveHeight, m_params.MoveHeightOffset);
    if (m_params.ShapeType == 0) {
        if (m_params.CombineStartTime > 0.0f) {
            m_state = 0;
            m_combineTimer = m_params.CombineStartTime;
        } else {
            m_state = 1;
        }
    } else {
        m_state = 2;
    }
    m_size = m_combined / 5.0f;
}

void BkChaos::vfStat_0x38() {
    m_status->vfStat_0x3C(false);
    m_status->vfStat_0x30(true);
}

void BkChaos::vfDisp_0x64() {
    void *loader = fn_800122D4();
    int slot = m_motionBase;
    void *model = fn_80012684(loader, slot, 0xB);
    void *motion = fn_80012608(loader, slot, 0xE);
    m_render = fn_802CDB08(slot, MotionTable(lbl_804FD6FC, 3), model, motion);
    _stMtnPlayWork *work = fn_802CD420(m_render.get());
    if (work != 0) {
        fn_80415808(work, 0x1773, ScaleCallback, &m_size);
        fn_80415808(work, 0xFA0, FollowCallback, work);
        fn_80415808(work, 0x1776, StretchCallback, this);
        fn_80415808(work, 0x1774, StretchCallback, this);
    }
}

} // namespace BkChaos

/* Slot 0x10 of the status, its update: it takes the frame time, which Game/Enemy.h does not
 * declare, so it is defined under its symbol (as in EnemyStatusCommon.cpp). */
extern "C" void vfStat_0x10__17EnemyStatusCommonFv(EnemyStatusCommon *status, float dt);

extern "C" void vfStat_0x10__Q27BkChaos7BkChaosFv(BkChaos::BkChaos *self, float dt) {
    vfStat_0x10__17EnemyStatusCommonFv(&static_cast<EnemyStatusCommon &>(*self), dt);
    if (self->m_size > 0.0f) {
        fn_801A9540(&self->EnemyStatusCommon::field_0x8, 0x1F40);
        fn_801A95B4(&self->EnemyStatusCommon::field_0x8, 15.0f * self->m_size);
    } else {
        fn_801A9508(&self->EnemyStatusCommon::field_0x8, 0x1F40);
    }
    switch (self->m_state) {
    case 0:
        if ((self->m_combineTimer -= dt) < 0.0f) {
            self->Gather();
        }
        break;
    case 1:
        if (self->m_sound.m_id != 0) {
            self->m_sound.fn_801D455C();
        }
        break;
    case 2:
        if (self->m_sound.m_id == 0) {
            self->m_sound = fn_8019A908(self, 0x6070, 1);
        }
        break;
    }
}

namespace BkChaos {

void BkChaos::vfDisp_0x08() {
    EnemyDispCommon::vfDisp_0x08();
    for (int i = 0; i < m_params.ChibiNum; i++) {
        new BkChibiChaos(this);
    }
}

void BkChaos::vfDisp_0x10(float dt) {
    UpdateSize();
    EnemyDispCommon::vfDisp_0x10(dt);
    if (m_size > 0.2f) {
        if (m_effect.expired()) {
            m_effect = fn_801A1E48(GetPartMatrix(0x1F40), 0x2AB);
        }
    } else if (!m_effect.expired()) {
        m_effect.lock()->Stop();
        m_effect.reset();
    }
    if (field_0x300) {
        field_0x300 = 0;
        if (m_rollSound.m_id == 0) {
            m_rollSound = fn_8019A908(this, 0x6073, 1);
        }
    } else {
        m_rollSound.fn_801D455C();
    }
    m_attack.Update();
    for (ChibiList::iterator it = m_chibis.begin(); it != m_chibis.end();) {
        (*it++)->Update(dt);
    }
}

void BkChaos::vfDisp_0x40(EnemyMotionEventArg &arg) {
    EnemyDispCommon::vfDisp_0x40(arg);
    if (m_size > 0.0f && arg.m_type == 0xD000) {
        EnemyMotionEventArg::Data *data = arg.m_data;
        Vec3f left;
        fn_800091EC(&left, &(const Vec3f &)fn_800496F8(GetPartMatrix(0x177A)));
        Vec3f right;
        fn_800091EC(&right, &(const Vec3f &)fn_800496F8(GetPartMatrix(0x1784)));
        Vec3f center;
        fn_800091EC(&center, &(const Vec3f &)fn_8040CDA8(
                                 &(const Vec3f &)fn_8040CDD0(&left, &right), 0.5f));
        switch (data->value) {
        case 1: {
            field_0x2FC += 2;
            UnkSphere_80035CEC sphere(lbl_805E07A0, 10.0f * (0.5f * m_size));
            m_attack.Set(EnemyAttack::Desc(data->value, 1, EnemyAttack::Desc::Shape(0, &sphere),
                                           boost::weak_ptr<CharInfo>(fn_8017D014(this))),
                         reinterpret_cast<CharInfo *>(&center));
            break;
        }
        case 0: {
            field_0x300 = 1;
            /* Named: with the literals in the products GC/2.7 multiplies them the other way
             * round. */
            float halfHeight = 5.0f;
            float radius = 30.0f;
            UnkCylinder_80034854 cylinder(lbl_805E07A0, m_size * halfHeight, m_size * radius,
                                          Vec3f(90.0f * (lbl_805FAD38 / 180.0f), 0.0f, 0.0f), 0);
            m_attack.Set(EnemyAttack::Desc(data->value, 1, EnemyAttack::Desc::Shape(3, &cylinder),
                                           boost::weak_ptr<CharInfo>(fn_8017D014(this))),
                         reinterpret_cast<CharInfo *>(&center));
            break;
        }
        }
    }
}

bool BkChaos::vfDisp_0x44(int *command) {
    if (EnemyDispCommon::vfDisp_0x44(command)) {
        return true;
    }
    switch (*command) {
    case 0x1C:
        fn_8019AA40(this, 0x6072, 1);
        break;
    }
    return true;
}

void BkChaos::vfDisp_0x4C() {
    Mtx34f *matrix = GetPartMatrix(0x1388);
    if (matrix != 0) {
        fn_801A1ED8(matrix, 0x72);
    }
    matrix = GetPartMatrix(0x1F40);
    if (matrix != 0) {
        fn_801A1ED8(matrix, 0x1F3);
    }
    fn_8019AA40(this, 0x6007, 1);
}

/* A view of the chaos's primary vtable (not original): the status's slot 0x98 takes the attacker,
 * which Game/Enemy.h does not declare (see vfStat_0x98 below), and the chaos calls it through its
 * own copy of the slot, at 0x244 of the vtable at 0x18. The slots before it are placeholders. */
#define BK_CHAOS_SLOT(n) virtual void slot##n();
#define BK_CHAOS_SLOTS10(n)                                                                          BK_CHAOS_SLOT(n##0) BK_CHAOS_SLOT(n##1) BK_CHAOS_SLOT(n##2) BK_CHAOS_SLOT(n##3)                  BK_CHAOS_SLOT(n##4) BK_CHAOS_SLOT(n##5) BK_CHAOS_SLOT(n##6) BK_CHAOS_SLOT(n##7)                  BK_CHAOS_SLOT(n##8) BK_CHAOS_SLOT(n##9)
class BkChaosSlotView {
  public:
    /* 0x00 */ u8 field_0x0[0x18];
    /* 143 slots, 0x08 to 0x240. */
    BK_CHAOS_SLOTS10(0) BK_CHAOS_SLOTS10(1) BK_CHAOS_SLOTS10(2) BK_CHAOS_SLOTS10(3)
    BK_CHAOS_SLOTS10(4) BK_CHAOS_SLOTS10(5) BK_CHAOS_SLOTS10(6) BK_CHAOS_SLOTS10(7)
    BK_CHAOS_SLOTS10(8) BK_CHAOS_SLOTS10(9) BK_CHAOS_SLOTS10(10) BK_CHAOS_SLOTS10(11)
    BK_CHAOS_SLOTS10(12) BK_CHAOS_SLOTS10(13) BK_CHAOS_SLOT(140) BK_CHAOS_SLOT(141)
    BK_CHAOS_SLOT(142)
    /* 0x244 */ virtual void vfStat_0x98(const boost::shared_ptr<CharInfo> &attacker);
};

void BkChaos::TakeHit(boost::shared_ptr<CharInfo> attacker) {
    field_0x7C -= GetChibiLife();
    if (field_0x7C <= 0.0f) {
        field_0x76 = 1;
        if (attacker.get() != 0) {
            reinterpret_cast<BkChaosSlotView *>(this)->vfStat_0x98(attacker);
            if (fn_8021C958(attacker)) {
                EnemyStatusCommon::m_flags.Set(0xC);
            }
        }
    }
}

} // namespace BkChaos

/* Defined under its symbol over a const chaos (as GetViewport in Game/CameraScreenSet.cpp): only
 * for a const object does GC/2.7 read ChibiNum and m_hits above the prologue's stores, the way
 * retail does. */
extern "C" float GetChibiLife__Q27BkChaos7BkChaosFv(const BkChaos::BkChaos *self) {
    int count = self->m_params.ChibiNum - self->m_hits;
    if (count > 0) {
        return const_cast<BkChaos::BkChaos *>(self)->vfStat_0xAC() / count;
    }
    return self->m_params.MaxLife;
}

namespace BkChaos {

float BkChaos::vfPara_0x6C() {
    return m_params.MaxLife;
}

int BkChaos::vfPara_0x48() {
    return 0;
}

int BkChaos::vfPara_0x4C() {
    return vfPara_0x48();
}

int BkChaos::vfPara_0x54() {
    return vfPara_0x48();
}

int BkChaos::vfPara_0x5C() const {
    return 1;
}

bool BkChaos::IsTargetNear() {
    Vec3f target;
    fn_800091EC(&target, static_cast<Vec3f *>(m_enemy->m_ctrl->vfCtrl_0x10()));
    if (std::fabs(target.y) < 20.0f) {
        target.y = 0.0f;
        if (fn_8019B298(this, &target) < s_rollRange * m_size) {
            return true;
        }
    }
    return false;
}

int BkChaos::vfPara_0x58() {
    return IsTargetNear() ? 2 : 3;
}

void BkChaos::vfStat_0x70(AttackCallbackParam &param) {
    EnemyStatusCommon::vfStat_0x70(param);
    if (field_0x76) {
        for (ChibiList::iterator it = m_chibis.begin(); it != m_chibis.end(); ++it) {
            (*it)->m_state = 4;
        }
    } else {
        Gather();
        ReleaseChibis();
    }
}

int BkChaos::vfStat_0xB8() {
    return 0x61;
}

} // namespace BkChaos

/* Slot 0x98 of the status: it takes the attacker, which Game/Enemy.h does not declare, so it is
 * defined under its symbol (as vfStat_0x10 above). */
extern "C" void vfStat_0x98__17EnemyStatusCommonFv(EnemyStatusCommon *status,
                                                    const boost::shared_ptr<CharInfo> &attacker);

extern "C" void vfStat_0x98__Q27BkChaos7BkChaosFv(BkChaos::BkChaos *self,
                                                   boost::shared_ptr<CharInfo> &attacker) {
    if (attacker.get() != 0) {
        for (int i = 0; i < self->m_params.ChibiNum; i++) {
            vfStat_0x98__17EnemyStatusCommonFv(&static_cast<EnemyStatusCommon &>(*self), attacker);
        }
    }
}

namespace BkChaos {

void BkChaos::Gather() {
    m_state = 2;
    for (ChibiList::iterator it = m_chibis.begin(); it != m_chibis.end(); ++it) {
        (*it)->m_flags |= 2;
    }
}

void BkChaos::ReleaseChibis() {
    for (ChibiList::iterator it = m_chibis.begin(); it != m_chibis.end(); ++it) {
        BkChibiChaos *chibi = *it;
        if (chibi->m_state == 3) {
            chibi->m_flags |= 1;
        }
    }
    fn_8019AA40(this, 0x606D, 1);
}

Fn8040CopyVector3 BkChaos::GetCorePosition() {
    Mtx34f *matrix = fn_802CD560(m_render.get(), 0x1773);
    if (matrix != 0) {
        return ((MatrixTranslationFn)fn_800496F8)(matrix);
    }
    return Fn8040CopyVector3(lbl_805E07A0);
}

void BkChaos::UpdateSize() {
    float size = m_combined / 5.0f;
    if (size >= m_size) {
        if ((m_size += 3.0f * GetFrameTime()) > size) {
            m_size = size;
        }
    } else if ((m_size -= 4.0f * GetFrameTime()) < size) {
        m_size = size;
    }
}

BkChaosTask::BkChaosTask(Task *parent, UnkPlacement_800C9CD0 *placement)
    : EnemyTemplate<BkChaos, BkChaosAI>(parent, placement) {
    m_enemy = this;
    m_name = s_taskName;
}

BkChaosTask::~BkChaosTask() {}

} // namespace BkChaos

extern "C" const char *fn_802AF3E0(UnkLabelQuery_800CBC20 *query) {
    static const char *s_shapeTypes[2] = {"ShapeType : CHIBI", "ShapeType : OYA"};
    static int s_shapeType = 0;

    switch (query->index) {
    case 7:
        s_shapeType = query->value;
        return s_shapeTypes[query->value];
    case 11:
        if (s_shapeType == 0) {
            if (query->fvalue < 0.0f) {
                return "Not Combine !";
            }
            return query->params[query->index].label;
        }
        break;
    }
    return "";
}

namespace BkChaos {

void BkChaosTask::LoadFiles() {
    void *loader = fn_800122D4();
    fn_8001299C(loader, 0x11, new ResourceOneFile("enemy/BkChaosData.one", false));
}

void BkChaosTask::DropFiles() {
    fn_800127F8(fn_800122D4(), 0x11);
}

void BkChaosTask::Instantiate(UnkPlacement_800C9CD0 *placement) {
    new BkChaosTask(fn_801EB304(fn_8001C4F8(), 0xE), placement);
}

} // namespace BkChaos

static GadgetParamDesc lbl_80555D78[] = {
    {4, "MoveRange", 0, 0, 0, 200.0f, 0.0f, 65536.0f, fn_800CBC20, 0},
    {4, "SearchRange", 0, 0, 0, 0.0f, 0.0f, 65536.0f, fn_800CBC20, 0},
    {4, "SearchAngle", 0, 0, 0, 0.0f, -360.0f, 360.0f, fn_800CBC20, 0},
    {4, "SearchWidth", 0, 0, 0, 100.0f, 0.0f, 65536.0f, fn_800CBC20, 0},
    {4, "SearchHeight", 0, 0, 0, 100.0f, 0.0f, 65536.0f, fn_800CBC20, 0},
    {4, "SearchHeightOffset", 0, 0, 0, 0.0f, -65536.0f, 65536.0f, fn_800CBC20, 0},
    {4, "MaxLife", 0, 0, 0, 20.0f, 1.0f, 65536.0f, fn_800CBC20, 0},
    {1, "ShapeType", 0, 0, 1, 0.0f, 0.0f, 0.0f, fn_802AF3E0, 0},
    {1, "ChibiNum", 2, 2, 0x10000, 0.0f, 0.0f, 0.0f, fn_800CBC20, 0},
    {4, "MoveHeight", 0, 0, 0, 100.0f, 0.0f, 65536.0f, fn_800CBC20, 0},
    {4, "MoveHeightOffset", 0, 0, 0, 0.0f, -65536.0f, 65536.0f, fn_800CBC20, 0},
    {4, "CombineStartTime", 0, 0, 0, 1.0f, -65536.0f, 65536.0f, fn_802AF3E0, 0},
    {4, "CombineTime", 0, 0, 0, 1.0f, 0.1f, 65536.0f, fn_800CBC20, 0},
    {0},
};

GadgetClassDesc lbl_80555FA8 = {
    "BK_CHAOS", BkChaos::BkChaosTask::LoadFiles, BkChaos::BkChaosTask::DropFiles,
    reinterpret_cast<void (*)(UnkPlacement_800C9CD0 *)>(BkChaos::BkChaosTask::Instantiate), 0, 0,
    0x92, 2, 0, 5, lbl_80555D78,
};

/* Not called anywhere: the linker strips it. Retail pools 0 and 0.5 ahead of the other float
 * constants in .sdata2; this function, emitted first, makes the compiler pool them there (as in
 * BkGiant.cpp; the name is a placeholder). */
static float UnkConstantUser(float x) {
    x *= 0.0f;
    x *= 0.5f;
    return x;
}
