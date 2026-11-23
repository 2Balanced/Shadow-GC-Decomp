# Round 6 for src/RenderWare/RtFSyst.c: the path join's loop body.
SYMBOLS = {
    "fn_8047971C": "char *fn_8047971C(char *buffer, s32 size, const char *prefix, const char *name, char separator) {",
}
BS = chr(92)
HEAD = ("\n    s32 prefixLength = strlen(prefix);\n    s32 nameLength = strlen(name);\n    %s\n\n"
        "    if (prefixLength + nameLength > size - 1) {\n        return 0;\n    }\n"
        "    RWSRCGLOBAL(vecStrcpy)(buffer, prefix);\n")
loops = {
 "c_local": "    src = name;\n    dst = buffer + prefixLength;\n    for (i = 0; i <= nameLength; i++) {\n        char c = *src;\n\n        if (c == '/' || c == '" + BS + BS + "') {\n            *dst = separator;\n        } else {\n            *dst = c;\n        }\n        dst++;\n        src++;\n    }\n    return buffer;\n",
 "c_local2": "    src = name;\n    dst = buffer + prefixLength;\n    for (i = 0; i <= nameLength; i++) {\n        c = *src;\n        if (c == '/' || c == '" + BS + BS + "') {\n            *dst = separator;\n        } else {\n            *dst = c;\n        }\n        dst++;\n        src++;\n    }\n    return buffer;\n",
 "postinc": "    src = name;\n    dst = buffer + prefixLength;\n    for (i = 0; i <= nameLength; i++, dst++, src++) {\n        if (*src == '/' || *src == '" + BS + BS + "') {\n            *dst = separator;\n        } else {\n            *dst = *src;\n        }\n    }\n    return buffer;\n",
 "ternary": "    src = name;\n    dst = buffer + prefixLength;\n    for (i = 0; i <= nameLength; i++) {\n        *dst = (*src == '/' || *src == '" + BS + BS + "') ? separator : *src;\n        dst++;\n        src++;\n    }\n    return buffer;\n",
}
decls = {
 "A": "char *dst;\n    const char *src;\n    s32 i;",
 "B": "const char *src;\n    char *dst;\n    s32 i;",
 "C": "s32 i;\n    char *dst;\n    const char *src;",
 "D": "s32 i;\n    const char *src;\n    char *dst;",
}
BODIES = {"fn_8047971C": {}}
for ln, lb in loops.items():
    for dn, d in decls.items():
        dd = d + ("\n    char c;" if ln == "c_local2" else "")
        BODIES["fn_8047971C"][ln + "_" + dn] = HEAD % dd + lb
