/* StageManager (class name from RTTI; see Game/StageManager.h): the stage the stage sequence
 * plays while the game runs. Its first Tick loads the stage's data archive (stgNNNN/
 * stgNNNN_dat.one: the path file PATH.PTP, the block data *BDT* and *ADB* files) and creates the
 * stage's tasks; it then runs the stage action manager until that ends the stage, and its last
 * Tick tears the stage down. The unit also holds the getters other units reach the manager and a
 * few singletons through. The file name and the names marked "(name inferred)" are not original,
 * nor are the fn_ names the functions other units call keep. Definitions are in reverse address
 * order (deferred inlining emits functions last-defined first). The unit's code reaches its
 * .rodata through a pooled base address, so the linker keeps that section whole.
 *
 * Not matching yet (everything else, data and .sbss/.bss layout included, links): Start
 * (0x80178020) holds the new TFogSet in r29 where retail uses r28, and OnStageDataLoaded
 * (0x801775B0) adds the archive entry's 0x34 after the header where retail adds it first, which
 * also shifts four registers. */
/* The shared_ptr control blocks' allocation failure throws a bad_alloc, whose names this unit's
 * .rodata holds (see <new>). */
#define MSL_NEW_INLINE_BAD_ALLOC
#include <cstdio>
#include <cstring>

/* The unreferenced constant block of the original headers before the CharaColliBase names (as in
 * GadgetModelInfo.cpp; the bytes are reproduced). */
static const float s_zeros[7] = {0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f};
#include "Game/CharaColliBaseCharInfo.h"
#include "Game/TNumEdit.h"
/* Another unreferenced constant block of the original headers (the bytes are reproduced). */
static const float s_zeros3[3] = {0.0f, 0.0f, 0.0f};

#include "Game/MathTypes.h"
#include "Game/StageAction.h"
#include "Game/StageManager.h"
#include "Game/StageTable.h"
#include "Game/Task.h"
#include "Game/UnkHintManager.h"
#include "boost/shared_ptr.hpp"
#include "boost/weak_ptr.hpp"
#include <dolphin/types.h>
#include "size_assert.h"

/* The game state (struct name unknown; see Game/CityBombSmall.cpp). */
struct UnkGameState_8057D6A0 {
    /* 0x000 */ u8 field_0x0[0x1C];
    /* 0x01C */ float field_0x1C;
    /* 0x020 */ u8 field_0x20[0x24 - 0x20];
    /* 0x024 */ float field_0x24;
    /* 0x028 */ u8 field_0x28[0x30 - 0x28];
    /* 0x030 */ float field_0x30;
    /* 0x034 */ u8 field_0x34[0x38 - 0x34];
    /* 0x038 */ s32 field_0x38;
    /* 0x03C */ u8 field_0x3C[0x74 - 0x3C];
    /* 0x074 */ float field_0x74;
    /* 0x078 */ u8 field_0x78[0x88 - 0x78];
    /* 0x088 */ u8 field_0x88[0xA8 - 0x88];
    /* 0x0A8 */ s32 m_stageId; /* (name inferred) */
    /* 0x0AC */ s32 field_0xAC;
    /* 0x0B0 */ u8 field_0xB0[0xC0 - 0xB0];
    /* 0x0C0 */ u8 field_0xC0;
    /* 0x0C1 */ u8 field_0xC1[0x124 - 0xC1];
    /* 0x124 */ u8 field_0x124;
    /* 0x125 */ u8 field_0x125;
    /* 0x126 */ u8 field_0x126;
    /* 0x127 */ u8 field_0x127[0x150 - 0x127];
};
SIZE_ASSERT(UnkGameState_8057D6A0, 0x150);

extern UnkGameState_8057D6A0 lbl_8057D6A0;

/* The message source (class name unknown; see Game/MessageTask.cpp). */
class UnkHintTarget_801FB180 {
  public:
    /* 0x00 */ u8 field_0x0[0x64];
    /* 0x64 */ float m_scale;
};

/* TMessageDisp (class name from the RTTI of its shared_ptr control block): the stage's message
 * source, held as the UnkHintTarget_801FB180 the hint manager shows messages from (presumably the
 * same class). Its constructor (0x8004ABD0) and destructor (0x8004AB88) keep their fn_ names. */
class TMessageDisp : public UnkHintTarget_801FB180 {
  public:
    /* 0x68 */ u8 field_0x68[0x74 - 0x68];
};

/* TFogSet (class name from RTTI; Game/TFogSet.cpp). */
class TFogSet : public Task {
  public:
    TFogSet(Task *parent, int player);
    virtual ~TFogSet();
    virtual void Tick(float dt);

    /* 0x28 */ s32 m_player;
    /* 0x2C */ u8 m_first;
};
SIZE_ASSERT(TFogSet, 0x30);

