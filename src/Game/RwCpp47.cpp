/* RenderWare small callbacks and accessors. Names are inferred from behavior. */
#include <dolphin/types.h>

extern "C" {
void fn_804854B4(u32 *object) {
    ((void (*)(void))object[7])();
}

void fn_804854DC(u32 *object) {
    ((void (*)(void))object[6])();
}
}
