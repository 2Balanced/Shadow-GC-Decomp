/* The manager of the game's top-level systems (class name unknown; fn_801E2128 in
 * Game/GameSystem.cpp returns it) and the singletons of the game's common systems. The manager keeps
 * the id of the system to start next (1 WaitInitialize, 4 Advertise, 5 StageSequence::Manager) and
 * creates it. The start-up function (fn_801E59C4) creates every singleton once, by calling the
 * functions that return them; this unit holds eighteen of those functions, each making its object on
 * first use, and the destructors of the objects they register for destruction. No RTTI names this
 * unit; the file is named after the manager. Names marked "(name inferred)" are not original.
 * Definitions are in reverse address order (deferred inlining emits functions last-defined first).
 *
 * The singletons are function-local statics in effect (guard, destructor record, object and the
 * pointer the function returns). They are written out as file statics so that .bss and .sbss keep
 * retail's order: the destructor records first, then the objects. GC/2.7 places each record next to
 * its object (see Game/ResourceOneFile.cpp). Most instance pointers are defined in the units that
 * use them first. */
#define MSL_VECTOR_MIDDLE
#define MSL_VECTOR_DESTROY_ALL_LATE
#include <dolphin/types.h>
#include "size_assert.h"
#include <boost/shared_ptr.hpp>
#include <boost/weak_ptr.hpp>
#include <map>
#include <string>
#include <vector>

#include "Game/FogTable.h"
#include "Game/PjsCsd.h"

class CharInfo;
class UnkHintTarget_801FB180;

namespace AdvReal {
class AdvBase;
} // namespace AdvReal

namespace Effect {
namespace System {
class EffectElement;
} // namespace System
} // namespace Effect

namespace Chao {
namespace CSD {
class CProject;
} // namespace CSD
} // namespace Chao

/* The manager of the systems (see Game/GameSystem.cpp): the id of the system to start next. */
class UnkSystemManager_801E2128 {
  public:
    /* 0x0 */ s32 m_next;
    /* 0x4 */ u32 field_0x4;
};
SIZE_ASSERT(UnkSystemManager_801E2128, 0x8);

/* The systems the manager creates (Game/WaitInitialize.cpp, Game/Advertise.cpp,
 * Game/StageSequenceManager.cpp; layouts reduced to their sizes). Their constructors register them
 * as the current system. */
class WaitInitialize {
  public:
    WaitInitialize();

    /* 0x0 */ u8 field_0x0[0x8];
};

class Advertise {
  public:
    Advertise();

    /* 0x0 */ u8 field_0x0[0xC];
};

namespace StageSequence {
class Manager {
  public:
    Manager();

    /* 0x0 */ u8 field_0x0[0x10];
};
} // namespace StageSequence

/* The front-end managers (see Game/AdvFolder.cpp and Game/AdvPhaseManager.h): what the hints are
 * shown through, and the phase manager. */
class UnkManager_801E5D48 {
  public:
    ~UnkManager_801E5D48();

    /* 0x0 */ boost::shared_ptr<UnkHintTarget_801FB180> m_target;
    /* 0x8 */ u32 field_0x8;
};
SIZE_ASSERT(UnkManager_801E5D48, 0xC);

class UnkManager_801E5E00 {
  public:
    ~UnkManager_801E5E00();

    /* 0x00 */ s32 m_request;
    /* 0x04 */ s32 m_current;
    /* 0x08 */ boost::shared_ptr<AdvReal::AdvBase> m_phase;
};
SIZE_ASSERT(UnkManager_801E5E00, 0x10);

/* The CSD project cache (see Game/AdvCsdCache.cpp). */
struct UnkCsdCacheEntry_801E5F0C {
    /* 0x0 */ std::string m_name;
    /* 0xC */ Chao::CSD::CProject *m_project;
};
SIZE_ASSERT(UnkCsdCacheEntry_801E5F0C, 0x10);

/* The vector's destructor is specialized for the entries so that it comes out where retail has it:
 * GC/2.7 emits the template's own definition after the tree members of <map_impl>, retail before
 * them. The body is the template's. */
