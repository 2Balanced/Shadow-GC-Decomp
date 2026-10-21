/* CityBombSmall (class name from RTTI): the CITY_BOMB_SMALL gadget, a small bomb
 * (CITYBOMBSMALL.BIN) with particles. It arms itself once a character comes within its "Range
 * (point)" (in the stage whose number is 8 the character the manager at fn_800DDF20 keeps under id
 * 0x91 counts too) or once attacks wear its health down. Armed, it explodes after a second: it
 * breaks into its debris, sets its switch and finishes. A vacuum can pull it in before it is armed:
 * it then hands its model, sound and particles to a VacuumCityBombSmall (class name from RTTI),
 * hits the player, sets its switch and finishes. In the stage whose number is 12 it carries a
 * positioned object (see ElecBomb.cpp). File name inferred from the class. Method names marked
 * "(name inferred)" are not original. Definitions are in reverse address order (deferred inlining
 * emits functions last-defined first). */
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
#include "Game/GadgetClassDesc.h"
#include "Game/GadgetUtil.h"
#include "Game/Mission.h"
#include "Game/UnkSwitchBank_800CB69C.h"
#include "Game/VacuumObject.h"
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

class UnkPlayerManager_8006D6A8;
class UnkCharManager_800DDF20;

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
    /* 0x000 */ u8 field_0x0[0xAC];
    /* 0x0AC */ s32 m_stage; /* (name inferred) */
    /* 0x0B0 */ u8 field_0xB0[0x150 - 0xB0];
};
SIZE_ASSERT(UnkGameState_8057D6A0, 0x150);

/* fn_8040CD74 called as returning a - b by value (it stores the result through its first
 * argument). */
typedef Vec3f (*VectorSubtractFn)(const Vec3f *a, const Vec3f *b);
/* fn_8040CDD0 called as returning a + b by value, likewise. */
typedef Vec3f (*VectorAddFn)(const Vec3f *a, const Vec3f *b);

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
void fn_80166F88(GadgetParticle::GadgetParticleCtrl *ctrl);
void fn_80167010(GadgetParticle::GadgetParticleCtrl *ctrl, int id);
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

/* Plays sound id at a character info through the sound manager (fn_80053164, GadgetUtil.h;
 * behaviour inferred). */
void fn_801D54F4(void *manager, int id, GadgetCharInfo *source, int arg3, int arg4);

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
boost::weak_ptr<Mission::Mission> fn_800740E4(UnkPlayerManager_8006D6A8 *manager, int index);
/* A character of the manager at fn_80074194 by index (behaviour inferred). */
boost::weak_ptr<CharInfo> fn_800A8C54(UnkCharaList_80074194 *manager, int index);
/* A manager of characters and its character with id (behaviour inferred). */
UnkCharManager_800DDF20 *fn_800DDF20(void);
boost::weak_ptr<CharInfo> *fn_801DCB14(UnkCharManager_800DDF20 *manager, int id);

/* destination = a + b (behaviour inferred). */
void fn_8040CDD0(Vec3f *destination, const Vec3f *a, const Vec3f *b);
}

/* A tenth of the vacuum objects' length (name and meaning unknown; see VacuumObject.h). */
static float s_vacuumLength = lbl_805F72B0 / 10.0f;

/* A length (name inferred). As a call it is not a constant expression, so the constants below are
 * stored by a static initializer, as in retail. */
inline float Span(float length) {
    return length;
}

/* The height of the positioned object above the bomb, and its range (names inferred). */
static float s_soundHeight = Span(16.0f);
static float s_soundRange = Span(400.0f);

/* The flag word at 0x60 (struct name unknown). */
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

/* VacuumCityBombSmall (class name from RTTI): the bomb's model, positioned object and particles
 * while a vacuum pulls them in (names inferred). */
class VacuumCityBombSmall : public VacuumObject {
  public:
    VacuumCityBombSmall(VacuumCommand &command, const boost::shared_ptr<GadgetModelInfo> &model,
                        const boost::shared_ptr<UnkSound_802B4254> &sound,
                        const boost::shared_ptr<GadgetParticle::GadgetParticleCtrl> &particles);
    virtual ~VacuumCityBombSmall();
    virtual void Tick(float dt);
    virtual void fn_80238180();

