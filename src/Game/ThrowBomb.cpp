/* NonMatching: every function and section matches in objdiff, but GC/2.7 emits ThrowBomb's weak
 * vtable and RTTI record before the control blocks' (retail has __RTTI__9ThrowBomb after
 * __RTTI__Q35boost6detail15sp_counted_base in .sdata), which swaps two .sdata records and the four
 * pointers to them; no source shape tried changed that order.
 *
 * ThrowBomb (class name from RTTI): the bomb that bosses and enemies throw (Heavy Dog, ...). Its
 * implementation, ThrowBombImpl (class name from RTTI), keeps a PickupCharInfo, so characters can
 * pick the bomb up, carry it and throw or drop it in turn; a vacuum can pull it in, through a
 * VacuumThrowBomb (class name from RTTI). Thrown, it flies under gravity, bounces off the ground and
 * spins, and once its timer runs out or an attack hits it, it explodes. A ThrowBombTask (class
 * name from RTTI) drives it every frame. File name inferred from the classes; names marked "(name
 * inferred)" and all member names are not original. Definitions are in reverse address order
 * (deferred inlining emits functions last-defined first). */
#include <boost/function.hpp>

#include "Game/CharaColliBaseCharInfo.h"
#include "Effect/System/EffectElement.h"
#include "Game/CharCommand.h"
#include "Game/CharCommandUnit.h"
#include "Game/CharInfo.h"
#include "Game/CharaColli.h"
#include "Game/CharaColliAttack.h"
#include "Game/MathConstants.h"
#include "Game/MathTypes.h"
#include "Game/PickupCharInfo.h"
#include "Game/Quaternion.h"
#include "Game/Random.h"
#include "Game/Task.h"
#include "Game/ThrowBomb.h"
#include "Game/VacuumObject.h"
#include "Game/Vibration.h"
#include "boost/enable_shared_from_this.hpp"
#include "boost/shared_ptr.hpp"
#include "boost/weak_ptr.hpp"
#include "size_assert.h"
/* After the shared_ptr headers: retail emits the instances of shared_ptr's function templates
 * before those of boost::bind (CodeWarrior emits them in the order they are declared; as in
 * EggLastMech.cpp). */
#include <boost/bind.hpp>

using SonicteamUSA::System::CharaColli::AttackCallback;
using SonicteamUSA::System::CharaColli::AttackCallbackParam;
using SonicteamUSA::System::CharaColli::CharaColliAttack;
using SonicteamUSA::System::CharaColli::CharaColliBase;
using SonicteamUSA::System::CharaColli::CharaColliBaseCharInfo;

/* MSL's std::cos for floats, which the game calls out of line (as in DevilDoomCtrlUnit.cpp). */
namespace std {
float cos(float x);
} // namespace std

/* The parameters of a throw (struct name unknown; layout from its constructor, 0x80194C1C; as in
 * HeavyDog0510CtrlUnit.cpp). */
struct UnkEffectParam_80194C1C {
    /* 0x00 */ Vec3f m_position;
    /* 0x0C */ Vec3f m_velocity;
    /* 0x18 */ float m_scale;
    /* 0x1C */ float field_0x1C;
    /* 0x20 */ u8 m_flag;
};

/* Target (class name from RTTI): a point characters aim at. Only the flag word is mapped here
 * (bit 0 makes the target active; names inferred, as in Bomb.cpp). */
class Target {
  public:
    void TurnOn() {
        m_flags |= 1;
    }
    void TurnOff() {
        m_flags &= ~1;
    }
    void SetFlags(u32 flags) {
        m_flags = flags;
    }

    /* 0x00 */ u8 field_0x0[0x30];
    /* 0x30 */ u32 m_flags;
};

/* The model of the bomb (class name unknown; 8 bytes, the word at 0x4 is its number of frames;
 * names inferred). */
class UnkModel_801A3E9C {
  public:
    UnkModel_801A3E9C();
    ~UnkModel_801A3E9C();

    /* 0x0 */ u32 field_0x0;
    /* 0x4 */ s32 m_frameCount;
};

/* What UnkModel_801A3E9C's fn_801A3CE4 takes (struct name unknown). */
struct UnkModelParam_801A3CE4 {
    /* 0x0 */ const void *m_data;
    /* 0x4 */ s32 m_count;
};

/* A RenderWare matrix and frame (as in GadgetModelInfo.cpp). */
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
struct RwFrame {
    /* 0x00 */ u8 field_0x0[0x10];
    /* 0x10 */ RwMatrix modelling;
};

/* A sphere: centre and radius (built by fn_80021C04; as in MeteorsHolder.cpp). */
struct UnkSphere_80021C04 {
    /* 0x00 */ float center[3];
    /* 0x0C */ float radius;
};

/* What fn_80021934 and fn_80021CA4 pass their filter for each candidate hit (as in
 * EggMecaCtrlUnit.cpp). */
struct UnkCollisionCandidate_80021934 {
    /* 0x00 */ u8 field_0x0[0x8];
    /* 0x08 */ CharaColliBase *m_collider;
    /* 0x0C */ u32 m_attributes;
    /* 0x10 */ CharaColliBase *m_self;
};

/* A collision query's result list and its contacts (names unknown; see ScatteringRing.cpp and
 * BlockChange.cpp). */
struct UnkHits_80021CA4;
struct UnkContactShape_8001E9BC;
struct UnkContact_8001EE04 {
    /* 0x00 */ u8 field_0x0[0x14];
    /* 0x14 */ UnkContactShape_8001E9BC *m_shape;
};

/* The game state (struct name unknown; as far as this unit reads it). */
struct UnkGameState_8057D6A0 {
    /* 0x000 */ u8 field_0x0[0x90];
    /* 0x090 */ u32 m_frame; /* counts frames (meaning inferred) */
    /* 0x094 */ u8 field_0x94[0xA8 - 0x94];
    /* 0x0A8 */ s32 m_stageId;
    /* 0x0AC */ u8 field_0xAC[0xC8 - 0xAC];
    /* 0x0C8 */ u8 m_paused; /* (meaning inferred) */
    /* 0x0C9 */ u8 field_0xC9[0x150 - 0xC9];
};
SIZE_ASSERT(UnkGameState_8057D6A0, 0x150);

