/* BASwitch::BASwitch (class name from RTTI; its s_taskName is "BASwitch"): the BA Switch gadget, a
 * switch (BASWITCH.BIN) that an attack or a vacuum command (type 0x14) flips between off (look 1,
 * the "blue" model) and on (look 2, the "red" model), at most once every 2 seconds. It sets the
 * shared switch state of its owner's switch id, hits the player object of the manager at
 * fn_8006D6A8 and records the state in a small table of BA switch states (up to four ids) that BA
 * Shields read (fn_80301F4C), save points save (fn_80301F2C) and the stage callbacks reset. File
 * name inferred from the class. Names marked "(name inferred)" are not original. Definitions are in
 * reverse address order (deferred inlining emits functions last-defined first). */
#include <boost/bind.hpp>
#include <boost/function.hpp>

#include "Game/CharCommand.h"
#include "Game/CharCommandUnit.h"
#include "Game/CharInfo.h"
#include "Game/CharaColliAttack.h"
#include "Game/Gadget.h"
#include "Game/GadgetCharInfo.h"
#include "Game/GadgetModelInfo.h"
#include "Game/GadgetParticle.h"
#include "Game/GadgetClassDesc.h"
#include "Game/GadgetUtil.h"
#include "Game/UnkSwitchBank_800CB69C.h"
#include "Game/FileQueue.h"
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

extern "C" {
/* A 32-bit flag word: fn_80014E04 constructs it cleared, fn_80014E10 clears it, fn_80014E1C sets
 * bit, fn_8006127C clears bit and fn_80014E34 tests bit (behaviour from their bodies). */
void fn_80014E04(u32 *flags);
void fn_80014E10(u32 *flags);
void fn_80014E1C(u32 *flags, int bit);
void fn_8006127C(u32 *flags, int bit);
bool fn_80014E34(u32 *flags, int bit);

/* Gives the character info a command handler unless it has one (behaviour inferred). */
void fn_8015CF7C(GadgetCharInfo *charInfo);
/* The switch id the owner was placed with (behaviour inferred). */
void fn_800C9E50(UnkPlacement_800C9CD0 *owner, s32 *id);

/* GadgetModelInfo helpers (behaviour inferred; see BAMine.cpp, BreakWall.cpp and Door.cpp). */
boost::shared_ptr<GadgetModelInfo> fn_8015F800(int arg0, const char *fileName);
bool fn_80161FC8(GadgetModelInfo *model, int variant, const boost::weak_ptr<CharInfo> &info);
boost::weak_ptr<CharaColliAttack> fn_80161428(GadgetModelInfo *model, int variant);
void fn_801629D8(GadgetModelInfo *model, const char *name, int arg2);
void fn_80160A84(GadgetModelInfo *model, int index, int motion);
void fn_801608AC(GadgetModelInfo *model, int index, int motion);
bool fn_80160040(GadgetModelInfo *model, int index);
void fn_8016271C(GadgetModelInfo *model);
void fn_80162790(GadgetModelInfo *model);
void fn_8015FA78(GadgetModelInfo *model, void *(*callback)(void *part, void *data), void *data);
void *fn_80169D00(void *part, void *data);

/* Particle controller helpers (behaviour inferred). */
boost::shared_ptr<GadgetParticle::GadgetParticleCtrl> fn_80166AF4(void);
void fn_80166B70(GadgetParticle::GadgetParticleCtrl *ctrl);
void fn_80166C70(GadgetParticle::GadgetParticleCtrl *ctrl);
void fn_80167010(GadgetParticle::GadgetParticleCtrl *ctrl, int id);
void fn_8016761C(GadgetParticle::GadgetParticleCtrl *ctrl, float dt);

/* The positioned object (see ElecBomb.cpp). */
boost::shared_ptr<UnkSound_802B4254> fn_802B4254(int kind, int arg1);
void fn_802B4510(UnkSound_802B4254 *sound, u8 flag);
void fn_802B44C8(UnkSound_802B4254 *sound, const Vec3f *position);

/* Creates a Target for owner, and removes a gadget's target (behaviour inferred). */
boost::shared_ptr<Target> fn_801E7128(const boost::weak_ptr<CharInfo> &owner, int flags);
void fn_801E7108(boost::shared_ptr<Target> *target);

/* The player manager and its entry (see ElecBomb.cpp). */
UnkPlayerManager_8006D6A8 *fn_8006D6A8(void);
boost::weak_ptr<UnkPlayer_800740E4> fn_800740E4(UnkPlayerManager_8006D6A8 *manager, int index);

/* Plays sound id at a character info (behaviour inferred). */
void fn_801D54F4(void *manager, int id, GadgetCharInfo *source, int arg3, int arg4);

/* The table of BA switch states (defined at the end of this unit; names of the functions are not
 * known). */
void fn_80301D04(int id);
void fn_80301D90(int id);
bool fn_80301E1C(int id);
}

