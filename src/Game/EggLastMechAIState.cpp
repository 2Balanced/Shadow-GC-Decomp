/* EggLastMech::AIState (namespace and class names from RTTI): the states of EggLastMech's AI
 * (EggLastMechAI in Game/EggLastMech.h). Each state is a singleton, a function-local static its
 * getter constructs on first use and registers for destruction. The states walk the boss through
 * its fight: Start waits for the event camera, Idle and Flee keep away from the player while the
 * slot machine (UnkSlot_803142C0) spins, Brake stops before the next state, Pursuit chases the
 * player, the Fever states play the slot results, Event and EventStart play the boss's scripted
 * events (s_events) and End finishes it. File name inferred from the namespace. Names marked
 * "(name inferred)" are not original; the state ids are those GetId returns. Definitions are in
 * reverse address order (deferred inlining emits functions last-defined first). */

#include "Game/CharInfo.h"
#include "Game/EggLastMech.h"
#include "Game/EnemyCharInfo.h"
#include "Game/EnemyHint.h"
#include "Game/Random.h"
#include "Game/MathTypes.h"
#include "boost/shared_ptr.hpp"
#include "boost/weak_ptr.hpp"
#include <dolphin/types.h>
#include "size_assert.h"

/* The game state (see Coaster.cpp): Event sets the float at 0x1C (meaning unknown). */
struct UnkGameState_8057D6A0 {
    /* 0x000 */ u8 field_0x0[0x1C];
    /* 0x01C */ float field_0x1C;
    /* 0x020 */ u8 field_0x20[0x150 - 0x20];
};
SIZE_ASSERT(UnkGameState_8057D6A0, 0x150);

/* The save data (see SavePoint.cpp): the record at 0x70C keeps the score (meaning inferred). */
struct UnkObject_80079808 {
    /* 0x000 */ u8 field_0x0[0x70C];
    /* 0x70C */ u8 m_score[1];
};

/* What fn_803577D4 takes (struct name unknown; names inferred). */
struct UnkParam_803577D4 {
    UnkParam_803577D4() {}

    /* 0x00 */ Vec3f m_position;
    /* 0x0C */ s32 m_value;
};

namespace EggLastMech {

inline EggLastMechAIStateBase::~EggLastMechAIStateBase() {}

inline bool EggLastMechTimer::IsExpired() {
    bool expired = false;
    if (m_time < 0.0f) {
        expired = true;
    }
    return expired;
}

inline void EggLastMechTimer::Set(float time) {
    m_time = time;
    m_duration = time;
}

extern "C" {
extern UnkGameState_8057D6A0 lbl_8057D6A0;

/* Pi and a score bonus (constants of other units). */
extern const float lbl_805FAD38;
extern const float lbl_805F4BF0;

/* The zero vector (in MathTypes.h) and the rotation the event camera uses with it
 * (read-only data). */
extern const Vec3f lbl_805E07B8;

/* Motion helpers (behaviour inferred): whether the motion ended, its id, and changing it. */
bool fn_801A4810(void *motion);
u16 fn_801A4494(void *motion);
void fn_801A4898(void *motion, int id, int arg2, float arg3);

/* The event manager and its events (behaviour inferred). */
void *fn_80074470(void);
void fn_80175EEC(void *manager, int event, int arg2);

/* The character manager (see CharInfo.h); fn_800A8DFC stops its characters (behaviour inferred). */
void fn_800A8DFC(UnkCharaList_80074194 *manager);

/* Vector helpers (see MathTypes.h). */
Vec3f fn_8040CEE8(const Vec3f *vector);              /* normalised */
Vec3f fn_8040CDA8(const Vec3f *vector, float scale); /* vector * scale */
Vec3f fn_8040CDD0(const Vec3f *a, const Vec3f *b); /* a + b */
void fn_8040CE04(Vec3f *vector, const Vec3f *value);
void fn_8040CE60(Vec3f *vector);
void fn_8040E970(Vec3f *angles);
float fn_8040E99C(float angle);
Vec3f fn_80016284(const Fn8040CopyVector3 *vector);
double fn_80016260(float x, float z);

/* Turns the enemy toward a point (behaviour inferred). */
void fn_8029EB5C(EnemyCharInfo *info, const Vec3f *target, float rate);

/* The movement part at 0xF0 (behaviour inferred). */
void fn_8032D518(UnkPart_8032D6FC *part, Vec3f *target, Vec3f *offset);
void fn_8032D674(UnkPart_8032D6FC *part, const Vec3f *offset);
void fn_8032D970(void);


/* The part at 0xF8 (behaviour inferred). */
Vec3f fn_80334918(void);
bool fn_8033496C(UnkPart_80334C78 *part);
int fn_803349A0(UnkPart_80334C78 *part);

/* The slot machine (behaviour inferred). */
void fn_80313C8C(UnkSlot_803142C0 *slot);
int fn_80313F48(UnkSlot_803142C0 *slot);
void fn_80314004(UnkSlot_803142C0 *slot, int reel, int arg2, int arg3);

/* The sound player (behaviour inferred). */
void fn_80336B1C(UnkSound_80336F48 *sound, int id);
void fn_80336B58(UnkSound_80336F48 *sound, int id);

/* Loads a layout file (behaviour inferred). */
void fn_8035367C(const char *path, const char *name, const char *motion);
/* Effects around the player (behaviour inferred). */
void fn_801ED4B0(int id, CharInfo *charInfo, float radiusSquared);
void fn_803577D4(UnkParam_803577D4 *param);
/* The save data and its score (see SavePoint.cpp). */
UnkObject_80079808 *fn_80079808(void);
void fn_8016F178(void *score, int arg1, int points);
}

/* Vector helpers returning by value, assigned through the out-of-line assignment. */
typedef Vec3f (*VectorSubtractFn)(const Vec3f *a, const Vec3f *b);

inline void AssignVector(Vec3f &vector, const Vec3f &value) {
    fn_800091D0(&vector, &value);
}

inline float GetSpeed(EnemyCharInfo *info) {
    return info->field_0xAC;
}

inline float DegToRad(float degrees) {
    return degrees * (lbl_805FAD38 / 180.0f);
}

/* A float handed to a state's Enter through its argument (struct name unknown; a class, so a
 * function-local one is constructed on first use). */
struct UnkFloat {
    UnkFloat(float value) : value(value) {}

