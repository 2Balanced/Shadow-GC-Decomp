/* The VehicleDataStart and VehicleDataEnd gadget types (names from their records), whose load and
 * unload hooks set up and release the data the vehicles share (the common/vehicleResource.one file
 * in loader slot 3, and the vehicle table's state), and two helpers the vehicles call: what a
 * ridden vehicle's collider touches is used (a UseCommand from the vehicle to each touched
 * character of kind 0x34 that takes it), and a vehicle's attack collision. File name inferred from
 * the records, as Game/GadgetCommonData.cpp's; the functions keep their placeholder names because
 * other units call them by those names. Behaviour is inferred. Definitions are in reverse address
 * order (deferred inlining emits functions last-defined first). */
#include <boost/function.hpp>

#include "Game/CharCommand.h"
#include "Game/CharInfo.h"
#include "Game/CharaColli.h"
#include "Game/CharaColliAttack.h"
#include "Game/GadgetClassDesc.h"
#include "Game/ResourceOneFile.h"
#include "Game/ResourceOneFileReq.h"
#include "Game/TVehicleBase.h"
#include "boost/shared_ptr.hpp"
#include "boost/weak_ptr.hpp"
#include <dolphin/types.h>

using SonicteamUSA::System::CharaColli::AttackCallback;
using SonicteamUSA::System::CharaColli::CharaColliAttack;
using SonicteamUSA::System::CharaColli::CharaColliBase;
using Vehicle::TVehicleBase;

/* A node of a collider's contact list (see Game/BirdBase.cpp). */
struct UnkColliContactNode_800587A0 {
    /* 0x00 */ u8 field_0x0[0x14];
    /* 0x14 */ void *m_object;
};

struct UnkColliContactIter_8001EDFC {
    UnkColliContactIter_8001EDFC(UnkColliContactNode_800587A0 *node) : m_node(node) {}

    UnkColliContactNode_800587A0 *m_node;
};

struct UnkColliContacts_8001EE04;

extern "C" {
/* The vehicles' shared data's managers (see Game/GadgetCommonData.cpp), the loader and its slots
 * (see Game/BkGiant.cpp), and the vehicle table's load and reset (Game/VehicleTable.cpp). */
void fn_8006B5A0(void);
void fn_801B48B4(void);
void fn_801B486C(void);

/* The collider manager and the character a collider belongs to, a contact's collider, and the
 * contact list (see Game/BirdBase.cpp). */
void *fn_80021B08(void);
boost::weak_ptr<CharInfo> fn_800C4568(void *manager, boost::weak_ptr<CharaColliBase> collider);
boost::weak_ptr<CharaColliBase> fn_800587A0(UnkColliContactNode_800587A0 *node);
UnkColliContactNode_800587A0 *fn_8001EE04(UnkColliContacts_8001EE04 *contacts);
UnkColliContactNode_800587A0 *fn_8001EDEC(UnkColliContacts_8001EE04 *contacts);
UnkColliContactIter_8001EDFC fn_80027374(UnkColliContactIter_8001EDFC *it, int);

/* Whether a character may use things (see Game/PlayerHelpers.cpp), and whether a weapon type is
 * one (see Game/WeaponTable.cpp; behaviour inferred). */
bool fn_800B7694(const boost::shared_ptr<CharInfo> &character);
bool fn_8005B81C(int type);

/* Sets up an attack collision (see Game/BulletUtil.cpp). */
void fn_80054DD0(CharaColliAttack *attack, int kind, u32 flags, float power, AttackCallback onHit,
                 AttackCallback onEnd);
}

/* What the vehicle data's file request is handed (see Game/GadgetCommonData.cpp). */
extern u8 lbl_80575EE4[];

/* Called by the vehicle table (Game/VehicleTable.cpp): empty, and false (meanings unknown). */
extern "C" void fn_801C0ADC(void) {}

extern "C" bool fn_801C0AD4(void) {
    return false;
}

/* A vehicle's attack collision: kind 3, flags 0x21, no power, with the callbacks, for owner. */
extern "C" boost::shared_ptr<CharaColliAttack> fn_801C08F0(const boost::weak_ptr<CharInfo> &owner,
                                                           const AttackCallback &onHit,
                                                           const AttackCallback &onEnd) {
    boost::shared_ptr<CharaColliAttack> attack = boost::shared_ptr<CharaColliAttack>(new CharaColliAttack());
    if (attack.get() != 0) {
        fn_80054DD0(attack.get(), 3, 0x21, 0.0f, onHit, onEnd);
        attack->m_owner = owner;
    }
    return attack;
}

/* Sends a UseCommand from the vehicle to each character of kind 0x34 its collider touches, when
 * someone rides it and the rider may use things. */
extern "C" void fn_801C067C(TVehicleBase *vehicle, const boost::shared_ptr<CharaColliBase> *collider) {
    UnkColliContacts_8001EE04 *contacts;
    void *manager = fn_80021B08();
    if (vehicle->GetRider().expired()) {
        return;
    }
    if (collider->get() == 0) {
        return;
    }
    contacts = reinterpret_cast<UnkColliContacts_8001EE04 *>(reinterpret_cast<u8 *>(collider->get()) + 0xC);
    if (contacts == 0) {
        return;
    }
    if (!fn_800B7694(vehicle->GetRider().lock())) {
        return;
    }
    for (UnkColliContactIter_8001EDFC it(fn_8001EE04(contacts)); it.m_node != fn_8001EDEC(contacts);
         fn_80027374(&it, 0)) {
        boost::weak_ptr<CharaColliBase> hit = fn_800587A0(it.m_node);
        boost::shared_ptr<CharInfo> chara = fn_800C4568(manager, hit).lock();
        if (chara.get() != 0) {
            switch (chara->GetKind()) {
            case 0x34:
                if (fn_8005B81C(chara.get()->field_0x34)) {
                    chara.get()->PushCommand(new UseCommand(vehicle->GetSelf()));
                }
                break;
            }
        }
    }
}

/* VehicleDataStart's hooks. */
extern "C" void fn_801C0658(void) {
    fn_8006B5A0();
    fn_801B48B4();
}

extern "C" void fn_801C0654(void) {}

/* VehicleDataEnd's load hook: puts the vehicle data file in loader slot 3. */
extern "C" void fn_801C05F8(void) {
    ResourceOneFileReq *resource = new ResourceOneFileReq("common/vehicleResource.one", lbl_80575EE4);
    fn_8001299C(fn_800122D4(), 3, reinterpret_cast<ResourceOneFile *>(resource));
}

/* VehicleDataEnd's unload hook. */
extern "C" void fn_801C05CC(void) {
    fn_800127F8(fn_800122D4(), 3);
    fn_801B486C();
}

GadgetClassDesc lbl_8053BF10 = {
    "VehicleDataStart", fn_801C0658, fn_801C0654, 0, 0, 0x80, 0x258C, 1, 0, 0, 0,
};

GadgetClassDesc lbl_8053BF34 = {
    "VehicleDataEnd", fn_801C05F8, fn_801C05CC, 0, 0, 0x80, 0x258D, 1, 0, 0, 0,
};
