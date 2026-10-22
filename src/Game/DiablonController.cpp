/* Boss::Diablon::Controller (class name from RTTI): the task that runs the Diablon boss's AI. Its
 * state (Controller::Impl) holds the boss and a desire manager with the boss's five desires
 * (Desire::Move, Wipe, Beam, Bomb and Event); each frame, unless the game is paused, it tells the
 * manager whether the first two desire groups may run (boss flags 7 and 8) and updates it with the
 * boss's frame time. File name inferred from the class. Names marked "(name inferred)" are not
 * original. Definitions are in reverse address order (deferred inlining emits functions
 * last-defined first). */
#include <boost/scoped_ptr.hpp>

#include "Game/BossDesire.h"
#include "Game/BossDesireManager.h"
#include "Game/Diablon.h"
#include "Game/Task.h"
#include "boost/shared_ptr.hpp"
#include <dolphin/types.h>
#include "size_assert.h"

/* The game state (struct name unknown; see Game/ElecSecurityCode.cpp). */
struct UnkGameState_8057D6A0 {
    /* 0x000 */ u8 field_0x0[0xC8];
    /* 0x0C8 */ u8 m_paused; /* nonzero while the game is paused (meaning inferred) */
    /* 0x0C9 */ u8 field_0xC9[0x150 - 0xC9];
};
SIZE_ASSERT(UnkGameState_8057D6A0, 0x150);

extern UnkGameState_8057D6A0 lbl_8057D6A0;

namespace Boss {
namespace Diablon {
namespace Desire {

/* The desires (defined in Game/DiablonDesire<Name>.cpp); only their factories are used here. */
class Move {
  public:
    static boost::shared_ptr<DiablonDesireBase> Instantiate(const boost::shared_ptr<Diablon> &owner);
};
class Wipe {
  public:
    static boost::shared_ptr<DiablonDesireBase> Instantiate(const boost::shared_ptr<Diablon> &owner);
};
class Beam {
  public:
    static boost::shared_ptr<DiablonDesireBase> Instantiate(const boost::shared_ptr<Diablon> &owner);
};
class Bomb {
  public:
    static boost::shared_ptr<DiablonDesireBase> Instantiate(const boost::shared_ptr<Diablon> &owner);
};
class Event {
  public:
    static boost::shared_ptr<DiablonDesireBase> Instantiate(const boost::shared_ptr<Diablon> &owner);
};

} // namespace Desire

class Controller : public Task {
  public:
    /* The controller's state (class name inferred). */
    class Impl {
      public:
        Impl(const boost::shared_ptr<Diablon> &owner);
        ~Impl();

        /* Adds the boss's desires to the manager (name inferred). */
        void AddDesires();
        void Update(float dt);

        /* 0x0 */ boost::shared_ptr<Diablon> m_owner;
        /* 0x8 */ Boss::Desire::DesireManager m_desires;
    };

    Controller(Task *parent, const boost::shared_ptr<Diablon> &owner);
    virtual ~Controller();
    virtual void Tick(float dt);

    /* 0x28 */ boost::scoped_ptr<Impl> m_impl;
    /* 0x2C */ u32 field_0x2C; /* never touched (size from the allocation, 0x802D9A78) */
};
SIZE_ASSERT(Controller::Impl, 0xC);
SIZE_ASSERT(Controller, 0x30);

/* Deletes the state (name inferred; see checked_delete below). */
inline void DeleteImpl(Controller::Impl *impl) {
    delete impl;
}

} // namespace Diablon
} // namespace Boss

/* Retail keeps boost::checked_delete<Impl> out of line (0x802D5A9C) and ~Impl at its own place
 * (0x802D5DB0). GC/2.7 inlines checked_delete when the destructor is not inline; this
 * specialization calls an inline helper instead, which gives retail's code and order (as in
 * Game/BossCtrl.cpp; see docs/research-notes.md). */
namespace boost {
template <>
inline void checked_delete<Boss::Diablon::Controller::Impl>(Boss::Diablon::Controller::Impl *x) {
    Boss::Diablon::DeleteImpl(x);
}
} // namespace boost

namespace Boss {
namespace Diablon {

Controller::Impl::Impl(const boost::shared_ptr<Diablon> &owner) : m_owner(owner) {
    m_desires.Init(5);
    AddDesires();
}

Controller::Impl::~Impl() {}

void Controller::Impl::AddDesires() {
    m_desires.Set(0,
                boost::shared_ptr<Boss::Desire::DesireBase>(Desire::Move::Instantiate(m_owner)));
    m_desires.Set(1,
                boost::shared_ptr<Boss::Desire::DesireBase>(Desire::Wipe::Instantiate(m_owner)));
    m_desires.Set(2,
                boost::shared_ptr<Boss::Desire::DesireBase>(Desire::Beam::Instantiate(m_owner)));
    m_desires.Set(3,
                boost::shared_ptr<Boss::Desire::DesireBase>(Desire::Bomb::Instantiate(m_owner)));
    m_desires.Set(4,
                boost::shared_ptr<Boss::Desire::DesireBase>(Desire::Event::Instantiate(m_owner)));
}

void Controller::Impl::Update(float dt) {
    m_desires.SetFlag(0, m_owner->m_flags.Test(7));
    m_desires.SetFlag(1, m_owner->m_flags.Test(8));
    m_desires.Tick(dt);
}

Controller::Controller(Task *parent, const boost::shared_ptr<Diablon> &owner)
    : Task(parent), m_impl(new Impl(owner)) {
    m_name = "Controller";
    m_impl->m_owner->m_controller = this;
}

Controller::~Controller() {
    m_impl->m_owner->m_controller = 0;
}

void Controller::Tick(float dt) {
    /* CharInfo's word at 0x60 points to the character's frame time (meaning inferred). */
    float frameTime = **reinterpret_cast<float **>(m_impl->m_owner->field_0x60);
    if (!lbl_8057D6A0.m_paused) {
        m_impl->Update(frameTime);
    }
}

} // namespace Diablon
} // namespace Boss
