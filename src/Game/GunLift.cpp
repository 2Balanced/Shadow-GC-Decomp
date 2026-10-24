/* Vehicle::GunLift (class name from RTTI): the gun lift, a vehicle (GUNLIFT.BIN) that rides a
 * path while its rider aims and fires a gun. It comes in two kinds, "GunLiftExpress" and
 * "GunLiftLocal", which differ only in their speeds. It is built from three models (body, turret
 * and gun), each with its own character info; the rider turns the turret and tilts the gun
 * (command 1), speeds up (command 0x15 action 2/3), fires (6/7) or gets off (1/4). While someone
 * rides it the lift moves along the path, fires bullets from the gun's muzzle and places the
 * camera behind the gun; attacks on the turret or gun hurt the rider. File name inferred from the
 * class. Names marked "(name inferred)" are not original. Definitions are in reverse address order
 * (deferred inlining emits functions last-defined first). The unit's code reaches its .data
 * through a pooled base address, so the linker keeps that section whole: Game/TNumEdit.h and
 * Game/CharaColliBaseCharInfo.h supply the RTTI base lists it starts with. */
#include <boost/bind.hpp>
#include <boost/function.hpp>

#include "Game/TNumEdit.h"
/* After TNumEdit.h: the order of the casts sets the order of the RTTI base lists in .data. */
#include "Game/CharaColliBaseCharInfo.h"
#include "Effect/System/EffectElement.h"
#include "Game/CameraModeUser.h"
#include "Game/CharCommand.h"
#include "Game/CharInfo.h"
#include "Game/CharaColli.h"
#include "Game/CharaColliAttack.h"
#include "Game/ExternalCameraRequest.h"
#include "Game/GadgetCharInfo.h"
#include "Game/GadgetModelInfo.h"
#include "Game/GadgetParticle.h"
#include "Game/GadgetSound.h"
#include "Game/GadgetClassDesc.h"
#include "Game/PathCtrl.h"
#include "Game/MathConstants.h"
#include "Game/TVehicleBase.h"
#include "Game/Vibration.h"
#include "Game/WeaponBullet.h"
#include "Game/FileQueue.h"
#include "boost/shared_ptr.hpp"
#include "boost/weak_ptr.hpp"
#include "size_assert.h"

using Effect::System::EffectElement;
using SonicteamUSA::System::PathCtrl;
using SonicteamUSA::System::Camera::CameraMode;
using SonicteamUSA::System::Camera::CameraModeUser;
using SonicteamUSA::System::CharaColli::AttackCallback;
using SonicteamUSA::System::CharaColli::AttackCallbackParam;
using SonicteamUSA::System::CharaColli::CharaColliAttack;
using SonicteamUSA::System::CharaColli::CharaColliBase;

class Task;
struct _stMtnPlayWork;
class UnkPathManager_8001B494;
class UnkColliderManager_80021B08;

/* A path's record as the path search's filter sees it (struct name unknown). */
struct UnkPathRecord_8001CDA0 {
    /* 0x0 */ u32 flags; /* bit 22: a gun lift's path (inferred) */
};

/* The commands the gun lift takes (AccelerateInputCommand: x turns the turret and z tilts the gun,
 * degrees per call; ControlVehicleCommand) are declared in Game/CharCommand.h. */

