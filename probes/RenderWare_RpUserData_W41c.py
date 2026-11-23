"""Wave 41, RenderWare/RpUserData round 3: the stream write callback."""
SCORE = ["fn_8045294C"]
OLD_A = OLD = None
import re
SRC = open("src/RenderWare/RpUserData.c", newline="").read().replace("\r\n", "\n")


def _span(text, start, end, after=0):
    """The text from start up to and including end (searched from start); a marker when the
    anchor is gone because a variant of this table was kept in the source."""
    try:
        a = text.index(start)
        b = text.index(end, a) + len(end) + after
    except ValueError:
        return "<<anchor gone: a variant of this table is in the source>>"
    return text[a:b]

OLD = _span(SRC, "/* The number of arrays that have a format. */", "/* The stream size of one array. */")[:-len("/* The stream size of one array. */")]
def helper(count_t, two_i, cnt_decl_first):
    decl = ("    %s count = 0;\n    s32 i;\n" % count_t) if cnt_decl_first else ("    s32 i;\n    %s count = 0;\n" % count_t)
    i2 = "        s32 j;\n" if two_i else ""
    jj = "j" if two_i else "i"
    return """/* Writes the number of arrays that have a format, then those arrays. */
static inline void *UserDataListStreamWrite(UserDataList *list, void *stream) {
    if (list != NULL && list->numElements > 0) {
""" + decl.replace("    ", "        ") + i2 + """
        for (i = 0; i < list->numElements; i++) {
            if (list->userData[i].format != 0) {
                count++;
            }
        }
        if (count != 0 && fn_8047D458(stream, &count, 4) != 0) {
            for (%s = 0; %s < list->numElements; %s++) {
                if (list->userData[%s].format != 0) {
                    stream = fn_80452228((u32 *)&list->userData[%s], stream);
                }
            }
        }
    }
    return stream;
}

/* Object stream write callback. */
void *fn_8045294C(void *stream, u32 size, u8 *object, u32 offset)
{
    UserDataList *list = (UserDataList *)(object + offset);

    UserDataListStreamWrite(list, stream);
    return stream;
}

""" % (jj, jj, jj, jj, jj)
REPL = {"base": []}
for ct in ("u32", "s32"):
    for two in (False, True):
        for cf in (True, False):
            REPL["w_%s_%d_%d" % (ct, two, cf)] = [(OLD, helper(ct, two, cf))]
