/* RenderWare PowerPipe: the render state vector (RxRenderStateVector), 0x8049F2A4..0x8049F3F8.
 * Only RxRenderStateVectorSetDefaultRenderStateVector is linked.  Its .sdata2 is the opaque white
 * of the default border and fog colours, padded to 8 bytes (0x805FB430..0x805FB438): the heap
 * before it has no data and the file after it none either.  The default vector itself lives in
 * the PowerPipe module's engine globals (offset lbl_805F1690, defined in range r7).
 * Status: fn_8049F2A4 differs only in where the scheduler puts the colour load (3 rows). */
#include <dolphin/types.h>
#include "Game/rw/r8.h"

extern s32 lbl_805F1690; /* offset of the PowerPipe module's globals in RwEngineInstance */

typedef struct RxPipelineGlobals {
    u32 unk0;
    RxRenderStateVector defaultRenderState; /* 0x04 */
} RxPipelineGlobals;

#define RXPIPELINEGLOBALS() ((RxPipelineGlobals *)((u8 *)RwEngineInstance + lbl_805F1690))

/* RxRenderStateVectorSetDefaultRenderStateVector */
RxRenderStateVector *fn_8049F2A4(RxRenderStateVector *rsvp) {
    if (rsvp != NULL) {
        if (RwEngineInstance->engineStatus == 3) {
            *rsvp = RXPIPELINEGLOBALS()->defaultRenderState;
            return rsvp;
        }
        if (rsvp != &RXPIPELINEGLOBALS()->defaultRenderState) {
            RWERROR(1, (0x80000018));
            return NULL;
        }
        {
            RwRGBA white = {255, 255, 255, 255};
            rsvp->flags = 7;
            rsvp->shadeMode = 2;
            rsvp->srcBlend = 5;
            rsvp->destBlend = 6;
            rsvp->textureRaster = NULL;
            rsvp->addressModeU = 1;
            rsvp->addressModeV = 1;
            rsvp->filterMode = 2;
            rsvp->borderColor = white;
            rsvp->fogType = 0;
            rsvp->fogColor = white;
        }
        return rsvp;
    }
    RWERROR(1, (0x80000016));
    return NULL;
}
