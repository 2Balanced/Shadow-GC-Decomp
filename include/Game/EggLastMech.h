#ifndef GAME_EGGLASTMECH_H
#define GAME_EGGLASTMECH_H

/* The classes of Game/EggLastMech.cpp that Game/EggLastMechTask.cpp also uses. A unit that
 * includes this header defines BOOST_WEAK_PTR_CONVERTING_ASSIGNMENT first when it needs weak_ptr's
 * converting assignment (see boost/weak_ptr.hpp). */

#include "Game/CameraModeUser.h"
#include "Game/CharCommand.h"
#include "Game/CharaColli.h"
#include "Game/CharaColliAttack.h"
#include "Game/EnemyCharInfo.h"
#include "Game/EnemyRender.h"
#include "Game/ExternalCameraRequest.h"
#include "Game/Gadget.h"
#include "Game/GadgetSound.h"
#include "Game/PathCtrl.h"
#include "Game/Task.h"
#include "Game/WeaponBullet.h"
#include "Game/MathTypes.h"
#include "boost/enable_shared_from_this.hpp"
#include "boost/shared_ptr.hpp"
#include "boost/weak_ptr.hpp"
#include <dolphin/types.h>
#include "size_assert.h"

/* A 32-bit flag word: fn_80014E04 clears it, fn_80014E1C sets bit and fn_80014E34 tests bit (see
 * CityBombBig.cpp). */
extern "C" {
void fn_80014E04(u32 *flags);
void fn_80014E1C(u32 *flags, int bit);
bool fn_80014E34(u32 *flags, int bit);
}

struct EggLastMechFlags {
    EggLastMechFlags() {
        fn_80014E04(&m_bits);
    }
    void Set(int bit) {
        fn_80014E1C(&m_bits, bit);
    }
    bool Test(int bit) {
        return fn_80014E34(&m_bits, bit);
    }

    u32 m_bits;
};

namespace EggLastMech {

class EggLastMech;

/* EggLastMechAIStateBase (class name from RTTI; vtable at 0x8055BBC0): a state of the AI. The
 * states are singletons of the classes in EggLastMech::AIState (Game/EggLastMechAIState.cpp; their
 * getters are declared in EggLastMech.cpp). The vtable holds the destructor and four pure virtual
 * functions. Virtual function names are inferred. */
class EggLastMechAIStateBase {
  public:
    /* Inline, defined in Game/EggLastMechAIState.cpp (a body here would renumber the @N labels of
     * every unit that includes this header). */
    inline virtual ~EggLastMechAIStateBase();
    virtual void Enter(EggLastMech *mech, int arg) = 0;
    virtual void Tick(EggLastMech *mech) = 0;
    virtual void Exit(EggLastMech *mech) = 0;
    virtual int GetId() = 0;
};

/* The AI state machine (class name unknown; layout from its allocation and use here). */
class EggLastMechAI {
  public:
    EggLastMechAI(EggLastMech *mech) : m_mech(mech), m_state(0), m_prevState(0), m_lock(0), m_busy(0) {}
    /* Declared: retail's delete keeps the null test that an empty destructor leaves. */
    ~EggLastMechAI() {}

    /* Updates the current state (name inferred). */
    void Tick() {
        if (m_state != 0) {
            m_busy = 1;
            m_state->Tick(m_mech);
            m_busy = 0;
        }
    }
    /* Whether the current state's id is id (name inferred). */
    bool IsState(int id) {
        if (m_state != 0 && m_state->GetId() == id) {
            return true;
        }
        return false;
    }

    void ChangeState(EggLastMechAIStateBase *state, int arg);

