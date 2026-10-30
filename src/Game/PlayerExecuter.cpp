/* Player::Executer (class name from RTTI; declared in Game/PlayerExecuter.h): the task that runs a
 * player character's behaviors. It switches between the behaviors its factory makes, takes the
 * commands the behaviors leave, turns the player upright, and now and then has a waiting player
 * talk (Player::WaitTalkingCommand). File name inferred from the class; other names marked
 * "(inferred)" are not original. Functions other units call by address keep their fn_ names (C
 * functions taking the executer). Definitions are in reverse address order (deferred inlining
 * emits functions last-defined first). */
#define PLAYER_BEHAVIOR_SLOT_ARGS
#define PLAYER_EXECUTER_MEMBERS
#include "Game/CharCommand.h"
#include "Game/CharInfo.h"
#include "Game/MathTypes.h"
#include "Game/Player.h"
#include "Game/PlayerBehavior.h"
#include "Game/PlayerExecuter.h"
#include "Game/PlayerFactory.h"
#include "Game/PlayerPadInput.h"
#include "Game/PlayerWeaponBase.h"
#include "Game/Quaternion.h"
#include "Game/Task.h"
#include "Game/UnkWords_8007A4D4.h"
#include "boost/shared_ptr.hpp"
#include "boost/weak_ptr.hpp"
#include <dolphin/types.h>
#include "size_assert.h"

namespace std {
float fabs(float x);
}

namespace Mission {
class MissionManager;
}


/* A matrix copied with fn_8040C148 (name inferred; as in Game/PlayerUtil.cpp). */
struct UnkExecuterMtx_800A750C : public Mtx34f {
    UnkExecuterMtx_800A750C() {}
    UnkExecuterMtx_800A750C(const Mtx34f &matrix) {
        fn_8040C148(this, &matrix);
    }
};

/* The player's members used here (struct name unknown; Player::PlayerBase leaves them unmapped). */
struct UnkPlayerFields_800A72E4 {
    /* 0x000 */ Vec3f m_position;
    /* 0x00C */ Vec3f m_rotation;
    /* 0x018 */ Vec3f m_velocity;
    /* 0x024 */ Vec3f field_0x24;
    /* 0x030 */ u8 field_0x30[0x54 - 0x30];
    /* 0x054 */ s32 m_pad;
    /* 0x058 */ u8 field_0x58[0x5C - 0x58];
    /* 0x05C */ s32 field_0x5C;
    /* 0x060 */ float *m_frameTime;
    /* 0x064 */ u8 field_0x64[0xA0 - 0x64];
    /* 0x0A0 */ u32 m_flags;
    /* 0x0A4 */ u32 field_0xA4;
    /* 0x0A8 */ UnkWords_8007A4D4 m_stateBits;
    /* 0x0B4 */ u32 field_0xB4;
    /* 0x0B8 */ Vec3f field_0xB8;
    /* 0x0C4 */ u8 field_0xC4[0xD0 - 0xC4];
    /* 0x0D0 */ Vec3f field_0xD0;
    /* 0x0DC */ Vec3f field_0xDC;
    /* 0x0E8 */ Vec3f field_0xE8;
    /* 0x0F4 */ Vec3f m_up;
    /* 0x100 */ Vec3f field_0x100;
    /* 0x10C */ Quatf field_0x10C;
    /* 0x11C */ UnkExecuterMtx_800A750C field_0x11C;
    /* 0x14C */ UnkExecuterMtx_800A750C field_0x14C;
    /* 0x17C */ u8 field_0x17C[0x1C8 - 0x17C];
    /* 0x1C8 */ Vec3f m_groundUp;
    /* 0x1D4 */ u8 field_0x1D4[0x1E4 - 0x1D4];
    /* 0x1E4 */ Mtx34f field_0x1E4;
    /* 0x214 */ u8 field_0x214[0x22C - 0x214];
    /* 0x22C */ Player::Executer *m_executer;
    /* 0x230 */ Player::PlayerMotion *m_motion;
    /* 0x234 */ u8 field_0x234[0x248 - 0x234];
    /* 0x248 */ void *field_0x248;
    /* 0x24C */ Player::PlayerTargetBase *m_target;
    /* 0x250 */ u32 field_0x250;
    /* 0x254 */ Player::UnkCharaSlots_800AADA4 *m_charaSlots;
    /* 0x258 */ u8 field_0x258[0x298 - 0x258];
    /* 0x298 */ s32 m_number;
};

