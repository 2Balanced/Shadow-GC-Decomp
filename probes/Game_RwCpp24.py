"""Sector-tree destroy fn_8045F7D0 of Game/RwCpp24.cpp (wave 40, agent r3): each case keeps its
own typed copy of the sector pointer."""
REPL = {
    "voidp": [("void fn_8045F7D0(u32 *);", "void fn_8045F7D0(void *);"),
              ("void fn_8045F7D0(u32 *input) {", "void fn_8045F7D0(void *input) {"),
              ("        u32 *object = input;", "        u32 *object = (u32 *)input;"),
              ("        u32 *target = input;", "        u32 *target = (u32 *)input;"),
              ("        ((void (*)(u32 *))*(u32 *)(lbl_805F15B4 + 0x10C))(input);", "        ((void (*)(void *))*(u32 *)(lbl_805F15B4 + 0x10C))(input);")],
    "voidp_u8": [("void fn_8045F7D0(u32 *);", "void fn_8045F7D0(void *);"),
              ("void fn_8045F7D0(u32 *input) {", "void fn_8045F7D0(void *input) {"),
              ("        u32 *object = input;", "        u32 *object = (u32 *)input;"),
              ("        u32 *target = input;", "        s32 *target = (s32 *)input;"),
              ("            fn_8045F7D0((u32 *)target[2]);", "            fn_8045F7D0((void *)target[2]);"),
              ("            fn_8045F7D0((u32 *)target[3]);", "            fn_8045F7D0((void *)target[3]);"),
              ("        ((void (*)(u32 *))*(u32 *)(lbl_805F15B4 + 0x10C))(input);", "        ((void (*)(void *))*(u32 *)(lbl_805F15B4 + 0x10C))(input);")],
}
SCORE = ["fn_8045F7D0"]
