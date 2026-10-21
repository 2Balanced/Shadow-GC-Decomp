/* ArkLaser (class name from RTTI): the gadget type of the same name, a laser barrier (ARKLASER.BIN)
 * of "Length point" units. It cycles through warming up (one second, with an effect and a sound),
 * an opening motion, firing for "ShotSec" seconds (its capsule collision enabled and a looping
 * sound playing), a closing motion and a pause of "WaitSec" seconds. What the beam hits is pushed
 * away from its axis. File name inferred from the class. Names marked "(name inferred)" are not
 * original. Definitions are in reverse address order (deferred inlining emits functions
 * last-defined first). */
#include <boost/bind.hpp>
#include <boost/function.hpp>

/* The orphan RTTI base lists that start this unit's .data (see docs/matching-gadgets.md). */
#include "Game/TNumEdit.h"

#include "Game/CharaColliBaseCharInfo.h"
#include "Effect/System/EffectElement.h"
#include "Game/CharInfo.h"
#include "Game/CharaColli.h"
#include "Game/CharaColliAttack.h"
#include "Game/Gadget.h"
#include "Game/GadgetCharInfo.h"
#include "Game/GadgetModelInfo.h"
#include "Game/GadgetSound.h"
#include "Game/GadgetClassDesc.h"
#include "Game/GadgetUtil.h"
#include "Game/FileQueue.h"
#include "boost/shared_ptr.hpp"
#include "boost/weak_ptr.hpp"
#include "size_assert.h"

using SonicteamUSA::System::CharaColli::AttackCallback;
using SonicteamUSA::System::CharaColli::AttackCallbackParam;
using SonicteamUSA::System::CharaColli::CharaColliAttack;
using SonicteamUSA::System::CharaColli::CharaColliBase;

class UnkColliderManager_80021B08;
struct _stMtnPlayWork;

/* fn_8040CD74 called as returning a - b by value (it stores the result through its first
 * argument; see ArkCannon.cpp). */
typedef Vec3f (*VectorSubtractFn)(const Vec3f *a, const Vec3f *b);

/* The degrees-to-radians conversion (see BAGunShip.cpp). */
float DegToRad(float degrees);

extern "C" {
/* The collider manager, a collider for a character info, and the two attack collisions of a
 * collider (see DarkSpinEntrance.cpp and GiantLantern.cpp). */
UnkColliderManager_80021B08 *fn_80021B08(void);
boost::shared_ptr<CharaColliBase>
fn_800C7A84(UnkColliderManager_80021B08 *manager, const boost::weak_ptr<CharInfo> &charInfo, int flags,
            int arg3, u16 arg4, const Vec3f *offset, const Vec3f *rotation,
            float height, float radius, int arg9);
boost::weak_ptr<CharaColliAttack> fn_800C42D0(UnkColliderManager_80021B08 *manager,
                                              boost::shared_ptr<CharaColliBase> collider);
boost::weak_ptr<CharaColliAttack> fn_800C4374(UnkColliderManager_80021B08 *manager,
                                              boost::shared_ptr<CharaColliBase> collider);
/* Sets the two hit callbacks of an attack collision (see TurnOver.cpp). */
void fn_80054DD0(CharaColliAttack *attack, int arg1, int arg2, float arg3, const AttackCallback &onHit,
                 const AttackCallback &onEnd);

/* Loads a gadget model by file name (see ArkCannon.cpp). */
boost::shared_ptr<GadgetModelInfo> fn_8015F800(int arg0, const char *fileName);
/* Plays motion of a model part, and the motion work of a part (see BAGunShip.cpp). */
void fn_80160A84(GadgetModelInfo *model, int index, int motion);
boost::weak_ptr<_stMtnPlayWork> fn_801602B4(GadgetModelInfo *model, int index);
int fn_80416F28(_stMtnPlayWork *work, int arg1, int arg2, int arg3);

/* Starts effect id at position, pointing along direction, and makes an effect follow a matrix
 * (behaviour inferred; see GiantLantern.cpp and BKTank.cpp). */
boost::weak_ptr<Effect::System::EffectElement> fn_80168C48(const Vec3f *position,
                                                           const Vec3f *direction, int id);
void fn_80425E68(Effect::System::EffectElement *effect, const Mtx34f *matrix);

/* Starts the looping sound soundId at charInfo's position for owner, and plays sound id at a
 * character info (behaviour inferred; see GiantLantern.cpp). */
UnkSoundHandle_801D3DC8 fn_801D3E58(int soundId, GadgetCharInfo *charInfo, void *owner);
void fn_801D54F4(void *manager, int id, GadgetCharInfo *charInfo, int arg3, int arg4);

/* fn_8040CDD0 called as returning a + b by value, normalising a vector (returns the former
 * length), and the point of a segment (built by fn_80020A20) nearest to point (see
 * Helicopter.cpp). */
Vec3f fn_8040CDD0(const Vec3f *a, const Vec3f *b);
float fn_8040CE74(Vec3f *vector);
Vec3f fn_8040FC3C(const void *segment, const Vec3f *point);
}

