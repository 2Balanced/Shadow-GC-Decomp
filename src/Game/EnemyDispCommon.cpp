/* EnemyDispCommon (class name from RTTI; Game/Enemy.h): the model and motion part of an enemy. It
 * creates the enemy's EnemyRender (through the body's slot 0x64), its health gauge and sound,
 * places the model each frame (shaking it after a hit, hiding it when dead or blinking), plays the
 * hit, damage and death effects and sounds, and draws the shadow. File name inferred from the
 * class. Names marked "(name inferred)" are not original. Definitions are in reverse address
 * order (deferred inlining emits functions last-defined first). */
/* The fields of EnemyDispCommon (Game/Enemy.h). */
#define ENEMY_DISP_COMMON_UNIT

#include "Game/Enemy.h"
#include "Game/EnemyRender.h"
#include "Game/GiHPGaugeEnemy.h"
#include "Game/MathTypes.h"
#include "Game/Random.h"
#include "boost/bind.hpp"
#include "boost/function.hpp"
#include "boost/shared_ptr.hpp"
#include "boost/weak_ptr.hpp"

namespace Effect {
namespace System {
class EffectElement;
}
}

/* The model and the motion of an EnemyRender (see Game/EnemyRender.cpp). */
class UnkModel_801A3E9C;
class UnkMotion_801A4D28;

extern "C" {
/* The resource loader (see Game/BkSoldier.cpp). */
void *fn_800122D4(void);
/* The enemies' sound (see Game/Enemy.cpp). */
void fn_8019AA40(Enemy *enemy, u16 sound, bool positional);
/* A sound at a position (see Game/GadgetSound.h). */
void *fn_80053164(void);
void fn_801D54F4(void *manager, int sound, const Vec3f *position, int arg3, int arg4);
/* Effects at a matrix and at a position with a rotation (see Game/BkLarva.cpp). */
boost::weak_ptr<Effect::System::EffectElement> fn_801A1ED8(const Mtx34f *matrix, int id);
boost::weak_ptr<Effect::System::EffectElement> fn_801A1F54(const Vec3f *position,
                                                           const Vec3f *rotation, int id);
/* The pitch and yaw of direction (see Game/BkWorm.cpp). */
void fn_8003B9E8(const Vec3f *direction, float *pitch, float *yaw);
/* The EnemyRender's functions (Game/EnemyRender.cpp). */
void fn_802CD270(EnemyRender *render, const EnemyMotionCallback &callback);
void fn_802CD450(Vec3f *result, EnemyRender *render);
void fn_802CD4AC(Vec3f *result, EnemyRender *render);
Mtx34f *fn_802CD560(EnemyRender *render, int bone);
UnkMotion_801A4D28 *fn_802CD588(EnemyRender *render);
UnkModel_801A3E9C *fn_802CD5AC(EnemyRender *render);
void fn_802CD5CC(EnemyRender *render);
void fn_802CD5F0(EnemyRender *render);
void fn_802CD638(EnemyRender *render, float scale);
void fn_802CD668(EnemyRender *render, const Vec3f *position);
void fn_802CD6B8(EnemyRender *render, const Vec3f *rotation);
void fn_802CD728(EnemyRender *render, const Vec3f *position, const Vec3f *rotation);
void fn_802CD7A4(EnemyRender *render, int motion);
void fn_802CD7E8(EnemyRender *render);
void fn_802CD824(EnemyRender *render, float dt);
/* The motion: a bone's matrix, and whether it stops (behaviour inferred). */
Mtx34f *fn_801A46E4(UnkMotion_801A4D28 *motion, int bone);
void fn_801A44DC(UnkMotion_801A4D28 *motion, bool stopped);
/* The health gauge: a new one, and whether it is shown (behaviour inferred). */
boost::shared_ptr<GiHPGaugeEnemy> fn_800BED6C(void);
bool fn_800BF124(GiHPGaugeEnemy *gauge);
/* The enemies' sounds: the options, a new sound, and its position, playing and play state (see
 * Game/BkLarva.cpp). */
u32 fn_8020D1E8(void);
boost::shared_ptr<UnkSound_802B4254> fn_802B4254(int kind, int arg1);
void fn_802B44C8(UnkSound_802B4254 *sound, const Vec3f *position);
void fn_802B4510(UnkSound_802B4254 *sound, u8 flag);
bool fn_802B4548(UnkSound_802B4254 *sound);
/* The shadow of the enemy (behaviour inferred). */
void fn_80356544(const Vec3f *position, void *ground, const Vec3f *direction, float ratio,
                 float size);
/* Whether value is zero (behaviour inferred). */
bool fn_8000A950(float value);
/* Vector helpers (see Game/Enemy.cpp): v scaled, v turned about y, a + b, the translation of a
 * matrix. */
Vec3f fn_8040CDA8(const Vec3f *v, float scale);
Vec3f fn_8040D0AC(const Vec3f *v, float angle);
Vec3f fn_8040CDD0(const Vec3f *a, const Vec3f *b);
void fn_8040FAA0(Vec3f *translation, const Mtx34f *matrix);
}

