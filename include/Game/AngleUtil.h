#ifndef GAME_ANGLEUTIL_H
#define GAME_ANGLEUTIL_H

#include "Game/GadgetUtil.h"

/* Angle helpers (names inferred). Angles are in radians. TurnAngleToward calls the inline
 * WrapAngle, so it is never inlined itself: units emit weak copies, and the one linked is
 * UpDownReel's (0x8012739C unit, at 0x8012773C), which about ten other units call. */

/* Wraps angle into [-pi, pi]. */
inline float WrapAngle(float angle) {
    if (angle < -lbl_805FAD38) {
        angle += 2.0f * lbl_805FAD38;
    } else if (angle > lbl_805FAD38) {
        angle -= 2.0f * lbl_805FAD38;
    }
    return angle;
}

/* Turns current toward target by at most step; returns target once within step of it. */
inline float TurnAngleToward(float target, float current, float step) {
    float diff = WrapAngle(target - current);

    if (diff <= step && diff >= -step) {
        return target;
    }

    float result;
    if (diff < 0.0f) {
        result = current - step;
    } else {
        result = current + step;
    }
    return WrapAngle(result);
}

#endif
