/* ElecBar (class name from RTTI): the ELEC_BAR gadget, a row of "number of bars" electric bars
 * (BAR.BIN) that slide out of the owner's position, vertically or horizontally by the type
 * parameter, by "offset" over "moving second" seconds, and vanish for "hiding second" seconds; the
 * bars are spread over the cycle and "synchronize" shifts it. Each bar (ElecBarSub, class name from
 * the RTTI of its callback binders) fades in over the first "appearing ratio" of its way and out at
 * its end, carries a line a player can hang on (TouchLineCommand) while it is out, and drops its
 * rider when it hides. The bars live in a std::set of pointers. File name inferred from the class.
 * Names marked "(name inferred)" are not original. Definitions are in reverse address order
 * (deferred inlining emits functions last-defined first). */
/* ElecBar's override of the gadget's slot 0x14 takes the frame time (see Game/Gadget.h). */
#define GADGET_SLOT_14_TAKES_DT

#include <boost/bind.hpp>
#include <boost/function.hpp>
#include <cmath>
#include <cstdio>
#include <functional>
#include <memory>
#include <utility>

#include "Game/CharCommand.h"
#include "Game/CharCommandUnit.h"
#include "Game/CharInfo.h"
#include "Game/CharaColli.h"
#include "Game/Gadget.h"
#include "Game/GadgetCharCommands.h"
#include "Game/GadgetCharInfo.h"
#include "Game/GadgetColliDetectArgs.h"
#include "Game/GadgetModelInfo.h"
#include "Game/GadgetClassDesc.h"
#include "Game/GadgetUtil.h"
#include "Game/GeometryLine.h"
#include "Game/Player.h"
#include "Game/FileQueue.h"
#include "boost/shared_ptr.hpp"
#include "boost/weak_ptr.hpp"
#include "size_assert.h"

using SonicteamUSA::System::CharaColli::CharaColliBase;
using Sonicteam::System::Geometry::Line;

/* The handle fn_8015B8C4 wraps a collider in (class name unknown; layout as in CatchStick.cpp). */
class GadgetCharCollision {
  public:
    /* 0x00 */ boost::function1<void, GadgetColliDetectArgs &> m_onContact;
    /* 0x0C */ u8 field_0xC[0x24 - 0xC];
    /* 0x24 */ CharaColliBase *m_collider;
    /* 0x28 */ u32 field_0x28;
    /* 0x2C */ u32 m_flags; /* bit 1: the handle reports no contacts (inferred) */

    /* Turn the handle's contact reports on and off (names inferred). */
    void EnableContacts() {
        m_flags &= ~2;
    }
    void DisableContacts() {
        m_flags |= 2;
    }
};

/* A part of a player character fn_80078CEC returns (as in CatchStick.cpp). */
struct UnkPlayerPart_80078CEC {
    /* 0x00 */ u8 field_0x0[0xC];
    /* 0x0C */ Vec3f m_position;
};

/* RideCommand (class name from RTTI, base CharCommand): the command of type 0xB, which a character
 * sends to what it gets on; the character follows (layout from its use here). */
class RideCommand : public CharCommand {
  public:
    virtual ~RideCommand();

    /* 0x14 */ boost::weak_ptr<CharInfo> m_rider;
};

/* A colour of four bytes (as in ElecCristalWall.cpp). */
struct UnkColor_8040D2D8 {
    /* 0x0 */ u8 r;
    /* 0x1 */ u8 g;
    /* 0x2 */ u8 b;
    /* 0x3 */ u8 a;
};

/* The material animation of a model fn_801621F0 finds by name (as in ElecCristalWall.cpp). */
struct UnkMaterialAnim_801621F0 {
    /* 0x0 */ u8 field_0x0[0x8];
    /* 0x8 */ UnkColor_8040D2D8 color;
};

/* The fields of the game state used here (layout from its users; see FootingMovable.cpp). */
struct UnkGameState_8057D6A0 {
    /* 0x00 */ u8 field_0x0[0x94];
    /* 0x94 */ float m_time; /* seconds */
};

extern UnkGameState_8057D6A0 lbl_8057D6A0;

