# Probes for Game/RwCpp51 (the other agent's C++ draft, its flags): the mipmap name suffix
# (fn_8048B6AC) and the dirty frame list walk (fn_8048B354).
V = "    int valid = value != 0 && value < 0x10;\n"
W = """u32 fn_8048B354(void) {
    u8 *base = lbl_805F15B4;
    u32 *list = (u32 *)(base + 0xBC);
    for (u32 *link = *(u32 **)list; link != list; link = (u32 *)*link) {
        fn_8048B208((u8 *)(link - 2));
    }
    *(u32 *)(base + 0xBC) = (u32)(base + 0xBC);
    *(u32 *)(base + 0xC0) = (u32)(base + 0xBC);
    return 1;
}"""
W2 = """u32 fn_8048B354(void) {
    u32 *list = (u32 *)(lbl_805F15B4 + 0xBC);
    u32 *link = *(u32 **)list;

    while (link != list) {
        fn_8048B208((u8 *)(link - 2));
        link = (u32 *)*link;
    }
    *(u32 *)(lbl_805F15B4 + 0xBC) = (u32)(lbl_805F15B4 + 0xBC);
    *(u32 *)(lbl_805F15B4 + 0xC0) = (u32)(lbl_805F15B4 + 0xBC);
    return 1;
}"""
REPL = {
    "base": [],
    "valid": [(V, "    int valid = 0;\n    if (value != 0 && value < 0x10) valid = 1;\n")],
    "walk": [(W, W2)],
    "both": [(V, "    int valid = 0;\n    if (value != 0 && value < 0x10) valid = 1;\n"), (W, W2)],
}
SCORE = ["fn_8048B6AC", "fn_8048B354"]
