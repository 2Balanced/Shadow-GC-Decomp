/* RenderWare error module, 0x8047DB38..0x8047DC90: the module's open and close (engine plugin
 * 0x40F), RwErrorSet, RwErrorGet and _rwerror.
 *
 * One source file: its .sbss is the module's RwModuleInfo (0x805F1558).
 *
 * Written from the retail code; the fn_ symbols are kept because game code calls them by address.
 * C, built with cflags_rw_r5 (-O4,p, no stmw/lmw, automatic inlining). */
#include "Game/rw/r5.h"

/* The error module's bookkeeping. */
R5ModuleInfo lbl_805F1558;

/* Opens the error module: no error yet. */
void *fn_8047DB38(void *instance, s32 offset) {
    lbl_805F1558.globalsOffset = offset;
    lbl_805F1558.numInstances++;
    ((RwError *)((u8 *)lbl_805F15B4 + offset))->pluginID = 0;
    ((RwError *)((u8 *)lbl_805F15B4 + lbl_805F1558.globalsOffset))->errorCode = 0x80000000;
    return instance;
}

/* Closes it. */
void *fn_8047DB70(void *instance) {
    lbl_805F1558.numInstances--;
    return instance;
}

#define R5ERRORGLOBAL ((RwError *)((u8 *)lbl_805F15B4 + lbl_805F1558.globalsOffset))

/* RwErrorSet: keeps the first error until it is read. */
RwError *fn_8047DB84(RwError *code) {
    if (R5ERRORGLOBAL->pluginID == 0 && R5ERRORGLOBAL->errorCode == 0x80000000) {
        if (code->errorCode & 0x80000000) {
            R5ERRORGLOBAL->pluginID = 0;
        } else {
            R5ERRORGLOBAL->pluginID = code->pluginID;
        }
        R5ERRORGLOBAL->errorCode = code->errorCode;
    }
    return code;
}

/* RwErrorGet: the error, which is cleared. */
RwError *fn_8047DBE4(RwError *code) {
    *code = *R5ERRORGLOBAL;
    R5ERRORGLOBAL->pluginID = 0;
    R5ERRORGLOBAL->errorCode = 0x80000000;
    return code;
}

/* _rwerror: the error code (the arguments are only used by debug builds). */
s32 fn_8047DC28(s32 code, ...) {
    R5VaList ap;

    __builtin_va_info(&ap);
    return code;
}
