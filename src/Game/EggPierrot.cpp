/* EggPierrot (namespace and class names from RTTI; its task's s_taskName is "EggPierrot"): the
 * Egg Pierrot, EGG_PIERROT in the stage editor (enemy/EggPierrotData_*.one, by stage). Its body
 * EggPierrot::EggPierrot is an EnemyWalkerCommon (Game/EggPierrot.h) that rides a PierrotBall,
 * which takes the hits until it bursts, and juggles three bombs (BakudanManager), which it throws
 * at its target; its AI is EggPierrotAI (Game/EggPierrotAI.cpp). File name inferred from the class.
 * Names marked "(name inferred)" and all member names are not original. Definitions are in reverse
 * address order (deferred inlining emits functions last-defined first). The unit's code reaches its
 * .data and .rodata through pooled base addresses, so the linker keeps those sections whole:
 * Game/CharaColliBaseCharInfo.h and Game/TNumEdit.h supply the RTTI base lists the .data starts
 * with (as in BkSoldier.cpp). */
/* See <new>: the unit holds local copies of bad_alloc's functions, vtable and RTTI. */
#define MSL_NEW_INLINE_BAD_ALLOC

/* See EnemyTemplate (Game/Enemy.h): keeps its destructor out of line, as retail has it, without the
 * weak destructors ENEMY_REFERER_DTOR adds (they would move the zero-filled vtable copies of this
 * unit's whole .data). */
#define ENEMY_TEMPLATE_DTOR_OUT_OF_LINE

#include <stdio.h>
#include <string.h>

/* This unit's .rodata is kept whole, with unreferenced constant blocks of the original headers in
 * it (as in BkSoldier.cpp): seven zero words before the CharaColliBase names. */
static const float s_zeros[7] = {0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f};
/* CharaColliBaseCharInfo.h before TNumEdit.h: the order of the casts sets the order of the RTTI
 * base lists in .data (as in BkLarva.cpp). */
#include "Game/CharaColliBaseCharInfo.h"
#include "Game/TNumEdit.h"

#include "Game/EggPierrot.h"
#include "Game/CharInfo.h"
#include "Game/CharaColli.h"
#include "Game/CharaColliAttack.h"
#include "Game/EnemyRender.h"
#include "Game/Gadget.h"
#include "Game/GadgetClassDesc.h"
#include "Game/GadgetSound.h"
#include "Game/MathTypes.h"
#include "Game/Quaternion.h"
#include "Game/Random.h"
#include "Game/ResourceOneFile.h"
#include "Game/ThrowBomb.h"
#include "Game/Vibration.h"
#include "boost/shared_ptr.hpp"
#include "boost/weak_ptr.hpp"
#include <boost/function.hpp>
/* After the shared_ptr headers: retail emits the instances of shared_ptr's function templates
 * before those of boost::bind (as in ThrowBomb.cpp). */
#include <boost/bind.hpp>

using SonicteamUSA::System::CharaColli::AttackCallback;
using SonicteamUSA::System::CharaColli::AttackCallbackParam;
using SonicteamUSA::System::CharaColli::CharaColliAttack;
using SonicteamUSA::System::CharaColli::CharaColliBase;

#include "Game/MathConstants.h"
/* The flags of an enemy's colliders (Game/Enemy.cpp). */
extern const u32 lbl_805F56D0;

/* The cursor a parameter's label callback receives (layout as in Helicopter.cpp). */
class UnkLabelQuery_800CBC20 {
  public:
    /* 0x0 */ const GadgetParamDesc *params;
    /* 0x4 */ s32 index; /* the parameter being shown */
    /* 0x8 */ s32 value; /* its current value */
};

/* The game state (struct name unknown; as far as this unit reads it; see ThrowBomb.cpp). */
struct UnkGameState_8057D6A0 {
    /* 0x000 */ u8 field_0x0[0xA8];
    /* 0x0A8 */ s32 m_stageId;
};

/* Target (class name from RTTI): a point characters aim at. Only the flag word is mapped here
 * (bit 0 makes the target active; names inferred, as in ThrowBomb.cpp). */
class Target {
  public:
    void fn_800CCE38(const Vec3f *position);

    void TurnOn() {
        m_flags |= 1;
    }
    void TurnOff() {
        m_flags &= ~1;
    }
    u32 GetFlags() {
        return m_flags;
    }
    void SetFlags(u32 flags) {
        m_flags = flags;
    }

    /* 0x00 */ u8 field_0x0[0x30];
    /* 0x30 */ u32 m_flags;
};

/* EnemyTarget (class name from RTTI; see Game/EnemyTarget.cpp). */
class EnemyTarget {
  public:
    boost::shared_ptr<Target> GetTarget(int index);
};

/* The model of the ball (class name unknown; as in ThrowBomb.cpp). */
class UnkModel_801A3E9C {
  public:
    UnkModel_801A3E9C();
    ~UnkModel_801A3E9C();

    /* 0x0 */ u32 field_0x0;
    /* 0x4 */ s32 m_frameCount;
};

/* A RenderWare matrix and frame (as in ThrowBomb.cpp). */
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

/* The parameters of a throw (struct name unknown; layout from its constructor, 0x80194C1C; as in
 * ThrowBomb.cpp). */
struct UnkEffectParam_80194C1C {
    UnkEffectParam_80194C1C(const Vec3f &position, const Vec3f &velocity, float time, bool flag);

    /* 0x00 */ Vec3f m_position;
    /* 0x0C */ Vec3f m_velocity;
    /* 0x18 */ float m_scale;
    /* 0x1C */ float field_0x1C;
    /* 0x20 */ u8 m_flag;
};

class UnkCameraManager_80009548;

