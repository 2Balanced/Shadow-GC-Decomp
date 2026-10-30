/* The player's recent states (class name unknown): a ring of 120 records of the player's
 * position, motion, behavior and state bits, one taken each frame. File and other names inferred.
 * Definitions are in reverse address order (deferred inlining emits functions last-defined
 * first). */
#include "Game/MathTypes.h"
#include "Game/Player.h"
#include "Game/PlayerBehavior.h"
#include "Game/UnkWords_8007A4D4.h"
#include "boost/shared_ptr.hpp"
#include <dolphin/types.h>
#include "size_assert.h"

namespace Player {
class Executer;
}

/* The player's members used here (struct name unknown; Player::PlayerBase leaves them unmapped). */
struct UnkPlayerFields_800A849C {
    /* 0x000 */ Vec3f m_position;
    /* 0x00C */ Vec3f m_rotation;
    /* 0x018 */ Vec3f m_velocity;
    /* 0x024 */ Vec3f field_0x24;
    /* 0x030 */ u8 field_0x30[0x60 - 0x30];
    /* 0x060 */ float *m_frameTime;
    /* 0x064 */ u8 field_0x64[0xA4 - 0x64];
    /* 0x0A4 */ u32 field_0xA4;
    /* 0x0A8 */ UnkWords_8007A4D4 m_stateBits;
    /* 0x0B4 */ u32 field_0xB4;
    /* 0x0B8 */ Vec3f field_0xB8;
    /* 0x0C4 */ u8 field_0xC4[0x1C8 - 0xC4];
    /* 0x1C8 */ Vec3f m_groundUp;
    /* 0x1D4 */ u8 field_0x1D4[0x22C - 0x1D4];
    /* 0x22C */ Player::Executer *m_executer;
};

#define Fields(player) reinterpret_cast<UnkPlayerFields_800A849C *>(player)

/* The game state's frame counter (struct name unknown; see Game/TGi.h). */
struct UnkFrameCounter_8057D6A0 {
    /* 0x00 */ u8 field_0x0[0x90];
    /* 0x90 */ u32 m_frame;
};
extern UnkFrameCounter_8057D6A0 lbl_8057D6A0;

extern "C" {
void fn_80014E04(u32 *flags);
void fn_8000B21C(u32 *flags, const u32 *source);
/* The player's facing vector and its executer's running behavior (behaviour inferred). */
Vec3f fn_800796DC(Player::PlayerBase *player);
boost::shared_ptr<Player::Behavior::Behavior> fn_80074200(Player::Executer *executer);
}

/* One of the player's recent states (struct name unknown; names inferred). */
struct UnkPlayerRecord_800A849C {
    UnkPlayerRecord_800A849C() : m_rotation(0.0f, 0.0f, 0.0f) {
        fn_80014E04(&m_flags);
    }
    ~UnkPlayerRecord_800A849C() {}
    void Set(Player::PlayerBase *player);

    /* 0x00 */ Vec3f field_0x0;
    /* 0x0C */ Vec3f m_position;
    /* 0x18 */ Vec3f m_velocity;
    /* 0x24 */ Vec3f field_0x24;
    /* 0x30 */ Vec3f m_groundUp;
    /* 0x3C */ Vec3f field_0x3C;
    /* 0x48 */ Vec3f m_rotation;
    /* 0x54 */ const char *m_behavior;
    /* 0x58 */ UnkWords_8007A4D4 m_stateBits;
    /* 0x64 */ u32 m_flags;
    /* 0x68 */ u32 m_frame;
    /* 0x6C */ float m_frameTime;
};
SIZE_ASSERT(UnkPlayerRecord_800A849C, 0x70);

void UnkPlayerRecord_800A849C::Set(Player::PlayerBase *player) {
    fn_800091D0(&field_0x0, &Fields(player)->field_0xB8);
    fn_800091D0(&m_position, &Fields(player)->m_position);
    fn_800091D0(&m_velocity, &Fields(player)->m_velocity);
    fn_800091D0(&field_0x24, &Fields(player)->field_0x24);
    fn_800091D0(&m_groundUp, &Fields(player)->m_groundUp);
    fn_800091D0(&field_0x3C, &(const Vec3f &)fn_800796DC(player));
    m_rotation = Fields(player)->m_rotation;
    boost::shared_ptr<Player::Behavior::Behavior> behavior = fn_80074200(Fields(player)->m_executer);
    m_behavior = behavior ? behavior->m_name : "";
    m_stateBits = Fields(player)->m_stateBits;
    fn_8000B21C(&m_flags, &Fields(player)->field_0xA4);
    m_frame = lbl_8057D6A0.m_frame;
    m_frameTime = *Fields(player)->m_frameTime;
}

/* The player's recent states (class name unknown): a ring of 120 records (names inferred). */
class UnkPlayerHistory_800A82C8 {
  public:
    UnkPlayerHistory_800A82C8(Player::PlayerBase *player);
    ~UnkPlayerHistory_800A82C8();
    void Record();
    void Nothing();
    UnkPlayerRecord_800A849C *Get(int back);

    /* 0x00 */ Player::PlayerBase *m_player;
    /* 0x04 */ UnkPlayerRecord_800A849C *m_records;
    /* 0x08 */ s32 m_index;
    /* 0x0C */ u32 field_0xC;
    /* 0x10 */ float field_0x10;
    /* 0x14 */ u32 field_0x14;
    /* 0x18 */ s32 m_count;
    /* 0x1C */ u32 field_0x1C;
};

UnkPlayerHistory_800A82C8::UnkPlayerHistory_800A82C8(Player::PlayerBase *player) {
    m_player = player;
    m_index = 0;
    field_0xC = 0;
    field_0x10 = 0.0f;
    field_0x14 = 0;
    m_count = 120;
    m_records = new UnkPlayerRecord_800A849C[120];
    field_0x1C = 0;
}

UnkPlayerHistory_800A82C8::~UnkPlayerHistory_800A82C8() {
    delete[] m_records;
}

void UnkPlayerHistory_800A82C8::Record() {
    m_index++;
    if (m_index >= 120) {
        m_index = 0;
    }
    m_records[m_index].Set(m_player);
}

void UnkPlayerHistory_800A82C8::Nothing() {}

UnkPlayerRecord_800A849C *UnkPlayerHistory_800A82C8::Get(int back) {
    int index = m_index - back;
    if (index < 0) {
        index += 120;
    }
    return &m_records[index];
}