    float value;
};

/* The boss's scripted events (values from .rodata; names inferred). */
struct EventInfo {
    /* 0x00 */ s32 m_camera;     /* the camera mode while the event plays */
    /* 0x04 */ s32 m_nextCamera; /* the camera mode after it, or -1 */
    /* 0x08 */ u16 m_motion;
    /* 0x0A */ u16 m_sound; /* 0xFFFF for none */
    /* 0x0C */ s32 m_hint;  /* -1 for none */
    /* 0x10 */ u8 m_keepPosition;
};

static const EventInfo s_events[8] = {
    {0x18, -1, 0xD, 0xFFFF, 0, 0},    {0x12, -1, 1, 0x7F37, 2, 1},   {0x13, -1, 2, 0x7F38, 3, 0},
    {0x17, 0x16, 7, 0xFFFF, -1, 1},   {0x14, -1, 3, 0xFFFF, -1, 1},  {0x17, 0x16, 7, 0xFFFF, -1, 1},
    {0x15, -1, 5, 0xFFFF, -1, 1},     {0x17, 0x16, 7, 0xFFFF, -1, 1},
};

/* Where the boss stood when its event started (names inferred). */
static Vec3f s_eventPosition(0.0f, 0.0f, 0.0f);
static Vec3f s_eventRotation(0.0f, 0.0f, 0.0f);
/* Distances (meaning inferred; the first is not used here). */
static const UnkFloat s_distance(300.0f);
static const UnkFloat s_effectRadius(100.0f);

namespace AIState {

/* The states (class names from RTTI; layouts from their use here). */
class End : public EggLastMechAIStateBase {
  public:
    virtual ~End();
    virtual void Enter(EggLastMech *mech, int arg);
    virtual void Tick(EggLastMech *mech);
    virtual void Exit(EggLastMech *mech);
    virtual int GetId();
};

class BreakTime : public EggLastMechAIStateBase {
  public:
    virtual void Enter(EggLastMech *mech, int arg);
    virtual void Tick(EggLastMech *mech);
    virtual void Exit(EggLastMech *mech);
    virtual int GetId() {
        return 0xD;
    }
};

class FeverRing : public EggLastMechAIStateBase {
  public:
    virtual void Enter(EggLastMech *mech, int arg);
    virtual void Tick(EggLastMech *mech);
    virtual void Exit(EggLastMech *mech);
    virtual int GetId() {
        return 0xC;
    }
};

class FeverShadow : public EggLastMechAIStateBase {
  public:
    virtual void Enter(EggLastMech *mech, int arg);
    virtual void Tick(EggLastMech *mech);
    virtual void Exit(EggLastMech *mech);
    virtual int GetId() {
        return 0xB;
    }
};

class FeverPawn : public EggLastMechAIStateBase {
  public:
    virtual void Enter(EggLastMech *mech, int arg);
    virtual void Tick(EggLastMech *mech);
    virtual void Exit(EggLastMech *mech);
    virtual int GetId() {
        return 0xA;
    }
};

class FeverBombFailed : public EggLastMechAIStateBase {
  public:
    virtual void Enter(EggLastMech *mech, int arg);
    virtual void Tick(EggLastMech *mech);
    virtual void Exit(EggLastMech *mech);
    virtual int GetId() {
        return 9;
    }
};

class FeverBomb : public EggLastMechAIStateBase {
  public:
    virtual void Enter(EggLastMech *mech, int arg);
    virtual void Tick(EggLastMech *mech);
    virtual void Exit(EggLastMech *mech);
    virtual int GetId() {
        return 8;
    }
};

class FeverMissile : public EggLastMechAIStateBase {
  public:
    virtual void Enter(EggLastMech *mech, int arg);
    virtual void Tick(EggLastMech *mech);
    virtual void Exit(EggLastMech *mech);
    virtual int GetId() {
        return 7;
    }
};

class Brake : public EggLastMechAIStateBase {
  public:
    virtual void Enter(EggLastMech *mech, int arg);
    virtual void Tick(EggLastMech *mech);
    virtual void Exit(EggLastMech *mech);
    virtual int GetId() {
        return 0xF;
    }

