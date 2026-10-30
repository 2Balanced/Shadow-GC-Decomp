#ifndef GAME_CHARACOLLIBASECHARINFO_H
#define GAME_CHARACOLLIBASECHARINFO_H

#include "Game/CharaColli.h"
#ifdef CHARACOLLI_BASE_MEMBERS
#include "boost/shared_ptr.hpp"
#include "boost/weak_ptr.hpp"
#endif

namespace SonicteamUSA {
namespace System {
namespace CharaColli {

/* CharaColliBaseCharInfo (class name from RTTI, CharaColliBase its only base): a collider attached
 * to a character info (fn_800C4374 casts the colliders it is given to it). */
#ifdef CHARACOLLI_BASE_MEMBERS
class CharaColliAttack;

/* The class as the colliders' own units see it (CharaColliBody.cpp and the units after it): the
 * character it follows and its attack collision. Its constructor (0x800C0720) and destructor
 * (0x800C06A0) are out of line. Names of the new virtuals inferred. */
class CharaColliBaseCharInfo : public CharaColliBase {
  public:
    CharaColliBaseCharInfo(u16 type, u16 arg, boost::weak_ptr<CharInfo> info);
    virtual ~CharaColliBaseCharInfo();
    /* Takes the character's position over (slot 0x88), and applies a pending move (0x8C). */
    virtual void vf_0x88() = 0;
    virtual void vf_0x8C() = 0;

    /* 0x30 */ boost::weak_ptr<CharInfo> m_info;
    /* 0x38 */ boost::shared_ptr<CharaColliAttack> m_attack;
};
#else
class CharaColliBaseCharInfo : public CharaColliBase {};
#endif

/* Most game units emit the RTTI records of CharaColliBaseCharInfo and of its base, which only a
 * dynamic_cast or typeid in inline code of the original headers explains. This cast reproduces
 * those records (the original's inline code is unknown; nothing calls it). The records are local:
 * the linker keeps them only where a unit's .data is addressed through one base and so is kept
 * whole, where their base list leads the section. */
inline CharaColliBaseCharInfo *AsCharInfoColli(CharaColliBase *colli) {
    return dynamic_cast<CharaColliBaseCharInfo *>(colli);
}

} // namespace CharaColli
} // namespace System
} // namespace SonicteamUSA

#endif
