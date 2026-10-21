/* Vine::Vine (class name from RTTI): the FIRE_VINE gadget, a vine (VINE.BIN, parts "vine" and
 * "seed") that spits fireballs. While a character is within "radius" it turns towards it and,
 * every "attack sec" seconds, plays its attack motion, whose motion event 0x5000 launches a
 * Vine::FireBall at the character. Three hits (an attack collision with one of the bits 0x3D1)
 * kill it; it then drops a seed and grows back after "revive sec" seconds. A fireball
 * (JG0000FVINEBALL.DFF) flies straight for five seconds; when it hits the ground or a character it
 * leaves a Vine::FireBallBlast, a task whose attack collision grows for a moment (as the BOMB
 * gadget's Bomb::BombBlast does). Both motions run through MotionSequenceModules. File name
 * inferred from the namespace. Names marked "(name inferred)" are not original. Definitions are in
 * reverse address order (deferred inlining emits functions last-defined first). */
#include <boost/bind.hpp>
#include <boost/function.hpp>

#include "Game/TNumEdit.h"
/* This unit's .rodata is kept whole, with the same two unreferenced constant blocks of the original
 * headers as StretchGrass.cpp's (see there). */
static const float lbl_804C15BC[16] = {
    1.0f, 0.0f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f,
};
/* After TNumEdit.h: the order of the casts sets the order of the RTTI base lists in .data. */
#include "Game/CharaColliBaseCharInfo.h"
static const float lbl_804C166C[3] = {0.0f, 0.0f, 0.0f};
#include "Effect/System/EffectElement.h"
#include "Game/CharInfo.h"
#include "Game/CharaColli.h"
#include "Game/CharaColliAttack.h"
#include "Game/Gadget.h"
#include "Game/GadgetCharInfo.h"
#include "Game/GadgetColliDetectArgs.h"
#include "Game/GadgetCollision.h"
#include "Game/GadgetModelInfo.h"
#include "Game/GadgetParticle.h"
#include "Game/GadgetClassDesc.h"
#include "Game/GadgetUtil.h"
#include "Game/MotionSequenceModule.h"
#include "Game/FileQueue.h"
#include "boost/shared_ptr.hpp"
#include "boost/weak_ptr.hpp"
#include "size_assert.h"
#include "Game/GadgetCharCollision.h"

using SonicteamUSA::System::CharaColli::AttackCallback;
using SonicteamUSA::System::CharaColli::AttackCallbackParam;
using SonicteamUSA::System::CharaColli::CharaColliAttack;
using SonicteamUSA::System::CharaColli::CharaColliBase;

/* The RenderWare types and functions used here, declared locally (layouts as in rw/rwcore.h and
 * rw/rpworld.h, reduced to the members used; as in StretchGrass.cpp). */
extern "C" {
typedef struct RwV3d {
    float x;
    float y;
    float z;
} RwV3d;

typedef struct RwFrame RwFrame;

typedef struct RwMatrix {
    /* 0x00 */ RwV3d right;
    /* 0x0C */ u32 flags;
    /* 0x10 */ RwV3d up;
    /* 0x1C */ u32 pad1;
    /* 0x20 */ RwV3d at;
    /* 0x2C */ u32 pad2;
    /* 0x30 */ RwV3d pos;
    /* 0x3C */ u32 pad3;
} RwMatrix;

typedef struct RwObject {
    u8 type;
    u8 subKind;
    u8 flags;
    u8 privateFlags;
    void *parent;
} RwObject;

typedef struct RpClump {
    /* 0x00 */ RwObject object; /* the parent is the clump's frame */
} RpClump;

RwMatrix *RwFrameGetLTM(RwFrame *frame);
/* translates a frame */
RwFrame *fn_804875B0(RwFrame *frame, const RwV3d *translation, int combine);
/* Clones a clump from loaded data, the world a clump is in, removes a clump from a world, and
 * destroys a clump (behaviour inferred). */
RpClump *fn_80457090(void *data);
void *fn_8046256C(RpClump *clump);
void fn_804623EC(void *world, RpClump *clump);
void fn_80457468(RpClump *clump);
}

/* Target (class name from RTTI): a point other objects can aim at (meaning inferred). */
class Target {
  public:
    /* Moves the target to position (name inferred). */
    void fn_800CCE38(const Vec3f *position);
};

/* What fn_8015FA78 passes to fn_80169E8C: the id of the model frame to find, and the frame found
 * (as in TurnOver.cpp). */
struct UnkFrameSearch_80169E8C {
    UnkFrameSearch_80169E8C() : id(0), frame(0) {}

    /* 0x0 */ u32 id;
    /* 0x4 */ RwFrame *frame;
};
SIZE_ASSERT(UnkFrameSearch_80169E8C, 0x8);

