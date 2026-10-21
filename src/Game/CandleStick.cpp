/* CandleStick (class name from RTTI): the CandleStick gadget, a candlestick (CANDLESTICK.BIN) that
 * some attacks light (those of attackers with flag 0x400; a flame, by the effect it shows) and that
 * attacks wear down. Its state lives in a CandleStickSetState
 * (class name from RTTI) kept by the owner record, so it survives the gadget being recreated. A
 * lit candlestick shows a flame effect and counts towards the switch whose id the owner supplies,
 * which turns on once "SAME_COMID_SWITCH_NUM" candlesticks with that id are lit; a candlestick
 * worn down breaks into debris. File name inferred from the class. Names marked "(name inferred)"
 * are not original. Definitions are in reverse address order (deferred inlining emits functions
 * last-defined first). */
#include <boost/bind.hpp>
#include <boost/function.hpp>

#include "Effect/System/EffectElement.h"
#include "Game/CharInfo.h"
#include "Game/CharaColliAttack.h"
#include "Game/Gadget.h"
#include "Game/GadgetCharInfo.h"
#include "Game/GadgetModelInfo.h"
#include "Game/GadgetSound.h"
#include "Game/GadgetClassDesc.h"
#include "Game/GadgetUtil.h"
#include "Game/SetAdapterState.h"
#include "Game/TrigerCounter.h"
#include "Game/FileQueue.h"
#include "boost/shared_ptr.hpp"
#include "boost/weak_ptr.hpp"
#include "size_assert.h"

using SonicteamUSA::System::CharaColli::AttackCallback;
using SonicteamUSA::System::CharaColli::AttackCallbackParam;
using SonicteamUSA::System::CharaColli::CharaColliAttack;

/* The parameters of a debris spawn (layout as in CaptureCage.cpp). */
struct UnkDebrisParam_8015F0B8 {
    UnkDebrisParam_8015F0B8(const boost::shared_ptr<CharInfo> &info, int index);

    /* 0x00 */ u8 field_0x0[0x34];
    /* 0x34 */ float scale;
    /* 0x38 */ u8 field_0x38[0x40 - 0x38];
    /* 0x40 */ float field_0x40;
    /* 0x44 */ s32 model;
    /* 0x48 */ u8 field_0x48[0x4A - 0x48];
    /* 0x4A */ u8 field_0x4A;
    /* 0x4B */ u8 field_0x4B;
    /* 0x4C */ s32 field_0x4C;
    /* 0x50 */ u8 field_0x50[0x54 - 0x50];
    /* 0x54 */ u8 field_0x54;
    /* 0x55 */ u8 field_0x55[0x5C - 0x55];
};
SIZE_ASSERT(UnkDebrisParam_8015F0B8, 0x5C);

extern "C" {
/* A 32-bit flag word: fn_80014E04 constructs it cleared, fn_80014E10 clears it, fn_80014E1C sets
 * bit, fn_8006127C clears bit and fn_80014E34 tests bit (behaviour from their bodies). */
void fn_80014E04(u32 *flags);
void fn_80014E10(u32 *flags);
void fn_80014E1C(u32 *flags, int bit);
void fn_8006127C(u32 *flags, int bit);
bool fn_80014E34(u32 *flags, int bit);
/* Loads a gadget model (see GadgetUtil.h). */
boost::shared_ptr<GadgetModelInfo> fn_8015F800(int arg0, const char *fileName);
/* Attaches a character info to the collision of a model variant, and returns the attack of a
 * variant's collision (behaviour inferred). */
bool fn_80161FC8(GadgetModelInfo *model, int variant, const boost::weak_ptr<CharInfo> &info);
boost::weak_ptr<CharaColliAttack> fn_80161428(GadgetModelInfo *model, int variant);
/* Sets callback as the attack callback of an attack collision (see BreakRoad.cpp). */
void fn_80096B64(CharaColliAttack *attack, int arg1, const AttackCallback &callback);
/* A test on the attack (meaning unknown), and its attacker (behaviour inferred). */
bool fn_8015D4E4(AttackCallbackParam &param);
boost::shared_ptr<CharInfo> fn_8015D570(AttackCallbackParam &param);
/* Gives the attacker the reward for breaking the object (behaviour inferred). */
void fn_8020642C(const boost::shared_ptr<CharInfo> &attacker, int reward);
/* Whether |value| is below a small tolerance (from its body). */
bool fn_8000B044(float value);
/* a + b. */
Vec3f fn_8040CDD0(const Vec3f *a, const Vec3f *b);
/* Starts effect id at position, pointing along direction (behaviour inferred). */
boost::weak_ptr<Effect::System::EffectElement> fn_80168C48(const Vec3f *position,
                                                           const Vec3f *direction, int id);
/* Moves the effect to position (behaviour inferred). */
void fn_80425E4C(Effect::System::EffectElement *effect, const Vec3f *position);
/* Starts the looping sound soundId at charInfo's position for owner (behaviour inferred; see
 * fn_801D3DC8 in GadgetSound.h). */
UnkSoundHandle_801D3DC8 fn_801D3E58(int soundId, GadgetCharInfo *charInfo, void *owner);
/* Plays sound id at the character info's position (behaviour inferred). */
void fn_801D54F4(void *manager, int id, GadgetCharInfo *charInfo, int arg3, int arg4);
/* The file loader, and a file of it (see BreakObj.cpp). */
void *fn_800122D4(void);
s32 fn_80012700(void *manager, int arg1, const char *fileName);
/* Launches the debris described by param (behaviour inferred). */
void fn_8015D93C(UnkDebrisParam_8015F0B8 *param);
}

