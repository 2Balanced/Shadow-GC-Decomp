/* Free geometry helpers (no class: no RTTI or vtable reaches this code): polynomial root solvers
 * (quadratic, cubic, quartic), the two angles of a direction, a segment against a plane, and the
 * closest points between a point, lines and segments. The Range shapes (Game/RangeCapsule.cpp,
 * Game/RangeCylinder.cpp, Game/RangeSphere.cpp), the collision queries and the cameras call them
 * through their fn_ names. Names and meanings are inferred. Definitions are in reverse address
 * order (deferred inlining emits functions last-defined first). */
#include "Game/Fn8040CopyVector3.h"
#include "Game/GeometryLine.h"
#include "Game/MathTypes.h"
#include <dolphin/types.h>

using Sonicteam::System::Geometry::Line;

namespace std {
float fabs(float x);
float sin(float x);
float cos(float x);
} // namespace std

typedef Vec3f (*VectorSubtractFn)(const Vec3f *a, const Vec3f *b);

/* A plane as the plane helpers read it (layout not needed here). */
struct UnkPlane_8003BADC;

extern "C" {
float fn_8000FEE4(float x);  /* square root */
float fn_80015D68(float x);  /* arc cosine */
float fn_80016260(float y, float x); /* the angle of (x, y) */
float fn_8040CF90(const Vec3f *a, const Vec3f *b); /* a . b */
float fn_8040D180(const Vec3f *a, const Vec3f *b); /* |a - b|^2 */
Vec3f fn_8040CDA8(const Vec3f *v, float scale);
Vec3f fn_8040CDD0(const Vec3f *a, const Vec3f *b); /* a + b */
Vec3f fn_8040CFB8(const Vec3f *a, const Vec3f *b); /* a x b */
float fn_8040CE74(Vec3f *v); /* normalises v */
float fn_8040C340(const UnkPlane_8003BADC *plane, const Line *line, Vec3f *point);
float fn_8040C3AC(const UnkPlane_8003BADC *plane, const Vec3f *point);
extern float lbl_805FAD38; /* pi */

bool fn_8003B03C(float *root0, float *root1, float *root2, float *root3, float a, float b, float c, float d);
bool fn_8003B304(float *root, float a, float b, float c);
bool fn_8003B6C4(float *root0, float *root1, float b, float c);
bool fn_8003B7AC(float *root0, float *root1, float a, float b, float c);
void fn_8003B8F4(const Vec3f *direction, float *angleA, float *angleB);
void fn_8003B9E8(const Vec3f *direction, float *angleA, float *angleB);
bool fn_8003BADC(const Line *line, const UnkPlane_8003BADC *plane, Vec3f *hit);
float fn_8003BC14(const Line *a, const Line *b, const Vec3f *directionA, const Vec3f *directionB, Vec3f *closestA,
                  Vec3f *closestB);
float fn_8003C4C4(const Line *a, const Line *b, Vec3f *closestA, Vec3f *closestB);
float fn_8003C55C(const Vec3f *point, const Line *line, Vec3f *closest);
float fn_8003C63C(const Vec3f *point, const Line *line, Vec3f *closest);
float fn_8003C660(const Vec3f *point, const Line *line, Vec3f *closest);
}

inline void AssignVector(Vec3f *vector, const Vec3f &value) {
    fn_800091D0(vector, &value);
}

/* Cube root (the fdlibm cbrtf, inlined: each word access is its own union). */
typedef union {
    float value;
    unsigned int word;
} ieee_float_shape_type;

#define GET_FLOAT_WORD(i, d)                                                                       \
    do {                                                                                           \
        ieee_float_shape_type gf_u;                                                                \
        gf_u.value = (d);                                                                          \
        (i) = gf_u.word;                                                                           \
    } while (0)

#define SET_FLOAT_WORD(d, i)                                                                       \
    do {                                                                                           \
        ieee_float_shape_type sf_u;                                                                \
        sf_u.word = (i);                                                                           \
        (d) = sf_u.value;                                                                          \
    } while (0)

