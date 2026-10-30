/* The tests of a sphere moving against a triangle of the stage (class name unknown; the unit has
 * no data of its own but its constants): the sphere against the triangle (fn_800221A8, called by
 * Game/UnkCollisionQuery_80020D40.cpp while the sphere moves), against an edge (fn_80023B28) and
 * against a corner (fn_800245C8), the list of planes the tests skip (fn_80023AA0 to fn_80023AF0;
 * Game/UnkSphereTriangle_8002483C.cpp walks it too), a square root and the distance between two
 * points. Other units call these functions by their C names; names and meanings are inferred.
 * Definitions are in reverse address order (deferred inlining emits functions last-defined first).
 *
 * The moves the tests copy are RenderWare's plain RwV3d: a struct without constructors is copied
 * through integer registers, as the game does, where Vec3f is copied through float registers.
 *
 * NonMatching (every instruction of the three tests is in place; what differs is register choice):
 * - fn_800221A8 (97%): GC/2.7 gives the radius and the corner's saved contact other float registers
 *   than the game (radius f28 instead of f31; scoping the locals moved it from f24).
 * - fn_80023B28 (97%): the game loads seg again for the cross product although the values it loaded
 *   for seg's squared length stay live (no source shape tried reproduces that), and keeps the cross
 *   product in memory unread; the unused pointer to n is a fake that reproduces only the latter.
 * - fn_800245C8 (95.7%): GC/2.7 won't fuse the first sum of squares into fmadds. */
#include "Game/MathTypes.h"
#include "Game/RangeTriangle.h"
#include <dolphin/types.h>

/* ---- The planes the tests skip ---- */

/* A plane touched before (struct name unknown): a node of a list of them, a point on it and its
 * normal. */
struct UnkPlaneNode_80023AA0 {
    /* 0x00 */ UnkPlaneNode_80023AA0 *m_prev;
    /* 0x04 */ UnkPlaneNode_80023AA0 *m_next;
    /* 0x08 */ Vec3f m_point;
    /* 0x14 */ Vec3f m_normal;
};

/* The list of planes: its size and the node that ends it (its links at 0x4). */
struct UnkPlaneList_80023AA0 {
    /* 0x0 */ u32 m_size;
    /* 0x4 */ UnkPlaneNode_80023AA0 *m_last;
    /* 0x8 */ UnkPlaneNode_80023AA0 *m_first;
};

/* A position in the list of planes. */
struct UnkPlaneIter_80023AA0 {
    UnkPlaneIter_80023AA0() {}
    UnkPlaneIter_80023AA0(UnkPlaneNode_80023AA0 *node) : m_node(node) {}

    /* 0x0 */ UnkPlaneNode_80023AA0 *m_node;
};

typedef UnkPlaneNode_80023AA0 PlaneNode;
typedef UnkPlaneList_80023AA0 PlaneList;
typedef UnkPlaneIter_80023AA0 PlaneIter;

using SonicteamUSA::System::Range::RangeTriangle;

namespace SonicteamUSA {
namespace System {
namespace Collision {
/* What a query found (see Game/CollisionResult.cpp). */
class CollisionResult {
  public:
    /* 0x00 */ u8 field_0x0[0xC];
    /* 0x0C */ RangeTriangle *m_shape;
    /* 0x10 */ u32 field_0x10;
    /* 0x14 */ UnkColliHit_8002686C *m_hit;
};

class CollisionResultList;
} // namespace Collision
} // namespace System
} // namespace SonicteamUSA

using SonicteamUSA::System::Collision::CollisionResult;
using SonicteamUSA::System::Collision::CollisionResultList;

/* A position in a list of results. */
struct UnkResultIter_8001EDFC {
    UnkResultIter_8001EDFC() : m_node(0) {}

    /* 0x0 */ CollisionResult *m_node;
};

typedef UnkResultIter_8001EDFC ResultIter;

/* RenderWare's plain vector. */
struct RwV3d {
    float x;
    float y;
    float z;
};

