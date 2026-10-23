/* CollisionEffect (class name from RTTI; declared in Game/CollisionEffect.h): plays the effect of
 * the ground kind something stands on (dust, splashes, ...), restarting it when the kind changes
 * or the mover has moved or turned far enough, and the one-shot effect of a landing. It keeps its
 * state in a separate object (CollisionEffectImpl, class name unknown). The effects of each ground
 * kind come from a table (s_effects); a stage can replace kind 14's entry (fn_80344420, called by
 * Game/StageManager.cpp when a stage starts, and fn_803443CC when it ends). The file name predates
 * the class name; all names other than CollisionEffect are inferred. The functions other than the
 * constructor and destructor keep their fn_ names, as other units call them by those. Definitions
 * are in reverse address order (deferred inlining emits functions last-defined first). */
#include "Effect/System/EffectElement.h"
#include "Game/CollisionEffect.h"
#include "Game/MathConstants.h"
#include "Game/MathTypes.h"
#include "boost/scoped_ptr.hpp"
#include "boost/shared_ptr.hpp"
#include "boost/weak_ptr.hpp"
#include <dolphin/types.h>
#include "size_assert.h"

using Effect::System::EffectElement;

/* The game state (struct name unknown; as far as this unit reads it). */
struct UnkGameState_8057D6A0 {
    /* 0x000 */ u8 field_0x0[0x18];
    /* 0x018 */ float field_0x18; /* the effects' rate is divided by it (meaning unknown) */
    /* 0x01C */ u8 field_0x1C[0xA8 - 0x1C];
    /* 0x0A8 */ s32 field_0xA8; /* the stage (meaning inferred) */
    /* 0x0AC */ u8 field_0xAC[0x150 - 0xAC];
};
SIZE_ASSERT(UnkGameState_8057D6A0, 0x150);

extern UnkGameState_8057D6A0 lbl_8057D6A0;

extern "C" {
/* Starts effect id (behaviour inferred). */
boost::weak_ptr<EffectElement> fn_801D2BB0(int arg0, int id, int arg2);
void fn_80425E4C(EffectElement *effect, const Vec3f *position);
void fn_80425D94(EffectElement *effect, const Vec3f *rotation);
/* The two angles of a direction (behaviour inferred; see Game/ChaoHolder.cpp). */
void fn_8003B9E8(const Vec3f *direction, float *pitch, float *yaw);
float fn_8040CF90(const Vec3f *a, const Vec3f *b); /* a . b */
/* Sets and resets entry index of another table of the same shape (behaviour inferred). */
void fn_800A2730(int index, s32 start, s32 effect, s32 arg3, s32 sound);
void fn_800A26F0(int index);
/* The direction's rotation (behaviour inferred; see Game/BulletLaser.cpp). */
Vec3f fn_80016284(const Vec3f &direction);
}

/* The float overload of std::cos, out of line in the game (<cmath> is not included: its sqrtf
 * constants would land in this unit's .sdata2; as in Game/BAMine.cpp). */
namespace std {
float cos(float x);
}

/* The effects of a ground kind (struct name unknown): the landing effect, the effect played while
 * on the ground, and a sound (meanings inferred). */
struct UnkGroundEffect {
    /* 0x0 */ s32 m_land;
    /* 0x4 */ s32 m_effect;
    /* 0x8 */ s32 field_0x8;
    /* 0xC */ s32 m_sound;
};

/* A stage's replacement of kind 14's entry (struct name unknown). */
struct UnkStageGroundEffect {
    /* 0x0 */ s32 m_stage;
    /* 0x4 */ UnkGroundEffect m_effect;
};

static const UnkGroundEffect s_defaultEffects[16] = {
    {0x20, 0x56, -1, 0x7004},  {-1, -1, -1, 0x7005},     {-1, -1, -1, 0x7006},
    {0x22, 0x35, -1, 0x7007},  {0x21, 0x2E, -1, 0x7008}, {0x21, 0x2E, -1, 0x7009},
    {0x20, 0x32, -1, 0x700A},  {-1, -1, -1, 0x700B},     {-1, -1, -1, 0x700C},
    {0x67, 0x64, -1, 0x700D},  {0x2B, 0x2F, -1, 0x700E}, {-1, 0x59, -1, 0x700F},
    {-1, 0x1A6, -1, 0x7010},   {-1, -1, -1, 0x7011},     {-1, -1, -1, 0x7012},
    {-1, -1, -1, -1},
};

