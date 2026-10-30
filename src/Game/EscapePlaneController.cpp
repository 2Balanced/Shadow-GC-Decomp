/* EscapePlaneController::Controller (class name from RTTI; its s_taskName is
 * "EscapePlane::Controller"): the task that flies the escape plane (EscapePlane.cpp, a PathObject)
 * along eleven paths, as EggBalloonController.cpp drives the egg balloon. It has a character info of
 * its own (id 0x80), through which E_P_COLI boxes (EscapePlaneColi.cpp) command it:
 * GadgetCommand::MoveStartCommand and MoveEndCommand start and stop it, SwitchCommand switches a
 * path to its alternative (RailSwitch.cpp also sets them), AccelerateCommand changes its speed and
 * SetPositionCommand moves it on to a later path. Once the plane's resources are loaded it starts
 * the plane and moves it each frame, shifted by a fixed offset; the plane turns smoothly when a
 * path changes its heading by more than five degrees. When the plane is destroyed, or at the end of
 * the last path, it notifies the player. File name from the class; the other names are inferred.
 * Definitions are in reverse address order (deferred inlining emits functions last-defined
 * first). */
#include <boost/bind.hpp>
#include <boost/function.hpp>

#include "Game/AngleUtil.h"
#include "Game/CharCommand.h"
#include "Game/CharCommandUnit.h"
#include "Game/CharInfo.h"
#include "Game/GadgetCharCommands.h"
#include "Game/GadgetCharInfo.h"
#include "Game/GadgetCommand.h"
#include "Game/MathTypes.h"
#include "Game/Mission.h"
#include "Game/PathCtrl.h"
#include "Game/PathObject.h"
#include "Game/ResourceOneFile.h"
#include "Game/Task.h"
#include "boost/shared_ptr.hpp"
#include "boost/weak_ptr.hpp"
#include "size_assert.h"

using SonicteamUSA::System::PathCtrl;

/* boost/shared_ptr.hpp only declares shared_ptr::reset, which the game has out of line; this unit,
 * the first user, emits the copy for PathObject that the others call (boost 1.32's definition). */
namespace boost {
template <> inline void shared_ptr<PathObject>::reset() {
    this_type().swap(*this);
}
} // namespace boost

/* SwitchCommand (command type 9) is declared in Game/CharCommand.h. */

class UnkPlayerManager_8006D6A8;

/* The game state (see BKTankController.cpp); the float at 0x20 is the frame time (meaning
 * inferred). */
struct UnkGameState_8057D6A0 {
    /* 0x000 */ u8 field_0x0[0x20];
    /* 0x020 */ float m_dt;
    /* 0x024 */ u8 field_0x24[0x150 - 0x24];
};
SIZE_ASSERT(UnkGameState_8057D6A0, 0x150);

/* The character manager (see Monster.cpp). */
class UnkCharManager_800DDF20;

/* The path manager fn_8001B494 returns (class name unknown). */
class UnkPathManager_8001B494;

/* MSL's std::fabs for floats, which the game calls out of line (as in BkSoldier.cpp). */
namespace std {
float fabs(float x);
}

