/* EnemyStatusCommon (class name from RTTI; Game/Enemy.h): the status part of an enemy, its health,
 * damage and flags, with its collider (EnemyCharaColliModule, class name from the RTTI of its
 * callback binder) and its motion sequence player (EnemyMotionSequenceModule). Names marked
 * "(name inferred)" are not original. Definitions are in reverse address order (deferred inlining
 * emits functions last-defined first).
 *
 * NonMatching: 87 of 90 functions and all data match. In slot 0x98 retail loads the attacker's
 * pointer above the prologue's store of r31 (as in Game/EnemyParamCommon.cpp); in Reset retail
 * keeps the result in r4 and the table in r5, GC/2.7 the other way round; and boost's
 * function1::operator() (0x801AA680) has a stack frame 0x10 bytes larger in retail, with the
 * same code. */
/* The fields of EnemyCharaColliModule (Game/Enemy.h). */
#define ENEMY_STATUS_COMMON_UNIT

#include "Game/CharaColliBaseCharInfo.h"
#include "Game/BossDef.h"
#include "Game/CharInfo.h"
#include "Game/CharaColli.h"
#include "Game/CharaColliAttack.h"
#include "Game/Enemy.h"
#include "Game/GadgetCollision.h"
#include "Game/MathConstants.h"
#include "Game/MathTypes.h"
#include "Game/MotionSequenceModule.h"
#include "boost/shared_ptr.hpp"
#include "boost/weak_ptr.hpp"
#include <boost/bind.hpp>
#include <boost/function.hpp>

using SonicteamUSA::System::CharaColli::AttackCallback;
using SonicteamUSA::System::CharaColli::AttackCallbackParam;
using SonicteamUSA::System::CharaColli::CharaColliAttack;
using SonicteamUSA::System::CharaColli::CharaColliBase;
using SonicteamUSA::System::CharaColli::CharaColliBaseCharInfo;

struct _stMtnPlayWork;
struct UnkColliContactNode_800587A0;
class UnkCharaColliObject_8001E9C4;

/* MSL's std::fabs for floats, which the game calls out of line (as in Game/Enemy.cpp). */
namespace std {
float fabs(float x);
}

/* What the collider passes to the contact callback (struct name from the RTTI of the binder;
 * names inferred): the contacts and the collider. */
struct EnemyColliDetectArgs {
    /* 0x0 */ UnkColliContacts_8001EE04 *m_contacts;
    /* 0x4 */ CharaColliBase *m_collider;
};

/* A node of a contact list (see Game/BkLarva.cpp). */
struct UnkColliContactNode_800587A0 {
    /* 0x00 */ u8 field_0x0[0x14];
    /* 0x14 */ UnkCharaColliObject_8001E9C4 *m_object;
};

/* The enemy as Game/EnemyTarget.cpp sees it (struct name unknown). */
struct UnkEnemy_8017D014;
struct EnemyTargetEntry;

/* EnemyTarget (class name from RTTI; see Game/EnemyTarget.cpp). */
class EnemyTarget {
  public:
    static boost::shared_ptr<EnemyTarget> Instantiate(UnkEnemy_8017D014 *enemy, const EnemyTargetEntry *entries);
    void Tick();
    void TurnOn();
    void TurnOff();
};

/* The motion of an enemy's model (struct name unknown; names inferred). */
struct UnkEnemyMotion_801A83D8 {
    /* 0x0 */ _stMtnPlayWork *m_work;
    /* 0x4 */ u32 field_0x4;
    /* 0x8 */ s32 m_base;
};