/* The stage's start-up tasks (Game/StageInitalizeTask.cpp). */
class TStageInitalizeTask : public Task {
  public:
    TStageInitalizeTask(Task *parent, u32 frames);

    /* 0x28 */ u8 field_0x28[0x48 - 0x28];
};
SIZE_ASSERT(TStageInitalizeTask, 0x48);

class TStageKeyNoInput : public Task {
  public:
    TStageKeyNoInput(Task *parent, float time);

    /* 0x28 */ u8 field_0x28[0x38 - 0x28];
};
SIZE_ASSERT(TStageKeyNoInput, 0x38);

namespace Mission {
/* The missions of the players (Game/Mission.cpp). */
class MissionManager {
  public:
    void Init(Task *parent, int stage);
};
} // namespace Mission

/* A file of the stage's data archive (struct names unknown; layout from its use). */
struct UnkArchiveEntry_8004C358 {
    /* 0x00 */ char m_name[0x2C];
    /* 0x2C */ u32 m_size;
    /* 0x30 */ u8 field_0x30[0x38 - 0x30];
};

struct UnkArchiveHeader_8004C358 {
    /* 0x00 */ u8 field_0x0[0x10];
    /* 0x10 */ s32 m_count; /* the files after the first two */
    /* 0x14 */ u8 field_0x14[0x34 - 0x14];
    UnkArchiveEntry_8004C358 *GetEntry(int i) {
        return &m_entries[i];
    }

    /* 0x34 */ UnkArchiveEntry_8004C358 m_entries[1];
};

/* The archive loader (class unknown; constructor at 0x8004CFB0, 0x58 bytes). */
struct UnkArchive_8004CFB0 {
    /* 0x00 */ u8 field_0x0[0x34];
    /* 0x34 */ UnkArchiveHeader_8004C358 *m_header;
    /* 0x38 */ u8 field_0x38[0x58 - 0x38];
};

/* The stage's paths (class unknown; constructor at 0x8040E61C, 0x28 bytes). */
struct UnkPath_8040E61C {
    /* 0x00 */ u8 field_0x0[0x14];
    /* 0x14 */ void *m_nodes;
    /* 0x18 */ u8 field_0x18[0x28 - 0x18];
};

/* Two vectors (struct name unknown; fn_8001C4B8 constructs it). */
struct UnkVecPair_8001C4B8 {
    /* 0x00 */ float a[3];
    /* 0x0C */ float b[3];
};

/* The object fn_800BADE8 returns (see Game/SavePoint.cpp). */
struct UnkObject_800BADE8 {
    /* Sets the word at 0x30 (meaning unknown). */
    void SetField30(s32 value) {
        field_0x30 = value;
    }

    /* 0x00 */ u8 field_0x0[0x30];
    /* 0x30 */ s32 field_0x30;
};

/* What fn_8003A840 returns (see Game/TFogSet.cpp). */
struct UnkStage_8003A840 {
    /* 0x0000 */ u8 field_0x0[0x180C];
    /* 0x180C */ float field_0x180C;
};

/* A table fn_800CBAA4 returns (struct names unknown). */
struct UnkEntry_800CBAA4 {
    /* 0x00 */ u8 field_0x0[0xB4];
    /* 0xB4 */ float field_0xB4;
    /* 0xB8 */ u8 field_0xB8[0xC0 - 0xB8];
};

struct UnkTable_800CBAA4 {
    UnkEntry_800CBAA4 m_entries[1];
};

/* The world of each stage area (see Game/BossRender.cpp). */
struct UnkWorldEntry_8051D208 {
    /* 0x00 */ u8 field_0x0[0x10];
    /* 0x10 */ void *m_world;
    /* 0x14 */ void *field_0x14;
};
extern UnkWorldEntry_8051D208 lbl_8051D208[];

/* The stage's block data (struct name unknown; see Game/GadgetModelInfo.cpp). */
struct UnkStageData_800289B0;
extern "C" void fn_80048710(UnkStageData_800289B0 *data, s32 stageId);

struct UnkStageData_800289B0 {
    /* Loads the blocks of stage stageId (name inferred). */
    void Load(s32 stageId) {
        fn_80048710(this, stageId);
    }

    /* 0x000 */ u8 field_0x0[0x454];
    /* 0x454 */ s8 field_0x454;
};

/* The singletons the getters below return (class names unknown). */
class UnkVibrationManager_80177F38 {
  public:
    /* The manager fn_80177F38 made. */
    static UnkVibrationManager_80177F38 *s_instance;

    /* 0x00 */ u8 field_0x0[0x18];
};

class UnkObject_80177D1C {
  public:
    /* 0x00 */ u8 field_0x0[0x68];
};

class UnkObject_801E67EC;

extern "C" {
/* The constructors and destructors of the two (they keep their fn_ names). */
void *fn_8034B63C(void *object);
void fn_8034B548(void *object, int free);
void *fn_8040D594(void *object);
void fn_8040D4B8(void *object, int free);
void *__register_global_object(void *object, void *destructor, void *record);
}