/* A length (name inferred). As a call it is not a constant expression, so the constants below are
 * stored by the static initializer, as in retail (as in GiantLantern.cpp). */
inline float Span(float length) {
    return length;
}

/* The model's length in units at scale 1, and the radius of the beam (names inferred). */
static const float s_modelLength = Span(100.0f);
static const float s_beamRadius = Span(50.0f);

/* The direction of the warm-up effect (name inferred). The original copy-constructs it from a
 * temporary through the vector type's out-of-line copy constructor, fn_800091EC. */
static struct ArkLaserEffectDirection {
    ArkLaserEffectDirection() {
        float value[3];

        value[0] = 0.0f;
        value[1] = 0.0f;
        value[2] = -1.0f;
        fn_800091EC((Vec3f *)m_value, (Vec3f *)value);
    }

    const Vec3f *Get() const {
        return (const Vec3f *)m_value;
    }

    float m_value[3];
} s_effectDirection;

/* The speed a hit pushes with (name inferred). */
static const float s_pushSpeed = Span(100.0f);

/* The parameters of the owner record. */
struct ArkLaserParams {
    /* 0x0 */ float length;
    /* 0x4 */ float shotTime;
    /* 0x8 */ float waitTime;
};

/* a - b and a + b (names inferred; as in ElecBar.cpp). */
inline Vec3f SubtractVectors(const Vec3f &a, const Vec3f &b) {
    return ((VectorSubtractFn)fn_8040CD74)(&a, &b);
}

inline Vec3f AddVectors(const Vec3f &a, const Vec3f &b) {
    return fn_8040CDD0(&a, &b);
}

/* The model's scale along its axis for a laser of length (name inferred). */
inline float LengthToScale(float length) {
    return length / s_modelLength;
}

/* Builds the model's matrix: placed at position, turned by rotation and stretched to length
 * (name inferred). The original may have returned the matrix by value (the game's compiler builds a
 * named result in the caller's slot, GC/2.7 copies it), so the result pointer is explicit. */
void CalcMatrix(Mtx34f *matrix, const Vec3f &position, const Vec3f &rotation,
                float length) {
    fn_8040C148(matrix, &lbl_805E0720);
    fn_8040B59C(matrix, &position);
    fn_8040B848(matrix, rotation.y);
    fn_8040B884(matrix, rotation.x);
    fn_8040B80C(matrix, rotation.z);
    fn_8040B720(matrix, 1.0f, LengthToScale(length), 1.0f);
}

class ArkLaser : public Gadget {
  public:
    ArkLaser(Task *parent, UnkPlacement_800C9CD0 *owner);
    virtual ~ArkLaser();
    virtual void Behave(float dt);
    virtual void ReloadPlacement(UnkPlacement_800C9CD0 *owner);

    /* Creates the gadget under task layer 11 (name inferred). */
    static void Instantiate(UnkPlacement_800C9CD0 *owner);

    /* Method names are inferred. */
    /* Copies the owner's position, rotation and parameters. */
    void ReadPlacement(UnkPlacement_800C9CD0 *owner);
    /* Places the model. */
    void RebuildTransform();
    /* Enters state. */
    void SetState(int state);
    /* Leaves state from and enters state to (the model and effect side). */
    void ChangeState(int from, int to);
    /* The callbacks of the collision's attack: hitting something and the end of a hit. */
    void OnHit(AttackCallbackParam &param);
    void OnHitEnd(AttackCallbackParam &param);

    static const char *s_taskName;

