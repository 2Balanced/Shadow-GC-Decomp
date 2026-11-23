# Round 5 for src/RenderWare/RtFSyst.c: the path join's declaration and assignment orders.
import itertools
SYMBOLS = {
    "fn_8047971C": "char *fn_8047971C(char *buffer, s32 size, const char *prefix, const char *name, char separator) {",
}
BS = chr(92)
decls = ["char *dst;", "const char *src;", "s32 i;"]
assigns = ["src = name;", "dst = buffer + prefixLength;"]
BODIES = {"fn_8047971C": {}}
for di, d in enumerate(itertools.permutations(decls)):
    for ai, a in enumerate(itertools.permutations(assigns)):
        body = ("\n    s32 prefixLength = strlen(prefix);\n    s32 nameLength = strlen(name);\n    "
                + "\n    ".join(d) + "\n\n    if (prefixLength + nameLength > size - 1) {\n        return 0;\n    }\n"
                "    RWSRCGLOBAL(vecStrcpy)(buffer, prefix);\n    " + "\n    ".join(a) +
                "\n    for (i = 0; i <= nameLength; i++) {\n        if (*src == '/' || *src == '" + BS + BS + "') {\n"
                "            *dst = separator;\n        } else {\n            *dst = *src;\n        }\n"
                "        dst++;\n        src++;\n    }\n    return buffer;\n")
        BODIES["fn_8047971C"]["join_d%d_a%d" % (di, ai)] = body
