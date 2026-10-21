/* JumpBoard::JumpBoard (class name from RTTI): the JUMP_BOARD gadget, a ramp that launches the
 * characters that touch it. Its collision callback (bound with boost::bind and kept in a
 * boost::function) sends each new character position, velocity and launch commands: the launch
 * speed comes from the "level" parameter through two 16-entry tables, its direction from the
 * board's rotation tilted by "angle", and "nocontrol time (sec)" sets how long the character
 * stays uncontrollable. File name inferred from the class. Method names marked "(name inferred)"
 * are not original. Definitions are in reverse address order (deferred inlining emits functions
 * last-defined first), except the static HitCharacter, which is defined before the constructor:
 * that puts its code and its float constants where the original has them.
 *
 * Not yet matching: HitCharacter's float-to-int and int-to-float conversions share one stack slot
 * in the original, which GC/2.7 does not reproduce (see docs/research-notes.md). Everything else,
 * including the data, matches. */
#include <boost/bind.hpp>
#include <boost/function.hpp>

#include "Game/CharCommand.h"
#include "Game/CharInfo.h"
#include "Game/Gadget.h"
#include "Game/GadgetCollision.h"
#include "Game/GadgetCharInfo.h"
#include "Game/GadgetModelInfo.h"
#include "Game/GadgetClassDesc.h"
#include "Game/GadgetUtil.h"
#include "boost/weak_ptr.hpp"
#include "size_assert.h"

/* Launch speed for each level, and the matching factor for the acceleration command. */
static const float lbl_804BC1F8[16] = {
    600.0f,  900.0f,  1200.0f, 1500.0f, 1800.0f, 2100.0f, 2400.0f, 2700.0f,
    3000.0f, 3300.0f, 3600.0f, 3900.0f, 4200.0f, 4500.0f, 4800.0f, 5100.0f,
};
static const float lbl_804BC238[16] = {
    1.0f, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f, 0.0f,
};

namespace JumpBoard {

/* The three parameters the owner supplies (names from the parameter table). */
struct JumpBoardParams {
    JumpBoardParams() : level(0.0f), angle(0.0f), noControlTime(0.0f) {}

    /* 0x0 */ float level;
    /* 0x4 */ float angle; /* degrees */
    /* 0x8 */ float noControlTime;
};
SIZE_ASSERT(JumpBoardParams, 0xC);

class JumpBoard : public Gadget {
  public:
    JumpBoard(Task *parent, UnkPlacement_800C9CD0 *owner);
    virtual ~JumpBoard();
    virtual void Behave(float dt);
    virtual void ReloadPlacement(UnkPlacement_800C9CD0 *owner);

    /* Creates the board under task layer 11 (name inferred). */
    static void Instantiate(UnkPlacement_800C9CD0 *owner);

    /* The model's collision callback (name inferred). */
    void OnColliDetect(ColliDetectArgs &args);
    /* Places the model at the character info's translation and rotation (name inferred). */
    void RebuildTransform();
    /* Copies the owner's position, rotation and parameters (name inferred). */
    void LoadFromOwner(UnkPlacement_800C9CD0 *owner);

    static const char *s_taskName;

    /* 0x30 */ boost::shared_ptr<GadgetCharInfo> m_chara;
    /* 0x38 */ boost::shared_ptr<GadgetModelInfo> m_model;
    /* 0x40 */ boost::shared_ptr<GadgetUtility::Watcher> m_hitList;
    /* 0x48 */ JumpBoardParams m_params;
    /* 0x54 */ u32 pad_0x54; /* the class size is a multiple of 8 */
};

/* What the collision callback passes to the per-character hit function (name inferred). */
struct JumpBoardHitContext {
    JumpBoardHitContext() : m_params(0), m_launched(false) {}

    /* 0x00 */ boost::shared_ptr<GadgetUtility::Watcher> m_hitList;
    /* 0x08 */ boost::weak_ptr<GadgetCharInfo> m_chara;
    /* 0x10 */ JumpBoardParams *m_params;
    /* 0x14 */ bool m_launched;
};
SIZE_ASSERT(JumpBoardHitContext, 0x18);

} // namespace JumpBoard

SIZE_ASSERT(JumpBoard::JumpBoard, 0x58);

