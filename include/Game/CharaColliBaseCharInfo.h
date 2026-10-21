#ifndef GAME_CHARACOLLIBASECHARINFO_H
#define GAME_CHARACOLLIBASECHARINFO_H

#include "Game/CharaColli.h"

namespace SonicteamUSA {
namespace System {
namespace CharaColli {

/* CharaColliBaseCharInfo (class name from RTTI, CharaColliBase its only base): a collider attached
 * to a character info (fn_800C4374 casts the colliders it is given to it). */
class CharaColliBaseCharInfo : public CharaColliBase {};

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
