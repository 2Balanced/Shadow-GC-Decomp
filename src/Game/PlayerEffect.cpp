/* Player::Effect (class name from RTTI; vtable 0x80521904): the task that runs a player character's
 * effects, kept at 0x23C of the player. Its state lives in an implementation object behind a
 * boost::scoped_ptr (class name unknown), which starts and ends the effect tasks of the file's
 * anonymous namespace (class names from RTTI, @unnamed@PlayerEffect_cpp@::...): RangeEffect (the
 * effects at three nodes of the body), Footmark (the effect at the feet on the ground), Slow (the
 * slow-down of Chaos Control), Awake (the hero and dark gauge's awakening), Grind (the grind
 * sound), Jet (the jets at the shoes), Belt (the ribbon of the homing attack) and CameraDemo (a
 * camera motion). The behaviours call the Effect task by the fn_ names of its functions, so they keep
 * them here. Names of functions and members are inferred. Definitions are in reverse address order
 * (deferred inlining emits functions last-defined first).
 *
 * NonMatching: four functions differ from retail only in register allocation (Awake's constructor
 * and Tick, 4 instructions each; Jet::UpdateVisibility, 1; Jet::UpdateModels, which swaps the
 * registers of the frame pointer and the loop's base pointer). Everything else matches. */
#define EFFECT_ELEMENT_POLYMORPHIC
#include <boost/bind.hpp>
#include <boost/function.hpp>
#include <boost/scoped_ptr.hpp>

#include "Effect/System/EffectElement.h"
#include "Game/CameraModeUser.h"
#include "Game/CameraState.h"
#include "Game/CharacterFiles.h"
#include "Game/DebugDispShape.h"
#include "Game/EffBelt.h"
#include "Game/EffectManagerPJS.h"
#include "Game/ExternalCameraRequest.h"
#include "Game/GadgetSound.h"
#include "Game/MathConstants.h"
#include "Game/MathTypes.h"
#include "Game/Player.h"
#include "Game/PlayerSoundBase.h"
#include "Game/PlayerWeaponBase.h"
#include "Game/Task.h"
#include "Game/Vibration.h"
#include "boost/shared_ptr.hpp"
#include "boost/weak_ptr.hpp"
#include <dolphin/types.h>
#include "size_assert.h"

using Effect::System::EffectElement;
using SonicteamUSA::System::Camera::CameraMode;
using SonicteamUSA::System::Camera::CameraModeUser;

/* The game state (see Game/TGi.h); this unit writes two floats of it (0x24 and 0x30) and passes the
 * stage record at 0xA8. */
extern u8 lbl_8057D6A0[0x150];

/* A world of the stage's areas (see Game/BulletManagerTask.cpp). */
struct UnkWorldEntry_8051D208 {
    /* 0x00 */ u8 field_0x0[0x10];
    /* 0x10 */ void *m_world;
    /* 0x14 */ u8 field_0x14[0x18 - 0x14];
};
extern UnkWorldEntry_8051D208 lbl_8051D208[];

/* The stage (class name unknown; see Game/BulletManagerTask.cpp, which emits GetWorld). */
class UnkStage_8003A840 {
  public:
    UnkWorldEntry_8051D208 *GetAreas() {
        return lbl_8051D208;
    }
    void *GetWorld(int area) {
        return GetAreas()[area].m_world;
    }
};

/* An object that keeps a clump in a world (see Game/DevilDoomCtrlUnit.h). */
class UnkClumpUpdater_80015A7C {
  public:
    UnkClumpUpdater_80015A7C(RpClump *clump, _stMtnPlayWork *motion);
    ~UnkClumpUpdater_80015A7C();

    /* 0x00 */ RpClump *m_clump;
    /* 0x04 */ u8 field_0x4[0x1C - 0x4];
};

/* A character's frame time (struct name unknown; CharInfo's word at 0x60 points at it): the time
 * step, and the step unscaled by slow motion (meaning inferred). */
struct UnkCharTime {
    /* 0x0 */ float m_dt;
    /* 0x4 */ float m_rawDt;
};

/* The effect manager's screen blur (struct name unknown; fn_8042E0C4 sets it; names inferred). */
struct UnkBlur_8042E0C4 {
    UnkBlur_8042E0C4() : m_on(false), m_alpha(0), m_scale(1.0f) {}

    /* 0x0 */ bool m_on;
    /* 0x1 */ u8 m_alpha;
    /* 0x4 */ float m_scale;
    /* 0x8 */ u8 field_0x8[0x10 - 0x8];
};

/* The effect manager's fog (struct name unknown; fn_804388A4 returns it, fn_8043891C sets it;
 * names inferred). */
struct UnkFog_804388A4 {
    UnkFog_804388A4(u8 on, u8 enabled, u8 mode);

    /* 0x0 */ u8 m_on;
    /* 0x1 */ u8 m_enabled;
    /* 0x2 */ u8 m_mode;
    /* 0x3 */ RwRGBA m_color;
    /* 0x7 */ RwRGBA m_color2;
};

/* The table of a stage's ground kinds (struct name unknown; 0x2C bytes; this unit reads the belt's
 * effect and colour). */
struct UnkGroundColor_804F1834 {
    /* 0x00 */ u8 field_0x0[0x1C];
    /* 0x1C */ s32 m_beltEffect;
    /* 0x20 */ s32 m_red;
    /* 0x24 */ s32 m_green;
    /* 0x28 */ s32 m_blue;
};
extern const UnkGroundColor_804F1834 lbl_804F1834[];

/* What a player stands on (struct name unknown; kept at 0x278 of the player): its position and
 * normal, and the kind of ground (names inferred). */
struct UnkGround {
    /* 0x00 */ Vec3f m_position;
    /* 0x0C */ Vec3f m_normal;
    /* 0x18 */ u32 field_0x18;
    /* 0x1C */ s32 m_kind;
};

namespace Player {
namespace Shadow {
/* Super Shadow's task (Game/PlayerShadowSuper.cpp). */
class Super : public Task {
  public:
    Super(Task *parent, const boost::shared_ptr<PlayerBase> &player, Super **slot);
    void Finish();

    /* 0x28 */ u8 field_0x28[0x30 - 0x28];
};
} // namespace Shadow

/* The player's barrier task (Game/PlayerBarrier.cpp). */
class Barrier : public Task {
  public:
    Barrier(Task *parent, const boost::shared_ptr<PlayerBase> &player);
    void Cancel();
    void Off();
    void Set(int kind);

    /* 0x28 */ u8 field_0x28[0x30 - 0x28];
};

class Effect;
} // namespace Player

