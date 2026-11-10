/* RenderWare small callbacks and accessors. Names are inferred from behavior. */
#include <dolphin/types.h>

extern u32 lbl_805E1950[];

extern "C" {
u32 fn_80473528(u32 value) {
    lbl_805E1950[0x210] = value;
    return 1;
}
}
