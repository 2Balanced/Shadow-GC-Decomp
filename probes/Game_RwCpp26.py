"""Game/RwCpp26.cpp (wave 40, agent r3)."""
A = """    lbl_805F1524 = fn_80460790(sizeof(RxGCVertexDesc *), 0x511, fn_80462E28, fn_80462E34, NULL);
    return lbl_805F1524 >= 0;
"""
REPL = {
    "base": [],
    "local": [(A, """    {
        s32 offset = fn_80460790(sizeof(RxGCVertexDesc *), 0x511, fn_80462E28, fn_80462E34, NULL);
        lbl_805F1524 = offset;
        return offset >= 0;
    }
""")],
    "assign_expr": [(A, """    return (lbl_805F1524 = fn_80460790(sizeof(RxGCVertexDesc *), 0x511, fn_80462E28, fn_80462E34,
                                       NULL)) >= 0;
""")],
    "ifelse": [(A, """    lbl_805F1524 = fn_80460790(sizeof(RxGCVertexDesc *), 0x511, fn_80462E28, fn_80462E34, NULL);
    if (lbl_805F1524 < 0) {
        return 0;
    }
    return 1;
""")],
    "ternary": [(A, """    lbl_805F1524 = fn_80460790(sizeof(RxGCVertexDesc *), 0x511, fn_80462E28, fn_80462E34, NULL);
    return (lbl_805F1524 < 0) ? 0 : 1;
""")],
    "not": [(A, """    lbl_805F1524 = fn_80460790(sizeof(RxGCVertexDesc *), 0x511, fn_80462E28, fn_80462E34, NULL);
    return !(lbl_805F1524 < 0);
""")],
}
SCORE = ["fn_80462ED4"]
MAC = "#define RPWORLDGLOBALPIPE(offset) (*(void **)((u8 *)lbl_805F15B4 + (lbl_805F1690 + (offset))))"
G = """s32 fn_8046318C(void) {
    if (!fn_80462ED4()) {
        return 0;
    }
    return fn_804647C8() != 0;
}"""
REPL.update({
    "idx": [(MAC, "#define RPWORLDGLOBALPIPE(offset) (*(void **)&((u8 *)lbl_805F15B4)[lbl_805F1690 + (offset)])")],
    "offfirst": [(MAC, "#define RPWORLDGLOBALPIPE(offset) (*(void **)(lbl_805F1690 + (offset) + (u8 *)lbl_805F15B4))")],
    "g_eq0": [(G, G.replace("if (!fn_80462ED4())", "if (fn_80462ED4() == 0)"))],
    "g_nested": [(G, """s32 fn_8046318C(void) {
    if (fn_80462ED4()) {
        return fn_804647C8() != 0;
    }
    return 0;
}""")],
    "g_ifelse": [(G, """s32 fn_8046318C(void) {
    if (!fn_80462ED4()) {
        return 0;
    } else {
        return fn_804647C8() != 0;
    }
}""")],
})
SCORE = ["fn_804631D8", "fn_80463244", "fn_8046318C"]
REPL.update({
    "int1": [(MAC, "#define RPWORLDGLOBALPIPE(offset) (*(void **)((u32)lbl_805F15B4 + (lbl_805F1690 + (offset))))")],
    "int2": [(MAC, "#define RPWORLDGLOBALPIPE(offset) (*(void **)((lbl_805F1690 + (offset)) + (u32)lbl_805F15B4))")],
    "int3": [(MAC, "#define RPWORLDGLOBALPIPE(offset) (*(void **)((u32)lbl_805F15B4 + lbl_805F1690 + (offset)))")],
    "arr": [(MAC, "#define RPWORLDGLOBALPIPE(offset) (((void **)((u8 *)lbl_805F15B4 + lbl_805F1690))[(offset) / 4])")],
})
CL = """        if (data->ambient.red > lbl_805FB1A4) {
            data->ambient.red = lbl_805FB1A4;
        }
        if (data->ambient.green > lbl_805FB1A4) {
            data->ambient.green = lbl_805FB1A4;
        }
        if (data->ambient.blue > lbl_805FB1A4) {
            data->ambient.blue = lbl_805FB1A4;
        }"""
REPL.update({
    "cl_rev": [(CL, CL.replace("data->ambient.red > lbl_805FB1A4", "lbl_805FB1A4 < data->ambient.red").replace("data->ambient.green > lbl_805FB1A4", "lbl_805FB1A4 < data->ambient.green").replace("data->ambient.blue > lbl_805FB1A4", "lbl_805FB1A4 < data->ambient.blue"))],
    "cl_nonconst": [("extern const f32 lbl_805FB1A4; /* 1.0f */", "extern f32 lbl_805FB1A4; /* 1.0f */")],
    "cl_lit": [(CL, CL.replace("lbl_805FB1A4", "1.0f"))],
    "cl_ge": [(CL, """        if (!(data->ambient.red <= lbl_805FB1A4)) {
            data->ambient.red = lbl_805FB1A4;
        }
        if (!(data->ambient.green <= lbl_805FB1A4)) {
            data->ambient.green = lbl_805FB1A4;
        }
        if (!(data->ambient.blue <= lbl_805FB1A4)) {
            data->ambient.blue = lbl_805FB1A4;
        }""")],
})
SCORE = ["fn_80463DF4"]
INIT = """    data->ambient.red = lbl_805FB1A0;
    data->ambient.green = lbl_805FB1A0;
    data->ambient.blue = lbl_805FB1A0;
    data->ambient.alpha = lbl_805FB1A4;"""
REPL.update({
    "all_lit": [(CL, CL.replace("lbl_805FB1A4", "1.0f")), (INIT, INIT.replace("lbl_805FB1A0", "0.0f").replace("lbl_805FB1A4", "1.0f"))],
})
