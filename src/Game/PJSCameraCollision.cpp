/* PJSCamera::Collision::ProcessDeltaMoveNoLook and ProcessRay2Eye (class names and bases from
 * RTTI): the two ways a camera mode (CameraModeFixLookChar) keeps its eye clear of the stage, each
 * handed out through a shared_ptr to their base ProcessBase (Game/PJSCameraMode.h). ProcessRay2Eye
 * pulls the eye in every frame to where the line from the watched point hits the stage
 * (fn_801C7A20); ProcessDeltaMoveNoLook does that only on the first frame after a reset and when
 * the eye's own movement since the last frame, stretched by the radius, hits the stage. File name
 * from the classes' namespace; other names are inferred. Definitions are in reverse address order
 * (deferred inlining emits functions last-defined first). */
#include "Game/CameraModeUser.h"
#include "Game/CharInfo.h"
#include "Game/MathTypes.h"
#include "Game/PJSCameraMode.h"
#include "boost/shared_ptr.hpp"
#include "size_assert.h"

using PJSCamera::Collision::ProcessBase;
using SonicteamUSA::System::Camera::CameraMode;

namespace SonicteamUSA {
namespace System {
namespace Collision {
/* CollisionResult (class name from the RTTI of its shared_ptr control block): the first hit of a
 * line test (fn_801C7C70; see Game/CameraModeGeneral.cpp). */
class CollisionResult {
  public:
    /* 0x00 */ u8 field_0x0[0x14];
    /* 0x14 */ void *m_object;
};
} // namespace Collision
} // namespace System
} // namespace SonicteamUSA

using SonicteamUSA::System::Collision::CollisionResult;

extern "C" {
/* A 32-bit flag word (see Game/BKTankController.h). */
void fn_80014E04(u32 *flags);
void fn_80014E10(u32 *flags);
void fn_80014E1C(u32 *flags, int bit);
void fn_8006127C(u32 *flags, int bit);
bool fn_80014E34(u32 *flags, int bit);

/* Releases a hit (CollisionResult's destructor; see checked_delete below). */
void fn_800265A4(CollisionResult *hit, int arg1);

/* The first hit of the line from one point to another, or 0 (see Game/CameraModeGeneral.cpp). */
CollisionResult *fn_801C7C70(const Vec3f *from, const Vec3f *to);
/* Pulls a mode's eye in to where the line from point to it, stretched by radius, hits the stage;
 * whether it did (behaviour from its body). */
bool fn_801C7A20(CameraMode *mode, Vec3f *point, CharInfo *chara, float radius);

/* Vector routines (behaviour inferred). */
Vec3f fn_8040CDD0(const Vec3f *a, const Vec3f *b); /* a + b */
float fn_8040CE74(Vec3f *vector);                  /* normalises */
void fn_8040CE38(Vec3f *vector, float scale);      /* vector *= scale */
}

/* fn_8040CD74 called as returning a - b by value (see Game/ArkCannon.cpp). */
typedef Vec3f (*VectorSubtractFn)(const Vec3f *a, const Vec3f *b);

/* Retail releases a hit through its out-of-line destructor (as Game/CameraModeGeneral.cpp does). */
namespace boost {
template<> inline void checked_delete<CollisionResult>(CollisionResult *x) {
    fn_800265A4(x, 1);
}
} // namespace boost

/* The flag word of ProcessDeltaMoveNoLook (struct name unknown; see Game/BKTankController.h). */
struct ProcessFlags {
    ProcessFlags() {
        fn_80014E04(&m_bits);
    }
    void Clear() {
        fn_80014E10(&m_bits);
    }
    void Set(int bit) {
        fn_80014E1C(&m_bits, bit);
    }
    void Clear(int bit) {
        fn_8006127C(&m_bits, bit);
    }
    bool Test(int bit) {
        return fn_80014E34(&m_bits, bit);
    }

    u32 m_bits;
};

/* Whether two vectors are equal. */
inline bool operator==(const Vec3f &a, const Vec3f &b) {
    return a.x == b.x && a.y == b.y && a.z == b.z;
}

