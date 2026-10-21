/* TurnOver::TurnOverTask (class name from RTTI): the TANK and TURNOVER gadgets, which share one
 * class and differ only in their model file (TANK.BIN or TURNOVER.BIN) and a kind passed to the
 * model loaders. The "model" parameter picks the model variant. A character uses the gadget through
 * character commands (a ReadyCommand, then a UseCommand or a CancelCommand), which play the
 * variant's motions; while the use motion runs, its motion events play sounds and start particle
 * emitters, and when it ends the gadget lifts up, waits and finishes. Its attack collision tells the
 * attack whether a player owns it, and a LineTarget between two model frames marks it for aiming.
 * File name inferred from the namespace. Method names marked "(name inferred)" are not original.
 * Definitions are in reverse address order (deferred inlining emits functions last-defined
 * first). */
#include "Game/CharCommand.h"
#include "Game/CharCommandUnit.h"
#include "Game/CharInfo.h"
#include "Game/CharaColli.h"
#include "Game/Gadget.h"
#include "Game/GadgetCharInfo.h"
#include "Game/GadgetModelInfo.h"
#include "Game/GadgetParticle.h"
#include "Game/GadgetClassDesc.h"
#include "Game/GadgetUtil.h"
#include "Game/LineTarget.h"
#include "Game/Vibration.h"
#include "Game/FileQueue.h"
#include "boost/bind.hpp"
#include "boost/function.hpp"
#include "boost/shared_ptr.hpp"
#include "boost/weak_ptr.hpp"
#include <cstdio>
#include "size_assert.h"

namespace SonicteamUSA {
namespace System {
namespace CharaColli {

/* CharaColliAttack (class name from the RTTI of its shared_ptr control block): an attack
 * collision. Only the owner is mapped (name inferred). */
class CharaColliAttack {
  public:
    /* 0x00 */ u8 field_0x0[0x8];
    /* 0x08 */ boost::weak_ptr<CharInfo> m_owner;
};

/* AttackCallbackParam (struct name from the RTTI of the gadget's callback binders): what an attack
 * collision passes to its callbacks. Only the members used here are mapped (names and meanings
 * inferred). */
struct AttackCallbackParam {
    /* 0x00 */ boost::weak_ptr<CharaColliAttack> m_attacker;
    /* 0x08 */ boost::weak_ptr<CharaColliAttack> m_target; /* the attack collision that was hit */
    /* 0x10 */ u8 field_0x10[0x30 - 0x10];
    /* 0x30 */ u8 m_targetIsPlayer; /* set when the hit collision's owner is of kind 1 */
};

} // namespace CharaColli
} // namespace System
} // namespace SonicteamUSA

using SonicteamUSA::System::CharaColli::AttackCallbackParam;
using SonicteamUSA::System::CharaColli::CharaColliAttack;
using SonicteamUSA::System::CharaColli::CharaColliBase;

/* The motion work of a model (struct name from the RTTI of its shared_ptr control block). */
struct _stMtnPlayWork;

/* The RenderWare frame of a model part, and the part of its world matrix used here (layout from
 * rw/rwcore.h, which the game C++ units do not include). */
struct RwFrame;

struct RwV3d {
    float x;
    float y;
    float z;
};

struct RwMatrix {
    /* 0x00 */ RwV3d right;
    /* 0x0C */ u32 flags;
    /* 0x10 */ RwV3d up;
    /* 0x1C */ u32 pad1;
    /* 0x20 */ RwV3d at;
    /* 0x2C */ u32 pad2;
    /* 0x30 */ RwV3d pos;
    /* 0x3C */ u32 pad3;
};
SIZE_ASSERT(RwMatrix, 0x40);

/* The object at 0x24 of a handle; only the virtual functions at vtable offsets 0xC and 0x10 are
 * used here (they look like an enable and a disable taking a mask; meanings inferred). */
class UnkHandleBShape_802589F8 {
  public:
    virtual void vf08();
    virtual void vf0C(int mask);
    virtual void vf10(int mask);
};

/* The handle fn_8015B8C4 wraps a collider in (the class name and the layout beyond the member
 * used here are unknown). */