/* The data of a motion event (8 bytes, as in TurnOver.cpp; names inferred). */
struct UnkMotionEvent_80415A18 {
    /* 0x0 */ s32 id;
    /* 0x4 */ u32 field_0x4;
};
SIZE_ASSERT(UnkMotionEvent_80415A18, 0x8);

extern "C" {
/* The model file and resource loaders (behaviour inferred). */
boost::shared_ptr<GadgetModelInfo> fn_8015F800(int arg0, const char *fileName);
void *fn_800122D4(void);
void *fn_80012700(void *manager, int arg1, const char *fileName);
/* GadgetModelInfo methods not declared in its header (behaviour inferred where named): select the
 * part named name, attach a character info to a node of a motion, the attack of a node, the motion
 * work of a named motion and the number of its first motion, show and hide a part, and two more of
 * unknown use. */
void fn_801629D8(GadgetModelInfo *model, const char *name, int arg2);
bool fn_80161AB8(GadgetModelInfo *model, const char *nodeName,
                 const boost::weak_ptr<CharInfo> &info, const char *motionName);
boost::weak_ptr<CharaColliAttack> fn_80161334(GadgetModelInfo *model, const char *nodeName);
boost::weak_ptr<_stMtnPlayWork> fn_801601A0(GadgetModelInfo *model, const char *motionName);
s32 fn_80160124(GadgetModelInfo *model, const char *motionName);
void fn_80161090(GadgetModelInfo *model, const char *name, int arg2);
void fn_801611A4(GadgetModelInfo *model, const char *name, int arg2);
void fn_80162790(GadgetModelInfo *model);
void fn_8016271C(GadgetModelInfo *model);
/* Calls callback with each part of the model (as in TurnOver.cpp). */
void fn_8015FA78(GadgetModelInfo *model, void *(*callback)(void *part, void *data), void *data);
void *fn_80169E8C(void *part, void *data);
/* The motion event iteration (as in TurnOver.cpp). */
int fn_80415A18(_stMtnPlayWork *work, int arg1, int event, void *data, int arg4);
/* Creates a target with the given flags, and removes a gadget's target (behaviour inferred). */
boost::shared_ptr<Target> fn_801E72DC(int flags);
void fn_801E7108(boost::shared_ptr<Target> *target);
/* The collider manager, a sphere collider at position, a collider from a collision file, the
 * attack collision of a collider (behaviour inferred; the other arguments are unknown). */
void *fn_80021B08(void);
boost::shared_ptr<CharaColliBase> fn_800C7EA8(void *manager, const Vec3f *position,
                                              u32 flags, int arg3, int arg4,
                                              const boost::weak_ptr<CharInfo> &owner,
                                              const Vec3f *offset, float radius);
boost::shared_ptr<CharaColliBase> fn_800C81F4(void *manager, const boost::weak_ptr<CharInfo> &owner,
                                              void *collision, int arg3, int arg4);
boost::weak_ptr<CharaColliAttack> fn_800C4374(void *manager,
                                              boost::shared_ptr<CharaColliBase> collider);
/* Wraps collider in a new handle, and advances the handle by dt (behaviour inferred). */
boost::shared_ptr<GadgetCharCollision> fn_8015B8C4(boost::shared_ptr<CharaColliBase> collider);
void fn_8015BE9C(GadgetCharCollision *handle, float dt);
/* A sweep from start to end that stops at the ground; the stopping point goes to hit (behaviour
 * inferred). */
bool fn_8015D61C(Vec3f *hit, const Vec3f *start, const Vec3f *end);
/* Starts effect id at position, pointing along direction (behaviour inferred). */
boost::weak_ptr<Effect::System::EffectElement> fn_80168C48(const Vec3f *position,
                                                           const Vec3f *direction, int id);
/* Particle controllers (as in BreakWall.cpp and TurnOver.cpp). */
boost::shared_ptr<GadgetParticle::GadgetParticleCtrl> fn_8016690C(void *part);
void fn_80167010(GadgetParticle::GadgetParticleCtrl *ctrl, int id);
void fn_8016761C(GadgetParticle::GadgetParticleCtrl *ctrl, float dt);
/* Plays sound id at position (behaviour inferred; the other arguments are unknown). */
void fn_801D54F4(void *manager, int id, const Vec3f *position, int arg3, int arg4);
/* A debug cone at position, turned by rotation (behaviour inferred). */
void fn_802E0C60(const Vec3f *position, const Vec3f *rotation, int arg2,
                 float length, float radius);
/* The pitch and yaw of a direction (behaviour inferred). */
void fn_8003B9E8(const Vec3f *direction, float *pitch, float *yaw);
/* a + b, vector += value, and direction normalised (behaviour inferred). */
Vec3f fn_8040CDD0(const Vec3f *a, const Vec3f *b);
void fn_8040CFF8(Vec3f *vector, const Vec3f *value);
Vec3f fn_8040CEE8(const Vec3f *vector);
}

/* Turns current towards target by at most step (behaviour inferred). */
float TurnAngleToward(float target, float current, float step);