extern "C" {
/* The collider manager, and a capsule collider for charInfo (see CatchStick.cpp). */
void *fn_80021B08(void);
boost::shared_ptr<CharaColliBase> fn_800C78D4(void *manager, const boost::weak_ptr<CharInfo> &charInfo,
                                              int flags, int arg3, int arg4, float halfLength,
                                              float radius, const Vec3f *offset,
                                              const Vec3f *rotation, int arg9);
/* Wraps collider in a new handle, and advances the handle by dt (behaviour inferred). */
boost::shared_ptr<GadgetCharCollision> fn_8015B8C4(boost::shared_ptr<CharaColliBase> collider);
void fn_8015BE9C(GadgetCharCollision *handle, float dt);
/* Sets the handle's contact callback (an inline function emitted by CatchStick's unit). */
GadgetCharCollision *fn_800F5980(GadgetCharCollision *handle,
                                       const boost::function1<void, GadgetColliDetectArgs &> &callback);
/* Calls callback with every character in the collision event (behaviour inferred). */
void fn_8015AA84(ColliDetectArgs &args, void (*callback)(boost::shared_ptr<CharInfo> hit, void *user),
                 void *user);
/* A test on the player character that touched the bar (behaviour unknown). */
bool fn_800B7828(const boost::shared_ptr<CharInfo> &character);
/* Returns part index of player, and the distance of a point or a segment from a line (see
 * CatchStick.cpp). */
UnkPlayerPart_80078CEC *fn_80078CEC(Player::PlayerBase *player, int index);
float fn_8003C660(const Vec3f *point, const Line *line, int arg2);
float fn_8003C4C4(const void *segment, const Line *line, int arg2, int arg3);
/* GadgetModelInfo helpers (see ElecCristalWall.cpp): find the material animation named name, and
 * select the material named name. */
UnkMaterialAnim_801621F0 *fn_801621F0(GadgetModelInfo *model, const char *name);
void fn_8016239C(GadgetModelInfo *model, const char *name);
/* Plays sound id at position (behaviour inferred; the other arguments are unknown). */
void fn_801D54F4(void *manager, int id, const Vec3f *position, int arg3, int arg4);
/* A debug box at center of the given size, turned by rotation (behaviour inferred). */
void fn_802E0E5C(const Vec3f *center, const Vec3f *size,
                 const Vec3f *rotation, int arg3);
/* vector * scale, and a + b (behaviour inferred). */
Vec3f fn_8040CDA8(const Vec3f *vector, float scale);
Vec3f fn_8040CDD0(const Vec3f *a, const Vec3f *b);
}

/* fn_8040CD74 called as returning a - b by value (as in ChaoHolder.cpp). */
typedef Vec3f (*VectorSubtractFn)(const Vec3f *a, const Vec3f *b);

/* Copies a rotation one angle at a time (as in Bomb.cpp; name inferred). */
inline void CopyRotation(Vec3f &rotation, const Vec3f &value) {
    rotation.x = value.x;
    rotation.y = value.y;
    rotation.z = value.z;
}

/* a - b and a + b (names inferred). */
inline Vec3f SubtractVectors(const Vec3f &a, const Vec3f &b) {
    return ((VectorSubtractFn)fn_8040CD74)(&a, &b);
}

inline Vec3f AddVectors(const Vec3f &a, const Vec3f &b) {
    return fn_8040CDD0(&a, &b);
}

/* Sets all four bytes of a material's colour (name inferred). */
inline void SetColor(UnkMaterialAnim_801621F0 *material, u8 value) {
    material->color.r = value;
    material->color.g = value;
    material->color.b = value;
    material->color.a = value;
}

/* The red-black tree behind the bar set, reconstructed from the template instances this unit
 * emits (0x8015245C..0x80153478): a set of pointers ordered by address, the tree's link, rebalance
 * and iteration code shared at 0x8000CD64 and 0x80012E90. The original is the MSL <set>; its source
 * is not used here. Which members are written inline, and which call an inline helper so that they
 * stay out of line, reproduces the game's call structure (deferred inlining is one level deep:
 * a function that calls an inline function is not inlined itself). The tree's larger members are
 * defined at the end of the file, after their uses, as the game emits them last. Names are
 * invented. */
struct __tree_node_base;

extern "C" {
/* Rebalances the tree under root after x was linked in, and steps an iterator to the next node
 * (behaviour inferred from their bodies). */
void fn_8000CD64(__tree_node_base *x, __tree_node_base *root);
void fn_80012E90(__tree_node_base **iterator);
}

/* Returns x (an inline call that keeps its caller out of line). */
template <class T> inline T __tree_id(T x) {
    return x;
}

inline void __tree_nothing() {}

/* The end node: the root hangs off its left link. */
struct __tree_end_node {
    __tree_end_node() : left_(__tree_id<__tree_node_base *>(0)) {}

    /* 0x0 */ __tree_node_base *left_;
};

