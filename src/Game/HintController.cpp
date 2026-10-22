/* HintController (class name from RTTI: HintController::HintController): the HINT CONTROLLER
 * gadget. Its type's load hook reads the stage's hint script, stg%04d/stg%04d_hint.bin: entries of a
 * hint id, a branch and a delay, ended by id -1. The gadget plays the script in order, showing each
 * hint after its delay (in the branch of the current character when the entry's branch is
 * negative), and ends itself after the last one. File name inferred from the class. Method and
 * function names are inferred. Definitions are in reverse address order (deferred inlining emits
 * functions last-defined first). */
#include "Game/Gadget.h"
#include "Game/GadgetCharInfo.h"
#include "Game/GadgetClassDesc.h"
#include "boost/shared_ptr.hpp"
#include "boost/weak_ptr.hpp"
#include <dolphin/types.h>
#include "size_assert.h"

/* The game state block in .bss (only the members used here; see EnemyHint.cpp). */
struct UnkGameState_8057D6A0 {
    /* 0x000 */ u8 field_0x0[0xA8];
    /* 0x0A8 */ s32 m_stageId; /* the stage number in file names (meaning inferred) */
    /* 0x0AC */ u8 field_0xAC[0x150 - 0xAC];
};
SIZE_ASSERT(UnkGameState_8057D6A0, 0x150);

extern UnkGameState_8057D6A0 lbl_8057D6A0;

/* The hint manager (class name unknown; see HintRing.cpp and EnemyHint.cpp). */
class UnkHintManager_801FB180;

/* The out-of-line copy of the inline accessor HintRing.cpp defines (0x80109D64). */
UnkHintManager_801FB180 *GetHintManager();

/* The player manager fn_8006D6A8 returns (see BDHologram.cpp); only the member read here is laid
 * out. */
class UnkPlayerManager_8006D6A8 {
  public:
    /* 0x00 */ u8 field_0x0[0x20];
    /* 0x20 */ s32 m_character; /* the character played (meaning inferred) */
};

/* The file system (class name unknown; see FogTable.cpp). */
class UnkFileSystem_800371D8;

/* The reader of a loaded hint script (struct name unknown; a 0x28-byte object built by
 * fn_8040E61C, see StageManager.cpp). */
struct UnkPath_8040E61C {
    /* 0x00 */ u8 field_0x0[0x14];
    /* 0x14 */ void *m_nodes;
    /* 0x18 */ u8 field_0x18[0x28 - 0x18];
};

extern "C" {
/* Shows message id of branch (see HintRing.cpp). */
s32 fn_801FAF00(UnkHintManager_801FB180 *manager, s32 id, s32 branch, int arg3, int arg4);
UnkPlayerManager_8006D6A8 *fn_8006D6A8(void);
/* Ends a gadget (see BAMine.cpp). */
void fn_8015A794(Gadget *gadget);
/* Loads a file and frees loaded data (behaviour inferred; see FogTable.cpp). */
UnkFileSystem_800371D8 *fn_800371D8(void);
void *fn_8004509C(UnkFileSystem_800371D8 *fileSystem, const char *path, u32 *size);
void fn_80049B34(void *data);
/* Builds and destroys the script's reader (see StageManager.cpp). */
UnkPath_8040E61C *fn_8040E61C(UnkPath_8040E61C *path, void *data);
void fn_8040E57C(UnkPath_8040E61C *path, int free);
int sprintf(char *buffer, const char *format, ...);
}

/* One entry of the hint script (struct name unknown; names inferred). */
struct HintScriptEntry {
    /* 0x0 */ s32 m_id; /* -1 ends the script */
    /* 0x4 */ s32 m_branch; /* negative: the current character's */
    /* 0x8 */ float m_delay;
};
SIZE_ASSERT(HintScriptEntry, 0xC);

/* The loaded script: the file, its reader and the entries. */
static void *s_file;
static UnkPath_8040E61C *s_reader;
static const HintScriptEntry *s_script;

namespace HintController {

class HintController : public Gadget {
  public:
    HintController(Task *parent, UnkPlacement_800C9CD0 *owner);
    virtual ~HintController();
    virtual void Behave(float dt);
    virtual void fn_800CBC1C();
    virtual void ReloadPlacement(UnkPlacement_800C9CD0 *owner);