    /* 0x00 */ EggLastMech *m_mech;
    /* 0x04 */ EggLastMechAIStateBase *m_state;
    /* 0x08 */ EggLastMechAIStateBase *m_prevState;
    /* 0x0C */ s32 m_lock; /* while nonzero, states change only from inside an update */
    /* 0x10 */ u8 m_busy;
};
SIZE_ASSERT(EggLastMechAI, 0x14);

/* The helper objects defined in other units (class names unknown; sizes from their allocations).
 * They take the EggLastMech they serve. */
/* A reel of the slot machine (class name from the RTTI of its attack callback's bind_t; declared in
 * Game/EggLastMechSlotSwitch.h) and the slot machine's display (class name from the RTTI of its
 * shared_ptr's control block; Game/EggLastMechSlotDisp2D.cpp). */
class SlotSwitch;
class EggLastMechSlotDisp2D;

/* The symbols the slot machine's three reels stopped on (class name unknown; layout from its
 * constructor, 0x803141C4; names inferred). */
class UnkSlotLine_803141C4 {
  public:
    UnkSlotLine_803141C4();
    /* Whether the three symbols all differ (0x80314190; name inferred). */
    bool IsAllDifferent();

    /* Clears the symbols (name inferred; defined in Game/EggLastMechSlot.cpp, as are the inline
     * functions below whose bodies would renumber the compiler's @N labels of every unit). */
    inline void Reset();

    /* Whether the three symbols are the same, remembering a match (name inferred). */
    inline bool IsMatch();

    /* 0x00 */ u8 m_matched;
    /* 0x04 */ s32 m_symbols[3]; /* -1 while a reel spins */
    /* 0x10 */ s32 m_shotType;
};
SIZE_ASSERT(UnkSlotLine_803141C4, 0x14);

class UnkSlot_803142C0 {
  public:
    UnkSlot_803142C0(EggLastMech *mech);
    ~UnkSlot_803142C0();

    /* The reels still spinning, one bit each (name inferred). */
    u32 GetSpinMask() {
        return m_spinMask;
    }
    /* Whether the stopped symbols all differ (name inferred). */
    bool IsAllDifferent() {
        return m_line.IsAllDifferent();
    }

    /* 0x00 */ EggLastMech *m_mech;
    /* 0x04 */ s32 m_result; /* the slot result, or negative while none (meaning inferred) */
    /* 0x08 */ UnkSlotLine_803141C4 m_line;
    /* 0x1C */ float m_timer;
    /* 0x20 */ s32 m_state;
    /* 0x24 */ SlotSwitch *m_reels[3];
    /* 0x30 */ boost::shared_ptr<EggLastMechSlotDisp2D> m_disp;
    /* 0x38 */ UnkSoundHandle_801D3DC8 m_sound;
    /* 0x54 */ u32 m_spinMask;
    /* 0x58 */ u32 m_symbolMask;     /* the symbols a reel may still stop on (meaning inferred) */
    /* 0x5C */ u32 m_lineMasks[2];   /* the same per shot type (meaning inferred) */
    /* 0x64 */ s32 m_nextSymbols[2]; /* the symbol each shot type stops on next */
};
SIZE_ASSERT(UnkSlot_803142C0, 0x6C);

/* The path the boss follows (class name unknown; Game/EggLastMechPart.cpp). */
class UnkPart_8032D6FC {
  public:
    UnkPart_8032D6FC(EggLastMech *mech);
    ~UnkPart_8032D6FC();

    /* 0x00 */ EggLastMech *m_mech;
    /* 0x04 */ SonicteamUSA::System::PathCtrl m_path;
    /* 0x5C */ s32 m_direction; /* 1 forward (meaning inferred) */
};
SIZE_ASSERT(UnkPart_8032D6FC, 0x60);

/* The camera control object of EggLastMechCamera (class name unknown; size from its allocation;
 * its functions are at 0x8031A0A0-0x8031A268). */
class UnkCameraCtrl_8031A268 {
  public:
    UnkCameraCtrl_8031A268(const boost::weak_ptr<CharInfo> &player, const boost::weak_ptr<CharInfo> &owner);

    /* Sets the pitch limits (name inferred). */
    void SetPitchLimits(float min, float max);

