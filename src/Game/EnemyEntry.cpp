/* The enemies' shared helpers: the object that counts the stage's enemies (UnkObject_801A2AF4,
 * Game/UnkSingletons.h) with the code of its two maps' trees, the stage tests that pick the
 * soldiers' data, the entries EnemyEntryStart and EnemyEntryEnd of the gadget type table (the
 * first loads and drops the enemies' path file), and the functions that launch an effect on the
 * stage's clock. File name inferred from the entries' names; names marked "(name inferred)" are
 * not original. Definitions are in reverse address order (deferred inlining emits functions
 * last-defined first). */
#define UNK_TREE_MAP
#define ENEMY_AREA_PARTS
#include "Game/UnkSingletons.h"
#include "Effect/System/EffectElement.h"
#include "Game/BkLarva.h"
#include "Game/CharInfo.h"
#include "Game/Enemy.h"
#include "Game/Gadget.h"
#include "Game/GadgetClassDesc.h"
#include "Game/GunSoldier.h"
#include "Game/MathTypes.h"
#include "Game/Npc.h"
#include "Game/ResourceOneFile.h"
#include "Game/SetAdapterState.h"
#include "Game/SoldierParam.h"
#include "Game/UnkSwitchBank_800CB69C.h"
#include "boost/shared_ptr.hpp"
#include "boost/weak_ptr.hpp"
#include <string.h>

using Effect::System::EffectElement;

/* The list of placement records and the character manager (class names unknown). */
class UnkObject_800C9B84;
class UnkCharManager_8006F754;

extern "C" {
/* Launch an effect of kind kind (see Game/EffectLauncherTable.cpp). */
boost::weak_ptr<EffectElement> fn_801D2C28(int kind, const Mtx34f *matrix, int id, void *clock);
boost::weak_ptr<EffectElement> fn_801D2BEC(int kind, const Vec3f *position, const Vec3f *rotation, int id,
                                           void *clock);
boost::weak_ptr<EffectElement> fn_801D2B74(int kind, const Mtx34f *matrix, int id, void *clock);
boost::weak_ptr<EffectElement> fn_801D2AFC(int kind, const Vec3f *position, const Vec3f *rotation, int id,
                                           void *clock);
/* The list of placement records, its first record of a switch number (the records of a number
 * are linked at 0x8), and a record's type (behaviour inferred). */
UnkObject_800C9B84 *fn_800C9B84(void);
UnkPlacement_800C9CD0 *fn_800CB560(UnkObject_800C9B84 *list, int id);
u16 fn_800C9D24(UnkPlacement_800C9CD0 *owner);
/* Sets switch id (behaviour and meaning of the last argument inferred). */
void fn_8016907C(UnkSwitchBank_800CB69C *manager, int id, int arg2);
/* The character manager, the partner's character id in it and the character of an id (see
 * Game/SetCharacter.cpp; behaviour inferred). */
UnkCharManager_8006F754 *fn_8006F754(void);
s32 fn_800747E0(UnkCharManager_8006F754 *manager);
boost::weak_ptr<CharInfo> fn_8007488C(UnkCharManager_8006F754 *manager, int character);

/* Defined below. */
void fn_801A2A68(UnkObject_801A2AF4 *counter);
void fn_801A27E4(UnkObject_801A2AF4 *counter);
void fn_801A2674(UnkObject_801A2AF4 *counter);
void fn_801A2574(UnkObject_801A2AF4 *counter, u32 flags, int kind);
void fn_801A22FC(UnkObject_801A2AF4 *counter);
boost::weak_ptr<EffectElement> fn_801A1F54(const Vec3f *position, const Vec3f *rotation, int id);
}

/* The next placement record of the same switch number. */
#define NEXT_RECORD(owner) (*(UnkPlacement_800C9CD0 **)((u8 *)(owner) + 0x8))

/* The kind of an enemy type besides the soldiers and larvae (name inferred). */
inline int EnemyKind(int type) {
    if (type < 120) {
        return 0;
    }
    if (type < 140) {
        return 1;
    }
    return 2;
}

UnkObject_801A2AF4::UnkObject_801A2AF4() {
    fn_801A2A68(this);
}

