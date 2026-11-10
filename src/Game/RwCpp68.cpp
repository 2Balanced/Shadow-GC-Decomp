/* RenderWare small callbacks and accessors. Names are inferred from behavior. */
#include <dolphin/types.h>

extern "C" {
void fn_804A616C(void) {
}

u8 *fn_804A60D0(u8 *object) {
    return object + 0x30;
}

void fn_804A5A94(void);
void fn_804A560C(void);
void fn_804A55CC(void);
void fn_804A912C(u32, void *, void *, void *, u32);

void fn_804A5F08(void) {
    fn_804A912C(6, fn_804A5A94, fn_804A560C, fn_804A55CC, 0);
}
}
