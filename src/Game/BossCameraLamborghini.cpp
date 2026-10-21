/* Boss::Camera::LamborghiniImpl (class name from the RTTI of its bound callbacks): a boss camera
 * that keeps the player in front and the boss behind. It pushes a user camera mode (CameraModeUser)
 * whose eye sits behind the player, on the line from the boss, and looks at the boss. Each frame
 * both points follow their characters within ranges, the boss point is kept within pitch limits
 * seen from the player, and it swings around the player at a limited angular speed. Its owner is a
 * pimpl wrapper (EggLastMech's UnkCameraCtrl_8031A268, probably Boss::Camera::Lamborghini) whose
 * methods EggLastMechCamera calls by their fn_ names. File name inferred from the class. Names
 * marked "(name inferred)" are not original. Definitions are in reverse address order (deferred
 * inlining emits functions last-defined first). */
#include <boost/bind.hpp>
#include <boost/function.hpp>
#include <boost/scoped_ptr.hpp>

#include "Game/CameraModeUser.h"
#include "Game/CharInfo.h"
#include "Game/CharaColliBaseCharInfo.h"
#include "Game/ExternalCameraRequest.h"
#include "Game/WeaponBullet.h"
#include "Game/MathTypes.h"
#include "boost/shared_ptr.hpp"
#include "boost/weak_ptr.hpp"
#include <dolphin/types.h>
#include "size_assert.h"

using SonicteamUSA::System::Camera::CameraMode;
using SonicteamUSA::System::Camera::CameraModeUser;

extern const float lbl_805FAD38; /* pi */

extern "C" {
/* Vector helpers (behaviour inferred from their bodies). */
Vec3f fn_8040CDD0(const Vec3f *a, const Vec3f *b); /* a + b */
Vec3f fn_8040CDA8(const Vec3f *vector, float scale);      /* vector * scale */
Vec3f fn_8040CEE8(const Vec3f *vector);                   /* normalised */
Vec3f fn_8040CFB8(const Vec3f *a, const Vec3f *b); /* cross product */
float fn_8040CE74(Vec3f *vector); /* normalises; returns the former length */
void fn_8040CE60(Vec3f *vector);  /* zero */
/* The angle between two unit vectors (behaviour inferred). */
float fn_8040F8FC(const Vec3f *a, const Vec3f *b);
/* vector turned by angle about the y axis (behaviour inferred). */
Vec3f fn_8040D0AC(const Vec3f *vector, float angle);
/* vector turned by the Euler angles rotation in the given axis order (behaviour inferred). */
Vec3f fn_80410098(const Vec3f *vector, const Vec3f *rotation, int order);
/* A rotation by angle about axis, and a point transformed by a matrix (behaviour inferred). */
Mtx34f fn_8040FD8C(const Vec3f *axis, float angle);
Vec3f fn_8040B67C(const Mtx34f *matrix, const Vec3f *point);
/* Whether value is about zero, and the tangent of angle (behaviour inferred). */
bool fn_8000A950(float value);
float fn_8000F3D4(float angle);
/* value limited to [min, max] (behaviour from its body). */
const float &fn_80070460(const float &value, const float &min, const float &max);
}

typedef Vec3f (*VectorSubtractFn)(const Vec3f *a, const Vec3f *b);

/* Sets mode's eye and the point it looks at (name inferred). */
inline void SetView(CameraModeUser *mode, const Vec3f *eye, const Vec3f *target) {
    fn_8000A5D4(mode, eye, target);
}

/* Degrees to radians (name inferred). */
inline float DegToRad(float degrees) {
    return degrees * (lbl_805FAD38 / 180.0f);
}

namespace Boss {
namespace Camera {

class LamborghiniImpl {
  public:
    LamborghiniImpl(const boost::weak_ptr<CharInfo> &player, const boost::weak_ptr<CharInfo> &boss);
    ~LamborghiniImpl();

    /* Puts the camera mode on the stage camera, with blend if given (name inferred). */
    void Start(const UnkCameraBlend_8000B2C0 *blend);
    /* Withdraws the camera mode (name inferred). */
    void Stop() {
        fn_801CEAD4(&m_handle);
    }
    /* Moves both points on by dt (name inferred). */
    void Tick(float dt);
    void Step(float dt) {
        Tick(dt);
    }