/* A 32-bit flag word (class name unknown). */
struct CandleStickFlags {
    CandleStickFlags() {
        fn_80014E04(&m_bits);
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

/* CandleStickSetState (class name from RTTI): the candlestick's state that the owner record keeps
 * (member names inferred). */
class CandleStickSetState : public SetAdapterState {
  public:
    CandleStickSetState() {
        m_health = 2.0f;
        m_flags.Clear();
    }

    /* 0x4 */ float m_health;
    /* 0x8 */ CandleStickFlags m_flags; /* bit 0 broken, bit 1 lit */
};
SIZE_ASSERT(CandleStickSetState, 0xC);

/* The parameters the owner supplies (name from the parameter table). */
struct CandleStickParams {
    /* 0x0 */ s32 count; /* SAME_COMID_SWITCH_NUM */
};

class CandleStick : public Gadget {
  public:
    CandleStick(Task *parent, UnkPlacement_800C9CD0 *owner);
    virtual ~CandleStick();
    virtual void Behave(float dt);
    virtual void ReloadPlacement(UnkPlacement_800C9CD0 *owner);

    /* Creates the gadget under task layer 11 (name inferred). */
    static void Instantiate(UnkPlacement_800C9CD0 *owner);

    /* Method names are inferred. */
    /* Copies the owner's parameters, switch id, position and rotation. */
    void LoadFromOwner(UnkPlacement_800C9CD0 *owner);
    /* Shows the model for the state and attaches the collision. */
    void SetupModel();
    /* Places the model and the flame. */
    void RebuildTransform();
    /* The attack callback of the model's collision. */
    void OnAttack(AttackCallbackParam &param);

    static const char *s_taskName;

    /* Member names are inferred. */
    /* 0x30 */ boost::shared_ptr<GadgetCharInfo> m_chara;
    /* 0x38 */ boost::shared_ptr<GadgetModelInfo> m_model;
    /* 0x40 */ boost::weak_ptr<Effect::System::EffectElement> m_flame;
    /* 0x48 */ u8 m_switchId;
    /* 0x4C */ CandleStickFlags m_flags; /* bit 0 break now, bit 1 light now */
    /* 0x50 */ CandleStickSetState *m_state;
    /* 0x54 */ CandleStickParams m_params;
    /* 0x58 */ UnkSoundHandle_801D3DC8 m_sound;
    /* 0x74 */ u32 pad_0x74; /* the class size is a multiple of 8 */
};
SIZE_ASSERT(CandleStick, 0x78);

/* The model of the debris (name inferred). */
static const char *s_debrisFile = "PR0300CANDLESTICK_BRK.DFF";

const char *CandleStick::s_taskName = "CandleStick";

/* The flame's offset from the candlestick and its direction (names inferred). The original
 * copy-constructs them from temporaries through the vector type's out-of-line copy constructor,
 * fn_800091EC; these wrappers' constructors write those copies out so that the startup code stays
 * the same (as in Cage.cpp; a vector member would keep them from being inlined). */
static struct CandleStickFlameOffset {
    CandleStickFlameOffset() {
        float value[3];

        value[0] = 0.0f;
        value[1] = 15.0f;
        value[2] = 0.0f;
        fn_800091EC((Vec3f *)m_value, (Vec3f *)value);
    }

    const Vec3f *Get() const {
        return (const Vec3f *)m_value;
    }

    float m_value[3];
} s_flameOffset;

static struct CandleStickFlameDirection {
    CandleStickFlameDirection() {
        float value[3];

        value[0] = 0.0f;
        value[1] = 1.0f;
        value[2] = 0.0f;
        fn_800091EC((Vec3f *)m_value, (Vec3f *)value);
    }

    const Vec3f *Get() const {
        return (const Vec3f *)m_value;
    }

    float m_value[3];
} s_flameDirection;

/* The switch manager (name inferred), a function-local static object whose address the accessor
 * below caches (the cache is defined at the end of the file: retail places it after the unit's
 * other .sbss objects). */
inline UnkCandleSwitchManager_801660A0 &CandleSwitchManagerObject() {
    static UnkCandleSwitchManager_801660A0 s_object;
    return s_object;
}

inline UnkCandleSwitchManager_801660A0 *GetCandleSwitchManager() {
    if (UnkCandleSwitchManager_801660A0::s_cached == 0) {
        UnkCandleSwitchManager_801660A0::s_cached = &CandleSwitchManagerObject();
    }
    return UnkCandleSwitchManager_801660A0::s_cached;
}

CandleStick::CandleStick(Task *parent, UnkPlacement_800C9CD0 *owner) : Gadget(parent, owner) {
    m_name = s_taskName;
    m_flags.Clear();
    if (owner->m_adapterState == 0) {
        fn_800C9930(owner, new CandleStickSetState);
    }
    m_state = dynamic_cast<CandleStickSetState *>(owner->m_adapterState);
    m_chara = fn_8015CD90(0x4C, 4);
    LoadFromOwner(owner);
    m_model = fn_8015F800(1, "CANDLESTICK.BIN");
    SetupModel();
    RebuildTransform();
}

CandleStick::~CandleStick() {
    if (!m_flame.expired()) {
        boost::shared_ptr<Effect::System::EffectElement> flame = m_flame.lock();
        if (flame.get() != 0) {
            flame->Retire();
        }
        m_flame.reset();
    }
    m_sound.fn_801D455C();
}

extern "C" void fn_8012D678(void) {
    fn_8006B474("CANDLESTICK.BIN", 1);
    GetCandleSwitchManager()->Enable();
}

extern "C" void fn_8012D654(void) {
    GetCandleSwitchManager()->Disable();
}

void CandleStick::Instantiate(UnkPlacement_800C9CD0 *owner) {
    new CandleStick(fn_801EB304(fn_8001C4F8(), 11), owner);
}

void CandleStick::Behave(float dt) {
    if (m_flags.Test(0)) {
        m_model->fn_80162804();
        m_model->fn_80162A68(1, 0);
        RebuildTransform();
        UnkDebrisParam_8015F0B8 debris(m_chara, 0);
        debris.model = fn_80012700(fn_800122D4(), 1, s_debrisFile);
        debris.field_0x40 = m_chara->m_position.y;
        debris.scale = 1.0f;
        debris.field_0x4A = 0;
        debris.field_0x54 = 1;
        fn_8015D93C(&debris);
        m_state->m_flags.Set(0);
        m_flags.Reset(0);
    }
    if (m_flags.Test(1)) {
        fn_80161FC8(m_model.get(), 1, m_chara);
        m_flame = fn_80168C48(
            &(const Vec3f &)fn_8040CDD0(&m_chara->m_position, s_flameOffset.Get()),
            s_flameDirection.Get(), 0x19);
        fn_801D54F4(fn_80053164(), 0x7431, m_chara.get(), 0, 1);
        m_sound = fn_801D3E58(0x7430, m_chara.get(), this);
        GetCandleSwitchManager()->Add(m_switchId, 1);
        m_state->m_flags.Set(1);
        m_flags.Reset(1);
    }
    m_model->fn_80162FB0(dt);
}

void CandleStick::ReloadPlacement(UnkPlacement_800C9CD0 *owner) {
    CandleStickParams params = m_params;
    u8 switchId = m_switchId;

    LoadFromOwner(owner);
    if (m_params.count != params.count || m_switchId != switchId) {
        CandleStickSetState *state = m_state;
        state->m_health = 2.0f;
        state->m_flags.Clear();
        m_model->fn_80162804();
        m_model->fn_80161880();
        m_sound.fn_801D455C();
        if (!m_flame.expired()) {
            boost::shared_ptr<Effect::System::EffectElement> flame = m_flame.lock();
            if (flame.get() != 0) {
                flame->Retire();
            }
            m_flame.reset();
        }
        SetupModel();
    }
    RebuildTransform();
    m_model->fn_80162EF4();
    m_model->fn_8016108C();
}

void CandleStick::LoadFromOwner(UnkPlacement_800C9CD0 *owner) {
    fn_800C9DBC(owner, &m_params, sizeof(m_params));
    fn_800C9E60(owner, &m_switchId);
    fn_800C9F14(owner, &m_chara->m_position);
    fn_800C9ED4(owner, &m_chara->m_rotation);
}

void CandleStick::SetupModel() {
    if (m_state->m_flags.Test(0)) {
        m_model->fn_80162A68(1, 0);
    } else {
        m_model->fn_80162A68(0, 0);
    }
    if (m_state->m_flags.Test(1)) {
        fn_80161FC8(m_model.get(), 1, m_chara);
        m_flame = fn_80168C48(
            &(const Vec3f &)fn_8040CDD0(&m_chara->m_position, s_flameOffset.Get()),
            s_flameDirection.Get(), 0x19);
        m_sound = fn_801D3DC8(0x7430, m_chara.get(), this);
    }
    fn_80161FC8(m_model.get(), 0, m_chara);
    {
        boost::shared_ptr<CharaColliAttack> collision = fn_80161428(m_model.get(), 0).lock();
        if (collision.get() != 0) {
            fn_80096B64(collision.get(), 1, boost::bind(&CandleStick::OnAttack, this, _1));
        }
    }
    if (!GetCandleSwitchManager()->IsRegistered(m_switchId)) {
        GetCandleSwitchManager()->Register(m_switchId, m_params.count, 0);
    }
}

void CandleStick::RebuildTransform() {
    Mtx34f matrix;

    fn_8040C148(&matrix, &lbl_805E0720);
    fn_8040B59C(&matrix, &m_chara->m_position);
    fn_8040B848(&matrix, m_chara->m_rotation.y);
    m_model->fn_801633DC(&matrix);
    if (!m_flame.expired()) {
        boost::shared_ptr<Effect::System::EffectElement> flame = m_flame.lock();
        if (flame.get() != 0) {
            const Vec3f &position = fn_8040CDD0(&m_chara->m_position, s_flameOffset.Get());
            fn_80425E4C(flame.get(), &position);
        }
    }
}

void CandleStick::OnAttack(AttackCallbackParam &param) {
    param.m_result = 1;
    if (!m_state->m_flags.Test(1)) {
        if (fn_8015D4E4(param)) {
            boost::shared_ptr<CharaColliAttack> attacker = param.m_attacker.lock();
            if (attacker.get() != 0 && (attacker->m_flags & 0x400)) {
                m_flags.Set(1);
            }
        }
    }
    if (!m_state->m_flags.Test(0)) {
        m_state->m_health -= param.m_damage;
        if (fn_8000B044(m_state->m_health) || m_state->m_health < 0.0f) {
            param.m_result = 3;
            if (!m_flags.Test(0)) {
                m_flags.Set(0);
                boost::shared_ptr<CharInfo> attacker = fn_8015D570(param);
                if (attacker.get() != 0) {
                    fn_8020642C(attacker, 0x23);
                }
            }
        }
    }
}

static GadgetParamDesc lbl_80528460[] = {
    {1, "SAME_COMID_SWITCH_NUM", 1, 1, 0x100, 0.0f, 0.0f, 0.0f, fn_800CBC20, 0},
    {0},
};

GadgetClassDesc lbl_805284B0 = {
    "CandleStick", fn_8012D678, fn_8012D654, CandleStick::Instantiate, 0, 0, 0xBBB, 1, 0, 10, lbl_80528460,
};

/* The accessor's cache (see above). */
UnkCandleSwitchManager_801660A0 *UnkCandleSwitchManager_801660A0::s_cached;
