/* RenderWare core: the camera module (0x804842C8-0x80485BCC).
 * The frustum and view matrix of perspective and parallel cameras, the sync callback, the default
 * begin and end update, module close and open, RwCameraEndUpdate/BeginUpdate, the view offset,
 * clip planes, RwCameraFrustumTestSphere, RwCameraClear, RwCameraShowRaster, the projection, the
 * view window, RwCameraRegisterPlugin, RwCameraDestroy and RwCameraCreate. The file owns the module
 * globals (.sbss), the free list parameters (.sdata), the free list (.bss), the plugin registry
 * and the projection error message (.data 0x8056DF70-0x8056DFB0) and its constant pool (.sdata2
 * 0x805FB2E0-0x805FB308).
 * The depth range helper is a static function defined first: the setters inline it, and its own
 * copy (dead-stripped in retail) puts its constants at the head of the pool, as in retail.
 * NonMatching (floors from wave 40, probes/Game_RwCpp47.py): fn_804842C8 (the perspective frustum:
 * GC/2.7 keeps another count of float values in saved registers and strength-reduces the corner
 * loop differently) and fn_80484C10 (float registers of the corner sums).
 * C, cflags_rw_core (configure.py). */
#include "Game/rw/r6.h"

RwFreeList *fn_8047FC04(s32 entrySize, s32 entriesPerBlock, s32 alignment, s32 preallocBlocks,
                        RwFreeList *inPlace, u32 hint);
RwBool fn_8047FCDC(RwFreeList *list);
RwMatrix *fn_8047E990(RwMatrix *matrix, const void *tolerance);
s32 fn_80481DC8(RwPluginRegistry *reg, s32 size, u32 pluginID, RwPluginObjectConstructor constructCB,
                RwPluginObjectDestructor destructCB, RwPluginObjectCopy copyCB);
const RwPluginRegistry *fn_8048209C(const RwPluginRegistry *reg, void *object);
const RwPluginRegistry *fn_80482144(const RwPluginRegistry *reg, void *object);
f32 fn_80482394(f32 value);
RwV3d *fn_804824A4(RwV3d *out, const RwV3d *in, s32 count, const RwMatrix *matrix);
RwBBox *fn_8048349C(RwBBox *box, const RwV3d *verts, s32 numVerts);
RwFrame *fn_8048722C(RwFrame *frame);
RwRaster *fn_80489D44(RwRaster *raster, void *device, u32 flags);
void fn_8048B354(void);
void fn_8048D834(void *object);
void fn_8049B148(RwCamera *camera);

static RwModuleInfo cameraModule;
RwPluginRegistry cameraTKList = {sizeof(RwCamera), sizeof(RwCamera), 0, 0, NULL, NULL};
static s32 cameraFreeListBlockSize = 4;
static s32 cameraFreeListPreallocBlocks = 1;
static RwFreeList cameraFreeList;

#define RWCAMERAGLOBAL (RWGLOBALAT(RwFreeList *, cameraModule.globalsOffset))
#define CameraFrame(camera) ((RwFrame *)(camera)->object.object.parent)

/* Depth range of the device mapped onto the clip planes (their reciprocals in perspective). */
static void CameraUpdateZShiftAndScale(RwCamera *camera) {
    f32 zNear = RwEngineInstance->dOpenDevice.zBufferNear;
    f32 zFar = RwEngineInstance->dOpenDevice.zBufferFar;
    f32 nearPlane;
    f32 farPlane;
    f32 margin;
    f32 zScale;

    switch (camera->projectionType) {
    case 2:
        farPlane = camera->farPlane;
        nearPlane = camera->nearPlane;
        break;
    case 1:
    default:
        farPlane = 1.0f / camera->farPlane;
        nearPlane = 1.0f / camera->nearPlane;
        break;
    }
    margin = 0.0001f * (zFar - zNear);
    zFar -= margin;
    zNear += margin;
    zScale = (zFar - zNear) / (farPlane - nearPlane);
    camera->zScale = zScale;
    camera->zShift = 0.5f * ((zFar + zNear) - zScale * (farPlane + nearPlane));
}