    /* The offsets and ranges the points follow their characters within (names inferred). */
    void SetPlayerOffset(const Vec3f *offset);
    void SetBossOffset(const Vec3f *offset);
    void SetPlayerRange(const Vec2f *range);
    void SetBossRange(const Vec2f *range);
    /* The pitch limits of the boss seen from the player (names inferred). */
    void SetPitchLimits(float min, float max) {
        m_minPitch = min;
        m_maxPitch = max;
    }

    /* Where a point following chara goes from current (names inferred). */
    Fn8040CopyVector3 Follow(const Vec3f &current, const boost::weak_ptr<CharInfo> &chara,
                             const Vec3f &offset, const Vec2f &range);
    Fn8040CopyVector3 Limit(const Vec3f &current, const Vec3f &target,
                            const Vec2f &range);

    /* The camera mode's events (names inferred). */
    void OnSwitchIn(CameraModeUser::SwitchEventArgs &args);
    void OnSwitchOut(CameraModeUser::SwitchEventArgs &args);
    void OnUpdate(CameraModeUser::UpdateEventArgs &args);

    /* 0x00 */ boost::weak_ptr<PJSCamera::ExternalCameraRequest> m_handle;
    /* 0x08 */ boost::shared_ptr<CameraModeUser> m_mode;
    /* 0x10 */ boost::weak_ptr<CharInfo> m_player;
    /* 0x18 */ boost::weak_ptr<CharInfo> m_boss;
    /* 0x20 */ Vec3f m_bossPoint;
    /* 0x2C */ Vec3f m_playerPoint;
    /* 0x38 */ Vec3f m_playerOffset;
    /* 0x44 */ Vec3f m_playerOffset2; /* set with m_playerOffset (meaning unknown) */
    /* 0x50 */ Vec3f m_bossOffset;
    /* 0x5C */ Vec3f m_bossOffset2; /* set with m_bossOffset (meaning unknown) */
    /* 0x68 */ u8 m_turning;
    /* 0x6C */ float m_turnSign;
    /* 0x70 */ Vec2f m_playerRange; /* x: distance, y: height */
    /* 0x78 */ Vec2f m_bossRange;
    /* 0x80 */ float m_minPitch;
    /* 0x84 */ float m_maxPitch;
    /* 0x88 */ float m_distance;
    /* 0x8C */ Vec3f m_eyeOffset;
};
SIZE_ASSERT(LamborghiniImpl, 0x98);

} // namespace Camera
} // namespace Boss

/* Deletes the camera (name inferred; see checked_delete below). */
inline void DeleteLamborghini(Boss::Camera::LamborghiniImpl *impl) {
    delete impl;
}

/* Retail keeps boost::checked_delete<LamborghiniImpl> out of line (0x8031A244) and the destructor at
 * its definition; see Rocket.cpp and docs/research-notes.md. */
namespace boost {
template<> inline void checked_delete<Boss::Camera::LamborghiniImpl>(Boss::Camera::LamborghiniImpl *x) {
    DeleteLamborghini(x);
}
} // namespace boost

namespace EggLastMech {

/* The pimpl wrapper of LamborghiniImpl (Game/EggLastMech.h declares it with a placeholder member;
 * its methods keep the fn_ names EggLastMechCamera.cpp calls them by). */
class UnkCameraCtrl_8031A268 {
  public:
    UnkCameraCtrl_8031A268(const boost::weak_ptr<CharInfo> &player, const boost::weak_ptr<CharInfo> &owner);

    /* The pitch limits (name inferred). */
    void SetPitchLimits(float min, float max);

    /* 0x0 */ boost::scoped_ptr<Boss::Camera::LamborghiniImpl> m_impl;
};
SIZE_ASSERT(UnkCameraCtrl_8031A268, 0x4);

extern "C" {
void fn_8031A0A0(UnkCameraCtrl_8031A268 *ctrl, const Vec2f *value);
void fn_8031A0C4(UnkCameraCtrl_8031A268 *ctrl, const Vec3f *value);
void fn_8031A0E8(UnkCameraCtrl_8031A268 *ctrl, const Vec2f *value);
void fn_8031A10C(UnkCameraCtrl_8031A268 *ctrl, const Vec3f *value);
void fn_8031A130(UnkCameraCtrl_8031A268 *ctrl, float dt);
void fn_8031A164(UnkCameraCtrl_8031A268 *ctrl);
void fn_8031A188(UnkCameraCtrl_8031A268 *ctrl, const UnkCameraBlend_8000B2C0 *blend);
UnkCameraCtrl_8031A268 *fn_8031A1AC(UnkCameraCtrl_8031A268 *ctrl, int flags);
}

} // namespace EggLastMech