extern UnkGameState_8057D6A0 lbl_8057D6A0;

class UnkCameraManager_80009548;

extern "C" {
/* A 32-bit flag word: fn_80014E04 constructs it, fn_80014E10 clears it, fn_80014E1C sets bit and
 * fn_80014E34 tests bit (see FootingMovable.cpp). */
void fn_80014E04(u32 *flags);
void fn_80014E10(u32 *flags);
void fn_80014E1C(u32 *flags, int bit);
bool fn_80014E34(u32 *flags, int bit);

/* The model: loading it from data or by id, its frame, a frame by id, its colour (behaviours
 * inferred). */
void fn_801A3CE4(UnkModel_801A3E9C *model, int arg1, const UnkModelParam_801A3CE4 *param,
                 int arg3);
void fn_801A3CA4(UnkModel_801A3E9C *model, int id, int arg2, int arg3);
RwFrame *fn_801A3750(UnkModel_801A3E9C *model);
RwFrame *fn_801A3704(UnkModel_801A3E9C *model, int id);
void fn_801A395C(UnkModel_801A3E9C *model, float r, float g, float b, float a);
RwMatrix *fn_8047EBA0(RwMatrix *matrix);
RwFrame *fn_804876E8(RwFrame *frame, const RwMatrix *matrix, int combine);
RwFrame *fn_8048722C(RwFrame *frame);
Mtx34f fn_800635A4(const RwMatrix *matrix);
Mtx34f fn_8040B8FC(const Mtx34f *a, const Mtx34f *b);

/* Starts effect id (behaviour inferred). */
boost::weak_ptr<Effect::System::EffectElement> fn_801D2BB0(int arg0, int id, int arg2);
void fn_80425E68(Effect::System::EffectElement *effect, const Mtx34f *matrix);
void fn_80425E4C(Effect::System::EffectElement *effect, const Vec3f *position);

/* Sets a character info's name, creates the pickup's target and moves it to the character info
 * each frame (behaviours inferred; see Bomb.cpp). */
void fn_801DC984(CharInfo *charInfo, const char *name);
void fn_801E4160(PickupCharInfo *charInfo, const boost::weak_ptr<CharInfo> &owner, int flags,
                 float radius);
void fn_801E413C(PickupCharInfo *charInfo, CharInfo *position);

/* The collider manager, a sphere collider for a character info and its release, and the attack
 * collision of a collider (behaviours inferred). */
void *fn_80021B08(void);
boost::shared_ptr<CharaColliBase> fn_800C7DEC(void *manager, const boost::weak_ptr<CharInfo> &owner,
                                              int flags, int arg3, int arg4, const Vec3f *offset,
                                              float radius);
void fn_800C78B0(void *manager, boost::shared_ptr<CharaColliBase> *collider);
boost::weak_ptr<CharaColliAttack> fn_800C4374(void *manager,
                                              boost::shared_ptr<CharaColliBase> collider);

/* The collision world, a sphere, a query of the colliders a sphere touches and a sweep of a sphere
 * along a vector, their result lists' release and first contact, and a contact shape's normal and
 * point (behaviours inferred). */
void *fn_80020D40(void);
void fn_80021C04(UnkSphere_80021C04 *sphere, const Vec3f *center, float radius);
UnkHits_80021CA4 *fn_80021CA4(void *world, const UnkSphere_80021C04 *sphere, int arg2, int arg3,
                              bool (*filter)(UnkCollisionCandidate_80021934 *candidate),
                              CharaColliBase *self, int arg6);
UnkHits_80021CA4 *fn_80021934(void *world, UnkSphere_80021C04 *sphere, Vec3f *move, int arg3,
                              int arg4, UnkHits_80021CA4 *ignore,
                              bool (*filter)(UnkCollisionCandidate_80021934 *candidate),
                              CharaColliBase *self, int arg8);
void fn_80026FB4(UnkHits_80021CA4 *hits, int arg1);
UnkContact_8001EE04 *fn_8001EE04(UnkHits_80021CA4 *hits);
const Vec3f *fn_8001E9C4(UnkContactShape_8001E9BC *shape);
const Vec3f *fn_8001E9BC(UnkContactShape_8001E9BC *shape);
boost::weak_ptr<CharaColliBase> fn_800587A0(UnkContact_8001EE04 *contact);
boost::weak_ptr<CharInfo> fn_800586EC(CharaColliBaseCharInfo *collider);

/* Explosions: with the character that set it off, and without (behaviours inferred). */
boost::weak_ptr<CharInfo> fn_800B75DC(const boost::weak_ptr<CharInfo> &character);
void fn_801DD040(const boost::weak_ptr<CharInfo> &attacker, PickupCharInfo *charInfo, int kind,
                 int arg3, float power);
void fn_801DD0BC(PickupCharInfo *charInfo, int kind, int arg2, float power);

/* The camera manager, a camera's position and a camera shake (behaviours inferred). */
UnkCameraManager_80009548 *fn_80009548(void);
const Vec3f *fn_80010244(UnkCameraManager_80009548 *manager, int index);
void fn_8000D8EC(UnkCameraManager_80009548 *manager, int arg1, int arg2, float strength);

/* A rumble (see Vibration.h) and a sound at a character info (behaviours inferred). */
boost::weak_ptr<Vibration::RequestHandle> fn_8034DE08(int pattern, PickupCharInfo *charInfo,
                                                      int arg2);
void *fn_80053164(void);
void fn_801D54F4(void *manager, int id, PickupCharInfo *source, int arg3, int arg4);

/* Degrees to radians (behaviour inferred). */
float fn_8005AE20(float degrees);

/* Vector and matrix helpers (behaviours inferred): vector * scale, a + b, vector += value, the
 * negation of a vector, the dot and cross products, the parts of vector along and across axis,
 * the blend of a towards b, matrix * vector, the translation of a matrix, a * b, matrix
 * translated by (x, y, z), and moving position by velocity, which acceleration changes, over
 * dt. */
Vec3f fn_8040CDA8(const Vec3f *vector, float scale);
Vec3f fn_8040CDD0(const Vec3f *a, const Vec3f *b);
void fn_8040CFF8(Vec3f *vector, const Vec3f *value);
Vec3f fn_80016284(const Vec3f *vector);
float fn_8040CF90(const Vec3f *a, const Vec3f *b);
Vec3f fn_8040CFB8(const Vec3f *a, const Vec3f *b);
void fn_8040F990(Vec3f *along, Vec3f *across, const Vec3f *vector, const Vec3f *axis);
Vec3f fn_8040F268(const Vec3f *a, const Vec3f *b, float t);
void fn_8040CE38(Vec3f *vector, float scale);
void fn_8040CE60(Vec3f *vector);
Vec3f fn_8040B618(const Mtx34f *matrix, const Vec3f *vector);
Vec3f fn_800496F8(const Mtx34f *matrix);
Mtx34f fn_8001E044(const Mtx34f *a, const Mtx34f *b);
void fn_8040B52C(Mtx34f *matrix, float x, float y, float z);
void fn_804103E8(Vec3f *position, Vec3f *velocity, const Vec3f *acceleration, float scale,
                 float dt);
/* A blend of from and to along an arc of height up (behaviour inferred). */
Vec3f fn_800930AC(const Vec3f *from, const Vec3f *to, const Vec3f *up, float t);
/* The quaternion of Euler angles in the axis order order, and back (behaviours inferred). */
Quatf fn_804102F4(const Vec3f *angles, int order);
Vec3f fn_80410204(const Quatf *q, int order);
}

