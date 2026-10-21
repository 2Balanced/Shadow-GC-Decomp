/* EscapePlane::SetEscapePlane (class name from RTTI): the escape plane (ESCAPEPLANE.BIN, shown as
 * two models), a PathObject run by an EscapePlaneController::Controller task. The ESCAPE_PLANE type
 * record only loads the model file; the four hooks at the start of the unit create, restart and
 * delete the plane and its controller. It has three targets on model frames; attacks wear its 800
 * health down; at 600, 400 and 200 it changes stage (more smoke, a hint), and at 0 it is done four
 * seconds later. Built like Game/EggBalloon.cpp. File name inferred from the namespace. Method and
 * member names are inferred. Definitions are in reverse address order (deferred inlining emits
 * functions last-defined first). */
#include <boost/bind.hpp>
#include <boost/function.hpp>

#include "Game/CharInfo.h"
#include "Game/CharaColliAttack.h"
#include "Game/Gadget.h"
#include "Game/GadgetCharInfo.h"
#include "Game/GadgetModelInfo.h"
#include "Game/GadgetParticle.h"
#include "Game/GadgetClassDesc.h"
#include "Game/GadgetUtil.h"
#include "Game/PathObject.h"
#include "Game/FileQueue.h"
#include "boost/shared_ptr.hpp"
#include "boost/weak_ptr.hpp"
#include "size_assert.h"

using SonicteamUSA::System::CharaColli::AttackCallback;
using SonicteamUSA::System::CharaColli::AttackCallbackParam;
using SonicteamUSA::System::CharaColli::CharaColliAttack;

/* Target (class name from RTTI): a point characters aim at (see Spring.cpp). Only the member set
 * here is laid out. */
class Target {
  public:
    /* Moves the target to position (name inferred). */
    void fn_800CCE38(const Vec3f *position);

    /* 0x00 */ u8 field_0x0[0x20];
    /* Sets the range (name inferred). */
    void SetRange(float range) {
        m_range = range;
    }

    /* 0x20 */ float m_range; /* (meaning inferred) */
};

/* The positioned object fn_802B4254 creates (see ChaoHolder.cpp; it may be a sound source). */
class UnkSound_802B4254;

/* The RenderWare frame of a model part and its matrices (layout from rw/rwcore.h; as in
 * EggBalloon.cpp). */
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

struct RwFrame;

/* What fn_8015FA78 passes to fn_80169E8C: the id of the model frame to find, and the frame found
 * (as in EggBalloon.cpp). */
struct UnkFrameSearch_80169E8C {
    UnkFrameSearch_80169E8C() : id(0), frame(0) {}

    /* 0x0 */ u32 id;
    /* 0x4 */ RwFrame *frame;
};
SIZE_ASSERT(UnkFrameSearch_80169E8C, 0x8);

/* What an attack collision passes along with its attack (struct name unknown); only the kind read
 * here is laid out. */
struct UnkAttackSource {
    /* 0x00 */ u8 field_0x0[0x20];
    /* 0x20 */ s32 m_kind;
};

/* The pointer at 0x14 of the attack parameters (name inferred). */
inline UnkAttackSource *GetAttackSource(AttackCallbackParam &param) {
    return *(UnkAttackSource **)(param.field_0x8 + (0x14 - 0x8));
}

/* The player manager (see BDHologram.cpp); only the member read here is laid out. */
class UnkPlayerManager_8006D6A8 {
  public:
    /* 0x00 */ u8 field_0x0[0x20];
    /* 0x20 */ void *field_0x20;
};

/* The hint manager (see BDHologram.cpp). */
class UnkHintManager_801FB180;
UnkHintManager_801FB180 *GetHintManager();

namespace EscapePlaneController {

/* EscapePlaneController::Controller (class name from RTTI; vtable at 0x8054BA00): the task that
 * moves the plane along its path (as EggBalloonController::Controller does the balloon). Only
 * its constructor and virtual destructor are used here. */
class Controller : public Task {
  public:
    Controller(Task *parent, boost::shared_ptr<PathObject> object);
    virtual ~Controller();

    /* 0x28 */ u8 field_0x28[0x48 - 0x28];
};
SIZE_ASSERT(Controller, 0x48);

} // namespace EscapePlaneController