class GadgetCharCollision {
  public:
    /* 0x00 */ u8 field_0x0[0x24];
    /* 0x24 */ UnkHandleBShape_802589F8 *m_shape;
};

/* ScoreCommand (class name from RTTI): command type 0x1C. Its five values are passed through
 * unchanged (meanings unknown). */
class ScoreCommand : public CharCommand {
  public:
    ScoreCommand(s32 arg0, s32 arg1, s32 arg2, s32 arg3, s32 arg4);

    /* 0x14 */ s32 m_values[5];
};
SIZE_ASSERT(ScoreCommand, 0x28);

/* What fn_8015FA78 passes to fn_80169E8C: the id of the model frame to find, and the frame found
 * (names inferred). */
struct UnkFrameSearch_80169E8C {
    UnkFrameSearch_80169E8C() : id(0), frame(0) {}

    /* 0x0 */ u32 id;
    /* 0x4 */ RwFrame *frame;
};
SIZE_ASSERT(UnkFrameSearch_80169E8C, 0x8);

/* The data of the motion events used here (8 bytes, as the stack slots show): the sound or emitter
 * id, then a word of unknown use (names inferred). */
struct UnkMotionEvent_80415A18 {
    /* 0x0 */ s32 id;
    /* 0x4 */ u32 field_0x4;
};
SIZE_ASSERT(UnkMotionEvent_80415A18, 0x8);

