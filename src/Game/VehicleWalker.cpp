/* Vehicle::Walker (class name from RTTI): the walker, a vehicle on legs that its rider steers
 * (command 1), jumps with (command 0x15, action 2: a short hop while the button is held, or a drop
 * from a ledge) and, as the "WalkerCannon" kind (vehicle type 6), fires a gun (actions 6 and 7).
 * It walks on the ground through the vehicles' collision move, carries along with what it stands
 * on, turns toward the steering, casts a shadow, and takes damage from attacks and from landing on
 * enemies. State 0 walks, 1 jumps, 2 falls, 3 lands, 4 flinches and 5 drops. File name from the
 * class (prefixed like Game/VehicleAirBolt.cpp); member and function names are inferred.
 * Definitions are in reverse address order (deferred inlining emits functions last-defined first).
 * The unit's code reaches its .data through a pooled base address, so the linker keeps that
 * section whole: Game/TNumEdit.h and Game/CharaColliBaseCharInfo.h supply the RTTI base lists it
 * starts with. */
#include <list>
#include <boost/bind.hpp>
#include <boost/function.hpp>

/* Slot 0x48 with the rider (see Game/TVehicleBase.h). */
#define VEHICLE_GET_ON_RIDER
#include "Game/TNumEdit.h"
/* After TNumEdit.h: the order of the casts sets the order of the RTTI base lists in .data. */
#include "Game/CharaColliBaseCharInfo.h"
#include "Effect/System/EffectElement.h"
#include "Game/BirdBase.h"
#define CHARCOMMAND_PLAYER_COMMANDS
#include "Game/CharCommand.h"
#include "Game/CharInfo.h"
#include "Game/CharaColli.h"
#include "Game/CharaColliAttack.h"
#include "Game/GadgetClassDesc.h"
#include "Game/MathConstants.h"
#include "Game/MathTypes.h"
#include "Game/TGi.h"
#include "Game/TVehicleBase.h"
#include "Game/VehicleEffect.h"
#include "Game/Vibration.h"
#include "Game/WalkerModel.h"
#include "boost/shared_ptr.hpp"
#include "boost/weak_ptr.hpp"
#include "size_assert.h"

using Effect::System::EffectElement;
using SonicteamUSA::System::CharaColli::AttackCallback;
using SonicteamUSA::System::CharaColli::AttackCallbackParam;
using SonicteamUSA::System::CharaColli::CharaColliAttack;
using SonicteamUSA::System::CharaColli::CharaColliBase;
using Vehicle::TVehicleBase;
using Vehicle::WalkerModel;
using Vehicle::BirdBase::UnkBirdCollision_801B4EBC;

namespace std {
float fabs(float x);
float cos(float x);
} // namespace std

/* A vehicle kind's entry in the vehicle table (see Game/GunLift.cpp). */
struct UnkVehicleEntry {
    const char *name;
    Vehicle::TVehicleBase *(*create)(void *arg);
};

/* What a mover stands on (see Game/VehicleCharaColliMove.cpp). */
struct UnkColliMoveContact_803334A4 {
    bool IsValid() const;

    /* 0x00 */ u8 m_isCollider;
    /* 0x04 */ boost::weak_ptr<CharInfo> m_chara;
    /* 0x0C */ boost::weak_ptr<CharaColliBase> m_collider;
    /* 0x14 */ void *m_object;
};

/* A placement taken from a contact (see Game/VehicleCharaColliMove.cpp). */
struct UnkColliMovePlacement_80333098 {
    /* 0x00 */ Vec3f m_position;
    /* 0x0C */ Vec3f m_rotation;
    /* 0x18 */ u32 m_kind;
};

/* CharaColliMoveArgs (class name from RTTI; layout as in Game/VehicleCharaColliMove.cpp): what the
 * collision move hands its callback. */
struct CharaColliMoveArgs {
    /* 0x00 */ UnkColliMoveContact_803334A4 *m_contact;
    /* 0x04 */ boost::shared_ptr<CharInfo> m_chara;
    /* 0x0C */ UnkColliMovePlacement_80333098 m_current;
    /* 0x28 */ UnkColliMovePlacement_80333098 m_previous;
};

typedef boost::function1<void, CharaColliMoveArgs &> CharaColliMoveCallback;

/* The list the collision move skips (std::list<boost::shared_ptr<CharaColliBase> >; see
 * Game/VehicleColliMove.cpp). */
struct UnkColliList_80095314;
typedef std::list<boost::shared_ptr<CharaColliBase> > ColliderList;

/* TVehicleBase::OnGetOn (0x801B62C4) called with the rider, which it does not use: the other
 * vehicles' units and the symbol name it without the argument (see Game/TVehicleBase.h), so the
 * call goes to that name. Passing the rider on keeps it in r4, as in retail. */
extern "C" void OnGetOn__Q27Vehicle12TVehicleBaseFv(TVehicleBase *vehicle,
                                                    const boost::weak_ptr<CharInfo> &rider);

inline void TVehicleBaseOnGetOn(TVehicleBase *vehicle, const boost::weak_ptr<CharInfo> &rider) {
    OnGetOn__Q27Vehicle12TVehicleBaseFv(vehicle, rider);
}

/* The vehicles' collision move (see Game/VehicleColliMove.cpp). */
class UnkVehicleColliMove_8033377C {
  public:
    UnkVehicleColliMove_8033377C(TVehicleBase *vehicle);
    ~UnkVehicleColliMove_8033377C();

    bool Move(Vec3f *position, Vec3f *velocity, bool ground, UnkColliList_80095314 *ignore);
    void TickGround(float dt);
    void ClearOnGround();
    bool IsOnGround();
    bool IsLineDrawn();