extern "C" {
/* The collider manager and the colliders it makes (see Game/BkLarva.cpp and
 * Game/BossCharaColli.cpp; behaviours inferred): from a table of shapes, a sphere, the attack
 * collision of a collider, and removing a collider. */
void *fn_80021B08(void);
boost::shared_ptr<CharaColliBase> fn_800C7F7C(void *manager, const boost::weak_ptr<CharInfo> &owner,
                                              const void *shapes, void *work, int kind, int arg5);
boost::shared_ptr<CharaColliBase> fn_800C7DEC(void *manager, const boost::weak_ptr<CharInfo> &owner,
                                              int flags, int kind, int arg4, const Vec3f &offset,
                                              float radius);
boost::weak_ptr<CharaColliAttack> fn_800C4374(void *manager, boost::shared_ptr<CharaColliBase> collider);
void fn_800C78B0(void *manager, boost::shared_ptr<CharaColliBase> *collider);
/* The attack table of an enemy's motion slot (Game/EnemyCharInfo.cpp). */
void *fn_8019A890(s32 slot, int kind);
/* The contacts (see Game/BkLarva.cpp). */
UnkColliContactNode_800587A0 *fn_8001EE04(UnkColliContacts_8001EE04 *contacts);
UnkColliContactNode_800587A0 *fn_8001EDEC(UnkColliContacts_8001EE04 *contacts);
void fn_80027398(UnkColliContactNode_800587A0 **it);
u32 fn_80026588(UnkColliContactNode_800587A0 *node);
const Vec3f *fn_8001E9C4(UnkCharaColliObject_8001E9C4 *object);
float fn_8001E9E8(void *shape);
void fn_8040CE38(Vec3f *v, float scale);
void fn_8040CFF8(Vec3f *vector, const Vec3f *value);
/* The enemy's push (Game/EnemyAI.h) and the enemies' shared helpers (Game/Enemy.cpp; behaviours
 * inferred). */
Vec3f *fn_80181F0C(Enemy *enemy);
float fn_8019B184(Enemy *enemy, const Vec3f *direction);
float fn_8019B1D8(Enemy *enemy, const Vec3f *direction);
Vec3f fn_80016284(const Vec3f *v);
/* Whether the attacker is the player, the rewards and the hard difficulty (behaviours
 * inferred). */
bool fn_8021C958(const boost::shared_ptr<CharInfo> &attacker);
void fn_8020642C(const boost::shared_ptr<CharInfo> &attacker, int reward);
bool fn_801A2030(void);
/* The enemy registry's kill report (Game/EnemyEntry.cpp). */
void fn_801A2150(void *registry, UnkPlacement_800C9CD0 *owner, int type, int result);
/* Flag words (see Game/BKTankController.h). */
bool fn_80014E34(u32 *flags, int bit);
void fn_8006127C(u32 *flags, int bit);
/* A random number below count (Game/Random.h). */
int fn_8040EA90(int count);
}

/* fn_8040CD74 called as returning a - b by value (see Game/AdjustCosRot.cpp). */
typedef Vec3f (*VectorSubtractFn)(const Vec3f *a, const Vec3f *b);

/* The attack collision of a collider attached to a character, at 0x38 (inline; as in
 * Game/BulletUtil.cpp). */
inline boost::weak_ptr<CharaColliAttack> AttackOf(CharaColliBaseCharInfo *collider) {
    return *reinterpret_cast<boost::shared_ptr<CharaColliAttack> *>(reinterpret_cast<u8 *>(collider) + 0x38);
}

/* The slots this unit calls with arguments Game/Enemy.h does not declare: views of the
 * interfaces, used for those calls only (not original; see Game/TEnemySetTask.cpp). */
class EnemyStatFuncCalls {
  public:
    virtual void vfStat_0x08();
    virtual void vfStat_0x0C();
    virtual void vfStat_0x10();
    virtual void vfStat_0x14();
    virtual void vfStat_0x18();
    virtual void vfStat_0x1C();
    virtual void vfStat_0x20();
    virtual void vfStat_0x24();
    virtual void vfStat_0x28();
    virtual void vfStat_0x2C();
    virtual void vfStat_0x30();
    virtual void vfStat_0x34();
    virtual void vfStat_0x38();
    virtual void vfStat_0x3C();
    virtual void vfStat_0x40();
    virtual void vfStat_0x44();
    virtual void vfStat_0x48();
    virtual void vfStat_0x4C();
    virtual void vfStat_0x50();
    virtual void vfStat_0x54();
    virtual void vfStat_0x58();
    virtual void vfStat_0x5C();
    virtual void vfStat_0x60();
    virtual void vfStat_0x64();
    virtual void vfStat_0x68();
    virtual void vfStat_0x6C();
    virtual void vfStat_0x70();
    virtual void vfStat_0x74();
    virtual bool vfStat_0x78(float damage);
    virtual void vfStat_0x7C();
    virtual void vfStat_0x80();
    virtual void vfStat_0x84();
    virtual void vfStat_0x88();
    virtual void vfStat_0x8C();
    virtual void vfStat_0x90();
    virtual void vfStat_0x94();
    virtual void vfStat_0x98(const boost::shared_ptr<CharInfo> &attacker);
};

