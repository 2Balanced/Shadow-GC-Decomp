#ifndef GAME_ENEMY_H
#define GAME_ENEMY_H

/* The enemy framework (class names from RTTI). An enemy object is assembled from interfaces, one
 * per concern, each with a "Common" implementation:
 *
 *   EnemyParaFunc  -> EnemyParamCommon   parameters        (Enemy::m_para,   0x00)
 *   EnemyStatFunc  -> EnemyStatusCommon  status            (Enemy::m_status, 0x04)
 *   EnemyDispFunc  -> EnemyDispCommon    model and motion  (Enemy::m_disp,   0x08)
 *   EnemyMoveFunc  -> EnemyMoveCommon    movement          (Enemy::m_move,   0x0C)
 *   EnemyCtrlFunc  -> EnemyBaseAI        control (AI)      (Enemy::m_ctrl,   0x10)
 *   EnemySetFunc   -> TEnemySetBase      placement         (Enemy::m_set,    0x14)
 *
 * Enemy holds one pointer per interface (the six Enemy*ObjBase bases; they start out pointing at
 * static do-nothing objects) and the derived classes point them at their own parts. Every "Common"
 * part and EnemyRefBase derive virtually from EnemyReferer, which points back at the Enemy, so a
 * part reaches the others through m_enemy. The concrete enemy is EnemyTemplate<Base, AI>: the
 * enemy's body (e.g. BkSoldierBase), its AI (e.g. BkSoldierAI) and TEnemySetTask, the Task that
 * runs it.
 *
 * MWCC layout rules seen here: a class's vtable pointer sits where it declares its first virtual
 * function (after the data members of the class that introduces it); a class that names a
 * virtual base gets a pointer to it (here always EnemyReferer) after its bases; pure virtual slots
 * are null; and a derived class appends a slot to its primary vtable for every virtual of a
 * secondary base it overrides, so the slot lists below must keep their order.
 *
 * Slot functions are named vf_0x<offset> after their byte offset in the introducing class's
 * vtable (the 8-byte header included) unless their use is known; member names marked "inferred"
 * or field_0x<offset> are not original. Functions whose addresses are given are defined in other
 * units; the inline ones are weak copies, first emitted in Game/BkSoldier.cpp. */

#include "Game/CharaColliAttack.h"
#include "Game/CollisionEffect.h"
#include "Game/Fn8040CopyVector3.h"
#include "Game/MathTypes.h"
#include "Game/MotionSequenceModule.h"
#include "Game/Task.h"
#include "boost/shared_ptr.hpp"
#include "boost/weak_ptr.hpp"
#include <dolphin/types.h>
#include "size_assert.h"

class Enemy;
class CharInfo;
class EnemyTarget;
class EnemyCtrlFunc;
class EnemyRender;
struct EnemyMotionEventArg;
namespace Vehicle {
class TVehicleBase;
}

/* The character info an enemy hands out (class name unknown in this project so far; it is most
 * likely EnemyCharInfo, whose shared_ptr control block RTTI the game has, but the boost instances
 * already carry this name, see Game/EnemyTarget.cpp). */
#include "Game/CharInfo.h"
class UnkEnemyInfo_8017D014 : public CharInfo {};

/* The placement record an enemy's parameters are read from (layout unknown). */
class UnkPlacement_800C9CD0;

extern "C" {
/* Flag word helpers (see Game/BKTankController.h). */
void fn_80014E04(u32 *flags);
void fn_80014E1C(u32 *flags, int bit);
}

/* A 32-bit flag word (struct name unknown; see Game/BKTankController.h). */
struct EnemyFlags {
    EnemyFlags() {
        fn_80014E04(&m_bits);
    }
    void Set(int bit) {
        fn_80014E1C(&m_bits, bit);
    }

    u32 m_bits;
};

/* EnemyReferer (class name from RTTI): the back pointer from an enemy's parts to the enemy. Every
 * part derives from it virtually, so an enemy holds one, at its end. Its constructor leaves the
 * byte at 0x4 alone; EnemyMotionSequenceModule clears it (meaning unknown). */
class EnemyReferer {
  public:
    EnemyReferer() : m_enemy(0) {}
#ifdef ENEMY_REFERER_DTOR
    /* An empty inline destructor (not original as far as is known). EnemyTemplate's destructor
     * then calls an inline function, so GC/2.7 keeps it out of line where retail does (EggPawn);
     * the linker strips the unreferenced weak copy it adds. Behind a macro, so other units' @N
     * locals do not renumber. */
    ~EnemyReferer() {}
#endif

    /* 0x0 */ Enemy *m_enemy;
    /* 0x4 */ u8 field_0x4;
};

/* ------------------------------------------------------------------------------------------- */
/* The interfaces. Their own vtables are in the Enemy unit (around 0x8019B350); the do-nothing
 * objects the Enemy*ObjBase pointers start with are statics of that unit. */

/* EnemyParaFunc (class name from RTTI; vtable 0x8053A538): the enemy's parameters. */
class EnemyParaFunc {
  public:
    virtual int vfPara_0x08();    /* 0x8019B464 */
    virtual int vfPara_0x0C();    /* 0x8019B46C */
    virtual int vfPara_0x10();    /* 0x8019B474 */
    virtual int vfPara_0x14();    /* 0x8019B47C */
    virtual int vfPara_0x18();    /* 0x8019B484 */
    virtual int vfPara_0x1C();    /* 0x8019B48C */
    virtual float vfPara_0x20();  /* 0x80181F04 */
    virtual float vfPara_0x24();  /* 0x8019B494 */
    virtual float vfPara_0x28();  /* 0x8019B49C */
    virtual float vfPara_0x2C();  /* 0x8019B4A4 */
    virtual float vfPara_0x30();  /* 0x8019B4AC */
    virtual float vfPara_0x34();  /* 0x8019AD04 */
    virtual int vfPara_0x38();    /* 0x8019B4B4 */
    virtual int vfPara_0x3C();    /* 0x80192B58 */
    virtual void vfPara_0x40(int value); /* 0x8019B4BC */
    virtual int vfPara_0x44();    /* 0x8019B4C0 */
    virtual int vfPara_0x48();    /* 0x8017E14C */
    virtual int vfPara_0x4C();    /* 0x8017EE98 */
    virtual int vfPara_0x50();    /* 0x8019B4C8 */
    virtual int vfPara_0x54();    /* 0x8018019C */
    virtual int vfPara_0x58();    /* 0x8017FA78 */
    virtual int vfPara_0x5C();    /* 0x8019B4D0 */
    virtual int vfPara_0x60();    /* 0x80180C58 */
    virtual int vfPara_0x64();    /* 0x8019B4D8 */
    virtual float vfPara_0x68();  /* 0x8019B4E0 */
    virtual float vfPara_0x6C();  /* 0x8019B4E8 */
    virtual int vfPara_0x70();    /* 0x8019B4F0 */
    virtual int vfPara_0x74();    /* 0x8019B4F8 */
};
SIZE_ASSERT(EnemyParaFunc, 0x4);

