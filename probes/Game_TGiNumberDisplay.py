"""Source shapes for fn_800BC7BC in Game/TGiNumberDisplay.cpp (0x800BC7BC, 880 bytes).

The base shape (bottom edge computed at the store, not folded into `height`) matches retail
instruction for instruction except for two rows in the alignment switch:

    retail  fneg f0,f0        ours  fneg f1,f0
            lfs  f1,0(0)            lfs  f0,0(0)
            fmuls f27,f1,f0         fmuls f27,f1,f0

i.e. retail negates `total` in place and loads 0.5f into the free f1, ours negates into f1 and
loads the constant over f0.  Everything else in the function (218 rows) is identical, so the
shapes below only respell the two offset expressions.
"""

SYMBOLS = {
    "fn_800BC7BC": 'extern "C" bool fn_800BC7BC(TGiNumberDisplay *display, const char *text, '
                   "float x, int align,\n                            float y) {",
}

HEAD = """    if (!lbl_805E3780) {
        return true;
    }
    if (display->m_raster == 0) {
        return false;
    }
    int length = strlen(text);
    if (length <= 0) {
        return true;
    }
    float width = display->m_cellWidth * display->m_scaleX;
    float height = Scale(display->m_cellHeight * display->m_scaleY, 1.0f);
    float total = width * length + display->field_0x14 * (length - 1);
"""

TAIL = """    void *cullMode;
    void *filter;
    void *zTest;
    void *zWrite;
    lbl_805F15B4->fpRenderStateGet(20, &cullMode);
    lbl_805F15B4->fpRenderStateGet(9, &filter);
    lbl_805F15B4->fpRenderStateGet(6, &zTest);
    lbl_805F15B4->fpRenderStateGet(8, &zWrite);
    lbl_805F15B4->fpRenderStateSet(20, (void *)1);
    lbl_805F15B4->fpRenderStateSet(9, (void *)2);
    lbl_805F15B4->fpRenderStateSet(6, (void *)0);
    lbl_805F15B4->fpRenderStateSet(8, (void *)0);
    lbl_805F15B4->fpRenderStateSet(1, display->m_raster);
    for (int i = 0; i < length; i++, text++) {
        int glyph;
        switch ((u8)*text) {
        case ':':
            glyph = 10;
            break;
        case '/':
            glyph = 11;
            break;
        case ' ':
            glyph = -1;
            break;
        default:
            glyph = (u8)*text - '0';
            break;
        }
        if (glyph >= 0) {
            TGiVertex *quad = display->m_glyphs[glyph];
            float left = x + offset;
            float right = width + left;
            quad[0].x = left;
            quad[1].x = right;
            quad[2].x = left;
            quad[3].x = right;
            quad[0].y = y;
            quad[1].y = y;
            quad[2].y = y + height;
            quad[3].y = y + height;
            lbl_805F15B4->fpIm2DRenderPrimitive(4, quad, 4);
        }
        offset += width + display->field_0x14;
    }
    lbl_805F15B4->fpRenderStateSet(1, (void *)0);
    lbl_805F15B4->fpRenderStateSet(20, cullMode);
    lbl_805F15B4->fpRenderStateSet(9, filter);
    lbl_805F15B4->fpRenderStateSet(6, zTest);
    lbl_805F15B4->fpRenderStateSet(8, zWrite);
    return true;
"""

CASE1 = {
    "negmul": "offset = -total * 0.5f;",
    "halfrev": "offset = 0.5f * -total;",
    "negparen": "offset = -(total * 0.5f);",
    "negparen2": "offset = -(0.5f * total);",
    "mulneg": "offset = total * -0.5f;",
    "scale_neg": "offset = Scale(-total, 0.5f);",
    "scale_total": "offset = Scale(total, -0.5f);",
    "scale_neg1": "offset = Scale(0.5f * total, -1.0f);",
    "div2": "offset = -total / 2.0f;",
    "div2b": "offset = -(total / 2.0f);",
    "twostep": "offset = -total;\n        offset *= 0.5f;",
    "twostep2": "offset = total * 0.5f;\n        offset = -offset;",
    "halfconst": "offset = -total * (1.0f / 2.0f);",
}

CASE2 = {
    "neg": "offset = -total;",
    "scale1": "offset = Scale(-total, 1.0f);",
    "mulneg1": "offset = -1.0f * total;",
    "sub": "offset = 0.0f - total;",
}


def shape(case1, case2="offset = -total;"):
    return (
        "\n"
        + HEAD
        + "    float offset;\n"
        + "    switch (align) {\n    case 1:\n        "
        + case1.replace("\n", "\n        ")
        + "\n        break;\n    case 2:\n        "
        + case2
        + "\n        break;\n    default:\n        offset = 0.0f;\n        break;\n    }\n"
        + TAIL
    )


BODIES = {"fn_800BC7BC": {"base": shape(CASE1["negmul"])}}
for _n, _c1 in CASE1.items():
    if _n != "negmul":
        BODIES["fn_800BC7BC"][_n] = shape(_c1)
for _n, _c2 in CASE2.items():
    if _n != "neg":
        BODIES["fn_800BC7BC"]["negmul_" + _n] = shape(CASE1["negmul"], _c2)
for _n in ("scale_neg", "halfrev", "negparen"):
    BODIES["fn_800BC7BC"][_n + "_scale1"] = shape(CASE1[_n], CASE2["scale1"])
