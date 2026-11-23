"""Wave 41, RenderWare/RpUserData round 5: the list copy's destroy part as its own inline helper."""
import itertools
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

SIG = "/* Copies one list into another (the copy callback's work): every array and string is duplicated. */\n"
CALL = "    UserDataListDestroy(dest);\n"
def helper(decls):
    return """/* Empties a list before a copy. */
static inline void UserDataListEmpty(UserDataList *list) {
    if (list->userData != NULL) {
""" + decls + """
        for (i = 0; i < list->numElements; i++) {
            array = &list->userData[i];
            if (array->name != NULL) {
                rwFree(array->name);
                array->name = NULL;
            }
            if (array->format == 3) {
                strings = (char **)array->data;
                for (j = 0; j < array->numElements; j++) {
                    if (strings[j] != NULL) {
                        rwFree(strings[j]);
                    }
                }
            }
            if (array->data != NULL) {
                rwFree(array->data);
                array->data = NULL;
            }
        }
        rwFree(list->userData);
    }
    list->userData = NULL;
    list->numElements = 0;
}

"""
DV = {"a": "        UserDataArray *array;\n", "i": "        s32 i;\n", "j": "        s32 j;\n", "s": "        char **strings;\n"}
REPL = {"base": []}
for perm in itertools.permutations("aijs"):
    REPL["e_" + "".join(perm)] = [(SIG, helper("".join(DV[k] for k in perm)) + SIG), (CALL, "    UserDataListEmpty(dest);\n")]