/* EnemyStatFunc (class name from RTTI; vtable 0x805316AC, emitted with Game/BkSoldier.cpp): the
 * enemy's status: health, damage, flags. */
class EnemyStatFunc {
  public:
    virtual void vfStat_0x08() {}
    virtual void vfStat_0x0C() {}
    virtual void vfStat_0x10() {}
    virtual void vfStat_0x14() {}
    virtual void vfStat_0x18() {}
    virtual void vfStat_0x1C() {}
    /* Starts motion sequence motion, again from its start when restart is set (behaviour
     * inferred from EnemyStatusCommon). */
    virtual int vfStat_0x20(int motion, bool restart) {
        return 0;
    }
    virtual int vfStat_0x24() {
        return -1;
    }
    virtual bool vfStat_0x28() {
        return 0;
    }
    virtual bool vfStat_0x2C() {
        return 0;
    }
    virtual void vfStat_0x30(bool value) {}
    virtual bool vfStat_0x34() {
        return 0;
    }
    virtual void vfStat_0x38() {}
    virtual void vfStat_0x3C(bool value) {}
    virtual bool vfStat_0x40() {
        return 0;
    }
    virtual bool vfStat_0x44() {
        return 0;
    }
    virtual bool vfStat_0x48() {
        return 0;
    }
    virtual bool vfStat_0x4C() {
        return 0;
    }
    virtual bool vfStat_0x50() {
        return 0;
    }
    virtual bool vfStat_0x54() {
        return 0;
    }
    virtual bool vfStat_0x58() {
        return 0;
    }
    virtual bool vfStat_0x5C() {
        return 0;
    }
    virtual bool vfStat_0x60() {
        return 0;
    }
    virtual bool vfStat_0x64() {
        return 0;
    }
    virtual bool vfStat_0x68() {
        return 0;
    }
    virtual bool vfStat_0x6C() {
        return 0;
    }
    /* Takes a hit (behaviour inferred from EnemyStatusCommon and BkSoldierBase). */
    virtual void vfStat_0x70(SonicteamUSA::System::CharaColli::AttackCallbackParam &param) {}
    virtual int vfStat_0x74() {
        return 0;
    }
    virtual bool vfStat_0x78() {
        return 0;
    }
    virtual void vfStat_0x7C(bool arg0, bool arg1) {}
    virtual void vfStat_0x80(bool value) {}
    virtual void vfStat_0x84(int kind) {}
    virtual void vfStat_0x88(u32 value) {}
    virtual void vfStat_0x8C(bool value) {}
#ifdef ENEMY_STAT_0x90_ARG
    /* The slot takes a flag (BkWormAI passes 0 or 1). Behind a macro because the symbols of this
     * slot carry the argument-less signature; only units that need the argument define it. */
    virtual void vfStat_0x90(bool value) {}
#elif defined(ENEMY_STAT_SLOT_ARGS)
    /* The same, declared only (see ENEMY_STAT_SLOT_ARGS below). */
    virtual void vfStat_0x90(bool value);
#else
    virtual void vfStat_0x90() {}
#endif
    virtual void vfStat_0x94(int flags) {}
#ifdef ENEMY_STAT_SLOT_ARGS
    /* The slots take the character to reward: 0x98 for taking the enemy with the vacuum, 0x9C
     * for recovering it (EnemyStatusCommon passes it on to fn_8020642C; Game/EnemyBaseAI.cpp
     * calls them). Behind a macro because the symbols of these slots carry the argument-less
     * signature; only units that call them with the argument define it. Declared only, so that
     * those units emit no copies of the empty defaults under these signatures. */
    virtual void vfStat_0x98(const boost::shared_ptr<CharInfo> &attacker);
    virtual void vfStat_0x9C(const boost::shared_ptr<CharInfo> &attacker);
#else
    virtual void vfStat_0x98() {}
    virtual void vfStat_0x9C() {}
#endif
    virtual void vfStat_0xA0() {}
    virtual void vfStat_0xA4() {}
    virtual EnemyFlags *vfStat_0xA8() {
        static EnemyFlags s_flags;
        return &s_flags;
    }
    virtual float vfStat_0xAC() {
        return 0.0f;
    }
    virtual float vfStat_0xB0() {
        return 0.0f;
    }
    virtual void vfStat_0xB4(float dt) {}
};
SIZE_ASSERT(EnemyStatFunc, 0x4);

/* EnemyDispFunc (class name from RTTI; vtable 0x8053A480): the enemy's model and motion. Its data
 * comes before the vtable pointer at 0xC. */
class EnemyDispFunc {
  public:
    /* 0x0 */ Vec3f field_0x0;

