/* Parachute::Parachute (class name from RTTI): the parachute a character opens when the ROCKET
 * gadget lets go of it (Game/Rocket.cpp calls fn_80292A64). A task under layer 11 with a
 * character info ("Parachute") and the ROCKET.BIN model: it keeps the character hanging from it
 * while it sinks and drifts (pushed by commands of type 1) within 200 units of where it opened,
 * until it lands or the character leaves it (command 0xC), then cancels the character's behaviour
 * and ends. File name inferred from the class; names marked "(name inferred)" and all member
 * names are not original. Definitions are in reverse address order (deferred inlining emits
 * functions last-defined first). */
#include <boost/bind.hpp>
#include <boost/function.hpp>

#include "Game/CharCommand.h"
#include "Game/CharCommandUnit.h"
#include "Game/CharInfo.h"
#include "Game/GadgetCharCommands.h"
#include "Game/GadgetCharInfo.h"
#include "Game/GadgetModelInfo.h"
#include "Game/GadgetUtil.h"
#include "Game/MathTypes.h"
#include "Game/Task.h"
#include "boost/shared_ptr.hpp"
#include "boost/weak_ptr.hpp"
#include "size_assert.h"

/* What a probe hit belongs to (struct name unknown): its type at 0x4 (meaning inferred; read
 * through an accessor, which keeps retail's extra zero extension). */
struct UnkCollisionOwner_80021E24 {
    /* 0x0 */ u8 field_0x0[0x4];
    /* 0x4 */ u16 m_type;

    u16 GetType() const {
        return m_type;
    }
};

/* A probe hit (layout as in ContainerBehavior.cpp). */
struct UnkCollision_80021E24 {
    /* 0x00 */ u8 field_0x0[0x8];
    /* 0x08 */ UnkCollisionOwner_80021E24 *m_owner;
    /* 0x0C */ u32 field_0xC;
    /* 0x10 */ u32 field_0x10;
    /* 0x14 */ void *m_object;
};

/* The command of type 1 (class name unknown): a push along the vector at 0x14 (meaning
 * inferred). */
struct UnkPushCommand {
    /* 0x00 */ u8 field_0x0[0x14];
    /* 0x14 */ Vec3f m_direction;
};

extern "C" {
/* The task layers (see Gadget.h). */
void *fn_8001C4F8(void);
Task *fn_801EB304(void *root, int layer);

/* A 32-bit flag word: fn_80014E04 constructs it cleared, fn_80014E10 clears it, fn_80014E1C sets
 * bit, fn_8006127C clears bit and fn_80014E34 tests bit (as in Rocket.cpp). */
void fn_80014E04(u32 *flags);
void fn_80014E10(u32 *flags);
void fn_80014E1C(u32 *flags, int bit);
void fn_8006127C(u32 *flags, int bit);
bool fn_80014E34(u32 *flags, int bit);

/* Gives the character info a command handler unless it has one, and its placement matrix (see
 * Rocket.cpp and GadgetCharInfo users). */
void fn_8015CF7C(GadgetCharInfo *charInfo);
void fn_8015CEA0(GadgetCharInfo *charInfo, Mtx34f *matrix);
/* Loads the model fileName, and whether its motion index ended (behaviour inferred). */
boost::shared_ptr<GadgetModelInfo> fn_8015F800(int arg0, const char *fileName);
bool fn_80160040(GadgetModelInfo *model, int index);

/* Moves position by velocity and velocity by acceleration (see ContainerHard.cpp). */
void fn_804103E8(Vec3f *position, Vec3f *velocity, const Vec3f *acceleration, float scale,
                 float dt);
/* The collision manager, and a sweep test from start along delta that stops start at a hit (see
 * ContainerHard.cpp). */
void *fn_80021B08(void);
bool fn_801BFC28(int arg0, Vec3f *start, Vec3f *delta, u32 *flags, int arg4, float radius);
/* The collision world and a probe along segment (see ContainerBehavior.cpp). */
void *fn_80020D40(void);
UnkCollision_80021E24 *fn_80021E24(void *world, void *segment, int arg2, int arg3,
                                   bool (*filter)(UnkCollision_80021E24 *hit), void *ignore);
void fn_800265A4(UnkCollision_80021E24 *hit, int arg1);

/* Vectors (see ContainerHard.cpp). */
Vec3f fn_8040CDA8(const Vec3f *vector, float scale);
Vec3f fn_8040CEE8(const Vec3f *vector);

/* Sounds (see BlackDoomCtrlUnit.cpp). */
void fn_801D53C4(void *manager, int id, const Vec3f *position, void *source, int arg4, int arg5,
                 int arg6);
}

