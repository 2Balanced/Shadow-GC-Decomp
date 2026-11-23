/* RenderWare: RpPTank (particle tank, plugin 0x12F), GameCube back end: the tank's pipeline, the
 * choice of vertex writer, the tank atomic's one-vertex geometry and its render callback.
 *
 * 0x804485BC-0x80448D04 (.sbss 0x805F1450, .sdata2 0x805FB068-0x805FB078). The platform-neutral
 * part of the plugin (0x80447598-0x804485BC) is the file before this one; its callback table at
 * 0x8056D6F8 names fn_80448850, fn_80448A1C and fn_80448A24. The render file (RpPTankGameCubeRender.c)
 * follows: its .sdata2 starts on a new 8-byte group at 0x805FB078, behind the 4-byte hole after
 * this file's -1.0f, which one translation unit would not leave.
 *
 * Built as C with cflags_rw_c (configure.py), the flags the code was matched with in wave 40.
 * fn_80448850 differs only in which saved registers hold its values: with the atomic made from a
 * `void *` parameter (probes/RenderWare_RpPTankGameCube_W41*.py) the atomic, the geometry flags, the
 * data flags and the vertex descriptor get retail's r31-r28, but the material and the mesh header
 * still trade r28/r30 and the atomic RpAtomicSetGeometry returns takes a register of its own
 * (21 rows in about 250 shapes: declaration orders, block scopes, inline helpers).
 */
#include <dolphin/types.h>
#include "Game/rw/r2.h"

extern s32 lbl_805F1448; /* offset of the R2PTank pointer in RpAtomic (the plugin file's) */
extern s32 lbl_805F14F0; /* offset of R2MeshGlobals in the engine globals */

R2Matrix *RwFrameGetLTM(void *frame);
void *fn_8046335C(u32 pluginId, u32 type, void *a, void *b, void *instance, void *render);
void fn_8049B800(void *pipeline);
R2VertexDesc *fn_8046301C(void);
void fn_80462F7C(R2VertexDesc *desc, u32 type, u32 frac);
void fn_80462F88(R2VertexDesc *desc, u32 type, u32 frac);
void fn_80462FA4(R2VertexDesc *desc, u32 type);
void fn_80462F94(R2VertexDesc *desc, u32 set, u32 type, u32 frac);
void fn_80463104(R2Geometry *geometry, R2VertexDesc *desc);
void fn_804630C0(R2VertexDesc *desc);
void fn_80463428(void *a, void *b);
R2Geometry *fn_80458D24(f32 radius);
R2Geometry *fn_8045905C(R2Geometry *geometry, void *out, R2Material *material);
void fn_8045985C(R2Geometry *geometry);
R2Material *fn_8045B13C(void);
void fn_8045B1F4(R2Material *material);
R2MeshHeader *fn_8045BF30(u32 size);
u16 fn_8045CA20(void);
R2Atomic *fn_80456F0C(R2Atomic *atomic, R2Geometry *geometry, u32 flags);

/* The vertex writers of RpPTankGameCubeRender.c. */
void fn_8044920C(void);
void fn_804495D0(void);
void fn_80449994(void);
void fn_80449D5C(void);
void fn_8044A168(void);
void fn_8044A540(void);
void fn_8044A93C(void);
void fn_8044ADA8(void);
void fn_8044B210(void);
void fn_8044B654(void);

void *fn_80448CB0(void *object, s32 *state);
void *fn_80448D04(void *object, s32 *state);
void fn_80448658(R2Atomic *atomic);

static void *ptankPipeline;

/* Creates the tank's pipeline. */
void *fn_804485BC(void *instance) {
    ptankPipeline = fn_8046335C(0x12F, 0, NULL, NULL, (void *)fn_80448CB0, (void *)fn_80448D04);
    return instance;
}

/* Destroys the tank's pipeline. */
void *fn_80448610(void *instance) {
    if (ptankPipeline != NULL) {
        fn_8049B800(ptankPipeline);
        ptankPipeline = NULL;
    }
    return instance;
}

