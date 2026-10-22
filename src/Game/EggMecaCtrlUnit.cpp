/* The control units of the Egg Breaker boss (class names from RTTI: EggMecaCtrlUnit and its
 * EggMecaCtrlUnit_* subclasses). Each unit is a BossCtrlUnit that the boss task runs on its
 * EggMecaVarParam: the stage area, the aura, eye, Eggman and canopy models, the iron ball and the
 * shock wave where it lands, the bombs, the two lasers, the spin of the upper body, the motion
 * attributes, the camera, the turning, the body colliders' callbacks and the status update. File
 * name inferred from the classes; names marked "(name inferred)" and all member names are not
 * original. Definitions are in reverse address order (deferred inlining emits functions
 * last-defined first).
 *
 * The unit is NonMatching (251/256 functions): the ShockWave constructor (two registers swapped in
 * the arguments of fn_800C7EA8, as in DevilDoomCtrlUnit.cpp), Laser::GetAngles (retail reloads
 * m_index between its two tests, 4 bytes longer), IronBall::GetThrowVelocity (the stack slots of
 * its two-float vectors), AcceptIronBallHit (prologue scheduling) and FixLookCharCamera::IsEnabled
 * (float register numbering in its two interpolations). .data also differs in the zero-filled
 * vtables near its middle (retail has 0x1C more bytes before EggMecaCtrlUnit's vtable and the
 * zeroed BossCtrlUnit vtable after it). The boss's mode unit (EggMecaModeCtrlUnit) starts right
 * after this unit, at 0x802A7C9C with its in-class Tick. */
#include <boost/bind.hpp>
#include <boost/function.hpp>
#include <string>

#include "Effect/System/EffectElement.h"
#include "Game/BossCtrlUnit.h"
#include "Game/CharInfo.h"
#include "Game/CharaColli.h"
#include "Game/CharaColliAttack.h"
#include "Game/CharaColliBaseCharInfo.h"
#include "Game/EffectPJSLauncher.h"
#include "Game/EggMecaCtrlUnit.h"
#include "Game/EnemyCharInfo.h"
#include "Game/ExternalCameraRequest.h"
#include "Game/MathConstants.h"
#include "Game/MathTypes.h"
#include "Game/PJSCameraMode.h"
#include "Game/PooledList.h"
#include "Game/Vibration.h"
#include "boost/shared_ptr.hpp"
#include "boost/weak_ptr.hpp"
#include "size_assert.h"

/* MSL's std::sin, std::cos and std::fabs for floats, which the game calls out of line (as in
 * DevilDoomCtrlUnit.cpp). */
namespace std {
float sin(float x);
float cos(float x);
float fabs(float x);
} // namespace std

extern "C" double pow(double x, double y);

using Effect::System::EffectElement;
using Effect::System::EffectLaunchInfo;
using SonicteamUSA::System::Camera::CameraMode;
using SonicteamUSA::System::CharaColli::AttackCallback;
using SonicteamUSA::System::CharaColli::AttackCallbackParam;
using SonicteamUSA::System::CharaColli::CharaColliAttack;
using SonicteamUSA::System::CharaColli::CharaColliBase;
using SonicteamUSA::System::CharaColli::CharaColliBaseCharInfo;

/* What a motion passes to its callbacks (as in DevilDoomCtrlUnit.cpp). */
struct _stMtnCallbackParam {
    /* 0x0 */ void *m_frame;
    /* 0x4 */ u32 field_0x4;
    /* 0x8 */ s32 m_id;
};

/* The RenderWare types used here, declared locally (layouts as in rw/rwcore.h and
 * rw/rpworld.h, reduced to the members used; as in HeavyDogCtrlUnit.cpp). */
extern "C" {
typedef struct RwObject {
    u8 type;
    u8 subKind;
    u8 flags;
    u8 privateFlags;
    void *parent;
} RwObject;

typedef struct RwV3d {
    float x;
    float y;
    float z;
} RwV3d;

typedef struct RwMatrix {
    RwV3d right;
    u32 flags;
    RwV3d up;
    u32 pad1;
    RwV3d at;
    u32 pad2;
    RwV3d pos;
    u32 pad3;
} RwMatrix;

typedef struct RwFrame {
    /* 0x00 */ RwObject object;
    /* 0x08 */ u8 inDirtyListLink[0x8];
    /* 0x10 */ RwMatrix modelling;
} RwFrame;

typedef struct RpClump {
    /* 0x00 */ RwObject object; /* the parent is the clump's frame */
} RpClump;
}

/* A clump's colour fade (layout as in BossRender.cpp; the frame time at 0x28 inferred). */
struct UnkClumpFade_80027D5C {
    /* Sets the opacity (name inferred). */
    void SetAlpha(float value) {
        m_color[3] = value;
    }

    /* Sets the colour to grey level value (name inferred). */
    void SetColor(float value) {
        m_color[0] = value;
        m_color[1] = value;
        m_color[2] = value;
    }

    /* 0x00 */ float m_color[4];
    /* 0x10 */ u8 field_0x10[0x24 - 0x10];
    /* 0x24 */ u8 m_mode;
    /* 0x25 */ u8 field_0x25[0x28 - 0x25];
    /* 0x28 */ float m_dt;
};
SIZE_ASSERT(UnkClumpFade_80027D5C, 0x2C);

/* An object that keeps a clump on a motion (as in DevilDoomCtrlUnit.h; size from its
 * allocation). */
class UnkClumpUpdater_80015A7C {
  public:
    UnkClumpUpdater_80015A7C(RpClump *clump, _stMtnPlayWork *motion);
    ~UnkClumpUpdater_80015A7C();

    /* 0x00 */ u8 field_0x0[0x1C];
};

/* The parts of AttackCallbackParam the units read (as in DevilDoomCtrlUnit.cpp). */
struct EggMecaAttackParam {
    /* 0x00 */ boost::weak_ptr<CharaColliAttack> m_attacker;
    /* 0x08 */ u8 field_0x8[0x18 - 0x8];
    /* 0x18 */ Vec3f m_position;
    /* 0x24 */ Vec3f m_direction;
    /* 0x30 */ u8 field_0x30[0x34 - 0x30];
    /* 0x34 */ Vec3f m_offset;
    /* 0x40 */ u8 field_0x40[0x50 - 0x40];
    /* 0x50 */ float m_damage;
    /* 0x54 */ s32 m_result;
};

#define Param(param) (*(EggMecaAttackParam *)&(param))

/* The effect manager fn_800A31F0 returns (as in DevilDoomCtrlUnit.cpp). */
class UnkEffectManager_800A31F0 {
  public:
    virtual void vf08();
    virtual boost::weak_ptr<EffectElement> LaunchId(s32 id, s32 arg);
    virtual boost::weak_ptr<EffectElement> Launch(boost::shared_ptr<EffectLaunchInfo> info, s32 id);
};

/* The game state (struct name unknown; see AdvMemoryCheck.cpp): the frame times at 0x20 and 0x24
 * and the stage id at 0xA8. */
struct UnkGameState_8057D6A0 {
    /* 0x000 */ u8 field_0x0[0x20];
    /* 0x020 */ float m_frameTime;
    /* 0x024 */ float m_dt;
    /* 0x028 */ u8 field_0x28[0xA8 - 0x28];
    /* 0x0A8 */ s32 m_stage;
    /* 0x0AC */ u8 field_0xAC[0x150 - 0xAC];
};
extern UnkGameState_8057D6A0 lbl_8057D6A0;

/* The frame time the effects follow (name inferred). */
#define FrameTime() ((u32)&lbl_8057D6A0.m_dt)

/* The world of each stage area (as in DevilDoomCtrlUnit.cpp). */
struct UnkWorldEntry_8051D208 {
    /* 0x00 */ u8 field_0x0[0x10];
    /* 0x10 */ void *m_world;
    /* 0x14 */ u8 field_0x14[0x18 - 0x14];
};
extern UnkWorldEntry_8051D208 lbl_8051D208[];

/* The save data (see EggLastMech.cpp): the record at 0x70C keeps the score. */
struct UnkObject_80079808 {
    /* 0x000 */ u8 field_0x0[0x70C];
    /* 0x70C */ u8 m_score[1];
};

/* A sphere: centre and radius (built by fn_80021C04; as in MeteorsHolder.cpp). */
struct UnkSphere_80021C04 {
    /* 0x00 */ float center[3];
    /* 0x0C */ float radius;
};

/* What fn_80021934 passes its filter for each candidate hit (as in MeteorsHolder.cpp). */
struct UnkCollisionCandidate_80021934 {
    /* 0x00 */ u8 field_0x0[0x8];
    /* 0x08 */ CharaColliBase *m_collider;
    /* 0x0C */ u32 m_attributes;
    /* 0x10 */ CharaColliBase *m_self;
};

/* The hits of a sweep or a line (see MeteorsHolder.cpp and BlackBullCtrlUnit.cpp). */
struct CollisionResultList;

/* A line for fn_80021FE4 (see BlackBullCtrlUnit.cpp). */
struct UnkLine_80020A20 {
    u8 field_0x0[0x18];
};

/* A hit of a line and an iterator over the hits (layouts from their use here; the object hit is at
 * 0x14, as in BlackBullCtrlUnit.cpp). */
struct UnkLineHit_8001EE04 {
    /* 0x00 */ u8 field_0x0[0x14];
    /* 0x14 */ void *m_object;
};

struct UnkLineHitIter_8001EDFC {
    struct Unset {};

    UnkLineHitIter_8001EDFC() : m_node(0) {}
    UnkLineHitIter_8001EDFC(Unset) {}

    UnkLineHit_8001EE04 *m_node;
};

/* A collision hit (see ContainerBehavior.cpp). */
struct UnkCollision_80021E24;

