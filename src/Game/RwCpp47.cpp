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

extern u8 lbl_8056DF70[];
void fn_80481DC8(u8 *, u32, u32, u32, u32, u32);

void fn_804859B8(u32 first, u32 second, u32 third, u32 fourth, u32 fifth) {
    fn_80481DC8(lbl_8056DF70, first, second, third, fourth, fifth);
}
}