    virtual void vfDisp_0x08();             /* 0x8019B3A0 */
    virtual void vfDisp_0x0C();             /* 0x8019B3A4 */
    /* Per-frame update (dt in seconds; behaviour inferred). */
    virtual void vfDisp_0x10(float dt);     /* 0x8019B3A8 */
    virtual int vfDisp_0x14();              /* 0x8019B3AC */
    virtual int vfDisp_0x18();              /* 0x8019B3B4 */
    virtual void *vfDisp_0x1C();            /* 0x8017CC74 */
    virtual boost::shared_ptr<EnemyRender> vfDisp_0x20(); /* 0x80181038 */
    /* The matrix of model part index, or null (name from Game/EnemyTarget.cpp). */
    virtual Mtx34f *GetPartMatrix(int index); /* 0x8019448C */
    /* Two vectors of the model (meanings unknown; by default field_0x0). */
    virtual Fn8040CopyVector3 vfDisp_0x28(); /* 0x8019B3BC */
    virtual Fn8040CopyVector3 vfDisp_0x2C(); /* 0x8019B3DC */
    /* Called with a turn's cosine (0.2 for a turn of more than 90 degrees; behaviour inferred from
     * Enemy::vf_0x0C). */
    virtual void vfDisp_0x30(float value);  /* 0x80188E0C */
    virtual float vfDisp_0x34();            /* 0x8019B3FC */
    /* Places the model at position with rotation (behaviour inferred). */
    virtual void vfDisp_0x38(const Vec3f *position, const Vec3f *rotation); /* 0x8019B404 */
    /* Whether the enemy is moving (behaviour inferred from Enemy::vf_0x0C). */
    virtual void vfDisp_0x3C(bool moving);  /* 0x8017F234 */
    /* A motion event (behaviour inferred). */
    virtual void vfDisp_0x40(EnemyMotionEventArg &arg); /* 0x8019B408 */
    virtual bool vfDisp_0x44(int *command);  /* 0x8019B40C */
    virtual void vfDisp_0x48();             /* 0x8019B414 */
    virtual void vfDisp_0x4C();             /* 0x801834D4 */
    virtual void vfDisp_0x50();             /* 0x801812C8 */
    virtual void vfDisp_0x54();             /* 0x8019B418 */
    virtual void vfDisp_0x58(bool value);   /* 0x8019B41C */
};
SIZE_ASSERT(EnemyDispFunc, 0x10);

/* EnemyMoveFunc (class name from RTTI; vtable 0x8053A500): the enemy's movement. */
class EnemyMoveFunc {
  public:
    virtual void vfMove_0x08();         /* 0x8019B438 */
    /* Per-frame movement (dt in seconds; behaviour inferred). */
    virtual void vfMove_0x0C(float dt); /* 0x8019B43C */
    virtual bool vfMove_0x10();         /* 0x80187CBC */
    virtual bool vfMove_0x14();         /* 0x8019B440 */
    virtual bool vfMove_0x18();         /* 0x8019588C */
    virtual int vfMove_0x1C();          /* 0x8019B448 */
    virtual int vfMove_0x20();          /* 0x8019B450 */
    virtual float vfMove_0x24();        /* 0x8017B5A0 */
    virtual float vfMove_0x28();        /* 0x8019B458 */
    virtual int vfMove_0x2C();          /* 0x8017CAC0 */
    virtual void *vfMove_0x30();        /* 0x8017CAC8 */
    virtual void vfMove_0x34();         /* 0x8019B460 */
};
SIZE_ASSERT(EnemyMoveFunc, 0x4);

/* EnemySetFunc (class name from RTTI; vtable 0x8053A450): the enemy's placement. */
class EnemySetFunc {
  public:
    virtual bool vfSet_0x08() {
        return 0;
    }
    virtual bool vfSet_0x0C(); /* 0x8019B350 */
    virtual void vfSet_0x10(); /* 0x80180BE4 */
    virtual void vfSet_0x14(); /* 0x8019B358 */
    virtual void vfSet_0x18(); /* 0x80187CB8 */
    /* Places the enemy from placement (behaviour inferred). */
    virtual void vfSet_0x1C(UnkPlacement_800C9CD0 *placement); /* 0x8019B35C */
    /* Reads the enemy's parameters from placement (behaviour inferred). */
    virtual void vfSet_0x20(UnkPlacement_800C9CD0 *placement); /* 0x8019B360 */
    virtual void vfSet_0x24(); /* 0x8019B364 */
    virtual bool vfSet_0x28(); /* 0x8019B368 */
    virtual int vfSet_0x2C();  /* 0x8017D788 */
};
SIZE_ASSERT(EnemySetFunc, 0x4);

/* EnemyCtrlFunc (class name from RTTI; vtable 0x8053A4DC): the enemy's control, its AI. Its data
 * comes before the vtable pointer at 0x18. */
class EnemyCtrlFunc {
  public:
    /* Inline; first emitted with Game/Enemy.cpp (0x8019B370). */
    EnemyCtrlFunc() : field_0xC(0.0f, 0.0f, 0.0f) {}

    /* 0x00 */ Vec3f field_0x0; /* what vfCtrl_0x1C returns by default */
    /* 0x0C */ Vec3f field_0xC;

    virtual void vfCtrl_0x08();  /* 0x8019B420 */
    virtual void vfCtrl_0x0C();  /* 0x8019B424 */
    virtual void *vfCtrl_0x10(); /* 0x8017C3C8 */
    virtual void *vfCtrl_0x14(); /* 0x8019B428 */
    virtual float vfCtrl_0x18(); /* 0x8019B430 */
    virtual Vec3f vfCtrl_0x1C(float arg); /* 0x80196628 */
    virtual void vfCtrl_0x20();  /* 0x8017C680 */
};
SIZE_ASSERT(EnemyCtrlFunc, 0x1C);

/* A 3x4 matrix whose copy goes through fn_8040C148, the game's out-of-line matrix copy (not
 * original; it reproduces the copies the enemies' matrix code makes). */
struct Matrix34 : public Mtx34f {
    Matrix34() {}
    Matrix34(const Matrix34 &other) {
        fn_8040C148(this, &other);
    }
};

extern "C" {
/* Rotation matrix from Euler angles (see Game/MathTypes.h fn_8001E070; behaviour inferred). */
void fn_800193B8(Mtx34f *m, const Vec3f *eulerAngles, int order);
}

/* The rotation matrix of eulerAngles (inline; weak copy 0x8018F870, also used by GunSoldier;
 * name inferred). */
inline Matrix34 MakeRotation(const Vec3f *eulerAngles, int order) {
    Matrix34 m;
    fn_800193B8(&m, eulerAngles, order);
    return m;
}

/* ------------------------------------------------------------------------------------------- */
/* The Enemy: one pointer per interface. */

/* The do-nothing parts the pointers start with (statics of the Enemy unit). */
extern EnemyParaFunc lbl_805EF6E8;
extern EnemyStatFunc lbl_805EF6EC;
extern EnemyDispFunc lbl_80580F78;
extern EnemyMoveFunc lbl_805EF6F0;
extern EnemyCtrlFunc lbl_80580F88;
extern EnemySetFunc lbl_805EF6F4;

/* Enemy*ObjBase (class names from RTTI): one interface pointer each. */
class EnemyParamObjBase {
  public:
    EnemyParamObjBase() : m_para(&lbl_805EF6E8) {}

    /* 0x0 */ EnemyParaFunc *m_para;
};

