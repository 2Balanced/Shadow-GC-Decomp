/* RenderWare: RpHAnim (plugin 0x11E, rwID_HANIMPLUGIN), 0x80445CF4..0x80447598: the standard
 * keyframe scheme the plugin registers with RtAnim (apply, blend, interpolate, stream read, write
 * and size, add, multiply by the inverse), then the plugin: the frame callback that links a
 * hierarchy's nodes to their frames, the engine callbacks (the hierarchy free list), the frame
 * extension's callbacks, the attach, and creating, destroying and attaching hierarchies.
 * Its .sdata2 pool (0x805FAFE8..0x805FB050) is the keyframe code's (the slerp's acos and sine
 * polynomials), its .bss the free list's in-place space (0x805E1510), its .sdata the free list's
 * block size and preallocation (0x805EDD70) and its .sbss the frame extension offset and the free
 * list (0x805F1440). The keyframe scheme reaches none of the plugin's data and may be a file of its
 * own; nothing in the binary marks a cut between them.
 * Built as C without lmw/stmw at -O4,p, inlining only what is declared inline, in definition order
 * (cflags_rw_c: cflags_rw_r3, _r5, _r6 and the game's flags give the same code for every matched
 * function here; this one emits in definition order and inlines only what is marked inline, as
 * retail's separate static helpers need). Names are inferred from behavior. fn_804474D8 matches
 * with its body in an inline function (AttachHierarchy: retail copies the zeroed counter), the
 * keyframe stream functions with their locals declared in retail's register order (the write
 * takes the animation first, as RpHAnimKeyFrameStreamWrite does).
 * NonMatching: fn_80445CF4, fn_80445E08, fn_8044621C, fn_80446790, fn_804468D0 (drafts),
 * fn_80446780 (retail multiplies by 36 with li + mullw where every compiler and spelling tried
 * gives mulli), fn_804472AC (6 rows: retail gives the key frame size and the new hierarchy the
 * same r31). */
#include <dolphin/types.h>
#include "Game/rw/r1.h"

extern u8 *lbl_805F15B4;

/* The frame extension's offset and the hierarchy free list. */
s32 lbl_805F1440[2];
static s32 s_freeListBlockSize = 0x80;
static s32 s_freeListPreallocBlocks = 1;
static u32 s_freeListSpace[10];

typedef void (*R1Callback)(void);

float fn_80482340(float);
u32 fn_8047D784(void *, void *, u32);
u32 fn_8047D868(void *, void *, u32);
u32 fn_8047D350(void *, void *, u32);
u32 fn_8047D458(void *, void *, u32);
void fn_8048722C(void *);
void fn_8048752C(void *, void *, void *);
void *fn_8047FC04(u32, u32, u32, u32, void *, u32);
void fn_8047FCDC(void *);
void fn_80477344(void *);
void *fn_804779DC(int, int);
u32 fn_80477A8C(void *);
s32 fn_80485D78(u32, u32, R1Callback, R1Callback);
s32 fn_804878D4(u32, u32, R1Callback, R1Callback, R1Callback);
s32 fn_804836EC(u32, R1Callback, R1Callback, R1Callback);
int fn_8044758C(void *frame);
int fn_80447418(R1Hierarchy *);
R1Hierarchy *fn_804472AC(int, int *, int *, int, int);

/* Keyframe apply: the quaternion and translation as a matrix. NonMatching draft. */
void fn_80445CF4(float *out, const float *q) {
    float x = q[2];
    float y = q[3];
    float z = q[4];
    float w = q[5];
    float two = 2.0f;
    float one = 1.0f;
    float xx = x * x;
    float yy = y * y;
    float zz = z * z;
    float xy = x * y;
    float wz = w * z;
    float yz = y * z;
    float wx = w * x;
    float zx = z * x;
    float wy = w * y;

    out[0] = one - two * (yy + zz);
    out[1] = two * (xy + wz);
    out[2] = two * (zx - wy);
    *(u32 *)(out + 3) = 3;
    out[4] = two * (xy - wz);
    out[5] = one - two * (xx + zz);
    out[6] = two * (yz + wx);
    out[8] = two * (zx + wy);
    out[9] = two * (yz - wx);
    out[10] = one - two * (xx + yy);
    out[12] = q[6];
    out[13] = q[7];
    out[14] = q[8];
}

