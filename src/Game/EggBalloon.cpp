/* EggBalloon::SetEggBalloon (class name from RTTI): the egg balloon (EGGBALLOON.BIN), a
 * PathObject run by an EggBalloonController::Controller task. The EGG_BALLOON type record only
 * loads the model file; the controller's unit creates, restarts and deletes the balloon through
 * the hooks here. The balloon has six cannons that turn toward the nearest character and fire on
 * the controller's command (message 0x303, kinds 0 and 1 for the two groups of three), two bomb
 * hatches (kind 2) and kind 3, which does nothing here. Attacks wear its 400 health down; at 300,
 * 200 and 100 it changes stage (more smoke, a hint), and at 0 it is done two seconds later. File
 * name inferred from the namespace. Method and member names are inferred. Definitions are in
 * reverse address order (deferred inlining emits functions last-defined first). */
#include <boost/bind.hpp>
#include <boost/function.hpp>

#include "Game/AngleUtil.h"
#include "Game/CharInfo.h"
#include "Game/CharaColliAttack.h"
#include "Game/EggBalloonController.h"
#include "Game/Gadget.h"
#include "Game/GadgetCharInfo.h"
#include "Game/GadgetModelInfo.h"
#include "Game/GadgetParticle.h"
#include "Game/GadgetClassDesc.h"
#include "Game/GadgetUtil.h"
#include "Game/PathObject.h"
#include "Game/UnkZeroVector3_80147830.h"
#include "Game/WeaponBullet.h"
#include "Game/FileQueue.h"
#include "boost/shared_ptr.hpp"
#include "boost/weak_ptr.hpp"
#include "size_assert.h"

using SonicteamUSA::System::CharaColli::AttackCallback;
using SonicteamUSA::System::CharaColli::AttackCallbackParam;
using SonicteamUSA::System::CharaColli::CharaColliAttack;

/* Target (class name from RTTI): a point characters aim at (see Spring.cpp). */
class Target {
  public:
    /* Moves the target to position (name inferred). */
    void fn_800CCE38(const Vec3f *position);
};

/* The positioned object fn_802B4254 creates (see ChaoHolder.cpp; it may be a sound source). */
class UnkSound_802B4254;

/* The RenderWare frame of a model part and its matrices (layout from rw/rwcore.h, which the game
 * C++ units do not include; as in Rocket.cpp). */
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

struct RwFrame {
    /* 0x00 */ u8 field_0x0[0x10];
    /* 0x10 */ RwMatrix modelling;
};

/* What fn_8015FA78 passes to fn_80169E8C: the id of the model frame to find, and the frame found
 * (as in Rocket.cpp). */
struct UnkFrameSearch_80169E8C {
    UnkFrameSearch_80169E8C() : id(0), frame(0) {}

    /* 0x0 */ u32 id;
    /* 0x4 */ RwFrame *frame;
};
SIZE_ASSERT(UnkFrameSearch_80169E8C, 0x8);

/* A model part as FindPartFrame sees it (struct name unknown). */
struct UnkClump_80038E84 {
    /* 0x0 */ u32 field_0x0;
    /* 0x4 */ void *field_0x4; /* passed to fn_80038E84 */
};

struct UnkModelPart_8014788C {
    /* 0x0 */ UnkClump_80038E84 *clump;
    /* 0x4 */ u32 field_0x4;
    /* 0x8 */ s32 index;
};

/* What FindPartFrame looks for: frame frameId of model part part (struct name unknown). */
struct UnkPartFrameSearch_8014788C {
    UnkPartFrameSearch_8014788C() : frameId(-1), part(-1), frame(0) {}

    /* 0x0 */ s32 frameId;
    /* 0x4 */ s32 part;
    /* 0x8 */ RwFrame *frame;
};
SIZE_ASSERT(UnkPartFrameSearch_8014788C, 0xC);

/* The parameters of an effect fn_80209B58 launches (layout as in EggLastMech.cpp, which has its
 * out-of-line constructor; names inferred). */
struct UnkEffectParam_80194C1C {
    UnkEffectParam_80194C1C() {
        fn_800091EC(&m_position, &lbl_805E07A0);
        fn_800091EC(&m_velocity, &lbl_805E07A0);
        m_scale = 5.0f;
        field_0x1C = 4.0f;
        m_flag = 1;
    }