    /* 0x0 */ u32 field_0x0;
};

class CameraTask;

/* EggLastMechCamera (class name from the RTTI of its bound callbacks): the boss's event camera, in
 * Game/EggLastMechCamera.cpp. Member names are inferred. */
class EggLastMechCamera {
  public:
    EggLastMechCamera(EggLastMech *mech);
    ~EggLastMechCamera();

    /* The time left of the current mode, whether it finished, ending it, and starting mode at
     * position and rotation. */
    float GetTimeLeft();
    bool HasEnded();
    void End();
    void Start(int mode, const Vec3f *position, const Vec3f *rotation);
    void Tick(float dt);
    bool IsPlayerAway();
    void OnSwitch(SonicteamUSA::System::Camera::CameraModeUser::SwitchEventArgs &args);
    void OnUpdate(SonicteamUSA::System::Camera::CameraModeUser::UpdateEventArgs &args);
    void Apply();

    /* 0x00 */ EggLastMech *m_mech;
    /* 0x04 */ UnkCameraCtrl_8031A268 *m_ctrl;
    /* 0x08 */ CameraTask *m_task;
    /* 0x0C */ boost::weak_ptr<PJSCamera::ExternalCameraRequest> m_handle;
    /* 0x14 */ boost::shared_ptr<SonicteamUSA::System::Camera::CameraModeUser> m_mode;
    /* 0x1C */ UnkCameraData_80012510 *m_data;
    /* 0x20 */ float m_time;
    /* 0x24 */ u32 field_0x24;
    /* 0x28 */ Mtx34f m_matrix;
    /* 0x58 */ bool m_finished;
    /* 0x59 */ u8 m_playerAway;
    /* 0x5A */ u8 m_paused;
    /* 0x5C */ float m_awayDistanceSquared;
};
SIZE_ASSERT(EggLastMechCamera, 0x60);

/* The object at 0x14 of UnkPart_80334C78 (class name unknown); Stop calls fn_801ED5A8 on it
 * (name inferred). */
class UnkObject_801ED5A8;
extern "C" void fn_801ED5A8(UnkObject_801ED5A8 *object);

class UnkObject_801ED5A8 {
  public:
    void Stop() {
        fn_801ED5A8(this);
    }
};

class UnkPart_80334C78 {
  public:
    UnkPart_80334C78(EggLastMech *mech);
    ~UnkPart_80334C78();

    /* The object at 0x14 (name inferred). */
    UnkObject_801ED5A8 *GetObject14() {
        return m_object14;
    }

    /* 0x00 */ EggLastMech *m_mech;
    /* 0x04 */ boost::weak_ptr<CharInfo> m_characters[2]; /* the player and the boss (inferred) */
    /* The area each character is in, (1 + row) * 10 + 1 + column of a 3 by 3 grid on the arena
     * floor, or -1, its previous area, and the time it has been in the area (Game/EggLastMechArea.cpp;
     * names inferred). m_object14 overlaps the player's area: GetObject14's caller passes it to
     * fn_801ED5A8, which takes it as a byte. */
    union {
        /* 0x14 */ UnkObject_801ED5A8 *m_object14;
        /* 0x14 */ s32 m_area[2];
    };
    /* 0x1C */ s32 m_lastArea[2];
    /* 0x24 */ float m_areaTime[2];
};
SIZE_ASSERT(UnkPart_80334C78, 0x2C);

class UnkSound_80336F48 {
  public:
    UnkSound_80336F48(EggLastMech *mech);
    ~UnkSound_80336F48();

    /* 0x00 */ u8 field_0x0[0x54];
};
SIZE_ASSERT(UnkSound_80336F48, 0x54);

/* A countdown kept at 0x118 of EggLastMech: the time left and the time it started from (names
 * inferred). */
struct EggLastMechTimer {
    /* (Defined in Game/EggLastMechAIState.cpp.) */
    inline bool IsExpired();
    inline void Set(float time);

    /* 0x0 */ float m_time;
    /* 0x4 */ float m_duration;
};

/* EggLastMech::EggLastMech (class name from RTTI; vtable at 0x8055A6FC). */
class EggLastMech : public EnemyCharInfo, public boost::enable_shared_from_this<EggLastMech> {
  public:
    EggLastMech(Task *task);
    virtual ~EggLastMech();

