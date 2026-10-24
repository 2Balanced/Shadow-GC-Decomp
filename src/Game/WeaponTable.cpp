/* The weapon table: the record of each weapon type (the types' units define them), whether the type
 * is enabled, its rumble pattern and a flag, and the functions that read it and create a type's
 * weapons and pick-up items. Other units call the functions by their addresses, so they keep those
 * names. The unit has no class of its own; the file name and the other names are inferred.
 * Definitions are in reverse address order (deferred inlining emits functions last-defined
 * first). */
#include "Game/WeaponBase.h"
#include "Game/WeaponItem.h"
#include "Game/MathTypes.h"
#include "boost/shared_ptr.hpp"
#include "boost/weak_ptr.hpp"
#include "size_assert.h"

using Weapon::Item;
using Weapon::WeaponBase;

extern "C" {
/* The weapon system's start and end hooks (both do nothing in this build; meaning inferred). */
void fn_800690D8(void);
void fn_800690E0(void);
}

/* A zero vector (see Game/PJSCameraTargetInfo.cpp). */
extern Vec2f lbl_805F1358;

/* The records of the weapon types (defined by the types' units; Game/GunBattery.cpp's two have
 * their own struct there). */
extern UnkWeaponTypeInfo lbl_8051EB8C;
extern UnkWeaponTypeInfo lbl_8051F834;
extern UnkWeaponTypeInfo lbl_8051E59C;
extern UnkWeaponTypeInfo lbl_8051E434;
extern UnkWeaponTypeInfo lbl_8051F99C;
extern UnkWeaponTypeInfo lbl_80541E7C;
extern UnkWeaponTypeInfo lbl_8051EC5C;
extern UnkWeaponTypeInfo lbl_8051E9D4;
extern UnkWeaponTypeInfo lbl_8051EDCC;
extern UnkWeaponTypeInfo lbl_80559E84;
extern UnkWeaponTypeInfo lbl_8051EA8C;
extern UnkWeaponTypeInfo lbl_8051E684;
extern UnkWeaponTypeInfo lbl_8051F8EC;
extern UnkWeaponTypeInfo lbl_8051E7E4;
extern UnkWeaponTypeInfo lbl_8051E734;
extern UnkWeaponTypeInfo lbl_80541DCC;
extern UnkWeaponTypeInfo lbl_8051E4EC;
extern UnkWeaponTypeInfo lbl_8051EEA8;
extern UnkWeaponTypeInfo lbl_8051EEFC;
extern UnkWeaponTypeInfo lbl_8054D0B8;
extern UnkWeaponTypeInfo lbl_8054D10C;
extern UnkWeaponTypeInfo lbl_8054D160;
extern UnkWeaponTypeInfo lbl_8054657C;
extern UnkWeaponTypeInfo lbl_80546164;
extern UnkWeaponTypeInfo lbl_805464A4;
extern UnkWeaponTypeInfo lbl_805463CC;
extern UnkWeaponTypeInfo lbl_80542080;
extern UnkWeaponTypeInfo lbl_805420B0;
extern UnkWeaponTypeInfo lbl_8051EFE8;
extern UnkWeaponTypeInfo lbl_8051E894;
extern UnkWeaponTypeInfo lbl_80559DD4;
extern UnkWeaponTypeInfo lbl_8051F03C;
extern UnkWeaponTypeInfo lbl_8051F090;
extern UnkWeaponTypeInfo lbl_8051F0E4;
extern UnkWeaponTypeInfo lbl_8051F138;
extern UnkWeaponTypeInfo lbl_8051F18C;
extern UnkWeaponTypeInfo lbl_80547D3C;
extern UnkWeaponTypeInfo lbl_8051F1E0;
extern UnkWeaponTypeInfo lbl_8051F234;
extern UnkWeaponTypeInfo lbl_8051F288;
extern UnkWeaponTypeInfo lbl_8051F2DC;
extern UnkWeaponTypeInfo lbl_8051F330;
extern UnkWeaponTypeInfo lbl_8051F384;
extern UnkWeaponTypeInfo lbl_8051F3D8;
extern UnkWeaponTypeInfo lbl_8051F42C;
extern UnkWeaponTypeInfo lbl_8051F480;
extern UnkWeaponTypeInfo lbl_8051F4D4;
extern UnkWeaponTypeInfo lbl_8051F528;
extern UnkWeaponTypeInfo lbl_8051F57C;
extern UnkWeaponTypeInfo lbl_8051F5D0;
extern UnkWeaponTypeInfo lbl_8051F624;
extern UnkWeaponTypeInfo lbl_8051F678;
extern UnkWeaponTypeInfo lbl_8051F6CC;
extern UnkWeaponTypeInfo lbl_8051F720;
extern UnkWeaponTypeInfo lbl_8054D2C0;
extern UnkWeaponTypeInfo lbl_8054D2F0;
extern UnkWeaponTypeInfo lbl_8054D460;
extern UnkWeaponTypeInfo lbl_8054D490;
extern UnkWeaponTypeInfo lbl_8054D5B0;
extern UnkWeaponTypeInfo lbl_8054D5E0;
extern UnkWeaponTypeInfo lbl_8054D380;
extern UnkWeaponTypeInfo lbl_8054D3B0;
extern UnkWeaponTypeInfo lbl_8054D200;
extern UnkWeaponTypeInfo lbl_8054D230;
extern UnkWeaponTypeInfo lbl_8054D518;

