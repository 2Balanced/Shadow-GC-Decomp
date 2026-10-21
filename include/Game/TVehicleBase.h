#ifndef GAME_TVEHICLEBASE_H
#define GAME_TVEHICLEBASE_H

#include "Game/CharInfo.h"
#include "Game/MathTypes.h"
#include "boost/weak_ptr.hpp"
#include <dolphin/types.h>
#include "size_assert.h"

class CharCommand;

namespace Vehicle {

/* TVehicleBase (class name from RTTI; vtable at 0x8053B87C): the base of the vehicles
 * (Vehicle::GunLift, Vehicle::TVehicleCar, Vehicle::Walker, ...), a CharInfo. Constructor
 * 0x801B6C4C (character type 0x33), destructor 0x801B6B50. Layout from them; member and virtual
 * function names other than PushCommand are inferred or placeholders. The slots after CharInfo's
 * (0x30 to 0x7C) are the vehicle's own; slots 0x3C and 0x44 are pure (zero in its vtable). */
class TVehicleBase : public CharInfo {
  public:
    /* arg is passed on to the 0x2C-byte object at 0x98 (constructor 0x801B7338; meaning
     * unknown). */
    TVehicleBase(void *arg);
    virtual ~TVehicleBase();
    /* 0x801B6B2C */
    virtual void PushCommand(CharCommand *command);
    /* 0x801B6AB8, 0x801B6A44, 0x801B6A3C, 0x801B6A34, 0x801B69C0 */
    virtual bool vf_0x18();
    virtual bool vf_0x1C();
    virtual void vf_0x20();
    virtual bool vf_0x24();
    virtual void vf_0x28();
    /* Places the vehicle (0x801B663C; meaning inferred from GunLift, which calls it with its
     * body's position and rotation). */
    virtual void SetPlacement(const Vec3f *position, const Vec3f *rotation);
    /* Reads the seat's position and rotation; either may be null (0x801B657C; name inferred). */
    virtual void GetSeat(Vec3f *position, Vec3f *rotation);
    /* Builds the vehicle's models and colliders (0x801B634C; name inferred). */
    virtual void Setup();
    /* Takes a command sent to the vehicle (pure; name inferred). */
    virtual void OnCommand(CharCommand *command) = 0;
    virtual void vf_0x40();
    /* Per-frame work (pure; name inferred). */
    virtual void Tick(float dt) = 0;
    /* A rider gets on and off (0x801B62C4, 0x801B6254; names inferred from GunLift). */
    virtual void OnGetOn();
    virtual void OnGetOff();
    /* 0x801B624C, 0x801B6244 (meanings unknown; GunLift returns true from both). */
    virtual bool vf_0x50();
    virtual bool vf_0x54();
    virtual void vf_0x58();
    virtual void vf_0x5C();
    virtual void vf_0x60();
    virtual void vf_0x64();
    virtual void vf_0x68();
    virtual void vf_0x6C();
    virtual void vf_0x70();
    virtual void vf_0x74();
    virtual void vf_0x78();
    virtual void vf_0x7C();

    /* The rider (0x801B67C4) and a second character (0x801B66EC; the shooter of the GunLift's
     * bullets) (names inferred). */
    boost::weak_ptr<CharInfo> &GetRider();
    boost::weak_ptr<CharInfo> &GetShooter();
    /* Set a float of the object at 0x98 (0x801B6504), and hand it a model clump (0x801B641C)
     * (names and meanings inferred). */
    void SetField98Float(float value);
    void SetClump(void *clump);
    /* The vehicle's weak pointer to itself (name inferred). An inline function: it calls the weak
     * pointer's inline copy constructor, so it stays out of line (weak, 0x800E8CD4). */
    boost::weak_ptr<TVehicleBase> GetSelf() const {
        return m_self;
    }

    /* 0x64 */ u8 field_0x64[0x70 - 0x64];
    /* 0x70 */ u32 m_vehicleFlags; /* bit 0: someone rides it (inferred) */
    /* 0x74 */ u32 field_0x74;
    /* 0x78 */ u32 field_0x78;
    /* 0x7C */ u32 field_0x7C;
    /* 0x80 */ boost::weak_ptr<CharInfo> m_rider;
    /* 0x88 */ boost::weak_ptr<CharInfo> m_shooter;
    /* 0x90 */ boost::weak_ptr<TVehicleBase> m_self;
    /* 0x98 */ void *field_0x98;
};
SIZE_ASSERT(TVehicleBase, 0x9C);

} // namespace Vehicle

#endif