class EnemyDispFuncCalls {
  public:
    /* 0x0 */ Vec3f field_0x0;

    virtual void vfDisp_0x08();
    virtual void vfDisp_0x0C();
    virtual void vfDisp_0x10();
    virtual void vfDisp_0x14();
    virtual void vfDisp_0x18();
    virtual void vfDisp_0x1C();
    virtual void vfDisp_0x20();
    virtual void vfDisp_0x24();
    virtual void vfDisp_0x28();
    virtual void vfDisp_0x2C();
    virtual void vfDisp_0x30();
    virtual void vfDisp_0x34();
    virtual void vfDisp_0x38();
    virtual void vfDisp_0x3C();
    virtual void vfDisp_0x40();
    virtual void vfDisp_0x44();
    virtual void vfDisp_0x48(const Vec3f *position, const Vec3f *direction);
};

/* The collider shapes of the enemies without an attack table: one sphere, filled in below, and
 * the zero entry that ends the list. */
static const UnkBossColliShape s_sphereShapes[2] = {{0, 0, 0, 0}, {0, 0, 0, 0}};

EnemyCharaColliModule::EnemyCharaColliModule() : m_enemy(0), m_collider(), m_onContact() {}

EnemyCharaColliModule::~EnemyCharaColliModule() {
    Release();
}

void EnemyCharaColliModule::Release() {
    if (m_collider.get() != 0) {
        fn_800C78B0(fn_80021B08(), &m_collider);
    }
}

/* The radius of the enemy's body (name inferred). */
inline float BodyRadius(Enemy *enemy) {
    return *reinterpret_cast<float *>(enemy->m_para->vfPara_0x1C());
}

/* Creates the collider: from the enemy's attack table, else from its body shape, else a sphere. */
void EnemyCharaColliModule::Create() {
    boost::shared_ptr<CharaColliBase> collider;
    u8 walking = m_enemy->m_move->vfMove_0x20();
    int kind = 5;
    if (walking) {
        kind = 4;
    }
    int index = m_enemy->m_para->vfPara_0x38();
    if (index >= 0) {
        void *table = fn_8019A890(m_enemy->m_motionBase, index);
        if (table != 0) {
            collider = fn_800C7F7C(fn_80021B08(), boost::weak_ptr<CharInfo>(fn_8017D014(m_enemy)), table,
                                   *reinterpret_cast<void **>(m_enemy->m_disp->vfDisp_0x1C()), kind, 0);
        }
    }
    if (collider.get() == 0) {
        UnkSphere_80035CEC sphere(Vec3f(0.0f, 0.0f, 0.0f), *reinterpret_cast<const float *>(m_enemy->m_para->vfPara_0x1C()));
        struct Shapes {
            UnkBossColliShape entries[2];
        };
        Shapes shapes = *reinterpret_cast<const Shapes *>(s_sphereShapes);
        shapes.entries[0].m_sphere = &sphere;
        shapes.entries[0].field_0xC = m_enemy->m_para->vfPara_0x3C();
        if (m_enemy->m_disp->vfDisp_0x1C() != 0) {
            collider = fn_800C7F7C(fn_80021B08(), boost::weak_ptr<CharInfo>(fn_8017D014(m_enemy)), &shapes,
                                   *reinterpret_cast<void **>(m_enemy->m_disp->vfDisp_0x1C()), kind, 0);
        } else {
            collider = fn_800C7DEC(fn_80021B08(), boost::weak_ptr<CharInfo>(fn_8017D014(m_enemy)), 0, kind, 0,
                                   lbl_805E07A0, BodyRadius(m_enemy));
        }
    }
    if (collider.get() != 0) {
        collider->SetField2C(1);
        m_collider = collider;
        boost::weak_ptr<CharaColliAttack> attack(fn_800C4374(fn_80021B08(), collider));
        if (boost::shared_ptr<CharaColliAttack> attackPtr = attack.lock()) {
            attackPtr->m_power = 1.0f;
            attackPtr->m_kind = 1;
            *reinterpret_cast<s32 *>(attackPtr->field_0x3C) = 1;
            attackPtr->m_flags = 1;
            attackPtr->field_0x54 = 1;
            attackPtr->field_0x55 = 1;
            attackPtr->m_hitCallback = AttackCallback(boost::bind(&EnemyCharaColliModule::OnHit, this, _1));
            attackPtr->m_attackCallback = AttackCallback(boost::bind(&EnemyCharaColliModule::OnAttacked, this, _1));
        }
    }
}

