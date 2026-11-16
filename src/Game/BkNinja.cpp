/* BkNinja (namespace and class names from RTTI; its task's s_taskName is "BkNinja"): the Black
 * Arms ninja, BK_NINJA in the stage editor (enemy/BkNinjaData.one). Its body BkNinja::BkNinja is
 * an EnemyWalkerCommon (Game/Enemy.h) that throws its weapon (ShootCount shots at a time), fades
 * out and warps, between two points (Pos0, Pos1) or at random near its target, and may ride an
 * Air Saucer (AppearType 3); its AI is BkNinjaAI (Game/BkNinjaAI.cpp). File name inferred from
 * the class. Names marked "(name inferred)" are not original. Definitions are in reverse address
 * order (deferred inlining emits functions last-defined first). */
/* See EnemyReferer (Game/Enemy.h): keeps EnemyTemplate's destructor out of line, as in EggPawn.cpp. */
#define ENEMY_REFERER_DTOR

#include "Game/BkNinja.h"
#include "Game/CharaColliAttack.h"
#include "Game/EnemyRender.h"
#include "Game/EnemyWeapon.h"
#include "Game/Fn8040CopyVector3.h"
#include "Game/Gadget.h"
#include "Game/GadgetClassDesc.h"
#include "Game/MathTypes.h"
#include "Game/Random.h"
#include "Game/ResourceOneFile.h"
#include "Game/TVehicleBase.h"
#include "Effect/System/EffectElement.h"
#include "boost/shared_ptr.hpp"
#include "boost/weak_ptr.hpp"

using SonicteamUSA::System::CharaColli::AttackCallbackParam;
using Effect::System::EffectElement;

extern "C" double pow(double x, double y);

namespace BkNinja {

/* BkNinja::BkNinjaTask (class name from RTTI; vtable 0x8055503C): the task that runs a ninja. */
class BkNinjaTask : public EnemyTemplate<BkNinja, BkNinjaAI> {
  public:
    BkNinjaTask(Task *parent, UnkPlacement_800C9CD0 *placement);
    virtual ~BkNinjaTask();

    /* The stage editor's hooks (names inferred). */
    static void Instantiate(UnkPlacement_800C9CD0 *placement);
    static void LoadFiles();
    static void DropFiles();

    static const char *s_taskName;
};
SIZE_ASSERT(BkNinjaTask, 0x3D8);

} // namespace BkNinja

/* The cursor a parameter's label callback receives (layout as in Helicopter.cpp). */
class UnkLabelQuery_800CBC20 {
  public:
    /* 0x0 */ const GadgetParamDesc *params;
    /* 0x4 */ s32 index; /* the parameter being shown */
    /* 0x8 */ s32 value; /* its current value */
};

/* The table of vehicles (class name unknown; as in BkSoldier.cpp). */
class UnkVehicleSlot_801B48D4 {
  public:
    /* The vehicle of the slot (inline; weak copy 0x8018DF2C; name inferred). */
    boost::weak_ptr<Vehicle::TVehicleBase> GetVehicle() {
        return m_vehicle;
    }

    /* 0x00 */ u8 field_0x0[0x28];
    /* 0x28 */ boost::shared_ptr<Vehicle::TVehicleBase> m_vehicle;
};

/* A line between two points for the collision test, a collider it meets, and what it meets (see
 * Game/EnemyMoveCommon.cpp; only the members read here are mapped). */
struct UnkLine_80020A20 {
    u8 field_0x0[0x18];
};

struct UnkLineObject_8005B5A0 {
    /* 0x00 */ u8 field_0x0[0xC];
    /* 0x0C */ u32 m_flags;
};

struct UnkColliContactNode_800587A0 {
    /* 0x00 */ u8 field_0x0[0x14];
    /* 0x14 */ void *m_object;
};

struct UnkLineHits_80021FE4 {
    /* 0x0 */ u8 field_0x0[0x8];
    /* 0x8 */ UnkColliContactNode_800587A0 *m_first;
};

/* What the model's update callback receives (layout from its body; names inferred). */
struct UnkModelCallbackArg {
    /* 0x0 */ void *m_node;
    /* 0x4 */ BkNinja::BkNinja *m_owner;
};

