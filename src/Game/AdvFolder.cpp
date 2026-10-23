/* AdvReal::AdvFolder (class name from RTTI; its task name is the same string): the front-end task.
 * It sets up the front-end managers, picks the first phase from the stage sequence that just ended
 * (none: the memory card check; a story: the story library, ...) and then runs the phase manager
 * every frame. This unit also holds the phase manager's functions (see Game/AdvPhaseManager.h),
 * which create each phase through a shared_ptr<AdvBase>. File name from the class. Names marked
 * "(name inferred)" are not original. Definitions are in reverse address order (deferred inlining
 * emits functions last-defined first).
 *
 * Every function and section matches in objdiff, but the unit stays NonMatching: retail places the
 * first RTTI name string the compiler emits after the source is read (the control block name for
 * Title::Phase, 0x804FE030) on an 8-byte boundary, leaving a 4-byte gap after "AdvReal::AdvFolder".
 * GC/2.7 aligns it to 4, so the linked .rodata is 8 bytes short. Game/Mission.cpp shows the same
 * gap before its first RTTI name (see docs/research-notes.md on the newer game compiler). */
#include "Game/AdvBase.h"
#include "Game/AdvPhaseManager.h"
#include "Game/StageSequence.h"
#include "Game/Task.h"
#include "boost/shared_ptr.hpp"
#include <dolphin/types.h>
#include "size_assert.h"

/* boost/shared_ptr.hpp only declares shared_ptr::reset, which the game has out of line; this unit
 * emits the copy for AdvBase (boost 1.32's definition). */
namespace boost {
template <> inline void shared_ptr<AdvReal::AdvBase>::reset() {
    this_type().swap(*this);
}
} // namespace boost

/* The phases the manager creates (class names from the RTTI of their shared_ptr control blocks),
 * declared as far as creating and deleting them needs. Each is 8 bytes. */
namespace AdvReal {

#define ADV_DECLARE_PHASE(ctorArgs)                                                                 \
    class Phase : public AdvBase {                                                                  \
      public:                                                                                       \
        Phase ctorArgs;                                                                             \
        virtual ~Phase();                                                                           \
        virtual bool Tick();                                                                        \
        virtual bool Draw();                                                                        \
                                                                                                    \
        /* 0x4 */ void *m_impl;                                                                     \
    };

namespace Title {
ADV_DECLARE_PHASE(())
}
namespace Option {
ADV_DECLARE_PHASE(())
}
namespace StoryLibrary {
ADV_DECLARE_PHASE((int mode))
}
namespace Synopsis {
ADV_DECLARE_PHASE(())
}
namespace SelectMode {
ADV_DECLARE_PHASE(())
}
namespace BattleMode {
ADV_DECLARE_PHASE(())
}
namespace Movie {
ADV_DECLARE_PHASE(())
}
namespace MemoryCheck {
ADV_DECLARE_PHASE(())
}
namespace Logo {
ADV_DECLARE_PHASE(())
}

#undef ADV_DECLARE_PHASE

} // namespace AdvReal

/* An object holding a number display, kept in a function-local static (class name unknown; its
 * constructor 0x80353AB0 and the functions 0x80353A70 and 0x80353A90 are in another unit). */
class TGiNumberDisplay;

class UnkDisplayHolder_80353AB0 {
  public:
    /* A weak copy at 0x80353AB0 (it calls the inline shared_ptr constructor, so it is not
     * inlined, and neither is GetInstance). */
    UnkDisplayHolder_80353AB0() : field_0x8(0) {}

    /* The holder, created on first use (name inferred). */
    static UnkDisplayHolder_80353AB0 *GetInstance() {
        if (s_instance == 0) {
            static UnkDisplayHolder_80353AB0 holder;
            s_instance = &holder;
        }
        return s_instance;
    }

    static UnkDisplayHolder_80353AB0 *s_instance;

    /* 0x0 */ boost::shared_ptr<TGiNumberDisplay> m_display;
    /* 0x8 */ u32 field_0x8;
};
SIZE_ASSERT(UnkDisplayHolder_80353AB0, 0xC);

UnkDisplayHolder_80353AB0 *UnkDisplayHolder_80353AB0::s_instance;

class UnkManager_801E5D48;
class UnkManager_801E5CF8;

