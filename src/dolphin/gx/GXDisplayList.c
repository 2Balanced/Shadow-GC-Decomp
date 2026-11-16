/* Dolphin SDK GX: display lists. Only the call is linked: it sends pending state, then tells the
 * graphics processor to run a list of commands stored in main memory. */
#include <dolphin/gx/GXPrivate.h>

void GXCallDisplayList(void *list, u32 nbytes) {
    if (gx->dirtyState) {
        __GXSetDirtyState();
    }
    if (*(u32 *)&gx->vNumNot == 0) {
        __GXSendFlushPrim();
    }
    GX_WRITE_U8(0x40);
    GX_WRITE_U32(list);
    GX_WRITE_U32(nbytes);
}
