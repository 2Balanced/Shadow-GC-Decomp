/* PJSCamera::StageCameraManager (class name and base from RTTI; Game/StageCameraManager.h) and its
 * state (class name unknown): the camera of a player on the stage. The state keeps the camera
 * areas of the stage's set data (Game/SetCameraData.h), the external requests other code makes
 * (ExternalCameraRequest, newest first) and a slot with the default, area and external camera
 * modes; it is in one of five states (none, default, area, external, and one the stage action
 * state 9 forces) and requests the matching mode on the player's camera unit whenever the state
 * changes. While the camera stick is used it requests a general mode of its own over them
 * (UnkCameraHandle_801CF664). File name from the class; other names are inferred. Definitions are
 * in reverse address order (deferred inlining emits functions last-defined first). */
#define MSL_LIST_ACCESSORS
#define MSL_LIST_VIBRATION
#define MSL_FUNCTIONAL_BINDERS
#define MSL_LIST_PUSH_FRONT
#include <iterator>
#include <functional>
#include <list>

#include "Game/StageCameraManager.h"
#include "Game/CameraManagerUnit.h"
#include "Game/CameraModeUser.h"
#include "Game/CameraState.h"
#include "Game/CharInfo.h"
#include "Game/ExternalCameraRequest.h"
#include "Game/MathTypes.h"
#include "Game/PJSCameraTargetInfo.h"
#include "Game/PtrVector.h"
#include "Game/SetCameraData.h"
#include "Game/StageAction.h"
#include "Game/StageCameraList.h"
#include "Game/StageCameraPostProcess.h"
#include "Game/Task.h"
#include "boost/scoped_ptr.hpp"
#include "boost/shared_ptr.hpp"
#include "boost/weak_ptr.hpp"
#include <dolphin/types.h>
#include "size_assert.h"

using PJSCamera::ExternalCameraRequest;
using PJSCamera::PJSCameraTargetInfo;
using PJSCamera::StageCameraManager;
using PJSCamera::StageCameraPostProcess;
using SonicteamUSA::System::Camera::CameraMode;
using SonicteamUSA::System::Camera::CameraRequest;

/* The camera manager (class name unknown; see Game/CameraManagerUnit.h). */
class UnkCameraManager_80009548;

/* The game state block in .bss (only the member used here; see Game/CityBombBig.cpp). */
struct UnkGameState_8057D6A0 {
    /* 0x000 */ u8 field_0x0[0x125];
    /* 0x125 */ u8 field_0x125; /* selects the flag bit of the areas in play (meaning unknown) */
    /* 0x126 */ u8 field_0x126[0x150 - 0x126];
};
SIZE_ASSERT(UnkGameState_8057D6A0, 0x150);

extern UnkGameState_8057D6A0 lbl_8057D6A0;

extern "C" {
/* The camera manager, the target a screen's camera follows, the placement of a screen's camera
 * unit (its eye first) and a call whose meaning is unknown (behaviour inferred). */
UnkCameraManager_80009548 *fn_80009548(void);
const PJSCameraTargetInfo *fn_801C0B98(UnkCameraManager_80009548 *manager, int index);
const Vec3f *fn_80010244(UnkCameraManager_80009548 *manager, int index);
void fn_8000D534(UnkCameraManager_80009548 *manager, int arg1);

/* A player's character (see Game/VibrationManagerUnit.cpp). */
boost::weak_ptr<CharInfo> fn_800A8C54(UnkCharaList_80074194 *list, int player);

/* Camera modes and requests (see Game/CameraManagerUnit.cpp; the mode is passed by value here):
 * a mode of a kind with arguments for a screen, a request of a mode with a priority, a blend and
 * flags, and releasing either; a request's blend changed (behaviour inferred). */
boost::shared_ptr<CameraMode> fn_8026F6A4(int kind, const UnkCameraModeArgs_8000EBB0 *args,
                                          int player);
void fn_8026F684(boost::shared_ptr<CameraMode> *mode);
boost::shared_ptr<CameraRequest> fn_8026F464(boost::shared_ptr<CameraMode> mode, int priority,
                                             int blendKind, const UnkCameraBlend_8000B2C0 *blend,
                                             u32 flags);
void fn_8026F444(boost::shared_ptr<CameraRequest> *request);
void fn_8000EC44(CameraRequest *request, int blendKind, const UnkCameraBlend_8000B2C0 *blend);

/* A 32-bit flag word: set up, clear all, set, clear and test bit (see Game/ArkCannon.cpp). */
u32 *fn_8000AAEC(u32 *flags, u32 value);
void fn_80014E10(u32 *flags);
void fn_80014E1C(u32 *flags, int bit);
void fn_8006127C(u32 *flags, int bit);
bool fn_80014E34(const u32 *flags, int bit);
/* A bit of a word array (see Game/PlayerPostManager.cpp). */
bool fn_8006CD78(const u32 *bits, int bit);

/* A camera mode's look vector (its placement comes first; see Game/CameraModeGeneral.cpp) and the
 * angle between two vectors. */
Vec3f fn_8000A3BC(const CameraMode *mode);
float fn_8040F8FC(const Vec3f *a, const Vec3f *b);
}