namespace std {
template <>
__vec_base<UnkCsdCacheEntry_801E5F0C, allocator<UnkCsdCacheEntry_801E5F0C> >::~__vec_base();
} // namespace std

class UnkCsdCache_801E5F0C {
  public:
    ~UnkCsdCache_801E5F0C();

    /* 0x0 */ std::vector<UnkCsdCacheEntry_801E5F0C> m_entries;
};
SIZE_ASSERT(UnkCsdCache_801E5F0C, 0xC);

/* The list implementation the game's lists of pointers share (see Game/ElecSecurityCode.h). */
struct UnkPtrListLink_8001B5B8 {
    /* 0x0 */ UnkPtrListLink_8001B5B8 *prev;
    /* 0x4 */ UnkPtrListLink_8001B5B8 *next;
};

struct UnkPtrList_8001B5B8 {
    /* 0x0 */ u32 m_size;
    /* 0x4 */ UnkPtrListLink_8001B5B8 m_end;
};
SIZE_ASSERT(UnkPtrList_8001B5B8, 0xC);

extern "C" {
/* The list implementation's destructor (the usual destructor flag in flag). */
void fn_8001B5B8(UnkPtrList_8001B5B8 *list, short flag);
}

/* Keeps an inline function out of line (deferred inlining is one level deep). */
inline void UnkLightColliListKeep() {}

class LightColli;

/* The light colliders' list (a std::list of pointers by its shape; name inferred). */
class UnkLightColliList_801E6500 : public UnkPtrList_8001B5B8 {
  public:
    ~UnkLightColliList_801E6500() {
        UnkLightColliListKeep();
        fn_8001B5B8(this, 0);
    }
};

/* The light colliders' shared state (class name unknown; constructed by fn_801EEA70, used by
 * LightColliMasterTask): the colliders, and per character the light effect and the lights. */
class UnkLightColliState_801E6500 {
  public:
    ~UnkLightColliState_801E6500();

    /* 0x00 */ u32 field_0x0;
    /* 0x04 */ UnkLightColliList_801E6500 m_colliders;
    /* 0x10 */ std::map<boost::weak_ptr<CharInfo>, boost::weak_ptr<Effect::System::EffectElement> >
        m_effects;
    /* 0x20 */ std::map<boost::weak_ptr<CharInfo>, int> m_lights;
    /* 0x30 */ std::map<boost::weak_ptr<CharInfo>, int> m_nextLights;
};
SIZE_ASSERT(UnkLightColliState_801E6500, 0x40);

/* The electric panels' tables (class name unknown; the panels' unit, around 0x80258D48, fills them;
 * the values' types are unknown). */
struct UnkElecPanelEntryA_801E6114;
struct UnkElecPanelEntryB_801E6114;
struct UnkElecPanelEntryC_801E6114;

class UnkElecPanelTables_801E6114 {
  public:
    ~UnkElecPanelTables_801E6114();

    /* 0x00 */ std::map<int, UnkElecPanelEntryA_801E6114 *> m_tableA;
    /* 0x10 */ std::map<int, UnkElecPanelEntryB_801E6114 *> m_tableB;
    /* 0x20 */ std::map<int, UnkElecPanelEntryC_801E6114 *> m_tableC;
};
SIZE_ASSERT(UnkElecPanelTables_801E6114, 0x30);

/* The stage title (class name unknown; see Game/StageTitleTask.cpp). */
class UnkObject_801E67EC {
  public:
    ~UnkObject_801E67EC();

    /* 0x00 */ TPjsCsd m_csd;
    /* 0x54 */ u32 field_0x54[2];
};
SIZE_ASSERT(UnkObject_801E67EC, 0x5C);

/* The instance pointer of a class made on first use (name invented; see
 * Game/NpcControlSonicType.cpp). */
template <class T> struct Instance {
    static T *s_instance;
};

class UnkPair_800A26E0;

/* The objects whose constructors (and destructors) other units define, reduced to their sizes. */
struct UnkObject4_801E5C8C {
    /* 0x0 */ u32 field_0x0[2];
};

