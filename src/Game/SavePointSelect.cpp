/* SavePointSelect::SavePointSelectTask (class name from RTTI): the save point menu the SAVEPOINT
 * gadget (Game/SavePoint.cpp) opens. Its state lives in a SavePointSelect::SavePointSelectImpl
 * (class name from the RTTI of its draw callback's bind_t), which plays the stage's CSD project
 * "CHECKPOINTn" (n the number of save points in the stage, 5 to 8): the scene "Box", one "Icons"
 * scene per save point and a "Shadow" scene on the current one. The pad moves the cursor over the
 * save points already reached; choosing one calls the task's m_onSelect with its number, cancelling
 * (or choosing the current one) calls m_onCancel. fn_80330CC4 counts the stage's save points when
 * the stage loads (with a SavePointSelect::EventArgs, class name from RTTI) and loads the project's
 * resource list. The file name and the names marked "(name inferred)" are not original; the
 * functions other units call keep their fn_ names. Definitions are in reverse address order
 * (deferred inlining emits functions last-defined first). */
/* boost::function's operator() puts a bad_function_call on the stack (see the MSL's stdexcept). */
#define MSL_RUNTIME_ERROR_STRING
#include <boost/bind.hpp>
#include <boost/function.hpp>
#include <cstdio>

#define GAME_SAVEPOINTSELECT_IMPL
#include "Game/CsdRCPtr.h"
#include "Game/Render.h"
#include "Game/SavePointSelect.h"
#include "Game/SetAdapterEventArgs.h"
#include "Game/Task.h"
#include "boost/scoped_ptr.hpp"
#include "boost/shared_ptr.hpp"
#include <dolphin/types.h>
#include "size_assert.h"

using Chao::CSD::CNode;
using Chao::CSD::CScene;
using Chao::CSD::RCPtr;

/* The argument a Render passes to its draw callbacks (unused here). */
struct RednerDrawArgs;

/* The save data (class unknown): the save points reached, indexed by number from 1 (the layout
 * as far as this unit reads it). */
struct UnkObject_80079808 {
    struct SavePoint {
        /* 0x00 */ u8 m_reached;
        /* 0x01 */ u8 field_0x1[0x1C - 0x1];
    };

    /* 0x00 */ u8 field_0x0[0x24];
    /* 0x24 */ SavePoint m_savePoints[9];
};

/* The object fn_80074470 returns (class unknown; as in SavePoint.cpp). */
struct UnkObject_80074470 {
    /* 0x00 */ u8 field_0x0[0x8];
    /* 0x08 */ s32 field_0x8;
};

/* A node's position (struct name unknown): two floats, which fn_8036AE98 returns in r3/r4. */
struct UnkCsdPosition_8036AE98 {
    float x;
    float y;
};

class UnkObject_801175AC;
class UnkObject_802E5B2C;
class UnkPlacement_800C9CD0;