namespace Boss {
namespace Camera {

LamborghiniImpl::LamborghiniImpl(const boost::weak_ptr<CharInfo> &player,
                                 const boost::weak_ptr<CharInfo> &boss)
    : m_player(player), m_boss(boss) {
    m_mode = fn_80009704(0, 0);
    fn_800A6A24(&m_mode.get()->m_onSwitch,
                &(const CameraModeUser::SwitchCallback &)CameraModeUser::SwitchCallback(
                    boost::bind(&LamborghiniImpl::OnSwitchIn, this, _1)));
    fn_800A673C(&m_mode.get()->m_onUpdate,
                &(const CameraModeUser::UpdateCallback &)CameraModeUser::UpdateCallback(
                    boost::bind(&LamborghiniImpl::OnUpdate, this, _1)));
    fn_800A6A24(reinterpret_cast<CameraModeUser::SwitchCallback *>(m_mode.get()->field_0xB8),
                &(const CameraModeUser::SwitchCallback &)CameraModeUser::SwitchCallback(
                    boost::bind(&LamborghiniImpl::OnSwitchOut, this, _1)));
    fn_8040CE60(&m_playerOffset);
    fn_8040CE60(&m_playerOffset2);
    fn_8040CE60(&m_bossOffset);
    fn_8040CE60(&m_bossOffset2);
    m_playerRange.x = 10.0f;
    m_playerRange.y = 20.0f;
    m_bossRange.x = 80.0f;
    m_bossRange.y = 20.0f;
    m_minPitch = DegToRad(-45.0f);
    m_maxPitch = DegToRad(15.0f);
    m_distance = 70.0f;
    fn_800091D0(&m_eyeOffset, &Vec3f(0.0f, 15.0f, 0.0f));
    fn_800091D0(&m_playerPoint, &player.lock()->m_position);
    fn_800091D0(&m_bossPoint, &boss.lock()->m_position);
    m_turning = 0;
}

LamborghiniImpl::~LamborghiniImpl() {}

void LamborghiniImpl::Start(const UnkCameraBlend_8000B2C0 *blend) {
    if (m_handle.expired()) {
        if (blend != 0) {
            m_handle = fn_801CED24(boost::shared_ptr<CameraMode>(m_mode), 0, 4, (int)blend, 0, 0, 0);
        } else {
            m_handle = fn_801CED24(boost::shared_ptr<CameraMode>(m_mode), 0, 0, 0, 0, 0, 0);
        }
    }
}

void LamborghiniImpl::SetPlayerOffset(const Vec3f *offset) {
    fn_800091D0(&m_playerOffset2, offset);
    fn_800091D0(&m_playerOffset, offset);
}

void LamborghiniImpl::SetPlayerRange(const Vec2f *range) {
    m_playerRange = *range;
}

void LamborghiniImpl::SetBossOffset(const Vec3f *offset) {
    fn_800091D0(&m_bossOffset2, offset);
    fn_800091D0(&m_bossOffset, offset);
}

void LamborghiniImpl::SetBossRange(const Vec2f *range) {
    m_bossRange = *range;
}

Fn8040CopyVector3 LamborghiniImpl::Limit(const Vec3f &current, const Vec3f &target,
                                         const Vec2f &range) {
    Vec3f result;
    fn_800091EC(&result, &current);
    result.y = fn_80070460(result.y, target.y - range.y, target.y + range.y);
    Vec3f offset;
    fn_800091EC(&offset, &(const Vec3f &)((VectorSubtractFn)fn_8040CD74)(&result, &target));
    if (fn_8040CF50(&offset) > range.x) {
        fn_800091D0(&result, &(const Vec3f &)fn_8040CDD0(
                                 &target, &(const Vec3f &)fn_8040CDA8(
                                              &(const Vec3f &)fn_8040CEE8(&offset), range.x)));
    }
    return reinterpret_cast<const Fn8040CopyVector3 &>(result);
}

Fn8040CopyVector3 LamborghiniImpl::Follow(const Vec3f &current,
                                          const boost::weak_ptr<CharInfo> &chara,
                                          const Vec3f &offset, const Vec2f &range) {
    if (chara.expired()) {
        return reinterpret_cast<const Fn8040CopyVector3 &>(current);
    }
    boost::shared_ptr<CharInfo> info = chara.lock();
    Vec3f target;
    fn_800091EC(&target, &(const Vec3f &)fn_8040CDD0(&info->m_position, &offset));
    return Limit(current, target, range);
}

void LamborghiniImpl::Tick(float dt) {
    if (m_boss.expired() || m_player.expired()) {
        Stop();
        return;
    }
    Vec3f player;
    fn_800091EC(&player, &(const Vec3f &)Follow(m_playerPoint, m_player, m_playerOffset,
                                                        m_playerRange));
    Vec3f move;
    fn_800091EC(&move,
                &(const Vec3f &)((VectorSubtractFn)fn_8040CD74)(&player, &m_playerPoint));
    Vec3f boss;
    fn_800091EC(&boss, &(const Vec3f &)fn_8040CDD0(&m_bossPoint, &move));
    fn_800091D0(&boss, &(const Vec3f &)Follow(boss, m_boss, m_bossOffset, m_bossRange));
    Vec3f oldOffset;
    fn_800091EC(&oldOffset,
                &(const Vec3f &)((VectorSubtractFn)fn_8040CD74)(&m_bossPoint, &m_playerPoint));
    Vec3f offsetNow;
    fn_800091EC(&offsetNow, &(const Vec3f &)((VectorSubtractFn)fn_8040CD74)(&boss, &player));
    Vec3f oldFlat(oldOffset.x, 0.0f, oldOffset.z);
    Vec3f newFlat(offsetNow.x, 0.0f, offsetNow.z);
    float flat = fn_8040CF50(&newFlat);
    float height = offsetNow.y;
    if (!fn_8000A950(flat)) {
        float slope = height / flat;
        if (height >= 0.0f) {
            float limit = fn_8000F3D4(m_maxPitch);
            if (slope > limit) {
                offsetNow.y = flat * limit;
                boss.y = offsetNow.y + player.y;
            }
        } else {
            float limit = fn_8000F3D4(m_minPitch);
            if (slope < limit) {
                offsetNow.y = flat * limit;
                boss.y = offsetNow.y + player.y;
            }
        }
    }
    fn_8040CE74(&oldFlat);
    fn_8040CE74(&newFlat);
    float angle = fn_8040F8FC(&oldFlat, &newFlat);
    float step = dt * DegToRad(300.0f);
    if (angle > step) {
        if (!m_turning) {
            m_turning = 1;
            if (angle > DegToRad(90.0f)) {
                boost::shared_ptr<CharInfo> info = m_player.lock();
                Vec3f forward(0.0f, 0.0f, -1.0f);
                CharInfo *p = info.operator->();
                const Vec3f *rotation = &p->m_rotation;
                int order = p->field_0x30;
                fn_800091D0(&forward, &(const Vec3f &)fn_80410098(
                                          &Vec3f(0.0f, 0.0f, -1.0f), rotation, order));
                if (fn_8040CFB8(&newFlat, &forward).y >= 0.0f) {
                    m_turnSign = 1.0f;
                } else {
                    m_turnSign = -1.0f;
                }
            } else {
                if (fn_8040CFB8(&oldFlat, &newFlat).y >= 0.0f) {
                    m_turnSign = 1.0f;
                } else {
                    m_turnSign = -1.0f;
                }
            }
        }
        if (angle > DegToRad(45.0f)) {
            Vec3f offset;
            fn_800091EC(&offset, &(const Vec3f &)((VectorSubtractFn)fn_8040CD74)(
                                     &m_bossPoint, &m_playerPoint));
            fn_800091D0(&offset, &(const Vec3f &)fn_8040D0AC(&offset, step * m_turnSign));
            fn_800091D0(&m_bossPoint, &(const Vec3f &)fn_8040CDD0(&offset, &player));
        } else {
            Vec3f axis;
            fn_800091EC(&axis, &(const Vec3f &)fn_8040CEE8(
                                   &(const Vec3f &)fn_8040CFB8(&oldOffset, &offsetNow)));
            Mtx34f rotation;
            fn_8040C148(&rotation, &(const Mtx34f &)fn_8040FD8C(&axis, step));
            Vec3f offset;
            fn_800091EC(&offset, &(const Vec3f &)((VectorSubtractFn)fn_8040CD74)(
                                     &m_bossPoint, &m_playerPoint));
            fn_800091D0(&offset, &(const Vec3f &)fn_8040B67C(&rotation, &offset));
            fn_800091D0(&m_bossPoint, &(const Vec3f &)fn_8040CDD0(&offset, &player));
        }
        fn_800091D0(&m_playerPoint, &player);
    } else {
        m_turning = 0;
        fn_800091D0(&m_bossPoint, &boss);
        fn_800091D0(&m_playerPoint, &player);
    }
}

void LamborghiniImpl::OnSwitchIn(CameraModeUser::SwitchEventArgs &args) {
    if (!m_player.expired()) {
        fn_800091D0(&m_playerPoint, &m_player.lock()->m_position);
    }
    if (!m_boss.expired()) {
        fn_800091D0(&m_bossPoint, &m_boss.lock()->m_position);
    }
    m_turning = 0;
    Tick(0.0f);
}

void LamborghiniImpl::OnSwitchOut(CameraModeUser::SwitchEventArgs &args) {}

void LamborghiniImpl::OnUpdate(CameraModeUser::UpdateEventArgs &args) {
    Vec3f target;
    fn_800091EC(&target, &m_bossPoint);
    Vec3f eye;
    Vec3f direction;
    fn_800091EC(&direction, &(const Vec3f &)((VectorSubtractFn)fn_8040CD74)(&m_playerPoint,
                                                                                    &m_bossPoint));
    fn_8040CE74(&direction);
    fn_800091D0(&eye, &(const Vec3f &)fn_8040CDD0(
                          &(const Vec3f &)fn_8040CDD0(
                              &m_playerPoint, &(const Vec3f &)fn_8040CDA8(&direction, m_distance)),
                          &m_eyeOffset));
    SetView(m_mode.operator->(), &eye, &target);
}

} // namespace Camera
} // namespace Boss

