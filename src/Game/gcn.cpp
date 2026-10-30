/* The GameCube platform layer of the RenderWare application skeleton (the file name is the game's:
 * its panic message passes "gcn.c"): main, the heap and RenderWare's memory functions, the video
 * mode, the DVD file system, path names, timers and the platform side of the skeleton's events
 * (Game/RwSkeleton.cpp holds the platform-independent side). Names are inferred. Definitions are
 * in reverse address order (deferred inlining emits functions last-defined first). */
#include <dolphin/types.h>

typedef int OSHeapHandle;
struct GXRenderModeObj;

extern "C" {
void *OSGetArenaLo(void);
void *OSGetArenaHi(void);
void OSSetArenaLo(void *addr);
void *OSInitAlloc(void *arenaStart, void *arenaEnd, int maxHeaps);
OSHeapHandle OSCreateHeap(void *start, void *end);
OSHeapHandle OSSetCurrentHeap(OSHeapHandle heap);
void *OSAllocFromHeap(OSHeapHandle heap, u32 size);
void OSFreeToHeap(OSHeapHandle heap, void *ptr);
BOOL OSDisableInterrupts(void);
BOOL OSRestoreInterrupts(BOOL level);
s32 OSDisableScheduler(void);
s32 OSEnableScheduler(void);
s64 OSGetTime(void);
void OSInit(void);
void DVDInit(void);
u32 VIGetTvFormat(void);
void GXSetMisc(int token, u32 value);
/* OSPanic. */
void fn_80372404(const char *file, int line, const char *msg, ...);
void *memcpy(void *dst, const void *src, u32 n);
void *memset(void *dst, int c, u32 n);

extern volatile OSHeapHandle __OSCurrHeap;

extern GXRenderModeObj lbl_805677C0; /* NTSC */
extern GXRenderModeObj lbl_80567874; /* PAL */
extern GXRenderModeObj lbl_80567838; /* MPAL */

/* The skeleton's event handler (Game/RwSkeleton.cpp). */
int fn_80051850(int event, void *param);

void fn_8037D154(int arg0);
void fn_8037D9DC(void);
void fn_8032BDEC(void);
void fn_8032CCFC(void);
void fn_8032BAF8(void);
void fn_8032BD7C(void);
/* RenderWare: the engine's video mode, the default camera's raster, the file system manager. */
int fn_80485E70(void);
void fn_80485DE4(void *mode, int index);
void *fn_80485808(void *camera, void *rect, int flags);
void *fn_8047A178(void *camera);
void *fn_8047B40C(int maxFiles, void *buffer, u32 bufferSize, const char *deviceName,
                  const char *name);
int fn_80479BAC(void *fileSystem);
void fn_8047A168(void *fileSystem);
/* RenderWare: the OS memory interface, and the character set's size and printing. */
struct UnkRwMemoryInterface {
    void *(*rwmalloc)(u32 size, u32 hint);
    void (*rwfree)(void *p);
};
UnkRwMemoryInterface *fn_80480530(void);
void fn_80478B5C(void *charset, void *desc);
void fn_80478AC8(void *charset, const char *string, int x, int y);
}

/* A raster's size (RwRaster). */
struct UnkRwRaster {
    /* 0x00 */ u8 field_0x0[0xC];
    /* 0x0C */ u32 m_width;
    /* 0x10 */ u32 m_height;
};

/* A camera's frame buffer (RwCamera). */
struct UnkRwCamera {
    /* 0x00 */ u8 field_0x0[0x60];
    /* 0x60 */ UnkRwRaster *m_frameBuffer;
};

/* The engine instance: the current camera and the string functions (RwGlobals). */
struct UnkRwStringFunctions {
    /* 0x00 */ UnkRwCamera *m_curCamera;
    /* 0x04 */ u8 field_0x4[0xCC - 0x4];
    /* 0xCC */ char *(*rwstrcpy)(char *dst, const char *src);
    /* 0xD0 */ u8 field_0xD0[0xE0 - 0xD0];
    /* 0xE0 */ char *(*rwstrchr)(const char *string, int c);
    /* 0xE4 */ u8 field_0xE4[0xF4 - 0xE4];
    /* 0xF4 */ u32 (*rwstrlen)(const char *string);
};
extern UnkRwStringFunctions *lbl_805F15B4;

/* RenderWare's memory functions (RwMemoryFunctions). */
struct UnkRwMemoryFunctions {
    void *(*rwmalloc)(u32 size, u32 hint);
    void (*rwfree)(void *p);
    void *(*rwrealloc)(void *p, u32 size, u32 hint);
    void *(*rwcalloc)(u32 count, u32 size, u32 hint);
};

/* The platform's globals (struct name unknown). */
struct UnkPlatformGlobals_80573CC0 {
    /* 0x00 */ GXRenderModeObj *m_renderMode;
    /* 0x04 */ u32 field_0x4;
    /* 0x08 */ u32 m_size;
    /* 0x0C */ u8 field_0xC[0x20 - 0xC];
};