/* Picks the vertex writer for the tank's data flags and packs the flags the writer tests. */
void fn_80448658(R2Atomic *atomic) {
    R2PTank *tank = R2_PLUGIN(R2PTank, atomic, lbl_805F1448);
    u32 mask = 0;

    tank->unk24 = NULL;
    tank->submit = NULL;
    tank->unk2C = NULL;
    tank->unk30 = NULL;
    tank->unk34 = NULL;
    tank->unk38 = NULL;

    if (tank->dataFlags & 0x8) {
        if (tank->dataFlags & 0x1000000) {
            tank->submit = (void *)fn_804495D0;
            mask |= 0x2;
        } else {
            tank->submit = (void *)fn_8044920C;
            mask |= 0x1;
        }
    } else if (tank->dataFlags & 0x8000) {
        if (tank->dataFlags & 0x1000000) {
            tank->submit = (void *)fn_80449D5C;
            mask |= 0x8;
        } else {
            tank->submit = (void *)fn_80449994;
            mask |= 0x4;
        }
    } else if (tank->dataFlags & 0x20) {
        if (tank->dataFlags & 0x4) {
            if (tank->dataFlags & 0x1000000) {
                tank->submit = (void *)fn_8044B654;
                mask |= 0x200;
            } else {
                tank->submit = (void *)fn_8044B210;
                mask |= 0x100;
            }
        } else {
            if (tank->dataFlags & 0x1000000) {
                tank->submit = (void *)fn_8044ADA8;
                mask |= 0x80;
            } else {
                tank->submit = (void *)fn_8044A93C;
                mask |= 0x40;
            }
        }
    } else if (tank->dataFlags & 0x4) {
        if (tank->dataFlags & 0x1000000) {
            tank->submit = (void *)fn_8044A540;
            mask |= 0x20;
        } else {
            tank->submit = (void *)fn_8044A168;
            mask |= 0x10;
        }
    } else {
        if (tank->dataFlags & 0x1000000) {
            tank->submit = (void *)fn_80449D5C;
            mask |= 0x8;
        } else {
            tank->submit = (void *)fn_80449994;
            mask |= 0x4;
        }
    }

    if (tank->dataFlags & 0x80) {
        mask |= 0x100000;
    } else if (tank->dataFlags & 0x100) {
        mask |= 0x200000;
    } else if (tank->dataFlags & 0x80000) {
        mask |= 0x400000;
    } else if (tank->dataFlags & 0x100000) {
        mask |= 0x800000;
    }

    if (tank->dataFlags & 0x2) {
        mask |= 0x10000;
    } else if (tank->dataFlags & 0x40) {
        mask |= 0x20000;
    } else if (tank->dataFlags & 0x40000) {
        mask |= 0x40000;
    } else {
        mask |= 0x80000;
    }

    if (tank->dataFlags & 0x10) {
        mask |= 0x1000000;
    }
    tank->submitMask = mask;
}

