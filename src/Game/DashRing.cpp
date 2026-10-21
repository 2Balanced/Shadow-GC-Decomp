/* DashRing::DashRing (class name from RTTI): the DASHRING gadget, a ring that a character flies
 * through. It snaps the character to the ring's centre, faces it along the ring's axis and
 * launches it at the speed for the "level" parameter; "nocontrol time (sec)" sets how long its
 * input is ignored. The ring keeps a looping sound attached to itself. File name inferred from
 * the class. Method names marked "(name inferred)" are not original. Definitions are in reverse
 * address order (deferred inlining emits functions last-defined first), except the static
 * HitCharacter, which is defined before the constructor: that puts its code and its float
 * constants where the original has them.
 *
 * Not yet matching: HitCharacter's float-to-int and int-to-float conversions share one stack slot
 * in the original, which GC/2.7 does not reproduce (see docs/research-notes.md). Everything else,
 * including the data, matches. */
#include <boost/bind.hpp>
#include <boost/function.hpp>

#include "Game/CharCommand.h"
#include "Game/CharInfo.h"
#include "Game/Gadget.h"
#include "Game/GadgetCharInfo.h"
#include "Game/GadgetCollision.h"
#include "Game/GadgetModelInfo.h"
#include "Game/GadgetSound.h"
#include "Game/GadgetClassDesc.h"
#include "Game/GadgetUtil.h"
#include "boost/weak_ptr.hpp"
#include "size_assert.h"

extern "C" {
/* The angles that face direction with up as the given axis (behaviour inferred). */
Vec3f fn_8040F5F4(const Vec3f *direction, const Vec3f *up, int axis);
}

/* Launch speed for each level. */
static const float lbl_804BAD80[16] = {
    400.0f,  600.0f,  800.0f,  1000.0f, 1200.0f, 1400.0f, 1600.0f, 1800.0f,
    2000.0f, 2200.0f, 2400.0f, 2600.0f, 2800.0f, 3000.0f, 3200.0f, 3400.0f,
};

namespace DashRing {

/* The two parameters the owner supplies (names from the parameter table). */
struct DashRingParams {
    DashRingParams() : level(0.0f), noControlTime(0.0f) {}

    /* 0x0 */ float level;
    /* 0x4 */ float noControlTime;
};
SIZE_ASSERT(DashRingParams, 0x8);

class DashRing : public Gadget {
  public:
    DashRing(Task *parent, UnkPlacement_800C9CD0 *owner);
    virtual ~DashRing();
    virtual void Behave(float dt);
    virtual void ReloadPlacement(UnkPlacement_800C9CD0 *owner);

    /* Creates the ring under task layer 11 (name inferred). */
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
    /* 0x48 */ DashRingParams m_params;
    /* 0x50 */ UnkSoundHandle_801D3DC8 m_sound;
    /* 0x6C */ u32 pad_0x6C; /* the class size is a multiple of 8 */
};

/* What the collision callback passes to the per-character hit function (name inferred). */
struct DashRingHitContext {
    DashRingHitContext() : m_params(0), m_launched(false) {}

    /* 0x00 */ boost::shared_ptr<GadgetUtility::Watcher> m_hitList;
    /* 0x08 */ boost::weak_ptr<GadgetCharInfo> m_chara;
    /* 0x10 */ DashRingParams *m_params;
    /* 0x14 */ bool m_launched;
};
SIZE_ASSERT(DashRingHitContext, 0x18);

} // namespace DashRing

SIZE_ASSERT(DashRing::DashRing, 0x70);

