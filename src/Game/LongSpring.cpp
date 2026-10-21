/* LongSpring::LongSpring (class name from RTTI): the LONGSPRING gadget, a wide spring with three
 * contact points. A character that touches it is snapped to the nearest point and launched along
 * the spring's up axis at the speed for the "level" parameter; "nocontrol time (sec)" sets how long
 * its input is ignored. Each point carries a Target (class name from RTTI) that follows the
 * spring. File name inferred from the class. Method names marked "(name inferred)" are not
 * original. Definitions are in reverse address order (deferred inlining emits functions
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
#include "Game/GadgetCharInfo.h"
#include "Game/GadgetCollision.h"
#include "Game/GadgetModelInfo.h"
#include "Game/GadgetClassDesc.h"
#include "Game/GadgetUtil.h"
#include "boost/weak_ptr.hpp"
#include "size_assert.h"

/* Target (class name from RTTI): a point other objects can aim at (meaning inferred). */
class Target {
  public:
    /* Moves the target to position (name inferred). */
    void fn_800CCE38(const Vec3f *position);
};

extern "C" {
/* Creates a target with the given flags (behaviour inferred). */
boost::shared_ptr<Target> fn_801E72DC(int flags);
/* Removes a gadget's target (behaviour inferred). */
void fn_801E7108(boost::shared_ptr<Target> *target);
}

/* Launch speed for each level. */
static const float lbl_804BC388[16] = {
    150.0f,  300.0f,  450.0f,  600.0f,  750.0f,  900.0f,  1050.0f, 1200.0f,
    1350.0f, 1500.0f, 1650.0f, 1800.0f, 1950.0f, 2100.0f, 2250.0f, 2400.0f,
};

namespace LongSpring {

/* The two parameters the owner supplies (names from the parameter table). */
struct LongSpringParams {
    LongSpringParams() : level(0.0f), noControlTime(0.0f) {}

    /* 0x0 */ float level;
    /* 0x4 */ float noControlTime;
};
SIZE_ASSERT(LongSpringParams, 0x8);

class LongSpring : public Gadget {
  public:
    LongSpring(Task *parent, UnkPlacement_800C9CD0 *owner);
    virtual ~LongSpring();
    virtual void Behave(float dt);
    virtual void ReloadPlacement(UnkPlacement_800C9CD0 *owner);

    /* Creates the spring under task layer 11 (name inferred). */
    static void Instantiate(UnkPlacement_800C9CD0 *owner);

    /* The model's collision callback (name inferred). */
    void OnColliDetect(ColliDetectArgs &args);
    /* Places the model and moves the three contact points and their targets (name inferred). */
    void UpdatePoints();
    /* Copies the owner's position, rotation and parameters (name inferred). */
    void LoadFromOwner(UnkPlacement_800C9CD0 *owner);

    static const char *s_taskName;

    /* 0x30 */ boost::shared_ptr<GadgetCharInfo> m_chara;
    /* 0x38 */ boost::shared_ptr<GadgetModelInfo> m_model;
    /* 0x40 */ boost::shared_ptr<GadgetUtility::Watcher> m_hitList;
    /* 0x48 */ LongSpringParams m_params;
    /* 0x50 */ Vec3f m_points[3];
    /* 0x74 */ boost::shared_ptr<Target> m_targets[3];
    /* 0x8C */ u32 pad_0x8C; /* the class size is a multiple of 8 */
};

/* What the collision callback passes to the per-character hit function (name inferred). */
struct LongSpringHitContext {
    LongSpringHitContext() : m_params(0), m_points(0), m_launched(false) {}

    /* 0x00 */ boost::shared_ptr<GadgetUtility::Watcher> m_hitList;
    /* 0x08 */ boost::weak_ptr<GadgetCharInfo> m_chara;
    /* 0x10 */ LongSpringParams *m_params;
    /* 0x14 */ Vec3f *m_points;
    /* 0x18 */ bool m_launched;
};
SIZE_ASSERT(LongSpringHitContext, 0x1C);

} // namespace LongSpring

SIZE_ASSERT(LongSpring::LongSpring, 0x90);