extern "C" {
RwMatrix *RwFrameGetLTM(RwFrame *frame);

/* A 32-bit flag word (see BAGunShip.cpp; fn_80014E10 clears it). */
void fn_80014E04(u32 *flags);
void fn_80014E10(u32 *flags);
void fn_80014E1C(u32 *flags, int bit);
bool fn_80014E34(u32 *flags, int bit);

/* GadgetModelInfo helpers (behaviour inferred; see BAGunShip.cpp). */
boost::shared_ptr<GadgetModelInfo> fn_8015F800(int arg0, const char *fileName);
bool fn_80161FC8(GadgetModelInfo *model, int variant, const boost::weak_ptr<CharInfo> &info);
boost::weak_ptr<CharaColliAttack> fn_80161428(GadgetModelInfo *model, int variant);
void fn_8015FA78(GadgetModelInfo *model, void *(*callback)(void *part, void *data), void *data);
void *fn_80169E8C(void *part, void *data);
void *fn_80169D00(void *part, void *data);

/* Particle controller helpers (behaviour inferred). */
boost::shared_ptr<GadgetParticle::GadgetParticleCtrl> fn_80166AF4(void);
void fn_80166E20(GadgetParticle::GadgetParticleCtrl *ctrl, int id);
void fn_80167010(GadgetParticle::GadgetParticleCtrl *ctrl, int id);
void fn_80167448(GadgetParticle::GadgetParticleCtrl *ctrl, int id);
void fn_8016761C(GadgetParticle::GadgetParticleCtrl *ctrl, float dt);

/* Targets (see BAGunShip.cpp). */
boost::shared_ptr<Target> fn_801E72DC(int flags);
void fn_801E7108(boost::shared_ptr<Target> *target);

/* The sound source and sounds (see ChaoHolder.cpp and ElecSearchLight.cpp). */
boost::shared_ptr<UnkSound_802B4254> fn_802B4254(int kind, int arg1);
void fn_802B4510(UnkSound_802B4254 *sound, u8 flag);
void fn_802B44C8(UnkSound_802B4254 *sound, const Vec3f *position);
void fn_801D54F4(void *manager, int id, GadgetCharInfo *source, int arg3, int arg4);
void fn_801D53C4(void *manager, int id, GadgetCharInfo *charInfo, void *source, int arg4, int arg5,
                 int arg6);

/* -vector (behaviour from its body; as in BulletLaser.cpp). */
Vec3f fn_80016284(const Vec3f &vector);

/* Hints (see BDHologram.cpp). */
UnkPlayerManager_8006D6A8 *fn_8006D6A8(void);
s32 fn_801FAAC4(void *arg);
s32 fn_801FAF00(UnkHintManager_801FB180 *manager, s32 id, s32 branch, int arg3, int arg4);

/* Calls the object's vf_0x28 through the controller (as fn_802742D4 does for the balloon). */
void fn_8025F054(EscapePlaneController::Controller *controller);
}

/* A value. As a call it is not a constant expression, so the constants below are stored by a
 * static initializer, as in retail (name inferred). */
inline float Value(float value) {
    return value;
}

/* The point of the second model the plane turns about, the targets' range and the height of the
 * sound source (names inferred). */
static Vec3f s_pivot(0.186f, -59.141f, 6.969f);
static const float s_targetRange = Value(1000.0f);
static float s_soundHeight = Value(-50.0f);

/* The ids of the frames the targets sit on (name inferred). */
static u32 s_frameIds[3] = {0x6F, 0xDE, 0x14D};

namespace EscapePlane {

/* The flag word at 0x40 (struct name unknown). */
struct SetEscapePlaneFlags {
    SetEscapePlaneFlags() {
        fn_80014E04(&m_bits);
    }
    void Clear() {
        fn_80014E10(&m_bits);
    }
    void Set(int bit) {
        fn_80014E1C(&m_bits, bit);
    }
    bool Test(int bit) {
        return fn_80014E34(&m_bits, bit);
    }

    /* bit 0: done; 1: damaged (plays the damage sound) */
    u32 m_bits;
};

class SetEscapePlane : public PathObject {
  public:
    SetEscapePlane();
    virtual ~SetEscapePlane();
    virtual void Tick(float dt);
    virtual void Init();
    virtual void vf_0x14();
    virtual void UpdateMatrix();
    virtual bool HasEnded();
    virtual boost::weak_ptr<CharInfo> GetCharInfo();
    virtual void vf_0x24();
    virtual void vf_0x28();