extern "C" {
/* Test, set and clear a bit of the player's state bits. */
bool fn_8006CD78(u32 *bits, int bit);
void fn_80076CA8(u32 *bits, int bit);
void fn_80076CC8(u32 *bits, int bit);
/* Switch two of the player's states on and off (see Game/PlayerBarrier.cpp). */
void fn_8007939C(Player::PlayerBase *player, int on);
void fn_80079334(Player::PlayerBase *player, int on);
/* Marks a player's camera demo (behaviour inferred). */
void fn_80078F2C(Player::PlayerBase *player, int index, bool on);
/* A node of a motion's skeleton. */
Player::UnkMotionNode_800AA760 *fn_800AA760(Player::PlayerMotion *motion, int index);
_stMtnPlayWork *fn_800AA758(Player::PlayerMotion *motion);
void fn_800AA5D4(Player::PlayerMotion *motion, float dt);
/* Effect launches. */
boost::weak_ptr<EffectElement> fn_801D2BB0(int kind, s32 id, float *owner);
boost::weak_ptr<EffectElement> fn_801D2B38(int kind, const Vec3f *position, const Vec2f &angles,
                                           s32 id, float *owner);
boost::weak_ptr<EffectElement> fn_801D2AFC(int kind, const Vec3f *position, const Vec3f &direction,
                                           s32 id, float *owner);
boost::weak_ptr<EffectElement> fn_801D2C28(int kind, const Mtx34f *matrix, s32 id, float *owner);
void fn_80425E68(EffectElement *effect, const Mtx34f *matrix);
void fn_80425E4C(EffectElement *effect, const Vec3f *position);
void fn_80425D94(EffectElement *effect, const Vec3f *rotation);
/* The effect manager's blur and fog (behaviour inferred). */
void fn_8042E0C4(Effect::PJS::EffectManagerPJS *manager, const UnkBlur_8042E0C4 *blur);
UnkFog_804388A4 fn_804388A4(Effect::PJS::EffectManagerPJS *manager);
void fn_8043891C(Effect::PJS::EffectManagerPJS *manager, const UnkFog_804388A4 *fog);
/* Sounds. */
void *fn_80053164(void);
void fn_801D560C(void *manager, int soundId, int arg2, int arg3);
UnkSoundHandle_801D3DC8 fn_801D3EF0(int id, void *owner);
void fn_801D3888(int jingle);
void fn_801D3838(int jingle);
/* Vibration. */
boost::weak_ptr<Vibration::RequestHandle> fn_8034DE74(int pattern, int pad, float duration);
boost::weak_ptr<Vibration::RequestHandle> fn_8034DF48(int pattern, int pad);
/* The save data and its gauges (behaviour inferred). */
u8 *fn_80079808(void);
void fn_8016F0CC(void *gauges, int gauge, int points);
void fn_8016F0B4(void *gauges, int player);
void fn_8016F094(void *gauges, int player);
/* The stage record's camera demo flag (behaviour inferred). */
void fn_802039C0(void *record, u8 value);
void fn_802039D0(void *record, u8 value);
/* Files. */
void *fn_800122D4(void);
RpClump *fn_80012700(void *loader, s32 handle, const char *fileName);
UnkCameraData_80012510 *fn_80012510(void *loader, s32 handle, int index);
/* Clumps. */
RpClump *fn_80457090(RpClump *clump);
void fn_80457468(RpClump *clump);
void *fn_80169364(RpClump *clump);
int fn_80169350(void *area);
UnkStage_8003A840 *fn_8003A840(void);
void *fn_8046256C(RpClump *clump);
void fn_804623EC(void *world, RpClump *clump);
void fn_804622FC(void *world, RpClump *clump);
RwFrame *fn_804872D8(RwFrame *frame);
void fn_80039114(RpClump *clump, const float *color);
void fn_80039B94(RpClump *clump, float dt, float frame);
void fn_800159AC(UnkClumpUpdater_80015A7C *updater);
void fn_80038F94(RpClump *clump);
void fn_80038FC0(RpClump *clump);
/* Vectors and matrices. */
Vec3f fn_8040CDD0(const Vec3f *a, const Vec3f *b);
Vec3f fn_8040CDA8(const Vec3f *v, float scale);
Vec3f fn_8040CEE8(const Vec3f *v);
Vec3f fn_80016284(const Vec3f *v);
Vec2f fn_8040F664(const Vec3f &rotation);
Vec3f *fn_800091D0(Vec3f *out, const Vec3f *from);
float fn_8040EC80(float lo, float hi);
Mtx34f fn_8040B138(float angle);
Mtx34f fn_8001E044(const Mtx34f *a, const Mtx34f *b);
void fn_8040B034(Mtx34f *m, const Vec3f *offset);
void fn_8040C148(Mtx34f *m, const Mtx34f *from);
void fn_8040BAC4(Mtx34f *m, const Mtx34f *other);
void fn_8040B848(Mtx34f *m, float radians);
void fn_8040B884(Mtx34f *m, float radians);
void fn_8040B80C(Mtx34f *m, float radians);
void fn_8040B52C(Mtx34f *m, float x, float y, float z);
/* Tasks. */
bool fn_8004EAC0(Task *task);
/* Camera. */
Vec2f fn_8000F2E8(float angle);
boost::shared_ptr<CameraMode> fn_8026F6A4(int kind, UnkCameraModeArgs_8000EBB0 *args, int index);
void fn_8000FBFC(const boost::shared_ptr<CameraMode> &mode, UnkCameraData_80012510 *data, float time,
                 const Mtx34f *matrix);
int fn_80011B7C(UnkCameraData_80012510 *data, float *time, const float *step);
/* The task that fn_8028D9D8 makes (class name unknown) and its release (behaviour inferred). */
void fn_8028D9D8(void *task, Task *parent, void *impl, void *slot);
void fn_8028D834(void *task);
}

namespace {

/* The camera motions of the camera demos. */
const s32 s_cameraMotions[3] = {0x1C, 0x1D, 0x1D};

/* The nodes the jets sit at. */
const s32 s_jetNodes[4] = {7, 8, 9, 10};

} // namespace

/* An entry of the ground table (struct name unknown): the effects for landing, footmarks and
 * running, and the landing sound (names inferred; Game/GroundEffectTracker.cpp sets them). */
struct UnkGroundEffect_805217E0 {
    s32 m_landEffect;
    s32 m_footEffect;
    s32 m_runEffect;
    s32 m_landSound;
};

static const UnkGroundEffect_805217E0 s_defaultGroundEffects[16] = {
    {0x20, 0x56, -1, 0x7004},   {-1, -1, -1, 0x7005},     {-1, -1, -1, 0x7006},
    {0x22, 0x35, -1, 0x7007},   {0x21, 0x2E, -1, 0x7008}, {0x21, 0x2E, -1, 0x7009},
    {0x20, 0x32, -1, 0x700A},   {-1, -1, -1, 0x700B},     {-1, -1, -1, 0x700C},
    {0x67, 0x64, -1, 0x700D},   {0x2B, 0x2F, -1, 0x700E}, {-1, 0x59, -1, 0x700F},
    {-1, 0x1A6, -1, 0x7010},    {-1, -1, -1, 0x7011},     {-1, -1, -1, 0x7012},
    {-1, -1, -1, -1},
};

namespace {

/* The effects at the nodes of the body: node and effect. */
struct RangeKind {
    s32 m_node;
    s32 m_effect;
};
const RangeKind s_rangeKinds[3] = {{6, 0x38}, {4, 0x38}, {8, 0x1E}};

} // namespace

static UnkGroundEffect_805217E0 s_groundEffects[16] = {
    {0x20, 0x56, -1, 0x7004},   {-1, -1, -1, 0x7005},     {-1, -1, -1, 0x7006},
    {0x22, 0x35, -1, 0x7007},   {0x21, 0x2E, -1, 0x7008}, {0x21, 0x2E, -1, 0x7009},
    {0x20, 0x32, -1, 0x700A},   {-1, -1, -1, 0x700B},     {-1, -1, -1, 0x700C},
    {0x67, 0x64, -1, 0x700D},   {0x2B, 0x2F, -1, 0x700E}, {-1, 0x59, -1, 0x700F},
    {-1, 0x1A6, -1, 0x7010},    {-1, -1, -1, 0x7011},     {-1, -1, -1, 0x7012},
    {-1, -1, -1, -1},
};


inline UnkFog_804388A4::UnkFog_804388A4(u8 on, u8 enabled, u8 mode)
    : m_on(on), m_enabled(enabled), m_mode(mode) {
    RwRGBA color = {0xFF, 0xFF, 0xFF, 0xFF};
    m_color = color;
    RwRGBA color2 = {0, 0, 0, 0};
    m_color2 = color2;
}

/* The id of the effect an element was launched as (the description at 0x44; inferred). */
inline s32 EffectId(EffectElement *effect) {
    return *reinterpret_cast<s32 *>(*reinterpret_cast<u8 **>(&effect->field_0x34[0x44 - 0x34]) + 0x24);
}

/* The skeleton nodes' positions as the effects read them (layout of UnkMotionNode_800AA760). */
inline const Vec3f &NodePosition(Player::UnkMotionNode_800AA760 *node) {
    return *reinterpret_cast<const Vec3f *>(&node->field_0x4[0x4 - 0x4]);
}
inline const Vec3f &NodeDirection(Player::UnkMotionNode_800AA760 *node) {
    return *reinterpret_cast<const Vec3f *>(&node->field_0x4[0x1C - 0x4]);
}

namespace Player {

/* The motion's effects that the Belt and the Awake tasks drive (offsets 0x40 and 0x48). */
inline boost::weak_ptr<EffectElement> &BeltEffect(PlayerMotion *motion) {
    return *reinterpret_cast<boost::weak_ptr<EffectElement> *>(motion->field_0x28 + (0x40 - 0x28));
}
inline boost::weak_ptr<EffectElement> &AwakeEffect(PlayerMotion *motion) {
    return *reinterpret_cast<boost::weak_ptr<EffectElement> *>(motion->field_0x28 + (0x48 - 0x28));
}

class Effect : public Task {
  public:
    class Impl;

    Effect(Task *parent, const boost::shared_ptr<PlayerBase> &player);
    virtual ~Effect();
    virtual void Tick(float dt);

    /* 0x28 */ boost::scoped_ptr<Impl> m_impl;
    /* 0x2C */ u32 pad_0x2C;
};
SIZE_ASSERT(Effect, 0x30);

} // namespace Player

