/* The gun of a vehicle (file and class names unknown): the vehicles of type 6 (fn_801B6938) keep
 * one (UnkVehicleGun_80347A18, which keeps its state in UnkVehicleGunImpl_8034808C). It loads a
 * model with four barrels on two slides, turns it toward the nearest target in front of it and,
 * while firing, shoots weapon type 4 from each barrel in turn, kicking the barrel's slide back and
 * rumbling the rider's pad. All names are inferred. Definitions are in reverse address order
 * (deferred inlining emits functions last-defined first).
 *
 * NonMatching: all functions but MoveSlide match, and all data matches. MoveSlide (0x80348404)
 * differs in two loads: retail loads the slide's dt before the constant it multiplies (fmadds
 * f3=dt, f2=constant); GC/2.7 loads the constant first whatever the operand order. The .sdata2
 * pool also differs at link: retail places 0.5 (0x805FA324) before MoveSlide's -50, -2 and 15,
 * GC/2.7 after them, next to FindAim's other constants. */
#define BOOST_SP_RESET_POINTER
#include "Game/CharInfo.h"
#include "Game/MathConstants.h"
#include "Game/MathTypes.h"
#include "Game/TVehicleBase.h"
#include "Game/TargetSearchList.h"
#include "Game/Vibration.h"
#include "Game/WeaponBullet.h"
#include "boost/scoped_ptr.hpp"
#include "boost/shared_ptr.hpp"
#include "boost/weak_ptr.hpp"
#include <dolphin/types.h>
#include "size_assert.h"

using Vehicle::TVehicleBase;

struct _stMtnPlayWork;

extern "C" {
typedef struct RpClump RpClump;
}

/* What a motion passes to its node callbacks (as in Game/BossCallbackContainer.cpp). */
struct _stMtnCallbackParam {
    /* 0x0 */ void *m_frame;
    /* 0x4 */ void *m_data;
};

/* An object that keeps a clump in step with a motion (class name unknown; as in
 * Game/BlackDoomCtrlUnit.cpp). Its first word is the clump. */
class UnkClumpUpdater_80015A7C {
  public:
    UnkClumpUpdater_80015A7C(RpClump *clump, _stMtnPlayWork *motion);
    ~UnkClumpUpdater_80015A7C();

    /* 0x00 */ RpClump *m_clump;
    /* 0x04 */ u8 field_0x4[0x1C - 0x4];
};

/* A clump's colour fade (struct name unknown; as in Game/BlackDoomCtrlUnit.cpp). */
struct UnkClumpFade_80027D5C {
    /* 0x00 */ float m_color[4];
    /* 0x10 */ float m_values[5];
    /* 0x24 */ u8 m_mode;
    /* 0x28 */ float field_0x28;

    /* Sets the first two values to 1 (name and meaning inferred). */
    void ResetValues() {
        for (int i = 0; i < 2; i++) {
            m_values[i] = 1.0f;
        }
    }
};
SIZE_ASSERT(UnkClumpFade_80027D5C, 0x2C);

/* The stage (class name unknown; fn_8003A840 returns it): the world of each of its areas (as in
 * Game/BossCtrlUnit.cpp). */
class UnkStage_8003A840 {
  public:
    void *GetWorld(int area);
};

/* Target (class name from RTTI; as in Game/PlayerShadowWeaponGun.cpp): a point other objects can
 * aim at. The float at 0x20 is how near one has to be to aim at it (negative: from anywhere;
 * meaning inferred). */
class Target {
  public:
    virtual Vec3f GetPosition(const Vec3f &from);
    virtual ~Target();

    /* 0x04 */ u8 field_0x4[0x20 - 0x4];
    /* 0x20 */ float m_range;
};

/* The game state (struct name unknown; as far as this unit reads it). */
struct UnkGameState_8057D6A0 {
    /* 0x000 */ u8 field_0x0[0x88];
    /* 0x088 */ u32 field_0x88; /* bit 0: the camera is the rider's own (meaning inferred) */
    /* 0x08C */ u8 field_0x8C[0x150 - 0x8C];
};
SIZE_ASSERT(UnkGameState_8057D6A0, 0x150);