#define Fields(player) reinterpret_cast<UnkPlayerFields_800A72E4 *>(player)

/* What the hint command reads of a player's record and the save data (names unknown). */
struct UnkSave_80079808;

extern "C" {
/* Bits and flag words (see Game/Player.h). */
bool fn_8006CD78(UnkWords_8007A4D4 *bits, int bit);
void fn_80076CA8(UnkWords_8007A4D4 *bits, int bit);
void fn_80076CC8(UnkWords_8007A4D4 *bits, int bit);
bool fn_80014E34(u32 *flags, int bit);

/* Vector and matrix routines. */
float fn_8040CF90(const Vec3f *a, const Vec3f *b); /* dot product */
Mtx34f fn_8040BEF8(const Mtx34f *matrix);         /* inverse */
Vec3f fn_8040B67C(const Mtx34f *matrix, const Vec3f *point);
Vec3f fn_8040CDA8(const Vec3f *vector, float scale);
Vec3f fn_8040F364(const Vec3f *forward, const Vec3f *up); /* Euler angles */
Vec3f fn_8040F16C(const Vec3f *a, const Vec3f *b, int order, float t);
float fn_8040E99C(float angle);
Quatf fn_800934FC(const Vec3f *up);
void fn_8007A34C(Quatf *rotation, const Quatf *value);
Mtx34f fn_8040B228(const Quatf *rotation);
void fn_8040CE60(Vec3f *vector);
void fn_8040CFF8(Vec3f *vector, const Vec3f *value);
void fn_8040CE74(Vec3f *vector);
Mtx34f fn_8001E044(const Mtx34f *a, const Mtx34f *b); /* product */
int fn_8040EA90(int count);

/* The player (behaviour inferred). */
Vec3f fn_800796DC(Player::PlayerBase *player);
Vec3f fn_80079698(Player::PlayerBase *player);
void fn_800932B4(boost::shared_ptr<Player::PlayerBase> *player);
void fn_80078F64(Player::PlayerBase *player, float dt);
void fn_80078D34(Player::PlayerBase *player);
void fn_80078E84(Player::PlayerBase *player);
void fn_80078F1C(Player::PlayerBase *player, int arg1, float arg2);
void fn_80095AD0(void *arg);
void fn_80095AF4(void *arg);
void fn_80095A88(void *arg);
void fn_800AAE14(Player::UnkCharaSlots_800AADA4 *slots);
void fn_800AA638(Player::PlayerMotion *motion, int arg);

UnkPlayerInput_800A85B0 *fn_8006F8D0(Player::PlayerBase *player);

/* The mission, the characters and the save data (behaviour inferred). */
Mission::MissionManager *fn_8006D6A8(void);
void *fn_8006F754(void);
int fn_800747E0(void);
boost::weak_ptr<CharInfo> fn_8007488C(void *manager, int character);
UnkSave_80079808 *fn_80079808(void);
void *fn_80178B5C(UnkSave_80079808 *save, int player);
void fn_8016F5B8(void *record, int index, int value);
void fn_8016F0CC(void *score, int index, int value);
}

/* The vector addition and the matrix copy taking references (not original; see
 * Game/BkLarva.cpp). */
inline void AddVector(Vec3f &dst, const Vec3f &value) {
    fn_8040CFF8(&dst, &value);
}
inline void CopyMatrix(Mtx34f &dst, const Mtx34f &src) {
    fn_8040C148(&dst, &src);
}

