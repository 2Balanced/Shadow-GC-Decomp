/* Dolphin SDK OS: relocatable modules. The game links no REL files, so only the start-up hook
 * survives: it empties the module list (0x800030C8/0x800030CC) and the string table pointer
 * (0x800030D0) the debugger reads. */
#include <dolphin/os.h>

typedef struct OSModuleInfo OSModuleInfo;

typedef struct OSModuleQueue {
    OSModuleInfo *head;
    OSModuleInfo *tail;
} OSModuleQueue;

OSModuleQueue __OSModuleList : 0x800030C8;
void *__OSStringTable : 0x800030D0;

void __OSModuleInit(void) {
    __OSModuleList.head = __OSModuleList.tail = NULL;
    __OSStringTable = NULL;
}