inline float CubeRoot(float x) {
    float r;
    float s;
    float t;
    int hx;
    unsigned int sign;
    unsigned int high;

    GET_FLOAT_WORD(hx, x);
    sign = hx & 0x80000000;
    hx ^= sign;
    if (hx >= 0x7F800000) {
        return x + x;
    }
    if (hx == 0) {
        return x;
    }
    SET_FLOAT_WORD(x, hx);
    if (hx < 0x00800000) {
        SET_FLOAT_WORD(t, 0x4B800000);
        t *= x;
        GET_FLOAT_WORD(high, t);
        SET_FLOAT_WORD(t, high / 3 + 642849266);
    } else {
        SET_FLOAT_WORD(t, hx / 3 + 709958130);
    }
    r = t * t / x;
    s = 0.54285717f + r * t;
    t *= 0.35714287f + 1.6071428f / (s + 1.4142857f + -0.7053061f / s);
    GET_FLOAT_WORD(high, t);
    SET_FLOAT_WORD(t, high | sign);
    return t;
}

float fn_8003C660(const Vec3f *point, const Line *line, Vec3f *closest) {
    Fn8040CopyVector3 direction(((VectorSubtractFn)fn_8040CD74)(&line->m_end, &line->m_start));
    Fn8040CopyVector3 offset(((VectorSubtractFn)fn_8040CD74)(point, &line->m_start));
    float along = fn_8040CF90((Vec3f *)&offset, (Vec3f *)&direction);
    if (along > 0.0f) {
        float length = fn_8040CF90((Vec3f *)&direction, (Vec3f *)&direction);
        if (along >= length) {
            if (closest) {
                AssignVector(closest, line->m_end);
            }
            return fn_8040D180(point, &line->m_end);
        }
        Fn8040CopyVector3 nearest(fn_8040CDD0(&line->m_start, &fn_8040CDA8((Vec3f *)&direction, along / length)));
        if (closest) {
            AssignVector(closest, *(Vec3f *)&nearest);
        }
        return fn_8040D180(point, (Vec3f *)&nearest);
    }
    if (closest) {
        AssignVector(closest, line->m_start);
    }
    return fn_8040CF2C((Vec3f *)&offset);
}

float fn_8003C63C(const Vec3f *point, const Line *line, Vec3f *closest) {
    return fn_8000FEE4(fn_8003C660(point, line, closest));
}

float fn_8003C55C(const Vec3f *point, const Line *line, Vec3f *closest) {
    Fn8040CopyVector3 direction(((VectorSubtractFn)fn_8040CD74)(&line->m_end, &line->m_start));
    Fn8040CopyVector3 offset(((VectorSubtractFn)fn_8040CD74)(point, &line->m_start));
    float along = fn_8040CF90((Vec3f *)&offset, (Vec3f *)&direction);
    Fn8040CopyVector3 nearest(
        fn_8040CDD0(&line->m_start, &fn_8040CDA8((Vec3f *)&direction,
                                                 along / fn_8040CF90((Vec3f *)&direction, (Vec3f *)&direction))));
    if (closest) {
        AssignVector(closest, *(Vec3f *)&nearest);
    }
    return fn_8040D180(point, (Vec3f *)&nearest);
}

float fn_8003C4C4(const Line *a, const Line *b, Vec3f *closestA, Vec3f *closestB) {
    Fn8040CopyVector3 directionA(((VectorSubtractFn)fn_8040CD74)(&a->m_end, &a->m_start));
    Fn8040CopyVector3 directionB(((VectorSubtractFn)fn_8040CD74)(&b->m_end, &b->m_start));
    fn_8040CE74((Vec3f *)&directionA);
    fn_8040CE74((Vec3f *)&directionB);
    return fn_8003BC14(a, b, (Vec3f *)&directionA, (Vec3f *)&directionB, closestA, closestB);
}

