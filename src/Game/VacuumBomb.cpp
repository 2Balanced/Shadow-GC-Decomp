/* VacuumBomb (class name from RTTI: VacuumBomb::Bomb, which also gives the file name): a bomb's
 * model while a vacuum pulls it in. Bomb gadgets hand their model to it through fn_8033F0F8 when a
 * VacuumCommand reaches them. Like VacuumContainer::Container (Game/VacuumContainer.cpp), it spins
 * the model about the bomb's axis nearest to the pull and, when the vacuum lets it go, throws the
 * bomb out again (ThrowVacuumBomb::Bomb, through fn_8033ED10). Names other than the class name are
 * inferred. Definitions are in reverse address order (deferred inlining emits functions
 * last-defined first). */
#include "Game/CharInfo.h"
#include "Game/GadgetModelInfo.h"
#include "Game/GadgetUtil.h"
#include "Game/VacuumObject.h"
#include "boost/shared_ptr.hpp"
#include "boost/weak_ptr.hpp"
#include "size_assert.h"

/* MSL's std::fabs for floats, which the game calls out of line (as in DevilDoomCtrlUnit.cpp). */
namespace std {
float fabs(float x);
} // namespace std

extern "C" {
/* Sets bit of a flag word (see FootingMovable.cpp). */
void fn_80014E1C(u32 *flags, int bit);
/* matrix * vector without the translation, a matrix's translation, the cross product of a and b,
 * vector turned about the y, x and z axes, and the blend of two rotations (behaviour inferred; see
 * Game/VacuumContainer.cpp). */
Vec3f fn_8040B618(const Mtx34f *matrix, const Vec3f *vector);
Vec3f fn_8040FB08(const Mtx34f *matrix);
Vec3f fn_8040F364(const Vec3f *a, const Vec3f *b);
Vec3f fn_8040D0AC(const Vec3f *vector, float angle);
Vec3f fn_8040D118(const Vec3f *vector, float angle);
Vec3f fn_8040D044(const Vec3f *vector, float angle);
Vec3f fn_8040F16C(const Vec3f *a, const Vec3f *b, int order, float t);
/* An angle wrapped into one turn (behaviour inferred). */
float fn_8040E9D8(float angle);
/* The angles of a rotation matrix (behaviour inferred). */
void fn_800494F8(Vec3f *rotation, const Mtx34f *matrix);
/* Throws the bomb's model out as a ThrowVacuumBomb::Bomb (Game/ThrowVacuumBomb.cpp). */
void fn_8033ED10(const boost::weak_ptr<CharInfo> &thrower,
                 const boost::shared_ptr<GadgetModelInfo> &model, int type,
                 const Vec3f &position, const Vec3f &rotation, const Vec3f &velocity, float spin,
                 float scale);
/* value clamped to [low, high] (as in Coaster.cpp). */
const float &fn_80070460(const float &value, const float &low, const float &high);
}

/* fn_8040CD74 called as returning a - b by value (as in ChaoHolder.cpp). */
typedef Vec3f (*VectorSubtractFn)(const Vec3f *a, const Vec3f *b);
/* fn_8040C148 called as returning a copy of the matrix (as in VacuumObject.cpp). */
typedef Mtx34f (*MatrixCopyFn)(const Mtx34f *matrix);

namespace VacuumBomb {

class Bomb : public VacuumObject {
  public:
    Bomb(VacuumCommand &command, const boost::shared_ptr<GadgetModelInfo> &model, int type);
    virtual ~Bomb();
    virtual void fn_80238268(const Vec3f &rotation);
    virtual void Tick(float dt);
    virtual void fn_80238184(const Vec3f &velocity);
    virtual Mtx34f fn_80237EE4();

    /* Picks the bomb's axis nearest to the direction of the pull. */
    void ComputeAxes();
    /* Applies the bomb's rotation and spin to matrix. */
    void ApplyRotation(Mtx34f *matrix);
    /* Places the model. */
    void UpdateMatrix();

    /* 0x08 */ Vec3f m_axis;  /* the bomb's axis nearest to the pull */
    /* 0x14 */ Vec3f m_axis2; /* m_axis with its components rotated */
    /* 0x20 */ Vec3f m_rotation;
    /* 0x2C */ boost::shared_ptr<GadgetModelInfo> m_model;
    /* 0x34 */ int m_type; /* handed to ThrowVacuumBomb::Bomb */
    /* 0x38 */ float m_spin;
};
SIZE_ASSERT(Bomb, 0x3C);

} // namespace VacuumBomb

using VacuumBomb::Bomb;

/* The bounds of the progress the spin speed is scaled by. */
/* Retail keeps the zero in .sdata. */
#pragma explicit_zero_data on
static float s_progressMin = 0.0f;
#pragma explicit_zero_data reset
static float s_progressMax = 1.0f;

/* A tenth of length. As a call it is not a constant expression, so the constant below is stored by
 * a static initializer and read from memory, as in retail. */
inline float Tenth(float length) {
    return length / 10.0f;
}

/* A tenth of the vacuum objects' length (see VacuumObject.h), the model's scale. */
static const float s_scale = Tenth(lbl_805F72B0);

Bomb::Bomb(VacuumCommand &command, const boost::shared_ptr<GadgetModelInfo> &model, int type)
    : VacuumObject(fn_80237E34(&command), command), m_rotation(0.0f, 0.0f, 0.0f) {
    m_type = type;
    m_spin = 0.0f;
    m_model = model;
}

