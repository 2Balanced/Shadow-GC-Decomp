/* TPauseManager (class name from RTTI): the task behind the pause menu. It watches for the pause
 * button, pauses the game and opens the menu, moves the menu's cursor and the player whose mission
 * is shown, and resumes, restarts or quits the stage. Other units reach the one instance through
 * lbl_805EF9F4 and call fn_801E2F28, fn_801E2EDC and fn_801E3B40 under those names. File name from
 * the class. Names marked "(name inferred)" are not original; behaviours are inferred from the
 * calls. Definitions are in reverse address order (deferred inlining emits functions last-defined
 * first). */
#include "Game/HandleUser.h"
#include "Game/Task.h"
#include "boost/shared_ptr.hpp"
#include "boost/weak_ptr.hpp"
#include <dolphin/types.h>
#include "size_assert.h"

namespace Mission {
/* A player's mission (Game/Mission.h; reduced to the member read here). */
class Mission {
  public:
    /* Whether the mission can be shown (meaning inferred). */
    u8 Flag() const {
        return m_flag;
    }

    /* 0x00 */ u8 field_0x0[0x14];
    /* 0x14 */ u8 m_flag;
};

/* The players' missions (class name from its methods' symbols): the player shown at 0x20. */
class MissionManager {
  public:
    void SetPlayer(int player);

    /* 0x00 */ u8 field_0x0[0x20];
    /* 0x20 */ s32 m_player;
};
} // namespace Mission

/* The game state (struct name unknown; see Game/GoalAction.cpp). */
struct UnkGameState_8057D6A0 {
    /* 0x000 */ u8 field_0x0[0x38];
    /* 0x038 */ s32 field_0x38;
    /* 0x03C */ u8 field_0x3C[0x84 - 0x3C];
    /* 0x084 */ u8 m_paused; /* (name inferred) */
    /* 0x085 */ u8 field_0x85[0xA8 - 0x85];
    /* 0x0A8 */ u8 field_0xA8[0xB1 - 0xA8];
    /* 0x0B1 */ u8 field_0xB1;
    /* 0x0B2 */ u8 field_0xB2;
    /* 0x0B3 */ u8 field_0xB3[0xB5 - 0xB3];
    /* 0x0B5 */ u8 field_0xB5;
    /* 0x0B6 */ u8 field_0xB6[0xCC - 0xB6];
    /* 0x0CC */ s32 field_0xCC;
    /* 0x0D0 */ u8 field_0xD0[0x125 - 0xD0];
    /* 0x125 */ u8 field_0x125;
    /* 0x126 */ u8 field_0x126[0x150 - 0x126];
};
SIZE_ASSERT(UnkGameState_8057D6A0, 0x150);

extern UnkGameState_8057D6A0 lbl_8057D6A0;

/* One pad's state (struct name unknown; see Game/SaveCycleMessage.cpp). Pad n is entry n + 1. */
struct UnkPad_80051734 {
    /* 0x00 */ u8 field_0x0[0x8];
    /* 0x08 */ u32 m_triggered;
    /* 0x0C */ u8 field_0xC[0x28 - 0xC];
};
SIZE_ASSERT(UnkPad_80051734, 0x28);

/* The pads (struct name unknown): the two players' pads at 0x11C and 0x120. */
struct UnkPads_80051734 {
    /* The buttons pressed this frame on pad. */
    u32 GetTriggered(int pad) {
        return m_pads[pad + 1].m_triggered;
    }

    /* Whether all of mask, or all of its pad or stick part, was pressed on pad (names inferred; as
     * in Game/SaveCycleMessage.cpp). */
    bool IsPressed(int pad, u32 mask) {
        if ((m_pads[pad + 1].m_triggered & mask) == mask) {
            return true;
        }
        u32 buttons = m_pads[pad + 1].m_triggered;
        u32 part = mask & 0x3C0;
        if (part != 0 && (part & buttons) == part) {
            return true;
        }
        part = mask & 0x3C0000;
        if (part != 0 && (part & buttons) == part) {
            return true;
        }
        return false;
    }

