/* Boss::Diablon::Behavior::Event (class name from RTTI): the Diablon behavior that plays one of the
 * boss's three events (InitializeArgsEvent). After the event's delay it places the boss, a second
 * character (Diablon field 0x78, which the second event sends down with a ToUnderCommand) and the
 * player, has the parts perform the event's action and plays the event's camera path on a user
 * camera mode (CameraModeUser) with hint messages; when the path ends it switches to the event's
 * next behavior. File name inferred from the class. Names marked "(name inferred)" are not original.
 * Definitions are in reverse address order (deferred inlining emits functions last-defined
 * first). */
/* Keeps the dead copy of bad_alloc's vtable in .data, as retail has it (see <new>). */
#define MSL_NEW_INLINE_BAD_ALLOC
#include <boost/bind.hpp>
#include <boost/function.hpp>

#include "Game/CameraModeUser.h"
#include "Game/CharCommand.h"
#include "Game/CharInfo.h"
#include "Game/CharaColliBaseCharInfo.h"
#include "Game/Diablon.h"
#include "Game/ExternalCameraRequest.h"
#include "Game/MathConstants.h"
#include "Game/MathTypes.h"
#include "Game/Vibration.h"
#include "boost/shared_ptr.hpp"
#include "boost/weak_ptr.hpp"
#include <dolphin/types.h>
#include "size_assert.h"

using SonicteamUSA::System::Camera::CameraMode;
using SonicteamUSA::System::Camera::CameraModeUser;

/* The hint manager (class name unknown; see HintRing.cpp and EnemyHint.cpp). */
class UnkHintManager_801FB180;
UnkHintManager_801FB180 *GetHintManager();

class UnkObject_80074470;

/* Converts degrees to radians (0x8000AB90). */
float DegToRad(float degrees);

extern "C" {
/* A 32-bit flag word: fn_80014E04 clears it, fn_80014E10 clears every bit, fn_80014E1C sets bit
 * and fn_80014E34 tests bit (see EggLastMech.h). */
void fn_80014E04(u32 *flags);
void fn_80014E10(u32 *flags);
void fn_80014E1C(u32 *flags, int bit);
bool fn_80014E34(u32 *flags, int bit);

/* The hint manager showing message id of branch, and two calls around it (see HintRing.cpp and
 * BlackBull0210ModeCtrlUnit.cpp). */
s32 fn_801FAF00(UnkHintManager_801FB180 *manager, s32 id, s32 branch, int arg3, int arg4);
void fn_801FAAA4(UnkHintManager_801FB180 *manager);
void fn_801FAAB4(UnkHintManager_801FB180 *manager);
/* Loads a 2D layout and plays one of its animations (behaviour inferred). */
void fn_8035367C(const char *file, const char *layout, const char *animation);
/* Starts a controller vibration (see Game/Vibration.h). */
boost::weak_ptr<Vibration::RequestHandle> fn_8034DF48(int pattern, int arg1);
/* The music player and a request to it (behaviour inferred). */
UnkObject_80074470 *fn_80074470(void);
void fn_80175EEC(UnkObject_80074470 *object, int arg1, int arg2);

/* Places a character with a position and Euler angles (behaviour inferred). */
void fn_800B8000(const boost::weak_ptr<CharInfo> &chara, const Vec3f &position,
                 const Vec3f &rotation);

/* The resource loader, a camera path of a resource slot and moving a path's time on (see
 * EggLastMechCamera.cpp). */
void *fn_800122D4(void);
UnkCameraData_80012510 *fn_80012510(void *loader, int slot, int index);
int fn_80011B7C(UnkCameraData_80012510 *data, float *time, const float *step);
void fn_8000FBFC(const boost::shared_ptr<CameraMode> &mode, UnkCameraData_80012510 *data, float time,
                 const Mtx34f *matrix);
}

extern const Vec3f lbl_805E07B8;