class EnemyStatusObjBase {
  public:
    EnemyStatusObjBase() : m_status(&lbl_805EF6EC) {}

    /* 0x0 */ EnemyStatFunc *m_status;
};

class EnemyDispObjBase {
  public:
    EnemyDispObjBase() : m_disp(&lbl_80580F78) {}

    /* 0x0 */ EnemyDispFunc *m_disp;
};

class EnemyMoveObjBase {
  public:
    EnemyMoveObjBase() : m_move(&lbl_805EF6F0) {}

    /* 0x0 */ EnemyMoveFunc *m_move;
};

class EnemyCtrlObjBase {
  public:
    EnemyCtrlObjBase() : m_ctrl(&lbl_80580F88) {}

    /* 0x0 */ EnemyCtrlFunc *m_ctrl;
};

class EnemySetObjBase {
  public:
    EnemySetObjBase() : m_set(&lbl_805EF6F4) {}

    /* 0x0 */ EnemySetFunc *m_set;
};

#ifdef ENEMY_AREA_PARTS
/* The two halves of UnkEnemyArea_8018A374 as Game/BkLarva.cpp, which emits the area's
 * constructor (0x8018A374), sees them: their implicit constructors are the empty functions
 * 0x8018A3D0 and 0x8018A3CC (layouts and names unknown; the Vec3f members give the constructors
 * their bodies). The whole area is then 0x44 bytes, which is what Enemy's field_0xA8 and
 * field_0xCC cover. */
struct UnkEnemyAreaShape {
    /* 0x00 */ Vec3f field_0x0;
    /* 0x0C */ Vec3f field_0xC;
    /* 0x18 */ u8 field_0x18[0x20 - 0x18];
};

struct UnkEnemyAreaBounds {
    /* 0x00 */ Vec3f field_0x0;
    /* 0x0C */ Vec3f field_0xC;
    /* 0x18 */ Vec3f field_0x18;
};

struct UnkEnemyArea_8018A374 {
    UnkEnemyArea_8018A374() {
        memset(&m_shape, 0, sizeof(m_shape));
        memset(&m_bounds, 0, sizeof(m_bounds));
    }
    /* Whether key is in the area (0x801A68C0; behaviour inferred). */
    u8 Contains(void *key);
    /* The float at 0x18 (see the other definition below). */
    float GetRange() {
        return *reinterpret_cast<float *>(&m_shape.field_0x18[0]);
    }

    /* 0x00 */ UnkEnemyAreaShape m_shape;
    /* 0x20 */ UnkEnemyAreaBounds m_bounds;
};
#else
/* A 0x24-byte value at Enemy+0xA8 (constructor 0x8018A374 clears it; class name unknown). */
struct UnkEnemyArea_8018A374 {
    UnkEnemyArea_8018A374();
    /* Whether key is in the area (0x801A68C0; behaviour inferred). */
    u8 Contains(void *key);
    /* The float at 0x18 (name inferred: the area's radius). */
    float GetRange() {
        return *reinterpret_cast<float *>(&field_0x0[0x18]);
    }

    /* 0x00 */ u8 field_0x0[0x24];
};
#endif

extern "C" {
#ifndef UNK_TREE_MAP
/* The enemy registry (a singleton) and its add and remove calls (behaviour inferred). Not declared
 * where UNK_TREE_MAP is defined: Game/UnkSingletons.h declares the registry's accessor there, and
 * Game/EnemyEntry.cpp defines the two calls. */
void *fn_80119178(void);
u32 fn_801A29B0(void *registry, Enemy *enemy);
void fn_801A2968(void *registry, u32 handle);
#endif
/* The character info of an enemy of type type (behaviour inferred). */
boost::shared_ptr<UnkEnemyInfo_8017D014> fn_801A0030(int type);
int fn_8019A8F4(int type);
void fn_801DC984(UnkEnemyInfo_8017D014 *info, int arg);
/* Vec3f set to (1, 1, 1) (behaviour inferred). */
void fn_8040E970(Vec3f *vector);
void fn_8040CE60(Vec3f *vector);
}

/* A global the Enemy constructor reads (its word at 0x90; meaning unknown). */
extern u8 lbl_8057D6A0[0x150];

extern "C" {
/* The enemy's character info (0x8017D014; Game/EnemyTarget.cpp calls it by this name too). */
boost::weak_ptr<UnkEnemyInfo_8017D014> fn_8017D014(Enemy *enemy);
}

/* Enemy (class name from RTTI; vtable 0x8053A43C): the base of every enemy. type is the enemy's
 * character type. Constructor 0x8018F458 and destructor 0x8018E9B0 are inline (weak copies in
 * Game/BkSoldier.cpp); the virtuals 0x8019AD0C and 0x8019ABF4 are in the Enemy unit. */