extern "C" {
/* The enemies' shared helpers (behaviour inferred). */
void fn_8019AA40(Enemy *enemy, u16 sound, int arg);
void fn_8019A954(Enemy *enemy, u16 sound, int arg);
void fn_8019AAD4(Enemy *enemy, float value);
void fn_8017CA2C(Enemy *enemy);
/* The enemy's home position and area radius, and whether its area holds a point (see
 * Game/SoldierCommonBase.cpp and Game/EnemyMoveCommon.cpp). */
Vec3f *fn_80198BD8(Enemy *enemy);
float fn_8018346C(Enemy *enemy);
bool fn_801A6944(UnkEnemyArea_8018A374 *area, const Vec3f *point);
/* Resources and models (see BkSoldier.cpp). */
void *fn_80012684(void *loader, int slot, int kind);
void *fn_80012608(void *loader, int slot, int kind);
boost::shared_ptr<EnemyRender> fn_802CDB08(int slot, const EnemyMotionTable &table, void *model,
                                           void *motion);
void *fn_802CD420(EnemyRender *render);
void fn_80415808(void *motion, int id, void (*callback)(void *), void *data);
void *fn_801A46E4(void *model, int part);
/* An effect at a matrix (see BkWorm.cpp). */
boost::weak_ptr<EffectElement> fn_801A1ED8(const Mtx34f *matrix, int id);
/* The vehicles (see BkSoldier.cpp). */
bool fn_801B4B5C(int kind);
UnkVehicleSlot_801B48D4 *fn_801B48D4(int kind);
void fn_801B5D1C(UnkVehicleSlot_801B48D4 *slot);
void fn_801B5D24(UnkVehicleSlot_801B48D4 *slot, const Vec3f *position, const Vec3f *rotation);
void fn_801B67CC(Vehicle::TVehicleBase *vehicle, const boost::weak_ptr<CharInfo> &rider);
void fn_801B6718(Vehicle::TVehicleBase *vehicle, const boost::weak_ptr<CharInfo> &rider);
void fn_801B6944(Vehicle::TVehicleBase *vehicle);
void fn_801B65DC(Vehicle::TVehicleBase *vehicle, Vec3f *position, Vec3f *rotation);
void fn_800C9938(int arg);
/* The collision world and a line test against it (see Game/EnemyMoveCommon.cpp). */
void *fn_80020D40(void);
void *fn_80020A20(UnkLine_80020A20 *line, const Vec3f *start, const Vec3f *end);
UnkLineHits_80021FE4 *fn_80021FE4(void *world, const UnkLine_80020A20 *line, int arg2, int kind,
                                  bool (*filter)(const UnkLineObject_8005B5A0 *object), void *data);
void fn_80026FB4(UnkLineHits_80021FE4 *hits, int arg1);
const Vec3f *fn_8001E9BC(void *object);
/* A model node's matrix, and setting it (see BKTank.cpp). */
void fn_80016370(Mtx34f *matrix, void *node);
void fn_800163F4(void *node, const Mtx34f *matrix);
/* Matrix and vector helpers (see BkSoldier.cpp and BirdBase.cpp). */
Vec3f fn_8040FB08(const Mtx34f *matrix); /* translation */
void fn_8040FA20(Mtx34f *matrix, const Vec3f *translation);
Mtx34f fn_8040BEF8(const Mtx34f *matrix);              /* inverse */
Mtx34f fn_8001E044(const Mtx34f *a, const Mtx34f *b); /* product */
void fn_8040BAC4(Mtx34f *matrix, const Mtx34f *other);
Vec3f fn_8040B67C(const Mtx34f *matrix, const Vec3f *point);
Vec2f fn_8040F760(const Vec3f *direction);
float fn_8040F314(float value, float target, float step);
Vec3f fn_8040CEE8(const Vec3f *v);
Vec3f fn_8040D0AC(const Vec3f *v, float angle);
Vec3f fn_8040CDD0(const Vec3f *a, const Vec3f *b); /* a + b */
Vec3f fn_80016284(const Vec3f *v);
void fn_8040CFF8(Vec3f *vector, const Vec3f *value); /* += */
void fn_8040CE04(Vec3f *vector, const Vec3f *value); /* -= */
float fn_8040CE74(Vec3f *vector);
void fn_8040CE38(Vec3f *vector, float scale);
void fn_8040E988(Vec2f *angles);
}

extern const float lbl_805FAD38; /* pi */

/* The vector type's out-of-line assignment (see JumpBoard.cpp). */
inline void AssignVector(Vec3f &vector, const Vec3f &value) {
    fn_800091D0(&vector, &value);
}

/* A motion table built in place (not original; as in BkWorm.cpp). */
struct MotionTable : public EnemyMotionTable {
    MotionTable(const EnemyMotionEntry *e, s32 c) {
        entries = e;
        count = c;
    }
};

/* Fires the weapon once (name inferred; as GunRobot.cpp's EnemyWeapon::Fire). */
inline void Fire(EnemyWeapon *weapon) {
    *reinterpret_cast<u8 *>(weapon) = 0;
    fn_801AB3F0(weapon);
}

