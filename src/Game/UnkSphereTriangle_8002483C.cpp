/* The tests of a sphere at rest against a triangle of the stage (class name unknown; the unit has
 * no data of its own but its constants): whether the triangle lies on a plane already touched
 * (fn_8002483C), the nearest point of a segment (fn_80024968), the sphere against the triangle
 * (fn_80024AC8, called by Game/UnkCollisionQuery_80020D40.cpp once the sphere has stopped), and
 * whether two segments from a common point cross (fn_800259C0 through the game's vector routines,
 * fn_80025E2C with them inlined). Other units call these functions by their C names; names and
 * meanings are inferred. Definitions are in reverse address order (deferred inlining emits
 * functions last-defined first). */
#include "Game/MathTypes.h"
#include "Game/RangeTriangle.h"
#include <algorithm>
#include <dolphin/types.h>

namespace std {
/* MSL's std::fabs for floats, which the game calls out of line. */
float fabs(float x);
} // namespace std

/* ---- The planes and results the tests skip ---- */

/* A plane touched before (struct name unknown): a node of a list of them, a point on it and its
 * normal. */
struct UnkPlaneNode_80023AA0 {
    /* 0x00 */ UnkPlaneNode_80023AA0 *m_prev;
    /* 0x04 */ UnkPlaneNode_80023AA0 *m_next;
    /* 0x08 */ Vec3f m_point;
    /* 0x14 */ Vec3f m_normal;
};

/* The list of planes: its size and the node that ends it. */
struct UnkPlaneList_80023AA0 {
    /* 0x0 */ u32 m_size;
    /* 0x4 */ UnkPlaneNode_80023AA0 *m_last;
    /* 0x8 */ UnkPlaneNode_80023AA0 *m_first;
};

/* A position in the list of planes. */
struct UnkPlaneIter_80023AA0 {
    UnkPlaneIter_80023AA0() {}

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

/* Room for a position that fn_8001EDFC constructs. */
struct UnkResultIterStorage_8001EDFC {
    CollisionResult *m_node;
};

typedef UnkResultIter_8001EDFC ResultIter;

extern "C" {
/* The list of planes: its first and end position, and it++ (Game/UnkSphereSweep_800221A8.cpp). */
PlaneIter fn_80023AF0(PlaneList *list);
PlaneIter fn_80023AA0(PlaneList *list);
PlaneIter fn_80023ADC(PlaneIter *it, int);
/* The distance between two points (the same unit). */
float fn_80024570(const Vec3f *a, const Vec3f *b);

/* A list of results: its first and end result, a position's constructor (Game/
 * UnkCollisionQuery_80020D40.cpp) and it++ (Game/CollisionResult.cpp). */
CollisionResult *fn_8001EE04(CollisionResultList *list);
CollisionResult *fn_8001EDEC(CollisionResultList *list);
ResultIter *fn_8001EDFC(ResultIter *it, CollisionResult *node);
ResultIter fn_80027374(ResultIter *it, int);
/* A contact's position and normal (Game/UnkCollisionQuery_80020D40.cpp, Game/CollisionResult.cpp). */
const Vec3f *fn_8001E9BC(UnkColliHit_8002686C *hit);
const Vec3f *fn_800266D4(UnkColliHit_8002686C *hit);

/* Where the lines a + s * b and t * d meet (Game/UnkCollisionQuery_80020D40.cpp; meaning inferred). */
void fn_80020F48(const Vec3f *a, const Vec3f *b, const Vec3f *c, const Vec3f *d, float *s, float *t);

/* a . b */
float fn_8040CF90(const Vec3f *a, const Vec3f *b);

bool fn_8002483C(const Vec3f *point, const Vec3f *normal, const Vec3f *planePoint, const Vec3f *planeNormal);
float fn_80024968(const Vec3f *point, const Vec3f *segment, Vec3f *nearest);
}

namespace ret {
extern "C" Vec3f fn_8040CDA8(const Vec3f *v, float scale);
extern "C" Vec3f fn_8040CD74(const Vec3f *a, const Vec3f *b);
extern "C" Vec3f fn_8040CDD0(const Vec3f *a, const Vec3f *b);
extern "C" Vec3f fn_8040CFB8(const Vec3f *a, const Vec3f *b);
} // namespace ret

/* A vector copied and assigned through the out-of-line copy constructor and assignment. */
struct ColliVector {
    ColliVector(const Vec3f &v) {
        fn_800091EC(reinterpret_cast<Vec3f *>(this), &v);
    }
    ColliVector &operator=(const Vec3f &v) {
        fn_800091D0(reinterpret_cast<Vec3f *>(this), &v);
        return *this;
    }
    const Vec3f *Ptr() const {
        return reinterpret_cast<const Vec3f *>(this);
    }

