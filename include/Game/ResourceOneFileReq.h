#ifndef GAME_RESOURCEONEFILEREQ_H
#define GAME_RESOURCEONEFILEREQ_H

#include <dolphin/types.h>
#include "size_assert.h"

/* ResourceOneFileReq (class name from RTTI; vtable at 0x8051FA3C, base Resource): a single-file
 * resource loaded on request. Only the constructor is declared (0x8006B2A4); what list points to
 * is unknown (the common gadget data passes the .bss object at 0x80575EE4). The size comes from the
 * allocation in GadgetCommand.cpp's loader. */
class ResourceOneFileReq {
  public:
    ResourceOneFileReq(const char *fileName, void *list);

    /* 0x00 */ u8 field_0x0[0x28];
};
SIZE_ASSERT(ResourceOneFileReq, 0x28);

#endif
