"""Wave 41, RenderWare/RpUserData: the loops written in inline helpers (zeroed-counter copy)."""
SCORE = ["fn_80452B90", "fn_80452C04"]
OLD_B90 = """u32 fn_80452B90(u8 *object)
{
    u32 found = 0;
    u32 offset = found;
    u32 *dictionary = (u32 *)(object + userDataGeometryOffset);
    { s32 i; for (i = found; i < (s32)dictionary[0]; i++, offset += 0x10) {
        if (*(u32 *)(dictionary[1] + offset + 0x0C) != 0) {
            found++;
        }
    } }
    return found;
}"""
HELP = """typedef struct UserDataArray {
    char *name;
    s32 format;
    s32 numElements;
    void *data;
} UserDataArray;

typedef struct UserDataList {
    s32 numElements;
    UserDataArray *userData;
} UserDataList;

static inline s32 UserDataListCount(UserDataList *list) {
    s32 count = 0;
    s32 i;

    for (i = 0; i < list->numElements; i++) {
        if (list->userData[i].data != NULL) {
            count++;
        }
    }
    return count;
}

"""
NEW_B90 = HELP + """s32 fn_80452B90(u8 *object)
{
    return UserDataListCount((UserDataList *)(object + userDataGeometryOffset));
}"""
NEW_B90_u = NEW_B90.replace("static inline s32 UserDataListCount(UserDataList *list) {\n    s32 count = 0;\n    s32 i;", "static inline s32 UserDataListCount(UserDataList *list) {\n    s32 i;\n    s32 count = 0;")
NEW_B90_c = NEW_B90.replace("static inline s32 UserDataListCount(UserDataList *list) {", "static inline s32 UserDataListCount(const UserDataList *list) {")
REPL = {
    "base": [],
    "inline": [(OLD_B90, NEW_B90)],
    "inline_i_first": [(OLD_B90, NEW_B90_u)],
    "inline_const": [(OLD_B90, NEW_B90_c)],
}
HELP_A = HELP.replace("""static inline s32 UserDataListCount(UserDataList *list) {
    s32 count = 0;
    s32 i;
""", """static inline s32 UserDataListCount(u8 *object, u32 offset) {
    s32 count = 0;
    s32 i;
    UserDataList *list = (UserDataList *)(object + offset);
""")
NEW_A = HELP_A + """s32 fn_80452B90(u8 *object)
{
    return UserDataListCount(object, userDataGeometryOffset);
}"""
HELP_B = HELP.replace("""static inline s32 UserDataListCount(UserDataList *list) {
    s32 count = 0;
    s32 i;
""", """static inline s32 UserDataListCount(u8 *object, u32 offset) {
    s32 count = 0;
    s32 i;
    UserDataList *list;

    list = (UserDataList *)(object + offset);
""")
NEW_B = HELP_B + """s32 fn_80452B90(u8 *object)
{
    return UserDataListCount(object, userDataGeometryOffset);
}"""
NEW_C = HELP + """s32 fn_80452B90(u8 *object)
{
    UserDataList *list = (UserDataList *)(object + userDataGeometryOffset);
    return UserDataListCount(list);
}"""
HELP_D = HELP.replace("""static inline s32 UserDataListCount(UserDataList *list) {
    s32 count = 0;
    s32 i;
""", """static inline s32 UserDataListCount(UserDataList *list) {
    s32 i;
    s32 count;

    count = 0;
""")
NEW_D = HELP_D + """s32 fn_80452B90(u8 *object)
{
    return UserDataListCount((UserDataList *)(object + userDataGeometryOffset));
}"""
REPL.update({
    "A": [(OLD_B90, NEW_A)],
    "B": [(OLD_B90, NEW_B)],
    "C": [(OLD_B90, NEW_C)],
    "D": [(OLD_B90, NEW_D)],
})
