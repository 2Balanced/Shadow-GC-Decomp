/* EnemyBaseAI (class name from RTTI; Game/EnemyAI.h): the base of the enemies' AIs. It runs a
 * state machine of TEnemyAIState objects, keeps track of the player (its offset, distance and the
 * angles to it) and turns the enemy toward a direction, receives the commands sent to the enemy's
 * character info (vacuum, recover, generate, ...) and has its own states: none, idle, waiting at
 * home, guard, caution, going home, waiting for a warp or capture to end, waiting for the damage
 * and falling motions to end, dead and recovering. Each vfAI slot changes to one of the states;
 * the AIs of the enemies override them. File name inferred from the class. Names marked "(name
 * inferred)" are not original. Definitions are in reverse address order (deferred inlining emits
 * functions last-defined first). */
/* See EnemyBaseAI (Game/EnemyAI.h). */
#define ENEMY_BASE_AI_STATES
/* The layouts of the AI's state machine, target and turn (Game/EnemyAI.h). */
#define ENEMY_BASE_AI_UNIT
/* EnemyStatFunc's slots 0x90, 0x98 and 0x9C with their arguments (Game/Enemy.h). */
#define ENEMY_STAT_SLOT_ARGS

#include "Game/CharCommand.h"
#include "Game/CharCommandUnit.h"
#include "Game/CharInfo.h"
#include "Game/Enemy.h"
#include "Game/EnemyAI.h"
#include "Game/EnemyAIState.h"
#include "Game/EnemyCharInfo.h"
#include "Game/EnemyRender.h"
#include "Game/MathConstants.h"
#include "Game/MathTypes.h"
#include "Game/Player.h"
#include "Game/Random.h"
#include "boost/bind.hpp"
#include "boost/function.hpp"
#include "boost/shared_ptr.hpp"
#include "boost/weak_ptr.hpp"

/* MSL's std::fabs for floats, which the game calls out of line (as in Enemy.cpp). */
namespace std {
float fabs(float x);
}

/* The degrees-to-radians conversion (see BAGunShip.cpp). */
float DegToRad(float degrees);

/* Turns current toward target by at most step (Game/AngleUtil.h; the linked copy is UpDownReel's,
 * declared here as in Enemy.cpp). */
float TurnAngleToward(float target, float current, float step);

namespace Effect {
namespace System {
class EffectElement;
}
}

/* The model and the motion of an enemy's EnemyRender (see Game/EnemyRender.cpp). */
class UnkModel_801A3E9C;
class UnkMotion_801A4D28;
struct RwFrame;
/* The camera of a player (see Game/CameraScreenSet.cpp). */
class UnkCameraManager_80009548;
class UnkCamera_80009370;

extern "C" {
/* The enemies' shared helpers (Game/Enemy.cpp). */
bool fn_8019AF04(Enemy *enemy);
void fn_8019B0F8(Enemy *enemy, const Vec3f *rotation);
void fn_8019B0B0(Enemy *enemy, const Vec3f *direction);
float fn_8019B184(Enemy *enemy, const Vec3f *direction);
float fn_8019B298(Enemy *enemy, const Vec3f *point);
void fn_8019AAD4(Enemy *enemy, float value);
void fn_8019AAA4(Enemy *enemy, int bit, float value);
void fn_8019AA40(Enemy *enemy, u16 sound, bool positional);
/* Angles: wrapped into [-pi, pi], the angle of (x, z) and a direction's pitch and yaw (see
 * Game/Enemy.cpp). */
float fn_8040E99C(float angle);
float fn_80016260(float x, float z);
Vec2f fn_8040F760(const Vec3f *direction);
/* Vector helpers (see Game/Enemy.cpp): -v, v turned about y by angle, v normalized, v scaled,
 * v += value. */
Vec3f fn_80016284(const Vec3f *v);
Vec3f fn_8040D0AC(const Vec3f *v, float angle);
float fn_8040CE74(Vec3f *v);
void fn_8040CE38(Vec3f *v, float scale);
void fn_8040CFF8(Vec3f *v, const Vec3f *value);
/* The player of number player (see Game/BkWorm.cpp). */
boost::weak_ptr<CharInfo> fn_800A8B74(UnkCharaList_80074194 *list, int player);
/* The cameras and the screen position of a point in a camera's view (x and y in [-1, 1] and z not
 * above 0 while it is on screen; behaviour inferred). */
UnkCameraManager_80009548 *fn_80009548(void);
UnkCamera_80009370 *fn_80009370(UnkCameraManager_80009548 *manager, int player);
Vec3f fn_8000F57C(UnkCamera_80009370 *camera, const Vec3f *point, int arg2);
/* The flag word of Game/Enemy.h's EnemyFlags: clear and test a bit. */
void fn_8006127C(u32 *flags, int bit);
bool fn_80014E34(u32 *flags, int bit);
/* An effect at a matrix (see Game/BkLarva.cpp). */
boost::weak_ptr<Effect::System::EffectElement> fn_801A1ED8(const Mtx34f *matrix, int id);
/* The placement record's flags: test and set (see Game/BkLarva.cpp). */
bool fn_800C9E80(void *placement, u32 mask);
void fn_800C9EB0(void *placement, u32 mask);
/* Whether the attacker was the player, the vacuum command's owner and the vacuum taking the
 * enemy's model (see Game/BkLarva.cpp). */
bool fn_8021C958(const boost::shared_ptr<CharInfo> &attacker);
boost::weak_ptr<CharInfo> fn_801DADFC(VacuumCommand *command);
void fn_8028CFC4(VacuumCommand *command, const boost::shared_ptr<EnemyRender> &render,
                 const Vec3f *position, const Vec3f *rotation, int arg4);
/* The enemy's render: its model and motion (see Game/EnemyRender.cpp). */
void fn_802CD228(EnemyRender *render);
UnkModel_801A3E9C *fn_802CD5AC(EnemyRender *render);
UnkMotion_801A4D28 *fn_802CD588(EnemyRender *render);
void fn_802CD5F0(EnemyRender *render);
/* The motion id is a u16 here: this unit passes fn_801A61E4's result on unchanged. */
void fn_802CD68C(EnemyRender *render, u16 id, float blend);

void fn_801A3A44(UnkModel_801A3E9C *model);
RwFrame *fn_801A3750(UnkModel_801A3E9C *model);
void fn_801A4548(UnkMotion_801A4D28 *motion, int arg1);
/* The motion an enemy of type type plays while the vacuum takes it, 0 for none (behaviour
 * inferred). */
u16 fn_801A61E4(int type);
/* RwFrameUpdateObjects (see Game/BkLarva.cpp). */
RwFrame *fn_8048722C(RwFrame *frame);
}

