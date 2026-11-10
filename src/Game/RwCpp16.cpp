/* RenderWare small callbacks and accessors. Names are inferred from behavior. */
#include <dolphin/types.h>

extern "C" {
void fn_80453818(void) {
}

void fn_8045381C(void) {
}

u32 fn_80452C78(u32 *object) {
    return object[0];
}

u32 fn_80452C80(u32 *object) {
    return object[1];
}

u32 fn_80452C88(u32 *object, u32 index) {
    return ((u32 *)object[3])[index];
}
}
