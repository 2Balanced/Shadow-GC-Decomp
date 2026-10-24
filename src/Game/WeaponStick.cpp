/* Stick (class name from RTTI: Weapon::Stick): the swung weapons whose blade draws a trail through its
 * tip and root while it swings. The unit registers 23 weapons with it, SurvivalKnife, EggSpear and the
 * sticks Stick0100 to Stick0700, which differ in weapon type, resources and model; their records are
 * completed by the static initializer. The code is close to Game/WeaponBlackSword.cpp's. File name
 * from the class name; the other names are inferred. Definitions are in reverse address order
 * (deferred inlining emits functions last-defined first). The unit's code reaches its .data through
 * a pooled base address, so the linker keeps that section whole: Game/TNumEdit.h supplies the RTTI
 * base lists it starts with. */
#include "Game/TNumEdit.h"
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
/* The translation of a matrix. */
Vec3f fn_8040FB08(const Mtx34f *matrix);
/* Whether task and all its parents are still running (behaviour inferred). */
bool fn_8004EAC0(Task *task);
}

namespace Weapon {

class Stick : public WeaponStickBase {
  public:
    /* A stick whose model is built without a model number (the knife and the spear), and one
     * with model number model. */
    Stick(const boost::weak_ptr<CharInfo> &owner, int type, const UnkWeaponEffectSet_80063428 *effects)
        : WeaponStickBase(owner, type, 2.0f) {
        m_model = new UnkWeaponModel_80063428(owner, effects);
        m_hidden = false;
        m_trail = 0;
    }
    Stick(const boost::weak_ptr<CharInfo> &owner, int type, int model,
          const UnkWeaponEffectSet_80063428 *effects)
        : WeaponStickBase(owner, type, 2.0f) {
        m_model = new UnkWeaponModel_80063428(owner, model, effects);
        m_hidden = false;
        m_trail = 0;
    }
    virtual ~Stick();
    virtual void SetTransform(const Vec3f *position, const Vec3f *rotation);
    virtual void Tick(float dt);
    virtual void Show();
    virtual void Hide();
    virtual void SwingHeld();
    virtual void SwingReleased();

    /* Moves the blade to the model's tip and root, and the trail's points with it. */
    void UpdateBlade();
    /* Ends the trail. */
    void EndTrail() {
        if (m_trail != 0) {
            m_trail->fn_802DE9DC();
            m_trail = 0;
        }
    }

    /* 0x3C */ u8 m_hidden;
    /* 0x40 */ Vec3f m_points[2]; /* the tip and the root */
    /* 0x58 */ Test::EffLocusMultiVtx *m_trail;
};
SIZE_ASSERT(Stick, 0x5C);

} // namespace Weapon

using Weapon::Stick;

/* The records of the weapons. The names are read by the static initializer. */
/* SurvivalKnife. */
static const char *s_nameSurvivalKnife = "SurvivalKnife";

static const UnkWeaponEffect_80063428 s_effectsSurvivalKnife[] = {
    {-1, 0x2328, 0x47, -1, 0, 0},
};

static const u32 s_soundsSurvivalKnife[] = {0x233C, 0x233D};

static const UnkWeaponEffectSet_80063428 s_effectSetSurvivalKnife = {s_effectsSurvivalKnife, 1, s_soundsSurvivalKnife, 2};

static const UnkWeaponEffect_80063428 s_itemEffectsSurvivalKnife[] = {
    {-1, 0x801, 0x47, -1, 0, 0},
};

static const u32 s_itemSoundsSurvivalKnife[] = {0x2328, 0x801};

static const UnkWeaponEffectSet_80063428 s_itemEffectSetSurvivalKnife = {s_itemEffectsSurvivalKnife, 1, s_itemSoundsSurvivalKnife, 2};

/* EggSpear. */
static const char *s_nameEggSpear = "EggSpear";

static const UnkWeaponEffect_80063428 s_effectsEggSpear[] = {
    {-1, 0x2328, 0x34, -1, 0, 0},
};

static const u32 s_soundsEggSpear[] = {0x233C, 0x233D};

static const UnkWeaponEffectSet_80063428 s_effectSetEggSpear = {s_effectsEggSpear, 1, s_soundsEggSpear, 2};

static const UnkWeaponEffect_80063428 s_itemEffectsEggSpear[] = {
    {-1, 0x801, 0x34, -1, 0, 0},
};

static const u32 s_itemSoundsEggSpear[] = {0x2328, 0x801};

static const UnkWeaponEffectSet_80063428 s_itemEffectSetEggSpear = {s_itemEffectsEggSpear, 1, s_itemSoundsEggSpear, 2};

/* Stick0100. */
static const char *s_nameStick0100 = "Stick0100";

static const UnkWeaponEffect_80063428 s_effectsStick0100[] = {
    {-1, 0x2328, 0x3B, -1, 0, 0},
};

static const u32 s_soundsStick0100[] = {0x233C, 0x233D};

static const UnkWeaponEffectSet_80063428 s_effectSetStick0100 = {s_effectsStick0100, 1, s_soundsStick0100, 2};

static const UnkWeaponEffect_80063428 s_itemEffectsStick0100[] = {
    {-1, 0x801, 0x3B, -1, 0, 0},
};

static const u32 s_itemSoundsStick0100[] = {0x2328, 0x801};

static const UnkWeaponEffectSet_80063428 s_itemEffectSetStick0100 = {s_itemEffectsStick0100, 1, s_itemSoundsStick0100, 2};

/* Stick0200. */
static const char *s_nameStick0200 = "Stick0200";

static const UnkWeaponEffect_80063428 s_effectsStick0200[] = {
    {-1, 0x2328, 0x2F, -1, 0, 0},
    {0x233C, -1, 0x30, -1, 0, 0},
};

static const u32 s_soundsStick0200[] = {0x233C, 0x233D};

static const UnkWeaponEffectSet_80063428 s_effectSetStick0200 = {s_effectsStick0200, 2, s_soundsStick0200, 2};

static const UnkWeaponEffect_80063428 s_itemEffectsStick0200[] = {
    {-1, 0x801, 0x2F, -1, 0, 0},
    {0x233C, -1, 0x30, -1, 0, 0},
};

static const u32 s_itemSoundsStick0200[] = {0x2328, 0x801};

static const UnkWeaponEffectSet_80063428 s_itemEffectSetStick0200 = {s_itemEffectsStick0200, 2, s_itemSoundsStick0200, 2};

/* Stick0201. */
static const char *s_nameStick0201 = "Stick0201";

