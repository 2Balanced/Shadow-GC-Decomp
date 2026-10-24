/* Player::Behavior::ChaosBlast (class name from RTTI): behavior 0x1C of Shadow, the Chaos Blast.
 * Entering it plays the charge motion, listens to the player's motion events and starts an effect;
 * the player floats up until the motion sends event 0x5000, then it stops, starts the blast effect
 * and a camera shake, takes points off the score and grows the blast for 1.5 seconds. It switches
 * to behavior 2 once the player lands. File name inferred from the class (as
 * Game/PlayerBehaviorSuper.cpp); all other names are inferred. Definitions are in reverse address
 * order (deferred inlining emits functions last-defined first). */
/* Declares Player::Behavior::Behavior's slots with their arguments (see Game/PlayerBehavior.h). */
#define PLAYER_BEHAVIOR_SLOT_ARGS
#include <boost/bind.hpp>
#include <boost/function.hpp>

#include <cmath>

#include "Effect/System/EffectElement.h"
#include "Game/CharCommand.h"
#include "Game/MathTypes.h"
#include "Game/Player.h"
#include "Game/PlayerBehavior.h"
#include "Game/PlayerMotionEvent.h"
#include "Game/PlayerSoundBase.h"
#include "Game/PlayerWeaponBase.h"
#include "Game/Vibration.h"
#include "boost/shared_ptr.hpp"
#include "boost/weak_ptr.hpp"
#include <dolphin/types.h>
#include "size_assert.h"

using Effect::System::EffectElement;

/* The camera manager (see Game/AdjustCosRot.cpp). */
class UnkCameraManager_80009548;

/* The save data (see Game/BlackBullCtrlUnit.cpp): the record at 0x70C keeps the score. */
struct UnkObject_80079808 {
    /* 0x000 */ u8 field_0x0[0x70C];
    /* 0x70C */ u8 m_score[1];
};

extern "C" {
/* A 32-bit flag word: fn_80014E04 constructs it cleared, fn_80014E10 clears it, fn_80014E1C sets
 * bit and fn_80014E34 tests bit (see Game/BAMine.cpp). */
void fn_80014E04(u32 *flags);
void fn_80014E10(u32 *flags);
void fn_80014E1C(u32 *flags, int bit);
bool fn_80014E34(u32 *flags, int bit);
/* The player's state bits (see Game/PlayerShadowSuper.cpp). */
bool fn_8006CD78(u32 *bits, int bit);
void fn_80076CA8(u32 *bits, int bit);
void fn_80076CC8(u32 *bits, int bit);
/* Counts the player's requests for state bit 0x17, and per player index for the word at 0x280
 * (behaviour from their bodies; meanings unknown). */
void fn_8007939C(Player::PlayerBase *player, bool on);
void fn_80078F2C(Player::PlayerBase *player, int index, bool on);
/* Called with the player's object at 0x234 (behaviour unknown). */
void fn_80077908(void *object, float scale);
void fn_8007792C(void *object);
void fn_80077950(void *object, int arg);
/* The player's up axis (behaviour inferred). */
Vec3f fn_80079698(Player::PlayerBase *player);
/* Player updates run before the behavior's own (behaviour unknown; as in FirstMeet). */
void fn_80079720(Player::PlayerBase *player);
void fn_8009728C(boost::shared_ptr<Player::PlayerBase> *player, int arg1);
/* Tests bit 3 of the player's word at 0xB4 (meaning unknown; as in AirAttack). */
bool fn_800A9E1C(boost::shared_ptr<Player::PlayerBase> *player);
/* Switches a player's behavior (behaviour inferred). */
void fn_800A7F14(Player::Behavior::UnkBehaviorExecuter_800A7F14 *executer,
                 const Player::Behavior::InitializeArgs *args);
/* Starts effect id of the player's effects at 0x23C, at the player (behaviour inferred), and a
 * call on them when the behavior starts (meaning unknown). */
boost::weak_ptr<EffectElement> fn_800A27D4(void *effects, Player::PlayerBase *player,
                                           Vec3f *velocity, int id);
void fn_800A2D7C(void *effects, int arg);
/* The camera manager, and shaking the camera and ending it (behaviour inferred; see
 * Game/CityBombBig.cpp). */
UnkCameraManager_80009548 *fn_80009548(void);
void fn_8000D664(UnkCameraManager_80009548 *manager, int arg1, float arg2, float arg3,
                 float arg4);
void fn_8000D534(UnkCameraManager_80009548 *manager, int arg1);
/* Plays and stops a jingle (see Game/ActStageController.cpp). */
void fn_801D3888(int jingle);
void fn_801D3838(int jingle);
/* The save data, and adding points to a score (see Game/BlackBullCtrlUnit.cpp). */
UnkObject_80079808 *fn_80079808(void);
void fn_8016F0CC(void *score, int arg1, int points);
/* Rumbles the controller (see Game/VibrationRequest.cpp). */
boost::weak_ptr<Vibration::RequestHandle> fn_8034DE74(int pattern, int pad, float duration);
/* Marks the commands a chaos behavior takes as handled (in Game/PlayerBehaviorChaosControl.cpp's
 * range; behaviour from its body). */
void fn_8007C810(CharCommand *command);

void fn_8040CE60(Vec3f *vector);                     /* vector = 0 */
Vec3f fn_800091A4(const Vec3f *vector, float scale); /* vector * scale */
void fn_8040CFF8(Vec3f *vector, const Vec3f *value); /* vector += value */
}