extern "C" {
/* The resource manager's language-specific path of a file, and loading a resource list (behaviour
 * inferred; see Gong.cpp). */
void *fn_8003F8D4(void);
const char *fn_8004237C(void *arg0, const char *name, int arg2);
void fn_8006B474(const char *name, int flag);
/* The model loader, and loading a model file (behaviour inferred; see ArkCannon.cpp). */
void *fn_800122D4(void);
void *fn_80012700(void *loader, int arg1, const char *fileName);

/* Projects (behaviour inferred): drawing, advancing, resetting, a scene by name and playing a
 * scene's motion. */
void fn_8036D94C(UnkCsdProject_800BFF30 *project);
void fn_8036DA1C(UnkCsdProject_800BFF30 *project, float dt);
void fn_8036C248(UnkCsdProject_800BFF30 *project);
RCPtr<CScene> fn_8036D660(UnkCsdProject_800BFF30 *project, const char *name);
RCPtr<CScene> fn_8036D890(UnkCsdProject_800BFF30 *project, const char *name, int motion, int arg3);
/* Scenes and nodes (behaviour inferred): a node by name, a node's position, placing a scene at a
 * position, and a word of the scene. */
RCPtr<CNode> fn_803666B4(CScene *scene, const char *name);
UnkCsdPosition_8036AE98 fn_8036AE98(CNode *node);
void fn_80365B18(CScene *scene, const UnkCsdPosition_8036AE98 *position);
void fn_803659DC(CScene *scene, int arg1);

/* Whether button of pad was pressed (behaviour inferred). */
bool fn_80356260(int button, int pad);
/* The sound manager and playing a sound (behaviour inferred; see GadgetUtil.h). */
void *fn_80053164(void);
void fn_801D560C(void *manager, int soundId, int arg2, int arg3);
/* The save data. */
UnkObject_80079808 *fn_80079808(void);

/* Queries around saving (behaviour unknown; as in SavePoint.cpp). */
UnkObject_801175AC *fn_80117578(void);
UnkObject_802E5B2C *fn_80117514(void);
bool fn_8020261C(UnkObject_801175AC *object);
u8 fn_802E5810(UnkObject_802E5B2C *object);
UnkObject_80074470 *fn_80074470(void);

/* The set object manager, its type of an object, and calling callback with every object
 * (behaviour inferred; the callback returns 0 to stop). */
void *fn_800C9B84(void);
u16 fn_800C9D24(UnkPlacement_800C9CD0 *owner);
void fn_800CB47C(void *manager, void *(*callback)(UnkPlacement_800C9CD0 *owner, void *arg), void *arg);
}

/* The flag word at SavePointSelectImpl+0x80 (class name unknown; its out-of-line members are the
 * fn_80014E04 family, see GadgetPickup.h). Bit 0: a save point was chosen, bit 1: cancelled,
 * bit 2: closing to choose, bit 3: closing to cancel (meanings inferred). */
extern "C" {
void fn_80014E04(u32 *flags);
void fn_80014E1C(u32 *flags, int bit);
bool fn_80014E34(u32 *flags, int bit);
}

struct UnkFlags_80014E04 {
    UnkFlags_80014E04() {
        fn_80014E04(&m_bits);
    }
    void Set(int bit) {
        fn_80014E1C(&m_bits, bit);
    }
    bool Test(int bit) {
        return fn_80014E34(&m_bits, bit);
    }

    u32 m_bits;
};

/* Not original: an unused table that pools the unit's .rodata strings in retail's order (the
 * linker drops the table; the RTTI names and literals below reuse these copies). The .rodata is
 * kept whole (fn_80330CC4 and the constructor address it through one base), so it also holds the
 * RTTI names of TNumEdit.h's classes and of the CSD and boost headers' inline classes, which this
 * unit does not reconstruct. */
static const char *s_rodataPool[] = {
    "TNumEditHex",
    "TNumEdit",
    "TNumEditInt",
    "TNumEditFloat",
    "boost::_bi::bind_t<void, boost::_mfi::mf1<void, SavePointSelect::SavePointSelectImpl, RednerDrawArgs &>, boost::_bi::list2<boost::_bi::value<SavePointSelect::SavePointSelectImpl *>, boost::arg<1>>>",
    "SavePointSelectTask",
    "SavePointSelect::SavePointSelectTask",
    "CHECKPOINT5",
    "CHECKPOINT6",
    "CHECKPOINT7",
    "CHECKPOINT8",
    "Not inititalized m_eDealloctor\n",
    "Memory allocation failure",
    "SavePointSelectImpll",
    "csdBase.h",
    "csdRCObject.h",
    "Chao::CSD::RCPtr<unsigned char>::RCObject<unsigned char>::CFunctorBase",
    "Chao::CSD::CBase",
    "Chao::CSD::RCPtr<unsigned char>::RCObject<unsigned char>",
    "Chao::CSD::CResourceBase<Chao::CSD::Project>",
    "Chao::CSD::RCPtr<Chao::CSD::CTexList>::RCObject<Chao::CSD::CTexList>::CFunctorBase",
    "Chao::CSD::RCPtr<Chao::CSD::CTexList>::RCObject<Chao::CSD::CTexList>",
    "Chao::CSD::RCPtr<Chao::CSD::CNode>::RCObject<Chao::CSD::CNode>::CFunctorBase",
    "Chao::CSD::RCPtr<Chao::CSD::CNode>::RCObject<Chao::CSD::CNode>",
    "Chao::CSD::RCPtr<Chao::CSD::CScene>::RCObject<Chao::CSD::CScene>::CFunctorBase",
    "Chao::CSD::RCPtr<Chao::CSD::CScene>::RCObject<Chao::CSD::CScene>",
    "boost::detail::sp_counted_base",
    "SetAdapterEventArgs",
    "SavePointSelect::EventArgs",
    "call to empty boost::function",
    "std::exception",
    "exception",
    "std::runtime_error",
    "boost::bad_function_call",
    "_RefCountedPtr out of memory",
};

