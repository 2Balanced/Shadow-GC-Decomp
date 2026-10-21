/* BossMotion (class name from RTTI): the task that plays a boss's motion. BossCtrl creates it with
 * the boss's other tasks (0x8017A108). Its state is a separately allocated object (class name
 * unknown) that loads the motion from the boss's tables, keeps it on the character's position and
 * turn every frame, moves the character by the motion's root and keeps the model's clump updated.
 * Member and function names are inferred. Definitions are in reverse address order (deferred
 * inlining emits functions last-defined first). */
#include "Game/BossCtrlUnit.h"
#include "Game/DevilDoomCtrlUnit.h"
#include "Game/EnemyCharInfo.h"
#include "Game/MathTypes.h"
#include "Game/Task.h"
#include "boost/scoped_ptr.hpp"
#include "size_assert.h"

/* The tables a boss's BossVarParam points to (struct name unknown; see Game/DevilDoom.cpp). */
struct UnkBossDef {
    /* 0x00 */ s32 field_0x0;
    /* 0x04 */ s32 m_model;      /* the boss's model data (slot 0x16 of the loader) */
    /* 0x08 */ s32 m_motionData; /* its motions */
    /* 0x0C */ s32 m_motionBase; /* subtracted from the motions' first id */
    /* 0x10 */ s32 m_motionKind; /* passed on to the motion's creation (meaning unknown) */
};

/* The game state (struct name unknown; see Game/DevilDoomCtrlUnit.cpp): the frame time at 0x20 and
 * the motion time at 0x24. */
struct UnkGameState_8057D6A0 {
    /* 0x000 */ u8 field_0x0[0x20];
    /* 0x020 */ float m_frameTime;
    /* 0x024 */ float m_dt;
    /* 0x028 */ u8 field_0x28[0x150 - 0x28];
};
extern UnkGameState_8057D6A0 lbl_8057D6A0;

extern "C" {
/* The resource loader (see Game/DevilDoomCtrlUnit.cpp and Game/EggLastMech.cpp). */
void *fn_800122D4(void);
void *fn_80012684(void *loader, int slot, int index);
s32 fn_80012608(void *loader, int slot, int index);

/* Motions (see Game/DevilDoomCtrlUnit.cpp; behaviour inferred). */
_stMtnPlayWork *fn_8041B908(void *data, int arg1);
void fn_8041B884(_stMtnPlayWork *work);
void fn_804173A8(_stMtnPlayWork *work, const Vec3f *position, const Vec3f *rotation, int arg3);
int fn_8041583C(_stMtnPlayWork *work);
void fn_80416DC8(_stMtnPlayWork *work, int arg1, float dt);
void fn_804160F8(_stMtnPlayWork *work, int arg1, float dt);
void fn_80415EA4(_stMtnPlayWork *work);
/* The motion's root position. */
void fn_80415E34(_stMtnPlayWork *work, Vec3f *position);

/* Updates the clump updater (behaviour inferred). */
void fn_80015980(UnkClumpUpdater_80015A7C *updater, Vec3f *arg1);
}

class BossMotion : public Task {
  public:
    class Impl;

    BossMotion(Task *parent, BossVarParam *param);
    virtual ~BossMotion();
    virtual void Tick(float dt);

    /* 0x28 */ boost::scoped_ptr<Impl> m_impl;
    /* 0x2C */ u32 pad_0x2C; /* the allocation is 0x30 bytes */
};

class BossMotion::Impl {
  public:
    Impl(BossVarParam *param);
    ~Impl() {
        if (m_motion) {
            fn_8041B884(m_motion);
            m_motion = 0;
            m_param->m_motion = 0;
        }
        if (m_updater) {
            delete m_updater;
            m_updater = 0;
        }
    }
    void Update(float dt);

    /* 0x0 */ _stMtnPlayWork *m_motion;
    /* 0x4 */ UnkClumpUpdater_80015A7C *m_updater;
    /* 0x8 */ BossVarParam *m_param;
};
SIZE_ASSERT(BossMotion::Impl, 0xC);
SIZE_ASSERT(BossMotion, 0x30);

BossMotion::Impl::Impl(BossVarParam *param) : m_updater(0), m_param(param) {
    void *model = fn_80012684(fn_800122D4(), 0x16, m_param->m_def->m_model);
    m_param->m_motionBase = fn_80012608(fn_800122D4(), 0x16, m_param->m_def->m_motionData);
    m_param->m_motionBase -= m_param->m_def->m_motionBase;
    m_motion = fn_8041B908(model, m_param->m_def->m_motionKind);
    m_param->m_motion = m_motion;
}

void BossMotion::Impl::Update(float dt) {
    if (!m_updater && m_motion && m_param->m_clump) {
        m_updater = new UnkClumpUpdater_80015A7C(m_param->m_clump, m_motion);
    }
    Vec3f position = m_param->m_chara->m_position;
    Vec3f rotation = m_param->m_chara->m_rotation;
    fn_804173A8(m_motion, &position, &rotation, 0);
    for (int i = 0; i < fn_8041583C(m_motion); i++) {
        fn_80416DC8(m_motion, i, lbl_8057D6A0.m_dt);
        fn_804160F8(m_motion, i, lbl_8057D6A0.m_dt);
    }
    fn_80415EA4(m_motion);
    Vec3f root;
    fn_80415E34(m_motion, &root);
    m_param->m_chara->m_position.x = root.x;
    m_param->m_chara->m_position.y = root.y;
    m_param->m_chara->m_position.z = root.z;
    if (m_updater) {
        fn_80015980(m_updater, &m_param->field_0x48);
    }
}

BossMotion::BossMotion(Task *parent, BossVarParam *param) : Task(parent), m_impl(new Impl(param)) {
    m_name = "Motion";
}

BossMotion::~BossMotion() {}

void BossMotion::Tick(float dt) {
    m_impl->Update(lbl_8057D6A0.m_frameTime);
}