/* The skeleton's globals (Game/RwSkeleton.cpp). */
struct UnkSkeletonGlobals_80575CF0 {
    /* 0x00 */ const char *m_appName;
    /* 0x04 */ int m_maximumWidth;
    /* 0x08 */ int m_maximumHeight;
    /* 0x0C */ int m_quit;
    /* 0x10 */ u8 field_0x10[0x38 - 0x10];
};
extern UnkSkeletonGlobals_80575CF0 lbl_80575CF0;

/* A video mode of the engine (RwVideoMode). */
struct UnkRwVideoMode {
    /* 0x0 */ int m_width;
    /* 0x4 */ int m_height;
    /* 0x8 */ int m_depth;
    /* 0xC */ int m_flags;
};

/* A screen rectangle (RwRect). */
struct UnkRwRect {
    /* 0x0 */ int x;
    /* 0x4 */ int y;
    /* 0x8 */ int w;
    /* 0xC */ int h;
};

UnkPlatformGlobals_80573CC0 lbl_80573CC0;
/* The file system's buffers: four of 0x800 bytes (DVD reads need 32-byte alignment). */
u8 lbl_80573CE0[0x2000] __attribute__((aligned(32)));
static OSHeapHandle s_heap;

/* The size of a character set's characters (RtCharsetDesc). */
struct UnkCharsetDesc {
    /* 0x0 */ int m_width;
    /* 0x4 */ int m_height;
    /* 0x8 */ int field_0x8[6];
};

/* Prints string with the character set at x, y (in characters), placed as pos says: 1 left,
 * 2 right, 4 top, 8 bottom, centred along each axis no flag names. */
extern "C" void fn_80051400(void *charset, const char *string, int x, int y, int pos) {
    UnkRwRaster *raster = lbl_805F15B4->m_curCamera->m_frameBuffer;
    u32 width = raster->m_width;
    u32 height = raster->m_height;
    UnkCharsetDesc desc;
    fn_80478B5C(charset, &desc);
    int length = lbl_805F15B4->rwstrlen(string);
    x *= desc.m_width;
    y *= desc.m_height;
    if (pos & 1) {
        x += desc.m_width * 5;
    } else if (pos & 2) {
        x += width - desc.m_width * (length + 5);
    }
    if (pos == 1 || pos == 2 || pos == 0) {
        y += (height - desc.m_height) / 2;
    }
    if (pos & 4) {
        y += desc.m_height * 2;
    } else if (pos & 8) {
        y += height - desc.m_height * 5;
    }
    if (pos == 4 || pos == 8 || pos == 0) {
        x += (width - desc.m_width * length) / 2;
    }
    fn_80478AC8(charset, string, x, y);
}

extern "C" void *fn_800513E0(void *camera) {
    return fn_8047A178(camera);
}

extern "C" void fn_800513DC(const char *message) {}

extern "C" void *fn_800513B4(void *camera) {
    return fn_80485808(camera, 0, 0);
}

/* The time in milliseconds. */
extern "C" u32 fn_80051374(void) {
    return OSGetTime() / ((*(u32 *)0x800000F8 / 4) / 1000);
}

/* The time in microseconds. */
extern "C" u64 fn_80051328(void) {
    return (OSGetTime() * 8) / ((*(u32 *)0x800000F8 / 4) / 125000);
}

/* A copy of a path name with '/' separators, on the heap. */
extern "C" char *fn_80051218(const char *path) {
    char *copy;
    u32 size = lbl_805F15B4->rwstrlen(path) + 1;
    copy = (char *)fn_80480530()->rwmalloc(size, 0);
    if (copy != 0) {
        if (*path == '.') {
            path++;
        }
        if (*path == '/') {
            path++;
        }
        if (*path == '\\') {
            path++;
        }
        if (*path == ':') {
            path++;
        }
        lbl_805F15B4->rwstrcpy(copy, path);
        char *c;
        while ((c = lbl_805F15B4->rwstrchr(copy, '\\')) != 0) {
            *c = '/';
        }
        while ((c = lbl_805F15B4->rwstrchr(copy, ':')) != 0) {
            *c = '/';
        }
    }
    return copy;
}

extern "C" void fn_800511D8(char *path) {
    if (path != 0) {
        fn_80480530()->rwfree(path);
    }
}

/* Installs the DVD file system. */
extern "C" int fn_8005114C(void) {
    const char *device = "dvd:";
    const char *name = "dvd";
    void *fileSystem = fn_8047B40C(4, lbl_80573CE0, 0x800, device, name);
    if (fileSystem != 0) {
        fn_8032CCFC();
        *(void (**)(void))((u8 *)fileSystem + 0x5C) = fn_8032BD7C;
        if (!fn_80479BAC(fileSystem)) {
            return 0;
        }
        fn_8047A168(fileSystem);
    } else {
        return 0;
    }
    fn_8032BAF8();
    return 1;
}

extern "C" int fn_80051144(void) {
    return 1;
}

