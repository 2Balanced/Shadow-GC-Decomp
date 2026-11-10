/* RenderWare small callbacks and accessors. Names are inferred from behavior. */
#include <dolphin/types.h>

extern "C" {
u32 fn_8049D408(u32 *object, u32 value) {
    u32 count = object[0];
    if (count != 0) {
        u32 *entries = (u32 *)object[3];
        for (u32 i = 0; i < count; i++) {
            if (entries[0] == value) {
                return (u32)entries;
            }
            entries += 9;
        }
    }
    return 0;
}
}
