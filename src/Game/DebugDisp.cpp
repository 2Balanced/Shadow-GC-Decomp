/* SonicteamUSA::System::DebugDisp (class name from RTTI): the task behind the debug view's shapes.
 * The draw helpers below (the gadgets call them by their fn_ names, see Game/DebugDraw.h) queue a
 * new shape at the front of one of two lists, the opaque one when the colour's first byte is 0xFF.
 * Three render callbacks, registered with three render layers, each draw their layer's lists: the
 * shapes kept from the previous frame, then the new ones (which they move to the kept list), then
 * the one-shot list, whose shapes they delete after drawing. Tick deletes the kept shapes. The
 * shapes are drawn with the models of DebugModels.one, which the constructor loads. Each layer has
 * three lists; only the first two layers' new lists are filled by the helpers that remain in the
 * game. File name from the class; names marked "(name inferred)" are not original. Definitions are
 * in reverse address order (deferred inlining emits functions last-defined first). */
#include "Game/DebugDisp.h"
#include "Game/ElecSecurityCode.h"
#include "Game/Task.h"
#include <dolphin/types.h>
#include <memory>
#include "size_assert.h"

using SonicteamUSA::System::DebugDispBase;
using SonicteamUSA::System::DebugDispBox;
using SonicteamUSA::System::DebugDispCapsule;
using SonicteamUSA::System::DebugDispCylinder;
using SonicteamUSA::System::DebugDispLine;
using SonicteamUSA::System::DebugDispSphere;

/* The archive the models come from (class name unknown; see Game/PjsEventActionScene.cpp). */
struct UnkOneFile_8004CFB0;

extern "C" {
/* Where new tasks go (see Game/Gadget.h). */
void *fn_8001C4F8(void);
Task *fn_801EB304(void *root, int layer);

/* Opens an archive; the index of a file in it (-1 when missing); a clump made from one of its
 * files; and the archive's deleting destructor (behaviour inferred). */
UnkOneFile_8004CFB0 *fn_8004CFB0(void *file, const char *name, int arg2);
int fn_8004C910(UnkOneFile_8004CFB0 *file, const char *name);
RpClump *fn_8004BB24(UnkOneFile_8004CFB0 *file, int index, int arg2, int arg3);
void fn_8004CF68(UnkOneFile_8004CFB0 *file, int flags);
/* Destroys a clump (behaviour inferred). */
void fn_80457468(RpClump *clump);

/* Render layers (see Game/Render.h): adding a callback, which returns a handle, and removing one
 * by its handle (behaviour inferred). */
struct UnkRenderOwner_80046178 *fn_80046178(void);
void *fn_8004E2D4(void *layer, void *user, void (*callback)(void *user));
void fn_8004E4B0(void *handle, int arg1);

/* More of the list implementation of Game/ElecSecurityCode.h: its end node, its first value, and
 * inserting value before position (behaviour inferred). */
ElecSecurityCodeLink *fn_8001B6A8(UnkPtrList_8001B5B8 *list);
DebugDispBase *&fn_80058F48(UnkPtrList_8001B5B8 *list);
UnkPtrListPosition_8001B674 fn_8001E528(UnkPtrList_8001B5B8 *list,
                                        UnkPtrListPosition_8001B674 position,
                                        DebugDispBase *const &value);
}

/* The render layers DebugDisp draws in (see Game/Render.h; meanings unknown). */
struct UnkRenderOwner_80046178 {
    /* 0x00 */ u8 field_0x0[0x8];
    /* 0x08 */ void *field_0x8;
    /* 0x0C */ u8 field_0xC[0x18 - 0xC];
    /* 0x18 */ void *field_0x18;
    /* 0x1C */ u8 field_0x1C[0x38 - 0x1C];
    /* 0x38 */ void *field_0x38;
};

/* Not original: this unit's copy of the shared list constructor. Retail emits one here, after
 * the list's constructor, and the linker keeps the first unit's copy (0x8001BE04) instead; only
 * the message of its node allocation stays in this unit's .rodata. The body is not retail's. */
extern "C" inline void fn_8001BE04(UnkPtrList_8001B5B8 *list) {
    if (list == 0) {
        std::__msl_error("Memory allocation failure");
    }
    list->m_size = 0;
    list->m_end.prev = &list->m_end;
    list->m_end.next = &list->m_end;
}

/* Keeps the inline function that calls it out of line, as retail's list members are. */
inline void DebugDispListKeep() {}

/* A list of shapes: the game's list of pointers (Game/ElecSecurityCode.h) holding DebugDispBase
 * pointers. Its members are inline and emitted after their first user, as the game's list
 * template's are (names inferred). */