/* The player's number, for fn_800A8B74 (see Game/BkWorm.cpp). */
extern s32 lbl_805EE588;

/* fn_8040CD74 called as returning a - b by value (see Game/EnemyRender.cpp). */
typedef Vec3f (*VectorSubtractFn)(const Vec3f *a, const Vec3f *b);

/* fn_80014E1C (EnemyFlags::Set) called for the flag word it returns (it returns its argument). */
typedef u32 *(*FlagSetFn)(u32 *flags, int bit);

/* The vector type's out-of-line copy, assignment and addition, called with a temporary the way the
 * original's operators are (names inferred; see Game/Enemy.cpp). */
inline void ConstructVector(Vec3f *vector, const Vec3f &value) {
    fn_800091EC(vector, &value);
}

inline void AssignVector(Vec3f &vector, const Vec3f &value) {
    fn_800091D0(&vector, &value);
}

inline void AddVector(Vec3f &vector, const Vec3f &value) {
    fn_8040CFF8(&vector, &value);
}

/* degrees in radians (name inferred; as in Game/Enemy.cpp). */
inline float Radians(float degrees) {
    return degrees * (lbl_805FAD38 / 180.0f);
}

/* The command of type 0x17 (class name unknown): it sets field_0xF0 of the enemy and a bit of
 * its flags at field_0xF4 (see fn_8019AAA4; names inferred). */
class UnkCommand_0x17 : public CharCommand {
  public:
    /* 0x14 */ u8 m_value;
    /* 0x18 */ u8 field_0x18[0x1C - 0x18];
    /* 0x1C */ s32 m_bit;
};

/* ------------------------------------------------------------------------------------------- */
/* The states' constructors and owners. */

EnemyAIState_EnemyBaseAI_None::EnemyAIState_EnemyBaseAI_None() : m_ai(0) {}

void EnemyAIState_EnemyBaseAI_None::SetOwner(EnemyBaseAI *ai) {
    EnemyBaseAI *owner = dynamic_cast<EnemyBaseAI *>(ai);
    if (owner != 0) {
        m_ai = owner;
    }
}

EnemyAIState_EnemyBaseAI_Idle::EnemyAIState_EnemyBaseAI_Idle() : m_ai(0) {}

void EnemyAIState_EnemyBaseAI_Idle::SetOwner(EnemyBaseAI *ai) {
    EnemyBaseAI *owner = dynamic_cast<EnemyBaseAI *>(ai);
    if (owner != 0) {
        m_ai = owner;
    }
}

EnemyAIState_EnemyBaseAI_CaptureWait::EnemyAIState_EnemyBaseAI_CaptureWait() : m_ai(0) {}

void EnemyAIState_EnemyBaseAI_CaptureWait::SetOwner(EnemyBaseAI *ai) {
    EnemyBaseAI *owner = dynamic_cast<EnemyBaseAI *>(ai);
    if (owner != 0) {
        m_ai = owner;
    }
}

EnemyAIState_EnemyBaseAI_WarpWait::EnemyAIState_EnemyBaseAI_WarpWait() : m_ai(0) {}

void EnemyAIState_EnemyBaseAI_WarpWait::SetOwner(EnemyBaseAI *ai) {
    EnemyBaseAI *owner = dynamic_cast<EnemyBaseAI *>(ai);
    if (owner != 0) {
        m_ai = owner;
    }
}

EnemyAIState_EnemyBaseAI_Wait::EnemyAIState_EnemyBaseAI_Wait() : m_ai(0) {}

void EnemyAIState_EnemyBaseAI_Wait::SetOwner(EnemyBaseAI *ai) {
    EnemyBaseAI *owner = dynamic_cast<EnemyBaseAI *>(ai);
    if (owner != 0) {
        m_ai = owner;
    }
}

EnemyAIState_EnemyBaseAI_Guard::EnemyAIState_EnemyBaseAI_Guard() : m_ai(0) {}

void EnemyAIState_EnemyBaseAI_Guard::SetOwner(EnemyBaseAI *ai) {
    EnemyBaseAI *owner = dynamic_cast<EnemyBaseAI *>(ai);
    if (owner != 0) {
        m_ai = owner;
    }
}

EnemyAIState_EnemyBaseAI_Caution::EnemyAIState_EnemyBaseAI_Caution() : m_ai(0) {}

void EnemyAIState_EnemyBaseAI_Caution::SetOwner(EnemyBaseAI *ai) {
    EnemyBaseAI *owner = dynamic_cast<EnemyBaseAI *>(ai);
    if (owner != 0) {
        m_ai = owner;
    }
}

EnemyAIState_EnemyBaseAI_GoHome::EnemyAIState_EnemyBaseAI_GoHome() : m_ai(0) {}

void EnemyAIState_EnemyBaseAI_GoHome::SetOwner(EnemyBaseAI *ai) {
    EnemyBaseAI *owner = dynamic_cast<EnemyBaseAI *>(ai);
    if (owner != 0) {
        m_ai = owner;
    }
}

EnemyAIState_EnemyBaseAI_WaitFinishDamaged::EnemyAIState_EnemyBaseAI_WaitFinishDamaged() : m_ai(0) {}

void EnemyAIState_EnemyBaseAI_WaitFinishDamaged::SetOwner(EnemyBaseAI *ai) {
    EnemyBaseAI *owner = dynamic_cast<EnemyBaseAI *>(ai);
    if (owner != 0) {
        m_ai = owner;
    }
}

EnemyAIState_EnemyBaseAI_WaitFinishFalling::EnemyAIState_EnemyBaseAI_WaitFinishFalling() : m_ai(0) {}

void EnemyAIState_EnemyBaseAI_WaitFinishFalling::SetOwner(EnemyBaseAI *ai) {
    EnemyBaseAI *owner = dynamic_cast<EnemyBaseAI *>(ai);
    if (owner != 0) {
        m_ai = owner;
    }
}