/* fn_802CD4AC and fn_802CD450 called as returning their vector (they store it through their first
 * argument). */
typedef Vec3f (*RenderVectorFn)(EnemyRender *render);

/* v * scale (name inferred). */
inline Vec3f operator*(const Vec3f &v, float scale) {
    return fn_8040CDA8(&v, scale);
}

/* The vector type's out-of-line copy and assignment
, called with a temporary the way the
 * original's operators are (names inferred; see Game/Enemy.cpp). */
inline void ConstructVector(Vec3f *vector, const Vec3f &value) {
    fn_800091EC(vector, &value);
}

inline void AssignVector(Vec3f &vector, const Vec3f &value) {
    fn_800091D0(&vector, &value);
}

EnemyDispCommon::EnemyDispCommon() {
    m_shakeTime = 0.0f;
    m_hidden = true;
    m_moving = true;
    m_soundOn = true;
    m_speedScale = 1.0f;
    m_shadowSize = 10.0f;
    m_shadowRatio = 10.0f;
    m_shadowRatio = 1.0f;
}

EnemyDispCommon::~EnemyDispCommon() {}

void EnemyDispCommon::vfDisp_0x38(const Vec3f *position, const Vec3f *rotation) {
    if (m_render.get() != 0) {
        fn_802CD728(m_render.get(), position, rotation);
        fn_802CD824(m_render.get(), 1.0f);
        fn_802CD7E8(m_render.get());
    }
}

/* Updates the health gauge and the sound over the enemy (name inferred). */
extern "C" void fn_801A102C(EnemyDispCommon *disp);

void EnemyDispCommon::vfDisp_0x08() {
    fn_800122D4();
    vfDisp_0x64();
    if (m_render.get() != 0) {
        fn_802CD728(m_render.get(), m_enemy->GetPosition(), m_enemy->GetRotation());
        fn_802CD824(m_render.get(), 0.0f);
        fn_802CD5CC(m_render.get());
        fn_802CD638(m_render.get(), m_enemy->field_0xEC);
        fn_802CD270(m_render.get(), boost::bind(&EnemyDispCommon::vfDisp_0x40, this, _1));
    }
    m_gauge = fn_800BED6C();
    if (m_gauge.get() != 0) {
        fn_800BF0EC(m_gauge.get(), 0);
    }
    if (m_enemy->m_para->vfPara_0x08() == 0) {
        if (fn_8020D1E8() & 1) {
            m_sound = fn_802B4254(0, 0);
        }
    } else if (m_enemy->m_para->vfPara_0x08() == 2 && (fn_8020D1E8() & 4)) {
        m_sound = fn_802B4254(2, 0);
    }
    if (m_sound.get() != 0) {
        fn_800BF0EC(m_gauge.get(), 0);
    }
    fn_801A102C(this);
    m_shadowSize = reinterpret_cast<float *>(m_enemy->m_para->vfPara_0x1C())[4];
    m_shadowRatio = reinterpret_cast<float *>(m_enemy->m_para->vfPara_0x1C())[5];
    if (!fn_8000A950(m_shadowRatio)) {
        m_shadowRatio = m_shadowRatio / m_shadowSize;
    }
}

