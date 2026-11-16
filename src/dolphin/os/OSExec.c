/* Dolphin SDK OS: rebooting into another program on the disc.
 *
 * __OSBootDolSimple shuts the drive down cleanly, reads the disc's apploader to 0x81200000 and
 * lets it load the target executable, then jumps to its entry point. The parameters for the new
 * program (restart code, preserved region, argv) are left in arena memory that __OSExecParams
 * (0x800030F0) points at; the new program's OSInit picks them up with __OSGetExecParams. Discs
 * whose apploader predates 2004/02/01 cannot do this, so they get the old reboot path instead:
 * the small reboot program after the apploader is run at 0x81300000.
 *
 * The DVD and string calls this file makes are still unnamed in the binary: fn_8037C0F8 is
 * DVDReadAbsAsyncPrio, fn_8037C548 DVDGetCommandBlockStatus, fn_8037C9F4 DVDCheckDisk,
 * fn_8037C9EC DVDGetCurrentDiskID, fn_8037C640 DVDSetAutoInvalidation, fn_8037C650 DVDResume,
 * fn_8037CAEC __DVDPrepareResetAsync, fn_8037C378 DVDCancelStreamAsync, fn_803AC8DC strncmp;
 * the reset and reboot code lives in OSReset.c and OSReboot.c. */
#include <dolphin/os.h>

void *memset(void *dst, int value, unsigned long n);
void *memcpy(void *dst, const void *src, unsigned long n);
unsigned long strlen(const char *s);
char *strcpy(char *dst, const char *src);
int sprintf(char *s, const char *format, ...);
int fn_803AC8DC(const char *a, const char *b, unsigned long n);

volatile u32 __PIRegs[] : 0xCC003000;

typedef struct OSExecParams {
    BOOL valid;
    u32 restartCode;
    u32 bootDol;
    void *regionStart;
    void *regionEnd;
    BOOL argsUseDefault;
    void *argsAddr;
} OSExecParams;

typedef struct DVDDiskID {
    char gameName[4];
    char company[2];
    u8 diskNumber;
    u8 gameVersion;
    u8 streaming;
    u8 streamingBufSize;
    u8 padding[22];
} DVDDiskID;

typedef struct DVDCommandBlock DVDCommandBlock;
typedef void (*DVDCBCallback)(s32 result, DVDCommandBlock *block);
struct DVDCommandBlock {
    DVDCommandBlock *next;
    DVDCommandBlock *prev;
    u32 command;
    s32 state;
    u32 offset;
    u32 length;
    void *addr;
    u32 currTransferSize;
    u32 transferredSize;
    DVDDiskID *id;
    DVDCBCallback callback;
    void *userData;
};

/* The header at the start of the disc's apploader. */
typedef struct AppLoaderStruct {
    char date[16];
    void (*entry)(void *init, void *main, void *close);
    u32 size;       /* of the apploader proper */
    u32 rebootSize; /* of the reboot program after it */
    u32 reserved;
} AppLoaderStruct;

typedef void (*AppLoaderReport)(const char *msg, ...);
typedef void (*AppLoaderInit)(AppLoaderReport report);
typedef BOOL (*AppLoaderMain)(void **addr, s32 *length, s32 *offset);
typedef void *(*AppLoaderClose)(void);

OSExecParams *__OSExecParams : 0x800030F0;
#define __OSAppLoaderOffset (*(s32 *)OSPhysicalToCached(0x30F4))
#define __OSRebootFlag (*(u8 *)OSPhysicalToCached(0x30E2))
#define __OSRebootRegionStart (*(void **)OSPhysicalToCached(0x12FDFF0))
#define __OSRebootRegionEnd (*(void **)OSPhysicalToCached(0x12FDFEC))

#define APPLOADER_ADDR ((void *)OSPhysicalToCached(0x1200000))
#define REBOOT_ADDR ((void *)OSPhysicalToCached(0x1300000))
#define DEFAULT_APPLOADER_POSITION 0x2440
#define ARGS_SIZE 0x2000
#define OSRoundUp32B(x) (((u32)(x) + 31) & ~31)

#define PI_RESET_CODE 9 /* __PIRegs index of the reset register */

extern BOOL __OSIsGcam;

void *OSAllocFromArenaLo(u32 size, u32 align);
void OSSetArenaLo(void *addr);
void DVDInit(void);
BOOL fn_8037C0F8(DVDCommandBlock *block, void *addr, s32 length, s32 offset,
                 DVDCBCallback callback, s32 prio);
s32 fn_8037C548(DVDCommandBlock *block);
BOOL fn_8037C9F4(void);
DVDDiskID *fn_8037C9EC(void);
BOOL fn_8037C640(BOOL autoInval);
void fn_8037C650(void);
BOOL fn_8037CAEC(void (*callback)(void));
BOOL fn_8037C378(DVDCommandBlock *block, DVDCBCallback callback);
void AISetStreamVolLeft(u8 volume);
void AISetStreamVolRight(u8 volume);
void AISetStreamPlayState(u32 state);
void __OSDoHotReset(u32 resetCode);
void OSGetSaveRegion(void **start, void **end);
u32 __OSMaskInterrupts(u32 mask);
u32 __OSUnmaskInterrupts(u32 mask);