    /* Whether port is the second player's (name inferred). */
    bool IsSecondPort(const u32 &port) {
        return port == m_port2;
    }

    /* 0x000 */ UnkPad_80051734 m_pads[6];
    /* 0x0F0 */ u8 field_0xF0[0x11C - 0xF0];
    /* 0x11C */ u32 m_port1;
    /* 0x120 */ u32 m_port2;
};

/* The stage's mode (struct name unknown). */
struct UnkMode_80074470 {
    /* 0x00 */ u8 field_0x0[0x8];
    /* 0x08 */ s32 field_0x8;
    /* 0x0C */ s32 field_0xC;
    /* 0x10 */ s32 field_0x10;
};

/* The stage's players (struct name unknown): a word of flags and the players' entries. */
struct UnkPlayerEntry_80176C3C {
    /* 0x0 */ s32 field_0x0;
    /* 0x4 */ u8 field_0x4[0xC - 0x4];
};

struct UnkPlayers_80176C3C {
    /* 0x00 */ u8 field_0x0[0x8];
    /* 0x08 */ u32 m_flags;
    /* 0x0C */ u8 field_0xC[0x20 - 0xC];
    /* 0x20 */ UnkPlayerEntry_80176C3C m_entries[3];
    /* 0x44 */ u8 field_0x44[0x48 - 0x44];
    /* 0x48 */ s32 field_0x48;
};

/* The stage (struct name unknown): a flag at 0x180A. */
struct UnkStage_8003A840 {
    /* 0x0000 */ u8 field_0x0[0x180A];
    /* 0x180A */ u8 field_0x180A;
};

/* A record of the stage data (struct name unknown). */
struct UnkRecord_80178B5C {
    /* 0x00 */ u8 field_0x0[0x20];
    /* 0x20 */ s32 field_0x20;
};

/* The save data (struct name unknown). */
struct UnkSave_800BB74C {
    /* 0x00 */ u8 field_0x0[0xA0];
    /* 0xA0 */ s32 field_0xA0;
};

/* A menu event (struct name unknown): its id at 0x2. */
struct UnkEvent_8040D4AC {
    /* 0x0 */ u16 field_0x0;
    /* 0x2 */ u16 m_id;
};

class UnkHintManager_801FB180;
UnkHintManager_801FB180 *GetHintManager();

extern "C" {
/* Where new tasks go, and pausing and resuming the task tree (behaviours inferred). */
void *fn_8001C4F8(void);
Task *fn_801EB304(void *root, int layer);
void fn_801EB364(void *root);
void fn_801EB358(void *root);
/* The sound manager: a sound effect, and the sounds paused and resumed (behaviours inferred). */
void *fn_80053164(void);
void fn_801D560C(void *sound, int id, int arg2, int arg3);
void fn_801D51EC(void *sound);
void fn_801D520C(void *sound);
/* The stage's mode and a screen request to it (behaviours inferred). */
UnkMode_80074470 *fn_80074470(void);
void fn_80175EEC(UnkMode_80074470 *mode, int screen, int arg2);
/* The game's pause counter (behaviour inferred). */
void fn_80203970(void *counter, int paused);
UnkStage_8003A840 *fn_8003A840(void);
UnkPads_80051734 *fn_80051734(void);
/* The stage data and one of its records, and a count of the record's (behaviours inferred). */
void *fn_80079808(void);
UnkRecord_80178B5C *fn_80178B5C(void *data, int index);
int fn_8016EF84(void *record);
Mission::MissionManager *fn_8006D6A8(void);
boost::weak_ptr<Mission::Mission> fn_800740E4(Mission::MissionManager *manager, int player);
void *fn_800B74B4(void);
UnkPlayers_80176C3C *fn_80176C3C(void *stage, int index);
/* The hint manager's page cleared, and a hint shown (behaviours inferred). */
void fn_801FABE4(UnkHintManager_801FB180 *manager);
void fn_801FAF00(UnkHintManager_801FB180 *manager, s32 id, s32 branch, int arg3, int arg4);
void *fn_80177D1C(void);
UnkEvent_8040D4AC *fn_8040D4AC(void *events, int index);
/* The menu: a question asked and whether it is open (behaviours inferred). */
void *fn_80117514(void);
int fn_802E58B4(void *menu, int question, s32 port, int arg3);
bool fn_802E5810(void *menu);
int fn_80044E10(void);
UnkSave_800BB74C *fn_800BB74C(void);
/* Whether button kind (0 pause, 1 decide) was pressed on port, and whether it is held (behaviours
 * inferred). */
bool fn_80356260(int button, s32 port);
bool fn_803560C8(int button, s32 port);
}