/* The scenes of the project. */
static const char *s_boxScene = "Box";
static const char *s_iconScene = "Icons";
static const char *s_shadowScene = "Shadow";

/* Whether the stage has a save point menu, and its number of save points (names inferred). */
static bool s_enabled;
static s32 s_count;

namespace SavePointSelect {

class SavePointSelectImpl {
  public:
    /* A save point of the menu (name inferred). */
    struct Entry {
        Entry() : m_number(-1), m_enabled(0) {}

        /* 0x0 */ s32 m_number;
        /* 0x4 */ u8 m_enabled;
    };

    SavePointSelectImpl(s32 count, s32 current);
    ~SavePointSelectImpl();

    /* Runs the menu (name inferred). */
    void Tick(float dt);
    /* Moves the cursor and chooses (name inferred). */
    void UpdateSelect(float dt);
    /* Waits for the box to close (name inferred). */
    void UpdateClose(float dt);
    /* The Render's draw callback (name inferred). */
    void Draw(RednerDrawArgs &args);
    /* Whether the menu has to close (name inferred). */
    bool IsInterrupted();

    /* Tests a bit of m_flags (name inferred). */
    bool TestFlag(int bit) {
        return fn_80014E34(&m_flags.m_bits, bit);
    }

    /* The number of the save point under the cursor (name inferred). */
    s32 GetSelected() {
        return m_entries[m_cursor].m_number;
    }

    /* Moves the cursor by step until it is on a save point reached (name inferred). */
    void SkipDisabled(int step) {
        for (;;) {
            if (m_cursor < 0) {
                m_cursor = m_count - 1;
            } else if (m_cursor > m_count - 1) {
                m_cursor = 0;
            }
            if (m_entries[m_cursor].m_enabled) {
                break;
            }
            m_cursor += step;
        }
    }

    /* 0x00 */ boost::shared_ptr<Render> m_render;
    /* 0x08 */ UnkCsdProject_800BFF30 *m_box;
    /* 0x0C */ UnkCsdProject_800BFF30 *m_icons[8];
    /* 0x2C */ UnkCsdProject_800BFF30 *m_shadow;
    /* 0x30 */ s32 m_state;
    /* 0x34 */ s32 m_cursor;
    /* 0x38 */ s32 m_count;
    /* 0x3C */ s32 m_current;
    /* 0x40 */ Entry m_entries[8];
    /* 0x80 */ UnkFlags_80014E04 m_flags;
};

/* Deletes the impl (name inferred; see checked_delete below). */
inline void DeleteImpl(SavePointSelectImpl *impl) {
    delete impl;
}

} // namespace SavePointSelect

/* Retail keeps boost::checked_delete<SavePointSelectImpl> out of line and the destructor at its own
 * place; this specialization gives that order under GC/2.7 (see docs/research-notes.md and
 * Game/EnergyCore.cpp). */
namespace boost {
template <>
inline void checked_delete<SavePointSelect::SavePointSelectImpl>(SavePointSelect::SavePointSelectImpl *x) {
    SavePointSelect::DeleteImpl(x);
}
} // namespace boost

SIZE_ASSERT(SavePointSelect::SavePointSelectImpl, 0x84);
SIZE_ASSERT(SavePointSelect::SavePointSelectTask, 0x48);