/* The clump manager (see HintRing.cpp). */
class UnkUVAnimManager_801EB994;

class UnkCharaList_80074194;

namespace PJSCamera {

/* CameraModeFixLookChar (class name and bases from RTTI; defined in
 * Game/CameraModeFixLookChar.cpp): the distance and height the unit sets. */
class CameraModeFixLookChar : public PJSCameraMode {
  public:
    CameraModeFixLookChar(UnkCameraOwner_8000E9BC *owner, int number);
    virtual ~CameraModeFixLookChar();
    virtual int GetModeId();
    virtual void Update(float dt, UnkCameraUpdateArg_800099B8 *arg);
    virtual void OnSwitch(UnkCameraSwitchArg_8000996C *arg);
    virtual void vf_0x14(UnkCameraArg_800098E4 *arg);

    /* 0xAC */ float m_distance;
    /* 0xB0 */ float m_height;
};

} // namespace PJSCamera

using PJSCamera::CameraModeFixLookChar;
using PJSCamera::ExternalCameraRequest;

/* The player's character id (see Helicopter.cpp) and the null owner of colliders. */
extern s32 lbl_805EE588;
extern boost::shared_ptr<CharInfo> lbl_805EE698;

/* Out-of-line copies of inline functions of other units (see ElecCircuitEntrance.cpp and
 * ContainerBehavior.cpp). */
void GetCirclePoint(float *x, float *z, float angle, float radius);
bool IsSpecialSurface(UnkCollision_80021E24 *hit);
UnkUVAnimManager_801EB994 *GetUVAnimManager();

extern "C" {
/* Effects (see EffectPJS.cpp and HeavyDogCtrlUnit.cpp). */
UnkEffectManager_800A31F0 *fn_800A31F0(void);
void fn_800A4210(const boost::shared_ptr<EffectElement> &effect);
void fn_80425E68(EffectElement *effect, const Mtx34f *matrix);
void fn_80425E4C(EffectElement *effect, const Vec3f *position);
void fn_80425D94(EffectElement *effect, const Vec3f *rotation);
boost::weak_ptr<EffectElement> fn_801A1F54(const Vec3f *position, const Vec3f *rotation, int id);

/* Sounds (see BAGunShip.cpp). */
void *fn_80053164(void);
void fn_801D54F4(void *manager, int id, const void *source, int arg3, int arg4);
void fn_801D560C(void *manager, int id, int arg2, int arg3);

/* The score (see EggLastMech.cpp). */
UnkObject_80079808 *fn_80079808(void);
void fn_8016F0CC(void *score, int arg1, int points);

/* Characters and colliders (see DevilDoomCtrlUnit.cpp, MeteorsHolder.cpp and ThrowObject.cpp). */
UnkCharaList_80074194 *fn_80074194(void);
boost::weak_ptr<CharInfo> fn_800A8B74(UnkCharaList_80074194 *manager, int id);
bool fn_800B7AD0(const boost::shared_ptr<CharInfo> *chara);
void *fn_80021B08(void);
boost::shared_ptr<CharaColliBase> fn_800C7EA8(void *manager, const Vec3f *position, u32 flags,
                                              int arg3, int arg4,
                                              const boost::weak_ptr<CharInfo> &owner,
                                              const Vec3f *offset, float radius);
boost::weak_ptr<CharaColliAttack> fn_800C4374(void *manager,
                                              boost::shared_ptr<CharaColliBase> collider);
boost::weak_ptr<CharaColliAttack> fn_800C42D0(void *manager,
                                              boost::shared_ptr<CharaColliBase> collider);
boost::weak_ptr<CharInfo> fn_800586EC(CharaColliBaseCharInfo *collider);
void fn_80054DD0(CharaColliAttack *attack, int arg1, int arg2, float arg3,
                 const AttackCallback &onHit, const AttackCallback &onEnd);
void *fn_80020D40(void);
void fn_80021C04(UnkSphere_80021C04 *sphere, const Vec3f *center, float radius);
CollisionResultList *fn_80021934(void *world, const UnkSphere_80021C04 *sphere,
                                 const Vec3f *velocity, int arg3, int arg4, int arg5,
                                 bool (*filter)(UnkCollisionCandidate_80021934 *candidate),
                                 void *data, int arg8);
void fn_800C4190(void *manager, boost::weak_ptr<CharaColliAttack> attack,
                 CollisionResultList *hits, int arg3);
void fn_80026FB4(CollisionResultList *hits, int arg1);
void *fn_80020A20(UnkLine_80020A20 *line, const Vec3f *start, const Vec3f *end);
CollisionResultList *fn_80021FE4(void *world, const UnkLine_80020A20 *line, int arg2, int arg3,
                                 void *filter, int arg5);
UnkLineHit_8001EE04 *fn_8001EE04(CollisionResultList *hits);
UnkLineHit_8001EE04 *fn_8001EDEC(CollisionResultList *hits);
void fn_8001EDFC(UnkLineHitIter_8001EDFC *it, UnkLineHit_8001EE04 *node);
UnkLineHitIter_8001EDFC fn_80027374(UnkLineHitIter_8001EDFC *it, int);
void fn_80026AAC(CollisionResultList *hits, UnkLineHitIter_8001EDFC *it);
const Vec3f *fn_8001E9BC(void *object);

/* The camera (see CityBombBig.cpp): a shake. */
class UnkCameraManager_80009548;
UnkCameraManager_80009548 *fn_80009548(void);
void fn_8000D664(UnkCameraManager_80009548 *manager, int arg1, float arg2, float arg3, float arg4);
/* The camera mode of a request (behaviour inferred). */
boost::shared_ptr<CameraMode> fn_801CA8D0(ExternalCameraRequest *request);

/* Vibration (see BKTank.cpp). */
boost::weak_ptr<Vibration::RequestHandle> fn_8034DE08(int pattern, const Vec3f *position, int arg2);

/* RenderWare clumps and frames (see DevilDoomCtrlUnit.cpp and HeavyDogCtrlUnit.cpp). */
void *fn_8046256C(RpClump *clump);
void fn_804623EC(void *world, RpClump *clump);
void fn_804622FC(void *world, RpClump *clump);
RpClump *fn_80457090(void *data);
void fn_80457468(RpClump *clump);
void *fn_80169364(RpClump *clump);
int fn_80169350(void *frame);
void fn_8003A840(int area);
RwFrame *fn_8048722C(RwFrame *frame);
void fn_800389F8(RpClump *clump, s16 arg1);
void fn_80016390(RwMatrix *out, const Mtx34f *matrix);
Vec3f fn_800169F8(const RwV3d *position);
void fn_80016370(Mtx34f *matrix, void *frame);
void fn_800163F4(void *frame, const Mtx34f *matrix);
UnkClumpFade_80027D5C *fn_80027D5C(UnkClumpFade_80027D5C *fade);
void fn_80038198(RpClump *clump, UnkClumpFade_80027D5C *fade);
void fn_80015980(UnkClumpUpdater_80015A7C *updater, Vec3f *arg1);
void fn_801EB660(UnkUVAnimManager_801EB994 *manager, const std::string &name, void *data);
void fn_801EB550(UnkUVAnimManager_801EB994 *manager, const std::string &name);

/* The resource loader (see EggLastMech.cpp). */
void *fn_800122D4(void);
void *fn_8001277C(void *loader, s32 slot, int arg2);

/* Motions (see DevilDoomCtrlUnit.cpp). */
Mtx34f *fn_80415E70(_stMtnPlayWork *work, int id);
void fn_80415808(_stMtnPlayWork *work, int id, void (*callback)(void *, void *), void *data);
void fn_80335844(void *, void *);

/* Math (behaviour inferred). */
float fn_8000FEE4(float value); /* square root */
float fn_8040E99C(float angle);
float fn_80016260(float x, float z); /* the angle of (x, z) */
Vec3f fn_80016284(const Vec3f *direction);
void fn_8003B9E8(const Vec3f *direction, float *pitch, float *yaw);
bool fn_8003B7AC(float *root1, float *root2, float a, float b, float c);
Vec2f fn_8040CBF8(const Vec2f *a, const Vec2f *b);
float fn_8040CCEC(const Vec2f *vector);
void fn_8040CFF8(Vec3f *vector, const Vec3f *value);
void fn_8040B790(Mtx34f *matrix, const Vec3f *scale);
void fn_804103E8(Vec3f *position, Vec3f *velocity, const Vec3f *acceleration, float arg3,
                 float dt);
Vec3f fn_8040CDA8(const Vec3f *vector, float scale);
Vec3f fn_800091A4(const Vec3f *vector, float scale);
Vec3f fn_8040CDD0(const Vec3f *a, const Vec3f *b);
Vec3f fn_8040CEE8(const Vec3f *vector);
Vec3f fn_8040D0AC(const Vec3f *vector, float angle);
Vec3f fn_8040D118(const Vec3f *vector, float angle);
}

/* fn_8040CD74 called as returning a - b by value (see ArkCannon.cpp). */
typedef Vec3f (*VectorSubtractFn)(const Vec3f *a, const Vec3f *b);

inline Vec3f Sub(const Vec3f &a, const Vec3f &b) {
    return ((VectorSubtractFn)fn_8040CD74)(&a, &b);
}

/* Passes a returned vector on by address (as in ArkCannon.cpp's casts). */
inline const Vec3f &Ref(const Vec3f &vector) {
    return vector;
}

/* Degrees to radians (name inferred). */
inline float ToRadian(float degrees) {
    return degrees * (lbl_805FAD38 / 180.0f);
}

/* Puts clump in world (name inferred). */
inline void SetClumpWorld(RpClump *clump, void *world) {
    void *current = fn_8046256C(clump);
    if (current != world) {
        if (current != 0) {
            fn_804623EC(current, clump);
        }
        fn_804622FC(world, clump);
    }
}