struct UnkFlag_80178678 {
    /* 0x0 */ u8 m_value;
};

/* The object fn_80046178 returns (see Game/Render.h); the word at 0x8 is the fog's parent task. */
struct UnkRenderOwner_80046178 {
    /* 0x0 */ u32 field_0x0;
    /* 0x4 */ u32 field_0x4;
    /* 0x8 */ Task *field_0x8;
};

extern const Vec3f lbl_805E07B8;
extern UnkObject_80177D1C *lbl_805EDF84;

/* The out-of-line copy of the inline accessor HintRing.cpp defines (0x80109D64). */
UnkHintManager_801FB180 *GetHintManager();

extern "C" {
void fn_803A953C(const char *format, ...);
char *fn_803AC6C0(const char *s, const char *sub);
UnkStageData_800289B0 *fn_800289B0(void);
void fn_800483FC(UnkStageData_800289B0 *data);
void fn_800483B0(UnkStageData_800289B0 *data);
void fn_80047F34(UnkStageData_800289B0 *data);
void fn_80048710(UnkStageData_800289B0 *data, s32 stageId);
void fn_8004854C(UnkStageData_800289B0 *data);
void *fn_8001B494(void);
void fn_8001E358(void *paths);
void fn_8001E20C(void *paths, void *nodes);
void fn_8001D9C0(void *paths, void *nodes, const Vec3f *origin, Vec3f scale, int arg4, int arg5,
                 UnkVecPair_8001C4B8 *axes, bool (*filter)(const u32 *flags), int arg8);
UnkVecPair_8001C4B8 *fn_8001C4B8(UnkVecPair_8001C4B8 *pair, const Vec3f *a, const Vec3f *b);
UnkPath_8040E61C *fn_8040E61C(UnkPath_8040E61C *path, void *data);
void fn_8040E57C(UnkPath_8040E61C *path, int free);
void fn_80049B34(void *p);
void *fn_80049B90(u32 size, int heap);
UnkArchive_8004CFB0 *fn_8004CFB0(UnkArchive_8004CFB0 *archive, int arg1, int arg2);
void fn_8004CF68(UnkArchive_8004CFB0 *archive, int free);
bool fn_8004CC6C(UnkArchive_8004CFB0 *archive, const char *path,
                 void (*callback)(StageManager *manager), StageManager *manager, int arg4, int arg5);
void fn_8004CDF4(UnkArchive_8004CFB0 *archive, const char *path);
void fn_8004C748(UnkArchive_8004CFB0 *archive, int index, void *buffer, void (*callback)(void *),
                 int arg4);
void *fn_8004C358(UnkArchive_8004CFB0 *archive, int index, int arg2, int arg3);
bool fn_8004CAF8(UnkArchive_8004CFB0 *archive);
void *fn_8001C4F8(void);
Task *fn_801EB304(void *manager, int kind);
void fn_801EB314(void *manager);
void *fn_8003AD8C(void *block, Task *parent, int arg2, int arg3, int arg4);
void fn_8003AB68(void *block, void *data);
void *fn_80013BC4(void);
void fn_8004FE88(void *manager, void *data);
void fn_8004FF84(void *manager);
TMessageDisp *fn_8004ABD0(TMessageDisp *message, const char *name, int arg2);
void fn_8004AB88(TMessageDisp *message, int free);
void fn_801FAFFC(UnkHintManager_801FB180 *manager);
void fn_801FB0BC(UnkHintManager_801FB180 *manager,
                 const boost::weak_ptr<UnkHintTarget_801FB180> &target);
void *fn_80117514(void);
Task **fn_80117578(void);
void fn_802E5958(void *manager);
void fn_802E597C(void *manager, const boost::weak_ptr<UnkHintTarget_801FB180> &target);
void fn_80174544(StageActionManager *manager);
void fn_80174504(StageActionManager *manager);
void fn_80175F18(StageActionManager *manager);
void fn_803443CC(void);
void fn_80344420(void);
void fn_800C0388(void);
void fn_800C03AC(void);
void fn_801723E0(void);
void fn_801743B8(void);
void fn_801743BC(void);
void *fn_80053164(void);
void fn_801D615C(void *sound, u8 arg1);
void fn_801D6340(void *sound, s32 arg1, s32 stageId, u8 arg3);
void fn_801D4E78(void *sound);
void fn_801D4C5C(void *sound);
void fn_80206494(void);
void fn_802064E4(void);
void *fn_8009978C(void);
void fn_801D9E00(void *manager);
void fn_801D9E4C(void *manager);
void fn_80046024(void);
void fn_80046098(Task *parent);
void *fn_800DDF20(void);
void fn_801DCCA8(void *manager);
void fn_801D2D14(int arg0);
void fn_801D2DE8(int arg0);
void *fn_80119178(void);
void fn_801A2540(void *manager);
void fn_801A2940(void *manager);
void *fn_800C9B84(void);
void fn_800CB820(void *manager);
void fn_800CB86C(void *manager, s32 stageId, s32 arg2);
void *fn_80021B08(void);
void fn_800C8B80(void *manager);
void fn_8034B3E4(UnkVibrationManager_80177F38 *manager);
void fn_8034B454(UnkVibrationManager_80177F38 *manager);
void fn_80411714(void);
UnkObject_800B74B4 *fn_800B74B4(void);
UnkObject_800BADE8 *fn_800BADE8(void);
void fn_801E2F28(UnkObject_800BADE8 *object);
void fn_801E2EDC(UnkObject_800BADE8 *object);
void *fn_80079808(void);
void fn_80178C54(void *record, u8 arg1);
void fn_800CB438(void *task, Task *parent);
bool fn_8025DD20(UnkFlag_80178678 *flag);
void fn_801D3980(Task *parent, s32 stageId);
void fn_801D9DA0(void *task, Task *parent);
void fn_800C9130(void *task, Task *parent);
void fn_801D20F8(void *task, Task *parent, u32 arg2);
UnkTable_800CBAA4 *fn_800CBAA4(void);
UnkStage_8003A840 *fn_8003A840(void);
void fn_801D1E64(void *world);
void fn_801E769C(Task *parent);
void fn_80058A04(Task *parent);
Mission::MissionManager *fn_8006D6A8(void);
void fn_8029773C(Task *parent);
void *fn_800D51EC(void);
void fn_801D0940(void *manager);
void *fn_80099954(void);
void fn_801CE8CC(void *manager, s32 stageId);
UnkRenderOwner_80046178 *fn_80046178(void);
void fn_80336FE4(void *arg0);
void fn_80203890(s32 *stageId);
void fn_802038B4(s32 *stageId, s32 id);
void fn_802BEBE0(s32 stageId);
void fn_800452F8(u8 arg0);
void fn_8003AE90(void);
void *fn_80044254(void);
void fn_80044648(void *arg0, float value);

StageManager *fn_80177D14(void);
u8 fn_80177CF8(void);
bool fn_80177CB8(void);
boost::shared_ptr<UnkHintTarget_801FB180> fn_80177C80(void);
boost::shared_ptr<UnkHintTarget_801FB180> fn_8017792C(const char *name);
void fn_8017786C(void);
void fn_8017743C(void);
UnkObject_80177D1C *fn_80177D1C(void);
UnkVibrationManager_80177F38 *fn_80177F38(void);
void fn_80177FA4(Task **task);
UnkFlag_80178678 *fn_80178678(void);
void fn_80178778(StageManager *manager);
}

