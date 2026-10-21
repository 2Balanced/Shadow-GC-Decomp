/* EnemyHint (class name inferred; the unit has no RTTI): the one-time hint messages enemies show.
 * GunSoldier keeps an EnemyHint and calls Show(index) for situations 0-4; each index shows its
 * hint the first time only. The message ids depend on the stage: a table holds, for each of nine
 * stages, five entries of one or two candidate ids, one of which is picked at random. ShowBossHint
 * shows the hint of a boss stage (used by EggLastMech). File name inferred from the class. Names
 * marked "(name inferred)" are not original. Definitions are in reverse address order (deferred
 * inlining emits functions last-defined first). The unit's .rodata ends with the orphan RTTI name
 * of boost::detail::sp_counted_base (its vtable and RTTI record are dead-stripped);
 * KeepSpCountedBaseRtti reproduces it. */
#include "Game/EnemyHint.h"
#include "Game/Random.h"
#include "boost/shared_ptr.hpp"
#include <dolphin/types.h>
#include "size_assert.h"

/* The hint manager (class name unknown; see HintRing.cpp). Only the member used here is laid
 * out. */
class UnkHintManager_801FB180 {
  public:
    /* 0x00 */ u8 field_0x0[0x24];
    /* 0x24 */ s32 field_0x24; /* nonzero while a hint is shown (meaning inferred) */
};

/* The out-of-line copy of the inline accessor HintRing.cpp defines (0x80109D64). */
UnkHintManager_801FB180 *GetHintManager();

extern "C" {
/* Shows message id (behaviour inferred; arg2 unknown). */
void fn_801FAC44(UnkHintManager_801FB180 *manager, s32 id, int arg2);
/* Closes the shown hint (behaviour inferred). */
void fn_801FABE4(UnkHintManager_801FB180 *manager);
/* Hold and release the hint display around a change (behaviour inferred). */
void fn_801FAAA4(UnkHintManager_801FB180 *manager);
void fn_801FAAB4(UnkHintManager_801FB180 *manager);
/* Shows message id of branch (see HintRing.cpp). */
s32 fn_801FAF00(UnkHintManager_801FB180 *manager, s32 id, s32 branch, int arg3, int arg4);
}

/* The game state block in .bss (only the member used here). */
struct UnkGameState_8057D6A0 {
    /* 0x000 */ u8 field_0x0[0xAC];
    /* 0x0AC */ s32 m_stage; /* (name inferred) */
    /* 0x0B0 */ u8 field_0xB0[0x150 - 0xB0];
};
SIZE_ASSERT(UnkGameState_8057D6A0, 0x150);

extern UnkGameState_8057D6A0 lbl_8057D6A0;

/* One hint situation: the candidate message ids (names inferred). */
struct EnemyHintEntry {
    /* 0x0 */ s32 count;
    /* 0x4 */ const s32 *ids;
};


/* Never called, and dropped by the linker: its inline base destructor call emits the vtable and
 * RTTI of boost::detail::sp_counted_base, whose name string retail keeps in this unit's .rodata
 * (the original's source of it is unknown). Defined first so that it is emitted last. */
void KeepSpCountedBaseRtti(boost::detail::sp_counted_base *counter) {
    counter->boost::detail::sp_counted_base::~sp_counted_base();
}

/* The candidate ids of each stage (values from .sdata2). */
static const s32 s_ids5a[] = {0x7F0, 0x79E};
static const s32 s_ids5b[] = {0x74F, 0x79F};
static const s32 s_ids7a[] = {0x76D, 0x7A8};
static const s32 s_ids7b[] = {0x74E, 0x7A9};
static const s32 s_ids8a[] = {0x76D, 0x7B2};
static const s32 s_ids8b[] = {0x74E, 0x7B3};
static const s32 s_ids12a[] = {0x76C, 0x7BC};
static const s32 s_ids12b[] = {0x74F, 0x7BD};
static const s32 s_ids13a[] = {0x76D, 0x7C6};
static const s32 s_ids13b[] = {0x750, 0x7C7};
static const s32 s_ids16a[] = {0x76D, 0x7D0};
static const s32 s_ids16b[] = {0x74E, 0x7D1};
static const s32 s_ids18a[] = {0x76C, 0x7DA};
static const s32 s_ids18b[] = {0x74F, 0x7DB};
static const s32 s_ids21a[] = {0x76C, 0x7E4};
static const s32 s_ids21b[] = {0x74F, 0x7E5};
static const s32 s_ids22a[] = {0x76D, 0x7EE};
static const s32 s_ids22b[] = {0x74E, 0x7EF};
static const s32 s_ids4[] = {0x762};
static const s32 s_ids3[] = {0x758};
static const s32 s_ids2[] = {0x744};