/* vector += value (name inferred; see Bomb.cpp). */
inline void AddVector(Vec3f &vector, const Vec3f &value) {
    fn_8040CFF8(&vector, &value);
}

/* The vector type's out-of-line assignment (fn_800091D0), called with temporaries the way the
 * original's operator is (name inferred; as in GadgetUtil.h). */
inline void AssignVector(Vec3f &vector, const Vec3f &value) {
    fn_800091D0(&vector, &value);
}

/* Copies a rotation one angle at a time (name inferred; as in Bomb.cpp). */
inline void CopyRotation(Vec3f &rotation, const Vec3f &value) {
    rotation.x = value.x;
    rotation.y = value.y;
    rotation.z = value.z;
}

/* fn_8040CD74 called as returning a - b by value, and fn_8040C148 as returning a copy of a matrix
 * (they store the result through their first argument; as in VacuumContainer.cpp). */
typedef Vec3f (*VectorSubtractFn)(const Vec3f *a, const Vec3f *b);
typedef Mtx34f (*MatrixCopyFn)(const Mtx34f *matrix);

inline Vec3f operator*(const Vec3f &vector, float scale) {
    return fn_8040CDA8(&vector, scale);
}

/* A length (name inferred). As a call it is not a constant expression, so the constants below are
 * stored by a static initializer, as in retail (as in CityBombSmall.cpp). */
inline float Span(float length) {
    return length;
}

/* The bomb's radius and its gravity (names inferred). */
static const float s_radius = Span(3.1999998f);
static const float s_gravity = Span(-320.0f);

/* A flag word (struct name unknown; as in CityBombSmall.cpp). */
struct ThrowBombFlags {
    ThrowBombFlags() {
        fn_80014E04(&m_bits);
    }
    void Clear() {
        fn_80014E10(&m_bits);
    }
    void Set(int bit) {
        fn_80014E1C(&m_bits, bit);
    }
    bool Test(int bit) {
        return fn_80014E34(&m_bits, bit);
    }

    u32 m_bits;
};

class ThrowBombImpl : public ThrowBomb, public boost::enable_shared_from_this<ThrowBombImpl> {
  public:
    ThrowBombImpl(int kind);
    virtual void SetMatrix(const Mtx34f &matrix);
    virtual void Throw(const UnkEffectParam_80194C1C &param);
    virtual void Remove();
    virtual void SetAlpha(float alpha);
    virtual float GetAlpha();
    virtual ~ThrowBombImpl();

    /* Method names are inferred. */
    void SetTask(Task *task) {
        m_task = task;
    }
    /* The gravity's acceleration. */
    Vec3f GetGravity();

    void Update(float dt);
    void UpdateColor();
    void SetPosition(const Vec3f &position);
    void SetRotation(const Vec3f &rotation);
    void UpdateMatrix(const Vec3f &position, const Vec3f &rotation);
    const Vec3f &GetPosition();
    const Vec3f &GetRotation();
    Vec3f RandomSpin();
    void StartHitTest();
    void LetGo(const Vec3f &velocity);
    void CheckHit(float dt);
    void Fly(float dt);
    void OnCommand(CharCommand &command);
    void OnSetGrabPosition(PickupCharInfo::SetGrabPositionArgs &args);
    bool TickTimer(float dt);
    void Init(int kind);
    void CreatePickup();
    void CreateCollider();
    void OnAttack(AttackCallbackParam &param);
    void Explode();
    void Finish();

    /* The hit test's filter (name inferred). */
    static bool HitFilter(UnkCollisionCandidate_80021934 *candidate);

