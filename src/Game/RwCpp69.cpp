/* RenderWare small callbacks and accessors. Names are inferred from behavior. */
#include <dolphin/types.h>

extern "C" {
void fn_804A871C(u8 *base, u32 offset) {
    *(u32 *)(base + offset) = 0;
}
}
