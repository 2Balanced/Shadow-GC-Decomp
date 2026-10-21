/* Catapult::Catapult (class name from RTTI): the CATAPULT gadget, a launcher shown with the model
 * picked by its "model" parameter from CATAPULT.BIN. It receives commands through its character
 * info's command unit (OnCommand, bound with boost::bind): a ride command arms it, and a use
 * command takes the character on board. Once the motion flags the throw, the character is launched
 * at the speed for the "level" parameter, along the catapult's up axis tilted by the "X" and "Z"
 * angles, and "nocontrol time (sec)" sets how long its input is ignored. File name inferred from the
 * class. Method names marked "(name inferred)" are not original. Definitions are in reverse address
 * order (deferred inlining emits functions last-defined first). */
#include <boost/bind.hpp>
#include <boost/function.hpp>

#include "Game/CharCommand.h"
#include "Game/CharCommandUnit.h"
#include "Game/CharInfo.h"
#include "Game/Gadget.h"
#include "Game/GadgetCharInfo.h"
#include "Game/GadgetCollision.h"
#include "Game/GadgetModelInfo.h"
#include "Game/GadgetClassDesc.h"
#include "Game/GadgetUtil.h"
#include "Game/FileQueue.h"
#include "boost/weak_ptr.hpp"
#include "size_assert.h"

extern "C" {
/* A 32-bit flag word: fn_80014E04 clears it, fn_80014E1C sets bit, fn_8006127C clears bit and
 * fn_80014E34 tests bit (behaviour inferred from their bodies). */
void fn_80014E04(u32 *flags);
void fn_80014E1C(u32 *flags, int bit);
bool fn_80014E34(u32 *flags, int bit);
void fn_8006127C(u32 *flags, int bit);

/* Loads the model file fileName (behaviour inferred; arg0 unknown). */
boost::shared_ptr<GadgetModelInfo> fn_8015F800(int arg0, const char *fileName);
/* Returns the number of models the model file holds (behaviour inferred). */
int fn_8015FF44(GadgetModelInfo *model);
/* GadgetModelInfo methods that are not declared in its header yet (behaviour inferred):
 * fn_80160A84 plays motion number motion of model variant index, fn_801612B8 stops the motion,
 * fn_80161308 sets the motion speed and fn_80161BD8 attaches a character info to the collision
 * of a model variant. */
void fn_80160A84(GadgetModelInfo *model, int index, int motion);
void fn_801612B8(GadgetModelInfo *model);
void fn_80161308(GadgetModelInfo *model, float speed);
bool fn_80161BD8(GadgetModelInfo *model, int index, const boost::weak_ptr<CharInfo> &info,
                 int arg3);
/* Copies params back into the owner's parameter block when its size equals size (behaviour
 * inferred; the counterpart of fn_800C9DBC). */
void fn_800C9D68(UnkPlacement_800C9CD0 *owner, const void *params, int size);
/* Plays sound id at charInfo (behaviour inferred). */
void fn_801D54F4(void *sound, int id, GadgetCharInfo *charInfo, int arg3, int arg4);
}

/* Launch speed for each level. */
static const float lbl_804C0680[16] = {
    600.0f,  900.0f,  1200.0f, 1500.0f, 1800.0f, 2100.0f, 2400.0f, 2700.0f,
    3000.0f, 3300.0f, 3600.0f, 3900.0f, 4200.0f, 4500.0f, 4800.0f, 5100.0f,
};

namespace Catapult {

/* The parameters the owner supplies (names from the parameter table). */
struct CatapultParams {
    CatapultParams() : model(0), level(0.0f), noControlTime(0.0f), x(0.0f), y(0.0f), z(0.0f) {}

    /* 0x00 */ s32 model;
    /* 0x04 */ float level;
    /* 0x08 */ float noControlTime;
    /* 0x0C */ float x; /* degrees */
    /* 0x10 */ float y;
    /* 0x14 */ float z;
};
SIZE_ASSERT(CatapultParams, 0x18);

class Catapult : public Gadget {
  public:
    Catapult(Task *parent, UnkPlacement_800C9CD0 *owner);
    virtual ~Catapult();
    virtual void Behave(float dt);
    virtual void ReloadPlacement(UnkPlacement_800C9CD0 *owner);

    /* Creates the catapult under task layer 11 (name inferred). */
    static void Instantiate(UnkPlacement_800C9CD0 *owner);

    /* The command unit's callback (name inferred). */
    void OnCommand(CharCommand &command);
    /* Places the model at the character info's translation, turned about the Y axis (name
     * inferred). */
    void RebuildTransform();
    /* Copies the owner's position, rotation and parameters, and rearms the catapult (name
     * inferred). */
    void LoadFromOwner(UnkPlacement_800C9CD0 *owner);

    static const char *s_taskName;