namespace SavePointSelect {

/* NonMatching: every instruction matches except the stack slots of the Render temporary and the
 * three positions. Retail puts the temporary above the positions (0x68, then 0x60/0x58/0x50); GC/2.7
 * places named locals before temporaries of the same size, so the positions come first. Passing the
 * positions as temporaries (&fn_8036AE98(...), a const reference, a by-value parameter) orders the
 * slots like retail but computes their address before scene.Get() (an extra mr); binding the
 * Render to a const reference orders them too but loads its address before `this`. Neither
 * matches; UpdateSelect, which has no competing temporary, matches with named positions. */
SavePointSelectImpl::SavePointSelectImpl(s32 count, s32 current) {
    m_state = 0;
    m_cursor = 0;
    m_count = count;
    m_current = current;
    void *loader = fn_800122D4();
    void *resource = 0;
    switch (m_count) {
    case 5:
        resource = fn_80012700(loader, 0, fn_8004237C(fn_8003F8D4(), "CHECKPOINT5", 1));
        break;
    case 6:
        resource = fn_80012700(loader, 0, fn_8004237C(fn_8003F8D4(), "CHECKPOINT6", 1));
        break;
    case 7:
        resource = fn_80012700(loader, 0, fn_8004237C(fn_8003F8D4(), "CHECKPOINT7", 1));
        break;
    case 8:
        resource = fn_80012700(loader, 0, fn_8004237C(fn_8003F8D4(), "CHECKPOINT8", 1));
        break;
    }
    m_render = fn_8004DFB0(fn_80046178()->field_0x38, "SavePointSelectImpll");
    fn_80062CD0(&m_render->m_drawCallbacks, boost::bind(&SavePointSelectImpl::Draw, this, _1));

    m_box = new UnkCsdProject_800BFF30(resource);
    {
        RCPtr<CScene> scene = fn_8036D890(m_box, s_boxScene, 0, 0);
        fn_803659DC(scene.Get(), 0);
    }

    UnkObject_80079808 *save = fn_80079808();
    s32 number = 1;
    for (int i = 0; i < m_count; i++, number++) {
        if (m_current == number) {
            m_entries[i].m_enabled = 1;
            m_cursor = i;
        } else if (!save->m_savePoints[number].m_reached) {
            m_entries[i].m_enabled = 0;
        } else {
            m_entries[i].m_enabled = 1;
        }
        m_entries[i].m_number = number;
    }
    SkipDisabled(1);

    for (int i = 0; i < m_count; i++) {
        m_icons[i] = new UnkCsdProject_800BFF30(resource);
        if (m_entries[i].m_enabled) {
            const char *name;
            int motion;
            if (m_cursor == i) {
                name = s_iconScene;
                motion = 3;
            } else {
                name = s_iconScene;
                motion = 1;
            }
            fn_8036D890(m_icons[i], name, motion, 0);
            RCPtr<CScene> scene = fn_8036D660(m_icons[i], name);
            char text[32];
            sprintf(text, "%d", i + 1);
            RCPtr<CNode> node = fn_803666B4(scene.Get(), text);
            UnkCsdPosition_8036AE98 position = fn_8036AE98(node.Get());
            fn_80365B18(scene.Get(), &position);
            if (m_cursor == i) {
                fn_803659DC(scene.Get(), 0);
            }
        } else {
            fn_8036D890(m_icons[i], s_iconScene, 2, 0);
            RCPtr<CScene> scene = fn_8036D660(m_icons[i], s_iconScene);
            char text[32];
            sprintf(text, "%d", i + 1);
            RCPtr<CNode> node = fn_803666B4(scene.Get(), text);
            UnkCsdPosition_8036AE98 position = fn_8036AE98(node.Get());
            fn_80365B18(scene.Get(), &position);
        }
    }

    for (int i = 0; i < m_count; i++) {
        if (m_entries[i].m_number == m_current) {
            m_shadow = new UnkCsdProject_800BFF30(resource);
            fn_8036D890(m_shadow, s_shadowScene, 1, 0);
            RCPtr<CScene> scene = fn_8036D660(m_shadow, s_shadowScene);
            char text[32];
            sprintf(text, "%d", i + 1);
            RCPtr<CNode> node = fn_803666B4(scene.Get(), text);
            UnkCsdPosition_8036AE98 position = fn_8036AE98(node.Get());
            fn_80365B18(scene.Get(), &position);
        }
    }
    fn_801D560C(fn_80053164(), 0xE000, 0, 0);
}

SavePointSelectImpl::~SavePointSelectImpl() {
    if (m_shadow != 0) {
        fn_8036C248(m_shadow);
        delete m_shadow;
        m_shadow = 0;
    }
    for (int i = 0; i < m_count; i++) {
        if (m_icons[i] != 0) {
            fn_8036C248(m_icons[i]);
            delete m_icons[i];
            m_icons[i] = 0;
        }
    }
    if (m_box != 0) {
        fn_8036C248(m_box);
        delete m_box;
        m_box = 0;
    }
}

void SavePointSelectImpl::Tick(float dt) {
    if (m_box != 0) {
        fn_8036DA1C(m_box, dt);
    }
    for (int i = 0; i < m_count; i++) {
        if (m_icons[i] != 0) {
            fn_8036DA1C(m_icons[i], dt);
        }
    }
    if (m_shadow != 0) {
        fn_8036DA1C(m_shadow, dt);
    }
    switch (m_state) {
    case 0:
        m_state = 1;
        break;
    case 1:
        m_state = 2;
        break;
    case 2:
        UpdateSelect(dt);
        break;
    case 3:
        UpdateClose(dt);
        break;
    }
}

void SavePointSelectImpl::UpdateSelect(float dt) {
    s32 previous = m_cursor;
    if (fn_80356260(1, 4)) {
        if (m_entries[m_cursor].m_number == m_current) {
            m_flags.Set(3);
            fn_801D560C(fn_80053164(), 0xE004, 0, 0);
        } else {
            m_flags.Set(2);
            fn_801D560C(fn_80053164(), 0xE003, 0, 0);
        }
    } else if (fn_80356260(2, 4)) {
        m_flags.Set(3);
        fn_801D560C(fn_80053164(), 0xE004, 0, 0);
    } else if (fn_80356260(6, 4)) {
        m_cursor++;
        SkipDisabled(1);
    } else if (fn_80356260(5, 4)) {
        m_cursor--;
        SkipDisabled(-1);
    }

    if (previous != m_cursor) {
        fn_8036C248(m_icons[m_cursor]);
        {
            RCPtr<CScene> scene = fn_8036D890(m_icons[m_cursor], s_iconScene, 3, 0);
            char text[32];
            sprintf(text, "%d", m_cursor + 1);
            RCPtr<CNode> node = fn_803666B4(scene.Get(), text);
            UnkCsdPosition_8036AE98 position = fn_8036AE98(node.Get());
            fn_80365B18(scene.Get(), &position);
            fn_803659DC(scene.Get(), 0);
        }
        fn_8036C248(m_icons[previous]);
        {
            RCPtr<CScene> scene = fn_8036D890(m_icons[previous], s_iconScene, 1, 0);
            char text[32];
            sprintf(text, "%d", previous + 1);
            RCPtr<CNode> node = fn_803666B4(scene.Get(), text);
            UnkCsdPosition_8036AE98 position = fn_8036AE98(node.Get());
            fn_80365B18(scene.Get(), &position);
        }
        fn_801D560C(fn_80053164(), 0xE002, 0, 0);
    }

    if (IsInterrupted()) {
        m_flags.Set(3);
    }

    if (m_flags.Test(2) || m_flags.Test(3)) {
        if (m_shadow != 0) {
            fn_8036C248(m_shadow);
            delete m_shadow;
            m_shadow = 0;
        }
        for (int i = 0; i < m_count; i++) {
            fn_8036C248(m_icons[i]);
            RCPtr<CScene> scene = fn_8036D890(m_icons[i], s_iconScene, 4, 0);
            char text[32];
            sprintf(text, "%d", i + 1);
            RCPtr<CNode> node = fn_803666B4(scene.Get(), text);
            UnkCsdPosition_8036AE98 position = fn_8036AE98(node.Get());
            fn_80365B18(scene.Get(), &position);
            fn_803659DC(scene.Get(), 0);
        }
        if (m_box != 0) {
            fn_8036C248(m_box);
            RCPtr<CScene> scene = fn_8036D890(m_box, s_boxScene, 1, 0);
            fn_803659DC(scene.Get(), 0);
        }
        m_state = 3;
    }
}

void SavePointSelectImpl::UpdateClose(float dt) {
    if (IsInterrupted()) {
        m_flags.Set(1);
    } else if (m_box != 0) {
        RCPtr<CScene> scene = fn_8036D660(m_box, s_boxScene);
        if (scene->IsEnd()) {
            if (m_flags.Test(2)) {
                m_flags.Set(0);
            } else if (m_flags.Test(3)) {
                m_flags.Set(1);
            }
        }
    }
}

void SavePointSelectImpl::Draw(RednerDrawArgs &args) {
    if (m_box != 0) {
        fn_8036D94C(m_box);
    }
    for (int i = 0; i < m_count; i++) {
        if (m_icons[i] != 0) {
            fn_8036D94C(m_icons[i]);
        }
    }
    if (m_shadow != 0) {
        fn_8036D94C(m_shadow);
    }
}

bool SavePointSelectImpl::IsInterrupted() {
    if (!fn_8020261C(fn_80117578())) {
        return true;
    }
    if (fn_80074470()->field_0x8 == 6) {
        return true;
    }
    return !fn_802E5810(fn_80117514());
}

const char *SavePointSelectTask::s_taskName = "SavePointSelectTask";

SavePointSelectTask::SavePointSelectTask(Task *parent, int arg)
    : Task(parent), m_impl(new SavePointSelectImpl(s_count, arg)) {
    m_name = s_taskName;
}

SavePointSelectTask::~SavePointSelectTask() {}

void SavePointSelectTask::Tick(float dt) {
    m_impl->Tick(dt);
    if (m_impl->TestFlag(0)) {
        if (!m_onSelect.IsEmpty()) {
            Args args(m_impl->GetSelected());
            m_onSelect.Call(args);
        }
        m_flags |= 1;
    } else if (m_impl->TestFlag(1)) {
        if (!m_onCancel.IsEmpty()) {
            Args args(-1);
            m_onCancel.Call(args);
        }
        m_flags |= 1;
    }
}

/* EventArgs (class name from RTTI): counts the stage's save points (name inferred). */
class EventArgs : public SetAdapterEventArgs {
  public:
    EventArgs() : m_count(0) {}
    virtual ~EventArgs() {}

