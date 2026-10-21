/* fn_80329524: the two-digit number that follows "_CT" in a file name (the event unit at 0x8016952C
 * calls it with the names of its .CEN files). A unit of its own: retail pools the "_CT" string after
 * the int-to-float constant of Game/CenFile.cpp, which GC/2.7 cannot do within one unit (it pools a
 * global initializer's string before any function constant). File name and the names marked "(name
 * inferred)" are not original. Definitions are in reverse address order. */
#include <cstring>

#include <dolphin/types.h>

extern "C" {
/* Allocates size bytes from heap, and frees them (behaviour inferred). */
void *fn_80049B90(u32 size, int heap);
void fn_80049B34(void *p);
/* strstr (behaviour inferred). */
char *fn_803AC6C0(const char *s, const char *sub);
}

/* The tag fn_80329524 looks for (name inferred; the event unit at 0x8016952C reads it too). */
const char *lbl_805ED3D0 = "_CT";

/* A heap copy of name with every '.' after the first character overwritten by a terminator, which
 * cuts the extension off (name inferred). */
inline char *CopyBaseName(const char *name) {
    char *copy;
    if (name != 0) {
        u32 length = strlen(name);
        copy = (char *)fn_80049B90(length + 1, 2);
        strcpy(copy, name);
        for (u32 i = length - 1; i > 0; i--) {
            if (copy[i] == '.') {
                copy[i] = 0;
            }
        }
    } else {
        copy = 0;
    }
    return copy;
}

/* The two-digit number after "_CT" in name's base name, or 0 when there is none (name inferred;
 * the unit at 0x8016952C calls it). */
extern "C" int fn_80329524(const char *name) {
    int number = 0;
    char *copy = CopyBaseName(name);
    char *tag = fn_803AC6C0(copy, lbl_805ED3D0);
    if (tag != 0) {
        tag += strlen(lbl_805ED3D0);
        number = (tag[0] - '0') * 10 + tag[1] - '0';
    }
    fn_80049B34(copy);
    return number;
}
