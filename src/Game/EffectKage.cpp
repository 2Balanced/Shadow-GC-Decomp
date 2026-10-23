/* @unnamed@EffectKage_cpp@::KageTask (class name from RTTI; the anonymous namespace and the file
 * name come from it): the blob shadows ("kage") under characters and objects. Each frame the code that wants a
 * shadow adds a placement matrix (fn_803563D0, fn_80356400, fn_80356544); the task draws the shadow
 * model (character file 12, model 8) once per matrix from a draw callback of its Render and empties
 * the list on its next tick. fn_803566D4 and fn_803566B0 create and end the one task. Names other
 * than the class's are inferred. Definitions are in reverse address order (deferred inlining emits
 * functions last-defined first). */
/* The list's values have no destructor (see <list>). */
#define MSL_LIST_TRIVIAL_VALUES
#include <list>
#include <boost/bind.hpp>
#include <boost/function.hpp>

#include "Game/CharacterFiles.h"
#include "Game/DebugDispShape.h"
#include "Game/MathTypes.h"
#include "Game/Render.h"
#include "Game/Task.h"
#include "boost/shared_ptr.hpp"
#include <dolphin/types.h>
#include "size_assert.h"

extern "C" {
/* Where new tasks go: the root of the task tree and its task of a layer (see Game/Gadget.h). */
void *fn_8001C4F8(void);
Task *fn_801EB304(void *root, int layer);
/* The model loader, and a model of a loaded file (behaviour inferred). */
void *fn_800122D4(void);
RpClump *fn_8001277C(void *loader, s32 slot, int index);
/* Creates a Render (behaviour inferred; see Game/ElecSearchLight.cpp). */
boost::shared_ptr<Render> fn_8004E24C(void *arg0);
/* A length in metres in the game's units (behaviour inferred). */
float fn_8005AE20(float value);
/* vector * scale, a + b, a . b, a x b, the normalised vector, the arc cosine. */
Vec3f fn_800091A4(const Vec3f *vector, float scale);
Vec3f fn_80093204(const Vec3f *a, const Vec3f *b);
float fn_8040CF90(const Vec3f *a, const Vec3f *b);
Vec3f fn_8040CFB8(const Vec3f *a, const Vec3f *b);
Vec3f fn_8040CEE8(const Vec3f *vector);
float fn_80015D68(float value);
/* The rotation by angle about axis, and matrix = matrix * other (behaviour inferred). */
Mtx34f fn_8040FD8C(const Vec3f *axis, float angle);
void fn_8040BAC4(Mtx34f *matrix, const Mtx34f *other);
/* The Euler angles of a direction with an up vector (behaviour inferred). */
Vec3f fn_8040F364(const Vec3f *forward, const Vec3f *up);
}

/* A shadow cast on the ground (as in Game/BossCtrlUnit.h): its position and the ground's normal,
 * and whether it is shown. */
struct UnkShadow_8009368C {
    /* 0x00 */ Vec3f m_position;
    /* 0x0C */ Vec3f m_normal;
    /* 0x18 */ u32 field_0x18;
    /* 0x1C */ u32 field_0x1C;
    /* 0x20 */ float field_0x20;
    /* 0x24 */ bool m_visible;
};

namespace {

/* The placement of one shadow (struct name inferred); its copies go through fn_8040C148. The
 * conversion from a plain matrix is a member template defined after the class: retail emits it
 * with the unit's late template instances, after the task's constructor. */
struct KageMatrix : public Mtx34f {
    template <class M> KageMatrix(const M &matrix);
    KageMatrix(const KageMatrix &matrix) {
        Copy(matrix);
    }

    void Copy(const Mtx34f &matrix) {
        fn_8040C148(this, &matrix);
    }
};

class KageTask : public Task {
  public:
    KageTask();
    virtual ~KageTask();
    virtual void Tick(float dt);

    /* The draw callback: one shadow per matrix. */
    void Draw(RednerDrawArgs &args);
    /* Adds a shadow for this frame. */
    void Add(KageMatrix matrix);

    /* 0x28 */ RpClump *m_clump;
    /* 0x2C */ boost::shared_ptr<Render> m_render;
    /* 0x34 */ std::list<KageMatrix> m_matrices;
    /* 0x40 */ s32 m_state; /* 1 once the model is loaded and the callback registered */
    /* 0x44 */ u32 field_0x44;
};
SIZE_ASSERT(KageTask, 0x48);

KageTask *s_task;

template <class M> KageMatrix::KageMatrix(const M &matrix) {
    Copy(matrix);
}

} // namespace