/* Draws the enemy's shadow on the ground below it (name inferred). */
void EnemyDispCommon::vfDisp_0x5C() {
    if (!m_enemy->m_status->vfStat_0x40() && !m_enemy->m_move->vfMove_0x14()) {
        Vec3f position;
        UnkMotion_801A4D28 *motion = reinterpret_cast<UnkMotion_801A4D28 *>(m_enemy->m_disp->vfDisp_0x1C());
        Mtx34f *matrix;
        if (motion != 0 && (matrix = fn_801A46E4(motion, m_enemy->m_para->vfPara_0x3C())) != 0) {
            fn_8040FAA0(&position, matrix);
        } else {
            fn_800091D0(&position, m_enemy->GetPosition());
        }
        fn_80356544(&(const Vec3f &)Vec3f(position.x, m_enemy->m_move->vfMove_0x28(), position.z),
                    m_enemy->m_move->vfMove_0x30(),
                    &(const Vec3f &)fn_8040D0AC(&(const Vec3f &)Vec3f(0.0f, 0.0f, -1.0f),
                                                m_enemy->GetRotation()->y),
                    m_shadowRatio, m_shadowSize);

    }
}

extern "C" void fn_801A102C(EnemyDispCommon *disp) {
    if (disp->m_gauge.get() != 0) {
        if (!disp->m_enemy->m_status->vfStat_0x40()) {
            float ratio = 0.0f;
            float maxHealth = disp->m_enemy->m_status->vfStat_0xB0();
            if (!fn_8000A950(maxHealth)) {
                ratio = disp->m_enemy->m_status->vfStat_0xAC() / maxHealth;
                fn_800BEFD0(disp->m_gauge.get(), maxHealth);
            }
            fn_800BF0B0(disp->m_gauge.get(), ratio);
            Mtx34f *matrix = disp->GetPartMatrix(disp->m_enemy->m_para->vfPara_0x3C());
            Vec3f position;
            ConstructVector(&position, *disp->m_enemy->GetPosition());
            if (matrix != 0) {
                fn_8040FAA0(&position, matrix);
                position.y += 0.5f * reinterpret_cast<float *>(disp->m_enemy->m_para->vfPara_0x1C())[1];
            }
            fn_800BF068(disp->m_gauge.get(), &position);
            if (!fn_800BF124(disp->m_gauge.get())) {
                fn_800BF0EC(disp->m_gauge.get(), 1);
            }
            if (disp->m_sound.get() != 0) {
                if (disp->m_soundOn) {
                    fn_802B44C8(disp->m_sound.px,
                                &(const Vec3f &)fn_8040CDD0(&position, &(const Vec3f &)Vec3f(0.0f, 5.0f, 0.0f)));
                    if (!fn_802B4548(disp->m_sound.get())) {
                        fn_802B4510(disp->m_sound.get(), 1);
                    }
                } else if (fn_802B4548(disp->m_sound.get())) {
                    fn_802B4510(disp->m_sound.get(), 0);
                }
            }
        } else {
            if (fn_800BF124(disp->m_gauge.get())) {
                fn_800BF0EC(disp->m_gauge.get(), 0);
            }
            if (disp->m_sound.get() != 0 && fn_802B4548(disp->m_sound.get())) {
                fn_802B4510(disp->m_sound.get(), 0);
            }
        }
    }
}

/* Slot 0x0C, the update of the model after the motion: it takes the frame time, which
 * Game/Enemy.h does not declare, so it is defined under its symbol (as in Game/EnemyBaseAI.cpp). */
extern "C" void vfDisp_0x0C__15EnemyDispCommonFv(EnemyDispCommon *disp, float dt) {
    EnemyRender *render = disp->m_render.get();
    if (render != 0) {
        fn_802CD728(render, disp->m_enemy->GetPosition(), disp->m_enemy->GetRotation());
        fn_802CD824(disp->m_render.get(), dt);
    }
}