    /* 0x00 */ Vec3f m_position;
    /* 0x0C */ Vec3f m_velocity;
    /* 0x18 */ float m_scale;
    /* 0x1C */ float field_0x1C;
    /* 0x20 */ u8 m_flag;
};
SIZE_ASSERT(UnkEffectParam_80194C1C, 0x24);

/* The player manager (see BDHologram.cpp); only the member read here is laid out. */
class UnkPlayerManager_8006D6A8 {
  public:
    /* 0x00 */ u8 field_0x0[0x20];
    /* 0x20 */ void *field_0x20;
};

/* The hint manager (see BDHologram.cpp). */
class UnkHintManager_801FB180;
UnkHintManager_801FB180 *GetHintManager();

extern "C" {
RwMatrix *RwFrameGetLTM(RwFrame *frame);
/* out = in transformed by matrix (behaviour inferred). */
RwV3d *fn_80482504(RwV3d *out, const RwV3d *in, const RwMatrix *matrix);
/* Turn a frame about its y and x axes, and transform it (see Door.cpp and Rocket.cpp). */
void fn_800E9CEC(RwFrame *frame, float angle, int combine);
void fn_800E9C64(RwFrame *frame, float angle, int combine);
RwFrame *fn_804876E8(RwFrame *frame, const RwMatrix *matrix, int combine);
/* The frame id of a clump (behaviour inferred). */
RwFrame *fn_80038E84(void *clump, s32 id);

/* A 32-bit flag word (see BAGunShip.cpp; fn_80014E10 clears it). */
void fn_80014E04(u32 *flags);
void fn_80014E10(u32 *flags);
void fn_80014E1C(u32 *flags, int bit);
void fn_8006127C(u32 *flags, int bit);
bool fn_80014E34(u32 *flags, int bit);

/* GadgetModelInfo helpers (behaviour inferred; see BAGunShip.cpp). */
boost::shared_ptr<GadgetModelInfo> fn_8015F800(int arg0, const char *fileName);
bool fn_80161FC8(GadgetModelInfo *model, int variant, const boost::weak_ptr<CharInfo> &info);
boost::weak_ptr<CharaColliAttack> fn_80161428(GadgetModelInfo *model, int variant);
void fn_80160A84(GadgetModelInfo *model, int index, int motion);
void fn_8015FA78(GadgetModelInfo *model, void *(*callback)(void *part, void *data), void *data);
void *fn_80169E8C(void *part, void *data);
void *fn_80169D00(void *part, void *data);
/* The character info's world matrix (see BalloonGenerator.cpp). */
void fn_8015CEA0(GadgetCharInfo *charInfo, Mtx34f *matrix);

/* Particle controller helpers (behaviour inferred). */
boost::shared_ptr<GadgetParticle::GadgetParticleCtrl> fn_80166AF4(void);
void fn_80167010(GadgetParticle::GadgetParticleCtrl *ctrl, int id);
void fn_80167448(GadgetParticle::GadgetParticleCtrl *ctrl, int id);
void fn_8016761C(GadgetParticle::GadgetParticleCtrl *ctrl, float dt);
/* While it returns true, the balloon's timers, aim and attacks stand still (meaning unknown). */
bool fn_80169CD0(void);

/* Targets (see BAGunShip.cpp). */
boost::shared_ptr<Target> fn_801E72DC(int flags);
void fn_801E7108(boost::shared_ptr<Target> *target);

/* The sound source (see ChaoHolder.cpp). */
boost::shared_ptr<UnkSound_802B4254> fn_802B4254(int kind, int arg1);
void fn_802B4498(UnkSound_802B4254 *sound, float range);
void fn_802B4510(UnkSound_802B4254 *sound, u8 flag);
void fn_802B44C8(UnkSound_802B4254 *sound, const Vec3f *position);
void fn_801D54F4(void *manager, int id, GadgetCharInfo *source, int arg3, int arg4);

/* Fires a bullet of type for a shot (see BAGunShip.cpp). */
void fn_80051C34(int type, const Weapon::ShotInfo *info);
/* Launches the effect param describes on a task layer (see EggLastMech.cpp). */
void fn_80209B58(Task *layer, int arg1, const UnkEffectParam_80194C1C *param);

/* Matrix helpers (see Helicopter.cpp). */
Mtx34f fn_8040BEF8(const Mtx34f *matrix);
Vec3f fn_8040F7F8(const Vec3f *direction);
void fn_8040E970(Vec3f *vector);

/* Hints (see BDHologram.cpp). */
UnkPlayerManager_8006D6A8 *fn_8006D6A8(void);
s32 fn_801FAAC4(void *arg);
s32 fn_801FAF00(UnkHintManager_801FB180 *manager, s32 id, s32 branch, int arg3, int arg4);
}

