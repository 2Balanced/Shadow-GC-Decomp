/* Dolphin SDK DVD: disc ID comparison. A zero game name and a disc number or version of 0xFF act
 * as wildcards; the company code must be present in both and equal. */
#include <dolphin/dvd/__dvd.h>

int fn_803AC8DC(const char *a, const char *b, u32 n);
#define strncmp fn_803AC8DC

BOOL fn_8037CFD0(DVDDiskID *id1, DVDDiskID *id2) {
    if (id1->gameName[0] && id2->gameName[0] && strncmp(&id1->gameName[0], &id2->gameName[0], 4)) {
        return FALSE;
    }
    if (!id1->company[0] || !id2->company[0] || strncmp(&id1->company[0], &id2->company[0], 2)) {
        return FALSE;
    }
    if (id1->diskNumber != 0xFF && id2->diskNumber != 0xFF && id1->diskNumber != id2->diskNumber) {
        return FALSE;
    }
    if (id1->gameVersion != 0xFF && id2->gameVersion != 0xFF && id1->gameVersion != id2->gameVersion) {
        return FALSE;
    }
    return TRUE;
}
