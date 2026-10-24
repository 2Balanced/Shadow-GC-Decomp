/* BKTankController::Controller (class name from RTTI): the task that drives the Black Arms tank
 * (BKTank::SetBKTank, a PathObject) along one of four paths. It has a character info of its own
 * (id 0x92), through which BK_TANK_COLI boxes (BKTankColi.cpp) and other gadgets command it:
 * GadgetCommand::MoveStartCommand and MoveEndCommand start and stop it, SwitchCommand switches a
 * path to its alternative, AccelerateCommand changes its speed and SetPositionCommand moves it to a
 * path. Once the tank's resources are loaded it starts the tank and moves it each frame; at the
 * end of the last path, or when the tank is destroyed, it notifies the player. File name from the
 * class; the other names are inferred. Definitions are in reverse address order (deferred inlining
 * emits functions last-defined first). */
#include <boost/bind.hpp>
#include <boost/function.hpp>

#include "Game/BKTankController.h"
#include "Game/CharCommand.h"
#include "Game/CharCommandUnit.h"
#include "Game/CharInfo.h"
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

/* SwitchCommand (command type 9) is declared in Game/CharCommand.h. */

class UnkPlayerManager_8006D6A8;

/* The game state (see CityBombSmall.cpp); the byte at 0xC8 pauses the tank (meaning inferred). */
struct UnkGameState_8057D6A0 {
    /* 0x000 */ u8 field_0x0[0xC8];
    /* 0x0C8 */ u8 m_paused;
    /* 0x0C9 */ u8 field_0xC9[0x150 - 0xC9];
};
SIZE_ASSERT(UnkGameState_8057D6A0, 0x150);

/* The character manager (see Monster.cpp). */
class UnkCharManager_800DDF20;

/* The path manager fn_8001B494 returns (class name unknown). */
class UnkPathManager_8001B494;

extern "C" {
extern UnkGameState_8057D6A0 lbl_8057D6A0;

UnkPlayerManager_8006D6A8 *fn_8006D6A8(void);
boost::weak_ptr<Mission::Mission> fn_800740E4(UnkPlayerManager_8006D6A8 *manager, int index);
/* Tells the player that the tank is through (behaviour inferred). */
void fn_8016B270(Mission::Mission *player);

/* The state of resource slot slot (3: loaded; see EffectPJS.cpp), and whether the stage has
 * started (behaviour inferred). */
s32 fn_80012408(void *loader, s32 slot);
u8 fn_80177CF8(void);

/* The character manager and character info helpers (see BKTank.cpp and BAMine.cpp). */
UnkCharManager_800DDF20 *fn_800DDF20(void);
void fn_801DCD8C(UnkCharManager_800DDF20 *manager, const boost::shared_ptr<CharInfo> &info);
void fn_801DCCF4(UnkCharManager_800DDF20 *manager, const boost::shared_ptr<CharInfo> &info);
boost::shared_ptr<GadgetCharInfo> fn_8015CD90(int arg0, int arg1);
void fn_8015CF7C(GadgetCharInfo *charInfo);
/* While it returns true, gadgets stand still (see EggBalloon.cpp). */
bool fn_80169CD0(void);

/* Path control: the path manager, setting a path up by id, the path's length, updating it, and
 * moving a character along it by distance (behaviour inferred; the other arguments are unknown). */
UnkPathManager_8001B494 *fn_8001B494(void);
bool fn_8001C7B4(UnkPathManager_8001B494 *manager, PathCtrl *path, s32 id);
float fn_800183D8(PathCtrl *path);
void fn_80018304(PathCtrl *path);
bool fn_8001C91C(UnkPathManager_8001B494 *manager, PathCtrl *path, int arg2, CharInfo *character,
                 Vec3f *rotation, int arg5, float distance);
}

namespace BKTankController {

/* The four paths the tank follows in turn: path id, the id of its alternative, and whether the
 * alternative is taken (struct and field names inferred). */
struct PathEntry {
    s32 m_id;
    s32 m_altId;
    s32 m_useAlt;
    u8 field_0xC;
};

static PathEntry s_paths[4] = {
    {0x64, 0x64, 0, 1},
    {0x65, 0x65, 0, 1},
    {0x66, 0x66, 0, 1},
    {0x68, 0x68, 0, 1},
};

/* The tank's position on its paths (class name inferred): the current path and the distance
 * along it, and a speed that moves toward a target speed by m_accel per second. */
class TankPath {
  public:
    TankPath();
    /* Sets path m_index up; false if there is none. */
    bool SelectPath();
    /* Moves target along the path by dt; true at the end of the path. */
    bool Move(boost::weak_ptr<CharInfo> target, float dt);
    /* Goes on to the next path; false after the last. */
    bool Next();
    /* Puts the tank on path index, rate (0 to 1) of the way along it. */
    void Set(int index, float rate);

