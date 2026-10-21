/* WatchCamera::SetWatchCamera (class name from RTTI): the WATCH CAMERA gadget, a security camera
 * (WATCHCAMERA.BIN) with a box trigger ("posX/Y/Z", "angX/Y/Z", "width", "height", "depth"). While
 * a character is inside the trigger it shows the stage through a user camera mode placed at the
 * camera and looking at "lookPosX/Y/Z", with a monitor frame (WatchCamera::Monitor) in front of
 * the view and a sight (WatchCamera::Sight) that wanders around the player. Once the camera has a
 * clear line to the player the sight locks on, and after two seconds the camera starts firing
 * bullets at the sight. File name inferred from the class. Method and member names are inferred.
 * Definitions are in reverse address order (deferred inlining emits functions last-defined
 * first). */
#include <boost/bind.hpp>
#include <boost/function.hpp>

#include "Effect/System/EffectElement.h"
#include "Game/CameraModeUser.h"
#include "Game/CharInfo.h"
#include "Game/CharaColli.h"
#include "Game/ExternalCameraRequest.h"
#include "Game/Gadget.h"
#include "Game/GadgetCharInfo.h"
#include "Game/GadgetColliDetectArgs.h"
#include "Game/GadgetModelInfo.h"
#include "Game/GadgetClassDesc.h"
#include "Game/MathConstants.h"
#include "Game/WeaponBullet.h"
#include "Game/FileQueue.h"
#include "boost/shared_ptr.hpp"
#include "boost/weak_ptr.hpp"
#include "size_assert.h"

using Effect::System::EffectElement;
using SonicteamUSA::System::Camera::CameraMode;
using SonicteamUSA::System::Camera::CameraModeUser;
using SonicteamUSA::System::CharaColli::CharaColliBase;

struct _stMtnPlayWork;

/* The handle fn_8015B8C4 wraps a collider in (class name unknown; only the collider is mapped, as
 * in Weight). */
class GadgetCharCollision {
  public:
    /* 0x00 */ u8 field_0x0[0x24];
    /* 0x24 */ CharaColliBase *m_collider;
};

/* What a probe along a segment hits (class name unknown; see SetWeapon.cpp). */
struct UnkCollision_80021E24 {
    /* 0x00 */ u8 field_0x0[0xC];
    /* 0x0C */ u32 m_flags;
    /* 0x10 */ u32 field_0x10;
    /* 0x14 */ void *m_object;
};

/* The screen (class name unknown; only its size is used here). */
struct UnkScreen_8000DCC0 {
    /* 0x00 */ u8 field_0x0[0x68];
    /* 0x68 */ float m_width;
    /* 0x6C */ float m_height;
};

struct UnkScreenHolder_8000DCC0 {
    /* 0x0 */ UnkScreen_8000DCC0 *m_screen;
};

class UnkCameraManager_80009548;

/* The data of a motion event (see TurnOver.cpp; names inferred). */
struct UnkMotionEvent_80415A18 {
    /* 0x0 */ s32 id;
    /* 0x4 */ u32 field_0x4;
};
SIZE_ASSERT(UnkMotionEvent_80415A18, 0x8);