namespace boost {

/* boost/shared_ptr.hpp only declares shared_ptr::reset, which the game has out of line (boost
 * 1.32's definition). */
template<class T> inline void shared_ptr<T>::reset() {
    this_type().swap(*this);
}

/* TMessageDisp's destructor keeps its fn_ name. */
template <> inline void checked_delete<TMessageDisp>(TMessageDisp *x) {
    fn_8004AB88(x, 1);
}

} // namespace boost

/* This unit's .sbss and .bss, in retail's order. The function-local statics of the getters below,
 * their guards and destructor records, and the deleters the shared_ptr constructors copy are
 * spelled out: GC/2.7 lays the compiler's own out in a different order (see the getters). */
static UnkFlag_80178678 *s_flag;
UnkObject_801E67EC *lbl_805EF314;
static UnkArchive_8004CFB0 *s_archive;
static void *s_pathData;
StageManager *StageManager::s_instance;
static UnkPath_8040E61C *s_path;
static UnkFlag_80178678 s_flagObject;
static boost::checked_deleter<TMessageDisp> s_messageDeleter;
static s8 s_vibrationInitialized;
static boost::checked_deleter<TFogSet> s_fogDeleter;
static s8 s_objectInitialized;
UnkVibrationManager_80177F38 *UnkVibrationManager_80177F38::s_instance;

static u32 s_vibrationRecord[3];
static u32 s_objectRecord[3];
static u32 s_vibrationObject[6];
static u32 s_objectObject[0x1A];

namespace boost {

/* The shared_ptr constructors (boost 1.32's), copying the deleter from the statics above. */
template <> template <>
inline shared_ptr<UnkHintTarget_801FB180>::shared_ptr(TMessageDisp *p)
    : px(p), pn(p, s_messageDeleter) {
    detail::sp_enable_shared_from_this(pn, p, p);
}

template <> template <> inline shared_ptr<TFogSet>::shared_ptr(TFogSet *p) : px(p), pn(p, s_fogDeleter) {
    detail::sp_enable_shared_from_this(pn, p, p);
}

} // namespace boost