/* Degrees to radians (name inferred). */
inline float ToRadians(float degrees) {
    return degrees * (lbl_805FAD38 / 180.0f);
}

/* Never read in this unit (meaning unknown); set up by a static initializer. */
static float s_unused = ToRadians(180.0f);

namespace EggBalloon {

/* The flag word at 0x48 (struct name unknown). */
struct SetEggBalloonFlags {
    SetEggBalloonFlags() {
        fn_80014E04(&m_bits);
    }
    void Clear() {
        fn_80014E10(&m_bits);
    }
    void Set(int bit) {
        fn_80014E1C(&m_bits, bit);
    }
    void Clear(int bit) {
        fn_8006127C(&m_bits, bit);
    }
    bool Test(int bit) {
        return fn_80014E34(&m_bits, bit);
    }

    /* bit 0: done; 1-4: attack kinds 0-3 running; 5, 6: aim the cannons of kind 0, 1; 7: the
     * last hit sound */
    u32 m_bits;
};

class SetEggBalloon : public PathObject {
  public:
    SetEggBalloon();
    virtual ~SetEggBalloon();
    virtual void Tick(float dt);
    virtual void Init();
    virtual void vf_0x14();
    virtual void UpdateMatrix();
    virtual bool HasEnded();
    virtual boost::weak_ptr<CharInfo> GetCharInfo() {
        return m_chara;
    }
    virtual void vf_0x24();
    virtual void vf_0x28();
    virtual void OnMessage(UnkPathObjectMessage *message);

    /* Forgets the frames and removes the targets. */
    void ClearFrames() {
        for (int i = 0; i < 2; i++) {
            m_hatches[i] = 0;
        }
        for (int i = 0; i < 6; i++) {
            m_muzzles[i] = 0;
        }
        for (int i = 0; i < 6; i++) {
            fn_801E7108(&m_targets[i]);
        }
    }
    /* The attack collision's callback. */
    void OnAttack(AttackCallbackParam &param);
    /* Moves to stage stage (0-3) as the health drops. */
    void ChangeStage(int stage);
    /* Finds the model frames of model variant set. */
    void FindFrames(int set);
    /* Turns the cannons. */
    void UpdateAim();
    /* Turns cannons start to end toward the nearest character. */
    void AimCannons(int start, int end);
    /* Fires when the attack timer has run out. */
    void UpdateAttack();
    /* Finds the cannon frames of model variant set. */
    void FindCannons(int set);
    /* Drops a bomb; returns the time to the next one. */
    float DropBomb();
    /* Fires the running group of cannons; returns the time to the next shot. */
    float Shoot();

    /* 0x04 */ boost::shared_ptr<GadgetCharInfo> m_chara;
    /* 0x0C */ boost::shared_ptr<GadgetModelInfo> m_model;
    /* 0x14 */ boost::shared_ptr<GadgetParticle::GadgetParticleCtrl> m_smoke;
    /* 0x1C */ boost::shared_ptr<GadgetParticle::GadgetParticleCtrl> m_particle;
    /* 0x24 */ boost::shared_ptr<UnkSound_802B4254> m_sound;
    /* 0x2C */ float m_health;
    /* 0x30 */ float m_hitCooldown;
    /* 0x34 */ float m_attackTime;
    /* 0x38 */ float m_shotTimer;
    /* 0x3C */ float m_shotInterval;
    /* 0x40 */ float m_timer;
    /* 0x44 */ s32 m_state;
    /* 0x48 */ SetEggBalloonFlags m_flags;
    /* 0x4C */ RwFrame *m_hatches[2];
    /* 0x54 */ RwFrame *m_muzzles[6];
    /* 0x6C */ RwFrame *m_cannons[6];
    /* 0x84 */ RwFrame *m_aims[6];
    /* 0x9C */ RwMatrix m_cannonMatrices[6];
    /* 0x21C */ UnkZeroVector3_80147830 m_cannonAngles[6];
    /* 0x264 */ boost::shared_ptr<Target> m_targets[6];