static inline float AnglePolynomial(float dot) {
    float x = dot * dot;
    float a = 3.479331e-05f * x + 0.000791535f;
    float b = x * a + -0.040055536f;
    float c = 0.077038154f * x + -0.688284f;
    float d = x * b + 0.20121253f;
    float e = x * c + 2.0209458f;
    float f = x * d + -0.32556581f;
    float g = x * e + -2.403395f;
    float h = x * f + 0.16666667f;
    float i = x * g + 1.0f;
    float j = x * h;
    float k = j / i;
    float l = 7.5497894e-08f - dot * k;
    float m = dot - l;
    return 1.5707963f - m;
}

static inline float AngleSqrt(float dot, int negative) {
    float x = 0.5f * (negative ? 1.0f + dot : 1.0f - dot);
    float root = fn_80482340(x);
    u32 rootBits;
    float adjusted;
    float a, b, c, d, e, f, g, h;
    float numerator, denominator, first, second;
    if (negative) {
        float i, j, k;
        a = 3.479331e-05f * x + 0.000791535f;
        b = x * a + -0.040055536f;
        c = 0.077038154f * x + -0.688284f;
        d = x * b + 0.20121253f;
        e = x * c + 2.0209458f;
        f = x * d + -0.32556581f;
        g = x * e + 0.16666667f;
        h = x * g;
        i = h / 1.0f;
        j = i * root - 7.5497894e-08f;
        k = 7.5497894e-08f + j;
        return 3.1415925f - 2.0f * k;
    }

    rootBits = *(u32 *)&root;
    rootBits &= 0xFFFFF000;
    adjusted = *(float *)&rootBits;
    a = 3.479331e-05f * x + 0.000791535f;
    b = x * a + -0.040055536f;
    c = 0.077038154f * x + -0.688284f;
    d = x * b + 0.20121253f;
    e = x * c + 2.0209458f;
    f = x * d + -0.32556581f;
    g = x * e + 0.16666667f;
    h = x * f + -2.403395f;
    numerator = x - adjusted * adjusted;
    denominator = root + adjusted;
    first = numerator / denominator;
    second = (x * g) / (x * h + 1.0f);
    return 2.0f * (adjusted + second * root + first);
}

static inline float Angle(float dot) {
    u32 bits = *(u32 *)&dot;
    u32 magnitude = bits << 1;
    if (magnitude >= 0x3F800000) {
        return (bits & 0x80000000) ? 3.1415927f : 0.0f;
    }
    if (magnitude < 0x3F000000) {
        if (magnitude <= 0x23000000) return 1.5707964f;
        return AnglePolynomial(dot);
    }
    return AngleSqrt(dot, (bits & 0x80000000) != 0);
}

static inline void NormalizeWeights(float angle, float *first, float *second) {
    float angle2 = angle * angle;
    float f0 = 1.589691e-10f * angle2 + -2.505076e-08f;
    float f3 = angle2 * f0 + 2.7557314e-06f;
    float f1 = 1.589691e-10f * (*first * *first) + -2.505076e-08f;
    float f2 = 1.589691e-10f * (*second * *second) + -2.505076e-08f;
    float f7;
    float f6;
    float f3a;
    float scale;
    float f2a;
    float f1a;
    f3 = angle2 * f3 + -0.0001984127f;
    f1 = *first * *first * f1 + 2.7557314e-06f;
    f2 = *second * *second * f2 + 2.7557314e-06f;
    f3 = angle2 * f3 + 0.008333334f;
    f7 = angle2 * angle;
    f1 = *first * *first * f1 + -0.0001984127f;
    f3 = angle2 * f3 + -0.16666667f;
    f2 = *second * *second * f2 + -0.0001984127f;
    f1 = *first * *first * f1 + 0.008333334f;
    f6 = f7 * f3 + angle;
    f2 = *second * *second * f2 + 0.008333334f;
    f3a = *first * *first * *first;
    scale = 1.0f / f6;
    f2a = *first * *first * f1 + -0.16666667f;
    f1a = *second * *second * *second;
    f2 = *second * *second * f2 + -0.16666667f;
    *first = (f3a * f2a + *first) * scale;
    *second = (f1a * f2 + *second) * scale;
}