StageManager::StageManager(s32 stageId, u8 replay)
    : StageBase::Base(1), m_state(0), m_loadPath(1), m_replay(replay) {
    fn_800452F8(1);
    m_ended = 0;
    s32 *id = &lbl_8057D6A0.m_stageId;
    fn_802038B4(id, stageId);
    field_0x14 = 0;
    s_instance = this;
    fn_80178678()->m_value = replay;
    if (lbl_8057D6A0.field_0x126 || replay) {
        fn_802BEBE0(-1);
    } else {
        fn_802BEBE0(*id);
    }
    fn_80175F18(fn_80074470());
    m_state = 0;
    fn_8003AE90();
    fn_80044648(fn_80044254(), 1.0f);
}

StageManager::~StageManager() {
    fn_80203890(&lbl_8057D6A0.m_stageId);
    s_instance = 0;
    fn_801D4C5C(fn_80053164());
    fn_802BEBE0(-1);
}

void StageManager::vf_0x10() {
    fn_80074470()->SetResult(2);
    UnkObject_800BADE8 *object = fn_800BADE8();
    fn_801E2F28(object);
    fn_801E2EDC(object);
    object->field_0x30 = 0;
    m_ended = 1;
}

/* Ends the stage (name inferred). */
extern "C" void fn_80178778(StageManager *manager) {
    manager->m_state = 3;
}

int StageManager::Tick() {
    switch (m_state) {
    case 0:
        if (Start()) {
            m_state = 2;
            fn_800BADE8()->SetField30(1);
            fn_80175EEC(fn_80074470(), 2, false);
        } else {
            m_state = 1;
        }
        break;
    case 1:
        if (WaitLoad()) {
            m_state = 2;
            fn_800BADE8()->SetField30(1);
            fn_80175EEC(fn_80074470(), 2, false);
        }
        break;
    case 2:
        Run();
        break;
    case 3:
        Finish();
        return fn_80074470()->field_0x10;
    }
    return 1;
}

/* A flag set to the manager's replay flag (meaning unknown). */
extern "C" UnkFlag_80178678 *fn_80178678(void) {
    if (s_flag == 0) {
        s_flag = &s_flagObject;
    }
    return s_flag;
}

/* Creates the stage's tasks and starts loading its data; true once the data is loaded (name
 * inferred). */
bool StageManager::Start() {
    s32 index;
    s32 stageId;
    fn_80178C54(fn_80079808(), 1);
    Task *parent = fn_801EB304(fn_8001C4F8(), 6);
    void *task = operator new(0x28);
    if (task != 0) {
        fn_800CB438(task, parent);
    }
    new TStageInitalizeTask(parent, 1);
    fn_801D2DE8(0);
    fn_801D2DE8(3);
    if (fn_8025DD20(fn_80178678())) {
        lbl_8057D6A0.field_0xC0 = 1;
    }
    if (!m_replay && !lbl_8057D6A0.field_0x126) {
        if (!(fn_80176C3C(fn_800B74B4(), -1)->m_flags & 4)) {
            fn_801D3980(parent, lbl_8057D6A0.m_stageId);
        }
    }
    Task *layer8 = fn_801EB304(fn_8001C4F8(), 8);
    fn_801D9E4C(fn_8009978C());
    task = operator new(0x28);
    if (task != 0) {
        fn_801D9DA0(task, layer8);
    }
    Task *layer9 = fn_801EB304(fn_8001C4F8(), 9);
    task = operator new(0x28);
    if (task != 0) {
        fn_800C9130(task, layer9);
    }
    Task *layer16 = fn_801EB304(fn_8001C4F8(), 0x10);
    task = operator new(0x30);
    if (task != 0) {
        fn_801D20F8(task, layer16, 0x100000);
    }
    if (lbl_8057D6A0.field_0x38 == 4 || lbl_8057D6A0.field_0x38 == 5) {
        new TStageKeyNoInput(parent, 60.0f);
    }
    fn_802064E4();
    fn_80046098(fn_801EB304(fn_8001C4F8(), 10));
    UnkGameState_8057D6A0 *state = &lbl_8057D6A0;
    index = state->field_0xAC;
    float value = fn_800CBAA4()->m_entries[index].field_0xB4;
    lbl_8057D6A0.field_0x74 = value;
    fn_8003A840()->field_0x180C = value;
    fn_8003A840();
    fn_801D1E64(lbl_8051D208[0].field_0x14);
    Task *layer15 = fn_801EB304(fn_8001C4F8(), 0xF);
    fn_801E769C(layer15);
    fn_80058A04(layer15);
    fn_8006D6A8()->Init(layer15, state->field_0xAC);
    fn_8029773C(layer15);
    fn_801EB304(fn_8001C4F8(), 3);
    field_0x14 = 0;
    stageId = lbl_8057D6A0.m_stageId;
    fn_801D6340(fn_80053164(), state->field_0xAC, stageId, lbl_8057D6A0.field_0x124);
    if (m_replay || lbl_8057D6A0.field_0x126) {
        fn_801D4E78(fn_80053164());
    }
    char name[0x20];
    sprintf(name, "stg%04d", lbl_8057D6A0.m_stageId);
    TMessageDisp *message = (TMessageDisp *)operator new(0x74);
    if (message != 0) {
        message = fn_8004ABD0(message, name, 1);
    }
    m_message = boost::shared_ptr<UnkHintTarget_801FB180>(message);
    fn_801FB0BC(GetHintManager(), boost::weak_ptr<UnkHintTarget_801FB180>(m_message));
    fn_802E597C(fn_80117514(), boost::weak_ptr<UnkHintTarget_801FB180>(m_message));
    fn_800CB86C(fn_800C9B84(), lbl_8057D6A0.m_stageId, state->field_0xAC);
    fn_801A2940(fn_80119178());
    fn_801D0940(fn_800D51EC());
    fn_801CE8CC(fn_80099954(), lbl_8057D6A0.m_stageId);
    fn_801723E0();
    SetUpBlocks();
    fn_8034B454(fn_80177F38());
    fn_801723E0();
    fn_801743BC();
    fn_800C03AC();
    UnkStageInfo_804C4A18 *info = fn_80176C3C(fn_800B74B4(), lbl_8057D6A0.m_stageId);
    if (info != 0 && info->m_callbacks[0] != 0) {
        info->m_callbacks[0]();
    }
    fn_80344420();
    m_fog = boost::shared_ptr<TFogSet>(new TFogSet(fn_80046178()->field_0x8, 0));
    if (LoadStageData(true)) {
        lbl_8057D6A0.field_0x30 = 1.0f;
        lbl_8057D6A0.field_0x1C = 1.0f;
        lbl_8057D6A0.field_0x24 = 1.0f;
        fn_80336FE4(lbl_8057D6A0.field_0x88);
        return true;
    }
    return false;
}

