/* The control units every boss shares (class names from RTTI): BossCtrlUnit_Shadow (the boss's
 * shadow on the ground), BossCtrlUnit_StageObject (a model of the boss's stage that fades while it
 * stands between the camera and the player), BossCtrlUnit_CheckMotionAttribute (hands the boss's
 * motion attributes on), BossCtrlUnit_CheckEvent (the boss's stage state and time),
 * BossCtrlUnit_DispGauge (the health gauge), BossCtrlUnit_CheckFirstDamage (a hint at the first
 * hit), BossCtrlUnit_FixLookCharCamera (a camera on the player and the boss),
 * BossCtrlUnit_PlayCameraMotion (a camera motion), BossCtrlUnit_DamageEff (the damage effect) and
 * BossModeCtrlUnit (runs the boss's modes). File name inferred from the classes; names marked
 * "(name inferred)" and all member names are not original. Definitions are in reverse address
 * order (deferred inlining emits functions last-defined first).
 *
 * The unit is NonMatching (81/82 functions; every section but .text is 100%): the code, data and
 * function order match, but BossCtrlUnit_StageObject::Tick differs in register allocation only
 * (retail keeps both line tests' hits in r27 and the clump moved to world 9 in r28; GC/2.7 picks
 * r30/r29 and r27). About 40 source shapes were tried. Linked as Matching, the DOL differs only in
 * those 8 instructions. */
#define BOSSCTRLUNIT_HELPERS

#include <boost/bind.hpp>
#include <boost/function.hpp>

#include "Game/BossCtrlUnit.h"
#include "Game/CameraModeFixLookChar.h"
#include "Game/CameraModeUser.h"
#include "Game/CameraState.h"
#include "Game/CharInfo.h"
#include "Game/EnemyCharInfo.h"
#include "Game/ExternalCameraRequest.h"
#include "Game/MathTypes.h"
#include "Game/StageAction.h"
#include "boost/shared_ptr.hpp"
#include "boost/weak_ptr.hpp"
#include "size_assert.h"

using SonicteamUSA::System::Camera::CameraMode;
using SonicteamUSA::System::Camera::CameraModeUser;

/* The RenderWare types used here, declared locally (layouts as in rw/rwcore.h, reduced to the
 * members used; as in BlackDoomCtrlUnit.cpp). */
extern "C" {
typedef struct RwObject {
    u8 type;
    u8 subKind;
    u8 flags;
    u8 privateFlags;
    void *parent;
} RwObject;

typedef struct RwV3d {
    float x;
    float y;
    float z;
} RwV3d;

typedef struct RwMatrix {
    RwV3d right;
    u32 flags;
    RwV3d up;
    u32 pad1;
    RwV3d at;
    u32 pad2;
    RwV3d pos;
    u32 pad3;
} RwMatrix;

typedef struct RwFrame {
    /* 0x00 */ RwObject object;
    /* 0x08 */ u8 inDirtyListLink[0x8];
    /* 0x10 */ RwMatrix modelling;
} RwFrame;

typedef struct RpClump {
    /* 0x00 */ RwObject object; /* the parent is the clump's frame */
} RpClump;
}

/* A clump's colour fade (struct name unknown; as in BlackDoomCtrlUnit.cpp). */
struct UnkClumpFade_80027D5C {
    /* 0x00 */ float m_color[4];
    /* 0x10 */ u8 field_0x10[0x24 - 0x10];
    /* 0x24 */ u8 m_mode;
    /* 0x25 */ u8 field_0x25[0x2C - 0x25];

    /* Sets the alpha (name inferred; as in BlackDoomCtrlUnit.cpp). */
    void SetAlpha(float alpha) {
        m_color[3] = alpha;
    }
};
SIZE_ASSERT(UnkClumpFade_80027D5C, 0x2C);