extern "C" {
/* The enemies' shared helpers (behaviour inferred). */
void fn_8019AA40(Enemy *enemy, u16 sound, int arg);
UnkSoundHandle_801D3DC8 fn_8019A908(Enemy *enemy, u16 sound, int arg);
void fn_8017CA2C(Enemy *enemy);
/* A point dist ahead of enemy toward position (behaviour inferred; see GunBigfootAI.cpp). */
Vec3f fn_8019AADC(Enemy *enemy, const Vec3f *position, float dist);
/* The enemy's velocity this frame (behaviour inferred). */
const Vec3f *fn_80181F0C(Enemy *enemy);
/* Whether key is in the area (behaviour inferred; another test than Contains). */
u8 fn_801A6944(UnkEnemyArea_8018A374 *area, void *key);
/* Whether the game runs at its hard difficulty (behaviour inferred; see EggPawn.cpp). */
bool fn_801A2030(void);
/* Resources and models (see BkSoldier.cpp). */
void *fn_80012684(void *loader, int slot, int kind);
void *fn_80012608(void *loader, int slot, int kind);
boost::shared_ptr<EnemyRender> fn_802CDB08(int slot, const EnemyMotionTable &table, void *model,
                                           void *motion);
void fn_802CD614(EnemyRender *render);
void *fn_801A46E4(void *model, int part);
/* The model: loading it by id, showing and hiding it, its update and a frame by id (behaviours
 * inferred; see ThrowBomb.cpp). */
void fn_801A3CA4(UnkModel_801A3E9C *model, int id, int arg2, int arg3);
void fn_801A3AFC(UnkModel_801A3E9C *model);
void fn_801A3AA0(UnkModel_801A3E9C *model);
void fn_801A39E0(UnkModel_801A3E9C *model);
RwFrame *fn_801A3704(UnkModel_801A3E9C *model, int id);
RwMatrix *fn_8047EBA0(RwMatrix *matrix);
RwFrame *fn_804876E8(RwFrame *frame, const RwMatrix *matrix, int combine);
RwFrame *fn_8048764C(RwFrame *frame, const RwV3d *scale, int combine);
RwFrame *fn_8048722C(RwFrame *frame);
/* The flag word's clear (see ThrowBomb.cpp). */
void fn_80014E10(u32 *flags);
/* The collider manager, a sphere collider at a position, its release and its attack collision
 * (behaviours inferred; see BlackBullCtrlUnit.cpp and ThrowBomb.cpp). */
void *fn_80021B08(void);
/* The radius is declared ahead of the owner: retail evaluates it between the owner and the
 * position (the registers are the same either way). */
boost::shared_ptr<CharaColliBase> fn_800C7EA8(void *manager, const Vec3f *position, u32 flags,
                                              int arg3, int arg4, float radius,
                                              const boost::weak_ptr<CharInfo> &owner,
                                              const Vec3f *offset);
void fn_800C78B0(void *manager, boost::shared_ptr<CharaColliBase> *collider);
boost::weak_ptr<CharaColliAttack> fn_800C4374(void *manager,
                                              boost::shared_ptr<CharaColliBase> collider);
/* Sets the attack collision's callbacks (see BKTank.cpp). */
void fn_80054DD0(CharaColliAttack *attack, int arg1, int arg2, float arg3, const AttackCallback &onHit,
                 const AttackCallback &onEnd);
void fn_80096B64(CharaColliAttack *attack, int arg1, const AttackCallback &callback);
/* Targets (see BKTank.cpp). */
boost::shared_ptr<Target> fn_801E7128(const boost::weak_ptr<CharInfo> &owner, int flags);
/* An explosion at a position (behaviour inferred; see ThrowBomb.cpp). */
void fn_801DD0BC(const Vec3f *position, int kind, int arg2, float power);
/* The camera manager and a camera shake, and a rumble (see BkWorm.cpp). */
UnkCameraManager_80009548 *fn_80009548(void);
void fn_8000D8EC(UnkCameraManager_80009548 *manager, bool arg1, int arg2, float strength);
boost::weak_ptr<Vibration::RequestHandle> fn_8034DE08(int pattern, const Vec3f *position,
                                                      int arg);
/* A sound at a position (behaviour inferred; see ThrowBomb.cpp). */
void *fn_80053164(void);
void fn_801D54F4(void *manager, int id, const Vec3f *position, int arg3, int arg4);
/* Whether a float is zero (behaviour inferred). */
bool fn_8000A950(float value);
/* Vector routines: a + b and a - b, v * scale, the cross product, the dot product, the length and
 * the squared length, v turned about the y and x axes (behaviours inferred). */
Vec3f fn_8040CDD0(const Vec3f *a, const Vec3f *b);
Vec3f fn_8040CDA8(const Vec3f *v, float scale);
Vec3f fn_8040CFB8(const Vec3f *a, const Vec3f *b);
float fn_8040CF90(const Vec3f *a, const Vec3f *b);
float fn_8040CF50(const Vec3f *v);
float fn_8040CF2C(const Vec3f *v);
Vec3f fn_8040D0AC(const Vec3f *v, float angle);
Vec3f fn_8040D118(const Vec3f *v, float angle);
void fn_8040CFF8(Vec3f *v, const Vec3f *value);
void fn_8040CE04(Vec3f *v, const Vec3f *value);
void fn_8040CE38(Vec3f *v, float scale);
void fn_8040CE60(Vec3f *vector);
/* The translation of a matrix (behaviour inferred). */
Vec3f fn_800496F8(const Mtx34f *matrix);
/* A matrix's translation and its rotations about y, x and z (see ThrowBomb.cpp). */
void fn_8040B59C(Mtx34f *matrix, const Vec3f *position);
void fn_8040B848(Mtx34f *matrix, float angle);
void fn_8040B884(Mtx34f *matrix, float angle);
void fn_8040B80C(Mtx34f *matrix, float angle);
/* The quaternion of Euler angles in the axis order order, and back (see ThrowBomb.cpp). */
Quatf fn_804102F4(const Vec3f *angles, int order);
Vec3f fn_80410204(const Quatf *q, int order);
}

/* The global the stage id comes from (Game/Enemy.h declares it as bytes). */
#define GAME_STATE (reinterpret_cast<UnkGameState_8057D6A0 *>(lbl_8057D6A0))