    /* 0x4 */ EggLastMechAIStateBase *m_next; /* the state to change to once stopped */
};

class Pursuit : public EggLastMechAIStateBase {
  public:
    virtual void Enter(EggLastMech *mech, int arg);
    virtual void Tick(EggLastMech *mech);
    virtual void Exit(EggLastMech *mech);
    virtual int GetId() {
        return 6;
    }

    /* 0x4 */ float m_speed;       /* the boss's speed before the pursuit */
    /* 0x8 */ float m_effectTimer; /* until the next effect */
};

class Flee : public EggLastMechAIStateBase {
  public:
    virtual void Enter(EggLastMech *mech, int arg);
    virtual void Tick(EggLastMech *mech);
    virtual void Exit(EggLastMech *mech);
    virtual int GetId() {
        return 5;
    }
};

class Idle : public EggLastMechAIStateBase {
  public:
    virtual void Enter(EggLastMech *mech, int arg);
    virtual void Tick(EggLastMech *mech);
    virtual void Exit(EggLastMech *mech);
    virtual int GetId() {
        return 2;
    }
};

class Start : public EggLastMechAIStateBase {
  public:
    virtual void Enter(EggLastMech *mech, int arg);
    virtual void Tick(EggLastMech *mech);
    virtual void Exit(EggLastMech *mech);
    virtual int GetId() {
        return 1;
    }
};

class Event : public EggLastMechAIStateBase {
  public:
    virtual void Enter(EggLastMech *mech, int arg);
    virtual void Tick(EggLastMech *mech);
    virtual void Exit(EggLastMech *mech);
    virtual int GetId() {
        return 4;
    }
};

class EventStart : public EggLastMechAIStateBase {
  public:
    virtual void Enter(EggLastMech *mech, int arg);
    virtual void Tick(EggLastMech *mech);
    virtual void Exit(EggLastMech *mech);
    virtual int GetId() {
        return 3;
    }

