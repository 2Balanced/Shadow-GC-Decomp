/* SonicteamUSA::System::Camera::CameraManagerUnit (class name from the RTTI of its shared_ptr
 * control block; see Game/CameraManagerUnit.h): the camera of one screen, and the requests and
 * camera modes made for it. A camera mode and a request are owned through shared_ptrs whose
 * deleters (DeleteMode, DeleteRequest) take them off their unit again; the unit only keeps weak
 * pointers. The file name and the names of the members and functions are inferred. Definitions are
 * in reverse address order (deferred inlining emits functions last-defined first).
 *
 * Every function matches, but the unit does not link yet: retail emits the dispose and get_deleter
 * of sp_counted_base_impl<CameraRequest *, ...> (with the type_info name of its deleter) before
 * those of sp_counted_base_impl<CameraMode *, ...>, while their vtables and RTTI come in the
 * other order; GC/2.7 emits both in vtable order. This looks like another difference of the
 * game's compiler (see docs/research-notes.md). Tried without success: an explicit instantiation
 * of the request's control block (it puts get_deleter before dispose), unused functions that
 * inline or call dispose and get_deleter at several points of the file, and a local class
 * template whose virtual function calls them. */
#define BOOST_SP_DELETER_CONSTRUCTOR
#define MSL_LIST_ACCESSORS
#include "Game/CameraManagerUnit.h"

using namespace SonicteamUSA::System::Camera;

/* What fn_80009548 returns: the factory comes first. */
class UnkCameraManager_80009548 {
  public:
    /* 0x0 */ CameraFactory *m_factory;
};

/* A flag that stops requests of priorities 0 to 0x400 (meaning unknown). */
extern u8 lbl_805EE1DC;

extern "C" {
UnkCameraManager_80009548 *fn_80009548(void);
/* The camera unit of a screen index. */
boost::shared_ptr<CameraManagerUnit> fn_8000D0F0(UnkCameraManager_80009548 *manager, int index);
/* Makes a camera mode of a kind (behaviour inferred; the arguments' meanings are unknown). */
CameraMode *fn_8000BB00(CameraFactory *factory, int arg1, int arg2, int index);
/* Whether request goes before other, and a request's activation and deactivation for its mode
 * (behaviour inferred). */
bool fn_8000EC28(CameraRequest *request, CameraRequest *other);
void fn_8000E8D8(CameraMode *mode, CameraRequest *request);
void fn_8000E88C(CameraMode *mode, CameraRequest *request);
/* Starts a request's blend from state (behaviour inferred). */
void fn_8000AF74(void *blend, UnkCameraState_8000AA70 *state, CameraMode *mode);
/* A mode's frame with its request, and the blended placement (behaviour inferred). */
void fn_8000E928(CameraMode *mode, CameraRequest *request, float dt);
const UnkCameraState_8000AA70 *fn_8000ADD0(void *blend, CameraMode *mode, float dt);
/* The shake's frame and whether it is over (behaviour inferred). */
void fn_8000EFC0(CameraShake *shake, UnkCameraState_8000AA70 *state, float dt);
bool fn_8000EF58(CameraShake *shake);
/* A placement's view and world matrices (behaviour inferred). */
void fn_8000A6C4(Mtx34f *out, const UnkCameraState_8000AA70 *state);
void fn_8000A67C(Mtx34f *out, const UnkCameraState_8000AA70 *state);
/* Whether a time step is zero (behaviour inferred). */
bool fn_8000A950(float dt);
void fn_8040CDA8(Vec3f *out, const Vec3f *vector, float scale); /* vector * scale */
}

boost::shared_ptr<CameraMode> CameraManagerUnit::GetCurrentMode() {
    boost::shared_ptr<CameraMode> mode;
    if (HasRequest()) {
        mode = m_requests.front().lock()->m_mode;
    }
    return mode;
}

boost::shared_ptr<CameraRequest> CameraManagerUnit::GetCurrentRequest() {
    boost::shared_ptr<CameraRequest> request;
    if (HasRequest()) {
        request = m_requests.front().lock();
    }
    return request;
}

void CameraManagerUnit::Update(float dt) {
    if (!HasRequest()) {
        return;
    }
    std::list<boost::weak_ptr<CameraRequest> >::iterator it = m_requests.begin();
    boost::shared_ptr<CameraRequest> top = it->lock();
    if (top) {
        if (lbl_805EE1DC != 0 && top->m_priority >= 0 && top->m_priority <= 0x400) {
            return;
        }
        boost::shared_ptr<CameraMode> mode(top->m_mode);
        UnkCameraState_8000AA70 state;
        fn_8000E928(mode.get(), top.get(), dt);
        it++;
        for (; it != m_requests.end(); it++) {
            boost::shared_ptr<CameraRequest> request = it->lock();
            if (request && (request->m_flags & 2)) {
                boost::shared_ptr<CameraMode> other(request->m_mode);
                other->Update(dt, reinterpret_cast<UnkCameraUpdateArg_800099B8 *>(request.get()));
            }
        }
        if (top->m_blend != 0) {
            state = *fn_8000ADD0(top->m_blend, top->m_mode.get(), dt);
        } else {
            state = *reinterpret_cast<UnkCameraState_8000AA70 *>(mode.get());
        }
        if (m_shake.get() != 0) {
            fn_8000EFC0(m_shake.get(), &state, dt);
            if (fn_8000EF58(m_shake.get())) {
                m_shake.reset();
            }
        }
        m_state = state;
        if (field_0xD0 == 0) {
            m_state2 = state;
        }
        Mtx34f view;
        fn_8000A6C4(&view, &m_state);
        fn_8040C148(&m_worldMtx.value, &view);
        Mtx34f world;
        fn_8000A67C(&world, &m_state);
        fn_8040C148(&m_viewMtx.value, &world);
    }
    Vec3f moved;
    fn_8040CD74(&moved, &m_state.m_eye, &m_lastState.m_eye);
    fn_800091D0(&m_velocity, &moved);
    if (fn_8000A950(dt)) {
        fn_800091D0(&m_velocity, &lbl_805E07A0);
    } else {
        Vec3f moved2;
        fn_8040CD74(&moved2, &m_state.m_eye, &m_lastState.m_eye);
        fn_800091D0(&m_velocity, &moved2);
        Vec3f velocity;
        fn_8040CDA8(&velocity, &m_velocity, 1.0f / dt);
        fn_800091D0(&m_velocity, &velocity);
    }
    fn_800091D0(&m_targetPosition, &m_target->m_position);
    static_cast<UnkCameraState_8000AA70 &>(m_lastState) = m_state;
}