/* Forgets all enemies (name inferred). */
extern "C" void fn_801A2A68(UnkObject_801A2AF4 *counter) {
    counter->m_mapA.clear();
    counter->m_mapB.clear();
    counter->m_placed = 0;
    memset(counter->m_placedOfKind, 0, sizeof(counter->m_placedOfKind));
    memset(counter->m_destroyed, 0, sizeof(counter->m_destroyed));
    memset(counter->m_savedDestroyed, 0, sizeof(counter->m_savedDestroyed));
    memset(counter->m_destroyedByPlayer, 0, sizeof(counter->m_destroyedByPlayer));
    memset(counter->m_savedDestroyedByPlayer, 0, sizeof(counter->m_savedDestroyedByPlayer));
}

/* Gives enemy a handle: the first one given back, or a new one (name inferred). */
extern "C" u32 fn_801A29B0(UnkObject_801A2AF4 *counter, Enemy *enemy) {
    bool found = false;
    s32 count = counter->m_handles++;
    s32 handle = 0;
    for (handle = 0; handle < count; handle++) {
        if (counter->m_mapA[handle] == 0) {
            counter->m_mapA[handle] = enemy;
            found = true;
            break;
        }
    }
    if (!found) {
        handle = counter->m_handles;
        counter->m_mapA[handle] = enemy;
    }
    return handle;
}

/* Takes handle back (name inferred). */
extern "C" void fn_801A2968(UnkObject_801A2AF4 *counter, u32 handle) {
    counter->m_mapA[handle] = 0;
    counter->m_handles--;
}

/* Counts the enemies placed anew and forgets the handles (name inferred). */
extern "C" void fn_801A2940(UnkObject_801A2AF4 *counter) {
    counter->m_handles = 0;
    fn_801A27E4(counter);
}

/* Counts the enemies placed: in all, by kind and by switch number (name inferred). */
extern "C" void fn_801A27E4(UnkObject_801A2AF4 *counter) {
    fn_801A2A68(counter);
    for (int group = 0; group < 0x100; group++) {
        UnkPlacement_800C9CD0 *owner = fn_800CB560(fn_800C9B84(), group);
        int count = 0;
        for (; owner != 0; owner = NEXT_RECORD(owner)) {
            u16 type = fn_800C9D24(owner);
            if (type >= 100 && type < 150) {
                if (type == 100) {
                    SoldierParam params;
                    fn_800C9DBC(owner, &params, sizeof(params));
                    if (params.AppearType != 5) {
                        count++;
                        counter->m_placedOfKind[0]++;
                    }
                } else if (type == 0x91) {
                    BkLarvaParams params;
                    fn_800C9DBC(owner, &params, sizeof(params));
                    counter->m_placedOfKind[2] += params.Num;
                    count += params.Num;
                } else {
                    int kind = EnemyKind(type);
                    count++;
                    counter->m_placedOfKind[kind]++;
                }
            }
        }
        if (count != 0) {
            counter->m_mapB[group] = count;
            counter->m_placed += count;
        }
    }
}

/* Counts the enemies still about by switch number again, from the state the soldiers and the
 * larva generators keep on their placement records (name inferred). */
extern "C" void fn_801A2674(UnkObject_801A2AF4 *counter) {
    counter->m_mapB.clear();
    for (int group = 0; group < 0x100; group++) {
        UnkPlacement_800C9CD0 *owner = fn_800CB560(fn_800C9B84(), group);
        int count = 0;
        for (; owner != 0; owner = NEXT_RECORD(owner)) {
            u16 type = fn_800C9D24(owner);
            if (type >= 100 && type < 150) {
                if (type == 100) {
                    if (owner->m_adapterState != 0) {
                        GunSoldier::STemporaryData *data =
                            dynamic_cast<GunSoldier::STemporaryData *>(owner->m_adapterState);
                        if (data != 0 && data->m_state != 1) {
                            count++;
                        }
                    } else {
                        SoldierParam params;
                        fn_800C9DBC(owner, &params, sizeof(params));
                        if (params.AppearType != 5) {
                            count++;
                        }
                    }
                } else if (type == 0x91) {
                    if (owner->m_adapterState != 0) {
                        SBkLarvaStateData *data = dynamic_cast<SBkLarvaStateData *>(owner->m_adapterState);
                        if (data != 0) {
                            count += data->m_remaining;
                        }
                    } else {
                        BkLarvaParams params;
                        fn_800C9DBC(owner, &params, sizeof(params));
                        count += params.Num;
                    }
                } else {
                    count++;
                }
            }
        }
        if (count != 0) {
            counter->m_mapB[group] = count;
        }
    }
}

