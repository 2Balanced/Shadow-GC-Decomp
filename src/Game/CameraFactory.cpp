/* SonicteamUSA::System::Camera::CameraFactory (class name from RTTI; Game/CameraManagerUnit.h): the
 * camera manager's maker of camera modes and adjusts, kept in two std::maps by kind and type. The
 * constructor registers the camera's own modes (MtnViewer, Stop and User). The unit also holds the
 * first copies of the map's shared tree code (the rebalancing after an insertion, fn_8000CD64, and
 * its helpers), which other units call by address. File name from the class; other names are
 * inferred. Definitions are in reverse address order (deferred inlining emits functions
 * last-defined first).
 *
 * NonMatching: every function but fn_8000CD64 matches (99.8%: p and its grandparent swap r4 and r5
 * at the loop's top), and retail emits fn_8000CD64 and its helpers after all the
 * tree's template instances (0x8000CD64..0x8000D094), as the non-inline member of a class template
 * instantiated once would be; written as extern "C" functions (other units call them by these
 * names) GC/2.7 emits them either before the instances (non-inline) or after the first
 * insert_node (inline, as here). */
#define MSL_PAIR_CONVERTING_CONSTRUCTOR
#define MSL_TREE_ITERATOR_ASSIGN_CALL
#define MSL_TREE_FIXUP_INLINE
#define MSL_TREE_END_NODE_PER_TREE
#define CAMERAFACTORY_MAPS
#define MSL_LIST_ACCESSORS
#include <map>

#include "Game/CameraManagerUnit.h"
#include "Game/CameraModeUser.h"
#include "Game/CameraState.h"
#include "Game/MathTypes.h"

using SonicteamUSA::System::Camera::CameraAdjust;
using SonicteamUSA::System::Camera::CameraFactory;
using SonicteamUSA::System::Camera::CameraMode;

namespace SonicteamUSA {
namespace System {
namespace Camera {

/* The camera's own modes (Game/CameraModeMtnViewer.cpp, Game/CameraModeStop.cpp). */
class CameraModeMtnViewer {
  public:
    static CameraMode *Create(UnkCameraOwner_8000E9BC *owner, int screen);
};

class CameraModeStop {
  public:
    static CameraMode *Create(UnkCameraOwner_8000E9BC *owner, int screen);
};

} // namespace Camera
} // namespace System
} // namespace SonicteamUSA

/* The red-black tree's node links (see <map>): rebalancing after an insertion, a node's colour
 * (red when the parent link's low bit is set) and the two rotations (behaviour inferred). */
typedef std::__tree_node_base TreeNode;

namespace std {
extern "C" {
inline bool fn_8000CF60(TreeNode *node);
inline void fn_8000CF6C(TreeNode *node, bool red);
inline void fn_8000CF94(TreeNode *node, TreeNode *&root);
inline void fn_8000D014(TreeNode *node, TreeNode *&root);
}
} // namespace std

inline TreeNode *Parent(const TreeNode *node) {
    return (TreeNode *)(node->parent_ & ~1);
}

