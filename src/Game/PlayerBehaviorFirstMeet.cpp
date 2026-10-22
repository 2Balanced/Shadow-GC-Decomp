/* Player::Behavior::FirstMeet (class name from RTTI): behavior 0x24 of an NPC, the scene in which
 * the player first meets it. When the event state allows, it starts a user camera mode that
 * follows a camera path of the NPC's model resource (the path is chosen by the NPC's character),
 * placed at the NPC; when the path ends, the NPC switches back to behavior 0. File name inferred
 * from the class; names marked "(name inferred)" are not original. Definitions are in reverse
 * address order (deferred inlining emits functions last-defined first). */

#include <boost/bind.hpp>
#include <boost/function.hpp>

#include "Game/CameraModeUser.h"
#include "Game/ExternalCameraRequest.h"
#include "Game/MathTypes.h"
#include "Game/Npc.h"
#include "Game/NpcPartModel.h"
#include "Game/Player.h"
#include "Game/PlayerBehavior.h"
#include "Game/PlayerWeaponBase.h"
#include "boost/shared_ptr.hpp"
#include "boost/weak_ptr.hpp"
#include <dolphin/types.h>
#include "size_assert.h"

using SonicteamUSA::System::Camera::CameraMode;
using SonicteamUSA::System::Camera::CameraModeUser;

/* The event state fn_80074470 returns (struct name unknown; only the members read here). */
struct UnkObject_80074470 {
    /* 0x0 */ u8 field_0x0[0x8];
    /* 0x8 */ s32 m_mode;
    /* 0xC */ s32 field_0xC;
};

extern "C" {
/* The event state, and setting its mode (behaviour inferred). */
UnkObject_80074470 *fn_80074470(void);
void fn_80175EEC(UnkObject_80074470 *object, int mode, int arg2);
/* Starts and ends the event of an NPC character (behaviour inferred). */
bool fn_80204240(s32 kind);
bool fn_80204214(void);
/* Player updates run before the behavior's own (behaviour unknown). */
void fn_80079720(Player::PlayerBase *player);
void fn_8009728C(boost::shared_ptr<Player::PlayerBase> *player, int arg1);
/* The NPC's height (behaviour inferred). */
float fn_80073814(Player::Npc::Npc *npc);
/* Switches a player's behavior (behaviour inferred). */
void fn_800A7F14(Player::Behavior::UnkBehaviorExecuter_800A7F14 *executer,
                 const Player::Behavior::InitializeArgs *args);

/* The resource loader and a camera path of a resource (behaviour inferred). */
void *fn_800122D4(void);
UnkCameraData_80012510 *fn_80012510(void *loader, void *resource, int index);
/* Moves a camera path's time on; whether it ended (behaviour inferred). */
int fn_80011B7C(UnkCameraData_80012510 *data, float *time, const float *step);
/* Places a camera mode on a camera path (behaviour inferred). */
void fn_8000FBFC(const boost::shared_ptr<CameraMode> &mode, UnkCameraData_80012510 *data,
                 float time, const Mtx34f *matrix);

/* Matrix routines (see MathTypes.h): multiply, and apply a translation by x, y, z. */
void fn_8040BAC4(Mtx34f *matrix, const Mtx34f *other);
void fn_8040B52C(Mtx34f *matrix, float x, float y, float z);
}

/* m becomes m * other (name inferred). */
inline void MultiplyMtx(Mtx34f *m, const Mtx34f &other) {
    fn_8040BAC4(m, &other);
}

namespace Player {
namespace Behavior {

class FirstMeet : public Behavior {
  public:
    FirstMeet(const boost::shared_ptr<PlayerBase> &player);
    virtual ~FirstMeet();
    virtual bool vf_0x0C();
    virtual void Enter();
    virtual void Leave();
    virtual void vf_0x18(CharCommand *command);
    virtual void Update(float dt);

    /* The factory record's functions (names inferred). */
    static Behavior *Create(const boost::shared_ptr<PlayerBase> &player);
    static void Release();

    /* Method names are inferred. */
    bool IsEventReady();
    void Begin();
    void End();
    void StartCamera();
    void OnSwitch(CameraModeUser::SwitchEventArgs &args);
    void OnUpdate(CameraModeUser::UpdateEventArgs &args);
    void Apply();

    static const char *s_name;