    /* 0x04: the enable_shared_from_this base */
    /* 0x0C */ Task *m_task;
    /* 0x10 */ s32 m_state; /* 0 new, 1 flying, 2 picked up, 3 carried, 4 vacuumed */
    /* 0x14 */ ThrowBombFlags m_flags;
    /* 0x18 */ ThrowBombFlags m_hitFlags; /* bit 0: it bounced */
    /* 0x1C */ UnkModel_801A3E9C m_model;
    /* 0x24 */ boost::shared_ptr<PickupCharInfo> m_chara;
    /* 0x2C */ boost::shared_ptr<CharaColliBase> m_collider;
    /* 0x34 */ boost::weak_ptr<CharInfo> m_owner; /* the character that picked it up last */
    /* 0x3C */ float m_gravity;
    /* 0x40 */ Vec3f m_gravityDir;
    /* 0x4C */ Vec3f m_lastPosition;
    /* 0x58 */ Vec3f m_pickupFrom;
    /* 0x64 */ Vec3f m_pickupTo;
    /* 0x70 */ Vec3f m_spin;
    /* 0x7C */ Mtx34f m_handMatrix;
    /* 0xAC */ float m_timer;
    /* 0xB0 */ float m_power;
    /* 0xB4 */ float m_pickupTime;
    /* 0xB8 */ float m_pickupDuration;
    /* 0xBC */ float m_alpha;
    /* 0xC0 */ boost::weak_ptr<Effect::System::EffectElement> m_effect;
    /* 0xC8 */ UnkHits_80021CA4 *m_hits;
    /* 0xCC */ s32 m_explosion;
    /* 0xD0 */ u8 m_exploded;
    /* 0xD1 */ u8 m_armed; /* explodes at the first bounce */
};
SIZE_ASSERT(ThrowBombImpl, 0xD4);

/* The task that drives a bomb (class name from RTTI). */
class ThrowBombTask : public Task {
  public:
    ThrowBombTask(Task *parent, const boost::shared_ptr<ThrowBombImpl> &bomb);
    virtual ~ThrowBombTask();
    virtual void Tick(float dt);

    /* 0x28 */ boost::shared_ptr<ThrowBombImpl> m_bomb;
};
SIZE_ASSERT(ThrowBombTask, 0x30);

/* The bomb while a vacuum pulls it in (class name from RTTI; names inferred). */
class VacuumThrowBomb : public VacuumObject {
  public:
    VacuumThrowBomb(VacuumCommand &command, const boost::weak_ptr<ThrowBombImpl> &bomb);
    virtual ~VacuumThrowBomb();
    virtual void Tick(float dt);
    virtual void fn_80238184(const Vec3f &velocity);
    virtual void fn_80238180();
    virtual bool fn_80238088();
    virtual Mtx34f fn_80237EE4();

    /* Hands bomb to a new VacuumThrowBomb and starts a task for it. */
    static void Start(VacuumCommand &command, const boost::weak_ptr<ThrowBombImpl> &bomb);

    /* 0x08 */ boost::weak_ptr<ThrowBombImpl> m_bomb;
};
SIZE_ASSERT(VacuumThrowBomb, 0x10);

/* Instantiates the control block class of shared_ptr<ThrowBombImpl> ahead of the others, which gives
 * retail's order of their dispose and get_deleter instances (a workaround; the original's code for
 * this is unknown). */
typedef char ForceT[sizeof(boost::detail::sp_counted_base_impl<ThrowBombImpl *, boost::checked_deleter<ThrowBombImpl> >)];

/* The stages' groups (name inferred). */
inline int GetStageGroup(int id) {
    switch (id) {
    case 300:
    case 301:
        return 0;
    case 501:
    case 602:
        return 1;
    case 612:
    case 614:
    case 615:
        return 2;
    default:
        return -1;
    }
}

bool ThrowBombImpl::HitFilter(UnkCollisionCandidate_80021934 *candidate) {
    if (candidate->m_collider != 0 && candidate->m_collider == candidate->m_self) {
        return false;
    }
    if (candidate->m_attributes & 0x140000) {
        return false;
    }
    return true;
}

ThrowBombImpl::ThrowBombImpl(int kind) : m_task(0), m_hits(0), m_exploded(0), m_armed(0) {
    fn_8040CE60(&m_spin);
    m_state = 0;
    m_flags.Clear();
    m_hitFlags.Clear();
    m_timer = 0.0f;
    m_power = 4.0f;
    m_alpha = 1.0f;
    CreatePickup();
    fn_8040CE60(&m_gravityDir);
    m_gravityDir.y = 1.0f;
    m_gravity = s_gravity;
    if (kind == 2) {
        m_explosion = 3;
    } else {
        m_explosion = 0;
    }
    Init(kind);
}

ThrowBombImpl::~ThrowBombImpl() {
    if (m_hits != 0) {
        fn_80026FB4(m_hits, 1);
        m_hits = 0;
    }
    if (boost::shared_ptr<Effect::System::EffectElement> effect = m_effect.lock()) {
        effect->Stop();
        m_effect.reset();
    }
    if (m_collider.px != 0) {
        fn_800C78B0(fn_80021B08(), &m_collider);
    }
}

void ThrowBombImpl::Update(float dt) {
    if (TickTimer(dt)) {
        Explode();
        Finish();
        return;
    }
    UpdateColor();
    switch (m_state) {
    case 0:
        return;
    case 1:
        Fly(dt);
        UpdateMatrix(m_chara->m_position, m_chara->m_rotation);
        break;
    case 2:
        m_timer = 10.0f;
        if ((m_pickupTime += dt) >= m_pickupDuration) {
            fn_800091D0(&m_chara->m_position, &m_pickupTo);
            boost::shared_ptr<CharInfo> owner = m_owner.lock();
            if (owner.get() != 0) {
                owner->PushCommand(new ReadyCommand(m_chara));
            }
            m_state = 3;
        } else {
            float t = m_pickupTime / m_pickupDuration;
            fn_800091D0(&m_chara->m_position,
                        &(const Vec3f &)fn_800930AC(&m_pickupFrom, &m_pickupTo,
                                                    &(const Vec3f &)(m_gravityDir * -100.0f), t));
        }
        UpdateMatrix(m_chara->m_position, m_chara->m_rotation);
        break;
    case 3:
        m_timer = 10.0f;
        break;
    case 4:
        m_timer = 10.0f;
        break;
    }
    fn_801E413C(m_chara.px, m_chara.px);
}

