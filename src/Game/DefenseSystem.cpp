/* DefenseSystem (class name from RTTI): the DEFENSE_SYSTEM gadget, a shield generator
 * (DEFENSESYSTEM.BIN) with three looks: whole (state 1), damaged (2) and destroyed (3). It keeps a
 * Target at its centre and four more on a ring that turns at 12 degrees a second, and a positioned
 * sound (see CityBombBig.cpp). Its health and state live in a DefenseSystemSetState kept by the
 * owner record, so they survive the gadget being recreated. Attacks wear the health down: below 10
 * it breaks into its damaged look, and once the health is gone the attacker is rewarded, the
 * generator breaks into its debris, sets its switch, hits a player and finishes. The debris models
 * depend on the stage. File name inferred from the class. Method names marked "(name inferred)" are
 * not original. Definitions are in reverse address order (deferred inlining emits functions
 * last-defined first). */
#include "Game/CharInfo.h"
#include "Game/CharaColliAttack.h"
#include "Game/Gadget.h"
#include "Game/GadgetCharInfo.h"
#include "Game/GadgetModelInfo.h"
#include "Game/GadgetParticle.h"
#include "Game/GadgetSound.h"
#include "Game/GadgetClassDesc.h"
#include "Game/GadgetUtil.h"
#include "Game/SetAdapterState.h"
#include "Game/TNumEdit.h"
/* This unit's .rodata is kept whole (the type record's hook addresses it through one base), and
 * retail has two unreferenced constant blocks of the original headers in it: 0x1C zero bytes after
 * the TNumEdit names and a zero vector after the CharaColliBase names (their meaning and types are
 * unknown; the bytes are reproduced, as in StretchGrass.cpp). */
static const float lbl_804F0EE4[7] = {0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f};
/* After TNumEdit.h: the order of the casts sets the order of the RTTI names in .rodata. */
#include "Game/CharaColliBaseCharInfo.h"
static const float lbl_804F0F70[3] = {0.0f, 0.0f, 0.0f};
#include "Game/UnkSwitchBank_800CB69C.h"
#include "Game/UnkTargetList_80224850.h"
#include "Game/FileQueue.h"
#include "boost/bind.hpp"
#include "boost/function.hpp"
#include "boost/shared_ptr.hpp"
#include "boost/weak_ptr.hpp"
#include "size_assert.h"

using SonicteamUSA::System::CharaColli::AttackCallback;
using SonicteamUSA::System::CharaColli::AttackCallbackParam;
using SonicteamUSA::System::CharaColli::CharaColliAttack;

/* Target (class name from RTTI): a point other objects can aim at (see Spring.cpp). */
class Target {
  public:
    /* Moves the target to position (name inferred). */
    void fn_800CCE38(const Vec3f *position);
};

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

/* The parameters of a debris spawn (layout as in ArkCannon.cpp). */
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

/* The game state block in .bss (only the member used here). */
struct UnkGameState_8057D6A0 {
    /* 0x000 */ u8 field_0x0[0xAC];
    /* 0x0AC */ s32 m_stage; /* (meaning inferred) */
    /* 0x0B0 */ u8 field_0xB0[0x150 - 0xB0];
};
SIZE_ASSERT(UnkGameState_8057D6A0, 0x150);

