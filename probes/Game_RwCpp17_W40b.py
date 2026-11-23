"""Wave 40 (r2), RwCpp17 round 2: the stream write callbacks fn_80454B20/fn_80454C40 take the
object through a pointer to const (retail loads it above the prologue's store)."""
SCORE = ["fn_80454B20", "fn_80454C40"]
REPL = {
 "base": [("void *fn_80454B20(void *stream, void *entry, u32 *object) {", "void *fn_80454B20(void *stream, void *entry, u32 *object) {")],
 "c": [("void *fn_80454B20(void *stream, void *entry, u32 *object) {", "void *fn_80454B20(void *stream, s32 length, const u32 *object) {"),
       ("void *fn_80454C40(void *stream, void *entry, u32 *object) {", "void *fn_80454C40(void *stream, s32 length, const u32 *object) {")],
}