/* fn_8040CD74 called as returning a - b by value (see ArkCannon.cpp). */
typedef Vec3f (*VectorSubtractFn)(const Vec3f *a, const Vec3f *b);

/* *v += value (name inferred; as in GunBigfootAI.cpp). */
inline void AddVector(Vec3f *v, const Vec3f &value) {
    fn_8040CFF8(v, &value);
}

/* *v -= value (name inferred). */
inline void SubtractVector(Vec3f *v, const Vec3f &value) {
    fn_8040CE04(v, &value);
}

/* The vector type's out-of-line assignment (fn_800091D0), called with temporaries the way the
 * original's operator is (name inferred; as in ThrowBomb.cpp). */
inline void AssignVector(Vec3f &vector, const Vec3f &value) {
    fn_800091D0(&vector, &value);
}

/* Copies a rotation one angle at a time (name inferred; as in ThrowBomb.cpp). */
inline void CopyRotation(Vec3f &rotation, const Vec3f &value) {
    rotation.x = value.x;
    rotation.y = value.y;
    rotation.z = value.z;
}

/* A motion table built in place (not original; as in BkWorm.cpp). */
struct MotionTable : public EnemyMotionTable {
    MotionTable(const EnemyMotionEntry *e, s32 c) {
        entries = e;
        count = c;
    }
};

/* Three floats copied as a plain struct (not original; as in BkGiant.cpp). */
struct Vec3fWords {
    float x;
    float y;
    float z;
};

namespace EggPierrot {

/* EggPierrot::PierrotBall (class name from the RTTI of its shared_ptr control block): the ball the
 * clown balances on. It has its own collider, which takes the hits: each lowers its health
 * (m_hp) and the ball rolls slower, until it bursts (names inferred). */
class PierrotBall {
  public:
    PierrotBall(EggPierrot *owner);
    ~PierrotBall();

    void OnHitEnd(AttackCallbackParam &param);
    void Tick();
    void Burst();
    void Hide();
    void Show();
    void UpdateSpeed();
    void OnHit(AttackCallbackParam &param);
    void OnAttacked(AttackCallbackParam &param);
    void UpdateMatrix(const Vec3f &position, const Vec3f &rotation);
    void SetMatrix(const Mtx34f &matrix);

    /* Out of line (retail has it between Burst and Hide). */
    float GetSpeed();
    /* The size of the ball: its speed (name inferred). Inline: the collider's creation reads it
     * as an inline call, in its place among the other calls of the arguments. */
    float GetRadius() {
        return m_speed;
    }

    /* 0x00 */ EggPierrot *m_owner;
    /* 0x04 */ Vec3f m_rotation;
    /* 0x10 */ boost::shared_ptr<Target> m_target;
    /* 0x18 */ boost::shared_ptr<CharaColliBase> m_collider;
    /* 0x20 */ UnkModel_801A3E9C m_model;
    /* 0x28 */ float m_hp;
    /* 0x2C */ float m_speed; /* also the ball's size */
    /* 0x30 */ u8 field_0x30[0x34 - 0x30];
    /* 0x34 */ s32 m_state;
    /* 0x38 */ float m_slowdown; /* how fast the speed follows the health */
    /* 0x3C */ UnkSoundHandle_801D3DC8 m_sound;
    /* 0x58 */ EnemyFlags m_flags;
};
SIZE_ASSERT(PierrotBall, 0x5C);

/* EggPierrot::BakudanManager (class name from the RTTI of its shared_ptr control block): the three
 * bombs the clown juggles, bakudan being Japanese for bomb (names inferred). */
class BakudanManager {
  public:
    BakudanManager(EggPierrot *owner);
    ~BakudanManager();

    void Tick();
    void Reset();
    void RemoveAll();
    void FadeIn();
    void FadeOut();
    void Attach();

    /* 0 fade in, 1 juggle, 2 throw, 3 fade out (inferred). */
    void SetState(int state) {
        m_state = state;
    }

    /* 0x00 */ EggPierrot *m_owner;
    /* 0x04 */ boost::weak_ptr<ThrowBomb> m_bombs[3];
    /* 0x1C */ s32 m_state;
    /* 0x20 */ UnkSoundHandle_801D3DC8 m_sound;
};
SIZE_ASSERT(BakudanManager, 0x3C);

/* EggPierrot::EggPierrotTask (class name from RTTI; vtable 0x80550D0C): the task that runs a
 * clown. */
class EggPierrotTask : public EnemyTemplate<EggPierrot, EggPierrotAI> {
  public:
    EggPierrotTask(Task *parent, UnkPlacement_800C9CD0 *placement);
    virtual ~EggPierrotTask();

    /* The stage editor's hooks (names inferred). */
    static void Instantiate(UnkPlacement_800C9CD0 *placement);
    static void LoadFiles();
    static void DropFiles();

    static const char *s_taskName;
};
SIZE_ASSERT(EggPierrotTask, 0x390);

/* Deletes a ball and a bomb set (names inferred; see checked_delete below). */
inline void DeleteBall(PierrotBall *ball) {
    delete ball;
}

inline void DeleteBakudan(BakudanManager *bakudan) {
    delete bakudan;
}

} // namespace EggPierrot

/* Retail keeps boost::checked_delete<...> out of line and the destructors at their own place; see
 * Rocket.cpp and docs/research-notes.md. boost/weak_ptr.hpp only declares weak_ptr::reset, which
 * the game has out of line; this unit emits the copy for the bombs (boost 1.32's definition; as in
 * Game/EggPawn.cpp). */
namespace boost {
template <> inline void checked_delete<EggPierrot::PierrotBall>(EggPierrot::PierrotBall *x) {
    EggPierrot::DeleteBall(x);
}
template <> inline void checked_delete<EggPierrot::BakudanManager>(EggPierrot::BakudanManager *x) {
    EggPierrot::DeleteBakudan(x);
}
template <> inline void shared_ptr<EggPierrot::PierrotBall>::reset() {
    this_type().swap(*this);
}
template <> inline void shared_ptr<EggPierrot::BakudanManager>::reset() {
    this_type().swap(*this);
}
template <> inline void weak_ptr<ThrowBomb>::reset() {
    this_type().swap(*this);
}
} // namespace boost

