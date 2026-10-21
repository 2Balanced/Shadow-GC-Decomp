/* Player::Npc::Bee::Feather (class name from RTTI): the task that draws a bee NPC's feathers, a
 * weapon-style model the NPC's code shows and hides. It is built like Player::Npc::Tails::Tail
 * (Game/NpcTailsTail.cpp): the state lives in a separately allocated object (class name unknown)
 * holding the NPC, the model and the matrix of the skeleton node the model follows. The file name,
 * the function and member names and the helper's class name are inferred. Definitions are in reverse
 * address order (deferred inlining emits functions last-defined first). */
#include "Game/Npc.h"
#include "Game/NpcPartModel.h"
#include "Game/Task.h"
#include "Game/WeaponBase.h"
#include "Game/MathTypes.h"
#include "boost/scoped_ptr.hpp"
#include "boost/shared_ptr.hpp"
#include "size_assert.h"

using Weapon::UnkWeaponEffect_80063428;
using Weapon::UnkWeaponEffectSet_80063428;
using Weapon::UnkWeaponEffectSub_80063428;
using Weapon::UnkWeaponModel_80063428;

extern "C" {
/* The product of two matrices (behaviour inferred; see Game/ArkCannon.cpp). */
Mtx34f fn_8001E044(const Mtx34f *a, const Mtx34f *b);
}

namespace Player {
namespace Npc {
namespace Bee {

/* The feathers' state (class name unknown). */
class UnkFeather_8032D4C0 {
  public:
    UnkFeather_8032D4C0(const boost::shared_ptr<Npc> &npc);
    ~UnkFeather_8032D4C0();
    void Tick(float dt);
    /* Creates the model (name inferred). */
    void CreateModel();
    /* Places the model on the NPC's node and animates it (name inferred). */
    void UpdateModel(float dt);

    /* 0x0 */ boost::shared_ptr<Npc> m_npc;
    /* 0x8 */ UnkWeaponModel_80063428 *m_model;
    /* 0xC */ const Mtx34f *m_node;
};

class Feather : public Task {
  public:
    Feather(Task *parent, const boost::shared_ptr<Npc> &npc);
    virtual ~Feather();
    virtual void Tick(float dt);
    void fn_8032D1BC(s32 arg);
    void Show();
    void Hide();

    /* 0x28 */ boost::scoped_ptr<UnkFeather_8032D4C0> m_state;
    /* 0x2C */ u32 pad_0x2C; /* the class size is a multiple of 8 */
};

/* Deletes the feathers' state (name inferred; see checked_delete below). */
inline void DeleteFeatherState(UnkFeather_8032D4C0 *state) {
    delete state;
}

} // namespace Bee
} // namespace Npc
} // namespace Player

/* Retail keeps boost::checked_delete<UnkFeather_8032D4C0> out of line (0x8032D298) and the state's
 * destructor at its own place; this specialization gives that order under GC/2.7 (see
 * docs/research-notes.md and Game/EnergyCore.cpp). */
namespace boost {
template<> inline void checked_delete<Player::Npc::Bee::UnkFeather_8032D4C0>(
    Player::Npc::Bee::UnkFeather_8032D4C0 *x) {
    Player::Npc::Bee::DeleteFeatherState(x);
}
} // namespace boost

SIZE_ASSERT(Player::Npc::Bee::UnkFeather_8032D4C0, 0x10);
SIZE_ASSERT(Player::Npc::Bee::Feather, 0x30);

static const UnkWeaponEffectSub_80063428 s_effectSubs[] = {
    {0xB, 0xE, 0x0},
};

static const UnkWeaponEffect_80063428 s_effects[] = {
    {-1, -1, 3, 10, s_effectSubs, 1},
};

static const UnkWeaponEffectSet_80063428 s_effectSet = {s_effects, 1, 0, 0};

namespace Player {
namespace Npc {
namespace Bee {

UnkFeather_8032D4C0::UnkFeather_8032D4C0(const boost::shared_ptr<Npc> &npc) : m_npc(npc), m_model(0) {
    CreateModel();
    m_node = fn_800AA760(((UnkNpcFields_8032D0FC *)m_npc.get())->m_skeleton, 0xC)->m_matrix;
}

UnkFeather_8032D4C0::~UnkFeather_8032D4C0() {
    if (m_model) {
        delete m_model;
        m_model = 0;
    }
}

void UnkFeather_8032D4C0::Tick(float dt) {
    UpdateModel(dt);
}

void UnkFeather_8032D4C0::CreateModel() {
    m_model = new UnkWeaponModel_80063428(fn_80078C4C(m_npc.get()), &s_effectSet);
}

void UnkFeather_8032D4C0::UpdateModel(float dt) {
    if (m_model) {
        Mtx34f matrix;
        if (fn_80073908(m_npc.get()) < 1.0f) {
            fn_8040C148(&matrix, &(const Mtx34f &)fn_8001E044(
                                     &(const Mtx34f &)fn_80073844(fn_8006CA70(&m_npc)), m_node));
        } else {
            fn_8040C148(&matrix, m_node);
        }
        m_model->fn_800632C4(&matrix);
        m_model->fn_800632A0(dt);
    }
}

Feather::Feather(Task *parent, const boost::shared_ptr<Npc> &npc)
    : Task(parent), m_state(new UnkFeather_8032D4C0(npc)) {
    m_name = "Feather";
}

Feather::~Feather() {}

void Feather::Tick(float dt) {
    m_state->Tick(dt);
}

void Feather::fn_8032D1BC(s32 arg) {}

void Feather::Show() {
    if (m_state->m_model) {
        m_state->m_model->fn_80063130();
    }
}

void Feather::Hide() {
    if (m_state->m_model) {
        m_state->m_model->fn_80063154();
    }
}

} // namespace Bee
} // namespace Npc
} // namespace Player