/* A node's links; the parent link keeps the colour in its low bit. */
struct __tree_node_base : public __tree_end_node {
    void set_parent(__tree_node_base *p) {
        parent_ = reinterpret_cast<u32>(p) | (parent_ & 1);
    }

    /* 0x4 */ __tree_node_base *right_;
    /* 0x8 */ u32 parent_;
};

template <class T> struct __tree_node : public __tree_node_base {
    /* 0xC */ T value_;
};

/* The allocators of the values and of the nodes (an allocation failure aborts). */
template <class T> class __tree_value_allocator {
  public:
    __tree_value_allocator() {}

    void construct(T *p, const T &value) {
        new ((void *)p) T(value);
    }
    void destroy(T *p) {
        __tree_nothing();
    }
};

template <class N> class __tree_node_allocator {
  public:
    __tree_node_allocator() {}

    N *allocate(std::size_t n, const void *hint = 0) {
        N *p = static_cast<N *>(::operator new(n * sizeof(N)));
        if (p == 0) {
            std::__msl_error("Memory allocation failure");
        }
        return p;
    }
    void deallocate(N *p, std::size_t n) {
        ::operator delete(__tree_id<void *>(p));
    }
};

template <class T> class __tree_iterator {
  public:
    __tree_iterator() {}
    __tree_iterator(__tree_node_base *n) : node_(n) {}
    __tree_iterator(const __tree_iterator &x) : node_(x.node_) {}
    __tree_iterator &operator=(const __tree_iterator &x) {
        node_ = __tree_id(x.node_);
        return *this;
    }

    T &operator*() const {
        return static_cast<__tree_node<T> *>(node_)->value_;
    }
    __tree_iterator &operator++() {
        fn_80012E90(&node_);
        return *this;
    }
    bool operator!=(const __tree_iterator &x) const {
        return node_ != x.node_;
    }

    /* 0x0 */ __tree_node_base *node_;
};

/* The value allocator and the size. */
template <class A, class S> struct __tree_size_base : public A {
    __tree_size_base(const A &a, S n) : second_(__tree_id(n)) {}

    /* 0x0 */ S second_;
};

template <class A, class S> struct __tree_size_pair : public __tree_size_base<A, S> {
    __tree_size_pair(const A &a, S n) : __tree_size_base<A, S>(a, n) {}

    A &first() {
        return *this;
    }
    S &second() {
        return this->second_;
    }
    const S &second() const {
        return this->second_;
    }
};

/* The node allocator and the end node. */
template <class A> struct __tree_end_pair : public A {
    __tree_end_pair(const A &a) {}

    A &first() {
        return *this;
    }
    __tree_end_node &second() {
        return second_;
    }

    /* 0x0 */ __tree_end_node second_;
};

/* The comparison and the leftmost node. */
template <class C> struct __tree_begin_pair : public C {
    __tree_begin_pair(const C &c) : C(c) {
        __tree_nothing();
    }

    C &first() {
        return *this;
    }
    __tree_node_base *&second() {
        return second_;
    }

    /* 0x0 */ __tree_node_base *second_;
};

template <class T, class C> class __tree {
  public:
    typedef std::size_t size_type;
    typedef __tree_node<T> node;
    typedef __tree_iterator<T> iterator;
    typedef __tree_value_allocator<T> value_allocator;
    typedef __tree_node_allocator<node> node_allocator;

    __tree(const C &comp, const value_allocator &a);
    ~__tree() {
        if (root() != 0) {
            destroy(static_cast<node *>(root()));
        }
    }

    iterator begin() {
        return iterator(begin_node());
    }
    iterator end() {
        return iterator(static_cast<__tree_node_base *>(header()));
    }
    size_type size() const {
        return size_ref();
    }
    size_type max_size() const {
        return std::__max_size_t();
    }
    std::pair<iterator, bool> insert_unique(const T &v);
    void clear();

  private:
    size_type &size_ref() {
        return size_.second();
    }
    const size_type &size_ref() const {
        return size_.second();
    }
    value_allocator &value_alloc() {
        return size_.first();
    }
    node_allocator &node_alloc() {
        return header_.first();
    }
    __tree_end_node *header() {
        return &header_.second();
    }
    __tree_node_base *&root() {
        return header()->left_;
    }
    __tree_node_base *&begin_node() {
        return begin_.second();
    }
    C &comp() {
        return begin_.first();
    }

    node *insert_node(__tree_node_base *parent, bool left, bool leftmost, const T &v);
    void destroy(node *n);

    /* 0x0 */ __tree_size_pair<value_allocator, size_type> size_;
    /* 0x4 */ __tree_end_pair<node_allocator> header_;
    /* 0x8 */ __tree_begin_pair<C> begin_;
};