extern "C" {
/* The 32-bit flag word (see CityBombSmall.cpp). */
void fn_80014E04(u32 *flags);
void fn_80014E10(u32 *flags);
void fn_80014E1C(u32 *flags, int bit);
bool fn_80014E34(u32 *flags, int bit);
void fn_8006127C(u32 *flags, int bit);

/* Character infos, models and sounds (see GadgetUtil.h, which this unit does not include: it reads
 * pi as the constant Game/MathConstants.h declares). */
boost::shared_ptr<GadgetCharInfo> fn_8015CD90(int arg0, int arg1);
void fn_8015CEA0(GadgetCharInfo *charInfo, Mtx34f *matrix);
void *fn_80053164(void);
void fn_801D560C(void *manager, int soundId, int arg2, int arg3);
void fn_801D54F4(void *manager, u16 id, const void *source, int arg3, int arg4);
boost::shared_ptr<GadgetModelInfo> fn_8015F800(int arg0, const char *fileName);
void fn_80160A84(GadgetModelInfo *model, int index, int motion);
void fn_8016271C(GadgetModelInfo *model);
void fn_80162790(GadgetModelInfo *model);
boost::weak_ptr<_stMtnPlayWork> fn_801602B4(GadgetModelInfo *model, int index);
int fn_80415A18(_stMtnPlayWork *work, int arg1, int event, void *data, int arg4);

/* Starts effect id at position, pointing along direction (behaviour inferred; see
 * CandleStick.cpp). */
boost::weak_ptr<EffectElement> fn_80168C48(const Vec3f *position,
                                           const Vec3f *direction, int id);

/* The players: the one with index index, and the one with id id (behaviour inferred). */
boost::weak_ptr<CharInfo> fn_800A8C54(UnkCharaList_80074194 *manager, int index);
boost::weak_ptr<CharInfo> fn_800A8B74(UnkCharaList_80074194 *manager, int id);

/* Vector and matrix helpers (behaviour inferred). */
Vec3f fn_800091A4(const Vec3f *vector, float scale);
Vec3f fn_8040B67C(const Mtx34f *matrix, const Vec3f *point);
Vec3f fn_8040CDD0(const Vec3f *a, const Vec3f *b);
Vec3f fn_8040CEE8(const Vec3f *vector);
void fn_8040CE38(Vec3f *vector, float scale);
void fn_8040CE60(Vec3f *vector);
void fn_8040CE74(Vec3f *vector);
void fn_8040CFF8(Vec3f *vector, const Vec3f *value); /* vector += value */
void fn_8040FAA0(Vec3f *translation, const Mtx34f *matrix);
void fn_8040FA20(Mtx34f *matrix, const Vec3f *translation);
float fn_8040ECF8(void); /* a random number from 0 to 1 */

/* The camera: its manager, its matrix and the point it looks at, and the screen (behaviour
 * inferred). */
UnkCameraManager_80009548 *fn_80009548(void);
const Mtx34f *fn_800D4E3C(UnkCameraManager_80009548 *manager, int index);
const Vec3f *fn_800101F8(UnkCameraManager_80009548 *manager, int index);
UnkScreenHolder_8000DCC0 *fn_8000DCC0(void);
/* The user camera mode: it is released, and its eye and the point looked at are set (see
 * GunLift.cpp). */
void fn_80009618(boost::shared_ptr<CameraModeUser> *camera);
void fn_8000DB78(CameraModeUser *mode, const Vec3f *eye);
void fn_8000DB50(CameraModeUser *mode, const Vec3f *target);

/* A probe along a segment (see SetWeapon.cpp). */
void *fn_80020A20(void *segment, const Vec3f *start, const Vec3f *end);
void *fn_80020D40(void);
UnkCollision_80021E24 *fn_80021E24(void *world, void *segment, int arg2, int arg3,
                                   bool (*filter)(UnkCollision_80021E24 *hit), int arg5);
void fn_800265A4(UnkCollision_80021E24 *hit, int arg1);

/* The trigger collider (see Weight.cpp). */
void *fn_80021B08(void);
boost::shared_ptr<CharaColliBase>
fn_800C7D04(void *manager, GadgetCharInfo *charInfo, const Vec3f &size,
            const Vec3f &rotation, int arg4, int arg5, int arg6, int arg7,
            const boost::weak_ptr<CharInfo> &owner, const Vec3f &offset,
            const Vec3f &arg10, int arg11);
extern Vec3f lbl_805E07B8;
boost::shared_ptr<GadgetCharCollision> fn_8015B8C4(boost::shared_ptr<CharaColliBase> collider);
void fn_8015BDC4(GadgetCharCollision *handle, int arg1, int arg2);
void fn_8015BE64(GadgetCharCollision *handle);
void fn_8015BE9C(GadgetCharCollision *handle, float dt);
bool fn_8015AC84(ColliDetectArgs &args);
/* Draws a debug marker at position (behaviour inferred). */
void fn_8015F5D8(const Vec3f *position, float size);

/* Fires a bullet of type type (see BAGunShip.cpp). */
void fn_80051C34(int type, const Weapon::ShotInfo *shot);
}

