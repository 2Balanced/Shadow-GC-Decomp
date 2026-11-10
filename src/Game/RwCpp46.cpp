/* RenderWare small callbacks and accessors. Names are inferred from behavior. */
#include <dolphin/types.h>

extern "C" {
void fn_80482F60(void) {
}

u32 fn_804829D4(void) {
    return 1;
}

char *fn_80482DF0(char *text, char value) {
    char *found = NULL;
    char current;
    do {
        current = *text;
        if (current == value) {
            found = text;
        }
        text++;
    } while (current != 0);
    return found;
}
}
