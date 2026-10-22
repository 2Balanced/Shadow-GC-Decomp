#ifndef GAME_CAMERAMODEFIXLOOKCHAR_H
#define GAME_CAMERAMODEFIXLOOKCHAR_H

#include "Game/CharInfo.h"
#include "Game/MathTypes.h"
#include "Game/PJSCameraMode.h"
#include "boost/shared_ptr.hpp"
#include "boost/weak_ptr.hpp"
#include "size_assert.h"

/* A vector member copy-constructed through the out-of-line copy constructor (fn_800091EC). It
 * does not derive from Vec3f: a constructor that calls Vec3f's inline one is not inlined. */
struct UnkCopiedVector {
    UnkCopiedVector(const Vec3f &from) {
        fn_800091EC((Vec3f *)this, &from);
    }

    /* Assignment through the out-of-line vector assignment (fn_800091D0). */
    void operator=(const Vec3f &from) {
        fn_800091D0((Vec3f *)this, &from);
    }

    const Vec3f *Get() const {
        return (const Vec3f *)this;
    }

    float x;
    float y;
    float z;
};

namespace PJSCamera {

/* CameraModeFixLookChar (class name and bases from RTTI; Game/CameraModeFixLookChar.cpp): a camera
 * mode that watches one character from beside another. BossCtrlUnit_FixLookCharCamera gives it the
 * character to look at and the offset; their setters are emitted out of line where they are used
 * (names inferred). */
class CameraModeFixLookChar : public PJSCameraMode {
  public:
    CameraModeFixLookChar(UnkCameraOwner_8000E9BC *owner, int number);
    virtual ~CameraModeFixLookChar();
    virtual int GetModeId();
    virtual void Update(float dt, UnkCameraUpdateArg_800099B8 *arg);
    virtual void OnSwitch(UnkCameraSwitchArg_8000996C *arg);
    virtual void vf_0x14(UnkCameraArg_800098E4 *arg);

    /* Creates the mode for the camera factory (name inferred). */
    static SonicteamUSA::System::Camera::CameraMode *Create(UnkCameraOwner_8000E9BC *owner,
                                                            int number);

    void SetChara(const boost::weak_ptr<CharInfo> &chara) {
        m_chara = chara;
    }

    void SetOffset(const Vec3f &offset) {
        m_offset = offset;
    }

    /* 0xAC */ float m_distance;
    /* 0xB0 */ float m_height;
    /* 0xB4 */ boost::weak_ptr<CharInfo> m_chara; /* the character looked at */
    /* 0xBC */ UnkCopiedVector m_offset;
    /* 0xC8 */ boost::shared_ptr<Collision::ProcessBase> m_process;
};
SIZE_ASSERT(CameraModeFixLookChar, 0xD0);

} // namespace PJSCamera

#endif
