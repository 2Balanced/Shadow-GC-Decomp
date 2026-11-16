/* Dolphin SDK MTX: 3x4 matrix helpers written with the Gekko's paired-single instructions. A
 * paired-single store writes two floats at once, so a row of four is two stores. The game links
 * only the identity, product and scale builders; the rest of the library was dead-stripped. The
 * names stay dtk's because nothing in the binary names them. */
#include <dolphin/mtx.h>

/* {0, 1}: the bottom row's last two columns, read as one pair by the product below. */
static f32 Unit01[] = {0.0f, 1.0f};

/* Never called, and dropped by the linker: naming 1.0 before 0.0 here makes the compiler pool the
 * two constants in retail's order. */
void MTXPoolOrder(f32 *p) {
    p[0] = 1.0f;
    p[1] = 0.0f;
}

/* m = identity. Pairs (0,0), (1,0) and (0,1) cover every pair of a row. */
void fn_803788F8(register Mtx m) {
    register f32 zero = 0.0f;
    register f32 one = 1.0f;
    register f32 zeroOne;
    register f32 oneZero;

    asm {
        psq_st zero, 8(m), 0, 0
        ps_merge01 zeroOne, zero, one
        psq_st zero, 24(m), 0, 0
        ps_merge10 oneZero, one, zero
        psq_st zero, 32(m), 0, 0
        psq_st zeroOne, 16(m), 0, 0
        psq_st oneZero, 0(m), 0, 0
        psq_st oneZero, 40(m), 0, 0
    }
}

/* ab = a * b, treating both as 4x4 with an implied bottom row of (0, 0, 0, 1). `ab` may alias
 * either input: every row of `a` is read before the matching row of `ab` is written. */
asm void fn_80378924(const register Mtx a, const register Mtx b, register Mtx ab) {
    nofralloc
    stwu r1, -64(r1)
    psq_l f0, 0(a), 0, 0
    stfd f14, 8(r1)
    psq_l f6, 0(b), 0, 0
    lis r6, Unit01@ha
    psq_l f7, 8(b), 0, 0
    stfd f15, 16(r1)
    addi r6, r6, Unit01@l
    stfd f31, 40(r1)
    psq_l f8, 16(b), 0, 0
    ps_muls0 f12, f6, f0
    psq_l f2, 16(a), 0, 0
    ps_muls0 f13, f7, f0
    psq_l f31, 0(r6), 0, 0
    ps_muls0 f14, f6, f2
    psq_l f9, 24(b), 0, 0
    ps_muls0 f15, f7, f2
    psq_l f1, 8(a), 0, 0
    ps_madds1 f12, f8, f0, f12
    psq_l f3, 24(a), 0, 0
    ps_madds1 f14, f8, f2, f14
    psq_l f10, 32(b), 0, 0
    ps_madds1 f13, f9, f0, f13
    psq_l f11, 40(b), 0, 0
    ps_madds1 f15, f9, f2, f15
    psq_l f4, 32(a), 0, 0
    psq_l f5, 40(a), 0, 0
    ps_madds0 f12, f10, f1, f12
    ps_madds0 f13, f11, f1, f13
    ps_madds0 f14, f10, f3, f14
    ps_madds0 f15, f11, f3, f15
    psq_st f12, 0(ab), 0, 0
    ps_muls0 f2, f6, f4
    ps_madds1 f13, f31, f1, f13
    ps_muls0 f0, f7, f4
    psq_st f14, 16(ab), 0, 0
    ps_madds1 f15, f31, f3, f15
    psq_st f13, 8(ab), 0, 0
    ps_madds1 f2, f8, f4, f2
    ps_madds1 f0, f9, f4, f0
    ps_madds0 f2, f10, f5, f2
    lfd f14, 8(r1)
    psq_st f15, 24(ab), 0, 0
    ps_madds0 f0, f11, f5, f0
    psq_st f2, 32(ab), 0, 0
    ps_madds1 f0, f31, f5, f0
    lfd f15, 16(r1)
    psq_st f0, 40(ab), 0, 0
    lfd f31, 40(r1)
    addi r1, r1, 64
    blr
}

/* m = scale by (xS, yS, zS). */
void fn_803789F0(register Mtx m, register f32 xS, register f32 yS, register f32 zS) {
    register f32 zero = 0.0f;

    asm {
        stfs xS, 0(m)
        psq_st zero, 4(m), 0, 0
        psq_st zero, 12(m), 0, 0
        stfs yS, 20(m)
        psq_st zero, 24(m), 0, 0
        psq_st zero, 32(m), 0, 0
        stfs zS, 40(m)
        stfs zero, 44(m)
    }
}