extern "C" {
/* The square root (MSL's sqrtf), through its out-of-line wrapper and directly. */
float fn_8000FEE4(float x);
float fn_8000FF04(float x);
/* The roots of a * t^2 + b * t + c; false when there are none (behaviour inferred). */
bool fn_8003B7AC(float *t0, float *t1, float a, float b, float c);

PlaneNode *fn_80023AD4(PlaneList *list);
/* A list of results: its first and end result, a position's constructor (Game/
 * UnkCollisionQuery_80020D40.cpp) and it++ (Game/CollisionResult.cpp). */
CollisionResult *fn_8001EE04(CollisionResultList *list);
CollisionResult *fn_8001EDEC(CollisionResultList *list);
/* (It constructs the position in the room its first argument points to; declared here as returning
 * it, which passes the same arguments.) */
ResultIter fn_8001EDFC(CollisionResult *node);
ResultIter fn_80027374(ResultIter *it, int);
/* A contact's position and normal (Game/UnkCollisionQuery_80020D40.cpp, Game/CollisionResult.cpp). */
const Vec3f *fn_8001E9BC(UnkColliHit_8002686C *hit);
const Vec3f *fn_800266D4(UnkColliHit_8002686C *hit);
/* Where the lines a + s * b and t * d meet (Game/UnkSphereTriangle_8002483C.cpp). */
void fn_80024AA8(const Vec3f *a, const Vec3f *b, const Vec3f *c, const Vec3f *d, float *s, float *t);
/* One over the square root. */
float fn_80482394(float x);

PlaneIter fn_80023AF0(PlaneList *list);
PlaneIter fn_80023AA0(PlaneList *list);
PlaneIter fn_80023ADC(PlaneIter *it, int);
bool fn_80023B28(const Vec3f *p, RwV3d *move, const Vec3f *seg, Vec3f *point, float *distance, s32 *type,
                 s32 *index, float radius);
/* The length of a vector. */
float fn_80482300(const Vec3f *v);
/* Whether the segments from the origin to a and from b to b + c cross; nearest receives the crossing
 * (Game/UnkSphereTriangle_8002483C.cpp). */
bool fn_80025E2C(const Vec3f *a, const RwV3d *b, const Vec3f *c, Vec3f *nearest);

bool fn_800245C8(const Vec3f *point, RwV3d *move, float *distance, float radius);
float fn_80024570(const Vec3f *a, const Vec3f *b);
float fn_80024550(float x);
}


/* The squared length of a vector, summed without fusing. */
inline float LengthSq(const Vec3f &v) {
    float x = v.x * v.x;
    float y = v.y * v.y;
    float z = v.z * v.z;
    return z + (x + y);
}

/* The absolute value, inline. */
inline float Abs(float x) {
    return x >= 0.0f ? x : -x;
}

/* How far v lies above the plane through p with normal n (times the normal's length). */
inline float Above(const Vec3f &v, const Vec3f &p, const Vec3f &n) {
    return n.x * (v.x - p.x) + n.y * (v.y - p.y) + n.z * (v.z - p.z);
}

/* The squared length of a move, summed without fusing. */
inline float MoveLengthSq(const RwV3d &v) {
    float x = v.x * v.x;
    float y = v.y * v.y;
    float z = v.z * v.z;
    return z + (x + y);
}

/* ---- The tests ---- */

/* Whether a sphere of the given radius at the origin, moved by move, touches point (given relative
 * to the sphere's start): move is cut down to the contact, and distance receives the distance at
 * the contact (meanings inferred). */
extern "C" bool fn_800245C8(const Vec3f *point, RwV3d *move, float *distance, float radius) {
    int hits = 0;
    float pp = point->x * point->x + point->y * point->y + point->z * point->z;
    float pm = point->x * move->x + point->y * move->y + point->z * move->z;
    float mm = move->x * move->x + move->y * move->y + move->z * move->z;
    float radiusSq = radius * radius;
    if (pp <= 0.001f + radiusSq) {
        move->x = move->y = move->z = 0.0f;
        if (distance != 0) {
            *distance = fn_8000FEE4(pp);
        }
        return true;
    }
    if (pm >= 0.0f) {
        return false;
    }
    float t0;
    float t1;
    if (fn_8003B7AC(&t0, &t1, mm, 2.0f * pm, pp - radiusSq)) {
        if (0.0f <= t0 && t0 <= 1.0f) {
            hits = 1;
        }
        if (0.0f <= t1 && t1 <= 1.0f) {
            hits |= 2;
        }
        if (hits == 0) {
            return false;
        }
        if (distance != 0) {
            *distance = radius;
        }
        switch (hits) {
        case 1:
            move->x = move->x * t0;
            move->y = move->y * t0;
            move->z = move->z * t0;
            return true;
        case 2:
            move->x = move->x * t1;
            move->y = move->y * t1;
            move->z = move->z * t1;
            return true;
        case 3:
            move->x = move->x * (t0 < t1 ? t0 : t1);
            move->y = move->y * (t0 < t1 ? t0 : t1);
            move->z = move->z * (t0 < t1 ? t0 : t1);
            return true;
        }
    }
    return false;
}

/* The distance between two points. */
extern "C" float fn_80024570(const Vec3f *a, const Vec3f *b) {
    float x = (a->x - b->x) * (a->x - b->x);
    float y = (a->y - b->y) * (a->y - b->y);
    float z = (a->z - b->z) * (a->z - b->z);
    return fn_8000FEE4(z + (x + y));
}

/* The square root. */
extern "C" float fn_80024550(float x) {
    return fn_8000FF04(x);
}

/* Whether a sphere of the given radius at the origin, moved by move, touches the segment from p to
 * p + seg (p given relative to the sphere's start): move is cut down to the contact; point, distance,
 * type and index describe it (meanings inferred). */
