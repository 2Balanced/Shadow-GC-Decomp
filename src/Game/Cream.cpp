/* Cream (class name from RTTI): the Cream gadget of Circus Park, a cream figure (CREAM.BIN) with a
 * sound source above it. The first time a player touches it, it plays a motion, gives a hint
 * (0x3D1, or 0x3D0 for another character), stops its sound and switches on the switch its owner
 * names; an attack makes it play a hurt motion and sends the attacker message 0x2E once. What it
 * already did survives the gadget being re-created through a CreamSetState kept by the owner. File
 * name inferred from the class. Method names marked "(name inferred)" are not original.
 * Definitions are in reverse address order (deferred inlining emits functions last-defined
 * first). */
#include <boost/bind.hpp>
#include <boost/function.hpp>

#include "Game/CharInfo.h"
#include "Game/CharaColli.h"
#include "Game/CharaColliAttack.h"
#include "Game/Gadget.h"
#include "Game/GadgetCharInfo.h"
#include "Game/GadgetModelInfo.h"
#include "Game/GadgetClassDesc.h"
#include "Game/Mission.h"
#include "Game/SetAdapterState.h"
#include "Game/UnkSwitchBank_800CB69C.h"
#include "Game/FileQueue.h"
#include "boost/shared_ptr.hpp"
#include "boost/weak_ptr.hpp"
#include "size_assert.h"

using SonicteamUSA::System::CharaColli::AttackCallback;
using SonicteamUSA::System::CharaColli::AttackCallbackParam;
using SonicteamUSA::System::CharaColli::CharaColliAttack;
using SonicteamUSA::System::CharaColli::CharaColliBase;

/* Target (class name from RTTI): a point other objects can aim at (meaning inferred). */
class Target {
  public:
    /* Moves the target to position (name inferred). */
    void fn_800CCE38(const Vec3f *position);
};

/* The positioned object fn_802B4254 creates (see ElecBomb.cpp; it may be a sound source). */
class UnkSound_802B4254;

class UnkPlayerManager_8006D6A8;
class UnkHintManager_801FB180;
struct _stMtnPlayWork;

/* A 32-bit flag word whose constructor clears it (fn_80014E04): fn_80014E10 clears it, fn_80014E1C
 * sets bit and fn_80014E34 tests bit (behaviour inferred from their bodies; name inferred). */
extern "C" {
void fn_80014E04(u32 *flags);
void fn_80014E10(u32 *flags);
void fn_80014E1C(u32 *flags, int bit);
bool fn_80014E34(u32 *flags, int bit);
}

struct UnkFlags_80014E04 {
    UnkFlags_80014E04() {
        fn_80014E04(&m_bits);
    }

    u32 m_bits;
};

/* CreamSetState (class name from RTTI): the cream's state kept by the owner: bit 0 set once a
 * player touched it, bit 1 once an attacker was told (meanings inferred). */
class CreamSetState : public SetAdapterState {
  public:
    CreamSetState() {
        fn_80014E10(&m_flags.m_bits);
    }

    /* 0x4 */ UnkFlags_80014E04 m_flags;
};
SIZE_ASSERT(CreamSetState, 0x8);