/* Keyframe blend: a spherical blend of the rotations, a linear one of the translations.
 * NonMatching draft. */
void fn_80445E08(float *out, float *a, float *b, float t) {
    float ratio = (t - a[1]) / (b[1] - a[1]);
    float dot;
    float angle;
    float first;
    float second;
    out[6] = a[6] + ratio * (b[6] - a[6]);
    out[7] = a[7] + ratio * (b[7] - a[7]);
    out[8] = a[8] + ratio * (b[8] - a[8]);

    dot = a[3] * b[3] + a[2] * b[2] + a[4] * b[4] + a[5] * b[5];
    if (dot < 0.0f) {
        dot = -dot;
        b[2] = -b[2];
        b[3] = -b[3];
        b[4] = -b[4];
        b[5] = -b[5];
    }
    angle = Angle(dot);
    first = (1.0f - ratio) * angle;
    second = ratio * angle;
    NormalizeWeights(angle, &first, &second);
    out[2] = second * b[2] + first * a[2];
    out[3] = second * b[3] + first * a[3];
    out[4] = second * b[4] + first * a[4];
    out[5] = second * b[5] + first * a[5];
}

/* Keyframe interpolate: the same at a given fraction. NonMatching draft. */
void fn_8044621C(float *out, float *a, float *b, float t) {
    float dot;
    float angle;
    float first;
    float second;
    out[6] = a[6] + t * (b[6] - a[6]);
    out[7] = a[7] + t * (b[7] - a[7]);
    out[8] = a[8] + t * (b[8] - a[8]);

    dot = a[3] * b[3] + a[2] * b[2] + a[4] * b[4] + a[5] * b[5];
    if (dot < 0.0f) {
        dot = -dot;
        b[2] = -b[2];
        b[3] = -b[3];
        b[4] = -b[4];
        b[5] = -b[5];
    }
    angle = Angle(dot);
    first = (1.0f - t) * angle;
    second = t * angle;
    NormalizeWeights(angle, &first, &second);
    out[2] = second * b[2] + first * a[2];
    out[3] = second * b[3] + first * a[3];
    out[4] = second * b[4] + first * a[4];
    out[5] = second * b[5] + first * a[5];
}

/* Keyframe stream read. NonMatching draft. */
void *fn_80446620(void *stream, u32 *animation) {
    u32 *entry;
    int i;
    u8 *frames;
    frames = (u8 *)animation[4];
    entry = (u32 *)frames;
    for (i = 0; i < (int)animation[1]; i++) {
        u32 value;
        if (fn_8047D784(stream, entry + 1, 0x20) == 0) return NULL;
        if (fn_8047D868(stream, &value, 4) == 0) return NULL;
        entry[0] = (u32)(frames + (value / 0x24) * 0x24);
        entry += 9;
    }
    return animation;
}

/* Keyframe stream write. NonMatching draft. */
u32 fn_804466DC(const u32 *animation, void *stream) {
    int count;
    u32 *entry;
    int i;
    u8 *frames;
    frames = (u8 *)animation[4];
    entry = (u32 *)frames;
    count = animation[1];
    for (i = 0; i < count; i++) {
        u32 value;
        if (fn_8047D350(stream, entry + 1, 0x20) == 0) return 0;
        value = entry[0] - (u32)frames;
        if (fn_8047D458(stream, &value, 4) == 0) return 0;
        entry += 9;
    }
    return 1;
}

/* Keyframe stream size. NonMatching draft. */
u32 fn_80446780(u32 *object) {
    return object[1] * 0x24;
}

