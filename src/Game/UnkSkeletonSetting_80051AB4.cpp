/* A setting next to the RenderWare skeleton (meaning unknown; default 0x20), which
 * Game/UnkSkeletonWrappers_80049AF4.cpp resets and sets through these two functions. Its own unit:
 * .sdata pads the skeleton's s_defaultVideoMode (Game/RwSkeleton.cpp) to 8 bytes before it. The
 * file name and the names are inferred. Definitions are in reverse address order (deferred
 * inlining emits functions last-defined first). */
#include <dolphin/types.h>

int lbl_805E39C8 = 0x20;

extern "C" void fn_80051AC0(int value) {
    lbl_805E39C8 = value;
}

extern "C" void fn_80051AB4(void) {
    lbl_805E39C8 = 0x20;
}