extern "C" {
boost::weak_ptr<EffectElement> fn_800A2754(Player::Effect *effect, s32 id);
boost::weak_ptr<EffectElement> fn_800A2794(Player::Effect *effect, const Vec3f *position,
                                           const Vec2f &angles, s32 id);
boost::weak_ptr<EffectElement> fn_800A27D4(Player::Effect *effect, const Vec3f *position,
                                           const Vec3f &direction, s32 id);
boost::weak_ptr<EffectElement> fn_800A2814(Player::Effect *effect, const Mtx34f *matrix, s32 id);
boost::weak_ptr<EffectElement> fn_800A2854(Player::Effect *effect, int node, s32 id);
Effect::PJS::EffectManagerPJS *fn_800A31F0(void);
void fn_800A4210(boost::shared_ptr<EffectElement> *effect);
}

namespace {

using Player::PlayerBase;
using Player::PlayerMotion;
using Player::UnkMotionNode_800AA760;

/* Puts clump in world (name inferred; as in Game/BulletManagerTask.cpp). */
inline void SetClumpWorld(RpClump *clump, void *world) {
    void *current = fn_8046256C(clump);
    if (current != world) {
        if (current != 0) {
            fn_804623EC(current, clump);
        }
        fn_804622FC(world, clump);
    }
}

/* Matrix helpers around the math library (names inferred). */
inline void SetTranslation(Mtx34f &matrix, const Vec3f &position) {
    fn_8040B034(&matrix, &position);
}
inline void Multiply(Mtx34f &matrix, const Mtx34f &other) {
    fn_8040BAC4(&matrix, &other);
}

/* A frame time of the player. */
inline UnkCharTime *Time(PlayerBase *player) {
    return *reinterpret_cast<UnkCharTime **>(player->field_0x60);
}

/* CameraDemo: plays a camera motion around the player (kind 1 hands over to a following camera at
 * its end). */
class CameraDemo : public Task {
  public:
    CameraDemo(Task *parent, const boost::shared_ptr<PlayerBase> &player, int kind);
    virtual ~CameraDemo();
    virtual void Tick(float dt);

    void UpdateView();
    void OnSwitch(CameraModeUser::SwitchEventArgs &args);
    void OnUpdate(CameraModeUser::UpdateEventArgs &args);
    void Start();
    void SetupMode();
    void StartBlend();
    void StartCut();
    void Finish();

    /* 0x28 */ boost::shared_ptr<PlayerBase> m_player;
    /* 0x30 */ s32 m_kind;
    /* 0x34 */ UnkCameraData_80012510 *m_data;
    /* 0x38 */ float m_time;
    /* 0x3C */ float m_step;
    /* 0x40 */ s32 m_state;
    /* 0x44 */ boost::weak_ptr<PJSCamera::ExternalCameraRequest> m_request;
    /* 0x4C */ boost::shared_ptr<CameraModeUser> m_mode;
    /* 0x54 */ u32 pad_0x54;
};
SIZE_ASSERT(CameraDemo, 0x58);

CameraDemo::CameraDemo(Task *parent, const boost::shared_ptr<PlayerBase> &player, int kind)
    : Task(parent), m_player(player), m_kind(kind) {
    m_name = "CameraDemo";
    m_data = 0;
    void *loader = fn_800122D4();
    m_data = fn_80012510(loader, fn_80078C88()->GetHandle(0xC), s_cameraMotions[m_kind]);
    Start();
    fn_80078F2C(m_player.get(), m_player->field_0x54, true);
}

CameraDemo::~CameraDemo() {
    fn_801CEAD4(&m_request);
    fn_802039D0(&lbl_8057D6A0[0xA8], 0);
    fn_80078F2C(m_player.get(), m_player->field_0x54, false);
}

void CameraDemo::Tick(float dt) {
    m_step = 60.0f * dt * Time(m_player.get())->m_rawDt;
    switch (m_state) {
    case 0:
        if (fn_80011B7C(m_data, &m_time, &m_step)) {
            if (m_kind == 1) {
                Finish();
                m_state++;
            } else {
                m_flags |= 1;
            }
        }
        break;
    }
    if (!fn_8006CD78(m_player->m_stateBits, 0x16) && !fn_8006CD78(m_player->m_stateBits, 0x14)) {
        m_flags |= 1;
    }
}

void CameraDemo::UpdateView() {
    if (m_data != 0) {
        Mtx34f matrix;
        Mtx34f translation;
        SetTranslation(translation, m_player->m_position);
        fn_8040C148(&matrix, &translation);
        Multiply(matrix, *reinterpret_cast<const Mtx34f *>(&m_player->field_0xB0[0x1E4 - 0xB0]));
        fn_8040B848(&matrix, m_player.get()->m_rotation.y);
        fn_8040B884(&matrix, m_player.get()->m_rotation.x);
        fn_8040B80C(&matrix, m_player.get()->m_rotation.z);
        fn_8040B52C(&matrix, 0.0f, -m_player.get()->field_0x274, 0.0f);
        fn_8000FBFC(boost::shared_ptr<CameraMode>(m_mode), m_data, m_time, &matrix);
    }
}

void CameraDemo::OnSwitch(CameraModeUser::SwitchEventArgs &args) {
    UpdateView();
}

void CameraDemo::OnUpdate(CameraModeUser::UpdateEventArgs &args) {
    UpdateView();
}

void CameraDemo::Start() {
    m_time = 0.0f;
    m_state = 0;
    switch (m_kind) {
    case 0:
        StartBlend();
        break;
    case 1:
        StartCut();
        break;
    }
    fn_802039D0(&lbl_8057D6A0[0xA8], 1);
}

void CameraDemo::SetupMode() {
    m_mode = fn_80009704(0, 0);
    if (m_mode) {
        fn_800A6A24(&m_mode.get()->m_onSwitch,
                    &(const CameraModeUser::SwitchCallback &)CameraModeUser::SwitchCallback(
                        boost::bind(&CameraDemo::OnSwitch, this, _1)));
        fn_800A673C(&m_mode.get()->m_onUpdate,
                    &(const CameraModeUser::UpdateCallback &)CameraModeUser::UpdateCallback(
                        boost::bind(&CameraDemo::OnUpdate, this, _1)));
    }
    if (m_data != 0) {
        m_time = m_data->m_startTime;
    }
}

void CameraDemo::StartBlend() {
    SetupMode();
    UnkCameraBlend_8000B2C0 blend(0.25f, 0.0f);
    m_request = fn_801CED24(boost::shared_ptr<CameraMode>(m_mode), 0, 0, 0, 1,
                            reinterpret_cast<int>(&blend), 1);
}

void CameraDemo::StartCut() {
    SetupMode();
    m_request = fn_801CED24(boost::shared_ptr<CameraMode>(m_mode), 0, 0, 0, -1, 0, 0);
}

/* Sets a camera view's window (name inferred). */
static void SetWindow(UnkCameraView_8000B4AC *view, const Vec2f &window) {
    view->m_windowX = window.x;
    view->m_windowY = window.y;
}

void CameraDemo::Finish() {
    fn_801CEAD4(&m_request);
    fn_802039D0(&lbl_8057D6A0[0xA8], 0);
    UnkCameraModeArgs_8000EBB0 args;
    SetWindow(&args.m_view, fn_8000F2E8(140.0f * (lbl_805FAD38 / 180.0f)));
    args.m_distance = 200.0f;
    args.m_height = 15.0f;
    args.field_0x54 = 10.0f;
    args.field_0x58 = 360.0f;
    args.field_0x5C = 60.0f;
    args.field_0x60 = 180.0f;
    UnkCameraBlend_8000B2C0 blendIn(1.0f, 2.0f);
    UnkCameraBlend_8000B2C0 blendOut(0.0f, 0.5f);
    boost::shared_ptr<CameraMode> mode = fn_8026F6A4(0xB, &args, 0);
    m_request = fn_801CED24(boost::shared_ptr<CameraMode>(mode), 0, 2,
                            reinterpret_cast<int>(&blendIn), 3, reinterpret_cast<int>(&blendOut),
                            0);
}

/* Belt: the ribbon behind the player during the homing attack. */
class Belt : public Task {
  public:
    Belt(Task *parent, const boost::shared_ptr<PlayerBase> &player, float width);
    virtual ~Belt();
    virtual void Tick(float dt);