struct UnkObject14_801E6488 {
    /* 0x00 */ u32 field_0x0[0x30 / 4];
};

struct UnkObject18_801E68A4 {
    /* 0x0 */ u8 field_0x0;
};

struct UnkReset_801DDE9C {
    /* 0x0 */ u8 field_0x0;
};

extern "C" {
void *__register_global_object(void *object, void *destructor, void *record);
/* Clears the frame's state (behaviour inferred, see Game/NowLoading.cpp). */
void fn_802BEB90(void);

/* The constructors and destructors of the singletons' objects. */
void *__ct__26UnkCardCtrlHolder_801E5B80Fv(void *object);
void *__ct__16UnkPair_800A26E0Fv(void *object);
void __dt__16UnkPair_800A26E0Fv(void *object, short flag);
void *fn_800C18AC(void *object);
void *__ct__21UnkBgmPlayer_801E5CA8Fv(void *object);
void *__ct__18UnkHolder_801E5CF8Fv(void *object);
void *__ct__19UnkManager_801E5D48Fv(void *object);
void __dt__19UnkManager_801E5D48Fv(void *object, short flag);
void *__ct__19UnkManager_801E5E00Fv(void *object);
void __dt__19UnkManager_801E5E00Fv(void *object, short flag);
void *__ct__20UnkCsdCache_801E5F0CFv(void *object);
void __dt__20UnkCsdCache_801E5F0CFv(void *object, short flag);
void *__ct__21UnkBuildDate_801E4700Fv(void *object);
void *fn_801E29CC(void *object);
void *__ct__17UnkReset_801DDE9CFv(void *object);
void *fn_8025BEEC(void *object);
void __dt__27UnkElecPanelTables_801E6114Fv(void *object, short flag);
void *fn_8001C5C8(void *object);
void *fn_801EEA70(void *object);
void __dt__27UnkLightColliState_801E6500Fv(void *object, short flag);
void *__ct__18UnkObject_801E67ECFv(void *object);
void __dt__18UnkObject_801E67ECFv(void *object, short flag);

/* The other singletons the start-up function creates. */
void *fn_80074194(void);
void *fn_8006F754(void);
void *fn_800522A8(void);
void *fn_8006D6A8(void);
void *fn_800C9B84(void);
void *fn_800122D4(void);
void *fn_80028B30(void);
void *fn_8001FD38(void);
void *fn_800264A8(void);
void *fn_8001FB68(void);
void *fn_80020054(void);
void *fn_80117578(void);
void *fn_80170D8C(void);
void *fn_801B8828(void);
void *fn_800289B0(void);
void *fn_80013BC4(void);
void *fn_802E05A8(void);
void *fn_80020D40(void);
void *fn_8001B494(void);
void *fn_800218E4(void);
void *fn_80014534(void);
void *fn_80044254(void);
void *fn_800371D8(void);
void *fn_80045AD0(void);
void *fn_80045A00(void);
void *fn_80014B90(void);
void *fn_8003A840(void);
void *fn_8004B13C(void);
void *fn_80009548(void);
void *fn_8000DCC0(void);
void *fn_800144C8(void);
void *fn_80170234(void);
void *fn_8003F8D4(void);
void *fn_801702E4(void);
void *fn_8009978C(void);
void *fn_800DDF20(void);
void *fn_801E2844(void);
void *fn_800CBAA4(void);
void *fn_80051734(void);
void *fn_80046178(void);
void *fn_800BB74C(void);
void *fn_801E2128(void);
void *fn_80060058(void);
void *fn_8001C4F8(void);
void *fn_801D3CC8(void);
void *fn_80053164(void);
void *fn_800D51EC(void);
void *fn_80099954(void);
void *fn_80021B08(void);
void *fn_800A31F0(void);
void *fn_80119178(void);
void *fn_80117514(void);
void *fn_80074470(void);
void *fn_800B74B4(void);
void *fn_80079808(void);
void *fn_80178678(void);
void *fn_801723E0(void);
void *fn_801D3140(void);
void *fn_8016F990(void);
void *fn_8009B6A0(void);
void *fn_801E265C(void);
}