extern "C" {
RwMatrix *RwFrameGetLTM(RwFrame *frame);

/* The collider manager (behaviour inferred). */
void *fn_80021B08(void);
/* Creates a collider of the given model part for charInfo (behaviour inferred; the meanings of
 * the integer arguments are unknown). */
boost::shared_ptr<CharaColliBase> fn_800C7F7C(void *manager, const boost::weak_ptr<CharInfo> &charInfo,
                                              void *part, _stMtnPlayWork *work, int arg4, int arg5);
/* Returns the attack collision of a collider (behaviour inferred). */
boost::weak_ptr<CharaColliAttack> fn_800C4374(void *manager, boost::shared_ptr<CharaColliBase> collider);
/* Sets the attack collision's two callbacks (behaviour inferred; the meanings of the other
 * arguments are unknown). */
void fn_80054DD0(CharaColliAttack *attack, int arg1, int arg2, float arg3,
                 const boost::function1<void, AttackCallbackParam &> &onHit,
                 const boost::function1<void, AttackCallbackParam &> &onEnd);
/* Wraps collider in a new handle (behaviour inferred). */
boost::shared_ptr<GadgetCharCollision> fn_8015B8C4(boost::shared_ptr<CharaColliBase> collider);
/* Draws the handle's shape for debugging, and advances the handle by dt (behaviour inferred). */
void fn_8015BE64(GadgetCharCollision *handle);
void fn_8015BE9C(GadgetCharCollision *handle, float dt);

/* Creates the command receiver of a character info unless it has one (behaviour inferred). */
void fn_8015CF7C(GadgetCharInfo *charInfo);

/* A 32-bit flag word: fn_80014E04 clears it, fn_80014E1C sets bit, fn_8006127C clears bit and
 * fn_80014E34 tests bit (behaviour inferred from their bodies). */
void fn_80014E04(u32 *flags);
void fn_80014E1C(u32 *flags, int bit);
bool fn_80014E34(u32 *flags, int bit);
void fn_8006127C(u32 *flags, int bit);

/* Returns the model manager, and the sound table of a model file in it (behaviour inferred). */
void *fn_800122D4(void);
void *fn_80012700(void *manager, int kind, const char *fileName);
/* Looks up the entry named name in a sound table (behaviour inferred). */
struct UnkSoundEntry_80208888 {
    /* 0x0 */ u8 field_0x0[0x8];
    /* 0x8 */ u32 id;
};
UnkSoundEntry_80208888 *fn_80208888(void *table, const char *name);
/* Plays sound id at charInfo (behaviour inferred). */
void fn_801D54F4(void *sound, int id, GadgetCharInfo *charInfo, int arg3, int arg4);

/* Loads the model fileName (behaviour inferred). */
boost::shared_ptr<GadgetModelInfo> fn_8015F800(int kind, const char *fileName);
/* GadgetModelInfo helpers (behaviour inferred): the name of the model, whether variant can be
 * shown, the part and the motion work of a variant, attaching a character info to a variant,
 * playing motion of a variant, whether a variant's motion is over, and a fade. */
const char *fn_801626D8(GadgetModelInfo *model);
bool fn_80162EA8(GadgetModelInfo *model, int variant);
void *fn_801616CC(GadgetModelInfo *model, int variant);
boost::weak_ptr<_stMtnPlayWork> fn_801602B4(GadgetModelInfo *model, int variant);
void fn_80161BD8(GadgetModelInfo *model, int variant, const boost::weak_ptr<CharInfo> &info, int arg3);
void fn_80160A84(GadgetModelInfo *model, int variant, int motion);
bool fn_80160040(GadgetModelInfo *model, int variant);
void fn_8016268C(GadgetModelInfo *model, float arg1);
/* Calls callback with each part of the model (behaviour inferred). */
void fn_8015FA78(GadgetModelInfo *model, void *(*callback)(void *part, void *data), void *data);
/* fn_8015FA78 callbacks: attach a particle controller to the part, and find a frame by id. */
void *fn_80169D00(void *part, void *data);
void *fn_80169E8C(void *part, void *data);

/* Motion library: the flag test, and the lookup of a motion event, which copies the event's data
 * to data and returns whether the event fired (behaviour inferred). */
int fn_80416F28(_stMtnPlayWork *work, int arg1, int arg2, int arg3);
int fn_80415A18(_stMtnPlayWork *work, int arg1, int event, void *data, int arg4);

/* Particle controller helpers (behaviour inferred). */
boost::shared_ptr<GadgetParticle::GadgetParticleCtrl> fn_80166AF4(void);
void fn_80167448(GadgetParticle::GadgetParticleCtrl *ctrl, int id);
void fn_80167010(GadgetParticle::GadgetParticleCtrl *ctrl, int id);
void fn_8016761C(GadgetParticle::GadgetParticleCtrl *ctrl, float dt);

/* Controller vibrations near charInfo (behaviour inferred). */
boost::weak_ptr<Vibration::RequestHandle> fn_80169B80(int type, GadgetCharInfo *charInfo);
boost::weak_ptr<Vibration::RequestHandle> fn_80169AEC(int type, float strength, GadgetCharInfo *charInfo);

/* Sets matrix to the identity (behaviour inferred). */
void fn_8040B6F8(Mtx34f *matrix);
/* Queues a debug marker of the given size at position (behaviour inferred). */
void fn_8015F5D8(const Vec3f *position, float size);
/* Tells the owner the gadget is done (behaviour inferred; a Gadget method). */
void fn_8015A794(Gadget *gadget);
}

/* Returns x unchanged (name inferred). Being initialised through a function call, the radius
 * below is set up at startup. */
inline float Plain(float x) {
    return x;
}

/* The radius of the gadget's line target (name inferred). */
static float s_targetRadius = Plain(20.0f);

namespace TurnOver {

/* The two parameters the owner supplies (names from the parameter table). */
struct TurnOverParams {
    TurnOverParams() : model(0), blast(0) {}

    /* 0x0 */ s32 model;
    /* 0x4 */ s32 blast;
};
SIZE_ASSERT(TurnOverParams, 0x8);

class TurnOverTask : public Gadget {
  public:
    TurnOverTask(Task *parent, UnkPlacement_800C9CD0 *owner, int kind, const char *fileName);
    virtual ~TurnOverTask();
    virtual void Behave(float dt);
    virtual void ReloadPlacement(UnkPlacement_800C9CD0 *owner);

    /* Instantiate a TANK and a TURNOVER gadget under task layer 11 (names inferred). */
    static void CreateTank(UnkPlacement_800C9CD0 *owner);
    static void CreateTurnOver(UnkPlacement_800C9CD0 *owner);