/* Passes the contacts to the callback (name inferred). */
void EnemyCharaColliModule::Update() {
    CharaColliBase *collider = m_collider.get();
    if (collider != 0) {
        UnkColliContacts_8001EE04 *contacts =
            reinterpret_cast<UnkColliContacts_8001EE04 *>(reinterpret_cast<u8 *>(collider) + 0xC);
        if (contacts != 0 && contacts->m_count > 0) {
            EnemyColliDetectArgs args;
            args.m_contacts = contacts;
            args.m_collider = collider;
            m_onContact(args);
        }
    }
}

/* Setters of the attack collision (names inferred). */
extern "C" void fn_801A9844(EnemyCharaColliModule *module, bool hits, bool hittable) {
    if (module->m_collider.get() != 0) {
        CharaColliBaseCharInfo *info = dynamic_cast<CharaColliBaseCharInfo *>(module->m_collider.get());
        if (info != 0) {
            boost::weak_ptr<CharaColliAttack> attack(AttackOf(info));
            boost::shared_ptr<CharaColliAttack> attackPtr = attack.lock();
            if (attackPtr.get() != 0) {
                attackPtr->field_0x54 = hits;
                attackPtr->field_0x55 = hittable;
            }
        }
    }
}

extern "C" void fn_801A9784(EnemyCharaColliModule *module, int kind) {
    if (module->m_collider.get() != 0) {
        CharaColliBaseCharInfo *info = dynamic_cast<CharaColliBaseCharInfo *>(module->m_collider.get());
        if (info != 0) {
            boost::weak_ptr<CharaColliAttack> attack(AttackOf(info));
            boost::shared_ptr<CharaColliAttack> attackPtr = attack.lock();
            if (attackPtr.get() != 0) {
                attackPtr->m_kind = kind;
            }
        }
    }
}

extern "C" void fn_801A96C4(EnemyCharaColliModule *module, u32 value) {
    if (module->m_collider.get() != 0) {
        CharaColliBaseCharInfo *info = dynamic_cast<CharaColliBaseCharInfo *>(module->m_collider.get());
        if (info != 0) {
            boost::weak_ptr<CharaColliAttack> attack(AttackOf(info));
            boost::shared_ptr<CharaColliAttack> attackPtr = attack.lock();
            if (attackPtr.get() != 0) {
                *reinterpret_cast<u32 *>(attackPtr->field_0x3C) = value;
            }
        }
    }
}

extern "C" void fn_801A95F8(EnemyCharaColliModule *module, bool hits, bool hittable) {
    if (module->m_collider.get() != 0) {
        CharaColliBaseCharInfo *info = dynamic_cast<CharaColliBaseCharInfo *>(module->m_collider.get());
        if (info != 0) {
            boost::weak_ptr<CharaColliAttack> attack(AttackOf(info));
            boost::shared_ptr<CharaColliAttack> attackPtr = attack.lock();
            if (attackPtr.get() != 0) {
                attackPtr->field_0x54 = hits;
                attackPtr->field_0x55 = hittable;
            }
        }
    }
}