void *AttackIgnorePool();
void *GetHintManager();
void *GetCandleSwitchManager();
void *GetDeltaMorphAnimManager();
void *GetUVAnimManager();

/* The instance pointers other units define. */
class UnkCardCtrlHolder_801E5B80; /* its instance pointer is defined in Game/Npc.cpp */
extern void *lbl_805EE6A8;
extern void *lbl_805EFA00;
extern void *lbl_805EF9E8;
extern void *lbl_805EF2C0;
extern void *lbl_805EE130;
extern void *lbl_805EE02C;
extern void *lbl_805EF208;
extern void *lbl_805EF314;
extern void *lbl_805EE624;

/* The destructor records, then the objects (.bss). */
static u32 s_stageTitleRecord[3];
static u32 s_lightColliStateRecord[3];
static u32 s_elecPanelTablesRecord[3];
static u32 s_csdCacheRecord[3];
static u32 s_phaseManagerRecord[3];
static u32 s_manager801E5D48Record[3];
static u32 s_pairRecord[3];
static u32 s_stageTitle[0x5C / 4];
static u32 s_lightColliState[0x40 / 4];
static u32 s_object801E64A8[0xC / 4];
static UnkObject14_801E6488 s_object801E6488;
static u32 s_elecPanelTables[0x30 / 4];
static u32 s_object801E606C[0xC / 4];
static u32 s_buildDate[0x40 / 4];
static u32 s_csdCache[0xC / 4];
static u32 s_phaseManager[0x10 / 4];
static u32 s_manager801E5D48[0xC / 4];
static u32 s_object801E5C34[0x10 / 4];

/* The guards, the small objects and this unit's instance pointers (.sbss). */
static UnkObject18_801E68A4 s_object801E68A4;
static s8 s_stageTitleInitialized;
static s8 s_lightColliStateInitialized;
static s8 s_object801E64A8Initialized;
static s8 s_elecPanelTablesInitialized;
static s8 s_resetInitialized;
static UnkReset_801DDE9C s_reset;
static s8 s_object801E606CInitialized;
static s8 s_buildDateInitialized;
static s8 s_csdCacheInitialized;
static s8 s_phaseManagerInitialized;
static s8 s_manager801E5D48Initialized;
static s8 s_holderInitialized;
static u32 s_holder;
static s8 s_bgmPlayerInitialized;
static u32 s_bgmPlayer;
static UnkObject4_801E5C8C s_object801E5C8C;
static s8 s_object801E5C34Initialized;
static s8 s_pairInitialized;
static u32 s_pair[2];
static s8 s_cardCtrlHolderInitialized;
static u32 s_cardCtrlHolder[2];
static UnkObject4_801E5C8C *s_object801E5C8CInstance;
static void *s_bgmPlayerInstance;
static void *s_holderInstance;
static UnkManager_801E5D48 *s_manager801E5D48Instance;
static UnkManager_801E5E00 *s_phaseManagerInstance;
static UnkCsdCache_801E5F0C *s_csdCacheInstance;
static UnkElecPanelTables_801E6114 *s_elecPanelTablesInstance;

namespace std {
template <>
__vec_base<UnkCsdCacheEntry_801E5F0C, allocator<UnkCsdCacheEntry_801E5F0C> >::~__vec_base() {
    destroy_all();
    if (data_ != 0) {
        cap_.first().deallocate(data(), cap_.second());
    }
}
} // namespace std

extern "C" {

void *fn_801E68A4(void) {
    if (lbl_805EE624 == 0) {
        lbl_805EE624 = &s_object801E68A4;
    }
    return lbl_805EE624;
}

}

UnkObject_801E67EC::~UnkObject_801E67EC() {}

/* The stage title. */
extern "C" void *fn_801E67EC(void) {
    if (lbl_805EF314 == 0) {
        if (!s_stageTitleInitialized) {
            __register_global_object(__ct__18UnkObject_801E67ECFv(s_stageTitle),
                                     __dt__18UnkObject_801E67ECFv, s_stageTitleRecord);
            s_stageTitleInitialized = 1;
        }
        lbl_805EF314 = s_stageTitle;
    }
    return lbl_805EF314;
}

