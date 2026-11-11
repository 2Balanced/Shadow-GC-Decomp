/* RenderWare small callbacks and accessors. Names are inferred from behavior. */
#include <dolphin/types.h>

extern u32 lbl_805F14F8;

extern "C" {
u32 fn_8045F2A0(u32 object) {
    (&lbl_805F14F8)[1]++;
    return object;
}

u32 fn_8045F2B4(u32 object) {
    (&lbl_805F14F8)[1]--;
    return object;
}

extern u8 *lbl_805F15B4;
u32 fn_8045F2C8(void);
u32 fn_8045C26C(u32 *object);
u32 *fn_8045EF64(u32 *object, u32 value);
u32 fn_804566D0(u32 *node);
s32 fn_804856C8(u32, u32);

extern u32 lbl_8056D868[];

u32 *fn_8045F1FC(u32 *object, u32 mode) {
    if (object == NULL) {
        goto done;
    }
    if (object[1] == 0) {
        fn_8045C26C(object);
        return (u32 *)lbl_8056D868;
    }
    u32 *result;
    u32 *(*callback)(u32 *, u32);
    u32 argument;
    if ((mode & 1) != 0) {
        callback = (u32 *(*)(u32 *, u32))lbl_8056D868[4];
        argument = lbl_8056D868[5];
    } else {
        callback = fn_8045EF64;
        argument = 0;
    }
    result = callback(object, argument);
    if (result != NULL) {
        fn_8045C26C(object);
        return result;
    }
    return NULL;
done:
    return object;
}

u32 *fn_8045F390(u32 *object) {
    if (fn_8045F2C8() == 0) {
        goto done;
    }
    u32 *node;
    u32 *sentinel = object + 14;
    u32 *current = (u32 *)object[14];
    while (current != sentinel) {
        node = (u32 *)current[2];
        if ((((u8 *)node)[2] & 4) != 0) {
            if (*(u16 *)((u8 *)node + 96) != *(u16 *)(lbl_805F15B4 + 8)) {
                u32 result = fn_804566D0(node);
                if (fn_804856C8(*(u32 *)lbl_805F15B4, result) != 0) {
                    ((void (*)(u32 *))*(u32 *)((u8 *)node + 72))(node);
                }
            }
            *(u16 *)((u8 *)node + 96) = *(u16 *)(lbl_805F15B4 + 8);
        }
        current = (u32 *)current[0];
    }
done:
    return object;
}

u32 fn_8045F374(u32 value, u32 *object) {
    if (object[0] == value) {
        object[1] = 1;
    }
    return 0;
}

void fn_8045E904(u32, u32, u32);

void fn_8045EF3C(u32 first) {
    fn_8045E904(first, 0, 1);
}

u32 fn_8045F2C8(void) {
    u32 *table = (u32 *)lbl_805F15B4;
    table = (u32 *)table[1];
    return ((u32 (*)(void))table[0x1A])();
}

extern u8 lbl_8056D880[];
void fn_804815F0(u8 *, u32, u32, u32, u32);

void fn_8045F338(u32 first, u32 second, u32 third, u32 fourth) {
    fn_804815F0(lbl_8056D880, first, second, third, fourth);
}

void fn_80481DC8(u8 *, u32, u32, u32, u32, u32);

void fn_8045F2F8(u32 first, u32 second, u32 third, u32 fourth, u32 fifth) {
    fn_80481DC8(lbl_8056D880, first, second, third, fourth, fifth);
}
}
