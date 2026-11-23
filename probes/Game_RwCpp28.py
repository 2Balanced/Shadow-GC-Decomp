"""fn_80465ED4 of Game/RwCpp28.cpp (wave 40, agent r3): retail keeps the header size in r31 and
the result in r30; declaration order of the function's locals."""
H = """    u32 bytes = (streams & 0x1FFFFFFF) * 8;
    for (u32 i = 0; i < streams; i++) {
        bytes += descriptors[i * 2 + 1] * rwElementSize(*(u32 *)(inputs + i * 0x18 + 4));
    }
    u32 *result = rwAlloc(bytes);
    if (result == NULL) return NULL;
    u32 headerBytes = (streams & 0x1FFFFFFF) * 8;
"""
V1 = """    u32 headerBytes;
    u32 *result;
    u32 bytes = (streams & 0x1FFFFFFF) * 8;
    for (u32 i = 0; i < streams; i++) {
        bytes += descriptors[i * 2 + 1] * rwElementSize(*(u32 *)(inputs + i * 0x18 + 4));
    }
    result = rwAlloc(bytes);
    if (result == NULL) return NULL;
    headerBytes = (streams & 0x1FFFFFFF) * 8;
"""
V2 = """    u32 headerBytes;
    u32 *result;
    u32 bytes;
    bytes = (streams & 0x1FFFFFFF) * 8;
    for (u32 i = 0; i < streams; i++) {
        bytes += descriptors[i * 2 + 1] * rwElementSize(*(u32 *)(inputs + i * 0x18 + 4));
    }
    result = rwAlloc(bytes);
    if (result == NULL) return NULL;
    headerBytes = (streams & 0x1FFFFFFF) * 8;
"""
V3 = """    u32 headerBytes = (streams & 0x1FFFFFFF) * 8;
    u32 *result;
    u32 bytes = headerBytes;
    for (u32 i = 0; i < streams; i++) {
        bytes += descriptors[i * 2 + 1] * rwElementSize(*(u32 *)(inputs + i * 0x18 + 4));
    }
    result = rwAlloc(bytes);
    if (result == NULL) return NULL;
"""
V4 = """    u32 *result;
    u32 headerBytes;
    u32 bytes = (streams & 0x1FFFFFFF) * 8;
    for (u32 i = 0; i < streams; i++) {
        bytes += descriptors[i * 2 + 1] * rwElementSize(*(u32 *)(inputs + i * 0x18 + 4));
    }
    result = rwAlloc(bytes);
    if (result == NULL) return NULL;
    headerBytes = (streams & 0x1FFFFFFF) * 8;
"""
REPL = {"v1": [(H, V1)], "v2": [(H, V2)], "v3": [(H, V3)], "v4": [(H, V4)]}
SCORE = ["fn_80465ED4"]
