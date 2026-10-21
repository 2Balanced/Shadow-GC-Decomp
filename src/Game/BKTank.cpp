/* BKTank::SetBKTank (class name from RTTI): the Black Arms tank (BKTANK.BIN), a PathObject run by
 * a BKTankController::Controller task. The BK_TANK type record only loads the model file; the
 * stage's script calls the hooks here to create, restart and delete the tank. The tank has 350
 * health; attacks wear it down and start smoke and fire effects as it drops below 150 and 50, and
 * at 0 it breaks apart and is done 1.5 seconds later. Effects follow the model's frame 10; the
 * tank also shows dust while it moves and stops an effect once it moves fast. File name inferred
 * from the namespace. Method and member names are inferred. Definitions are in reverse address
 * order (deferred inlining emits functions last-defined first). The unit's code reaches its .data
 * through a pooled base address, so the linker keeps that section whole: Game/TNumEdit.h and
 * Game/CharaColliBaseCharInfo.h supply the RTTI base lists it starts with. */
/* Keeps the dead copy of bad_alloc's vtable in .data, as retail has it (see <new>). */
#define MSL_NEW_INLINE_BAD_ALLOC

#include <boost/bind.hpp>
#include <boost/function.hpp>

#include "Game/TNumEdit.h"
/* After TNumEdit.h: the order of the casts sets the order of the RTTI base lists in .data. */
#include "Game/CharaColliBaseCharInfo.h"
#include "Effect/System/EffectElement.h"
#include "Game/BKTankController.h"
#include "Game/CharInfo.h"
#include "Game/CharaColli.h"
#include "Game/CharaColliAttack.h"
#include "Game/Gadget.h"
#include "Game/GadgetCharInfo.h"
#include "Game/GadgetDebris.h"
#include "Game/GadgetModelInfo.h"
#include "Game/GadgetParticle.h"
#include "Game/GadgetClassDesc.h"
#include "Game/GadgetUtil.h"
#include "Game/PathObject.h"
#include "Game/ResourceOneFile.h"
#include "Game/Vibration.h"
#include "Game/FileQueue.h"
#include "Game/MathTypes.h"
#include "boost/shared_ptr.hpp"
#include "boost/weak_ptr.hpp"
#include "size_assert.h"

using Effect::System::EffectElement;
using SonicteamUSA::System::CharaColli::AttackCallback;
using SonicteamUSA::System::CharaColli::AttackCallbackParam;
using SonicteamUSA::System::CharaColli::CharaColliAttack;
using SonicteamUSA::System::CharaColli::CharaColliBase;

/* Target (class name from RTTI): a point characters aim at. Only the flag word is mapped here
 * (bit 0 makes the target active; names inferred). */
class Target {
  public:
    void TurnOn() {
        m_flags |= 1;
    }
    void TurnOff() {
        m_flags &= ~1;
    }
    /* Moves the target to position (name inferred). */
    void fn_800CCE38(const Vec3f *position);

    /* 0x00 */ u8 field_0x0[0x30];
    /* 0x30 */ u32 m_flags;
};

/* The handle fn_8015B8C4 wraps a collider in (class name unknown; see Bomb.cpp). */
class GadgetCharCollision {
  public:
    /* Clears flag bit 1 (meaning unknown). */
    void ClearFlag1() {
        m_flags &= ~2;
    }

    /* 0x00 */ u8 field_0x0[0x24];
    /* 0x24 */ boost::shared_ptr<CharaColliBase> m_collider;
    /* 0x2C */ u32 m_flags;
};

/* The character manager (see Monster.cpp). */
class UnkCharManager_800DDF20;

/* A model part (see BombingWall.cpp); field_0x4 names the debris model. */
struct UnkModelPart_8015F7BC {
    /* 0x0 */ u32 field_0x0;
    /* 0x4 */ const char *name;
};

struct _stMtnPlayWork;