/* A length (name inferred). As a call it is not a constant expression, so the constants below are
 * stored by the static initializer, as in retail (as in SecretDisk.cpp). */
inline float Span(float length) {
    return length;
}

/* How high above the switch the target and the positioned object sit (names inferred). */
static const float s_targetHeight = Span(10.0f);
static const float s_soundHeight = Span(50.0f);

namespace BASwitch {

/* The flag word at 0x84 (struct name unknown). */
struct BASwitchFlags {
    BASwitchFlags() {
        fn_80014E04(&m_bits);
    }
    void Clear() {
        fn_80014E10(&m_bits);
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

    u32 m_bits; /* bit 0: flipped recently, 1: click sound due when the motion ends */
};

class BASwitch : public Gadget {
  public:
    BASwitch(Task *parent, UnkPlacement_800C9CD0 *owner);
    virtual ~BASwitch();
    virtual void Behave(float dt);
    virtual void ReloadPlacement(UnkPlacement_800C9CD0 *owner);

    /* Creates the gadget under task layer 11 (name inferred). */
    static void Instantiate(UnkPlacement_800C9CD0 *owner);

    /* Copies the owner's placement and switch id and resets the state (name inferred). */
    void ReadPlacement(UnkPlacement_800C9CD0 *owner);
    /* Places the models, the positioned object and the target (name inferred). */
    void UpdatePosition();
    /* The attack collision's callback (name inferred). */
    void OnAttack(AttackCallbackParam &param);
    /* Flips the switch (name inferred). */
    void Toggle();
    /* Loads the three models and their particles (name inferred). */
    void LoadModels();
    /* Shows look 1 (off) or 2 (on) (name inferred). */
    void SetLook(int look);
    /* The command unit's callback (name inferred). */
    void OnCommand(CharCommand &command);

    static const char *s_fileName;
    static const char *s_taskName;