/* An entry of the table (struct name unknown). */
struct UnkWeaponTypeEntry_8051DF38 {
    /* 0x0 */ UnkWeaponTypeInfo *m_info;
    /* 0x4 */ bool m_enabled;
    /* 0x8 */ s32 m_rumble; /* the rumble pattern while firing, -1 for none */
    /* 0xC */ bool field_0xC;
};
SIZE_ASSERT(UnkWeaponTypeEntry_8051DF38, 0x10);

/* The table, by weapon type. */
static UnkWeaponTypeEntry_8051DF38 s_types[0x48] = {
    {0, false, -1, false},
    {&lbl_8051EB8C, false, 0, true},
    {&lbl_8051F834, false, 1, true},
    {&lbl_8051E59C, false, 1, true},
    {&lbl_8051E434, false, 2, true},
    {&lbl_8051F99C, false, 2, true},
    {0, false, -1, true},
    {&lbl_80541E7C, false, 0, true},
    {&lbl_8051EC5C, false, 0, true},
    {&lbl_8051E9D4, false, 1, true},
    {&lbl_8051EDCC, false, 0, true},
    {&lbl_80559E84, false, 2, true},
    {&lbl_8051EA8C, false, 2, true},
    {&lbl_8051E684, false, 3, true},
    {&lbl_8051F8EC, false, 3, true},
    {&lbl_8051E7E4, false, 3, true},
    {&lbl_8051E734, false, 3, true},
    {&lbl_80541DCC, false, 3, true},
    {&lbl_8051E4EC, false, 2, true},
    {&lbl_8051EEA8, false, 2, true},
    {&lbl_8051EEFC, false, 2, true},
    {&lbl_8054D0B8, false, 2, true},
    {&lbl_8054D10C, false, 2, true},
    {&lbl_8054D160, false, 2, true},
    {&lbl_8054657C, false, 0, true},
    {&lbl_80546164, false, 0, true},
    {&lbl_805464A4, false, 0, true},
    {&lbl_805463CC, false, 0, true},
    {&lbl_80542080, false, -1, false},
    {&lbl_805420B0, false, -1, false},
    {&lbl_8051EFE8, false, -1, false},
    {&lbl_8051E894, false, -1, false},
    {&lbl_80559DD4, false, -1, false},
    {&lbl_8051F03C, false, -1, false},
    {&lbl_8051F090, false, -1, false},
    {&lbl_8051F0E4, false, -1, false},
    {&lbl_8051F138, false, -1, false},
    {&lbl_8051F18C, false, -1, false},
    {&lbl_80547D3C, false, -1, false},
    {&lbl_8051F1E0, false, -1, false},
    {&lbl_8051F234, false, -1, false},
    {&lbl_8051F288, false, -1, false},
    {&lbl_8051F2DC, false, -1, false},
    {&lbl_8051F330, false, -1, false},
    {&lbl_8051F384, false, -1, false},
    {&lbl_8051F3D8, false, -1, false},
    {0, false, -1, false},
    {&lbl_8051F42C, false, -1, false},
    {&lbl_8051F480, false, -1, false},
    {&lbl_8051F4D4, false, -1, false},
    {&lbl_8051F528, false, -1, false},
    {&lbl_8051F57C, false, -1, false},
    {&lbl_8051F5D0, false, -1, false},
    {&lbl_8051F624, false, -1, false},
    {&lbl_8051F678, false, -1, false},
    {&lbl_8051F6CC, false, -1, false},
    {&lbl_8051F720, false, -1, false},
    {&lbl_8054D2C0, false, -1, false},
    {&lbl_8054D2F0, false, -1, false},
    {&lbl_8054D460, false, -1, true},
    {&lbl_8054D490, false, -1, true},
    {&lbl_8054D5B0, false, -1, true},
    {&lbl_8054D5E0, false, -1, true},
    {&lbl_8054D380, false, 0, true},
    {&lbl_8054D3B0, false, 0, true},
    {&lbl_8054D200, false, 3, true},
    {&lbl_8054D230, false, 3, true},
    {&lbl_8054D518, false, 2, true},
    {0, true, -1, false},
    {0, true, -1, false},
    {0, true, -1, false},
    {0, true, -1, false},
};