/* Retries loading the stage's data; true once it is loaded (name inferred). */
bool StageManager::WaitLoad() {
    if (LoadStageData(true)) {
        lbl_8057D6A0.field_0x30 = 1.0f;
        lbl_8057D6A0.field_0x1C = 1.0f;
        lbl_8057D6A0.field_0x24 = 1.0f;
        fn_80336FE4(lbl_8057D6A0.field_0x88);
        return true;
    }
    return false;
}

/* Marks a task done and forgets it (name inferred). */
extern "C" void fn_80177FA4(Task **task) {
    if (*task != 0) {
        (*task)->m_flags |= 1;
        *task = 0;
    }
}

/* The vibration manager (class name unknown). */
extern "C" UnkVibrationManager_80177F38 *fn_80177F38(void) {
    if (UnkVibrationManager_80177F38::s_instance == 0) {
        if (!s_vibrationInitialized) {
            __register_global_object(fn_8034B63C(s_vibrationObject), fn_8034B548, s_vibrationRecord);
            s_vibrationInitialized = 1;
        }
        UnkVibrationManager_80177F38::s_instance = (UnkVibrationManager_80177F38 *)s_vibrationObject;
    }
    return UnkVibrationManager_80177F38::s_instance;
}

/* Tears the stage down (name inferred). */
bool StageManager::Finish() {
    s32 stageId = lbl_8057D6A0.m_stageId;
    UnkStageInfo_804C4A18 *info = fn_80176C3C(fn_800B74B4(), stageId);
    if (info != 0 && info->m_callbacks[3] != 0) {
        info->m_callbacks[3]();
    }
    fn_803443CC();
    fn_80177FA4(fn_80117578());
    fn_802E5958(fn_80117514());
    fn_801FAFFC(GetHintManager());
    fn_8004FF84(fn_80013BC4());
    fn_800C0388();
    fn_801723E0();
    fn_801743B8();
    fn_801D615C(fn_80053164(), lbl_8057D6A0.field_0x124);
    fn_80206494();
    fn_801D9E00(fn_8009978C());
    fn_80046024();
    fn_801EB314(fn_8001C4F8());
    fn_801DCCA8(fn_800DDF20());
    fn_801D2D14(0);
    fn_801D2D14(3);
    fn_801A2540(fn_80119178());
    fn_800CB820(fn_800C9B84());
    fn_800C8B80(fn_80021B08());
    ReleasePath();
    ReleaseBlocks();
    fn_8034B3E4(fn_80177F38());
    fn_80411714();
    field_0x14 = 0;
    if (s_archive != 0) {
        fn_8004CF68(s_archive, 1);
        s_archive = 0;
    }
    fn_80174504(fn_80074470());
    return true;
}

