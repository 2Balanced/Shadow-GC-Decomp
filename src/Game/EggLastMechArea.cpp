/* The arena areas of the EggLastMech fight (file name unknown): EggLastMech's UnkPart_80334C78
 * tracks which cell of a 3 by 3 grid on the arena floor the player and the boss stand in (areas 11
 * to 33: ten times the row along z plus the column along x, split at +-s_border), how long each
 * has been there, and how far apart they are; the boss's AI states use it. Names marked "(name
 * inferred)" are not original. Definitions are in reverse address order (deferred inlining emits
 * functions last-defined first).
 *
 * NonMatching: 6 of 8 functions match. fn_803349A0 differs only in which volatile registers hold
 * the two areas and the constant 10 (and the order of two independent instructions); fn_80334A70
 * keeps &part->m_area[i] in a saved register across the operator-> call where retail recomputes
 * it with indexed loads (and colours its registers differently). */
#define BOOST_WEAK_PTR_CONVERTING_ASSIGNMENT
#include "Game/EggLastMech.h"
#include "Game/CharInfo.h"
#include "Game/MathTypes.h"
#include "boost/shared_ptr.hpp"
#include "boost/weak_ptr.hpp"
#include <dolphin/types.h>
#include "size_assert.h"

using EggLastMech::UnkPart_80334C78;

/* The game state (see CityBombSmall.cpp): the frame time at 0x18 and the stage at 0xAC (names
 * inferred). */
struct UnkGameState_8057D6A0 {
    /* 0x000 */ u8 field_0x0[0x18];
    /* 0x018 */ float m_frameTime;
    /* 0x01C */ u8 field_0x1C[0xAC - 0x1C];
    /* 0x0AC */ s32 m_stage;
    /* 0x0B0 */ u8 field_0xB0[0x150 - 0xB0];
};
SIZE_ASSERT(UnkGameState_8057D6A0, 0x150);

extern UnkGameState_8057D6A0 lbl_8057D6A0;

/* A vector returned through the out-of-line vector copy constructor (fn_800091EC; see
 * Game/WeaponBullet.h). */
struct UnkCopyVector3_800091EC {
    UnkCopyVector3_800091EC(float x, float y, float z) : x(x), y(y), z(z) {}
    UnkCopyVector3_800091EC(const Vec3f &v) {
        fn_800091EC(reinterpret_cast<Vec3f *>(this), &v);
    }

    float x;
    float y;
    float z;
};

/* A length (name inferred). As a call it is not a constant expression, so the constant is stored
 * by a static initializer, as in retail (see CityBombBig.cpp). */
inline float Span(float length) {
    return length;
}

/* Half the width of the middle row and column of the grid (name inferred). */
static float s_border = Span(200.0f);

/* The pointer to the time scale at 0x60 of a character info (struct name unknown). */
struct UnkTimeScale_60 {
    UnkTimeScale_60() {}

    float *m_scale;
};

/* Moves character index into area, restarting its time there (name inferred). */
inline void SetArea(UnkPart_80334C78 *part, int index, int area) {
    if (part->m_area[index] != area) {
        part->m_lastArea[index] = part->m_area[index];
        part->m_area[index] = area;
        part->m_areaTime[index] = 0.0f;
    }
}

/* The distance between two rows or two columns (name inferred). */
inline int Gap(int a, int b) {
    return a > b ? a - b : b - a;
}

namespace EggLastMech {

UnkPart_80334C78::UnkPart_80334C78(EggLastMech *mech) : m_mech(mech) {
    for (int i = 0; i < 2; i++) {
        m_area[i] = -1;
        m_lastArea[i] = -1;
        m_areaTime[i] = 0.0f;
    }
    m_characters[0] = m_mech->GetPlayer();
    m_characters[1] = m_mech->GetWeakPtr();
}

UnkPart_80334C78::~UnkPart_80334C78() {}

} // namespace EggLastMech

/* Updates the areas of the player and the boss (name inferred). */
extern "C" void fn_80334A70(UnkPart_80334C78 *part) {
    float border = s_border;
    float negBorder = -border;
    for (int i = 0; i < 2; i++) {
        boost::shared_ptr<CharInfo> character = part->m_characters[i].lock();
        if (character.get() == 0) {
            SetArea(part, i, -1);
            return;
        }
        int row;
        if (character.get()->m_position.z < negBorder) {
            row = 10;
        } else if (character.get()->m_position.z < border) {
            row = 20;
        } else {
            row = 30;
        }
        int column;
        if (character.get()->m_position.x < negBorder) {
            column = 1;
        } else if (character.get()->m_position.x < border) {
            column = 2;
        } else {
            column = 3;
        }
        int area = row + column;
        SetArea(part, i, area);
        UnkTimeScale_60 scale;
        scale = reinterpret_cast<const UnkTimeScale_60 &>(character->field_0x60);
        if (scale.m_scale != 0) {
            part->m_areaTime[i] += **reinterpret_cast<float *const *>(character.get()->field_0x60);
        } else {
            part->m_areaTime[i] += lbl_8057D6A0.m_frameTime;
        }
    }
}

/* How far the player's area is from the boss's: 0 the same, 2 the player in the middle, 4
 * opposite, 3 diagonal from a corner boss, else the grid distance (name and meanings inferred). */
extern "C" bool fn_8033496C(UnkPart_80334C78 *part);
extern "C" int fn_803349A0(UnkPart_80334C78 *part) {
    int player = part->m_area[0];
    int boss = part->m_area[1];
    if (player == boss) {
        return 0;
    }
    if (player == 22) {
        return 2;
    }
    int bossRow = boss / 10;
    int bossColumn = boss % 10;
    if (player == (4 - bossRow) * 10 + (4 - bossColumn)) {
        return 4;
    }
    int playerRow = player / 10;
    int playerColumn = player % 10;
    if (fn_8033496C(part) && playerRow != bossRow && playerColumn != bossColumn) {
        return 3;
    }
    int rows = Gap(playerRow, bossRow);
    return rows + Gap(playerColumn, bossColumn);
}

/* Whether the boss stands in a corner area (name inferred). */
extern "C" bool fn_8033496C(UnkPart_80334C78 *part) {
    int area = part->m_area[1];
    if (area == 11 || area == 13 || area == 31 || area == 33) {
        return true;
    }
    return false;
}

/* The centre of the arena: raised in stage 0x28 (name and meaning inferred). */
extern "C" UnkCopyVector3_800091EC fn_80334918(void) {
    if (lbl_8057D6A0.m_stage == 0x28) {
        return UnkCopyVector3_800091EC(0.0f, 20.0f, 0.0f);
    }
    return lbl_805E07A0;
}
