/* TEnemyVacuumBullet (class name from RTTI, in the anonymous namespace of EnemyVacuumBullet.cpp):
 * an enemy a vacuum lets go, flying as a ThrowObject (Game/ThrowObject.h) with the enemy's model,
 * which grows back to full size and spins about its X and Z axes as it flies; when it hits
 * something it leaves effect 0x2D4. The enemy's vacuum object (Game/EnemyVacuumObject.cpp) creates
 * it through fn_8028CD0C. Like ThrowVacuumBomb::Bomb (Game/ThrowVacuumBomb.cpp). Names marked
 * "(name inferred)" are not original. Definitions are in reverse address order (deferred inlining
 * emits functions last-defined first). */
#include "Game/CharInfo.h"
#include "Game/EnemyRender.h"
#include "Game/MathTypes.h"
#include "Game/ThrowObject.h"
#include "Game/VacuumObject.h"
#include "Effect/System/EffectElement.h"
#include "boost/shared_ptr.hpp"
#include "boost/weak_ptr.hpp"
#include "size_assert.h"

using Effect::System::EffectElement;

extern "C" {
/* The model's scale and its update (see Game/EnemyRender.cpp). */
float fn_802CD614(EnemyRender *render);
void fn_802CD638(EnemyRender *render, float scale);
void fn_802CD728(EnemyRender *render, const Vec3f *position, const Vec3f *rotation);
void fn_802CD824(EnemyRender *render, float dt);
void fn_802CD7E8(EnemyRender *render);
/* An angle wrapped into one turn (behaviour inferred). */
float fn_8040E99C(float angle);
/* Starts effect id at position (behaviour inferred; see BirdBase.cpp). */
boost::weak_ptr<EffectElement> fn_801D2AFC(int arg0, const Vec3f *position, const Vec3f *direction,
                                           int id, int arg4);
/* The speed the vacuumed enemies spin at about X (Game/EnemyVacuumObject.cpp; meaning
 * inferred). */
float fn_8028CFBC(void);
}

namespace {

class TEnemyVacuumBullet : public ThrowObject {
  public:
    TEnemyVacuumBullet(const boost::weak_ptr<CharInfo> &thrower,
                       const boost::shared_ptr<EnemyRender> &render, const Vec3f &position,
                       const Vec3f &velocity, const Vec3f &rotation);
    virtual ~TEnemyVacuumBullet();
    virtual void Tick(float dt);
    virtual void OnHit();

    /* 0x30 */ boost::shared_ptr<EnemyRender> m_render;
    /* 0x38 */ Vec3f m_rotation;
    /* 0x44 */ u32 field_0x44;
};
SIZE_ASSERT(TEnemyVacuumBullet, 0x48);

} // namespace

/* A value (name inferred). As a call it is not a constant expression, so the values are stored by
 * the static initializer, as in retail (see Game/ThrowVacuumBomb.cpp). */
inline float Plain(float value) {
    return value;
}

/* The bullet's collision radius and range (names inferred). */
static const float s_radius = Plain(3.0f);
static const float s_range = Plain(500.0f);

namespace {

TEnemyVacuumBullet::TEnemyVacuumBullet(const boost::weak_ptr<CharInfo> &thrower,
                                       const boost::shared_ptr<EnemyRender> &render,
                                       const Vec3f &position, const Vec3f &velocity,
                                       const Vec3f &rotation)
    : ThrowObject(thrower, position, velocity, Vec3f(0.0f, 0.0f, 0.0f), s_radius, s_range),
      m_render(render), m_rotation(0.0f, 0.0f, 0.0f) {
    m_name = "EnemyVacuumBullet";
    m_rotation = rotation;
    SetEffect(0x254);
}

TEnemyVacuumBullet::~TEnemyVacuumBullet() {}

void TEnemyVacuumBullet::Tick(float dt) {
    ThrowObject::Tick(dt);
    float scale = fn_802CD614(m_render.get());
    if (scale < 1.0f) {
        scale += 2.0f * dt;
        if (scale > 1.0f) {
            scale = 1.0f;
        }
        fn_802CD638(m_render.get(), scale);
    }
    m_rotation.x = fn_8040E99C(m_rotation.x + dt * fn_8028CFBC());
    m_rotation.z = fn_8040E99C(m_rotation.z + dt * lbl_805EFEE8);
    fn_802CD728(m_render.get(), &(const Vec3f &)GetPosition(), &m_rotation);
    fn_802CD824(m_render.get(), dt);
    fn_802CD7E8(m_render.get());
}

void TEnemyVacuumBullet::OnHit() {
    fn_801D2AFC(0, &(const Vec3f &)GetPosition(), &lbl_805E07A0, 0x2D4, 0);
}

} // namespace

/* Creates the bullet (name inferred; called by the enemies' vacuum object). */
extern "C" void fn_8028CD0C(const boost::weak_ptr<CharInfo> &thrower,
                            const boost::shared_ptr<EnemyRender> &render, const Vec3f &position,
                            const Vec3f &velocity, const Vec3f &rotation) {
    new TEnemyVacuumBullet(thrower, render, position, velocity, rotation);
}
