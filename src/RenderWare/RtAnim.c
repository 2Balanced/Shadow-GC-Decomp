/* RenderWare animation toolkit (RtAnim), 0x80477344..0x8047832C: interpolation schemes,
 * animations (create, destroy, stream read and write) and interpolators (create, destroy, set
 * the animation, add and subtract time).
 *
 * One source file: its .bss is the table of registered schemes (16 of 0x30 bytes, 0x805E2740),
 * its .sbss their count (0x805F1540) and its .sdata2 the pool 0.0f, -1.0f (0x805FB280..0x805FB288).
 *
 * Written from the retail code; the fn_ symbols are kept because game code calls them by address,
 * and each function says which public RenderWare API it is. C, built with cflags_rw_r5 (-O4,p, no
 * stmw/lmw, automatic inlining in definition order). */
#include "Game/rw/r5.h"

void *memcpy(void *dst, const void *src, u32 n);

/* The interpolation schemes registered so far. */
RtAnimInterpolatorInfo lbl_805E2740[16];
s32 lbl_805F1540; /* their number */

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
    interp->currentTime = 0.0f;
    interp->pAnimCallBack = 0;
    interp->animCallBackTime = -1.0f;
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
    interp->currentTime = 0.0f;
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
                                      0.0f, anim->customData);
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
    if (interp->currentTime < 0.0f) {
        f32 duration = anim->duration;
        f32 current;

        while (interp->currentTime < 0.0f) {
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
    if (time > 0.0f) {
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

    if (delta < 0.0f) {
        fn_80477C2C(interp, -delta);
    } else {
        fn_80477ED4(interp, delta);
    }
    return 1;
}
