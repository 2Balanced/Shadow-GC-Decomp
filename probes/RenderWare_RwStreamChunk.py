# Probe table for src/RenderWare/RwStreamChunk.c: the 32-bit byte swap loop.
SYMBOLS = {"fn_8047D158": "void *fn_8047D158(void *mem, u32 size) {"}
BODIES = {"fn_8047D158": {
 "lwbrx": "\n    u32 *p = (u32 *)mem;\n    u32 n;\n\n    for (n = size >> 2; n != 0; n--) {\n        *p = __lwbrx(p, 0);\n        p++;\n    }\n    return mem;\n",
 "val": "\n    u32 *p = (u32 *)mem;\n    u32 n;\n\n    for (n = size >> 2; n != 0; n--) {\n        u32 v = *p;\n\n        __stwbrx(v, p, 0);\n        p++;\n    }\n    return mem;\n",
 "idx": "\n    u32 *p = (u32 *)mem;\n    u32 n = size >> 2;\n    u32 i;\n\n    for (i = 0; i < n; i++) {\n        __stwbrx(p[i], p + i, 0);\n    }\n    return mem;\n",
 "idxoff": "\n    u32 *p = (u32 *)mem;\n    u32 n = size >> 2;\n    u32 i;\n\n    for (i = 0; i < n; i++) {\n        __stwbrx(p[i], p, i * 4);\n    }\n    return mem;\n",
 "whilen": "\n    u32 *p = (u32 *)mem;\n    u32 n = size >> 2;\n\n    while (n--) {\n        __stwbrx(*p, p, 0);\n        p++;\n    }\n    return mem;\n",
 "postinc": "\n    u32 *p = (u32 *)mem;\n    u32 n = size >> 2;\n\n    while (n--) {\n        u32 v = *p;\n        __stwbrx(v, p++, 0);\n    }\n    return mem;\n",
 "shift": "\n    u32 *p = (u32 *)mem;\n    u32 n;\n\n    for (n = size >> 2; n != 0; n--) {\n        u32 v = *p;\n\n        *p = (v >> 24) | ((v >> 8) & 0xFF00) | ((v & 0xFF00) << 8) | (v << 24);\n        p++;\n    }\n    return mem;\n",
 "s32n": "\n    u32 *p = (u32 *)mem;\n    s32 n;\n\n    for (n = size >> 2; n > 0; n--) {\n        __stwbrx(*p, p, 0);\n        p++;\n    }\n    return mem;\n",
 "bytes": "\n    u8 *p = (u8 *)mem;\n    u32 n;\n\n    for (n = size >> 2; n != 0; n--) {\n        __stwbrx(*(u32 *)p, p, 0);\n        p += 4;\n    }\n    return mem;\n",
}}