/* Gives a new tank atomic its one-vertex geometry, vertex descriptor, material and mesh. */
u32 fn_80448850(R2Atomic *atomic, s32 maxParticles, u32 unused, u32 dataFlags) {
    u32 local[2];
    R2SurfaceProperties surface;
    R2MeshHeader *header;
    R2Mesh *mesh;
    R2Material *material;
    R2Geometry *geometry;
    R2VertexDesc *desc;
    u32 geometryFlags;

    desc = fn_8046301C();
    geometryFlags = 0x2;
    fn_80462F7C(desc, 4, 0);
    if ((dataFlags & 0x10) == 0x10) {
        geometryFlags |= 0x30;
        fn_80462F88(desc, 4, 0);
    }
    if (dataFlags & 0x40042) {
        geometryFlags |= 0x8;
        fn_80462FA4(desc, 5);
    }
    if (dataFlags & 0x180180) {
        geometryFlags |= 0x4;
        fn_80462F94(desc, 1, 4, 0);
    }

    geometry = fn_80458D24(1.0f);
    geometry->flags = geometryFlags;
    geometry->numVertices = 1;
    fn_80463104(geometry, desc);
    fn_804630C0(desc);

    material = fn_8045B13C();
    surface.ambient = 0.3f;
    surface.specular = 1.0f;
    surface.diffuse = 1.0f;
    material->surfaceProps = surface;
    fn_8045905C(geometry, local, material);

    header = fn_8045BF30(sizeof(R2MeshHeader) + sizeof(R2Mesh));
    memset(header, 0, sizeof(R2MeshHeader) + sizeof(R2Mesh));
    header->flags = (header->flags & ~0xFF) |
                    ((R2MeshGlobals *)((u8 *)lbl_805F15B4 + lbl_805F14F0))->meshFlagsDefault;
    mesh = (R2Mesh *)(header + 1);
    mesh->indices = (u16 *)mesh;
    mesh->material = material;
    header->numMeshes = 1;
    header->serialNum = fn_8045CA20();
    header->firstMeshOffset = 0;
    header->totalIndicesInMesh = 0;
    geometry->mesh = header;

    atomic = fn_80456F0C(atomic, geometry, 0);
    fn_8045985C(geometry);
    fn_8045B1F4(material);
    fn_80448658(atomic);
    atomic->pipeline = ptankPipeline;
    return 1;
}

u32 fn_80448A1C(void) {
    return 1;
}

/* The tank's render callback: builds the two screen axes of the quads, sets the blend states and
 * calls the pipeline. */
u32 fn_80448A24(R2Atomic *atomic) {
    u32 srcBlend;
    u32 dstBlend;
    R2V3d axes[2];
    R2PTank *tank = R2_PLUGIN(R2PTank, atomic, lbl_805F1448);
    s32 flags = tank->dataFlags;

    if ((flags & 0x8000) == 0x8000) {
        axes[0] = tank->cMatrix.right;
        axes[1] = tank->cMatrix.up;
    } else {
        R2Matrix *ltm = RwFrameGetLTM(lbl_805F15B4->curCamera->parent);
        if ((flags & 0x20000) == 0x20000) {
            f32 s = (f32)sin(tank->cRotate);
            f32 c = (f32)cos(tank->cRotate);
            f32 ns = -s;
            axes[0].x = c * ltm->right.x + ns * ltm->up.x;
            axes[0].y = c * ltm->right.y + ns * ltm->up.y;
            axes[0].z = c * ltm->right.z + ns * ltm->up.z;
            axes[1].x = s * ltm->right.x + c * ltm->up.x;
            axes[1].y = s * ltm->right.y + c * ltm->up.y;
            axes[1].z = s * ltm->right.z + c * ltm->up.z;
        } else {
            axes[0] = ltm->right;
            axes[1] = ltm->up;
        }
    }
    axes[1].x = -1.0f * axes[1].x;
    axes[1].y = -1.0f * axes[1].y;
    axes[1].z = -1.0f * axes[1].z;
    tank->axes = axes;

    if (tank->vertexAlphaBlend) {
        lbl_805F15B4->renderStateGet(10, &srcBlend);
        lbl_805F15B4->renderStateSet(10, (void *)tank->srcBlend);
        lbl_805F15B4->renderStateGet(11, &dstBlend);
        lbl_805F15B4->renderStateSet(11, (void *)tank->dstBlend);
        lbl_805F15B4->renderStateSet(12, (void *)1);
    } else {
        lbl_805F15B4->renderStateSet(12, (void *)0);
    }

    tank->render(atomic);

    if (tank->vertexAlphaBlend) {
        lbl_805F15B4->renderStateSet(10, (void *)srcBlend);
        lbl_805F15B4->renderStateSet(11, (void *)dstBlend);
    }
    return 0;
}

/* The pipeline's instance callback. */
void *fn_80448CB0(void *object, s32 *state) {
    if ((state[2] & 0x10) == 0x10) {
        fn_80463428(object, state);
    } else {
        state[8] = 0;
        state[7] = 0;
        state[9] = 0;
    }
    return object;
}