float fn_8003BC14(const Line *a, const Line *b, const Vec3f *directionA, const Vec3f *directionB, Vec3f *closestA,
                  Vec3f *closestB) {
    Fn8040CopyVector3 axisA(((VectorSubtractFn)fn_8040CD74)(&a->m_end, &a->m_start));
    Fn8040CopyVector3 axisB(((VectorSubtractFn)fn_8040CD74)(&b->m_end, &b->m_start));
    Fn8040CopyVector3 normal(fn_8040CFB8(directionA, directionB));
    if (std::fabs(normal.x) < 0.0001f && std::fabs(normal.y) < 0.0001f && std::fabs(normal.z) < 0.0001f) {
        float start = fn_8040CF90(directionA, &((VectorSubtractFn)fn_8040CD74)(&b->m_start, &a->m_start));
        Fn8040CopyVector3 along(fn_8040CDA8(directionA, start));
        Fn8040CopyVector3 gap(((VectorSubtractFn)fn_8040CD74)(&((VectorSubtractFn)fn_8040CD74)(&b->m_start, &a->m_start),
                                                             (Vec3f *)&along));
        float distance = fn_8040CF2C((Vec3f *)&gap);
        float length = fn_8040CF90((Vec3f *)&axisA, directionA);
        if (0.0f <= start && start <= length) {
            if (closestB) {
                AssignVector(closestB, b->m_start);
            }
            if (closestA) {
                AssignVector(closestA, ((VectorSubtractFn)fn_8040CD74)(&b->m_start, (Vec3f *)&gap));
            }
            return distance;
        }
        float end = fn_8040CF90(directionA, &((VectorSubtractFn)fn_8040CD74)(&b->m_end, &a->m_start));
        if (0.0f <= end && end <= length) {
            if (closestB) {
                AssignVector(closestB, b->m_end);
            }
            if (closestA) {
                AssignVector(closestA, ((VectorSubtractFn)fn_8040CD74)(&b->m_end, (Vec3f *)&gap));
            }
            return distance;
        }
        if (start < end) {
            if (start <= 0.0f && 0.0f <= end) {
                if (closestA) {
                    AssignVector(closestA, a->m_start);
                }
                if (closestB) {
                    AssignVector(closestB, fn_8040CDD0(&a->m_start, (Vec3f *)&gap));
                }
                return distance;
            }
            if (start <= length && length <= end) {
                if (closestA) {
                    AssignVector(closestA, a->m_end);
                }
                if (closestB) {
                    AssignVector(closestB, fn_8040CDD0(&a->m_end, (Vec3f *)&gap));
                }
                return distance;
            }
            if (end < 0.0f) {
                if (closestA) {
                    AssignVector(closestA, a->m_start);
                }
                if (closestB) {
                    AssignVector(closestB, b->m_end);
                }
                return fn_8040D180(&a->m_start, &b->m_end);
            }
            if (closestA) {
                AssignVector(closestA, a->m_end);
            }
            if (closestB) {
                AssignVector(closestB, b->m_start);
            }
            return fn_8040D180(&a->m_end, &b->m_start);
        } else {
            if (end <= 0.0f && 0.0f <= start) {
                if (closestA) {
                    AssignVector(closestA, a->m_start);
                }
                if (closestB) {
                    AssignVector(closestB, fn_8040CDD0(&a->m_start, (Vec3f *)&gap));
                }
                return distance;
            }
            if (end <= length && length <= start) {
                if (closestA) {
                    AssignVector(closestA, a->m_end);
                }
                if (closestB) {
                    AssignVector(closestB, fn_8040CDD0(&a->m_end, (Vec3f *)&gap));
                }
                return distance;
            }
            if (start < 0.0f) {
                if (closestA) {
                    AssignVector(closestA, a->m_start);
                }
                if (closestB) {
                    AssignVector(closestB, b->m_start);
                }
                return fn_8040D180(&a->m_start, &b->m_start);
            }
            if (closestA) {
                AssignVector(closestA, a->m_end);
            }
            if (closestB) {
                AssignVector(closestB, b->m_end);
            }
            return fn_8040D180(&a->m_end, &b->m_end);
        }
    }

    fn_8040CE74((Vec3f *)&normal);
    float separation = fn_8040CF90(&((VectorSubtractFn)fn_8040CD74)(&b->m_start, &a->m_start), (Vec3f *)&normal);
    Fn8040CopyVector3 shift(fn_8040CDA8((Vec3f *)&normal, separation));
    AssignVector((Vec3f *)&normal, fn_8040CFB8((Vec3f *)&shift, directionA));
    float t = fn_8040CF90((Vec3f *)&normal, &((VectorSubtractFn)fn_8040CD74)(&a->m_start, &b->m_start));
    t /= fn_8040CF90((Vec3f *)&normal, (Vec3f *)&axisB);
    Fn8040CopyVector3 pointB(fn_8040CDD0(&fn_8040CDA8((Vec3f *)&axisB, t), &b->m_start));
    Fn8040CopyVector3 pointA(((VectorSubtractFn)fn_8040CD74)((Vec3f *)&pointB, (Vec3f *)&shift));
    Fn8040CopyVector3 offset(((VectorSubtractFn)fn_8040CD74)((Vec3f *)&pointA, &a->m_start));
    float s = fn_8040CF90((Vec3f *)&axisA, (Vec3f *)&offset);
    s /= fn_8040CF90((Vec3f *)&axisA, (Vec3f *)&axisA);
    if (s < 0.0f) {
        if (t < 0.0f) {
            float distanceB = fn_8040D180((Vec3f *)&pointB, &b->m_start);
            if (fn_8040D180((Vec3f *)&pointA, &a->m_start) < distanceB) {
                if (closestA) {
                    AssignVector(closestA, a->m_start);
                }
                return fn_8003C660(&a->m_start, b, closestB);
            }
            if (closestB) {
                AssignVector(closestB, b->m_start);
            }
            return fn_8003C660(&b->m_start, a, closestA);
        } else if (t > 1.0f) {
            float distanceB = fn_8040D180((Vec3f *)&pointB, &b->m_end);
            if (fn_8040D180((Vec3f *)&pointA, &a->m_start) < distanceB) {
                if (closestA) {
                    AssignVector(closestA, a->m_start);
                }
                return fn_8003C660(&a->m_start, b, closestB);
            }
            if (closestB) {
                AssignVector(closestB, b->m_end);
            }
            return fn_8003C660(&b->m_end, a, closestA);
        } else {
            if (closestA) {
                AssignVector(closestA, a->m_start);
            }
            return fn_8003C660(&a->m_start, b, closestB);
        }
    } else if (s > 1.0f) {
        if (t < 0.0f) {
            float distanceB = fn_8040D180((Vec3f *)&pointB, &b->m_start);
            if (fn_8040D180((Vec3f *)&pointA, &a->m_end) < distanceB) {
                if (closestA) {
                    AssignVector(closestA, a->m_end);
                }
                return fn_8003C660(&a->m_end, b, closestB);
            }
            if (closestB) {
                AssignVector(closestB, b->m_start);
            }
            return fn_8003C660(&b->m_start, a, closestA);
        } else if (t > 1.0f) {
            float distanceB = fn_8040D180((Vec3f *)&pointB, &b->m_end);
            if (fn_8040D180((Vec3f *)&pointA, &a->m_end) < distanceB) {
                if (closestA) {
                    AssignVector(closestA, a->m_end);
                }
                return fn_8003C660(&a->m_end, b, closestB);
            }
            if (closestB) {
                AssignVector(closestB, b->m_end);
            }
            return fn_8003C660(&b->m_end, a, closestA);
        } else {
            if (closestA) {
                AssignVector(closestA, a->m_end);
            }
            return fn_8003C660(&a->m_end, b, closestB);
        }
    } else {
        if (t < 0.0f) {
            if (closestB) {
                AssignVector(closestB, b->m_start);
            }
            return fn_8003C660(&b->m_start, a, closestA);
        } else if (t > 1.0f) {
            if (closestB) {
                AssignVector(closestB, b->m_end);
            }
            return fn_8003C660(&b->m_end, a, closestA);
        } else {
            if (closestA) {
                AssignVector(closestA, *(Vec3f *)&pointA);
            }
            if (closestB) {
                AssignVector(closestB, *(Vec3f *)&pointB);
            }
            return separation * separation;
        }
    }
}

