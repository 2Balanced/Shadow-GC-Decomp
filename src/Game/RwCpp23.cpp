/* RenderWare small callbacks and accessors. Names are inferred from behavior.
 * fn_8045EF64 implements the full grouped-object factory; loop register allocation and group
 * initialization differ from retail. */
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

extern u32 lbl_805F14F0;
void fn_803AB44C(u32 **, u32 *, u32);
s32 fn_8045CA38(u32 **left_argument, u32 **right_argument);
u32 *fn_8045BF30(u32 size);
u32 *fn_8045C4E4(u32 *object, u32 (*callback)(u8 *, u32 *, u32), u32 argument);
u32 *fn_8045CBD4(u32 *object);
u32 fn_8047DC28(u32 code, ...);
void fn_8047DB84(u32 *error);

u32 *fn_8045EF64(u32 *input, u32) {
    u32 count = input[1];
    u32 *array = ((u32 *(*)(u32, u32))*(u32 *)(lbl_805F15B4 + 0x108))(count * 4, 0x00010502);
    if (array == NULL) {
        goto initial_error;
    }
    u32 i = 0;
    u32 offset = 0;
    u32 *destination = array;
    do {
        destination[i] = input[2] + offset;
        i++;
        offset += 20;
    } while (i < count);
    fn_803AB44C((u32 **)array, (u32 *)fn_8045CA38, 4);
    u32 groups = 1;
    if (count >= 2) {
        u32 previous = ((u32 *)array[0])[2];
        for (u32 i = 1; i < count; i++) {
            u32 current = ((u32 *)array[i])[2];
            if (current != previous) {
                previous = current;
                groups++;
            }
        }
    }
    u32 total = groups * 12 + count * 6 + 16;
    u32 *object = fn_8045BF30(total);
    if (object == NULL) {
        ((void (*)(u32 *))*(u32 *)(lbl_805F15B4 + 0x10C))(array);
        u32 error[2];
        error[0] = 2;
        error[1] = fn_8047DC28(0x80000013, total);
        fn_8047DB84(error);
        return NULL;
    }
    object[0] = 0;
    *(u16 *)((u8 *)object + 4) = 1;
    *(s16 *)((u8 *)object + 6) = *(s16 *)(lbl_805F15B4 + lbl_805F14F0);
    object[3] = 0;
    object[2] = count * 3;
    (*(s16 *)(lbl_805F15B4 + lbl_805F14F0))++;
    object[5] = 0;
    object[6] = ((u32 *)array[0])[2];
    u32 *group = object + 4;
    u16 *values = (u16 *)((u8 *)group + groups * 12);
    object[4] = (u32)values;
    for (u32 i = 1; i < count; i++) {
        u32 *record = (u32 *)array[i - 1];
        if (record[2] != group[2]) {
            group[4] = 0;
            group[5] = record[2];
            group += 3;
            (*(u16 *)((u8 *)object + 4))++;
        }
        *(u16 *)values = *(u16 *)record;
        *(u16 *)(values + 1) = *(u16 *)((u8 *)record + 2);
        *(u16 *)(values + 2) = *(u16 *)((u8 *)record + 4);
        values += 3;
        group[1] += 3;
    }
    fn_8045C4E4(object, (u32 (*)(u8 *, u32 *, u32))fn_8045CBD4, 0);
    ((void (*)(u32 *))*(u32 *)(lbl_805F15B4 + 0x10C))(array);
    return object;
initial_error:
    {
        u32 error[2];
        error[0] = 2;
        error[1] = fn_8047DC28(0x80000013, count * 4);
        fn_8047DB84(error);
        return NULL;
    }
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