/* The collider's size, attributes and switches (names inferred). */
extern "C" void fn_801A95B4(EnemyCharaColliModule *module, float size) {
    if (module->m_collider.get() != 0) {
        module->m_collider->SetSize(size, 0.0f, 0.0f, -2);
    }
}

extern "C" void fn_801A9578(EnemyCharaColliModule *module, u32 attributes) {
    if (module->m_collider.get() != 0) {
        module->m_collider->SetAttributes(attributes, -1);
    }
}

extern "C" void fn_801A9540(EnemyCharaColliModule *module, int arg) {
    if (module->m_collider.get() != 0) {
        module->m_collider->TurnOn(arg);
    }
}

extern "C" void fn_801A9508(EnemyCharaColliModule *module, int arg) {
    if (module->m_collider.get() != 0) {
        module->m_collider->TurnOff(arg);
    }
}

/* The attack callback: records where the hit came from and whether the player made it. */
void EnemyCharaColliModule::OnHit(AttackCallbackParam &param) {
    boost::shared_ptr<CharInfo> attacker = param.m_attacker.lock()->m_owner.lock();
    fn_800091D0(reinterpret_cast<Vec3f *>(&param.field_0x8[0x34 - 0x8]),
                &(const Vec3f &)((VectorSubtractFn)fn_8040CD74)(
                    reinterpret_cast<Vec3f *>(&param.field_0x8[0x18 - 0x8]), &attacker->m_position));
    if (m_enemy->m_status->vfStat_0x44()) {
        boost::shared_ptr<CharInfo> owner =
            reinterpret_cast<boost::weak_ptr<CharaColliAttack> *>(&param.field_0x8[0])->lock()->m_owner.lock();
        if (owner.get() != 0 && owner->field_0x5C == 1) {
            param.field_0x8[0x30 - 0x8] = 1;
        }
    }
}

void EnemyCharaColliModule::OnAttacked(AttackCallbackParam &param) {
    param.m_result = 5;
    m_enemy->m_status->vfStat_0x70(param);
}

/* EnemyMotionSequenceModule (Game/Enemy.h). */
bool EnemyMotionSequenceModule::vf_0x0C() {
    int count;
    bool changed = false;
    int flags = reinterpret_cast<const s8 *>(m_sequence->steps)[m_step * 8 + 7];
    if (flags & 2) {
        if (!m_enemy->m_status->vfStat_0x54()) {
            changed = true;
        }
    } else if (flags & 4) {
        count = field_0x30;
        if (count > fn_8040EA90(4) + 2) {
            changed = true;
        }
    }
    return changed;
}

/* Reset returns whether a sequence started, which Game/MotionSequenceModule.h does not declare, so
 * it is defined under its symbol (as in Game/EnemyBaseAI.cpp). */
typedef int (*StartSequenceFn)(MotionSequenceModule *module, const MotionSequence *sequence);

/* Starts sequence index when the table has it (as MotionSequenceModule::Start, returning the
 * result; name inferred). */
inline int StartSequence(MotionSequenceModule *module, int index) {
    if (module->m_table != 0 && index < module->m_table->count) {
        module->m_index = index;
        return ((StartSequenceFn)fn_800C961C)(module, &module->m_table->sequences[index]);
    }
    return 0;
}

extern "C" int Reset__25EnemyMotionSequenceModuleFv(EnemyMotionSequenceModule *module) {
    module->field_0x4 = 0;
    int index = module->m_enemy->m_para->vfPara_0x48();
    const MotionSequenceTable *table = module->m_table;
    int result = 0;
    if (table != 0 && index < table->count) {
        module->m_index = index;
        result = ((StartSequenceFn)fn_800C961C)(module, &module->m_table->sequences[index]);
    }
    return result;
}