    static const char *s_fileName;
};
SIZE_ASSERT(SetEggBalloon, 0x294);

const char *SetEggBalloon::s_fileName = "EGGBALLOON.BIN";

/* fn_8015FA78's callback: finds frame frameId in the model part numbered part. */
static void *FindPartFrame(void *part_, void *data) {
    /* Kept in its own variable: it gives retail's register assignment. */
    void *result = part_;
    UnkModelPart_8014788C *part = (UnkModelPart_8014788C *)part_;
    UnkPartFrameSearch_8014788C *search = (UnkPartFrameSearch_8014788C *)data;

    if (search != 0 && part->clump != 0 && part->index == search->part) {
        search->frame = fn_80038E84(part->clump->field_0x4, search->frameId);
        if (search->frame != 0) {
            return 0;
        }
    }
    return result;
}

SetEggBalloon::SetEggBalloon() : m_health(400.0f) {}

SetEggBalloon::~SetEggBalloon() {
    ClearFrames();
}

void SetEggBalloon::Init() {
    m_chara = fn_8015CB88(0x3D, 4, "SetEggBalloon");
    m_model = fn_8015F800(1, s_fileName);
    m_model->fn_80162A68(0, 0);
    m_model->fn_80160C28(0);
    m_model->fn_8015FF60();
    fn_80160A84(m_model.get(), 0, 1);
    fn_80161FC8(m_model.get(), 0, m_chara);
    boost::weak_ptr<CharaColliAttack> attack = fn_80161428(m_model.get(), 0);
    if (boost::shared_ptr<CharaColliAttack> collision = attack.lock()) {
        collision->field_0x54 = 0;
        collision->field_0x55 = 1;
        collision->m_attackCallback = AttackCallback(boost::bind(&SetEggBalloon::OnAttack, this, _1));
    }
    FindFrames(0);
    m_sound = fn_802B4254(2, 0);
    fn_802B4510(m_sound.get(), 1);
    fn_802B4498(m_sound.get(), 300.0f);
    FindCannons(0);
    UpdateMatrix();
    m_hitCooldown = 0.0f;
    m_attackTime = 0.0f;
    m_shotTimer = 0.0f;
    m_shotInterval = 1.0f;
    m_timer = 0.0f;
    m_state = 0;
    m_flags.Clear();
    for (int i = 0; i < 6; i++) {
        fn_8040E970(&m_cannonAngles[i]);
    }
}

void SetEggBalloon::vf_0x14() {}

void SetEggBalloon::Tick(float dt) {
    if (m_model.get() != 0) {
        m_model->fn_80162FB0(dt);
    }
    if (m_smoke.get() != 0) {
        fn_8016761C(m_smoke.get(), dt);
    }
    if (m_particle.get() != 0) {
        fn_8016761C(m_particle.get(), dt);
    }
    bool active = true;
    if (fn_80169CD0()) {
        active = false;
    }
    m_hitCooldown -= dt;
    if (m_hitCooldown < 0.0f) {
        m_hitCooldown = 0.0f;
    }
    if (active) {
        m_attackTime -= dt;
        if (m_attackTime < 0.0f) {
            m_attackTime = 0.0f;
        }
        m_shotTimer -= dt;
        if (m_shotTimer < 0.0f) {
            m_shotTimer = 0.0f;
        }
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
            fn_80167010(m_particle.get(), 0x29A);
        }
        m_state = 2;
        break;
    case 2:
        if (m_health < 300.0f) {
            ChangeStage(0);
            m_state = 3;
            s32 branch = fn_801FAAC4(fn_8006D6A8()->field_0x20);
            if (branch != -1) {
                fn_801FAF00(GetHintManager(), 0x430, branch, 0, 0);
            }
        }
        break;
    case 3:
        if (m_health < 200.0f) {
            ChangeStage(1);
            m_state = 4;
            s32 branch = fn_801FAAC4(fn_8006D6A8()->field_0x20);
            if (branch != -1) {
                fn_801FAF00(GetHintManager(), 0x431, branch, 0, 0);
            }
        }
        break;
    case 4:
        if (m_health < 100.0f) {
            ChangeStage(2);
            m_state = 5;
            s32 branch = fn_801FAAC4(fn_8006D6A8()->field_0x20);
            if (branch != -1) {
                fn_801FAF00(GetHintManager(), 0x432, branch, 0, 0);
            }
        }
        break;
    case 5:
        if (m_health <= 0.0f) {
            ChangeStage(3);
            m_state = 6;
        }
        break;
    case 6:
        m_timer = 2.0f;
        m_state = 7;
        break;
    case 7:
        m_timer -= dt;
        if (m_timer <= 0.0f) {
            m_timer = 0.0f;
            m_state = 8;
            m_flags.Set(0);
            m_flags.Clear(1);
            m_flags.Clear(2);
            m_flags.Clear(3);
            m_flags.Clear(4);
        }
        break;
    case 1:
    case 8:
        break;
    }

