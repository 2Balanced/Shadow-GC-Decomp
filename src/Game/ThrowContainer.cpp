/* ThrowContainer::Container (class name from RTTI): a container in flight after it was thrown, or
 * after a vacuum let it go. It is a ThrowObject that carries the container's model, turning it
 * about its X axis as it flies; when it hits something it breaks the model into debris, releases
 * the container's content and rewards the thrower, and when it has flown its range it only
 * rewards the thrower. The container gadgets (Game/ContainerNormal.cpp, ...) create it through
 * fn_8027B338. File name inferred from the RTTI name. Names marked "(name inferred)" are not
 * original. Definitions are in reverse address order (deferred inlining emits functions
 * last-defined first).
 *
 * Not yet matching: in fn_8027B3E8 retail loads s_range for the constructor call right after the
 * float moves, GC/2.7 after the integer argument moves (a scheduling difference; the parameter
 * order and the statics' linkage were varied without effect). Everything else, including the
 * data, matches. */
#include "Game/CharInfo.h"
#include "Game/GadgetModelInfo.h"
#include "Game/MathTypes.h"
#include "Game/ThrowObject.h"
#include "boost/shared_ptr.hpp"
#include "boost/weak_ptr.hpp"
#include "size_assert.h"

extern "C" {
/* Rewards the character (see ContainerWeapon.cpp; behaviour and meaning of reward inferred). */
int fn_8020642C(const boost::shared_ptr<CharInfo> &character, int reward);
/* Releases the content: item value of kind select at position (behaviour inferred). */
void fn_8033E828(int select, int value, const Vec3f *position, const Vec3f *rotation);
/* Breaks the model into debris (behaviour inferred). */
void fn_8033E644(boost::shared_ptr<GadgetModelInfo> model, int kind, const Vec3f *position,
                 const Vec3f *rotation);
/* The rotation of a matrix (see ContainerWeapon.cpp). */
void fn_800494F8(Vec3f *rotation, const Mtx34f *matrix);
/* a * b (behaviour inferred). */
Mtx34f fn_8001E044(const Mtx34f *a, const Mtx34f *b);
}

namespace ThrowContainer {

class Container : public ThrowObject {
  public:
    Container(const boost::weak_ptr<CharInfo> &thrower,
              const boost::shared_ptr<GadgetModelInfo> &model, int kind, int select, int value,
              const Vec3f &position, const Vec3f &rotation, const Vec3f &velocity,
              const Vec3f &acceleration, int reward, float spin, float scale, float range);
    virtual ~Container();
    virtual void Tick(float dt);
    virtual void OnRangeEnd();
    virtual void OnHit();

    /* Places the model (name inferred). */
    void UpdateModel();

    /* 0x30 */ boost::shared_ptr<GadgetModelInfo> m_model;
    /* 0x38 */ s32 m_kind;   /* the debris kind */
    /* 0x3C */ s32 m_select; /* the content */
    /* 0x40 */ s32 m_value;
    /* 0x44 */ Mtx34f m_matrix; /* the model's rotation */
    /* 0x74 */ float m_spin;    /* about X, per second */
    /* 0x78 */ float m_scale;
    /* 0x7C */ s32 m_reward;
};
SIZE_ASSERT(Container, 0x80);

} // namespace ThrowContainer

using ThrowContainer::Container;

/* A value (name inferred). As a call it is not a constant expression, so the values are stored by
 * the static initializer, as in retail. */
inline float Plain(float value) {
    return value;
}

/* The gravity, and the range and the (lack of) acceleration of a container a vacuum lets go (names
 * inferred). */
static float s_gravity = Plain(5.0f);
static float s_range = Plain(600.0f);
static Vec3f s_noAcceleration(0.0f, 0.0f, 0.0f);

Container::Container(const boost::weak_ptr<CharInfo> &thrower,
                     const boost::shared_ptr<GadgetModelInfo> &model, int kind, int select,
                     int value, const Vec3f &position, const Vec3f &rotation,
                     const Vec3f &velocity, const Vec3f &acceleration, int reward, float spin,
                     float scale, float range)
    : ThrowObject(thrower, position, velocity, acceleration, s_gravity, range) {
    fn_8040C148(&m_matrix, &lbl_805E0720);
    m_model = model;
    m_kind = kind;
    m_select = select;
    m_value = value;
    fn_8040B848(&m_matrix, rotation.y);
    fn_8040B884(&m_matrix, rotation.x);
    fn_8040B80C(&m_matrix, rotation.z);
    m_spin = spin;
    m_scale = scale;
    m_reward = reward;
}

Container::~Container() {}

void Container::Tick(float dt) {
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

void Container::OnRangeEnd() {
    if (boost::shared_ptr<CharInfo> thrower = GetThrower().lock()) {
        fn_8020642C(thrower, m_reward);
    }
}

void Container::OnHit() {
    Vec3f position;
    fn_800091EC(&position, &(const Vec3f &)GetPosition());
    Vec3f rotation(0.0f, 0.0f, 0.0f);
    fn_800494F8(&rotation, &m_matrix);
    fn_8033E644(m_model, m_kind, &position, &rotation);
    fn_8033E828(m_select, m_value, &position, &rotation);
    if (boost::shared_ptr<CharInfo> thrower = GetThrower().lock()) {
        fn_8020642C(thrower, m_reward);
    }
}

void Container::UpdateModel() {
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

/* Creates the container a vacuum lets go (name inferred; called by 0x80254384). */
extern "C" void fn_8027B3E8(const boost::weak_ptr<CharInfo> &thrower,
                            const boost::shared_ptr<GadgetModelInfo> &model, int kind,
                            int select, int value, const Vec3f &position,
                            const Vec3f &rotation, const Vec3f &velocity, float spin,
                            float scale, int reward) {
    Container *container = new Container(thrower, model, kind, select, value, position, rotation,
                                         velocity, s_noAcceleration, reward, spin, scale,
                                         s_range);
    container->SetEffect(0x254);
}

/* Creates the container a character throws (name inferred; see ContainerWeapon.cpp). */
extern "C" void fn_8027B338(const boost::weak_ptr<CharInfo> &thrower,
                            const boost::shared_ptr<GadgetModelInfo> &model, int kind,
                            int select, int value, const Vec3f &position,
                            const Vec3f &rotation, const Vec3f &velocity, float spin,
                            float scale, float range, const Vec3f &acceleration, int reward) {
    new Container(thrower, model, kind, select, value, position, rotation, velocity,
                  acceleration, reward, spin, scale, range);
}