extern "C" {
/* Helpers as in Game/GadgetUtil.h, which this unit does not include: it reads pi as the constant
 * Game/MathConstants.h declares. */
boost::shared_ptr<GadgetCharInfo> fn_8015CD90(int arg0, int arg1);
void *fn_80053164(void);
Vec3f fn_8040B67C(const Mtx34f *matrix, const Vec3f *point);
void fn_8040CE60(Vec3f *vector);

/* The 32-bit flag word (see CityBombSmall.cpp). */
void fn_80014E04(u32 *flags);
void fn_80014E10(u32 *flags);
void fn_80014E1C(u32 *flags, int bit);
bool fn_80014E34(u32 *flags, int bit);
void fn_8006127C(u32 *flags, int bit);

/* Marks vehicle type type as in use (behaviour inferred). */
void fn_801B4850(int type);

/* The path manager: finds the path nearest position that filter accepts, tests a path, puts the
 * object at its start and moves it along (behaviour inferred, see FootingMovable.cpp). */
UnkPathManager_8001B494 *fn_8001B494(void);
bool fn_8001CDA0(UnkPathManager_8001B494 *manager, PathCtrl *path, const Vec3f *position,
                 const Vec3f *direction, int arg4,
                 u32 (*filter)(const UnkPathRecord_8001CDA0 *record), int arg6, float arg7);
bool fn_8001C91C(UnkPathManager_8001B494 *manager, PathCtrl *path, int backward,
                 GadgetCharInfo *charInfo, Vec3f *rotation, int arg5, float distance);
bool fn_800ABDCC(PathCtrl *path);
void fn_80018304(PathCtrl *path);
Vec3f fn_80018634(PathCtrl *path);
Vec3f fn_80018434(PathCtrl *path, int arg1, int arg2);

/* Clamps (see Helicopter.cpp and WeaponVacuumPod.cpp). */
const float &fn_80070460(const float &value, const float &low, const float &high);
const float &fn_80070494(const float &a, const float &b);

/* True when value is (nearly) zero (behaviour inferred). */
bool fn_8000A950(float value);

/* Vector and matrix helpers (behaviour inferred). */
Vec3f fn_8040CDA8(const Vec3f *vector, float scale);
Vec3f fn_8040B618(const Mtx34f *matrix, const Vec3f *direction);
void fn_8040CFF8(Vec3f *vector, const Vec3f *value); /* vector += value */
void fn_8040FAA0(Vec3f *translation, const Mtx34f *matrix);
void fn_8040FA20(Mtx34f *matrix, const Vec3f *translation);
void fn_800493C4(Vec3f *rotation, const Mtx34f *matrix);
void fn_800494F8(Vec3f *rotation, const Mtx34f *matrix);

/* Model helpers (see Monster.cpp, ArkCannon.cpp and Door.cpp). */
boost::shared_ptr<GadgetModelInfo> fn_8015F800(int arg0, const char *fileName);
void fn_80160A84(GadgetModelInfo *model, int arg1, int arg2);
void fn_80160670(GadgetModelInfo *model, int motion);
int fn_80161FC8(GadgetModelInfo *model, int index, const boost::weak_ptr<CharInfo> &info);
bool fn_80169EF0(Mtx34f *matrix, boost::shared_ptr<GadgetModelInfo> model, int arg2, int id);
boost::weak_ptr<CharaColliAttack> fn_80161428(GadgetModelInfo *model, int index);
boost::weak_ptr<CharaColliBase> fn_80161614(GadgetModelInfo *model, int index);
boost::weak_ptr<_stMtnPlayWork> fn_801602B4(GadgetModelInfo *model, int index);
int fn_80416F28(_stMtnPlayWork *work, int arg1, int arg2, int arg3);
int fn_80415854(_stMtnPlayWork *work, int arg1, u16 *type, void **data, int arg4);
void fn_8015FA78(GadgetModelInfo *model,
                 void *(*callback)(void *part, UnkParticleAttach_80169D00 *param),
                 UnkParticleAttach_80169D00 *param);
void *fn_80169D00(void *part, UnkParticleAttach_80169D00 *param);
void *fn_800122D4(void);
void *fn_8001277C(void *loader, int arg1, int id);

/* Particle helpers (behaviour inferred). */
boost::shared_ptr<GadgetParticle::GadgetParticleCtrl> fn_80166AF4(void);
void fn_80167010(GadgetParticle::GadgetParticleCtrl *ctrl, int id);
void fn_80167448(GadgetParticle::GadgetParticleCtrl *ctrl, u32 id);
void fn_8016761C(GadgetParticle::GadgetParticleCtrl *ctrl, float dt);

/* The collider manager and an attack collision of a collider (see Monster.cpp). */
UnkColliderManager_80021B08 *fn_80021B08(void);
boost::weak_ptr<CharaColliAttack> fn_800C42D0(UnkColliderManager_80021B08 *manager,
                                              boost::shared_ptr<CharaColliBase> collider);
/* Sets callback as the attack callback of an attack collision (see BreakRoad.cpp). */
void fn_80096B64(CharaColliAttack *attack, int arg1, const AttackCallback &callback);

/* Character infos (see GadgetUtil.h) and their command handler. */
void fn_801D54F4(void *manager, u16 id, const void *source, int arg3, int arg4);

/* An effect at source (behaviour inferred; see Fire.cpp). */
boost::weak_ptr<EffectElement> fn_8015D794(const void *source, int id);
void fn_80425E68(EffectElement *effect, const Mtx34f *matrix);

/* Bullets (see BAGunShip.cpp): what a shot would hit within range, and firing it. */
Weapon::Bullet::HitResult fn_80051AF0(int type, const Weapon::ShotInfo *shot, float range);
void fn_80051C34(int type, const Weapon::ShotInfo *shot);

/* Rumble (see Rocket.cpp and EggLastMech.cpp). */
void fn_8034DB60(boost::weak_ptr<Vibration::RequestHandle> *handle);
boost::weak_ptr<Vibration::RequestHandle> fn_8034DF48(int pattern, int arg1);

/* The camera mode (see Coaster.cpp): it is released, and the eye and the point looked at are set
 * (behaviour inferred). */
void fn_80009618(boost::shared_ptr<CameraModeUser> *camera);
void fn_8000DB78(CameraModeUser *mode, const Vec3f *eye);
void fn_8000DB50(CameraModeUser *mode, const Vec3f *target);
}

