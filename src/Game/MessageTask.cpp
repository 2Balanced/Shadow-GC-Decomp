/* The hint message manager (UnkHintManager_801FB180, class name unknown) and MessageTask (class
 * name from RTTI), the task that shows its messages. Gadgets queue a hint (a message id and the
 * branch for a character) and the task shows its pages one at a time from the message source the
 * manager is attached to, starting each page's voice. File name inferred from the class. Names
 * marked "(name inferred)" are not original. Definitions are in reverse address order (deferred
 * inlining emits functions last-defined first). The unit's code reaches its .data through a pooled
 * base address, so the linker keeps that section whole. */
/* The weak copies of std::bad_alloc's functions, vtable and RTTI are in retail's .data (as in
 * BreakWall). */
#define MSL_NEW_INLINE_BAD_ALLOC
/* std::copy, and the hint list's members (see Game/UnkHintManager.h). */
#define MSL_ALGORITHM_COPY
#define UNK_HINT_LIST_MEMBERS
#include <algorithm>
#include <memory>

#include "Game/Render.h"
#include "Game/Task.h"
#include "Game/UnkHintManager.h"
#include "boost/shared_ptr.hpp"
#include "boost/weak_ptr.hpp"
#include <dolphin/types.h>
#include "size_assert.h"

/* A message of the message source (layout from its users; names inferred). */
struct UnkHintMessage_80049E1C {
    /* 0x00 */ u8 field_0x0[0x8];
    /* 0x08 */ s32 m_priority;
    /* 0x0C */ s32 m_frames; /* how long the page shows, or 0 for 180 */
    /* 0x10 */ s32 m_voice;
};

/* The message source (class name unknown; only the member used here is mapped). */
class UnkHintTarget_801FB180 {
  public:
    /* 0x00 */ u8 field_0x0[0x64];
    /* 0x64 */ float m_scale;
};

/* The game state block in .bss (only the members used here; names inferred). */
struct UnkGameState_8057D6A0 {
    /* 0x000 */ u8 field_0x0[0x58];
    /* 0x058 */ s32 field_0x58;
    /* 0x05C */ u8 field_0x5C[0xA8 - 0x5C];
    /* 0x0A8 */ u8 m_pause[0x9]; /* tested by fn_802039E8 */
    /* 0x0B1 */ u8 field_0xB1;
    /* 0x0B2 */ u8 field_0xB2[0x150 - 0xB2];
};
SIZE_ASSERT(UnkGameState_8057D6A0, 0x150);

extern UnkGameState_8057D6A0 lbl_8057D6A0;

/* The out-of-line copy of the inline accessor HintRing.cpp defines (0x80109D64). */
UnkHintManager_801FB180 *GetHintManager();

extern "C" {
/* Whether the game is paused (behaviour inferred). */
bool fn_802039E8(void *pause);
/* The sound manager, and stopping and starting a voice on it (behaviour inferred). */
void *fn_80053164(void);
void fn_801D517C(void *sound, s32 voice);
void fn_801D51A0(void *sound, s32 voice, int arg2, int arg3);
/* A page of message id (behaviour inferred). */
UnkHintMessage_80049E1C *fn_80049E1C(UnkHintTarget_801FB180 *target, s32 id, s32 branch,
                                      s32 index);
/* The same, laid out at x, y; voice receives the page's voice (behaviour inferred). */
UnkHintMessage_80049E1C *fn_80049BC0(UnkHintTarget_801FB180 *target, s32 id, s32 branch,
                                      s32 index, float x, float y, s32 *voice, u8 changed);
/* A random integer below n (behaviour inferred). */
s32 fn_8040EA90(s32 n);

/* The manager's functions other units call by these names (names inferred). */
void fn_801FAAA4(UnkHintManager_801FB180 *manager);
void fn_801FAAB4(UnkHintManager_801FB180 *manager);
s32 fn_801FAAC4(s32 character);
bool fn_801FAAC8(UnkHintManager_801FB180 *manager, s32 hint);
void fn_801FABE4(UnkHintManager_801FB180 *manager);
s32 fn_801FAC44(UnkHintManager_801FB180 *manager, s32 id, int arg2);
s32 fn_801FAF00(UnkHintManager_801FB180 *manager, s32 id, s32 branch, int arg3, s32 index);
void fn_801FAFFC(UnkHintManager_801FB180 *manager);
void fn_801FB0BC(UnkHintManager_801FB180 *manager,
                 const boost::weak_ptr<UnkHintTarget_801FB180> &target);
}