/* Puts clump in world, given in the other order (name inferred). */
inline void MoveClumpToWorld(void *world, RpClump *clump) {
    void *current = fn_8046256C(clump);
    if (current != world) {
        if (current != 0) {
            fn_804623EC(current, clump);
        }
        fn_804622FC(world, clump);
    }
}

/* The angle wrapped to [-pi, pi] (name inferred; as in DevilDoomCtrlUnit.cpp). */
inline float WrapStep(float angle) {
    return angle > 0.0f ? -2.0f * lbl_805FAD38 : 2.0f * lbl_805FAD38;
}

#define WRAP_ANGLE(angle)                                                                          \
    while (std::fabs(angle) > lbl_805FAD38) {                                                      \
        angle += WrapStep(angle);                                                                  \
    }

/* A new clump fade (name inferred; as in BossRender.cpp). */
inline UnkClumpFade_80027D5C *NewFade() {
    UnkClumpFade_80027D5C *fade = (UnkClumpFade_80027D5C *)operator new(sizeof(UnkClumpFade_80027D5C));
    if (fade != 0) {
        fade = fn_80027D5C(fade);
    }
    return fade;
}

EggMecaCtrlUnit_GetStatus::EggMecaCtrlUnit_GetStatus(EggMecaVarParam *param)
    : BossCtrlUnit(param), EggMecaCtrlUnit(param) {
    m_kind = 0;
    boost::weak_ptr<CharInfo> player = fn_800A8B74(fn_80074194(), lbl_805EE588);
    boost::shared_ptr<CharInfo> chara(player.lock());
    if (chara) {
        fn_800091D0(&m_owner->m_prevTarget, fn_800091D0(&m_owner->m_target, &chara->m_position));
    }
}

EggMecaCtrlUnit_GetStatus::~EggMecaCtrlUnit_GetStatus() {}

int EggMecaCtrlUnit_GetStatus::Tick(float dt) {
    fn_800091D0(&m_owner->m_prevTarget, &m_owner->m_target);
    boost::weak_ptr<CharInfo> player = fn_800A8B74(fn_80074194(), lbl_805EE588);
    boost::shared_ptr<CharInfo> chara(player.lock());
    if (chara) {
        fn_800091D0(&m_owner->m_playerPosition, &chara->m_position);
        if (!m_owner->m_targetFixed) {
            fn_800091D0(&m_owner->m_target, &chara->m_position);
        }
        if (fn_800B7AD0(&chara)) {
            m_owner->field_0x1F8 = 1;
        } else {
            m_owner->field_0x1F8 = 0;
        }
    }
    Vec3f delta;
    fn_800091EC(&delta, &Ref(Sub(m_owner->m_target, m_owner->m_prevTarget)));
    float length = fn_8040CF50(&delta);
    float speed = m_owner->m_settings.m_targetSpeed;
    if (length > speed * dt) {
        fn_800091D0(&delta, &Ref(fn_8040CDA8(&Ref(fn_8040CDA8(&Ref(fn_8040CEE8(&delta)), speed)), dt)));
        fn_800091D0(&m_owner->m_target, &Ref(fn_8040CDD0(&m_owner->m_prevTarget, &delta)));
    }
    fn_800091D0(&delta, &Ref(Sub(m_owner->m_target, m_owner->m_chara->m_position)));
    delta.y = 0.0f;
    m_owner->m_distance = fn_8040CF50(&delta);
    m_owner->m_targetAngle = fn_80016260(-delta.x, -delta.z);
    return false;
}

EggMecaCtrlUnit_AttackCallback::EggMecaCtrlUnit_AttackCallback(EggMecaVarParam *param)
    : BossCtrlUnit(param), EggMecaCtrlUnit(param) {
    m_kind = 0;
    PooledList<CharaColliBase>::iterator it;
    boost::weak_ptr<CharaColliAttack> attack;
    boost::shared_ptr<CharaColliAttack> collision;
    void *manager = fn_80021B08();
    it = PooledList<CharaColliBase>::iterator(m_owner->m_colliders->Head());
    for (int i = 0; it != m_owner->m_colliders->End(); it++, i++) {
        attack = fn_800C42D0(manager, *it);
        collision = attack.lock();
        switch (i) {
        case 0:
            collision->m_attackCallback =
                AttackCallback(boost::bind(&EggMecaCtrlUnit_AttackCallback::OnDamage, this, _1));
            break;
        case 1:
            (*it)->SetAttributes(0x40004, -1);
            collision->m_hitCallback =
                AttackCallback(boost::bind(&EggMecaCtrlUnit_AttackCallback::OnHit, this, _1));
            break;
        }
    }
}

EggMecaCtrlUnit_AttackCallback::~EggMecaCtrlUnit_AttackCallback() {}

int EggMecaCtrlUnit_AttackCallback::Tick(float dt) {
    return false;
}

void EggMecaCtrlUnit_AttackCallback::OnDamage(AttackCallbackParam &param) {
    if (m_owner->m_invincible) {
        Param(param).m_result = 5;
        return;
    }
    Param(param).m_result = 1;
    fn_803358D0(m_owner, new BossCtrlUnit_DamageEff(m_owner));
    Vec3f rotation;
    fn_800091EC(&rotation, &lbl_805E07A0);
    fn_801A1F54(&Param(param).m_position, &rotation, 0x225);
    float before = m_owner->m_health;
    m_owner->m_health = before - Param(param).m_damage;
    if (m_owner->m_health < 0.0f) {
        m_owner->m_health = 0.0f;
    }
    float after = m_owner->m_health;
    m_owner->m_damageTaken += Param(param).m_damage;
    fn_8016F0CC(fn_80079808()->m_score, 0, (float)m_owner->m_settings.m_score * Param(param).m_damage);
    boost::shared_ptr<CharaColliAttack> attack(Param(param).m_attacker.lock());
    if (after <= 0.0f) {
        m_owner->m_nextMode = 0x10;
        return;
    }
    if (before / m_owner->m_maxHealth >= 0.5f && after / m_owner->m_maxHealth < 0.5f) {
        m_owner->m_nextMode = 0xF;
        return;
    }
    if (attack->m_flags & 8) {
        m_owner->m_nextMode = 4;
        return;
    }
}

void EggMecaCtrlUnit_AttackCallback::OnHit(AttackCallbackParam &param) {
    fn_800091D0(&Param(param).m_offset, &Param(param).m_direction);
    Param(param).m_damage = 1.0f;
}

EggMecaCtrlUnit_Move::EggMecaCtrlUnit_Move(EggMecaVarParam *param)
    : BossCtrlUnit(param), EggMecaCtrlUnit(param) {
    m_kind = 2;
}

EggMecaCtrlUnit_Move::~EggMecaCtrlUnit_Move() {}

int EggMecaCtrlUnit_Move::Tick(float dt) {
    if (!m_owner->m_fixed) {
        float diff = m_owner->m_targetAngle - m_owner->m_chara->m_rotation.y;
        WRAP_ANGLE(diff);
        m_owner->m_turnRate = ToRadian(m_owner->m_turnSpeed);
        if (std::fabs(diff) < m_owner->m_turnRate * dt) {
            m_owner->m_chara->m_rotation.y += diff;
            m_owner->m_turnRate = 0.0f;
        } else {
            if (diff < 0.0f) {
                m_owner->m_turnRate = -m_owner->m_turnRate;
            }
            m_owner->m_chara->m_rotation.y += m_owner->m_turnRate * dt;
        }
    } else {
        m_owner->m_turnRate = 0.0f;
    }
    m_owner->m_angleDiff = m_owner->m_targetAngle - m_owner->m_chara->m_rotation.y;
    WRAP_ANGLE(m_owner->m_angleDiff);
    return false;
}

EggMecaCtrlUnit_FixLookCharCamera::EggMecaCtrlUnit_FixLookCharCamera(EggMecaVarParam *param)
    : BossCtrlUnit(param), EggMecaCtrlUnit(param), BossCtrlUnit_FixLookCharCamera(param) {
    field_0x8 = m_owner->m_settings.m_cameraDistance;
    field_0xC = m_owner->m_settings.m_cameraHeight;
    m_offset.y = m_owner->m_settings.m_cameraOffsetY;
    m_offset.z = m_owner->m_settings.m_cameraOffsetZ;
}

/* The camera request the camera unit keeps at 0x28 (name inferred). */
inline boost::weak_ptr<ExternalCameraRequest> GetCameraRequest(BossCtrlUnit_FixLookCharCamera &camera) {
    return *reinterpret_cast<boost::weak_ptr<ExternalCameraRequest> *>(
        reinterpret_cast<u8 *>(&camera) + 0x28);
}

