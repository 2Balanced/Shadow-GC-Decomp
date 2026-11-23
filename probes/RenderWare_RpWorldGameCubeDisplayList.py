"""Wave 41: fn_8046D8C8's first six instructions: retail stores the two zeroes through r9 before
the counter's li r9,0."""
SCORE = ["fn_8046D8C8"]
OLD = "    *numStrips = 0;\n    *numVertices = 0;\n    run = 0;\n    for (i = 0; i < numIndices - 2; i++) {\n"
DECL = "    u32 run;\n    u32 i;\n"
REPL = {
    "base": [],
    "s32_i": [(DECL, "    u32 run;\n    s32 i;\n")],
    "all_s32": [(DECL, "    s32 run;\n    s32 i;\n")],
    "while": [(OLD, "    *numStrips = 0;\n    *numVertices = 0;\n    run = 0;\n    i = 0;\n    while (i < numIndices - 2) {\n"), ("        indices++;\n    }\n    if (run != 0) {\n        CLOSE_STRIP();\n    }\n}", "        indices++;\n        i++;\n    }\n    if (run != 0) {\n        CLOSE_STRIP();\n    }\n}")],
    "count_var": [(DECL, "    u32 run;\n    u32 i;\n    u32 count;\n"), (OLD, "    *numStrips = 0;\n    *numVertices = 0;\n    run = 0;\n    count = numIndices - 2;\n    for (i = 0; i < count; i++) {\n")],
    "zero_var": [(DECL, "    u32 run;\n    u32 i;\n    s32 zero = 0;\n"), (OLD, "    *numStrips = zero;\n    *numVertices = zero;\n    run = 0;\n    for (i = 0; i < numIndices - 2; i++) {\n")],
}