extern "C" {
/* Adds and removes a character info from the character manager (behaviour inferred). */
UnkCharManager_800DDF20 *fn_800DDF20(void);
void fn_801DCD8C(UnkCharManager_800DDF20 *manager, const boost::shared_ptr<CharInfo> &info);
void fn_801DCCF4(UnkCharManager_800DDF20 *manager, const boost::shared_ptr<CharInfo> &info);

/* GadgetModelInfo helpers (behaviour inferred; see EggBalloon.cpp). */
boost::shared_ptr<GadgetModelInfo> fn_8015F800(int arg0, const char *fileName);
void fn_8016082C(GadgetModelInfo *model, int arg);
bool fn_80161FC8(GadgetModelInfo *model, int variant, const boost::weak_ptr<CharInfo> &info);
boost::weak_ptr<CharaColliBase> fn_80161614(GadgetModelInfo *model, int variant);
boost::weak_ptr<CharaColliAttack> fn_80161428(GadgetModelInfo *model, int variant);
boost::weak_ptr<_stMtnPlayWork> fn_801602B4(GadgetModelInfo *model, int index);
UnkModelPart_8015F7BC *fn_8015F7BC(GadgetModelInfo *model, int id);
/* The character info's world matrix (see BalloonGenerator.cpp). */
void fn_8015CEA0(GadgetCharInfo *charInfo, Mtx34f *matrix);
void fn_8016761C(GadgetParticle::GadgetParticleCtrl *ctrl, float dt);

/* A motion node and its world matrix (see DevilDoomCtrlUnit.cpp). */
void *fn_80415E70(_stMtnPlayWork *work, int id);
void fn_80016370(Mtx34f *matrix, void *frame);

/* The collision manager and its colliders (see Bomb.cpp and EggLastMech.cpp). */
void *fn_80021B08(void);
boost::weak_ptr<CharaColliAttack> fn_800C42D0(void *manager, boost::shared_ptr<CharaColliBase> collider);
boost::weak_ptr<CharaColliAttack> fn_800C4374(void *manager, boost::shared_ptr<CharaColliBase> collider);
boost::shared_ptr<CharaColliBase> fn_800C7DEC(void *manager, const boost::weak_ptr<CharInfo> &charInfo,
                                              int flags, int arg3, int arg4, const Vec3f *offset,
                                              float radius);
boost::shared_ptr<GadgetCharCollision> fn_8015B8C4(boost::shared_ptr<CharaColliBase> collider);
/* Sets the attack collision's callbacks (see EggLastMech.cpp). */
void fn_80054DD0(CharaColliAttack *attack, int arg1, int arg2, float arg3, const AttackCallback &onHit,
                 const AttackCallback &onEnd);
void fn_80096B64(CharaColliAttack *attack, int arg1, const AttackCallback &callback);

/* Targets (see Bomb.cpp). */
boost::shared_ptr<Target> fn_801E7128(const boost::weak_ptr<CharInfo> &owner, int flags);

/* Effects: launches effect id at a matrix or a position, moves an effect, and lets one go
 * (behaviour inferred). */
boost::weak_ptr<EffectElement> fn_80168A50(const Mtx34f *matrix, int id);
boost::weak_ptr<EffectElement> fn_80168C48(const Vec3f *position, const Vec3f *rotation,
                                           int id);
void fn_80425E68(EffectElement *effect, const Mtx34f *matrix);
void fn_800A4210(boost::shared_ptr<EffectElement> *effect);
void *fn_800A31F0(void);

/* Sounds (see BAGunShip.cpp). */
void fn_801D54F4(void *manager, int id, GadgetCharInfo *source, int arg3, int arg4);
void fn_801D53C4(void *manager, int id, GadgetCharInfo *charInfo, void *source, int arg4, int arg5,
                 int arg6);
/* Vibrations (see Vibration.h). */
boost::weak_ptr<Vibration::RequestHandle> fn_8034DE08(int pattern, const Vec3f *position,
                                                      int arg2);

/* Debris (see GadgetDebris.h). */
s32 fn_80012700(void *manager, int arg1, const char *fileName);

/* A 32-bit flag word (see EggBalloon.cpp). */
void fn_80014E04(u32 *flags);
void fn_80014E10(u32 *flags);
void fn_80014E1C(u32 *flags, int bit);
bool fn_80014E34(u32 *flags, int bit);
}

/* fn_8040CD74 called as returning a - b by value (see ArkCannon.cpp). */
typedef Vec3f (*VectorSubtractFn)(const Vec3f *a, const Vec3f *b);

