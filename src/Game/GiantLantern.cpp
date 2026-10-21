/* GiantLantern (class name from RTTI): the gadget type of the same name, a giant lantern (a
 * spherical collision of radius 30) that the attacks of attackers with flag 0x400 (a flame) light.
 * Its lit state lives in a GiantLanternSetState (class name from RTTI) kept by the owner record, so
 * it survives the gadget being recreated. Lighting it shows a flame effect, starts a looping sound,
 * turns on the switch whose id the owner supplies and hits the first entry of the manager at
 * fn_8006D6A8. An unlit lantern keeps a positioned object (a sound source, probably) 13 units above
 * itself. File name inferred from the class. Names marked "(name inferred)" are not original.
 * Definitions are in reverse address order (deferred inlining emits functions last-defined
 * first). */
#include <boost/bind.hpp>
#include <boost/function.hpp>

#include "Effect/System/EffectElement.h"
#include "Game/CharInfo.h"
#include "Game/CharaColli.h"
#include "Game/CharaColliAttack.h"
#include "Game/Gadget.h"
#include "Game/GadgetCharInfo.h"
#include "Game/GadgetSound.h"
#include "Game/GadgetClassDesc.h"
#include "Game/GadgetUtil.h"
#include "Game/SetAdapterState.h"
#include "Game/UnkSwitchBank_800CB69C.h"
#include "boost/shared_ptr.hpp"
#include "boost/weak_ptr.hpp"
#include "size_assert.h"

/* The orphan RTTI base lists that start this unit's .data (see docs/matching-gadgets.md). */
#include "Game/TNumEdit.h"

#include "Game/CharaColliBaseCharInfo.h"

using SonicteamUSA::System::CharaColli::AttackCallback;
using SonicteamUSA::System::CharaColli::AttackCallbackParam;
using SonicteamUSA::System::CharaColli::CharaColliAttack;
using SonicteamUSA::System::CharaColli::CharaColliBase;

/* The positioned object fn_802B4254 creates (see ElecBomb.cpp; it may be a sound source). */
class UnkSound_802B4254;

/* An object kept by the manager returned by fn_8006D6A8 (see ElecBomb.cpp). */
class UnkPlayer_800740E4 {
  public:
    virtual void vf08();
    virtual void vf0C();
    virtual void vf10();
    virtual void vf14();
    virtual void vf18();
    virtual void vf1C();
    virtual void vf20();
    /* Hits the object (name and meaning of the arguments inferred). */
    virtual void Hit(int arg0, int arg1);
};

class UnkPlayerManager_8006D6A8;
class UnkColliderManager_80021B08;

/* fn_8040CD74 called as returning a - b by value (it stores the result through its first
 * argument; see ArkCannon.cpp). */
typedef Vec3f (*VectorSubtractFn)(const Vec3f *a, const Vec3f *b);