class TPauseManager : public Task, public HandleUser {
  public:
    TPauseManager(Task *parent);
    virtual ~TPauseManager();
    virtual void Tick(float dt);

    /* Closes the menu (name inferred). */
    void Reset();

    static const char *s_taskName;

    /* 0x30 */ s32 m_state;
    /* 0x34 */ s32 m_player; /* the player whose mission the menu shows */
    /* 0x38 */ s32 m_cursor;
    /* 0x3C */ u8 m_disabled;
    /* 0x3D */ u8 m_canRestart; /* (name inferred) */
    /* 0x3E */ u8 m_timerDone;
    /* 0x40 */ float m_timer;
    /* 0x44 */ s16 field_0x44;
    /* 0x46 */ s16 field_0x46;
    /* 0x48 */ s32 m_request; /* 1 pause the task tree, 2 resume it */
    /* 0x4C */ u8 m_soundPaused;
    /* 0x4D */ u8 field_0x4D;
    /* 0x50 */ u32 m_port;
    /* 0x54 */ s32 field_0x54;
};
SIZE_ASSERT(TPauseManager, 0x58);

extern "C" {
/* This unit's functions (see their definitions). */
void fn_801E2E30(TPauseManager *manager);
void fn_801E2E88(TPauseManager *manager);
void fn_801E2EDC(TPauseManager *manager);
void fn_801E2F28(TPauseManager *manager);
void fn_801E2F88(TPauseManager *manager);
void fn_801E30C4(TPauseManager *manager);
s32 fn_801E3120(TPauseManager *manager);
s32 fn_801E31E8(TPauseManager *manager);
bool fn_801E3260(TPauseManager *manager);
void fn_801E32C4(TPauseManager *manager);
s32 fn_801E333C(const TPauseManager *manager);
s32 fn_801E3404(TPauseManager *manager);
s32 fn_801E3A28(TPauseManager *manager);
}

/* A request to open the menu, and the one instance (fn_801E3F24) (names unknown). */
u8 lbl_805EF9F0;
TPauseManager *lbl_805EF9F4;

/* A value clamped between lo and hi, and the larger and smaller of two (names inferred). */
template <class T> const T &Clamp(const T &value, const T &lo, const T &hi);
template <class T> const T &Min(const T &a, const T &b);
template <class T> const T &Max(const T &a, const T &b);

const char *TPauseManager::s_taskName = "TPauseManager";

/* The bounds the menu's choice is clamped to: named objects, so that they come before the RTTI
 * records in .sdata as in retail. */
#pragma explicit_zero_data on
static s32 s_firstChoice = 0;
#pragma explicit_zero_data reset
static s32 s_lastChoice = 1;