void ShowRandomHint(float chance);
void ShowRandomHint(const boost::weak_ptr<Player::PlayerBase> &player, float chance);

namespace Player {

/* WaitTalkingCommand (class name from RTTI; vtable 0x80521A34): command 0x132, which the executer
 * sends to a character when the player stands still long enough (meaning inferred). */
class WaitTalkingCommand : public PlayerCommand {
  public:
    WaitTalkingCommand() : PlayerCommand(0x132, 4) {
        Init();
    }
    void Init() {}
    virtual ~WaitTalkingCommand();
};
SIZE_ASSERT(WaitTalkingCommand, 0x14);

} // namespace Player

/* The commands the executer reads (struct names unknown; members inferred). */
struct UnkVectorCommand_800A78FC : public CharCommand {
    /* 0x14 */ Vec3f m_vector;
};

struct UnkMotionCommand_800A78FC : public CharCommand {
    /* 0x14 */ s32 m_value;
};

struct UnkFloatCommand_800A78FC : public CharCommand {
    /* 0x14 */ u32 field_0x14;
    /* 0x18 */ float m_value;
    /* 0x1C */ s32 m_arg;
};

struct UnkScoreCommand_800A78FC : public CharCommand {
    /* 0x14 */ s32 m_values[5];
};

namespace Player {

WaitTalkingCommand::~WaitTalkingCommand() {}

Executer::Executer(Task *parent, const boost::shared_ptr<PlayerBase> &player)
    : Task(parent), m_player(player) {
    Fields(m_player.get())->m_executer = this;
    m_waitTime = 10.0f;
    m_waitCount = 0;
}

Executer::~Executer() {
    if (m_behavior) {
        m_behavior->Leave(0x25);
    }
    Fields(m_player.get())->m_executer = 0;
}

} // namespace Player

/* Sets the factory that makes the behaviors. */
extern "C" void fn_800A8078(Player::Executer *executer,
                            const boost::shared_ptr<Player::Behavior::Factory> &factory) {
    executer->m_factory = factory;
}

/* Switches to the behavior args names; false if it may not start. */
extern "C" bool fn_800A7F14(Player::Executer *executer, const Player::Behavior::InitializeArgs &args) {
    int prev;
    if (executer->m_behavior) {
        prev = executer->m_behavior->m_id;
    } else {
        prev = 0x25;
    }
    boost::shared_ptr<Player::Behavior::Behavior> next = executer->m_factory->Create(args.m_id);
    executer->m_waitTime = 10.0f;
    if (next && next->vf_0x0C(prev, args)) {
        if (executer->m_behavior) {
            executer->m_behavior->Leave(next->m_id);
        }
        executer->m_behavior = next;
        if (executer->m_behavior) {
            executer->m_behavior->Enter(prev, args);
        }
        return true;
    }
    return false;
}

/* Whether the running behavior is id. */
extern "C" bool fn_800A7EEC(Player::Executer *executer, int id) {
    if (executer->m_behavior && id == executer->m_behavior->m_id) {
        return true;
    }
    return false;
}

extern "C" void fn_800A72E4(Player::Executer *executer, float dt);
extern "C" void fn_800A750C(Player::Executer *executer, float dt);
extern "C" void fn_800A7700(Player::Executer *executer);