static UnkGroundEffect s_effects[16] = {
    {0x20, 0x56, -1, 0x7004},  {-1, -1, -1, 0x7005},     {-1, -1, -1, 0x7006},
    {0x22, 0x35, -1, 0x7007},  {0x21, 0x2E, -1, 0x7008}, {0x21, 0x2E, -1, 0x7009},
    {0x20, 0x32, -1, 0x700A},  {-1, -1, -1, 0x700B},     {-1, -1, -1, 0x700C},
    {0x67, 0x64, -1, 0x700D},  {0x2B, 0x2F, -1, 0x700E}, {-1, 0x59, -1, 0x700F},
    {-1, 0x1A6, -1, 0x7010},   {-1, -1, -1, 0x7011},     {-1, -1, -1, 0x7012},
    {-1, -1, -1, -1},
};

static UnkStageGroundEffect s_stageEffects[11] = {
    {0x12D, {0x25D, -1, -1, 0x7012}}, {0x190, {0x268, 0x262, -1, 0x7012}},
    {0x191, {0x247, 0x24D, -1, 0x7012}}, {0x192, {0x68, 0x61, -1, 0x7012}},
    {0x194, {0x2B, 0x2F, -1, 0x7012}}, {0x1F7, {0x244, -1, -1, 0x7012}},
    {0x1F8, {0x24C, 0x251, -1, 0x7012}}, {0x259, {-1, -1, 0x241, 0x7012}},
    {0x25A, {-1, -1, 0x144, 0x7012}}, {0x322, {-1, -1, 0x242, 0x7012}},
    {-1, {-1, -1, -1, -1}},
};

/* The kind a stage can replace, and how many stages the lookup checks (names inferred). */
static const int kStageKind = 14;
static const int kStageLookups = 59;

/* How far the mover may move and how far it may turn before the effect restarts (names
 * inferred). */
inline float Plain(float value) {
    return value;
}
static float s_restartDistance = Plain(2000.0f);
static float s_restartAngle = 0.5f * lbl_805FAD38;

/* The effect's rate (the float at 0x10; name inferred). */
inline void SetRate(EffectElement *effect, float rate) {
    *(float *)((u8 *)effect + 0x10) = rate;
}

/* The state of a CollisionEffect (class name unknown). */
class CollisionEffectImpl {
  public:
    CollisionEffectImpl();
    ~CollisionEffectImpl();
    void Stop();
    void Update(const Vec3f *position, const Vec3f *normal, u32 attributes, int kind);
    void Land(const Vec3f *position, const Vec3f *normal, u32 attributes, int kind);
    void Tick(float dt);

    /* 0x00 */ u8 m_active;  /* on the ground */
    /* 0x01 */ u8 m_updated; /* updated since the last tick */
    /* 0x04 */ boost::weak_ptr<EffectElement> m_effect;
    /* 0x0C */ s32 m_id;
    /* 0x10 */ Vec3f m_position;
    /* 0x1C */ Vec3f m_normal;
};
SIZE_ASSERT(CollisionEffectImpl, 0x28);

/* Deletes the state (name inferred; see checked_delete below). */
inline void DeleteImpl(CollisionEffectImpl *impl) {
    delete impl;
}

/* Retail keeps boost::checked_delete<CollisionEffectImpl> out of line; this specialization gives
 * that under GC/2.7 (as in Game/VibrationManagerUnit.cpp). */
namespace boost {
template <> inline void checked_delete<CollisionEffectImpl>(CollisionEffectImpl *x) {
    DeleteImpl(x);
}
} // namespace boost

/* The dot product of two vectors (name inferred). */
inline float Dot(const Vec3f &a, const Vec3f &b) {
    return fn_8040CF90(&a, &b);
}

CollisionEffectImpl::CollisionEffectImpl() {
    m_active = 0;
    m_updated = 0;
}

CollisionEffectImpl::~CollisionEffectImpl() {
    if (!m_effect.expired()) {
        boost::shared_ptr<EffectElement> effect = m_effect.lock();
        effect->Stop();
    }
}

void CollisionEffectImpl::Tick(float dt) {
    if (m_active && !m_updated) {
        Stop();
    } else {
        boost::shared_ptr<EffectElement> effect = m_effect.lock();
        if (effect) {
            float scale = lbl_8057D6A0.field_0x18;
            if (0.0f == scale) {
                SetRate(effect.operator->(), 0.0f);
            } else {
                SetRate(effect.operator->(), dt / scale);
            }
        }
        m_updated = 0;
    }
}

