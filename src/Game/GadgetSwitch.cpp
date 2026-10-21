/* The numbered switches gadgets share (the UnkSwitchBank_800CB69C that fn_800CB69C returns: 256
 * entries of a state and two flags, set up and released by the common gadget data's hooks), the
 * stage areas' world table, and two clump helpers that read the user data the stage's models carry
 * (ATTR_WORLDPARM: the area a model is in; ATTR_OBJPARM: whether its geometry is lit). File name
 * and all names are inferred; the functions keep their address names, as other units call them.
 * Definitions are in reverse address order (deferred inlining emits functions last-defined
 * first). */
#include "Game/UnkSwitchBank_800CB69C.h"
#include "size_assert.h"
#include <cstdio>
#include <cstring>

extern "C" {
/* A 32-bit flag word: fn_8000AAEC constructs one from a value, fn_80014E10 clears it, fn_80014E1C
 * sets bit and fn_80014E34 tests bit (see Game/GadgetPickup.h and Game/ArkCannon.cpp). */
u32 *fn_8000AAEC(u32 *flags, u32 value);
void fn_80014E10(u32 *flags);
void fn_80014E1C(u32 *flags, int bit);
bool fn_80014E34(u32 *flags, int bit);
}

/* The RenderWare types used here, declared locally (layouts as in rw/rpworld.h, reduced to the
 * members used). */
extern "C" {
typedef struct RpGeometry {
    /* 0x00 */ u8 field_0x0[0x8];
    /* 0x08 */ u32 flags; /* bit 5: rpGEOMETRYLIGHT */
} RpGeometry;

typedef struct RpAtomic {
    /* 0x00 */ u8 field_0x0[0x18];
    /* 0x18 */ RpGeometry *geometry;
} RpAtomic;

typedef struct RpClump RpClump;
typedef struct RpUserDataArray RpUserDataArray;

typedef RpAtomic *(*RpAtomicCallBack)(RpAtomic *atomic, void *data);

/* RpClumpForAllAtomics, and RenderWare's user data: the number of arrays of a geometry, an array,
 * its format (1: integers), its name and an integer of it. */
RpClump *fn_80456C54(RpClump *clump, RpAtomicCallBack callBack, void *data);
s32 fn_80452B90(RpGeometry *geometry);
RpUserDataArray *fn_80452BD8(RpGeometry *geometry, s32 index);
s32 fn_80452C80(RpUserDataArray *userData);
const char *fn_80452C78(RpUserDataArray *userData);
s32 fn_80452C88(RpUserDataArray *userData, s32 index);
}

/* A switch's flag word (class name unknown). */
struct UnkSwitchFlags {
    UnkSwitchFlags(u32 value) {
        fn_8000AAEC(&m_bits, value);
    }
    void Clear() {
        fn_80014E10(&m_bits);
    }
    void Set(int bit) {
        fn_80014E1C(&m_bits, bit);
    }
    bool Test(int bit) {
        return fn_80014E34(&m_bits, bit);
    }

    u32 m_bits;
};

/* A switch: its state, and flags (bit 0: set since the last update, bit 1: kept). */
struct UnkSwitch {
    UnkSwitch();

    /* 0x0 */ s32 m_state;
    /* 0x4 */ UnkSwitchFlags m_flags;
};
SIZE_ASSERT(UnkSwitch, 0x8);

class UnkSwitchBank_800CB69C {
  public:
    /* 0x0 */ UnkSwitch *m_switches; /* 256 of them */
};

/* The world index of each stage area (meaning inferred from fn_80169350's callers). */
s32 lbl_8052B6C0[14] = {4, 4, 5, 6, 2, 1, 8, 9, 11, 13, 14, 12, 0, 0};

/* The label format of fn_801692A0's debug loop, and the names of the user data arrays read
 * below. */
static const char s_labelFormat[] = "%i : ";
static const char *const s_objParm = "ATTR_OBJPARM.ATTR_OBJPARM";
static const char *const s_worldParm = "ATTR_WORLDPARM.ATTR_WORLDPARM";

/* Turns lighting off for an atomic whose ATTR_OBJPARM has bit 0 set (an RpClumpForAllAtomics
 * callback). */
