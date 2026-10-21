/* Rocket::RocketBase (class name from RTTI): the ROCKET gadget, a launch base (ROCKET.BIN, part
 * "base") that holds a Rocket::Rocket (class name from the RTTI of its shared_ptr control block)
 * aimed by the "angle" parameter. A character that touches the rocket hangs from it; using it
 * hands the rocket to a Rocket::RocketTask (class name from RTTI), which flies it and the
 * character along the aim until it is "distance" away, then lets go. The base then builds a new
 * rocket. File name from the namespace. Names marked "(name inferred)" are not original.
 * Definitions are in reverse address order (deferred inlining emits functions last-defined
 * first). */
#include <boost/bind.hpp>
#include <boost/function.hpp>

#include "Game/CharCommand.h"
#include "Game/GadgetCharCommands.h"
#include "Game/CharCommandUnit.h"
#include "Game/CharInfo.h"
#include "Game/Gadget.h"
#include "Game/GadgetCharInfo.h"
#include "Game/GadgetCollision.h"
#include "Game/GadgetModelInfo.h"
#include "Game/GadgetParticle.h"
#include "Game/GadgetClassDesc.h"
#include "Game/GadgetUtil.h"
#include "Game/Task.h"
#include "Game/Vibration.h"
#include "Game/FileQueue.h"
#include "boost/shared_ptr.hpp"
#include "boost/weak_ptr.hpp"
#include "size_assert.h"

/* The RenderWare types used here, declared locally (layouts as in rw/rwcore.h, reduced to the
 * members used; as in SecretDoor.cpp). */
struct RwV3d {
    float x;
    float y;
    float z;
};

struct RwMatrix {
    /* 0x00 */ RwV3d right;
    /* 0x0C */ u32 flags;
    /* 0x10 */ RwV3d up;
    /* 0x1C */ u32 pad1;
    /* 0x20 */ RwV3d at;
    /* 0x2C */ u32 pad2;
    /* 0x30 */ RwV3d pos;
    /* 0x3C */ u32 pad3;
};
SIZE_ASSERT(RwMatrix, 0x40);

struct RwFrame {
    /* 0x00 */ u8 object[0x8];
    /* 0x08 */ u8 inDirtyListLink[0x8];
    /* 0x10 */ RwMatrix modelling;
    /* 0x50 */ RwMatrix ltm;
};

/* What fn_8015FA78 passes to fn_80169E8C: the id of the model frame to find, and the frame found
 * (as in TurnOver.cpp). */
struct UnkFrameSearch_80169E8C {
    UnkFrameSearch_80169E8C() : id(0), frame(0) {}

    /* 0x0 */ u32 id;
    /* 0x4 */ RwFrame *frame;
};
SIZE_ASSERT(UnkFrameSearch_80169E8C, 0x8);