/* A distance Rocket.cpp passes to its debug arc (meaning unknown; defined here). */
extern const float lbl_805F8560 = 200.0f;

/* result = a - b (as in Rocket.cpp). */
inline const Vec3f &Subtract(Vec3f *result, const Vec3f *a, const Vec3f *b) {
    fn_8040CD74(result, a, b);
    return *result;
}

/* Copies a rotation one angle at a time (as in Rocket.cpp). */
inline void CopyRotation(Vec3f &rotation, const Vec3f &value) {
    rotation.x = value.x;
    rotation.y = value.y;
    rotation.z = value.z;
}

namespace Parachute {

/* A 32-bit flag word (as Rocket::RocketFlags). */
struct ParachuteFlags {
    ParachuteFlags() {
        fn_80014E04(&m_bits);
    }
    void Clear() {
        fn_80014E10(&m_bits);
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

/* The parachute (class name from RTTI; names inferred). Flag bit 0: let go of the character; bit
 * 1: falling (set once the model's opening motion ended). States: 0 opening, 1 falling, 2 landed
 * or left, 3 done. */
class Parachute : public Task {
  public:
    Parachute(Task *parent, const boost::weak_ptr<CharInfo> &chara, const Vec3f *position,
              Vec3f *rotation);
    virtual ~Parachute();
    virtual void Tick(float dt);

    void UpdateModel();
    void Fall(float dt);
    void OnCommand(CharCommand &command);

    static bool IsLandable(UnkCollision_80021E24 *hit);

    /* 0x28 */ boost::shared_ptr<GadgetCharInfo> m_chara;
    /* 0x30 */ boost::shared_ptr<GadgetModelInfo> m_model;
    /* 0x38 */ boost::weak_ptr<CharInfo> m_player;
    /* 0x40 */ s32 m_state;
    /* 0x44 */ Vec3f m_origin;   /* where it opened */
    /* 0x50 */ Vec3f m_previous; /* the position of the previous frame */
    /* 0x5C */ ParachuteFlags m_status;
    /* 0x60 */ ParachuteFlags m_sweepFlags;
    /* 0x64 */ u32 field_0x64;
};
SIZE_ASSERT(Parachute, 0x68);

/* The model file (name inferred). */
static const char *s_fileName = "ROCKET.BIN";

/* A float initialized at startup (struct name unknown; retail sets both values below in the
 * unit's static initializer). */
struct UnkStaticFloat {
    UnkStaticFloat(float value) : m_value(value) {}

    float m_value;
};

/* How far below the parachute the ground probe reaches, and a turn speed of 2 degrees that
 * nothing reads (names inferred). */
static UnkStaticFloat s_probeLength(20.0f);
static UnkStaticFloat s_turnSpeed(2.0f * (lbl_805FAD38 / 180.0f));

/* The ground probe's filter: not characters of types 1, 2 and 0xD (name inferred). */
bool Parachute::IsLandable(UnkCollision_80021E24 *hit) {
    if (hit->m_owner != 0) {
        u16 type = hit->m_owner->GetType();
        if (type == 1 || type == 2 || type == 0xD) {
            return false;
        }
    }
    return true;
}

Parachute::Parachute(Task *parent, const boost::weak_ptr<CharInfo> &chara,
                     const Vec3f *position, Vec3f *rotation)
    : Task(parent), m_player(chara), m_state(0) {
    m_chara = fn_8015CB88(0x89, 4, "Parachute");
    fn_8015CF7C(m_chara.get());
    fn_800729B0(&m_chara->m_commandUnit->m_callback,
                &(const CharCommandCallback &)CharCommandCallback(
                    boost::bind(&Parachute::OnCommand, this, _1)));
    fn_800091D0(&m_chara->m_position, position);
    CopyRotation(m_chara->m_rotation, *rotation);
    fn_800091D0(&m_origin, fn_800091D0(&m_previous, &m_chara->m_position));
    m_model = fn_8015F800(0, s_fileName);
    m_model->fn_80162A68(2, 0);
    m_model->fn_80160C28(2);
    m_model->fn_80160B18(2);
    if (boost::shared_ptr<CharInfo> player = m_player.lock()) {
        player->PushCommand(new Player::TouchHangCommand(m_chara, &m_chara->m_position,
                                                         &m_chara->m_rotation, 3, 4));
    }
    UpdateModel();
    m_status.Clear();
    m_status.Set(1);
    m_sweepFlags.Set(1);
    fn_801D560C(fn_80053164(), 0x100C, 0, 0);
}

Parachute::~Parachute() {}

void Parachute::Tick(float dt) {
    if (m_status.Test(1)) {
        Fall(dt);
    }
    UpdateModel();
    if (m_model.get() != 0) {
        m_model->fn_80162FB0(dt);
    }
    if (!m_status.Test(0)) {
        if (boost::shared_ptr<CharInfo> player = m_player.lock()) {
            player->PushCommand(new Player::TouchHangCommand(m_chara, &m_chara->m_position,
                                                             &m_chara->m_rotation, 3, 4));
        }
    }
    switch (m_state) {
    case 0:
        if (fn_80160040(m_model.get(), 2)) {
            m_status.Set(1);
            m_state = 1;
        }
        break;
    case 2:
        if (boost::shared_ptr<CharInfo> player = m_player.lock()) {
            player->PushCommand(new Player::CancelBehaviorCommand(4));
        }
        m_state = 3;
        break;
    case 3:
        m_flags |= 1; /* Task's: done */
        break;
    }
}

void Parachute::UpdateModel() {
    if (m_model.get() != 0) {
        Mtx34f matrix;
        fn_8015CEA0(m_chara.get(), &matrix);
        m_model->fn_801633DC(&matrix);
    }
}

void Parachute::Fall(float dt) {
    fn_804103E8(&m_chara->m_position, &m_chara->field_0x18, &m_chara->field_0x24, 1.0f, dt);
    m_chara->m_position.y -= 66.0f * dt;
    Vec3f delta;
    fn_800091EC(&delta, &Subtract(&Vec3f(), &m_chara.px->m_position, &m_previous));
    Vec3f start;
    fn_800091EC(&start, &m_previous);
    fn_80021B08();
    if (fn_801BFC28(0, &start, &delta, &m_sweepFlags.m_bits, 0, 10.0f)) {
        fn_800091D0(&m_chara->m_position, &start);
        AssignVector(m_chara.px->field_0x18, fn_8040CDA8(&delta, 1.0f / dt));
        m_chara->field_0x18.y = 0.0f;
    }
    fn_800091D0(&m_previous, &m_chara->m_position);
    fn_8040CE60(&m_chara->field_0x24);
    Vec3f top;
    fn_800091EC(&top, &m_chara->m_position);
    Vec3f bottom;
    fn_800091EC(&bottom, &top);
    bottom.y -= s_probeLength.m_value;
    u8 segment[0x18];
    fn_80020A20(segment, &top, &bottom);
    UnkCollision_80021E24 *hit = fn_80021E24(fn_80020D40(), segment, 3, 8, IsLandable, 0);
    if (hit != 0) {
        if (hit->m_object != 0) {
            m_status.Set(0);
            m_status.Reset(1);
            m_state = 2;
        }
        fn_800265A4(hit, 1);
    }
    Vec3f offset;
    fn_800091EC(&offset, &Subtract(&Vec3f(), &m_chara.px->m_position, &m_origin));
    offset.y = 0.0f;
    float distance = fn_8040CF50(&offset);
    if (distance > 200.0f) {
        distance = 200.0f;
    }
    AssignVector(offset, fn_8040CDA8(&fn_8040CEE8(&offset), distance));
    m_chara->m_position.x = m_origin.x + offset.x;
    m_chara->m_position.z = m_origin.z + offset.z;
    fn_801D53C4(fn_80053164(), 0x100D, 0, this, 2, 0, 2);
}

void Parachute::OnCommand(CharCommand &command) {
    switch (command.TypeId()) {
    case 0xC:
        m_status.Set(0);
        m_state = 2;
        break;
    case 1:
        AssignVector(m_chara.px->field_0x24,
                     fn_8040CDA8(&reinterpret_cast<UnkPushCommand &>(command).m_direction, 80.0f));
        m_chara->field_0x24.y = 0.0f;
        break;
    }
}

} // namespace Parachute

/* Opens a parachute for chara at position and rotation (name unknown; called by
 * Game/Rocket.cpp). */
extern "C" void fn_80292A64(boost::weak_ptr<CharInfo> *chara, const Vec3f *position,
                            Vec3f *rotation) {
    Task *parent = fn_801EB304(fn_8001C4F8(), 0xB);
    new Parachute::Parachute(parent, *chara, position, rotation);
}
