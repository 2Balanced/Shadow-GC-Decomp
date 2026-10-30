/* The motion of an enemy (UnkMotion_801A4D28, class name unknown; Game/EnemyRender.cpp's
 * EnemyRender::Impl holds one): a motion player (_stMtnPlayWork) loaded for a model, the base of
 * its motion numbers, the bone matrices asked for so far (a map from bone id to matrix, refreshed
 * on each update) and the event callback the motion's events go to. Game/EnemyRender.cpp declares
 * the class with only what it uses; its functions keep their address names (the callers use them).
 * The map is UnkTree (Game/UnkSingletons.h), the tree behind the library's map, whose instances
 * this unit carries. File name inferred; names marked "(name inferred)" are not original.
 * Definitions are in reverse address order (deferred inlining emits functions last-defined
 * first).
 *
 * NonMatching: all 62 functions and all data match, but the unit does not link in place. GC/2.7
 * emits boost::function1's operator() (retail 0x801A5020, between UnkTree::find and
 * UnkTree::destroy) after the tree's constructor instances (0x801A5094..0x801A52D8) instead.
 * Moving Call's definition, including the tree's members first and an extra reference did not
 * change that order. */
#define UNK_TREE_MAP
#define UNK_TREE_ITERATE
/* boost::function's operator() puts a bad_function_call on the stack (see <stdexcept>). */
#define MSL_RUNTIME_ERROR_STRING

#include "Game/UnkSingletons.h"
#include "Game/Enemy.h"
#include "Game/EnemyRender.h"
#include "Game/MathTypes.h"
#include <dolphin/types.h>
#include "size_assert.h"

struct _stMtnPlayWork;

extern "C" {
/* The motion player (behaviour inferred from the callers): create one for a model and free it, its
 * position (move by, get, get from the root), a bone's node, the current motion number, play a
 * motion (with a blend or at once), set a track's state, step it, its root matrix, and the next
 * event since the last call (src 0 continues the last). */
_stMtnPlayWork *fn_8041B908(void *model, int arg);
void fn_8041B884(_stMtnPlayWork *work);
void fn_80415BDC(_stMtnPlayWork *work, const Vec3f *delta);
void fn_80415DBC(_stMtnPlayWork *work, Vec3f *position);
void fn_80415E34(_stMtnPlayWork *work, Vec3f *position);
void *fn_80415E70(_stMtnPlayWork *work, u32 bone);
u32 fn_80415CDC(_stMtnPlayWork *work, int track);
void fn_804165D8(_stMtnPlayWork *work, int arg1, int arg2, int arg3);
void fn_80416148(_stMtnPlayWork *work, int track, float blend);
void fn_80416198(_stMtnPlayWork *work, int track);
void fn_80417568(_stMtnPlayWork *work, u16 motion);
int fn_80416F28(_stMtnPlayWork *work, int track, int arg2, int arg3);
void fn_80416DC8(_stMtnPlayWork *work, int track);
void fn_804160F8(_stMtnPlayWork *work, int track, float dt);
void fn_804172D8(_stMtnPlayWork *work, const void *node);
void fn_8041734C(_stMtnPlayWork *work, const void *node);
void fn_80415EA4(_stMtnPlayWork *work);
int fn_80415854(_stMtnPlayWork *work, int arg1, u16 *type, EnemyMotionEventArg::Data **data, int arg4);
/* A node's matrix and the node form of a matrix (see Game/BkSoldier.cpp). */
void fn_80016370(Mtx34f *matrix, void *node);
void fn_800163F4(void *node, const Mtx34f *matrix);
/* Sets the translation of a matrix (behaviour inferred). */
void fn_8040FA20(Mtx34f *matrix, const Vec3f *position);
}

/* A node's matrix in the motion player's form (struct name inferred; 0x30 bytes). */
struct UnkMotionNode {
    /* 0x00 */ u8 field_0x0[0x30];
};

/* The motion's event callback (class name unknown; Game/EnemyRender.cpp, which has its assignment
 * and reset, declares it deriving from the function): it holds the function as a member, whose
 * destructor its own calls as a complete object. */
class UnkEnemyMotionCallback {
  public:
    /* Calls the callback if one is set (name inferred). */
    void Call(EnemyMotionEventArg &arg);

