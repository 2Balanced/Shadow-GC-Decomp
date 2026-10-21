/* SpringPanel (class name from RTTI): the ELEC_SPRING gadget of the electric circuit area, a
 * spring panel (SPRINGPANEL.BIN). A character that touches it is placed on its contact point,
 * turned to the panel's heading and launched along the panel's up axis at the speed for the
 * "level" parameter; "nocontrol time (sec)" sets how long its input is ignored. Its "mat"
 * material cycles through three colours once a second. Like Spring, it registers a command
 * callback on its character info: command 0x1A disables the Target (class name from RTTI) that
 * marks its contact point and 0x1B enables it again. File name inferred from the class. Method
 * names marked "(name inferred)" are not original. Definitions are in reverse address order
 * (deferred inlining emits functions last-defined first). */
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
#include "Game/Vibration.h"
#include "Game/FileQueue.h"
#include "boost/detail/sp_counted_base_impl.hpp"
#include "boost/weak_ptr.hpp"
#include "size_assert.h"

/* Target (class name from RTTI): a point other objects can aim at (meaning inferred; as in
 * Spring.cpp). Bit 0 of the word at 0x30 enables it (inferred). */
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

/* Four colour bytes (red, green, blue, alpha; inferred) built by an out-of-line constructor (as in
 * ElecBarrier.cpp). */
struct UnkColor_8040D2D8 {
    UnkColor_8040D2D8(u8 r, u8 g, u8 b, u8 a);

    /* 0x0 */ u8 r;
    /* 0x1 */ u8 g;
    /* 0x2 */ u8 b;
    /* 0x3 */ u8 a;
};
SIZE_ASSERT(UnkColor_8040D2D8, 0x4);

/* The material animation of a model fn_801621F0 finds by name (layout unused here). */
struct UnkMaterialAnim_801621F0;

/* The fields of the game state used here (layout from its users; see FootingMovable.cpp). */
struct UnkGameState_8057D6A0 {
    /* 0x00 */ u8 field_0x0[0x94];
    /* 0x94 */ float m_time; /* seconds */
};

extern UnkGameState_8057D6A0 lbl_8057D6A0;

extern "C" {
/* Creates a target with the given flags, and removes a gadget's target (behaviour inferred). */
boost::shared_ptr<Target> fn_801E72DC(int flags);
void fn_801E7108(boost::shared_ptr<Target> *target);
/* Calls callback with every character in the collision event (behaviour inferred). */
void fn_8015AA84(ColliDetectArgs &args, void (*callback)(boost::shared_ptr<CharInfo> hit, void *user),
                 void *user);
/* Sets the motion speed of the model (behaviour inferred; see Catapult.cpp). */
void fn_80161308(GadgetModelInfo *model, float speed);
/* GadgetModelInfo helpers (see ElecCristalWall.cpp): find the material animation named name, and
 * select the material named name. */
UnkMaterialAnim_801621F0 *fn_801621F0(GadgetModelInfo *model, const char *name);
void fn_8016239C(GadgetModelInfo *model, const char *name);
/* Blends animation's colour from from to to as t goes from t0 to t1 (behaviour inferred). */
void fn_80154C6C(UnkMaterialAnim_801621F0 *animation, const UnkColor_8040D2D8 *from,
                 const UnkColor_8040D2D8 *to, float t, float t0, float t1);
/* Plays sound id at position (behaviour inferred; the other arguments are unknown). */
void fn_801D54F4(void *manager, int id, const Vec3f *position, int arg3, int arg4);
/* Starts controller vibration pattern (behaviour inferred). */
boost::weak_ptr<Vibration::RequestHandle> fn_8034DF48(int pattern, int arg1);
}

/* Launch speed for each level (as in Spring.cpp). */
static const float lbl_804C2238[16] = {
    150.0f,  300.0f,  450.0f,  600.0f,  750.0f,  900.0f,  1050.0f, 1200.0f,
    1350.0f, 1500.0f, 1650.0f, 1800.0f, 1950.0f, 2100.0f, 2250.0f, 2400.0f,
};

/* The two parameters the owner supplies (names from the parameter table). */
struct SpringPanelParams {
    /* 0x0 */ float level;
    /* 0x4 */ float noControlTime;
};
SIZE_ASSERT(SpringPanelParams, 0x8);

class SpringPanel : public Gadget {
  public:
    SpringPanel(Task *parent, UnkPlacement_800C9CD0 *owner);
    virtual ~SpringPanel();
    virtual void Behave(float dt);
    virtual void fn_800CBC1C();
    virtual void ReloadPlacement(UnkPlacement_800C9CD0 *owner);

    /* Creates the panel under task layer 11 (name inferred). */
    static void Instantiate(UnkPlacement_800C9CD0 *owner);