extern "C" {
RwMatrix *RwFrameGetLTM(RwFrame *frame);
/* Turns a frame about its x axis (behaviour inferred; the last argument is the combine mode), and
 * applies a matrix to a frame. */
void fn_800E9C64(RwFrame *frame, float angle, int combine);
RwFrame *fn_804876E8(RwFrame *frame, const RwMatrix *matrix, int combine);

/* A 32-bit flag word: fn_80014E04 constructs it cleared, fn_80014E1C sets bit, fn_8006127C clears
 * bit and fn_80014E34 tests bit (behaviour from their bodies). */
void fn_80014E04(u32 *flags);
void fn_80014E1C(u32 *flags, int bit);
void fn_8006127C(u32 *flags, int bit);
bool fn_80014E34(u32 *flags, int bit);

/* Gives the character info a command handler unless it has one (behaviour inferred). */
void fn_8015CF7C(GadgetCharInfo *charInfo);
/* Loads the model fileName (behaviour inferred; arg0 unknown). */
boost::shared_ptr<GadgetModelInfo> fn_8015F800(int arg0, const char *fileName);
/* Shows the model's part name (behaviour inferred; arg2 unknown). */
void fn_801629D8(GadgetModelInfo *model, const char *name, int arg2);
/* Calls callback with each part of the model; fn_80169E8C finds a frame by id, fn_80169D00
 * attaches a particle controller (behaviour inferred). */
void fn_8015FA78(GadgetModelInfo *model, void *(*callback)(void *part, void *data), void *data);
void *fn_80169E8C(void *part, void *data);
void *fn_80169D00(void *part, void *data);
/* Calls callback with every character in the collision event (behaviour inferred). */
void fn_8015AE44(ColliDetectArgs &args, void (*callback)(boost::shared_ptr<CharInfo> hit, void *user),
                 void *user);
/* Particle controller helpers (behaviour inferred): create one, advance it by dt, start its
 * emitters. */
boost::shared_ptr<GadgetParticle::GadgetParticleCtrl> fn_80166AF4(void);
void fn_8016761C(GadgetParticle::GadgetParticleCtrl *ctrl, float dt);
void fn_80167010(GadgetParticle::GadgetParticleCtrl *ctrl, int id);
/* Rumbles the controller of the player behind target, and stops a rumble (behaviour
 * inferred). */
boost::weak_ptr<Vibration::RequestHandle> fn_80169B4C(const boost::weak_ptr<CharInfo> &target,
                                                      int pattern);
void fn_8034DB60(boost::weak_ptr<Vibration::RequestHandle> *handle);
/* Lets the hanging character go at position and rotation (behaviour inferred). */
void fn_80292A64(boost::weak_ptr<CharInfo> *character, const Vec3f *position,
                 const Vec3f *rotation);
/* Plays sound id (behaviour inferred). */
void fn_801D560C(void *manager, int id, int arg2, int arg3);
/* matrix = identity, matrix * point, vector += value (behaviour inferred). */
void fn_8040B6F8(Mtx34f *matrix);
Vec3f fn_8040B67C(const Mtx34f *matrix, const Vec3f *point);
void fn_8040CFF8(Vec3f *vector, const Vec3f *value);
/* Draws a debug arc at position (behaviour inferred; the arguments are unknown). */
void fn_802E0C60(const Vec3f *position, const Vec3f *rotation, int arg2,
                 float radius, float arg4);
}

/* A constant fn_802E0C60 is passed (meaning unknown). */
extern float lbl_805F8560;

/* vector += value (name inferred). */
inline void AddVector(Vec3f &vector, const Vec3f &value) {
    fn_8040CFF8(&vector, &value);
}

/* Copies a rotation one angle at a time (name inferred). */
inline void CopyRotation(Vec3f &rotation, const Vec3f &value) {
    rotation.x = value.x;
    rotation.y = value.y;
    rotation.z = value.z;
}

/* result = a - b (name inferred). Retail's a - b returns its result through the caller's
 * temporary, which this reproduces. */
inline const Vec3f &Subtract(Vec3f *result, const Vec3f *a,
                                     const Vec3f *b) {
    fn_8040CD74(result, a, b);
    return *result;
}

/* Pi, and degrees to radians (as in TargetSwitch; retail calls the shared out-of-line copy of the
 * conversion at 0x8000AB90). */
inline float Pi() {
    return lbl_805FAD38;
}

inline float DegToRad(float degrees) {
    return degrees * (Pi() / 180.0f);
}

/* The 0x18 bytes fn_80020A20 builds a debug line in; retail passes a temporary (name inferred). */
struct UnkSegment {
    UnkSegment() {}

    u8 m_data[0x18];
};

namespace Rocket {
class Rocket;
} // namespace Rocket

/* boost/shared_ptr.hpp only declares shared_ptr::reset, which the game has out of line; this unit
 * emits the copy for Rocket (0x80114A34; boost 1.32's definition). */
