/* Vehicle::BirdBase::AirBolt (class name from RTTI): the AirBolt, a flying vehicle (a
 * Vehicle::BirdBase::BirdBase) with an Weapon::AirBolt::AirBolt lock-on gun. The rider locks on
 * (command 0x15, action 6) and fires on release (action 7); the gun's six muzzles follow the
 * model's frames 1 to 6. The vehicle table creates it through lbl_805EC0E4. File name from the
 * class (Game/WeaponAirBolt.cpp holds the gun); member names are inferred. Definitions are in
 * reverse address order (deferred inlining emits functions last-defined first). */
#include "Game/AirBoltModel.h"
#include "Game/BirdBase.h"
#include "Game/CharCommand.h"
#include "Game/GadgetClassDesc.h"
#include "Game/MathConstants.h"
#include "Game/UnkMatrix_800B1304.h"
#include "Game/Vibration.h"
#include "Game/WeaponAirBolt.h"
#include "boost/shared_ptr.hpp"
#include "boost/weak_ptr.hpp"
#include "size_assert.h"

/* A vehicle kind's entry in the vehicle table (see Game/GunLift.cpp). */
struct UnkVehicleEntry {
    const char *name;
    Vehicle::TVehicleBase *(*create)(void *arg);
};

/* The commands the AirBolt takes itself (class name unknown; see Game/BirdBase.cpp). */
class UnkActionCommand_AirBolt : public CharCommand {
  public:
    /* 0x14 */ s32 m_action;
};

extern "C" {
/* The 32-bit flag word (see Game/BAMine.cpp). */
void fn_80014E04(u32 *flags);
void fn_80014E10(u32 *flags);
void fn_80014E1C(u32 *flags, int bit);
void fn_8006127C(u32 *flags, int bit);
bool fn_80014E34(u32 *flags, int bit);

/* Debug output and resource loading (see Game/WeaponDarkHammer.cpp), and the vehicle type in use
 * (see Game/GunLift.cpp). */
void fn_803A953C(const char *format, ...);
void fn_8006B50C(int id, int arg1);
void fn_801B4850(int type);

/* TVehicleBase's other functions (behaviour inferred): the seat's position and rotation (either
 * may be null), a float of the object at 0x98, two calls on it when the rider gets on, and a
 * second model clump. */
void fn_801B65DC(Vehicle::TVehicleBase *vehicle, Vec3f *position, Vec3f *rotation);
void fn_801B6570(Vehicle::TVehicleBase *vehicle, float value);
void fn_801B63B0(Vehicle::TVehicleBase *vehicle);
void fn_801B644C(Vehicle::TVehicleBase *vehicle);
void fn_801B64B8(Vehicle::TVehicleBase *vehicle, void *clump);
/* The vehicle's display object (TVehicleBase+0x74): follows a model frame (behaviour inferred). */
void fn_8034F250(void *display, void *frame);

/* Weapon::WeaponLockonBase methods (see Game/PlayerShadowWeaponLockon.cpp). */
void fn_8005FA68(Weapon::WeaponLockonBase *weapon);
void fn_8005FA38(Weapon::WeaponLockonBase *weapon);
float fn_8005FA98(Weapon::WeaponLockonBase *weapon);
int fn_8005F9A0(Weapon::WeaponLockonBase *weapon);

/* Rumble (see Game/GunLift.cpp). */
boost::weak_ptr<Vibration::RequestHandle> fn_8034DF48(int pattern, int player);

/* The resource loader and a model clump in it (see Game/GunLift.cpp). */
void *fn_800122D4(void);
void *fn_8001277C(void *loader, int arg1, int id);

/* Matrix helpers (behaviour inferred). */
Vec3f fn_8040FB08(const void *matrix);                       /* its translation */
Vec3f fn_8040B618(const void *matrix, const Vec3f *direction); /* a direction turned by it */
/* The matrix rotated by the angles. */
Mtx34f fn_8040FFBC(const Mtx34f *matrix, const Vec3f *angles, int order);
Vec3f fn_8040F5F4(const Vec3f *forward, const Vec3f *up, int order);    /* Euler angles */
}

/* fn_8040B034 called as returning a pure translation by value (it stores the matrix through its
 * first argument). */
typedef Mtx34f (*TranslationFn)(const Vec3f *offset);