static volatile BOOL Prepared;

/* Lays argv out at the top of the 8 KB block at `addr`: the strings, then the argument count
 * followed by the offsets of the strings, which the new program turns back into pointers. */
static BOOL PackArgs(void *addr, s32 argc, char *argv[]) {
    s32 numArgs;
    char *bootInfo2;
    char *ptr;
    char **list;
    u32 i;

    bootInfo2 = (char *)addr;
    memset(bootInfo2, 0, ARGS_SIZE);

    if (argc == 0) {
        *(u32 *)&bootInfo2[8] = 0;
    } else {
        numArgs = argc;
        ptr = bootInfo2 + ARGS_SIZE;
        while (--argc >= 0) {
            ptr -= strlen(argv[argc]) + 1;
            strcpy(ptr, argv[argc]);
            argv[argc] = (char *)(ptr - bootInfo2);
        }

        ptr = bootInfo2 + ((ptr - bootInfo2) & ~3);
        ptr -= 4 * (numArgs + 1);
        list = (char **)ptr;
        for (i = 0; i < numArgs + 1; i++) {
            list[i] = argv[i];
        }
        ptr -= 4;
        *(s32 *)ptr = numArgs;
        *(u32 *)&bootInfo2[8] = ptr - bootInfo2;
    }
    return TRUE;
}

/* Jumps to `entryPoint` with a clean instruction cache. */
static asm void Run(register void *entryPoint) {
    fralloc
    bl ICFlashInvalidate
    sync
    isync
    mtlr entryPoint
    blr
    frfree
    blr
}

static void ReadDisc(void *addr, s32 length, s32 offset);
static s32 GetApploaderPosition(void);

/* Reads the apploader's header and then the apploader itself to 0x81200000. It stands ahead of
 * the two helpers it calls on purpose: retail inlines this into __OSBootDolSimple but calls
 * ReadDisc and GetApploaderPosition from it, where every later use of them is inlined - which
 * is what the compiler does for callees it has not seen the body of yet. */
static inline AppLoaderStruct *LoadApploader(void) {
    AppLoaderStruct *header;

    header = (AppLoaderStruct *)OSAllocFromArenaLo(0x20, 32);
    ReadDisc(header, 0x20, GetApploaderPosition());
    ReadDisc(APPLOADER_ADDR, OSRoundUp32B(header->size), GetApploaderPosition() + 0x20);
    ICInvalidateRange(APPLOADER_ADDR, OSRoundUp32B(header->size));
    return header;
}

/* Reads from the disc and waits; a disc that goes away meanwhile resets the console. */
static void ReadDisc(void *addr, s32 length, s32 offset) {
    DVDCommandBlock block;

    fn_8037C0F8(&block, addr, length, offset, NULL, 0);
    while (fn_8037C548(&block)) {
        if (!fn_8037C9F4()) {
            __OSDoHotReset(0);
        }
    }
}

static void Callback(void) {
    Prepared = TRUE;
}

void __OSGetExecParams(OSExecParams *params) {
    if ((u32)OSPhysicalToCached(0) <= (u32)__OSExecParams) {
        memcpy(params, __OSExecParams, sizeof(OSExecParams));
    } else {
        params->valid = FALSE;
    }
}

/* Disc offset of the apploader: fixed on retail discs, taken from the TGC header otherwise. */
static s32 apploaderPosition;

static s32 GetApploaderPosition(void) {
    u32 *tgcHeader;
    u32 unused[2]; /* stack the original reserves but never touches */

    if (apploaderPosition != 0) {
        return apploaderPosition;
    }

    if (__OSAppLoaderOffset != 0) {
        tgcHeader = (u32 *)OSAllocFromArenaLo(0x40, 32);
        ReadDisc(tgcHeader, 0x40, __OSAppLoaderOffset);
        apploaderPosition = __OSAppLoaderOffset + tgcHeader[0x38 / 4];
    } else {
        apploaderPosition = DEFAULT_APPLOADER_POSITION;
    }
    return apploaderPosition;
}

static inline BOOL IsStreamEnabled(void) {
    if (fn_8037C9EC()->streaming) {
        return TRUE;
    }
    return FALSE;
}

static inline BOOL IsNewApploader(AppLoaderStruct *header) {
    return fn_803AC8DC(header->date, "2004/02/01", 10) > 0 ? TRUE : FALSE;
}

