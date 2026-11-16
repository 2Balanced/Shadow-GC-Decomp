/* Dolphin SDK CARD: the two settings the rest of the library reads. In the SDK this file also holds
 * the calls for network-era titles (serial number, file attributes); this game links none of them,
 * so only the data is left. It has no code to place it by: its .sdata sits after CARDUnlock's and
 * before GX's, and the two variables share one 8-byte-aligned block, so they come from one file. */
#include <dolphin/card_internal.h>

u16 __CARDVendorID = 0xFFFF; /* 0xFFFF: no vendor ID has been read yet */
u8 __CARDPermMask = 0x1C;    /* the permission bits another game's file must have to be visible */