/* The vector type's out-of-line copy constructor and assignment (see GadgetUtil.h). */
inline void ConstructVector(Vec3f *vector, const Vec3f &value) {
    fn_800091EC(vector, &value);
}

inline void AssignVector(Vec3f &vector, const Vec3f &value) {
    fn_800091D0(&vector, &value);
}

/* fn_8040CD74 called as returning a - b by value (it stores the result through its first
 * argument). */
typedef Vec3f (*VectorSubtractFn)(const Vec3f *a, const Vec3f *b);

/* The 32-bit flag word at 0x15C (struct name unknown; see CityBombSmall.cpp). */
struct GunLiftFlags {
    GunLiftFlags() {
        fn_80014E04(&m_bits);
    }
    void Clear() {
        fn_80014E10(&m_bits);
    }
    void Set(int bit) {
        fn_80014E1C(&m_bits, bit);
    }
    void Clear(int bit) {
        fn_8006127C(&m_bits, bit);
    }
    bool Test(int bit) {
        return fn_80014E34(&m_bits, bit);
    }

    /* bit 0: firing, 2: speeding up, 3: moving (its sound plays), 4: the gun's motion plays */
    u32 m_bits;
};

/* A kind's speeds (struct name unknown; names inferred). */
struct GunLiftSpec {
    const float &maxSpeed;
    const float &accel;
    const float &cruiseSpeed;
    const float &cruiseAccel;
};

/* A vehicle kind's entry in the vehicle table (struct name unknown). */
struct UnkVehicleEntry {
    const char *name;
    Vehicle::TVehicleBase *(*create)(void *arg);
};

namespace Vehicle {

class GunLift : public TVehicleBase {
  public:
    GunLift(int type, void *arg);
    virtual ~GunLift();
    virtual void SetPlacement(const Vec3f *position, const Vec3f *rotation);
    virtual void GetSeat(Vec3f *position, Vec3f *rotation);
    virtual void Setup();
    virtual void OnCommand(CharCommand *command);
    virtual void Tick(float dt);
    virtual void OnGetOn();
    virtual void OnGetOff();
    virtual bool vf_0x50();
    virtual bool vf_0x54();

    /* Sends the rider off (name inferred). */
    void LetOff();
    /* Finds the gun lift's path nearest position (name inferred). */
    void FindPath(const Vec3f *position);
    /* Moves along the path and places the models (name inferred). */
    void UpdateTransforms(float dt);
    /* The camera mode's update handler (name inferred). */
    void OnCameraUpdate(CameraModeUser::UpdateEventArgs &args);
    /* The turret's and the gun's attack callback (name inferred). */
    void OnAttack(AttackCallbackParam &param);