/* Restarts the mission: the mode switches to its restart screen (name inferred). */
inline void RequestRestart() {
    UnkMode_80074470 *mode = fn_80074470();
    mode->field_0x10 = 2;
    if (lbl_8057D6A0.field_0x125) {
        UnkSave_800BB74C *save = fn_800BB74C();
        save->field_0xA0 = fn_80178B5C(fn_80079808(), 0)->field_0x20;
        fn_80175EEC(fn_80074470(), 0xA, 1);
    } else {
        fn_80175EEC(fn_80074470(), 0x10, 1);
    }
}

void TPauseManager::Reset() {
    fn_801E2F28(this);
    fn_801E2EDC(this);
    m_state = 0;
}

TPauseManager::TPauseManager(Task *parent) : Task(parent) {
    m_disabled = 0;
    m_canRestart = 1;
    m_timerDone = 1;
    m_timer = 0.0f;
    field_0x44 = 0;
    field_0x46 = -1;
    m_request = 0;
    m_soundPaused = 0;
    field_0x4D = 1;
    field_0x54 = -1;
    m_name = s_taskName;
    m_flags |= 0x100;
    Reset();
}

TPauseManager::~TPauseManager() {}

/* Creates the one instance (name inferred). */
extern "C" void fn_801E3F24(void) {
    if (lbl_805EF9F4 == 0) {
        lbl_805EF9F4 = new TPauseManager(fn_801EB304(fn_8001C4F8(), 0));
    }
}

/* Pauses or resumes the task tree as requested (name inferred). */
extern "C" void fn_801E3EC4(TPauseManager *manager) {
    switch (manager->m_request) {
    case 1:
        fn_801EB364(fn_8001C4F8());
        break;
    case 2:
        fn_801EB358(fn_8001C4F8());
        break;
    }
    manager->m_request = 0;
}

void TPauseManager::Tick(float dt) {
    if (!m_timerDone) {
        m_timer -= dt;
        if (m_timer <= 0.0f) {
            fn_801D560C(fn_80053164(), 0xE000, 0, 0);
            m_timerDone = 1;
        }
    }
    if (m_disabled == 0) {
        s32 state = m_state;
        switch (state) {
        case 1: {
            s32 choice = fn_801E3A28(this);
            if (choice >= 0) {
                choice = Clamp(choice, s_firstChoice, s_lastChoice);
                fn_801E2F88(this);
                m_state = choice + 2;
                m_cursor = 0;
            }
            break;
        }
        case 9:
            if (!fn_803560C8(1, m_port)) {
                fn_801E2EDC(this);
                m_state = 1;
                fn_80175EEC(fn_80074470(), 3, 1);
            }
            break;
        case 0:
            if (lbl_805EF9F0) {
                fn_801E2E88(this);
            }
            break;
        case 2:
        case 3:
            if (state != 2) {
                m_port = fn_80051734()->m_port2;
            } else {
                m_port = fn_80051734()->m_port1;
            }
            switch (fn_801E3404(this)) {
            case 4:
                break;
            case 1:
                fn_801E2F28(this);
                m_state = 9;
                break;
            case 2:
                m_state = 4;
                break;
            case 3:
                fn_801E2F28(this);
                fn_801E2EDC(this);
                fn_80175EEC(fn_80074470(), 7, 1);
                m_state = 1;
                break;
            }
            break;
        case 4:
            if (fn_801E3260(this)) {
                m_state = 2;
            }
            break;
        case 5:
            switch (fn_801E31E8(this)) {
            case 1:
                fn_801E2F28(this);
                fn_801E2EDC(this);
                m_state = 1;
                break;
            case 2:
                if (fn_80051734()->IsSecondPort(m_port)) {
                    m_state = 3;
                } else {
                    m_state = 2;
                }
                break;
            }
            break;
        case 6:
            switch (fn_801E3120(this)) {
            case 1:
                fn_801E2F28(this);
                fn_801E2EDC(this);
                m_state = 1;
                break;
            case 2:
                if (fn_80051734()->IsSecondPort(m_port)) {
                    m_state = 3;
                } else {
                    m_state = 2;
                }
                break;
            }
            break;
        case 7:
            if (lbl_805EF9F0) {
                fn_801E2E30(this);
            } else {
                fn_801E30C4(this);
            }
            break;
        case 8:
            m_request = 1;
            m_state = 7;
            break;
        }
        lbl_805EF9F0 = 0;
        field_0x54 = -1;
    }
}