    /* Hands the bomb's parts to a new VacuumCityBombSmall at position and rotation and starts a
     * task for it (name inferred). */
    static void Start(VacuumCommand &command, const Vec3f &position,
                      const Vec3f &rotation,
                      const boost::shared_ptr<GadgetModelInfo> &model,
                      const boost::shared_ptr<UnkSound_802B4254> &sound,
                      const boost::shared_ptr<GadgetParticle::GadgetParticleCtrl> &particles);

    /* 0x08 */ boost::shared_ptr<GadgetModelInfo> m_model;
    /* 0x10 */ boost::shared_ptr<UnkSound_802B4254> m_sound;
    /* 0x18 */ boost::shared_ptr<GadgetParticle::GadgetParticleCtrl> m_particles;
};
SIZE_ASSERT(VacuumCityBombSmall, 0x20);

class CityBombSmall : public Gadget {
  public:
    CityBombSmall(Task *parent, UnkPlacement_800C9CD0 *owner);
    virtual ~CityBombSmall();
    virtual void Behave(float dt);
    virtual void ReloadPlacement(UnkPlacement_800C9CD0 *owner);

    /* Creates the gadget under task layer 11 (name inferred). */
    static void Instantiate(UnkPlacement_800C9CD0 *owner);

    /* Copies the owner's placement, range and switch id and resets the state (name inferred). */
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
    /* 0x48 */ float m_reach; /* "Range (point)" */
    /* 0x4C */ boost::shared_ptr<Target> m_target;
    /* 0x54 */ boost::shared_ptr<UnkSound_802B4254> m_sound;
    /* 0x5C */ s32 m_state; /* 0 waiting, 1 armed, 2 vacuumed */
    /* 0x60 */ CityBombFlags m_flags2;
    /* 0x64 */ float m_health;
    /* 0x68 */ float m_timer;
    /* 0x6C */ s32 m_switchId;
};
SIZE_ASSERT(CityBombSmall, 0x70);

const char *CityBombSmall::s_taskName = "CityBombSmall";

CityBombSmall::CityBombSmall(Task *parent, UnkPlacement_800C9CD0 *owner)
    : Gadget(parent, owner), m_reach(0.0f), m_state(0) {
    m_name = s_taskName;
    m_chara = fn_8015CD90(0x55, 4);
    fn_8015CF7C(m_chara.get());
    fn_800729B0(&m_chara->m_commandUnit->m_callback,
                &(const CharCommandCallback &)CharCommandCallback(
                    boost::bind(&CityBombSmall::OnCommand, this, _1)));
    ReadPlacement(owner);
    m_model = fn_8015F800(1, "CITYBOMBSMALL.BIN");
    m_model->fn_80162A68(0, 0);
    fn_80161FC8(m_model.get(), 0, m_chara);
    {
        boost::shared_ptr<CharaColliAttack> collision = fn_80161428(m_model.get(), 0).lock();
        if (collision.get() != 0) {
            fn_80096B64(collision.get(), 1, boost::bind(&CityBombSmall::OnAttack, this, _1));
        }
    }
    m_target = fn_801E7128(m_chara, 0xA480);
    if (lbl_8057D6A0.m_stage == 12) {
        m_sound = fn_802B4254(2, 0);
        fn_802B4498(m_sound.px, s_soundRange);
        fn_802B4510(m_sound.px, 1);
    }
    RebuildTransform();
    m_particles = fn_80166AF4();
    {
        UnkParticleAttach_80169D00 ref;
        ref.ctrl = m_particles;
        fn_8015FA78(m_model.get(), fn_80169D00, &ref);
    }
    fn_80167010(m_particles.px, 1);
}

CityBombSmall::~CityBombSmall() {}

extern "C" void fn_802F5340(void) {
    fn_8006B474("CITYBOMBSMALL.BIN", 1);
}

extern "C" void fn_802F533C(void) {}

void CityBombSmall::Instantiate(UnkPlacement_800C9CD0 *owner) {
    new CityBombSmall(fn_801EB304(fn_8001C4F8(), 11), owner);
}