/* Converts degrees to radians (name inferred; retail has this inline besides DegToRad). */
inline float ToRadians(float degrees) {
    return degrees * (lbl_805FAD38 / 180.0f);
}

namespace Boss {
namespace Diablon {

/* ToUnderCommand (class name from RTTI): sent to the second character as the second event starts
 * (command type 0x501). */
class ToUnderCommand : public CharCommand {
  public:
    ToUnderCommand() : CharCommand(CommandType(0x501), 4) {}

    /* The command type as CharCommand stores it. Calling this inline function keeps the
     * constructor out of line, as the game has it (as Game/RailSwitch.cpp does). */
    static u16 CommandType(int type) {
        return type;
    }
};
SIZE_ASSERT(ToUnderCommand, 0x14);

namespace Behavior {

/* A vector of the table below (struct name inferred; see s_events). */
struct EventVector {
    float x;
    float y;
    float z;
};

/* An event of the table below (struct name inferred). */
struct EventInfo {
    /* 0x00 */ float m_delay;
    /* 0x04 */ EventVector m_bossPosition;
    /* 0x10 */ EventVector m_otherPosition; /* the second character's */
    /* 0x1C */ EventVector m_playerPosition;
    /* 0x28 */ s32 m_action; /* the parts action */
    /* 0x2C */ s32 m_camera; /* the camera path; none when negative */
    /* 0x30 */ s32 m_next;   /* the behavior that follows; none when 0 */
};
SIZE_ASSERT(EventInfo, 0x34);

/* Not original: retail initializes the table's scalars statically and its vectors at startup, as
 * a compiler that accepts Vec3f(...) members in an aggregate initializer does; GC/2.7 rejects
 * those, and initializing the coordinates through this inline function gives the same startup
 * code (name inferred). */
inline float Coord(float value) {
    return value;
}

/* The boss's events (name inferred). */
static const EventInfo s_events[3] = {
    {0.0f,
     {Coord(0.0f), Coord(0.0f), Coord(0.0f)},
     {Coord(0.0f), Coord(5.0f), Coord(60.0f)},
     {Coord(0.0f), Coord(5.0f), Coord(260.0f)},
     15,
     15,
     1},
    {0.5f,
     {Coord(0.0f), Coord(0.0f), Coord(0.0f)},
     {Coord(-50.0f), Coord(5.0f), Coord(0.0f)},
     {Coord(0.0f), Coord(5.0f), Coord(260.0f)},
     16,
     13,
     1},
    {1.0f,
     {Coord(0.0f), Coord(0.0f), Coord(0.0f)},
     {Coord(50.0f), Coord(5.0f), Coord(0.0f)},
     {Coord(0.0f), Coord(5.0f), Coord(260.0f)},
     17,
     14,
     0},
};

class Event : public BehaviorBase {
  public:
    Event(const boost::shared_ptr<Diablon> &owner);
    virtual ~Event();
    virtual void Begin(int previous, const InitializeArgs &args);
    virtual void End(int next);
    virtual void Tick(float dt);

    /* Whether the second character is still there (name inferred). */
    bool IsOtherAlive();
    /* The hints and requests made as the event starts and ends (names inferred). */
    void StartEvent();
    void EndEvent();
    /* Starts the event's camera path (name inferred). */
    void StartCamera();
    void OnSwitch(CameraModeUser::SwitchEventArgs &args);
    void OnUpdate(CameraModeUser::UpdateEventArgs &args);
    /* Places the camera mode on the path (name inferred). */
    void Apply();
    /* Places the boss, the second character and the player (name inferred). */
    void PlaceCharacters();

    static boost::shared_ptr<BehaviorBase> Instantiate(const boost::shared_ptr<Diablon> &owner);
    static void Setup(void *executer);
    static void Teardown(void *executer);

