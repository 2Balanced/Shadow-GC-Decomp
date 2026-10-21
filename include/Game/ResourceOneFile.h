#ifndef GAME_RESOURCEONEFILE_H
#define GAME_RESOURCEONEFILE_H

#include <dolphin/types.h>
#include "size_assert.h"

/* Resource and ResourceOneFile (class names from RTTI; vtables at 0x8051C66C and 0x8051C6BC): a
 * loadable resource, and one that is a single file. Only the constructor is declared (0x80014C58;
 * the flag sets bit 0 of the flag word at 0x1C, meaning unknown); the size comes from the
 * allocation in EggLastMech's loader. */
class ResourceOneFile {
  public:
    ResourceOneFile(const char *fileName, bool flag);

    /* 0x00 */ u8 field_0x0[0x28];
};
SIZE_ASSERT(ResourceOneFile, 0x28);

extern "C" {
/* The resource loader, and adding and removing the resource of a slot (behaviour inferred). */
void *fn_800122D4(void);
void fn_8001299C(void *loader, int slot, ResourceOneFile *resource);
void fn_800127F8(void *loader, int slot);
}

#endif
