/* DiablonSet (class @unnamed@DiablonSetData_cpp@::DiablonSet from RTTI, which also gives the file
 * name): the gadget that sets up the Diablon boss. It loads the boss's resources
 * ("enemy/boss/diablon.one" in resource slot 0x16) and, once they are in, creates the boss with
 * the tuning table of the stage at the record's position and rotation, and makes the boss its own
 * second character when character 13 is not there. Destroying it ends the boss's tasks. Names
 * marked "(name inferred)" are not original. Definitions are in reverse address order (deferred
 * inlining emits functions last-defined first). */
#include "Game/CharInfo.h"
#include "Game/CharacterFiles.h"
#include "Game/Diablon.h"
#include "Game/Gadget.h"
#include "Game/GadgetClassDesc.h"
#include "Game/MathTypes.h"
#include "Game/Npc.h"
#include "Game/ResourceOneFile.h"
#include "Game/Task.h"
#include "boost/shared_ptr.hpp"
#include "boost/weak_ptr.hpp"
#include "size_assert.h"

/* boost/weak_ptr.hpp only declares weak_ptr::reset, which the game has out of line; this unit
 * emits the copy for the boss (boost 1.32's definition; as in Game/BaseDoor.cpp). */
namespace boost {
template <> inline void weak_ptr<Boss::Diablon::Diablon>::reset() {
    this_type().swap(*this);
}
} // namespace boost

/* The game state block in .bss (layout of the word this unit reads only). */
struct UnkGameState_8057D6A0 {
    /* 0x00 */ u8 field_0x0[0xA8];
    /* 0xA8 */ s32 m_stage;
    /* 0xAC */ u8 field_0xAC[0x150 - 0xAC];
};
SIZE_ASSERT(UnkGameState_8057D6A0, 0x150);

class UnkCharManager_8006F754;

extern "C" {
extern UnkGameState_8057D6A0 lbl_8057D6A0;

/* The state of resource slot (3 once loaded), and whether the boss's other files are in
 * (behaviour inferred). */
int fn_80012408(void *loader, int slot);
u8 fn_801D2C64(int arg0);
/* Loads and unloads the boss's other files (behaviour inferred). */
void fn_801D2DE8(int arg0);
void fn_801D2D14(int arg0);

/* The character manager and the character of an id in it (see Game/SetCharacter.cpp). */
UnkCharManager_8006F754 *fn_8006F754(void);
boost::weak_ptr<CharInfo> fn_8007488C(UnkCharManager_8006F754 *manager, int character);
/* A weak pointer to a character (behaviour inferred). */
boost::weak_ptr<CharInfo> fn_80293C98(boost::shared_ptr<CharInfo> chara);
/* Names a character info (see Game/Bomb.cpp). */
void fn_801DC984(CharInfo *charInfo, const char *name);
/* Marks the owner done (see Game/BlockChange.cpp). */
void fn_800C9BE8(UnkPlacement_800C9CD0 *owner);

/* The task registry, and adding and removing a task (see Game/NpcNameDisplay.cpp). */
void *fn_80014B90(void);
void fn_8025D144(void *registry, Task *task);
void fn_8025D1C4(void *registry, Task *task, int arg2, const char *name);

}

/* The record of a gadget type, as GadgetClassDesc but with a function at 0x10 (struct name
 * unknown). */
struct UnkGadgetClassDesc_80558BE0 {
    /* 0x00 */ const char *typeName;
    /* 0x04 */ void (*loadFiles)(void);
    /* 0x08 */ void (*dropFiles)(void);
    /* 0x0C */ void (*spawn)(UnkPlacement_800C9CD0 *owner);
    /* 0x10 */ void (*field_0x10)(void);
    /* 0x14 */ u32 field_0x14;
    /* 0x18 */ u16 typeId;
    /* 0x1A */ u8 field_0x1A;
    /* 0x1B */ u8 field_0x1B;
    /* 0x1C */ u8 field_0x1C;
    /* 0x20 */ GadgetParamDesc *paramTable;
};
SIZE_ASSERT(UnkGadgetClassDesc_80558BE0, 0x24);

/* Ends a task (name inferred; Task flag 1). */
inline void EndTask(Task *task) {
    task->m_flags |= 1;
}

namespace {

class DiablonSet : public Gadget {
  public:
    DiablonSet(Task *parent, UnkPlacement_800C9CD0 *owner);
    virtual ~DiablonSet();
    virtual void Behave(float dt);
    virtual void ReloadPlacement(UnkPlacement_800C9CD0 *owner);

    /* Names inferred. */
    static bool IsLoaded();
    void SetOtherCharacter();
    void CreateBoss();
    void ReleaseBoss();