    /* The character info's command callback (name inferred). */
    void OnCommand(CharCommand &command);
    /* The model's collision callback (name inferred). */
    void OnColliDetect(ColliDetectArgs &args);
    /* Places the model and moves the contact point and its target (name inferred). */
    void UpdatePoint();
    /* Copies the owner's position, rotation and parameters (name inferred). */
    void LoadFromOwner(UnkPlacement_800C9CD0 *owner);
    /* Cycles the "mat" material through its three colours once a second (name inferred). */
    void UpdateColor();

    static const char *s_taskName;

    /* 0x30 */ Vec3f m_point;
    /* 0x3C */ boost::shared_ptr<GadgetCharInfo> m_chara;
    /* 0x44 */ GadgetModelInfo *m_model;
    /* 0x48 */ SpringPanelParams m_params;
    /* 0x50 */ boost::shared_ptr<Target> m_target;
};
SIZE_ASSERT(SpringPanel, 0x58);

/* What the collision callback passes to the per-character hit function (name inferred). */
struct SpringPanelHitContext {
    SpringPanelHitContext() : m_params(0), m_point(0), m_launched(false) {}

    /* 0x00 */ boost::weak_ptr<GadgetCharInfo> m_chara;
    /* 0x08 */ SpringPanelParams *m_params;
    /* 0x0C */ Vec3f *m_point;
    /* 0x10 */ bool m_launched;
};
SIZE_ASSERT(SpringPanelHitContext, 0x14);

const char *SpringPanel::s_taskName = "SpringPanel";

/* The three colours of the "mat" material. */
static UnkColor_8040D2D8 lbl_805EF1A4(0xFF, 0x33, 0x00, 0x00);
static UnkColor_8040D2D8 lbl_805EF1A8(0xFF, 0x7F, 0x00, 0x00);
static UnkColor_8040D2D8 lbl_805EF1AC(0xFF, 0x99, 0x33, 0x00);

/* Launches hit from the contact point (name inferred). The level interpolation keeps this function
 * at about 99.7%: retail shares one stack slot between the float-to-int and int-to-float
 * conversions, which GC/2.7 does not (see docs/research-notes.md; Spring's HitCharacter has the
 * same blocker). Everything else in the unit matches. */
static void HitCharacter(boost::shared_ptr<CharInfo> hit, void *user) {
    SpringPanelHitContext *context = static_cast<SpringPanelHitContext *>(user);
    Mtx34f matrix;
    boost::shared_ptr<GadgetCharInfo> self = context->m_chara.lock();

    if (self.get() == 0) {
        return;
    }

    hit->PushCommand(new Player::IgnoreInputCommand(1, context->m_params->noControlTime));
    hit->PushCommand(new ClrVelocityCommand(4));
    hit->PushCommand(new SetPositionCommand(*context->m_point, 4));

    Vec3f angle(hit->m_rotation);
    angle.y = self->m_rotation.y;
    hit->PushCommand(new SetAngleCommand(angle, 2, 4));
    Vec3f velocity;

    int level = context->m_params->level;
    float speed = lbl_804C2238[level];
    if (level < 15) {
        speed += (lbl_804C2238[level + 1] - speed) * (context->m_params->level - level);
    }

    fn_8040C148(&matrix, &lbl_805E0720);
    fn_8040B884(&matrix, self->m_rotation.x);
    fn_8040B80C(&matrix, self->m_rotation.z);
    velocity.x = 0.0f;
    velocity.y = 1.0f;
    velocity.z = 0.0f;
    AssignVector(velocity, fn_8040B67C(&matrix, &velocity));
    fn_8040CE38(&velocity, speed);
    hit->PushCommand(new SetVelocityCommand(velocity, 4));
    hit->PushCommand(new Player::LaunchCommand(0, 4));

    fn_801D54F4(fn_80053164(), 0x7238, context->m_point, 0, 1);
    fn_8034DF48(5, -2);
    context->m_launched = true;
}

/* Retail addresses the vtable and the two bound member pointers from the start of the unit's .data,
 * which begins with the orphan TNumEdit and CharaColliBase RTTI base lists of Game/TNumEdit.h. */
SpringPanel::SpringPanel(Task *parent, UnkPlacement_800C9CD0 *owner) : Gadget(parent, owner) {
    m_name = s_taskName;
    fn_800C9DBC(owner, &m_params, sizeof(m_params));
    m_chara = fn_8015CB88(5, 4, "SpringPanel");
    m_chara->m_commandUnit = boost::shared_ptr<CharCommandUnit>(new CharCommandUnit);
    fn_800729B0(&m_chara->m_commandUnit->m_callback,
                &(const CharCommandCallback &)CharCommandCallback(
                    boost::bind(&SpringPanel::OnCommand, this, _1)));
    m_model = new GadgetModelInfo(1, "SPRINGPANEL.BIN");
    m_model->fn_80162A68(0, 0);
    fn_8016239C(m_model, "mat");
    m_model->fn_80161F08("col", m_chara);
    fn_800D0B64(m_model->field_0x4, boost::bind(&SpringPanel::OnColliDetect, this, _1));
    fn_8040CE60(&m_point);
    m_target = fn_801E72DC(0x400);
    LoadFromOwner(m_owner);
    UpdatePoint();
}