/* The model frames the bombs follow (.data). */
static int s_bombParts[3] = {3000, 4000, 5000};

/* Which data file the stage loaded: 0 pierro, 1 horror, 2 busou (-1 none). */
static int s_type = -1;

/* The motion table (steps in .sdata2 and .rodata). */
static const MotionSequenceStep lbl_805F83E8[] = {{1, -1, -1, 10, 1}};
static const MotionSequenceStep lbl_805F83F0[] = {{6, -1, -1, 5, 1}};
static const MotionSequenceStep lbl_805F83F8[] = {{2, -1, -1, 10, 1}};
static const MotionSequenceStep lbl_804F8E70[] = {{3, -1, -1, 30, 2}, {5, -1, -1, 10, 0}};
static const MotionSequenceStep lbl_805F8400[] = {{4, -1, -1, 10, 0}};
static const MotionSequenceStep lbl_805F8408[] = {{7, -1, -1, 10, 0}};
static const MotionSequenceStep lbl_805F8410[] = {{4, -1, -1, 0, 0}};
static const MotionSequence lbl_804F8E80[] = {
    {lbl_805F83E8, 1, -3}, {lbl_805F83F0, 1, -3}, {lbl_805F83F8, 1, -3}, {lbl_804F8E70, 2, -3},
    {lbl_805F8400, 1, -3}, {lbl_805F8408, 1, -3}, {lbl_805F8410, 1, -3},
};
static const MotionSequenceTable lbl_805F8418 = {lbl_804F8E80, 7};

/* The ball's rolling sounds and the clown's voices, by data file. */
static const u16 s_rollSounds[3] = {0x6048, 0x6049, 0x604A};

/* The model's motion table. */
static const EnemyMotionEntry lbl_805F8428[] = {{4, -1}};

static const u16 s_voices[3] = {0x6056, 0x6057, 0x6058};