extern "C" {
/* A 32-bit flag word: fn_8000AAEC constructs one from a value, fn_80014E04 constructs it cleared,
 * fn_80014E10 clears it, fn_80014E1C sets bit, fn_8006127C clears bit and fn_80014E34 tests bit
 * (behaviour from their bodies). */
u32 *fn_8000AAEC(u32 *flags, u32 value);
void fn_80014E04(u32 *flags);
void fn_80014E10(u32 *flags);
void fn_80014E1C(u32 *flags, int bit);
void fn_8006127C(u32 *flags, int bit);
bool fn_80014E34(u32 *flags, int bit);

/* The collider manager, a collision for owner and the attack collision of a collider (see
 * ElecKyuu.cpp, GunLift.cpp and TurnOver.cpp). */
UnkColliderManager_80021B08 *fn_80021B08(void);
boost::shared_ptr<CharaColliBase> fn_800C7DEC(UnkColliderManager_80021B08 *manager,
                                              const boost::weak_ptr<CharInfo> &owner, int flags,
                                              int arg3, int arg4, const Vec3f *offset,
                                              float radius);
boost::weak_ptr<CharaColliAttack> fn_800C42D0(UnkColliderManager_80021B08 *manager,
                                              boost::shared_ptr<CharaColliBase> collider);
boost::weak_ptr<CharaColliAttack> fn_800C4374(UnkColliderManager_80021B08 *manager,
                                              boost::shared_ptr<CharaColliBase> collider);
/* Sets callback as the attack callback of an attack collision (see BreakRoad.cpp), and sets its
 * two hit callbacks (see TurnOver.cpp). */
void fn_80096B64(CharaColliAttack *attack, int arg1, const AttackCallback &callback);
void fn_80054DD0(CharaColliAttack *attack, int arg1, int arg2, float arg3, const AttackCallback &onHit,
                 const AttackCallback &onEnd);
/* A test on the attack (meaning unknown; see CandleStick.cpp). */
bool fn_8015D4E4(AttackCallbackParam &param);

/* fn_8040CDD0 called as returning a + b by value (it stores the result through its first
 * argument). */
Vec3f fn_8040CDD0(const Vec3f *a, const Vec3f *b);
/* Normalises vector; returns the former length. */
float fn_8040CE74(Vec3f *vector);

/* Starts effect id at position, pointing along direction, and moves an effect (behaviour inferred;
 * see CandleStick.cpp). */
boost::weak_ptr<Effect::System::EffectElement> fn_80168C48(const Vec3f *position,
                                                           const Vec3f *direction, int id);
void fn_80425E4C(Effect::System::EffectElement *effect, const Vec3f *position);
/* Starts the looping sound soundId at charInfo's position for owner, and plays sound id at a
 * character info (behaviour inferred; see CandleStick.cpp). */
UnkSoundHandle_801D3DC8 fn_801D3E58(int soundId, GadgetCharInfo *charInfo, void *owner);
void fn_801D54F4(void *manager, int id, GadgetCharInfo *charInfo, int arg3, int arg4);

/* The positioned object (see ElecBomb.cpp and ChaoHolder.cpp). */
boost::shared_ptr<UnkSound_802B4254> fn_802B4254(int kind, int arg1);
void fn_802B4498(UnkSound_802B4254 *sound, float range);
void fn_802B4510(UnkSound_802B4254 *sound, u8 flag);
void fn_802B44C8(UnkSound_802B4254 *sound, const Vec3f *position);
void fn_800CC978(boost::shared_ptr<UnkSound_802B4254> *sound);

/* The manager and its entry by index (see ElecBomb.cpp). */
UnkPlayerManager_8006D6A8 *fn_8006D6A8(void);
boost::weak_ptr<UnkPlayer_800740E4> fn_800740E4(UnkPlayerManager_8006D6A8 *manager, int index);
/* Turns switch id on (behaviour inferred; see UnkSwitchBank_800CB69C.h). */
void fn_8016907C(UnkSwitchBank_800CB69C *manager, int id, int arg2);
/* Tells the owner the gadget changed (behaviour unknown; see ChaoHolder.cpp). */
void fn_800C985C(UnkPlacement_800C9CD0 *owner);
}

/* A length (name inferred). As a call it is not a constant expression, so the constants below are
 * stored by the static initializer, as in retail (as in BASwitch.cpp). */
inline float Span(float length) {
    return length;
}

/* The direction of the flame (name inferred). The original copy-constructs it from a temporary
 * through the vector type's out-of-line copy constructor, fn_800091EC (as in CandleStick.cpp). */
static struct GiantLanternFlameDirection {
    GiantLanternFlameDirection() {
        float value[3];

        value[0] = 0.0f;
        value[1] = 0.0f;
        value[2] = -1.0f;
        fn_800091EC((Vec3f *)m_value, (Vec3f *)value);
    }

    const Vec3f *Get() const {
        return (const Vec3f *)m_value;
    }

    float m_value[3];
} s_flameDirection;

/* How high above the lantern the positioned object sits, and its range (names inferred). */
static const float s_soundHeight = Span(13.0f);
static const float s_soundRange = Span(400.0f);

/* A 32-bit flag word (class name unknown). */
struct GiantLanternFlags {
    GiantLanternFlags() {
        fn_80014E04(&m_bits);
    }
    GiantLanternFlags(u32 value) {
        fn_8000AAEC(&m_bits, value);
    }
    void Clear() {
        fn_80014E10(&m_bits);
    }
    void Set(int bit) {
        fn_80014E1C(&m_bits, bit);
    }
    void Reset(int bit) {
        fn_8006127C(&m_bits, bit);
    }
    bool Test(int bit) {
        return fn_80014E34(&m_bits, bit);
    }

    u32 m_bits;
};

/* GiantLanternSetState (class name from RTTI): the lantern's state that the owner record keeps
 * (member names inferred). */