/* value moved toward target in a tenth of a second (name inferred). */
inline float FadeTo(float value, float target, float dt) {
    float result;
    if (target > value) {
        result = value + dt / 0.1f;
        if (result > target) {
            result = target;
        }
    } else if (target < value) {
        result = value - dt / 0.1f;
        if (result < target) {
            result = target;
        }
    } else {
        result = target;
    }
    return result;
}

/* EnemyStatFunc's slot 0x90 with the flag it takes (see ENEMY_STAT_0x90_ARG in Game/Enemy.h). This
 * unit's vtables need the slot's argument-less symbol, so the call goes through this stand-in of
 * the interface (not original; its slots are declared only, so no vtable is emitted). */
class UnkStatusFlags_0x90 {
  public:
    virtual void vf_0x08();
    virtual void vf_0x0C();
    virtual void vf_0x10();
    virtual void vf_0x14();
    virtual void vf_0x18();
    virtual void vf_0x1C();
    virtual void vf_0x20();
    virtual void vf_0x24();
    virtual void vf_0x28();
    virtual void vf_0x2C();
    virtual void vf_0x30();
    virtual void vf_0x34();
    virtual void vf_0x38();
    virtual void vf_0x3C();
    virtual void vf_0x40();
    virtual void vf_0x44();
    virtual void vf_0x48();
    virtual void vf_0x4C();
    virtual void vf_0x50();
    virtual void vf_0x54();
    virtual void vf_0x58();
    virtual void vf_0x5C();
    virtual void vf_0x60();
    virtual void vf_0x64();
    virtual void vf_0x68();
    virtual void vf_0x6C();
    virtual void vf_0x70();
    virtual void vf_0x74();
    virtual void vf_0x78();
    virtual void vf_0x7C();
    virtual void vf_0x80();
    virtual void vf_0x84();
    virtual void vf_0x88();
    virtual void vf_0x8C();
    virtual void vfStat_0x90(bool value);
};

inline UnkStatusFlags_0x90 &StatusFlags(EnemyStatFunc &status) {
    return reinterpret_cast<UnkStatusFlags_0x90 &>(status);
}

/* point raised by height, in storage of the caller's frame (name inferred). fn_8040CDD0 is called
 * through its result pointer and the vectors are plain storage, so that the function is inlined
 * and each call's vectors come out in retail's order; this is not original. */
typedef void (*VectorAddFn)(Vec3f *out, const Vec3f *a, const Vec3f *b);

inline const Vec3f *Raise(const Vec3f *point, float height) {
    float offset[3];
    float result[3];
    offset[0] = 0.0f;
    offset[1] = height;
    offset[2] = 0.0f;
    ((VectorAddFn)fn_8040CDD0)(reinterpret_cast<Vec3f *>(result), point,
                               reinterpret_cast<Vec3f *>(offset));
    return reinterpret_cast<Vec3f *>(result);
}

/* pow for floats, as MSL's float overload (name inferred). */
inline float PowF(float x, float y) {
    return pow(x, y);
}

/* Three floats copied as a plain struct (not original). */
struct Vec3fWords {
    float x;
    float y;
    float z;
};

/* The motion table (steps in .sdata2 and .rodata). */
static const MotionSequenceStep lbl_805F87B8[] = {{1, -1, -1, 30, 1}};
static const MotionSequenceStep lbl_805F87C0[] = {{1, -1, -1, 0, 1}};
static const MotionSequenceStep lbl_805F87C8[] = {{2, -1, -1, 0, 1}};
static const MotionSequenceStep lbl_805F87D0[] = {{3, -1, -1, 0, 1}};
static const MotionSequenceStep lbl_805F87D8[] = {{4, -1, -1, 0, 1}};
static const MotionSequenceStep lbl_805F87E0[] = {{12, -1, -1, 30, 1}};
static const MotionSequenceStep lbl_804FCE58[] = {{8, -1, -1, 30, 2}, {9, -1, -1, 0, 0}};
static const MotionSequenceStep lbl_804FCE68[] = {
    {14, 10, -1, 20, 0}, {14, 10, -1, 30, 0}, {13, -1, -1, 0, 0}};
static const MotionSequenceStep lbl_804FCE80[] = {
    {6, -1, 14, 0, 0}, {6, 14, 15, 0, 2}, {6, 15, -1, 0, 0}, {10, -1, -1, 0, 0}};
static const MotionSequenceStep lbl_804FCEA0[] = {
    {7, -1, 14, 0, 0}, {7, 14, 15, 0, 2}, {7, 15, -1, 0, 0}, {11, -1, -1, 0, 0}};
static const MotionSequenceStep lbl_804FCEC0[] = {
    {6, -1, 14, 0, 0}, {6, 14, 15, 0, 2}, {6, 15, -1, 0, 0}};