    /* 0x28 */ Test::EffBelt *m_belt;
    /* 0x2C */ boost::shared_ptr<PlayerBase> m_player;
    /* 0x34 */ u32 pad_0x34;
};
SIZE_ASSERT(Belt, 0x38);

Belt::Belt(Task *parent, const boost::shared_ptr<PlayerBase> &player, float width)
    : Task(parent), m_player(player) {
    m_name = "Belt";
    const UnkGroundColor_804F1834 *color =
        &lbl_804F1834[*reinterpret_cast<s32 *>(&m_player->field_0x64[0])];
    m_belt = new Test::EffBelt(boost::weak_ptr<PlayerBase>(m_player), 0x3C,
                               UnkColor_8040D2EC(color->m_red, color->m_green, color->m_blue),
                               width);
    if (PlayerMotion *motion = m_player->m_motion) {
        boost::weak_ptr<EffectElement> &effect = BeltEffect(motion);
        if (!effect.expired()) {
            effect.lock()->Retire();
        }
        effect = fn_800A2854(reinterpret_cast<Player::Effect *>(m_player->field_0x23C), 0,
                             color->m_beltEffect);
        if (m_player->IsWeaponHidden()) {
            boost::shared_ptr<EffectElement> shown = effect.lock();
            if (shown) {
                shown->Hide();
            }
        }
    }
}

Belt::~Belt() {
    if (m_belt != 0) {
        m_belt->m_flags |= 1;
    }
    if (PlayerMotion *motion = m_player->m_motion) {
        boost::weak_ptr<EffectElement> &effect = BeltEffect(motion);
        if (!effect.expired()) {
            effect.lock()->Kill();
        }
    }
}

void Belt::Tick(float dt) {
    if (m_belt == 0 || !fn_8004EAC0(m_belt)) {
        m_flags |= 1;
    } else {
        m_belt->AddPosition(m_player.get()->m_position);
    }
}

/* Jet: the jets of the shoes (models that turn at four nodes of the feet). */
class Jet : public Task {
  public:
    Jet(Task *parent, const boost::shared_ptr<PlayerBase> &player, Jet **slot, bool voice);
    virtual ~Jet();
    virtual void Tick(float dt);

    void UpdateModels();
    void UpdateVisibility();

    void End() {
        if (m_state < 500) {
            m_state = 500;
        }
    }

    /* 0x28 */ boost::shared_ptr<PlayerBase> m_player;
    /* 0x30 */ Jet **m_slot;
    /* 0x34 */ s32 m_state;
    /* 0x38 */ bool m_hidden;
    /* 0x39 */ bool m_voice;
    /* 0x3C */ UnkSoundHandle_801D3DC8 m_sound;
    /* 0x58 */ float m_alpha;
    /* 0x5C */ RpClump *m_clumps[4];
    /* 0x6C */ RwFrame *m_frames[4];
    /* 0x7C */ float m_angles[4];
    /* 0x8C */ u32 pad_0x8C;
};
SIZE_ASSERT(Jet, 0x90);

Jet::Jet(Task *parent, const boost::shared_ptr<PlayerBase> &player, Jet **slot, bool voice)
    : Task(parent), m_player(player), m_slot(slot), m_voice(voice) {
    m_name = "Jet";
    if (m_slot != 0) {
        *m_slot = this;
    }
    m_state = 0;
    m_hidden = false;
    m_alpha = 0.0f;
    RpClump *clump;
    void *loader = fn_800122D4();
    UnkStage_8003A840 *stage = fn_8003A840();
    s32 handle = fn_80078C88()->GetHandle(0xC);
    m_clumps[0] = fn_80012700(loader, handle, "SHADOW_FOOTEFF_R.DFF");
    m_clumps[1] = 0;
    m_clumps[2] = 0;
    m_clumps[3] = 0;
    if (m_clumps[0] != 0) {
        m_clumps[1] = fn_80457090(m_clumps[0]);
        m_clumps[2] = fn_80457090(m_clumps[0]);
        m_clumps[0] = fn_80457090(m_clumps[0]);
    }
    for (int i = 0; i < 4; i++) {
        if (m_clumps[i] != 0) {
            int area = fn_80169350(fn_80169364(m_clumps[i]));
            clump = m_clumps[i];
            SetClumpWorld(clump, stage->GetWorld(area));
            m_frames[i] = fn_804872D8(m_clumps[i]->frame);
        }
        m_angles[i] = 0.0f;
    }
}

Jet::~Jet() {
    if (m_slot != 0) {
        *m_slot = 0;
    }
    for (int i = 0; i < 4; i++) {
        if (m_clumps[i] != 0) {
            void *world = fn_8046256C(m_clumps[i]);
            if (world != 0) {
                fn_804623EC(world, m_clumps[i]);
            }
            fn_80457468(m_clumps[i]);
        }
    }
    m_sound.fn_801D455C();
}

void Jet::Tick(float dt) {
    float step = Time(m_player.get())->m_dt;
    switch (m_state) {
    case 0:
        m_sound.fn_801D455C();
        if (m_voice) {
            m_sound = m_player->m_sound->PlayVoice(6, reinterpret_cast<s32>(this));
        }
        m_state++;
        break;
    case 1:
        m_alpha += 15.0f * step;
        if (m_alpha >= 1.0f) {
            m_alpha = 1.0f;
            m_state = 100;
        }
        break;
    case 100:
        break;
    case 500:
        m_sound.fn_801D455C();
        m_state++;
    case 501:
        m_alpha -= 4.0f * step;
        if (m_alpha <= 0.0f) {
            m_alpha = 0.0f;
            m_flags |= 1;
            if (m_slot != 0) {
                *m_slot = 0;
            }
            m_slot = 0;
        }
        break;
    }
    UpdateModels();
    UpdateVisibility();
}

void Jet::UpdateModels() {
    int i;
    RwFrame *frame;
    for (i = 0; i < 4; i++) {
        if (m_clumps[i] != 0) {
            float color[4];
            color[3] = m_alpha;
            color[0] = -1.0f;
            color[1] = -1.0f;
            color[2] = -1.0f;
            fn_80039114(m_clumps[i], color);
            m_angles[i] += fn_8040EC80(40.0f * (lbl_805FAD38 / 180.0f),
                                       50.0f * (lbl_805FAD38 / 180.0f));
            UnkMotionNode_800AA760 *node = fn_800AA760(m_player->m_motion, s_jetNodes[i]);
            frame = m_frames[i];
            frame->modelling = fn_800591F0(
                &(const Mtx34f &)fn_8001E044(node->m_matrix, &(const Mtx34f &)fn_8040B138(m_angles[i])));
            fn_8048722C(m_frames[i]);
        }
    }
}

void Jet::UpdateVisibility() {
    int i;
    if (m_player->IsWeaponHidden()) {
        if (!m_hidden) {
            m_hidden = true;
            for (i = 0; i < 4; i++) {
                if (m_clumps[i] != 0) {
                    fn_80038FC0(m_clumps[i]);
                }
            }
        }
    } else if (m_hidden) {
        m_hidden = false;
        for (i = 0; i < 4; i++) {
            if (m_clumps[i] != 0) {
                fn_80038F94(m_clumps[i]);
            }
        }
    }
}

/* Grind: the sound of grinding. */
class Grind : public Task {
  public:
    Grind(Task *parent, const boost::shared_ptr<PlayerBase> &player, Grind **slot);
    virtual ~Grind();
    virtual void Tick(float dt);

    void Play();
    void SetSparks(bool on);
    void End();

    void Stop() {
        if (m_playing) {
            m_sound.fn_801D455C();
            m_playing = false;
        }
    }

    /* 0x28 */ boost::shared_ptr<PlayerBase> m_player;
    /* 0x30 */ Grind **m_slot;
    /* 0x34 */ bool m_playing;
    /* 0x38 */ UnkSoundHandle_801D3DC8 m_sound;
    /* 0x54 */ u32 pad_0x54;
};
SIZE_ASSERT(Grind, 0x58);

Grind::Grind(Task *parent, const boost::shared_ptr<PlayerBase> &player, Grind **slot)
    : Task(parent), m_player(player), m_slot(slot) {
    m_name = "Grind";
    if (m_slot != 0) {
        *m_slot = this;
    }
    m_playing = false;
    Play();
}

Grind::~Grind() {
    if (m_slot != 0) {
        *m_slot = 0;
    }
    Stop();
}

void Grind::Tick(float dt) {}

void Grind::End() {
    Stop();
    m_flags |= 1;
}

void Grind::SetSparks(bool on) {
    if (on) {
        Play();
    } else {
        Stop();
    }
}

void Grind::Play() {
    if (!m_playing) {
        switch (*reinterpret_cast<s32 *>(&lbl_8057D6A0[0xA8])) {
        case 0x12E:
        case 0x194:
            fn_801D560C(fn_80053164(), 0x7502, 0, 0);
            m_sound = fn_801D3EF0(0x7503, this);
            break;
        default:
            m_player->m_sound->StopVoice(10);
            m_sound = m_player->m_sound->PlayVoice(0xB, reinterpret_cast<s32>(this));
            break;
        }
        fn_8034DF48(0xA, m_player->field_0x54);
        m_playing = true;
    }
}

/* Awake: the hero or dark awakening (a model around the player, an effect and a gauge that runs
 * down). */
struct AwakeKind {
    /* 0x00 */ s32 m_stateBit;
    /* 0x04 */ s32 m_startEffect;
    /* 0x08 */ s32 m_effect;
    /* 0x0C */ s32 m_voice;
    /* 0x10 */ s32 m_jingle;
    /* 0x14 */ s32 m_gauge;
    /* 0x18 */ s32 m_side;
    /* 0x1C */ const char *m_model;
};

const AwakeKind s_awakeKinds[2] = {
    {0x23, 0x4B, 0x54, 0x1A, 3, 1, 0, "EFFSHADOWEVIL.DFF"},
    {0x22, 0x3F, 0x49, 0x19, 4, 0, 1, "EFFSHADOWHERO.DFF"},
};

/* A gauge of the save data (struct name unknown). */
struct UnkGauge {
    /* 0x0 */ s32 m_value;
    /* 0x4 */ u8 field_0x4[0xC - 0x4];
};

class Awake : public Task {
  public:
    Awake(Task *parent, const boost::shared_ptr<PlayerBase> &player, Awake **slot, int kind);
    virtual ~Awake();
    virtual void Tick(float dt);

