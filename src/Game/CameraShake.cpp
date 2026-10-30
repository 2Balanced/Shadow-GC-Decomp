/* SonicteamUSA::System::Camera::CameraShake and CameraRequest (class names from the RTTI of their
 * shared_ptr control blocks; Game/CameraShake.h, Game/CameraManagerUnit.h): a camera unit's shake,
 * made from one of three kinds (fn_8000EDD0) or from its values, which moves the camera along its
 * up vector each frame (fn_8000EFC0) until it is over (fn_8000EF58); and a camera request's
 * constructor, destructor, adjust (fn_8000EC44) and order (fn_8000EC28). The unit is split from
 * CameraMode before it by that unit's .sdata2 and from the screen code after it by its own .sdata2
 * (0x805F1760..0x805F1788). File name from the class; other names are inferred. Definitions are in
 * reverse address order (deferred inlining emits functions last-defined first). */
#define MSL_LIST_ACCESSORS
#include "Game/CameraAdjust.h"
#include "Game/CameraManagerUnit.h"
#include "Game/CameraShake.h"
#include "Game/CameraState.h"
#include "Game/MathTypes.h"
#include "boost/shared_ptr.hpp"

using SonicteamUSA::System::Camera::CameraAdjust;
using SonicteamUSA::System::Camera::CameraFactory;
using SonicteamUSA::System::Camera::CameraMode;
using SonicteamUSA::System::Camera::CameraRequest;
using SonicteamUSA::System::Camera::CameraShake;

/* What fn_80009548 returns: the factory comes first (as in Game/CameraManagerUnit.cpp). */
class UnkCameraManager_80009548 {
  public:
    /* 0x0 */ CameraFactory *m_factory;
};

extern "C" {
UnkCameraManager_80009548 *fn_80009548(void);
CameraAdjust *fn_8000BAB4(CameraFactory *factory, int type, const UnkCameraBlend_8000B2C0 &blend);
/* Whether a value is about zero, and a placement moved by offset (Game/CameraState.cpp). */
bool fn_8000A950(float value);
void fn_8000A508(UnkCameraState_8000AA70 *state, const Vec3f *offset);
/* vector * scale, vector *= scale, floor. */
Vec3f fn_8040CDA8(const Vec3f *vector, float scale);
void fn_8040CE38(Vec3f *vector, float scale);
double fn_803B1618(double x);
}

/* The camera manager's factory. */
inline CameraFactory *GetFactory() {
    return fn_80009548()->m_factory;
}

/* The type of a request's adjust. */
inline int AdjustType(const CameraRequest *request) {
    return request->field_0xC;
}

/* An adjust of a type from the camera manager's factory. */
inline CameraAdjust *CreateAdjust(int type, const UnkCameraBlend_8000B2C0 &blend) {
    return fn_8000BAB4(fn_80009548()->m_factory, type, blend);
}

/* Copies value into vector through the game's out-of-line assignment (as in Game/GadgetUtil.h). */
inline void AssignVector(Vec3f &vector, const Vec3f &value) {
    fn_800091D0(&vector, &value);
}

namespace SonicteamUSA {
namespace System {
namespace Camera {

CameraShake::CameraShake(float duration, float amplitude, float frequency)
    : m_duration(duration), m_amplitude(amplitude), m_frequency(frequency), m_time(0.0f), m_mode(0), m_step(0) {}

CameraShake::~CameraShake() {}

} // namespace Camera
} // namespace System
} // namespace SonicteamUSA

extern "C" bool fn_8000EF58(CameraShake *shake);