namespace boost {
template<> inline void shared_ptr<Rocket::Rocket>::reset() {
    this_type().swap(*this);
}
} // namespace boost

namespace Rocket {

/* A 32-bit flag word (the class name is unknown; its out-of-line members are the fn_80014E04
 * family above). */
struct RocketFlags {
    RocketFlags() {
        fn_80014E04(&m_bits);
    }
    void Set(int bit) {
        fn_80014E1C(&m_bits, bit);
    }
    void Reset(int bit) {
        fn_8006127C(&m_bits, bit);
    }
    bool Test(int bit) {
        return fn_80014E34(&m_bits, bit);
    }

    u32 m_bits;
};

/* What the collision callback passes to FindRider: the first character found (name inferred). */
struct RiderSearch {
    RiderSearch() : m_found(false) {}

    /* 0x0 */ boost::weak_ptr<CharInfo> m_character;
    /* 0x8 */ bool m_found;
};

/* The rocket (class name from RTTI; names inferred). Flag bit 0: a character used it; bit 1: a
 * character touched it this frame (the hang command was sent); bit 2: done. */
class Rocket {
  public:
    Rocket(const boost::weak_ptr<CharInfo> &base);
    ~Rocket();

    /* Method names are inferred. */
    void Tick(float dt);
    void UpdateModel();
    void SetPlacement(const Vec3f *position, Vec3f *rotation);
    void SetVelocity(const Vec3f *velocity);
    void OnCommand(CharCommand &command);
    /* Moves the rocket by its velocity; true once it is out of range. */
    bool Move();
    void OnColliDetect(ColliDetectArgs &args);

    /* Whether the rocket is done (flag bit 2). */
    bool IsDone() {
        return fn_80014E34(&m_flags.m_bits, 2);
    }

    /* Sets the range (stored squared). */
    void SetRange(float range) {
        m_rangeSq = range * range;
    }

    /* Member names are inferred. */
    /* 0x00 */ boost::shared_ptr<GadgetCharInfo> m_chara;
    /* 0x08 */ boost::shared_ptr<GadgetModelInfo> m_model;
    /* 0x10 */ boost::weak_ptr<CharInfo> m_base;
    /* 0x18 */ boost::weak_ptr<CharInfo> m_rider;
    /* 0x20 */ boost::shared_ptr<GadgetParticle::GadgetParticleCtrl> m_particle;
    /* 0x28 */ Vec3f m_start;
    /* 0x34 */ Vec3f m_hangPosition;
    /* 0x40 */ Vec3f m_hangRotation;
    /* 0x4C */ Vec3f m_velocity;
    /* 0x58 */ float m_rangeSq;
    /* 0x5C */ float m_timer;
    /* 0x60 */ s32 m_state; /* 0 waiting, 1 used, 2 flying, 3 arrived, 4 done */
    /* 0x64 */ RocketFlags m_flags;
    /* 0x68 */ boost::weak_ptr<Vibration::RequestHandle> m_vibration;
};
SIZE_ASSERT(Rocket, 0x70);

/* Deletes a rocket (name inferred; see checked_delete below). */
inline void DeleteRocket(Rocket *rocket) {
    delete rocket;
}

} // namespace Rocket

/* Retail keeps boost::checked_delete<Rocket> out of line (0x80115104) and ~Rocket at its own place
 * (0x801163EC). GC/2.7 only keeps checked_delete out of line when its body calls an inline
 * function, and emits an inline ~Rocket right after it (see docs/research-notes.md, EnergyCore);
 * this specialization calls an inline helper instead, which gives retail's code and order. */
namespace boost {
template<> inline void checked_delete<Rocket::Rocket>(Rocket::Rocket *x) {
    Rocket::DeleteRocket(x);
}
} // namespace boost