template <class T, class C = std::less<T> > class __set {
  public:
    typedef __tree<T, C> tree_type;

    class iterator {
      public:
        iterator() {}
        iterator(const typename tree_type::iterator &i) : i_(i) {}
        iterator(const iterator &x) : i_(x.i_) {}
        iterator &operator=(const iterator &x) {
            i_ = x.i_;
            return *this;
        }

        T &operator*() const {
            return *i_;
        }
        iterator operator++(int) {
            iterator tmp(*this);
            ++i_;
            return tmp;
        }
        bool operator!=(const iterator &x) const {
            return i_ != x.i_;
        }

      private:
        /* 0x0 */ typename tree_type::iterator i_;
    };

    __set() : tree_(C(), typename tree_type::value_allocator()) {}
    ~__set() {}

    iterator begin() {
        return iterator(tree_.begin());
    }
    iterator end() {
        return iterator(tree_.end());
    }
    std::pair<iterator, bool> insert(const T &x) {
        std::pair<typename tree_type::iterator, bool> r = tree_.insert_unique(x);
        return std::pair<iterator, bool>(iterator(r.first), r.second);
    }
    void clear() {
        tree_.clear();
    }

  private:
    /* 0x0 */ tree_type tree_;
};

class ElecBarSub;

/* The parameters the owner supplies (names from the parameter table). */
struct ElecBarParams {
    /* 0x00 */ s32 type; /* 0 vertical, 1 horizontal */
    /* 0x04 */ float scale;
    /* 0x08 */ float offset; /* how far a bar slides out */
    /* 0x0C */ float movingSecond;
    /* 0x10 */ float hidingSecond;
    /* 0x14 */ float appearingRatio;
    /* 0x18 */ float synchronize;
    /* 0x1C */ s32 numberOfBars;
};
SIZE_ASSERT(ElecBarParams, 0x20);

class ElecBar : public Gadget {
  public:
    ElecBar(Task *parent, UnkPlacement_800C9CD0 *owner);
    virtual ~ElecBar();
    virtual void Behave(float dt);
    /* Updates the bars and draws the gadget's box (the debug counterpart of Behave,
     * inferred). */
    virtual void fn_800CBC1C(float dt);
    virtual void ReloadPlacement(UnkPlacement_800C9CD0 *owner);

    /* Creates the gadget under task layer 11 (name inferred). */
    static void Instantiate(UnkPlacement_800C9CD0 *owner);

    /* Method names are inferred. */
    /* Copies the owner's position, rotation and parameters, and derives the bars' timing. */
    void LoadFromOwner(UnkPlacement_800C9CD0 *owner) {
        fn_800C9F14(owner, &m_position);
        fn_800C9ED4(owner, &m_rotation);
        fn_800C9DBC(owner, &m_params, sizeof(m_params));
        if (0.0f != m_params.movingSecond) {
            m_speed = m_params.offset / m_params.movingSecond;
        } else {
            m_speed = 0.0f;
        }
        m_appearTime = m_params.movingSecond * m_params.appearingRatio;
        m_period = m_params.movingSecond + m_params.hidingSecond;
    }
    void CreateBars();
    void ClearBars();

    static const char *s_taskName;

    /* Member names are inferred. */
    /* 0x30 */ ElecBarParams m_params;
    /* 0x50 */ __set<ElecBarSub *> m_bars;
    /* 0x5C */ float m_appearTime; /* the time a bar takes to fade in */
    /* 0x60 */ float m_speed;
    /* 0x64 */ float m_period;
    /* 0x68 */ s32 m_barCount;
    /* 0x6C */ Vec3f m_position;
    /* 0x78 */ Vec3f m_rotation;
    /* 0x84 */ u32 pad_0x84;
};
SIZE_ASSERT(ElecBar, 0x88);

/* One bar (class name from the RTTI of its callback binders). */
class ElecBarSub {
  public:
    ElecBarSub(ElecBar *parent);
    ~ElecBarSub();

    /* Method names are inferred. */
    void Tick(float dt);
    /* Copies the gadget's position and rotation. */
    void UpdatePlacement();
    /* Places the model, the line and the collider at the bar's way out. */
    void UpdateMatrix();
    /* The command unit's callback: remembers or forgets the character riding the bar. */
    void OnCommand(CharCommand &command);
    /* The collider's callback. */
    void OnColliDetect(ColliDetectArgs &args);
    /* Not called anywhere (see its definition). */
    float IndexToFloat(int index);