    /* 0x30 */ boost::shared_ptr<GadgetCharInfo> m_chara;
    /* 0x38 */ boost::shared_ptr<GadgetModelInfo> m_model;
    /* 0x40 */ boost::weak_ptr<CharInfo> m_rider; /* the character on board (name inferred) */
    /* 0x48 */ s32 m_modelIndex;                  /* the model shown */
    /* 0x4C */ CatapultParams m_params;
    /* 0x64 */ s32 m_state; /* 0 rearming, 1 ready, 2 loaded, 3 cooling down (inferred) */
    /* 0x68 */ float m_timer;
    /* 0x6C */ u32 m_flags; /* bit 0: throw requested, bit 1: armed (inferred) */
};

/* What Behave passes to the launch function (name inferred). */
struct CatapultHitContext {
    CatapultHitContext() {}

    /* 0x0 */ boost::weak_ptr<GadgetCharInfo> m_chara;
    /* 0x8 */ CatapultParams *m_params;
};
SIZE_ASSERT(CatapultHitContext, 0xC);

} // namespace Catapult

SIZE_ASSERT(Catapult::Catapult, 0x70);

namespace Catapult {

const char *Catapult::s_taskName = "Catapult";

/* Launches hit from the catapult (name inferred). The level interpolation converts float to int
 * and back; retail shares one stack slot for both conversions, which GC/2.7 does not (see
 * docs/research-notes.md), so this function stays at 99.7% and the unit NonMatching. */
static void HitCharacter(boost::shared_ptr<CharInfo> hit, CatapultHitContext *context) {
    boost::shared_ptr<GadgetCharInfo> self = context->m_chara.lock();

    if (self.get() == 0) {
        return;
    }

    hit->PushCommand(new Player::IgnoreInputCommand(0, context->m_params->noControlTime));
    hit->PushCommand(new ClrVelocityCommand(4));

    int level = context->m_params->level;
    float speed = lbl_804C0680[level];
    if (level < 15) {
        speed += (lbl_804C0680[level + 1] - speed) * (context->m_params->level - level);
    }

    Mtx34f matrix;
    fn_8040C148(&matrix, &lbl_805E0720);
    fn_8040B848(&matrix, self->m_rotation.y);
    fn_8040B884(&matrix, self->m_rotation.x);
    fn_8040B80C(&matrix, self->m_rotation.z);
    fn_8040B884(&matrix, context->m_params->x * (lbl_805FAD38 / 180.0f));
    fn_8040B80C(&matrix, context->m_params->z * (lbl_805FAD38 / 180.0f));

    Vec3f velocity(0.0f, 1.0f, 0.0f);
    AssignVector(velocity, fn_8040B67C(&matrix, &velocity));
    fn_8040CE38(&velocity, speed);
    hit->PushCommand(new SetVelocityCommand(velocity, 4));

    fn_80169BB0(boost::weak_ptr<CharInfo>(hit), 5);
    hit->PushCommand(new Player::LaunchCommand(0, 4));
}

Catapult::Catapult(Task *parent, UnkPlacement_800C9CD0 *owner) : Gadget(parent, owner) {
    fn_80014E04(&m_flags);
    m_name = s_taskName;
    m_chara = fn_8015CD90(0x36, 4);
    LoadFromOwner(owner);
    m_chara->m_commandUnit = boost::shared_ptr<CharCommandUnit>(new CharCommandUnit());
    fn_800729B0(&m_chara->m_commandUnit->m_callback,
                &(const CharCommandCallback &)CharCommandCallback(
                    boost::bind(&Catapult::OnCommand, this, _1)));
    m_modelIndex = m_params.model;
    m_model = fn_8015F800(1, "CATAPULT.BIN");
    m_model->fn_80162A68(m_modelIndex, 0);
    m_model->fn_80160C28(m_modelIndex);
    m_model->fn_8015FF60();
    fn_80161BD8(m_model.get(), m_modelIndex, m_chara, m_modelIndex);
    RebuildTransform();
}

Catapult::~Catapult() {}

void Catapult::Behave(float dt) {
    if (m_model.get() != 0) {
        m_model->fn_80162FB0(dt);
    }

    switch (m_state) {
    case 0:
        m_state = 1;
        m_timer = 1.0f;
        break;
    case 2:
        if (fn_80014E34(&m_flags, 0)) {
            fn_80160A84(m_model.get(), m_modelIndex, 2);
            fn_80161308(m_model.get(), 1.0f);
            if (boost::shared_ptr<CharInfo> rider = m_rider.lock()) {
                CatapultHitContext context;

                context.m_chara = m_chara;
                context.m_params = &m_params;
                HitCharacter(rider, &context);
            }
            m_timer = 1.0f;
            m_state = 3;
            fn_8006127C(&m_flags, 0);
        }
        break;
    case 3:
        m_timer -= dt;
        if (m_timer <= 0.0f) {
            m_state = 0;
        }
        break;
    }

    RebuildTransform();
}

void Catapult::ReloadPlacement(UnkPlacement_800C9CD0 *owner) {
    LoadFromOwner(owner);
    RebuildTransform();
    if (m_model.get() != 0) {
        if (m_params.model != m_modelIndex) {
            int count = fn_8015FF44(m_model.get());

            m_modelIndex = m_params.model;
            if (m_modelIndex < 0) {
                m_modelIndex = 0;
            }
            if (m_modelIndex > count - 1) {
                m_modelIndex = count - 1;
            }
            m_model->fn_80161880();
            m_model->fn_801604FC();
            m_model->fn_80162804();
            m_model->fn_80162A68(m_modelIndex, 0);
            m_model->fn_80160C28(m_modelIndex);
            m_model->fn_8015FF60();
            fn_80161BD8(m_model.get(), m_modelIndex, m_chara, m_modelIndex);
            m_params.model = m_modelIndex;
            fn_800C9D68(owner, &m_params, sizeof(m_params));
        }
        m_model->fn_80162EF4();
        m_model->fn_8016108C();

        Mtx34f matrix;
        fn_8040C148(&matrix, &lbl_805E0720);
        fn_8040B59C(&matrix, &m_chara->m_position);
        fn_8040B848(&matrix, m_chara->m_rotation.y);
        fn_8040B884(&matrix, m_chara->m_rotation.x);
        fn_8040B80C(&matrix, m_chara->m_rotation.z);

        Vec3f start(0.0f, 0.0f, 0.0f);
        Vec3f end(0.0f, 1000000.0f, 0.0f);
        fn_8040B884(&matrix, m_params.x * (lbl_805FAD38 / 180.0f));
        fn_8040B80C(&matrix, m_params.z * (lbl_805FAD38 / 180.0f));
        AssignVector(start, fn_8040B67C(&matrix, &start));
        AssignVector(end, fn_8040B67C(&matrix, &end));
        u8 segment[0x18];
        fn_802E0B58(fn_80020A20(segment, &start, &end), &lbl_805F06D8);
    }
}

void Catapult::LoadFromOwner(UnkPlacement_800C9CD0 *owner) {
    fn_800C9F14(owner, &m_chara->m_position);
    fn_800C9ED4(owner, &m_chara->m_rotation);
    fn_800C9DBC(owner, &m_params, sizeof(m_params));
    m_state = 0;
}

void Catapult::RebuildTransform() {
    if (m_model.get() != 0) {
        Mtx34f matrix;

        fn_8040C148(&matrix, &lbl_805E0720);
        fn_8040B59C(&matrix, &m_chara->m_position);
        fn_8040B848(&matrix, m_chara->m_rotation.y);
        m_model->fn_801633DC(&matrix);
    }
}

void Catapult::OnCommand(CharCommand &command) {
    switch (command.TypeId()) {
    case 0xB: /* RideCommand */
        if (m_state == 1 || m_state == 3) {
            fn_80014E1C(&m_flags, 1);
            m_state = 2;
            fn_80160A84(m_model.get(), m_modelIndex, 1);
            m_chara->SetField50(0x37);
            fn_801D54F4(fn_80053164(), 0x7548, m_chara.get(), 0, 1);
        }
        break;
    case 0xC: /* LeaveCommand */
        if (m_state == 2 && fn_80014E34(&m_flags, 1)) {
            m_timer = 1.5f;
            m_state = 3;
            fn_801612B8(m_model.get());
            fn_80160A84(m_model.get(), m_modelIndex, 2);
            fn_8006127C(&m_flags, 1);
            m_chara->SetField50(0x36);
        }
        break;
    case 0xD: /* UseCommand */
        if (m_state == 2 && fn_80014E34(&m_flags, 1)) {
            m_rider = static_cast<UseCommand &>(command).m_user;
            fn_80014E1C(&m_flags, 0);
            fn_8006127C(&m_flags, 1);
            m_chara->SetField50(0x36);
            fn_801D560C(fn_80053164(), 0x7549, 0, 0);
        }
        break;
    }
}

} // namespace Catapult

