/* Dolphin SDK OS: rebooting into another program on the disc.
 *
 * __OSBootDolSimple shuts the drive down cleanly, reads the disc's apploader to 0x81200000 and
 * lets it load the target executable, then jumps to its entry point. The parameters for the new
 * program (restart code, preserved region, argv) are left in arena memory that __OSExecParams
 * (0x800030F0) points at; the new program's OSInit picks them up with __OSGetExecParams. Discs
 * whose apploader predates 2004/02/01 cannot do this, so they get the old reboot path instead:
 * the small reboot program after the apploader is run at 0x81300000.
 *
 * The DVD, AI and string calls this file makes are still unnamed in the binary: fn_8037C0F8 is
 * DVDReadAbsAsyncPrio, fn_8037C548 DVDGetCommandBlockStatus, fn_8037C9F4 DVDCheckDisk,
 * fn_8037C9EC DVDGetCurrentDiskID, fn_8037C640 DVDSetAutoInvalidation, fn_8037C650 DVDResume,
 * fn_8037CAEC __DVDPrepareResetAsync, fn_8037C378 DVDCancelStreamAsync, fn_803813E0 and
 * fn_8038140C AISetStreamVolLeft/Right, fn_80381120 AISetStreamPlayState, fn_803AC8DC strncmp,
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
void fn_803813E0(u8 volume);
void fn_8038140C(u8 volume);
void fn_80381120(u32 state);
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

/* Built without automatic inlining: the helper code it does inline is written out in place. */
#pragma dont_inline on
void __OSBootDolSimple(u32 doloffset, u32 restartCode, void *regionStart, void *regionEnd,
                       BOOL argsUseDefault, s32 argc, char **argv) {
    DVDCommandBlock streamBlock;
    u32 unused1; /* stack the original reserves but never touches */
    s32 offset;
    s32 length;
    void *addr;
    AppLoaderClose appLoaderClose;
    AppLoaderMain appLoaderMain;
    AppLoaderInit appLoaderInit;
    u32 unused2;
    DVDCommandBlock rebootBlock;
    DVDCommandBlock tgcBlock;
    DVDCommandBlock loadBlock;
    u32 unused3[2];
    OSExecParams *params;
    OSExecParams *copy;
    AppLoaderStruct *header;
    void *dolEntry;
    s32 pos;
    u32 *tgcHeader;

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
    if (!__OSIsGcam && (fn_8037C9EC()->streaming ? TRUE : FALSE)) {
        fn_803813E0(0);
        fn_8038140C(0);
        fn_8037C378(&streamBlock, NULL);
        while (fn_8037C548(&streamBlock)) {
            if (!fn_8037C9F4()) {
                __OSDoHotReset(0);
            }
        }
        fn_80381120(0);
    }

    header = (AppLoaderStruct *)OSAllocFromArenaLo(0x20, 32);
    ReadDisc(header, 0x20, GetApploaderPosition());
    ReadDisc(APPLOADER_ADDR, OSRoundUp32B(header->size), GetApploaderPosition() + 0x20);
    ICInvalidateRange(APPLOADER_ADDR, OSRoundUp32B(header->size));

    if ((fn_803AC8DC(header->date, "2004/02/01", 10) > 0) ? TRUE : FALSE) {
        if (doloffset == 0xFFFFFFFF) {
            /* The executable follows the apploader. */
            if (apploaderPosition != 0) {
                pos = apploaderPosition;
            } else {
                if (__OSAppLoaderOffset != 0) {
                    tgcHeader = (u32 *)OSAllocFromArenaLo(0x40, 32);
                    fn_8037C0F8(&tgcBlock, tgcHeader, 0x40, __OSAppLoaderOffset, NULL, 0);
                    while (fn_8037C548(&tgcBlock)) {
                        if (!fn_8037C9F4()) {
                            __OSDoHotReset(0);
                        }
                    }
                    apploaderPosition = __OSAppLoaderOffset + tgcHeader[0x38 / 4];
                } else {
                    apploaderPosition = DEFAULT_APPLOADER_POSITION;
                }
                pos = apploaderPosition;
            }
            doloffset = pos + header->size + 0x20;
        }
        params->bootDol = doloffset;

        header->entry(&appLoaderInit, &appLoaderMain, &appLoaderClose);
        copy = (OSExecParams *)OSAllocFromArenaLo(sizeof(OSExecParams), 1);
        memcpy(copy, params, sizeof(OSExecParams));
        __OSExecParams = copy;
        appLoaderInit(OSReport);
        OSSetArenaLo(copy);
        while (appLoaderMain(&addr, &length, &offset)) {
            s32 o = offset;
            s32 l = length;
            void *a = addr;

            fn_8037C0F8(&loadBlock, a, l, o, NULL, 0);
            while (fn_8037C548(&loadBlock)) {
                if (!fn_8037C9F4()) {
                    __OSDoHotReset(0);
                }
            }
        }
        dolEntry = appLoaderClose();

        copy = (OSExecParams *)OSAllocFromArenaLo(sizeof(OSExecParams), 1);
        memcpy(copy, params, sizeof(OSExecParams));
        __OSExecParams = copy;
        __PIRegs[PI_RESET_CODE] = 7;
        OSDisableInterrupts();
        Run(dolEntry);
    } else {
        /* Old apploader: run the reboot program that follows it instead. */
        __OSRebootRegionStart = regionStart;
        __OSRebootRegionEnd = regionEnd;
        __OSRebootFlag = TRUE;
        {
            s32 o;
            s32 l;
            void *a;

            o = GetApploaderPosition() + header->size;
            o += 0x20;
            l = OSRoundUp32B(header->rebootSize);
            a = REBOOT_ADDR;

            fn_8037C0F8(&rebootBlock, a, l, o, NULL, 0);
        }
        while (fn_8037C548(&rebootBlock)) {
            if (!fn_8037C9F4()) {
                __OSDoHotReset(0);
            }
        }
        ICInvalidateRange(REBOOT_ADDR, OSRoundUp32B(header->rebootSize));
        OSDisableInterrupts();
        ICFlashInvalidate();
        Run(REBOOT_ADDR);
    }
}
#pragma dont_inline reset

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