    /* 0x00 */ Vec3f m_velocity;
    /* 0x0C */ Vec3f m_normal; /* of the ground */
    /* 0x18 */ u8 field_0x18[0x28 - 0x18];
};

/* The fade of a model clump (see Game/VehicleGun.cpp). */
struct UnkClumpFade_80027D5C;

/* The gun of the vehicles of type 6 (see Game/VehicleGun.cpp). */
class UnkVehicleGun_80347A18 {
  public:
    UnkVehicleGun_80347A18(TVehicleBase *vehicle, const Mtx34f *parent);
    ~UnkVehicleGun_80347A18();
    void Update(float dt);
    void StartFire();
    void StopFire();
    void StartAim();
    void StopAim();
    void HideWithCamera();
    void ShowAlways();
    void Hide();
    void Show();
    void SetFade(const UnkClumpFade_80027D5C *source);

    /* 0x0 */ void *m_impl;
};

/* A shadow cast on the ground (see Game/BossCtrlUnit.h; constructor out of line at 0x8009368C). */
struct UnkShadow_8009368C {
    UnkShadow_8009368C();

    /* 0x00 */ Vec3f m_position;
    /* 0x0C */ Vec3f m_normal;
    /* 0x18 */ u32 field_0x18;
    /* 0x1C */ u32 field_0x1C;
    /* 0x20 */ float field_0x20;
    /* 0x24 */ bool m_visible;
};

/* The parts of AttackCallbackParam the walker reads (layout as in Game/CharaColliAttack.h; the
 * members between are named here, meanings inferred). */
struct WalkerAttackParam {
    /* 0x00 */ boost::weak_ptr<CharaColliAttack> m_attacker;
    /* 0x08 */ boost::weak_ptr<CharaColliAttack> m_target;
    /* 0x10 */ u8 field_0x10[0x18 - 0x10];
    /* 0x18 */ Vec3f m_position;
    /* 0x24 */ Vec3f m_direction;
    /* 0x30 */ u8 m_ignore;
    /* 0x34 */ Vec3f m_push;
    /* 0x40 */ u8 field_0x40[0x4C - 0x40];
    /* 0x4C */ float m_pushSpeed;
    /* 0x50 */ float m_damage;
    /* 0x54 */ s32 m_result;
};

#define Param(param) (*(WalkerAttackParam *)&(param))