namespace EggLastMech {

UnkCameraCtrl_8031A268::UnkCameraCtrl_8031A268(const boost::weak_ptr<CharInfo> &player,
                                               const boost::weak_ptr<CharInfo> &owner)
    : m_impl(new Boss::Camera::LamborghiniImpl(player, owner)) {}

extern "C" UnkCameraCtrl_8031A268 *fn_8031A1AC(UnkCameraCtrl_8031A268 *ctrl, int flags) {
    if (ctrl != 0) {
        ctrl->m_impl.~scoped_ptr();
        if ((s16)flags > 0) {
            operator delete(ctrl);
        }
    }
    return ctrl;
}

extern "C" void fn_8031A188(UnkCameraCtrl_8031A268 *ctrl, const UnkCameraBlend_8000B2C0 *blend) {
    ctrl->m_impl.get()->Start(blend);
}

extern "C" void fn_8031A164(UnkCameraCtrl_8031A268 *ctrl) {
    ctrl->m_impl->Stop();
}

extern "C" void fn_8031A130(UnkCameraCtrl_8031A268 *ctrl, float dt) {
    ctrl->m_impl->Step(dt);
}

extern "C" void fn_8031A10C(UnkCameraCtrl_8031A268 *ctrl, const Vec3f *value) {
    ctrl->m_impl.get()->SetPlayerOffset(value);
}

extern "C" void fn_8031A0E8(UnkCameraCtrl_8031A268 *ctrl, const Vec2f *value) {
    ctrl->m_impl.get()->SetPlayerRange(value);
}

extern "C" void fn_8031A0C4(UnkCameraCtrl_8031A268 *ctrl, const Vec3f *value) {
    ctrl->m_impl.get()->SetBossOffset(value);
}

extern "C" void fn_8031A0A0(UnkCameraCtrl_8031A268 *ctrl, const Vec2f *value) {
    ctrl->m_impl.get()->SetBossRange(value);
}

void UnkCameraCtrl_8031A268::SetPitchLimits(float min, float max) {
    m_impl->SetPitchLimits(min, max);
}

} // namespace EggLastMech