/* boost/shared_ptr.hpp only declares shared_ptr::reset, which the game has out of line; this unit
 * emits its own copy (0x802CC394), so it defines it here (boost 1.32's definition; see
 * Game/SetWeapon.cpp). */
namespace boost {
template <class T> inline void shared_ptr<T>::reset() {
    this_type().swap(*this);
}
} // namespace boost

namespace Vehicle {
namespace BirdBase {

class AirBolt : public BirdBase {
  public:
    AirBolt(void *arg);
    virtual ~AirBolt();
    virtual void GetSeat(Vec3f *position, Vec3f *rotation);
    virtual void Setup();
    virtual void OnCommand(CharCommand *command);
    virtual void Tick(float dt);
    virtual void OnGetOn();
    virtual void OnGetOff();
    virtual bool vf_0x50();

    void CreateModel();
    void UpdateWeapon(float dt);
    void UpdateMuzzles();
    void CreateWeapon();
    void ReleaseWeapon();

    /* 0x120 */ BirdFlags m_gunFlags; /* bit 0: locking on */
    /* 0x124 */ UnkMatrix_800B1304 m_muzzles[7]; /* the six muzzles and the sight */
    /* 0x274 */ boost::shared_ptr<Weapon::WeaponLockonBase> m_weapon;
    /* 0x27C */ s32 m_weaponState;
    /* 0x280 */ float m_weaponTimer;
};
SIZE_ASSERT(AirBolt, 0x284);

/* Converts metres to world units (name inferred; see Game/Coaster.cpp). */
inline float MetresToUnits(float metres) {
    return 10.0f * metres;
}

/* The AirBolt's tuning. The static initializer stores the two radii. */
static BirdParam s_param = {
    MetresToUnits(1.5f), MetresToUnits(1.4f), 5.0f, 500.0f, 1900.0f, 0.125f, 2.0f, 150.0f,
    1.0f, 2.0f, 30.0f, 60.0f, 1.0f, 10.0f, 130.0f, 1.0f, 100.0f, 2.0f, 200.0f, 3.0f, 2.0f, 2.0f,
    1.0f,
};

static const char *s_taskName = "AirBolt";

AirBolt::AirBolt(void *arg) : BirdBase(arg, &s_param) {}

AirBolt::~AirBolt() {
    Release();
    if (m_model != 0) {
        delete m_model;
        m_model = 0;
    }
    ReleaseWeapon();
}

void AirBolt::GetSeat(Vec3f *position, Vec3f *rotation) {
    if (m_model != 0) {
        void *frame = m_model->GetFrame(0);
        if (position != 0) {
            fn_800091D0(position, &(const Vec3f &)fn_8040FB08(frame));
        }
        if (rotation != 0) {
            Vec3f up;
            fn_800091EC(&up, &(const Vec3f &)fn_8040B618(
                                 frame, &(const Vec3f &)Vec3f(0.0f, 1.0f, 0.0f)));
            Vec3f forward;
            fn_800091EC(&forward, &(const Vec3f &)fn_8040B618(
                                      frame, &(const Vec3f &)Vec3f(0.0f, 0.0f, -1.0f)));
            *rotation = fn_8040F5F4(&forward, &up, 2);
        }
    }
}

void AirBolt::Setup() {
    fn_801B6570(this, 20.0f);
    SetField98Float(20.0f);
    CreateModel();
    CreateAttack();
    fn_80014E10(&m_flags.m_bits);
    m_speed = 0.0f;
    void *loader = fn_800122D4();
    void *clump = fn_8001277C(loader, 3, 0x53);
    SetClump(clump);
    fn_801B64B8(this, fn_8001277C(loader, 3, 0x54));
}

void AirBolt::OnCommand(CharCommand *command) {
    switch (command->TypeId()) {
    case 0x15:
        switch (static_cast<UnkActionCommand_AirBolt *>(command)->m_action) {
        case 6:
            fn_80014E1C(&m_gunFlags.m_bits, 0);
            command->Flags() |= 2;
            break;
        case 7:
            fn_8006127C(&m_gunFlags.m_bits, 0);
            command->Flags() |= 2;
            break;
        }
        break;
    }
    BirdBase::OnCommand(command);
}

void AirBolt::Tick(float dt) {
    BirdBase::Tick(dt);
    UpdateWeapon(dt);
}

void AirBolt::OnGetOn() {
    BirdBase::OnGetOn();
    CreateWeapon();
    fn_801B63B0(this);
    fn_801B644C(this);
}

void AirBolt::OnGetOff() {
    BirdBase::OnGetOff();
    ReleaseWeapon();
}

bool AirBolt::vf_0x50() {
    return true;
}

void AirBolt::CreateModel() {
    if (m_model != 0) {
        if (m_model != 0) {
            delete m_model;
            m_model = 0;
        }
    }
    m_model = new AirBoltModel(this);
    m_model->SetState(0);
    if (field_0x74 != 0) {
        fn_8034F250((void *)field_0x74, m_model->GetFrame(0));
    }
}

void AirBolt::UpdateWeapon(float dt) {
    if (m_weapon.get() != 0) {
        switch (m_weaponState) {
        case 0:
            if (m_weaponTimer > 0.0f) {
                m_weaponTimer -= dt;
                if (m_weaponTimer <= 0.0f) {
                    m_weaponTimer = 0.0f;
                }
            } else if (fn_80014E34(&m_gunFlags.m_bits, 0)) {
                fn_8005FA68(m_weapon.get());
                m_weaponState++;
            }
            break;
        case 1:
            if (!fn_80014E34(&m_gunFlags.m_bits, 0)) {
                fn_8005FA38(m_weapon.get());
                m_weaponTimer = fn_8005FA98(m_weapon.get());
                m_weaponState++;
            }
            break;
        case 2:
            if (m_weapon->fn_8005F9C8() <= 0) {
                m_weaponState = 100;
            }
            break;
        case 100:
            m_weaponState = 0;
            break;
        }
        if (fn_8005F9A0(m_weapon.get()) > 0 && !GetRider().expired()) {
            fn_8034DF48(2, GetRider().lock()->field_0x54);
        }
        UpdateMuzzles();
        m_weapon->Tick(dt);
    }
}

void AirBolt::UpdateMuzzles() {
    Vec3f rotation(0.0f, 0.0f, 0.0f);
    fn_801B65DC(this, 0, &rotation);
    Vec3f position;
    fn_800091EC(&position, &(const Vec3f &)fn_8040FB08(m_model->GetFrame(0)));
    fn_8040C148(&m_muzzles[6],
                &(const Mtx34f &)fn_8040FFBC(&(const Mtx34f &)((TranslationFn)fn_8040B034)(&position),
                                             &rotation, 2));
    fn_8040B848(&m_muzzles[6], 180.0f * (lbl_805FAD38 / 180.0f));
    if (m_weapon.get() != 0 && m_weapon->fn_8005F9C8() <= 0) {
        for (int i = 0; i < 6; i++) {
            fn_8040C148(&m_muzzles[i], &m_muzzles[6]);
        }
    } else {
        for (int i = 0; i < 6; i++) {
            fn_8040C148(&m_muzzles[i], (const Mtx34f *)m_model->GetFrame(i + 1));
        }
    }
}

void AirBolt::CreateWeapon() {
    m_weapon = Weapon::AirBolt::AirBolt::Create(GetShooter(), m_muzzles);
    m_weaponState = 0;
    m_weaponTimer = 0.0f;
}

void AirBolt::ReleaseWeapon() {
    if (m_weapon.get() != 0) {
        m_weapon.reset();
    }
}

} // namespace BirdBase
} // namespace Vehicle

extern "C" void fn_802CC310(void) {
    fn_803A953C("%s::Initialize\n", Vehicle::BirdBase::s_taskName);
    fn_801B4850(9);
    Vehicle::AirBoltModel::Load();
    fn_8006B50C(0x53, 0);
    fn_8006B50C(0x54, 0);
}

extern "C" void fn_802CC2E0(void) {
    fn_803A953C("%s::Finalize\n", Vehicle::BirdBase::s_taskName);
}

static Vehicle::TVehicleBase *Create(void *arg) {
    return new Vehicle::BirdBase::AirBolt(arg);
}

/* The task record (see Game/GunLift.cpp) and the vehicle table's entry (it refers to them by
 * address); the static initializer stores their names. */
GadgetClassDesc lbl_80557FB4 = {
    Vehicle::BirdBase::s_taskName, fn_802CC310, fn_802CC2E0, 0, 0, 0x80, 0x004E, 4, 0, 0, 0,
};

UnkVehicleEntry lbl_805EC0E4 = {Vehicle::BirdBase::s_taskName, Create};