void ThrowBombImpl::UpdateColor() {
    static const float s_color[4] = {1.0f, 0.0f, 0.0f, 0.2f};

    float timer = m_timer;
    if (timer > 5.0f || m_state == 0) {
        fn_801A395C(&m_model, 1.0f, 1.0f, 1.0f, m_alpha);
    } else if (timer < 1.0f) {
        if (lbl_8057D6A0.m_frame & 1) {
            fn_801A395C(&m_model, s_color[0], s_color[1], s_color[2], s_color[3] * m_alpha);
        } else {
            fn_801A395C(&m_model, s_color[0], s_color[1], s_color[2], m_alpha);
        }
    } else if (timer < 3.0f) {
        float alpha = std::cos(2.0f * (3.0f - timer) * lbl_805FAD38);
        alpha = 0.8f * (alpha * alpha) + 0.2f;
        alpha *= m_alpha;
        fn_801A395C(&m_model, s_color[0], s_color[1], s_color[2], alpha);
    } else {
        float alpha = std::cos((5.0f - timer) * lbl_805FAD38);
        alpha = 0.8f * (alpha * alpha) + 0.2f;
        alpha *= m_alpha;
        fn_801A395C(&m_model, s_color[0], s_color[1], s_color[2], alpha);
    }
}

void ThrowBombImpl::SetPosition(const Vec3f &position) {
    fn_800091D0(&m_chara->m_position, &position);
}

void ThrowBombImpl::SetRotation(const Vec3f &rotation) {
    CopyRotation(m_chara->m_rotation, rotation);
}

void ThrowBombImpl::UpdateMatrix(const Vec3f &position, const Vec3f &rotation) {
    Mtx34f matrix;

    fn_8040C148(&matrix, &lbl_805E0720);
    fn_8040B59C(&matrix, &position);
    fn_8040B848(&matrix, rotation.y);
    fn_8040B884(&matrix, rotation.x);
    fn_8040B80C(&matrix, rotation.z);
    SetMatrix(matrix);
}

/* The effects of the bomb's explosion by stage group, and the model frame it follows (names
 * inferred). */
struct ThrowBombEffect {
    s32 m_effect;
    s32 m_frame;
};

static const ThrowBombEffect s_effects[3] = {
    {0x20D, 2000},
    {0x20A, 3000},
    {4, -1},
};

void ThrowBombImpl::SetMatrix(const Mtx34f &matrix) {
    if (m_model.m_frameCount > 0) {
        RwFrame *frame = fn_801A3750(&m_model);
        if (frame != 0) {
            RwMatrix rwMatrix;
            rwMatrix.right.x = matrix.values[0][0];
            rwMatrix.right.y = matrix.values[1][0];
            rwMatrix.right.z = matrix.values[2][0];
            rwMatrix.up.x = matrix.values[0][1];
            rwMatrix.up.y = matrix.values[1][1];
            rwMatrix.up.z = matrix.values[2][1];
            rwMatrix.at.x = matrix.values[0][2];
            rwMatrix.at.y = matrix.values[1][2];
            rwMatrix.at.z = matrix.values[2][2];
            rwMatrix.pos.x = matrix.values[0][3];
            rwMatrix.pos.y = matrix.values[1][3];
            rwMatrix.pos.z = matrix.values[2][3];
            rwMatrix.pad3 = 0;
            rwMatrix.pad2 = 0;
            rwMatrix.pad1 = 0;
            rwMatrix.flags = 0;
            fn_8047EBA0(&rwMatrix);
            fn_804876E8(frame, &rwMatrix, 0);
            fn_8048722C(frame);
        }
    }
    if (GetStageGroup(lbl_8057D6A0.m_stageId) == -1) {
        return;
    }
    if (boost::shared_ptr<Effect::System::EffectElement> effect = m_effect.lock()) {
        if (s_effects[GetStageGroup(lbl_8057D6A0.m_stageId)].m_frame >= 0) {
            RwFrame *frame =
                fn_801A3704(&m_model, s_effects[GetStageGroup(lbl_8057D6A0.m_stageId)].m_frame);
            if (frame != 0) {
                RwMatrix *frameMatrix = &frame->modelling;
                if (frameMatrix != 0) {
                    Mtx34f local;
                    fn_8040C148(&local, &(const Mtx34f &)fn_800635A4(frameMatrix));
                    Vec3f position;
                    fn_800091EC(&position, &(const Vec3f &)fn_800496F8(
                                               &(const Mtx34f &)fn_8040B8FC(&matrix, &local)));
                    fn_80425E4C(effect.get(), &position);
                }
            }
        } else {
            fn_80425E68(effect.get(), &matrix);
        }
    }
}

const Vec3f &ThrowBombImpl::GetPosition() {
    return m_chara->m_position;
}

const Vec3f &ThrowBombImpl::GetRotation() {
    return m_chara->m_rotation;
}

Vec3f ThrowBombImpl::RandomSpin() {
    return Vec3f(fn_8040EC80(-20.0f, 20.0f), fn_8040EC80(-20.0f, 20.0f), 0.0f);
}

void ThrowBombImpl::StartHitTest() {
    if (m_hits != 0) {
        fn_80026FB4(m_hits, 1);
        m_hits = 0;
    }
    UnkSphere_80021C04 sphere;
    fn_80021C04(&sphere, &m_chara.px->m_position, s_radius);
    m_hits = fn_80021CA4(fn_80020D40(), &sphere, 2, 1, HitFilter, m_collider.px, 0);
}

void ThrowBombImpl::Throw(const UnkEffectParam_80194C1C &param) {
    Mtx34f matrix;

    fn_8040B6F8(&matrix);
    fn_8040B59C(&matrix, &param.m_position);
    SetMatrix(matrix);
    fn_800091D0(&m_chara->m_position, &param.m_position);
    fn_800091D0(&m_chara->field_0x18, &param.m_velocity);
    m_timer = param.m_scale;
    m_power = param.field_0x1C;
    CreateCollider();
    if (param.m_flag) {
        m_armed = 1;
    }
    StartHitTest();
    m_chara->m_target->TurnOn();
    m_state = 1;
    AssignVector(m_spin, RandomSpin());
}

