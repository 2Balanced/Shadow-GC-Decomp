/* DashPanel::DashPanel (class name from RTTI): the DASHPANEL gadget, a floor panel that boosts the
 * characters that run over it. Its collision callback (bound with boost::bind and kept in a
 * boost::function) cancels the character's behaviour, places it 5 units above the panel, turns it
 * to face along the panel and sends it forward at the speed for the "level" parameter, slightly
 * pressed down; "nocontrol time (sec)" sets how long the push lasts. File name inferred from the
 * class. Method names marked "(name inferred)" are not original. Definitions are in reverse
 * address order (deferred inlining emits functions last-defined first). */
#include <boost/bind.hpp>
#include <boost/function.hpp>

#include "Game/CharCommand.h"
#include "Game/CharInfo.h"
#include "Game/Gadget.h"
#include "Game/GadgetCharCommands.h"
#include "Game/GadgetCharInfo.h"
#include "Game/GadgetCollision.h"
#include "Game/GadgetModelInfo.h"
#include "Game/GadgetClassDesc.h"
#include "Game/GadgetUtil.h"
#include "boost/weak_ptr.hpp"
#include "size_assert.h"

extern "C" {
/* vector * scale (behaviour inferred). */
Vec3f fn_8040CDA8(const Vec3f *vector, float scale);
/* The rotation angles, in axis order order, that turn the model's axes to face forward with up
 * as its up direction (behaviour inferred). */
Vec3f fn_8040F5F4(const Vec3f *forward, const Vec3f *up, int order);
/* result += value. */
void fn_8040CFF8(Vec3f *result, const Vec3f *value);
}

typedef Vec3f *(*VectorCtorFn)(Vec3f *vector, float x, float y, float z);

/* Boost speed for each level, and the matching factor for the push command. */
static const float lbl_804BABF0[16] = {
    300.0f,  600.0f,  900.0f,  1200.0f, 1500.0f, 1800.0f, 2100.0f, 2400.0f,
    2700.0f, 3000.0f, 3300.0f, 3600.0f, 3900.0f, 4200.0f, 4500.0f, 4800.0f,
};
static const float lbl_804BAC30[16] = {
    1.0f, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f, 0.0f,
};

namespace DashPanel {

/* The two parameters the owner supplies (names from the parameter table). */
struct DashPanelParams {
    DashPanelParams() : level(0.0f), noControlTime(0.0f) {}

    /* 0x0 */ float level;
    /* 0x4 */ float noControlTime;
};
SIZE_ASSERT(DashPanelParams, 0x8);

class DashPanel : public Gadget {
  public:
    DashPanel(Task *parent, UnkPlacement_800C9CD0 *owner);
    virtual ~DashPanel();
    virtual void Behave(float dt);
    virtual void ReloadPlacement(UnkPlacement_800C9CD0 *owner);

    /* Creates the panel under task layer 11 (name inferred). */
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
    /* 0x48 */ DashPanelParams m_params;
};

/* What the collision callback passes to the per-character hit function (name inferred). */
struct DashPanelHitContext {
    DashPanelHitContext() : m_params(0), m_launched(false) {}

    /* 0x00 */ boost::shared_ptr<GadgetUtility::Watcher> m_hitList;
    /* 0x08 */ boost::weak_ptr<GadgetCharInfo> m_chara;
    /* 0x10 */ DashPanelParams *m_params;
    /* 0x14 */ bool m_launched;
};
SIZE_ASSERT(DashPanelHitContext, 0x18);

} // namespace DashPanel

SIZE_ASSERT(DashPanel::DashPanel, 0x50);