int main(int argc, char **argv) {
    fn_8037D154(1);
    lbl_80573CC0.field_0x4 = 0;
    lbl_80573CC0.m_size = 0x40000;
    if (!fn_80051850(16, 0)) {
        return 0;
    }
    for (int i = 1; i < argc; i++) {
        fn_80051850(28, argv[i]);
    }
    if (!fn_80051850(13, &lbl_80573CC0)) {
        fn_80051850(17, 0);
        return 0;
    }
    for (int i = 1; i < argc; i++) {
        fn_80051850(1, argv[i]);
    }
    UnkRwVideoMode mode;
    fn_80485DE4(&mode, fn_80485E70());
    lbl_80575CF0.m_maximumWidth = mode.m_width;
    lbl_80575CF0.m_maximumHeight = mode.m_height;
    UnkRwRect r;
    r.x = 0;
    r.y = 0;
    r.w = lbl_80575CF0.m_maximumWidth;
    r.h = lbl_80575CF0.m_maximumHeight;
    fn_80051850(0, &r);
    while (!lbl_80575CF0.m_quit) {
        fn_80051850(18, 0);
        fn_8032BDEC();
    }
    fn_80051850(14, 0);
    fn_80051850(17, 0);
    return 0;
}

/* Starts the OS and picks the render mode for the TV format. */
extern "C" int fn_80050F3C(void) {
    GXSetMisc(1, 8);
    OSInit();
    DVDInit();
    fn_8037D9DC();
    switch (VIGetTvFormat()) {
    case 0:
        lbl_80573CC0.m_renderMode = &lbl_805677C0;
        break;
    case 1:
        lbl_80573CC0.m_renderMode = &lbl_80567874;
        break;
    case 2:
        lbl_80573CC0.m_renderMode = &lbl_80567838;
        break;
    }
    return 1;
}

extern "C" void fn_80050F08(void) {
    fn_80372404("gcn.c", 792, "The End.");
}

extern "C" int fn_80050F00(void) {
    return 1;
}

/* Sets up the heap and gives RenderWare its memory functions. */
extern "C" int fn_80050BA0(UnkRwMemoryFunctions *functions);

/* The platform's events. */
extern "C" int fn_80050EBC(int event, void *param) {
    switch (event) {
    case 30:
        if (!fn_80050BA0((UnkRwMemoryFunctions *)param)) {
            return 0;
        }
        break;
    }
    return 2;
}

/* The heap with interrupts and the scheduler held off. */
inline void HeapFree(void *p) {
    if (p != 0) {
        bool enabled = OSDisableInterrupts();
        OSDisableScheduler();
        OSFreeToHeap(__OSCurrHeap, p);
        OSEnableScheduler();
        OSRestoreInterrupts(enabled);
    }
}

inline void *HeapAlloc(u32 size) {
    if (size == 0) {
        return 0;
    }
    bool enabled = OSDisableInterrupts();
    OSDisableScheduler();
    void *p = OSAllocFromHeap(__OSCurrHeap, size);
    OSEnableScheduler();
    OSRestoreInterrupts(enabled);
    return p;
}

/* RenderWare's memory functions. */
static void Free(void *p) {
    HeapFree(p);
}

static void *Malloc(u32 size, u32 hint) {
    if (size == 0) {
        return 0;
    }
    bool enabled = OSDisableInterrupts();
    OSDisableScheduler();
    void *p = OSAllocFromHeap(__OSCurrHeap, size);
    OSEnableScheduler();
    OSRestoreInterrupts(enabled);
    return p;
}

static void *Calloc(u32 count, u32 size, u32 hint) {
    u32 total = count * size;
    void *p = HeapAlloc(total);
    if (p != 0) {
        memset(p, 0, total);
    }
    return p;
}

static void *Realloc(void *old, u32 size, u32 hint) {
    if (size == 0) {
        HeapFree(old);
        return 0;
    }
    u32 oldSize = old != 0 ? *(u32 *)((u8 *)old - 0x18) - 0x20 : 0;
    void *p = HeapAlloc(size);
    if (p != 0 && oldSize != 0) {
        if (oldSize >= size) {
            memcpy(p, old, size);
        } else {
            memcpy(p, old, oldSize);
        }
        HeapFree(old);
    }
    return p;
}

/* Makes the arena one heap, the current one. */
inline void InitHeap() {
    void *lo = OSGetArenaLo();
    void *hi = OSGetArenaHi();
    lo = OSInitAlloc(lo, hi, 1);
    OSSetArenaLo(lo);
    s_heap = OSCreateHeap((void *)(((u32)lo + 0x1F) & ~0x1F), (void *)((u32)hi & ~0x1F));
    OSSetCurrentHeap(s_heap);
    OSSetArenaLo(hi);
}

extern "C" int fn_80050BA0(UnkRwMemoryFunctions *functions) {
    InitHeap();
    functions->rwmalloc = Malloc;
    functions->rwcalloc = Calloc;
    functions->rwrealloc = Realloc;
    functions->rwfree = Free;
    return 1;
}