/* Runs the stage (name inferred). */
bool StageManager::Run() {
    fn_80174544(fn_80074470());
    if (m_ended) {
        switch (fn_80074470()->m_prevState) {
        case 3:
        case 4:
        case 5:
        case 6:
        case 11:
        case 12:
        case 13:
        case 14:
            fn_80175EEC(fn_80074470(), 0x10, false);
            break;
        }
    }
    return true;
}

extern "C" UnkObject_80177D1C *fn_80177D1C(void) {
    if (lbl_805EDF84 == 0) {
        if (!s_objectInitialized) {
            __register_global_object(fn_8040D594(s_objectObject), fn_8040D4B8, s_objectRecord);
            s_objectInitialized = 1;
        }
        lbl_805EDF84 = (UnkObject_80177D1C *)s_objectObject;
    }
    return lbl_805EDF84;
}

extern "C" StageManager *fn_80177D14(void) {
    return StageManager::s_instance;
}

extern "C" u8 fn_80177CF8(void) {
    if (StageManager::s_instance != 0) {
        return StageManager::s_instance->field_0x14;
    }
    return 0;
}

/* Whether the stage's data archive finished loading (meaning inferred). */
extern "C" bool fn_80177CB8(void) {
    if (StageManager::s_instance != 0 && s_archive != 0) {
        return fn_8004CAF8(s_archive);
    }
    return false;
}

/* The stage's message source (name inferred). */
extern "C" boost::shared_ptr<UnkHintTarget_801FB180> fn_80177C80(void) {
    if (StageManager::s_instance != 0) {
        return StageManager::s_instance->m_message;
    }
    return boost::shared_ptr<UnkHintTarget_801FB180>();
}

/* Replaces the stage's message source with the messages of name, or of the stage without one
 * (name inferred). */
extern "C" boost::shared_ptr<UnkHintTarget_801FB180> fn_8017792C(const char *name) {
    if (StageManager::s_instance != 0) {
        fn_801FAFFC(GetHintManager());
        StageManager::s_instance->m_message.reset();
        if (name != 0) {
            TMessageDisp *message = (TMessageDisp *)operator new(0x74);
            if (message != 0) {
                message = fn_8004ABD0(message, name, 1);
            }
            StageManager::s_instance->m_message =
                boost::shared_ptr<UnkHintTarget_801FB180>(message);
        } else {
            char stage[0x20];
            sprintf(stage, "stg%04d", lbl_8057D6A0.m_stageId);
            TMessageDisp *message = (TMessageDisp *)operator new(0x74);
            if (message != 0) {
                message = fn_8004ABD0(message, stage, 1);
            }
            StageManager::s_instance->m_message =
                boost::shared_ptr<UnkHintTarget_801FB180>(message);
        }
        fn_801FB0BC(GetHintManager(),
                    boost::weak_ptr<UnkHintTarget_801FB180>(StageManager::s_instance->m_message));
        fn_802E597C(fn_80117514(),
                    boost::weak_ptr<UnkHintTarget_801FB180>(StageManager::s_instance->m_message));
        return StageManager::s_instance->m_message;
    }
    StageManager::s_instance->m_message.reset();
    fn_802E5958(fn_80117514());
    return StageManager::s_instance->m_message;
}

/* Drops the stage's message source (name inferred). */
extern "C" void fn_8017786C(void) {
    StageManager *manager = StageManager::s_instance;
    fn_801FAFFC(GetHintManager());
    if (manager->m_message) {
        manager->m_message.reset();
    }
}

/* Whether a path node is left out (bit 22 of its flags; meaning inferred). */
static bool IsPathNodeHidden(const u32 *flags) {
    return (*flags >> 22) & 1;
}

/* Called when PATH.PTP is loaded: builds the stage's paths (name inferred). */
static void OnPathLoaded(void *) {
    UnkPath_8040E61C *path = (UnkPath_8040E61C *)operator new(0x28);
    if (path != 0) {
        path = fn_8040E61C(path, s_pathData);
    }
    s_path = path;
    void *nodes = path->m_nodes;
    fn_8001E20C(fn_8001B494(), nodes);
    if (lbl_8057D6A0.m_stageId == 403) {
        UnkVecPair_8001C4B8 axes;
        Vec3f axis(1.0f, 0.0f, 0.0f);
        fn_8001C4B8(&axes, &lbl_805E07A0, &axis);
        fn_8001D9C0(fn_8001B494(), nodes, &lbl_805E07A0, lbl_805E07B8, 0, 0, &axes,
                    IsPathNodeHidden, 0);
    }
}

