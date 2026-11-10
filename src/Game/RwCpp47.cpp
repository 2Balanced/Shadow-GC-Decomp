/* RenderWare small callbacks and accessors. Names are inferred from behavior. */
#include <dolphin/types.h>

extern "C" {
void fn_804854B4(u32 *object) {
    ((void (*)(void))object[7])();
}

void fn_804854DC(u32 *object) {
    ((void (*)(void))object[6])();
}

void fn_8048CD8C(void);

void *fn_80483F8C(void *argument) {
    fn_8048CD8C();
    return argument;
}
}