/* fn_8040CD74 called as returning a - b by value (it stores the result through its first argument;
 * as in ChaoHolder.cpp). */
typedef Vec3f (*VectorSubtractFn)(const Vec3f *a, const Vec3f *b);

/* The empty character info (an .sbss shared_ptr; meaning inferred). */
extern boost::shared_ptr<CharInfo> lbl_805EE698;

/* The RenderWare copy of a vector (name inferred). */
inline void CopyToRwV3d(RwV3d &result, const Vec3f &vector) {
    result.x = vector.x;
    result.y = vector.y;
    result.z = vector.z;
}

/* Whether the squared length of delta is below or above rangeSq (names inferred). */
inline bool IsWithin(float rangeSq, const Vec3f &delta) {
    return fn_8040CF2C(&delta) < rangeSq;
}

inline bool IsBeyond(float rangeSq, const Vec3f &delta) {
    return fn_8040CF2C(&delta) > rangeSq;
}

/* vector += value (name inferred). */
inline void AddVector(Vec3f &vector, const Vec3f &value) {
    fn_8040CFF8(&vector, &value);
}

static const char *s_fileName = "VINE.BIN";
static const char *s_vineName = "vine";
static const char *s_seedName = "seed";

/* The motion sequences of the vine (0 standing, 1 attacking, 2 hit, 3 dying, 4 growing back) and of
 * the seed (0 dropping, 1 growing back); names inferred. */
static const MotionSequenceStep s_vineStand[] = {{5, -1, -1, 30, 1}};
static const MotionSequenceStep s_vineAttack[] = {{4, -1, -1, 30, 0}};
static const MotionSequenceStep s_vineHit[] = {{3, -1, -1, 30, 0}};
static const MotionSequenceStep s_vineDie[] = {{1, -1, -1, 30, 0}};
static const MotionSequenceStep s_vineRevive[] = {{2, -1, -1, 30, 0}};
static const MotionSequence s_vineSequences[] = {
    {s_vineStand, 1, -1}, {s_vineAttack, 1, -1}, {s_vineHit, 1, -1},
    {s_vineDie, 1, -1},   {s_vineRevive, 1, -1},
};
static const MotionSequenceTable s_vineTable = {s_vineSequences, 5};

static const MotionSequenceStep s_seedDrop[] = {{1, -1, -1, 30, 0}};
static const MotionSequenceStep s_seedRevive[] = {{2, -1, -1, 30, 0}};
static const MotionSequence s_seedSequences[] = {
    {s_seedDrop, 1, -1},
    {s_seedRevive, 1, -1},
};
static const MotionSequenceTable s_seedTable = {s_seedSequences, 2};

namespace Vine {

/* The three parameters the owner supplies (names from the parameter table). */
struct VineParams {
    VineParams() : radius(0.0f), attackSec(0.0f), reviveSec(0.0f) {}

    /* 0x0 */ float radius;    /* the range in which it attacks */
    /* 0x4 */ float attackSec; /* the time between attacks */
    /* 0x8 */ float reviveSec; /* the time before it grows back */
};
SIZE_ASSERT(VineParams, 0xC);

class Vine : public Gadget {
  public:
    Vine(Task *parent, UnkPlacement_800C9CD0 *owner);
    virtual ~Vine();
    virtual void Behave(float dt);
    virtual void ReloadPlacement(UnkPlacement_800C9CD0 *owner);

    /* Creates the vine under task layer 11 (name inferred). */
    static void Instantiate(UnkPlacement_800C9CD0 *owner);

    /* Method names are inferred. */
    void LoadFromOwner(UnkPlacement_800C9CD0 *owner);
    /* Turns towards the target and places the models and the target. */
    void UpdateMatrix();
    /* The attack collision's callback. */
    void OnAttack(AttackCallbackParam &param);
    /* Launches a fireball from the mouth at the target position. */
    void Shoot();
    /* Keeps or finds a character in range; true while there is one. */
    bool FindTarget();
    /* The states. */
    void UpdateStart(float dt);
    void UpdateIdle(float dt);
    void UpdateAttack(float dt);
    void UpdateDie(float dt);
    void UpdateDead(float dt);
    void UpdateRevive(float dt);
    void UpdateHit(float dt);
    /* Sets up the motion works and sequences of both parts. */
    void InitMotion();
    /* Deletes the sequence modules. */
    void ReleaseSequences() {
        for (int i = 0; i < 2; i++) {
            if (m_sequences[i] != 0) {
                delete m_sequences[i];
                m_sequences[i] = 0;
            }
        }
    }
    /* Not called anywhere (see its definition). */
    void ReleaseMotion();
    void UpdateMotion(float dt);
    /* Starts sequence index of part which, and whether the sequence of part which ended. */
    void PlaySequence(int which, int index);
    bool IsSequenceDone(int which);
    /* The square of the range at which a character is lost. */
    float GetLoseRangeSq();

