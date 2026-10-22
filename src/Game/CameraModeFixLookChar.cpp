/* PJSCamera::CameraModeFixLookChar (class name and bases from RTTI): a camera mode that watches one
 * character from beside another. The point looked at follows a character the mode holds (with an
 * offset turned by that character's rotation); the eye sits on the line from that point through
 * the character numbered by CameraMode's second constructor argument, "distance" beyond it and
 * raised by "height", and a collision process keeps it clear of the stage. File name inferred
 * from the class; member and method names other than the classes' are inferred. Definitions are
 * in reverse address order (deferred inlining emits functions last-defined first). */
#include "Game/CameraState.h"
#include "Game/CharInfo.h"
#include "Game/MathTypes.h"
#include "Game/PJSCameraMode.h"
#include "boost/shared_ptr.hpp"
#include "boost/weak_ptr.hpp"
#include "size_assert.h"

using PJSCamera::PJSCameraMode;
using PJSCamera::Collision::ProcessBase;
using SonicteamUSA::System::Camera::CameraMode;

/* The switch argument; only the flags are read (layout from that use). */
class UnkCameraSwitchArg_8000996C {
  public:
    /* 0x00 */ u8 field_0x0[0x14];
    /* 0x14 */ u32 m_flags; /* bit 2: no collision process; bit 3: the 0x14-byte process */
};

/* A camera state fn_8000B3B4 reads from CameraMode's member at 0x34; it is applied to the mode
 * with UnkCameraState_8000AA70's assignment and destroyed with its destructor (see
 * Game/CameraState.h; the type is unknown and the size taken from the caller's frame). */
struct UnkCameraState_8000B3B4 {
    /* 0x00 */ u8 field_0x0[0x40];
};

extern "C" {
void fn_8000B3B4(UnkCameraState_8000B3B4 *state, const void *source);

/* The character with the given number (behaviour inferred). */
boost::weak_ptr<CharInfo> fn_800A8C54(UnkCharaList_80074194 *list, int number);

/* vector *= scale (see GadgetUtil.h). */
void fn_8040CE38(Vec3f *vector, float scale);
}

/* Vector routines called as returning their result by value (they store it through their first
 * argument): vector turned by the Euler angles in the given axis order, a + b, a - b, and
 * a + b again (behaviour inferred). */
typedef Vec3f (*VectorRotateFn)(const Vec3f *vector, const Vec3f *angles, int order);
typedef Vec3f (*VectorBinaryFn)(const Vec3f *a, const Vec3f *b);
extern "C" {
void fn_80410098(void);
void fn_8040CDD0(void);
void fn_80093204(void);
}

/* a + b through fn_8040CDD0 (name inferred). */
inline Vec3f operator+(const Vec3f &a, const Vec3f &b) {
    return ((VectorBinaryFn)fn_8040CDD0)(&a, &b);
}

/* A vector member copy-constructed through the out-of-line copy constructor (fn_800091EC). It
 * does not derive from Vec3f: a constructor that calls Vec3f's inline one is not inlined. */
struct UnkCopiedVector {
    UnkCopiedVector(const Vec3f &from) {
        fn_800091EC((Vec3f *)this, &from);
    }

    const Vec3f *Get() const {
        return (const Vec3f *)this;
    }

    float x;
    float y;
    float z;
};

namespace PJSCamera {

class CameraModeFixLookChar : public PJSCameraMode {
  public:
    CameraModeFixLookChar(UnkCameraOwner_8000E9BC *owner, int number);
    virtual ~CameraModeFixLookChar();
    virtual int GetModeId();
    virtual void Update(float dt, UnkCameraUpdateArg_800099B8 *arg);
    virtual void OnSwitch(UnkCameraSwitchArg_8000996C *arg);
    virtual void vf_0x14(UnkCameraArg_800098E4 *arg);

    /* Creates the mode for the camera factory (name inferred). */
    static CameraMode *Create(UnkCameraOwner_8000E9BC *owner, int number);

    /* 0xAC */ float m_distance;
    /* 0xB0 */ float m_height;
    /* 0xB4 */ boost::weak_ptr<CharInfo> m_chara; /* the character looked at */
    /* 0xBC */ UnkCopiedVector m_offset;
    /* 0xC8 */ boost::shared_ptr<Collision::ProcessBase> m_process;
};
SIZE_ASSERT(CameraModeFixLookChar, 0xD0);

CameraModeFixLookChar::CameraModeFixLookChar(UnkCameraOwner_8000E9BC *owner, int number)
    : PJSCameraMode(owner, number), m_distance(60.0f), m_height(0.0f), m_offset(lbl_805E07A0) {
    UnkCameraState_8000B3B4 state;

    fn_8000B3B4(&state, field_0x34);
    *reinterpret_cast<UnkCameraState_8000AA70 *>(static_cast<CameraMode *>(this)) =
        *reinterpret_cast<UnkCameraState_8000AA70 *>(&state);
    reinterpret_cast<UnkCameraState_8000AA70 *>(&state)->~UnkCameraState_8000AA70();
    m_distance = field_0x80;
    m_height = field_0x84;
}

CameraModeFixLookChar::~CameraModeFixLookChar() {}

void CameraModeFixLookChar::Update(float dt, UnkCameraUpdateArg_800099B8 *arg) {
    boost::shared_ptr<CharInfo> chara = m_chara.lock();
    if (chara.get() != 0) {
        Vec3f target;
        Vec3f direction;

        fn_800091EC(&target, &(const Vec3f &)(chara->m_position +
                                              ((VectorRotateFn)fn_80410098)(
                                                  m_offset.Get(), &chara->m_rotation,
                                                  chara->field_0x30)));
        fn_8000A5D4(this, &m_eye, &target);
        boost::shared_ptr<CharInfo> watched = fn_800A8C54(fn_80074194(), field_0xA0).lock();
        if (watched.get() != 0) {
            Vec3f eye;

            fn_800091EC(&eye, &watched->m_position);
            eye.y += m_height;
            fn_800091D0(&direction,
                        &(const Vec3f &)((VectorBinaryFn)fn_8040CD74)(&eye, &target));
            float length = fn_8040CF50(&direction);
            fn_8040CE38(&direction, (length + m_distance) / length);
            fn_8000A5D4(this, &(const Vec3f &)((VectorBinaryFn)fn_80093204)(&direction, &target),
                        &m_target);
            if (m_process.get() != 0) {
                m_process->Process(this, &eye, watched.get());
            }
        }
    }
}

void CameraModeFixLookChar::OnSwitch(UnkCameraSwitchArg_8000996C *arg) {
    u32 flags = arg->m_flags;
    if (flags & 4) {
        m_process.reset();
    } else if (flags & 8) {
        m_process = fn_801C68E8();
    } else {
        m_process = fn_801C6EB0();
    }
}

void CameraModeFixLookChar::vf_0x14(UnkCameraArg_800098E4 *arg) {}

CameraMode *CameraModeFixLookChar::Create(UnkCameraOwner_8000E9BC *owner, int number) {
    return new CameraModeFixLookChar(owner, number);
}

int CameraModeFixLookChar::GetModeId() {
    return 14;
}

} // namespace PJSCamera
