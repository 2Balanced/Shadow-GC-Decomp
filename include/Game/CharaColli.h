#ifndef GAME_CHARACOLLI_H
#define GAME_CHARACOLLI_H

#include "Game/MathTypes.h"
#include "dolphin/types.h"

class CharInfo;
struct UnkColor_8040D2D8;
namespace SonicteamUSA {
namespace System {
namespace Range {
class RangeBase;
} // namespace Range
} // namespace System
} // namespace SonicteamUSA
struct UnkColliQuery_800C0D20;
struct UnkColliContact_8001FF00;
struct UnkColliHit_8002686C;
struct UnkDebugLine_80020A20;
struct UnkSphere_8004E6CC;

namespace boost {
template <class T> class weak_ptr;
} // namespace boost

/* Retail's vtable of CharaColliBaseCharInfo (Game/CharaColliBaseCharInfo.cpp, which defines
 * CHARACOLLI_BASE_PURE) has null entries for the virtuals of CharaColliBase it does not override:
 * they are pure there. Other units keep the plain declarations, so that they are unchanged. */
/* The type vf_0x20 returns: Game/CharaColliBase.cpp, whose copy of the empty extent goes through
 * the out-of-line vector copy (fn_800091EC), defines it as Fn8040CopyVector3 (same layout). */
#ifndef CHARACOLLI_EXTENT
#define CHARACOLLI_EXTENT Vec3f
#endif

#ifdef CHARACOLLI_BASE_PURE
#define CHARACOLLI_PURE = 0
#else
#define CHARACOLLI_PURE
#endif

namespace SonicteamUSA {
namespace System {
namespace CharaColli {

/* CharaColliBase (class name from RTTI): the base class of the character collision objects
 * (CharaColliBody, CharaColliGeneral, ...). Gadgets reach the colliders of their models through
 * shared and weak pointers to this class. Only the virtual functions gadgets call are named (names
 * and argument types inferred); the other slots keep the vtable layout. */
class CharaColliBase {
  public:
    /* A filter a query calls for each collider before it tests it; false skips the collider. */
    typedef bool (*Filter)(UnkColliQuery_800C0D20 *query);

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

#ifdef CHARACOLLI_BASE_TYPE_INLINE
    /* The halfword at 0x4, and whether it is 4 or 5 (names inferred; meanings unknown). Inline
     * functions in the game, each of which calls another, so that IsType4Or5 calls GetType out of
     * line and is called out of line itself; Game/PlayerBehaviorLightDash.cpp, which defines
     * CHARACOLLI_BASE_TYPE_INLINE, emits their out-of-line copies. */
    u16 Halfword(int offset) const {
        return *reinterpret_cast<const u16 *>(reinterpret_cast<const u8 *>(this) + offset);
    }
    u16 GetType() const {
        return Halfword(0x4);
    }
    bool IsType4Or5() const {
        int type = GetType();
        return type == 4 || type == 5;
    }
#endif

