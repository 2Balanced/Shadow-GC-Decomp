/* CityBombBig (class name from RTTI): the CITY_BOMB_BIG gadget, a large bomb (CITYBOMBBIG.BIN)
 * with a ticking sound and particles. Attacks wear its health down; once destroyed it rewards the
 * attacker and arms itself. A command of type 0x1B also arms it and one of type 0x1A disarms it.
 * Once armed, it stops ticking, plays its alarm and explodes after a second: it breaks into its
 * debris, shakes the camera, hits the first player, sets its switch and finishes. File name
 * inferred from the class. Method names marked "(name inferred)" are not original. Definitions are
 * in reverse address order (deferred inlining emits functions last-defined first). */
#include <boost/bind.hpp>
#include <boost/function.hpp>

#include "Game/CharCommand.h"
#include "Game/CharCommandUnit.h"
#include "Game/CharInfo.h"
#include "Game/CharaColli.h"
#include "Game/CharaColliAttack.h"
#include "Game/Gadget.h"
#include "Game/GadgetCharInfo.h"
#include "Game/GadgetModelInfo.h"
#include "Game/GadgetParticle.h"
#include "Game/GadgetSound.h"
#include "Game/GadgetClassDesc.h"
#include "Game/GadgetUtil.h"
#include "Game/TNumEdit.h"
/* After TNumEdit.h: the order of the casts sets the order of the RTTI base lists in .data. */
#include "Game/CharaColliBaseCharInfo.h"
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
class UnkCameraManager_80009548;

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

/* A part of a model looked up by id (see BreakObj.cpp). */
struct UnkModelPart_8015F7BC {
    /* 0x0 */ u32 field_0x0;
    /* 0x4 */ const char *name; /* the part's model file */
};

/* The game state block in .bss (only the member used here). */
struct UnkGameState_8057D6A0 {
    /* 0x000 */ u8 field_0x0[0x125];
    /* 0x125 */ u8 field_0x125; /* (meaning unknown) */
    /* 0x126 */ u8 field_0x126[0x150 - 0x126];
};
SIZE_ASSERT(UnkGameState_8057D6A0, 0x150);