    if (active) {
        UpdateAim();
        if (m_attackTime > 0.0f) {
            UpdateAttack();
        }
    }
}

void SetEggBalloon::UpdateMatrix() {
    if (m_model.get() != 0) {
        Mtx34f matrix;
        fn_8015CEA0(m_chara.get(), &matrix);
        m_model->fn_801633DC(&matrix);
    }
    for (int i = 0; i < 6; i++) {
        fn_800E9CEC(m_cannons[i], m_cannonAngles[i].y, 0);
        fn_800E9C64(m_cannons[i], m_cannonAngles[i].x, 2);
        fn_804876E8(m_cannons[i], &m_cannonMatrices[i], 2);
    }
    if (m_sound.get() != 0) {
        Vec3f position;
        fn_800091EC(&position, &m_chara->m_position);
        position.y += -105.0f;
        fn_802B44C8(m_sound.get(), &position);
    }
    for (int i = 0; i < 6; i++) {
        if (m_targets[i].get() != 0) {
            RwMatrix *ltm = RwFrameGetLTM(m_cannons[i]);
            Vec3f position(ltm->pos.x, ltm->pos.y, ltm->pos.z);
            m_targets[i]->fn_800CCE38(&position);
        }
    }
}

bool SetEggBalloon::HasEnded() {
    return m_flags.Test(0);
}

void SetEggBalloon::vf_0x24() {}

void SetEggBalloon::vf_0x28() {
    if (m_health < 100.0f) {
        if (m_smoke.get() != 0) {
            fn_80167010(m_smoke.get(), 0x4B);
        }
    } else if (m_health < 200.0f) {
        if (m_smoke.get() != 0) {
            fn_80167010(m_smoke.get(), 0x32);
        }
    } else if (m_health < 300.0f) {
        if (m_smoke.get() != 0) {
            fn_80167010(m_smoke.get(), 0x19);
        }
    }
    if (m_particle.get() != 0) {
        fn_80167010(m_particle.get(), 0x29A);
    }
}

void SetEggBalloon::OnAttack(AttackCallbackParam &param) {
    param.m_result = 5;
    if (m_hitCooldown > 0.0f) {
        return;
    }
    if (boost::shared_ptr<CharaColliAttack> attack = param.m_attacker.lock()) {
        if ((attack->m_flags & 0x3D0) == 0) {
            return;
        }
        m_health -= param.m_damage;
        m_hitCooldown = 0.5f;
        if (m_health > 0.0f) {
            param.m_result = 1;
            if (m_flags.Test(7)) {
                fn_801D54F4(fn_80053164(), 0x754F, m_chara.get(), 0, 1);
                m_flags.Clear(7);
            } else {
                fn_801D54F4(fn_80053164(), 0x754E, m_chara.get(), 0, 1);
                m_flags.Set(7);
            }
        } else {
            param.m_result = 3;
        }
    }
}

