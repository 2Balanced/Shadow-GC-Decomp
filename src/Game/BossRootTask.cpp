/* BossRootTask (class name from RTTI; see Game/BossRootTask.h): the task each boss gadget derives
 * from. Once the stage's boss data is loaded (and the stage is in mode 3) it calls Init; then, while
 * its owner record exists, it switches the boss to mode 1 while the record is active and to mode 2
 * when it is not, copies the record's placement to the boss when the record changed, and ends when
 * the record goes away. The unit also holds the bosses' shared effect table and a small setup
 * routine of the owner records. File name inferred from the class. Names marked "(name inferred)"
 * are not original. Definitions are in reverse address order (deferred inlining emits functions
 * last-defined first). */
#include "Game/BossCtrlUnit.h"
#include "Game/BossRootTask.h"
#include "Game/EnemyCharInfo.h"
#include "Game/MathTypes.h"
#include "boost/shared_ptr.hpp"
#include "size_assert.h"

/* The stage state (see CenFile.cpp). */
struct UnkObject_80074470 {
    /* 0x0 */ u8 field_0x0[0x8];
    /* 0x8 */ s32 m_mode; /* (meaning unknown) */
};

/* A part of an owner record that the record's constructor (0x800CBA..) sets up through fn_8027DE0C
 * (struct name unknown; layout from that function). */
struct UnkOwnerData_8027DE0C {
    /* 0x00 */ u32 field_0x0;
    /* 0x04 */ u32 field_0x4;
    /* 0x08 */ u32 field_0x8;
    /* 0x0C */ u8 field_0xC[0x14 - 0xC];
    /* 0x14 */ float field_0x14;
};

/* -7.0f, a constant of the unit around 0x801E028C (meaning unknown). */
extern const float lbl_805F1C08;

extern "C" {
/* The resource loader and the state of slot (see BKTankController.cpp). */
void *fn_800122D4(void);
s32 fn_80012408(void *loader, s32 slot);
/* The stage state (see CenFile.cpp). */
UnkObject_80074470 *fn_80074470(void);
/* The owner record (see BlockChange.cpp): registers the task with it, releases it, whether it is
 * active, whether it changed (and clearing that), and whether it went away (behaviour
 * inferred). */
void fn_800C9CD0(UnkPlacement_800C9CD0 *owner, Task *task);
void fn_800C9BE8(UnkPlacement_800C9CD0 *owner);
bool fn_800C9D0C(UnkPlacement_800C9CD0 *owner);
bool fn_800C9D14(UnkPlacement_800C9CD0 *owner);
void fn_800C9CF8(UnkPlacement_800C9CD0 *owner);
bool fn_800C9A64(UnkPlacement_800C9CD0 *owner);
/* The owner's position and rotation. */
void fn_800C9F14(UnkPlacement_800C9CD0 *owner, Vec3f *position);
void fn_800C9ED4(UnkPlacement_800C9CD0 *owner, Vec3f *rotation);
/* An angle brought into range (behaviour inferred; see EggLastMechAIState.cpp). */
float fn_8040E99C(float angle);
/* Defined below. */
void fn_8027DE28(BossRootTask *task);
}

/* The bosses' effects (a global the boss units fill and copy into their BossVarParam; see
 * BlackBull0210.cpp). */
boost::shared_ptr<SonicteamUSA::System::Effect::EffectLaunchInfoUSA> lbl_805F0240;

const char *BossRootTask::s_taskName = "BossRootTask";

BossRootTask::BossRootTask(Task *parent, UnkPlacement_800C9CD0 *owner, bool flag, BossVarParam *param)
    : Task(parent), m_param(param) {
    m_name = s_taskName;
    if (owner != 0) {
        m_owner = owner;
        fn_800C9CD0(m_owner, this);
    }
    m_started = 0;
    m_flag = flag;
}

BossRootTask::~BossRootTask() {
    if (m_owner != 0) {
        fn_800C9BE8(m_owner);
        m_owner = 0;
    }
}

void BossRootTask::Tick(float dt) {
    if (!m_started) {
        if (fn_80012408(fn_800122D4(), 0x16) == 3 && fn_80074470()->m_mode == 3) {
            Init();
            m_started = 1;
        } else {
            return;
        }
    }
    if (m_owner != 0) {
        if (fn_800C9D0C(m_owner)) {
            if (m_param->m_mode != 1) {
                m_param->m_nextMode = 1;
            }
            if (fn_800C9D14(m_owner)) {
                fn_8027DE28(this);
                fn_800C9CF8(m_owner);
            }
        } else if (m_param->m_mode == 1) {
            m_param->m_nextMode = 2;
        }
        if (fn_800C9A64(m_owner)) {
            m_flags |= 1;
        }
    }
}

/* Moves the boss to the owner record's placement (name inferred). */
extern "C" void fn_8027DE28(BossRootTask *task) {
    fn_800C9F14(task->m_owner, &task->m_param->m_chara->m_position);
    fn_800C9ED4(task->m_owner, &task->m_param->m_chara->m_rotation);
    float &x = task->m_param->m_chara->m_rotation.x;
    x = fn_8040E99C(x);
    float &y = task->m_param->m_chara->m_rotation.y;
    y = fn_8040E99C(y);
    float &z = task->m_param->m_chara->m_rotation.z;
    z = fn_8040E99C(z);
}

/* Sets up an owner record's data (name inferred). */
extern "C" void fn_8027DE0C(UnkOwnerData_8027DE0C *data) {
    data->field_0x0 = 0;
    data->field_0x4 = 0;
    data->field_0x8 = 0;
    data->field_0x14 = lbl_805F1C08;
}