    /* Plays the variant's "liftup" (which 0) or "cancel" (which 1) sound (name inferred). */
    void PlaySound(int which);
    /* Drops the attack collision's handle (name inferred). */
    void ReleaseHandle();
    /* Creates the attack collision of the model variant and its handle (name inferred). */
    void SetupAttack();
    /* The attack collision's callbacks (names inferred). */
    void OnAttackEnd(AttackCallbackParam &param);
    void OnAttackHit(AttackCallbackParam &param);
    /* The character info's command callback (name inferred). */
    void OnCommand(CharCommand &command);
    /* Moves the line target in front of the gadget (name inferred). */
    void UpdateTarget();
    /* Places the model at the character info's translation and yaw, and the line target's segment
     * between the two frames (name inferred). */
    void RebuildTransform();
    /* Copies the owner's position, rotation and parameters, and resets the state (name
     * inferred). */
    void LoadFromOwner(UnkPlacement_800C9CD0 *owner);

    /* 0x30 */ boost::shared_ptr<GadgetCharInfo> m_chara;
    /* 0x38 */ boost::shared_ptr<GadgetModelInfo> m_model;
    /* 0x40 */ boost::shared_ptr<LineTarget> m_target;
    /* 0x48 */ boost::shared_ptr<GadgetParticle::GadgetParticleCtrl> m_particle;
    /* 0x50 */ boost::shared_ptr<GadgetCharCollision> m_handle;
    /* 0x58 */ boost::weak_ptr<_stMtnPlayWork> m_motion;
    /* 0x60 */ boost::weak_ptr<CharInfo> m_user;
    /* 0x68 */ s32 m_state; /* 0 start, 1 idle, 2 ready, 3 cancel, 4 use, 5 lift, 6 done (inferred) */
    /* 0x6C */ RwFrame *m_frame0; /* the model frames with ids 1000 and 1001 */
    /* 0x70 */ RwFrame *m_frame1;
    /* 0x74 */ float m_timer;
    /* 0x78 */ s32 m_variant; /* the model variant shown (name inferred) */
    /* 0x7C */ TurnOverParams m_params;
    /* 0x84 */ void *m_sounds;
    /* 0x88 */ u32 m_soundFlags; /* bit 1: the liftup sound played (name inferred) */
    /* 0x8C */ u32 pad_0x8C; /* the class size is a multiple of 8 */
};

} // namespace TurnOver

SIZE_ASSERT(TurnOver::TurnOverTask, 0x90);