/* Sets which player's pause button opens the menu (name inferred). */
extern "C" void fn_801E3B40(TPauseManager *manager, s32 port) {
    if (fn_80074470()->field_0x8 == 3 && fn_80074470()->field_0xC == 0 && fn_80044E10() == 0 &&
        manager->m_state != 0) {
        manager->field_0x54 = fn_80051734()->m_port2 == port ? 1 : 0;
    }
}

/* The player who pressed pause: 0 or 1, or -1 for none (name inferred). */
extern "C" s32 fn_801E3A28(TPauseManager *manager) {
    if (fn_80074470()->field_0x8 != 3) {
        return -1;
    }
    if (fn_80074470()->field_0xC != 0) {
        return -1;
    }
    if (fn_80044E10() != 0) {
        return -1;
    }
    if (manager->m_state == 0) {
        return -1;
    }
    s32 port = fn_80051734()->m_port1;
    if (manager->field_0x4D && (fn_80051734()->GetTriggered(port) & 0x400) == 0x400) {
        return 0;
    }
    if (lbl_8057D6A0.field_0xB2) {
        port = fn_80051734()->m_port2;
        if (manager->field_0x4D && (fn_80051734()->GetTriggered(port) & 0x400) == 0x400) {
            return 1;
        }
    }
    if (manager->field_0x54 >= 0) {
        return manager->field_0x54;
    }
    return -1;
}

/* Runs the menu: moves the cursor and the player shown, and returns what was chosen (0 nothing,
 * 1 resume, 2 the confirmation, 3 restart, 4 quit; names inferred). */
