/* CircusBall::CircusBall (class name from RTTI): the CIRCUS_BALL gadget, a ball that launches the
 * characters that touch it. A ball of "type" 0 is live from the start; a ball of type 1 (the "key"
 * kind) plays its idle motion until the circuit id copied from its owner is switched on, then
 * plays a sound and goes live. Once live, its collision callback (bound with boost::bind and kept
 * in a boost::function) launches each new character at the speed for the "level" parameter, tilted
 * by "angle", and ignores its input for "nocontrol time (sec)". A Target (class name from RTTI)
 * follows the ball. File name inferred from the class. Method names marked "(name inferred)" are
 * not original. Definitions are in reverse address order (deferred inlining emits functions
 * last-defined first), except the static HitCharacter, which the compiler places after them all.
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
#include "Game/UnkSwitchBank_800CB69C.h"
#include "Game/FileQueue.h"
#include "boost/weak_ptr.hpp"
#include "size_assert.h"

/* Target (class name from RTTI): a point other objects can aim at (meaning inferred). */
class Target {
  public:
    /* Moves the target to position (name inferred). */
    void fn_800CCE38(const Vec3f *position);
};

/* An entry of a model's sound table (name inferred): fn_80208888 finds it by name. */
struct UnkSoundEntry_80208888 {
    /* 0x00 */ const char *m_name;
    /* 0x04 */ u32 field_0x4;
    /* 0x08 */ u32 m_soundId; /* passed to the sound call as 16 bits */
    /* 0x0C */ u32 field_0xC;
    /* 0x10 */ Vec3f m_offset;
};
SIZE_ASSERT(UnkSoundEntry_80208888, 0x1C);

class UnkSoundTable_80208888;
class UnkResourceManager_800122D4;

extern "C" {
/* Creates a target with the given flags, and removes a gadget's target (behaviour inferred). */
boost::shared_ptr<Target> fn_801E72DC(int flags);
void fn_801E7108(boost::shared_ptr<Target> *target);
/* Loads model fileName for a gadget (behaviour inferred; arg0's meaning is unknown). */
boost::shared_ptr<GadgetModelInfo> fn_8015F800(int arg0, const char *fileName);
/* The resource manager, and the sound table that fileName carries (behaviour inferred). */
UnkResourceManager_800122D4 *fn_800122D4(void);
UnkSoundTable_80208888 *fn_80012700(UnkResourceManager_800122D4 *manager, int arg1,
                                    const char *fileName);
/* Finds the entry called name in the sound table (behaviour inferred). */
UnkSoundEntry_80208888 *fn_80208888(UnkSoundTable_80208888 *table, const char *name);
/* GadgetModelInfo methods not declared in its header (behaviour unknown; fn_801626D8 returns the
 * name of a sound table entry). */
void fn_80160A84(GadgetModelInfo *model, int arg1, int arg2);
void fn_80162790(GadgetModelInfo *model);
void fn_8016271C(GadgetModelInfo *model);
const char *fn_801626D8(GadgetModelInfo *model);
/* Copies the owner's byte at +0x22 into *value (behaviour inferred; the circuit id). */
void fn_800C9E50(UnkPlacement_800C9CD0 *owner, int *value);
/* Plays sound id at the character info's position through the sound manager (behaviour
 * inferred). */
void fn_801D54F4(void *manager, u16 id, GadgetCharInfo *charInfo, int arg3, int arg4);
}

/* Launch speed for each level. */
static const float lbl_804BE910[16] = {
    150.0f,  300.0f,  450.0f,  600.0f,  750.0f,  900.0f,  1050.0f, 1200.0f,
    1350.0f, 1500.0f, 1650.0f, 1800.0f, 1950.0f, 2100.0f, 2250.0f, 2400.0f,
};

namespace CircusBall {

/* The four parameters the owner supplies (names from the parameter table). */
struct CircusBallParams {
    CircusBallParams() : type(0), level(0.0f), noControlTime(0.0f), angle(0.0f) {}

    /* 0x0 */ int type; /* 0 normal, 1 key */
    /* 0x4 */ float level;
    /* 0x8 */ float noControlTime;
    /* 0xC */ float angle; /* degrees */
};
SIZE_ASSERT(CircusBallParams, 0x10);

class CircusBall : public Gadget {
  public:
    CircusBall(Task *parent, UnkPlacement_800C9CD0 *owner);
    virtual ~CircusBall();
    virtual void Behave(float dt);
    virtual void ReloadPlacement(UnkPlacement_800C9CD0 *owner);

    /* Creates the ball under task layer 11 (name inferred). */
    static void Instantiate(UnkPlacement_800C9CD0 *owner);

    /* The model's collision callback (name inferred). */
    void OnColliDetect(ColliDetectArgs &args);
    /* Places the model and moves the ball position and its target (name inferred). */
    void RebuildTransform();
    /* Copies the owner's position, rotation, parameters and circuit id (name inferred). */
    void LoadFromOwner(UnkPlacement_800C9CD0 *owner);
    /* Attaches the character info and the collision callback and creates the target (name
     * inferred). */
    void Activate();

