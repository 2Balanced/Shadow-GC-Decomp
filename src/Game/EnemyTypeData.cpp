/* The enemies' data by enemy type (Game/Enemy.h; the type indexes Enemy::m_type's list, see
 * fn_8019A8F4 in Game/Enemy.cpp): the resource file names, which EnemyParamCommon hands out, and a
 * count. The unit has no class, and its name is inferred from its contents. */
#include <dolphin/types.h>

/* The enemies' resource files, by enemy type: the data file and the model, bone and motion files
 * (EnemyParamCommon's slots 0x0C to 0x18 return them; the table keeps its address name). */
extern const char *const lbl_804CBF2C[15][4] = {
    {"GunSoldierData.one", "SOLDIERGUN.DFF", "SOLDIERGUN.BON", "GUNSOLDIER.MTP"},
    {"GunBeetleData.one", "BEETLEBOMB.DFF", "BEETLEBOMB.BON", "GUNBEETLE.MTP"},
    {"GunBigFootData.one", "BIGFOOTA.DFF", "BIGFOOTA.BON", "GUNBIGFOOT.MTP"},
    {"GunRobotData.one", "GUNROBOA.DFF", "GUNROBOB.BON", "GUNROBOT.MTP"},
    {"BkGiantData.one", "GIANTBK.DFF", "GIANTBK.BON", "BKGIANT.MTP"},
    {"BkSoldierData.one", "SOLDIERBK.DFF", "SOLDIERBK.BON", "BKSOLDIER.MTP"},
    {"BkWingLargeData.one", "WINGBKLARGE.DFF", "WINGBKLARGE.BON", "BKWINGLARGE.MTP"},
    {"BkWingSmallData.one", "WINGBKSMALL.DFF", "WINGBKSMALL.BON", "BKWINGSMALL.MTP"},
    {"BkWormData.one", "BKWORM.DFF", "BKWORM6.BON", "BKWORM.MTP"},
    {"BkLarvaData.one", "BKLARVA.DFF", "BKLARVA.BON", "BKLARVA.MTP"},
    {"BkChaosData.one", "BKCHAOS.DFF", "BKCHAOS.BON", "BKCHAOS.MTP"},
    {"BkNinjaData.one", "NINJABK.DFF", "BKNINJA.BON", "BKNINJA.MTP"},
    {"EggPawnData.one", "PAWN.DFF", "PAWN.BON", "EGGPAWN.MTP"},
    {"EggPierrotData.one", "EGGPIERROT.DFF", "EGGPIERROT.BON", "EGGPIERROT.MTP"},
    {"EggShadowAndroidData.one", "SHADOW.DFF", "SHADOW.BON", ""},
};

/* A count by enemy type (meaning unknown; name inferred). */
static const s32 s_typeCounts[15] = {6, 11, 9, 11, 5, 7, 6, 5, 9, -1, 12, 5, 6, 6, 6};

/* The count of enemy type type (name inferred). */
extern "C" s32 fn_801A04C0(int type) {
    return s_typeCounts[type];
}