extern "C" s32 fn_801E3404(TPauseManager *manager) {
    if (!manager->field_0x4D || !fn_802E5810(fn_80117514()) || fn_80074470()->field_0x8 == 0xA) {
        return 0;
    }
    UnkPlayers_80176C3C *players = fn_80176C3C(fn_800B74B4(), -1);
    s32 port;
    if (manager->m_state != 2) {
        port = fn_80051734()->m_port2;
    } else {
        port = fn_80051734()->m_port1;
    }
    if (fn_80051734()->IsPressed(port, 0x80080)) {
        manager->m_cursor++;
        if (!manager->m_canRestart && manager->m_cursor == 1) {
            manager->m_cursor = 2;
            fn_801D560C(fn_80053164(), 0xE002, 0, 0);
        } else if (manager->m_cursor >= 3) {
            manager->m_cursor = 2;
        } else {
            fn_801D560C(fn_80053164(), 0xE002, 0, 0);
        }
    } else if (fn_80051734()->IsPressed(port, 0x40040)) {
        manager->m_cursor--;
        if (!manager->m_canRestart && manager->m_cursor == 1) {
            manager->m_cursor = 0;
            fn_801D560C(fn_80053164(), 0xE002, 0, 0);
        } else if (manager->m_cursor < 0) {
            manager->m_cursor = 0;
        } else {
            fn_801D560C(fn_80053164(), 0xE002, 0, 0);
        }
    } else if (fn_80051734()->IsPressed(port, 0x100100)) {
        s32 old = manager->m_player;
        s32 player = old - 1;
        if (player < 0) {
            player = 0;
        }
        if (players->m_entries[player].field_0x0 == -1) {
            player = player - 1;
            if (player < 0) {
                player = 0;
            }
            if (players->m_entries[player].field_0x0 == -1) {
                player = old;
            }
        }
        if (player != old) {
            manager->m_player = player;
            manager->m_cursor = 0;
            fn_801E32C4(manager);
        }
    } else if (fn_80051734()->IsPressed(port, 0x200200)) {
        s32 old = manager->m_player;
        s32 player = old + 1;
        if (player >= 3) {
            player = 2;
        }
        if (players->m_entries[player].field_0x0 == -1) {
            player = player + 1;
            if (player >= 3) {
                player = 2;
            }
            if (players->m_entries[player].field_0x0 == -1) {
                player = old;
            }
        }
        if (player != old) {
            manager->m_player = player;
            manager->m_cursor = 0;
            fn_801E32C4(manager);
        }
    }
    if (lbl_8057D6A0.field_0xCC == 1 && lbl_8057D6A0.field_0xB1 == 0 && lbl_8057D6A0.field_0x125 == 0 &&
        !(fn_80176C3C(fn_800B74B4(), -1)->m_flags & 0x10) && !(fn_80176C3C(fn_800B74B4(), -1)->m_flags & 4) &&
        (fn_80051734()->GetTriggered(port) & 0x10) == 0x10) {
        switch (lbl_8057D6A0.field_0x38) {
        case 3:
        case 4:
        case 5:
            break;
        default:
            return 2;
        }
    } else if (fn_80356260(0, port)) {
        return fn_801E333C(manager);
    } else if (fn_80356260(1, port)) {
        if (manager->m_cursor == 0) {
            if (lbl_8057D6A0.field_0xB1 != 0 || (fn_80176C3C(fn_800B74B4(), -1)->m_flags & 0x10)) {
                return fn_801E333C(manager);
            }
            s32 player = manager->m_player;
            boost::shared_ptr<Mission::Mission> mission = fn_800740E4(fn_8006D6A8(), player).lock();
            if (player == 1 && players->m_entries[player].field_0x0 == -1) {
                fn_801D560C(fn_80053164(), 0xE005, 0, 0);
            } else if (player != 1 && mission.get() != 0 && !mission->Flag()) {
                fn_801D560C(fn_80053164(), 0xE005, 0, 0);
            } else {
                fn_8006D6A8()->SetPlayer(manager->m_player);
                fn_801D560C(fn_80053164(), 0xE003, 0, 0);
                return fn_801E333C(manager);
            }
        } else {
            return fn_801E333C(manager);
        }
    }
    return 0;
}

/* The choice under the cursor (name inferred). The const parameter lets the compiler load the
 * cursor before saving r31, as retail does. */
extern "C" s32 fn_801E333C(const TPauseManager *manager) {
    switch (manager->m_cursor) {
    case 0:
        return 1;
    case 1:
        return 3;
    case 2:
        RequestRestart();
        return 4;
    }
    return 1;
}

/* Shows the hint of the player whose mission the menu shows (name inferred). */
extern "C" void fn_801E32C4(TPauseManager *manager) {
    if (lbl_8057D6A0.field_0x125 == 0) {
        UnkPlayers_80176C3C *players = fn_80176C3C(fn_800B74B4(), -1);
        if (players->field_0x48 > 0) {
            fn_801FABE4(GetHintManager());
            fn_801FAF00(GetHintManager(), players->field_0x48, manager->m_player, 0, 0);
        }
    }
}

/* Whether the first player's pad asks to close the confirmation (name inferred). */
extern "C" bool fn_801E3260(TPauseManager *manager) {
    s32 port = fn_80051734()->m_port1;
    if (manager->field_0x4D && (fn_80051734()->GetTriggered(port) & 0x10) == 0x10) {
        return true;
    }
    return false;
}

/* Asks whether to quit (name inferred): 1 quit, 2 no, 0 not answered. */
extern "C" s32 fn_801E31E8(TPauseManager *manager) {
    switch (fn_802E58B4(fn_80117514(), 0x13, manager->m_port, 0)) {
    case 2:
        fn_80175EEC(fn_80074470(), 7, 1);
        return 1;
    case 1:
        return 2;
    }
    return 0;
}

