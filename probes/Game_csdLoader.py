"""Source shapes tried for the open functions of Game/csdLoader.cpp (wave 39)."""

SYMBOLS = {
    "SwapEndian__Q24Chao3CSDFPvl": "void SwapEndian(void *p, s32 size) {",
}

BODIES = {
    "SwapEndian__Q24Chao3CSDFPvl": {
        "named_locals": """    switch (size) {
    case 2: {
        u16 v = *(u16 *)p;
        __sthbrx(v, p, 0);
        break;
    }
    case 3:
        break;
    case 4: {
        u32 v = *(u32 *)p;
        __stwbrx(v, p, 0);
        break;
    }
    }
""",
        "one_local": """    u32 v;
    switch (size) {
    case 2:
        v = *(u16 *)p;
        __sthbrx(v, p, 0);
        break;
    case 3:
        break;
    case 4:
        v = *(u32 *)p;
        __stwbrx(v, p, 0);
        break;
    }
""",
        "typed_ptrs": """    switch (size) {
    case 2: {
        u16 *q = (u16 *)p;
        __sthbrx(*q, q, 0);
        break;
    }
    case 3:
        break;
    case 4: {
        u32 *q = (u32 *)p;
        __stwbrx(*q, q, 0);
        break;
    }
    }
""",
        "size_reuse": """    switch (size) {
    case 2:
        size = *(u16 *)p;
        __sthbrx(size, p, 0);
        break;
    case 3:
        break;
    case 4:
        size = *(u32 *)p;
        __stwbrx(size, p, 0);
        break;
    }
""",
    },
}

XTRA = {
    "SwapEndian__Q24Chao3CSDFPvl": """inline void Swap16(u16 *p) {
    u16 v = *p;
    __sthbrx(v, p, 0);
}
inline void Swap32(u32 *p) {
    u32 v = *p;
    __stwbrx(v, p, 0);
}
inline void Swap16v(void *p, u16 v) {
    __sthbrx(v, p, 0);
}
inline void Swap32v(void *p, u32 v) {
    __stwbrx(v, p, 0);
}
inline u16 Rev16(u16 v) {
    u16 r;
    __sthbrx(v, &r, 0);
    return r;
}
""",
}
BODIES["SwapEndian__Q24Chao3CSDFPvl"].update({
    "inline_ptr": """    switch (size) {
    case 2:
        Swap16((u16 *)p);
        break;
    case 3:
        break;
    case 4:
        Swap32((u32 *)p);
        break;
    }
""",
    "inline_val": """    switch (size) {
    case 2:
        Swap16v(p, *(u16 *)p);
        break;
    case 3:
        break;
    case 4:
        Swap32v(p, *(u32 *)p);
        break;
    }
""",
    "if_chain": """    if (size == 2) {
        __sthbrx(*(u16 *)p, p, 0);
    } else if (size == 4) {
        __stwbrx(*(u32 *)p, p, 0);
    }
""",
    "idx": """    switch (size) {
    case 2:
        __sthbrx(*(u16 *)p, 0, (int)p);
        break;
    case 3:
        break;
    case 4:
        __stwbrx(*(u32 *)p, 0, (int)p);
        break;
    }
""",
})
