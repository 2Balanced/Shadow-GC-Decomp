/* What a broken container held (fn_8033E828): an item box, a weapon, rings, a healing unit, an
 * energy core, or one of each weapon the save data has unlocked (fn_8033E9A0). The containers
 * (Game/ContainerHard.cpp, ContainerNormal.cpp, ContainerSecret.cpp, ContainerWeapon.cpp) and the
 * thrown ones (Game/ThrowContainer.cpp) call it after Game/ContainerBreak.cpp has broken them. The
 * unit has no RTTI; the file and function names are ours. Definitions are in reverse address
 * order (deferred inlining emits functions last-defined first). */
#include "Game/GadgetUtil.h"
#include "Game/MathTypes.h"
#include "size_assert.h"

/* What ScatteringRing::Instantiate scatters (layout from Game/ScatteringRing.cpp; its implicit
 * constructor is out of line at 0x800B7E40). */
struct UnkScatterRequest_800B7E40 {
    /* 0x0 */ Vec3f position;
    /* 0xC */ s32 count;
};

/* The save data fn_800BB74C returns (class unknown; see Game/SecretKey.cpp). */
struct UnkSaveData_800BB74C {
    /* 0x0000 */ u8 field_0x0[0x1710];
    /* 0x1710 */ u8 m_weapons[4]; /* the weapons found (size unknown) */
};

namespace ScatteringRing {
class ScatteringRing {
  public:
    static void Instantiate(const UnkScatterRequest_800B7E40 &request, bool flag);
};
} // namespace ScatteringRing

namespace HealingUnit {
class HealingUnit {
  public:
    /* Spawns a healing unit at position (name inferred; see Game/HealingUnit.cpp). */
    static void Spawn(const Vec3f &position, const Vec3f &rotation, float range);
};
} // namespace HealingUnit

namespace EnergyCore {
class EnergyCoreTask {
  public:
    static void Instantiate(int type, const Vec3f &position, Vec3f &rotation, float radius);
};
} // namespace EnergyCore

extern "C" {
/* Drops a box holding item at position (Game/SetItem.cpp). */
void fn_8010B824(int item, const Vec3f &position, Vec3f &rotation, float delay);
/* Drops a weapon of type at position, thrown along velocity (behaviour inferred). */
void fn_80221BA8(const Vec3f *position, const Vec3f *rotation, const Vec3f *velocity, int type,
                 int arg);
/* The save data, and whether its flag index is set (behaviour inferred). */
UnkSaveData_800BB74C *fn_800BB74C(void);
bool fn_800BBDD8(void *flags, int index);
}

/* The weapons a container of random weapons can hold, in pairs by kind (the second of a pair once
 * both were found; names inferred). */
static s32 lbl_8055DB90[11] = {
    0x39, 0x3A, 0x3B, 0x3C, 0x3D, 0x3E, 0x3F, 0x40, 0x41, 0x42, 0x43,
};

/* Drops one of each weapon found, spread around the container, or the given weapon when none is
 * found (name inferred). */
extern "C" void fn_8033E9A0(const Vec3f *position, const Vec3f *rotation, const Vec3f *velocity,
                            int type) {
    static float s_lift = 10.0f;
    int dropped = 0;
    UnkSaveData_800BB74C *save = fn_800BB74C();
    if (save != 0) {
        s32 weapons[6];
        for (int i = 0; i < 6; i++) {
            weapons[i] = 0;
        }
        int index = -1;
        if (fn_800BBDD8(save->m_weapons, 0)) {
            index = 0;
        }
        if (fn_800BBDD8(save->m_weapons, 1)) {
            index++;
        }
        if (index >= 0) {
            weapons[0] = lbl_8055DB90[index];
        }
        index = -1;
        if (fn_800BBDD8(save->m_weapons, 2)) {
            index = 0;
        }
        if (fn_800BBDD8(save->m_weapons, 3)) {
            index++;
        }
        if (index >= 0) {
            weapons[1] = lbl_8055DB90[index + 2];
        }
        index = -1;
        if (fn_800BBDD8(save->m_weapons, 4)) {
            index = 0;
        }
        if (fn_800BBDD8(save->m_weapons, 5)) {
            index++;
        }
        if (index >= 0) {
            weapons[2] = lbl_8055DB90[index + 4];
        }
        index = -1;
        if (fn_800BBDD8(save->m_weapons, 6)) {
            index = 0;
        }
        if (fn_800BBDD8(save->m_weapons, 7)) {
            index++;
        }
        if (index >= 0) {
            weapons[3] = lbl_8055DB90[index + 6];
        }
        index = -1;
        if (fn_800BBDD8(save->m_weapons, 8)) {
            index = 0;
        }
        if (fn_800BBDD8(save->m_weapons, 9)) {
            index++;
        }
        if (index >= 0) {
            weapons[4] = lbl_8055DB90[index + 8];
        }
        if (fn_800BBDD8(save->m_weapons, 10)) {
            weapons[5] = lbl_8055DB90[10];
        }

        Vec3f from;
        ConstructVector(&from, *position);
        from.y += s_lift;
        for (int i = 0; i < 6; i++) {
            if (weapons[i] != 0) {
                Vec3f direction(0.0f, 1.0f, -1.0f);
                Mtx34f matrix;
                fn_8040B6F8(&matrix);
                fn_8040B848(&matrix, rotation->y);
                fn_8040B848(&matrix, 60.0f * i * (lbl_805FAD38 / 180.0f));
                AssignVector(direction, fn_8040B67C(&matrix, &direction));
                fn_8040CE38(&direction, 10.0f);
                fn_80221BA8(&from, rotation, &direction, weapons[i], -1);
                dropped++;
            }
        }
    }
    if (dropped == 0) {
        Vec3f from;
        ConstructVector(&from, *position);
        from.y += s_lift;
        fn_80221BA8(&from, rotation, velocity, type, -1);
    }
}

/* Releases the content of kind select (1 item box, 2 weapon, 3 rings, 4 healing unit, 5 energy
 * core, 6 the weapons found) with value at position. */
extern "C" void fn_8033E828(int select, int value, const Vec3f *position, const Vec3f *rotation) {
    switch (select) {
    case 0:
        break;
    case 1: {
        Vec3f at;
        ConstructVector(&at, *position);
        Vec3f facing = *rotation;
        fn_8010B824(value, at, facing, 0.5f);
        break;
    }
    case 2: {
        Vec3f velocity(0.0f, 1.0f, 0.0f);
        fn_80221BA8(position, rotation, &velocity, value, -1);
        break;
    }
    case 3: {
        UnkScatterRequest_800B7E40 request;
        AssignVector(request.position, *position);
        request.count = value;
        ScatteringRing::ScatteringRing::Instantiate(request, true);
        break;
    }
    case 4: {
        Vec3f at;
        ConstructVector(&at, *position);
        at.y -= 10.0f;
        HealingUnit::HealingUnit::Spawn(at, *rotation, 500.0f);
        break;
    }
    case 5: {
        Vec3f at;
        ConstructVector(&at, *position);
        Vec3f facing = *rotation;
        EnergyCore::EnergyCoreTask::Instantiate(value, at, facing, 500.0f);
        break;
    }
    case 6: {
        Vec3f velocity(0.0f, 1.0f, 0.0f);
        fn_8033E9A0(position, rotation, &velocity, value);
        break;
    }
    }
}