static const UnkWeaponEffect_80063428 s_effectsStick0201[] = {
    {-1, 0x2328, 0x24, -1, 0, 0},
};

static const u32 s_soundsStick0201[] = {0x233C, 0x233D};

static const UnkWeaponEffectSet_80063428 s_effectSetStick0201 = {s_effectsStick0201, 1, s_soundsStick0201, 2};

static const UnkWeaponEffect_80063428 s_itemEffectsStick0201[] = {
    {-1, 0x801, 0x24, -1, 0, 0},
};

static const u32 s_itemSoundsStick0201[] = {0x2328, 0x801};

static const UnkWeaponEffectSet_80063428 s_itemEffectSetStick0201 = {s_itemEffectsStick0201, 1, s_itemSoundsStick0201, 2};

/* Stick0202. */
static const char *s_nameStick0202 = "Stick0202";

static const UnkWeaponEffect_80063428 s_effectsStick0202[] = {
    {-1, 0x2328, 0x4D, -1, 0, 0},
};

static const u32 s_soundsStick0202[] = {0x233C, 0x233D};

static const UnkWeaponEffectSet_80063428 s_effectSetStick0202 = {s_effectsStick0202, 1, s_soundsStick0202, 2};

static const UnkWeaponEffect_80063428 s_itemEffectsStick0202[] = {
    {-1, 0x801, 0x4D, -1, 0, 0},
};

static const u32 s_itemSoundsStick0202[] = {0x2328, 0x801};

static const UnkWeaponEffectSet_80063428 s_itemEffectSetStick0202 = {s_itemEffectsStick0202, 1, s_itemSoundsStick0202, 2};

/* Stick0301. */
static const char *s_nameStick0301 = "Stick0301";

static const UnkWeaponEffect_80063428 s_effectsStick0301[] = {
    {-1, 0x2328, 0x32, -1, 0, 0},
    {0x233C, -1, 0x33, -1, 0, 0},
};

static const u32 s_soundsStick0301[] = {0x233C, 0x233D};

static const UnkWeaponEffectSet_80063428 s_effectSetStick0301 = {s_effectsStick0301, 2, s_soundsStick0301, 2};

static const UnkWeaponEffect_80063428 s_itemEffectsStick0301[] = {
    {-1, 0x801, 0x32, -1, 0, 0},
    {0x233C, -1, 0x33, -1, 0, 0},
};

static const u32 s_itemSoundsStick0301[] = {0x2328, 0x801};

static const UnkWeaponEffectSet_80063428 s_itemEffectSetStick0301 = {s_itemEffectsStick0301, 2, s_itemSoundsStick0301, 2};

/* Stick0302. */
static const char *s_nameStick0302 = "Stick0302";

static const UnkWeaponEffect_80063428 s_effectsStick0302[] = {
    {-1, 0x2328, 0x42, -1, 0, 0},
};

static const u32 s_soundsStick0302[] = {0x233C, 0x233D};

static const UnkWeaponEffectSet_80063428 s_effectSetStick0302 = {s_effectsStick0302, 1, s_soundsStick0302, 2};

static const UnkWeaponEffect_80063428 s_itemEffectsStick0302[] = {
    {-1, 0x801, 0x42, -1, 0, 0},
};

static const u32 s_itemSoundsStick0302[] = {0x2328, 0x801};

static const UnkWeaponEffectSet_80063428 s_itemEffectSetStick0302 = {s_itemEffectsStick0302, 1, s_itemSoundsStick0302, 2};

/* Stick0400. */
static const char *s_nameStick0400 = "Stick0400";

static const UnkWeaponEffect_80063428 s_effectsStick0400[] = {
    {-1, 0x2328, 0x4E, -1, 0, 0},
};

static const u32 s_soundsStick0400[] = {0x233C, 0x233D};

static const UnkWeaponEffectSet_80063428 s_effectSetStick0400 = {s_effectsStick0400, 1, s_soundsStick0400, 2};

static const UnkWeaponEffect_80063428 s_itemEffectsStick0400[] = {
    {-1, 0x801, 0x4E, -1, 0, 0},
};

static const u32 s_itemSoundsStick0400[] = {0x2328, 0x801};

static const UnkWeaponEffectSet_80063428 s_itemEffectSetStick0400 = {s_itemEffectsStick0400, 1, s_itemSoundsStick0400, 2};

/* Stick0401. */
static const char *s_nameStick0401 = "Stick0401";

static const UnkWeaponEffect_80063428 s_effectsStick0401[] = {
    {-1, 0x2328, 0x35, -1, 0, 0},
    {0x233C, -1, 0x36, -1, 0, 0},
};

static const u32 s_soundsStick0401[] = {0x233C, 0x233D};

static const UnkWeaponEffectSet_80063428 s_effectSetStick0401 = {s_effectsStick0401, 2, s_soundsStick0401, 2};

static const UnkWeaponEffect_80063428 s_itemEffectsStick0401[] = {
    {-1, 0x801, 0x35, -1, 0, 0},
    {0x233C, -1, 0x36, -1, 0, 0},
};

static const u32 s_itemSoundsStick0401[] = {0x2328, 0x801};

static const UnkWeaponEffectSet_80063428 s_itemEffectSetStick0401 = {s_itemEffectsStick0401, 2, s_itemSoundsStick0401, 2};

/* Stick0402. */
static const char *s_nameStick0402 = "Stick0402";

static const UnkWeaponEffect_80063428 s_effectsStick0402[] = {
    {-1, 0x2328, 0x24, -1, 0, 0},
};

static const u32 s_soundsStick0402[] = {0x233C, 0x233D};

static const UnkWeaponEffectSet_80063428 s_effectSetStick0402 = {s_effectsStick0402, 1, s_soundsStick0402, 2};

static const UnkWeaponEffect_80063428 s_itemEffectsStick0402[] = {
    {-1, 0x801, 0x24, -1, 0, 0},
};

static const u32 s_itemSoundsStick0402[] = {0x2328, 0x801};

static const UnkWeaponEffectSet_80063428 s_itemEffectSetStick0402 = {s_itemEffectsStick0402, 1, s_itemSoundsStick0402, 2};

/* Stick0403. */
static const char *s_nameStick0403 = "Stick0403";

static const UnkWeaponEffect_80063428 s_effectsStick0403[] = {
    {-1, 0x2328, 0x2F, -1, 0, 0},
    {0x233C, -1, 0x30, -1, 0, 0},
};

static const u32 s_soundsStick0403[] = {0x233C, 0x233D};