extern UnkGameState_8057D6A0 lbl_8057D6A0;
/* Set while the camera is free (meaning inferred; see Game/CameraManagerUnit.cpp). */
extern u8 lbl_805EE1DC;

extern "C" {
/* A 32-bit flag word: fn_80014E04 constructs it cleared, fn_80014E1C sets bit, fn_8006127C clears
 * it and fn_80014E34 tests it (as in Game/BAMine.cpp). */
void fn_80014E04(u32 *flags);
void fn_80014E1C(u32 *flags, int bit);
void fn_8006127C(u32 *flags, int bit);
bool fn_80014E34(u32 *flags, int bit);

/* The resource manager, a motion of a slot and a model of a slot (see
 * Game/DiablonPartsModelBase.cpp). */
void *fn_800122D4(void);
void *fn_80012684(void *manager, s32 slot, int anim);
RpClump *fn_8001277C(void *manager, s32 slot, int model);
/* The stage, the area a clump's frame lies in. */
UnkStage_8003A840 *fn_8003A840(void);
void *fn_80169364(RpClump *clump);
int fn_80169350(void *frame);
/* RpClumpClone, the clump's set-up, its fade, showing and hiding it (behaviour inferred), and
 * RpClumpDestroy. */
RpClump *fn_80457090(RpClump *clump);
void fn_80038C84(RpClump *clump);
UnkClumpFade_80027D5C *fn_80027D5C(UnkClumpFade_80027D5C *fade);
void fn_80038198(RpClump *clump, UnkClumpFade_80027D5C *fade);
void fn_80038F94(RpClump *clump);
void fn_80038FC0(RpClump *clump);
void fn_80457468(RpClump *clump);
/* The world a clump is in, and taking it out of and adding it to a world. */
void *fn_8046256C(RpClump *clump);
void fn_804623EC(void *world, RpClump *clump);
void fn_804622FC(void *world, RpClump *clump);
/* The motion player: creating and freeing one, a node's callback, placing it under a matrix and
 * running it, and the clump updater's step (see Game/DiablonPartsModelBase.cpp). */
_stMtnPlayWork *fn_8041B908(void *anim, int arg1);
void fn_8041B884(_stMtnPlayWork *work);
void fn_80415808(_stMtnPlayWork *work, int id, void (*callback)(_stMtnCallbackParam *), void *data);
void fn_804172D8(_stMtnPlayWork *work, const Mtx34f *matrix);
void fn_80415EA4(_stMtnPlayWork *work);
void fn_800159AC(UnkClumpUpdater_80015A7C *updater);
/* A frame's matrix, read and written (see Game/BlackBullCtrlUnit.cpp). */
void fn_80016370(Mtx34f *matrix, void *frame);
void fn_800163F4(void *frame, const Mtx34f *matrix);

/* Shots and muzzle flashes of weapon type, and the range of weapon type (see
 * Game/HeavyDogCtrlUnit.cpp and Game/WeaponSpecialSatelliteLaser.cpp). */
void fn_80051C34(int type, const Weapon::ShotInfo *info);
void fn_8005B84C(int type, const Mtx34f *matrix);
float fn_80051AC8(int type);
/* Plays a rumble pattern on a player's pad (Game/VibrationRequest.cpp). */
boost::weak_ptr<Vibration::RequestHandle> fn_8034DF48(int pattern, int player);

/* The smaller of a and b, and whether a value is zero (see Game/BirdBase.cpp). */
const float &fn_800704AC(const float &a, const float &b);
bool fn_8000A950(float value);
/* Math (behaviour inferred; see Game/BlackBullCtrlUnit.cpp). */
float fn_80015D68(float value); /* arc cosine */
Mtx34f fn_8040BEF8(const Mtx34f *matrix); /* inverse */
Vec3f fn_8040B67C(const Mtx34f *matrix, const Vec3f *point);
Vec3f fn_8040B618(const Mtx34f *matrix, const Vec3f *vector);
Vec3f fn_8040FB08(const Mtx34f *matrix); /* the translation */
Mtx34f fn_8040FD8C(const Vec3f *axis, float angle); /* a rotation about axis */
Vec3f fn_8040F7F8(const Vec3f *direction); /* the angles of a direction (pitch, yaw, 0) */
void fn_8040CE60(Vec3f *vector); /* sets the vector to zero */
float fn_8040CE74(Vec3f *vector); /* normalizes in place */
float fn_8040CF90(const Vec3f *a, const Vec3f *b); /* a . b */
float fn_8040D180(const Vec3f *a, const Vec3f *b); /* the squared distance */
Vec3f fn_8040CEE8(const Vec3f *vector); /* normalized */
Vec3f fn_8040CFB8(const Vec3f *a, const Vec3f *b); /* a x b */
void fn_8040B848(Mtx34f *m, float radians);
void fn_8040B884(Mtx34f *m, float radians);
void fn_8040B59C(Mtx34f *m, const Vec3f *offset);
void fn_8040C148(Mtx34f *m, const Mtx34f *from);
/* Narrowing a target list to what lies in front of position (behaviour inferred). */
void fn_801E81B0(TargetSearchList *list, const Vec3f *position);
}