bool EggMecaCtrlUnit_FixLookCharCamera::IsEnabled() {
    s32 stage = lbl_8057D6A0.m_stage;
    if (stage == 0x136) {
        Vec3f target;
        fn_800091EC(&target, &m_owner->m_target);
        target.y = 0.0f;
        float distance = fn_8040CF50(&target);
        if (250.0f < distance) {
            boost::weak_ptr<ExternalCameraRequest> request = GetCameraRequest(*this);
            if (!request.expired()) {
                boost::shared_ptr<ExternalCameraRequest> locked = request.lock();
                boost::shared_ptr<CameraMode> mode = fn_801CA8D0(locked.get());
                CameraModeFixLookChar *camera = dynamic_cast<CameraModeFixLookChar *>(mode.get());
                if (camera != 0) {
                    if (350.0f <= distance) {
                        field_0x8 = 60.0f + m_owner->m_settings.m_cameraDistance;
                        camera->m_distance = field_0x8;
                    } else if (250.0f < distance) {
                        field_0x8 = 60.0f * ((distance - 250.0f) / 100.0f) + m_owner->m_settings.m_cameraDistance;
                        camera->m_distance = field_0x8;
                    }
                }
            }
        }
    } else if (stage == 0x19B) {
        boost::weak_ptr<ExternalCameraRequest> request = GetCameraRequest(*this);
        if (!request.expired()) {
            boost::shared_ptr<ExternalCameraRequest> locked = request.lock();
            boost::shared_ptr<CameraMode> mode = fn_801CA8D0(locked.get());
            dynamic_cast<CameraModeFixLookChar *>(mode.get());
        }
        float distance = m_owner->m_target.x * m_owner->m_target.x +
                         m_owner->m_target.z * m_owner->m_target.z;
        if (m_owner->m_settings.m_cameraFreeHeight <= m_owner->m_target.y &&
            m_owner->m_settings.m_cameraFreeRadius * m_owner->m_settings.m_cameraFreeRadius > distance) {
            return false;
        }
        return true;
    } else if (stage == 0x1FF) {
        Vec3f target;
        fn_800091EC(&target, &m_owner->m_target);
        target.y = 0.0f;
        float distance = fn_8040CF50(&target);
        boost::weak_ptr<ExternalCameraRequest> request = GetCameraRequest(*this);
        if (!request.expired()) {
            boost::shared_ptr<ExternalCameraRequest> locked = request.lock();
            boost::shared_ptr<CameraMode> mode = fn_801CA8D0(locked.get());
            CameraModeFixLookChar *camera = dynamic_cast<CameraModeFixLookChar *>(mode.get());
            if (camera != 0) {
                if (distance >= 250.0f || m_owner->m_health < 0.5f * m_owner->m_maxHealth) {
                    field_0xC = m_owner->m_settings.m_cameraHeight;
                    camera->m_height = field_0xC;
                } else if (distance <= 225.0f) {
                    field_0xC = 12.0f + m_owner->m_settings.m_cameraHeight;
                    camera->m_height = field_0xC;
                } else {
                    field_0xC = 12.0f * (1.0f - (distance - 225.0f) / 25.0f) + m_owner->m_settings.m_cameraHeight;
                    camera->m_height = field_0xC;
                }
            }
        }
    }
    return true;
}

void EggMecaCtrlUnit_CheckMotionAttribute::OnAttribute(u16 attribute, int value) {
    switch (attribute) {
    case 0x5001:
        switch (*(s32 *)value) {
        case 0x2E:
            fn_801D54F4(fn_80053164(), 0x7C2E, m_owner->m_chara, 0, 1);
            break;
        case 0x3E:
            fn_801D560C(fn_80053164(), 0x7C3E, 0, 0);
            break;
        }
        break;
    case 0x5000:
        m_owner->m_attribute = value;
        break;
    }
}

EggMecaCtrlUnit_DispCanopy::EggMecaCtrlUnit_DispCanopy(EggMecaVarParam *param, int model)
    : BossCtrlUnit(param), EggMecaCtrlUnit(param) {
    m_kind = 3;
    void *data = fn_8001277C(fn_800122D4(), 0x16, model);
    if (data != 0) {
        m_clump = fn_80457090(data);
    }
    if (m_clump != 0) {
        int area = fn_80169350(fn_80169364(m_clump));
        fn_8003A840(area);
        SetClumpWorld(m_clump, lbl_8051D208[area].m_world);
        m_frame = (RwFrame *)m_clump->object.parent;
        fn_800389F8(m_clump, 8);
    }
}

EggMecaCtrlUnit_DispCanopy::~EggMecaCtrlUnit_DispCanopy() {
    if (m_clump != 0) {
        void *world = fn_8046256C(m_clump);
        if (world != 0) {
            fn_804623EC(world, m_clump);
        }
        fn_80457468(m_clump);
        m_clump = 0;
        m_frame = 0;
    }
}

int EggMecaCtrlUnit_DispCanopy::Tick(float dt) {
    if (!m_owner->m_motion || !m_frame || !m_clump) {
        return false;
    }
    fn_80016390(&m_frame->modelling, fn_80415E70(m_owner->m_motion, 0x1E));
    fn_8048722C(m_frame);
    return false;
}

EggMecaCtrlUnit_DispEggman::EggMecaCtrlUnit_DispEggman(EggMecaVarParam *param, int model)
    : BossCtrlUnit(param), EggMecaCtrlUnit(param) {
    m_kind = 3;
    void *data = fn_8001277C(fn_800122D4(), 0x16, model);
    if (data != 0) {
        m_clump = fn_80457090(data);
    }
    if (m_clump != 0) {
        int area = fn_80169350(fn_80169364(m_clump));
        fn_8003A840(area);
        SetClumpWorld(m_clump, lbl_8051D208[area].m_world);
        m_frame = (RwFrame *)m_clump->object.parent;
        fn_800389F8(m_clump, 8);
    }
}

EggMecaCtrlUnit_DispEggman::~EggMecaCtrlUnit_DispEggman() {
    if (m_clump != 0) {
        void *world = fn_8046256C(m_clump);
        if (world != 0) {
            fn_804623EC(world, m_clump);
        }
        fn_80457468(m_clump);
        m_clump = 0;
        m_frame = 0;
    }
}

int EggMecaCtrlUnit_DispEggman::Tick(float dt) {
    if (!m_owner->m_motion || !m_frame || !m_clump) {
        return false;
    }
    fn_80016390(&m_frame->modelling, fn_80415E70(m_owner->m_motion, 0x1E));
    fn_8048722C(m_frame);
    return false;
}

EggMecaCtrlUnit_DispEye::EggMecaCtrlUnit_DispEye(EggMecaVarParam *param)
    : BossCtrlUnit(param), EggMecaCtrlUnit(param) {
    m_kind = 2;
    if (m_owner->m_motion) {
        m_callback =
            MotionCallbackFunction(boost::bind(&EggMecaCtrlUnit_DispEye::OnMotion, this, _1));
        fn_80415808(m_owner->m_motion, 0x1E, fn_80335844, &m_callback);
    }
}

EggMecaCtrlUnit_DispEye::~EggMecaCtrlUnit_DispEye() {
    boost::shared_ptr<EffectElement> effect;
    effect = m_effect.lock();
    if (effect) {
        effect->Stop();
        fn_800A4210(effect);
    }
}

int EggMecaCtrlUnit_DispEye::Tick(float dt) {
    return false;
}

void EggMecaCtrlUnit_DispEye::OnMotion(_stMtnCallbackParam &param) {
    Mtx34f matrix;
    fn_80016370(&matrix, param.m_frame);
    boost::shared_ptr<EffectElement> effect(m_effect.lock());
    if (effect) {
        fn_80425E68(effect.get(), &matrix);
    } else {
        m_effect = fn_800A31F0()->Launch(m_owner->m_effects, 0x1A);
        boost::shared_ptr<EffectElement> launched(m_effect.lock());
        if (launched) {
            fn_80425E68(launched.get(), &matrix);
            launched->SetField14(FrameTime());
        }
    }
}

EggMecaCtrlUnit_IronBall::EggMecaCtrlUnit_IronBall(EggMecaVarParam *param)
    : BossCtrlUnit(param), EggMecaCtrlUnit(param) {
    m_started = 0;
    m_kind = 3;
    m_chainFade = 0;
    m_fade = 0;
    void *data = fn_8001277C(fn_800122D4(), 0x16, 9);
    if (data != 0) {
        m_clump = fn_80457090(data);
    }
    if (m_clump != 0) {
        int area = fn_80169350(fn_80169364(m_clump));
        fn_8003A840(area);
        SetClumpWorld(m_clump, lbl_8051D208[area].m_world);
        m_frame = (RwFrame *)m_clump->object.parent;
        m_fade = NewFade();
        if (m_fade != 0) {
            m_fade->m_mode = 8;
            fn_80038198(m_clump, m_fade);
        }
    }
    if (lbl_8057D6A0.m_stage == 0x19B) {
        data = fn_8001277C(fn_800122D4(), 0x16, 0xF);
        fn_801EB660(GetUVAnimManager(), "IronBall", data);
        if (data != 0) {
            m_chainClump = fn_80457090(data);
        }
        if (m_chainClump != 0) {
            int area = fn_80169350(fn_80169364(m_chainClump));
            fn_8003A840(area);
            MoveClumpToWorld(lbl_8051D208[area].m_world, m_chainClump);
            m_chainFrame = (RwFrame *)m_chainClump->object.parent;
            m_chainFade = NewFade();
            if (m_chainFade != 0) {
                m_chainFade->m_mode = 8;
                fn_80038198(m_chainClump, m_chainFade);
            }
        }
    } else {
        m_chainClump = 0;
        m_chainFrame = 0;
        m_chainFade = 0;
    }
    m_time = m_owner->m_settings.m_ironBallTime;
    m_shown = 1;
}

EggMecaCtrlUnit_IronBall::~EggMecaCtrlUnit_IronBall() {
    fn_801EB550(GetUVAnimManager(), "IronBall");
    if (m_clump != 0) {
        void *world = fn_8046256C(m_clump);
        if (world != 0) {
            fn_804623EC(world, m_clump);
        }
        fn_80457468(m_clump);
        m_clump = 0;
        m_frame = 0;
    }
    if (m_chainClump != 0) {
        void *world = fn_8046256C(m_chainClump);
        if (world != 0) {
            fn_804623EC(world, m_chainClump);
        }
        fn_80457468(m_chainClump);
        m_chainClump = 0;
        m_chainFrame = 0;
    }
    boost::shared_ptr<EffectElement> effect(m_effect.lock());
    if (effect) {
        effect->Retire();
        fn_800A4210(effect);
    }
    effect = m_throwEffect.lock();
    if (effect) {
        effect->Stop();
        fn_800A4210(effect);
    }
    if (m_fade != 0) {
        delete m_fade;
    }
    if (m_chainFade != 0) {
        delete m_chainFade;
    }
}