/* The byte of a frustum plane that picks a bounding box corner: 1 where the normal's component
 * is positive, 0 where it is negative (the float's sign bit). */
#define PLANECLOSEST(f) ((u8)((*(const s32 *)&(f) >> 31) + 1))

#define PLANESETCLOSEST(p)                                                                         \
    do {                                                                                           \
        (p).closestX = PLANECLOSEST((p).plane.normal.x);                                           \
        (p).closestY = PLANECLOSEST((p).plane.normal.y);                                           \
        (p).closestZ = PLANECLOSEST((p).plane.normal.z);                                           \
    } while (0)

#define V3DDOT(a, b) ((a).x * (b).x + (a).y * (b).y + (a).z * (b).z)

#define V3DNORMALIZE(v)                                                                            \
    do {                                                                                           \
        f32 recip_ = fn_80482394((v).x * (v).x + (v).y * (v).y + (v).z * (v).z);                   \
        (v).x *= recip_;                                                                           \
        (v).y *= recip_;                                                                           \
        (v).z *= recip_;                                                                           \
    } while (0)

/* The frustum of a perspective camera: the four corner rays through the view window, the near
 * and far corners along them from the eye (shifted by the view offset), then the planes. */
void fn_804842C8(RwCamera *camera) {
    RwV3d *corners = camera->frustumCorners;
    const RwMatrix *ltm = &CameraFrame(camera)->ltm;
    RwV3d temp;
    RwV3d right;
    RwV3d up;
    RwV3d offset;
    RwV3d edge;
    s32 i;
    RwV3d *corner;

    right.x = ltm->right.x * camera->viewWindow.x;
    right.y = ltm->right.y * camera->viewWindow.x;
    right.z = ltm->right.z * camera->viewWindow.x;
    temp.x = ltm->at.x + right.x;
    temp.y = ltm->at.y + right.y;
    temp.z = ltm->at.z + right.z;
    up.x = ltm->up.x * camera->viewWindow.y;
    up.y = ltm->up.y * camera->viewWindow.y;
    up.z = ltm->up.z * camera->viewWindow.y;
    temp.x = temp.x + up.x;
    temp.y = temp.y + up.y;
    temp.z = temp.z + up.z;
    offset.x = ltm->right.x * -camera->viewOffset.x;
    offset.y = ltm->right.y * -camera->viewOffset.x;
    offset.z = ltm->right.z * -camera->viewOffset.x;
    offset.x += ltm->up.x * camera->viewOffset.y;
    offset.y += ltm->up.y * camera->viewOffset.y;
    offset.z += ltm->up.z * camera->viewOffset.y;
    corners[0] = temp;
    right.x = 2.0f * right.x;
    right.y = 2.0f * right.y;
    right.z = 2.0f * right.z;
    temp.x = temp.x - right.x;
    temp.y = temp.y - right.y;
    temp.z = temp.z - right.z;
    corners[1] = temp;
    up.x = 2.0f * up.x;
    up.y = 2.0f * up.y;
    up.z = 2.0f * up.z;
    temp.x = temp.x - up.x;
    temp.y = temp.y - up.y;
    temp.z = temp.z - up.z;
    corners[2] = temp;
    temp.x = temp.x + right.x;
    temp.y = temp.y + right.y;
    temp.z = temp.z + right.z;
    corners[3] = temp;

    for (i = 0; i < 4; i++) {
        temp.x = corners[i].x - offset.x;
        temp.y = corners[i].y - offset.y;
        temp.z = corners[i].z - offset.z;
        corners[i].x = offset.x + ltm->pos.x;
        corners[i].y = offset.y + ltm->pos.y;
        corners[i].z = offset.z + ltm->pos.z;
        corners[i].x += temp.x * camera->nearPlane;
        corners[i].y += temp.y * camera->nearPlane;
        corners[i].z += temp.z * camera->nearPlane;
        corner = &corners[i + 4];
        corner->x = offset.x + ltm->pos.x;
        corner->y = offset.y + ltm->pos.y;
        corner->z = offset.z + ltm->pos.z;
        corner->x += temp.x * camera->farPlane;
        corner->y += temp.y * camera->farPlane;
        corner->z += temp.z * camera->farPlane;
    }

    camera->frustumPlanes[0].plane.normal = ltm->at;
    camera->frustumPlanes[0].plane.distance = V3DDOT(corners[4], ltm->at);
    PLANESETCLOSEST(camera->frustumPlanes[0]);

    camera->frustumPlanes[1].plane.normal.x = -camera->frustumPlanes[0].plane.normal.x;
    camera->frustumPlanes[1].plane.normal.y = -camera->frustumPlanes[0].plane.normal.y;
    camera->frustumPlanes[1].plane.normal.z = -camera->frustumPlanes[0].plane.normal.z;
    camera->frustumPlanes[1].plane.distance =
        V3DDOT(corners[0], camera->frustumPlanes[1].plane.normal);
    PLANESETCLOSEST(camera->frustumPlanes[1]);

    temp.x = corners[1].x - corners[5].x;
    temp.y = corners[1].y - corners[5].y;
    temp.z = corners[1].z - corners[5].z;
    edge.x = corners[6].x - corners[5].x;
    edge.y = corners[6].y - corners[5].y;
    edge.z = corners[6].z - corners[5].z;
    camera->frustumPlanes[2].plane.normal.x = temp.y * edge.z - temp.z * edge.y;
    camera->frustumPlanes[2].plane.normal.y = temp.z * edge.x - temp.x * edge.z;
    camera->frustumPlanes[2].plane.normal.z = temp.x * edge.y - temp.y * edge.x;
    V3DNORMALIZE(camera->frustumPlanes[2].plane.normal);
    camera->frustumPlanes[2].plane.distance =
        V3DDOT(corners[1], camera->frustumPlanes[2].plane.normal);
    PLANESETCLOSEST(camera->frustumPlanes[2]);

    edge.x = corners[4].x - corners[5].x;
    edge.y = corners[4].y - corners[5].y;
    edge.z = corners[4].z - corners[5].z;
    camera->frustumPlanes[3].plane.normal.x = edge.y * temp.z - edge.z * temp.y;
    camera->frustumPlanes[3].plane.normal.y = edge.z * temp.x - edge.x * temp.z;
    camera->frustumPlanes[3].plane.normal.z = edge.x * temp.y - edge.y * temp.x;
    V3DNORMALIZE(camera->frustumPlanes[3].plane.normal);
    camera->frustumPlanes[3].plane.distance =
        V3DDOT(corners[1], camera->frustumPlanes[3].plane.normal);
    PLANESETCLOSEST(camera->frustumPlanes[3]);

    temp.x = corners[3].x - corners[7].x;
    temp.y = corners[3].y - corners[7].y;
    temp.z = corners[3].z - corners[7].z;
    edge.x = corners[4].x - corners[7].x;
    edge.y = corners[4].y - corners[7].y;
    edge.z = corners[4].z - corners[7].z;
    camera->frustumPlanes[4].plane.normal.x = temp.y * edge.z - temp.z * edge.y;
    camera->frustumPlanes[4].plane.normal.y = temp.z * edge.x - temp.x * edge.z;
    camera->frustumPlanes[4].plane.normal.z = temp.x * edge.y - temp.y * edge.x;
    V3DNORMALIZE(camera->frustumPlanes[4].plane.normal);
    camera->frustumPlanes[4].plane.distance =
        V3DDOT(corners[3], camera->frustumPlanes[4].plane.normal);
    PLANESETCLOSEST(camera->frustumPlanes[4]);

    edge.x = corners[6].x - corners[7].x;
    edge.y = corners[6].y - corners[7].y;
    edge.z = corners[6].z - corners[7].z;
    camera->frustumPlanes[5].plane.normal.x = edge.y * temp.z - edge.z * temp.y;
    camera->frustumPlanes[5].plane.normal.y = edge.z * temp.x - edge.x * temp.z;
    camera->frustumPlanes[5].plane.normal.z = edge.x * temp.y - edge.y * temp.x;
    V3DNORMALIZE(camera->frustumPlanes[5].plane.normal);
    camera->frustumPlanes[5].plane.distance =
        V3DDOT(corners[3], camera->frustumPlanes[5].plane.normal);
    PLANESETCLOSEST(camera->frustumPlanes[5]);
}

