/* Spring::Spring (class name from RTTI): the SPRING gadget. A character that touches it is placed
 * on its contact point, turned to face along it and launched along the spring's up axis at the
 * speed for the "level" parameter; "nocontrol time (sec)" sets how long its input is ignored. The
 * spring registers a command callback on its character info: command 0x1A disables it and 0x1B
 * enables it again, together with the Target (class name from RTTI) that marks its contact point.
 * File name inferred from the class. Method names marked "(name inferred)" are not original.
 * Definitions are in reverse address order (deferred inlining emits functions last-defined
 * first). */
#include <boost/bind.hpp>
#include <boost/function.hpp>
#include <stdexcept>

#include "Game/CharCommand.h"
#include "Game/CharCommandUnit.h"
#include "Game/CharInfo.h"
#include "Game/Gadget.h"
#include "Game/GadgetCharCommands.h"
#include "Game/GadgetCharInfo.h"
#include "Game/GadgetCollision.h"
#include "Game/GadgetModelInfo.h"
#include "Game/GadgetClassDesc.h"
#include "Game/GadgetUtil.h"
#include "Game/TNumEdit.h"
/* After TNumEdit.h: the order of the casts sets the order of the RTTI base lists in .data. */
#include "Game/CharaColliBaseCharInfo.h"
#include "Game/FileQueue.h"
#include "boost/detail/sp_counted_base_impl.hpp"
#include "boost/weak_ptr.hpp"
#include "size_assert.h"

/* Target (class name from RTTI): a point other objects can aim at (meaning inferred).
 * LongSpring.cpp declares the same class; bit 0 of the word at 0x30 enables it (inferred). */
class Target {
  public:
    /* Moves the target to position (name inferred). */
    void fn_800CCE38(const Vec3f *position);

    bool IsEnabled() const {
        return m_flags & 1;
    }

    void TurnOn() {
        m_flags |= 1;
    }

    void TurnOff() {
        m_flags &= ~1;
    }

    /* 0x00 */ u8 field_0x0[0x30];
    /* 0x30 */ u32 m_flags;
};

/* A word of flag bits; the functions below construct it, clear all bits, and set, test and clear
 * one bit (behaviour inferred from their bodies). */
struct UnkFlags_80014E04 {
    /* 0x0 */ u32 m_bits;
};

extern "C" {
void fn_80014E04(UnkFlags_80014E04 *flags);
void fn_80014E10(UnkFlags_80014E04 *flags);
void fn_80014E1C(UnkFlags_80014E04 *flags, int bit);
bool fn_80014E34(const UnkFlags_80014E04 *flags, int bit);
void fn_8006127C(UnkFlags_80014E04 *flags, int bit);
/* Creates a target with the given flags, and removes a gadget's target (behaviour inferred). */
boost::shared_ptr<Target> fn_801E72DC(int flags);
void fn_801E7108(boost::shared_ptr<Target> *target);
/* Loads the model in fileName for a gadget (behaviour inferred). */
boost::shared_ptr<GadgetModelInfo> fn_8015F800(int arg0, const char *fileName);
/* The rotation angles that turn the model's axes to face forward with up as its up direction
 * (behaviour inferred). */
Vec3f fn_8040F364(const Vec3f *forward, const Vec3f *up);
}

/* Launch speed for each level. */
static const float lbl_804BD7D8[16] = {
    150.0f,  300.0f,  450.0f,  600.0f,  750.0f,  900.0f,  1050.0f, 1200.0f,
    1350.0f, 1500.0f, 1650.0f, 1800.0f, 1950.0f, 2100.0f, 2250.0f, 2400.0f,
};

namespace Spring {

/* The two parameters the owner supplies (names from the parameter table). */
struct SpringParams {
    SpringParams() : level(0.0f), noControlTime(0.0f) {}

    /* 0x0 */ float level;
    /* 0x4 */ float noControlTime;
};
SIZE_ASSERT(SpringParams, 0x8);

class Spring : public Gadget {
  public:
    Spring(Task *parent, UnkPlacement_800C9CD0 *owner);
    virtual ~Spring();
    virtual void Behave(float dt);
    virtual void ReloadPlacement(UnkPlacement_800C9CD0 *owner);

    /* Creates the spring under task layer 11 (name inferred). */
    static void Instantiate(UnkPlacement_800C9CD0 *owner);

    /* The character info's command callback (name inferred). */
    void OnCommand(CharCommand &command);
    /* The model's collision callback (name inferred). */
    void OnColliDetect(ColliDetectArgs &args);
    /* Places the model and moves the contact point and its target (name inferred). */
    void UpdatePoint();
    /* Copies the owner's position, rotation and parameters and enables the spring (name
     * inferred). */
    void LoadFromOwner(UnkPlacement_800C9CD0 *owner);

    static const char *s_taskName;

