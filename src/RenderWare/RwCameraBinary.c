/* RenderWare core: cameras in streams (0x80483554-0x804836EC): RwCameraRegisterPluginStream and
 * RwCameraStreamRead. No data; the camera plugin registry belongs to the camera module.
 * C, cflags_rw_core (configure.py). */
#include "Game/rw/r6.h"

RwBool fn_8047CF50(RwStream *stream, u32 type, u32 *length, u32 *version);
void *fn_8047D254(void *mem, u32 size);
u32 fn_80480D4C(RwStream *stream, void *buffer, u32 length);
s32 fn_804815F0(RwPluginRegistry *reg, u32 pluginID, RwPluginDataChunkReadCallBack readCB,
                RwPluginDataChunkWriteCallBack writeCB,
                RwPluginDataChunkGetSizeCallBack getSizeCB);
const RwPluginRegistry *fn_804816AC(const RwPluginRegistry *reg, RwStream *stream, void *object);
RwCamera *fn_80485A58(void);
RwCamera *fn_80485504(RwCamera *camera, const RwV2d *offset);
RwCamera *fn_80485550(RwCamera *camera, f32 nearClip);
RwCamera *fn_8048560C(RwCamera *camera, f32 farClip);
RwCamera *fn_8048584C(RwCamera *camera, u32 projection);
RwCamera *fn_80485950(RwCamera *camera, const RwV2d *window);

extern RwPluginRegistry cameraTKList;

/* RwCameraRegisterPluginStream. */
s32 fn_80483554(u32 pluginID, RwPluginDataChunkReadCallBack readCB,
                RwPluginDataChunkWriteCallBack writeCB,
                RwPluginDataChunkGetSizeCallBack getSizeCB) {
    return fn_804815F0(&cameraTKList, pluginID, readCB, writeCB, getSizeCB);
}

/* RwCameraStreamRead. */
RwCamera *fn_80483590(RwStream *stream) {
    u32 size;
    u32 version;

    if (!fn_8047CF50(stream, 1, &size, &version)) {
        return NULL;
    }
    if (version >= 0x35000 && version <= 0x37002) {
        rwStreamCamera cam;
        RwCamera *camera;

        memset(&cam, 0, sizeof(cam));
        if (fn_80480D4C(stream, &cam, size) != size) {
            return NULL;
        }
        fn_8047D254(&cam, sizeof(cam));
        camera = fn_80485A58();
        if (!camera) {
            return NULL;
        }
        if (!fn_804816AC(&cameraTKList, stream, camera)) {
            return NULL;
        }
        fn_80485950(camera, &cam.viewWindow);
        fn_80485504(camera, &cam.viewOffset);
        fn_80485550(camera, cam.nearPlane);
        fn_8048560C(camera, cam.farPlane);
        camera->fogPlane = cam.fogPlane;
        fn_8048584C(camera, cam.projection);
        return camera;
    }
    RWERROR((0x80000004));
    return NULL;
}
