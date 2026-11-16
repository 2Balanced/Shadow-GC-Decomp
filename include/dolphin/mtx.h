#ifndef DOLPHIN_MTX_H
#define DOLPHIN_MTX_H

/* The parts of the Dolphin SDK matrix library the decompiled SDK units use. A Mtx is a 3x4
 * row-major affine matrix, a Mtx44 a full 4x4 projection matrix. */

#include <dolphin/types.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef f32 Mtx[3][4];
typedef f32 Mtx44[4][4];
typedef f32 (*MtxPtr)[4];

void fn_803788F8(Mtx m);
void fn_80378924(const Mtx a, const Mtx b, Mtx ab);
void fn_803789F0(Mtx m, f32 xS, f32 yS, f32 zS);
void fn_80378A18(Mtx44 m, f32 t, f32 b, f32 l, f32 r, f32 n, f32 f);

#ifdef __cplusplus
}
#endif

#endif