    /* Member names are inferred. */
    /* 0x30 */ ArkLaserParams m_params;
    /* 0x3C */ boost::shared_ptr<GadgetCharInfo> m_chara;
    /* 0x44 */ boost::shared_ptr<GadgetModelInfo> m_model;
    /* 0x4C */ boost::shared_ptr<CharaColliBase> m_collider;
    /* 0x54 */ boost::weak_ptr<Effect::System::EffectElement> m_effect;
    /* 0x5C */ s32 m_state; /* 1 warming up, 2 opening, 3 firing, 4 closing, 5 waiting, 6 reset */
    /* 0x60 */ float m_timer;
    /* 0x64 */ UnkSoundHandle_801D3DC8 m_sound;
};
SIZE_ASSERT(ArkLaser, 0x80);

const char *ArkLaser::s_taskName = "ArkLaser";

ArkLaser::ArkLaser(Task *parent, UnkPlacement_800C9CD0 *owner) : Gadget(parent, owner), m_state(0) {
    m_name = s_taskName;
    m_effect.reset();
    m_chara = fn_8015CD90(0x6E, 4);
    ReadPlacement(owner);
    m_model = fn_8015F800(1, "ARKLASER.BIN");
    Vec3f rotation(DegToRad(90.0f), 0.0f, 0.0f);
    m_collider = fn_800C7A84(fn_80021B08(), m_chara, 0x140000, 7, 0, &lbl_805E07A0, &rotation,
                             0.5f * m_params.length, s_beamRadius, 0);
    {
        boost::shared_ptr<CharaColliAttack> attack = fn_800C4374(fn_80021B08(), m_collider).lock();
        fn_80054DD0(attack.get(), 4, 0x100, 1.0f, boost::bind(&ArkLaser::OnHit, this, _1),
                    boost::bind(&ArkLaser::OnHitEnd, this, _1));
        attack->field_0x54 = 0;
        m_collider->TurnOff(-1);
    }
    SetState(1);
}

ArkLaser::~ArkLaser() {
    if (!m_effect.expired()) {
        boost::shared_ptr<Effect::System::EffectElement> effect = m_effect.lock();
        if (effect.get() != 0) {
            effect->Retire();
        }
        m_effect.reset();
    }
    m_sound.fn_801D455C();
}

extern "C" void fn_8026E404(void) {
    fn_8006B474("ARKLASER.BIN", 1);
}

extern "C" void fn_8026E400(void) {}

void ArkLaser::Instantiate(UnkPlacement_800C9CD0 *owner) {
    new ArkLaser(fn_801EB304(fn_8001C4F8(), 11), owner);
}

void ArkLaser::Behave(float dt) {
    switch (m_state) {
    case 1:
        m_timer += dt;
        if (m_timer >= 1.0f) {
            SetState(2);
        }
        break;
    case 2: {
        int done = fn_80416F28(fn_801602B4(m_model.get(), 0).lock().get(), 0, 3, 1);
        if (done) {
            SetState(3);
        }
        break;
    }
    case 3:
        m_timer += dt;
        if (m_timer >= m_params.shotTime) {
            SetState(4);
        }
        break;
    case 4: {
        int done = fn_80416F28(fn_801602B4(m_model.get(), 0).lock().get(), 0, 3, 1);
        if (done) {
            SetState(5);
        }
        break;
    }
    case 5:
        m_timer += dt;
        if (m_timer >= m_params.waitTime) {
            SetState(1);
        }
        break;
    case 6:
        SetState(1);
        break;
    }
    m_model->fn_80162FB0(dt);
}

void ArkLaser::ReloadPlacement(UnkPlacement_800C9CD0 *owner) {
    ReadPlacement(owner);
    SetState(6);
    RebuildTransform();
    m_collider->SetSize(s_beamRadius, 0.0f, 0.5f * m_params.length, -2);
    m_model->fn_80162EF4();
    m_collider->Reset();
}

void ArkLaser::ReadPlacement(UnkPlacement_800C9CD0 *owner) {
    fn_800C9F14(owner, &m_chara->m_position);
    fn_800C9ED4(owner, &m_chara->m_rotation);
    fn_800C9DBC(owner, &m_params, sizeof(m_params));
}

void ArkLaser::RebuildTransform() {
    Mtx34f matrix;

    CalcMatrix(&matrix, m_chara->m_position, m_chara->m_rotation, m_params.length);
    m_model->fn_801633DC(&matrix);
}