bool fn_8003BADC(const Line *line, const UnkPlane_8003BADC *plane, Vec3f *hit) {
    Vec3f point;
    float distance = fn_8040C340(plane, line, &point);
    if (distance < -0.0001f) {
        return false;
    }
    if (fn_8040CF90(&((VectorSubtractFn)fn_8040CD74)(&line->m_start, &point),
                    &((VectorSubtractFn)fn_8040CD74)(&line->m_end, &point)) < 0.0f) {
        if (hit) {
            float total = distance + std::fabs(fn_8040C3AC(plane, &line->m_end));
            if (std::fabs(total) > 0.0001f) {
                AssignVector(hit, fn_8040CDD0(&line->m_start,
                                              &fn_8040CDA8(&((VectorSubtractFn)fn_8040CD74)(&line->m_end,
                                                                                            &line->m_start),
                                                           distance / total)));
            } else {
                AssignVector(hit, line->m_start);
            }
        }
        return true;
    }
    return false;
}

void fn_8003B9E8(const Vec3f *direction, float *angleA, float *angleB) {
    float angle = fn_80016260(direction->x, direction->z);
    *angleB = angle;
    float result;
    float s;
    if (std::fabs(direction->x) > std::fabs(direction->z)) {
        s = std::sin(angle);
        result = fn_80016260(-direction->y * s, direction->x);
    } else {
        s = std::cos(angle);
        result = fn_80016260(-direction->y * s, direction->z);
    }
    if (s < 0.0f) {
        result += result < 0.0f ? lbl_805FAD38 : -lbl_805FAD38;
    }
    *angleA = result;
}