extern const float lbl_805FAD38; /* pi */
/* A zero field of view and window (meaning inferred). */
extern const float lbl_805F1708;
extern Vec2f lbl_805EDFA8;

/* A 32-bit flag word set up through its out-of-line constructor (fn_8000AAEC; see
 * Game/CameraModeGeneral.cpp). */
struct UnkFlags_8000AAEC {
    UnkFlags_8000AAEC(u32 value) {
        fn_8000AAEC(&m_bits, value);
    }
    void Clear() {
        fn_80014E10(&m_bits);
    }
    void Set(int bit) {
        fn_80014E1C(&m_bits, bit);
    }
    void Reset(int bit) {
        fn_8006127C(&m_bits, bit);
    }
    bool Test(int bit) {
        return fn_80014E34(&m_bits, bit);
    }

    u32 m_bits;
};

/* Whether v is (nearly) the zero vector (as in Game/CameraModeGeneral.cpp). */
inline bool IsNearZero(const Vec3f &v) {
    return v.x * v.x + v.y * v.y + v.z * v.z < 0.0001f;
}

inline bool IsNearZero(const Vec2f &v) {
    return v.x * v.x + v.y * v.y < 0.0001f;
}

/* The camera modes a stage camera chooses from (class name unknown; 0x3C bytes, constructor
 * 0x801C9200): the default mode, the mode of the area the player is in and an external mode, each
 * with its blend and request flags. The state says which one is in use (0 default, 1 area, 2
 * external). */
class UnkStageCameraSlot_801C9200 {
  public:
    UnkStageCameraSlot_801C9200();
    ~UnkStageCameraSlot_801C9200();
    void SetArea(const SetCameraData *data, int player);
    void SetExternal(boost::shared_ptr<CameraMode> mode, int blendKind,
                     const UnkCameraBlend_8000B2C0 *blend, u32 flags, int option);
    void Reset();
    void ReleaseArea();
    bool ReleaseExternal(boost::shared_ptr<CameraMode> mode);
    void SetDefault(const boost::shared_ptr<CameraMode> &mode, int blendKind,
                    const UnkCameraBlend_8000B2C0 *blend, u32 flags);
    boost::shared_ptr<CameraMode> GetMode() const;
    int GetBlendKind() const;
    const UnkCameraBlend_8000B2C0 *GetBlend() const;
    u32 GetFlags() const;

    const SetCameraData *GetAreaData() const {
        return m_areaData;
    }
    void ClearExternal() {
        fn_8026F684(&m_externalMode);
        m_externalBlendKind = -1;
        if (m_externalBlend != 0) {
            delete m_externalBlend;
            m_externalBlend = 0;
        }
    }
    void ClearArea() {
        fn_8026F684(&m_areaMode);
        m_areaData = 0;
        if (m_state == 1) {
            m_state = 0;
        }
    }

    /* 0x00 */ s32 m_state;
    /* 0x04 */ boost::shared_ptr<CameraMode> m_defaultMode;
    /* 0x0C */ s32 m_defaultBlendKind;
    /* 0x10 */ const UnkCameraBlend_8000B2C0 *m_defaultBlend;
    /* 0x14 */ u32 m_defaultFlags;
    /* 0x18 */ boost::shared_ptr<CameraMode> m_areaMode;
    /* 0x20 */ const SetCameraData *m_areaData;
    /* 0x24 */ boost::shared_ptr<CameraMode> m_externalMode;
    /* 0x2C */ s32 m_externalBlendKind;
    /* 0x30 */ UnkCameraBlend_8000B2C0 *m_externalBlend; /* a copy the slot owns */
    /* 0x34 */ u32 m_externalFlags;
    /* 0x38 */ s32 m_externalOption; /* bit 0: the camera stick leaves the mode alone */
};
SIZE_ASSERT(UnkStageCameraSlot_801C9200, 0x3C);

/* The state of a stage camera (class name unknown; Game/StageCameraManager.h). The states are 0
 * none, 1 the slot's mode (default or area of order kind), 2 an area of priority kind, 3 the
 * newest external request and 4 the stage action state 9 forces. The flags at 0x70 are set for one
 * frame: bit 0 a state change, 1 to 3 the camera stick and the target's state bit 60, 4 the forced
 * state, 5 the stick let go; those at 0x74 keep the stick's state. */
