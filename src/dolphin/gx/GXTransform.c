/* Dolphin SDK GX: transform state.
 *
 * Projection, position/normal/texture matrices, viewport, scissor and clipping. Matrices are
 * copied into the FIFO two floats at a time with paired-single loads and stores. */
#include <dolphin/gx/GXPrivate.h>

/* Copies six floats. */
static inline void Copy6Floats(register const f32 *src, register f32 *dst) {
    register f32 p01;
    register f32 p23;
    register f32 p45;

    asm {
        psq_l p01, 0(src), 0, 0
        psq_l p23, 8(src), 0, 0
        psq_l p45, 16(src), 0, 0
        psq_st p01, 0(dst), 0, 0
        psq_st p23, 8(dst), 0, 0
        psq_st p45, 16(dst), 0, 0
    }
}

/* Sends six floats to a write-gather address. */
static inline void WriteProjPS(register const f32 *src, register volatile void *dst) {
    register f32 p01;
    register f32 p23;
    register f32 p45;

    asm {
        psq_l p01, 0(src), 0, 0
        psq_l p23, 8(src), 0, 0
        psq_l p45, 16(src), 0, 0
        psq_st p01, 0(dst), 0, 0
        psq_st p23, 0(dst), 0, 0
        psq_st p45, 0(dst), 0, 0
    }
}

/* Sends a 3x4 matrix. */
static inline void WriteMTXPS4x3(register const f32 (*src)[4], register volatile void *dst) {
    register f32 a00_a01;
    register f32 a02_a03;
    register f32 a10_a11;
    register f32 a12_a13;
    register f32 a20_a21;
    register f32 a22_a23;

    asm {
        psq_l a00_a01, 0(src), 0, 0
        psq_l a02_a03, 8(src), 0, 0
        psq_l a10_a11, 16(src), 0, 0
        psq_l a12_a13, 24(src), 0, 0
        psq_l a20_a21, 32(src), 0, 0
        psq_l a22_a23, 40(src), 0, 0
        psq_st a00_a01, 0(dst), 0, 0
        psq_st a02_a03, 0(dst), 0, 0
        psq_st a10_a11, 0(dst), 0, 0
        psq_st a12_a13, 0(dst), 0, 0
        psq_st a20_a21, 0(dst), 0, 0
        psq_st a22_a23, 0(dst), 0, 0
    }
}

/* Sends the upper 3x3 of a 3x4 matrix. */
static inline void WriteMTXPS3x3from3x4(register const f32 (*src)[4], register volatile void *dst) {
    register f32 a00_a01;
    register f32 a02;
    register f32 a10_a11;
    register f32 a12;
    register f32 a20_a21;
    register f32 a22;

    asm {
        psq_l a00_a01, 0(src), 0, 0
        lfs a02, 8(src)
        psq_l a10_a11, 16(src), 0, 0
        lfs a12, 24(src)
        psq_l a20_a21, 32(src), 0, 0
        lfs a22, 40(src)
        psq_st a00_a01, 0(dst), 0, 0
        stfs a02, 0(dst)
        psq_st a10_a11, 0(dst), 0, 0
        stfs a12, 0(dst)
        psq_st a20_a21, 0(dst), 0, 0
        stfs a22, 0(dst)
    }
}

/* Sends the first two rows of a matrix. */
static inline void WriteMTXPS4x2(register const f32 (*src)[4], register volatile void *dst) {
    register f32 a00_a01;
    register f32 a02_a03;
    register f32 a10_a11;
    register f32 a12_a13;

    asm {
        psq_l a00_a01, 0(src), 0, 0
        psq_l a02_a03, 8(src), 0, 0
        psq_l a10_a11, 16(src), 0, 0
        psq_l a12_a13, 24(src), 0, 0
        psq_st a00_a01, 0(dst), 0, 0
        psq_st a02_a03, 0(dst), 0, 0
        psq_st a10_a11, 0(dst), 0, 0
        psq_st a12_a13, 0(dst), 0, 0
    }
}

static inline void __GXSetProjection(void) {
    GX_WRITE_U8(0x10);
    GX_WRITE_U32(0x00061020);
    WriteProjPS(gx->projMtx, &GXWGFifo);
    GX_WRITE_U32(gx->projType);
}

void GXSetProjection(const Mtx44 mtx, GXProjectionType type) {
    gx->projType = type;
    gx->projMtx[0] = mtx[0][0];
    gx->projMtx[2] = mtx[1][1];
    gx->projMtx[4] = mtx[2][2];
    gx->projMtx[5] = mtx[2][3];
    if (type == 1) { /* GX_ORTHOGRAPHIC */
        gx->projMtx[1] = mtx[0][3];
        gx->projMtx[3] = mtx[1][3];
    } else {
        gx->projMtx[1] = mtx[0][2];
        gx->projMtx[3] = mtx[1][2];
    }
    __GXSetProjection();
    gx->bpSentNot = GX_TRUE;
}

void GXSetProjectionv(const f32 *ptr) {
    gx->projType = (ptr[0] == 0.0f) ? 0 : 1;
    Copy6Floats(ptr + 1, gx->projMtx);
    __GXSetProjection();
    gx->bpSentNot = GX_TRUE;
}

void GXGetProjectionv(f32 *ptr) {
    ptr[0] = (gx->projType != 0) ? 1.0f : 0.0f;
    Copy6Floats(gx->projMtx, ptr + 1);
}