/* The game sets this in __sinit rather than in .sdata, as an initializer that is not a constant
 * expression does; the inline call reproduces that (as in Game/PlayerBehaviorSuper.cpp). */
inline float DynamicFloat(float value) {
    return value;
}

/* How far beyond the eye the line tests reach (name inferred). */
static const float s_radius = DynamicFloat(2.0f);

namespace PJSCamera {
namespace Collision {

inline ProcessBase::~ProcessBase() {}

/* ProcessDeltaMoveNoLook (class name and base from RTTI; vtable 0x8053C24C). */
class ProcessDeltaMoveNoLook : public ProcessBase {
  public:
    ProcessDeltaMoveNoLook();
    virtual ~ProcessDeltaMoveNoLook();
    virtual void vf_0x0C();
    virtual bool Process(CameraMode *mode, Vec3f *eye, CharInfo *chara);

    /* Pulls the eye in when it is the first frame or the eye's movement hits the stage. */
    bool Push(CameraMode *mode, Vec3f *eye, CharInfo *chara);

    /* 0x04 */ ProcessFlags m_flags; /* bit 0: the first frame after a reset */
    /* 0x08 */ Vec3f m_lastEye;
};
SIZE_ASSERT(ProcessDeltaMoveNoLook, 0x14);

/* ProcessRay2Eye (class name and base from RTTI; vtable 0x8053C2C8). */
class ProcessRay2Eye : public ProcessBase {
  public:
    virtual void vf_0x0C() {}
    virtual bool Process(CameraMode *mode, Vec3f *eye, CharInfo *chara);
};
SIZE_ASSERT(ProcessRay2Eye, 0x4);

bool ProcessRay2Eye::Process(CameraMode *mode, Vec3f *eye, CharInfo *chara) {
    return fn_801C7A20(mode, eye, chara, s_radius);
}

} // namespace Collision
} // namespace PJSCamera

using PJSCamera::Collision::ProcessDeltaMoveNoLook;
using PJSCamera::Collision::ProcessRay2Eye;

extern "C" boost::shared_ptr<ProcessBase> fn_801C6EB0(void) {
    return boost::shared_ptr<ProcessBase>(new ProcessRay2Eye);
}

namespace PJSCamera {
namespace Collision {

ProcessDeltaMoveNoLook::ProcessDeltaMoveNoLook() {
    vf_0x0C();
}

ProcessDeltaMoveNoLook::~ProcessDeltaMoveNoLook() {}

void ProcessDeltaMoveNoLook::vf_0x0C() {
    m_flags.Clear();
    m_flags.Set(0);
    fn_800091D0(&m_lastEye, &lbl_805E07A0);
}

bool ProcessDeltaMoveNoLook::Process(CameraMode *mode, Vec3f *eye, CharInfo *chara) {
    bool pushed = Push(mode, eye, chara);
    fn_800091D0(&m_lastEye, &mode->m_eye);
    return pushed;
}

bool ProcessDeltaMoveNoLook::Push(CameraMode *mode, Vec3f *eye, CharInfo *chara) {
    if (m_flags.Test(0)) {
        bool pushed = fn_801C7A20(mode, eye, chara, s_radius);
        m_flags.Clear(0);
        return pushed;
    }
    Vec3f now;
    fn_800091EC(&now, &mode->m_eye);
    if (now == m_lastEye) {
        return false;
    }
    Vec3f move;
    fn_800091EC(&move, &(const Vec3f &)((VectorSubtractFn)fn_8040CD74)(&now, &m_lastEye));
    float length = fn_8040CF50(&move);
    fn_8040CE74(&move);
    fn_8040CE38(&move, length + s_radius);
    boost::shared_ptr<CollisionResult> hit(
        fn_801C7C70(&m_lastEye, &(const Vec3f &)fn_8040CDD0(&m_lastEye, &move)));
    if (hit.get() == 0) {
        return false;
    }
    return fn_801C7A20(mode, eye, chara, s_radius);
}

} // namespace Collision
} // namespace PJSCamera

extern "C" boost::shared_ptr<ProcessBase> fn_801C68E8(void) {
    return boost::shared_ptr<ProcessBase>(new ProcessDeltaMoveNoLook);
}
