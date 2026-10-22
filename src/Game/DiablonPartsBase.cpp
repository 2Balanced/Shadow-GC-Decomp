/* Boss::Diablon::Parts::PartsBase (class name from RTTI; declared in Game/Diablon.h): a part of the
 * Diablon boss. The parts form a tree: a part links itself in below its parent when it is made and
 * out again when it is deleted, deleting its own children first. File name inferred from the
 * class. Names marked "(name inferred)" are not original. Definitions are in reverse address order
 * (deferred inlining emits functions last-defined first). */
#include "Game/Diablon.h"
#include "boost/shared_ptr.hpp"

namespace Boss {
namespace Diablon {
namespace Parts {

inline void PartsBase::AddChild(PartsBase *child) {
    PartsBase **link = &m_child;
    while (*link != 0) {
        link = &(*link)->m_next;
    }
    *link = child;
    child->m_parent = this;
}

inline void PartsBase::RemoveChild(PartsBase *child) {
    PartsBase **link = &m_child;
    PartsBase *part;
    while ((part = *link) != child || part == 0) {
        link = &part->m_next;
    }
    if (part == child) {
        *link = child->m_next;
    }
    child->m_parent = 0;
}

PartsBase::PartsBase(PartsBase *parent, const char *name, const boost::shared_ptr<Diablon> &owner)
    : m_owner(owner), m_name(name), m_parent(parent), m_child(0), m_next(0), field_0x1C(0),
      m_finished(false) {
    if (m_parent != 0) {
        m_parent->AddChild(this);
    }
}

PartsBase::~PartsBase() {
    while (m_child != 0) {
        delete m_child;
    }
    if (m_parent != 0) {
        m_parent->RemoveChild(this);
    }
}

inline void PartsBase::TickChildren(float dt) {
    PartsBase *child = m_child;
    while (child != 0) {
        child->Tick(dt);
        child->TickChildren(dt);
        if (child->m_finished) {
            PartsBase *next = child->m_next;
            delete child;
            child = next;
        } else {
            child = child->m_next;
        }
    }
}

inline void PartsBase::SendAction(const Action &action) {
    for (PartsBase *child = m_child; child != 0; child = child->m_next) {
        child->OnAction(action);
        child->SendAction(action);
    }
}

/* Not original: the two functions above are inline, and retail emits its copies of them here, after
 * OnAction. This unused function makes GC/2.7 emit them in that place (the linker drops it). */
static void EmitTreeFunctions(PartsBase *part, const Action &action, float dt) {
    part->TickChildren(dt);
    part->SendAction(action);
}

void PartsBase::OnAction(const Action &action) {}

void PartsBase::Tick(float dt) {}

} // namespace Parts
} // namespace Diablon
} // namespace Boss
