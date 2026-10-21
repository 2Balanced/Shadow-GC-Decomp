/* Player::Shadow::ShadowWeapon (class name from RTTI): Shadow's weapon slot. It keeps the weapon
 * Shadow carries, picks a ControlWeaponBase for the weapon's kind (gun, cannon, lock-on, stick or
 * vacuum; the controls are the units after this one), shows the ammo in the player's record,
 * aims the arm at the weapon's sight through an inverse-kinematics callback, and throws the
 * weapon away when asked. File name inferred from the class and the control units' file names
 * (PlayerShadowWeaponCannon.cpp, ...). Method and member names are inferred. Definitions are in
 * reverse address order (deferred inlining emits functions last-defined first).
 *
 * Where a shared_ptr's operator-> (or Weapon::WeaponBase::TypeId) is called out of line in retail,
 * the expression is an argument of an inline call (AddVector) or feeds an inline getter returning
 * int; retail's direct fn_800091D0 calls are written as such so that the pointers stay inline.
 *
 * Not yet matching: GiveWeapon (99.4%) keeps the given weapon's type in r4 and the held one's in r0
 * where retail has them the other way round; about 30 source shapes did not change it. Every other
 * function and all data match, and with the unit linked only those three instructions differ. */
#include <boost/bind.hpp>
#include <boost/function.hpp>

#include "Game/CharInfo.h"
#include "Game/MathConstants.h"
#include "Game/MathTypes.h"
#include "Game/NpcPartModel.h"
#include "Game/Player.h"
#include "Game/PlayerShadowWeapon.h"
#include "Game/PlayerWeaponBase.h"
#include "Game/VectorDot.h"
#include "Game/Vibration.h"
#include "Game/WeaponBase.h"
#include "boost/shared_ptr.hpp"
#include "boost/weak_ptr.hpp"

/* The player's record in the game state (struct names unknown; see SavePoint.cpp). */
struct UnkObject_80079808;
struct UnkRecord_80178B5C {
    /* 0x00 */ u8 field_0x0[0x1C];
    /* 0x1C */ s32 m_ammo;
};
struct UnkSoundManager_80053164;

extern "C" {
/* The game state and the record of player index (behaviour inferred). */
UnkObject_80079808 *fn_80079808(void);
UnkRecord_80178B5C *fn_80178B5C(UnkObject_80079808 *data, int player);
/* Sets the weapon kind and the weapon type the record shows (behaviour inferred). */
void fn_8016EEE8(UnkRecord_80178B5C *record, int kind);
void fn_8016EEE0(UnkRecord_80178B5C *record, int type);
/* Reads, adds to (clamped to 0..999) and sets an ammo count. */
s32 fn_8016EF30(s32 *ammo);
s32 fn_8016EF38(s32 *ammo, int count);
void fn_8016EF74(s32 *ammo, int count);

/* A bit array: test, set and clear bit. */
bool fn_8006CD78(u32 *bits, int bit);
void fn_80076CA8(u32 *bits, int bit);
void fn_80076CC8(u32 *bits, int bit);

/* PlayerMotion helpers (behaviour inferred): lowers or raises the hands, the attachment point of
 * held weapons, and skeleton node index. */
void fn_800AA6C0(Player::PlayerMotion *motion, bool raise);
void *fn_800AA584(Player::PlayerMotion *motion);
/* The aim's yaw (PlayerTargetBase, 0x80206AD4). */
float fn_80206AD4(Player::PlayerTargetBase *target);
/* Entry index of a player's character slots. */
boost::weak_ptr<CharInfo> fn_800AADA4(Player::UnkCharaSlots_800AADA4 *slots, int index);
/* A player's direction turned into the world, and its forward direction (behaviour inferred). */
Vec3f fn_80079628(Player::PlayerBase *player, const Vec3f *direction);
Vec3f fn_80079698(Player::PlayerBase *player);

/* The weapon table (behaviour inferred): a new weapon of type for owner, the default ammo, the
 * hold pose, the sight angle, the pick-up sound, the recoil and the rumble pattern of a type. */
boost::shared_ptr<Weapon::WeaponBase> fn_8005BBF4(const boost::weak_ptr<CharInfo> &owner, int type);
int fn_8005B9AC(int type);
int fn_8005B984(int type);
float fn_8005B92C(int type);
u16 fn_8005B954(int type);
float fn_8005B904(int type);
int fn_8005B834(int type);
/* Copies a weapon's position and rotation out (Weapon::WeaponBase; either may be null). */
void fn_8005CF90(Weapon::WeaponBase *weapon, Vec3f *position, Vec3f *rotation);
/* Throws a weapon of type type (see ScatteringWeapon.cpp). */
void fn_80221B08(const Vec3f *from, const Vec3f *position, const Vec3f *direction,
                 const Vec3f *velocity, int type, int arg);

/* Sound and rumble. */
UnkSoundManager_80053164 *fn_80053164(void);
int fn_801D560C(UnkSoundManager_80053164 *manager, int id, int arg2, int arg3);
boost::weak_ptr<Vibration::RequestHandle> fn_8034DF48(int pattern, int arg1);
float fn_8040EE90(void); /* a random value in [0, 1) */

/* Matrix and vector helpers (behaviour inferred). */
void fn_80016370(Mtx34f *matrix, void *frame);
Vec3f fn_8040FB08(const Mtx34f *matrix);
Vec3f fn_8040D0AC(const Vec3f *vector, float angle);
Vec3f fn_8040F364(const Vec3f *forward, const Vec3f *up);
Vec3f fn_800091A4(const Vec3f *vector, float scale);
Vec3f fn_80093230(const Vec3f *direction);
Vec3f fn_8040CEE8(const Vec3f *vector);
Vec3f fn_8040CDA8(const Vec3f *vector, float scale);
void fn_8040CE38(Vec3f *vector, float scale);
void fn_8040CE60(Vec3f *vector);
void fn_8040CFF8(Vec3f *vector, const Vec3f *value); /* vector += value */
}

