/* RenderWare small callbacks and accessors. Names are inferred from behavior. */
#include <dolphin/types.h>

extern "C" {
void fn_804A871C(u8 *base, u32 offset) {
    *(u32 *)(base + offset) = 0;
}

extern u32 lbl_805EDF68;
void fn_804A9158(void);
void fn_804A9274(void);

void *fn_804A869C(void *argument) {
    (&lbl_805EDF68)[1]++;
    fn_804A9158();
    return argument;
}

void *fn_804A86DC(void *argument) {
    fn_804A9274();
    (&lbl_805EDF68)[1]--;
    return argument;
}
}
