/* RenderWare small callbacks and accessors. Names are inferred from behavior. */
#include <dolphin/types.h>

extern u32 lbl_805F169C;

extern "C" {
void fn_8049B148(void) {
}

u32 fn_8049B140(void) {
    return 1;
}

u32 fn_8049B66C(void) {
    return lbl_805F169C;
}

void fn_8049B1F8(void);

void *fn_8049B110(void *argument) {
    fn_8049B1F8();
    return argument;
}
}
