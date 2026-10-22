/* The hint helpers that character code calls (file name inferred; the unit has no RTTI): the level,
 * greeting and random hints of the NPC partners, from per-stage tables. The unit sits right before
 * Game/HintController.cpp; its .sdata (the highest level hint) is padded to its own 8-byte boundary,
 * so it is a unit of its own. Function names are inferred. Definitions are in reverse address order
 * (deferred inlining emits functions last-defined first). */
#include "Game/Npc.h"
#include "Game/Player.h"
#include "Game/Random.h"
#include "boost/shared_ptr.hpp"
#include "boost/weak_ptr.hpp"
#include <dolphin/types.h>
#include "size_assert.h"

/* The game state block in .bss (only the members used here; see EnemyHint.cpp). */
struct UnkGameState_8057D6A0 {
    /* 0x000 */ u8 field_0x0[0xA8];
    /* 0x0A8 */ s32 m_stageId; /* the stage number in file names (meaning inferred) */
    /* 0x0AC */ s32 m_stage;   /* the stage's index (name inferred; see Game/CenFile.cpp) */
    /* 0x0B0 */ u8 field_0xB0;
    /* 0x0B1 */ u8 field_0xB1; /* meaning unknown */
    /* 0x0B2 */ u8 field_0xB2[0x150 - 0xB2];
};
SIZE_ASSERT(UnkGameState_8057D6A0, 0x150);

extern UnkGameState_8057D6A0 lbl_8057D6A0;

/* The hint manager (class name unknown; see HintRing.cpp and EnemyHint.cpp). */
class UnkHintManager_801FB180;

/* The out-of-line copy of the inline accessor HintRing.cpp defines (0x80109D64). */
UnkHintManager_801FB180 *GetHintManager();

extern "C" {
/* Shows message id of branch (see HintRing.cpp). */
s32 fn_801FAF00(UnkHintManager_801FB180 *manager, s32 id, s32 branch, int arg3, int arg4);
/* The hint branch of a character (see BDHologram.cpp). */
s32 fn_801FAAC4(int character);
/* The character and the level of an NPC (behaviour inferred). */
int fn_80073A08(Player::Npc::Npc *npc);
int fn_800736FC(Player::Npc::Npc *npc);
/* A random value in [0, 1). */
float fn_8040EE90(void);
}

/* The hints of each stage (struct name unknown; names inferred): three level hints, four hints
 * picked at random and a greeting. */
struct HintSet {
    /* 0x00 */ s32 m_level[3];
    /* 0x0C */ s32 m_random[4];
    /* 0x1C */ s32 m_greeting;
};
SIZE_ASSERT(HintSet, 0x20);