namespace DashPanel {

const char *DashPanel::s_taskName = "DashPanel";

/* Boosts hit unless the panel already boosted it (name inferred). */
static void HitCharacter(boost::shared_ptr<CharInfo> hit, void *user) {
    DashPanelHitContext *context = static_cast<DashPanelHitContext *>(user);
    boost::shared_ptr<GadgetCharInfo> self = context->m_chara.lock();

    if (self.get() == 0) {
        return;
    }
    if (context->m_hitList->Check(boost::weak_ptr<CharInfo>(hit))) {
        return;
    }

    hit->PushCommand(new Player::CancelBehaviorCommand(4));
    hit->PushCommand(new ClrVelocityCommand(4));

    Mtx34f matrix;
    fn_8040C148(&matrix, &lbl_805E0720);
    fn_8040B848(&matrix, self->m_rotation.y);
    fn_8040B884(&matrix, self->m_rotation.x);
    fn_8040B80C(&matrix, self->m_rotation.z);

    /* The original builds these vectors with the vector type's operators. Their temporaries are
     * passed by address here (CodeWarrior accepts the address of a temporary with a "not an
     * lvalue" warning); fn_800091EC is the type's out-of-line copy constructor, fn_80008E1C its
     * out-of-line component constructor, which returns the vector it built. */
    Vec3f position;
    fn_800091EC(&position, &self->m_position);
    fn_8040CFF8(&position,
                &(const Vec3f &)fn_8040B67C(
                    &matrix, ((VectorCtorFn)fn_80008E1C)(&Vec3f(), 0.0f, 5.0f, 0.0f)));
    hit->PushCommand(new SetPositionCommand(position, 4));

    int level = context->m_params->level;
    float speed = lbl_804BABF0[level];
    if (level < 15)
        speed += (lbl_804BABF0[level + 1] - speed) * (context->m_params->level - level);

    Vec3f forward;
    fn_800091EC(&forward,
                &(const Vec3f &)fn_8040B67C(&matrix, &Vec3f(0.0f, 0.0f, -1.0f)));
    Vec3f up;
    fn_800091EC(&up,
                &(const Vec3f &)fn_8040B67C(&matrix, &Vec3f(0.0f, 1.0f, 0.0f)));
    Vec3f angles;
    angles = fn_8040F5F4(&forward, &up, 2);
    hit->PushCommand(new SetAngleCommand(angles, 2, 4));

    /* (0, -sin 3 degrees, -cos 3 degrees): forward and slightly down. */
    Vec3f velocity;
    fn_800091EC(&velocity, &(const Vec3f &)fn_8040CDA8(
                               &Vec3f(0.0f, -0.052335955f, -0.9986295f), speed));
    AssignVector(velocity, fn_8040B67C(&matrix, &velocity));
    hit->PushCommand(new SetVelocityCommand(velocity, 4));

    float factor = lbl_804BAC30[level];
    if (level < 15)
        factor += (lbl_804BAC30[level + 1] - factor) * (context->m_params->level - level);

    Vec3f acceleration(0.0f, 0.0f, -1.0f);
    AssignVector(acceleration, fn_8040B67C(&matrix, &acceleration));
    fn_8040CE38(&acceleration, factor);
    hit->PushCommand(
        new Player::ActingAccelerateCommand(acceleration, context->m_params->noControlTime));
    hit->PushCommand(new Player::PressGroundCommand(4));

    fn_80169BB0(boost::weak_ptr<CharInfo>(hit), 1);
    fn_801D560C(fn_80053164(), 0xE010, 0, 0);
    context->m_launched = true;
}

DashPanel::DashPanel(Task *parent, UnkPlacement_800C9CD0 *owner) : Gadget(parent, owner) {
    m_name = s_taskName;
    m_chara = fn_8015CD90(6, 4);
    LoadFromOwner(owner);
    m_model = fn_8015F9D8(0, 0xD0);
    m_model->fn_80162A68(0, 0);
    m_model->fn_80161F08("col", m_chara);
    fn_800D0B64(m_model->field_0x4, boost::bind(&DashPanel::OnColliDetect, this, _1));
    RebuildTransform();
    m_hitList = fn_801695B8();
}

DashPanel::~DashPanel() {
    fn_8016952C(&m_hitList);
}

void DashPanel::Behave(float dt) {
    if (m_hitList.get() != 0) {
        m_hitList->Update(dt);
    }
    if (m_model.get() != 0) {
        m_model->fn_80162FB0(dt);
    }
}

void DashPanel::ReloadPlacement(UnkPlacement_800C9CD0 *owner) {
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

        Vec3f start(0.0f, 0.0f, 0.0f);
        Vec3f end(0.0f, 0.0f, -1000000.0f);
        AssignVector(start, fn_8040B67C(&matrix, &start));
        AssignVector(end, fn_8040B67C(&matrix, &end));
        u8 segment[0x18];
        fn_802E0B58(fn_80020A20(segment, &start, &end), &lbl_805F06D8);
    }
}

void DashPanel::LoadFromOwner(UnkPlacement_800C9CD0 *owner) {
    fn_800C9F14(owner, &m_chara->m_position);
    fn_800C9ED4(owner, &m_chara->m_rotation);
    fn_800C9DBC(owner, &m_params, sizeof(m_params));
}

void DashPanel::RebuildTransform() {
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

void DashPanel::OnColliDetect(ColliDetectArgs &args) {
    DashPanelHitContext context;

    context.m_hitList = m_hitList;
    context.m_chara = m_chara;
    context.m_params = &m_params;
    fn_8015B004(args, HitCharacter, &context);
}

} // namespace DashPanel

extern "C" void fn_800FF04C(void) {
    fn_8006B50C(0xD0, 1);
}

extern "C" void fn_800FF048(void) {}

void DashPanel::DashPanel::Instantiate(UnkPlacement_800C9CD0 *owner) {
    Task *parent = fn_801EB304(fn_8001C4F8(), 11);
    new DashPanel(parent, owner);
}

static GadgetParamDesc lbl_8052599C[] = {
    {4, "level", 0, 0, 0, 0.0f, 0.0f, 15.0f, fn_800CBC20, 0},
    {4, "nocontrol time (sec)", 0, 0, 0, 0.0f, 0.0f, 3600.0f, fn_800CBC20, 0},
    {0},
};

GadgetClassDesc lbl_80525A14 = {
    "DASHPANEL", fn_800FF04C, fn_800FF048, DashPanel::DashPanel::Instantiate, 0, 0, 3, 1, 0, 10,
    lbl_8052599C,
};