/* The frame update: the player, the behavior and what follows them. */
extern "C" void fn_800A7D78(Player::Executer *executer) {
    float dt = *Fields(executer->m_player.get())->m_frameTime;
    fn_800A72E4(executer, dt);
    fn_80078F64(executer->m_player.get(), dt);
    fn_800A7700(executer);
    if (fn_8040CF2C(&Fields(executer->m_player.get())->field_0xB8) > 1.0f) {
        fn_8040CE74(&Fields(executer->m_player.get())->field_0xB8);
    }
    if (executer->m_behavior) {
        boost::shared_ptr<Player::Behavior::Behavior> behavior = executer->m_behavior;
        behavior->Update(dt);
    }
    fn_800A750C(executer, dt);
    fn_80078D34(executer->m_player.get());
    if (fn_8006CD78(&Fields(executer->m_player.get())->m_stateBits, 0x43)) {
        fn_80095AD0(Fields(executer->m_player.get())->field_0x248);
    } else {
        fn_80095AF4(Fields(executer->m_player.get())->field_0x248);
    }
    fn_80095A88(Fields(executer->m_player.get())->field_0x248);
    Fields(executer->m_player.get())->m_target->vf_0x0C();
    fn_800AAE14(Fields(executer->m_player.get())->m_charaSlots);
    fn_8040CE60(&Fields(executer->m_player.get())->field_0x24);
    fn_8040CE60(&Fields(executer->m_player.get())->field_0xB8);
    fn_80076CC8(&Fields(executer->m_player.get())->m_stateBits, 0x1B);
    fn_800091D0(&Fields(executer->m_player.get())->field_0xD0, &Fields(executer->m_player.get())->m_position);
    fn_80078E84(executer->m_player.get());
}

/* Takes the commands the player's behaviors left. */
extern "C" void fn_800A78FC(Player::Executer *executer, CharCommand *command) {
    switch (command->TypeId()) {
    case 1: {
        UnkVectorCommand_800A78FC *move = static_cast<UnkVectorCommand_800A78FC *>(command);
        if (!(move->m_vector.x * move->m_vector.x + move->m_vector.y * move->m_vector.y +
                  move->m_vector.z * move->m_vector.z <
              0.0001f)) {
            fn_80076CA8(&Fields(executer->m_player.get())->m_stateBits, 0x1B);
            Vec3f vector;
            fn_800091EC(&vector, &move->m_vector);
            AddVector(Fields(executer->m_player.operator->())->field_0xB8, vector);
        }
        command->Flags() |= 2;
        break;
    }
    case 2: {
        UnkVectorCommand_800A78FC *place = static_cast<UnkVectorCommand_800A78FC *>(command);
        fn_800091D0(&Fields(executer->m_player.get())->field_0xD0, &place->m_vector);
        fn_800091D0(&Fields(executer->m_player.get())->m_position, &place->m_vector);
        fn_800091D0(&Fields(executer->m_player.get())->field_0xDC, &place->m_vector);
        command->Flags() |= 2;
        break;
    }
    case 5: {
        UnkVectorCommand_800A78FC *push = static_cast<UnkVectorCommand_800A78FC *>(command);
        AddVector(Fields(executer->m_player.operator->())->m_velocity, push->m_vector);
        if (fn_8040CF90(&Fields(executer->m_player.get())->m_velocity,
                        &Fields(executer->m_player.get())->m_groundUp) > 0.0f) {
            fn_80076CC8(&Fields(executer->m_player.get())->m_stateBits, 2);
        }
        command->Flags() |= 2;
        break;
    }
    case 3: {
        command->Reject(boost::weak_ptr<CharInfo>(executer->m_player));
        Mtx34f matrix;
        fn_8040C148(&matrix, &lbl_805E0720);
        fn_8040B848(&matrix, Fields(executer->m_player.get())->m_rotation.y);
        fn_8040B884(&matrix, Fields(executer->m_player.get())->m_rotation.x);
        fn_8040B80C(&matrix, Fields(executer->m_player.get())->m_rotation.z);
        CopyMatrix(matrix, fn_8001E044(&(const Mtx34f &)fn_8040BEF8(
                                           &Fields(executer->m_player.operator->())->field_0x1E4),
                                       &matrix));
        Vec3f front;
        fn_800091D0(&front, &(const Vec3f &)fn_8040B67C(&matrix, &Vec3f(0.0f, 0.0f, -1.0f)));
        Vec3f top;
        fn_800091D0(&top, &(const Vec3f &)fn_8040B67C(&matrix, &Vec3f(0.0f, 1.0f, 0.0f)));
        Fields(executer->m_player.operator->())->m_rotation = fn_8040F364(&front, &top);
        break;
    }
    case 0xB:
    case 0xC:
        command->Flags() |= 2;
        break;
    case 0x17: {
        UnkFloatCommand_800A78FC *set = static_cast<UnkFloatCommand_800A78FC *>(command);
        fn_80078F1C(executer->m_player.get(), set->m_arg, set->m_value);
        break;
    }
    case 0x18: {
        Player::PlayerMotion *motion = Fields(executer->m_player.get())->m_motion;
        if (motion != 0) {
            fn_800AA638(motion, static_cast<UnkMotionCommand_800A78FC *>(command)->m_value);
        }
        command->Flags() |= 2;
        break;
    }
    case 0x1C: {
        UnkScoreCommand_800A78FC *score = static_cast<UnkScoreCommand_800A78FC *>(command);
        int pad = Fields(executer->m_player.get())->m_pad;
        if (pad == -1) {
            pad = 0;
        }
        u8 *record = static_cast<u8 *>(fn_80178B5C(fn_80079808(), pad));
        if (record != 0) {
            fn_8016F5B8(record + 0x2C, 0, score->m_values[1]);
            fn_8016F5B8(record + 0x2C, 1, score->m_values[2]);
            fn_8016F5B8(record + 0x2C, 2, score->m_values[0]);
        }
        u8 *save = reinterpret_cast<u8 *>(fn_80079808());
        if (save != 0) {
            fn_8016F0CC(save + 0x70C, 0, score->m_values[4]);
            fn_8016F0CC(save + 0x70C, 1, score->m_values[3]);
        }
        command->Flags() |= 2;
        break;
    }
    case 0x132:
        if (Fields(executer->m_player.get())->field_0x5C == 1) {
            ShowRandomHint(0.005f);
        } else {
            ShowRandomHint(boost::weak_ptr<Player::PlayerBase>(executer->m_player), 0.005f);
        }
        command->Flags() |= 2;
        break;
    }
}