/* The world of each stage area (layout as in DevilDoomCtrlUnit.cpp). */
struct UnkWorldEntry_8051D208 {
    /* 0x00 */ u8 field_0x0[0x10];
    /* 0x10 */ void *m_world;
    /* 0x14 */ u8 field_0x14[0x18 - 0x14];
};
extern UnkWorldEntry_8051D208 lbl_8051D208[];

/* The stage (class name unknown; fn_8003A840 returns it): the world of each of its areas (as in
 * NpcMotion.cpp). */
class UnkStage_8003A840 {
  public:
    void *GetWorld(int area);
};

/* A line test and its hits (as in BlackDoomCtrlUnit.cpp). */
struct UnkLine_80020A20 {
    u8 field_0x0[0x18];
};
struct UnkLineHits_80021FE4;
struct UnkLineHit_8002735C {
    /* 0x00 */ u8 field_0x0[0x14];
    /* 0x14 */ void *m_object;
};

class UnkCharaList_80074194;
class UnkCameraManager_80009548;
class UnkHintManager_801FB180;
struct UnkStageData_80079808;
struct UnkPlayerInfo_80178B5C;

/* The player id (see BlackBullCtrlUnit.cpp). */
extern s32 lbl_805EE588;

UnkHintManager_801FB180 *GetHintManager();

extern "C" {
/* Characters (see BlackDoomCtrlUnit.cpp). */
UnkCharaList_80074194 *fn_80074194(void);
boost::weak_ptr<CharInfo> fn_800A8B74(UnkCharaList_80074194 *manager, int id);
/* Whether the player is in an event (behaviour inferred). */
bool fn_800B77A0(const boost::shared_ptr<CharInfo> *chara);
EnemyCharInfo *fn_801792D0(EnemyCharInfo *const *body);

/* The line test (see BlackDoomCtrlUnit.cpp); the normal of the surface hit (as in
 * DiablonPartsBeam.cpp). */
void *fn_80020A20(UnkLine_80020A20 *line, const Vec3f *start, const Vec3f *end);
void *fn_80020D40(void);
UnkLineHits_80021FE4 *fn_80021FE4(void *world, const UnkLine_80020A20 *line, int arg2, int arg3,
                                  void *filter, int arg5);
UnkLineHit_8002735C *fn_8002735C(UnkLineHits_80021FE4 *hits);
const Vec3f *fn_8001E9BC(void *object);
const Vec3f *fn_800266D4(void *object);
void fn_80026FB4(UnkLineHits_80021FE4 *hits, int arg1);

/* Draws a shadow (behaviour inferred). */
void fn_802DE99C(float size, UnkShadow_8009368C *shadow);

/* The camera's position (see BDHologram.cpp): the screen and the camera of a screen. */
void *fn_8000DCC0(void);
int fn_800623A8(void *screen);
UnkCameraManager_80009548 *fn_80009548(void);
const Vec3f *fn_80010244(UnkCameraManager_80009548 *manager, int index);

/* RenderWare clumps and frames, and the models' fades (see BlackDoomCtrlUnit.cpp). */
void *fn_8046256C(RpClump *clump);
void fn_804623EC(void *world, RpClump *clump);
void fn_804622FC(void *world, RpClump *clump);
RpClump *fn_80457090(void *data);
void fn_80457468(RpClump *clump);
void *fn_80169364(RpClump *clump);
int fn_80169350(void *frame);
UnkStage_8003A840 *fn_8003A840(void);
RwFrame *fn_8048722C(RwFrame *frame);
UnkClumpFade_80027D5C *fn_80027D5C(UnkClumpFade_80027D5C *fade);
void fn_80038198(RpClump *clump, UnkClumpFade_80027D5C *fade);

/* The resource loader and textures (see GadgetModelInfo.cpp). */
void *fn_800122D4(void);
void *fn_8001277C(void *loader, s32 slot, int arg2);
void *fn_80012700(void *loader, int group, const char *fileName);
void *fn_80013BC4(void);
void *fn_8048D0C4(void *dictionary);
void fn_8004F9E8(void *out, void *manager, RpClump *clump, void *dictionary, const char *name,
                 int arg4, int arg5);

/* Matrices (see GadgetUtil.h). */
void fn_8040C148(Mtx34f *out, const Mtx34f *m);
void fn_8040B034(Mtx34f *m, const Vec3f *offset);
void fn_8040B59C(Mtx34f *m, const Vec3f *offset);
void fn_8040B80C(Mtx34f *m, float radians);
void fn_8040B848(Mtx34f *m, float radians);
void fn_8040B884(Mtx34f *m, float radians);

/* Vectors (see BlackDoomCtrlUnit.cpp). */
void fn_8040CDD0(Vec3f *out, const Vec3f *a, const Vec3f *b);

/* The motion's attributes (see BlackBullCtrlUnit.cpp). */
int fn_80415854(_stMtnPlayWork *work, int arg1, u16 *type, int *data, int arg4);

/* The health gauge: its value and whether it shows (see HeavyDogCtrlUnit.cpp). */
void fn_800BE2DC(float ratio);
void fn_800BE2EC(int show);

/* Hints (see BDHologram.cpp). */
s32 fn_801FAF00(UnkHintManager_801FB180 *manager, s32 id, s32 branch, int arg3, int arg4);

/* The stage data and the player's record of it (see TGi.h). */
UnkStageData_80079808 *fn_80079808(void);
UnkPlayerInfo_80178B5C *fn_80178B5C(UnkStageData_80079808 *data, int player);
int fn_8016EEC8(UnkPlayerInfo_80178B5C *player);

/* Camera modes (see CameraManagerUnit.cpp) and camera motions (see EggLastMechCamera.cpp). */
boost::shared_ptr<CameraMode> fn_8026F6A4(int kind, UnkCameraModeArgs_8000EBB0 *args, int index);
UnkCameraData_80012510 *fn_80011CE8(void *resource, int arg1);
int fn_80011B7C(UnkCameraData_80012510 *data, float *time, const float *step);
void fn_8000FBFC(const boost::shared_ptr<CameraMode> &mode, UnkCameraData_80012510 *data, float time,
                 const Mtx34f *matrix);

/* The damage effect (see EggMecaCtrlUnit.cpp). */
void fn_80336888(u32 arg0, UnkClumpFade_80027D5C *fade, float arg2, float arg3, float arg4,
                 float arg5);
}