/* Keyframe multiply by the inverse. NonMatching draft. */
void fn_80446790(float *out, const float *q) {
    float x = q[2];
    float y = q[3];
    float z = q[4];
    float w = q[5];
    float norm = x * x + y * y + z * z + w * w;
    float xNorm = x;
    float yNorm = y;
    float zNorm = z;
    float wNorm = w;
    float oldX;
    float oldY;
    float oldZ;
    float oldW;
    if (norm > 0.0f) {
        float inverse = 1.0f / norm;
        float negative = -inverse;
        wNorm = w * inverse;
        xNorm = x * negative;
        yNorm = y * negative;
        zNorm = z * negative;
    }

    oldX = out[2];
    oldY = out[3];
    oldZ = out[4];
    oldW = out[5];
    out[5] = wNorm * oldW - (zNorm * oldZ + xNorm * oldX + yNorm * oldY);
    out[2] = yNorm * oldZ - zNorm * oldY;
    out[3] = zNorm * oldX - xNorm * oldZ;
    out[4] = xNorm * oldY - yNorm * oldX;
    out[2] += oldX * wNorm;
    out[3] += oldY * wNorm;
    out[4] += oldZ * wNorm;
    out[2] += oldW * xNorm;
    out[3] += oldW * yNorm;
    out[4] += oldW * zNorm;
    out[6] -= q[6];
    out[7] -= q[7];
    out[8] -= q[8];
}

/* Keyframe add. NonMatching draft. */
void fn_804468D0(float *out, const float *a, const float *b) {
    out[5] = a[5] * b[5] - (a[3] * b[3] + a[2] * b[2] + a[4] * b[4]);
    out[2] = a[3] * b[4] - a[4] * b[3];
    out[3] = a[4] * b[2] - a[2] * b[4];
    out[4] = a[2] * b[3] - a[3] * b[2];
    out[2] += b[2] * a[5];
    out[3] += b[3] * a[5];
    out[4] += b[4] * a[5];
    out[2] += a[2] * b[5];
    out[3] += a[3] * b[5];
    out[4] += a[4] * b[5];
    out[6] = a[6] + b[6];
    out[7] = a[7] + b[7];
    out[8] = a[8] + b[8];
}

/* Frame callback: points the hierarchy's nodes whose ID is the frame's at the frame, then recurses
 * into the frame's children. */
void *fn_80446A04(void *frame, R1Hierarchy *hierarchy) {
    int i;
    for (i = 0; i < hierarchy->m_nodeCount; i++) {
        if (hierarchy->m_nodes[i].m_id == fn_8044758C(frame)) {
            hierarchy->m_nodes[i].m_frame = frame;
        }
    }
    fn_8048722C(frame);
    fn_8048752C(frame, (void *)fn_80446A04, hierarchy);
    return frame;
}

void *fn_80446AA8(u32 *object) {
    void *result = fn_8047FC04(0x24, s_freeListBlockSize, 4, s_freeListPreallocBlocks,
                               s_freeListSpace, 0x4011E);
    u32 callbacks[12];
    ((u32 *)lbl_805F1440)[1] = (u32)result;
    if (result == NULL) object = NULL;
    callbacks[0] = 1;
    callbacks[1] = 0x24;
    callbacks[2] = 0x24;
    callbacks[3] = (u32)fn_80445CF4;
    callbacks[4] = (u32)fn_8044621C;
    callbacks[5] = (u32)fn_80445E08;
    callbacks[6] = (u32)fn_804468D0;
    callbacks[7] = (u32)fn_80446790;
    callbacks[8] = (u32)fn_80446620;
    callbacks[9] = (u32)fn_804466DC;
    callbacks[10] = (u32)fn_80446780;
    callbacks[11] = 0;
    fn_80477344(callbacks);
    return object;
}

void *fn_80446B90(u32 *object) {
    if (((u32 *)lbl_805F1440)[1] != 0) {
        fn_8047FCDC((void *)((u32 *)lbl_805F1440)[1]);
        ((u32 *)lbl_805F1440)[1] = 0;
    }
    return object;
}

u32 *fn_80446BE0(u32 *object) {
    u32 *target = (u32 *)((u8 *)object + lbl_805F1440[0]);
    target[1] = 0;
    target[0] = 0xFFFFFFFF;
    return object;
}

