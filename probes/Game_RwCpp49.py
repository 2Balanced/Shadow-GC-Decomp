# Probes for Game/RwCpp49 (cflags_rw_r6). Round 1: "#pragma auto_inline off" around the path
# search keeps it out of line (fn_80488B58 0 rows); fn_80488A70 matched with format declared
# before the end pointer. Round 2: image/mask declaration orders in fn_80488B94 (no change).
# Round 3: loop locals of fn_804886CC in six orders and hoisted (22 rows, register pair swap).
# Round 4: RwImageMakeMask (fn_80487FA8): function-scope locals 34-35 rows, u8 max / < / ternary
# spellings 29-39 (base 23).
# Round 5: RwImageApplyMask (fn_80488088): temp declared first, width/height/depth locals, four
# spellings of the palette index (GC/2.7 always masks the byte index with rlwinm): 92-96 rows.
H = """RwImage *fn_80488088(RwImage *image, const RwImage *mask) {
    if (image->width != mask->width || image->height != mask->height) {
"""
T = """        RwImage *temp = fn_80487D08(image->width, image->height, image->depth);
"""
REPL = {
    "base": [],
    "wloc": [(H, """RwImage *fn_80488088(RwImage *image, const RwImage *mask) {
    s32 width = image->width;
    s32 height;

    if (width != mask->width || (height = image->height) != mask->height) {
"""), (T, "        RwImage *temp = fn_80487D08(width, height, image->depth);\n")],
    "tloc": [(H, """RwImage *fn_80488088(RwImage *image, const RwImage *mask) {
    RwImage *temp;
    s32 width = image->width;

    if (width != mask->width || image->height != mask->height) {
"""), (T, "        temp = fn_80487D08(width, image->height, image->depth);\n")],
}
SCORE = ["fn_80488088"]
