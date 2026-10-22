/* StageTitleTask (class name from RTTI) and the stage title it plays: the CSD project
 * "csdFiles/stageTitle/StageTitle<stage><language>.txd" shown when a stage starts. The title
 * object is a singleton (fn_801E67EC returns it; Game/StageManager.cpp owns its storage) and the
 * task only advances it once per frame. File name from the class; all member names and the names
 * marked "(name inferred)" are not original. Definitions are in reverse address order (deferred
 * inlining emits functions last-defined first). */
#include "Game/PjsCsd.h"
#include "Game/Render.h"
#include "Game/Task.h"
#include <dolphin/types.h>
#include <stdio.h>
#include <string.h>
#include "size_assert.h"

/* The game state (struct name unknown; as far as this unit reads it, see Game/Mission.cpp). */
struct UnkGameState_8057D6A0 {
    /* 0x000 */ u8 field_0x0[0x58];
    /* 0x058 */ s32 m_language; /* indexes lbl_8051D528 (meaning inferred) */
    /* 0x05C */ u8 field_0x5C[0xB1 - 0x5C];
    /* 0x0B1 */ u8 field_0xB1; /* no stage title while set (meaning unknown) */
    /* 0x0B2 */ u8 field_0xB2[0x150 - 0xB2];
};
SIZE_ASSERT(UnkGameState_8057D6A0, 0x150);

extern UnkGameState_8057D6A0 lbl_8057D6A0;
extern const char *const lbl_8051D528[];

/* The file system (see Game/GadgetStageData.cpp). */
struct UnkFileSystem_800371D8 {
    /* 0x0 */ u32 field_0x0;
    /* 0x4 */ bool (*exists)(const char *path, UnkFileSystem_800371D8 *fileSystem);
};

extern "C" {
UnkFileSystem_800371D8 *fn_800371D8(void);
/* Unloads the project (behaviour inferred). */
void fn_8003E490(TPjsCsd &csd);
/* Loads the project from the files under dir (behaviour inferred). */
bool fn_8003F938(TPjsCsd &csd, const char *dir, const char *path);
}

/* The stage title (class name unknown; Game/StageManager.cpp declares it under this name). */
class UnkObject_801E67EC {
  public:
    UnkObject_801E67EC();

    /* Loads the title of stage (name inferred). */
    bool Load(int stage);
    /* Advances and draws the title; false while nothing is loaded (name inferred). */
    bool Update();
    /* Starts showing the loaded title (name inferred). */
    void Show();
    /* Unloads the title and ends its task (name inferred). */
    void Stop();

    /* 0x00 */ TPjsCsd m_csd;
    /* 0x54 */ s32 m_state; /* 0 none, 1 loaded, 2 to show, 3 shown, 4 loading */
    /* 0x58 */ Task *m_task;
};
SIZE_ASSERT(UnkObject_801E67EC, 0x5C);

extern "C" UnkObject_801E67EC *fn_801E67EC(void);

/* Whether file path exists (name inferred; see Game/PjsEventActionManager.cpp). */
inline bool FileExists(const char *path) {
    UnkFileSystem_800371D8 *fileSystem = fn_800371D8();
    return fileSystem->exists(path, fileSystem);
}

class StageTitleTask : public Task {
  public:
    StageTitleTask(UnkRenderOwner_80046178 *owner) : Task((Task *)owner->field_0x3C) {
        m_name = "StageTitleTask";
    }
    virtual ~StageTitleTask();
    virtual void Tick(float dt);
};
SIZE_ASSERT(StageTitleTask, 0x28);

/* NonMatching: GC/2.7 gives `this` r31 and the string pool base r30; retail has them the other
 * way round (14 register operands differ). About ten source shapes were tried. */
bool UnkObject_801E67EC::Load(int stage) {
    char path[0x80];
    if (m_state != 0) {
        return false;
    }
    m_state = 4;
    if (lbl_8057D6A0.field_0xB1 != 0) {
        return false;
    }
    sprintf(path, "csdFiles/stageTitle/StageTitle%04d%s.txd", stage,
            lbl_8051D528[lbl_8057D6A0.m_language]);
    if (!FileExists(path)) {
        return false;
    }
    if (!fn_8003F938(m_csd, "csdFiles/stageTitle/StageTitle", path)) {
        return false;
    }
    strcpy(m_csd.m_sceneName, "StageTitle");
    fn_8003E414(m_csd, 1, 1);
    fn_8003E414(m_csd, 2, 0);
    fn_8003FB10(m_csd);
    m_state = 1;
    fn_8003FE2C(m_csd);
    return true;
}

bool UnkObject_801E67EC::Update() {
    switch (m_state) {
    case 0:
        return false;
    case 1:
        fn_8003FE2C(m_csd);
        fn_8003FDF4(m_csd);
        break;
    case 2:
        fn_8003FB10(m_csd);
        m_state = 3;
    case 3:
        if (!m_csd.m_playing) {
            Stop();
        } else {
            fn_8003FE2C(m_csd);
            fn_8003FDF4(m_csd);
        }
        break;
    case 4:
        break;
    }
    return true;
}

void UnkObject_801E67EC::Show() {
    if (m_state != 0) {
        m_state = 2;
        if (m_task == 0) {
            m_task = new StageTitleTask(fn_80046178());
        }
    }
}

void UnkObject_801E67EC::Stop() {
    fn_8003E490(m_csd);
    m_state = 0;
    if (m_task != 0) {
        m_task->m_flags |= 1;
        m_task = 0;
    }
}

UnkObject_801E67EC::UnkObject_801E67EC() {
    m_state = 0;
}

StageTitleTask::~StageTitleTask() {}

void StageTitleTask::Tick(float dt) {
    fn_801E67EC()->Update();
}
