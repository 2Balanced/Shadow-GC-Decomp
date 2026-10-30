/* SonicteamUSA::System::Camera::CameraModeUser (class name and base from RTTI; mode -0x3FE;
 * Game/CameraModeUser.h): a camera mode whose owner places the camera itself, through the
 * handlers of three events the mode raises. The unit is the first user of boost::function, so it
 * also emits the first copies of boost::bad_function_call and of the library's std::exception and
 * std::runtime_error (the <stdexcept> stand-in's MSL_RUNTIME_ERROR_IMPL). File name from the class;
 * other names are inferred. Definitions are in reverse address order (deferred inlining emits
 * functions last-defined first). */
#define MSL_RUNTIME_ERROR_IMPL
#define CAMERAMODEUSER_EVENTS
#include <boost/function.hpp>
#include <stdexcept>

#include "Game/CameraModeUser.h"
#include "boost/shared_ptr.hpp"
#include "size_assert.h"

using SonicteamUSA::System::Camera::CameraMode;
using SonicteamUSA::System::Camera::CameraModeUser;

/* The event arguments (contents from the code that builds them). */
struct CameraModeUser::SwitchEventArgs {
    /* 0x0 */ void *m_arg;
};

struct CameraModeUser::UpdateEventArgs {
    /* 0x0 */ float m_dt;
    /* 0x4 */ UnkCameraUpdateArg_800099B8 *m_arg;
};

SIZE_ASSERT(CameraModeUser, 0xD0);

extern "C" {
/* Makes a camera mode of a kind (Game/CameraManagerUnit.cpp). */
boost::shared_ptr<CameraMode> fn_8026F6A4(int kind, int arg0, int arg1);
}

namespace boost {

/* reset() is out of line in the game (see boost/shared_ptr.hpp). */
template <> inline void shared_ptr<CameraModeUser>::reset() {
    this_type().swap(*this);
}

} // namespace boost

CameraModeUser::CameraModeUser(UnkCameraOwner_8000E9BC *owner, int screen)
    : CameraMode(owner, screen) {}

CameraModeUser::~CameraModeUser() {}

void CameraModeUser::Update(float dt, UnkCameraUpdateArg_800099B8 *arg) {
    if (m_onUpdate.empty()) {
        return;
    }
    UpdateEventArgs args;
    args.m_dt = dt;
    args.m_arg = arg;
    m_onUpdate.Raise(args);
}

void CameraModeUser::OnSwitch(UnkCameraSwitchArg_8000996C *arg) {
    if (m_onSwitch.empty()) {
        return;
    }
    SwitchEventArgs args;
    args.m_arg = arg;
    m_onSwitch.Raise(args);
}

void CameraModeUser::vf_0x14(UnkCameraArg_800098E4 *arg) {
    if (field_0xB8.empty()) {
        return;
    }
    SwitchEventArgs args;
    args.m_arg = arg;
    field_0xB8.Raise(args);
}

CameraMode *CameraModeUser::Create(UnkCameraOwner_8000E9BC *owner, int screen) {
    return new CameraModeUser(owner, screen);
}

extern "C" {

boost::shared_ptr<CameraModeUser> fn_80009704(int arg0, int arg1) {
    boost::shared_ptr<CameraMode> mode(fn_8026F6A4(-0x3FE, arg0, arg1));
    boost::shared_ptr<CameraModeUser> user(boost::dynamic_pointer_cast<CameraModeUser>(mode));
    return user;
}

/* Releases a user camera mode. */
void fn_80009618(boost::shared_ptr<CameraModeUser> *mode) {
    mode->reset();
}

} // extern "C"

int CameraModeUser::GetModeId() {
    return -0x3FE;
}
