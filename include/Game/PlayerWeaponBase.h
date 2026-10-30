#ifndef GAME_PLAYERWEAPONBASE_H
#define GAME_PLAYERWEAPONBASE_H

#include <boost/function.hpp>

#include "Game/MathTypes.h"
#include "Game/Player.h"
#include "Game/Task.h"
#include "boost/shared_ptr.hpp"
#include "boost/weak_ptr.hpp"
#include "size_assert.h"

namespace Weapon {
class WeaponBase;
}

struct _stMtnPlayWork;

/* Target (class name from RTTI): see Game/CharCommand.h. */
class Target;

#ifdef PLAYER_MOTION_MEMBERS
namespace Effect {
namespace System {
class EffectElement;
} // namespace System
} // namespace Effect

/* The flag word at 0x60 of a player's motion (struct name unknown; as BirdFlags in
 * Game/BirdBase.h). */
extern "C" void fn_80014E04(u32 *flags);
struct UnkMotionFlags_800AAA30 {
    UnkMotionFlags_800AAA30() {
        fn_80014E04(&m_bits);
    }

    u32 m_bits;
};

/* A clump's colour fade (struct name unknown; as in Game/Diablon.h); its constructor is out of
 * line (0x80027D5C). */
struct UnkClumpFade_80027D5C;
extern "C" UnkClumpFade_80027D5C *fn_80027D5C(UnkClumpFade_80027D5C *fade);
struct UnkClumpFade_80027D5C {
    UnkClumpFade_80027D5C() {
        fn_80027D5C(this);
    }

    /* 0x00 */ float m_color[4];
    /* 0x10 */ u8 field_0x10[0x24 - 0x10];
    /* 0x24 */ u8 m_mode;
    /* 0x25 */ u8 field_0x25[0x2C - 0x25];
};
#endif

#ifdef PLAYER_MOTION_SLOT_ARGS
/* What slot 0x1C of a motion gets (see Game/PlayerShadowMotionHand.cpp). */
struct UnkHandMotion_800B07FC;
#endif

namespace Player {

/* What a character's motion passes to an inverse-kinematics callback (class name from the RTTI of
 * Game/PlayerShadowWeapon.cpp's bound callback; layout and member names inferred from that
 * callback): the node being solved, whose frame comes first, and the point the node should aim
 * at, which the callback writes. */
struct UnkIKNode {
    /* 0x0 */ void *m_frame;
};

struct IKEventArgs {
    /* 0x0 */ UnkIKNode *m_node;
    /* 0x4 */ Vec3f *m_target;
};

typedef boost::function1<void, IKEventArgs &> IKCallback;

/* A node of a motion's skeleton as PlayerMotion keeps it (0x38 bytes; the same entries as
 * UnkSkeletonNode_800AA760 in Game/NpcPartModel.h, which fn_800AA760 returns; struct name unknown):
 * the node's matrix, and whether the node was found in the motion (names inferred from
 * Player::Npc::NpcMotion::BindNodes). */
struct UnkMotionNode_800AA760 {
#ifdef PLAYER_MOTION_MEMBERS
    /* The members as Game/PlayerMotion.cpp keeps them (names inferred): the node's position,
     * front and up vectors, how far it moved in the frame, and whether it is kept up to date. */
    UnkMotionNode_800AA760() {}
    void Update();

