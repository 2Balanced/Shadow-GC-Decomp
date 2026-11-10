/* RenderWare small callbacks and accessors. Names are inferred from behavior. */
#include <dolphin/types.h>

extern u32 lbl_805F16AC;

extern "C" {
u32 fn_804A2064(u8 *object) {
    return *(u32 *)(object + lbl_805F16AC);
}
}