class UnkStageCamera_801CBB30 {
  public:
    UnkStageCamera_801CBB30(int player);
    ~UnkStageCamera_801CBB30();
    void Reset();
    void AddRequest(boost::shared_ptr<ExternalCameraRequest> request);
    void RemoveRequest(boost::shared_ptr<ExternalCameraRequest> request);
    void CheckInput();
    void UpdateNone(float dt, boost::shared_ptr<CharInfo> chara);
    void UpdateExternal(float dt, boost::shared_ptr<CharInfo> chara);
    void UpdateDefault(float dt, boost::shared_ptr<CharInfo> chara);
    void UpdateArea(float dt, boost::shared_ptr<CharInfo> chara);
    void SetExternalMode(boost::shared_ptr<CameraMode> mode, int blendKind,
                         const UnkCameraBlend_8000B2C0 *blend, u32 flags, int option);
    void ReleaseExternalMode(boost::shared_ptr<CameraMode> mode);
    void UpdateHandle();
    void UpdateHandleTime(float dt, boost::shared_ptr<CharInfo> chara);
    void ResetTime();
    bool SelectArea(boost::shared_ptr<CharInfo> chara, int priority, bool line,
                    ExternalCameraRequest *request);
    void ChangeState(int state, SetCameraArea *area, ExternalCameraRequest *request);
    SetCameraArea *FindPriorityArea(boost::shared_ptr<CharInfo> chara, int priority, bool line);
    SetCameraArea *FindOrderArea(boost::shared_ptr<CharInfo> chara, int order, bool line);
    void SetupAreas();
    void ClearAreas();

    /* 0x00 */ s32 m_player;
    /* 0x04 */ StageCameraManager *m_manager;
    /* 0x08 */ const PJSCameraTargetInfo *m_target;
    /* 0x0C */ StageCameraPostProcess *m_postProcess;
    /* 0x10 */ UnkCameraModeHolder_801CF3C0 m_general;
    /* 0x18 */ UnkStageCameraSlot_801C9200 m_slot;
    /* 0x54 */ UnkCameraHandle_801CF664 m_handle;
    /* 0x68 */ float m_time; /* since the stick's mode was requested */
    /* 0x6C */ s32 m_state;
    /* 0x70 */ UnkFlags_8000AAEC m_flags;
    /* 0x74 */ UnkFlags_8000AAEC m_input;
    /* 0x78 */ SetCameraArea *m_area;
    /* 0x7C */ boost::shared_ptr<CameraRequest> m_request;
    /* 0x84 */ std::list<boost::shared_ptr<ExternalCameraRequest> > m_requests;
    /* 0x90 */ std::vector<SetCameraArea *> m_priorityAreas; /* by priority, highest first */
    /* 0x9C */ std::vector<SetCameraArea *> m_orderAreas;    /* by order */
};
SIZE_ASSERT(UnkStageCamera_801CBB30, 0xA8);

/* Deletes the state (name inferred; see checked_delete below). */
inline void DeleteStageCamera(UnkStageCamera_801CBB30 *camera) {
    delete camera;
}

namespace boost {
/* boost/shared_ptr.hpp only declares shared_ptr::reset, which the game has out of line; this unit
 * emits the copy for CameraMode (boost 1.32's definition). */
template <> inline void shared_ptr<CameraMode>::reset() {
    this_type().swap(*this);
}

/* Retail keeps boost::checked_delete<UnkStageCamera_801CBB30> out of line and the destructor at its
 * own place; this specialization gives that under GC/2.7 (see docs/research-notes.md and
 * Game/EnergyCore.cpp). */
template <> inline void checked_delete<UnkStageCamera_801CBB30>(UnkStageCamera_801CBB30 *x) {
    DeleteStageCamera(x);
}
} // namespace boost

UnkStageCamera_801CBB30::UnkStageCamera_801CBB30(int player)
    : m_player(player), m_manager(0), m_target(0), m_postProcess(0), m_general(player),
      m_handle(player), m_time(0.0f), m_state(0), m_flags(0), m_input(0), m_area(0) {}

UnkStageCamera_801CBB30::~UnkStageCamera_801CBB30() {
    ClearAreas();
    fn_801CE760(fn_80099954(), m_manager);
}

void UnkStageCamera_801CBB30::Reset() {
    if (m_state != 3) {
        fn_8000D534(fn_80009548(), 0);
        ChangeState(0, 0, 0);
    }
}

void UnkStageCamera_801CBB30::AddRequest(boost::shared_ptr<ExternalCameraRequest> request) {
    m_requests.push_front(request);
    ChangeState(3, 0, 0);
}

void UnkStageCamera_801CBB30::RemoveRequest(boost::shared_ptr<ExternalCameraRequest> request) {
    m_requests.remove(request);
    if (m_requests.empty()) {
        boost::shared_ptr<CharInfo> chara = fn_800A8C54(fn_80074194(), m_player).lock();
        if (chara.get() != 0) {
            if (!SelectArea(chara, -1, false, request.get())) {
                ChangeState(1, 0, request.get());
            }
        } else {
            ChangeState(0, 0, request.get());
        }
    } else {
        ChangeState(3, 0, request.get());
    }
}