/* Counts an enemy of a kind destroyed; one the player destroyed (flag 4) also counts for the
 * player, and the partner is told (name inferred). */
extern "C" void fn_801A2574(UnkObject_801A2AF4 *counter, u32 flags, int kind) {
    counter->m_destroyed[kind]++;
    if (flags & 4) {
        counter->m_destroyedByPlayer[kind]++;
        UnkCharManager_8006F754 *manager = fn_8006F754();
        boost::weak_ptr<CharInfo> partner = fn_8007488C(manager, fn_800747E0(manager));
        boost::shared_ptr<CharInfo> info = partner.lock();
        if (info.get() != 0) {
            info->PushCommand(
                new Player::Npc::EnemyDeadByPlayerCommand(kind, counter->m_destroyedByPlayer[kind]));
        }
    }
}

/* Forgets all enemies and the handles (name inferred). */
extern "C" void fn_801A2540(UnkObject_801A2AF4 *counter) {
    fn_801A2A68(counter);
    counter->m_handles = 0;
}

/* Counts the enemies placed anew (name inferred). */
extern "C" void fn_801A2520(UnkObject_801A2AF4 *counter) {
    fn_801A27E4(counter);
}

/* Goes back to the counts and states of the last save (name inferred). */
extern "C" void fn_801A24EC(UnkObject_801A2AF4 *counter) {
    fn_801A22FC(counter);
    fn_801A2674(counter);
}

/* Keeps the counts and the soldiers' and larva generators' states for a later restart (name
 * inferred). */
extern "C" void fn_801A23F4(UnkObject_801A2AF4 *counter) {
    for (int i = 0; i < 3; i++) {
        counter->m_savedDestroyed[i] = counter->m_destroyed[i];
        counter->m_savedDestroyedByPlayer[i] = counter->m_destroyedByPlayer[i];
    }
    for (int group = 0; group < 0x100; group++) {
        for (UnkPlacement_800C9CD0 *owner = fn_800CB560(fn_800C9B84(), group); owner != 0;
             owner = NEXT_RECORD(owner)) {
            u16 type = fn_800C9D24(owner);
            SetAdapterState *state = owner->m_adapterState;
            if (state != 0) {
                if (type == 100) {
                    GunSoldier::STemporaryData *data = dynamic_cast<GunSoldier::STemporaryData *>(state);
                    if (data != 0) {
                        data->field_0x8 = data->m_state;
                    }
                } else if (type == 0x91) {
                    SBkLarvaStateData *data = dynamic_cast<SBkLarvaStateData *>(state);
                    if (data != 0) {
                        data->m_total = data->m_remaining;
                    }
                }
            }
        }
    }
}

/* Restores the counts and states fn_801A23F4 kept (name inferred). */
extern "C" void fn_801A22FC(UnkObject_801A2AF4 *counter) {
    for (int i = 0; i < 3; i++) {
        counter->m_destroyed[i] = counter->m_savedDestroyed[i];
        counter->m_destroyedByPlayer[i] = counter->m_savedDestroyedByPlayer[i];
    }
    for (int group = 0; group < 0x100; group++) {
        for (UnkPlacement_800C9CD0 *owner = fn_800CB560(fn_800C9B84(), group); owner != 0;
             owner = NEXT_RECORD(owner)) {
            u16 type = fn_800C9D24(owner);
            SetAdapterState *state = owner->m_adapterState;
            if (state != 0) {
                if (type == 100) {
                    GunSoldier::STemporaryData *data = dynamic_cast<GunSoldier::STemporaryData *>(state);
                    if (data != 0) {
                        data->m_state = data->field_0x8;
                    }
                } else if (type == 0x91) {
                    SBkLarvaStateData *data = dynamic_cast<SBkLarvaStateData *>(state);
                    if (data != 0) {
                        data->m_remaining = data->m_total;
                    }
                }
            }
        }
    }
}

/* Counts the enemy of record owner gone (flag 1: destroyed, with flag 4 if by the player) or back
 * (flag 2), when its switch number has enemies counted; the switch goes on when the last of them
 * is gone (name inferred). */