UnkLightColliState_801E6500::~UnkLightColliState_801E6500() {}

extern "C" {

/* The light colliders' shared state. */
void *fn_801E6500(void) {
    if (lbl_805EF208 == 0) {
        if (!s_lightColliStateInitialized) {
            __register_global_object(fn_801EEA70(s_lightColliState),
                                     __dt__27UnkLightColliState_801E6500Fv,
                                     s_lightColliStateRecord);
            s_lightColliStateInitialized = 1;
        }
        lbl_805EF208 = s_lightColliState;
    }
    return lbl_805EF208;
}

void *fn_801E64A8(void) {
    if (lbl_805EE02C == 0) {
        if (!s_object801E64A8Initialized) {
            fn_8001C5C8(s_object801E64A8);
            s_object801E64A8Initialized = 1;
        }
        lbl_805EE02C = s_object801E64A8;
    }
    return lbl_805EE02C;
}

void *fn_801E6488(void) {
    if (lbl_805EE130 == 0) {
        lbl_805EE130 = &s_object801E6488;
    }
    return lbl_805EE130;
}

}

UnkElecPanelTables_801E6114::~UnkElecPanelTables_801E6114() {}

/* The electric panels' tables. */
extern "C" UnkElecPanelTables_801E6114 *fn_801E6114(void) {
    if (s_elecPanelTablesInstance == 0) {
        if (!s_elecPanelTablesInitialized) {
            __register_global_object(fn_8025BEEC(s_elecPanelTables),
                                     __dt__27UnkElecPanelTables_801E6114Fv,
                                     s_elecPanelTablesRecord);
            s_elecPanelTablesInitialized = 1;
        }
        s_elecPanelTablesInstance =
            reinterpret_cast<UnkElecPanelTables_801E6114 *>(s_elecPanelTables);
    }
    return s_elecPanelTablesInstance;
}

extern "C" {

void *fn_801E60C4(void) {
    if (lbl_805EF2C0 == 0) {
        if (!s_resetInitialized) {
            __ct__17UnkReset_801DDE9CFv(&s_reset);
            s_resetInitialized = 1;
        }
        lbl_805EF2C0 = &s_reset;
    }
    return lbl_805EF2C0;
}

void *fn_801E606C(void) {
    if (lbl_805EF9E8 == 0) {
        if (!s_object801E606CInitialized) {
            fn_801E29CC(s_object801E606C);
            s_object801E606CInitialized = 1;
        }
        lbl_805EF9E8 = s_object801E606C;
    }
    return lbl_805EF9E8;
}

/* The build date. */
void *fn_801E6014(void) {
    if (lbl_805EFA00 == 0) {
        if (!s_buildDateInitialized) {
            __ct__21UnkBuildDate_801E4700Fv(s_buildDate);
            s_buildDateInitialized = 1;
        }
        lbl_805EFA00 = s_buildDate;
    }
    return lbl_805EFA00;
}

}

UnkCsdCache_801E5F0C::~UnkCsdCache_801E5F0C() {}

/* The CSD project cache. */
extern "C" UnkCsdCache_801E5F0C *fn_801E5F0C(void) {
    if (s_csdCacheInstance == 0) {
        if (!s_csdCacheInitialized) {
            __register_global_object(__ct__20UnkCsdCache_801E5F0CFv(s_csdCache),
                                     __dt__20UnkCsdCache_801E5F0CFv, s_csdCacheRecord);
            s_csdCacheInitialized = 1;
        }
        s_csdCacheInstance = reinterpret_cast<UnkCsdCache_801E5F0C *>(s_csdCache);
    }
    return s_csdCacheInstance;
}

UnkManager_801E5E00::~UnkManager_801E5E00() {}