    /* Member names are inferred. */
    /* 0x09C */ s32 m_type; /* 0 express, 1 local */
    /* 0x0A0 */ const GunLiftSpec *m_spec;
    /* 0x0A4 */ PathCtrl m_path;
    /* 0x0FC */ boost::shared_ptr<GadgetCharInfo> m_bodyInfo;
    /* 0x104 */ boost::shared_ptr<GadgetCharInfo> m_turretInfo;
    /* 0x10C */ boost::shared_ptr<GadgetCharInfo> m_gunInfo;
    /* 0x114 */ boost::shared_ptr<GadgetModelInfo> m_body;
    /* 0x11C */ boost::shared_ptr<GadgetModelInfo> m_turret;
    /* 0x124 */ boost::shared_ptr<GadgetModelInfo> m_gun;
    /* 0x12C */ boost::shared_ptr<GadgetParticle::GadgetParticleCtrl> m_bodyParticles;
    /* 0x134 */ boost::shared_ptr<GadgetParticle::GadgetParticleCtrl> m_turretParticles;
    /* 0x13C */ boost::shared_ptr<GadgetParticle::GadgetParticleCtrl> m_gunParticles;
    /* 0x144 */ boost::weak_ptr<EffectElement> m_sight;
    /* 0x14C */ boost::weak_ptr<PJSCamera::ExternalCameraRequest> m_cameraRequest;
    /* 0x154 */ boost::shared_ptr<CameraModeUser> m_camera;
    /* 0x15C */ GunLiftFlags m_state;
    /* 0x160 */ float m_pitch; /* the gun's tilt */
    /* 0x164 */ float m_yaw;   /* the turret's turn */
    /* 0x168 */ float m_speed;
    /* 0x16C */ float m_fireTimer;
    /* 0x170 */ Mtx34f m_gunMatrix;
    /* 0x1A0 */ UnkSoundHandle_801D3DC8 m_moveSound;
    /* 0x1BC */ UnkSoundHandle_801D3DC8 m_accelSound;
    /* 0x1D8 */ boost::weak_ptr<Vibration::RequestHandle> m_vibration;
};
SIZE_ASSERT(GunLift, 0x1E0);

} // namespace Vehicle

using Vehicle::GunLift;
using Vehicle::TVehicleBase;

static TVehicleBase *CreateExpress(void *arg);
static TVehicleBase *CreateLocal(void *arg);

/* The kinds' names and speeds. */
static const char *s_expressName = "GunLiftExpress";
static const char *s_localName = "GunLiftLocal";
static const float s_expressMaxSpeed = 400.0f;
static const float s_expressAccel = 200.0f;
static const float s_expressCruiseSpeed = 100.0f;
static const float s_expressCruiseAccel = 50.0f;
static const float s_localMaxSpeed = 200.0f;
static const float s_localAccel = 100.0f;
static const float s_localCruiseSpeed = 50.0f;
static const float s_localCruiseAccel = 50.0f;
static const GunLiftSpec s_specs[2] = {
    {s_expressMaxSpeed, s_expressAccel, s_expressCruiseSpeed, s_expressCruiseAccel},
    {s_localMaxSpeed, s_localAccel, s_localCruiseSpeed, s_localCruiseAccel},
};

/* The muzzle and the camera's eye relative to the gun (names inferred). */
static Vec3f s_muzzle(0.0f, 2.92f, -55.491f);
static Vec3f s_eye(0.0f, 13.039f, 0.3f);

GunLift::GunLift(int type, void *arg) : TVehicleBase(arg), m_type(type), m_spec(&s_specs[type]) {
    field_0x5C = 6;
    field_0x30 = 1;
}

GunLift::~GunLift() {
    {
        boost::shared_ptr<EffectElement> sight = m_sight.lock();
        if (sight.get() != 0) {
            sight->Retire();
        }
    }
    m_sight.reset();
    if (!m_cameraRequest.expired()) {
        fn_801CEAD4(&m_cameraRequest);
    }
    fn_80009618(&m_camera);
    m_moveSound.fn_801D455C();
    m_accelSound.fn_801D455C();
}