EnemyAIState_EnemyBaseAI_Dead::EnemyAIState_EnemyBaseAI_Dead() : m_ai(0) {}

void EnemyAIState_EnemyBaseAI_Dead::SetOwner(EnemyBaseAI *ai) {
    EnemyBaseAI *owner = dynamic_cast<EnemyBaseAI *>(ai);
    if (owner != 0) {
        m_ai = owner;
    }
}

EnemyAIState_EnemyBaseAI_Recover::EnemyAIState_EnemyBaseAI_Recover() : m_ai(0) {}

void EnemyAIState_EnemyBaseAI_Recover::SetOwner(EnemyBaseAI *ai) {
    EnemyBaseAI *owner = dynamic_cast<EnemyBaseAI *>(ai);
    if (owner != 0) {
        m_ai = owner;
    }
}

/* ------------------------------------------------------------------------------------------- */
/* The state machine (Game/EnemyAI.h). The other units call these under their addresses. Retail
 * inlines locking, unlocking and changing the state into this unit's callers and keeps them out of
 * line as well, which GC/2.7 does for functions it may inline on its own. */

#pragma auto_inline on
/* Keeps the state from changing (name inferred). */
extern "C" void fn_8019F9FC(UnkAIStateMachine_8019F944 *machine) {
    machine->m_locked = true;
}

/* Lets the state change again (name inferred). */
extern "C" void fn_8019F9F0(UnkAIStateMachine_8019F944 *machine) {
    machine->m_locked = false;
}

/* Leaves the current state and enters state (name inferred). */
extern "C" void fn_8019F944(UnkAIStateMachine_8019F944 *machine, TEnemyAIState *state) {
    if (!machine->m_locked) {
        if (machine->m_current != 0) {
            machine->m_current->SetOwner(machine->m_owner);
            machine->m_current->Exit();
        }
        if (state != 0) {
            machine->m_previous = machine->m_current;
            machine->m_current = state;
            machine->m_current->SetOwner(machine->m_owner);
            machine->m_current->Enter();
        }
    }
}
#pragma auto_inline reset

/* Goes back to the previous state (name inferred). */
extern "C" void fn_8019F898(UnkAIStateMachine_8019F944 *machine) {
    fn_8019F944(machine, machine->m_previous);
}

/* Whether state is the current state (name inferred). */
extern "C" bool fn_8019F86C(UnkAIStateMachine_8019F944 *machine, TEnemyAIState *state) {
    if (machine->m_current != 0 && state != 0 && machine->m_current == state) {
        return true;
    }
    return false;
}

/* ------------------------------------------------------------------------------------------- */
/* The tracked player (Game/EnemyAI.h). */

UnkAITarget_8019F65C::UnkAITarget_8019F65C() {
    m_enemy = 0;
    m_valid = false;
}

/* Brings the target up to date (name inferred). */
extern "C" void fn_8019F74C(UnkAITarget_8019F65C *target) {
    target->m_valid = true;
    AssignVector(target->m_offset,
                 ((VectorSubtractFn)fn_8040CD74)(reinterpret_cast<Vec3f *>(target->m_ai->vfCtrl_0x10()),
                                                 target->m_enemy->GetPosition()));
    Vec3f ground;
    ConstructVector(&ground, target->m_offset);
    ground.y = 0.0f;
    target->m_distance = fn_8040CF50(&ground);
    if (target->m_distance > 0.1f) {
        target->m_angle = fn_8019B184(target->m_enemy, &target->m_offset);
        AssignVector(ground, fn_80016284(&target->m_offset));
        target->m_backAngle = fn_8019B184(target->m_enemy, &ground);
        target->m_yaw = fn_80016260(-target->m_offset.x, -target->m_offset.z);
    } else {
        target->m_angle = 0.0f;
        target->m_backAngle = lbl_805FAD38;
        target->m_yaw = target->m_enemy->GetRotation()->y;
    }
}

/* The distance to the player on the ground plane (name inferred). */
extern "C" float fn_8019F710(UnkAITarget_8019F65C *target) {
    if (!target->m_valid) {
        fn_8019F74C(target);
    }
    return target->m_distance;
}

/* The yaw of the player as seen from the enemy (name inferred). */
extern "C" float fn_8019F6D4(UnkAITarget_8019F65C *target) {
    if (!target->m_valid) {
        fn_8019F74C(target);
    }
    return target->m_yaw;
}

/* How far the enemy has to turn to have its back to the player (name inferred). */
extern "C" float fn_8019F698(UnkAITarget_8019F65C *target) {
    if (!target->m_valid) {
        fn_8019F74C(target);
    }
    return target->m_backAngle;
}

/* The offset from the enemy to the player (name inferred). */
extern "C" Vec3f *fn_8019F65C(UnkAITarget_8019F65C *target) {
    if (!target->m_valid) {
        fn_8019F74C(target);
    }
    return &target->m_offset;
}

/* ------------------------------------------------------------------------------------------- */
/* Turning the enemy (Game/EnemyAI.h). */

/* Turns the enemy a step to face away from direction, about y only; true once it does (name
 * inferred). */
extern "C" bool fn_8019F53C(UnkAITurn_8019F53C *turn, const Vec3f *direction) {
    float yaw = fn_80016260(-direction->x, -direction->z);
    float step = turn->m_speed * turn->m_ai->GetDeltaTime();
    float half = std::fabs(0.5f * fn_8040E99C(turn->m_enemy->GetRotation()->y - yaw));
    if (step < half) {
        half = step;
    }
    step = Radians(1.0f);
    if (half > step) {
        step = half;
    }
    bool faced = false;
    turn->m_enemy->GetRotation()->y = TurnAngleToward(yaw, turn->m_enemy->GetRotation()->y, step);
    if (std::fabs(yaw - turn->m_enemy->GetRotation()->y) < Radians(1.0f)) {
        faced = true;
    }
    return faced;
}

/* Turns the enemy a step toward direction, about x and y, and levels its roll; true once it
 * faces it (name inferred). */