extern "C" bool fn_80023B28(const Vec3f *p, RwV3d *move, const Vec3f *seg, Vec3f *point, float *distance, s32 *type,
                            s32 *index, float radius) {
    Vec3f n;
    Vec3f q;
    Vec3f nearest;
    Vec3f d;
    Vec3f end;
    Vec3f c;
    Vec3f r;
    Vec3f at;
    Vec3f end2;
    float startDistance;
    float endDistance;
    float distance0;
    float distance1;
    float t;
    bool flag = false;

    float ss = seg->x * seg->x + seg->y * seg->y + seg->z * seg->z;
    if (ss < 0.001f) {
        if (fn_800245C8(p, move, distance, radius)) {
            if (point != 0) {
                point->x = point->y = point->z = 0.0f;
            }
            if (type != 0) {
                *type = 2;
            }
            if (index != 0) {
                *index = 1;
            }
            return true;
        }
        return false;
    }
    float rr = radius * radius;
    float nx = seg->y * move->z - seg->z * move->y;
    float ny = seg->z * move->x - seg->x * move->z;
    float nz = seg->x * move->y - seg->y * move->x;
    n.x = nx;
    n.y = ny;
    n.z = nz;
    Vec3f *pn = &n; /* fake: keeps n in memory (see the top of the file) */
    float nn = nx * nx + ny * ny + nz * nz;
    float np = n.x * p->x + n.y * p->y + n.z * p->z;
    if (nn < 0.001f) {
        t = (seg->x * p->x + seg->y * p->y + seg->z * p->z) / ss;
        float ox = seg->x * t;
        float oy = seg->y * t;
        float oz = seg->z * t;
        d.x = p->x - ox;
        d.y = p->y - oy;
        d.z = p->z - oz;
        float dd = LengthSq(d);
        if (dd > rr) {
            return false;
        }
        if (0.0f <= t && t <= 1.0f) {
            move->x = move->y = move->z = 0.0f;
            if (dd < 0.001f) {
                if (point != 0) {
                    point->x = p->x;
                    point->y = p->y;
                    point->z = p->z;
                }
                if (distance != 0) {
                    *distance = 0.0f;
                }
                if (type != 0) {
                    *type = 4;
                }
                if (index != 0) {
                    *index = 1;
                }
            } else {
                if (point != 0) {
                    point->x = p->x - d.x;
                    point->y = p->y - d.y;
                    point->z = p->z - d.z;
                }
                if (distance != 0) {
                    *distance = fn_80482300(&d);
                }
                if (type != 0) {
                    *type = 1;
                }
            }
            return true;
        }
        if (t < 0.0f) {
            if (fn_800245C8(p, move, &startDistance, radius)) {
                if (type != 0) {
                    if (startDistance < 0.001f) {
                        *type = 4;
                    } else {
                        *type = 2;
                    }
                }
                if (distance != 0) {
                    *distance = startDistance;
                }
                if (point != 0) {
                    point->x = point->y = point->z = 0.0f;
                }
                if (index != 0) {
                    *index = 1;
                }
                return true;
            }
            return false;
        } else if (t > 0.0f) {
            end.x = p->x - seg->x;
            end.y = p->y - seg->y;
            end.z = p->z - seg->z;
            if (fn_800245C8(&end, move, &endDistance, radius)) {
                if (type != 0) {
                    if (endDistance < 0.001f) {
                        *type = 4;
                    } else {
                        *type = 2;
                    }
                }
                if (distance != 0) {
                    *distance = endDistance;
                }
                if (point != 0) {
                    *point = *seg;
                }
                if (index != 0) {
                    *index = 0;
                }
                return true;
            }
            return false;
        }
    }

    float f = np / nn;
    float h = np * f;
    if (h > rr) {
        return false;
    }
    q.x = p->x - nx * f;
    q.y = p->y - ny * f;
    q.z = p->z - nz * f;
    if (h == rr) {
        if (fn_80025E2C(&q, move, seg, &nearest)) {
            if (point != 0) {
                point->x = nearest.x;
                point->y = nearest.y;
                point->z = nearest.z;
            }
            if (distance != 0) {
                *distance = fn_80024570(p, &nearest);
            }
            if (type != 0) {
                *type = 1;
            }
            return true;
        }
        return false;
    }
    float rem = rr - h;
    float sq = seg->x * q.x + seg->y * q.y + seg->z * q.z;
    float u = sq / ss;
    c.x = seg->x * u;
    c.y = seg->y * u;
    c.z = seg->z * u;
    float wx = c.x - q.x;
    float wy = c.y - q.y;
    float wz = c.z - q.z;
    float ww = wx * wx + wy * wy + wz * wz;
    if (sq <= 0.0f) {
        if (q.x * q.x + q.y * q.y + q.z * q.z <= 0.001f + rem) {
            if (point != 0) {
                point->x = point->y = point->z = 0.0f;
            }
            float length = fn_80482300(p);
            if (distance != 0) {
                *distance = length;
            }
            if (type != 0) {
                if (length < 0.001f) {
                    *type = 4;
                } else {
                    *type = 2;
                }
            }
            if (index != 0) {
                *index = 1;
            }
            move->x = move->y = move->z = 0.0f;
            return true;
        }
        if (q.x * move->x + q.y * move->y + q.z * move->z > 0.0f) {
            return false;
        }
    } else if (u >= 1.0f) {
        float ex = seg->x - q.x;
        float ey = seg->y - q.y;
        float ez = seg->z - q.z;
        if (ex * ex + ey * ey + ez * ez <= 0.001f + rem) {
            if (point != 0) {
                point->x = seg->x;
                point->y = seg->y;
                point->z = seg->z;
            }
            float length = fn_80024570(p, seg);
            if (distance != 0) {
                *distance = length;
            }
            if (type != 0) {
                if (length < 0.001f) {
                    *type = 4;
                } else {
                    *type = 2;
                }
            }
            if (index != 0) {
                *index = 0;
            }
            move->x = move->y = move->z = 0.0f;
            return true;
        }
        if (ex * move->x + ey * move->y + ez * move->z < 0.0f) {
            return false;
        }
    } else {
        if (0.001f + rem >= ww) {
            if (point != 0) {
                point->x = c.x;
                point->y = c.y;
                point->z = c.z;
            }
            float length = fn_80024570(p, &c);
            if (distance != 0) {
                *distance = length;
            }
            if (type != 0) {
                if (length < 0.001f) {
                    *type = 4;
                } else {
                    *type = 1;
                }
            }
            move->x = move->y = move->z = 0.0f;
            return true;
        }
        if (wx * move->x + wy * move->y + wz * move->z < 0.0f) {
            return false;
        }
    }
    if (rem >= ww) {
        flag = true;
    }
    float scale = fn_80024550(rem / ww);
    wx *= scale;
    wy *= scale;
    wz *= scale;
    r.x = q.x + wx;
    r.y = q.y + wy;
    r.z = q.z + wz;
    if (!flag && fn_80025E2C(&r, move, seg, &nearest)) {
        if (point != 0) {
            point->x = nearest.x;
            point->y = nearest.y;
            point->z = nearest.z;
        }
        move->x = nearest.x - r.x;
        move->y = nearest.y - r.y;
        move->z = nearest.z - r.z;
        if (distance != 0) {
            at.x = p->x + move->x;
            at.y = p->y + move->y;
            at.z = p->z + move->z;
            *distance = fn_80024570(&at, &nearest);
        }
        if (type != 0) {
            *type = 1;
        }
        return true;
    }
    RwV3d move0 = *move;
    RwV3d move1 = *move;
    flag = fn_800245C8(p, &move0, &distance0, radius);
    end2.x = p->x - seg->x;
    end2.y = p->y - seg->y;
    end2.z = p->z - seg->z;
    bool hit = fn_800245C8(&end2, &move1, &distance1, radius);
    if (flag && hit) {
        if (distance0 < distance1) {
            hit = false;
        } else {
            flag = false;
        }
    }
    if (flag) {
        move->x = move0.x;
        move->y = move0.y;
        move->z = move0.z;
        if (point != 0) {
            point->x = point->y = point->z = 0.0f;
        }
        if (distance != 0) {
            *distance = distance0;
        }
        if (type != 0) {
            *type = 2;
        }
        if (index != 0) {
            *index = 1;
        }
        return true;
    }
    if (hit) {
        move->x = move1.x;
        move->y = move1.y;
        move->z = move1.z;
        if (point != 0) {
            *point = *seg;
        }
        if (distance != 0) {
            *distance = distance1;
        }
        if (type != 0) {
            *type = 2;
        }
        if (index != 0) {
            *index = 0;
        }
        return true;
    }
    return false;
}

