/* Boss::Diablon::Executer (class name from RTTI; declared in Game/Diablon.h): the task that runs
 * the Diablon boss. Its state (Executer::Impl) holds the boss, the boss's matrix and the behavior
 * running; it makes the root of the parts tree and tells the behaviors in the behavior table when
 * it is set up and torn down. Each frame the executer finds the player, ticks the behavior,
 * rebuilds the matrix and ticks the parts. The functions other units call keep their fn_ names (C
 * functions taking the executer). File name inferred from the class. Names marked "(name
 * inferred)" are not original. Definitions are in reverse address order (deferred inlining emits
 * functions last-defined first). */
#include <boost/scoped_ptr.hpp>

#include "Game/CharInfo.h"
#include "Game/Diablon.h"
#include "Game/MathTypes.h"
#include "Game/Task.h"
#include "boost/shared_ptr.hpp"
#include "boost/weak_ptr.hpp"
#include "size_assert.h"

class UnkCharaList_80074194;

extern "C" {
/* The character list, and the character nearest to position in it (see Game/ArkCannon.cpp). */
UnkCharaList_80074194 *fn_80074194(void);
boost::weak_ptr<CharInfo> fn_800A8A7C(UnkCharaList_80074194 *list, const Vec3f *position);

/* The executer's functions other units call (see the file comment). */
void fn_802D9E4C(Boss::Diablon::Executer *executer, const Boss::Diablon::Parts::Action &action);
int fn_802D9E78(Boss::Diablon::Executer *executer);
void fn_802D9ED8(Boss::Diablon::Executer *executer,
                 const Boss::Diablon::Behavior::InitializeArgs &args);
}

/* boost/shared_ptr.hpp only declares shared_ptr::reset, which the game has out of line; this unit
 * emits the copy for the behaviors (boost 1.32's definition; as in Game/Bomb.cpp). */
namespace boost {
template <> inline void shared_ptr<Boss::Diablon::Behavior::BehaviorBase>::reset() {
    this_type().swap(*this);
}
} // namespace boost

namespace Boss {
namespace Diablon {

namespace Parts {

/* The parts the executer makes (defined in their own units); only their factories are used
 * here. */
class Body {
  public:
    static PartsBase *Instantiate(PartsBase *parent, const boost::shared_ptr<Diablon> &owner,
                                  const Mtx34f *parentMatrix);
};
class HPGauge {
  public:
    static PartsBase *Instantiate(PartsBase *parent, const boost::shared_ptr<Diablon> &owner);
};
class CameraControl {
  public:
    static PartsBase *Instantiate(PartsBase *parent, const boost::shared_ptr<Diablon> &owner);
};

} // namespace Parts

namespace Behavior {
/* The behavior table (0x80558CA8; name inferred): index 0 is unused; the indices are the ids of
 * InitializeArgs. */
extern const Info g_idleInfo;
extern const Info g_moveInfo;
extern const Info g_bombInfo;
extern const Info g_beamInfo;
extern const Info g_wipeInfo;
extern const Info g_eventInfo;
} // namespace Behavior

static const Behavior::Info *s_behaviors[7] = {
    0,
    &Behavior::g_idleInfo,
    &Behavior::g_moveInfo,
    &Behavior::g_bombInfo,
    &Behavior::g_beamInfo,
    &Behavior::g_wipeInfo,
    &Behavior::g_eventInfo,
};

class Executer::Impl {
  public:
    Impl(const boost::shared_ptr<Diablon> &owner);
    ~Impl();

    /* Names inferred. */
    boost::shared_ptr<Behavior::BehaviorBase> CreateBehavior(int id);
    bool SwitchBehavior(const Behavior::InitializeArgs &args);
    void UpdateMatrix();

    /* 0x00 */ boost::shared_ptr<Diablon> m_owner;
    /* 0x08 */ Mtx34f m_matrix;
    /* 0x38 */ boost::shared_ptr<Behavior::BehaviorBase> m_current;
};
SIZE_ASSERT(Executer::Impl, 0x40);

/* Deletes the state (name inferred; see checked_delete below). */
inline void DeleteImpl(Executer::Impl *impl) {
    delete impl;
}

} // namespace Diablon
} // namespace Boss

/* Retail keeps boost::checked_delete<Impl> out of line (0x802DA18C) and ~Impl at its own place
 * (0x802DA3B8). GC/2.7 inlines checked_delete when the destructor is not inline; this
 * specialization calls an inline helper instead, which gives retail's code and order (as in
 * Game/DiablonController.cpp; see docs/research-notes.md). */
namespace boost {
template <>
inline void checked_delete<Boss::Diablon::Executer::Impl>(Boss::Diablon::Executer::Impl *x) {
    Boss::Diablon::DeleteImpl(x);
}
} // namespace boost