namespace DashRing {

const char *DashRing::s_taskName = "DashRing";

/* Launches hit through the ring unless the ring already launched it (name inferred). */
static void HitCharacter(boost::shared_ptr<CharInfo> hit, void *user) {
    DashRingHitContext *context = static_cast<DashRingHitContext *>(user);
    Mtx34f matrix;
    Vec3f velocity;
    Vec3f up;
    boost::shared_ptr<GadgetCharInfo> self = context->m_chara.lock();

    if (self.get() == 0) {
        return;
    }
    if (context->m_hitList->Check(boost::weak_ptr<CharInfo>(hit))) {
        return;
    }

    hit->PushCommand(new Player::IgnoreInputCommand(0, context->m_params->noControlTime));
    hit->PushCommand(new ClrVelocityCommand(4));
    hit->PushCommand(new SetPositionCommand(self->m_position, 4));

    int level = context->m_params->level;
    float speed = lbl_804BAD80[level];
    if (level < 15) {
        speed += (lbl_804BAD80[level + 1] - speed) * (context->m_params->level - level);
    }

    fn_8040C148(&matrix, &lbl_805E0720);
    fn_8040B848(&matrix, self->m_rotation.y);
    fn_8040B884(&matrix, self->m_rotation.x);
    fn_8040B80C(&matrix, self->m_rotation.z);

    velocity.x = 0.0f;
    velocity.y = 0.0f;
    velocity.z = -1.0f;
    AssignVector(velocity, fn_8040B67C(&matrix, &velocity));
    fn_8040CE38(&velocity, speed);

    up.x = 0.0f;
    up.y = 1.0f;
    up.z = 0.0f;
    Vec3f angles = fn_8040F5F4(&velocity, &up, 2);
    hit->PushCommand(new SetAngleCommand(angles, 2, 4));
    hit->PushCommand(new SetVelocityCommand(velocity, 4));
    hit->PushCommand(new Player::LaunchCommand(0, 4));

    fn_80169BB0(boost::weak_ptr<CharInfo>(hit), 1);
    fn_801D560C(fn_80053164(), 0xE012, 0, 0);
    context->m_launched = true;
}

DashRing::DashRing(Task *parent, UnkPlacement_800C9CD0 *owner) : Gadget(parent, owner) {
    m_name = s_taskName;
    fn_800C9DBC(owner, &m_params, sizeof(m_params));
    m_chara = fn_8015CD90(11, 4);
    m_model = fn_8015F9D8(0, 0xD1);
    m_model->fn_80162A68(0, 0);
    m_model->fn_80161F08("col", m_chara);
    fn_800D0B64(m_model->field_0x4, boost::bind(&DashRing::OnColliDetect, this, _1));
    LoadFromOwner(m_owner);
    RebuildTransform();
    m_sound = fn_801D3DC8(0xE011, m_chara.px, this);
    m_hitList = fn_801695B8();
}

DashRing::~DashRing() {
    fn_8016952C(&m_hitList);
    m_sound.fn_801D455C();
}

void DashRing::Behave(float dt) {
    if (m_hitList.get() != 0) {
        m_hitList->Update(dt);
    }
    if (m_model.get() != 0) {
        m_model->fn_80162FB0(dt);
    }
}

void DashRing::ReloadPlacement(UnkPlacement_800C9CD0 *owner) {
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

void DashRing::LoadFromOwner(UnkPlacement_800C9CD0 *owner) {
    fn_800C9F14(owner, &m_chara->m_position);
    fn_800C9ED4(owner, &m_chara->m_rotation);
    fn_800C9DBC(owner, &m_params, sizeof(m_params));
}

void DashRing::RebuildTransform() {
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

void DashRing::OnColliDetect(ColliDetectArgs &args) {
    DashRingHitContext context;

    context.m_hitList = m_hitList;
    context.m_chara = m_chara;
    context.m_params = &m_params;
    fn_8015B004(args, HitCharacter, &context);
}

} // namespace DashRing

extern "C" void fn_801001F4(void) {
    fn_8006B50C(0xD1, 1);
}

extern "C" void fn_801001F0(void) {}

void DashRing::DashRing::Instantiate(UnkPlacement_800C9CD0 *owner) {
    Task *parent = fn_801EB304(fn_8001C4F8(), 11);
    new DashRing(parent, owner);
}

static GadgetParamDesc lbl_80525A84[] = {
    {4, "level", 0, 0, 0, 0.0f, 0.0f, 15.0f, fn_800CBC20, 0},
    {4, "nocontrol time (sec)", 0, 0, 0, 0.0f, 0.0f, 3600.0f, fn_800CBC20, 0},
    {0},
};

GadgetClassDesc lbl_80525AFC = {
    "DASHRING", fn_801001F4, fn_801001F0, DashRing::DashRing::Instantiate, 0, 0, 6, 1, 0, 10, lbl_80525A84,
};