void ThrowBombImpl::Remove() {
    Finish();
}

void ThrowBombImpl::SetAlpha(float alpha) {
    m_alpha = alpha;
}

float ThrowBombImpl::GetAlpha() {
    return m_alpha;
}

void ThrowBombImpl::LetGo(const Vec3f &velocity) {
    fn_800091D0(&m_chara->field_0x18, &velocity);
    AssignVector(m_spin, RandomSpin());
    m_armed = 1;
    m_collider->TurnOn(-1);
    m_chara->m_target->TurnOn();
    StartHitTest();
    m_state = 1;
}

void ThrowBombImpl::CheckHit(float dt) {
    UnkSphere_80021C04 sphere;
    fn_80021C04(&sphere, &m_lastPosition, s_radius);
    Vec3f move;
    fn_800091EC(&move, &(const Vec3f &)((VectorSubtractFn)fn_8040CD74)(&m_chara->m_position,
                                                                         &m_lastPosition));
    int hits = 0;
    UnkHits_80021CA4 *hit;
    bool sound = true;
    static int s_maxHits = (int)Span(4.0f);
    for (int i = 0; i < s_maxHits; i++) {
        Vec3f start;
        fn_800091EC(&start, &move);
        hit = fn_80021934(fn_80020D40(), &sphere, &move, 3, 1, m_hits,
                                            HitFilter, m_collider.px, 1);
        if (hit == 0) {
            break;
        }
        UnkContact_8001EE04 *contact = fn_8001EE04(hit);
        UnkContactShape_8001E9BC *shape = contact->m_shape;
        if (shape != 0) {
            Vec3f normal;
            fn_800091EC(&normal, fn_8001E9C4(shape));
            Vec3f point;
            fn_800091EC(&point, fn_8001E9BC(shape));
            fn_800091D0((Vec3f *)sphere.center,
                        &(const Vec3f &)fn_8040CDD0(
                            &point, &(const Vec3f &)fn_8040CDA8(&normal, 0.001f + (0.001f + sphere.radius))));
            Vec3f travelled;
            fn_800091EC(&travelled, &(const Vec3f &)((VectorSubtractFn)fn_8040CD74)(&start, &move));
            Vec3f along;
            Vec3f across;
            fn_8040F990(&along, &across, &m_chara->field_0x18, &normal);
            fn_8040CE38(&across, 0.95f);
            fn_8040CE38(&along, 0.5f);
            Vec3f moveAlong;
            Vec3f moveAcross;
            fn_8040F990(&moveAlong, &moveAcross, &travelled, &normal);
            static float s_minBounce = Span(10.0f);
            float fall = fn_8040CF90(&normal, &(const Vec3f &)(GetGravity() * dt));
            if (fn_8040CF50(&along) < s_minBounce - fall) {
                fn_8040CE60(&along);
                fn_8040CE60(&moveAlong);
                sound = false;
            }
            fn_800091D0(&m_chara.px->field_0x18,
                        &(const Vec3f &)fn_8040CDD0(&across, &(const Vec3f &)fn_80016284(&along)));
            {
                boost::weak_ptr<CharaColliBase> other = fn_800587A0(contact);
                if (!other.expired()) {
                    CharaColliBaseCharInfo *info =
                        dynamic_cast<CharaColliBaseCharInfo *>(other.lock().get());
                    if (info != 0 && !fn_800586EC(info).expired()) {
                        boost::shared_ptr<CharInfo> character = fn_800586EC(info).lock();
                        if (*(s32 *)&character->field_0x3C[0x50 - 0x3C] == 0x98) {
                            character->field_0x18.x -= m_chara->field_0x18.x;
                            character->field_0x18.z -= m_chara->field_0x18.z;
                        }
                    }
                }
            }
            Vec3f axis;
            fn_800091EC(&axis, &(const Vec3f &)fn_8040CFB8(&across, &(const Vec3f &)fn_80016284(&normal)));
            AssignVector(m_spin, fn_8040F268(&m_spin, &axis, 0.6f));
            AssignVector(move, fn_8040CDD0(&moveAcross, &(const Vec3f &)fn_80016284(&moveAlong)));
        }
        fn_80026FB4(hit, 1);
        hits++;
    }
    if (hits != 0) {
        m_hitFlags.Set(0);
        if (m_armed) {
            m_timer = 0.0f;
        }
        if (m_hits != 0) {
            fn_80026FB4(m_hits, 1);
            m_hits = 0;
        }
        fn_800091D0(&m_chara.px->m_position, &(const Vec3f &)fn_8040CDD0((Vec3f *)sphere.center, &move));
        if (sound) {
            fn_801D54F4(fn_80053164(), 0x6038, m_chara.px, 0, 1);
        }
    }
}

Vec3f ThrowBombImpl::GetGravity() {
    return m_gravityDir * m_gravity;
}

void ThrowBombImpl::Fly(float dt) {
    AddVector(m_chara->field_0x24, GetGravity());
    fn_800091D0(&m_lastPosition, &m_chara->m_position);
    fn_804103E8(&m_chara->m_position, &m_chara->field_0x18, &m_chara->field_0x24, 1.0f, dt);
    fn_8040CE38(&m_spin, 0.98f);
    Quatf rotation(fn_804102F4(&m_chara->m_rotation, 2));
    Quatf spin(m_spin.x, m_spin.y, m_spin.z, 0.0f);
    fn_8040CAB4(&rotation, &(const Quatf &)(spin * rotation * 0.5f * dt));
    fn_8040C958(&rotation);
    CopyRotation(m_chara->m_rotation, fn_80410204(&rotation, 2));
    m_hitFlags.Clear();
    CheckHit(dt);
    fn_8040CE60(&m_chara.px->field_0x24);
}

