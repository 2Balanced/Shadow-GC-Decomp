/* BlackSword (class name from RTTI: Weapon::BlackSword): a sword swung at close range, weapon type
 * 0x1F, that draws a trail through seven points along its blade while it swings. The code is close to
 * Game/WeaponDarkHammer.cpp's, with the trail's points placed along the blade by the weapon's
 * position and rotation. File name inferred from the class name. Method and member names are
 * inferred. Definitions are in reverse address order (deferred inlining emits functions last-defined
 * first). */
#include "Game/CharInfo.h"
#include "Game/EffLocusMultiVtx.h"
#include "Game/GadgetUtil.h"
#include "Game/MathTypes.h"
#include "Game/WeaponBase.h"
#include "Game/WeaponItem.h"
#include "Game/WeaponStickBase.h"
#include "boost/weak_ptr.hpp"
#include "size_assert.h"

using Weapon::UnkWeaponEffect_80063428;
using Weapon::UnkWeaponEffectSet_80063428;
using Weapon::UnkWeaponModel_80063428;

extern "C" {
/* Debug output (behaviour inferred). */
void fn_803A953C(const char *format, ...);
/* Loads resource id (behaviour inferred). */
void fn_8006B50C(int id, int arg1);
/* Enables weapon type type (Game/WeaponTable.cpp). */
void fn_8005B760(int type);
/* The translation of a matrix, and a point taken through a matrix. */
Vec3f fn_8040FB08(const Mtx34f *matrix);
Vec3f fn_8040B67C(const Mtx34f *matrix, const Vec3f *point);
/* Whether task and all its parents are still running (behaviour inferred). */
bool fn_8004EAC0(Task *task);
}

namespace Weapon {

class BlackSword : public WeaponStickBase {
  public:
    BlackSword(const boost::weak_ptr<CharInfo> &owner);
    virtual ~BlackSword();
    virtual void SetTransform(const Vec3f *position, const Vec3f *rotation);
    virtual void Tick(float dt);
    virtual void Show();
    virtual void Hide();
    virtual void SwingHeld();
    virtual void SwingReleased();

    /* Moves the blade to the model's tip and root, and the trail's points along it. */
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

    /* 0x3C */ u8 m_hidden;
    /* 0x40 */ Vec3f m_lastPosition;
    /* 0x4C */ Vec3f m_lastRotation;
    /* 0x58 */ Vec3f m_points[7]; /* from the hilt to the tip (inferred) */
    /* 0xAC */ Test::EffLocusMultiVtx *m_trail;
};
SIZE_ASSERT(BlackSword, 0xB0);

} // namespace Weapon

static const char *s_taskName = "BlackSword";

/* The effects of the weapon model and of the item's model (meanings inferred). */
static const UnkWeaponEffect_80063428 s_effects[] = {
    {-1, 0x2328, 0x1A, -1, 0, 0},
    {0x233C, -1, 0x1B, -1, 0, 0},
};

static const u32 s_sounds[] = {0x233C, 0x233D};

static const UnkWeaponEffectSet_80063428 s_effectSet = {s_effects, 2, s_sounds, 2};

static const UnkWeaponEffect_80063428 s_itemEffects[] = {
    {-1, 0x801, 0x1A, -1, 0, 0},
    {0x233C, -1, 0x1B, -1, 0, 0},
};

static const u32 s_itemSounds[] = {0x2328, 0x801};

static const UnkWeaponEffectSet_80063428 s_itemEffectSet = {s_itemEffects, 2, s_itemSounds, 2};

/* A distance (name inferred). As a call it is not a constant expression, so the records' values are
 * stored by the static initializer, as in retail. */
inline float Span(float length) {
    return length;
}

/* The type's load and release hooks (names inferred). */
static void Initialize() {
    fn_803A953C("%s::Initialize\n", s_taskName);
    fn_8006B50C(0x1A, 0);
    fn_8006B50C(2, 0);
    fn_8006B50C(0x1B, 0);
    fn_8005B760(0x1F);
}

static void Finalize() {
    fn_803A953C("%s::Finalize\n", s_taskName);
}