static const UnkWeaponEffectSet_80063428 s_effectSetStick0403 = {s_effectsStick0403, 2, s_soundsStick0403, 2};

static const UnkWeaponEffect_80063428 s_itemEffectsStick0403[] = {
    {-1, 0x801, 0x2F, -1, 0, 0},
    {0x233C, -1, 0x30, -1, 0, 0},
};

static const u32 s_itemSoundsStick0403[] = {0x2328, 0x801};

static const UnkWeaponEffectSet_80063428 s_itemEffectSetStick0403 = {s_itemEffectsStick0403, 2, s_itemSoundsStick0403, 2};

/* Stick0404. */
static const char *s_nameStick0404 = "Stick0404";

static const UnkWeaponEffect_80063428 s_effectsStick0404[] = {
    {-1, 0x2328, 0x34, -1, 0, 0},
    {0x233C, -1, 0x35, -1, 0, 0},
};

static const u32 s_soundsStick0404[] = {0x233C, 0x233D};

static const UnkWeaponEffectSet_80063428 s_effectSetStick0404 = {s_effectsStick0404, 2, s_soundsStick0404, 2};

static const UnkWeaponEffect_80063428 s_itemEffectsStick0404[] = {
    {-1, 0x801, 0x34, -1, 0, 0},
    {0x233C, -1, 0x35, -1, 0, 0},
};

static const u32 s_itemSoundsStick0404[] = {0x2328, 0x801};

static const UnkWeaponEffectSet_80063428 s_itemEffectSetStick0404 = {s_itemEffectsStick0404, 2, s_itemSoundsStick0404, 2};

/* Stick0501. */
static const char *s_nameStick0501 = "Stick0501";

static const UnkWeaponEffect_80063428 s_effectsStick0501[] = {
    {-1, 0x2328, 0x34, -1, 0, 0},
};

static const u32 s_soundsStick0501[] = {0x233C, 0x233D};

static const UnkWeaponEffectSet_80063428 s_effectSetStick0501 = {s_effectsStick0501, 1, s_soundsStick0501, 2};

static const UnkWeaponEffect_80063428 s_itemEffectsStick0501[] = {
    {-1, 0x801, 0x34, -1, 0, 0},
};

static const u32 s_itemSoundsStick0501[] = {0x2328, 0x801};

static const UnkWeaponEffectSet_80063428 s_itemEffectSetStick0501 = {s_itemEffectsStick0501, 1, s_itemSoundsStick0501, 2};

/* Stick0502. */
static const char *s_nameStick0502 = "Stick0502";

static const UnkWeaponEffect_80063428 s_effectsStick0502[] = {
    {-1, 0x2328, 0x2A, -1, 0, 0},
};

static const u32 s_soundsStick0502[] = {0x233C, 0x233D};

static const UnkWeaponEffectSet_80063428 s_effectSetStick0502 = {s_effectsStick0502, 1, s_soundsStick0502, 2};

static const UnkWeaponEffect_80063428 s_itemEffectsStick0502[] = {
    {-1, 0x801, 0x2A, -1, 0, 0},
};

static const u32 s_itemSoundsStick0502[] = {0x2328, 0x801};

static const UnkWeaponEffectSet_80063428 s_itemEffectSetStick0502 = {s_itemEffectsStick0502, 1, s_itemSoundsStick0502, 2};

/* Stick0503. */
static const char *s_nameStick0503 = "Stick0503";

static const UnkWeaponEffect_80063428 s_effectsStick0503[] = {
    {-1, 0x2328, 0x4D, -1, 0, 0},
    {0x233C, -1, 0x4E, -1, 0, 0},
};

static const u32 s_soundsStick0503[] = {0x233C, 0x233D};

static const UnkWeaponEffectSet_80063428 s_effectSetStick0503 = {s_effectsStick0503, 2, s_soundsStick0503, 2};

static const UnkWeaponEffect_80063428 s_itemEffectsStick0503[] = {
    {-1, 0x801, 0x4D, -1, 0, 0},
    {0x233C, -1, 0x4E, -1, 0, 0},
};

static const u32 s_itemSoundsStick0503[] = {0x2328, 0x801};

static const UnkWeaponEffectSet_80063428 s_itemEffectSetStick0503 = {s_itemEffectsStick0503, 2, s_itemSoundsStick0503, 2};

/* Stick0504. */
static const char *s_nameStick0504 = "Stick0504";

static const UnkWeaponEffect_80063428 s_effectsStick0504[] = {
    {-1, 0x2328, 0x34, -1, 0, 0},
    {0x233C, -1, 0x35, -1, 0, 0},
};

static const u32 s_soundsStick0504[] = {0x233C, 0x233D};

static const UnkWeaponEffectSet_80063428 s_effectSetStick0504 = {s_effectsStick0504, 2, s_soundsStick0504, 2};

static const UnkWeaponEffect_80063428 s_itemEffectsStick0504[] = {
    {-1, 0x801, 0x34, -1, 0, 0},
    {0x233C, -1, 0x35, -1, 0, 0},
};

static const u32 s_itemSoundsStick0504[] = {0x2328, 0x801};

static const UnkWeaponEffectSet_80063428 s_itemEffectSetStick0504 = {s_itemEffectsStick0504, 2, s_itemSoundsStick0504, 2};

/* Stick0600. */
static const char *s_nameStick0600 = "Stick0600";

static const UnkWeaponEffect_80063428 s_effectsStick0600[] = {
    {-1, 0x2328, 0x35, -1, 0, 0},
};

static const u32 s_soundsStick0600[] = {0x233C, 0x233D};

static const UnkWeaponEffectSet_80063428 s_effectSetStick0600 = {s_effectsStick0600, 1, s_soundsStick0600, 2};

static const UnkWeaponEffect_80063428 s_itemEffectsStick0600[] = {
    {-1, 0x801, 0x35, -1, 0, 0},
};

static const u32 s_itemSoundsStick0600[] = {0x2328, 0x801};

static const UnkWeaponEffectSet_80063428 s_itemEffectSetStick0600 = {s_itemEffectsStick0600, 1, s_itemSoundsStick0600, 2};

/* Stick0601. */
static const char *s_nameStick0601 = "Stick0601";

static const UnkWeaponEffect_80063428 s_effectsStick0601[] = {
    {-1, 0x2328, 0x4C, -1, 0, 0},
};

static const u32 s_soundsStick0601[] = {0x233C, 0x233D};

static const UnkWeaponEffectSet_80063428 s_effectSetStick0601 = {s_effectsStick0601, 1, s_soundsStick0601, 2};

