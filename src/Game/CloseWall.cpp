/* CloseWall::SetCloseWall (class name from RTTI): the CLOSE WALL gadget, two wall halves
 * (CLOSEWALL.BIN) "width" apart that close in on each other at "speed" once a command of type 11
 * arrives, until they are 40 apart. Each half has its own character info. File name inferred from
 * the namespace. Names marked "(name inferred)" are not original. Definitions are in reverse
 * address order (deferred inlining emits functions last-defined first). The unit's code reaches
 * its .data through a pooled base address, so the linker keeps that section whole:
 * Game/TNumEdit.h and Game/CharaColliBaseCharInfo.h supply the RTTI base lists it starts with. */
#include <boost/bind.hpp>
#include <boost/function.hpp>

#include "Game/TNumEdit.h"
/* After TNumEdit.h: the order of the casts sets the order of the RTTI base lists in .data. */
#include "Game/CharaColliBaseCharInfo.h"
#include "Game/CharCommand.h"
#include "Game/CharCommandUnit.h"
#include "Game/CharInfo.h"
#include "Game/Gadget.h"
#include "Game/GadgetCharInfo.h"
#include "Game/GadgetClassDesc.h"
#include "Game/GadgetModelInfo.h"
#include "Game/GadgetUtil.h"
#include "Game/FileQueue.h"
#include "Game/MathTypes.h"
#include "boost/shared_ptr.hpp"
#include "boost/weak_ptr.hpp"
#include "size_assert.h"

extern "C" {
/* Gives the character info a command handler unless it has one (behaviour inferred). */
void fn_8015CF7C(GadgetCharInfo *charInfo);

/* GadgetModelInfo helpers (see BASwitch.cpp). */
boost::shared_ptr<GadgetModelInfo> fn_8015F800(int arg0, const char *fileName);
bool fn_80161FC8(GadgetModelInfo *model, int variant, const boost::weak_ptr<CharInfo> &info);

/* Plays sound id at owner (see Game/SoundTrigger.cpp; arguments other than the id unknown). */
void fn_801D53C4(void *sound, int id, void *source, void *owner, int arg4, int arg5, int arg6);
}

/* fn_8040CD74 and fn_8040CDD0 called as returning a - b and a + b by value (they store the result
 * through their first argument; see Game/ArkCannon.cpp). */
typedef Vec3f (*VectorOpFn)(const Vec3f *a, const Vec3f *b);
extern "C" void fn_8040CDD0(Vec3f *out, const Vec3f *a, const Vec3f *b);

/* The wall halves stop when they are this far apart (the game sets it in __sinit; the inline call
 * reproduces that, as in Game/CameraModeFixLook2.cpp). */
inline float DynamicFloat(float value) {
    return value;
}
static const float s_closedWidth = DynamicFloat(40.0f);

namespace CloseWall {

/* The parameters of the owner record (names from the parameter table). */
struct SetCloseWallParams {
    SetCloseWallParams() : width(40.0f), speed(4.0f) {}

    /* 0x0 */ float width;
    /* 0x4 */ float speed;
};

class SetCloseWall : public Gadget {
  public:
    SetCloseWall(Task *parent, UnkPlacement_800C9CD0 *owner);
    virtual ~SetCloseWall();
    virtual void Behave(float dt);
    virtual void ReloadPlacement(UnkPlacement_800C9CD0 *owner);

    /* Creates the gadget under task layer 11 (name inferred). */
    static void Instantiate(UnkPlacement_800C9CD0 *owner);

    /* Copies the owner's position, rotation and parameters and places the halves (name
     * inferred). */
    void LoadPlacement(UnkPlacement_800C9CD0 *owner);
    /* Places the models at the halves (name inferred). */
    void RebuildTransform();
    /* Starts closing on a command of type 11 (name inferred). */
    void OnCommand(CharCommand &command);

    static const char *s_taskName;

