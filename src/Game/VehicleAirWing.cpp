/* Vehicle::BirdBase::AirWing (class name from RTTI): the AirWing, a flying vehicle (a
 * Vehicle::BirdBase::BirdBase) without a gun: it only sets up its model (Vehicle::AirWingModel),
 * its body attack and its clumps, and leaves the rest to BirdBase. The vehicle table creates it
 * through lbl_805E8464. File name from the class (as Game/VehicleAirBolt.cpp); member names are
 * inferred. Definitions are in reverse address order (deferred inlining emits functions
 * last-defined first). */
#include "Game/AirWingModel.h"
#include "Game/BirdBase.h"
#include "Game/CharCommand.h"
#include "Game/GadgetClassDesc.h"
#include "Game/VehicleEffect.h"
#include "size_assert.h"

/* A vehicle kind's entry in the vehicle table (see Game/GunLift.cpp). */
struct UnkVehicleEntry {
    const char *name;
    Vehicle::TVehicleBase *(*create)(void *arg);
};

extern "C" {
/* The 32-bit flag word (see Game/BAMine.cpp). */
void fn_80014E10(u32 *flags);

/* Debug output and resource loading (see Game/WeaponDarkHammer.cpp), and the vehicle type in use
 * (see Game/GunLift.cpp). */
void fn_803A953C(const char *format, ...);
void fn_8006B50C(int id, int arg1);
void fn_801B4850(int type);

/* TVehicleBase's other functions (see Game/VehicleAirBolt.cpp): the vehicle's life and a second
 * model clump. */
void fn_801B6570(Vehicle::TVehicleBase *vehicle, float value);
void fn_801B64B8(Vehicle::TVehicleBase *vehicle, void *clump);

/* The resource loader and a model clump in it (see Game/GunLift.cpp). */
void *fn_800122D4(void);
void *fn_8001277C(void *loader, int arg1, int id);

/* Matrix helpers (see Game/VehicleAirBolt.cpp). */
Vec3f fn_8040FB08(const void *matrix);                         /* its translation */
Vec3f fn_8040B618(const void *matrix, const Vec3f *direction); /* a direction turned by it */
Vec3f fn_8040F5F4(const Vec3f *forward, const Vec3f *up, int order); /* Euler angles */
}

namespace Vehicle {
namespace BirdBase {

class AirWing : public BirdBase {
  public:
    AirWing(void *arg);
    virtual ~AirWing();
    virtual void GetSeat(Vec3f *position, Vec3f *rotation);
    virtual void Setup();
    virtual void OnCommand(CharCommand *command);

    void CreateModel();
};
SIZE_ASSERT(AirWing, 0x120);

/* Converts metres to world units (name inferred; see Game/Coaster.cpp). */
inline float MetresToUnits(float metres) {
    return 10.0f * metres;
}

/* The AirWing's tuning (the same as the AirBolt's). The static initializer stores the two radii. */
static BirdParam s_param = {
    MetresToUnits(1.5f), MetresToUnits(1.4f), 5.0f, 500.0f, 1900.0f, 0.125f, 2.0f, 150.0f,
    1.0f, 2.0f, 30.0f, 60.0f, 1.0f, 10.0f, 130.0f, 1.0f, 100.0f, 2.0f, 200.0f, 3.0f, 2.0f, 2.0f,
    1.0f,
};

static const char *s_taskName = "AirWing";

/* Inline in the game: the vehicle table's create function has it. */
inline AirWing::AirWing(void *arg) : BirdBase(arg, &s_param) {}

AirWing::~AirWing() {
    Release();
    if (m_model != 0) {
        delete m_model;
        m_model = 0;
    }
}

void AirWing::GetSeat(Vec3f *position, Vec3f *rotation) {
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

void AirWing::Setup() {
    fn_801B6570(this, 14.0f);
    SetField98Float(14.0f);
    CreateModel();
    CreateAttack();
    fn_80014E10(&m_flags.m_bits);
    m_speed = 0.0f;
    void *loader = fn_800122D4();
    void *clump = fn_8001277C(loader, 3, 0x53);
    SetClump(clump);
    fn_801B64B8(this, fn_8001277C(loader, 3, 0x54));
}

void AirWing::OnCommand(CharCommand *command) {
    BirdBase::OnCommand(command);
}

void AirWing::CreateModel() {
    if (m_model != 0) {
        if (m_model != 0) {
            delete m_model;
            m_model = 0;
        }
    }
    m_model = new AirWingModel(this);
    m_model->SetState(0);
    if (field_0x74 != 0) {
        reinterpret_cast<Vehicle::Effect *>(field_0x74)->SetNode(m_model->GetFrame(0));
    }
}

} // namespace BirdBase
} // namespace Vehicle

extern "C" void fn_801B2D28(void) {
    fn_803A953C("%s::Initialize\n", Vehicle::BirdBase::s_taskName);
    fn_801B4850(8);
    Vehicle::AirWingModel::Load();
    fn_8006B50C(0x53, 0);
    fn_8006B50C(0x54, 0);
}

extern "C" void fn_801B2CF8(void) {
    fn_803A953C("%s::Finalize\n", Vehicle::BirdBase::s_taskName);
}

static Vehicle::TVehicleBase *Create(void *arg) {
    return new Vehicle::BirdBase::AirWing(arg);
}

/* The task record (see Game/GunLift.cpp) and the vehicle table's entry (it refers to them by
 * address); the static initializer stores their names. */
GadgetClassDesc lbl_8053B644 = {
    Vehicle::BirdBase::s_taskName, fn_801B2D28, fn_801B2CF8, 0, 0, 0x80, 0x004D, 4, 0, 0, 0,
};

UnkVehicleEntry lbl_805E8464 = {Vehicle::BirdBase::s_taskName, Create};