    static void LoadFiles();
    static void DropFiles();
    static void Instantiate(UnkPlacement_800C9CD0 *owner);

    static const char *s_taskName;

    /* 0x30 */ s32 m_state;
    /* 0x34 */ boost::weak_ptr<Boss::Diablon::Diablon> m_boss;
    /* 0x3C */ u32 field_0x3C; /* never touched (size from the allocation) */
};
SIZE_ASSERT(DiablonSet, 0x40);

const char *DiablonSet::s_taskName = "Diablon";

DiablonSet::DiablonSet(Task *parent, UnkPlacement_800C9CD0 *owner) : Gadget(parent, owner) {
    m_name = "DiablonSet";
    m_state = 0;
    fn_8025D1C4(fn_80014B90(), this, 0, "Diablon");
    fn_8025D144(fn_80014B90(), this);
}

DiablonSet::~DiablonSet() {
    if (m_owner != 0) {
        fn_800C9BE8(m_owner);
    }
    ReleaseBoss();
}

void DiablonSet::Behave(float dt) {
    switch (m_state) {
    case 0:
        if (IsLoaded()) {
            CreateBoss();
            SetOtherCharacter();
            m_state++;
        }
        break;
    }
}

void DiablonSet::ReloadPlacement(UnkPlacement_800C9CD0 *owner) {}

bool DiablonSet::IsLoaded() {
    if (fn_80012408(fn_800122D4(), 0x16) != 3) {
        return false;
    }
    if (fn_80012408(fn_800122D4(), 0x16) != 3) {
        return false;
    }
    return (bool)fn_801D2C64(1);
}

void DiablonSet::SetOtherCharacter() {
    boost::weak_ptr<CharInfo> other = fn_8007488C(fn_8006F754(), 13);
    if (other.expired()) {
        boost::shared_ptr<CharInfo> chara = fn_80293C98(m_boss.lock()).lock();
        m_boss.lock()->field_0x78 = chara;
        chara->PushCommand(new Player::Npc::ShowCommand(0, 4));
    }
}

void DiablonSet::CreateBoss() {
    if (m_boss.expired()) {
        Boss::Diablon::DiablonParams *params;
        switch (lbl_8057D6A0.m_stage) {
        case 0x263:
            params = &Boss::Diablon::g_params611;
            break;
        case 0x265:
            params = &Boss::Diablon::g_params613;
            break;
        case 0x264:
        default:
            params = &Boss::Diablon::g_paramsDefault;
            break;
        }
        boost::shared_ptr<Boss::Diablon::Diablon> boss;
        boss = Boss::Diablon::Diablon::Create(this, 0x16, params);
        fn_801DC984(boss.get(), s_taskName);
        m_boss = boss;
        if (m_owner != 0) {
            Vec3f position;
            Vec3f rotation(0.0f, 0.0f, 0.0f);
            fn_800C9F14(m_owner, &position);
            fn_800C9ED4(m_owner, &rotation);
            fn_800091D0(&boss->m_position, &position);
            boss->m_rotation = rotation;
        }
    }
}

void DiablonSet::ReleaseBoss() {
    if (!m_boss.expired()) {
        boost::shared_ptr<Boss::Diablon::Diablon> boss = m_boss.lock();
        if (boss.get()->m_controller != 0) {
            EndTask((Task *)boss->m_controller);
        }
        if (boss.get()->m_executer != 0) {
            EndTask((Task *)boss->m_executer);
        }
        m_boss.reset();
    }
}

void DiablonSet::LoadFiles() {
    ResourceOneFile *file = new ResourceOneFile("enemy/boss/diablon.one", false);
    fn_8001299C(fn_800122D4(), 0x16, file);
    fn_801D2DE8(1);
    fn_80078C88()->Load(0);
}

void DiablonSet::DropFiles() {
    fn_800127F8(fn_800122D4(), 0x16);
    fn_801D2D14(1);
    fn_80078C88()->Unload(0);
}

void DiablonSet::Instantiate(UnkPlacement_800C9CD0 *owner) {
    new DiablonSet(fn_801EB304(fn_8001C4F8(), 14), owner);
}

} // namespace

extern "C" void fn_802D8BAC(void) {}

/* The record of the DIABLON gadget type (name inferred). */
UnkGadgetClassDesc_80558BE0 lbl_80558BE0 = {
    DiablonSet::s_taskName, DiablonSet::LoadFiles, DiablonSet::DropFiles, DiablonSet::Instantiate,
    fn_802D8BAC, 0, 0xBC, 2, 0, 10, 0,
};