extern "C" bool fn_8019F2E0(UnkAITurn_8019F53C *turn, const Vec3f *direction) {
    Vec2f angles = fn_8040F760(direction);
    if (angles.x > Radians(45.0f)) {
        angles.x = Radians(45.0f);
    } else if (angles.x < Radians(-45.0f)) {
        angles.x = Radians(-45.0f);
    }
    float step = turn->m_speed * turn->m_ai->GetDeltaTime();
    float pitchStep = std::fabs(0.5f * fn_8040E99C(turn->m_enemy->GetRotation()->x - angles.x));
    float yawStep = std::fabs(0.5f * fn_8040E99C(turn->m_enemy->GetRotation()->y - angles.y));
    if (step < pitchStep) {
        pitchStep = step;
    }
    float minimum = Radians(1.0f);
    pitchStep = pitchStep > minimum ? pitchStep : minimum;
    yawStep = step < yawStep ? step : yawStep;
    minimum = yawStep > minimum ? yawStep : minimum;

    turn->m_enemy->GetRotation()->y = TurnAngleToward(angles.y, turn->m_enemy->GetRotation()->y, minimum);
    turn->m_enemy->GetRotation()->x = TurnAngleToward(angles.x, turn->m_enemy->GetRotation()->x, pitchStep);
    turn->m_enemy->GetRotation()->z = TurnAngleToward(0.0f, turn->m_enemy->GetRotation()->z, step);
    bool pitchDone = false;
    bool yawDone = false;
    if (std::fabs(angles.y - turn->m_enemy->GetRotation()->y) < Radians(1.0f)) {
        yawDone = true;
    }
    if (std::fabs(angles.x - turn->m_enemy->GetRotation()->x) < Radians(1.0f)) {
        pitchDone = true;
    }
    return pitchDone & yawDone;
}

UnkAITurn_8019F53C::UnkAITurn_8019F53C() {
    m_enemy = 0;
}

/* ------------------------------------------------------------------------------------------- */
/* Setting up the parts (names inferred). */

/* The machine starts in the none state. */
extern "C" void fn_8019F16C(UnkAIStateMachine_8019F944 *machine, EnemyBaseAI *owner) {
    machine->m_owner = owner;
    fn_8019F944(machine, EnemyBaseAI::GetNoneState());
    machine->m_locked = false;
}

extern "C" void fn_8019F160(UnkAITarget_8019F65C *target, Enemy *enemy, EnemyBaseAI *ai) {
    target->m_enemy = enemy;
    target->m_ai = ai;
}

/* The turn speed is the enemy's parameter 0x34. */
extern "C" void fn_8019F114(UnkAITurn_8019F53C *turn, Enemy *enemy, EnemyBaseAI *ai) {
    turn->m_enemy = enemy;
    turn->m_ai = ai;
    turn->m_speed = turn->m_enemy->m_para->vfPara_0x34();
}

/* ------------------------------------------------------------------------------------------- */
/* EnemyBaseAI. */

/* The command handler, slot 0x28 (vfAI_0x28 below), as the member function the commands' callback
 * binds: Game/EnemyAI.h declares the slot without its argument. */
typedef void (EnemyBaseAI::*CommandHandler)(CharCommand &command);

EnemyBaseAI::EnemyBaseAI() : field_0x6C(0.0f), field_0x70(0.0f) {
    fn_8019F16C(&m_stateMachine, this);
    fn_8019F160(&m_target, m_enemy, this);
    fn_8019F114(&m_turn, m_enemy, this);
    boost::shared_ptr<UnkEnemyInfo_8017D014> chara(fn_8017D014(m_enemy).lock());
    if (chara.get() != 0) {
        fn_8019FD64(reinterpret_cast<EnemyCharInfo *>(chara.get()),
                    boost::bind(reinterpret_cast<CommandHandler>(&EnemyBaseAI::vfAI_0x28), this, _1));
    }
}

void EnemyBaseAI::vfCtrl_0x08() {
    vfAI_0x38();
}

void EnemyBaseAI::vfAI_0x2C() {
    m_target.m_valid = false;
    field_0x6C -= field_0x20;
    if (m_enemy->m_status->vfStat_0x58()) {
        if (!m_enemy->m_status->vfStat_0x6C()) {
            fn_8019F9F0(&m_stateMachine);
            vfAI_0x58();
        }
    } else if (lbl_8057D6A0[0xC8] != 0) {
        fn_8019F944(&m_stateMachine, GetIdleState());
    } else if (fn_80014E34(&m_enemy->m_status->vfStat_0xA8()->m_bits, 5)) {
        fn_8006127C(&m_enemy->m_status->vfStat_0xA8()->m_bits, 5);
        vfAI_0x50();
    } else if (m_enemy->m_status->vfStat_0x50()) {
        fn_8019F9F0(&m_stateMachine);
        vfAI_0x54();
    } else if (!(u8)m_enemy->m_move->vfMove_0x20() && m_enemy->m_status->vfStat_0x54()) {

        fn_8019F944(&m_stateMachine, GetWaitFinishFallingState());
    }
}

void EnemyBaseAI::vfAI_0x30() {}

/* Slot 0x0C, the AI's update: it takes the frame time, which Game/EnemyAI.h does not declare, so
 * it is defined under its symbol (as slot 0x28 below). */
extern "C" void vfCtrl_0x0C__11EnemyBaseAIFv(EnemyBaseAI *ai, float dt) {
    ai->field_0x20 = dt;
    ai->vfAI_0x2C();
    if (ai->m_stateMachine.m_current != 0) {
        ai->m_stateMachine.m_current->SetOwner(ai->m_stateMachine.m_owner);
        ai->m_stateMachine.m_current->Update();
    }
    ai->vfAI_0x30();
}

/* Slot 0x28, the handler of the commands sent to the enemy (bound in the constructor). It takes
 * the command, which Game/EnemyAI.h does not declare (ENEMY_AI_MESSAGE_ARG gives it another
 * type), so it is defined under the slot's symbol. */