/* Called when the stage's data archive is loaded: loads its files (name inferred). */
void StageManager::OnStageDataLoaded(StageManager *manager) {
    fn_803A953C("\x83X\x83" "e\x81[\x83W\x83" "f\x81[\x83^\x83R\x81[\x83\x8B\x83o\x83" "b\x83N\n");
    UnkArchiveHeader_8004C358 *header = s_archive->m_header;
    for (int i = 2; i < header->m_count + 2; i++) {
        UnkArchiveEntry_8004C358 *entry = header->GetEntry(i);
        if (strcmp(entry->m_name, "PATH.PTP") == 0) {
            if (manager->m_loadPath) {
                fn_803A953C("\x83p\x83XPATH.PTP\x82\xCC\x93\xC7\x82\xDD\x8D\x9E\x82\xDD\n");
                s_pathData = fn_80049B90(entry->m_size, 2);
                if (s_pathData != 0) {
                    fn_8004C748(s_archive, i, s_pathData, OnPathLoaded, 0);
                }
                fn_803A953C(
                    "\x83p\x83XPATH.PTP\x82\xCC\x93\xC7\x82\xDD\x8D\x9E\x82\xDD\x8A\xAE\x97\xB9\n");
            }
        } else if (fn_803AC6C0(entry->m_name, "BDT") != 0) {
            fn_803A953C("\x83u\x83\x8D\x83" "b\x83N\x83" "f\x81[\x83^%s\x82\xCC\x93\xC7\x82\xDD"
                        "\x8D\x9E\x82\xDD\n",
                        entry->m_name);
            void *block;
            void *data = fn_8004C358(s_archive, i, 0, 0);
            block = operator new(0x980);
            if (block != 0) {
                block = fn_8003AD8C(block, fn_801EB304(fn_8001C4F8(), 10), 0, 0, 0);
            }
            fn_8003AB68(block, data);
            fn_803A953C("\x83u\x83\x8D\x83" "b\x83N\x83" "f\x81[\x83^%s\x82\xCC\x93\xC7\x82\xDD"
                        "\x8D\x9E\x82\xDD\x8A\xAE\x97\xB9\n",
                        entry->m_name);
        } else if (fn_803AC6C0(entry->m_name, "ADB") != 0) {
            fn_803A953C("\x83" "e\x83N\x83X\x83`\x83\x83\x83" "f\x81[\x83^\x83`\x83" "F\x83\x93\x83W"
                        "\x83" "f\x81[\x83^%s\x82\xCC\x93\xC7\x82\xDD\x8D\x9E\x82\xDD\n",
                        entry->m_name);
            void *data = fn_8004C358(s_archive, i, 0, 0);
            fn_8004FE88(fn_80013BC4(), data);
            fn_803A953C("\x83" "e\x83N\x83X\x83`\x83\x83\x83" "f\x81[\x83^\x83`\x83" "F\x83\x93\x83W"
                        "\x83" "f\x81[\x83^%s\x82\xCC\x93\xC7\x82\xDD\x8D\x9E\x82\xDD\x8A\xAE\x97\xB9\n",
                        entry->m_name);
        }
    }
}

/* Starts loading the stage's data archive, in the background when async; false when the loader
 * could not be made (name inferred). */
bool StageManager::LoadStageData(bool async) {
    char path[0x18];
    s_pathData = 0;
    s_path = 0;
    s_archive = 0;
    sprintf(path, "stg%04d/stg%04d_dat.one", lbl_8057D6A0.m_stageId, lbl_8057D6A0.m_stageId);
    UnkArchive_8004CFB0 *archive = (UnkArchive_8004CFB0 *)operator new(0x58);
    if (archive != 0) {
        archive = fn_8004CFB0(archive, 0, 0);
    }
    s_archive = archive;
    if (archive == 0) {
        return false;
    }
    if (async) {
        fn_803A953C("%s \x82\xF0\x97\xA0\x93\xC7\x82\xDD\x8AJ\x8En\n", path);
        return fn_8004CC6C(s_archive, path, OnStageDataLoaded, this, 0, 0);
    }
    fn_803A953C("%s \x82\xF0\x8A\xAE\x97\xB9\x93\xC7\x82\xDD\x8AJ\x8En\n", path);
    fn_8004CDF4(s_archive, path);
    OnStageDataLoaded(this);
    return true;
}

void StageManager::ReleasePath() {
    fn_8017743C();
}

/* Frees the stage's paths (name inferred). */
extern "C" void fn_8017743C(void) {
    fn_8001E358(fn_8001B494());
    if (s_path != 0) {
        fn_8040E57C(s_path, 1);
        s_path = 0;
    }
    if (s_pathData != 0) {
        fn_80049B34(s_pathData);
        s_pathData = 0;
    }
}

void StageManager::SetUpBlocks() {
    s8 none = -1;
    fn_800289B0()->field_0x454 = none;
    fn_80047F34(fn_800289B0());
    fn_800289B0()->Load(lbl_8057D6A0.m_stageId);
    fn_8004854C(fn_800289B0());
}

void StageManager::ReleaseBlocks() {
    UnkStageData_800289B0 *data = fn_800289B0();
    fn_800483FC(data);
    fn_800483B0(data);
}