class GiantLanternSetState : public SetAdapterState {
  public:
    GiantLanternSetState() {
        m_flags.Clear();
    }

    /* 0x4 */ GiantLanternFlags m_flags; /* bit 0 lit */
};
SIZE_ASSERT(GiantLanternSetState, 0x8);

class GiantLantern : public Gadget {
  public:
    GiantLantern(Task *parent, UnkPlacement_800C9CD0 *owner);
    virtual ~GiantLantern();
    virtual void Behave(float dt);
    virtual void ReloadPlacement(UnkPlacement_800C9CD0 *owner);

    /* Creates the gadget under task layer 11 (name inferred). */
    static void Instantiate(UnkPlacement_800C9CD0 *owner);

    /* Method names are inferred. */
    /* Copies the owner's position, rotation and switch id. */
    void ReadPlacement(UnkPlacement_800C9CD0 *owner);
    /* Moves the flame and the positioned object along with the lantern. */
    void UpdatePositions();
    /* The callbacks of the collision's attack: being attacked, hitting something and the end of a
     * hit. */
    void OnAttack(AttackCallbackParam &param);
    void OnHit(AttackCallbackParam &param);
    void OnAttackEnd(AttackCallbackParam &param);

    static const char *s_taskName;

    /* Member names are inferred. */
    /* 0x30 */ boost::shared_ptr<GadgetCharInfo> m_chara;
    /* 0x38 */ boost::shared_ptr<CharaColliBase> m_collider;
    /* 0x40 */ boost::shared_ptr<UnkSound_802B4254> m_sound;
    /* 0x48 */ boost::weak_ptr<Effect::System::EffectElement> m_flame;
    /* 0x50 */ GiantLanternFlags m_flags; /* bit 0 light now */
    /* 0x54 */ GiantLanternSetState *m_state;
    /* 0x58 */ UnkSoundHandle_801D3DC8 m_loopSound;
    /* 0x74 */ u8 m_switchId;
};
SIZE_ASSERT(GiantLantern, 0x78);

const char *GiantLantern::s_taskName = "GiantLantern";

GiantLantern::GiantLantern(Task *parent, UnkPlacement_800C9CD0 *owner)
    : Gadget(parent, owner), m_flags(0) {
    m_name = s_taskName;
    m_flame.reset();
    if (owner->m_adapterState == 0) {
        fn_800C9930(owner, new GiantLanternSetState);
    }
    m_state = dynamic_cast<GiantLanternSetState *>(owner->m_adapterState);
    m_chara = fn_8015CD90(0x4D, 4);
    ReadPlacement(owner);
    m_collider = fn_800C7DEC(fn_80021B08(), m_chara, 0x40100, 7, 0, &lbl_805E07A0, 30.0f);
    {
        boost::shared_ptr<CharaColliAttack> attack = fn_800C4374(fn_80021B08(), m_collider).lock();
        if (attack.get() != 0) {
            fn_80096B64(attack.get(), 1, boost::bind(&GiantLantern::OnAttack, this, _1));
            fn_80054DD0(attack.get(), 1, 0x401, 1.0f, boost::bind(&GiantLantern::OnHit, this, _1),
                        boost::bind(&GiantLantern::OnAttackEnd, this, _1));
        }
    }
    if (m_state->m_flags.Test(0)) {
        m_flame = fn_80168C48(&m_chara->m_position, s_flameDirection.Get(), 0x1E);
        m_loopSound = fn_801D3E58(0x7430, m_chara.get(), this);
        boost::shared_ptr<CharaColliAttack> attack = fn_800C42D0(fn_80021B08(), m_collider).lock();
        if (attack.get() != 0) {
            attack->field_0x55 = 0;
        }
    } else {
        m_sound = fn_802B4254(0, 0);
        fn_802B4498(m_sound.get(), s_soundRange);
        fn_802B4510(m_sound.get(), 1);
        Vec3f offset(0.0f, s_soundHeight, 0.0f);
        fn_802B44C8(m_sound.px, &(const Vec3f &)fn_8040CDD0(&offset, &m_chara->m_position));
        boost::shared_ptr<CharaColliAttack> attack = fn_800C42D0(fn_80021B08(), m_collider).lock();
        if (attack.get() != 0) {
            attack->field_0x54 = 0;
        }
    }
}