/* The view matrix of a perspective camera: as the parallel one below, but the at axis carries
 * the view window's centre (0.5 minus the offset) instead of a shear. */
RwCamera *fn_80484AAC(RwCamera *camera) {
    RwMatrix *view = &camera->viewMatrix;
    const RwMatrix *ltm = &CameraFrame(camera)->ltm;
    f32 x, y, z;
    f32 scale;
    f32 shear;

    scale = -0.5f * camera->recipViewWindow.x;
    x = ltm->right.x * scale;
    y = ltm->right.y * scale;
    z = ltm->right.z * scale;
    shear = 0.5f - scale * camera->viewOffset.x;
    x = ltm->at.x * shear + x;
    y = ltm->at.y * shear + y;
    z = ltm->at.z * shear + z;
    view->right.x = x;
    view->up.x = y;
    view->at.x = z;
    view->pos.x = 0.5f - (shear + (ltm->pos.x * x + ltm->pos.y * y + ltm->pos.z * z));

    scale = -0.5f * camera->recipViewWindow.y;
    x = ltm->up.x * scale;
    y = ltm->up.y * scale;
    z = ltm->up.z * scale;
    scale = scale * camera->viewOffset.y + 0.5f;
    shear = scale;
    x = ltm->at.x * shear + x;
    y = ltm->at.y * shear + y;
    z = ltm->at.z * shear + z;
    view->right.y = x;
    view->up.y = y;
    view->at.y = z;
    view->pos.y = 0.5f - (shear + (ltm->pos.x * x + ltm->pos.y * y + ltm->pos.z * z));

    view->right.z = ltm->at.x;
    view->up.z = ltm->at.y;
    view->at.z = ltm->at.z;
    view->pos.z = -(ltm->pos.x * ltm->at.x + ltm->pos.y * ltm->at.y + ltm->pos.z * ltm->at.z);
    fn_8047E990(view, NULL);
    return camera;
}