namespace JumpBoard {

const char *JumpBoard::s_taskName = "JumpBoard";

/* Launches hit unless the board already launched it (name inferred). */
static void HitCharacter(boost::shared_ptr<CharInfo> hit, void *user) {
    JumpBoardHitContext *context = static_cast<JumpBoardHitContext *>(user);
    boost::shared_ptr<GadgetCharInfo> self = context->m_chara.lock();

    if (self.get() == 0) {
        return;
    }
    if (context->m_hitList->Check(boost::weak_ptr<CharInfo>(hit))) {
        return;
    }

    hit->PushCommand(new ClrVelocityCommand(4));

    Vec3f position;
    fn_800091EC(&position, &self->m_position);
    position.y += 5.0f;
    hit->PushCommand(new SetPositionCommand(position, 4));

    int level = context->m_params->level;
    float speed = lbl_804BC1F8[level];
    if (level < 15)
        speed += (lbl_804BC1F8[level + 1] - speed) * (context->m_params->level - level);

    Mtx34f matrix;
    fn_8040C148(&matrix, &lbl_805E0720);
    fn_8040B848(&matrix, self->m_rotation.y);
    fn_8040B884(&matrix, self->m_rotation.x);
    fn_8040B80C(&matrix, self->m_rotation.z);
    fn_8040B884(&matrix, context->m_params->angle * (lbl_805FAD38 / 180.0f));

    Vec3f velocity(0.0f, 0.0f, -1.0f);
    AssignVector(velocity, fn_8040B67C(&matrix, &velocity));
    fn_8040CE38(&velocity, speed);
    hit->PushCommand(new SetVelocityCommand(velocity, 4));
    hit->PushCommand(new Player::LaunchCommand(0, 4));

    float factor = lbl_804BC238[level];
    if (level < 15)
        factor += (lbl_804BC238[level + 1] - factor) * (context->m_params->level - level);

    Vec3f acceleration(0.0f, 0.0f, -1.0f);
    AssignVector(acceleration, fn_8040B67C(&matrix, &acceleration));
    fn_8040CE38(&acceleration, factor);
    hit->PushCommand(new Player::ActingAccelerateCommand(acceleration, context->m_params->noControlTime));

    fn_80169BB0(boost::weak_ptr<CharInfo>(hit), 1);
    fn_801D560C(fn_80053164(), 0xE010, 0, 0);
    context->m_launched = true;
}

JumpBoard::JumpBoard(Task *parent, UnkPlacement_800C9CD0 *owner) : Gadget(parent, owner) {
    m_name = s_taskName;
    fn_800C9DBC(owner, &m_params, sizeof(m_params));
    m_chara = fn_8015CD90(10, 4);
    m_model = fn_8015F9D8(0, 0xD8);
    m_model->fn_80162A68(0, 0);
    m_model->fn_80161F08("col", m_chara);
    fn_800D0B64(m_model->field_0x4, boost::bind(&JumpBoard::OnColliDetect, this, _1));
    LoadFromOwner(m_owner);
    RebuildTransform();
    m_hitList = fn_801695B8();
}

JumpBoard::~JumpBoard() {
    fn_8016952C(&m_hitList);
}

void JumpBoard::Behave(float dt) {
    if (m_hitList.get() != 0) {
        m_hitList->Update(dt);
    }
    if (m_model.get() != 0) {
        m_model->fn_80162FB0(dt);
    }
}

void JumpBoard::ReloadPlacement(UnkPlacement_800C9CD0 *owner) {
    LoadFromOwner(owner);
    RebuildTransform();
    if (m_model.get() != 0) {
        m_model->fn_80162EF4();
        m_model->fn_8016108C();

        Mtx34f matrix;
        fn_8040C148(&matrix, &lbl_805E0720);
        fn_8040B59C(&matrix, &m_chara->m_position);
        fn_8040B848(&matrix, m_chara->m_rotation.y);
        fn_8040B884(&matrix, m_chara->m_rotation.x);
        fn_8040B80C(&matrix, m_chara->m_rotation.z);

        float degToRad = lbl_805FAD38 / 180.0f;
        Vec3f start(0.0f, 5.0f, 0.0f);
        Vec3f end(0.0f, 5.0f, -1000000.0f);
        fn_8040B884(&matrix, m_params.angle * degToRad);
        AssignVector(start, fn_8040B67C(&matrix, &start));
        AssignVector(end, fn_8040B67C(&matrix, &end));
        u8 segment[0x18];
        fn_802E0B58(fn_80020A20(segment, &start, &end), &lbl_805F06D8);
    }
}

void JumpBoard::LoadFromOwner(UnkPlacement_800C9CD0 *owner) {
    fn_800C9F14(owner, &m_chara->m_position);
    fn_800C9ED4(owner, &m_chara->m_rotation);
    fn_800C9DBC(owner, &m_params, sizeof(m_params));
}

void JumpBoard::RebuildTransform() {
    if (m_model.get() != 0) {
        Mtx34f matrix;

        fn_8040C148(&matrix, &lbl_805E0720);
        fn_8040B59C(&matrix, &m_chara->m_position);
        fn_8040B848(&matrix, m_chara->m_rotation.y);
        fn_8040B884(&matrix, m_chara->m_rotation.x);
        fn_8040B80C(&matrix, m_chara->m_rotation.z);
        m_model->fn_801633DC(&matrix);
    }
}

void JumpBoard::OnColliDetect(ColliDetectArgs &args) {
    JumpBoardHitContext context;

    context.m_hitList = m_hitList;
    context.m_chara = m_chara;
    context.m_params = &m_params;
    fn_8015B004(args, HitCharacter, &context);
}

} // namespace JumpBoard

extern "C" void fn_8010E570(void) {
    fn_8006B50C(0xD8, 1);
}

extern "C" void fn_8010E56C(void) {}

void JumpBoard::JumpBoard::Instantiate(UnkPlacement_800C9CD0 *owner) {
    Task *parent = fn_801EB304(fn_8001C4F8(), 11);
    new JumpBoard(parent, owner);
}

static GadgetParamDesc lbl_805266E4[] = {
    {4, "level", 0, 0, 0, 0.0f, 0.0f, 15.0f, fn_800CBC20, 0},
    {4, "angle", 0, 0, 0, 0.0f, 0.0f, 65536.0f, fn_800CBC20, 0},
    {4, "nocontrol time (sec)", 0, 0, 0, 0.0f, 0.0f, 3600.0f, fn_800CBC20, 0},
    {0},
};

GadgetClassDesc lbl_80526784 = {
    "JUMP_BOARD", fn_8010E570, fn_8010E56C, JumpBoard::JumpBoard::Instantiate, 0, 0, 4, 1, 0, 10, lbl_805266E4,
};