class DebugDispList : public UnkPtrList_8001B5B8 {
  public:
    struct Node : public ElecSecurityCodeLink {
        /* 0x8 */ DebugDispBase *value;
    };

    class iterator {
      public:
        iterator() {
            DebugDispListKeep();
        }
        iterator(const iterator &other) : m_node(other.m_node) {
            DebugDispListKeep();
        }
        iterator(const UnkPtrListPosition_8001B674 &position) : m_node(position.node) {
            DebugDispListKeep();
        }

        DebugDispBase *&operator*() const {
            DebugDispListKeep();
            return static_cast<Node *>(m_node)->value;
        }
        iterator operator++(int) {
            iterator old(*this);
            m_node = m_node->next;
            return old;
        }
        bool operator!=(const iterator &other) const {
            DebugDispListKeep();
            return m_node != other.m_node;
        }

        /* 0x0 */ ElecSecurityCodeLink *m_node;
    };

    DebugDispList() {
        DebugDispListKeep();
        fn_8001BE04(this);
    }
    ~DebugDispList() {
        DebugDispListKeep();
        fn_8001B5B8(this, 0);
    }

    iterator begin() {
        return iterator(fn_8001B6B0(this));
    }
    iterator end() {
        return iterator(fn_8001B674(this));
    }
    DebugDispBase *&front() {
        DebugDispListKeep();
        return fn_80058F48(this);
    }
    void push_front(DebugDispBase *const &value) {
        PushFront(value);
    }
    void PushFront(DebugDispBase *const &value) {
        DebugDispListKeep();
        fn_8001E528(this, fn_8001B6B0(this), value);
    }
    void push_back(DebugDispBase *const &value) {
        DebugDispListKeep();
        fn_8001D820(this, reinterpret_cast<ElecSecurityCode *const &>(value));
    }
    void pop_front() {
        reinterpret_cast<ElecSecurityCodeList *>(this)->EraseNode(
            UnkPtrListPosition_8001B674(fn_8001B6A8(this)->next));
    }
};

namespace SonicteamUSA {
namespace System {

class DebugDisp : public Task {
  public:
    DebugDisp();
    virtual ~DebugDisp();
    virtual void Tick(float dt);

    /* Loads the shapes' models (name inferred). */
    void LoadModels();

    /* The render callbacks of the three layers (names inferred). */
    static void DrawLayer0(void *user);
    static void DrawLayer1(void *user);
    static void DrawLayer2(void *user);

    static const char *s_taskName;

    /* 0x28 */ void *m_layerHandles[3];
};

} // namespace System
} // namespace SonicteamUSA

SIZE_ASSERT(SonicteamUSA::System::DebugDisp, 0x34);

using SonicteamUSA::System::DebugDisp;

/* Not original: data that only an unreferenced object of the original placed here (four rows of
 * floats). */
static const float lbl_805058E0[4][4] = {
    {0.0f, 0.0f, 0.0f, 0.0f},
    {0.0f, 0.0f, 0.0f, 1.0f},
    {0.0f, 0.0f, 0.0f, 1.0f},
    {0.0f, 0.0f, 0.0f, 1.0f},
};

const char *DebugDisp::s_taskName = "DebugDispDeleteList";

/* The colour the gadgets' debug views use. */
UnkColor_8040D2D8 lbl_805F06D8(0xC0, 0xFF, 0xFF, 0xFF);

RpClump *lbl_805F06DC;
RpClump *lbl_805F06E0;
RpClump *lbl_805F06E4;
RpClump *lbl_805F06E8;
RpClump *lbl_805F06EC;

/* Per layer: the new opaque and translucent shapes and a third list, the kept shapes, and the
 * one-shot shapes (names inferred). */
static DebugDispList s_newOpaque;
static DebugDispList s_newTranslucent;
static DebugDispList s_newLayer2;
static DebugDispList s_keptOpaque;
static DebugDispList s_keptTranslucent;
static DebugDispList s_keptLayer2;
static DebugDispList s_onceOpaque;
static DebugDispList s_onceTranslucent;
static DebugDispList s_onceLayer2;

void DebugDisp::DrawLayer0(void *user) {
    DebugDispList::iterator it;

    for (it = s_keptOpaque.begin(); it != s_keptOpaque.end();) {
        (*it)->Draw();
        it++;
    }
    for (it = s_newOpaque.begin(); it != s_newOpaque.end();) {
        (*it)->Draw();
        it++;
        s_keptOpaque.push_back(s_newOpaque.front());
        s_newOpaque.pop_front();
    }
    for (it = s_onceOpaque.begin(); it != s_onceOpaque.end();) {
        (*it)->Draw();
        delete *it;
        it++;
        s_onceOpaque.pop_front();
    }
}