void ThrowBombImpl::OnCommand(CharCommand &command) {
    switch (command.TypeId()) {
    case 0x11:
        if (m_state == 1) {
            PickupCommand &pickup = static_cast<PickupCommand &>(command);
            m_pickupTime = 0.0f;
            fn_800091D0(&m_gravityDir, &pickup.field_0x2C);
            m_gravity = s_gravity;
            m_pickupDuration = pickup.field_0x20;
            fn_800091D0(&m_pickupFrom, &m_chara->m_position);
            fn_800091D0(&m_pickupTo,
                        &(const Vec3f &)fn_8040CDD0(&pickup.field_0x14,
                                                    &(const Vec3f &)fn_8040CDA8(&pickup.field_0x2C, s_radius)));
            m_owner = pickup.m_character;
            m_chara->m_target->TurnOff();
            m_collider->TurnOff(-1);
            m_state = 2;
        }
        break;
    case 0x13:
        if (m_state >= 2) {
            fn_800091D0(&m_gravityDir, &static_cast<ThrowCommand &>(command).m_direction);
            m_gravity = s_gravity;
            fn_8040CE60(&m_chara.px->field_0x24);
            Vec3f offset(0.0f, fn_8005AE20(10.0f), fn_8005AE20(-20.0f));
            fn_800091D0(&m_chara->field_0x18, &(const Vec3f &)fn_8040B618(&m_handMatrix, &offset));
            AssignVector(m_spin, RandomSpin());
            m_collider->TurnOn(-1);
            m_armed = 1;
            StartHitTest();
            m_state = 1;
        }
        break;
    case 0x12:
        if (m_state >= 2) {
            fn_800091D0(&m_gravityDir, &static_cast<CastCommand &>(command).m_direction);
            m_gravity = s_gravity;
            m_owner.reset();
            fn_8040CE60(&m_chara.px->field_0x24);
            Vec3f offset(0.0f, fn_8005AE20(10.0f), fn_8005AE20(0.0f));
            fn_800091D0(&m_chara->field_0x18, &(const Vec3f &)fn_8040B618(&m_handMatrix, &offset));
            AssignVector(m_spin, RandomSpin());
            m_collider->TurnOn(-1);
            m_chara->m_target->TurnOn();
            StartHitTest();
            m_state = 1;
        }
        break;
    case 0x14:
        if (m_state == 1) {
            VacuumThrowBomb::Start(static_cast<VacuumCommand &>(command), shared_from_this());
            m_chara->m_target->TurnOff();
            m_collider->TurnOff(-1);
            m_state = 4;
        }
        break;
    }
    command.field_0x4[3] |= 2;
}

void ThrowBombImpl::OnSetGrabPosition(PickupCharInfo::SetGrabPositionArgs &args) {
    fn_8040C148(&m_handMatrix, args.m_matrix);
    fn_8040B52C(&m_handMatrix, 0.0f, s_radius, 0.0f);
    if (m_state == 3) {
        fn_800091D0(&m_chara->m_position, &(const Vec3f &)fn_800496F8(&m_handMatrix));
        Mtx34f matrix;
        fn_8040B6F8(&matrix);
        fn_8040B848(&matrix, m_chara.px->m_rotation.y);
        fn_8040B884(&matrix, m_chara.px->m_rotation.x);
        fn_8040B80C(&matrix, m_chara.px->m_rotation.z);
        fn_8040C148(&matrix, &(const Mtx34f &)fn_8001E044(&m_handMatrix, &matrix));
        SetMatrix(matrix);
    }
}

bool ThrowBombImpl::TickTimer(float dt) {
    if (m_state >= 1 && !lbl_8057D6A0.m_paused) {
        if ((m_timer -= dt) < 0.0f) {
            return true;
        }
    }
    return false;
}

void ThrowBombImpl::Init(int kind) {
    switch (kind) {
    case 0: {
        static const s32 s_data[2] = {0x7FFFF, 0x807D0};
        UnkModelParam_801A3CE4 param;
        param.m_data = s_data;
        param.m_count = 2;
        fn_801A3CE4(&m_model, 8, &param, 0);
        break;
    }
    case 1:
        fn_801A3CA4(&m_model, 0x14, 3, 0);
        if (GetStageGroup(lbl_8057D6A0.m_stageId) != -1) {
            m_effect = fn_801D2BB0(0, s_effects[GetStageGroup(lbl_8057D6A0.m_stageId)].m_effect, 0);
        }
        break;
    case 2:
        if (GetStageGroup(lbl_8057D6A0.m_stageId) != -1) {
            m_effect = fn_801D2BB0(3, s_effects[GetStageGroup(lbl_8057D6A0.m_stageId)].m_effect, 0);
        }
        break;
    }
}

void ThrowBombImpl::CreatePickup() {
    m_chara = boost::shared_ptr<PickupCharInfo>(new PickupCharInfo(0x98));
    fn_800729B0(&m_chara.px->m_commandUnit->m_callback,
                &(const CharCommandCallback &)CharCommandCallback(
                    boost::bind(&ThrowBombImpl::OnCommand, this, _1)));
    fn_801E4160(m_chara.px, m_chara, 0, 30.0f);
    fn_801DC984(m_chara.px, "ThrowBomb");
    m_chara->SetKind(4);
    m_chara.px->field_0x30 = 2;
    m_chara.px->m_grabCallback = boost::function1<void, PickupCharInfo::SetGrabPositionArgs &>(
        boost::bind(&ThrowBombImpl::OnSetGrabPosition, this, _1));
    u32 flags = m_chara.px->m_target.px->m_flags;
    m_chara->m_target->SetFlags(flags | 0x80);
    m_chara->m_target->TurnOff();
}

void ThrowBombImpl::CreateCollider() {
    m_collider = fn_800C7DEC(fn_80021B08(), m_chara, 0x80, 7, 0, &lbl_805E07A0, s_radius);
    if (m_collider.px != 0) {
        boost::weak_ptr<CharaColliAttack> attack = fn_800C4374(fn_80021B08(), m_collider);
        if (boost::shared_ptr<CharaColliAttack> collision = attack.lock()) {
            collision->field_0x54 = 0;
            collision->field_0x55 = 1;
            collision->m_attackCallback =
                AttackCallback(boost::bind(&ThrowBombImpl::OnAttack, this, _1));
        }
    }
}