/* Indexed by the stage's index; the last three rows are zero. */
static const HintSet s_hintSets[59] = {
    {{-1, -1, -1}, {-1, -1, -1, -1}, -1},
    {{-1, -1, -1}, {-1, -1, -1, -1}, -1},
    {{-1, -1, -1}, {-1, -1, -1, -1}, -1},
    {{-1, -1, -1}, {-1, -1, -1, -1}, -1},
    {{-1, -1, -1}, {-1, -1, -1, -1}, -1},
    {{0x208, 0x212, 0x21C}, {0x1B7, 0x1B8, 0x1BA, 0x1BB}, 0x729},
    {{0x244, 0x245, 0x246}, {0x1B7, 0x1B8, 0x1BC, 0x1BD}, 0x72A},
    {{0x236, 0x237, 0x238}, {0x1B7, 0x1B8, 0x1BE, 0x1BF}, 0x72B},
    {{0x22A, 0x22B, 0x22C}, {0x1B7, 0x1B8, 0x1C0, 0x1C1}, 0x72C},
    {{0x251, 0x252, 0x253}, {0x1B7, 0x1B8, 0x1C2, 0x1C3}, 0x72D},
    {{0x25F, 0x260, 0x261}, {0x1B7, 0x1B8, 0x1C4, 0x1C5}, 0x72E},
    {{0x258, 0x259, 0x25A}, {0x1B7, 0x1B8, 0x1C6, 0x1C7}, 0x72F},
    {{0x230, 0x231, 0x232}, {0x1B7, 0x1B8, 0x1C8, 0x1C9}, 0x730},
    {{0x273, 0x274, 0x275}, {0x1B7, 0x1B8, 0x1CA, 0x1CB}, 0x731},
    {{0x23D, 0x23E, 0x23F}, {0x1B7, 0x1B8, 0x1CC, 0x1CD}, 0x732},
    {{0x24A, 0x24B, 0x24C}, {0x1B7, 0x1B8, 0x1CE, 0x1CF}, 0x733},
    {{0x266, 0x267, 0x268}, {0x1B7, 0x1B8, 0x1D0, 0x1D1}, 0x734},
    {{0x27A, 0x27B, 0x27C}, {0x1B7, 0x1B8, 0x1D2, 0x1D3}, 0x735},
    {{0x293, 0x294, 0x295}, {0x1B7, 0x1B8, 0x1D4, 0x1D5}, 0x736},
    {{0x26C, 0x26D, 0x26E}, {0x1B7, 0x1B8, 0x1D6, 0x1D7}, 0x737},
    {{0x280, 0x281, 0x282}, {0x1B7, 0x1B8, 0x1D8, 0x1D9}, 0x738},
    {{0x287, 0x288, 0x289}, {0x1B7, 0x1B8, 0x1DA, 0x1DB}, 0x739},
    {{0x29A, 0x29B, 0x29C}, {0x1B7, 0x1B8, 0x1DC, 0x1DD}, 0x73A},
    {{0x2A6, 0x2A7, 0x2A8}, {0x1B7, 0x1B8, 0x1DE, 0x1DF}, 0x73B},
    {{0x2A0, 0x2A1, 0x2A2}, {0x1B7, 0x1B8, 0x1E0, 0x1E1}, 0x73C},
    {{0x28D, 0x28E, 0x28F}, {0x1B7, 0x1B8, 0x1E2, 0x1E3}, 0x73D},
    {{0x2AC, 0x2AD, 0x2AE}, {0x1B7, 0x1B8, 0x1E4, 0x1E5}, 0x73E},
    {{-1, -1, -1}, {0x1B7, 0x1B8, 0x1E6, 0x1E6}, -1},
    {{-1, -1, -1}, {0x1B7, 0x1B8, 0x1E7, 0x1E8}, -1},
    {{-1, -1, -1}, {0x1B7, 0x1B8, 0x1E9, 0x1EA}, -1},
    {{-1, -1, -1}, {0x1B7, 0x1B8, 0x1EB, 0x1EC}, -1},
    {{-1, -1, -1}, {0x1B7, 0x1B8, 0x1ED, 0x1EE}, -1},
    {{-1, -1, -1}, {0x1B7, 0x1B8, 0x1EF, 0x1F0}, -1},
    {{-1, -1, -1}, {0x1B7, 0x1B8, 0x1F1, 0x1F2}, -1},
    {{-1, -1, -1}, {0x1B7, 0x1B8, 0x1F3, 0x1F4}, -1},
    {{-1, -1, -1}, {0x1B7, 0x1B8, 0x1F5, 0x1F6}, -1},
    {{-1, -1, -1}, {0x1B7, 0x1B8, 0x1F7, 0x1F8}, -1},
    {{-1, -1, -1}, {0x1B7, 0x1B8, 0x1F9, 0x1FA}, -1},
    {{-1, -1, -1}, {0x1B7, 0x1B8, 0x1FB, 0x1FC}, -1},
    {{-1, -1, -1}, {0x1B7, 0x1B8, 0x1FD, 0x1FE}, -1},
    {{-1, -1, -1}, {0x1B7, 0x1B8, 0x1FF, 0x200}, -1},
    {{-1, -1, -1}, {0x1B7, 0x1B8, 0x201, 0x202}, -1},
    {{-1, -1, -1}, {0x1B7, 0x1B8, 0x203, 0x204}, -1},
    {{-1, -1, -1}, {0x1B7, 0x1B8, 0x205, 0x206}, -1},
    {{-1, -1, -1}, {0x1B7, 0x1B8, 0x207, 0x207}, -1},
    {{-1, -1, -1}, {-1, -1, -1, -1}, -1},
    {{-1, -1, -1}, {-1, -1, -1, -1}, -1},
    {{-1, -1, -1}, {-1, -1, -1, -1}, -1},
    {{-1, -1, -1}, {-1, -1, -1, -1}, -1},
    {{-1, -1, -1}, {-1, -1, -1, -1}, -1},
    {{-1, -1, -1}, {-1, -1, -1, -1}, -1},
    {{-1, -1, -1}, {-1, -1, -1, -1}, -1},
    {{-1, -1, -1}, {-1, -1, -1, -1}, -1},
    {{-1, -1, -1}, {-1, -1, -1, -1}, -1},
    {{-1, -1, -1}, {-1, -1, -1, -1}, -1},
    {{-1, -1, -1}, {-1, -1, -1, -1}, -1},
};