/* Asks whether to restart (name inferred): 1 restart, 2 no, 0 not answered. */
extern "C" s32 fn_801E3120(TPauseManager *manager) {
    switch (fn_802E58B4(fn_80117514(), 0x14, manager->m_port, 0)) {
    case 2:
        RequestRestart();
        return 1;
    case 1:
        return 2;
    }
    return 0;
}

/* Handles the menu's events (name inferred). */
extern "C" void fn_801E30C4(TPauseManager *manager) {
    UnkEvent_8040D4AC *event = fn_8040D4AC(fn_80177D1C(), 0);
    if (event != 0) {
        switch (event->m_id) {
        case 0x47:
            manager->m_request = 2;
            manager->m_state = 8;
            break;
        }
    }
}

/* Opens the menu (name inferred). */
extern "C" void fn_801E2F88(TPauseManager *manager) {
    if (lbl_8057D6A0.field_0xB2 == 0 && fn_8016EF84(&fn_80178B5C(fn_80079808(), 0)->field_0x20) > 0) {
        manager->m_canRestart = 1;
    } else {
        manager->m_canRestart = 0;
    }
    manager->m_cursor = 0;
    manager->m_player = fn_8006D6A8()->m_player;
    fn_801FABE4(GetHintManager());
    UnkPlayers_80176C3C *players = fn_80176C3C(fn_800B74B4(), -1);
    if (players->m_entries[manager->m_player].field_0x0 == -1) {
        switch (manager->m_player) {
        case 0:
        case 1:
            manager->m_player++;
            break;
        default:
            manager->m_player--;
            break;
        }
    }
    fn_801EB364(fn_8001C4F8());
    lbl_8057D6A0.m_paused = 1;
    UnkStage_8003A840 *stage = fn_8003A840();
    stage->field_0x180A = 1;
    manager->m_timerDone = 0;
    manager->m_timer = 0.2f;
    fn_801D520C(fn_80053164());
    manager->m_soundPaused = 1;
    fn_80175EEC(fn_80074470(), 5, 1);
    lbl_8057D6A0.field_0xB5 = 1;
}

/* Closes the menu's screen and resumes the sounds (name inferred). */
extern "C" void fn_801E2F28(TPauseManager *manager) {
    UnkStage_8003A840 *stage = fn_8003A840();
    stage->field_0x180A = 0;
    if (manager->m_soundPaused) {
        fn_801D560C(fn_80053164(), 0xE001, 0, 0);
        manager->m_soundPaused = 0;
    }
}

/* Resumes the game (name inferred). */
extern "C" void fn_801E2EDC(TPauseManager *manager) {
    manager->m_request = 2;
    lbl_8057D6A0.m_paused = 0;
    fn_801D51EC(fn_80053164());
    lbl_8057D6A0.field_0xB5 = 0;
}

/* Pauses the game for a message (name inferred). */
extern "C" void fn_801E2E88(TPauseManager *manager) {
    lbl_8057D6A0.m_paused = 1;
    manager->m_state = 7;
    manager->m_request = 1;
    fn_80203970(lbl_8057D6A0.field_0xA8, 1);
    fn_80175EEC(fn_80074470(), 5, 1);
}

/* Resumes the game after a message (name inferred). */
extern "C" void fn_801E2E30(TPauseManager *manager) {
    lbl_8057D6A0.m_paused = 0;
    manager->m_state = 1;
    manager->m_request = 2;
    fn_80203970(lbl_8057D6A0.field_0xA8, 0);
    fn_80175EEC(fn_80074470(), 3, 1);
}

template <class T> const T &Clamp(const T &value, const T &lo, const T &hi) {
    return Min(Max(value, lo), hi);
}

template <class T> const T &Max(const T &a, const T &b) {
    return a < b ? b : a;
}

template <class T> const T &Min(const T &a, const T &b) {
    return a < b ? a : b;
}
