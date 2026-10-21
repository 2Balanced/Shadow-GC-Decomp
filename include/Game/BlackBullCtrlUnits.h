#ifndef GAME_BLACKBULLCTRLUNITS_H
#define GAME_BLACKBULLCTRLUNITS_H

/* The control units both Black Bull stages use (class names from RTTI), defined in the unit at
 * 0x8023F150 (not yet decompiled); the boss tasks and mode units create them. Only what their
 * creators need is declared: the constructors (out of line, except where noted) and the sizes
 * that new passes. Member names and meanings are inferred. Kept apart from
 * Game/BlackBullCtrlUnit.h, since inline functions there would renumber the compiler-generated
 * @N locals of the units that include it. */

#include "Game/BlackBullCtrlUnit.h"
#include "Game/BossCtrlUnit.h"
#include "Game/MathTypes.h"
#include <dolphin/types.h>
#include "size_assert.h"

/* BlackBullCtrlUnit_AttackCallback (class name from RTTI; vtable at 0x80547454): receives the
 * callbacks of one of the boss's attack colliders (index picks the collider). Its constructor is
 * out of line at 0x802428F0; its destructor and vtable are emitted by Game/BlackBull0210.cpp. */
class BlackBullCtrlUnit_AttackCallback : public BlackBullCtrlUnit, public AttackCallbackContainer {
  public:
    BlackBullCtrlUnit_AttackCallback(BlackBullVarParam *param, int index);
    virtual int Tick(float dt) {
        return false;
    }
};

/* The collider callbacks of the boss's body, eyes and the rest (class names from RTTI; vtables
 * at 0x80548274, 0x80548208 and 0x8054819C). */
class BlackBullCtrlUnit_AttackCallbackBody : public BlackBullCtrlUnit_AttackCallback {
  public:
    BlackBullCtrlUnit_AttackCallbackBody(BlackBullVarParam *param, int index)
        : BossCtrlUnit(param), BlackBullCtrlUnit_AttackCallback(param, index) {}
    virtual void OnHit(SonicteamUSA::System::CharaColli::AttackCallbackParam &param);
    virtual void OnDamage(SonicteamUSA::System::CharaColli::AttackCallbackParam &param);
};

class BlackBullCtrlUnit_AttackCallbackEye : public BlackBullCtrlUnit_AttackCallback {
  public:
    BlackBullCtrlUnit_AttackCallbackEye(BlackBullVarParam *param, int index)
        : BossCtrlUnit(param), BlackBullCtrlUnit_AttackCallback(param, index) {}
    virtual void OnHit(SonicteamUSA::System::CharaColli::AttackCallbackParam &param);
    virtual void OnDamage(SonicteamUSA::System::CharaColli::AttackCallbackParam &param);
};

class BlackBullCtrlUnit_AttackCallbackOther : public BlackBullCtrlUnit_AttackCallback {
  public:
    BlackBullCtrlUnit_AttackCallbackOther(BlackBullVarParam *param, int index)
        : BossCtrlUnit(param), BlackBullCtrlUnit_AttackCallback(param, index) {}
    virtual void OnHit(SonicteamUSA::System::CharaColli::AttackCallbackParam &param);
    virtual void OnDamage(SonicteamUSA::System::CharaColli::AttackCallbackParam &param);
};

/* BlackBullCtrlUnit_CheckMotionAttribute (class name from RTTI; vtable at 0x80547E1C): passes
 * the boss's motion attributes on to its BlackBullVarParam. */
class BlackBullCtrlUnit_CheckMotionAttribute : public BlackBullCtrlUnit,
                                               public BossCtrlUnit_CheckMotionAttribute {
  public:
    BlackBullCtrlUnit_CheckMotionAttribute(BlackBullVarParam *param)
        : BossCtrlUnit(param), BlackBullCtrlUnit(param), BossCtrlUnit_CheckMotionAttribute(param) {}
    virtual ~BlackBullCtrlUnit_CheckMotionAttribute() {}
    virtual int Tick(float dt) {
        return BossCtrlUnit_CheckMotionAttribute::Tick(dt);
    }
    virtual void OnAttribute(u16 attribute, int value);
};