/* The first level hint of each NPC character, or -1 for none; the last five entries are zero. */
static const s32 s_levelHints[20] = {
    -1, 0x691, 0x69E, 0x6AB, 0x6B8, 0x6C5, 0x6D2, 0x6DF, 0x6F9, 0x706, 0x6EC, 0x720, 0x713, -1, -1,
};

/* The highest level hint. */
static int s_maxLevel = 2;

/* The smaller of a and b (b when they are equal). */
static const int &Min(const int &a, const int &b) {
    return a < b ? a : b;
}

/* Shows hint id in branch, if there is one. */
static void ShowHint(s32 id, s32 branch) {
    if (id >= 0) {
        fn_801FAF00(GetHintManager(), id, branch, 1, 0);
    }
}

/* Shows the level hint of npc's level. */
void ShowLevelHint(const boost::weak_ptr<Player::PlayerBase> &player) {
    if (!player.expired()) {
        const HintSet &set = s_hintSets[lbl_8057D6A0.m_stage];
        boost::shared_ptr<Player::PlayerBase> base = player.lock();
        boost::shared_ptr<Player::Npc::Npc> npc = boost::dynamic_pointer_cast<Player::Npc::Npc>(base);
        int level = fn_800736FC(npc.get());
        int index = Min(level, s_maxLevel);
        if (set.m_level[index] < 0) {
            return;
        }
        s32 id = set.m_level[index];
        s32 branch = fn_801FAAC4(fn_80073A08(npc.get()));
        ShowHint(id, branch);
    }
}

/* Shows the stage's greeting for player. */
void ShowGreeting(const boost::weak_ptr<Player::PlayerBase> &player) {
    if (!player.expired()) {
        const HintSet &set = s_hintSets[lbl_8057D6A0.m_stage];
        if (set.m_greeting >= 0) {
            boost::shared_ptr<Player::PlayerBase> base = player.lock();
            boost::shared_ptr<Player::Npc::Npc> npc = boost::dynamic_pointer_cast<Player::Npc::Npc>(base);
            s32 id = set.m_greeting;
            s32 branch = fn_801FAAC4(fn_80073A08(npc.get()));
            ShowHint(id, branch);
        }
    }
}

/* Shows one of the stage's random hints, or hint 0x1B9 with chance chance. */
void ShowRandomHint(float chance) {
    s32 branch = fn_801FAAC4(1);
    s32 id;
    if (fn_8040EE90() <= chance) {
        id = 0x1B9;
    } else {
        int pick = fn_8040EA90(lbl_8057D6A0.field_0xB1 ? 2 : 4);
        const HintSet &set = s_hintSets[lbl_8057D6A0.m_stage];
        id = set.m_random[pick];
    }
    ShowHint(id, branch);
}

/* Shows one of the stage's random hints for player, or hint 0x1B9 with chance chance. */
void ShowRandomHint(const boost::weak_ptr<Player::PlayerBase> &player, float chance) {
    if (!player.expired()) {
        boost::shared_ptr<Player::PlayerBase> base = player.lock();
        boost::shared_ptr<Player::Npc::Npc> npc = boost::dynamic_pointer_cast<Player::Npc::Npc>(base);
        s32 branch = fn_801FAAC4(fn_80073A08(npc.get()));
        s32 id;
        if (fn_8040EE90() <= chance) {
            id = 0x1B9;
        } else {
            int pick = fn_8040EA90(4);
            const HintSet &set = s_hintSets[lbl_8057D6A0.m_stage];
            id = set.m_random[pick];
        }
        ShowHint(id, branch);
    }
}

/* Shows the hint of npc reaching level count (every third count; kind picks the variant). */
void ShowLevelUpHint(const boost::shared_ptr<Player::Npc::Npc> &npc, int kind, int count) {
    int n = count - 1;
    if (n % 3 == 0) {
        int level = (n / 3) % 3;
        int character = *reinterpret_cast<const s32 *>(npc->field_0x64);
        if (character == 9) {
            switch (kind) {
            case 0:
                if (lbl_8057D6A0.m_stageId == 0x1F8) {
                    level += 3;
                }
                break;
            case 1:
                break;
            case 2:
                level += 6;
                break;
            }
        } else {
            switch (kind) {
            case 2:
                level += 3;
                break;
            case 1:
                level += 6;
                break;
            }
        }
        if (s_levelHints[character] >= 0) {
            ShowHint(level + s_levelHints[character], fn_801FAAC4(fn_80073A08(npc.get())));
        }
    }
}