extern "C" {
extern UnkGameState_8057D6A0 lbl_8057D6A0;

/* GadgetModelInfo helpers (see BreakObj.cpp and ArkCannon.cpp; fn_8016082C unknown). */
boost::shared_ptr<GadgetModelInfo> fn_8015F800(int arg0, const char *fileName);
bool fn_80161FC8(GadgetModelInfo *model, int variant, const boost::weak_ptr<CharInfo> &info);
boost::weak_ptr<CharaColliAttack> fn_80161428(GadgetModelInfo *model, int variant);
void fn_8016082C(GadgetModelInfo *model, int arg1);
/* Sets callback as the attack callback of an attack collision (see BreakRoad.cpp). */
void fn_80096B64(CharaColliAttack *attack, int arg1, const AttackCallback &callback);

/* Creates a Target for owner (behaviour inferred; the flags are unknown). */
boost::shared_ptr<Target> fn_801E7128(const boost::weak_ptr<CharInfo> &owner, int flags);
/* The attacker of an attack (behaviour inferred). */
boost::shared_ptr<CharInfo> fn_8015D570(AttackCallbackParam &param);
/* Gives the attacker the reward for breaking the object (behaviour inferred). */
void fn_8020642C(const boost::shared_ptr<CharInfo> &attacker, int reward);
/* Whether value is zero, and whether a equals b, within a small tolerance (from their bodies). */
bool fn_8000A950(float value);
bool fn_801C455C(float a, float b);

/* A 32-bit flag word: fn_8000AAEC constructs one from a value, fn_80014E1C sets bit, fn_8006127C
 * clears bit and fn_80014E34 tests bit (see FireRing.cpp and Elevator.cpp). */
u32 *fn_8000AAEC(u32 *flags, u32 value);
void fn_80014E1C(u32 *flags, int bit);
void fn_8006127C(u32 *flags, int bit);
bool fn_80014E34(u32 *flags, int bit);

/* Particle controller helpers (see TurnOver.cpp and CityBombBig.cpp). */
boost::shared_ptr<GadgetParticle::GadgetParticleCtrl> fn_80166AF4(void);
void fn_80166F88(GadgetParticle::GadgetParticleCtrl *ctrl);
void fn_80167180(GadgetParticle::GadgetParticleCtrl *ctrl, int id);
void fn_8016761C(GadgetParticle::GadgetParticleCtrl *ctrl, float dt);
void fn_8015FA78(GadgetModelInfo *model, void *(*callback)(void *part, void *data), void *data);
void *fn_80169D00(void *part, void *data);

/* The positioned object (see ElecBomb.cpp). */
boost::shared_ptr<UnkSound_802B4254> fn_802B4254(int kind, int arg1);
void fn_802B4498(UnkSound_802B4254 *sound, float range);
void fn_802B4510(UnkSound_802B4254 *sound, u8 flag);
void fn_802B44C8(UnkSound_802B4254 *sound, const Vec3f *position);

/* Sounds (behaviour inferred): playing sound id at a character info through the sound manager
 * (fn_80053164, GadgetUtil.h), and starting a sound kept by owner. */
void fn_801D54F4(void *manager, int id, GadgetCharInfo *source, int arg3, int arg4);
UnkSoundHandle_801D3DC8 fn_801D3E58(u16 id, const Vec3f *position, Task *owner);

/* Debris (see GadgetDebris.h) and the model loader (see BreakObj.cpp). */
void fn_8015D93C(UnkDebrisParam_8015F0B8 *param);
void *fn_800122D4(void);
s32 fn_80012700(void *manager, int arg1, const char *fileName);
/* Tells the owner that the gadget is gone (behaviour inferred). */
void fn_800C9B04(UnkPlacement_800C9CD0 *owner);

/* Sets switch id (behaviour and meaning of the last argument inferred). */
void fn_8016907C(UnkSwitchBank_800CB69C *manager, int id, int arg2);

/* The player manager and its entry (see ElecBomb.cpp). */
UnkPlayerManager_8006D6A8 *fn_8006D6A8(void);
boost::weak_ptr<UnkPlayer_800740E4> fn_800740E4(UnkPlayerManager_8006D6A8 *manager, int index);

/* destination = a + b (behaviour inferred). */
void fn_8040CDD0(Vec3f *destination, const Vec3f *a, const Vec3f *b);
/* vector turned by angle about the y axis, and matrix * point (see ArkCannon.cpp). */
Vec3f fn_8040D044(const Vec3f *vector, float angle);
Vec3f fn_8040B67C(const Mtx34f *matrix, const Vec3f *point);
}

/* fn_8040CDD0 called as returning a + b by value (as in CityBombSmall.cpp). */
typedef Vec3f (*VectorAddFn)(const Vec3f *a, const Vec3f *b);

/* degrees in radians (name inferred). */
inline float DegToRad(float degrees) {
    return degrees * (lbl_805FAD38 / 180.0f);
}

/* A length (name inferred). As a call it is not a constant expression, so the constants below are
 * stored by a static initializer, as in retail. */
inline float Span(float length) {
    return length;
}

/* The height of the ring of targets, and the height and range of the positioned object (names
 * inferred). */
static const float s_targetHeight = Span(15.0f);
static const float s_soundHeight = Span(60.0f);
static const float s_soundRange = Span(800.0f);

/* The number of targets on the ring (name inferred). */
static const int kRingTargetCount = 4;