void fn_8003B8F4(const Vec3f *direction, float *angleA, float *angleB) {
    float angle = fn_80016260(-direction->x, direction->y);
    *angleB = angle;
    float result;
    float s;
    if (std::fabs(direction->x) > std::fabs(direction->y)) {
        s = std::sin(angle);
        result = fn_80016260(direction->z * s, -direction->x);
    } else {
        s = std::cos(angle);
        result = fn_80016260(direction->z * s, direction->y);
    }
    if (s < 0.0f) {
        result += result < 0.0f ? lbl_805FAD38 : -lbl_805FAD38;
    }
    *angleA = result;
}

/* Roots of a x^2 + b x + c. */
bool fn_8003B7AC(float *root0, float *root1, float a, float b, float c) {
    if (std::fabs(a) > 0.0f) {
        c /= a;
        b /= a;
        if (std::fabs(c) > 0.0f) {
            b *= 0.5f;
            float d = b * b - c;
            if (d > 0.0f) {
                *root0 = b > 0.0f ? -b - fn_8000FEE4(d) : -b + fn_8000FEE4(d);
                *root1 = c / *root0;
                return true;
            } else if (d < 0.0f) {
                return false;
            } else {
                *root0 = *root1 = -b;
                return true;
            }
        } else {
            *root0 = -b;
            *root1 = 0.0f;
            return true;
        }
    }
    if (std::fabs(b) > 0.0001f) {
        *root0 = *root1 = -c / b;
        return true;
    }
    return false;
}

