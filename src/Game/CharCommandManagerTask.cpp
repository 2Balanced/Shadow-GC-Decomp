/* CharCommandManagerTask (class name from RTTI) and the command queues it runs. Commands sent to a
 * command unit (fn_801D9C0C, Game/CharCommand.cpp) are filed by priority level (0..6) in one of
 * two banks of queues; once a frame the task delivers the commands of the current bank level by
 * level to their units' callbacks (Game/CharCommandUnit.cpp) and deletes them, while commands sent
 * during the delivery go to the other bank. The queue manager's class name is unknown
 * (fn_8009978C returns its static instance; names marked "(name inferred)" are not original).
 * Definitions are in reverse address order (deferred inlining emits functions last-defined
 * first). */
#include <string>

#define CHARCOMMAND_IMPLEMENTATION
#include "Game/CharCommand.h"
#include "Game/CharCommandUnit.h"
#include "Game/Task.h"
#include "boost/shared_ptr.hpp"
#include "boost/weak_ptr.hpp"
#include <dolphin/types.h>
#include "size_assert.h"

extern "C" {
/* A 32-bit flag word (see Game/SoundManager.h). */
void fn_80014E04(u32 *flags);
void fn_80014E1C(u32 *flags, int bit);
void fn_8006127C(u32 *flags, int bit);
bool fn_80014E34(u32 *flags, int bit);
/* The stage's switches, and the first of a list of them a command's index selects (behaviour
 * inferred; see Game/ContinueAction.cpp). */
struct UnkSwitch_800CB560 {
    /* 0x0 */ u8 field_0x0[0x8];
    /* 0x8 */ UnkSwitch_800CB560 *m_next;
};
void *fn_800C9B84(void);
UnkSwitch_800CB560 *fn_800CB560(void *switches, u8 index);
}

/* Calls callback with command unless it is empty: an inline function of Game/EnemyCharInfo.cpp,
 * whose out-of-line copy (0x8019FEF8) this unit calls. */
void CallCommandCallback(const CharCommandCallback &callback, CharCommand &command);

/* The parts of the MSL <vector> the queues use: vector<T *> is built on vector<void *>, whose
 * out-of-line functions the game shares, through a layer of its own (as in Game/csdMatrix.cpp; the
 * layer's name is invented). The bodies of the shared functions are placeholders: the game's
 * copies are the ones linked. */
namespace std {

template <class T, class Allocator = allocator<T> > class vector;

/* An inline call that keeps the shared vector<void *> functions from being inlined. */
inline void __vec_nop() {}

template <> class vector<void *, allocator<void *> > {
  public:
    vector() {
        __vec_nop();
    }
    ~vector() {
        __vec_nop();
    }
    void reserve(size_t n) {
        __vec_nop();
    }
    void push_back(void *const &x) {
        __vec_nop();
    }
    void **begin() {
        __vec_nop();
        return m_data;
    }
    void **end() {
        __vec_nop();
        return m_data + m_size;
    }
    size_t size() const {
        return m_size;
    }
    void clear() {
        m_size = 0;
    }

    /* 0x0 */ size_t m_capacity;
    /* 0x4 */ size_t m_size;
    /* 0x8 */ void **m_data;
};

template <class T> class __vec_ptr_base : public vector<void *, allocator<void *> > {
  public:
    typedef vector<void *, allocator<void *> > base;

    __vec_ptr_base() {}
    ~__vec_ptr_base() {}
    void push_back(T *const &x) {
        base::push_back((void *const &)x);
    }
    T **begin() {
        return (T **)base::begin();
    }
    T **end() {
        return (T **)base::end();
    }
    size_t size() const {
        return base::size();
    }
    void clear() {
        base::clear();
    }
};

template <class T> class vector<T *, allocator<T *> > : public __vec_ptr_base<T> {
  public:
    typedef __vec_ptr_base<T> base;

    vector() {}
    ~vector() {}
    void reserve(size_t n) {
        vector<void *, allocator<void *> >::reserve(n);
    }
    void push_back(T *const &x) {
        base::push_back(x);
    }
    T **begin() {
        return base::begin();
    }
    T **end() {
        return base::end();
    }
    size_t size() const {
        return base::size();
    }
    void clear() {
        base::clear();
    }
};

} // namespace std

/* The manager's flag word: bit 0 is set while the task runs; commands are taken only then
 * (meaning inferred). */
struct UnkCommandFlags {
    UnkCommandFlags() {
        fn_80014E04(&m_bits);
    }
    void Set(int bit) {
        fn_80014E1C(&m_bits, bit);
    }
    void Clear(int bit) {
        fn_8006127C(&m_bits, bit);
    }
    bool Test(int bit) {
        return fn_80014E34(&m_bits, bit);
    }

    u32 m_bits;
};

/* The command queues (class name unknown; a static object of fn_8009978C). */
class UnkCharCommandManager_8009978C {
  public:
    UnkCharCommandManager_8009978C();
    ~UnkCharCommandManager_8009978C();
    /* Delivers and deletes the commands of the current bank (name inferred). */
    void Dispatch(float dt);
    /* Deletes the commands of level in the current bank (name inferred). */
    void Clear(int level);
    /* Files command for unit at its level, or deletes it while the task does not run (name
     * inferred). */
    bool Send(CharCommand *command, boost::shared_ptr<CharCommandUnit> unit, u8 set, u8 mask);