/* The debris models of a stage (struct and member names inferred). */
struct DefenseSystemDebris {
    s32 player;          /* the player the destroyed generator hits */
    const char *damaged; /* the debris of the damaged look */
    const char *broken;  /* the debris of the destruction */
};

static const DefenseSystemDebris s_debris10500 = {
    0, "A10500SHIELDCONT_BRK.DFF", "A10500SHIELDCONTAFT_BRK.DFF"};
static const DefenseSystemDebris s_debris10503 = {
    0, "A10503SHIELDCONT_BRK.DFF", "A10503SHIELDCONTAFT_BRK.DFF"};

/* The debris of the current stage (name inferred). */
static const DefenseSystemDebris *s_debris = &s_debris10500;

/* The flag word at 0x68 (struct name unknown; as in DefenseShield.cpp). */
struct DefenseSystemFlags {
    DefenseSystemFlags(u32 value) {
        fn_8000AAEC(&m_bits, value);
    }
    void Set(int bit) {
        fn_80014E1C(&m_bits, bit);
    }
    void Clear(int bit) {
        fn_8006127C(&m_bits, bit);
    }
    bool Test(int bit) {
        return fn_80014E34(&m_bits, bit);
    }

    u32 m_bits; /* bit 0 damaged, bit 1 destroyed (meanings inferred) */
};

/* DefenseSystemSetState (class name from RTTI): the generator's state that the owner record keeps
 * (member names inferred). */
class DefenseSystemSetState : public SetAdapterState {
  public:
    DefenseSystemSetState();

    /* Makes the generator whole again (name inferred). */
    void Reset() {
        m_state = 1;
        m_health = 50.0f;
    }

    /* 0x4 */ s32 m_state;
    /* 0x8 */ float m_health;
};
SIZE_ASSERT(DefenseSystemSetState, 0xC);

class DefenseSystem : public Gadget {
  public:
    DefenseSystem(Task *parent, UnkPlacement_800C9CD0 *owner);
    virtual ~DefenseSystem();
    virtual void Behave(float dt);
    virtual void ReloadPlacement(UnkPlacement_800C9CD0 *owner);

    /* Creates the gadget under task layer 11 (name inferred). */
    static void Instantiate(UnkPlacement_800C9CD0 *owner);

    /* Copies the owner's placement and switch id (name inferred). */
    void ReadPlacement(UnkPlacement_800C9CD0 *owner);
    /* Moves the model and the targets to the character info's place (name inferred). */
    void RebuildTransform();
    /* Leaves state from and enters state to: 1 whole, 2 damaged, 3 destroyed (name inferred). */
    void SetState(int from, int to);
    /* The attack callback of the model's collision (name inferred). */
    void OnAttack(AttackCallbackParam &param);

    static const char *s_taskName;

    /* Member names are inferred. */
    /* 0x30 */ boost::shared_ptr<GadgetCharInfo> m_chara;
    /* 0x38 */ boost::shared_ptr<GadgetModelInfo> m_model;
    /* 0x40 */ UnkTargetList_80224850 m_ringTargets;
    /* 0x4C */ boost::shared_ptr<Target> m_target;
    /* 0x54 */ boost::shared_ptr<UnkSound_802B4254> m_sound;
    /* 0x5C */ boost::shared_ptr<GadgetParticle::GadgetParticleCtrl> m_particle;
    /* 0x64 */ DefenseSystemSetState *m_setState;
    /* 0x68 */ DefenseSystemFlags m_stateFlags;
    /* 0x6C */ u8 m_switchId;
    /* 0x70 */ float m_ringAngle;
    /* 0x74 */ UnkSoundHandle_801D3DC8 m_humSound;
};
SIZE_ASSERT(DefenseSystem, 0x90);

const char *DefenseSystem::s_taskName = "DefenseSystem";

DefenseSystemSetState::DefenseSystemSetState() : m_state(1), m_health(50.0f) {}