namespace Rocket {

/* The task that flies a used rocket (class name from RTTI; names inferred). */
class RocketTask : public Task {
  public:
    RocketTask(Task *parent, boost::shared_ptr<Rocket> rocket);
    virtual ~RocketTask();
    virtual void Tick(float dt);

    static const char *s_taskName;

    /* 0x28 */ boost::shared_ptr<Rocket> m_rocket;
};
SIZE_ASSERT(RocketTask, 0x30);

/* The parameters the owner supplies (names from the parameter table). */
struct RocketParams {
    RocketParams() : angle(0.0f), distance(0.0f) {}

    /* 0x0 */ float angle; /* degrees */
    /* 0x4 */ float distance;
};

class RocketBase : public Gadget {
  public:
    RocketBase(Task *parent, UnkPlacement_800C9CD0 *owner);
    virtual ~RocketBase();
    virtual void Behave(float dt);
    virtual void ReloadPlacement(UnkPlacement_800C9CD0 *owner);

    /* Creates the gadget under task layer 11 (name inferred). */
    static void Instantiate(UnkPlacement_800C9CD0 *owner);

    /* Method names are inferred. */
    void LoadFromOwner(UnkPlacement_800C9CD0 *owner);
    void UpdateModel();
    void OnCommand(CharCommand &command);

    static const char *s_taskName;