/* Accepts the players and, of the other characters of kind 4, those whose collider lacks the
 * attributes 0x40004 (name inferred). */
static bool AcceptIronBallHit(UnkCollisionCandidate_80021934 *candidate) {
    if (candidate->m_collider != 0) {
        CharaColliBaseCharInfo *collider = dynamic_cast<CharaColliBaseCharInfo *>(candidate->m_collider);
        if (collider != 0) {
            boost::shared_ptr<CharInfo> chara(fn_800586EC(collider).lock());
            if (chara) {
                s32 kind = chara->field_0x5C;
                if (kind == 1 || kind == 2 ||
                    (kind == 4 && !(collider->GetAttributes(0) & 0x40004))) {
                    return true;
                }
            }
        }
    }
    return false;
}

/* The fades of the ball and its chain follow alpha (name inferred). */
#define SET_FADE(fade, alpha)                                                                      \
    if (fade != 0) {                                                                               \
        fade->m_color[3] = alpha;                                                                  \
        fade->SetColor(alpha);                                                                     \
    }

int EggMecaCtrlUnit_IronBall::Tick(float dt) {
    if (!m_owner->m_motion || !m_frame || !m_clump) {
        return false;
    }
    if (m_owner->m_ironBallRequest != -1) {
        if (m_owner->m_ironBallRequest != m_owner->m_ironBallState) {
            m_owner->m_ironBallState = m_owner->m_ironBallRequest;
            m_owner->m_ironBallRequest = -1;
            switch (m_owner->m_ironBallState) {
            case 1: {
                Vec3f target;
                fn_800091EC(&target, &m_owner->m_target);
                Vec3f offset;
                fn_800091EC(&offset, &Ref(Sub(m_owner->m_ironBallPosition, m_owner->m_target)));
                offset.y = 0.0f;
                fn_8040CFF8(&target, &Ref(fn_800091A4(&Ref(fn_8040CEE8(&offset)), m_owner->m_settings.m_ironBallReach)));
                float height = m_owner->m_ironBallPosition.y + m_owner->m_settings.m_ironBallHeight;
                target.y = 0.0f;
                if (GetThrowVelocity(&m_owner->m_ironBallPosition, &target, &m_velocity, height,
                                     m_owner->m_settings.m_gravity)) {
                    fn_800091D0(&m_acceleration, &Vec3f(0.0f, -m_owner->m_settings.m_gravity, 0.0f));
                    fn_804103E8(&m_owner->m_ironBallPosition, &m_velocity, &m_acceleration, 0.0f,
                                lbl_8057D6A0.m_frameTime);
                    m_effect = fn_800A31F0()->Launch(m_owner->m_effects, 0x11);
                    boost::shared_ptr<EffectElement> effect(m_effect.lock());
                    if (effect) {
                        fn_80425E4C(effect.get(), &m_owner->m_ironBallPosition);
                        effect->SetField14(FrameTime());
                    }
                    m_throwEffect = fn_800A31F0()->Launch(m_owner->m_effects, 0xD);
                    effect = m_throwEffect.lock();
                    if (effect) {
                        Mtx34f *node = fn_80415E70(m_owner->m_motion, 0x18);
                        Mtx34f matrix;
                        fn_80016370(&matrix, node);
                        fn_80425E68(effect.get(), &matrix);
                        effect->SetField14(FrameTime());
                    }
                } else {
                    m_owner->m_ironBallState = 0;
                }
                m_owner->m_hasIronBall = 0;
                break;
            }
            case 0:
                break;
            }
        } else {
            m_owner->m_ironBallRequest = -1;
        }
    }
    fn_800091D0(&m_prevPosition, &m_owner->m_ironBallPosition);
    if (m_owner->m_visible) {
        if (!m_shown && m_chainClump != 0) {
            int area = fn_80169350(fn_80169364(m_chainClump));
            fn_8003A840(area);
            SetClumpWorld(m_chainClump, lbl_8051D208[area].m_world);
        }
        if (m_chainFade != 0) {
            m_chainFade->m_dt = lbl_8057D6A0.m_frameTime;
        }
    } else if (m_shown && m_chainClump != 0) {
        void *world = fn_8046256C(m_chainClump);
        if (world != 0) {
            fn_804623EC(world, m_chainClump);
        }
    }
    m_shown = m_owner->m_visible;
    switch (m_owner->m_ironBallState) {
    case 0: {
        Mtx34f *node = fn_80415E70(m_owner->m_motion, 0x18);
        RwMatrix *matrix = &m_frame->modelling;
        fn_80016390(matrix, node);
        fn_8048722C(m_frame);
        if (m_chainFrame != 0) {
            matrix = &m_chainFrame->modelling;
            fn_80016390(matrix, node);
            fn_8048722C(m_chainFrame);
        }
        fn_800091D0(&m_owner->m_ironBallPosition, &Ref(fn_800169F8(&matrix->pos)));
        if (m_time < m_owner->m_settings.m_ironBallTime) {
            m_time += lbl_8057D6A0.m_frameTime;
            if (m_time >= m_owner->m_settings.m_ironBallTime) {
                m_time = m_owner->m_settings.m_ironBallTime;
                int area = fn_80169350(fn_80169364(m_clump));
                fn_8003A840(area);
                MoveClumpToWorld(lbl_8051D208[area].m_world, m_clump);
                m_owner->m_hasIronBall = 1;
            }
            float alpha = m_time / m_owner->m_settings.m_ironBallTime;
            SET_FADE(m_fade, alpha);
            SET_FADE(m_chainFade, alpha);
        }
        break;
    }
    case 1: {
        fn_804103E8(&m_owner->m_ironBallPosition, &m_velocity, &m_acceleration, 0.0f,
                    lbl_8057D6A0.m_frameTime);
        if (m_owner->m_ironBallPosition.y <= 0.0f) {
            m_owner->m_ironBallPosition.y = 0.0f;
            m_owner->m_units->PushBack(
                boost::shared_ptr<BossCtrlUnit>(new EggMecaCtrlUnit_ShockWave(m_owner, 7)));
            m_owner->m_ironBallState = 2;
            m_time = m_owner->m_settings.m_ironBallTime;
            boost::shared_ptr<EffectElement> effect(m_effect.lock());
            if (effect) {
                effect->Retire();
                fn_800A4210(effect);
            }
            fn_801D54F4(fn_80053164(), 0x7C35, m_owner->m_chara, 0, 1);
        } else {
            boost::shared_ptr<EffectElement> effect(m_effect.lock());
            if (effect) {
                fn_80425E4C(effect.get(), &m_owner->m_ironBallPosition);
            }
        }
        fn_80415E70(m_owner->m_motion, 0x18);
        RwFrame *frame = m_frame;
        frame->modelling.pos.x = m_owner->m_ironBallPosition.x;
        frame->modelling.pos.y = m_owner->m_ironBallPosition.y;
        frame->modelling.pos.z = m_owner->m_ironBallPosition.z;
        fn_8048722C(m_frame);
        frame = m_chainFrame;
        if (frame != 0) {
            frame->modelling.pos.x = m_owner->m_ironBallPosition.x;
            frame->modelling.pos.y = m_owner->m_ironBallPosition.y;
            frame->modelling.pos.z = m_owner->m_ironBallPosition.z;
            fn_8048722C(m_chainFrame);
        }
        break;
    }
    case 2: {
        m_time -= lbl_8057D6A0.m_frameTime;
        if (m_time < 0.0f) {
            m_time = 0.0f;
            m_owner->m_ironBallState = 0;
            int area = fn_80169350((void *)0xA);
            fn_8003A840(area);
            MoveClumpToWorld(lbl_8051D208[area].m_world, m_clump);
            m_owner->m_hasIronBall = 0;
        }
        float alpha = m_time / m_owner->m_settings.m_ironBallTime;
        SET_FADE(m_fade, alpha);
        SET_FADE(m_chainFade, alpha);
        break;
    }
    }
    if (!m_started) {
        fn_800091D0(&m_prevPosition, &m_owner->m_ironBallPosition);
        m_started = 1;
    }
    float radius;
    if (m_owner->m_moving) {
        radius = m_owner->m_settings.m_ironBallRadius2;
    } else {
        radius = m_owner->m_settings.m_ironBallRadius;
    }
    void *world = fn_80020D40();
    UnkSphere_80021C04 sphere;
    fn_80021C04(&sphere, &m_prevPosition, radius);
    Vec3f velocity;
    fn_800091EC(&velocity, &Ref(Sub(m_owner->m_ironBallPosition, m_prevPosition)));
    CollisionResultList *hits =
        fn_80021934(world, &sphere, &velocity, 2, 6, 0, AcceptIronBallHit, 0, 0);
    if (hits != 0) {
        boost::shared_ptr<CharaColliAttack> attack(new CharaColliAttack);
        fn_80054DD0(attack.get(), 4, 1, 1.0f,
                    boost::bind(&EggMecaCtrlUnit_IronBall::OnHit, this, _1),
                    boost::bind(&EggMecaCtrlUnit_IronBall::OnEnd, this, _1));
        fn_800C4190(fn_80021B08(), attack, hits, 0);
        fn_80026FB4(hits, 1);
    }
    return false;
}

void EggMecaCtrlUnit_IronBall::OnHit(AttackCallbackParam &param) {
    fn_800091D0(&Param(param).m_offset, &Param(param).m_direction);
    Param(param).m_damage = Param(param).m_attacker.lock()->m_power;
}