namespace EggPierrot {

BakudanManager::BakudanManager(EggPierrot *owner) : m_owner(owner) {
    m_state = 0;
}

BakudanManager::~BakudanManager() {
    m_sound.fn_801D455C();
    m_owner = 0;
    RemoveAll();
}

void BakudanManager::Tick() {
    Attach();
    switch (m_state) {
    case 0:
        FadeIn();
        m_state = 1;
        break;
    case 1:
        FadeIn();
        break;
    case 2:
        if (boost::shared_ptr<ThrowBomb> bomb = m_bombs[2].lock()) {
            void *model = m_owner->vfDisp_0x1C();
            if (model != 0 && fn_801A46E4(model, s_bombParts[2]) != 0) {
                Vec3f position;
                fn_800091EC(&position, &(const Vec3f &)fn_800496F8(
                                           static_cast<Mtx34f *>(fn_801A46E4(model, s_bombParts[2]))));
                Vec3f toTarget;
                fn_800091EC(&toTarget, &(const Vec3f &)((VectorSubtractFn)fn_8040CD74)(
                                           static_cast<Vec3f *>(m_owner->m_ctrl->vfCtrl_0x10()),
                                           &position));
                if (fn_8040CF90(&toTarget, &(const Vec3f &)fn_8040D0AC(
                                                &(const Vec3f &)Vec3f(0.0f, 0.0f, -1.0f),
                                                m_owner->GetRotation()->y)) > 0.5f) {
                    float distance = fn_8040CF50(&toTarget);
                    if (distance < 100.0f) {
                        distance = 100.0f;
                    }
                    Vec3f velocity(0.0f, distance, -distance);
                    float yaw = m_owner->GetRotation()->y;
                    float spread = fn_8040EC80(-10.0f, 10.0f);
                    spread *= lbl_805FAD38 / 180.0f;
                    AssignVector(velocity, fn_8040D0AC(&velocity, yaw + spread));
                    float pitch = fn_8040EC80(-5.0f, 5.0f);
                    pitch *= lbl_805FAD38 / 180.0f;
                    AssignVector(velocity, fn_8040D118(&velocity, pitch));
                    UnkEffectParam_80194C1C param(position, velocity, m_owner->m_params.BakudanTime,
                                                  false);
                    bomb->Throw(param);
                    m_bombs[2].reset();
                    fn_801D54F4(fn_80053164(), 0x6037, &position, 0, 1);
                }
            }
        }
        m_state = 0;
        break;
    case 3:
        FadeOut();
        break;
    case 4:
        break;
    }
}

void BakudanManager::Reset() {
    for (int i = 0; i < 3; i++) {
        boost::weak_ptr<ThrowBomb> *slot = &m_bombs[i];
        if (slot->expired()) {
            *slot = fn_80209908(fn_801EB304(fn_8001C4F8(), 0xE), 1);
            if (boost::shared_ptr<ThrowBomb> bomb = slot->lock()) {
                bomb->SetAlpha(0.0f);
            }
        }
    }
    Attach();
}

void BakudanManager::RemoveAll() {
    for (int i = 0; i < 3; i++) {
        if (boost::shared_ptr<ThrowBomb> bomb = m_bombs[i].lock()) {
            bomb->Remove();
            m_bombs[i].reset();
        }
    }
    if (m_sound.m_id != 0) {
        m_sound.fn_801D455C();
    }
}

void BakudanManager::FadeIn() {
    for (int i = 0; i < 3; i++) {
        if (boost::shared_ptr<ThrowBomb> bomb = m_bombs[i].lock()) {
            float alpha = bomb->GetAlpha() + m_owner->GetFrameTime();
            if (alpha > 1.0f) {
                alpha = 1.0f;
            }
            bomb->SetAlpha(alpha);
        } else {
            m_bombs[i] = fn_80209908(fn_801EB304(fn_8001C4F8(), 0xE), 1);
            if (boost::shared_ptr<ThrowBomb> created = m_bombs[i].lock()) {
                created->SetAlpha(0.0f);
            }
        }
    }
    if (m_sound.m_id == 0) {
        m_sound = fn_8019A908(m_owner, 0x6047, 1);
    }
}

void BakudanManager::FadeOut() {
    for (int i = 0; i < 3; i++) {
        if (boost::shared_ptr<ThrowBomb> bomb = m_bombs[i].lock()) {
            float alpha = bomb->GetAlpha() - m_owner->GetFrameTime();
            if (alpha < 0.0f) {
                bomb->Remove();
                m_bombs[i].reset();
            } else {
                bomb->SetAlpha(alpha);
            }
        }
    }
    if (m_sound.m_id != 0) {
        m_sound.fn_801D455C();
    }
}

void BakudanManager::Attach() {
    for (int i = 0; i < 3; i++) {
        if (boost::shared_ptr<ThrowBomb> bomb = m_bombs[i].lock()) {
            void *model = m_owner->vfDisp_0x1C();
            if (model != 0 && fn_801A46E4(model, s_bombParts[i]) != 0) {
                bomb->SetMatrix(*static_cast<Mtx34f *>(fn_801A46E4(model, s_bombParts[i])));
            }
        }
    }
}

void PierrotBall::OnHitEnd(AttackCallbackParam &param) {}

PierrotBall::PierrotBall(EggPierrot *owner)
    : m_owner(owner), m_rotation(0.0f, 0.0f, 0.0f), m_speed(12.0f), m_slowdown(1.0f) {
    m_state = 0;
    m_hp = 6.0f;
    fn_80014E10(&m_flags.m_bits);
    fn_8040E970(&m_rotation);
    m_collider = fn_800C7EA8(fn_80021B08(), m_owner->GetPosition(), lbl_805F56D0, 4, 0, GetRadius(),
                             fn_8017D014(m_owner), &lbl_805E07A0);
    if (m_collider.get() != 0) {
        boost::weak_ptr<CharaColliAttack> attack = fn_800C4374(fn_80021B08(), m_collider);
        if (boost::shared_ptr<CharaColliAttack> collision = attack.lock()) {
            fn_80054DD0(collision.get(), 1, 1, 1.0f,
                        AttackCallback(boost::bind(&PierrotBall::OnHit, this, _1)),
                        AttackCallback(boost::bind(&PierrotBall::OnHitEnd, this, _1)));
            fn_80096B64(collision.get(), 1,
                        AttackCallback(boost::bind(&PierrotBall::OnAttacked, this, _1)));
        }
    }
    fn_801A3CA4(&m_model, m_owner->m_motionBase, 2, 0);
    UpdateMatrix(*m_owner->GetPosition(), *m_owner->GetRotation());
    m_target = fn_801E7128(fn_8017D014(m_owner), 0x40800);
}

PierrotBall::~PierrotBall() {
    if (m_collider.get() != 0) {
        fn_800C78B0(fn_80021B08(), &m_collider);
    }
    m_sound.fn_801D455C();
}

void PierrotBall::Tick() {
    UpdateSpeed();
    Vec3f position;
    fn_800091EC(&position, &(const Vec3f &)fn_8040CDD0(m_owner->GetPosition(),
                                                      &(const Vec3f &)Vec3f(0.0f, m_speed, 0.0f)));
    switch (m_state) {
    case 0:
        m_target->TurnOn();
    case 1: {
        Vec3f velocity;
        fn_800091EC(&velocity, fn_80181F0C(m_owner));
        if (!fn_8000A950(m_owner->GetFrameTime())) {
            fn_8040CE38(&velocity, 1.0f / m_owner->GetFrameTime());
            if (fn_8040CF2C(&velocity) > 1.0f) {
                if (m_hp <= 2.0f) {
                    if (m_sound.m_id == 0 || m_sound.field_0x4 != 0x604B) {
                        m_sound.fn_801D455C();
                        m_sound = fn_8019A908(m_owner, 0x604B, 1);
                    }
                } else if (m_sound.m_id == 0) {
                    m_sound = fn_8019A908(m_owner, s_rollSounds[s_type], 1);
                }
            } else if (m_sound.m_id != 0) {
                m_sound.fn_801D455C();
            }
        }
        Vec3f spin;
        fn_800091EC(&spin, &lbl_805E07A0);
        float size = m_speed;
        if (size > 0.0f) {
            fn_800091D0(&spin, &(const Vec3f &)fn_8040CDA8(
                                   &(const Vec3f &)fn_8040CDA8(
                                       &(const Vec3f &)fn_8040CFB8(
                                           &(const Vec3f &)Vec3f(0.0f, 1.0f, 0.0f), &velocity),
                                       1.5f),
                                   1.0f / size));
        }
        Quatf rotation(fn_804102F4(&m_rotation, 2));
        Quatf spinQ(spin.x, spin.y, spin.z, 0.0f);
        float dt = m_owner->GetFrameTime();
        fn_8040CAB4(&rotation, &(const Quatf &)(spinQ * rotation * 0.5f * dt));
        fn_8040C958(&rotation);
        CopyRotation(m_rotation, fn_80410204(&rotation, 2));
        UpdateMatrix(position, m_rotation);
        break;
    }
    }
    if (m_collider.get() != 0) {
        m_collider->SetCharInfo(reinterpret_cast<CharInfo *>(&position));
        m_collider->SetRotation(&m_rotation);
        m_collider->SetSize(m_speed, 0.0f, 0.0f, -2);
    }
    if (m_target.get() != 0) {
        m_target->fn_800CCE38(&position);
    }
    const boost::shared_ptr<EnemyRender> &render = m_owner->vfDisp_0x20();
    fn_802CD614(render.get());
    fn_801A39E0(&m_model);
}

/* The ternary makes the compiler compute the shake's flag before it calls fn_80009548, as retail
 * does; pattern is declared first for retail's register choice. */
void PierrotBall::Burst() {
    int pattern;
    int kind = 2;
    if (m_hp < 2.0f) {
        kind = 0;
    } else if (m_hp < 5.0f) {
        kind = 1;
    }
    fn_801DD0BC(&(const Vec3f &)fn_8040CDD0(m_owner->GetPosition(),
                                           &(const Vec3f &)Vec3f(0.0f, m_speed, 0.0f)),
                kind, 3, 1.0f);
    fn_8000D8EC(fn_80009548(), kind == 2 ? true : false, 0, 0.5f);
    pattern = 8;
    if (kind == 2) {
        pattern = 7;
    }
    fn_8034DE08(pattern, m_owner->GetPosition(), -2);
    fn_8019AA40(m_owner, 0x604F, 1);
}

float PierrotBall::GetSpeed() {
    return m_speed;
}

void PierrotBall::Hide() {
    fn_801A3AA0(&m_model);
    m_target->TurnOff();
    m_collider->TurnOff(-1);
}

void PierrotBall::Show() {
    fn_801A3AFC(&m_model);
    m_target->TurnOn();
    m_collider->TurnOn(-1);
}

void PierrotBall::UpdateSpeed() {
    float target;
    if (m_hp <= 0.0f) {
        target = 0.0f;
    } else if (m_hp <= 2.0f) {
        target = 4.0f;
    } else {
        target = 12.0f * (m_hp / 6.0f);
    }
    if (m_speed != target) {
        float step = 8.0f * m_slowdown * m_owner->GetFrameTime();
        float speed;
        if (target > m_speed) {
            speed = m_speed + step;
            if (speed > target) {
                speed = target;
            }
        } else if (target < m_speed) {
            speed = m_speed - step;
            if (speed < target) {
                speed = target;
            }
        } else {
            speed = target;
        }
        m_speed = speed;
    }
}

void PierrotBall::OnHit(AttackCallbackParam &param) {
    boost::shared_ptr<CharInfo> attacker = param.m_attacker.lock()->m_owner.lock();
    if (attacker.get() != 0) {
        fn_800091D0(reinterpret_cast<Vec3f *>(&param.field_0x8[0x34 - 0x8]),
                    &(const Vec3f &)((VectorSubtractFn)fn_8040CD74)(
                        reinterpret_cast<Vec3f *>(&param.field_0x8[0x18 - 0x8]),
                        &attacker->m_position));
    } else {
        fn_800091D0(reinterpret_cast<Vec3f *>(&param.field_0x8[0x34 - 0x8]), &lbl_805E07A0);
    }
    if (m_owner->vfStat_0x44()) {
        boost::shared_ptr<CharInfo> owner =
            reinterpret_cast<boost::weak_ptr<CharaColliAttack> *>(&param.field_0x8[0])
                ->lock()
                ->m_owner.lock();
        if (owner.get() != 0 && owner->field_0x5C == 1) {
            param.field_0x8[0x30 - 0x8] = 1;
        }
    }
}

void PierrotBall::OnAttacked(AttackCallbackParam &param) {
    m_flags.Set(0);
    param.m_result = 1;
    m_slowdown += 0.5f;
    if ((m_hp -= param.m_damage) < 0.0f) {
        m_flags.Set(1);
        m_hp = 0.0f;
    }
}

void PierrotBall::UpdateMatrix(const Vec3f &position, const Vec3f &rotation) {
    Mtx34f matrix;

    fn_8040C148(&matrix, &lbl_805E0720);
    fn_8040B59C(&matrix, &position);
    fn_8040B848(&matrix, rotation.y);
    fn_8040B884(&matrix, rotation.x);
    fn_8040B80C(&matrix, rotation.z);
    SetMatrix(matrix);
}

void PierrotBall::SetMatrix(const Mtx34f &matrix) {
    RwFrame *frame = fn_801A3704(&m_model, 3000);
    if (frame != 0) {
        RwMatrix rwMatrix;
        rwMatrix.right.x = matrix.values[0][0];
        rwMatrix.right.y = matrix.values[1][0];
        rwMatrix.right.z = matrix.values[2][0];
        rwMatrix.up.x = matrix.values[0][1];
        rwMatrix.up.y = matrix.values[1][1];
        rwMatrix.up.z = matrix.values[2][1];
        rwMatrix.at.x = matrix.values[0][2];
        rwMatrix.at.y = matrix.values[1][2];
        rwMatrix.at.z = matrix.values[2][2];
        rwMatrix.pos.x = matrix.values[0][3];
        rwMatrix.pos.y = matrix.values[1][3];
        rwMatrix.pos.z = matrix.values[2][3];
        rwMatrix.pad3 = 0;
        rwMatrix.pad2 = 0;
        rwMatrix.pad1 = 0;
        rwMatrix.flags = 0;
        fn_8047EBA0(&rwMatrix);
        fn_804876E8(frame, &rwMatrix, 0);
        float scale = m_speed / 12.0f;
        RwV3d scales = {scale, scale, scale};
        fn_8048764C(frame, &scales, 1);
        fn_8048722C(frame);
    }
}

const char *EggPierrotTask::s_taskName = "EggPierrot";

EggPierrot::EggPierrot() : EnemyWalkerCommon(0xD) {
    m_enemy = this;
    vfPara_0x40(reinterpret_cast<int>(&lbl_805F8418));
    m_targetInArea = 0;
    m_throwReady = 0;
}

EggPierrot::~EggPierrot() {
    if (m_bakudan.get() != 0) {
        m_bakudan.reset();
    }
}

void EggPierrot::vfSet_0x1C(UnkPlacement_800C9CD0 *placement) {
    vfSet_0x20(placement);
    /* Copied as a plain struct (word loads and stores), unlike Vec3f's own copy. */
    Vec3fWords rotation = *reinterpret_cast<Vec3fWords *>(GetRotation());
    vfDisp_0x38(GetPosition(), reinterpret_cast<Vec3f *>(&rotation));
    m_ctrl->vfCtrl_0x20();
}

void EggPierrot::vfSet_0x20(UnkPlacement_800C9CD0 *placement) {
    TEnemySetBase::vfSet_0x20(placement);
    fn_800C9DBC(placement, &m_params, sizeof(m_params));
    if (m_params.AppearType == 1) {
        GetPosition()->x += m_params.OffsetPos_X;
        GetPosition()->y += m_params.OffsetPos_Y;
        GetPosition()->z += m_params.OffsetPos_Z;
    }
}

void EggPierrot::vfStat_0x3C(bool value) {
    field_0x6D = value;
    if (value) {
        if (m_ball.get() != 0) {
            m_ball->Hide();
        }
        if (m_bakudan.get() != 0) {
            m_bakudan->RemoveAll();
        }
    } else {
        if (m_ball.get() != 0) {
            m_ball->Show();
        }
        if (m_bakudan.get() != 0) {
            m_bakudan->Reset();
        }
    }
}

void EggPierrot::vfStat_0x38() {
    if (m_params.AppearType == 1 && !field_0xA8.Contains(m_ctrl->vfCtrl_0x10())) {
        m_status->vfStat_0x3C(true);
        m_status->vfStat_0x30(false);
    } else {
        m_status->vfStat_0x3C(false);
        m_status->vfStat_0x30(true);
    }
}

void EggPierrot::vfDisp_0x64() {
    void *loader = fn_800122D4();
    int slot = m_motionBase;
    void *model = fn_80012684(loader, slot, 7);
    void *motion = fn_80012608(loader, slot, 8);
    m_render = fn_802CDB08(slot, MotionTable(lbl_805F8428, 1), model, motion);
}

void EggPierrot::vfDisp_0x08() {
    EnemyDispCommon::vfDisp_0x08();
    m_ball = boost::shared_ptr<PierrotBall>(new PierrotBall(this));
    m_bakudan = boost::shared_ptr<BakudanManager>(new BakudanManager(this));
    m_bakudan->Reset();
    fn_8040CE60(&field_0x2B8);
    AssignVector(m_home, fn_8019AADC(this, &lbl_805E07A0, 1.0f));
    fn_8040CE60(&field_0x2D0);
    field_0x2DC = 0.0f;
}

void EggPierrot::vfStat_0x10() {
    EnemyStatusCommon::vfStat_0x10();
    if (m_ball.get() == 0 && m_target.get() != 0) {
        if (m_target->GetTarget(0).get() != 0) {
            if (!((m_target->GetTarget(0)->m_flags >> 11) & 1)) {
                m_target->GetTarget(0)->SetFlags(m_target->GetTarget(0)->GetFlags() | 0x800);
            }
        }
    }
}

} // namespace EggPierrot

