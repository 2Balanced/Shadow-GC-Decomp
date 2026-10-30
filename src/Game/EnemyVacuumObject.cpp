/* CEnemyVacuumObject (class name from RTTI, in the anonymous namespace of EnemyVacuumObject.cpp):
 * an enemy's model while a vacuum pulls it in. The enemies (EnemyBaseAI, BkLarva) hand their
 * EnemyRender to it through fn_8028CFC4 when a VacuumCommand reaches them. It turns the model to
 * face the pull, spins it about its X and Z axes and grows it back to full size; when the vacuum
 * lets it go, it throws the enemy out as a TEnemyVacuumBullet (Game/EnemyVacuumBullet.cpp). Like
 * VacuumContainer::Container (Game/VacuumContainer.cpp). Names other than the class name are
 * inferred. Definitions are in reverse address order (deferred inlining emits functions
 * last-defined first). */
#include "Game/CharInfo.h"
#include "Game/EnemyRender.h"
#include "Game/GadgetUtil.h"
#include "Game/MathTypes.h"
#include "Game/VacuumObject.h"
#include "boost/shared_ptr.hpp"
#include "boost/weak_ptr.hpp"
#include "size_assert.h"

extern "C" {
/* Sets bit of a flag word (see FootingMovable.cpp). */
void fn_80014E1C(u32 *flags, int bit);
/* matrix * vector without the translation, the rotation that turns a onto b, and the blend of
 * two rotations (behaviour inferred; see VacuumContainer.cpp). */
Vec3f fn_8040B618(const Mtx34f *matrix, const Vec3f *vector);
Vec3f fn_8040F364(const Vec3f *a, const Vec3f *b);
Vec3f fn_8040F16C(const Vec3f *a, const Vec3f *b, int order, float t);
/* An angle wrapped into one turn (behaviour inferred). */
float fn_8040E99C(float angle);
/* The model: its matrix, scale and update (see Game/EnemyRender.cpp). */
void fn_802CD6F0(EnemyRender *render, const Mtx34f *matrix);
float fn_802CD614(EnemyRender *render);
void fn_802CD638(EnemyRender *render, float scale);
void fn_802CD824(EnemyRender *render, float dt);
void fn_802CD7E8(EnemyRender *render);
/* Throws the enemy out (Game/EnemyVacuumBullet.cpp). */
void fn_8028CD0C(const boost::weak_ptr<CharInfo> &thrower,
                 const boost::shared_ptr<EnemyRender> &render, const Vec3f &position,
                 const Vec3f &velocity, const Vec3f &rotation);
}

/* fn_8040C148 called as returning a copy of the matrix (as in VacuumObject.cpp). */
typedef Mtx34f (*MatrixCopyFn)(const Mtx34f *matrix);

namespace {

class CEnemyVacuumObject : public VacuumObject {
  public:
    CEnemyVacuumObject(VacuumCommand &command, const boost::shared_ptr<EnemyRender> &render,
                       bool single);
    virtual ~CEnemyVacuumObject();
    virtual void fn_80238268(const Vec3f &rotation);
    virtual void Tick(float dt);
    virtual void fn_80238184(const Vec3f &velocity);
    virtual Mtx34f fn_80237EE4();

    /* Creates the object for render at position and rotation, flying to the command's vacuum, and
     * starts a task for it. */
    static void Create(VacuumCommand &command, const boost::shared_ptr<EnemyRender> &render,
                       const Vec3f &position, const Vec3f &rotation, bool single);

    /* Places the model. */
    void UpdateModel();

    /* 0x08 */ Vec3f m_front; /* the model's front, which turns toward the vacuum */
    /* 0x14 */ Vec3f m_up;
    /* 0x20 */ Vec3f m_rotation;
    /* 0x2C */ float m_spinX;
    /* 0x30 */ float m_spinZ;
    /* 0x34 */ boost::shared_ptr<EnemyRender> m_render;
};
SIZE_ASSERT(CEnemyVacuumObject, 0x3C);

} // namespace

