/* TestNakazawa (class name from RTTI): the TESTNAKAZAWA object type, a developer's test. It is a
 * plain Task with a character info that follows its placement; each frame it casts a line 1000
 * units straight down from the character and draws a debug line from the character to the start of
 * the line (the point the ground test finds is copied into a variable that is never used). File
 * name inferred from the class. Names marked "(name inferred)" are not original. Definitions are in
 * reverse address order (deferred inlining emits functions last-defined first). */
#include "Game/CharInfo.h"
#include "Game/CharaColli.h"
#include "Game/GadgetClassDesc.h"
#include "Game/MathTypes.h"
#include "Game/Task.h"
#include "boost/shared_ptr.hpp"
#include "boost/weak_ptr.hpp"
#include <dolphin/types.h>
#include "size_assert.h"

using SonicteamUSA::System::CharaColli::CharaColliBase;

/* A line test and its hits (as in Game/BossCtrlUnit.cpp). */
struct UnkLine_80020A20 {
    u8 field_0x0[0x18];
};
struct UnkLineHits_80021FE4;

/* A hit of a line test (layout as far as used here; see Game/VehicleColliMove.cpp). */
struct UnkColliContactNode_800587A0 {
    /* 0x00 */ u8 field_0x0[0x14];
    /* 0x14 */ void *m_object;
};

extern "C" {
/* The task tree and its layer n (see Game/Gadget.h). */
void *fn_8001C4F8(void);
Task *fn_801EB304(void *root, int layer);
/* The placement (see Game/Sample.cpp; behaviour inferred). */
void fn_800C9CD0(UnkPlacement_800C9CD0 *owner, Task *task);
void fn_800C9BE8(UnkPlacement_800C9CD0 *owner);
bool fn_800C9D0C(UnkPlacement_800C9CD0 *owner);
bool fn_800C9D14(UnkPlacement_800C9CD0 *owner);
void fn_800C9CF8(UnkPlacement_800C9CD0 *owner);
bool fn_800C9A64(UnkPlacement_800C9CD0 *owner);
bool fn_800C996C(UnkPlacement_800C9CD0 *owner);
void fn_800C9F14(UnkPlacement_800C9CD0 *owner, Vec3f *position);
void fn_800C9ED4(UnkPlacement_800C9CD0 *owner, Vec3f *rotation);
/* A line segment from start to end, the collision world, a line test against it and the release of
 * its hits, the hits as a list, a hit's collider and the position of a hit object (see
 * Game/BossCtrlUnit.cpp and Game/GadgetCharCollision.cpp). */
void *fn_80020A20(UnkLine_80020A20 *line, const Vec3f *start, const Vec3f *end);
void *fn_80020D40(void);
UnkLineHits_80021FE4 *fn_80021FE4(void *world, const UnkLine_80020A20 *line, int arg2, int arg3,
                                  int arg4, int arg5);
void fn_80026FB4(UnkLineHits_80021FE4 *hits, int arg1);
UnkColliContactNode_800587A0 *fn_8001EE04(UnkLineHits_80021FE4 *hits);
UnkColliContactNode_800587A0 *fn_8001EDEC(UnkLineHits_80021FE4 *hits);
void fn_80027398(UnkColliContactNode_800587A0 **it);
boost::weak_ptr<CharaColliBase> fn_800587A0(UnkColliContactNode_800587A0 *node);
const Vec3f *fn_8001E9BC(void *object);
/* Draws a debug line (see Game/GadgetUtil.h). */
void fn_802E0B58(void *segment, u32 *arg1);
}

extern u32 lbl_805F06D8; /* passed to the debug line draw call */

class TestNakazawa : public Task {
  public:
    TestNakazawa(Task *parent, UnkPlacement_800C9CD0 *owner);
    virtual ~TestNakazawa();
    virtual void Tick(float dt);

    /* Creates the object under task layer 11 (name inferred). */
    static void Instantiate(UnkPlacement_800C9CD0 *owner);

    static const char *s_taskName;

    /* 0x28 */ UnkPlacement_800C9CD0 *m_owner;
    /* 0x2C */ CharInfo *m_chara;
    /* 0x30 */ u8 field_0x30[0x40 - 0x30]; /* never touched (size from the allocation) */
};
SIZE_ASSERT(TestNakazawa, 0x40);

const char *TestNakazawa::s_taskName = "TestNakazawa";

TestNakazawa::TestNakazawa(Task *parent, UnkPlacement_800C9CD0 *owner) : Task(parent) {
    m_name = s_taskName;
    m_owner = owner;
    fn_800C9CD0(m_owner, this);
    m_chara = new CharInfo(5);
    m_chara->field_0x5C = 4;
}

TestNakazawa::~TestNakazawa() {
    if (m_chara != 0) {
        delete m_chara;
        m_chara = 0;
    }
    fn_800C9BE8(m_owner);
}

void TestNakazawa::Tick(float dt) {
    if (fn_800C9D0C(m_owner) && fn_800C9D14(m_owner)) {
        fn_800C9F14(m_owner, &m_chara->m_position);
        fn_800C9ED4(m_owner, &m_chara->m_rotation);
        fn_800C9CF8(m_owner);
    }
    if (fn_800C9A64(m_owner)) {
        m_flags |= 1;
    } else if (fn_800C996C(m_owner)) {
        m_flags |= 1;
    } else {
        UnkLine_80020A20 line;
        UnkLine_80020A20 segment;
        Vec3f start;
        Vec3f end;
        Vec3f ground;
        fn_800091EC(&start, &m_chara->m_position);
        fn_800091EC(&end, &start);
        end.y -= 1000.0f;
        fn_800091EC(&ground, &start);
        fn_80020A20(&line, &start, &end);
        UnkColliContactNode_800587A0 *node;
        UnkLineHits_80021FE4 *hits = fn_80021FE4(fn_80020D40(), &line, 1, 0, 0, 0);
        if (hits != 0) {
            UnkColliContactNode_800587A0 *it;
            for (it = fn_8001EE04(hits); (node = it) != fn_8001EDEC(hits); fn_80027398(&it)) {
                if (node != 0) {
                    boost::shared_ptr<CharaColliBase> collider = fn_800587A0(node).lock();
                    if (collider.get() != 0) {
                        Vec3f ground;
                        fn_800091EC(&ground, fn_8001E9BC(node->m_object));
                        break;
                    }
                }
            }
        }
        fn_80026FB4(hits, 1);
        fn_802E0B58(fn_80020A20(&segment, &start, &ground), &lbl_805F06D8);
    }
}

extern "C" void fn_802DF708(void) {}

extern "C" void fn_802DF704(void) {}

void TestNakazawa::Instantiate(UnkPlacement_800C9CD0 *owner) {
    new TestNakazawa(fn_801EB304(fn_8001C4F8(), 0xB), owner);
}

GadgetClassDesc lbl_80559210 = {
    "TESTNAKAZAWA", fn_802DF708, fn_802DF704, TestNakazawa::Instantiate, 0, 0, 0x2587, 1, 0, 10, 0,
};