/* Slot 0x0C, the update of the model after the motion: it takes the frame time, which
 * Game/Enemy.h does not declare, so it is defined under its symbol (as in EnemyDispCommon.cpp). */
extern "C" void vfDisp_0x0C__15EnemyDispCommonFv(EnemyDispCommon &disp, float dt);

extern "C" void vfDisp_0x0C__Q210EggPierrot10EggPierrotFv(EggPierrot::EggPierrot *self, float dt) {
    if (self->m_ball.get() != 0) {
        Vec3f lift;
        AddVector(self->GetPosition(),
                  (fn_80008E1C(&lift, 0.0f, 2.0f * self->m_ball->GetSpeed(), 0.0f), lift));
    }
    vfDisp_0x0C__15EnemyDispCommonFv(*self, dt);
    if (self->m_ball.get() != 0) {
        Vec3f lift;
        SubtractVector(self->GetPosition(),
                       (fn_80008E1C(&lift, 0.0f, 2.0f * self->m_ball->GetSpeed(), 0.0f), lift));
    }
}

namespace EggPierrot {

void EggPierrot::vfDisp_0x10(float dt) {
    Vec3f position;
    fn_800091EC(&position, GetPosition());
    if (m_ball.get() != 0) {
        Vec3f lift;
        AddVector(GetPosition(), (fn_80008E1C(&lift, 0.0f, 2.0f * m_ball->GetSpeed(), 0.0f), lift));
    }
    EnemyWalkerCommon::vfDisp_0x10(dt);
    fn_800091D0(GetPosition(), &position);
    if (fn_801A6944(&field_0xA8, m_ctrl->vfCtrl_0x10())) {
        m_targetInArea = 1;
    } else {
        m_targetInArea = 0;
    }
    if (m_bakudan.get() != 0) {
        m_bakudan->Tick();
    }
    if (m_ball.get() != 0) {
        m_ball->Tick();
        if (m_ball->m_speed <= 0.1f) {
            m_ball.reset();
        }
    }
}

void EggPierrot::vfDisp_0x40(EnemyMotionEventArg &arg) {
    EnemyDispCommon::vfDisp_0x40(arg);
    if (arg.m_type == 0x5000 && arg.m_data->kind == 2 && m_throwReady) {
        m_bakudan->SetState(2);
    }
}

bool EggPierrot::vfDisp_0x44(int *command) {
    if (EnemyDispCommon::vfDisp_0x44(command)) {
        return true;
    }
    switch (*command) {
    case 12:
        fn_8034DE08(10, GetPosition(), -2);
        if (s_type != -1) {
            fn_8019AA40(this, s_voices[s_type], 1);
        }
        return true;
    case 11:
        if (m_ball.get() == 0) {
            fn_8019AA40(this, 0x605C, 1);
            fn_8017CA2C(this);
        }
        return true;
    }
    return true;
}

float EggPierrot::vfPara_0x6C() {
    return 4.0f;
}

int EggPierrot::vfPara_0x48() {
    return vfPara_0x54();
}

int EggPierrot::vfPara_0x4C() {
    return 1;
}

int EggPierrot::vfPara_0x50() {
    return 3;
}

int EggPierrot::vfPara_0x54() {
    return IsBallWeak() ? 4 : 0;
}

int EggPierrot::vfPara_0x58() {
    return m_targetInArea ? 2 : 0;
}

int EggPierrot::vfPara_0x5C() {
    return 5;
}

int EggPierrot::vfPara_0x60() {
    return 6;
}

void EggPierrot::vfStat_0x70(AttackCallbackParam &param) {
    EnemyStatusCommon::vfStat_0x70(param);
}

int EggPierrot::vfStat_0xB8() {
    return fn_801A2030() ? 0x6C : 0x66;
}

void EggPierrot::vfStat_0xA0() {
    EnemyStatusCommon::vfStat_0xA0();
    StopBakudan();
    if (m_ball.get() != 0) {
        m_ball->Burst();
        m_ball.reset();
    }
}

void EggPierrot::StopBakudan() {
    m_bakudan->SetState(3);
}

void EggPierrot::StartBakudan() {
    m_bakudan->SetState(0);
}

bool EggPierrot::IsBallWeak() {
    if (m_ball.get() != 0 && m_ball->m_hp <= 2.0f) {
        return true;
    }
    return false;
}

EggPierrotTask::EggPierrotTask(Task *parent, UnkPlacement_800C9CD0 *placement)
    : EnemyTemplate<EggPierrot, EggPierrotAI>(parent, placement) {
    m_enemy = this;
    m_name = s_taskName;
}

EggPierrotTask::~EggPierrotTask() {}

} // namespace EggPierrot