namespace Weapon {

BlackSword::BlackSword(const boost::weak_ptr<CharInfo> &owner)
    : WeaponStickBase(owner, 0x1F, 4.0f), m_lastRotation(0.0f, 0.0f, 0.0f) {
    m_model = new UnkWeaponModel_80063428(owner, &s_effectSet);
    m_hidden = false;
    m_trail = 0;
}

BlackSword::~BlackSword() {
    delete m_model;
    EndTrail();
}

void BlackSword::SetTransform(const Vec3f *position, const Vec3f *rotation) {
    WeaponBase::SetTransform(position, rotation);
    m_model->fn_800632E8(position, rotation);
    fn_800091D0(&m_lastPosition, position);
    m_lastRotation = *rotation;
}

void BlackSword::Tick(float dt) {
    WeaponStickBase::Tick(dt);
    m_model->fn_800632A0(dt);
    UpdateBlade();
}

void BlackSword::Show() {
    if (m_model != 0) {
        m_model->fn_80063130();
    }
    if (m_trail != 0) {
        m_trail->fn_802DE9BC();
    }
    m_hidden = false;
}

void BlackSword::Hide() {
    if (m_model != 0) {
        m_model->fn_80063154();
    }
    if (m_trail != 0) {
        m_trail->fn_802DE9CC();
    }
    m_hidden = true;
}

void BlackSword::SwingHeld() {
    if (m_trail == 0 && !m_hidden) {
        UnkColor_8040D2EC color(0x80, 0, 0x60);
        m_trail = new Test::EffLocusMultiVtx(10, 7, m_points, 0, &color);
        UpdateBlade();
    }
    WeaponStickBase::SwingHeld();
}

void BlackSword::SwingReleased() {
    EndTrail();
    WeaponStickBase::SwingReleased();
}

void BlackSword::UpdateBlade() {
    Vec3f tip;
    fn_800091EC(&tip, &(const Vec3f &)fn_8040FB08(m_model->fn_80063234(0)));
    Vec3f root;
    fn_800091EC(&root, &(const Vec3f &)fn_8040FB08(m_model->fn_80063234(1)));
    fn_80067870(&tip, &root);
    if (m_trail != 0) {
        if (!fn_8004EAC0(m_trail)) {
            m_trail = 0;
        } else {
            Mtx34f matrix;
            Mtx34f translation;
            fn_8040B034(&translation, &m_lastPosition);
            fn_8040C148(&matrix, &translation);
            /* The points' height follows the swing's scale. */
            float scale = fn_8006790C();
            fn_8040B848(&matrix, m_lastRotation.y);
            fn_8040B884(&matrix, m_lastRotation.x);
            fn_8040B80C(&matrix, m_lastRotation.z);
            fn_800091D0(&m_points[0], &(const Vec3f &)fn_8040B67C(&matrix,
                        &(const Vec3f &)Vec3f(0.24f, -5.05f * scale, -0.31f)));
            fn_800091D0(&m_points[1], &(const Vec3f &)fn_8040B67C(&matrix,
                        &(const Vec3f &)Vec3f(0.45f, -9.08f * scale, -1.25f)));
            fn_800091D0(&m_points[2], &(const Vec3f &)fn_8040B67C(&matrix,
                        &(const Vec3f &)Vec3f(0.5f, -9.44f * scale, -3.13f)));
            fn_800091D0(&m_points[3], &(const Vec3f &)fn_8040B67C(&matrix,
                        &(const Vec3f &)Vec3f(0.4f, -10.7f * scale, -0.9f)));
            fn_800091D0(&m_points[4], &(const Vec3f &)fn_8040B67C(&matrix,
                        &(const Vec3f &)Vec3f(-0.2f, -16.5f * scale, 2.6f)));
            fn_800091D0(&m_points[5], &(const Vec3f &)fn_8040B67C(&matrix,
                        &(const Vec3f &)Vec3f(0.4f, -22.4f * scale, 2.31f)));
            fn_800091D0(&m_points[6], &(const Vec3f &)fn_8040B67C(&matrix,
                        &(const Vec3f &)Vec3f(0.51f, -26.2f * scale, 1.17f)));
        }
    }
}

WeaponBase *BlackSword::Instantiate(const boost::weak_ptr<CharInfo> &owner) {
    return new BlackSword(owner);
}

Item *BlackSword::CreateItem() {
    return new Item(&s_itemEffectSet);
}

} // namespace Weapon

UnkWeaponTaskInfo lbl_8051E870 = {
    s_taskName, Initialize, Finalize, 0, 0, 0x80, 0x00FB0400, 0, 0,
};

UnkWeaponTypeInfo lbl_8051E894 = {
    s_taskName, Weapon::BlackSword::Instantiate, Weapon::BlackSword::CreateItem, 0, Span(1.0f), Span(5.0f),
    6, 3, 0x2001, 0.0f, 0.0f, 0,
};
