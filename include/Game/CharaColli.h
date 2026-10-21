#ifndef GAME_CHARACOLLI_H
#define GAME_CHARACOLLI_H

#include "Game/MathTypes.h"
#include "dolphin/types.h"

class CharInfo;

namespace SonicteamUSA {
namespace System {
namespace CharaColli {

/* CharaColliBase (class name from RTTI): the base class of the character collision objects
 * (CharaColliBody, CharaColliGeneral, ...). Gadgets reach the colliders of their models through
 * shared and weak pointers to this class. Only the virtual functions gadgets call are named (names
 * and argument types inferred); the other slots keep the vtable layout. */
class CharaColliBase {
  public:
    /* Sets the byte at 0x2C (meaning unknown; EggLastMech sets it to 1 on its body collider). An
     * inline function in the original: retail calls the shared_ptr's operator-> out of line
     * before it. */
    void SetField2C(u8 value) {
        reinterpret_cast<u8 *>(this)[0x2C] = value;
    }

    /* The byte at 0x2A (meaning unknown; Game/WeaponTorch.cpp turns its attack collider on when
     * it is clear). An inline function in the original: retail calls the shared_ptr's
     * operator-> out of line before it. */
    u8 GetField2A() {
        return reinterpret_cast<u8 *>(this)[0x2A];
    }

    /* The number of shapes in the collider (meaning inferred; slot 0x08). */
    virtual u16 GetShapeCount();
    /* Turns the collider back on and off (names inferred from RegularRing; arg is -1 there). */
    virtual void TurnOn(int arg);
    virtual void TurnOff(int arg);
    virtual void vf_0x14();
    virtual void vf_0x18();
    virtual void vf_0x1C();
    virtual void vf_0x20();
    virtual void vf_0x24();
    virtual void vf_0x28();
    virtual void vf_0x2C();
    /* Returns the collider's attributes (the counterpart of SetAttributes; meaning of arg
     * unknown). */
    virtual u32 GetAttributes(int arg);
    virtual void vf_0x34();
    virtual void vf_0x38();
    /* Attaches the collision to a character info. */
    virtual void SetCharInfo(CharInfo *info);
    /* Sets the collision's rotation. */
    virtual void SetRotation(const Vec3f *rotation);
    /* Sets the collision's scale (name and argument inferred from CityGadget). */
    virtual void SetScale(const Vec3f *scale);
    /* Sets the size of a box collider, and its offset from the character info (meaning of arg
     * unknown). */
    virtual void SetSize(float x, float y, float z, int arg);
    virtual void SetOffset(const Vec3f *offset, int arg);
    virtual void vf_0x50();
    virtual void vf_0x54();
    /* Sets the collider's attributes (meaning of the arguments unknown). */
    virtual void SetAttributes(u32 attributes, int arg1);
    virtual void vf_0x5C();
    virtual void vf_0x60();
    virtual void vf_0x64();
    virtual void vf_0x68();
    virtual void vf_0x6C();
    /* Re-registers the collision after its owner moved. */
    virtual void Reset();
};

} // namespace CharaColli
} // namespace System
} // namespace SonicteamUSA

#endif