/* fn_8040CD74 called as returning a - b by value (see ArkCannon.cpp). */
typedef Vec3f (*VectorSubtractFn)(const Vec3f *a, const Vec3f *b);

inline const Vec3f &Ref(const Vec3f &vector) {
    return vector;
}

/* The boss's character info, which BossVarParam keeps in a shared_ptr (its px is mapped as
 * m_chara; as in DevilDoomCtrlUnitPsychoObject.cpp). */
inline const boost::shared_ptr<CharInfo> &BossCharInfo(BossVarParam *param) {
    return *reinterpret_cast<const boost::shared_ptr<CharInfo> *>(&param->m_chara);
}

/* Puts clump in world (name inferred; as in BlackDoomCtrlUnit.cpp). */
inline void SetClumpWorld(RpClump *clump, void *world) {
    void *current = fn_8046256C(clump);
    if (current != world) {
        if (current != 0) {
            fn_804623EC(current, clump);
        }
        fn_804622FC(world, clump);
    }
}

/* The object a line test hit (name inferred; as in BlackDoomCtrlUnit.cpp). */
inline void *HitObject(UnkLineHits_80021FE4 *hits) {
    return fn_8002735C(hits)->m_object;
}

/* BossModeCtrlUnit */

SIZE_ASSERT(BossModeCtrlUnit, 0x1C);

BossModeCtrlUnit::BossModeCtrlUnit(BossVarParam *owner) : BossCtrlUnit(owner), m_mode(0) {
    m_kind = 1;
}

