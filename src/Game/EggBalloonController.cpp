/* EggBalloonController::Controller (class name from RTTI): the task that drives the egg balloon
 * (EggBalloon::SetEggBalloon, a PathObject) along one of five paths, as BKTankController.cpp
 * drives the Black Arms tank. It has a character info of its own (id 0x3F), through which E_B_COLI
 * boxes (EggBalloonColi.cpp) command it: GadgetCommand::MoveStartCommand and MoveEndCommand start
 * and stop it, SwitchCommand switches a path to its alternative, AccelerateCommand changes its
 * speed, SetPositionCommand moves it on to a later path, and GadgetCommand::AttackStartCommand
 * and AttackEndCommand are passed on to the balloon. Once the balloon's resources are loaded it
 * starts the balloon and moves it each frame, turned half round to face backwards; when the
 * balloon is destroyed, or at the end of the last path, it notifies the player. File name from the
 * class; the other names are inferred. Definitions are in reverse address order (deferred
 * inlining emits functions last-defined first). */
#include <boost/bind.hpp>
#include <boost/function.hpp>

#include "Game/CharCommand.h"
#include "Game/CharCommandUnit.h"
#include "Game/CharInfo.h"
#include "Game/EggBalloonController.h"
#include "Game/GadgetCharCommands.h"
#include "Game/GadgetCharInfo.h"
#include "Game/GadgetCommand.h"
#include "Game/Mission.h"
#include "Game/PathCtrl.h"
#include "Game/PathObject.h"
#include "Game/ResourceOneFile.h"
#include "Game/Task.h"
#include "boost/shared_ptr.hpp"
#include "boost/weak_ptr.hpp"
#include "size_assert.h"

using SonicteamUSA::System::PathCtrl;

/* SwitchCommand (class name from RTTI; vtable at 0x8053D404): command 9, which switches path
 * m_index of a path-following object to its alternative (see BKTankController.cpp). */
class SwitchCommand : public CharCommand {
  public:
    /* 0x14 */ s32 m_index;
};

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

extern "C" {
extern UnkGameState_8057D6A0 lbl_8057D6A0;
extern const float lbl_805FAD38; /* pi */

UnkPlayerManager_8006D6A8 *fn_8006D6A8(void);
boost::weak_ptr<Mission::Mission> fn_800740E4(UnkPlayerManager_8006D6A8 *manager, int index);
/* Tells the player that the balloon is through (behaviour inferred). */
void fn_8016B270(Mission::Mission *player);

/* The state of resource slot slot (3: loaded; see EffectPJS.cpp), and whether the stage has
 * started (behaviour inferred). */
s32 fn_80012408(void *loader, s32 slot);
u8 fn_80177CF8(void);

/* The sound manager, and a sound played at a character for owner (behaviour inferred; the other
 * arguments are unknown). */
void *fn_80053164(void);
void fn_801D53C4(void *manager, int id, CharInfo *character, void *owner, int arg4, int arg5, int arg6);

/* The character manager and character info helpers (see BKTankController.cpp). */
UnkCharManager_800DDF20 *fn_800DDF20(void);
void fn_801DCD8C(UnkCharManager_800DDF20 *manager, const boost::shared_ptr<CharInfo> &info);
void fn_801DCCF4(UnkCharManager_800DDF20 *manager, const boost::shared_ptr<CharInfo> &info);
boost::shared_ptr<GadgetCharInfo> fn_8015CD90(int arg0, int arg1);
void fn_8015CF7C(GadgetCharInfo *charInfo);
/* While it returns true, gadgets stand still (see EggBalloon.cpp). */
bool fn_80169CD0(void);

/* Path control (see BKTankController.cpp). */
UnkPathManager_8001B494 *fn_8001B494(void);
bool fn_8001C7B4(UnkPathManager_8001B494 *manager, PathCtrl *path, s32 id);
float fn_800183D8(PathCtrl *path);
void fn_80018304(PathCtrl *path);
bool fn_8001C91C(UnkPathManager_8001B494 *manager, PathCtrl *path, int arg2, CharInfo *character,
                 Vec3f *rotation, int arg5, float distance);
}