    static const char *s_taskName;

    /* Member names are inferred. */
    /* 0x30 */ VineParams m_params;
    /* 0x3C */ boost::shared_ptr<GadgetCharInfo> m_chara;
    /* 0x44 */ boost::shared_ptr<GadgetModelInfo> m_models[2]; /* vine, seed */
    /* 0x54 */ boost::shared_ptr<Target> m_target;
    /* 0x5C */ Vec3f m_targetPosition;
    /* 0x68 */ float m_life;
    /* 0x6C */ float m_timer;
    /* 0x70 */ RwFrame *m_mouth; /* model frame 1 */
    /* 0x74 */ boost::weak_ptr<_stMtnPlayWork> m_motions[2];
    /* 0x84 */ s32 m_motionBase[2];
    /* 0x8C */ MotionSequenceModule *m_sequences[2];
    /* 0x94 */ boost::weak_ptr<CharInfo> m_targetChar;
    /* 0x9C */ boost::weak_ptr<Target> field_0x9C; /* not used */
    /* 0xA4 */ s32 m_state; /* 0 start, 1 idle, 2 attack, 3 die, 4 dead, 6 revive, 7 hit */
    /* 0xA8 */ s32 m_subState;
    /* 0xAC */ u32 pad_0xAC;
};
SIZE_ASSERT(Vine, 0xB0);

const char *Vine::s_taskName = "Vine";

Vine::Vine(Task *parent, UnkPlacement_800C9CD0 *owner) : Gadget(parent, owner) {
    m_name = s_taskName;
    m_chara = fn_8015CB88(0x29, 4, "Vine");
    LoadFromOwner(owner);
    m_models[0] = fn_8015F800(1, s_fileName);
    fn_801629D8(m_models[0].get(), s_vineName, 0);
    m_models[1] = fn_8015F800(1, s_fileName);
    fn_801629D8(m_models[1].get(), s_seedName, 0);
    fn_80162790(m_models[1].get());
    m_models[0]->fn_80160BA0(s_vineName);
    m_models[0]->fn_8015FF60();
    m_models[1]->fn_80160BA0(s_seedName);
    m_models[1]->fn_8015FF60();
    InitMotion();
    fn_80161AB8(m_models[0].get(), s_vineName, m_chara, s_vineName);
    {
        boost::weak_ptr<CharaColliAttack> attack = fn_80161334(m_models[0].get(), s_vineName);
        if (boost::shared_ptr<CharaColliAttack> collision = attack.lock()) {
            collision->field_0x54 = 0;
            collision->field_0x55 = 1;
            collision->m_attackCallback = AttackCallback(boost::bind(&Vine::OnAttack, this, _1));
        }
        m_models[1]->fn_80161F08(s_seedName, m_chara);
        fn_801611A4(m_models[1].get(), s_seedName, -1);
    }
    UnkFrameSearch_80169E8C search;
    search.id = 1;
    fn_8015FA78(m_models[0].get(), fn_80169E8C, &search);
    m_mouth = search.frame;
    m_target = fn_801E72DC(0x800);
    UpdateMatrix();
}

Vine::~Vine() {
    fn_801E7108(&m_target);
    ReleaseSequences();
}

void Vine::Behave(float dt) {
    for (int i = 0; i < 2; i++) {
        if (m_models[i].get() != 0) {
            m_models[i]->fn_80162FB0(dt);
        }
    }
    UpdateMotion(dt);

    switch (m_state) {
    case 0:
        UpdateStart(dt);
        break;
    case 1:
        UpdateIdle(dt);
        break;
    case 2:
        UpdateAttack(dt);
        break;
    case 3:
        UpdateDie(dt);
        break;
    case 4:
        UpdateDead(dt);
        break;
    case 6:
        UpdateRevive(dt);
        break;
    case 7:
        UpdateHit(dt);
        break;
    case 8:
        break;
    }
    UpdateMatrix();
}

void Vine::ReloadPlacement(UnkPlacement_800C9CD0 *owner) {
    LoadFromOwner(owner);
    UpdateMatrix();
    if (m_models[0].get() != 0) {
        m_models[0]->fn_80162EF4();
        m_models[0]->fn_8016108C();
        Vec3f rotation(0.0f, 0.0f, 0.0f);
        rotation.x = 90.0f * (lbl_805FAD38 / 180.0f);
        fn_802E0C60(&m_chara->m_position, &rotation, 0, 5.0f, m_params.radius);
    }
}

void Vine::LoadFromOwner(UnkPlacement_800C9CD0 *owner) {
    fn_800C9F14(owner, &m_chara->m_position);
    fn_800C9ED4(owner, &m_chara->m_rotation);
    fn_800C9DBC(owner, &m_params, sizeof(m_params));
    m_life = 3.0f;
    m_timer = 0.0f;
    m_state = 1;
    m_subState = 0;
}

void Vine::UpdateMatrix() {
    static float s_turnStep = 2.0f * (lbl_805FAD38 / 180.0f);
    static float s_halfTurn = 180.0f * (lbl_805FAD38 / 180.0f);

    if (m_state == 1 || m_state == 2) {
        Mtx34f rotation;
        fn_8040C148(&rotation, &lbl_805E0720);
        fn_8040B848(&rotation, m_chara->m_rotation.y);
        fn_8040B884(&rotation, m_chara->m_rotation.x);
        fn_8040B80C(&rotation, m_chara->m_rotation.z);
        Vec3f direction(0.0f, 1.0f, 0.0f);
        AssignVector(direction, fn_8040B67C(&rotation, &direction));
        if (direction.y < 0.0f) {
            fn_800091D0(&direction, &(const Vec3f &)((VectorSubtractFn)fn_8040CD74)(
                                        &m_targetPosition, &m_chara->m_position));
            float yaw;
            float pitch;
            fn_8003B9E8(&direction, &pitch, &yaw);
            yaw += s_halfTurn;
            m_chara->m_rotation.y = TurnAngleToward(yaw, m_chara->m_rotation.y, s_turnStep);
        } else {
            fn_800091D0(&direction, &(const Vec3f &)((VectorSubtractFn)fn_8040CD74)(
                                        &m_targetPosition, &m_chara->m_position));
            float yaw;
            float pitch;
            fn_8003B9E8(&direction, &pitch, &yaw);
            m_chara->m_rotation.y = TurnAngleToward(yaw, m_chara->m_rotation.y, s_turnStep);
        }
    }

    Mtx34f matrix;
    fn_8040C148(&matrix, &lbl_805E0720);
    fn_8040B59C(&matrix, &m_chara->m_position);
    fn_8040B848(&matrix, m_chara->m_rotation.y);
    fn_8040B884(&matrix, m_chara->m_rotation.x);
    fn_8040B80C(&matrix, m_chara->m_rotation.z);
    for (int i = 0; i < 2; i++) {
        if (m_models[i].get() != 0) {
            m_models[i]->fn_801633DC(&matrix);
        }
    }

    if (m_mouth != 0 && m_target.get() != 0) {
        RwMatrix *ltm = RwFrameGetLTM(m_mouth);
        m_target->fn_800CCE38(&Vec3f(ltm->pos.x, ltm->pos.y, ltm->pos.z));
    }
}

void Vine::OnAttack(AttackCallbackParam &param) {
    if (m_state == 1 || m_state == 2) {
        if (boost::shared_ptr<CharaColliAttack> attacker = param.m_attacker.lock()) {
            if (!(attacker->m_flags & 0x3D1)) {
                param.m_result = 5;
                return;
            }
        }
        m_life -= param.m_damage;
        if (m_life <= 0.0f) {
            m_state = 3;
            m_subState = 0;
        } else {
            m_state = 7;
            m_subState = 0;
        }
        param.m_result = 1;
    } else {
        param.m_result = 5;
    }
}

} // namespace Vine