    /* Member names are inferred. */
    /* 0x00 */ ElecBar *m_parent;
    /* 0x04 */ boost::shared_ptr<GadgetCharInfo> m_chara;
    /* 0x0C */ GadgetModelInfo *m_model;
    /* 0x10 */ boost::shared_ptr<GadgetCharCollision> m_handle;
    /* 0x18 */ boost::shared_ptr<Line> m_line;
    /* 0x20 */ boost::weak_ptr<CharInfo> m_rider;
    /* 0x28 */ s32 m_state; /* 0 appearing, 1 out, 2 hidden, 3 new */
    /* 0x2C */ float m_height; /* how far the bar is out */
    /* 0x30 */ float m_phase;  /* the time into the cycle */
    /* 0x34 */ float m_offset; /* the bar's share of the cycle */
};
SIZE_ASSERT(ElecBarSub, 0x38);

/* What the collision callback passes to the per-character hit function (name inferred). */
struct ElecBarHitContext {
    ElecBarHitContext() {}

    /* 0x0 */ boost::weak_ptr<GadgetCharInfo> m_chara;
    /* 0x8 */ boost::weak_ptr<Line> m_line;
};
SIZE_ASSERT(ElecBarHitContext, 0x10);

const char *ElecBar::s_taskName = "ElecBar";

/* The labels of the type parameter. */
static const char *s_typeLabels[2] = {"type< Vertical   >", "type< Horizontal >"};

ElecBar::ElecBar(Task *parent, UnkPlacement_800C9CD0 *owner)
    : Gadget(parent, owner), m_rotation(0.0f, 0.0f, 0.0f) {
    m_name = s_taskName;
    LoadFromOwner(owner);
    m_barCount = m_params.numberOfBars;
    CreateBars();
}

ElecBar::~ElecBar() {
    ClearBars();
}

void ElecBar::Behave(float dt) {
    __set<ElecBarSub *>::iterator it;
    for (it = m_bars.begin(); it != m_bars.end(); it++) {
        (*it)->Tick(dt);
    }
}

void ElecBar::ReloadPlacement(UnkPlacement_800C9CD0 *owner) {
    LoadFromOwner(owner);
    __set<ElecBarSub *>::iterator it;
    for (it = m_bars.begin(); it != m_bars.end(); it++) {
        (*it)->UpdatePlacement();
    }
    if (m_barCount != m_params.numberOfBars) {
        ClearBars();
        m_barCount = m_params.numberOfBars;
        CreateBars();
    }
}

void ElecBar::fn_800CBC1C(float dt) {
    __set<ElecBarSub *>::iterator it;
    for (it = m_bars.begin(); it != m_bars.end(); it++) {
        (*it)->Tick(dt);
    }

    Vec3f center;
    fn_800091EC(&center, &m_position);
    Vec3f size;
    size.x = 0.5f * (10.0f * m_params.scale);
    if (m_params.type == 0) {
        center.y += 0.5f * m_params.offset;
        size.y = 0.5f * m_params.offset;
        size.z = 2.0f;
    } else {
        center.z += 0.5f * -m_params.offset;
        size.y = 2.0f;
        size.z = 0.5f * m_params.offset;
    }
    fn_802E0E5C(&center, &size, &m_rotation, 0);
}

void ElecBar::CreateBars() {
    for (int i = 0; i < m_barCount; i++) {
        ElecBarSub *bar = new ElecBarSub(this);
        if (bar != 0) {
            bar->m_offset = (float)i * (1.0f / (float)m_params.numberOfBars);
            m_bars.insert(bar);
        }
    }
}

void ElecBar::ClearBars() {
    __set<ElecBarSub *>::iterator it;
    for (it = m_bars.begin(); it != m_bars.end(); it++) {
        delete *it;
    }
    m_bars.clear();
}

extern "C" void fn_80152434(void) {
    fn_8006B474("BAR.BIN", 1);
}

extern "C" void fn_80152430(void) {}

void ElecBar::Instantiate(UnkPlacement_800C9CD0 *owner) {
    new ElecBar(fn_801EB304(fn_8001C4F8(), 11), owner);
}

/* The fields of the cursor that the label callback reads (layout from its accesses). */
struct ElecBarParamCursor {
    /* 0x0 */ u32 field_0x0;
    /* 0x4 */ int index; /* the parameter */
    /* 0x8 */ int value;
};

