"""Wave 41: UnkScreenBlur_8042DA00::Draw (retail: x r0, width r10, y r8, height r11)."""
SCORE = ["Draw__22UnkScreenBlur_8042DA00Fv"]
H = "void UnkScreenBlur_8042DA00::Draw() {\n"
P0 = "#pragma opt_propagation off\n"
W = "    s32 width = frameBuffer->width;\n    s32 height = frameBuffer->height;\n"
REPL = {
  "prag": [(H, P0 + H)],
  "wh_after_screen": [(W + "    RwRaster_8042DA00 *screen = frameBuffer->parent;\n", "    RwRaster_8042DA00 *screen = frameBuffer->parent;\n" + W)],
  "wh_before_xy": [("    s16 x;\n    s16 y;\n    fn_80489B2C(frameBuffer, &x, &y);\n" + W, W + "    s16 x;\n    s16 y;\n    fn_80489B2C(frameBuffer, &x, &y);\n")],
  "wh_top": [("    int srcBlend;\n", "    s32 width;\n    s32 height;\n    int srcBlend;\n"), (W, "    width = frameBuffer->width;\n    height = frameBuffer->height;\n")],
  "xy_ints": [("    RwV2d_8042DA00 u = {", "    s32 sx = x;\n    s32 sy = y;\n    RwV2d_8042DA00 u = {")],
  "prag_xy_ints": [(H, P0 + H), ("    RwV2d_8042DA00 u = {", "    s32 sx = x;\n    s32 sy = y;\n    RwV2d_8042DA00 u = {")],
  "u32wh": [(W, "    u32 width = frameBuffer->width;\n    u32 height = frameBuffer->height;\n")],
}