/* The front-end phase manager. */
extern "C" UnkManager_801E5E00 *fn_801E5E00(void) {
    if (s_phaseManagerInstance == 0) {
        if (!s_phaseManagerInitialized) {
            __register_global_object(__ct__19UnkManager_801E5E00Fv(s_phaseManager),
                                     __dt__19UnkManager_801E5E00Fv, s_phaseManagerRecord);
            s_phaseManagerInitialized = 1;
        }
        s_phaseManagerInstance = reinterpret_cast<UnkManager_801E5E00 *>(s_phaseManager);
    }
    return s_phaseManagerInstance;
}

UnkManager_801E5D48::~UnkManager_801E5D48() {}

extern "C" UnkManager_801E5D48 *fn_801E5D48(void) {
    if (s_manager801E5D48Instance == 0) {
        if (!s_manager801E5D48Initialized) {
            __register_global_object(__ct__19UnkManager_801E5D48Fv(s_manager801E5D48),
                                     __dt__19UnkManager_801E5D48Fv, s_manager801E5D48Record);
            s_manager801E5D48Initialized = 1;
        }
        s_manager801E5D48Instance = reinterpret_cast<UnkManager_801E5D48 *>(s_manager801E5D48);
    }
    return s_manager801E5D48Instance;
}

extern "C" {

void *fn_801E5CF8(void) {
    if (s_holderInstance == 0) {
        if (!s_holderInitialized) {
            __ct__18UnkHolder_801E5CF8Fv(&s_holder);
            s_holderInitialized = 1;
        }
        s_holderInstance = &s_holder;
    }
    return s_holderInstance;
}

/* The front end's music player. */
void *fn_801E5CA8(void) {
    if (s_bgmPlayerInstance == 0) {
        if (!s_bgmPlayerInitialized) {
            __ct__21UnkBgmPlayer_801E5CA8Fv(&s_bgmPlayer);
            s_bgmPlayerInitialized = 1;
        }
        s_bgmPlayerInstance = &s_bgmPlayer;
    }
    return s_bgmPlayerInstance;
}

UnkObject4_801E5C8C *fn_801E5C8C(void) {
    if (s_object801E5C8CInstance == 0) {
        s_object801E5C8CInstance = &s_object801E5C8C;
    }
    return s_object801E5C8CInstance;
}

void *fn_801E5C34(void) {
    if (lbl_805EE6A8 == 0) {
        if (!s_object801E5C34Initialized) {
            fn_800C18AC(s_object801E5C34);
            s_object801E5C34Initialized = 1;
        }
        lbl_805EE6A8 = s_object801E5C34;
    }
    return lbl_805EE6A8;
}

UnkPair_800A26E0 *fn_801E5BD0(void) {
    if (Instance<UnkPair_800A26E0>::s_instance == 0) {
        if (!s_pairInitialized) {
            __register_global_object(__ct__16UnkPair_800A26E0Fv(s_pair),
                                     __dt__16UnkPair_800A26E0Fv, s_pairRecord);
            s_pairInitialized = 1;
        }
        Instance<UnkPair_800A26E0>::s_instance = reinterpret_cast<UnkPair_800A26E0 *>(s_pair);
    }
    return Instance<UnkPair_800A26E0>::s_instance;
}

/* The memory card controller's holder. */
void *fn_801E5B80(void) {
    if (Instance<UnkCardCtrlHolder_801E5B80>::s_instance == 0) {
        if (!s_cardCtrlHolderInitialized) {
            __ct__26UnkCardCtrlHolder_801E5B80Fv(s_cardCtrlHolder);
            s_cardCtrlHolderInitialized = 1;
        }
        Instance<UnkCardCtrlHolder_801E5B80>::s_instance = (UnkCardCtrlHolder_801E5B80 *)s_cardCtrlHolder;
    }
    return Instance<UnkCardCtrlHolder_801E5B80>::s_instance;
}

/* Creates every singleton (name inferred). */
void fn_801E59C4(void) {
    fn_80074194();
    fn_8006F754();
    fn_800522A8();
    fn_8006D6A8();
    fn_800C9B84();
    fn_800122D4();
    fn_80028B30();
    AttackIgnorePool();
    fn_8001FD38();
    fn_800264A8();
    fn_8001FB68();
    fn_80020054();
    GetHintManager();
    fn_80117578();
    fn_801E68A4();
    fn_801E67EC();
    fn_80170D8C();
    FogTable::Get();
    fn_801B8828();
    fn_800289B0();
    fn_801E6500();
    fn_80013BC4();
    fn_802E05A8();
    fn_80020D40();
    fn_8001FD38();
    fn_800264A8();
    fn_8001FB68();
    fn_801E64A8();
    fn_8001B494();
    fn_80020054();
    fn_800218E4();
    fn_80014534();
    fn_80044254();
    fn_800371D8();
    fn_80045AD0();
    fn_80045A00();
    fn_80014B90();
    fn_8003A840();
    fn_8004B13C();
    fn_801E6488();
    fn_80009548();
    fn_8000DCC0();
    fn_800144C8();
    fn_80170234();
    fn_8003F8D4();
    fn_801702E4();
    fn_801E6114();
    fn_8009978C();
    GetCandleSwitchManager();
    fn_8009978C();
    fn_800DDF20();
    fn_801E68A4();
    fn_801E60C4();
    GetDeltaMorphAnimManager();
    fn_801E2844();
    fn_800CBAA4();
    fn_80051734();
    fn_801E606C();
    GetHintManager();
    fn_801E6014();
    fn_80046178();
    fn_800BB74C();
    fn_801E2128();
    fn_80060058();
    fn_8001C4F8();
    GetUVAnimManager();
    fn_801E5F0C();
    fn_801E5E00();
    fn_801E5D48();
    fn_801E5CF8();
    fn_801E5CA8();
    fn_801E5C8C();
    fn_801D3CC8();
    fn_80053164();
    fn_800D51EC();
    fn_80099954();
    fn_801E5C34();
    fn_80021B08();
    fn_800A31F0();
    fn_80119178();
    fn_80117578();
    fn_801E67EC();
    fn_801E5BD0();
    fn_801E5B80();
    fn_80117514();
    fn_800C9B84();
    fn_80074470();
    fn_800B74B4();
    fn_80079808();
    fn_80178678();
    fn_801723E0();
    fn_801D3140();
    fn_801702E4();
    fn_8016F990();
    fn_80170D8C();
    fn_8009B6A0();
    FogTable::Get();
    GetCandleSwitchManager();
    fn_801E6500();
    fn_801E6114();
    fn_8006D6A8();
    fn_801E265C();
    fn_801B8828();
    fn_800522A8();
}

/* The manager's constructor (name inferred). */
void fn_801E59B8(UnkSystemManager_801E2128 *manager) {
    manager->m_next = 0;
}

/* Creates system id (name inferred). */
void fn_801E590C(UnkSystemManager_801E2128 *manager, int id) {
    fn_802BEB90();
    switch (id) {
    case 1:
        new WaitInitialize;
        break;
    case 4:
        new Advertise;
        break;
    case 5:
        new StageSequence::Manager;
        break;
    case 13:
        break;
    }
    manager->m_next = 0;
}

/* Creates the system requested next (name inferred). */
void fn_801E5860(UnkSystemManager_801E2128 *manager) {
    int id = manager->m_next;
    fn_802BEB90();
    switch (id) {
    case 1:
        new WaitInitialize;
        break;
    case 4:
        new Advertise;
        break;
    case 5:
        new StageSequence::Manager;
        break;
    case 13:
        break;
    }
    manager->m_next = 0;
}

/* Requests system id next (name inferred). */
void fn_801E5858(UnkSystemManager_801E2128 *manager, int id) {
    manager->m_next = id;
}

/* The system to start after the start-up wait: the front end (name inferred). */
int fn_801E5850(UnkSystemManager_801E2128 *manager, int arg1) {
    return 4;
}

/* Requests the front end (name inferred). */
int fn_801E5840(UnkSystemManager_801E2128 *manager) {
    manager->m_next = 4;
    return manager->m_next;
}

}

#include <map_impl>
#include <vector_impl>