namespace TurnOver {

TurnOverTask::TurnOverTask(Task *parent, UnkPlacement_800C9CD0 *owner, int kind, const char *fileName)
    : Gadget(parent, owner) {
    fn_80014E04(&m_soundFlags);
    m_chara = fn_8015CB88(0x25, 4, "TurnOver");
    fn_8015CF7C(m_chara.get());
    fn_800729B0(&m_chara->m_commandUnit.px->m_callback,
                &(const CharCommandCallback &)CharCommandCallback(
                    boost::bind(&TurnOverTask::OnCommand, this, _1)));
    LoadFromOwner(owner);
    m_sounds = fn_80012700(fn_800122D4(), kind, fileName);
    m_variant = m_params.model;
    m_model = fn_8015F800(kind, fileName);
    m_model->fn_80162A68(m_variant, 0);
    m_model->fn_80160C28(m_variant);
    m_model->fn_8015FF60();
    fn_80161BD8(m_model.get(), m_variant, m_chara, m_variant);
    SetupAttack();
    if (m_handle.get() != 0) {
        m_handle->m_shape->vf10(-1);
    }
    m_target = fn_801E29D8(m_chara, 0x40);

    UnkFrameSearch_80169E8C search;
    search.id = 1000;
    search.frame = 0;
    fn_8015FA78(m_model.get(), fn_80169E8C, &search);
    m_frame0 = search.frame;
    search.id = 1001;
    search.frame = 0;
    fn_8015FA78(m_model.get(), fn_80169E8C, &search);
    m_frame1 = search.frame;

    UpdateTarget();
    RebuildTransform();
}

TurnOverTask::~TurnOverTask() {
    ReleaseHandle();
}

void TurnOverTask::Behave(float dt) {
    m_model->fn_80162FB0(dt);
    if (m_particle.get() != 0) {
        fn_8016761C(m_particle.get(), dt);
    }
    if (m_handle.get() != 0) {
        fn_8015BE9C(m_handle.get(), dt);
    }

    switch (m_state) {
    case 0:
        if (m_model.get() != 0) {
            m_particle = fn_80166AF4();

            UnkParticleAttach_80169D00 attach;
            attach.ctrl = m_particle;
            fn_8015FA78(m_model.get(), fn_80169D00, &attach);
        }
        if (m_particle.get() != 0) {
            fn_80167010(m_particle.get(), 4);
        }
        m_state = 1;
        break;
    case 2:
        if (!fn_80014E34(&m_soundFlags, 1)) {
            m_timer -= dt;
            if (m_timer <= 0.0f) {
                PlaySound(0);
                fn_80014E1C(&m_soundFlags, 1);
            }
        }
        break;
    case 3:
        if (fn_80160040(m_model.get(), m_variant)) {
            m_state = 1;
            if (m_particle.get() != 0) {
                fn_80167448(m_particle.get(), 1);
            }
            fn_80169B80(5, m_chara.get());
            PlaySound(1);
        }
        break;
    case 4:
        if (m_motion.expired()) {
            m_state = 5;
            if (m_handle.get() != 0) {
                m_handle->m_shape->vf10(-1);
            }
            fn_80169AEC(8, 2.0f, m_chara.get());
        } else {
            boost::shared_ptr<_stMtnPlayWork> work = m_motion.lock();
            if (fn_80416F28(work.get(), 0, 3, 1) != 0) {
                m_state = 5;
                if (m_handle.get() != 0) {
                    m_handle->m_shape->vf10(-1);
                }
            }

            UnkMotionEvent_80415A18 sound;
            if (fn_80415A18(work.get(), 0, 0x5001, &sound, 0) != 0) {
                fn_801D54F4(fn_80053164(), (u16)sound.id, m_chara.get(), 0, 1);
            }
            UnkMotionEvent_80415A18 emitter;
            if (fn_80415A18(work.get(), 0, 0x5000, &emitter, 0) != 0) {
                if (m_particle.get() != 0) {
                    fn_80167448(m_particle.get(), emitter.id);
                }
            }
        }
        break;
    case 5:
        m_timer -= dt;
        if (m_timer <= 0.0f) {
            m_timer = 1.5f;
            fn_8016268C(m_model.get(), -1.0f);
            m_state = 6;
        }
        break;
    case 6:
        m_timer -= dt;
        if (m_timer <= 0.0f) {
            fn_8015A794(this);
            m_flags |= 1;
        }
        break;
    }

    RebuildTransform();
}

void TurnOverTask::ReloadPlacement(UnkPlacement_800C9CD0 *owner) {
    s32 model = m_params.model;

    LoadFromOwner(owner);
    if (model != m_params.model) {
        m_particle.reset();
        ReleaseHandle();
        m_model->fn_80161880();
        m_model->fn_801604FC();
        m_model->fn_80162804();
        if (!fn_80162EA8(m_model.get(), m_params.model)) {
            return;
        }
        m_variant = m_params.model;
        m_model->fn_80162A68(m_variant, 0);
        m_model->fn_80160C28(m_variant);
        m_model->fn_8015FF60();
        fn_80161BD8(m_model.get(), m_variant, m_chara, m_variant);

        UnkFrameSearch_80169E8C search;
        search.id = 1000;
        search.frame = 0;
        fn_8015FA78(m_model.get(), fn_80169E8C, &search);
        m_frame0 = search.frame;
        search.id = 1001;
        search.frame = 0;
        fn_8015FA78(m_model.get(), fn_80169E8C, &search);
        m_frame1 = search.frame;
    }

    u8 segment[0x18];
    RwMatrix *ltm = RwFrameGetLTM(m_frame0);
    Vec3f start(ltm->pos.x, ltm->pos.y, ltm->pos.z);
    ltm = RwFrameGetLTM(m_frame1);
    Vec3f end(ltm->pos.x, ltm->pos.y, ltm->pos.z);
    start.y += 1.0f;
    end.y += 1.0f;
    fn_8015F5D8(&start, 10.0f);
    fn_8015F5D8(&end, 10.0f);
    fn_802E0B58(fn_80020A20(segment, &start, &end), &lbl_805F06D8);

    RebuildTransform();
    UpdateTarget();
    m_model->fn_80162EF4();
    m_model->fn_8016108C();
    if (m_handle.get() != 0) {
        fn_8015BE64(m_handle.get());
    }
}

void TurnOverTask::LoadFromOwner(UnkPlacement_800C9CD0 *owner) {
    fn_800C9F14(owner, &m_chara->m_position);
    fn_800C9ED4(owner, &m_chara->m_rotation);
    fn_800C9DBC(owner, &m_params, sizeof(m_params));
    m_state = 0;
}

void TurnOverTask::RebuildTransform() {
    Mtx34f matrix;

    fn_8040C148(&matrix, &lbl_805E0720);
    fn_8040B59C(&matrix, &m_chara->m_position);
    fn_8040B848(&matrix, m_chara->m_rotation.y);
    m_model->fn_801633DC(&matrix);

    u8 segment[0x18];
    RwMatrix *ltm = RwFrameGetLTM(m_frame0);
    Vec3f start(ltm->pos.x, ltm->pos.y, ltm->pos.z);
    ltm = RwFrameGetLTM(m_frame1);
    Vec3f end(ltm->pos.x, ltm->pos.y, ltm->pos.z);
    start.y += 1.0f;
    end.y += 1.0f;
    fn_80020A20(segment, &start, &end);
    fn_801E2C9C(m_target.get(), segment);
}

void TurnOverTask::UpdateTarget() {
    if (m_target.get() != 0) {
        m_target->SetRadius(s_targetRadius);
        Mtx34f matrix;
        Vec3f position(0.0f, 0.0f, 1.0f);
        fn_8040B6F8(&matrix);
        fn_8040B848(&matrix, m_chara->m_rotation.y);
        AssignVector(position, fn_8040B67C(&matrix, &position));
        fn_801E73B0(m_target.get(), &position);
    }
}

void TurnOverTask::OnCommand(CharCommand &command) {
    /* The command type is the u16 at 0x4 (the CharCommand constructor stores it there). */
    switch (*(u16 *)command.field_0x4) {
    case 0xE: /* ReadyCommand */
        if (m_state == 1) {
            m_state = 2;
            m_timer = 0.2f;
            fn_8006127C(&m_soundFlags, 1);
            fn_80160A84(m_model.get(), m_variant, 1);
            RebuildTransform();
            if (m_particle.get() != 0) {
                fn_80167448(m_particle.get(), 1);
            }
        }
        break;
    case 0xD: /* UseCommand */
        if (m_state == 2) {
            m_user = static_cast<UseCommand &>(command).m_user;
            m_state = 4;
            m_timer = 2.0f;
            fn_80160A84(m_model.get(), m_variant, 2);
            RebuildTransform();
            m_motion = fn_801602B4(m_model.get(), m_variant);
            if (m_particle.get() != 0) {
                fn_80167448(m_particle.get(), 2);
            }
            if (m_handle.get() != 0) {
                m_handle->m_shape->vf0C(-1);
            }
        }
        break;
    case 0xF: /* CancelCommand */
        if (m_state == 2) {
            m_user.reset();
            m_state = 3;
            fn_80160A84(m_model.get(), m_variant, 3);
            RebuildTransform();
        }
        break;
    case 0x1C: /* ScoreCommand: passed on to the user */
        if (boost::shared_ptr<CharInfo> user = m_user.lock()) {
            ScoreCommand &score = static_cast<ScoreCommand &>(command);
            user->PushCommand(new ScoreCommand(score.m_values[0], score.m_values[1], score.m_values[2],
                                               score.m_values[3], score.m_values[4]));
        }
        break;
    }
}

void TurnOverTask::OnAttackHit(AttackCallbackParam &param) {
    boost::shared_ptr<CharaColliAttack> target = param.m_target.lock();
    if (target.get() != 0) {
        boost::shared_ptr<CharInfo> owner = target->m_owner.lock();
        /* The owner's kind is the s32 at 0x5C of the character info (1 for a player; inferred). */
        if (owner.get() != 0 && *(s32 *)&owner->field_0x3C[0x5C - 0x3C] == 1) {
            param.m_targetIsPlayer = 1;
            return;
        }
    }
}

void TurnOverTask::OnAttackEnd(AttackCallbackParam &param) {}

void TurnOverTask::SetupAttack() {
    void *part = fn_801616CC(m_model.get(), m_variant);
    boost::weak_ptr<_stMtnPlayWork> motion = fn_801602B4(m_model.get(), m_variant);

    if (part != 0) {
        if (boost::shared_ptr<_stMtnPlayWork> work = motion.lock()) {
            boost::shared_ptr<CharaColliBase> collider =
                fn_800C7F7C(fn_80021B08(), m_chara, part, work.get(), 3, 0);
            boost::weak_ptr<CharaColliAttack> attack = fn_800C4374(fn_80021B08(), collider);
            if (boost::shared_ptr<CharaColliAttack> attackPtr = attack.lock()) {
                fn_80054DD0(attackPtr.get(), 3, 0xBC0, 999.0f,
                            boost::bind(&TurnOverTask::OnAttackHit, this, _1),
                            boost::bind(&TurnOverTask::OnAttackEnd, this, _1));
            }
            m_handle = fn_8015B8C4(collider);
        }
    }
}

void TurnOverTask::ReleaseHandle() {
    m_handle.reset();
}

void TurnOverTask::PlaySound(int which) {
    switch (which) {
    case 0: {
        char name[0x40];
        const char *modelName = fn_801626D8(m_model.get());
        sprintf(name, "%s_liftup", modelName);
        UnkSoundEntry_80208888 *entry = fn_80208888(m_sounds, name);
        if (entry != 0) {
            fn_801D560C(fn_80053164(), (u16)entry->id, 0, 0);
        }
        break;
    }
    case 1: {
        char name[0x40];
        const char *modelName = fn_801626D8(m_model.get());
        sprintf(name, "%s_cancel", modelName);
        UnkSoundEntry_80208888 *entry = fn_80208888(m_sounds, name);
        if (entry != 0) {
            fn_801D560C(fn_80053164(), (u16)entry->id, 0, 0);
        }
        break;
    }
    }
}

} // namespace TurnOver