/* Moves the player's frame along: its up vector, rotation and matrices. */
extern "C" void fn_800A7700(Player::Executer *executer) {
    if (!fn_8006CD78(&Fields(executer->m_player.get())->m_stateBits, 0x39)) {
        if (fn_8006CD78(&Fields(executer->m_player.get())->m_stateBits, 0x3E)) {
            fn_800932B4(&executer->m_player);
        } else {
            UnkPlayerFields_800A72E4 *fields = Fields(executer->m_player.get());
            fn_800091D0(&fields->m_up, &fields->field_0xE8);
        }
    }
    UnkPlayerFields_800A72E4 *fields = Fields(executer->m_player.get());
    fn_8007A34C(&fields->field_0x10C, &(const Quatf &)fn_800934FC(&fields->m_up));
    CopyMatrix(Fields(executer->m_player.operator->())->field_0x11C,
               fn_8040B228(&Fields(executer->m_player.operator->())->field_0x10C));
    CopyMatrix(Fields(executer->m_player.operator->())->field_0x14C,
               fn_8040BEF8(&Fields(executer->m_player.operator->())->field_0x11C));
    fn_8040CE60(&Fields(executer->m_player.get())->field_0x100);
    if (fn_8006CD78(&Fields(executer->m_player.get())->m_stateBits, 0)) {
        float gravity;
        if (fn_8006CD78(&Fields(executer->m_player.get())->m_stateBits, 1)) {
            gravity = -0.0625f;
        } else if (fn_8006CD78(&Fields(executer->m_player.get())->m_stateBits, 2)) {
            gravity = -800.0f;
        } else {
            gravity = -320.0f;
        }
        if (!fn_8006CD78(&Fields(executer->m_player.get())->m_stateBits, 0xD) &&
            fn_80014E34(&Fields(executer->m_player.get())->m_flags, 0)) {
            const Vec3f &force = fn_8040CDA8(&Fields(executer->m_player.get())->m_groundUp, gravity);
            fn_800091D0(&Fields(executer->m_player.get())->field_0x100, &force);
        } else {
            const Vec3f &force = fn_8040CDA8(&Fields(executer->m_player.get())->m_up, gravity);
            fn_800091D0(&Fields(executer->m_player.get())->field_0x100, &force);
        }
        AddVector(Fields(executer->m_player.operator->())->field_0x24,
                  Fields(executer->m_player.operator->())->field_0x100);
    }
}

