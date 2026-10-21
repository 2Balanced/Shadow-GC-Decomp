#ifndef GAME_CAMERAMODEUSER_H
#define GAME_CAMERAMODEUSER_H

#include <boost/function.hpp>

#include "Game/MathTypes.h"
#include "boost/shared_ptr.hpp"
#include <dolphin/types.h>
#include "size_assert.h"

namespace SonicteamUSA {
namespace System {
namespace Camera {

/* CameraMode (class name from RTTI): the base of the camera's modes, which the camera manager
 * stacks. Its layout is unknown. */
class CameraMode {
  public:
    virtual ~CameraMode();
};

/* CameraModeUser (class name from RTTI; vtable at 0x8051C484): a camera mode whose owner places
 * the camera itself. Each frame it raises an update event, whose handler (at 0xC4; name inferred)
 * the owner sets; fn_8000A5D4 sets the eye and the point looked at, fn_8000ACE4 the up vector
 * (behaviour inferred from COASTER). shared_ptr<CameraModeUser>'s constructor, assignment and
 * destructor are shared weak instances (0x800096F4, 0x800A6D0C, 0x800096A4), as are
 * shared_ptr<CameraMode>'s destructor (0x80009844), the update handler's destructor (0x80009C14)
 * and list1<UpdateEventArgs &>::operator[](arg<1>) (0x800A62CC). */
class CameraModeUser : public CameraMode {
  public:
    /* UpdateEventArgs (class name from the RTTI of a bound callback): the update event. Its
     * contents are unknown. */
    struct UpdateEventArgs;
    /* SwitchEventArgs (class name from the RTTI of a bound callback, EggLastMech's camera): the
     * event raised when the mode is switched to (meaning inferred). Its contents are unknown. */
    struct SwitchEventArgs;

    typedef boost::function1<void, UpdateEventArgs &> UpdateCallback;
    typedef boost::function1<void, SwitchEventArgs &> SwitchCallback;

    /* 0x04 */ u8 field_0x4[0xAC - 0x4];
    /* 0xAC */ SwitchCallback m_onSwitch;
    /* 0xB8 */ u8 field_0xB8[0xC4 - 0xAC - sizeof(SwitchCallback)];
    /* 0xC4 */ UpdateCallback m_onUpdate;
};

} // namespace Camera
} // namespace System
} // namespace SonicteamUSA

/* A camera mode change's blend (class name unknown; layout from its constructor, which stores the
 * time twice and clears the rest; meaning inferred). Its constructor and destructor are out of line
 * (0x8000B2C0, 0x8000B228). */
struct UnkCameraBlend_8000B2C0 {
    UnkCameraBlend_8000B2C0(float time);
    ~UnkCameraBlend_8000B2C0();

    /* 0x00 */ float m_time;
    /* 0x04 */ float field_0x4;
    /* 0x08 */ u32 field_0x8;
    /* 0x0C */ u8 field_0xC[0x2C - 0xC];
};
SIZE_ASSERT(UnkCameraBlend_8000B2C0, 0x2C);

/* The camera manager (class name unknown): a function-local static object behind an accessor of
 * the player code (0x80099954). */
class UnkCameraManager_80099954;

extern "C" {
UnkCameraManager_80099954 *fn_80099954(void);
/* Pushes a camera mode with a blend, and removes it again (behaviour inferred; the meanings of the
 * integer arguments are unknown). */
void fn_801CEA54(UnkCameraManager_80099954 *manager,
                 const boost::shared_ptr<SonicteamUSA::System::Camera::CameraMode> &mode, int arg2,
                 const UnkCameraBlend_8000B2C0 &blend, int arg4, int arg5);
void fn_801CE9F4(UnkCameraManager_80099954 *manager,
                 const boost::shared_ptr<SonicteamUSA::System::Camera::CameraMode> &mode);
/* Creates a user camera mode (behaviour inferred; the arguments' meanings are unknown). */
boost::shared_ptr<SonicteamUSA::System::Camera::CameraModeUser> fn_80009704(int arg0, int arg1);
/* Assigns source to a user camera mode's update handler (behaviour inferred, as fn_800729B0 does for
 * command units). */
SonicteamUSA::System::Camera::CameraModeUser::UpdateCallback *
fn_800A673C(SonicteamUSA::System::Camera::CameraModeUser::UpdateCallback *callback,
            const SonicteamUSA::System::Camera::CameraModeUser::UpdateCallback *source);
/* Assigns source to a user camera mode's switch handler (behaviour inferred, as fn_800A673C). */
SonicteamUSA::System::Camera::CameraModeUser::SwitchCallback *
fn_800A6A24(SonicteamUSA::System::Camera::CameraModeUser::SwitchCallback *callback,
            const SonicteamUSA::System::Camera::CameraModeUser::SwitchCallback *source);
/* Sets a user camera mode's eye and point looked at, and its up vector (behaviour inferred). */
void fn_8000A5D4(SonicteamUSA::System::Camera::CameraModeUser *mode, const Vec3f *eye,
                 const Vec3f *target);
void fn_8000ACE4(SonicteamUSA::System::Camera::CameraModeUser *mode, const Vec3f *up);
}

#endif