BossModeCtrlUnit::~BossModeCtrlUnit() {
    if (m_mode != 0) {
        delete m_mode;
        m_mode = 0;
    }
}

int BossModeCtrlUnit::Tick(float dt) {
    bool again;
    do {
        again = false;
        if (m_mode != 0) {
            if (m_owner->m_nextMode == 0) {
                m_mode->Tick(dt);
            }
            if (m_owner->m_nextMode != 0) {
                delete m_mode;
                m_mode = 0;
                m_owner->field_0x10 = m_owner->m_mode;
            }
        }
        if (m_mode == 0) {
            m_owner->m_mode = m_owner->m_nextMode;
            m_owner->m_nextMode = 0;
            m_mode = CreateMode(m_owner->m_mode);
            if (m_owner->m_nextMode != 0) {
                delete m_mode;
                m_mode = 0;
                again = true;
            }
        }
    } while (again);
    return false;
}

/* BossCtrlUnit_DamageEff */

BossCtrlUnit_DamageEff::BossCtrlUnit_DamageEff(BossVarParam *param) : BossCtrlUnit(param) {
    m_kind = 2;
    fn_80336888(m_owner->field_0x34, m_owner->m_fade, 1.0f, 0.0f, 0.0f, 1.0f);
}

BossCtrlUnit_DamageEff::~BossCtrlUnit_DamageEff() {}

int BossCtrlUnit_DamageEff::Tick(float dt) {
    return true;
}

/* BossCtrlUnit_PlayCameraMotion */

SIZE_ASSERT(BossCtrlUnit_PlayCameraMotion, 0x54);

BossCtrlUnit_PlayCameraMotion::BossCtrlUnit_PlayCameraMotion(BossVarParam *owner, int motion,
                                                             float arg2, float arg3)
    : BossCtrlUnit(owner) {
    Mtx34f matrix;
    fn_8040B034(&matrix, &fn_801792D0(&m_owner->m_chara)->m_position);
    fn_8040C148(&m_matrix, &matrix);
    fn_8040B848(&m_matrix, m_owner->m_chara->m_rotation.y);
    fn_8040B884(&m_matrix, m_owner->m_chara->m_rotation.x);
    fn_8040B80C(&m_matrix, m_owner->m_chara->m_rotation.z);
    Init(motion, arg2, arg3);
}

BossCtrlUnit_PlayCameraMotion::BossCtrlUnit_PlayCameraMotion(BossVarParam *owner, int motion,
                                                             const Vec3f *position,
                                                             const Vec3f *rotation, float arg4,
                                                             float arg5)
    : BossCtrlUnit(owner) {
    Mtx34f matrix;
    fn_8040B034(&matrix, position);
    fn_8040C148(&m_matrix, &matrix);
    fn_8040B848(&m_matrix, rotation->y);
    fn_8040B884(&m_matrix, rotation->x);
    fn_8040B80C(&m_matrix, rotation->z);
    Init(motion, arg4, arg5);
}

void BossCtrlUnit_PlayCameraMotion::Init(int motion, float arg1, float arg2) {
    m_kind = 3;
    void *resource = fn_8001277C(fn_800122D4(), 0x16, motion);
    if (resource != 0) {
        m_data = fn_80011CE8(resource, 2);
        if (m_data != 0) {
            m_time = m_data->m_startTime;
        }
    }
    m_mode = fn_80009704(0, 0);
    if (m_mode.get() != 0) {
        fn_800A6A24(&m_mode.get()->m_onSwitch,
                    &(const CameraModeUser::SwitchCallback &)CameraModeUser::SwitchCallback(
                        boost::bind(&BossCtrlUnit_PlayCameraMotion::OnSwitch, this, _1)));
        fn_800A673C(&m_mode.get()->m_onUpdate,
                    &(const CameraModeUser::UpdateCallback &)CameraModeUser::UpdateCallback(
                        boost::bind(&BossCtrlUnit_PlayCameraMotion::OnUpdate, this, _1)));
    }
    m_request = fn_801CED24(boost::shared_ptr<CameraMode>(m_mode), 0, 0, 0, 0, 0, 1);
}