    /* 0x0 */ EnemyMotionCallback m_function;
};

/* The values of the motion's map: a bone id and its matrix (names inferred). */
struct UnkTreeValue_801A4D28 {
    typedef u32 KeyType;
    typedef Matrix34 MappedType;

    UnkTreeValue_801A4D28(const KeyType &key, const MappedType &value) : first(key), second(value) {}

    /* 0x00 */ const KeyType first;
    /* 0x04 */ MappedType second;
};

/* The bone matrices of the motion by bone id (class name unknown). */
struct UnkMap_801A4D28 {
    typedef UnkTree<UnkTreeValue_801A4D28> Tree;

    UnkMap_801A4D28() : m_tree(Tree::KeyCompare(), Tree::ValueAllocator()) {}

    Matrix34 &operator[](const u32 &bone) {
        return m_tree.find_or_insert(bone).second;
    }
    Tree::Iterator find(const u32 &bone) {
        return m_tree.find(bone);
    }
    Tree::Iterator begin() {
        return m_tree.begin();
    }
    Tree::Iterator end() {
        return m_tree.end();
    }

    Tree m_tree;
};

class UnkMotion_801A4D28 {
  public:
    UnkMotion_801A4D28();
    ~UnkMotion_801A4D28();

    /* Frees the motion player (name inferred). */
    void Release() {
        if (m_work != 0) {
            fn_8041B884(m_work);
            m_work = 0;
        }
    }

    /* The rotation matrix of the Euler angles rotation, Y then X then Z (name inferred). */
    Matrix34 Rotation(const Vec3f *rotation);

    /* 0x00 */ _stMtnPlayWork *m_work;
    /* 0x04 */ void *m_model;
    /* 0x08 */ u32 m_base; /* the motion number of the first motion */
    /* 0x0C */ bool m_active;
    /* 0x10 */ UnkMap_801A4D28 m_bones;
    /* 0x20 */ UnkEnemyMotionCallback m_callback;
};
SIZE_ASSERT(UnkMotion_801A4D28, 0x2C);

UnkMotion_801A4D28::UnkMotion_801A4D28() : m_work(0), m_model(0), m_base(0), m_active(true) {}

UnkMotion_801A4D28::~UnkMotion_801A4D28() {
    Release();
}

/* Loads the motion player for a model (name inferred). */
extern "C" void fn_801A4B30(UnkMotion_801A4D28 *motion, void *model, u32 base, int arg3) {
    motion->m_model = model;
    motion->m_base = base;
    if (motion->m_work == 0) {
        motion->m_work = fn_8041B908(model, arg3);
        fn_80416F28(motion->m_work, 0, 0xB, 0);
    }
}

extern "C" void fn_801A4AF0(UnkMotion_801A4D28 *motion) {
    motion->Release();
}

inline void UnkEnemyMotionCallback::Call(EnemyMotionEventArg &arg) {
    if (!m_function.empty()) {
        m_function(arg);
    }
}

/* Refreshes the bone matrices asked for and passes the motion's new events to the callback (name
 * inferred). */
extern "C" void fn_801A4934(UnkMotion_801A4D28 *motion) {
    if (motion->m_work != 0 && motion->m_active) {
        fn_80415EA4(motion->m_work);
        for (UnkMap_801A4D28::Tree::Iterator it = motion->m_bones.begin(); it != motion->m_bones.end(); ++it) {
            void *node = fn_80415E70(motion->m_work, (*it).first);
            if (node != 0) {
                fn_80016370(&motion->m_bones[(*it).first], node);
            }
        }
        if (!motion->m_callback.m_function) {
            return;
        }
        u16 type = 0;
        EnemyMotionEventArg::Data *data = 0;
        _stMtnPlayWork *work = motion->m_work;
        while (fn_80415854(work, 0, &type, &data, 0) != 0) {
            EnemyMotionEventArg arg;
            arg.m_type = type;
            arg.m_data = data;
            work = 0;
            motion->m_callback.Call(arg);
        }
    }
}


/* Plays motion id (from the first motion), blended over blend frames if blend is positive (name
 * inferred). */