void GunLift::Setup() {
    m_fireTimer = 0.0f;
    m_speed = 0.0f;
    m_yaw = 0.0f;
    m_pitch = 0.0f;
    m_state.Clear();
    SetField98Float(1.0f);
    m_bodyInfo = fn_8015CD90(0x5E, 4);
    m_bodyInfo->field_0x30 = 1;
    m_turretInfo = fn_8015CD90(0x5E, 4);
    m_turretInfo->field_0x30 = 1;
    m_gunInfo = fn_8015CD90(0x5E, 4);
    m_gunInfo->field_0x30 = 2;
    m_body = fn_8015F800(1, "GUNLIFT.BIN");
    m_body->fn_80162A68(0, 0);
    fn_80161FC8(m_body.get(), 0, m_bodyInfo);
    m_turret = fn_8015F800(1, "GUNLIFT.BIN");
    m_turret->fn_80162A68(1, 0);
    fn_80161FC8(m_turret.get(), 1, GetSelf());
    m_gun = fn_8015F800(1, "GUNLIFT.BIN");
    m_gun->fn_80162A68(2, 0);
    fn_80161FC8(m_gun.get(), 2, m_gunInfo);
    {
        void *clump = fn_8001277C(fn_800122D4(), 1, 0x79);
        SetClump(clump);
    }
    {
        boost::shared_ptr<CharaColliAttack> attack = fn_80161428(m_turret.get(), 1).lock();
        if (attack.get() != 0) {
            fn_80096B64(attack.get(), 1, boost::bind(&GunLift::OnAttack, this, _1));
            attack->field_0x55 = 0;
        }
    }
    {
        boost::shared_ptr<CharaColliAttack> attack = fn_80161428(m_gun.get(), 2).lock();
        if (attack.get() != 0) {
            fn_80096B64(attack.get(), 1, boost::bind(&GunLift::OnAttack, this, _1));
            attack->field_0x55 = 0;
        }
    }
    m_bodyParticles = fn_80166AF4();
    {
        UnkParticleAttach_80169D00 attach;
        attach.ctrl = m_bodyParticles;
        fn_8015FA78(m_body.get(), fn_80169D00, &attach);
    }
    m_turretParticles = fn_80166AF4();
    {
        UnkParticleAttach_80169D00 attach;
        attach.ctrl = m_turretParticles;
        fn_8015FA78(m_turret.get(), fn_80169D00, &attach);
    }
    fn_80167010(m_bodyParticles.get(), 0x1F4);
    m_camera = fn_80009704(0, 0);
    fn_800A673C(&m_camera.get()->m_onUpdate,
                &(const CameraModeUser::UpdateCallback &)CameraModeUser::UpdateCallback(
                    boost::bind(&GunLift::OnCameraUpdate, this, _1)));
}

void GunLift::SetPlacement(const Vec3f *position, const Vec3f *rotation) {
    FindPath(position);
    UpdateTransforms(0.0f);
}

void GunLift::GetSeat(Vec3f *position, Vec3f *rotation) {
    Mtx34f matrix;

    fn_8040C148(&matrix, &lbl_805E0720);
    fn_80169EF0(&matrix, m_turret, 1, 10);
    if (position != 0) {
        fn_8040FAA0(position, &matrix);
    }
    if (rotation != 0) {
        fn_800493C4(rotation, &matrix);
    }
}

void GunLift::OnGetOn() {
    TVehicleBase::OnGetOn();
    m_pitch = 0.0f;
    m_speed = 0.0f;
    {
        boost::shared_ptr<CharaColliBase> collider = fn_80161614(m_turret.get(), 1).lock();
        if (collider.get() != 0) {
            boost::shared_ptr<CharaColliAttack> attack =
                fn_800C42D0(fn_80021B08(), collider).lock();
            if (attack.get() != 0) {
                attack->field_0x55 = 1;
            }
        }
    }
    {
        boost::shared_ptr<CharaColliBase> collider = fn_80161614(m_gun.get(), 2).lock();
        if (collider.get() != 0) {
            boost::shared_ptr<CharaColliAttack> attack =
                fn_800C42D0(fn_80021B08(), collider).lock();
            if (attack.get() != 0) {
                attack->field_0x55 = 1;
            }
        }
    }
    m_cameraRequest = fn_801CED24(boost::shared_ptr<CameraMode>(m_camera), 8, 0, 0, 0, 0, 1);
    m_gun->fn_80160C28(2);
    m_gun->fn_8015FF60();
    fn_80160A84(m_gun.get(), 2, 1);
    m_state.Set(4);
    m_gunParticles.reset();
    m_gunParticles = fn_80166AF4();
    {
        UnkParticleAttach_80169D00 attach;
        attach.ctrl = m_gunParticles;
        fn_8015FA78(m_gun.get(), fn_80169D00, &attach);
    }
    fn_80167010(m_gunParticles.get(), 0x1F6);
    m_sight = fn_8015D794(this, 0xCC);
    fn_801D54F4(fn_80053164(), 0x7637, m_gunInfo.get(), 0, 1);
}