    /* 0x00 */ bool m_dispatching;
    /* 0x04 */ s32 m_bank;
    /* 0x08 */ s32 m_level; /* the level being delivered; 7 between deliveries */
    /* 0x0C */ std::vector<CharCommand *> m_queues[2][7];
    /* 0xB4 */ UnkCommandFlags m_flags;
};
SIZE_ASSERT(UnkCharCommandManager_8009978C, 0xB8);

extern "C" {
UnkCharCommandManager_8009978C *fn_8009978C(void);
}

class CharCommandManagerTask : public Task {
  public:
    CharCommandManagerTask(Task *parent);
    virtual ~CharCommandManagerTask();
    virtual void Tick(float dt);

    static const char *s_taskName;
};
SIZE_ASSERT(CharCommandManagerTask, 0x28);

UnkCharCommandManager_8009978C::UnkCharCommandManager_8009978C() {
    for (int bank = 0; bank < 2; bank++) {
        for (int level = 0; level < 7; level++) {
            m_queues[bank][level].reserve(0x40);
        }
    }
}

UnkCharCommandManager_8009978C::~UnkCharCommandManager_8009978C() {}

void UnkCharCommandManager_8009978C::Dispatch(float dt) {
    m_dispatching = true;
    for (int level = 0; level < 7; level++) {
        m_level = level;
        CharCommand **it = m_queues[m_bank][level].begin();
        while (it != m_queues[m_bank][level].end()) {
            CharCommand *command = *it++;
            if (command != 0) {
                if (!(command->m_flags & 1)) {
                    if (command->m_flags & 4) {
                        if (boost::shared_ptr<CharCommandUnit> unit = command->m_receiver.lock()) {
                            CallCommandCallback(unit->m_callback.m_function, *command);
                        }
                    } else {
                        for (UnkSwitch_800CB560 *node =
                                 fn_800CB560(fn_800C9B84(), command->field_0x6);
                             node != 0; node = node->m_next) {
                        }
                    }
                }
                delete command;
            }
        }
        /* The count is read and not used (debug code left out). */
        m_queues[m_bank][level].size();
        m_queues[m_bank][level].clear();
    }
    m_level = 7;
    m_bank = m_bank == 0 ? 1 : 0;
    m_dispatching = false;
}

void UnkCharCommandManager_8009978C::Clear(int level) {
    for (CharCommand **it = m_queues[m_bank][level].begin(); it != m_queues[m_bank][level].end();
         it++) {
        delete *it;
    }
    m_queues[m_bank][level].clear();
}

bool UnkCharCommandManager_8009978C::Send(CharCommand *command,
                                          boost::shared_ptr<CharCommandUnit> unit, u8 set,
                                          u8 mask) {
    if (!m_flags.Test(0)) {
        delete command;
        return false;
    }
    /* Retail keeps the command in a register here and reloads the parameter, whose address the
     * queue takes, for the rest. */
    CharCommand *sent = command;
    sent->m_receiver = unit;
    sent->m_flags |= set;
    sent->m_flags &= mask;
    if (m_dispatching) {
        if (m_bank == 0) {
            m_queues[1][command->m_priority].push_back(command);
        } else {
            m_queues[0][command->m_priority].push_back(command);
        }
    } else {
        m_queues[m_bank][command->m_priority].push_back(command);
    }
    return true;
}

/* Deletes the commands of level unless a level below it is being delivered (name inferred). */
extern "C" void fn_801D9F00(UnkCharCommandManager_8009978C *manager, int level) {
    if (manager->m_level == 7) {
        manager->Clear(level);
    } else if (level > manager->m_level) {
        manager->Clear(level);
    }
}

/* Empties every queue (name inferred; see Game/StageManager.cpp). */
extern "C" void fn_801D9E4C(UnkCharCommandManager_8009978C *manager) {
    manager->m_level = 7;
    for (int bank = 0; bank < 2; bank++) {
        for (int level = 0; level < 7; level++) {
            manager->m_queues[bank][level].clear();
        }
    }
    manager->m_dispatching = false;
    manager->m_bank = 0;
    manager->m_flags.Clear(0);
}

/* Deletes every command of the current bank and empties every queue (name inferred). */
extern "C" void fn_801D9E00(UnkCharCommandManager_8009978C *manager) {
    for (int level = 0; level < 7; level++) {
        manager->Clear(level);
    }
    fn_801D9E4C(manager);
}

const char *CharCommandManagerTask::s_taskName = "CharCommandManagerTask";

CharCommandManagerTask::CharCommandManagerTask(Task *parent) : Task(parent) {
    m_name = s_taskName;
    m_flags |= 0x100;
    fn_80014E1C(&fn_8009978C()->m_flags.m_bits, 0);
}

CharCommandManagerTask::~CharCommandManagerTask() {
    fn_8006127C(&fn_8009978C()->m_flags.m_bits, 0);
}

void CharCommandManagerTask::Tick(float dt) {
    fn_8009978C()->Dispatch(dt);
}
