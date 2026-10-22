/* Player::Npc::BossSonic::BossSonicWork (class name from the RTTI of its shared_ptr control block;
 * see Game/BossSonic.h): the state of Boss Sonic's AI, and the mode requests the desires make.
 * The stage picks the AI's tuning. File, member and function names are inferred. Definitions are
 * in reverse address order (deferred inlining emits functions last-defined first). */
#include "Game/BossSonic.h"
#include "Game/CharInfo.h"
#include "boost/shared_ptr.hpp"
#include "boost/weak_ptr.hpp"
#include "size_assert.h"

/* The game state (struct name unknown; see Game/CenFile.cpp): the stage number at 0xA8
 * (meaning inferred). */
struct UnkGameState_8057D6A0 {
    /* 0x000 */ u8 field_0x0[0xA8];
    /* 0x0A8 */ s32 m_stageId;
    /* 0x0AC */ u8 field_0xAC[0x150 - 0xAC];
};
SIZE_ASSERT(UnkGameState_8057D6A0, 0x150);

extern UnkGameState_8057D6A0 lbl_8057D6A0;

/* The AI's tuning for stages 0x263 and 0x265, and for the others: the three tables of
 * Game/DiablonParam.cpp, which Boss::Diablon reads as DiablonParams (Game/Diablon.h). Which
 * class owns them is unknown, so this unit reads them through its own layout. */
namespace Boss {
namespace Diablon {
struct DiablonParams;
extern DiablonParams g_params611;
extern DiablonParams g_params613;
extern DiablonParams g_paramsDefault;
} // namespace Diablon
} // namespace Boss

namespace Player {
namespace Npc {
namespace BossSonic {

void BossSonicMode::Request(int mode) {
    if (!m_locked) {
        m_next = mode;
    }
}

bool BossSonicMode::HasRequest() {
    return m_next != 0;
}

void BossSonicMode::Apply() {
    m_mode = m_next;
    m_next = 0;
}

UnkBossSonicPart_8033A8B4::UnkBossSonicPart_8033A8B4() {}

BossSonicWork::BossSonicWork(const boost::weak_ptr<Npc> &npc) : m_npc(npc), m_part(new UnkBossSonicPart_8033A8B4) {
    switch (lbl_8057D6A0.m_stageId) {
    case 0x263:
        m_params = (BossSonicParams *)&Boss::Diablon::g_params611;
        break;
    case 0x265:
        m_params = (BossSonicParams *)&Boss::Diablon::g_params613;
        break;
    default:
        m_params = (BossSonicParams *)&Boss::Diablon::g_paramsDefault;
        break;
    }
}

BossSonicWork::~BossSonicWork() {}

void BossSonicWork::UpdateTarget() {
    boost::shared_ptr<CharInfo> npc = m_npc.lock();
    m_target = fn_800A8A7C(fn_80074194(), &npc->m_position);
}

} // namespace BossSonic
} // namespace Npc
} // namespace Player