static const UnkWeaponEffect_80063428 s_itemEffectsStick0601[] = {
    {-1, 0x801, 0x4C, -1, 0, 0},
};

static const u32 s_itemSoundsStick0601[] = {0x2328, 0x801};

static const UnkWeaponEffectSet_80063428 s_itemEffectSetStick0601 = {s_itemEffectsStick0601, 1, s_itemSoundsStick0601, 2};

/* Stick0602. */
static const char *s_nameStick0602 = "Stick0602";

static const UnkWeaponEffect_80063428 s_effectsStick0602[] = {
    {-1, 0x2328, 0x4E, -1, 0, 0},
};

static const u32 s_soundsStick0602[] = {0x233C, 0x233D};

static const UnkWeaponEffectSet_80063428 s_effectSetStick0602 = {s_effectsStick0602, 1, s_soundsStick0602, 2};

static const UnkWeaponEffect_80063428 s_itemEffectsStick0602[] = {
    {-1, 0x801, 0x4E, -1, 0, 0},
};

static const u32 s_itemSoundsStick0602[] = {0x2328, 0x801};

static const UnkWeaponEffectSet_80063428 s_itemEffectSetStick0602 = {s_itemEffectsStick0602, 1, s_itemSoundsStick0602, 2};

/* Stick0603. */
static const char *s_nameStick0603 = "Stick0603";

static const UnkWeaponEffect_80063428 s_effectsStick0603[] = {
    {-1, 0x2328, 0x4B, -1, 0, 0},
    {0x233C, -1, 0x4C, -1, 0, 0},
};

static const u32 s_soundsStick0603[] = {0x233C, 0x233D};

static const UnkWeaponEffectSet_80063428 s_effectSetStick0603 = {s_effectsStick0603, 2, s_soundsStick0603, 2};

static const UnkWeaponEffect_80063428 s_itemEffectsStick0603[] = {
    {-1, 0x801, 0x4B, -1, 0, 0},
    {0x233C, -1, 0x4C, -1, 0, 0},
};

static const u32 s_itemSoundsStick0603[] = {0x2328, 0x801};

static const UnkWeaponEffectSet_80063428 s_itemEffectSetStick0603 = {s_itemEffectsStick0603, 2, s_itemSoundsStick0603, 2};

/* Stick0604. */
static const char *s_nameStick0604 = "Stick0604";

static const UnkWeaponEffect_80063428 s_effectsStick0604[] = {
    {-1, 0x2328, 0x4C, -1, 0, 0},
};

static const u32 s_soundsStick0604[] = {0x233C, 0x233D};

static const UnkWeaponEffectSet_80063428 s_effectSetStick0604 = {s_effectsStick0604, 1, s_soundsStick0604, 2};

static const UnkWeaponEffect_80063428 s_itemEffectsStick0604[] = {
    {-1, 0x801, 0x4C, -1, 0, 0},
};

static const u32 s_itemSoundsStick0604[] = {0x2328, 0x801};

static const UnkWeaponEffectSet_80063428 s_itemEffectSetStick0604 = {s_itemEffectsStick0604, 1, s_itemSoundsStick0604, 2};

/* Stick0700. */
static const char *s_nameStick0700 = "Stick0700";

static const UnkWeaponEffect_80063428 s_effectsStick0700[] = {
    {-1, 0x2328, 0x4C, -1, 0, 0},
};

static const u32 s_soundsStick0700[] = {0x233C, 0x233D};

static const UnkWeaponEffectSet_80063428 s_effectSetStick0700 = {s_effectsStick0700, 1, s_soundsStick0700, 2};

static const UnkWeaponEffect_80063428 s_itemEffectsStick0700[] = {
    {-1, 0x801, 0x4C, -1, 0, 0},
};

static const u32 s_itemSoundsStick0700[] = {0x2328, 0x801};

static const UnkWeaponEffectSet_80063428 s_itemEffectSetStick0700 = {s_itemEffectsStick0700, 1, s_itemSoundsStick0700, 2};

/* A distance (name inferred). As a call it is not a constant expression, so the records' values are
 * stored by the static initializer, as in retail. */
inline float Span(float length) {
    return length;
}