extern "C" {
extern UnkGameState_8057D6A0 lbl_8057D6A0;

/* A 32-bit flag word (see BASwitch.cpp). */
void fn_80014E04(u32 *flags);
void fn_80014E10(u32 *flags);
void fn_80014E1C(u32 *flags, int bit);
void fn_8006127C(u32 *flags, int bit);
bool fn_80014E34(u32 *flags, int bit);

UnkPlayerManager_8006D6A8 *fn_8006D6A8(void);
boost::weak_ptr<Mission::Mission> fn_800740E4(UnkPlayerManager_8006D6A8 *manager, int index);
/* Tells the player that the plane is through (behaviour inferred). */
void fn_8016B270(Mission::Mission *player);

/* The state of resource slot slot (3: loaded; see EffectPJS.cpp), and whether the stage has
 * started (behaviour inferred). */
s32 fn_80012408(void *loader, s32 slot);
u8 fn_80177CF8(void);

/* The sound manager, and sounds played at a character (behaviour inferred; the other arguments are
 * unknown). */
void *fn_80053164(void);
void fn_801D53C4(void *manager, int id, CharInfo *character, void *owner, int arg4, int arg5, int arg6);
void fn_801D54F4(void *manager, int id, CharInfo *character, int arg3, int arg4);

/* The character manager and character info helpers (see BKTankController.cpp). */
UnkCharManager_800DDF20 *fn_800DDF20(void);
void fn_801DCD8C(UnkCharManager_800DDF20 *manager, const boost::shared_ptr<CharInfo> &info);
void fn_801DCCF4(UnkCharManager_800DDF20 *manager, const boost::shared_ptr<CharInfo> &info);
boost::shared_ptr<GadgetCharInfo> fn_8015CD90(int arg0, int arg1);
void fn_8015CF7C(GadgetCharInfo *charInfo);
/* While it returns true, gadgets stand still (see EggBalloon.cpp). */
bool fn_80169CD0(void);

/* Path control (see BKTankController.cpp): places position and rotation distance along the path. */
UnkPathManager_8001B494 *fn_8001B494(void);
bool fn_8001C7B4(UnkPathManager_8001B494 *manager, PathCtrl *path, s32 id);
float fn_800183D8(PathCtrl *path);
void fn_80018304(PathCtrl *path);
bool fn_8001C91C(UnkPathManager_8001B494 *manager, PathCtrl *path, int arg2, Vec3f *position,
                 Vec3f *rotation, int arg5, float distance);

/* vector += add (behaviour inferred). */
void fn_8040CFF8(Vec3f *vector, const Vec3f *add);
}

/* Degrees to radians (name inferred). */
inline float DegToRad(float degrees) {
    return degrees * (lbl_805FAD38 / 180.0f);
}

/* vector += add (name inferred). */
inline void AddVector(Vec3f &vector, const Vec3f &add) {
    fn_8040CFF8(&vector, &add);
}

/* The eleven paths the plane follows in turn: path id, the id of its alternative, whether the
 * alternative is taken, and whether the plane turns smoothly onto it (struct name inferred; as in
 * BKTankController.cpp). RailSwitch.cpp reads it by this name. */
struct UnkPlanePath_8054B8E0 {
    s32 m_id;
    s32 m_altId;
    s32 m_useAlt;
    u8 field_0xC;
    u8 m_turn;
};

UnkPlanePath_8054B8E0 lbl_8054B8E0[11] = {
    {1, 1, 0, 1, 0},   {2, 2, 0, 1, 0},   {4, 3, 0, 0, 1},   {5, 5, 0, 1, 1},
    {6, 6, 0, 1, 0},   {7, 7, 0, 1, 0},   {8, 9, 0, 0, 1},   {10, 10, 0, 1, 1},
    {11, 11, 0, 1, 0}, {12, 12, 0, 1, 0}, {13, 13, 0, 1, 0},
};