/* The five hint entries of each stage. */
static const EnemyHintEntry s_stage5[5] = {
    {2, s_ids5a}, {2, s_ids5b}, {1, s_ids2}, {1, s_ids3}, {1, s_ids4}};
static const EnemyHintEntry s_stage7[5] = {
    {2, s_ids7a}, {2, s_ids7b}, {1, s_ids2}, {1, s_ids3}, {1, s_ids4}};
static const EnemyHintEntry s_stage8[5] = {
    {2, s_ids8a}, {2, s_ids8b}, {1, s_ids2}, {1, s_ids3}, {1, s_ids4}};
static const EnemyHintEntry s_stage12[5] = {
    {2, s_ids12a}, {2, s_ids12b}, {1, s_ids2}, {1, s_ids3}, {1, s_ids4}};
static const EnemyHintEntry s_stage13[5] = {
    {2, s_ids13a}, {2, s_ids13b}, {1, s_ids2}, {1, s_ids3}, {1, s_ids4}};
static const EnemyHintEntry s_stage16[5] = {
    {2, s_ids16a}, {2, s_ids16b}, {1, s_ids2}, {1, s_ids3}, {1, s_ids4}};
static const EnemyHintEntry s_stage18[5] = {
    {2, s_ids18a}, {2, s_ids18b}, {1, s_ids2}, {1, s_ids3}, {1, s_ids4}};
static const EnemyHintEntry s_stage21[5] = {
    {2, s_ids21a}, {2, s_ids21b}, {1, s_ids2}, {1, s_ids3}, {1, s_ids4}};
static const EnemyHintEntry s_stage22[5] = {
    {2, s_ids22a}, {2, s_ids22b}, {1, s_ids2}, {1, s_ids3}, {1, s_ids4}};

void EnemyHint::ShowBossHint(int index) {
    if (index == -2) {
        fn_801FABE4(GetHintManager());
    } else if (index != -1) {
        bool held = false;
        if (GetHintManager()->field_0x24 != 0) {
            held = true;
            fn_801FAAA4(GetHintManager());
        }
        switch (lbl_8057D6A0.m_stage) {
        case 0x25:
            fn_801FAF00(GetHintManager(), index + 0x11E, 0, 0, 0);
            break;
        case 0x27:
            fn_801FAF00(GetHintManager(), index + 0x149, 0, 0, 0);
            break;
        case 0x28:
            fn_801FAF00(GetHintManager(), index + 0x15D, 0, 0, 0);
            break;
        }
        if (held) {
            fn_801FAAB4(GetHintManager());
        }
    }
}

/* The entry of hint index on the current stage, or 0 (name inferred). */
static const EnemyHintEntry *GetEntry(int index) {
    switch (lbl_8057D6A0.m_stage) {
    case 5:
        return &s_stage5[index];
    case 7:
        return &s_stage7[index];
    case 8:
        return &s_stage8[index];
    case 12:
        return &s_stage12[index];
    case 13:
        return &s_stage13[index];
    case 16:
        return &s_stage16[index];
    case 18:
        return &s_stage18[index];
    case 21:
        return &s_stage21[index];
    case 22:
        return &s_stage22[index];
    }
    return 0;
}

/* Shows one of the candidate ids of hint index (name inferred). */
static void ShowEntry(int index) {
    const EnemyHintEntry *entry = GetEntry(index);
    if (entry != 0) {
        int pick = 0;
        if (entry->count > 1) {
            pick = fn_8040EA90(entry->count);
        }
        fn_801FAC44(GetHintManager(), entry->ids[pick], 1);
    }
}

EnemyHint::EnemyHint() : m_shown(0) {}

EnemyHint::~EnemyHint() {}

void EnemyHint::Show(int index) {
    if ((m_shown & (1 << index)) == 0) {
        ShowEntry(index);
        m_shown |= 1 << index;
    }
}