/* Degrees to radians (name inferred). */
inline float DegToRad(float degrees) {
    return degrees * (lbl_805FAD38 / 180.0f);
}

namespace EggBalloonController {

/* How far the balloon is turned from its path's direction (name inferred). A call, so the static
 * initializer stores it, as in retail. */
static const float s_turn = DegToRad(180.0f);

/* The five paths the balloon follows in turn: path id, the id of its alternative, and whether the
 * alternative is taken (as in BKTankController.cpp). */
struct PathEntry {
    s32 m_id;
    s32 m_altId;
    s32 m_useAlt;
    u8 field_0xC;
};

static PathEntry s_paths[5] = {
    {0x64, 0x64, 0, 1},
    {0x65, 0x65, 0, 1},
    {0x66, 0x66, 0, 1},
    {0x67, 0x67, 0, 1},
    {0x68, 0x68, 0, 1},
};

/* The balloon's position on its paths (class name inferred): the current path and the distance
 * along it, and a speed that moves toward a target speed by one unit per second. */
class BalloonPath {
  public:
    BalloonPath();
    /* Sets path m_index up; false if there is none. */
    bool SelectPath();
    /* Moves target along the path by a frame; true at the end of the path. */
    bool Move(boost::weak_ptr<CharInfo> target);
    /* Goes on to the next path; false after the last. */
    bool Next();
    /* Puts the balloon on path index, rate (0 to 1) of the way along it, unless it is past it. */
    void Set(int index, float rate);

