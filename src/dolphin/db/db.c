/* Dolphin SDK debugger hooks. When a debugger stub is resident (development hardware) it shares a
 * four-word block with the game at physical address 0x40: a presence flag, the set of exception
 * numbers it wants to see, and the physical entry point it should branch to. Only the pieces the
 * game links survive; the presence queries and DBStack were dead-stripped. */
#include <dolphin/base/PPCArch.h>
#include <dolphin/os.h>

/* Layout of the block shared with the debugger stub. */
typedef struct DebuggerBlock {
    u32 attached;
    u32 trapMask;
    void (*entry)(void);
    void *resume;
} DebuggerBlock;

#define DB_BLOCK_PHYS 0x40       /* where the stub keeps its DebuggerBlock */
#define DB_CONTEXT_SLOT_PHYS 0xC0 /* physical pointer to the faulting OSContext */

typedef void (*DebuggerEntry)(void);

BOOL DBVerbose;
DebuggerBlock *__DBInterface;

void __DBExceptionDestination(void);

void DBInit(void) {
    __DBInterface = (DebuggerBlock *)OSPhysicalToCached(DB_BLOCK_PHYS);
    /* The stub runs untranslated, so it needs the physical address of our entry stub. */
    __DBInterface->entry = (DebuggerEntry)OSCachedToPhysical(__DBExceptionDestination);
    DBVerbose = TRUE;
}

/* Runs once __DBExceptionDestination has re-enabled translation: dump the saved context, halt. */
void __DBExceptionDestinationAux(void) {
    u32 *slot = (u32 *)DB_CONTEXT_SLOT_PHYS;
    OSContext *faulting = (OSContext *)OSPhysicalToCached(*slot);

    OSReport("DBExceptionDestination\n");
    OSDumpContext(faulting);
    PPCHalt();
}

/* Entered from the debugger with MSR[IR] and MSR[DR] clear; set both, then continue in C. */
asm void __DBExceptionDestination(void) {
    nofralloc
    mfmsr r3
    ori r3, r3, 0x30
    mtmsr r3
    b __DBExceptionDestinationAux
}

BOOL __DBIsExceptionMarked(__OSException exception) {
    return __DBInterface->trapMask & (1 << exception);
}

void DBPrintf(char *str, ...) {}