    /* 0x14 */ boost::weak_ptr<PJSCamera::ExternalCameraRequest> m_handle;
    /* 0x1C */ boost::shared_ptr<CameraModeUser> m_mode;
    /* 0x24 */ UnkCameraData_80012510 *m_data;
    /* 0x28 */ float m_time;
    /* 0x2C */ float m_step;
    /* 0x30 */ u32 m_flags; /* 0 the event started, 1 the end request was made */
    /* 0x34 */ s32 m_event;
    /* 0x38 */ float m_delay;
    /* 0x3C */ s32 m_state;
};
SIZE_ASSERT(Event, 0x40);

Event::Event(const boost::shared_ptr<Diablon> &owner) : BehaviorBase(owner, "Event", 6) {
    fn_80014E04(&m_flags);
    m_data = 0;
}

Event::~Event() {}

void Event::Begin(int previous, const InitializeArgs &args) {
    m_owner->m_flags.Set(7);
    m_owner->m_flags.Set(8);
    m_event = static_cast<const InitializeArgsEvent &>(args).m_event;
    fn_80014E10(&m_flags);
    StartEvent();
    m_delay = s_events[m_event].m_delay;
    m_state = 0;
}

void Event::End(int next) {
    m_owner->m_flags.Reset(7);
    m_owner->m_flags.Reset(8);
    fn_801CEAD4(&m_handle);
    if (fn_80014E34(&m_flags, 0)) {
        fn_80175EEC(fn_80074470(), 3, 0);
    }
}

void Event::Tick(float dt) {
    switch (m_state) {
    case 0:
        if (IsOtherAlive()) {
            m_state++;
        }
        break;
    case 1:
        m_delay -= dt;
        if (m_delay <= 0.0f) {
            fn_80175EEC(fn_80074470(), 4, 0);
            fn_80014E1C(&m_flags, 0);
            PlaceCharacters();
            fn_802D9E4C(m_owner->m_executer, Parts::Action(s_events[m_event].m_action));
            StartCamera();
            m_state++;
        }
        break;
    case 2:
        m_step = 60.0f * dt;
        if (m_data != 0 && !fn_80014E34(&m_flags, 1) && m_event == 2 &&
            m_time >= m_data->m_endTime - 60.0f) {
            fn_80014E1C(&m_flags, 1);
            fn_80175EEC(fn_80074470(), 6, 0);
        }
        if (m_data == 0 || fn_80011B7C(m_data, &m_time, &m_step) != 0) {
            EndEvent();
            if (s_events[m_event].m_next != 0) {
                fn_802D9ED8(m_owner->m_executer, InitializeArgs(s_events[m_event].m_next));
            }
        }
        break;
    }
}

bool Event::IsOtherAlive() {
    return !m_owner->field_0x78.expired();
}


void Event::StartEvent() {
    UnkHintManager_801FB180 *hint = GetHintManager();
    switch (m_event) {
    case 0:
        fn_8035367C("CSDFiles/BChara_SandD", "bchara", "open");
        fn_801FAF00(hint, m_owner->m_params->m_eventStartHintId, 0, 0, 0);
        break;
    case 1: {
        m_owner->m_flags.Set(0);
        boost::shared_ptr<CharInfo> other = m_owner->field_0x78.lock();
        if (other.get() != 0) {
            other->PushCommand(new ToUnderCommand());
        }
        break;
    }
    case 2:
        if (fn_80014E34(&m_flags, 0)) {
            fn_801FAAA4(hint);
        }
        fn_801FAF00(hint, m_owner->m_params->m_eventStartHintId2[0], 0, 1, 0);
        fn_801FAF00(hint, m_owner->m_params->m_eventStartHintId2[1], 0, 0, 0);
        if (fn_80014E34(&m_flags, 0)) {
            fn_801FAAB4(hint);
        }
        fn_802D9E4C(m_owner->m_executer, Parts::Action(12));
        fn_8034DF48(7, -2);
        break;
    }
}

void Event::EndEvent() {
    UnkHintManager_801FB180 *hint = GetHintManager();
    switch (m_event) {
    case 0:
        break;
    case 1:
        if (fn_80014E34(&m_flags, 0)) {
            fn_801FAAA4(hint);
        }
        fn_801FAF00(hint, m_owner->m_params->m_eventEndHintId1[0], 0, 1, 0);
        fn_801FAF00(hint, m_owner->m_params->m_eventEndHintId1[1], 0, 0, 0);
        if (fn_80014E34(&m_flags, 0)) {
            fn_801FAAB4(hint);
        }
        break;
    case 2:
        if (!fn_80014E34(&m_flags, 1)) {
            fn_80014E1C(&m_flags, 1);
            fn_80175EEC(fn_80074470(), 6, 0);
        }
        break;
    }
}

void Event::StartCamera() {
    void *loader = fn_800122D4();
    int slot = m_owner->m_cameraSlot;
    int camera = s_events[m_event].m_camera;
    if (camera >= 0) {
        m_data = fn_80012510(loader, slot, camera);
        m_mode = fn_80009704(0, 0);
        if (m_mode.get() != 0) {
            fn_800A6A24(&m_mode.get()->m_onSwitch,
                        &(const CameraModeUser::SwitchCallback &)CameraModeUser::SwitchCallback(
                            boost::bind(&Event::OnSwitch, this, _1)));
            fn_800A673C(&m_mode.get()->m_onUpdate,
                        &(const CameraModeUser::UpdateCallback &)CameraModeUser::UpdateCallback(
                            boost::bind(&Event::OnUpdate, this, _1)));
        }
        if (m_data != 0) {
            m_time = m_data->m_startTime;
        }
        m_handle = fn_801CED24(boost::shared_ptr<CameraMode>(m_mode), 0, 0, 0, 0, 0, 1);
    }
}

void Event::OnSwitch(CameraModeUser::SwitchEventArgs &args) {
    Apply();
}

void Event::OnUpdate(CameraModeUser::UpdateEventArgs &args) {
    Apply();
}

void Event::Apply() {
    if (m_data != 0) {
        Mtx34f matrix;
        Mtx34f translation;
        SetTranslationMtx(&translation, m_owner->m_position);
        fn_8040C148(&matrix, &translation);
        fn_8040B848(&matrix, m_owner->m_rotation.y);
        fn_8000FBFC(boost::shared_ptr<CameraMode>(m_mode), m_data, m_time, &matrix);
    }
}

void Event::PlaceCharacters() {
    const EventInfo &info = s_events[m_event];
    m_owner->m_position.x = info.m_bossPosition.x;
    m_owner->m_position.y = info.m_bossPosition.y;
    m_owner->m_position.z = info.m_bossPosition.z;
    m_owner->m_rotation = Vec3f(DegToRad(0.0f), DegToRad(180.0f), DegToRad(0.0f));
    boost::weak_ptr<CharInfo> other(m_owner->field_0x78);
    if (!other.expired()) {
        fn_800B8000(other,
                    Vec3f(info.m_otherPosition.x, info.m_otherPosition.y, info.m_otherPosition.z),
                    Vec3f(ToRadians(0.0f), ToRadians(180.0f), ToRadians(0.0f)));
    }
    boost::weak_ptr<CharInfo> player(m_owner->m_target);
    if (!player.expired()) {
        fn_800B8000(player,
                    Vec3f(info.m_playerPosition.x, info.m_playerPosition.y,
                          info.m_playerPosition.z),
                    lbl_805E07B8);
    }
}

boost::shared_ptr<BehaviorBase> Event::Instantiate(const boost::shared_ptr<Diablon> &owner) {
    return boost::shared_ptr<BehaviorBase>(new Event(owner));
}

void Event::Setup(void *executer) {}

void Event::Teardown(void *executer) {}

/* The behavior table's entry (name inferred). */
extern const Info g_eventInfo;
const Info g_eventInfo = {Event::Instantiate, Event::Setup, Event::Teardown};

} // namespace Behavior
} // namespace Diablon
} // namespace Boss