void DebugDisp::DrawLayer1(void *user) {
    DebugDispList::iterator it;

    for (it = s_keptTranslucent.begin(); it != s_keptTranslucent.end();) {
        (*it)->Draw();
        it++;
    }
    for (it = s_newTranslucent.begin(); it != s_newTranslucent.end();) {
        (*it)->Draw();
        it++;
        s_keptTranslucent.push_back(s_newTranslucent.front());
        s_newTranslucent.pop_front();
    }
    for (it = s_onceTranslucent.begin(); it != s_onceTranslucent.end();) {
        (*it)->Draw();
        delete *it;
        it++;
        s_onceTranslucent.pop_front();
    }
}

void DebugDisp::DrawLayer2(void *user) {
    DebugDispList::iterator it;

    for (it = s_keptLayer2.begin(); it != s_keptLayer2.end();) {
        (*it)->Draw();
        it++;
    }
    for (it = s_newLayer2.begin(); it != s_newLayer2.end();) {
        (*it)->Draw();
        it++;
        s_keptLayer2.push_back(s_newLayer2.front());
        s_newLayer2.pop_front();
    }
    for (it = s_onceLayer2.begin(); it != s_onceLayer2.end();) {
        (*it)->Draw();
        delete *it;
        it++;
        s_onceLayer2.pop_front();
    }
}

DebugDisp::DebugDisp() : Task(fn_801EB304(fn_8001C4F8(), 0)) {
    m_name = s_taskName;
    LoadModels();
    m_layerHandles[0] = fn_8004E2D4(fn_80046178()->field_0x8, this, DrawLayer0);
    m_layerHandles[1] = fn_8004E2D4(fn_80046178()->field_0x18, this, DrawLayer1);
    m_layerHandles[2] = fn_8004E2D4(fn_80046178()->field_0x38, this, DrawLayer2);
}

DebugDisp::~DebugDisp() {
    if (lbl_805F06DC != 0) {
        fn_80457468(lbl_805F06DC);
        lbl_805F06DC = 0;
    }
    if (lbl_805F06E0 != 0) {
        fn_80457468(lbl_805F06E0);
        lbl_805F06E0 = 0;
    }
    if (lbl_805F06E4 != 0) {
        fn_80457468(lbl_805F06E4);
        lbl_805F06E4 = 0;
    }
    if (lbl_805F06E8 != 0) {
        fn_80457468(lbl_805F06E8);
        lbl_805F06E8 = 0;
    }
    if (lbl_805F06EC != 0) {
        fn_80457468(lbl_805F06EC);
        lbl_805F06EC = 0;
    }
    fn_8004E4B0(m_layerHandles[0], 1);
    fn_8004E4B0(m_layerHandles[1], 1);
    fn_8004E4B0(m_layerHandles[2], 1);
}

void DebugDisp::Tick(float dt) {
    DebugDispList::iterator it;

    for (it = s_keptOpaque.begin(); it != s_keptOpaque.end();) {
        delete *it;
        it++;
        s_keptOpaque.pop_front();
    }
    for (it = s_keptTranslucent.begin(); it != s_keptTranslucent.end();) {
        delete *it;
        it++;
        s_keptTranslucent.pop_front();
    }
    for (it = s_keptLayer2.begin(); it != s_keptLayer2.end();) {
        delete *it;
        it++;
        s_keptLayer2.pop_front();
    }
}

/* Returns the clump made from the file name of archive, or 0 when it is missing (name inferred). */
static inline RpClump *LoadModel(UnkOneFile_8004CFB0 *archive, const char *name) {
    int index = fn_8004C910(archive, name);

    if (index != -1) {
        return fn_8004BB24(archive, index, 0, 0);
    }
    return 0;
}

void DebugDisp::LoadModels() {
    UnkOneFile_8004CFB0 *archive = (UnkOneFile_8004CFB0 *)operator new(0x58);

    if (archive != 0) {
        archive = fn_8004CFB0(archive, "DebugModels.one", 0);
    }
    lbl_805F06DC = LoadModel(archive, "sphere_model.dff");
    lbl_805F06E0 = LoadModel(archive, "cube_model.dff");
    lbl_805F06E4 = LoadModel(archive, "cylinder_model.dff");
    lbl_805F06E8 = LoadModel(archive, "capsule_model.dff");
    lbl_805F06EC = LoadModel(archive, "cone_model.dff");
    fn_8004CF68(archive, 1);
}