Bomb::~Bomb() {}

void Bomb::fn_80238268(const Vec3f &rotation) {
    VacuumObject::fn_80238268(rotation);
    m_rotation = rotation;
}

void Bomb::Tick(float dt) {
    VacuumObject::Tick(dt);
    float progress = GetProgress();
    Mtx34f *matrix = GetMatrix();
    Vec3f target;
    target = fn_8040F364(&(const Vec3f &)fn_8040B618(matrix, &m_axis),
                         &(const Vec3f &)fn_8040B618(matrix, &m_axis2));
    Vec3f rotation(0.0f, 0.0f, 0.0f);

    if (progress <= 0.0f) {
        rotation = m_rotation;
    }
    if (progress >= 1.0f) {
        rotation = target;
    } else {
        rotation = fn_8040F16C(&m_rotation, &target, 2, progress);
    }
    fn_80238268(rotation);

    float t = GetProgress();
    t = fn_80070460(t, s_progressMin, s_progressMax);
    m_spin += dt * (lbl_805EFEE8 * t);
    m_spin = fn_8040E9D8(m_spin);
    UpdateMatrix();
    if (m_model.px != 0) {
        m_model->fn_80162FB0(dt);
    }
}

void Bomb::fn_80238184(const Vec3f &velocity) {
    fn_80014E1C(GetFlags(), 1);
    Mtx34f matrix;
    fn_8040C148(&matrix, &lbl_805E0720);
    ApplyRotation(&matrix);
    Vec3f angles(0.0f, 0.0f, 0.0f);
    fn_800494F8(&angles, &matrix);
    fn_8033ED10(fn_80238058(this), m_model, m_type, fn_802382B0(this), angles, velocity,
                lbl_805EFEE8, s_scale);
}

Mtx34f Bomb::fn_80237EE4() {
    Mtx34f matrix;
    fn_8040C148(&matrix, &(const Mtx34f &)CalcMatrix());
    Vec3f offset(0.0f, 0.0f, 5.0f);
    fn_8040B59C(&matrix, &offset);
    return ((MatrixCopyFn)fn_8040C148)(&matrix);
}

void Bomb::ComputeAxes() {
    Mtx34f *matrix = GetMatrix();
    fn_800091D0(&m_axis, &(const Vec3f &)((VectorSubtractFn)fn_8040CD74)(
                             &(const Vec3f &)fn_802382B0(this),
                             &(const Vec3f &)fn_8040FB08(matrix)));
    AssignVector(m_axis, fn_8040D0AC(&m_axis, -m_rotation.y));
    AssignVector(m_axis, fn_8040D118(&m_axis, -m_rotation.x));
    AssignVector(m_axis, fn_8040D044(&m_axis, -m_rotation.z));

    float x = std::fabs(m_axis.x);
    float y = std::fabs(m_axis.y);
    float z = std::fabs(m_axis.z);
    if (x > y) {
        if (z > x) {
            fn_800091D0(&m_axis, &(const Vec3f &)Vec3f(0.0f, 0.0f, m_axis.z >= 0.0f ? 1.0f : -1.0f));
        } else {
            fn_800091D0(&m_axis, &(const Vec3f &)Vec3f(m_axis.x >= 0.0f ? 1.0f : -1.0f, 0.0f, 0.0f));
        }
    } else if (z > y) {
        fn_800091D0(&m_axis, &(const Vec3f &)Vec3f(0.0f, 0.0f, m_axis.z >= 0.0f ? 1.0f : -1.0f));
    } else {
        fn_800091D0(&m_axis, &(const Vec3f &)Vec3f(0.0f, m_axis.y >= 0.0f ? 1.0f : -1.0f, 0.0f));
    }

    m_axis2.x = m_axis.y;
    m_axis2.y = m_axis.z;
    m_axis2.z = m_axis.x;
}

void Bomb::ApplyRotation(Mtx34f *matrix) {
    Vec3f rotation = fn_80238248(this);
    fn_8040B848(matrix, rotation.y);
    fn_8040B884(matrix, rotation.x);
    fn_8040B80C(matrix, rotation.z);
    fn_8040B884(matrix, m_spin);
}

void Bomb::UpdateMatrix() {
    if (m_model.px != 0) {
        Mtx34f matrix;
        Mtx34f translation;
        Vec3f position;
        ConstructVector(&position, fn_802382B0(this));
        Vec3f rotation = fn_80238248(this);

        fn_8040B034(&translation, &position);
        fn_8040C148(&matrix, &translation);
        ApplyRotation(&matrix);
        fn_8040B720(&matrix, s_scale, s_scale, s_scale);
        m_model->fn_801633DC(&matrix);
    }
}

/* Hands a bomb's model to a new Bomb at position and rotation, flying to the command's vacuum, and
 * starts a task for it (as fn_80254384 does for containers). */
extern "C" void fn_8033F0F8(VacuumCommand *command, boost::shared_ptr<GadgetModelInfo> *model,
                            int type, const Vec3f *position, const Vec3f *rotation) {
    Bomb *bomb = new Bomb(*command, *model, type);
    boost::shared_ptr<VacuumObject> object = boost::shared_ptr<VacuumObject>(bomb);
    object->fn_802382E0(*position);
    object->fn_80238268(*rotation);
    object->StartEffect(0x252);
    bomb->ComputeAxes();
    new VacuumObjectTask(object);
}
