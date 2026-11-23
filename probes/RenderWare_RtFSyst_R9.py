# Round 9 for src/RenderWare/RtFSyst.c: fgets with the file as a void * (as fread).
OLD = "char *fn_8047A518(char *buffer, s32 maxLength, RtFile *file) {\n    s32 i = 0;\n    s32 count = file->fs->fileRead(file, buffer, maxLength - 1);\n\n    if (count == 0) {\n        return 0;\n    }\n    while (count > i) {\n        char *p = buffer + i;\n\n        if (*p == '\\n') {\n            buffer[++i] = 0;\n            fn_8047A470(file, i - count, 1);\n            return buffer;\n        }\n        if (*p == '\\r' && i < count - 1 && p[1] == '\\n') {\n            memcpy(p, p + 1, count - i - 1);\n            count--;\n        } else {\n            i++;\n        }\n    }\n    buffer[count] = 0;\n    return buffer;\n}"
BODY = OLD[OLD.index('{') + 1:]
B2 = BODY.replace("    s32 i = 0;\n    s32 count = file->fs->fileRead(file, buffer, maxLength - 1);\n",
                  "    RtFile *file = (RtFile *)fptr;\n    s32 i = 0;\n    s32 count = file->fs->fileRead(file, buffer, maxLength - 1);\n")
B3 = BODY.replace("    s32 i = 0;\n    s32 count = file->fs->fileRead(file, buffer, maxLength - 1);\n",
                  "    RtFile *file = (RtFile *)fptr;\n    s32 count;\n    s32 i = 0;\n\n    count = file->fs->fileRead(file, buffer, maxLength - 1);\n")
B4 = BODY.replace("    s32 i = 0;\n    s32 count = file->fs->fileRead(file, buffer, maxLength - 1);\n",
                  "    RtFile *file = (RtFile *)fptr;\n    s32 i;\n    s32 count;\n\n    i = 0;\n    count = file->fs->fileRead(file, buffer, maxLength - 1);\n")
SIG = "char *fn_8047A518(char *buffer, s32 maxLength, void *fptr) {"
REPL = {"vp2": [(OLD, SIG + B2)], "vp3": [(OLD, SIG + B3)], "vp4": [(OLD, SIG + B4)]}
SCORE = ["fn_8047A518"]