/* The frustum of a parallel camera: the eight corners of the sheared view box in world space,
 * the far and near planes from the frame's at axis, and the four sides from the corners. */
void fn_80484C10(RwCamera *camera) {
    RwV3d *corners = camera->frustumCorners;
    const RwMatrix *ltm = &CameraFrame(camera)->ltm;
    RwV3d edge0;
    RwV3d edge1;
    f32 x0, x1, y0, y1;
    f32 nearZ, farZ;
    f32 nearX, nearY, farX, farY;
    f32 offX, offY;

    /* Read into locals first: the corners are stored through a pointer into the camera. */
    x0 = camera->viewWindow.x;
    y0 = camera->viewWindow.y;
    nearZ = camera->nearPlane;
    farZ = camera->farPlane;
    offX = camera->viewOffset.x;
    offY = camera->viewOffset.y;
    x1 = -x0;
    y1 = -y0;
    nearX = (1.0f - nearZ) * -offX;
    nearY = (1.0f - nearZ) * offY;
    farX = (1.0f - farZ) * -offX;
    farY = (1.0f - farZ) * offY;
    corners[0].z = corners[1].z = corners[2].z = corners[3].z = nearZ;
    corners[4].z = corners[5].z = corners[6].z = corners[7].z = farZ;
    corners[0].x = corners[3].x = x0 + nearX;
    corners[1].x = corners[2].x = x1 + nearX;
    corners[4].x = corners[7].x = x0 + farX;
    corners[5].x = corners[6].x = x1 + farX;
    corners[0].y = corners[1].y = y0 + nearY;
    corners[2].y = corners[3].y = y1 + nearY;
    corners[4].y = corners[5].y = y0 + farY;
    corners[6].y = corners[7].y = y1 + farY;
    fn_804824A4(corners, corners, 8, ltm);

    camera->frustumPlanes[0].plane.normal = ltm->at;
    camera->frustumPlanes[0].plane.distance =
        V3DDOT(corners[4], camera->frustumPlanes[0].plane.normal);
    PLANESETCLOSEST(camera->frustumPlanes[0]);

    camera->frustumPlanes[1].plane.normal.x = -camera->frustumPlanes[0].plane.normal.x;
    camera->frustumPlanes[1].plane.normal.y = -camera->frustumPlanes[0].plane.normal.y;
    camera->frustumPlanes[1].plane.normal.z = -camera->frustumPlanes[0].plane.normal.z;
    camera->frustumPlanes[1].plane.distance =
        V3DDOT(corners[0], camera->frustumPlanes[1].plane.normal);
    PLANESETCLOSEST(camera->frustumPlanes[1]);

    edge0.x = corners[1].x - corners[5].x;
    edge0.y = corners[1].y - corners[5].y;
    edge0.z = corners[1].z - corners[5].z;
    edge1.x = corners[6].x - corners[5].x;
    edge1.y = corners[6].y - corners[5].y;
    edge1.z = corners[6].z - corners[5].z;
    camera->frustumPlanes[2].plane.normal.x = edge0.y * edge1.z - edge0.z * edge1.y;
    camera->frustumPlanes[2].plane.normal.y = edge0.z * edge1.x - edge0.x * edge1.z;
    camera->frustumPlanes[2].plane.normal.z = edge0.x * edge1.y - edge0.y * edge1.x;
    V3DNORMALIZE(camera->frustumPlanes[2].plane.normal);
    camera->frustumPlanes[2].plane.distance =
        V3DDOT(corners[1], camera->frustumPlanes[2].plane.normal);
    PLANESETCLOSEST(camera->frustumPlanes[2]);

    edge1.x = corners[4].x - corners[5].x;
    edge1.y = corners[4].y - corners[5].y;
    edge1.z = corners[4].z - corners[5].z;
    camera->frustumPlanes[3].plane.normal.x = edge1.y * edge0.z - edge1.z * edge0.y;
    camera->frustumPlanes[3].plane.normal.y = edge1.z * edge0.x - edge1.x * edge0.z;
    camera->frustumPlanes[3].plane.normal.z = edge1.x * edge0.y - edge1.y * edge0.x;
    V3DNORMALIZE(camera->frustumPlanes[3].plane.normal);
    camera->frustumPlanes[3].plane.distance =
        V3DDOT(corners[1], camera->frustumPlanes[3].plane.normal);
    PLANESETCLOSEST(camera->frustumPlanes[3]);

    camera->frustumPlanes[4].plane.normal.x = -camera->frustumPlanes[2].plane.normal.x;
    camera->frustumPlanes[4].plane.normal.y = -camera->frustumPlanes[2].plane.normal.y;
    camera->frustumPlanes[4].plane.normal.z = -camera->frustumPlanes[2].plane.normal.z;
    camera->frustumPlanes[4].plane.distance =
        V3DDOT(corners[3], camera->frustumPlanes[4].plane.normal);
    PLANESETCLOSEST(camera->frustumPlanes[4]);

    camera->frustumPlanes[5].plane.normal.x = -camera->frustumPlanes[3].plane.normal.x;
    camera->frustumPlanes[5].plane.normal.y = -camera->frustumPlanes[3].plane.normal.y;
    camera->frustumPlanes[5].plane.normal.z = -camera->frustumPlanes[3].plane.normal.z;
    camera->frustumPlanes[5].plane.distance =
        V3DDOT(corners[3], camera->frustumPlanes[5].plane.normal);
    PLANESETCLOSEST(camera->frustumPlanes[5]);
}

