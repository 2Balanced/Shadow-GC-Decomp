/* PJSCamera::StageCameraPostProcess (class name and base from RTTI; Game/StageCameraPostProcess.h):
 * the stage camera's task that fades the player when the camera comes close and makes the
 * characters on the line from the player to the camera transparent. File name from the class;
 * other names are inferred. Definitions are in reverse address order (deferred inlining emits
 * functions last-defined first).
 *
 * Not linking yet: every function and every data section matches, but as in
 * Game/ItemIconManagerTask.cpp GC/2.7 emits the vector members instantiated at the end of the unit
 * class by class (erase, insert_n, destroy_all, reserve and the destructor for the weak pointers,
 * then the same for the shared pointers), while retail emits erase, insert_n and reserve of both
 * vectors first and destroy_all and the destructors of both after them (0x801CE3DC..0x801CE5C4).
 * Moving reserve, insert_n and erase to a class of their own (between the vector and its storage,
 * before the storage, or as an empty base of it) did not change GC/2.7's order. */
#define MSL_VECTOR_MIDDLE
#define MSL_VECTOR_ERASE
#define MSL_VECTOR_DESTROY_ALL_LATE
#define MSL_ALGORITHM_COPY
#define MSL_ALGORITHM_REMOVE_IF
#define MSL_ALGORITHM_FIND
#define MSL_LIST_ACCESSORS
#include <vector>
#include <boost/bind.hpp>

#include "Game/StageCameraPostProcess.h"
#include "Game/CameraManagerUnit.h"
#include "Game/CharaColli.h"
#include "Game/CharCommand.h"
#include "Game/CharInfo.h"
#include "Game/MathTypes.h"
#include "Game/StageCameraManager.h"
#include "Game/Task.h"
#include "boost/shared_ptr.hpp"
#include "boost/weak_ptr.hpp"
#include <dolphin/types.h>
#include "size_assert.h"

using PJSCamera::StageCameraManager;
using PJSCamera::StageCameraPostProcess;
using SonicteamUSA::System::Camera::CameraManagerUnit;
using SonicteamUSA::System::Camera::CameraRequest;
using SonicteamUSA::System::CharaColli::CharaColliBase;

namespace SonicteamUSA {
namespace System {
namespace Collision {
class CollisionResult;

/* CollisionResultList (class name from the RTTI of its shared_ptr control block): the hits of a
 * query (see Game/PJSCameraCollisionQuery.cpp). */
class CollisionResultList {
  public:
    /* 0x00 */ u8 field_0x0[0x8];
    /* 0x08 */ CollisionResult *m_first;
};
} // namespace Collision
} // namespace System
} // namespace SonicteamUSA

using SonicteamUSA::System::Collision::CollisionResultList;

/* The camera manager (class name unknown; see Game/CameraManagerUnit.h). */
class UnkCameraManager_80009548;

/* A line for the collision queries (see Game/PJSCameraCollisionQuery.cpp). */
struct UnkLine_80020A20 {
    u8 field_0x0[0x18];
};

/* A hit of a query and an iterator over them (see Game/BirdBase.cpp). */
struct UnkHitNode_800587A0;

struct UnkHitIter_80027374 {
    UnkHitIter_80027374(UnkHitNode_800587A0 *node) : m_node(node) {}

    UnkHitNode_800587A0 *m_node;
};

/* A candidate a query's filter looks at (see Game/PJSCameraCollisionQuery.cpp). */
struct UnkCollisionCandidate;
typedef bool (*UnkCollisionFilter_80021934)(UnkCollisionCandidate *candidate);

extern "C" {
/* The camera manager, the camera unit of a screen and the eye of its placement (see
 * Game/StageCameraManager.cpp). */
UnkCameraManager_80009548 *fn_80009548(void);
boost::shared_ptr<CameraManagerUnit> fn_8000D0F0(UnkCameraManager_80009548 *manager, int index);
const Vec3f *fn_80010244(UnkCameraManager_80009548 *manager, int index);

/* A player's character (see Game/VibrationManagerUnit.cpp). */
boost::weak_ptr<CharInfo> fn_800A8C54(UnkCharaList_80074194 *list, int player);

/* The collision world, a line, the hits of a line (see Game/PJSCameraCollisionQuery.cpp) and the
 * filter of the post process's line (Game/PJSCameraCollisionQuery.cpp). */
void *fn_80020D40(void);
void *fn_80020A20(UnkLine_80020A20 *line, const Vec3f *start, const Vec3f *end);
CollisionResultList *fn_80021FE4(void *world, const UnkLine_80020A20 *line, int arg2, int arg3,
                                 UnkCollisionFilter_80021934 filter, int arg5);
bool fn_801C7D04(UnkCollisionCandidate *candidate);
/* The hits' first node and end, an iterator's increment, a hit's collider and the character a
 * collider belongs to (see Game/BirdBase.cpp). */
UnkHitNode_800587A0 *fn_8001EE04(CollisionResultList *hits);
UnkHitNode_800587A0 *fn_8001EDEC(CollisionResultList *hits);
UnkHitIter_80027374 fn_80027374(UnkHitIter_80027374 *it, int);
boost::weak_ptr<CharaColliBase> fn_800587A0(UnkHitNode_800587A0 *node);
void *fn_80021B08(void);
boost::weak_ptr<CharInfo> fn_800C4568(void *manager, boost::weak_ptr<CharaColliBase> collider);
/* shared_ptr<CollisionResultList>'s pointer, out of line (see Game/CameraModeGeneral.cpp). */
CollisionResultList *fn_800CFDE4(const boost::shared_ptr<CollisionResultList> *hits);
}