void EggMecaCtrlUnit_IronBall::OnEnd(AttackCallbackParam &param) {}

/* The launch velocity that throws a body from from to to through height under gravity (name
 * inferred). */
bool EggMecaCtrlUnit_IronBall::GetThrowVelocity(const Vec3f *from, const Vec3f *to,
                                                Vec3f *velocity, float height, float gravity) {
    velocity->y = fn_8000FEE4(2.0f * gravity * (height - from->y));
    float time1;
    float time2;
    float time;
    if (fn_8003B7AC(&time1, &time2, 0.5f * -gravity, velocity->y, -(to->y - from->y))) {
        time = time1 > time2 ? time1 : time2;
    } else {
        return false;
    }
    if (0.0f == time) {
        return false;
    }
    Vec2f distance;
    Vec2f start(from->x, from->z);
    Vec2f end(to->x, to->z);
    distance = fn_8040CBF8(&start, &end);
    float speed = fn_8040CCEC(&distance) / time;
    GetCirclePoint(&velocity->x, &velocity->z, fn_80016260(from->x - to->x, from->z - to->z),
                   -speed);
    return true;
}

EggMecaCtrlUnit_Laser::EggMecaCtrlUnit_Laser(EggMecaVarParam *param, int index)
    : BossCtrlUnit(param), EggMecaCtrlUnit(param), m_angles(0.0f, 0.0f, 0.0f), m_hit(0),
      m_index(index) {
    m_kind = 3;
    m_owner->m_laserState[m_index] = 0;
    m_owner->m_laserRequest[m_index] = -1;
}

EggMecaCtrlUnit_Laser::~EggMecaCtrlUnit_Laser() {
    StopEffects();
}

void EggMecaCtrlUnit_Laser::StopEffects() {
    boost::shared_ptr<EffectElement> effect(m_beam.lock());
    if (effect) {
        effect->Retire();
        fn_800A4210(effect);
    }
    effect = m_hitEffect.lock();
    if (effect) {
        effect->Stop();
        fn_800A4210(effect);
    }
}

void EggMecaCtrlUnit_Laser::UpdateMuzzle() {
    Mtx34f *node = fn_80415E70(m_owner->m_motion, m_index == 0 ? 0x20 : 0x1F);
    RwMatrix matrix;
    fn_80016390(&matrix, node);
    fn_800091D0(&m_muzzle, &Ref(fn_800169F8(&matrix.pos)));
}

void EggMecaCtrlUnit_Laser::Aim() {
    UpdateMuzzle();
    Vec3f target;
    fn_800091EC(&target, &m_owner->m_target);
    target.y -= 5.0f;
    Vec3f direction;
    fn_800091EC(&direction, &Ref(Sub(target, m_muzzle)));
    m_angles.z = 0.0f;
    fn_8003B9E8(&direction, &m_angles.x, &m_angles.y);
    if (m_index == 0) {
        m_sweepRange = ToRadian(m_owner->m_settings.m_laserSweep[0]);
        m_angles.y -= 0.5f * m_sweepRange;
    } else {
        m_sweepRange = ToRadian(m_owner->m_settings.m_laserSweep[1]);
        m_angles.x += 0.5f * m_sweepRange;
    }
}

Vec3f EggMecaCtrlUnit_Laser::GetAngles() {
    float x = m_angles.x;
    float y = m_angles.y;
    float z = m_angles.z;
    if (m_index == 0) {
        y += m_sweep;
    }
    if (m_index != 0) {
        x -= m_sweep;
    }
    return Vec3f(x, y, z);
}

void EggMecaCtrlUnit_Laser::PlaceHitEffect(boost::shared_ptr<EffectElement> effect, bool attack) {
    if (effect) {
        fn_80425E4C(effect.get(), &m_hitPosition);
        effect->SetField14(FrameTime());
    }
}

void EggMecaCtrlUnit_Laser::Fire(bool attack) {
    void *world = fn_80020D40();
    Vec3f direction(0.0f, 0.0f, 1000.0f);
    Vec3f end;
    UnkSphere_80021C04 sphere;
    Vec3f velocity;
    Vec3f angles = GetAngles();
    float yaw = angles.y;
    fn_800091D0(&direction, &Ref(fn_8040D118(&direction, angles.x)));
    fn_800091D0(&direction, &Ref(fn_8040D0AC(&direction, yaw)));
    fn_800091EC(&end, &Ref(fn_8040CDD0(&m_muzzle, &direction)));
    UnkLine_80020A20 line;
    fn_80020A20(&line, &m_muzzle, &end);
    bool found = false;
    fn_800091D0(&m_prevHitPosition, &m_hitPosition);
    fn_800091D0(&m_hitPosition, &end);
    CollisionResultList *hits = fn_80021FE4(world, &line, 3, 6, 0, 0);
    if (hits != 0) {
        UnkLineHitIter_8001EDFC it;
        UnkLineHitIter_8001EDFC next;
        {
            UnkLineHitIter_8001EDFC first((UnkLineHitIter_8001EDFC::Unset()));
            fn_8001EDFC(&first, fn_8001EE04(hits));
            it = first;
        }
        while (it.m_node != fn_8001EDEC(hits)) {
            next = it;
            fn_80027374(&next, 0);
            if (found) {
                fn_80026AAC(hits, &it);
            } else if (IsSpecialSurface((UnkCollision_80021E24 *)it.m_node)) {
                if (it.m_node->m_object != 0) {
                    fn_800091D0(&m_hitPosition, fn_8001E9BC(it.m_node->m_object));
                }
                found = true;
            }
            it = next;
        }
        if (attack) {
            boost::shared_ptr<CharaColliAttack> collision(new CharaColliAttack);
            fn_80054DD0(collision.get(), 4, 1, m_owner->m_settings.m_laserPower,
                        boost::bind(&EggMecaCtrlUnit_Laser::OnHit, this, _1),
                        boost::bind(&EggMecaCtrlUnit_Laser::OnEnd, this, _1));
            fn_800C4190(fn_80021B08(), collision, hits, 0);
        }
        fn_80026FB4(hits, 1);
    }
    m_length = fn_8040CF50(fn_800091EC(&Vec3f(), &Ref(Sub(m_muzzle, m_hitPosition)))) /
               m_owner->m_settings.m_laserLength;
    if (!m_hit) {
        fn_800091D0(&m_prevHitPosition, &m_hitPosition);
        m_hit = 1;
        if (attack) {
            m_hitEffect = fn_800A31F0()->Launch(m_owner->m_effects, 0x17);
            if (found) {
                boost::shared_ptr<EffectElement> effect(m_hitEffect.lock());
                PlaceHitEffect(effect, false);
            }
        }
    } else if (attack) {
        if (found) {
            boost::shared_ptr<EffectElement> effect(m_hitEffect.lock());
            PlaceHitEffect(effect, true);
        }
        fn_80021C04(&sphere, &m_prevHitPosition, m_owner->m_settings.m_laserRadius);
        fn_800091EC(&velocity, &Ref(Sub(m_prevHitPosition, m_hitPosition)));
        hits = fn_80021934(world, &sphere, &velocity, 2, 6, 0, 0, 0, 0);
        if (hits != 0) {
            boost::shared_ptr<CharaColliAttack> collision(new CharaColliAttack);
            fn_80054DD0(collision.get(), 4, 1, m_owner->m_settings.m_laserPower,
                        boost::bind(&EggMecaCtrlUnit_Laser::OnHit, this, _1),
                        boost::bind(&EggMecaCtrlUnit_Laser::OnEnd, this, _1));
            fn_800C4190(fn_80021B08(), collision, hits, 0);
            fn_80026FB4(hits, 1);
        }
    }
}

void EggMecaCtrlUnit_Laser::UpdateBeam() {
    boost::shared_ptr<EffectElement> effect(m_beam.lock());
    if (effect) {
        Mtx34f matrix;
        Mtx34f translation;
        Vec3f angles = GetAngles();
        float pitch = angles.x;
        float yaw = angles.y;
        UpdateMuzzle();
        fn_8040B034(&translation, &m_muzzle);
        fn_8040C148(&matrix, &translation);
        fn_8040B848(&matrix, yaw);
        fn_8040B884(&matrix, pitch);
        fn_8040B790(&matrix, &Vec3f(m_width, m_width, m_length));
        fn_80425E68(effect.get(), &matrix);
        effect->SetField14(FrameTime());
    }
}

int EggMecaCtrlUnit_Laser::Tick(float dt) {
    if (m_owner->m_dead) {
        m_owner->m_laserState[m_index] = 0;
        m_owner->m_laserRequest[m_index] = -1;
    }
    if (m_owner->m_laserRequest[m_index] != -1) {
        if (m_owner->m_laserRequest[m_index] != m_owner->m_laserState[m_index]) {
            m_owner->m_laserState[m_index] = m_owner->m_laserRequest[m_index];
            m_owner->m_laserRequest[m_index] = -1;
            switch (m_owner->m_laserState[m_index]) {
            case 0:
                break;
            case 1:
                m_beam = fn_800A31F0()->Launch(m_owner->m_effects, 0x14);
                Aim();
                m_sweep = 0.0f;
                UpdateBeam();
                m_fireTime = 0.0f;
                m_chargeTime = 0.0f;
                break;
            case 2:
                m_hit = 0;
                break;
            }
        } else {
            m_owner->m_laserRequest[m_index] = -1;
        }
    }
    switch (m_owner->m_laserState[m_index]) {
    case 1:
        m_chargeTime += lbl_8057D6A0.m_frameTime;
        if (m_chargeTime >= m_owner->m_settings.m_laserChargeTime) {
            m_chargeTime = m_owner->m_settings.m_laserChargeTime;
            m_owner->m_laserRequest[m_index] = 2;
        }
        Fire(false);
        if (0.0f != m_owner->m_settings.m_laserChargeTime) {
            m_width = m_chargeTime / m_owner->m_settings.m_laserChargeTime;
        } else {
            m_width = 1.0f;
        }
        UpdateBeam();
        break;
    case 2:
        m_fireTime += lbl_8057D6A0.m_frameTime;
        if (m_fireTime >= m_owner->m_settings.m_laserFireTime) {
            m_fireTime = m_owner->m_settings.m_laserFireTime;
            m_owner->m_laserRequest[m_index] = 0;
            StopEffects();
        }
        if (0.0f != m_owner->m_settings.m_laserFireTime) {
            m_sweep = m_fireTime * (m_sweepRange / m_owner->m_settings.m_laserFireTime);
        }
        Fire(true);
        UpdateBeam();
        break;
    case 0:
        m_chargeTime = 0.0f;
        m_fireTime = 0.0f;
        StopEffects();
        break;
    }
    return false;
}