extern "C" void fn_80141D94(void) {
    fn_8006B474("CATAPULT.BIN", 1);
}

extern "C" void fn_80141D90(void) {}

void Catapult::Catapult::Instantiate(UnkPlacement_800C9CD0 *owner) {
    Task *parent = fn_801EB304(fn_8001C4F8(), 11);
    new Catapult(parent, owner);
}

static GadgetParamDesc lbl_8052962C[] = {
    {1, "model", 0, 0, 1, 0.0f, 0.0f, 0.0f, fn_800CBC20, 0},
    {4, "level", 0, 0, 0, 0.0f, 0.0f, 15.0f, fn_800CBC20, 0},
    {4, "nocontrol time (sec)", 0, 0, 0, 0.0f, 0.0f, 3600.0f, fn_800CBC20, 0},
    {4, "X", 0, 0, 0, 0.0f, 0.0f, 360.0f, fn_800CBC20, 0},
    {4, "Y", 0, 0, 0, 0.0f, 0.0f, 360.0f, fn_800CBC20, 0},
    {4, "Z", 0, 0, 0, 0.0f, 0.0f, 360.0f, fn_800CBC20, 0},
    {0},
};

GadgetClassDesc lbl_80529744 = {
    "CATAPULT", fn_80141D94, fn_80141D90, Catapult::Catapult::Instantiate, 0, 0, 0x1138, 1, 0, 10,
    lbl_8052962C,
};