class Enemy : public EnemyParamObjBase,
              public EnemyStatusObjBase,
              public EnemyDispObjBase,
              public EnemyMoveObjBase,
              public EnemyCtrlObjBase,
              public EnemySetObjBase {
  public:
    Enemy(int type)
        : m_id(type + 0x99), m_type(type), m_motionBase(type + 7),
          field_0x2C(*reinterpret_cast<u32 *>(&lbl_8057D6A0[0x90])), field_0x60(0.0f, 0.0f, 0.0f),
          field_0x84(0.0f, 0.0f, 0.0f), field_0x9C(0.0f, 0.0f, 0.0f) {
        m_handle = fn_801A29B0(fn_80119178(), this);
        m_chara = fn_801A0030(m_id);
        fn_801DC984(m_chara.get(), fn_8019A8F4(type));
        m_charaRef = m_chara;
        fn_8040CE60(&field_0x48);
        fn_8040E970(&field_0x60);
        fn_8040CE60(&field_0x54);
        fn_8040CE60(&field_0x6C);
        fn_8040E970(&field_0x84);
        fn_8040CE60(&field_0x78);
        field_0xEC = 0.0f;
        field_0xF0 = 1.0f;
        field_0xF4 = 0;
        field_0xF8 = 0;
        field_0xFC = 0;
    }
    virtual ~Enemy() {
        fn_801A2968(fn_80119178(), m_handle);
    }
    virtual bool vf_0x0C(); /* 0x8019AD0C */
    virtual bool vf_0x10(); /* 0x8019ABF4 */

    /* Weak helpers emitted by the first enemy unit (0x8017C684, 0x8017C690, 0x8017C3B0; names
     * inferred). The enemy's character info comes from fn_8017D014 below. */
    Vec3f *GetRotation();
    Vec3f *GetPosition();
    float GetFrameTime();
    /* The vector at 0x60 (weak copy first at 0x80183EAC; name inferred: its y is the angle the
     * flyers float at). */
    Vec3f *GetHomeRotation() {
        return &field_0x60;
    }
    /* The second word of the frame data (0x8018F860, weak here; name inferred). */
    float GetFrameTime2() {
        return (*reinterpret_cast<float **>(m_chara->field_0x60))[1];
    }

    /* 0x1C */ s32 m_id;
    /* 0x20 */ s32 m_type;
    /* 0x24 */ s32 m_motionBase;
    /* 0x28 */ u32 m_handle;
    /* 0x2C */ u32 field_0x2C;
    /* 0x30 */ boost::shared_ptr<UnkEnemyInfo_8017D014> m_chara;
    /* 0x38 */ boost::weak_ptr<UnkEnemyInfo_8017D014> m_charaRef;
    /* 0x40 */ boost::weak_ptr<CharInfo> field_0x40;
    /* 0x48 */ Vec3f field_0x48;
    /* 0x54 */ Vec3f field_0x54;
    /* 0x60 */ Vec3f field_0x60;
    /* 0x6C */ Vec3f field_0x6C;
    /* 0x78 */ Vec3f field_0x78;
    /* 0x84 */ Vec3f field_0x84;
    /* 0x90 */ Vec3f field_0x90;
    /* 0x9C */ Vec3f field_0x9C;
    /* 0xA8 */ UnkEnemyArea_8018A374 field_0xA8;
#ifndef ENEMY_AREA_PARTS
    /* 0xCC */ u8 field_0xCC[0xEC - 0xCC];
#endif
    /* 0xEC */ float field_0xEC;
    /* 0xF0 */ float field_0xF0;
    /* 0xF4 */ u32 field_0xF4;
    /* 0xF8 */ u32 field_0xF8;
    /* 0xFC */ u32 field_0xFC;
};
SIZE_ASSERT(Enemy, 0x100);

/* EnemyRefBase (class name from RTTI): Enemy with its EnemyReferer. */
class EnemyRefBase : public Enemy, public virtual EnemyReferer {
  public:
    EnemyRefBase(int type) : Enemy(type) {
        m_enemy = this;
    }
    virtual ~EnemyRefBase() {}
};

/* ------------------------------------------------------------------------------------------- */
/* The common parts. */

/* EnemyParamCommon (class name from RTTI; vtable 0x8053AA90; constructor 0x801A654C, destructor
 * 0x801A6504). */
class EnemyParamCommon : public virtual EnemyReferer, public EnemyParaFunc {
  public:
    EnemyParamCommon();
    ~EnemyParamCommon();

    virtual int vfPara_0x08();            /* 0x801A64E4 */
    virtual int vfPara_0x0C();            /* 0x801A64AC */
    virtual int vfPara_0x10();            /* 0x801A6470 */
    virtual int vfPara_0x14();            /* 0x801A6434 */
    virtual int vfPara_0x18();            /* 0x801A63F0 */
    virtual int vfPara_0x1C();            /* 0x801A6384 */
    virtual float vfPara_0x20();          /* 0x801A6364 */
    virtual float vfPara_0x24();          /* 0x801A6340 */
    virtual float vfPara_0x28();          /* 0x801A631C */
    virtual float vfPara_0x2C();          /* 0x801A62F8 */
    virtual float vfPara_0x30();          /* 0x801A62D8 */
    virtual float vfPara_0x34();          /* 0x801A62B4 */
    virtual int vfPara_0x38();            /* 0x801A63C4 */
    virtual int vfPara_0x3C();            /* 0x801A63A4 */
    virtual void vfPara_0x40(int value);  /* 0x8017B8E4 */
    virtual int vfPara_0x44();            /* 0x8017B8EC */
    virtual int vfPara_0x48();            /* 0x80183F5C */
    virtual int vfPara_0x4C() {
        return 0;
    }
    virtual int vfPara_0x50() {
        return 0;
    }
    virtual int vfPara_0x54() {
        return 0;
    }
    virtual int vfPara_0x58() {
        return 0;
    }
    virtual int vfPara_0x5C() {
        return 0;
    }
    virtual int vfPara_0x60() {
        return 0;
    }
    virtual int vfPara_0x64() {
        return 0;
    }
    virtual float vfPara_0x68();          /* 0x801A6290 */
    virtual float vfPara_0x6C();          /* 0x801A6270 */
    virtual int vfPara_0x70();            /* 0x801A6250 */
    virtual int vfPara_0x74();            /* 0x801A6230 */

    /* 0x8 */ s32 field_0x8;
};

/* The object at EnemyStatusCommon+0x8 (constructor 0x801AA430, destructor 0x801AA384; class
 * name unknown). Its first word is the enemy. */
class UnkEnemyStatus_801AA430 {
  public:
    UnkEnemyStatus_801AA430();
    ~UnkEnemyStatus_801AA430();

    /* 0x00 */ Enemy *m_enemy;
    /* 0x04 */ u8 field_0x4[0x18 - 0x4];
};

/* EnemyMotionSequenceModule (class name from RTTI; vtable 0x8053ACE4): the motion sequence player
 * of an enemy. Constructor and destructor are inline (weak copies 0x8018F3AC, 0x8018EAA4). */
class EnemyMotionSequenceModule : public MotionSequenceModule, public EnemyReferer {
  public:
    EnemyMotionSequenceModule() {
        field_0x4 = 0;
    }
    virtual ~EnemyMotionSequenceModule() {}
    virtual bool vf_0x0C(); /* 0x801A9314 */
    virtual void Reset();   /* 0x801A9288 */
};

/* Their initial values (bytes in .sdata, defined in Game/BkSoldier.cpp; names unknown). */
extern u8 lbl_805E76E8;
extern u8 lbl_805E76E9;
extern u8 lbl_805E76EA;
extern u8 lbl_805E76EB;
extern u8 lbl_805E76EC;

/* A flag pair of EnemyStatusCommon: current and previous (names inferred). */
struct EnemyStatusFlag {
    void Set(u8 value) {
        m_previous = value;
        m_current = value;
    }