inline const Vec3f &Ref(const Vec3f &vector) {
    return vector;
}

inline const Mtx34f &RefM(const Mtx34f &matrix) {
    return matrix;
}

/* A vector returned through the out-of-line copy constructor (fn_800091EC; as in
 * Game/VehicleColliMove.cpp). */
struct UnkCopyVector3_800091EC {
    UnkCopyVector3_800091EC(const Vec3f &v) {
        fn_800091EC(reinterpret_cast<Vec3f *>(this), &v);
    }
    UnkCopyVector3_800091EC(const UnkCopyVector3_800091EC &v) {
        fn_800091EC(reinterpret_cast<Vec3f *>(this), reinterpret_cast<const Vec3f *>(&v));
    }

    float x;
    float y;
    float z;
};

inline const Vec3f &Ref(const UnkCopyVector3_800091EC &vector) {
    return reinterpret_cast<const Vec3f &>(vector);
}

/* fn_8040CD74 called as returning a - b by value (see Game/VehicleColliMove.cpp). */
typedef Vec3f (*VectorSubtractFn)(const Vec3f *a, const Vec3f *b);

/* fn_8040CEE8 called as returning such a vector. */
typedef UnkCopyVector3_800091EC (*NormalizeCopyFn)(const Vec3f *vector);

/* Degrees to radians (name inferred). */
inline float ToRadian(float degrees) {
    return degrees * (lbl_805FAD38 / 180.0f);
}

/* Puts clump in world (name inferred; as in Game/BlackDoomCtrlUnit.cpp). */
inline void SetClumpWorld(RpClump *clump, void *world) {
    void *current = fn_8046256C(clump);
    if (current != world) {
        if (current != 0) {
            fn_804623EC(current, clump);
        }
        fn_804622FC(world, clump);
    }
}

/* How fast the gun turns (name inferred). */
static float s_turnRate = ToRadian(300.0f);

/* The flags of the gun (names inferred). */
enum {
    kGunAim = 0,    /* turn toward targets */
    kGunFire = 1,   /* firing */
    kGunHidden = 2, /* the model is hidden */
    kGunCamera = 3, /* hidden while the camera is the rider's own */
};

/* A slide two of the barrels sit on: it kicks back when one of them fires and creeps forward
 * again (struct name unknown). */
struct UnkGunSlide {
    UnkGunSlide();

    /* 0x00 */ Vec3f m_offset;
    /* 0x0C */ s32 m_kick; /* kicking back */
    /* 0x10 */ float m_dt;
};
SIZE_ASSERT(UnkGunSlide, 0x14);

/* A barrel: its node's matrix, where its muzzle is and where it points (struct name unknown). */
struct UnkGunBarrel {
    UnkGunBarrel();