void EnemyDispCommon::vfDisp_0x10(float dt) {
    if (m_render.get() != 0) {
        Vec3f position;
        ConstructVector(&position, *m_enemy->GetPosition());
        if (m_enemy->m_status->vfStat_0x24() != m_enemy->m_para->vfPara_0x5C() &&
            m_enemy->m_status->vfStat_0x24() != m_enemy->m_para->vfPara_0x60()) {
            if (m_enemy->m_status->vfStat_0x4C() && !m_enemy->m_status->vfStat_0x60()) {
                m_shakeTime = 0.5f;
            }
            if ((m_shakeTime -= dt) > 0.0f) {

                position.x += fn_8040EC80(-1.5f, 1.5f);
                position.z += fn_8040EC80(-1.5f, 1.5f);
            }
        } else {
            m_shakeTime = 0.0f;
        }
        fn_802CD638(m_render.get(), m_enemy->field_0xEC);
        bool hide = false;
        if (m_enemy->m_status->vfStat_0x40()) {
            hide = true;
        } else if (m_enemy->m_status->vfStat_0x58() && m_enemy->m_type != 0 &&
                   (*reinterpret_cast<u32 *>(&lbl_8057D6A0[0x90]) & 1)) {
            hide = true;
        }
        if (hide) {
            if (!m_hidden) {
                m_hidden = true;
                fn_802CD5CC(m_render.get());
            }
        } else if (m_hidden) {
            m_hidden = false;
            fn_802CD5F0(m_render.get());
        }
        fn_802CD668(m_render.get(), &position);
        fn_802CD6B8(m_render.get(), m_enemy->GetRotation());
        int motion = vfDisp_0x14();
        if (motion != 0) {
            fn_802CD7A4(m_render.get(), motion);
        } else {
            fn_802CD7E8(m_render.get());
        }
        fn_801A102C(this);
        vfDisp_0x5C();
    }
    m_speedScale = 1.0f;
}

void *EnemyDispCommon::vfDisp_0x1C() {
    return fn_802CD588(m_render.get());
}

int EnemyDispCommon::vfDisp_0x18() {
    return reinterpret_cast<int>(fn_802CD5AC(m_render.get()));
}

Mtx34f *EnemyDispCommon::GetPartMatrix(int index) {
    if (m_render.get() != 0) {
        return fn_802CD560(m_render.get(), index);
    }
    return 0;
}

/* The model's movement of the frame, scaled by m_speedScale (behaviour inferred). */
Fn8040CopyVector3 EnemyDispCommon::vfDisp_0x28() {
    if (m_moving && m_render.get() != 0) {
        Vec3f velocity;
        fn_800091D0(&velocity, &(const Vec3f &)(((RenderVectorFn)fn_802CD4AC)(m_render.get()) * m_speedScale));
        return *reinterpret_cast<Fn8040CopyVector3 *>(&velocity);
    }
    return *reinterpret_cast<const Fn8040CopyVector3 *>(&lbl_805E07A0);
}

void EnemyDispCommon::vfDisp_0x30(float value) {
    m_speedScale = value;
}

float EnemyDispCommon::vfDisp_0x34() {
    return m_speedScale;
}

Fn8040CopyVector3 EnemyDispCommon::vfDisp_0x2C() {
    if (m_render.get() != 0) {
        Vec3f vector;
        AssignVector(vector, ((RenderVectorFn)fn_802CD450)(m_render.px));
        return *reinterpret_cast<Fn8040CopyVector3 *>(&vector);
    }
    return *reinterpret_cast<const Fn8040CopyVector3 *>(&lbl_805E07A0);
}

void EnemyDispCommon::vfDisp_0x3C(bool moving) {
    m_moving = moving;
    if (m_render.get() != 0) {
        fn_801A44DC(fn_802CD588(m_render.get()), moving == false);
    }
}

void EnemyDispCommon::vfDisp_0x40(EnemyMotionEventArg &arg) {
    if ((arg.m_type == 0x5003 || arg.m_type == 0xD004) && arg.m_data != 0) {
        vfDisp_0x44(reinterpret_cast<int *>(arg.m_data));
    }
}