    void Hide();
    void Show();
    void AddToWorld();
    void RemoveFromWorld();

    void End() {
        if (m_state < 500) {
            m_state = 500;
        }
    }
    void Detach() {
        if (m_slot != 0) {
            *m_slot = 0;
        }
        m_slot = 0;
    }
    void Finish() {
        if (m_state < 500) {
            m_state = 500;
        }
        if (m_slot != 0) {
            *m_slot = 0;
        }
        m_slot = 0;
    }

    /* 0x28 */ boost::shared_ptr<PlayerBase> m_player;
    /* 0x30 */ UnkClumpUpdater_80015A7C *m_model;
    /* 0x34 */ Awake **m_slot;
    /* 0x38 */ float m_alpha;
    /* 0x3C */ s32 m_state;
    /* 0x40 */ s32 m_kind;
    /* 0x44 */ float m_time;
    /* 0x48 */ bool m_hidden;
    /* 0x4C */ u32 pad_0x4C;
};
SIZE_ASSERT(Awake, 0x50);

Awake::Awake(Task *parent, const boost::shared_ptr<PlayerBase> &player, Awake **slot, int kind)
    : Task(parent), m_player(player), m_slot(slot), m_kind(kind) {
    RpClump *clump;
    _stMtnPlayWork *work = 0;
    m_name = "Awake";
    m_model = 0;
    m_hidden = false;
    if (m_player->m_motion != 0) {
        work = fn_800AA758(m_player->m_motion);
    }
    m_alpha = 0.0f;
    m_time = 0.0f;
    if (m_slot != 0) {
        *m_slot = this;
    }
    const AwakeKind *awake = &s_awakeKinds[m_kind];
    void *loader = fn_800122D4();
    clump = fn_80012700(loader, fn_80078C88()->GetHandle(0xC), awake->m_model);
    if (clump != 0 && work != 0) {
        m_model = new UnkClumpUpdater_80015A7C(clump, work);
    }
    fn_8007939C(m_player.get(), 1);
    fn_80079334(m_player.get(), 1);
    if (PlayerMotion *motion = m_player->m_motion) {
        boost::weak_ptr<EffectElement> &effect = AwakeEffect(motion);
        if (!effect.expired()) {
            effect.lock()->Retire();
        }
        effect = fn_800A2854(reinterpret_cast<Player::Effect *>(m_player->field_0x23C), 1,
                             awake->m_startEffect);
        if (m_player->IsWeaponHidden()) {
            Hide();
        }
    }
    fn_80076CA8(m_player->m_stateBits, awake->m_stateBit);
    m_player->m_sound->StopVoice(awake->m_voice);
    fn_801D3888(awake->m_jingle);
    fn_8016F0B4(fn_80079808() + 0x70C, awake->m_side);
    fn_8034DE74(8, m_player->field_0x54, 2.0f);
    m_state = 0;
}

Awake::~Awake() {
    fn_8007939C(m_player.get(), 0);
    fn_80079334(m_player.get(), 0);
    if (PlayerMotion *motion = m_player->m_motion) {
        boost::weak_ptr<EffectElement> &effect = AwakeEffect(motion);
        if (!effect.expired()) {
            effect.lock()->Kill();
        }
    }
    const AwakeKind *awake = &s_awakeKinds[m_kind];
    fn_801D3838(awake->m_jingle);
    fn_8016F094(fn_80079808() + 0x70C, awake->m_side);
    RemoveFromWorld();
    if (m_model != 0) {
        delete m_model;
    }
    if (m_slot != 0) {
        *m_slot = 0;
    }
}

void Awake::Tick(float dt) {
    const AwakeKind *awake = &s_awakeKinds[m_kind];
    float step = Time(m_player.get())->m_dt;
    u8 *save = fn_80079808();
    if (!fn_8006CD78(m_player->m_stateBits, 0x16) && !fn_8006CD78(m_player->m_stateBits, 0x14) &&
        !fn_8006CD78(m_player->m_stateBits, 0x15)) {
        fn_8016F0CC(save + 0x70C, awake->m_gauge, -1500.0f * step);
    }
    if (reinterpret_cast<UnkGauge *>(save + 0x70C)[awake->m_gauge].m_value <= 0.0f) {
        fn_80076CC8(m_player->m_stateBits, awake->m_stateBit);
    }
    if (!fn_8006CD78(m_player->m_stateBits, awake->m_stateBit)) {
        End();
    }
    m_time += step;
    switch (m_state) {
    case 0:
        if (m_time >= 1.5f) {
            if (PlayerMotion *motion = m_player->m_motion) {
                boost::weak_ptr<EffectElement> &effect = AwakeEffect(motion);
                if (!effect.expired()) {
                    effect.lock()->Retire();
                }
                effect = fn_800A2854(reinterpret_cast<Player::Effect *>(m_player->field_0x23C), 1,
                                     s_awakeKinds[m_kind].m_effect);
            }
            m_hidden = true;
            Show();
            AddToWorld();
            m_state = 100;
        }
        break;
    case 100:
        m_alpha += 4.0f * step;
        if (m_alpha >= 1.0f) {
            m_alpha = 1.0f;
            m_state++;
        }
        break;
    case 101:
        break;
    case 500:
        if (!fn_8006CD78(m_player->m_stateBits, 0x16) &&
            !fn_8006CD78(m_player->m_stateBits, 0x14) &&
            !fn_8006CD78(m_player->m_stateBits, 0x15)) {
            m_state++;
        }
        break;
    case 501:
        m_alpha -= 2.0f * step;
        if (m_alpha <= 0.0f) {
            m_alpha = 0.0f;
            m_flags |= 1;
        }
        break;
    }
    if (m_model != 0) {
        float color[4];
        color[3] = m_alpha;
        color[0] = -1.0f;
        color[1] = -1.0f;
        color[2] = -1.0f;
        fn_80039114(m_model->m_clump, color);
        fn_80039B94(m_model->m_clump, step, -1.0f);
        fn_800159AC(m_model);
    }
    if (m_player->IsWeaponHidden()) {
        Hide();
    } else {
        Show();
    }
}

void Awake::Hide() {
    if (!m_hidden) {
        m_hidden = true;
        if (m_model != 0 && m_model->m_clump != 0) {
            fn_80038FC0(m_model->m_clump);
        }
    }
}

void Awake::Show() {
    if (m_hidden) {
        m_hidden = false;
        if (m_model != 0 && m_model->m_clump != 0) {
            fn_80038F94(m_model->m_clump);
        }
    }
}

void Awake::AddToWorld() {
    if (m_model != 0) {
        RpClump *clump = m_model->m_clump;
        int area = fn_80169350(fn_80169364(clump));
        void *world = fn_8003A840()->GetAreas()[area].m_world;
        if (world != 0) {
            void *current = fn_8046256C(clump);
            if (current != world) {
                if (current != 0) {
                    fn_804623EC(current, clump);
                }
                fn_804622FC(world, clump);
            }
        }
    }
}

void Awake::RemoveFromWorld() {
    if (m_model != 0) {
        RpClump *clump = m_model->m_clump;
        void *world = fn_8046256C(clump);
        if (world != 0) {
            fn_804623EC(world, clump);
        }
    }
}

/* Slow: the slow-down of Chaos Control (it blurs and tints the screen while it runs). */
class Slow : public Task {
  public:
    Slow(Task *parent, const boost::shared_ptr<PlayerBase> &player, Slow **slot);
    virtual ~Slow();
    virtual void Tick(float dt);