/* The empty character info and the current player's id (.sbss; meanings inferred). */
extern boost::shared_ptr<CharInfo> lbl_805EE698;
extern int lbl_805EE588;

/* fn_8040CD74 called as returning a - b by value (it stores the result through its first
 * argument). */
typedef Vec3f (*VectorSubtractFn)(const Vec3f *a, const Vec3f *b);

/* The vector type's out-of-line copy constructor and assignment, and vector += value. */
inline void ConstructVector(Vec3f *vector, const Vec3f &value) {
    fn_800091EC(vector, &value);
}

inline void AssignVector(Vec3f &vector, const Vec3f &value) {
    fn_800091D0(&vector, &value);
}

inline void AddVector(Vec3f &vector, const Vec3f &value) {
    fn_8040CFF8(&vector, &value);
}

namespace WatchCamera {

/* A 32-bit flag word (class name unknown). */
struct Flags {
    Flags() {
        fn_80014E04(&m_bits);
    }
    void Clear() {
        fn_80014E10(&m_bits);
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

/* The monitor frame shown in front of the view while the camera is on (class name from the RTTI
 * of its shared_ptr control block). */
class Monitor {
  public:
    Monitor();
    ~Monitor();

    void Show();
    void Hide();
    /* Places the frame in front of the camera, scaled to the screen. */
    void Tick(float dt);

    /* 0x0 */ boost::shared_ptr<GadgetModelInfo> m_model;
};
SIZE_ASSERT(Monitor, 0x8);

/* The sight effect that marks where the camera aims (class name from the RTTI of its shared_ptr
 * control block). Flag bit 3: hidden. */
class Sight {
  public:
    Sight(const Vec3f &position);
    ~Sight();

    /* Replaces the effect with effect id. */
    void SetEffect(int id);
    /* The searching and locked-on looks. */
    void SetSearching();
    void SetLocked();
    void Show();
    void Hide();
    void SetPosition(const Vec3f &position);
    void Tick() {}

    /* 0x00 */ Vec3f m_position;
    /* 0x0C */ Flags m_flags;
    /* 0x10 */ boost::weak_ptr<EffectElement> m_effect;
};
SIZE_ASSERT(Sight, 0x18);

/* Deletes a monitor or a sight (see checked_delete below). */
inline void DeleteMonitor(Monitor *monitor) {
    delete monitor;
}

inline void DeleteSight(Sight *sight) {
    delete sight;
}

} // namespace WatchCamera

/* Retail keeps boost::checked_delete out of line and the destructors at their own places (see
 * docs/research-notes.md and Rocket.cpp). */
namespace boost {
template<> inline void checked_delete<WatchCamera::Monitor>(WatchCamera::Monitor *x) {
    WatchCamera::DeleteMonitor(x);
}
template<> inline void checked_delete<WatchCamera::Sight>(WatchCamera::Sight *x) {
    WatchCamera::DeleteSight(x);
}
} // namespace boost

namespace WatchCamera {

class SetWatchCamera : public Gadget {
  public:
    SetWatchCamera(Task *parent, UnkPlacement_800C9CD0 *owner);
    virtual ~SetWatchCamera();
    virtual void Behave(float dt);
    virtual void fn_800CBC1C();
    virtual void ReloadPlacement(UnkPlacement_800C9CD0 *owner);

    /* Creates the gadget under task layer 14. */
    static void Instantiate(UnkPlacement_800C9CD0 *owner);

    void ReadPlacement(UnkPlacement_800C9CD0 *owner);
    void RebuildTransform();
    /* Places the trigger collider. */
    void UpdateCollider();
    /* The trigger's callback: turns the camera on while a character is inside. */
    void OnColliDetect(ColliDetectArgs &args);
    void StartCamera();
    void StopCamera();
    /* The camera mode's update callback: places the camera. */
    void OnCameraUpdate(CameraModeUser::UpdateEventArgs &args);
    /* Whether nothing is between the camera and the player. */
    bool CanSeePlayer();
    /* Fires at the sight and returns the time to the next shot. */
    float Fire();
    /* Moves the sight to random points around the player. */
    void UpdateSearch();
    /* Moves the sight towards the player. */
    void UpdateAim();