DefenseSystem::DefenseSystem(Task *parent, UnkPlacement_800C9CD0 *owner)
    : Gadget(parent, owner), m_setState(0), m_stateFlags(0), m_switchId(0), m_ringAngle(0.0f) {
    m_name = s_taskName;
    m_chara = fn_8015CD90(0x5F, 4);
    ReadPlacement(owner);
    m_target = fn_801E7128(m_chara, 0xA400);
    m_ringTargets.reserve(kRingTargetCount);
    for (u32 i = 0; i < kRingTargetCount; i++) {
        m_ringTargets.push_back(fn_801E7128(m_chara, 0x8800));
    }
    m_sound = fn_802B4254(0, 0);
    fn_802B4498(m_sound.px, s_soundRange);
    fn_802B4510(m_sound.px, 1);
    {
        const Vec3f offset(0.0f, s_soundHeight, 0.0f);
        const Vec3f &position = ((VectorAddFn)fn_8040CDD0)(&offset, &m_chara->m_position);

        fn_802B44C8(m_sound.px, &position);
    }
    m_model = fn_8015F800(1, "DEFENSESYSTEM.BIN");
    if (owner->m_adapterState == 0) {
        fn_800C9930(owner, new DefenseSystemSetState);
    }
    m_setState = dynamic_cast<DefenseSystemSetState *>(owner->m_adapterState);
    m_humSound = fn_801D3E58(0x7730, &m_chara->m_position, this);
    SetState(0, m_setState->m_state);
}

DefenseSystem::~DefenseSystem() {
    m_humSound.fn_801D455C();
}

extern "C" void fn_8022440C(void) {
    s_debris = lbl_8057D6A0.m_stage == 0x11 ? &s_debris10500 : &s_debris10503;
    fn_8006B474("DEFENSESYSTEM.BIN", 1);
}

extern "C" void fn_80224408(void) {}

void DefenseSystem::Instantiate(UnkPlacement_800C9CD0 *owner) {
    new DefenseSystem(fn_801EB304(fn_8001C4F8(), 11), owner);
}

void DefenseSystem::Behave(float dt) {
    if (m_stateFlags.Test(1)) {
        {
            UnkDebrisParam_8015F0B8 debris(m_chara, 0);

            debris.model = fn_80012700(fn_800122D4(), 1, s_debris->broken);
            debris.scale = 1.0f;
            debris.field_0x4A = 0;
            debris.field_0x54 = 1;
            fn_8015D93C(&debris);
        }
        fn_8016907C(fn_800CB69C(), m_switchId, 1);
        {
            boost::shared_ptr<UnkPlayer_800740E4> player =
                fn_800740E4(fn_8006D6A8(), s_debris->player).lock();
            if (player.get() != 0) {
                player->Hit(1, 0);
            }
        }
        fn_802B4510(m_sound.px, 0);
        fn_801D54F4(fn_80053164(), 0x7732, m_chara.get(), 0, 1);
        fn_800C9B04(m_owner);
        SetState(m_setState->m_state, 3);
        m_stateFlags.Clear(1);
        m_stateFlags.Clear(0);
        m_flags |= 1;
        return;
    }
    if (m_stateFlags.Test(0)) {
        {
            UnkDebrisParam_8015F0B8 debris(m_chara, 0);

            debris.model = fn_80012700(fn_800122D4(), 1, s_debris->damaged);
            debris.scale = 1.0f;
            debris.field_0x4A = 0;
            debris.field_0x54 = 1;
            fn_8015D93C(&debris);
        }
        fn_801D54F4(fn_80053164(), 0x7732, m_chara.get(), 0, 1);
        SetState(m_setState->m_state, 2);
        m_stateFlags.Clear(0);
    }
    m_ringAngle += dt * DegToRad(12.0f);
    RebuildTransform();
    m_model->fn_80162FB0(dt);
    if (m_particle.get() != 0) {
        fn_8016761C(m_particle.get(), dt);
    }
}

void DefenseSystem::ReloadPlacement(UnkPlacement_800C9CD0 *owner) {
    ReadPlacement(owner);
    if (m_setState->m_state == 3) {
        m_setState->Reset();
        SetState(3, m_setState->m_state);
    }
    RebuildTransform();
    m_model->fn_80162EF4();
    m_model->fn_8016108C();
}

void DefenseSystem::ReadPlacement(UnkPlacement_800C9CD0 *owner) {
    fn_800C9F14(owner, &m_chara->m_position);
    fn_800C9ED4(owner, &m_chara->m_rotation);
    fn_800C9E60(owner, &m_switchId);
}