/* The current request on the camera unit of a screen (out of line here). */
inline boost::shared_ptr<CameraRequest> GetCurrentRequest(UnkCameraManager_80009548 *manager,
                                                          int index) {
    return fn_8000D0F0(manager, index)->GetCurrentRequest();
}

/* The game sets these in __sinit rather than in .sdata, as an initializer that is not a constant
 * expression does; the inline call reproduces that (see Game/CameraModeGeneral.cpp). */
inline float DynamicFloat(float value) {
    return value;
}

/* The distances from the camera between which the player fades (names inferred). */
static const float s_fadeFar = DynamicFloat(15.0f);
static const float s_fadeNear = DynamicFloat(6.0f);

const char *StageCameraPostProcess::s_taskName = "StageCameraPostProcess";

StageCameraPostProcess::StageCameraPostProcess(Task *parent, StageCameraManager *manager)
    : Task(parent), m_manager(manager), m_enabled(false), m_playerFaded(false) {
    m_name = s_taskName;
    m_hidden.clear();
    m_blocking.clear();
    m_hidden.reserve(8);
    m_blocking.reserve(8);
}

StageCameraPostProcess::~StageCameraPostProcess() {}

void StageCameraPostProcess::Reset() {
    if (m_playerFaded) {
        boost::shared_ptr<CharInfo> chara =
            fn_800A8C54(fn_80074194(), m_manager->GetPlayer()).lock();
        if (chara.get() != 0) {
            chara->PushCommand(new SetTransparentCommand(false, 1.0f, m_manager->GetPlayer()));
        }
        m_playerFaded = false;
    }
    for (std::vector<boost::weak_ptr<CharInfo> >::iterator it = m_hidden.begin();
         it != m_hidden.end(); ++it) {
        boost::shared_ptr<CharInfo> chara = it->lock();
        if (chara.get() != 0) {
            chara->PushCommand(new SetTransparentCommand(false, 1.0f, m_manager->GetPlayer()));
        }
    }
    m_hidden.clear();
    m_enabled = false;
}

void StageCameraPostProcess::Tick(float dt) {
    if (m_enabled) {
        int player = m_manager->GetPlayer();
        boost::shared_ptr<CharInfo> chara = fn_800A8C54(fn_80074194(), player).lock();
        if (chara.get() != 0) {
            boost::shared_ptr<CameraRequest> request = GetCurrentRequest(fn_80009548(), player);
            if (request.get() != 0 && request->m_priority >= 0 && request->m_priority <= 0x400) {
                std::vector<boost::weak_ptr<CharInfo> >::iterator last;
                const Vec3f *eye = fn_80010244(fn_80009548(), player);
                Vec3f offset;
                fn_8040CD74(&offset, &chara->m_position, eye);
                float distance = fn_8040CF50(&offset);
                if (distance < s_fadeFar) {
                    float alpha;
                    if (distance < s_fadeNear) {
                        alpha = 0.0f;
                    } else {
                        alpha = (distance - s_fadeNear) / (s_fadeFar - s_fadeNear);
                    }
                    chara->PushCommand(new SetTransparentCommand(true, alpha, player));
                    m_playerFaded = true;
                } else if (m_playerFaded) {
                    chara->PushCommand(new SetTransparentCommand(false, 1.0f, player));
                    m_playerFaded = false;
                }
                {
                    UnkLine_80020A20 line;
                    fn_80020A20(&line, &chara->m_position, eye);
                    boost::shared_ptr<CollisionResultList> hits(
                        fn_80021FE4(fn_80020D40(), &line, 2, 0xD, fn_801C7D04, 0));
                    if (hits.get() != 0) {
                        for (UnkHitIter_80027374 it(fn_8001EE04(fn_800CFDE4(&hits)));
                             it.m_node != fn_8001EDEC(fn_800CFDE4(&hits)); fn_80027374(&it, 0)) {
                            boost::shared_ptr<CharInfo> owner =
                                fn_800C4568(fn_80021B08(), fn_800587A0(it.m_node)).lock();
                            if (owner.get() != 0) {
                                m_blocking.push_back(owner);
                            }
                        }
                    }
                }
                last = m_hidden.end();
                m_hidden.erase(std::remove_if(m_hidden.begin(), m_hidden.end(),
                                              boost::bind(&StageCameraPostProcess::Restore, this, _1)),
                               last);
                s32 index;
                std::vector<boost::shared_ptr<CharInfo> >::iterator it;
                for (it = m_blocking.begin(), index = player; it != m_blocking.end(); ++it) {
                    std::vector<boost::weak_ptr<CharInfo> >::iterator hidden;
                    for (hidden = m_hidden.begin(); hidden != m_hidden.end(); ++hidden) {
                        boost::shared_ptr<CharInfo> other = hidden->lock();
                        if (*it == other) {
                            break;
                        }
                    }
                    if (hidden == m_hidden.end()) {
                        (*it)->PushCommand(new SetTransparentCommand(true, 0.4f, index));
                        m_hidden.push_back(boost::weak_ptr<CharInfo>(*it));
                    }
                }
                m_blocking.clear();
            }
        }
    }
}

bool StageCameraPostProcess::Restore(boost::weak_ptr<CharInfo> hidden) {
    boost::shared_ptr<CharInfo> chara = hidden.lock();
    if (chara.get() != 0) {
        std::vector<boost::shared_ptr<CharInfo> >::iterator it =
            std::find(m_blocking.begin(), m_blocking.end(), chara);
        if (it == m_blocking.end()) {
            chara->PushCommand(new SetTransparentCommand(false, 1.0f, m_manager->GetPlayer()));
            return true;
        }
        return false;
    }
    return true;
}

#include <vector_impl>