void UnkStageCamera_801CBB30::CheckInput() {
    if (fn_80074470()->m_prevState == 9) {
        if (m_state != 4) {
            m_flags.Set(4);
            ChangeState(4, 0, 0);
        }
        return;
    }
    if (fn_8006CD78(m_target->field_0x40.m_words, 60)) {
        m_flags.Set(1);
        m_flags.Set(3);
        if (!m_input.Test(0)) {
            if (m_handle.m_active) {
                m_input.Set(1);
            }
            m_input.Set(0);
        }
    } else if (!IsNearZero(m_target->field_0x4C)) {
        m_flags.Set(1);
        m_flags.Set(2);
        m_input.Reset(0);
    } else if (m_input.Test(0)) {
        if (!m_input.Test(1) && m_state != 4) {
            m_flags.Set(5);
        }
        m_input.Reset(0);
        m_input.Reset(1);
    }
}

void UnkStageCamera_801CBB30::UpdateNone(float dt, boost::shared_ptr<CharInfo> chara) {
    if (!SelectArea(chara, -1, false, 0)) {
        ChangeState(1, 0, 0);
    }
}

void UnkStageCamera_801CBB30::UpdateExternal(float dt, boost::shared_ptr<CharInfo> chara) {
    UpdateHandle();
    UpdateHandleTime(dt, chara);
}

void UnkStageCamera_801CBB30::UpdateDefault(float dt, boost::shared_ptr<CharInfo> chara) {
    if (m_slot.m_state == 1) {
        bool inside = false;
        if (m_area != 0) {
            inside = m_area->Check(chara, m_target, false);
        }
        if (inside) {
            SelectArea(chara, m_area->m_data->m_priority + 1, true, 0);
        } else if (!SelectArea(chara, -1, true, 0)) {
            if (FindOrderArea(chara, 1, true)) {
                m_slot.ReleaseArea();
                ChangeState(1, 0, 0);
            }
        }
    } else {
        SelectArea(chara, -1, true, 0);
    }
    UpdateHandle();
    UpdateHandleTime(dt, chara);
}

void UnkStageCamera_801CBB30::UpdateArea(float dt, boost::shared_ptr<CharInfo> chara) {
    bool inside;
    if (m_area->m_data->m_shape.m_kind != 4) {
        inside = m_area->Check(chara, m_target, false);
    } else {
        inside = true;
    }
    if (inside) {
        SelectArea(chara, m_area->m_data->m_priority + 1, true, 0);
    } else if (!SelectArea(chara, -1, true, 0)) {
        ChangeState(1, 0, 0);
    }
    UpdateHandle();
    UpdateHandleTime(dt, chara);
}

void UnkStageCamera_801CBB30::SetExternalMode(boost::shared_ptr<CameraMode> mode, int blendKind,
                                              const UnkCameraBlend_8000B2C0 *blend, u32 flags,
                                              int option) {
    m_slot.SetExternal(mode, blendKind, blend, flags, option);
    if (m_state == 1) {
        ChangeState(1, 0, 0);
    }
}

void UnkStageCamera_801CBB30::ReleaseExternalMode(boost::shared_ptr<CameraMode> mode) {
    if (m_slot.ReleaseExternal(mode) && m_state == 1) {
        ChangeState(1, 0, 0);
    }
}

void UnkStageCamera_801CBB30::UpdateHandle() {
    if (!m_flags.Test(1)) {
        return;
    }
    if (!m_handle.m_active) {
        switch (m_state) {
        case 2:
            if (m_area != 0) {
                if (m_area->m_data->m_flags.Test(3)) {
                    return;
                }
                if (m_area->m_data->m_modeKind == m_handle.m_mode->GetModeId()) {
                    m_handle.m_active = true;
                } else {
                    m_handle.Request();
                    m_flags.Set(0);
                }
            }
            break;
        case 1:
            if (m_slot.m_state == 1) {
                const SetCameraData *data = m_slot.GetAreaData();
                if (data->m_flags.Test(3)) {
                    return;
                }
                if (data->m_modeKind == m_handle.m_mode->GetModeId()) {
                    m_handle.m_active = true;
                } else {
                    m_handle.Request();
                    m_flags.Set(0);
                }
            }
            if (m_slot.m_state == 2 && (m_slot.m_externalOption & 1)) {
                return;
            }
            {
                boost::shared_ptr<CameraMode> mode = m_slot.GetMode();
                if (mode->GetModeId() == m_handle.m_mode->GetModeId()) {
                    m_handle.m_active = true;
                } else {
                    m_handle.Request();
                    m_flags.Set(0);
                }
            }
            break;
        case 3: {
            boost::shared_ptr<ExternalCameraRequest> top = m_requests.front();
            if (top.get() != 0 && (top->m_flags & 1)) {
                if (m_request->GetMode()->GetModeId() == m_handle.m_mode->GetModeId()) {
                    m_handle.m_active = true;
                } else {
                    m_handle.Request();
                    m_flags.Set(0);
                }
            }
            break;
        }
        }
    }
    if (m_handle.m_active) {
        u32 flags = 0;
        if (m_flags.Test(2)) {
            flags |= 1;
        }
        if (m_flags.Test(3)) {
            flags |= 2;
        }
        if (m_handle.IsRequested()) {
            m_handle.m_mode->vf_0x18(flags);
        } else {
            m_request->GetMode()->vf_0x18(flags);
        }
        m_time = 0.0f;
    }
}

