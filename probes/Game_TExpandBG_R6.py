"""fn_80043C58 of Game/TExpandBG.cpp (wave 39, second pass): the aligned swaps as inline functions."""
SYMBOLS = {"fn_80043C58": 'extern "C" u8 *fn_80043C58(u8 *data, const char *format, int count) {'}
def body(c16, c32):
    return """    for (int i = 0; i < count; i++) {
        for (const char *f = format; *f != 0; f++) {
            switch (*f) {
            case 'C':
            case 'c':
                data++;
                break;
            case 'S':
            case 's':
%s                break;
            case 'I':
            case 'i':
            case 'F':
            case 'f':
            case 'X':
            case 'x':
%s                break;
            }
        }
    }
    return data;
""" % (c16, c32)
I = "                "
SW16 = "((*p >> 8) & 0xFF) | ((*p << 8) & 0xFF00)"
SW32 = "(*p >> 24) | ((*p >> 8) & 0xFF00) | ((*p << 8) & 0xFF0000) | (*p << 24)"
XTRA = {"fn_80043C58": """inline u8 *SwapNext16(u8 *data) {
    u16 *p = (u16 *)(((u32)data + 1) & ~1);
    data = (u8 *)(p + 1);
    *p = %s;
    return data;
}
inline u8 *SwapNext32(u8 *data) {
    u32 *p = (u32 *)(((u32)data + 3) & ~3);
    data = (u8 *)(p + 1);
    *p = %s;
    return data;
}
inline void SwapAt16(u16 *p) {
    *p = %s;
}
inline void SwapAt32(u32 *p) {
    *p = %s;
}
inline void SwapRef16(u16 &r) {
    r = ((r >> 8) & 0xFF) | ((r << 8) & 0xFF00);
}
inline void SwapRef32(u32 &r) {
    r = (r >> 24) | ((r >> 8) & 0xFF00) | ((r << 8) & 0xFF0000) | (r << 24);
}
inline void SwapData16(u8 *&data) {
    u16 *p = (u16 *)(((u32)data + 1) & ~1);
    data = (u8 *)(p + 1);
    *p = %s;
}
inline void SwapData32(u8 *&data) {
    u32 *p = (u32 *)(((u32)data + 3) & ~3);
    data = (u8 *)(p + 1);
    *p = %s;
}
""" % (SW16, SW32, SW16, SW32, SW16, SW32)}
BODIES = {"fn_80043C58": {
 "next": body(I + "data = SwapNext16(data);\n", I + "data = SwapNext32(data);\n"),
 "dataref": body(I + "SwapData16(data);\n", I + "SwapData32(data);\n"),
 "at": body(I + "{\n" + I + "u16 *p = (u16 *)(((u32)data + 1) & ~1);\n" + I + "data = (u8 *)(p + 1);\n" + I + "SwapAt16(p);\n" + I + "}\n", I + "{\n" + I + "u32 *p = (u32 *)(((u32)data + 3) & ~3);\n" + I + "data = (u8 *)(p + 1);\n" + I + "SwapAt32(p);\n" + I + "}\n"),
 "ref": body(I + "{\n" + I + "u16 *p = (u16 *)(((u32)data + 1) & ~1);\n" + I + "data = (u8 *)(p + 1);\n" + I + "SwapRef16(*p);\n" + I + "}\n", I + "{\n" + I + "u32 *p = (u32 *)(((u32)data + 3) & ~3);\n" + I + "data = (u8 *)(p + 1);\n" + I + "SwapRef32(*p);\n" + I + "}\n"),
 "at_expr": body(I + "data = (u8 *)(((u32)data + 1) & ~1);\n" + I + "SwapAt16((u16 *)data);\n" + I + "data += 2;\n", I + "data = (u8 *)(((u32)data + 3) & ~3);\n" + I + "SwapAt32((u32 *)data);\n" + I + "data += 4;\n"),
 "at_fn": body(I + "{\n" + I + "u16 *p = (u16 *)(((u32)data + 1) & ~1);\n" + I + "data = (u8 *)(p + 1);\n" + I + "fn_80043CE4(p, 2);\n" + I + "}\n", I + "{\n" + I + "u32 *p = (u32 *)(((u32)data + 3) & ~3);\n" + I + "data = (u8 *)(p + 1);\n" + I + "fn_80043D0C(p, 4);\n" + I + "}\n"),
}}