void GXLoadPosMtxImm(const Mtx mtx, u32 id) {
    u32 reg;
    u32 addr;

    addr = id * 4;
    reg = addr | 0xB0000;
    GX_WRITE_U8(0x10);
    GX_WRITE_U32(reg);
    WriteMTXPS4x3(mtx, &GXWGFifo);
}

void GXLoadNrmMtxImm(const Mtx mtx, u32 id) {
    u32 reg;
    u32 addr;

    addr = id * 3 + 0x400;
    reg = addr | 0x80000;
    GX_WRITE_U8(0x10);
    GX_WRITE_U32(reg);
    WriteMTXPS3x3from3x4(mtx, &GXWGFifo);
}

void GXSetCurrentMtx(u32 id) {
    SET_REG_FIELD(gx->matIdxA, 6, 0, id);
    __GXSetMatrixIndex(0);
}

void GXLoadTexMtxImm(const Mtx mtx, u32 id, GXTexMtxType type) {
    u32 reg;
    u32 addr;
    u32 count;

    if (id >= 64) { /* post-transform matrices */
        addr = (id - 64) * 4 + 0x500;
    } else {
        addr = id * 4;
    }
    count = (type == 1) ? 8 : 12;
    reg = addr | ((count - 1) << 16);
    GX_WRITE_U8(0x10);
    GX_WRITE_U32(reg);
    if (type == 0) {
        WriteMTXPS4x3(mtx, &GXWGFifo);
    } else {
        WriteMTXPS4x2(mtx, &GXWGFifo);
    }
}

void __GXSetViewport(void) {
    f32 sx;
    f32 sy;
    f32 sz;
    f32 ox;
    f32 oy;
    f32 oz;
    f32 zmin;
    f32 zmax;

    sx = gx->vpWd / 2.0f;
    sy = -gx->vpHt / 2.0f;
    ox = 342.0f + (gx->vpLeft + gx->vpWd / 2.0f);
    oy = 342.0f + (gx->vpTop + gx->vpHt / 2.0f);
    zmax = gx->vpFarz * gx->zScale;
    zmin = gx->vpNearz * gx->zScale;
    sz = zmax - zmin;
    oz = zmax + gx->zOffset;

    GX_WRITE_U8(0x10);
    GX_WRITE_U32(0x0005101A);
    GX_WRITE_F32(sx);
    GX_WRITE_F32(sy);
    GX_WRITE_F32(sz);
    GX_WRITE_F32(ox);
    GX_WRITE_F32(oy);
    GX_WRITE_F32(oz);
}

void GXSetViewportJitter(f32 left, f32 top, f32 wd, f32 ht, f32 nearz, f32 farz, u32 field) {
    if (field == 0) {
        top -= 0.5f;
    }
    gx->vpLeft = left;
    gx->vpTop = top;
    gx->vpWd = wd;
    gx->vpHt = ht;
    gx->vpNearz = nearz;
    gx->vpFarz = farz;
    __GXSetViewport();
    gx->bpSentNot = GX_TRUE;
}

void GXSetViewport(f32 left, f32 top, f32 wd, f32 ht, f32 nearz, f32 farz) {
    GXSetViewportJitter(left, top, wd, ht, nearz, farz, 1);
}

void GXSetScissor(u32 left, u32 top, u32 wd, u32 ht) {
    u32 tp;
    u32 lf;
    u32 bm;
    u32 rt;

    tp = top + 342;
    lf = left + 342;
    bm = tp + ht - 1;
    rt = lf + wd - 1;
    SET_REG_FIELD(gx->suScis0, 11, 0, tp);
    SET_REG_FIELD(gx->suScis0, 11, 12, lf);
    SET_REG_FIELD(gx->suScis1, 11, 0, bm);
    SET_REG_FIELD(gx->suScis1, 11, 12, rt);
    GX_WRITE_RAS_REG(gx->suScis0);
    GX_WRITE_RAS_REG(gx->suScis1);
    gx->bpSentNot = GX_FALSE;
}

void GXSetScissorBoxOffset(s32 x_off, s32 y_off) {
    u32 reg;
    u32 hx;
    u32 hy;

    hx = (u32)(x_off + 342) >> 1;
    hy = (u32)(y_off + 342) >> 1;
    reg = 0;
    SET_REG_FIELD(reg, 10, 0, hx);
    SET_REG_FIELD(reg, 10, 10, hy);
    SET_REG_FIELD(reg, 8, 24, 0x59);
    GX_WRITE_RAS_REG(reg);
    gx->bpSentNot = GX_FALSE;
}

void GXSetClipMode(GXClipMode mode) {
    GX_WRITE_XF_REG(5, mode);
    gx->bpSentNot = GX_TRUE;
}

void __GXSetMatrixIndex(GXAttr matIdxAttr) {
    if (matIdxAttr < 5) { /* position and texture matrices 0-3 */
        GX_WRITE_CP_REG(0x30, gx->matIdxA);
        GX_WRITE_XF_REG(0x18, gx->matIdxA);
    } else {
        GX_WRITE_CP_REG(0x40, gx->matIdxB);
        GX_WRITE_XF_REG(0x19, gx->matIdxB);
    }
    gx->bpSentNot = GX_TRUE;
}