    /* 0x00 */ const Mtx34f *m_matrix;
    /* 0x04 */ Vec3f m_position;
    /* 0x10 */ Vec3f m_front;
    /* 0x1C */ Vec3f m_up;
    /* 0x28 */ Vec3f m_move;
    /* 0x34 */ u8 m_valid;
    /* 0x35 */ u8 m_used;
#else
    /* 0x00 */ const Mtx34f *m_matrix;
    /* 0x04 */ u8 field_0x4[0x34 - 0x4];
    /* 0x34 */ u8 m_valid;
#endif
};

/* What vf_0x14 hands the clumps to (class name unknown; fn_8006CAE4 adds one). */
class UnkClumpList_8006CAE4;

/* PlayerMotion (class name from RTTI; vtable 0x80521A84): the animation of a player character,
 * a Task kept at 0x230 of the player. Only the slots the weapon code calls are named (names and
 * meanings inferred from those callers). */
class PlayerMotion : public Task {
  public:
    /* Constructor 0x800AAA30, destructor 0x800AA94C. */
    PlayerMotion(Task *parent, const boost::shared_ptr<PlayerBase> &player);
    virtual ~PlayerMotion();
    virtual void Tick(float dt);
    /* Plays motion, blending over blendTime (meaning inferred from Player::Behavior::FirstMeet,
     * which passes 0x2E and 0). */
    virtual void vf_0x10(int motion, float blendTime) = 0;
    /* Hands the motion's clumps to a list (argument type unknown; meaning inferred from
     * Player::Npc::EggMonitor::MotionImpl, Game/NpcEggMonitorMotion.cpp). */
    virtual void vf_0x14(UnkClumpList_8006CAE4 *list) = 0;
    /* Selects the pose for a held weapon (0: none; the value comes from the weapon type). */
    virtual void SetWeaponPose(int pose);
#ifdef PLAYER_MOTION_SLOT_ARGS
    /* Slots 0x1C, 0x2C-0x38 with the arguments Player::Shadow::ShadowMotion gives them
     * (Game/PlayerShadowMotion.cpp): a hand motion played with a blend, an inverse-kinematics
     * slot's weight (set and read), the hands' motion works, and the eyes' blink frame (meanings
     * inferred). The units that emit PlayerMotion's vtables name the symbols without them. */
    virtual void vf_0x1C(const UnkHandMotion_800B07FC *motion, float blendTime);
#else
    virtual void vf_0x1C();
#endif
    virtual void vf_0x20(int arg);
#ifdef PLAYER_MOTION_HAND_MATRIX
    /* Slot 0x24 with the arguments its caller passes: a hand (2 or 3) and the matrix it is placed
     * at (meaning inferred from Game/PlayerBehaviorDriveVehicle.cpp); the units that emit
     * PlayerMotion's vtables name the symbol without them. */
    virtual void vf_0x24(int hand, const Mtx34f *matrix);
#else
    virtual void vf_0x24();
#endif
    /* Installs the inverse-kinematics callback of a slot. */
    virtual void SetIKCallback(int slot, const IKCallback &callback);
    /* Slots 0x2C-0x38 (bodies at 0x8006C0A4, 0x8006C0A8, 0x800AA748, 0x800AA740; names unknown). */
#ifdef PLAYER_MOTION_SLOT_ARGS
    virtual void vf_0x2C(int slot, float weight);
    virtual float vf_0x30(int slot);
    virtual void vf_0x34(_stMtnPlayWork **left, _stMtnPlayWork **right);
    virtual int vf_0x38();
#else
    virtual void vf_0x2C();
    virtual float vf_0x30();
    virtual void vf_0x34();
    virtual void vf_0x38();
#endif

    /* Members as Player::Npc::NpcMotion (Game/NpcMotion.cpp) and its subclasses use them (names
     * and meanings inferred). */
#ifdef PLAYER_MOTION_MEMBERS
    /* The members as Game/PlayerMotion.cpp keeps them (names inferred): the effects that follow
     * the model, and the player. */
    /* 0x028 */ boost::weak_ptr<Effect::System::EffectElement> m_effects[6];
    /* 0x058 */ boost::shared_ptr<PlayerBase> m_player;
    /* 0x060 */ UnkMotionFlags_800AAA30 m_flags; /* bit 2: animating, bit 3: hidden */
#else
    /* 0x028 */ u8 field_0x28[0x58 - 0x28];
    /* 0x058 */ boost::shared_ptr<PlayerBase> m_player; /* (Game/NpcSonicMotion.cpp) */
    /* 0x060 */ u32 m_flags; /* a fn_80014E04 flag word (bit 2: animating, bit 3: hidden) */
#endif
    /* 0x064 */ u8 field_0x64[0x70 - 0x64];
    /* 0x070 */ Mtx34f m_bodyMatrix; /* the model's matrix with its offset */
    /* 0x0A0 */ Mtx34f m_baseMatrix; /* the character's matrix */
    /* 0x0D0 */ UnkMotionNode_800AA760 m_nodes[14];
    /* 0x3E0 */ s32 m_motionBase; /* id of the model's first motion */
    /* 0x3E4 */ u32 field_0x3E4;
    /* 0x3E8 */ _stMtnPlayWork *m_work;
    /* 0x3EC */ s32 m_blinkCount;
#ifdef PLAYER_MOTION_MEMBERS
    /* 0x3F0 */ s32 m_fadeMode; /* the colour fade's mode when no weapon asks for its own */
    /* 0x3F4 */ UnkClumpFade_80027D5C m_fade;
#else
    /* 0x3F0 */ u8 field_0x3F0[0x420 - 0x3F0];
#endif
    /* 0x420 */ s32 m_nextMotion;
    /* 0x424 */ s32 m_nextMotionArg;
    /* 0x428 */ s32 m_prevMotionId;
    /* 0x42C */ s32 m_motionId;
    /* 0x430 */ float m_blendTime;
    /* 0x434 */ float m_startFrame;
    /* 0x438 */ u32 field_0x438;
    /* 0x43C */ float m_speed;
    /* 0x440 */ s32 m_playingMotion;
    /* 0x444 */ u8 field_0x444[0x44C - 0x444];
    /* 0x44C */ float m_frameTime;
    /* 0x450 */ s32 m_step;
    /* 0x454 */ u32 field_0x454;
};

/* A position returned through the game's out-of-line vector copy constructor (fn_800091EC), as
 * PlayerTargetBase's position slots build it. It has no base class, so that its inline
 * constructor calls no other inline function and is inlined; callers read it as a Vec3f (as
 * ThrowObjectPosition in Game/ThrowObject.h; name invented). */
struct AimPosition {
    AimPosition(const Vec3f &position) {
        fn_800091EC(reinterpret_cast<Vec3f *>(this), &position);
    }

