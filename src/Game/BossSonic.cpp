/* Player::Npc::BossSonic::BossSonic (class name from RTTI): Boss Sonic, the NPC the player fights
 * (named "BossSonic"). fn_80293C98 creates it for the Diablon stage's set-up
 * (Game/DiablonSetData.cpp): it registers the NPC, switches its control to 2 and returns it. Its
 * setup (slot 0x30) builds the NPC's tasks below a task of its own: the controller, the sound,
 * the executer (Game/BossSonicExecuter.cpp), the motion, the attack and the effect, then its aim
 * (Game/BossSonicTarget.cpp) and its control and behavior factories
 * (Game/BossSonicSonicTypeControl.cpp, Game/SonicTypeBehaviorFactory.cpp). The AI's state
 * (BossSonicWork, Game/BossSonic.h) is made with the NPC. File name from the class. Function and
 * member names are inferred. Definitions are in reverse address order (deferred inlining emits
 * functions last-defined first). */
#include "Game/BossSonic.h"
#include "Game/BossSonicTarget.h"
#include "Game/Npc.h"
#include "Game/Player.h"
#include "Game/PlayerControl.h"
#include "Game/PlayerFactory.h"
#include "Game/Task.h"
#include "boost/shared_ptr.hpp"
#include "boost/weak_ptr.hpp"
#include "size_assert.h"

using boost::shared_ptr;
using boost::weak_ptr;
using Player::PlayerBase;
using Player::Npc::Npc;
using Player::Npc::BossSonic::BossSonicWork;

struct UnkCharManager_8006F754;
struct UnkNpcRecord_8006F8D0;

namespace Player {

/* Controller, Attack and Effect (class names from RTTI): tasks of a player character. Only their
 * constructors (0x8009ED38, 0x80077B1C, 0x800A2EC8) are called here. */
class Controller : public Task {
  public:
    Controller(Task *parent, const shared_ptr<PlayerBase> &player);

    /* 0x28 */ u8 field_0x28[0x30 - 0x28];
};
SIZE_ASSERT(Controller, 0x30);

class Attack : public Task {
  public:
    Attack(Task *parent, const shared_ptr<PlayerBase> &player);

    /* 0x28 */ u8 field_0x28[0x30 - 0x28];
};
SIZE_ASSERT(Attack, 0x30);

class Effect : public Task {
  public:
    Effect(Task *parent, const shared_ptr<PlayerBase> &player);

    /* 0x28 */ u8 field_0x28[0x30 - 0x28];
};
SIZE_ASSERT(Effect, 0x30);

namespace Npc {
namespace BossSonic {

/* The executer's factory function (Game/BossSonicExecuter.cpp). */
class BossSonicExecuter {
  public:
    static BossSonicExecuter *Create(Task *parent, const shared_ptr<Npc> &npc,
                                     const shared_ptr<BossSonicWork> &work);
};

} // namespace BossSonic
} // namespace Npc
} // namespace Player

/* What a player keeps at 0x248 (class unknown): a pointer to a 0x2C-byte object made for the
 * player (constructor 0x80095BD4). */
struct UnkPlayerPart_80095BD4 {
    UnkPlayerPart_80095BD4(const shared_ptr<PlayerBase> &player);

    /* 0x0 */ void *m_object;
};
SIZE_ASSERT(UnkPlayerPart_80095BD4, 0x4);

extern "C" {
void *fn_8001C4F8(void);
Task *fn_801EB304(void *root, int layer);
/* Task makers of a character (behaviour inferred from their sizes; fn_802DB7CC is Sonic's sound,
 * Game/PlayerShadowSound.cpp, and fn_8006C0B0 Sonic's motion). */
Task *fn_80075E30(Task *parent, const shared_ptr<Npc> &npc);
Task *fn_800B8310(Task *parent, const weak_ptr<PlayerBase> &player);
void fn_80094088(Task *parent, const shared_ptr<PlayerBase> &player);
void fn_8006C0B0(Task *parent, const shared_ptr<Npc> &npc);
Task *fn_802DB7CC(Task *parent, const shared_ptr<PlayerBase> &player);
/* Hands the controller its control factory and the executer its behavior factory. */
void fn_8009EBC8(Task *controller, const shared_ptr<Player::Control::Factory> &factory);
void fn_800A8078(Player::Behavior::UnkBehaviorExecuter_800A7F14 *behaviors,
                 const shared_ptr<Player::Behavior::Factory> &factory);
/* Boss Sonic's factories (Game/BossSonicSonicTypeControl.cpp, Game/SonicTypeBehaviorFactory.cpp). */
shared_ptr<Player::Control::Factory> fn_802951EC(const shared_ptr<Npc> &npc,
                                                 const shared_ptr<BossSonicWork> &work);
shared_ptr<Player::Behavior::Factory> fn_80295DE4(const shared_ptr<Npc> &npc);
/* NPC helpers (behaviour inferred; fn_80073770, fn_80073820 and fn_80073838 set three of its
 * ranges, see Game/SetCharacter.cpp). */
void fn_80073820(Npc *npc, float range);
void fn_80073838(Npc *npc, float range);
void fn_8007398C(Npc *npc);
void fn_80073770(Npc *npc, float range);
/* The NPC manager: registers an NPC (behaviour inferred). */
UnkCharManager_8006F754 *fn_8006F754(void);
void fn_80074D94(UnkCharManager_8006F754 *manager, const shared_ptr<Npc> &npc);
/* Switches the control of the character's controller (see Game/BossSonicControlRestart.cpp). */
void fn_8009EBA4(Task *controller, const Player::Control::InitializeArgs *args);
/* An NPC's record and a setting of it (behaviour unknown). */
UnkNpcRecord_8006F8D0 *fn_8006F8D0(Npc *npc);
void fn_800A871C(UnkNpcRecord_8006F8D0 *record, int arg);
}