    /* Forgets the frames and removes the targets. */
    void ClearFrames();
    /* Finds the target frames and creates the targets. */
    void FindFrames();
    /* Moves to stage stage (0-3) as the health drops. */
    void ChangeStage(int stage);
    /* The attack collision's callback. */
    void OnAttack(AttackCallbackParam &param);

    /* 0x04 */ boost::shared_ptr<GadgetCharInfo> m_chara;
    /* 0x0C */ boost::shared_ptr<GadgetModelInfo> m_model;
    /* 0x14 */ boost::shared_ptr<GadgetModelInfo> m_model2;
    /* 0x1C */ boost::shared_ptr<GadgetParticle::GadgetParticleCtrl> m_smoke;
    /* 0x24 */ boost::shared_ptr<UnkSound_802B4254> m_sound;
    /* 0x2C */ float m_health;
    /* 0x30 */ float m_timer;
    /* 0x34 */ float m_hitCooldown;
    /* 0x38 */ s32 m_state;
    /* 0x3C */ s32 m_hitKind; /* the kind of the last attack */
    /* 0x40 */ SetEscapePlaneFlags m_flags;
    /* 0x44 */ RwFrame *m_frames[3];
    /* 0x50 */ boost::shared_ptr<Target> m_targets[3];

    static const char *s_fileName;
};
SIZE_ASSERT(SetEscapePlane, 0x68);

const char *SetEscapePlane::s_fileName = "ESCAPEPLANE.BIN";

SetEscapePlane::SetEscapePlane() : m_health(800.0f) {}

SetEscapePlane::~SetEscapePlane() {
    ClearFrames();
}

void SetEscapePlane::Init() {
    m_chara = fn_8015CD90(0x7F, 4);
    m_model = fn_8015F800(1, s_fileName);
    m_model->fn_80162A68(0, 0);
    m_model2 = fn_8015F800(1, s_fileName);
    m_model2->fn_80162A68(2, 0);
    fn_80161FC8(m_model.get(), 0, m_chara);
    boost::weak_ptr<CharaColliAttack> attack = fn_80161428(m_model.get(), 0);
    if (boost::shared_ptr<CharaColliAttack> collision = attack.lock()) {
        collision->field_0x54 = 0;
        collision->field_0x55 = 1;
        collision->m_attackCallback =
            AttackCallback(boost::bind(&SetEscapePlane::OnAttack, this, _1));
    }
    FindFrames();
    m_sound = fn_802B4254(0, 1);
    fn_802B4510(m_sound.get(), 1);
    UpdateMatrix();
    m_timer = 0.0f;
    m_hitCooldown = 0.0f;
    m_state = 0;
    m_hitKind = 0;
    m_flags.Clear();
    if (m_model.get() != 0) {
        m_smoke = fn_80166AF4();
        UnkParticleAttach_80169D00 attach;
        attach.ctrl = m_smoke;
        fn_8015FA78(m_model.get(), fn_80169D00, &attach);
    }
    if (m_smoke.get() != 0) {
        fn_80167010(m_smoke.get(), -1);
    }
}

void SetEscapePlane::vf_0x14() {}

void SetEscapePlane::Tick(float dt) {
    if (m_model.get() != 0) {
        m_model->fn_80162FB0(dt);
    }
    if (m_model2.get() != 0) {
        m_model2->fn_80162FB0(dt);
    }
    if (m_smoke.get() != 0) {
        fn_8016761C(m_smoke.get(), dt);
    }
    m_hitCooldown -= dt;
    if (m_hitCooldown < 0.0f) {
        m_hitCooldown = 0.0f;
    }

    switch (m_state) {
    case 0:
        m_state = 2;
        break;
    case 2:
        if (m_health < 600.0f) {
            ChangeStage(0);
            m_state = 3;
            s32 branch = fn_801FAAC4(fn_8006D6A8()->field_0x20);
            if (branch != -1) {
                fn_801FAF00(GetHintManager(), 0x4F2, branch, 0, 0);
            }
        }
        break;
    case 3:
        if (m_health < 400.0f) {
            ChangeStage(1);
            m_state = 4;
            s32 branch = fn_801FAAC4(fn_8006D6A8()->field_0x20);
            if (branch != -1) {
                fn_801FAF00(GetHintManager(), 0x4F3, branch, 0, 0);
            }
        }
        break;
    case 4:
        if (m_health < 200.0f) {
            ChangeStage(2);
            m_state = 5;
            s32 branch = fn_801FAAC4(fn_8006D6A8()->field_0x20);
            if (branch != -1) {
                fn_801FAF00(GetHintManager(), 0x4F4, branch, 0, 0);
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
        m_timer = 4.0f;
        m_state = 7;
        break;
    case 7:
        m_timer -= dt;
        if (m_timer <= 0.0f) {
            m_flags.Set(0);
            m_state = 8;
        }
        break;
    case 1:
    case 8:
        break;
    }

    if (m_flags.Test(1)) {
        fn_801D53C4(fn_80053164(), 0x7834, m_chara.get(), this, 3, 0, 2);
    }
}

void SetEscapePlane::UpdateMatrix() {
    if (m_model2.get() != 0) {
        Mtx34f matrix;
        fn_8040B6F8(&matrix);
        fn_8040B59C(&matrix, &m_chara->m_position);
        fn_8040B59C(&matrix, &(const Vec3f &)fn_80016284(s_pivot));
        fn_8040B848(&matrix, m_chara->m_rotation.y);
        m_model2->fn_801633DC(&matrix);
    }
    if (m_model.get() != 0) {
        Mtx34f matrix;
        fn_8040B6F8(&matrix);
        fn_8040B59C(&matrix, &m_chara->m_position);
        fn_8040B848(&matrix, m_chara->m_rotation.y);
        m_model->fn_801633DC(&matrix);
    }
    for (int i = 0; i < 3; i++) {
        if (m_frames[i] != 0) {
            RwMatrix *ltm = RwFrameGetLTM(m_frames[i]);
            m_targets[i]->fn_800CCE38(&(const Vec3f &)Vec3f(ltm->pos.x, ltm->pos.y, ltm->pos.z));
        }
    }
    if (m_sound.get() != 0) {
        Vec3f position;
        fn_800091EC(&position, &m_chara->m_position);
        position.y += s_soundHeight;
        fn_802B44C8(m_sound.get(), &position);
    }
}

bool SetEscapePlane::HasEnded() {
    return m_flags.Test(0);
}

void SetEscapePlane::vf_0x24() {}

void SetEscapePlane::vf_0x28() {
    if (m_health < 200.0f) {
        if (m_smoke.get() != 0) {
            fn_80167010(m_smoke.get(), 0x4B);
        }
    } else if (m_health < 400.0f) {
        if (m_smoke.get() != 0) {
            fn_80167010(m_smoke.get(), 0x32);
        }
    } else if (m_health < 600.0f) {
        if (m_smoke.get() != 0) {
            fn_80167010(m_smoke.get(), 0x19);
        }
    }
}

void SetEscapePlane::OnAttack(AttackCallbackParam &param) {
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
        if (GetAttackSource(param) != 0) {
            m_hitKind = GetAttackSource(param)->m_kind;
        } else {
            m_hitKind = 0;
        }
        if (m_health > 0.0f) {
            param.m_result = 1;
            fn_801D54F4(fn_80053164(), 0x7830, m_chara.get(), 0, 1);
        } else {
            param.m_result = 3;
        }
    }
}

void SetEscapePlane::ChangeStage(int stage) {
    switch (stage) {
    case 0:
        m_smoke.reset();
        ClearFrames();
        m_model->fn_80162804();
        m_model->fn_80162A68(1, 0);
        FindFrames();
        UpdateMatrix();
        if (m_model.get() != 0) {
            m_smoke = fn_80166AF4();
            UnkParticleAttach_80169D00 attach;
            attach.ctrl = m_smoke;
            fn_8015FA78(m_model.get(), fn_80169D00, &attach);
        }
        if (m_smoke.get() != 0) {
            fn_80167010(m_smoke.get(), 0x19);
        }
        fn_801D54F4(fn_80053164(), 0x7832, m_chara.get(), 0, 1);
        break;
    case 1:
        if (m_smoke.get() != 0) {
            fn_80166E20(m_smoke.get(), 0x19);
            fn_80167010(m_smoke.get(), 0x32);
        }
        fn_801D54F4(fn_80053164(), 0x7832, m_chara.get(), 0, 1);
        break;
    case 2:
        if (m_smoke.get() != 0) {
            fn_80166E20(m_smoke.get(), 0x32);
            fn_80167010(m_smoke.get(), 0x4B);
        }
        fn_801D54F4(fn_80053164(), 0x7832, m_chara.get(), 0, 1);
        break;
    case 3:
        if (m_smoke.get() != 0) {
            fn_80166E20(m_smoke.get(), 0x4B);
            fn_80167448(m_smoke.get(), 0x64);
        }
        fn_801D54F4(fn_80053164(), 0x7832, m_chara.get(), 0, 1);
        break;
    }

    switch (stage) {
    case 0:
    case 1:
    case 2:
        if (m_smoke.get() != 0) {
            switch (m_hitKind) {
            case 1:
                fn_80167448(m_smoke.get(), 2);
                break;
            case 3:
                fn_80167448(m_smoke.get(), 4);
                break;
            case 5:
                fn_80167448(m_smoke.get(), 6);
                break;
            }
        }
        break;
    }
    m_flags.Set(1);
}

void SetEscapePlane::FindFrames() {
    UnkFrameSearch_80169E8C search;
    for (int i = 0; i < 3; i++) {
        search.id = s_frameIds[i];
        search.frame = 0;
        fn_8015FA78(m_model.get(), fn_80169E8C, &search);
        m_frames[i] = search.frame;
        m_targets[i] = fn_801E72DC(0x800);
        m_targets[i]->SetRange(s_targetRange);
    }
}

void SetEscapePlane::ClearFrames() {
    for (int i = 0; i < 3; i++) {
        m_frames[i] = 0;
        if (m_targets[i].get() != 0) {
            fn_801E7108(&m_targets[i]);
        }
    }
}

} // namespace EscapePlane

/* The plane's controller task. */
static EscapePlaneController::Controller *s_controller;

/* Creates the plane and its controller under task layer 6 (name inferred). */
extern "C" void fn_80255074(void) {
    boost::shared_ptr<PathObject> object =
        boost::shared_ptr<PathObject>(new EscapePlane::SetEscapePlane);
    Task *parent = fn_801EB304(fn_8001C4F8(), 6);
    s_controller = new EscapePlaneController::Controller(parent, object);
}

/* Deletes the controller (name inferred). */
extern "C" void fn_8025502C(void) {
    if (s_controller != 0) {
        delete s_controller;
        s_controller = 0;
    }
}

/* Calls the plane's vf_0x28 through the controller (name inferred). */
extern "C" void fn_80255000(void) {
    if (s_controller != 0) {
        fn_8025F054(s_controller);
    }
}

/* Deletes the controller and creates a new plane (name inferred). */
extern "C" void fn_80254FB4(void) {
    if (s_controller != 0) {
        delete s_controller;
        s_controller = 0;
    }
    fn_80255074();
}

extern "C" void fn_80254F88(void) {
    fn_8006B474("ESCAPEPLANE.BIN", 1);
}

extern "C" void fn_80254F84(void) {}

/* Retail emits this between the type's hooks and the path hooks below. */
boost::weak_ptr<CharInfo> EscapePlane::SetEscapePlane::GetCharInfo() {
    return m_chara;
}

/* The hooks the plane's path runs (behaviour inferred): create, call vf_0x28, restart and delete;
 * each returns 1. */
extern "C" int fn_80254EFC(void) {
    fn_80255074();
    return 1;
}

extern "C" int fn_80254ED8(void) {
    fn_80255000();
    return 1;
}

extern "C" int fn_80254EB4(void) {
    fn_80254FB4();
    return 1;
}

extern "C" int fn_80254E90(void) {
    fn_8025502C();
    return 1;
}

GadgetClassDesc lbl_80549B28 = {
    "ESCAPE_PLANE", fn_80254F88, fn_80254F84, 0, 0, 0, 0x13EC, 1, 0x80, 10, 0,
};