    static const char *s_taskName;

    /* 0x30 */ boost::shared_ptr<GadgetCharInfo> m_chara;
    /* 0x38 */ boost::shared_ptr<GadgetModelInfo> m_model;
    /* 0x40 */ boost::shared_ptr<UnkHitList_801695B8> m_hitList;
    /* 0x48 */ boost::shared_ptr<Target> m_target;
    /* 0x50 */ UnkSoundTable_80208888 *m_soundTable;
    /* 0x54 */ CircusBallParams m_params;
    /* 0x64 */ Vec3f m_ballPosition; /* the model's origin (inferred) */
    /* 0x70 */ s32 m_state;                  /* 0 starting, 1 waiting for the circuit, 2 live */
    /* 0x74 */ int m_circuitId;              /* names inferred */
};

/* What the collision callback passes to the per-character hit function (name inferred). */
struct CircusBallHitContext {
    CircusBallHitContext() : m_params(0), m_ballPosition(0), m_launched(false) {}

    /* 0x00 */ boost::shared_ptr<UnkHitList_801695B8> m_hitList;
    /* 0x08 */ boost::weak_ptr<GadgetCharInfo> m_chara;
    /* 0x10 */ CircusBallParams *m_params;
    /* 0x14 */ Vec3f *m_ballPosition;
    /* 0x18 */ bool m_launched;
};
SIZE_ASSERT(CircusBallHitContext, 0x1C);

} // namespace CircusBall

SIZE_ASSERT(CircusBall::CircusBall, 0x78);

namespace CircusBall {

const char *CircusBall::s_taskName = "CircusBall";

/* Launches hit from the ball unless the ball already launched it (name inferred). Defined first:
 * functions are generated last-defined first, and the original pools this function's float
 * literals after all the others. */
static void HitCharacter(boost::shared_ptr<CharInfo> hit, void *user) {
    CircusBallHitContext *context = static_cast<CircusBallHitContext *>(user);
    boost::shared_ptr<GadgetCharInfo> self = context->m_chara.lock();

    if (self.get() == 0) {
        return;
    }
    if (context->m_hitList->fn_801697CC(boost::weak_ptr<CharInfo>(hit))) {
        return;
    }

    hit->PushCommand(new Player::IgnoreInputCommand(0, context->m_params->noControlTime));
    hit->PushCommand(new ClrVelocityCommand(4));
    hit->PushCommand(new SetPositionCommand(*context->m_ballPosition, 4));

    int level = context->m_params->level;
    float speed = lbl_804BE910[level];
    if (level < 15) {
        speed += (lbl_804BE910[level + 1] - speed) * (context->m_params->level - level);
    }

    Mtx34f matrix;
    fn_8040C148(&matrix, &lbl_805E0720);
    fn_8040B848(&matrix, self->m_rotation.y);
    fn_8040B884(&matrix, -context->m_params->angle * (lbl_805FAD38 / 180.0f));

    Vec3f velocity(0.0f, 1.0f, 0.0f);
    AssignVector(velocity, fn_8040B67C(&matrix, &velocity));
    fn_8040CE38(&velocity, speed);
    hit->PushCommand(new SetVelocityCommand(velocity, 4));
    hit->PushCommand(new Player::LaunchCommand(0, 4));

    fn_80169BB0(boost::weak_ptr<CharInfo>(hit), 5);
    fn_801D560C(fn_80053164(), 0x742E, 0, 0);
    context->m_launched = true;
}

CircusBall::CircusBall(Task *parent, UnkPlacement_800C9CD0 *owner) : Gadget(parent, owner) {
    m_name = s_taskName;
    m_chara = fn_8015CB88(0x41, 4, "CircusBall");
    LoadFromOwner(m_owner);
    m_soundTable = fn_80012700(fn_800122D4(), 1, "CIRCUSBALL.BIN");
    m_model = fn_8015F800(1, "CIRCUSBALL.BIN");
    m_model->fn_80162A68(0, 0);
    m_model->fn_80160C28(0);
    m_model->fn_8015FF60();
    fn_8040CE60(&m_ballPosition);
    RebuildTransform();
    m_hitList = fn_801695B8();
}

CircusBall::~CircusBall() {
    fn_8016952C(&m_hitList);
    fn_801E7108(&m_target);
}

void CircusBall::Activate() {
    m_model->fn_80161F08("ball", m_chara);
    fn_800D0B64(m_model->field_0x4, boost::bind(&CircusBall::OnColliDetect, this, _1));
    m_target = fn_801E72DC(0x400);
}

void CircusBall::Behave(float dt) {
    if (m_hitList.get() != 0) {
        m_hitList->fn_801697F0();
    }
    if (m_model.get() != 0) {
        m_model->fn_80162FB0(dt);
    }

    switch (m_state) {
    case 0:
        if (m_params.type == 0) {
            m_state = 2;
            Activate();
        } else {
            m_state = 1;
            fn_80162790(m_model.get());
        }
        break;
    case 1:
        if (fn_8016904C(fn_800CB69C(), m_circuitId) == 1) {
            fn_8016271C(m_model.get());
            fn_80160A84(m_model.get(), 0, 1);
            m_state = 2;
            Activate();
            UnkSoundEntry_80208888 *entry = fn_80208888(m_soundTable, fn_801626D8(m_model.get()));
            if (entry != 0) {
                fn_801D54F4(fn_80053164(), entry->m_soundId, m_chara.get(), 0, 1);
            }
        }
        break;
    case 2:
        /* Live. Listing the case keeps the original's extra branch. */
        break;
    }

    RebuildTransform();
}

void CircusBall::ReloadPlacement(UnkPlacement_800C9CD0 *owner) {
    LoadFromOwner(owner);
    RebuildTransform();
    if (m_model.get() != 0) {
        m_model->fn_80162EF4();
        m_model->fn_8016108C();

        Mtx34f matrix;
        u8 segment[0x18];
        Vec3f start(0.0f, 0.0f, 0.0f);
        Vec3f end(0.0f, 1000000.0f, 0.0f);

        fn_8040C148(&matrix, &lbl_805E0720);
        fn_8040B59C(&matrix, &m_chara->m_position);
        fn_8040B848(&matrix, m_chara->m_rotation.y);
        fn_8040B884(&matrix, -m_params.angle * (lbl_805FAD38 / 180.0f));
        AssignVector(start, fn_8040B67C(&matrix, &start));
        AssignVector(end, fn_8040B67C(&matrix, &end));
        fn_802E0B58(fn_80020A20(segment, &start, &end), &lbl_805F06D8);
    }
}

void CircusBall::LoadFromOwner(UnkPlacement_800C9CD0 *owner) {
    fn_800C9F14(owner, &m_chara->m_position);
    fn_800C9ED4(owner, &m_chara->m_rotation);
    fn_800C9DBC(owner, &m_params, sizeof(m_params));
    fn_800C9E50(owner, &m_circuitId);
    m_state = 0;
}

void CircusBall::RebuildTransform() {
    if (m_model.get() != 0) {
        Mtx34f matrix;

        fn_8040C148(&matrix, &lbl_805E0720);
        fn_8040B59C(&matrix, &m_chara->m_position);
        fn_8040B848(&matrix, m_chara->m_rotation.y);
        m_model->fn_801633DC(&matrix);

        Vec3f origin(0.0f, 0.0f, 0.0f);
        AssignVector(m_ballPosition, fn_8040B67C(&matrix, &origin));
        if (m_target.get() != 0) {
            m_target->fn_800CCE38(&m_ballPosition);
        }
    }
}

void CircusBall::OnColliDetect(ColliDetectArgs &args) {
    CircusBallHitContext context;

    context.m_hitList = m_hitList;
    context.m_chara = m_chara;
    context.m_params = &m_params;
    context.m_ballPosition = &m_ballPosition;
    fn_8015B004(args, HitCharacter, &context);
    if (context.m_launched && m_model.get() != 0) {
        fn_80160A84(m_model.get(), 0, 2);
    }
}

} // namespace CircusBall