/* The label callback of the type parameter. */
extern "C" const char *fn_801523B0(UnkLabelQuery_800CBC20 *cursor) {
    const ElecBarParamCursor *fields = (const ElecBarParamCursor *)cursor;

    switch (fields->index) {
    case 0:
        return s_typeLabels[fields->value];
    }
    return 0;
}

static GadgetParamDesc lbl_8052A580[] = {
    {2, "", 0, 0, 1, 0.0f, 0.0f, 0.0f, fn_801523B0, 0},
    {4, "scale", 0, 0, 0, 1.0f, 0.0f, 65536.0f, fn_800CBC20, 0},
    {4, "offset", 0, 0, 0, 0.0f, -65536.0f, 65536.0f, fn_800CBC20, 0},
    {4, "moving second", 0, 0, 0, 5.0f, 0.0f, 65536.0f, fn_800CBC20, 0},
    {4, "hiding second", 0, 0, 0, 1.0f, 0.0f, 65536.0f, fn_800CBC20, 0},
    {4, "appearing ratio", 0, 0, 0, 0.1f, 0.0f, 1.0f, fn_800CBC20, 0},
    {4, "synchronize", 0, 0, 0, 0.0f, 0.0f, 1.0f, fn_800CBC20, 0},
    {1, "number of bars", 1, 1, 9, 0.0f, 0.0f, 0.0f, fn_800CBC20, 0},
    {0},
};

GadgetClassDesc lbl_8052A6E8 = {
    "ELEC_BAR", fn_80152434, fn_80152430, ElecBar::Instantiate, 0, 0, 0x7D3, 1, 0, 10, lbl_8052A580,
};

/* Hands the bar's line to a player whose two parts are close enough to it (name inferred; as in
 * CatchStick.cpp). */
static void HitCharacter(boost::shared_ptr<CharInfo> hit, void *user) {
    ElecBarHitContext *context = static_cast<ElecBarHitContext *>(user);

    if (hit.get() != 0 && context != 0) {
        if (fn_800B7828(hit)) {
            return;
        }
        Player::PlayerBase *player = dynamic_cast<Player::PlayerBase *>(hit.get());
        if (player != 0) {
            boost::shared_ptr<Line> line = context->m_line.lock();
            if (line.get() != 0) {
                u8 segment[0x18];
                Vec3f start;
                Vec3f end;
                Vec3f offset;
                Vec3f difference;
                float distance;

                UnkPlayerPart_80078CEC *part = fn_80078CEC(player, 0);
                ConstructVector(&start, part->m_position);
                part = fn_80078CEC(player, 1);
                ConstructVector(&end, part->m_position);
                fn_8040CD74(&difference, &start, &end);
                if (fn_8040CF50(fn_800091EC(&offset, &difference)) < 0.1f) {
                    distance = fn_8003C660(&start, line.get(), 0);
                } else {
                    fn_80020A20(segment, &start, &end);
                    distance = fn_8003C4C4(segment, line.get(), 0, 0);
                }
                if (distance > 144.0f) {
                    return;
                }
            }
            hit->PushCommand(new Player::TouchLineCommand(context->m_chara, context->m_line, 4));
        }
    }
}

ElecBarSub::ElecBarSub(ElecBar *parent) : m_parent(parent) {
    m_chara = fn_8015CB88(0x38, 4, "ElecBar");
    fn_800091D0(&m_chara->m_position, &m_parent->m_position);
    CopyRotation(m_chara->m_rotation, m_parent->m_rotation);
    m_chara->m_commandUnit = boost::shared_ptr<CharCommandUnit>(new CharCommandUnit());
    fn_800729B0(&m_chara->m_commandUnit->m_callback,
                &(const CharCommandCallback &)CharCommandCallback(
                    boost::bind(&ElecBarSub::OnCommand, this, _1)));
    m_model = new GadgetModelInfo(1, "BAR.BIN");
    m_model->fn_80162A68(0, 0);
    fn_8016239C(m_model, "mat1");
    fn_8016239C(m_model, "mat2");
    Vec3f rotation(0.0f, 0.5f * lbl_805FAD38, 0.0f);
    boost::shared_ptr<CharaColliBase> collider =
        fn_800C78D4(fn_80021B08(), m_chara, 0x40000, 10, 0, 0.5f * (10.0f * m_parent->m_params.scale),
                    2.0f, &lbl_805E07A0, &rotation, 0);
    m_handle = fn_8015B8C4(collider);
    fn_800F5980(m_handle.get(), boost::bind(&ElecBarSub::OnColliDetect, this, _1));
    m_chara->m_rotation.x = m_chara->m_rotation.z = 0.0f;
    m_line = boost::shared_ptr<Line>(new Line());
    m_height = 0.0f;
    m_state = 3;
    UpdateMatrix();
}