/* Plugin 0x11E's frame destructor: detaches the frame from its hierarchy. */
void *fn_80446BFC(void *frame, s32 offset, s32 size) {
    R1FrameHierarchyData *data = (R1FrameHierarchyData *)((u8 *)frame + lbl_805F1440[0]);
    if (data->m_hierarchy != NULL) {
        R1Hierarchy *hierarchy = data->m_hierarchy;
        int i;
        for (i = 0; i < hierarchy->m_nodeCount; i++) {
            hierarchy->m_nodes[i].m_frame = NULL;
        }
        if (hierarchy->m_parentFrame == frame) {
            fn_80447418(hierarchy);
        }
        data->m_hierarchy = NULL;
    }
    data->m_id = -1;
    return frame;
}

/* Plugin 0x11E's frame copy callback: copies the node ID and, for a hierarchy root, the
 * hierarchy. */
void *fn_80446C94(void *dstFrame, const void *srcFrame, s32 offset, s32 size) {
    R1Hierarchy *srcHierarchy;
    R1FrameHierarchyData *dst;
    const R1FrameHierarchyData *src = (const R1FrameHierarchyData *)((const u8 *)srcFrame + lbl_805F1440[0]);
    dst = (R1FrameHierarchyData *)((u8 *)dstFrame + lbl_805F1440[0]);
    srcHierarchy = src->m_hierarchy;
    dst->m_id = src->m_id;
    if (srcHierarchy != NULL && (srcHierarchy->m_flags & 1) == 0) {
        R1Hierarchy *hierarchy = fn_804472AC(srcHierarchy->m_nodeCount, NULL, NULL, srcHierarchy->m_flags,
                                             *(int *)((u8 *)srcHierarchy->m_interpolator + 0x20));
        int i;
        for (i = 0; i < hierarchy->m_nodeCount; i++) {
            hierarchy->m_nodes[i].m_frame = NULL;
            hierarchy->m_nodes[i].m_flags = srcHierarchy->m_nodes[i].m_flags;
            hierarchy->m_nodes[i].m_index = srcHierarchy->m_nodes[i].m_index;
            hierarchy->m_nodes[i].m_id = srcHierarchy->m_nodes[i].m_id;
        }
        dst->m_hierarchy = hierarchy;
        hierarchy->m_parentFrame = dstFrame;
    }
    return dstFrame;
}

/* Plugin 0x11E's frame stream write callback: the frame's node ID and, for the root of a
 * hierarchy, the hierarchy's nodes. */
void *fn_80446D84(void *stream, s32 length, const void *frame, s32 offset, s32 size) {
    R1FrameHierarchyData *data;
    int i;
    R1HierarchyNode *node;
    R1Hierarchy *hierarchy;
    u32 version = 0x100;
    if (fn_8047D458(stream, &version, 4) == 0) {
        return NULL;
    }
    data = (R1FrameHierarchyData *)((const u8 *)frame + lbl_805F1440[0]);
    if (fn_8047D458(stream, &data->m_id, 4) == 0) {
        return NULL;
    }
    hierarchy = data->m_hierarchy;
    if (hierarchy != NULL && (hierarchy->m_flags & 1) == 0) {
        if (fn_8047D458(stream, &hierarchy->m_nodeCount, 4) == 0) {
            return NULL;
        }
        if (fn_8047D458(stream, &hierarchy->m_flags, 4) == 0) {
            return NULL;
        }
        if (fn_8047D458(stream, (u8 *)hierarchy->m_interpolator + 0x20, 4) == 0) {
            return NULL;
        }
        node = hierarchy->m_nodes;
        for (i = 0; i < hierarchy->m_nodeCount; i++) {
            if (fn_8047D458(stream, &node->m_id, 4) == 0) {
                return NULL;
            }
            if (fn_8047D458(stream, &node->m_index, 4) == 0) {
                return NULL;
            }
            if (fn_8047D458(stream, &node->m_flags, 4) == 0) {
                return NULL;
            }
            node++;
        }
    } else {
        u32 none = 0;
        if (fn_8047D458(stream, &none, 4) == 0) {
            return NULL;
        }
    }
    return stream;
}