void CollisionEffectImpl::Land(const Vec3f *position, const Vec3f *normal, u32 attributes, int kind) {
    s32 id = s_effects[kind].m_land;
    if (id != -1) {
        m_effect = fn_801D2BB0(0, id, 0);
        boost::shared_ptr<EffectElement> effect = m_effect.lock();
        if (effect) {
            Vec3f rotation;
            fn_800091EC(&rotation, &lbl_805E07A0);
            fn_8003B9E8(&fn_80016284(*normal), &rotation.x, &rotation.y);
            fn_80425E4C(effect.get(), position);
            fn_80425D94(effect.get(), &rotation);
            effect->ClearFlag4();
        }
    }
    m_active = 0;
}

void CollisionEffectImpl::Update(const Vec3f *position, const Vec3f *normal, u32 attributes, int kind) {
    s32 id = s_effects[kind].m_effect;
    bool restart = false;
    Vec3f rotation;
    if (id == -1) {
        return;
    }
    if (!m_effect.expired()) {
        if (m_id != id) {
            boost::shared_ptr<EffectElement> effect = m_effect.lock();
            if (effect) {
                effect->Stop();
            }
            restart = true;
        } else {
            Vec3f moved;
            Vec3f offset;
            fn_8040CD74(&offset, &m_position, position);
            if (fn_8040CF2C(fn_800091EC(&moved, &offset)) >= s_restartDistance * s_restartDistance ||
                Dot(m_normal, *normal) <= std::cos(s_restartAngle)) {
                boost::shared_ptr<EffectElement> effect = m_effect.lock();
                if (effect) {
                    effect->Stop();
                }
                restart = true;
            }
        }
    } else {
        restart = true;
    }
    if (restart) {
        m_effect = fn_801D2BB0(0, id, 0);
    }
    boost::shared_ptr<EffectElement> effect = m_effect.lock();
    if (effect) {
        fn_800091EC(&rotation, &lbl_805E07A0);
        fn_8003B9E8(&fn_80016284(*normal), &rotation.x, &rotation.y);
        fn_80425E4C(effect.get(), position);
        fn_80425D94(effect.get(), &rotation);
        effect->ClearFlag4();
    }
    m_active = 1;
    m_updated = 1;
    m_id = id;
    fn_800091D0(&m_position, position);
    fn_800091D0(&m_normal, normal);
}

void CollisionEffectImpl::Stop() {
    if (!m_effect.expired()) {
        boost::shared_ptr<EffectElement> effect = m_effect.lock();
        effect->Stop();
    }
    m_active = 0;
    m_updated = 0;
}

CollisionEffect::CollisionEffect() : m_impl(new CollisionEffectImpl) {}

CollisionEffect::~CollisionEffect() {}

extern "C" {

/* The ticks, landings, ground updates and stop, and whether the owner is on the ground. */
void fn_80344528(CollisionEffect *effect, float dt) {
    effect->m_impl->Tick(dt);
}

void fn_80344504(CollisionEffect *effect, const Vec3f *position, const Vec3f *normal,
                 u32 attributes, int kind) {
    effect->m_impl->Land(position, normal, attributes, kind);
}

void fn_803444E0(CollisionEffect *effect, const Vec3f *position, const Vec3f *normal,
                 u32 attributes, int kind) {
    effect->m_impl->Update(position, normal, attributes, kind);
}

void fn_803444BC(CollisionEffect *effect) {
    effect->m_impl->Stop();
}

u8 fn_803444B0(CollisionEffect *effect) {
    return effect->m_impl->m_active;
}

/* Gives kind 14 the current stage's entry, if it has one. */
void fn_80344420(void) {
    for (int i = 0; i < kStageLookups; i++) {
        if (lbl_8057D6A0.field_0xA8 == s_stageEffects[i].m_stage) {
            s_effects[kStageKind] = s_stageEffects[i].m_effect;
            fn_800A2730(kStageKind, s_stageEffects[i].m_effect.m_land, s_stageEffects[i].m_effect.m_effect,
                        s_stageEffects[i].m_effect.field_0x8, s_stageEffects[i].m_effect.m_sound);
            return;
        }
    }
}

/* Gives kind 14 its default entry back. */
void fn_803443CC(void) {
    s_effects[kStageKind] = s_defaultEffects[kStageKind];
    fn_800A26F0(kStageKind);
}

}