    /* 0x00 */ PathCtrl m_ctrl;
    /* 0x58 */ s32 m_index;
    /* 0x5C */ float m_distance;
    /* 0x60 */ float m_speed;
    /* 0x64 */ float m_targetSpeed;
    /* 0x68 */ float m_accel;
    /* 0x6C */ float field_0x6C;
};
SIZE_ASSERT(TankPath, 0x70);

TankPath::TankPath() {
    m_index = 0;
    m_distance = 0.0f;
    m_speed = 150.0f;
    m_targetSpeed = 150.0f;
    m_accel = 0.0f;
    field_0x6C = 0.0f;
    SelectPath();
}

bool TankPath::SelectPath() {
    m_targetSpeed = 150.0f;
    m_speed = 150.0f;
    m_accel = 0.0f;
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

bool TankPath::Move(boost::weak_ptr<CharInfo> target, float dt) {
    if (fn_80169CD0()) {
        return false;
    }
    if (boost::shared_ptr<CharInfo> character = target.lock()) {
        float distance = m_speed * dt + 0.5f * (dt * (m_accel * dt));
        if (m_targetSpeed > m_speed) {
            m_speed = m_speed + m_accel * dt;
            if (m_speed > m_targetSpeed) {
                m_speed = m_targetSpeed;
                distance = m_speed * dt;
            }
        } else if (m_targetSpeed < m_speed) {
            m_speed = m_speed - m_accel * dt;
            if (m_speed < m_targetSpeed) {
                m_speed = m_targetSpeed;
                distance = m_speed * dt;
            }
        }
        m_distance += distance;
        fn_80018304(&m_ctrl);
        CharInfo *info = character.get();
        return fn_8001C91C(fn_8001B494(), &m_ctrl, 0, info, &info->m_rotation, 1, m_distance);
    }
    return false;
}
bool TankPath::Next() {
    ++m_index;
    if (m_index >= 4) {
        return false;
    }
    return SelectPath();
}

void TankPath::Set(int index, float rate) {
    boost::shared_ptr<Mission::Mission> player = fn_800740E4(fn_8006D6A8(), 2).lock();
    if (player.get() != 0 && player->IsState(2)) {
        return;
    }
    m_index = index;
    SelectPath();
    if (0.0f <= rate && rate <= 1.0f) {
        m_distance = rate * fn_800183D8(&m_ctrl);
    }
}

const char *Controller::s_taskName = "BKTank::Controller";

Controller::Controller(Task *parent, boost::shared_ptr<PathObject> object) : Task(parent) {
    m_name = s_taskName;
    m_chara = fn_8015CD90(0x92, 4);
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

void Controller::Tick(float dt) {
    m_timer -= dt;
    if (m_timer < 0.0f) {
        m_timer = 0.0f;
    }
    if (lbl_8057D6A0.m_paused) {
        return;
    }
    switch (m_state) {
    case 0:
        if (fn_80012408(fn_800122D4(), 1) == 3 && fn_80177CF8() == 1) {
            m_object->Init();
            m_state = 1;
            m_path = new TankPath;
            boost::weak_ptr<CharInfo> info = m_object->GetCharInfo();
            m_path->Move(info, dt);
            m_object->UpdateMatrix();
        }
        break;
    case 1:
        if (m_object.get() != 0 && !(m_timer > 0.0f)) {
            bool done = false;
            if (!m_flags.Test(0)) {
                boost::weak_ptr<CharInfo> info = m_object->GetCharInfo();
                done = m_path->Move(info, dt);
            }
            m_object->UpdateMatrix();
            m_object->Tick(dt);
            if (done && !m_path->Next()) {
                boost::shared_ptr<Mission::Mission> player = fn_800740E4(fn_8006D6A8(), 2).lock();
                if (player.get() != 0) {
                    fn_8016B270(player.get());
                }
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
        m_state = 3;
        break;
    }
    case 3:
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
                if (index >= 0 && index < 4) {
                    s_paths[index].m_useAlt = s_paths[index].m_useAlt == 1 ? 0 : 1;
                }
            }
        }
        break;
    case 6:
        if (AccelerateCommand *accelerate = dynamic_cast<AccelerateCommand *>(&command)) {
            if (m_path != 0) {
                m_path->m_targetSpeed = accelerate->m_velocity.x;
                float time = accelerate->m_velocity.z;
                TankPath *path = m_path;
                if (time <= 0.0f) {
                    path->m_speed = path->m_targetSpeed;
                    path->m_accel = 0.0f;
                } else {
                    path->m_accel = (path->m_targetSpeed - path->m_speed) / time;
                }
            }
            m_flags.Clear(0);
        }
        break;
    case 2: {
        boost::shared_ptr<Mission::Mission> player = fn_800740E4(fn_8006D6A8(), 2).lock();
        if (player.get() != 0 && player->IsState(2)) {
            break;
        }
        if (SetPositionCommand *set = dynamic_cast<SetPositionCommand *>(&command)) {
            int index = (int)set->m_position.x;
            float rate = set->m_position.y;
            if (m_path != 0) {
                m_path->m_targetSpeed = 150.0f;
                m_path->m_speed = 150.0f;
                m_path->m_accel = 0.0f;
                s32 current = m_path->m_index;
                if (current <= index) {
                    m_path->Set(index, rate);
                } else {
                    if (current >= 4) {
                        current = 3;
                        if (m_state == 3) {
                            m_state = 1;
                        }
                    }
                    m_path->Set(current, 0.0f);
                }
            }
            m_flags.Clear(0);
        }
        break;
    }
    }
}

} // namespace BKTankController