    /* Member names are inferred. */
    /* 0x30 */ boost::shared_ptr<GadgetCharInfo> m_chara;
    /* 0x38 */ boost::shared_ptr<GadgetModelInfo> m_models[3]; /* the switch, "blue", "red" */
    /* 0x50 */ boost::shared_ptr<GadgetParticle::GadgetParticleCtrl> m_particles[3];
    /* 0x68 */ boost::shared_ptr<UnkSound_802B4254> m_sound;
    /* 0x70 */ s32 m_state; /* 0 new, 1 running */
    /* 0x74 */ float m_timer;
    /* 0x78 */ s32 m_switchId;
    /* 0x7C */ boost::shared_ptr<Target> m_target;
    /* 0x84 */ BASwitchFlags m_flags2;
};
SIZE_ASSERT(BASwitch, 0x88);

const char *BASwitch::s_fileName = "BASWITCH.BIN";
const char *BASwitch::s_taskName = "BASwitch";

BASwitch::BASwitch(Task *parent, UnkPlacement_800C9CD0 *owner) : Gadget(parent, owner) {
    m_name = s_taskName;
    m_chara = fn_8015CD90(0x8B, 4);
    fn_8015CF7C(m_chara.px);
    fn_800729B0(&m_chara->m_commandUnit->m_callback,
                &(const CharCommandCallback &)CharCommandCallback(
                    boost::bind(&BASwitch::OnCommand, this, _1)));
    ReadPlacement(owner);
    LoadModels();
    m_sound = fn_802B4254(0, 0);
    fn_802B4510(m_sound.px, 1);
    m_target = fn_801E7128(m_chara, 0x80);
    if (fn_80301E1C(m_switchId)) {
        fn_801690D0(fn_800CB69C(), m_switchId, 1);
        fn_801608AC(m_models[0].px, 0, 2);
        SetLook(2);
    } else {
        fn_801690D0(fn_800CB69C(), m_switchId, 0);
        fn_801608AC(m_models[0].px, 0, 1);
        SetLook(1);
    }
    UpdatePosition();
}

BASwitch::~BASwitch() {
    fn_801E7108(&m_target);
}

void BASwitch::Behave(float dt) {
    if (m_flags2.Test(1) && fn_80160040(m_models[0].px, 0)) {
        fn_801D54F4(fn_80053164(), 0x793A, m_chara.px, 0, 1);
        m_flags2.Clear(1);
    }
    for (int i = 0; i < 3; i++) {
        if (m_models[i].px != 0) {
            m_models[i]->fn_80162FB0(dt);
        }
    }
    for (int i = 0; i < 3; i++) {
        if (m_particles[i].px != 0) {
            fn_8016761C(m_particles[i].px, dt);
        }
    }
    m_timer -= dt;
    if (m_timer <= 0.0f) {
        m_timer = 0.0f;
        m_flags2.Clear(0);
    }
    switch (m_state) {
    case 0:
        m_state = 1;
        break;
    case 1:
        break;
    case 2:
        break;
    }
    UpdatePosition();
}

void BASwitch::ReloadPlacement(UnkPlacement_800C9CD0 *owner) {
    ReadPlacement(owner);
    UpdatePosition();
}

void BASwitch::ReadPlacement(UnkPlacement_800C9CD0 *owner) {
    fn_800C9F14(owner, &m_chara->m_position);
    fn_800C9ED4(owner, &m_chara->m_rotation);
    fn_800C9E50(owner, &m_switchId);
    m_state = 0;
    m_timer = 0.0f;
    m_flags2.Clear();
}

void BASwitch::UpdatePosition() {
    Mtx34f matrix;

    fn_8040C148(&matrix, &lbl_805E0720);
    fn_8040B59C(&matrix, &m_chara->m_position);
    fn_8040B848(&matrix, m_chara->m_rotation.y);
    for (int i = 0; i < 3; i++) {
        if (m_models[i].px != 0) {
            m_models[i]->fn_801633DC(&matrix);
        }
    }
    if (m_sound.px != 0) {
        Vec3f position;
        fn_800091EC(&position, &m_chara->m_position);
        position.y += s_soundHeight;
        fn_802B44C8(m_sound.px, &position);
    }
    if (m_target.px != 0) {
        Vec3f position;
        fn_800091EC(&position, &m_chara->m_position);
        position.y += s_targetHeight;
        m_target->fn_800CCE38(&position);
    }
}

void BASwitch::OnAttack(AttackCallbackParam &param) {
    param.m_result = 5;
    if (m_timer > 0.0f) {
        return;
    }
    if (m_state == 1) {
        if (boost::shared_ptr<CharaColliAttack> attack = param.m_attacker.lock()) {
            if ((attack->m_flags & 0x3D1) == 0) {
                return;
            }
            m_timer = 2.0f;
            m_flags2.Set(0);
            param.m_result = 1;
            Toggle();
        }
    }
}

void BASwitch::Toggle() {
    if (fn_8016904C(fn_800CB69C(), m_switchId) == 1) {
        fn_801690D0(fn_800CB69C(), m_switchId, 0);
        fn_80160A84(m_models[0].px, 0, 1);
        SetLook(1);
        boost::shared_ptr<UnkPlayer_800740E4> player = fn_800740E4(fn_8006D6A8(), 0).lock();
        if (player.px != 0) {
            player->Hit(-1, 0);
        }
        fn_80301D04(m_switchId);
    } else {
        fn_801690D0(fn_800CB69C(), m_switchId, 1);
        fn_80160A84(m_models[0].px, 0, 2);
        SetLook(2);
        boost::shared_ptr<UnkPlayer_800740E4> player = fn_800740E4(fn_8006D6A8(), 0).lock();
        if (player.px != 0) {
            player->Hit(1, 0);
        }
        m_flags2.Set(1);
        fn_80301D90(m_switchId);
    }
    fn_801D54F4(fn_80053164(), 0x7939, m_chara.px, 0, 1);
}

void BASwitch::LoadModels() {
    m_models[0] = fn_8015F800(1, s_fileName);
    m_models[0]->fn_80162A68(0, 0);
    m_models[1] = fn_8015F800(1, s_fileName);
    fn_801629D8(m_models[1].px, "blue", 0);
    m_models[2] = fn_8015F800(1, s_fileName);
    fn_801629D8(m_models[2].px, "red", 0);
    m_models[0]->fn_80160C28(0);
    m_models[0]->fn_8015FF60();
    fn_80161FC8(m_models[0].px, 0, m_chara);
    boost::weak_ptr<CharaColliAttack> attack = fn_80161428(m_models[0].px, 0);
    if (boost::shared_ptr<CharaColliAttack> collision = attack.lock()) {
        collision->field_0x54 = 0;
        collision->field_0x55 = 1;
        collision->m_attackCallback = AttackCallback(boost::bind(&BASwitch::OnAttack, this, _1));
    }
    for (int i = 0; i < 3; i++) {
        if (m_models[i].px != 0) {
            m_particles[i] = fn_80166AF4();
            UnkParticleAttach_80169D00 ref;
            ref.ctrl = m_particles[i];
            fn_8015FA78(m_models[i].px, fn_80169D00, &ref);
        }
        if (m_particles[i].px != 0) {
            fn_80167010(m_particles[i].px, -1);
        }
    }
}

void BASwitch::SetLook(int look) {
    switch (look) {
    case 1:
        if (m_models[1].px != 0) {
            fn_8016271C(m_models[1].px);
        }
        if (m_models[2].px != 0) {
            fn_80162790(m_models[2].px);
        }
        if (m_particles[1].px != 0) {
            fn_80166B70(m_particles[1].px);
        }
        if (m_particles[2].px != 0) {
            fn_80166C70(m_particles[2].px);
        }
        break;
    case 2:
        if (m_models[2].px != 0) {
            fn_8016271C(m_models[2].px);
        }
        if (m_models[1].px != 0) {
            fn_80162790(m_models[1].px);
        }
        if (m_particles[2].px != 0) {
            fn_80166B70(m_particles[2].px);
        }
        if (m_particles[1].px != 0) {
            fn_80166C70(m_particles[1].px);
        }
        break;
    }
}

void BASwitch::OnCommand(CharCommand &command) {
    switch (command.TypeId()) {
    case 0x14:
        if (!m_flags2.Test(0)) {
            Toggle();
            m_timer = 2.0f;
            m_flags2.Set(0);
        }
        break;
    }
    command.field_0x4[3] |= 2;
}

} // namespace BASwitch

extern "C" void fn_80301FC8(void) {
    fn_8006B474(BASwitch::BASwitch::s_fileName, 1);
}

extern "C" void fn_80301FC4(void) {}

namespace BASwitch {

void BASwitch::Instantiate(UnkPlacement_800C9CD0 *owner) {
    Task *parent = fn_801EB304(fn_8001C4F8(), 11);
    new BASwitch(parent, owner);
}

/* One entry of the BA switch state table: a switch id (-1 when free) and whether it is on, with
 * copies a save point keeps (struct and member names inferred). */
struct SwitchState {
    SwitchState();

