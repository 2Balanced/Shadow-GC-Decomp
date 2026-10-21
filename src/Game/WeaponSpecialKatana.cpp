/* Katana (class name from RTTI: @unnamed@WeaponSpecialKatana_cpp@::Katana): a sword that draws a
 * trail while it swings and fires a shot straight back from its owner when the swing starts outside
 * the base's blocking state. Registered as two weapons, Katana01 (type 0x39, reach 4) and Katana02
 * (type 0x3A, reach 8). File name from the RTTI name. Method names marked "(name inferred)" are not
 * original. Definitions are in reverse address order (deferred inlining emits functions last-defined
 * first). */
#include "Game/CharInfo.h"
#include "Game/EffLocusMultiVtx.h"
#include "Game/GadgetUtil.h"
#include "Game/WeaponBase.h"
#include "Game/WeaponBullet.h"
#include "Game/WeaponItem.h"
#include "Game/WeaponSpecial.h"
#include "Game/WeaponStickBase.h"
#include "boost/shared_ptr.hpp"
#include "boost/weak_ptr.hpp"
#include "size_assert.h"

using Weapon::ShotInfo;
using Weapon::UnkWeaponEffect_80063428;
using Weapon::UnkWeaponEffectSet_80063428;
using Weapon::UnkWeaponModel_80063428;

extern "C" {
/* Loads resource id (behaviour inferred). */
void fn_8006B50C(int id, int arg1);
/* The bullet types' records (behaviour inferred; see WeaponLaserRifle.cpp): fires bullet type type
 * with info, and runs its set-up and clean-up. */
void fn_80051C34(int type, const ShotInfo &info);
void fn_80051C80(int type);
void fn_80051CC8(int type);
/* Sets up the pick-up item of weapon type type (behaviour inferred). */
void fn_8005B760(int type);
/* The translation of a matrix. */
Vec3f fn_8040FB08(const Mtx34f *matrix);
/* m rotated by the Euler angles in order (behaviour inferred). */
Mtx34f fn_8040FFBC(const Mtx34f *m, const Vec3f *angles, int order);
/* Returns its argument (an empty conversion; meaning inferred). */
int fn_801DBD00(int value);
/* Plays sound id at position (behaviour inferred). */
void fn_801D54F4(void *sound, u16 id, const Vec3f *position, int arg3, int arg4);
/* Whether task and all its parents are still running (behaviour inferred). */
bool fn_8004EAC0(Task *task);
/* Builds a weapon's straight shot from a matrix (behaviour inferred). */
ShotInfo fn_80069540(Weapon::WeaponBase *weapon, const Mtx34f *matrix);
}

namespace {

class Katana : public Weapon::WeaponStickBase {
  public:
    Katana(const boost::weak_ptr<CharInfo> &owner, int type, float reach,
           const UnkWeaponEffectSet_80063428 *effects)
        : Weapon::WeaponStickBase(owner, type, reach) {
        m_hidden = false;
        m_model = new UnkWeaponModel_80063428(owner, effects);
        m_trail = 0;
    }
    virtual ~Katana();
    virtual void SetTransform(const Vec3f *position, const Vec3f *rotation);
    virtual void Tick(float dt);
    virtual void Show();
    virtual void Hide();
    virtual void SwingHeld();
    virtual void SwingReleased();
    virtual void vf_0x38(int n);

    /* Fires the swing's shot (name inferred). */
    void FireShot();
    /* Moves the blade and the trail's points to the model's tip and root (name inferred). */
    void UpdateBlade();
    /* Ends the trail (name inferred). */
    void EndTrail() {
        if (m_trail != 0) {
            m_trail->fn_802DE9DC();
            m_trail = 0;
        }
    }

    /* 0x3C */ u8 m_hidden;
    /* 0x40 */ Vec3f m_points[2]; /* tip and root */
    /* 0x58 */ Test::EffLocusMultiVtx *m_trail;
};
SIZE_ASSERT(Katana, 0x5C);

} // namespace

/* The effects of the weapon model and of the item's model (meanings inferred). */
static const UnkWeaponEffect_80063428 s_effects[] = {
    {-1, 0x2328, 0x4B, -1, 0, 0},
    {0x233C, -1, 0x4C, -1, 0, 0},
};

static const u32 s_sounds[] = {0x233C, 0x233D};

static const UnkWeaponEffectSet_80063428 s_effectSet = {s_effects, 2, s_sounds, 2};

static const UnkWeaponEffect_80063428 s_itemEffects[] = {
    {-1, 0x801, 0x4B, -1, 0, 0},
    {0x233C, -1, 0x4C, -1, 0, 0},
};

static const u32 s_itemSounds[] = {0x2328, 0x801};

static const UnkWeaponEffectSet_80063428 s_itemEffectSet = {s_itemEffects, 2, s_itemSounds, 2};

/* The names are read by the static initializer. */
static const char *s_name01 = "Katana01";
static const char *s_name02 = "Katana02";

