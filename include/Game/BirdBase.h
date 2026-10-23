#ifndef GAME_BIRDBASE_H
#define GAME_BIRDBASE_H

#include "Game/CharaColli.h"
#include "Game/CharaColliAttack.h"
#include "Game/MathTypes.h"
#include "Game/TVehicleBase.h"
#include "boost/shared_ptr.hpp"
#include <dolphin/types.h>
#include "size_assert.h"

extern "C" {
/* The 32-bit flag word (see Game/BAMine.cpp). */
void fn_80014E04(u32 *flags);
}

namespace Vehicle {
namespace BirdBase {

class BirdModelBase;

/* A bird's tuning (struct name unknown; Game/VehicleAirBolt.cpp has AirBolt's). Times are in
 * seconds and angles in degrees; member names are inferred from their use. */
struct BirdParam {
    /* 0x00 */ float m_radius;        /* of the movement collision */
    /* 0x04 */ float m_bodyRadius;    /* of the body collider */
    /* 0x08 */ float m_drag;
    /* 0x0C */ float m_minSpeed;
    /* 0x10 */ float m_maxSpeed;
    /* 0x14 */ float m_accelTime;
    /* 0x18 */ float m_decelTime;
    /* 0x1C */ float m_returnPower;
    /* 0x20 */ float m_pitchSpeed;
    /* 0x24 */ float m_yawSpeed;
    /* 0x28 */ float m_maxPitch;
    /* 0x2C */ float m_maxYaw;
    /* 0x30 */ float m_levelSpeed;
    /* 0x34 */ float m_pathRange;
    /* 0x38 */ float m_maxOffset;
    /* 0x3C */ float m_pathScale;
    /* 0x40 */ float m_hitSpeed;
    /* 0x44 */ float m_maxHitRatio;
    /* 0x48 */ float m_pushSpeed;
    /* 0x4C */ float m_maxPushRatio;
    /* 0x50 */ float m_damage;
    /* 0x54 */ float m_flinchTime;
    /* 0x58 */ float m_returnTime;
};
SIZE_ASSERT(BirdParam, 0x5C);

/* The 32-bit flag words at 0x9C and 0x114 (struct name unknown; see Game/BAMine.cpp). */
struct BirdFlags {
    BirdFlags() {
        fn_80014E04(&m_bits);
    }

    u32 m_bits;
};

/* The member at 0x104 (class name unknown; code at 0x801B42E0..0x801B4724, not yet matched): the
 * path the bird follows. It points to its state (0x7C bytes, holding a PathCtrl). */
struct UnkBirdPath_801B446C {
    UnkBirdPath_801B446C();  /* 0x801B446C */
    ~UnkBirdPath_801B446C(); /* 0x801B4394 */

    /* 0x0 */ void *m_impl;
};

/* The member at 0x108 (class name unknown; code at 0x801B4CF4..0x801B4F04, not yet matched): the
 * bird's body collider and its hit callback. It points to its state (0x38 bytes). */
struct UnkBirdCollision_801B4EBC {
    UnkBirdCollision_801B4EBC();  /* 0x801B4EBC */
    ~UnkBirdCollision_801B4EBC(); /* 0x801B4E00 */

    /* 0x0 */ void *m_impl;
};

/* The object at 0x100 (class name unknown; constructor 0x801B3148, destructor 0x801B308C). */
struct UnkBirdTrail_801B3148;

/* Vehicle::BirdBase::BirdBase (class name from RTTI; Game/BirdBase.cpp): a flying vehicle that
 * follows a path while its rider steers it, the base of Vehicle::BirdBase::AirBolt and
 * Vehicle::BirdBase::AirWing. Member and function names are inferred. */
class BirdBase : public TVehicleBase {
  public:
    BirdBase(void *arg, const BirdParam *param);
    virtual ~BirdBase();
    virtual void SetPlacement(const Vec3f *position, const Vec3f *rotation);
    virtual void OnCommand(CharCommand *command);
    virtual void vf_0x40(UnkColliContactNode_800587A0 *const &hit);
    virtual void Tick(float dt);
    virtual void OnGetOn();
    virtual void OnGetOff();
    virtual bool vf_0x54();
    virtual bool vf_0x60();
    virtual float vf_0x64();
    virtual void vf_0x7C(void *arg);

    void UpdateLaunch(float dt);
    void UpdateFlight(float dt);
    void UpdateLanding(float dt);
    void CreateAttack();
    void Release();
    void Damage(float power, const Vec3f *position, const Vec3f *direction);
    void OnAttackHit(SonicteamUSA::System::CharaColli::AttackCallbackParam &param);
    void OnAttackEnd(SonicteamUSA::System::CharaColli::AttackCallbackParam &param);
    void Push(const Vec3f *position, const Vec3f *normal, float power);
    void OnHit(SonicteamUSA::System::CharaColli::AttackCallbackParam &param);
    void UpdateMove(float dt);
    void UpdatePathMove(float dt);
    Mtx34f GetPathMatrix();
    void AlignToPath();
    void ClampToPath();
    void UpdateDrift(float dt);
    void UpdateRotation(float dt);
    void CheckContacts();

    /* 0x09C */ BirdFlags m_flags;
    /* 0x0A0 */ BirdModelBase *m_model;
    /* 0x0A4 */ Vec3f m_prevPosition;
    /* 0x0B0 */ Vec3f m_angles;  /* steering angles on the path */
    /* 0x0BC */ Vec3f m_offset;  /* from the path */
    /* 0x0C8 */ Vec3f m_offsetVelocity;
    /* 0x0D4 */ Vec3f m_offsetForce;
    /* 0x0E0 */ Vec3f m_input;   /* the rider's steering */
    /* 0x0EC */ float m_speed;
    /* 0x0F0 */ float m_returnPower;
    /* 0x0F4 */ const BirdParam *m_param;
    /* 0x0F8 */ s32 m_step;
    /* 0x0FC */ s32 m_state;
    /* 0x100 */ UnkBirdTrail_801B3148 *m_trail;
    /* 0x104 */ UnkBirdPath_801B446C m_path;
    /* 0x108 */ UnkBirdCollision_801B4EBC m_collision;
    /* 0x10C */ boost::shared_ptr<SonicteamUSA::System::CharaColli::CharaColliAttack> m_attack;
    /* 0x114 */ BirdFlags m_moveFlags;
    /* 0x118 */ float m_flinchTimer;
    /* 0x11C */ float m_returnTimer;
};
SIZE_ASSERT(BirdBase, 0x120);

} // namespace BirdBase
} // namespace Vehicle

#endif