/* The hit's position and direction in an attack callback's parameters (names inferred). */
inline const Vec3f *HitPosition(AttackCallbackParam &param) {
    return reinterpret_cast<Vec3f *>(param.field_0x8 + (0x18 - 0x8));
}
inline const Vec3f *HitRotation(AttackCallbackParam &param) {
    return reinterpret_cast<Vec3f *>(param.field_0x8 + (0x24 - 0x8));
}

/* Degrees to radians (name inferred). */
inline float ToRadians(float degrees) {
    return degrees * (lbl_805FAD38 / 180.0f);
}

/* Never read in this unit (meaning unknown); set up by a static initializer. */
static float s_unused = ToRadians(0.05f);

namespace BKTank {

/* The flag word at 0x34 (struct name unknown; bit 0: done). */
struct SetBKTankFlags {
    SetBKTankFlags() {
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

class SetBKTank : public PathObject {
  public:
    SetBKTank();
    virtual ~SetBKTank();
    virtual void Tick(float dt);
    virtual void Init();
    virtual void vf_0x14();
    virtual void UpdateMatrix();
    virtual bool HasEnded();
    virtual boost::weak_ptr<CharInfo> GetCharInfo();
    virtual void vf_0x24();
    virtual void vf_0x28();
    virtual void OnMessage(UnkPathObjectMessage *message);

    /* The attack collisions' callbacks. */
    void OnHit(AttackCallbackParam &param);
    void OnHitEnd(AttackCallbackParam &param);
    void OnAttack(AttackCallbackParam &param);
    /* Breaks the model into debris. */
    void Break();
    /* Moves to stage stage (0, 1) as the health drops. */
    void ChangeStage(int stage);

    /* 0x04 */ boost::shared_ptr<GadgetCharInfo> m_chara;
    /* 0x0C */ boost::shared_ptr<GadgetModelInfo> m_model;
    /* 0x14 */ boost::shared_ptr<GadgetParticle::GadgetParticleCtrl> m_particle;
    /* 0x1C */ float m_health;
    /* 0x20 */ float m_hitCooldown;
    /* 0x24 */ float m_timer;
    /* 0x28 */ float m_smokeTimer;
    /* 0x2C */ s32 m_hitCount;
    /* 0x30 */ s32 m_state;
    /* 0x34 */ SetBKTankFlags m_flags;
    /* 0x38 */ boost::shared_ptr<Target> m_target;
    /* 0x40 */ Vec3f m_lastPosition;
    /* 0x4C */ float m_speed;
    /* 0x50 */ boost::shared_ptr<GadgetCharCollision> m_handle;
    /* 0x58 */ boost::weak_ptr<EffectElement> m_effects[10];
    /* 0xA8 */ u8 m_smoking;
    /* 0xA9 */ u8 m_stopped;
    /* 0xAA */ u8 m_fast;
    /* 0xAB */ u8 m_tooFast;

    static const char *s_fileName;
};
SIZE_ASSERT(SetBKTank, 0xAC);

const char *SetBKTank::s_fileName = "BKTANK.BIN";

static void GetMatrix(Mtx34f *matrix, SetBKTank *tank);

SetBKTank::SetBKTank() : m_health(350.0f) {}

SetBKTank::~SetBKTank() {}

void SetBKTank::Init() {
    m_chara = fn_8015CB88(0x91, 4, "SetBKTank");
    fn_801DCD8C(fn_800DDF20(), boost::shared_ptr<CharInfo>(m_chara));
    m_chara->field_0x30 = 1;
    m_model = fn_8015F800(1, s_fileName);
    m_model->fn_80162A68(0, 0);
    m_model->fn_80160C28(0);
    fn_8016082C(m_model.get(), 0);
    m_model->fn_80160B18(0);
    fn_80161FC8(m_model.get(), 0, m_chara);
    boost::weak_ptr<CharaColliBase> collider = fn_80161614(m_model.get(), 0);
    if (!collider.expired()) {
        boost::shared_ptr<CharaColliBase> body = collider.lock();
        body->SetAttributes(0x100000, -1);
    }
    boost::weak_ptr<CharaColliAttack> attack = fn_80161428(m_model.get(), 0);
    if (boost::shared_ptr<CharaColliAttack> collision = attack.lock()) {
        fn_80054DD0(collision.get(), 1, 1, 1.0f, AttackCallback(boost::bind(&SetBKTank::OnHit, this, _1)),
                    AttackCallback(boost::bind(&SetBKTank::OnHitEnd, this, _1)));
    }
    boost::shared_ptr<CharaColliBase> body = fn_800C7DEC(
        fn_80021B08(), m_chara, 0x100000, 5, 0, &lbl_805E07A0, 56.0f);
    m_handle = fn_8015B8C4(body);
    m_handle->ClearFlag1();
    attack = fn_800C4374(fn_80021B08(), m_handle.get()->m_collider);
    if (boost::shared_ptr<CharaColliAttack> collision = attack.lock()) {
        fn_80054DD0(collision.get(), 0, 1, 1.0f, AttackCallback(boost::bind(&SetBKTank::OnHit, this, _1)),
                    AttackCallback(boost::bind(&SetBKTank::OnHitEnd, this, _1)));
        fn_80096B64(collision.get(), 1, AttackCallback(boost::bind(&SetBKTank::OnAttack, this, _1)));
    }
    UpdateMatrix();
    fn_800091D0(&m_lastPosition, &m_chara->m_position);
    m_target = fn_801E7128(m_chara, 0x22C00);
    m_hitCooldown = 0.0f;
    m_timer = 0.0f;
    m_smokeTimer = 0.0f;
    m_smoking = 0;
    m_stopped = 1;
    m_fast = 0;
    m_tooFast = 0;
    m_state = 0;
    m_flags.Clear();
}

void SetBKTank::vf_0x14() {
    fn_801DCCF4(fn_800DDF20(), boost::shared_ptr<CharInfo>(m_chara));
    boost::shared_ptr<EffectElement> effect;
    for (int i = 0; i < 10; i++) {
        effect = m_effects[i].lock();
        if (effect.get() != 0) {
            effect->Retire();
            fn_800A4210(&effect);
        }
    }
}

void SetBKTank::Tick(float dt) {
    if (0.0f != dt) {
        Vec3f delta;
        m_speed = fn_8040CF50(fn_800091EC(&delta, &(const Vec3f &)((VectorSubtractFn)fn_8040CD74)(
                                                      &m_lastPosition, &m_chara->m_position))) /
                  dt;
    }
    boost::shared_ptr<EffectElement> effect;
    if (m_model.get() != 0) {
        m_model->fn_80162FB0(dt);
    }
    if (m_particle.get() != 0) {
        fn_8016761C(m_particle.get(), dt);
    }
    m_hitCooldown -= dt;
    if (m_hitCooldown < 0.0f) {
        m_hitCooldown = 0.0f;
    }
    m_timer -= dt;
    if (m_timer < 0.0f) {
        m_timer = 0.0f;
    }
    if (0.0f < m_smokeTimer) {
        m_smokeTimer -= dt;
        if (m_smokeTimer < 0.0f) {
            m_smokeTimer = 0.0f;
            effect = m_effects[1].lock();
            if (effect.get() != 0) {
                effect->Stop();
                fn_800A4210(&effect);
            }
            effect = m_effects[2].lock();
            if (effect.get() != 0) {
                effect->Stop();
                fn_800A4210(&effect);
            }
            m_smoking = 0;
        }
    }
    if (0.0f == m_smokeTimer) {
        m_target->TurnOn();
        m_hitCount = 0;
    }
    effect = m_effects[0].lock();
    if (EffectElement *element = effect.get()) {
        Mtx34f matrix;
        GetMatrix(&matrix, this);
        fn_80425E68(element, &matrix);
    }
    effect = m_effects[9].lock();
    if (EffectElement *element = effect.get()) {
        Mtx34f matrix;
        GetMatrix(&matrix, this);
        fn_80425E68(element, &matrix);
    }
    effect = m_effects[7].lock();
    if (EffectElement *element = effect.get()) {
        Mtx34f matrix;
        GetMatrix(&matrix, this);
        fn_80425E68(element, &matrix);
    }
    effect = m_effects[1].lock();
    if (EffectElement *element = effect.get()) {
        Mtx34f matrix;
        GetMatrix(&matrix, this);
        fn_80425E68(element, &matrix);
    }
    effect = m_effects[2].lock();
    if (EffectElement *element = effect.get()) {
        Mtx34f matrix;
        GetMatrix(&matrix, this);
        fn_80425E68(element, &matrix);
    }
    effect = m_effects[6].lock();
    if (EffectElement *element = effect.get()) {
        Mtx34f matrix;
        GetMatrix(&matrix, this);
        fn_80425E68(element, &matrix);
    }

    switch (m_state) {
    case 0:
        m_state = 2;
        break;
    case 2:
        if (m_health <= 149.98999f) {
            m_state = 3;
        }
        break;
    case 3:
        if (m_health <= 50.0f) {
            ChangeStage(0);
            m_state = 4;
        }
        break;
    case 4:
        if (m_health <= 0.0f) {
            ChangeStage(1);
            m_timer = 1.5f;
            m_state = 5;
        }
        break;
    case 5:
        if (m_timer <= 0.0f) {
            m_state = 6;
        }
        break;
    case 6:
        m_flags.Set(0);
        m_state = 7;
        break;
    case 1:
    case 7:
        break;
    }

    if (m_speed >= 1800.0f) {
        m_speed = 0.0f;
        m_tooFast = 1;
        boost::shared_ptr<EffectElement> dust;
        dust = m_effects[0].lock();
        if (dust.get() != 0) {
            dust->Retire();
            fn_800A4210(&dust);
        }
        dust = m_effects[7].lock();
        if (dust.get() != 0) {
            dust->Retire();
            fn_800A4210(&dust);
        }
    } else if (m_tooFast) {
        m_tooFast = 0;
        if (m_effects[0].expired()) {
            Mtx34f matrix;
            GetMatrix(&matrix, this);
            m_effects[0] = fn_80168A50(&matrix, 0x11);
        }
        Mtx34f matrix;
        GetMatrix(&matrix, this);
        m_effects[8] = fn_80168A50(&matrix, 0x70);
    }

    bool stopped = false;
    if (m_lastPosition.x == m_chara->m_position.x && m_lastPosition.y == m_chara->m_position.y &&
        m_lastPosition.z == m_chara->m_position.z) {
        stopped = true;
    }
    if (stopped) {
        if (!m_stopped) {
            if (m_effects[9].expired()) {
                Mtx34f matrix;
                GetMatrix(&matrix, this);
                m_effects[9] = fn_80168A50(&matrix, 0x6F);
            }
            boost::shared_ptr<EffectElement> dust;
            dust = m_effects[0].lock();
            if (dust.get() != 0) {
                dust->Retire();
                fn_800A4210(&dust);
            }
        }
        fn_801D53C4(fn_80053164(), 0x714F, m_chara.get(), this, 3, 0, 5);
    } else {
        if (m_stopped) {
            if (m_effects[0].expired()) {
                Mtx34f matrix;
                GetMatrix(&matrix, this);
                m_effects[0] = fn_80168A50(&matrix, 0x11);
            }
            boost::shared_ptr<EffectElement> idle = m_effects[9].lock();
            if (idle.get() != 0) {
                idle->Retire();
                fn_800A4210(&idle);
            }
            fn_801D54F4(fn_80053164(), 0x7160, m_chara.get(), 0, 1);
        }
        fn_801D53C4(fn_80053164(), 0x714D, m_chara.get(), this, 3, 0, 5);
    }
    m_stopped = stopped;
    bool fast = m_speed >= 180.0f;
    if (!fast && m_fast) {
        boost::shared_ptr<EffectElement> trail = m_effects[7].lock();
        if (trail.get() != 0) {
            trail->Retire();
            fn_800A4210(&trail);
        }
    }
    m_fast = fast;
    fn_800091D0(&m_lastPosition, &m_chara->m_position);
}

void SetBKTank::UpdateMatrix() {
    if (m_model.get() != 0) {
        Mtx34f matrix;
        fn_8015CEA0(m_chara.get(), &matrix);
        m_model->fn_801633DC(&matrix);
    }
    if (m_target.get() != 0) {
        m_target->fn_800CCE38(&m_chara->m_position);
    }
}

bool SetBKTank::HasEnded() {
    return m_flags.Test(0);
}

void SetBKTank::vf_0x24() {}

void SetBKTank::vf_0x28() {}

void SetBKTank::OnHit(AttackCallbackParam &param) {
    Vec3f direction(0.0f, 1.0f, 0.0f);
    fn_800091D0(reinterpret_cast<Vec3f *>(param.field_0x8 + (0x34 - 0x8)), &direction);
    *reinterpret_cast<float *>(param.field_0x8 + (0x4C - 0x8)) = 1.0f;
    param.m_damage = 1.0f;
}

void SetBKTank::OnHitEnd(AttackCallbackParam &param) {}

void SetBKTank::OnAttack(AttackCallbackParam &param) {
    param.m_result = 5;
    if (m_hitCooldown > 0.0f) {
        return;
    }
    if (boost::shared_ptr<CharaColliAttack> attack = param.m_attacker.lock()) {
        u32 flags = attack->m_flags;
        if ((flags & 0x3FA) == 0) {
            return;
        }
        if (flags & 0xA) {
            if (++m_hitCount >= 3) {
                m_target->TurnOff();
                return;
            }
        }
        float health = m_health;
        m_health = health - param.m_damage;
        m_hitCooldown = 0.05f;
        m_smokeTimer = 2.0f;
        if (149.98999f < m_health) {
            m_effects[3] = fn_80168C48(HitPosition(param), HitRotation(param), 0x71);
            fn_800A31F0();
            fn_801D54F4(fn_80053164(), 0x7150, m_chara.get(), 0, 1);
        } else if (50.0f < m_health) {
            m_effects[4] = fn_80168C48(HitPosition(param), HitRotation(param), 0x64);
            fn_801D54F4(fn_80053164(), 0x7150, m_chara.get(), 0, 1);
        } else {
            m_effects[6] = fn_80168C48(HitPosition(param), HitRotation(param), 0x7F);
            fn_801D54F4(fn_80053164(), 0x7152, m_chara.get(), 0, 1);
        }
        if (!m_smoking) {
            if (149.98999f < m_health) {
                m_smoking = 1;
                Mtx34f matrix;
                GetMatrix(&matrix, this);
                m_effects[1] = fn_80168A50(&matrix, 0x12);
            } else if (50.0f < m_health) {
                m_smoking = 1;
                Mtx34f matrix;
                GetMatrix(&matrix, this);
                m_effects[2] = fn_80168A50(&matrix, 0x63);
            }
        } else if (149.98999f < health && m_health <= 149.98999f) {
            boost::shared_ptr<EffectElement> smoke = m_effects[1].lock();
            if (smoke.get() != 0) {
                smoke->Stop();
                fn_800A4210(&smoke);
            }
            Mtx34f matrix;
            GetMatrix(&matrix, this);
            m_effects[2] = fn_80168A50(&matrix, 0x63);
        } else if (50.0f < health && m_health <= 50.0f) {
            boost::shared_ptr<EffectElement> smoke = m_effects[2].lock();
            if (smoke.get() != 0) {
                smoke->Stop();
                fn_800A4210(&smoke);
            }
            Mtx34f matrix;
            GetMatrix(&matrix, this);
            m_effects[5] = fn_80168A50(&matrix, 0x13);
            fn_801D54F4(fn_80053164(), 0x7151, m_chara.get(), 0, 1);
        }
        if (m_health > 0.0f) {
            param.m_result = 1;
        } else {
            param.m_result = 3;
        }
    }
}

void SetBKTank::Break() {
    if (m_model.get() != 0) {
        UnkModelPart_8015F7BC *part = fn_8015F7BC(m_model.get(), 1);
        if (part != 0) {
            void *manager = fn_800122D4();
            UnkDebrisParam_8015F0B8 debris(m_chara, 0);
            debris.scale = 2.0f;
            debris.model = fn_80012700(manager, 1, part->name);
            debris.field_0x54 = 1;
            debris.field_0x4C = 0x96;
            fn_8015D93C(&debris);
        }
    }
    fn_801D54F4(fn_80053164(), 0x7153, m_chara.get(), 0, 1);
}

void SetBKTank::ChangeStage(int stage) {
    switch (stage) {
    case 0: {
        m_handle.reset();
        boost::weak_ptr<CharaColliBase> collider = fn_80161614(m_model.get(), 0);
        boost::shared_ptr<CharaColliBase> body = collider.lock();
        if (body.get() != 0) {
            boost::weak_ptr<CharaColliAttack> attack = fn_800C42D0(fn_80021B08(), body);
            if (boost::shared_ptr<CharaColliAttack> collision = attack.lock()) {
                fn_80096B64(collision.get(), 1, AttackCallback(boost::bind(&SetBKTank::OnAttack, this, _1)));
            }
        }
        fn_8034DE08(8, &m_chara->m_position, -2);
        break;
    }
    case 1: {
        boost::shared_ptr<EffectElement> effect;
        Break();
        m_model->fn_801604FC();
        m_model->fn_80162804();
        effect = m_effects[7].lock();
        if (effect.get() != 0) {
            effect->Retire();
            fn_800A4210(&effect);
        }
        effect = m_effects[9].lock();
        if (effect.get() != 0) {
            effect->Retire();
            fn_800A4210(&effect);
        }
        effect = m_effects[0].lock();
        if (effect.get() != 0) {
            effect->Retire();
            fn_800A4210(&effect);
        }
        fn_8034DE08(8, &m_chara->m_position, -2);
        break;
    }
    }
}

void SetBKTank::OnMessage(UnkPathObjectMessage *message) {}

} // namespace BKTank

/* The tank's controller task. */
static BKTankController::Controller *s_controller;

/* Creates the tank and its controller under task layer 9 (name inferred). */
extern "C" void fn_8031EBEC(void) {
    boost::shared_ptr<PathObject> object = boost::shared_ptr<PathObject>(new BKTank::SetBKTank);
    Task *parent = fn_801EB304(fn_8001C4F8(), 9);
    s_controller = new BKTankController::Controller(parent, object);
}

/* Deletes the controller (name inferred). */
extern "C" void fn_8031EBA4(void) {
    if (s_controller != 0) {
        delete s_controller;
        s_controller = 0;
    }
}

extern "C" void fn_8031EBA0(void) {}

/* Deletes the controller and creates a new tank (name inferred). */
extern "C" void fn_8031EB54(void) {
    if (s_controller != 0) {
        delete s_controller;
        s_controller = 0;
    }
    fn_8031EBEC();
}

namespace BKTank {

/* The world matrix of the model's motion node 10, or the identity matrix without a motion (name
 * inferred). */
static void GetMatrix(Mtx34f *matrix, SetBKTank *tank) {
    boost::weak_ptr<_stMtnPlayWork> motion = fn_801602B4(tank->m_model.get(), 0);
    if (!motion.expired()) {
        _stMtnPlayWork *work = motion.lock().get();
        void *frame = fn_80415E70(work, 10);
        Mtx34f node;
        fn_80016370(&node, frame);
        fn_8040C148(matrix, &node);
        return;
    }
    fn_8040C148(matrix, &lbl_805E0720);
}

} // namespace BKTank

extern "C" void fn_8031EA58(void) {
    fn_8006B474("BKTANK.BIN", 1);
}

extern "C" void fn_8031EA54(void) {}

namespace BKTank {

boost::weak_ptr<CharInfo> SetBKTank::GetCharInfo() {
    return m_chara;
}

} // namespace BKTank

/* The stage script's hooks: create, (nothing), restart and delete the tank (names inferred). */
extern "C" bool fn_8031E9CC(void) {
    fn_8031EBEC();
    return true;
}

extern "C" bool fn_8031E9A8(void) {
    fn_8031EBA0();
    return true;
}

extern "C" bool fn_8031E984(void) {
    fn_8031EB54();
    return true;
}

extern "C" bool fn_8031E960(void) {
    fn_8031EBA4();
    return true;
}

GadgetClassDesc lbl_8055C4F8 = {
    "BK_TANK", fn_8031EA58, fn_8031EA54, 0, 0, 0, 0x898, 1, 0x80, 10, 0,
};