/* The trail's colour (name inferred). */
static UnkColor_8040D2EC s_trailColor(0xA5, 0, 0xFF);

/* Rotates m by a character's Euler angles in order (name inferred). */
inline void RotateMtx(Mtx34f *m, const CharInfo *chara, int order) {
    fn_8040C148(m, &(const Mtx34f &)fn_8040FFBC(m, &chara->m_rotation, order));
}

/* A distance (name inferred). As a call it is not a constant expression, so the records' values are
 * stored by the static initializer, as in retail. */
inline float Span(float length) {
    return length;
}

Katana::~Katana() {
    delete m_model;
    EndTrail();
}

void Katana::SetTransform(const Vec3f *position, const Vec3f *rotation) {
    Weapon::WeaponBase::SetTransform(position, rotation);
    m_model->fn_800632E8(position, rotation);
}

void Katana::Tick(float dt) {
    Weapon::WeaponStickBase::Tick(dt);
    m_model->fn_800632A0(dt);
    UpdateBlade();
}

void Katana::Show() {
    if (m_model != 0) {
        m_model->fn_80063130();
    }
    if (m_trail != 0) {
        m_trail->fn_802DE9BC();
    }
    m_hidden = false;
}

void Katana::Hide() {
    if (m_model != 0) {
        m_model->fn_80063154();
    }
    if (m_trail != 0) {
        m_trail->fn_802DE9CC();
    }
    m_hidden = true;
}

void Katana::SwingHeld() {
    if (m_trail == 0 && !m_hidden) {
        m_trail = new Test::EffLocusMultiVtx(10, 2, m_points, 0, &s_trailColor);
        UpdateBlade();
    }
    if (!fn_80067918()) {
        FireShot();
    }
    Weapon::WeaponStickBase::SwingHeld();
}

void Katana::SwingReleased() {
    EndTrail();
    Weapon::WeaponStickBase::SwingReleased();
}

void Katana::vf_0x38(int n) {
    fn_801D54F4(fn_80053164(), n + 0xE024, &m_position, 0, 1);
}

void Katana::FireShot() {
    boost::shared_ptr<CharInfo> owner = m_owner.lock();
    Mtx34f matrix;
    Mtx34f place;
    SetTranslationMtx(&place, owner->m_position);
    fn_8040C148(&matrix, &place);
    /* Retail calls the shared_ptr's operator-> out of line for each argument, as for arguments of
     * an inline call. */
    RotateMtx(&matrix, owner.operator->(), fn_801DBD00(owner->field_0x30));
    fn_8040B848(&matrix, 180.0f * (lbl_805FAD38 / 180.0f));
    fn_80051C34(m_type, fn_80069540(this, &matrix));
}

void Katana::UpdateBlade() {
    Vec3f tip;
    fn_800091EC(&tip, &(const Vec3f &)fn_8040FB08(m_model->fn_80063234(0)));
    Vec3f root;
    fn_800091EC(&root, &(const Vec3f &)fn_8040FB08(m_model->fn_80063234(1)));
    fn_80067870(&tip, &root);
    if (m_trail != 0) {
        if (!fn_8004EAC0(m_trail)) {
            m_trail = 0;
        } else {
            fn_800091D0(&m_points[0], &tip);
            fn_800091D0(&m_points[1], &root);
        }
    }
}

void WeaponSpecialKatana::Initialize() {
    fn_8006B50C(0x4B, 0);
    fn_8006B50C(0x4C, 0);
    fn_80051CC8(0x39);
    fn_80051CC8(0x3A);
    fn_8005B760(0x39);
    fn_8005B760(0x3A);
}

void WeaponSpecialKatana::Finalize() {
    fn_80051C80(0x39);
    fn_80051C80(0x3A);
}

/* The records' callbacks (names inferred). */
static Weapon::WeaponBase *Instantiate01(const boost::weak_ptr<CharInfo> &owner) {
    return new Katana(owner, 0x39, 4.0f, &s_effectSet);
}

static Weapon::Item *CreateItem01() {
    return new Weapon::Item(&s_itemEffectSet);
}

static Weapon::WeaponBase *Instantiate02(const boost::weak_ptr<CharInfo> &owner) {
    return new Katana(owner, 0x3A, 8.0f, &s_effectSet);
}

static Weapon::Item *CreateItem02() {
    return new Weapon::Item(&s_itemEffectSet);
}

UnkWeaponTypeInfo lbl_8054D2C0 = {
    s_name01, Instantiate01, CreateItem01, 0, Span(1.0f), Span(5.0f), 4, 3, 0x2001, 0.0f, 0.0f, 0,
};

UnkWeaponTypeInfo lbl_8054D2F0 = {
    s_name02, Instantiate02, CreateItem02, 0, Span(1.0f), Span(5.0f), 8, 3, 0x2001, 0.0f, 0.0f, 0,
};
