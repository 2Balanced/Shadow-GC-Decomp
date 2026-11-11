/* RenderWare small callbacks and accessors. Names are inferred from behavior. */
#include <dolphin/types.h>

extern "C" {
void fn_8045FB10(void);

void fn_80460120(u32 *object, void (*callback)(void)) {
    if (callback == NULL) {
        callback = fn_8045FB10;
    }
    object[0x1A] = (u32)callback;
}

extern u8 lbl_8056D898[];
extern u32 lbl_805F1508;
extern u8 *lbl_805F15B4;
void fn_804815F0(u8 *, u32, u32, u32, u32);

void fn_804607D0(u32 first, u32 second, u32 third, u32 fourth) {
    fn_804815F0(lbl_8056D898, first, second, third, fourth);
}

void fn_80481DC8(u8 *, u32, u32, u32, u32, u32);

void fn_80460790(u32 first, u32 second, u32 third, u32 fourth, u32 fifth) {
    fn_80481DC8(lbl_8056D898, first, second, third, fourth, fifth);
}

void fn_80460B68(u8 *object) {
    ((u32 *)lbl_805F15B4)[1] = 0;
    u32 *target = (u32 *)(object + lbl_805F1508);
    void (*callback)(void) = (void (*)(void))target[5];
    callback();
}

u32 *fn_8045F390(u32 *object);
void fn_804624E8(u8 *, u32, u32 *, u32);
void fn_80462130(u32 *object);
void fn_8047FCDC(u32 *table);
void fn_80462770(void);
extern u32 lbl_805F1500;

u32 *fn_8045FF5C(u32 *object) {
    fn_804624E8(lbl_805F15B4, (u32)fn_8045F390, object, object[1]);
    return object;
}

void fn_80460B1C(u32 *object) {
    u32 *table = (u32 *)((u8 *)object + lbl_805F1508);
    ((u32 *)lbl_805F15B4)[1] = table[3];
    ((u16 *)lbl_805F15B4)[4]++;
    ((void (*)(void))table[4])();
}

void fn_80460BA4(u32 *object);
void fn_80460B68(u8 *object);

void fn_80460C48(u32 *object) {
    u32 *table = (u32 *)((u8 *)object + lbl_805F1508);
    table[0] = 0;
    table[1] = 0;
    table[2] = 0;
    table[4] = object[6];
    table[5] = object[7];
    table[6] = object[4];
    object[4] = (u32)fn_80460BA4;
    object[6] = (u32)fn_80460B1C;
    object[7] = (u32)fn_80460B68;
    table[3] = 0;
}

u32 *fn_80460CA4(u32 *object) {
    u32 *table = (u32 *)((u8 *)object + lbl_805F1508);
    table[0] = 0;
    table[1] = 0;
    table[2] = 0;
    if (*(u32 *)((u8 *)object + lbl_805F1508 + 12) != 0) {
        fn_80462130(object);
    }
    return object;
}

u32 *fn_8045F9E8(u32 *object) {
    u32 *table = *(u32 **)(lbl_805F15B4 + lbl_805F1500);
    if (table != NULL) {
        fn_8047FCDC(table);
        *(u32 **)(lbl_805F15B4 + lbl_805F1500) = NULL;
    }
    fn_80462770();
    (&lbl_805F1500)[1]--;
    return object;
}

u32 *fn_804606F0(u32 *object, u32 (*callback)(u32 *, u32 *), u32 *argument);
u32 fn_8045F374(u32 value, u32 *object);
extern u32 lbl_805F1518;

u32 *fn_8045FEA0(u32 target) {
    u32 *sentinel = (u32 *)(lbl_805F15B4 + lbl_805F1500 + 4);
    u32 *current = (u32 *)sentinel[0];
    while (current != sentinel) {
        u32 *candidate = (u32 *)current[-2];
        if ((candidate[3] & 1) != 0) {
            if (target < (u32)candidate || target >= (u32)((u8 *)candidate + current[-1])) {
                current = (u32 *)current[0];
                continue;
            }
            return candidate;
        }
        if (fn_804606F0(candidate, (u32 (*)(u32 *, u32 *))fn_8045F374, &target) != 0) {
            return (u32 *)current[-2];
        }
        current = (u32 *)current[0];
    }
    return NULL;
}

u32 fn_80460D80(u32 *object) {
    ((u32 *)object[4])[0] = object[3];
    ((u32 *)object[3])[1] = object[4];
    ((u32 *)object[1])[0] = object[0];
    ((u32 *)object[0])[1] = object[1];
    ((void (*)(u32 *, u32 *))*(u32 *)(lbl_805F15B4 + 0x11C))(
        *(u32 **)(lbl_805F15B4 + lbl_805F1518 + 4), object);
    return 1;
}

u32 fn_80460DF0(u32 *object) {
    if (object[2] != 0 && object[5] != 0) {
        ((u32 *)object[4])[0] = object[3];
        ((u32 *)object[3])[1] = object[4];
        ((u32 *)object[1])[0] = object[0];
        ((u32 *)object[0])[1] = object[1];
        ((void (*)(u32 *, u32 *))*(u32 *)(lbl_805F15B4 + 0x11C))(
            *(u32 **)(lbl_805F15B4 + lbl_805F1518), object);
    }
    return 1;
}

u32 *fn_8046066C(u32 *object, u32 (*callback)(u32 *, u32 *), u32 *argument) {
    s32 count = object[5];
    for (s32 i = 0; i < count; i++) {
        if (callback(((u32 **)object[4])[i], argument) == 0) {
            return object;
        }
    }
    return object;
}

u32 *fn_804606F0(u32 *object, u32 (*callback)(u32 *, u32 *), u32 *argument) {
    u32 buffer[62];
    u32 *node = (u32 *)object[7];
    s32 index = 0;
    do {
        if (*(s32 *)node < 0) {
            if (callback(node, argument) == 0) {
                return object;
            }
            node = (u32 *)buffer[index--];
        } else {
            buffer[++index] = node[3];
            node = (u32 *)node[2];
        }
    } while (index >= 0);
    return object;
}
}