extern "C" {
/* The 32-bit flag word (see Game/BAMine.cpp). */
void fn_80014E1C(u32 *flags, int bit);
void fn_8006127C(u32 *flags, int bit);
bool fn_80014E34(u32 *flags, int bit);

/* Debug output and resource loading (see Game/VehicleAirBolt.cpp), the vehicle type in use, and
 * the loader's model clumps (see Game/GunLift.cpp). */
void fn_803A953C(const char *format, ...);
void fn_8006B50C(int id, int arg1);
void fn_801B4850(int type);
void *fn_800122D4(void);
void *fn_8001277C(void *loader, int arg1, int id);

/* The walker's model (Vehicle::WalkerModel, behaviour inferred): loading the cannon's and the
 * walker's files, the motion's flag word, whether the motion ended, the fade, showing and hiding
 * it, a frame, placing it, its update, the motion playing and starting one. */
void fn_8020DF48(void);
void fn_8020DFD0(void);
u32 *fn_8020E034(WalkerModel *model);
bool fn_8020E040(WalkerModel *model);
void fn_8020E068(WalkerModel *model, void *fade);
void fn_8020E090(WalkerModel *model);
void fn_8020E0EC(WalkerModel *model);
bool fn_8020E140(WalkerModel *model);
void fn_8020E16C(WalkerModel *model);
void fn_8020E190(WalkerModel *model);
float fn_8020E1BC(WalkerModel *model);
const Mtx34f *fn_8020E1F4(WalkerModel *model, int index);
void fn_8020E22C(WalkerModel *model, const Vec3f *position, const Vec3f *rotation);
void fn_8020E274(WalkerModel *model, float dt);
int fn_8020E298(WalkerModel *model);
void fn_8020E2BC(WalkerModel *model, int motion);

/* TVehicleBase's other functions (behaviour inferred; see Game/BirdBase.cpp): the rider leaves,
 * the vehicle's life (set and get) and a change of it, its kind, the end of the ride, the default
 * command handling, a second model clump, its collision-move callback, its two colliders (get and
 * set) and a line test. */
void fn_801B6030(TVehicleBase *vehicle, int arg1);
void fn_801B6570(TVehicleBase *vehicle, float value);
float fn_801B64F8(TVehicleBase *vehicle);
void fn_801B6510(TVehicleBase *vehicle, float delta);
s32 fn_801B6938(TVehicleBase *vehicle);
void fn_801B6944(TVehicleBase *vehicle);
void fn_801B6860(TVehicleBase *vehicle, CharCommand *command);
void fn_801B64B8(TVehicleBase *vehicle, void *clump);
void fn_801B61DC(TVehicleBase *vehicle, const CharaColliMoveCallback &callback);
boost::shared_ptr<CharaColliBase> fn_801B6350(TVehicleBase *vehicle);
boost::shared_ptr<CharaColliBase> fn_801B6380(TVehicleBase *vehicle);
void fn_801B63D4(TVehicleBase *vehicle, boost::shared_ptr<CharaColliBase> collider);
void fn_801B6470(TVehicleBase *vehicle, boost::shared_ptr<CharaColliBase> collider);
void fn_801B5FA4(TVehicleBase *vehicle);
void fn_801C067C(TVehicleBase *vehicle, const boost::shared_ptr<CharaColliBase> *collider);

/* The collision at 0xA4 (see Game/BirdBase.cpp): its owner, its collider (set and get), its
 * callbacks, and two resets. */
void fn_801B4DD0(UnkBirdCollision_801B4EBC *collision, TVehicleBase *vehicle);
void fn_801B4DAC(UnkBirdCollision_801B4EBC *collision,
                 const boost::shared_ptr<CharaColliBase> *collider);
boost::shared_ptr<CharaColliBase> fn_801B4D84(UnkBirdCollision_801B4EBC *collision);
void fn_801B4D3C(UnkBirdCollision_801B4EBC *collision, const AttackCallback &onHit,
                 const AttackCallback &onEnd);
void fn_801B4D18(UnkBirdCollision_801B4EBC *collision, const AttackCallback &callback);
void fn_801B4CF4(UnkBirdCollision_801B4EBC *collision);
void fn_801B4D60(UnkBirdCollision_801B4EBC *collision);

/* The hit list at 0x10C (see Game/PlayerBarrierHeatEffect.cpp). */
void *fn_803555CC(void *list);
void fn_80355478(void *list, int);
bool fn_803553E8(void *list, const void *target);
void fn_803552DC(void *list, const void *target);
void fn_80355260(void *list, int count);
void fn_8035511C(void *list);

/* Whether an attack collision's weak pointer has expired (see Game/AttackIgnoreList.h). */
bool fn_80053538(const boost::weak_ptr<CharaColliAttack> *attack);

/* The shadow: finding the ground below and drawing it with a size (see Game/EffectKage.cpp). */
void fn_800935B4(UnkShadow_8009368C *shadow, const Vec3f *position, const Vec3f *direction,
                 ColliderList *ignore);
void fn_803563D0(UnkShadow_8009368C *shadow, float size);

/* The collider manager and a sphere collider (see Game/BirdBase.cpp). */
void *fn_80021B08(void);
boost::shared_ptr<CharaColliBase> fn_800C7DEC(void *manager,
                                              const boost::weak_ptr<CharInfo> &charInfo, int flags,
                                              int arg3, int arg4, float radius, const Vec3f *offset);

/* Effects, sounds and rumble (see Game/BirdBase.cpp), and the cannon's wreck (behaviour
 * inferred). */
boost::weak_ptr<EffectElement> fn_801D2AFC(int arg0, const Vec3f *position, const Vec3f *direction,
                                           int id, int arg4);
void *fn_80053164(void);
void fn_801D560C(void *manager, int id, int arg2, int arg3);
boost::weak_ptr<Vibration::RequestHandle> fn_8034DF48(int pattern, int player);
void fn_80221BA8(const Vec3f *position, const Vec3f *rotation, const Vec3f *velocity, int arg3,
                 int arg4);

/* Clamps (see Game/GunLift.cpp), and whether a value is zero. */
const float &fn_80070460(const float &value, const float &low, const float &high);
const float &fn_80070494(const float &a, const float &b);
bool fn_8000A950(float value);

/* Vector and matrix helpers (behaviour inferred). */
Vec3f fn_800091A4(const Vec3f *vector, float scale);
Vec3f fn_80016284(const Vec3f *vector); /* the negated vector */
void fn_8040CE60(Vec3f *vector);        /* sets the vector to zero */
Vec3f fn_8040CDA8(const Vec3f *vector, float scale);
Vec3f fn_8040CDD0(const Vec3f *a, const Vec3f *b); /* a + b */
Vec3f fn_8040CEE8(const Vec3f *vector);           /* normalized */
float fn_8040CF90(const Vec3f *a, const Vec3f *b); /* the dot product */
void fn_8040CFF8(Vec3f *vector, const Vec3f *value); /* vector += value */
Vec3f fn_8040D044(const Vec3f *vector, float angle);
Vec3f fn_8040D0AC(const Vec3f *vector, float angle);
Vec3f fn_8040D118(const Vec3f *vector, float angle);
float fn_8040E99C(float angle);
float fn_8040F314(float from, float to, float rate);
Vec3f fn_8040F5F4(const Vec3f *forward, const Vec3f *up, int order);
Vec2f fn_8040F664(const Vec3f *direction); /* the pitch and the heading */
void fn_8040F990(Vec3f *along, Vec3f *across, const Vec3f *vector, const Vec3f *axis);
void fn_804103E8(Vec3f *position, Vec3f *velocity, const Vec3f *acceleration, float drag,
                 float dt);
Vec3f fn_8040FB08(const Mtx34f *matrix);
Vec3f fn_8040B618(const Mtx34f *matrix, const Vec3f *direction);
Vec3f fn_8040B67C(const Mtx34f *matrix, const Vec3f *point);
Mtx34f fn_8040FFBC(const Mtx34f *matrix, const Vec3f *angles, int order);
Mtx34f fn_8040FEBC(const Mtx34f *matrix, const Vec3f *angles, int order);
}

/* fn_8040CD74 called as returning a - b by value (see Game/GunLift.cpp). */
typedef Vec3f (*VectorSubtractFn)(const Vec3f *a, const Vec3f *b);

/* The vector type's out-of-line copy constructor and assignment (see Game/GunLift.cpp). */
inline void ConstructVector(Vec3f *vector, const Vec3f &value) {
    fn_800091EC(vector, &value);
}

inline void AssignVector(Vec3f &vector, const Vec3f &value) {
    fn_800091D0(&vector, &value);
}

/* Degrees to radians (see Game/GunLift.cpp). */
inline float DegToRad(float degrees) {
    return degrees * (lbl_805FAD38 / 180.0f);
}

/* The dot product (name inferred). */
inline float Dot(const Vec3f *a, const Vec3f *b) {
    return fn_8040CF90(a, b);
}