static const MotionSequenceStep lbl_804FCED8[] = {
    {7, -1, 14, 0, 0}, {7, 14, 15, 0, 2}, {7, 15, -1, 0, 0}};
static const MotionSequence lbl_804FCEF0[] = {
    {lbl_805F87B8, 1, -3}, {lbl_805F87C0, 1, -3}, {lbl_805F87C8, 1, -3}, {lbl_805F87D0, 1, -3},
    {lbl_805F87D8, 1, -3}, {lbl_805F87E0, 1, -3}, {lbl_804FCE58, 2, -3}, {lbl_804FCE68, 3, -3},
    {lbl_804FCE80, 4, -3}, {lbl_804FCEA0, 4, -3}, {lbl_804FCEC0, 3, -3}, {lbl_804FCED8, 3, -3},
};
static const MotionSequenceTable lbl_805F87E8 = {lbl_804FCEF0, 12};

/* The model's motion table. */
static const EnemyMotionEntry lbl_805F87F0[] = {{3, -1}};

/* The motions the WaitType parameters select. */
static const s32 lbl_804FCF50[] = {1, 2, 3, 4};

namespace BkNinja {

const char *BkNinjaTask::s_taskName = "BkNinja";

void BkNinja::OnModelUpdate(void *data) {
    UnkModelCallbackArg *arg = static_cast<UnkModelCallbackArg *>(data);
    BkNinja *self = arg->m_owner;
    Mtx34f matrix;
    fn_80016370(&matrix, arg->m_node);
    Vec3f position;
    fn_800091EC(&position, &(const Vec3f &)fn_8040FB08(&matrix));
    Vec2f angles(0.0f, 0.0f);
    Matrix34 rotation = MakeRotation(self->GetRotation(), 0);
    if (self->IsAttacking()) {
        Vec3f target;
        fn_800091EC(&target, &(const Vec3f &)self->m_ctrl->vfCtrl_0x1C(-1.0f));
        fn_8040FA20(&rotation, &position);
        Vec3f local;
        fn_800091EC(&local, &(const Vec3f &)fn_8040B67C(&(const Mtx34f &)fn_8040BEF8(&rotation),
                                                        &target));
        Vec3f direction;
        fn_800091EC(&direction, &(const Vec3f &)fn_8040CEE8(&local));
        angles = fn_8040F760(&direction);
        float limit = 30.0f * (lbl_805FAD38 / 180.0f);
        if (angles.x > limit) {
            angles.x = limit;
        } else if (angles.x < -limit) {
            angles.x = -limit;
        }
        limit = 10.0f * (lbl_805FAD38 / 180.0f);
        if (angles.y > limit) {
            angles.y = limit;
        } else if (angles.y < -limit) {
            angles.y = -limit;
        }
    }
    self->m_aim.x = fn_8040F314(self->m_aim.x, angles.x, 0.2f * self->GetFrameTime2());
    self->m_aim.y = fn_8040F314(self->m_aim.y, angles.y, 0.2f * self->GetFrameTime2());
    Mtx34f turn;
    fn_8040B6F8(&turn);
    fn_8040B848(&turn, self->m_aim.y + self->GetRotation()->y);
    fn_8040B884(&turn, self->m_aim.x);
    fn_8040BAC4(&turn, &(const Mtx34f &)fn_8040BEF8(&rotation));
    fn_8040C148(&matrix, &(const Mtx34f &)fn_8001E044(&turn, &matrix));
    fn_8040FA20(&matrix, &position);
    fn_800163F4(arg->m_node, &matrix);
}

BkNinja::BkNinja() : EnemyWalkerCommon(11), m_weapon(0), m_aim(0.0f, 0.0f) {
    m_warpIndex = 0;
    m_hidden = 0;
    m_enemy = this;
    vfPara_0x40(reinterpret_cast<int>(&lbl_805F87E8));
    m_vanishing = 0;
    fn_8040B6F8(&m_vanishMatrix);
    m_vanish = 0.0f;
    m_voiceTime = fn_8040EC80(8.0f, 12.0f);
    fn_8040E988(&m_aim);
}

BkNinja::~BkNinja() {
    if (!m_enemy->m_set->vfSet_0x08()) {
        ReleaseVehicle();
    }
    if (m_weapon != 0) {
        delete m_weapon;
        m_weapon = 0;
    }
}

Vec3f BkNinja::GetWarpPosition(int index) const {
    if (index == 0) {
        return *reinterpret_cast<const Vec3f *>(&m_params.Pos0_X);
    }
    return *reinterpret_cast<const Vec3f *>(&m_params.Pos1_X);
}

Vec3f BkNinja::GetWarpPosition() const {
    return GetWarpPosition(m_warpIndex);
}

float BkNinja::GetWarpAngle(int index) const {
    if (index == 0) {
        return m_params.Pos0_AngY * (lbl_805FAD38 / 180.0f);
    }
    return m_params.Pos1_AngY * (lbl_805FAD38 / 180.0f);
}

float BkNinja::GetWarpAngle() const {
    return GetWarpAngle(m_warpIndex);
}

float BkNinja::GetDisappearTime() {
    if (m_warpIndex == 0) {
        return m_params.Pos0_DisappearTime;
    }
    return m_params.Pos1_DisappearTime;
}

void BkNinja::NextWarpPoint() {
    int index = m_warpIndex;
    index++;
    index %= 2;
    m_warpIndex = index;
}

bool BkNinja::vfStat_0x54() {
    if (m_hidden) {
        return 0;
    }
    return EnemyStatusCommon::vfStat_0x54();
}

float BkNinja::vfPara_0x68() {
    if (m_hidden) {
        return 0.0f;
    }
    return EnemyParamCommon::vfPara_0x68();
}

bool BkNinja::vfStat_0x68() {
    if (m_hidden) {
        return 0;
    }
    return EnemyStatusCommon::vfStat_0x68();
}

void BkNinja::vfSet_0x1C(UnkPlacement_800C9CD0 *placement) {
    vfSet_0x20(placement);
    /* Copied as a plain struct (word loads and stores), unlike Vec3f's own copy. */
    Vec3fWords rotation = *reinterpret_cast<Vec3fWords *>(GetRotation());
    vfDisp_0x38(GetPosition(), reinterpret_cast<Vec3f *>(&rotation));
    m_ctrl->vfCtrl_0x20();
}

/* Whether the line test counts a collider (name inferred). */
static bool IsFloor(const UnkLineObject_8005B5A0 *object) {
    return (object->m_flags & 0x41) != 0;
}

Fn8040CopyVector3 BkNinja::FindWarpPoint(const Vec3f *center, float radius) {
    UnkLine_80020A20 line;
    Vec3f point(0.0f, 0.0f, -radius);
    for (int i = 0; i < 10; i++) {
        AssignVector(point, fn_8040D0AC(&point, fn_8040EC80(0.0f, 2.0f * lbl_805FAD38)));
        fn_8040CFF8(&point, center);
        if (!fn_801A6944(&field_0xA8, &point)) {
            fn_8040CE04(&point, fn_80198BD8(this));
            point.y = 0.0f;
            fn_8040CE74(&point);
            fn_8040CE38(&point, 0.95f * fn_8018346C(this));
            point.x += field_0x48.x;
            point.z += field_0x48.z;
            point.y = center->y;
        }
        fn_80020A20(&line, Raise(&point, 100.0f), Raise(&point, -1000.0f));
        UnkLineHits_80021FE4 *hits = fn_80021FE4(fn_80020D40(), &line, 1, 4, IsFloor, 0);
        if (hits != 0) {
            if (hits->m_first != 0 && hits->m_first->m_object != 0) {
                point.y = fn_8001E9BC(hits->m_first->m_object)->y;
                fn_80026FB4(hits, 1);
                return Fn8040CopyVector3(point);
            }
            fn_80026FB4(hits, 1);
        }
    }
    return Fn8040CopyVector3(field_0x48);
}

void BkNinja::vfSet_0x20(UnkPlacement_800C9CD0 *placement) {
    TEnemySetBase::vfSet_0x20(placement);
    fn_800C9DBC(placement, &m_params, sizeof(m_params));
    if (m_params.AppearType != 0) {
        GetPosition()->x += m_params.Pos0_X;
        GetPosition()->y += m_params.Pos0_Y;
        GetPosition()->z += m_params.Pos0_Z;
        m_params.Pos0_X += field_0x48.x;
        m_params.Pos0_Y += field_0x48.y;
        m_params.Pos0_Z += field_0x48.z;
        m_params.Pos1_X += field_0x48.x;
        m_params.Pos1_Y += field_0x48.y;
        m_params.Pos1_Z += field_0x48.z;
    }
}

void BkNinja::vfStat_0x38() {
    if (m_params.AppearType == 1 && !field_0xA8.Contains(m_ctrl->vfCtrl_0x10())) {
        vfStat_0x3C(true);
        vfStat_0x30(false);
        if (m_weapon != 0) {
            fn_801AA830(m_weapon);
        }
    } else {
        vfStat_0x3C(false);
        vfStat_0x30(true);
        if (m_weapon != 0) {
            fn_801AA868(m_weapon);
        }
    }
}

void BkNinja::vfDisp_0x64() {
    void *loader = fn_800122D4();
    int slot = m_motionBase;
    void *model = fn_80012684(loader, slot, 4);
    void *motion = fn_80012608(loader, slot, 6);
    m_render = fn_802CDB08(slot, MotionTable(lbl_805F87F0, 1), model, motion);
    void *data = fn_802CD420(m_render.get());
    if (data != 0) {
        fn_80415808(data, 0x15, OnModelUpdate, this);
    }
}

void BkNinja::vfDisp_0x08() {
    EnemyDispCommon::vfDisp_0x08();
    if (m_weapon != 0) {
        delete m_weapon;
        m_weapon = 0;
    }
    m_weapon = new EnemyWeapon(fn_8017D014(this), 0x1B);
    if (m_vehicle.expired() && m_params.AppearType == 3 && fn_801B4B5C(7)) {
        UnkVehicleSlot_801B48D4 *slot = fn_801B48D4(7);
        if (slot != 0) {
            fn_800C9938(m_set->vfSet_0x2C());
            fn_801B5D1C(slot);
            fn_801B5D24(slot, GetPosition(), GetRotation());
            m_vehicle = slot->GetVehicle();
            boost::shared_ptr<Vehicle::TVehicleBase> vehicle = m_vehicle.lock();
            if (vehicle) {
                fn_801B67CC(vehicle.get(), fn_8017D014(this));
            }
        }
    }
}

int BkNinja::vfDisp_0x14() {
    if (m_vanish <= 0.0f) {
        return 0;
    }
    return reinterpret_cast<int>(&m_vanishMatrix);
}

void BkNinja::vfDisp_0x10(float dt) {
    if (m_vanishing) {
        m_vanish = FadeTo(m_vanish, 1.0f, dt);
        if (m_vanish >= 1.0f && !vfStat_0x40()) {
            vfStat_0x3C(true);
            if (m_weapon != 0) {
                fn_801AA830(m_weapon);
            }
            if (boost::shared_ptr<Vehicle::TVehicleBase> vehicle = m_vehicle.lock()) {
                vehicle->vf_0x74();
            }
        }
    } else {
        m_vanish = FadeTo(m_vanish, 0.0f, dt);
    }
    if (m_vanish < 1.0f) {
        Mtx34f translation;
        Vec3f center;
        Mtx34f *part = GetPartMatrix(vfPara_0x3C());
        fn_800091EC(&center, &(const Vec3f &)fn_8040FB08(part));
        fn_8040B034(&translation, &center);
        fn_8040C148(&m_vanishMatrix, &translation);
        fn_8040B720(&m_vanishMatrix, 1.5f * m_vanish + 1.0f, 1.0f - PowF(m_vanish, 0.4f),
                    1.5f * m_vanish + 1.0f);
        fn_8040B59C(&m_vanishMatrix, &(const Vec3f &)fn_80016284(&center));
    }
    EnemyWalkerCommon::vfDisp_0x10(dt);
    if (vfStat_0x4C() && !vfStat_0x60()) {
        fn_8019A954(this, 0x606B, 1);
    }
    if (field_0xA8.Contains(m_ctrl->vfCtrl_0x10())) {
        if ((m_voiceTime -= dt) < 0.0f) {
            m_voiceTime = fn_8040EC80(8.0f, 12.0f);
            fn_8019A954(this, 0x6069, 1);
        }
    }
    if (m_weapon != 0) {
        Mtx34f *hand = static_cast<Mtx34f *>(fn_801A46E4(vfDisp_0x1C(), 0x18));
        fn_801AB650(m_weapon, hand);
        fn_801AA8E4(m_weapon, (1.0f - m_vanish) * field_0xEC);
        m_weapon->Tick(dt);
    }
}

float BkNinja::vfStat_0xCC() {
    if (m_params.AppearType == 3) {
        return -1.0f;
    }
    return 0.5f * EnemyStatusCommon::field_0x78;
}

void BkNinja::vfDisp_0x40(EnemyMotionEventArg &arg) {
    EnemyDispCommon::vfDisp_0x40(arg);
    if (m_weapon != 0 && arg.m_type == 0x5000 && arg.m_data->kind == 2) {
        Fire(m_weapon);
    }
}

bool BkNinja::vfDisp_0x44(int *command) {
    if (EnemyDispCommon::vfDisp_0x44(command)) {
        return true;
    }
    switch (*command) {
    case 3:
        fn_8019AA40(this, 0x6011, 1);
        fn_8017CA2C(this);
        break;
    case 27:
        fn_8019AA40(this, 0x6065, 1);
        break;
    }
    return true;
}

float BkNinja::vfPara_0x6C() {
    return 16.0f;
}

int BkNinja::vfPara_0x48() {
    return 0;
}

int BkNinja::vfPara_0x4C() {
    return m_vehicle.expired() ? 5 : 0;
}

int BkNinja::vfPara_0x50() {
    return 6;
}

int BkNinja::vfPara_0x54() {
    return vfPara_0x48();
}

int BkNinja::vfPara_0x58() {
    return 7;
}

int BkNinja::vfPara_0x5C() {
    return vfStat_0x74() == 0 ? 8 : 9;
}

int BkNinja::vfPara_0x60() {
    return vfStat_0x74() == 0 ? 10 : 11;
}

int BkNinja::GetWaitMotion() {
    return lbl_804FCF50[m_warpIndex == 0 ? m_params.Pos0_WaitType : m_params.Pos1_WaitType];
}

void BkNinja::vfStat_0x70(AttackCallbackParam &param) {
    EnemyStatusCommon::vfStat_0x70(param);
}

void BkNinja::vfDisp_0x50() {
    Mtx34f *matrix = GetPartMatrix(vfPara_0x3C());
    if (matrix != 0) {
        Mtx34f moved;
        if (!m_vehicle.expired()) {
            Mtx34f offset;
            Vec3f height;
            fn_8040B034(&offset, fn_80008E1C(&height, 0.0f, 7.02f, 0.0f));
            fn_8040C148(&moved, &offset);
            fn_8040BAC4(&moved, matrix);
            matrix = &moved;
        }
        fn_801A1ED8(matrix, 0x1DA);
    }
    fn_8019AA40(this, 0x6067, 1);
}

int BkNinja::vfStat_0xB8() {
    return 0x60;
}

void BkNinja::ReleaseVehicle() {
    if (boost::shared_ptr<Vehicle::TVehicleBase> vehicle = m_vehicle.lock()) {
        fn_801B6944(vehicle.get());
        m_vehicle.reset();
    }
}

void BkNinja::GetOffVehicle() {
    if (boost::shared_ptr<Vehicle::TVehicleBase> vehicle = m_vehicle.lock()) {
        vehicle->OnGetOff();
        fn_801B6718(vehicle.get(), fn_8017D014(this));
        m_vehicle.reset();
        GetRotation()->x = 0.0f;
        GetRotation()->z = 0.0f;
    }
}

void BkNinja::vfStat_0xA0() {
    EnemyStatusCommon::vfStat_0xA0();
    fn_8019A954(this, 0x606C, 1);
    if (m_weapon != 0) {
        float y = GetRotation()->y;
        if (vfStat_0x74() == 1) {
            y = -y;
        }
        Vec3f offset;
        fn_800091EC(&offset, &(const Vec3f &)fn_8040D0AC(&(const Vec3f &)Vec3f(0.0f, 20.0f, 10.0f), y));
        fn_801AA8A0(m_weapon, &offset);
        delete m_weapon;
        m_weapon = 0;
    }
}

void BkNinja::vfStat_0xC0() {
    if (!m_vehicle.expired()) {
        GetOffVehicle();
    }
}

void BkNinja::vfMove_0x0C(float dt) {
    if (!m_vehicle.expired()) {
        if (boost::shared_ptr<Vehicle::TVehicleBase> vehicle = m_vehicle.lock()) {
            Vec3f seat;
            Vec3f rotation(0.0f, 0.0f, 0.0f);
            fn_801B65DC(vehicle.get(), &seat, &rotation);
            GetPosition()->y = seat.y;
            vehicle->SetPlacement(GetPosition(), GetRotation());
            vehicle->GetSeat(GetPosition(), GetRotation());
            return;
        }
    }
    EnemyMoveCommon::vfMove_0x0C(dt);
}

void BkNinja::Appear() {
    m_vanishing = 0;
    vfStat_0x3C(false);
    fn_8019AAD4(this, 1.0f);
    vfStat_0x8C(true);
    StatusFlags(*this).vfStat_0x90(true);
    if (m_weapon != 0) {
        fn_801AA868(m_weapon);
    }
    if (boost::shared_ptr<Vehicle::TVehicleBase> vehicle = m_vehicle.lock()) {
        vehicle->vf_0x78();
    }
    fn_8019AA40(this, 0x6063, 1);
}

void BkNinja::Disappear() {
    m_vanishing = 1;
    vfStat_0x8C(false);
    StatusFlags(*this).vfStat_0x90(false);
    fn_8019AA40(this, 0x6068, 1);
}

bool BkNinja::IsAttacking() {
    if (vfStat_0x24() == vfPara_0x58() && field_0xA8.Contains(m_ctrl->vfCtrl_0x10())) {
        return true;
    }
    return false;
}

BkNinjaTask::BkNinjaTask(Task *parent, UnkPlacement_800C9CD0 *placement)
    : EnemyTemplate<BkNinja, BkNinjaAI>(parent, placement) {
    m_enemy = this;
    m_name = s_taskName;
}

BkNinjaTask::~BkNinjaTask() {}

} // namespace BkNinja