bool EnemyDispCommon::vfDisp_0x44(int *command) {
    return false;
}

/* Slot 0x48, a hit at position from direction: the enemy's hit sound and effect. It takes them,
 * which Game/Enemy.h does not declare, so it is defined under its symbol. */
extern "C" void vfDisp_0x48__15EnemyDispCommonFv(EnemyDispCommon *disp, const Vec3f *position,
                                                 const Vec3f *direction) {
    Vec3f rotation(0.0f, 0.0f, 0.0f);
    fn_8003B9E8(direction, &rotation.x, &rotation.y);
    switch (disp->m_enemy->m_para->vfPara_0x08()) {
    case 0:
        if (disp->m_enemy->m_type == 0) {
            if ((int)(2.0f * fn_8040EE90()) != 0) {
                fn_801D54F4(fn_80053164(), 0x6000, disp->m_enemy->GetPosition(), 0, 1);
            } else {
                fn_801D54F4(fn_80053164(), 0x6001, disp->m_enemy->GetPosition(), 0, 1);
            }
            fn_801A1F54(position, &rotation, 0x225);
        } else {
            if ((int)(2.0f * fn_8040EE90()) != 0) {
                fn_801D54F4(fn_80053164(), 0x6002, disp->m_enemy->GetPosition(), 0, 1);
            } else {
                fn_801D54F4(fn_80053164(), 0x6003, disp->m_enemy->GetPosition(), 0, 1);
            }
            fn_801A1F54(position, &rotation, 0x6A);
        }
        break;
    case 2:
        if ((int)(2.0f * fn_8040EE90()) != 0) {
            fn_801D54F4(fn_80053164(), 0x6004, disp->m_enemy->GetPosition(), 0, 1);
        } else {
            fn_801D54F4(fn_80053164(), 0x6005, disp->m_enemy->GetPosition(), 0, 1);
        }
        fn_801A1F54(position, &rotation, 0x69);
        break;
    case 1:
        if ((int)(2.0f * fn_8040EE90()) != 0) {
            fn_801D54F4(fn_80053164(), 0x6002, disp->m_enemy->GetPosition(), 0, 1);
        } else {
            fn_801D54F4(fn_80053164(), 0x6003, disp->m_enemy->GetPosition(), 0, 1);
        }
        fn_801A1F54(position, &rotation, 0x6A);
        break;
    }
}

/* The death effect and sound (behaviour inferred). */
void EnemyDispCommon::vfDisp_0x4C() {
    Mtx34f *matrix = GetPartMatrix(m_enemy->m_para->vfPara_0x3C());
    if (matrix != 0) {
        switch (m_enemy->m_para->vfPara_0x08()) {
        case 0:
            if (m_enemy->m_type != 0) {
                fn_801A1ED8(matrix, 0x72);
                if (m_enemy->m_type == 2) {
                    fn_801D54F4(fn_80053164(), 0x6008, m_enemy->GetPosition(), 0, 1);
                } else {
                    fn_801D54F4(fn_80053164(), 0x6007, m_enemy->GetPosition(), 0, 1);
                }
            }
            break;
        case 2:
            fn_801D54F4(fn_80053164(), 0x6009, m_enemy->GetPosition(), 0, 1);
            fn_801A1ED8(matrix, 0x78);
            break;
        case 1:
            fn_801D54F4(fn_80053164(), 0x6007, m_enemy->GetPosition(), 0, 1);
            fn_801A1ED8(matrix, 0x72);
            break;
        }
    }
}

void EnemyDispCommon::vfDisp_0x50() {
    Mtx34f *matrix = GetPartMatrix(m_enemy->m_para->vfPara_0x3C());
    if (matrix != 0) {
        fn_801A1ED8(matrix, 0xE0);
    }
}

void EnemyDispCommon::vfDisp_0x54() {
    Mtx34f *matrix = GetPartMatrix(m_enemy->m_para->vfPara_0x3C());
    if (matrix != 0) {
        fn_801A1ED8(matrix, 0xE0);
        fn_8019AA40(m_enemy, 0xE015, true);
    }
}
