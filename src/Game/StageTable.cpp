/* The stage table at 0x804C4A18 (file name inferred): one entry per stage, with its id, name,
 * flags, the four callbacks of stages with special handling, and the mission ids of each player,
 * and the lookups on it. Names marked "(name inferred)" are not original, nor are the fn_ names
 * the functions other units call keep. Definitions are in reverse address order (deferred inlining
 * emits functions last-defined first). */
#include "Game/StageTable.h"
#include <dolphin/types.h>
#include "size_assert.h"

/* The game state (struct name unknown; see Game/CityBombSmall.cpp). */
struct UnkGameState_8057D6A0 {
    /* 0x000 */ u8 field_0x0[0xA8];
    /* 0x0A8 */ s32 m_stageId; /* (name inferred) */
    /* 0x0AC */ u8 field_0xAC[0x150 - 0xAC];
};
SIZE_ASSERT(UnkGameState_8057D6A0, 0x150);

extern UnkGameState_8057D6A0 lbl_8057D6A0;

/* The stages' callbacks, in their own units. */
extern "C" {
void fn_8016A9E0(void);
void fn_8016AA20(void);
void fn_8016AA28(void);
void fn_8016AA30(void);
void fn_8016AA70(void);
void fn_8016AA78(void);
void fn_8016AA80(void);
void fn_8016AA88(void);
void fn_80254E90(void);
void fn_80254EB4(void);
void fn_80254ED8(void);
void fn_80254EFC(void);
void fn_802733C4(void);
void fn_802733E8(void);
void fn_8027340C(void);
void fn_80273430(void);
void fn_80286D4C(void);
void fn_80286D80(void);
void fn_80286D88(void);
void fn_80286D90(void);
void fn_80286DC4(void);
void fn_80286DF8(void);
void fn_80286E00(void);
void fn_80286E08(void);
void fn_80286E4C(void);
void fn_80286E80(void);
void fn_80286E88(void);
void fn_80286E90(void);
void fn_80301C40(void);
void fn_80301C64(void);
void fn_80301C88(void);
void fn_80301CAC(void);
void fn_8031E960(void);
void fn_8031E984(void);
void fn_8031E9A8(void);
void fn_8031E9CC(void);
void fn_80328048(void);
void fn_80328050(void);
void fn_80328058(void);
void fn_80328060(void);
void fn_80328068(void);
void fn_80328070(void);
void fn_80328078(void);
void fn_80328080(void);
void fn_80328088(void);
void fn_80328090(void);
void fn_80328098(void);
void fn_803280A0(void);
void fn_803280A8(void);
void fn_803280B0(void);
void fn_803280B8(void);
void fn_803280C0(void);
void fn_803280C8(void);
void fn_803280D0(void);
void fn_803280D8(void);
void fn_803280E0(void);
}