namespace Vine {

/* What a fireball starts with (names inferred). */
struct FireBallParam {
    FireBallParam() : life(5.0f) {}
    FireBallParam &operator=(const FireBallParam &other) {
        AssignVector(position, other.position);
        AssignVector(velocity, other.velocity);
        life = other.life;
        return *this;
    }

    /* 0x00 */ Vec3f position;
    /* 0x0C */ Vec3f velocity; /* per frame */
    /* 0x18 */ float life;             /* seconds */
};
SIZE_ASSERT(FireBallParam, 0x1C);

class FireBall : public Task {
  public:
    FireBall(Task *parent, const FireBallParam &param);
    virtual ~FireBall();
    virtual void Tick(float dt);

    /* Creates a fireball under task layer 11 (name inferred). */
    static void Instantiate(const FireBallParam &param);

    /* Method names are inferred. */
    /* Moves the clump to the character info. */
    void UpdateClump();
    /* The collision handle's callback. */
    void OnColliDetect(ColliDetectArgs &args);

    static const char *s_taskName;

    /* Member names are inferred. */
    /* 0x28 */ RpClump *m_clump;
    /* 0x2C */ FireBallParam m_param;
    /* 0x48 */ boost::shared_ptr<GadgetCharInfo> m_chara;
    /* 0x50 */ boost::shared_ptr<GadgetParticle::GadgetParticleCtrl> m_particles;
    /* 0x58 */ boost::shared_ptr<GadgetCharCollision> m_handle;
    /* 0x60 */ s32 m_state; /* 0 new, 1 flying, 2 hit, 3 done */
    /* 0x64 */ u32 pad_0x64;
};
SIZE_ASSERT(FireBall, 0x68);

class FireBallBlast : public Task {
  public:
    FireBallBlast(Task *parent, const Vec3f &position);
    virtual ~FireBallBlast();
    virtual void Tick(float dt);