extern "C" void vfAI_0x28__11EnemyBaseAIFv(EnemyBaseAI *ai, CharCommand &command) {
    switch (command.TypeId()) {
    case 0x201:
        ai->vfAI_0x34();
        command.Flags() |= 2;
        break;
    case 0x202:
        ai->vfAI_0x38();
        command.Flags() |= 2;
        break;
    case 0x203:
        ai->vfAI_0x3C();
        command.Flags() |= 2;
        break;
    case 0x204:
        ai->vfAI_0x40();
        command.Flags() |= 2;
        break;
    case 0x205:
        ai->vfAI_0x44();
        command.Flags() |= 2;
        break;
    case 0x206:
        ai->vfAI_0x48();
        command.Flags() |= 2;
        break;
    case 0x207:
        ai->vfAI_0x4C();
        command.Flags() |= 2;
        break;
    case 0x208:
        ai->vfAI_0x50();
        command.Flags() |= 2;
        break;
    case 0x209:
        ai->vfAI_0x54();
        command.Flags() |= 2;
        break;
    case 0x20A:
        ai->vfAI_0x58();
        command.Flags() |= 2;
        break;
    case 0x20B:
        ai->vfAI_0x5C();
        command.Flags() |= 2;
        break;
    case 0x20C:
        ai->vfAI_0x60();
        command.Flags() |= 2;
        break;
    }
    switch (command.TypeId()) {
    case 0x16:
        command.Flags() |= 2;
        break;
    case 0x19:
        fn_8019F944(&ai->m_stateMachine, EnemyBaseAI::GetWarpWaitState());
        command.Flags() |= 2;
        break;
    case 0x1A:
        if (*reinterpret_cast<u32 *>(&lbl_8057D6A0[0x90]) - ai->m_enemy->field_0xF8 < 10 &&
            !fn_80014E34(&ai->m_enemy->m_status->vfStat_0xA8()->m_bits, 0xE) &&
            !ai->m_enemy->m_status->vfStat_0x58()) {
            ai->m_enemy->m_status->vfStat_0xA8()->Set(0xE);
            fn_8019F9F0(&ai->m_stateMachine);
            fn_8019F944(&ai->m_stateMachine, EnemyBaseAI::GetCaptureWaitState());
        }
        command.Flags() |= 2;
        break;
    case 0x1B:
        if (*((FlagSetFn)fn_80014E1C)(&ai->m_enemy->m_status->vfStat_0xA8()->m_bits, 0xE) != 0 &&
            !ai->m_enemy->m_status->vfStat_0x58()) {
            fn_8019F9F0(&ai->m_stateMachine);
            ai->vfAI_0x3C();
        }
        command.Flags() |= 2;
        break;
    case 0x10: {
        RecoverCommand &recover = static_cast<RecoverCommand &>(command);
        ai->m_enemy->m_status->vfStat_0xB4(recover.m_amount);
        Mtx34f *matrix = ai->m_enemy->m_disp->GetPartMatrix(ai->m_enemy->m_para->vfPara_0x3C());
        if (matrix != 0) {
            fn_801A1ED8(matrix, 0x2AA);
            fn_8019AA40(ai->m_enemy, 0xE02F, true);
        }
        if (!fn_80014E34(&ai->m_enemy->m_status->vfStat_0xA8()->m_bits, 0xD) &&
            !recover.m_source.expired()) {
            UnkPlacement_800C9CD0 *placement =
                reinterpret_cast<UnkPlacement_800C9CD0 *>(ai->m_enemy->m_set->vfSet_0x2C());
            if (placement != 0 && !fn_800C9E80(placement, 0x1000000)) {
                fn_800C9EB0(placement, 0x1000000);
                boost::shared_ptr<CharInfo> source(recover.m_source.lock());
                ai->m_enemy->m_status->vfStat_0x9C(source);
            }
        }
        ai->vfAI_0x60();
        command.Flags() |= 2;
        break;
    }
    case 0x17: {
        UnkCommand_0x17 &set = static_cast<UnkCommand_0x17 &>(command);
        fn_8019AAA4(ai->m_enemy, set.m_bit, set.m_value);
        command.Flags() |= 2;
        break;
    }
    case 0x14: {
        VacuumCommand *vacuum = static_cast<VacuumCommand *>(&command);
        boost::shared_ptr<EnemyRender> render(ai->m_enemy->m_disp->vfDisp_0x20());
        if (render.get() != 0) {
            fn_802CD228(render.get());
            fn_801A3A44(fn_802CD5AC(render.get()));
            if (fn_801A61E4(ai->m_enemy->m_type) != 0) {
                EnemyRender *model = render.get();
                fn_802CD68C(model, fn_801A61E4(ai->m_enemy->m_type), 30.0f);
                fn_801A4548(fn_802CD588(render.get()), 1);
                fn_802CD5F0(render.get());
                RwFrame *frame = fn_801A3750(fn_802CD5AC(render.get()));
                if (frame != 0) {
                    fn_8048722C(frame);
                }
            }
            Vec3f *rotation = ai->m_enemy->GetRotation();
            fn_8028CFC4(vacuum, render, ai->m_enemy->GetPosition(), rotation, 0);
            if (boost::shared_ptr<CharInfo> attacker = fn_801DADFC(vacuum).lock()) {
                ai->m_enemy->m_status->vfStat_0x98(attacker);
                if (fn_8021C958(attacker)) {
                    ai->m_enemy->m_status->vfStat_0xA8()->Set(0xC);
                }
            }
            ai->m_enemy->m_status->vfStat_0x94(1);
            ai->m_enemy->m_status->vfStat_0xA0();
            if (ai->m_enemy->m_type == 0) {
                ai->m_enemy->m_set->vfSet_0x18();
            } else {
                ai->m_enemy->m_set->vfSet_0x14();
            }
            ai->m_enemy->m_set->vfSet_0x10();
        }
        command.Flags() |= 2;
        break;
    }
    }
    if (!(command.Flags() & 2)) {
        command.Reject(boost::weak_ptr<CharInfo>(fn_8017D014(ai->m_enemy)));
    }
}

/* The position of the player the enemy keeps track of, or the enemy's own without one (name
 * inferred). */
void *EnemyBaseAI::vfCtrl_0x10() {
    boost::weak_ptr<CharInfo> target = fn_800A8A7C(fn_80074194(), m_enemy->GetPosition());
    if (!target.expired()) {
        boost::shared_ptr<CharInfo> chara = target.lock();
        return &chara->m_position;
    }
    return m_enemy->GetPosition();
}

/* The rotation of player 1, or the enemy's own without one (name inferred). */
void *EnemyBaseAI::vfCtrl_0x14() {
    boost::weak_ptr<CharInfo> player = fn_800A8B74(fn_80074194(), lbl_805EE588);
    boost::shared_ptr<CharInfo> chara = player.lock();
    if (chara.get() != 0) {
        return &chara->m_rotation;
    }
    return m_enemy->GetRotation();
}