    void UpdateBlur();

    void End() {
        m_ending = true;
    }

    /* 0x28 */ boost::shared_ptr<PlayerBase> m_player;
    /* 0x30 */ Slow **m_slot;
    /* 0x34 */ bool m_ending;
    /* 0x38 */ float m_amount;
    /* 0x3C */ u32 pad_0x3C;
};
SIZE_ASSERT(Slow, 0x40);

Slow::Slow(Task *parent, const boost::shared_ptr<PlayerBase> &player, Slow **slot)
    : Task(parent), m_player(player), m_slot(slot) {
    m_name = "Slow";
    if (m_slot != 0) {
        *m_slot = this;
    }
    m_ending = false;
    m_amount = 0.0f;
    fn_80076CA8(m_player->m_stateBits, 0x15);
    fn_802039C0(&lbl_8057D6A0[0xA8], 1);
    UnkFog_804388A4 fog(0, 0, 0);
    fog = fn_804388A4(fn_800A31F0());
    fog.m_on = 1;
    fog.m_enabled = 1;
    fn_8043891C(fn_800A31F0(), &fog);
}

Slow::~Slow() {
    fn_80076CC8(m_player->m_stateBits, 0x15);
    fn_802039C0(&lbl_8057D6A0[0xA8], 0);
    *reinterpret_cast<float *>(&lbl_8057D6A0[0x24]) = *reinterpret_cast<float *>(&lbl_8057D6A0[0x30]) =
        1.0f;
    fn_801D3838(2);
    UnkFog_804388A4 fog(0, 0, 0);
    fog = fn_804388A4(fn_800A31F0());
    fog.m_on = 0;
    fog.m_enabled = 0;
    fn_8043891C(fn_800A31F0(), &fog);
    if (m_slot != 0) {
        *m_slot = 0;
    }
}

void Slow::Tick(float dt) {
    if (m_ending) {
        m_amount -= dt / 0.5f;
        if (m_amount < 0.0f) {
            m_amount = 0.0f;
        }
    } else {
        m_amount += dt / 0.5f;
        if (m_amount > 1.0f) {
            m_amount = 1.0f;
        }
    }
    *reinterpret_cast<float *>(&lbl_8057D6A0[0x24]) = *reinterpret_cast<float *>(&lbl_8057D6A0[0x30]) =
        0.125f * m_amount;
    UpdateBlur();
    fn_8016F0CC(fn_80079808() + 0x70C, 0, -2000.0f * Time(m_player.get())->m_dt);
    if (m_amount <= 0.0f) {
        m_flags |= 1;
    }
}

void Slow::UpdateBlur() {
    UnkBlur_8042E0C4 blur;
    blur.m_on = m_amount <= 0.0f ? false : true;
    blur.m_alpha = 64.0f * m_amount;
    blur.m_scale = 1.05f;
    fn_8042E0C4(fn_800A31F0(), &blur);
}

/* Footmark: the effect at the feet while the player stands on the ground. */
class Footmark : public Task {
  public:
    Footmark(Task *parent, const boost::shared_ptr<PlayerBase> &player, Footmark **slot);
    virtual ~Footmark();
    virtual void Tick(float dt);

    s32 GetEffect();

    /* 0x28 */ boost::shared_ptr<PlayerBase> m_player;
    /* 0x30 */ Footmark **m_slot;
    /* 0x34 */ boost::weak_ptr<EffectElement> m_effect;
    /* 0x3C */ u32 pad_0x3C;
};
SIZE_ASSERT(Footmark, 0x40);

Footmark::Footmark(Task *parent, const boost::shared_ptr<PlayerBase> &player, Footmark **slot)
    : Task(parent), m_player(player), m_slot(slot) {
    m_name = "Footmark";
    if (m_slot != 0) {
        *m_slot = this;
    }
}

Footmark::~Footmark() {
    if (m_slot != 0) {
        *m_slot = 0;
    }
    if (!m_effect.expired()) {
        m_effect.lock()->Stop();
    }
}

} // namespace

/* Releases an effect pointer (boost::shared_ptr<EffectElement>::reset; other units call it by
 * this name). */
extern "C" void fn_800A4210(boost::shared_ptr<EffectElement> *effect) {
    boost::shared_ptr<EffectElement>().swap(*effect);
}

namespace {

void Footmark::Tick(float dt) {
    boost::shared_ptr<EffectElement> effect = m_effect.lock();
    if (fn_8006CD78(m_player->m_stateBits, 2) && fn_8006CD78(m_player->m_stateBits, 3)) {
        UnkMotionNode_800AA760 *right = fn_800AA760(m_player->m_motion, 10);
        UnkMotionNode_800AA760 *left = fn_800AA760(m_player->m_motion, 8);
        Vec3f position;
        fn_800091D0(&position, &(const Vec3f &)fn_8040CDA8(&(const Vec3f &)fn_8040CDD0(&NodePosition(right), &NodePosition(left)),
                                             0.5f));
        Vec3f direction;
        fn_800091D0(&direction,
                    &(const Vec3f &)fn_8040CEE8(&(const Vec3f &)fn_8040CDA8(
                        &(const Vec3f &)fn_8040CDD0(&NodeDirection(right), &NodeDirection(left)), -0.5f)));
        s32 id = GetEffect();
        if (effect && EffectId(effect.px) != id) {
            effect->Stop();
            fn_800A4210(&effect);
            m_effect.reset();
        }
        if (!effect && id >= 0) {
            m_effect = fn_800A27D4(reinterpret_cast<Player::Effect *>(m_player->field_0x23C),
                                   &position, direction, id);
            effect = m_effect.lock();
        }
        if (effect) {
            Vec2f angles = fn_8040F664(fn_80016284(&direction));
            float pitch = angles.x;
            float yaw = angles.y;
            fn_80425E4C(effect.px, &position);
            fn_80425D94(effect.px, &Vec3f(pitch, yaw, 0.0f));
            if (m_player->IsWeaponHidden()) {
                effect->Hide();
            } else {
                effect->Show();
            }
        }
    } else if (effect) {
        effect->Stop();
        m_effect.reset();
    }
}

s32 Footmark::GetEffect() {
    return s_groundEffects[(*reinterpret_cast<UnkGround **>(m_player.get()->field_0x278))->m_kind]
        .m_footEffect;
}

/* RangeEffect: the effects at three nodes of the body. */
struct RangeEntry {
    /* 0x0 */ boost::weak_ptr<EffectElement> m_effect;
    /* 0x8 */ bool m_active;
};

class RangeEffect : public Task {
  public:
    RangeEffect(Task *parent, const boost::shared_ptr<PlayerBase> &player, RangeEffect **slot);
    virtual ~RangeEffect();
    virtual void Tick(float dt);

    void Start(const int &kind);
    void Update(int kind);

    /* 0x28 */ boost::shared_ptr<PlayerBase> m_player;
    /* 0x30 */ RangeEffect **m_slot;
    /* 0x34 */ RangeEntry m_entries[3];
};
SIZE_ASSERT(RangeEffect, 0x58);

RangeEffect::RangeEffect(Task *parent, const boost::shared_ptr<PlayerBase> &player,
                         RangeEffect **slot)
    : Task(parent), m_player(player), m_slot(slot) {
    m_name = "RangeEffect";
    if (m_slot != 0) {
        *m_slot = this;
    }
    for (int i = 0; i < 3; i++) {
        m_entries[i].m_active = false;
    }
}

RangeEffect::~RangeEffect() {
    if (m_slot != 0) {
        *m_slot = 0;
    }
    boost::shared_ptr<EffectElement> effect;
    for (int i = 0; i < 3; i++) {
        effect = m_entries[i].m_effect.lock();
        if (effect) {
            effect->Stop();
            m_entries[i].m_effect.reset();
            m_entries[i].m_active = false;
        }
    }
}

void RangeEffect::Tick(float dt) {
    boost::shared_ptr<EffectElement> effect;
    for (int i = 0; i < 3; i++) {
        effect = m_entries[i].m_effect.lock();
        if (effect) {
            if (!effect->IsActive()) {
                m_entries[i].m_effect.reset();
            } else if (!m_entries[i].m_active) {
                effect->Stop();
                m_entries[i].m_effect.reset();
            } else {
                Update(i);
            }
            m_entries[i].m_active = false;
        }
    }
}

void RangeEffect::Start(const int &kind) {
    boost::shared_ptr<EffectElement> effect = m_entries[kind].m_effect.lock();
    if (!effect) {
        m_entries[kind].m_effect =
            fn_800A2754(reinterpret_cast<Player::Effect *>(m_player->field_0x23C),
                        s_rangeKinds[kind].m_effect);
        Update(kind);
    }
    m_entries[kind].m_active = true;
}

void RangeEffect::Update(int kind) {
    boost::shared_ptr<EffectElement> effect = m_entries[kind].m_effect.lock();
    if (effect) {
        fn_80425E68(effect.px,
                    fn_800AA760(m_player->m_motion, s_rangeKinds[kind].m_node)->m_matrix);
        if (m_player->IsWeaponHidden()) {
            effect->Hide();
        } else {
            effect->Show();
        }
    }
}

} // namespace