void UnkStageCamera_801CBB30::UpdateHandleTime(float dt, boost::shared_ptr<CharInfo> chara) {
    if (m_handle.m_active && m_time <= 2.0f) {
        m_time += dt;
    }
    if (m_handle.IsRequested() && !m_flags.Test(0)) {
        if (m_time > 2.0f && !IsNearZero(chara->field_0x18)) {
            Vec3f handleLook;
            fn_800091EC(&handleLook, &(const Vec3f &)fn_8000A3BC(m_handle.m_mode.get()));
            Vec3f requestLook;
            fn_800091EC(&requestLook, &(const Vec3f &)fn_8000A3BC(m_request->GetMode().get()));
            Vec3f a;
            Vec3f b;
            fn_800091D0(&a, &handleLook);
            fn_800091D0(&b, &requestLook);
            a.x = 0.0f;
            b.x = 0.0f;
            float angleX = fn_8040F8FC(&a, &b);
            fn_800091D0(&a, &handleLook);
            fn_800091D0(&b, &requestLook);
            a.y = 0.0f;
            b.y = 0.0f;
            float angleY = fn_8040F8FC(&a, &b);
            if (angleX < 20.0f * (lbl_805FAD38 / 180.0f) &&
                angleY < 20.0f * (lbl_805FAD38 / 180.0f)) {
                UnkCameraBlend_8000B2C0 blend(2.0f);
                fn_8000EC44(m_request.get(), 2, &blend);
                m_handle.Release();
                m_time = 0.0f;
                m_flags.Set(0);
                return;
            }
        }
        if (m_flags.Test(5)) {
            m_handle.Release();
            m_time = 0.0f;
            m_flags.Set(0);
        }
    }
}

/* Never called (the linker drops it). The game's constant pool has 0.0f before UpdateHandleTime's
 * constants, so a function using it was compiled between UpdateHandleTime and ChangeState; name
 * and body are inferred. */
void UnkStageCamera_801CBB30::ResetTime() {
    m_time = 0.0f;
}

bool UnkStageCamera_801CBB30::SelectArea(boost::shared_ptr<CharInfo> chara, int priority,
                                         bool line, ExternalCameraRequest *request) {
    SetCameraArea *area = FindPriorityArea(chara, priority, line);
    if (area != 0) {
        if (area != m_area) {
            if (area->m_data->m_priority == -1) {
                ChangeState(1, area, request);
            } else {
                ChangeState(2, area, request);
            }
        }
        return true;
    }
    return false;
}

