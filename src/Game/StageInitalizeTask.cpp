/* The stage's start-up tasks (file name inferred from TStageInitalizeTask, the class whose name
 * the game spells that way).
 *
 * - TStageInitalizeTask (class name from RTTI): counts down a few frames, then waits for the
 *   players' stage blocks (TBlockManage, reached through a HandleUser) to be ready, places the
 *   players at their start or checkpoint and opens the stage.
 * - TStageKeyNoInput (class name from RTTI): ends the stage once no button or stick has been
 *   touched for a given time.
 *
 * Names marked "(name inferred)" are not original, nor are the fn_ names the functions other units
 * call keep. Definitions are in reverse address order (deferred inlining emits functions
 * last-defined first). */
#include "Game/MathTypes.h"
#include "Game/StageAction.h"
#include "Game/StageTable.h"
#include "Game/Task.h"
#define TBLOCKMANAGE_DEFINE_CLASS
#include "Game/TBlockManage.h"
#include "boost/shared_ptr.hpp"
#include <dolphin/types.h>
#include "size_assert.h"

extern "C" void *memset(void *dst, int value, unsigned long count);

/* The game state (struct name unknown; see Game/CityBombSmall.cpp). */
struct UnkGameState_8057D6A0 {
    /* 0x000 */ u8 field_0x0[0xA8];
    /* 0x0A8 */ s32 m_stageId; /* (name inferred) */
    /* 0x0AC */ u8 field_0xAC[0x126 - 0xAC];
    /* 0x126 */ u8 field_0x126;
    /* 0x127 */ u8 field_0x127[0x13C - 0x127];
    /* 0x13C */ s32 field_0x13C;
    /* 0x140 */ s32 field_0x140;
    /* 0x144 */ u8 field_0x144[0x150 - 0x144];
};
SIZE_ASSERT(UnkGameState_8057D6A0, 0x150);

extern UnkGameState_8057D6A0 lbl_8057D6A0;

struct UnkObject_80079808;
class UnkCharManager_80074194;

/* The game pad fn_80051734 returns (struct name unknown). */
struct UnkPad_80051734 {
    /* 0x00 */ u8 field_0x0[0xC8];
    /* 0xC8 */ u32 m_buttons;
    /* 0xCC */ u8 field_0xCC[0xD8 - 0xCC];
    /* The stick's position (names inferred). */
    float GetStickX() {
        return m_stickX;
    }
    float GetStickY() {
        return m_stickY;
    }

    /* 0xD8 */ float m_stickX;
    /* 0xDC */ float m_stickY;
};

/* What fn_80177D14 returns (struct name unknown). */
struct UnkStage_80177D14 {
    /* 0x00 */ u8 field_0x0[0x14];
    /* 0x14 */ u8 field_0x14;
    /* 0x15 */ u8 field_0x15[0x29 - 0x15];
    /* 0x29 */ u8 field_0x29;
};

/* A handle to a HandleUser, as fn_80021C80 returns it and as fn_80045A98 takes it (struct names
 * unknown). */
struct UnkHandle_80021C80 {
    UnkHandle_80021C80(const UnkHandle_80021C80 &other) : m_index(other.m_index), m_serial(other.m_serial) {}

    /* 0x0 */ u16 m_index;
    /* 0x2 */ s16 m_serial;
};

struct UnkHandleRef_80045A98 {
    UnkHandleRef_80045A98(const UnkHandle_80021C80 &handle) : m_index(handle.m_index), m_serial(handle.m_serial) {}
    UnkHandleRef_80045A98(const UnkHandleRef_80045A98 &other) : m_index(other.m_index), m_serial(other.m_serial) {}

    /* 0x0 */ u16 m_index;
    /* 0x2 */ s16 m_serial;
};

/* The stage blocks of a player (struct name unknown; see Game/BlockChange.cpp). */
struct UnkBlockSet_8003AAB0;