void GunLift::LetOff() {
    boost::shared_ptr<CharInfo> rider = GetRider().lock();
    if (rider.get() != 0) {
        rider->PushCommand(new Player::StartJumpCommand(4));
    }
}

void GunLift::OnGetOff() {
    TVehicleBase::OnGetOff();
    {
        boost::shared_ptr<EffectElement> sight = m_sight.lock();
        if (sight.get() != 0) {
            sight->Retire();
        }
    }
    m_sight.reset();
    m_gunParticles.reset();
    fn_80160670(m_gun.get(), 2);
    m_state.Clear(4);
    {
        boost::shared_ptr<CharaColliBase> collider = fn_80161614(m_turret.get(), 1).lock();
        if (collider.get() != 0) {
            boost::shared_ptr<CharaColliAttack> attack =
                fn_800C42D0(fn_80021B08(), collider).lock();
            if (attack.get() != 0) {
                attack->field_0x55 = 0;
            }
        }
    }
    {
        boost::shared_ptr<CharaColliBase> collider = fn_80161614(m_gun.get(), 2).lock();
        if (collider.get() != 0) {
            boost::shared_ptr<CharaColliAttack> attack =
                fn_800C42D0(fn_80021B08(), collider).lock();
            if (attack.get() != 0) {
                attack->field_0x55 = 0;
            }
        }
    }
    m_pitch = 0.0f;
    m_speed = 0.0f;
    fn_801CEAD4(&m_cameraRequest);
    m_state.Clear(0);
    m_state.Clear(3);
    m_moveSound.fn_801D455C();
    m_accelSound.fn_801D455C();
    fn_8034DB60(&m_vibration);
}

void GunLift::Tick(float dt) {
    if (fn_80014E34(&m_vehicleFlags, 0) && fn_800ABDCC(&m_path)) {
        if (m_state.Test(2)) {
            m_speed += m_spec->accel * dt;
            m_speed = fn_80070460(m_speed, 0.0f, m_spec->maxSpeed);
        } else if (m_speed > m_spec->cruiseSpeed) {
            m_speed -= m_spec->accel * dt;
            m_speed = fn_80070494(m_speed, m_spec->cruiseSpeed);
        } else {
            m_speed += m_spec->cruiseAccel * dt;
            m_speed = fn_80070460(m_speed, 0.0f, m_spec->cruiseSpeed);
        }
        if (fn_8001C91C(fn_8001B494(), &m_path, 0, m_bodyInfo.get(), 0, 0, m_speed * dt)) {
            m_speed = 0.0f;
            if (m_state.Test(3)) {
                fn_801D54F4(fn_80053164(), 0x7636, m_turretInfo.get(), 0, 1);
                m_moveSound.fn_801D455C();
                m_accelSound.fn_801D455C();
                m_state.Clear(3);
            }
        } else {
            if (!m_state.Test(3)) {
                m_moveSound = fn_801D3DC8(0x7635, m_bodyInfo.get(), this);
                fn_801D54F4(fn_80053164(), 0x7636, m_turretInfo.get(), 0, 1);
                m_state.Set(3);
            }
            if (m_state.Test(2) && m_accelSound.m_id == 0) {
                m_accelSound = fn_801D3DC8(0x7647, m_bodyInfo.get(), this);
            }
        }
        if (m_state.Test(4)) {
            boost::shared_ptr<_stMtnPlayWork> work = fn_801602B4(m_gun.get(), 2).lock();
            if (work.get() != 0) {
                if (fn_80416F28(work.get(), 0, 3, 1) != 0) {
                    m_state.Clear(4);
                } else {
                    _stMtnPlayWork *iterator = work.get();
                    u16 type;
                    void *data;
                    while (fn_80415854(iterator, 0, &type, &data, 0) != 0) {
                        if (type == 0x5001) {
                            const u32 *soundId = (const u32 *)data;
                            fn_801D54F4(fn_80053164(), *soundId, m_gunInfo.get(), 0, 1);
                        }
                        iterator = 0;
                    }
                }
            }
        }
    }
    UpdateTransforms(dt);
    if (fn_80014E34(&m_vehicleFlags, 0)) {
        Vec3f start;
        Vec3f direction(0.0f, 0.0f, -1.0f);

        AssignVector(start, fn_8040B67C(&m_gunMatrix, &s_muzzle));
        AssignVector(direction, fn_8040B618(&m_gunMatrix, &direction));
        Weapon::ShotInfo shot(GetShooter(), false, start, direction, m_bodyInfo->field_0x18);
        Weapon::Bullet::HitResult hit = fn_80051AF0(0x1C, &shot, 5.0f);
        if (hit.m_hit) {
            boost::shared_ptr<EffectElement> sight = m_sight.lock();
            if (sight.get() != 0) {
                Mtx34f matrix;

                fn_8040C148(&matrix, &m_gunMatrix);
                fn_8040FA20(&matrix, (const Vec3f *)&hit.m_position);
                sight->Show();
                fn_80425E68(sight.get(), &matrix);
            }
        } else {
            boost::shared_ptr<EffectElement> sight = m_sight.lock();
            if (sight.get() != 0) {
                sight->Hide();
            }
        }
        if (m_state.Test(0)) {
            m_fireTimer -= dt;
            if (m_fireTimer < 0.0f) {
                fn_80051C34(0x1C, &shot);
                fn_80167448(m_gunParticles.get(), 0x1F7);
                fn_801D54F4(fn_80053164(), 0x200B, &start, 0, 1);
                m_fireTimer = 0.0625f;
                fn_8034DB60(&m_vibration);
                m_vibration = fn_8034DF48(2, -2);
            }
        }
    }
    m_body->fn_80162FB0(dt);
    m_turret->fn_80162FB0(dt);
    m_gun->fn_80162FB0(dt);
    fn_8016761C(m_bodyParticles.get(), dt);
    fn_8016761C(m_turretParticles.get(), dt);
    if (m_gunParticles.get() != 0) {
        fn_8016761C(m_gunParticles.get(), dt);
    }
}