/* The yaw of player 1, or the enemy's own without one (name inferred). */
float EnemyBaseAI::vfCtrl_0x18() {
    boost::weak_ptr<CharInfo> player = fn_800A8B74(fn_80074194(), lbl_805EE588);
    boost::shared_ptr<CharInfo> chara = player.lock();
    if (chara.get() != 0) {
        return chara->m_rotation.y;
    }
    return m_enemy->GetRotation()->y;
}

/* Slot 0x1C: a point to go to near the player the enemy keeps track of: beside the player when
 * the enemy is off screen (on the side it is on, 30 or 50 away), then at most range away in a
 * random direction (behaviour inferred). The result is copied through fn_800091EC, which Vec3f's
 * inline copy does not do; hence this definition by its linker name (not original; as with
 * EnemyCtrlFunc's default in Game/GunRobot.cpp). */
extern "C" Fn8040CopyVector3 vfCtrl_0x1C__11EnemyBaseAIFf(EnemyBaseAI *ai, float range) {
    boost::weak_ptr<CharInfo> target = fn_800A8A7C(fn_80074194(), ai->m_enemy->GetPosition());
    if (!target.expired()) {
        boost::shared_ptr<Player::PlayerBase> player =
            boost::dynamic_pointer_cast<Player::PlayerBase>(target.lock());
        if (player->field_0x54 != -1) {
            UnkCamera_80009370 *camera = fn_80009370(fn_80009548(), player->field_0x54);
            Vec3f enemyScreen;
            ConstructVector(&enemyScreen, fn_8000F57C(camera, ai->m_enemy->GetPosition(), 0));
            bool enemyOnScreen = true;
            if (enemyScreen.x < -1.0f || enemyScreen.x > 1.0f || enemyScreen.y < -1.0f ||
                enemyScreen.y > 1.0f || enemyScreen.z > 0.0f) {
                enemyOnScreen = false;
            }
            Vec3f playerScreen;
            ConstructVector(&playerScreen, fn_8000F57C(camera, &player.px->m_position, 0));
            bool playerOnScreen = true;
            if (playerScreen.x < -1.0f || playerScreen.x > 1.0f || playerScreen.y < -1.0f ||
                playerScreen.y > 1.0f || playerScreen.z > 0.0f) {
                playerOnScreen = false;
            }
            Vec3f point;
            ConstructVector(&point, player.px->m_position);
            if (!enemyOnScreen) {
                float distance;
                if (playerOnScreen) {
                    distance = 30.0f;
                } else {
                    distance = 50.0f;
                }
                Vec3f *playerPosition = &player->m_position;
                Vec3f side;
                ConstructVector(&side, ((VectorSubtractFn)fn_8040CD74)(ai->m_enemy->GetPosition(),
                                                                       playerPosition));
                side.y = 0.0f;
                fn_8040CE74(&side);
                fn_8040CE38(&side, distance);
                if (playerScreen.z > enemyScreen.z) {
                    if (playerScreen.x > enemyScreen.x) {
                        AddVector(point, fn_8040D0AC(&side, DegToRad(-90.0f)));
                    } else {
                        AddVector(point, fn_8040D0AC(&side, DegToRad(90.0f)));
                    }
                } else if (playerScreen.x > enemyScreen.x) {
                    AddVector(point, fn_8040D0AC(&side, DegToRad(90.0f)));
                } else {
                    AddVector(point, fn_8040D0AC(&side, DegToRad(-90.0f)));
                }
            }
            if (range > 0.0f) {
                fn_8040CFF8(&point, &(const Vec3f &)fn_8040D0AC(
                                        fn_80008E1C((Vec3f *)&(const Vec3f &)Vec3f(), 0.0f, 0.0f, range),
                                        fn_8040EC80(0.0f, DegToRad(360.0f))));

            }
            return *reinterpret_cast<Fn8040CopyVector3 *>(&point);
        }
    }
    return *reinterpret_cast<Fn8040CopyVector3 *>(ai->m_enemy->GetPosition());
}

void EnemyBaseAI::vfAI_0x34() {
    fn_8019F944(&m_stateMachine, GetIdleState());
}

void EnemyBaseAI::vfAI_0x38() {
    vfAI_0x3C();
}

void EnemyBaseAI::vfAI_0x3C() {
    fn_8019F944(&m_stateMachine, GetWaitState());
}

void EnemyBaseAI::vfAI_0x40() {
    fn_8019F944(&m_stateMachine, GetWaitState());
}

void EnemyBaseAI::vfAI_0x44() {
    fn_8019F944(&m_stateMachine, GetCautionState());
}

void EnemyBaseAI::vfAI_0x48() {
    fn_8019F944(&m_stateMachine, GetWaitState());
}

void EnemyBaseAI::vfAI_0x4C() {
    fn_8019F944(&m_stateMachine, GetWaitState());
}

void EnemyBaseAI::vfAI_0x50() {
    fn_8019F944(&m_stateMachine, GetGuardState());
}

void EnemyBaseAI::vfAI_0x54() {
    fn_8019F944(&m_stateMachine, GetWaitFinishDamagedState());
}

void EnemyBaseAI::vfAI_0x58() {
    fn_8019F944(&m_stateMachine, GetDeadState());
}

void EnemyBaseAI::vfAI_0x5C() {}

void EnemyBaseAI::vfAI_0x60() {
    fn_8019F9F0(&m_stateMachine);
    fn_8019F944(&m_stateMachine, GetRecoverState());
}

/* ------------------------------------------------------------------------------------------- */
/* The states. */

void EnemyAIState_EnemyBaseAI_None::Enter() {}

void EnemyAIState_EnemyBaseAI_None::Update() {}

void EnemyAIState_EnemyBaseAI_None::Exit() {}

void EnemyAIState_EnemyBaseAI_Idle::Enter() {
    if (m_ai->m_enemy->m_status->vfStat_0x24() != m_ai->m_enemy->m_para->vfPara_0x48()) {
        m_ai->m_enemy->m_status->vfStat_0x20(m_ai->m_enemy->m_para->vfPara_0x48(), true);
    }
    fn_8019F9FC(&m_ai->m_stateMachine);
}

void EnemyAIState_EnemyBaseAI_Idle::Update() {
    if (lbl_8057D6A0[0xC8] == 0) {
        fn_8019F9F0(&m_ai->m_stateMachine);
        fn_8019F898(&m_ai->m_stateMachine);
    } else if (m_ai->m_enemy->m_status->vfStat_0x2C()) {
        m_ai->m_enemy->m_status->vfStat_0x20(m_ai->m_enemy->m_para->vfPara_0x48(), true);
    }
}