/* Takes the sequence table from the enemy's parameters and starts it (name inferred). */
extern "C" void fn_801A9214(EnemyMotionSequenceModule *module) {
    module->m_table = reinterpret_cast<const MotionSequenceTable *>(module->m_enemy->m_para->vfPara_0x44());
    module->m_defaultIndex = module->m_enemy->m_para->vfPara_0x48();
    module->Reset();
}

void EnemyStatusCommon::vfStat_0x08() {
    fn_801A9214(&m_motion);
    field_0x78 = field_0x7C = m_enemy->m_para->vfPara_0x6C();
    m_target = EnemyTarget::Instantiate(reinterpret_cast<UnkEnemy_8017D014 *>(m_enemy),
                                        reinterpret_cast<const EnemyTargetEntry *>(m_enemy->m_para->vfPara_0x74()));
}

void EnemyStatusCommon::vfStat_0x0C() {
    field_0x8.Create();
    field_0x8.m_onContact =
        boost::function1<void, EnemyColliDetectArgs &>(boost::bind(&EnemyStatusCommon::vfStat_0xD0, this, _1));
}

/* The character's push vector (name inferred). */
extern "C" Vec3f *fn_801A89F0(Enemy *enemy) {
    return reinterpret_cast<Vec3f *>(reinterpret_cast<u8 *>(enemy->m_chara.get()) + 0x6C);
}

extern "C" void fn_801A89CC(EnemyStatusCommon *status, bool hits, bool hittable) {
    fn_801A9844(&status->field_0x8, hits, hittable);
}

/* The values the flags take (bytes in .sdata; names unknown). Defined here, after the callback
 * binders' type records, as retail orders them. */
#pragma explicit_zero_data on
u8 lbl_805E8278 = 1;
u8 lbl_805E8279 = 0;
u8 lbl_805E827A = 0;
u8 lbl_805E827B = 1;
#pragma explicit_zero_data reset

/* A flag pair taking a new value (name inferred). */
inline void Push(EnemyStatusFlag &flag, bool value) {
    flag.m_previous = flag.m_current;
    flag.m_current = value;
}

/* Slot 0x10, the update before the motion: it takes the frame time, which Game/Enemy.h does not
 * declare, so it is defined under its symbol (as in Game/EnemyBaseAI.cpp). */
extern "C" void vfStat_0x10__17EnemyStatusCommonFv(EnemyStatusCommon *status, float dt) {
    fn_800091D0(fn_801A89F0(status->m_enemy), status->m_enemy->GetPosition());
    fn_8040CE60(fn_80181F0C(status->m_enemy));
    if ((status->field_0x94 -= dt) < 0.0f) {
        status->field_0x94 = 0.0f;
    }
    status->field_0x8.Update();
    Push(status->field_0x8E, !status->m_enemy->m_move->vfMove_0x18());
    status->field_0x8A.m_previous = status->field_0x8A.m_current;
    status->field_0x8A.m_current = status->field_0x74;
    if (status->field_0x75) {
        status->vfStat_0xC0();
        status->m_enemy->m_move->vfMove_0x34();
        status->field_0x8C.m_previous = status->field_0x8C.m_current;
        status->field_0x8C.m_current = lbl_805E8278;
    } else {
        status->field_0x8C.m_previous = status->field_0x8C.m_current;
        status->field_0x8C.m_current = lbl_805E8279;
    }
    if (status->m_enemy->m_move->vfMove_0x10() && status->m_enemy->m_status->vfStat_0x34()) {
        status->field_0x7C = 0.0f;
    }
    if (status->field_0x7C > 0.0f) {
        status->field_0x90.m_previous = status->field_0x90.m_current;
        status->field_0x90.m_current = lbl_805E827A;
    } else {
        status->field_0x90.m_previous = status->field_0x90.m_current;
        status->field_0x90.m_current = lbl_805E827B;
    }
    if (status->vfStat_0x58() && !status->vfStat_0x6C()) {
        {
            if (!(u8)status->vfStat_0xC4()) {
                status->vfStat_0x94(1);
            }
            status->vfStat_0xA0();
            if (status->m_enemy->m_type == 0) {
                fn_801A89CC(status, 0, 1);
            } else {
                status->m_enemy->m_set->vfSet_0x14();
                fn_801A89CC(status, 0, 0);
            }
            status->m_target->TurnOff();
        }
    } else if (!status->vfStat_0x58() && status->vfStat_0x6C()) {
        status->vfStat_0x94(2);
        status->field_0x76 = 0;
        fn_8006127C(&status->m_flags.m_bits, 0xC);
        status->vfStat_0xA4();
        fn_801A89CC(status, 1, 1);
        status->m_target->TurnOn();
    }
}

