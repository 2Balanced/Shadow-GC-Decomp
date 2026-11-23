# Round 4 for src/RenderWare/RtFSyst.c.
SYMBOLS = {
    "fn_80479698": "const char *fn_80479698(const char *name) {",
    "fn_8047971C": "char *fn_8047971C(char *buffer, s32 size, const char *prefix, const char *name, char separator) {",
    "fn_804799C8": "void fn_804799C8(RtFile *file) {",
    "fn_80479A80": "void fn_80479A80(void) {",
}
BODIES = {
    "fn_80479698": {
        "skip_a": """
    const char *p = name;
    s32 i = 0;

    while (i < (s32)RWSRCGLOBAL(vecStrlen)(name)) {
        if (*p == ':') {
            name = name + i;
            name++;
            break;
        }
        i++;
        p++;
    }
    return name;
""",
        "skip_b": """
    const char *p = name;
    s32 i = 0;

    while (i < (s32)RWSRCGLOBAL(vecStrlen)(name)) {
        if (*p == ':') {
            name += i + 1;
            break;
        }
        i++;
        p++;
    }
    return name;
""",
        "skip_c": """
    const char *p = name;
    s32 i = 0;

    while (i < (s32)RWSRCGLOBAL(vecStrlen)(name)) {
        if (*p == ':') {
            name = &name[i];
            name++;
            break;
        }
        i++;
        p++;
    }
    return name;
""",
        "skip_d": """
    const char *p = name;
    s32 i = 0;

    while (i < (s32)RWSRCGLOBAL(vecStrlen)(name)) {
        if (*p == ':') {
            name = &name[i + 1];
            break;
        }
        i++;
        p++;
    }
    return name;
""",
    },
    "fn_8047971C": {
        "join_a": """
    s32 prefixLength = strlen(prefix);
    s32 nameLength = strlen(name);
    const char *src;
    char *dst;
    s32 i;

    if (prefixLength + nameLength > size - 1) {
        return 0;
    }
    RWSRCGLOBAL(vecStrcpy)(buffer, prefix);
    src = name;
    dst = buffer + prefixLength;
    for (i = 0; i <= nameLength; i++) {
        if (*src == '/' || *src == '\\') {
            *dst = separator;
        } else {
            *dst = *src;
        }
        dst++;
        src++;
    }
    return buffer;
""",
        "join_b": """
    s32 prefixLength = strlen(prefix);
    s32 nameLength = strlen(name);
    char *dst;
    const char *src;
    s32 i;

    if (prefixLength + nameLength > size - 1) {
        return 0;
    }
    RWSRCGLOBAL(vecStrcpy)(buffer, prefix);
    dst = buffer + prefixLength;
    src = name;
    for (i = 0; i <= nameLength; i++) {
        if (*src == '/' || *src == '\\') {
            *dst = separator;
        } else {
            *dst = *src;
        }
        dst++;
        src++;
    }
    return buffer;
""",
        "join_c": """
    s32 prefixLength = strlen(prefix);
    s32 nameLength = strlen(name);
    const char *src;
    char *dst;
    s32 i;

    if (prefixLength + nameLength > size - 1) {
        return 0;
    }
    RWSRCGLOBAL(vecStrcpy)(buffer, prefix);
    dst = buffer + prefixLength;
    src = name;
    for (i = 0; i <= nameLength; i++) {
        if (*src == '/' || *src == '\\') {
            *dst = separator;
        } else {
            *dst = *src;
        }
        dst++;
        src++;
    }
    return buffer;
""",
    },
    "fn_804799C8": {
        "async_a": """
    if (file != 0 && file->field_0x18 != 0) {
        s32 status = file->asyncStatus;

        if (status != 1 && file->asyncCallBack != 0) {
            void *data;

            if (status == 4) {
                data = (void *)(u32)file->position;
            } else {
                data = file->asyncData;
            }
            file->asyncStatus = 1;
            file->asyncCallBack(file, data, file->status, status, file->asyncCallBackData);
        } else {
            file->asyncStatus = 1;
        }
    }
""",
        "async_b": """
    if (file != 0 && file->field_0x18 != 0) {
        if (file->asyncStatus != 1 && file->asyncCallBack != 0) {
            s32 status = file->asyncStatus;
            void *data;

            if (status == 4) {
                data = (void *)(u32)file->position;
            } else {
                data = file->asyncData;
            }
            file->asyncStatus = 1;
            file->asyncCallBack(file, data, file->status, status, file->asyncCallBackData);
        } else {
            file->asyncStatus = 1;
        }
    }
""",
        "async_c": """
    if (file != 0 && file->field_0x18 != 0) {
        if (file->asyncStatus != 1 && file->asyncCallBack != 0) {
            void *data;
            s32 status;

            if (file->asyncStatus == 4) {
                data = (void *)(u32)file->position;
            } else {
                data = file->asyncData;
            }
            status = file->asyncStatus;
            file->asyncStatus = 1;
            file->asyncCallBack(file, data, file->status, status, file->asyncCallBackData);
        } else {
            file->asyncStatus = 1;
        }
    }
""",
        "async_d": """
    if (file != 0 && file->field_0x18 != 0) {
        s32 status = file->asyncStatus;

        if (status != 1 && file->asyncCallBack != 0) {
            void *data = (status == 4) ? (void *)(u32)file->position : file->asyncData;

            file->asyncStatus = 1;
            file->asyncCallBack(file, data, file->status, status, file->asyncCallBackData);
        } else {
            file->asyncStatus = 1;
        }
    }
""",
    },
    "fn_80479A80": {
        "close_a": """
    if (lbl_805E2A40.isOpen == 1) {
        RtFileSystem *next;
        RtFileSystem *fs = lbl_805E2A40.fsList;

        while (fs != 0) {
            next = fs->next;
            fs->close(fs);
            RWSRCGLOBAL(rwfree)(fs->deviceName);
            fs->deviceName = 0;
            fn_80479894(fs, 2);
            RWSRCGLOBAL(rwfree)(fs);
            fs = next;
        }
        lbl_805E2A40.isOpen = 0;
        lbl_805E2A40.numFS = 0;
    }
""",
        "close_b": """
    if (lbl_805E2A40.isOpen == 1) {
        RtFileSystem *next;
        RtFileSystem *fs;

        for (fs = lbl_805E2A40.fsList; fs != 0; fs = next) {
            next = fs->next;
            fs->close(fs);
            RWSRCGLOBAL(rwfree)(fs->deviceName);
            fs->deviceName = 0;
            fn_80479894(fs, 2);
            RWSRCGLOBAL(rwfree)(fs);
        }
        lbl_805E2A40.isOpen = 0;
        lbl_805E2A40.numFS = 0;
    }
""",
    },
}
