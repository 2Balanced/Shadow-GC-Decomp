/* Player::Npc::Tails::Tail (class name from RTTI): the task that draws Tails's tails, a weapon-style
 * model the NPC's code shows, hides and animates. It keeps its state in a separately allocated
 * object (class name unknown) that holds the NPC, the model and the matrix of the skeleton node the
 * model follows. The file name, the function and member names and the helper's class name are
 * inferred. Definitions are in reverse address order (deferred inlining emits functions last-defined
 * first). */
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
namespace Tails {

/* The tail's state (class name unknown). */
class UnkTail_8032D0FC {
  public:
    UnkTail_8032D0FC(const boost::shared_ptr<Npc> &npc, void *arg);
    ~UnkTail_8032D0FC();
    void Tick(float dt);
    /* Creates the model (name inferred). */
    void CreateModel(void *arg);
    /* Places the model on the NPC's node and animates it (name inferred). */
    void UpdateModel(float dt);

    /* 0x0 */ boost::shared_ptr<Npc> m_npc;
    /* 0x8 */ UnkWeaponModel_80063428 *m_model;
    /* 0xC */ const Mtx34f *m_node;
};

class Tail : public Task {
  public:
    Tail(Task *parent, const boost::shared_ptr<Npc> &npc, void *arg);
    virtual ~Tail();
    virtual void Tick(float dt);
    void fn_8032CDD8(s32 arg);
    /* Sets the model's state (see UnkWeaponModel_80063428::fn_80063258). */
    void SetState(int state);
    void Show();
    void Hide();

    /* 0x28 */ boost::scoped_ptr<UnkTail_8032D0FC> m_state;
    /* 0x2C */ u32 pad_0x2C; /* the class size is a multiple of 8 */
};

/* Deletes the tail's state (name inferred; see checked_delete below). */
inline void DeleteTailState(UnkTail_8032D0FC *state) {
    delete state;
}

} // namespace Tails
} // namespace Npc
} // namespace Player

/* Retail keeps boost::checked_delete<UnkTail_8032D0FC> out of line (0x8032CEB4) and the state's
 * destructor at its own place; this specialization gives that order under GC/2.7 (see
 * docs/research-notes.md and Game/EnergyCore.cpp). */
namespace boost {
template<> inline void checked_delete<Player::Npc::Tails::UnkTail_8032D0FC>(
    Player::Npc::Tails::UnkTail_8032D0FC *x) {
    Player::Npc::Tails::DeleteTailState(x);
}
} // namespace boost

SIZE_ASSERT(Player::Npc::Tails::UnkTail_8032D0FC, 0x10);
SIZE_ASSERT(Player::Npc::Tails::Tail, 0x30);

static const UnkWeaponEffectSub_80063428 s_effectSubs[] = {
    {0xB, 0x1F, 0x0},  {0xB, 0x16, 0x1},  {0xB, 0x17, 0x2},  {0xB, 0x18, 0x3},  {0xB, 0x19, 0x4},
    {0xB, 0x1A, 0x5},  {0xB, 0x1B, 0x6},  {0xB, 0x1C, 0x7},  {0xB, 0x1D, 0x8},  {0xB, 0x1E, 0x9},
    {0xB, 0x20, 0xA},  {0xB, 0x21, 0xB},  {0xB, 0x22, 0xC},  {0xB, 0x23, 0xD},  {0xB, 0x24, 0xE},
    {0xB, 0x25, 0xF},  {0xB, 0x2A, 0x10}, {0xB, 0x27, 0x11}, {0xB, 0x26, 0x12}, {0xB, 0x28, 0x13},
    {0xB, 0x29, 0x14},
};

static const UnkWeaponEffect_80063428 s_effects[] = {
    {-1, -1, 3, 10, s_effectSubs, 21},
};

static const UnkWeaponEffectSet_80063428 s_effectSet = {s_effects, 1, 0, 0};

namespace Player {
namespace Npc {
namespace Tails {

UnkTail_8032D0FC::UnkTail_8032D0FC(const boost::shared_ptr<Npc> &npc, void *arg)
    : m_npc(npc), m_model(0) {
    CreateModel(arg);
    m_node = fn_800AA760(((UnkNpcFields_8032D0FC *)m_npc.get())->m_skeleton, 0xC)->m_matrix;
}

UnkTail_8032D0FC::~UnkTail_8032D0FC() {
    if (m_model) {
        delete m_model;
        m_model = 0;
    }
}

void UnkTail_8032D0FC::Tick(float dt) {
    UpdateModel(dt);
}

void UnkTail_8032D0FC::CreateModel(void *arg) {
    m_model = new UnkWeaponModel_80063428(fn_80078C4C(m_npc.get()), &s_effectSet);
    if (m_model) {
        m_model->fn_8006310C(arg);
    }
}

void UnkTail_8032D0FC::UpdateModel(float dt) {
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

Tail::Tail(Task *parent, const boost::shared_ptr<Npc> &npc, void *arg)
    : Task(parent), m_state(new UnkTail_8032D0FC(npc, arg)) {
    m_name = "Tail";
}

Tail::~Tail() {}

void Tail::Tick(float dt) {
    m_state->Tick(dt);
}

void Tail::fn_8032CDD8(s32 arg) {}

void Tail::SetState(int state) {
    if (m_state->m_model) {
        m_state->m_model->fn_80063258(state);
        m_state->m_model->fn_8006327C();
    }
}

void Tail::Show() {
    if (m_state->m_model) {
        m_state->m_model->fn_80063130();
    }
}

void Tail::Hide() {
    if (m_state->m_model) {
        m_state->m_model->fn_80063154();
    }
}

} // namespace Tails
} // namespace Npc
} // namespace Player