extern "C" {
/* The collider manager, a sphere collider for owner, and the attack collision of a collider
 * (behaviour inferred). */
void *fn_80021B08(void);
boost::shared_ptr<CharaColliBase> fn_800C7DEC(void *manager, const boost::weak_ptr<CharInfo> &owner,
                                              int arg2, int arg3, int arg4, const Vec3f *offset,
                                              float radius);
boost::weak_ptr<CharaColliAttack> fn_800C4374(void *manager, boost::shared_ptr<CharaColliBase> collider);
/* Sets an attack collision's callback (see CityBombSmall.cpp). */
void fn_80096B64(CharaColliAttack *attack, int arg1, const AttackCallback &callback);
/* Whether the attack counts, and its attacker (behaviour inferred). */
bool fn_8015D4E4(AttackCallbackParam *param);
boost::shared_ptr<CharInfo> fn_8015D570(AttackCallbackParam &param);
/* Sends message to the character (behaviour and meaning of message inferred). */
int fn_8020642C(const boost::shared_ptr<CharInfo> &character, int message);
/* Whether the contacts of a collision involve a player (behaviour inferred). */
bool fn_8015D2EC(void *contacts);

/* Builds the character info of a gadget type, loads a model, and creates a target (behaviour
 * inferred). */
boost::shared_ptr<GadgetCharInfo> fn_8015CD90(int arg0, int arg1);
boost::shared_ptr<GadgetModelInfo> fn_8015F800(int arg0, const char *fileName);
boost::shared_ptr<Target> fn_801E72DC(int flags);
/* Plays motion index of a model (behaviour inferred). */
void fn_80160A84(GadgetModelInfo *model, int arg1, int index);
/* The motion work of a model, and two of its controls (behaviour inferred; see TurnOver.cpp). */
boost::weak_ptr<_stMtnPlayWork> fn_801602B4(GadgetModelInfo *model, int variant);
int fn_80416F28(_stMtnPlayWork *work, int arg1, int arg2, int arg3);
void fn_804165D8(_stMtnPlayWork *work, int arg1, int arg2, int arg3);
void fn_80416148(_stMtnPlayWork *work, int arg1, float frames);

/* The positioned object (see ElecBomb.cpp). */
boost::shared_ptr<UnkSound_802B4254> fn_802B4254(int kind, int arg1);
void fn_802B4498(UnkSound_802B4254 *sound, float range);
void fn_802B4510(UnkSound_802B4254 *sound, u8 flag);
void fn_802B44C8(UnkSound_802B4254 *sound, const Vec3f *position);
void fn_800CC978(boost::shared_ptr<UnkSound_802B4254> *sound);

/* Switches switch id on (behaviour inferred; see CityBombSmall.cpp). */
void fn_8016907C(UnkSwitchBank_800CB69C *manager, int id, int arg2);
/* Sets flag 0x8 of the owner's record (behaviour inferred). */
void fn_800C985C(UnkPlacement_800C9CD0 *owner);
/* The player manager and its entry index (behaviour inferred). */
UnkPlayerManager_8006D6A8 *fn_8006D6A8(void);
boost::weak_ptr<Mission::Mission> fn_800740E4(UnkPlayerManager_8006D6A8 *manager, int index);
/* Shows a hint (see HintRing.cpp). */
s32 fn_801FAF00(UnkHintManager_801FB180 *manager, s32 id, s32 branch, int arg3, int arg4);
/* The sound manager and its play call at a character info (behaviour inferred). */
void *fn_80053164(void);
void fn_801D54F4(void *manager, u16 id, GadgetCharInfo *charInfo, int arg3, int arg4);
/* destination = a + b (behaviour inferred). */
void fn_8040CDD0(Vec3f *destination, const Vec3f *a, const Vec3f *b);
}

/* The hint manager (see HintRing.cpp, which emits the copy the game keeps). */
UnkHintManager_801FB180 *GetHintManager();

/* A length (name inferred). As a call it is not a constant expression, so the constants below are
 * stored by a static initializer, as in retail. */
inline float Span(float length) {
    return length;
}

/* The height of the character info above the figure, the collider's radius, and the sound
 * source's height and range (names inferred). */
static float lbl_805EEEF4 = Span(4.0f);
static float lbl_805EEEF8 = Span(4.0f);
static float lbl_805EEEFC = Span(8.0f);
static float lbl_805EEF00 = Span(80.0f);

class Cream : public Gadget {
  public:
    Cream(Task *parent, UnkPlacement_800C9CD0 *owner);
    virtual ~Cream();
    virtual void Behave(float dt);
    virtual void ReloadPlacement(UnkPlacement_800C9CD0 *owner);

