/* RenderWare small callbacks and accessors. Names are inferred from behavior. */
#include <dolphin/types.h>

extern "C" {
u32 fn_8045B79C(u32 *object, u32 index) {
    return ((u32 *)object[0])[index];
}
}