inline void AddVector(Vec3f &vector, const Vec3f &value) {
    fn_8040CFF8(&vector, &value);
}

/* The blend from a to b (name inferred). */
inline float Lerp(float a, float b, float t) {
    return t * (b - a) + a;
}

/* x to the power y (an out-of-line copy in the game, as an inline function that calls another
 * inline function is; name inferred). */
inline float Pow(float x, float y) {
    return std::pow(x, y);
}

namespace Player {
namespace Behavior {

/* The behavior's flag word (struct name unknown): bit 0 is set by the motion's event 0x5000. */
struct ChaosBlastFlags {
    ChaosBlastFlags() {
        fn_80014E04(&m_bits);
    }
    void Set(int bit) {
        fn_80014E1C(&m_bits, bit);
    }
    bool Test(int bit) {
        return fn_80014E34(&m_bits, bit);
    }
    void Reset() {
        fn_80014E10(&m_bits);
    }

    u32 m_bits;
};

class ChaosBlast : public Behavior {
  public:
    ChaosBlast(const boost::shared_ptr<PlayerBase> &player);
    virtual ~ChaosBlast();
    virtual bool vf_0x0C(int prevId, const InitializeArgs &args);
    virtual void Enter(int prevId, const InitializeArgs &args);
    virtual void Leave(int nextId);
    virtual void vf_0x18(CharCommand *command);
    virtual void Update(float dt);

    /* The factory record's functions (names inferred). */
    static Behavior *Create(const boost::shared_ptr<PlayerBase> &player);
    static void Release();

    void OnMotion(MotionEventArgs &args);

    static const char *s_name;

