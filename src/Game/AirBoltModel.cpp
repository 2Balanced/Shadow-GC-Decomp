/* Vehicle::AirBoltModel (class name from RTTI; declared in Game/AirBoltModel.h): the model of the
 * AirBolt vehicle, a BirdModelBase built from the motion table below. Member names are inferred.
 * Definitions are in reverse address order (deferred inlining emits functions last-defined
 * first). */
#include "Game/AirBoltModel.h"
#include "Game/UnkMotion_8022F058.h"

extern "C" {
/* Loads resource id (behaviour inferred; see Game/WeaponDarkHammer.cpp). */
void fn_8006B50C(int id, int arg1);
}

namespace Vehicle {

static const BirdMotionFrames s_frames[] = {
    {0x86, 0x74, 0x7D}, {0x86, 0x74, 0x7D}, {0x88, 0x76, 0x7F},
    {0x83, 0x71, 0x7A}, {0x80, 0x6E, 0x77},
};

static const BirdMotionEntry s_motions[] = {
    {0x5D, 7, 0}, {0x5D, 6, 1}, {0x5D, 9, 2}, {0x5D, 3, 3}, {0x5D, 0, 4},
};

static const BirdMotionSet s_motionSets[] = {
    {-1, 0x5B, 4, s_motions, 5},
};

static const s32 s_ids[] = {0x1F40, 0x251C, 0x251D, 0x251E, 0x251F, 0x2520, 0x2521};

static const BirdMotionTable s_motionTable = {s_motionSets, 1, s_ids, 7, s_frames, 0x5D, 100};

AirBoltModel::AirBoltModel(CharInfo *owner) : BirdModelBase(owner, &s_motionTable) {}

AirBoltModel::~AirBoltModel() {}

void AirBoltModel::Load() {
    fn_8006B50C(0x5B, 0);
    fn_8006B50C(4, 0);
    fn_8006B50C(0x5D, 0);
}

} // namespace Vehicle
