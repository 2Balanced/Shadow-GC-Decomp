/* BombServer (class name from RTTI): the BombServer gadget, a dispenser (BOMBSERVER.BIN) that
 * hands out a bomb or a healing unit ("Type 0:Bomb 1:HealUnit"). A command of type 0xD arms it:
 * once no player (nor a character of kind 0x4E or 0xE) has touched its collider for four seconds,
 * it starts its particles and a sound, a second later spawns the item for its owner, and half a
 * second after that rearms. File name inferred from the class. Method names marked "(name
 * inferred)" are not original. Definitions are in reverse address order (deferred inlining emits
 * functions last-defined first). */
#include "Game/CharCommand.h"
#include "Game/CharCommandUnit.h"
#include "Game/CharInfo.h"
#include "Game/CharaColli.h"
#include "Game/Gadget.h"
#include "Game/GadgetCharInfo.h"
#include "Game/GadgetCollision.h"
#include "Game/GadgetModelInfo.h"
#include "Game/GadgetParticle.h"
#include "Game/GadgetClassDesc.h"
#include "Game/FileQueue.h"
#include "boost/bind.hpp"
#include "boost/function.hpp"
#include "boost/shared_ptr.hpp"
#include "boost/weak_ptr.hpp"
#include "size_assert.h"

using SonicteamUSA::System::CharaColli::CharaColliBase;

/* A node of a collider's contact list (see BlockChange.cpp). */
struct UnkColliContactNode_800587A0;

/* An iterator over a contact list (layout from fn_8001EDFC; name inferred). */
struct UnkColliContactIter_8001EDFC {
    UnkColliContactIter_8001EDFC(UnkColliContactNode_800587A0 *node) : m_node(node) {}

    UnkColliContactNode_800587A0 *m_node;
};

namespace Bomb {
/* Bomb (class name from RTTI; see Bomb.cpp). */
class Bomb {
  public:
    /* Spawns a bomb for owner at position, finished once out of range (name inferred). */
    static void Spawn(const Vec3f &position, const Vec3f &rotation,
                      const boost::weak_ptr<CharInfo> &owner, float range);
};
} // namespace Bomb

namespace HealingUnit {
/* HealingUnit (class name from RTTI; see HealingUnit.cpp). */
class HealingUnit {
  public:
    /* Spawns a healing unit for owner at position, finished once out of range (name inferred). */
    static void Spawn(const Vec3f &position, const Vec3f &rotation,
                      const boost::weak_ptr<CharInfo> &owner, float range);
};
} // namespace HealingUnit

extern "C" {
/* Gives the character info a command handler unless it has one (behaviour inferred). */
void fn_8015CF7C(GadgetCharInfo *charInfo);
/* The range the owner was placed with (behaviour inferred; see Coaster.cpp). */
float fn_800C9938(UnkPlacement_800C9CD0 *owner);

/* Builds the character info of a gadget type and loads a gadget model (see GadgetUtil.h). */
boost::shared_ptr<GadgetCharInfo> fn_8015CD90(int arg0, int arg1);
boost::shared_ptr<GadgetModelInfo> fn_8015F800(int arg0, const char *fileName);
/* Calls callback with param for each part of the model (behaviour inferred). */
void fn_8015FA78(GadgetModelInfo *model,
                 void *(*callback)(void *part, UnkParticleAttach_80169D00 *param),
                 UnkParticleAttach_80169D00 *param);
void *fn_80169D00(void *part, UnkParticleAttach_80169D00 *param);

/* Particle controller helpers (behaviour inferred). */
boost::shared_ptr<GadgetParticle::GadgetParticleCtrl> fn_80166AF4(void);
void fn_80167180(GadgetParticle::GadgetParticleCtrl *ctrl, int id);
void fn_80167448(GadgetParticle::GadgetParticleCtrl *ctrl, int id);
void fn_8016761C(GadgetParticle::GadgetParticleCtrl *ctrl, float dt);

/* The collider manager, a sphere collider for charInfo and the character info a collider belongs
 * to (behaviour inferred; see ChaoHolder.cpp and BlockChange.cpp). */
void *fn_80021B08(void);
boost::shared_ptr<CharaColliBase> fn_800C7DEC(void *manager, const boost::weak_ptr<CharInfo> &charInfo,
                                              int flags, int arg3, int arg4, const Vec3f *offset,
                                              float radius);
boost::weak_ptr<CharInfo> fn_800C4568(void *manager,
                                      const boost::weak_ptr<CharaColliBase> &collider);
/* The contact list: its first and end nodes, the iterator's post-increment, and the collider of a
 * node (see BlockChange.cpp). */
UnkColliContactNode_800587A0 *fn_8001EE04(UnkColliContacts_8001EE04 *contacts);
UnkColliContactNode_800587A0 *fn_8001EDEC(UnkColliContacts_8001EE04 *contacts);
UnkColliContactIter_8001EDFC fn_80027374(UnkColliContactIter_8001EDFC *it, int);
boost::weak_ptr<CharaColliBase> fn_800587A0(UnkColliContactNode_800587A0 *node);

/* The sound manager and its positioned play call (behaviour inferred). */
void *fn_80053164(void);
void fn_801D54F4(void *manager, int id, GadgetCharInfo *source, int arg3, int arg4);
}