/* The table. */
extern const UnkStageInfo_804C4A18 lbl_804C4A18[59] = {
    {0, "Practice", 0, {fn_8016AA30, fn_8016AA28, fn_8016AA20, fn_8016A9E0}, {{-2, 2, -1}, {-2, 0, -1}, {-2, 1, -1}}, 0, 0x7f9, -1, 0},
    {1, "Practice01", 0, {0, 0, 0, 0}, {{-2, -1, -1}, {-2, -1, -1}, {-2, -1, -1}}, -1, 0x7f9, -1, 0},
    {2, "Practice02", 0, {0, 0, 0, 0}, {{-2, -1, -1}, {-2, -1, -1}, {-2, -1, -1}}, -1, 0x7f9, -1, 0},
    {3, "Practice03", 0, {0, 0, 0, 0}, {{-2, -1, -1}, {-2, -1, -1}, {-2, -1, -1}}, -1, 0x7f9, -1, 0},
    {5, "EVENT", 0x20, {0, 0, 0, 0}, {{-2, -1, -1}, {-2, -1, -1}, {-2, -1, -1}}, -1, 0x7f9, -1, 0},
    {0x64, "City1", 0, {0, 0, 0, 0}, {{6, 1, 0x229}, {7, 0, 0x227}, {8, 2, 0x228}}, 0, 0x7f9, 0x226, 2},
    {0xc8, "Circuit", 0, {fn_80286E90, fn_80286E88, fn_80286E80, fn_80286E4C}, {{9, 3, 0x249}, {9, -1, -1}, {0xa, 0, 0x248}}, 0, 0x7fe, 0x247, 3},
    {0xc9, "Canyon1", 0, {0, 0, 0, 0}, {{9, 4, 0x23c}, {0xa, 0, 0x23a}, {0xb, 5, 0x23b}}, 0, 0x7fc, 0x239, 3},
    {0xca, "Highway", 0, {fn_8031E9CC, fn_8031E9A8, fn_8031E984, fn_8031E960}, {{0x1c, 0, 0x22f}, {0x1c, -1, -1}, {0x1c, 6, 0x22e}}, 0, 0x7fa, 0x22d, 0},
    {0x12c, "HorrorCastle", 0, {0, 0, 0, 0}, {{0x1d, 7, 0x257}, {0x1d, 0, 0x255}, {0x1d, 8, 0x256}}, 0, 0x800, 0x254, 0},
    {0x12d, "PrisonIsland", 0, {fn_80328060, fn_80328058, fn_80328050, fn_80328048}, {{0xd, 9, 0x265}, {0xe, 0, 0x263}, {0xf, 0xa, 0x264}}, 0, 0x802, 0x262, 4},
    {0x12e, "Circus", 0, {0, 0, 0, 0}, {{0xe, 0xb, 0x25e}, {0xf, 0, 0x25c}, {0x10, 0xc, 0x25d}}, 0, 0x801, 0x25b, 4},
    {0x190, "City2", 0, {0, 0, 0, 0}, {{0x11, 0xd, 0x235}, {0x12, -1, -1}, {0x12, 0xe, 0x234}}, 0x25, 0x7fb, 0x233, 0},
    {0x191, "ARKPast1", 0, {0, 0, 0, 0}, {{0x1e, 0xf, 0x279}, {0x1e, 0, 0x277}, {0x1e, 0x10, 0x278}}, 0x26, 0x805, 0x276, 0},
    {0x192, "canyon2", 0, {fn_8016AA88, fn_8016AA80, fn_8016AA78, fn_8016AA70}, {{0x12, 0x11, 0x243}, {0x13, 0, 0x241}, {0x14, 0x12, 0x242}}, 0, 0x7fd, 0x240, 0},
    {0x193, "eWorld", 0, {fn_80286E08, fn_80286E00, fn_80286DF8, fn_80286DC4}, {{0x1f, 0x13, 0x250}, {0x1f, 0, 0x24e}, {0x1f, 0x14, 0x24f}}, 0x27, 0x7ff, 0x24d, 0},
    {0x194, "Ruins", 0, {0, 0, 0, 0}, {{0x20, 0, 0x26b}, {0x20, -1, -1}, {0x20, 0x15, 0x26a}}, 0, 0x803, 0x269, 0},
    {0x1f4, "ARKRuins1", 0, {0, 0, 0, 0}, {{0x21, 0x16, 0x27f}, {0x21, 0, 0x27e}, {0x21, -1, -1}}, 0, 0x806, 0x27d, 0},
    {0x1f5, "Sky", 0, {fn_80254EFC, fn_80254ED8, fn_80254EB4, fn_80254E90}, {{0x16, 0x17, 0x299}, {0x17, 0, 0x297}, {0x18, 0x18, 0x298}}, 0, 0x80a, 0x296, 5},
    {0x1f6, "Jungle", 0, {fn_80273430, fn_8027340C, fn_802733E8, fn_802733C4}, {{0x22, 0x19, 0x272}, {0x22, 0, 0x270}, {0x22, 0x1a, 0x271}}, 0, 0x804, 0x26f, 0},
    {0x1f7, "Space", 0, {fn_803280A0, fn_80328098, fn_80328090, fn_80328088}, {{0x18, 0x1b, 0x286}, {0x19, 0, 0x284}, {0x1a, 0x1c, 0x285}}, 0, 0x807, 0x283, 5},
    {0x1f8, "ARKPast2", 0, {fn_80328080, fn_80328078, fn_80328070, fn_80328068}, {{0x19, -1, -1}, {0x19, 0, 0x28b}, {0x1a, 0x1d, 0x28c}}, 0, 0x808, 0x28a, 0},
    {0x258, "GunsBase", 0, {0, 0, 0, 0}, {{0x24, 0x1e, 0x29f}, {0x24, -1, -1}, {0x23, 0, 0x29e}}, 0, 0x80b, 0x29d, 6},
    {0x259, "DoomsBase1", 0, {fn_803280C0, fn_803280B8, fn_803280B0, fn_803280A8}, {{0x26, 0x1f, 0x2ab}, {0x25, -1, -1}, {0x25, 0, 0x2aa}}, 0, 0x80d, 0x2a9, 6},
    {0x25a, "EggmansBase", 0, {fn_803280E0, fn_803280D8, fn_803280D0, fn_803280C8}, {{0x27, 0x20, 0x2a5}, {0x27, -1, -1}, {0x27, 0, 0x2a4}}, 0, 0x80c, 0x2a3, 6},
    {0x25b, "ARKRuins2", 0, {0, 0, 0, 0}, {{0x28, 0x21, 0x292}, {0x28, -1, -1}, {0x29, 0x22, 0x291}}, 0, 0x809, 0x290, 6},
    {0x25c, "DoomsBase2", 0, {fn_80301CAC, fn_80301C88, fn_80301C64, fn_80301C40}, {{0x2b, 0x23, 0x2b1}, {0x2b, -1, -1}, {0x2a, 0, 0x2b0}}, 0, 0x80e, 0x2af, 6},
    {0x2bc, "DoomsCore", 0x10, {0, 0, 0, 0}, {{0x2c, -1, -1}, {0x2c, -1, -1}, {0x2c, 0x24, 0x2b4}}, 0, 0x80d, -1, 0},
    {0xd2, "BossBK1", 1, {0, 0, 0, 0}, {{0xa, -1, 0x2b5}, {0xb, -1, 0x2b5}, {0xb, -1, 0x2b5}}, -1, -1, -1, 3},
    {0x136, "BossEggMeka1", 1, {0, 0, 0, 0}, {{0xc, -1, 0x2b7}, {0xd, -1, 0x2b7}, {0xe, -1, 0x2b7}}, -1, -1, -1, 4},
    {0x19a, "BossGUN1", 1, {0, 0, 0, 0}, {{0x11, -1, 0x2ba}, {0x12, -1, 0x2ba}, {0x13, -1, 0x2ba}}, -1, -1, -1, 0},
    {0x19b, "BossEggMeka2", 1, {0, 0, 0, 0}, {{0x13, -1, 0x2b8}, {0x14, -1, 0x2b8}, {0x15, -1, 0x2b8}}, -1, -1, -1, 0},
    {0x19c, "BossBK2", 1, {0, 0, 0, 0}, {{0x14, -1, 0x2b6}, {0x15, -1, 0x2b6}, {0x15, -1, 0x2b6}}, -1, -1, -1, 0},
    {0x1fe, "BossGUN2", 1, {0, 0, 0, 0}, {{0x16, -1, 0x2bb}, {0x17, -1, 0x2bb}, {0x17, -1, 0x2bb}}, -1, -1, -1, 5},
    {0x1ff, "BossEggMeka3", 1, {0, 0, 0, 0}, {{0x17, -1, 0x2b9}, {0x18, -1, 0x2b9}, {0x19, -1, 0x2b9}}, -1, -1, -1, 5},
    {0x262, "BossBlack_0610", 1, {0, 0, 0, 0}, {{-2, -1, 0x2bc}, {-2, -1, 0x2bc}, {-2, -1, 0x2bc}}, -1, -1, -1, 7},
    {0x263, "BossSonic_0611", 1, {0, 0, 0, 0}, {{-2, -1, 0x2bd}, {-2, -1, 0x2bd}, {-2, -1, 0x2bd}}, -1, -1, -1, 7},
    {0x264, "BossEggmanRobbo_0612", 1, {0, 0, 0, 0}, {{-2, -1, 0x2be}, {-2, -1, 0x2be}, {-2, -1, 0x2be}}, -1, -1, -1, 7},
    {0x265, "BossSonic_0613", 1, {0, 0, 0, 0}, {{-2, -1, 0x2bf}, {-2, -1, 0x2bf}, {-2, -1, 0x2bf}}, -1, -1, -1, 7},
    {0x266, "BossEggmanRobbo_0614", 1, {0, 0, 0, 0}, {{-2, -1, 0x2c0}, {-2, -1, 0x2c0}, {-2, -1, 0x2c0}}, -1, -1, -1, 7},
    {0x267, "BossEggmanRobbo_0615", 1, {0, 0, 0, 0}, {{-2, -1, 0x2c1}, {-2, -1, 0x2c1}, {-2, -1, 0x2c1}}, -1, -1, -1, 7},
    {0x268, "BossBlack_0616", 1, {0, 0, 0, 0}, {{-2, -1, 0x2c2}, {-2, -1, 0x2c2}, {-2, -1, 0x2c2}}, -1, -1, -1, 7},
    {0x269, "BossBlack_0617", 1, {0, 0, 0, 0}, {{-2, -1, 0x2c3}, {-2, -1, 0x2c3}, {-2, -1, 0x2c3}}, -1, -1, -1, 7},
    {0x26a, "BossSonic_0618", 1, {0, 0, 0, 0}, {{-2, -1, 0x2c4}, {-2, -1, 0x2c4}, {-2, -1, 0x2c4}}, -1, -1, -1, 7},
    {0x2c6, "BossLast", 0x19, {fn_80286D90, fn_80286D88, fn_80286D80, fn_80286D4C}, {{-2, -1, 0x2c5}, {-2, -1, 0x2c5}, {-2, -1, 0x2c5}}, -1, -1, -1, 0},
    {0x320, "GUN2P_0800", 6, {0, 0, 0, 0}, {{-2, -1, -1}, {-2, 0x28, -1}, {-2, -1, -1}}, -1, -1, -1, 0},
    {0x321, "GUN2P_0801", 6, {0, 0, 0, 0}, {{-2, -1, -1}, {-2, 0x28, -1}, {-2, -1, -1}}, -1, -1, -1, 0},
    {0x322, "GUN2P_0802", 6, {0, 0, 0, 0}, {{-2, -1, -1}, {-2, 0x28, -1}, {-2, -1, -1}}, -1, -1, -1, 0},
    {0x385, "EVENT0901", 0x20, {0, 0, 0, 0}, {{-2, -1, -1}, {-2, -1, -1}, {-2, -1, -1}}, -1, -1, -1, 0},
    {0x386, "EVENT0902", 0x20, {0, 0, 0, 0}, {{-2, -1, -1}, {-2, -1, -1}, {-2, -1, -1}}, -1, -1, -1, 0},
    {0x387, "EVENT0903", 0x20, {0, 0, 0, 0}, {{-2, -1, -1}, {-2, -1, -1}, {-2, -1, -1}}, -1, -1, -1, 0},
    {0x388, "EVENT0904", 0x20, {0, 0, 0, 0}, {{-2, -1, -1}, {-2, -1, -1}, {-2, -1, -1}}, -1, -1, -1, 0},
    {0x389, "EVENT0905", 0x20, {0, 0, 0, 0}, {{-2, -1, -1}, {-2, -1, -1}, {-2, -1, -1}}, -1, -1, -1, 0},
    {0x38a, "EVENT0906", 0x20, {0, 0, 0, 0}, {{-2, -1, -1}, {-2, -1, -1}, {-2, -1, -1}}, -1, -1, -1, 0},
    {0x38b, "EVENT0907", 0x20, {0, 0, 0, 0}, {{-2, -1, -1}, {-2, -1, -1}, {-2, -1, -1}}, -1, -1, -1, 0},
    {0x38c, "EVENT0908", 0x20, {0, 0, 0, 0}, {{-2, -1, -1}, {-2, -1, -1}, {-2, -1, -1}}, -1, -1, -1, 0},
    {0x38d, "EVENT0909", 0x20, {0, 0, 0, 0}, {{-2, -1, -1}, {-2, -1, -1}, {-2, -1, -1}}, -1, -1, -1, 0},
    {0x38e, "EVENT0910", 0x20, {0, 0, 0, 0}, {{-2, -1, -1}, {-2, -1, -1}, {-2, -1, -1}}, -1, -1, -1, 0},
    {0x38f, "EVENT0911", 0x20, {0, 0, 0, 0}, {{-2, -1, -1}, {-2, -1, -1}, {-2, -1, -1}}, -1, -1, -1, 0},
};

/* The stage entry of id, or of the current stage when id is -1 (name inferred). */
extern "C" UnkStageInfo_804C4A18 *fn_80176C3C(UnkObject_800B74B4 *object, int id) {
    if (id == -1) {
        id = lbl_8057D6A0.m_stageId;
    }
    for (int i = 0; i < 59; i++) {
        if (id == lbl_804C4A18[i].m_id) {
            return const_cast<UnkStageInfo_804C4A18 *>(&lbl_804C4A18[i]);
        }
    }
    return 0;
}

/* The index of the stage entry of id (name inferred). */
extern "C" int fn_80176C04(UnkObject_800B74B4 *object, int id) {
    for (int i = 0; i < 59; i++) {
        if (id == lbl_804C4A18[i].m_id) {
            return i;
        }
    }
    return -1;
}

/* The mission id of a player in the stage entry at index (name inferred). */
extern "C" s32 fn_80176BE4(UnkObject_800B74B4 *object, int index, int player) {
    return reinterpret_cast<const UnkStageInfo_804C4A18 *>(reinterpret_cast<const u8 *>(lbl_804C4A18) +
                                                           index * sizeof(UnkStageInfo_804C4A18))
        ->m_missions[player]
        .m_id;
}