typedef Vec3f (*VectorSubtractFn)(const Vec3f *lhs, const Vec3f *rhs);

/* vector = value through the game's out-of-line Vec3f assignment (name inferred). */
inline void AssignVector(Vec3f &vector, const Vec3f &value) {
    fn_800091D0(&vector, &value);
}

/* vector += value (name inferred). */
inline void AddVector(Vec3f &vector, const Vec3f &value) {
    fn_8040CFF8(&vector, &value);
}

/* The recoil a weapon gives at most, also the direction it pushes in (name inferred). */
static Vec3f s_recoilLimit(0.0f, 0.0f, 3.0f);

namespace Player {
namespace Shadow {

ShadowWeapon::ShadowWeapon(Task *parent, const boost::shared_ptr<PlayerBase> &player)
    : WeaponBase(parent, player) {
    m_name = "ShadowWeapon";
    m_control = 0;
    m_flags.Clear();
    fn_8040CE60(&m_recoil);
    m_sightAngle = 0.0f;
}

ShadowWeapon::~ShadowWeapon() {
    RemoveWeapon();
}

void ShadowWeapon::Tick(float) {
    float dt = **reinterpret_cast<float *const *>(m_player->field_0x60);
    m_flags.Clear(3);
    m_flags.Clear(4);
    if (m_weapon) {
        UpdateTransform();
        m_weapon->Tick(dt);
        if (m_control) {
            m_control->Update(dt);
        }
    }
    if (m_weapon && IsFiring()) {
        Vec3f direction;
        fn_800091EC(&direction,
                    &(const Vec3f &)fn_8040D0AC(&s_recoilLimit, fn_80206AD4(m_player->m_target)));
        AddVector(m_recoil, fn_800091A4(&direction, m_weapon->vf_0x24()));
        AddVector(m_recoil, fn_800091A4(&direction, fn_8005B904(m_weapon->TypeId())));
        int pattern = fn_8005B834(GetWeaponType());
        if (pattern != -1 && m_player->field_0x54 != -1) {
            fn_8034DF48(pattern, m_player->field_0x54);
        }
        float limit = fn_8040CF2C(&s_recoilLimit);
        if (fn_8040CF2C(&m_recoil) > limit) {
            float length = fn_8040CF50(&s_recoilLimit);
            AssignVector(m_recoil, fn_8040CDA8(&(const Vec3f &)fn_8040CEE8(&m_recoil), length));
        }
    }
    fn_8040CE38(&m_recoil, 0.859375f);
}

void ShadowWeapon::GiveWeaponType(int type, int ammo) {
    u16 sound = fn_8005B954(type);
    if (sound != 0xFFFF) {
        fn_801D560C(fn_80053164(), sound, 0, 0);
    }
    if (m_weapon) {
        if (type == m_weapon->m_type) {
            if (ammo < 0) {
                ammo = fn_8005B9AC(type);
            }
            AddAmmo(ammo);
            return;
        }
        RemoveWeapon();
    }
    SetWeaponType(type, ammo);
}

void ShadowWeapon::GiveWeapon(const boost::shared_ptr<Weapon::WeaponBase> &weapon, int ammo) {
    int type = weapon->m_type;
    if (m_weapon) {
        if (type == m_weapon->m_type) {
            if (ammo < 0) {
                ammo = fn_8005B9AC(m_weapon->m_type);
            }
            AddAmmo(ammo);
            return;
        }
        RemoveWeapon();
    }
    SetWeapon(weapon, ammo);
}

boost::shared_ptr<Weapon::WeaponBase> ShadowWeapon::GetWeapon() {
    return m_weapon;
}

int ShadowWeapon::GetWeaponType() {
    if (m_weapon) {
        return m_weapon->TypeId();
    }
    return 0;
}

int ShadowWeapon::GetWeaponKind() {
    if (m_weapon) {
        return m_weapon->Kind();
    }
    return 0;
}

float ShadowWeapon::GetWeaponValue() {
    if (m_weapon) {
        return m_weapon->vf_0x1C();
    }
    return 0.0f;
}

void ShadowWeapon::BeginUse() {
    m_flags.Set(0);
    m_flags.Clear(1);
    fn_80076CA8(m_player->m_stateBits, 0x19);
}

void ShadowWeapon::BeginAltUse() {
    m_flags.Set(1);
    m_flags.Clear(0);
    fn_80076CC8(m_player->m_stateBits, 0x19);
}

void ShadowWeapon::EndUse() {
    m_flags.Clear(0);
    fn_80076CC8(m_player->m_stateBits, 0x19);
}

void ShadowWeapon::DropWeapon() {
    if (!fn_8006CD78(m_player->m_stateBits, 0x33)) {
        m_flags.Clear(0);
        fn_80076CC8(m_player->m_stateBits, 0x19);
        ThrowWeapon();
    }
}

void ShadowWeapon::DiscardWeapon() {
    m_flags.Clear(0);
    fn_80076CC8(m_player->m_stateBits, 0x19);
    RemoveWeapon();
}

void ShadowWeapon::HideWeapon() {
    if (m_weapon) {
        m_weapon->Hide();
    }
}

void ShadowWeapon::ShowWeapon() {
    if (m_weapon) {
        m_weapon->Show();
    }
}

bool ShadowWeapon::CanUse() {
    return true;
}

bool ShadowWeapon::IsFlag2() {
    return m_flags.Test(2);
}

bool ShadowWeapon::IsFiring() {
    return m_flags.Test(3);
}

bool ShadowWeapon::IsFlag4() {
    return m_flags.Test(4);
}

void ShadowWeapon::ThrowWeapon() {
    if (m_weapon) {
        Vec3f position;
        Vec3f velocity;
        Vec3f rotation(0.0f, 0.0f, 0.0f);
        fn_8005CF90(m_weapon.get(), &position, &rotation);
        if (fn_8006CD78(m_player->m_stateBits, 0x10)) {
            fn_800091D0(&velocity, &Vec3f(-8.0f, 20.0f, 15.0f));
        } else {
            fn_800091D0(&velocity, &Vec3f(0.0f, 20.0f, 10.0f));
        }
        fn_800091D0(&velocity, &(const Vec3f &)fn_80079628(m_player.get(), &velocity));
        Vec3f forward;
        fn_800091EC(&forward, &(const Vec3f &)fn_80079698(m_player.get()));
        Vec3f from;
        fn_800091EC(&from, &m_player->m_position);
        Vec3f offset;
        fn_800091EC(&offset, &(const Vec3f &)((VectorSubtractFn)fn_8040CD74)(&position, &from));
        DotProduct(offset, forward);
        AddVector(from, fn_80093230(&forward));
        int type = m_weapon->m_type;
        fn_80221B08(&from, &position, &rotation, &velocity, type, GetAmmo());
        if (GetAmmo() == 0 && fn_8040EE90() <= 0.2f) {
            fn_801D560C(fn_80053164(), 0x3025, 0, 0);
        }
        m_player->field_0x98 = 0.5f;
        RemoveWeapon();
    }
}

void ShadowWeapon::RaiseArm() {
    if ((!m_weapon || !m_weapon->vf_0x2C()) && m_player->m_motion) {
        fn_800AA6C0(m_player->m_motion, true);
    }
}

void ShadowWeapon::LowerArm() {
    if ((!m_weapon || !m_weapon->vf_0x2C()) && m_player->m_motion) {
        fn_800AA6C0(m_player->m_motion, false);
    }
}

int ShadowWeapon::GetAmmo() {
    UnkRecord_80178B5C *record = fn_80178B5C(fn_80079808(), m_player->field_0x54);
    if (record) {
        return fn_8016EF30(&record->m_ammo);
    }
    return 0;
}

void ShadowWeapon::SetAmmo(int count) {
    UnkRecord_80178B5C *record = fn_80178B5C(fn_80079808(), m_player->field_0x54);
    if (record) {
        fn_8016EF74(&record->m_ammo, count);
    }
}

void ShadowWeapon::AddAmmo(int count) {
    UnkRecord_80178B5C *record = fn_80178B5C(fn_80079808(), m_player->field_0x54);
    if (record) {
        if (count > 0 || !fn_8006CD78(m_player->m_stateBits, 0x3F)) {
            fn_8016EF38(&record->m_ammo, count);
        }
    }
}

bool ShadowWeapon::HasAmmo() {
    if (fn_8006CD78(m_player->m_stateBits, 0x3F)) {
        return true;
    }
    UnkRecord_80178B5C *record = fn_80178B5C(fn_80079808(), m_player->field_0x54);
    if (record) {
        return fn_8016EF30(&record->m_ammo) > 0;
    }
    return false;
}

void ShadowWeapon::UpdateTransform() {
    UnkSkeletonNode_800AA760 *node = fn_800AA760(m_player->m_motion, 5);
    Vec3f position;
    fn_800091EC(&position, &node->m_position);
    Vec3f rotation = (const Vec3f &)fn_8040F364(&node->m_forward, &node->m_up);
    m_weapon->SetTransform(&position, &rotation);
}

void ShadowWeapon::SetWeapon(const boost::shared_ptr<Weapon::WeaponBase> &weapon, int ammo) {
    m_weapon = weapon;
    if (m_weapon) {
        if (ammo < 0) {
            ammo = fn_8005B9AC(m_weapon->m_type);
        }
        SetAmmo(ammo);
    } else {
        SetAmmo(0);
    }
    m_flags.Clear(0);
    m_flags.Clear(2);
    if (m_weapon) {
        switch (m_weapon->Kind()) {
        case 1:
        case 6:
            m_control = CreateControlGun(this, m_weapon);
            break;
        case 2:
            m_control = CreateControlCannon(this, m_weapon);
            break;
        case 3:
            m_control = CreateControlLockon(this, m_weapon);
            break;
        case 5:
            m_control = CreateControlStick(this, m_weapon);
            break;
        case 4:
            m_control = CreateControlVacuum(this, m_weapon);
            break;
        }
        if (m_weapon->vf_0x2C()) {
            fn_800AA6C0(m_player->m_motion, true);
        }
        Weapon::UnkWeaponModel_80063428 *model = m_weapon->m_model;
        if (model && m_player->m_motion) {
            model->fn_8006310C(fn_800AA584(m_player->m_motion));
        }
        m_player->m_motion->SetWeaponPose(fn_8005B984(m_weapon->m_type));
        m_player->m_motion->SetIKCallback(1, IKCallback(boost::bind(&ShadowWeapon::OnIK, this, _1)));
        fn_800091D0(&m_sight, &(const Vec3f &)m_weapon->vf_0x20());
        m_sightAngle = 90.0f * fn_8005B92C(m_weapon->m_type);
        fn_80076CA8(m_player->m_stateBits, 0x1F);
        if (!fn_8006CD78(m_player->m_stateBits, 0x24)) {
            UnkRecord_80178B5C *record = fn_80178B5C(fn_80079808(), m_player->field_0x54);
            if (record) {
                fn_8016EEE8(record, m_weapon->field_0x18);
                fn_8016EEE0(record, m_weapon->m_type);
            }
        }
        if (m_player->IsWeaponHidden()) {
            HideWeapon();
        }
    }
}

void ShadowWeapon::SetWeaponType(int type, int ammo) {
    SetWeapon(fn_8005BBF4(fn_800AADA4(m_player->m_charaSlots, 1), type), ammo);
}

void ShadowWeapon::RemoveWeapon() {
    if (m_weapon) {
        m_weapon.reset();
    }
    if (m_control) {
        delete m_control;
    }
    m_control = 0;
    if (m_player->m_motion) {
        m_player->m_motion->SetWeaponPose(0);
        m_player->m_motion->vf_0x20(1);
        fn_800AA6C0(m_player->m_motion, false);
    }
    fn_80076CC8(m_player->m_stateBits, 0x1F);
    UnkRecord_80178B5C *record = fn_80178B5C(fn_80079808(), m_player->field_0x54);
    if (record) {
        fn_8016EEE8(record, 0);
        fn_8016EEE0(record, 0);
    }
}

void ShadowWeapon::OnIK(IKEventArgs &args) {
    Mtx34f frame;
    fn_80016370(&frame, args.m_node->m_frame);
    float angle = m_sightAngle * (lbl_805FAD38 / 180.0f);
    Vec3f target;
    fn_800091D0(&target,
                &(const Vec3f &)fn_8040D0AC(&m_sight, angle + fn_80206AD4(m_player->m_target)));
    fn_800091D0(&target, &(const Vec3f &)m_player->m_target->ToAimSpace(target));
    AddVector(target, m_player->m_target->ToAimSpace(m_recoil));
    AddVector(target, fn_8040FB08(&frame));
    fn_800091D0(args.m_target, &target);
}

WeaponBase *ShadowWeapon::Create(Task *parent, const boost::shared_ptr<PlayerBase> &player) {
    return new ShadowWeapon(parent, player);
}

} // namespace Shadow
} // namespace Player