    /* Creates the blast under task layer 11 (name inferred). */
    static void Instantiate(const Vec3f &position);

    /* The callback of the blast's attack collision (name inferred). */
    void OnAttack(AttackCallbackParam &param);

    static const char *s_taskName;

    /* Member names are inferred. */
    /* 0x28 */ Vec3f m_position;
    /* 0x34 */ boost::shared_ptr<GadgetCharCollision> m_handle;
    /* 0x3C */ float m_radius;
    /* 0x40 */ float m_timer;
    /* 0x44 */ s32 m_state; /* 0 new, 1 starting, 2 growing, 3 done */
};
SIZE_ASSERT(FireBallBlast, 0x48);

void Vine::Shoot() {
    if (m_models[0].get() != 0 && m_mouth != 0) {
        FireBallParam param;
        RwMatrix *ltm = RwFrameGetLTM(m_mouth);
        param.position.x = ltm->pos.x;
        param.position.y = ltm->pos.y;
        param.position.z = ltm->pos.z;
        AssignVector(param.velocity,
                     fn_8040CEE8(&(const Vec3f &)((VectorSubtractFn)fn_8040CD74)(
                         &m_targetPosition, &param.position)));
        fn_8040CE38(&param.velocity, 3.0f);
        FireBall::Instantiate(param);
        fn_801D54F4(fn_80053164(), 0x7543, &param.position, 0, 1);
    }
}

bool Vine::FindTarget() {
    if (m_targetChar.expired()) {
        boost::weak_ptr<CharInfo> nearest;
        nearest = fn_800A8A7C(fn_80074194(), &m_chara->m_position);
        if (!nearest.expired()) {
            boost::shared_ptr<CharInfo> character = nearest.lock();
            Vec3f to;
            fn_800091D0(&to, &character->m_position);
            to.y = 0.0f;
            Vec3f from;
            fn_800091D0(&from, &m_chara->m_position);
            from.y = 0.0f;
            if (IsWithin(m_params.radius * m_params.radius,
                         ((VectorSubtractFn)fn_8040CD74)(&to, &from))) {
                m_targetChar = character;
                fn_800091D0(&m_targetPosition, &character->m_position);
                return true;
            }
        }
    } else {
        if (boost::shared_ptr<CharInfo> character = m_targetChar.lock()) {
            Vec3f to;
            fn_800091D0(&to, &character->m_position);
            to.y = 0.0f;
            Vec3f from;
            fn_800091D0(&from, &m_chara->m_position);
            from.y = 0.0f;
            if (IsBeyond(GetLoseRangeSq(), ((VectorSubtractFn)fn_8040CD74)(&to, &from))) {
                m_targetChar.reset();
                return false;
            }
            fn_800091D0(&m_targetPosition, &character->m_position);
            return true;
        }
    }
    return false;
}

void Vine::UpdateStart(float dt) {
    switch (m_subState) {
    case 0:
        m_state = 1;
        m_subState = 0;
        break;
    case 1:
        break;
    case 2:
        break;
    }
}

void Vine::UpdateIdle(float dt) {
    switch (m_subState) {
    case 0:
        PlaySequence(0, 0);
        m_subState = 1;
        m_timer = m_params.attackSec;
        break;
    case 1:
        if (FindTarget()) {
            m_timer -= dt;
            if (m_timer < 0.0f) {
                m_state = 2;
                m_subState = 0;
            }
        }
        break;
    case 2:
        break;
    }
}

void Vine::UpdateAttack(float dt) {
    switch (m_subState) {
    case 0:
        PlaySequence(0, 1);
        m_subState = 1;
        break;
    case 1:
        if (boost::shared_ptr<_stMtnPlayWork> work = m_motions[0].lock()) {
            UnkMotionEvent_80415A18 event;
            if (fn_80415A18(work.get(), 0, 0x5000, &event, 0) != 0) {
                Shoot();
            }
        }
        if (IsSequenceDone(0)) {
            m_state = 1;
            m_subState = 0;
        }
        break;
    case 2:
        break;
    }
}

void Vine::UpdateDie(float dt) {
    switch (m_subState) {
    case 0:
        fn_8016271C(m_models[1].get());
        m_subState = 1;
        PlaySequence(0, 3);
        PlaySequence(1, 0);
        break;
    case 1:
        if (IsSequenceDone(0)) {
            m_state = 4;
            m_subState = 0;
        }
        break;
    case 2:
        break;
    }
}

void Vine::UpdateDead(float dt) {
    switch (m_subState) {
    case 0:
        m_subState = 1;
        m_timer = m_params.reviveSec;
        fn_801611A4(m_models[0].get(), s_vineName, -1);
        fn_80161090(m_models[1].get(), s_seedName, -1);
        break;
    case 1:
        if (IsSequenceDone(1)) {
            m_timer -= dt;
            if (m_timer <= 0.0f) {
                m_state = 6;
                m_subState = 0;
                PlaySequence(0, 4);
                PlaySequence(1, 1);
            }
        }
        break;
    case 2:
        break;
    }
}

void Vine::UpdateRevive(float dt) {
    switch (m_subState) {
    case 0:
        m_subState = 1;
        m_life = 3.0f;
        fn_801D54F4(fn_80053164(), 0x7530, &m_chara->m_position, 0, 1);
        break;
    case 1:
        if (IsSequenceDone(0)) {
            m_state = 1;
            m_subState = 0;
            fn_80161090(m_models[0].get(), s_vineName, -1);
            fn_80162790(m_models[1].get());
            fn_801611A4(m_models[1].get(), s_seedName, -1);
        }
        break;
    case 2:
        break;
    }
}

void Vine::UpdateHit(float dt) {
    switch (m_subState) {
    case 0:
        PlaySequence(0, 2);
        m_subState = 1;
        break;
    case 1:
        if (IsSequenceDone(0)) {
            m_state = 1;
            m_subState = 0;
        }
        break;
    case 2:
        break;
    }
}

void Vine::InitMotion() {
    m_motions[0] = fn_801601A0(m_models[0].get(), s_vineName);
    m_motionBase[0] = fn_80160124(m_models[0].get(), s_vineName);
    m_sequences[0] = new MotionSequenceModule();
    m_sequences[0]->m_table = &s_vineTable;
    m_sequences[0]->Start(0);
    m_motions[1] = fn_801601A0(m_models[1].get(), s_seedName);
    m_motionBase[1] = fn_80160124(m_models[1].get(), s_seedName);
    m_sequences[1] = new MotionSequenceModule();
    m_sequences[1]->m_table = &s_seedTable;
    m_sequences[1]->Start(0);
}

/* Not called anywhere: the linker strips it from the game. Retail emits MotionSequenceModule's
 * destructor between UpdateMotion and InitMotion, where GC/2.7 places it after the last function
 * in the file that deletes a module; this function at this point of the file reproduces that
 * (inferred, as in ElecSmallCube). */
void Vine::ReleaseMotion() {
    ReleaseSequences();
}

void Vine::UpdateMotion(float dt) {
    for (int i = 0; i < 2; i++) {
        if (m_sequences[i] != 0) {
            if (boost::shared_ptr<_stMtnPlayWork> work = m_motions[i].lock()) {
                fn_800C9178(m_sequences[i], work.get(), m_motionBase[i], dt);
                fn_800C9554(m_sequences[i], work.get());
            }
        }
    }
}

void Vine::PlaySequence(int which, int index) {
    MotionSequenceModule *sequence = m_sequences[which];
    if (sequence != 0) {
        sequence->Start(index);
    }
}

bool Vine::IsSequenceDone(int which) {
    MotionSequenceModule *sequence = m_sequences[which];
    if (sequence != 0) {
        return sequence->HasEnded();
    }
    return false;
}

float Vine::GetLoseRangeSq() {
    float radius = m_params.radius;
    float margin = 10.0f;
    return (radius + margin) * (radius + margin);
}

} // namespace Vine