BossCtrlUnit_PlayCameraMotion::~BossCtrlUnit_PlayCameraMotion() {
    fn_801CEAD4(&m_request);
}

int BossCtrlUnit_PlayCameraMotion::Tick(float dt) {
    float step = 60.0f * dt;
    if (fn_80011B7C(m_data, &m_time, &step) != 0 && m_owner->m_active) {
        return true;
    }
    return false;
}

void BossCtrlUnit_PlayCameraMotion::Apply() {
    if (m_data != 0) {
        fn_8000FBFC(boost::shared_ptr<CameraMode>(m_mode), m_data, m_time, &m_matrix);
    }
}

void BossCtrlUnit_PlayCameraMotion::OnSwitch(CameraModeUser::SwitchEventArgs &args) {
    Apply();
}

void BossCtrlUnit_PlayCameraMotion::OnUpdate(CameraModeUser::UpdateEventArgs &args) {
    Apply();
}

/* BossCtrlUnit_FixLookCharCamera */

SIZE_ASSERT(BossCtrlUnit_FixLookCharCamera, 0x40);

BossCtrlUnit_FixLookCharCamera::BossCtrlUnit_FixLookCharCamera(BossVarParam *param)
    : BossCtrlUnit(param) {
    m_kind = 2;
    field_0x8 = 50.0f;
    field_0xC = 16.0f;
    m_blendIn = 1.0f;
    m_blendOut = 1.0f;
    fn_800091D0(&m_offset, &lbl_805E07A0);
    m_restart = 0;
}

BossCtrlUnit_FixLookCharCamera::~BossCtrlUnit_FixLookCharCamera() {
    if (!m_request.expired()) {
        fn_801CEAD4(&m_request);
    }
}

bool BossCtrlUnit_FixLookCharCamera::IsEnabled() {
    return true;
}

int BossCtrlUnit_FixLookCharCamera::Tick(float dt) {
    if (!m_owner->m_active) {
        m_restart = 1;
        if (!m_request.expired()) {
            fn_801CEAD4(&m_request);
            m_request.reset();
        }
        return false;
    }
    if (IsEnabled()) {
        if (m_request.expired()) {
            UnkPlayerInfo_80178B5C *player = fn_80178B5C(fn_80079808(), 0);
            if (player == 0 || fn_8016EEC8(player) != 0xC) {
                UnkCameraModeArgs_8000EBB0 args;
                args.m_distance = field_0x8;
                args.m_height = field_0xC;
                UnkCameraBlend_8000B2C0 blendIn(m_blendIn);
                UnkCameraBlend_8000B2C0 blendOut(m_blendOut);
                boost::shared_ptr<CameraMode> mode = fn_8026F6A4(0xE, &args, 0);
                PJSCamera::CameraModeFixLookChar *fix =
                    dynamic_cast<PJSCamera::CameraModeFixLookChar *>(mode.get());
                if (fix != 0) {
                    fix->SetChara(boost::weak_ptr<CharInfo>(BossCharInfo(m_owner)));
                    fix->SetOffset(m_offset);
                }
                if (m_restart) {
                    m_request = fn_801CED24(boost::shared_ptr<CameraMode>(mode), 0, 0, 0, 2,
                                            (int)&blendOut, 0);
                } else {
                    m_request = fn_801CED24(boost::shared_ptr<CameraMode>(mode), 0, 4,
                                            (int)&blendIn, 4, (int)&blendOut, 0);
                }
            }
        }
    } else if (!m_request.expired()) {
        fn_801CEAD4(&m_request);
        m_request.reset();
    }
    m_restart = 0;
    return false;
}

/* BossCtrlUnit_CheckFirstDamage */

