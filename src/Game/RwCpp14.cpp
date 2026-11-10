/* RenderWare debug-table construction. Names are inferred from the call sequence; the public
 * entry points retain their address names. */
#include <dolphin/types.h>

extern u32 lbl_805E1560[];
extern u32 lbl_805F1520;

extern "C" {
u32 fn_8044CCAC(u8 *);
void *fn_80462E1C(void *);
void fn_8046FC1C(u32 *);
void fn_8046FC48(u32 *, u32);
void fn_8046FC50(u32 *, u32, u32, u32, u32);
void fn_8046FF34(u32 *, u32, u32);
void fn_8047046C(u32 *, u32);
}

extern "C" void fn_8044F4D0(u32 *object, u32 *table) {
    u32 *record = (u32 *)fn_8044CCAC((u8 *)object);
    u8 *format = *(u8 **)((u8 *)object + lbl_805F1520);
    if (format == NULL) format = (u8 *)fn_80462E1C(record);

    fn_8046FC1C(lbl_805E1560);
    fn_8046FC48(lbl_805E1560, 0);
    if (record[4] == 1) fn_8046FF34(lbl_805E1560, 0, 1);
    fn_8046FC50(lbl_805E1560, 9, 1, format[0], format[0xC]);

    u32 value = table[1];
    fn_8046FF34(lbl_805E1560, 9, value < 0xFF ? 2 : 3);
    u32 index = 1;
    if ((object[2] & 0x10) != 0) {
        if (format[0xD] != 0) {
            fn_8046FC50(lbl_805E1560, 0x19, 1, format[1], 0);
            value = table[index * 2 + 1];
            fn_8046FF34(lbl_805E1560, 0x19, value < 0xFF ? 2 : 3);
        } else {
            fn_8046FC50(lbl_805E1560, 0xA, 0, format[1], 0);
            value = table[index * 2 + 1];
            fn_8046FF34(lbl_805E1560, 0xA, value < 0xFF ? 2 : 3);
        }
        index = 2;
    }

    if ((object[2] & 0x8) != 0) {
        u32 byte = format[0xA];
        fn_8046FC50(lbl_805E1560, 0xB, (2 - byte) >> 31, byte, 0);
        value = table[index * 2 + 1];
        fn_8046FF34(lbl_805E1560, 0xB, value < 0xFF ? 2 : 3);
        index++;
    }

    if ((object[2] & 0x84) != 0) {
        u32 *entry = table + index * 2;
        for (u32 i = 0; i < (u32)object[7]; i++) {
            fn_8046FC50(lbl_805E1560, i + 0xD, 1, format[i + 2], format[i + 0xE]);
            value = entry[1];
            fn_8046FF34(lbl_805E1560, i + 0xD, value < 0xFF ? 2 : 3);
            entry += 2;
            index++;
        }
    }
    fn_8047046C(lbl_805E1560, index & 0xFF);
}