void ArkLaser::SetState(int state) {
    switch (m_state) {
    case 3: {
        boost::shared_ptr<CharaColliAttack> attack = fn_800C42D0(fn_80021B08(), m_collider).lock();
        attack->field_0x54 = 0;
        m_collider->TurnOff(-1);
        m_sound.fn_801D455C();
        break;
    }
    }
    switch (state) {
    case 1:
        m_timer = 0.0f;
        fn_801D54F4(fn_80053164(), 0x7737, m_chara.get(), 0, 1);
        break;
    case 5:
        m_timer = 0.0f;
        break;
    case 3: {
        boost::shared_ptr<CharaColliAttack> attack = fn_800C42D0(fn_80021B08(), m_collider).lock();
        attack->field_0x54 = 1;
        m_collider->TurnOn(-1);
        m_sound = fn_801D3E58(0x7738, m_chara.get(), this);
        break;
    }
    }
    ChangeState(m_state, state);
    m_state = state;
}

void ArkLaser::ChangeState(int from, int to) {
    switch (from) {
    case 1: {
        boost::shared_ptr<Effect::System::EffectElement> effect = m_effect.lock();
        if (effect.get() != 0) {
            effect->Stop();
        }
        m_effect.reset();
        break;
    }
    case 4:
        m_model->fn_801604FC();
        m_model->fn_80162804();
        break;
    }
    switch (to) {
    case 1:
        if (m_effect.expired()) {
            m_effect = fn_80168C48(&m_chara->m_position, s_effectDirection.Get(), 0x27);
            boost::shared_ptr<Effect::System::EffectElement> effect = m_effect.lock();
            if (effect.get() != 0) {
                Mtx34f matrix;

                CalcMatrix(&matrix, m_chara->m_position, m_chara->m_rotation, m_params.length);
                fn_80425E68(effect.get(), &matrix);
            }
        }
        break;
    case 2:
        m_model->fn_80162A68(0, 0);
        m_model->fn_80160C28(0);
        fn_80160A84(m_model.get(), 0, 1);
        RebuildTransform();
        break;
    case 4:
        fn_80160A84(m_model.get(), 0, 2);
        break;
    case 6:
        m_model->fn_801604FC();
        m_model->fn_80162804();
        break;
    }
}

void ArkLaser::OnHit(AttackCallbackParam &param) {
    Vec3f tip(0.0f, 0.5f * m_params.length, 0.0f);
    Mtx34f matrix;

    fn_8040C148(&matrix, &lbl_805E0720);
    fn_8040B848(&matrix, m_chara->m_rotation.y);
    fn_8040B884(&matrix, m_chara->m_rotation.x);
    fn_8040B80C(&matrix, m_chara->m_rotation.z);
    AssignVector(tip, fn_8040B67C(&matrix, &tip));
    u8 segment[0x18];
    fn_80020A20(segment, &(const Vec3f &)AddVectors(m_chara->m_position, tip),
                &(const Vec3f &)SubtractVectors(m_chara->m_position, tip));
    /* The position of what was hit at 0x18, and the direction and speed of the push at 0x34 and
     * 0x4C (meanings inferred; see GiantLantern.cpp). */
    Vec3f nearest;
    ConstructVector(&nearest, fn_8040FC3C(segment, reinterpret_cast<Vec3f *>(param.field_0x8 + (0x18 - 0x8))));
    fn_800091D0(reinterpret_cast<Vec3f *>(param.field_0x8 + (0x34 - 0x8)),
                &(const Vec3f &)((VectorSubtractFn)fn_8040CD74)(
                    reinterpret_cast<Vec3f *>(param.field_0x8 + (0x18 - 0x8)), &nearest));
    fn_8040CE74(reinterpret_cast<Vec3f *>(param.field_0x8 + (0x34 - 0x8)));
    *reinterpret_cast<float *>(param.field_0x8 + (0x4C - 0x8)) = s_pushSpeed;
    param.m_damage = 1.0f;
}

void ArkLaser::OnHitEnd(AttackCallbackParam &param) {}

static GadgetParamDesc lbl_8054CB08[] = {
    {4, "Length point", 0, 0, 0, 100.0f, 0.0f, 65536.0f, fn_800CBC20, 0},
    {4, "ShotSec", 0, 0, 0, 2.0f, 0.0f, 65536.0f, fn_800CBC20, 0},
    {4, "WaitSec", 0, 0, 0, 6.0f, 0.0f, 65536.0f, fn_800CBC20, 0},
    {0},
};

GadgetClassDesc lbl_8054CBA8 = {
    "ArkLaser", fn_8026E404, fn_8026E400, ArkLaser::Instantiate, 0, 0, 0x14BE, 1, 0, 10, lbl_8054CB08,
};
