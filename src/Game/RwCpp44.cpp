/* RenderWare small callbacks and accessors. Names are inferred from behavior. */
#include <dolphin/types.h>

extern u32 lbl_805EDEA0;

extern "C" {
void fn_8047FA28(u32 value) {
    lbl_805EDEA0 = value;
}
}