/* Moves state for this frame of the shake. */
extern "C" void fn_8000EFC0(CameraShake *shake, UnkCameraState_8000AA70 *state, float dt) {
    if (fn_8000EF58(shake)) {
        return;
    }
    shake->m_time += dt;
    float half = 0.5f * (1.0f / shake->m_frequency);
    float quarter = 0.25f * (1.0f / shake->m_frequency);
    if (dt > half) {
        shake->m_mode = 2;
    } else if (dt > quarter) {
        shake->m_mode = 1;
    } else {
        shake->m_mode = 0;
    }
    const Vec3f *up = &state->m_up;
    Vec3f offset;
    fn_800091EC(&offset, &lbl_805E07A0);
    switch (shake->m_mode) {
    case 2:
        if (shake->m_step == 1) {
            shake->m_step = 3;
            AssignVector(offset, fn_8040CDA8(&(const Vec3f &)fn_8040CDA8(up, shake->m_amplitude), -1.0f));
        } else {
            shake->m_step = 1;
            AssignVector(offset, fn_8040CDA8(up, shake->m_amplitude));
        }
        break;
    case 1:
        shake->m_step++;
        if (shake->m_step >= 4) {
            shake->m_step = 0;
        }
        if (shake->m_step == 1) {
            AssignVector(offset, fn_8040CDA8(up, shake->m_amplitude));
        } else if (shake->m_step == 3) {
            AssignVector(offset, fn_8040CDA8(&(const Vec3f &)fn_8040CDA8(up, shake->m_amplitude), -1.0f));
        }
        break;
    case 0: {
        float t = shake->m_frequency * (shake->m_time - (float)fn_803B1618(shake->m_time));
        t = t - (float)fn_803B1618(t);
        AssignVector(offset, fn_8040CDA8(up, shake->m_amplitude));
        if (t < 0.25f) {
            shake->m_step = 0;
            t *= 4.0f;
        } else if (t < 0.5f) {
            t -= 0.25f;
            shake->m_step = 1;
            t *= 4.0f;
            t = 1.0f - t;
        } else if (t < 0.75f) {
            t -= 0.5f;
            shake->m_step = 2;
            t *= 4.0f;
            t = -t;
        } else {
            t -= 0.75;
            shake->m_step = 3;
            t *= 4.0f;
            t = -(1.0f - t);
        }
        fn_8040CE38(&offset, t);
        break;
    }
    }
    fn_8000A508(state, &offset);
}

/* Whether the shake is over. */
extern "C" bool fn_8000EF58(CameraShake *shake) {
    if (fn_8000A950(shake->m_frequency)) {
        return true;
    }
    if (shake->m_duration < 0.0f) {
        return false;
    }
    return shake->m_time > shake->m_duration;
}

/* The kinds of shake. */
static const CameraShake::Kind s_kinds[3] = {
    {0.6f, 7.0f},
    {1.5f, 7.0f},
    {3.0f, 7.0f},
};

/* A shake of a kind for duration. */
extern "C" boost::shared_ptr<CameraShake> fn_8000EDD0(int kind, float duration) {
    return boost::shared_ptr<CameraShake>(new CameraShake(duration, s_kinds[kind]));
}

/* Not called anywhere: the linker strips it from the game. Retail emits the shared pointer's
 * default constructor here, where the instances of a function defined here would go (inferred). */
boost::shared_ptr<CameraMode> NoMode_8000EDC0() {
    return boost::shared_ptr<CameraMode>();
}

namespace SonicteamUSA {
namespace System {
namespace Camera {

CameraRequest::CameraRequest(boost::shared_ptr<CameraMode> mode, int arg1, int arg2, int arg3, int arg4)
    : m_mode(mode), m_priority(arg1), field_0xC(arg2), m_flags(arg4) {
    m_blend = CreateAdjust(field_0xC, *reinterpret_cast<const UnkCameraBlend_8000B2C0 *>(arg3));
}

CameraRequest::~CameraRequest() {
    if (m_blend != 0) {
        delete static_cast<CameraAdjust *>(m_blend);
        m_blend = 0;
    }
}

} // namespace Camera
} // namespace System
} // namespace SonicteamUSA

/* Changes the request's adjust to one of type made from blend, unless its adjust is still
 * moving. */
extern "C" bool fn_8000EC44(CameraRequest *request, int type, const UnkCameraBlend_8000B2C0 &blend) {
    if (request->m_blend != 0) {
        if (static_cast<CameraAdjust *>(request->m_blend)->m_status == 1) {
            return false;
        }
        delete static_cast<CameraAdjust *>(request->m_blend);
        request->m_blend = 0;
    }
    request->field_0xC = type;
    request->m_blend = fn_8000BAB4(GetFactory(), AdjustType(request), blend);
    return true;
}

/* Whether request goes before other. */
extern "C" bool fn_8000EC28(CameraRequest *request, CameraRequest *other) {
    return request->m_priority >= other->m_priority;
}