    /* Creates the figure under task layer 11 (name inferred). */
    static void Instantiate(UnkPlacement_800C9CD0 *owner);

    /* The attack collision's callback (name inferred). */
    void OnAttack(AttackCallbackParam &param);
    /* Plays motion index from its start (name inferred). */
    void PlayMotion(int index);
    /* Places the model, the collider, the target and the sound source (name inferred). */
    void UpdateModel();
    /* Copies the owner's position, rotation and switch id (name inferred). */
    void LoadFromOwner(UnkPlacement_800C9CD0 *owner);

    static const char *s_taskName;

    /* 0x30 */ Vec3f m_position;
    /* 0x3C */ boost::shared_ptr<GadgetCharInfo> m_chara;
    /* 0x44 */ boost::shared_ptr<Target> m_target;
    /* 0x4C */ boost::shared_ptr<UnkSound_802B4254> m_sound;
    /* 0x54 */ boost::shared_ptr<CharaColliBase> m_collider;
    /* 0x5C */ boost::shared_ptr<GadgetModelInfo> m_model;
    /* 0x64 */ CreamSetState *m_data;
    /* 0x68 */ s32 m_state; /* 0 waiting, 1 touched, 2 done, 3 hurt, 4 after the hurt motion */
    /* 0x6C */ u8 m_touching;
    /* 0x6D */ u8 m_switchId;
};
SIZE_ASSERT(Cream, 0x70);

const char *Cream::s_taskName = "Cream";

Cream::Cream(Task *parent, UnkPlacement_800C9CD0 *owner) : Gadget(parent, owner), m_touching(0) {
    m_name = s_taskName;
    if (owner->m_adapterState == 0) {
        fn_800C9930(owner, new CreamSetState());
    }
    m_data = dynamic_cast<CreamSetState *>(owner->m_adapterState);
    m_chara = fn_8015CD90(0x3B, 4);
    m_model = fn_8015F800(1, "CREAM.BIN");
    m_model->fn_80162A68(0, 0);
    m_collider = fn_800C7DEC(fn_80021B08(), boost::weak_ptr<CharInfo>(m_chara), 2, 7, 0, &lbl_805E07A0,
                             lbl_805EEEF8);

    boost::weak_ptr<CharaColliAttack> attack(
        fn_800C4374(fn_80021B08(), boost::shared_ptr<CharaColliBase>(m_collider)));
    boost::shared_ptr<CharaColliAttack> attackPtr(attack.lock());
    fn_80096B64(attackPtr.get(), 1, boost::bind(&Cream::OnAttack, this, _1));
    m_target = fn_801E72DC(0x2000);
    m_model->fn_80160C28(0);
    m_model->fn_8015FF60();
    if (!fn_80014E34(&m_data->m_flags.m_bits, 0)) {
        m_state = 0;
        fn_80160A84(m_model.get(), 0, 2);
        m_sound = fn_802B4254(2, 0);
        fn_802B4498(m_sound.px, lbl_805EEF00);
        fn_802B4510(m_sound.px, 1);
    } else {
        m_state = 2;
        fn_80160A84(m_model.get(), 0, 1);
    }
    LoadFromOwner(owner);
    UpdateModel();
}

Cream::~Cream() {}

extern "C" void fn_80134840(void) {
    fn_8006B474("CREAM.BIN", 1);
}

extern "C" void fn_8013483C(void) {}

void Cream::Instantiate(UnkPlacement_800C9CD0 *owner) {
    new Cream(fn_801EB304(fn_8001C4F8(), 11), owner);
}

