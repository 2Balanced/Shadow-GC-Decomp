/* Dolphin SDK debugger interface. A debugger stub (e.g. on a development kit) shares a small
 * record at physical 0x40 with the game: whether it is present, which exceptions it wants, and
 * where to jump on one. DBStack, DBIsDebuggerPresent, __DBMarkException and __DBSetPresent are
 * unused by the game and were dead-stripped. */
#include <dolphin/base/PPCArch.h>
#include <dolphin/os.h>

typedef struct DBInterface {
    u32 bPresent;
    u32 exceptionMask;
    void (*ExceptionDestination)(void);
    void *exceptionReturn;
} DBInterface;

BOOL DBVerbose;
DBInterface *__DBInterface;

void __DBExceptionDestination(void);

void DBInit(void) {
    __DBInterface = OSPhysicalToCached(0x40);
    __DBInterface->ExceptionDestination = (void (*)(void))OSCachedToPhysical(__DBExceptionDestination);
    DBVerbose = TRUE;
}

/* Reached with translation off from __DBExceptionDestination: reports the context saved at
 * physical 0xC0 and stops. */
void __DBExceptionDestinationAux(void) {
    u32 *contextAddr;
    OSContext *context;

    contextAddr = (void *)0xC0;
    context = OSPhysicalToCached(*contextAddr);
    OSReport("DBExceptionDestination\n");
    OSDumpContext(context);
    PPCHalt();
}

/* Turns address translation back on (MSR[IR|DR]) before running C code. */
asm void __DBExceptionDestination(void) {
    nofralloc
    mfmsr r3
    ori r3, r3, 0x30
    mtmsr r3
    b __DBExceptionDestinationAux
}

BOOL __DBIsExceptionMarked(__OSException exception) {
    u32 mask = (1 << exception);
    return __DBInterface->exceptionMask & mask;
}

void DBPrintf(char *str, ...) {}