    /* 0x30 */ boost::shared_ptr<GadgetCharInfo> m_chara;
    /* 0x38 */ boost::shared_ptr<GadgetModelInfo> m_model;
    /* 0x40 */ boost::shared_ptr<Target> m_target;
    /* 0x48 */ boost::shared_ptr<UnkHitList_801695B8> m_hitList;
    /* 0x50 */ SpringParams m_params;
    /* 0x58 */ Vec3f m_point;
    /* 0x64 */ UnkFlags_80014E04 m_flags; /* bit 0: the spring is enabled */
};

/* What the collision callback passes to the per-character hit function (name inferred). */
struct SpringHitContext {
    SpringHitContext() : m_params(0), m_point(0), m_launched(false) {}

    /* 0x00 */ boost::shared_ptr<UnkHitList_801695B8> m_hitList;
    /* 0x08 */ boost::weak_ptr<GadgetCharInfo> m_chara;
    /* 0x10 */ SpringParams *m_params;
    /* 0x14 */ Vec3f *m_point;
    /* 0x18 */ bool m_launched;
};
SIZE_ASSERT(SpringHitContext, 0x1C);

} // namespace Spring

SIZE_ASSERT(Spring::Spring, 0x68);

namespace Spring {

const char *Spring::s_taskName = "Spring";

/* Launches hit from the contact point unless the spring already launched it (name inferred). */
static void HitCharacter(boost::shared_ptr<CharInfo> hit, void *user) {
    SpringHitContext *context = static_cast<SpringHitContext *>(user);
    Mtx34f matrix;
    Mtx34f launchMatrix;
    Vec3f up;
    Vec3f forward;
    Vec3f velocity;
    boost::shared_ptr<GadgetCharInfo> self = context->m_chara.lock();

    if (self.get() == 0) {
        return;
    }
    if (context->m_hitList->fn_801697CC(boost::weak_ptr<CharInfo>(hit))) {
        return;
    }

    hit->PushCommand(new SpringCommand());
    hit->PushCommand(new Player::IgnoreInputCommand(1, context->m_params->noControlTime));
    hit->PushCommand(new Player::CancelBehaviorCommand(4));
    hit->PushCommand(new ClrVelocityCommand(4));
    hit->PushCommand(new SetPositionCommand(*context->m_point, 4));

    /* The original builds up and forward with the vector type's operators; their temporaries are
     * passed by address here (CodeWarrior accepts the address of a temporary with a "not an
     * lvalue" warning), and fn_800091EC is the type's out-of-line copy constructor. */
    fn_8040C148(&matrix, &lbl_805E0720);
    fn_8040B848(&matrix, self->m_rotation.y);
    fn_8040B884(&matrix, self->m_rotation.x);
    fn_8040B80C(&matrix, self->m_rotation.z);
    fn_8040B884(&matrix, 90.0f * (lbl_805FAD38 / 180.0f));
    fn_800091EC(&up,
                &(const Vec3f &)fn_8040B67C(&matrix, &Vec3f(0.0f, 1.0f, 0.0f)));
    fn_800091EC(&forward,
                &(const Vec3f &)fn_8040B67C(&matrix, &Vec3f(0.0f, 0.0f, -1.0f)));
    hit->PushCommand(new SetAngleCommand(fn_8040F364(&forward, &up), 2, 4));

    int level = context->m_params->level;
    float speed = lbl_804BD7D8[level];
    if (level < 15) {
        speed += (lbl_804BD7D8[level + 1] - speed) * (context->m_params->level - level);
    }

    fn_8040C148(&launchMatrix, &lbl_805E0720);
    fn_8040B884(&launchMatrix, self->m_rotation.x);
    fn_8040B80C(&launchMatrix, self->m_rotation.z);
    velocity.x = 0.0f;
    velocity.y = 1.0f;
    velocity.z = 0.0f;
    AssignVector(velocity, fn_8040B67C(&launchMatrix, &velocity));
    fn_8040CE38(&velocity, speed);
    hit->PushCommand(new SetVelocityCommand(velocity, 4));
    hit->PushCommand(new Player::LaunchCommand(0, 4));

    fn_80169BB0(boost::weak_ptr<CharInfo>(hit), 5);
    fn_801D560C(fn_80053164(), 0xE00F, 0, 0);
    context->m_launched = true;
}

/* Retail addresses the vtable and the two bound member pointers from the start of the unit's .data,
 * which begins with the orphan TNumEdit and CharaColliBase RTTI base lists of Game/TNumEdit.h. */
Spring::Spring(Task *parent, UnkPlacement_800C9CD0 *owner) : Gadget(parent, owner) {
    fn_80014E04(&m_flags);
    m_name = s_taskName;
    fn_800C9DBC(owner, &m_params, sizeof(m_params));
    m_chara = fn_8015CB88(5, 4, "Spring");
    m_chara->m_commandUnit = boost::shared_ptr<CharCommandUnit>(new CharCommandUnit);
    fn_800729B0(&m_chara->m_commandUnit->m_callback,
                &(const CharCommandCallback &)CharCommandCallback(
                    boost::bind(&Spring::OnCommand, this, _1)));
    m_model = fn_8015F800(0, "SPRING.BIN");
    m_model->fn_80162A68(0, 0);
    m_model->fn_80161F08("col0", m_chara);
    fn_800D0B64(m_model->field_0x4, boost::bind(&Spring::OnColliDetect, this, _1));
    m_model->fn_80160BA0("motion0");
    m_model->fn_8015FF60();
    fn_8040CE60(&m_point);
    m_target = fn_801E72DC(0x400);
    LoadFromOwner(m_owner);
    UpdatePoint();
    m_hitList = fn_801695B8();
}

/* Not called anywhere: the linker strips it from the game, with the std::runtime_error functions
 * and records it uses. Retail's .data, which the linker keeps whole, ends with the zero-filled
 * vtable copies and the base lists of the weak classes, and holds a std::runtime_error base list
 * and vtable between those of sp_counted_base_impl and sp_counted_base. An unused function that
 * constructs a runtime_error, defined after the constructor, reproduces that layout (inferred; the
 * original function is unknown, and the name is a placeholder). */
static void UnkRuntimeErrorUser() {
    boost::throw_exception(std::runtime_error(""));
}

Spring::~Spring() {
    fn_8016952C(&m_hitList);
    fn_801E7108(&m_target);
}

void Spring::Behave(float dt) {
    if (m_hitList.get() != 0) {
        m_hitList->fn_801697F0();
    }
    if (m_model.get() != 0) {
        m_model->fn_80162FB0(dt);
    }
    UpdatePoint();
}

void Spring::ReloadPlacement(UnkPlacement_800C9CD0 *owner) {
    LoadFromOwner(owner);
    UpdatePoint();
    if (m_model.get() != 0) {
        m_model->fn_80162EF4();
        m_model->fn_8016108C();

        Vec3f start(0.0f, 10.0f, 0.0f);
        Vec3f end(0.0f, 1000000.0f, 0.0f);
        Mtx34f matrix;
        fn_8040C148(&matrix, &lbl_805E0720);
        fn_8040B59C(&matrix, &m_chara->m_position);
        fn_8040B884(&matrix, m_chara->m_rotation.x);
        fn_8040B80C(&matrix, m_chara->m_rotation.z);
        AssignVector(start, fn_8040B67C(&matrix, &start));
        AssignVector(end, fn_8040B67C(&matrix, &end));
        u8 segment[0x18];
        fn_802E0B58(fn_80020A20(segment, &start, &end), &lbl_805F06D8);
    }
}

void Spring::LoadFromOwner(UnkPlacement_800C9CD0 *owner) {
    fn_800C9F14(owner, &m_chara->m_position);
    fn_800C9ED4(owner, &m_chara->m_rotation);
    fn_800C9DBC(owner, &m_params, sizeof(m_params));
    fn_80014E10(&m_flags);
    fn_80014E1C(&m_flags, 0);
}

void Spring::UpdatePoint() {
    if (m_model.get() != 0) {
        Mtx34f matrix;

        fn_8040C148(&matrix, &lbl_805E0720);
        fn_8040B59C(&matrix, &m_chara->m_position);
        fn_8040B884(&matrix, m_chara->m_rotation.x);
        fn_8040B80C(&matrix, m_chara->m_rotation.z);
        m_model->fn_801633DC(&matrix);

        Vec3f offset(0.0f, 10.0f, 0.0f);
        AssignVector(m_point, fn_8040B67C(&matrix, &offset));
        if (m_target.get() != 0) {
            m_target->fn_800CCE38(&m_point);
        }
    }
}

void Spring::OnColliDetect(ColliDetectArgs &args) {
    if (fn_80014E34(&m_flags, 0)) {
        SpringHitContext context;

        context.m_hitList = m_hitList;
        context.m_chara = m_chara;
        context.m_params = &m_params;
        context.m_point = &m_point;
        fn_8015B004(args, HitCharacter, &context);
        if (context.m_launched && m_model.get() != 0) {
            m_model->fn_80160B18(0);
        }
    }
}

void Spring::OnCommand(CharCommand &command) {
    /* The command's id is the halfword at 0x4, which CharCommand's constructor sets. */
    switch (*reinterpret_cast<u16 *>(command.field_0x4)) {
    case 0x1A:
        fn_8006127C(&m_flags, 0);
        if (m_target->IsEnabled()) {
            m_target->TurnOff();
        }
        break;
    case 0x1B:
        fn_80014E1C(&m_flags, 0);
        if (!m_target->IsEnabled()) {
            m_target->TurnOn();
        }
        break;
    }
}

} // namespace Spring

extern "C" void fn_801201C4(void) {
    fn_8006B474("SPRING.BIN", 1);
}

extern "C" void fn_801201C0(void) {}

void Spring::Spring::Instantiate(UnkPlacement_800C9CD0 *owner) {
    new Spring(fn_801EB304(fn_8001C4F8(), 11), owner);
}

static GadgetParamDesc lbl_80527820[] = {
    {4, "level", 0, 0, 0, 0.0f, 0.0f, 15.0f, fn_800CBC20, 0},
    {4, "nocontrol time (sec)", 0, 0, 0, 0.0f, 0.0f, 3600.0f, fn_800CBC20, 0},
    {0},
};

GadgetClassDesc lbl_80527898 = {
    "SPRING", fn_801201C4, fn_801201C0, Spring::Spring::Instantiate, 0, 0, 1, 1, 0, 10, lbl_80527820,
};
