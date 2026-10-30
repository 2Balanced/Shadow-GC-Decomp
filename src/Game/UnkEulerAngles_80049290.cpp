/* Angle and matrix helpers that many units call by their fn_ names (file and function meanings
 * inferred): stepping an angle towards another, the Euler angles of a rotation matrix in three
 * axis orders, whether a vector's components are all zero and a matrix's translation column. The
 * unit is split by its .sdata2 (2.0 and 0.0; the next unit pools its own 0.0). Definitions are in
 * reverse address order (deferred inlining emits functions last-defined first). */
#include "Game/MathConstants.h"
#include "Game/MathTypes.h"
#include <cmath>
#include <dolphin/types.h>

/* The sine and cosine of an angle (out of line at 0x80031568). */
void SinCos(float *s, float *c, float angle);

extern "C" {
/* atan2 (see Game/AudioChannel.cpp). */
float fn_80016260(float y, float x);
/* Whether a value is (nearly) zero (see Game/CameraModeGeneral.cpp). */
bool fn_8000B044(float value);
/* A matrix element by column and row (Game/MathTypes.h's matrix routines). */
float fn_8040C168(const Mtx34f *m, int column, int row);
}

/* The translation column, read back to front. */
extern "C" Vec3f fn_800496F8(const Mtx34f *m) {
    Vec3f v;
    v.z = fn_8040C168(m, 2, 3);
    v.y = fn_8040C168(m, 1, 3);
    v.x = fn_8040C168(m, 0, 3);
    return v;
}

/* The translation column (callers take it as a return value). */
extern "C" void fn_80049690(Vec3f *out, const Mtx34f *m) {
    out->x = fn_8040C168(m, 0, 3);
    out->y = fn_8040C168(m, 1, 3);
    out->z = fn_8040C168(m, 2, 3);
}

/* Whether all three components are zero. */
extern "C" bool fn_8004962C(const Vec3f *v) {
    if (fn_8000B044(v->x) && fn_8000B044(v->y) && fn_8000B044(v->z)) {
        return true;
    }
    return false;
}

/* The Euler angles of a rotation matrix, y first. */
extern "C" void fn_800494F8(Vec3f *angles, const Mtx34f *m) {
    float s;
    float c;
    float angle;
    float first = fn_80016260(m->values[0][2], m->values[2][2]);
    angles->y = first;
    SinCos(&s, &c, first);
    if (std::fabs(m->values[2][2]) > std::fabs(m->values[0][2])) {
        angle = fn_80016260(-m->values[1][2] * c, m->values[2][2]);
        if (c < 0.0f) {
            angle += angle < 0.0f ? lbl_805FAD38 : -lbl_805FAD38;
        }
    } else {
        angle = fn_80016260(-m->values[1][2] * s, m->values[0][2]);
        if (s < 0.0f) {
            angle += angle < 0.0f ? lbl_805FAD38 : -lbl_805FAD38;
        }
    }
    angles->x = angle;
    angles->z = fn_80016260(m->values[2][1] * s - m->values[0][1] * c,
                            m->values[0][0] * c - m->values[2][0] * s);
}

/* The Euler angles of a rotation matrix, z first and x next. */
extern "C" void fn_800493C4(Vec3f *angles, const Mtx34f *m) {
    float s;
    float c;
    float angle;
    float first = fn_80016260(-m->values[0][1], m->values[1][1]);
    angles->z = first;
    SinCos(&s, &c, first);
    if (std::fabs(m->values[1][1]) > std::fabs(m->values[0][1])) {
        angle = fn_80016260(m->values[2][1] * c, m->values[1][1]);
        if (c < 0.0f) {
            angle += angle < 0.0f ? lbl_805FAD38 : -lbl_805FAD38;
        }
    } else {
        angle = fn_80016260(m->values[2][1] * s, -m->values[0][1]);
        if (s < 0.0f) {
            angle += angle < 0.0f ? lbl_805FAD38 : -lbl_805FAD38;
        }
    }
    angles->x = angle;
    angles->y = fn_80016260(m->values[0][2] * c + m->values[1][2] * s,
                            m->values[0][0] * c + m->values[1][0] * s);
}

/* The Euler angles of a rotation matrix, z first and y next. */
extern "C" void fn_80049290(Vec3f *angles, const Mtx34f *m) {
    float s;
    float c;
    float angle;
    float first = fn_80016260(m->values[1][0], m->values[0][0]);
    angles->z = first;
    SinCos(&s, &c, first);
    if (std::fabs(m->values[0][0]) > std::fabs(m->values[1][0])) {
        angle = fn_80016260(-m->values[2][0] * c, m->values[0][0]);
        if (c < 0.0f) {
            angle += angle < 0.0f ? lbl_805FAD38 : -lbl_805FAD38;
        }
    } else {
        angle = fn_80016260(-m->values[2][0] * s, m->values[1][0]);
        if (s < 0.0f) {
            angle += angle < 0.0f ? lbl_805FAD38 : -lbl_805FAD38;
        }
    }
    angles->y = angle;
    angles->x = fn_80016260(m->values[0][2] * s - m->values[1][2] * c,
                            m->values[1][1] * c - m->values[0][1] * s);
}

/* Steps angle towards target (taking the shorter way round) by rate / (rate + damping) of the
 * difference. */
extern "C" float fn_8004924C(float angle, float target, float rate, float damping) {
    float diff = target - angle;
    if (diff < -lbl_805FAD38) {
        diff += 2.0f * lbl_805FAD38;
    } else if (diff > lbl_805FAD38) {
        diff -= 2.0f * lbl_805FAD38;
    }
    return angle + diff * rate / (rate + damping);
}
