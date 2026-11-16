/* Dolphin SDK OS: rebooting into the disc's main program (OSResetSystem's restart path). The
 * arena is moved to the top of RAM, out of the way of the apploader, and the boot runs on a fresh
 * exception context. */
#include <dolphin/os.h>

void OSSetArenaLo(void *addr);
void OSSetArenaHi(void *addr);
void __OSBootDol(u32 doloffset, u32 restartCode, const char **argv);

static void *SaveStart = NULL;
static void *SaveEnd = NULL;

void __OSReboot(u32 resetCode, u32 bootDol) {
    OSContext exceptionContext;
    char *argvToPass;

    OSDisableInterrupts();
    OSSetArenaLo((void *)0x81280000);
    OSSetArenaHi((void *)0x812F0000);
    OSClearContext(&exceptionContext);
    OSSetCurrentContext(&exceptionContext);
    argvToPass = NULL;
    __OSBootDol(bootDol, resetCode | 0x80000000, (const char **)&argvToPass);
}

/* The range of memory the next program should leave untouched. */
void OSGetSaveRegion(void **start, void **end) {
    *start = SaveStart;
    *end = SaveEnd;
}