    /* 0x0 */ u8 m_current;
    /* 0x1 */ u8 m_previous;
};


/* EnemyStatusCommon (class name from RTTI; vtable 0x8053ABFC). Constructor and destructor are
 * inline (weak copies 0x8018F294 and 0x8018EA30). */
class EnemyStatusCommon : public virtual EnemyReferer, public EnemyStatFunc {
  public:
    EnemyStatusCommon() {
        field_0x8.m_enemy = m_enemy;
        m_motion.m_enemy = m_enemy;
        field_0x6C = 0;
        field_0x6D = 1;
        field_0x70 = -1;
        field_0x74 = 0;
        field_0x75 = 0;
        field_0x76 = 0;
        field_0x77 = 1;
        field_0x78 = 0.0f;
        field_0x7C = 0.0f;
        field_0x88.Set(lbl_805E76E8);
        field_0x8A.Set(lbl_805E76E9);
        field_0x8C.Set(lbl_805E76EA);
        field_0x8E.Set(lbl_805E76EB);
        field_0x90.Set(lbl_805E76EC);
        field_0x94 = 0.0f;
    }

    /* Declared ahead of the overrides: derived classes' thunks follow this order (layout is
     * unchanged, as both are new slots). */
    virtual int vfStat_0xB8() = 0;
    virtual ~EnemyStatusCommon() {}
    virtual void vfStat_0x08();                  /* 0x801A9138 */
    virtual void vfStat_0x0C();                  /* 0x801A8CE8 */
    virtual void vfStat_0x10();                  /* 0x801A86A8 */
    virtual void vfStat_0x14();                  /* 0x801A8440 */
    virtual void vfStat_0x18();                  /* 0x801A83D8 */
    virtual void vfStat_0x1C();                  /* 0x801A833C */
    virtual int vfStat_0x20(int motion, bool restart); /* 0x8017B61C */
    virtual int vfStat_0x24();                   /* 0x8017B6F8 */
    virtual bool vfStat_0x2C();                  /* 0x8017B700 */
    virtual bool vfStat_0x28();                  /* 0x8017B708 */
    virtual void vfStat_0x30(bool value);        /* 0x8017B710 */
    virtual void vfStat_0x3C(bool value);        /* 0x8017B718 */
    virtual bool vfStat_0x34();                  /* 0x8017B720 */
    virtual void vfStat_0x38();                  /* 0x801A7FA0 */
    virtual bool vfStat_0x40();                  /* 0x8017B728 */
    virtual bool vfStat_0x44();                  /* 0x801A8494 */
    virtual bool vfStat_0x48();                  /* 0x8017B730 */
    virtual bool vfStat_0x4C();                  /* 0x8017B738 */
    virtual bool vfStat_0x50();                  /* 0x8017B740 */
    virtual bool vfStat_0x54();                  /* 0x8017B748 */
    virtual bool vfStat_0x58();                  /* 0x8017B750 */
    virtual bool vfStat_0x5C();                  /* 0x8017B758 */
    virtual bool vfStat_0x60();                  /* 0x8017B760 */
    virtual bool vfStat_0x64();                  /* 0x8017B768 */
    virtual bool vfStat_0x68();                  /* 0x8017B770 */
    virtual bool vfStat_0x6C();                  /* 0x8017B778 */
    virtual void vfStat_0x70(SonicteamUSA::System::CharaColli::AttackCallbackParam &param); /* 0x801A8020 */
    virtual int vfStat_0x74();                   /* 0x8017B780 */
    virtual bool vfStat_0x78();                  /* 0x801A8388 */
    virtual void vfStat_0x80(bool value);        /* 0x8017B788 */
    virtual void vfStat_0x84(int kind);          /* 0x8017B790 */
    virtual void vfStat_0x88(u32 value);         /* 0x8017B7B4 */
    virtual void vfStat_0x7C(bool arg0, bool arg1); /* 0x8017B7D8 */
    virtual void vfStat_0x8C(bool value);        /* 0x8017B7FC */
#if defined(ENEMY_STAT_0x90_ARG) || defined(ENEMY_STAT_SLOT_ARGS)
    virtual void vfStat_0x90(bool value);        /* 0x801A7F64 */
#else
    virtual void vfStat_0x90();                  /* 0x801A7F64 */
#endif
    virtual void vfStat_0x94(int flags);         /* 0x801A8608 */
#ifdef ENEMY_STAT_SLOT_ARGS
    virtual void vfStat_0x98(const boost::shared_ptr<CharInfo> &attacker); /* 0x801A85B4 */
    virtual void vfStat_0x9C(const boost::shared_ptr<CharInfo> &attacker); /* 0x801A8504 */
#else
    virtual void vfStat_0x98();                  /* 0x801A85B4 */
    virtual void vfStat_0x9C();                  /* 0x801A8504 */
#endif
    virtual void vfStat_0xA0();                  /* 0x801A84DC */
    virtual void vfStat_0xA4();                  /* 0x801A84D8 */
    virtual EnemyFlags *vfStat_0xA8();           /* 0x8017B83C */
    virtual float vfStat_0xAC();                 /* 0x8017B844 */
    virtual float vfStat_0xB0();                 /* 0x8017B84C */
    virtual void vfStat_0xB4(float dt);          /* 0x8017B854 */
    virtual void vfStat_0xC0();                  /* 0x801A84D4 */
    virtual int vfStat_0xC4();                   /* 0x8017B8CC */
    virtual bool vfStat_0xC8();                  /* 0x801A84AC */
    virtual float vfStat_0xCC();                 /* 0x8017B8D4 */
    virtual void vfStat_0xD0();                  /* 0x801A8258 */

    /* 0x08 */ UnkEnemyStatus_801AA430 field_0x8;
    /* 0x20 */ EnemyMotionSequenceModule m_motion;
    /* 0x64 */ boost::shared_ptr<EnemyTarget> m_target;
    /* 0x6C */ u8 field_0x6C;
    /* 0x6D */ u8 field_0x6D;
    /* 0x70 */ s32 field_0x70;
    /* 0x74 */ u8 field_0x74;
    /* 0x75 */ u8 field_0x75;
    /* 0x76 */ u8 field_0x76;
    /* 0x77 */ u8 field_0x77;
    /* 0x78 */ float field_0x78;
    /* 0x7C */ float field_0x7C;
    /* 0x80 */ EnemyFlags m_flags;
    /* 0x84 */ s32 field_0x84;
    /* 0x88 */ EnemyStatusFlag field_0x88;
    /* 0x8A */ EnemyStatusFlag field_0x8A;
    /* 0x8C */ EnemyStatusFlag field_0x8C;
    /* 0x8E */ EnemyStatusFlag field_0x8E;
    /* 0x90 */ EnemyStatusFlag field_0x90;
    /* 0x94 */ float field_0x94;
};