namespace Player {

/* The effect task's state (class name unknown; 0x40 bytes). */
class Effect::Impl {
  public:
    Impl(const boost::shared_ptr<PlayerBase> &player);
    ~Impl();

    void Create(Task *parent);
    Task *StartCameraDemo(Task *parent, int kind);
    void StartBelt(Task *parent, float width);
    void EndBelt();
    void StartJet(Task *parent);
    void EndJet();
    void StartVoiceJet(Task *parent);
    void EndVoiceJet();
    void StartGrind(Task *parent);
    void EndGrind();
    void SetGrindSparks(bool on);
    void StartDarkAwake(Task *parent);
    void StartHeroAwake(Task *parent);
    void StartChaosBlur();
    void EndChaosBlur();
    void StartSlow(Task *parent);
    void EndSlow();
    void Land(int kind);
    void LaunchGround(int kind, const Vec3f *position, const Vec3f &direction);
    void StartRange(const int &kind);

    void CancelBarrier() {
        if (m_barrier != 0) {
            m_barrier->Cancel();
        }
    }
    void OffBarrier() {
        if (m_barrier != 0) {
            m_barrier->Off();
        }
    }
    void SetBarrier(int kind) {
        if (m_barrier != 0) {
            switch (kind) {
            case 1:
            case 2:
            case 3:
            case 4:
                m_barrier->Set(kind);
                break;
            }
        }
    }
    void ReleaseAura() {
        m_auraCount--;
        if (m_auraCount < 0) {
            m_auraCount = 0;
        }
        if (m_auraCount == 0 && m_aura != 0) {
            fn_8028D834(m_aura);
        }
    }
    void StartAura(Task *parent) {
        if (m_aura == 0) {
            void *aura = operator new(0x30);
            if (aura != 0) {
                fn_8028D9D8(aura, parent, this, &m_aura);
            }
        }
        m_auraCount++;
    }
    void EndSuper() {
        if (m_super != 0) {
            m_super->Finish();
        }
    }
    void StartSuper(Task *parent) {
        if (m_super == 0) {
            new Shadow::Super(parent, m_player, &m_super);
        }
    }

    /* 0x00 */ boost::shared_ptr<PlayerBase> m_player;
    /* 0x08 */ Belt *m_belt;
    /* 0x0C */ Jet *m_voiceJet;
    /* 0x10 */ Jet *m_jet;
    /* 0x14 */ Grind *m_grind;
    /* 0x18 */ Awake *m_awake;
    /* 0x1C */ Shadow::Super *m_super;
    /* 0x20 */ s32 m_auraCount;
    /* 0x24 */ void *m_aura;
    /* 0x28 */ boost::weak_ptr<EffectElement> m_chaosEffect;
    /* 0x30 */ Slow *m_slow;
    /* 0x34 */ Footmark *m_footmark;
    /* 0x38 */ RangeEffect *m_range;
    /* 0x3C */ Barrier *m_barrier;
};
SIZE_ASSERT(Effect::Impl, 0x40);

Effect::Impl::Impl(const boost::shared_ptr<PlayerBase> &player)
    : m_player(player), m_belt(0), m_voiceJet(0), m_jet(0), m_grind(0), m_awake(0), m_super(0),
      m_auraCount(0), m_aura(0), m_slow(0), m_footmark(0), m_range(0), m_barrier(0) {}

Effect::Impl::~Impl() {
    if (m_voiceJet != 0) {
        m_voiceJet->m_flags |= 1;
    }
    if (m_belt != 0) {
        m_belt->m_flags |= 1;
    }
    if (m_footmark != 0) {
        m_footmark->m_flags |= 1;
    }
    if (m_awake != 0) {
        m_awake->Finish();
    }
    if (m_slow != 0) {
        m_slow->End();
    }
}

void Effect::Impl::Create(Task *parent) {
    new Footmark(parent, m_player, &m_footmark);
    new RangeEffect(parent, m_player, &m_range);
    m_barrier = new Barrier(parent, m_player);
}

Task *Effect::Impl::StartCameraDemo(Task *parent, int kind) {
    return new CameraDemo(parent, m_player, kind);
}

void Effect::Impl::StartBelt(Task *parent, float width) {
    if (m_belt == 0) {
        m_belt = new Belt(parent, m_player, width);
    }
}

void Effect::Impl::EndBelt() {
    if (Belt *belt = m_belt) {
        if (belt->m_belt != 0) {
            belt->m_belt->Finish();
            belt->m_belt = 0;
        }
        m_belt = 0;
    }
}

void Effect::Impl::StartVoiceJet(Task *parent) {
    if (m_voiceJet == 0) {
        new Jet(parent, m_player, &m_voiceJet, true);
    } else if (m_voiceJet->m_state > 100) {
        m_voiceJet->m_state = 0;
    }
}

void Effect::Impl::EndVoiceJet() {
    if (m_voiceJet != 0) {
        m_voiceJet->End();
    }
}

void Effect::Impl::StartJet(Task *parent) {
    if (m_jet == 0) {
        new Jet(parent, m_player, &m_jet, false);
    } else if (m_jet->m_state > 100) {
        m_jet->m_state = 0;
    }
}

void Effect::Impl::EndJet() {
    if (m_jet != 0) {
        m_jet->End();
    }
}

void Effect::Impl::StartGrind(Task *parent) {
    if (m_grind == 0) {
        new Grind(parent, m_player, &m_grind);
    }
}

void Effect::Impl::EndGrind() {
    if (m_grind != 0) {
        m_grind->End();
    }
}

void Effect::Impl::SetGrindSparks(bool on) {
    if (m_grind != 0) {
        m_grind->SetSparks(on);
    }
}

void Effect::Impl::StartHeroAwake(Task *parent) {
    if (m_awake != 0) {
        m_awake->Finish();
    }
    new Awake(parent, m_player, &m_awake, 0);
}

void Effect::Impl::StartDarkAwake(Task *parent) {
    if (m_awake != 0) {
        m_awake->Finish();
    }
    new Awake(parent, m_player, &m_awake, 1);
}

void Effect::Impl::StartChaosBlur() {
    UnkBlur_8042E0C4 blur;
    blur.m_on = true;
    blur.m_alpha = 0x80;
    blur.m_scale = 1.05f;
    fn_8042E0C4(fn_800A31F0(), &blur);
    m_chaosEffect = fn_800A2854(reinterpret_cast<Effect *>(m_player->field_0x23C), 1, 0x51);
}

} // namespace Player

namespace Sonicteam {
namespace System {

template <> class CreateStatic<Effect::PJS::EffectManagerPJS> {
  public:
    static Effect::PJS::EffectManagerPJS *Create() {
        static Effect::PJS::EffectManagerPJS s_instance;
        return &s_instance;
    }
};

} // namespace System
} // namespace Sonicteam

/* The PJS effect manager singleton's instance (Singleton<EffectManagerPJS>'s static member). */
extern Effect::PJS::EffectManagerPJS *lbl_805EE238;

/* The PJS effect manager (Sonicteam::System::Singleton<EffectManagerPJS>'s accessor). */
extern "C" Effect::PJS::EffectManagerPJS *fn_800A31F0(void) {
    if (lbl_805EE238 == 0) {
        lbl_805EE238 =
            Sonicteam::System::CreateStatic<Effect::PJS::EffectManagerPJS>::Create();
    }
    return lbl_805EE238;
}