/* Lets the apploader read the executable in, piece by piece; returns its entry point. */
static inline void *LoadDol(OSExecParams *params,
                            void (*entry)(void *init, void *main, void *close)) {
    AppLoaderInit appLoaderInit;
    AppLoaderMain appLoaderMain;
    AppLoaderClose appLoaderClose;
    void *addr;
    s32 length;
    s32 offset;
    OSExecParams *copy;

    entry(&appLoaderInit, &appLoaderMain, &appLoaderClose);
    copy = (OSExecParams *)OSAllocFromArenaLo(sizeof(OSExecParams), 1);
    memcpy(copy, params, sizeof(OSExecParams));
    __OSExecParams = copy;
    appLoaderInit(OSReport);
    OSSetArenaLo(copy);
    while (appLoaderMain(&addr, &length, &offset)) {
        ReadDisc(addr, length, offset);
    }
    return appLoaderClose();
}

/* Leaves the parameters where the new program looks for them and jumps to it. */
static inline void StartDol(OSExecParams *params, void *entry) {
    OSExecParams *copy;

    copy = (OSExecParams *)OSAllocFromArenaLo(sizeof(OSExecParams), 1);
    memcpy(copy, params, sizeof(OSExecParams));
    __OSExecParams = copy;
    __PIRegs[PI_RESET_CODE] = 7;
    OSDisableInterrupts();
    Run(entry);
}

void __OSBootDolSimple(u32 doloffset, u32 restartCode, void *regionStart, void *regionEnd,
                       BOOL argsUseDefault, s32 argc, char **argv) {
    OSExecParams *params;
    AppLoaderStruct *header;
    void *dolEntry;
    DVDCommandBlock block;
    u32 unused; /* stack the original reserves but never touches */

    OSDisableInterrupts();

    params = (OSExecParams *)OSAllocFromArenaLo(sizeof(OSExecParams), 1);
    params->valid = TRUE;
    params->restartCode = restartCode;
    params->regionStart = regionStart;
    params->regionEnd = regionEnd;
    params->argsUseDefault = argsUseDefault;
    if (!argsUseDefault) {
        params->argsAddr = OSAllocFromArenaLo(ARGS_SIZE, 1);
        PackArgs(params->argsAddr, argc, argv);
    }

    /* Let the drive finish whatever it is doing, with only the DI interrupt enabled. */
    DVDInit();
    fn_8037C640(TRUE);
    fn_8037C650();
    Prepared = FALSE;
    fn_8037CAEC(Callback);
    __OSMaskInterrupts(~0x1F);
    __OSUnmaskInterrupts(0x400);
    OSEnableInterrupts();
    while (Prepared != TRUE) {
        if (!fn_8037C9F4()) {
            __OSDoHotReset(0);
        }
    }

    /* Silence and stop any audio streaming from the disc. */
    if (!__OSIsGcam && IsStreamEnabled()) {
        AISetStreamVolLeft(0);
        AISetStreamVolRight(0);
        fn_8037C378(&block, NULL);
        while (fn_8037C548(&block)) {
            if (!fn_8037C9F4()) {
                __OSDoHotReset(0);
            }
        }
        AISetStreamPlayState(0);
    }

    header = LoadApploader();

    if (IsNewApploader(header)) {
        if (doloffset == 0xFFFFFFFF) {
            /* The executable follows the apploader. The constant comes first: that is the
             * order retail adds the three terms in. */
            doloffset = sizeof(AppLoaderStruct) + GetApploaderPosition() + header->size;
        }
        params->bootDol = doloffset;
        dolEntry = LoadDol(params, header->entry);
        StartDol(params, dolEntry);
    } else {
        /* Old apploader: run the reboot program that follows it instead. */
        __OSRebootRegionStart = regionStart;
        __OSRebootRegionEnd = regionEnd;
        __OSRebootFlag = TRUE;
        ReadDisc(REBOOT_ADDR, OSRoundUp32B(header->rebootSize),
                 sizeof(AppLoaderStruct) + GetApploaderPosition() + header->size);
        ICInvalidateRange(REBOOT_ADDR, OSRoundUp32B(header->rebootSize));
        OSDisableInterrupts();
        ICFlashInvalidate();
        Run(REBOOT_ADDR);
    }
}

/* Boots the executable at disc offset `doloffset`, passing the offset as argv[0]. */
void __OSBootDol(u32 doloffset, u32 restartCode, const char **argv) {
    char doloffInString[20];
    s32 argvlen;
    char **argvToPass;
    s32 i;
    void *saveStart;
    void *saveEnd;

    OSGetSaveRegion(&saveStart, &saveEnd);
    sprintf(doloffInString, "%d", doloffset);

    argvlen = 0;
    if (argv != NULL) {
        while (argv[argvlen] != NULL) {
            argvlen++;
        }
    }

    argvToPass = (char **)OSAllocFromArenaLo((argvlen + 2) * sizeof(char *), 1);
    argvToPass[0] = doloffInString;
    argvlen++;
    for (i = 1; i < argvlen; i++) {
        argvToPass[i] = (char *)argv[i - 1];
    }

    __OSBootDolSimple(0xFFFFFFFF, restartCode, saveStart, saveEnd, FALSE, argvlen,
                      argvToPass);
}
