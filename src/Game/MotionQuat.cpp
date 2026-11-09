/* The motion library's quaternion helpers (no class with RTTI; file name and all names inferred):
 * a quaternion to a rotation matrix, a rotation matrix to a quaternion, and spherical linear
 * interpolation. The motion library's pose blending (fn_8042062C, in the unit before this one)
 * calls all three. Definitions are in reverse address order (deferred inlining emits functions
 * last-defined first). */
#include "Game/MathTypes.h"
#include "Game/Quaternion.h"

namespace std {
float acos(float x);
float sin(float x);
}

extern "C" {
float fn_8000FEE4(float value); /* square root */

void fn_80420B60(Mtx34f *m, const Quatf *q);
void fn_804208EC(Quatf *q, const Mtx34f *m);
int fn_80420724(Quatf *out, const Quatf *to, const Quatf *from, float t);
}

/* m becomes the rotation q describes, with no translation. */
void fn_80420B60(Mtx34f *m, const Quatf *q) {
    float x = q->x;
    float y = q->y;
    float z = q->z;
    float w = q->w;
    float ww = 2.0f * w * w - 1.0f;

    m->values[0][0] = 2.0f * x * x + ww;
    m->values[1][0] = 2.0f * (x * y + w * z);
    m->values[2][0] = 2.0f * (x * z - w * y);
    m->values[0][1] = 2.0f * (x * y - w * z);
    m->values[1][1] = 2.0f * y * y + ww;
    m->values[2][1] = 2.0f * (y * z + w * x);
    m->values[0][2] = 2.0f * (x * z + w * y);
    m->values[1][2] = 2.0f * (y * z - w * x);
    m->values[2][2] = 2.0f * z * z + ww;
    m->values[0][3] = m->values[1][3] = m->values[2][3] = 0.0f;
}

/* q becomes the rotation of m's 3x3 part. */
void fn_804208EC(Quatf *q, const Mtx34f *m) {
    float trace = m->values[0][0] + m->values[1][1] + m->values[2][2] + 1.0f;

    if (trace < 1.0f) {
        if (m->values[0][0] > m->values[1][1]) {
            if (m->values[0][0] > m->values[2][2]) {
                float x = 0.5f * fn_8000FEE4(1.0f + (m->values[0][0] - m->values[1][1] - m->values[2][2]));
                float inv = 1.0f / (4.0f * x);
                q->x = x;
                q->y = inv * (m->values[1][0] + m->values[0][1]);
                q->z = inv * (m->values[2][0] + m->values[0][2]);
                q->w = inv * (m->values[2][1] - m->values[1][2]);
            } else {
                float z = 0.5f * fn_8000FEE4(1.0f + (m->values[2][2] - m->values[0][0] - m->values[1][1]));
                float inv = 1.0f / (4.0f * z);
                q->z = z;
                q->x = inv * (m->values[0][2] + m->values[2][0]);
                q->y = inv * (m->values[1][2] + m->values[2][1]);
                q->w = inv * (m->values[1][0] - m->values[0][1]);
            }
        } else {
            if (m->values[1][1] > m->values[2][2]) {
                float y = 0.5f * fn_8000FEE4(1.0f + (m->values[1][1] - m->values[2][2] - m->values[0][0]));
                float inv = 1.0f / (4.0f * y);
                q->y = y;
                q->z = inv * (m->values[2][1] + m->values[1][2]);
                q->x = inv * (m->values[0][1] + m->values[1][0]);
                q->w = inv * (m->values[0][2] - m->values[2][0]);
            } else {
                float z = 0.5f * fn_8000FEE4(1.0f + (m->values[2][2] - m->values[0][0] - m->values[1][1]));
                float inv = 1.0f / (4.0f * z);
                q->z = z;
                q->x = inv * (m->values[0][2] + m->values[2][0]);
                q->y = inv * (m->values[1][2] + m->values[2][1]);
                q->w = inv * (m->values[1][0] - m->values[0][1]);
            }
        }
    } else {
        float s = fn_8000FEE4(trace);
        float inv = 1.0f / (2.0f * s);
        q->x = inv * (m->values[2][1] - m->values[1][2]);
        q->y = inv * (m->values[0][2] - m->values[2][0]);
        q->z = inv * (m->values[1][0] - m->values[0][1]);
        q->w = 0.5f * s;
    }
}

/* out becomes the rotation t of the way from from to to (the shorter way round); always 1. */
int fn_80420724(Quatf *out, const Quatf *to, const Quatf *from, float t) {
    float tx = to->x;
    float ty = to->y;
    float tz = to->z;
    float tw = to->w;
    float fx = from->x;
    float fy = from->y;
    float fz = from->z;
    float fw = from->w;
    float cosom = tx * fx + ty * fy + tz * fz + tw * fw;
    float scaleFrom;
    float scaleTo = 1.0f;

    if (cosom < 0.0f) {
        cosom = -cosom;
        scaleTo = -scaleTo;
    }
    if (cosom < 0.9999f) {
        float omega = std::acos(cosom);
        float invSin = 1.0f / std::sin(omega);
        scaleFrom = invSin * std::sin((1.0f - t) * omega);
        scaleTo *= invSin * std::sin(t * omega);
    } else {
        scaleTo *= t;
        scaleFrom = 1.0f - t;
    }
    out->x = fx * scaleFrom + tx * scaleTo;
    out->y = fy * scaleFrom + ty * scaleTo;
    out->z = fz * scaleFrom + tz * scaleTo;
    out->w = fw * scaleFrom + tw * scaleTo;
    return 1;
}

/* Not called anywhere: the linker strips it from the game. The original pools the float literals
 * of fn_80420B60 (0, 2, 1) first, then those of fn_80420724 and fn_804208EC; GC/2.7 pools them in
 * the order it emits the functions, so this function, emitted first, uses them in that order (see
 * docs/research-notes.md). */
extern "C" void MotionQuatLiteralOrder(float *out) {
    out[0] = 0.0f;
    out[1] = 2.0f;
    out[2] = 1.0f;
    out[3] = 0.9999f;
    out[4] = 0.5f;
    out[5] = 4.0f;
}
