/* RenderWare: the end of the 2D toolkit (rt2d) path module (stream read, the stroke renderer),
 * the open and close functions of two rt2d modules, then from fn_80477344 on the animation
 * toolkit RtAnim (interpolation schemes, animations, interpolators; continued in RwCpp40).
 * Written from the retail code; the fn_ symbols are kept because game code calls them by
 * address, and each function says which public RenderWare API it is. Built as C: -O4,p, no
 * stmw/lmw, automatic inlining in definition order. */
#include "Game/rw/r5.h"


#ifdef __cplusplus
extern "C" {
#endif

void *memcpy(void *dst, const void *src, u32 n);
RwMatrix *fn_804749CC(void);                     /* the current 2D-to-world matrix */
Rt2dPath *fn_8047548C(void);                     /* empties the scratch path */
Rt2dPath *fn_804758C8(Rt2dPath *dst, const Rt2dPath *src); /* flattens a path */
RwBool fn_80475E2C(Rt2dPath *path);              /* Rt2dPathDestroy */
void fn_804760C8(Rt2dPath *path);                /* Rt2dPathEmpty */
Rt2dPath *fn_80476490(void);                     /* Rt2dPathCreate */
void *fn_8047FC04(s32 entrySize, s32 entriesPerBlock, s32 alignment, s32 preallocBlocks, void *inPlace,
                  u32 hint);                     /* RwFreeListCreateAndPreallocateSpace */
RwBool fn_8047FCDC(void *freeList);              /* RwFreeListDestroy */

extern s32 lbl_805EDE58; /* brushes per free list block */
extern s32 lbl_805EDE5C; /* brush blocks to preallocate */
extern s32 lbl_805EDE60;
extern s32 lbl_805EDE64;
extern s32 lbl_805EDE68;
extern s32 lbl_805EDE6C;
extern u8 lbl_805E26D0[0x28]; /* the brush free list's own storage */
extern u8 lbl_805E26F8[0x24];
extern u8 lbl_805E271C[0x24];
extern const f32 lbl_805FB278; /* 1.0f */
extern const f32 lbl_805FB280; /* lbl_805FB280 */
extern const f32 lbl_805FB284; /* lbl_805FB284 */

Rt2dPath *fn_80476C4C(RwStream *stream);

/* Reads the nodes of a path whose header has been read, and the sub-paths chained to it. Retail
 * does not inline it into its caller below, where GC/2.7 would (as with Rt2dPathClose). */
#pragma auto_inline off
Rt2dPath *fn_80476ABC(Rt2dPath *path, Rt2dPathStreamHeader *header, RwStream *stream) {
    Rt2dPathNode *nodes;
    s32 i;

    fn_804760C8(path);
    path->curr = path;
    path->numSegment = header->numSegment;
    path->closed = header->flag & 1;
    path->flat = (header->flag >> 1) & 1;
    path->inset = header->inset;
    nodes = (Rt2dPathNode *)RWSRCGLOBAL(rwmalloc)(path->numSegment * sizeof(Rt2dPathNode), 0x301A1);
    if (nodes == 0) {
        return 0;
    }
    if (path->numSegment != 0) {
        if (fn_80480D4C(stream, nodes, path->numSegment * sizeof(Rt2dPathNode)) !=
            path->numSegment * sizeof(Rt2dPathNode)) {
            return 0;
        }
        fn_8047D158(nodes, path->numSegment * sizeof(Rt2dPathNode));
    }
    for (i = 0; i < header->numSegment; i++) {
    }
    path->segments = (RwSList *)nodes;
    if (header->flag & 4) {
        if (fn_8047CF50(stream, 0x1A1, 0, 0)) {
            path->next = fn_80476C4C(stream);
        } else {
            return 0;
        }
    }
    {
        Rt2dPath *sub;

        for (sub = path->next; sub != 0; sub = sub->next) {
            sub->curr = path;
        }
    }
    return path;
}
#pragma auto_inline reset

/* Rt2dPathStreamRead: a new path read from a stream. */
Rt2dPath *fn_80476C4C(RwStream *stream) {
    Rt2dPath *result;
    Rt2dPath *path;
    Rt2dPathStreamHeader header;

    path = fn_80476490();
    result = path;
    if (fn_80480D4C(stream, &header, sizeof(header)) != sizeof(header)) {
        result = 0;
    } else {
        fn_8047D254(&header, sizeof(header));
        switch (header.type) {
        case 1:
            result = fn_80476ABC(path, &header, stream);
            break;
        }
    }
    if (result == 0) {
        fn_80475E2C(path);
    }
    return result;
}

/* Strokes a path with a brush: a triangle strip along both sides of the flattened path, shaded
 * and textured along its length when the brush says so; then the sub-paths chained to it.
 * NonMatching (same instructions; ~85 rows of float and scratch register numbering): retail has
 * the half width in f29 and the inset in f30, the shading factor in f7, and loads the flat
 * colour's four bytes before storing them. 3500 sampled declaration orders and four spellings of
 * the colour copies were tried. */
Rt2dPath *fn_80476CF8(Rt2dPath *path, Rt2dBrush *brush) {
    if (path != 0) {
        f32 outer;
        f32 inner;
        RwIm3DVertex *vertex;
        s32 count;
        f32 inset;
        f32 scale;
        Rt2dPathNode *node;
        s32 i;
        const Rt2dPath *flat;
        s32 numVertex;
        f32 z;
        RwMatrix *ltm;
        f32 halfwidth;

        ltm = fn_804749CC();
        halfwidth = brush->halfwidth;
        z = lbl_805E1950.layerDepth;
        inset = path->inset;
        flat = path;
        if (!path->flat) {
            flat = fn_8047548C();
            fn_804758C8((Rt2dPath *)flat, path);
        }
        if (flat->numSegment == -1) {
            node = (Rt2dPathNode *)fn_8047DFAC(flat->segments);
        } else {
            node = (Rt2dPathNode *)flat->segments;
        }
        count = flat->numSegment;
        if (count == -1) {
            count = fn_8047E03C(flat->segments);
        }
        if (brush->texture != 0) {
            RwTexture *texture = brush->texture;
            u32 address;
            u32 filter;

            filter = texture->filterAddressing & 0xFF;
            address = 0;
            if (((texture->filterAddressing >> 8) & 0xF) == ((texture->filterAddressing >> 12) & 0xF)) {
                address = (texture->filterAddressing >> 12) & 0xF;
            }
            RWSRCGLOBAL(renderStateSet)(1, texture->raster);
            RWSRCGLOBAL(renderStateSet)(9, (void *)filter);
            RWSRCGLOBAL(renderStateSet)(2, (void *)address);
        } else {
            RWSRCGLOBAL(renderStateSet)(1, 0);
        }
        outer = halfwidth + inset;
        inner = halfwidth - inset;
        scale = lbl_805FB278 / node[count - 1].dist;
        vertex = lbl_805E1950.vertexBuffer;
        numVertex = 0;
        for (i = 0; i < count; i++, node++) {
            if (numVertex >= 62) {
                if (fn_8049A82C(lbl_805E1950.vertexBuffer, numVertex, ltm,
                                lbl_805E1950.field_0x31C | (brush->texture != 0))) {
                    fn_8049A95C(4, lbl_805E1950.index, numVertex);
                    fn_8049A90C();
                }
                vertex = lbl_805E1950.vertexBuffer;
                numVertex = 0;
                i--;
                node--;
            }
            {
                RwV2d offset;
                RwV2d pos;

                offset.x = node->normal.x * outer;
                offset.y = node->normal.y * outer;
                pos.x = node->pos.x - offset.x;
                pos.y = node->pos.y - offset.y;
                vertex[0].objVertex.x = pos.x;
                vertex[0].objVertex.y = pos.y;
                vertex[0].objVertex.z = z;
            }
            if (brush->flag & 1) {
                f32 t = node->dist * scale;
                RwRGBAReal delta;
                RwRGBAReal col;

                delta.red = brush->dtop.col.red * t;
                delta.green = brush->dtop.col.green * t;
                delta.blue = brush->dtop.col.blue * t;
                delta.alpha = brush->dtop.col.alpha * t;
                col.red = brush->top.col.red + delta.red;
                col.green = brush->top.col.green + delta.green;
                col.blue = brush->top.col.blue + delta.blue;
                col.alpha = brush->top.col.alpha + delta.alpha;
                vertex[0].color.red = col.red;
                vertex[0].color.green = col.green;
                vertex[0].color.blue = col.blue;
                vertex[0].color.alpha = col.alpha;
            } else {
                vertex[0].color.red = brush->colorCache.red;
                vertex[0].color.green = brush->colorCache.green;
                vertex[0].color.blue = brush->colorCache.blue;
                vertex[0].color.alpha = brush->colorCache.alpha;
            }
            if (brush->flag & 2) {
                f32 t = node->dist * scale;
                RwV2d delta;
                RwV2d uv;

                delta.x = brush->dtop.uv.x * t;
                delta.y = brush->dtop.uv.y * t;
                uv.x = brush->top.uv.x + delta.x;
                uv.y = brush->top.uv.y + delta.y;
                vertex[0].u = uv.x;
                vertex[0].v = uv.y;
            }
            {
                RwV2d offset;
                RwV2d pos;

                offset.x = node->normal.x * inner;
                offset.y = node->normal.y * inner;
                pos.x = node->pos.x + offset.x;
                pos.y = node->pos.y + offset.y;
                vertex[1].objVertex.x = pos.x;
                vertex[1].objVertex.y = pos.y;
                vertex[1].objVertex.z = z;
            }
            if (brush->flag & 1) {
                f32 t = node->dist * scale;
                RwRGBAReal delta;
                RwRGBAReal col;

                delta.red = brush->dbottom.col.red * t;
                delta.green = brush->dbottom.col.green * t;
                delta.blue = brush->dbottom.col.blue * t;
                delta.alpha = brush->dbottom.col.alpha * t;
                col.red = brush->bottom.col.red + delta.red;
                col.green = brush->bottom.col.green + delta.green;
                col.blue = brush->bottom.col.blue + delta.blue;
                col.alpha = brush->bottom.col.alpha + delta.alpha;
                vertex[1].color.red = col.red;
                vertex[1].color.green = col.green;
                vertex[1].color.blue = col.blue;
                vertex[1].color.alpha = col.alpha;
            } else {
                vertex[1].color.red = brush->colorCache.red;
                vertex[1].color.green = brush->colorCache.green;
                vertex[1].color.blue = brush->colorCache.blue;
                vertex[1].color.alpha = brush->colorCache.alpha;
            }
            if (brush->flag & 2) {
                f32 t = node->dist * scale;
                RwV2d delta;
                RwV2d uv;

                delta.x = brush->dbottom.uv.x * t;
                delta.y = brush->dbottom.uv.y * t;
                uv.x = brush->bottom.uv.x + delta.x;
                uv.y = brush->bottom.uv.y + delta.y;
                vertex[1].u = uv.x;
                vertex[1].v = uv.y;
            }
            vertex += 2;
            numVertex += 2;
        }
        if (numVertex > 0) {
            if (fn_8049A82C(lbl_805E1950.vertexBuffer, numVertex, ltm,
                            lbl_805E1950.field_0x31C | (brush->texture != 0))) {
                fn_8049A95C(4, lbl_805E1950.index, numVertex);
                fn_8049A90C();
            }
        }
        fn_80476CF8(path->next, brush);
    }
    return path;
}

/* Rt2dPathStroke. */
Rt2dPath *fn_804771C0(Rt2dPath *path, Rt2dBrush *brush) {
    return fn_80476CF8(path, brush);
}

/* Closes the brush module. */
void fn_804771E0(void) {
    fn_8047FCDC(lbl_805E1950.brushFreeList);
    lbl_805E1950.brushFreeList = 0;
}

/* Opens the brush module: the free list of brushes. */
RwBool fn_8047721C(void) {
    lbl_805E1950.brushFreeList =
        fn_8047FC04(0x100, lbl_805EDE58, 16, lbl_805EDE5C, lbl_805E26D0, 0x401A3);
    return 1;
}

/* Closes a module with two free lists (8- and 0x14-byte entries). */
void fn_8047726C(void) {
    fn_8047FCDC(lbl_805E1950.freeList174);
    fn_8047FCDC(lbl_805E1950.freeList178);
    lbl_805E1950.freeList174 = 0;
    lbl_805E1950.freeList178 = 0;
}

/* Opens it. */
RwBool fn_804772C4(void) {
    lbl_805E1950.freeList174 = fn_8047FC04(8, lbl_805EDE68, 4, lbl_805EDE6C, lbl_805E26F8, 0x40190);
    lbl_805E1950.freeList178 = fn_8047FC04(0x14, lbl_805EDE60, 4, lbl_805EDE64, lbl_805E271C, 0x40190);
    return 1;
}

/* RtAnim. The interpolation schemes registered so far. */
extern s32 lbl_805F1540;                     /* their number */
extern RtAnimInterpolatorInfo lbl_805E2740[16];

/* RtAnimGetInterpolatorInfo (inlined everywhere it is used). */
static RtAnimInterpolatorInfo *GetInterpolatorInfo(s32 typeID) {
    s32 i;

    for (i = 0; i < lbl_805F1540; i++) {
        if (typeID == lbl_805E2740[i].typeID) {
            return &lbl_805E2740[i];
        }
    }
    R5ERROR(0x1B7, (2));
    return 0;
}

/* RtAnimRegisterInterpolationScheme. */
RwBool fn_80477344(RtAnimInterpolatorInfo *info) {
    s32 n = lbl_805F1540;

    if (n < 16) {
        s32 i;

        for (i = 0; i < n; i++) {
            if (info->typeID == lbl_805E2740[i].typeID) {
                R5ERROR(0x1B7, (0));
                return 0;
            }
        }
        lbl_805E2740[n] = *info;
        lbl_805F1540++;
        return 1;
    } else {
        R5ERROR(0x1B7, (1));
        return 0;
    }
}

/* RtAnimAnimationCreate. */
RtAnimAnimation *fn_80477468(s32 typeID, s32 numFrames, s32 flags, f32 duration) {
    RtAnimInterpolatorInfo *info = GetInterpolatorInfo(typeID);
    RtAnimAnimation *anim;

    if (info == 0) {
        return 0;
    }
    anim = (RtAnimAnimation *)RWSRCGLOBAL(rwmalloc)(
        sizeof(RtAnimAnimation) + numFrames * info->animKeyFrameSize + info->customDataSize, 0x3001B);
    anim->numFrames = numFrames;
    anim->duration = duration;
    anim->flags = flags;
    anim->interpInfo = info;
    anim->pFrames = anim + 1;
    if (info->customDataSize > 0) {
        anim->customData = (u8 *)anim->pFrames + numFrames * info->animKeyFrameSize;
    } else {
        anim->customData = 0;
    }
    return anim;
}

/* RtAnimAnimationDestroy. */
RwBool fn_8047759C(RtAnimAnimation *anim) {
    RWSRCGLOBAL(rwfree)(anim);
    return 1;
}

/* RtAnimAnimationStreamRead. */
RtAnimAnimation *fn_804775CC(RwStream *stream) {
    s32 version;
    s32 typeID;
    s32 numFrames;
    s32 flags;
    f32 duration;
    RtAnimInterpolatorInfo *info;
    RtAnimAnimation *anim;

    if (fn_8047D868(stream, &version, 4) == 0) {
        return 0;
    }
    if (version != 0x100) {
        return 0;
    }
    if (fn_8047D868(stream, &typeID, 4) == 0) {
        return 0;
    }
    info = GetInterpolatorInfo(typeID);
    if (info == 0) {
        return 0;
    }
    if (fn_8047D868(stream, &numFrames, 4) == 0) {
        return 0;
    }
    if (fn_8047D868(stream, &flags, 4) == 0) {
        return 0;
    }
    if (fn_8047D784(stream, &duration, 4) == 0) {
        return 0;
    }
    anim = fn_80477468(typeID, numFrames, flags, duration);
    if (anim == 0) {
        return 0;
    }
    info->keyFrameStreamReadCB(stream, anim);
    return anim;
}

s32 fn_80477978(const RtAnimAnimation *anim);

/* RtAnimAnimationStreamWrite. */
RwBool fn_80477860(const RtAnimAnimation *anim, RwStream *stream) {
    s32 version = 0x100;

    fn_8047CEF0(stream, 0x1B, fn_80477978(anim), 0x37002, 0x37);
    if (fn_8047D458(stream, &version, 4) == 0) {
        return 0;
    }
    if (fn_8047D458(stream, &anim->interpInfo->typeID, 4) == 0) {
        return 0;
    }
    if (fn_8047D458(stream, &anim->numFrames, 4) == 0) {
        return 0;
    }
    if (fn_8047D458(stream, &anim->flags, 4) == 0) {
        return 0;
    }
    if (fn_8047D350(stream, &anim->duration, 4) == 0) {
        return 0;
    }
    anim->interpInfo->keyFrameStreamWriteCB(anim, stream);
    return 1;
}

/* RtAnimAnimationStreamGetSize. */
s32 fn_80477978(const RtAnimAnimation *anim) {
    return anim->interpInfo->keyFrameStreamGetSizeCB(anim) + 0x14;
}

/* RtAnimAnimationGetNumNodes: the key frames before the first one that follows frame 0. */
s32 fn_804779A8(const RtAnimAnimation *anim) {
    s32 numNodes = 0;
    RtAnimKeyFrameHeader *frame = (RtAnimKeyFrameHeader *)anim->pFrames;
    s32 size = anim->interpInfo->animKeyFrameSize;

    while (frame->prevFrame != anim->pFrames) {
        frame = (RtAnimKeyFrameHeader *)((u8 *)frame + size);
        numNodes++;
    }
    return numNodes;
}

/* RtAnimInterpolatorCreate. */
RtAnimInterpolator *fn_804779DC(s32 numNodes, s32 maxKeyFrameSize) {
    RtAnimInterpolator *interp = (RtAnimInterpolator *)RWSRCGLOBAL(rwmalloc)(
        numNodes * maxKeyFrameSize + sizeof(RtAnimInterpolator), 0x3001B);

    interp->numNodes = numNodes;
    interp->pCurrentAnim = 0;
    interp->pNextFrame = 0;
    interp->currentTime = lbl_805FB280;
    interp->pAnimCallBack = 0;
    interp->animCallBackTime = lbl_805FB284;
    interp->pAnimCallBackData = 0;
    interp->pAnimLoopCallBack = 0;
    interp->pAnimLoopCallBackData = 0;
    interp->currentInterpKeyFrameSize = maxKeyFrameSize;
    interp->currentAnimKeyFrameSize = -1;
    interp->maxInterpKeyFrameSize = maxKeyFrameSize;
    interp->isSubInterpolator = 0;
    interp->offsetInParent = 0;
    interp->parentAnimation = interp;
    interp->keyFrameApplyCB = 0;
    interp->keyFrameInterpolateCB = 0;
    interp->keyFrameBlendCB = 0;
    interp->keyFrameAddCB = 0;
    return interp;
}

/* RtAnimInterpolatorDestroy. */
void fn_80477A8C(RtAnimInterpolator *interp) {
    RWSRCGLOBAL(rwfree)(interp);
}

/* RtAnimInterpolatorSetCurrentAnim: starts `anim` at time 0, between its first two key frames of
 * every node. */
RwBool fn_80477AB8(RtAnimInterpolator *interp, RtAnimAnimation *anim) {
    RtAnimInterpolatorInfo *info;
    s32 i;

    interp->pCurrentAnim = anim;
    interp->currentTime = lbl_805FB280;
    info = anim->interpInfo;
    interp->currentInterpKeyFrameSize = info->interpKeyFrameSize;
    interp->currentAnimKeyFrameSize = info->animKeyFrameSize;
    interp->keyFrameApplyCB = info->keyFrameApplyCB;
    interp->keyFrameBlendCB = info->keyFrameBlendCB;
    interp->keyFrameInterpolateCB = info->keyFrameInterpolateCB;
    interp->keyFrameAddCB = info->keyFrameAddCB;
    for (i = 0; i < interp->numNodes; i++) {
        interp->keyFrameInterpolateCB(R5ANIMFRAME(interp, i),
                                      (u8 *)anim->pFrames + i * interp->currentAnimKeyFrameSize,
                                      (u8 *)anim->pFrames + (i + interp->numNodes) * interp->currentAnimKeyFrameSize,
                                      lbl_805FB280, anim->customData);
    }
    {
        u8 *frame1;
        u8 *frame2;
        RtAnimInterpFrameHeader *frame;

        frame = R5ANIMFRAME(interp, 0);
        frame1 = (u8 *)anim->pFrames;
        frame2 = (u8 *)anim->pFrames + interp->numNodes * interp->currentAnimKeyFrameSize;

        for (i = 0; i < interp->numNodes; i++) {
            frame->keyFrame1 = (RtAnimKeyFrameHeader *)frame1;
            frame->keyFrame2 = (RtAnimKeyFrameHeader *)frame2;
            frame1 += interp->currentAnimKeyFrameSize;
            frame2 += interp->currentAnimKeyFrameSize;
            frame = (RtAnimInterpFrameHeader *)((u8 *)frame + interp->currentInterpKeyFrameSize);
        }
    }
    interp->pNextFrame = (u8 *)anim->pFrames + interp->numNodes * interp->currentAnimKeyFrameSize * 2;
    return 1;
}

/* RtAnimInterpolatorCopy: the interpolated frames of `in` into `out`. */
RwBool fn_80477BF0(RtAnimInterpolator *out, RtAnimInterpolator *in) {
    memcpy(R5ANIMFRAME(out, 0), R5ANIMFRAME(in, 0), out->currentInterpKeyFrameSize * out->numNodes);
    return 1;
}

RwBool fn_80477ED4(RtAnimInterpolator *interp, f32 time);

/* RtAnimInterpolatorSubAnimTime: steps the animation back by `time`. */
RwBool fn_80477C2C(RtAnimInterpolator *interp, f32 time) {
    RtAnimAnimation *anim;
    RwBool callBack;
    RwBool looped = 0;

    callBack = interp->currentTime > interp->animCallBackTime &&
               interp->currentTime - time <= interp->animCallBackTime;
    interp->currentTime -= time;
    anim = interp->pCurrentAnim;
    if (interp->currentTime < lbl_805FB280) {
        f32 duration = anim->duration;
        f32 current;

        while (interp->currentTime < lbl_805FB280) {
            interp->currentTime += duration;
        }
        current = interp->currentTime;
        fn_80477AB8(interp, anim);
        fn_80477ED4(interp, current);
        callBack |= interp->currentTime < interp->animCallBackTime;
        looped = 1;
    } else {
        s32 n = interp->numNodes;
        RtAnimInterpFrameHeader *frame = R5ANIMFRAME(interp, 0);

        while (n--) {
            while (interp->currentTime < frame->keyFrame1->time) {
                frame->keyFrame2 = frame->keyFrame1;
                frame->keyFrame1 = frame->keyFrame1->prevFrame;
            }
            interp->keyFrameInterpolateCB(frame, frame->keyFrame1, frame->keyFrame2, interp->currentTime,
                                          interp->pCurrentAnim->customData);
            frame = (RtAnimInterpFrameHeader *)((u8 *)frame + interp->currentInterpKeyFrameSize);
        }
        interp->pNextFrame = 0;
    }
    if (callBack && interp->pAnimCallBack != 0) {
        if (interp->pAnimCallBack(interp, interp->pAnimCallBackData) == 0) {
            interp->pAnimCallBack = 0;
        }
    }
    if (looped && interp->pAnimLoopCallBack != 0) {
        if (interp->pAnimLoopCallBack(interp, interp->pAnimLoopCallBackData) == 0) {
            interp->pAnimLoopCallBack = 0;
        }
    }
    return 1;
}

/* RtAnimInterpolatorAddAnimTime: steps the animation on by `time`, looping at its end. */
RwBool fn_80477ED4(RtAnimInterpolator *interp, f32 time) {
    if (time > lbl_805FB280) {
        RtAnimAnimation *anim;
        s32 numNodes;
        RwBool callBack;
        RwBool looped = 0;
        f32 duration;

        anim = interp->pCurrentAnim;
        numNodes = interp->numNodes;
        duration = anim->duration;
        callBack = interp->currentTime < interp->animCallBackTime &&
                   interp->currentTime + time >= interp->animCallBackTime;
        interp->currentTime += time;
        if (interp->currentTime > duration) {
            f32 current;

            while ((current = interp->currentTime) > duration) {
                interp->currentTime -= duration;
                callBack |= interp->currentTime >= interp->animCallBackTime;
                looped = 1;
            }
            fn_80477AB8(interp, anim);
            interp->currentTime = current;
        }
        if (interp->pNextFrame == 0) {
            f32 current = interp->currentTime;

            fn_80477AB8(interp, anim);
            interp->currentTime = current;
        }
        {
            s32 j;
            RtAnimInterpFrameHeader *frame;
            u8 *end;
            RtAnimKeyFrameHeader *prev;

            j = 0;
            end = (u8 *)anim->pFrames + anim->numFrames * interp->currentAnimKeyFrameSize;
            while ((u8 *)interp->pNextFrame < end) {
                prev = ((RtAnimKeyFrameHeader *)interp->pNextFrame)->prevFrame;
                if (prev->time > interp->currentTime) {
                    break;
                }
                do {
                    frame = R5ANIMFRAME(interp, j);
                    j++;
                    if (j >= numNodes) {
                        j = 0;
                    }
                } while (frame->keyFrame2 != prev);
                frame->keyFrame1 = frame->keyFrame2;
                frame->keyFrame2 = (RtAnimKeyFrameHeader *)interp->pNextFrame;
                interp->pNextFrame = (u8 *)interp->pNextFrame + interp->currentAnimKeyFrameSize;
            }
        }
        {
            s32 i;
            RtAnimInterpFrameHeader *frame;

            frame = R5ANIMFRAME(interp, 0);
            for (i = 0; i < numNodes; i++) {
                interp->keyFrameInterpolateCB(frame, frame->keyFrame1, frame->keyFrame2, interp->currentTime,
                                              interp->pCurrentAnim->customData);
                frame = (RtAnimInterpFrameHeader *)((u8 *)frame + interp->currentInterpKeyFrameSize);
            }
        }
        if (callBack && interp->pAnimCallBack != 0) {
            if (interp->pAnimCallBack(interp, interp->pAnimCallBackData) == 0) {
                interp->pAnimCallBack = 0;
            }
        }
        if (looped && interp->pAnimLoopCallBack != 0) {
            if (interp->pAnimLoopCallBack(interp, interp->pAnimLoopCallBackData) == 0) {
                interp->pAnimLoopCallBack = 0;
            }
        }
    }
    return 1;
}

/* RtAnimInterpolatorSetCurrentTime. */
RwBool fn_804782E8(RtAnimInterpolator *interp, f32 time) {
    f32 delta = time - interp->currentTime;

    if (delta < lbl_805FB280) {
        fn_80477C2C(interp, -delta);
    } else {
        fn_80477ED4(interp, delta);
    }
    return 1;
}

#ifdef __cplusplus
}
#endif
