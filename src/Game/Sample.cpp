/* Sample (class name from RTTI): a sample object type for the stage editor, a plain Task (not a
 * Gadget) that only follows its placement's position. Its parameter table shows each parameter
 * kind ("Single", "Hex", "Uint32" and a run of "Sint32"). File name inferred from the class. Names
 * marked "(name inferred)" are not original. Definitions are in reverse address order (deferred
 * inlining emits functions last-defined first). */
#include "Game/GadgetClassDesc.h"
#include "Game/MathTypes.h"
#include "Game/Task.h"
#include <dolphin/types.h>
#include "size_assert.h"

extern "C" {
/* The task tree and its layer n (see Game/Gadget.h). */
void *fn_8001C4F8(void);
Task *fn_801EB304(void *root, int layer);
/* The placement: attaching a task to it, detaching it, whether it is active, whether it changed
 * (and clearing that), whether it went away, and its position (see Game/BossRootTask.cpp and
 * Game/Gadget.h; behaviour inferred). */
void fn_800C9CD0(UnkPlacement_800C9CD0 *owner, Task *task);
void fn_800C9BE8(UnkPlacement_800C9CD0 *owner);
bool fn_800C9D0C(UnkPlacement_800C9CD0 *owner);
bool fn_800C9D14(UnkPlacement_800C9CD0 *owner);
void fn_800C9CF8(UnkPlacement_800C9CD0 *owner);
bool fn_800C9A64(UnkPlacement_800C9CD0 *owner);
bool fn_800C996C(UnkPlacement_800C9CD0 *owner);
void fn_800C9F14(UnkPlacement_800C9CD0 *owner, Vec3f *position);
}

class Sample : public Task {
  public:
    Sample(Task *parent, UnkPlacement_800C9CD0 *owner);
    virtual ~Sample();
    virtual void Tick(float dt);

    /* Creates the object under task layer 11 (name inferred). */
    static void Instantiate(UnkPlacement_800C9CD0 *owner);

    static const char *s_taskName;

    /* 0x28 */ Vec3f m_position;
    /* 0x34 */ UnkPlacement_800C9CD0 *m_owner;
};
SIZE_ASSERT(Sample, 0x38);

const char *Sample::s_taskName = "Sample";

extern "C" void fn_802DF6AC(void) {}

extern "C" void fn_802DF6A8(void) {}

void Sample::Instantiate(UnkPlacement_800C9CD0 *owner) {
    new Sample(fn_801EB304(fn_8001C4F8(), 0xB), owner);
}

Sample::Sample(Task *parent, UnkPlacement_800C9CD0 *owner) : Task(parent) {
    m_name = s_taskName;
    if (owner != 0) {
        m_owner = owner;
        fn_800C9CD0(m_owner, this);
    }
}

Sample::~Sample() {
    if (m_owner != 0) {
        fn_800C9BE8(m_owner);
        m_owner = 0;
    }
}

void Sample::Tick(float dt) {
    if (m_owner != 0) {
        if (fn_800C9D0C(m_owner) && fn_800C9D14(m_owner)) {
            fn_800C9F14(m_owner, &m_position);
            fn_800C9CF8(m_owner);
        }
        if (fn_800C9A64(m_owner)) {
            m_flags |= 1;
        } else if (fn_800C996C(m_owner)) {
            m_flags |= 1;
        }
    }
}

/* The label function of the last parameter: it supplies no label. */
extern "C" const char *fn_802DF4D8(UnkLabelQuery_800CBC20 *query) {
    return 0;
}

static GadgetParamDesc lbl_80558F28[] = {
    {4, "Single", 0, 0, 0, 0.0f, 0.0f, 100.0f, fn_800CBC20, 0},
    {3, "Hex", 0, 0, 256, 0.0f, 0.0f, 0.0f, fn_800CBC20, 0},
    {2, "Uint32", 0, 0, 100, 0.0f, 0.0f, 0.0f, fn_800CBC20, 0},
    {1, "Sint32", 1, 0, 100, 0.0f, 0.0f, 0.0f, fn_800CBC20, 0},
    {1, "Sint32", 2, 0, 100, 0.0f, 0.0f, 0.0f, fn_800CBC20, 0},
    {1, "Sint32", 3, 0, 100, 0.0f, 0.0f, 0.0f, fn_800CBC20, 0},
    {1, "Sint32", 4, 0, 100, 0.0f, 0.0f, 0.0f, fn_800CBC20, 0},
    {1, "Sint32", 5, 0, 100, 0.0f, 0.0f, 0.0f, fn_800CBC20, 0},
    {1, "Sint32", 6, 0, 100, 0.0f, 0.0f, 0.0f, fn_800CBC20, 0},
    {1, "Sint32", 7, 0, 100, 0.0f, 0.0f, 0.0f, fn_800CBC20, 0},
    {1, "Sint32", 8, 0, 100, 0.0f, 0.0f, 0.0f, fn_800CBC20, 0},
    {1, "Sint32", 9, 0, 100, 0.0f, 0.0f, 0.0f, fn_800CBC20, 0},
    {1, "Sint32", 10, 0, 100, 0.0f, 0.0f, 0.0f, fn_800CBC20, 0},
    {1, "Sint32", 11, 0, 100, 0.0f, 0.0f, 0.0f, fn_800CBC20, 0},
    {1, "Sint32", 12, 0, 100, 0.0f, 0.0f, 0.0f, fn_800CBC20, 0},
    {1, "Sint32", 13, 0, 100, 0.0f, 0.0f, 0.0f, fn_802DF4D8, 0},
    {0},
};

GadgetClassDesc lbl_805591D0 = {
    "Sample", fn_802DF6AC, fn_802DF6A8, Sample::Instantiate, 0, 0, 0x2586, 1, 0, 10, lbl_80558F28,
};