void EggMecaCtrlUnit_Laser::OnHit(AttackCallbackParam &param) {
    Param(param).m_damage = Param(param).m_attacker.lock()->m_power;
}

void EggMecaCtrlUnit_Laser::OnEnd(AttackCallbackParam &param) {}

EggMecaCtrlUnit_Bomb::EggMecaCtrlUnit_Bomb(EggMecaVarParam *param, const Vec3f &center, float angle,
                                           float spin)
    : BossCtrlUnit(param), EggMecaCtrlUnit(param), m_center(center), m_angle(angle), m_spin(spin) {
    m_kind = 3;
    m_height = m_owner->m_settings.m_bombHeight;
    UpdatePosition();
    m_effect = fn_800A31F0()->Launch(m_owner->m_effects, 0x1D);
    boost::shared_ptr<EffectElement> effect(m_effect.lock());
    if (effect) {
        fn_80425E4C(effect.get(), &m_position);
        Vec3f rotation(0.0f, m_angle, 0.0f);
        fn_80425D94(effect.get(), &rotation);
        effect->SetField14(FrameTime());
    }
    m_time = 0.0f;
    m_speed = m_owner->m_settings.m_bombSpeed;
    m_state = -1;
    m_request = 0;
}

EggMecaCtrlUnit_Bomb::~EggMecaCtrlUnit_Bomb() {
    boost::shared_ptr<EffectElement> effect(m_effect.lock());
    if (effect) {
        effect->Retire();
        fn_800A4210(effect);
    }
}

int EggMecaCtrlUnit_Bomb::Tick(float dt) {
    if (m_owner->m_dead) {
        return true;
    }
    if (!m_owner->m_motion) {
        return false;
    }
    if (m_request != -1) {
        if (m_request != m_state) {
            m_state = m_request;
            m_request = -1;
        } else {
            m_request = -1;
        }
    }
    fn_800091D0(&m_prevPosition, &m_position);
    UnkSphere_80021C04 sphere;
    Vec3f velocity;
    switch (m_state) {
    case 0: {
        m_time += lbl_8057D6A0.m_frameTime;
        if (m_time >= m_owner->m_settings.m_bombGrowTime) {
            m_time = m_owner->m_settings.m_bombGrowTime;
            m_request = 1;
        }
        boost::shared_ptr<EffectElement> effect(m_effect.lock());
        if (effect) {
            float scale = m_time / m_owner->m_settings.m_bombGrowTime;
            Mtx34f matrix;
            Mtx34f translation;
            fn_8040B034(&translation, &m_position);
            fn_8040C148(&matrix, &translation);
            fn_8040B848(&matrix, m_angle);
            Vec3f scales(scale, scale, scale);
            fn_8040B790(&matrix, &scales);
            fn_80425E68(effect.get(), &matrix);
        }
        break;
    }
    case 1: {
        Spring(m_owner->m_settings.m_bombDamping, lbl_8057D6A0.m_frameTime, &m_height, &m_speed,
               &m_owner->m_settings.m_bombAcceleration);
        if (m_owner->m_settings.m_bombDropHeight < m_height) {
            m_request = 2;
        }
        m_angle += m_spin * lbl_8057D6A0.m_frameTime;
        UpdatePosition();
        boost::shared_ptr<EffectElement> effect(m_effect.lock());
        if (effect) {
            fn_80425E4C(effect.get(), &m_position);
            Vec3f rotation(0.0f, m_angle, 0.0f);
            fn_80425D94(effect.get(), &rotation);
        }
        break;
    }
    case 2:
        return true;
    }
    float radius = m_owner->m_settings.m_bombRadius;
    void *world = fn_80020D40();
    fn_80021C04(&sphere, &m_prevPosition, radius);
    fn_800091EC(&velocity, &Ref(Sub(m_position, m_prevPosition)));
    CollisionResultList *hits = fn_80021934(world, &sphere, &velocity, 2, 6, 0, 0, 0, 0);
    if (hits != 0) {
        boost::shared_ptr<CharaColliAttack> attack(new CharaColliAttack);
        fn_80054DD0(attack.get(), 4, 1, 1.0f, boost::bind(&EggMecaCtrlUnit_Bomb::OnHit, this, _1),
                    boost::bind(&EggMecaCtrlUnit_Bomb::OnEnd, this, _1));
        fn_800C4190(fn_80021B08(), attack, hits, 0);
        fn_80026FB4(hits, 1);
    }
    return false;
}

/* Moves the bomb to its place at its angle and height around the centre (name inferred). */
void EggMecaCtrlUnit_Bomb::UpdatePosition() {
    fn_800091D0(&m_position, &Ref(fn_8040CDD0(m_center.Get(),
                                              &Ref(fn_8040D0AC(&Vec3f(0.0f, m_owner->m_settings.m_bombDistance, m_height), m_angle)))));
}

void EggMecaCtrlUnit_Bomb::OnHit(AttackCallbackParam &param) {
    fn_800091D0(&Param(param).m_offset, &Ref(fn_80016284(&Param(param).m_direction)));
    Param(param).m_damage = Param(param).m_attacker.lock()->m_power;
}

void EggMecaCtrlUnit_Bomb::OnEnd(AttackCallbackParam &param) {}

/* A damped spring: speed accelerates by acceleration and decays with damping over dt, and height
 * follows it (name inferred). */
void EggMecaCtrlUnit_Bomb::Spring(float damping, float dt, float *height, float *speed,
                                  const float *acceleration) {
    if (damping <= 0.0f) {
        *height = *height + *speed * dt + dt * (*acceleration * dt) / 2.0f;
        *speed = *acceleration * dt + *speed;
    } else {
        float decay = (float)pow(2.7182817f, damping * dt);
        float inverse = 1.0f / damping;
        decay = 1.0f / decay;
        *height = (1.0f - decay) * (inverse * (inverse * (damping * *speed - *acceleration))) +
                  (dt * (*acceleration * inverse) + *height);
        float terminal = *acceleration * inverse;
        *speed = decay * (*speed - terminal) + terminal;
    }
}

EggMecaCtrlUnit_RollUpper::EggMecaCtrlUnit_RollUpper(EggMecaVarParam *param)
    : BossCtrlUnit(param), EggMecaCtrlUnit(param) {
    m_kind = 2;
    m_angle = 0.0f;
    m_speed = 0.0f;
    if (m_owner->m_motion) {
        m_callback =
            MotionCallbackFunction(boost::bind(&EggMecaCtrlUnit_RollUpper::OnMotion, this, _1));
        fn_80415808(m_owner->m_motion, 0xA, fn_80335844, &m_callback);
    }
}

EggMecaCtrlUnit_RollUpper::~EggMecaCtrlUnit_RollUpper() {
    boost::shared_ptr<EffectElement> effect;
    effect = m_effect.lock();
    if (effect) {
        effect->Stop();
        fn_800A4210(effect);
    }
}

int EggMecaCtrlUnit_RollUpper::Tick(float dt) {
    m_dt = dt;
    return false;
}

/* The spin's acceleration once the boss stops spinning (name inferred). */
static float s_rest[1] = {0.0f};

void EggMecaCtrlUnit_RollUpper::OnMotion(_stMtnCallbackParam &param) {
    Mtx34f matrix;
    fn_80016370(&matrix, param.m_frame);
    if (m_owner->m_moving) {
        Spring(m_owner->m_settings.m_rollDamping, m_dt, &m_angle, &m_speed, &m_owner->m_settings.m_rollAcceleration);
    } else if (0.0f != m_speed) {
        if (m_speed > m_owner->m_settings.m_rollStopSpeed) {
            Spring(m_owner->m_settings.m_rollDamping, m_dt, &m_angle, &m_speed, s_rest);
        } else {
            m_angle = Approach(0.0f, m_angle, m_speed * m_dt);
            if (0.0f == m_angle) {
                m_speed = 0.0f;
            }
        }
    }
    fn_8040B848(&matrix, m_angle);
    fn_800163F4(param.m_frame, &matrix);
    boost::shared_ptr<EffectElement> effect(m_effect.lock());
    if (effect) {
        fn_80425E68(effect.get(), &matrix);
    } else {
        m_effect = fn_800A31F0()->Launch(m_owner->m_effects, 0x1B);
        boost::shared_ptr<EffectElement> launched(m_effect.lock());
        if (launched) {
            fn_80425E68(launched.get(), &matrix);
            launched->SetField14(FrameTime());
        }
    }
}

/* The damped spring of the spin, which keeps the angle wrapped (name inferred; see
 * EggMecaCtrlUnit_Bomb::Spring). */