GiantLantern::~GiantLantern() {
    if (!m_flame.expired()) {
        boost::shared_ptr<Effect::System::EffectElement> flame = m_flame.lock();
        if (flame.get() != 0) {
            flame->Retire();
        }
        m_flame.reset();
    }
    m_loopSound.fn_801D455C();
}

extern "C" void fn_80236D10(void) {}

extern "C" void fn_80236D0C(void) {}

void GiantLantern::Instantiate(UnkPlacement_800C9CD0 *owner) {
    new GiantLantern(fn_801EB304(fn_8001C4F8(), 11), owner);
}

void GiantLantern::Behave(float dt) {
    if (m_flags.Test(0)) {
        if (m_flame.expired()) {
            m_flame = fn_80168C48(&m_chara->m_position, s_flameDirection.Get(), 0x1E);
        }
        fn_801D54F4(fn_80053164(), 0x7431, m_chara.get(), 0, 1);
        m_loopSound = fn_801D3E58(0x7430, m_chara.get(), this);
        {
            boost::shared_ptr<CharaColliAttack> attack =
                fn_800C42D0(fn_80021B08(), m_collider).lock();
            if (attack.get() != 0) {
                attack->field_0x55 = 0;
                attack->field_0x54 = 1;
            }
        }
        fn_8016907C(fn_800CB69C(), m_switchId, 1);
        m_state->m_flags.Set(0);
        {
            boost::shared_ptr<UnkPlayer_800740E4> player = fn_800740E4(fn_8006D6A8(), 0).lock();
            if (player.get() != 0) {
                player->Hit(1, 0);
            }
        }
        fn_800C985C(m_owner);
        fn_800CC978(&m_sound);
        m_flags.Reset(0);
    }
}

void GiantLantern::ReloadPlacement(UnkPlacement_800C9CD0 *owner) {
    ReadPlacement(owner);
    UpdatePositions();
    if (m_collider.get() != 0) {
        m_collider->Reset();
    }
}

void GiantLantern::ReadPlacement(UnkPlacement_800C9CD0 *owner) {
    fn_800C9F14(owner, &m_chara->m_position);
    fn_800C9ED4(owner, &m_chara->m_rotation);
    fn_800C9E60(owner, &m_switchId);
}

void GiantLantern::UpdatePositions() {
    if (!m_flame.expired()) {
        boost::shared_ptr<Effect::System::EffectElement> flame = m_flame.lock();
        if (flame.get() != 0) {
            fn_80425E4C(flame.get(), &m_chara->m_position);
        }
    }
    if (m_sound.get() != 0) {
        Vec3f offset(0.0f, s_soundHeight, 0.0f);
        fn_802B44C8(m_sound.px, &(const Vec3f &)fn_8040CDD0(&offset, &m_chara->m_position));
    }
}

void GiantLantern::OnAttack(AttackCallbackParam &param) {
    if (!m_state->m_flags.Test(0) && fn_8015D4E4(param)) {
        boost::shared_ptr<CharaColliAttack> attacker = param.m_attacker.lock();
        if (attacker.get() != 0 && (attacker->m_flags & 0x400)) {
            m_flags.Set(0);
            param.m_result = 1;
            return;
        }
    }
    param.m_result = 5;
}

void GiantLantern::OnHit(AttackCallbackParam &param) {
    /* The position of what was hit at 0x18, and the direction and speed of the hit at 0x34 and
     * 0x4C (meanings inferred; see Meteor.cpp). */
    fn_800091D0(reinterpret_cast<Vec3f *>(param.field_0x8 + (0x34 - 0x8)),
                &(const Vec3f &)((VectorSubtractFn)fn_8040CD74)(
                    reinterpret_cast<Vec3f *>(param.field_0x8 + (0x18 - 0x8)),
                    &m_chara->m_position));
    fn_8040CE74(reinterpret_cast<Vec3f *>(param.field_0x8 + (0x34 - 0x8)));
    *reinterpret_cast<float *>(param.field_0x8 + (0x4C - 0x8)) = 10.0f;
    param.m_damage = 1.0f;
}

void GiantLantern::OnAttackEnd(AttackCallbackParam &param) {}

GadgetClassDesc lbl_80547948 = {
    "GiantLantern", fn_80236D10, fn_80236D0C, GiantLantern::Instantiate, 0, 0, 0xBC9, 1, 0, 10, 0,
};
