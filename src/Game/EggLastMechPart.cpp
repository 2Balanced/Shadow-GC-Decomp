/* EggLastMech::UnkPart_8032D6FC (class name unknown; declared in Game/EggLastMech.h): a path the
 * boss EggLastMech follows. Its constructor takes the path manager's path 1; fn_8032D674 starts it
 * from an offset, and fn_8032D518 moves the boss along it each frame by the distance to the path's
 * point plus the boss's speed, returning the target and, on request, the facing direction. The file
 * name and all names are inferred; the two functions Game/EggLastMechAIState.cpp calls keep their fn_
 * names. Definitions are in reverse address order (deferred inlining emits functions last-defined
 * first). */
#include "Game/EggLastMech.h"
#include "Game/MathTypes.h"
#include "size_assert.h"

using EggLastMech::UnkPart_8032D6FC;
using SonicteamUSA::System::PathCtrl;

class UnkPathManager_8001B494;

/* fn_8040CD74 called as returning a - b by value (it stores the result through its first
 * argument). */
typedef Vec3f (*VectorSubtractFn)(const Vec3f *a, const Vec3f *b);

extern "C" {
/* Paths (behaviour inferred, as in Game/Coaster.cpp): the path manager, taking path id, the
 * position, moving a position distance along a path (reporting whether it reached the end), and
 * placing the path at a character's position plus offset. */
UnkPathManager_8001B494 *fn_8001B494(void);
bool fn_8001C7B4(UnkPathManager_8001B494 *manager, PathCtrl *path, u32 id);
Vec3f fn_80018634(PathCtrl *path);
bool fn_8001C91C(UnkPathManager_8001B494 *manager, PathCtrl *path, int direction, Vec3f *position,
                 Vec3f *rotation, int arg5, float distance);
bool fn_80018150(PathCtrl *path, CharInfo *owner, const Vec3f *offset, s32 *direction,
                 float arg4);
/* matrix * point. */
Vec3f fn_8040B67C(const Mtx34f *matrix, const Vec3f *point);

void fn_8032D518(UnkPart_8032D6FC *part, Vec3f *target, Vec3f *direction);
void fn_8032D674(UnkPart_8032D6FC *part, const Vec3f *offset);
}

namespace EggLastMech {

UnkPart_8032D6FC::UnkPart_8032D6FC(EggLastMech *mech) : m_mech(mech) {
    fn_8001C7B4(fn_8001B494(), &m_path, 1);
}

UnkPart_8032D6FC::~UnkPart_8032D6FC() {}

} // namespace EggLastMech

/* Places the path at the boss's position plus offset. */
void fn_8032D674(UnkPart_8032D6FC *part, const Vec3f *offset) {
    fn_80018150(&part->m_path, part->m_mech, offset, &part->m_direction, 0.0f);
}

/* Moves along the path (see the file comment). */
void fn_8032D518(UnkPart_8032D6FC *part, Vec3f *target, Vec3f *direction) {
    if (fn_80018150(&part->m_path, part->m_mech, &lbl_805E07A0, 0, 0.0f)) {
        Vec3f position;
        fn_800091EC(&position, &(const Vec3f &)fn_80018634(&part->m_path));
        float distance =
            fn_8040CF50(&((VectorSubtractFn)fn_8040CD74)(&position, &part->m_mech->m_position));
        Vec3f rotation(0.0f, 0.0f, 0.0f);
        fn_8001C91C(fn_8001B494(), &part->m_path, part->m_direction, target, &rotation, 1,
                    distance + fn_8040CF50(&part->m_mech->field_0x18) * fn_8019FFE0(part->m_mech));
        if (direction) {
            Mtx34f matrix;
            fn_8040B6F8(&matrix);
            fn_8001E070(&matrix, &rotation, 1);
            fn_800091D0(direction, &fn_8040B67C(&matrix, &Vec3f(0.0f, 0.0f,
                                                                        part->m_direction == 1 ? 1.0f : -1.0f)));
        }
    }
}
