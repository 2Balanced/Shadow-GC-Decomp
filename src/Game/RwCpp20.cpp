/* RenderWare 3.7 world plugin: the tail of the clump file, 0x80458C28-0x80458CF8 (left over from the
 * wave 41 re-cut of the size-cut Game/RwCpp20). Three wrappers of the atomic plugin registry (two
 * that set a per-plugin stream callback, one that looks a plugin's offset up) and RpAtomicSetFrame
 * end the clump file whose first function lies in old Game/RwCpp18/RwCpp19 (another agent's
 * range): they use the atomic registry (.data 0x8056D7E0) of that file. They stay here, all
 * matching (the unit links), until the clump file is re-cut and takes them; RpGeometry
 * (RenderWare/RpGeometry.c) starts behind them at 0x80458CF8.
 * Built with cflags_rw_r3 (configure.py). */
#include <dolphin/types.h>
#include "Game/rw/r3.h"

extern RwPluginRegistry lbl_8056D7E0; /* the atomic plugin registry of the clump file */

s32 fn_80481634(RwPluginRegistry *, u32, void *); /* set a plugin's stream callback */
s32 fn_80481670(RwPluginRegistry *, u32, void *); /* set a plugin's stream callback */
s32 fn_80481D98(RwPluginRegistry *, u32);         /* RwPluginRegistryGetPluginOffset */
void fn_8048D7C0(void *);                         /* _rwObjectHasFrameSetFrame */

s32 fn_80458C28(u32 pluginID, void *callback) {
    return fn_80481634(&lbl_8056D7E0, pluginID, callback);
}

s32 fn_80458C5C(u32 pluginID, void *callback) {
    return fn_80481670(&lbl_8056D7E0, pluginID, callback);
}

s32 fn_80458C90(u32 pluginID) {
    return fn_80481D98(&lbl_8056D7E0, pluginID);
}

RwObject *fn_80458CBC(RwObject *object) {
    fn_8048D7C0(object);
    object->privateFlags |= 1;
    return object;
}