namespace EscapePlaneController {

/* Turns every path's alternative off (name inferred). */
inline void ClearAlternatives() {
    for (int i = 0; i < 11; i++) {
        lbl_8054B8E0[i].m_useAlt = 0;
    }
}

/* How far the plane is turned from its path's direction, how far it sits from its path, and the
 * heading change it turns onto smoothly (names inferred). The static initializer stores them, in
 * this order, as in retail. */
static const float s_turn = DegToRad(180.0f);
static Vec3f s_offset(0.186f, -59.141f, 6.969f);
static const float s_turnLimit = DegToRad(5.0f);

/* The flag word at 0x44 of the controller and 0x70 of its path (struct name unknown). */
struct ControllerFlags {
    ControllerFlags() {
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

/* The plane's position on its paths (class name inferred): the current path and the distance along
 * it, and a speed that moves toward a target speed. */
class PlanePath {
  public:
    PlanePath(const boost::weak_ptr<CharInfo> &target);
    /* Sets path m_index up; false if there is none. */
    bool SelectPath();
    /* Moves target along the path by a frame; true at the end of the path. */
    bool Move(const boost::weak_ptr<CharInfo> &target);
    /* Goes on to the next path; false after the last. */
    bool Next(const boost::weak_ptr<CharInfo> &target);
    /* Puts the plane on path index, rate (0 to 1) of the way along it, unless it is past it. */
    void Set(int index, float rate);

    /* 0x00 */ PathCtrl m_ctrl;
    /* 0x58 */ s32 m_index;
    /* 0x5C */ float m_distance;
    /* 0x60 */ float m_speed;
    /* 0x64 */ float m_targetSpeed;
    /* 0x68 */ float field_0x68;
    /* 0x6C */ float m_yaw; /* the heading the plane turns to */
    /* 0x70 */ ControllerFlags m_flags; /* bit 1: turning; bit 2: just placed */
};
SIZE_ASSERT(PlanePath, 0x74);

class Controller : public Task {
  public:
    Controller(Task *parent, boost::shared_ptr<PathObject> object);
    virtual ~Controller();
    virtual void Tick(float dt);
    /* The command callback of the controller's character info. */
    void OnCommand(CharCommand &command);

    /* 0x28 */ boost::shared_ptr<GadgetCharInfo> m_chara;
    /* 0x30 */ boost::shared_ptr<PathObject> m_object;
    /* 0x38 */ PlanePath *m_path;
    /* 0x3C */ s32 m_state;
    /* 0x40 */ float m_timer; /* the plane waits while it runs */
    /* 0x44 */ ControllerFlags m_flags; /* bit 0: stopped */

    static const char *s_taskName;
};
SIZE_ASSERT(Controller, 0x48);

PlanePath::PlanePath(const boost::weak_ptr<CharInfo> &target)
    : m_index(0), m_distance(0.0f), m_speed(2.0f), m_targetSpeed(2.0f), field_0x68(0.0f), m_yaw(0.0f) {
    ClearAlternatives();
    SelectPath();
    if (boost::shared_ptr<CharInfo> character = target.lock()) {
        CharInfo *info = character.get();
        fn_8001C91C(fn_8001B494(), &m_ctrl, 0, &info->m_position, &info->m_rotation, 0, 0.0f);
        character->m_rotation.y += s_turn;
    }
    m_flags.Clear(1);
}

bool PlanePath::SelectPath() {
    m_targetSpeed = 2.0f;
    m_speed = 2.0f;
    UnkPlanePath_8054B8E0 *entry = &lbl_8054B8E0[m_index];
    s32 id;
    if (entry->m_useAlt == 1) {
        id = entry->m_altId;
    } else {
        id = entry->m_id;
    }
    if (fn_8001C7B4(fn_8001B494(), &m_ctrl, id)) {
        m_distance = 0.0f;
        fn_80018304(&m_ctrl);
        Vec3f position;
        Vec3f rotation(0.0f, 0.0f, 0.0f);
        fn_8001C91C(fn_8001B494(), &m_ctrl, 0, &position, &rotation, 0, m_distance);
        m_yaw = rotation.y + s_turn;
        return true;
    }
    return false;
}

bool PlanePath::Move(const boost::weak_ptr<CharInfo> &target) {
    if (boost::shared_ptr<CharInfo> character = target.lock()) {
        if (m_flags.Test(2)) {
            fn_80018304(&m_ctrl);
            CharInfo *info = character.get();
            fn_8001C91C(fn_8001B494(), &m_ctrl, 0, &info->m_position, &info->m_rotation, 0, m_distance);
            character->m_rotation.y += s_turn;
            m_yaw = character->m_rotation.y;
            m_flags.Clear(2);
        } else if (m_flags.Test(1) && m_yaw != character->m_rotation.y) {
            CharInfo *info = character.get();
            info->m_rotation.y = TurnAngleToward(m_yaw, info->m_rotation.y, DegToRad(0.5f));
            if (m_yaw == character->m_rotation.y) {
                m_flags.Clear(1);
            }
            Vec3f rotation(0.0f, 0.0f, 0.0f);
            CharInfo *mover = character.get();
            return fn_8001C91C(fn_8001B494(), &m_ctrl, 0, &mover->m_position, &rotation, 0, 0.0f);
        } else {
            if (!fn_80169CD0()) {
                if (m_targetSpeed > m_speed) {
                    m_speed = m_speed + 5.0f * lbl_8057D6A0.m_dt;
                    if (m_speed > m_targetSpeed) {
                        m_speed = m_targetSpeed;
                    }
                } else if (m_targetSpeed < m_speed) {
                    m_speed = m_speed - 5.0f * lbl_8057D6A0.m_dt;
                    if (m_speed < m_targetSpeed) {
                        m_speed = m_targetSpeed;
                    }
                }
                m_distance += lbl_8057D6A0.m_dt * (10.0f * m_speed);
            }
            fn_80018304(&m_ctrl);
            Vec3f rotation(0.0f, 0.0f, 0.0f);
            CharInfo *info = character.get();
            bool done = fn_8001C91C(fn_8001B494(), &m_ctrl, 0, &info->m_position, &rotation, 0, m_distance);
            rotation.y += s_turn;
            if (std::fabs(rotation.y - character->m_rotation.y) >= s_turnLimit) {
                m_flags.Set(1);
                m_yaw = rotation.y;
                fn_801D54F4(fn_80053164(), 0x782F, character.get(), 0, 1);
            } else {
                character->m_rotation = rotation;
                fn_801D53C4(fn_80053164(), 0x782E, character.get(), this, 3, 0, 2);
            }
            return done;
        }
    }
    return false;
}

bool PlanePath::Next(const boost::weak_ptr<CharInfo> &target) {
    ++m_index;
    if (m_index >= 11) {
        return false;
    }
    float speed = m_speed;
    float targetSpeed = m_targetSpeed;
    bool result = SelectPath();
    m_speed = speed;
    m_targetSpeed = targetSpeed;
    if (lbl_8054B8E0[m_index].m_turn != 0) {
        m_flags.Set(1);
    } else {
        m_flags.Clear(1);
        if (boost::shared_ptr<CharInfo> character = target.lock()) {
            CharInfo *info = character.get();
            fn_8001C91C(fn_8001B494(), &m_ctrl, 0, &info->m_position, &info->m_rotation, 0, 0.0f);
            character->m_rotation.y += s_turn;
        }
    }
    return result;
}

void PlanePath::Set(int index, float rate) {
    if (index >= m_index) {
        m_index = index;
        SelectPath();
        if (0.0f <= rate && rate <= 1.0f) {
            m_distance = rate * fn_800183D8(&m_ctrl);
        }
        m_flags.Set(2);
    }
}

const char *Controller::s_taskName = "EscapePlane::Controller";

Controller::Controller(Task *parent, boost::shared_ptr<PathObject> object) : Task(parent) {
    m_name = s_taskName;
    m_chara = fn_8015CD90(0x80, 4);
    fn_801DCD8C(fn_800DDF20(), boost::shared_ptr<CharInfo>(m_chara));
    fn_8015CF7C(m_chara.get());
    fn_800729B0(&m_chara->m_commandUnit->m_callback,
                &(const CharCommandCallback &)CharCommandCallback(
                    boost::bind(&Controller::OnCommand, this, _1)));
    m_object = object;
    m_state = 0;
    m_timer = 0.0f;
    m_path = 0;
    m_flags.Clear();
}

Controller::~Controller() {
    fn_801DCCF4(fn_800DDF20(), boost::shared_ptr<CharInfo>(m_chara));
    if (m_path != 0) {
        delete m_path;
        m_path = 0;
    }
    if (m_object.get() != 0) {
        m_object->vf_0x14();
        m_object.reset();
    }
}

} // namespace EscapePlaneController

/* Calls the plane's vf_0x28 and starts the controller (EscapePlane.cpp calls it by this name). */
extern "C" void fn_8025F054(EscapePlaneController::Controller *controller) {
    if (controller->m_object.get() != 0) {
        controller->m_object->vf_0x28();
    }
    controller->m_state = 1;
}

namespace EscapePlaneController {

void Controller::Tick(float dt) {
    m_timer -= dt;
    if (m_timer < 0.0f) {
        m_timer = 0.0f;
    }
    switch (m_state) {
    case 0:
        if (fn_80012408(fn_800122D4(), 1) == 3 && fn_80177CF8() == 1) {
            m_object->Init();
            m_state = 1;
            boost::weak_ptr<CharInfo> info = m_object->GetCharInfo();
            m_path = new PlanePath(info);
            m_path->Move(info);
            if (boost::shared_ptr<CharInfo> character = info.lock()) {
                AddVector(character->m_position, s_offset);
                character->m_rotation.x = 0.0f;
                character->m_rotation.z = 0.0f;
            }
            m_object->UpdateMatrix();
        }
        break;
    case 1:
        if (m_object.get() != 0 && !(m_timer > 0.0f)) {
            bool done = false;
            if (!m_flags.Test(0)) {
                boost::weak_ptr<CharInfo> info = m_object->GetCharInfo();
                done = m_path->Move(info);
                if (boost::shared_ptr<CharInfo> character = info.lock()) {
                    AddVector(character->m_position, s_offset);
                    character->m_rotation.x = 0.0f;
                    character->m_rotation.z = 0.0f;
                }
            }
            m_object->UpdateMatrix();
            m_object->Tick(dt);
            if (done) {
                boost::weak_ptr<CharInfo> info = m_object->GetCharInfo();
                if (!m_path->Next(info)) {
                    m_state = 3;
                }
            }
            if (m_object->HasEnded()) {
                m_state = 2;
            }
        }
        break;
    case 2: {
        if (m_object.get() != 0) {
            m_object->Tick(dt);
        }
        boost::shared_ptr<Mission::Mission> player = fn_800740E4(fn_8006D6A8(), 0).lock();
        if (player.get() != 0) {
            player->AddCount(1, 0);
        }
        m_state = 4;
        break;
    }
    case 3: {
        if (m_object.get() != 0) {
            m_object->Tick(dt);
        }
        boost::shared_ptr<Mission::Mission> player = fn_800740E4(fn_8006D6A8(), 0).lock();
        if (player.get() != 0) {
            fn_8016B270(player.get());
        }
        m_state = 4;
        break;
    }
    case 4:
        if (m_object.get() != 0) {
            m_object->Tick(dt);
        }
        break;
    }
}

void Controller::OnCommand(CharCommand &command) {
    switch (command.TypeId()) {
    case 0x301:
        if (dynamic_cast<GadgetCommand::MoveStartCommand *>(&command) != 0) {
            m_timer = 0.0f;
            m_flags.Clear(0);
        }
        break;
    case 0x302:
        if (GadgetCommand::MoveEndCommand *end = dynamic_cast<GadgetCommand::MoveEndCommand *>(&command)) {
            m_timer = end->m_time;
            if (m_timer <= 0.0f) {
                m_flags.Set(0);
            }
        }
        break;
    case 9:
        if (SwitchCommand *change = dynamic_cast<SwitchCommand *>(&command)) {
            if (m_path != 0) {
                s32 index = change->m_index;
                if (index >= 0 && index < 11) {
                    lbl_8054B8E0[index].m_useAlt = lbl_8054B8E0[index].m_useAlt == 1 ? 0 : 1;
                }
            }
        }
        break;
    case 6:
        if (AccelerateCommand *accelerate = dynamic_cast<AccelerateCommand *>(&command)) {
            if (m_path != 0) {
                m_path->m_targetSpeed = accelerate->m_velocity.x;
            }
            m_flags.Clear(0);
        }
        break;
    case 2:
        if (SetPositionCommand *set = dynamic_cast<SetPositionCommand *>(&command)) {
            int index = (int)set->m_position.x;
            float rate = set->m_position.y;
            if (m_path != 0) {
                m_path->Set(index, rate);
            }
            m_flags.Clear(0);
        }
        break;
    }
}

} // namespace EscapePlaneController