void DefenseSystem::RebuildTransform() {
    Mtx34f matrix;

    fn_8040C148(&matrix, &lbl_805E0720);
    fn_8040B59C(&matrix, &m_chara->m_position);
    fn_8040B848(&matrix, m_chara->m_rotation.y);
    fn_8040B884(&matrix, m_chara->m_rotation.x);
    fn_8040B80C(&matrix, m_chara->m_rotation.z);
    fn_8040B80C(&matrix, m_ringAngle);
    m_model->fn_801633DC(&matrix);
    m_target->fn_800CCE38(&m_chara->m_position);

    boost::shared_ptr<Target> *target = m_ringTargets.begin();
    u32 i = 0;
    float step = DegToRad(360.0f) / kRingTargetCount;
    for (; target != m_ringTargets.end(); i++, target++) {
        Vec3f position(0.0f, s_targetHeight, 0.0f);

        AssignVector(position, fn_8040D044(&position, step * i));
        AssignVector(position, fn_8040B67C(&matrix, &position));
        (*target)->fn_800CCE38(&position);
    }
}

void DefenseSystem::SetState(int from, int to) {
    switch (from) {
    case 1:
        if (m_particle.get() != 0) {
            fn_80166F88(m_particle.get());
            m_particle.reset();
        }
        m_model->fn_801604FC();
        m_model->fn_80162804();
        m_model->fn_80161880();
        break;
    case 2:
        if (m_particle.get() != 0) {
            fn_80166F88(m_particle.get());
            m_particle.reset();
        }
        m_model->fn_80162804();
        m_model->fn_80161880();
        break;
    }

    switch (to) {
    case 1: {
        m_model->fn_80162A68(0, 0);
        m_model->fn_80160C28(0);
        fn_8016082C(m_model.get(), 0);
        m_model->fn_8015FF60();
        m_model->fn_80160B18(0);
        RebuildTransform();
        m_particle = fn_80166AF4();
        {
            UnkParticleAttach_80169D00 attach;
            attach.ctrl = m_particle;
            fn_8015FA78(m_model.get(), fn_80169D00, &attach);
            fn_80167180(m_particle.get(), -1);
        }
        fn_80161FC8(m_model.get(), 0, m_chara);
        boost::shared_ptr<CharaColliAttack> collision = fn_80161428(m_model.get(), 0).lock();
        if (collision.get() != 0) {
            fn_80096B64(collision.get(), 1, boost::bind(&DefenseSystem::OnAttack, this, _1));
        }
        break;
    }
    case 2: {
        m_model->fn_80162A68(1, 0);
        RebuildTransform();
        m_particle = fn_80166AF4();
        {
            UnkParticleAttach_80169D00 attach;
            attach.ctrl = m_particle;
            fn_8015FA78(m_model.get(), fn_80169D00, &attach);
            fn_80167180(m_particle.get(), -1);
        }
        fn_80161FC8(m_model.get(), 1, m_chara);
        boost::shared_ptr<CharaColliAttack> collision = fn_80161428(m_model.get(), 1).lock();
        if (collision.get() != 0) {
            fn_80096B64(collision.get(), 1, boost::bind(&DefenseSystem::OnAttack, this, _1));
        }
        break;
    }
    }
    m_setState->m_state = to;
}

void DefenseSystem::OnAttack(AttackCallbackParam &param) {
    param.m_result = 1;
    if (m_setState->m_state != 3) {
        m_setState->m_health -= param.m_damage;
        if (m_setState->m_health < 0.0f || fn_8000A950(m_setState->m_health)) {
            param.m_result = 3;
            boost::shared_ptr<CharInfo> attacker = fn_8015D570(param);
            if (attacker.get() != 0) {
                fn_8020642C(attacker, 0x47);
            }
            m_stateFlags.Set(1);
        } else if (m_setState->m_state == 1 && !m_stateFlags.Test(1)) {
            if (m_setState->m_health < 10.0f || fn_801C455C(m_setState->m_health, 10.0f)) {
                m_stateFlags.Set(0);
            }
        }
        if (param.m_damage >= 1.0f) {
            fn_801D54F4(fn_80053164(), 0x7731, m_chara.get(), 0, 1);
        }
    }
}

GadgetClassDesc lbl_80546334 = {
    "DEFENSE_SYSTEM", fn_8022440C, fn_80224408, DefenseSystem::Instantiate, 0, 0, 0x1388, 1, 0, 10, 0,
};