void CityBombSmall::Behave(float dt) {
    switch (m_state) {
    case 0:
        if (lbl_8057D6A0.m_stage == 8) {
            boost::weak_ptr<CharInfo> *other = fn_801DCB14(fn_800DDF20(), 0x91);
            if (other != 0) {
                boost::shared_ptr<CharInfo> character = other->lock();
                if (character.get() != 0) {
                    Vec3f offset;

                    fn_800091EC(&offset, &(const Vec3f &)((VectorSubtractFn)fn_8040CD74)(
                                             &m_chara->m_position, &character->m_position));
                    if (fn_8040CF50(&offset) < m_reach) {
                        m_flags2.Set(0);
                    }
                }
            }
        }
        {
            boost::shared_ptr<CharInfo> nearest =
                fn_800A8A7C(fn_80074194(), &m_chara->m_position).lock();
            if (nearest.get() != 0) {
                Vec3f offset;

                fn_800091EC(&offset, &(const Vec3f &)((VectorSubtractFn)fn_8040CD74)(
                                         &m_chara->m_position, &nearest->m_position));
                if (fn_8040CF50(&offset) < m_reach) {
                    m_flags2.Set(0);
                }
            }
        }
        if (m_flags2.Test(0)) {
            fn_80167180(m_particles.px, 2);
            fn_801D54F4(fn_80053164(), 0x7131, m_chara.px, 0, 1);
            m_state = 1;
            m_flags2.Clear(0);
        }
        break;
    case 1:
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
            fn_801DD0BC(m_chara.px, 1, 8.0f, 7);
            fn_801D54F4(fn_80053164(), 0x714C, m_chara.px, 0, 1);
            fn_8016907C(fn_800CB69C(), m_switchId, 1);
            fn_800CC978(&m_sound);
            fn_800C9B04(m_owner);
            m_flags |= 1;
        }
        break;
    }
    if (m_model.get() != 0) {
        m_model->fn_80162FB0(dt);
    }
    if (m_particles.get() != 0) {
        fn_8016761C(m_particles.px, dt);
    }
}

void CityBombSmall::ReloadPlacement(UnkPlacement_800C9CD0 *owner) {
    ReadPlacement(owner);
    RebuildTransform();
    if (m_model.get() != 0) {
        m_model->fn_80162EF4();
        m_model->fn_8016108C();
    }
}

void CityBombSmall::ReadPlacement(UnkPlacement_800C9CD0 *owner) {
    fn_800C9F14(owner, &m_chara->m_position);
    fn_800C9ED4(owner, &m_chara->m_rotation);
    fn_800C9E50(owner, &m_switchId);
    fn_800C9DBC(owner, &m_reach, sizeof(m_reach));
    m_health = 5.0f;
    m_timer = 1.0f;
    m_state = 0;
    m_flags2.Clear();
}

void CityBombSmall::RebuildTransform() {
    Mtx34f matrix;

    fn_8040C148(&matrix, &lbl_805E0720);
    fn_8040B59C(&matrix, &m_chara->m_position);
    fn_8040B848(&matrix, m_chara->m_rotation.y);
    if (m_model.get() != 0) {
        m_model->fn_801633DC(&matrix);
    }
    if (m_target.get() != 0) {
        m_target->fn_800CCE38(&m_chara->m_position);
    }
    if (m_sound.get() != 0) {
        Vec3f offset(0.0f, s_soundHeight, 0.0f);
        Vec3f position;

        fn_8040CDD0(&position, &offset, &m_chara->m_position);
        fn_802B44C8(m_sound.px, &position);
    }
}