void Cream::Behave(float dt) {
    if (fn_8015D2EC(reinterpret_cast<u8 *>(m_collider.get()) + 0xC)) {
        if (!m_touching) {
            if (m_state == 0 || m_state == 4) {
                if (fn_80014E34(&m_data->m_flags.m_bits, 0)) {
                    PlayMotion(1);
                    m_state = 2;
                } else {
                    PlayMotion(5);
                    m_state = 1;
                    fn_80014E1C(&m_data->m_flags.m_bits, 0);
                    {
                        boost::shared_ptr<Mission::Mission> player = fn_800740E4(fn_8006D6A8(), 2).lock();
                        if (player.get() != 0) {
                            player->AddCount(1, 0);
                            s32 kind;
                            player->GetState(&kind);
                            if (kind == 1) {
                                fn_801FAF00(GetHintManager(), 0x3D1, 0, 0, 0);
                            } else {
                                fn_801FAF00(GetHintManager(), 0x3D0, 0, 0, 0);
                            }
                        }
                    }
                    fn_800CC978(&m_sound);
                    fn_800C985C(m_owner);
                    fn_8016907C(fn_800CB69C(), m_switchId, 1);
                }
            }
            m_touching = 1;
        }
    } else {
        m_touching = 0;
    }

    switch (m_state) {
    case 3:
        if (fn_80416F28(fn_801602B4(m_model.get(), 0).lock().get(), 0, 3, 1)) {
            PlayMotion(3);
            m_state = 4;
        }
        break;
    case 1:
        if (fn_80416F28(fn_801602B4(m_model.get(), 0).lock().get(), 0, 3, 1)) {
            PlayMotion(1);
            m_state = 2;
        }
        break;
    }

    UpdateModel();
    m_model->fn_80162FB0(dt);
}

void Cream::ReloadPlacement(UnkPlacement_800C9CD0 *owner) {
    LoadFromOwner(owner);
    UpdateModel();
    m_model->fn_80162EF4();
}

void Cream::LoadFromOwner(UnkPlacement_800C9CD0 *owner) {
    fn_800C9F14(owner, &m_position);
    fn_800C9ED4(owner, &m_chara->m_rotation);
    fn_800C9E60(owner, &m_switchId);
    fn_800091D0(&m_chara->m_position, &m_position);
    m_chara->m_position.y += lbl_805EEEF4;
}

void Cream::UpdateModel() {
    Mtx34f matrix;

    fn_8040C148(&matrix, &lbl_805E0720);
    fn_8040B59C(&matrix, &m_position);
    fn_8040B848(&matrix, m_chara->m_rotation.y);
    m_model->fn_801633DC(&matrix);
    m_collider->SetCharInfo(m_chara.get());
    m_target->fn_800CCE38(&m_chara->m_position);
    if (m_sound.get() != 0) {
        Vec3f offset(0.0f, lbl_805EEEFC, 0.0f);
        Vec3f position;

        fn_8040CDD0(&position, &offset, &m_chara->m_position);
        fn_802B44C8(m_sound.px, &position);
    }
}

void Cream::PlayMotion(int index) {
    boost::shared_ptr<_stMtnPlayWork> work(fn_801602B4(m_model.get(), 0).lock());
    if (work.get() != 0) {
        fn_804165D8(work.get(), 0, -1, -1);
        fn_80416148(work.get(), 0, 15.0f);
    }
    fn_80160A84(m_model.get(), 0, index);
}

void Cream::OnAttack(AttackCallbackParam &param) {
    if (fn_8015D4E4(&param) && m_state != 4 && m_state != 3) {
        param.m_result = 1;
        m_state = 3;
        fn_80160A84(m_model.get(), 0, 4);
        fn_801D54F4(fn_80053164(), 0x7446, m_chara.get(), 0, 1);
        if (!fn_80014E34(&m_data->m_flags.m_bits, 1)) {
            {
                boost::shared_ptr<CharInfo> attacker(fn_8015D570(param));
                if (attacker.get() != 0) {
                    fn_8020642C(attacker, 0x2E);
                }
            }
            fn_80014E1C(&m_data->m_flags.m_bits, 1);
        }
    } else {
        param.m_result = 8;
    }
}

GadgetClassDesc lbl_80528AB0 = {
    "Cream", fn_80134840, fn_8013483C, Cream::Instantiate, 0, 0, 0xBBD, 1, 0, 10, 0,
};