#ifdef ENEMY_DISP_COMMON_UNIT
/* The health gauge (Game/GiHPGaugeEnemy.h) and a sound (as Game/BkLarva.h names it). */
class GiHPGaugeEnemy;
class UnkSound_802B4254;
#endif

/* EnemyDispCommon (class name from RTTI; vtable 0x8053A928; constructor 0x801A1CB4). */
class EnemyDispCommon : public virtual EnemyReferer, public EnemyDispFunc {
  public:
    EnemyDispCommon();

    /* New slots declared ahead of the overrides, as in EnemyStatusCommon. */
    virtual void vfDisp_0x5C();                       /* 0x801A128C */
    virtual ~EnemyDispCommon();                   /* 0x801A1C40 */
    virtual void vfDisp_0x08();                       /* 0x801A170C */
    virtual void vfDisp_0x0C();                       /* 0x801A0FB4 */
    virtual void vfDisp_0x10(float dt);               /* 0x801A0CD4 */
    virtual int vfDisp_0x14();                        /* 0x8017B5A8 */
    virtual int vfDisp_0x18();                        /* 0x801A0C8C */
    virtual void *vfDisp_0x1C();                      /* 0x801A0CB0 */
    virtual boost::shared_ptr<EnemyRender> vfDisp_0x20(); /* 0x8017B5B0 */
    virtual Mtx34f *GetPartMatrix(int index);     /* 0x801A0C58 */
    virtual Fn8040CopyVector3 vfDisp_0x28();          /* 0x801A0BBC */
    virtual Fn8040CopyVector3 vfDisp_0x2C();          /* 0x801A0B48 */
    virtual void vfDisp_0x30(float value);            /* 0x801A0BB4 */
    virtual float vfDisp_0x34();                      /* 0x801A0BAC */
    virtual void vfDisp_0x38(const Vec3f *position, const Vec3f *rotation); /* 0x801A1BF4 */
    virtual void vfDisp_0x3C(bool moving);            /* 0x801A0AFC */
    virtual void vfDisp_0x40(EnemyMotionEventArg &arg); /* 0x801A0AB0 */
    virtual bool vfDisp_0x44(int *command);           /* 0x801A0AA8 */
    virtual void vfDisp_0x48();                       /* 0x801A07A8 */
    virtual void vfDisp_0x4C();                       /* 0x801A05EC */
    virtual void vfDisp_0x50();                       /* 0x801A056C */
    virtual void vfDisp_0x54();                       /* 0x801A04D4 */
    virtual void vfDisp_0x58(bool value);             /* 0x8017B614 */
    /* Creates the model (behaviour inferred from BkSoldierBase). */
    virtual void vfDisp_0x64() = 0;

    /* 0x14 */ boost::shared_ptr<EnemyRender> m_render;
#ifdef ENEMY_DISP_COMMON_UNIT
    /* The rest as Game/EnemyDispCommon.cpp, which holds the class's code, sees it (names
     * inferred; the layout is the same). */
    /* 0x1C */ float m_shakeTime;  /* the model shakes while it is above 0 */
    /* 0x20 */ u8 m_hidden;        /* the model is hidden (dead, or blinking) */
    /* 0x21 */ u8 m_moving;        /* set by vfDisp_0x3C */
    /* 0x22 */ u8 m_soundOn;       /* m_sound plays (and follows the gauge) */
    /* 0x24 */ boost::shared_ptr<GiHPGaugeEnemy> m_gauge;
    /* 0x2C */ boost::shared_ptr<UnkSound_802B4254> m_sound;
    /* 0x34 */ float m_shadowSize;
    /* 0x38 */ float m_shadowRatio;
    /* 0x3C */ float m_speedScale; /* set each frame by vfDisp_0x30, back to 1 after the update */
#else
    /* 0x1C */ u8 field_0x1C[0x40 - 0x1C];
#endif
};

/* EnemyMoveCommon (class name from RTTI; vtable 0x8053AA04; constructor 0x801A5D14). */
class EnemyMoveCommon : public virtual EnemyReferer, public EnemyMoveFunc {
  public:
    EnemyMoveCommon();

    virtual void vfMove_0x08();          /* 0x801A5CBC */
    virtual void vfMove_0x0C(float dt);  /* 0x801A5778 */
    virtual bool vfMove_0x10();          /* 0x8017B578 */
    virtual bool vfMove_0x14();          /* 0x8017B580 */
    virtual bool vfMove_0x18();          /* 0x801A5760 */
    virtual int vfMove_0x1C();           /* 0x801A5734 */
    virtual float vfMove_0x28();         /* 0x8017B588 */
    virtual int vfMove_0x2C();           /* 0x8017B590 */
    virtual void *vfMove_0x30();         /* 0x8017B598 */
#ifdef ENEMY_MOVE_COMMON_INLINE
    /* Inline in the original (weak copy 0x8017B5A0): units that define ENEMY_MOVE_COMMON_INLINE
     * emit it, and its constant, as retail does (see Game/BkWingLarge.cpp). It is declared after
     * 0x30, the order in which Game/EggPawn.cpp, where its first copy is, emits the slots. */
    virtual float vfMove_0x24() {
        return 0.0f;
    }
#else
    virtual float vfMove_0x24();         /* 0x8017B5A0 */
#endif
    virtual void vfMove_0x34();          /* 0x801A560C */

    /* 0x08 */ u8 field_0x8[0x64 - 0x8];
};

/* EnemyMoveWalker (class name from RTTI; vtable 0x80531764, emitted with Game/BkSoldier.cpp):
 * movement on the ground. Its constructor is inline (weak copy 0x8018F178). */
class EnemyMoveWalker : public EnemyMoveCommon, public virtual EnemyReferer {
  public:
    EnemyMoveWalker() {}

    virtual int vfMove_0x20() {
        return 0;
    }
};

/* TEnemySetBase (class name from RTTI; vtable 0x8053AB40; constructor 0x801A7400). */
class TEnemySetBase : public virtual EnemyReferer, public EnemySetFunc {
  public:
    TEnemySetBase();