/* Reports the enemy's end to the registry (flags: 1 destroyed, 2 taken; name inferred). */
void EnemyStatusCommon::vfStat_0x94(int flags) {
    u32 result = flags;
    if (result == 1 && fn_80014E34(&m_flags.m_bits, 0xC)) {
        result |= 4;
    }
    UnkPlacement_800C9CD0 *placement = reinterpret_cast<UnkPlacement_800C9CD0 *>(m_enemy->m_set->vfSet_0x2C());
    int type = m_enemy->m_para->vfPara_0x08();
    fn_801A2150(fn_80119178(), placement, type, result);
}

/* Slots 0x98 and 0x9C reward the attacker; they take it, which Game/Enemy.h does not declare (see
 * ENEMY_STAT_SLOT_ARGS there), so they are defined under their symbols. */
extern "C" void vfStat_0x98__17EnemyStatusCommonFv(EnemyStatusCommon *status,
                                                    const boost::shared_ptr<CharInfo> &attacker) {
    if (attacker.get() != 0) {
        int reward = status->vfStat_0xB8();
        fn_8020642C(attacker, reward);
    }
}

extern "C" void vfStat_0x9C__17EnemyStatusCommonFv(EnemyStatusCommon *status,
                                                    const boost::shared_ptr<CharInfo> &attacker) {
    switch (status->m_enemy->m_para->vfPara_0x08()) {
    case 0:
        fn_8020642C(attacker, 0x57);
        break;
    case 1:
        if (fn_801A2030()) {
            fn_8020642C(attacker, 0x6E);
        } else {
            fn_8020642C(attacker, 0x68);
        }
        break;
    case 2:
        fn_8020642C(attacker, 0x62);
        break;
    }
}

void EnemyStatusCommon::vfStat_0xA0() {
    fn_801A9578(&field_0x8, 0x100000);
}

void EnemyStatusCommon::vfStat_0xA4() {}

void EnemyStatusCommon::vfStat_0xC0() {}

bool EnemyStatusCommon::vfStat_0xC8() {
    if (field_0x75 || field_0x76) {
        return true;
    }
    return false;
}

bool EnemyStatusCommon::vfStat_0x44() {
    return field_0x94 > 0.0f;
}

void EnemyStatusCommon::vfStat_0x14() {
    fn_800C9554(&m_motion, reinterpret_cast<UnkEnemyMotion_801A83D8 *>(m_enemy->m_disp->vfDisp_0x1C())->m_work);
}

/* Slot 0x18 takes the frame time (see slot 0x10). */
extern "C" void vfStat_0x18__17EnemyStatusCommonFv(EnemyStatusCommon *status, float dt) {
    UnkEnemyMotion_801A83D8 *motion =
        reinterpret_cast<UnkEnemyMotion_801A83D8 *>(status->m_enemy->m_disp->vfDisp_0x1C());
    fn_800C9178(&status->m_motion, motion->m_work, motion->m_base, dt);
}

/* Slot 0x78 takes the damage and returns whether it was the last (see slot 0x10). */
extern "C" bool vfStat_0x78__17EnemyStatusCommonFv(EnemyStatusCommon *status, float damage) {
    if (damage > 0.0f) {
        status->field_0x74 = 1;
        status->field_0x94 += 1.0f;
        if ((status->field_0x7C -= damage) <= 0.0f) {
            status->field_0x7C = 0.0f;
            return true;
        }
    }
    return false;
}