namespace LongSpring {

const char *LongSpring::s_taskName = "LongSpring";

/* Launches hit from the nearest contact point unless the spring already launched it (name
 * inferred). */
static void HitCharacter(boost::shared_ptr<CharInfo> hit, void *user) {
    LongSpringHitContext *context = static_cast<LongSpringHitContext *>(user);
    Mtx34f matrix;
    Vec3f velocity;
    boost::shared_ptr<GadgetCharInfo> self = context->m_chara.lock();

    if (self.get() == 0) {
        return;
    }
    if (context->m_hitList->Check(boost::weak_ptr<CharInfo>(hit))) {
        return;
    }

    hit->PushCommand(new SpringCommand());
    hit->PushCommand(new Player::IgnoreInputCommand(1, context->m_params->noControlTime));
    hit->PushCommand(new ClrVelocityCommand(4));

    float nearestDistance = 100000000.0f;
    int nearest = 0;
    for (int i = 0; i < 3; i++) {
        Vec3f offset;
        fn_8040CD74(&offset, &hit->m_position, &context->m_points[i]);
        float distance = fn_8040CF2C(&offset);
        if (distance < nearestDistance) {
            nearestDistance = distance;
            nearest = i;
        }
    }

    int level = context->m_params->level;
    float speed = lbl_804BC388[level];
    if (level < 15) {
        speed += (lbl_804BC388[level + 1] - speed) * (context->m_params->level - level);
    }

    hit->PushCommand(new SetPositionCommand(context->m_points[nearest], 4));

    fn_8040C148(&matrix, &lbl_805E0720);
    fn_8040B848(&matrix, self->m_rotation.y);
    fn_8040B884(&matrix, self->m_rotation.x);
    fn_8040B80C(&matrix, self->m_rotation.z);

    velocity.x = 0.0f;
    velocity.y = 1.0f;
    velocity.z = 0.0f;
    AssignVector(velocity, fn_8040B67C(&matrix, &velocity));
    fn_8040CE38(&velocity, speed);
    hit->PushCommand(new SetVelocityCommand(velocity, 4));
    hit->PushCommand(new Player::LaunchCommand(1, 4));

    fn_80169BB0(boost::weak_ptr<CharInfo>(hit), 5);
    fn_801D560C(fn_80053164(), 0xE00F, 0, 0);
    context->m_launched = true;
}

LongSpring::LongSpring(Task *parent, UnkPlacement_800C9CD0 *owner) : Gadget(parent, owner) {
    m_name = s_taskName;
    fn_800C9DBC(owner, &m_params, sizeof(m_params));
    m_chara = fn_8015CB88(5, 4, "LongSpring");
    m_model = fn_8015F9D8(0, 0xD9);
    m_model->fn_80162A68(0, 0);
    m_model->fn_80161F08("col", m_chara);
    fn_800D0B64(m_model->field_0x4, boost::bind(&LongSpring::OnColliDetect, this, _1));
    m_model->fn_80160BA0("motion");
    m_model->fn_8015FF60();
    for (int i = 0; i < 3; i++) {
        fn_8040CE60(&m_points[i]);
        m_targets[i] = fn_801E72DC(0x400);
    }
    LoadFromOwner(m_owner);
    UpdatePoints();
    m_hitList = fn_801695B8();
}

LongSpring::~LongSpring() {
    fn_8016952C(&m_hitList);
    for (int i = 0; i < 3; i++) {
        fn_801E7108(&m_targets[i]);
    }
}

void LongSpring::Behave(float dt) {
    if (m_hitList.get() != 0) {
        m_hitList->Update(dt);
    }
    if (m_model.get() != 0) {
        m_model->fn_80162FB0(dt);
    }
    UpdatePoints();
}

void LongSpring::ReloadPlacement(UnkPlacement_800C9CD0 *owner) {
    LoadFromOwner(owner);
    UpdatePoints();
    if (m_model.get() != 0) {
        m_model->fn_80162EF4();
        m_model->fn_8016108C();

        Mtx34f matrix;
        u8 segment[0x18];
        Vec3f start;
        Vec3f end;

        /* The contact points and points far above them, for the debug lines. */
        static s8 s_lowInitialized;
        static float s_low[3][3];
        if (!s_lowInitialized) {
            Vec3f s_low0(0.0f, 10.0f, 10.0f);
            fn_800091EC((Vec3f *)s_low[0], &s_low0);
            Vec3f s_low1(20.0f, 10.0f, 10.0f);
            fn_800091EC((Vec3f *)s_low[1], &s_low1);
            Vec3f s_low2(-20.0f, 10.0f, 10.0f);
            fn_800091EC((Vec3f *)s_low[2], &s_low2);
            s_lowInitialized = 1;
        }
        static s8 s_highInitialized;
        static float s_high[3][3];
        if (!s_highInitialized) {
            Vec3f s_high0(0.0f, 1000000.0f, 10.0f);
            fn_800091EC((Vec3f *)s_high[0], &s_high0);
            Vec3f s_high1(20.0f, 1000000.0f, 10.0f);
            fn_800091EC((Vec3f *)s_high[1], &s_high1);
            Vec3f s_high2(-20.0f, 1000000.0f, 10.0f);
            fn_800091EC((Vec3f *)s_high[2], &s_high2);
            s_highInitialized = 1;
        }

        fn_8040C148(&matrix, &lbl_805E0720);
        fn_8040B59C(&matrix, &m_chara->m_position);
        fn_8040B848(&matrix, m_chara->m_rotation.y);
        fn_8040B884(&matrix, m_chara->m_rotation.x);
        fn_8040B80C(&matrix, m_chara->m_rotation.z);

        for (int i = 0; i < 3; i++) {
            AssignVector(start, fn_8040B67C(&matrix, (Vec3f *)s_low[i]));
            AssignVector(end, fn_8040B67C(&matrix, (Vec3f *)s_high[i]));
            fn_802E0B58(fn_80020A20(segment, &start, &end), &lbl_805F06D8);
        }
    }
}

void LongSpring::LoadFromOwner(UnkPlacement_800C9CD0 *owner) {
    fn_800C9F14(owner, &m_chara->m_position);
    fn_800C9ED4(owner, &m_chara->m_rotation);
    fn_800C9DBC(owner, &m_params, sizeof(m_params));
}

void LongSpring::UpdatePoints() {
    if (m_model.get() != 0) {
        Mtx34f matrix;

        fn_8040C148(&matrix, &lbl_805E0720);
        fn_8040B59C(&matrix, &m_chara->m_position);
        fn_8040B848(&matrix, m_chara->m_rotation.y);
        fn_8040B884(&matrix, m_chara->m_rotation.x);
        fn_8040B80C(&matrix, m_chara->m_rotation.z);
        m_model->fn_801633DC(&matrix);

        static s8 s_offsetsInitialized;
        static float s_offsets[3][3];
        if (!s_offsetsInitialized) {
            Vec3f s_offsets0(0.0f, 10.0f, 10.0f);
            fn_800091EC((Vec3f *)s_offsets[0], &s_offsets0);
            Vec3f s_offsets1(20.0f, 10.0f, 10.0f);
            fn_800091EC((Vec3f *)s_offsets[1], &s_offsets1);
            Vec3f s_offsets2(-20.0f, 10.0f, 10.0f);
            fn_800091EC((Vec3f *)s_offsets[2], &s_offsets2);
            s_offsetsInitialized = 1;
        }

        for (int i = 0; i < 3; i++) {
            AssignVector(m_points[i], fn_8040B67C(&matrix, (Vec3f *)s_offsets[i]));
            if (m_targets[i].get() != 0) {
                m_targets[i]->fn_800CCE38(&m_points[i]);
            }
        }
    }
}

void LongSpring::OnColliDetect(ColliDetectArgs &args) {
    LongSpringHitContext context;

    context.m_hitList = m_hitList;
    context.m_chara = m_chara;
    context.m_params = &m_params;
    context.m_points = m_points;
    fn_8015B004(args, HitCharacter, &context);
    if (context.m_launched && m_model.get() != 0) {
        m_model->fn_80160B18(0);
    }
}

} // namespace LongSpring

extern "C" void fn_8010F560(void) {
    fn_8006B50C(0xD9, 1);
}

extern "C" void fn_8010F55C(void) {}

void LongSpring::LongSpring::Instantiate(UnkPlacement_800C9CD0 *owner) {
    new LongSpring(fn_801EB304(fn_8001C4F8(), 11), owner);
}

static GadgetParamDesc lbl_805267F4[] = {
    {4, "level", 0, 0, 0, 0.0f, 0.0f, 15.0f, fn_800CBC20, 0},
    {4, "nocontrol time (sec)", 0, 0, 0, 0.0f, 0.0f, 3600.0f, fn_800CBC20, 0},
    {0},
};

GadgetClassDesc lbl_8052686C = {
    "LONGSPRING", fn_8010F560, fn_8010F55C, LongSpring::LongSpring::Instantiate, 0, 0, 2, 1, 0, 10, lbl_805267F4,
};