void ThrowBombImpl::OnAttack(AttackCallbackParam &param) {
    if (m_state == 1) {
        if (boost::shared_ptr<CharaColliAttack> collision = param.m_attacker.lock()) {
            if (collision->m_flags & 0x200) {
                param.m_result = 3;
                Explode();
                Finish();
            }
        }
    }
    param.m_result = 5;
}

void ThrowBombImpl::Explode() {
    if (!m_exploded) {
        m_exploded = 1;
        if (!m_owner.expired()) {
            fn_801DD040(fn_800B75DC(m_owner), m_chara.px, m_explosion, 7, m_power);
        } else {
            fn_801DD0BC(m_chara.px, m_explosion, 7, m_power);
        }
        if (fn_8040CF2C(&(const Vec3f &)((VectorSubtractFn)fn_8040CD74)(
                fn_80010244(fn_80009548(), 0), &m_chara.px->m_position)) < 40000.0f) {
            fn_8000D8EC(fn_80009548(), 0, 0, 0.5f);
        }
        fn_8034DE08(8, m_chara.px, -2);
        fn_801D54F4(fn_80053164(), 0x6039, m_chara.px, 0, 1);
    }
}

void ThrowBombImpl::Finish() {
    if (m_task != 0) {
        m_task->m_flags |= 1;
    }
}

/* The task's name. Unused (the task uses the literal), but it puts the string in .rodata ahead of
 * the RTTI names, where retail has it; the linker drops the pointer. */
static const char *s_taskName = "ThrowBomb";

ThrowBombTask::ThrowBombTask(Task *parent, const boost::shared_ptr<ThrowBombImpl> &bomb)
    : Task(parent), m_bomb(bomb) {
    m_name = "ThrowBomb";
    m_bomb->SetTask(this);
}

ThrowBombTask::~ThrowBombTask() {
    m_bomb->SetTask(0);
}

void ThrowBombTask::Tick(float dt) {
    m_bomb.px->Update(dt);
}

/* The deleters the shared_ptr constructors below copy. GC/2.7 lays the compiler's own copies out in
 * a different order than retail's, so they are spelled out (as in StageManager.cpp). */
static boost::checked_deleter<ThrowBombImpl> s_bombDeleter;
static boost::checked_deleter<VacuumThrowBomb> s_vacuumDeleter;

namespace boost {

/* The shared_ptr constructors (boost 1.32's), copying the deleters above. */
template <> template <>
inline shared_ptr<ThrowBombImpl>::shared_ptr(ThrowBombImpl *p) : px(p), pn(p, s_bombDeleter) {
    detail::sp_enable_shared_from_this(pn, p, p);
}

template <> template <>
inline shared_ptr<VacuumObject>::shared_ptr(VacuumThrowBomb *p) : px(p), pn(p, s_vacuumDeleter) {
    detail::sp_enable_shared_from_this(pn, p, p);
}

} // namespace boost

extern "C" void fn_80209B58(Task *layer, int kind, const UnkEffectParam_80194C1C &param) {
    boost::shared_ptr<ThrowBombImpl> bomb = boost::shared_ptr<ThrowBombImpl>(new ThrowBombImpl(kind));
    bomb->Throw(param);
    new ThrowBombTask(layer, bomb);
}

extern "C" boost::weak_ptr<ThrowBomb> fn_80209908(Task *layer, int kind) {
    boost::shared_ptr<ThrowBombImpl> bomb = boost::shared_ptr<ThrowBombImpl>(new ThrowBombImpl(kind));
    new ThrowBombTask(layer, bomb);
    return bomb;
}

void VacuumThrowBomb::Start(VacuumCommand &command, const boost::weak_ptr<ThrowBombImpl> &bomb) {
    boost::shared_ptr<VacuumObject> object =
        boost::shared_ptr<VacuumObject>(new VacuumThrowBomb(command, bomb));
    object->fn_802382E0(bomb.lock()->GetPosition());
    object->fn_80238268(bomb.lock()->GetRotation());
    new VacuumObjectTask(object);
}

VacuumThrowBomb::VacuumThrowBomb(VacuumCommand &command, const boost::weak_ptr<ThrowBombImpl> &bomb)
    : VacuumObject(fn_80237E34(&command), command) {
    m_bomb = bomb;
}

VacuumThrowBomb::~VacuumThrowBomb() {}

void VacuumThrowBomb::Tick(float dt) {
    VacuumObject::Tick(dt);
    if (boost::shared_ptr<ThrowBombImpl> bomb = m_bomb.lock()) {
        bomb->SetPosition(fn_802382B0(this));
        bomb->SetRotation(fn_80238248(this));
        bomb->UpdateMatrix(fn_802382B0(this), fn_80238248(this));
    }
}

void VacuumThrowBomb::fn_80238184(const Vec3f &velocity) {
    fn_80014E1C(GetFlags(), 1);
    if (boost::shared_ptr<ThrowBombImpl> bomb = m_bomb.lock()) {
        bomb->LetGo(velocity);
    }
}

void VacuumThrowBomb::fn_80238180() {
    if (boost::shared_ptr<ThrowBombImpl> bomb = m_bomb.lock()) {
        bomb->Remove();
    }
}

Mtx34f VacuumThrowBomb::fn_80237EE4() {
    Mtx34f matrix;
    fn_8040C148(&matrix, &(const Mtx34f &)CalcMatrix());
    Vec3f offset(0.0f, 0.0f, s_radius);
    fn_8040B59C(&matrix, &offset);
    return ((MatrixCopyFn)fn_8040C148)(&matrix);
}

bool VacuumThrowBomb::fn_80238088() {
    bool alive = false;
    if (VacuumObject::fn_80238088() && !m_bomb.expired()) {
        alive = true;
    }
    return alive;
}