void SetEggBalloon::ChangeStage(int stage) {
    switch (stage) {
    case 0:
        ClearFrames();
        m_smoke.reset();
        m_particle.reset();
        m_model->fn_801604FC();
        m_model->fn_80162804();
        m_model->fn_80162A68(1, 0);
        m_model->fn_80160C28(1);
        m_model->fn_8015FF60();
        fn_80160A84(m_model.get(), 1, 1);
        FindFrames(1);
        FindCannons(1);
        UpdateMatrix();
        if (m_model.get() != 0) {
            UnkParticleAttach_80169D00 attach;
            m_smoke = fn_80166AF4();
            attach.ctrl = m_smoke;
            fn_8015FA78(m_model.get(), fn_80169D00, &attach);
            m_particle = fn_80166AF4();
            attach.ctrl = m_particle;
            fn_8015FA78(m_model.get(), fn_80169D00, &attach);
        }
        if (m_smoke.get() != 0) {
            fn_80167448(m_smoke.get(), 0x19);
            fn_80167010(m_smoke.get(), 0x19);
        }
        if (m_particle.get() != 0) {
            fn_80167010(m_particle.get(), 0x29A);
        }
        fn_801D54F4(fn_80053164(), 0x7550, m_chara.get(), 0, 1);
        break;
    case 1:
        if (m_smoke.get() != 0) {
            fn_80167448(m_smoke.get(), 0x19);
            fn_80167448(m_smoke.get(), 0x32);
            fn_80167010(m_smoke.get(), 0x32);
        }
        fn_801D54F4(fn_80053164(), 0x7550, m_chara.get(), 0, 1);
        break;
    case 2:
        if (m_smoke.get() != 0) {
            fn_80167448(m_smoke.get(), 0x19);
            fn_80167448(m_smoke.get(), 0x32);
            fn_80167448(m_smoke.get(), 0x4B);
            fn_80167010(m_smoke.get(), 0x4B);
        }
        fn_801D54F4(fn_80053164(), 0x7550, m_chara.get(), 0, 1);
        break;
    case 3:
        if (m_smoke.get() != 0) {
            fn_80167448(m_smoke.get(), 0x64);
        }
        fn_801D54F4(fn_80053164(), 0x7550, m_chara.get(), 0, 1);
        break;
    }
}

void SetEggBalloon::FindFrames(int set) {
    static const u32 s_hatchIds[2] = {30, 20};
    static const s32 s_muzzleParts[2][6] = {{1, 2, 3, 4, 5, 6}, {8, 9, 10, 11, 12, 13}};
    static const u32 s_aimIds[6] = {1, 2, 3, 4, 5, 6};

    UnkFrameSearch_80169E8C search;
    for (int i = 0; i < 2; i++) {
        search.id = s_hatchIds[i];
        search.frame = 0;
        fn_8015FA78(m_model.get(), fn_80169E8C, &search);
        m_hatches[i] = search.frame;
    }
    UnkPartFrameSearch_8014788C partSearch;
    const s32 *parts = s_muzzleParts[set];
    for (int i = 0; i < 6; i++) {
        partSearch.frameId = 10;
        partSearch.part = parts[i];
        partSearch.frame = 0;
        fn_8015FA78(m_model.get(), FindPartFrame, &partSearch);
        m_muzzles[i] = partSearch.frame;
    }
    UnkFrameSearch_80169E8C aimSearch;
    for (int i = 0; i < 6; i++) {
        aimSearch.id = s_aimIds[i];
        aimSearch.frame = 0;
        fn_8015FA78(m_model.get(), fn_80169E8C, &aimSearch);
        m_aims[i] = aimSearch.frame;
    }
    for (int i = 0; i < 6; i++) {
        m_targets[i] = fn_801E72DC(0x2800);
    }
}

void SetEggBalloon::UpdateAim() {
    if (m_flags.Test(5)) {
        AimCannons(3, 6);
    } else if (m_flags.Test(6)) {
        AimCannons(0, 3);
    } else {
        float step = ToRadians(0.5f);
        for (int i = 0; i < 6; i++) {
            m_cannonAngles[i].y = TurnAngleToward(0.0f, m_cannonAngles[i].y, step);
            m_cannonAngles[i].x = TurnAngleToward(0.0f, m_cannonAngles[i].x, step);
        }
    }
}