void CameraManagerUnit::AddMode(boost::shared_ptr<CameraMode> mode) {
    m_modes.push_back(boost::weak_ptr<CameraMode>(mode));
}

void CameraManagerUnit::PruneModes() {
    std::list<boost::weak_ptr<CameraMode> >::iterator it = m_modes.begin();
    for (; it != m_modes.end(); it++) {
        if (it->expired()) {
            m_modes.erase(it);
            break;
        }
    }
}

void CameraManagerUnit::AddRequest(boost::shared_ptr<CameraRequest> request) {
    std::list<boost::weak_ptr<CameraRequest> >::iterator it = m_requests.begin();
    for (; it != m_requests.end(); it++) {
        if (fn_8000EC28(request.get(), it->lock().get())) {
            break;
        }
    }
    if (it == m_requests.begin()) {
        if (!m_requests.empty()) {
            boost::shared_ptr<CameraRequest> front = m_requests.front().lock();
            if (!(front->m_flags & 2)) {
                fn_8000E88C(front->m_mode.get(), front.get());
            }
        }
        fn_8000E8D8(request->m_mode.get(), request.get());
        if (request->m_blend != 0) {
            fn_8000AF74(request->m_blend, &m_state, request->m_mode.get());
        }
    } else if (request->m_flags & 2) {
        fn_8000E8D8(request->m_mode.get(), request.get());
    }
    m_requests.insert(it, boost::weak_ptr<CameraRequest>(request));
}

void CameraManagerUnit::RemoveRequest(CameraRequest *request) {
    std::list<boost::weak_ptr<CameraRequest> >::iterator it = m_requests.begin();
    for (; it != m_requests.end(); it++) {
        if (it->expired()) {
            break;
        }
    }
    if (it == m_requests.begin()) {
        fn_8000E88C(request->m_mode.get(), request);
        if (m_requests.size() > 1) {
            std::list<boost::weak_ptr<CameraRequest> >::iterator next = m_requests.begin();
            next++;
            boost::shared_ptr<CameraRequest> following = next->lock();
            if (!(following->m_flags & 2)) {
                fn_8000E8D8(following->m_mode.get(), following.get());
            }
            if (following->m_blend != 0) {
                fn_8000AF74(following->m_blend, &m_state, following->m_mode.get());
            }
        }
    } else if (it != m_requests.end() && (request->m_flags & 2)) {
        fn_8000E88C(request->m_mode.get(), request);
    }
    m_requests.erase(it);
}

CameraManagerUnit::CameraManagerUnit(int index)
    : field_0x0(0), m_index(index), m_viewMtx(lbl_805E0720), m_worldMtx(lbl_805E0720), field_0xD0(0),
      m_target(0) {
    m_modes.clear();
    m_requests.clear();
    m_target = fn_80009548()->m_factory->CreateTarget();
}

CameraManagerUnit::~CameraManagerUnit() {
    if (m_target != 0) {
        delete m_target;
        m_target = 0;
    }
}

/* The deleter of a camera mode: takes it off its unit's modes and deletes it. */
static void DeleteMode(CameraMode *mode) {
    fn_8000D0F0(fn_80009548(), mode->field_0xA0)->PruneModes();
    delete mode;
}

/* The deleter of a request: takes it off its unit's requests and deletes it. */
static void DeleteRequest(CameraRequest *request) {
    fn_8000D0F0(fn_80009548(), request->m_mode->field_0xA0)->RemoveRequest(request);
    delete request;
}

/* Makes a camera mode of a kind for the unit of a screen index (the other arguments' meanings are
 * unknown). */
extern "C" boost::shared_ptr<CameraMode> fn_8026F6A4(int arg0, int arg1, int index) {
    boost::shared_ptr<CameraMode> mode(fn_8000BB00(fn_80009548()->m_factory, arg0, arg1, index),
                                       DeleteMode);
    fn_8000D0F0(fn_80009548(), index)->AddMode(mode);
    return mode;
}

/* Releases a camera mode. */
extern "C" void fn_8026F684(boost::shared_ptr<CameraMode> *mode) {
    mode->reset();
}

/* Requests mode for its unit (the other arguments' meanings are unknown). */
extern "C" boost::shared_ptr<CameraRequest> fn_8026F464(const boost::shared_ptr<CameraMode> &mode, int arg1,
                                                        int arg2, int arg3, int arg4) {
    boost::shared_ptr<CameraRequest> request(new CameraRequest(mode, arg1, arg2, arg3, arg4), DeleteRequest);
    fn_8000D0F0(fn_80009548(), mode->field_0xA0)->AddRequest(request);
    return request;
}

/* Withdraws a request. */
extern "C" void fn_8026F444(boost::shared_ptr<CameraRequest> *request) {
    request->reset();
}