    /* 0x1C */ s32 m_state; /* 0 start, 1 rising, 2 blasting, 3 landed */
    /* 0x20 */ ChaosBlastFlags m_flags;
    /* 0x24 */ float m_scale; /* the blast's size */
    /* 0x28 */ float m_time;
};
SIZE_ASSERT(ChaosBlast, 0x2C);

const char *ChaosBlast::s_name = "ChaosBlast";

/* The blast's size at its start and its end (names inferred). The game sets them in __sinit rather
 * than in .sdata, as an initializer that is not a constant expression does; the inline call
 * reproduces that (as in Game/PlayerBehaviorSuper.cpp). */
inline float DynamicFloat(float value) {
    return value;
}
static float s_minScale = DynamicFloat(10.0f);
static float s_maxScale = DynamicFloat(200.0f);

ChaosBlast::ChaosBlast(const boost::shared_ptr<PlayerBase> &player)
    : Behavior(player, s_name, 0x1C) {}

ChaosBlast::~ChaosBlast() {}

bool ChaosBlast::vf_0x0C(int prevId, const InitializeArgs &args) {
    if (!fn_8006CD78(m_player.get()->m_stateBits, 0x34)) {
        return true;
    }
    m_player.get()->m_sound->StopVoice(0x1B);
    return false;
}

void ChaosBlast::Enter(int prevId, const InitializeArgs &args) {
    fn_80076CC8(m_player.get()->m_stateBits, 2);
    fn_80076CA8(m_player.get()->m_stateBits, 0x16);
    fn_8007939C(m_player.get(), 1);
    m_player.get()->m_motion->vf_0x10(0x24, 0.0f);
    PlayerMotionEventCallback(m_player.get()) =
        MotionEventCallback(boost::bind(&ChaosBlast::OnMotion, this, _1));
    fn_800A2D7C(reinterpret_cast<void *>(m_player.get()->field_0x23C), 0);
    fn_8040CE60(&m_player.get()->field_0x18);
    m_state = 0;
    m_flags.Reset();
    fn_801D3888(0);
    fn_8034DE74(7, m_player.get()->field_0x54, 3.0f);
    fn_80078F2C(m_player.get(), m_player.get()->field_0x54, 1);
}

void ChaosBlast::Leave(int nextId) {
    fn_80076CC8(m_player.get()->m_stateBits, 0x16);
    fn_8007939C(m_player.get(), 0);
    fn_80076CC8(m_player.get()->m_stateBits, 0x35);
    fn_8007792C(reinterpret_cast<void *>(m_player.get()->field_0x234));
    PlayerMotionEventCallback(m_player.get()).Clear();
    fn_8000D534(fn_80009548(), 0);
    fn_801D3838(0);
    fn_80078F2C(m_player.get(), m_player.get()->field_0x54, 0);
}

void ChaosBlast::vf_0x18(CharCommand *command) {
    fn_8007C810(command);
}

void ChaosBlast::Update(float dt) {
    switch (m_state) {
    case 0:
        fn_800A27D4(reinterpret_cast<void *>(m_player.get()->field_0x23C), m_player.get(),
                    &m_player.get()->field_0x18, 0x25);
        m_state = 1;
        break;
    case 1:
        if (m_flags.Test(0)) {
            fn_8040CE60(&m_player.get()->field_0x18);
            fn_800A27D4(reinterpret_cast<void *>(m_player.get()->field_0x23C), m_player.get(),
                        &m_player.get()->field_0x18, 0x45);
            fn_80076CA8(m_player.get()->m_stateBits, 0x35);
            fn_80077950(reinterpret_cast<void *>(m_player.get()->field_0x234), 10);
            m_scale = s_minScale;
            m_time = 0.0f;
            fn_8000D664(fn_80009548(), 0, -1.0f, 2.0f, 15.0f);
            fn_8016F0CC(fn_80079808()->m_score, 1, -10000);
            m_state = 2;
        } else {
            AddVector(m_player->field_0x24,
                      fn_800091A4(&(const Vec3f &)fn_80079698(m_player.operator->()), 12.5f));
        }
        break;
    case 2:
        m_time += dt;
        if (m_time >= 1.5f) {
            m_time = 1.5f;
        }
        m_scale = Lerp(s_minScale, s_maxScale, Pow(m_time / 1.5f, 0.75f));
        fn_80077908(reinterpret_cast<void *>(m_player.get()->field_0x234), m_scale);
        break;
    case 3:
        fn_800A7F14(m_player.get()->m_behaviors, &(const InitializeArgs &)InitializeArgs(2));
        return;
    }
    fn_80079720(m_player.get());
    fn_8009728C(&m_player, 0);
    if (fn_800A9E1C(&m_player)) {
        m_state = 3;
    }
}

void ChaosBlast::OnMotion(MotionEventArgs &args) {
    if (args.m_event == 0x5000) {
        m_flags.Set(0);
    }
}

Behavior *ChaosBlast::Create(const boost::shared_ptr<PlayerBase> &player) {
    return new ChaosBlast(player);
}

void ChaosBlast::Release() {}

} // namespace Behavior
} // namespace Player

/* The record the behavior factory table finds ChaosBlast by (keeps its lbl_ name). */
extern const Player::Behavior::UnkBehaviorDesc_804EC024 lbl_804AF894 = {
    Player::Behavior::ChaosBlast::Create, Player::Behavior::ChaosBlast::Release, 0};