    /* 0x30 */ boost::shared_ptr<GadgetCharInfo> m_chara[2];
    /* 0x40 */ boost::shared_ptr<GadgetModelInfo> m_model[2];
    /* 0x50 */ SetCloseWallParams m_params;
    /* 0x58 */ s32 m_state; /* 0 placed, 1 open, 2 closing, 3 closed */
    /* 0x5C */ Vec3f m_position;
    /* 0x68 */ Vec3f m_rotation;
    /* 0x74 */ float m_halfWidth;
};
SIZE_ASSERT(SetCloseWall, 0x78);

const char *SetCloseWall::s_taskName = "SetCloseWall";

SetCloseWall::SetCloseWall(Task *parent, UnkPlacement_800C9CD0 *owner)
    : Gadget(parent, owner), m_rotation(0.0f, 0.0f, 0.0f) {
    m_name = s_taskName;
    m_chara[0] = fn_8015CD90(0x7B, 4);
    m_chara[1] = fn_8015CD90(0x7B, 4);
    fn_8015CF7C(m_chara[0].px);
    fn_800729B0(&m_chara[0]->m_commandUnit->m_callback,
                &(const CharCommandCallback &)CharCommandCallback(
                    boost::bind(&SetCloseWall::OnCommand, this, _1)));
    fn_8015CF7C(m_chara[1].px);
    fn_800729B0(&m_chara[1]->m_commandUnit->m_callback,
                &(const CharCommandCallback &)CharCommandCallback(
                    boost::bind(&SetCloseWall::OnCommand, this, _1)));
    LoadPlacement(owner);
    m_model[0] = fn_8015F800(1, "CLOSEWALL.BIN");
    m_model[0]->fn_80162A68(0, 0);
    m_model[1] = fn_8015F800(1, "CLOSEWALL.BIN");
    m_model[1]->fn_80162A68(0, 0);
    fn_80161FC8(m_model[0].px, 0, boost::weak_ptr<CharInfo>(m_chara[0]));
    fn_80161FC8(m_model[1].px, 0, boost::weak_ptr<CharInfo>(m_chara[1]));
    RebuildTransform();
}

SetCloseWall::~SetCloseWall() {}

void SetCloseWall::Behave(float dt) {
    if (m_model[0].px != 0) {
        m_model[0]->fn_80162FB0(dt);
    }
    if (m_model[1].px != 0) {
        m_model[1]->fn_80162FB0(dt);
    }
    switch (m_state) {
    case 0:
        m_state = 1;
        break;
    case 1:
        break;
    case 2: {
        m_halfWidth -= m_params.speed * dt;
        if (m_halfWidth <= s_closedWidth) {
            m_halfWidth = s_closedWidth;
            m_state = 3;
        }
        Mtx34f matrix;
        fn_8040B6F8(&matrix);
        fn_8040B848(&matrix, m_rotation.y);
        Vec3f side(1.0f, 0.0f, 0.0f);
        fn_800091D0(&side, &(const Vec3f &)fn_8040B67C(&matrix, &side));
        fn_8040CE38(&side, m_halfWidth);
        const Vec3f &left = ((VectorOpFn)fn_8040CD74)(&m_position, &side);
        fn_800091D0(&m_chara[0]->m_position, &left);
        const Vec3f &right = ((VectorOpFn)fn_8040CDD0)(&m_position, &side);
        fn_800091D0(&m_chara[1]->m_position, &right);
        fn_801D53C4(fn_80053164(), 0x7849, 0, this, 2, 0, 2);
        break;
    }
    case 3:
        break;
    }
    RebuildTransform();
}

void SetCloseWall::ReloadPlacement(UnkPlacement_800C9CD0 *owner) {
    LoadPlacement(owner);
    RebuildTransform();
}

void SetCloseWall::LoadPlacement(UnkPlacement_800C9CD0 *owner) {
    fn_800C9F14(owner, &m_position);
    fn_800C9ED4(owner, &m_rotation);
    fn_800C9DBC(owner, &m_params, sizeof(m_params));
    m_state = 0;
    m_halfWidth = 0.5f * m_params.width;
    Mtx34f matrix;
    fn_8040B6F8(&matrix);
    fn_8040B848(&matrix, m_rotation.y);
    Vec3f side(1.0f, 0.0f, 0.0f);
    fn_800091D0(&side, &(const Vec3f &)fn_8040B67C(&matrix, &side));
    fn_8040CE38(&side, m_halfWidth);
    const Vec3f &left = ((VectorOpFn)fn_8040CD74)(&m_position, &side);
    fn_800091D0(&m_chara[0]->m_position, &left);
    const Vec3f &right = ((VectorOpFn)fn_8040CDD0)(&m_position, &side);
    fn_800091D0(&m_chara[1]->m_position, &right);
    m_chara[0]->m_rotation = m_rotation;
    m_chara[1]->m_rotation = m_rotation;
}

void SetCloseWall::RebuildTransform() {
    if (m_model[0].px != 0) {
        Mtx34f matrix;
        fn_8040C148(&matrix, &lbl_805E0720);
        fn_8040B59C(&matrix, &m_chara[0]->m_position);
        fn_8040B848(&matrix, m_chara[0]->m_rotation.y);
        m_model[0]->fn_801633DC(&matrix);
    }
    if (m_model[1].px != 0) {
        Mtx34f matrix;
        fn_8040C148(&matrix, &lbl_805E0720);
        fn_8040B59C(&matrix, &m_chara[1]->m_position);
        fn_8040B848(&matrix, m_chara[1]->m_rotation.y);
        m_model[1]->fn_801633DC(&matrix);
    }
}

void SetCloseWall::OnCommand(CharCommand &command) {
    switch (command.TypeId()) {
    case 11:
        if (m_state == 1) {
            m_state = 2;
        }
        break;
    }
}

} // namespace CloseWall

extern "C" void fn_80228A34(void) {
    fn_8006B474("CLOSEWALL.BIN", 1);
}

extern "C" void fn_80228A30(void) {}

namespace CloseWall {

void SetCloseWall::Instantiate(UnkPlacement_800C9CD0 *owner) {
    Task *parent = fn_801EB304(fn_8001C4F8(), 11);
    new SetCloseWall(parent, owner);
}

} // namespace CloseWall

static GadgetParamDesc lbl_80546850[] = {
    {4, "width", 0, 0, 0, 40.0f, -65536.0f, 65536.0f, fn_800CBC20, 0},
    {4, "speed", 0, 0, 0, 4.0f, -65536.0f, 65536.0f, fn_800CBC20, 0},
    {0},
};

GadgetClassDesc lbl_805468C8 = {
    "CLOSE WALL", fn_80228A34, fn_80228A30, CloseWall::SetCloseWall::Instantiate, 0, 0, 0x1773, 1, 0,
    10, lbl_80546850,
};
