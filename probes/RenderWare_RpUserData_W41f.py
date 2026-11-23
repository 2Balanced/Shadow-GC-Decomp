"""Wave 41, RenderWare/RpUserData round 6: array destroy without a strings local."""
SCORE = ["fn_80452434", "fn_80452744"]
OLD = """    if (array->format == 3) {
        char **strings = (char **)array->data;
        s32 i;

        for (i = 0; i < array->numElements; i++) {
            if (strings[i] != NULL) {
                rwFree(strings[i]);
            }
        }
    }"""
REPL = {
    "base": [],
    "nostr": [(OLD, """    if (array->format == 3) {
        s32 i;

        for (i = 0; i < array->numElements; i++) {
            if (((char **)array->data)[i] != NULL) {
                rwFree(((char **)array->data)[i]);
            }
        }
    }""")],
    "nostr_top": [(OLD, """    if (array->format == 3) {
        for (i = 0; i < array->numElements; i++) {
            if (((char **)array->data)[i] != NULL) {
                rwFree(((char **)array->data)[i]);
            }
        }
    }"""), ("static inline void UserDataArrayDestroy(UserDataArray *array) {\n", "static inline void UserDataArrayDestroy(UserDataArray *array) {\n    s32 i;\n\n")],
    "str_top": [(OLD, """    if (array->format == 3) {
        strings = (char **)array->data;
        for (i = 0; i < array->numElements; i++) {
            if (strings[i] != NULL) {
                rwFree(strings[i]);
            }
        }
    }"""), ("static inline void UserDataArrayDestroy(UserDataArray *array) {\n", "static inline void UserDataArrayDestroy(UserDataArray *array) {\n    s32 i;\n    char **strings;\n\n")],
}