/* The kind of a character (offset from its use here; meaning unknown). */
inline s32 GetKind(CharInfo *info) {
    return *(s32 *)&info->field_0x3C[0x50 - 0x3C];
}

/* The parameter of the owner record (name from the parameter table). */
struct BombServerParams {
    /* 0x0 */ u32 type; /* 0 bomb, 1 healing unit */
};

class BombServer : public Gadget {
  public:
    BombServer(Task *parent, UnkPlacement_800C9CD0 *owner);
    virtual ~BombServer();
    virtual void Behave(float dt);
    virtual void ReloadPlacement(UnkPlacement_800C9CD0 *owner);

    /* Creates the gadget under task layer 11 (name inferred). */
    static void Instantiate(UnkPlacement_800C9CD0 *owner);

    /* Copies the owner's placement and parameter and resets the timer (name inferred). */
    void ReadPlacement(UnkPlacement_800C9CD0 *owner);
    /* Shows the model of the type with its particles and builds the collider (name inferred). */
    void SetupModel();
    /* Moves the model to the character info's place (name inferred). */
    void UpdatePosition();
    /* The command unit's callback (name inferred). */
    void OnCommand(CharCommand &command);

    static const char *s_taskName;

    /* Member names are inferred. */
    /* 0x30 */ boost::shared_ptr<GadgetCharInfo> m_chara;
    /* 0x38 */ boost::shared_ptr<GadgetModelInfo> m_model;
    /* 0x40 */ boost::shared_ptr<GadgetParticle::GadgetParticleCtrl> m_particle;
    /* 0x48 */ boost::shared_ptr<CharaColliBase> m_collider;
    /* 0x50 */ BombServerParams m_params;
    /* 0x54 */ s32 m_state; /* 0 idle, 1 armed, 2 dispensing, 3 rearming */
    /* 0x58 */ float m_timer;
    /* 0x5C */ u32 pad_0x5C; /* the class size is a multiple of 8 */
};
SIZE_ASSERT(BombServer, 0x60);

const char *BombServer::s_taskName = "BombServer";

/* Converts the parameters' metres to world units (name inferred). */
inline float MetresToUnits(float metres) {
    return 10.0f * metres;
}

/* The radius of the collider. A constant with a dynamic initializer, so it lives in .sbss and the
 * static initializer (0x80316000) stores it. */
static const float lbl_805F0A4C = MetresToUnits(2.0f);

BombServer::BombServer(Task *parent, UnkPlacement_800C9CD0 *owner)
    : Gadget(parent, owner), m_state(1) {
    m_name = s_taskName;
    m_chara = fn_8015CD90(0x5D, 4);
    fn_8015CF7C(m_chara.get());
    fn_800729B0(&m_chara->m_commandUnit->m_callback,
                &(const CharCommandCallback &)CharCommandCallback(
                    boost::bind(&BombServer::OnCommand, this, _1)));
    ReadPlacement(owner);
    m_model = fn_8015F800(1, "BOMBSERVER.BIN");
    SetupModel();
}

BombServer::~BombServer() {}

extern "C" void fn_803158AC(void) {
    fn_8006B474("BOMBSERVER.BIN", 1);
}

extern "C" void fn_803158A8(void) {}

void BombServer::Instantiate(UnkPlacement_800C9CD0 *owner) {
    new BombServer(fn_801EB304(fn_8001C4F8(), 11), owner);
}