    /* Member names are inferred. */
    /* 0x30 */ boost::shared_ptr<GadgetCharInfo> m_chara;
    /* 0x38 */ boost::shared_ptr<GadgetModelInfo> m_model;
    /* 0x40 */ boost::shared_ptr<Rocket> m_rocket;
    /* 0x48 */ RocketParams m_params;
    /* 0x50 */ RwMatrix *m_launchMatrix; /* of frame 0x7D3 */
    /* 0x54 */ RwMatrix m_armMatrix;     /* frame 0x7D0's own matrix */
    /* 0x94 */ RwFrame *m_arm;           /* frame 0x7D0 */
    /* 0x98 */ s32 m_state;              /* 0 no rocket, 1 ready, 2 launched */
    /* 0x9C */ float field_0x9C;
};
SIZE_ASSERT(RocketBase, 0xA0);

const char *RocketTask::s_taskName = "RocketTask";
const char *RocketBase::s_taskName = "RocketBase";

/* Remembers the first character of a collision event (name inferred). */
static void FindRider(boost::shared_ptr<CharInfo> hit, void *user) {
    RiderSearch *search = static_cast<RiderSearch *>(user);
    if (search != 0 && !search->m_found && hit.get() != 0) {
        search->m_character = hit;
        search->m_found = true;
    }
}

Rocket::Rocket(const boost::weak_ptr<CharInfo> &base) : m_hangRotation(0.0f, 0.0f, 0.0f) {
    m_base = base;
    m_state = 0;
    m_timer = 0.0f;
    m_chara = fn_8015CB88(0x1E, 4, "Rocket");
    fn_8015CF7C(m_chara.get());
    fn_800729B0(&m_chara->m_commandUnit->m_callback,
                &(const CharCommandCallback &)CharCommandCallback(
                    boost::bind(&Rocket::OnCommand, this, _1)));
    if (boost::shared_ptr<CharInfo> owner = m_base.lock()) {
        fn_800091D0(&m_chara->m_position, &owner->m_position);
        CopyRotation(m_chara->m_rotation, owner->m_rotation);
    }
    m_model = fn_8015F800(0, "ROCKET.BIN");
    fn_801629D8(m_model.get(), "rocket", 0);
    m_model->fn_80161F08("area", m_chara);
    m_model->fn_80161F08("rocket", m_chara);
    fn_800D0B64(m_model->field_0x4,
                boost::function1<void, ColliDetectArgs &>(
                    boost::bind(&Rocket::OnColliDetect, this, _1)));
    UpdateModel();
}

Rocket::~Rocket() {
    fn_8034DB60(&m_vibration);
}

void Rocket::Tick(float dt) {
    m_flags.Reset(1);
    if (m_model.get() != 0) {
        m_model->fn_80162FB0(dt);
    }
    if (m_particle.get() != 0) {
        fn_8016761C(m_particle.get(), dt);
    }

    switch (m_state) {
    case 0:
        break;
    case 1:
        if (boost::shared_ptr<CharInfo> rider = m_rider.lock()) {
            rider->PushCommand(new Player::TouchHangCommand(m_chara, &m_hangPosition,
                                                            &m_hangRotation, 4, 4));
        }
        m_timer -= dt;
        if (m_timer < 0.0f) {
            m_model->fn_80161880();
            m_state = 2;
            m_particle = fn_80166AF4();
            {
                UnkParticleAttach_80169D00 attach;
                attach.ctrl = m_particle;
                fn_8015FA78(m_model.get(), fn_80169D00, &attach);
                fn_80167010(m_particle.get(), -1);
            }
            if (!m_vibration.expired()) {
                fn_8034DB60(&m_vibration);
            }
            m_vibration = fn_80169B4C(m_rider, 0xB);
            fn_801D560C(fn_80053164(), 0x100A, 0, 0);
        }
        break;
    case 2:
        if (Move()) {
            fn_8034DB60(&m_vibration);
            m_state = 3;
            m_timer = 5.0f;
            UpdateModel();
            fn_80292A64(&m_rider, &m_hangPosition, &m_hangRotation);
            if (boost::shared_ptr<CharInfo> base = m_base.lock()) {
                base->PushCommand(new ReadyCommand(m_chara));
            }
        } else {
            if (boost::shared_ptr<CharInfo> rider = m_rider.lock()) {
                rider->PushCommand(new Player::TouchHangCommand(m_chara, &m_hangPosition,
                                                                &m_hangRotation, 4, 4));
            }
        }
        break;
    case 3:
        Move();
        m_timer -= dt;
        if (m_timer < 0.0f) {
            m_state = 4;
        }
        break;
    case 4:
        m_flags.Set(2);
        break;
    }
    UpdateModel();
}

void Rocket::UpdateModel() {
    if (m_model.get() != 0) {
        Mtx34f matrix;

        fn_8040C148(&matrix, &lbl_805E0720);
        fn_8040B59C(&matrix, &m_chara->m_position);
        fn_8040B848(&matrix, m_chara->m_rotation.y);
        fn_8040B884(&matrix, m_chara->m_rotation.x);
        m_model->fn_801633DC(&matrix);
        Vec3f offset(0.0f, -8.0f, 1.0f);
        fn_800091D0(&m_hangPosition, &(const Vec3f &)fn_8040B67C(&matrix, &offset));
        m_hangRotation.x = 0.0f;
        m_hangRotation.y = m_chara->m_rotation.y;
        m_hangRotation.z = 0.0f;
    }
}

void Rocket::SetPlacement(const Vec3f *position, Vec3f *rotation) {
    fn_800091D0(&m_chara->m_position, position);
    CopyRotation(m_chara->m_rotation, *rotation);
    fn_800091D0(&m_start, &m_chara->m_position);
    UpdateModel();
}

void Rocket::SetVelocity(const Vec3f *velocity) {
    fn_800091D0(&m_velocity, velocity);
}

void Rocket::OnCommand(CharCommand &command) {
    switch (command.TypeId()) {
    case 0xB:
        if (!m_flags.Test(0)) {
            m_flags.Set(0);
            if (boost::shared_ptr<CharInfo> base = m_base.lock()) {
                base->PushCommand(new UseCommand(m_chara));
            }
        }
        break;
    case 0xC:
        break;
    case 0xD:
        if (m_flags.Test(0) && m_state == 0) {
            m_state = 1;
            m_timer = 0.5f;
        }
        break;
    }
}

bool Rocket::Move() {
    AddVector(m_chara->m_position, m_velocity);
    Vec3f offset;
    fn_800091EC(&offset, &Subtract(&Vec3f(), &m_chara.px->m_position, &m_start));
    if (fn_8040CF2C(&offset) > m_rangeSq) {
        return true;
    }
    return false;
}

void Rocket::OnColliDetect(ColliDetectArgs &args) {
    if (m_state == 0 && !m_flags.Test(1)) {
        RiderSearch search;
        fn_8015AE44(args, FindRider, &search);
        if (search.m_found) {
            if (boost::shared_ptr<CharInfo> rider = search.m_character.lock()) {
                rider->PushCommand(new Player::TouchHangCommand(m_chara, &m_hangPosition,
                                                                &m_hangRotation, 4, 4));
                m_rider = search.m_character;
                m_flags.Set(1);
            }
        }
    }
}

RocketTask::RocketTask(Task *parent, boost::shared_ptr<Rocket> rocket) : Task(parent) {
    m_name = s_taskName;
    m_rocket = rocket;
}

RocketTask::~RocketTask() {}

void RocketTask::Tick(float dt) {
    m_rocket->Tick(dt);
    if (m_rocket->IsDone()) {
        m_flags |= 1;
    }
}

RocketBase::RocketBase(Task *parent, UnkPlacement_800C9CD0 *owner) : Gadget(parent, owner) {
    m_name = s_taskName;
    m_chara = fn_8015CB88(0x1E, 4, "RocketBase");
    fn_8015CF7C(m_chara.get());
    fn_800729B0(&m_chara->m_commandUnit->m_callback,
                &(const CharCommandCallback &)CharCommandCallback(
                    boost::bind(&RocketBase::OnCommand, this, _1)));
    LoadFromOwner(owner);
    m_model = fn_8015F800(0, "ROCKET.BIN");
    fn_801629D8(m_model.get(), "base", 0);
    m_model->fn_80161F08("base", m_chara);
    {
        UnkFrameSearch_80169E8C search;
        search.id = 0x7D3;
        fn_8015FA78(m_model.get(), fn_80169E8C, &search);
        if (search.frame != 0) {
            m_launchMatrix = RwFrameGetLTM(search.frame);
        }
    }
    {
        UnkFrameSearch_80169E8C search;
        search.id = 0x7D0;
        fn_8015FA78(m_model.get(), fn_80169E8C, &search);
        if (search.frame != 0) {
            m_arm = search.frame;
            m_armMatrix = m_arm->modelling;
        }
    }
    UpdateModel();
}

RocketBase::~RocketBase() {}

void RocketBase::Behave(float dt) {
    if (m_model.get() != 0) {
        m_model->fn_80162FB0(dt);
    }
    if (m_rocket.get() != 0) {
        m_rocket->Tick(dt);
    }
    UpdateModel();

    switch (m_state) {
    case 0:
        m_rocket = boost::shared_ptr<Rocket>(new Rocket(m_chara));
        m_state = 1;
        break;
    case 1:
        break;
    case 2:
        break;
    }
}

void RocketBase::ReloadPlacement(UnkPlacement_800C9CD0 *owner) {
    LoadFromOwner(owner);
    UpdateModel();
    if (m_model.get() != 0) {
        m_model->fn_8016108C();

        Vec3f start(0.0f, 0.0f, 0.0f);
        Vec3f end(0.0f, m_params.distance, 0.0f);
        Vec3f position(m_launchMatrix->pos.x, m_launchMatrix->pos.y,
                               m_launchMatrix->pos.z);
        Mtx34f matrix;
        fn_8040C148(&matrix, &lbl_805E0720);
        fn_8040B59C(&matrix, &position);
        fn_8040B848(&matrix, m_chara->m_rotation.y);
        fn_8040B884(&matrix, -m_params.angle * (lbl_805FAD38 / 180.0f));
        AssignVector(start, fn_8040B67C(&matrix, &start));
        AssignVector(end, fn_8040B67C(&matrix, &end));
        fn_802E0B58(fn_80020A20(&UnkSegment(), &start, &end), &lbl_805F06D8);
        Vec3f arcRotation(DegToRad(90.0f), 0.0f, 0.0f);
        fn_802E0C60(&end, &arcRotation, 0, m_params.distance, lbl_805F8560);
    }
}

void RocketBase::LoadFromOwner(UnkPlacement_800C9CD0 *owner) {
    fn_800C9F14(owner, &m_chara->m_position);
    fn_800C9ED4(owner, &m_chara->m_rotation);
    fn_800C9DBC(owner, &m_params, sizeof(m_params));
    m_state = 0;
    field_0x9C = 0.0f;
}

void RocketBase::UpdateModel() {
    if (m_model.get() != 0) {
        Mtx34f matrix;

        fn_8040C148(&matrix, &lbl_805E0720);
        fn_8040B59C(&matrix, &m_chara->m_position);
        fn_8040B848(&matrix, m_chara->m_rotation.y);
        m_model->fn_801633DC(&matrix);
    }
    if (m_arm != 0) {
        fn_800E9C64(m_arm, -m_params.angle * (lbl_805FAD38 / 180.0f), 0);
        fn_804876E8(m_arm, &m_armMatrix, 2);
    }
    if (m_launchMatrix != 0 && m_rocket.get() != 0) {
        Mtx34f matrix;
        Vec3f position(m_launchMatrix->pos.x, m_launchMatrix->pos.y,
                               m_launchMatrix->pos.z);
        Vec3f rotation(0.0f, 0.0f, 0.0f);
        rotation.x = -m_params.angle * (lbl_805FAD38 / 180.0f);
        rotation.y = m_chara->m_rotation.y;
        rotation.z = 0.0f;
        Vec3f direction(0.0f, 1.0f, 0.0f);
        fn_8040B6F8(&matrix);
        fn_8040B848(&matrix, m_chara->m_rotation.y);
        fn_8040B884(&matrix, -m_params.angle * (lbl_805FAD38 / 180.0f));
        Vec3f velocity;
        fn_800091EC(&velocity, &(const Vec3f &)fn_8040B67C(&matrix, &direction));
        fn_8040CE38(&velocity, 14.0f);
        m_rocket->SetPlacement(&position, &rotation);
        m_rocket->SetVelocity(&velocity);
        m_rocket->SetRange(m_params.distance);
    }
}

void RocketBase::OnCommand(CharCommand &command) {
    switch (command.TypeId()) {
    case 0xD:
        if (m_state == 1) {
            Task *parent = fn_801EB304(fn_8001C4F8(), 11);
            new RocketTask(parent, m_rocket);
            m_rocket.reset();
            m_state = 2;
        }
        break;
    case 0xE:
        if (m_state == 2) {
            m_state = 0;
        }
        break;
    }
}

extern "C" void fn_80114938(void) {
    fn_8006B474("ROCKET.BIN", 1);
}

extern "C" void fn_80114934(void) {}

void RocketBase::Instantiate(UnkPlacement_800C9CD0 *owner) {
    Task *parent = fn_801EB304(fn_8001C4F8(), 11);
    new RocketBase(parent, owner);
}

static GadgetParamDesc lbl_80526CD4[] = {
    {4, "angle", 0, 0, 0, 0.0f, 0.0f, 360.0f, fn_800CBC20, 0},
    {4, "distance", 0, 0, 0, 10.0f, 0.0f, 10000.0f, fn_800CBC20, 0},
    {0},
};

} // namespace Rocket

GadgetClassDesc lbl_80526D4C = {
    "ROCKET", Rocket::fn_80114938, Rocket::fn_80114934, Rocket::RocketBase::Instantiate, 0, 0, 0xE, 1, 0,
    10, Rocket::lbl_80526CD4,
};