    /* 0x00 */ Mtx34f m_matrix;
    /* 0x30 */ Vec3f m_position;
    /* 0x3C */ Vec3f m_direction;
    /* 0x48 */ UnkGunSlide *m_slide;
};
SIZE_ASSERT(UnkGunBarrel, 0x4C);

/* The direction the gun points and the one it turns toward (struct name unknown). */
struct UnkGunAim {
    UnkGunAim();

    /* 0x00 */ Vec3f m_direction;
    /* 0x0C */ Vec3f m_target;
    /* 0x18 */ float m_dt;
    /* 0x1C */ u8 m_aim; /* turn toward the nearest target */
};
SIZE_ASSERT(UnkGunAim, 0x20);

/* The gun's state (class name unknown). */
class UnkVehicleGunImpl_8034808C {
  public:
    UnkVehicleGunImpl_8034808C(TVehicleBase *vehicle, const Mtx34f *parent);
    ~UnkVehicleGunImpl_8034808C();
    void Update(float dt);
    void Hide();
    void Show();
    void Load();
    void Release();
    void UpdateModel();

    /* 0x000 */ TVehicleBase *m_vehicle;
    /* 0x004 */ const Mtx34f *m_parent; /* the matrix the gun sits under */
    /* 0x008 */ _stMtnPlayWork *m_motion;
    /* 0x00C */ UnkClumpUpdater_80015A7C *m_updater;
    /* 0x010 */ UnkGunSlide m_slides[2];
    /* 0x038 */ UnkGunBarrel m_barrels[4];
    /* 0x168 */ UnkGunAim m_aim;
    /* 0x188 */ u32 m_flags;
    /* 0x18C */ float m_cooldown;
    /* 0x190 */ s32 m_barrel; /* the barrel that fires next */
    /* 0x194 */ const UnkClumpFade_80027D5C *m_fadeSource;
    /* 0x198 */ UnkClumpFade_80027D5C m_fade;
};
SIZE_ASSERT(UnkVehicleGunImpl_8034808C, 0x1C4);

/* The gun (class name unknown). */
class UnkVehicleGun_80347A18 {
  public:
    UnkVehicleGun_80347A18(TVehicleBase *vehicle, const Mtx34f *parent);
    ~UnkVehicleGun_80347A18();
    void Update(float dt);
    void StartFire();
    void StopFire();
    void StartAim();
    void StopAim();
    void HideWithCamera();
    void ShowAlways();
    void Hide();
    void Show();
    UnkVehicleGunImpl_8034808C *GetImpl();
    void SetFade(const UnkClumpFade_80027D5C *source);

    /* 0x0 */ boost::scoped_ptr<UnkVehicleGunImpl_8034808C> m_impl;
};
SIZE_ASSERT(UnkVehicleGun_80347A18, 0x4);

/* Deletes the state (name inferred; see checked_delete below). */
inline void DeleteImpl(UnkVehicleGunImpl_8034808C *impl) {
    delete impl;
}

/* Retail keeps boost::checked_delete<UnkVehicleGunImpl_8034808C> out of line; this
 * specialization gives that under GC/2.7 (as in Game/VibrationManagerUnit.cpp). */
namespace boost {
template <> inline void checked_delete<UnkVehicleGunImpl_8034808C>(UnkVehicleGunImpl_8034808C *x) {
    DeleteImpl(x);
}
} // namespace boost

/* The direction from the matrix to the nearest target in front of it within reach, or the
 * matrix's forward direction kept level (name inferred). */