void UnkStageCamera_801CBB30::ChangeState(int state, SetCameraArea *area,
                                          ExternalCameraRequest *request) {
    fn_8026F444(&m_request);
    switch (m_state) {
    case 3:
        if (request != 0 && (request->m_flags & 4)) {
            m_postProcess->m_enabled = true;
        }
        break;
    case 0:
        m_postProcess->m_enabled = true;
        break;
    }
    switch (state) {
    case 4:
        if (m_state == 3) {
            boost::shared_ptr<ExternalCameraRequest> top = m_requests.front();
            if (top->m_flags & 8) {
                boost::shared_ptr<CameraMode> mode =
                    fn_8026F6A4(lbl_805F5D18, &lbl_805812E4, m_player);
                m_request = fn_8026F464(mode, 0x15, 0, 0, 1);
                break;
            }
        }
        {
            UnkCameraModeArgs_8000EBB0 args(fn_80010244(fn_80009548(), m_player),
                                            &m_target->m_position, lbl_805F1708, &lbl_805EDFA8, 0);
            UnkCameraBlend_8000B2C0 blend(fn_8006CD78(m_target->field_0x40.m_words, 2) ? 1.0f
                                                                                       : 0.2f);
            boost::shared_ptr<CameraMode> mode = fn_8026F6A4(7, &args, m_player);
            m_request = fn_8026F464(mode, 0x15, 2, &blend, 0);
        }
        break;
    case 3: {
        boost::shared_ptr<ExternalCameraRequest> top = m_requests.front();
        if (top.get() != 0) {
            if (top->m_flags & 4) {
                m_postProcess->Reset();
            }
            m_request = fn_8026F464(top->GetMode(), 0x13, top->m_blendKindIn, &top->m_blendIn, 0);
        }
        break;
    }
    case 0:
        m_slot.Reset();
        m_postProcess->Reset();
        break;
    case 1: {
        if (area != 0) {
            m_slot.SetArea(area->m_data, m_player);
        }
        int blendKind;
        const UnkCameraBlend_8000B2C0 *blend;
        boost::shared_ptr<CameraMode> mode = m_slot.GetMode();
        u32 flags = m_slot.GetFlags() | 2;
        if (m_state == 0) {
            m_request = fn_8026F464(mode, 1, 0, 0, flags | 1);
            break;
        }
        if (m_state == 3 && request != 0 && request->m_blendKindOut != -1) {
            flags |= request->m_requestFlags;
            m_request =
                fn_8026F464(mode, 1, request->m_blendKindOut, &request->m_blendOut, flags);
            break;
        }
        blendKind = m_slot.GetBlendKind();
        blend = m_slot.GetBlend();
        m_request = fn_8026F464(mode, 1, blendKind, blend, flags);
        break;
    }
    case 2: {
        boost::shared_ptr<CameraMode> mode =
            fn_8026F6A4(area->m_data->m_modeKind, &area->m_data->m_args, m_player);
        u32 flags = 0;
        SetCameraFlags &areaFlags = area->m_data->m_flags;
        if (areaFlags.Test(5)) {
            flags |= 1;
        }
        if (areaFlags.Test(7)) {
            flags |= 4;
        }
        if (areaFlags.Test(8)) {
            flags |= 8;
        }
        flags |= 2;
        if (m_state == 0) {
            m_request = fn_8026F464(mode, area->m_data->m_priority + 2, 0, 0, flags | 1);
            break;
        }
        if (m_state == 3 && request != 0 && request->m_blendKindOut != -1) {
            flags |= request->m_requestFlags;
            m_request = fn_8026F464(mode, area->m_data->m_priority + 2, request->m_blendKindOut,
                                    &request->m_blendOut, flags);
            break;
        }
        m_request = fn_8026F464(mode, area->m_data->m_priority + 2, area->m_data->m_blendKind,
                                &area->m_data->m_blend, flags);
        break;
    }
    }
    if (m_handle.m_active) {
        switch (state) {
        case 0:
            m_handle.Release();
            break;
        case 3: {
            boost::shared_ptr<ExternalCameraRequest> top = m_requests.front();
            if (top.get() != 0 && m_request.get() != 0) {
                if (top->m_flags & 2) {
                    if (m_request->GetMode()->GetModeId() == m_handle.m_mode->GetModeId()) {
                        if (m_handle.IsRequested()) {
                            m_handle.Release();
                            m_handle.m_active = true;
                        }
                    } else if (!m_handle.IsRequested()) {
                        m_handle.Request();
                    }
                } else {
                    m_handle.Release();
                }
            }
            break;
        }
        case 2:
            if (area != 0) {
                if (area->m_data->m_flags.Test(3) || area->m_data->m_flags.Test(4)) {
                    m_handle.Release();
                } else if (area->m_data->m_modeKind == m_handle.m_mode->GetModeId()) {
                    if (m_handle.IsRequested()) {
                        m_handle.Release();
                        m_handle.m_active = true;
                    }
                } else if (!m_handle.IsRequested()) {
                    m_handle.Request();
                }
            }
            break;
        case 1:
            if (m_slot.m_state == 1) {
                const SetCameraData *data = m_slot.m_areaData;
                if (data->m_flags.Test(3) || data->m_flags.Test(4)) {
                    m_handle.Release();
                } else if (data->m_modeKind == m_handle.m_mode->GetModeId()) {
                    if (m_handle.IsRequested()) {
                        m_handle.Release();
                        m_handle.m_active = true;
                    }
                } else if (!m_handle.IsRequested()) {
                    m_handle.Request();
                }
            } else if (m_slot.m_state == 2 && (m_slot.m_externalOption & 1)) {
                m_handle.Release();
            } else {
                boost::shared_ptr<CameraMode> mode = m_slot.GetMode();
                if (mode->GetModeId() == m_handle.m_mode->GetModeId()) {
                    if (m_handle.IsRequested()) {
                        m_handle.Release();
                        m_handle.m_active = true;
                    }
                } else if (!m_handle.IsRequested()) {
                    m_handle.Request();
                }
            }
            break;
        }
    }
    m_area = area;
    m_state = state;
    m_flags.Set(0);
}

SetCameraArea *UnkStageCamera_801CBB30::FindPriorityArea(boost::shared_ptr<CharInfo> chara,
                                                         int priority, bool line) {
    std::vector<SetCameraArea *>::iterator it = m_priorityAreas.begin();
    SetCameraArea *found = 0;
    for (; it != m_priorityAreas.end(); ++it) {
        SetCameraArea *area = *it;
        if (area->m_data->m_priority < priority) {
            break;
        }
        if (area->Check(chara, m_target, line)) {
            found = area;
            break;
        }
    }
    return found;
}