    /* 0x4 */ s32 m_event; /* the event to play once stopped */
};

class Nop : public EggLastMechAIStateBase {
  public:
    virtual void Enter(EggLastMech *mech, int arg);
    virtual void Tick(EggLastMech *mech);
    virtual void Exit(EggLastMech *mech);
    virtual int GetId() {
        return 0;
    }
};

} // namespace AIState

extern "C" {
/* The state singletons (names as in EggLastMech.cpp). */
EggLastMechAIStateBase *fn_8031017C(void); /* End */
EggLastMechAIStateBase *fn_80310314(void); /* BreakTime */
EggLastMechAIStateBase *fn_803104C0(void); /* FeverRing */
EggLastMechAIStateBase *fn_8031062C(void); /* FeverShadow */
EggLastMechAIStateBase *fn_803107FC(void); /* FeverPawn */
EggLastMechAIStateBase *fn_80310AC0(void); /* FeverBombFailed */
EggLastMechAIStateBase *fn_80310C9C(void); /* FeverBomb */
EggLastMechAIStateBase *fn_80310EA0(void); /* FeverMissile */
EggLastMechAIStateBase *fn_80311060(void); /* Brake */
EggLastMechAIStateBase *fn_80311378(void); /* Pursuit */
EggLastMechAIStateBase *fn_80311838(void); /* Flee */
EggLastMechAIStateBase *fn_80311AE4(void); /* Idle */
EggLastMechAIStateBase *fn_80311C5C(void); /* Start */
EggLastMechAIStateBase *fn_80312274(void); /* Event */
EggLastMechAIStateBase *fn_803123E8(void); /* EventStart */
EggLastMechAIStateBase *fn_803124BC(void); /* Nop */
}

/* Adds to the score (name inferred). */
static void AddScore() {
    fn_8016F178(fn_80079808()->m_score, 1, lbl_805F4BF0);
}

/* Starts the effect around the player (name inferred). */
static void StartPlayerEffect(boost::shared_ptr<CharInfo> &player) {
    UnkParam_803577D4 param;
    AssignVector(param.m_position, fn_80334918());
    param.m_value = 30;
    fn_803577D4(&param);
}

/* Starts the effect at the player (name inferred). */
static void StartPlayerEffect2(boost::shared_ptr<CharInfo> &player) {
    fn_801ED4B0(100, player.get(), s_effectRadius.value * s_effectRadius.value);
}

using namespace AIState;

extern "C" EggLastMechAIStateBase *fn_803124BC(void) {
    static Nop s_state;
    return &s_state;
}

void Nop::Enter(EggLastMech *mech, int arg) {}

void Nop::Tick(EggLastMech *mech) {}

void Nop::Exit(EggLastMech *mech) {}

extern "C" EggLastMechAIStateBase *fn_803123E8(void) {
    static EventStart s_state;
    return &s_state;
}

void EventStart::Enter(EggLastMech *mech, int arg) {
    mech->LockState();
    m_event = arg;
}

void EventStart::Tick(EggLastMech *mech) {
    if (mech->IsStopped()) {
        mech->ChangeState(fn_80312274(), m_event);
    }
}

void EventStart::Exit(EggLastMech *mech) {
    mech->UnlockState();
}

extern "C" EggLastMechAIStateBase *fn_80312274(void) {
    static Event s_state;
    return &s_state;
}

/* Starts event index: its motion and camera (name inferred). */
static void StartEvent(EggLastMech *mech, int index) {
    fn_80175EEC(fn_80074470(), 4, 0);
    mech->field_0x10C = index;
    mech->m_events.Set(index);
    const EventInfo *info = &s_events[index];
    fn_801A4898(mech->GetMotion(), info->m_motion, 0, 0.0f);
    fn_800091D0(&s_eventPosition, &mech->m_position);
    s_eventRotation.x = mech->m_rotation.x;
    s_eventRotation.y = mech->m_rotation.y;
    s_eventRotation.z = mech->m_rotation.z;
    if (info->m_keepPosition) {
        mech->m_partF4->Start(info->m_camera, &mech->m_position, &mech->m_rotation);
    } else {
        fn_8040CE60(&mech->m_position);
        fn_8040E970(&mech->m_rotation);
        fn_8040CE60(&mech->field_0x24);
        fn_8040CE60(&mech->field_0x18);
        mech->m_partF4->Start(info->m_camera, &lbl_805E07A0, &lbl_805E07B8);
    }
}

/* Ends the current event (name inferred). */
static void EndEvent(EggLastMech *mech) {
    const EventInfo *info = &s_events[mech->field_0x10C];
    if (!info->m_keepPosition) {
        fn_800091D0(&mech->m_position, &s_eventPosition);
        mech->m_rotation.x = s_eventRotation.x;
        mech->m_rotation.y = s_eventRotation.y;
        mech->m_rotation.z = s_eventRotation.z;
    }
    mech->m_partF4->End();
    fn_80175EEC(fn_80074470(), 3, 0);
}

/* Plays the current event's sound and hint (name inferred). */
static void PlayEventSound(EggLastMech *mech) {
    const EventInfo *info = &s_events[mech->field_0x10C];
    if (info->m_sound != 0xFFFF) {
        fn_80336B1C(mech->m_sound, info->m_sound);
    }
    if (info->m_hint != -1) {
        EnemyHint::ShowBossHint(info->m_hint);
    }
}

void Event::Enter(EggLastMech *mech, int arg) {
    mech->LockState();
    mech->m_step = 0;
    StartEvent(mech, arg);
}

void Event::Tick(EggLastMech *mech) {
    lbl_8057D6A0.field_0x1C = 1.0f;
    switch (mech->m_step) {
    case 0:
        PlayEventSound(mech);
        mech->m_step = 1;
        break;
    case 1:
        if (mech->m_partF4->HasEnded()) {
            int index = mech->field_0x10C;
            const EventInfo *info = &s_events[index];
            if (info->m_nextCamera < 0) {
                switch (index) {
                case 0:
                    mech->ChangeState(fn_80312274(), 6);
                    break;
                case 4:
                    if (mech->m_slot->m_line.m_shotType == 0) {
                        mech->ChangeState(fn_80310AC0(), 0);
                    } else {
                        mech->ChangeState(fn_80311378(), 0);
                    }
                    break;
                default:
                    mech->ChangeState(fn_80311AE4(), 0);
                    break;
                }
            } else {
                mech->m_partF4->End();
                {
                boost::shared_ptr<CharInfo> player = mech->GetPlayer().lock();
                if (player.get() == 0) {
                    mech->ChangeState(fn_80311AE4(), 0);
                    return;
                }
                fn_800A8DFC(fn_80074194());
                switch (mech->field_0x10C) {
                case 7: {
                    StartPlayerEffect(player);
                    Vec3f rotation(0.0f, fn_80016260(player->m_position.x, player->m_position.z), 0.0f);
                    rotation.y = fn_8040E99C(rotation.y - DegToRad(45.0f));
                    EggLastMechCamera *camera = mech->m_partF4;
                    camera->Start(info->m_nextCamera, &fn_80334918(), &rotation);
                    break;
                }
                case 3:
                    StartPlayerEffect2(player);
                    mech->m_partF4->Start(info->m_nextCamera, &player.get()->m_position,
                                &player.get()->m_rotation);
                    break;
                case 5:
                    AddScore();
                    mech->m_partF4->Start(info->m_nextCamera, &player.get()->m_position,
                                &player.get()->m_rotation);
                    break;
                default:
                    mech->ChangeState(fn_80311AE4(), 0);
                    return;
                }
                }
                mech->m_step = 2;
            }
        }
        break;
    case 2:
        if (mech->m_partF4->HasEnded()) {
            mech->ChangeState(fn_80311AE4(), 0);
        }
        break;
    }
}

void Event::Exit(EggLastMech *mech) {
    mech->UnlockState();
    EndEvent(mech);
}

extern "C" EggLastMechAIStateBase *fn_80311C5C(void) {
    static Start s_state;
    return &s_state;
}

void Start::Enter(EggLastMech *mech, int arg) {
    mech->LockState();
}

void Start::Tick(EggLastMech *mech) {
    if (*reinterpret_cast<s32 *>(reinterpret_cast<u8 *>(fn_80074470()) + 8) == 3) {
        fn_8035367C("csdFiles/BChara_EDealer", "bchara", "open");
        mech->ChangeState(fn_80312274(), 0);
        EnemyHint::ShowBossHint(-2);
    }
}

void Start::Exit(EggLastMech *mech) {
    mech->UnlockState();
}

extern "C" EggLastMechAIStateBase *fn_80311AE4(void) {
    static Idle s_state;
    return &s_state;
}

void Idle::Enter(EggLastMech *mech, int arg) {
    fn_801A4898(mech->GetMotion(), 0xC, 0, 30.0f);
    const UnkFloat *time = reinterpret_cast<const UnkFloat *>(arg);
    if (time != 0) {
        mech->GetTimer()->Set(time->value);
    } else {
        mech->GetTimer()->Set(1.0f);
    }
}

void Idle::Tick(EggLastMech *mech) {
    if (mech->GetTimer()->IsExpired()) {
        mech->ChangeState(fn_80311838(), 0);
    }
}

void Idle::Exit(EggLastMech *mech) {}

/* Plays the turning motions while the boss moves (name inferred). */
static void UpdateMoveMotion(EggLastMech *mech) {
    if (fn_8033496C(mech->m_partF8)) {
        if (fn_801A4494(mech->GetMotion()) == 0xA && fn_8040CF50(&mech->field_0x18) > 300.0f) {
            if (*reinterpret_cast<s32 *>(reinterpret_cast<u8 *>(mech->m_partF0) + 0x5C) == 1) {
                fn_801A4898(mech->GetMotion(), 0xF, 0, 10.0f);
            } else {
                fn_801A4898(mech->GetMotion(), 0xE, 0, 10.0f);
            }
        }
    } else if (fn_801A4494(mech->GetMotion()) == 0xE || fn_801A4494(mech->GetMotion()) == 0xF) {
        fn_801A4898(mech->GetMotion(), 0xA, 0, 10.0f);
    }
}

extern "C" EggLastMechAIStateBase *fn_80311838(void) {
    static Flee s_state;
    return &s_state;
}

void Flee::Enter(EggLastMech *mech, int arg) {
    UnkPart_8032D6FC *part = mech->m_partF0;
    fn_8032D674(part, &fn_80016284(&mech->GetPlayerOffset()));
    mech->GetTimer()->Set(fn_8040EC80(8.0f, 12.0f));
    mech->m_step = 0;
    if (mech->m_slot->m_result >= 0) {
        fn_80313C8C(mech->m_slot);
    }
    if (fn_803349A0(mech->m_partF8) <= 2) {
        mech->field_0x124 = 1;
    } else {
        mech->field_0x124 = 0;
    }
    if (mech->field_0x124) {
        fn_801A4898(mech->GetMotion(), 0xB, 0, 10.0f);
    } else {
        fn_801A4898(mech->GetMotion(), 0xA, 0, 10.0f);
    }
    mech->StopSound(0);
    mech->StopSound(3);
    fn_80336B58(mech->m_sound, 0x7F2F);
}

void Flee::Tick(EggLastMech *mech) {
    if (mech->field_0x105) {
        if (mech->GetSlot()->IsAllDifferent()) {
            mech->GetTimer()->Set(1.0f);
        } else {
            mech->GetTimer()->Set(fn_8040EC80(8.0f, 12.0f));
        }
    }
    if (mech->GetTimer()->IsExpired()) {
        UnkSlot_803142C0 *slot = mech->m_slot;
        fn_80314004(slot, fn_80313F48(slot), 1, -1);
        if (mech->GetSlot()->IsAllDifferent()) {
            mech->GetTimer()->Set(1.0f);
        } else {
            mech->GetTimer()->Set(10.0f);
            if (mech->GetSlot()->GetSpinMask()) {
                mech->ChangeState(fn_80311060(), (int)fn_80311AE4());
                return;
            }
        }
    }
    float ratio;
    if (mech->field_0x11C > 0.01f) {
        ratio = mech->field_0x118 / mech->field_0x11C;
    } else {
        ratio = 0.0f;
    }
    if (ratio < 0.7f && fn_803349A0(mech->m_partF8) >= 2 && mech->field_0x124) {
        mech->field_0x124 = 0;
        fn_801A4898(mech->GetMotion(), 0xA, 0, 10.0f);
    }
    UpdateMoveMotion(mech);
    Vec3f target;
    Vec3f offset;
    fn_8032D518(mech->m_partF0, &target, &offset);
    float speed;
    if (mech->field_0x124) {
        speed = 0.6f * mech->field_0xAC;
    } else {
        speed = mech->field_0xAC;
    }
    AssignVector(mech->field_0x24,
                 fn_8040CDA8(&fn_8040CEE8(&((VectorSubtractFn)fn_8040CD74)(&target, &mech->m_position)),
                             speed));
    fn_8040CE04(&mech->field_0x24, &mech->field_0x18);
    if (mech->field_0x124) {
        fn_8029EB5C(mech, &((VectorSubtractFn)fn_8040CD74)(&mech->m_position, &offset), 4.0f);
    } else {
        fn_8029EB5C(mech, &fn_8040CDD0(&mech->m_position, &offset), 4.0f);
    }
}

void Flee::Exit(EggLastMech *mech) {
    mech->PlaySound(0);
    mech->PlaySound(3);
}

extern "C" EggLastMechAIStateBase *fn_80311378(void) {
    static Pursuit s_state;
    return &s_state;
}

void Pursuit::Enter(EggLastMech *mech, int arg) {
    fn_801A4898(mech->GetMotion(), 0xA, 0, 30.0f);
    mech->field_0x124 = 0;
    mech->GetTimer()->Set(10.0f);
    UnkPart_8032D6FC *part = mech->m_partF0;
    fn_8032D674(part, (const Vec3f *)&mech->GetPlayerOffset());
    m_speed = mech->field_0xAC;
    mech->field_0xAC = 800.0f;
    m_effectTimer = 0.6f;
    mech->StopSound(0);
    fn_80336B58(mech->m_sound, 0x7F2F);
}

void Pursuit::Tick(EggLastMech *mech) {
    if (mech->GetTimer()->IsExpired()) {
        mech->ChangeState(fn_80311060(), (int)fn_80311AE4());
        return;
    }
    if ((mech->field_0x118 < 0.0f ? 0.0f : mech->field_0x118) > 1.0f) {
        if ((m_effectTimer -= fn_8019FFE0(mech)) < 0.0f) {
            m_effectTimer = 0.6f;
            mech->SpawnEffect(1);
        }
    }
    UpdateMoveMotion(mech);
    Vec3f target;
    Vec3f offset;
    fn_8032D518(mech->m_partF0, &target, &offset);
    float speed = mech->field_0xAC;
    AssignVector(mech->field_0x24,
                 fn_8040CDA8(&fn_8040CEE8(&((VectorSubtractFn)fn_8040CD74)(&target, &mech->m_position)),
                             speed));
    fn_8040CE04(&mech->field_0x24, &mech->field_0x18);
    fn_8029EB5C(mech, &fn_8040CDD0(&mech->m_position, &offset), -1.0f);
}

void Pursuit::Exit(EggLastMech *mech) {
    mech->field_0xAC = m_speed;
    mech->PlaySound(2);
    mech->PlaySound(0);
}

extern "C" EggLastMechAIStateBase *fn_80311060(void) {
    static Brake s_state;
    return &s_state;
}

void Brake::Enter(EggLastMech *mech, int arg) {
    m_next = reinterpret_cast<EggLastMechAIStateBase *>(arg);
    if (mech->field_0x124) {
        fn_801A4898(mech->GetMotion(), 0x15, 0, 10.0f);
    } else {
        fn_801A4898(mech->GetMotion(), 0x14, 0, 10.0f);
    }
    mech->StopSound(4);
    fn_80336B58(mech->m_sound, 0x7F30);
}

void Brake::Tick(EggLastMech *mech) {
    if (mech->IsStopped()) {
        mech->ChangeState(m_next, 0);
    }
}

void Brake::Exit(EggLastMech *mech) {
    mech->PlaySound(4);
}

extern "C" EggLastMechAIStateBase *fn_80310EA0(void) {
    static FeverMissile s_state;
    return &s_state;
}

void FeverMissile::Enter(EggLastMech *mech, int arg) {
    fn_801A4898(mech->GetMotion(), 0xC, 0, 10.0f);
    mech->m_step = 0;
}

void FeverMissile::Tick(EggLastMech *mech) {
    switch (mech->m_step) {
    case 0:
        if (mech->IsStopped()) {
            fn_801A4898(mech->GetMotion(), 6, 0, 10.0f);
            fn_80336B58(mech->m_sound, 0x7F3C);
            mech->m_step = 1;
        }
        break;
    case 1:
        if (fn_801A4810(mech->GetMotion())) {
            if (mech->m_slot->m_line.m_shotType == 0) {
                static UnkFloat s_time(2.5f);
                mech->ChangeState(fn_80311AE4(), (int)&s_time);
            } else {
                mech->ChangeState(fn_80311AE4(), 0);
            }
        }
        break;
    }
}

void FeverMissile::Exit(EggLastMech *mech) {}

extern "C" EggLastMechAIStateBase *fn_80310C9C(void) {
    static FeverBomb s_state;
    return &s_state;
}

void FeverBomb::Enter(EggLastMech *mech, int arg) {
    fn_801A4898(mech->GetMotion(), 0xC, 0, 10.0f);
    mech->m_step = 0;
}

void FeverBomb::Tick(EggLastMech *mech) {
    switch (mech->m_step) {
    case 0:
        if (mech->IsStopped()) {
            fn_801A4898(mech->GetMotion(), 4, 0, 10.0f);
            mech->m_step = 1;
        }
        break;
    case 1:
        if (fn_801A4810(mech->GetMotion())) {
            if (mech->m_slot->m_line.m_shotType == 0) {
                mech->ChangeState(fn_80310AC0(), 0);
            } else {
                mech->ChangeState(fn_80311378(), 0);
            }
        }
        break;
    }
}

void FeverBomb::Exit(EggLastMech *mech) {}

extern "C" EggLastMechAIStateBase *fn_80310AC0(void) {
    static FeverBombFailed s_state;
    return &s_state;
}

void FeverBombFailed::Enter(EggLastMech *mech, int arg) {
    fn_801A4898(mech->GetMotion(), 0x11, 0, 10.0f);
    mech->GetTimer()->Set(0.8333333f);
    mech->m_step = 0;
}

void FeverBombFailed::Tick(EggLastMech *mech) {
    switch (mech->m_step) {
    case 0:
        if (mech->GetTimer()->IsExpired()) {
            fn_80336B58(mech->m_sound, 0x7F37);
            mech->StopSound(1);
            mech->m_step = 1;
        }
        break;
    case 1:
        if (fn_801A4810(mech->GetMotion())) {
            mech->SpawnEffect(0);
            fn_801A4898(mech->GetMotion(), 0x12, 0, 10.0f);
            mech->GetTimer()->Set(2.0f);
            mech->m_step = 2;
        }
        break;
    case 2:
        if (mech->GetTimer()->IsExpired()) {
            mech->PlaySound(1);
            fn_801A4898(mech->GetMotion(), 0x13, 0, 10.0f);
            mech->m_step = 3;
        }
        break;
    case 3:
        if (fn_801A4810(mech->GetMotion())) {
            mech->ChangeState(fn_80311AE4(), 0);
        }
        break;
    }
}

void FeverBombFailed::Exit(EggLastMech *mech) {
    mech->PlaySound(1);
}

extern "C" EggLastMechAIStateBase *fn_803107FC(void) {
    static FeverPawn s_state;
    return &s_state;
}

void FeverPawn::Enter(EggLastMech *mech, int arg) {
    fn_801A4898(mech->GetMotion(), 8, 0, 30.0f);
    fn_80336B58(mech->m_sound, 0x7F3F);
}

void FeverPawn::Tick(EggLastMech *mech) {
    if (fn_801A4810(mech->GetMotion())) {
        boost::shared_ptr<CharInfo> player = mech->GetPlayer().lock();
        if (player.get() != 0) {
            StartPlayerEffect2(player);
            fn_80336B58(mech->m_sound, 0x7F40);
        }
        mech->ChangeState(fn_80311AE4(), 0);
    }
}

void FeverPawn::Exit(EggLastMech *mech) {}

extern "C" EggLastMechAIStateBase *fn_8031062C(void) {
    static FeverShadow s_state;
    return &s_state;
}

void FeverShadow::Enter(EggLastMech *mech, int arg) {
    fn_80336B58(mech->m_sound, 0x7F47);
    fn_801A4898(mech->GetMotion(), 8, 0, 30.0f);
}

void FeverShadow::Tick(EggLastMech *mech) {
    if (fn_801A4810(mech->GetMotion())) {
        AddScore();
        mech->ChangeState(fn_80311AE4(), 0);
    }
}

void FeverShadow::Exit(EggLastMech *mech) {}

extern "C" EggLastMechAIStateBase *fn_803104C0(void) {
    static FeverRing s_state;
    return &s_state;
}

void FeverRing::Enter(EggLastMech *mech, int arg) {
    fn_801A4898(mech->GetMotion(), 8, 0, 30.0f);
}

void FeverRing::Tick(EggLastMech *mech) {
    if (fn_801A4810(mech->GetMotion())) {
        boost::shared_ptr<CharInfo> player = mech->GetPlayer().lock();
        if (player.get() != 0) {
            StartPlayerEffect(player);
        }
        mech->ChangeState(fn_80311AE4(), 0);
    }
}

void FeverRing::Exit(EggLastMech *mech) {}

extern "C" EggLastMechAIStateBase *fn_80310314(void) {
    static BreakTime s_state;
    return &s_state;
}

void BreakTime::Enter(EggLastMech *mech, int arg) {
    fn_801A4898(mech->GetMotion(), 0xC, 0, 30.0f);
    mech->GetTimer()->Set(1.0f);
}

void BreakTime::Tick(EggLastMech *mech) {
    if (mech->GetTimer()->IsExpired()) {
        mech->GetPartF8()->GetObject14()->Stop();
        mech->ChangeState(fn_80311838(), 0);
    }
}

void BreakTime::Exit(EggLastMech *mech) {}

extern "C" EggLastMechAIStateBase *fn_8031017C(void) {
    static End s_state;
    return &s_state;
}

void End::Enter(EggLastMech *mech, int arg) {
    mech->LockState();
    mech->m_step = 0;
    StartEvent(mech, 2);
    PlayEventSound(mech);
}

void End::Tick(EggLastMech *mech) {
    switch (mech->m_step) {
    case 0:
        if (mech->m_partF4->GetTimeLeft() < 180.0f) {
            fn_8032D970();
            fn_80175EEC(fn_80074470(), 6, 0);
            mech->m_step = 1;
        }
        break;
    case 1:
        break;
    }
}

void End::Exit(EggLastMech *mech) {
    mech->UnlockState();
}

End::~End() {}

int End::GetId() {
    return 0xE;
}

} // namespace EggLastMech