/* The view matrix of a parallel camera: the frame's axes scaled by the view window, sheared by
 * the view offset, with the frame's position taken out. */
RwCamera *fn_80485124(RwCamera *camera) {
    RwMatrix *view = &camera->viewMatrix;
    const RwMatrix *ltm = &CameraFrame(camera)->ltm;
    f32 x, y, z;
    f32 scale;
    f32 shear;

    scale = -0.5f * camera->recipViewWindow.x;
    x = ltm->right.x * scale;
    y = ltm->right.y * scale;
    z = ltm->right.z * scale;
    shear = -(scale * camera->viewOffset.x);
    x = ltm->at.x * shear + x;
    y = ltm->at.y * shear + y;
    z = ltm->at.z * shear + z;
    view->right.x = x;
    view->up.x = y;
    view->at.x = z;
    view->pos.x = 0.5f - (shear + (ltm->pos.x * x + ltm->pos.y * y + ltm->pos.z * z));

    scale = -0.5f * camera->recipViewWindow.y;
    x = ltm->up.x * scale;
    y = ltm->up.y * scale;
    z = ltm->up.z * scale;
    scale *= camera->viewOffset.y;
    shear = scale;
    x = ltm->at.x * shear + x;
    y = ltm->at.y * shear + y;
    z = ltm->at.z * shear + z;
    view->right.y = x;
    view->up.y = y;
    view->at.y = z;
    view->pos.y = 0.5f - (shear + (ltm->pos.x * x + ltm->pos.y * y + ltm->pos.z * z));

    view->right.z = ltm->at.x;
    view->up.z = ltm->at.y;
    view->at.z = ltm->at.z;
    view->pos.z = -(ltm->pos.x * ltm->at.x + ltm->pos.y * ltm->at.y + ltm->pos.z * ltm->at.z);
    fn_8047E990(view, NULL);
    return camera;
}

