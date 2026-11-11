/* RenderWare small callbacks and accessors. Names are inferred from behavior. */
#include <dolphin/types.h>
#include <string.h>

extern "C" {
u32 *fn_8045FB10(u32 *object);

void fn_80460120(u32 *object, void (*callback)(void)) {
    if (callback == NULL) {
        callback = (void (*)(void))fn_8045FB10;
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
    u32 offset = lbl_805F1508;
    u32 *target = (u32 *)(object + offset);
    ((u32 *)lbl_805F15B4)[1] = 0;
    void (*callback)(u8 *) = (void (*)(u8 *))target[5];
    callback(object);
}

u32 *fn_8045F390(u32 *object);
void fn_804624E8(u8 *, u32, u32 *, u32);
void fn_80462130(u32 *, u32 *, u32);
void fn_8047FCDC(u32 *table);
void fn_80462770(void);
extern u32 lbl_805F1500;

u32 *fn_8045FF5C(u32 *object, u32 unused_a, u32 unused_b, u32 last) {
    fn_804624E8(*(u8 **)lbl_805F15B4, (u32)fn_8045F390, object, last);
    return object;
}

void fn_80460B1C(u32 *object) {
    u8 *engine = lbl_805F15B4;
    u32 offset = lbl_805F1508;
    u32 *table = (u32 *)((u8 *)object + offset);
    ((u32 *)engine)[1] = table[3];
    engine = lbl_805F15B4;
    u32 count = ((u16 *)engine)[4];
    ((u16 *)engine)[4] = count + 1;
    ((void (*)(u32 *, u32))table[4])(object, count);
}

u32 *fn_80460BA4(u32 *object);
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

u32 *fn_80460CA4(u32 *object, u32 *engine) {
    u32 zero = 0;
    u32 offset = lbl_805F1508;
    u32 *table = (u32 *)((u8 *)object + offset);
    table[0] = zero;
    table[1] = zero;
    table[2] = zero;
    u32 condition = *(u32 *)((u8 *)engine + offset + 12);
    if (condition != 0) {
        fn_80462130((u32 *)condition, object, zero);
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
        if (((u8 *)current[-2])[3] & 1) {
            if (target < (u32)current[-2] || target >= (u32)((u8 *)current[-2] + current[-1])) {
                goto advance;
            }
            return (u32 *)current[-2];
        }
        s32 search[2];
        search[0] = (s32)target;
        search[1] = 0;
        fn_804606F0((u32 *)current[-2], (u32 (*)(u32 *, u32 *))fn_8045F374, (u32 *)search);
        if (search[1] != 0) {
            return (u32 *)current[-2];
        }
advance:
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

void fn_80461658(u32 *, u32 *);

u32 *fn_80460BA4(u32 *object) {
    u32 *table = (u32 *)((u8 *)object + lbl_805F1508);
    if (((u32 (*)(u32 *))table[6])(object) != 0) {
        u32 *child = (u32 *)table[3];
        if (child != NULL) {
            void (*notify)(u32, u32, u32) = (void (*)(u32, u32, u32))*(u32 *)(lbl_805F15B4 + 0xA0);
            fn_80461658(child, object);
            notify(0, 0, child[3] == 1 ? 1 : 0);
        }
        return object;
    }
    return NULL;
}

u32 *fn_80460A88(u32 *object) {
    u32 *first = *(u32 **)(lbl_805F15B4 + lbl_805F1518 + 4);
    if (first != NULL) {
        fn_8047FCDC(first);
        *(u32 **)(lbl_805F15B4 + lbl_805F1518 + 4) = NULL;
    }
    u32 *second = *(u32 **)(lbl_805F15B4 + lbl_805F1518);
    if (second != NULL) {
        fn_8047FCDC(second);
        *(u32 **)(lbl_805F15B4 + lbl_805F1518) = NULL;
    }
    (&lbl_805F1518)[1]--;
    return object;
}

u32 *fn_80460D00(u32 *object) {
    u32 *table = (u32 *)((u8 *)object + lbl_805F1508);
    if (table[0] != 0) {
        ((void (*)(u32 *))*(u32 *)(lbl_805F15B4 + 0x10C))((u32 *)table[0]);
    }
    table[0] = 0;
    table[1] = 0;
    table[2] = 0;
    object[6] = table[4];
    object[7] = table[5];
    object[4] = table[6];
    return object;
}

void *fn_8047FC04(u32, u32, u32, u32, u32, u32);
extern u32 lbl_805EDDC8;
extern u32 lbl_805EDDCC;
extern u32 lbl_805EDDD0;
extern u32 lbl_805EDDD4;
extern u8 lbl_805E1640[];
extern u8 lbl_805E1664[];

u32 *fn_80460974(u32 *object, u32 offset) {
    lbl_805F1518 = offset;
    *(u32 **)(lbl_805F15B4 + lbl_805F1518) =
        (u32 *)fn_8047FC04(24, lbl_805EDDC8, 4, lbl_805EDDCC, (u32)lbl_805E1640, 0x40507);
    if (*(u32 **)(lbl_805F15B4 + lbl_805F1518) == NULL) {
        return NULL;
    }
    *(u32 **)(lbl_805F15B4 + lbl_805F1518 + 4) =
        (u32 *)fn_8047FC04(24, lbl_805EDDD0, 4, lbl_805EDDD4, (u32)lbl_805E1664, 0x40507);
    if (*(u32 *)(lbl_805F15B4 + lbl_805F1518 + 4) == 0) {
        fn_8047FCDC(*(u32 **)(lbl_805F15B4 + lbl_805F1518));
        *(u32 **)(lbl_805F15B4 + lbl_805F1518) = NULL;
        return NULL;
    }
    ((u16 *)lbl_805F15B4)[4] = 1;
    *(u32 *)(lbl_805F15B4 + lbl_805F1518 + 8) = 0;
    (&lbl_805F1518)[1]++;
    return object;
}

void fn_8045FCFC(u32 first, u32 second) {
    u32 *node = ((u32 *(*)(u32 *, u32))*(u32 *)(lbl_805F15B4 + 0x118))(
        *(u32 **)(lbl_805F15B4 + lbl_805F1500), 0x40507);
    if (node != NULL) {
        node[0] = first;
        node[1] = second;
        node[2] = *(u32 *)(lbl_805F15B4 + lbl_805F1500 + 4);
        node[3] = (u32)(lbl_805F15B4 + lbl_805F1500 + 4);
        *(u32 *)(*(u32 *)(lbl_805F15B4 + lbl_805F1500 + 4) + 4) = (u32)(node + 2);
        *(u32 **)(lbl_805F15B4 + lbl_805F1500 + 4) = node + 2;
    }
}

u32 *fn_8045C144(u32, u32);
u32 *fn_8045F458(u32 *, u32 *, u32 *, u32 *);

u32 *fn_8045FDB0(u32 *object) {
    volatile u32 buffer[66];
    u32 *node = (u32 *)object[7];
    s32 index = 0;
    if (node == NULL) {
        return NULL;
    }
    do {
        if (*(s32 *)node < 0) {
            u32 *slot = (u32 *)(object[4] + ((u16 *)node)[64] * 4);
            if (node[30] == 0) {
                u32 *value = fn_8045C144(((u16 *)node)[66], object[4]);
                if (value == NULL) {
                    goto value_failed;
                }
                object = fn_8045F458(object, value, node, slot);
                if (object != NULL) {
                    goto pop_node;
                }
                return NULL;
value_failed:
                return NULL;
            }
pop_node:
            node = (u32 *)buffer[index--];
        } else {
            u32 child = node[3];
            node = *(u32 *volatile *)(node + 2);
            buffer[++index] = child;
        }
    } while (index >= 0);
    return object;
}

s32 fn_80485D78(u32, u32, void *, void *);
s32 fn_80461E20(void);
s32 fn_80456B48(void);
s32 fn_8046318C(void);
void fn_8045B048(void);
void fn_8045B0D8(void);
void fn_8045BFBC(void);
void fn_8045BF64(void);
void fn_80458CF8(void);
void fn_80458D10(void);
void fn_80456A50(void);
void fn_80456994(void);
void fn_8045AE98(void);
void fn_8045AE20(void);
void fn_8045F2A0(void);
void fn_8045F2B4(void);
u32 *fn_8045FA50(u32 *, u32);
void fn_80455CF8(void);
void fn_80455CE4(void);

u32 fn_8046080C(void) {
    s32 status = fn_80485D78(4, 1281, fn_8045B048, fn_8045B0D8);
    status |= fn_80485D78(48, 1282, fn_8045BFBC, fn_8045BF64);
    status |= fn_80485D78(4, 1283, fn_80458CF8, fn_80458D10);
    status |= fn_80485D78(8, 1284, fn_80456A50, fn_80456994);
    status |= fn_80485D78(4, 1285, fn_8045AE98, fn_8045AE20);
    status |= fn_80485D78(0, 1290, fn_8045F2A0, fn_8045F2B4);
    status |= fn_80485D78(12, 1287, (void *)fn_8045FA50, (void *)fn_8045F9E8);
    status |= fn_80485D78(0, 1291, fn_80455CF8, fn_80455CE4);
    if (status < 0) {
        return 0;
    }
    if (fn_80461E20() == 0) {
        return 0;
    }
    if (fn_80456B48() == 0) {
        return 0;
    }
    return fn_8046318C() != 0;
}

void fn_8045C2D4(u32 *);
void fn_8045B6F0(u32 *);
void fn_80482144(u8 *, u32 *);
void fn_8045F6E0(u32 *);
void fn_8045F7D0(u32 *);

u32 fn_8045FFA0(u32 *object) {
    u32 buffer[62];
    u32 *current;
    u32 *sentinel = (u32 *)(lbl_805F15B4 + lbl_805F1500 + 4);
    current = (u32 *)sentinel[0];
    while (current != sentinel) {
        u32 *link = current - 2;
        if (*(u32 volatile *)link == (u32)object) {
            ((u32 *)link[3])[0] = link[2];
            ((u32 *)link[2])[1] = link[3];
            ((void (*)(u32 *, u32 *))*(u32 *)(lbl_805F15B4 + 0x11C))(
                *(u32 **)(lbl_805F15B4 + lbl_805F1500), link);
            break;
        }
        current = (u32 *)current[0];
    }
    s32 index = 0;
    u32 *node = (u32 *)object[7];
    if (node != NULL) {
        u32 zero = index;
        do {
            if (*(s32 *)node < 0) {
                if (node[30] != 0) {
                    fn_8045C2D4((u32 *)node[30]);
                    node[30] = zero;
                }
                node = (u32 *)buffer[index--];
            } else {
                u32 child = node[3];
                node = (u32 *)node[2];
                buffer[++index] = child;
            }
        } while (index >= 0);
    }
    fn_8045B6F0(object + 4);
    if (((u8 *)object)[3] & 1) {
        if (object[7] != 0) {
            fn_8045F6E0((u32 *)object[7]);
        }
        fn_80482144(lbl_8056D898, object);
        ((void (*)(u32 *))*(u32 *)(lbl_805F15B4 + 0x10C))(object);
    } else {
        if (object[7] != 0) {
            fn_8045F7D0((u32 *)object[7]);
        }
        fn_80482144(lbl_8056D898, object);
        ((void (*)(u32 *))*(u32 *)(lbl_805F15B4 + 0x10C))(object);
    }
    return 1;
}

s32 fn_80462798(void);
extern u8 lbl_805E1618[];

u32 *fn_8045FA50(u32 *object, u32 offset) {
    lbl_805F1500 = offset;
    if (fn_80462798() == 0) {
        return NULL;
    }
    *(u32 **)(lbl_805F15B4 + lbl_805F1500) =
        (u32 *)fn_8047FC04(16, 8, 4, 1, (u32)lbl_805E1618, 0x4000B);
    u32 *table = (u32 *)(lbl_805F15B4 + lbl_805F1500);
    if (table[0] == 0) {
        return NULL;
    }
    *(u32 *)(lbl_805F15B4 + lbl_805F1500 + 4) = (u32)(lbl_805F15B4 + lbl_805F1500 + 4);
    *(u32 *)(lbl_805F15B4 + lbl_805F1500 + 8) = (u32)(lbl_805F15B4 + lbl_805F1500 + 4);
    (&lbl_805F1500)[1]++;
    return object;
}

extern u32 lbl_805F1690;
u32 *fn_8049B674(u32 *, u32 *, u32);

u32 *fn_8045FB10(u32 *object) {
    if (((u16 *)object)[66] == 0) {
        return object;
    }
    u32 *callback;
    if (object[31] != 0) {
        callback = (u32 *)object[31];
    } else {
        u32 *engine = *(u32 **)(lbl_805F15B4 + 4);
        if (engine[27] != 0) {
            callback = (u32 *)engine[27];
        } else {
            callback = *(u32 **)(lbl_805F15B4 + lbl_805F1690 + 64);
        }
    }
    if (fn_8049B674(callback, object, 1) != 0) {
        return object;
    }
    return NULL;
}

void fn_8048071C(u32 *);
extern u8 lbl_8056D880[];

void fn_8045F6E0(u32 *object) {
    switch (*(s32 *)object) {
    case -1:
        if (object[13] != 0) {
            fn_8048071C((u32 *)object[13]);
        }
        {
            u32 *sentinel = object + 14;
            u32 *current = (u32 *)object[14];
            while (current != sentinel) {
                u32 *item = current;
                current = (u32 *)current[0];
                fn_80460DF0(item);
            }
        }
        {
            u32 *sentinel = object + 16;
            u32 *current = (u32 *)object[16];
            while (current != sentinel) {
                u32 *item = current;
                current = (u32 *)current[0];
                fn_80460D80(item);
            }
        }
        fn_80482144(lbl_8056D880, object);
        if (object[30] != 0) {
            fn_8045C2D4((u32 *)object[30]);
            object[30] = 0;
        }
        break;
    case -2:
        break;
    default:
        if (object[2] != 0) {
            fn_8045F6E0((u32 *)object[2]);
        }
        if (object[3] != 0) {
            fn_8045F6E0((u32 *)object[3]);
        }
        break;
    }
}

void fn_8045FBA4(u32 *object) {
    u32 current[6];
    u32 links[64];
    u32 states[64][6];
    s32 index = 0;
    s32 offset;
    u32 *node;
    current[0] = object[20];
    current[1] = object[21];
    current[2] = object[22];
    current[3] = object[23];
    current[4] = object[24];
    current[5] = object[25];
    node = (u32 *)object[7];
    do {
        offset = *(s32 *)node;
        if (offset < 0) {
            node[18] = current[0];
            node[19] = current[1];
            node[20] = current[2];
            node[21] = current[3];
            node[22] = current[4];
            node[23] = current[5];
            current[0] = states[index][0];
            current[1] = states[index][1];
            current[2] = states[index][2];
            current[3] = states[index][3];
            current[4] = states[index][4];
            current[5] = states[index][5];
            node = (u32 *)links[index--];
        } else {
            index++;
            states[index][0] = current[0];
            states[index][1] = current[1];
            states[index][2] = current[2];
            states[index][3] = current[3];
            states[index][4] = current[4];
            states[index][5] = current[5];
            u32 child = node[3];
            u8 *saved = (u8 *)states[index] + offset;
            f32 first = ((f32 *)node)[4];
            f32 second = ((f32 *)node)[5];
            node = (u32 *)node[2];
            links[index] = child;
            *(f32 *)(saved + 12) = second;
            *(f32 *)((u8 *)current + offset) = first;
        }
    } while (index >= 0);
}

void fn_8045F7D0(u32 *input) {
    switch (*(s32 *)input) {
    case -1:
        u32 *object = input;
        if (object[13] != 0) {
            fn_8048071C((u32 *)object[13]);
        }
        {
            u32 *sentinel = object + 14;
            u32 *current = (u32 *)object[14];
            while (current != sentinel) {
                u32 *item = current;
                current = (u32 *)current[0];
                fn_80460DF0(item);
            }
        }
        {
            u32 *current = (u32 *)object[16];
            u32 *sentinel = object + 16;
            while (current != sentinel) {
                u32 *item = current;
                current = (u32 *)current[0];
                fn_80460D80(item);
            }
        }
        fn_80482144(lbl_8056D880, object);
        if (object[2] != 0) {
            ((void (*)(u32 *))*(u32 *)(lbl_805F15B4 + 0x10C))((u32 *)object[2]);
            object[2] = 0;
        }
        if (object[3] != 0) {
            ((void (*)(u32 *))*(u32 *)(lbl_805F15B4 + 0x10C))((u32 *)object[3]);
            object[3] = 0;
        }
        if (object[12] != 0) {
            ((void (*)(u32 *))*(u32 *)(lbl_805F15B4 + 0x10C))((u32 *)object[12]);
            object[12] = 0;
        }
        if (object[1] != 0) {
            ((void (*)(u32 *))*(u32 *)(lbl_805F15B4 + 0x10C))((u32 *)object[1]);
            object[1] = 0;
        }
        {
            u32 *field = object;
            u32 zero = 0;
            for (s32 i = 0; i < 8; i++) {
                if (field[4] != 0) {
                    ((void (*)(u32 *))*(u32 *)(lbl_805F15B4 + 0x10C))((u32 *)field[4]);
                    field[4] = zero;
                }
                field++;
            }
        }
        if (object[30] != 0) {
            fn_8045C2D4((u32 *)object[30]);
            object[30] = 0;
        }
        ((void (*)(u32 *))*(u32 *)(lbl_805F15B4 + 0x10C))(object);
        break;
    case -2:
        ((void (*)(u32 *))*(u32 *)(lbl_805F15B4 + 0x10C))(input);
        break;
    default: {
        u32 *target = input;
        if (target[2] != 0) {
            fn_8045F7D0((u32 *)target[2]);
        }
        target[2] = 0;
        if (target[3] != 0) {
            fn_8045F7D0((u32 *)target[3]);
        }
        target[3] = 0;
        ((void (*)(u32 *))*(u32 *)(lbl_805F15B4 + 0x10C))(target);
        break;
    }
    }
}

u32 *fn_8045F1FC(u32 *, u32);
void fn_8045C26C(u32 *);
void fn_8045C340(u32 *, u32 *, u32, u32, u32, u32, u32, u32);

u32 *fn_8045F458(u32 *object, u32 *value, u32 *node, u32 *slot) {
    u32 count_a = 0;
    u32 count_b = 0;
    u32 count_c = 0;
    if (object[5] != 0) {
        u32 size = object[5] * 4;
        u32 *table_a = ((u32 *(*)(u32, u32))*(u32 *)(lbl_805F15B4 + 0x108))(size, 0x10006);
        u32 *table_b = ((u32 *(*)(u32, u32))*(u32 *)(lbl_805F15B4 + 0x108))(size, 0x10507);
        u32 *table_c = ((u32 *(*)(u32, u32))*(u32 *)(lbl_805F15B4 + 0x108))(size, 0x10507);
        s32 entry_index = 0;
        u32 entry_offset = 0;
        while (entry_index < ((u16 *)node)[66]) {
            u16 *entry = (u16 *)(node[1] + entry_offset);
            u32 *entry_slot = *(u32 **)((u8 *)slot + entry[3] * 4);
            u32 index_a, index_b, index_c;
            u32 *target_a = (u32 *)entry_slot[0];
            index_a = 0;
            while ((u16)index_a < (u16)count_a) {
                if (table_a[(u16)index_a] == (u32)target_a) {
                    break;
                }
                index_a++;
            }
            if ((u16)index_a == (u16)count_a) {
                table_a[(u16)index_a] = (u32)target_a;
                count_a++;
            }
            u32 target_b = target_a != NULL ? target_a[0] : 0;
            index_b = 0;
            while ((u16)index_b < (u16)count_b) {
                if (table_b[(u16)index_b] == target_b) {
                    break;
                }
                index_b++;
            }
            if ((u16)index_b == (u16)count_b) {
                table_b[(u16)index_b] = target_b;
                count_b++;
            }
            u32 target_c = entry_slot[2];
            index_c = 0;
            while ((u16)index_c < (u16)count_c) {
                if (table_c[(u16)index_c] == target_c) {
                    break;
                }
                index_c++;
            }
            if ((u16)index_c == (u16)count_c) {
                table_c[(u16)index_c] = target_c;
                count_c++;
            }
            fn_8045C340(value, entry_slot, entry[0], entry[1], entry[2], entry[3], index_a, index_c);
            entry_offset += 8;
            entry_index++;
        }
        ((void (*)(u32 *))*(u32 *)(lbl_805F15B4 + 0x10C))(table_a);
        ((void (*)(u32 *))*(u32 *)(lbl_805F15B4 + 0x10C))(table_b);
        ((void (*)(u32 *))*(u32 *)(lbl_805F15B4 + 0x10C))(table_c);
    }
    u32 *result;
    if (object[2] & 1) {
        result = fn_8045F1FC(value, 1);
    } else {
        result = fn_8045F1FC(value, 0);
    }
    if (result != NULL) {
        node[30] = (u32)result;
    } else {
        fn_8045C26C(value);
        return NULL;
    }
    return object;
}

void fn_8045B788(u32 *);
void fn_8048209C(u8 *, u32 *);
u32 fn_8047DC28(u32, u32, ...);
void fn_8047DB84(u32 *);
extern f32 lbl_805FB178;

u32 *fn_80460138(u32 *config) {
    u32 errors[4];
    u32 resolve_stack[64];
    u32 cleanup_stack[64];
    u32 *outer = ((u32 *(*)(u32, u32))*(u32 *)(lbl_805F15B4 + 0x108))(
        *(u32 *)lbl_8056D898, 0x3000B);
    if (outer == NULL) {
        errors[2] = 2;
        errors[3] = fn_8047DC28(0x80000013, *(u32 *)lbl_8056D898);
        fn_8047DB84(errors + 2);
        return NULL;
    }
    ((u8 *)outer)[0] = 7;
    ((u8 *)outer)[1] = 0;
    ((u8 *)outer)[2] = 0;
    ((u8 *)outer)[3] = 0;
    outer[1] = 0;
    fn_8045B788(outer + 4);
    outer[3] = 2;
    outer[2] = 0;
    u32 *inner = ((u32 *(*)(u32, u32))*(u32 *)(lbl_805F15B4 + 0x108))(
        *(u32 *)lbl_8056D880, 0x3000B);
    if (inner == NULL) {
        errors[0] = 2;
        errors[1] = fn_8047DC28(0x80000013, 4);
        fn_8047DB84(errors);
        ((void (*)(u32 *))*(u32 *)(lbl_805F15B4 + 0x10C))(outer);
        return NULL;
    }
    ((s32 *)inner)[0] = -1;
    inner[13] = 0;
    inner[30] = 0;
    inner[14] = (u32)(inner + 14);
    inner[15] = (u32)(inner + 14);
    inner[16] = (u32)(inner + 16);
    inner[17] = (u32)(inner + 16);
    ((u16 *)inner)[65] = 0;
    ((u16 *)inner)[66] = 0;
    inner[2] = 0;
    inner[1] = 0;
    inner[3] = 0;
    memset(inner + 4, 0, 32);
    inner[12] = 0;
    inner[21] = config[3];
    inner[22] = config[4];
    inner[23] = config[5];
    inner[18] = config[0];
    inner[19] = config[1];
    inner[20] = config[2];
    inner[27] = config[3];
    inner[28] = config[4];
    inner[29] = config[5];
    inner[24] = config[0];
    inner[25] = config[1];
    inner[26] = config[2];
    inner[31] = 0;
    outer[7] = (u32)inner;
    outer[8] = 0;
    ((f32 *)outer)[19] = lbl_805FB178;
    ((f32 *)outer)[18] = lbl_805FB178;
    ((f32 *)outer)[17] = lbl_805FB178;
    outer[23] = config[3];
    outer[24] = config[4];
    outer[25] = config[5];
    outer[20] = config[0];
    outer[21] = config[1];
    outer[22] = config[2];
    outer[11] = (u32)(outer + 11);
    outer[12] = (u32)(outer + 11);
    outer[9] = 0;
    outer[10] = (u32)(outer + 11);
    outer[13] = (u32)(outer + 13);
    outer[14] = (u32)(outer + 13);
    outer[15] = (u32)(outer + 15);
    outer[16] = (u32)(outer + 15);
    outer[26] = (u32)fn_8045FB10;
    outer[27] = 0;
    u32 config_value = *(u32 *)lbl_8056D898;
    u32 *link_node = ((u32 *(*)(u32 *, u32))*(u32 *)(lbl_805F15B4 + 0x118))(
        *(u32 **)(lbl_805F15B4 + lbl_805F1500), 0x40507);
    if (link_node != NULL) {
        link_node[0] = (u32)outer;
        link_node[1] = config_value;
        u32 *link = link_node + 2;
        link_node[2] = *(u32 *)(lbl_805F15B4 + lbl_805F1500 + 4);
        link_node[3] = (u32)(lbl_805F15B4 + lbl_805F1500 + 4);
        *(u32 *)(*(u32 *)(lbl_805F15B4 + lbl_805F1500 + 4) + 4) = (u32)link;
        *(u32 **)(lbl_805F15B4 + lbl_805F1500 + 4) = link;
    }
    fn_8048209C(lbl_8056D898, outer);
    fn_8048209C(lbl_8056D880, inner);
    u32 *resolved = NULL;
    u32 *node = (u32 *)outer[7];
    s32 index = 0;
    if (node != NULL) {
        resolved = outer;
        do {
            if (*(s32 *)node < 0) {
                u32 *slot = (u32 *)(resolved[4] + ((u16 *)node)[64] * 4);
                if (node[30] == 0) {
                    u32 *value = fn_8045C144(((u16 *)node)[66], resolved[4]);
                    if (value == NULL) {
                        resolved = NULL;
                        break;
                    }
                    resolved = fn_8045F458(resolved, value, node, slot);
                    if (resolved == NULL) {
                        break;
                    }
                }
                node = (u32 *)resolve_stack[index--];
            } else {
                u32 child = node[3];
                node = (u32 *)node[2];
                resolve_stack[++index] = child;
            }
        } while (index >= 0);
    }
    if (resolved != NULL) {
        return outer;
    }
    u32 *current;
    u32 *sentinel = (u32 *)(lbl_805F15B4 + lbl_805F1500 + 4);
    current = (u32 *)sentinel[0];
    while (current != sentinel) {
        u32 *link = current - 2;
        if (current[-2] == (u32)outer) {
            ((u32 *)link[3])[0] = link[2];
            ((u32 *)link[2])[1] = link[3];
            ((void (*)(u32 *, u32 *))*(u32 *)(lbl_805F15B4 + 0x11C))(
                *(u32 **)(lbl_805F15B4 + lbl_805F1500), link);
            break;
        }
        current = (u32 *)current[0];
    }
    node = (u32 *)outer[7];
    index = 0;
    if (node != NULL) {
        u32 zero = index;
        do {
            if (*(s32 *)node < 0) {
                if (node[30] != 0) {
                    fn_8045C2D4((u32 *)node[30]);
                    node[30] = zero;
                }
                node = (u32 *)cleanup_stack[index--];
            } else {
                u32 child = node[3];
                node = (u32 *)node[2];
                cleanup_stack[++index] = child;
            }
        } while (index >= 0);
    }
    fn_8045B6F0(outer + 4);
    if (((u8 *)outer)[3] & 1) {
        if (outer[7] != 0) {
            fn_8045F6E0((u32 *)outer[7]);
        }
        fn_80482144(lbl_8056D898, outer);
        ((void (*)(u32 *))*(u32 *)(lbl_805F15B4 + 0x10C))(outer);
    } else {
        if (outer[7] != 0) {
            fn_8045F7D0((u32 *)outer[7]);
        }
        fn_80482144(lbl_8056D898, outer);
        ((void (*)(u32 *))*(u32 *)(lbl_805F15B4 + 0x10C))(outer);
    }
    return NULL;
}
}