SIZE_ASSERT(BossCtrlUnit_CheckFirstDamage, 0x14);

BossCtrlUnit_CheckFirstDamage::BossCtrlUnit_CheckFirstDamage(BossVarParam *param, int arg1,
                                                             int arg2)
    : BossCtrlUnit(param) {
    field_0xC = arg1;
    field_0x10 = arg2;
}

BossCtrlUnit_CheckFirstDamage::~BossCtrlUnit_CheckFirstDamage() {}

int BossCtrlUnit_CheckFirstDamage::Tick(float dt) {
    if (m_owner->m_health < m_owner->m_maxHealth) {
        fn_801FAF00(GetHintManager(), field_0xC, field_0x10, 0, 0);
        return true;
    }
    return false;
}

/* BossCtrlUnit_DispGauge */

SIZE_ASSERT(BossCtrlUnit_DispGauge, 0x18);

BossCtrlUnit_DispGauge::BossCtrlUnit_DispGauge(BossVarParam *param) : BossCtrlUnit(param) {
    m_kind = 2;
    fn_800BE2EC(1);
}

BossCtrlUnit_DispGauge::~BossCtrlUnit_DispGauge() {
    fn_800BE2EC(0);
}

int BossCtrlUnit_DispGauge::Tick(float dt) {
    fn_800BE2DC(m_owner->m_health / m_owner->m_maxHealth);
    return false;
}

/* BossCtrlUnit_CheckEvent */

SIZE_ASSERT(BossCtrlUnit_CheckEvent, 0xC);

BossCtrlUnit_CheckEvent::BossCtrlUnit_CheckEvent(BossVarParam *param) : BossCtrlUnit(param) {
    m_kind = 2;
}

BossCtrlUnit_CheckEvent::~BossCtrlUnit_CheckEvent() {}

int BossCtrlUnit_CheckEvent::Tick(float dt) {
    boost::weak_ptr<CharInfo> player = fn_800A8B74(fn_80074194(), lbl_805EE588);
    boost::shared_ptr<CharInfo> chara = player.lock();
    if (chara && fn_800B77A0(&chara)) {
        return false;
    }
    if (m_owner->m_active) {
        if (fn_80074470()->m_prevState != 3) {
            fn_80175EEC(fn_80074470(), 3, false);
        }
        m_owner->m_time += dt;
    } else if (fn_80074470()->m_prevState == 3) {
        fn_80175EEC(fn_80074470(), 4, false);
    }
    return false;
}

/* BossCtrlUnit_CheckMotionAttribute */

SIZE_ASSERT(BossCtrlUnit_CheckMotionAttribute, 0x18);

BossCtrlUnit_CheckMotionAttribute::BossCtrlUnit_CheckMotionAttribute(BossVarParam *param)
    : BossCtrlUnit(param) {
    m_kind = 3;
}

BossCtrlUnit_CheckMotionAttribute::~BossCtrlUnit_CheckMotionAttribute() {}

int BossCtrlUnit_CheckMotionAttribute::Tick(float dt) {
    u16 attribute;
    int value;
    _stMtnPlayWork *work = m_owner->m_motion;
    while (fn_80415854(work, 0, &attribute, &value, 0) != 0) {
        OnAttribute(attribute, value);
        work = 0;
    }
    return false;
}

/* BossCtrlUnit_StageObject */

SIZE_ASSERT(BossCtrlUnit_StageObject, 0x28);

/* Gives frame the placement in matrix (name inferred). */
inline void SetFrameMatrix(RwFrame *frame, const Mtx34f &matrix) {
    frame->modelling.right.x = matrix.values[0][0];
    frame->modelling.right.y = matrix.values[1][0];
    frame->modelling.right.z = matrix.values[2][0];
    frame->modelling.up.x = matrix.values[0][1];
    frame->modelling.up.y = matrix.values[1][1];
    frame->modelling.up.z = matrix.values[2][1];
    frame->modelling.at.x = matrix.values[0][2];
    frame->modelling.at.y = matrix.values[1][2];
    frame->modelling.at.z = matrix.values[2][2];
    frame->modelling.pos.x = matrix.values[0][3];
    frame->modelling.pos.y = matrix.values[1][3];
    frame->modelling.pos.z = matrix.values[2][3];
}