extern "C" const char *fn_80289928(UnkLabelQuery_800CBC20 *query) {
    static const char *s_appearTypes[2] = {"AppearType : WANDER", "AppearType : OFFSET"};
    static int s_appearType = 0;

    switch (query->index) {
    case 7:
        s_appearType = query->value;
        return s_appearTypes[query->value];
    case 10:
    case 11:
    case 12:
        if (s_appearType == 1) {
            return query->params[query->index].label;
        }
        break;
    }
    return "";
}

namespace EggPierrot {

void EggPierrotTask::LoadFiles() {
    void *loader = fn_800122D4();
    switch (GAME_STATE->m_stageId) {
    case 300:
    case 301:
        s_type = 1;
        fn_8001299C(loader, 0x14, new ResourceOneFile("enemy/EggPierrotData_horror.one", false));
        break;
    case 501:
    case 602:
        s_type = 2;
        fn_8001299C(loader, 0x14, new ResourceOneFile("enemy/EggPierrotData_busou.one", false));
        break;
    default:
        s_type = 0;
        fn_8001299C(loader, 0x14, new ResourceOneFile("enemy/EggPierrotData_pierro.one", false));
        break;
    }
}

void EggPierrotTask::DropFiles() {
    s_type = -1;
    fn_800127F8(fn_800122D4(), 0x14);
}

void EggPierrotTask::Instantiate(UnkPlacement_800C9CD0 *placement) {
    new EggPierrotTask(fn_801EB304(fn_8001C4F8(), 0xE), placement);
}

} // namespace EggPierrot