    float x;
    float y;
    float z;
};

/* The absolute value, inline. */
inline float Abs(float x) {
    return x >= 0.0f ? x : -x;
}

/* How far v lies above the plane through p with normal n (times the normal's length). */
inline float Above(const Vec3f &n, const Vec3f &v, const Vec3f &p) {
    return n.x * (v.x - p.x) + n.y * (v.y - p.y) + n.z * (v.z - p.z);
}

/* ---- The tests ---- */

/* Whether the segments from the origin to a and from b to b + c cross (meaning inferred), with
 * the vector routines inlined; nearest receives the crossing. */
extern "C" bool fn_80025E2C(const Vec3f *a, const Vec3f *b, const Vec3f *c, Vec3f *nearest) {
    float abx = a->x + b->x;
    float aby = a->y + b->y;
    float abz = a->z + b->z;
    float ma = std::max(std::max(Abs(a->x), Abs(a->y)), Abs(a->z));
    float mb = std::max(std::max(Abs(b->x), Abs(b->y)), Abs(b->z));
    float mc = std::max(std::max(Abs(c->x), Abs(c->y)), Abs(c->z));
    float nx = c->y * b->z - c->z * b->y;
    float ny = c->z * b->x - c->x * b->z;
    float nz = c->x * b->y - c->y * b->x;
    float eps = 0.0001f * (mc * mb);
    if (Abs(nx) <= eps && Abs(ny) <= eps && Abs(nz) <= eps) {
        eps = 0.0001f * (mc * ma);
        nx = c->y * a->z - c->z * a->y;
        ny = c->z * a->x - c->x * a->z;
        nz = c->x * a->y - c->y * a->x;
        if (Abs(nx) <= eps && Abs(ny) <= eps && Abs(nz) <= eps) {
            float da = c->x * a->x + c->y * a->y + c->z * a->z;
            float dab = c->x * abx + c->y * aby + c->z * abz;
            float cc = c->x * c->x + c->y * c->y + c->z * c->z;
            if ((da < 0.0f && dab < 0.0f) || (cc < da && cc < dab)) {
                return false;
            }
            if (0.0f <= da && da <= cc) {
                if (nearest != 0) {
                    *nearest = *a;
                }
                return true;
            }
            if (0.0f <= dab && dab <= cc) {
                if (da < 0.0f) {
                    if (nearest != 0) {
                        nearest->x = 0.0f;
                        nearest->y = 0.0f;
                        nearest->z = 0.0f;
                    }
                    return true;
                }
                if (nearest != 0) {
                    *nearest = *c;
                }
                return true;
            }
            if (da < 0.0f) {
                if (nearest != 0) {
                    nearest->x = nearest->y = nearest->z = 0.0f;
                }
            } else if (nearest != 0) {
                *nearest = *c;
            }
            return true;
        }
    }
    float mx = c->y * nz - c->z * ny;
    float my = c->z * nx - c->x * nz;
    float mz = c->x * ny - c->y * nx;
    if ((a->x * mx + a->y * my + a->z * mz) * (abx * mx + aby * my + abz * mz) > 0.0f) {
        return false;
    }
    mx = b->y * nz - b->z * ny;
    my = b->z * nx - b->x * nz;
    mz = b->x * ny - b->y * nx;
    float t0 = -(a->x * mx + a->y * my + a->z * mz);
    float t1 = (c->x - a->x) * mx + (c->y - a->y) * my + (c->z - a->z) * mz;
    if (t0 * t1 > 0.0f) {
        return false;
    }
    if (nearest != 0) {
        float t = t0 / (t0 - t1);
        nearest->x = c->x * Abs(t);
        nearest->y = c->y * Abs(t);
        nearest->z = c->z * Abs(t);
    }
    return true;
}

/* The same through the game's vector routines. */
extern "C" bool fn_800259C0(const Vec3f *a, const Vec3f *b, const Vec3f *c, ColliVector *nearest) {
    ColliVector ab(ret::fn_8040CDD0(a, b));
    float eps;
    float ma = std::max(std::max(std::fabs(a->x), std::fabs(a->y)), std::fabs(a->z));
    float mb = std::max(std::max(std::fabs(b->x), std::fabs(b->y)), std::fabs(b->z));
    float mc = std::max(std::max(std::fabs(c->x), std::fabs(c->y)), std::fabs(c->z));
    ColliVector n(ret::fn_8040CFB8(c, b));
    eps = 0.0001f * (mc * mb);
    if (std::fabs(n.x) <= eps && std::fabs(n.y) <= eps && std::fabs(n.z) <= eps) {
        n = ret::fn_8040CFB8(c, a);
        eps = 0.0001f * (mc * ma);
        if (std::fabs(n.x) <= eps && std::fabs(n.y) <= eps && std::fabs(n.z) <= eps) {
            float da = fn_8040CF90(c, a);
            float dab = fn_8040CF90(c, reinterpret_cast<const Vec3f *>(&ab));
            float cc = fn_8040CF2C(c);
            if ((da < 0.0f && dab < 0.0f) || (cc < da && cc < dab)) {
                return false;
            }
            if (0.0f <= da && da <= cc) {
                if (nearest != 0) {
                    *nearest = *a;
                }
                return true;
            }
            if (0.0f <= dab && dab <= cc) {
                if (da < 0.0f) {
                    if (nearest != 0) {
                        *nearest = lbl_805E07A0;
                    }
                    return true;
                }
                if (nearest != 0) {
                    *nearest = *c;
                }
                return true;
            }
            if (da < 0.0f) {
                if (nearest != 0) {
                    *nearest = lbl_805E07A0;
                }
            } else if (nearest != 0) {
                *nearest = *c;
            }
            return true;
        }
    }
    ColliVector m(ret::fn_8040CFB8(c, reinterpret_cast<const Vec3f *>(&n)));
    float s0 = fn_8040CF90(a, reinterpret_cast<const Vec3f *>(&m));
    if (s0 * fn_8040CF90(reinterpret_cast<const Vec3f *>(&ab), reinterpret_cast<const Vec3f *>(&m)) > 0.0f) {
        return false;
    }
    m = ret::fn_8040CFB8(b, reinterpret_cast<const Vec3f *>(&n));
    ColliVector ca(ret::fn_8040CD74(c, a));
    float t0 = fn_8040CF90(a, reinterpret_cast<const Vec3f *>(&m));
    t0 = -t0;
    float t1 = fn_8040CF90(reinterpret_cast<const Vec3f *>(&ca), reinterpret_cast<const Vec3f *>(&m));
    if (t0 * t1 > 0.0f) {
        return false;
    }
    if (nearest != 0) {
        *nearest = ret::fn_8040CDA8(c, std::fabs(t0 / (t0 - t1)));
    }
    return true;
}

/* The squared distance from point to the segment from the origin to segment (the inline copy of
 * fn_80024968, without the nearest point). */
inline float SegmentDistanceSq(float x, float y, float z, const Vec3f &segment) {
    float t = x * segment.x + y * segment.y + z * segment.z;
    if (t > 0.0f) {
        float length = segment.x * segment.x + segment.y * segment.y + segment.z * segment.z;
        if (length < 0.001f) {
            return x * x + y * y + z * z;
        }
        if (t >= length) {
            float dx = x - segment.x;
            float dy = y - segment.y;
            float dz = z - segment.z;
            return dx * dx + dy * dy + dz * dz;
        }
        float f = t / length;
        float dx = x - f * segment.x;
        float dy = y - f * segment.y;
        float dz = z - f * segment.z;
        return dx * dx + dy * dy + dz * dz;
    }
    return x * x + y * y + z * z;
}

/* Whether a sphere of the given radius at center touches the triangle v; point, distance, type
 * (0: the face, 1: an edge, 2: a corner) and index (which edge or corner) describe the contact.
 * A triangle that lies on one of the planes, or on the triangles or contacts of results, is
 * skipped; so is one the sphere moves away from (move) or, unless both sides count, one the
 * sphere is behind (meanings inferred). */
extern "C" bool fn_80024AC8(const Vec3f *center, const Vec3f **v, Vec3f *point, float *distance, s32 *type,
                            s32 *index, PlaneList *planes, CollisionResultList *results, const Vec3f *move,
                            bool contacts, bool bothSides, float radius) {
    Vec3f e1;
    Vec3f e2;
    Vec3f normal;
    Vec3f out;
    Vec3f q;
    Vec3f nearest;
    float s;
    float t;

    e1.x = v[1]->x - v[0]->x;
    float dx = center->x - v[0]->x;
    float dy = center->y - v[0]->y;
    e1.y = v[1]->y - v[0]->y;
    float dz = center->z - v[0]->z;
    e1.z = v[1]->z - v[0]->z;
    e2.x = v[2]->x - v[0]->x;
    e2.y = v[2]->y - v[0]->y;
    e2.z = v[2]->z - v[0]->z;
    normal.z = e1.x * e2.y - e1.y * e2.x;
    normal.x = e1.y * e2.z - e1.z * e2.y;
    normal.y = e1.z * e2.x - e1.x * e2.z;

    if (planes != 0) {
        PlaneIter it;
        for (it = fn_80023AF0(planes); it.m_node != fn_80023AA0(planes).m_node; fn_80023ADC(&it, 0)) {
            PlaneNode *plane = it.m_node;
            if (!bothSides) {
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
            } else if (fn_8002483C(v[0], &normal, &plane->m_point, &plane->m_normal)) {
                return false;
            }
        }
    } else if (results != 0) {
        ResultIter it;
        UnkResultIterStorage_8001EDFC first;
        fn_8001EDFC(reinterpret_cast<ResultIter *>(&first), fn_8001EE04(results));
        for (it = reinterpret_cast<ResultIter &>(first); it.m_node != fn_8001EDEC(results); fn_80027374(&it, 0)) {
            RangeTriangle *shape = it.m_node->m_shape;
            if (shape->m_priority == -1) {
                if (!bothSides) {
                    if (it.m_node->m_hit != 0) {
                        const Vec3f *p = fn_8001E9BC(it.m_node->m_hit);
                        if (Above(*shape->GetNormal(), *v[0], *p) < 0.0001f && Above(*shape->GetNormal(), *v[1], *p) < 0.0001f &&
                            Above(*shape->GetNormal(), *v[2], *p) < 0.0001f) {
                            return false;
                        }
                    }
                } else {
                    UnkColliHit_8002686C *hit = it.m_node->m_hit;
                    if (hit != 0 && fn_8002483C(v[0], &normal, fn_8001E9BC(hit), fn_800266D4(hit))) {
                        return false;
                    }
                }
            } else if (!bothSides) {
                if (contacts) {
                    UnkColliHit_8002686C *hit = it.m_node->m_hit;
                    if (hit != 0) {
                        const Vec3f *p = fn_8001E9BC(hit);
                        const Vec3f *n = fn_800266D4(hit);
                        if (n->x * (v[0]->x - p->x) + n->y * (v[0]->y - p->y) + n->z * (v[0]->z - p->z) < 0.0001f &&
                            n->x * (v[1]->x - p->x) + n->y * (v[1]->y - p->y) + n->z * (v[1]->z - p->z) < 0.0001f &&
                            n->x * (v[2]->x - p->x) + n->y * (v[2]->y - p->y) + n->z * (v[2]->z - p->z) < 0.0001f) {
                            return false;
                        }
                    }
                }
            } else {
                UnkColliHit_8002686C *hit = it.m_node->m_hit;
                if (hit != 0 && fn_8002483C(v[0], &normal, fn_8001E9BC(hit), fn_800266D4(hit))) {
                    return false;
                }
            }
        }
    }

    if (move != 0 && move->x * normal.x + move->y * normal.y + move->z * normal.z >= -0.0001f) {
        return false;
    }
    float dn = dx * normal.x + dy * normal.y + dz * normal.z;
    if (!bothSides && dn < 0.0f) {
        return false;
    }
    float px;
    float py;
    float pz;
    float projection;
    float nn = normal.x * normal.x + normal.y * normal.y + normal.z * normal.z;
    if (nn > 0.001f) {
        float f = dn / nn;
        py = normal.y * f;
        px = normal.x * f;
        pz = normal.z * f;
        projection = px * px + py * py + pz * pz;
        q.x = dx - px;
        q.y = dy - py;
        q.z = dz - pz;
    } else {
        pz = 0.0f;
        py = pz;
        px = pz;
        q.x = dx;
        q.y = dy;
        q.z = dz;
        projection = pz;
    }
    float radiusSq = radius * radius;
    if (0.001f + radiusSq < projection) {
        return false;
    }
    fn_80020F48(&e1, &e2, &q, &normal, &s, &t);

    int region;
    if (s < 0.0f) {
        if (t < 0.0f) {
            if (e1.x * e2.x + e1.y * e2.y + e1.z * e2.z < 0.0f) {
                if (e1.x * dx + e1.y * dy + e1.z * dz > 0.0f) {
                    region = 2;
                } else if (e2.x * dx + e2.y * dy + e2.z * dz > 0.0f) {
                    region = 4;
                } else {
                    region = 0x10;
                }
            } else {
                region = 0x10;
            }
        } else if (s + t > 1.0f) {
            float fx = e1.x - e2.x;
            float fy = e1.y - e2.y;
            float fz = e1.z - e2.z;
            if (e2.x * fx + e2.y * fy + e2.z * fz > 0.0f) {
                float wx = dx - e2.x;
                float wy = dy - e2.y;
                float wz = dz - e2.z;
                if (e2.x * wx + e2.y * wy + e2.z * wz < 0.0f) {
                    region = 4;
                } else if (fx * wx + fy * wy + fz * wz > 0.0f) {
                    region = 8;
                } else {
                    region = 0x40;
                }
            } else {
                region = 0x40;
            }
        } else {
            region = 4;
        }
    } else if (t < 0.0f) {
        if (s + t > 1.0f) {
            float fx = e2.x - e1.x;
            float fy = e2.y - e1.y;
            float fz = e2.z - e1.z;
            if (e1.x * fx + e1.y * fy + e1.z * fz > 0.0f) {
                float wx = dx - e1.x;
                float wy = dy - e1.y;
                float wz = dz - e1.z;
                if (e1.x * wx + e1.y * wy + e1.z * wz < 0.0f) {
                    region = 2;
                } else if (fx * wx + fy * wy + fz * wz > 0.0f) {
                    region = 8;
                } else {
                    region = 0x20;
                }
            } else {
                region = 0x20;
            }
        } else {
            region = 2;
        }
    } else if (s + t > 1.0f) {
        region = 8;
    } else {
        region = 1;
    }

    switch (region) {
    case 1:
        out.x = center->x - px;
        out.y = center->y - py;
        out.z = center->z - pz;
        if (type != 0) {
            *type = 0;
            if (index != 0) {
                *index = 0;
            }
        }
        break;
    case 0x10:
        if (0.001f + radiusSq < dx * dx + dy * dy + dz * dz) {
            return false;
        }
        out.x = center->x - dx;
        out.y = center->y - dy;
        out.z = center->z - dz;
        if (type != 0) {
            *type = 2;
            if (index != 0) {
                *index = 0;
            }
        }
        break;
    case 0x20:
        dy -= e1.y;
        dx -= e1.x;
        dz -= e1.z;
        if (0.001f + radiusSq < dx * dx + dy * dy + dz * dz) {
            return false;
        }
        out.x = center->x - dx;
        out.y = center->y - dy;
        out.z = center->z - dz;
        if (type != 0) {
            *type = 2;
            if (index != 0) {
                *index = 1;
            }
        }
        break;
    case 0x40:
        dy -= e2.y;
        dx -= e2.x;
        dz -= e2.z;
        if (0.001f + radiusSq < dx * dx + dy * dy + dz * dz) {
            return false;
        }
        out.x = center->x - dx;
        out.y = center->y - dy;
        out.z = center->z - dz;
        if (type != 0) {
            *type = 2;
            if (index != 0) {
                *index = 2;
            }
        }
        break;
    case 2: {
        if (0.001f + radiusSq < SegmentDistanceSq(dx, dy, dz, e1)) {
            return false;
        }
        Vec3f from(dx - px, dy - py, dz - pz);
        fn_80024968(&from, &e1, &nearest);
        out.x = nearest.x - dx;
        out.y = nearest.y - dy;
        out.z = nearest.z - dz;
        out.x += center->x;
        out.y += center->y;
        out.z += center->z;
        if (type != 0) {
            *type = 1;
            if (index != 0) {
                *index = 0;
            }
        }
        break;
    }
    case 4: {
        if (0.001f + radiusSq < SegmentDistanceSq(dx, dy, dz, e2)) {
            return false;
        }
        Vec3f from(dx - px, dy - py, dz - pz);
        fn_80024968(&from, &e2, &nearest);
        out.x = nearest.x - dx;
        out.y = nearest.y - dy;
        out.z = nearest.z - dz;
        out.x += center->x;
        out.y += center->y;
        out.z += center->z;
        if (type != 0) {
            *type = 1;
            if (index != 0) {
                *index = 1;
            }
        }
        break;
    }
    case 8: {
        dy -= e1.y;
        e2.y -= e1.y;
        dx -= e1.x;
        e2.x -= e1.x;
        dz -= e1.z;
        e2.z -= e1.z;
        if (0.001f + radiusSq < SegmentDistanceSq(dx, dy, dz, e2)) {
            return false;
        }
        Vec3f from(dx - px, dy - py, dz - pz);
        fn_80024968(&from, &e2, &nearest);
        out.x = nearest.x - dx;
        out.y = nearest.y - dy;
        out.z = nearest.z - dz;
        out.x += center->x;
        out.y += center->y;
        out.z += center->z;
        if (type != 0) {
            *type = 1;
            if (index != 0) {
                *index = 2;
            }
        }
        break;
    }
    }

    if (point != 0) {
        *point = out;
    }
    if (distance != 0) {
        *distance = fn_80024570(center, &out);
    }
    return true;
}

/* Where the lines a + s * b and t * d meet (an out-of-line call of fn_80020F48). */
extern "C" void fn_80024AA8(const Vec3f *a, const Vec3f *b, const Vec3f *c, const Vec3f *d, float *s, float *t) {
    fn_80020F48(a, b, c, d, s, t);
}

/* The squared distance from point to the segment from the origin to segment; nearest receives the
 * nearest point of the segment. */
extern "C" float fn_80024968(const Vec3f *point, const Vec3f *segment, Vec3f *nearest) {
    float t = point->x * segment->x + point->y * segment->y + point->z * segment->z;
    if (t > 0.0f) {
        float length = segment->x * segment->x + segment->y * segment->y + segment->z * segment->z;
        if (length < 0.001f) {
            if (nearest != 0) {
                nearest->x = nearest->y = nearest->z = 0.0f;
            }
            float x = point->x * point->x;
            float y = point->y * point->y;
            float z = point->z * point->z;
            return z + (x + y);
        }
        if (t >= length) {
            float dx = point->x - segment->x;
            float dy = point->y - segment->y;
            float dz = point->z - segment->z;
            if (nearest != 0) {
                *nearest = *segment;
            }
            return dx * dx + dy * dy + dz * dz;
        }
        float f = t / length;
        float nx = f * segment->x;
        float ny = f * segment->y;
        float nz = f * segment->z;
        if (nearest != 0) {
            nearest->x = nx;
            nearest->y = ny;
            nearest->z = nz;
        }
        float dx = point->x - nx;
        float dy = point->y - ny;
        float dz = point->z - nz;
        return dx * dx + dy * dy + dz * dz;
    }
    if (nearest != 0) {
        nearest->x = nearest->y = nearest->z = 0.0f;
    }
    float x = point->x * point->x;
    float y = point->y * point->y;
    float z = point->z * point->z;
    return z + (x + y);
}

/* Whether the plane through point with the given normal is the plane (planePoint, planeNormal):
 * the normals are parallel and the point lies on it, within a tolerance of its size. */
extern "C" bool fn_8002483C(const Vec3f *point, const Vec3f *normal, const Vec3f *planePoint,
                            const Vec3f *planeNormal) {
    float dot = normal->x * planeNormal->x + normal->y * planeNormal->y + normal->z * planeNormal->z;
    if (dot * dot > 0.9999f * (normal->x * normal->x + normal->y * normal->y + normal->z * normal->z)) {
        Vec3f offset(point->x - planePoint->x, point->y - planePoint->y, point->z - planePoint->z);
        float limit = 0.0001f * std::max(std::max(std::fabs(offset.x), std::fabs(offset.y)), std::fabs(offset.z));
        return std::fabs(fn_8040CF90(planeNormal, &offset)) < limit;
    }
    return false;
}