void EnemyStatusCommon::vfStat_0x1C() {
    if (m_target.get() != 0) {
        m_target->Tick();
    }
    field_0x74 = 0;
    field_0x75 = 0;
    field_0x84 = -1;
}

/* The contact callback: pushes the enemy out of what it touches. */
void EnemyStatusCommon::vfStat_0xD0(EnemyColliDetectArgs &args) {
    if (field_0x77) {
        UnkCharaColliObject_8001E9C4 *object;
        UnkColliContacts_8001EE04 *contacts = args.m_contacts;
        for (UnkColliContactNode_800587A0 *it = fn_8001EE04(contacts); it != fn_8001EDEC(contacts); fn_80027398(&it)) {
            object = it->m_object;
            if (object != 0 && fn_80026588(it) != 0 && !(fn_80026588(it) & 0x40000) &&
                !(fn_80026588(it) & 0x100000)) {
                Vec3f push;
                fn_800091EC(&push, fn_8001E9C4(object));
                fn_8040CE38(&push, -fn_8001E9E8(reinterpret_cast<u8 *>(object) + 0xC));
                push.y = 0.0f;
                fn_8040CFF8(fn_80181F0C(m_enemy), &push);
            }
        }
    }
}

/* Takes a hit. */
void EnemyStatusCommon::vfStat_0x70(AttackCallbackParam &param) {
    param.m_result = 1;
    boost::shared_ptr<CharInfo> attacker = param.m_attacker.lock()->m_owner.lock();
    float health = field_0x7C;
    if (!field_0x76 && reinterpret_cast<EnemyStatFuncCalls *>(this)->vfStat_0x78(param.m_damage)) {
        field_0x76 = 1;
        if (attacker.get() != 0) {
            reinterpret_cast<EnemyStatFuncCalls *>(this)->vfStat_0x98(attacker);
            if (fn_8021C958(attacker)) {
                m_flags.Set(0xC);
            }
        }
        field_0x75 = 1;
    }
    float threshold = vfStat_0xCC();
    if (health > threshold && field_0x7C <= threshold) {
        field_0x75 = 1;
    }
    if (field_0x74) {
        reinterpret_cast<EnemyDispFuncCalls *>(m_enemy->m_disp)
            ->vfDisp_0x48(reinterpret_cast<Vec3f *>(&param.field_0x8[0x18 - 0x8]),
                          reinterpret_cast<Vec3f *>(&param.field_0x8[0x34 - 0x8]));
    }
    if (std::fabs(fn_8019B184(m_enemy, reinterpret_cast<Vec3f *>(&param.field_0x8[0x34 - 0x8]))) <
        0.75f * lbl_805FAD38) {
        field_0x84 = 0;
        if (vfStat_0xC8()) {
            float angle = fn_8019B1D8(m_enemy, reinterpret_cast<Vec3f *>(&param.field_0x8[0x34 - 0x8]));
            m_enemy->GetRotation()->y = angle;
        }
    } else {
        field_0x84 = 1;
        if (vfStat_0xC8()) {
            float angle = fn_8019B1D8(
                m_enemy, &(const Vec3f &)fn_80016284(reinterpret_cast<Vec3f *>(&param.field_0x8[0x34 - 0x8])));
            m_enemy->GetRotation()->y = angle;
        }
    }
}

void EnemyStatusCommon::vfStat_0x38() {
    Vec3f *position = m_enemy->GetPosition();
    if (m_enemy->field_0xA8.Contains(position)) {
        vfStat_0x30(1);
        vfStat_0x3C(0);
    }
}

/* Slot 0x90 switches the targets on or off; it takes the switch (see slot 0x10). */
extern "C" void vfStat_0x90__17EnemyStatusCommonFv(EnemyStatusCommon *status, bool on) {
    if (status->m_target.get() != 0) {
        if (on) {
            status->m_target->TurnOn();
        } else {
            status->m_target->TurnOff();
        }
    }
}