static UnkCopyVector3_800091EC FindAim(const Mtx34f *matrix) {
    Vec3f position;
    fn_800091D0(&position, &Ref(fn_8040FB08(matrix)));
    Vec3f forward;
    fn_800091D0(&forward, &Ref(fn_8040B618(matrix, &Vec3f(0.0f, 0.0f, 1.0f))));
    if (forward.y < 0.0f) {
        forward.y = 0.0f;
        fn_8040CE74(&forward);
    }
    boost::shared_ptr<TargetSearchList> list;
    list.reset(fn_801E7930(fn_80060058()));
    fn_801E7FB4(list.get(), &position, 0.5f, &forward);
    Vec3f up;
    fn_800091EC(&up, &Ref(fn_8040B618(matrix, &Vec3f(0.0f, 0.8660254f, 0.5f))));
    fn_801E7FB4(list.get(), &position, 0.0f, &up);
    fn_801E81B0(list.get(), &position);
    fn_801E80C0(list.get(), 0x800);
    boost::weak_ptr<Target> target = fn_801E848C(list.get(), &position);
    if (!target.expired()) {
        boost::shared_ptr<Target> locked = target.lock();
        float range = locked->m_range;
        Vec3f aim;
        fn_800091EC(&aim, &Ref(locked->GetPosition(position)));
        float distance = fn_8040D180(&aim, &position);
        if (range < 0.0f || distance < range * range) {
            float reach = fn_80051AC8(4);
            if (distance < reach * reach) {
                return ((NormalizeCopyFn)fn_8040CEE8)(&Ref(((VectorSubtractFn)fn_8040CD74)(&aim, &position)));
            }
        }
    }
    return forward;
}

/* The slides' node callback: kicks the slide back after a shot and lets it creep forward. */
static void MoveSlide(_stMtnCallbackParam *param) {
    UnkGunSlide *slide = (UnkGunSlide *)param->m_data;
    Mtx34f matrix;
    fn_80016370(&matrix, param->m_frame);
    if (slide->m_kick != 0) {
        slide->m_offset.z += -50.0f * slide->m_dt;
        if (slide->m_offset.z < -2.0f) {
            slide->m_offset.z = -2.0f;
            slide->m_kick = 0;
        }
    } else {
        slide->m_offset.z += 15.0f * slide->m_dt;
        if (slide->m_offset.z > 0.0f) {
            slide->m_offset.z = 0.0f;
        }
    }
    fn_8040B59C(&matrix, &slide->m_offset);
    fn_800163F4(param->m_frame, &matrix);
}

/* The barrels' node callback: keeps the barrel's matrix, muzzle and direction. */
static void ReadBarrel(_stMtnCallbackParam *param) {
    UnkGunBarrel *barrel = (UnkGunBarrel *)param->m_data;
    fn_80016370(&barrel->m_matrix, param->m_frame);
    fn_800091D0(&barrel->m_position, &Ref(fn_8040FB08(&barrel->m_matrix)));
    fn_800091D0(&barrel->m_direction, &Ref(fn_8040B618(&barrel->m_matrix, &Vec3f(0.0f, 0.0f, -1.0f))));
}

/* The gun's node callback: turns the gun toward its target. */
static void TurnGun(_stMtnCallbackParam *param) {
    UnkGunAim *aim = (UnkGunAim *)param->m_data;
    Mtx34f matrix;
    fn_80016370(&matrix, param->m_frame);
    if (aim->m_aim) {
        fn_800091D0(&aim->m_target, &Ref(FindAim(&matrix)));
    } else {
        fn_800091D0(&aim->m_target, &Ref(fn_8040B618(&matrix, &Vec3f(0.0f, 0.0f, 1.0f))));
    }
    Vec3f local;
    fn_800091EC(&local, &Ref(fn_8040B618(&RefM(fn_8040BEF8(&matrix)), &aim->m_target)));
    float angle = fn_80015D68(fn_8040CF90(&aim->m_direction, &local));
    if (!fn_8000A950(angle)) {
        float turn = fn_800704AC(s_turnRate * aim->m_dt / angle, 0.5f);
        Vec3f axis;
        fn_800091EC(&axis, &Ref(fn_8040CEE8(&Ref(fn_8040CFB8(&aim->m_direction, &local)))));
        Mtx34f rotation;
        fn_8040C148(&rotation, &RefM(fn_8040FD8C(&axis, turn * angle)));
        fn_800091D0(&aim->m_direction, &Ref(fn_8040B67C(&rotation, &aim->m_direction)));
        fn_8040CE74(&aim->m_direction);
    } else {
        fn_800091D0(&aim->m_direction, &local);
    }
    Vec3f angles = fn_8040F7F8(&aim->m_direction);
    fn_8040B848(&matrix, angles.y);
    fn_8040B884(&matrix, angles.x);
    fn_800163F4(param->m_frame, &matrix);
}