namespace Boss {
namespace Diablon {

boost::shared_ptr<Behavior::BehaviorBase> Executer::Impl::CreateBehavior(int id) {
    const Behavior::Info *info = s_behaviors[id];
    if (info != 0 && info->create != 0) {
        return info->create(m_owner);
    }
    return boost::shared_ptr<Behavior::BehaviorBase>();
}

/* Retail sets up the loop below (and the one in ~Impl) with its counter first and copies it into
 * the table offset; GC/2.7 loads the table address first and clears both separately for every
 * shape tried (index, element pointer, inline helper, do-while, const and unsized table), so these
 * two functions stay at about 98-99%. */
Executer::Impl::Impl(const boost::shared_ptr<Diablon> &owner) : m_owner(owner) {
    m_owner->m_rootPart = new Parts::PartsBase(0, "Root", m_owner);
    for (u32 i = 0; i < 7; i++) {
        if (s_behaviors[i] != 0 && s_behaviors[i]->setup != 0) {
            s_behaviors[i]->setup(this);
        }
    }
}

Executer::Impl::~Impl() {
    if (m_current.get() != 0) {
        m_current->End(7);
        m_current.reset();
    }
    for (u32 i = 0; i < 7; i++) {
        if (s_behaviors[i] != 0 && s_behaviors[i]->teardown != 0) {
            s_behaviors[i]->teardown(this);
        }
    }
    if (m_owner->m_rootPart != 0) {
        delete m_owner->m_rootPart;
    }
    m_owner->m_rootPart = 0;
}

bool Executer::Impl::SwitchBehavior(const Behavior::InitializeArgs &args) {
    boost::shared_ptr<Behavior::BehaviorBase> next = CreateBehavior(args.m_id);
    int previous = m_current.get() != 0 ? m_current.get()->GetId() : 7;
    int nextId = next.get() != 0 ? next.get()->field_0x10 : 7;
    if (m_current.get() != 0) {
        m_current.get()->End(nextId);
    }
    m_current = next;
    if (m_current.get() != 0) {
        m_current.get()->Begin(previous, args);
    }
    return true;
}

void Executer::Impl::UpdateMatrix() {
    Mtx34f translation;
    SetTranslationMtx(&translation, m_owner->m_position);
    fn_8040C148(&m_matrix, &translation);
    fn_8040B80C(&m_matrix, m_owner.get()->m_rotation.z);
    fn_8040B848(&m_matrix, m_owner.get()->m_rotation.y);
    fn_8040B884(&m_matrix, m_owner.get()->m_rotation.x);
}

Executer::Executer(Task *parent, const boost::shared_ptr<Diablon> &owner)
    : Task(parent), m_impl(new Impl(owner)) {
    m_name = "Executer";
    m_impl->m_owner->m_executer = this;
}

Executer::~Executer() {
    m_impl->m_owner->m_executer = 0;
}

void Executer::Start() {
    Parts::Body::Instantiate(m_impl->m_owner->m_rootPart, m_impl->m_owner, &m_impl->m_matrix);
    Parts::HPGauge::Instantiate(m_impl->m_owner->m_rootPart, m_impl->m_owner);
    Parts::CameraControl::Instantiate(m_impl->m_owner->m_rootPart, m_impl->m_owner);
}

void Executer::Tick(float dt) {
    /* CharInfo's word at 0x60 points to the character's frame time (meaning inferred). */
    float frameTime = **reinterpret_cast<float **>(m_impl->m_owner->field_0x60);
    m_impl->m_owner->m_target = fn_800A8A7C(fn_80074194(), &m_impl->m_owner->m_position);
    if (m_impl->m_current) {
        boost::shared_ptr<Behavior::BehaviorBase> current = m_impl->m_current;
        current->Tick(frameTime);
    }
    m_impl->UpdateMatrix();
    m_impl->m_owner->m_rootPart->TickChildren(frameTime);
}

} // namespace Diablon
} // namespace Boss

using Boss::Diablon::Executer;

/* Switches the boss to the behavior args names (name inferred). */
extern "C" void fn_802D9ED8(Executer *executer, const Boss::Diablon::Behavior::InitializeArgs &args) {
    executer->m_impl->SwitchBehavior(args);
}

/* Meant to give the running behavior's id; retail always returns 7 (none), dropping the id it
 * reads (name inferred). */
extern "C" int fn_802D9E78(Executer *executer) {
    if (executer->m_impl->m_current) {
        executer->m_impl->m_current->GetId();
    }
    return 7;
}

/* Passes action to the boss's parts (name inferred). */
extern "C" void fn_802D9E4C(Executer *executer, const Boss::Diablon::Parts::Action &action) {
    executer->m_impl->m_owner->m_rootPart->SendAction(action);
}