extern "C" void fn_8012F6F8(void) {
    fn_8006B474("CIRCUSBALL.BIN", 1);
}

extern "C" void fn_8012F6F4(void) {}

void CircusBall::CircusBall::Instantiate(UnkPlacement_800C9CD0 *owner) {
    Task *parent = fn_801EB304(fn_8001C4F8(), 11);
    new CircusBall(parent, owner);
}

/* The fields of the cursor that the label callback reads (layout from its accesses). */
struct CircusBallParamCursor {
    /* 0x0 */ u32 field_0x0;
    /* 0x4 */ int index; /* the parameter */
    /* 0x8 */ int value;
};

/* The labels of the type values. */
static const char *lbl_805E5ECC[] = {
    "type : normal",
    "type : key",
};

/* The label callback of the type parameter. */
extern "C" const char *fn_8012F674(UnkLabelQuery_800CBC20 *cursor) {
    const CircusBallParamCursor *fields = (const CircusBallParamCursor *)cursor;

    if (fields->index == 0) {
        return lbl_805E5ECC[fields->value];
    }
    return 0;
}

static GadgetParamDesc lbl_8052867C[] = {
    {1, "type", 0, 0, 1, 0.0f, 0.0f, 0.0f, fn_8012F674, 0},
    {4, "level", 0, 0, 0, 0.0f, 0.0f, 15.0f, fn_800CBC20, 0},
    {4, "nocontrol time (sec)", 0, 0, 0, 0.0f, 0.0f, 3600.0f, fn_800CBC20, 0},
    {4, "angle", 0, 0, 0, 0.0f, 0.0f, 360.0f, fn_800CBC20, 0},
    {0},
};

GadgetClassDesc lbl_80528744 = {
    "CIRCUS_BALL", fn_8012F6F8, fn_8012F6F4, CircusBall::CircusBall::Instantiate, 0, 0, 0xC80, 1, 0, 10,
    lbl_8052867C,
};