    virtual bool vfSet_0x08();                                     /* 0x8017B538 */
    virtual bool vfSet_0x0C();                                     /* 0x8017B540 */
    virtual void vfSet_0x10();                                     /* 0x8017B548 */
    virtual void vfSet_0x14();                                     /* 0x801A6E84 */
    virtual void vfSet_0x18();                                     /* 0x801A6E44 */
    virtual void vfSet_0x1C(UnkPlacement_800C9CD0 *placement); /* 0x801A73A0 */
    virtual void vfSet_0x20(UnkPlacement_800C9CD0 *placement); /* 0x801A7380 */
    virtual void vfSet_0x24();                                     /* 0x801A6EC4 */
    virtual bool vfSet_0x28();                                     /* 0x8017B554 */
    virtual ~TEnemySetBase();                                   /* 0x801A73A4 */

    /* 0x08 */ u8 field_0x8[0x18 - 0x8];
};

/* TEnemySetTask (class name from RTTI; vtable 0x8053AB1C; constructor 0x801A6D14): the Task that
 * runs an enemy. parent is the task it hangs below. It is 8-byte aligned: BkWorm places it at
 * 0x368, after an AI of 0x80 bytes at 0x2E4. The alignment probably comes from Task's 8-byte
 * value at 0x20 (the Task constructor stores a pair of words returned by fn_8004F098 there),
 * but Game/Task.h is left as it is; the attribute gives the same layout. EggPawn
 * agrees (its TEnemySetTask starts 4 bytes after its AI). */
class TEnemySetTask : public virtual EnemyReferer, public Task {
  public:
    TEnemySetTask(Task *parent, UnkPlacement_800C9CD0 *placement);
    virtual ~TEnemySetTask();     /* 0x801A6C78 */
    virtual void Tick(float dt);  /* 0x801A6AFC */

    /* 0x2C */ s32 field_0x2C;
} __attribute__((aligned(8)));

/* ------------------------------------------------------------------------------------------- */
/* The bodies. */

extern "C" {
void fn_801A6504(EnemyParamCommon *param, short flags);
}

/* TEnemyCommonBase (class name from RTTI; vtable 0x80531804): the enemy with its parameter and
 * status parts. Constructor and destructor are inline (weak copies 0x8018F1D8, 0x8018E8D4). */
class TEnemyCommonBase : public EnemyRefBase, public EnemyParamCommon, public EnemyStatusCommon {
  public:
    TEnemyCommonBase(int type) : EnemyRefBase(type) {
        m_enemy = this;
        m_para = this;
        m_status = this;
    }
    virtual ~TEnemyCommonBase() {}
};

/* EnemyWalkerCommon (class name from RTTI; vtable 0x80531964): an enemy that walks. Constructor
 * and destructor are inline (weak copies 0x8018EE90, 0x8018E82C). */
class EnemyWalkerCommon : public TEnemyCommonBase,
                          public EnemyDispCommon,
                          public EnemyMoveWalker,
                          public TEnemySetBase {
  public:
    EnemyWalkerCommon(int type) : TEnemyCommonBase(type) {
        m_enemy = this;
        m_disp = this;
        m_move = this;
        m_set = this;
        m_collision = boost::shared_ptr<CollisionEffect>(new CollisionEffect());
    }
    virtual ~EnemyWalkerCommon() {}
    virtual void vfDisp_0x10(float dt) {
        EnemyDispCommon::vfDisp_0x10(dt);
        if (!vfStat_0x40() && m_collision.get() != 0) {
            fn_80344528(m_collision.get(), dt);
        }
    }

    /* 0x268 */ boost::shared_ptr<CollisionEffect> m_collision;
};

/* SoldierCommonBase (class name from RTTI; vtable 0x80531B9C): the soldiers' body (BkSoldier,
 * GunSoldier). Constructor and destructor are inline (weak copies 0x8018ED74, 0x8018E7AC). */
class SoldierCommonBase : public EnemyWalkerCommon {
  public:
    /* One of the four patrol points (names inferred). */
    struct Point {
        Point() {
            fn_8040CE60(&m_position);
            field_0xC = 0.0f;
            field_0x10 = 0.0f;
            field_0x14 = 0;
        }

        /* 0x00 */ Vec3f m_position;
        /* 0x0C */ float field_0xC;
        /* 0x10 */ float field_0x10;
        /* 0x14 */ s32 field_0x14;
    };

    SoldierCommonBase(int type) : EnemyWalkerCommon(type) {
        field_0x270 = 0;
        m_appearType = 0;
        m_weaponType = 0;
        field_0x27C = 0;
        field_0x280 = 0;
        field_0x284 = 0.0f;
    }
    virtual ~SoldierCommonBase() {}
    /* Called by the soldier AI states (meanings unknown). */
    virtual int vf_0x238(int index) = 0;
    virtual int vf_0x23C() = 0;

    /* The weapon the soldier carries, 0 for none (an index into the WeaponType list). */
    int GetWeaponType() {
        return m_weaponType;
    }
    /* The point the soldier walks to or waits at (0 to 3), and the time it waits at point index
     * (weak copies first at 0x80188E04 and 0x80188DF4; names inferred). */
    int GetPointIndex() {
        return field_0x280;
    }
    float GetWaitTime(int index) {
        return m_points[index].field_0xC;
    }

    /* 0x270 */ u8 field_0x270;
    /* 0x274 */ s32 m_appearType;
    /* 0x278 */ s32 m_weaponType;
    /* 0x27C */ s32 field_0x27C;
    /* 0x280 */ s32 field_0x280;
    /* 0x284 */ float field_0x284;
    /* 0x288 */ Point m_points[4];
};

/* EnemyTemplate<Base, AI> (class name from RTTI): a complete enemy, its body, AI and task. The
 * functions of each instance are emitted with the enemy's unit. */
template <class Base, class AI>
class EnemyTemplate : public Base, public AI, public TEnemySetTask {
  public:
    EnemyTemplate(Task *parent, UnkPlacement_800C9CD0 *placement)
        : TEnemySetTask(parent, placement) {
        this->m_enemy = this;
        this->m_ctrl = this;
    }
    virtual ~EnemyTemplate() {}
    virtual int vfSet_0x2C() {
        return this->TEnemySetTask::field_0x2C;
    }
};

#endif