/* A value (name inferred). As a call it is not a constant expression, so the constants below are
 * stored by the static initializer, as in retail (as in Game/SecretDisk.cpp). */
inline float Value(float value) {
    return value;
}

/* The NPC's name, which is also its root task's (name inferred). As an array defined before the
 * class it comes first in .rodata, ahead of the RTTI names, as in retail. */
static const char s_name[] = "BossSonic";

/* The two ranges the constructor sets (names inferred). They are const: the constructor loads the
 * first before its store to 0x5C, as in retail. */
static const float s_value274 = Value(5.0f);
static const float s_range = Value(13000.0f);

/* Sets the NPC's range at 0x274 and passes it on (meaning inferred; the value stays in the
 * argument register for both). */
inline void SetRange(Npc *npc, float range) {
    npc->field_0x274 = range;
    fn_80073820(npc, range);
}

namespace Player {
namespace Npc {
namespace BossSonic {

class BossSonic : public Npc {
  public:
    BossSonic();
    virtual ~BossSonic();
    virtual bool vf_0x18();
    virtual bool vf_0x1C();
    virtual bool vf_0x20();
    virtual void vf_0x30();

    /* Makes Boss Sonic and its AI's state; player is the character it fights. */
    static shared_ptr<Npc> Create(const shared_ptr<CharInfo> &player);
    void Init(const shared_ptr<BossSonic> &self, const shared_ptr<CharInfo> &player);

    /* 0x294 */ shared_ptr<BossSonicWork> m_work;
};
SIZE_ASSERT(BossSonic, 0x29C);

shared_ptr<Npc> BossSonic::Create(const shared_ptr<CharInfo> &player) {
    shared_ptr<BossSonic> boss = shared_ptr<BossSonic>(new BossSonic);
    boss->m_self = boss;
    boss->Init(boss, player);
    return boss;
}

BossSonic::BossSonic() : Npc(3, 0xD, s_name, 2) {
    field_0x5C = 3;
    SetRange(this, s_value274);
    fn_80073838(this, s_range);
}

BossSonic::~BossSonic() {}

void BossSonic::Init(const shared_ptr<BossSonic> &self, const shared_ptr<CharInfo> &player) {
    m_work = shared_ptr<BossSonicWork>(new BossSonicWork(self));
    m_work->m_player = player;
    vf_0x30();
}

void BossSonic::vf_0x30() {
    shared_ptr<Npc> npc = boost::dynamic_pointer_cast<Npc>(m_self.lock());
    m_layerTask = fn_80075E30(fn_801EB304(fn_8001C4F8(), 7), npc);
    m_rootTask = new Task(fn_801EB304(fn_8001C4F8(), 13));
    m_rootTask->m_name = s_name;
    fn_800B8310(m_layerTask, m_self);
    PlayerBase::vf_0x30();
    m_target = new BossSonicTarget(npc);
    new Controller(m_layerTask, npc);
    fn_80094088(m_rootTask, npc);
    BossSonicExecuter::Create(m_rootTask, npc, m_work);
    fn_8006C0B0(m_rootTask, npc);
    new Attack(m_rootTask, npc);
    new Effect(m_rootTask, npc);
    fn_802DB7CC(m_rootTask, npc);
    *reinterpret_cast<UnkPlayerPart_80095BD4 **>(field_0x244 + (0x248 - 0x244)) =
        new UnkPlayerPart_80095BD4(npc);
    fn_8009EBC8(m_task, fn_802951EC(npc, m_work));
    fn_800A8078(m_behaviors, fn_80295DE4(npc));
    fn_8007398C(this);
    fn_80073770(this, 0.0f);
}

} // namespace BossSonic
} // namespace Npc
} // namespace Player

using Player::Npc::BossSonic::BossSonic;

/* Creates Boss Sonic to fight player, registers it with the NPCs and starts its control 2 (name
 * inferred). */
extern "C" weak_ptr<CharInfo> fn_80293C98(shared_ptr<CharInfo> player) {
    shared_ptr<Npc> npc = BossSonic::Create(player);
    if (npc) {
        fn_80074D94(fn_8006F754(), npc);
        fn_8009EBA4(npc->m_task, &Player::Control::InitializeArgs(2));
        fn_800A871C(fn_8006F8D0(npc.get()), 1);
    }
    return npc;
}

namespace Player {
namespace Npc {
namespace BossSonic {

bool BossSonic::vf_0x20() {
    return false;
}

bool BossSonic::vf_0x1C() {
    return false;
}

bool BossSonic::vf_0x18() {
    return false;
}

} // namespace BossSonic
} // namespace Npc
} // namespace Player
