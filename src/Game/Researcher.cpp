/* Researcher (class name from RTTI): the Researcher gadget, a researcher (RESEARCHER.BIN) lying
 * face up or face down ("Type 0:FaceUp 1:FaceDown", the model variant). Its character info is a
 * ResearcherCharInfo (class name from RTTI). Command 0x10 sent to it (a rescue, inferred) plays a
 * sound, gives a player and the first character a reward, tells the owner it is gone and fades the
 * model out over two seconds before it finishes. It carries a positioned object (see ElecBomb.cpp)
 * above it. File name inferred from the class. Names marked "(name inferred)" are not original.
 * Definitions are in reverse address order (deferred inlining emits functions last-defined
 * first). */
#include <boost/bind.hpp>
#include <boost/function.hpp>

#include "Game/CharCommand.h"
#include "Game/CharCommandUnit.h"
#include "Game/CharInfo.h"
#include "Game/Gadget.h"
#include "Game/GadgetCharInfo.h"
#include "Game/GadgetModelInfo.h"
#include "Game/GadgetParticle.h"
#include "Game/GadgetClassDesc.h"
#include "Game/GadgetUtil.h"
#include "Game/FileQueue.h"
#include "boost/shared_ptr.hpp"
#include "boost/weak_ptr.hpp"
#include "size_assert.h"

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

/* The game state block in .bss (only the member used here). */
struct UnkGameState_8057D6A0 {
    /* 0x000 */ u8 field_0x0[0x125];
    /* 0x125 */ u8 field_0x125; /* meaning unknown */
    /* 0x126 */ u8 field_0x126[0x150 - 0x126];
};
SIZE_ASSERT(UnkGameState_8057D6A0, 0x150);

extern "C" {
extern UnkGameState_8057D6A0 lbl_8057D6A0;

/* Tells the owner that the gadget is gone (behaviour inferred). */
void fn_800C9B04(UnkPlacement_800C9CD0 *owner);
/* Gives the character info a command handler unless it has one (behaviour inferred). */
void fn_8015CF7C(GadgetCharInfo *charInfo);

/* GadgetModelInfo helpers (see Monster.cpp and Helicopter.cpp). */
boost::shared_ptr<GadgetModelInfo> fn_8015F800(int arg0, const char *fileName);
void fn_80160A84(GadgetModelInfo *model, int arg1, int arg2);
bool fn_80161BD8(GadgetModelInfo *model, int index, const boost::weak_ptr<CharInfo> &info,
                 int arg3);
void fn_8015FDD8(GadgetModelInfo *model, float alpha);
void fn_8015FA78(GadgetModelInfo *model,
                 void *(*callback)(void *part, UnkParticleAttach_80169D00 *param),
                 UnkParticleAttach_80169D00 *param);
void *fn_80169D00(void *part, UnkParticleAttach_80169D00 *param);

/* Particle controller helpers (behaviour inferred). */
boost::shared_ptr<GadgetParticle::GadgetParticleCtrl> fn_80166AF4(void);
void fn_80166F88(GadgetParticle::GadgetParticleCtrl *ctrl);
void fn_80167448(GadgetParticle::GadgetParticleCtrl *ctrl, u32 id);
void fn_8016761C(GadgetParticle::GadgetParticleCtrl *ctrl, float dt);

/* Creates a Target for owner (behaviour inferred; the flags are unknown). */
boost::shared_ptr<Target> fn_801E7128(const boost::weak_ptr<CharInfo> &owner, int flags);
/* Sends message to the character (behaviour and meaning of message inferred). */
int fn_8020642C(const boost::shared_ptr<CharInfo> &character, int message);

/* The positioned object (see ElecBomb.cpp). */
boost::shared_ptr<UnkSound_802B4254> fn_802B4254(int kind, int arg1);
void fn_802B4498(UnkSound_802B4254 *sound, float range);
void fn_802B4510(UnkSound_802B4254 *sound, u8 flag);
void fn_802B44C8(UnkSound_802B4254 *sound, const Vec3f *position);
void fn_800CC978(boost::shared_ptr<UnkSound_802B4254> *sound);

/* Plays sound id at a character info through the sound manager (fn_80053164, GadgetUtil.h;
 * behaviour inferred). */
void fn_801D54F4(void *manager, int id, GadgetCharInfo *source, int arg3, int arg4);

/* The player manager and its entry (see ElecBomb.cpp). */
UnkPlayerManager_8006D6A8 *fn_8006D6A8(void);
boost::weak_ptr<UnkPlayer_800740E4> fn_800740E4(UnkPlayerManager_8006D6A8 *manager, int index);
/* A character of the manager at fn_80074194 by index (behaviour inferred). */
boost::weak_ptr<CharInfo> fn_800A8C54(UnkCharaList_80074194 *manager, int index);

/* destination = a + b (behaviour inferred). */
void fn_8040CDD0(Vec3f *destination, const Vec3f *a, const Vec3f *b);
}

