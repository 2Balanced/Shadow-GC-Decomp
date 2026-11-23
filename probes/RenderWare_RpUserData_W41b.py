"""Wave 41, RenderWare/RpUserData round 2: stream read/size callbacks."""
SCORE = ["fn_80452898", "fn_80452A40"]
R_OLD = """    UserDataList *list = (UserDataList *)(object + offset);

    if (list != NULL) {
        UserDataListStreamRead(list, stream);
    }
    return stream;"""
S_OLD = """    UserDataList *list = (UserDataList *)(object + offset);
    s32 size = 0;

    if (list != NULL && list->numElements > 0) {
        s32 i;

        for (i = 0; i < list->numElements; i++) {
            if (list->userData[i].format != 0) {
                size += UserDataArrayGetStreamSize(&list->userData[i]);
            }
        }
        if (size > 0) {
            size += 4;
        }
    }
    return size;"""
S_HELP = """static inline s32 UserDataListGetStreamSize(UserDataList *list) {
    s32 size = 0;

    if (list != NULL && list->numElements > 0) {
        s32 i;

        for (i = 0; i < list->numElements; i++) {
            if (list->userData[i].format != 0) {
                size += UserDataArrayGetStreamSize(&list->userData[i]);
            }
        }
        if (size > 0) {
            size += 4;
        }
    }
    return size;
}

/* Object stream size callback. */
s32 fn_80452A40("""
S_SIG = """/* Object stream size callback. */
s32 fn_80452A40("""
REPL = {
    "base": [],
    "r_current": [(R_OLD, """    UserDataList *list = (UserDataList *)(object + offset);
    void *current = stream;

    if (list != NULL) {
        UserDataListStreamRead(list, current);
    }
    return stream;""")],
    "r_inline_all": [(R_OLD, """    UserDataList *list = (UserDataList *)(object + offset);
    void *current = stream;

    if (list != NULL) {
        s32 numElements;
        s32 i;

        if (fn_8047D868(stream, &numElements, 4) != 0) {
            list->numElements = numElements;
            list->userData = (UserDataArray *)rwAlloc(list->numElements * sizeof(UserDataArray), 0x3011F);
            for (i = 0; i < list->numElements; i++) {
                current = fn_80451F70((u32 *)&list->userData[i], current);
            }
        }
    }
    return stream;""")],
    "s_helper": [(S_SIG, S_HELP), (S_OLD, """    return UserDataListGetStreamSize((UserDataList *)(object + offset));""")],
    "s_helper_local": [(S_SIG, S_HELP), (S_OLD, """    UserDataList *list = (UserDataList *)(object + offset);
    return UserDataListGetStreamSize(list);""")],
}
R_HELP_OLD = """static inline void *UserDataListStreamRead(UserDataList *list, void *stream) {
    s32 numElements;
    s32 i;

    if (fn_8047D868(stream, &numElements, 4) != 0) {
        list->numElements = numElements;
        list->userData = (UserDataArray *)rwAlloc(list->numElements * sizeof(UserDataArray), 0x3011F);
        for (i = 0; i < list->numElements; i++) {
            stream = fn_80451F70((u32 *)&list->userData[i], stream);
        }
    }
    return stream;
}"""
R_HELP_NEW = """static inline void *UserDataListStreamRead(UserDataList *list, void *stream) {
    if (list != NULL) {
        s32 numElements;
        s32 i;

        if (fn_8047D868(stream, &numElements, 4) != 0) {
            list->numElements = numElements;
            list->userData = (UserDataArray *)rwAlloc(list->numElements * sizeof(UserDataArray), 0x3011F);
            for (i = 0; i < list->numElements; i++) {
                stream = fn_80451F70((u32 *)&list->userData[i], stream);
            }
        }
    }
    return stream;
}"""
REPL.update({
    "r_nullin": [(R_HELP_OLD, R_HELP_NEW), (R_OLD, """    UserDataList *list = (UserDataList *)(object + offset);

    UserDataListStreamRead(list, stream);
    return stream;""")],
})
def shelp(decl, body_i_decl=True):
    h = S_HELP.replace("""    s32 size = 0;

    if (list != NULL && list->numElements > 0) {
        s32 i;
""", decl)
    return h
D1 = """    s32 i;
    s32 size = 0;

    if (list != NULL && list->numElements > 0) {
"""
D2 = """    s32 i;
    s32 size;

    size = 0;
    if (list != NULL && list->numElements > 0) {
"""
D3 = """    s32 size;
    s32 i;

    size = 0;
    if (list != NULL && list->numElements > 0) {
"""
STR_OLD = """                if (strings[i] != NULL) {
                    size += rwStringLength(strings[i]) + 1;
                }"""
STR_NEW = """                if (strings[i] != NULL) {
                    size += (s32)(rwStringLength(strings[i]) + 1);
                }"""
STR_NEW2 = """                if (strings[i] != NULL) {
                    s32 length = rwStringLength(strings[i]) + 1;
                    size += length;
                }"""
