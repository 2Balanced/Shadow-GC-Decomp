/* Vibration::Request, Vibration::RequestWithPos and Vibration::RequestHandle (class names from
 * RTTI; declared in Game/VibrationRequest.h), and the rumble functions the game calls (under their
 * fn_ names): each starts a pattern of Game/VibrationPattern.cpp on one player's pad or on every
 * pad through the vibration manager (class name unknown; the function-local static behind
 * fn_80177F38) and returns the handle that stops it. File name from the classes; all other names
 * are inferred. Definitions are in reverse address order (deferred inlining emits functions
 * last-defined first). */
#define MSL_VECTOR_MIDDLE
#define MSL_VECTOR_DESTROY_ALL_LATE
#define MSL_VECTOR_DESTROY_ALL_FIRST
#include <vector>

#include "Game/MathTypes.h"
#include "Game/VibrationManagerUnit.h"
#include "Game/VibrationPattern.h"
#include "Game/VibrationRequest.h"
#include "boost/shared_ptr.hpp"
#include "boost/weak_ptr.hpp"
#include <dolphin/types.h>

/* The vibration manager (class name unknown; see Game/StageManager.cpp). */
class UnkVibrationManager_80177F38;


namespace boost {
/* boost/weak_ptr.hpp only declares weak_ptr::reset, which the game has out of line; this unit
 * emits the copy for the handles (boost 1.32's definition). */
template <> inline void weak_ptr<Vibration::RequestHandle>::reset() {
    this_type().swap(*this);
}
} // namespace boost

using namespace Vibration;

extern "C" {
UnkVibrationManager_80177F38 *fn_80177F38(void);
/* The manager's requests: on the pad of player (or every pad for -2), with or without a position;
 * and stopping the requests of a handle (behaviour inferred). */
boost::weak_ptr<RequestHandle> fn_8034AF24(UnkVibrationManager_80177F38 *manager, int kind,
                                           const Pattern *pattern, const Vec3f *position, int mode,
                                           int arg5, int player, float duration);
boost::weak_ptr<RequestHandle> fn_8034B0A8(UnkVibrationManager_80177F38 *manager, int kind,
                                           const Pattern *pattern, int mode, int arg4, int player,
                                           float duration);
void fn_8034A418(UnkVibrationManager_80177F38 *manager, boost::shared_ptr<RequestHandle> &handle);
/* Whether two floats are about equal (behaviour inferred; see Game/CameraScreenSet.cpp). */
bool fn_801C455C(float a, float b);
}

RequestHandle::RequestHandle() {
    m_requests.clear();
    m_requests.reserve(kPadCount);
}

RequestHandle::~RequestHandle() {
    m_requests.clear();
}

void RequestHandle::Add(const boost::weak_ptr<Request> &request) {
    m_requests.push_back(request);
}

Request::Request(float duration, boost::shared_ptr<RequestHandle> handle, int pad, int hasPosition, int kind,
                 const Pattern *pattern, int mode, int arg7)
    : m_handle(handle), m_pad(pad), m_hasPosition(hasPosition), m_kind(kind), m_pattern(*pattern), m_mode(mode),
      m_duration(duration), field_0x34(arg7), m_active(1), m_strength(pattern->m_strength),
      m_strength2(pattern->m_strength2), m_time(m_pattern.m_time), m_timeLeft(duration) {}

Request::~Request() {}

void Request::Update(float dt) {
    if (m_active) {
        if (!fn_801C455C(m_time, m_pattern.m_time)) {
            m_strength -= m_pattern.m_fade;
            m_strength2 -= m_pattern.m_fade2;
        }
        m_time -= dt;
        switch (m_mode) {
        case 0:
            if (m_time < 0.0f) {
                m_active = 0;
            }
            break;
        case 1:
            if (m_time < 0.0f) {
                m_strength = m_pattern.m_strength;
                m_strength2 = m_pattern.m_strength2;
                m_time = m_pattern.m_time;
            }
            break;
        case 2:
            if (m_time < 0.0f) {
                m_strength = m_pattern.m_strength;
                m_strength2 = m_pattern.m_strength2;
                m_time = m_pattern.m_time;
            }
            m_timeLeft -= dt;
            if (m_timeLeft < 0.0f) {
                m_active = 0;
            }
            break;
        }
    }
}

RequestWithPos::RequestWithPos(float duration, boost::shared_ptr<RequestHandle> handle, int pad, int kind,
                               const Pattern *pattern, const Vec3f *position, int mode, int arg7)
    : Request(duration, handle, pad, 1, kind, pattern, mode, arg7) {
    fn_800091EC(&m_position, position);
}

RequestWithPos::~RequestWithPos() {}

/* Plays pattern kind once on the pad of player (-2: every pad). */
extern "C" boost::weak_ptr<RequestHandle> fn_8034DF48(int kind, int player) {
    const Pattern *pattern = GetPattern(kind);
    return fn_8034B0A8(fn_80177F38(), kind, pattern, 0, 0, player, 0.0f);
}

/* Repeats pattern kind on the pad of player until stopped. */
extern "C" boost::weak_ptr<RequestHandle> fn_8034DEE4(int kind, int player) {
    const Pattern *pattern = GetPattern(kind);
    return fn_8034B0A8(fn_80177F38(), kind, pattern, 1, 0, player, 0.0f);
}

/* Repeats pattern kind on the pad of player for duration seconds. */
extern "C" boost::weak_ptr<RequestHandle> fn_8034DE74(int kind, int player, float duration) {
    const Pattern *pattern = GetPattern(kind);
    return fn_8034B0A8(fn_80177F38(), kind, pattern, 2, 0, player, duration);
}

/* The same at a position. */
extern "C" boost::weak_ptr<RequestHandle> fn_8034DE08(int kind, const Vec3f *position, int player) {
    const Pattern *pattern = GetPattern(kind);
    return fn_8034AF24(fn_80177F38(), kind, pattern, position, 0, 0, player, 0.0f);
}

extern "C" boost::weak_ptr<RequestHandle> fn_8034DD9C(int kind, const Vec3f *position, int player) {
    const Pattern *pattern = GetPattern(kind);
    return fn_8034AF24(fn_80177F38(), kind, pattern, position, 1, 0, player, 0.0f);
}

extern "C" boost::weak_ptr<RequestHandle> fn_8034DD24(int kind, float duration, const Vec3f *position,
                                                      int player) {
    const Pattern *pattern = GetPattern(kind);
    return fn_8034AF24(fn_80177F38(), kind, pattern, position, 2, 0, player, duration);
}

/* Stops the requests of a handle and forgets it. */
extern "C" void fn_8034DB60(boost::weak_ptr<RequestHandle> *handle) {
    if (!handle->expired()) {
        boost::shared_ptr<RequestHandle> locked = handle->lock();
        fn_8034A418(fn_80177F38(), locked);
    }
    handle->reset();
}

#include <vector_impl>
