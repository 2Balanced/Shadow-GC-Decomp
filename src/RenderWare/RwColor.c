/* RenderWare colour module, 0x8047DB10..0x8047DB38: the module's open and close (engine plugin
 * 0x40D, the colour module, with no globals of its own: they only count the instances). The
 * colour functions themselves are macros or not in the binary.
 *
 * One source file: its .sbss is the module's RwModuleInfo (0x805F1550); the engine registers it
 * between the vector module (0x401) and the matrix module (0x402), and the error module (0x40F)
 * that follows has an RwModuleInfo of its own (0x805F1558).
 *
 * Written from the retail code; the fn_ symbols are kept because RwEngineInit refers to them by
 * address. C, built with cflags_rw_r5. */
#include "Game/rw/r5.h"

/* The colour module's bookkeeping. */
R5ModuleInfo lbl_805F1550;

/* Opens the colour module. */
void *fn_8047DB10(void *instance) {
    lbl_805F1550.numInstances++;
    return instance;
}

/* Closes it. */
void *fn_8047DB24(void *instance) {
    lbl_805F1550.numInstances--;
    return instance;
}