/* Plugin 0x11E's frame stream read callback. */
void *fn_80446F40(void *stream, s32 length, void *frame, s32 offset, s32 size) {
    int nodeCount;
    s32 version;
    int flags;
    int keyFrameSize;
    R1FrameHierarchyData *data = (R1FrameHierarchyData *)((u8 *)frame + lbl_805F1440[0]);
    if (fn_8047D868(stream, &version, 4) == 0) {
        return NULL;
    }
    if (version != 0x100) {
        return NULL;
    }
    if (fn_8047D868(stream, &data->m_id, 4) == 0) {
        return NULL;
    }
    if (fn_8047D868(stream, &nodeCount, 4) == 0) {
        return NULL;
    }
    if (nodeCount > 0) {
        R1Hierarchy *hierarchy;
        if (fn_8047D868(stream, &flags, 4) == 0) {
            return NULL;
        }
        if (fn_8047D868(stream, &keyFrameSize, 4) == 0) {
            return NULL;
        }
        hierarchy =
            ((R1Hierarchy *(*)(u32, u32))*(u32 *)(lbl_805F15B4 + 0x118))(lbl_805F1440[1], 0x3011E);
        memset(hierarchy, 0, sizeof(R1Hierarchy));
        if (hierarchy != NULL) {
            int i;
            R1HierarchyNode *node;
            hierarchy->m_interpolator = fn_804779DC(nodeCount, keyFrameSize);
            hierarchy->m_flags = flags;
            hierarchy->m_parentFrame = frame;
            hierarchy->m_nodeCount = nodeCount;
            hierarchy->m_parentHierarchy = hierarchy;
            if ((hierarchy->m_flags & 2) != 0) {
                hierarchy->m_matrices = NULL;
                hierarchy->m_matricesUnaligned = NULL;
            } else {
                void *memory =
                    ((void *(*)(u32, u32))*(u32 *)(lbl_805F15B4 + 0x108))((nodeCount << 6) + 0xF, 0x3011E);
                hierarchy->m_matrices = (void *)(((u32)memory + 0xF) & ~0xF);
                hierarchy->m_matricesUnaligned = memory;
            }
            hierarchy->m_nodes =
                (R1HierarchyNode *)((void *(*)(u32, u32))*(u32 *)(lbl_805F15B4 + 0x108))(nodeCount << 4, 0x3011E);
            node = hierarchy->m_nodes;
            for (i = 0; i < hierarchy->m_nodeCount; i++) {
                if (fn_8047D868(stream, &node->m_id, 4) == 0) {
                    return NULL;
                }
                if (fn_8047D868(stream, &node->m_index, 4) == 0) {
                    return NULL;
                }
                if (fn_8047D868(stream, &node->m_flags, 4) == 0) {
                    return NULL;
                }
                node->m_frame = NULL;
                node++;
            }
        }
        data->m_hierarchy = hierarchy;
    }
    return stream;
}

/* Plugin 0x11E's frame stream size callback. */
s32 fn_8044719C(const void *frame, s32 offset, s32 size) {
    const R1FrameHierarchyData *data = (const R1FrameHierarchyData *)((const u8 *)frame + lbl_805F1440[0]);
    int used = 0;
    if (data->m_id != -1 || data->m_hierarchy != NULL) {
        used = 1;
    }
    if (used) {
        s32 result = 0xC;
        R1Hierarchy *hierarchy = data->m_hierarchy;
        if (hierarchy != NULL && (hierarchy->m_flags & 1) == 0) {
            result = hierarchy->m_nodeCount * 0xC + 0x14;
        }
        return result;
    }
    return 0;
}

u32 fn_80447200(void) {
    s32 result = fn_80485D78(0, 0x11E, (R1Callback)fn_80446AA8, (R1Callback)fn_80446B90);
    s32 status;
    int success;
    if (result < 0) return 0;
    result = fn_804878D4(8, 0x11E, (R1Callback)fn_80446BE0, (R1Callback)fn_80446BFC,
                         (R1Callback)fn_80446C94);
    lbl_805F1440[0] = result;
    status = fn_804836EC(0x11E, (R1Callback)fn_80446F40, (R1Callback)fn_80446D84,
                         (R1Callback)fn_8044719C);
    success = 0;
    if (status >= 0 && lbl_805F1440[0] >= 0) {
        success = 1;
    }
    return success;
}

