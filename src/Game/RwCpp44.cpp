/* RenderWare small callbacks and accessors. Names are inferred from behavior. */
#include <dolphin/types.h>

extern u32 lbl_805EDEA0;
extern u8 *lbl_805F15B4;

extern "C" {
void fn_8047FA28(u32 value) {
    lbl_805EDEA0 = value;
}

u8 *fn_80480530(void) {
    return lbl_805F15B4 + 0x108;
}

void fn_8047FA30(u32, u32, u32, u32, u32, u32);
void fn_803A2DC4(void);
void fn_803A2CC8(void);
void fn_803A2C40(void);

void fn_8047FC04(u32 first, u32 second, u32 third, u32 fourth, u32 fifth, u32 sixth) {
    fn_8047FA30(first, second, third, fourth, fifth, sixth);
}

void fn_80480214(void) {
    fn_803A2DC4();
}

void fn_80480234(void) {
    fn_803A2CC8();
}

void fn_80480254(void) {
    fn_803A2C40();
}

void fn_8047FBD8(u32 first, u32 second, u32 third, u32 fourth) {
    fn_8047FA30(first, second, third, 1, 0, fourth);
}
}
