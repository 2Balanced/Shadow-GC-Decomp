/* Vibration::ManagerUnit (class name from the RTTI of its shared_ptr control block; declared in
 * Game/VibrationManagerUnit.h) and its implementation (class name unknown): the vibration of one
 * pad. The unit keeps the requests playing on its pad in a list sorted by request, sums their
 * strengths every frame (a request made at a position is weaker the further away the pad's
 * player is) and drives the pad's Vibration::ServerGC with the sum. File name from the class; all
 * other names are inferred. Definitions are in reverse address order (deferred inlining emits
 * functions last-defined first). */
#define MSL_LIST_ACCESSORS
#define MSL_LIST_VIBRATION
#define MSL_FUNCTIONAL_BINDERS
#define MSL_ALGORITHM_UPPER_BOUND
#define MSL_ITERATOR_DISPATCH
#define BOOST_SP_LESS
#include <iterator>
#include <functional>
#include <list>
#include <algorithm>

#include "Game/CharInfo.h"
#include "Game/MathTypes.h"
#include "Game/VibrationManagerUnit.h"
#include "Game/VibrationRequest.h"
#include "Game/VibrationServer.h"
#include "boost/scoped_ptr.hpp"
#include "boost/shared_ptr.hpp"
#include "boost/weak_ptr.hpp"
#include <dolphin/types.h>
#include "size_assert.h"

struct UnkCharaList_80074194;

/* Set while the game is paused (as in Game/CameraManagerUnit.cpp; meaning inferred). */
extern u8 lbl_805EE1DC;

extern "C" {
UnkCharaList_80074194 *fn_80074194(void);
/* A player's character (behaviour inferred). */
boost::weak_ptr<CharInfo> fn_800A8C54(UnkCharaList_80074194 *list, int player);
}

namespace Vibration {

/* A pad's requests and server (class name unknown; the implementation behind ManagerUnit). */
class ManagerUnitImpl {
  public:
    /* How strong a request at position is for a player at target (name inferred). */
    static float Falloff(const Vec3f *position, const Vec3f *target);
    /* Whether a request has ended (name inferred). */
    static bool IsEnded(const boost::shared_ptr<Request> &request);

    ManagerUnitImpl(int pad);
    ~ManagerUnitImpl();
    void Reset();
    void Add(boost::shared_ptr<Request> request);
    void Remove(boost::shared_ptr<Request> request);
    void Stop();
    void Update(float dt, int mode);

    /* 0x00 */ s32 m_pad;
    /* 0x04 */ s32 m_player; /* the player the pad belongs to, or -1 */
    /* 0x08 */ std::list<boost::shared_ptr<Request> > m_requests;
    /* 0x14 */ Server *m_server;
};
SIZE_ASSERT(ManagerUnitImpl, 0x18);

/* Deletes the impl (name inferred; see checked_delete below). */
inline void DeleteImpl(ManagerUnitImpl *impl) {
    delete impl;
}

} // namespace Vibration

/* Retail keeps boost::checked_delete<ManagerUnitImpl> out of line and the destructor at its own
 * place; this specialization gives that under GC/2.7 (see docs/research-notes.md and
 * Game/EnergyCore.cpp). */
namespace boost {
template <> inline void checked_delete<Vibration::ManagerUnitImpl>(Vibration::ManagerUnitImpl *x) {
    Vibration::DeleteImpl(x);
}
} // namespace boost

/* The distances within which a request at a position is felt fully, and beyond which it is not
 * felt (names inferred). */
inline float Plain(float value) {
    return value;
}
static float s_fullDistance = Plain(50.0f);
static float s_noDistance = Plain(250.0f);

namespace Vibration {

bool ManagerUnitImpl::IsEnded(const boost::shared_ptr<Request> &request) {
    return !request->m_active;
}

float ManagerUnitImpl::Falloff(const Vec3f *position, const Vec3f *target) {
    Vec3f offset;
    fn_8040CD74(&offset, position, target);
    float distance = fn_8040CF50(&offset);
    if (distance > s_noDistance) {
        return 0.0f;
    }
    if (distance < s_fullDistance) {
        return 1.0f;
    }
    return 1.0f - (distance - s_fullDistance) / (s_noDistance - s_fullDistance);
}

ManagerUnitImpl::ManagerUnitImpl(int pad) : m_pad(pad), m_player(-1), m_server(0) {
    m_requests.clear();
    m_server = new ServerGC(pad);
}

ManagerUnitImpl::~ManagerUnitImpl() {
    m_requests.clear();
    if (m_server != 0) {
        delete m_server;
    }
    m_server = 0;
}

void ManagerUnitImpl::Reset() {
    Stop();
}

void ManagerUnitImpl::Add(boost::shared_ptr<Request> request) {
    std::list<boost::shared_ptr<Request> >::iterator position =
        std::upper_bound(m_requests.begin(), m_requests.end(), request);
    m_requests.insert(position, request);
}

void ManagerUnitImpl::Remove(boost::shared_ptr<Request> request) {
    m_requests.remove(request);
}

void ManagerUnitImpl::Stop() {
    m_requests.clear();
    if (m_server != 0) {
        m_server->Stop();
    }
}

void ManagerUnitImpl::Update(float dt, int mode) {
    float strength = 0.0f;
    float strength2 = strength;
    bool active = false;
    if (!lbl_805EE1DC && !m_requests.empty()) {
        bool hasChara = false;
        boost::shared_ptr<CharInfo> chara;
        if (mode == 1 && m_player != -1) {
            chara = fn_800A8C54(fn_80074194(), m_player).lock();
            if (chara.get() != 0) {
                hasChara = true;
            }
        }
        for (std::list<boost::shared_ptr<Request> >::iterator it = m_requests.begin(); it != m_requests.end();
             it++) {
            it->get()->Update(dt);
            if (it->get()->m_hasPosition == 1) {
                if (hasChara) {
                    float falloff = Falloff(&((RequestWithPos *)it->get())->m_position, &chara->m_position);
                    strength += falloff * it->get()->m_strength;
                    strength2 += falloff * it->get()->m_strength2;
                } else {
                    (*it)->End();
                }
            } else {
                strength += it->get()->m_strength;
                strength2 += it->get()->m_strength2;
            }
            active = true;
        }
        m_requests.remove_if(IsEnded);
    }
    if (m_server != 0) {
        if (active) {
            m_server->SetLevel(strength, strength2);
        } else {
            m_server->Stop();
        }
        m_server->Update(dt);
    }
}

ManagerUnit::ManagerUnit(int pad) : m_impl(new ManagerUnitImpl(pad)) {}

ManagerUnit::~ManagerUnit() {}

void ManagerUnit::Reset() {
    m_impl->Reset();
}

void ManagerUnit::Update(float dt, int mode) {
    m_impl->Update(dt, mode);
}

void ManagerUnit::SetPlayer(int player) {
    m_impl->m_player = player;
}

void ManagerUnit::Add(boost::shared_ptr<Request> request) {
    m_impl->Add(request);
}

void ManagerUnit::Remove(boost::shared_ptr<Request> request) {
    m_impl->Remove(request);
}

void ManagerUnit::Stop() {
    m_impl->Stop();
}

} // namespace Vibration