    /* 0x8 */ s32 m_count;
};

/* Counts the save points among the set objects (name inferred). */
static void *CountSavePoints(UnkPlacement_800C9CD0 *owner, void *arg) {
    EventArgs *args = (EventArgs *)arg;
    if (owner != 0 && fn_800C9D24(owner) == 5 && args != 0) {
        args->m_count++;
    }
    return owner;
}

} // namespace SavePointSelect

/* Counts the stage's save points and loads the menu's resources (name inferred). */
extern "C" void fn_80330CC4(void) {
    SavePointSelect::EventArgs args;
    args.field_0x0 = 0;
    fn_800CB47C(fn_800C9B84(), SavePointSelect::CountSavePoints, &args);
    s_count = args.m_count;
    s_enabled = 1;
    switch (s_count) {
    case 5:
        fn_8006B474(fn_8004237C(fn_8003F8D4(), "CHECKPOINT5", 1), 0);
        break;
    case 6:
        fn_8006B474(fn_8004237C(fn_8003F8D4(), "CHECKPOINT6", 1), 0);
        break;
    case 7:
        fn_8006B474(fn_8004237C(fn_8003F8D4(), "CHECKPOINT7", 1), 0);
        break;
    case 8:
        fn_8006B474(fn_8004237C(fn_8003F8D4(), "CHECKPOINT8", 1), 0);
        break;
    default:
        s_enabled = 0;
        break;
    }
}

/* Disables the menu (name inferred). */
extern "C" void fn_80330CB8(void) {
    s_enabled = 0;
}

/* Whether the stage has a save point menu (name inferred). */
extern "C" bool fn_80330CB0(void) {
    return s_enabled;
}