/* Whether a vector is (nearly) zero (name inferred). */
inline bool IsSmall(const Vec3f &v) {
    return v.x * v.x + v.y * v.y + v.z * v.z < 0.0001f;
}

/* A length (see Game/VehicleColliMove.cpp): as a call it is not a constant expression, so the
 * constants below are stored by a static initializer, as in retail. */
inline float Span(float length) {
    return length;
}

/* The 32-bit flag word at 0x104 (struct name unknown; see Game/GunLift.cpp). */
struct WalkerFlags {
    WalkerFlags() {
        fn_80014E04(&m_bits);
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

    /* bit 0: gravity, 1: (set up), 2: jumping, 3: dropping, 4: the rider leaves on landing, 5: jump
     * requested, 6: drop requested, 7: just placed */
    u32 m_bits;
};

namespace Vehicle {

class Walker : public TVehicleBase {
  public:
    Walker(void *arg);
    virtual ~Walker();
    virtual void SetPlacement(const Vec3f *position, const Vec3f *rotation);
    virtual void GetSeat(Vec3f *position, Vec3f *rotation);
    virtual void Setup();
    virtual void OnCommand(CharCommand *command);
    virtual void vf_0x40(UnkColliContactNode_800587A0 *const &hit);
    virtual void Tick(float dt);
    virtual void OnGetOn(const boost::weak_ptr<CharInfo> &rider);
    virtual void OnGetOff();
    virtual bool vf_0x50();
    virtual bool vf_0x5C();
    virtual bool vf_0x60();
    virtual float vf_0x64();
    virtual void vf_0x68();
    virtual void vf_0x6C();
    virtual bool vf_0x70();
    virtual void vf_0x74();
    virtual void vf_0x78();
    virtual void vf_0x7C(void *arg);

    void SetState(int state) {
        m_state = state;
        m_step = 0;
        m_stateTime = 0.0f;
    }

    void UpdateModel(float dt);
    void SetupModel();
    void Turn(float dt);
    void Accelerate(float dt);
    void StateDrop(float dt);
    void StateFlinch(float dt);
    void StateLand(float dt);
    void StateFall(float dt);
    void StateJump(float dt);
    void StateWalk(float dt);
    void OnHit(AttackCallbackParam &param);
    void OnAttackEnd(AttackCallbackParam &param);
    void OnAttack(AttackCallbackParam &param);
    void Damage(float power, const Vec3f *position, const Vec3f *direction);
    void OnColliMove(CharaColliMoveArgs &args);
    void Move(float dt);
    void CreateBodyCollider();
    void LoadClumps();

