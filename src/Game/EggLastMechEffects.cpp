/* The sound and effect helper of EggLastMech (class name unknown; EggLastMech::UnkSound_80336F48
 * in Game/EggLastMech.h, which keeps its layout opaque): it plays the boss's sounds, and keeps the
 * ten effects that run on the boss's bones. Each group of effects (0: bones 0-3, 1: bone 4,
 * 2: bones 5-6, 3: bones 7-8, 5: bone 9; 4 shares group 0's bones) is started with the effect of
 * its kind (fn_80336DF4; an effect that already runs is kept) and stopped (fn_80336D38). The other
 * units call these functions by their addresses. File and function names are inferred.
 * Definitions are in reverse address order (deferred inlining emits functions last-defined
 * first). */
#include "Effect/System/EffectElement.h"
#include "Game/GadgetSound.h"
#include "boost/shared_ptr.hpp"
#include "boost/weak_ptr.hpp"
#include <dolphin/types.h>
#include "size_assert.h"

using Effect::System::EffectElement;

/* The definition an effect was started from (struct name unknown): its id at 0x24. */
struct UnkEffectDef_80336C48 {
    /* 0x00 */ u8 field_0x0[0x24];
    /* 0x24 */ s32 m_id;
};

/* The effect data a character info keeps at 0x60 (see Game/EggLastMechSlotSwitch.cpp). */
struct UnkCharData_60 {
    /* 0x00 */ u32 field_0x0;
    /* 0x04 */ u32 field_0x4;
};

namespace EggLastMech {

/* EggLastMech, as far as this unit uses it (see Game/EggLastMech.h): its effect data at 0x60, and
 * the bone a node id names (EggLastMech::SetMotion returns fn_801A46E4's result). */
class EggLastMech {
  public:
    void *SetMotion(int node);

    /* 0x00 */ u8 field_0x0[0x60];
    /* 0x60 */ UnkCharData_60 *m_effectData;
};

/* The effect data at 0x60 (name inferred; as in Game/EggLastMechSlotSwitch.cpp). */
inline UnkCharData_60 *GetEffectData(EggLastMech *mech) {
    return mech->m_effectData;
}

class UnkSound_80336F48 {
  public:
    UnkSound_80336F48(EggLastMech *mech);
    ~UnkSound_80336F48();

    /* 0x00 */ EggLastMech *m_mech;
    /* 0x04 */ boost::weak_ptr<EffectElement> m_effects[10];
};
SIZE_ASSERT(UnkSound_80336F48, 0x54);

} // namespace EggLastMech

using EggLastMech::UnkSound_80336F48;

/* The effect of each group: its id and kind (values from .rodata). */
struct UnkEffectEntry_80336C48 {
    s32 m_id;
    s32 m_kind;
};

static const UnkEffectEntry_80336C48 s_effects[6] = {
    {0x0D, 3}, {0x10, 3}, {0x98, 3}, {0x14, 3}, {0x17, 3}, {0x228, 0},
};

/* The bone of each effect (values from .rodata). */
static const s32 s_bones[10] = {0x1394, 0x139E, 0x13A8, 0x13B2, 0x1388, 0x13CE, 0x13CF, 0x13CE, 0x13CF, 0x1389};

/* The sound manager (see Game/GadgetSound.h and Game/EggLastMechSlotSwitch.cpp). */
struct UnkSoundManager_80053164;

extern "C" {
UnkSoundManager_80053164 *fn_80053164(void);
void fn_801D54F4(UnkSoundManager_80053164 *manager, u16 id, const EggLastMech::EggLastMech *source, int arg3, int arg4);
void fn_801D560C(UnkSoundManager_80053164 *manager, u16 id, int arg2, int arg3);
UnkSoundHandle_801D3DC8 fn_801D3C38(int id, EggLastMech::EggLastMech *source, void *owner);
/* Starts effect id of the kind on node (see Game/EggLastMechSlotSwitch.cpp). */
boost::weak_ptr<EffectElement> fn_801D2C28(int kind, void *node, int id, void *arg3);

void fn_80336B9C(UnkSound_80336F48 *sound, int index);
void fn_80336C48(UnkSound_80336F48 *sound, int index, int group);
}

EggLastMech::UnkSound_80336F48::UnkSound_80336F48(EggLastMech *mech) : m_mech(mech) {}

EggLastMech::UnkSound_80336F48::~UnkSound_80336F48() {
    for (int i = 0; i < 10; i++) {
        fn_80336B9C(this, i);
    }
}

extern "C" {

/* Starts the effects of group (name inferred). */
void fn_80336DF4(UnkSound_80336F48 *sound, int group) {
    switch (group) {
    case 0:
    case 4:
        for (int i = 0; i <= 3; i++) {
            fn_80336C48(sound, i, group);
        }
        break;
    case 2:
        fn_80336C48(sound, 5, group);
        fn_80336C48(sound, 6, group);
        break;
    case 3:
        fn_80336C48(sound, 7, group);
        fn_80336C48(sound, 8, group);
        break;
    case 1:
        fn_80336C48(sound, 4, group);
        break;
    case 5:
        fn_80336C48(sound, 9, group);
        break;
    }
}

/* Stops the effects of group (name inferred). */
void fn_80336D38(UnkSound_80336F48 *sound, int group) {
    switch (group) {
    case 0:
    case 4:
        for (int i = 0; i <= 3; i++) {
            fn_80336B9C(sound, i);
        }
        break;
    case 2:
        fn_80336B9C(sound, 5);
        fn_80336B9C(sound, 6);
        break;
    case 3:
        fn_80336B9C(sound, 7);
        fn_80336B9C(sound, 8);
        break;
    case 1:
        fn_80336B9C(sound, 4);
        break;
    case 5:
        fn_80336B9C(sound, 9);
        break;
    }
}

/* Starts the effect of group on the bone of index, unless that effect already runs there (name
 * inferred). */
void fn_80336C48(UnkSound_80336F48 *sound, int index, int group) {
    const UnkEffectEntry_80336C48 &entry = s_effects[group];
    boost::shared_ptr<EffectElement> effect = sound->m_effects[index].lock();
    if (!effect || reinterpret_cast<UnkEffectDef_80336C48 **>(effect.get())[0x44 / 4]->m_id != entry.m_id) {
        fn_80336B9C(sound, index);
        sound->m_effects[index] = fn_801D2C28(entry.m_kind, sound->m_mech->SetMotion(s_bones[index]),
                                              entry.m_id, &GetEffectData(sound->m_mech)->field_0x4);
    }
}

/* Stops the effect on the bone of index (name inferred). */
void fn_80336B9C(UnkSound_80336F48 *sound, int index) {
    boost::weak_ptr<EffectElement> &slot = sound->m_effects[index];
    if (!slot.expired()) {
        boost::shared_ptr<EffectElement> effect = slot.lock();
        effect->SetMatrix(0);
        effect->Stop();
        slot.reset();
    }
}

/* Plays sound id at the boss (name inferred). */
void fn_80336B58(UnkSound_80336F48 *sound, int id) {
    fn_801D54F4(fn_80053164(), id, sound->m_mech, 0, 1);
}

/* Plays sound id (name inferred). */
void fn_80336B1C(UnkSound_80336F48 *sound, int id) {
    fn_801D560C(fn_80053164(), id, 0, 0);
}

/* Starts sound id at the boss and returns its handle (name inferred). */
UnkSoundHandle_801D3DC8 fn_80336AE4(UnkSound_80336F48 *sound, int id) {
    return fn_801D3C38(id, sound->m_mech, sound);
}
}
