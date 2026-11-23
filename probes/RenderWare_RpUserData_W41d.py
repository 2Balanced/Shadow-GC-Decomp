"""Wave 41, RenderWare/RpUserData round 4: the list copy (fn_80452434)."""
SCORE = ["fn_80452434"]
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

OLD = _span(SRC, "/* Copies one list into another", "\n}\n", 3 - len("\n}\n"))
MACRO = """/* Duplicates a string (the argument expressions are evaluated again, as a macro does). */
#define USERDATA_STRING_COPY(dest, source)                                          \
    do {                                                                            \
        (dest) = NULL;                                                              \
        if ((source) != NULL) {                                                     \
            (dest) = (char *)rwAlloc(rwStringLength(source) + 1, 0x30002);          \
            if ((dest) != NULL) {                                                   \
                rwStringCopy((dest), (source));                                     \
            }                                                                       \
        }                                                                           \
    } while (0)

"""
def body(src_t, decls, hoist):
    s = """/* Copies one list into another (the copy callback's work): every array and string is duplicated. */
void fn_80452434(UserDataList *dest, %s *source) {
%s
    UserDataListDestroy(dest);
    dest->numElements = source->numElements;
    if (dest->numElements > 0) {
        dest->userData = (UserDataArray *)rwAlloc(dest->numElements * sizeof(UserDataArray), 0x3011F);
%s        for (i = 0; i < dest->numElements; i++) {
            from = &%s[i];
            to = &dest->userData[i];
            to->format = from->format;
            to->numElements = from->numElements;
            if (from->name != NULL) {
                USERDATA_STRING_COPY(to->name, from->name);
            }
            if (from->data != NULL) {
                size = to->numElements * fn_80452C98(to->format);
                to->data = rwAlloc(size, 0x3011F);
                if (to->format == 3) {
                    fromStrings = (char **)from->data;
                    toStrings = (char **)to->data;
                    for (j = 0; j < to->numElements; j++) {
                        if (fromStrings[j] == NULL) {
                            toStrings[j] = NULL;
                        } else {
                            USERDATA_STRING_COPY(toStrings[j], fromStrings[j]);
                        }
                    }
                } else {
                    memcpy(to->data, from->data, size);
                }
            }
        }
    }
}
""" % (src_t, decls, "        fromArrays = source->userData;\n" if hoist else "", "fromArrays" if hoist else "source->userData")
    return MACRO + s
D_A = """    s32 i;
    UserDataArray *fromArrays;
    UserDataArray *from;
    UserDataArray *to;
    u32 size;
    char **fromStrings;
    char **toStrings;
    s32 j;
"""
D_B = """    UserDataArray *from;
    UserDataArray *to;
    char **fromStrings;
    char **toStrings;
    u32 size;
    s32 j;
    s32 i;
    UserDataArray *fromArrays;
"""
D_C = """    UserDataArray *fromArrays;
    s32 i;
    UserDataArray *from;
    UserDataArray *to;
    s32 j;
    char **fromStrings;
    char **toStrings;
    u32 size;
"""
REPL = {"base": []}
for st in ("UserDataList", "const UserDataList"):
    for dn, d in (("A", D_A), ("B", D_B), ("C", D_C)):
        for h in (0, 1):
            REPL["c_%s_%s_%d" % ("const" if "const" in st else "mut", dn, h)] = [(OLD, body(st, d, h))]
import itertools
DV = {"fa": "    UserDataArray *fromArrays;\n", "to": "    UserDataArray *to;\n", "fr": "    UserDataArray *from;\n",
      "i": "    s32 i;\n", "j": "    s32 j;\n", "fs": "    char **fromStrings;\n", "ts": "    char **toStrings;\n", "sz": "    u32 size;\n"}
REPL = {}
for perm in itertools.permutations(["fa", "to", "fr", "i", "j"]):
    for tail_first in (0, 1):
        rest = ["fs", "ts", "sz"]
        order = (rest + list(perm)) if tail_first else (list(perm) + rest)
        d = "".join(DV[k] for k in order)
        REPL["p%d_%s" % (tail_first, "_".join(perm))] = [(OLD, body("const UserDataList", d, 0))]
def body2(arr_sig, hdr_decl, call):
    return MACRO + """/* Copies one array into another. */
static inline void UserDataArrayCopy(%s) {
    u32 size;
    char **fromStrings;
    char **toStrings;
    s32 j;

    to->format = from->format;
    to->numElements = from->numElements;
    if (from->name != NULL) {
        USERDATA_STRING_COPY(to->name, from->name);
    }
    if (from->data != NULL) {
        size = to->numElements * fn_80452C98(to->format);
        to->data = rwAlloc(size, 0x3011F);
        if (to->format == 3) {
            fromStrings = (char **)from->data;
            toStrings = (char **)to->data;
            for (j = 0; j < to->numElements; j++) {
                if (fromStrings[j] == NULL) {
                    toStrings[j] = NULL;
                } else {
                    USERDATA_STRING_COPY(toStrings[j], fromStrings[j]);
                }
            }
        } else {
            memcpy(to->data, from->data, size);
        }
    }
}

/* Copies one list into another (the copy callback's work): every array and string is duplicated. */
void fn_80452434(UserDataList *dest, const UserDataList *source) {
%s
    UserDataListDestroy(dest);
    dest->numElements = source->numElements;
    if (dest->numElements > 0) {
        dest->userData = (UserDataArray *)rwAlloc(dest->numElements * sizeof(UserDataArray), 0x3011F);
        for (i = 0; i < dest->numElements; i++) {
            %s
        }
    }
}
""" % (arr_sig, hdr_decl, call)
REPL.update({
    "h1": [(OLD, body2("UserDataArray *to, const UserDataArray *from", "    s32 i;\n", "UserDataArrayCopy(&dest->userData[i], &source->userData[i]);"))],
    "h2": [(OLD, body2("UserDataArray *to, UserDataArray *from", "    s32 i;\n", "UserDataArrayCopy(&dest->userData[i], &source->userData[i]);"))],
    "h3": [(OLD, body2("UserDataArray *to, const UserDataArray *from", "    s32 i;\n    const UserDataArray *fromArrays;\n", "UserDataArrayCopy(&dest->userData[i], &fromArrays[i]);").replace("        for (i = 0;", "        fromArrays = source->userData;\n        for (i = 0;"))],
    "h4": [(OLD, body2("const UserDataArray *from, UserDataArray *to", "    s32 i;\n", "UserDataArrayCopy(&source->userData[i], &dest->userData[i]);"))],
})
