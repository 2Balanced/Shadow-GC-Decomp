/* The platform-independent side of the RenderWare application skeleton: its globals, start-up,
 * RenderWare's start and end, its event handler (the platform's handler, Game/gcn.cpp, sees each
 * event first, then the application's, Game/DebugListener.cpp), and wrappers around the platform's
 * path names, timer and error message. The game added the pads, a function-local static, which
 * RenderWare's start initializes. The file name and all names are inferred. Definitions are in
 * reverse address order (deferred inlining emits functions last-defined first).
 *
 * NonMatching (10/11; every section's data matches): fn_80051850 (99.4%) returns the result of
 * starting RenderWare as a comparison that retail moves into r3 (mr r3, r0) and GC/2.7 masks
 * (clrlwi r3, r0, 24) as a bool; about 25 source shapes (bool, int, ?:, casts, variables, the
 * unit compiled as C) were tried. It is the game's only such site. */
#include <string.h>
#include <dolphin/types.h>
#include "size_assert.h"

/* The pad state (class name unknown; Game/SaveCycleMessage.cpp and others call fn_80051734). */
class UnkPads_80051734 {
  public:
    UnkPads_80051734();
    ~UnkPads_80051734();
    void Init();

    /* 0x000 */ u8 field_0x0[0x124];
};
SIZE_ASSERT(UnkPads_80051734, 0x124);

/* RenderWare's memory functions (RwMemoryFunctions). */
struct UnkRwMemoryFunctions {
    void *(*rwmalloc)(u32 size, u32 hint);
    void (*rwfree)(void *p);
    void *(*rwrealloc)(void *p, u32 size, u32 hint);
    void *(*rwcalloc)(u32 count, u32 size, u32 hint);
};

/* The skeleton's globals (struct name unknown; see Game/DebugListener.cpp). */
struct UnkSkeletonGlobals_80575CF0 {
    /* 0x00 */ const char *m_appName;
    /* 0x04 */ int m_maximumWidth;
    /* 0x08 */ int m_maximumHeight;
    /* 0x0C */ int m_quit;
    /* 0x10 */ u32 field_0x10;
    /* 0x14 */ u32 field_0x14;
    /* 0x18 */ int m_hasHandler;
    /* 0x1C */ int (*m_handler)(int event, void *param);
    /* 0x20 */ int field_0x20;
    /* 0x24 */ u32 field_0x24;
    /* 0x28 */ u32 field_0x28;
    /* 0x2C */ int field_0x2C;
    /* 0x30 */ u32 field_0x30;
    /* 0x34 */ u32 field_0x34;
};
SIZE_ASSERT(UnkSkeletonGlobals_80575CF0, 0x38);

extern "C" {
/* The platform (Game/gcn.cpp): its event handler, start-up, end, device selection, file system,
 * texture support, path names, raster display, error message and timer. */
int fn_80050EBC(int event, void *param);
int fn_80050F3C(void);
void fn_80050F08(void);
int fn_80050F00(int useDefault);
int fn_8005114C(void);
int fn_80051144(void);
char *fn_80051218(const char *path);
void fn_800511D8(char *path);
void *fn_800513B4(void *camera);
void fn_800513DC(const char *message);
u64 fn_80051328(void);
/* The application's event handler (Game/DebugListener.cpp). */
int fn_80048E90(int event, void *param);
/* RenderWare: the engine's init, open, start, stop, close and term. */
int fn_804864F8(UnkRwMemoryFunctions *functions, int flags, u32 arenaSize);
int fn_80486204(void *params);
int fn_80485FE0(void);
void fn_80485EF4(void);
void fn_80486138(void);
void fn_804864AC(void);
/* RenderWare: image path, character sets, texture dictionaries (behaviour inferred). */
int fn_80479A50(int arg0);
void fn_80479A80(void);
int fn_80478F74(void);
void fn_80478F00(void);
void *fn_8048CB38(void *dictionary);
void fn_8048CC54(void *(*callBack)(void *dictionary, void *data), void *data);
void fn_803800AC(void);
void fn_80332E7C(void *params);

int fn_80051850(int event, void *param);
int fn_80051598(void *params);
}

static UnkRwMemoryFunctions s_memoryFunctions;
UnkSkeletonGlobals_80575CF0 lbl_80575CF0;
/* Whether the platform's default video mode is used ("-vms" turns it off). */
static int s_defaultVideoMode = 1;

/* The time in microseconds. */
extern "C" u64 fn_80051A94(void) {
    return fn_80051328();
}

/* Shows message as an error. */
extern "C" void fn_80051A10(const char *message) {
    fn_800513DC(message);
    struct {
        int a;
        int b;
    } param;
    param.a = 0xFF;
    param.b = 0xFF;
    if (lbl_80575CF0.m_hasHandler) {
        lbl_80575CF0.m_handler(0x14, &param);
    }
    if (lbl_80575CF0.m_hasHandler) {
        lbl_80575CF0.m_handler(0x13, &param);
    }
}