    /* 0x09C */ UnkVehicleGun_80347A18 *m_gun;
    /* 0x0A0 */ WalkerModel *m_model;
    /* 0x0A4 */ UnkBirdCollision_801B4EBC m_collision;
    /* 0x0A8 */ UnkVehicleColliMove_8033377C *m_colliMove;
    /* 0x0AC */ UnkShadow_8009368C m_shadow;
    /* 0x0D4 */ s32 m_state;
    /* 0x0D8 */ s32 m_step;
    /* 0x0DC */ float m_stateTime;
    /* 0x0E0 */ float m_accel;
    /* 0x0E4 */ float m_dragAcross;
    /* 0x0E8 */ float m_dragAlong;
    /* 0x0EC */ Vec3f m_up;
    /* 0x0F8 */ Vec3f m_steer;
    /* 0x104 */ WalkerFlags m_flags;
    /* 0x108 */ float m_flinchTime;
    /* 0x10C */ u8 m_hits[0xC]; /* the hit list (constructor 0x803555CC, destructor 0x80355478) */
};
SIZE_ASSERT(Walker, 0x118);

} // namespace Vehicle

using Vehicle::Walker;

/* The body's height above the ground and the radius of the attack collider (names inferred). */
static const float s_height = Span(7.0f);
static const float s_attackRadius = Span(10.0f);


Walker::Walker(void *arg) : TVehicleBase(arg), m_gun(0), m_model(0) {
    fn_803555CC(m_hits);
    fn_801B4DD0(&m_collision, this);
    fn_8040CE60(&m_steer);
    fn_800091D0(&m_up, &Vec3f(0.0f, 1.0f, 0.0f));
    m_flinchTime = 0.0f;
    m_colliMove = new UnkVehicleColliMove_8033377C(this);
}

Walker::~Walker() {
    if (m_model != 0) {
        delete m_model;
        m_model = 0;
    }
    if (m_gun != 0) {
        delete m_gun;
        m_gun = 0;
    }
    if (m_colliMove != 0) {
        delete m_colliMove;
    }
    fn_80355478(m_hits, -1);
}

void Walker::GetSeat(Vec3f *position, Vec3f *rotation) {
    if (m_model != 0) {
        const Mtx34f *frame = fn_8020E1F4(m_model, 1);
        if (position != 0) {
            AssignVector(*position, fn_8040FB08(frame));
        }
        if (rotation != 0) {
            Vec3f up;
            fn_800091EC(&up, &(const Vec3f &)fn_8040B618(frame, &Vec3f(0.0f, 1.0f, 0.0f)));
            Vec3f forward;
            fn_800091EC(&forward, &(const Vec3f &)fn_8040B618(frame, &Vec3f(0.0f, 0.0f, 1.0f)));
            *rotation = fn_8040F5F4(&forward, &up, 2);
        }
    }
}

void Walker::Setup() {
    if (lbl_8057D6A0.m_twoPlayer != 0) {
        fn_801B6570(this, 40.0f);
        SetField98Float(40.0f);
    } else {
        fn_801B6570(this, 8.0f);
        SetField98Float(8.0f);
    }
    SetupModel();
    m_flags.Set(0);
    m_flags.Set(1);
    LoadClumps();
    fn_801B61DC(this, CharaColliMoveCallback(boost::bind(&Walker::OnColliMove, this, _1)));
    SetState(0);
}

void Walker::OnCommand(CharCommand *command) {
    switch (command->TypeId()) {
    case 1:
        fn_8040CFF8(&m_steer, &static_cast<AccelerateInputCommand *>(command)->m_direction);
        command->Flags() |= 2;
        break;
    case 2: {
        SetPositionCommand *move = static_cast<SetPositionCommand *>(command);
        AssignVector(*reinterpret_cast<Vec3f *>(field_0x64), move->m_position);
        AssignVector(m_position, move->m_position);
        m_flags.Set(7);
        command->Flags() |= 2;
        break;
    }
    case 3:
        m_rotation.y = static_cast<SetAngleCommand *>(command)->m_angles.y;
        command->Flags() |= 2;
        break;
    case 4:
    case 5:
        command->Reject(GetSelf());
        command->Flags() |= 2;
        break;
    case 7:
        Damage(static_cast<DamageCommand *>(command)->m_amount, &m_position,
               &static_cast<DamageCommand *>(command)->m_direction);
        command->Flags() |= 2;
        break;
    case 0x15:
        switch (static_cast<ControlVehicleCommand *>(command)->m_action) {
        case 1:
            fn_801B6030(this, 0);
            command->Flags() |= 2;
            break;
        case 2:
            if (m_colliMove->IsOnGround()) {
                m_flags.Set(5);
            } else {
                m_flags.Set(6);
            }
            break;
        case 3:
            m_flags.Clear(2);
            m_flags.Clear(3);
            break;
        case 4:
            m_flags.Set(4);
            command->Flags() |= 2;
            break;
        case 6:
            if (m_gun != 0) {
                m_gun->StartFire();
            }
            break;
        case 7:
            if (m_gun != 0) {
                m_gun->StopFire();
            }
            break;
        }
        break;
    }
    if (!(command->Flags() & 2)) {
        fn_801B6860(this, command);
    }
}

void Walker::vf_0x40(UnkColliContactNode_800587A0 *const &hit) {}

void Walker::SetPlacement(const Vec3f *position, const Vec3f *rotation) {
    TVehicleBase::SetPlacement(position, rotation);
    UpdateModel(0.0f);
    AssignVector(*reinterpret_cast<Vec3f *>(field_0x64), m_position);
    m_flags.Set(7);
}

void Walker::Tick(float dt) {
    if (m_flinchTime > 0.0f) {
        fn_8020E190(m_model);
        if (m_gun != 0) {
            m_gun->HideWithCamera();
        }
        m_flinchTime -= dt;
        if (m_flinchTime <= 0.0f) {
            m_flinchTime = 0.0f;
            fn_8020E16C(m_model);
            if (m_gun != 0) {
                m_gun->ShowAlways();
            }
        }
    }
    if (m_colliMove != 0) {
        m_colliMove->TickGround(dt);
    }
    if (m_colliMove->IsOnGround()) {
        if (m_flags.Test(5)) {
            SetState(1);
        }
    } else if (m_flags.Test(6)) {
        SetState(5);
    }
    bool grounded = false;
    switch (m_state) {
    case 0:
        StateWalk(dt);
        grounded = true;
        break;
    case 1:
        StateJump(dt);
        break;
    case 2:
        StateFall(dt);
        break;
    case 3:
        StateLand(dt);
        grounded = true;
        break;
    case 4:
        StateFlinch(dt);
        if (m_colliMove->IsOnGround()) {
            grounded = true;
        }
        break;
    case 5:
        StateDrop(dt);
        break;
    }
    if (grounded && m_flags.Test(4)) {
        fn_801B6030(this, 0);
    }
    Turn(dt);
    Accelerate(dt);
    Move(dt);
    UpdateModel(dt);
    {
        ColliderList ignore;
        ignore.push_back(fn_801B6380(this));
        ignore.push_back(fn_801B6350(this));
        fn_800935B4(&m_shadow, &m_position, &(const Vec3f &)fn_8040CDA8(&Vec3f(0.0f, -1.0f, 0.0f), 200.0f), &ignore);
        m_flags.Clear(7);
    }
    fn_803563D0(&m_shadow, 8.0f);
    fn_801C067C(this, &fn_801B4D84(&m_collision));
    fn_80355260(&m_hits, 1);
    fn_8035511C(&m_hits);
    fn_8040CE60(&field_0x24);
    fn_8040CE60(&m_steer);
    AssignVector(*reinterpret_cast<Vec3f *>(field_0x64), m_position);
    m_flags.Clear(4);
    m_flags.Clear(5);
    m_flags.Clear(6);
    if (fn_801B64F8(this) <= 0.0f) {
        fn_801D560C(fn_80053164(), 0x2053, 0, 0);
        if (field_0x74 != 0) {
            reinterpret_cast<Vehicle::Effect *>(field_0x74)->StartBurst((void *)fn_8020E1F4(m_model, 0));
        }
        if (fn_801B6938(this) == 6) {
            fn_80221BA8(&(const Vec3f &)fn_8040CDD0(&m_position, &Vec3f(0.0f, 10.0f, 0.0f)), &m_rotation,
                        &Vec3f(0.0f, 20.0f, 0.0f), 4, -1);
        }
        fn_801B6030(this, 0);
        fn_801B6944(this);
    }
    if (m_colliMove->IsLineDrawn()) {
        fn_801B5FA4(this);
    }
}

void Walker::OnGetOn(const boost::weak_ptr<CharInfo> &rider) {
    TVehicleBaseOnGetOn(this, rider);
    SetState(0);
    {
        void *manager = fn_80021B08();
        boost::shared_ptr<CharaColliBase> collider;
        collider = fn_800C7DEC(manager, boost::weak_ptr<CharInfo>(GetSelf()), 0, 2, 0, s_attackRadius,
                               &lbl_805E07A0);
        fn_801B4DAC(&m_collision, &collider);
        fn_801B4D3C(&m_collision, AttackCallback(boost::bind(&Walker::OnAttack, this, _1)),
                    AttackCallback(boost::bind(&Walker::OnAttackEnd, this, _1)));
        fn_801B4D18(&m_collision, AttackCallback(boost::bind(&Walker::OnHit, this, _1)));
    }
    fn_801D560C(fn_80053164(), 0x204A, 0, 0);
    if (m_gun != 0) {
        m_gun->StartAim();
    }
    CreateBodyCollider();
}

void Walker::OnGetOff() {
    TVehicleBase::OnGetOff();
    m_flags.Clear(2);
    m_flags.Clear(3);
    fn_801B4CF4(&m_collision);
    fn_801B4D60(&m_collision);
    if (m_gun != 0) {
        m_gun->StopAim();
        m_gun->StopFire();
    }
    LoadClumps();
}

bool Walker::vf_0x60() {
    return true;
}

float Walker::vf_0x64() {
    return fn_8020E1BC(m_model);
}

void Walker::vf_0x68() {
    fn_8020E190(m_model);
    if (m_gun != 0) {
        m_gun->HideWithCamera();
    }
}

void Walker::vf_0x6C() {
    fn_8020E16C(m_model);
    if (m_gun != 0) {
        m_gun->ShowAlways();
    }
}

bool Walker::vf_0x70() {
    return fn_8020E140(m_model);
}

void Walker::vf_0x74() {
    fn_8020E0EC(m_model);
    if (m_gun != 0) {
        m_gun->Hide();
    }
}

void Walker::vf_0x78() {
    fn_8020E090(m_model);
    if (m_gun != 0) {
        m_gun->Show();
    }
}

void Walker::vf_0x7C(void *arg) {
    fn_8020E068(m_model, arg);
    if (m_gun != 0) {
        m_gun->SetFade((const UnkClumpFade_80027D5C *)arg);
    }
}

void Walker::LoadClumps() {
    void *loader = fn_800122D4();
    void *clump = fn_8001277C(loader, 3, 0x2E);
    SetClump(clump);
    fn_801B64B8(this, fn_8001277C(loader, 3, 0x2F));
}

void Walker::CreateBodyCollider() {
    void *manager = fn_80021B08();
    boost::shared_ptr<CharaColliBase> collider = fn_800C7DEC(
        manager, boost::weak_ptr<CharInfo>(GetSelf()), 0x80302, 2, 0, 0.51f + s_height, &lbl_805E07A0);
    fn_801B6470(this, collider);
    fn_801B63D4(this, boost::shared_ptr<CharaColliBase>());
}

void Walker::Move(float dt) {
    Vec3f delta;
    ConstructVector(&delta, ((VectorSubtractFn)fn_8040CD74)(&m_position,
                                                            reinterpret_cast<Vec3f *>(field_0x64)));
    Vec3f position;
    ConstructVector(&position, *reinterpret_cast<Vec3f *>(field_0x64));
    m_colliMove->Move(&position, &delta, true, 0);
    if (m_colliMove->IsOnGround() && !fn_8000A950(dt)) {
        AssignVector(field_0x18, fn_8040CDA8(&delta, 1.0f / dt));
    }
    AssignVector(m_position, position);
}

void Walker::OnColliMove(CharaColliMoveArgs &args) {
    Mtx34f matrix;
    Mtx34f translation;
    fn_8040B034(&translation, &args.m_current.m_position);
    fn_8040C148(&matrix, &translation);
    fn_8040C148(&matrix, &(const Mtx34f &)fn_8040FFBC(&matrix, &args.m_current.m_rotation, args.m_current.m_kind));
    fn_8040C148(&matrix, &(const Mtx34f &)fn_8040FEBC(&matrix, &args.m_previous.m_rotation, args.m_previous.m_kind));
    fn_8040B59C(&matrix, &(const Vec3f &)fn_80016284(&args.m_previous.m_position));
    const Vec3f &moved = fn_8040B67C(&matrix, &args.m_chara->m_position);
    fn_800091D0(&args.m_chara->m_position, &moved);
    args.m_chara->m_rotation.y += args.m_current.m_rotation.y - args.m_previous.m_rotation.y;
    Vec3f delta;
    ConstructVector(&delta, ((VectorSubtractFn)fn_8040CD74)(&m_position,
                                                            reinterpret_cast<Vec3f *>(field_0x64)));
    Vec3f position;
    ConstructVector(&position, *reinterpret_cast<Vec3f *>(field_0x64));
    ColliderList ignore;
    if (args.m_contact != 0 && args.m_contact->IsValid() && !args.m_contact->m_collider.expired()) {
        ignore.push_back(args.m_contact->m_collider.lock());
    }
    m_colliMove->Move(&position, &delta, false, (UnkColliList_80095314 *)&ignore);
    AssignVector(m_position, position);
    AssignVector(*reinterpret_cast<Vec3f *>(field_0x64), position);
}

void Walker::Damage(float power, const Vec3f *position, const Vec3f *direction) {
    if (power >= 1.0f) {
        fn_801D2AFC(0, position, direction, 0x17, 0);
        if (m_flinchTime <= 0.0f) {
            SetState(4);
            fn_801B6510(this, -power);
            m_flinchTime = 2.0f;
            fn_801D560C(fn_80053164(), 0x2052, 0, 0);
            if (!GetRider().expired()) {
                fn_8034DF48(4, GetRider().lock()->field_0x54);
            }
        }
    }
}

void Walker::OnAttack(AttackCallbackParam &param) {
    WalkerAttackParam &hit = Param(param);
    if (!fn_80053538(&hit.m_target)) {
        if (fn_803553E8(&m_hits, &hit.m_target)) {
            hit.m_ignore = 1;
        }
        fn_803552DC(&m_hits, &hit.m_target);
        if (hit.m_ignore != 0) {
            return;
        }
    }
    if (m_state != 2) {
        hit.m_ignore = 1;
    } else if (hit.m_position.y >= m_position.y) {
        hit.m_ignore = 1;
    } else {
        hit.m_damage = 4.0f;
        hit.m_pushSpeed = fn_8040CF50(&field_0x18);
        fn_800091D0(&hit.m_push, &Vec3f(0.0f, -1.0f, 0.0f));
    }
}

void Walker::OnAttackEnd(AttackCallbackParam &param) {}

void Walker::OnHit(AttackCallbackParam &param) {
    if (!fn_80053538(&param.m_attacker) && m_state == 2) {
        bool armed = param.m_attacker.lock()->m_kind >= 1;
        if (armed && Param(param).m_position.y < m_position.y) {
            param.m_result = 5;
            return;
        }
    }
    Damage(param.m_damage, &Param(param).m_position, &Param(param).m_direction);
    if (fn_801B64F8(this) <= 0.0f) {
        param.m_result = 3;
    } else {
        param.m_result = 1;
    }
}

void Walker::StateWalk(float dt) {
    switch (m_step) {
    case 0:
        fn_8020E2BC(m_model, 0);
        m_step++;
    }
    m_accel = 2400.0f;
    if (!m_colliMove->IsOnGround()) {
        SetState(2);
    }
}

void Walker::StateJump(float dt) {
    switch (m_step) {
    case 0:
        fn_8020E2BC(m_model, 3);
        m_flags.Set(2);
        m_accel = 0.0f;
        m_step++;
        return;
    case 1:
        if (fn_80014E34(fn_8020E034(m_model), 3)) {
            m_colliMove->ClearOnGround();
            fn_8040CFF8(&field_0x18, &(const Vec3f &)fn_800091A4(&m_up, fn_801B6938(this) == 5 ? 400.0f : 200.0f));
            m_accel = 2400.0f;
            m_step++;
        }
        return;
    case 2:
        if (m_flags.Test(2)) {
            fn_8040CFF8(&field_0x24, &(const Vec3f &)fn_800091A4(&m_up, fn_801B6938(this) == 5 ? 500.0f : 250.0f));
            m_stateTime += dt;
            if (m_stateTime >= 1.25f) {
                m_flags.Clear(2);
            }
        } else {
            m_step++;
        }
        break;
    }
    if (fn_8040CF90(&field_0x18, &m_up) <= 0.0f) {
        SetState(2);
    } else if (m_colliMove->IsOnGround()) {
        SetState(3);
    }
}

void Walker::StateFall(float dt) {
    switch (m_step) {
    case 0:
        fn_8020E2BC(m_model, 4);
        m_accel = 2400.0f;
        m_step++;
        break;
    default:
        if (m_colliMove->IsOnGround()) {
            SetState(3);
        }
        break;
    }
}

void Walker::StateLand(float dt) {
    switch (m_step) {
    case 0:
        fn_8020E2BC(m_model, 5);
        m_accel = 0.0f;
        m_step++;
        break;
    case 1:
        if (fn_8020E040(m_model)) {
            SetState(0);
        }
        break;
    }
}

void Walker::StateFlinch(float dt) {
    switch (m_step) {
    case 0:
        fn_8020E2BC(m_model, 1);
        m_step++;
        break;
    case 1:
        if (fn_8020E298(m_model) != 1) {
            SetState(0);
        }
        break;
    }
}

void Walker::StateDrop(float dt) {
    switch (m_step) {
    case 0:
        fn_8020E2BC(m_model, 6);
        m_flags.Set(3);
        m_step++;
        break;
    case 1:
        if (!m_flags.Test(3)) {
            SetState(2);
        } else if (m_colliMove->IsOnGround()) {
            SetState(3);
            m_flags.Clear(3);
        }
        break;
    }
}

void Walker::Accelerate(float dt) {
    Vec3f up;
    ConstructVector(&up, m_up);
    if (m_flags.Test(0)) {
        float gravity = m_colliMove->IsOnGround() ? -4000.0f : -475.0f;
        if (Dot(&m_colliMove->m_normal, &Vec3f(0.0f, 1.0f, 0.0f)) > std::cos(DegToRad(30.0f))) {
            AssignVector(up, m_colliMove->m_normal);
        }
        fn_8040CFF8(&field_0x24, &(const Vec3f &)fn_800091A4(&up, gravity));
    }
    if (m_flags.Test(3) && fn_8040CF90(&field_0x18, &up) < 0.0f) {
        fn_8040CFF8(&field_0x24, &(const Vec3f &)fn_800091A4(&up, 400.0f));
        m_dragAcross = 300.0f;
        m_dragAlong = 7.0f;
    } else {
        m_dragAcross = 25.0f;
        m_dragAlong = 2.25f;
    }
    if (!IsSmall(m_steer)) {
        Vec3f forward(0.0f, 0.0f, -1.0f);
        AssignVector(forward, fn_8040D044(&forward, m_rotation.z));
        AssignVector(forward, fn_8040D118(&forward, m_rotation.x));
        AssignVector(forward, fn_8040D0AC(&forward, m_rotation.y));
        float drive = fn_8040CF90(&m_steer, &forward);
        drive = fn_80070494(drive, 0.05f);
        if (m_colliMove->IsOnGround()) {
            fn_8040CFF8(&field_0x24, &(const Vec3f &)fn_800091A4(&(const Vec3f &)fn_800091A4(&forward, drive), m_accel));
        } else {
            fn_8040CFF8(&field_0x24, &(const Vec3f &)fn_800091A4(&(const Vec3f &)fn_800091A4(&m_steer, drive), m_accel));
        }
    }
    Vec3f velocityAcross;
    Vec3f velocityAlong;
    fn_8040F990(&velocityAlong, &velocityAcross, &field_0x18, &up);
    Vec3f accelAcross;
    Vec3f accelAlong;
    fn_8040F990(&accelAlong, &accelAcross, &field_0x24, &up);
    fn_804103E8(&m_position, &velocityAcross, &accelAcross, m_dragAcross, dt);
    fn_804103E8(&m_position, &velocityAlong, &accelAlong, m_dragAlong, dt);
    AssignVector(field_0x18, fn_8040CDD0(&velocityAcross, &velocityAlong));
}

void Walker::Turn(float dt) {
    if (!IsSmall(m_steer)) {
        Vec2f angles = fn_8040F664(&(const Vec3f &)fn_80016284(&m_steer));
        float yaw = angles.y;
        float step = dt * DegToRad(125.0f);
        float rate = step / std::fabs(fn_8040E99C(m_rotation.y - yaw));
        rate = fn_80070460(rate, 0.0f, 0.5f);
        m_rotation.y = fn_8040F314(m_rotation.y, yaw, rate);
        m_rotation.y = fn_8040E99C(m_rotation.y);
        if (!GetRider().expired()) {
            boost::shared_ptr<CharInfo> rider = GetRider().lock();
            Vec3f target;
            fn_800091EC(&target, &(const Vec3f &)fn_8040CDD0(&m_position, &(const Vec3f &)fn_8040CDA8(&(const Vec3f &)fn_8040CEE8(&m_steer), 300.0f)));
            rider->PushCommand(new Player::SetLookAtPositionCommand(target, 0.05f));
        }
    }
}

void Walker::SetupModel() {
    if (m_model != 0) {
        if (m_model != 0) {
            delete m_model;
            m_model = 0;
        }
        if (m_gun != 0) {
            delete m_gun;
            m_gun = 0;
        }
    }
    m_model = new WalkerModel(this);
    if (fn_801B6938(this) == 6) {
        const Mtx34f *frame = fn_8020E1F4(m_model, 3);
        m_gun = new UnkVehicleGun_80347A18(this, frame);
    }
    if (field_0x74 != 0) {
        reinterpret_cast<Vehicle::Effect *>(field_0x74)->SetNode((void *)fn_8020E1F4(m_model, 0));
    }
}

void Walker::UpdateModel(float dt) {
    Mtx34f matrix;
    fn_8040C148(&matrix, &lbl_805E0720);
    fn_8040B848(&matrix, m_rotation.y);
    fn_8040B884(&matrix, m_rotation.x);
    fn_8040B80C(&matrix, m_rotation.z);
    Vec3f position;
    fn_800091EC(&position, &(const Vec3f &)fn_8040CDD0(&m_position, &(const Vec3f &)fn_8040B67C(&matrix, &Vec3f(0.0f, -s_height, 0.0f))));
    fn_8020E22C(m_model, &position, &m_rotation);
    fn_8020E274(m_model, dt);
    if (m_gun != 0) {
        m_gun->Update(dt);
    }
}

/* The walker and its cannon kind: loading and dropping their files, creating one, the task
 * records and the vehicle table's entries (it refers to them by address); the static initializer
 * stores their names. */
static const char *s_walkerName = "Walker";

extern "C" void fn_801ACA5C(void) {
    fn_803A953C("%s::Initialize\n", s_walkerName);
    fn_801B4850(5);
    fn_8020DFD0();
    fn_8006B50C(0x2E, 0);
    fn_8006B50C(0x2F, 0);
}

extern "C" void fn_801ACA2C(void) {
    fn_803A953C("%s::Finalize\n", s_walkerName);
}

static TVehicleBase *CreateWalker(void *arg) {
    return new Walker(arg);
}

GadgetClassDesc lbl_8053B2D8 = {
    s_walkerName, fn_801ACA5C, fn_801ACA2C, 0, 0, 0x80, 0x004A, 4, 0, 0, 0,
};

UnkVehicleEntry lbl_805E83D8 = {s_walkerName, CreateWalker};

static const char *s_cannonName = "WalkerCannon";

extern "C" void fn_801AC990(void) {
    fn_803A953C("%s::Initialize\n", s_cannonName);
    fn_801B4850(6);
    fn_8020DF48();
    fn_8006B50C(0x2E, 0);
    fn_8006B50C(0x2F, 0);
}

extern "C" void fn_801AC960(void) {
    fn_803A953C("%s::Finalize\n", s_cannonName);
}

static TVehicleBase *CreateCannon(void *arg) {
    return new Walker(arg);
}

GadgetClassDesc lbl_8053B2FC = {
    s_cannonName, fn_801AC990, fn_801AC960, 0, 0, 0x80, 0x004B, 4, 0, 0, 0,
};

UnkVehicleEntry lbl_805E83E4 = {s_cannonName, CreateCannon};

bool Walker::vf_0x5C() {
    return true;
}

bool Walker::vf_0x50() {
    return fn_801B6938(this) == 6;
}