static GadgetParamDesc lbl_805505E0[] = {
    {4, "MoveRange", 0, 0, 0, 200.0f, 0.0f, 65536.0f, fn_800CBC20, 0},
    {4, "SearchRange", 0, 0, 0, 0.0f, 0.0f, 65536.0f, fn_800CBC20, 0},
    {4, "SearchAngle", 0, 0, 0, 0.0f, -360.0f, 360.0f, fn_800CBC20, 0},
    {4, "SearchWidth", 0, 0, 0, 100.0f, 0.0f, 65536.0f, fn_800CBC20, 0},
    {4, "SearchHeight", 0, 0, 0, 100.0f, 0.0f, 65536.0f, fn_800CBC20, 0},
    {4, "SearchHeightOffset", 0, 0, 0, 0.0f, -65536.0f, 65536.0f, fn_800CBC20, 0},
    {4, "MoveSpeedRatio", 0, 0, 0, 1.0f, 0.0f, 65536.0f, fn_800CBC20, 0},
    {1, "AppearType", 0, 0, 1, 0.0f, 0.0f, 0.0f, fn_80289928, 0},
    {1, "NOT_USE", 0, 0, 0, 0.0f, 0.0f, 0.0f, fn_800CBC20, 0},
    {4, "BakudanTime", 0, 0, 0, 5.0f, 0.0f, 65536.0f, fn_800CBC20, 0},
    {4, "OffsetPos_X", 0, 0, 0, 0.0f, -65536.0f, 65536.0f, fn_80289928, 0},
    {4, "OffsetPos_Y", 0, 0, 0, 0.0f, -65536.0f, 65536.0f, fn_80289928, 0},
    {4, "OffsetPos_Z", 0, 0, 0, 0.0f, -65536.0f, 65536.0f, fn_80289928, 0},
    {0},
};

GadgetClassDesc lbl_80550810 = {
    "EGG_PIERROT", EggPierrot::EggPierrotTask::LoadFiles, EggPierrot::EggPierrotTask::DropFiles,
    reinterpret_cast<void (*)(UnkPlacement_800C9CD0 *)>(EggPierrot::EggPierrotTask::Instantiate), 0,
    0, 0x78, 2, 0, 5, lbl_805505E0,
};

/* Not called anywhere: the linker strips it. Retail pools 0 and 0.5 ahead of the other float
 * constants in .sdata2; this function, emitted first, makes the compiler pool them there (as in
 * BkGiant.cpp; the name is a placeholder). */
static float UnkConstantUser(float x) {
    x *= 0.0f;
    x *= 0.5f;
    return x;
}

/* Not called anywhere: the linker strips it. Retail's .data, which the linker keeps whole, has the
 * zero-filled copy of sp_counted_base's vtable after EnemyTemplate's; this function, defined
 * last, makes the compiler place it there (as in GunRobot.cpp; the name is a placeholder). */
static void UnkCountedBaseUser(boost::detail::sp_counted_base *base) {
    base->boost::detail::sp_counted_base::~sp_counted_base();
}