static RpAtomic *ApplyObjParm(RpAtomic *atomic, void *data) {
    RpGeometry *geometry = atomic->geometry;
    if (geometry != 0) {
        s32 count = fn_80452B90(geometry);
        for (s32 i = 0; i < count; i++) {
            RpUserDataArray *userData = fn_80452BD8(geometry, i);
            if (userData != 0 && fn_80452C80(userData) == 1 &&
                strcmp(fn_80452C78(userData), s_objParm) == 0) {
                if (fn_80452C88(userData, 0) & 1) {
                    geometry->flags &= ~0x20;
                }
                return atomic;
            }
        }
    }
    return atomic;
}

/* Applies the ATTR_OBJPARM of each atomic of clump. */
extern "C" void fn_80169448(RpClump *clump) {
    fn_80456C54(clump, ApplyObjParm, 0);
}

/* Stops at the first atomic with an ATTR_WORLDPARM, storing its value through data (an
 * RpClumpForAllAtomics callback). */
static RpAtomic *FindWorldParm(RpAtomic *atomic, void *data) {
    RpGeometry *geometry = atomic->geometry;
    if (geometry != 0) {
        s32 count = fn_80452B90(geometry);
        for (s32 i = 0; i < count; i++) {
            RpUserDataArray *userData = fn_80452BD8(geometry, i);
            if (userData != 0 && fn_80452C80(userData) == 1 &&
                strcmp(fn_80452C78(userData), s_worldParm) == 0) {
                *(s32 *)data = fn_80452C88(userData, 0);
                return 0;
            }
        }
    }
    return atomic;
}

/* The stage area clump is in (its ATTR_WORLDPARM; 0 without one). */
extern "C" s32 fn_80169364(RpClump *clump) {
    s32 area = 0;
    fn_80456C54(clump, FindWorldParm, &area);
    return area;
}

/* The world index of stage area area. */
extern "C" s32 fn_80169350(s32 area) {
    return lbl_8052B6C0[area];
}

UnkSwitch::UnkSwitch() : m_state(0), m_flags(0) {}

/* Sets the switches up. */
extern "C" void fn_801692A0(UnkSwitchBank_800CB69C *manager) {
    manager->m_switches = new UnkSwitch[256];
    for (int i = 0; i < 256; i++) {
        char label[0x20];
        sprintf(label, s_labelFormat, i);
    }
}

/* Releases the switches. */
extern "C" void fn_80169258(UnkSwitchBank_800CB69C *manager) {
    if (manager->m_switches != 0) {
        delete[] manager->m_switches;
        manager->m_switches = 0;
    }
}

/* Turns every switch off. */
extern "C" void fn_801691F4(UnkSwitchBank_800CB69C *manager) {
    if (manager->m_switches != 0) {
        for (int i = 0; i < 256; i++) {
            UnkSwitch &entry = manager->m_switches[i];
            entry.m_state = 0;
            entry.m_flags.Clear();
        }
    }
}

/* Turns off the switches that are not kept. */
extern "C" void fn_80169174(UnkSwitchBank_800CB69C *manager) {
    if (manager->m_switches != 0) {
        for (int i = 0; i < 256; i++) {
            if (!manager->m_switches[i].m_flags.Test(1)) {
                UnkSwitch &entry = manager->m_switches[i];
                entry.m_state = 0;
                entry.m_flags.Clear();
            }
        }
    }
}

/* Keeps the switches set since the last call. */
extern "C" void fn_801690F8(UnkSwitchBank_800CB69C *manager) {
    if (manager->m_switches != 0) {
        for (int i = 0; i < 256; i++) {
            if (manager->m_switches[i].m_flags.Test(0)) {
                manager->m_switches[i].m_flags.Set(1);
            }
        }
    }
}

/* Sets the state of switch id. */
extern "C" void fn_801690D0(UnkSwitchBank_800CB69C *manager, int id, int state) {
    if (manager->m_switches != 0 && id >= 0 && id < 256) {
        manager->m_switches[id].m_state = state;
    }
}

/* Sets the state of switch id and marks it set. */
extern "C" void fn_8016907C(UnkSwitchBank_800CB69C *manager, int id, int state) {
    if (manager->m_switches != 0 && id >= 0 && id < 256) {
        manager->m_switches[id].m_state = state;
        manager->m_switches[id].m_flags.Set(0);
    }
}

/* The state of switch id (1 when on). */
extern "C" u32 fn_8016904C(UnkSwitchBank_800CB69C *manager, int id) {
    if (manager->m_switches != 0 && id >= 0 && id < 256) {
        return manager->m_switches[id].m_state;
    }
    return 0;
}
