/* The files the cars share (class name unknown; see Game/VehicleCarResource.h): loading them once,
 * and three of each handling type's files. A unit of its own: it follows Game/TVehicleCar.cpp's
 * template instances in .text. Names are inferred. Definitions are in reverse address order
 * (deferred inlining emits functions last-defined first). */
#include "Game/VehicleCarResource.h"
#include <dolphin/types.h>

extern "C" {
/* Resource loading (see Game/VehicleWalker.cpp), the loader and its files' offsets (see
 * Game/BkLarva.cpp). */
void fn_8006B50C(int id, int arg1);
void *fn_800122D4(void);
int fn_80012608(void *loader, int slot, int id);
}

/* The three files of each type. */
static const UnkCarFiles s_files[4][6] = {
    {{14, 0, 7}, {15, 1, 8}, {16, 2, 9}, {17, 3, 10}, {18, 4, 11}, {19, 5, 12}},
    {{14, 0, 7}, {15, 1, 8}, {16, 2, 9}, {17, 3, 10}, {18, 4, 11}, {19, 5, 12}},
    {{14, 0, 7}, {15, 1, 8}, {16, 2, 9}, {17, 3, 10}, {18, 4, 11}, {19, 5, 12}},
    {{26, 3, 14}, {28, 4, 16}, {31, 7, 19}, {32, 8, 20}, {33, 9, 21}, {34, 10, 22}},
};

UnkCarResource_801B9950::UnkCarResource_801B9950() {
    m_loaded = false;
}

bool UnkCarResource_801B9950::Load() {
    if (m_loaded) {
        return false;
    }
    fn_8006B50C(0x64, 0);
    fn_8006B50C(0x5E, 0);
    m_loaded = true;
    return true;
}

void UnkCarResource_801B9950::Reset() {
    m_loaded = false;
}

const UnkCarFiles *UnkCarResource_801B9950::GetFiles(int type, int index) {
    static UnkCarFiles s_result;
    s_result = s_files[type][index];
    int base;
    if (type == 3) {
        base = fn_80012608(fn_800122D4(), 3, 0x5E);
    } else {
        base = fn_80012608(fn_800122D4(), 3, 0x64);
    }
    s_result.a += base;
    s_result.b += base;
    s_result.c += base;
    return &s_result;
}