extern "C" void fn_8014EE20(void) {
    fn_8006B474("VINE.BIN", 1);
}

extern "C" void fn_8014EE1C(void) {}

void Vine::Vine::Instantiate(UnkPlacement_800C9CD0 *owner) {
    new Vine(fn_801EB304(fn_8001C4F8(), 11), owner);
}

static GadgetParamDesc lbl_8052A3F4[] = {
    {4, "radius", 0, 0, 0, 100.0f, 1.0f, 10000.0f, fn_800CBC20, 0},
    {4, "attack sec", 0, 0, 0, 1.0f, 0.0f, 10000.0f, fn_800CBC20, 0},
    {4, "revive sec", 0, 0, 0, 1.0f, 0.0f, 10000.0f, fn_800CBC20, 0},
    {0},
};

GadgetClassDesc lbl_8052A494 = {
    "FIRE_VINE", fn_8014EE20, fn_8014EE1C, Vine::Vine::Instantiate, 0, 0, 0x1131, 1, 0, 10, lbl_8052A3F4,
};

namespace Vine {

const char *FireBall::s_taskName = "FireBall";

FireBall::FireBall(Task *parent, const FireBallParam &param) : Task(parent) {
    m_name = s_taskName;
    m_param = param;
    m_clump = fn_80457090(fn_80012700(fn_800122D4(), 1, "JG0000FVINEBALL.DFF"));
    m_chara = fn_8015CB88(0x46, 4, "FireBall");
    fn_800091D0(&m_chara->m_position, &m_param.position);
    m_particles = fn_8016690C(m_clump);
    fn_80167010(m_particles.get(), -1);
    {
        void *collision = fn_80012700(fn_800122D4(), 1, "JG0000FVINEBALL.CCL");
        boost::shared_ptr<CharaColliBase> collider =
            fn_800C81F4(fn_80021B08(), m_chara, collision, 6, 0);
        m_handle = fn_8015B8C4(collider);
        fn_800F5980(m_handle.get(), boost::function1<void, GadgetColliDetectArgs &>(
                                        boost::bind(&FireBall::OnColliDetect, this, _1)));
    }
    m_state = 0;
}

FireBall::~FireBall() {
    m_particles.reset();
    if (m_clump != 0) {
        void *world = fn_8046256C(m_clump);
        if (world != 0) {
            fn_804623EC(world, m_clump);
        }
        fn_80457468(m_clump);
        m_clump = 0;
    }
}

void FireBall::Tick(float dt) {
    switch (m_state) {
    case 0:
        m_state = 1;
        break;
    case 1: {
        Vec3f hit;
        Vec3f next;
        fn_800091EC(&next, &(const Vec3f &)fn_8040CDD0(&m_chara->m_position,
                                                               &m_param.velocity));
        if (fn_8015D61C(&hit, &m_chara->m_position, &next)) {
            fn_800091D0(&m_chara->m_position, &hit);
            m_state = 2;
        } else {
            AddVector(m_chara->m_position, m_param.velocity);
        }
        m_param.life -= dt;
        if (m_param.life < 0.0f) {
            m_state = 3;
        }
        break;
    }
    case 2:
        FireBallBlast::Instantiate(m_chara->m_position);
        m_state = 3;
        break;
    case 3:
        m_flags |= 1;
        break;
    }

    UpdateClump();
    if (m_handle.get() != 0) {
        fn_8015BE9C(m_handle.get(), dt);
    }
    if (m_particles.get() != 0) {
        fn_8016761C(m_particles.get(), dt);
    }
}

void FireBall::UpdateClump() {
    RwV3d translation;
    CopyToRwV3d(translation, m_chara->m_position);
    fn_804875B0((RwFrame *)m_clump->object.parent, &translation, 0);
}

void FireBall::OnColliDetect(ColliDetectArgs &args) {
    if (m_state == 1) {
        m_state = 2;
    }
}

void FireBall::Instantiate(const FireBallParam &param) {
    new FireBall(fn_801EB304(fn_8001C4F8(), 11), param);
}

const char *FireBallBlast::s_taskName = "FireBallBlast";

FireBallBlast::FireBallBlast(Task *parent, const Vec3f &position) : Task(parent) {
    m_name = s_taskName;
    AssignVector(m_position, position);
    boost::shared_ptr<CharaColliBase> collider = fn_800C7EA8(
        fn_80021B08(), &m_position, 0x40000, 6, 0, lbl_805EE698, &lbl_805E07A0, 1.0f);
    m_handle = fn_8015B8C4(collider);
    Vec3f up(0.0f, 1.0f, 0.0f);
    fn_80168C48(&m_position, &up, 0x1A);
    m_state = 0;
    m_timer = 0.0f;
    m_radius = 0.0f;
}

FireBallBlast::~FireBallBlast() {}

void FireBallBlast::Tick(float dt) {
    switch (m_state) {
    case 0:
        m_timer = 0.0f;
        m_state = 1;
        m_radius = 5.0f;
        fn_801D54F4(fn_80053164(), 0x7544, &m_position, 0, 1);
        break;
    case 1:
        m_timer += dt;
        if (m_timer >= 0.13f) {
            m_state = 2;
            m_timer = 0.0f;
            if (m_handle.get() != 0) {
                boost::weak_ptr<CharaColliAttack> attack =
                    fn_800C4374(fn_80021B08(), m_handle->m_collider);
                if (boost::shared_ptr<CharaColliAttack> collision = attack.lock()) {
                    collision->field_0x54 = 1;
                    collision->m_kind = 3;
                    collision->m_power = 2.0f;
                    collision->m_flags = 1;
                    collision->m_hitCallback =
                        AttackCallback(boost::bind(&FireBallBlast::OnAttack, this, _1));
                }
                m_handle->m_collider->SetSize(m_radius, 0.0f, 0.0f, -2);
            }
        }
        break;
    case 2:
        m_timer += dt;
        if (m_timer >= 0.87f) {
            m_timer = 0.87f;
            m_state = 3;
        }
        m_radius = 11.494253f * m_timer + 5.0f;
        if (m_radius >= 15.0f) {
            m_radius = 15.0f;
        }
        if (m_handle.get() != 0) {
            m_handle->m_collider->SetSize(m_radius, 0.0f, 0.0f, -2);
        }
        break;
    case 3:
        m_flags |= 1;
        break;
    }
    if (m_handle.get() != 0) {
        fn_8015BE9C(m_handle.get(), dt);
    }
}

void FireBallBlast::OnAttack(AttackCallbackParam &param) {}

void FireBallBlast::Instantiate(const Vec3f &position) {
    Task *parent = fn_801EB304(fn_8001C4F8(), 11);
    new FireBallBlast(parent, position);
}

} // namespace Vine