/* Creates a hierarchy of `count` nodes from the hierarchy free list. */
R1Hierarchy *fn_804472AC(int count, int *flagsTable, int *ids, int flags, int keyFrameSize) {
    R1Hierarchy *hierarchy =
        ((R1Hierarchy *(*)(u32, u32))*(u32 *)(lbl_805F15B4 + 0x118))(lbl_805F1440[1], 0x3011E);
    int i;
    hierarchy->m_interpolator = fn_804779DC(count, keyFrameSize);
    hierarchy->m_flags = flags;
    hierarchy->m_nodeCount = count;
    hierarchy->m_parentFrame = NULL;
    if ((flags & 2) == 0) {
        void *memory = ((void *(*)(u32, u32))*(u32 *)(lbl_805F15B4 + 0x108))((count << 6) + 0xF, 0x3011E);
        hierarchy->m_matrices = (void *)(((u32)memory + 0xF) & ~0xF);
        hierarchy->m_matricesUnaligned = memory;
    } else {
        hierarchy->m_matrices = NULL;
        hierarchy->m_matricesUnaligned = NULL;
    }
    hierarchy->m_nodes =
        (R1HierarchyNode *)((void *(*)(u32, u32))*(u32 *)(lbl_805F15B4 + 0x108))(count << 4, 0x3011E);
    for (i = 0; i < count; i++) {
        hierarchy->m_nodes[i].m_frame = NULL;
        if (ids != NULL) {
            hierarchy->m_nodes[i].m_id = ids[i];
        }
        hierarchy->m_nodes[i].m_index = i;
        if (flagsTable != NULL) {
            hierarchy->m_nodes[i].m_flags = flagsTable[i];
        }
    }
    hierarchy->m_parentHierarchy = hierarchy;
    return hierarchy;
}

/* Destroys a hierarchy: its node and matrix arrays unless it shares them, its interpolator, the
 * hierarchy itself (back to the free list), and the parent frame's link to it. */
int fn_80447418(R1Hierarchy *hierarchy) {
    void *parentFrame = hierarchy->m_parentFrame;
    if ((hierarchy->m_flags & 1) == 0) {
        if ((hierarchy->m_flags & 2) == 0) {
            ((void (*)(void *))*(u32 *)(lbl_805F15B4 + 0x10C))(hierarchy->m_matricesUnaligned);
        }
        ((void (*)(void *))*(u32 *)(lbl_805F15B4 + 0x10C))(hierarchy->m_nodes);
    }
    hierarchy->m_matricesUnaligned = NULL;
    hierarchy->m_matrices = NULL;
    hierarchy->m_nodes = NULL;
    fn_80477A8C(hierarchy->m_interpolator);
    ((void (*)(u32, void *))*(u32 *)(lbl_805F15B4 + 0x11C))(lbl_805F1440[1], hierarchy);
    if (parentFrame != NULL) {
        ((R1FrameHierarchyData *)((u8 *)parentFrame + lbl_805F1440[0]))->m_hierarchy = NULL;
    }
    return 0;
}

static inline void AttachHierarchy(R1Hierarchy *hierarchy) {
    void *frame;
    int i;
    frame = hierarchy->m_parentFrame;
    for (i = 0; i < hierarchy->m_nodeCount; i++) {
        if (hierarchy->m_nodes[i].m_id == fn_8044758C(frame)) {
            hierarchy->m_nodes[i].m_frame = frame;
        }
    }
    fn_8048722C(frame);
    fn_8048752C(frame, (void *)fn_80446A04, hierarchy);
}

/* Attaches the hierarchy to its parent frame's tree: points every node at the frame with its ID. */
R1Hierarchy *fn_804474D8(R1Hierarchy *hierarchy) {
    AttachHierarchy(hierarchy);
    return hierarchy;
}

u32 fn_8044757C(u8 *object) {
    return *(u32 *)(object + lbl_805F1440[0] + 4);
}

/* The frame's node ID (the first word of plugin 0x11E's frame data). */
int fn_8044758C(void *frame) {
    return *(int *)((u8 *)frame + lbl_805F1440[0]);
}