namespace Player {

void Effect::Impl::EndChaosBlur() {
    UnkBlur_8042E0C4 blur;
    blur.m_on = false;
    blur.m_alpha = 0x80;
    blur.m_scale = 1.05f;
    fn_8042E0C4(fn_800A31F0(), &blur);
    if (!m_chaosEffect.expired()) {
        m_chaosEffect.lock()->Stop();
    }
    fn_801D3838(1);
}

void Effect::Impl::StartSlow(Task *parent) {
    if (m_slow != 0) {
        m_slow->End();
    }
    new Slow(parent, m_player, &m_slow);
}

void Effect::Impl::EndSlow() {
    if (m_slow != 0) {
        m_slow->End();
    }
}

void Effect::Impl::Land(int kind) {
    if (fn_8006CD78(m_player->m_stateBits, 2) && !m_player->IsWeaponHidden()) {
        UnkGround *ground;
        s32 sound = -1;
        ground = *reinterpret_cast<UnkGround **>(m_player.get()->field_0x278);
        switch (kind) {
        case 4:
        case 5:
            sound = s_groundEffects[ground->m_kind].m_landSound;
            break;
        case 6:
            sound = 0x3001;
            break;
        }
        if (sound >= 0) {
            fn_801D560C(fn_80053164(), sound & 0xFFFF, 0, 0);
        }
        if (s_groundEffects[ground->m_kind].m_landEffect >= 0) {
            fn_800A27D4(reinterpret_cast<Effect *>(m_player->field_0x23C), &ground->m_position,
                        ground->m_normal, s_groundEffects[ground->m_kind].m_landEffect);
        }
    }
}

void Effect::Impl::LaunchGround(int kind, const Vec3f *position, const Vec3f &direction) {
    s32 id = s_groundEffects[kind].m_runEffect;
    if (id >= 0) {
        fn_800A27D4(reinterpret_cast<Effect *>(m_player->field_0x23C), position, direction, id);
    }
}

void Effect::Impl::StartRange(const int &kind) {
    if (m_range != 0) {
        m_range->Start(kind);
    }
}

/* Deletes the effect task's state (name inferred; see checked_delete below). */
inline void DeleteEffectImpl(Effect::Impl *impl) {
    delete impl;
}

} // namespace Player

/* Retail keeps boost::checked_delete<Effect::Impl> out of line (0x800A2EA4) and ~Impl at its own
 * place (0x800A385C); this specialization calls an inline helper, which gives retail's code and
 * order (see docs/research-notes.md and Game/Rocket.cpp). */
namespace boost {
template <> inline void checked_delete<Player::Effect::Impl>(Player::Effect::Impl *x) {
    Player::DeleteEffectImpl(x);
}
} // namespace boost

namespace Player {

Effect::Effect(Task *parent, const boost::shared_ptr<PlayerBase> &player)
    : Task(parent), m_impl(new Impl(player)) {
    m_name = "Effect";
    m_impl->Create(this);
    m_impl->m_player->field_0x23C = reinterpret_cast<u32>(this);
}

Effect::~Effect() {
    m_impl->m_player->field_0x23C = 0;
}

void Effect::Tick(float dt) {
    if (PlayerMotion *motion = m_impl->m_player->m_motion) {
        fn_800AA5D4(motion, dt);
    }
}

} // namespace Player


extern "C" {

void fn_800A2D7C(Player::Effect *effect, int kind) {
    effect->m_impl->StartCameraDemo(effect, kind);
}

void fn_800A2D54(Player::Effect *effect, float width) {
    effect->m_impl->StartBelt(effect, width);
}

void fn_800A2D30(Player::Effect *effect) {
    effect->m_impl->EndBelt();
}

void fn_800A2D08(Player::Effect *effect) {
    effect->m_impl->StartVoiceJet(effect);
}

void fn_800A2CE4(Player::Effect *effect) {
    effect->m_impl->EndVoiceJet();
}

void fn_800A2CBC(Player::Effect *effect) {
    effect->m_impl->StartJet(effect);
}

void fn_800A2C98(Player::Effect *effect) {
    effect->m_impl->EndJet();
}

void fn_800A2C70(Player::Effect *effect) {
    effect->m_impl->StartGrind(effect);
}

void fn_800A2C4C(Player::Effect *effect) {
    effect->m_impl->EndGrind();
}

void fn_800A2C28(Player::Effect *effect, bool sparks) {
    effect->m_impl->SetGrindSparks(sparks);
}

void fn_800A2C00(Player::Effect *effect) {
    effect->m_impl->StartHeroAwake(effect);
}

void fn_800A2BD8(Player::Effect *effect) {
    effect->m_impl->StartDarkAwake(effect);
}

void fn_800A2B78(Player::Effect *effect) {
    effect->m_impl->StartSuper(effect);
}

void fn_800A2B44(Player::Effect *effect) {
    effect->m_impl->EndSuper();
}

void fn_800A2AD8(Player::Effect *effect) {
    effect->m_impl->StartAura(effect);
}

void fn_800A2A78(Player::Effect *effect) {
    effect->m_impl->ReleaseAura();
}

void fn_800A2A54(Player::Effect *effect) {
    effect->m_impl->StartChaosBlur();
}

void fn_800A2A30(Player::Effect *effect) {
    effect->m_impl->EndChaosBlur();
}

void fn_800A2A08(Player::Effect *effect) {
    effect->m_impl->StartSlow(effect);
}

void fn_800A29E4(Player::Effect *effect) {
    effect->m_impl->EndSlow();
}

void fn_800A29C0(Player::Effect *effect, int kind) {
    effect->m_impl->Land(kind);
}

void fn_800A299C(Player::Effect *effect, int kind, const Vec3f *position, const Vec3f &direction) {
    effect->m_impl->LaunchGround(kind, position, direction);
}

void fn_800A2978(Player::Effect *effect, const int &kind) {
    effect->m_impl->StartRange(kind);
}

void fn_800A2920(Player::Effect *effect, int kind) {
    effect->m_impl->SetBarrier(kind);
}

void fn_800A28EC(Player::Effect *effect) {
    effect->m_impl->OffBarrier();
}

void fn_800A28B0(Player::Effect *effect) {
    effect->m_impl->CancelBarrier();
}

boost::weak_ptr<EffectElement> fn_800A2854(Player::Effect *effect, int node, s32 id) {
    return fn_800A2814(effect, fn_800AA760(effect->m_impl->m_player->m_motion, node)->m_matrix, id);
}

boost::weak_ptr<EffectElement> fn_800A2814(Player::Effect *effect, const Mtx34f *matrix, s32 id) {
    return fn_801D2C28(0, matrix, id, &Time(effect->m_impl->m_player.get())->m_rawDt);
}

boost::weak_ptr<EffectElement> fn_800A27D4(Player::Effect *effect, const Vec3f *position,
                                           const Vec3f &direction, s32 id) {
    return fn_801D2AFC(0, position, direction, id, &Time(effect->m_impl->m_player.get())->m_rawDt);
}

boost::weak_ptr<EffectElement> fn_800A2794(Player::Effect *effect, const Vec3f *position,
                                           const Vec2f &angles, s32 id) {
    return fn_801D2B38(0, position, angles, id, &Time(effect->m_impl->m_player.get())->m_rawDt);
}

boost::weak_ptr<EffectElement> fn_800A2754(Player::Effect *effect, s32 id) {
    return fn_801D2BB0(0, id, &Time(effect->m_impl->m_player.get())->m_rawDt);
}

/* Sets entry index of the ground table, and puts it back to its default. */
void fn_800A2730(int index, s32 landEffect, s32 footEffect, s32 runEffect, s32 landSound) {
    s_groundEffects[index].m_landEffect = landEffect;
    s_groundEffects[index].m_footEffect = footEffect;
    s_groundEffects[index].m_runEffect = runEffect;
    s_groundEffects[index].m_landSound = landSound;
}

void fn_800A26F0(int index) {
    s_groundEffects[index] = s_defaultGroundEffects[index];
}

} // extern "C"

/* An object of 8 bytes that another unit keeps as a function-local static (class name unknown). */
class UnkPair_800A26E0 {
  public:
    UnkPair_800A26E0();
    ~UnkPair_800A26E0();

    /* 0x0 */ u32 field_0x0;
    /* 0x4 */ u32 field_0x4;
};

UnkPair_800A26E0::UnkPair_800A26E0() {
    field_0x0 = 0;
    field_0x4 = 0;
}

UnkPair_800A26E0::~UnkPair_800A26E0() {}

namespace Player {

/* What the player keeps at 0x244 (class name unknown). */
struct UnkData_800A267C {
    ~UnkData_800A267C() {}

    /* 0x0 */ u32 field_0x0;
};

class UnkPlayerData_800A25E0 {
  public:
    ~UnkPlayerData_800A25E0();

    /* 0x00 */ u8 field_0x0[0x10];
    /* 0x10 */ boost::scoped_ptr<UnkData_800A267C> m_data;
};

UnkPlayerData_800A25E0::~UnkPlayerData_800A25E0() {}

} // namespace Player