/* boost/weak_ptr.hpp only declares weak_ptr::reset, which the game has out of line; this unit
 * emits the copy for the message source (boost 1.32's definition). */
namespace boost {
template <> inline void weak_ptr<UnkHintTarget_801FB180>::reset() {
    this_type().swap(*this);
}
} // namespace boost

/* The page layouts (x, y, scale for field_0x58 nonzero, scale otherwise; names inferred): normal,
 * for field_0xB1, and for the manager's m_altLayout. Tick reaches them through one pooled base
 * address, so the linker keeps this unit's .data whole. */
static float s_layoutNormal[4] = {0.56f, 0.18f, 0.038f, 0.04f};
static float s_layoutB1[4] = {0.55f, 0.22f, 0.038f, 0.04f};
static float s_layoutAlt[4] = {0.61f, 0.18f, 0.04f, 0.04f};

class MessageTask : public Task {
  public:
    MessageTask(Task *parent);
    virtual ~MessageTask();
    virtual void Tick(float dt);

    /* 0x28 */ float m_timer; /* how long the page has shown, in seconds */
    /* 0x2C */ u8 m_changed; /* the page is of another message than the last */
    /* 0x30 */ UnkHintMessage_80049E1C *m_message;
    /* 0x34 */ float m_x;
    /* 0x38 */ float m_y;
    /* 0x3C */ float m_scale;
};
SIZE_ASSERT(MessageTask, 0x40);

MessageTask::MessageTask(Task *parent) : Task(parent), m_timer(0.0f), m_changed(0), m_message(0) {
    m_name = "MessageTask";
    fn_801FABE4(GetHintManager());
    GetHintManager()->SetTaskActive();
    m_flags |= 0x100;
}

MessageTask::~MessageTask() {
    fn_801FABE4(GetHintManager());
}

void MessageTask::Tick(float dt) {
    if (fn_802039E8(lbl_8057D6A0.m_pause)) {
        return;
    }
    float *layout;
    if (GetHintManager()->m_altLayout) {
        layout = s_layoutAlt;
    } else if (lbl_8057D6A0.field_0xB1) {
        layout = s_layoutB1;
    } else {
        layout = s_layoutNormal;
    }
    m_x = layout[0];
    m_y = layout[1];
    if (lbl_8057D6A0.field_0x58 == 0) {
        m_scale = layout[3];
    } else {
        m_scale = layout[2];
    }

    UnkHintManager_801FB180 *manager = GetHintManager();
    boost::shared_ptr<UnkHintTarget_801FB180> target = manager->m_target.lock();
    if (target) {
        bool started = false;
        if (manager->m_hints.empty() && manager->m_current.id < 0) {
            GetHintManager()->m_priority = -1;
            return;
        }
        UnkHintEntry_801FB180 *next = manager->m_hints.begin();
        if (next != manager->m_hints.end()) {
            UnkHintMessage_80049E1C *message =
                fn_80049E1C(target.get(), next->id, next->branch, next->index);
            if (message != 0) {
                if (manager->m_current.id == -1 || m_message == 0 ||
                    message->m_priority > m_message->m_priority ||
                    (manager->m_current.id != next->id &&
                     message->m_priority == m_message->m_priority)) {
                    if (manager->m_current.id != next->id) {
                        m_changed = 1;
                    } else {
                        m_changed = 0;
                    }
                    manager->m_current = *next;
                    started = true;
                    m_timer = 0.0f;
                    manager->m_hints.erase(next);
                } else if (message->m_priority < m_message->m_priority) {
                    manager->m_hints.erase(next);
                }
            } else {
                manager->m_hints.erase(next);
            }
        }
        if (manager->m_current.id < 0) {
            GetHintManager()->m_priority = -1;
            return;
        }
        target->m_scale = m_scale;
        s32 voice;
        if (manager->m_taskActive) {
            m_message = fn_80049BC0(target.get(), manager->m_current.id, manager->m_current.branch,
                                    manager->m_current.index, m_x, m_y, &voice, m_changed);
        } else {
            m_message = fn_80049E1C(target.get(), manager->m_current.id, manager->m_current.branch,
                                    manager->m_current.index);
            voice = m_message->m_voice;
        }
        if (m_message != 0) {
            s32 frames = m_message->m_frames;
            if (frames == 0) {
                frames = 180;
            }
            GetHintManager()->m_priority = m_message->m_priority;
            m_timer += dt;
            if (m_timer >= frames / 60.0f) {
                manager->m_current.id = -1;
            }
            if (started && voice >= 0) {
                fn_801D517C(fn_80053164(), manager->m_voice);
                manager->m_voice = voice;
                fn_801D51A0(fn_80053164(), voice, -1, -1);
            }
        } else {
            manager->m_current.id = -1;
        }
    }
}