extern "C" const char *fn_802ABF6C(UnkLabelQuery_800CBC20 *query) {
    return "";
}

namespace BkNinja {

void BkNinjaTask::LoadFiles() {
    void *loader = fn_800122D4();
    fn_8001299C(loader, 0x12, new ResourceOneFile("enemy/BkNinjaData.one", false));
}

void BkNinjaTask::DropFiles() {
    fn_800127F8(fn_800122D4(), 0x12);
}

void BkNinjaTask::Instantiate(UnkPlacement_800C9CD0 *placement) {
    new BkNinjaTask(fn_801EB304(fn_8001C4F8(), 0xE), placement);
}

} // namespace BkNinja

static GadgetParamDesc lbl_80554780[] = {
    {4, "MoveRange", 0, 0, 0, 200.0f, 0.0f, 65536.0f, fn_800CBC20, 0},
    {4, "SearchRange", 0, 0, 0, 0.0f, 0.0f, 65536.0f, fn_800CBC20, 0},
    {4, "SearchAngle", 0, 0, 0, 0.0f, -360.0f, 360.0f, fn_800CBC20, 0},
    {4, "SearchWidth", 0, 0, 0, 100.0f, 0.0f, 65536.0f, fn_800CBC20, 0},
    {4, "SearchHeight", 0, 0, 0, 100.0f, 0.0f, 65536.0f, fn_800CBC20, 0},
    {4, "SearchHeightOffset", 0, 0, 0, 0.0f, -65536.0f, 65536.0f, fn_800CBC20, 0},
    {4, "MoveSpeedRatio", 0, 0, 0, 1.0f, 0.0f, 65536.0f, fn_800CBC20, 0},
    {1, "AppearType", 0, 0, 3, 0.0f, 0.0f, 0.0f, fn_802ABF6C, 0},
    {1, "ShootCount", 1, 1, 0x10000, 0.0f, 0.0f, 0.0f, fn_800CBC20, 0},
    {4, "AttackInterval", 0, 0, 0, 1.0f, 0.0f, 65536.0f, fn_800CBC20, 0},
    {4, "WaitInterval", 0, 0, 0, 1.0f, 0.0f, 65536.0f, fn_802ABF6C, 0},
    {4, "Pos0_X", 0, 0, 0, 0.0f, -65536.0f, 65536.0f, fn_802ABF6C, 0},
    {4, "Pos0_Y", 0, 0, 0, 0.0f, -65536.0f, 65536.0f, fn_802ABF6C, 0},
    {4, "Pos0_Z", 0, 0, 0, 0.0f, -65536.0f, 65536.0f, fn_802ABF6C, 0},
    {4, "Pos0_AngY", 0, 0, 0, 0.0f, -180.0f, 180.0f, fn_802ABF6C, 0},
    {1, "Pos0_WaitType", 0, 0, 3, 0.0f, 0.0f, 0.0f, fn_802ABF6C, 0},
    {4, "Pos0_DisappearTime", 0, 0, 0, 0.0f, 0.0f, 65536.0f, fn_802ABF6C, 0},
    {4, "Pos1_X", 0, 0, 0, 0.0f, -65536.0f, 65536.0f, fn_802ABF6C, 0},
    {4, "Pos1_Y", 0, 0, 0, 0.0f, -65536.0f, 65536.0f, fn_802ABF6C, 0},
    {4, "Pos1_Z", 0, 0, 0, 0.0f, -65536.0f, 65536.0f, fn_802ABF6C, 0},
    {4, "Pos1_AngY", 0, 0, 0, 0.0f, -180.0f, 180.0f, fn_802ABF6C, 0},
    {1, "Pos1_WaitType", 0, 0, 3, 0.0f, 0.0f, 0.0f, fn_802ABF6C, 0},
    {4, "Pos1_DisappearTime", 0, 0, 0, 0.0f, 0.0f, 65536.0f, fn_802ABF6C, 0},
    {0},
};

GadgetClassDesc lbl_80554B40 = {
    "BK_NINJA", BkNinja::BkNinjaTask::LoadFiles, BkNinja::BkNinjaTask::DropFiles,
    reinterpret_cast<void (*)(UnkPlacement_800C9CD0 *)>(BkNinja::BkNinjaTask::Instantiate), 0, 0,
    0x93, 2, 0, 5, lbl_80554780,
};

/* Not called anywhere: the linker strips it. Retail pools 0 ahead of the other float constants
 * in .sdata2; this function, emitted first, makes the compiler pool it there (as in BkGiant.cpp;
 * the name is a placeholder). */
static float UnkConstantUser(float x) {
    x *= 0.0f;
    return x;
}
