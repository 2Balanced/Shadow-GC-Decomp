#ifndef GAME_GADGETUTIL_H
#define GAME_GADGETUTIL_H

#include "Game/GadgetCharInfo.h"
#include "Game/GadgetModelInfo.h"
#include "Game/MathTypes.h"
#include "boost/shared_ptr.hpp"
#include <dolphin/types.h>

/* Helpers that gadget units call. Names and behaviour are inferred from their bodies and callers;
 * the original names are unknown. */
extern "C" {
/* Builds the character info of a gadget type (arguments unknown). */
boost::shared_ptr<GadgetCharInfo> fn_8015CD90(int arg0, int arg1);
/* Loads model modelId for a gadget. */
boost::shared_ptr<GadgetModelInfo> fn_8015F9D8(int arg0, int modelId);
/* Queues model file modelId for loading (the gadget descriptors' load hooks call it). */
void fn_8006B50C(int modelId, int arg1);
/* The sound manager and its play call. */
void *fn_80053164(void);
void fn_801D560C(void *manager, int soundId, int arg2, int arg3);
/* matrix * point. */
Vec3f fn_8040B67C(const Mtx34f *matrix, const Vec3f *point);
/* vector *= scale. */
void fn_8040CE38(Vec3f *vector, float scale);
/* vector = (0, 0, 0). */
void fn_8040CE60(Vec3f *vector);
/* A debug line from start to end, and its draw call (segment is 0x18 bytes of caller stack). */
void *fn_80020A20(void *segment, const Vec3f *start, const Vec3f *end);
void fn_802E0B58(void *segment, u32 *arg1);
}

extern float lbl_805FAD38; /* pi */
extern u32 lbl_805F06D8;   /* passed to the debug line draw call */

/* The vector type's out-of-line copy constructor (fn_800091EC) and assignment (fn_800091D0),
 * called with temporaries the way the original's operators are (names inferred). */
inline void ConstructVector(Vec3f *vector, const Vec3f &value) {
    fn_800091EC(vector, &value);
}

inline void AssignVector(Vec3f &vector, const Vec3f &value) {
    fn_800091D0(&vector, &value);
}

#endif