    static const char *s_taskName;

    /* 0x30 */ boost::shared_ptr<GadgetCharInfo> m_chara;
    /* 0x38 */ boost::shared_ptr<GadgetModelInfo> m_model;
    /* 0x40 */ boost::shared_ptr<GadgetCharCollision> m_handle;
    /* 0x48 */ boost::weak_ptr<_stMtnPlayWork> m_motion;
    /* 0x50 */ s32 m_state; /* 0 start, 1 off, 2 searching, 3 locked on, 4 firing */
    /* 0x54: the parameters the owner supplies (names from the parameter table) */
    /* 0x54 */ Vec3f m_position;     /* the trigger's offset */
    /* 0x60 */ Vec3f m_angle;        /* the trigger's rotation, degrees */
    /* 0x6C */ Vec3f m_size;         /* width, height, depth */
    /* 0x78 */ Vec3f m_lookPosition; /* the point looked at, from the camera */
    /* 0x84 */ Vec3f m_lookAngle;
    /* 0x90 */ float m_timer;
    /* 0x94 */ float m_fireTimer;
    /* 0x98 */ Flags m_flags; /* bit 0 firing, 1 touched this frame, 2 camera on, 4 sounds */
    /* 0x9C */ boost::weak_ptr<PJSCamera::ExternalCameraRequest> m_request;
    /* 0xA4 */ boost::shared_ptr<CameraModeUser> m_camera;
    /* 0xAC */ Vec3f m_playerPosition;
    /* 0xB8 */ boost::shared_ptr<Monitor> m_monitor;
    /* 0xC0 */ s32 m_searchState; /* 0 pick a point, 1 move there */
    /* 0xC4 */ float m_aimSpeed;
    /* 0xC8 */ Vec3f m_sightVelocity;
    /* 0xD4 */ Vec3f m_sightTarget;
    /* 0xE0 */ Vec3f m_sightPosition;
    /* 0xEC */ boost::shared_ptr<Sight> m_sight;
    /* 0xF4 */ u32 pad_0xF4; /* the class size is a multiple of 8 */
};
SIZE_ASSERT(SetWatchCamera, 0xF8);

const char *SetWatchCamera::s_taskName = "SetWatchCamera";

/* The line-of-sight probe's filter: objects without flag bit 19 (meaning inferred). Defined first,
 * it is emitted last, after the constructor and its callees. */
static bool IsSolid(UnkCollision_80021E24 *hit) {
    return !(hit->m_flags & 0x80000);
}

SetWatchCamera::SetWatchCamera(Task *parent, UnkPlacement_800C9CD0 *owner)
    : Gadget(parent, owner), m_position(0.0f, 0.0f, 0.0f), m_angle(0.0f, 0.0f, 0.0f),
      m_size(10.0f, 10.0f, 10.0f), m_lookPosition(0.0f, 0.0f, 0.0f), m_lookAngle(0.0f, 0.0f, 0.0f) {
    m_name = s_taskName;
    m_chara = fn_8015CD90(0x87, 4);
    ReadPlacement(owner);
    m_model = fn_8015F800(1, "WATCHCAMERA.BIN");
    m_model->fn_80162A68(0, 0);
    m_model->fn_80160C28(0);
    m_model->fn_8015FF60();
    fn_80160A84(m_model.get(), 0, 1);
    {
        Vec3f size;
        fn_800091EC(&size, &Vec3f(0.0f, 0.0f, 0.0f));
        boost::shared_ptr<CharaColliBase> collider =
            fn_800C7D04(fn_80021B08(), m_chara.get(), size, m_chara->m_rotation, 2, 0x40100,
                        10, 0, lbl_805EE698, lbl_805E07A0, lbl_805E07B8, 0);
        m_handle = fn_8015B8C4(collider);
        fn_800F5980(m_handle.get(), boost::bind(&SetWatchCamera::OnColliDetect, this, _1));
        fn_8015BDC4(m_handle.get(), 2, 1);
        UpdateCollider();
    }
    m_monitor = boost::shared_ptr<Monitor>(new Monitor());
    m_monitor->Hide();
    Vec3f sight(m_lookPosition.x, m_lookPosition.y, m_lookPosition.z);
    AddVector(sight, m_chara->m_position);
    AssignVector(m_sightPosition, sight);
    m_sight = boost::shared_ptr<Sight>(new Sight(m_sightPosition));
    m_sight->Hide();
    m_motion = fn_801602B4(m_model.get(), 0);
    m_flags.Set(4);
    RebuildTransform();
}

SetWatchCamera::~SetWatchCamera() {
    StopCamera();
}

void SetWatchCamera::Behave(float dt) {
    m_flags.Clear(1);
    if (m_model.get() != 0) {
        m_model->fn_80162FB0(dt);
    }
    if (m_handle.get() != 0) {
        fn_8015BE9C(m_handle.get(), dt);
    }
    if (m_monitor.get() != 0) {
        m_monitor->Tick(dt);
    }
    if (m_flags.Test(0)) {
        m_fireTimer -= dt;
        if (m_fireTimer <= 0.0f) {
            m_fireTimer = Fire();
        }
    }
    if (m_sight.get() != 0) {
        m_sight->Tick();
    }
    if (!m_flags.Test(1) && m_flags.Test(2)) {
        StopCamera();
        if (m_model.get() != 0) {
            fn_8016271C(m_model.get());
        }
        m_state = 1;
        m_flags.Clear(0);
        m_flags.Clear(2);
    }

    switch (m_state) {
    case 0:
        m_state = 1;
        m_searchState = 0;
        if (m_sight.get() != 0) {
            m_sight->SetSearching();
        }
        break;
    case 1:
        break;
    case 2:
        if (CanSeePlayer()) {
            m_timer = 2.0f;
            m_state = 3;
            m_aimSpeed = 0.0f;
            if (m_sight.get() != 0) {
                m_sight->SetLocked();
            }
        }
        UpdateSearch();
        break;
    case 3:
        if (!CanSeePlayer()) {
            m_state = 2;
            m_searchState = 0;
            if (m_sight.get() != 0) {
                m_sight->SetSearching();
            }
        } else {
            m_timer -= dt;
            if (m_timer <= 0.0f) {
                m_timer = 1.0f;
                m_state = 4;
                m_flags.Set(0);
            }
        }
        UpdateAim();
        break;
    case 4:
        m_timer -= dt;
        if (m_timer <= 0.0f) {
            m_state = 2;
            m_searchState = 0;
            m_flags.Clear(0);
            if (m_sight.get() != 0) {
                m_sight->SetSearching();
            }
        }
        break;
    case 5:
        break;
    }

    RebuildTransform();
    if (m_flags.Test(4)) {
        if (boost::shared_ptr<_stMtnPlayWork> work = m_motion.lock()) {
            UnkMotionEvent_80415A18 sound;
            if (fn_80415A18(work.get(), 0, 0x5001, &sound, 0) != 0) {
                fn_801D54F4(fn_80053164(), sound.id, m_chara.get(), 0, 1);
            }
        }
    }
}

void SetWatchCamera::fn_800CBC1C() {
    if (m_handle.get() != 0) {
        fn_8015BE64(m_handle.get());
    }
    Vec3f position(m_lookPosition.x, m_lookPosition.y, m_lookPosition.z);
    AddVector(position, m_chara->m_position);
    fn_8015F5D8(&position, 20.0f);
}

void SetWatchCamera::ReloadPlacement(UnkPlacement_800C9CD0 *owner) {
    ReadPlacement(owner);
    RebuildTransform();
    if (m_model.get() != 0) {
        m_model->fn_80162EF4();
    }
    UpdateCollider();
}

void SetWatchCamera::ReadPlacement(UnkPlacement_800C9CD0 *owner) {
    fn_800C9F14(owner, &m_chara->m_position);
    fn_800C9ED4(owner, &m_chara->m_rotation);
    fn_800C9DBC(owner, &m_position, 0x3C);
    m_state = 0;
    m_timer = 0.0f;
    m_fireTimer = 0.0f;
    m_flags.Clear();
    m_aimSpeed = 0.0f;
    m_searchState = 0;
    fn_8040CE60(&m_sightVelocity);
}

void SetWatchCamera::RebuildTransform() {
    if (m_model.get() != 0) {
        Mtx34f matrix;

        fn_8015CEA0(m_chara.get(), &matrix);
        m_model->fn_801633DC(&matrix);
    }
}

void SetWatchCamera::UpdateCollider() {
    if (m_handle.get() != 0) {
        Vec3f position(m_position.x, m_position.y, m_position.z);
        AddVector(position, m_chara->m_position);
        Vec3f rotation(0.0f, 0.0f, 0.0f);
        rotation.x = m_angle.x * (lbl_805FAD38 / 180.0f);
        rotation.y = m_angle.y * (lbl_805FAD38 / 180.0f);
        rotation.z = m_angle.z * (lbl_805FAD38 / 180.0f);
        m_handle->m_collider->SetCharInfo((CharInfo *)&position);
        m_handle->m_collider->SetRotation(&rotation);
        m_handle->m_collider->SetSize(m_size.x, m_size.y, m_size.z, -2);
    }
}

void SetWatchCamera::OnColliDetect(ColliDetectArgs &args) {
    if (fn_8015AC84(args)) {
        if (!m_flags.Test(2)) {
            StartCamera();
            if (m_model.get() != 0) {
                fn_80162790(m_model.get());
            }
            m_state = 2;
            m_flags.Clear(0);
            m_flags.Set(2);
        }
        m_flags.Set(1);
    }
}

void SetWatchCamera::StartCamera() {
    m_camera = fn_80009704(0, 0);
    fn_800A673C(&m_camera.get()->m_onUpdate,
                &(const CameraModeUser::UpdateCallback &)CameraModeUser::UpdateCallback(
                    boost::bind(&SetWatchCamera::OnCameraUpdate, this, _1)));
    m_request = fn_801CED24(boost::shared_ptr<CameraMode>(m_camera), 0, 0, 0, 0, 0, 0);
    if (m_monitor.get() != 0) {
        m_monitor->Show();
    }
    if (m_sight.get() != 0) {
        m_sight->Show();
    }
}

void SetWatchCamera::StopCamera() {
    fn_801CEAD4(&m_request);
    if (m_camera.get() != 0) {
        fn_80009618(&m_camera);
    }
    if (m_monitor.get() != 0) {
        m_monitor->Hide();
    }
    if (m_sight.get() != 0) {
        m_sight->Hide();
    }
}

void SetWatchCamera::OnCameraUpdate(CameraModeUser::UpdateEventArgs &args) {
    if (m_camera.get() != 0) {
        fn_8000DB78(m_camera.get(), &m_chara.get()->m_position);
        Vec3f target(m_lookPosition.x, m_lookPosition.y, m_lookPosition.z);
        AddVector(target, m_chara->m_position);
        fn_8000DB50(m_camera.get(), &target);
    }
}

bool SetWatchCamera::CanSeePlayer() {
    bool visible = true;
    boost::weak_ptr<CharInfo> player = fn_800A8B74(fn_80074194(), lbl_805EE588);
    if (boost::shared_ptr<CharInfo> target = player.lock()) {
        u8 segment[0x18];
        Vec3f start;
        Vec3f end;

        fn_800091EC(&start, &m_chara.get()->m_position);
        fn_800091EC(&end, &target.get()->m_position);
        fn_800091D0(&m_playerPosition, &target.get()->m_position);
        fn_80020A20(segment, &start, &end);
        UnkCollision_80021E24 *hit = fn_80021E24(fn_80020D40(), segment, 3, 0xD, IsSolid, 0);
        if (hit != 0) {
            if (hit->m_object != 0) {
                visible = false;
            }
            fn_800265A4(hit, 1);
        }
    }
    return visible;
}

float SetWatchCamera::Fire() {
    Vec3f origin;
    fn_800091EC(&origin, &m_chara.get()->m_position);
    origin.y += -20.0f;
    Vec3f direction;
    fn_800091EC(&direction,
                &(const Vec3f &)((VectorSubtractFn)fn_8040CD74)(&m_sight->m_position, &origin));
    fn_8040CE74(&direction);
    Weapon::ShotInfo shot(m_chara, true, origin, direction, Vec3f(0.0f, 0.0f, 0.0f));
    fn_80051C34(0x1C, &shot);
    fn_801D560C(fn_80053164(), 0x7840, 0, 0);
    return 0.083333336f;
}

void SetWatchCamera::UpdateSearch() {
    switch (m_searchState) {
    case 0: {
        boost::weak_ptr<CharInfo> player = fn_800A8C54(fn_80074194(), 0);
        Vec3f center;
        if (!player.expired()) {
            if (boost::shared_ptr<CharInfo> target = player.lock()) {
                fn_800091D0(&center, &target.get()->m_position);
            }
        }
        float yaw = 360.0f;
        float distance = 60.0f;
        yaw *= fn_8040ECF8();
        distance *= fn_8040ECF8();
        Vec3f offset(0.0f, 0.0f, distance);
        Mtx34f rotation;
        fn_8040B6F8(&rotation);
        fn_8040B848(&rotation, yaw * (lbl_805FAD38 / 180.0f));
        AssignVector(offset, fn_8040B67C(&rotation, &offset));
        AssignVector(m_sightTarget, fn_8040CDD0(&center, &offset));
        AssignVector(m_sightVelocity, ((VectorSubtractFn)fn_8040CD74)(&m_sightTarget, &m_sightPosition));
        AssignVector(m_sightVelocity, fn_8040CEE8(&m_sightVelocity));
        fn_8040CE38(&m_sightVelocity, 0.8f);
        m_searchState = 1;
        break;
    }
    case 1:
        fn_8040CFF8(&m_sightPosition, &m_sightVelocity);
        if (fn_8040CF50(&(const Vec3f &)((VectorSubtractFn)fn_8040CD74)(
                &m_sightPosition, &m_sightTarget)) <= 5.0f) {
            m_searchState = 0;
        }
        break;
    }
    if (m_sight.get() != 0) {
        m_sight->SetPosition(m_sightPosition);
    }
}

void Sight::SetPosition(const Vec3f &position) {
    AssignVector(m_position, position);
}

void SetWatchCamera::UpdateAim() {
    if (m_sight.get() != 0) {
        Vec3f direction;
        ConstructVector(&direction,
                        ((VectorSubtractFn)fn_8040CD74)(&m_playerPosition, &m_sightPosition));
        m_aimSpeed += 0.02f;
        if (m_aimSpeed > 1.0f) {
            m_aimSpeed = 1.0f;
        }
        AddVector(m_sightPosition, fn_800091A4(&direction, m_aimSpeed));
        m_sight->SetPosition(m_sightPosition);
    }
}

Monitor::Monitor() {
    m_model = fn_8015F800(1, "WATCHCAMERA.BIN");
    m_model->fn_80162A68(1, 0);
}

Monitor::~Monitor() {}

void Monitor::Tick(float dt) {
    if (m_model.get() != 0) {
        Mtx34f matrix;
        Vec3f eye;
        Vec3f direction;

        fn_8040C148(&matrix, fn_800D4E3C(fn_80009548(), 0));
        fn_8040FAA0(&eye, &matrix);
        ConstructVector(&direction,
                        ((VectorSubtractFn)fn_8040CD74)(fn_800101F8(fn_80009548(), 0), &eye));
        fn_8040CE74(&direction);
        fn_8040CE38(&direction, 1.1f);
        fn_8040FA20(&matrix, &(const Vec3f &)fn_8040CDD0(&eye, &direction));
        UnkScreen_8000DCC0 *screen = fn_8000DCC0()->m_screen;
        float scale = 1.1f;
        fn_8040B720(&matrix, 2.0f * (screen->m_width * scale), 2.0f * (screen->m_height * scale),
                    1.0f);
        m_model->fn_801633DC(&matrix);
    }
}

void Monitor::Hide() {
    if (m_model.get() != 0) {
        fn_80162790(m_model.get());
    }
}

void Monitor::Show() {
    if (m_model.get() != 0) {
        fn_8016271C(m_model.get());
    }
}

Sight::Sight(const Vec3f &position) {
    AssignVector(m_position, position);
    m_effect = fn_80168C48(&position, &Vec3f(0.0f, 1.0f, 0.0f), 0x6A);
    if (boost::shared_ptr<EffectElement> effect = m_effect.lock()) {
        effect->SetField4C((u32)this);
    }
}

Sight::~Sight() {
    if (boost::shared_ptr<EffectElement> effect = m_effect.lock()) {
        effect->Retire();
    }
    m_effect.reset();
}

void Sight::Hide() {
    if (boost::shared_ptr<EffectElement> effect = m_effect.lock()) {
        effect->Hide();
    }
    m_flags.Set(3);
}

void Sight::Show() {
    if (boost::shared_ptr<EffectElement> effect = m_effect.lock()) {
        effect->Show();
    }
    m_flags.Clear(3);
}

void Sight::SetLocked() {
    SetEffect(0x6F);
    if (m_flags.Test(3)) {
        Hide();
    }
}

void Sight::SetSearching() {
    SetEffect(0x6A);
    if (m_flags.Test(3)) {
        Hide();
    }
}

void Sight::SetEffect(int id) {
    if (boost::shared_ptr<EffectElement> effect = m_effect.lock()) {
        effect->Retire();
    }
    m_effect.reset();
    m_effect = fn_80168C48(&m_position, &Vec3f(0.0f, 1.0f, 0.0f), id);
    if (boost::shared_ptr<EffectElement> effect = m_effect.lock()) {
        effect->SetField4C((u32)this);
    }
}

} // namespace WatchCamera

extern "C" void fn_80267D90(void) {
    fn_8006B474("WATCHCAMERA.BIN", 1);
}

extern "C" void fn_80267D8C(void) {}

void WatchCamera::SetWatchCamera::Instantiate(UnkPlacement_800C9CD0 *owner) {
    Task *parent = fn_801EB304(fn_8001C4F8(), 14);
    new SetWatchCamera(parent, owner);
}

static GadgetParamDesc lbl_8054C328[] = {
    {4, "posX", 0, 0, 0, 0.0f, -65536.0f, 65536.0f, fn_800CBC20, 0},
    {4, "posY", 0, 0, 0, 0.0f, -65536.0f, 65536.0f, fn_800CBC20, 0},
    {4, "posZ", 0, 0, 0, 0.0f, -65536.0f, 65536.0f, fn_800CBC20, 0},
    {4, "angX", 0, 0, 0, 0.0f, 0.0f, 360.0f, fn_800CBC20, 0},
    {4, "angY", 0, 0, 0, 0.0f, 0.0f, 360.0f, fn_800CBC20, 0},
    {4, "angZ", 0, 0, 0, 0.0f, 0.0f, 360.0f, fn_800CBC20, 0},
    {4, "width", 0, 0, 0, 10.0f, 0.0f, 65536.0f, fn_800CBC20, 0},
    {4, "height", 0, 0, 0, 10.0f, 0.0f, 65536.0f, fn_800CBC20, 0},
    {4, "depth", 0, 0, 0, 10.0f, 0.0f, 65536.0f, fn_800CBC20, 0},
    {4, "lookPosX", 0, 0, 0, 0.0f, -65536.0f, 65536.0f, fn_800CBC20, 0},
    {4, "lookPosY", 0, 0, 0, 0.0f, -65536.0f, 65536.0f, fn_800CBC20, 0},
    {4, "lookPosZ", 0, 0, 0, 0.0f, -65536.0f, 65536.0f, fn_800CBC20, 0},
    {4, "lookAngX", 0, 0, 0, 0.0f, 0.0f, 360.0f, fn_800CBC20, 0},
    {4, "lookAngY", 0, 0, 0, 0.0f, 0.0f, 360.0f, fn_800CBC20, 0},
    {4, "lookAngZ", 0, 0, 0, 0.0f, 0.0f, 360.0f, fn_800CBC20, 0},
    {0},
};

GadgetClassDesc lbl_8054C5A8 = {
    "WATCH CAMERA", fn_80267D90, fn_80267D8C, WatchCamera::SetWatchCamera::Instantiate, 0, 0, 0x1770, 1,
    0, 10, lbl_8054C328,
};