void EnemyAIState_EnemyBaseAI_Idle::Exit() {
    fn_8019F9F0(&m_ai->m_stateMachine);
}

void EnemyAIState_EnemyBaseAI_CaptureWait::Enter() {
    if (m_ai->m_enemy->m_status->vfStat_0x24() != m_ai->m_enemy->m_para->vfPara_0x48()) {
        m_ai->m_enemy->m_status->vfStat_0x20(m_ai->m_enemy->m_para->vfPara_0x48(), true);
    }
    m_ai->m_enemy->m_status->vfStat_0x7C(false, false);
    m_ai->m_enemy->m_status->vfStat_0x90(false);
    m_ai->m_enemy->m_status->vfStat_0x80(false);
    fn_8019F9FC(&m_ai->m_stateMachine);
}

void EnemyAIState_EnemyBaseAI_CaptureWait::Update() {
    m_ai->m_enemy->m_disp->vfDisp_0x3C(false);
    if (m_ai->m_enemy->m_status->vfStat_0x2C()) {
        m_ai->m_enemy->m_status->vfStat_0x20(m_ai->m_enemy->m_para->vfPara_0x48(), true);
    }
}

void EnemyAIState_EnemyBaseAI_CaptureWait::Exit() {
    fn_8019F9F0(&m_ai->m_stateMachine);
    m_ai->m_enemy->m_status->vfStat_0x90(true);
    m_ai->m_enemy->m_status->vfStat_0x7C(true, true);
    m_ai->m_enemy->m_status->vfStat_0x80(true);
}

void EnemyAIState_EnemyBaseAI_WarpWait::Enter() {
    if (m_ai->m_enemy->m_status->vfStat_0x24() != m_ai->m_enemy->m_para->vfPara_0x48()) {
        m_ai->m_enemy->m_status->vfStat_0x20(m_ai->m_enemy->m_para->vfPara_0x48(), true);
    }
    m_ai->m_enemy->m_disp->vfDisp_0x54();
    m_ai->m_enemy->m_status->vfStat_0x7C(false, false);
    m_ai->m_enemy->m_status->vfStat_0x90(false);
    m_ai->m_enemy->m_status->vfStat_0xA8()->Set(10);
    fn_8019AAD4(m_ai->m_enemy, 0.0f);
    m_ai->SetTimer(0.5f);
    fn_8019F9FC(&m_ai->m_stateMachine);
}

void EnemyAIState_EnemyBaseAI_WarpWait::Update() {
    if (m_ai->IsTimeUp()) {
        fn_8019F9F0(&m_ai->m_stateMachine);
        m_ai->vfAI_0x38();
    } else {
        fn_8019AAD4(m_ai->m_enemy, 0.0f);
    }
}

void EnemyAIState_EnemyBaseAI_WarpWait::Exit() {
    fn_8019F9F0(&m_ai->m_stateMachine);
    m_ai->m_enemy->m_status->vfStat_0x90(true);
    m_ai->m_enemy->m_status->vfStat_0x7C(true, true);
    fn_8006127C(&m_ai->m_enemy->m_status->vfStat_0xA8()->m_bits, 10);
}

void EnemyAIState_EnemyBaseAI_Caution::Enter() {
    m_ai->m_enemy->m_status->vfStat_0x20(m_ai->m_enemy->m_para->vfPara_0x4C(), false);
    fn_8019B0B0(m_ai->m_enemy, fn_8019F65C(&m_ai->m_target));
    m_ai->SetTimer(3.0f);
    m_ai->m_step = 0;
}

void EnemyAIState_EnemyBaseAI_Caution::Update() {
    switch (m_ai->m_step) {
    case 0:
        m_ai->m_enemy->m_disp->vfDisp_0x30(0.2f);
        if (m_ai->m_enemy->vf_0x10() || m_ai->IsTimeUp()) {
            m_ai->m_enemy->m_status->vfStat_0x20(m_ai->m_enemy->m_para->vfPara_0x54(), false);
            m_ai->SetTimer(1.0f);
            m_ai->m_step++;
        }
        break;
    case 1:
        if (m_ai->m_enemy->m_status->vfStat_0x2C() || m_ai->IsTimeUp()) {
            m_ai->vfAI_0x48();
            return;
        }
        break;
    }
    void *key = m_ai->vfCtrl_0x10();
    if (!m_ai->m_enemy->field_0xA8.Contains(key)) {
        fn_8019F944(&m_ai->m_stateMachine, EnemyBaseAI::GetGoHomeState());

    }
}

void EnemyAIState_EnemyBaseAI_Caution::Exit() {}

void EnemyAIState_EnemyBaseAI_Guard::Enter() {
    if (m_ai->m_enemy->m_status->vfStat_0x24() != m_ai->m_enemy->m_para->vfPara_0x64()) {
        m_ai->m_enemy->m_status->vfStat_0x20(m_ai->m_enemy->m_para->vfPara_0x64(), true);
    }
    m_ai->SetTimer(1.0f);
}

void EnemyAIState_EnemyBaseAI_Guard::Update() {
    fn_8019F53C(&m_ai->m_turn, fn_8019F65C(&m_ai->m_target));
    if (m_ai->m_enemy->m_status->vfStat_0x2C() || m_ai->IsTimeUp()) {
        m_ai->vfAI_0x3C();
    }
}

void EnemyAIState_EnemyBaseAI_Guard::Exit() {}

void EnemyAIState_EnemyBaseAI_Wait::Enter() {
    fn_8017DE10(m_ai->m_enemy, &m_ai->m_enemy->field_0x48);
    Vec3f offset;
    ConstructVector(&offset, ((VectorSubtractFn)fn_8040CD74)(&m_ai->m_enemy->field_0x6C,
                                                            m_ai->m_enemy->GetPosition()));
    offset.y = 0.0f;
    if (fn_8019B298(m_ai->m_enemy, &m_ai->m_enemy->field_0x6C) < 10.0f &&
        std::fabs(fn_8019B184(m_ai->m_enemy, &offset)) < Radians(1.0f)) {
        m_ai->m_enemy->m_status->vfStat_0x20(m_ai->m_enemy->m_para->vfPara_0x48(), false);
        m_ai->m_step = 1;
    } else {
        m_ai->m_enemy->m_status->vfStat_0x20(m_ai->m_enemy->m_para->vfPara_0x4C(), false);
        m_ai->m_step = 0;
    }
}