namespace Weapon {

Stick::~Stick() {
    delete m_model;
    EndTrail();
}

void Stick::SetTransform(const Vec3f *position, const Vec3f *rotation) {
    WeaponBase::SetTransform(position, rotation);
    m_model->fn_800632E8(position, rotation);
}

void Stick::Tick(float dt) {
    WeaponStickBase::Tick(dt);
    m_model->fn_800632A0(dt);
    UpdateBlade();
}

void Stick::Show() {
    if (m_model != 0) {
        m_model->fn_80063130();
    }
    if (m_trail != 0) {
        m_trail->fn_802DE9BC();
    }
    m_hidden = false;
}

void Stick::Hide() {
    if (m_model != 0) {
        m_model->fn_80063154();
    }
    if (m_trail != 0) {
        m_trail->fn_802DE9CC();
    }
    m_hidden = true;
}

void Stick::SwingHeld() {
    if (m_trail == 0 && !m_hidden) {
        UnkColor_8040D2EC color(0x80, 0x20, 0);
        m_trail = new Test::EffLocusMultiVtx(10, 2, m_points, 0, &color);
        UpdateBlade();
    }
    WeaponStickBase::SwingHeld();
}

void Stick::SwingReleased() {
    EndTrail();
    WeaponStickBase::SwingReleased();
}

void Stick::UpdateBlade() {
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

/* SurvivalKnife's callbacks (names inferred). */
static void InitializeSurvivalKnife() {
    fn_803A953C("%s::Initialize\n", s_nameSurvivalKnife);
    fn_8006B50C(0x47, 0);
    fn_8005B760(0x1E);
}

static void FinalizeSurvivalKnife() {
    fn_803A953C("%s::Finalize\n", s_nameSurvivalKnife);
}

static Weapon::WeaponBase *InstantiateSurvivalKnife(const boost::weak_ptr<CharInfo> &owner) {
    return new Stick(owner, 0x1E, &s_effectSetSurvivalKnife);
}

static Weapon::Item *CreateItemSurvivalKnife() {
    return new Weapon::Item(&s_itemEffectSetSurvivalKnife);
}

/* EggSpear's callbacks (names inferred). */
static void InitializeEggSpear() {
    fn_803A953C("%s::Initialize\n", s_nameEggSpear);
    fn_8006B50C(0x34, 0);
    fn_8005B760(0x21);
}

static void FinalizeEggSpear() {
    fn_803A953C("%s::Finalize\n", s_nameEggSpear);
}

static Weapon::WeaponBase *InstantiateEggSpear(const boost::weak_ptr<CharInfo> &owner) {
    return new Stick(owner, 0x21, &s_effectSetEggSpear);
}

static Weapon::Item *CreateItemEggSpear() {
    return new Weapon::Item(&s_itemEffectSetEggSpear);
}

/* Stick0100's callbacks (names inferred). */
static void InitializeStick0100() {
    fn_803A953C("%s::Initialize\n", s_nameStick0100);
    fn_8006B50C(0x3B, 0);
    fn_8005B760(0x22);
}

static void FinalizeStick0100() {
    fn_803A953C("%s::Finalize\n", s_nameStick0100);
}

static Weapon::WeaponBase *InstantiateStick0100(const boost::weak_ptr<CharInfo> &owner) {
    return new Stick(owner, 0x22, 1, &s_effectSetStick0100);
}

static Weapon::Item *CreateItemStick0100() {
    return new Weapon::Item((void *)1, &s_itemEffectSetStick0100);
}

/* Stick0200's callbacks (names inferred). */
static void InitializeStick0200() {
    fn_803A953C("%s::Initialize\n", s_nameStick0200);
    fn_8006B50C(0x2F, 0);
    fn_8006B50C(0x30, 0);
    fn_8005B760(0x23);
}

static void FinalizeStick0200() {
    fn_803A953C("%s::Finalize\n", s_nameStick0200);
}

static Weapon::WeaponBase *InstantiateStick0200(const boost::weak_ptr<CharInfo> &owner) {
    return new Stick(owner, 0x23, 1, &s_effectSetStick0200);
}

static Weapon::Item *CreateItemStick0200() {
    return new Weapon::Item((void *)1, &s_itemEffectSetStick0200);
}

/* Stick0201's callbacks (names inferred). */
static void InitializeStick0201() {
    fn_803A953C("%s::Initialize\n", s_nameStick0201);
    fn_8006B50C(0x24, 0);
    fn_8005B760(0x24);
}

static void FinalizeStick0201() {
    fn_803A953C("%s::Finalize\n", s_nameStick0201);
}

static Weapon::WeaponBase *InstantiateStick0201(const boost::weak_ptr<CharInfo> &owner) {
    return new Stick(owner, 0x24, 1, &s_effectSetStick0201);
}

static Weapon::Item *CreateItemStick0201() {
    return new Weapon::Item((void *)1, &s_itemEffectSetStick0201);
}

/* Stick0202's callbacks (names inferred). */
static void InitializeStick0202() {
    fn_803A953C("%s::Initialize\n", s_nameStick0202);
    fn_8006B50C(0x4D, 0);
    fn_8005B760(0x25);
}

static void FinalizeStick0202() {
    fn_803A953C("%s::Finalize\n", s_nameStick0202);
}

static Weapon::WeaponBase *InstantiateStick0202(const boost::weak_ptr<CharInfo> &owner) {
    return new Stick(owner, 0x25, 1, &s_effectSetStick0202);
}

static Weapon::Item *CreateItemStick0202() {
    return new Weapon::Item((void *)1, &s_itemEffectSetStick0202);
}

/* Stick0301's callbacks (names inferred). */
static void InitializeStick0301() {
    fn_803A953C("%s::Initialize\n", s_nameStick0301);
    fn_8006B50C(0x32, 0);
    fn_8006B50C(0x33, 0);
    fn_8005B760(0x27);
}

static void FinalizeStick0301() {
    fn_803A953C("%s::Finalize\n", s_nameStick0301);
}

static Weapon::WeaponBase *InstantiateStick0301(const boost::weak_ptr<CharInfo> &owner) {
    return new Stick(owner, 0x27, 1, &s_effectSetStick0301);
}

static Weapon::Item *CreateItemStick0301() {
    return new Weapon::Item((void *)1, &s_itemEffectSetStick0301);
}

/* Stick0302's callbacks (names inferred). */
static void InitializeStick0302() {
    fn_803A953C("%s::Initialize\n", s_nameStick0302);
    fn_8006B50C(0x42, 0);
    fn_8005B760(0x28);
}

static void FinalizeStick0302() {
    fn_803A953C("%s::Finalize\n", s_nameStick0302);
}

static Weapon::WeaponBase *InstantiateStick0302(const boost::weak_ptr<CharInfo> &owner) {
    return new Stick(owner, 0x28, 1, &s_effectSetStick0302);
}

static Weapon::Item *CreateItemStick0302() {
    return new Weapon::Item((void *)1, &s_itemEffectSetStick0302);
}

/* Stick0400's callbacks (names inferred). */
static void InitializeStick0400() {
    fn_803A953C("%s::Initialize\n", s_nameStick0400);
    fn_8006B50C(0x4E, 0);
    fn_8005B760(0x29);
}

static void FinalizeStick0400() {
    fn_803A953C("%s::Finalize\n", s_nameStick0400);
}

static Weapon::WeaponBase *InstantiateStick0400(const boost::weak_ptr<CharInfo> &owner) {
    return new Stick(owner, 0x29, 1, &s_effectSetStick0400);
}

static Weapon::Item *CreateItemStick0400() {
    return new Weapon::Item((void *)1, &s_itemEffectSetStick0400);
}

/* Stick0401's callbacks (names inferred). */
static void InitializeStick0401() {
    fn_803A953C("%s::Initialize\n", s_nameStick0401);
    fn_8006B50C(0x35, 0);
    fn_8006B50C(0x36, 0);
    fn_8005B760(0x2A);
}

static void FinalizeStick0401() {
    fn_803A953C("%s::Finalize\n", s_nameStick0401);
}

static Weapon::WeaponBase *InstantiateStick0401(const boost::weak_ptr<CharInfo> &owner) {
    return new Stick(owner, 0x2A, 1, &s_effectSetStick0401);
}

static Weapon::Item *CreateItemStick0401() {
    return new Weapon::Item((void *)1, &s_itemEffectSetStick0401);
}

/* Stick0402's callbacks (names inferred). */
static void InitializeStick0402() {
    fn_803A953C("%s::Initialize\n", s_nameStick0402);
    fn_8006B50C(0x24, 0);
    fn_8005B760(0x2B);
}

static void FinalizeStick0402() {
    fn_803A953C("%s::Finalize\n", s_nameStick0402);
}

static Weapon::WeaponBase *InstantiateStick0402(const boost::weak_ptr<CharInfo> &owner) {
    return new Stick(owner, 0x2B, 1, &s_effectSetStick0402);
}

static Weapon::Item *CreateItemStick0402() {
    return new Weapon::Item((void *)1, &s_itemEffectSetStick0402);
}

/* Stick0403's callbacks (names inferred). */
static void InitializeStick0403() {
    fn_803A953C("%s::Initialize\n", s_nameStick0403);
    fn_8006B50C(0x2F, 0);
    fn_8006B50C(0x30, 0);
    fn_8005B760(0x2C);
}

static void FinalizeStick0403() {
    fn_803A953C("%s::Finalize\n", s_nameStick0403);
}

static Weapon::WeaponBase *InstantiateStick0403(const boost::weak_ptr<CharInfo> &owner) {
    return new Stick(owner, 0x2C, 1, &s_effectSetStick0403);
}

static Weapon::Item *CreateItemStick0403() {
    return new Weapon::Item((void *)1, &s_itemEffectSetStick0403);
}

/* Stick0404's callbacks (names inferred). */
static void InitializeStick0404() {
    fn_803A953C("%s::Initialize\n", s_nameStick0404);
    fn_8006B50C(0x34, 0);
    fn_8006B50C(0x35, 0);
    fn_8005B760(0x2D);
}

static void FinalizeStick0404() {
    fn_803A953C("%s::Finalize\n", s_nameStick0404);
}

static Weapon::WeaponBase *InstantiateStick0404(const boost::weak_ptr<CharInfo> &owner) {
    return new Stick(owner, 0x2D, 1, &s_effectSetStick0404);
}

static Weapon::Item *CreateItemStick0404() {
    return new Weapon::Item((void *)1, &s_itemEffectSetStick0404);
}

/* Stick0501's callbacks (names inferred). */
static void InitializeStick0501() {
    fn_803A953C("%s::Initialize\n", s_nameStick0501);
    fn_8006B50C(0x34, 0);
    fn_8005B760(0x2F);
}

static void FinalizeStick0501() {
    fn_803A953C("%s::Finalize\n", s_nameStick0501);
}

static Weapon::WeaponBase *InstantiateStick0501(const boost::weak_ptr<CharInfo> &owner) {
    return new Stick(owner, 0x2F, 1, &s_effectSetStick0501);
}

static Weapon::Item *CreateItemStick0501() {
    return new Weapon::Item((void *)1, &s_itemEffectSetStick0501);
}

/* Stick0502's callbacks (names inferred). */
static void InitializeStick0502() {
    fn_803A953C("%s::Initialize\n", s_nameStick0502);
    fn_8006B50C(0x2A, 0);
    fn_8005B760(0x30);
}

static void FinalizeStick0502() {
    fn_803A953C("%s::Finalize\n", s_nameStick0502);
}

static Weapon::WeaponBase *InstantiateStick0502(const boost::weak_ptr<CharInfo> &owner) {
    return new Stick(owner, 0x30, 1, &s_effectSetStick0502);
}

static Weapon::Item *CreateItemStick0502() {
    return new Weapon::Item((void *)1, &s_itemEffectSetStick0502);
}

/* Stick0503's callbacks (names inferred). */
static void InitializeStick0503() {
    fn_803A953C("%s::Initialize\n", s_nameStick0503);
    fn_8006B50C(0x4D, 0);
    fn_8006B50C(0x4E, 0);
    fn_8005B760(0x31);
}

static void FinalizeStick0503() {
    fn_803A953C("%s::Finalize\n", s_nameStick0503);
}

static Weapon::WeaponBase *InstantiateStick0503(const boost::weak_ptr<CharInfo> &owner) {
    return new Stick(owner, 0x31, 1, &s_effectSetStick0503);
}

static Weapon::Item *CreateItemStick0503() {
    return new Weapon::Item((void *)1, &s_itemEffectSetStick0503);
}

/* Stick0504's callbacks (names inferred). */
static void InitializeStick0504() {
    fn_803A953C("%s::Initialize\n", s_nameStick0504);
    fn_8006B50C(0x34, 0);
    fn_8006B50C(0x35, 0);
    fn_8005B760(0x32);
}

static void FinalizeStick0504() {
    fn_803A953C("%s::Finalize\n", s_nameStick0504);
}

static Weapon::WeaponBase *InstantiateStick0504(const boost::weak_ptr<CharInfo> &owner) {
    return new Stick(owner, 0x32, 1, &s_effectSetStick0504);
}

static Weapon::Item *CreateItemStick0504() {
    return new Weapon::Item((void *)1, &s_itemEffectSetStick0504);
}

/* Stick0600's callbacks (names inferred). */
static void InitializeStick0600() {
    fn_803A953C("%s::Initialize\n", s_nameStick0600);
    fn_8006B50C(0x35, 0);
    fn_8005B760(0x33);
}

static void FinalizeStick0600() {
    fn_803A953C("%s::Finalize\n", s_nameStick0600);
}

static Weapon::WeaponBase *InstantiateStick0600(const boost::weak_ptr<CharInfo> &owner) {
    return new Stick(owner, 0x33, 1, &s_effectSetStick0600);
}

static Weapon::Item *CreateItemStick0600() {
    return new Weapon::Item((void *)1, &s_itemEffectSetStick0600);
}

/* Stick0601's callbacks (names inferred). */
static void InitializeStick0601() {
    fn_803A953C("%s::Initialize\n", s_nameStick0601);
    fn_8006B50C(0x4C, 0);
    fn_8005B760(0x34);
}

static void FinalizeStick0601() {
    fn_803A953C("%s::Finalize\n", s_nameStick0601);
}

static Weapon::WeaponBase *InstantiateStick0601(const boost::weak_ptr<CharInfo> &owner) {
    return new Stick(owner, 0x34, 1, &s_effectSetStick0601);
}

static Weapon::Item *CreateItemStick0601() {
    return new Weapon::Item((void *)1, &s_itemEffectSetStick0601);
}

/* Stick0602's callbacks (names inferred). */
static void InitializeStick0602() {
    fn_803A953C("%s::Initialize\n", s_nameStick0602);
    fn_8006B50C(0x4E, 0);
    fn_8005B760(0x35);
}

static void FinalizeStick0602() {
    fn_803A953C("%s::Finalize\n", s_nameStick0602);
}

static Weapon::WeaponBase *InstantiateStick0602(const boost::weak_ptr<CharInfo> &owner) {
    return new Stick(owner, 0x35, 1, &s_effectSetStick0602);
}

static Weapon::Item *CreateItemStick0602() {
    return new Weapon::Item((void *)1, &s_itemEffectSetStick0602);
}

/* Stick0603's callbacks (names inferred). */
static void InitializeStick0603() {
    fn_803A953C("%s::Initialize\n", s_nameStick0603);
    fn_8006B50C(0x4B, 0);
    fn_8006B50C(0x4C, 0);
    fn_8005B760(0x36);
}

static void FinalizeStick0603() {
    fn_803A953C("%s::Finalize\n", s_nameStick0603);
}

static Weapon::WeaponBase *InstantiateStick0603(const boost::weak_ptr<CharInfo> &owner) {
    return new Stick(owner, 0x36, 1, &s_effectSetStick0603);
}

static Weapon::Item *CreateItemStick0603() {
    return new Weapon::Item((void *)1, &s_itemEffectSetStick0603);
}

/* Stick0604's callbacks (names inferred). */
static void InitializeStick0604() {
    fn_803A953C("%s::Initialize\n", s_nameStick0604);
    fn_8006B50C(0x4C, 0);
    fn_8005B760(0x37);
}

static void FinalizeStick0604() {
    fn_803A953C("%s::Finalize\n", s_nameStick0604);
}

static Weapon::WeaponBase *InstantiateStick0604(const boost::weak_ptr<CharInfo> &owner) {
    return new Stick(owner, 0x37, 1, &s_effectSetStick0604);
}

static Weapon::Item *CreateItemStick0604() {
    return new Weapon::Item((void *)1, &s_itemEffectSetStick0604);
}

/* Stick0700's callbacks (names inferred). */
static void InitializeStick0700() {
    fn_803A953C("%s::Initialize\n", s_nameStick0700);
    fn_8006B50C(0x4C, 0);
    fn_8005B760(0x38);
}

static void FinalizeStick0700() {
    fn_803A953C("%s::Finalize\n", s_nameStick0700);
}

static Weapon::WeaponBase *InstantiateStick0700(const boost::weak_ptr<CharInfo> &owner) {
    return new Stick(owner, 0x38, 1, &s_effectSetStick0700);
}

static Weapon::Item *CreateItemStick0700() {
    return new Weapon::Item((void *)1, &s_itemEffectSetStick0700);
}

UnkWeaponTaskInfo lbl_8051EFC4 = {
    s_nameSurvivalKnife, InitializeSurvivalKnife, FinalizeSurvivalKnife, 0, 0, 0x80, 0x00FA0400, 0, 0,
};

UnkWeaponTypeInfo lbl_8051EFE8 = {
    s_nameSurvivalKnife, InstantiateSurvivalKnife, CreateItemSurvivalKnife, 0, Span(1.0f), Span(5.0f), 6, 2,
    0x2001, 0.0f, 0.0f, 0,
};

UnkWeaponTaskInfo lbl_8051F018 = {
    s_nameEggSpear, InitializeEggSpear, FinalizeEggSpear, 0, 0, 0x80, 0x00FD0400, 0, 0,
};

UnkWeaponTypeInfo lbl_8051F03C = {
    s_nameEggSpear, InstantiateEggSpear, CreateItemEggSpear, 0, Span(1.0f), Span(5.0f), 6, 3,
    0x2001, 0.0f, 0.0f, 0,
};

UnkWeaponTaskInfo lbl_8051F06C = {
    s_nameStick0100, InitializeStick0100, FinalizeStick0100, 0, 0, 0x80, 0x00FE0400, 0, 0,
};

UnkWeaponTypeInfo lbl_8051F090 = {
    s_nameStick0100, InstantiateStick0100, CreateItemStick0100, 0, Span(1.0f), Span(5.0f), 4, 3,
    0x2001, 0.0f, 0.0f, 0,
};

UnkWeaponTaskInfo lbl_8051F0C0 = {
    s_nameStick0200, InitializeStick0200, FinalizeStick0200, 0, 0, 0x80, 0x00FF0400, 0, 0,
};

UnkWeaponTypeInfo lbl_8051F0E4 = {
    s_nameStick0200, InstantiateStick0200, CreateItemStick0200, 0, Span(1.0f), Span(5.0f), 4, 3,
    0x2001, 0.0f, 0.0f, 0,
};

UnkWeaponTaskInfo lbl_8051F114 = {
    s_nameStick0201, InitializeStick0201, FinalizeStick0201, 0, 0, 0x80, 0x01000400, 0, 0,
};

UnkWeaponTypeInfo lbl_8051F138 = {
    s_nameStick0201, InstantiateStick0201, CreateItemStick0201, 0, Span(1.0f), Span(5.0f), 4, 3,
    0x2001, 0.0f, 0.0f, 0,
};

UnkWeaponTaskInfo lbl_8051F168 = {
    s_nameStick0202, InitializeStick0202, FinalizeStick0202, 0, 0, 0x80, 0x01010400, 0, 0,
};

UnkWeaponTypeInfo lbl_8051F18C = {
    s_nameStick0202, InstantiateStick0202, CreateItemStick0202, 0, Span(1.0f), Span(5.0f), 4, 3,
    0x2001, 0.0f, 0.0f, 0,
};

UnkWeaponTaskInfo lbl_8051F1BC = {
    s_nameStick0301, InitializeStick0301, FinalizeStick0301, 0, 0, 0x80, 0x01030400, 0, 0,
};

UnkWeaponTypeInfo lbl_8051F1E0 = {
    s_nameStick0301, InstantiateStick0301, CreateItemStick0301, 0, Span(1.0f), Span(5.0f), 4, 3,
    0x2001, 0.0f, 0.0f, 0,
};

UnkWeaponTaskInfo lbl_8051F210 = {
    s_nameStick0302, InitializeStick0302, FinalizeStick0302, 0, 0, 0x80, 0x01040400, 0, 0,
};

UnkWeaponTypeInfo lbl_8051F234 = {
    s_nameStick0302, InstantiateStick0302, CreateItemStick0302, 0, Span(1.0f), Span(5.0f), 4, 3,
    0x2001, 0.0f, 0.0f, 0,
};

UnkWeaponTaskInfo lbl_8051F264 = {
    s_nameStick0400, InitializeStick0400, FinalizeStick0400, 0, 0, 0x80, 0x01050400, 0, 0,
};

UnkWeaponTypeInfo lbl_8051F288 = {
    s_nameStick0400, InstantiateStick0400, CreateItemStick0400, 0, Span(1.0f), Span(5.0f), 4, 3,
    0x2001, 0.0f, 0.0f, 0,
};

UnkWeaponTaskInfo lbl_8051F2B8 = {
    s_nameStick0401, InitializeStick0401, FinalizeStick0401, 0, 0, 0x80, 0x01060400, 0, 0,
};

UnkWeaponTypeInfo lbl_8051F2DC = {
    s_nameStick0401, InstantiateStick0401, CreateItemStick0401, 0, Span(1.0f), Span(5.0f), 4, 3,
    0x2001, 0.0f, 0.0f, 0,
};

UnkWeaponTaskInfo lbl_8051F30C = {
    s_nameStick0402, InitializeStick0402, FinalizeStick0402, 0, 0, 0x80, 0x01070400, 0, 0,
};

UnkWeaponTypeInfo lbl_8051F330 = {
    s_nameStick0402, InstantiateStick0402, CreateItemStick0402, 0, Span(1.0f), Span(5.0f), 4, 3,
    0x2001, 0.0f, 0.0f, 0,
};

UnkWeaponTaskInfo lbl_8051F360 = {
    s_nameStick0403, InitializeStick0403, FinalizeStick0403, 0, 0, 0x80, 0x01080400, 0, 0,
};

UnkWeaponTypeInfo lbl_8051F384 = {
    s_nameStick0403, InstantiateStick0403, CreateItemStick0403, 0, Span(1.0f), Span(5.0f), 4, 3,
    0x2001, 0.0f, 0.0f, 0,
};

UnkWeaponTaskInfo lbl_8051F3B4 = {
    s_nameStick0404, InitializeStick0404, FinalizeStick0404, 0, 0, 0x80, 0x01090400, 0, 0,
};

UnkWeaponTypeInfo lbl_8051F3D8 = {
    s_nameStick0404, InstantiateStick0404, CreateItemStick0404, 0, Span(1.0f), Span(5.0f), 4, 3,
    0x2001, 0.0f, 0.0f, 0,
};

UnkWeaponTaskInfo lbl_8051F408 = {
    s_nameStick0501, InitializeStick0501, FinalizeStick0501, 0, 0, 0x80, 0x010B0400, 0, 0,
};

UnkWeaponTypeInfo lbl_8051F42C = {
    s_nameStick0501, InstantiateStick0501, CreateItemStick0501, 0, Span(1.0f), Span(5.0f), 4, 3,
    0x2001, 0.0f, 0.0f, 0,
};

UnkWeaponTaskInfo lbl_8051F45C = {
    s_nameStick0502, InitializeStick0502, FinalizeStick0502, 0, 0, 0x80, 0x010C0400, 0, 0,
};

UnkWeaponTypeInfo lbl_8051F480 = {
    s_nameStick0502, InstantiateStick0502, CreateItemStick0502, 0, Span(1.0f), Span(5.0f), 4, 3,
    0x2001, 0.0f, 0.0f, 0,
};

UnkWeaponTaskInfo lbl_8051F4B0 = {
    s_nameStick0503, InitializeStick0503, FinalizeStick0503, 0, 0, 0x80, 0x010D0400, 0, 0,
};

UnkWeaponTypeInfo lbl_8051F4D4 = {
    s_nameStick0503, InstantiateStick0503, CreateItemStick0503, 0, Span(1.0f), Span(5.0f), 4, 3,
    0x2001, 0.0f, 0.0f, 0,
};

UnkWeaponTaskInfo lbl_8051F504 = {
    s_nameStick0504, InitializeStick0504, FinalizeStick0504, 0, 0, 0x80, 0x010E0400, 0, 0,
};

UnkWeaponTypeInfo lbl_8051F528 = {
    s_nameStick0504, InstantiateStick0504, CreateItemStick0504, 0, Span(1.0f), Span(5.0f), 4, 3,
    0x2001, 0.0f, 0.0f, 0,
};

UnkWeaponTaskInfo lbl_8051F558 = {
    s_nameStick0600, InitializeStick0600, FinalizeStick0600, 0, 0, 0x80, 0x010F0400, 0, 0,
};

UnkWeaponTypeInfo lbl_8051F57C = {
    s_nameStick0600, InstantiateStick0600, CreateItemStick0600, 0, Span(1.0f), Span(5.0f), 4, 3,
    0x2001, 0.0f, 0.0f, 0,
};

UnkWeaponTaskInfo lbl_8051F5AC = {
    s_nameStick0601, InitializeStick0601, FinalizeStick0601, 0, 0, 0x80, 0x01100400, 0, 0,
};

UnkWeaponTypeInfo lbl_8051F5D0 = {
    s_nameStick0601, InstantiateStick0601, CreateItemStick0601, 0, Span(1.0f), Span(5.0f), 4, 3,
    0x2001, 0.0f, 0.0f, 0,
};

UnkWeaponTaskInfo lbl_8051F600 = {
    s_nameStick0602, InitializeStick0602, FinalizeStick0602, 0, 0, 0x80, 0x01110400, 0, 0,
};

UnkWeaponTypeInfo lbl_8051F624 = {
    s_nameStick0602, InstantiateStick0602, CreateItemStick0602, 0, Span(1.0f), Span(5.0f), 4, 3,
    0x2001, 0.0f, 0.0f, 0,
};

UnkWeaponTaskInfo lbl_8051F654 = {
    s_nameStick0603, InitializeStick0603, FinalizeStick0603, 0, 0, 0x80, 0x01120400, 0, 0,
};

UnkWeaponTypeInfo lbl_8051F678 = {
    s_nameStick0603, InstantiateStick0603, CreateItemStick0603, 0, Span(1.0f), Span(5.0f), 4, 3,
    0x2001, 0.0f, 0.0f, 0,
};

UnkWeaponTaskInfo lbl_8051F6A8 = {
    s_nameStick0604, InitializeStick0604, FinalizeStick0604, 0, 0, 0x80, 0x01130400, 0, 0,
};

UnkWeaponTypeInfo lbl_8051F6CC = {
    s_nameStick0604, InstantiateStick0604, CreateItemStick0604, 0, Span(1.0f), Span(5.0f), 4, 3,
    0x2001, 0.0f, 0.0f, 0,
};

UnkWeaponTaskInfo lbl_8051F6FC = {
    s_nameStick0700, InitializeStick0700, FinalizeStick0700, 0, 0, 0x80, 0x01140400, 0, 0,
};

UnkWeaponTypeInfo lbl_8051F720 = {
    s_nameStick0700, InstantiateStick0700, CreateItemStick0700, 0, Span(1.0f), Span(5.0f), 4, 3,
    0x2001, 0.0f, 0.0f, 0,
};