    /* 0x1C */ boost::shared_ptr<Npc::Npc> m_npc;
    /* 0x24 */ UnkCameraData_80012510 *m_data;
    /* 0x28 */ float m_time;
    /* 0x2C */ float m_step;
    /* 0x30 */ u8 m_active;
    /* 0x34 */ boost::weak_ptr<PJSCamera::ExternalCameraRequest> m_handle;
    /* 0x3C */ boost::shared_ptr<CameraModeUser> m_mode;
};
SIZE_ASSERT(FirstMeet, 0x44);

/* The camera path of each NPC character, or -1 for none (name inferred). */
static const s32 s_cameraPaths[20] = {-1, 10, 12, 8, 10, 6, 6, 10, 12, 10, 12, 13, 6, -1, -1};

const char *FirstMeet::s_name = "FirstMeet";

FirstMeet::FirstMeet(const boost::shared_ptr<PlayerBase> &player) : Behavior(player, s_name, 0x24) {
    m_npc = boost::dynamic_pointer_cast<Npc::Npc>(player);
    m_data = 0;
}

FirstMeet::~FirstMeet() {}

bool FirstMeet::vf_0x0C() {
    return true;
}

void FirstMeet::Enter() {
    m_active = 0;
}

void FirstMeet::Leave() {
    End();
}

void FirstMeet::vf_0x18(CharCommand *command) {}

void FirstMeet::Update(float dt) {
    if (!m_active && IsEventReady()) {
        Begin();
    }
    fn_80079720(m_player.get());
    fn_8009728C(&m_player, 0);
    m_step = 60.0f * dt;
    if (m_active) {
        if (m_data == 0 || fn_80011B7C(m_data, &m_time, &m_step) != 0) {
            InitializeArgs args(0);
            fn_800A7F14(m_player.get()->m_behaviors, &args);
        }
    }
}

bool FirstMeet::IsEventReady() {
    UnkObject_80074470 *state = fn_80074470();
    if (state->m_mode == 3 && state->field_0xC == 0) {
        return true;
    }
    return false;
}

void FirstMeet::Begin() {
    m_active = 1;
    fn_80175EEC(fn_80074470(), 4, 0);
    m_player.get()->m_motion->vf_0x10(0x2E, 0.0f);
    StartCamera();
    fn_80204240(reinterpret_cast<UnkNpcFields_8032D0FC *>(m_npc.get())->m_kind);
}

void FirstMeet::End() {
    if (m_active) {
        m_active = 0;
        fn_801CEAD4(&m_handle);
        fn_80175EEC(fn_80074470(), 3, 0);
        fn_80204214();
    }
}

void FirstMeet::StartCamera() {
    void *loader = fn_800122D4();
    void *resource = fn_80078C4C(m_npc.get());
    s32 index = s_cameraPaths[reinterpret_cast<UnkNpcFields_8032D0FC *>(m_npc.get())->m_kind];
    if (index < 0) {
        return;
    }
    m_data = fn_80012510(loader, resource, index);
    m_mode = fn_80009704(0, 0);
    if (m_mode.get() != 0) {
        fn_800A6A24(&m_mode.get()->m_onSwitch,
                    &(const CameraModeUser::SwitchCallback &)CameraModeUser::SwitchCallback(
                        boost::bind(&FirstMeet::OnSwitch, this, _1)));
        fn_800A673C(&m_mode.get()->m_onUpdate,
                    &(const CameraModeUser::UpdateCallback &)CameraModeUser::UpdateCallback(
                        boost::bind(&FirstMeet::OnUpdate, this, _1)));
    }
    if (m_data != 0) {
        m_time = m_data->m_startTime;
    }
    m_handle = fn_801CED24(boost::shared_ptr<CameraMode>(m_mode), 0, 0, 0, 0, 0, 1);
}

void FirstMeet::OnSwitch(CameraModeUser::SwitchEventArgs &args) {
    Apply();
}

void FirstMeet::OnUpdate(CameraModeUser::UpdateEventArgs &args) {
    Apply();
}

/* Places the camera mode on the path, at the NPC (name inferred). */
void FirstMeet::Apply() {
    if (m_data != 0) {
        Mtx34f matrix;
        Mtx34f translation;
        fn_8040B034(&translation, &fn_8006CA70(&m_npc)->m_position);
        fn_8040C148(&matrix, &translation);
        MultiplyMtx(&matrix, ((UnkNpcFields_8032D0FC *)fn_8006CA70(&m_npc))->m_baseMatrix);
        fn_8040B848(&matrix, m_npc.get()->m_rotation.y);
        fn_8040B884(&matrix, m_npc.get()->m_rotation.x);
        fn_8040B80C(&matrix, m_npc.get()->m_rotation.z);
        fn_8040B52C(&matrix, 0.0f, -fn_80073814(m_npc.get()), 0.0f);
        fn_8000FBFC(boost::shared_ptr<CameraMode>(m_mode), m_data, m_time, &matrix);
    }
}

Behavior *FirstMeet::Create(const boost::shared_ptr<PlayerBase> &player) {
    return new FirstMeet(player);
}

void FirstMeet::Release() {}

} // namespace Behavior
} // namespace Player

/* The record the behavior factory table finds FirstMeet by (0x804FA558; keeps its lbl_ name). */
extern const Player::Behavior::UnkBehaviorDesc_804EC024 lbl_804EC024 = {
    Player::Behavior::FirstMeet::Create, Player::Behavior::FirstMeet::Release, 0};
