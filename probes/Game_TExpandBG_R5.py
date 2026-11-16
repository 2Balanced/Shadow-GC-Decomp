"""fn_80043C58 of Game/TExpandBG.cpp (wave 39, second pass): retail has the pointer in r6, the value
in r7, the count in r8 and the format in r9."""
SYMBOLS = {"fn_80043C58": 'extern "C" u8 *fn_80043C58(u8 *data, const char *format, int count) {'}
def body(top, loop_i, loop_f, c16, c32):
    return top + """    for (%s; i < count; i++) {
        for (%s; *f != 0; f++) {
            switch (*f) {
            case 'C':
            case 'c':
                data++;
                break;
            case 'S':
            case 's': {
%s                break;
            }
            case 'I':
            case 'i':
            case 'F':
            case 'f':
            case 'X':
            case 'x': {
%s                break;
            }
            }
        }
    }
    return data;
""" % (loop_i, loop_f, c16, c32)
I = "                "
SW16 = "((*p >> 8) & 0xFF) | ((*p << 8) & 0xFF00)"
SW32 = "(*p >> 24) | ((*p >> 8) & 0xFF00) | ((*p << 8) & 0xFF0000) | (*p << 24)"
A16 = I + "u16 *p = (u16 *)(((u32)data + 1) & ~1);\n" + I + "data = (u8 *)(p + 1);\n" + I + "*p = " + SW16 + ";\n"
A32 = I + "u32 *p = (u32 *)(((u32)data + 3) & ~3);\n" + I + "data = (u8 *)(p + 1);\n" + I + "*p = " + SW32 + ";\n"
V16 = I + "u16 *p = (u16 *)(((u32)data + 1) & ~1);\n" + I + "data = (u8 *)(p + 1);\n" + I + "u16 value = *p;\n" + I + "*p = ((value >> 8) & 0xFF) | ((value << 8) & 0xFF00);\n"
V32 = I + "u32 *p = (u32 *)(((u32)data + 3) & ~3);\n" + I + "data = (u8 *)(p + 1);\n" + I + "u32 value = *p;\n" + I + "*p = (value >> 24) | ((value >> 8) & 0xFF00) | ((value << 8) & 0xFF0000) | (value << 24);\n"
T16 = I + "p16 = (u16 *)(((u32)data + 1) & ~1);\n" + I + "data = (u8 *)(p16 + 1);\n" + I + "*p16 = " + SW16.replace("*p", "*p16") + ";\n"
T32 = I + "p32 = (u32 *)(((u32)data + 3) & ~3);\n" + I + "data = (u8 *)(p32 + 1);\n" + I + "*p32 = " + SW32.replace("*p", "*p32") + ";\n"
BODIES = {"fn_80043C58": {
 "i_f_top": body("    int i;\n    const char *f;\n", "i = 0", "f = format", A16, A32),
 "f_i_top": body("    const char *f;\n    int i;\n", "i = 0", "f = format", A16, A32),
 "p_top": body("    u16 *p16;\n    u32 *p32;\n", "int i = 0", "const char *f = format", T16, T32),
 "p_i_f_top": body("    u16 *p16;\n    u32 *p32;\n    int i;\n    const char *f;\n", "i = 0", "f = format", T16, T32),
 "i_f_p_top": body("    int i;\n    const char *f;\n    u16 *p16;\n    u32 *p32;\n", "i = 0", "f = format", T16, T32),
 "val_locals": body("", "int i = 0", "const char *f = format", V16, V32),
 "val_i_f_top": body("    int i;\n    const char *f;\n", "i = 0", "f = format", V16, V32),
}}
