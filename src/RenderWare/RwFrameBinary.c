/* RenderWare core: frame lists in streams (0x804836EC-0x80483AFC): RwFrameRegisterPluginStream,
 * _rwFrameListDeinitialize and _rwFrameListStreamRead. The file owns the flag that links the
 * frames read without updating them (.sdata, padded to 8) and the orthonormal tolerances
 * (.sdata2 0x805FB2D8-0x805FB2E0); the frame plugin registry belongs to the frame module.
 * C, cflags_rw_core (configure.py). */
#include "Game/rw/r6.h"

RwBool fn_8047CF50(RwStream *stream, u32 type, u32 *length, u32 *version);
void *fn_8047D254(void *mem, u32 size);
f32 fn_8047E704(const RwMatrix *matrix);
f32 fn_8047E750(const RwMatrix *matrix);
f32 fn_8047E7A8(const RwMatrix *matrix);
u32 fn_80480D4C(RwStream *stream, void *buffer, u32 length);
s32 fn_804815F0(RwPluginRegistry *reg, u32 pluginID, RwPluginDataChunkReadCallBack readCB,
                RwPluginDataChunkWriteCallBack writeCB,
                RwPluginDataChunkGetSizeCallBack getSizeCB);
const RwPluginRegistry *fn_804816AC(const RwPluginRegistry *reg, RwStream *stream, void *object);
RwFrame *fn_80486D00(void);
RwBool fn_80487208(RwFrame *frame);
RwFrame *fn_8048722C(RwFrame *frame);
RwFrame *fn_804872D8(RwFrame *frame);
RwFrame *fn_804872E0(RwFrame *parent, RwFrame *child);
RwFrame *fn_80487348(RwFrame *parent, RwFrame *child);

extern RwPluginRegistry frameTKList;

static s32 frameListStreamFastLink = 1;

/* RwFrameRegisterPluginStream. */
s32 fn_804836EC(u32 pluginID, RwPluginDataChunkReadCallBack readCB,
                RwPluginDataChunkWriteCallBack writeCB,
                RwPluginDataChunkGetSizeCallBack getSizeCB) {
    return fn_804815F0(&frameTKList, pluginID, readCB, writeCB, getSizeCB);
}

/* _rwFrameListDeinitialize. */
rwFrameList *fn_80483728(rwFrameList *frameList) {
    if (frameList->numFrames) {
        RwEngineInstance->memoryFuncs.rwfree(frameList->frames);
    }
    return frameList;
}

/* _rwFrameListStreamRead: creates the frames, links each to its parent, then reads every
 * frame's plugin data. */
rwFrameList *fn_80483774(RwStream *stream, rwFrameList *frameList) {
    s32 numFrames;
    u32 size;
    u32 version;
    s32 i;
    RwFrame *frame;
    RwMatrix *matrix;

    if (!fn_8047CF50(stream, 1, &size, &version)) {
        return NULL;
    }
    if (version >= 0x35000 && version <= 0x37002) {
        if (fn_80480D4C(stream, &numFrames, sizeof(numFrames)) != sizeof(numFrames)) {
            return NULL;
        }
        fn_8047D254(&numFrames, sizeof(numFrames));
        frameList->numFrames = numFrames;
        frameList->frames = (RwFrame **)RwEngineInstance->memoryFuncs.rwmalloc(
            sizeof(RwFrame *) * numFrames, 0x3000E);
        if (!frameList->frames) {
            RWERROR((0x80000013, sizeof(RwFrame *) * numFrames));
            return NULL;
        }
        for (i = 0; i < numFrames; i++) {
            rwStreamFrame f;

            if (fn_80480D4C(stream, &f, sizeof(f)) != sizeof(f)) {
                RwEngineInstance->memoryFuncs.rwfree(frameList->frames);
                return NULL;
            }
            fn_8047D254(&f, sizeof(f));
            frame = fn_80486D00();
            if (!frame) {
                RwEngineInstance->memoryFuncs.rwfree(frameList->frames);
                return NULL;
            }
            matrix = &frame->modelling;
            matrix->right = f.right;
            matrix->up = f.up;
            matrix->at = f.at;
            matrix->pos = f.pos;
            if (0.01f >= fn_8047E7A8(matrix) && 0.01f >= fn_8047E750(matrix) &&
                0.99f <= fn_8047E704(matrix)) {
                matrix->flags &= ~0x20000;
            } else {
                matrix->flags &= ~0x20003;
            }
            frameList->frames[i] = frame;
            if (f.parentIndex >= 0) {
                if (frameListStreamFastLink) {
                    fn_80487348(frameList->frames[f.parentIndex], frame);
                } else {
                    fn_804872E0(frameList->frames[f.parentIndex], frame);
                }
            }
        }
    } else {
        RWERROR((0x80000004));
        return NULL;
    }
    for (i = 0; i < numFrames; i++) {
        frame = frameList->frames[i];
        if (!fn_804816AC(&frameTKList, stream, frame)) {
            fn_80487208(frameList->frames[0]);
            RwEngineInstance->memoryFuncs.rwfree(frameList->frames);
            return NULL;
        }
        if (frame == fn_804872D8(frame) && frameListStreamFastLink == 1) {
            fn_8048722C(frame);
        }
    }
    return frameList;
}