SetCameraArea *UnkStageCamera_801CBB30::FindOrderArea(boost::shared_ptr<CharInfo> chara,
                                                      int order, bool line) {
    std::vector<SetCameraArea *>::iterator it = m_orderAreas.begin();
    SetCameraArea *found = 0;
    for (; it != m_orderAreas.end(); ++it) {
        SetCameraArea *area = *it;
        if (area->m_data->m_order < order) {
            continue;
        }
        if (area->m_data->m_order > order) {
            break;
        }
        if (area->Check(chara, m_target, line)) {
            found = area;
            break;
        }
    }
    return found;
}

void UnkStageCamera_801CBB30::SetupAreas() {
    m_requests.clear();
    SetCameraDataManager *data = fn_80099954()->GetSetCameraData();
    int num = data->GetNum();
    m_priorityAreas.clear();
    m_orderAreas.clear();
    if (num > 0) {
        int bit = lbl_8057D6A0.field_0x125 ? 1 : 0;
        SetCameraData **it;
        SetCameraData **begin = data->Begin();
        SetCameraData **end = data->End();
        int priorityCount = 0;
        int orderCount = 0;
        for (it = begin; it != end; ++it) {
            if ((*it)->m_flags.Test(bit)) {
                if ((*it)->m_modeKind == 0) {
                    orderCount++;
                } else {
                    priorityCount++;
                }
            }
        }
        m_priorityAreas.reserve(priorityCount);
        m_orderAreas.reserve(orderCount);
        for (it = begin; it != end; ++it) {
            if ((*it)->m_flags.Test(bit)) {
                SetCameraArea *area = new SetCameraArea(*it);
                if (area->m_data->m_modeKind == 0) {
                    m_orderAreas.push_back(area);
                } else {
                    m_priorityAreas.push_back(area);
                }
            }
        }
    }
}

void UnkStageCamera_801CBB30::ClearAreas() {
    fn_8026F444(&m_request);
    m_area = 0;
    m_state = 0;
    std::vector<SetCameraArea *>::iterator it;
    for (it = m_priorityAreas.begin(); it != m_priorityAreas.end(); ++it) {
        delete *it;
    }
    m_priorityAreas.clear();
    for (it = m_orderAreas.begin(); it != m_orderAreas.end(); ++it) {
        delete *it;
    }
    m_orderAreas.clear();
    m_requests.clear();
}

const char *StageCameraManager::s_taskName = "StageCameraManager";

StageCameraManager::StageCameraManager(Task *parent, int player)
    : Task(parent), m_impl(new UnkStageCamera_801CBB30(player)), field_0x2C(0) {
    m_name = s_taskName;
    m_impl->m_manager = this;
    m_impl->m_postProcess = new StageCameraPostProcess(fn_800D51EC()->m_postTask, this);
    m_impl->m_postProcess->Reset();
    m_impl->m_target = fn_801C0B98(fn_80009548(), player);
    m_impl->SetupAreas();
    m_impl->m_slot.SetDefault(m_impl->m_general.GetMode(), lbl_805E87D8, &lbl_8058135C, 0);
}

StageCameraManager::~StageCameraManager() {
    if (field_0x2C != 0) {
        delete field_0x2C;
        field_0x2C = 0;
    }
}

void StageCameraManager::Tick(float dt) {
    boost::shared_ptr<CharInfo> chara = fn_800A8C54(fn_80074194(), m_impl->m_player).lock();
    if (chara.get() == 0) {
        return;
    }
    m_impl->CheckInput();
    switch (m_impl->m_state) {
    case 0:
        m_impl->UpdateNone(dt, chara);
        break;
    case 1:
        m_impl->UpdateDefault(dt, chara);
        break;
    case 2:
        m_impl->UpdateArea(dt, chara);
        break;
    case 3:
        m_impl->UpdateExternal(dt, chara);
        break;
    }
    m_impl->m_flags.Clear();
}

void StageCameraManager::Reset() {
    m_impl->Reset();
}

int StageCameraManager::GetPlayer() {
    return m_impl->m_player;
}

void StageCameraManager::AddRequest(boost::shared_ptr<ExternalCameraRequest> request) {
    m_impl->AddRequest(request);
}

void StageCameraManager::RemoveRequest(boost::shared_ptr<ExternalCameraRequest> request) {
    m_impl->RemoveRequest(request);
}

void StageCameraManager::SetExternalMode(boost::shared_ptr<CameraMode> mode, int blendKind,
                                         const UnkCameraBlend_8000B2C0 *blend, u32 flags,
                                         int arg) {
    m_impl->SetExternalMode(mode, blendKind, blend, flags, arg);
}