/* v pointed against the sign of dot (not original; an inline function in the original, whose
 * argument retail computes before the sign). */
inline Vec3f AgainstSign(const Vec3f &v, float dot) {
    return fn_8040CDA8(&v, dot < 0.0f ? 1.0f : -1.0f);
}

/* Turns the player upright towards its up vector, then wraps its angles. */
extern "C" void fn_800A750C(Player::Executer *executer, float dt) {
    if (!fn_8006CD78(&Fields(executer->m_player.get())->m_stateBits, 0x38)) {
        Vec3f angles(0.0f, 0.0f, 0.0f);
        const Vec3f &front = fn_800796DC(executer->m_player.get());
        float dot = fn_8040CF90(&Fields(executer->m_player.get())->m_up, &front);
        if (std::fabs(dot) > 0.985f) {
            UnkExecuterMtx_800A750C matrix(fn_8040BEF8(&Fields(executer->m_player.operator->())->field_0x11C));
            Vec3f up;
            fn_800091EC(&up, &Fields(executer->m_player.get())->m_up);
            Vec3f side;
            fn_800091EC(&side, &(const Vec3f &)AgainstSign(fn_8040B67C(&matrix, &(const Vec3f &)fn_80079698(executer->m_player.get())), dot));
            angles = fn_8040F364(&side, &up);
        } else {
            angles.x = 0.0f;
            angles.y = Fields(executer->m_player.get())->m_rotation.y;
            angles.z = 0.0f;
        }
        float t = 1.0f;
        if (30.0f * dt < t) {
            t = 30.0f * dt;
        }
        Fields(executer->m_player.operator->())->m_rotation =
            fn_8040F16C(&Fields(executer->m_player.operator->())->m_rotation, &angles, 2, t);
    }
    UnkPlayerFields_800A72E4 *fields = Fields(executer->m_player.get());
    fields->m_rotation.x = fn_8040E99C(fields->m_rotation.x);
    fields = Fields(executer->m_player.get());
    fields->m_rotation.y = fn_8040E99C(fields->m_rotation.y);
    fields = Fields(executer->m_player.get());
    fields->m_rotation.z = fn_8040E99C(fields->m_rotation.z);
}

/* While the player waits, sends a talking command to itself or another character now and then. */
extern "C" void fn_800A72E4(Player::Executer *executer, float dt) {
    if (Fields(executer->m_player.get())->m_pad == -1) {
        return;
    }
    if (!fn_80014E34(&Fields(executer->m_player.get())->m_flags, 9) ||
        fn_8006F8D0(executer->m_player.get())->GetIdleTime() == 0.0f) {
        executer->m_waitTime = 20.0f;
        return;
    }
    executer->m_waitTime -= dt;
    if (executer->m_waitTime <= 0.0f) {
        int mode = reinterpret_cast<s32 *>(fn_8006D6A8())[0x20 / 4];
        boost::shared_ptr<CharInfo> target(executer->m_player);
        switch (mode) {
        case 0:
        case 2:
            if (!(fn_8040EA90(0x7FFF) & 0x100)) {
                void *characters = fn_8006F754();
                boost::weak_ptr<CharInfo> other = fn_8007488C(characters, fn_800747E0());
                if (!other.expired()) {
                    target = other.lock();
                }
            }
            break;
        }
        target->PushCommand(new Player::WaitTalkingCommand());
        executer->m_waitCount++;
        executer->m_waitTime = 20.0f;
    }
}