UnkHintManager_801FB180::UnkHintManager_801FB180() {
    m_hints.reserve(0x40);
}

/* Attaches the manager to a message source and starts a MessageTask (name inferred). */
void fn_801FB0BC(UnkHintManager_801FB180 *manager,
                 const boost::weak_ptr<UnkHintTarget_801FB180> &target) {
    fn_801FAFFC(manager);
    manager->m_target = target;
    manager->m_task = new MessageTask(static_cast<Task *>(fn_80046178()->field_0x54));
    manager->m_current.id = -1;
    manager->m_voice = -1;
    manager->m_lock = 0;
    manager->m_altLayout = 0;
    manager->m_priority = -1;
}

/* Ends the MessageTask and detaches the message source (name inferred). */
void fn_801FAFFC(UnkHintManager_801FB180 *manager) {
    if (manager->m_task != 0) {
        manager->m_task->m_flags |= 1;
        manager->m_task = 0;
    }
    manager->m_target.reset();
}

/* Shows the pages of message id of branch from page index on, unless a message of higher priority
 * shows; returns the hint, or -1 (name inferred; arg3 is not used). */
s32 fn_801FAF00(UnkHintManager_801FB180 *manager, s32 id, s32 branch, int arg3, s32 index) {
    if (manager->m_lock != 0) {
        return -1;
    }
    boost::shared_ptr<UnkHintTarget_801FB180> target = manager->m_target.lock();
    if (target) {
        UnkHintMessage_80049E1C *message = fn_80049E1C(target.get(), id, branch, 0);
        if (message == 0 || message->m_priority < manager->m_priority) {
            return -1;
        }
    }
    fn_801FABE4(manager);
    for (; index < 0x24; index++) {
        UnkHintEntry_801FB180 entry(id, branch, index);
        manager->m_hints.push_back(entry);
    }
    return id * 1000 + branch * 100;
}

/* Shows one random page of message id (branch 0); returns the hint, or -1 (name inferred; arg2 is
 * not used). */
s32 fn_801FAC44(UnkHintManager_801FB180 *manager, s32 id, int arg2) {
    if (manager->m_lock != 0) {
        return -1;
    }
    boost::shared_ptr<UnkHintTarget_801FB180> target = manager->m_target.lock();
    s32 count;
    if (target) {
        for (count = 0; count < 0x24; count++) {
            if (fn_80049E1C(target.get(), id, 0, count) == 0) {
                break;
            }
        }
    } else {
        return -1;
    }
    if (count == 0x24) {
        return -1;
    }
    if (target) {
        UnkHintMessage_80049E1C *message = fn_80049E1C(target.get(), id, 0, 0);
        if (message == 0 || message->m_priority < manager->m_priority) {
            return -1;
        }
    }
    UnkHintEntry_801FB180 entry(id, 0, fn_8040EA90(count));
    manager->m_hints.push_back(entry);
    return id * 1000;
}

/* Drops the queued pages and stops the voice (name inferred). */
void fn_801FABE4(UnkHintManager_801FB180 *manager) {
    manager->m_hints.clear();
    fn_801D517C(fn_80053164(), manager->m_voice);
    manager->m_current.id = -1;
}

/* Whether hint is over: neither shown nor queued (for a negative hint, whether pages are queued;
 * name inferred). */
bool fn_801FAAC8(UnkHintManager_801FB180 *manager, s32 hint) {
    if (hint < 0) {
        return !manager->m_hints.empty();
    }
    if (manager->m_current.id * 1000 + manager->m_current.branch * 100 == hint) {
        return false;
    }
    for (UnkHintEntry_801FB180 *entry = manager->m_hints.begin(); entry != manager->m_hints.end();
         ++entry) {
        if (entry->id * 1000 + entry->branch * 100 == hint) {
            return false;
        }
    }
    return true;
}

/* The message branch for character (name inferred). */
s32 fn_801FAAC4(s32 character) {
    return character;
}

/* Accept and refuse new hints (names inferred). */
void fn_801FAAB4(UnkHintManager_801FB180 *manager) {
    manager->m_lock++;
}

void fn_801FAAA4(UnkHintManager_801FB180 *manager) {
    manager->m_lock--;
}