void StageCameraManager::ReleaseExternalMode(boost::shared_ptr<CameraMode> mode) {
    m_impl->ReleaseExternalMode(mode);
}

UnkStageCameraSlot_801C9200::UnkStageCameraSlot_801C9200()
    : m_state(0), m_defaultBlendKind(-1), m_defaultBlend(0), m_defaultFlags(0), m_areaData(0),
      m_externalBlendKind(-1), m_externalBlend(0), m_externalFlags(0), m_externalOption(0) {
    m_defaultMode.reset();
    m_areaMode.reset();
    m_externalMode.reset();
}

UnkStageCameraSlot_801C9200::~UnkStageCameraSlot_801C9200() {
    fn_8026F684(&m_externalMode);
    fn_8026F684(&m_areaMode);
    fn_8026F684(&m_defaultMode);
}

void UnkStageCameraSlot_801C9200::SetArea(const SetCameraData *data, int player) {
    m_areaMode = fn_8026F6A4(data->m_modeKind, &data->m_args, player);
    m_areaData = data;
    if (m_state == 0) {
        m_state = 1;
    }
}

void UnkStageCameraSlot_801C9200::SetExternal(boost::shared_ptr<CameraMode> mode, int blendKind,
                                              const UnkCameraBlend_8000B2C0 *blend, u32 flags,
                                              int option) {
    if (m_state == 2) {
        ReleaseExternal(m_externalMode);
    }
    m_externalMode = mode;
    m_externalBlendKind = blendKind;
    m_externalBlend = new UnkCameraBlend_8000B2C0(*blend);
    m_externalFlags = flags;
    m_externalOption = option;
    m_state = 2;
}

void UnkStageCameraSlot_801C9200::Reset() {
    ClearExternal();
    ClearArea();
    m_state = 0;
}

void UnkStageCameraSlot_801C9200::ReleaseArea() {
    ClearArea();
}

bool UnkStageCameraSlot_801C9200::ReleaseExternal(boost::shared_ptr<CameraMode> mode) {
    if (m_externalMode.get() != 0 && mode == m_externalMode) {
        ClearExternal();
        if (m_areaMode.get() != 0) {
            m_state = 1;
        } else {
            m_state = 0;
        }
        return true;
    }
    return false;
}

void UnkStageCameraSlot_801C9200::SetDefault(const boost::shared_ptr<CameraMode> &mode,
                                             int blendKind, const UnkCameraBlend_8000B2C0 *blend,
                                             u32 flags) {
    m_defaultMode = mode;
    m_defaultBlendKind = blendKind;
    m_defaultBlend = blend;
    m_defaultFlags = flags;
}

boost::shared_ptr<CameraMode> UnkStageCameraSlot_801C9200::GetMode() const {
    if (m_state == 2) {
        return m_externalMode;
    }
    if (m_state == 1) {
        return m_areaMode;
    }
    return m_defaultMode;
}

int UnkStageCameraSlot_801C9200::GetBlendKind() const {
    if (m_state == 2) {
        return m_externalBlendKind;
    }
    if (m_state == 1) {
        return m_areaData->m_blendKind;
    }
    return m_defaultBlendKind;
}

const UnkCameraBlend_8000B2C0 *UnkStageCameraSlot_801C9200::GetBlend() const {
    if (m_state == 2) {
        return m_externalBlend;
    }
    if (m_state == 1) {
        return &m_areaData->m_blend;
    }
    return m_defaultBlend;
}

u32 UnkStageCameraSlot_801C9200::GetFlags() const {
    if (m_state == 2) {
        return m_externalFlags;
    }
    if (m_state == 1) {
        const SetCameraFlags &areaFlags = m_areaData->m_flags;
        u32 flags = 0;
        if (areaFlags.Test(5)) {
            flags |= 1;
        }
        if (areaFlags.Test(7)) {
            flags |= 4;
        }
        if (areaFlags.Test(8)) {
            flags |= 8;
        }
        return flags;
    }
    return m_defaultFlags;
}

ExternalCameraRequest::ExternalCameraRequest(boost::shared_ptr<CameraMode> mode, u32 flags,
                                             int blendKindIn,
                                             const UnkCameraBlend_8000B2C0 *blendIn,
                                             int blendKindOut,
                                             const UnkCameraBlend_8000B2C0 *blendOut,
                                             u32 requestFlags)
    : m_mode(mode), m_flags(flags), m_blendKindIn(blendKindIn), m_blendKindOut(blendKindOut),
      m_requestFlags(requestFlags) {
    if (blendIn != 0) {
        m_blendIn = *blendIn;
    }
    if (blendOut != 0) {
        m_blendOut = *blendOut;
    }
}

ExternalCameraRequest::~ExternalCameraRequest() {
    fn_8026F684(&m_mode);
}