    /* 0x00 */ PathCtrl m_ctrl;
    /* 0x58 */ s32 m_index;
    /* 0x5C */ float m_distance;
    /* 0x60 */ float m_speed;
    /* 0x64 */ float m_targetSpeed;
    /* 0x68 */ float field_0x68;
};
SIZE_ASSERT(BalloonPath, 0x6C);

BalloonPath::BalloonPath() {
    m_index = 0;
    m_distance = 0.0f;
    m_speed = 0.5f;
    m_targetSpeed = 0.5f;
    field_0x68 = 0.0f;
    for (int i = 0; i < 5; i++) {
        s_paths[i].m_useAlt = 0;
    }
    SelectPath();
}

bool BalloonPath::SelectPath() {
    m_targetSpeed = 0.5f;
    m_speed = 0.5f;
    PathEntry *entry = &s_paths[m_index];
    s32 id;
    if (entry->m_useAlt == 1) {
        id = entry->m_altId;
    } else {
        id = entry->m_id;
    }
    if (fn_8001C7B4(fn_8001B494(), &m_ctrl, id)) {
        m_distance = 0.0f;
        fn_80018304(&m_ctrl);
        return true;
    }
    return false;
}

bool BalloonPath::Move(boost::weak_ptr<CharInfo> target) {
    if (boost::shared_ptr<CharInfo> character = target.lock()) {
        if (!fn_80169CD0()) {
            if (m_targetSpeed > m_speed) {
                m_speed = m_speed + lbl_8057D6A0.m_dt;
                if (m_speed > m_targetSpeed) {
                    m_speed = m_targetSpeed;
                }
            } else if (m_targetSpeed < m_speed) {
                m_speed = m_speed - lbl_8057D6A0.m_dt;
                if (m_speed < m_targetSpeed) {
                    m_speed = m_targetSpeed;
                }
            }
            m_distance += lbl_8057D6A0.m_dt * (20.0f * m_speed);
        }
        fn_80018304(&m_ctrl);
        CharInfo *info = character.get();
        return fn_8001C91C(fn_8001B494(), &m_ctrl, 0, info, &info->m_rotation, 1, m_distance);
    }
    return false;
}

bool BalloonPath::Next() {
    ++m_index;
    if (m_index >= 5) {
        return false;
    }
    return SelectPath();
}

void BalloonPath::Set(int index, float rate) {
    if (index >= m_index) {
        m_index = index;
        SelectPath();
        if (0.0f <= rate && rate <= 1.0f) {
            m_distance = rate * fn_800183D8(&m_ctrl);
        }
    }
}

const char *Controller::s_taskName = "EggBalloon::Controller";

Controller::Controller(Task *parent, boost::shared_ptr<PathObject> object) : Task(parent) {
    m_name = s_taskName;
    m_chara = fn_8015CD90(0x3F, 4);
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

} // namespace EggBalloonController

/* Calls the balloon's vf_0x28 (EggBalloon.cpp calls it by this name). */
extern "C" void fn_802742D4(EggBalloonController::Controller *controller) {
    if (controller->m_object.get() != 0) {
        controller->m_object->vf_0x28();
    }
}

namespace EggBalloonController {

void Controller::Tick(float dt) {
    if (!fn_80169CD0()) {
        m_timer -= dt;
        if (m_timer < 0.0f) {
            m_timer = 0.0f;
            if (m_flags.Test(0)) {
                m_flags.Clear(0);
            }
        }
    }
    switch (m_state) {
    case 0:
        if (fn_80012408(fn_800122D4(), 1) == 3 && fn_80177CF8() == 1) {
            m_object->Init();
            m_state = 1;
            m_path = new BalloonPath;
            boost::weak_ptr<CharInfo> info = m_object->GetCharInfo();
            m_path->Move(info);
            if (boost::shared_ptr<CharInfo> character = info.lock()) {
                character->m_rotation.y += s_turn;
                character->m_rotation.x = 0.0f;
                character->m_rotation.z = 0.0f;
            }
            m_object->UpdateMatrix();
        }
        break;
    case 1:
        if (m_object.get() != 0) {
            bool done = false;
            if (!m_flags.Test(0) && !m_flags.Test(1)) {
                boost::weak_ptr<CharInfo> info = m_object->GetCharInfo();
                done = m_path->Move(info);
                if (boost::shared_ptr<CharInfo> character = info.lock()) {
                    character->m_rotation.y += s_turn;
                    character->m_rotation.x = 0.0f;
                    character->m_rotation.z = 0.0f;
                    fn_801D53C4(fn_80053164(), 0x754C, character.get(), this, 3, 0, 2);
                }
            }
            m_object->UpdateMatrix();
            m_object->Tick(dt);
            if (done && !m_path->Next()) {
                m_state = 3;
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
        boost::shared_ptr<Mission::Mission> player = fn_800740E4(fn_8006D6A8(), 2).lock();
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
        boost::shared_ptr<Mission::Mission> player = fn_800740E4(fn_8006D6A8(), 2).lock();
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
            m_flags.Clear(1);
        }
        break;
    case 0x302:
        if (GadgetCommand::MoveEndCommand *end = dynamic_cast<GadgetCommand::MoveEndCommand *>(&command)) {
            m_timer = end->m_time;
            if (m_timer > 0.0f) {
                m_flags.Set(0);
            } else {
                m_flags.Set(1);
            }
        }
        break;
    case 9:
        if (SwitchCommand *change = dynamic_cast<SwitchCommand *>(&command)) {
            if (m_path != 0) {
                s32 index = change->m_index;
                if (index >= 0 && index < 5) {
                    s_paths[index].m_useAlt = s_paths[index].m_useAlt == 1 ? 0 : 1;
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
    case 0x303:
        if (m_object.get() != 0) {
            m_object->OnMessage(reinterpret_cast<UnkPathObjectMessage *>(&command));
        }
        break;
    case 0x304:
        if (m_object.get() != 0) {
            m_object->OnMessage(reinterpret_cast<UnkPathObjectMessage *>(&command));
        }
        break;
    }
}

} // namespace EggBalloonController