extern "C" void *fn_800519F0(void *camera) {
    return fn_800513B4(camera);
}

/* Sets the globals and starts the platform. */
inline int Initialize() {
    lbl_80575CF0.m_appName = "RenderWare Application";
    lbl_80575CF0.m_maximumWidth = 0;
    lbl_80575CF0.m_maximumHeight = 0;
    lbl_80575CF0.m_quit = 0;
    lbl_80575CF0.field_0x14 = 0;
    lbl_80575CF0.m_handler = 0;
    lbl_80575CF0.m_hasHandler = 0;
    lbl_80575CF0.field_0x20 = 1;
    lbl_80575CF0.field_0x28 = 0;
    lbl_80575CF0.field_0x24 = 0;
    lbl_80575CF0.field_0x2C = 2;
    lbl_80575CF0.field_0x34 = 0;
    lbl_80575CF0.field_0x30 = 0;
    return fn_80050F3C();
}

static void *DestroyTexDictionary(void *dictionary, void *data);

/* Ends RenderWare. */
inline void Terminate() {
    fn_80478F00();
    fn_8048CC54(DestroyTexDictionary, 0);
    fn_80479A80();
    fn_80485EF4();
    fn_80486138();
    fn_804864AC();
}

/* The skeleton's events: the platform's handler first, then the application's, then the
 * skeleton's own. */
extern "C" int fn_80051850(int event, void *param) {
    int result = fn_80050EBC(event, param);
    if (result != 2) {
        return result;
    }
    result = fn_80048E90(event, param);
    if (event == 21) {
        lbl_80575CF0.m_quit = 1;
    }
    if (result == 2) {
        switch (event) {
        case 15:
            return fn_80050F00(s_defaultVideoMode) ? 1 : 0;
        case 1:
            fn_80051850(2, param);
            return 1;
        case 28: {
            int processed;
            if (strcmp((const char *)param, "-vms") == 0) {
                s_defaultVideoMode = 0;
                processed = 1;
            } else {
                processed = 0;
            }
            return processed ? 1 : 0;
        }
        case 3:
            return 1;
        case 10:
            return 1;
        case 14:
            Terminate();
            return 1;
        case 13:
            /* Retail moves the comparison into r3 (mr) where GC/2.7 masks it (clrlwi). */
            return fn_80051598(param) != 0;
        case 17:
            fn_80050F08();
            return 1;
        case 16:
            return Initialize() ? 1 : 0;
        }
    }
    return result;
}

extern "C" void fn_80051830(char *path) {
    fn_800511D8(path);
}

extern "C" char *fn_80051810(const char *path) {
    return fn_80051218(path);
}

static void *DestroyTexDictionary(void *dictionary, void *data) {
    fn_8048CB38(dictionary);
    return dictionary;
}

extern "C" void fn_800517A0(void) {
    Terminate();
}

/* The pads. */
extern "C" UnkPads_80051734 *fn_80051734(void) {
    static UnkPads_80051734 *s_pads;
    if (s_pads == 0) {
        static UnkPads_80051734 s_object;
        s_pads = &s_object;
    }
    return s_pads;
}

/* Starts RenderWare. */
extern "C" int fn_80051598(void *params) {
    if (!fn_80051850(30, &s_memoryFunctions)) {
        return 0;
    }
    if (!fn_804864F8(&s_memoryFunctions, 1, 0x40000)) {
        return 0;
    }
    fn_80051850(3, 0);
    if (!fn_80051850(9, 0)) {
        return 0;
    }
    if (!fn_80051850(4, 0)) {
        return 0;
    }
    void *openParams = params;
    if (!fn_80486204(&openParams)) {
        fn_804864AC();
        return 0;
    }
    if (!fn_80051850(15, params)) {
        fn_80486138();
        fn_804864AC();
        return 0;
    }
    if (!fn_80485FE0()) {
        fn_80486138();
        fn_804864AC();
        return 0;
    }
    if (fn_80479A50(-1)) {
        if (!fn_8005114C()) {
            fn_80485EF4();
            fn_80486138();
            fn_804864AC();
            return 0;
        }
    } else {
        fn_80485EF4();
        fn_80486138();
        fn_804864AC();
        return 0;
    }
    fn_803800AC();
    fn_80051734()->Init();
    fn_80332E7C(&openParams);
    fn_80051850(10, 0);
    fn_80051144();
    if (!fn_80478F74()) {
        fn_80485EF4();
        fn_80486138();
        fn_804864AC();
        return 0;
    }
    return 1;
}

extern "C" int fn_80051528(void) {
    return Initialize();
}
