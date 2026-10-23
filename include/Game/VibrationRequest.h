#ifndef GAME_VIBRATIONREQUEST_H
#define GAME_VIBRATIONREQUEST_H

/* Include <vector> with the unit's MSL_VECTOR_* settings before this header. */
#include <vector>

#include "Game/MathTypes.h"
#include "Game/Vibration.h"
#include "Game/VibrationPattern.h"
#include "boost/shared_ptr.hpp"
#include "boost/weak_ptr.hpp"
#include <dolphin/types.h>
#include "size_assert.h"

namespace Vibration {

class Request;

/* RequestHandle (class name from the RTTI of its shared_ptr control block): what the rumble
 * functions return; it keeps the requests made for each pad, so that one call can stop them all.
 * Game/VibrationRequest.cpp; member names inferred. */
class RequestHandle {
  public:
    RequestHandle();
    ~RequestHandle();
    void Add(const boost::weak_ptr<Request> &request);

    /* 0x0 */ std::vector<boost::weak_ptr<Request> > m_requests;
};
SIZE_ASSERT(RequestHandle, 0xC);

/* Vibration::Request (class name from RTTI): a pattern playing on one pad. Mode 0 plays the
 * pattern once, mode 1 repeats it, and mode 2 repeats it for m_duration seconds. Member names are
 * inferred. */
class Request {
  public:
    Request(float duration, boost::shared_ptr<RequestHandle> handle, int pad, int hasPosition, int kind,
            const Pattern *pattern, int mode, int arg7);
    virtual ~Request();
    void Update(float dt);
    /* Ends the request (name inferred). */
    void End() {
        m_active = 0;
    }

    /* 0x04 */ boost::shared_ptr<RequestHandle> m_handle;
    /* 0x0C */ s32 m_pad;
    /* 0x10 */ s32 m_hasPosition;
    /* 0x14 */ s32 m_kind;
    /* 0x18 */ Pattern m_pattern;
    /* 0x2C */ s32 m_mode;
    /* 0x30 */ float m_duration;
    /* 0x34 */ s32 field_0x34;
    /* 0x38 */ u8 m_active;
    /* 0x3C */ float m_strength;
    /* 0x40 */ float m_strength2;
    /* 0x44 */ float m_time;      /* left of the pattern */
    /* 0x48 */ float m_timeLeft;  /* left of the duration (mode 2) */
};
SIZE_ASSERT(Request, 0x4C);

/* Vibration::RequestWithPos (class name from RTTI): a request made at a position. */
class RequestWithPos : public Request {
  public:
    RequestWithPos(float duration, boost::shared_ptr<RequestHandle> handle, int pad, int kind,
                   const Pattern *pattern, const Vec3f *position, int mode, int arg7);
    virtual ~RequestWithPos();

    /* 0x4C */ Vec3f m_position;
};
SIZE_ASSERT(RequestWithPos, 0x58);

} // namespace Vibration

#endif
