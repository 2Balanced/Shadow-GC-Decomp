/* EggLastMech::EggLastMechTask (class name from RTTI): the task of the EGG_LASTMECH gadget. It
 * creates the boss, an EggLastMech::EggLastMech (Game/EggLastMech.cpp), for its gadget owner and
 * runs it every frame until the owner goes away; the gadget type's load and unload callbacks load
 * and release the boss's data file. File name inferred from the class (the code follows the boss's
 * unit, with sections of its own). Names marked "(name inferred)" are not original. Definitions are
 * in reverse address order (deferred inlining emits functions last-defined first), except the type
 * record and the data file name: retail's .rodata holds "EGG_LASTMECH", the file name and then
 * "EggLastMech", the order in which they are defined here. */
#include "Game/EggLastMech.h"
#include "Game/Gadget.h"
#include "Game/GadgetClassDesc.h"
#include "Game/ResourceOneFile.h"
#include "Game/Task.h"
#include "boost/shared_ptr.hpp"
#include "size_assert.h"

extern "C" {
/* The gadget owner (see Gadget.h and BlockChange.cpp). */
void fn_800C9CD0(UnkPlacement_800C9CD0 *owner, Task *task);
bool fn_800C9A64(UnkPlacement_800C9CD0 *owner);
void fn_800C9B34(UnkPlacement_800C9CD0 *owner);
/* Setup and teardown of the boss's shared data (behaviour inferred). */
void fn_8032D9B8(void);
void fn_8032D970(void);
}

/* boost/shared_ptr.hpp only declares shared_ptr::reset, which the game has out of line; this unit
 * emits the copy for EggLastMech (0x802FDFD4; boost 1.32's definition, as in SetWeapon.cpp). */
namespace boost {
template<class T> inline void shared_ptr<T>::reset() {
    this_type().swap(*this);
}
} // namespace boost

/* The gadget type's callbacks (defined at the end). */
namespace EggLastMech {
static void Load();
static void Unload();
static void Instantiate(UnkPlacement_800C9CD0 *owner);
} // namespace EggLastMech

/* The EGG_LASTMECH type record (a central table references it). Retail has four zero bytes after
 * it, before the RTTI base list, where every other type record is followed directly by the next
 * object; the unit's record is taken to carry an extra word (meaning unknown). */
struct EggLastMechTypeInfo {
    GadgetClassDesc info;
    u32 field_0x24;
};
SIZE_ASSERT(EggLastMechTypeInfo, 0x28);

EggLastMechTypeInfo lbl_8055A7F8 = {
    {"EGG_LASTMECH", EggLastMech::Load, EggLastMech::Unload, EggLastMech::Instantiate, 0, 0x800, 0xBD, 2,
     0, 0xA, 0},
    0,
};

namespace EggLastMech {

/* EggLastMech::EggLastMechTask (class name from RTTI; vtable at 0x8055A82C): the task that owns
 * the boss for the gadget owner. */
class EggLastMechTask : public Task {
  public:
    EggLastMechTask(Task *parent, UnkPlacement_800C9CD0 *owner);
    virtual ~EggLastMechTask();
    virtual void Tick(float dt);

    /* Releases the boss and finishes the task (name inferred). */
    void Retire();

    static const char *s_taskName;

    /* 0x28 */ boost::shared_ptr<EggLastMech> m_mech;
    /* 0x30 */ UnkPlacement_800C9CD0 *m_owner;
    /* 0x34 */ u32 field_0x34;
};
SIZE_ASSERT(EggLastMechTask, 0x38);

/* The data file of resource slot 0x16 (name inferred). */
static const char s_dataFile[] = "enemy/boss/EggLastMechData.one";

const char *EggLastMechTask::s_taskName = "EggLastMech";

EggLastMechTask::EggLastMechTask(Task *parent, UnkPlacement_800C9CD0 *owner)
    : Task(parent), m_owner(0) {
    m_name = s_taskName;
    if (owner != 0) {
        m_owner = owner;
        fn_800C9CD0(m_owner, this);
        m_mech = EggLastMech::Instantiate(this);
        if (m_mech.get() != 0) {
            m_mech->Init(m_owner);
        }
    }
}

EggLastMechTask::~EggLastMechTask() {
    if (m_owner != 0) {
        fn_800C9B34(m_owner);
        m_owner = 0;
    }
}

void EggLastMechTask::Tick(float dt) {
    if (m_mech.get() != 0) {
        if (m_owner != 0 && fn_800C9A64(m_owner)) {
            Retire();
        } else {
            m_mech->Tick(dt);
        }
    }
}

void EggLastMechTask::Retire() {
    if (m_mech.get() != 0) {
        m_mech.reset();
    }
    m_flags |= 1;
}

/* Loads the boss's data file into resource slot 0x16 (name inferred). */
static void Load() {
    fn_8001299C(fn_800122D4(), 0x16, new ResourceOneFile(s_dataFile, false));
    fn_8032D9B8();
}

/* Releases the boss's data (name inferred). */
static void Unload() {
    fn_8032D970();
    fn_800127F8(fn_800122D4(), 0x16);
}

/* Creates the boss's task under task layer 14 (name inferred). */
static void Instantiate(UnkPlacement_800C9CD0 *owner) {
    new EggLastMechTask(fn_801EB304(fn_8001C4F8(), 0xE), owner);
}

} // namespace EggLastMech

