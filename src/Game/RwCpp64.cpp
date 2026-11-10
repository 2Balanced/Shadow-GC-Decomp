/* RenderWare small callbacks and accessors. Names are inferred from behavior. */
#include <dolphin/types.h>

extern u8 lbl_8056EC6C[];
extern u8 lbl_8056ECC0[];

extern "C" {
u8 *fn_8049FB0C(void) {
    return lbl_8056EC6C;
}

u8 *fn_804A0760(void) {
    return lbl_8056ECC0;
}
}
