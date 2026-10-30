/* TGameInfomation (class name from RTTI, spelled so): the task that owns the heads-up display of a
 * stage. It creates the TGi displays (the gauges, the command window, the score, the pause screen,
 * the ring challenge), loads the number font and the enemy gauge's CSD project, and frees them
 * when it ends. The unit also keeps the language-specific path of a texture dictionary, which the
 * displays load. The file name and the names marked "(name inferred)" are not original; the
 * functions other units call keep their fn_ names. Definitions are in reverse address order
 * (deferred inlining emits functions last-defined first). */
#define TGI_NO_NUMBER_COLORS
#include <stdio.h>
#include <string.h>

#include "Game/Gadget.h"
#include "Game/Mission.h"
#include "Game/TGi.h"
#include "Game/TGiPause.h"
#include "Game/Task.h"
#include "boost/shared_ptr.hpp"
#include "boost/weak_ptr.hpp"
#include <dolphin/types.h>
#include "size_assert.h"

/* The displays the task creates (class names from RTTI; each is its own unit, Game/<Name>.cpp). */
class TGiHdGage : public Task {
  public:
    TGiHdGage(Task *parent);

    /* 0x28 */ u8 field_0x28[0x40 - 0x28];
};

class TGiHissatu : public Task {
  public:
    TGiHissatu(Task *parent);

    /* 0x28 */ u8 field_0x28[0xE0 - 0x28];
};

class TGiHPGaugeBoss : public Task {
  public:
    TGiHPGaugeBoss(Task *parent);

    /* 0x28 */ u8 field_0x28[0x30 - 0x28];
};

class TGiCommandWindow : public Task {
  public:
    TGiCommandWindow(Task *parent);

    /* 0x28 */ u8 field_0x28[0x30 - 0x28];
};

class TGiScoreDisplay : public Task {
  public:
    TGiScoreDisplay(Task *parent);

    /* 0x28 */ u8 field_0x28[0x30 - 0x28];
};

class TGiRingChallenge : public Task {
  public:
    TGiRingChallenge(Task *parent);

    /* 0x28 */ u8 field_0x28[0x30 - 0x28];
};

/* The item icons' manager (Game/ItemIconManagerTask.cpp). */
namespace ItemIcon {
class Manager {
  public:
    void CreateTask();
};
} // namespace ItemIcon

/* The missions' manager (struct name unknown). */
struct UnkMissionManager_8006D6A8;

/* The language suffixes of file names ("_JP", ...; another unit's .data). */
extern const char *lbl_8051D528[];

extern "C" {
/* The texture dictionaries' manager, and loading a dictionary from it (behaviour inferred). */
void *fn_80014534(void);
void *fn_80039650(void *manager, const char *name);
/* Frees a texture dictionary (behaviour inferred). */
void fn_802E0600(void *dictionary);
/* A file's path in the resource archive, and freeing it (behaviour inferred). */
const char *fn_80051810(const char *path);
void fn_80051830(const char *path);
/* The missions' manager and a player's mission (see Game/Mission.h). */
UnkMissionManager_8006D6A8 *fn_8006D6A8(void);
boost::weak_ptr<Mission::Mission> fn_800740E4(UnkMissionManager_8006D6A8 *manager, int index);
/* The mission displays: showing a player's, and ending them (behaviour inferred). */
void fn_802B4194(int player);
void fn_802B4124(void);
/* The item icons' manager (Game/ItemIconManagerTask.cpp). */
ItemIcon::Manager *fn_8009B6A0(void);
/* The enemy gauge's CSD project: loading and freeing it (Game/GiHPGaugeEnemy.cpp). */
void fn_800BECF8(void);
void fn_800BECB0(void);
}

class TGameInfomation : public Task {
  public:
    TGameInfomation(Task *parent);
    virtual ~TGameInfomation();
};
SIZE_ASSERT(TGameInfomation, 0x28);

/* The task, the font archive of the number displays (Game/TGi.h), a word ResultTask reads
 * (meaning unknown) and the pause screen. */
static TGameInfomation *s_instance;
void *lbl_805EE68C;
s32 lbl_805EE690;
static Task *s_pause;

/* The path fn_800C02CC returns (name inferred). */
static char s_path[0x40];

TGameInfomation::TGameInfomation(Task *parent) : Task(parent) {
    m_name = "TGameInfomation";
    m_flags |= 0x100;
    s_pause = 0;
    if (lbl_8057D6A0.field_0xB3[1] == 0) {
        if (lbl_805EE68C == 0) {
            lbl_805EE68C = fn_80039650(fn_80014534(), "number.txd");
        }
        if (!lbl_8057D6A0.m_twoPlayer) {
            new TGiHdGage(this);
            new TGiHissatu(this);
            if (lbl_8057D6A0.field_0xB1) {
                new TGiHPGaugeBoss(this);
            }
            UnkMissionManager_8006D6A8 *manager = fn_8006D6A8();
            boost::weak_ptr<Mission::Mission> second = fn_800740E4(manager, 2);
            boost::weak_ptr<Mission::Mission> first = fn_800740E4(manager, 0);
            if (!second.expired()) {
                fn_802B4194(2);
            }
            if (!first.expired()) {
                fn_802B4194(0);
            }
        }
        new TGiCommandWindow(this);
        new TGiScoreDisplay(this);
        fn_800BECF8();
        fn_8009B6A0()->CreateTask();
        s_pause = new TGiPause(this);
        new TGiRingChallenge(this);
    }
}

TGameInfomation::~TGameInfomation() {
    fn_802B4124();
    fn_800BECB0();
    if (lbl_805EE68C != 0) {
        fn_802E0600(lbl_805EE68C);
        lbl_805EE68C = 0;
    }
}

/* Creates the task (name inferred). */
extern "C" void fn_800C03AC(void) {
    if (s_instance == 0) {
        s_instance = new TGameInfomation(fn_801EB304(fn_8001C4F8(), 3));
    }
}

/* Ends the task (name inferred). */
extern "C" void fn_800C0388(void) {
    if (s_instance != 0) {
        s_instance->m_flags |= 1;
        s_instance = 0;
    }
}

/* Ends the pause screen; false when there is none (name inferred). */
extern "C" bool fn_800C0358(void) {
    if (s_pause != 0) {
        s_pause->m_flags |= 1;
        s_pause = 0;
        return true;
    }
    return false;
}

/* The path of the texture dictionary name in the current language (name inferred). */
extern "C" const char *fn_800C02CC(const char *name) {
    sprintf(s_path, "%s%s.txd", name, lbl_8051D528[lbl_8057D6A0.m_language]);
    const char *path = fn_80051810(s_path);
    strcpy(s_path, path);
    fn_80051830(path);
    return s_path;
}