void EnemyAIState_EnemyBaseAI_Wait::Update() {
    if (fn_8019AF04(m_ai->m_enemy)) {
        m_ai->vfAI_0x44();
        return;
    }
    switch (m_ai->m_step) {
    case 0:
        if (m_ai->m_enemy->vf_0x0C()) {
            fn_8019B0F8(m_ai->m_enemy, &m_ai->m_enemy->field_0x60);
            m_ai->m_step++;
        }
        break;
    case 1:
        m_ai->m_enemy->m_disp->vfDisp_0x30(0.2f);
        if (m_ai->m_enemy->vf_0x10()) {
            m_ai->m_enemy->m_status->vfStat_0x20(m_ai->m_enemy->m_para->vfPara_0x48(), false);
            m_ai->m_step++;
        }
        break;
    case 2:
        if (m_ai->m_enemy->m_status->vfStat_0x2C()) {
            m_ai->m_enemy->m_status->vfStat_0x20(m_ai->m_enemy->m_para->vfPara_0x48(), false);
        }
        break;
    }
}

void EnemyAIState_EnemyBaseAI_Wait::Exit() {}

/* Not called anywhere: the linker strips it from the game. Retail has the out-of-line copy of
 * GetGoHomeState right after GoHome's functions, although Caution's Update is its only caller;
 * compiling a change to the state at this point of the file puts it there (name inferred). */
void ChangeToGoHomeState(EnemyBaseAI *ai) {
    fn_8019F944(&ai->m_stateMachine, EnemyBaseAI::GetGoHomeState());
}


void EnemyAIState_EnemyBaseAI_GoHome::Enter() {
    m_ai->m_enemy->m_status->vfStat_0x20(m_ai->m_enemy->m_para->vfPara_0x4C(), true);
    fn_8017DE10(m_ai->m_enemy, &m_ai->m_enemy->field_0x48);
}

void EnemyAIState_EnemyBaseAI_GoHome::Update() {
    if (m_ai->m_enemy->vf_0x0C()) {
        m_ai->vfAI_0x3C();
    }
}

void EnemyAIState_EnemyBaseAI_GoHome::Exit() {}

void EnemyAIState_EnemyBaseAI_WaitFinishDamaged::Enter() {
    fn_8019F9FC(&m_ai->m_stateMachine);
    m_ai->m_enemy->m_status->vfStat_0xA8()->Set(2);
    m_ai->m_enemy->m_status->vfStat_0x20(m_ai->m_enemy->m_para->vfPara_0x5C(), true);
}

void EnemyAIState_EnemyBaseAI_WaitFinishDamaged::Update() {
    if (m_ai->m_enemy->m_status->vfStat_0x2C()) {
        fn_8019F9F0(&m_ai->m_stateMachine);
        m_ai->vfAI_0x3C();
    }
}

void EnemyAIState_EnemyBaseAI_WaitFinishDamaged::Exit() {
    fn_8006127C(&m_ai->m_enemy->m_status->vfStat_0xA8()->m_bits, 2);
    fn_8019F9F0(&m_ai->m_stateMachine);
}

void EnemyAIState_EnemyBaseAI_WaitFinishFalling::Enter() {
    fn_8019F9FC(&m_ai->m_stateMachine);
    m_ai->m_enemy->m_status->vfStat_0x20(m_ai->m_enemy->m_para->vfPara_0x50(), true);
}

void EnemyAIState_EnemyBaseAI_WaitFinishFalling::Update() {
    if (m_ai->m_enemy->m_status->vfStat_0x2C()) {
        fn_8019F9F0(&m_ai->m_stateMachine);
        fn_8019F898(&m_ai->m_stateMachine);
    }
}

void EnemyAIState_EnemyBaseAI_WaitFinishFalling::Exit() {
    fn_8019F9F0(&m_ai->m_stateMachine);
}

void EnemyAIState_EnemyBaseAI_Dead::Enter() {
    fn_8019F9FC(&m_ai->m_stateMachine);
    m_ai->m_enemy->m_status->vfStat_0x20(m_ai->m_enemy->m_para->vfPara_0x60(), true);
    m_ai->SetTimer(0.5f);
    m_ai->m_step = 0;
}

void EnemyAIState_EnemyBaseAI_Dead::Update() {
    switch (m_ai->m_step) {
    case 0:
        if (m_ai->IsTimeUp() || m_ai->m_enemy->m_status->vfStat_0x2C()) {
            m_ai->m_enemy->m_disp->vfDisp_0x4C();
            m_ai->m_enemy->m_set->vfSet_0x10();
            m_ai->m_step = 1;
        }
        break;
    case 1:
        break;
    }
}

void EnemyAIState_EnemyBaseAI_Dead::Exit() {
    fn_8019F9F0(&m_ai->m_stateMachine);
}

void EnemyAIState_EnemyBaseAI_Recover::Enter() {
    m_ai->m_enemy->m_status->vfStat_0xA8()->Set(0xD);
    if (m_ai->m_enemy->m_status->vfStat_0x24() != m_ai->m_enemy->m_para->vfPara_0x48()) {
        m_ai->m_enemy->m_status->vfStat_0x20(m_ai->m_enemy->m_para->vfPara_0x48(), true);
    }
    m_ai->m_enemy->m_status->vfStat_0x7C(false, true);
    m_ai->SetTimer(60.0f);
}

void EnemyAIState_EnemyBaseAI_Recover::Update() {
    if (m_ai->IsTimeUp() || m_ai->m_enemy->m_status->vfStat_0x4C()) {
        m_ai->vfAI_0x3C();
    } else if (m_ai->m_enemy->m_status->vfStat_0x2C()) {
        m_ai->m_enemy->m_status->vfStat_0x20(m_ai->m_enemy->m_para->vfPara_0x48(), true);
    }
}

void EnemyAIState_EnemyBaseAI_Recover::Exit() {
    m_ai->m_enemy->m_status->vfStat_0x7C(true, true);
    fn_8006127C(&m_ai->m_enemy->m_status->vfStat_0xA8()->m_bits, 0xD);
}