REPL.update({
    "s_d1": [(S_SIG, shelp(D1)), (S_OLD, "    UserDataList *list = (UserDataList *)(object + offset);\n    return UserDataListGetStreamSize(list);")],
    "s_d2": [(S_SIG, shelp(D2)), (S_OLD, "    UserDataList *list = (UserDataList *)(object + offset);\n    return UserDataListGetStreamSize(list);")],
    "s_d3": [(S_SIG, shelp(D3)), (S_OLD, "    UserDataList *list = (UserDataList *)(object + offset);\n    return UserDataListGetStreamSize(list);")],
    "s_d1_str": [(S_SIG, shelp(D1)), (S_OLD, "    UserDataList *list = (UserDataList *)(object + offset);\n    return UserDataListGetStreamSize(list);"), (STR_OLD, STR_NEW)],
    "s_d1_str2": [(S_SIG, shelp(D1)), (S_OLD, "    UserDataList *list = (UserDataList *)(object + offset);\n    return UserDataListGetStreamSize(list);"), (STR_OLD, STR_NEW2)],
    "s_d2_str2": [(S_SIG, shelp(D2)), (S_OLD, "    UserDataList *list = (UserDataList *)(object + offset);\n    return UserDataListGetStreamSize(list);"), (STR_OLD, STR_NEW2)],
})
A_OLD = """static inline s32 UserDataArrayGetStreamSize(UserDataArray *array) {
    s32 size = 0;

    if (array != NULL) {
        size = 4;
        if (array->name != NULL) {
            size = rwStringLength(array->name) + 5;
        }
        size += 8;
        switch (array->format) {
        case 1:
            size += array->numElements * 4;
            break;
        case 2:
            size += array->numElements * 4;
            break;
        case 3: {
            char **strings = (char **)array->data;
            s32 i;

            for (i = 0; i < array->numElements; i++) {
                size += 4;
                if (strings[i] != NULL) {
                    size += rwStringLength(strings[i]) + 1;
                }
            }
            break;
        }
        }
    }
    return size;
}"""
STRSZ = """static inline s32 UserDataStringGetStreamSize(const char *string) {
    if (string != NULL) {
        return rwStringLength(string) + 1;
    }
    return 0;
}

"""
def arr(decl_top, name_line, str_lines):
    return STRSZ + """static inline s32 UserDataArrayGetStreamSize(UserDataArray *array) {
""" + decl_top + """
    if (array != NULL) {
""" + name_line + """
        size += 8;
        switch (array->format) {
        case 1:
            size += array->numElements * 4;
            break;
        case 2:
            size += array->numElements * 4;
            break;
        case 3:
            strings = (char **)array->data;
            for (i = 0; i < array->numElements; i++) {
""" + str_lines + """
            }
            break;
        }
    }
    return size;
}"""
DT1 = "    s32 size = 0;\n    char **strings;\n    s32 i;\n"
DT2 = "    s32 i;\n    char **strings;\n    s32 size = 0;\n"
DT3 = "    char **strings;\n    s32 i;\n    s32 size = 0;\n"
N1 = "        size = 4 + UserDataStringGetStreamSize(array->name);"
N2 = "        size = 4;\n        size += UserDataStringGetStreamSize(array->name);"
S1 = "                size += 4 + UserDataStringGetStreamSize(strings[i]);"
S2 = "                size += 4;\n                size += UserDataStringGetStreamSize(strings[i]);"
SBASE = [(S_SIG, shelp(D1)), (S_OLD, "    UserDataList *list = (UserDataList *)(object + offset);\n    return UserDataListGetStreamSize(list);")]
for dn, dt in (("t1", DT1), ("t2", DT2), ("t3", DT3)):
    for nn, nl in (("n1", N1), ("n2", N2)):
        for sn, sl in (("s1", S1), ("s2", S2)):
            REPL["a_%s_%s_%s" % (dn, nn, sn)] = SBASE + [(A_OLD, arr(dt, nl, sl))]
SEXPR = {
 "e1": "size += 1 + rwStringLength(strings[i]);",
 "e2": "size = size + rwStringLength(strings[i]) + 1;",
 "e3": "size = (rwStringLength(strings[i]) + 1) + size;",
 "e4": "size = size + (rwStringLength(strings[i]) + 1);",
 "e0": "size += rwStringLength(strings[i]) + 1;",
}
A_DECL_OLD = """    s32 size = 0;

    if (array != NULL) {
        size = 4;"""
A_CASE_OLD = """        case 3: {
            char **strings = (char **)array->data;
            s32 i;

            for (i = 0; i < array->numElements; i++) {"""
for en, ex in SEXPR.items():
    REPL["x_" + en] = SBASE + [("size += rwStringLength(strings[i]) + 1;", ex)]
    REPL["xd_" + en] = SBASE + [("size += rwStringLength(strings[i]) + 1;", ex),
        (A_DECL_OLD, """    s32 i;
    char **strings;
    s32 size = 0;

    if (array != NULL) {
        size = 4;"""),
        (A_CASE_OLD, """        case 3:
            strings = (char **)array->data;
            {
            for (i = 0; i < array->numElements; i++) {"""),
        ("""                }
            }
            break;
        }
        }
    }
    return size;""", """                }
            }
            }
            break;
        }
    }
    return size;""")]