extern "C" {
extern UnkGameState_8057D6A0 lbl_8057D6A0;

/* A 32-bit flag word: fn_80014E04 constructs it, fn_80014E10 clears it, fn_80014E1C sets bit,
 * fn_8006127C clears bit and fn_80014E34 tests bit (see FootingMovable.cpp). */
void fn_80014E04(u32 *flags);
void fn_80014E10(u32 *flags);
void fn_80014E1C(u32 *flags, int bit);
bool fn_80014E34(u32 *flags, int bit);
void fn_8006127C(u32 *flags, int bit);

/* Copies the owner's switch id into value (behaviour inferred). */
void fn_800C9E50(UnkPlacement_800C9CD0 *owner, s32 *value);
/* Tells the owner that the gadget is gone (behaviour inferred). */
void fn_800C9B04(UnkPlacement_800C9CD0 *owner);
/* Gives the character info a command handler unless it has one (behaviour inferred). */
void fn_8015CF7C(GadgetCharInfo *charInfo);

/* GadgetModelInfo helpers (see BreakObj.cpp and ArkCannon.cpp). */
boost::shared_ptr<GadgetModelInfo> fn_8015F800(int arg0, const char *fileName);
bool fn_80161FC8(GadgetModelInfo *model, int variant, const boost::weak_ptr<CharInfo> &info);
boost::weak_ptr<CharaColliAttack> fn_80161428(GadgetModelInfo *model, int variant);
void fn_80162790(GadgetModelInfo *model);
UnkModelPart_8015F7BC *fn_8015F7BC(GadgetModelInfo *model, int id);
void fn_8015FA78(GadgetModelInfo *model,
                 void *(*callback)(void *part, UnkParticleAttach_80169D00 *param),
                 UnkParticleAttach_80169D00 *param);
void *fn_80169D00(void *part, UnkParticleAttach_80169D00 *param);
/* Sets callback as the attack callback of an attack collision (see BreakRoad.cpp). */
void fn_80096B64(CharaColliAttack *attack, int arg1, const AttackCallback &callback);

/* Particle controller helpers (behaviour inferred). */
boost::shared_ptr<GadgetParticle::GadgetParticleCtrl> fn_80166AF4(void);
void fn_80166E84(GadgetParticle::GadgetParticleCtrl *ctrl, int id);
void fn_80166F88(GadgetParticle::GadgetParticleCtrl *ctrl);
void fn_80167180(GadgetParticle::GadgetParticleCtrl *ctrl, int id);
void fn_8016761C(GadgetParticle::GadgetParticleCtrl *ctrl, float dt);

/* Creates a Target for owner (behaviour inferred; the flags are unknown). */
boost::shared_ptr<Target> fn_801E7128(const boost::weak_ptr<CharInfo> &owner, int flags);
/* The attacker of an attack (behaviour inferred). */
boost::shared_ptr<CharInfo> fn_8015D570(AttackCallbackParam &param);
/* Sends message to the character (behaviour and meaning of message inferred). */
int fn_8020642C(const boost::shared_ptr<CharInfo> &character, int message);
/* A test on a timer or the health (behaviour unknown; see FootingMovable.cpp). */
bool fn_8000A950(float value);

/* The positioned object (see ElecBomb.cpp). */
boost::shared_ptr<UnkSound_802B4254> fn_802B4254(int kind, int arg1);
void fn_802B4498(UnkSound_802B4254 *sound, float range);
void fn_802B4510(UnkSound_802B4254 *sound, u8 flag);
void fn_802B44C8(UnkSound_802B4254 *sound, const Vec3f *position);
void fn_800CC978(boost::shared_ptr<UnkSound_802B4254> *sound);

/* Sounds (behaviour inferred): playing sound id at a character info through the sound manager
 * (fn_80053164, GadgetUtil.h), and starting a sound kept by owner. */
void fn_801D54F4(void *manager, int id, GadgetCharInfo *source, int arg3,
                 int arg4);
UnkSoundHandle_801D3DC8 fn_801D3E58(u16 id, const Vec3f *position, Task *owner);

/* Debris (see GadgetDebris.h) and the model loader (see BreakObj.cpp). */
void fn_8015D93C(UnkDebrisParam_8015F0B8 *param);
void *fn_800122D4(void);
s32 fn_80012700(void *manager, int arg1, const char *fileName);

/* An effect at a character info (behaviour and arguments inferred). */
void fn_801DD0BC(GadgetCharInfo *charInfo, int arg1, float arg2, int arg3);

/* Sets switch id (behaviour and meaning of the last argument inferred). */
void fn_8016907C(UnkSwitchBank_800CB69C *manager, int id, int arg2);

/* The player manager and its entry (see ElecBomb.cpp). */
UnkPlayerManager_8006D6A8 *fn_8006D6A8(void);
boost::weak_ptr<UnkPlayer_800740E4> fn_800740E4(UnkPlayerManager_8006D6A8 *manager, int index);
/* The camera manager and a camera shake (see ElecBomb.cpp). */
UnkCameraManager_80009548 *fn_80009548(void);
void fn_8000D664(UnkCameraManager_80009548 *manager, int arg1, float arg2, float arg3,
                 float arg4);

/* destination = a + b (behaviour inferred). */
void fn_8040CDD0(Vec3f *destination, const Vec3f *a, const Vec3f *b);
}

/* A length (name inferred). As a call it is not a constant expression, so the constants below are
 * stored by a static initializer, as in retail. */
inline float Span(float length) {
    return length;
}

/* The height of the positioned object above the bomb, and its range (names inferred). */
static float s_soundHeight = Span(34.0f);
static float s_soundRange = Span(1200.0f);

/* The flag word at 0x5C (struct name unknown). */
struct CityBombFlags {
    CityBombFlags() {
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

    u32 m_bits; /* bit 0: armed */
};

class CityBombBig : public Gadget {
  public:
    CityBombBig(Task *parent, UnkPlacement_800C9CD0 *owner);
    virtual ~CityBombBig();
    virtual void Behave(float dt);
    virtual void ReloadPlacement(UnkPlacement_800C9CD0 *owner);

    /* Creates the gadget under task layer 11 (name inferred). */
    static void Instantiate(UnkPlacement_800C9CD0 *owner);

