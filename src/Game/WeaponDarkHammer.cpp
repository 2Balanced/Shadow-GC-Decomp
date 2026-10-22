/* DarkHammer (class name from RTTI: Weapon::DarkHammer): a hammer swung at close range, weapon type
 * 0x20, that draws a trail while it swings. The code is close to WeaponSpecialKatana.cpp's without
 * the shot. File name inferred from the class name. Method and member names are inferred.
 * Definitions are in reverse address order (deferred inlining emits functions last-defined
 * first). */
#include "Game/CharInfo.h"
#include "Game/EffLocusMultiVtx.h"
#include "Game/GadgetUtil.h"
#include "Game/WeaponBase.h"
#include "Game/WeaponItem.h"
#include "Game/WeaponStickBase.h"
#include "boost/shared_ptr.hpp"
#include "boost/weak_ptr.hpp"
#include "size_assert.h"

using Weapon::UnkWeaponEffect_80063428;
using Weapon::UnkWeaponEffectSet_80063428;
using Weapon::UnkWeaponEffectSub_80063428;
using Weapon::UnkWeaponModel_80063428;

extern "C" {
/* Debug output (behaviour inferred). */
void fn_803A953C(const char *format, ...);
/* Loads resource id (behaviour inferred). */
void fn_8006B50C(int id, int arg1);
/* Sets up the pick-up item of weapon type type (behaviour inferred). */
void fn_8005B760(int type);
/* The translation of a matrix. */
Vec3f fn_8040FB08(const Mtx34f *matrix);
/* Whether task and all its parents are still running (behaviour inferred). */
bool fn_8004EAC0(Task *task);
}

namespace Weapon {

class DarkHammer : public WeaponStickBase {
  public:
    DarkHammer(const boost::weak_ptr<CharInfo> &owner);
    virtual ~DarkHammer();
    virtual void SetTransform(const Vec3f *position, const Vec3f *rotation);
    virtual void Tick(float dt);
    virtual void Show();
    virtual void Hide();
    virtual void SwingHeld();
    virtual void SwingReleased();

    /* Moves the blade and the trail's points to the model's tip and root. */
    void UpdateBlade();
    /* Ends the trail. */
    void EndTrail() {
        if (m_trail != 0) {
            m_trail->fn_802DE9DC();
            m_trail = 0;
        }
    }

    static WeaponBase *Instantiate(const boost::weak_ptr<CharInfo> &owner);
    static Item *CreateItem();

    /* 0x3C */ Vec3f m_lastPosition;
    /* 0x48 */ Vec3f m_lastRotation;
    /* 0x54 */ u8 m_hidden;
    /* 0x58 */ Vec3f m_points[2]; /* tip and root */
    /* 0x70 */ Test::EffLocusMultiVtx *m_trail;
};
SIZE_ASSERT(DarkHammer, 0x74);

} // namespace Weapon

static const char *s_taskName = "DarkHammer";

/* The effects of the weapon model and of the item's model (meanings inferred). */
static const UnkWeaponEffectSub_80063428 s_effectSubs[] = {
    {0x6D, 0, 0},
};

static const UnkWeaponEffect_80063428 s_effects[] = {
    {-1, 0x2328, 0x1C, -1, 0, 0},
    {0x233C, -1, 0x1D, 0x5E, s_effectSubs, 1},
};

static const u32 s_sounds[] = {0x233C, 0x233D};

static const UnkWeaponEffectSet_80063428 s_effectSet = {s_effects, 2, s_sounds, 2};

static const UnkWeaponEffect_80063428 s_itemEffects[] = {
    {-1, 0x801, 0x1C, -1, 0, 0},
    {0x233C, -1, 0x1D, 0x5E, s_effectSubs, 1},
};

static const u32 s_itemSounds[] = {0x2328, 0x801};

static const UnkWeaponEffectSet_80063428 s_itemEffectSet = {s_itemEffects, 2, s_itemSounds, 2};

/* A distance (name inferred). As a call it is not a constant expression, so the records' values are
 * stored by the static initializer, as in retail. */
inline float Span(float length) {
    return length;
}

namespace Weapon {

DarkHammer::DarkHammer(const boost::weak_ptr<CharInfo> &owner)
    : WeaponStickBase(owner, 0x20, 6.0f), m_lastRotation(0.0f, 0.0f, 0.0f) {
    m_hidden = false;
    m_model = new UnkWeaponModel_80063428(owner, &s_effectSet);
    m_trail = 0;
}

DarkHammer::~DarkHammer() {
    delete m_model;
    EndTrail();
}

void DarkHammer::SetTransform(const Vec3f *position, const Vec3f *rotation) {
    WeaponBase::SetTransform(position, rotation);
    m_model->fn_800632E8(position, rotation);
    fn_800091D0(&m_lastPosition, position);
    m_lastRotation = *rotation;
}

void DarkHammer::Tick(float dt) {
    WeaponStickBase::Tick(dt);
    m_model->fn_800632A0(dt);
    UpdateBlade();
}

void DarkHammer::Show() {
    if (m_model != 0) {
        m_model->fn_80063130();
    }
    if (m_trail != 0) {
        m_trail->fn_802DE9BC();
    }
    m_hidden = false;
}

void DarkHammer::Hide() {
    if (m_model != 0) {
        m_model->fn_80063154();
    }
    if (m_trail != 0) {
        m_trail->fn_802DE9CC();
    }
    m_hidden = true;
}

void DarkHammer::SwingHeld() {
    if (m_trail == 0 && !m_hidden) {
        UnkColor_8040D2EC color(0x80, 0x20, 0);
        m_trail = new Test::EffLocusMultiVtx(10, 2, m_points, 0, &color);
        UpdateBlade();
    }
    WeaponStickBase::SwingHeld();
}

void DarkHammer::SwingReleased() {
    EndTrail();
    WeaponStickBase::SwingReleased();
}

void DarkHammer::UpdateBlade() {
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

} // namespace Weapon

extern "C" void fn_802F12E8(void) {
    fn_803A953C("%s::Initialize\n", s_taskName);
    fn_8006B50C(0x1C, 0);
    fn_8006B50C(0x1D, 0);
    fn_8006B50C(0x5E, 0);
    fn_8006B50C(0x6D, 0);
    fn_8005B760(0x20);
}

extern "C" void fn_802F12B8(void) {
    fn_803A953C("%s::Finalize\n", s_taskName);
}

Weapon::WeaponBase *Weapon::DarkHammer::Instantiate(const boost::weak_ptr<CharInfo> &owner) {
    return new DarkHammer(owner);
}

Weapon::Item *Weapon::DarkHammer::CreateItem() {
    return new Item(&s_itemEffectSet);
}

UnkWeaponTaskInfo lbl_80559DB0 = {
    s_taskName, fn_802F12E8, fn_802F12B8, 0, 0, 0x80, 0x00FC0400, 0, 0,
};

UnkWeaponTypeInfo lbl_80559DD4 = {
    s_taskName, Weapon::DarkHammer::Instantiate, Weapon::DarkHammer::CreateItem, 0, Span(1.0f), Span(5.0f),
    6, 3, 0x2001, 0.0f, 0.0f, 0,
};
