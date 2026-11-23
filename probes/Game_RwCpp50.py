# Probes for Game/RwCpp50 (cflags_rw_r6). RwImageCopy: 120 declaration orders and 24 assignment
# orders of its row loop locals, best 4 rows (rowBytes and srcStride trade r25/r29).
# RwRasterCreate matched with the device's create function read into a local first.
# RwImageSetGamma matched with the scaled value in a local (the add is not fused).
# The image conversion (fn_8048915C): the 720 declaration orders of its locals, best 74 rows
# (from 78), register numbering throughout.
# RwImageGammaCorrect (fn_804894B8): loop forms of the colour helper; retail unrolls by 4,
# GC/2.7 by 2 for every spelling tried (u32/s32 for, while (count--), do-while, indexed,
# increments folded in, table read in place); kept while (count--) with s32, 54 rows.
# Resampling: the row end in fn_8048A790, the start of fn_8048A178.
E = "        s32 srcYEnd = srcY + (yStep - 1);\n"
X = "            fn_8048A50C(sourceImage, srcX, srcX + (xStep - 1), srcY, srcYEnd, &color);\n"
H = """    f32 width = (f32)(x1 - x0) * lbl_805FB328;
    s32 first = x0 >> 16;
    s32 last = x1 >> 16;
    const u8 *pixel = image->cpPixels + (y >> 16) * image->stride + first * 4;
"""
REPL = {
    "base": [],
    "rev": [(E, "        s32 srcYEnd = (yStep - 1) + srcY;\n"), (X, "            fn_8048A50C(sourceImage, srcX, (xStep - 1) + srcX, srcY, srcYEnd, &color);\n")],
    "tmp": [(E, "        s32 srcYEnd = yStep - 1;\n"), ("        u8 *pixel = destImage->cpPixels + destImage->stride * y;\n", "        u8 *pixel;\n"),
            ("        s32 srcX = 0;\n        s32 x;\n\n", "        s32 srcX = 0;\n        s32 x;\n\n        srcYEnd = srcY + srcYEnd;\n        pixel = destImage->cpPixels + destImage->stride * y;\n")],
    "h1": [(H, """    const u8 *pixel;
    f32 width;
    s32 first = x0 >> 16;
    s32 last = x1 >> 16;

    pixel = image->cpPixels + (y >> 16) * image->stride + first * 4;
    width = (f32)(x1 - x0) * lbl_805FB328;
""")],
    "h2": [(H, """    s32 first = x0 >> 16;
    s32 last = x1 >> 16;
    f32 width = (f32)(x1 - x0) * lbl_805FB328;
    const u8 *pixel = image->cpPixels + (y >> 16) * image->stride + first * 4;
""")],
}
SCORE = ["fn_8048A790", "fn_8048A178"]