/* fn_8040CDD0 called as returning a + b by value (it stores the result through its first
 * argument). */
typedef Vec3f (*VectorAddFn)(const Vec3f *a, const Vec3f *b);

/* A length (name inferred). As a call it is not a constant expression, so the constants below are
 * stored by a static initializer, as in retail. */
inline float Span(float length) {
    return length;
}

/* The height of the positioned object above the researcher, and its range (names inferred). */
static float s_soundHeight = Span(10.0f);
static float s_soundRange = Span(200.0f);

/* Sets the word at 0x5C of a character info (name inferred; compare MonsterCharInfo::SetField5C).
 * Retail calls the shared_ptr's operator-> out of line before it, as for an inline call. */
inline void SetKind(CharInfo *info, s32 kind) {
    info->field_0x5C = kind;
}

/* ResearcherCharInfo (class name from RTTI): the researcher's character info. */
class ResearcherCharInfo : public GadgetCharInfo {
  public:
    ResearcherCharInfo(int type) : GadgetCharInfo(type) {}
    virtual ~ResearcherCharInfo() {}
    virtual bool vf_0x2C() {
        return true;
    }

    /* Creates a ResearcherCharInfo of character type type and sets its word at 0x5C to kind (name
     * inferred; compare fn_8015CD90). */
    static boost::shared_ptr<GadgetCharInfo> Instantiate(int type, s32 kind);
};
SIZE_ASSERT(ResearcherCharInfo, 0x6C);

/* The parameter the owner supplies (name from the parameter table). */
struct ResearcherParams {
    /* 0x0 */ u32 type; /* "Type 0:FaceUp 1:FaceDown", the model variant */
};

class Researcher : public Gadget {
  public:
    Researcher(Task *parent, UnkPlacement_800C9CD0 *owner);
    virtual ~Researcher();
    virtual void Behave(float dt);
    virtual void ReloadPlacement(UnkPlacement_800C9CD0 *owner);

    /* Creates the gadget under task layer 11 (name inferred). */
    static void Instantiate(UnkPlacement_800C9CD0 *owner);

    /* Copies the owner's placement and type (name inferred). */
    void ReadPlacement(UnkPlacement_800C9CD0 *owner);
    /* Sets up the model of the current type and its particles (name inferred). */
    void SetupModel();
    /* Places the model and the target (name inferred). */
    void RebuildTransform();
    /* The command unit's callback (name inferred). */
    void OnCommand(CharCommand &command);

    static const char *s_taskName;

    /* Member names are inferred. */
    /* 0x30 */ boost::shared_ptr<GadgetCharInfo> m_chara;
    /* 0x38 */ boost::shared_ptr<GadgetModelInfo> m_model;
    /* 0x40 */ boost::shared_ptr<GadgetParticle::GadgetParticleCtrl> m_particles;
    /* 0x48 */ boost::shared_ptr<Target> m_target;
    /* 0x50 */ boost::shared_ptr<UnkSound_802B4254> m_sound;
    /* 0x58 */ ResearcherParams m_params;
    /* 0x5C */ s32 m_state; /* 0 waiting, 1 rescued, 2 fading, 3 gone */
    /* 0x60 */ float m_timer;
    /* 0x64 */ u32 pad_0x64; /* the class size is a multiple of 8 */
};
SIZE_ASSERT(Researcher, 0x68);

const char *Researcher::s_taskName = "Researcher";

boost::shared_ptr<GadgetCharInfo> ResearcherCharInfo::Instantiate(int type, s32 kind) {
    boost::shared_ptr<GadgetCharInfo> info =
        boost::shared_ptr<GadgetCharInfo>(new ResearcherCharInfo(type));
    SetKind(info.operator->(), kind);
    return info;
}

Researcher::Researcher(Task *parent, UnkPlacement_800C9CD0 *owner)
    : Gadget(parent, owner), m_state(0) {
    m_name = s_taskName;
    m_chara = ResearcherCharInfo::Instantiate(0x5C, 4);
    fn_8015CF7C(m_chara.get());
    fn_800729B0(&m_chara->m_commandUnit->m_callback,
                &(const CharCommandCallback &)CharCommandCallback(
                    boost::bind(&Researcher::OnCommand, this, _1)));
    ReadPlacement(owner);
    m_model = fn_8015F800(1, "RESEARCHER.BIN");
    m_target = fn_801E7128(m_chara, 0xA000);
    m_sound = fn_802B4254(2, 0);
    fn_802B4498(m_sound.px, s_soundRange);
    fn_802B4510(m_sound.px, 1);
    {
        Vec3f offset(0.0f, s_soundHeight, 0.0f);

        fn_802B44C8(m_sound.px, &(const Vec3f &)((VectorAddFn)fn_8040CDD0)(
                                    &m_chara->m_position, &offset));
    }
    SetupModel();
}