ElecBarSub::~ElecBarSub() {
    if (m_model != 0) {
        delete m_model;
        m_model = 0;
    }
}

void ElecBarSub::Tick(float dt) {
    if (m_model != 0) {
        m_model->fn_80162FB0(dt);
    }

    float period = m_parent->m_period;
    float shift = period * (m_parent->m_params.synchronize + m_offset);
    float time = lbl_8057D6A0.m_time + shift;
    m_phase = time - period * (float)(int)(time / period);

    if (m_phase < m_parent->m_appearTime) {
        if (m_state == 2) {
            fn_801D54F4(fn_80053164(), 0x7239, &m_chara->m_position, 0, 1);
        }
        m_state = 0;
        UnkMaterialAnim_801621F0 *material1 = fn_801621F0(m_model, "mat1");
        UnkMaterialAnim_801621F0 *material2 = fn_801621F0(m_model, "mat2");
        m_height = m_parent->m_speed * m_phase;
        u8 alpha = 255.0f * (m_height / (m_parent->m_params.offset * m_parent->m_params.appearingRatio));
        SetColor(material1, alpha);
        SetColor(material2, alpha);
    } else if (m_phase < m_parent->m_params.movingSecond) {
        if (m_state != 1) {
            m_handle->EnableContacts();
            fn_800F5980(m_handle.get(), boost::bind(&ElecBarSub::OnColliDetect, this, _1));
            UnkMaterialAnim_801621F0 *material1 = fn_801621F0(m_model, "mat1");
            UnkMaterialAnim_801621F0 *material2 = fn_801621F0(m_model, "mat2");
            SetColor(material1, 0xFF);
            SetColor(material2, 0xFF);
        }
        m_state = 1;
        m_height = m_parent->m_speed * m_phase;
        float fade = m_parent->m_params.offset * m_parent->m_params.appearingRatio;
        float left = std::fabs(m_parent->m_params.offset - m_height);
        if (left < fade) {
            UnkMaterialAnim_801621F0 *material1 = fn_801621F0(m_model, "mat1");
            UnkMaterialAnim_801621F0 *material2 = fn_801621F0(m_model, "mat2");
            u8 alpha = 255.0f * (left / fade);
            SetColor(material1, alpha);
            SetColor(material2, alpha);
        }
    } else {
        if (m_state != 2) {
            m_handle->DisableContacts();
            boost::shared_ptr<CharInfo> rider = m_rider.lock();
            if (rider.get() != 0) {
                rider->PushCommand(new Player::StartActionCommand(4));
            }
            UnkMaterialAnim_801621F0 *material1 = fn_801621F0(m_model, "mat1");
            UnkMaterialAnim_801621F0 *material2 = fn_801621F0(m_model, "mat2");
            SetColor(material1, 0);
            SetColor(material2, 0);
        }
        m_state = 2;
    }

    UpdateMatrix();
    fn_8015BE9C(m_handle.get(), dt);
}

void ElecBarSub::UpdatePlacement() {
    fn_800091D0(&m_chara->m_position, &m_parent->m_position);
    CopyRotation(m_chara->m_rotation, m_parent->m_rotation);
}

void ElecBarSub::UpdateMatrix() {
    Mtx34f matrix;
    Vec3f start(5.0f, 0.0f, 0.0f);
    Vec3f end(-5.0f, 0.0f, 0.0f);
    Vec3f offset;

    if (m_parent->m_params.type == 0) {
        fn_800091D0(&offset, &Vec3f(0.0f, m_height, 0.0f));
    } else {
        fn_800091D0(&offset, &Vec3f(0.0f, 0.0f, -m_height));
    }
    fn_8040C148(&matrix, &lbl_805E0720);
    fn_8040B59C(&matrix, &m_chara->m_position);
    fn_8040B848(&matrix, m_chara->m_rotation.y);
    fn_8040B59C(&matrix, &offset);
    fn_800091D0(&m_line->m_start,
                &(const Vec3f &)fn_8040B67C(
                    &matrix, &(const Vec3f &)SubtractVectors(
                                 fn_8040CDA8(&start, m_parent->m_params.scale),
                                 Vec3f(3.75f, 0.0f, 0.0f))));
    fn_800091D0(&m_line->m_end,
                &(const Vec3f &)fn_8040B67C(
                    &matrix, &(const Vec3f &)AddVectors(
                                 fn_8040CDA8(&end, m_parent->m_params.scale),
                                 Vec3f(3.75f, 0.0f, 0.0f))));
    fn_8040B720(&matrix, m_parent->m_params.scale, 0.75f, 0.75f);
    if (m_model != 0) {
        m_model->fn_801633DC(&matrix);
    }
    m_handle->m_collider->SetOffset(&offset, -2);
}

