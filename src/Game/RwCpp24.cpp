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
}
