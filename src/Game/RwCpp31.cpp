/* RenderWare small callbacks and accessors. Names are inferred from behavior. */
#include <string.h>
#include <dolphin/types.h>

extern "C" {
void fn_8046B0E0(u32 *metadata, u32 unused, u32 size, u8 *buffer) {
    metadata[0] = (u32)buffer;
    metadata[1] = size;
    memset(buffer, 0, size);
}
}