void ElecBarSub::OnCommand(CharCommand &command) {
    u32 type = command.TypeId();
    if (type == 0xB) {
        RideCommand *ride = dynamic_cast<RideCommand *>(&command);
        if (ride != 0) {
            m_rider = ride->m_rider;
        }
    } else if (type == 0xC) {
        m_rider.reset();
    }
}

void ElecBarSub::OnColliDetect(ColliDetectArgs &args) {
    ElecBarHitContext context;

    context.m_chara = m_chara;
    context.m_line = m_line;
    fn_8015AA84(args, HitCharacter, &context);
}

/* Not called anywhere: the linker strips it from the game. Retail pools the int-to-float constant
 * of Tick's cycle arithmetic before UpdateMatrix's literals, which compiling a conversion at
 * this point of the file reproduces (inferred, as in ElecSmallCube). */
float ElecBarSub::IndexToFloat(int index) {
    return (float)index;
}

/* The tree's larger members, defined after their uses (see the tree above). */
template <class T, class C>
__tree<T, C>::__tree(const C &comp, const value_allocator &a)
    : size_(a, 0), header_(node_allocator()), begin_(comp) {
    begin_node() = static_cast<__tree_node_base *>(header());
}

template <class T, class C> void __tree<T, C>::destroy(node *n) {
    node *child = static_cast<node *>(n->left_);
    if (child != 0) {
        if (child->left_ != 0) {
            destroy(static_cast<node *>(child->left_));
        }
        if (child->right_ != 0) {
            destroy(static_cast<node *>(child->right_));
        }
        value_alloc().destroy(&child->value_);
        node_alloc().deallocate(child, 1);
    }
    child = static_cast<node *>(n->right_);
    if (child != 0) {
        if (child->left_ != 0) {
            destroy(static_cast<node *>(child->left_));
        }
        if (child->right_ != 0) {
            destroy(static_cast<node *>(child->right_));
        }
        value_alloc().destroy(&child->value_);
        node_alloc().deallocate(child, 1);
    }
    /* The outer level's value destruction and deallocation, inlined (see UnkTreeDestroy.h). */
    value_alloc();
    node_alloc();
    ::operator delete(n);
}

template <class T, class C> void __tree<T, C>::clear() {
    if (root() != 0) {
        destroy(static_cast<node *>(root()));
        size_ref() = 0;
        root() = 0;
        begin_node() = static_cast<__tree_node_base *>(header());
    }
}

template <class T, class C> std::pair<typename __tree<T, C>::iterator, bool> __tree<T, C>::insert_unique(const T &v) {
    __tree_node_base *last = 0;
    __tree_node_base *y = static_cast<__tree_node_base *>(header());
    __tree_node_base *x = root();
    bool left = true;
    bool leftmost = true;
    while (x != 0) {
        y = x;
        if (comp()(v, static_cast<node *>(x)->value_)) {
            x = x->left_;
            left = true;
        } else {
            last = x;
            x = x->right_;
            left = false;
            leftmost = false;
        }
    }
    if (last == 0 || comp()(static_cast<node *>(last)->value_, v)) {
        return std::pair<iterator, bool>(iterator(insert_node(y, left, leftmost, v)), true);
    }
    return std::pair<iterator, bool>(iterator(last), false);
}

template <class T, class C>
typename __tree<T, C>::node *__tree<T, C>::insert_node(__tree_node_base *parent, bool left,
                                                      bool leftmost, const T &v) {
    if (size() > max_size() - 1) {
        std::__msl_error("tree::insert length error\n");
    }
    node *n = node_alloc().allocate(1);
    value_alloc().construct(&n->value_, v);
    n->left_ = n->right_ = 0;
    n->set_parent(parent);
    if (left) {
        parent->left_ = n;
    } else {
        parent->right_ = n;
    }
    ++size_ref();
    fn_8000CD64(n, root());
    if (leftmost) {
        begin_node() = n;
    }
    return n;
}
