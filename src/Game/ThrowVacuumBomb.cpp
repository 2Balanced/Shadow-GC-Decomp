/* ThrowVacuumBomb::Bomb (class name from RTTI): a bomb a vacuum lets go. It is a ThrowObject that
 * carries the bomb's model, turning it about its X axis as it flies; when it hits something it
 * blows up as a bomb blast (type 0) or as a healing blast for the thrower (type 1). The vacuum's
 * bomb (VacuumBomb::Bomb, Game/VacuumBomb.cpp) creates it through fn_8033ED10. Like
 * ThrowContainer::Container (Game/ThrowContainer.cpp), which it mirrors. File name inferred from
 * the RTTI name. Names marked "(name inferred)" are not original. Definitions are in reverse
 * address order (deferred inlining emits functions last-defined first). */
#include "Game/CharInfo.h"
#include "Game/GadgetModelInfo.h"
#include "Game/MathTypes.h"
#include "Game/ThrowObject.h"
#include "boost/shared_ptr.hpp"
#include "boost/weak_ptr.hpp"
#include "size_assert.h"

extern "C" {
/* a * b (behaviour inferred; see Game/ThrowContainer.cpp). */
Mtx34f fn_8001E044(const Mtx34f *a, const Mtx34f *b);
}

namespace Bomb {
/* BombBlast (class name from the mangled name): the blast of a bomb. */
class BombBlast {
  public:
    static void Instantiate(const Vec3f &position);
};
} // namespace Bomb

namespace HealingUnit {
/* HealingBlast (class name from the mangled name): the blast of a healing bomb. */
class HealingBlast {
  public:
    static void Instantiate(const boost::weak_ptr<CharInfo> &owner, const Vec3f &position,
                            int arg);
};
} // namespace HealingUnit

namespace ThrowVacuumBomb {

class Bomb : public ThrowObject {
  public:
    Bomb(const boost::weak_ptr<CharInfo> &thrower, const boost::shared_ptr<GadgetModelInfo> &model,
         int type, const Vec3f &position, const Vec3f &rotation, const Vec3f &velocity,
         const Vec3f &acceleration, float spin, float scale, float range);
    virtual ~Bomb();
    virtual void Tick(float dt);
    virtual void OnRangeEnd();
    virtual void OnHit();

    /* Places the model (name inferred). */
    void UpdateModel();

    /* 0x30 */ boost::shared_ptr<GadgetModelInfo> m_model;
    /* 0x38 */ s32 m_type;      /* 0 a bomb, 1 a healing bomb */
    /* 0x3C */ Mtx34f m_matrix; /* the model's rotation */
    /* 0x6C */ float m_spin;    /* about X, per second */
    /* 0x70 */ float m_scale;
    /* 0x74 */ u32 field_0x74;
};
SIZE_ASSERT(Bomb, 0x78);

} // namespace ThrowVacuumBomb

/* A value (name inferred). As a call it is not a constant expression, so the values are stored by
 * the static initializer, as in retail (see Game/ThrowContainer.cpp). */
inline float Plain(float value) {
    return value;
}

/* The gravity, the range and the (lack of) acceleration of a bomb a vacuum lets go (names
 * inferred). */
static const float s_gravity = Plain(5.0f);
static const float s_range = Plain(600.0f);
static Vec3f s_noAcceleration(0.0f, 0.0f, 0.0f);

namespace ThrowVacuumBomb {

Bomb::Bomb(const boost::weak_ptr<CharInfo> &thrower, const boost::shared_ptr<GadgetModelInfo> &model,
           int type, const Vec3f &position, const Vec3f &rotation, const Vec3f &velocity,
           const Vec3f &acceleration, float spin, float scale, float range)
    : ThrowObject(thrower, position, velocity, acceleration, s_gravity, range) {
    fn_8040C148(&m_matrix, &lbl_805E0720);
    m_model = model;
    m_type = type;
    fn_8040B848(&m_matrix, rotation.y);
    fn_8040B884(&m_matrix, rotation.x);
    fn_8040B80C(&m_matrix, rotation.z);
    m_spin = spin;
    m_scale = scale;
}

Bomb::~Bomb() {}

void Bomb::Tick(float dt) {
    ThrowObject::Tick(dt);
    Mtx34f rotation;
    fn_8040C148(&rotation, &lbl_805E0720);
    fn_8040B884(&rotation, m_spin * dt);
    fn_8040C148(&m_matrix, &(const Mtx34f &)fn_8001E044(&m_matrix, &rotation));
    UpdateModel();
    if (m_model.get() != 0) {
        m_model->fn_80162FB0(dt);
    }
}

void Bomb::OnRangeEnd() {}

void Bomb::OnHit() {
    Vec3f position;
    fn_800091EC(&position, &(const Vec3f &)GetPosition());
    switch (m_type) {
    case 0:
        ::Bomb::BombBlast::Instantiate(position);
        break;
    case 1:
        HealingUnit::HealingBlast::Instantiate(GetThrower(), position, 0);
        break;
    }
}

void Bomb::UpdateModel() {
    if (m_model.get() != 0) {
        Vec3f position;
        fn_800091EC(&position, &(const Vec3f &)GetPosition());
        Mtx34f matrix;
        Mtx34f translation;
        fn_8040B034(&translation, &position);
        fn_8040C148(&matrix, &translation);
        fn_8040C148(&matrix, &(const Mtx34f &)fn_8001E044(&matrix, &m_matrix));
        fn_8040B720(&matrix, m_scale, m_scale, m_scale);
        m_model->fn_801633DC(&matrix);
    }
}

} // namespace ThrowVacuumBomb

/* Creates the bomb a vacuum lets go (name inferred; called by VacuumBomb::Bomb). */
extern "C" void fn_8033ED10(const boost::weak_ptr<CharInfo> &thrower,
                            const boost::shared_ptr<GadgetModelInfo> &model, int type,
                            const Vec3f &position, const Vec3f &rotation, const Vec3f &velocity,
                            float spin, float scale) {
    ThrowVacuumBomb::Bomb *bomb = new ThrowVacuumBomb::Bomb(thrower, model, type, position, rotation, velocity, s_noAcceleration,
                          spin, scale, s_range);
    bomb->SetEffect(0x254);
}