void SetEggBalloon::AimCannons(int start, int end) {
    Vec3f target(0.0f, 0.0f, 0.0f);
    boost::weak_ptr<CharInfo> nearest = fn_800A8A7C(fn_80074194(), &m_chara->m_position);
    if (!nearest.expired()) {
        boost::shared_ptr<CharInfo> character = nearest.lock();
        fn_800091D0(&target, &character->m_position);
    }
    float minY = ToRadians(-40.0f);
    float maxY = ToRadians(40.0f);
    float minX = ToRadians(-40.0f);
    float maxX = ToRadians(40.0f);
    float step = ToRadians(0.5f);
    for (int i = start; i < end; i++) {
        RwMatrix *ltm = RwFrameGetLTM(m_aims[i]);
        Mtx34f matrix;
        matrix.values[0][0] = ltm->right.x;
        matrix.values[0][1] = ltm->up.x;
        matrix.values[0][2] = ltm->at.x;
        matrix.values[0][3] = ltm->pos.x;
        matrix.values[1][0] = ltm->right.y;
        matrix.values[1][1] = ltm->up.y;
        matrix.values[1][2] = ltm->at.y;
        matrix.values[1][3] = ltm->pos.y;
        matrix.values[2][0] = ltm->right.z;
        matrix.values[2][1] = ltm->up.z;
        matrix.values[2][2] = ltm->at.z;
        matrix.values[2][3] = ltm->pos.z;
        Vec3f direction;
        fn_800091EC(&direction, &(const Vec3f &)fn_8040B67C(
                                    &(const Mtx34f &)fn_8040BEF8(&matrix), &target));
        const Vec3f &angles = fn_8040F7F8(&direction);
        float y = angles.y;
        float x = angles.x;
        if (y < minY) {
            y = minY;
        } else if (y > maxY) {
            y = maxY;
        }
        if (x < minX) {
            x = minX;
        } else if (x > maxX) {
            x = maxX;
        }
        m_cannonAngles[i].y = TurnAngleToward(y, m_cannonAngles[i].y, step);
        m_cannonAngles[i].x = TurnAngleToward(x, m_cannonAngles[i].x, step);
    }
}

void SetEggBalloon::UpdateAttack() {
    if (m_flags.Test(1)) {
        if (m_shotTimer > 0.0f) {
            return;
        }
        m_shotTimer = Shoot();
    } else if (m_flags.Test(2)) {
        if (m_shotTimer > 0.0f) {
            return;
        }
        m_shotTimer = Shoot();
    } else if (m_flags.Test(3)) {
        if (m_shotTimer > 0.0f) {
            return;
        }
        m_shotTimer = DropBomb();
    } else if (m_flags.Test(4)) {
    }
}

void SetEggBalloon::FindCannons(int set) {
    static const s32 s_cannonParts[2][6] = {{1, 2, 3, 4, 5, 6}, {8, 9, 10, 11, 12, 13}};

    UnkPartFrameSearch_8014788C search;
    const s32 *parts = s_cannonParts[set];
    for (int i = 0; i < 6; i++) {
        search.frameId = 0xC9;
        search.part = parts[i];
        search.frame = 0;
        fn_8015FA78(m_model.get(), FindPartFrame, &search);
        m_cannons[i] = search.frame;
        m_cannonMatrices[i] = search.frame->modelling;
    }
}

void SetEggBalloon::OnMessage(UnkPathObjectMessage *message) {
    switch (message->m_type) {
    case 0x303:
        switch (message->m_kind) {
        case 0:
            m_flags.Clear(2);
            m_flags.Clear(6);
            m_flags.Clear(3);
            m_flags.Clear(4);
            if (m_flags.Test(1)) {
                m_attackTime += message->m_time;
            } else {
                m_attackTime = message->m_time;
                m_flags.Set(1);
                m_flags.Set(5);
            }
            m_shotInterval = message->m_interval;
            break;
        case 1:
            m_flags.Clear(1);
            m_flags.Clear(5);
            m_flags.Clear(3);
            m_flags.Clear(4);
            if (m_flags.Test(2)) {
                m_attackTime += message->m_time;
            } else {
                m_attackTime = message->m_time;
                m_flags.Set(2);
                m_flags.Set(6);
            }
            m_shotInterval = message->m_interval;
            break;
        case 2:
            m_flags.Clear(1);
            m_flags.Clear(5);
            m_flags.Clear(2);
            m_flags.Clear(6);
            m_flags.Clear(4);
            if (m_flags.Test(3)) {
                m_attackTime += message->m_time;
            } else {
                m_attackTime = message->m_time;
                m_flags.Set(3);
            }
            m_shotInterval = message->m_interval;
            break;
        case 3:
            m_flags.Clear(1);
            m_flags.Clear(5);
            m_flags.Clear(2);
            m_flags.Clear(6);
            m_flags.Clear(3);
            if (m_flags.Test(4)) {
                m_attackTime += message->m_time;
            } else {
                m_attackTime = message->m_time;
                m_flags.Set(4);
            }
            break;
        }
        break;
    case 0x304:
        m_flags.Clear(1);
        m_flags.Clear(5);
        m_flags.Clear(2);
        m_flags.Clear(6);
        m_flags.Clear(3);
        m_flags.Clear(4);
        m_attackTime = 0.0f;
        break;
    }
}