extern "C" void fn_801A4898(UnkMotion_801A4D28 *motion, u16 id, int arg2, float blend) {
    if (blend > 0.0f) {
        fn_804165D8(motion->m_work, arg2, -1, -1);
        fn_80416148(motion->m_work, 0, blend);
    } else {
        fn_80416198(motion->m_work, 0);
    }
    fn_80417568(motion->m_work, id + motion->m_base - 1);
}

/* Steps the motion by dt (name inferred). */
extern "C" void fn_801A4848(UnkMotion_801A4D28 *motion, float dt) {
    fn_80416DC8(motion->m_work, 0);
    fn_804160F8(motion->m_work, 0, dt);
}

/* Whether the motion has ended (name inferred). */
extern "C" bool fn_801A4810(UnkMotion_801A4D28 *motion) {
    return fn_80416F28(motion->m_work, 0, 3, 1) != 0;
}

/* The matrix of bone, kept up to date from then on (name inferred). */
extern "C" Mtx34f *fn_801A46E4(UnkMotion_801A4D28 *motion, u32 bone) {
    void *node = fn_80415E70(motion->m_work, bone);
    if (node == 0) {
        return 0;
    }
    if (motion->m_bones.find(bone) == motion->m_bones.end()) {
        fn_80016370(&motion->m_bones[bone], node);
    }
    return &motion->m_bones[bone];
}

extern "C" void fn_801A46C0(UnkMotion_801A4D28 *motion, Vec3f *position) {
    fn_80415E34(motion->m_work, position);
}

extern "C" void fn_801A469C(UnkMotion_801A4D28 *motion, Vec3f *position) {
    fn_80415DBC(motion->m_work, position);
}

/* Sets the root matrix from a position and Euler angles (name inferred). */
extern "C" void fn_801A4604(UnkMotion_801A4D28 *motion, const Mtx34f *matrix);
extern "C" void fn_801A4640(UnkMotion_801A4D28 *motion, const Vec3f *position, const Vec3f *rotation) {
    Matrix34 matrix = motion->Rotation(rotation);
    fn_8040FA20(&matrix, position);
    fn_801A4604(motion, &matrix);
}

/* Sets the root matrix (name inferred). */
extern "C" void fn_801A4604(UnkMotion_801A4D28 *motion, const Mtx34f *matrix) {
    UnkMotionNode node;
    fn_800163F4(&node, matrix);
    fn_804172D8(motion->m_work, &node);
}

extern "C" void fn_801A45E0(UnkMotion_801A4D28 *motion, const Vec3f *delta) {
    fn_80415BDC(motion->m_work, delta);
}

/* Sets the root's rotation from Euler angles (name inferred). */
extern "C" void fn_801A4584(UnkMotion_801A4D28 *motion, const Vec3f *rotation) {
    Matrix34 matrix = motion->Rotation(rotation);
    UnkMotionNode node;
    fn_800163F4(&node, &matrix);
    fn_8041734C(motion->m_work, &node);
}

enum UnkMotionMode { kMotionLoop = 0, kMotionOnce = 2 };

/* Sets whether the first track loops (name inferred). */
extern "C" void fn_801A4548(UnkMotion_801A4D28 *motion, bool loop) {
    fn_80416F28(motion->m_work, 0, 1, loop ? kMotionLoop : kMotionOnce);
}

/* Stops or restarts both tracks (name inferred). */
extern "C" void fn_801A44DC(UnkMotion_801A4D28 *motion, bool stopped) {
    fn_80416F28(motion->m_work, 0, 0, stopped ? kMotionLoop : kMotionOnce);
    fn_80416F28(motion->m_work, 1, 0, stopped ? kMotionLoop : kMotionOnce);
}

/* The current motion number, counted from the first motion (name inferred). */
extern "C" u16 fn_801A4494(UnkMotion_801A4D28 *motion) {
    return (u16)fn_80415CDC(motion->m_work, 0) - motion->m_base + 1;
}

Matrix34 UnkMotion_801A4D28::Rotation(const Vec3f *rotation) {
    Matrix34 matrix;
    fn_8040B6F8(&matrix);
    fn_8040B848(&matrix, rotation->y);
    fn_8040B884(&matrix, rotation->x);
    fn_8040B80C(&matrix, rotation->z);
    return matrix;
}

#include "Game/UnkTreeMap.h"
#include "Game/UnkTreeDestroy.h"
