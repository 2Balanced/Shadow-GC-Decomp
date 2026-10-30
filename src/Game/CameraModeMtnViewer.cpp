/* SonicteamUSA::System::Camera::CameraModeMtnViewer (class name and base from RTTI; mode -0x3FF): a
 * debug camera that orbits the point it looks at with the first pad's sticks, raises and lowers
 * itself with two buttons and zooms with two more (meaning of the buttons inferred). The unit also
 * emits the first copies of the game's out-of-line vector helpers (construct from components,
 * scale, assign, copy-construct) and the pad manager's accessor, which many units call. File name
 * from the class; other names are inferred. Definitions are in reverse address order (deferred
 * inlining emits functions last-defined first). */
#include "Game/CameraModeUser.h"
#include "Game/CameraState.h"
#include "Game/MathConstants.h"
#include "Game/MathTypes.h"
#include "size_assert.h"

using SonicteamUSA::System::Camera::CameraMode;

/* A stick of a pad (struct name unknown; see Game/GamePeripheralTask.cpp). */
struct UnkPadStick_8040DD24 {
    /* 0x0 */ float x;
    /* 0x4 */ float y;
    /* 0x8 */ float field_0x8;
};

/* A pad of the pad manager (struct name unknown; see Game/GamePeripheralTask.cpp). */
struct UnkPad_8040DB6C {
    /* 0x00 */ u32 m_down;
    /* 0x04 */ u32 m_up;
    /* 0x08 */ u32 m_pressed;
    /* 0x0C */ u32 m_released;
    /* 0x10 */ UnkPadStick_8040DD24 m_stick;
    /* 0x1C */ UnkPadStick_8040DD24 m_subStick;
};

extern "C" {
/* The pad manager's constructor and destructor, and one of its pads (behaviour inferred). */
void *fn_8040DC5C(void *object);
void fn_8040DB80(void *object, int flags);
UnkPad_8040DB6C *fn_8040DB6C(void *manager, u32 index);
void *__register_global_object(void *object, void *destructor, void *record);

/* Vector and matrix routines (see Game/MathTypes.h). */
Vec3f fn_8040CDA8(const Vec3f *vector, float scale);
Vec3f fn_8040CDD0(const Vec3f *a, const Vec3f *b);
Vec3f fn_8040B67C(const Mtx34f *matrix, const Vec3f *point);
void fn_8040CFF8(Vec3f *vector, const Vec3f *value);
void fn_8003B9E8(const Vec3f *direction, float *pitch, float *yaw);
}

/* fn_8040CD74 called as returning a - b by value (see Game/BKTank.cpp). */
typedef Vec3f (*VectorSubtractFn)(const Vec3f *a, const Vec3f *b);

namespace SonicteamUSA {
namespace System {
namespace Camera {

class CameraModeMtnViewer : public CameraMode {
  public:
    CameraModeMtnViewer(UnkCameraOwner_8000E9BC *owner, int screen) : CameraMode(owner, screen) {}

    static CameraMode *Create(UnkCameraOwner_8000E9BC *owner, int screen);

    virtual int GetModeId();
    virtual void Update(float dt, UnkCameraUpdateArg_800099B8 *arg);
    virtual void OnSwitch(UnkCameraSwitchArg_8000996C *arg);
    virtual void vf_0x14(UnkCameraArg_800098E4 *arg);
    virtual ~CameraModeMtnViewer();
};
SIZE_ASSERT(CameraModeMtnViewer, 0xAC);

} // namespace Camera
} // namespace System
} // namespace SonicteamUSA

using SonicteamUSA::System::Camera::CameraModeMtnViewer;

/* The pointer to the pad manager, placed with the other singletons' instance pointers at the start
 * of .sbss (see Game/GamePeripheralTask.cpp), and its function-local static. */
extern void *lbl_805EDF8C;
static s8 s_padManagerInitialized;
static u32 s_padManagerRecord[3];
static u32 s_padManagerObject[47];

/* A camera mode starts with a placement. */
inline UnkCameraState_8000AA70 *ModeState(CameraMode *mode) {
    return reinterpret_cast<UnkCameraState_8000AA70 *>(mode);
}

CameraModeMtnViewer::~CameraModeMtnViewer() {}

extern "C" {

/* The pad manager (a function-local static object). */
void *fn_80009208(void) {
    if (lbl_805EDF8C == 0) {
        if (!s_padManagerInitialized) {
            __register_global_object(fn_8040DC5C(s_padManagerObject), fn_8040DB80,
                                     s_padManagerRecord);
            s_padManagerInitialized = 1;
        }
        lbl_805EDF8C = s_padManagerObject;
    }
    return lbl_805EDF8C;
}

Vec3f *fn_800091EC(Vec3f *out, const Vec3f *from) {
    out->x = from->x;
    out->y = from->y;
    out->z = from->z;
    return out;
}

Vec3f *fn_800091D0(Vec3f *out, const Vec3f *from) {
    out->x = from->x;
    out->y = from->y;
    out->z = from->z;
    return out;
}

/* vector * scale */
Vec3f fn_800091A4(const Vec3f *vector, float scale) {
    return fn_8040CDA8(vector, scale);
}

} // extern "C"