namespace std {

extern "C" inline void fn_8000CD64(TreeNode *x, TreeNode *root) {
    fn_8000CF6C(x, true);
    while (x != root && fn_8000CF60(Parent(x)) == true) {
        if (Parent(x) == Parent(Parent(x))->left_) {
            TreeNode *y = Parent(Parent(x))->right_;
            if (y != 0 && fn_8000CF60(y) == true) {
                fn_8000CF6C(Parent(x), false);
                fn_8000CF6C(y, false);
                x = Parent(Parent(x));
                fn_8000CF6C(x, true);
            } else {
                if (x == Parent(x)->right_) {
                    x = Parent(x);
                    fn_8000D014(x, root);
                }
                fn_8000CF6C(Parent(x), false);
                fn_8000CF6C(Parent(Parent(x)), true);
                fn_8000CF94(Parent(Parent(x)), root);
            }
        } else {
            TreeNode *y = Parent(Parent(x))->left_;
            if (y != 0 && fn_8000CF60(y) == true) {
                fn_8000CF6C(Parent(x), false);
                fn_8000CF6C(y, false);
                x = Parent(Parent(x));
                fn_8000CF6C(x, true);
            } else {
                if (x == Parent(x)->left_) {
                    x = Parent(x);
                    fn_8000CF94(x, root);
                }
                fn_8000CF6C(Parent(x), false);
                fn_8000CF6C(Parent(Parent(x)), true);
                fn_8000D014(Parent(Parent(x)), root);
            }
        }
    }
    fn_8000CF6C(root, false);
}

extern "C" inline bool fn_8000CF60(TreeNode *node) {
    return std::__tree_id(node->parent_ & 1);
}

extern "C" inline void fn_8000CF6C(TreeNode *node, bool red) {
    std::__tree_nothing();
    if (red) {
        node->parent_ |= 1;
    } else {
        node->parent_ &= ~1;
    }
}

/* Turns node's left child up into its place. */
extern "C" inline void fn_8000CF94(TreeNode *node, TreeNode *&root) {
    TreeNode *child = node->left_;
    if (root == node) {
        root = child;
    }
    node->left_ = child->right_;
    if (child->right_ != 0) {
        child->right_->set_parent(node);
    }
    child->parent_ = (node->parent_ & ~1) | (child->parent_ & 1);
    if (node == Parent(node)->left_) {
        Parent(node)->left_ = child;
    } else {
        Parent(node)->right_ = child;
    }
    child->right_ = node;
    node->set_parent(child);
}

/* Turns node's right child up into its place. */
extern "C" inline void fn_8000D014(TreeNode *node, TreeNode *&root) {
    TreeNode *child = node->right_;
    if (root == node) {
        root = child;
    }
    node->right_ = child->left_;
    if (child->left_ != 0) {
        child->left_->set_parent(node);
    }
    child->parent_ = (node->parent_ & ~1) | (child->parent_ & 1);
    if (node == Parent(node)->left_) {
        Parent(node)->left_ = child;
    } else {
        Parent(node)->right_ = child;
    }
    child->left_ = node;
    node->set_parent(child);
}

} // namespace std

namespace SonicteamUSA {
namespace System {
namespace Camera {

CameraFactory::CameraFactory() {
    m_modes.clear();
    m_adjusts.clear();
    AddMode(-0x3FF, "MtnViewer", CameraModeMtnViewer::Create, 0);
    AddMode(-0x400, "Stop", CameraModeStop::Create, 0);
    AddMode(-0x3FE, "User", CameraModeUser::Create, 0);
}

CameraFactory::~CameraFactory() {}

} // namespace Camera
} // namespace System
} // namespace SonicteamUSA

extern "C" {
const CameraFactory::UnkModeEntry_8000B904 *fn_8000B63C(const CameraFactory *factory, int kind);
const CameraFactory::UnkAdjustEntry_8000B76C *fn_8000B50C(const CameraFactory *factory, int type);
}

/* Makes a mode of a kind for owner, and an adjust of a type from a blend. */
extern "C" CameraMode *fn_8000BB00(CameraFactory *factory, int kind, UnkCameraOwner_8000E9BC *owner,
                                   int screen) {
    return fn_8000B63C(factory, kind)->m_create(owner, screen);
}

extern "C" CameraAdjust *fn_8000BAB4(CameraFactory *factory, int type, const UnkCameraBlend_8000B2C0 &blend) {
    const CameraFactory::UnkAdjustEntry_8000B76C *entry = fn_8000B50C(factory, type);
    if (entry->m_create != 0) {
        return entry->m_create(blend);
    }
    return 0;
}

namespace SonicteamUSA {
namespace System {
namespace Camera {

void CameraFactory::AddMode(int kind, const char *name, CreateModeFn create, int arg) {
    std::pair<ModeMap::iterator, bool> result;
    result = m_modes.insert(std::pair<int, UnkModeEntry_8000B904>(kind, UnkModeEntry_8000B904(name, create, arg)));
}

void CameraFactory::AddAdjust(int type, const char *name, CreateAdjustFn create) {
    std::pair<AdjustMap::iterator, bool> result;
    result = m_adjusts.insert(std::pair<int, UnkAdjustEntry_8000B76C>(type, UnkAdjustEntry_8000B76C(name, create)));
}

} // namespace Camera
} // namespace System
} // namespace SonicteamUSA

/* The maker of a kind of mode and of a type of adjust, or null. */
extern "C" const CameraFactory::UnkModeEntry_8000B904 *fn_8000B63C(const CameraFactory *factory, int kind) {
    CameraFactory::ModeMap::const_iterator it = factory->m_modes.find(kind);
    if (it == factory->m_modes.end()) {
        return 0;
    }
    return &(*it).second;
}

extern "C" const CameraFactory::UnkAdjustEntry_8000B76C *fn_8000B50C(const CameraFactory *factory, int type) {
    CameraFactory::AdjustMap::const_iterator it = factory->m_adjusts.find(type);
    if (it == factory->m_adjusts.end()) {
        return 0;
    }
    return &(*it).second;
}

#include <map_impl>
