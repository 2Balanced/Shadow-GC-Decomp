/* RenderWare small callbacks and accessors. Names are inferred from behavior. */
#include <dolphin/types.h>

extern "C" {
void fn_8047046C(u8 *object, u8 value) {
    object[0x1C] = value;
}
}
