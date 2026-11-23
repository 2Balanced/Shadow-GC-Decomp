# Round 7 for src/RenderWare/RtFSyst.c: the path join written with indexes.
SYMBOLS = {
    "fn_8047971C": "char *fn_8047971C(char *buffer, s32 size, const char *prefix, const char *name, char separator) {",
}
BS = chr(92)
HEAD = ("\n    s32 prefixLength = strlen(prefix);\n    s32 nameLength = strlen(name);\n    %s\n\n"
        "    if (prefixLength + nameLength > size - 1) {\n        return 0;\n    }\n"
        "    RWSRCGLOBAL(vecStrcpy)(buffer, prefix);\n")
BODIES = {"fn_8047971C": {
 "idx_a": HEAD % "s32 i;" + "    for (i = 0; i <= nameLength; i++) {\n        if (name[i] == '/' || name[i] == '" + BS + BS + "') {\n            buffer[prefixLength + i] = separator;\n        } else {\n            buffer[prefixLength + i] = name[i];\n        }\n    }\n    return buffer;\n",
 "idx_b": HEAD % "s32 i;\n    char *dst;" + "    dst = buffer + prefixLength;\n    for (i = 0; i <= nameLength; i++) {\n        if (name[i] == '/' || name[i] == '" + BS + BS + "') {\n            dst[i] = separator;\n        } else {\n            dst[i] = name[i];\n        }\n    }\n    return buffer;\n",
 "idx_c": HEAD % "char *dst;\n    s32 i;" + "    dst = &buffer[prefixLength];\n    for (i = 0; i <= nameLength; i++) {\n        if (name[i] == '/' || name[i] == '" + BS + BS + "') {\n            dst[i] = separator;\n        } else {\n            dst[i] = name[i];\n        }\n    }\n    return buffer;\n",
 "ptr_sw": HEAD % "char *dst;\n    const char *src;\n    s32 i;" + "    src = name;\n    dst = buffer + prefixLength;\n    for (i = 0; i <= nameLength; i++) {\n        switch (*src) {\n        case '/':\n        case '" + BS + BS + "':\n            *dst = separator;\n            break;\n        default:\n            *dst = *src;\n            break;\n        }\n        dst++;\n        src++;\n    }\n    return buffer;\n",
 "ptr_neq": HEAD % "char *dst;\n    const char *src;\n    s32 i;" + "    src = name;\n    dst = buffer + prefixLength;\n    for (i = 0; i <= nameLength; i++) {\n        if (*src != '/' && *src != '" + BS + BS + "') {\n            *dst = *src;\n        } else {\n            *dst = separator;\n        }\n        dst++;\n        src++;\n    }\n    return buffer;\n",
}}