/* The path search's filter: accepts the gun lift's paths (name inferred). */
static u32 IsGunLiftPath(const UnkPathRecord_8001CDA0 *record) {
    return (record->flags >> 22) & 1;
}

void GunLift::FindPath(const Vec3f *position) {
    fn_8001CDA0(fn_8001B494(), &m_path, position, &lbl_805E07A0, 0, IsGunLiftPath, 0, 0.0f);
    if (fn_800ABDCC(&m_path)) {
        fn_80018304(&m_path);
    }
}

void GunLift::UpdateTransforms(float dt) {
    if (fn_800ABDCC(&m_path)) {
        Vec3f previous;
        ConstructVector(&previous, m_bodyInfo.px->m_position);
        fn_800091D0(&m_bodyInfo->m_position, &(const Vec3f &)fn_80018634(&m_path));
        m_bodyInfo->m_rotation = fn_80018434(&m_path, 1, 0);
        if (fn_8000A950(dt)) {
            fn_8040CE60(&m_bodyInfo->field_0x18);
        } else {
            AssignVector(m_bodyInfo.px->field_0x18,
                         ((VectorSubtractFn)fn_8040CD74)(&m_bodyInfo.px->m_position, &previous));
            AssignVector(m_bodyInfo.px->field_0x18,
                         fn_8040CDA8(&m_bodyInfo.px->field_0x18, 1.0f / dt));
        }
        Mtx34f matrix;

        fn_8040C148(&matrix, &lbl_805E0720);
        fn_8040B59C(&matrix, &m_bodyInfo->m_position);
        fn_8040B848(&matrix, m_bodyInfo->m_rotation.y);
        m_body->fn_801633DC(&matrix);
    }
    Mtx34f matrix;

    fn_8040C148(&matrix, &lbl_805E0720);
    fn_80169EF0(&matrix, m_body, 0, 1);
    fn_8040FAA0(&m_turretInfo->m_position, &matrix);
    fn_800493C4(&m_turretInfo->m_rotation, &matrix);
    m_turretInfo->m_rotation.y += m_yaw;
    fn_8040C148(&matrix, &lbl_805E0720);
    fn_8040B59C(&matrix, &m_turretInfo->m_position);
    fn_8040B80C(&matrix, m_turretInfo->m_rotation.z);
    fn_8040B884(&matrix, m_turretInfo->m_rotation.x);
    fn_8040B848(&matrix, m_turretInfo->m_rotation.y);
    m_turret->fn_801633DC(&matrix);
    fn_80169EF0(&m_gunMatrix, m_turret, 1, 2);
    fn_8040FAA0(&m_gunInfo->m_position, &m_gunMatrix);
    fn_800494F8(&m_gunInfo->m_rotation, &m_gunMatrix);
    m_gunInfo->m_rotation.x += m_pitch;
    fn_8040C148(&m_gunMatrix, &lbl_805E0720);
    fn_8040B59C(&m_gunMatrix, &m_gunInfo->m_position);
    fn_8040B848(&m_gunMatrix, m_gunInfo->m_rotation.y);
    fn_8040B884(&m_gunMatrix, m_gunInfo->m_rotation.x);
    fn_8040B80C(&m_gunMatrix, m_gunInfo->m_rotation.z);
    m_gun->fn_801633DC(&m_gunMatrix);
    TVehicleBase::SetPlacement(&m_turretInfo->m_position, &m_turretInfo->m_rotation);
}

