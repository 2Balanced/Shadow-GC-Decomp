/* RenderWare 2D toolkit (rt2d), part: the toolkit's device and CTM functions from fn_80473E9C
 * on (their float constants start a new .sdata2 group at 0x805FB250). The font functions before
 * them (0x80470AC4..0x80473E9C) are RenderWare/Rt2dFont.c. Written from the retail code; the
 * names are descriptions, the fn_ symbols are kept because game code calls them by address.
 * Built as a C library: -O4,p, no stmw/lmw, automatic inlining. */
#include "Game/rw/r5.h"

#ifdef __cplusplus
extern "C" {
#endif

extern const f32 lbl_805FB230; /* 0.0f */
extern const f32 lbl_805FB234; /* 1.0f */
extern const f32 lbl_805FB250; /* 1.0f */
extern const f32 lbl_805FB254; /* 0.0f */
extern const f32 lbl_805FB258; /* 0.5f */

extern const f32 lbl_805FB25C; /* 2.0f */
extern const f32 lbl_805FB268; /* 0.0f */
extern const f32 lbl_805FB26C; /* 1.0f */
extern char lbl_8056DCE8[];    /* the path module's format strings, then "METRICS1\n" .. "METRICS3\n" */
extern char lbl_805EDE40[5];    /* ".met" */
extern char lbl_805EDE48[2];    /* "r" */

RwMatrix *fn_8047F994(void);                                            /* RwMatrixCreate */
RwBool fn_8047F958(RwMatrix *matrix);                                   /* RwMatrixDestroy */
RwMatrix *fn_8047EBB8(RwMatrix *dst, const RwMatrix *a, const RwMatrix *b); /* RwMatrixMultiply */
RwMatrix *fn_8047F398(RwMatrix *matrix, const RwV3d *scale, s32 op);    /* RwMatrixScale */
RwMatrix *fn_8047F59C(RwMatrix *matrix, const RwV3d *offset, s32 op);   /* RwMatrixTranslate */
RwMatrix *fn_8047F0B0(RwMatrix *dst, const RwMatrix *src);              /* RwMatrixInvert */
RwMatrix *RwFrameGetLTM(RwFrame *frame);
RwV3d *fn_80482504(RwV3d *out, const RwV3d *in, const RwMatrix *matrix); /* RwV3dTransformVector */
RwV3d *fn_80482458(RwV3d *out, const RwV3d *in, const RwMatrix *matrix); /* RwV3dTransformPoint */
f32 fn_80482340(f32 x); /* square root */
f32 fn_80482394(f32 x); /* inverse square root */

void *fn_8047A178(const char *name, const char *mode);  /* opens a file */
char *fn_8047A518(char *buffer, s32 size, void *file);  /* reads a line */
s32 fn_8047A388(void *file);                            /* closes a file */
Rt2dFont *fn_8047149C(Rt2dFont *font, char *buffer, void *file, char *line);
Rt2dFont *fn_80471A54(Rt2dFont *font, char *buffer, void *file, char *line);
Rt2dFont *fn_804724AC(Rt2dFont *font, char *buffer, void *file, char *line);
void fn_80470C54(void);

void fn_804772C4(void);
void fn_80476A58(void);
void fn_804704D4(void);
void fn_80473260(void);
void fn_8047721C(void);
void fn_804771E0(void);
void fn_80473118(void);
void fn_80470474(void);
void fn_80476A0C(void);
void fn_8047726C(void);
Rt2dPath *fn_80476490(void);          /* Rt2dPathCreate */
Rt2dPath *fn_80476510(Rt2dPath *path); /* Rt2dPathLock */

RwMatrix *fn_804749CC(void);
RwBool fn_80474498(void);

void *fn_80473E9C(void *camera) {
    return lbl_805E1950.cameraBeginUpdate(camera);
}

#define CTM() (lbl_805E1950.ctm[lbl_805E1950.ctmTop])

/* Makes the current transformation matrix the identity. */
RwBool fn_80473ECC(void) {
    CTM()->at.z = 1.0f;
    CTM()->up.y = 1.0f;
    CTM()->right.x = 1.0f;
    CTM()->up.x = 0.0f;
    CTM()->right.z = 0.0f;
    CTM()->right.y = 0.0f;
    CTM()->at.y = 0.0f;
    CTM()->at.x = 0.0f;
    CTM()->up.z = 0.0f;
    CTM()->pos.z = 0.0f;
    CTM()->pos.y = 0.0f;
    CTM()->pos.x = 0.0f;
    CTM()->flags |= 0x20003;
    lbl_805E1950.deviceValid = 0;
    lbl_805E1950.flatValid = 0;
    return 1;
}

RwMatrix *fn_804749CC(void);

/* The device's step vectors and origin in the current coordinate system. */
RwBool fn_80473FD0(RwV2d *xstep, RwV2d *ystep, RwV2d *origin) {
    RwV3d v;
    RwV2d view;
    RwV2d raster;
    f32 depth;
    RwMatrix *matrix;

    if (lbl_805E1950.camera->projection == 2) {
        depth = 1.0f;
    } else {
        depth = lbl_805E1950.layerDepth;
    }
    view = lbl_805E1950.viewSize;
    raster = lbl_805E1950.rasterSize;
    fn_804749CC();
    matrix = &lbl_805E1950.deviceMatrix;

    v.x = -depth * (view.x / raster.x);
    v.y = 0.0f;
    v.z = 0.0f;
    fn_80482504(&v, &v, matrix);
    xstep->x = v.x;
    xstep->y = v.y;

    v.x = 0.0f;
    v.y = depth * (view.y / raster.y);
    v.z = 0.0f;
    fn_80482504(&v, &v, matrix);
    ystep->x = v.x;
    ystep->y = v.y;

    v.x = 0.5f * view.x * depth;
    v.y = 0.5f * -view.y * depth;
    v.z = 0.0f;
    fn_80482458(&v, &v, matrix);
    origin->x = v.x;
    origin->y = v.y;
    return 1;
}

/* The squared length of the flatness tolerance in the current coordinate system. */
f32 fn_80474168(void) {
    if (!lbl_805E1950.flatValid) {
        RwV3d v;

        fn_804749CC();
        v.x = lbl_805E1950.flatness *
              (lbl_805E1950.layerDepth * (lbl_805E1950.viewSize.x / lbl_805E1950.rasterSize.x));
        v.y = 0.0f;
        v.z = 0.0f;
        fn_80482504(&v, &v, &lbl_805E1950.deviceMatrix);
        lbl_805E1950.flatValid = 1;
        lbl_805E1950.flatDepth = v.x * v.x + v.y * v.y + v.z * v.z;
    }
    return lbl_805E1950.flatDepth;
}

/* Sets the depth the 2D layer is drawn at. */
RwBool fn_80474228(f32 depth) {
    if (depth > 0.0f) {
        lbl_805E1950.layerDepth = depth;
        lbl_805E1950.deviceValid = 0;
        return 1;
    }
    return 0;
}

/* Sets the origin and the aspect of the device's metric. The parameters are in this order
 * because the call below loads the width before the height. */
static void SetMetric(f32 h, f32 w, f32 y, f32 x) {
    lbl_805E1950.field_0x338 = x;
    lbl_805E1950.field_0x33C = y;
    if (w > h) {
        lbl_805E1950.field_0x340 = w / h;
        lbl_805E1950.field_0x344 = 1.0f;
    } else {
        lbl_805E1950.field_0x340 = 1.0f;
        lbl_805E1950.field_0x344 = h / w;
    }
}

/* Attaches the toolkit to a camera: opens the modules, takes the raster and view window sizes
 * and hooks the camera's end-of-update function. */
void fn_80474258(RwCamera *camera) {
    lbl_805E1950.field_0x31C = 0;
    fn_804772C4();
    fn_80476A58();
    fn_804704D4();
    fn_80473260();
    fn_8047721C();
    fn_80474498();
    if (camera != 0) {
        RwRaster *raster = camera->frameBuffer;

        if (raster != 0) {
            {
                s32 height = raster->height;
                s32 width = raster->width;

                lbl_805E1950.rasterSize.x = (f32)width;
                lbl_805E1950.rasterSize.y = (f32)height;
            }
            lbl_805E1950.viewSize.x = 2.0f * camera->viewWindow.x;
            lbl_805E1950.viewSize.y = 2.0f * camera->viewWindow.y;
            if (camera->endUpdate != fn_80473E9C && lbl_805E1950.camera != camera) {
                lbl_805E1950.cameraBeginUpdate = camera->endUpdate;
                camera->endUpdate = fn_80473E9C;
            }
            lbl_805E1950.camera = camera;
            lbl_805E1950.deviceValid = 0;
            SetMetric(lbl_805E1950.viewSize.y, lbl_805E1950.viewSize.x, 0.0f, 0.0f);
        }
    } else {
        lbl_805E1950.camera = camera;
    }
}

void fn_804743F0(void);

/* Closes the toolkit's modules and forgets the camera. */
void fn_804743AC(void) {
    fn_804743F0();
    fn_804771E0();
    fn_80473118();
    fn_80470474();
    fn_80476A0C();
    fn_8047726C();
    lbl_805E1950.camera = 0;
}

/* Destroys the matrix stack. */
void fn_804743F0(void) {
    s32 i;

    for (i = 0; i < 32; i++) {
        if (lbl_805E1950.ctm[i] != 0) {
            fn_8047F958(lbl_805E1950.ctm[i]);
            lbl_805E1950.ctm[i] = 0;
        }
    }
    lbl_805E1950.ctmTop = 0;
    if (lbl_805E1950.field_0x944 != 0) {
        RWSRCGLOBAL(rwfree)(lbl_805E1950.field_0x944);
        lbl_805E1950.field_0x944 = 0;
        lbl_805E1950.field_0x948 = 0;
    }
}

/* Creates the matrix stack and resets the device state. */
RwBool fn_80474498(void) {
    u32 i;


    lbl_805E1950.ctmTop = 0;
    lbl_805E1950.ctm[0] = fn_8047F994();
    fn_80473ECC();
    lbl_805E1950.flatness = 0.5f;
    lbl_805E1950.flatValid = 0;
    lbl_805E1950.layerDepth = 1.0f;
    lbl_805E1950.deviceValid = 0;
    lbl_805E1950.field_0x330 = 7;
    lbl_805E1950.viewMatrix.at.z = 1.0f;
    lbl_805E1950.viewMatrix.up.y = 1.0f;
    lbl_805E1950.viewMatrix.right.x = 1.0f;
    lbl_805E1950.viewMatrix.up.x = 0.0f;
    lbl_805E1950.viewMatrix.right.z = 0.0f;
    lbl_805E1950.viewMatrix.right.y = 0.0f;
    lbl_805E1950.viewMatrix.at.y = 0.0f;
    lbl_805E1950.viewMatrix.at.x = 0.0f;
    lbl_805E1950.viewMatrix.up.z = 0.0f;
    lbl_805E1950.viewMatrix.pos.z = 0.0f;
    lbl_805E1950.viewMatrix.pos.y = 0.0f;
    lbl_805E1950.viewMatrix.pos.x = 0.0f;
    lbl_805E1950.viewMatrix.flags |= 0x20003;
    for (i = 0; i < 64; i++) {
        lbl_805E1950.index[i] = i;
    }
    for (i = 1; (s32)i < 32; i++) {
        lbl_805E1950.ctm[i] = 0;
    }
    lbl_805E1950.camera = 0;
    lbl_805E1950.field_0x338 = 0.0f;
    lbl_805E1950.field_0x33C = 0.0f;
    lbl_805E1950.field_0x340 = 1.0f;
    lbl_805E1950.field_0x344 = 1.0f;
    lbl_805E1950.field_0x944 = 0;
    lbl_805E1950.field_0x948 = 0;
    return 1;
}

/* Pushes a copy of the current transformation matrix. */
RwBool fn_804747A0(void) {
    RwBool ok = lbl_805E1950.ctmTop < 31;
    if (ok) {
        lbl_805E1950.ctmTop++;
        if (CTM() == 0) {
            CTM() = fn_8047F994();
        }
        *CTM() = *lbl_805E1950.ctm[lbl_805E1950.ctmTop - 1];
    }
    return ok;
}

/* Pops the current transformation matrix. */
RwBool fn_804748C8(void) {
    RwBool ok = 0 < lbl_805E1950.ctmTop;
    if (ok) {
        lbl_805E1950.ctmTop--;
        lbl_805E1950.deviceValid = 0;
        lbl_805E1950.flatValid = 0;
    }
    return ok;
}

/* Scales the current transformation matrix. */
RwBool fn_804748FC(f32 x, f32 y) {
    RwV3d scale;

    scale.x = x;
    scale.y = y;
    scale.z = 1.0f;
    fn_8047F398(CTM(), &scale, 1);
    lbl_805E1950.deviceValid = 0;
    lbl_805E1950.flatValid = 0;
    return 1;
}

/* Translates the current transformation matrix. */
RwBool fn_80474964(f32 x, f32 y) {
    RwV3d offset;

    offset.x = x;
    offset.y = y;
    offset.z = 0.0f;
    fn_8047F59C(CTM(), &offset, 1);
    lbl_805E1950.deviceValid = 0;
    lbl_805E1950.flatValid = 0;
    return 1;
}

/* The matrix from the current coordinate system to the world; rebuilds the device matrices when
 * the CTM, the camera or the layer depth changed. */
RwMatrix *fn_804749CC(void) {
    if (!lbl_805E1950.deviceValid) {
        RwMatrix matrix;
        RwV3d scale;
        RwV3d offset;
        RwV2d view;
        f32 depth;
        f32 size;

        if (lbl_805E1950.camera->projection == 2) {
            depth = 1.0f;
        } else {
            depth = lbl_805E1950.layerDepth;
        }
        fn_8047EBB8(&matrix, CTM(), &lbl_805E1950.viewMatrix);
        scale.x = depth;
        view = lbl_805E1950.viewSize;
        scale.y = depth;
        scale.z = 1.0f;
        fn_8047F398(&matrix, &scale, 2);
        size = (view.x < view.y) ? view.x : view.y;
        scale.y = size;
        scale.x = -size;
        scale.z = 1.0f;
        fn_8047F398(&matrix, &scale, 2);
        offset.x = 0.5f * view.x * depth;
        offset.y = -(0.5f * view.y * depth);
        offset.z = 0.0f;
        fn_8047F59C(&matrix, &offset, 2);
        lbl_805E1950.toDevice = matrix;
        fn_8047F0B0(&lbl_805E1950.deviceMatrix, &matrix);
        lbl_805E1950.deviceValid = 1;
    }
    fn_8047EBB8(&lbl_805E1950.toWorld, &lbl_805E1950.toDevice, RwFrameGetLTM(lbl_805E1950.camera->frame));
    return &lbl_805E1950.toWorld;
}

/* The path module follows (its constants start a new .sdata2 group at 0x805FB268). */

/* Frees one sub-path and returns the one after it. */
Rt2dPath *fn_80474BEC(Rt2dPath *path) {
    Rt2dPath *next = path->next;

    if (path->segments != 0) {
        if (path->numSegment == -1) {
            fn_8047DFC8(path->segments);
        } else {
            RWSRCGLOBAL(rwfree)(path->segments);
        }
    }
    path->segments = 0;
    RWSRCGLOBAL(memoryFree)(lbl_805E1950.pathFreeList, path);
    return next;
}

/* Starts a sub-path at (x, y): in the current sub-path if it is empty, else in a new one.
 * Retail inlines the recursion three times; GC/2.7 does that at depth 3, and one level more at
 * its default depth for this (probably shorter) source. */
#pragma inline_depth(3)
Rt2dPath *fn_80474C7C(Rt2dPath *path, f32 x, f32 y) {
    if (fn_8047E03C(path->curr->segments) == 0) {
        Rt2dPathNode *node = (Rt2dPathNode *)fn_8047DDBC(path->curr->segments, 0x30190);

        node->type = 0;
        node->pos.x = x;
        node->pos.y = y;
        node->normal.x = lbl_805FB268;
        node->normal.y = lbl_805FB268;
        node->dist = lbl_805FB268;
    } else {
        path->curr->next = fn_80476490();
        fn_80476510(path->curr->next);
        path->curr = path->curr->next;
        fn_80474C7C(path, x, y);
    }
    return path;
}

#pragma inline_depth(8)

/* Adds a line to (x, y) and gives the corner before it the bisecting normal. */
Rt2dPath *fn_80474EA0(Rt2dPath *path, f32 x, f32 y) {
    s32 count = fn_8047E03C(path->curr->segments);
    Rt2dPathNode *node = (Rt2dPathNode *)fn_8047DDBC(path->curr->segments, 0x30190);
    f32 length;
    f32 dy;
    f32 dx;
    f32 squared;
    f32 inverse;

    node->type = 1;
    node->pos.x = x;
    node->pos.y = y;
    dy = y - node[-1].pos.y;
    dx = node[-1].pos.x - x;
    squared = dy * dy + dx * dx;
    length = fn_80482340(squared);
    inverse = fn_80482394(squared);
    node->normal.x = dy * inverse;
    node->normal.y = dx * inverse;
    node->dist = node[-1].dist + length;
    if (count > 1) {
        f32 sumX;
        f32 sumY;
        f32 scale;

        sumY = node[-1].normal.y + node->normal.y;
        sumX = node[-1].normal.x + node->normal.x;
        scale = lbl_805FB26C / (node[-1].normal.x * sumX + node[-1].normal.y * sumY);
        node[-1].normal.x = sumX * scale;
        node[-1].normal.y = sumY * scale;
    } else {
        node[-1].normal = node->normal;
    }
    return path;
}

#ifdef __cplusplus
}
#endif