    float x;
    float y;
    float z;
};

/* PlayerTargetBase (class name from RTTI; vtable 0x80543DF8): the aim of a player character,
 * kept at 0x24C of the player (slot names inferred from Game/PlayerShadowWeapon.cpp). Its own
 * code is Game/PlayerTargetBase.cpp; the return types of slots 0x10, 0x1C, 0x20 and 0x28 come
 * from its bodies there. */
class PlayerTargetBase {
  public:
    PlayerTargetBase(const boost::shared_ptr<PlayerBase> &player);
    virtual ~PlayerTargetBase();
    /* Moves the player's own target along and turns the aim (name inferred). */
    virtual void vf_0x0C();
#ifdef PLAYER_TARGET_BOOL_SLOT_10
    /* Slot 0x10 as Game/PlayerShadowMotion.cpp tests it (a bool; the base's body returns 0). */
    virtual bool vf_0x10();
#else
    virtual int vf_0x10();
#endif
    /* Whether a target is locked on, its position, and the target (behaviour inferred from
     * Game/PlayerShadowWeaponGun.cpp). */
    virtual bool HasTarget();
#ifdef PLAYER_TARGET_SLOT_18_ARGS
    /* Slot 0x18 with the arguments Player::Shadow::PlayerTarget takes
     * (Game/PlayerShadowTarget.cpp): a position to aim at for a time (meanings inferred). The
     * units that emit PlayerTargetBase's vtables name the symbol without them. */
    virtual void vf_0x18(const Vec3f &position, float time);
#else
    virtual void vf_0x18();
#endif
    virtual AimPosition vf_0x1C();
    virtual AimPosition GetTargetPosition();
    virtual boost::weak_ptr<Target> GetTarget();
    virtual boost::weak_ptr<Target> vf_0x28();
    /* Turns a vector into the aim's frame (behaviour inferred). */
    virtual Vec3f ToAimSpace(const Vec3f &vector);

    /* The turn rate (0x80206AD4; name inferred). */
    float GetTurn();

    /* 0x04 */ boost::shared_ptr<PlayerBase> m_player;
    /* 0x0C */ boost::shared_ptr<Target> m_target; /* the player's own target (inferred) */
    /* 0x14 */ float m_turn; /* eased towards the stick's turn each frame (name inferred) */
};

/* WeaponBase (class name from RTTI; vtable 0x805223DC): the weapon slot of a player character, a
 * Task that holds the Weapon::WeaponBase the player carries. The constructor (0x800B8690)
 * registers it at 0x238 of the player, the destructor (0x800B861C) removes it. Slots 0x14 and
 * 0x18 have bodies of their own (0x800ADDD8, 0x800813B8); the others are pure. Slot names are
 * inferred from Player::Shadow::ShadowWeapon (Game/PlayerShadowWeapon.cpp). */
class WeaponBase : public Task {
  public:
    WeaponBase(Task *parent, const boost::shared_ptr<PlayerBase> &player);
    virtual ~WeaponBase();
    virtual void Tick(float dt) = 0;
    /* Gives the player a weapon of a type with ammo rounds (-1: the type's default). */
    virtual void GiveWeaponType(int type, int ammo) = 0;
    /* Gives the player a weapon (ammo as above). */
#ifdef PLAYER_WEAPON_BASE_GIVE_WEAPON_INLINE
    /* Defined in the class (empty): the game's copy at 0x800ADDD8 is the one emitted by
     * Game/PlayerShadow.cpp, the first unit to call the slot. Only defined there (see GetWeapon
     * below). */
    virtual void GiveWeapon(const boost::shared_ptr<Weapon::WeaponBase> &weapon, int ammo) {}
#else
    virtual void GiveWeapon(const boost::shared_ptr<Weapon::WeaponBase> &weapon, int ammo);
#endif
#ifdef PLAYER_WEAPON_BASE_GET_WEAPON_INLINE
    /* Defined in the class: the game's copy at 0x800813B8 is the one emitted by
     * Game/PlayerBehaviorGrind.cpp, the first unit to call the slot. Only defined there
     * (PLAYER_WEAPON_BASE_GET_WEAPON_INLINE): a virtual call emits a copy, which the game does not
     * keep elsewhere. */
    virtual boost::shared_ptr<Weapon::WeaponBase> GetWeapon() {
        return boost::shared_ptr<Weapon::WeaponBase>();
    }
#else
    virtual boost::shared_ptr<Weapon::WeaponBase> GetWeapon();
#endif
    virtual int GetWeaponType() = 0;
    virtual int GetWeaponKind() = 0;
    virtual float GetWeaponValue() = 0;
    virtual void BeginUse() = 0;
    virtual void BeginAltUse() = 0;
    virtual void EndUse() = 0;
    virtual void DropWeapon() = 0;
    virtual void DiscardWeapon() = 0;
    virtual void HideWeapon() = 0;
    virtual void ShowWeapon() = 0;
    virtual bool CanUse() = 0;
    virtual bool IsFlag2() = 0;
    virtual bool IsFiring() = 0;
    virtual bool IsFlag4() = 0;

    /* 0x28 */ boost::shared_ptr<PlayerBase> m_player;
};
SIZE_ASSERT(WeaponBase, 0x30);

} // namespace Player

#endif