extern "C" {
UnkObject_80079808 *fn_80079808(void);
UnkStage_80177D14 *fn_80177D14(void);
UnkPad_80051734 *fn_80051734(void);
int fn_802BEB68(void);
void fn_802BEB90(void);
Vec3f *fn_8016F360(UnkObject_80079808 *data, int player);
Vec3f *fn_8016F380(UnkObject_80079808 *data, int player);
void fn_800B7F88(int player, const Vec3f *position, const Vec3f *rotation);
void fn_8040CDD0(Vec3f *out, const Vec3f *a, const Vec3f *b); /* out = a + b */
UnkHandle_80021C80 fn_80021C80(int arg0);
HandleUser *fn_80045A98(UnkHandleRef_80045A98 handle);
void fn_800AD298(int arg0, int arg1, int arg2);
void fn_800B7E44(int player);
void fn_8003ACA8(UnkBlockSet_8003AAB0 *blocks, const Vec3f *position);
void fn_8003A52C(UnkBlockSet_8003AAB0 *blocks);
void fn_800452F8(int arg0);
void fn_80203970(s32 *stageId, int arg1);
UnkCharManager_80074194 *fn_80074194(void);
void fn_800A8D9C(UnkCharManager_80074194 *manager);
void fn_800A8DFC(UnkCharManager_80074194 *manager);

}

extern int lbl_805EE588;
extern int lbl_805E4968;
extern UnkBlockSet_8003AAB0 *lbl_8051CE88[4];
extern s32 lbl_805EFC20;

/* An unused vector (name and meaning unknown). Constructing it is the unit's static initializer,
 * 0x801773A8, which the empty Vec3f constructor leaves empty. Nothing refers to it, so it is
 * global and forced active (symbols.txt) to keep the linker from stripping it as retail's did
 * not. */
Vec3f lbl_80580250;

class TStageInitalizeTask : public Task {
  public:
    TStageInitalizeTask(Task *parent, u32 waits);
    virtual ~TStageInitalizeTask();
    virtual void Tick(float dt);

    /* Places the players once player's block is ready (name inferred). */
    bool WaitBlock(bool first);

    static const char *s_taskName;

    /* 0x28 */ u32 m_waits; /* 1: first player's block, 2: second player's */
    /* 0x2C */ s32 m_readyFrames;
    /* 0x30 */ s32 m_delay;
    /* 0x34 */ Vec3f m_position;
    /* 0x40 */ u8 m_moved;
};
SIZE_ASSERT(TStageInitalizeTask, 0x44);

class TStageKeyNoInput : public Task {
  public:
    TStageKeyNoInput(Task *parent, float limit);
    virtual ~TStageKeyNoInput();
    virtual void Tick(float dt);

    /* 0x28 */ float m_time;
    /* 0x2C */ float m_limit;
    /* 0x30 */ s32 m_delay;
};
SIZE_ASSERT(TStageKeyNoInput, 0x34);

TStageInitalizeTask::~TStageInitalizeTask() {}

const char *TStageInitalizeTask::s_taskName = "TStageInitalizeTask";

TStageInitalizeTask::TStageInitalizeTask(Task *parent, u32 waits) : Task(parent) {
    m_name = s_taskName;
    m_waits = waits;
    m_moved = 0;
    m_readyFrames = 0;
    m_delay = 30;
    fn_80203970(&lbl_8057D6A0.m_stageId, 1);
    m_flags |= 0x100;
}

void TStageInitalizeTask::Tick(float dt) {
    m_delay--;
    if (m_delay == 0) {
        fn_800452F8(0);
    }
    if (!m_moved && (m_waits & 3)) {
        if (lbl_8051CE88[0] != 0) {
            fn_800091D0(&m_position, fn_8016F380(fn_80079808(), 0));
            fn_8003ACA8(lbl_8051CE88[0], &m_position);
            fn_8003A52C(lbl_8051CE88[0]);
            m_moved = 1;
        } else {
            return;
        }
    }
    if (m_waits & 1) {
        if (WaitBlock(true)) {
            m_waits &= ~1;
        }
    }
    if (m_waits & 2) {
        if (WaitBlock(false)) {
            m_waits &= ~2;
        }
    }
    if (m_waits == 0) {
        m_readyFrames++;
        if (m_readyFrames >= 3) {
            UnkStage_80177D14 *stage = fn_80177D14();
            if (stage != 0) {
                stage->field_0x14 = 1;
                fn_802BEB90();
                fn_800A8D9C(fn_80074194());
                fn_800A8DFC(fn_80074194());
            }
            lbl_805EFC20 = 1;
            m_flags |= 1;
        }
    }
}

