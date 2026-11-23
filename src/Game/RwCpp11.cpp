/* RenderWare: RpPTank's GameCube vertex writers, then the start of RpSkin.
 *
 * 0x8044A168-0x8044BBFC. The six functions up to 0x8044BAB4 (fn_8044A168, fn_8044A540,
 * fn_8044A93C, fn_8044ADA8, fn_8044B210 and fn_8044B654) are RpPTank's hand-written paired-single
 * vertex writers (see RwCpp10.cpp) and are not written. RpSkin starts at 0x8044BAB4 with the two
 * split-data functions below; RwCpp12.cpp continues it.
 */
#include <dolphin/types.h>
#include "Game/rw/r2.h"

u32 fn_8047D458(void *stream, const void *data, u32 size);
u32 fn_80480ED0(void *stream, const void *data, u32 size);

/* Frees a skin's split data. */
u32 fn_8044BAB4(R2Skin *skin) {
    if (skin->split.remapIndices != NULL) {
        lbl_805F15B4->memFree(skin->split.remapIndices);
    }
    skin->split.boneLimit = 0;
    skin->split.numMeshes = 0;
    skin->split.numRLE = 0;
    skin->split.remapIndices = NULL;
    skin->split.meshRLECount = NULL;
    skin->split.meshRLE = NULL;
    return 1;
}

/* Writes a skin's split data to a stream. */
void *fn_8044BB18(void *stream, R2Skin *skin) {
    R2SkinSplit *split = &skin->split;

    if (!fn_8047D458(stream, &split->boneLimit, sizeof(u32))) {
        return NULL;
    }
    if (!fn_8047D458(stream, &split->numMeshes, sizeof(u32))) {
        return NULL;
    }
    if (!fn_8047D458(stream, &split->numRLE, sizeof(u32))) {
        return NULL;
    }
    if (skin->split.numMeshes != 0) {
        if (!fn_80480ED0(stream, split->remapIndices,
                         skin->numBones + split->numMeshes * 2 + split->numRLE * 2)) {
            return NULL;
        }
    }
    return stream;
}