extern "C" {
/* Debug view: a line of width (0: a thin line), and a thin line. */
void fn_802E0AA0(const UnkDebugLine_80020A20 *line, const UnkColor_8040D2D8 *color, float width);
void fn_802E0B58(const UnkDebugLine_80020A20 *line, const UnkColor_8040D2D8 *color);
/* Debug view: a cylinder, and one in the default colour. */
void fn_802E0B7C(const Vec3f *center, const Vec3f *rotation, const UnkColor_8040D2D8 *color, int axis,
                 float halfHeight, float radius);
void fn_802E0C60(const Vec3f *center, const Vec3f *rotation, int axis, float halfHeight, float radius);
/* Debug view: a capsule, and one in the default colour. */
void fn_802E0C88(const Vec3f *center, const Vec3f *rotation, const UnkColor_8040D2D8 *color, int axis,
                 float halfHeight, float radius);
void fn_802E0D6C(const Vec3f *center, const Vec3f *rotation, int axis, float halfHeight, float radius);
/* Debug view: a box, and one in the default colour. */
void fn_802E0D94(const Vec3f *center, const Vec3f *size, const Vec3f *rotation,
                 const UnkColor_8040D2D8 *color, int axis);
void fn_802E0E5C(const Vec3f *center, const Vec3f *size, const Vec3f *rotation, int axis);
/* Debug view: a sphere of radius size around at, and sphere itself. */
void fn_802E0E84(const Vec3f *at, const UnkColor_8040D2D8 *color, float size);
void fn_802E0F3C(const UnkSphere_80035CEC *sphere, const UnkColor_8040D2D8 *color);
}

extern "C" void fn_802E0F3C(const UnkSphere_80035CEC *sphere, const UnkColor_8040D2D8 *color) {
    fn_802E0E84(&sphere->m_center, color, sphere->m_radius);
}

extern "C" void fn_802E0E84(const Vec3f *at, const UnkColor_8040D2D8 *color, float size) {
    if (color->r == 0xFF) {
        s_newOpaque.push_front(new DebugDispSphere(*at, *color, size));
    } else {
        s_newTranslucent.push_front(new DebugDispSphere(*at, *color, size));
    }
}

extern "C" void fn_802E0E5C(const Vec3f *center, const Vec3f *size, const Vec3f *rotation, int axis) {
    fn_802E0D94(center, size, rotation, &lbl_805F06D8, axis);
}

extern "C" void fn_802E0D94(const Vec3f *center, const Vec3f *size, const Vec3f *rotation,
                            const UnkColor_8040D2D8 *color, int axis) {
    if (color->r == 0xFF) {
        s_newOpaque.push_front(new DebugDispBox(*center, *size, *rotation, *color, axis));
    } else {
        s_newTranslucent.push_front(new DebugDispBox(*center, *size, *rotation, *color, axis));
    }
}

extern "C" void fn_802E0D6C(const Vec3f *center, const Vec3f *rotation, int axis, float halfHeight,
                            float radius) {
    fn_802E0C88(center, rotation, &lbl_805F06D8, axis, halfHeight, radius);
}

extern "C" void fn_802E0C88(const Vec3f *center, const Vec3f *rotation, const UnkColor_8040D2D8 *color,
                            int axis, float halfHeight, float radius) {
    if (color->r == 0xFF) {
        s_newOpaque.push_front(new DebugDispCapsule(*center, *rotation, *color, axis, halfHeight, radius));
    } else {
        s_newTranslucent.push_front(
            new DebugDispCapsule(*center, *rotation, *color, axis, halfHeight, radius));
    }
}

extern "C" void fn_802E0C60(const Vec3f *center, const Vec3f *rotation, int axis, float halfHeight,
                            float radius) {
    fn_802E0B7C(center, rotation, &lbl_805F06D8, axis, halfHeight, radius);
}

extern "C" void fn_802E0B7C(const Vec3f *center, const Vec3f *rotation, const UnkColor_8040D2D8 *color,
                            int axis, float halfHeight, float radius) {
    if (color->r == 0xFF) {
        s_newOpaque.push_front(new DebugDispCylinder(*center, *rotation, *color, axis, halfHeight, radius));
    } else {
        s_newTranslucent.push_front(
            new DebugDispCylinder(*center, *rotation, *color, axis, halfHeight, radius));
    }
}

extern "C" void fn_802E0B58(const UnkDebugLine_80020A20 *line, const UnkColor_8040D2D8 *color) {
    fn_802E0AA0(line, color, 0.0f);
}

extern "C" void fn_802E0AA0(const UnkDebugLine_80020A20 *line, const UnkColor_8040D2D8 *color, float width) {
    if (color->r == 0xFF) {
        s_newOpaque.push_front(new DebugDispLine(*line, *color, width));
    } else {
        s_newTranslucent.push_front(new DebugDispLine(*line, *color, width));
    }
}

/* Not original: stands for the queueing functions the linker dropped from retail (unused there),
 * which emitted the list's push_front ahead of the helpers above. */
extern "C" void DebugDisp_QueueOnce(DebugDispBase *shape) {
    s_onceOpaque.push_front(shape);
}