float SetEggBalloon::DropBomb() {
    if (m_hatches[0] != 0) {
        RwMatrix *ltm = RwFrameGetLTM(m_hatches[0]);
        Vec3f position(ltm->pos.x, ltm->pos.y, ltm->pos.z);
        Vec3f velocity(0.0f, -1.0f, 0.0f);
        UnkEffectParam_80194C1C param;
        fn_800091D0(&param.m_position, &position);
        fn_800091D0(&param.m_velocity, &velocity);
        param.m_scale = 5.0f;
        param.m_flag = 0;
        fn_80209B58(fn_801EB304(fn_8001C4F8(), 11), 0, &param);
    }
    if (m_shotInterval <= 0.0f) {
        return 2.0f;
    }
    return m_shotInterval;
}

float SetEggBalloon::Shoot() {
    static const RwV3d s_forward = {0.0f, 0.0f, 1.0f};

    int start;
    int end;
    if (m_flags.Test(1)) {
        start = 3;
        end = 6;
    } else if (m_flags.Test(2)) {
        start = 0;
        end = 3;
    } else {
        return 1.0f;
    }
    for (int i = start; i < end; i++) {
        if (m_muzzles[i] != 0) {
            RwMatrix *ltm = RwFrameGetLTM(m_muzzles[i]);
            RwV3d position = ltm->pos;
            RwV3d direction = s_forward;
            fn_80482504(&direction, &direction, ltm);
            Vec3f muzzle(position.x, position.y, position.z);
            Vec3f aim(direction.x, direction.y, direction.z);
            Weapon::ShotInfo shot(m_chara, true, muzzle, aim);
            fn_80051C34(0x11, &shot);
        }
    }
    if (m_shotInterval <= 0.0f) {
        return 1.0f;
    }
    return m_shotInterval;
}

} // namespace EggBalloon

/* The egg balloon's controller task. */
static EggBalloonController::Controller *s_controller;

/* Creates the balloon and its controller under task layer 6 (name inferred). */
extern "C" void fn_8014565C(void) {
    boost::shared_ptr<PathObject> object = boost::shared_ptr<PathObject>(new EggBalloon::SetEggBalloon);
    Task *parent = fn_801EB304(fn_8001C4F8(), 6);
    s_controller = new EggBalloonController::Controller(parent, object);
}

/* Deletes the controller (name inferred). */
extern "C" void fn_80145614(void) {
    if (s_controller != 0) {
        delete s_controller;
        s_controller = 0;
    }
}

/* Calls the balloon's vf_0x28 through the controller (name inferred). */
extern "C" void fn_801455E8(void) {
    if (s_controller != 0) {
        fn_802742D4(s_controller);
    }
}

/* Deletes the controller and creates a new balloon (name inferred). */
extern "C" void fn_8014559C(void) {
    if (s_controller != 0) {
        delete s_controller;
        s_controller = 0;
    }
    fn_8014565C();
}

extern "C" void fn_80145570(void) {
    fn_8006B474("EGGBALLOON.BIN", 1);
}

extern "C" void fn_8014556C(void) {}

GadgetClassDesc lbl_80529AF4 = {
    "EGG_BALLOON", fn_80145570, fn_8014556C, 0, 0, 0, 0x1450, 1, 0x80, 10, 0,
};