extern "C" {
/* The CSD project cache (Game/AdvCsdCache.cpp) and the two calls the folder makes on it when it
 * starts (nothing) and ends (empties it) (behaviour inferred). */
void *fn_801E5F0C(void);
void fn_802B5858(void *cache);
void fn_802B5838(void *cache);
/* Two more front-end managers and their start and end calls (behaviour unknown). */
UnkManager_801E5D48 *fn_801E5D48(void);
void fn_802BF1BC(UnkManager_801E5D48 *manager);
void fn_802BF184(UnkManager_801E5D48 *manager);
UnkManager_801E5CF8 *fn_801E5CF8(void);
void fn_802C8338(UnkManager_801E5CF8 *manager);
void fn_802C8308(UnkManager_801E5CF8 *manager);
/* The number display holder's start and end calls (behaviour unknown). */
void fn_80353A90(UnkDisplayHolder_80353AB0 *holder);
void fn_80353A70(UnkDisplayHolder_80353AB0 *holder);
/* Called on the frames the phase manager reports false (behaviour unknown). */
void fn_801D2FD4(void);
/* (behaviour unknown; see AdvMovie.cpp) */
void fn_80173834(int arg0, float time);
}

UnkManager_801E5E00::UnkManager_801E5E00() {
    m_request = 1;
}

extern "C" bool fn_802B7528(UnkManager_801E5E00 *manager, s32 id) {
    manager->m_request = id;
    return true;
}

bool UnkManager_801E5E00::Update() {
    ChangePhase();
    if (m_phase.get() != 0) {
        return m_phase->Tick();
    }
    return false;
}

bool UnkManager_801E5E00::Draw() {
    if (m_phase.get() != 0) {
        return m_phase->Draw();
    }
    return false;
}

void UnkManager_801E5E00::ChangePhase() {
    if (m_request != m_current) {
        m_current = m_request;
        m_phase.reset();
        switch (m_current) {
        case 0:
            break;
        case 1:
            m_phase = boost::shared_ptr<AdvReal::AdvBase>(new AdvReal::Title::Phase);
            break;
        case 2:
            m_phase = boost::shared_ptr<AdvReal::AdvBase>(new AdvReal::Option::Phase);
            break;
        case 3:
            m_phase = boost::shared_ptr<AdvReal::AdvBase>(new AdvReal::StoryLibrary::Phase(0));
            break;
        case 4:
            m_phase = boost::shared_ptr<AdvReal::AdvBase>(new AdvReal::StoryLibrary::Phase(1));
            break;
        case 5:
            m_phase = boost::shared_ptr<AdvReal::AdvBase>(new AdvReal::Synopsis::Phase);
            break;
        case 6:
            m_phase = boost::shared_ptr<AdvReal::AdvBase>(new AdvReal::SelectMode::Phase);
            break;
        case 7:
            m_phase = boost::shared_ptr<AdvReal::AdvBase>(new AdvReal::BattleMode::Phase);
            break;
        case 8:
            m_phase = boost::shared_ptr<AdvReal::AdvBase>(new AdvReal::Movie::Phase);
            break;
        case 9:
            m_phase = boost::shared_ptr<AdvReal::AdvBase>(new AdvReal::MemoryCheck::Phase);
            break;
        case 10:
            m_phase = boost::shared_ptr<AdvReal::AdvBase>(new AdvReal::Logo::Phase);
            break;
        }
    }
}

namespace AdvReal {

class AdvFolder : public Task {
  public:
    AdvFolder(Task *parent);
    virtual ~AdvFolder();
    virtual void Tick(float dt);

    /* Starts the front-end managers and requests the first phase (name inferred). */
    void Start();
    /* Shuts the front-end managers down (name inferred). */
    void Finish();
};
SIZE_ASSERT(AdvFolder, 0x28);

AdvFolder::AdvFolder(Task *parent) : Task(parent) {
    m_name = "AdvReal::AdvFolder";
    m_flags |= 0x100;
    Start();
    fn_80173834(1, 0.0f);
}

AdvFolder::~AdvFolder() {
    Finish();
}

void AdvFolder::Tick(float dt) {
    if (fn_801E5E00()->Update()) {
        fn_801E5E00()->Draw();
    } else {
        fn_801D2FD4();
    }
}

void AdvFolder::Start() {
    fn_802B5858(fn_801E5F0C());
    fn_802BF1BC(fn_801E5D48());
    fn_802C8338(fn_801E5CF8());
    fn_80353A90(UnkDisplayHolder_80353AB0::GetInstance());
    switch (StageSequence::Manager::GetCurrent()) {
    case -1:
        fn_802B7528(fn_801E5E00(), 9);
        break;
    case 3:
    case 4:
        fn_802B7528(fn_801E5E00(), 7);
        break;
    case 1:
        fn_802B7528(fn_801E5E00(), 3);
        break;
    case 0:
        fn_802B7528(fn_801E5E00(), 6);
        break;
    case 7:
        fn_802B7528(fn_801E5E00(), 4);
        break;
    case 2:
    case 5:
    case 6:
    default:
        fn_802B7528(fn_801E5E00(), 1);
        break;
    }
}


void AdvFolder::Finish() {
    fn_802B5838(fn_801E5F0C());
    fn_802BF184(fn_801E5D48());
    fn_802C8308(fn_801E5CF8());
    fn_80353A70(UnkDisplayHolder_80353AB0::GetInstance());
}

} // namespace AdvReal