/* The camera's sync callback: rebuilds the view matrix, the frustum planes and their box. */
RwObjectHasFrame *fn_8048528C(RwObjectHasFrame *object) {
    RwCamera *camera = (RwCamera *)object;

    if (camera->projectionType == 1) {
        fn_80484AAC(camera);
        fn_804842C8(camera);
    } else {
        fn_80485124(camera);
        fn_80484C10(camera);
    }
    fn_8048349C(&camera->frustumBoundBox, camera->frustumCorners, 8);
    return object;
}

/* The default end update: hands the camera to the device and clears the current camera. */
RwCamera *fn_804852F0(RwCamera *camera) {
    if (RwEngineInstance->stdFunc[10](NULL, camera, 0)) {
        RwEngineInstance->curCamera = NULL;
        return camera;
    }
    return NULL;
}

/* The default begin update. */
RwCamera *fn_80485354(RwCamera *camera) {
    RwEngineInstance->curCamera = camera;
    fn_8048B354();
    if (RwEngineInstance->stdFunc[1](NULL, camera, 0)) {
        fn_8049B148(camera);
        return camera;
    }
    return NULL;
}

/* _rwCameraClose. */
void *fn_804853C0(void *instance) {
    if (RWCAMERAGLOBAL) {
        fn_8047FCDC(RWCAMERAGLOBAL);
        RWCAMERAGLOBAL = NULL;
    }
    cameraModule.numInstances--;
    return instance;
}

/* _rwCameraOpen. */
void *fn_80485424(void *instance, s32 offset) {
    cameraModule.globalsOffset = offset;
    RWCAMERAGLOBAL = fn_8047FC04(cameraTKList.sizeOfStruct, cameraFreeListBlockSize, 4, cameraFreeListPreallocBlocks,
                                 &cameraFreeList, 0x40005);
    if (!RWCAMERAGLOBAL) {
        return NULL;
    }
    cameraModule.numInstances++;
    return instance;
}

/* RwCameraEndUpdate. */
RwCamera *fn_804854B4(RwCamera *camera) {
    return camera->endUpdate(camera);
}

/* RwCameraBeginUpdate. */
RwCamera *fn_804854DC(RwCamera *camera) {
    return camera->beginUpdate(camera);
}

/* RwCameraSetViewOffset. */
RwCamera *fn_80485504(RwCamera *camera, const RwV2d *offset) {
    camera->viewOffset = *offset;
    if (CameraFrame(camera)) {
        fn_8048722C(CameraFrame(camera));
    }
    return camera;
}


/* RwCameraSetNearClipPlane. */
RwCamera *fn_80485550(RwCamera *camera, f32 nearClip) {
    camera->nearPlane = nearClip;
    CameraUpdateZShiftAndScale(camera);
    if (CameraFrame(camera)) {
        fn_8048722C(CameraFrame(camera));
    }
    return camera;
}

/* RwCameraSetFarClipPlane. */
RwCamera *fn_8048560C(RwCamera *camera, f32 farClip) {
    camera->farPlane = farClip;
    CameraUpdateZShiftAndScale(camera);
    if (CameraFrame(camera)) {
        fn_8048722C(CameraFrame(camera));
    }
    return camera;
}