/* The types without a record, which start enabled (meaning inferred). */
static s32 s_specialTypes[4] = {0x44, 0x45, 0x46, 0x47};

extern "C" {

/* Whether type is enabled. */
bool fn_8005BF14(int type) {
    return s_types[type].m_enabled;
}

/* The type's name ("" for none; each type's static initializer stores it). */
const char *fn_8005BEEC(int type) {
    UnkWeaponTypeInfo *info = s_types[type].m_info;
    if (info != 0) {
        return info->name;
    }
    return "";
}

/* A new weapon of type for owner, which keeps a weak pointer to itself; empty when the type is
 * disabled or has none. */
boost::shared_ptr<WeaponBase> fn_8005BBF4(const boost::weak_ptr<CharInfo> &owner, int type) {
    if (!s_types[type].m_enabled) {
        return boost::shared_ptr<WeaponBase>();
    }
    UnkWeaponTypeInfo *info = s_types[type].m_info;
    if (info != 0 && info->create != 0) {
        boost::shared_ptr<WeaponBase> weapon(info->create(owner));
        weapon->m_self = weapon;
        return weapon;
    }
    return boost::shared_ptr<WeaponBase>();
}

} // extern "C"

/* Slot 0x0C: the ammo the item gives (behaviour inferred). */
void Item::vf_0x0C(int arg) {
    m_ammo = arg;
}

extern "C" {

/* The pick-up item of type, with arg passed on to slot 0x0C; empty when the type is disabled or
 * has none. */
boost::shared_ptr<Item> fn_8005B9D4(int type, int arg) {
    if (s_types[type].m_enabled) {
        UnkWeaponTypeInfo *info = s_types[type].m_info;
        if (info != 0 && info->createItem != 0) {
            Item *item = info->createItem();
            item->m_type = type;
            item->vf_0x0C(arg);
            return boost::shared_ptr<Item>(item);
        }
    }
    return boost::shared_ptr<Item>();
}

/* The type's default ammo. */
int fn_8005B9AC(int type) {
    UnkWeaponTypeInfo *info = s_types[type].m_info;
    if (info != 0) {
        return info->field_0x18;
    }
    return 0;
}

/* The pose a character holds the type in. */
int fn_8005B984(int type) {
    UnkWeaponTypeInfo *info = s_types[type].m_info;
    if (info != 0) {
        return info->field_0x1C;
    }
    return 0;
}

/* The type's pick-up sound (0xFFFF for none). */
u16 fn_8005B954(int type) {
    UnkWeaponTypeInfo *info = s_types[type].m_info;
    if (info != 0) {
        return info->field_0x20;
    }
    return 0xFFFF;
}

/* The type's sight angle (in quarter turns), recoil and the float at 0x2C (meaning unknown). */
float fn_8005B92C(int type) {
    UnkWeaponTypeInfo *info = s_types[type].m_info;
    if (info != 0) {
        return info->field_0x24;
    }
    return 0.0f;
}

float fn_8005B904(int type) {
    UnkWeaponTypeInfo *info = s_types[type].m_info;
    if (info != 0) {
        return info->field_0x28;
    }
    return 0.0f;
}

float fn_8005B8DC(int type) {
    UnkWeaponTypeInfo *info = s_types[type].m_info;
    if (info != 0) {
        /* A float, which the record's struct declares as a word. */
        return *reinterpret_cast<float *>(&info->field_0x2C);
    }
    return 0.0f;
}

/* The type's pick-up size. */
Vec2f fn_8005B898(int type) {
    UnkWeaponTypeInfo *info = s_types[type].m_info;
    if (info != 0) {
        return Vec2f(info->field_0x10, info->field_0x14);
    }
    return lbl_805F1358;
}

/* The type's firing effect at a muzzle matrix. */
void fn_8005B84C(int type, const Mtx34f *muzzle) {
    UnkWeaponTypeInfo *info = s_types[type].m_info;
    if (info != 0 && info->fireEffect != 0) {
        info->fireEffect(muzzle);
    }
}

/* The type's rumble pattern while firing (-1 for none), and the flag at 0xC (meaning unknown). */
int fn_8005B834(int type) {
    return s_types[type].m_rumble;
}

bool fn_8005B81C(int type) {
    return s_types[type].field_0xC;
}

/* Starts the weapon system (the weapon setup at 0x8005E300 calls it; meaning inferred). */
void fn_8005B7FC(void) {
    fn_800690D8();
}

/* Ends the weapon system and disables every type (meaning inferred). */
void fn_8005B77C(void) {
    fn_800690E0();
    for (int i = 0; i < 0x48; i++) {
        s_types[i].m_enabled = false;
    }
    for (int i = 0; i < 4; i++) {
        s_types[s_specialTypes[i]].m_enabled = false;
    }
}

/* Enables type (each type's Initialize calls it). */
void fn_8005B760(int type) {
    s_types[type].m_enabled = true;
}

} // extern "C"
