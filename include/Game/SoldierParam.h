#ifndef GAME_SOLDIERPARAM_H
#define GAME_SOLDIERPARAM_H

#include <dolphin/types.h>
#include "size_assert.h"

/* The parameters a soldier shares with the other soldiers (struct name unknown; its constructor
 * 0x8018ED00 is inline, also used by GunSoldier). Names repeat the stage editor's. */
struct SoldierParam {
    SoldierParam() {
        Reset();
    }

    /* The stage editor's defaults (name inferred). */
    void Reset() {
        MoveRange = 200.0f;
        SearchRange = 0.0f;
        SearchAngle = 0.0f;
        SearchWidth = 100.0f;
        SearchHeight = 100.0f;
        SearchHeightOffset = 0.0f;
        MoveSpeedRatio = 1.0f;
        HaveShield = 0;
        WeaponType = 0;
        AppearType = 0;
        Pos0_WaitType = 0;
        Pos0_WaitSec = 1.0f;
        Pos0_MoveSpeedRatio = 1.0f;
        Pos1_X = 0.0f;
        Pos1_Z = 0.0f;
        Pos1_WaitType = 0;
        Pos1_WaitSec = 0.0f;
        Pos1_MoveSpeedRatio = 1.0f;
        Pos2_X = 0.0f;
        Pos2_Z = 0.0f;
        Pos2_WaitType = 0;
        Pos2_WaitSec = 0.0f;
        Pos2_MoveSpeedRatio = 1.0f;
    }

    /* 0x00 */ float MoveRange;
    /* 0x04 */ float SearchRange;
    /* 0x08 */ float SearchAngle;
    /* 0x0C */ float SearchWidth;
    /* 0x10 */ float SearchHeight;
    /* 0x14 */ float SearchHeightOffset;
    /* 0x18 */ float MoveSpeedRatio;
    /* 0x1C */ s32 HaveShield;
    /* 0x20 */ s32 WeaponType;
    /* 0x24 */ s32 AppearType;
    /* 0x28 */ s32 Pos0_WaitType;
    /* 0x2C */ float Pos0_WaitSec;
    /* 0x30 */ float Pos0_MoveSpeedRatio;
    /* 0x34 */ float Pos1_X;
    /* 0x38 */ float Pos1_Z;
    /* 0x3C */ s32 Pos1_WaitType;
    /* 0x40 */ float Pos1_WaitSec;
    /* 0x44 */ float Pos1_MoveSpeedRatio;
    /* 0x48 */ float Pos2_X;
    /* 0x4C */ float Pos2_Z;
    /* 0x50 */ s32 Pos2_WaitType;
    /* 0x54 */ float Pos2_WaitSec;
    /* 0x58 */ float Pos2_MoveSpeedRatio;
};
SIZE_ASSERT(SoldierParam, 0x5C);

#endif
