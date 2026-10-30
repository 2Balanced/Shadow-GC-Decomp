#ifndef GAME_TVEHICLECARMOVE_H
#define GAME_TVEHICLECARMOVE_H

#include "Game/MathTypes.h"
#include <dolphin/types.h>
#include "size_assert.h"

class CharCommand;

namespace Vehicle {

class TVehicleCar;

/* Vehicle::TVehicleCarMove (class name from the RTTI of its control block and of its bound
 * callbacks): the driving of a car (Vehicle::TVehicleCar), which owns it through a shared_ptr.
 * Constructor 0x801BF610, destructor 0x801BF580 (not virtual). This header gives the car's unit
 * (Game/TVehicleCar.cpp) the part it uses; member and function names are inferred.
 *
 * Its own unit (Game/TVehicleCarMove.cpp, 0x801B995C-0x801BFB70, which declares the whole class)
 * keeps its .data whole (0x8053BAC0-0x8053BF10: the TNumEdit/CharaColliBaseCharInfo lists, then the
 * per-type parameter table at 0x8053BAF0, 4 x 0xAC, named "Jeep", "Open", "GUN", "BIKE"). Layout
 * seen so far: 0x004 and 0x120 two 0x11C-byte states (the second a copy of the first, through the
 * implicit assignment 0x801BADCC), 0x23C 0x20 bytes, 0x25C the type, 0x264 the car, 0x268
 * shared_ptr<TVehicleCamera>, 0x270 and 0x278 the body colliders, 0x280 the bird collision
 * (UnkBirdCollision_801B4EBC *), 0x284 the body, 0x2C4 four shared_ptr<CollisionEffect>, 0x2E4
 * a hit list. */
class TVehicleCarMove {
  public:
    /* The car's position and rotation (struct name unknown). */
    struct Body {
        /* 0x0 */ Vec3f m_position;
        /* 0xC */ Vec3f m_rotation;
    };

    /* type: the car's model and handling (0 Jeep, 1 Open, 2 GUN, 3 BIKE; see ToType). */
    TVehicleCarMove(TVehicleCar *car, int type);
    ~TVehicleCarMove();

    /* The handling type of a car kind (1 to 4; 0x801BA558). */
    static int ToType(int kind);
    /* Called by the cars' loaders before their files load (0x801BF7C0, empty). */
    static void Initialize(int kind);

    /* Places the car (0x801BABF8). */
    void SetPlacement(const Vec3f *position, const Vec3f *rotation);
    /* Takes a command sent to the car (0x801BDD84). */
    void OnCommand(CharCommand *command);
    /* Updates the model from the state (0x801BDFA8). */
    void UpdateModel();
    /* Per-frame work (0x801BE8B0). */
    void Tick(float dt);
    /* Sets up the car on its first frame (0x801BF52C). */
    void Start();
    /* Switches the camera: 1 when the rider gets off, 2 when one gets on (0x801BEE58). */
    void SetCamera(int mode, int arg);

    Body *GetBody() {
        return &m_body;
    }

    /* 0x000 */ u8 field_0x0[0x8];
    /* 0x008 */ Vec3f m_position;
    /* 0x014 */ Vec3f m_rotation;
    /* 0x020 */ u8 field_0x20[0x284 - 0x20];
    /* 0x284 */ Body m_body;
    /* 0x29C */ u8 field_0x29C[0x2F0 - 0x29C];
};
SIZE_ASSERT(TVehicleCarMove, 0x2F0);

} // namespace Vehicle

#endif