BossCtrlUnit_StageObject::BossCtrlUnit_StageObject(BossVarParam *param, const Vec3f *position,
                                                   const Vec3f *rotation, int arg3, int arg4,
                                                   const char *arg5, float radius)
    : BossCtrlUnit(param) {
    m_kind = 3;
    m_radius = radius;
    fn_800091D0(&m_position, position);
    for (int i = 0; i < 2; i++) {
        m_clumps[i] = 0;
    }
    Mtx34f matrix;
    fn_8040C148(&matrix, &lbl_805E0720);
    fn_8040B59C(&matrix, &m_position);
    fn_8040B80C(&matrix, rotation->z);
    fn_8040B848(&matrix, rotation->y);
    fn_8040B884(&matrix, rotation->x);
    UnkClumpFade_80027D5C *fade =
        (UnkClumpFade_80027D5C *)operator new(sizeof(UnkClumpFade_80027D5C));
    if (fade != 0) {
        fade = fn_80027D5C(fade);
    }
    m_fade = fade;
    if (m_fade != 0) {
        m_fade->m_mode = 4;
        m_fade->SetAlpha(1.0f);
    }
    void *data = fn_8001277C(fn_800122D4(), 0x16, arg3);
    if (data != 0) {
        m_clumps[0] = fn_80457090(data);
    }
    if (m_clumps[0] != 0) {
        int area = fn_80169350(fn_80169364(m_clumps[0]));
        fn_8003A840();
        SetClumpWorld(m_clumps[0], lbl_8051D208[area].m_world);
        if (m_fade != 0) {
            fn_80038198(m_clumps[0], m_fade);
        }
        RwFrame *frame = (RwFrame *)m_clumps[0]->object.parent;
        SetFrameMatrix(frame, matrix);
        fn_8048722C(frame);
        if (arg5 != 0) {
            void *common = fn_8001277C(fn_800122D4(), 0x16, 2);
            if (common != 0) {
                fn_8048D0C4(common);
                void *dictionary = fn_80012700(fn_800122D4(), 0x16, arg5);
                u32 texture;
                fn_8004F9E8(&texture, fn_80013BC4(), m_clumps[0], dictionary, arg5, 1, 0);
            }
        }
    }
    if (arg4 != -1) {
        data = fn_8001277C(fn_800122D4(), 0x16, arg4);
        if (data != 0) {
            m_clumps[1] = fn_80457090(data);
        }
        if (m_clumps[1] != 0) {
            int area = fn_80169350(fn_80169364(m_clumps[1]));
            fn_8003A840();
            RpClump *clump = m_clumps[1];
            SetClumpWorld(clump, lbl_8051D208[area].m_world);
            if (m_fade != 0) {
                fn_80038198(m_clumps[1], m_fade);
            }
            RwFrame *frame = (RwFrame *)m_clumps[1]->object.parent;
            SetFrameMatrix(frame, matrix);
            fn_8048722C(frame);
        }
    }
}

BossCtrlUnit_StageObject::~BossCtrlUnit_StageObject() {
    for (int i = 0; i < 2; i++) {
        if (m_clumps[i] != 0) {
            void *world = fn_8046256C(m_clumps[i]);
            if (world != 0) {
                fn_804623EC(world, m_clumps[i]);
            }
            fn_80457468(m_clumps[i]);
            m_clumps[i] = 0;
        }
    }
    if (m_fade != 0) {
        operator delete(m_fade);
        m_fade = 0;
    }
}