extern "C" void fn_80125620(void) {
    fn_8006B474("TURNOVER.BIN", 1);
}

extern "C" void fn_8012561C(void) {}

void TurnOver::TurnOverTask::CreateTurnOver(UnkPlacement_800C9CD0 *owner) {
    Task *parent = fn_801EB304(fn_8001C4F8(), 11);
    new TurnOverTask(parent, owner, 1, "TURNOVER.BIN");
}

extern "C" void fn_8012558C(void) {
    fn_8006B474("TANK.BIN", 1);
}

extern "C" void fn_80125588(void) {}

void TurnOver::TurnOverTask::CreateTank(UnkPlacement_800C9CD0 *owner) {
    Task *parent = fn_801EB304(fn_8001C4F8(), 11);
    new TurnOverTask(parent, owner, 0, "TANK.BIN");
}

static GadgetParamDesc lbl_80527D94[] = {
    {1, "model", 0, 0, 0x10000, 0.0f, 0.0f, 0.0f, fn_800CBC20, 0},
    {1, "blast", 0, 0, 0x10000, 0.0f, 0.0f, 0.0f, fn_800CBC20, 0},
    {0},
};

GadgetClassDesc lbl_80527E0C = {
    "TURNOVER", fn_80125620, fn_8012561C, TurnOver::TurnOverTask::CreateTurnOver, 0, 0, 0x23, 1, 0, 10,
    lbl_80527D94,
};

GadgetClassDesc lbl_80527E30 = {
    "TANK", fn_8012558C, fn_80125588, TurnOver::TurnOverTask::CreateTank, 0, 0, 0x21, 1, 0, 10,
    lbl_80527D94,
};
