/* RenderWare small callbacks and accessors. Names are inferred from behavior. */
#include <dolphin/types.h>

extern u32 lbl_805F14B8;
extern u32 lbl_805F14BC;
extern u32 lbl_805F14C8;
extern u8 *lbl_805F15B4;
extern u8 lbl_8056D7E0[];
extern u8 lbl_8056D7F8[];

extern "C" {
u32 *fn_804573A8(u32 *);
void fn_80457468(u32 *);
void fn_80457608(u32 *, u32 *);
u32 fn_80486C5C(u32);
u32 fn_80486CA4(u32);
u32 *fn_80456DD8(void);
void fn_80458CBC(u32 *, u32);
void fn_8045984C(u32);
void fn_8045985C(u32);
u32 fn_804622F0(u32 *);
void fn_8048722C(u32);
void fn_804821B0(u8 *, u32 *, u32 *);
void fn_80482144(u8 *, u32 *);
void fn_8048071C(u32);
void fn_8048D834(u32 *);
u32 fn_8047DC28(u32);
void fn_8047DB84(u32 *);

u32 fn_80457090(u32 *object) {
    u32 *node = fn_804573A8(object);
    if (node == NULL) {
        return 0;
    }
    node[1] = fn_80486C5C(object[1]);
    if (node[1] == 0) {
        fn_80457468(node);
        return 0;
    }

    volatile u32 status = 1;
    u32 *sentinel = object + 2;
    u32 *link = (u32 *)object[2];
    while (link != sentinel) {
        u32 next = *link;
        u32 *current = link - 0x10;
        u32 *created = fn_80456DD8();
        if (created == NULL) {
            status = 0;
            break;
        }

        ((u8 *)created)[0] = ((u8 *)current)[0];
        ((u8 *)created)[1] = ((u8 *)current)[1];
        ((u8 *)created)[2] = ((u8 *)current)[2];
        ((u8 *)created)[3] = ((u8 *)current)[3];
        created[1] = 0;
        ((u8 *)created)[2] = ((u8 *)current)[2];
        fn_80458CBC(created, 0);
        u32 old = current[6];
        if (old != created[6]) {
            if (old != 0) fn_8045984C(old);
            if (created[6] != 0) fn_8045985C(created[6]);
            created[6] = old;
            if (old != 0) {
                u32 *extra = *(u32 **)(old + 0x5C);
                created[7] = extra[1];
                created[8] = extra[2];
                created[9] = extra[3];
                created[0xA] = extra[4];
            }
        }
        if (created[1] != 0 && fn_804622F0(created) != 0) {
            fn_8048722C(created[1]);
        }

        current[7] = created[7];
        current[8] = created[8];
        current[9] = created[9];
        current[0xA] = created[0xA];
        created[0x12] = current[0x12];
        created[0x1B] = current[0x1B];
        created[0x24] = current[0x24];
        created[0x25] = current[0x25];
        created[0x26] = current[0x26];
        created[0x27] = current[0x27];
        created[0x28] = current[0x28];
        created[0x29] = current[0x29];
        created[0x2A] = current[0x2A];
        created[0x2B] = current[0x2B];
        created[0x2C] = current[0x2C];
        created[0x2D] = current[0x2D];
        created[0x2E] = current[0x2E];
        created[0x2F] = current[0x2F];
        fn_804821B0(lbl_8056D7E0, created, current);

        if (created == NULL) {
            status = 0;
            break;
        }
        if (current[1] != 0) {
            fn_80458CBC(created, *(u32 *)(current[1] + 0xA0));
            fn_80457608(node, created);
        } else {
            fn_80482144(lbl_8056D7E0, created);
            if (created[5] != 0) {
                fn_8048071C(created[5]);
            }
            if (created[6] != 0) {
                fn_8045985C(created[6]);
                created[6] = 0;
                if (created[1] != 0 && fn_804622F0(created) != 0) {
                    fn_8048722C(created[1]);
                }
            }
            fn_8048D834(created);
            ((void (*)(u32 *))*(u32 *)(lbl_805F15B4 + lbl_805F14C8 + 0x11C))(created);
            u32 statusArgs[2];
            statusArgs[0] = 2;
            statusArgs[1] = fn_8047DC28(5);
            fn_8047DB84(statusArgs);
            status = 0;
        }
        if (status == 0) {
            break;
        }
        link = (u32 *)next;
    }

    fn_80486CA4(object[1]);
    if (status == 0) {
        fn_80457468(node);
        return 0;
    }
    node[0xA] = object[0xA];
    fn_804821B0(lbl_8056D7F8, node, object);
    return (u32)node;
}

void fn_80457608(u32 *object, u32 *other) {
    u32 next = object[2];
    u32 *node = object + 2;
    u32 newNext = (u32)other + 0x40;
    other[0x10] = next;
    other[0x11] = (u32)node;
    node = (u32 *)object[2];
    node[1] = newNext;
    object[2] = newNext;
    other[0x0F] = (u32)object;
}

void fn_80457630(u8 *other) {
    u32 *target = (u32 *)(other + lbl_805F14BC);
    *(u32 *)target[2] = target[1];
    *(u32 *)(target[1] + 4) = target[2];
    target[2] = 0;
    target[1] = 0;
    target[0] = 0;
}

void fn_80457664(u8 *other) {
    u32 *target = (u32 *)(other + lbl_805F14B8);
    *(u32 *)target[2] = target[1];
    *(u32 *)(target[1] + 4) = target[2];
    target[2] = 0;
    target[1] = 0;
    target[0] = 0;
}

void fn_804815F0(u8 *, u32, u32, u32, u32);

void fn_80458BEC(u32 first, u32 second, u32 third, u32 fourth) {
    fn_804815F0(lbl_8056D7E0, first, second, third, fourth);
}

void fn_80481DC8(u8 *, u32, u32, u32, u32, u32);

void fn_80458B6C(u32 first, u32 second, u32 third, u32 fourth, u32 fifth) {
    fn_80481DC8(lbl_8056D7E0, first, second, third, fourth, fifth);
}

void fn_80458BAC(u32 first, u32 second, u32 third, u32 fourth, u32 fifth) {
    fn_80481DC8(lbl_8056D7F8, first, second, third, fourth, fifth);
}
}
