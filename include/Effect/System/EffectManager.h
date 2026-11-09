#ifndef EFFECT_SYSTEM_EFFECTMANAGER_H
#define EFFECT_SYSTEM_EFFECTMANAGER_H

#include <dolphin/types.h>
#include "boost/weak_ptr.hpp"
#include "size_assert.h"

struct RpClump;

/* The effect managers (class names and bases from RTTI): Effect::System::EffectManager, its
 * RenderWare implementation Effect::RW3::EffectManagerRW3 and the game's
 * SonicteamUSA::System::Effect::EffectManagerUSA. Only what other classes use is declared; the
 * classes are polymorphic (vtable pointer at 0), so they can be the target of a dynamic_cast. */
namespace Effect {
namespace System {

class EffectElement;
class EffectLaunchInfo;
struct UnkEffectDesc_801D0AFC;
struct UnkEffectData_80425860;

/* The manager's own members and functions are in Game/EffectManager.cpp, which defines
 * EFFECT_MANAGER_FULL; elsewhere they stand as padding (names inferred). */
class EffectManager {
  public:
    EffectManager();
    virtual ~EffectManager();
    /* Launches effect id of the effect file info was loaded with (0x80425630; names inferred),
     * and the same through a shared pointer to the launch description (0x804255F4; the
     * parameter's type is inferred from its use, which only reads the pointer). */
    virtual boost::weak_ptr<EffectElement> Launch(EffectLaunchInfo *info, int id);
    virtual boost::weak_ptr<EffectElement> Launch(const boost::shared_ptr<EffectLaunchInfo> &info,
                                                  int id);
    /* Creates the element desc describes, launched by info (name inferred; the PJS manager's
     * version is in Game/EffectManagerPJS.cpp). */
    virtual EffectElement *CreateElement(const UnkEffectDesc_801D0AFC *desc,
                                         EffectLaunchInfo *info);

#ifdef EFFECT_MANAGER_FULL
    /* Unlinks an element from both lists (name inferred). Drawing, updating and clearing the
     * lists are the extern "C" fn_80425170, fn_80425294 and fn_8042554C, the names the game's
     * units call them by. */
    void Remove(boost::shared_ptr<EffectElement> element);
    /* Checks an effect file ("EFFD") and turns its offsets into pointers once (name
     * inferred). */
    bool Relocate(UnkEffectData_80425860 *data);

    /* 0x04 */ boost::shared_ptr<EffectElement> m_head; /* all elements, linked through m_next */
    /* 0x0C */ boost::shared_ptr<EffectElement> m_tail;
    /* 0x14 */ boost::shared_ptr<EffectElement> m_drawHead; /* linked through m_drawNext */
    /* 0x1C */ boost::shared_ptr<EffectElement> m_drawTail;
    /* 0x24 */ bool m_drawHidden; /* Draw marks the elements it draws hidden while drawing */
    /* 0x25 */ bool m_paused;     /* Update only drops finished elements */
    /* 0x28 */ int m_error;       /* why the last Launch failed */
    /* 0x2C */ u8 field_0x2C[0x58 - 0x2C]; /* split between the three manager classes unknown */
#else
    /* 0x04 */ u8 field_0x4[0x58 - 0x4]; /* split between the three manager classes unknown */
#endif
};

} // namespace System

namespace RW3 {

class EffectManagerRW3 : public ::Effect::System::EffectManager {
  public:
    /* 0x804329C4 (the PJS manager's constructor calls it; Game/PlayerEffect.cpp). */
    EffectManagerRW3();
    virtual ~EffectManagerRW3();
    virtual ::Effect::System::EffectElement *CreateElement(
        const ::Effect::System::UnkEffectDesc_801D0AFC *desc,
        ::Effect::System::EffectLaunchInfo *info);
    /* A virtual function the RenderWare manager adds (purpose unknown; 0x80437C14, and
     * 0x80028BA4 in EffectManagerUSA): it draws clump with the manager's effects of the given
     * number of passes (arguments inferred from the calls). */
    virtual void vf18(RpClump *clump, void *arg2, int count, void *arg4);
};

} // namespace RW3
} // namespace Effect

namespace SonicteamUSA {
namespace System {
namespace Effect {

class EffectManagerUSA : public ::Effect::RW3::EffectManagerRW3 {
  public:
    /* Inline, as is the destructor: the PJS manager's constructor (0x800A32E4) and destructor
     * (0x800A327C) inline them. */
    EffectManagerUSA() {}
    virtual ~EffectManagerUSA() {}
    /* Declared before CreateElement: the vtable is emitted with the first non-inline virtual
     * function, and retail has it with vf18 (0x80028BA4, in the unit after Game/EffectRayUSA.cpp),
     * not with CreateElement (Game/EffectManagerUSA.cpp). */
    virtual void vf18(RpClump *clump, void *arg2, int count, void *arg4);
    virtual ::Effect::System::EffectElement *CreateElement(
        const ::Effect::System::UnkEffectDesc_801D0AFC *desc,
        ::Effect::System::EffectLaunchInfo *info);
};
SIZE_ASSERT(EffectManagerUSA, 0x58);

} // namespace Effect
} // namespace System
} // namespace SonicteamUSA

/* Four colour bytes (meaning inferred). */
struct UnkFogColor_8002893C {
    /* 0x0 */ u8 r;
    /* 0x1 */ u8 g;
    /* 0x2 */ u8 b;
    /* 0x3 */ u8 a;
};
SIZE_ASSERT(UnkFogColor_8002893C, 0x4);

/* A fog setting of EffectManagerUSA (meaning inferred from its use by EffectGlobalParamUSA, which
 * blends its values): on/off, a second switch, a distance-like integer and a colour. The
 * constructor (fn_8002893C) clears both switches and sets 8 and a default colour. */
struct UnkFogParam_8002893C {
#ifdef UNK_FOG_PARAM_CONSTRUCTOR
    /* Defined in the class only in Game/EffectSystemUSA.cpp, where GC/2.7 emits it after its first
     * caller (fn_800288D8) and pools its colour constant first, as retail has them; elsewhere a
     * declaration (the body would renumber every unit's @N locals). */
    UnkFogParam_8002893C() {
        enabled = false;
        field_0x1 = false;
        field_0x4 = 8;
        UnkFogColor_8002893C white = {0xFF, 0xFF, 0xFF, 0xFF};
        color = white;
    }
#else
    UnkFogParam_8002893C();
#endif

    /* 0x0 */ bool enabled;
    /* 0x1 */ bool field_0x1;
    /* 0x4 */ int field_0x4;
    /* 0x8 */ UnkFogColor_8002893C color;
};
SIZE_ASSERT(UnkFogParam_8002893C, 0xC);

extern "C" {
/* Returns the manager's current fog setting, and sets it (behaviour inferred). */
UnkFogParam_8002893C fn_800288D8(SonicteamUSA::System::Effect::EffectManagerUSA *manager);
void fn_80028984(SonicteamUSA::System::Effect::EffectManagerUSA *manager,
                 const UnkFogParam_8002893C *param);
}

#endif