/* The list of planes: its first position, it++, the node that ends it and its end position. */
extern "C" PlaneIter fn_80023AF0(PlaneList *list) {
    return PlaneIter(fn_80023AD4(list)->m_next);
}

extern "C" PlaneIter fn_80023ADC(PlaneIter *it, int) {
    PlaneIter old(it->m_node);
    it->m_node = it->m_node->m_next;
    return old;
}

extern "C" PlaneNode *fn_80023AD4(PlaneList *list) {
    return reinterpret_cast<PlaneNode *>(&list->m_last);
}

extern "C" PlaneIter fn_80023AA0(PlaneList *list) {
    return PlaneIter(fn_80023AD4(list));
}

/* ---- The sphere against a triangle ---- */

/* Whether a sphere of the given radius at center, moved by move, touches the triangle v: move is
 * cut down to the contact; point, distance, type (0: the face, 1: an edge, 2: a corner) and index
 * (which edge or corner) describe it. A triangle that lies behind one of the planes, or behind the
 * triangles (or, with contacts, the contacts) of results, is skipped; so is one the sphere moves
 * away from (meanings inferred). */
extern "C" bool fn_800221A8(const Vec3f *center, RwV3d *move, const Vec3f **v, Vec3f *point, float *distance,
                            s32 *type, s32 *index, PlaneList *planes, CollisionResultList *results, bool contacts,
                            float radius) {
    Vec3f d;
    Vec3f e1;
    Vec3f e2;
    Vec3f o;
    Vec3f q2;
    Vec3f n;
    Vec3f q;
    float s;
    float t;
    float mx;
    float my;
    float mz;

    {
        float lo = center->y - (0.001f + radius);
        float hi = center->y + (0.001f + radius);
        bool below = false;
        bool down = move->y < 0.0f;
        if (down) {
            lo += move->y;
        }
        if (!down) {
            hi += move->y;
        }
        if (v[0]->y < lo) {
            below = true;
        } else if (v[0]->y <= hi) {
            goto insideY;
        }
        if (v[1]->y < lo) {
            if (!below) {
                goto insideY;
            }
        } else if (v[1]->y > hi) {
            if (below) {
                goto insideY;
            }
        } else {
            goto insideY;
        }
        if (v[2]->y < lo) {
            if (!below) {
                goto insideY;
            }
            return false;
        } else if (v[2]->y > hi) {
            if (below) {
                goto insideY;
            }
            return false;
        } else {
            goto insideY;
        }
    }
insideY:
    {
        float lo = center->x - (0.001f + radius);
        float hi = center->x + (0.001f + radius);
        bool below = false;
        bool down = move->x < 0.0f;
        if (down) {
            lo += move->x;
        }
        if (!down) {
            hi += move->x;
        }
        if (v[0]->x < lo) {
            below = true;
        } else if (v[0]->x <= hi) {
            goto insideX;
        }
        if (v[1]->x < lo) {
            if (!below) {
                goto insideX;
            }
        } else if (v[1]->x > hi) {
            if (below) {
                goto insideX;
            }
        } else {
            goto insideX;
        }
        if (v[2]->x < lo) {
            if (!below) {
                goto insideX;
            }
            return false;
        } else if (v[2]->x > hi) {
            if (below) {
                goto insideX;
            }
            return false;
        } else {
            goto insideX;
        }
    }
insideX:
    {
        float lo = center->z - (0.001f + radius);
        float hi = center->z + (0.001f + radius);
        bool below = false;
        bool down = move->z < 0.0f;
        if (down) {
            lo += move->z;
        }
        if (!down) {
            hi += move->z;
        }
        if (v[0]->z < lo) {
            below = true;
        } else if (v[0]->z <= hi) {
            goto insideZ;
        }
        if (v[1]->z < lo) {
            if (!below) {
                goto insideZ;
            }
        } else if (v[1]->z > hi) {
            if (below) {
                goto insideZ;
            }
        } else {
            goto insideZ;
        }
        if (v[2]->z < lo) {
            if (!below) {
                goto insideZ;
            }
            return false;
        } else if (v[2]->z > hi) {
            if (below) {
                goto insideZ;
            }
            return false;
        } else {
            goto insideZ;
        }
    }
insideZ:

    e1.x = v[1]->x - v[0]->x;
    e1.y = v[1]->y - v[0]->y;
    e1.z = v[1]->z - v[0]->z;
    e2.x = v[2]->x - v[0]->x;
    e2.y = v[2]->y - v[0]->y;
    e2.z = v[2]->z - v[0]->z;
    n.z = e1.x * e2.y - e1.y * e2.x;
    n.x = e1.y * e2.z - e1.z * e2.y;
    n.y = e1.z * e2.x - e1.x * e2.z;

    if (planes != 0) {
        PlaneIter it;
        for (it = fn_80023AF0(planes); it.m_node != fn_80023AA0(planes).m_node; fn_80023ADC(&it, 0)) {
            PlaneNode *plane = it.m_node;
            if (plane->m_normal.x * (v[0]->x - plane->m_point.x) + plane->m_normal.y * (v[0]->y - plane->m_point.y) +
                        plane->m_normal.z * (v[0]->z - plane->m_point.z) <
                    0.0001f &&
                plane->m_normal.x * (v[1]->x - plane->m_point.x) + plane->m_normal.y * (v[1]->y - plane->m_point.y) +
                        plane->m_normal.z * (v[1]->z - plane->m_point.z) <
                    0.0001f &&
                plane->m_normal.x * (v[2]->x - plane->m_point.x) + plane->m_normal.y * (v[2]->y - plane->m_point.y) +
                        plane->m_normal.z * (v[2]->z - plane->m_point.z) <
                    0.0001f) {
                return false;
            }
        }
    } else if (results != 0) {
        ResultIter it;
        for (it = fn_8001EDFC(fn_8001EE04(results)); it.m_node != fn_8001EDEC(results); fn_80027374(&it, 0)) {
            RangeTriangle *shape = it.m_node->m_shape;
            if (shape->m_priority == -1) {
                if (it.m_node->m_hit != 0) {
                    const Vec3f *p = fn_8001E9BC(it.m_node->m_hit);
                    if (Above(*v[0], *p, *shape->GetNormal()) < 0.0001f && Above(*v[1], *p, *shape->GetNormal()) < 0.0001f &&
                        Above(*v[2], *p, *shape->GetNormal()) < 0.0001f) {
                        return false;
                    }
                }
            } else if (contacts) {
                UnkColliHit_8002686C *hit = it.m_node->m_hit;
                if (hit != 0) {
                    const Vec3f *p = fn_8001E9BC(hit);
                    const Vec3f *pn = fn_800266D4(hit);
                    if (pn->x * (v[0]->x - p->x) + pn->y * (v[0]->y - p->y) + pn->z * (v[0]->z - p->z) < 0.0001f &&
                        pn->x * (v[1]->x - p->x) + pn->y * (v[1]->y - p->y) + pn->z * (v[1]->z - p->z) < 0.0001f &&
                        pn->x * (v[2]->x - p->x) + pn->y * (v[2]->y - p->y) + pn->z * (v[2]->z - p->z) < 0.0001f) {
                        return false;
                    }
                }
            }
        }
    }

    if (move->x * n.x + move->y * n.y + move->z * n.z >= -0.0001f) {
        return false;
    }
    float nn = n.x * n.x + n.y * n.y + n.z * n.z;
    d.x = center->x - v[0]->x;
    d.y = center->y - v[0]->y;
    d.z = center->z - v[0]->z;
    float scale = d.x * n.x + d.y * n.y + d.z * n.z;
    float inv = fn_80482394(nn);
    float h = scale * inv;
    if (h <= -radius) {
        return false;
    }
    float k = -(inv * h);
    o.x = n.x * k;
    o.y = n.y * k;
    o.z = n.z * k;
    float a = Abs(h);
    if (a <= 0.001f + radius) {
        q.x = d.x + o.x;
        q.y = d.y + o.y;
        q.z = d.z + o.z;
        fn_80024AA8(&e1, &e2, &q, &n, &s, &t);
        if (s >= 0.0f && t >= 0.0f && s + t <= 1.0f) {
            if (point != 0) {
                point->x = v[0]->x + q.x;
                point->y = v[0]->y + q.y;
                point->z = v[0]->z + q.z;
            }
            if (distance != 0) {
                *distance = fn_80482300(&o);
            }
            if (type != 0) {
                *type = 0;
            }
            if (index != 0) {
                *index = 0;
            }
            move->x = move->y = move->z = 0.0f;
            return true;
        }
    } else {
        if (a < 0.001f) {
            return false;
        }
        float f = (a - radius) / a;
        float ox = o.x * f;
        float oy = o.y * f;
        float oz = o.z * f;
        float dm = ox * move->x + oy * move->y + oz * move->z;
        float oo = ox * ox + oy * oy + oz * oz;
        if (oo >= 1.0001f * dm) {
            return false;
        }
        scale = oo / dm;
        mx = move->x * scale;
        my = move->y * scale;
        mz = move->z * scale;
        q2.x = d.x + mx + (o.x - ox);
        q2.y = d.y + my + (o.y - oy);
        q2.z = d.z + mz + (o.z - oz);
        fn_80024AA8(&e1, &e2, &q2, &n, &s, &t);
        if (s >= 0.0f && t >= 0.0f && s + t <= 1.0f) {
            if (point != 0) {
                point->x = v[0]->x + q2.x;
                point->y = v[0]->y + q2.y;
                point->z = v[0]->z + q2.z;
            }
            if (distance != 0) {
                *distance = radius;
            }
            if (scale < 1.0f) {
                move->x = mx;
                move->y = my;
                move->z = mz;
            }
            if (type != 0) {
                *type = 0;
            }
            if (index != 0) {
                *index = 0;
            }
            return true;
        }
    }

    if (s < 0.0f) {
        if (t < 0.0f) {
            /* The corner v[0]: its two edges. */
            RwV3d edgeMove = *move;
            float edgeDistance;
            float edgeDistance2;
            float px;
            float py;
            float pz;
            s32 savedType;
            bool hit = fn_80023B28(&d, &edgeMove, &e1, point, &edgeDistance, type, 0, radius);
            if (hit == true) {
                if (index != 0) {
                    *index = 0;
                }
                if (edgeDistance < radius - 0.001f) {
                    if (point != 0) {
                        point->x += v[0]->x;
                        point->y += v[0]->y;
                        point->z += v[0]->z;
                    }
                    if (distance != 0) {
                        *distance = edgeDistance;
                    }
                    move->x = edgeMove.x;
                    move->y = edgeMove.y;
                    move->z = edgeMove.z;
                    return true;
                }
                if (point != 0) {
                    px = point->x;
                    py = point->y;
                    pz = point->z;
                }
                if (type != 0) {
                    savedType = *type;
                }
            }
            bool hit2 = fn_80023B28(&d, move, &e2, point, &edgeDistance2, type, 0, radius);
            if ((hit2 && edgeDistance2 < radius - 0.001f) || (!hit && hit2)) {
                if (point != 0) {
                    point->x += v[0]->x;
                    point->y += v[0]->y;
                    point->z += v[0]->z;
                }
                if (distance != 0) {
                    *distance = edgeDistance2;
                }
                if (index != 0) {
                    *index = 1;
                }
                return true;
            }
            if (!hit && !hit2) {
                return false;
            }
            if (hit && !hit2) {
                if (point != 0) {
                    point->x += v[0]->x;
                    point->y += v[0]->y;
                    point->z += v[0]->z;
                }
                if (distance != 0) {
                    *distance = edgeDistance;
                }
                move->x = edgeMove.x;
                move->y = edgeMove.y;
                move->z = edgeMove.z;
                return true;
            }
            if (MoveLengthSq(edgeMove) <= MoveLengthSq(*move)) {
                move->x = edgeMove.x;
                move->y = edgeMove.y;
                move->z = edgeMove.z;
                if (point != 0) {
                    point->x = px + v[0]->x;
                    point->y = py + v[0]->y;
                    point->z = pz + v[0]->z;
                }
                if (distance != 0) {
                    *distance = edgeDistance;
                }
                if (type != 0) {
                    *type = savedType;
                }
            } else {
                if (distance != 0) {
                    *distance = edgeDistance2;
                }
                if (point != 0) {
                    point->x += v[0]->x;
                    point->y += v[0]->y;
                    point->z += v[0]->z;
                }
                if (index != 0) {
                    *index = 1;
                }
            }
            return true;
        } else if (s + t > 1.0f) {
            /* The corner v[2]: its two edges. */
            RwV3d edgeMove = *move;
            float edgeDistance;
            float edgeDistance2;
            float px;
            float py;
            float pz;
            s32 savedType;
            e1.x -= e2.x;
            d.x -= e2.x;
            e1.y -= e2.y;
            d.y -= e2.y;
            e1.z -= e2.z;
            d.z -= e2.z;
            bool hit = fn_80023B28(&d, &edgeMove, &e1, point, &edgeDistance, type, 0, radius);
            if (hit == true) {
                if (index != 0) {
                    *index = 2;
                }
                if (edgeDistance < radius - 0.001f) {
                    if (point != 0) {
                        point->x += v[2]->x;
                        point->y += v[2]->y;
                        point->z += v[2]->z;
                    }
                    if (distance != 0) {
                        *distance = edgeDistance;
                    }
                    move->x = edgeMove.x;
                    move->y = edgeMove.y;
                    move->z = edgeMove.z;
                    return true;
                }
                if (point != 0) {
                    px = point->x;
                    py = point->y;
                    pz = point->z;
                }
                if (type != 0) {
                    savedType = *type;
                }
            }
            e2.x = -e2.x;
            e2.y = -e2.y;
            e2.z = -e2.z;
            bool hit2 = fn_80023B28(&d, move, &e2, point, &edgeDistance2, type, 0, radius);
            if ((hit2 && edgeDistance2 < radius - 0.001f) || (!hit && hit2)) {
                if (point != 0) {
                    point->x += v[2]->x;
                    point->y += v[2]->y;
                    point->z += v[2]->z;
                }
                if (distance != 0) {
                    *distance = edgeDistance2;
                }
                if (index != 0) {
                    *index = 1;
                }
                return true;
            }
            if (!hit && !hit2) {
                return false;
            }
            if (hit && !hit2) {
                if (point != 0) {
                    point->x += v[2]->x;
                    point->y += v[2]->y;
                    point->z += v[2]->z;
                }
                if (distance != 0) {
                    *distance = edgeDistance;
                }
                move->x = edgeMove.x;
                move->y = edgeMove.y;
                move->z = edgeMove.z;
                return true;
            }
            if (MoveLengthSq(edgeMove) <= MoveLengthSq(*move)) {
                move->x = edgeMove.x;
                move->y = edgeMove.y;
                move->z = edgeMove.z;
                if (point != 0) {
                    point->x = px + v[2]->x;
                    point->y = py + v[2]->y;
                    point->z = pz + v[2]->z;
                }
                if (distance != 0) {
                    *distance = edgeDistance;
                }
                if (type != 0) {
                    *type = savedType;
                }
            } else {
                if (distance != 0) {
                    *distance = edgeDistance2;
                }
                if (point != 0) {
                    point->x += v[2]->x;
                    point->y += v[2]->y;
                    point->z += v[2]->z;
                }
                if (index != 0) {
                    *index = 1;
                }
            }
            return true;
        } else {
            /* The edge from v[0] to v[2]. */
            s32 end;
            if (fn_80023B28(&d, move, &e2, point, distance, type, &end, radius)) {
                if (point != 0) {
                    point->x += v[0]->x;
                    point->y += v[0]->y;
                    point->z += v[0]->z;
                }
                if (type != 0 && index != 0) {
                    if (*type == 2) {
                        *index = end != 0 ? 0 : 2;
                    } else {
                        *index = 1;
                    }
                }
                return true;
            }
            return false;
        }
    } else if (t < 0.0f) {
        if (s + t > 1.0f) {
            /* The corner v[1]: its two edges. */
            RwV3d edgeMove = *move;
            float edgeDistance;
            float edgeDistance2;
            float px;
            float py;
            float pz;
            s32 savedType;
            e2.x -= e1.x;
            d.x -= e1.x;
            e2.y -= e1.y;
            d.y -= e1.y;
            e2.z -= e1.z;
            d.z -= e1.z;
            bool hit = fn_80023B28(&d, &edgeMove, &e2, point, &edgeDistance, type, 0, radius);
            if (hit == true) {
                if (index != 0) {
                    *index = 1;
                }
                if (edgeDistance < radius - 0.001f) {
                    if (point != 0) {
                        point->x += v[1]->x;
                        point->y += v[1]->y;
                        point->z += v[1]->z;
                    }
                    if (distance != 0) {
                        *distance = edgeDistance;
                    }
                    move->x = edgeMove.x;
                    move->y = edgeMove.y;
                    move->z = edgeMove.z;
                    return true;
                }
                if (point != 0) {
                    px = point->x;
                    py = point->y;
                    pz = point->z;
                }
                if (type != 0) {
                    savedType = *type;
                }
            }
            e1.x = -e1.x;
            e1.y = -e1.y;
            e1.z = -e1.z;
            bool hit2 = fn_80023B28(&d, move, &e1, point, &edgeDistance2, type, 0, radius);
            if ((hit2 && edgeDistance2 < radius - 0.001f) || (!hit && hit2)) {
                if (point != 0) {
                    point->x += v[1]->x;
                    point->y += v[1]->y;
                    point->z += v[1]->z;
                }
                if (distance != 0) {
                    *distance = edgeDistance2;
                }
                if (index != 0) {
                    *index = 0;
                }
                return true;
            }
            if (!hit && !hit2) {
                return false;
            }
            if (hit && !hit2) {
                if (point != 0) {
                    point->x += v[1]->x;
                    point->y += v[1]->y;
                    point->z += v[1]->z;
                }
                if (distance != 0) {
                    *distance = edgeDistance;
                }
                move->x = edgeMove.x;
                move->y = edgeMove.y;
                move->z = edgeMove.z;
                return true;
            }
            if (MoveLengthSq(edgeMove) <= MoveLengthSq(*move)) {
                move->x = edgeMove.x;
                move->y = edgeMove.y;
                move->z = edgeMove.z;
                if (point != 0) {
                    point->x = px + v[1]->x;
                    point->y = py + v[1]->y;
                    point->z = pz + v[1]->z;
                }
                if (distance != 0) {
                    *distance = edgeDistance;
                }
                if (type != 0) {
                    *type = savedType;
                }
            } else {
                if (distance != 0) {
                    *distance = edgeDistance2;
                }
                if (point != 0) {
                    point->x += v[1]->x;
                    point->y += v[1]->y;
                    point->z += v[1]->z;
                }
                if (index != 0) {
                    *index = 0;
                }
            }
            return true;
        } else {
            /* The edge from v[0] to v[1]. */
            s32 end;
            if (fn_80023B28(&d, move, &e1, point, distance, type, &end, radius)) {
                if (point != 0) {
                    point->x += v[0]->x;
                    point->y += v[0]->y;
                    point->z += v[0]->z;
                }
                if (type != 0 && index != 0) {
                    if (*type == 2) {
                        *index = end == 0 ? 1 : 0;
                    } else {
                        *index = 0;
                    }
                }
                return true;
            }
            return false;
        }
    } else if (s + t > 1.0f) {
        /* The edge from v[1] to v[2]. */
        s32 end;
        e2.x -= e1.x;
        d.x -= e1.x;
        e2.y -= e1.y;
        d.y -= e1.y;
        e2.z -= e1.z;
        d.z -= e1.z;
        if (fn_80023B28(&d, move, &e2, point, distance, type, &end, radius)) {
            if (point != 0) {
                point->x += v[1]->x;
                point->y += v[1]->y;
                point->z += v[1]->z;
            }
            if (type != 0 && index != 0) {
                if (*type == 2) {
                    *index = end != 0 ? 1 : 2;
                } else {
                    *index = 2;
                }
            }
            return true;
        }
        return false;
    }
    return false;
}