int BossCtrlUnit_StageObject::Tick(float dt) {
    bool hidden = false;
    int index = fn_800623A8(fn_8000DCC0());
    Vec3f camera;
    fn_800091EC(&camera, fn_80010244(fn_80009548(), index));
    boost::weak_ptr<CharInfo> player = fn_800A8B74(fn_80074194(), lbl_805EE588);
    boost::shared_ptr<CharInfo> chara = player.lock();
    Vec3f target;
    if (chara.get() != 0) {
        fn_800091D0(&target, &chara->m_position);
    }
    void *world = fn_80020D40();
    UnkLine_80020A20 line;
    fn_80020A20(&line, &camera, &target);
    UnkLineHits_80021FE4 *hits = fn_80021FE4(world, &line, 1, 6, 0, 0);
    if (hits != 0) {
        Vec3f hit;
        fn_800091EC(&hit, fn_8001E9BC(HitObject(hits)));
        if (fn_8040CF50(&Ref(((VectorSubtractFn)fn_8040CD74)(&hit, &m_position))) < m_radius) {
            hidden = true;
        }
        fn_80026FB4(hits, 1);
    } else {
        UnkLine_80020A20 back;
        fn_80020A20(&back, &target, &camera);
        UnkLineHits_80021FE4 *backHits = fn_80021FE4(world, &back, 1, 6, 0, 0);
        if (backHits != 0) {
            Vec3f hit;
            fn_800091EC(&hit, fn_8001E9BC(HitObject(backHits)));
            if (fn_8040CF50(&Ref(((VectorSubtractFn)fn_8040CD74)(&hit, &m_position))) < m_radius) {
                hidden = true;
            }
            fn_80026FB4(backHits, 1);
        }
    }
    float alpha = hidden ? 0.5f : 1.0f;
    if (m_fade != 0) {
        m_fade->SetAlpha(alpha);
    }
    void *areaWorld;
    for (int i = 0; i < 2; i++) {
        if (m_clumps[i] != 0) {
            int area = fn_80169350(fn_80169364(m_clumps[i]));
            fn_8003A840();
            areaWorld = lbl_8051D208[area].m_world;
            void *current = fn_8046256C(m_clumps[i]);
            if (alpha < 1.0f) {
                if (area < 8 && areaWorld == current) {
                    fn_804623EC(current, m_clumps[i]);
                    RpClump *clump = m_clumps[i];
                    SetClumpWorld(clump, fn_8003A840()->GetWorld(9));
                }
            } else if (areaWorld != current) {
                fn_804623EC(current, m_clumps[i]);
                RpClump *clump = m_clumps[i];
                SetClumpWorld(clump, areaWorld);
            }
        }
    }
    return false;
}

/* BossCtrlUnit_Shadow */

SIZE_ASSERT(BossCtrlUnit_Shadow, 0x38);

BossCtrlUnit_Shadow::BossCtrlUnit_Shadow(BossVarParam *param, float size) : BossCtrlUnit(param) {
    m_kind = 3;
    m_size = size;
}

BossCtrlUnit_Shadow::~BossCtrlUnit_Shadow() {}

int BossCtrlUnit_Shadow::Tick(float dt) {
    UnkLine_80020A20 line;
    Vec3f end;
    m_shadow.m_visible = !m_owner->m_hidden;
    Vec3f down(0.0f, -1000.0f, 0.0f);
    fn_8040CDD0(&end, &m_owner->m_chara->m_position, &down);
    fn_80020A20(&line, &m_owner->m_chara->m_position, &end);
    UnkLineHits_80021FE4 *hits = fn_80021FE4(fn_80020D40(), &line, 1, 0xC, 0, 0);
    if (hits != 0) {
        void *object = HitObject(hits);
        fn_800091D0(&m_shadow.m_position, fn_8001E9BC(object));
        fn_800091D0(&m_shadow.m_normal, fn_800266D4(object));
        fn_80026FB4(hits, 1);
    }
    fn_802DE99C(m_size, &m_shadow);
    return false;
}