/* Roots of x^2 + b x + c. */
/* Not called anywhere: the linker strips it from the game. Retail pools the 1.0f of
 * fn_8003BC14 before the 0.0001f of fn_8003B7AC, which GC/2.7 does when a function using 1.0f
 * is emitted between fn_8003B6C4 and fn_8003B7AC (inferred from the order of the literals; see
 * docs/research-notes.md). Name inferred. */
float ClampUnit_Unused(float t) {
    return t > 1.0f ? 1.0f : t;
}

bool fn_8003B6C4(float *root0, float *root1, float b, float c) {
    if (std::fabs(c) > 0.0f) {
        b *= 0.5f;
        float d = b * b - c;
        if (d > 0.0f) {
            *root0 = b > 0.0f ? -b - fn_8000FEE4(d) : -b + fn_8000FEE4(d);
            *root1 = c / *root0;
            return true;
        } else if (d < 0.0f) {
            return false;
        } else {
            *root0 = *root1 = -b;
            return true;
        }
    } else {
        *root0 = -b;
        *root1 = 0.0f;
        return true;
    }
}

/* One real root of x^3 + a x^2 + b x + c. */
bool fn_8003B304(float *root, float a, float b, float c) {
    float mq;
    float third = 1.0f / 3.0f;
    float shift = a * third;
    float q = -shift * shift + third * b;
    float r = shift * (shift * shift) - 0.5f * shift * b + 0.5f * c;
    float d = q * (q * q) + r * r;
    if (d > 0.0f) {
        float s = fn_8000FEE4(d);
        *root = CubeRoot(-r + s) + CubeRoot(-r - s) - shift;
    } else if (0.0f == d) {
        *root = 2.0f * CubeRoot(-r) - shift;
    } else {
        mq = -q;
        float angle = 0.33333334f * fn_80015D68(-r / fn_8000FEE4(q * (mq * q)));
        float scale = 2.0f * fn_8000FEE4(mq);
        *root = scale * std::cos(angle) - shift;
    }
    return true;
}

/* Real roots of x^4 + a x^3 + b x^2 + c x + d, in two pairs. */
bool fn_8003B03C(float *root0, float *root1, float *root2, float *root3, float a, float b, float c, float d) {
    float r;
    float shift;
    float p;
    float q;
    shift = a / 4.0f;
    p = -6.0f * shift * shift + b;
    q = c + (shift * (8.0f * shift * shift) - 2.0f * shift * b);
    r = d + (shift * (shift * (-3.0f * shift * shift)) + b * (shift * shift) - shift * c);
    float z;
    fn_8003B304(&z, 0.5f * -p, -r, 0.125f * (4.0f * r * p - q * q));
    bool found;
    if (q >= 0.0f) {
        float u = fn_8000FEE4(2.0f * z - p);
        float v = fn_8000FEE4(z * z - r);
        found = fn_8003B6C4(root0, root1, u, z - v);
        if (found) {
            *root0 -= shift;
            *root1 -= shift;
        }
        if (fn_8003B6C4(root2, root3, -u, z + v)) {
            *root2 -= shift;
            *root3 -= shift;
            if (!found) {
                *root0 = *root2;
                *root1 = *root3;
            }
        } else if (found) {
            *root2 = *root0;
            *root3 = *root1;
        } else {
            return false;
        }
    } else {
        float u = fn_8000FEE4(2.0f * z - p);
        float v = fn_8000FEE4(z * z - r);
        found = fn_8003B6C4(root0, root1, u, z + v);
        if (found) {
            *root0 -= shift;
            *root1 -= shift;
        }
        if (fn_8003B6C4(root2, root3, -u, z - v)) {
            *root2 -= shift;
            *root3 -= shift;
            if (!found) {
                *root0 = *root2;
                *root1 = *root3;
            }
        } else if (found) {
            *root2 = *root0;
            *root3 = *root1;
        } else {
            return false;
        }
    }
    return true;
}