    /* The number of shapes in the collider (meaning inferred; slot 0x08). */
    virtual u16 GetShapeCount() CHARACOLLI_PURE;
    /* Turns the collider back on and off (names inferred from RegularRing; id: one shape's id, or -1
     * for the whole collider). */
    virtual void TurnOn(u32 id);
    virtual void TurnOff(u32 id);
    /* The collider's position and rotation (slots 0x14 and 0x18; names inferred). */
    virtual Vec3f *vf_0x14() CHARACOLLI_PURE;
    virtual Vec3f *vf_0x18() CHARACOLLI_PURE;
    /* The size of shape id (-1: the first) through the three pointers, and the shape's
     * priority, -1 when there is no such shape (names inferred). */
    virtual s32 vf_0x1C(float *x, float *y, float *z, u32 id);
    /* The extent of shape id (-1: the first; meaning inferred). */
    virtual CHARACOLLI_EXTENT vf_0x20(u32 id);
    virtual s32 vf_0x24();
    /* The shape at index, and the shape with the given id (0 if none; names inferred). */
    virtual SonicteamUSA::System::Range::RangeBase *vf_0x28(int index) const CHARACOLLI_PURE;
    virtual SonicteamUSA::System::Range::RangeBase *vf_0x2C(u32 id) const CHARACOLLI_PURE;
    /* Returns the collider's attributes (the counterpart of SetAttributes; meaning of arg
     * unknown). */
    virtual u32 GetAttributes(int arg) const CHARACOLLI_PURE;
    /* The attributes of shape id (0 if none). */
    virtual u32 vf_0x34(u32 id) const CHARACOLLI_PURE;
    /* The radius of a sphere around the collider (meaning inferred). */
    virtual float vf_0x38();
    /* Attaches the collision to a character info. */
    virtual void SetCharInfo(CharInfo *info) CHARACOLLI_PURE;
    /* Sets the collision's rotation. */
    virtual void SetRotation(const Vec3f *rotation) CHARACOLLI_PURE;
    /* Sets the collision's scale (name and argument inferred from CityGadget). */
    virtual void SetScale(const Vec3f *scale) CHARACOLLI_PURE;
    /* Sets the size of a box collider, and its offset from the character info (meaning of arg
     * unknown). */
    virtual void SetSize(float x, float y, float z, int arg);
    virtual void SetOffset(const Vec3f *offset, int arg);
    /* Sets the collider's colour in the debug view (name inferred; Game/LightColli.cpp). */
    virtual void SetDebugColor(const UnkColor_8040D2D8 *color) CHARACOLLI_PURE;
    /* Makes owner the owner of the shapes (meaning inferred). */
    virtual void vf_0x54(const boost::weak_ptr<CharaColliBase> &owner) CHARACOLLI_PURE;
    /* Sets the collider's attributes (meaning of the arguments unknown). */
    virtual void SetAttributes(u32 attributes, int arg1) CHARACOLLI_PURE;
    /* Sets the attributes of shape id. */
    virtual void vf_0x5C(u32 attributes, u32 id) CHARACOLLI_PURE;
    /* Tests a line, a segment, a sphere and a moving sphere against the collider; filter (when
     * set) is asked first with arg. They return the shape hit (names and meanings inferred). */
    virtual SonicteamUSA::System::Range::RangeBase *vf_0x60(const UnkDebugLine_80020A20 *line, UnkColliContact_8001FF00 *result,
                                             Filter filter, u32 arg) CHARACOLLI_PURE;
    virtual SonicteamUSA::System::Range::RangeBase *vf_0x64(const UnkDebugLine_80020A20 *segment, UnkColliContact_8001FF00 *result,
                                             Filter filter, u32 arg) CHARACOLLI_PURE;
    virtual SonicteamUSA::System::Range::RangeBase *vf_0x68(const UnkSphere_8004E6CC *sphere, UnkColliContact_8001FF00 *result,
                                             Filter filter, u32 arg) CHARACOLLI_PURE;
    virtual SonicteamUSA::System::Range::RangeBase *vf_0x6C(const UnkSphere_8004E6CC *sphere, const Vec3f *move,
                                             UnkColliContact_8001FF00 *result, Filter filter, u32 arg) CHARACOLLI_PURE;
    /* Re-registers the collision after its owner moved. */
    virtual void Reset() CHARACOLLI_PURE;
    /* Draws the collider for debugging (meaning inferred). */
    virtual void vf_0x74() CHARACOLLI_PURE;
    virtual ~CharaColliBase();
    virtual void vf_0x7C() CHARACOLLI_PURE;
    /* Tests the collider against other; on a hit it returns the two shapes and, when asked,
     * the contacts on each side (meanings inferred). */
    virtual bool vf_0x80(CharaColliBase *other, SonicteamUSA::System::Range::RangeBase **mine, SonicteamUSA::System::Range::RangeBase **theirs,
                         UnkColliHit_8002686C **mineHit, UnkColliHit_8002686C **theirHit) CHARACOLLI_PURE;
    /* Recomputes the radius (meaning inferred). */
    virtual void vf_0x84() CHARACOLLI_PURE;

#ifdef CHARACOLLI_BASE_MEMBERS
    /* The members the colliders' own units use (CharaColliBody.cpp and the units after it); the
     * constructor (0x8003797C) and destructor (0x80037920) are out of line. Meanings inferred. */
    CharaColliBase(u16 type, u16 arg);

    /* 0x04 */ u16 m_type;
    /* 0x06 */ u16 field_0x6;
    /* 0x08 */ float m_radius;
    /* 0x0C */ u8 field_0xC[0x28 - 0xC];
    /* 0x28 */ u8 m_moved;
    /* 0x29 */ u8 m_changed;
    /* 0x2A */ u8 field_0x2A;
    /* 0x2B */ u8 field_0x2B;
    /* 0x2C */ u8 field_0x2C;
#endif
};

} // namespace CharaColli
} // namespace System
} // namespace SonicteamUSA

#endif