    /* Creates the boss for task (name inferred). */
    static boost::shared_ptr<EggLastMech> Instantiate(Task *task);

    /* A weak pointer to this boss (name inferred). */
    boost::weak_ptr<EggLastMech> GetWeakPtr() {
        return shared_from_this();
    }
    /* A shared pointer to this boss (name inferred). */
    boost::shared_ptr<EggLastMech> GetSharedPtr() {
        return shared_from_this();
    }
    /* The player character (name inferred). */
    boost::weak_ptr<CharInfo> GetPlayer() {
        return m_player;
    }

    /* The countdown at 0x118 (name inferred). */
    EggLastMechTimer *GetTimer() {
        return reinterpret_cast<EggLastMechTimer *>(&field_0x118);
    }
    /* The helper objects (names inferred). */
    UnkPart_80334C78 *GetPartF8() {
        return m_partF8;
    }
    UnkSlot_803142C0 *GetSlot() {
        return m_slot;
    }

    void Init(UnkPlacement_800C9CD0 *owner);
    void OnSlotStop();
    void CheckDeath();
    void Tick(float dt);
    void ChangeState(EggLastMechAIStateBase *state, int arg);
    void LockState();
    void UnlockState();
    void StartFever(int result);
    bool IsReady();
    void OnCommand(CharCommand &command);
    void SetupCommand();
    void SetupRender();
    void SetupCollider();
    void OnAttackHit(SonicteamUSA::System::CharaColli::AttackCallbackParam &param);
    void OnAttackEnd(SonicteamUSA::System::CharaColli::AttackCallbackParam &param);
    void OnAttacked(SonicteamUSA::System::CharaColli::AttackCallbackParam &param);
    void OnDamage(SonicteamUSA::System::CharaColli::AttackCallbackParam &param);
    void OnMotionEvent(EnemyMotionEventArg &arg);
    void PlaySound(int arg);
    void StopSound(int arg);
    void *GetMotion();
    void SetMotion(int arg);
    Fn8040CopyVector3 GetPlayerOffset();
    void Fire(int type, int index);
    void SpawnEffect(int arg);
    bool IsStopped();

    /* 0xBC: the enable_shared_from_this base */
    /* 0xC4 */ EggLastMechFlags m_flags; /* bit 0: dead */
    /* 0xC8 */ boost::shared_ptr<EnemyRender> m_render;
    /* 0xD0 */ boost::shared_ptr<SonicteamUSA::System::CharaColli::CharaColliBase> m_collider;
    /* 0xD8 */ boost::shared_ptr<Target> m_target;
    /* 0xE0 */ boost::weak_ptr<CharInfo> m_player;
    /* 0xE8 */ EggLastMechAI *m_ai;
    /* 0xEC */ UnkSlot_803142C0 *m_slot;
    /* 0xF0 */ UnkPart_8032D6FC *m_partF0;
    /* 0xF4 */ EggLastMechCamera *m_partF4;
    /* 0xF8 */ UnkPart_80334C78 *m_partF8;
    /* 0xFC */ UnkSound_80336F48 *m_sound;
    /* 0x100 */ Task *m_task;
    /* 0x104 */ u8 m_hintShown;
    /* 0x105 */ u8 field_0x105;
    /* 0x108 */ float m_health;
    /* 0x10C */ s32 field_0x10C;
    /* 0x110 */ EggLastMechFlags m_events; /* bits set by the fever states (meaning inferred) */
    /* 0x114 */ float field_0x114;
    /* 0x118 */ float field_0x118;
    /* 0x11C */ float field_0x11C;
    /* 0x120 */ s32 m_step; /* the step within the current AI state (name inferred) */
    /* 0x124 */ u8 field_0x124;
    /* 0x125 */ u8 field_0x125[0x128 - 0x125];
};
SIZE_ASSERT(EggLastMech, 0x128);

} // namespace EggLastMech

#endif