UnkGunSlide::UnkGunSlide() {
    fn_8040CE60(&m_offset);
    m_kick = 0;
}

UnkGunBarrel::UnkGunBarrel() {}

UnkGunAim::UnkGunAim() {}

UnkVehicleGunImpl_8034808C::UnkVehicleGunImpl_8034808C(TVehicleBase *vehicle, const Mtx34f *parent)
    : m_vehicle(vehicle), m_parent(parent), m_motion(0) {
    fn_80014E04(&m_flags);
    fn_80027D5C(&m_fade);
    Load();
    m_aim.m_aim = 0;
    m_aim.m_dt = 0.0f;
    fn_800091D0(&m_aim.m_direction, &Vec3f(0.0f, 0.0f, 1.0f));
    m_cooldown = 0.0f;
    m_barrel = 0;
    m_fadeSource = 0;
    if (m_updater != 0 && m_updater->m_clump != 0) {
        fn_80038198(m_updater->m_clump, &m_fade);
    }
}

UnkVehicleGunImpl_8034808C::~UnkVehicleGunImpl_8034808C() {
    Release();
}

void UnkVehicleGunImpl_8034808C::Update(float dt) {
    if (fn_80014E34(&m_flags, kGunCamera)) {
        if (lbl_805EE1DC != 0 || !(lbl_8057D6A0.field_0x88 & 1)) {
            Show();
        } else {
            Hide();
        }
    }
    m_slides[0].m_dt = dt;
    m_slides[1].m_dt = dt;
    m_aim.m_dt = dt;
    m_aim.m_aim = fn_80014E34(&m_flags, kGunAim);
    UpdateModel();
    m_cooldown -= dt;
    if (m_cooldown <= 0.0f) {
        m_cooldown = 0.0f;
    }
    if (fn_80014E34(&m_flags, kGunFire) && 0.0f == m_cooldown) {
        m_cooldown = 0.25f;
        {
            Weapon::ShotInfo shot(m_vehicle->GetShooter(), false, m_barrels[m_barrel].m_position,
                                  m_barrels[m_barrel].m_direction);
            fn_80051C34(4, &shot);
            Mtx34f muzzle;
            fn_8040C148(&muzzle, &m_barrels[m_barrel].m_matrix);
            fn_8040B848(&muzzle, ToRadian(180.0f));
            fn_8005B84C(4, &muzzle);
            m_barrels[m_barrel].m_slide->m_kick = 1;
        }
        if (!m_vehicle->GetRider().expired()) {
            fn_8034DF48(2, m_vehicle->GetRider().lock()->field_0x54);
        }
        m_barrel++;
        if (m_barrel >= 4) {
            m_barrel = 0;
        }
    }
    if (m_fadeSource != 0) {
        m_fade = *m_fadeSource;
    }
}

void UnkVehicleGunImpl_8034808C::Hide() {
    if (!fn_80014E34(&m_flags, kGunHidden)) {
        if (m_updater != 0 && m_updater->m_clump != 0) {
            fn_80038FC0(m_updater->m_clump);
        }
        fn_80014E1C(&m_flags, kGunHidden);
    }
}

void UnkVehicleGunImpl_8034808C::Show() {
    if (fn_80014E34(&m_flags, kGunHidden)) {
        if (m_updater != 0 && m_updater->m_clump != 0) {
            fn_80038F94(m_updater->m_clump);
        }
        fn_8006127C(&m_flags, kGunHidden);
    }
}