/* BlackBullCtrlUnit_GetStatus (vtable at 0x805482D0, constructor 0x80242DF8). */
class BlackBullCtrlUnit_GetStatus : public BlackBullCtrlUnit {
  public:
    BlackBullCtrlUnit_GetStatus(BlackBullVarParam *param);
    virtual ~BlackBullCtrlUnit_GetStatus();
    virtual int Tick(float dt);
};

/* BlackBullCtrlUnit_Move (vtable at 0x80548158, constructor 0x80242258). */
class BlackBullCtrlUnit_Move : public BlackBullCtrlUnit {
  public:
    BlackBullCtrlUnit_Move(BlackBullVarParam *param);
    virtual ~BlackBullCtrlUnit_Move();
    virtual int Tick(float dt);
};

/* BlackBullCtrlUnit_AdjustNeck and BlackBullCtrlUnit_AdjustEye (vtables at 0x8054808C and
 * 0x805480DC, constructors 0x802415CC and 0x80241A4C). */
class BlackBullCtrlUnit_AdjustNeck : public BlackBullCtrlUnit, public MotionCallbackContainer {
  public:
    BlackBullCtrlUnit_AdjustNeck(BlackBullVarParam *param);
    virtual ~BlackBullCtrlUnit_AdjustNeck();
    virtual int Tick(float dt);
    virtual void OnMotion(_stMtnCallbackParam &param);

    /* 0x10 */ u8 field_0x10[0x20 - 0x10];
};

class BlackBullCtrlUnit_AdjustEye : public BlackBullCtrlUnit, public MotionCallbackContainer {
  public:
    BlackBullCtrlUnit_AdjustEye(BlackBullVarParam *param);
    virtual ~BlackBullCtrlUnit_AdjustEye();
    virtual int Tick(float dt);
    virtual void OnMotion(_stMtnCallbackParam &param);

    /* 0x10 */ u8 field_0x10[0x20 - 0x10];
};

/* BlackBullCtrlUnit_Dizzy (vtable at 0x80547ECC, constructor 0x8023F4B4). */
class BlackBullCtrlUnit_Dizzy : public BlackBullCtrlUnit {
  public:
    BlackBullCtrlUnit_Dizzy(BlackBullVarParam *param);
    virtual ~BlackBullCtrlUnit_Dizzy();
    virtual int Tick(float dt);

    /* 0x0C */ u8 field_0xC[0x14 - 0xC];
};

/* BlackBullCtrlUnit_FireBall (vtable at 0x80548028, constructor 0x80240E60): a fire ball shot
 * from position along velocity; kind picks the attack (0 breath, 1 close range, 2 wavering;
 * meanings inferred from the callers). */
class BlackBullCtrlUnit_FireBall : public BlackBullCtrlUnit, public AttackCallbackContainer {
  public:
    BlackBullCtrlUnit_FireBall(BlackBullVarParam *param, const Vec3f *position,
                               const Vec3f *velocity, int kind);
    virtual ~BlackBullCtrlUnit_FireBall();
    virtual int Tick(float dt);
    virtual void OnHit(SonicteamUSA::System::CharaColli::AttackCallbackParam &param);

    /* 0x10 */ u8 field_0x10[0x50 - 0x10];
};

SIZE_ASSERT(BlackBullCtrlUnit_AttackCallbackBody, 0x20);
SIZE_ASSERT(BlackBullCtrlUnit_CheckMotionAttribute, 0x24);
SIZE_ASSERT(BlackBullCtrlUnit_GetStatus, 0x1C);
SIZE_ASSERT(BlackBullCtrlUnit_Move, 0x1C);
SIZE_ASSERT(BlackBullCtrlUnit_AdjustNeck, 0x30);
SIZE_ASSERT(BlackBullCtrlUnit_AdjustEye, 0x30);
SIZE_ASSERT(BlackBullCtrlUnit_Dizzy, 0x24);
SIZE_ASSERT(BlackBullCtrlUnit_FireBall, 0x60);

#endif
