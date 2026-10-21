#ifndef GAME_EFFECTPJSLAUNCHER_H
#define GAME_EFFECTPJSLAUNCHER_H

#include <dolphin/types.h>
#include "size_assert.h"

class ResourceOneFile;

namespace Effect {
namespace System {

/* EffectLaunchInfo (class name from RTTI; vtable at 0x8051C7E8 with only the destructor,
 * 0x800274A4): the base of the effect systems' launch descriptions. The destructors here are
 * inline: the units that use them then emit these classes' vtables and RTTI as weak copies, which
 * puts the RTTI records where Game/EffectPJS.cpp has them in retail. */
class EffectLaunchInfo {
  public:
    virtual ~EffectLaunchInfo() {}
};

} // namespace System
} // namespace Effect

namespace SonicteamUSA {
namespace System {
namespace Effect {

/* EffectLaunchInfoUSA (class name from RTTI; vtable at 0x8051C7DC): the constructor stores its two
 * arguments at 0x4 and 0x8. The constructor and destructor are inline, but call the inline base
 * members, so they are not inlined: their out-of-line copies are at 0x8002759C and 0x800275C0.
 * Member names and meanings are inferred. */
class EffectLaunchInfoUSA : public ::Effect::System::EffectLaunchInfo {
  public:
    EffectLaunchInfoUSA(void *data, ResourceOneFile *file) : m_data(data), m_file(file) {}
    virtual ~EffectLaunchInfoUSA() {}

    /* 0x4 */ void *m_data; /* set once the resource is loaded (see Game/EffectPJS.cpp) */
    /* 0x8 */ ResourceOneFile *m_file;
};
SIZE_ASSERT(EffectLaunchInfoUSA, 0xC);

} // namespace Effect
} // namespace System
} // namespace SonicteamUSA

namespace Effect {
namespace PJS {

/* Launcher (class name from RTTI; vtable at 0x8055AFF8): the launch description of a PJS effect
 * file, loaded through resource slot m_slot (see Game/EffectPJS.cpp). Member names are inferred. */
class Launcher : public SonicteamUSA::System::Effect::EffectLaunchInfoUSA {
  public:
    Launcher(s32 slot, ResourceOneFile *file);
    virtual ~Launcher();

    /* Whether the effect data has been loaded (name inferred). */
    bool IsLoaded() const;

    /* 0x0C */ s32 m_slot;
    /* 0x10 */ u32 field_0x10;
};
SIZE_ASSERT(Launcher, 0x14);

} // namespace PJS
} // namespace Effect

#endif