void BombServer::Behave(float dt) {
    switch (m_state) {
    case 1: {
        UnkColliContacts_8001EE04 *contacts;
        bool clear = true;
        contacts = reinterpret_cast<UnkColliContacts_8001EE04 *>(
            reinterpret_cast<u8 *>(m_collider.get()) + 0xC);
        if (contacts != 0) {
            for (UnkColliContactIter_8001EDFC it(fn_8001EE04(contacts));
                 it.m_node != fn_8001EDEC(contacts); fn_80027374(&it, 0)) {
                boost::shared_ptr<CharInfo> hit =
                    fn_800C4568(fn_80021B08(), fn_800587A0(it.m_node)).lock();
                if (hit.get() != 0) {
                    if (hit->vf_0x18()) {
                        clear = false;
                        break;
                    }
                    if (GetKind(hit.get()) == 0x4E || GetKind(hit.get()) == 0xE) {
                        clear = false;
                        break;
                    }
                }
            }
        }
        if (clear) {
            m_timer -= dt;
            if (m_timer < 0.0f) {
                m_collider->SetAttributes(0x10302, -1);
                fn_80167448(m_particle.get(), 1);
                fn_801D54F4(fn_80053164(), 0x764F, m_chara.get(), 0, 1);
                m_timer = 1.0f;
                m_state = 2;
            }
        }
        break;
    }
    case 2:
        m_timer -= dt;
        if (m_timer < 0.0f) {
            m_collider->TurnOff(-1);
            if (m_params.type == 0) {
                Bomb::Bomb::Spawn(m_chara->m_position, m_chara->m_rotation, m_chara,
                                  fn_800C9938(m_owner));
                fn_801D54F4(fn_80053164(), 0x7632, m_chara.get(), 0, 1);
            } else {
                HealingUnit::HealingUnit::Spawn(m_chara->m_position, m_chara->m_rotation,
                                                m_chara, fn_800C9938(m_owner));
                fn_801D54F4(fn_80053164(), 0x7633, m_chara.get(), 0, 1);
            }
            m_timer = 0.5f;
            m_state = 3;
        }
        break;
    case 3:
        m_timer -= dt;
        if (m_timer < 0.0f) {
            fn_801D54F4(fn_80053164(), 0x7634, m_chara.get(), 0, 1);
            m_state = 0;
        }
        break;
    }
    m_model->fn_80162FB0(dt);
    if (m_particle.get() != 0) {
        fn_8016761C(m_particle.get(), dt);
    }
}

void BombServer::ReloadPlacement(UnkPlacement_800C9CD0 *owner) {
    BombServerParams params = m_params;

    ReadPlacement(owner);
    if (m_params.type != params.type) {
        m_model->fn_80162804();
        m_state = 1;
        m_timer = 0.0f;
        SetupModel();
    }
    UpdatePosition();
    m_model->fn_80162EF4();
    m_model->fn_8016108C();
}

void BombServer::ReadPlacement(UnkPlacement_800C9CD0 *owner) {
    fn_800C9F14(owner, &m_chara->m_position);
    fn_800C9ED4(owner, &m_chara->m_rotation);
    fn_800C9DBC(owner, &m_params, sizeof(m_params));
    m_timer = 0.0f;
}

void BombServer::SetupModel() {
    m_model->fn_80162A68(m_params.type, 0);
    UpdatePosition();
    m_particle = fn_80166AF4();
    {
        UnkParticleAttach_80169D00 attach;
        attach.ctrl = m_particle;
        fn_8015FA78(m_model.get(), fn_80169D00, &attach);
    }
    fn_80167180(m_particle.get(), 2);
    m_collider = fn_800C7DEC(fn_80021B08(), m_chara, 0x50100, 8, 0, &lbl_805E07A0, lbl_805F0A4C);
    if (m_state != 1) {
        m_collider->TurnOff(-1);
    }
}

void BombServer::UpdatePosition() {
    Mtx34f matrix;

    fn_8040C148(&matrix, &lbl_805E0720);
    fn_8040B59C(&matrix, &m_chara->m_position);
    fn_8040B848(&matrix, m_chara->m_rotation.y);
    m_model->fn_801633DC(&matrix);
}

void BombServer::OnCommand(CharCommand &command) {
    switch (command.TypeId()) {
    case 0xD:
        if (m_state == 0 || m_state == 3) {
            m_state = 1;
            m_collider->TurnOn(-1);
            m_collider->SetAttributes(0x50100, -1);
            m_timer = 4.0f;
        }
        break;
    }
    command.field_0x4[3] |= 2;
}

static GadgetParamDesc lbl_8055BF64[] = {
    {2, "Type 0:Bomb 1:HealUnit", 0, 0, 1, 0.0f, 0.0f, 0.0f, fn_800CBC20, 0},
    {0},
};

GadgetClassDesc lbl_8055BFB4 = {
    "BombServer", fn_803158AC, fn_803158A8, BombServer::Instantiate, 0, 0, 0x1006, 1, 0, 10, lbl_8055BF64,
};