void UnkVehicleGunImpl_8034808C::Load() {
    void *manager = fn_800122D4();
    UnkStage_8003A840 *stage = fn_8003A840();
    m_motion = fn_8041B908(fn_80012684(manager, 3, 5), 1);
    RpClump *clump = fn_8001277C(manager, 3, 0x18);
    if (m_motion != 0 && clump != 0) {
        clump = fn_80457090(clump);
        fn_80038C84(clump);
        int area = fn_80169350(fn_80169364(clump));
        SetClumpWorld(clump, stage->GetWorld(area));
        m_updater = new UnkClumpUpdater_80015A7C(clump, m_motion);
        fn_80415808(m_motion, 0x6E, MoveSlide, &m_slides[0]);
        fn_80415808(m_motion, 0x78, MoveSlide, &m_slides[1]);
        m_barrels[0].m_slide = &m_slides[0];
        m_barrels[1].m_slide = &m_slides[1];
        m_barrels[2].m_slide = &m_slides[0];
        m_barrels[3].m_slide = &m_slides[1];
        fn_80415808(m_motion, 0x6F, ReadBarrel, &m_barrels[0]);
        fn_80415808(m_motion, 0x79, ReadBarrel, &m_barrels[1]);
        fn_80415808(m_motion, 0x70, ReadBarrel, &m_barrels[2]);
        fn_80415808(m_motion, 0x7A, ReadBarrel, &m_barrels[3]);
        fn_80415808(m_motion, 0x66, TurnGun, &m_aim);
    }
}

void UnkVehicleGunImpl_8034808C::Release() {
    RpClump *clump = 0;
    if (m_updater != 0) {
        clump = m_updater->m_clump;
        delete m_updater;
        m_updater = 0;
    }
    if (m_motion != 0) {
        fn_8041B884(m_motion);
        m_motion = 0;
    }
    if (clump != 0) {
        void *world = fn_8046256C(clump);
        if (world != 0) {
            fn_804623EC(world, clump);
        }
        fn_80457468(clump);
    }
}

void UnkVehicleGunImpl_8034808C::UpdateModel() {
    if (m_motion != 0) {
        Mtx34f matrix;
        fn_800163F4(&matrix, m_parent);
        fn_804172D8(m_motion, &matrix);
        fn_80415EA4(m_motion);
        fn_800159AC(m_updater);
    }
}

UnkVehicleGun_80347A18::UnkVehicleGun_80347A18(TVehicleBase *vehicle, const Mtx34f *parent)
    : m_impl(new UnkVehicleGunImpl_8034808C(vehicle, parent)) {}

UnkVehicleGun_80347A18::~UnkVehicleGun_80347A18() {}

void UnkVehicleGun_80347A18::Update(float dt) {
    m_impl->Update(dt);
}

void UnkVehicleGun_80347A18::StartFire() {
    fn_80014E1C(&m_impl->m_flags, kGunFire);
}

void UnkVehicleGun_80347A18::StopFire() {
    fn_8006127C(&m_impl->m_flags, kGunFire);
}

void UnkVehicleGun_80347A18::StartAim() {
    fn_80014E1C(&m_impl->m_flags, kGunAim);
}

void UnkVehicleGun_80347A18::StopAim() {
    fn_8006127C(&m_impl->m_flags, kGunAim);
}

void UnkVehicleGun_80347A18::HideWithCamera() {
    fn_80014E1C(&m_impl->m_flags, kGunCamera);
}

void UnkVehicleGun_80347A18::ShowAlways() {
    fn_8006127C(&m_impl->m_flags, kGunCamera);
    m_impl->Show();
}

void UnkVehicleGun_80347A18::Hide() {
    m_impl->Hide();
}

void UnkVehicleGun_80347A18::Show() {
    m_impl->Show();
}

UnkVehicleGunImpl_8034808C *UnkVehicleGun_80347A18::GetImpl() {
    return m_impl.get();
}

void UnkVehicleGun_80347A18::SetFade(const UnkClumpFade_80027D5C *source) {
    m_impl->m_fadeSource = source;
    if (m_impl->m_fadeSource == 0) {
        GetImpl()->m_fade.ResetValues();
    }
}
