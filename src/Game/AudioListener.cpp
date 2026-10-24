/* Two distances the sound channels (Game/AudioChannel.cpp) and the sound manager
 * (Game/AudioManagerTask.cpp) use: how far a sound is from the player (from the nearer player in
 * two-player games), and how far a bank entry can be heard. A unit of its own: its float
 * constants are pooled apart from the channels'. Function and file names are not known
 * (behaviours inferred). Definitions are in reverse address order. */
#include "Game/CharInfo.h"
#include "Game/MathTypes.h"
#include "Game/SoundManager.h"
#include "boost/shared_ptr.hpp"
#include "boost/weak_ptr.hpp"
#include <dolphin/types.h>
#include "size_assert.h"

/* The game state (struct name unknown): the two-player flag at 0xB2 (see Game/AudioChannel.cpp). */
struct UnkGameState_8057D6A0 {
    /* 0x000 */ u8 field_0x0[0xB2];
    /* 0x0B2 */ u8 m_twoPlayer;
    /* 0x0B3 */ u8 field_0xB3[0x150 - 0xB3];
};
SIZE_ASSERT(UnkGameState_8057D6A0, 0x150);

extern UnkGameState_8057D6A0 lbl_8057D6A0;

/* The first and second players' numbers (see Game/BkLarva.cpp and Game/StageInitalizeTask.cpp). */
extern s32 lbl_805EE588;
extern s32 lbl_805E4968;

extern "C" {
/* The character of a player number (see Game/BkLarva.cpp). */
boost::weak_ptr<CharInfo> fn_800A8B74(UnkCharaList_80074194 *list, int id);
}

/* How far entry can be heard (name inferred). */
extern "C" float fn_801D9BCC(const UnkSoundBankEntry *entry) {
    if (entry->m_range == 0) {
        return 400.0f;
    }
    return 100.0f * entry->m_range;
}

/* The distance from position to the player, or to the nearer player in a two-player game (name
 * inferred). */
extern "C" float fn_801D9964(const Vec3f *position) {
    if (lbl_8057D6A0.m_twoPlayer) {
        float first = 9999999.0f;
        float second = first;
        {
            boost::weak_ptr<CharInfo> player = fn_800A8B74(fn_80074194(), lbl_805EE588);
            if (boost::shared_ptr<CharInfo> character = player.lock()) {
                Vec3f offset;
                fn_8040CD74(&offset, &character->m_position, position);
                first = fn_8040CF50(&offset);
            }
        }
        {
            boost::weak_ptr<CharInfo> player = fn_800A8B74(fn_80074194(), lbl_805E4968);
            if (boost::shared_ptr<CharInfo> character = player.lock()) {
                Vec3f offset;
                fn_8040CD74(&offset, &character->m_position, position);
                second = fn_8040CF50(&offset);
            }
        }
        return first < second ? first : second;
    }
    boost::weak_ptr<CharInfo> player = fn_800A8B74(fn_80074194(), lbl_805EE588);
    if (boost::shared_ptr<CharInfo> character = player.lock()) {
        Vec3f offset;
        fn_8040CD74(&offset, &character->m_position, position);
        return fn_8040CF50(&offset);
    }
    return 0.0f;
}