    static void Instantiate(UnkPlacement_800C9CD0 *owner);

    /* Copies the owner's position and rotation and restarts the script. */
    void LoadFromOwner(UnkPlacement_800C9CD0 *owner);

    static const char *s_taskName;

    /* 0x30 */ boost::shared_ptr<GadgetCharInfo> m_chara;
    /* 0x38 */ s32 m_state; /* 0 next entry, 1 waiting, 2 done */
    /* 0x3C */ s32 m_index;
    /* 0x40 */ const HintScriptEntry *m_entry;
    /* 0x44 */ float m_timer;
};
SIZE_ASSERT(HintController, 0x48);

const char *HintController::s_taskName = "HintController";

HintController::HintController(Task *parent, UnkPlacement_800C9CD0 *owner) : Gadget(parent, owner) {
    m_name = s_taskName;
    m_chara = fn_8015CB88(0, 4, s_taskName);
    LoadFromOwner(owner);
}

HintController::~HintController() {}

void HintController::Behave(float dt) {
    switch (m_state) {
    case 0: {
        const HintScriptEntry *entry = &s_script[m_index];
        if (entry->m_id != -1) {
            m_entry = entry;
            m_timer = m_entry->m_delay;
            m_index++;
            m_state = 1;
        } else {
            m_state = 2;
        }
        break;
    }
    case 1:
        m_timer -= dt;
        if (m_timer <= 0.0f) {
            const HintScriptEntry *entry = m_entry;
            if (entry->m_branch >= 0) {
                fn_801FAF00(GetHintManager(), entry->m_id, entry->m_branch, 0, 0);
            } else {
                switch (fn_8006D6A8()->m_character) {
                case 1:
                    fn_801FAF00(GetHintManager(), m_entry->m_id, 1, 0, 0);
                    break;
                case 2:
                    fn_801FAF00(GetHintManager(), m_entry->m_id, 2, 0, 0);
                    break;
                case 0:
                    fn_801FAF00(GetHintManager(), m_entry->m_id, 0, 0, 0);
                    break;
                }
            }
            m_state = 0;
        }
        break;
    case 2:
        fn_8015A794(this);
        m_flags |= 1;
        break;
    }
}

void HintController::fn_800CBC1C() {}

void HintController::ReloadPlacement(UnkPlacement_800C9CD0 *owner) {}

void HintController::LoadFromOwner(UnkPlacement_800C9CD0 *owner) {
    fn_800C9F14(owner, &m_chara->m_position);
    fn_800C9ED4(owner, &m_chara->m_rotation);
    m_state = 0;
    m_timer = 0.0f;
    m_entry = 0;
    m_index = 0;
}

} // namespace HintController

extern "C" void fn_802F2E44(void) {
    static const char *s_path = "stg%04d/stg%04d_hint.bin";
    char path[0x40];
    u32 size;
    sprintf(path, s_path, lbl_8057D6A0.m_stageId, lbl_8057D6A0.m_stageId);
    s_file = 0;
    s_reader = 0;
    s_script = 0;
    if ((s_file = fn_8004509C(fn_800371D8(), path, &size)) != 0) {
        UnkPath_8040E61C *reader = (UnkPath_8040E61C *)operator new(0x28);
        if (reader != 0) {
            reader = fn_8040E61C(reader, s_file);
        }
        s_reader = reader;
        s_script = (const HintScriptEntry *)reader->m_nodes;
    }
}

extern "C" void fn_802F2DEC(void) {
    if (s_reader != 0) {
        fn_8040E57C(s_reader, 1);
        s_reader = 0;
    }
    if (s_file != 0) {
        fn_80049B34(s_file);
        s_file = 0;
    }
    s_script = 0;
}

void HintController::HintController::Instantiate(UnkPlacement_800C9CD0 *owner) {
    Task *parent = fn_801EB304(fn_8001C4F8(), 11);
    new HintController(parent, owner);
}

GadgetClassDesc lbl_80559FC0 = {
    "HINT CONTROLLER", fn_802F2E44, fn_802F2DEC, HintController::HintController::Instantiate, 0, 0x800, 0x2596, 1, 0,
    10, 0,
};