namespace {

void CEnemyVacuumObject::Create(VacuumCommand &command,
                                const boost::shared_ptr<EnemyRender> &render,
                                const Vec3f &position, const Vec3f &rotation, bool single) {
    boost::shared_ptr<VacuumObject> object =
        boost::shared_ptr<VacuumObject>(new CEnemyVacuumObject(command, render, single));
    object->fn_802382E0(position);
    object->fn_80238268(rotation);
    object->StartEffect(0x252);
    new VacuumObjectTask(object);
}

CEnemyVacuumObject::CEnemyVacuumObject(VacuumCommand &command,
                                       const boost::shared_ptr<EnemyRender> &render, bool single)
    : VacuumObject(single ? 1 : fn_80237E34(&command), command), m_front(0.0f, 0.0f, -1.0f),
      m_up(0.0f, -1.0f, 0.0f), m_rotation(0.0f, 0.0f, 0.0f) {
    m_render = render;
    m_spinX = 0.0f;
    m_spinZ = 0.0f;
}

CEnemyVacuumObject::~CEnemyVacuumObject() {}

void CEnemyVacuumObject::fn_80238268(const Vec3f &rotation) {
    VacuumObject::fn_80238268(rotation);
    m_rotation = rotation;
}

void CEnemyVacuumObject::Tick(float dt) {
    VacuumObject::Tick(dt);
    float progress = GetProgress();
    Mtx34f *matrix = GetMatrix();
    Vec3f target;
    target = fn_8040F364(&(const Vec3f &)fn_8040B618(matrix, &m_front),
                         &(const Vec3f &)fn_8040B618(matrix, &m_up));
    Vec3f rotation(0.0f, 0.0f, 0.0f);

    if (progress <= 0.0f) {
        rotation = m_rotation;
    }
    if (progress >= 1.0f) {
        rotation = target;
    } else {
        rotation = fn_8040F16C(&m_rotation, &target, 2, progress);
    }

    float spin = progress * dt;
    m_spinX = fn_8040E99C(m_spinX + -18.0f * spin);
    m_spinZ = fn_8040E99C(m_spinZ + lbl_805EFEE8 * spin);
    rotation.x += m_spinX;
    rotation.z += m_spinZ;
    VacuumObject::fn_80238268(rotation);
    UpdateModel();

    if (m_render.get() != 0) {
        float scale = fn_802CD614(m_render.get());
        if (scale < 1.0f) {
            scale += 2.0f * dt;
            if (scale > 1.0f) {
                scale = 1.0f;
            }
            fn_802CD638(m_render.get(), scale);
        }
        fn_802CD824(m_render.get(), dt);
        fn_802CD7E8(m_render.get());
    }
}

void CEnemyVacuumObject::fn_80238184(const Vec3f &velocity) {
    fn_80014E1C(GetFlags(), 1);
    Vec3f position;
    ConstructVector(&position, fn_802382B0(this));
    Vec3f rotation;
    rotation = fn_80238248(this);
    fn_8028CD0C(fn_80238058(this), m_render, position, velocity, rotation);
}

Mtx34f CEnemyVacuumObject::fn_80237EE4() {
    Mtx34f matrix;
    fn_8040C148(&matrix, &(const Mtx34f &)CalcMatrix());
    Vec3f offset(0.0f, 0.0f, 5.0f);
    fn_8040B59C(&matrix, &offset);
    return ((MatrixCopyFn)fn_8040C148)(&matrix);
}

void CEnemyVacuumObject::UpdateModel() {
    if (m_render.get() != 0) {
        Vec3f position;
        ConstructVector(&position, fn_802382B0(this));
        Vec3f rotation = fn_80238248(this);
        Mtx34f matrix;
        Mtx34f translation;
        fn_8040B034(&translation, &position);
        fn_8040C148(&matrix, &translation);
        fn_8040B848(&matrix, rotation.y);
        fn_8040B884(&matrix, rotation.x);
        fn_8040B80C(&matrix, rotation.z);
        fn_802CD6F0(m_render.get(), &matrix);
    }
}

} // namespace

/* Hands an enemy's model to a new vacuum object (name inferred; single makes it of kind 1, see
 * VacuumObject.h). */
extern "C" void fn_8028CFC4(VacuumCommand *command, const boost::shared_ptr<EnemyRender> &render,
                            const Vec3f *position, const Vec3f *rotation, bool single) {
    CEnemyVacuumObject::Create(*command, render, *position, *rotation, single);
}

/* The speed the enemies spin at about X (name inferred; also read by EnemyVacuumBullet.cpp). */
extern "C" float fn_8028CFBC(void) {
    return -18.0f;
}