/* Puts the players at their checkpoints, or out of the way (name inferred). */
extern "C" void fn_80176E84(bool first);

bool TStageInitalizeTask::WaitBlock(bool first) {
    TBlockManage *block = dynamic_cast<TBlockManage *>(fn_80045A98(fn_80021C80(0)));
    if (block != 0 && block->m_ready && !(block->m_loading | (block->field_0x40 != 0))) {
        if (first == true) {
            if (!(fn_80176C3C(fn_800B74B4(), lbl_8057D6A0.m_stageId)->m_flags & 4)) {
                fn_800AD298(14, 0, 0);
            } else {
                int a = lbl_8057D6A0.field_0x13C;
                int b = lbl_8057D6A0.field_0x140;
                if (a > 19 || a < 14) {
                    a = 14;
                }
                if (b > 19 || b < 14) {
                    b = 14;
                }
                fn_800AD298(a, 0, 0);
                fn_800AD298(b, 1, 1);
            }
            fn_80176E84(first);
        }
        UnkStageInfo_804C4A18 *info = fn_80176C3C(fn_800B74B4(), -1);
        if (info != 0 && (info->m_flags & 8)) {
            fn_800B7E44(lbl_805EE588);
        }
        fn_8003ACA8(lbl_8051CE88[0], 0);
        return true;
    }
    return false;
}

extern "C" void fn_80176E84(bool first) {
    UnkObject_80079808 *data = fn_80079808();
    if (fn_80177D14()->field_0x29 || lbl_8057D6A0.field_0x126) {
        Vec3f away(-100000.0f, -100000.0f, -100000.0f);
        fn_800B7F88(lbl_805EE588, &away, fn_8016F360(data, lbl_805EE588));
    } else if (first == true) {
        fn_800B7F88(lbl_805EE588, fn_8016F380(data, lbl_805EE588), fn_8016F360(data, lbl_805EE588));
        Vec3f position;
        Vec3f offset(10.0f, 0.0f, 0.0f);
        fn_8040CDD0(&position, fn_8016F380(data, lbl_805E4968), &offset);
        fn_800B7F88(lbl_805E4968, &position, fn_8016F360(data, lbl_805E4968));
    } else {
        fn_800B7F88(lbl_805EE588, fn_8016F380(data, lbl_805EE588), fn_8016F360(data, lbl_805EE588));
        fn_800B7F88(lbl_805E4968, fn_8016F380(data, lbl_805E4968), fn_8016F360(data, lbl_805E4968));
    }
}

TStageKeyNoInput::TStageKeyNoInput(Task *parent, float limit) : Task(parent) {
    m_name = "TStageKeyNoInput";
    m_limit = limit;
    m_time = 0.0f;
    m_delay = 60;
    m_flags |= 0x100;
}

void TStageKeyNoInput::Tick(float dt) {
    if (fn_802BEB68() == 0) {
        if (m_delay == 0) {
            if (!((fn_80051734()->m_buttons & 1) == 1 || (fn_80051734()->m_buttons & 2) == 2 ||
                  (fn_80051734()->m_buttons & 8) == 8 || (fn_80051734()->m_buttons & 0x10) == 0x10 ||
                  0.0f != fn_80051734()->GetStickX() || 0.0f != fn_80051734()->GetStickY())) {
                m_time += dt;
                if (m_time >= m_limit) {
                    fn_80074470()->SetResult(2);
                    fn_80175EEC(fn_80074470(), 16, false);
                    m_flags |= 1;
                }
            } else {
                m_time = 0.0f;
            }
        } else {
            m_delay--;
        }
    }
}

TStageKeyNoInput::~TStageKeyNoInput() {}