Researcher::~Researcher() {}

extern "C" void fn_80318DB4(void) {
    fn_8006B474("RESEARCHER.BIN", 1);
}

extern "C" void fn_80318DB0(void) {}

void Researcher::Instantiate(UnkPlacement_800C9CD0 *owner) {
    new Researcher(fn_801EB304(fn_8001C4F8(), 11), owner);
}

static GadgetParamDesc lbl_8055C1A4[] = {
    {2, "Type 0:FaceUp 1:FaceDown", 0, 0, 1, 0.0f, 0.0f, 0.0f, fn_800CBC20, 0},
    {0},
};

GadgetClassDesc lbl_8055C1F4 = {
    "Researcher", fn_80318DB4, fn_80318DB0, Researcher::Instantiate, 0, 0, 0x1005, 1, 0, 10,
    lbl_8055C1A4,
};

void Researcher::Behave(float dt) {
    switch (m_state) {
    case 1:
        m_timer -= dt;
        if (m_timer < 0.0f) {
            m_timer = 2.0f;
            m_state = 2;
        }
        break;
    case 2:
        m_timer -= dt;
        if (m_timer < 0.0f) {
            m_model->fn_80161880();
            m_model->fn_801604FC();
            m_model->fn_80162804();
            m_timer = 0.5f;
            m_state = 3;
        } else {
            fn_8015FDD8(m_model.get(), m_timer / 2.0f);
        }
        break;
    case 3:
        m_timer -= dt;
        if (m_timer < 0.0f) {
            fn_80166F88(m_particles.px);
            m_flags |= 1;
            return;
        }
        break;
    }
    RebuildTransform();
    m_model->fn_80162FB0(dt);
    if (m_particles.get() != 0) {
        fn_8016761C(m_particles.px, dt);
    }
}

void Researcher::ReloadPlacement(UnkPlacement_800C9CD0 *owner) {
    ResearcherParams old = m_params;
    ReadPlacement(owner);
    if (m_params.type != old.type) {
        m_model->fn_80162804();
        m_model->fn_80161880();
        m_model->fn_801604FC();
        m_state = 0;
        m_timer = 0.0f;
        SetupModel();
    }
    RebuildTransform();
    m_model->fn_80162EF4();
    m_model->fn_8016108C();
}

void Researcher::ReadPlacement(UnkPlacement_800C9CD0 *owner) {
    fn_800C9F14(owner, &m_chara->m_position);
    fn_800C9ED4(owner, &m_chara->m_rotation);
    fn_800C9DBC(owner, &m_params, sizeof(m_params));
    m_timer = 0.0f;
}

void Researcher::SetupModel() {
    m_model->fn_80162A68(m_params.type, 1);
    m_model->fn_80160C28(m_params.type);
    m_model->fn_8015FF60();
    fn_80161BD8(m_model.get(), 0, m_chara, m_params.type);
    fn_80160A84(m_model.get(), m_params.type, 1);
    RebuildTransform();
    m_particles = fn_80166AF4();

    UnkParticleAttach_80169D00 attach;
    attach.ctrl = m_particles;
    fn_8015FA78(m_model.get(), fn_80169D00, &attach);
}

void Researcher::RebuildTransform() {
    Mtx34f matrix;

    fn_8040C148(&matrix, &lbl_805E0720);
    fn_8040B59C(&matrix, &m_chara->m_position);
    fn_8040B848(&matrix, m_chara->m_rotation.y);
    m_model->fn_801633DC(&matrix);
    m_target->fn_800CCE38(&m_chara->m_position);
}

void Researcher::OnCommand(CharCommand &command) {
    switch (command.TypeId()) {
    case 0x10:
        if (m_state == 0) {
            int index = 2;
            if (lbl_8057D6A0.field_0x125 != 0) {
                index = 1;
            }
            {
                boost::shared_ptr<UnkPlayer_800740E4> player =
                    fn_800740E4(fn_8006D6A8(), index).lock();
                if (player.get() != 0) {
                    player->Hit(1, 0);
                }
            }
            fn_800CC978(&m_sound);
            {
                boost::shared_ptr<CharInfo> character = fn_800A8C54(fn_80074194(), 0).lock();
                if (character.get() != 0) {
                    fn_8020642C(character, 0x28);
                }
            }
            fn_80160A84(m_model.get(), m_params.type, 2);
            fn_80167448(m_particles.px, -1);
            fn_801D54F4(fn_80053164(), 0x7634, m_chara.px, 0, 1);
            fn_800C9B04(m_owner);
            m_timer = 0.5f;
            m_state = 1;
        }
        break;
    }
    command.Flags() |= 2;
}