void CameraModeMtnViewer::Update(float dt, UnkCameraUpdateArg_800099B8 *arg) {
    UnkPad_8040DB6C *pad = fn_8040DB6C(fn_80009208(), 0);
    if (pad == 0 || (pad->m_down & 0x4000) == 0x4000 || (pad->m_down & 0x8000) == 0x8000) {
        return;
    }

    Mtx34f rotation;
    Mtx34f moveRotation;
    Vec3f direction;
    Vec3f eye;
    fn_800091EC(&eye, &m_eye);
    Vec3f target;
    fn_800091EC(&target, &m_target);

    float turnX = pad->m_subStick.x;
    float turnY = pad->m_subStick.y;
    fn_8040C148(&rotation, &lbl_805E0720);
    fn_800091D0(&direction, &((VectorSubtractFn)fn_8040CD74)(&eye, &target));
    Vec3f angles(0.0f, 0.0f, 0.0f);
    fn_8003B9E8(&direction, &angles.x, &angles.y);
    float turn = 2.0f * turnX * (lbl_805FAD38 / 180.0f);
    fn_8040B848(&rotation, angles.y + turn);
    fn_8040B884(&rotation, 2.0f * turnY * (lbl_805FAD38 / 180.0f));
    fn_8040B848(&rotation, -angles.y);
    fn_800091D0(&eye, &fn_8040CDD0(&fn_8040B67C(&rotation, &direction), &target));

    float speed = fn_8040CF50(&((VectorSubtractFn)fn_8040CD74)(&eye, &target)) / 25.0f;
    float moveX = pad->m_stick.x;
    float moveY = pad->m_stick.y;
    fn_8040C148(&moveRotation, &lbl_805E0720);
    fn_800091D0(&direction, &((VectorSubtractFn)fn_8040CD74)(&eye, &target));
    Vec3f moveAngles(0.0f, 0.0f, 0.0f);
    fn_8003B9E8(&direction, &moveAngles.x, &moveAngles.y);
    fn_8040B848(&moveRotation, moveAngles.y);
    fn_800091D0(&direction, &Vec3f(moveX, 0.0f, moveY));
    fn_800091D0(&direction, &fn_8040B67C(&moveRotation, &direction));
    fn_8040CFF8(&eye, &fn_800091A4(&direction, speed));
    fn_8040CFF8(&target, &fn_800091A4(&direction, speed));

    if ((pad->m_down & 0x40) == 0x40) {
        eye.y += 0.5f * speed;
        target.y += 0.5f * speed;
    }
    if ((pad->m_down & 0x80) == 0x80) {
        eye.y -= 0.5f * speed;
        target.y -= 0.5f * speed;
    }

    float zoom = 1.0f;
    if ((pad->m_down & 0x1000) == 0x1000) {
        zoom += 0.03f;
    }
    if ((pad->m_down & 0x2000) == 0x2000) {
        zoom -= 0.03f;
    }
    if (fn_8040CF50(&((VectorSubtractFn)fn_8040CD74)(&eye, &target)) > 0.1f || zoom > 1.0f) {
        fn_800091D0(&eye, &fn_8040CDD0(&fn_8040CDA8(&((VectorSubtractFn)fn_8040CD74)(&eye, &target),
                                                    zoom),
                                       &target));
    }
    fn_8000A5D4(this, &eye, &target);
}

extern "C" Vec3f *fn_80008E1C(Vec3f *out, float x, float y, float z) {
    out->x = x;
    out->y = y;
    out->z = z;
    return out;
}

void CameraModeMtnViewer::OnSwitch(UnkCameraSwitchArg_8000996C *arg) {
    ModeState(this)->m_fov = 0.0f;
    Vec3f eye;
    fn_80008E1C(&eye, 0.0f, 10.0f, 20.0f);
    fn_8000A5D4(this, &eye, &m_target);
    Vec3f target;
    fn_80008E1C(&target, 0.0f, 0.0f, 0.0f);
    fn_8000A5D4(this, &m_eye, &target);
}

void CameraModeMtnViewer::vf_0x14(UnkCameraArg_800098E4 *arg) {}

CameraMode *CameraModeMtnViewer::Create(UnkCameraOwner_8000E9BC *owner, int screen) {
    return new CameraModeMtnViewer(owner, screen);
}

void CameraMode::vf_0x18(int flags) {}

int CameraModeMtnViewer::GetModeId() {
    return -0x3FF;
}