    /* 0x0 */ s32 m_id;
    /* 0x4 */ s32 m_savedId;
    /* 0x8 */ bool m_on;
    /* 0x9 */ bool m_savedOn;
};
SIZE_ASSERT(SwitchState, 0xC);

} // namespace BASwitch

/* The BA switch states (name inferred). */
static BASwitch::SwitchState s_switchStates[4];

extern "C" {
void fn_80301CD0(void);
void fn_80301ED4(void);
void fn_80301E60(void);
void fn_80301E94(void);
void fn_80301EB4(void);

/* Whether switch id is on (name of the function unknown). */
bool fn_80301F4C(int id) {
    return fn_80301E1C(id);
}

/* Saves the states (at a save point). */
void fn_80301F2C(void) {
    fn_80301CD0();
}
}

BASwitch::SwitchState::SwitchState() {
    m_id = m_savedId = -1;
    m_on = m_savedOn = false;
}

extern "C" {
/* Clears the table. */
void fn_80301ED4(void) {
    for (int i = 0; i < 4; i++) {
        s_switchStates[i].m_id = s_switchStates[i].m_savedId = -1;
        s_switchStates[i].m_on = s_switchStates[i].m_savedOn = false;
    }
}

void fn_80301EB4(void) {
    fn_80301ED4();
}

void fn_80301E94(void) {
    fn_80301ED4();
}

/* Restores the saved states. */
void fn_80301E60(void) {
    for (int i = 0; i < 4; i++) {
        s_switchStates[i].m_id = s_switchStates[i].m_savedId;
        s_switchStates[i].m_on = s_switchStates[i].m_savedOn;
    }
}

bool fn_80301E1C(int id) {
    for (int i = 0; i < 4; i++) {
        s32 entry = s_switchStates[i].m_id;
        if (entry == id) {
            return s_switchStates[i].m_on;
        }
    }
    return false;
}

/* Records switch id as on, and as off. */
void fn_80301D90(int id) {
    for (int i = 0; i < 4; i++) {
        s32 entry = s_switchStates[i].m_id;
        if (entry == id) {
            s_switchStates[i].m_id = id;
            s_switchStates[i].m_on = true;
            return;
        }
    }
    for (int i = 0; i < 4; i++) {
        if (s_switchStates[i].m_id == -1) {
            s_switchStates[i].m_id = id;
            s_switchStates[i].m_on = true;
            return;
        }
    }
}

void fn_80301D04(int id) {
    for (int i = 0; i < 4; i++) {
        s32 entry = s_switchStates[i].m_id;
        if (entry == id) {
            s_switchStates[i].m_id = id;
            s_switchStates[i].m_on = false;
            return;
        }
    }
    for (int i = 0; i < 4; i++) {
        if (s_switchStates[i].m_id == -1) {
            s_switchStates[i].m_id = id;
            s_switchStates[i].m_on = false;
            return;
        }
    }
}

/* Saves the states. */
void fn_80301CD0(void) {
    for (int i = 0; i < 4; i++) {
        s_switchStates[i].m_savedId = s_switchStates[i].m_id;
        s_switchStates[i].m_savedOn = s_switchStates[i].m_on;
    }
}

/* Stage callbacks (in a stage table in .rodata): each clears or restores the table. */
int fn_80301CAC(void) {
    fn_80301ED4();
    return 1;
}

int fn_80301C88(void) {
    fn_80301E60();
    return 1;
}

int fn_80301C64(void) {
    fn_80301E94();
    return 1;
}

int fn_80301C40(void) {
    fn_80301EB4();
    return 1;
}
}

GadgetClassDesc lbl_8055AD78 = {
    "BA Switch", fn_80301FC8, fn_80301FC4, BASwitch::BASwitch::Instantiate, 0, 0, 0x1900, 1, 0, 10, 0,
};