/* RwCameraFrustumTestSphere: 0 outside, 1 crossing a plane, 2 inside. */
s32 fn_804856C8(const RwCamera *camera, const RwSphere *sphere) {
    const RwFrustumPlane *plane = camera->frustumPlanes;
    s32 count = 6;
    s32 result = 2;

    while (count--) {
        f32 distance = sphere->center.x * plane->plane.normal.x +
                       sphere->center.y * plane->plane.normal.y +
                       sphere->center.z * plane->plane.normal.z;
        distance -= plane->plane.distance;
        if (distance > sphere->radius) {
            return 0;
        }
        if (distance > -sphere->radius) {
            result = 1;
        }
        plane++;
    }
    return result;
}

/* RwCameraClear. */
RwCamera *fn_804857BC(RwCamera *camera, void *colour, s32 clearMode) {
    if (RwEngineInstance->stdFunc[21](camera, colour, clearMode)) {
        return camera;
    }
    return NULL;
}

/* RwCameraShowRaster. */
RwCamera *fn_80485808(RwCamera *camera, void *device, u32 flags) {
    if (fn_80489D44(camera->frameBuffer, device, flags)) {
        return camera;
    }
    return NULL;
}

/* RwCameraSetProjection. */
RwCamera *fn_8048584C(RwCamera *camera, s32 projection) {
    switch (projection) {
    case 1:
    case 2:
        camera->projectionType = projection;
        if (CameraFrame(camera)) {
            fn_8048722C(CameraFrame(camera));
        }
        CameraUpdateZShiftAndScale(camera);
        return camera;
    default:
        RWERROR((0x80000003, "Invalid projection type specified"));
        return NULL;
    }
}

/* RwCameraSetViewWindow. */
RwCamera *fn_80485950(RwCamera *camera, const RwV2d *window) {
    camera->viewWindow = *window;
    camera->recipViewWindow.x = 1.0f / camera->viewWindow.x;
    camera->recipViewWindow.y = 1.0f / camera->viewWindow.y;
    if (CameraFrame(camera)) {
        fn_8048722C(CameraFrame(camera));
    }
    return camera;
}

/* RwCameraRegisterPlugin. */
s32 fn_804859B8(s32 size, u32 pluginID, RwPluginObjectConstructor constructCB,
                RwPluginObjectDestructor destructCB, RwPluginObjectCopy copyCB) {
    return fn_80481DC8(&cameraTKList, size, pluginID, constructCB, destructCB, copyCB);
}

/* RwCameraDestroy. */
RwBool fn_804859F8(RwCamera *camera) {
    fn_80482144(&cameraTKList, camera);
    fn_8048D834(camera);
    RwEngineInstance->memoryFree(RWCAMERAGLOBAL, camera);
    return TRUE;
}

/* RwCameraCreate: a perspective camera with a unit view window, clip planes 0.05 and 10. */
RwCamera *fn_80485A58(void) {
    RwCamera *camera = (RwCamera *)RwEngineInstance->memoryAlloc(RWCAMERAGLOBAL, 0x30005);

    if (!camera) {
        return NULL;
    }
    camera->object.object.type = 4;
    camera->object.object.subType = 0;
    camera->object.object.flags = 0;
    camera->object.object.privateFlags = 0;
    camera->object.object.parent = NULL;
    camera->object.sync = fn_8048528C;
    camera->beginUpdate = fn_80485354;
    camera->endUpdate = fn_804852F0;
    camera->viewWindow.x = camera->viewWindow.y = 1.0f;
    camera->recipViewWindow.x = camera->recipViewWindow.y = 1.0f;
    camera->viewOffset.x = camera->viewOffset.y = 0.0f;
    camera->nearPlane = 0.05f;
    camera->farPlane = 10.0f;
    camera->fogPlane = 5.0f;
    camera->frameBuffer = NULL;
    camera->zBuffer = NULL;
    camera->projectionType = 1;
    CameraUpdateZShiftAndScale(camera);
    camera->viewMatrix.flags = 0;
    fn_8048209C(&cameraTKList, camera);
    return camera;
}
