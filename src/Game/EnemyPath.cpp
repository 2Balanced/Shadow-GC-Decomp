/* The path an enemy follows (UnkEnemyPath_801A6844, class name unknown; Game/EnemyFlyer.h): a
 * PathCtrl taken from the stage's path manager, with its length, the direction it is followed in
 * and the code that places the enemy along it. Game/FlyerCommonBase.cpp calls its functions by
 * their address names. File name inferred; names marked "(name inferred)" are not original.
 * Definitions are in reverse address order (deferred inlining emits functions last-defined
 * first).
 *
 * NonMatching: 5 of 6 functions and all data match. In fn_801A6688 retail loads the three words
 * of the rotation it passes by value into r6, r7 and r0 before storing them; GC/2.7 reuses r0 for
 * the last two, for every source form tried. */
/* The fields of UnkEnemyPath_801A6844 (Game/EnemyFlyer.h). */
#define ENEMY_PATH_UNIT

#include "Game/EnemyFlyer.h"
#include "Game/MathTypes.h"
#include "Game/PathCtrl.h"

using SonicteamUSA::System::PathCtrl;

/* The path manager fn_8001B494 returns (class name unknown; see Game/EggBalloonController.cpp). */
class UnkPathManager_8001B494;

/* Two vectors (struct name unknown; see Game/StageManager.cpp). */
struct UnkVecPair_8001C4B8 {
    /* 0x00 */ float a[3];
    /* 0x0C */ float b[3];
};

extern "C" {
/* The resource loader and a model of a slot (see Game/EnemyModel.cpp). */
void *fn_800122D4(void);
void *fn_8001277C(void *loader, s32 slot, int model);
/* The path manager, and the path functions (see Game/EggBalloonController.cpp; behaviours
 * inferred): whether a path is set, rewind it to its start or its end, its length, place a point
 * along it, take a path from a model and give it back. */
UnkPathManager_8001B494 *fn_8001B494(void);
bool fn_800ABDCC(PathCtrl *path);
void fn_80018304(PathCtrl *path);
void fn_800181BC(PathCtrl *path);
float fn_800183D8(PathCtrl *path);
bool fn_8001C91C(UnkPathManager_8001B494 *manager, PathCtrl *path, int backward, Vec3f *position,
                 Vec3f *rotation, int arg5, float distance);
void fn_8001DB08(UnkPathManager_8001B494 *manager, void *model, Vec3f *position, Vec3f rotation,
                 int arg4, UnkEnemyPath_801A6844 *path, const UnkVecPair_8001C4B8 *axes);
void fn_8001E2DC(UnkPathManager_8001B494 *manager, UnkEnemyPath_801A6844 *path);
/* The axes a path is placed with by default (struct name unknown). */
extern const UnkVecPair_8001C4B8 lbl_80570B50;
UnkVecPair_8001C4B8 *fn_8001C4B8(UnkVecPair_8001C4B8 *pair, const Vec3f *a, const Vec3f *b);
/* The angles of a rotation matrix (see Game/EggBalloonController.cpp). */
void fn_800494F8(Vec3f *rotation, const Mtx34f *matrix);
}

/* Gives the path back to the path manager (name inferred). */
extern "C" void fn_801A6648(UnkEnemyPath_801A6844 *path);
/* The model of the stage's path index (name inferred). */
extern "C" void *fn_801A6790(UnkEnemyPath_801A6844 *path, int index);

UnkEnemyPath_801A6844::UnkEnemyPath_801A6844() {
    field_0x58 = 0.0f;
}

UnkEnemyPath_801A6844::~UnkEnemyPath_801A6844() {
    fn_801A6648(this);
}

extern "C" void *fn_801A6790(UnkEnemyPath_801A6844 *path, int index) {
    s8 slots[9] = {2, 3, 4, 5, 6, 7, 8, 9, 10};
    return fn_8001277C(fn_800122D4(), 6, slots[index]);
}

/* Takes path pathId of the stage and puts position and rotation at its start (name inferred). */
extern "C" void fn_801A6688(UnkEnemyPath_801A6844 *path, Vec3f *position, Vec3f *rotation, int pathId,
                            bool mirror, int backward) {
    fn_801A6648(path);
    UnkPathManager_8001B494 *manager = fn_8001B494();
    void *model = fn_801A6790(path, pathId);
    UnkVecPair_8001C4B8 pair;
    fn_8001DB08(manager, model, position, *rotation, 2, path,
                mirror ? fn_8001C4B8(&pair, &lbl_805E07A0, &Vec3f(1.0f, 0.0f, 0.0f)) : &lbl_80570B50);
    fn_80018304(&path->m_path);
    path->field_0x58 = fn_800183D8(&path->m_path);
    if (backward == 0) {
        path->m_backward = 0;
    } else {
        path->m_backward = 1;
    }
    fn_8040CE60(&path->field_0x60);
}

extern "C" void fn_801A6648(UnkEnemyPath_801A6844 *path) {
    if (fn_800ABDCC(&path->m_path)) {
        fn_8001E2DC(fn_8001B494(), path);
    }
}

/* Places position and rotation distance along the path; false past its end (name inferred). */
extern "C" bool fn_801A6588(UnkEnemyPath_801A6844 *path, Vec3f *position, Vec3f *rotation,
                            float distance) {
    bool result = false;
    if (fn_800ABDCC(&path->m_path)) {
        if (path->m_backward == 0) {
            fn_80018304(&path->m_path);
        } else {
            fn_800181BC(&path->m_path);
        }
        result = fn_8001C91C(fn_8001B494(), &path->m_path, path->m_backward, position, rotation, 1,
                             distance);
        if (rotation != 0) {
            Mtx34f matrix;
            fn_8040B6F8(&matrix);
            fn_8001E070(&matrix, rotation, 1);
            fn_800494F8(rotation, &matrix);
        }
    }
    return result;
}
