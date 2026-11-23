"""Wave 40 (r7): fn_80493C84: register shape of the render mode copy (retail: address r8,
format copy r5, copy temporaries r7/r6)."""

SYM = "fn_80493C84"
SYMBOLS = {SYM: "void fn_80493C84(RwDlRenderMode *mode, RwInt32 format) {"}

REST = """        lbl_805F15E4 = used;
        lbl_805F15E8 = used;
        sVideoModes[3].width = lbl_805F164C->gx.fbWidth;
        sVideoModes[3].height = lbl_805F164C->gx.efbHeight;
        switch (format) {
        case 0:
        case 1:
            sVideoModes[3].depth = 24;
            break;
        case 2:
            sVideoModes[3].depth = 16;
            break;
        default:
            RW_ERROR_STRING(2, sInvalidPixelFormat);
            break;
        }
    } else {
        switch (VIGetTvFormat()) {
        case 0:
            lbl_805F164C = (RwDlRenderMode *)&GXNtsc480IntDf;
            lbl_805F15EC = 0;
            break;
        case 1:
        case 5:
            lbl_805F164C = (RwDlRenderMode *)&GXPal528IntDf;
            lbl_805F15EC = 1;
            break;
        case 2:
            lbl_805F164C = (RwDlRenderMode *)&GXMpal480IntDf;
            lbl_805F15EC = 2;
            break;
        default:
            RW_ERROR_STRING(2, sInvalidTvFormat);
            break;
        }
    }
"""
V = {
 "top_decl": """    RwInt32 used;
    if (mode != NULL) {
        lbl_805E3190 = *mode;
        lbl_805F164C = &lbl_805E3190;
        lbl_805F15EC = 0x2A;
        used = format;
        if (lbl_805F164C->gx.aa != 0) used = 2;
""",
 "top_init": """    RwInt32 used = format;
    if (mode != NULL) {
        lbl_805E3190 = *mode;
        lbl_805F164C = &lbl_805E3190;
        lbl_805F15EC = 0x2A;
        if (lbl_805F164C->gx.aa != 0) used = 2;
""",
 "blk_first": """    if (mode != NULL) {
        RwInt32 used = format;
        lbl_805E3190 = *mode;
        lbl_805F164C = &lbl_805E3190;
        lbl_805F15EC = 0x2A;
        if (lbl_805F164C->gx.aa != 0) used = 2;
""",
 "dst_ptr": """    if (mode != NULL) {
        RwDlRenderMode *dst = &lbl_805E3190;
        RwInt32 used;
        *dst = *mode;
        lbl_805F164C = dst;
        lbl_805F15EC = 0x2A;
        used = format;
        if (lbl_805F164C->gx.aa != 0) used = 2;
""",
 "tern": """    if (mode != NULL) {
        RwInt32 used;
        lbl_805E3190 = *mode;
        lbl_805F164C = &lbl_805E3190;
        lbl_805F15EC = 0x2A;
        used = (lbl_805F164C->gx.aa != 0) ? 2 : format;
""",
 "fmt_param": """    if (mode != NULL) {
        lbl_805E3190 = *mode;
        lbl_805F164C = &lbl_805E3190;
        lbl_805F15EC = 0x2A;
        {
        RwInt32 used = format;
        if (lbl_805F164C->gx.aa != 0) used = 2;
""",
}
V["tern2"] = V["tern"].replace("used = (lbl_805F164C->gx.aa != 0) ? 2 : format;", "used = (lbl_805F164C->gx.aa == 0) ? format : 2;")
V["tern3"] = V["tern"].replace("used = (lbl_805F164C->gx.aa != 0) ? 2 : format;", "used = (!lbl_805F164C->gx.aa) ? format : 2;")
V["tern4"] = V["tern"].replace("used = (lbl_805F164C->gx.aa != 0) ? 2 : format;", "used = lbl_805F164C->gx.aa ? 2 : format;")
V["if2"] = V["tern"].replace("used = (lbl_805F164C->gx.aa != 0) ? 2 : format;", "if (lbl_805F164C->gx.aa != 0) used = 2; else used = format;")
V["if3"] = V["tern"].replace("used = (lbl_805F164C->gx.aa != 0) ? 2 : format;", "if (lbl_805F164C->gx.aa == 0) used = format; else used = 2;")
T = "used = (lbl_805F164C->gx.aa != 0) ? 2 : format;"
NL = "\n"
V["blk_if"] = V["tern"].replace(T, "used = format;" + NL + "        if (lbl_805F164C->gx.aa != 0) used = 2;")
V["blk_if_aa"] = V["tern"].replace("        RwInt32 used;" + NL, "        RwInt32 used;" + NL + "        u8 aa;" + NL).replace(T, "aa = lbl_805F164C->gx.aa;" + NL + "        used = format;" + NL + "        if (aa != 0) used = 2;")
V["blk_if_aa2"] = V["tern"].replace("        RwInt32 used;" + NL, "        u8 aa;" + NL + "        RwInt32 used;" + NL).replace(T, "used = format;" + NL + "        aa = lbl_805F164C->gx.aa;" + NL + "        if (aa != 0) used = 2;")
V["blk_if_mode"] = V["tern"].replace(T, "used = format;" + NL + "        if (lbl_805E3190.gx.aa != 0) used = 2;")
V["fmt_reuse"] = V["tern"].replace(T, "if (lbl_805F164C->gx.aa != 0) format = 2;").replace("lbl_805F15E4 = used;" + NL + "        lbl_805F15E8 = used;", "lbl_805F15E4 = format;" + NL + "        lbl_805F15E8 = format;")
V["chain"] = V["tern"].replace(T, "lbl_805F15E8 = lbl_805F15E4 = (lbl_805F164C->gx.aa != 0) ? 2 : format;").replace("lbl_805F15E4 = used;" + NL + "        lbl_805F15E8 = used;" + NL, "")
V["chain2"] = V["tern"].replace(T, "lbl_805F15E4 = (lbl_805F164C->gx.aa != 0) ? 2 : format;" + NL + "        lbl_805F15E8 = lbl_805F15E4;").replace("lbl_805F15E4 = used;" + NL + "        lbl_805F15E8 = used;" + NL, "")
V["self"] = V["tern"].replace(T, "used = format;" + NL + "        used = (lbl_805F164C->gx.aa != 0) ? 2 : used;")
V["sw"] = V["tern"].replace(T, "switch (lbl_805F164C->gx.aa) {" + NL + "        case 0:" + NL + "            used = format;" + NL + "            break;" + NL + "        default:" + NL + "            used = 2;" + NL + "            break;" + NL + "        }")
V["inl"] = V["tern"].replace(T, "used = RwDlFrameBufferFormat(format);")
V["inl2"] = V["tern"].replace(T, "used = RwDlFrameBufferFormat2(format);")
V["inl3"] = V["tern"].replace(T, "used = RwDlFrameBufferFormat3(format);")
V["inl2b"] = V["tern"].replace("        RwInt32 used;" + NL, "").replace(T, "lbl_805F15E4 = RwDlFrameBufferFormat2(format);" + NL + "        lbl_805F15E8 = lbl_805F15E4;").replace("lbl_805F15E4 = used;" + NL + "        lbl_805F15E8 = used;" + NL, "")
V["t_var"] = V["tern"].replace("        RwInt32 used;" + NL, "        RwInt32 used;" + NL + "        RwInt32 aaFormat = 2;" + NL).replace(T, "used = (lbl_805F164C->gx.aa != 0) ? aaFormat : format;")
V["t_cvar"] = V["tern"].replace("        RwInt32 used;" + NL, "        RwInt32 used;" + NL + "        const RwInt32 aaFormat = 2;" + NL).replace(T, "used = (lbl_805F164C->gx.aa != 0) ? aaFormat : format;")
V["t_neg"] = V["tern"].replace(T, "used = (lbl_805F164C->gx.aa == 0) ? format : 2;")
V["t_bool"] = V["tern"].replace("        RwInt32 used;" + NL, "        RwInt32 used;" + NL + "        RwBool aa;" + NL).replace(T, "aa = lbl_805F164C->gx.aa != 0;" + NL + "        used = aa ? 2 : format;")
V["t_fmt"] = V["tern"].replace(T, "used = format;" + NL + "        used = (lbl_805F164C->gx.aa == 0) ? used : 2;")
BODIES = {SYM: {}}
for k, v in V.items():
    body = v + REST
    if k == "fmt_param":
        body = body.replace("            RW_ERROR_STRING(2, sInvalidPixelFormat);\n            break;\n        }\n    } else {", "            RW_ERROR_STRING(2, sInvalidPixelFormat);\n            break;\n        }\n        }\n    } else {")
    BODIES[SYM][k] = body
XTRA = {SYM: """static inline RwInt32 RwDlFrameBufferFormat(RwInt32 format) {
    if (lbl_805F164C->gx.aa != 0) return 2;
    return format;
}

static inline RwInt32 RwDlFrameBufferFormat2(RwInt32 format) {
    if (lbl_805F164C->gx.aa != 0) {
        format = 2;
    }
    return format;
}

static inline RwInt32 RwDlFrameBufferFormat3(RwInt32 format) {
    RwInt32 used = format;
    if (lbl_805F164C->gx.aa != 0) used = 2;
    return used;
}

"""}