void EggMecaCtrlUnit_RollUpper::Spring(float damping, float dt, float *angle, float *speed,
                                       const float *acceleration) {
    if (damping <= 0.0f) {
        *angle = *angle + *speed * dt + dt * (*acceleration * dt) / 2.0f;
        fn_8040E99C(*angle);
        *speed = *acceleration * dt + *speed;
    } else {
        float decay = (float)pow(2.7182817f, damping * dt);
        float inverse = 1.0f / damping;
        decay = 1.0f / decay;
        *angle = (1.0f - decay) * (inverse * (inverse * (damping * *speed - *acceleration))) +
                 (dt * (*acceleration * inverse) + *angle);
        fn_8040E99C(*angle);
        float terminal = *acceleration * inverse;
        *speed = decay * (*speed - terminal) + terminal;
    }
}

inline float WrapOnce(float angle) {
    if (angle < -lbl_805FAD38) {
        angle += 2.0f * lbl_805FAD38;
    } else if (angle > lbl_805FAD38) {
        angle -= 2.0f * lbl_805FAD38;
    }
    return angle;
}

/* angle turned by step toward target, or target when it is within step (name inferred). */
float EggMecaCtrlUnit_RollUpper::Approach(float target, float angle, float step) {
    float diff = WrapOnce(target - angle);
    if (diff <= step && diff >= -step) {
        return target;
    }
    return WrapOnce(angle + step);
}

EggMecaCtrlUnit_ShockWave::EggMecaCtrlUnit_ShockWave(EggMecaVarParam *param, int effect)
    : BossCtrlUnit(param), EggMecaCtrlUnit(param) {
    m_time = 0.0f;
    m_effect = effect;
    m_kind = 2;
    boost::shared_ptr<EffectElement> element(fn_800A31F0()->Launch(m_owner->m_effects, m_effect));
    Vec3f position;
    if (element) {
        fn_800091D0(&position, &m_owner->m_ironBallPosition);
        position.y = 0.0f;
        fn_80425E4C(element.get(), &position);
        element->SetField14(FrameTime());
    }
    fn_8000D664(fn_80009548(), 0, m_owner->m_settings.m_shakeArgs[0], m_owner->m_settings.m_shakeArgs[1],
                m_owner->m_settings.m_shakeArgs[2]);
    m_collider = fn_800C7EA8(fn_80021B08(), &position, 0x40000, 6, 0, lbl_805EE698, &lbl_805E07A0,
                             m_owner->m_settings.m_ironBallRadius);
    m_collider->SetField2C(1);
    fn_800091D0(&m_position, &position);
    m_small = effect == 7;
    if (m_collider) {
        boost::weak_ptr<CharaColliAttack> attack = fn_800C4374(fn_80021B08(), m_collider);
        if (boost::shared_ptr<CharaColliAttack> collision = attack.lock()) {
            collision->field_0x54 = 1;
            collision->m_kind = 4;
            collision->m_power = 4.0f;
            collision->m_flags = 0x200;
            collision->m_hitCallback =
                AttackCallback(boost::bind(&EggMecaCtrlUnit_ShockWave::OnHit, this, _1));
        }
    }
    fn_8034DE08(8, &position, -2);
}

EggMecaCtrlUnit_ShockWave::~EggMecaCtrlUnit_ShockWave() {}

int EggMecaCtrlUnit_ShockWave::Tick(float dt) {
    int done = false;
    m_time += dt;
    if (m_time >= m_owner->m_settings.m_shockWaveTime) {
        m_time = m_owner->m_settings.m_shockWaveTime;
        done = true;
    }
    if (m_collider) {
        float scale = m_small ? 0.5f : 1.0f;
        float end = m_owner->m_settings.m_shockWaveRadius * scale;
        float start = m_owner->m_settings.m_ironBallRadius;
        float radius = m_time * ((end - start) / m_owner->m_settings.m_shockWaveTime) + start;
        Vec3f position;
        fn_800091EC(&position, &m_position);
        radius /= std::cos(0.45f * lbl_805FAD38);
        position.y -= radius * std::sin(0.45f * lbl_805FAD38);
        m_collider->SetCharInfo((CharInfo *)&position);
        m_collider->SetSize(radius, 0.0f, 0.0f, -2);
    }
    return done;
}

void EggMecaCtrlUnit_ShockWave::OnHit(AttackCallbackParam &param) {}

EggMecaCtrlUnit_DispAura::EggMecaCtrlUnit_DispAura(EggMecaVarParam *param)
    : BossCtrlUnit(param), EggMecaCtrlUnit(param), m_clump(0), m_updater(0), m_fade(0) {
    m_kind = 3;
    void *data = fn_8001277C(fn_800122D4(), 0x16, 7);
    fn_801EB660(GetUVAnimManager(), "Aura", data);
    if (data != 0) {
        m_clump = fn_80457090(data);
    }
    if (m_clump != 0) {
        int area = fn_80169350(fn_80169364(m_clump));
        fn_8003A840(area);
        SetClumpWorld(m_clump, lbl_8051D208[area].m_world);
        if (m_owner->m_motion) {
            m_updater = new UnkClumpUpdater_80015A7C(m_clump, m_owner->m_motion);
        }
        m_fade = NewFade();
        if (m_fade != 0) {
            m_fade->m_mode = 8;
            m_fade->SetAlpha(m_owner->m_alpha);
            fn_80038198(m_clump, m_fade);
        }
    }
    m_shown = 1;
}

EggMecaCtrlUnit_DispAura::~EggMecaCtrlUnit_DispAura() {
    fn_801EB550(GetUVAnimManager(), "Aura");
    if (m_updater != 0) {
        delete m_updater;
        m_updater = 0;
    }
    if (m_clump != 0) {
        void *world = fn_8046256C(m_clump);
        if (world != 0) {
            fn_804623EC(world, m_clump);
        }
        fn_80457468(m_clump);
        m_clump = 0;
    }
    if (m_fade != 0) {
        delete m_fade;
        m_fade = 0;
    }
}

int EggMecaCtrlUnit_DispAura::Tick(float dt) {
    if (m_owner->m_visible) {
        if (!m_shown && m_clump != 0) {
            int area = fn_80169350(fn_80169364(m_clump));
            fn_8003A840(area);
            SetClumpWorld(m_clump, lbl_8051D208[area].m_world);
        }
        if (m_updater != 0) {
            fn_80015980(m_updater, &m_owner->field_0x48);
        }
        if (m_fade != 0) {
            m_fade->m_color[3] = m_owner->m_alpha;
            m_fade->m_dt = lbl_8057D6A0.m_frameTime;
        }
    } else if (m_shown && m_clump != 0) {
        void *world = fn_8046256C(m_clump);
        if (world != 0) {
            fn_804623EC(world, m_clump);
        }
    }
    m_shown = m_owner->m_visible;
    return false;
}

EggMecaCtrlUnit_DispGuideCircle::~EggMecaCtrlUnit_DispGuideCircle() {}

int EggMecaCtrlUnit_DispGuideCircle::Tick(float dt) {
    return false;
}

EggMecaCtrlUnit::EggMecaCtrlUnit(EggMecaVarParam *owner) : BossCtrlUnit(m_owner), m_owner(owner) {}

EggMecaCtrlUnit_AdjustCircleArea::EggMecaCtrlUnit_AdjustCircleArea(EggMecaVarParam *param)
    : BossCtrlUnit(param), EggMecaCtrlUnit(param) {
    m_kind = 2;
}

EggMecaCtrlUnit::~EggMecaCtrlUnit() {}

EggMecaCtrlUnit_AdjustCircleArea::~EggMecaCtrlUnit_AdjustCircleArea() {}

int EggMecaCtrlUnit_AdjustCircleArea::Tick(float dt) {
    Vec3f position;
    fn_800091EC(&position, &m_owner->m_chara->m_position);
    position.y = 0.0f;
    switch (lbl_8057D6A0.m_stage) {
    case 0x136:
        if (position.x < -m_owner->m_settings.m_areaHalfSize) {
            position.x = -m_owner->m_settings.m_areaHalfSize;
        } else if (position.x > m_owner->m_settings.m_areaHalfSize) {
            position.x = m_owner->m_settings.m_areaHalfSize;
        }
        if (position.z < -m_owner->m_settings.m_areaHalfSize) {
            position.z = -m_owner->m_settings.m_areaHalfSize;
        } else if (position.z > m_owner->m_settings.m_areaHalfSize) {
            position.z = m_owner->m_settings.m_areaHalfSize;
        }
        m_owner->m_chara->m_position.x = position.x;
        m_owner->m_chara->m_position.z = position.z;
        break;
    case 0x19B: {
        float length = fn_8040CF50(&position);
        float radius = m_owner->m_settings.m_areaRadius;
        if (length > radius) {
            fn_800091D0(&position, &Ref(fn_8040CDA8(&Ref(fn_8040CEE8(&position)), radius)));
        }
        m_owner->m_chara->m_position.x = position.x;
        m_owner->m_chara->m_position.z = position.z;
        break;
    }
    case 0x1FF:
        if (m_owner->m_health / m_owner->m_maxHealth < 0.5f && m_owner->m_mode != 0xF) {
            float length = fn_8040CF50(&position);
            float radius = m_owner->m_settings.m_areaMaxRadius;
            if (length > radius) {
                fn_800091D0(&position, &Ref(fn_8040CDA8(&Ref(fn_8040CEE8(&position)), radius)));
            } else if (0.0f != length) {
                radius = m_owner->m_settings.m_areaMinRadius;
                if (length < radius) {
                    fn_800091D0(&position, &Ref(fn_8040CDA8(&Ref(fn_8040CEE8(&position)), radius)));
                }
            }
            m_owner->m_chara->m_position.x = position.x;
            m_owner->m_chara->m_position.z = position.z;
        }
        break;
    }
    return false;
}
