#ifndef GAME_VEHICLECARRESOURCE_H
#define GAME_VEHICLECARRESOURCE_H

#include <dolphin/types.h>
#include "size_assert.h"

/* Three file offsets of a car (struct name unknown; meanings unknown). */
struct UnkCarFiles {
    s32 a;
    s32 b;
    s32 c;
};

/* The files the cars share, loaded once (class name unknown; Game/VehicleCarResource.cpp, the
 * instance from Game/TVehicleCar.cpp, the files read by Game/TVehicleCarMove.cpp). Names are
 * inferred. */
class UnkCarResource_801B9950 {
  public:
    UnkCarResource_801B9950();
    /* Loads the files unless they are loaded; whether it loaded them. */
    bool Load();
    void Reset();
    /* Three of the files of a handling type (0 to 3), with the loader's offsets added. */
    const UnkCarFiles *GetFiles(int type, int index);

    /* The one instance (0x801B8828; Game/TVehicleCar.cpp). */
    static UnkCarResource_801B9950 *GetInstance();

    /* The instance (0x805EF840). */
    static UnkCarResource_801B9950 *s_resource;

    /* 0x0 */ bool m_loaded;
};
SIZE_ASSERT(UnkCarResource_801B9950, 0x1);

#endif