/* Degrees to radians (name inferred). */
inline float DegToRad(float degrees) {
    return degrees * (lbl_805FAD38 / 180.0f);
}

void GunLift::OnCommand(CharCommand *command) {
    switch (command->TypeId()) {
    case 1: {
        AccelerateInputCommand *steer = static_cast<AccelerateInputCommand *>(command);
        m_pitch += 1.5f * DegToRad(steer->m_direction.z);
        m_yaw -= 1.5f * DegToRad(steer->m_direction.x);
        m_pitch = fn_80070460(m_pitch, DegToRad(-10.0f), DegToRad(60.0f));
        command->Flags() |= 2;
        break;
    }
    case 0x15:
        switch (static_cast<ControlVehicleCommand *>(command)->m_action) {
        case 1:
            LetOff();
            command->Flags() |= 2;
            break;
        case 6:
            m_state.Set(0);
            m_fireTimer = 0.0f;
            command->Flags() |= 2;
            break;
        case 7:
            m_state.Clear(0);
            fn_8034DB60(&m_vibration);
            command->Flags() |= 2;
            break;
        case 4:
            LetOff();
            command->Flags() |= 2;
            break;
        case 2:
            m_state.Set(2);
            break;
        case 3:
            m_state.Clear(2);
            m_accelSound.fn_801D455C();
            break;
        }
        break;
    }
}

void GunLift::OnCameraUpdate(CameraModeUser::UpdateEventArgs &args) {
    if (m_camera.get() != 0) {
        Vec3f eye;
        Vec3f target;

        AssignVector(eye, fn_8040B67C(&m_gunMatrix, &s_eye));
        fn_8000DB78(m_camera.get(), &eye);
        AssignVector(target, fn_8040B67C(&m_gunMatrix, &s_muzzle));
        Vec3f up(0.0f, 1.0f, 0.0f);
        AssignVector(up, fn_8040B618(&m_gunMatrix, &up));
        fn_8040CFF8(&target, &up);
        fn_8000DB50(m_camera.get(), &target);
    }
}

void GunLift::OnAttack(AttackCallbackParam &param) {
    param.m_result = 5;
    if (fn_80014E34(&m_vehicleFlags, 0)) {
        param.m_result = 1;
        boost::shared_ptr<CharInfo> rider = GetRider().lock();
        if (rider.get() != 0) {
            rider->PushCommand(new DamageCommand(Vec3f(0.0f, 1.0f, 0.0f), 4, 1.0f));
        }
        fn_80167448(m_turretParticles.get(), 0x1F5);
    }
}

extern "C" void fn_803160B8(void) {
    fn_801B4850(0xC);
    fn_801B4850(0xD);
    fn_8006B474("GUNLIFT.BIN", 1);
}

extern "C" void fn_803160B4(void) {}

GadgetClassDesc lbl_8055C06C = {
    "GunLift", fn_803160B8, fn_803160B4, 0, 0, 0x80, 0x1518, 4, 0, 0, 0,
};

/* The vehicle table's entries for the two kinds (it refers to them by address).
 * Defined here, after the functions: retail places them after Tick's constants. */
UnkVehicleEntry lbl_805ED118 = {s_expressName, CreateExpress};
UnkVehicleEntry lbl_805ED120 = {s_localName, CreateLocal};

static TVehicleBase *CreateExpress(void *arg) {
    return new GunLift(0, arg);
}

static TVehicleBase *CreateLocal(void *arg) {
    return new GunLift(1, arg);
}

bool GunLift::vf_0x54() {
    return true;
}

bool GunLift::vf_0x50() {
    return true;
}
