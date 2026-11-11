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

char *fn_80482DBC(char *text, char value) {
    char *found = NULL;
    char current;
    do {
        current = *text;
        if (current == value) {
            found = text;
            break;
        }
        text++;
    } while (current != 0);
    return found;
}

char *fn_80482D3C(char *text) {
    if (text == NULL) {
        return text;
    }
    char *cursor = text;
    while (*cursor != 0) {
        char current = *cursor;
        if (current >= 'a' && current <= 'z') {
            *cursor = current - 0x20;
        }
        cursor++;
    }
    return text;
}

char *fn_80482D7C(char *text) {
    if (text == NULL) {
        return text;
    }
    char *cursor = text;
    while (*cursor != 0) {
        char current = *cursor;
        if (current >= 'A' && current <= 'Z') {
            *cursor = current + 0x20;
        }
        cursor++;
    }
    return text;
}

extern u32 lbl_805FB2D0;
extern u8 *lbl_805F15B4;

u32 fn_80482F64(u32 argument) {
    if (argument == 0) {
        argument = (u32)&lbl_805FB2D0;
    }
    u32 result = ((u32 (*)(u32))*(u32 *)(lbl_805F15B4 + 0xF4))(argument);
    return (result + 4) & ~3;
}
}