/* Not called anywhere: the linker strips it. Retail's .data, which the linker keeps whole, holds a
 * std::runtime_error base list and vtable between those of the weak classes, as in Spring.cpp; an
 * unused function that constructs a runtime_error, defined after the constructor, reproduces that
 * layout (inferred; the original function is unknown, and the name is a placeholder). */
static void UnkRuntimeErrorUser() {
    boost::throw_exception(std::runtime_error(""));
}

SpringPanel::~SpringPanel() {
    fn_801E7108(&m_target);
    if (m_model != 0) {
        delete m_model;
        m_model = 0;
    }
}

void SpringPanel::Behave(float dt) {
    if (m_model != 0) {
        m_model->fn_80162FB0(dt);
    }
    UpdatePoint();
    UpdateColor();
}

void SpringPanel::ReloadPlacement(UnkPlacement_800C9CD0 *owner) {
    LoadFromOwner(owner);
    UpdatePoint();
    if (m_model != 0) {
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

void SpringPanel::fn_800CBC1C() {
    UpdateColor();
}

void SpringPanel::UpdateColor() {
    UnkMaterialAnim_801621F0 *material = fn_801621F0(m_model, "mat");
    float time = lbl_8057D6A0.m_time;
    float t = time - (int)time;

    if (t < 0.5f) {
        fn_80154C6C(material, &lbl_805EF1A4, &lbl_805EF1A8, t, 0.0f, 0.5f);
    } else if (t < 0.8333333f) {
        fn_80154C6C(material, &lbl_805EF1A8, &lbl_805EF1AC, t, 0.5f, 0.8333333f);
    } else if (t < 1.0f) {
        fn_80154C6C(material, &lbl_805EF1AC, &lbl_805EF1A4, t, 0.8333333f, 1.0f);
    }
}

void SpringPanel::LoadFromOwner(UnkPlacement_800C9CD0 *owner) {
    fn_800C9F14(owner, &m_chara->m_position);
    fn_800C9ED4(owner, &m_chara->m_rotation);
    fn_800C9DBC(owner, &m_params, sizeof(m_params));
}

void SpringPanel::UpdatePoint() {
    if (m_model != 0) {
        Mtx34f matrix;

        fn_8040C148(&matrix, &lbl_805E0720);
        fn_8040B59C(&matrix, &m_chara->m_position);
        fn_8040B884(&matrix, m_chara->m_rotation.x);
        fn_8040B80C(&matrix, m_chara->m_rotation.z);
        m_model->fn_801633DC(&matrix);

        Vec3f offset(0.0f, 20.0f, 0.0f);
        AssignVector(m_point, fn_8040B67C(&matrix, &offset));
        if (m_target.get() != 0) {
            m_target->fn_800CCE38(&m_point);
        }
    }
}

void SpringPanel::OnColliDetect(ColliDetectArgs &args) {
    SpringPanelHitContext context;

    context.m_chara = m_chara;
    context.m_params = &m_params;
    context.m_point = &m_point;
    fn_8015AA84(args, HitCharacter, &context);
    if (context.m_launched && m_model != 0) {
        fn_80161308(m_model, 1.0f);
    }
}

void SpringPanel::OnCommand(CharCommand &command) {
    /* The command's id is the halfword at 0x4, which CharCommand's constructor sets. */
    switch (*reinterpret_cast<u16 *>(command.field_0x4)) {
    case 0x1A:
        if (m_target->IsEnabled()) {
            m_target->TurnOff();
        }
        break;
    case 0x1B:
        if (!m_target->IsEnabled()) {
            m_target->TurnOn();
        }
        break;
    }
}

extern "C" void fn_80159018(void) {
    fn_8006B474("SPRINGPANEL.BIN", 1);
}

extern "C" void fn_80159014(void) {}

void SpringPanel::Instantiate(UnkPlacement_800C9CD0 *owner) {
    new SpringPanel(fn_801EB304(fn_8001C4F8(), 11), owner);
}

static GadgetParamDesc lbl_8052AD70[] = {
    {4, "level", 0, 0, 0, 0.0f, 0.0f, 15.0f, fn_800CBC20, 0},
    {4, "nocontrol time (sec)", 0, 0, 0, 0.0f, 0.0f, 3600.0f, fn_800CBC20, 0},
    {0},
};

GadgetClassDesc lbl_8052ADE8 = {
    "ELEC_SPRING", fn_80159018, fn_80159014, SpringPanel::Instantiate, 0, 0, 0x7DE, 1, 0, 10,
    lbl_8052AD70,
};