void CityBombSmall::OnCommand(CharCommand &command) {
    switch (command.TypeId()) {
    case 0x14: /* VacuumCommand */
        if (m_state == 0 && !m_flags2.Test(0)) {
            VacuumCityBombSmall::Start(static_cast<VacuumCommand &>(command),
                                       m_chara->m_position, m_chara->m_rotation, m_model,
                                       m_sound, m_particles);
            m_model.reset();
            m_particles.reset();
            fn_800CC978(&m_sound);
            m_state = 2;
            {
                boost::shared_ptr<Mission::Mission> player =
                    fn_800740E4(fn_8006D6A8(), 2).lock();
                if (player.get() != 0) {
                    player->AddCount(1, 0);
                }
            }
            fn_8016907C(fn_800CB69C(), m_switchId, 1);
            {
                boost::shared_ptr<CharInfo> character = fn_800A8C54(fn_80074194(), 0).lock();
                if (character.get() != 0) {
                    fn_8020642C(character, 0x38);
                }
            }
            fn_800C9B04(m_owner);
            m_flags |= 1;
        }
        break;
    }
    command.field_0x4[3] |= 2;
}

void CityBombSmall::OnAttack(AttackCallbackParam &param) {
    param.m_result = 5;
    if (m_state == 0) {
        param.m_result = 1;
        m_health -= param.m_damage;
        AssignVector(*reinterpret_cast<Vec3f *>(param.field_0x8 + (0x40 - 0x8)),
                     *reinterpret_cast<Vec3f *>(param.field_0x8 + (0x24 - 0x8)));
        if (m_health < 0.0f || fn_8000A950(m_health)) {
            param.m_result = 3;
            if (!m_flags2.Test(0)) {
                boost::shared_ptr<CharInfo> attacker = fn_8015D570(param);
                if (attacker.get() != 0) {
                    fn_8020642C(attacker, 0x37);
                }
                m_flags2.Set(0);
            }
        }
    }
}

VacuumCityBombSmall::VacuumCityBombSmall(
    VacuumCommand &command, const boost::shared_ptr<GadgetModelInfo> &model,
    const boost::shared_ptr<UnkSound_802B4254> &sound,
    const boost::shared_ptr<GadgetParticle::GadgetParticleCtrl> &particles)
    : VacuumObject(fn_80237EDC(&command), command), m_model(model), m_sound(sound),
      m_particles(particles) {}

VacuumCityBombSmall::~VacuumCityBombSmall() {}

void VacuumCityBombSmall::Tick(float dt) {
    VacuumObject::Tick(dt);
    Vec3f position;
    fn_800091EC(&position, &(const Vec3f &)fn_802382B0(this));
    Vec3f rotation = fn_80238248(this);
    if (m_model.get() != 0) {
        Mtx34f matrix;
        Mtx34f translation;

        fn_8040B034(&translation, &position);
        fn_8040C148(&matrix, &translation);
        fn_8040B848(&matrix, rotation.y);
        fn_8040B884(&matrix, rotation.x);
        fn_8040B80C(&matrix, rotation.z);
        m_model->fn_801633DC(&matrix);
        m_model->fn_80162FB0(dt);
    }
    if (m_sound.get() != 0) {
        Vec3f offset(0.0f, s_soundHeight, 0.0f);

        fn_802B44C8(m_sound.px, &(const Vec3f &)((VectorAddFn)fn_8040CDD0)(&position, &offset));
    }
    if (m_particles.get() != 0) {
        fn_8016761C(m_particles.px, dt);
    }
}

void VacuumCityBombSmall::fn_80238180() {}

void VacuumCityBombSmall::Start(VacuumCommand &command, const Vec3f &position,
                                const Vec3f &rotation,
                                const boost::shared_ptr<GadgetModelInfo> &model,
                                const boost::shared_ptr<UnkSound_802B4254> &sound,
                                const boost::shared_ptr<GadgetParticle::GadgetParticleCtrl> &particles) {
    boost::shared_ptr<VacuumObject> object =
        boost::shared_ptr<VacuumObject>(new VacuumCityBombSmall(command, model, sound, particles));
    object->fn_802382E0(position);
    object->fn_80238268(rotation);
    new VacuumObjectTask(object);
}

static GadgetParamDesc lbl_8055A130[] = {
    {4, "Range (point)", 0, 0, 0, 50.0f, 0.0f, 65536.0f, fn_800CBC20, 0},
    {0},
};

GadgetClassDesc lbl_8055A180 = {
    "CITY_BOMB_SMALL", fn_802F5340, fn_802F533C, CityBombSmall::Instantiate, 0, 0, 0xFA1, 1, 0, 10,
    lbl_8055A130,
};