    /* Copies the owner's placement and switch id and resets the state (name inferred). */
    void ReadPlacement(UnkPlacement_800C9CD0 *owner);
    /* Places the model, the target and the positioned object (name inferred). */
    void RebuildTransform();
    /* The command unit's callback (name inferred). */
    void OnCommand(CharCommand &command);
    /* The attack collision's callback (name inferred). */
    void OnAttack(AttackCallbackParam &param);

    static const char *s_taskName;

    /* Member names are inferred. */
    /* 0x30 */ boost::shared_ptr<GadgetCharInfo> m_chara;
    /* 0x38 */ boost::shared_ptr<GadgetModelInfo> m_model;
    /* 0x40 */ boost::shared_ptr<GadgetParticle::GadgetParticleCtrl> m_particles;
    /* 0x48 */ boost::shared_ptr<Target> m_target;
    /* 0x50 */ boost::shared_ptr<UnkSound_802B4254> m_sound;
    /* 0x58 */ s32 m_state; /* 0 disarmed, 1 waiting, 2 armed */
    /* 0x5C */ CityBombFlags m_flags2;
    /* 0x60 */ float m_health;
    /* 0x64 */ float m_timer;
    /* 0x68 */ s32 m_switchId;
    /* 0x6C */ UnkSoundHandle_801D3DC8 m_tickSound;
};
SIZE_ASSERT(CityBombBig, 0x88);

const char *CityBombBig::s_taskName = "CityBombBig";

CityBombBig::CityBombBig(Task *parent, UnkPlacement_800C9CD0 *owner) : Gadget(parent, owner) {
    m_name = s_taskName;
    m_chara = fn_8015CD90(0x54, 4);
    fn_8015CF7C(m_chara.get());
    fn_800729B0(&m_chara->m_commandUnit->m_callback,
                &(const CharCommandCallback &)CharCommandCallback(
                    boost::bind(&CityBombBig::OnCommand, this, _1)));
    ReadPlacement(owner);
    m_model = fn_8015F800(1, "CITYBOMBBIG.BIN");
    m_model->fn_80162A68(0, 0);
    fn_80161FC8(m_model.get(), 0, m_chara);
    {
        boost::shared_ptr<CharaColliAttack> collision = fn_80161428(m_model.get(), 0).lock();
        if (collision.get() != 0) {
            fn_80096B64(collision.get(), 1, boost::bind(&CityBombBig::OnAttack, this, _1));
        }
    }
    m_target = fn_801E7128(m_chara, 0xAC00);
    m_sound = fn_802B4254(0, 0);
    fn_802B4498(m_sound.px, s_soundRange);
    fn_802B4510(m_sound.px, 1);
    RebuildTransform();
    m_particles = fn_80166AF4();
    {
        UnkParticleAttach_80169D00 ref;
        ref.ctrl = m_particles;
        fn_8015FA78(m_model.get(), fn_80169D00, &ref);
    }
    fn_80167180(m_particles.px, 1);
    m_tickSound = fn_801D3E58(0x714A, &m_chara->m_position, this);
}

CityBombBig::~CityBombBig() {
    m_tickSound.fn_801D455C();
}

extern "C" void fn_802F37C0(void) {
    fn_8006B474("CITYBOMBBIG.BIN", 1);
}

extern "C" void fn_802F37BC(void) {}

void CityBombBig::Instantiate(UnkPlacement_800C9CD0 *owner) {
    new CityBombBig(fn_801EB304(fn_8001C4F8(), 11), owner);
}

void CityBombBig::Behave(float dt) {
    switch (m_state) {
    case 1:
        if (m_flags2.Test(0)) {
            fn_80166E84(m_particles.px, 1);
            m_tickSound.fn_801D455C();
            fn_80167180(m_particles.px, 2);
            fn_801D54F4(fn_80053164(), 0x7131, m_chara.px, 0, 1);
            fn_801D54F4(fn_80053164(), 0x7132, m_chara.px, 0, 1);
            m_state = 2;
            m_flags2.Clear(0);
        }
        break;
    case 2:
        m_timer -= dt;
        if (m_timer < 0.0f || fn_8000A950(m_timer)) {
            fn_80162790(m_model.get());
            m_model->fn_80161880();
            fn_80166F88(m_particles.px);
            {
                UnkDebrisParam_8015F0B8 debris(m_chara, 0);
                UnkModelPart_8015F7BC *part = fn_8015F7BC(m_model.get(), 1000);

                debris.model = fn_80012700(fn_800122D4(), 1, part->name);
                debris.field_0x40 = m_chara->m_position.y;
                debris.scale = 1.0f;
                debris.field_0x4A = 0;
                debris.field_0x54 = 1;
                fn_8015D93C(&debris);
            }
            fn_801DD0BC(m_chara.px, 2, 25.0f, 7);
            fn_801D54F4(fn_80053164(), 0x714B, m_chara.px, 0, 1);
            {
                int index = lbl_8057D6A0.field_0x125 ? 1 : 0;
                boost::shared_ptr<UnkPlayer_800740E4> player =
                    fn_800740E4(fn_8006D6A8(), index).lock();
                if (player.get() != 0) {
                    player->Hit(1, 0);
                }
            }
            fn_8016907C(fn_800CB69C(), m_switchId, 1);
            fn_8000D664(fn_80009548(), 0, 2.2f, 3.0f, 15.0f);
            fn_800CC978(&m_sound);
            fn_800C9B04(m_owner);
            m_flags |= 1;
        }
        break;
    }
    m_model->fn_80162FB0(dt);
    if (m_particles.get() != 0) {
        fn_8016761C(m_particles.px, dt);
    }
}

void CityBombBig::ReloadPlacement(UnkPlacement_800C9CD0 *owner) {
    ReadPlacement(owner);
    RebuildTransform();
    m_model->fn_80162EF4();
    m_model->fn_8016108C();
}

void CityBombBig::ReadPlacement(UnkPlacement_800C9CD0 *owner) {
    fn_800C9F14(owner, &m_chara->m_position);
    fn_800C9ED4(owner, &m_chara->m_rotation);
    fn_800C9E50(owner, &m_switchId);
    m_health = 10.0f;
    m_timer = 1.0f;
    m_state = 1;
    m_flags2.Clear();
}

void CityBombBig::RebuildTransform() {
    Mtx34f matrix;

    fn_8040C148(&matrix, &lbl_805E0720);
    fn_8040B59C(&matrix, &m_chara->m_position);
    fn_8040B848(&matrix, m_chara->m_rotation.y);
    m_model->fn_801633DC(&matrix);
    m_target->fn_800CCE38(&m_chara->m_position);
    if (m_sound.get() != 0) {
        Vec3f offset(0.0f, s_soundHeight, 0.0f);
        Vec3f position;

        fn_8040CDD0(&position, &offset, &m_chara->m_position);
        fn_802B44C8(m_sound.px, &position);
    }
}

void CityBombBig::OnCommand(CharCommand &command) {
    switch (command.TypeId()) {
    case 0x1A:
        if (m_state != 2) {
            m_state = 0;
        }
        break;
    case 0x1B:
        if (m_state == 0) {
            m_state = 1;
        }
        break;
    }
    command.field_0x4[3] |= 2;
}

void CityBombBig::OnAttack(AttackCallbackParam &param) {
    param.m_result = 5;
    if (m_state == 1) {
        param.m_result = 1;
        m_health -= param.m_damage;
        AssignVector(*reinterpret_cast<Vec3f *>(param.field_0x8 + (0x40 - 0x8)),
                     *reinterpret_cast<Vec3f *>(param.field_0x8 + (0x24 - 0x8)));
        if (m_health < 0.0f || fn_8000A950(m_health)) {
            param.m_result = 3;
            if (!m_flags2.Test(0)) {
                boost::shared_ptr<CharInfo> attacker = fn_8015D570(param);
                if (attacker.get() != 0) {
                    fn_8020642C(attacker, 0x36);
                }
                m_flags2.Set(0);
            }
        }
    }
}

GadgetClassDesc lbl_8055A070 = {
    "CITY_BOMB_BIG", fn_802F37C0, fn_802F37BC, CityBombBig::Instantiate, 0, 0, 0xFA0, 1, 0, 10, 0,
};
