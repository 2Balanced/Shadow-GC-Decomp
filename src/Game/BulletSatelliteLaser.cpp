/* BulletSatelliteLaser::SatelliteLaser (class name from RTTI): the shot of the satellite laser
 * weapon, in two sizes (bullet records lbl_8054DB28 and lbl_8054DB3C). It is a task that marks the
 * locked-on target's position with an effect and a sound, and a second later strikes there with
 * an attack of the owner's side, a sound and a rumble. File name from the RTTI name. Names marked
 * "(name inferred)" are not original. Definitions are in reverse address order (deferred inlining
 * emits functions last-defined first).
 *
 * Not yet matching: in the constructor retail loads the owner pointer for the null test into r5,
 * GC/2.7 into r0 (two instructions); about fifteen source shapes of the test did not change it.
 * Everything else, including the data, matches. */
#include "Effect/System/EffectElement.h"
#include "Game/CharInfo.h"
#include "Game/GadgetUtil.h"
#include "Game/Task.h"
#include "Game/Vibration.h"
#include "Game/WeaponBullet.h"
#include "Game/WeaponLockonTarget.h"
#include "boost/shared_ptr.hpp"
#include "boost/weak_ptr.hpp"
#include "size_assert.h"

using Weapon::LockonTarget;
using Weapon::ShotInfo;

/* Target (class name from RTTI): a point other objects can aim at. Only its first virtual
 * function, which returns the point, is used here (name inferred). */
class Target {
  public:
    virtual Vec3f GetPosition();
};

extern "C" {
/* The parent task of new tasks (see GadgetPickup.h). */
void *fn_8001C4F8(void);
Task *fn_801EB304(void *manager, int kind);
/* The target a lock-on follows (behaviour inferred: it returns its argument). */
const boost::weak_ptr<Target> *fn_80061050(LockonTarget *lockon);
/* Starts effect id for owner (see PlayerShadowWeaponCannon.cpp). */
boost::weak_ptr<Effect::System::EffectElement> fn_8005E3D0(int id,
                                                          const boost::weak_ptr<CharInfo> &owner);
/* Moves an effect (see BulletLaser.cpp). */
void fn_80425E4C(Effect::System::EffectElement *effect, const Vec3f *position);
/* Plays sound id at position (see Fan.cpp). */
void fn_801D54F4(void *sound, int id, const Vec3f *position, int arg3, int arg4);
/* Makes an attack of owner at position with the given kind, side flags and radius (behaviour
 * inferred). */
void fn_801DD040(const boost::weak_ptr<CharInfo> *owner, const Vec3f *position, int kind,
                 int flags, float radius);
/* Starts rumble pattern (see GadgetUtility.cpp). */
boost::weak_ptr<Vibration::RequestHandle> fn_8034DF48(int pattern, int arg1);
}

/* A bullet type's record (struct name unknown; see BulletLaser.cpp). */
struct UnkBulletType_80546018 {
    /* 0x00 */ void (*initialize)(void);
    /* 0x04 */ void (*finalize)(void);
    /* 0x08 */ void (*create)(const ShotInfo &info);
    /* 0x0C */ u32 field_0xC;
    /* 0x10 */ float m_reach;
};
SIZE_ASSERT(UnkBulletType_80546018, 0x14);

namespace BulletSatelliteLaser {

class SatelliteLaser : public Task {
  public:
    SatelliteLaser(const ShotInfo &info, int size);
    virtual ~SatelliteLaser();
    virtual void Tick(float dt);

    /* Strikes the marked position (name inferred). */
    void Strike();

    /* 0x28 */ boost::weak_ptr<CharInfo> m_owner;
    /* 0x30 */ boost::weak_ptr<Effect::System::EffectElement> m_effect;
    /* 0x38 */ s32 m_side;
    /* 0x3C */ Vec3f m_position;
    /* 0x48 */ s32 m_size; /* 0 small, 1 large */
    /* 0x4C */ float m_timer;
};
SIZE_ASSERT(SatelliteLaser, 0x50);

} // namespace BulletSatelliteLaser

using BulletSatelliteLaser::SatelliteLaser;

/* A distance (name inferred). As a call it is not a constant expression, so the records' values
 * are stored by the static initializer, as in retail. */
inline float Plain(float value) {
    return value;
}

SatelliteLaser::SatelliteLaser(const ShotInfo &info, int size)
    : Task(fn_801EB304(fn_8001C4F8(), 11)) {
    m_name = "SatelliteLaser";
    m_owner = info.m_owner;
    m_side = info.m_side;
    m_size = size;
    m_timer = 1.0f;
    boost::shared_ptr<Target> target;
    if (info.m_target.get() != 0) {
        target = fn_80061050(info.m_target.get())->lock();
    }
    boost::shared_ptr<CharInfo> owner = m_owner.lock();
    if (owner.get() == 0 || target.get() == 0) {
        m_flags |= 1;
        return;
    }
    Target *point = target.get();
    fn_800091D0(&m_position, &(const Vec3f &)point->GetPosition());
    m_effect = fn_8005E3D0(0x28B, owner);
    if (!m_effect.expired()) {
        boost::shared_ptr<Effect::System::EffectElement> effect = m_effect.lock();
        fn_80425E4C(effect.get(), &m_position);
    }
    fn_801D54F4(fn_80053164(), 0xE027, &m_position, 0, 1);
}

SatelliteLaser::~SatelliteLaser() {}

void SatelliteLaser::Tick(float dt) {
    m_timer -= dt;
    if (m_timer <= 0.0f) {
        Strike();
        m_flags |= 1;
    }
}

void SatelliteLaser::Strike() {
    int flags = 4;
    switch (m_side) {
    case 0:
        flags |= 2;
        break;
    default:
        flags |= 1;
        break;
    }
    int kind;
    float radius;
    switch (m_size) {
    case 0:
        radius = 10.0f;
        kind = 0;
        break;
    case 1:
    default:
        kind = 1;
        radius = 18.0f;
        break;
    }
    fn_801DD040(&m_owner, &m_position, kind, flags, radius);
    fn_801D54F4(fn_80053164(), 0xE028, &m_position, 0, 1);
    fn_8034DF48(7, -2);
}

/* The records' callbacks (names inferred). */
static void CreateSmall(const ShotInfo &info) {
    new SatelliteLaser(info, 0);
}

static void CreateLarge(const ShotInfo &info) {
    new SatelliteLaser(info, 1);
}

UnkBulletType_80546018 lbl_8054DB28 = {0, 0, CreateSmall, 0, Plain(300.0f)};
UnkBulletType_80546018 lbl_8054DB3C = {0, 0, CreateLarge, 0, Plain(300.0f)};