KageTask::KageTask() : Task(fn_801EB304(fn_8001C4F8(), 6)) {
    m_name = "Kage";
    m_clump = 0;
    m_state = 0;
}

KageTask::~KageTask() {
    s_task = 0;
}

void KageTask::Tick(float dt) {
    switch (m_state) {
    case 0:
        if (fn_80078C88()->IsLoaded(12)) {
            void *loader = fn_800122D4();
            s32 handle = fn_80078C88()->GetHandle(12);
            m_clump = fn_8001277C(loader, handle, 8);
            if (m_clump != 0) {
                m_render = fn_8004E24C(fn_80046178()->field_0x18);
                fn_80062CD0(&m_render->m_drawCallbacks, boost::bind(&KageTask::Draw, this, _1));
                m_state++;
            }
        }
        break;
    }
    m_matrices.clear();
}

void KageTask::Add(KageMatrix matrix) {
    m_matrices.push_back(matrix);
}

void KageTask::Draw(RednerDrawArgs &args) {
    RwFrame *frame = m_clump->frame;
    for (std::list<KageMatrix>::iterator it = m_matrices.begin(); it != m_matrices.end(); it++) {
        frame->modelling = fn_800591F0(&*it);
        fn_8048722C(frame);
        fn_80456BC4(m_clump);
    }
}

extern "C" void fn_803566D4(void) {
    if (s_task == 0) {
        s_task = new KageTask();
    }
}

extern "C" void fn_803566B0(void) {
    if (s_task != 0) {
        s_task->m_flags |= 1;
        s_task = 0;
    }
}

/* Adds a shadow at position (lifted a little along up), turned to direction, of size by
 * size * aspect (names inferred). */
extern "C" void fn_80356544(const Vec3f *position, const Vec3f *up, const Vec3f *direction,
                            float aspect, float size) {
    if (s_task != 0) {
        Mtx34f matrix;
        Mtx34f translation;
        fn_8040B034(&translation, &(const Vec3f &)fn_80093204(
                                      position, &(const Vec3f &)fn_800091A4(up, fn_8005AE20(0.02f))));
        fn_8040C148(&matrix, &translation);
        Vec3f side;
        fn_800091EC(&side, &(const Vec3f &)fn_8040CFB8(up, direction));
        Vec3f forward;
        fn_800091EC(&forward, &(const Vec3f &)fn_8040CEE8(&(const Vec3f &)fn_8040CFB8(&side, up)));
        Vec3f angles = fn_8040F364(&forward, up);
        fn_8040B848(&matrix, angles.y);
        fn_8040B884(&matrix, angles.x);
        fn_8040B80C(&matrix, angles.z);
        fn_8040B720(&matrix, size, 0.0f, size * aspect);
        s_task->Add(matrix);
    }
}

/* Adds a shadow at position (lifted a little along normal), lying on the plane of normal, of
 * size size (names inferred). */
extern "C" void fn_80356400(const Vec3f *position, const Vec3f *normal, float size) {
    if (s_task != 0) {
        Mtx34f matrix;
        Mtx34f translation;
        fn_8040B034(&translation, &(const Vec3f &)fn_80093204(
                                      position, &(const Vec3f &)fn_800091A4(normal, fn_8005AE20(0.02f))));
        fn_8040C148(&matrix, &translation);
        Vec3f up(0.0f, 1.0f, 0.0f);
        Vec3f axis;
        float cosAngle = fn_8040CF90(&up, normal);
        if (cosAngle < 0.9995f) {
            fn_800091D0(&axis, &(const Vec3f &)fn_8040CFB8(&up, normal));
            float angle = fn_80015D68(cosAngle);
            fn_8040BAC4(&matrix,
                        &(const Mtx34f &)fn_8040FD8C(&(const Vec3f &)fn_8040CEE8(&axis), angle));
        }
        fn_8040B720(&matrix, size, 0.0f, size);
        s_task->Add(matrix);
    }
}

/* Adds the shadow of shadow, if it is shown, of size size (name inferred). */
extern "C" void fn_803563D0(UnkShadow_8009368C *shadow, float size) {
    if (shadow->m_visible) {
        fn_80356400(&shadow->m_position, &shadow->m_normal, size);
    }
}