extern "C" void fn_801A2150(UnkObject_801A2AF4 *counter, UnkPlacement_800C9CD0 *owner, int kind, u32 flags) {
    if (flags != 0) {
        u8 group = 0;
        if (owner != 0) {
            fn_800C9E60(owner, &group);
            UnkMapB_801A2AF4::Tree::Iterator it = counter->m_mapB.find(group);
            if (it != counter->m_mapB.end()) {
                fn_800C9D24(owner);
                if (flags & 2) {
                    counter->m_destroyed[kind]--;
                    counter->m_mapB[group]++;
                } else if (flags & 1) {
                    fn_801A2574(counter, flags, kind);
                    if (--counter->m_mapB[group] <= 0) {
                        fn_8016907C(fn_800CB69C(), group, 1);
                    }
                }
            }
        }
    }
}

/* The number of enemies of a kind destroyed (name inferred). */
extern "C" s32 fn_801A2140(UnkObject_801A2AF4 *counter, int kind) {
    return counter->m_destroyed[kind];
}

/* Which data the soldiers of the stage use (an index into Game/GunSoldier.cpp's stage names;
 * name inferred). */
extern "C" int fn_801A2064(void) {
    switch (*reinterpret_cast<s32 *>(&lbl_8057D6A0[0xA8])) {
    case 500:
    case 503:
        return 0;
    case 401:
    case 504:
        return 1;
    case 100:
    case 202:
    case 400:
        return 2;
    case 404:
        return 4;
    case 501:
    case 600:
    case 800:
    case 801:
        return 6;
    case 201:
        return 7;
    }
    return 0;
}

/* Whether the stage is one of three (300, 402, 403; name and meaning inferred). */
extern "C" bool fn_801A2030(void) {
    s32 stage = *reinterpret_cast<s32 *>(&lbl_8057D6A0[0xA8]);
    if (stage == 300 || stage == 402 || stage == 403) {
        return true;
    }
    return false;
}

/* EnemyEntryStart's hooks: load and drop the enemies' path file. */
extern "C" void fn_801A1FD0(void) {
    void *loader = fn_800122D4();
    fn_8001299C(loader, 6, new ResourceOneFile("enemy/EnemyPath.one", false));
}

extern "C" void fn_801A1FA8(void) {
    fn_800127F8(fn_800122D4(), 6);
}

/* EnemyEntryEnd's hooks (empty). */
extern "C" void fn_801A1FA4(void) {}

extern "C" void fn_801A1FA0(void) {}

/* Launch an effect on the stage's clock (the one at 0x24 of lbl_8057D6A0): at a position with a
 * rotation (fn_801A1F54 and fn_801A1E8C, through two launch functions), at a position unrotated
 * (fn_801A1F1C), or at a matrix (fn_801A1ED8 and fn_801A1E48, through two launch functions; names
 * inferred). */
extern "C" boost::weak_ptr<EffectElement> fn_801A1F54(const Vec3f *position, const Vec3f *rotation, int id) {
    return fn_801D2AFC(0, position, rotation, id, &lbl_8057D6A0[0x24]);
}

extern "C" boost::weak_ptr<EffectElement> fn_801A1F1C(const Vec3f *position, int id) {
    return fn_801A1F54(position, &lbl_805E07A0, id);
}

extern "C" boost::weak_ptr<EffectElement> fn_801A1ED8(const Mtx34f *matrix, int id) {
    return fn_801D2B74(0, matrix, id, &lbl_8057D6A0[0x24]);
}

extern "C" boost::weak_ptr<EffectElement> fn_801A1E8C(const Vec3f *position, const Vec3f *rotation, int id) {
    return fn_801D2BEC(0, position, rotation, id, &lbl_8057D6A0[0x24]);
}

extern "C" boost::weak_ptr<EffectElement> fn_801A1E48(const Mtx34f *matrix, int id) {
    return fn_801D2C28(0, matrix, id, &lbl_8057D6A0[0x24]);
}

GadgetClassDesc lbl_8053A9A8 = {
    "EnemyEntryStart", fn_801A1FD0, fn_801A1FA8, 0, 0, 0x80, 0x258E, 2, 0, 0, 0,
};

GadgetClassDesc lbl_8053A9CC = {
    "EnemyEntryEnd", fn_801A1FA4, fn_801A1FA0, 0, 0, 0x80, 0x258E, 2, 0, 0, 0,
};

#include "Game/UnkTreeMap.h"
